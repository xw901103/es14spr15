// ----------------------------------------
// Filename: analog.h
// Description: Routines for setting up and
//   reading from the ADC/DAC
// Author: PMcL
// Date: 14-Feb-07

#ifndef ANALOG_H
#define ANALOG_H

#include "global.h"

// Maximum number of channels
#define NB_INPUT_CHANNELS 8
#define NB_OUTPUT_CHANNELS 4

// Number of channels used
extern TUINT16 NbAnalogInputs;
extern TUINT16 NbAnalogOutputs;

typedef enum
{
  Ch1,
  Ch2,
  Ch3,
  Ch4,
  Ch5,
  Ch6,
  Ch7,
  Ch8
} TChannelNb;

typedef struct
{
  TINT16 Value, OldValue;
  INT16 Value1, Value2, Value3;		// variables for median filtering
} TAnalogInput;

extern TAnalogInput Analog_Input[NB_INPUT_CHANNELS];

// ----------------------------------------
// Analog_Setup
// 
// Sets up the ADC and DAC
// Input:
//   none
// Output:
//   none
// Conditions:
//   none

void Analog_Setup(const UINT32 busClk);

// ----------------------------------------
// Analog_Get
// 
// Gets an analog input channel's value based on the mode
// Input:
//   channelNb is the number of the anlog input channel to read
// Output:
//   a Boolean value indicating if the channel was read successfully
// Conditions:
//   Assumes that the ADC has been set up

BOOL Analog_Get(const UINT8 channelNb);

#endif
