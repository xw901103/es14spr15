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
    UINT8  PRDIV8 = 0;
    UINT32 PRDCLK = 0;
    UINT8  EDIV = 0;
    if (!ECLKDIV_EDIVLD && busClk >= EEPROM_MINIMUM_BUSCLK) {
        PRDIV8 = 0;
        if (oscClk > EEPROM_CONDITION_OSCCLK) {
            PRDIV8 = 1;        
            PRDCLK = oscClk/8;
        } else {
            PRDCLK = oscClk;
        }
        if (((PRDCLK*5) % MATH_1_MEGA == 0) &&(PRDCLK % busClk == 0)) {
            EDIV = (UINT8)((PRDCLK*5/MATH_1_MEGA)+PRDCLK/busClk-1);        
        } else {
            EDIV = (UINT8)((PRDCLK*5/MATH_1_MEGA)+PRDCLK/busClk);        
        }
        if ((MATH_1_MEGA*(1+EDIV)/PRDCLK+MATH_1_MEGA/busClk) >= 5 && PRDCLK/(1+EDIV) >= EEPROM_MINIMUM_EECLK) {
            ECLKDIV_PRDIV8 = PRDIV8;
            ECLKDIV_EDIV = EDIV;
            return bTRUE;
        }
    }
    return bFALSE;    
}

BOOL EEPROM_Command(UINT16 volatile * const address, const UINT16 data, UINT8 command) {  
    if (ECLKDIV_EDIVLD) {      
        ESTAT_PVIOL = 1;  //clear PVIOL flag
        ESTAT_ACCERR = 1; //clear ACCERR flag
        while(!ESTAT_CBEIF) {
            // feed watch dog 
        }
        if (address)
            *address = data;
        ECMD = command;
        ESTAT_CBEIF = 1;
        if (!ESTAT_PVIOL && !ESTAT_ACCERR) {
            while(!ESTAT_CCIF) {
                // feed watch dog
            }
            return bTRUE;
        }
    }
    return bFALSE;    
}

BOOL EEPROM_Write32(UINT32 volatile * const address, const UINT32 data) {
    TUINT32 CACHE32;
    CACHE32.l = data;
    return EEPROM_Command((UINT16 volatile * const)address, 0xFFFF, EEPROM_COMMAND_SECTOR_ERASE) &&
           EEPROM_Command((UINT16 volatile * const)address, CACHE32.s.Hi, EEPROM_COMMAND_PROGRAM) &&
           EEPROM_Command((UINT16 volatile * const)address+1, CACHE32.s.Lo, EEPROM_COMMAND_PROGRAM);    
/*
    if (ECLKDIV_EDIVLD) {      
        ESTAT = (ESTAT_PVIOL_MASK|ESTAT_ACCERR_MASK); 
        while(!ESTAT_CBEIF);
        if (address)
            *(UINT16 volatile *)address = (const UINT16)data;
        ECMD = EEPROM_COMMAND_SECTOR_MODIFY;
        ESTAT_CBEIF = 1;
        if (!ESTAT_PVIOL)
            if(!ESTAT_ACCERR) {
                while(!ESTAT_CCIF);
                return bTRUE;
            }
    }
 */    
//    return EEPROM_Program(address, 0xFFFF, EEPROM_COMMAND_SECTOR_ERASE) && EEPROM_Program(address, data, EEPROM_COMMAND_PROGRAM);    
}

BOOL EEPROM_Write16(UINT16 volatile * const address, const UINT16 data) {
    TUINT32 CACHE32;
    CACHE32.l = *(UINT32 volatile * const) address;
    CACHE32.s.Hi = data;
    return EEPROM_Command((UINT16 volatile * const)address, 0xFFFF, EEPROM_COMMAND_SECTOR_ERASE) &&
           EEPROM_Command((UINT16 volatile * const)address, CACHE32.s.Hi, EEPROM_COMMAND_PROGRAM) &&
           EEPROM_Command((UINT16 volatile * const)address+1, CACHE32.s.Lo, EEPROM_COMMAND_PROGRAM);    
}

BOOL EEPROM_Write8(UINT8 volatile * const address, const UINT8 data) {
    UINT8 volatile * EEPROM_ADDR = address;
    TUINT32 CACHE32;
    TUINT16 CACHE16;
    
    if ((UINT16)EEPROM_ADDR%2 == 0) {
        CACHE32.l = *(UINT32 volatile * const) EEPROM_ADDR;
        CACHE16.l = CACHE32.s.Hi;
        CACHE16.s.Hi = data;
        CACHE32.s.Hi = CACHE16.l;
    } else {
        --EEPROM_ADDR;
        CACHE32.l = *(UINT32 volatile * const) EEPROM_ADDR;
        CACHE16.l = CACHE32.s.Hi;
        CACHE16.s.Lo = data;
        CACHE32.s.Hi = CACHE16.l;
    }
    return EEPROM_Command((UINT16 volatile * const)EEPROM_ADDR, 0xFFFF, EEPROM_COMMAND_SECTOR_ERASE) &&
           EEPROM_Command((UINT16 volatile * const)EEPROM_ADDR, CACHE32.s.Hi, EEPROM_COMMAND_PROGRAM) &&
           EEPROM_Command((UINT16 volatile * const)EEPROM_ADDR+1, CACHE32.s.Lo, EEPROM_COMMAND_PROGRAM);    
}

BOOL EEPROM_Erase(void) {
    return EEPROM_Command((UINT16 volatile * const)0x0400, 0xFFFF, EEPROM_COMMAND_MASS_ERASE);
}
