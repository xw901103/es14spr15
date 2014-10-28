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
 * ModCon HMI backlight
 */
#define DEFAULT_MODCON_HMI_BACKLIGHT 0
#define ModConHMIBacklight EEPROM_WORD(CONFIG_EEPROM_ADDRESS_HMI_BACKLIGHT)

/**
 * ModCon HMI contrast
 */
#define DEFAULT_MODCON_HMI_CONTRAST 15
#define ModConHMIContrast EEPROM_WORD(CONFIG_EEPROM_ADDRESS_HMI_CONTRAST)

/**
 * \fn void ApplyModConSettings(void)
 * \brief Writes all related ModCon setting menu values to EEPROM.
 */
void ApplyModConSettings(void);

/**
 * \fn void ApplyModConSwitchs(void)
 * \brief Writes all related ModCon IO switch menu values to EEPROM.
 */
void ApplyModConSwitchs(void);

/**
 * \fn void EraseModConSettings(THMIMenuItem* itemPtr)
 * \brief Erases EEPROM and force MCU reset.
 * \param itemPtr A pointer of THMIMenuItem
 */
void EraseModConSettings(THMIMenuItem* itemPtr);

/**
 * \fn BOOL IdlePanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key)
 * \brief Handles idle panel inputs.
 * \param panelPtr A pointer of THMIPanel
 * \param key Key that has been pressed
 * \return TRUE if input has been processed or FALSE if it is not acceptable.
 */
BOOL IdlePanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key);

/**
 * \fn BOOL SettingPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key)
 * \brief Handles setting panel inputs.
 * \param panelPtr A pointer of THMIPanel
 * \param key Key that has been pressed
 * \return TRUE if input has been processed or FALSE if it is not acceptable.
 */
BOOL SettingPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key);

/**
 * \fn void SettingPanelUpdateRoutine(THMIPanel* panelPtr)
 * \brief Updates related setting panel parameters once it has been exposed.
 * \param panelPtr A pointer of THMIPanel
 */
void SettingPanelUpdateRoutine(THMIPanel* panelPtr);

/**
 * \fn BOOL AnalogPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key)
 * \brief Handles analog panel inputs.
 * \param panelPtr A pointer of THMIPanel
 * \param key Key that has been pressed
 * \return TRUE if input has been processed or FALSE if it is not acceptable.
 */
BOOL AnalogPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key);

/**
 * \fn void AnalogPanelUpdateRoutine(THMIPanel* panelPtr)
 * \brief Updates related analog panel parameters once it has been exposed.
 * \param panelPtr A pointer of THMIPanel
 */
void AnalogPanelUpdateRoutine(THMIPanel* panelPtr);

/**
 * \fn BOOL SwitchPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key)
 * \brief Handles switch panel inputs.
 * \param panelPtr A pointer of THMIPanel
 * \param key Key that has been pressed
 * \return TRUE if input has been processed or FALSE if it is not acceptable.
 */
BOOL SwitchPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key);

/**
 * \fn void SwitchPanelUpdateRoutine(THMIPanel* panelPtr)
 * \brief Updates related switch panel parameters once it has been exposed.
 * \param panelPtr A pointer of THMIPanel
 */
void SwitchPanelUpdateRoutine(THMIPanel* panelPtr);

/**
 * \fn BOOL ConfirmPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key)
 * \brief Handles confirm panel inputs.
 * \param panelPtr A pointer of THMIPanel
 * \param key Key that has been pressed
 * \return TRUE if input has been processed or FALSE if it is not acceptable.
 */
BOOL ConfirmPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key);

/**
 * \fn void UpdateMenuItemVersion(THMIMenuItem* itemPtr)
 * \brief Updates version menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
void UpdateMenuItemVersion(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemNumber(THMIMenuItem* itemPtr)
 * \brief Updates number menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
void UpdateMenuItemNumber(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemDebug(THMIMenuItem* itemPtr)
 * \brief Updates debug menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
void UpdateMenuItemDebug(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemProtocol(THMIMenuItem* itemPtr)
 * \brief Updates protocol menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
void UpdateMenuItemProtocol(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemBacklight(THMIMenuItem* itemPtr)
 * \brief Updates backlight menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
void UpdateMenuItemBacklight(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemContrast(THMIMenuItem* itemPtr)
 * \brief Updates contrast menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
void UpdateMenuItemContrast(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemAnalogInputChannelValue(THMIMenuItem* itemPtr)
 * \brief Updates given input channel menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
void UpdateMenuItemAnalogInputChannelValue(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemAnalogOutputChannelValue(THMIMenuItem* itemPtr)
 * \brief Updates given output channel menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
void UpdateMenuItemAnalogOutputChannelValue(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemAnalogInputChannelSwitch(THMIMenuItem* itemPtr)
 * \brief Updates given input channel switch menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
void UpdateMenuItemAnalogInputChannelSwitch(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemAnalogOutputChannelSwitch(THMIMenuItem* itemPtr)
 * \brief Updates given output channel switch menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
void UpdateMenuItemAnalogOutputChannelSwitch(THMIMenuItem* itemPtr);

THMIMenuItem MODCON_HMI_MENU_ITEM_VERSION =
{
  {'V', 'E', 'R', 'S', 'I', 'O', 'N'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0, /* user defined attribute */
  0,
  0x0000, /* minimum */
  0xFFFF, /* maximum */
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_VERSION_NUMBER,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemVersion,
  (THMIMenuItemActionRoutine)0x00
};

