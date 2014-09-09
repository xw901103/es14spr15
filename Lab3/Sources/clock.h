// ----------------------------------------
// Filename: clock.h
// Description: Routines for maintaining
//   a real-time clock
// Author: PMcL
// Date: 29-Mar-06

#ifndef CLOCK_H
#define CLOCK_H

// new types
#include "global.h"

extern UINT8 Clock_Seconds, Clock_Minutes;

/**
 * \fn void Clock_Setup(const UINT8 prescaleRate, const UINT8 modulusCount)
 * \brief Initializes the clock.
 * \param prescaleRate The desired prescale rate
 * \param modulusCount modulus count of the CRG block's real-time interrupt module
 * \return TRUE if the clock was setup successfully.
 */
BOOL Clock_Setup(const UINT8 prescaleRate, const UINT8 modulusCount);

/**
 * \fn BOOL Clock_Update(void)
 * \brief Updates the clock by converting milliseconds and microseconds into seconds and minutes.
 * \return TRUE if clock seconds have changed
 * \warning Assumes that the clock has been set up
 */
BOOL Clock_Update(void);

#endif