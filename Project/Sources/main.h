/**
 * \mainpage Xu's Embedded Software Project
 *
 * \section intro_sec Introduction
 * Project is based on all previous works to implement a AWG.
 *
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
#include "timer.h"
#include "analog.h"
//#include "HMI.h"
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
const UINT8 MODCON_COMMAND_WAVE                = 0x60;
const UINT8 MODCON_COMMAND_ARBITRARY_WAVE      = 0x61;

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

const UINT8 MODCON_WAVE_STATUS         = 0;
const UINT8 MODCON_WAVE_WAVEFORM       = 1;
const UINT8 MODCON_WAVE_FREQUENCY      = 2;
const UINT8 MODCON_WAVE_AMPLITUDE      = 3;
const UINT8 MODCON_WAVE_OFFSET         = 4;
const UINT8 MODCON_WAVE_ON             = 5;
const UINT8 MODCON_WAVE_OFF            = 6;
const UINT8 MODCON_WAVE_ACTIVE_CHANNEL = 7;

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
#define DEFAULT_MODCON_ANALOG_OUTPUT_CHANNEL_SWITCH 0b0011 /* | Ch4 | Ch3 | Ch2 | Ch1 | */
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
//void EraseModConSettings(THMIMenuItem* itemPtr);

/**
 * \fn BOOL IdlePanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key)
 * \brief Handles idle panel inputs.
 * \param panelPtr A pointer of THMIPanel
 * \param key Key that has been pressed
 * \return TRUE if input has been processed or FALSE if it is not acceptable.
 */
//BOOL IdlePanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key);

/**
 * \fn BOOL SettingPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key)
 * \brief Handles setting panel inputs.
 * \param panelPtr A pointer of THMIPanel
 * \param key Key that has been pressed
 * \return TRUE if input has been processed or FALSE if it is not acceptable.
 */
//BOOL SettingPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key);

/**
 * \fn void SettingPanelUpdateRoutine(THMIPanel* panelPtr)
 * \brief Updates related setting panel parameters once it has been exposed.
 * \param panelPtr A pointer of THMIPanel
 */
//void SettingPanelUpdateRoutine(THMIPanel* panelPtr);

/**
 * \fn BOOL AnalogPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key)
 * \brief Handles analog panel inputs.
 * \param panelPtr A pointer of THMIPanel
 * \param key Key that has been pressed
 * \return TRUE if input has been processed or FALSE if it is not acceptable.
 */
//BOOL AnalogPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key);

/**
 * \fn void AnalogPanelUpdateRoutine(THMIPanel* panelPtr)
 * \brief Updates related analog panel parameters once it has been exposed.
 * \param panelPtr A pointer of THMIPanel
 */
//void AnalogPanelUpdateRoutine(THMIPanel* panelPtr);

/**
 * \fn BOOL SwitchPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key)
 * \brief Handles switch panel inputs.
 * \param panelPtr A pointer of THMIPanel
 * \param key Key that has been pressed
 * \return TRUE if input has been processed or FALSE if it is not acceptable.
 */
//BOOL SwitchPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key);

/**
 * \fn void SwitchPanelUpdateRoutine(THMIPanel* panelPtr)
 * \brief Updates related switch panel parameters once it has been exposed.
 * \param panelPtr A pointer of THMIPanel
 */
//void SwitchPanelUpdateRoutine(THMIPanel* panelPtr);

/**
 * \fn BOOL ConfirmPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key)
 * \brief Handles confirm panel inputs.
 * \param panelPtr A pointer of THMIPanel
 * \param key Key that has been pressed
 * \return TRUE if input has been processed or FALSE if it is not acceptable.
 */
//BOOL ConfirmPanelInputProcessRoutine(THMIPanel* panelPtr, THMIKey key);

