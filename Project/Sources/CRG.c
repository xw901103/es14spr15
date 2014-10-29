/**
 * \file CRG.c
 * \brief Routines for setting up the clock and reset generator
 * \author Xu Waycell
 * \date 13-August-2014
 */
#include "CRG.h"
#include <mc9s12a512.h>

/* watchdog reset sequence values */
const UINT8 COP_ARM    = 0x55;
const UINT8 COP_DISARM = 0xAA;

#ifndef NO_INTERRUPT
static TRTIRoutine rtiRoutine = (TRTIRoutine) 0x0000;

#ifndef CONFIG_EEPROM_ADDRESS_RTI_DEBUG
#define RTIDebugEnable *(volatile UINT16*)0x0420 /* fallback plan */
#warning "RTIDebugEnable using fallback setting 0x0420"
#else
#define RTIDebugEnable *(volatile UINT16*)CONFIG_EEPROM_ADDRESS_RTI_DEBUG
#endif

//void interrupt VectorNumber_Vrti CRGRTISR(void)
//{
  /* clear real time interrupt flag */
//  CRGFLG_RTIF = 1;

  /* check if debug is on */
//  if (RTIDebugEnable)
//  {
    /* set data direction register to output */
//    DDRT_DDRT5 = 1;
    /* toggle pin 5 */
//    PTT_PTT5 = !PTT_PTT5;
//  }

//  if (rtiRoutine)
//  {
//    rtiRoutine();
//  }
//}
#endif

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
BOOL CRG_SetupPLL(const UINT32 busClk, const UINT32 oscClk, const UINT32 refClk)
{
  UINT16 waitCount = 0;

  if (CONFIG_BUSCLK_MAXIMUM >= busClk)
  {    
    if (CLKSEL_PLLSEL)
    {
      CLKSEL_PLLSEL = 0;     /* de-select phase-locked loop as system clock */
      //PPLCTL_CME   = 1;     /* Clock monitor enable     0= off  1= on */
      //PLLCTL_PLLON = 0;     /* PLL enable               0= off  1= on */
      //PLLCTL_AUTO  = 1;     /* Auto bandwith control    0= off  1= on */
      //PLLCTL_ACQ   = 1;     /* Acquisition(affected by AUTO) 0= low  1= high */
      //PLLCTL_PRE   = 0;     /* RTI enable               0= off  1= on */
      //PLLCTL_PCE   = 0;     /* Watchdog pseudo stop bit 0= stop 1= cont. */
      //PLLCTL_SCME  = 1;     /* Self clock mode enable   0= off  1= on */
    }

    /* check if PLL is not selected as E_CLK source and bus clock is suitable */
    if (!CLKSEL_PLLSEL)
    {        
      REFDV_REFDV = (byte)(oscClk / refClk - 1);
      SYNR_SYN = (byte)(busClk / refClk - 1);
      //PPLCTL_CME  = 1;     /* Clock monitor enable     0= off  1= on */
      PLLCTL_PLLON  = 1;     /* PLL enable               0= off  1= on */
      //PLLCTL_AUTO = 1;     /* Auto bandwith control    0= off  1= on */
      //PLLCTL_ACQ  = 1;     /* Acquisition(affected by AUTO) 0= low  1= high */
      //PLLCTL_PRE  = 0;     /* RTI enable               0= off  1= on */
      //PLLCTL_PCE  = 0;     /* Watchdog pseudo stop bit 0= stop 1= cont. */
      //PLLCTL_SCME = 1;     /* Self clock mode enable   0= off  1= on */
    
      for (waitCount = 0; waitCount != 0xFFFF; ++waitCount)
      { 
        /* poll until it is stable */
        if (CRGFLG_LOCK)
        { 
          /* select phase-locked loop as system clock */
          CLKSEL_PLLSEL = 1;
          return bTRUE;
        }
      }
    }
  }
  return bFALSE;
}

/**
 * \fn BOOL CRG_SetupCOP(const TCOPRate aCOPRate)
 * \brief Sets up the COP to reset within a certain number of milliseconds.
 * \param aCOPRate the desired COP rate, corresponding to Table 3.3 in the CRG Block User Guide
 * \return TRUE if the COP was setup successfully
 */
BOOL CRG_SetupCOP(const TCOPRate aCOPRate)
{
  byte mask = (byte)aCOPRate;
  //mask |= COPCTL_WCOP_MASK; /* enable window mode */
#ifndef NO_DEBUG
  mask |= COPCTL_RSBCK_MASK; /* enable BDM mode */
#endif
  COPCTL = mask;
  return COPCTL == mask;
}

#ifndef NO_INTERRUPT
/**
 * \fn void CRG_SetupRTI(const UINT8 prescaleRate, const UINT8 modulusCount)
 * \brief Sets up the RTI as a periodic timer
 * \param prescaleRate The desired prescale rate
 * \param modulusCount modulus count of the CRG block's real-time interrupt module
 * \note Desired prescale rate and modulus count, corresponding to Table 3.2 in the CRG Block User Guide.
 */
void CRG_SetupRTI(const UINT8 prescaleRate, const UINT8 modulusCount)
{
  UINT8 divider = (prescaleRate << 4) | modulusCount;
  
  CRGINT_RTIE = 0;       /* real-time interrupt enable 1= on 0= off */
  RTICTL_RTR  = divider; /* real-time prescale and counter selection see CRG data sheet */
  CRGINT_RTIE = 1;       /* real-time interrupt enable 1= on 0= off */
}

void CRG_AttachRTIRoutine(TRTIRoutine routine)
{
  rtiRoutine = routine;
}
void CRG_DetachRTIRoutine()
{
  rtiRoutine = (TRTIRoutine) 0x0000;
}
#endif

/**
 * \fn void CRG_ArmCOP(void)
 * \brief Arm watchdog to start reset sequence.
 * \see CRG_DisarmCOP
 * \see CRG_ResetCOP
 */
void CRG_ArmCOP(void)
{
  ARMCOP = COP_ARM;    
}

/**
 * \fn void CRG_DisarmCOP(void)
 * \brief Disarm watchdog to finish reset sequence.
 * \see CRG_ArmCOP
 * \see CRG_ResetCOP
 */
void CRG_DisarmCOP(void)
{
  ARMCOP = COP_DISARM;    
}

/**
 * \fn void CRG_ResetCOP(void)
 * \brief Reset watchdog immediately.
 * \see CRG_ArmCOP
 * \see CRG_DisarmCOP
 */
void CRG_ResetCOP(void)
{
  __RESET_WATCHDOG();   
}
