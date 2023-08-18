#include <Arduino.h>
#include "pins.h"

void setupPins() 
{
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
}

void setOn(int pin)
{
  digitalWrite(pin, HIGH);
}

void setOff(int pin)
{
  digitalWrite(pin, LOW);
}

// Turn OFF all loads
void dropLoads()
{
  // When the PV relay is turned on, its setting is the same 
  // as wehn it has no power attached. Since we want the OFF
  // state to match the "no power" state, we have to negate
  // that control input.
  
  setOn(LOAD_1_RELAY); // PV is off when relay is on
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

void setLoads( bool pv, bool hp, bool hppv )
{
  digitalWrite(LOAD_1_RELAY, !pv);
  digitalWrite(LOAD_2_RELAY, hp);
  digitalWrite(LOAD_3_RELAY, hppv);
}
