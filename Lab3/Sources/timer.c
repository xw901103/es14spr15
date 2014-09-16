#include "timer.h"
#include <mc9s12a512.h>

#ifdef NO_INTERRUPT
#error "Timer module depends on interrupt."
#endif

#ifndef CONFIG_EEPROM_ADDRESS_MODULUS_DOWN_COUNTER_DEBUG
#define ModulusDownCounterDebugEnable *(volatile UINT16*)0x0420 /* fallback plan */
#warning "ModulusDownCounterDebugEnable using fallback setting 0x0420"
#else
#define ModulusDownCounterDebugEnable *(volatile UINT16*)CONFIG_EEPROM_ADDRESS_MODULUS_DOWN_COUNTER_DEBUG
#endif

#ifndef CONFIG_EEPROM_ADDRESS_TIMER_CH7_DEBUG
#define TimerCh7DebugEnable *(volatile UINT16*)0x0420 /* fallback plan */
#warning "TimerCh7DebugEnable using fallback setting 0x0420"
#else
#define TimerCh7DebugEnable *(volatile UINT16*)CONFIG_EEPROM_ADDRESS_TIMER_CH7_DEBUG
#endif

static TTimerRoutine timerCh0Routine = (TTimerRoutine) 0x0000,
                     timerCh1Routine = (TTimerRoutine) 0x0000,
                     timerCh2Routine = (TTimerRoutine) 0x0000,
                     timerCh3Routine = (TTimerRoutine) 0x0000,
                     timerCh4Routine = (TTimerRoutine) 0x0000,
                     timerCh5Routine = (TTimerRoutine) 0x0000,
                     timerCh6Routine = (TTimerRoutine) 0x0000,
                     timerCh7Routine = (TTimerRoutine) 0x0000;

void interrupt VectorNumber_Vtimmdcu Timer_PeriodicTimer_ISR(void) 
{
  /* ack flag */
  MCFLG_MCZF = 1;
  
  /* check if debug is on */
  if (ModulusDownCounterDebugEnable)
  {
    DDRT_DDRT4 = 1;
    PTT_PTT4 = !PTT_PTT4;
  }  
}

void interrupt VectorNumber_Vtimch0 TimerCh0ISR(void) 
{
  TFLG1_C0F = 1;

  if (timerCh0Routine)
  {
    timerCh0Routine(TIMER_Ch0);
  }
}

void interrupt VectorNumber_Vtimch1 TimerCh1ISR(void) 
{
  TFLG1_C1F = 1;

  if (timerCh1Routine)
  {
    timerCh1Routine(TIMER_Ch1);
  }
}

void interrupt VectorNumber_Vtimch2 TimerCh2ISR(void) 
{
  TFLG1_C2F = 1;

  if (timerCh2Routine)
  {
    timerCh2Routine(TIMER_Ch2);
  }
}

void interrupt VectorNumber_Vtimch3 TimerCh3ISR(void) 
{
  TFLG1_C3F = 1;

  if (timerCh3Routine)
  {
    timerCh3Routine(TIMER_Ch3);
  }
}

void interrupt VectorNumber_Vtimch4 TimerCh4ISR(void) 
{
  TFLG1_C4F = 1;

  if (timerCh4Routine)
  {
    timerCh4Routine(TIMER_Ch4);
  }
}

void interrupt VectorNumber_Vtimch5 TimerCh5ISR(void) 
{
  TFLG1_C5F = 1;

  if (timerCh5Routine)
  {
    timerCh5Routine(TIMER_Ch5);
  }
}

void interrupt VectorNumber_Vtimch6 TimerCh6ISR(void) 
{
  TFLG1_C6F = 1;

  if (timerCh6Routine)
  {
    timerCh6Routine(TIMER_Ch6);
  }
}

