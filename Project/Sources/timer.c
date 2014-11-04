/**
 * \file timer.c
 * \brief Routines to implement general purpose timer and support the modulus down-counter as a periodic timer
 * \author Xu Waycell
 * \date 8-September-2014 
 */
#include "timer.h"
#include "OS.h"
#include <mc9s12a512.h>

#ifdef NO_INTERRUPT
#error "Timer module depends on interrupt feature enabled."
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

static TTimerPeriodicTimerRoutine timerPeriodicTimerRoutinePtr = (TTimerPeriodicTimerRoutine) 0x0000;

static TTimerRoutine timerCh0RoutinePtr = (TTimerRoutine) 0x0000,
                     timerCh1RoutinePtr = (TTimerRoutine) 0x0000,
                     timerCh2RoutinePtr = (TTimerRoutine) 0x0000,
                     timerCh3RoutinePtr = (TTimerRoutine) 0x0000,
                     timerCh4RoutinePtr = (TTimerRoutine) 0x0000,
                     timerCh5RoutinePtr = (TTimerRoutine) 0x0000,
                     timerCh6RoutinePtr = (TTimerRoutine) 0x0000,
                     timerCh7RoutinePtr = (TTimerRoutine) 0x0000;

/**
 * \fn void interrupt VectorNumber_Vtimmdcu TimerPeriodicTimerISR(void)
 * \brief Timer modulus down counter interrupt service routine.
 * \note It will toggle Port T pin 4 if modulus down counter is on.
 */
void interrupt VectorNumber_Vtimmdcu TimerPeriodicTimerISR(void);

/**
 * \fn void interrupt VectorNumber_Vtimch0 TimerCh0ISR(void)
 * \brief Timer channel number 0 interrupt service routine
 */
void interrupt VectorNumber_Vtimch0 TimerCh0ISR(void); 

/**
 * \fn void interrupt VectorNumber_Vtimch1 TimerCh1ISR(void)
 * \brief Timer channel number 1 interrupt service routine
 */
void interrupt VectorNumber_Vtimch1 TimerCh1ISR(void); 

/**
 * \fn void interrupt VectorNumber_Vtimch2 TimerCh2ISR(void)
 * \brief Timer channel number 2 interrupt service routine
 */
void interrupt VectorNumber_Vtimch2 TimerCh2ISR(void); 

/**
 * \fn void interrupt VectorNumber_Vtimch3 TimerCh3ISR(void)
 * \brief Timer channel number 3 interrupt service routine
 */
void interrupt VectorNumber_Vtimch3 TimerCh3ISR(void); 

/**
 * \fn void interrupt VectorNumber_Vtimch4 TimerCh4ISR(void)
 * \brief Timer channel number 4 interrupt service routine
 */
void interrupt VectorNumber_Vtimch4 TimerCh4ISR(void); 

/**
 * \fn void interrupt VectorNumber_Vtimch5 TimerCh5ISR(void)
 * \brief Timer channel number 5 interrupt service routine
 */
void interrupt VectorNumber_Vtimch5 TimerCh5ISR(void); 

/**
 * \fn void interrupt VectorNumber_Vtimch6 TimerCh6ISR(void)
 * \brief Timer channel number 6 interrupt service routine
 */
void interrupt VectorNumber_Vtimch6 TimerCh6ISR(void); 

/**
 * \fn void interrupt VectorNumber_Vtimch7 TimerCh7ISR(void)
 * \brief Timer channel number 7 interrupt service routine
 * \note it will toggle Port T pin 6 output if the debug is on
 */
void interrupt VectorNumber_Vtimch7 TimerCh7ISR(void); 

void interrupt VectorNumber_Vtimmdcu TimerPeriodicTimerISR(void) 
{
  /* clear modulus down counter underflow flag */
  MCFLG_MCZF = 1;
  
  OS_ISREnter();

  /* check if debug is on */
  if (ModulusDownCounterDebugEnable)
  {
    /* set data direction register to output */
    DDRT_DDRT4 = 1;
    /* toggle pin 4 */
    PTT_PTT4 = !PTT_PTT4;
  }
  
  if (timerPeriodicTimerRoutinePtr)
  {    
    timerPeriodicTimerRoutinePtr();
  }

  OS_ISRExit();
}