/**
 * \fn void UpdateMenuItemVersion(THMIMenuItem* itemPtr)
 * \brief Updates version menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
//void UpdateMenuItemVersion(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemNumber(THMIMenuItem* itemPtr)
 * \brief Updates number menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
//void UpdateMenuItemNumber(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemDebug(THMIMenuItem* itemPtr)
 * \brief Updates debug menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
//void UpdateMenuItemDebug(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemProtocol(THMIMenuItem* itemPtr)
 * \brief Updates protocol menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
//void UpdateMenuItemProtocol(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemBacklight(THMIMenuItem* itemPtr)
 * \brief Updates backlight menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
//void UpdateMenuItemBacklight(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemContrast(THMIMenuItem* itemPtr)
 * \brief Updates contrast menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
//void UpdateMenuItemContrast(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemAnalogInputChannelValue(THMIMenuItem* itemPtr)
 * \brief Updates given input channel menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
//void UpdateMenuItemAnalogInputChannelValue(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemAnalogOutputChannelValue(THMIMenuItem* itemPtr)
 * \brief Updates given output channel menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
//void UpdateMenuItemAnalogOutputChannelValue(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemAnalogInputChannelSwitch(THMIMenuItem* itemPtr)
 * \brief Updates given input channel switch menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
//void UpdateMenuItemAnalogInputChannelSwitch(THMIMenuItem* itemPtr);

/**
 * \fn void UpdateMenuItemAnalogOutputChannelSwitch(THMIMenuItem* itemPtr)
 * \brief Updates given output channel switch menu item value.
 * \param itemPtr A pointer of THMIMenuItem
 */
//void UpdateMenuItemAnalogOutputChannelSwitch(THMIMenuItem* itemPtr);

/**
 * \fn void StoreHMIBacklightSetting(BOOL backlight)
 * \brief Stores given HMI display backlight setting to EEPROM
 * \param backlight Boolean state of HMI display backlight on or off
 */
void StoreHMIBacklightSetting(BOOL backlight);

/**
 * \fn void StoreHMIContrastSetting(UINT8 contrast)
 * \brief Stores given HMI display contrast setting to EEPROM
 * \param contrast a value from 0 to 63 of HMI display contrast level
 */
void StoreHMIContrastSetting(UINT8 contrast);

//THMIMenuItem MODCON_HMI_MENU_ITEM_VERSION =
//{
//  {'V', 'E', 'R', 'S', 'I', 'O', 'N'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  0, /* user defined attribute */
//  0,
//  0x0000, /* minimum */
//  0xFFFF, /* maximum */
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_VERSION_NUMBER,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemVersion,
//  (THMIMenuItemActionRoutine)0x00
//};

//THMIMenuItem MODCON_HMI_MENU_ITEM_NUMBER =
//{
//  {'N', 'U', 'M', 'B', 'E', 'R'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  0, /* user defined attribute */
//  0,
//  0x0000, /* minimum */
//  0xFFFF, /* maximum */
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_UNSIGNED_INTEGER,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemNumber,
//  (THMIMenuItemActionRoutine)0x00
//};

//THMIMenuItem MODCON_HMI_MENU_ITEM_DEBUG =
//{
//  {'D', 'E', 'B', 'U', 'G'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  0, /* user defined attribute */
//  0,
//  0x0000, /* minimum */
//  0x0001, /* maximum */
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemDebug,
//  (THMIMenuItemActionRoutine)0x00
//};

//THMIMenuItem MODCON_HMI_MENU_ITEM_PROTOCOL =
//{
//  {'P', 'R', 'O', 'T', 'O', 'C', 'O', 'L'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  0,      /* user defined attribute */
//  0,      /* menu item value */
//  0x0000, /* minimum */
//  0x0001, /* maximum */
//  0,      /* mutated menu item value */
//  bFALSE, /* use mutated value */
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_SYNC_ASYNC,
//  &UpdateMenuItemProtocol,
//  (THMIMenuItemActionRoutine)0x00
//};

//THMIMenuItem MODCON_HMI_MENU_ITEM_LCD_BACKLIGHT =
//{
//  {'B', 'K', 'L', 'I', 'G', 'H', 'T'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  0, /* user defined attribute */
//  0,
//  0x0000, /* minimum */
//  0x0001, /* maximum */
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
//  &UpdateMenuItemBacklight,
//  (THMIMenuItemActionRoutine)0x00
//};

//THMIMenuItem MODCON_HMI_MENU_ITEM_LCD_CONTRAST =
//{
//  {'C', 'O', 'N', 'T', 'R', 'A', 'S', 'T'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  0, /* user defined attribute */
//  0,
//  0x0000, /* minimum */
//  0x003F, /* maximum */
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_UNSIGNED_INTEGER,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemContrast,
//  (THMIMenuItemActionRoutine)0x00
//};

//THMIMenuItem MODCON_HMI_MENU_ITEM_RESET = 
//{
//  {'E', 'R', 'A', 'S', 'E', ' ', 'S', 'E', 'T', 'T', 'I', 'N', 'G', 'S'},
//  HMI_MENU_ITEM_TYPE_ACTION,
//  0, /* user defined attribute */
//  0,
//  0x0000, /* minimum */
//  0x0000, /* maximum */
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_NONE,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  (THMIMenuItemUpdateRoutine)0x00,
//  &EraseModConSettings
//};

