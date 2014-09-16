// ----------------------------------------
// Filename: timer.h
// Description:
//   Routines to implement general purpose timers
//   Routines to support the modulus down-counter
//     as a periodic timer
// Author: PMcL
// Date: 27-Mar-06

#ifndef TIMER_H
#define TIMER_H

// new types
#include "global.h"

typedef enum
{
  TIMER_OUTPUT_DISCONNECT,
  TIMER_OUTPUT_TOGGLE,
  TIMER_OUTPUT_LOW,
  TIMER_OUTPUT_HIGH
} TTimerOutputAction;

typedef enum
{
  TIMER_INPUT_OFF,
  TIMER_INPUT_RISING,
  TIMER_INPUT_FALLING,
  TIMER_INPUT_ANY
} TTimerInputDetection;

typedef enum
{
  TIMER_Ch0, 
  TIMER_Ch1,
  TIMER_Ch2,
  TIMER_Ch3,
  TIMER_Ch4,
  TIMER_Ch5,
  TIMER_Ch6,
  TIMER_Ch7
} TTimerChannel;

typedef void(*TTimerRoutine)(const TTimerChannel);

typedef struct
{
  BOOL outputCompare;
  TTimerOutputAction outputAction;
  TTimerInputDetection inputDetection;
  BOOL toggleOnOverflow;
  BOOL interruptEnable;
  BOOL pulseAccumulator;
  TTimerRoutine routine;
} TTimerSetup;

// ----------------------------------------
// Timer_SetupPeriodicTimer
// 
// Sets the period of the periodic timer
// Input:
//   microSeconds is the number of microseconds for one period
//   busClk is the actual bus clock rate in Hz
// Output:
//   none
// Conditions:
//   none

void Timer_SetupPeriodicTimer(const UINT16 microSeconds, const UINT32 busClk);

// ----------------------------------------
// Timer_PeriodicTimerEnable
// 
// Enables or disables the periodic timer
// Input:
//   enableTimer is a Boolean value indicating whether to enable the timer
// Output:
//   none
// Conditions:
//   Assumes the timer has been set up

void Timer_PeriodicTimerEnable(const BOOL enable);

// ----------------------------------------
// Timer_Setup
// 
// Sets up the Enhanced Capture Timer unit for
// operations with the timers
// Input:
//   none
// Output:
//   none
// Conditions:
//   none

void Timer_Setup(void);

// ----------------------------------------
// Timer_Init
// 
// Sets up a timer channel
// Input:
//   channelNb is the timer channel number
//   aTimerSetup is a structure containing the parameters to 
//     be used in setting up the timer channel:
//       outputCompare is TRUE when the timer should be configured as an
//         output, otherwise it is configured as an input
//       outputAction is the action to take on a successful output compare
//       inputDetection is the type of input capture detection
//       toggleOnOverflow is TRUE when the timer should toggle on overflow
//       interruptEnable is TRUE to enable interrupts
// Output:
//   none
// Conditions:
//   none

void Timer_Init(const TTimerChannel channelNb, const TTimerSetup * const aTimerSetup);

// ----------------------------------------
// Timer_Enable
// 
// Enables or disables a timer channel interrupt
// Input:
//   channelNb is the timer channel number
//   enableInt is a Boolean value indicating whether to enable the 
//     interrupt on the timer channel
// Output:
//   none
// Conditions:
//   none

void Timer_Enable(const TTimerChannel channelNb, const BOOL enableInt);

// ----------------------------------------
// Timer_Enabled
// 
// Returns the status of a timer
// Input:
//   channelNb is the timer channel number
// Output:
//   a Boolean value indicating whether the channel is enabled
// Conditions:
//   none

BOOL Timer_Enabled(const TTimerChannel channelNb);

// ----------------------------------------
// Timer_Set
// 
// Sets a timer channel to generate an
//   interrupt after a certain number of busClk cycles
// Input:
//   channelNb is the timer channel number
//   busClkCyclesDelay is the number of busClk cycles
//     to wait until an interrupt is triggered
// Output:
//   none
// Conditions:
//   none

void Timer_Set(const TTimerChannel channelNb, const UINT16 busClkCyclesDelay);

void Timer_AttachRoutine(const TTimerChannel channelNb, TTimerRoutine const routinePtr);

void Timer_DetachRoutine(const TTimerChannel channelNb);

#endif