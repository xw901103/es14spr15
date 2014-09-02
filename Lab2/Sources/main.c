/**
 * \file main.c
 * \brief Program main entry file.
 * \author Xu Waycell
 * \date 06-August-2014
 */

#include "main.h"
#include "utils.h"
#include "CRG.h"
#include "EEPROM.h"
#include "packet.h"

#include "mc9s12a512.h"

//int Error_Code = ERR_NO_ERROR;

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
         Handle_ModCon_Number_Get() &&
         Handle_ModCon_Mode_Get();    
}

BOOL Handle_ModCon_Version(void) 
{
  return Packet_Put(MODCON_COMMAND_SPECIAL, MODCON_VERSION_INITIAL, MODCON_VERSION_MAJOR, MODCON_VERSION_MINOR);    
}

BOOL Handle_ModCon_Number_Get(void)
{
  return Packet_Put(MODCON_COMMAND_NUMBER, MODCON_NUMBER_GET, ModConNumberLSB, ModConNumberMSB);
}

BOOL Handle_ModCon_Number_Set(void)
{
  ModConNumber.l = ForgeWord(Packet_Parameter3, Packet_Parameter2);
  return EEPROM_Write16((UINT16 volatile *)CONFIG_EEPROM_ADDRESS_MODCON_NUMBER, ModConNumber.l);
}

BOOL Handle_ModCon_Mode_Get(void)
{
  return Packet_Put(MODCON_COMMAND_MODE, MODCON_MODE_GET, ModConModeLSB, ModConModeMSB);
}

BOOL Handle_ModCon_Mode_Set(void)
{
  ModConMode.l = ForgeWord(Packet_Parameter3, Packet_Parameter2);
  return EEPROM_Write16((UINT16 volatile *)CONFIG_EEPROM_ADDRESS_MODCON_MODE, ModConMode.l);
}

BOOL Handle_ModCon_EEPROM_Program(void)
{ 
  UINT8 volatile * const address = (UINT8 volatile *)ForgeWord(Packet_Parameter2, Packet_Parameter1);
  if ((UINT16)address >= CONFIG_MODCON_EEPROM_ADDRESS_BEGIN && (UINT16)address <= CONFIG_MODCON_EEPROM_ADDRESS_END)
  {    
    if (address != (UINT8 volatile * const)CONFIG_MODCON_EEPROM_ADDRESS_END)
    {
      return EEPROM_Write8(address, Packet_Parameter3);
    }
    return EEPROM_Erase();
  }
  return bFALSE;
}

BOOL Handle_ModCon_EEPROM_Get(void)
{
  UINT8 volatile * const address = (UINT8 volatile *)ForgeWord(Packet_Parameter2, Packet_Parameter1);
  if ((UINT16)address >= CONFIG_MODCON_EEPROM_ADDRESS_BEGIN && (UINT16)address <= CONFIG_MODCON_EEPROM_ADDRESS_END)
  { 
    if (EEPROM_ValidateAddress((void * const)address))
    {    
      return Packet_Put(MODCON_COMMAND_EEPROM_GET, Packet_Parameter1, Packet_Parameter2, *address);
    }
  }
  return bFALSE;
}

void TurnOnStartupIndicator(void) {
  DDRE_BIT7= 1;   /* set PORT E pin 7 data direction to output */
  PORTE_BIT7 = 0; /* set PORT E pin 7 low */
}

// TODO: return boolen not void
BOOL Initialize(void)
{
  if (!CRG_SetupPLL(CONFIG_BUSCLK, CONFIG_OSCCLK, CONFIG_REFCLK))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_CRGPLL_SETUP);
#endif
    //Error_Code = ERR_CRGPLL_SETUP;
    return bFALSE;
  }
  if (!EEPROM_Setup(CONFIG_OSCCLK, CONFIG_BUSCLK))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_EEPROM_SETUP);
#endif
    //Error_Code = ERR_EEPROM_SETUP;
    return bFALSE;
	}
  if (!Packet_Setup(CONFIG_BAUDRATE, CONFIG_BUSCLK))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_SETUP);