void interrupt VectorNumber_Vtimch0 TimerCh0ISR(void) 
{
  /* clear channel 0 interrupt flag */
  TFLG1_C0F = 1;

  OS_ISREnter();

  /* check if there is associated function to execute */
  if (timerCh0RoutinePtr)
  {
    /* execute attached function */
    timerCh0RoutinePtr(TIMER_Ch0);
  }

  OS_ISRExit();
}

void interrupt VectorNumber_Vtimch1 TimerCh1ISR(void) 
{
  /* clear channel 0 interrupt flag */
  TFLG1_C1F = 1;

  OS_ISREnter();

  /* check if there is associated function to execute */
  if (timerCh1RoutinePtr)
  {
    /* execute attached function */
    timerCh1RoutinePtr(TIMER_Ch1);
  }

  OS_ISRExit();
}

void interrupt VectorNumber_Vtimch2 TimerCh2ISR(void) 
{
  /* clear channel 0 interrupt flag */
  TFLG1_C2F = 1;

  OS_ISREnter();

  /* check if there is associated function to execute */
  if (timerCh2RoutinePtr)
  {
    /* execute attached function */
    timerCh2RoutinePtr(TIMER_Ch2);
  }

  OS_ISRExit();
}

void interrupt VectorNumber_Vtimch3 TimerCh3ISR(void) 
{
  /* clear channel 0 interrupt flag */
  TFLG1_C3F = 1;

  OS_ISREnter();

  /* check if there is associated function to execute */
  if (timerCh3RoutinePtr)
  {
    /* execute attached function */
    timerCh3RoutinePtr(TIMER_Ch3);
  }

  OS_ISRExit();
}

void interrupt VectorNumber_Vtimch4 TimerCh4ISR(void) 
{
  /* clear channel 0 interrupt flag */
  TFLG1_C4F = 1;

  OS_ISREnter();

  /* check if there is associated function to execute */
  if (timerCh4RoutinePtr)
  {
    /* execute attached function */
    timerCh4RoutinePtr(TIMER_Ch4);
  }

  OS_ISRExit();
}

void interrupt VectorNumber_Vtimch5 TimerCh5ISR(void) 
{
  /* clear channel 0 interrupt flag */
  TFLG1_C5F = 1;

  OS_ISREnter();

  /* check if there is associated function to execute */
  if (timerCh5RoutinePtr)
  {
    /* execute attached function */
    timerCh5RoutinePtr(TIMER_Ch5);
  }

  OS_ISRExit();
}

void interrupt VectorNumber_Vtimch6 TimerCh6ISR(void) 
{
  /* clear channel 0 interrupt flag */
  TFLG1_C6F = 1;

  OS_ISREnter();

  /* check if there is associated function to execute */
  if (timerCh6RoutinePtr)
  {
    /* execute attached function */
    timerCh6RoutinePtr(TIMER_Ch6);
  }

  OS_ISRExit();
}

void interrupt VectorNumber_Vtimch7 TimerCh7ISR(void) 
{
  /* clear channel 7 interrupt flag */
  TFLG1_C7F = 1;
  
  OS_ISREnter();

  /* check channel 7 debug enable */
  if (TimerCh7DebugEnable)
  { 
    /* set data direction register to output */   
    DDRT_DDRT6 = 1;
    /* toggle pin 6 */
    PTT_PTT6 = !PTT_PTT6;  
  }
  
  /* check if there is associated function to execute */
  if (timerCh7RoutinePtr)
  {
    /* execute attached function */
    timerCh7RoutinePtr(TIMER_Ch7);
  }

  OS_ISRExit();
}

/**
 * \fn void Timer_SetupPeriodicTimer(const UINT16 microSeconds, const UINT32 busClk)
 * \brief Sets the period of the periodic timer
 * \param microSeconds number of microseconds for one period
 * \param busClk actual bus clock rate in Hz
 * \see Timer_PeriodicTimerEnable
 */
