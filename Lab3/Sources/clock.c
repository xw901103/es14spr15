/**
 * \file clock.c
 * \brief Routines for maintaining a real-time clock
 * \author Xu Waycell
 * \date 6-September-2014
 */
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
  if (++count == 8)
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

/**
 * \fn void Clock_Setup(const UINT8 prescaleRate, const UINT8 modulusCount)
 * \brief Initializes the clock.
 * \param prescaleRate The desired prescale rate
 * \param modulusCount modulus count of the CRG block's real-time interrupt module
 * \return TRUE if the clock was setup successfully.
 * \warning Given parameters must be able to trigger real-time interrupt occur every 65536 microseconds.
 */
BOOL Clock_Setup(const UINT8 prescaleRate, const UINT8 modulusCount)
{
  UINT8 divider = (prescaleRate << 4) | modulusCount;
  CRGINT_RTIE = 0;       /* real-time interrupt enable 1= on 0= off */
  RTICTL_RTR  = divider; /* real-time prescale and counter selection see CRG data sheet */
  CRGINT_RTIE = 1;       /* real-time interrupt enable 1= on 0= off */
  return RTICTL_RTR == divider;
}

/**
 * \fn BOOL Clock_Update(void)
 * \brief Updates the clock by converting milliseconds and microseconds into seconds and minutes.
 * \return TRUE if clock seconds have changed
 * \warning Assumes that the clock has been set up
 */
BOOL Clock_Update(void)
{
  UINT8 savedCCR;
  BOOL updated = bFALSE;
  
  /* check if one second passed */  
  while (ClockMicroSeconds >= MATH_1_MEGA)
  {
    ++Clock_Seconds;    
    /* check if one minute passed */
    if (Clock_Seconds >= 60)
    {
      ++Clock_Minutes;
      Clock_Seconds -= 60;
    }    
    EnterCritical();
    ClockMicroSeconds -= MATH_1_MEGA;    
    ExitCritical();    
    updated = bTRUE;
  } 
  return updated;  
}
