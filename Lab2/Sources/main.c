/**
 * \mainpage Xu's Embedded Software Lab 2
 *
 * \section intro_sec Introduction
 * Lab 2
 *
 * \file main.c
 * \brief Program main entry file.
 * \author Xu Waycell
 * \date 06-August-2014
 */
#include <hidef.h>          /* common defines and macros */		 
#pragma LINK_INFO DERIVATIVE "mc9s12a512" /* link mc9s12a512's library */

#include "global.h"
#include "packet.h"

/**
 * \fn void Initialize(void)
 * \brief Initializes hardware and software parameters that required for this program.
 */
void Initialize(void);

/**
 * \fn void Routine(void)
 * \brief Retrieves ModCon packets and sends back packets if it is necessary.
 */
void Routine(void);

/**
 * \fn void main(void)
 * \brief The main entry of the program will initialize runtime parameters and keep looping routine.
 */
void main(void);

#ifndef NO_DEBUG
void LogDebug(const UINT16 lineNumber, const UINT16 err) {
    /* break point here */
    UNUSED(lineNumber);
    UNUSED(err);
}
#endif

void Initialize(void) {
    ModConNumber.l = 29;
    ModConMode.l = 1;
    
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
			case MODCON_COMMNAD_EEPROM_PROGRAM:
				break;
			case MODCON_COMMAND_EEPROM_GET:
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
			case MODCON_COMMAND_MODE:
                if (Packet_Parameter1 == MODCON_MODE_GET) {                        
                    if (!Packet_Put_ModCon_Mode_Get()) {
#ifndef NO_DEBUG
                        DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
                    }
                } else if (Packet_Parameter1 == MODCON_MODE_SET) {
                    ModConModeLSB = Packet_Parameter2;
                    ModConModeMSB = Packet_Parameter3;
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