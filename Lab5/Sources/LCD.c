// ----------------------------------------
// Filename: LCD.c
// Description: Routines to implement 
//   writing and reading from the graphical
//   LCD via PORTA and PORTB
// Author: PMcL
// Date: 08-Jun-12

// derivative information
#include <mc9s12a512.h>     

// new types
#include "types.h"

// Graphical LCD
#include "LCD.h"

#define LCD_WR                          PORTB_BIT0
#define LCD_RD                          PORTB_BIT1
#define LCD_CD                          PORTB_BIT2
#define LCD_RST                         PORTB_BIT3
#define LCD_ADJ                         PORTB_BIT4
#define LCD_CTRL                        PORTB_BIT5
#define LCD_LED                         PORTB_BIT6

//----------------------------------------------------------------------------------------
// Define Status byte
typedef union
{
  UINT8 Byte;
  struct
  {
    UINT8 STA0       :1;                // Command execution capability
    UINT8 STA1       :1;                // Data read/write capability
    UINT8 STA2       :1;                // Auto mode data read capability
    UINT8 STA3       :1;                // Auto mode data write capability
    UINT8            :1; 
    UINT8 STA5       :1;                // Controller operation capability
    UINT8 STA6       :1;                // Error flag
    UINT8 STA7       :1;                // Blink condition 0=Display off, 1=Normal display
  } Bits;
} LCD_STATUSSTR;

volatile LCD_STATUSSTR _LCD_STATUS;

#define LCD_STATUS                      _LCD_STATUS.Byte
#define LCD_STA0                        _LCD_STATUS.Bits.STA0
#define LCD_STA1                        _LCD_STATUS.Bits.STA1

//----------------------------------------------------------------------------------------
// Define Display Mode byte
typedef union
{
  UINT8 Byte;
  struct
  {
    UINT8 cursorBlink      :1;                // Cursor blink
    UINT8 cursorDisplay    :1;                // Cursor display
    UINT8 textDisplay      :1;                // Text display
    UINT8 graphicDisplay   :1;                // Graphic display
  } Bits;
} LCD_DISPLAYMODESTR;

volatile LCD_DISPLAYMODESTR _LCD_DISPLAYMODE;

#define DisplayMode                     _LCD_DISPLAYMODE.Byte
#define DisplayMode_CursorBlink         _LCD_DISPLAYMODE.Bits.cursorBlink
#define DisplayMode_CursorDisplay       _LCD_DISPLAYMODE.Bits.cursorDisplay
#define DisplayMode_TextDisplay         _LCD_DISPLAYMODE.Bits.textDisplay
#define DisplayMode_GraphicDisplay      _LCD_DISPLAYMODE.Bits.graphicDisplay


//----------------------------------------------------------------------------------------
// Constants

// Safety count maximum value - used for LCD status checks
const UINT16 STATUS_COUNT_MAX = 50;
// LCD turn on delay
const UINT16 LCD_TURN_ON_DELAY = 1200;
// Maximum contrast setting
const UINT8 MAX_CONTRAST = 64;
// Default LCD contrast
const UINT8 LCD_DEFAULT_CONTRAST = 15;
// Blank line
const char LCD_BLANKLINE_STR[] = "                ";
// Number of lines on LCD
const UINT8 MAX_LCD_LINES = 8;
// LCD text size in characters
const UINT8 LCD_TEXT_SIZE_X = 16;

// LCD text and graphics addresses
const UINT16 LCD_TEXT_HOME_ADDRESS    = 0x0000;
const UINT16 LCD_GRAPHIC_HOME_ADDRESS	= 0x8000;
const UINT16 LCD_USER_CG_RAM_START_ADDRESS = 0x0C00;

// Enumerated types for LCD commands
typedef enum
{
  LCD_DATA_ZERO,
  LCD_DATA_ONE,
  LCD_DATA_TWO
} TLCDNbDataBytes;

typedef enum
{
  ORMode = 0,
  XORMode = 1,
  ANDMode = 3,
  TextAttributeMode = 4
} TLCDTextMode;

typedef enum
{
  StatusManual    = 0x03,
  StatusAutoRead  = 0x04,
  StatusAutoWrite = 0x08
} TLCDStatus;

