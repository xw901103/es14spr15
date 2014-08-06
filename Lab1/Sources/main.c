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

TUINT16 ModConNumber;

void LogDebug(const UINT16 lineNumber, const UINT16 err) {
    /* need to handle logs */
}

void Initialize(void) {
    ModConNumber.l = 7229;
    
    if (!Packet_Setup(BAUDRATE, BUSCLK)) {
        LogDebug(__LINE__, ERR_PACKET_SETUP);
    }
}

void Routine(void) {
    UINT8 ACK = 0;
    SCI_Poll();
    if (Packet_Get()) { 
        ACK = Packet_Command & MODCON_COMMAND_ACK_MASK;
        Packet_Command &= ~MODCON_COMMAND_ACK_MASK;
        switch(Packet_Command) {
            case MODCON_COMMAND_STARTUP:
                if (!Packet_Setup(BAUDRATE, BUSCLK)) {
                    LogDebug(__LINE__, ERR_PACKET_PUT);
                }
                break;
            case MODCON_COMMAND_VERSION:
                if (!ACK) {                  
                    if (!Packet_Put(MODCON_COMMAND_VERSION, MODCON_VERSION_INITIAL, MODCON_VERSION_MAJOR, MODCON_VERSION_MINOR)) {
                        LogDebug(__LINE__, ERR_PACKET_PUT);
                    }
                } else { /* Prepare parameters for ACK reply */
                    Packet_Parameter1 = MODCON_VERSION_INITIAL;
                    Packet_Parameter2 = MODCON_VERSION_MAJOR;
                    Packet_Parameter3 = MODCON_VERSION_MINOR;
                }                   
                break;
            case MODCON_COMMAND_NUMBER:
                if (Packet_Parameter1 == MODCON_NUMBER_GET) {
                    if (!ACK) {
                        if (!Packet_Put(MODCON_COMMAND_NUMBER, MODCON_NUMBER_GET, ModConNumberLSB, ModConNumberMSB)) {
                            LogDebug(__LINE__, ERR_PACKET_PUT);
                        }
                    } else { /* Prepare parameter for ACK reply */
                        Packet_Parameter2 = ModConNumberLSB;
                        Packet_Parameter3 = ModConNumberMSB;
                    }
                } else if (Packet_Parameter1 == MODCON_NUMBER_SET) {
                    ModConNumberLSB = Packet_Parameter2;
                    ModConNumberMSB = Packet_Parameter3;
                }
                break;
            default:
                break;
        }
        if (ACK) {            
            if (!Packet_Put(Packet_Command | MODCON_COMMAND_ACK_MASK, Packet_Parameter1, Packet_Parameter2, Packet_Parameter3)) {
                LogDebug(__LINE__, ERR_PACKET_PUT);
            }
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