/* menu items for analog channels */
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH1_VALUE =
//{
//  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '1'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  0, /* user defined attribute */
//  0, /* menu item value */
//  0x0000,
//  0xFFFF,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemAnalogInputChannelValue,
//  (THMIMenuItemActionRoutine)0x00    
//};

//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH2_VALUE =
//{
//  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '2'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  1, /* user defined attribute */
//  0, /* menu item value */
//  0x0000,
//  0xFFFF,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemAnalogInputChannelValue,
//  (THMIMenuItemActionRoutine)0x00    
//};

//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH3_VALUE =
//{
//  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '3'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  2, /* user defined attribute */
//  0, /* menu item value */
//  0x0000,
//  0xFFFF,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemAnalogInputChannelValue,
//  (THMIMenuItemActionRoutine)0x00    
//};

//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH4_VALUE =
//{
//  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '4'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  3, /* user defined attribute */
//  0, /* menu item value */
// 0x0000,
//  0xFFFF,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemAnalogInputChannelValue,
//  (THMIMenuItemActionRoutine)0x00    
//};

//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH5_VALUE =
//{
//  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '5'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  4, /* user defined attribute */
//  0, /* menu item value */
//  0x0000,
//  0xFFFF,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemAnalogInputChannelValue,
//  (THMIMenuItemActionRoutine)0x00    
//};

//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH6_VALUE =
//{
//  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '6'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  5, /* user defined attribute */
//  0, /* menu item value */
//  0x0000,
//  0xFFFF,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemAnalogInputChannelValue,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH7_VALUE =
//{
//  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '7'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  6, /* user defined attribute */
//  0, /* menu item value */
//  0x0000,
//  0xFFFF,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemAnalogInputChannelValue,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH8_VALUE =
//{
//  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '8'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  7, /* user defined attribute */
//  0, /* menu item value */
//  0x0000,
//  0xFFFF,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemAnalogInputChannelValue,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH1_VALUE =
//{
//  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '1'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  0, /* user defined attribute */
//  0, /* menu item value */
//  0x0000,
//  0xFFFF,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemAnalogOutputChannelValue,
//  (THMIMenuItemActionRoutine)0x00    
//};