// LCD commands
const UINT8 LCD_CMD_SET_CURSOR_POINTER       = 0x21;
const UINT8 LCD_CMD_SET_OFFSET_REGISTER      = 0x22;
const UINT8 LCD_CMD_SET_ADDRESS_POINTER      = 0x24;
const UINT8 LCD_CMD_SET_TEXT_HOME_ADDRESS    = 0x40;
const UINT8 LCD_CMD_SET_TEXT_AREA            = 0x41;
const UINT8 LCD_CMD_SET_GRAPHIC_HOME_ADDRESS = 0x42;
const UINT8 LCD_CMD_SET_GRAPHIC_AREA         = 0x43;
const UINT8 LCD_CMD_TEXT_MODE                = 0x80;
const UINT8 LCD_CMD_DISPLAY_OFF              = 0x90;
const UINT8 LCD_CMD_CURSOR_LINES             = 0xA0;
const UINT8 LCD_CMD_SET_DATA_AUTO_WRITE      = 0xB0;
const UINT8 LCD_CMD_SET_DATA_AUTO_READ       = 0xB1;
const UINT8 LCD_CMD_SET_DATA_AUTO_RESET      = 0xB2;
const UINT8 LCD_CMD_DATA_WRITE_INC_ADP       = 0xC0;
const UINT8 LCD_CMD_DATA_READ_INC_ADP        = 0xC1;
const UINT8 LCD_CMD_DATA_WRITE_DEC_ADP       = 0xC2;
const UINT8 LCD_CMD_DATA_READ_DEC_ADP        = 0xC3;
const UINT8 LCD_CMD_DATA_WRITE_NON_VAR_ADP   = 0xC4;
const UINT8 LCD_CMD_DATA_READ_NON_VAR_ADP    = 0xC5;
const UINT8 LCD_CMD_SCREEN_PEEK              = 0xE0;
const UINT8 LCD_CMD_SCREEN_COPY              = 0xE8;
const UINT8 LCD_CMD_BIT_RESET                = 0xF0;
const UINT8 LCD_CMD_BIT_SET                  = 0xF8;

// Masks for display command and text mode
const UINT8 LCD_CMD_EXTERNAL_CG_RAM_MODE     = 0x08;


// ----------------------------------------
// StatusCheck
// 
// Checks the status of the LCD
// Input:
//   statusMask is the bit mask used to check particular status bits
// Output:
//   returns bTRUE if the LCD is ready, otherwise FALSE
// Conditions:
//   LCD has been set up

BOOL StatusCheck(const TLCDStatus statusMask)
{
  UINT16 safetyCount = 0;

  // Turn Port A into an input port
  DDRA = 0x00;

  // Ask for status
  LCD_CD = 1;
 
  // Check response
  do
  {
    LCD_RD = 0;
    LCD_STATUS = PORTA;
    LCD_RD = 1;
    safetyCount++;
        
  } while (((LCD_STATUS & statusMask) != statusMask) && (safetyCount < STATUS_COUNT_MAX));

  // Turn Port A into an output port
  DDRA = 0xFF;

  __RESET_WATCHDOG();

  if (safetyCount < STATUS_COUNT_MAX)
    return bTRUE;
  
  return bFALSE;
}

// ----------------------------------------
// WriteByte
// 
// Writes a byte to the LCD
// Input:
//   data is a byte to be written to the LCD
//   cmd determines whether the byte is a command or data
// Output:
//   none
// Conditions:
//   LCD has been set up, 24 MHz bus clock is used (41.7 ns cycle time)

void WriteByte(const UINT8 data, const BOOL cmd)
{
  LCD_CD = (UINT8)cmd;
  LCD_WR = 0;
  PORTA = data;
  LCD_WR = 1;
}

// ----------------------------------------
// SendCommand
// 
// Sends a command to the LCD
// Input:
//   command is the LCD command
//   nbDataBytes is the number of data bytes that the command uses
//   data1 is the first data byte
//   data2 is the second data byte
// Output:
//   returns bTRUE if the command was successful, otherwise FALSE
// Conditions:
//   none

