/**
 * \file packet.h
 * \brief Routines to implement packet encoding and decoding for the serial port
 * \author Xu Waycell
 * \date 06-August-2014
 */
#ifndef PACKET_H
#define PACKET_H

#include "global.h"

/**
 * Packet content
 */
extern UINT8 Packet_Command, Packet_Parameter1, Packet_Parameter2, Packet_Parameter3;

/**
 * ModCon number
 */
extern TUINT16 ModConNumber;

#if !defined(ModConNumberLSB) && !defined(ModConNumberMSB)
#define ModConNumberLSB ModConNumber.s.Lo
#define ModConNumberMSB ModConNumber.s.Hi
#endif

/**
 * ModCon mode
 */
extern TUINT16 ModConMode;
#if !defined(ModConModeLSB) && !defined(ModConModeMSB)
#define ModConModeLSB ModConMode.s.Lo
#define ModConModeMSB ModConMode.s.Hi
#endif

/**
 * \fn UINT8 Packet_Checksum(const UINT8 command, const UINT8 parameter1, 
  const UINT8 parameter2, const UINT8 parameter3)
 * \brief Generates a checksum result of four given bytes.
 * \param command command byte
 * \param parameter1 first parameter byte
 * \param parameter2 second parameter byte
 * \param parameter3 third parameter byte
 * \return Checksum result of given bytes.
 */
UINT8 Packet_Checksum(const UINT8 command, const UINT8 parameter1, 
  const UINT8 parameter2, const UINT8 parameter3);

/**
 * \fn BOOL Packet_Setup(const UINT32 baudRate, const UINT32 busClk)
 * \brief Initializes the packets by calling the initialization routines of the supporting software modules.
 * \param baudRate the baud rate in bits/sec
 * \param busClk the bus clock rate in Hz
 * \return TRUE if the packets were initialized successfully
 */
BOOL Packet_Setup(const UINT32 baudRate, const UINT32 busClk);

/**
 * \fn BOOL Packet_Get(void)
 * \brief Attempts to get a packet from the received data.
 * \return TRUE if a valid packet was received
 */
BOOL Packet_Get(void);

/**
 * \fn BOOL Packet_Put(const UINT8 command, const UINT8 parameter1, 
  const UINT8 parameter2, const UINT8 parameter3)
 * \brief Builds a packet and places it in the transmit FIFO buffer.
 * \param command command byte
 * \param parameter1 first parameter byte
 * \param parameter2 second parameter byte
 * \param parameter3 third parameter byte
 * \return TRUE if a valid packet was queued for transmission successfully
 */
BOOL Packet_Put(const UINT8 command, const UINT8 parameter1, 
  const UINT8 parameter2, const UINT8 parameter3);

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

#endif