/**
 * \file main.c
 * \brief Program main entry file.
 * \author Xu Waycell
 * \date 12-August-2014
 */

#include "main.h"
#include "CRG.h"
#include "clock.h"
#include "timer.h"
#include "EEPROM.h"
#include "packet.h"
//#include "LCD.h"
//#include "HMI.h"
#include "utils.h"
#include <mc9s12a512.h>

/**
 * \fn BOOL HandleModConSpecialDebug(void)
 * \brief Toggles ModCon debug attribute 
 * \return TRUE if the new debugging state has been updated.
 */
BOOL HandleModConSpecialDebug(void);

/**
 * \fn BOOL HandleModConSpecialVersion(void)
 * \brief Builds a packet that contains ModCon version details and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully.
 */
BOOL HandleModConSpecialVersion(void);

/**
 * \fn BOOL HandleModConProtocolModeGet(void)
 * \brief
 * \return TRUE if the command has been executed successfully.
 */
BOOL HandleModConProtocolModeGet(void);

/**
 * \fn BOOL HandleModConProtocolModeSet(void)
 * \brief
 * \return TRUE if the command has been executed successfully.
 */
BOOL HandleModConProtocolModeSet(void);

/**
 * \fn BOOL HandleModConNumberGet(void)
 * \brief Builds a packet that contains ModCon number and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully.
 */
BOOL HandleModConNumberGet(void);

/**
 * \fn BOOL HandleModConNumberSet(void)
 * \brief Assign new value to ModCon number through given packet then update stored EEPROM value. 
 * \return TRUE if write new value to EEPROM is successful.
 */
BOOL HandleModConNumberSet(void);

/**
 * \fn BOOL HandleModConModeGet(void)
 * \brief Builds a packet that contains ModCon mode and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully.
 */
BOOL HandleModConModeGet(void);

/**
 * \fn BOOL HandleModConModeSet(void)
 * \brief Assign new value to ModCon mode through given packet then update stored EEPROM value. 
 * \return TRUE if write new value to EEPROM is successful.
 */
BOOL HandleModConModeSet(void);

/**
 * \fn void TurnOnStartupIndicator(void)
 * \brief turn on the Port E pin 7 connected LED.
 */
void TurnOnStartupIndicator(void);

/**
 * \fn void SampleAnalogInputChannels(void)
 * \brief Samples analog input values from enabled channels and send packets based on protocol mode asynchronous/synchronous.
 */
void SampleAnalogInputChannels(void);

#ifndef NO_DEBUG
/**
 * \fn void LogDebug(const UINT16 lineNumber, const UINT16 err)
 * \brief Logs debug information including line number and error number.
 * \param lineNumber line number of source file
 * \param err error number refers to predefined errors
 */
void LogDebug(const UINT16 lineNumber, const UINT16 err)
{
    /* break point here */
    UNUSED(lineNumber);
    UNUSED(err);
}
#endif

/**
 * \fn BOOL HandleModConStartup(void)
 * \brief Builds packets that are necessary for startup information and places them into transmit buffer. 
 * \return TRUE if packets were queued for transmission successfully.
 */  
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
           HandleModConProtocolModeGet() &&
           HandleModConNumberGet() &&
           HandleModConModeGet();
  }
  return bFALSE;    
}

/**
 * \fn BOOL HandleModConSpecial(void)
 * \brief response to ModCon special commands. 
 * \return TRUE if the command has been executed successfully.
 */
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

/**
 * \fn BOOL HandleModConSpecialDebug(void)
 * \brief Toggles ModCon debug attribute 
 * \return TRUE if the new debugging state has been updated.
 */
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

/**
 * \fn BOOL HandleModConSpecialVersion(void)
 * \brief Builds a packet that contains ModCon version details and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully.
 */
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

/**
 * \fn BOOL HandleModConProtocolMode(void)
 * \brief response to ModCon protocol commands. 
 * \return TRUE if the command has been executed successfully.
 */
