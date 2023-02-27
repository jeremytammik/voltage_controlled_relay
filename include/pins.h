#ifndef PINS_H
#define PINS_H

// pins.h -- set up load relay, led and voltage sensor pins

// Load 1
#define LOAD_1_RELAY    21  // GPIO21
#define LOAD_1_LED      19  // GPIO19

// Load 2
#define LOAD_2_RELAY    18   // GPIO18
#define LOAD_2_LED      5  // GPIO5

// Load 3
#define LOAD_3_RELAY    4  // GPIO4
#define LOAD_3_LED      2   // GPIO2

// Load 4
#define LOAD_4_RELAY    15  // GPIO15
#define LOAD_4_LED      13  // GPIO13

// Load 5
#define LOAD_5_RELAY    12  // GPIO12
#define LOAD_5_LED      14  // GPIO14

// Load 6
#define LOAD_6_RELAY    27  // GPIO27
#define LOAD_6_LED      26  // GPIO26

// Voltage Input Sensor
#define VOLTAGE_INPUT_SENSOR  34      // GPIO34

void setupPins();

#endif // PINS_H