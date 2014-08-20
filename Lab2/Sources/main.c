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

#include "main.h"
#include "CRG.h"
#include "EEPROM.h"
#include "packet.h"

#ifndef NO_DEBUG
void LogDebug(const UINT16 lineNumber, const UINT16 err) {
    /* break point here */
    UNUSED(lineNumber);
    UNUSED(err);
}
#endif

BOOL Handle_ModCon_Startup(void) 
{
  return Packet_Put(MODCON_COMMAND_STARTUP, 0, 0, 0) &&
         Handle_ModCon_Version() &&
         Handle_ModCon_Number_Get();    
}

BOOL Handle_ModCon_Version(void) 
{
  return Packet_Put(MODCON_COMMAND_SPECIAL, MODCON_VERSION_INITIAL, MODCON_VERSION_MAJOR, MODCON_VERSION_MINOR);    
}

BOOL Handle_ModCon_Number_Get(void)
{
  return Packet_Put(MODCON_COMMAND_NUMBER, MODCON_NUMBER_GET, ModConNumberLSB, ModConNumberMSB);
}

BOOL Handle_ModCon_Number_Set(const UINT16 number)
{
  ModConNumberLSB = Packet_Parameter2;
  ModConNumberMSB = Packet_Parameter3;
  return bFALSE;
}

BOOL Handle_ModCon_Mode_Get(void)
{
  return Packet_Put(MODCON_COMMAND_MODE, MODCON_MODE_GET, ModConModeLSB, ModConModeMSB);
}

BOOL Handle_ModCon_Mode_Set(const UINT16 mode)
{
  ModConModeLSB = Packet_Parameter2;
  ModConModeMSB = Packet_Parameter3;
  return bFALSE;
}

BOOL Handle_ModCon_EEPROM_Program(UINT8 volatile * const address, const UINT8 data)
{
  return bFALSE;
}

BOOL Handle_ModCon_EEPROM_Get(UINT8 volatile * const address)
{
  return bFALSE;
}

void Initialize(void)
{
  if (!CRG_SetupPLL(CONFIG_BUSCLK, CONFIG_OSCCLK, CONFIG_REFCLK))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_CRGPLL_SETUP);
#endif
  }
    /*
    if (!CRG_SetupCOP(CONFIG_COPRATE)) {
#ifndef NO_DEBUG
        DEBUG(__LINE__, ERR_CRGCOP_SETUP);
#endif
    }*/
  if (!EEPROM_Setup(CONFIG_OSCCLK, CONFIG_BUSCLK))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_EEPROM_SETUP);
#endif
	}
  if (!Packet_Setup(CONFIG_BAUDRATE, CONFIG_BUSCLK))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_SETUP);
#endif
  }
  ModConNumber.l = 29;
  ModConMode.l = 1;
  if (*(UINT16 volatile *)0x0400 == 0xFFFF)
  {
    if (!EEPROM_Write16((UINT16 volatile *)0x0400, ModConNumber.l))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
    }
  }
  else
  {
    ModConNumber.l = *(UINT16 volatile *)0x0400;
  }
  if (*(UINT16 volatile *)0x0402 == 0xFFFF)
  {
    if (EEPROM_Write16((UINT16 volatile *)0x0402, ModConMode.l))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
    }
  }
  else
  {
    ModConMode.l = *(UINT16 volatile *)0x0402;
  }
}

UINT16 Util_Merge16(UINT8 hi, UINT8 lo) {
  TUINT16 CACHE;
  CACHE.s.Hi = hi;
  CACHE.s.Lo = lo;
  return CACHE.l;
}

