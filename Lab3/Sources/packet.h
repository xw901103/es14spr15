// ----------------------------------------
// Filename: packet.h
// Description: Routines to implement packet
//   encoding and decoding for the serial port
// Author: PMcL
// Date: 13-Mar-06

#ifndef PACKET_H
#define PACKET_H

// new types
#include "types.h"

// Packet structure
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

// ----------------------------------------
// Packet_Setup
// 
// Initializes the packets by calling the
// initialization routines of the supporting
// software modules
// Input: baudRate is the baud rate in bits/sec
//   busClk is the bus clock rate in Hz
// Output: TRUE if the packets were initialized successfully
// Conditions: none

BOOL Packet_Setup(const UINT32 baudRate, const UINT32 busClk);

// ----------------------------------------
// Packet_Get
// 
// Attempts to get a packet from the received data
// Input: none
// Output: TRUE if a valid packet was received
// Conditions: none

BOOL Packet_Get(void);

// ----------------------------------------
// Packet_Put
// 
// Builds a packet and places it in the transmit FIFO buffer
// Input: none
// Output: TRUE if a valid packet was sent
// Conditions: none

BOOL Packet_Put(const UINT8 Command, const UINT8 Parameter1, 
  const UINT8 Parameter2, const UINT8 Parameter3);

#endif