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