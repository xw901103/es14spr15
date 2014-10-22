/**
 * \mainpage Xu's Embedded Software Lab 4
 *
 * \section intro_sec Introduction
 * Lab 4 is based on Lab 3 to interface a ADC chip via SPI bus to enable
 * analog input sampling every 10 ms period.
 * <br>
 * - Uptime will be updated every second
 * - A 10 millisecond period periodic analog input sampling rate will be using.
 * - SPI bus will be set up on 1Mhz baudrate.
 * - A simple three records median filter will be using for signal process.
 * - Protocol mode will be using for synchronous/asynchronous mode.
 * <br>
 * * 0x04 ModCon Startup
 * <br>This will send program settings including ModCon number and version.
 * * 0x07 ModCon Program EEPROM byte
 * <br>This will program a byte in EEPROM through given data and address. 
 * * 0x08 ModCon Get EEPROM byte
 * <br>This will send current data stored in given EEPROM address.
 * * 0x09 ModCon Special ModCon query
 * <br>This will special modcon subcommand.
 * * 0x0A ModCon protocol mode get and set
 * <br>This is the accessor and mutator of ModCon protocol mode.
 * * 0x0B ModCon number get and set
 * <br>This is the accessor and mutator of ModCon number.
 * * 0x0C ModCon uptime
 * <br>This will send current system uptime in minutes and seconds.
 * * 0x0D ModCon mode get and set
 * <br>This is the accessor and mutator of ModCon mode.
 * * 0x50 ModCon analog input value
 * <br>This will send analog input channel number and its current value.
 *
 * \file main.h
 * \brief Program main entry file. 
 * \author Xu Waycell
 * \date 11-August-2014
 */
#ifndef MAIN_H
#define MAIN_H

#include "global.h"
#include "analog.h"
#include "HMI.h"
#pragma LINK_INFO DERIVATIVE "mc9s12a512" /* link mc9s12a512's library */

const UINT8 MODCON_COMMAND_STARTUP             = 0x04; /* ModCon protocol startup command */
const UINT8 MODCON_COMMNAD_EEPROM_PROGRAM      = 0x07; /* ModCon protocol EEPROM program command */
const UINT8 MODCON_COMMAND_EEPROM_GET          = 0x08; /* ModCon protocol EEPROM get command */
const UINT8 MODCON_COMMAND_SPECIAL             = 0x09; /* ModCon protocol special command */
const UINT8 MODCON_COMMAND_PROTOCOL_MODE       = 0x0A; /* ModCon protocol protocol mode command */
const UINT8 MODCON_COMMAND_NUMBER              = 0x0B; /* ModCon protocol number command */
const UINT8 MODCON_COMMAND_TIME                = 0x0C; /* ModCon protocol time command */
const UINT8 MODCON_COMMAND_MODE                = 0x0D; /* ModCon protocol mode command */
const UINT8 MODCON_COMMAND_ANALOG_INPUT_VALUE  = 0x50; /* ModCon protocol analog input command */
const UINT8 MODCON_COMMAND_ANALOG_OUTPUT_VALUE = 0x51; /* ModCon protocol analog output command */

const UINT8 MODCON_DEBUG_INITIAL = 'd';
const UINT8 MODCON_DEBUG_TOKEN   = 'j';

const UINT8 MODCON_VERSION_INITIAL = 'v';
const UINT8 MODCON_VERSION_TOKEN   = 'x'; /* TODO: rename it for better functionality reflection */
const UINT8 MODCON_VERSION_MAJOR   =  1 ;
const UINT8 MODCON_VERSION_MINOR   =  0 ;

const UINT8 MODCON_PROTOCOL_MODE_GET = 1;
const UINT8 MODCON_PROTOCOL_MODE_SET = 2;
const UINT8 MODCON_PROTOCOL_MODE_ASYNCHRONOUS = 0;
const UINT8 MODCON_PROTOCOL_MODE_SYNCHRONOUS  = 1;

const UINT8 MODCON_NUMBER_GET = 1;
const UINT8 MODCON_NUMBER_SET = 2;

const UINT8 MODCON_TIME_INITIAL = 'i';

const UINT8 MODCON_MODE_GET = 1;
const UINT8 MODCON_MODE_SET = 2;

<<<<<<< Updated upstream
const THMIContext MODCON_HMI_CONTEXT = {0};
=======
<<<<<<< HEAD
void IdlePanelInputHandler(THMIKey key)
{
  if (key == HMI_KEY_SET)
  {
    HMI_ShowPanel(1);
  }
}

