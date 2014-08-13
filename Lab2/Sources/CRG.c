#include "CRG.h"
#include "mc9s12a512.h"

BOOL CRG_SetupPLL(const UINT32 busClk, const UINT32 oscClk, const UINT32 refClk) {
    UINT8 refDiv = 0, synReg = 0;

    if (!CLKSEL_PLLSEL) {        
        refDiv = (UINT8)(oscClk/refClk);
        synReg = (UINT8)((busClk*refDiv)/oscClk);
        if (CONFIG_BUSCLK_MAXIMUM >= (oscClk*synReg/refDiv)) { /* make sure that target bus clock is not harmful */            
            REFDV_REFDV = refDiv - 1;
            SYNR_SYN = synReg - 1;    
            PLLCTL_PLLON = 1;
            while(!CRGFLG_LOCK);                               /* poll until it is stable */
            CLKSEL_PLLSEL = 1;                                 /* select phase-locked loop as system clock */
            return bTRUE;
        }
    }
    return bFALSE;
}

BOOL CRG_SetupCOP(const TCOPRate aCOPRate) {
    return bFALSE;
}
