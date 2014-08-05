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

void SendInitialPackets(void) {
    Packet_Put(MODCON_STARTUP, 0, 0, 0);
    Packet_Put(MODCON_VERSION, 'V', 1, 0);
    Packet_Put(MODCON_NUMBER, 1, ModConNumberL, ModConNumberH);
}

void Initialize(void) {
    SCI_Setup(BAUDRATE, BUSCLK);
    SendInitialPackets();
}

void Routine(void) {
    UINT8 ACK = 0;
    SCI_Poll();
    if (Packet_Get()) {
        switch(Packet_Command) {
            case MODCON_STARTUP:
                SendInitialPackets();
                break;
            case MODCON_VERSION:
                Packet_Put(MODCON_VERSION, 'V', 1, 0);
                break;
            case MODCON_NUMBER:
                if (Packet_Parameter1 == 1) {
                    Packet_Put(MODCON_NUMBER, 1, ModConNumberL, ModConNumberH);
                } else if (Packet_Parameter1 == 2) {
                    ModConNumberL = Packet_Parameter2;
                    ModConNumberH = Packet_Parameter3;
                }
                Packet_Put(0x84, 0, 0, 0); /* ACK Packet */
                break;
            default:
                break;
        }
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