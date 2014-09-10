#include "clock.h"
#include <mc9s12a512.h>

UINT8 Clock_Seconds = 0, Clock_Minutes = 0;
static UINT32 Clock_MicroSeconds = 0;

#ifdef NO_INTERRUPT
#error "Clock module depends on interrupt."
#endif

#ifndef CONFIG_EEPROM_ADDRESS_RTI_DEBUG
#define RTIDebugEnable *(volatile UINT16*)0x0420 /* fallback plan */
#warning "RTIDebugEnable using fallback setting 0x0420"
#else
#define RTIDebugEnable *(volatile UINT16*)CONFIG_EEPROM_ADDRESS_RTI_DEBUG
#endif

void interrupt VectorNumber_Vrti RTI_ISR(void)
{
  UINT8 savedCCR;
  static UINT8 count = 0; /* count of half a second beacon */
  
  /* ack flag */
  CRGFLG_RTIF = 1;
  
  /* check if debug is on */
  if (RTIDebugEnable)
  {
    DDRT_DDRT5 = 1;
    PTT_PTT5 = !PTT_PTT5;
  }

  if (++count == 0x08)
  {
    PORTE_BIT7 = !PORTE_BIT7;
    count = 0;
  }
  
  EnterCritical();
  Clock_MicroSeconds += 0x00010000;
  ExitCritical();  

}

BOOL Clock_Setup(const UINT8 prescaleRate, const UINT8 modulusCount)
{
  UINT8 divider = (prescaleRate << 4) | modulusCount;
  CRGINT_RTIE = 0;
  RTICTL_RTR  = divider;
  DDRT_DDRT5  = 1; /* debug */
  CRGINT_RTIE = 1;
  return RTICTL_RTR == divider;
}

BOOL Clock_Update(void)
{
  UINT8 savedCCR;
  
  if (Clock_MicroSeconds >= MATH_1_MEGA)
  {
    EnterCritical();
    ++Clock_Seconds;
    if (Clock_Seconds >= 60)
    {
      ++Clock_Minutes;
      Clock_Seconds -= 60;
    }
    Clock_MicroSeconds -= MATH_1_MEGA;
    ExitCritical();
    return bTRUE;
  } 
  return bFALSE;  
}