void Timer_SetupPeriodicTimer(const UINT16 microSeconds, const UINT32 busClk)
{
  MCCTL_MCEN  = 0; /* modules down counter enable   1= on 0= off */
  MCCTL_MODMC = 1; /* */
  MCCTL_MCEN  = 1; /* modules down counter enable   1= on 0= off */
  // TODO: auto select suitable prescale rate
  MCCTL_MCPR  = 3; /* prescale rate see ECT data sheet */
  MCCNT       = (word)((busClk / MATH_1_MEGA) * microSeconds / 16); /* counter cycles */ 
  MCCTL_FLMC  = 1; /* force load counter register   1= on 0= off*/
  MCCTL_MCZI  = 0; /* modulus down interrupt enable 1= on 0= off */
}

/**
 * \fn void Timer_AttachPeriodicTimerRoutine(TTimerPeriodicTimerRoutine const routinePtr)
 * \param routinePtr
 * \brief Attach a routine to periodic timer interrupt service routine.
 */
void Timer_AttachPeriodicTimerRoutine(TTimerPeriodicTimerRoutine const routinePtr)
{
  timerPeriodicTimerRoutinePtr = routinePtr;
}

/**
 * \fn void Timer_DetachPeriodicTimerRoutine(void)
 * \brief Detach a routine to periodic timer interrupt service routine.
 */
void Timer_DetachPeriodicTimerRoutine(void)
{
  timerPeriodicTimerRoutinePtr = (TTimerPeriodicTimerRoutine) 0x0000;
}

/**
 * \fn void Timer_PeriodicTimerEnable(const BOOL enable)
 * \brief Enables or disables the periodic timer 
 * \param enableTimer boolean value indicating whether to enable the timer 
 * \see Timer_SetupPeriodicTimer
 * \warning Assumes the timer has been set up
 */
void Timer_PeriodicTimerEnable(const BOOL enableTimer)
{
  MCCTL_MCZI = (byte) enableTimer; /* modulus down counter interrupt enable 1= on 0= off */
}

/**
 * \fn void Timer_Setup(void)
 * \brief Sets up the Enhanced Capture Timer unit for operations with the timers
 * \see Timer_Init
 */
