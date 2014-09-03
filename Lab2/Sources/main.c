/**
 * \file main.c
 * \brief Program main entry file.
 * \author Xu Waycell
 * \date 12-August-2014
 */

#include "main.h"
#include "utils.h"
#include "CRG.h"
#include "EEPROM.h"
#include "packet.h"

#include "mc9s12a512.h"

#ifndef NO_DEBUG
void LogDebug(const UINT16 lineNumber, const UINT16 err)
{
    /* break point here */
    UNUSED(lineNumber);
    UNUSED(err);
}
#endif

BOOL HandleModConStartup(void) 
{
  if (!Packet_Parameter1 && !Packet_Parameter2 && !Packet_Parameter3)
  {    
    return Packet_Put(MODCON_COMMAND_STARTUP, 0, 0, 0) &&
           HandleModConSpecialVersion() &&
           HandleModConNumberGet() &&
           HandleModConModeGet();
  }
  return bFALSE;    
}

BOOL HandleModConSpecialVersion(void) 
{
  return Packet_Put(MODCON_COMMAND_SPECIAL, MODCON_VERSION_INITIAL, MODCON_VERSION_MAJOR, MODCON_VERSION_MINOR);    
}

BOOL HandleModConNumberGet(void)
{
  return Packet_Put(MODCON_COMMAND_NUMBER, MODCON_NUMBER_GET, ModConNumberLSB, ModConNumberMSB);
}

BOOL HandleModConNumberSet(void)
{
  ModConNumber.l = ForgeWord(Packet_Parameter3, Packet_Parameter2);
  return EEPROM_Write16((UINT16 volatile *)CONFIG_EEPROM_ADDRESS_MODCON_NUMBER, ModConNumber.l);
}

BOOL HandleModConModeGet(void)
{
  return Packet_Put(MODCON_COMMAND_MODE, MODCON_MODE_GET, ModConModeLSB, ModConModeMSB);
}

BOOL HandleModConModeSet(void)
{
  ModConMode.l = ForgeWord(0, Packet_Parameter2);
  return EEPROM_Write16((UINT16 volatile *)CONFIG_EEPROM_ADDRESS_MODCON_MODE, ModConMode.l);
}

BOOL HandleModConEEPROMProgram(void)
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
  /* update ModCon mode and number due to potential mutation */
  //ModConNumber.l = EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_NUMBER);
  //ModConMode.l = EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_MODE);  
  return bFALSE;
}

BOOL HandleModConEEPROMGet(void)
{
  UINT8 volatile * const address = (UINT8 volatile *)ForgeWord(Packet_Parameter2, Packet_Parameter1);
  
  if (!Packet_Parameter3 && (UINT16)address >= CONFIG_MODCON_EEPROM_ADDRESS_BEGIN && (UINT16)address <= CONFIG_MODCON_EEPROM_ADDRESS_END)
  { 
    if (EEPROM_ValidateAddress((void * const)address))
    {    
      return Packet_Put(MODCON_COMMAND_EEPROM_GET, Packet_Parameter1, Packet_Parameter2, *address);
    }
  }
  return bFALSE;
}

void TurnOnStartupIndicator(void)
{
  DDRE_BIT7= 1;   /* Port E pin 7 data direction  0= in  1= out */
  PORTE_BIT7 = 0; /* Port E pin 7 state           0= low 1= high */
}

BOOL Initialize(void)
{
  if (!CRG_SetupPLL(CONFIG_BUSCLK, CONFIG_OSCCLK, CONFIG_REFCLK))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_CRGPLL_SETUP);
#endif
    return bFALSE;
  }
  
  if (!EEPROM_Setup(CONFIG_OSCCLK, CONFIG_BUSCLK))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_EEPROM_SETUP);
#endif
    return bFALSE;
	}
	
  if (!Packet_Setup(CONFIG_BAUDRATE, CONFIG_BUSCLK))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_SETUP);
#endif
    return bFALSE;
  }
  
  if (!CRG_SetupCOP(CONFIG_COPRATE)) {
#ifndef NO_DEBUG
        DEBUG(__LINE__, ERR_CRGCOP_SETUP);
#endif
    return bFALSE;
  }
  
  /* initialize default value of ModCon number and mode */  
  ModConNumber.l = 29;
  ModConMode.l = 1;

  /* load ModeCon number from EEPROM */
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
  
  /* load ModeCon mode from EEPROM */
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
        /* it should contain zeros */
        //if (!Packet_Parameter1 && !Packet_Parameter2 && !Packet_Parameter3) 
        //{
          /* push packets of ModCon startup figures */                    
          if (!HandleModConStartup()) 
          {
#ifndef NO_DEBUG
            DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
            bad = bTRUE;
          }
        //}
        //else
        //{
        //  bad = bTRUE;  
        //}
        break;			
			case MODCON_COMMNAD_EEPROM_PROGRAM:
        //if (!HandleModConEEPROMProgram())
        //{
        //  bad = bTRUE; 
        //}
        bad = !HandleModConEEPROMProgram();
				break;			
			case MODCON_COMMAND_EEPROM_GET:
			  //if (!Packet_Parameter3) /* parameter 3 must be zero */
			  //{			    
			  //  if (!HandleModConEEPROMGet())
			  //  {
			  //    bad = bTRUE;
			  //  }
			  //}
			  //else
			  //{
			  //  bad = bTRUE;  
			  //}
			  bad = !HandleModConEEPROMGet();
				break;      
      case MODCON_COMMAND_SPECIAL:
        if (Packet_Parameter1 == MODCON_VERSION_INITIAL && Packet_Parameter2 == MODCON_VERSION_TOKEN && Packet_Parameter3 == CONTROL_CR)
        {                    
          if (!HandleModConSpecialVersion())
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
          if (!HandleModConNumberGet())
          {
#ifndef NO_DEBUG
            DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
            bad = bTRUE;
          }
        }
        else if (Packet_Parameter1 == MODCON_NUMBER_SET)
        {
          if(!HandleModConNumberSet())
          {
            bad = bTRUE;
          }
        }
        else /* unsupported parameter1 in ModCon number */
        {
          bad = bTRUE;
        }
        break;		  
		  case MODCON_COMMAND_MODE:
		    if (!Packet_Parameter3) /* parameter3 shall be zero */
		    {
		      /* when get parameter2 and 3 is not acceptable */		      
          if (Packet_Parameter1 == MODCON_MODE_GET && !Packet_Parameter2 && !Packet_Parameter3)
          { 
            if (!HandleModConModeGet())
            {
#ifndef NO_DEBUG
              DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
              bad = bTRUE;
            }
          }
          else if (Packet_Parameter1 == MODCON_MODE_SET)
          {
            if(!HandleModConModeSet())
            {
              bad = bTRUE;
            }
          }
          else
          {
            bad = bTRUE;
          }
		    }
		    else
		    {
		      bad = bTRUE;  
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
  if (!HandleModConStartup())
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