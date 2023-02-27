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
float voltageTurnOffR1 = 24.0;
float voltageTurnOnR2 = 27.0;
float voltageTurnOffR2 = 26.5;

// Create new FSM
YA_FSM stateMachine;

// State Alias
enum State
{
    START,
    OFF,
    R1_ON,
    R2_ON
};

// Helper for print labels instead integer when state change
const char *const stateName[] PROGMEM = {"START", "OFF", "R1_ON", "R2_ON"};

// Minimum time to remain in each state except start
#define MIN_TIME_MS 1000 * 60 * 1 // 1 minute (milliseconds * seconds * minutes) -> milliseconds

// Output variables
bool r1on = false;
bool r2on = false;

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
    while (!Serial)
    {
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
    digitalWrite(LOAD_1_LED, r1on);
    digitalWrite(LOAD_1_RELAY, r1on);
    digitalWrite(LOAD_2_LED, r2on);
    digitalWrite(LOAD_2_RELAY, r2on);

    // Sync the bluetooth controller
    BluetoothData btData = btController.sync();
    if(btData.available()) {
        // Handle the data sent over bluetooth here
        Serial.print("CMD -> '");
        Serial.print(btData.getCommand());
        Serial.print("\tData -> '");
        Serial.print(btData.getData());
        Serial.println("'");
    }

    delay(1000); // Sleep for a second
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

void setupStateMachine()
{
    // Follow the order of defined enumeration for the state definition (will be used as index)
    // Add States => name, timeout, onEnter cb, onState cb, onLeave cb
    stateMachine.AddState(stateName[START], 0, 0, onEnter, nullptr, onExit);
    stateMachine.AddState(stateName[OFF], 0, MIN_TIME_MS, onEnter, nullptr, onExit);
    stateMachine.AddState(stateName[R1_ON], 0, MIN_TIME_MS, onEnter, nullptr, onExit);
    stateMachine.AddState(stateName[R2_ON], 0, MIN_TIME_MS, onEnter, nullptr, onExit);

    stateMachine.AddAction(R1_ON, YA_FSM::N, r1on); // N -> while state is active led is ON
    stateMachine.AddAction(R2_ON, YA_FSM::N, r2on);

    // Add transitions with related trigger input callback functions
    stateMachine.AddTransition(START, R1_ON, []()
                                { 
                                    float v = readVoltage(stateMachine.ActiveStateName());
                                    sendVoltage(v);
                                    return v > voltageTurnOnR1; 
                                });

    stateMachine.AddTransition(OFF, R1_ON, []()
                                { 
                                    float v = readVoltage(stateMachine.ActiveStateName());
                                    sendVoltage(v);
                                    return v < voltageTurnOffR1; 
                                });
                            
    stateMachine.AddTransition(R1_ON, OFF, []()
                                { 
                                    float v = readVoltage(stateMachine.ActiveStateName());
                                    sendVoltage(v);
                                    return v < voltageTurnOffR1; 
                                });

    stateMachine.AddTransition(R1_ON, R2_ON, []()
                               { 
                                    float v = readVoltage(stateMachine.ActiveStateName());
                                    sendVoltage(v);
                                    return v > voltageTurnOnR2; 
                                });

    stateMachine.AddTransition(R2_ON, R1_ON, []()
                               { 
                                    float v = readVoltage(stateMachine.ActiveStateName());
                                    sendVoltage(v);
                                    return v < voltageTurnOffR2; 
                                });
}

/*
// State machine
// https://stackoverflow.com/questions/1371460/state-machines-tutorials

int entry_state(void);
int foo_state(void);
int bar_state(void);
int exit_state(void);

// array and enum below must be in sync!
int (* state[])(void) = { entry_state, foo_state, bar_state, exit_state};
enum state_codes { entry, foo, bar, end};

enum ret_codes { ok, fail, repeat};
struct transition {
    enum state_codes src_state;
    enum ret_codes   ret_code;
    enum state_codes dst_state;
};

// transitions from end state aren't needed
struct transition state_transitions[] = {
    {entry, ok,     foo},
    {entry, fail,   end},
    {foo,   ok,     bar},
    {foo,   fail,   end},
    {foo,   repeat, foo},
    {bar,   ok,     end},
    {bar,   fail,   end},
    {bar,   repeat, foo}};

#define EXIT_STATE end
#define ENTRY_STATE entry

int main(int argc, char *argv[]) {
    enum state_codes cur_state = ENTRY_STATE;
    enum ret_codes rc;
    int (* state_fun)(void);

    for (;;) {
        state_fun = state[cur_state];
        rc = state_fun();
        if (EXIT_STATE == cur_state)
            break;
        cur_state = lookup_transitions(cur_state, rc);
    }

    return EXIT_SUCCESS;
}
*/