const THMIPanel MODCON_HMI_IDLE_PANEL = 
{
  0,
  {
    'I','D','L','E',' ',' ',' '
  },
  &IdlePanelInputHandler,
  0
};

void SettingPanelInputHandler(THMIKey key)
{
  if (key == HMI_KEY_SET)
  {
    HMI_ClosePanel();
  }
}

const THMIPanel MODCON_HMI_SETTING_PANEL = 
{
  1,
  {
    'S','E','T','T','I','N','G'
  },
  &SettingPanelInputHandler,
  0
};

const THMISetup MODCON_HMI_SETUP =
{
  HMI_RENDER_MODE_CONTINUITY,
  {
    {
      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
      {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
      {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
      {'S', 'E', 'T', ' ', 'D', 'A', 'T', 'A', ' ', '<', ' ', '>', ' ', 'S', 'E', 'L'}    
    },
    16,
    8
  }
};
=======
const THMIContext MODCON_HMI_CONTEXT = {0};
>>>>>>> FETCH_HEAD
>>>>>>> Stashed changes

#if !defined(MODCON_COMMAND_ACK_MASK)
#define MODCON_COMMAND_ACK_MASK 0x80 /* ModCon protocol acknowledgement bitwise mask */
#endif

/**
 * ModCon acceptable EEPROM address begin boundary
 */
#ifndef CONFIG_MODCON_EEPROM_ADDRESS_BEGIN
#define MODCON_EEPROM_ADDRESS_BEGIN 0x0400 /* fallback plan */
#warning "MODCON_EEPROM_ADDRESS_BEGIN using fallback setting 0x0400"
#else
#define MODCON_EEPROM_ADDRESS_BEGIN CONFIG_MODCON_EEPROM_ADDRESS_BEGIN
#endif

/**
 * ModCon acceptable EEPROM address end boundary
 */
#ifndef CONFIG_MODCON_EEPROM_ADDRESS_END
#define MODCON_EEPROM_ADDRESS_END 0x1000 /* fallback plan */
#warning "MODCON_EEPROM_ADDRESS_END using fallback setting 0x1000"
#else
#define MODCON_EEPROM_ADDRESS_END CONFIG_MODCON_EEPROM_ADDRESS_END
#endif

/**
 * ModCon protocol mode
 */
#define DEFAULT_MODCON_PROTOCOL_MODE MODCON_PROTOCOL_MODE_ASYNCHRONOUS
#define ModConProtocolMode EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_PROTOCOL_MODE)

/**
 * ModCon analog input channel switch
 */
#define DEFAULT_MODCON_ANALOG_INPUT_CHANNEL_SWITCH 0b00000011 /* | Ch8 | Ch7 | Ch6 | Ch5 | Ch4 | Ch3 | Ch2 | Ch1 | */
#define ModConAnalogInputChannelSwitch EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_ANALOG_INPUT_CHANNEL_SWITCH)
#define MODCON_ANALOG_INPUT_CHANNEL_MASK_CH1 0b00000001 /* mask byte | Ch8 | Ch7 | Ch6 | Ch5 | Ch4 | Ch3 | Ch2 | Ch1 | */
#define MODCON_ANALOG_INPUT_CHANNEL_MASK_CH2 0b00000010
#define MODCON_ANALOG_INPUT_CHANNEL_MASK_CH3 0b00000100
#define MODCON_ANALOG_INPUT_CHANNEL_MASK_CH4 0b00001000
#define MODCON_ANALOG_INPUT_CHANNEL_MASK_CH5 0b00010000
#define MODCON_ANALOG_INPUT_CHANNEL_MASK_CH6 0b00100000
#define MODCON_ANALOG_INPUT_CHANNEL_MASK_CH7 0b01000000
#define MODCON_ANALOG_INPUT_CHANNEL_MASK_CH8 0b10000000

/**
 * ModCon analog output channel switch
 */
#define DEFAULT_MODCON_ANALOG_OUTPUT_CHANNEL_SWITCH 0b0000 /* | Ch4 | Ch3 | Ch2 | Ch1 | */
#define ModConAnalogOutputChannelSwitch EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_ANALOG_OUTPUT_CHANNEL_SWITCH)
#define MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH1 0b00000001 /* mask byte | X | X | X | X | Ch4 | Ch3 | Ch2 | Ch1 | */
#define MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH2 0b00000010
#define MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH3 0b00000100
#define MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH4 0b00001000

/**
 * ModCon analog sampling rate
 */
#define DEFAULT_MODCON_ANALOG_SAMPLING_RATE 10000 /* sampling frequency in microseconds */
#define ModConAnalogSamplingRate EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_ANALOG_SAMPLING_RATE)

/**
 * ModCon number
 */
#define DEFAULT_MODCON_NUMBER 29
#define ModConNumber EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_NUMBER)

/**
 * ModCon mode
 */
#define DEFAULT_MODCON_MODE 1
#define ModConMode EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_MODE)

/**
 * ModCon debug
 */
#define DEFAULT_MODCON_DEBUG 0
#define ModConDebug EEPROM_WORD(CONFIG_EEPROM_ADDRESS_MODCON_DEBUG)

/**
 * \fn BOOL HandleModConStartup(void)
 * \brief Builds packets that are necessary for startup information and places them into transmit buffer. 
 * \return TRUE if packets were queued for transmission successfully.
 */  
BOOL HandleModConStartup(void);

/**
 * \fn BOOL HandleModConSpecial(void)
 * \brief response to ModCon special commands. 
 * \return TRUE if the command has been executed successfully.
 */
BOOL HandleModConSpecial(void);

/**
 * \fn BOOL HandleModConProtocolMode(void)
 * \brief response to ModCon protocol mode commands. 
 * \return TRUE if the command has been executed successfully.
 */
BOOL HandleModConProtocolMode(void);

/**
 * \fn BOOL HandleModConNumber(void)
 * \brief response to ModCon number commands. 
 * \return TRUE if the command has been executed successfully.
 */
BOOL HandleModConNumber(void);

/**
 * \fn BOOL HandleModConUptime(void)
 * \brief Sends out system uptime in minutes and seconds 
 * \return TRUE if the packet has queued successfully.
 */
BOOL HandleModConUptime(void);

/**
 * \fn BOOL HandleModConMode(void)
 * \brief response to ModCon mode commands. 
 * \return TRUE if the command has been executed successfully.
 */
BOOL HandleModConMode(void);

/**
 * \fn BOOL HandleModConAnalogInputValue(const TAnalogChannel channelNb)
 * \brief Builds a packet that contains current ModCon analog input value and places it into transmit buffer. 
 * \param channelNb analog input channel number
 * \return TRUE if the command has been executed successfully.
 */
BOOL HandleModConAnalogInputValue(const TAnalogChannel channelNb);

/**
 * \fn BOOL HandleModConEEPROMProgram(void)
 * \brief Program a byte in EEPROM by given address.
 * \return TRUE if EEPROM program successfully. 
 */
BOOL HandleModConEEPROMProgram(void);

/**
 * \fn BOOL HandleModConEEPROMGet(void)
 * \brief Return byte value of given EEPROM address.
 * \return TRUE if address is validated and the packet was queued for transmission successfully.
 */
BOOL HandleModConEEPROMGet(void);

/**
 * \fn void Initialize(void)
 * \brief Initializes hardware and software parameters that required for this program.
 * \return TRUE if initialization routines executed successfully.
 */
BOOL Initialize(void);

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

const char blankFrame[8][16] = 
{
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
};

const char testFrame[8][16] = 
{
  {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'},
  {'1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {'2', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {'3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {'4', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {'5', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {'6', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {'7', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
};

char systemFrame[8][16] = 
{
//  {' ', 'U', 'P', 'T', 'I', 'M', 'E', ' ', '0', '0', ':', '0', '0', ':', '0', '0'},
  {'P', 'A', 'N', 'E', 'L', 'T', 'S', ' ', '0', '0', ':', '0', '0', ':', '0', '0'},
  {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
  {' ', ' ', 'N', 'U', 'M', 'B', 'E', 'R', ':', '0', '0', '0', '0', '0', ' ', ' '},
  {' ', 'V', 'E', 'R', 'S', 'I', 'O', 'N', ':', '0', '.', '0', ' ', ' ', ' ', ' '},
  {'P', 'R', 'O', 'T', 'O', 'C', 'O', 'L', ':', 'U', 'N', 'K', 'N', 'O', 'W', ' '},
  {' ', ' ', ' ', 'D', 'E', 'B', 'U', 'G', ':', 'U', 'N', 'K', 'N', 'O', 'W', ' '},
  {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
  {'S', 'E', 'T', ' ', 'D', 'A', 'T', 'A', ' ', '<', ' ', '>', ' ', 'S', 'E', 'L'}
};

#endif
