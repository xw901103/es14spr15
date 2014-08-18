/**
 * \file main.h
 * \brief Program main entry file. 
 * \author Xu Waycell
 * \date 11-August-2014
 */
#ifndef MAIN_H
#define MAIN_H

#include "global.h"
#pragma LINK_INFO DERIVATIVE "mc9s12a512" /* link mc9s12a512's library */

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
 * \fn BOOL Packet_Put_ModCon_Startup(void)
 * \brief Builds packets that are necessary for startup information and places them into transmit buffer. 
 * \return TRUE if packets were queued for transmission successfully
 */  
BOOL Packet_Put_ModCon_Startup(void);

/**
 * \fn BOOL Packet_Put_ModCon_Version(void)
 * \brief Builds a packet that contains ModCon version details and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully
 */
BOOL Packet_Put_ModCon_Version(void);

/**
 * \fn BOOL Packet_Put_ModCon_Number_Get(void)
 * \brief Builds a packet that contains ModCon number and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully
 */
BOOL Packet_Put_ModCon_Number_Get(void);

/**
 * \fn BOOL Packet_Put_ModCon_Mode_Get(void)
 * \brief Builds a packet that contains ModCon mode and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully
 */
BOOL Packet_Put_ModCon_Mode_Get(void);

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