BOOL HandleModConProtocolMode(void)
{
  /* parameter3 is not acceptable */    
  if (!Packet_Parameter3)
  {    
    switch(Packet_Parameter1)
    {
      case MODCON_PROTOCOL_MODE_GET:
        /* when get parameter2 is not acceptable */    
        if (!Packet_Parameter2)
        {
          return HandleModConProtocolModeGet();
        }
        break;
      case MODCON_PROTOCOL_MODE_SET:
        return HandleModConProtocolModeSet();
        break;
      default:
        break;
    }
  }
  return bFALSE;
}

/**
 * \fn BOOL HandleModConProtocolModeGet(void)
 * \brief Builds a packet that contains ModCon protocol mode and places it into transmit buffer.
 * \return TRUE if the packet was queued for transmission successfully.
 */
BOOL HandleModConProtocolModeGet(void)
{
  if (!Packet_Put(MODCON_COMMAND_PROTOCOL_MODE, MODCON_PROTOCOL_MODE_GET, (UINT8)ModConProtocolMode ,0))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
    return bFALSE;
  }
  return bTRUE;
}

/**
 * \fn BOOL HandleModConProtocolModeSet(void)
 * \brief Assign new value to ModCon protocol mode through given packet by updating stored EEPROM value.
 * \return TRUE if write new value to EEPROM is successful.
 */
BOOL HandleModConProtocolModeSet(void)
{
  if (Packet_Parameter2 == MODCON_PROTOCOL_MODE_ASYNCHRONOUS || Packet_Parameter2 == MODCON_PROTOCOL_MODE_SYNCHRONOUS)
  {
    if (!EEPROM_Write16(&ModConProtocolMode, (UINT16)Packet_Parameter2))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
      return bFALSE;
    }
    /* send back current protocol mode to confirm */
    UNUSED(HandleModConProtocolModeGet());
    return bTRUE;  
  }
  return bFALSE;
}

/**
 * \fn BOOL HandleModConNumber(void)
 * \brief response to ModCon number commands. 
 * \return TRUE if the command has been executed successfully.
 */
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

/**
 * \fn BOOL HandleModConNumberGet(void)
 * \brief Builds a packet that contains ModCon number and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully.
 */
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

/**
 * \fn BOOL HandleModConNumberSet(void)
 * \brief Assign new value to ModCon number through given packet by updating stored EEPROM value. 
 * \return TRUE if write new value to EEPROM is successful.
 */
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

/**
 * \fn BOOL HandleModConUptime(void)
 * \brief Sends out system uptime in minutes and seconds 
 * \return TRUE if the packet has queued successfully.
 */
BOOL HandleModConUptime(void)
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

/**
 * \fn BOOL HandleModConMode(void)
 * \brief response to ModCon mode commands. 
 * \return TRUE if the command has been executed successfully.
 */
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

/**
 * \fn BOOL HandleModConModeGet(void)
 * \brief Builds a packet that contains ModCon mode and places it into transmit buffer. 
 * \return TRUE if the packet was queued for transmission successfully.
 */
BOOL HandleModConModeGet(void)
{
  TUINT16 mode;
  
  mode.l = ModConMode;
  if (!Packet_Put(MODCON_COMMAND_MODE, MODCON_MODE_GET, mode.s.Lo, mode.s.Hi))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
    return bFALSE;
  }
  return bTRUE;
}

/**
 * \fn BOOL HandleModConModeSet(void)
 * \brief Assign new value to ModCon mode through given packet then update stored EEPROM value. 
 * \return TRUE if write new value to EEPROM is successful.
 */
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

/**
 * \fn BOOL HandleModConAnalogInputValue(const TAnalogChannel channelNb)
 * \brief Builds a packet that contains current ModCon analog input value of selected channel and places it into transmit buffer. 
 * \return TRUE if the command has been executed successfully.
 */
