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
    refDiv = (UINT8)(oscClk/refClk);
    synReg = (UINT8)((busClk*refDiv)/oscClk);
    REFDV_REFDV = (byte)(refDiv - 1);
    SYNR_SYN = (byte)(synReg - 1);    
    PLLCTL_PLLON = 1;
    while(!CRGFLG_LOCK);  /* poll until it is stable */
    CLKSEL_PLLSEL = 1;    /* select phase-locked loop as system clock */
    return bTRUE;
  }
  return bFALSE;
}

BOOL CRG_SetupCOP(const TCOPRate aCOPRate)
{
  COPCTL_WCOP = 0;  /* window mode */
#ifndef NO_DEBUG
  COPCTL_RSBCK = 1; /* BDM mode */
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
  /*
  ARMCOP = COP_ARM;    
  ARMCOP = COP_DISARM;    
   */
}
