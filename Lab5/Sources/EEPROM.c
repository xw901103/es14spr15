/**
 * \file EEPROM.c
 * \brief Routines for erasing and writing to the EEPROM
 * \author Xu Waycell
 * \date 12-August-2014
 */
#include "EEPROM.h"
#include <mc9s12a512.h>

const UINT32 EEPROM_CONDITION_OSCCLK = 12800000;  //12.8Mhz
const UINT32 EEPROM_MINIMUM_BUSCLK   = 1000000;   //1Mhz
const UINT32 EEPROM_MINIMUM_EECLK    = 150000;    //0.15Mhz
const UINT32 EEPROM_MAXIMUM_EECLK    = 200000;    //0.20Mhz
const UINT8 EEPROM_COMMAND_ERASE_VERIFY  = 0x05;
const UINT8 EEPROM_COMMAND_PROGRAM       = 0x20;
const UINT8 EEPROM_COMMAND_SECTOR_ERASE  = 0x40;
const UINT8 EEPROM_COMMAND_MASS_ERASE    = 0x41;
const UINT8 EEPROM_COMMAND_SECTOR_MODIFY = 0x60;
 
/**
 * \fn BOOL EEPROMCommand(UINT8 command, UINT16 volatile * const address, const UINT16 data)
 * \brief Routines to execute EEPROM commands.
 * \param command Supported EEPROM command
 * \param address EEPROM address
 * \param data 2 bytes data, the purpose of it depends on the given EEPROM command..
 * \return TRUE if given EEPROM command has been executed successfully.
 */
BOOL EEPROMCommand(UINT8 command, UINT16 volatile * const address, const UINT16 data);

BOOL EEPROMCommand(UINT8 command, UINT16 volatile * const address, const UINT16 data)
{  
  if (ECLKDIV_EDIVLD && EEPROM_ValidateAddress((void * const)address))
  {      
    /* clear PVIOL and ACCERR flags */
    ESTAT = ESTAT_PVIOL_MASK | ESTAT_ACCERR_MASK;
    while(!ESTAT_CBEIF)
    {
      /* feed watch dog */
      __RESET_WATCHDOG(); 
    }
        
    EEPROM_WORD(address) = data;        
    ECMD = command;    
    ESTAT = ESTAT_CBEIF_MASK;
    
    if (!ESTAT_PVIOL && !ESTAT_ACCERR)
    {
      while(!ESTAT_CCIF)
      {
        /* feed watchdog */
        __RESET_WATCHDOG(); 
      }
      return bTRUE;
    }
  }
  return bFALSE;    
}

/**
 * \fn BOOL EEPROM_Setup(const UINT32 oscClk, const UINT32 busClk)
 * \brief Sets up the EEPROM with the correct internal clock Based on Figure 4-1 of the EETS4K Block User Guide V02.07.
 * \param oscClk the oscillator clock frequency in Hz
 * \param busClk the bus clock frequency in Hz
 * \return TRUE if the EEPROM was setup succesfully
 */
BOOL EEPROM_Setup(const UINT32 oscClk, const UINT32 busClk)
{
  UINT8  PRDIV8 = 0;
  UINT32 PRDCLK = 0;
  UINT8  EDIV = 0;
  if (!ECLKDIV_EDIVLD && busClk >= EEPROM_MINIMUM_BUSCLK) /* EEPROM clock divider cannot be on and bus clock cannot less than minimum requirement */
  {
    PRDIV8 = 0;
    if (oscClk > EEPROM_CONDITION_OSCCLK)
    {
      PRDIV8 = 1;        
      PRDCLK = oscClk / 8;
    }
    else
    {
      PRDCLK = oscClk;
    }
    if (((PRDCLK*5) % MATH_1_MEGA == 0) && (PRDCLK % busClk == 0))
    {
      EDIV = (UINT8)((PRDCLK * 5 / MATH_1_MEGA) + PRDCLK / busClk-1);        
    }
    else
    {
      EDIV = (UINT8)((PRDCLK * 5 / MATH_1_MEGA) + PRDCLK / busClk);        
    }
    if ((MATH_1_MEGA * (1 + EDIV) / PRDCLK + MATH_1_KILO / busClk) >= 5 && PRDCLK / (1 + EDIV) >= EEPROM_MINIMUM_EECLK)
    {
      ECLKDIV_PRDIV8 = PRDIV8;
      ECLKDIV_EDIV = EDIV;
      return bTRUE;
    }
  }
  return bFALSE;    
}

