/**
 * \file main.c
 * \brief Program main entry file.
 * \author Xu Waycell
 * \date 12-August-2014
 */

#include "main.h"
#include "utils.h"
#include "CRG.h"
#include "clock.h"
#include "EEPROM.h"
#include "packet.h"

#include "mc9s12a512.h"

/**
 * \fn BOOL HandleModConSpecialDebug(void)
 * \brief  
 * \return 
 */
BOOL HandleModConSpecialDebug(void);

/**
 * \fn BOOL HandleModConSpecialVersion(void)
 * \brief Builds a packet that contains ModCon version details and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully.
 */
BOOL HandleModConSpecialVersion(void);

/**
 * \fn BOOL HandleModConNumberGet(void)
 * \brief Builds a packet that contains ModCon number and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully.
 */
BOOL HandleModConNumberGet(void);

/**
 * \fn BOOL Handle_ModCon_Number_Set(void)
 * \brief Assign new value to ModCon number through given packet then update stored EEPROM value. 
 * \return TRUE if write new value to EEPROM is successful.
 */
BOOL HandleModConNumberSet(void);

/**
 * \fn BOOL Handle_ModCon_Mode_Get(void)
 * \brief Builds a packet that contains ModCon mode and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully.
 */
BOOL HandleModConModeGet(void);

/**
 * \fn BOOL Handle_ModCon_Mode_Set(void)
 * \brief Assign new value to ModCon mode through given packet then update stored EEPROM value. 
 * \return TRUE if write new value to EEPROM is successful.
 */
BOOL HandleModConModeSet(void);

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
  /* it should contain zeros */
  if (!Packet_Parameter1 && !Packet_Parameter23)
  {    
    /* push packets of ModCon startup figures */                    
    if (!Packet_Put(MODCON_COMMAND_STARTUP, 0, 0, 0))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
      return bFALSE;    
    }
    return HandleModConSpecialVersion() &&
           HandleModConNumberGet() &&
           HandleModConModeGet();
  }
  return bFALSE;    
}

BOOL HandleModConSpecial(void)
{
  if (Packet_Parameter1 == MODCON_DEBUG_INITIAL && Packet_Parameter2 == MODCON_DEBUG_TOKEN && Packet_Parameter3 == CONTROL_CR)
  {                    
    return HandleModConSpecialDebug();
  }
  if (Packet_Parameter1 == MODCON_VERSION_INITIAL && Packet_Parameter2 == MODCON_VERSION_TOKEN && Packet_Parameter3 == CONTROL_CR)
  {                    
    return HandleModConSpecialVersion();
  }
  return bFALSE;
}

BOOL HandleModConSpecialDebug(void) 
{
  if (!EEPROM_Write16(&ModConDebug, !ModConDebug))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
    return bFALSE;
  }
  return bTRUE;
}

BOOL HandleModConSpecialVersion(void) 
{
  if (!Packet_Put(MODCON_COMMAND_SPECIAL, MODCON_VERSION_INITIAL, MODCON_VERSION_MAJOR, MODCON_VERSION_MINOR))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
    return bFALSE;
  }
  return bTRUE;
}

BOOL HandleModConNumber(void)
{
  switch(Packet_Parameter1)
  {
    case MODCON_NUMBER_GET:
      /* when get parameter2 and 3 is not acceptable */		      
      if (!Packet_Parameter23)
      {        
        return HandleModConNumberGet();
      }
      break;
    case MODCON_NUMBER_SET:
      return HandleModConNumberSet();
      break;
    default:
      break;
  }
  return bFALSE;
}

BOOL HandleModConNumberGet(void)
{
  TUINT16 number;  

  number.l = ModConNumber;
  if (!Packet_Put(MODCON_COMMAND_NUMBER, MODCON_NUMBER_GET, number.s.Lo, number.s.Hi))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
    return bFALSE;
  }
  return bTRUE;
}

BOOL HandleModConNumberSet(void)
{
  if (!EEPROM_Write16(&ModConNumber, Packet_Parameter23))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
    return bFALSE;
  }
  return bTRUE;
}

BOOL HandleModConTime(void)
{
  if (!Packet_Put(MODCON_COMMAND_TIME, MODCON_TIME_INITIAL, Clock_Seconds, Clock_Minutes)) 
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
    return bFALSE;
  }
  return bTRUE;
}

