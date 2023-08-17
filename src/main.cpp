#include <Arduino.h>
#include "pins.h"
#include "jsettime.h"
#include "readvolt.h"
#include "util.h"
#include "timerdelay.h"
#include "btcontroller.h"

TimerDelay btSendDelay(false, 2000);
BluetoothController btController;

void dropLoads();

// ADC thresholds to turn on and off relays R1 and R2

int adcTurnOffAll = 400; // 25.3V
int adcTurnOnPv = 1000; // 26.0V
//int adcTurnOffR2 = 1300; // 26.5V
//int adcTurnOnR2 = 1700; // 27.0V

// Output variables
bool pv_on = false;
//bool pv_and_hp_on = false;
//bool pv_and_hp_and_hppv_on = false;

// States
enum State
{
    OFF, // == Start
    PV_ON,
//    PV_AND_HP_ON, 
//    PV_AND_HP_AND_HPPV_ON
};

// Helper for print labels instead integer when state change
const char * const stateName[] PROGMEM = {"OFF", "PV_ON"}; //, "PV_AND_HP_ON", "PV_AND_HP_AND_HPPV_ON"};

// The current state is determined from the two output variables
State current_state()
{
    //if(pv_and_hp_and_hppv_on) { return PV_AND_HP_AND_HPPV_ON; }
    //else if(pv_and_hp_on) { return PV_AND_HP_ON; }
    if(pv_on) { return PV_ON; }
    else return OFF;
}

void setup()
{
    setupPins(); // set up Input/Output
    Serial.begin(115200);
    while (!Serial) {
    } // Needed for native USB port only

    Serial.println(F("Starting the Voltage Controlled Switch...\n"));

    dropLoads();

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
    State old_state = current_state();
    int adc = readVoltage();

    //btController.send("BAT_ADC", String(adc));

    if( OFF == old_state)
    {
        //if( adcTurnOnR2 < adc ) { r1_and_r2_on = true; }
        //else if( adcTurnOnR1 < adc ) { r1_on = true; }
        if( adcTurnOnPv < adc ) { pv_on = true; }
    }
    else if( PV_ON == old_state)
    {
        //if( adcTurnOffAll > adc ) { r1_on = r1_and_r2_on = false; }
        //else if( adcTurnOnR2 < adc ) { r1_and_r2_on = true; }
        if( adcTurnOffAll > adc ) { pv_on = false; }
    }
    //else if( R1_AND_R2_ON == old_state)
    //{
    //    if( adcTurnOffAll > adc ) { r1_on = r1_and_r2_on = false; }
    //    else if( adcTurnOffR2 > adc ) { r1_and_r2_on = false; }
    //}

    State current = current_state();

    if( (old_state != current) || (0 == (++loopCount % 100)) ) {
      Serialprintln( "ADC %d - state %s --> %s",
        adc, stateName[old_state], stateName[current]);

      digitalWrite(LOAD_1_RELAY, !pv_on);
    }

    // Set outputs
    //digitalWrite(LOAD_1_LED, r1_on || r1_and_r2_on);
    //digitalWrite(LOAD_1_RELAY, r1_on || r1_and_r2_on);
    //digitalWrite(LOAD_2_LED, r1_and_r2_on);
    //digitalWrite(LOAD_2_RELAY, r1_and_r2_on);
    
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

    delay(5); // sleep very briefly, need fast switch for heat pump
}

void setOn(int pin)
{
    digitalWrite(pin, HIGH);
}

void setOff(int pin)
{
    digitalWrite(pin, LOW);
}

void turnOffAll()
{
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

// Turn ON/OFF the loads
void dropLoads()
{
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
