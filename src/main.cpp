#include <Arduino.h>
#include "pins.h"
#include "jsettime.h"
#include "readvolt.h"
#include "util.h"
#include "timerdelay.h"
//#include "btcontroller.h"
#include "filtermedian.h"

// Looping and timer control

int loopCount = 0;
const int loopDelayMs = 8; // wait x milliseconds before next loop iteration
const int loopIterationsPerSecond = 1000 * loopDelayMs;
int skipPrintFor = 1000; // print status once in 1000 loop iterations

// Wait a while before switching on again after switching off;
// switching off, however, is executed immediately:

int stayOffForSeconds = 10; // pause x seconds before switching on again
int stayOffForIterations = stayOffForSeconds * loopIterationsPerSecond;
unsigned int stayOffCounter = 0;

// Read ADC via median filter

const int medianValuesLeftRight = 500;
const int medianWindowSize = 1 + 2 * medianValuesLeftRight;
MedianFilter<int> medianFilter(medianWindowSize);

//TimerDelay btSendDelay(false, 2000);
//BluetoothController btController;

// ADC thresholds to turn on and off relays

//  300  24.9 - 25.2V -- remove all loads from PV system
//  750  25.8
//  900  26.0 - 26.1
//  950  26.1
// 1000  25.9 - 26.2V -- attach moniwonig electricity to PV
// 1200  26.3 26.2 - 26.4V -- drive heat pump from grid mains, not PV
// 1250  26.3 - 26.5V -- turn off heat pump PV switch
// 1300  26.3 - 26.6V -- drive heat pump from grid mains, not PV
// 1400  26.4 - 26.5 - 26.7V -- drive heat pump from PV, not grid mains
// 1500  
// 1600  26.5
// 1700  27.0

int adc; // current ADC measurement
const int adcTurnOffPv    = 300; // remove all loads from PV system
const int adcTurnOnPv    =  700; // attach moniwonig electricity to PV
const int adcTurnOffHp   = 1100; // drive heat pump from grid mains, not PV
const int adcTurnOnHp    = 1600; // drive heat pump from PV, not grid mains
const int adcTurnOffHppv = 1200; // turn off heat pump PV switch
const int adcTurnOnHppv  = 2900; // turn on heat pump PV switch

enum State
{
  OFF, // == Start
  PV_ON,
  PV_AND_HP_ON,
  PV_AND_HP_AND_HPPV_ON
};

// Print labels far state
const char *const stateName[] PROGMEM = {"OFF", "PV_ON", "PV_AND_HP_ON", "PV_AND_HP_AND_HPPV_ON"};

State current_state = OFF;

void printStatus( State new_state )
{
  Serialprintln( 
    "ADC %d - state %s --> %s (countdown %d)",
    adc, stateName[current_state], stateName[new_state], 
    stayOffCounter );
}

// startCountdown -- Start countdown only if it is not already running

void startCountdown( State old_state, State new_state )
{
  if( 0 == stayOffCounter ) { 
    stayOffCounter = stayOffForIterations;
    printStatus( new_state );    
  }
}

State getNewState( int adc, State old_state )
{
  State new_state = old_state;
  if (OFF == old_state)
  {
    if( adc > adcTurnOnHppv )
    {
      new_state = PV_AND_HP_AND_HPPV_ON;
    }
    else if( adc > adcTurnOnHp )
    {
      new_state = PV_AND_HP_ON;
    }
    else if( adc > adcTurnOnPv )
    {
      new_state = PV_ON;
    }
  }
  else if (PV_ON == old_state)
  {
    if( adc > adcTurnOnHppv )
    {
      new_state = PV_AND_HP_AND_HPPV_ON;
    }
    else if( adc > adcTurnOnHp)
    {
      new_state = PV_AND_HP_ON;
    }
    else if( adc < adcTurnOffPv )
    {
      new_state = OFF;
      startCountdown( old_state, new_state );
    }
  }
  else if (PV_AND_HP_ON == old_state)
  {
    if( adc > adcTurnOnHppv )
    {
      new_state = PV_AND_HP_AND_HPPV_ON;
    }
    else if( adc < adcTurnOffPv )
    {
      new_state = OFF;
      startCountdown( old_state, new_state );
    }
    else if( adc < adcTurnOffHp )
    {
      new_state = PV_ON;
      startCountdown( old_state, new_state );
    }
  }
  else if (PV_AND_HP_AND_HPPV_ON == old_state)
  {
    if( adc < adcTurnOffPv )
    {
      new_state = OFF;
      startCountdown( old_state, new_state );
    }
    else if( adc < adcTurnOffHp )
    {
      new_state = PV_ON;
      startCountdown( old_state, new_state );
    }
    else if( adc < adcTurnOffHppv )
    {
      new_state = PV_AND_HP_ON;
      startCountdown( old_state, new_state );
    }
  }
  return new_state;  
}

void setup()
{
  setupPins(); // set up Input/Output
  Serial.begin(115200);
  while (!Serial)
  {
  } // Needed for native USB port only

  Serial.println(F("Starting the Voltage Controlled Switch...\n"));

  dropLoads();
  current_state = OFF;

  /*
  int year = 2023;
  int month = 4; // [0,11], January = 0
  int day = 24;
  int hour = 0;   // [0,24]
  int minute = 0; // [0,59]
  int second = 0; // [0.59]
  jsettime(year, month, day, hour, minute, second);
  */

  /*
  // Initialize bluetooth and its delay timer
  btController.init();
  btSendDelay.start();
  btController.sendInfo("Initializing voltage_controlled_relay...");
  */
}

void loop()
{
  bool printIt = (0 == (++loopCount % skipPrintFor));

  adc = readVoltage(printIt);
  // btController.send("BAT_ADC", String(adc));
  adc = medianFilter.AddValue(adc);
  // btController.send("median", String(adc));
  if( loopCount < medianWindowSize )
  {
    return;
  }
  
  if( 0 < stayOffCounter ) {
    --stayOffCounter;
  }

  /*
  // Collect n values for median determination
  // START comment block if using the median filter lib added
  if (adcValueIndex < medianWindowSize)
  {
    adcValues[adcValueIndex++] = adc;
    if (adcValueIndex < medianWindowSize)
    {
      return;
    }
  }
  memmove(adcValues, adcValues + 1, (medianWindowSize - 1) * sizeof(int));
  adcValues[medianWindowSize - 1] = adc;

  // Select median value

  memcpy(adcValuesSorted, adcValues, sizeof(adcValues));
  sortArray(adcValuesSorted, medianWindowSize);
  adc = adcValuesSorted[medianValuesLeftRight];
  // END comment block if using the median filter lib added
  */

  State new_state = getNewState( adc, current_state );

  if( (new_state != current_state ) || printIt )
  {
    // Go to lower state immediately;
    // take a break before going to a higher state:

    if( (new_state < current_state)
      || ( (new_state > current_state) && (0 == stayOffCounter) ) )
    {
      printStatus( new_state );    

      switch (new_state)
      {
      case OFF:
        dropLoads();
        break;
      case PV_ON:
        setLoads(true, false, false);
        break;
      case PV_AND_HP_ON:
        setLoads(true, true, false);
        break;
      case PV_AND_HP_AND_HPPV_ON:
        setLoads(true, true, true);
        break;
      }
      current_state = new_state;
    }
  }

  /*
  // Sync the bluetooth controller

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

  delay(loopDelayMs); // sleep as briefly as possible
}