BOOL SendCommand(const UINT8 command, const TLCDNbDataBytes nbDataBytes, const UINT8 data1, const UINT8 data2)
{
  BOOL success;

  // Send first data byte if it exists
  if (nbDataBytes >= LCD_DATA_ONE)
  {
    success = StatusCheck(StatusManual);
    if (!success)
      return success;
    WriteByte(data1, bFALSE);
  
    // Send second data byte if it exists
    if (nbDataBytes == LCD_DATA_TWO)
    {
      success = StatusCheck(StatusManual);
      if (!success)
        return success;
      WriteByte(data2, bFALSE);
    }
  }
  
  // Send command
  success = StatusCheck(StatusManual);
  if (!success)
    return success;
  WriteByte(command, bTRUE);
  
  return success;
}

/**
 * \fn BOOL LCD_Setup(void)
 * \brief Sets up the graphical LCD
 * \return bTRUE if the LCD was successfully set up, else bFALSE
 */
 BOOL LCD_Setup(void)
{
  /* Toshiba T6963 */
  
  UINT16 delay;
  BOOL success;

  // Ensure WR/RD lines are not asserted
  LCD_WR = 1;
  LCD_RD = 1;

  // Reset LCD
  LCD_RST = 0;

  // Turn Port B into an output port, except for bit 7 (audio volume control)
  DDRB = 0x7F;

  // Wait for 5 clock cycles of the LCD oscillator (2.5 us for a 2 MHz clock)
  for (delay = 0; delay < LCD_TURN_ON_DELAY; delay++);

  // Bring LCD out of reset
  LCD_RST = 1;

  // Set LCD contrast
  success = LCD_SetContrast(LCD_DEFAULT_CONTRAST);
  if (!success)
    return success;
  
  // Set text home address
  success = SendCommand(LCD_CMD_SET_TEXT_HOME_ADDRESS, 2, (UINT8)LCD_TEXT_HOME_ADDRESS, (UINT8)(LCD_TEXT_HOME_ADDRESS >> 8));
  if (!success)
    return success;
  
  // Set text area line length in multiples of 8 pixels
  success = SendCommand(LCD_CMD_SET_TEXT_AREA, 2, LCD_TEXT_SIZE_X, 0);
  if (!success)
    return success;
  
  // Set graphic home address
  success = SendCommand(LCD_CMD_SET_GRAPHIC_HOME_ADDRESS, 2, (UINT8)(LCD_GRAPHIC_HOME_ADDRESS & 0x00ff), (UINT8)(LCD_GRAPHIC_HOME_ADDRESS >> 8));
  if (!success)
    return success;
  
  // Set graphic area line length in multiples of 8 pixels
  success = SendCommand(LCD_CMD_SET_GRAPHIC_AREA, 2, LCD_TEXT_SIZE_X, 0);
  if (!success)
    return success;
  
  // Set Mode to OR so that text overwrites graphics
  success = SendCommand(LCD_CMD_TEXT_MODE + (UINT8)ORMode, 0, 0, 0);
  if (!success)
    return success;
  
  // Clear LCD
  success = LCD_Clear();
  if (!success)
    return success;
  
  // Set display mode
  DisplayMode = LCD_CMD_DISPLAY_OFF;
  DisplayMode_CursorBlink = 0;
  DisplayMode_CursorDisplay = 0;
  DisplayMode_TextDisplay = 1;
  DisplayMode_GraphicDisplay = 0;
  success = SendCommand(DisplayMode, 0, 0, 0);
  if (!success)
    return success;
  
  success = SendCommand(LCD_CMD_SET_ADDRESS_POINTER, 2, 0, 0);
  return success;
}

/**
 * \fn BOOL LCD_OutChar(const UINT8 data)
 * \brief Writes a character to the LCD and increments the address pointer
 * \param data A byte to be written to the LCD
 * \return bTRUE if the command was successful, otherwise FALSE
 * \warning Assumes LCD has been set up
 */
BOOL LCD_OutChar(const UINT8 data)
{
  // Converts ASCII to LCD's "ROM 0101"
  if (data < 128)
    return SendCommand(LCD_CMD_DATA_WRITE_INC_ADP, 1, data - 32, 0);
  else
    return bFALSE;
}

// ----------------------------------------
// WriteAuto
// 
// Writes data to the LCD in auto mode
// Input:
//   nbBytes is the number of bytes to write
//   data is a pointer to the bytes to write
//   convert determines whether the byte is converted from ASCII to ROM0101
// Output:
//   returns bTRUE if the command was successful, otherwise FALSE
// Conditions:
//   none

