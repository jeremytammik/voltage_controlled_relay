// readvolt.cpp -- read voltage

#include <Arduino.h>
#include "pins.h"
#include "util.h"
#include "readvolt.h"

// Voltage divider resistors Ra + Rb in Ohm
// Their resistance in parallel should come to ca. 10k, cf.
// https://arduino.stackexchange.com/questions/78768/what-is-the-most-efficient-voltage-divider-for-arduino
// Max ESP32 ADC input voltage is 3.3V, cf. 
// https://deepbluembedded.com/esp32-adc-tutorial-read-analog-voltage-arduino/
// 30V converts to 3.289V using Ra = 81.2k = 75k + 6.2k and Rb = 10k, cf.
// https://ohmslawcalculator.com/voltage-divider-calculator
// Vadc = Vbat * Rb/(Ra+Rb)
// Vbat = Vadc * (Ra+Rb)/Rb
#define RA 81200
#define RB 10000

float readVoltage() {
  int adc_raw = analogRead(VOLTAGE_INPUT_SENSOR); // 0..4095
  float adc_volt = (adc_raw * 3.3) / (4095);
  float battery_volt = adc_volt * ((RA+RB)/RB);

  Serialprintln(
    "readVoltage ADC raw %d = %fV ~ %fV battery",
    adc_raw, adc_volt, battery_volt);

  return battery_volt;
}