void interrupt VectorNumber_Vtimch7 TimerCh7ISR(void) 
{
  TFLG1_C7F = 1;
  
  if (TimerCh7DebugEnable)
  {    
    DDRT_DDRT6 = 1;
    PTT_PTT6 = !PTT_PTT6;  
  }
  
  if (timerCh7Routine)
  {
    timerCh7Routine(TIMER_Ch7);
  }
}

/*
void interrupt VectorNumber_Vtimch7 Timer_Ch7ISR(void) 
{
  TFLG1_C7F = 1;
  TC7 += 1000;
  DDRT_DDRT6 = 1;
  PTT_PTT6 = !PTT_PTT6;  
}
*/

void Timer_SetupPeriodicTimer(const UINT16 microSeconds, const UINT32 busClk)
{
  MCCTL_MCEN = 0;
  MCCTL_MODMC = 1;
  MCCTL_MCEN = 1;
  MCCNT = (word)((busClk / MATH_1_MEGA) * microSeconds);  
  MCCTL_FLMC = 1;
  MCCTL_MCPR = 0; // prescale 0
  //MCCTL_MCZI = 1;
}

void Timer_PeriodicTimerEnable(const BOOL enable)
{
  //MCCTL_MCEN = (byte) enable;  
  MCCTL_MCZI = (byte) enable;
}

void Timer_Setup(void) {
  TSCR1_TEN = 1;
  TSCR2_PR = 0; // prescale 0
  //TC7 = TCNT + 1000;
}

