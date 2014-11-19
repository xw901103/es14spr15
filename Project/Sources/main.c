/**
 * \file main.c
 * \brief Program main entry file.
 * \author Xu Waycell
 * \date 12-August-2014
 */

#include "main.h"
#include "CRG.h"
#include "clock.h"
#include "EEPROM.h"
#include "packet.h"
#include "AWG.h"
#include "OS.h"
#include "utils.h"
//#include <mc9s12a512.h>
#include "derivative.h"

#define OS_VENDOR_PETER_MCLEAN 1;

static UINT8 RoutineStack[THREAD_STACK_SIZE];
static UINT8 RuntimeIndictorRoutineStack[THREAD_STACK_SIZE];

static TAWGChannel AWGChannelLookupTable[4] =
{
  AWG_Ch1,
  AWG_Ch2,
  AWG_Ch3,
  AWG_Ch4
};

void RuntimeIndictorRoutine(void* dataPtr);

/**
 * HMI confirm dialog callback
 */
void (*ConfirmDialogCallback)(void); /* TODO: rename it */

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
void SampleAnalogChannels(void);

//void AWGPostProcessRoutine(TAWGChannel channelNb);

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
  static UINT8 seconds = 0;

  if (seconds != Clock_Seconds)
  {    
    if (!Packet_Put(MODCON_COMMAND_TIME, MODCON_TIME_INITIAL, Clock_Seconds, Clock_Minutes)) 
    {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
      return bFALSE;
    }
    seconds = Clock_Seconds;
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

BOOL HandleModConAnalogValue(const TAnalogChannel channelNb)
{
  switch(channelNb)
  {
    case ANALOG_INPUT_Ch1:
    case ANALOG_INPUT_Ch2:
    case ANALOG_INPUT_Ch3:
    case ANALOG_INPUT_Ch4:
    case ANALOG_INPUT_Ch5:
    case ANALOG_INPUT_Ch6:
    case ANALOG_INPUT_Ch7:
    case ANALOG_INPUT_Ch8:
      return HandleModConAnalogInputValue(channelNb);
      break;
    case ANALOG_OUTPUT_Ch1:
    case ANALOG_OUTPUT_Ch2:
    case ANALOG_OUTPUT_Ch3:
    case ANALOG_OUTPUT_Ch4:
      return HandleModConAnalogOutputValue(channelNb);
      break;
    default:
      break;
  }
  return bFALSE;
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

BOOL HandleModConAnalogOutputValue(const TAnalogChannel channelNb)
{
  UINT8 index = 0xFF;
  TINT16 value;
  
  /* NOTE: channel enums might not be in numberic order */
  switch(channelNb)
  {
    case ANALOG_OUTPUT_Ch1:
      index = 0;
      break;
    case ANALOG_OUTPUT_Ch2:
      index = 1;
      break;
    case ANALOG_OUTPUT_Ch3:
      index = 2;
      break;
    case ANALOG_OUTPUT_Ch4:
      index = 3;
      break;
    default:
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_INVALID_ARGUMENT);
#endif
      return bFALSE;
      break;
  }
  
  value.l = 0x07FF - Analog_Output[index].Value.l;
  
  if (!Packet_Put(MODCON_COMMAND_ANALOG_OUTPUT_VALUE, index, value.s.Lo, value.s.Hi))
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

BOOL HandleModConWaveGetStatus(void);
BOOL HandleModConWaveSetWaveform(void);
BOOL HandleModConWaveSetFrequency(void);
BOOL HandleModConWaveSetAmplitude(void);
BOOL HandleModConWaveSetOffset(void);
BOOL HandleModConWaveEnable(BOOL enable);

/**
 * \fn BOOL HandleModConWaveActiveChannel(void)
 * \brief Activates selected AWG channel to response to incoming settings
 */
BOOL HandleModConWaveActiveChannel(void);

/**
 * \fn BOOL HandleModConWaveArbitraryPhasor(void)
 * \brief Apply received phasor setting to AWG 
 */
BOOL HandleModConWaveArbitraryPhasor(void);

/**
 * \fn BOOL HandleModConWave(void)
 * \brief Hub of ModCon wave command 
 */
BOOL HandleModConWave(void)
{
  switch(Packet_Parameter1)
  {
    case MODCON_WAVE_STATUS:
      if (Packet_Parameter23 == 0)
      {
        return HandleModConWaveGetStatus();
      }
      break;
    case MODCON_WAVE_WAVEFORM:
      if (Packet_Parameter2 < 6 && Packet_Parameter3 == 0)
      {
        return HandleModConWaveSetWaveform();
      }
      break;
    case MODCON_WAVE_FREQUENCY:
      return HandleModConWaveSetFrequency();
      break;
    case MODCON_WAVE_AMPLITUDE:
      return HandleModConWaveSetAmplitude();
      break;
    case MODCON_WAVE_OFFSET:
      return HandleModConWaveSetOffset();
      break;
    case MODCON_WAVE_ON:
      if (Packet_Parameter23 == 0)
      {
        return HandleModConWaveEnable(bTRUE);
      }
      break;
    case MODCON_WAVE_OFF:
      if (Packet_Parameter23 == 0)
      {
        return HandleModConWaveEnable(bFALSE);
      }
      break;
    case MODCON_WAVE_ACTIVE_CHANNEL:
      if (Packet_Parameter3 == 0)
      {
        return HandleModConWaveActiveChannel();
      }
      break;    
    default:
      break;
  }
}

/**
 * \fn BOOL HandleModConWaveSetWaveform(void)
 * \brief Sends status of active AWG channel 
 */
BOOL HandleModConWaveGetStatus(void)
{

  UINT8 index, channelNb, enable, waveform;
  TUINT16 frequency, amplitude, offset;
  BOOL success = bFALSE;
  
  for (index = 0; index < NB_AWG_CHANNELS; ++index)
  {
    if (AWG_Channel[index].isActive)
    {      
      channelNb = index;
      enable = (UINT8)AWG_Channel[index].isEnabled;

      switch(AWG_Channel[index].waveformType)
      {
        case AWG_WAVEFORM_SINE:
          waveform = 0;
          break;
        case AWG_WAVEFORM_SQUARE:
          waveform = 1;
          break;
        case AWG_WAVEFORM_TRIANGLE:
          waveform = 2;
          break;
        case AWG_WAVEFORM_SAWTOOTH:
          waveform = 3;
          break;
        case AWG_WAVEFORM_NOISE:
          waveform = 4;
          break;
        case AWG_WAVEFORM_ARBITRARY:
          waveform = 5;
          break;
        default:
          waveform = 0xFF;
          break;
      }

      //frequency.l = (AWG_Channel[index].frequency * 256) / 10;
      frequency.l = AWG_Channel[index].frequency;
      amplitude.l = AWG_Channel[index].amplitude;
      offset.l = AWG_Channel[index].offset;
    
      success = Packet_Put(MODCON_COMMAND_WAVE, MODCON_WAVE_STATUS, channelNb, enable) &&
                Packet_Put(MODCON_COMMAND_WAVE, MODCON_WAVE_WAVEFORM, waveform, 0) &&
                Packet_Put(MODCON_COMMAND_WAVE, MODCON_WAVE_FREQUENCY, frequency.s.Lo, frequency.s.Hi) &&
                Packet_Put(MODCON_COMMAND_WAVE, MODCON_WAVE_AMPLITUDE, amplitude.s.Lo, amplitude.s.Hi) &&
                Packet_Put(MODCON_COMMAND_WAVE, MODCON_WAVE_OFFSET, offset.s.Lo, offset.s.Hi);
    }
  }
  return success;              
}

/**
 * \fn BOOL HandleModConWaveSetWaveform(void)
 * \brief Sets waveform to active AWG channel 
 */
BOOL HandleModConWaveSetWaveform(void)
{
  static UINT8 waveformTypeLookupTable[6] =
  {
    AWG_WAVEFORM_SINE,
    AWG_WAVEFORM_SQUARE,
    AWG_WAVEFORM_TRIANGLE,
    AWG_WAVEFORM_SAWTOOTH,
    AWG_WAVEFORM_NOISE,
    AWG_WAVEFORM_ARBITRARY
  };
    
  UINT8 index = 0;
  
  for (index = 0; index < NB_AWG_CHANNELS; ++index)
  {
    if (AWG_Channel[index].isActive)
    {
      AWG_Channel[index].waveformType = waveformTypeLookupTable[Packet_Parameter2];
      AWG_Update(AWGChannelLookupTable[index]);
    }
  }
  
  return bTRUE;  
}

/**
 * \fn BOOL HandleModConWaveSetFrequency(void)
 * \brief Sets frequency value to active AWG channel 
 */
BOOL HandleModConWaveSetFrequency(void)
{
  UINT8 index = 0;  
  
  for (index = 0; index < NB_AWG_CHANNELS; ++index)
  {
    if (AWG_Channel[index].isActive)
    {
      AWG_Channel[index].frequency = Packet_Parameter23;
      AWG_Update(AWGChannelLookupTable[index]);
    }
  }

  return bTRUE;  
}

/**
 * \fn BOOL HandleModConWaveSetOffset(void)
 * \brief Sets offset value to active AWG channel 
 */
BOOL HandleModConWaveSetAmplitude(void)
{
  UINT8 index = 0;

  for (index = 0; index < NB_AWG_CHANNELS; ++index)
  {
    if (AWG_Channel[index].isActive)
    {
      AWG_Channel[index].amplitude = Packet_Parameter23;
      AWG_Update(AWGChannelLookupTable[index]);
    }
  }

  return bTRUE;
}

/**
 * \fn BOOL HandleModConWaveSetOffset(void)
 * \brief Sets offset value to active AWG channel 
 */
BOOL HandleModConWaveSetOffset(void)
{
  UINT8 index = 0;

  for (index = 0; index < NB_AWG_CHANNELS; ++index)
  {
    if (AWG_Channel[index].isActive)
    {
      AWG_Channel[index].offset = Packet_Parameter23;
      AWG_Update(AWGChannelLookupTable[index]);
    }
  }

  return bTRUE;
}

/**
 * \fn BOOL HandleModConWaveEnable(BOOL enable)
 * \brief Enables/Disables selected AWG channel
 */
BOOL HandleModConWaveEnable(BOOL enable)
{
  UINT8 index = 0;

  for (index = 0; index < NB_AWG_CHANNELS; ++index)
  {
    if (AWG_Channel[index].isActive)
    {
      AWG_Channel[index].isEnabled = enable;
      AWG_Enable(AWGChannelLookupTable[index], enable);
    }
  }

  return bTRUE;
}

/**
 * \fn BOOL HandleModConWaveActiveChannel(void)
 * \brief Activates selected AWG channel to response to incoming settings
 */
BOOL HandleModConWaveActiveChannel(void)
{
  if (Packet_Parameter2 == 0)
  {
    AWG_Channel[0].isActive = bTRUE;
    AWG_Channel[1].isActive = bFALSE;
    //AWG_Channel[2].isActive = bFALSE;
    //AWG_Channel[3].isActive = bFALSE;    
  
    return bTRUE;
  }
  else if (Packet_Parameter2 == 1)
  {
    AWG_Channel[0].isActive = bFALSE;
    AWG_Channel[1].isActive = bTRUE;
    //AWG_Channel[2].isActive = bFALSE;
    //AWG_Channel[3].isActive = bFALSE;    

    return bTRUE;
  }
  
  return bFALSE;
}

/**
 * \fn BOOL HandleModConArbitraryWave(void)
 * \brief Handles arbitrary wave packet
 */
BOOL HandleModConArbitraryWave(void)
{
  UINT8 index = 0;

  if (Packet_Parameter1 < AWG_ARBITRARY_WAVE_SIZE)
  {

    for (index = 0; index < NB_AWG_CHANNELS; ++index)
    {
      if (AWG_Channel[index].isActive)
      {
        CRG_ResetCOP(); /* it gives more time for caculations */
        AWG_Channel[index].arbitraryWave[Packet_Parameter1] = (2047 - (INT32)Packet_Parameter23) * 10; /* match our other AWG waveform sample scale */
      }
    }
    //AWG_ARBITRARY_WAVE[Packet_Parameter1] = (2047 - (INT32)Packet_Parameter23) * 10; /* match our other AWG waveform sample scale */
    return bTRUE;
  }
  return bFALSE;
}

/**
 * \fn BOOL HandleModConArbitraryPhasor(void)
 * \brief Handles arbitrary phasor packet
 */
BOOL HandleModConArbitraryPhasor(void)
{
  UINT16 index = 0;
  UINT8 harmonicNb = 0xFF;
  UINT16 magnitude = 0xFFFF;
  INT16 angle = 0xFFFF;
  
  harmonicNb = Packet_Parameter1 >> 4;
  angle = ((Packet_Parameter1 & 0x0F) << 6) | ((Packet_Parameter2 & 0xFC) >> 2);
  magnitude = ((Packet_Parameter2 & 0x03) << 8) | Packet_Parameter3;
  
  switch(harmonicNb)
  {
    case MODCON_ARBITRARY_PHASOR_RESET:
      AWG_ResetArbitraryWave();
      break;
    case MODCON_ARBITRARY_PHASOR_HARMONIC_1:
    case MODCON_ARBITRARY_PHASOR_HARMONIC_2:
    case MODCON_ARBITRARY_PHASOR_HARMONIC_3:
    case MODCON_ARBITRARY_PHASOR_HARMONIC_4:
    case MODCON_ARBITRARY_PHASOR_HARMONIC_5:
    case MODCON_ARBITRARY_PHASOR_HARMONIC_6:
    case MODCON_ARBITRARY_PHASOR_HARMONIC_7:
    case MODCON_ARBITRARY_PHASOR_HARMONIC_8:
    case MODCON_ARBITRARY_PHASOR_HARMONIC_9:
    case MODCON_ARBITRARY_PHASOR_HARMONIC_A:
    case MODCON_ARBITRARY_PHASOR_HARMONIC_B:
    case MODCON_ARBITRARY_PHASOR_HARMONIC_C:
    case MODCON_ARBITRARY_PHASOR_HARMONIC_D:
    case MODCON_ARBITRARY_PHASOR_HARMONIC_E:
    case MODCON_ARBITRARY_PHASOR_HARMONIC_F:
      AWG_ApplyArbitraryPhasor(harmonicNb, magnitude, angle);
      break;
    default:
      return bFALSE;
      break;
  }
  return bTRUE;
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
void SampleAnalogChannels(void) 
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
  outputChannelSwitchMaskLookupTable[NB_OUTPUT_CHANNELS] = { MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH1,
                                                             MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH2,
                                                             MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH3,
                                                             MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH4 };
  static const TAnalogChannel
  /* NOTE: channel enums might not be in numeric order */
  inputChannelNumberLookupTable[NB_INPUT_CHANNELS] = { ANALOG_INPUT_Ch1,
                                                       ANALOG_INPUT_Ch2,
                                                       ANALOG_INPUT_Ch3,
                                                       ANALOG_INPUT_Ch4,
                                                       ANALOG_INPUT_Ch5,
                                                       ANALOG_INPUT_Ch6,
                                                       ANALOG_INPUT_Ch7,
                                                       ANALOG_INPUT_Ch8 },
  outputChannelNumberLookupTable[NB_OUTPUT_CHANNELS] = { ANALOG_OUTPUT_Ch1,
                                                         ANALOG_OUTPUT_Ch2,
                                                         ANALOG_OUTPUT_Ch3,
                                                         ANALOG_OUTPUT_Ch4 };
  
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
    
  for (index = 0; index < NB_OUTPUT_CHANNELS; ++index)
  {
    if (ModConAnalogOutputChannelSwitch & outputChannelSwitchMaskLookupTable[index])
    {
      UNUSED(HandleModConAnalogOutputValue(outputChannelNumberLookupTable[index]));
    }
  }
  
}

/**
 * \fn void AWGPostProcessRoutine(TAWGChannel channelNb)
 * \brief Sends out AWG channel value after analog process that is finished
 * \param channelNb AWG channel number
 */
void AWGPostProcessRoutine(TAWGChannel channelNb)
{
  UINT8 index = 0xFF;                      
  TAnalogChannel analogChannelNb;
  switch(channelNb)
  {
    case AWG_Ch1:
      analogChannelNb = ANALOG_OUTPUT_Ch1;
      index = 0;
      break;
    case AWG_Ch2:
      analogChannelNb = ANALOG_OUTPUT_Ch2;
      index = 1;
      break;
    case AWG_Ch3:
      analogChannelNb = ANALOG_OUTPUT_Ch3;
      index = 2;
      break;
    case AWG_Ch4:
      analogChannelNb = ANALOG_OUTPUT_Ch4;
      index = 3;
      break;
    default:
      return;
      break;
  }
  //if (Analog_Output[index].Value.l != Analog_Output[index].OldValue.l)
  //{    
    UNUSED(HandleModConAnalogOutputValue(analogChannelNb));
  //}
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

  if (ModConAnalogInputSamplingRate == 0xFFFF)
  {
    if (!EEPROM_Write16(&ModConAnalogInputSamplingRate, DEFAULT_MODCON_ANALOG_INPUT_SAMPLING_RATE))
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
  
//  Clock_Setup(CONFIG_RTI_PRESCALERATE, CONFIG_RTI_MODULUSCOUNT);
    
  Timer_Setup();
  
  Analog_Setup(CONFIG_BUSCLK);
  
  AWG_Setup(CONFIG_BUSCLK);
  //AWG_AttachPostProcessRoutine(&AWGPostProcessRoutine);
  
  //Timer_SetupPeriodicTimer(ModConAnalogInputSamplingRate, CONFIG_BUSCLK);
  //Timer_AttachPeriodicTimerRoutine(&SampleAnalogChannels);
  /* enable ModCon analog input sampling */
  //Timer_PeriodicTimerEnable(bTRUE);

  OS_Init();
  
#if !defined(NO_INTERRUPT) && !defined(OS_VENDOR_PETER_MCLEAN) 
  EnableInterrupts;
#endif
  
  return bTRUE;
}

/**
 * \fn void RuntimeIndictorRoutine(void* dataPtr)
 * \brief Toggles runtime indicator about every half a second. In addition, it sends out uptime packet.
 * \param dataPtr not used
 */
void RuntimeIndictorRoutine(void* dataPtr)
{
  static BOOL toggle = bFALSE;
  
  UNUSED(dataPtr);
  
  for(;;)
  {
    PORTE_BIT7 = (byte)toggle;        
    toggle = !toggle;
    UNUSED(HandleModConUptime());
    
    OS_TimeDelay(7); /* about half a second */
  }
}

/**
 * \fn void Routine(void*)
 * \brief Retrieves ModCon packets and sends back packets if it is necessary.
 */
void Routine(void* dataPtr)
{
  UINT8 ack = 0;
  BOOL bad = bTRUE;
  
  UNUSED(dataPtr);
    
  for (;;)
  {
    CRG_ArmCOP();
        
//    if (Clock_Update())
//    {
//      /* hours from 0 to 23; minutes from 0 to 59; seconds from 0 to 59 */
//      bad = !HandleModConUptime();
//    }

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
        case MODCON_COMMAND_WAVE:
          bad = !HandleModConWave();
          break;
        case MODCON_COMMAND_ARBITRARY_WAVE:
          bad = !HandleModConArbitraryWave();
          break;
        case MODCON_COMMAND_ARBITRARY_PHASOR:
          bad = !HandleModConArbitraryPhasor();
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
    CRG_DisarmCOP();
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
  
  UNUSED(OS_ThreadCreate(&RuntimeIndictorRoutine, 0x0000, &RuntimeIndictorRoutineStack[THREAD_STACK_SIZE - 1], 0));
  UNUSED(OS_ThreadCreate(&Routine, 0x0000, &RoutineStack[THREAD_STACK_SIZE - 1], 1));

  OS_Start();
}
