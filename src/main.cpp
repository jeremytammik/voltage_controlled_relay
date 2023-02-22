#include <Arduino.h>

// Load 1
#define LOAD_1_RELAY    19  // GPIO19
#define LOAD_1_LED      18  // GPIO18

// Load 2
#define LOAD_2_RELAY    5   // GPIO5
#define LOAD_2_LED      17  // GPIO17

// Load 3
#define LOAD_3_RELAY    16  // GPIO16
#define LOAD_3_LED      4   // GPIO4

// Load 4
#define LOAD_4_RELAY    12  // GPIO12
#define LOAD_4_LED      14  // GPIO14

// Load 5
#define LOAD_5_RELAY    27  // GPIO27
#define LOAD_5_LED      26  // GPIO26

// Load 6
#define LOAD_6_RELAY    25  // GPIO25
#define LOAD_6_LED      33  // GPIO33

// Voltage Input Sensor
#define VOLTAGE_INPUT_SENSOR  36      // GPIO36
int VOLTAGE_ADC_VALUE = 0;            // ADC value
double voltageValue = 0;              // ADC conversion to volts
double realVoltage = 0;               // Real battery voltage

// Voltage divider resistors
// Recommended 10k, 1k - 
// At 30v battery, voltage will be 30/10 = 3v at ESP
// At 24v battery, voltage will be 2.4 on ESP
#define RA      10  // 10k Ohms
#define RB      1   // 1k Ohms

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
float checkVoltage();
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
  float volts = checkVoltage();

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

float checkVoltage() {
  VOLTAGE_ADC_VALUE = analogRead(VOLTAGE_INPUT_SENSOR);
  voltageValue = (VOLTAGE_ADC_VALUE * 3.3) / (4095);

  realVoltage = voltageValue * (RA/RB);

  Serial.print("Voltages\tADC=");
  Serial.print(VOLTAGE_ADC_VALUE);
  Serial.print("\tVOLTS=");
  Serial.print(voltageValue);
  Serial.print("\tBAT VOLTAGE=");
  Serial.println(realVoltage);

  return realVoltage;
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
