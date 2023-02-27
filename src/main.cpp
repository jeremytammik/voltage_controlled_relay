#include <Arduino.h>
#include "YA_FSM.h"
#include "pins.h"
#include "readvolt.h"
#include "util.h"
#include "timerdelay.h"

<<<<<<< HEAD
// Load 1
#define LOAD_1_RELAY 19 // GPIO19
#define LOAD_1_LED 18   // GPIO18

// Load 2
#define LOAD_2_RELAY 5 // GPIO5
#define LOAD_2_LED 17  // GPIO17

// Load 3
#define LOAD_3_RELAY 16 // GPIO16
#define LOAD_3_LED 4    // GPIO4

// Load 4
#define LOAD_4_RELAY 12 // GPIO12
#define LOAD_4_LED 14   // GPIO14

// Load 5
#define LOAD_5_RELAY 27 // GPIO27
#define LOAD_5_LED 26   // GPIO26

// Load 6
#define LOAD_6_RELAY 25 // GPIO25
#define LOAD_6_LED 33   // GPIO33

// Voltage Input Sensor
#define VOLTAGE_INPUT_SENSOR 36 // GPIO36

// Voltage divider resistors Ra + Rb in Ohm
// Their resistance in parallel should come to ca. 10k, cf.
// https://arduino.stackexchange.com/questions/78768/what-is-the-most-efficient-voltage-divider-for-arduino
// Max ESP32 ADC input voltage is 3.3V, cf.
// https://deepbluembedded.com/esp32-adc-tutorial-read-analog-voltage-arduino/
// 30V converts to 3.289V using Ra = 81.2k = 75k + 6.2k and Rb = 10k, cf.
// https://ohmslawcalculator.com/voltage-divider-calculator
// Vadc = Vbat * Rb/(Ra+Rb)
// Vbat = Vadc * (Ra+Rb)/Rb
#define RA 81200 // Ohms
#define RB 10000 // ohms

// Voltage threshold cut off
#define DROP_ALL_LOADS_VOLTAGE_THRESHOLD 24          // volts
#define DROP_SECONDARY_LOADS_VOLTAGE_THRESHOLD 26.5  // Volts
#define CONNECT_PRIMARY_LOADS_VOLTAGE_THRESHOLD 24   // Volts
#define CONNECT_SECONDARY_LOADS_VOLTAGE_THRESHOLD 27 // Volts

#define DELAY_B4_SWITCHING 1000 * 60 * 1 // 1 minute (milliseconds * seconds * minutes) -> milliseconds
=======
void dropLoads();

// Voltage thresholds to turn on and off relay R1 and R2
float voltageTurnOnR1 = 25.0;
float voltageTurnOffR1 = 24.0;
float voltageTurnOnR2 = 27.0;
float voltageTurnOffR2 = 26.5;

// Create new FSM
YA_FSM stateMachine;

// State Alias
enum State { START, OFF, R1_ON, R2_ON };

// Helper for print labels instead integer when state change
const char * const stateName[] PROGMEM = { "START", "OFF", "R1_ON", "R2_ON" };

// Minimum time to remain in each state except start
#define MIN_TIME_MS 1000*60*1 // 1 minute (milliseconds * seconds * minutes) -> milliseconds

// Output variables
bool r1on = false;
bool r2on = false;

// Define "on entering" state machine callback function
void onEnter() {
  Serial.print(stateMachine.ActiveStateName());
  Serial.println(F(" enter")); 
}

// Define "on leaving" state machine callback function
void onExit() {
  Serial.print(stateMachine.ActiveStateName());
  Serial.println(F(" leave"));
}
>>>>>>> 2af71d8ef3201699bfe33db0feb8327663c307f6

