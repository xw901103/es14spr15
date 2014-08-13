/**
 * \file CRG.h
 * \brief Routines for setting up the clock and reset generator
 * \author Xu Waycell
 * \date 13-August-2014
 */
#ifndef CRG_H
#define CRG_H

#include "global.h"

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

/**
 * \fn BOOL CRG_SetupPLL(const UINT32 busClk, const UINT32 oscClk, const UINT32 refClk)
 * \brief Sets up the PLL to generate a certain bus clock.
 * \param busClk the desired bus clock rate in Hz
 * \param oscClk the oscillator clock in Hz 
 * \param refClk the reference clock in Hz
 * \return TRUE if the bus clock was setup successfully
 * \warning Assumes that refClk divides oscClk evenly
 * \warning Assumes that refClk divides busClk evenly
 */
BOOL CRG_SetupPLL(const UINT32 busClk, const UINT32 oscClk, const UINT32 refClk);

/**
 * \fn BOOL CRG_SetupCOP(const TCOPRate aCOPRate)
 * \brief Sets up the COP to reset within a certain number of milliseconds.
 * \param aCOPRate the desired COP rate, corresponding to Table 3.3 in the CRG Block User Guide
 * \return TRUE if the COP was setup successfully
 */
BOOL CRG_SetupCOP(const TCOPRate aCOPRate);

void CRG_ArmCOP(void);
void CRG_DisarmCOP(void);
void CRG_ResetCOP(void);

#endif