BOOL HandleModConMode(void)
{
	switch(Packet_Parameter1)
	{
		case MODCON_MODE_GET:
      /* when get parameter2 and 3 is not acceptable */		      
		  if (!Packet_Parameter23)
		  {		      
		    return HandleModConModeGet();
		  }
		  break;
		case MODCON_MODE_SET:
      return HandleModConModeSet();
		  break;
		default:
		  break;
	}
  return bFALSE;
}

BOOL HandleModConModeGet(void)
{
  TUINT16 mode;
  
  mode.l = ModConNumber;
  if (!Packet_Put(MODCON_COMMAND_MODE, MODCON_MODE_GET, mode.s.Lo, mode.s.Hi))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
    return bFALSE;
  }
  return bTRUE;
}

BOOL HandleModConModeSet(void)
{
  if (!EEPROM_Write16(&ModConMode, Packet_Parameter23))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
    return bFALSE;
  }
  return bTRUE;
}

BOOL HandleModConEEPROMProgram(void)
{ 
  UINT8 volatile * const address = (UINT8 volatile *)Packet_Parameter12;
    
  if ((UINT16)address >= MODCON_EEPROM_ADDRESS_BEGIN && (UINT16)address <= MODCON_EEPROM_ADDRESS_END)
  {    
    if (address != (UINT8 volatile * const)MODCON_EEPROM_ADDRESS_END)
    {
      if (!EEPROM_Write8(address, Packet_Parameter3))
      {
#ifndef NO_DEBUG
        DEBUG(__LINE__, ERR_EEPROM_WRITE);
#endif
        return bFALSE;      
      }
      return bTRUE;
    }
    if (!EEPROM_Erase())
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_ERASE);
#endif
      return bFALSE;            
    }
    return bTRUE;
  }
  return bFALSE;
}

BOOL HandleModConEEPROMGet(void)
{
  UINT8 volatile * const address = (UINT8 volatile *)Packet_Parameter12;
  
  if (!Packet_Parameter3 && EEPROM_ValidateAddress((void * const)address))
  { 
    if (!Packet_Put(MODCON_COMMAND_EEPROM_GET, Packet_Parameter1, Packet_Parameter2, *address))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
      return bFALSE;
    }
    return bTRUE;
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
  
  if (!CRG_SetupCOP(CONFIG_COPRATE))
  {
#ifndef NO_DEBUG
        DEBUG(__LINE__, ERR_CRGCOP_SETUP);
#endif
    return bFALSE;
  }
    
  if (ModConNumber == 0xFFFF)
  {
    if (!EEPROM_Write16(&ModConNumber, DEFAULT_MODCON_NUMBER))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
      return bFALSE;
    }
  }

  if (ModConMode == 0xFFFF)
  {
    if (!EEPROM_Write16(&ModConMode, DEFAULT_MODCON_MODE))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
      return bFALSE;
    }
  }

  if (ModConDebug == 0xFFFF)
  {
    if (!EEPROM_Write16(&ModConDebug, DEFAULT_MODCON_DEBUG))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
      return bFALSE;
    }
  }

#ifndef NO_INTERRUPT 
  EnableInterrupts;
#else
  DisableInterrupts;
#endif
  
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
        bad = !HandleModConStartup(); 
        break;			
			case MODCON_COMMNAD_EEPROM_PROGRAM:
        bad = !HandleModConEEPROMProgram();
				break;			
			case MODCON_COMMAND_EEPROM_GET:
			  bad = !HandleModConEEPROMGet();
				break;      
      case MODCON_COMMAND_SPECIAL:
        bad = !HandleModConSpecial();
        break;      
      case MODCON_COMMAND_NUMBER:
        bad = !HandleModConNumber();
        break;		  
		  case MODCON_COMMAND_MODE:
		    bad = !HandleModConMode();
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
  { /* hang if initialization failed */
    for(;;);
  }

  TurnOnStartupIndicator();    

  /* queue startup packets for transmission */
  UNUSED(HandleModConStartup());

  for (;;)
  {
    CRG_ArmCOP();
    Routine();
    CRG_DisarmCOP();
  }

}