//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH2_VALUE =
//{
//  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '2'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  1, /* user defined attribute */
//  0, /* menu item value */
//  0x0000,
//  0xFFFF,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemAnalogOutputChannelValue,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH3_VALUE =
//{
//  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '3'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  2, /* user defined attribute */
//  0, /* menu item value */
//  0x0000, /* minimum value */
//  0xFFFF, /* maximum value */
//  0,      /* mutated value */
//  bFALSE, /* useMutatedValue */
//  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemAnalogOutputChannelValue,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH4_VALUE =
//{
//  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '4'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  3, /* user defined attribute */
//  0, /* menu item value */
//  0x0000,
//  0xFFFF,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
//  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
//  &UpdateMenuItemAnalogOutputChannelValue,
//  (THMIMenuItemActionRoutine)0x00    
//};
///* menu items for analog channels */
//
///* menu items for io switchs */
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH1_SWITCH =
//{
//  {'I', 'N', ' ', 'C', 'H', '1'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH1, /* user defined attribute */
//  0, /* menu item value */
//  0x0000,
//  0x0001,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
//  &UpdateMenuItemAnalogInputChannelSwitch,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH2_SWITCH =
//{
//  {'I', 'N', ' ', 'C', 'H', '2'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH2, /* user defined attribute */
//  0, /* menu item value */
//  0x0000,
//  0x0001,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
//  &UpdateMenuItemAnalogInputChannelSwitch,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH3_SWITCH =
//{
//  {'I', 'N', ' ', 'C', 'H', '3'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH3, /* user defined attribute */
//  0, /* menu item value */
//  0x0000, /* minimum value */
//  0x0001,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
//  &UpdateMenuItemAnalogInputChannelSwitch,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH4_SWITCH =
//{
//  {'I', 'N', ' ', 'C', 'H', '4'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH4, /* user defined attribute */
//  0, /* menu item value */
//  0x0000, /* minimum value */
//  0x0001,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
//  &UpdateMenuItemAnalogInputChannelSwitch,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH5_SWITCH =
//{
//  {'I', 'N', ' ', 'C', 'H', '5'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH5, /* user defined attribute */
//  0, /* menu item value */
//  0x0000, /* minimum value */
//  0x0001,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
//  &UpdateMenuItemAnalogInputChannelSwitch,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH6_SWITCH =
//{
//  {'I', 'N', ' ', 'C', 'H', '6'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH6, /* user defined attribute */
//  0, /* menu item value */
//  0x0000, /* minimum value */
//  0x0001,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
//  &UpdateMenuItemAnalogInputChannelSwitch,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH7_SWITCH =
//{
//  {'I', 'N', ' ', 'C', 'H', '7'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH7, /* user defined attribute */
//  0, /* menu item value */
//  0x0000, /* minimum value */
//  0x0001,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
//  &UpdateMenuItemAnalogInputChannelSwitch,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH8_SWITCH =
//{
//  {'I', 'N', ' ', 'C', 'H', '8'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  MODCON_ANALOG_INPUT_CHANNEL_MASK_CH8, /* user defined attribute */
//  0, /* menu item value */
//  0x0000, /* minimum value */
//  0x0001,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
//  &UpdateMenuItemAnalogInputChannelSwitch,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH1_SWITCH =
//{
//  {'O', 'U', 'T', ' ', 'C', 'H', '1'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH1, /* user defined attribute */
//  0, /* menu item value */
//  0x0000, /* minimum value */
//  0x0001,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
//  &UpdateMenuItemAnalogOutputChannelSwitch,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH2_SWITCH =
//{
//  {'O', 'U', 'T', ' ', 'C', 'H', '2'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH2, /* user defined attribute */
//  0, /* menu item value */
//  0x0000, /* minimum value */
//  0x0001,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
//  &UpdateMenuItemAnalogOutputChannelSwitch,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH3_SWITCH =
//{
//  {'O', 'U', 'T', ' ', 'C', 'H', '3'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH3, /* user defined attribute */
//  0, /* menu item value */
//  0x0000, /* minimum value */
//  0x0001,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
//  &UpdateMenuItemAnalogOutputChannelSwitch,
//  (THMIMenuItemActionRoutine)0x00    
//};
//
//THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH4_SWITCH =
//{
//  {'O', 'U', 'T', ' ', 'C', 'H', '4'},
//  HMI_MENU_ITEM_TYPE_ENTRY,
//  MODCON_ANALOG_OUTPUT_CHANNEL_MASK_CH4, /* user defined attribute */
//  0, /* menu item value */
//  0x0000, /* minimum value */
//  0x0001,
//  0,
//  bFALSE,
//  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
//  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
//  &UpdateMenuItemAnalogOutputChannelSwitch,
//  (THMIMenuItemActionRoutine)0x00    
//};
///* menu items for io switchs */
//
//THMIMenu MODCON_HMI_MENU_IDLE =
//{
//  HMI_MENU_TYPE_STATIC,
//  {
//    &MODCON_HMI_MENU_ITEM_VERSION,
//    &MODCON_HMI_MENU_ITEM_NUMBER,
//    &MODCON_HMI_MENU_ITEM_PROTOCOL,
//    &MODCON_HMI_MENU_ITEM_DEBUG,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000    
//  },
//  4, /* total item count */
//  0  /* starting menu item index */
//};
//
//THMIMenu MODCON_HMI_MENU_SETTING =
//{
//  HMI_MENU_TYPE_SETTING,
//  {
//    &MODCON_HMI_MENU_ITEM_LCD_BACKLIGHT,
//    &MODCON_HMI_MENU_ITEM_LCD_CONTRAST,
//    &MODCON_HMI_MENU_ITEM_NUMBER,
//    &MODCON_HMI_MENU_ITEM_PROTOCOL,
//    &MODCON_HMI_MENU_ITEM_DEBUG,
//    &MODCON_HMI_MENU_ITEM_RESET,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000  
//  },
//  6, /* total item count */
//  0  /* starting menu item index */
//};
//
//THMIMenu MODCON_HMI_MENU_ANALOG =
//{
//  HMI_MENU_TYPE_STATIC,
//  {
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH1_VALUE,
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH2_VALUE,
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH3_VALUE,
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH4_VALUE,
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH5_VALUE,
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH6_VALUE,
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH7_VALUE,
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH8_VALUE,
//    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH1_VALUE,
//    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH2_VALUE,
//    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH3_VALUE,
//    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH4_VALUE,    
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000
//  },
//  12, /* total item count */
//  0   /* starting menu item index */
//};
//
//THMIMenu MODCON_HMI_MENU_SWITCH =
//{
//  HMI_MENU_TYPE_SETTING,
//  {
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH1_SWITCH,
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH2_SWITCH,
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH3_SWITCH,
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH4_SWITCH,
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH5_SWITCH,
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH6_SWITCH,
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH7_SWITCH,
//    &MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH8_SWITCH,
//    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH1_SWITCH,
//    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH2_SWITCH,
//    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH3_SWITCH,
//    &MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH4_SWITCH,    
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000,
//    (THMIMenuItem*) 0x0000
//  },
//  12,
//  0   /* starting menu item index */
//};
//
//THMIPanel MODCON_HMI_IDLE_PANEL = 
//{
//  0, /* panel id */
//  {
//    ' ',' ',' ',' ',' ',' ',' '
//  },
//  &IdlePanelInputProcessRoutine,
//  (THMIPanelUpdateRoutine)0x00,
//  &MODCON_HMI_MENU_IDLE,
//  (THMIDialog*)0x00
//};
//
//THMIPanel MODCON_HMI_SETTING_PANEL = 
//{
//  1,
//  {
//    'S','E','T','T','I','N','G'
//  },
//  &SettingPanelInputProcessRoutine,
//  &SettingPanelUpdateRoutine,
//  &MODCON_HMI_MENU_SETTING,
//  (THMIDialog*)0x00
//};
//
//THMIPanel MODCON_HMI_ANALOG_PANEL = 
//{
//  2,
//  {
//    'A', 'N', 'A', 'L', 'O', 'G', ' '
//  },
//  &AnalogPanelInputProcessRoutine,
//  &AnalogPanelUpdateRoutine,
//  &MODCON_HMI_MENU_ANALOG,
//  (THMIDialog*)0x00
//};
//
//THMIPanel MODCON_HMI_SWITCH_PANEL =
//{
//  3,
//  {
//    'S', 'W', 'I', 'T', 'C', 'H', ' '
//  },
//  &SwitchPanelInputProcessRoutine,
//  &SwitchPanelUpdateRoutine,
//  &MODCON_HMI_MENU_SWITCH,
//  (THMIDialog*)0x00
//};
//
//THMIDialog MODCON_HMI_CONFIRM_DIALOG = 
//{
//  {
//    {
//      'P', 'L', 'E', 'A', 'S', 'E', ' ', 'P', 'R', 'E', 'S', 'S', ' ', 'S', 'E', 'T'
//    },
//    {
//      'T', 'O', ' ', 'C', 'O', 'N', 'F', 'I', 'R', 'M', '/', 'S', 'A', 'V', 'E', ' '
//    },
//    {
//      'O', 'R', ' ', 'D', 'A', 'T', 'A', ' ', 'T', 'O', ' ', ' ', ' ', ' ', ' ', ' '
//    },
//    {
//      'C', 'A', 'N', 'C', 'E', 'L', '/', 'D', 'I', 'S', 'C', 'A', 'R', 'D', '.', ' '
//    }
//  }
//};
//
//THMIPanel MODCON_HMI_CONFIRM_PANEL = 
//{
//  15,
//  {
//    'C', 'O', 'N', 'F', 'I', 'R', 'M'
//  },
//  &ConfirmPanelInputProcessRoutine,
//  (THMIPanelUpdateRoutine)0x00,
//  (THMIMenu*)0x00,
//  &MODCON_HMI_CONFIRM_DIALOG
//};

