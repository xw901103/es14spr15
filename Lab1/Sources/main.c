/**
 * \mainpage Xu's Embedded Software Lab 1
 * \section intro_sec Introduction
 * Lab 1 is to implement a program that responds ModCon protocol via Serial Communication Interface
 * on ModCon board running at bus clock 8 Mhz with 38400 baud rate.
 * \file main.c
 * \brief Program main entry
 * \author Xu Waycell
 * \date
 */
#include <hidef.h>      /* common defines and macros */		 
#pragma LINK_INFO DERIVATIVE "mc9s12a512" /* link mc9s12a512's library */

#include "global.h"
#include "packet.h"

/**
 * 
 */
void Initialize(void);
void Routine(void);
void main(void);

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
    
    if (Packet_Get()) { 
        ACK = Packet_Command & MODCON_COMMAND_ACK_MASK; /* detect ACK mask from command */
        Packet_Command &= ~MODCON_COMMAND_ACK_MASK;     /* clear ACK mask from command */
        
        switch(Packet_Command) {
            case MODCON_COMMAND_STARTUP:
                if (!Packet_Put_ModCon_Startup()) { /* push first three packets */
#ifndef NO_DEBUG
                    DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
                }
                break;
            case MODCON_COMMAND_SPECIAL:
                if (Packet_Parameter1 == MODCON_VERSION_INITIAL && Packet_Parameter2 == MODCON_VERSION_TOKEN && Packet_Parameter3 == CONTROL_CR) {                    
                    if (!Packet_Put_ModCon_Version()) {
#ifndef NO_DEBUG
                        DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
                    }
                } else {
                    DENY = bTRUE;
                }
                break;
            case MODCON_COMMAND_NUMBER:
                if (Packet_Parameter1 == MODCON_NUMBER_GET) {                        
                    if (!Packet_Put_ModCon_Number_Get()) {
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
  
    for (;;) {
        Routine();
    }
}

#ifndef NO_DEBUG
void LogDebug(const UINT16 lineNumber, const UINT16 err) {
    /* break point here */
    UNUSED(lineNumber);
    UNUSED(err);
}
#endif
