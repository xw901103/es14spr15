#include "clock.h"
#include <mc9s12a512.h>

UINT8 Clock_Seconds = 0, Clock_Minutes = 0;
static UINT32 ClockMicroSeconds = 0;

#ifdef NO_INTERRUPT
#error "Real-time clock module depends on interrupt feature enabled."
#endif

#ifndef CONFIG_EEPROM_ADDRESS_RTI_DEBUG
#define RTIDebugEnable *(volatile UINT16*)0x0420 /* fallback plan */
#warning "RTIDebugEnable using fallback setting 0x0420"
#else
#define RTIDebugEnable *(volatile UINT16*)CONFIG_EEPROM_ADDRESS_RTI_DEBUG
#endif

/**
 * \fn void interrupt VectorNumber_Vrti ClockRTISR(void)
 * \brief real time clock real time interrupt service routine
 * \note It will toggle Port T pin 5 if real time interrupt debug is on
 * \note It will toggle Port E pin 7 LED every half second
 */
void interrupt VectorNumber_Vrti ClockRTISR(void);

void interrupt VectorNumber_Vrti ClockRTISR(void)
{
  UINT8 savedCCR;
  static UINT8 count = 0; /* count of half a second beacon */
  
  /* clear real time interrupt flag */
  CRGFLG_RTIF = 1;
  
  /* check if debug is on */
  if (RTIDebugEnable)
  {
    /* set data direction register to output */
    DDRT_DDRT5 = 1;
    /* toggle pin 5 */
    PTT_PTT5 = !PTT_PTT5;
  }

  /* check if it is half second */
  if (++count == 0x08)
  {
    /* set data direction register to output */
    DDRE_BIT7 = 1;
    /* toggle pin 7 */
    PORTE_BIT7 = !PORTE_BIT7;
    count = 0;
  }
  
  EnterCritical();
  ClockMicroSeconds += 0x00010000; /* increase 65536 microseconds */
  ExitCritical();
}

BOOL Clock_Setup(const UINT8 prescaleRate, const UINT8 modulusCount)
{
  UINT8 divider = (prescaleRate << 4) | modulusCount;
  CRGINT_RTIE = 0;       /* real time interrupt enable 1= on 0= off */
  RTICTL_RTR  = divider; /* */
  CRGINT_RTIE = 1;       /* real time interrupt enable 1= on 0= off */
  return RTICTL_RTR == divider;
}

BOOL Clock_Update(void)
{
  UINT8 savedCCR;

  /* check if one second passed */  
  while (ClockMicroSeconds >= MATH_1_MEGA)
  {
    EnterCritical();
    ++Clock_Seconds;    
    /* check if one minute passed */
    if (Clock_Seconds >= 60)
    {
      ++Clock_Minutes;
      Clock_Seconds -= 60;
    }    
    ClockMicroSeconds -= MATH_1_MEGA;    
    ExitCritical();    
    return bTRUE;
  } 
  return bFALSE;  
}