void Timer_Setup(void)
{
  TSCR1_TEN   = 0; /* timer enable                    1= on 0= off */
  TSCR1_TSWAI = 0; /* timer stop in wait mode         1= on 0= off */
  TSCR1_TSFRZ = 0; /* timer stop in freeze mode       1= on 0= off */
  TSCR1_TFFCA = 0; /* timer fast flag clear all       1= on 0= off */
  TSCR2_TCRE  = 0; /* timer counter reset enable      1= on 0= off */
  TSCR2_PR    = 0; /* timer prescale factor see table 3.5 of ECT   */
  TSCR2_TOI   = 0; /* timer overflow interrupt enable 1= on 0= off */
  TSCR1_TEN   = 1; /* timer enable                    1= on 0= off */
}

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
void Timer_Init(const TTimerChannel channelNb, const TTimerSetup * const aTimerSetup)
{
  if (aTimerSetup)
  {
    switch(channelNb)
    {
      /* timer channel 0 */
      case TIMER_Ch0:
        TIOS_IOS0 = (byte)aTimerSetup->outputCompare;
        /* setup output action */
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
        /* setup input detection */
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
        TTOV_TOV0   = (byte)aTimerSetup->toggleOnOverflow; /* toggle output compare on overflow 1= on 0=off */
        TIE_C0I     = (byte)aTimerSetup->interruptEnable;  /* channel 0 interrupt enable        1= on 0=off */
        ICPAR_PA0EN = (byte)aTimerSetup->pulseAccumulator; /* accumulator 0 enable              1= on 0=off */
        timerCh0RoutinePtr = aTimerSetup->routine;         /* set channel 0 routine */
        break;
      /* timer channel 1 */
      case TIMER_Ch1:
        TIOS_IOS1 = (byte)aTimerSetup->outputCompare;
        /* setup output action */
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
        /* setup input detection */
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
        TTOV_TOV1   = (byte)aTimerSetup->toggleOnOverflow; /* toggle output compare on overflow 1= on 0=off */
        TIE_C1I     = (byte)aTimerSetup->interruptEnable;  /* channel 1 interrupt enable        1= on 0=off */
        ICPAR_PA1EN = (byte)aTimerSetup->pulseAccumulator; /* accumulator 1 enable              1= on 0=off */
        timerCh1RoutinePtr = aTimerSetup->routine;         /* set channel 1 routine */
        break;
      /* timer channel 2 */
      case TIMER_Ch2:
        TIOS_IOS2 = (byte)aTimerSetup->outputCompare;
        /* setup output action */
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
        /* setup input detection */
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
        TTOV_TOV2   = (byte)aTimerSetup->toggleOnOverflow; /* toggle output compare on overflow 1= on 0=off */
        TIE_C2I     = (byte)aTimerSetup->interruptEnable;  /* channel 2 interrupt enable        1= on 0=off */
        ICPAR_PA2EN = (byte)aTimerSetup->pulseAccumulator; /* accumulator 2 enable              1= on 0=off */
        timerCh2RoutinePtr = aTimerSetup->routine;         /* set channel 2 routine */
        break;
      /* timer channel 3 */
      case TIMER_Ch3:
        TIOS_IOS3 = (byte)aTimerSetup->outputCompare;
        /* setup output action */
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
        /* setup input detection */
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
        TTOV_TOV3   = (byte)aTimerSetup->toggleOnOverflow; /* toggle output compare on overflow 1= on 0=off */
        TIE_C3I     = (byte)aTimerSetup->interruptEnable;  /* channel 3 interrupt enable        1= on 0=off */
        ICPAR_PA3EN = (byte)aTimerSetup->pulseAccumulator; /* accumulator 3 enable              1= on 0=off */
        timerCh3RoutinePtr = aTimerSetup->routine;         /* set channel 3 routine */
        break;
      /* timer channel 4 */
      case TIMER_Ch4:
        TIOS_IOS4 = (byte)aTimerSetup->outputCompare;
        /* setup output action */
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
        /* setup input detection */
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
        TTOV_TOV4 = (byte)aTimerSetup->toggleOnOverflow; /* toggle output compare on overflow 1= on 0=off */
        TIE_C4I   = (byte)aTimerSetup->interruptEnable;  /* channel 4 interrupt enable        1= on 0=off */
        /* we don't have pulse accumulator on channel 4 */
        timerCh4RoutinePtr = aTimerSetup->routine;       /* set channel 4 routine */
        break;
      /* timer channel 5 */
      case TIMER_Ch5:
        TIOS_IOS5 = (byte)aTimerSetup->outputCompare;
        /* setup output action */
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
        /* setup input detection */
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
        TTOV_TOV5 = (byte)aTimerSetup->toggleOnOverflow; /* toggle output compare on overflow 1= on 0=off */
        TIE_C5I   = (byte)aTimerSetup->interruptEnable;  /* channel 5 interrupt enable        1= on 0=off */
        /* we don't have pulse accumulator on channel 5 */
        timerCh5RoutinePtr = aTimerSetup->routine;       /* set channel 5 routine */
        break;
      /* timer channel 6 */        
      case TIMER_Ch6:
        TIOS_IOS6 = (byte)aTimerSetup->outputCompare;
        /* setup output action */
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
        /* setup input detection */
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
        TTOV_TOV6 = (byte)aTimerSetup->toggleOnOverflow; /* toggle output compare on overflow 1= on 0=off */
        TIE_C6I   = (byte)aTimerSetup->interruptEnable;  /* channel 6 interrupt enable        1= on 0=off */
        /* we don't have pulse accumulator on channel 6 */
        timerCh6RoutinePtr = aTimerSetup->routine;       /* set channel 6 routine */
        break;        
      /* timer channel 7 */        
      case TIMER_Ch7:
        TIOS_IOS7 = (byte)aTimerSetup->outputCompare;
        /* setup output action */
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
        /* setup input detection */
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
        TTOV_TOV7 = (byte)aTimerSetup->toggleOnOverflow; /* toggle output compare on overflow 1= on 0=off */
        TIE_C7I   = (byte)aTimerSetup->interruptEnable;  /* channel 7 interrupt enable        1= on 0=off */
        /* we don't have pulse accumulator on channel 7 */
        timerCh7RoutinePtr = aTimerSetup->routine;       /* set channel 7 routine */
        break;                
      default:
        break;
    }
  }
  else
  {
#ifndef NO_DEBUG
  DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif  
  }
}