void Timer_Init(const TTimerChannel channelNb, const TTimerSetup * const aTimerSetup)
{
  if (aTimerSetup)
  {
    switch(channelNb)
    {
      /* timer channel 0 */
      case TIMER_Ch0:
        TIOS_IOS0 = (byte)aTimerSetup->outputCompare;
        switch(aTimerSetup->outputAction)
        {
          case TIMER_OUTPUT_DISCONNECT:
            TCTL2_OM0 = 0;
            TCTL2_OL0 = 0;
            break;
          case TIMER_OUTPUT_TOGGLE:
            TCTL2_OM0 = 0;
            TCTL2_OL0 = 1;
            break;
          case TIMER_OUTPUT_LOW:
            TCTL2_OM0 = 1;
            TCTL2_OL0 = 0;
            break;
          case TIMER_OUTPUT_HIGH:
            TCTL2_OM0 = 1;
            TCTL2_OL0 = 1;
            break;
          default:
            break;
        }
        switch(aTimerSetup->inputDetection)
        {
          case TIMER_INPUT_OFF:
            TCTL4_EDG0B = 0;
            TCTL4_EDG0A = 0;
            break;
          case TIMER_INPUT_RISING:
            TCTL4_EDG0B = 0;
            TCTL4_EDG0A = 1;
            break;
          case TIMER_INPUT_FALLING:
            TCTL4_EDG0B = 1;
            TCTL4_EDG0A = 0;
            break;
          case TIMER_INPUT_ANY:
            TCTL4_EDG0B = 1;
            TCTL4_EDG0A = 1;
            break;
          default:
            break;
        }
        TTOV_TOV0 = (byte)aTimerSetup->toggleOnOverflow;
        Timer_Enable(channelNb, aTimerSetup->interruptEnable);
        ICPAR_PA0EN = (byte)aTimerSetup->pulseAccumulator;
        break;
      /* timer channel 1 */
      case TIMER_Ch1:
        TIOS_IOS1 = (byte)aTimerSetup->outputCompare;
        switch(aTimerSetup->outputAction)
        {
          case TIMER_OUTPUT_DISCONNECT:
            TCTL2_OM1 = 0;
            TCTL2_OL1 = 0;
            break;
          case TIMER_OUTPUT_TOGGLE:
            TCTL2_OM1 = 0;
            TCTL2_OL1 = 1;
            break;
          case TIMER_OUTPUT_LOW:
            TCTL2_OM1 = 1;
            TCTL2_OL1 = 0;
            break;
          case TIMER_OUTPUT_HIGH:
            TCTL2_OM1 = 1;
            TCTL2_OL1 = 1;
            break;
          default:
            break;
        }
        switch(aTimerSetup->inputDetection)
        {
          case TIMER_INPUT_OFF:
            TCTL4_EDG1B = 0;
            TCTL4_EDG1A = 0;
            break;
          case TIMER_INPUT_RISING:
            TCTL4_EDG1B = 0;
            TCTL4_EDG1A = 1;
            break;
          case TIMER_INPUT_FALLING:
            TCTL4_EDG1B = 1;
            TCTL4_EDG1A = 0;
            break;
          case TIMER_INPUT_ANY:
            TCTL4_EDG1B = 1;
            TCTL4_EDG1A = 1;
            break;
          default:
            break;
        }
        TTOV_TOV1 = (byte)aTimerSetup->toggleOnOverflow;
        Timer_Enable(channelNb, aTimerSetup->interruptEnable);
        ICPAR_PA1EN = (byte)aTimerSetup->pulseAccumulator;
        break;
      /* timer channel 2 */
      case TIMER_Ch2:
        TIOS_IOS2 = (byte)aTimerSetup->outputCompare;
        switch(aTimerSetup->outputAction)
        {
          case TIMER_OUTPUT_DISCONNECT:
            TCTL2_OM2 = 0;
            TCTL2_OL2 = 0;
            break;
          case TIMER_OUTPUT_TOGGLE:
            TCTL2_OM2 = 0;
            TCTL2_OL2 = 1;
            break;
          case TIMER_OUTPUT_LOW:
            TCTL2_OM2 = 1;
            TCTL2_OL2 = 0;
            break;
          case TIMER_OUTPUT_HIGH:
            TCTL2_OM2 = 1;
            TCTL2_OL2 = 1;
            break;
          default:
            break;
        }
        switch(aTimerSetup->inputDetection)
        {
          case TIMER_INPUT_OFF:
            TCTL4_EDG2B = 0;
            TCTL4_EDG2A = 0;
            break;
          case TIMER_INPUT_RISING:
            TCTL4_EDG2B = 0;
            TCTL4_EDG2A = 1;
            break;
          case TIMER_INPUT_FALLING:
            TCTL4_EDG2B = 1;
            TCTL4_EDG2A = 0;
            break;
          case TIMER_INPUT_ANY:
            TCTL4_EDG2B = 1;
            TCTL4_EDG2A = 1;
            break;
          default:
            break;
        }
        TTOV_TOV2 = (byte)aTimerSetup->toggleOnOverflow;
        Timer_Enable(channelNb, aTimerSetup->interruptEnable);
        ICPAR_PA1EN = (byte)aTimerSetup->pulseAccumulator;
        break;
      /* timer channel 3 */
      case TIMER_Ch3:
        TIOS_IOS3 = (byte)aTimerSetup->outputCompare;
        switch(aTimerSetup->outputAction)
        {
          case TIMER_OUTPUT_DISCONNECT:
            TCTL2_OM3 = 0;
            TCTL2_OL3 = 0;
            break;
          case TIMER_OUTPUT_TOGGLE:
            TCTL2_OM3 = 0;
            TCTL2_OL3 = 1;
            break;
          case TIMER_OUTPUT_LOW:
            TCTL2_OM3 = 1;
            TCTL2_OL3 = 0;
            break;
          case TIMER_OUTPUT_HIGH:
            TCTL2_OM3 = 1;
            TCTL2_OL3 = 1;
            break;
          default:
            break;
        }
        switch(aTimerSetup->inputDetection)
        {
          case TIMER_INPUT_OFF:
            TCTL4_EDG3B = 0;
            TCTL4_EDG3A = 0;
            break;
          case TIMER_INPUT_RISING:
            TCTL4_EDG3B = 0;
            TCTL4_EDG3A = 1;
            break;
          case TIMER_INPUT_FALLING:
            TCTL4_EDG3B = 1;
            TCTL4_EDG3A = 0;
            break;
          case TIMER_INPUT_ANY:
            TCTL4_EDG3B = 1;
            TCTL4_EDG3A = 1;
            break;
          default:
            break;
        }
        TTOV_TOV3 = (byte)aTimerSetup->toggleOnOverflow;
        Timer_Enable(channelNb, aTimerSetup->interruptEnable);
        ICPAR_PA3EN = (byte)aTimerSetup->pulseAccumulator;
        break;
      /* timer channel 4 */
      case TIMER_Ch4:
        TIOS_IOS4 = (byte)aTimerSetup->outputCompare;
        switch(aTimerSetup->outputAction)
        {
          case TIMER_OUTPUT_DISCONNECT:
            TCTL1_OM4 = 0;
            TCTL1_OL4 = 0;
            break;
          case TIMER_OUTPUT_TOGGLE:
            TCTL1_OM4 = 0;
            TCTL1_OL4 = 1;
            break;
          case TIMER_OUTPUT_LOW:
            TCTL1_OM4 = 1;
            TCTL1_OL4 = 0;
            break;
          case TIMER_OUTPUT_HIGH:
            TCTL1_OM4 = 1;
            TCTL1_OL4 = 1;
            break;
          default:
            break;
        }
        switch(aTimerSetup->inputDetection)
        {
          case TIMER_INPUT_OFF:
            TCTL3_EDG4B = 0;
            TCTL3_EDG4A = 0;
            break;
          case TIMER_INPUT_RISING:
            TCTL3_EDG4B = 0;
            TCTL3_EDG4A = 1;
            break;
          case TIMER_INPUT_FALLING:
            TCTL3_EDG4B = 1;
            TCTL3_EDG4A = 0;
            break;
          case TIMER_INPUT_ANY:
            TCTL3_EDG4B = 1;
            TCTL3_EDG4A = 1;
            break;
          default:
            break;
        }
        TTOV_TOV4 = (byte)aTimerSetup->toggleOnOverflow;
        Timer_Enable(channelNb, aTimerSetup->interruptEnable);
        /* we don't have pulse accumulator on channel 4 */
        break;
      /* timer channel 5 */
      case TIMER_Ch5:
        TIOS_IOS5 = (byte)aTimerSetup->outputCompare;
        switch(aTimerSetup->outputAction)
        {
          case TIMER_OUTPUT_DISCONNECT:
            TCTL1_OM5 = 0;
            TCTL1_OL5 = 0;
            break;
          case TIMER_OUTPUT_TOGGLE:
            TCTL1_OM5 = 0;
            TCTL1_OL5 = 1;
            break;
          case TIMER_OUTPUT_LOW:
            TCTL1_OM5 = 1;
            TCTL1_OL5 = 0;
            break;
          case TIMER_OUTPUT_HIGH:
            TCTL1_OM5 = 1;
            TCTL1_OL5 = 1;
            break;
          default:
            break;
        }
        switch(aTimerSetup->inputDetection)
        {
          case TIMER_INPUT_OFF:
            TCTL3_EDG5B = 0;
            TCTL3_EDG5A = 0;
            break;
          case TIMER_INPUT_RISING:
            TCTL3_EDG5B = 0;
            TCTL3_EDG5A = 1;
            break;
          case TIMER_INPUT_FALLING:
            TCTL3_EDG5B = 1;
            TCTL3_EDG5A = 0;
            break;
          case TIMER_INPUT_ANY:
            TCTL3_EDG5B = 1;
            TCTL3_EDG5A = 1;
            break;
          default:
            break;
        }
        TTOV_TOV5 = (byte)aTimerSetup->toggleOnOverflow;
        Timer_Enable(channelNb, aTimerSetup->interruptEnable);
        /* we don't have pulse accumulator on channel 5 */
        break;
      /* timer channel 6 */        
      case TIMER_Ch6:
        TIOS_IOS6 = (byte)aTimerSetup->outputCompare;
        switch(aTimerSetup->outputAction)
        {
          case TIMER_OUTPUT_DISCONNECT:
            TCTL1_OM6 = 0;
            TCTL1_OL6 = 0;
            break;
          case TIMER_OUTPUT_TOGGLE:
            TCTL1_OM6 = 0;
            TCTL1_OL6 = 1;
            break;
          case TIMER_OUTPUT_LOW:
            TCTL1_OM6 = 1;
            TCTL1_OL6 = 0;
            break;
          case TIMER_OUTPUT_HIGH:
            TCTL1_OM6 = 1;
            TCTL1_OL6 = 1;
            break;
          default:
            break;
        }
        switch(aTimerSetup->inputDetection)
        {
          case TIMER_INPUT_OFF:
            TCTL3_EDG6B = 0;
            TCTL3_EDG6A = 0;
            break;
          case TIMER_INPUT_RISING:
            TCTL3_EDG6B = 0;
            TCTL3_EDG6A = 1;
            break;
          case TIMER_INPUT_FALLING:
            TCTL3_EDG6B = 1;
            TCTL3_EDG6A = 0;
            break;
          case TIMER_INPUT_ANY:
            TCTL3_EDG6B = 1;
            TCTL3_EDG6A = 1;
            break;
          default:
            break;
        }
        TTOV_TOV6 = (byte)aTimerSetup->toggleOnOverflow;
        Timer_Enable(channelNb, aTimerSetup->interruptEnable);
        /* we don't have pulse accumulator on channel 6 */
        break;        
      /* timer channel 7 */        
      case TIMER_Ch7:
        TIOS_IOS7 = (byte)aTimerSetup->outputCompare;
        switch(aTimerSetup->outputAction)
        {
          case TIMER_OUTPUT_DISCONNECT:
            TCTL1_OM7 = 0;
            TCTL1_OL7 = 0;
            break;
          case TIMER_OUTPUT_TOGGLE:
            TCTL1_OM7 = 0;
            TCTL1_OL7 = 1;
            break;
          case TIMER_OUTPUT_LOW:
            TCTL1_OM7 = 1;
            TCTL1_OL7 = 0;
            break;
          case TIMER_OUTPUT_HIGH:
            TCTL1_OM7 = 1;
            TCTL1_OL7 = 1;
            break;
          default:
            break;
        }
        switch(aTimerSetup->inputDetection)
        {
          case TIMER_INPUT_OFF:
            TCTL3_EDG7B = 0;
            TCTL3_EDG7A = 0;
            break;
          case TIMER_INPUT_RISING:
            TCTL3_EDG7B = 0;
            TCTL3_EDG7A = 1;
            break;
          case TIMER_INPUT_FALLING:
            TCTL3_EDG7B = 1;
            TCTL3_EDG7A = 0;
            break;
          case TIMER_INPUT_ANY:
            TCTL3_EDG7B = 1;
            TCTL3_EDG7A = 1;
            break;
          default:
            break;
        }
        TTOV_TOV7 = (byte)aTimerSetup->toggleOnOverflow;
        Timer_Enable(channelNb, aTimerSetup->interruptEnable);
        /* we don't have pulse accumulator on channel 7 */
        break;                
      default:
        break;
    }
  }
}