BOOL HandleModConAnalogInputValue(const TAnalogChannel channelNb)
{
  UINT8 index = 0xFF;
  
  /* NOTE: channel enums might not be in numberic order */
  switch(channelNb)
  {
    case ANALOG_INPUT_Ch1:
      index = 0;
      break;
    case ANALOG_INPUT_Ch2:
      index = 1;
      break;
    case ANALOG_INPUT_Ch3:
      index = 2;
      break;
    case ANALOG_INPUT_Ch4:
      index = 3;
      break;
    case ANALOG_INPUT_Ch5:
      index = 4;
      break;
    case ANALOG_INPUT_Ch6:
      index = 5;
      break;
    case ANALOG_INPUT_Ch7:
      index = 6;
      break;
    case ANALOG_INPUT_Ch8:
      index = 7;
      break;
    default:
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_INVALID_ARGUMENT);
#endif
      return bFALSE;
      break;
  }
  
  if (!Packet_Put(MODCON_COMMAND_ANALOG_INPUT_VALUE, index, Analog_Input[index].Value.s.Lo, Analog_Input[index].Value.s.Hi))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
    return bFALSE;
  }
  return bTRUE;
}

/**
 * \fn BOOL HandleModConEEPROMProgram(void)
 * \brief Program a byte in EEPROM by given address.
 * \return TRUE if EEPROM program successfully. 
 */
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

/**
 * \fn BOOL HandleModConEEPROMGet(void)
 * \brief Return byte value of given EEPROM address.
 * \return TRUE if address is validated and the packet was queued for transmission successfully.
 */
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

/**
 * \fn void TurnOnStartupIndicator(void)
 * \brief turn on the Port E pin 7 connected LED.
 */
void TurnOnStartupIndicator(void)
{
  DDRE_BIT7 = 1;   /* Port E pin 7 data direction  0= in  1= out */
  PORTE_BIT7 = 0; /* Port E pin 7 state           0= low 1= high */
}

/**
 * \fn void SampleAnalogInputChannels(void)
 * \brief Samples analog input values from enabled channels and send packets based on protocol mode asynchronous/synchronous.
 */
void SampleAnalogInputChannels(void) 
{
  /* lookup tables for input analog channel enums and switch mask mapping */
  static const UINT8
  /* mask byte | Ch8 | Ch7 | Ch6 | Ch5 | Ch4 | Ch3 | Ch2 | Ch1 | */
  inputChannelSwitchMaskLookupTable[NB_INPUT_CHANNELS] = { MODCON_ANALOG_INPUT_CHANNEL_MASK_CH1,
                                                           MODCON_ANALOG_INPUT_CHANNEL_MASK_CH2, 
                                                           MODCON_ANALOG_INPUT_CHANNEL_MASK_CH3, 
                                                           MODCON_ANALOG_INPUT_CHANNEL_MASK_CH4, 
                                                           MODCON_ANALOG_INPUT_CHANNEL_MASK_CH5, 
                                                           MODCON_ANALOG_INPUT_CHANNEL_MASK_CH6, 
                                                           MODCON_ANALOG_INPUT_CHANNEL_MASK_CH7,
                                                           MODCON_ANALOG_INPUT_CHANNEL_MASK_CH8 },
  /* NOTE: channel enums might not be in numeric order */
  inputChannelNumberLookupTable[NB_INPUT_CHANNELS] = { ANALOG_INPUT_Ch1,
                                                       ANALOG_INPUT_Ch2,
                                                       ANALOG_INPUT_Ch3,
                                                       ANALOG_INPUT_Ch4,
                                                       ANALOG_INPUT_Ch5,
                                                       ANALOG_INPUT_Ch6,
                                                       ANALOG_INPUT_Ch7,
                                                       ANALOG_INPUT_Ch8 };
  BOOL mutated = bFALSE;
  UINT8 index = 0;
  
  for (index = 0; index < NB_INPUT_CHANNELS; ++index) 
  {
    if (ModConAnalogInputChannelSwitch & inputChannelSwitchMaskLookupTable[index])
    {      
      mutated = Analog_Get(inputChannelNumberLookupTable[index]);
      if (ModConProtocolMode == MODCON_PROTOCOL_MODE_SYNCHRONOUS)
      { 
        /* NOTE: debug is inside HandleModConAnalogInputValue */ 	
  	    UNUSED(HandleModConAnalogInputValue(inputChannelNumberLookupTable[index])); 
      }
      else if (ModConProtocolMode == MODCON_PROTOCOL_MODE_ASYNCHRONOUS && mutated)
      {
        /* NOTE: debug is inside HandleModConAnalogInputValue */
  	    UNUSED(HandleModConAnalogInputValue(inputChannelNumberLookupTable[index]));    
      }
    }
  }
}

