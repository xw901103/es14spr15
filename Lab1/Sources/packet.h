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
 * TODO: replace it with struct
 */
extern UINT8 Packet_Command, Packet_Parameter1, Packet_Parameter2, Packet_Parameter3;
typedef struct {
    UINT8 Command;
    UINT8 Parameter1;
    UINT8 Parameter2;
    UINT8 Parameter3;  
} TPACKET;

/**
 * Packet_Checksum
 */
UINT8 Packet_Checksum(const TPACKET* const PACKET);

/**
 * Packet_Setup
 * 
 * Initializes the packets by calling the
 * initialization routines of the supporting
 * software modules
 * Input: baudRate is the baud rate in bits/sec
 *   busClk is the bus clock rate in Hz
 * Output: TRUE if the packets were initialized successfully
 * Conditions: none
 */
BOOL Packet_Setup(const UINT32 baudRate, const UINT32 busClk);

/**
 * Packet_Get
 * 
 * Attempts to get a packet from the received data
 * Input: none
 * Output: TRUE if a valid packet was received
 * Conditions: none
 */
BOOL Packet_Get(void);

/**
 * Packet_Put
 * 
 * Builds a packet and places it in the transmit FIFO buffer
 * Input: none
 * Output: TRUE if a valid packet was sent
 * Conditions: none
 */
BOOL Packet_Put(const UINT8 command, const UINT8 parameter1, 
  const UINT8 parameter2, const UINT8 parameter3);

#endif