BOOL WriteAuto(const UINT16 nbBytes, const char *data, const BOOL convert)
{
  UINT16 byteNb;
  BOOL success;
  
  // Check for NULL pointer
  if (!data || (nbBytes == 0))
    return bTRUE;
  
  // Set auto write mode
  success = SendCommand(LCD_CMD_SET_DATA_AUTO_WRITE, 0, 0, 0);  
  if (!success)
    return success;
      
  // Write data bytes
  for (byteNb = 0; byteNb < nbBytes; byteNb++)
  {
    success = StatusCheck(StatusAutoWrite);
    if (!success)
      return success;
    if (convert)
      WriteByte((*data++) - 32, bFALSE);
    else
      WriteByte(*data++, bFALSE);
  }
  
  // Reset auto write mode
  success = SendCommand(LCD_CMD_SET_DATA_AUTO_RESET, 0, 0, 0);  
  return success;
}

/**
 * \fn BOOL LCD_OutString(const char *str)
 * \brief Writes a string to the LCD
 * \param str A string to be written to the LCD
 * \return bTRUE if the command was successful, otherwise FALSE
 * \warning Assumes LCD has been set up
 * \warning Given string has to end with \0
 */
BOOL LCD_OutString(const char *str)
{
  const char *strPtr = str;
  UINT16 nbBytes = 0;
  
  // Check for NULL pointer
  if (!str)
    return bTRUE;
  
  // Count number of bytes in string
  while (*strPtr++)
    nbBytes++;

  // Write string using auto mode
  return WriteAuto(nbBytes, str, bTRUE);
}

BOOL LCD_OutFrame(const char frame[8][16]) 
{
  return SendCommand(LCD_CMD_SET_ADDRESS_POINTER, 2, 0, 0) && WriteAuto(128, (const char*)frame, bTRUE);
}

/**
 * \fn BOOL LCD_Clear(void)
 * \brief Clears the LCD 
 * \return bTRUE if the clear was successful, otherwise FALSE
 * \warning Assumes LCD has been set up
 */
BOOL LCD_Clear(void)
{
  UINT8 lineNb;
  BOOL success;
  
  success = SendCommand(LCD_CMD_SET_ADDRESS_POINTER, 2, 0, 0);
  if (!success)
    return success;

  for (lineNb = 0; lineNb < MAX_LCD_LINES; lineNb++)
  {
    success = LCD_OutString(LCD_BLANKLINE_STR);
    if (!success)
      return success;
  }
  return success;
}

// ----------------------------------------
// LCD_SetContrast
// 
// Sets the contrast of the LCD
// Input:
//   contrast is a value between 0 and 63 (according to the MAX749 datasheet)
// Output:
//   returns bTRUE if the contrast was in range, otherwise FALSE
// Conditions:
//   Assumes LCD contrast has started in shutdown mode

BOOL LCD_SetContrast(const UINT8 contrast)
{
  UINT8 nbSteps, stepNb;
  
  if (contrast > 63)
    return bFALSE;
  
  // Start at mid-range
  LCD_ADJ = 1;
  LCD_CTRL = 0;

  // Minimum delay is 100 ns
  asm nop;
  asm nop;
  asm nop;

  // Set default signals for contrast adjustment
  LCD_CTRL = 1;
  LCD_ADJ = 0;

  // Increment to the desired contrast
  nbSteps = contrast + MAX_CONTRAST / 2;
  if (nbSteps >= MAX_CONTRAST)
    nbSteps -= MAX_CONTRAST;
  for (stepNb = 0; stepNb < nbSteps; stepNb++)
  {
    LCD_ADJ = 1;
    // Minimum high is 100 ns
    asm nop;
    asm nop;
    asm nop;
    LCD_ADJ = 0;
    // Minimum low is 200 ns
    asm nop;
    asm nop;
    asm nop;
    asm nop;
    asm nop;
  }
  return bTRUE;
}

// ----------------------------------------
// LCD_Backlight
// 
// Sets the LED backlight of the LCD on or off
// Input:
//   LEDBacklightOn determines whether the LED backlight is on or off
// Output:
//   none
// Conditions:
//   none

void LCD_Backlight(const BOOL LEDBacklightOn)
{
  LCD_LED = (UINT8)LEDBacklightOn;
}
