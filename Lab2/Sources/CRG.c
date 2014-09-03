#include "CRG.h"
#include "mc9s12a512.h"

/* watchdog reset sequence values */
const UINT8 COP_ARM    = 0x55;
const UINT8 COP_DISARM = 0xAA;

BOOL CRG_SetupPLL(const UINT32 busClk, const UINT32 oscClk, const UINT32 refClk)
{
  UINT8 refDiv = 0, synReg = 0;

  if (!CLKSEL_PLLSEL && CONFIG_BUSCLK_MAXIMUM >= busClk)
  {        
    //refDiv = (UINT8)(oscClk/refClk);
    //synReg = (UINT8)((busClk*refDiv)/oscClk);
    //REFDV_REFDV = (byte)(refDiv - 1);
    //SYNR_SYN = (byte)(synReg - 1);
    REFDV_REFDV = (byte)(oscClk / refClk - 1);
    SYNR_SYN = (byte)(busClk / refClk - 1);
    //PPLCTL_CME  = 1;     /* Clock monitor enable     0= off  1= on */
    PLLCTL_PLLON  = 1;     /* PLL enable               0= off  1= on */
    //PLLCTL_AUTO = 1;     /* Auto bandwith control    0= off  1= on */
    //PLLCTL_ACQ  = 1;     /* Acquisition(affected by AUTO) 0= low  1= high */
    //PLLCTL_PRE  = 0;     /* RTI enable               0= off  1= on */
    //PLLCTL_PCE  = 0;     /* Watchdog pseudo stop bit 0= stop 1= cont. */
    //PLLCTL_SCME = 1;     /* Self clock mode enable   0= off  1= on */
    while(!CRGFLG_LOCK);  /* poll until it is stable */
    CLKSEL_PLLSEL = 1;    /* select phase-locked loop as system clock */
    return bTRUE;
  }
  return bFALSE;
}

BOOL CRG_SetupCOP(const TCOPRate aCOPRate)
{
  COPCTL_WCOP = 0;  /* window mode 0= off 1= on */
#ifndef NO_DEBUG
  COPCTL_RSBCK = 1; /* BDM mode    0= off 1= on*/
#endif
  COPCTL_CR = (byte)aCOPRate;
  return bTRUE;
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