/**
 * \fn void Timer_Enable(const TTimerChannel channelNb, const BOOL enableInt)
 * \brief Enables or disables a timer channel interrupt 
 * \param channelNb timer channel number
 * \param enableInt boolean value indicating whether to enable the interrupt on the timer channel
 * \see Timer_Enabled
 */
void Timer_Enable(const TTimerChannel channelNb, const BOOL enableInt)
{
  /* enable/disable selected timer channel interrupt */
  switch(channelNb)
  {
    case TIMER_Ch0:
      TIE_C0I = (byte)enableInt; /* timer channel 0 interrupt enable 1= on 0= off */
      break;
    case TIMER_Ch1:
      TIE_C1I = (byte)enableInt; /* timer channel 1 interrupt enable 1= on 0= off */
      break;
    case TIMER_Ch2:
      TIE_C2I = (byte)enableInt; /* timer channel 2 interrupt enable 1= on 0= off */
      break;
    case TIMER_Ch3:
      TIE_C3I = (byte)enableInt; /* timer channel 3 interrupt enable 1= on 0= off */
      break;
    case TIMER_Ch4:
      TIE_C4I = (byte)enableInt; /* timer channel 4 interrupt enable 1= on 0= off */
      break;
    case TIMER_Ch5:
      TIE_C5I = (byte)enableInt; /* timer channel 5 interrupt enable 1= on 0= off */
      break;
    case TIMER_Ch6:
      TIE_C6I = (byte)enableInt; /* timer channel 6 interrupt enable 1= on 0= off */
      break;
    case TIMER_Ch7:
      TIE_C7I = (byte)enableInt; /* timer channel 7 interrupt enable 1= on 0= off */
      break;
    default:
      break;
  }
}

/**
 * \fn BOOL Timer_Enabled(const TTimerChannel channelNb)
 * \brief Returns the status of a timer
 * \param channelNb timer channel number
 * \return a Boolean value indicating whether the channel is enabled
 * \see Timer_Enable
 */
BOOL Timer_Enabled(const TTimerChannel channelNb)
{
  /* return a boolean value of selected timer channel interrupt enable register */
  switch(channelNb)
  {
    case TIMER_Ch0:
      return (BOOL)TIE_C0I; /* timer channel 0 interrupt enable 1= on 0= off */
      break;
    case TIMER_Ch1:
      return (BOOL)TIE_C1I; /* timer channel 1 interrupt enable 1= on 0= off */
      break;
    case TIMER_Ch2:
      return (BOOL)TIE_C2I; /* timer channel 2 interrupt enable 1= on 0= off */
      break;
    case TIMER_Ch3:
      return (BOOL)TIE_C3I; /* timer channel 3 interrupt enable 1= on 0= off */
      break;
    case TIMER_Ch4:
      return (BOOL)TIE_C4I; /* timer channel 4 interrupt enable 1= on 0= off */
      break;
    case TIMER_Ch5:
      return (BOOL)TIE_C5I; /* timer channel 5 interrupt enable 1= on 0= off */
      break;
    case TIMER_Ch6:
      return (BOOL)TIE_C6I; /* timer channel 6 interrupt enable 1= on 0= off */
      break;
    case TIMER_Ch7:
      return (BOOL)TIE_C7I; /* timer channel 7 interrupt enable 1= on 0= off */
      break;
    default:
      break;
  }
  /* if this happened, go back to check your design */
  return bFALSE;  
}

/**
 * \fn void Timer_Set(const TTimerChannel channelNb, const UINT16 busClkCyclesDelay)
 * \brief Sets a timer channel to generate an interrupt after a certain number of bus clock cycles.
 * \param channelNb timer channel number
 * \param busClkCyclesDelay the number of bus clock cycles to wait until an interrupt is triggered
 * \see Timer_Enable
 * \see Timer_Enabled
 * \warning TCn = TCNT + busClkCyclesDelay
 */
