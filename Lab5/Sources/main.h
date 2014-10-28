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

void (*ConfirmDialogCallback)(void);
void ApplyModConSettings(void);
void ApplyModConSwitchs(void);
void EraseModConSettings(void);

void UpdateMenuItemVersion(void);
void UpdateMenuItemNumber(void);
void UpdateMenuItemDebug(void);
void UpdateMenuItemProtocol(void);
void UpdateMenuItemBacklight(void);
void UpdateMenuItemContrast(void);

void UpdateMenuItemAnalogInputCh1Value(void);
void UpdateMenuItemAnalogInputCh2Value(void);
void UpdateMenuItemAnalogInputCh3Value(void);
void UpdateMenuItemAnalogInputCh4Value(void);
void UpdateMenuItemAnalogInputCh5Value(void);
void UpdateMenuItemAnalogInputCh6Value(void);
void UpdateMenuItemAnalogInputCh7Value(void);
void UpdateMenuItemAnalogInputCh8Value(void);

void UpdateMenuItemAnalogOutputCh1Value(void);
void UpdateMenuItemAnalogOutputCh2Value(void);
void UpdateMenuItemAnalogOutputCh3Value(void);
void UpdateMenuItemAnalogOutputCh4Value(void);

void UpdateMenuItemAnalogInputCh1Switch(void);
void UpdateMenuItemAnalogInputCh2Switch(void);
void UpdateMenuItemAnalogInputCh3Switch(void);
void UpdateMenuItemAnalogInputCh4Switch(void);
void UpdateMenuItemAnalogInputCh5Switch(void);
void UpdateMenuItemAnalogInputCh6Switch(void);
void UpdateMenuItemAnalogInputCh7Switch(void);
void UpdateMenuItemAnalogInputCh8Switch(void);

void UpdateMenuItemAnalogOutputCh1Switch(void);
void UpdateMenuItemAnalogOutputCh2Switch(void);
void UpdateMenuItemAnalogOutputCh3Switch(void);
void UpdateMenuItemAnalogOutputCh4Switch(void);

THMIMenuItem MODCON_HMI_MENU_ITEM_VERSION =
{
  {'V', 'E', 'R', 'S', 'I', 'O', 'N'},
  HMI_MENU_ITEM_TYPE_ENTRY,
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
  0,
  0x0000, /* minimum */
  0x0001, /* maximum */
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_SYNC_ASYNC,
  &UpdateMenuItemProtocol,
  (THMIMenuItemActionRoutine)0x00
};

THMIMenuItem MODCON_HMI_MENU_ITEM_LCD_BACKLIGHT =
{
  {'B', 'K', 'L', 'I', 'G', 'H', 'T'},
  HMI_MENU_ITEM_TYPE_ENTRY,
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
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputCh1Value,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH2_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '2'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputCh2Value,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH3_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '3'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputCh3Value,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH4_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '4'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputCh4Value,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH5_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '5'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputCh5Value,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH6_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '6'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputCh6Value,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH7_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '7'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputCh7Value,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH8_VALUE =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '8'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogInputCh8Value,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH1_VALUE =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '1'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogOutputCh1Value,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH2_VALUE =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '2'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogOutputCh2Value,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH3_VALUE =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '3'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogOutputCh3Value,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH4_VALUE =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '4'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0xFFFF,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT,
  &UpdateMenuItemAnalogOutputCh4Value,
  (THMIMenuItemActionRoutine)0x00    
};
/* menu items for analog channels */