/**
 * \fn void Initialize(void)
 * \brief Initializes hardware and software parameters that required for this program.
 * \return TRUE if initialization routines executed successfully.
 */
BOOL Initialize(void) /* TODO: check following statements */
{ 
  DisableInterrupts;
 
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
		
  if (!Packet_Setup(CONFIG_SCI_BAUDRATE, CONFIG_BUSCLK))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_SETUP);
#endif
    return bFALSE;
  }
  
  if (!CRG_SetupCOP(CONFIG_COP_RATE))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_CRGCOP_SETUP);
#endif
    return bFALSE;
  }
        
  if (ModConProtocolMode == 0xFFFF)
  {
    if (!EEPROM_Write16(&ModConProtocolMode, DEFAULT_MODCON_PROTOCOL_MODE))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
      return bFALSE;
    }
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
  
  if (ModConAnalogInputChannelSwitch == 0xFFFF)
  {
    if (!EEPROM_Write16(&ModConAnalogInputChannelSwitch, DEFAULT_MODCON_ANALOG_INPUT_CHANNEL_SWITCH))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
      return bFALSE;      
    }
  }

  if (ModConAnalogOutputChannelSwitch == 0xFFFF)
  {
    if (!EEPROM_Write16(&ModConAnalogOutputChannelSwitch, DEFAULT_MODCON_ANALOG_OUTPUT_CHANNEL_SWITCH))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
      return bFALSE;      
    }
  }

  if (ModConAnalogSamplingRate == 0xFFFF)
  {
    if (!EEPROM_Write16(&ModConAnalogSamplingRate, DEFAULT_MODCON_ANALOG_SAMPLING_RATE))
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
  
  if (ModConHMIBacklight == 0xFFFF)
  {
    if (!EEPROM_Write16(&ModConHMIBacklight, DEFAULT_MODCON_HMI_BACKLIGHT))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
      return bFALSE;
    }
  }

  if (ModConHMIContrast == 0xFFFF)
  {
    if (!EEPROM_Write16(&ModConHMIContrast, DEFAULT_MODCON_HMI_CONTRAST))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
      return bFALSE;
    }
  }
  
  Clock_Setup(CONFIG_RTI_PRESCALERATE, CONFIG_RTI_MODULUSCOUNT);
    
  Timer_Setup();
  Timer_SetupPeriodicTimer(ModConAnalogSamplingRate, CONFIG_BUSCLK);
  Timer_AttachPeriodicTimerRoutine(&SampleAnalogInputChannels);

  Analog_Setup(CONFIG_BUSCLK);

  MODCON_HMI_SETUP.backlight = ModConHMIBacklight;
  MODCON_HMI_SETUP.contrast = ModConHMIContrast;
  if (!HMI_Setup(&MODCON_HMI_SETUP))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_HMI_SETUP);
#endif
    return bFALSE;
  }  
  
  HMI_AppendPanel(&MODCON_HMI_IDLE_PANEL);  
  HMI_AppendPanel(&MODCON_HMI_SETTING_PANEL);
  HMI_AppendPanel(&MODCON_HMI_ANALOG_PANEL);
  HMI_AppendPanel(&MODCON_HMI_SWITCH_PANEL);
  HMI_AppendPanel(&MODCON_HMI_CONFIRM_PANEL);  
  
  Timer_PeriodicTimerEnable(bTRUE);
  
#ifndef NO_INTERRUPT 
  EnableInterrupts;
#endif
  
  return bTRUE;
}

/**
 * \fn void Routine(void)
 * \brief Retrieves ModCon packets and sends back packets if it is necessary.
 */