// Setup the State Machine
void setupStateMachine() {
  // Follow the order of defined enumeration for the state definition (will be used as index)
  // Add States => name, timeout, onEnter cb, onState cb, onLeave cb
  stateMachine.AddState(stateName[START], 0, 0, onEnter, nullptr, onExit);
  stateMachine.AddState(stateName[OFF], 0, MIN_TIME_MS, onEnter, nullptr, onExit);
  stateMachine.AddState(stateName[R1_ON], 0, MIN_TIME_MS, onEnter, nullptr, onExit);
  stateMachine.AddState(stateName[R2_ON], 0, MIN_TIME_MS, onEnter, nullptr, onExit);

  stateMachine.AddAction(R1_ON, YA_FSM::N, r1on); // N -> while state is active led is ON
  stateMachine.AddAction(R2_ON, YA_FSM::N, r2on);

<<<<<<< HEAD
// When voltage is above threshold
long aboveThresholdTriggerTimerStart = 0;
bool voltageIsAboveThreshold = false;

TimerDelay logDelay(false, 2000);           // Log ADC values after 2seconds

// Function declarations
void setOn(int pin);
void setOff(int pin);
float readVoltage();
void dropLoads(bool ok);

void setup()
{
    // Setup Serial
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);
    setOff(LED_BUILTIN);

    // Load 1
    pinMode(LOAD_1_RELAY, OUTPUT);
    pinMode(LOAD_1_LED, OUTPUT);

    // Load 2
    pinMode(LOAD_2_RELAY, OUTPUT);
    pinMode(LOAD_2_LED, OUTPUT);

    // Load 3
    pinMode(LOAD_3_RELAY, OUTPUT);
    pinMode(LOAD_3_LED, OUTPUT);

    // Load 4
    pinMode(LOAD_4_RELAY, OUTPUT);
    pinMode(LOAD_4_LED, OUTPUT);

    // Load 5
    pinMode(LOAD_5_RELAY, OUTPUT);
    pinMode(LOAD_5_LED, OUTPUT);

    // Load 6
    pinMode(LOAD_6_RELAY, OUTPUT);
    pinMode(LOAD_6_LED, OUTPUT);

    // Voltage sensor
    pinMode(VOLTAGE_INPUT_SENSOR, INPUT);

    // Set initial states
    dropLoads(true); // turn off all loads

    logDelay.start();
}

void loop()
{
    float volts = readVoltage();

    // Run this block only once when the application starts
    // Check for current voltage and set the relays as ON/OFF
    if (appIsStarting)
    {
        appIsStarting = false;

        if (volts <= DROP_ALL_LOADS_VOLTAGE_THRESHOLD)
        {
            dropLoads(true);
        }

        else
        {
            dropLoads(false);
        }
    }

    // Runs in every cycle, check voltage of the battery and turns ON/OFF the relays
    // after the specified delay
    if (volts <= DROP_ALL_LOADS_VOLTAGE_THRESHOLD)
    {
        voltageIsAboveThreshold = false;

        if (voltageIsBelowThreshold)
        {
            // Flag had been set initially

            if (((long)(millis() - belowThresholdTriggerTimerStart)) >= DELAY_B4_SWITCHING)
            {
                // Switch off the loads
                dropLoads(true);
                voltageIsBelowThreshold = false;
            }
        }

        else
        {
            voltageIsBelowThreshold = true;
            belowThresholdTriggerTimerStart = millis();
        }
    }

    else
    {
        voltageIsBelowThreshold = false;

        if (voltageIsAboveThreshold)
        {
            // Flag had been set initially

            if (((long)(millis() - aboveThresholdTriggerTimerStart)) >= DELAY_B4_SWITCHING)
            {
                // Switch off the loads
                dropLoads(false);
                voltageIsAboveThreshold = false;
            }
        }

        else
        {
            voltageIsAboveThreshold = true;
            aboveThresholdTriggerTimerStart = millis();
        }
    }
=======
  // Add transitions with related trigger input callback functions
  stateMachine.AddTransition(START, R1_ON, [](){return readVoltage(stateMachine.ActiveStateName()) > voltageTurnOnR1;} );    
  stateMachine.AddTransition(OFF, R1_ON, [](){return readVoltage(stateMachine.ActiveStateName()) > voltageTurnOnR1;} );    
  stateMachine.AddTransition(R1_ON, OFF, [](){return readVoltage(stateMachine.ActiveStateName()) < voltageTurnOffR1;} );    
  stateMachine.AddTransition(R1_ON, R2_ON, [](){return readVoltage(stateMachine.ActiveStateName()) > voltageTurnOnR2;} );    
  stateMachine.AddTransition(R2_ON, R1_ON, [](){return readVoltage(stateMachine.ActiveStateName()) < voltageTurnOffR2;} );    
}

