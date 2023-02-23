#include <Arduino.h>
#include "YA_FSM.h"
#include "pins.h"
#include "readvolt.h"
#include "util.h"

// Voltage threshold cut off
#define VOLTAGE_THRESHOLD 24 // volts
#define DELAY_B4_SWITCHING 1000*60*1 // 1 minute (milliseconds * seconds * minutes) -> milliseconds

// Flag to know when he first loop cycle begins so as to set default relay state
bool appIsStarting = true;

// When voltage is below the set threshold
long belowThresholdTriggerTimerStart = 0;
bool voltageIsBelowThreshold = false;

// When voltage is above threshold
long aboveThresholdTriggerTimerStart = 0;
bool voltageIsAboveThreshold = false;

// Function declarations
void setOn(int pin);
void setOff(int pin);
void dropLoads(bool ok);

void setup() {
  // Setup Serial
  Serial.begin(115200);
  
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
}

void loop() {
  float volts = readVoltage();

  // Run this block only once when the application starts
  // Check for current voltage and set the relays as ON/OFF
  if(appIsStarting) 
  {
    appIsStarting = false;

    if( volts <= VOLTAGE_THRESHOLD )
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
  if( volts <= VOLTAGE_THRESHOLD ) 
  {
    voltageIsAboveThreshold = false;

    if(voltageIsBelowThreshold) 
    {
      // Flag had been set initially

      if( ((long)(millis() - belowThresholdTriggerTimerStart)) >= DELAY_B4_SWITCHING ) 
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

    if(voltageIsAboveThreshold) 
    {
      // Flag had been set initially

      if( ((long)(millis() - aboveThresholdTriggerTimerStart)) >= DELAY_B4_SWITCHING ) 
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

  delay(500); // Sleep for half a second
}

void setOn(int pin) {
  digitalWrite(pin, HIGH);
}

void setOff(int pin) {
  digitalWrite(pin, LOW);
}

// Turn ON/OFF the loads
void dropLoads(bool ok) {
  if(ok) // turn off loads
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