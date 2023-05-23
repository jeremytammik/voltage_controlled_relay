#ifndef READVOLT_H
#define READVOLT_H

// readvolt.h -- read voltage

int readVoltage(); // return raw ADC 

/*
enum VoltRange
{
    Empty, // [0,24.9]
    Low, // [24.9,25.5]
    Ok, // [25.5,26.5]
    Good, // [26.5, 26.9]
    High // [26.9,33]
};

// map [24V,34V] to [0,4096]:
// we use two 12V zener diodes 
// to shift [24V,34V] doen to [0V,10V] 
// and a voltage divider to divide 
// [0V,10V] down to [0V,5V] for ADC input.

enum AdcRange
{
    AdcHigh = 1187, // 26.9 --> 2.9 --> 0.29 * 4096
    AdcGood = 1024,
    AdcOk = 614,
    AdcLow = 368,
    AdcEmpty = 0
};

VoltRange classifyVoltage();
*/

#endif // READVOLT_H