void Timer_Enable(const TTimerChannel channelNb, const BOOL enableInt) {
  switch(channelNb)
  {
    case TIMER_Ch0:
      TIE_C0I = (byte)enableInt;
      break;
    case TIMER_Ch1:
      TIE_C1I = (byte)enableInt;
      break;
    case TIMER_Ch2:
      TIE_C2I = (byte)enableInt;
      break;
    case TIMER_Ch3:
      TIE_C3I = (byte)enableInt;
      break;
    case TIMER_Ch4:
      TIE_C4I = (byte)enableInt;
      break;
    case TIMER_Ch5:
      TIE_C5I = (byte)enableInt;
      break;
    case TIMER_Ch6:
      TIE_C6I = (byte)enableInt;
      break;
    case TIMER_Ch7:
      TIE_C7I = (byte)enableInt;
      break;
    default:
      break;
  }
}

BOOL Timer_Enabled(const TTimerChannel channelNb)
{
  switch(channelNb)
  {
    case TIMER_Ch0:
      return (BOOL)TIE_C0I;
      break;
    case TIMER_Ch1:
      return (BOOL)TIE_C1I;
      break;
    case TIMER_Ch2:
      return (BOOL)TIE_C2I;
      break;
    case TIMER_Ch3:
      return (BOOL)TIE_C3I;
      break;
    case TIMER_Ch4:
      return (BOOL)TIE_C4I;
      break;
    case TIMER_Ch5:
      return (BOOL)TIE_C5I;
      break;
    case TIMER_Ch6:
      return (BOOL)TIE_C6I;
      break;
    case TIMER_Ch7:
      return (BOOL)TIE_C7I;
      break;
    default:
      break;
  }
  return bFALSE;  
}

