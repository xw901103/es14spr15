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
  /* analog interface output channels */
  ANALOG_OUTPUT_Ch1 = 0x00,
  ANALOG_OUTPUT_Ch2 = 0x01,
  ANALOG_OUTPUT_Ch3 = 0x02,
  ANALOG_OUTPUT_Ch4 = 0x03,
  /* analog interface input channels */
  ANALOG_INPUT_Ch1  = 0x10,
  ANALOG_INPUT_Ch2  = 0x11,
  ANALOG_INPUT_Ch3  = 0x12,
  ANALOG_INPUT_Ch4  = 0x13,
  ANALOG_INPUT_Ch5  = 0x14,
  ANALOG_INPUT_Ch6  = 0x15,
  ANALOG_INPUT_Ch7  = 0x16,
  ANALOG_INPUT_Ch8  = 0x17
} TAnalogChannel;

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

BOOL Analog_Get(const TAnalogChannel channelNb);

#endif
