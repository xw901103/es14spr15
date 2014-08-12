/**
 * \file packet.h
 * \brief Routines to implement packet encoding and decoding for the serial port
 * \author Xu Waycell
 * \date
 */
#ifndef PACKET_H
#define PACKET_H

/* global.h includes defs.h and types.h */
#include "global.h"

/**
 * Packet structure
 */
extern UINT8 Packet_Command, Packet_Parameter1, Packet_Parameter2, Packet_Parameter3;

/**
 */
extern TUINT16 ModConNumber;

#if !defined(ModConNumberLSB) && !defined(ModConNumberMSB)
#define ModConNumberLSB ModConNumber.s.Lo
#define ModConNumberMSB ModConNumber.s.Hi
#endif


/**
 * \fn UINT8 Packet_Checksum(const UINT8 command, const UINT8 parameter1, 
  const UINT8 parameter2, const UINT8 parameter3)
 * \brief Generates a checksum result of four given bytes.
 * \return Checksum result of given bytes.
 * \param command command byte
 * \param parameter1 first parameter byte
 * \param parameter2 second parameter byte
 * \param parameter3 third parameter byte
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
 * \return TRUE if a valid packet was sent
 */
BOOL Packet_Put(const UINT8 command, const UINT8 parameter1, 
  const UINT8 parameter2, const UINT8 parameter3);
  
BOOL Packet_Put_ModCon_Startup(void);
BOOL Packet_Put_ModCon_Version(void);
BOOL Packet_Put_ModCon_Number_Get(void);

#endif