void Routine(void)
{
  UINT8 ack = 0;
  BOOL deny = bFALSE;
  UINT8 volatile * EEPROM_ADDRESS = 0;
    
  if (Packet_Get())
  { 
    ack = Packet_Command & MODCON_COMMAND_ACK_MASK; /* detect ACK mask from command */
    Packet_Command &= ~MODCON_COMMAND_ACK_MASK;     /* clear ACK mask from command */
        
    switch(Packet_Command)
    {
      case MODCON_COMMAND_STARTUP:
        if (!Packet_Parameter1 && !Packet_Parameter2 && !Packet_Parameter3) 
        {                    
          if (!Handle_ModCon_Startup()) 
          { /* push first three packets of ModCon startup figures */
#ifndef NO_DEBUG
            DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
          }
        }
        break;
			case MODCON_COMMNAD_EEPROM_PROGRAM:
			  EEPROM_ADDRESS = (UINT8 volatile *)Util_Merge16(Packet_Parameter2, Packet_Parameter1);
        if (!Handle_ModCon_EEPROM_Program(EEPROM_ADDRESS, Packet_Parameter3))
        {
        }
//			  if (!EEPROM_Write8(EEPROM_ADDRESS, Packet_Parameter3))
//			  {
#ifndef NO_DEBUG
//          DEBUG(__LINE__, ERR_EEPROM_WRITE);
#endif			            
//			  }
				break;
			case MODCON_COMMAND_EEPROM_GET:
			  EEPROM_ADDRESS = (UINT8 volatile *)Util_Merge16(Packet_Parameter2, Packet_Parameter1);
			  if (!Handle_ModCon_EEPROM_Get(EEPROM_ADDRESS))
			  {
			  }
			  //if(!Packet_Put(MODCON_COMMAND_EEPROM_GET, Packet_Parameter1, Packet_Parameter2, *EEPROM_ADDRESS))
			  //{
#ifndef NO_DEBUG
        //  DEBUG(__LINE__, ERR_PACKET_PUT);
#endif			          
			  //}
				break;
      case MODCON_COMMAND_SPECIAL:
        if (Packet_Parameter1 == MODCON_VERSION_INITIAL && Packet_Parameter2 == MODCON_VERSION_TOKEN && Packet_Parameter3 == CONTROL_CR)
        {                    
          if (!Handle_ModCon_Version())
          {
#ifndef NO_DEBUG
            DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
          }
        }
        else
        {
          deny = bTRUE;
        }
        break;
      case MODCON_COMMAND_NUMBER:
        if (Packet_Parameter1 == MODCON_NUMBER_GET)
        {                        
          if (!Handle_ModCon_Number_Get())
          {
#ifndef NO_DEBUG
            DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
          }
        }
        else if (Packet_Parameter1 == MODCON_NUMBER_SET)
        {
          Handle_ModCon_Number_Set(0);
          ModConNumberLSB = Packet_Parameter2;
          ModConNumberMSB = Packet_Parameter3;
        }
        break;
		  case MODCON_COMMAND_MODE:
        if (Packet_Parameter1 == MODCON_MODE_GET)
        {                        
          if (!Handle_ModCon_Mode_Get())
          {
#ifndef NO_DEBUG
            DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
          }
        }
        else if (Packet_Parameter1 == MODCON_MODE_SET)
        {
          Handle_ModCon_Mode_Set(0);
          ModConModeLSB = Packet_Parameter2;
          ModConModeMSB = Packet_Parameter3;
        }
				break;				
      default:
        deny = bTRUE;
        break;
    }
        
    if (ack)
    {
      if (!deny)
      {                
        if (!Packet_Put(Packet_Command | MODCON_COMMAND_ACK_MASK, Packet_Parameter1, Packet_Parameter2, Packet_Parameter3))
        {
#ifndef NO_DEBUG
          DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
        }
      }
      else
      { /* NOTE: ACK mask has been cleared already */
        if (!Packet_Put(Packet_Command, Packet_Parameter1, Packet_Parameter2, Packet_Parameter3))
        {
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
  /* queue startup packets for transmission */
  if (!Handle_ModCon_Startup())
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_PUT);
#endif      
  }
  for (;;)
  {
    //CRG_ArmCOP();
    Routine();
    //CRG_DisarmCOP();
  }
}