THMIMenuItem MODCON_HMI_MENU_ITEM_NUMBER =
{
  {'N', 'U', 'M', 'B', 'E', 'R'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0, /* user defined attribute */
  0,
  0x0000, /* minimum */
  0xFFFF, /* maximum */
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_UNSIGNED_INTEGER,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemNumber,
  (THMIMenuItemActionRoutine)0x00
};

THMIMenuItem MODCON_HMI_MENU_ITEM_DEBUG =
{
  {'D', 'E', 'B', 'U', 'G'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0, /* user defined attribute */
  0,
  0x0000, /* minimum */
  0x0001, /* maximum */
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemDebug,
  (THMIMenuItemActionRoutine)0x00
};

THMIMenuItem MODCON_HMI_MENU_ITEM_PROTOCOL =
{
  {'P', 'R', 'O', 'T', 'O', 'C', 'O', 'L'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,      /* user defined attribute */
  0,      /* menu item value */
  0x0000, /* minimum */
  0x0001, /* maximum */
  0,      /* mutated menu item value */
  bFALSE, /* use mutated value */
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_SYNC_ASYNC,
  &UpdateMenuItemProtocol,
  (THMIMenuItemActionRoutine)0x00
};

THMIMenuItem MODCON_HMI_MENU_ITEM_LCD_BACKLIGHT =
{
  {'B', 'K', 'L', 'I', 'G', 'H', 'T'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0, /* user defined attribute */
  0,
  0x0000, /* minimum */
  0x0001, /* maximum */
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemBacklight,
  (THMIMenuItemActionRoutine)0x00
};

THMIMenuItem MODCON_HMI_MENU_ITEM_LCD_CONTRAST =
{
  {'C', 'O', 'N', 'T', 'R', 'A', 'S', 'T'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0, /* user defined attribute */
  0,
  0x0000, /* minimum */
  0x003F, /* maximum */
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_UNSIGNED_INTEGER,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemContrast,
  (THMIMenuItemActionRoutine)0x00
};

THMIMenuItem MODCON_HMI_MENU_ITEM_RESET = 
{
  {'E', 'R', 'A', 'S', 'E', ' ', 'S', 'E', 'T', 'T', 'I', 'N', 'G', 'S'},
  HMI_MENU_ITEM_TYPE_ACTION,
  0, /* user defined attribute */
  0,
  0x0000, /* minimum */
  0x0000, /* maximum */
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_NONE,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  (THMIMenuItemUpdateRoutine)0x00,
  &EraseModConSettings
};

/* menu items for analog channels */
THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH1_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '1'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0, /* user defined attribute */
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputChannelValue,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH2_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '2'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  1, /* user defined attribute */
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputChannelValue,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH3_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '3'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  2, /* user defined attribute */
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputChannelValue,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH4_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '4'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  3, /* user defined attribute */
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputChannelValue,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH5_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '5'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  4, /* user defined attribute */
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputChannelValue,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH6_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '6'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  5, /* user defined attribute */
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputChannelValue,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH7_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '7'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  6, /* user defined attribute */
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputChannelValue,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH8_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '8'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  7, /* user defined attribute */
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputChannelValue,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH1_VALUE =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '1'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0, /* user defined attribute */
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogOutputChannelValue,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH2_VALUE =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '2'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  1, /* user defined attribute */
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogOutputChannelValue,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH3_VALUE =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '3'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  2, /* user defined attribute */
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogOutputChannelValue,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH4_VALUE =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '4'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  3, /* user defined attribute */
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogOutputChannelValue,
  (THMIMenuItemActionRoutine)0x00    
};
/* menu items for analog channels */

/* menu items for io switchs */
THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH1_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '1'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH1, /* user defined attribute */
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputChannelSwitch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH2_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '2'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH2, /* user defined attribute */
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputChannelSwitch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH3_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '3'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH3, /* user defined attribute */
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputChannelSwitch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH4_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '4'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH4, /* user defined attribute */
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputChannelSwitch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH5_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '5'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH5, /* user defined attribute */
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputChannelSwitch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH6_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '6'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH6, /* user defined attribute */
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputChannelSwitch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH7_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '7'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH7, /* user defined attribute */
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputChannelSwitch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH8_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '8'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH8, /* user defined attribute */
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputChannelSwitch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH1_SWITCH =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '1'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH1, /* user defined attribute */
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogOutputChannelSwitch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH2_SWITCH =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '2'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH2, /* user defined attribute */
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogOutputChannelSwitch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH3_SWITCH =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '3'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH3, /* user defined attribute */
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogOutputChannelSwitch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH4_SWITCH =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '4'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH4, /* user defined attribute */
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogOutputChannelSwitch,
  (THMIMenuItemActionRoutine)0x00    
};
/* menu items for io switchs */

THMIMenu MODCON_HMI_MENU_IDLE =
{
  HMI_MENU_TYPE_STATIC,
  {
    &MODCON_HMI_MENU_ITEM_VERSION,
    &MODCON_HMI_MENU_ITEM_NUMBER,
    &MODCON_HMI_MENU_ITEM_PROTOCOL,
    &MODCON_HMI_MENU_ITEM_DEBUG,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00    
  },
  4, /* total item count */
  0
};

THMIMenu MODCON_HMI_MENU_SETTING =
{
  HMI_MENU_TYPE_SETTING,
  {
    &MODCON_HMI_MENU_ITEM_LCD_BACKLIGHT,
    &MODCON_HMI_MENU_ITEM_LCD_CONTRAST,
    &MODCON_HMI_MENU_ITEM_NUMBER,
    &MODCON_HMI_MENU_ITEM_PROTOCOL,
    &MODCON_HMI_MENU_ITEM_DEBUG,
    &MODCON_HMI_MENU_ITEM_RESET,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00  
  },
  6, /* total item count */
  0
};

THMIMenu MODCON_HMI_MENU_ANALOG =
{
  HMI_MENU_TYPE_STATIC,
  {
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH1_VALUE,
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH2_VALUE,
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH3_VALUE,
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH4_VALUE,
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH5_VALUE,
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH6_VALUE,
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH7_VALUE,
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH8_VALUE,
    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH1_VALUE,
    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH2_VALUE,
    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH3_VALUE,
    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH4_VALUE,    
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00
  },
  12, /* total item count */
  0
};

THMIMenu MODCON_HMI_MENU_SWITCH =
{
  HMI_MENU_TYPE_SETTING,
  {
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH1_SWITCH,
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH2_SWITCH,
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH3_SWITCH,
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH4_SWITCH,
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH5_SWITCH,
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH6_SWITCH,
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH7_SWITCH,
    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH8_SWITCH,
    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH1_SWITCH,
    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH2_SWITCH,
    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH3_SWITCH,
    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH4_SWITCH,    
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00,
    (THMIMenuItem*) 0x00
  },
  12,
  0
};

THMIPanel MODCON_HMI_IDLE_PANEL = 
{
  0,
  {
    ' ',' ',' ',' ',' ',' ',' '
  },
  &IdlePanelInputProcessRoutine,
  (THMIPanelUpdateRoutine)0x00,
  &MODCON_HMI_MENU_IDLE,
  (THMIDialog*)0x00
};

THMIPanel MODCON_HMI_SETTING_PANEL = 
{
  1,
  {
    'S','E','T','T','I','N','G'
  },
  &SettingPanelInputProcessRoutine,
  &SettingPanelUpdateRoutine,
  &MODCON_HMI_MENU_SETTING,
  (THMIDialog*)0x00
};

THMIPanel MODCON_HMI_ANALOG_PANEL = 
{
  2,
  {
    'A', 'N', 'A', 'L', 'O', 'G', ' '
  },
  &AnalogPanelInputProcessRoutine,
  &AnalogPanelUpdateRoutine,
  &MODCON_HMI_MENU_ANALOG,
  (THMIDialog*)0x00
};

THMIPanel MODCON_HMI_SWITCH_PANEL =
{
  3,
  {
    'S', 'W', 'I', 'T', 'C', 'H', ' '
  },
  &SwitchPanelInputProcessRoutine,
  &SwitchPanelUpdateRoutine,
  &MODCON_HMI_MENU_SWITCH,
  (THMIDialog*)0x00
};

THMIDialog MODCON_HMI_CONFIRM_DIALOG = 
{
  {
    {
      'P', 'L', 'E', 'A', 'S', 'E', ' ', 'P', 'R', 'E', 'S', 'S', ' ', 'S', 'E', 'T'
    },
    {
      'T', 'O', ' ', 'C', 'O', 'N', 'F', 'I', 'R', 'M', '/', 'S', 'A', 'V', 'E', ' '
    },
    {
      'O', 'R', ' ', 'D', 'A', 'T', 'A', ' ', 'T', 'O', ' ', ' ', ' ', ' ', ' ', ' '
    },
    {
      'C', 'A', 'N', 'C', 'E', 'L', '/', 'D', 'I', 'S', 'C', 'A', 'R', 'D', '.', ' '
    }
  }
};

THMIPanel MODCON_HMI_CONFIRM_PANEL = 
{
  15,
  {
    'C', 'O', 'N', 'F', 'I', 'R', 'M'
  },
  &ConfirmPanelInputProcessRoutine,
  (THMIPanelUpdateRoutine)0x00,
  (THMIMenu*)0x00,
  &MODCON_HMI_CONFIRM_DIALOG
};

THMISetup MODCON_HMI_SETUP =
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
  },
  0,
  100,
  0,
  15
};

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

#endif
