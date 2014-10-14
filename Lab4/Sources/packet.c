/**
 * \file packet.c
 * \brief Routines to implement packet encoding and decoding for the serial port to serve ModCon protocols.
 * \author Xu Waycell
 * \date 06-August-2014
 */
#include "packet.h"
#include "SCI.h"

/* define and initialize our externs */
TPacket Packet = { 0 };

/* states of packet receive state machine */
typedef enum 
{
  STATE_0,
  STATE_1,
  STATE_2,
  STATE_3,
  STATE_4,
  STATE_5        
} PACKET_STATE;

/**
 * \fn UINT8 Packet_Checksum(const UINT8 command, const UINT8 parameter1, const UINT8 parameter2, const UINT8 parameter3)
 * \brief Generates a checksum result of four given bytes.
 * \param command command byte
 * \param parameter1 first parameter byte
 * \param parameter2 second parameter byte
 * \param parameter3 third parameter byte
 * \return Checksum result of given bytes.
 */
UINT8 Packet_Checksum(const UINT8 command, const UINT8 parameter1, const UINT8 parameter2, const UINT8 parameter3) 
{
  return (command ^ parameter1 ^ parameter2 ^ parameter3); 
}

/**
 * \fn BOOL Packet_Setup(const UINT32 baudRate, const UINT32 busClk)
 * \brief Initializes the packets by calling the initialization routines of the supporting software modules.
 * \param baudRate the baud rate in bits/sec
 * \param busClk the bus clock rate in Hz
 * \return TRUE if the packets were initialized successfully
 */
BOOL Packet_Setup(const UINT32 baudRate, const UINT32 busClk)
{
  SCI_Setup(baudRate, busClk);
  return bTRUE; /* no extra configs for now so we leave it here */
}

/**
 * \fn BOOL Packet_Get(void)
 * \brief Attempts to get a packet from the received data.
 * \return TRUE if a valid packet was received
 */
BOOL Packet_Get(void)
{
  /* temps for packet receive state machine */
  static PACKET_STATE state = STATE_1;
  static UINT8 command = 0, parameter1 = 0, parameter2 = 0, parameter3 = 0, checksum = 0;

#ifdef NO_INTERRUPT    
  SCI_Poll();
#endif  
  switch(state)
  {
    case STATE_0:
      if (SCI_InChar(&command))
      {
        state = STATE_1;
      }
      break;
    case STATE_1:
      if (SCI_InChar(&parameter1))
      {
        state = STATE_2;
      }
      break;
    case STATE_2:
      if (SCI_InChar(&parameter2))
      {
        state = STATE_3;
      }
      break;
    case STATE_3:
      if (SCI_InChar(&parameter3))
      {
        state = STATE_4;
      }
      break;            
    case STATE_4:
      if (SCI_InChar(&checksum))
      {
        state = STATE_5;
      }
      break;
    case STATE_5:
      if (checksum != Packet_Checksum(command, parameter1, parameter2, parameter3))
      {
        command = parameter1;
        parameter1 = parameter2;
        parameter2 = parameter3;
        parameter3 = checksum;
        state = STATE_4;                    
      }
      else
      {
        Packet_Command = command;
        Packet_Parameter1 = parameter1;
        Packet_Parameter2 = parameter2;
        Packet_Parameter3 = parameter3;
        state = STATE_0;
        return bTRUE;
      }
      break;
    default:
      break;
  }
  return bFALSE;
}

/**
 * \fn BOOL Packet_Put(const UINT8 command, const UINT8 parameter1, const UINT8 parameter2, const UINT8 parameter3)
 * \brief Builds a packet and places it in the transmit FIFO buffer.
 * \param command command byte
 * \param parameter1 first parameter byte
 * \param parameter2 second parameter byte
 * \param parameter3 third parameter byte
 * \return TRUE if a valid packet was queued for transmission successfully
 */
BOOL Packet_Put(const UINT8 command, const UINT8 parameter1, const UINT8 parameter2, const UINT8 parameter3)
{  
  return SCI_OutChar(command) &&
         SCI_OutChar(parameter1) &&
         SCI_OutChar(parameter2) &&
         SCI_OutChar(parameter3) &&
         SCI_OutChar(Packet_Checksum(command, parameter1, parameter2, parameter3));
}