void setup() {
  setupPins(); // set up Input/Output
  Serial.begin(115200);
  while(!Serial) {}  // Needed for native USB port only
  Serial.println(F("Starting the Voltage Controlled Switch...\n"));
  setupStateMachine();
  dropLoads(); // turn off all loads
}

void loop() {
  // Update State Machine
  if(stateMachine.Update()){
    Serial.print(F("Active state: "));
    Serial.println(stateMachine.ActiveStateName());
  }

  // Set outputs
  digitalWrite(LOAD_1_LED, r1on);
  digitalWrite(LOAD_1_RELAY, r1on);
  digitalWrite(LOAD_2_LED, r2on);
  digitalWrite(LOAD_2_RELAY, r2on);
>>>>>>> 2af71d8ef3201699bfe33db0feb8327663c307f6

    // delay(500); // Sleep for half a second
}

void setOn(int pin)
{
    digitalWrite(pin, HIGH);
}

void setOff(int pin)
{
    digitalWrite(pin, LOW);
}

<<<<<<< HEAD
float readVoltage()
{
    int i, adc=0;

    for(i=0; i<10; i++) 
    {
        adc += analogRead(VOLTAGE_INPUT_SENSOR); // 0..4095
    }
    
    int adc_raw = adc/10;
    double adc_volt = (adc_raw * 3.3) / (4095);
    double battery_volt = adc_volt * ((RA + RB) / RB);

    if( logDelay.timedout() )
    {
        Serialprintln(
            "readVoltage ADC raw %d = %fV ~ %fV battery",
            adc_raw, adc_volt, battery_volt);
    }
    

    return battery_volt;
}

// Turn ON/OFF the loads
void dropLoads(bool ok)
{
    if (ok) // turn off loads
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

    else // turn on loads
    {
        setOn(LOAD_1_RELAY);
        setOn(LOAD_1_LED);

        setOn(LOAD_2_RELAY);
        setOn(LOAD_2_LED);

        setOn(LOAD_3_RELAY);
        setOn(LOAD_3_LED);

        setOn(LOAD_4_RELAY);
        setOn(LOAD_4_LED);

        setOn(LOAD_5_RELAY);
        setOn(LOAD_5_LED);

        setOn(LOAD_6_RELAY);
        setOn(LOAD_6_LED);
    }
=======
// Turn off the loads
void dropLoads() {
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
>>>>>>> 2af71d8ef3201699bfe33db0feb8327663c307f6
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

// https://www.norwegiancreations.com/2017/03/state-machines-and-arduino-implementation/

enum State_enum {STOP, FORWARD, ROTATE_RIGHT, ROTATE_LEFT};
enum Sensors_enum {NONE, SENSOR_RIGHT, SENSOR_LEFT, BOTH};
 
void state_machine_run(uint8_t sensors);
void motors_stop();
void motors_forward();
void motors_right();
void motors_left();
uint8_t read_IR();
 
uint8_t state = STOP;
 
void setup(){
}
 
void loop(){
  state_machine_run(read_IR());
 
  delay(10);
}
 
void state_machine_run(uint8_t sensors) 
{
  switch(state)
  {
    case STOP:
      if(sensors == NONE){
        motors_forward();
        state = FORWARD;
      }
      else if(sensors == SENSOR_RIGHT){
        motors_left();
        state = ROTATE_LEFT;
      }
      else{
        motors_right();
        state = ROTATE_RIGHT;
      }
      break;
       
    case FORWARD:
      if(sensors != NONE){
        motors_stop();
        state = STOP;
      }
      break;
 
    case ROTATE_RIGHT:
      if(sensors == NONE || sensors == SENSOR_RIGHT){
        motors_stop();
        state = STOP;
      }
      break;
 
    case ROTATE_LEFT:
      if(sensors != SENSOR_RIGHT)
      {
        motors_stop();
        state = STOP; 
      }
      break;
  }
}
 
void motors_stop()
{
  //code for stopping motors
}
 
void motors_forward()
{
  //code for driving forward  
}
 
void motors_right()
{
  //code for turning right
}
 
void motors_left()
{
  //code for turning left
}
 
uint8_t read_IR()
{
  //code for reading both sensors
}

// https://github.com/bask185/ArduinoToolchain

// https://www.hackster.io/tolentinocotesta/let-s-learn-how-to-use-finite-state-machine-with-arduino-c524ac
// https://github.com/cotestatnt/YA_FSM

*/