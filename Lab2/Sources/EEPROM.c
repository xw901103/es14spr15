#include "EEPROM.h"
#include "mc9s12a512.h"

#define EEPROM_MINIMUM_BUSCLK   1000000   //1Mhz
#define EEPROM_CONDITION_OSCCLK  12800000  //12.8Mhz
#define EEPROM_MINIMUM_EECLK    150000    //0.15Mhz
#define EEPROM_MAXIMUM_EECLK    200000    //0.20Mhz
#define EEPROM_COMMAND_ERASE_VERIFY   0x05
#define EEPROM_COMMAND_PROGRAM        0x20
#define EEPROM_COMMAND_SECTOR_ERASE   0x40
#define EEPROM_COMMAND_MASS_ERASE     0x41
#define EEPROM_COMMAND_SECTOR_MODIFY  0x60

BOOL EEPROM_Setup(const UINT32 oscClk, const UINT32 busClk) {
    UINT32 PRDCLK = 0;
    UINT8  EDIV = 0;
    UINT8  CACHE = 0;
    if (busClk >= EEPROM_MINIMUM_BUSCLK) {
        ECLKDIV_PRDIV8 = 0;
        if (oscClk > EEPROM_CONDITION_OSCCLK) {
            ECLKDIV_PRDIV8 = 1;        
            PRDCLK = oscClk/8;
        } else {
            PRDCLK = oscClk;
        }
        if ((PRDCLK % busClk == 0)) {
            EDIV = (UINT8)((PRDCLK/MATH_1_MEGA)*5+PRDCLK/busClk-1);        
        } else {
            EDIV = (UINT8)((PRDCLK/MATH_1_MEGA)*5+PRDCLK/busClk);        
        }
        CACHE = (MATH_1_MEGA*(1+EDIV)/PRDCLK+MATH_1_MEGA/busClk);
        if ((MATH_1_MEGA*(1+EDIV)/PRDCLK+MATH_1_MEGA/busClk) >= 5 && PRDCLK/(1+EDIV) >= EEPROM_MINIMUM_EECLK) {
            ECLKDIV_EDIV = EDIV;
            return bTRUE;
        }
    }
    return bFALSE;    
}

BOOL EEPROM_Program(UINT8 volatile * const address, const UINT8 data, UINT8 command) {  
    if (ECLKDIV_EDIVLD) {      
        ESTAT_PVIOL = 1;  //clear PVIOL flag
        ESTAT_ACCERR = 1; //clear ACCERR flag
        *address = data;
        ECMD = command;
        ESTAT_CBEIF = 1;
        if (!ESTAT_PVIOL && !ESTAT_ACCERR) {
            while(!ESTAT_CCIF);
            return bTRUE;
        }
    }
    return bFALSE;    
}

BOOL EEPROM_Write32(UINT32 volatile * const address, const UINT32 data) {
    return bFALSE;    
}

BOOL EEPROM_Write16(UINT16 volatile * const address, const UINT16 data) {
    return bFALSE;    
}

BOOL EEPROM_Write8(UINT8 volatile * const address, const UINT8 data) {
    return bFALSE;    
}

BOOL EEPROM_Erase(void) {
    return EEPROM_Program((UINT32 volatile * const)0x0400, 0, EEPROM_COMMAND_MASS_ERASE);
}
