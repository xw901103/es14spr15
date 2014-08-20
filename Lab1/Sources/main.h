/**
 * \mainpage Xu's Embedded Software Lab 1
 *
 * \section intro_sec Introduction
 * Lab 1 is to implement a program that responds ModCon protocol via Serial Communication Interface
 * on ModCon board running at bus clock 8 Mhz with 38400 baud rate.
 * Following ModCon Protocols have been implemented.
 *
 * * 0x04 ModCon Startup
 * <br>This will send program settings including ModCon number and version.
 * * 0x09 ModCon Special ModCon version number query
 * <br>This will send current ModCon version number
 * * 0x0B ModCon number get and set
 * <br>This is the accessor and mutator of ModCon number
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

const UINT8 MODCON_COMMAND_STARTUP = 0x04; /* ModCon protocol startup command */
const UINT8 MODCON_COMMAND_SPECIAL = 0x09; /* ModCon protocol special command */
const UINT8 MODCON_COMMAND_NUMBER  = 0x0B; /* ModCon protocol number command */

const UINT8 MODCON_VERSION_INITIAL = 'v';
const UINT8 MODCON_VERSION_TOKEN   = 'x'; /* TODO: rename it for better functionality reflection */
const UINT8 MODCON_VERSION_MAJOR   =  1 ;
const UINT8 MODCON_VERSION_MINOR   =  0 ;

const UINT8 MODCON_NUMBER_GET = 1;
const UINT8 MODCON_NUMBER_SET = 2;

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
 * \fn void Initialize(void)
 * \brief Initializes hardware and software parameters that required for this program.
 */
void Initialize(void);

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