void Routine(void)
{
  UINT8 ack = 0;
  BOOL bad = bFALSE;
    
  if (Clock_Update())
  {
    /* hours from 0 to 23; minutes from 0 to 59; seconds from 0 to 59 */
    HMI_SetTime((Clock_Minutes / 60) % 24, Clock_Minutes % 60, Clock_Seconds);    
    bad = !HandleModConUptime();
  }
      
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
      case MODCON_COMMAND_PROTOCOL_MODE:
        bad = !HandleModConProtocolMode();
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

/**
 * \fn void main(void)
 * \brief The main entry of the program will initialize runtime parameters and keep looping routine.
 */
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

/* TODO: organize following menu item updater */
void UpdateMenuItemVersion(void)
{
  MODCON_HMI_MENU_ITEM_VERSION.value.v.Major = MODCON_VERSION_MAJOR;
  MODCON_HMI_MENU_ITEM_VERSION.value.v.Minor = MODCON_VERSION_MINOR;
}

void UpdateMenuItemNumber(void)
{
  MODCON_HMI_MENU_ITEM_NUMBER.value.l = ModConNumber;
}

void UpdateMenuItemDebug(void)
{
  MODCON_HMI_MENU_ITEM_DEBUG.value.b.Boolean = (UINT8)ModConDebug; 
}

void UpdateMenuItemProtocol(void)
{
  MODCON_HMI_MENU_ITEM_PROTOCOL.value.b.Boolean =(UINT8)ModConProtocolMode;
}

void UpdateMenuItemBacklight(void)
{
  MODCON_HMI_MENU_ITEM_LCD_BACKLIGHT.value.b.Boolean = (UINT8)ModConHMIBacklight;
}

void UpdateMenuItemContrast(void)
{
  MODCON_HMI_MENU_ITEM_LCD_CONTRAST.value.l = ModConHMIContrast;
}

/* analog channels update routines */
void UpdateMenuItemAnalogInputCh1Value(void)
{
  MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH1_VALUE.value.f.Float = ((Analog_Input[0].Value.l * 49 + 7) / 100);
}

void UpdateMenuItemAnalogInputCh2Value(void)
{
  MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH2_VALUE.value.l = (((INT16)Analog_Input[1].Value.l * 49 + 7) / 100);
}

void UpdateMenuItemAnalogInputCh3Value(void)
{
  MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH3_VALUE.value.l = (((INT16)Analog_Input[2].Value.l * 49 + 7) / 100);
}

void UpdateMenuItemAnalogInputCh4Value(void)
{
  MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH4_VALUE.value.l = (((INT16)Analog_Input[3].Value.l * 49 + 7) / 100);
}

void UpdateMenuItemAnalogInputCh5Value(void)
{
  MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH5_VALUE.value.l = (((INT16)Analog_Input[3].Value.l * 49 + 7) / 100);
}

void UpdateMenuItemAnalogInputCh6Value(void)
{
  MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH6_VALUE.value.l = (((INT16)Analog_Input[3].Value.l * 49 + 7) / 100);
}

void UpdateMenuItemAnalogInputCh7Value(void)
{
  MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH7_VALUE.value.l = (((INT16)Analog_Input[3].Value.l * 49 + 7) / 100);
}

void UpdateMenuItemAnalogInputCh8Value(void)
{
  MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH8_VALUE.value.l = (((INT16)Analog_Input[3].Value.l * 49 + 7) / 100);
}

void UpdateMenuItemAnalogOutputCh1Value(void)
{
  MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH1_VALUE.value.l = Analog_Output[0].Value.l;
}

void UpdateMenuItemAnalogOutputCh2Value(void)
{
}

void UpdateMenuItemAnalogOutputCh3Value(void)
{
}

void UpdateMenuItemAnalogOutputCh4Value(void)
{
}
/* analog channels update routines */

/* switch update routines */
void UpdateMenuItemAnalogInputCh1Switch(void)
{
  if (ModConAnalogInputChannelSwitch & MODCON_ANALOG_INPUT_CHANNEL_MASK_CH1)
  {    
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH1_SWITCH.value.b.Boolean = bTRUE;
  }
  else
  {
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH1_SWITCH.value.b.Boolean = bFALSE;
  }
}

void UpdateMenuItemAnalogInputCh2Switch(void)
{
  if (ModConAnalogInputChannelSwitch & MODCON_ANALOG_INPUT_CHANNEL_MASK_CH2)
  {    
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH2_SWITCH.value.b.Boolean = bTRUE;
  }
  else
  {
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH2_SWITCH.value.b.Boolean = bFALSE;
  }
}

void UpdateMenuItemAnalogInputCh3Switch(void)
{
  if (ModConAnalogInputChannelSwitch & MODCON_ANALOG_INPUT_CHANNEL_MASK_CH3)
  {    
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH3_SWITCH.value.b.Boolean = bTRUE;
  }
  else
  {
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH3_SWITCH.value.b.Boolean = bFALSE;
  }
}

void UpdateMenuItemAnalogInputCh4Switch(void)
{
  if (ModConAnalogInputChannelSwitch & MODCON_ANALOG_INPUT_CHANNEL_MASK_CH4)
  {    
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH4_SWITCH.value.b.Boolean = bTRUE;
  }
  else
  {
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH4_SWITCH.value.b.Boolean = bFALSE;
  }
}

void UpdateMenuItemAnalogInputCh5Switch(void)
{
  if (ModConAnalogInputChannelSwitch & MODCON_ANALOG_INPUT_CHANNEL_MASK_CH5)
  {    
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH5_SWITCH.value.b.Boolean = bTRUE;
  }
  else
  {
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH5_SWITCH.value.b.Boolean = bFALSE;
  }
}

void UpdateMenuItemAnalogInputCh6Switch(void)
{
  if (ModConAnalogInputChannelSwitch & MODCON_ANALOG_INPUT_CHANNEL_MASK_CH6)
  {    
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH6_SWITCH.value.b.Boolean = bTRUE;
  }
  else
  {
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH6_SWITCH.value.b.Boolean = bFALSE;
  }
}

void UpdateMenuItemAnalogInputCh7Switch(void)
{
  if (ModConAnalogInputChannelSwitch & MODCON_ANALOG_INPUT_CHANNEL_MASK_CH7)
  {    
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH7_SWITCH.value.b.Boolean = bTRUE;
  }
  else
  {
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH7_SWITCH.value.b.Boolean = bFALSE;
  }
}

void UpdateMenuItemAnalogInputCh8Switch(void)
{
  if (ModConAnalogInputChannelSwitch & MODCON_ANALOG_INPUT_CHANNEL_MASK_CH8)
  {    
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH8_SWITCH.value.b.Boolean = bTRUE;
  }
  else
  {
    MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH8_SWITCH.value.b.Boolean = bFALSE;
  }
}

void UpdateMenuItemAnalogOutputCh1Switch(void)
{
  if (ModConAnalogOutputChannelSwitch & MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH1)
  {    
    MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH1_SWITCH.value.b.Boolean = bTRUE;
  }
  else
  {
    MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH1_SWITCH.value.b.Boolean = bFALSE;
  }
}

void UpdateMenuItemAnalogOutputCh2Switch(void)
{
  if (ModConAnalogOutputChannelSwitch & MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH2)
  {    
    MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH2_SWITCH.value.b.Boolean = bTRUE;
  }
  else
  {
    MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH2_SWITCH.value.b.Boolean = bFALSE;
  }
}

void UpdateMenuItemAnalogOutputCh3Switch(void)
{
  if (ModConAnalogOutputChannelSwitch & MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH3)
  {    
    MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH3_SWITCH.value.b.Boolean = bTRUE;
  }
  else
  {
    MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH3_SWITCH.value.b.Boolean = bFALSE;
  }
}

void UpdateMenuItemAnalogOutputCh4Switch(void)
{
  if (ModConAnalogOutputChannelSwitch & MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH4)
  {    
    MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH4_SWITCH.value.b.Boolean = bTRUE;
  }
  else
  {
    MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH4_SWITCH.value.b.Boolean = bFALSE;
  }
}
/* switch update routines */

void ApplyModConSettings(void)
{
    if (!EEPROM_Write16(&ModConProtocolMode, (UINT16)MODCON_HMI_MENU_ITEM_PROTOCOL.mutatedValue.b.Boolean))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
    }
    /* send back current protocol mode to confirm */
    UNUSED(HandleModConProtocolModeGet());  
    
    if (!EEPROM_Write16(&ModConNumber, (UINT16)MODCON_HMI_MENU_ITEM_NUMBER.mutatedValue.l))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
    }

    if (!EEPROM_Write16(&ModConDebug, (UINT16)MODCON_HMI_MENU_ITEM_DEBUG.mutatedValue.b.Boolean))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
    }
    
    /* TODO: add LCD backlight and contrast */
    if (!EEPROM_Write16(&ModConHMIBacklight, (UINT16)MODCON_HMI_MENU_ITEM_LCD_BACKLIGHT.mutatedValue.b.Boolean))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
    }

    if (!EEPROM_Write16(&ModConHMIContrast, (UINT16)MODCON_HMI_MENU_ITEM_LCD_CONTRAST.mutatedValue.l))
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
    }
    
    HMI_SetBacklight(ModConHMIBacklight);
    HMI_SetContrast((UINT8)ModConHMIContrast);
}