//THMISetup MODCON_HMI_SETUP =
//{
//  HMI_RENDER_MODE_CONTINUITY, /* rendering mode */
//  {
//    { /* frame template contents */
//      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
//      {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
//      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
//      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
//      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
//      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
//      {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
//      {'S', 'E', 'T', ' ', 'D', 'A', 'T', 'A', ' ', '<', ' ', '>', ' ', 'S', 'E', 'L'}    
//    },
//    16, /* frame template width */
//    8   /* frame template height */
//  },
//  0,   /* idle panel id */
//  100, /* maximum idle timeout count 100ms percycle */
//  bFALSE, /* HMI display backlight */
//  &StoreHMIBacklightSetting,
//  15,     /* HMI display contrast */
//  &StoreHMIContrastSetting
//};

#define NB_AWG_CHANNELS          2
#define AWG_ANALOG_SAMPLING_RATE 24000
#define AWG_ARBITRARY_WAVE_SIZE  256

typedef enum
{
  AWG_WAVEFORM_DC,
  AWG_WAVEFORM_SINE,
  AWG_WAVEFORM_SQUARE,
  AWG_WAVEFORM_TRIANGLE,
  AWG_WAVEFORM_SAWTOOTH,
  AWG_WAVEFORM_NOISE,
  AWG_WAVEFORM_ARBITRARY
} TAWGWaveformType;

