#include <Arduino.h>
#include "YA_FSM.h"
#include "pins.h"
#include "readvolt.h"
#include "util.h"
#include "timerdelay.h"
#include "btcontroller.h"

TimerDelay btSendDelay(false, 2000);
BluetoothController btController;

void dropLoads();

// Voltage thresholds to turn on and off relay R1 and R2
float voltageTurnOnR1 = 25.0;
float voltageTurnOffAll = 24.5;
float voltageTurnOnR2 = 27.0;
float voltageTurnOffR2 = 26.5;

// Create new FSM
YA_FSM stateMachine;

// State Alias
enum State
{
    START, // start can switch to R1 immediately
    OFF, // off (and all other transitions) have a minimum duration time
    R1_ON,
    R1_AND_R2_ON
};

// Helper for print labels instead integer when state change
const char *const stateName[] PROGMEM = {"START", "OFF", "R1_ON", "R1_AND_R2_ON"};

// Minimum time to remain in each state except start
#define MIN_TIME_MS 1000 * 60 * 1 // 1 minute (milliseconds * seconds * minutes) -> milliseconds

// Output variables
bool r1_on = false;
bool r1_and_r2_on = false;

// Define "on entering" state machine callback function
void onEnter();

// Define "on leaving" state machine callback function
void onExit();

// Setup the State Machine
void setupStateMachine();

void sendVoltage(float voltage);

void setup()
{
    setupPins(); // set up Input/Output
    Serial.begin(115200);
    while (!Serial) {
    } // Needed for native USB port only

    Serial.println(F("Starting the Voltage Controlled Switch...\n"));
    setupStateMachine();
    dropLoads(); // turn off all loads

    // Initialize bluetooth and its delay timer
    btController.init();
    btSendDelay.start();

    btController.sendInfo("Initializing the application ...");
}

void loop()
{
    // Update State Machine
    if (stateMachine.Update())
    {
        Serial.print(F("Active state: "));
        Serial.println(stateMachine.ActiveStateName());
    }

    // Set outputs
    digitalWrite(LOAD_1_LED, r1_on || r1_and_r2_on);
    digitalWrite(LOAD_1_RELAY, r1_on || r1_and_r2_on);
    digitalWrite(LOAD_2_LED, r1_and_r2_on);
    digitalWrite(LOAD_2_RELAY, r1_and_r2_on);

    // Sync the bluetooth controller
    BluetoothData btData = btController.sync();

    if(btData.available()) {
        // Handle data sent over bluetooth
        Serial.print("CMD -> '");
        Serial.print(btData.getCommand());
        Serial.print("\tData -> '");
        Serial.print(btData.getData());
        Serial.println("'");
    }

    delay(500); // Sleep
}

void setOn(int pin)
{
    digitalWrite(pin, HIGH);
}

void setOff(int pin)
{
    digitalWrite(pin, LOW);
}

// Turn ON/OFF the loads
void dropLoads()
{
    setOff(LOAD_1_RELAY);
    setOff(LOAD_1_LED);

    setOff(LOAD_2_RELAY);
    setOff(LOAD_2_LED);

    setOff(LOAD_3_RELAY);
    setOff(LOAD_3_LED);

    setOff(LOAD_4_RELAY);
    setOff(LOAD_4_LED);

    setOff(LOAD_5_RELAY);
    setOff(LOAD_5_LED);

    setOff(LOAD_6_RELAY);
    setOff(LOAD_6_LED);
}

void onEnter()
{
    Serial.print(stateMachine.ActiveStateName());
    Serial.println(F(" enter"));
}

void onExit()
{
    Serial.print(stateMachine.ActiveStateName());
    Serial.println(F(" leave"));
}

void sendVoltage(float voltage)
{
    btController.send("BAT_VOLTS", String(voltage));
}

float readAndSendVoltage(const char * stateName)
{
    float v = readVoltage(stateName);
    sendVoltage(v);
    return v;
}

void setupStateMachine()
{
    // Follow the order of defined enumeration for the state definition (will be used as index)
    // Add States => name, timeout, onEnter cb, onState cb, onLeave cb
    stateMachine.AddState(stateName[START], 0, 0, onEnter, nullptr, onExit);
    stateMachine.AddState(stateName[OFF], 0, MIN_TIME_MS, onEnter, nullptr, onExit);
    stateMachine.AddState(stateName[R1_ON], 0, MIN_TIME_MS, onEnter, nullptr, onExit);
    stateMachine.AddState(stateName[R1_AND_R2_ON], 0, MIN_TIME_MS, onEnter, nullptr, onExit);

    stateMachine.AddAction(R1_ON, YA_FSM::N, r1_on); // N -> while state is active led is ON
    stateMachine.AddAction(R1_AND_R2_ON, YA_FSM::N, r1_and_r2_on);

    // Add transitions with related trigger input callback functions
    stateMachine.AddTransition(START, R1_ON, []() { 
        return readAndSendVoltage(stateMachine.ActiveStateName()) > voltageTurnOnR1; 
    });
    stateMachine.AddTransition(OFF, R1_ON, []() { 
        return readAndSendVoltage(stateMachine.ActiveStateName()) > voltageTurnOnR1;
    });
    stateMachine.AddTransition(R1_ON, OFF, []() { 
        return readAndSendVoltage(stateMachine.ActiveStateName()) < voltageTurnOffAll;
    });
    stateMachine.AddTransition(R1_ON, R1_AND_R2_ON, []() { 
        return readAndSendVoltage(stateMachine.ActiveStateName()) > voltageTurnOnR2;
    });
    stateMachine.AddTransition(R1_AND_R2_ON, OFF, []() { 
        return readAndSendVoltage(stateMachine.ActiveStateName()) < voltageTurnOffAll;
    });
    stateMachine.AddTransition(R1_AND_R2_ON, R1_ON, []() { 
        return readAndSendVoltage(stateMachine.ActiveStateName()) < voltageTurnOffR2;
    });
}