void ApplyModConSwitchs(void)
{
  UINT16 analogInputChannelSwitch = 0, analogOutputChannelSwitch = 0;
  
  if (MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH1_SWITCH.mutatedValue.b.Boolean)
  {
    analogInputChannelSwitch = analogInputChannelSwitch | MODCON_ANALOG_INPUT_CHANNEL_MASK_CH1;
  }
  
  if (MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH2_SWITCH.mutatedValue.b.Boolean)
  {
    analogInputChannelSwitch = analogInputChannelSwitch | MODCON_ANALOG_INPUT_CHANNEL_MASK_CH2;
  }
  
  if (MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH3_SWITCH.mutatedValue.b.Boolean)
  {
    analogInputChannelSwitch = analogInputChannelSwitch | MODCON_ANALOG_INPUT_CHANNEL_MASK_CH3;
  }
  
  if (MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH4_SWITCH.mutatedValue.b.Boolean)
  {
    analogInputChannelSwitch = analogInputChannelSwitch | MODCON_ANALOG_INPUT_CHANNEL_MASK_CH4;
  }
  
  if (MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH5_SWITCH.mutatedValue.b.Boolean)
  {
    analogInputChannelSwitch = analogInputChannelSwitch | MODCON_ANALOG_INPUT_CHANNEL_MASK_CH5;
  }
  
  if (MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH6_SWITCH.mutatedValue.b.Boolean)
  {
    analogInputChannelSwitch = analogInputChannelSwitch | MODCON_ANALOG_INPUT_CHANNEL_MASK_CH6;
  }

  if (MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH7_SWITCH.mutatedValue.b.Boolean)
  {
    analogInputChannelSwitch = analogInputChannelSwitch | MODCON_ANALOG_INPUT_CHANNEL_MASK_CH7;
  }

  if (MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH8_SWITCH.mutatedValue.b.Boolean)
  {
    analogInputChannelSwitch = analogInputChannelSwitch | MODCON_ANALOG_INPUT_CHANNEL_MASK_CH8;
  }
  
  if (MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH1_SWITCH.mutatedValue.b.Boolean)
  {
    analogOutputChannelSwitch = analogOutputChannelSwitch | MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH1;
  }

  if (MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH2_SWITCH.mutatedValue.b.Boolean)
  {
    analogOutputChannelSwitch = analogOutputChannelSwitch | MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH2;
  }
  
  if (MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH3_SWITCH.mutatedValue.b.Boolean)
  {
    analogOutputChannelSwitch = analogOutputChannelSwitch | MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH3;
  }
  
  if (MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH4_SWITCH.mutatedValue.b.Boolean)
  {
    analogOutputChannelSwitch = analogOutputChannelSwitch | MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH4;
  }
  
  if (!EEPROM_Write16(&ModConAnalogInputChannelSwitch, analogInputChannelSwitch))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
  }
  
  if (!EEPROM_Write16(&ModConAnalogOutputChannelSwitch, analogOutputChannelSwitch))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_EEPROM_WRITE);          
#endif
  }
}

void EraseModConSettings(void)
{
    if (!EEPROM_Erase())
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_EEPROM_ERASE);
#endif
    }
}
