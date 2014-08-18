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
#include "CRG.h"
#include "EEPROM.h"
#include "packet.h"

/**
 * \fn BOOL Packet_Put_ModCon_Startup(void)
 * \brief Builds packets that are necessary for startup information and places them into transmit buffer. 
 * \return TRUE if packets were queued for transmission successfully
 */  
BOOL Packet_Put_ModCon_Startup(void);

/**
 * \fn BOOL Packet_Put_ModCon_Version(void)
 * \brief Builds a packet that contains ModCon version details and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully
 */
BOOL Packet_Put_ModCon_Version(void);

/**
 * \fn BOOL Packet_Put_ModCon_Number_Get(void)
 * \brief Builds a packet that contains ModCon number and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully
 */
BOOL Packet_Put_ModCon_Number_Get(void);

/**
 * \fn BOOL Packet_Put_ModCon_Mode_Get(void)
 * \brief Builds a packet that contains ModCon mode and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully
 */
BOOL Packet_Put_ModCon_Mode_Get(void);

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

BOOL Packet_Put_ModCon_Startup(void) {
    return Packet_Put(MODCON_COMMAND_STARTUP, 0, 0, 0) &&
           Packet_Put_ModCon_Version() &&
           Packet_Put_ModCon_Number_Get() &&    
           Packet_Put_ModCon_Mode_Get();    
}

BOOL Packet_Put_ModCon_Version(void) {
    return Packet_Put(MODCON_COMMAND_SPECIAL, MODCON_VERSION_INITIAL, MODCON_VERSION_MAJOR, MODCON_VERSION_MINOR);    
}

BOOL Packet_Put_ModCon_Number_Get(void) {
    return Packet_Put(MODCON_COMMAND_NUMBER, MODCON_NUMBER_GET, ModConNumberLSB, ModConNumberMSB);
}

BOOL Packet_Put_ModCon_Mode_Get(void) {
    return Packet_Put(MODCON_COMMAND_MODE, MODCON_MODE_GET, ModConModeLSB, ModConModeMSB);
}

void Initialize(void) {
    if (!CRG_SetupPLL(CONFIG_BUSCLK, CONFIG_OSCCLK, CONFIG_REFCLK)) {
#ifndef NO_DEBUG
        DEBUG(__LINE__, ERR_CRGPLL_SETUP);
#endif
    }
    if (!CRG_SetupCOP(CONFIG_COPRATE)) {
#ifndef NO_DEBUG
        DEBUG(__LINE__, ERR_CRGCOP_SETUP);
#endif
    }
  	if (!EEPROM_Setup(CONFIG_OSCCLK, CONFIG_BUSCLK)) {
#ifndef NO_DEBUG
        DEBUG(__LINE__, ERR_EEPROM_SETUP);
#endif
	  }
    if (!Packet_Setup(CONFIG_BAUDRATE, CONFIG_BUSCLK)) {
#ifndef NO_DEBUG
        DEBUG(__LINE__, ERR_PACKET_SETUP);
#endif
    }
    ModConNumber.l = 29;
    ModConMode.l = 1;
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
    if (!Packet_Put_ModCon_Startup()) {
#ifndef NO_DEBUG
        DEBUG(__LINE__, ERR_PACKET_PUT);
#endif      
    }
    EEPROM_Erase();
    for (;;) {
        CRG_ArmCOP();
        Routine();
        CRG_DisarmCOP();
    }
}