void Timer_Set(const TTimerChannel channelNb, const UINT16 busClkCyclesDelay) {
  switch(channelNb)
  {
    case TIMER_Ch0:
      TC0 = TCNT + (word)busClkCyclesDelay;
      break;
    case TIMER_Ch1:
      TC1 = TCNT + (word)busClkCyclesDelay;
      break;
    case TIMER_Ch2:
      TC2 = TCNT + (word)busClkCyclesDelay;
      break;
    case TIMER_Ch3:
      TC3 = TCNT + (word)busClkCyclesDelay;
      break;
    case TIMER_Ch4:
      TC4 = TCNT + (word)busClkCyclesDelay;
      break;
    case TIMER_Ch5:
      TC5 = TCNT + (word)busClkCyclesDelay;
      break;
    case TIMER_Ch6:
      TC6 = TCNT + (word)busClkCyclesDelay;
      break;
    case TIMER_Ch7:
      TC7 = TCNT + (word)busClkCyclesDelay;
      break;
    default:
      break;
  }

}

void Timer_AttachRoutine(const TTimerChannel channelNb, TTimerRoutine const routinePtr)
{
  switch(channelNb)
  {
    case TIMER_Ch0:
      timerCh0Routine = routinePtr;
      break;
    case TIMER_Ch1:
      timerCh1Routine = routinePtr;
      break;
    case TIMER_Ch2:
      timerCh2Routine = routinePtr;
      break;
    case TIMER_Ch3:
      timerCh3Routine = routinePtr;
      break;
    case TIMER_Ch4:
      timerCh4Routine = routinePtr;
      break;
    case TIMER_Ch5:
      timerCh5Routine = routinePtr;
      break;
    case TIMER_Ch6:
      timerCh6Routine = routinePtr;
      break;
    case TIMER_Ch7:
      timerCh7Routine = routinePtr;
      break;      
    default:
      break;
  }
}

void Timer_DetachRoutine(const TTimerChannel channelNb)
{
  switch(channelNb)
  {
    case TIMER_Ch0:
      timerCh0Routine = (TTimerRoutine) 0x0000;
      break;
    case TIMER_Ch1:
      timerCh1Routine = (TTimerRoutine) 0x0000;
      break;
    case TIMER_Ch2:
      timerCh2Routine = (TTimerRoutine) 0x0000;
      break;
    case TIMER_Ch3:
      timerCh3Routine = (TTimerRoutine) 0x0000;
      break;
    case TIMER_Ch4:
      timerCh4Routine = (TTimerRoutine) 0x0000;
      break;
    case TIMER_Ch5:
      timerCh5Routine = (TTimerRoutine) 0x0000;
      break;
    case TIMER_Ch6:
      timerCh6Routine = (TTimerRoutine) 0x0000;
      break;
    case TIMER_Ch7:
      timerCh7Routine = (TTimerRoutine) 0x0000;
      break;      
    default:
      break;
  }
}