void Timer_Set(const TTimerChannel channelNb, const UINT16 busClkCyclesDelay)
{
  /* assign the sum of free run counter and bus clock cycles to selected compare register */
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

/**
 * \fn void Timer_AttachRoutine(const TTimerChannel channelNb, TTimerRoutine const routinePtr)
 * \brief attach a function to given timer channel
 * \param channelNb timer channel number
 * \param routinePtr function pointer
 * \see Timer_DetachRoutine
 * \note timer channel interrupt will detach the routine if routinePtr is zero
 */
void Timer_AttachRoutine(const TTimerChannel channelNb, TTimerRoutine const routinePtr)
{
  /* set routine pointer to selected channel */
  switch(channelNb)
  {
    case TIMER_Ch0:
      timerCh0RoutinePtr = routinePtr;
      break;
    case TIMER_Ch1:
      timerCh1RoutinePtr = routinePtr;
      break;
    case TIMER_Ch2:
      timerCh2RoutinePtr = routinePtr;
      break;
    case TIMER_Ch3:
      timerCh3RoutinePtr = routinePtr;
      break;
    case TIMER_Ch4:
      timerCh4RoutinePtr = routinePtr;
      break;
    case TIMER_Ch5:
      timerCh5RoutinePtr = routinePtr;
      break;
    case TIMER_Ch6:
      timerCh6RoutinePtr = routinePtr;
      break;
    case TIMER_Ch7:
      timerCh7RoutinePtr = routinePtr;
      break;      
    default:
      break;
  }
}

/**
 * \fn void Timer_DetachRoutine(const TTimerChannel channelNb)
 * \brief detach function of given timer channel
 * \param channelNb timer channel number
 * \see Timer_AttachRoutine
 */
void Timer_DetachRoutine(const TTimerChannel channelNb)
{
  /* set null pointer to selected timer channel routine pointer */
  switch(channelNb)
  {
    case TIMER_Ch0:
      timerCh0RoutinePtr = (TTimerRoutine) 0x0000;
      break;
    case TIMER_Ch1:
      timerCh1RoutinePtr = (TTimerRoutine) 0x0000;
      break;
    case TIMER_Ch2:
      timerCh2RoutinePtr = (TTimerRoutine) 0x0000;
      break;
    case TIMER_Ch3:
      timerCh3RoutinePtr = (TTimerRoutine) 0x0000;
      break;
    case TIMER_Ch4:
      timerCh4RoutinePtr = (TTimerRoutine) 0x0000;
      break;
    case TIMER_Ch5:
      timerCh5RoutinePtr = (TTimerRoutine) 0x0000;
      break;
    case TIMER_Ch6:
      timerCh6RoutinePtr = (TTimerRoutine) 0x0000;
      break;
    case TIMER_Ch7:
      timerCh7RoutinePtr = (TTimerRoutine) 0x0000;
      break;      
    default:
      break;
  }
}

/**
 * \fn void Timer_ScheduleRoutine(const TTimerChannle channelNb, const UINT16 busClkCyclesDelay)
 * \brief Sets a timer channel to generate an interrupt to run attached routine after a certain number of bus clock cycles.
 * \param channelNb timer channel number
 * \param busClkCyclesDelay the number of bus clock cycles to wait until an interrupt is triggered
 * \warning TCn += busClkCyclesDelay 
 */
void Timer_ScheduleRoutine(const TTimerChannel channelNb, const UINT16 busClkCyclesDelay)
{
  /* add bus clock cycles to selected timer channel compare register */
  switch(channelNb)
  {
    case TIMER_Ch0:
      TC0 += busClkCyclesDelay;
      break;
    case TIMER_Ch1:
      TC1 += busClkCyclesDelay;
      break;
    case TIMER_Ch2:
      TC2 += busClkCyclesDelay;
      break;
    case TIMER_Ch3:
      TC3 += busClkCyclesDelay;
      break;
    case TIMER_Ch4:
      TC4 += busClkCyclesDelay;
      break;
    case TIMER_Ch5:
      TC5 += busClkCyclesDelay;
      break;
    case TIMER_Ch6:
      TC6 += busClkCyclesDelay;
      break;
    case TIMER_Ch7:
      TC7 += busClkCyclesDelay;
      break;      
    default:
      break;
  }
}
