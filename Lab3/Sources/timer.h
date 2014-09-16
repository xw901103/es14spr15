/**
 * \file timer.h
 * \brief Routines to implement general purpose timer and support the modulus down-counter as a periodic timer
 * \author Xu Waycell
 * \date 8-September-2014 
 */
#ifndef TIMER_H
#define TIMER_H

#include "global.h"

#ifdef NO_INTERRUPT
#error "Timer module depends on interrupt feature enabled."
#endif

/**
 * \brief
 */
typedef enum
{
  TIMER_OUTPUT_DISCONNECT,
  TIMER_OUTPUT_TOGGLE,
  TIMER_OUTPUT_LOW,
  TIMER_OUTPUT_HIGH
} TTimerOutputAction;

/**
 * \brief
 */
typedef enum
{
  TIMER_INPUT_OFF,
  TIMER_INPUT_RISING,
  TIMER_INPUT_FALLING,
  TIMER_INPUT_ANY
} TTimerInputDetection;

/**
 * \brief
 */
typedef enum
{
  TIMER_Ch0 = 0, 
  TIMER_Ch1 = 1,
  TIMER_Ch2 = 2,
  TIMER_Ch3 = 3,
  TIMER_Ch4 = 4,
  TIMER_Ch5 = 5,
  TIMER_Ch6 = 6,
  TIMER_Ch7 = 7
} TTimerChannel;

/**
 * \brief
 */
typedef void(*TTimerRoutine)(const TTimerChannel);

/**
 * \brief
 */
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

/**
 * \fn void Timer_SetupPeriodicTimer(const UINT16 microSeconds, const UINT32 busClk)
 * \brief Sets the period of the periodic timer
 * \param microSeconds number of microseconds for one period
 * \param busClk actual bus clock rate in Hz
 * \see Timer_PeriodicTimerEnable
 */
void Timer_SetupPeriodicTimer(const UINT16 microSeconds, const UINT32 busClk);

/**
 * \fn void Timer_PeriodicTimerEnable(const BOOL enable)
 * \brief Enables or disables the periodic timer 
 * \param enableTimer boolean value indicating whether to enable the timer 
 * \see Timer_SetupPeriodicTimer
 * \warning Assumes the timer has been set up
 */
void Timer_PeriodicTimerEnable(const BOOL enableTimer);

/**
 * \fn void Timer_Setup(void)
 * \brief Sets up the Enhanced Capture Timer unit for operations with the timers
 * \see Timer_Init
 */
void Timer_Setup(void);

/**
 * \fn void Timer_Init(const TTimerChannel channelNb, const TTimerSetup * const aTimerSetup)
 * \brief Sets up a timer channel 
 * \param channelNb timer channel number
 * \param aTimerSetup a structure containing the parameters to be used in setting up the timer channel:
 * - TTimerSetup
 * |- outputCompare is TRUE when the timer should be configured as an output, otherwise it is configured as an input
 * |- outputAction is the action to take on a successful output compare
 * |- inputDetection is the type of input capture detection
 * |- toggleOnOverflow is TRUE when the timer should toggle on overflow
 * |- interruptEnable is TRUE to enable interrupts
 * |- routine is the function that will be executed once an interrupt triggered
 * \see Timer_Setup
 */
void Timer_Init(const TTimerChannel channelNb, const TTimerSetup * const aTimerSetup);

/**
 * \fn void Timer_Enable(const TTimerChannel channelNb, const BOOL enableInt)
 * \brief Enables or disables a timer channel interrupt 
 * \param channelNb timer channel number
 * \param enableInt boolean value indicating whether to enable the interrupt on the timer channel
 * \see Timer_Enabled
 */
void Timer_Enable(const TTimerChannel channelNb, const BOOL enableInt);

/**
 * \fn BOOL Timer_Enabled(const TTimerChannel channelNb)
 * \brief Returns the status of a timer
 * \param channelNb timer channel number
 * \return a Boolean value indicating whether the channel is enabled
 * \see Timer_Enable
 */
BOOL Timer_Enabled(const TTimerChannel channelNb);

/**
 * \fn void Timer_Set(const TTimerChannel channelNb, const UINT16 busClkCyclesDelay)
 * \brief Sets a timer channel to generate an interrupt after a certain number of bus clock cycles.
 * \param channelNb timer channel number
 * \param busClkCyclesDelay the number of bus clock cycles to wait until an interrupt is triggered
 * \see Timer_Enable
 * \see Timer_Enabled
 * \warning TCn = TCNT + busClkCyclesDelay
 */
void Timer_Set(const TTimerChannel channelNb, const UINT16 busClkCyclesDelay);

/**
 * \fn void Timer_AttachRoutine(const TTimerChannel channelNb, TTimerRoutine const routinePtr)
 * \brief attach a function to given timer channel
 * \param channelNb timer channel number
 * \param routinePtr function pointer
 * \see Timer_DetachRoutine
 */
void Timer_AttachRoutine(const TTimerChannel channelNb, TTimerRoutine const routinePtr);

/**
 * \fn void Timer_DetachRoutine(const TTimerChannel channelNb)
 * \brief detach function of given timer channel
 * \param channelNb timer channel number
 * \see Timer_AttachRoutine
 */
void Timer_DetachRoutine(const TTimerChannel channelNb);

/**
 * \fn void Timer_ScheduleRoutine(const TTimerChannle channelNb, const UINT16 busClkCyclesDelay)
 * \brief Sets a timer channel to generate an interrupt to run attached routine after a certain number of bus clock cycles.
 * \param channelNb timer channel number
 * \param busClkCyclesDelay the number of bus clock cycles to wait until an interrupt is triggered
 * \warning TCn += busClkCyclesDelay 
 */
void Timer_ScheduleRoutine(const TTimerChannel channelNb, const UINT16 busClkCyclesDelay);

#endif