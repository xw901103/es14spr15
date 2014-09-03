/**
 * \file packet.h
 * \brief Routines to implement packet encoding and decoding for the serial port to serve ModCon protocols.
 * \author Xu Waycell
 * \date 06-August-2014
 */
#ifndef PACKET_H
#define PACKET_H

#include "global.h"

/* Packet structure */
typedef struct
{
  UINT8 command;
  union
  {
    struct
    {
      UINT8 parameter3;
      UINT8 parameter2;
      UINT8 parameter1;
    } separate;
    struct
    {
      UINT8 parameter3;
      UINT16 parameter12;
    } combined12;
    struct
    {
      UINT16 parameter23;
      UINT8 paramater1;
    } combined23;
  } parameters;
} TPacket;

#define Packet_Command     Packet.command
#define Packet_Parameter1  Packet.parameters.separate.parameter1
#define Packet_Parameter2  Packet.parameters.separate.parameter2
#define Packet_Parameter3  Packet.parameters.separate.parameter3
#define Packet_Parameter12 Packet.parameters.combined12.parameter12
#define Packet_Parameter23 Packet.parameters.combined23.parameter23

// Packet structure
extern TPacket Packet;
extern BOOL Packet_CommandOK;

/**
 * Packet content
 */
//extern UINT8 Packet_Command, Packet_Parameter1, Packet_Parameter2, Packet_Parameter3;

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
UINT8 Packet_Checksum(const UINT8 command, const UINT8 parameter1, const UINT8 parameter2, const UINT8 parameter3);

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
BOOL Packet_Put(const UINT8 command, const UINT8 parameter1, const UINT8 parameter2, const UINT8 parameter3);

#endif