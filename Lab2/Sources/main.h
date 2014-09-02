/**
 * \mainpage Xu's Embedded Software Lab 2
 *
 * \section intro_sec Introduction
 * Lab 2 is based on Lab 1 to setup bus clock to 24Mhz via Phase-Locked Loop
 * module, Watchdog for operating properly and EEPROM functions for ModCon 
 * protocol expansion. In addition, SCI baud rate has been increased to 115200
 * to adapte bus clock change.
 *
 * * 0x04 ModCon Startup
 * <br>This will send program settings including ModCon number and version.
 * * 0x07 ModCon Program EEPROM byte
 * <br>This will program a byte in EEPROM through given data and address. 
 * * 0x08 ModCon Get EEPROM byte
 * <br>This will send current data stored in given EEPROM address.
 * * 0x09 ModCon Special ModCon version number query
 * <br>This will send current ModCon version number
 * * 0x0B ModCon number get and set
 * <br>This is the accessor and mutator of ModCon number
 * * 0x0D ModCon mode get and set
 * <br>This is the accessor and mutator of ModCon mode
 *
 * \file main.h
 * \brief Program main entry file. 
 * \author Xu Waycell
 * \date 11-August-2014
 */
#ifndef MAIN_H
#define MAIN_H

#include "global.h"
#pragma LINK_INFO DERIVATIVE "mc9s12a512" /* link mc9s12a512's library */

const UINT8 MODCON_COMMAND_STARTUP        = 0x04; /* ModCon protocol startup command */
const UINT8 MODCON_COMMNAD_EEPROM_PROGRAM = 0x07;
const UINT8 MODCON_COMMAND_EEPROM_GET     = 0x08;
const UINT8 MODCON_COMMAND_SPECIAL        = 0x09; /* ModCon protocol special command */
const UINT8 MODCON_COMMAND_NUMBER         = 0x0B; /* ModCon protocol number command */
const UINT8 MODCON_COMMAND_MODE           = 0x0D;

const UINT8 MODCON_VERSION_INITIAL = 'v';
const UINT8 MODCON_VERSION_TOKEN   = 'x'; /* TODO: rename it for better functionality reflection */
const UINT8 MODCON_VERSION_MAJOR   =  1 ;
const UINT8 MODCON_VERSION_MINOR   =  0 ;

const UINT8 MODCON_NUMBER_GET = 1;
const UINT8 MODCON_NUMBER_SET = 2;

const UINT8 MODCON_MODE_GET = 1;
const UINT8 MODCON_MODE_SET = 2;

#if !defined(MODCON_COMMAND_ACK_MASK)
#define MODCON_COMMAND_ACK_MASK 0x80 /* ModCon protocol acknowledgement bitwise mask */
#endif

/**
 * ModCon number
 */
static TUINT16 ModConNumber;

#if !defined(ModConNumberLSB) && !defined(ModConNumberMSB)
#define ModConNumberLSB ModConNumber.s.Lo
#define ModConNumberMSB ModConNumber.s.Hi
#endif

/**
 * ModCon mode
 */
static TUINT16 ModConMode;

#if !defined(ModConModeLSB) && !defined(ModConModeMSB)
#define ModConModeLSB ModConMode.s.Lo
#define ModConModeMSB ModConMode.s.Hi
#endif

/**
 * \fn BOOL Handle_ModCon_Startup(void)
 * \brief Builds packets that are necessary for startup information and places them into transmit buffer. 
 * \return TRUE if packets were queued for transmission successfully
 */  
BOOL Handle_ModCon_Startup(void);

/**
 * \fn BOOL Handle_ModCon_Version(void)
 * \brief Builds a packet that contains ModCon version details and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully
 */
BOOL Handle_ModCon_Version(void);

/**
 * \fn BOOL Handle_ModCon_Number_Get(void)
 * \brief Builds a packet that contains ModCon number and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully
 */
BOOL Handle_ModCon_Number_Get(void);

/**
 * \fn BOOL Handle_ModCon_Number_Set(void)
 * \brief Assign new value to ModCon number through given packet then update stored EEPROM value. 
 * \return TRUE if write new value to EEPROM is successful.
 */
BOOL Handle_ModCon_Number_Set(void);

/**
 * \fn BOOL Handle_ModCon_Mode_Get(void)
 * \brief Builds a packet that contains ModCon mode and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully
 */
BOOL Handle_ModCon_Mode_Get(void);

/**
 * \fn BOOL Handle_ModCon_Mode_Set(void)
 * \brief Assign new value to ModCon mode through given packet then update stored EEPROM value. 
 * \return TRUE if write new value to EEPROM is successful.
 */
 BOOL Handle_ModCon_Mode_Set(void);

/**
 * \fn BOOL Handle_ModCon_EEPROM_Program(UINT8 volatile * const address, const UINT8 data)
 * \brief
 * \param address
 * \param data
 * \return
 */
BOOL Handle_ModCon_EEPROM_Program(void);

/**
 * \fn BOOL Handle_ModCon_EEPROM_Get(UINT8 volatile * const address)
 * \brief
 * \param address
 * \return
 */
BOOL Handle_ModCon_EEPROM_Get(void);

/**
 * \fn void TurnOnStartupIndicator(void)
 * \brief turn on the Port E pin 7 connected LED.
 */
void TurnOnStartupIndicator(void);

/**
 * \fn void Initialize(void)
 * \brief Initializes hardware and software parameters that required for this program.
 * \return TRUE if initialization routines executed successfully.
 */
BOOL Initialize(void);

/**
 * \fn void Routine(void)
 * \brief Retrieves ModCon packets and sends back packets if it is necessary.
 */
void Routine(void);

/**
 * \fn void main(void)
 * \brief The main entry of the program will initialize runtime parameters and keep looping routine.
 */
void main(void);

#endif
