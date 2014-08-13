// ----------------------------------------
// Filename: CRG.h
// Description: Routines for setting up the
//   clock and reset generator
// Author: PMcL
// Date: 16-Mar-06

#ifndef CRG_H
#define CRG_H

// new types
#include "types.h"

typedef enum
{
  COP_DISABLED = 0,
  COP_RATE_2_14 = 1,
  COP_RATE_2_16 = 2,
  COP_RATE_2_18 = 3,
  COP_RATE_2_20 = 4,
  COP_RATE_2_22 = 5,
  COP_RATE_2_23 = 6,
  COP_RATE_2_24 = 7
} TCOPRate;

// ----------------------------------------
// CRG_SetupPLL
// 
// Sets up the PLL to generate a certain bus clock
// Input:
//   busClk is the desired bus clock rate in Hz,
//   oscClk is the oscillator clock in Hz, 
//   refClk is the reference clock in Hz
// Output:
//   TRUE if the bus clock was setup successfully
// Conditions:
//   Assumes that refClk divides oscClk evenly
//   Assumes that refClk divides busClk evenly

BOOL CRG_SetupPLL(const UINT32 busClk, const UINT32 oscClk, const UINT32 refClk);

// ----------------------------------------
// CRG_SetupCOP
// 
// Sets up the COP to reset within a certain
//   number of milliseconds
// Input:
//   Desired COP rate, corresponding to
//   Table 3.3 in the CRG Block User Guide
// Output:
//   TRUE if the COP was setup successfully
// Conditions:
//   none

BOOL CRG_SetupCOP(const TCOPRate aCOPRate);

// ----------------------------------------
// CRG_SetupRTI
// 
// Sets up the RTI as a periodic timer
// Input:
//   Desired prescale rate and modulus count,
//   corresponding to Table 3.2 in the CRG Block User Guide
// Output:
//   none
// Conditions:
//   none

void CRG_SetupRTI(const UINT8 prescaleRate, const UINT8 modulusCount);

#endif