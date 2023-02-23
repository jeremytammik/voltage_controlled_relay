#include <Arduino.h>
#include "YA_FSM.h"
#include "pins.h"
#include "readvolt.h"
#include "util.h"

void dropLoads();

// Voltage thresholds to turn on and off relay R1 and R2
float voltageTurnOnR1 = 25.0;
float voltageTurnOffR1 = 24.0;
float voltageTurnOnR2 = 27.0;
float voltageTurnOffR2 = 26.5;

// Create new FSM
YA_FSM stateMachine;

// State Alias
enum State { OFF, R1_ON, R2_ON };

// Helper for print labels instead integer when state change
const char * const stateName[] PROGMEM = { "OFF", "R1_ON", "R2_ON" };

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

// Setup the State Machine
void setupStateMachine() {
  // Follow the order of defined enumeration for the state definition (will be used as index)
  // Add States => name, timeout, onEnter cb, onState cb, onLeave cb
  stateMachine.AddState(stateName[OFF], 0, MIN_TIME_MS, onEnter, nullptr, onExit);
  stateMachine.AddState(stateName[R1_ON], 0, MIN_TIME_MS, onEnter, nullptr, onExit);
  stateMachine.AddState(stateName[R2_ON], 0, MIN_TIME_MS, onEnter, nullptr, onExit);

  stateMachine.AddAction(R1_ON, YA_FSM::N, r1on); // N -> while state is active led is ON
  stateMachine.AddAction(R2_ON, YA_FSM::N, r2on);

  // Add transitions with related trigger input callback functions
  stateMachine.AddTransition(OFF, R1_ON, [](){return readVoltage() > voltageTurnOnR1;} );    
  stateMachine.AddTransition(R1_ON, OFF, [](){return readVoltage() < voltageTurnOffR1;} );    
  stateMachine.AddTransition(R1_ON, R2_ON, [](){return readVoltage() > voltageTurnOnR2;} );    
  stateMachine.AddTransition(R2_ON, R1_ON, [](){return readVoltage() < voltageTurnOffR2;} );    
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

  delay(500); // Sleep for half a second
}

void setOn(int pin) {
  digitalWrite(pin, HIGH);
}

void setOff(int pin) {
  digitalWrite(pin, LOW);
}

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