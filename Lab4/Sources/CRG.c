#include "CRG.h"
#include "mc9s12a512.h"

#if !defined(COP_ARM) && !defined(COP_DISARM)
#define COP_ARM     0x55
#define COP_DISARM  0xAA
#endif

BOOL CRG_SetupPLL(const UINT32 busClk, const UINT32 oscClk, const UINT32 refClk) {
    UINT8 refDiv = 0, synReg = 0;

    if (!CLKSEL_PLLSEL && CONFIG_BUSCLK_MAXIMUM >= busClk) {        
        refDiv = (UINT8)(oscClk/refClk);
        synReg = (UINT8)((busClk*refDiv)/oscClk);
        REFDV_REFDV = (byte)(refDiv - 1);
        SYNR_SYN = (byte)(synReg - 1);    
        PLLCTL_PLLON = 1;
        while(!CRGFLG_LOCK);                               /* poll until it is stable */
        CLKSEL_PLLSEL = 1;                                 /* select phase-locked loop as system clock */
        return bTRUE;
    }
    return bFALSE;
}

BOOL CRG_SetupCOP(const TCOPRate aCOPRate) {
    COPCTL_CR = (byte)aCOPRate;
#ifndef NO_DEBUG
    COPCTL_RSBCK = 1;
#endif
    return bTRUE;
}

void CRG_ArmCOP(void) {
    ARMCOP = COP_ARM;    
}

void CRG_DisarmCOP(void) {
    ARMCOP = COP_DISARM;    
}

void CRG_ResetCOP(void) {
//    __RESET_WATCHDOG();   
    ARMCOP = COP_ARM;    
    ARMCOP = COP_DISARM;    
}