/**
 * \file main.c
 * \brief Program main entry
 * \author Xu Waycell
 * \date
 */
#include <hidef.h>      /* common defines and macros */
//#include <mc9s12a512.h>     /* derivative information */		 
#pragma LINK_INFO DERIVATIVE "mc9s12a512"

#include "global.h"
#include "packet.h"
#include "SCI.h"

TUINT16 ModConNumber;

#ifndef NO_DEBUG
void LogDebug(const UINT16 lineNumber, const UINT16 err) {
    /* break point here */
    UNUSED(lineNumber);
    UNUSED(err);
}
#endif

void Initialize(void) {
    ModConNumber.l = 7229;
    
    if (!Packet_Setup(CONFIG_BAUDRATE, CONFIG_BUSCLK)) {
#ifndef NO_DEBUG
        DEBUG(__LINE__, ERR_PACKET_SETUP);
#endif
    }
}

void Routine(void) {
    UINT8 ACK = 0;
    BOOL DENY = bFALSE;
    
    SCI_Poll();
    if (Packet_Get()) { 
        ACK = Packet_Command & MODCON_COMMAND_ACK_MASK; /* detect ACK mask from command */
        Packet_Command &= ~MODCON_COMMAND_ACK_MASK;     /* clear ACK mask from command */
        switch(Packet_Command) {
            case MODCON_COMMAND_STARTUP:
                if (!Packet_Setup(CONFIG_BAUDRATE, CONFIG_BUSCLK)) { /* calibrate SCI configs and push first three packets */
#ifndef NO_DEBUG
                    DEBUG(__LINE__, ERR_PACKET_SETUP);
#endif
                }
                break;
            case MODCON_COMMAND_VERSION:
                if (!Packet_Put(MODCON_COMMAND_VERSION, MODCON_VERSION_INITIAL, MODCON_VERSION_MAJOR, MODCON_VERSION_MINOR)) {
#ifndef NO_DEBUG
                    DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
                }
                break;
            case MODCON_COMMAND_NUMBER:
                if (Packet_Parameter1 == MODCON_NUMBER_GET) {                        
                    if (!Packet_Put(MODCON_COMMAND_NUMBER, MODCON_NUMBER_GET, ModConNumberLSB, ModConNumberMSB)) {
#ifndef NO_DEBUG
                        DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
                    }
                } else if (Packet_Parameter1 == MODCON_NUMBER_SET) {
                    ModConNumberLSB = Packet_Parameter2;
                    ModConNumberMSB = Packet_Parameter3;
                }
                break;
            default:
                DENY = bTRUE;
                break;
        }
        if (ACK) {
            if (!DENY) {                
                if (!Packet_Put(Packet_Command | MODCON_COMMAND_ACK_MASK, Packet_Parameter1, Packet_Parameter2, Packet_Parameter3)) {
#ifndef NO_DEBUG
                    DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
                }
            } else { /* NOTE: ACK mask has been cleared already */
                if (!Packet_Put(Packet_Command, Packet_Parameter1, Packet_Parameter2, Packet_Parameter3)) {
#ifndef NO_DEBUG
                    DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
                }                
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