// ----------------------------------------
// Filename: main.c
// Description: (your description)
// Author: (your name)
// Date: (the date)

#include <hidef.h>      /* common defines and macros */
#include <mc9s12a512.h>     /* derivative information */
		 
#pragma LINK_INFO DERIVATIVE "mc9s12a512"

#include "global.h"
#include "packet.h"
#include "SCI.h"

/* TODO: use TUINT16 instead this crap */
UINT8 ModConNumberH = 0x1C;
UINT8 ModConNumberL = 0x3D;

BOOL SendInitialPackets(void) {
    return Packet_Put(MODCON_COMMAND_STARTUP, 0, 0, 0) && Packet_Put(MODCON_COMMAND_VERSION, MODCON_VERSION_INITIAL, MODCON_VERSION_MAJOR, MODCON_VERSION_MINOR) && Packet_Put(MODCON_COMMAND_NUMBER, 1, ModConNumberL, ModConNumberH);
}

void Initialize(void) {
    SCI_Setup(BAUDRATE, BUSCLK);
    SendInitialPackets();
}

void Routine(void) {
    UINT8 ACK = 0;
    SCI_Poll();
    if (Packet_Get()) { 
        ACK = Packet_Command & MODCON_COMMAND_ACK_MASK;
        Packet_Command &= ~MODCON_COMMAND_ACK_MASK;
        switch(Packet_Command) {
            case MODCON_COMMAND_STARTUP:
                SendInitialPackets();
                break;
            case MODCON_COMMAND_VERSION:
                if (!ACK) {                  
                    Packet_Put(MODCON_COMMAND_VERSION, MODCON_VERSION_INITIAL, MODCON_VERSION_MAJOR, MODCON_VERSION_MINOR);
                } else {
                    
                }                   
                break;
            case MODCON_COMMAND_NUMBER:
                if (Packet_Parameter1 == 1) {
                    Packet_Put(MODCON_COMMAND_NUMBER, 1, ModConNumberL, ModConNumberH);
                } else if (Packet_Parameter1 == 2) {
                    ModConNumberL = Packet_Parameter2;
                    ModConNumberH = Packet_Parameter3;
                }
                Packet_Put(0x84, 0, 0, 0); /* ACK Packet */
                break;
            default:
                break;
        }
        /*
        if (ACK)
            Packet_Put(Packet_Command | MODCON_COMMAND_ACK_MASK, Packet_Parameter1, Packet_Parameter2, Packet_Parameter3);        
         */
    }
}

void main(void)
{			
  Initialize();
  
  for (;;)
  {
    Routine();
  }
}