#endif
    //Error_Code = ERR_PACKET_SETUP;
    return bFALSE;
  }
  if (!CRG_SetupCOP(CONFIG_COPRATE)) {
#ifndef NO_DEBUG
        DEBUG(__LINE__, ERR_CRGCOP_SETUP);
#endif
    //Error_Code = ERR_CRGCOP_SETUP;
    return bFALSE;
  }  
  ModConNumber.l = 29;
  ModConMode.l = 1;
  if (EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_NUMBER) == 0xFFFF)
  {
    if (!EEPROM_Write16((UINT16 volatile *)CONFIG_EEPROM_ADDRESS_MODCON_NUMBER, ModConNumber.l))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
      return bFALSE;
    }
  }
  else
  {
    ModConNumber.l = EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_NUMBER);
  }
  
  if (EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_MODE) == 0xFFFF)
  {
    if (!EEPROM_Write16((UINT16 volatile *)CONFIG_EEPROM_ADDRESS_MODCON_MODE, ModConMode.l))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
      return bFALSE;
    }
  }
  else
  {
    ModConMode.l = EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_MODE);
  }
  //Error_Code = ERR_NO_ERROR; 
  return bTRUE;
}

void Routine(void)
{
  UINT8 ack = 0;
  BOOL bad = bFALSE;
    
  if (Packet_Get())
  { 
    ack = Packet_Command & MODCON_COMMAND_ACK_MASK; /* detect ACK mask from command */
    Packet_Command &= ~MODCON_COMMAND_ACK_MASK;     /* clear ACK mask from command */
        
    switch(Packet_Command)
    {
      
      case MODCON_COMMAND_STARTUP:
        if (!Packet_Parameter1 && !Packet_Parameter2 && !Packet_Parameter3) 
        {
          /* push packets of ModCon startup figures */                    
          if (!Handle_ModCon_Startup()) 
          {
#ifndef NO_DEBUG
            DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
            bad = bTRUE;
          }
        }
        break;
			
			case MODCON_COMMNAD_EEPROM_PROGRAM:
        if (!Handle_ModCon_EEPROM_Program())
        {
          bad = bTRUE; 
        }
        /* update ModCon mode and number due to potential mutation */
        ModConNumber.l = EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_NUMBER);
        ModConMode.l = EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_MODE);
				break;			
			case MODCON_COMMAND_EEPROM_GET:
			  if (!Handle_ModCon_EEPROM_Get())
			  {
			    bad = bTRUE;
			  }
				break;      
      case MODCON_COMMAND_SPECIAL:
        if (Packet_Parameter1 == MODCON_VERSION_INITIAL && Packet_Parameter2 == MODCON_VERSION_TOKEN && Packet_Parameter3 == CONTROL_CR)
        {                    
          if (!Handle_ModCon_Version())
          {
#ifndef NO_DEBUG
            DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
            bad = bTRUE;
          }
        }
        else
        {
          bad = bTRUE;
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
            bad = bTRUE;
          }
        }
        else if (Packet_Parameter1 == MODCON_NUMBER_SET)
        {
          if(!Handle_ModCon_Number_Set())
          {
            bad = bTRUE;
          }
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
            bad = bTRUE;
          }
        }
        else if (Packet_Parameter1 == MODCON_MODE_SET)
        {
          if(!Handle_ModCon_Mode_Set())
          {
            bad = bTRUE;
          }
        }
				break;				
      
      default:
        bad = bTRUE;
        break;
    }
        
    if (ack)
    {
      if (!bad)
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
  if (!Initialize())
  {
    /* hang if initialization failed */
    for(;;);
  }

  TurnOnStartupIndicator();    

  /* queue startup packets for transmission */
  if (!Handle_ModCon_Startup())
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_PUT);
#endif      
  }
  for (;;)
  {
    CRG_ArmCOP();
    Routine();
    CRG_DisarmCOP();
  }
}