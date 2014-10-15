/**
 * \file clock.c
 * \brief Routines for maintaining a real-time clock
 * \author Xu Waycell
 * \date 6-September-2014
 */
#include "clock.h"
#include "CRG.h"
#include <mc9s12a512.h>

UINT8 Clock_Seconds = 0, Clock_Minutes = 0;
static UINT32 ClockMicroSeconds = 0;

#ifdef NO_INTERRUPT
#error "Real-time clock module depends on interrupt feature enabled."
#endif

/**
 * \fn void ClockRTISR(void)
 * \brief real time clock real time interrupt service routine
 * \note It will toggle Port T pin 5 if real time interrupt debug is on
 * \note It will toggle Port E pin 7 LED every half second
 */
void ClockRTISR(void);

void ClockRTISR(void)
{
  UINT8 savedCCR;
  static UINT8 count = 0; /* count of half a second beacon */
  
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
void Clock_Setup(const UINT8 prescaleRate, const UINT8 modulusCount)
{
  CRG_SetupRTI(prescaleRate, modulusCount);
  CRG_AttachRTIRoutine(&ClockRTISR);
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
