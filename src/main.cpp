#include <Arduino.h>
#include "pins.h"
#include "jsettime.h"
#include "readvolt.h"
#include "util.h"
#include "timerdelay.h"
#include "btcontroller.h"

TimerDelay btSendDelay(false, 2000);
BluetoothController btController;

// ADC thresholds to turn on and off relays R1 and R2

int adcTurnOffPv = 300; // 25.2V -- remove all loads from PV system
int adcTurnOnPv = 1000; // 26.2V -- attach moniwonig electricity to PV
int adcTurnOffHp = 1300; // 26.5V -- drive heat pump from grid mains, not PV
int adcTurnOnHp = 1500; // 26.6V -- drive heat pump from PV, not grid mains
int adcTurnOffHppv = 1300; // 26.5V -- turn off heat pump PV switch
int adcTurnOnHppv = 1700; // 27.0V -- turn on heat pump PV switch, so it heats up to the max

// Output variables
//bool pv_on = false;
//bool pv_and_hp_on = false;
//bool pv_and_hp_and_hppv_on = false;

// States
enum State
{
  OFF, // == Start
  PV_ON,
  PV_AND_HP_ON, 
  PV_AND_HP_AND_HPPV_ON
};

// Helper for print labels instead integer when state change
const char * const stateName[] PROGMEM = {"OFF", "PV_ON", "PV_AND_HP_ON", "PV_AND_HP_AND_HPPV_ON"};

// The current state is determined from the two output variables
//State current_state()
//{
//    if(pv_and_hp_and_hppv_on) { return PV_AND_HP_AND_HPPV_ON; }
//    else if(pv_and_hp_on) { return PV_AND_HP_ON; }
//    else if(pv_on) { return PV_ON; }
//    else return OFF;
//}

State current_state = OFF;

void setup()
{
  setupPins(); // set up Input/Output
  Serial.begin(115200);
  while (!Serial) {
  } // Needed for native USB port only

  Serial.println(F("Starting the Voltage Controlled Switch...\n"));

  dropLoads();
  current_state = OFF;

  int year = 2023;
  int month = 4; // [0,11], January = 0
  int day = 24;
  int hour = 0; // [0,24]
  int minute = 0; // [0,59]
  int second = 0; // [0.59]

  jsettime(year, month, day, hour, minute, second );

  // Initialize bluetooth and its delay timer
  btController.init();
  btSendDelay.start();
  btController.sendInfo("Initializing voltage_controlled_relay...");
}

int loopCount = 0;

void loop()
{
  // Print status every 1000 ms

  bool printIt = (0 == (++loopCount % 1000));

  int adc = readVoltage( printIt );
  //btController.send("BAT_ADC", String(adc));

  State old_state = current_state;
  State new_state = OFF;
  if( OFF == old_state)
  {
    if( adcTurnOnHppv < adc ) { new_state = PV_AND_HP_AND_HPPV_ON; }
    else if( adcTurnOnHp < adc ) { new_state = PV_AND_HP_ON; }
    else if( adcTurnOnPv < adc ) { new_state = PV_ON; }
    else new_state = OFF;
  }
  else if( PV_ON == old_state)
  {
    if( adcTurnOnHppv < adc ) { new_state = PV_AND_HP_AND_HPPV_ON; }
    else if( adcTurnOnHp < adc ) { new_state = PV_AND_HP_ON; }
    else if( adcTurnOffPv > adc ) { new_state = OFF; }
  }
  else if( PV_AND_HP_ON == old_state)
  {
    if( adcTurnOnHppv < adc ) { new_state = PV_AND_HP_AND_HPPV_ON; }
    else if( adcTurnOffPv > adc ) { new_state = OFF; }
    else if( adcTurnOffHp > adc ) { new_state = PV_ON; }
  }
  else if( PV_AND_HP_AND_HPPV_ON == old_state)
  {
    if( adcTurnOnHppv < adc ) { new_state = PV_AND_HP_AND_HPPV_ON; }
    else if( adcTurnOffPv > adc ) { new_state = OFF; }
    else if( adcTurnOffHp > adc ) { new_state = PV_ON; }
    else if( adcTurnOffHppv > adc ) { new_state = PV_AND_HP_ON; }
  }

  if( (old_state != new_state) || printIt ) {
    Serialprintln( "ADC %d - state %s --> %s",
      adc, stateName[old_state], stateName[new_state]);

    if( old_state != new_state ) {
      switch (new_state) {
      case OFF: dropLoads(); break;
      case PV_ON: setLoads( true, false, false ); break;
      case PV_AND_HP_ON: setLoads( true, true, false ); break;
      case PV_AND_HP_AND_HPPV_ON: setLoads( true, true, true ); break;
      }
      current_state = new_state;
    }
  }

  // Sync the bluetooth controller
  /*
  BluetoothData btData = btController.sync();

  if(btData.available()) {
      // Handle data sent over bluetooth
      Serial.print("CMD -> '");
      Serial.print(btData.getCommand());
      Serial.print("\tData -> '");
      Serial.print(btData.getData());
      Serial.println("'");
  }
  */

  delay(1); // sleep very briefly, need fast switch for heat pump
}