typedef struct
{
  TAWGWaveformType waveformType;

  UINT16 sample;  
  
  BOOL isActive;
  BOOL isEnabled;
  
  UINT16 frequency;
  UINT16 amplitude;
  INT16 offset;
    
} TAWGChannel;

TAWGChannel AWG_Channel[NB_OUTPUT_CHANNELS];

const UINT16 AWG_SINE_WAVE[1000] =
{
0x800,0x80c,0x819,0x826,0x833,0x840,0x84d,0x85a,0x866,0x873,
0x880,0x88d,0x89a,0x8a7,0x8b3,0x8c0,0x8cd,0x8da,0x8e7,0x8f3,
0x900,0x90d,0x91a,0x926,0x933,0x940,0x94c,0x959,0x966,0x973,
0x97f,0x98c,0x998,0x9a5,0x9b2,0x9be,0x9cb,0x9d7,0x9e4,0x9f0,
0x9fd,0xa09,0xa16,0xa22,0xa2e,0xa3b,0xa47,0xa53,0xa60,0xa6c,
0xa78,0xa84,0xa91,0xa9d,0xaa9,0xab5,0xac1,0xacd,0xad9,0xae5,
0xaf1,0xafd,0xb09,0xb15,0xb21,0xb2d,0xb38,0xb44,0xb50,0xb5c,
0xb67,0xb73,0xb7e,0xb8a,0xb96,0xba1,0xbac,0xbb8,0xbc3,0xbcf,
0xbda,0xbe5,0xbf0,0xbfc,0xc07,0xc12,0xc1d,0xc28,0xc33,0xc3e,
0xc49,0xc53,0xc5e,0xc69,0xc74,0xc7e,0xc89,0xc94,0xc9e,0xca9,
0xcb3,0xcbd,0xcc8,0xcd2,0xcdc,0xce6,0xcf1,0xcfb,0xd05,0xd0f,
0xd19,0xd23,0xd2c,0xd36,0xd40,0xd4a,0xd53,0xd5d,0xd66,0xd70,
0xd79,0xd82,0xd8c,0xd95,0xd9e,0xda7,0xdb0,0xdb9,0xdc2,0xdcb,
0xdd4,0xddd,0xde6,0xdee,0xdf7,0xdff,0xe08,0xe10,0xe19,0xe21,
0xe29,0xe31,0xe39,0xe41,0xe49,0xe51,0xe59,0xe61,0xe69,0xe70,
0xe78,0xe7f,0xe87,0xe8e,0xe96,0xe9d,0xea4,0xeab,0xeb2,0xeb9,
0xec0,0xec7,0xece,0xed5,0xedb,0xee2,0xee8,0xeef,0xef5,0xefc,
0xf02,0xf08,0xf0e,0xf14,0xf1a,0xf20,0xf26,0xf2b,0xf31,0xf37,
0xf3c,0xf42,0xf47,0xf4c,0xf51,0xf57,0xf5c,0xf61,0xf66,0xf6a,
0xf6f,0xf74,0xf79,0xf7d,0xf82,0xf86,0xf8a,0xf8f,0xf93,0xf97,
0xf9b,0xf9f,0xfa3,0xfa6,0xfaa,0xfae,0xfb1,0xfb5,0xfb8,0xfbb,
0xfbf,0xfc2,0xfc5,0xfc8,0xfcb,0xfce,0xfd0,0xfd3,0xfd6,0xfd8,
0xfdb,0xfdd,0xfdf,0xfe2,0xfe4,0xfe6,0xfe8,0xfea,0xfeb,0xfed,
0xfef,0xff0,0xff2,0xff3,0xff5,0xff6,0xff7,0xff8,0xff9,0xffa,
0xffb,0xffc,0xffc,0xffd,0xffe,0xffe,0xffe,0xfff,0xfff,0xfff,
0xfff,0xfff,0xfff,0xfff,0xffe,0xffe,0xffe,0xffd,0xffc,0xffc,
0xffb,0xffa,0xff9,0xff8,0xff7,0xff6,0xff5,0xff3,0xff2,0xff0,
0xfef,0xfed,0xfeb,0xfea,0xfe8,0xfe6,0xfe4,0xfe2,0xfdf,0xfdd,
0xfdb,0xfd8,0xfd6,0xfd3,0xfd0,0xfce,0xfcb,0xfc8,0xfc5,0xfc2,
0xfbf,0xfbb,0xfb8,0xfb5,0xfb1,0xfae,0xfaa,0xfa6,0xfa3,0xf9f,
0xf9b,0xf97,0xf93,0xf8f,0xf8a,0xf86,0xf82,0xf7d,0xf79,0xf74,
0xf6f,0xf6a,0xf66,0xf61,0xf5c,0xf57,0xf51,0xf4c,0xf47,0xf42,
0xf3c,0xf37,0xf31,0xf2b,0xf26,0xf20,0xf1a,0xf14,0xf0e,0xf08,
0xf02,0xefc,0xef5,0xeef,0xee8,0xee2,0xedb,0xed5,0xece,0xec7,
0xec0,0xeb9,0xeb2,0xeab,0xea4,0xe9d,0xe96,0xe8e,0xe87,0xe7f,
0xe78,0xe70,0xe69,0xe61,0xe59,0xe51,0xe49,0xe41,0xe39,0xe31,
0xe29,0xe21,0xe19,0xe10,0xe08,0xdff,0xdf7,0xdee,0xde6,0xddd,
0xdd4,0xdcb,0xdc2,0xdb9,0xdb0,0xda7,0xd9e,0xd95,0xd8c,0xd82,
0xd79,0xd70,0xd66,0xd5d,0xd53,0xd4a,0xd40,0xd36,0xd2c,0xd23,
0xd19,0xd0f,0xd05,0xcfb,0xcf1,0xce6,0xcdc,0xcd2,0xcc8,0xcbd,
0xcb3,0xca9,0xc9e,0xc94,0xc89,0xc7e,0xc74,0xc69,0xc5e,0xc53,
0xc49,0xc3e,0xc33,0xc28,0xc1d,0xc12,0xc07,0xbfc,0xbf0,0xbe5,
0xbda,0xbcf,0xbc3,0xbb8,0xbac,0xba1,0xb96,0xb8a,0xb7e,0xb73,
0xb67,0xb5c,0xb50,0xb44,0xb38,0xb2d,0xb21,0xb15,0xb09,0xafd,
0xaf1,0xae5,0xad9,0xacd,0xac1,0xab5,0xaa9,0xa9d,0xa91,0xa84,
0xa78,0xa6c,0xa60,0xa53,0xa47,0xa3b,0xa2e,0xa22,0xa16,0xa09,
0x9fd,0x9f0,0x9e4,0x9d7,0x9cb,0x9be,0x9b2,0x9a5,0x998,0x98c,
0x97f,0x973,0x966,0x959,0x94c,0x940,0x933,0x926,0x91a,0x90d,
0x900,0x8f3,0x8e7,0x8da,0x8cd,0x8c0,0x8b3,0x8a7,0x89a,0x88d,
0x880,0x873,0x866,0x85a,0x84d,0x840,0x833,0x826,0x819,0x80c,
0x800,0x7f3,0x7e6,0x7d9,0x7cc,0x7bf,0x7b2,0x7a5,0x799,0x78c,
0x77f,0x772,0x765,0x758,0x74c,0x73f,0x732,0x725,0x718,0x70c,
0x6ff,0x6f2,0x6e5,0x6d9,0x6cc,0x6bf,0x6b3,0x6a6,0x699,0x68c,
0x680,0x673,0x667,0x65a,0x64d,0x641,0x634,0x628,0x61b,0x60f,
0x602,0x5f6,0x5e9,0x5dd,0x5d1,0x5c4,0x5b8,0x5ac,0x59f,0x593,
0x587,0x57b,0x56e,0x562,0x556,0x54a,0x53e,0x532,0x526,0x51a,
0x50e,0x502,0x4f6,0x4ea,0x4de,0x4d2,0x4c7,0x4bb,0x4af,0x4a3,
0x498,0x48c,0x481,0x475,0x469,0x45e,0x453,0x447,0x43c,0x430,
0x425,0x41a,0x40f,0x403,0x3f8,0x3ed,0x3e2,0x3d7,0x3cc,0x3c1,
0x3b6,0x3ac,0x3a1,0x396,0x38b,0x381,0x376,0x36b,0x361,0x356,
0x34c,0x342,0x337,0x32d,0x323,0x319,0x30e,0x304,0x2fa,0x2f0,
0x2e6,0x2dc,0x2d3,0x2c9,0x2bf,0x2b5,0x2ac,0x2a2,0x299,0x28f,
0x286,0x27d,0x273,0x26a,0x261,0x258,0x24f,0x246,0x23d,0x234,
0x22b,0x222,0x219,0x211,0x208,0x200,0x1f7,0x1ef,0x1e6,0x1de,
0x1d6,0x1ce,0x1c6,0x1be,0x1b6,0x1ae,0x1a6,0x19e,0x196,0x18f,
0x187,0x180,0x178,0x171,0x169,0x162,0x15b,0x154,0x14d,0x146,
0x13f,0x138,0x131,0x12a,0x124,0x11d,0x117,0x110,0x10a,0x103,
0xfd,0xf7,0xf1,0xeb,0xe5,0xdf,0xd9,0xd4,0xce,0xc8,
0xc3,0xbd,0xb8,0xb3,0xae,0xa8,0xa3,0x9e,0x99,0x95,
0x90,0x8b,0x86,0x82,0x7d,0x79,0x75,0x70,0x6c,0x68,
0x64,0x60,0x5c,0x59,0x55,0x51,0x4e,0x4a,0x47,0x44,
0x40,0x3d,0x3a,0x37,0x34,0x31,0x2f,0x2c,0x29,0x27,
0x24,0x22,0x20,0x1d,0x1b,0x19,0x17,0x15,0x14,0x12,
0x10,0xf,0xd,0xc,0xa,0x9,0x8,0x7,0x6,0x5,
0x4,0x3,0x3,0x2,0x1,0x1,0x1,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x2,0x3,0x3,
0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xc,0xd,0xf,
0x10,0x12,0x14,0x15,0x17,0x19,0x1b,0x1d,0x20,0x22,
0x24,0x27,0x29,0x2c,0x2f,0x31,0x34,0x37,0x3a,0x3d,
0x40,0x44,0x47,0x4a,0x4e,0x51,0x55,0x59,0x5c,0x60,
0x64,0x68,0x6c,0x70,0x75,0x79,0x7d,0x82,0x86,0x8b,
0x90,0x95,0x99,0x9e,0xa3,0xa8,0xae,0xb3,0xb8,0xbd,
0xc3,0xc8,0xce,0xd4,0xd9,0xdf,0xe5,0xeb,0xf1,0xf7,
0xfd,0x103,0x10a,0x110,0x117,0x11d,0x124,0x12a,0x131,0x138,
0x13f,0x146,0x14d,0x154,0x15b,0x162,0x169,0x171,0x178,0x180,
0x187,0x18f,0x196,0x19e,0x1a6,0x1ae,0x1b6,0x1be,0x1c6,0x1ce,
0x1d6,0x1de,0x1e6,0x1ef,0x1f7,0x200,0x208,0x211,0x219,0x222,
0x22b,0x234,0x23d,0x246,0x24f,0x258,0x261,0x26a,0x273,0x27d,
0x286,0x28f,0x299,0x2a2,0x2ac,0x2b5,0x2bf,0x2c9,0x2d3,0x2dc,
0x2e6,0x2f0,0x2fa,0x304,0x30e,0x319,0x323,0x32d,0x337,0x342,
0x34c,0x356,0x361,0x36b,0x376,0x381,0x38b,0x396,0x3a1,0x3ac,
0x3b6,0x3c1,0x3cc,0x3d7,0x3e2,0x3ed,0x3f8,0x403,0x40f,0x41a,
0x425,0x430,0x43c,0x447,0x453,0x45e,0x469,0x475,0x481,0x48c,
0x498,0x4a3,0x4af,0x4bb,0x4c7,0x4d2,0x4de,0x4ea,0x4f6,0x502,
0x50e,0x51a,0x526,0x532,0x53e,0x54a,0x556,0x562,0x56e,0x57b,
0x587,0x593,0x59f,0x5ac,0x5b8,0x5c4,0x5d1,0x5dd,0x5e9,0x5f6,
0x602,0x60f,0x61b,0x628,0x634,0x641,0x64d,0x65a,0x667,0x673,
0x680,0x68c,0x699,0x6a6,0x6b3,0x6bf,0x6cc,0x6d9,0x6e5,0x6f2,
0x6ff,0x70c,0x718,0x725,0x732,0x73f,0x74c,0x758,0x765,0x772,
0x77f,0x78c,0x799,0x7a5,0x7b2,0x7bf,0x7cc,0x7d9,0x7e6,0x7f3
};

static TUINT16 AWG_ARBITRARY_WAVE[AWG_ARBITRARY_WAVE_SIZE] = {0};

void SampleAnalogOutputChannels(TTimerChannel channelNb);

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
 * \fn BOOL HandleModConWave(void)
 * \brief
 * \return
 */
BOOL HandleModConWave(void);

/**
 * \fn BOOL HandleModConArbitraryWave(void)
 * \brief
 * \return
 */
BOOL HandleModConArbitraryWave(void);

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