/* menu items for io switchs */
THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH1_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '1'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputCh1Switch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH2_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '2'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputCh2Switch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH3_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '3'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputCh3Switch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH4_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '4'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputCh4Switch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH5_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '5'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputCh5Switch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH6_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '6'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputCh6Switch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH7_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '7'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputCh7Switch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_INPUT_CH8_SWITCH =
{
  {'I', 'N', 'P', 'U', 'T', ' ', 'C', 'H', '8'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogInputCh8Switch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH1_SWITCH =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '1'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogOutputCh1Switch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH2_SWITCH =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '2'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogOutputCh2Switch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH3_SWITCH =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '3'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogOutputCh3Switch,
  (THMIMenuItemActionRoutine)0x00    
};

THMIMenuItem MODCON_HMI_MENU_ITEM_ANALOG_OUTPUT_CH4_SWITCH =
{
  {'O', 'U', 'T', 'P', 'U', 'T', ' ', 'C', 'H', '4'},
  HMI_MENU_ITEM_TYPE_ENTRY,
  0,
  0x0000,
  0x0001,
  0,
  bFALSE,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  &UpdateMenuItemAnalogOutputCh4Switch,
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

BOOL IdlePanelInputProcessRoutine(THMIKey key);

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

BOOL IdlePanelInputProcessRoutine(THMIKey key)
{
  switch(key)
  {
    case HMI_KEY_SET:
      HMI_ShowPanel(1);
      return bTRUE;
      break;
    case HMI_KEY_DATA:
      HMI_ShowPanel(2);
      return bTRUE;
      break;
    default:
      break;
  };
  return bFALSE;
}

BOOL SettingPanelInputProcessRoutine(THMIKey key);
void SettingPanelUpdateRoutine(void);

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

BOOL SettingPanelInputProcessRoutine(THMIKey key)
{
  UINT8 focusedMenuItemIndex = HMI_GetFocusedMenuItemIndex();
  UINT8 selectedMenuItemIndex = HMI_GetSelectedMenuItemIndex();
  
  switch(key)
  {
    case HMI_KEY_SET:
    case HMI_KEY_DATA:
      if (HMI_GetSelectedMenuItemIndex() == 0xFF) /* no item selected */
      {
        ConfirmDialogCallback = &ApplyModConSettings;        
        HMI_ShowPanel(15);
      }
      return bTRUE;      
      break;
    default:
      break;
  }
  return bFALSE;
}

void SettingPanelUpdateRoutine(void)
{
  if (MODCON_HMI_SETTING_PANEL.menuPtr)
  {
    MODCON_HMI_SETTING_PANEL.menuPtr->startingMenuItemIndex = 0;
  }
}

BOOL AnalogPanelInputProcessRoutine(THMIKey key);
void AnalogPanelUpdateRoutine(void);

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

BOOL AnalogPanelInputProcessRoutine(THMIKey key)
{  
  switch(key)
  {
    case HMI_KEY_SET:
      HMI_ShowPanel(3);
      return bTRUE;
      break;
    case HMI_KEY_DATA:
      HMI_ClosePanel();
      return bTRUE;      
      break;
    default:
      break;
  }
  return bFALSE;
}

void AnalogPanelUpdateRoutine(void)
{
  if (MODCON_HMI_ANALOG_PANEL.menuPtr)
  {
    MODCON_HMI_ANALOG_PANEL.menuPtr->startingMenuItemIndex = 0;
  }
}

BOOL SwitchPanelInputProcessRoutine(THMIKey key);
void SwitchPanelUpdateRoutine(void);

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

BOOL SwitchPanelInputProcessRoutine(THMIKey key)
{  
  switch(key)
  {
    case HMI_KEY_SET:
    case HMI_KEY_DATA:
      ConfirmDialogCallback = &ApplyModConSwitchs;
      HMI_ShowPanel(15);
      return bTRUE;
      break;
    default:
      break;
  }
  return bFALSE;
}

void SwitchPanelUpdateRoutine(void)
{
  if (MODCON_HMI_ANALOG_PANEL.menuPtr)
  {
    MODCON_HMI_ANALOG_PANEL.menuPtr->startingMenuItemIndex = 0;
  }
}

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

BOOL ConfirmPanelInputProcessRoutine(THMIKey key);

THMIPanel MODCON_HMI_CONFIRM_PANEL = 
{
  15,
  {
    'C', 'O', 'N', 'F', 'I', 'R', 'M'
  },
  &ConfirmPanelInputProcessRoutine, /* TODO: rename it to ConfigPanelInputProcessRoutine */
  (THMIPanelUpdateRoutine)0x00,
  (THMIMenu*)0x00,
  &MODCON_HMI_CONFIRM_DIALOG
};

BOOL ConfirmPanelInputProcessRoutine(THMIKey key)
{
  switch(key)
  {
    case HMI_KEY_SET:
      //ApplyModConSettings(); /* TODO: rename it to something better */
      if (ConfirmDialogCallback)
      {
        ConfirmDialogCallback();
      }
      HMI_ClosePanel();
      return bTRUE;
      break;
    case HMI_KEY_DATA:
      HMI_ClosePanel();
      return bTRUE;
      break;
    default:
      break;
  }
  return bFALSE;
}

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
