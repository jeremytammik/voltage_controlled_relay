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
//#define RA 81200
//#define RB 10000

// Ra = 82k, Rb = 10k
// 3.3/4096 * (82+12)/12
// 3.3*(ra+rb)/rb = 30.36
// (3.3/4096)*(ra+rb)/rb = 0.007412109375
// tweak this value to match the real voltage:

// ADC to battery voltage conversion factor 
// using resistor-based voltage divider:

//#define ADC2BAT 0.007412109375

double vrange = 2 * 3.3; // max ADC range with 1:2 voltage divider
double vmin = 24.7;
double vmax = vmin + vrange;
//double vdiff = vmax - vmin;

int readVoltage( bool printIt ) 
{
  int adc_raw = analogRead(VOLTAGE_INPUT_SENSOR); // 0..4095

  double battery_volt = (adc_raw / 4096.0) * vrange + vmin;

  if( printIt ) {
    Serialprintln(
      "readVoltage ADC raw %d ~ ca. %fV battery",
      adc_raw, battery_volt);
  }

  return adc_raw;
}

/*
VoltRange classifyVoltage()
{
  int adc_raw = analogRead(VOLTAGE_INPUT_SENSOR); // 0..4095

  // ADC to battery voltage conversion 
  // using 24V Zener diode shifter and 1:2 divider:

  double battery_volt = (adc_raw / 2048.0) + 24;

  VoltRange result = Empty;

  if( adc_raw > AdcHigh ) { result = High; }
  else if( adc_raw > AdcGood ) { result = Good; }
  else if( adc_raw > AdcOk ) { result = Ok; }
  else if( adc_raw > AdcLow ) { result = Low; }

  Serialprintln(
    "readVoltage ADC raw %d ~ %fV battery --> %d",
    adc_raw, battery_volt, result);

  return result;
}
*/