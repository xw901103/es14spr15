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

#if !defined(COP_ARM) && !defined(COP_DISARM)
#define COP_ARM     0x55
#define COP_DISARM  0xAA
#endif

BOOL EEPROM_Setup(const UINT32 oscClk, const UINT32 busClk)
{
  UINT8  PRDIV8 = 0;
  UINT32 PRDCLK = 0;
  UINT8  EDIV = 0;
  if (!ECLKDIV_EDIVLD && busClk >= EEPROM_MINIMUM_BUSCLK)
  {
    PRDIV8 = 0;
    if (oscClk > EEPROM_CONDITION_OSCCLK)
    {
      PRDIV8 = 1;        
      PRDCLK = oscClk/8;
    }
    else
    {
      PRDCLK = oscClk;
    }
    if (((PRDCLK*5) % MATH_1_MEGA == 0) &&(PRDCLK % busClk == 0))
    {
      EDIV = (UINT8)((PRDCLK*5/MATH_1_MEGA)+PRDCLK/busClk-1);        
    }
    else
    {
      EDIV = (UINT8)((PRDCLK*5/MATH_1_MEGA)+PRDCLK/busClk);        
    }
    if ((MATH_1_MEGA*(1+EDIV)/PRDCLK+MATH_1_KILO/busClk) >= 5 && PRDCLK/(1+EDIV) >= EEPROM_MINIMUM_EECLK)
    {
      ECLKDIV_PRDIV8 = PRDIV8;
      ECLKDIV_EDIV = EDIV;
      return bTRUE;
    }
  }
  return bFALSE;    
}

BOOL EEPROM_ValidateAddress(UINT16 volatile * const address) {
  return (UINT16)address >= CONFIG_EEPROM_ADDRESS_BEGIN && (UINT16)address <= CONFIG_EEPROM_ADDRESS_END;
}

BOOL EEPROM_Command(UINT16 volatile * const address, const UINT16 data, UINT8 command)
{  
  if (ECLKDIV_EDIVLD && EEPROM_ValidateAddress(address))
  {      
    ESTAT_PVIOL = 1;  //clear PVIOL flag
    ESTAT_ACCERR = 1; //clear ACCERR flag
    while(!ESTAT_CBEIF)
    {
      // feed watch dog
      ARMCOP = COP_ARM;
      ARMCOP = COP_DISARM; 
    }
    if (address)
      *address = data;
    ECMD = command;
    ESTAT_CBEIF = 1;
    if (!ESTAT_PVIOL && !ESTAT_ACCERR)
    {
      while(!ESTAT_CCIF)
      {
        // feed watch dog
        ARMCOP = COP_ARM;
        ARMCOP = COP_DISARM; 
      }
      return bTRUE;
    }
  }
  return bFALSE;    
}

BOOL EEPROM_Write32(UINT32 volatile * const address, const UINT32 data)
{
  TUINT32 CACHE32;
  if ((UINT16)address%4 == 0)
  {
    
    CACHE32.l = data;
    return EEPROM_Command((UINT16 volatile * const)address, 0xFFFF, EEPROM_COMMAND_SECTOR_ERASE) &&
           EEPROM_Command((UINT16 volatile * const)address, CACHE32.s.Hi, EEPROM_COMMAND_PROGRAM) &&
           EEPROM_Command((UINT16 volatile * const)address+1, CACHE32.s.Lo, EEPROM_COMMAND_PROGRAM);    
  }
  return bFALSE;
}

BOOL EEPROM_Write16(UINT16 volatile * const address, const UINT16 data)
{
  UINT16 volatile * eepromAddress = address;
  TUINT32 cache;

  if ((UINT16)eepromAddress%2 == 0)
  { 
    /* alignment */  
    if ((UINT16)eepromAddress%4 != 0)
    {
      --eepromAddress; /* move 2 bytes to left */
      cache.l = *(UINT32 volatile *) eepromAddress;
      cache.s.Lo = data;
    }
    else
    {
      cache.l = *(UINT32 volatile *) eepromAddress;
      cache.s.Hi = data;
    }
    return EEPROM_Write32((UINT32 volatile * const)eepromAddress, cache.l); 
  }       
  return bFALSE;
}

BOOL EEPROM_Write8(UINT8 volatile * const address, const UINT8 data)
{
  TUINT16 cache;
  UINT8 volatile * eepromAddress = address;
  
  if ((UINT16)eepromAddress % 2 != 0) {
      --eepromAddress; /* move 1 byte to left */
      cache.l = *(UINT16 volatile *)eepromAddress;
      cache.s.Lo = data;      
  } else {
      cache.l = *(UINT16 volatile *)eepromAddress;
      cache.s.Hi = data;      
  }
  return EEPROM_Write16((UINT16 volatile * const)eepromAddress, cache.l);
} 

BOOL EEPROM_Erase(void)
{
  return EEPROM_Command((UINT16 volatile * const)0x0400, 0xFFFF, EEPROM_COMMAND_MASS_ERASE);
}