/**
 * \fn BOOL EEPROM_ValidateAddress(void * const address) 
 * \brief Verify given pointer it is in the legal access range or not.
 * \return TRUE if given EEPROM address is in the valid range.
 */
BOOL EEPROM_ValidateAddress(void * const address)
{
  /* return true if given pointer is in configed address range */
  return (UINT16)address >= EEPROM_ADDRESS_BEGIN &&
         (UINT16)address <= EEPROM_ADDRESS_END;
}

/**
 * \fn BOOL EEPROM_Write32(UINT32 volatile * const address, const UINT32 data)
 * \brief Writes a 32-bit number to EEPROM.
 * \param address the address of the data
 * \param data the data to write
 * \return TRUE if EEPROM was written successfully; FALSE if address is not aligned to a 4-byte boundary or if there is a programming error
 * \warning Assumes EEPROM has been initialized
 */
BOOL EEPROM_Write32(UINT32 volatile * const address, const UINT32 data)
{
  UINT16 volatile * eepromAddress = (UINT16 volatile * const)address;
  TUINT32 eepromSector;

  if ((UINT16)eepromAddress % 4 == 0)
  {    
    eepromSector.l = data;
    return EEPROMCommand(EEPROM_COMMAND_SECTOR_ERASE, eepromAddress, 0xFFFF) &&
           EEPROMCommand(EEPROM_COMMAND_PROGRAM, eepromAddress, eepromSector.s.Hi) &&
           EEPROMCommand(EEPROM_COMMAND_PROGRAM, ++eepromAddress, eepromSector.s.Lo);    
  }
  return bFALSE;
}

/**
 * \fn BOOL EEPROM_Write16(UINT16 volatile * const address, const UINT16 data)
 * \brief Writes a 16-bit number to EEPROM
 * \param address is the address of the data,
 * \param data is the data to write
 * \return TRUE if EEPROM was written successfully; FALSE if address is not aligned to a 2-byte boundary or if there is a programming error
 * \warning Assumes EEPROM has been initialized
 */
BOOL EEPROM_Write16(UINT16 volatile * const address, const UINT16 data)
{
  UINT16 volatile * eepromAddress = address;
  TUINT32 eepromSector;

  if ((UINT16) eepromAddress % 2 == 0)
  { 
    /* alignment */  
    if ((UINT16) eepromAddress % 4 != 0)
    {
      --eepromAddress; /* move 2 bytes to left */
      eepromSector.l = EEPROM_SECTOR(eepromAddress);
      eepromSector.s.Lo = data;
    }
    else
    {
      eepromSector.l = EEPROM_SECTOR(eepromAddress);
      eepromSector.s.Hi = data;
    }
    return EEPROM_Write32((UINT32 volatile * const)eepromAddress, eepromSector.l); 
  }       
  return bFALSE;
}

/**
 * \fn BOOL EEPROM_Write8(UINT8 volatile * const address, const UINT8 data)
 * \brief Writes an 8-bit number to EEPROM
 * \param address is the address of the data,
 * \param data is the data to write
 * \return TRUE if EEPROM was written successfully; FALSE if there is a programming error
 * \warning Assumes EEPROM has been initialized
 */
BOOL EEPROM_Write8(UINT8 volatile * const address, const UINT8 data)
{
  UINT8 volatile * eepromAddress = address;
  TUINT16 eepromWord;
  
  /* alignment */
  if ((UINT16)eepromAddress % 2 != 0) {
      --eepromAddress; /* move 1 byte to left */
      eepromWord.l = EEPROM_WORD(eepromAddress);
      eepromWord.s.Lo = data;      
  } else {
      eepromWord.l = EEPROM_WORD(eepromAddress);
      eepromWord.s.Hi = data;      
  }
  return EEPROM_Write16((UINT16 volatile * const)eepromAddress, eepromWord.l);
} 

/**
 * \fn BOOL EEPROM_Erase(void)  
 * \brief Erases the entire EEPROM
 * \return TRUE if EEPROM was erased successfully
 * \warning Assumes EEPROM has been initialized
 */
BOOL EEPROM_Erase(void)
{
  return EEPROMCommand(EEPROM_COMMAND_MASS_ERASE, (UINT16 volatile * const)EEPROM_ADDRESS_BEGIN, 0xFFFF) &&
         EEPROMCommand(EEPROM_COMMAND_ERASE_VERIFY, (UINT16 volatile * const)EEPROM_ADDRESS_BEGIN, 0xFFFF) &&
         ESTAT_BLANK;
}
