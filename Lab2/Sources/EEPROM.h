/**
 * \file EEPROM.h
 * \brief Routines for erasing and writing to the EEPROM
 * \author Xu Waycell
 * \date 13-August-2014
 */
#ifndef EEPROM_H
#define EEPROM_H

#include "global.h"

/* EEPROM data access */
#define EEPROM_BYTE(EEPROM_ADDRESS)	   *(UINT8 volatile *)(EEPROM_ADDRESS)
#define EEPROM_INTEGER(EEPROM_ADDRESS) *(INT16 volatile *)(EEPROM_ADDRESS)
#define EEPROM_WORD(EEPROM_ADDRESS)	   *(UINT16 volatile *)(EEPROM_ADDRESS)
#define EEPROM_LONG(EEPROM_ADDRESS)	   *(INT32 volatile *)(EEPROM_ADDRESS)
#define EEPROM_SECTOR(EEPROM_ADDRESS)	 *(UINT32 volatile *)(EEPROM_ADDRESS)

/**
 * \fn BOOL EEPROM_Setup(const UINT32 oscClk, const UINT32 busClk)
 * \brief Sets up the EEPROM with the correct internal clock Based on Figure 4-1 of the EETS4K Block User Guide V02.07.
 * \param oscClk the oscillator clock frequency in Hz
 * \param busClk the bus clock frequency in Hz
 * \return TRUE if the EEPROM was setup succesfully
 */
BOOL EEPROM_Setup(const UINT32 oscClk, const UINT32 busClk);
 
/**
 * \fn BOOL EEPROM_Write32(UINT32 volatile * const address, const UINT32 data)
 * \brief Writes a 32-bit number to EEPROM.
 * \param address the address of the data
 * \param data the data to write
 * \return TRUE if EEPROM was written successfully; FALSE if address is not aligned to a 4-byte boundary or if there is a programming error
 * \warning Assumes EEPROM has been initialized
 */
BOOL EEPROM_Write32(UINT32 volatile * const address, const UINT32 data);
 
/**
 * \fn BOOL EEPROM_Write16(UINT16 volatile * const address, const UINT16 data)
 * \brief Writes a 16-bit number to EEPROM
 * \param address is the address of the data,
 * \param data is the data to write
 * \return TRUE if EEPROM was written successfully; FALSE if address is not aligned to a 2-byte boundary or if there is a programming error
 * \warning Assumes EEPROM has been initialized
 */
BOOL EEPROM_Write16(UINT16 volatile * const address, const UINT16 data);

/**
 * \fn BOOL EEPROM_ValidateAddress(void * const address) 
 * \brief Verify given pointer it is in the legal access range or not.
 * \return TRUE if given EEPROM address is in the valid range.
 */
BOOL EEPROM_ValidateAddress(void * const address);

/**
 * \fn BOOL EEPROM_Write8(UINT8 volatile * const address, const UINT8 data)
 * \brief Writes an 8-bit number to EEPROM
 * \param address is the address of the data,
 * \param data is the data to write
 * \return TRUE if EEPROM was written successfully; FALSE if there is a programming error
 * \warning Assumes EEPROM has been initialized
 */
BOOL EEPROM_Write8(UINT8 volatile * const address, const UINT8 data);

/**
 * \fn BOOL EEPROM_Erase(void)  
 * \brief Erases the entire EEPROM
 * \return TRUE if EEPROM was erased successfully
 * \warning Assumes EEPROM has been initialized
 */
BOOL EEPROM_Erase(void);

#endif