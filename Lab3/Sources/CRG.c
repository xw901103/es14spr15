#include "CRG.h"
#include "mc9s12a512.h"

/* watchdog reset sequence values */
const UINT8 COP_ARM    = 0x55;
const UINT8 COP_DISARM = 0xAA;

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

void CRG_ArmCOP(void)
{
  ARMCOP = COP_ARM;    
}

void CRG_DisarmCOP(void)
{
  ARMCOP = COP_DISARM;    
}

void CRG_ResetCOP(void)
{
  __RESET_WATCHDOG();   
}
