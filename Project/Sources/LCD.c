/**
 * \file LCD.c
 * \brief Routines to implement writing and reading from the graphical LCD via PORTA and PORTB
 * \author
 * \date 
 */
#include "LCD.h"
#include <mc9s12a512.h>     

#define LCD_WR                          PORTB_BIT0
#define LCD_RD                          PORTB_BIT1
#define LCD_CD                          PORTB_BIT2
#define LCD_RST                         PORTB_BIT3
#define LCD_ADJ                         PORTB_BIT4
#define LCD_CTRL                        PORTB_BIT5
#define LCD_LED                         PORTB_BIT6

/**
 * define Status byte
 */
typedef union
{
  UINT8 Byte;
  struct
  {
    UINT8 STA0       :1; /* Command execution capability                    */
    UINT8 STA1       :1; /* Data read/write capability                      */
    UINT8 STA2       :1; /* Auto mode data read capability                  */
    UINT8 STA3       :1; /* Auto mode data write capability                 */
    UINT8            :1; 
    UINT8 STA5       :1; /* Controller operation capability                 */
    UINT8 STA6       :1; /* Error flag                                      */
    UINT8 STA7       :1; /* Blink condition 0=Display off, 1=Normal display */
  } Bits;
} LCD_STATUSSTR;

volatile LCD_STATUSSTR _LCD_STATUS;

#define LCD_STATUS                      _LCD_STATUS.Byte
#define LCD_STA0                        _LCD_STATUS.Bits.STA0
#define LCD_STA1                        _LCD_STATUS.Bits.STA1

/**
 * define Display Mode byte
 */
typedef union
{
  UINT8 Byte;
  struct
  {
    UINT8 cursorBlink      :1; /* Cursor blink    */
    UINT8 cursorDisplay    :1; /* Cursor display  */
    UINT8 textDisplay      :1; /* Text display    */
    UINT8 graphicDisplay   :1; /* Graphic display */
  } Bits;
} LCD_DISPLAYMODESTR;

volatile LCD_DISPLAYMODESTR _LCD_DISPLAYMODE;

#define DisplayMode                     _LCD_DISPLAYMODE.Byte
#define DisplayMode_CursorBlink         _LCD_DISPLAYMODE.Bits.cursorBlink
#define DisplayMode_CursorDisplay       _LCD_DISPLAYMODE.Bits.cursorDisplay
#define DisplayMode_TextDisplay         _LCD_DISPLAYMODE.Bits.textDisplay
#define DisplayMode_GraphicDisplay      _LCD_DISPLAYMODE.Bits.graphicDisplay

/**
 * safety count maximum value - used for LCD status checks
 */
const UINT16 STATUS_COUNT_MAX = 50;
/**
 * LCD turn on delay
 */
const UINT16 LCD_TURN_ON_DELAY = 1200;
/**
 * blank line
 */
const char LCD_BLANKLINE_STR[] = "                ";

/**
 * LCD text and graphics addresses
 */
const UINT16 LCD_TEXT_HOME_ADDRESS    = 0x0000;
const UINT16 LCD_GRAPHIC_HOME_ADDRESS	= 0x8000;
const UINT16 LCD_USER_CG_RAM_START_ADDRESS = 0x0C00;

/**
 * enumerated types for LCD commands
 */
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

/**
 * LCD commands
 */
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

/**
 * masks for display command and text mode
 */
const UINT8 LCD_CMD_EXTERNAL_CG_RAM_MODE     = 0x08;

/**
 * \fn BOOL StatusCheck(const TLCDStatus statusMask)
 * \brief Checks the status of the LCD
 * \param statusMask the bit mask used to check particular status bits
 * \return bTRUE if the LCD is ready, otherwise FALSE
 * \warning LCD has been set up
 */
BOOL StatusCheck(const TLCDStatus statusMask)
{
  UINT16 safetyCount = 0;

  /* turn Port A into an input port */
  DDRA = 0x00;

  /* ask for status */
  LCD_CD = 1;
 
  /* check response */
  do
  {
    LCD_RD = 0;
    LCD_STATUS = PORTA;
    LCD_RD = 1;
    safetyCount++;
        
  } while (((LCD_STATUS & statusMask) != statusMask) && (safetyCount < STATUS_COUNT_MAX));

  /* turn Port A into an output port */
  DDRA = 0xFF;

  __RESET_WATCHDOG();

  if (safetyCount < STATUS_COUNT_MAX)
  {    
    return bTRUE;
  }
  return bFALSE;
}

/**
 * \fn void WriteByte(const UINT8 data, const BOOL cmd)
 * \brief Writes a byte to the LCD
 * \param data a byte to be written to the LCD
 * \param cmd whether the byte is a command or data
 * \warning LCD has been set up, 24 MHz bus clock is used (41.7 ns cycle time)
 */
void WriteByte(const UINT8 data, const BOOL cmd)
{
  LCD_CD = (UINT8)cmd;
  LCD_WR = 0;
  PORTA = data;
  LCD_WR = 1;
}

/**
 * \fn BOOL SendCommand(const UINT8 command, const TLCDNbDataBytes nbDataBytes, const UINT8 data1, const UINT8 data2)
 * \brief Sends a command to the LCD
 * \param the LCD command
 * \param the number of data bytes that the command uses
 * \param the first data byte
 * \param the second data byte
 * \return bTRUE if the command was successful, otherwise FALSE
 */
BOOL SendCommand(const UINT8 command, const TLCDNbDataBytes nbDataBytes, const UINT8 data1, const UINT8 data2)
{
  BOOL success;

  /* send first data byte if it exists */
  if (nbDataBytes >= LCD_DATA_ONE)
  {
    success = StatusCheck(StatusManual);
    if (!success)
    {      
      return success;
    }
    WriteByte(data1, bFALSE);
  
    /* send second data byte if it exists */
    if (nbDataBytes == LCD_DATA_TWO)
    {
      success = StatusCheck(StatusManual);
      if (!success)
      {        
        return success;
      }
      WriteByte(data2, bFALSE);
    }
  }
  
  /* send command */
  success = StatusCheck(StatusManual);
  if (!success)
  {    
    return success;
  }
  WriteByte(command, bTRUE);
  
  return success;
}

/**
 * \fn BOOL SetTextHomeAddress(UINT8 lsb, UINT8 msb)
 * \brief Sets the starting address in the external display RAM for text display. The text home address indicates the leftmost and uppermost position.
 * \param lsb less significant byte of the address
 * \param msb most significant byte of the address
 * \return bTRUE if the text home address was successfully set up, else bFALSE
 */
BOOL SetTextHomeAddress(UINT8 lsb, UINT8 msb)
{
  return SendCommand(LCD_CMD_SET_TEXT_HOME_ADDRESS, 2, lsb, msb);
}

/**
 * \fn BOOL SetGraphicHomeAddress(UINT8 lsb, UINT8 msb)
 * \brief Sets the starting address in the external display RAM for graphic display. The text home address indicates the leftmost and uppermost position.
 * \param lsb less significant byte of the address
 * \param msb most significant byte of the address
 * \return bTRUE if the graphic home address was successfully set up, else bFALSE
 */
BOOL SetGraphicHomeAddress(UINT8 lsb, UINT8 msb)
{
  return SendCommand(LCD_CMD_SET_GRAPHIC_HOME_ADDRESS, 2, lsb, msb);
}

/**
 * \fn BOOL SetTextMode(TLCDTextMode mode)
 * \brief Sets the mode of text.  
 * \param mode predefined acceptable text mode.
 */
BOOL SetTextMode(TLCDTextMode mode)
{
  return SendCommand(LCD_CMD_TEXT_MODE + (UINT8)mode, 0, 0, 0);
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

  /* ensure WR/RD lines are not asserted */
  LCD_WR = 1;
  LCD_RD = 1;

  /* reset LCD */
  LCD_RST = 0;

  /* turn Port B into an output port, except for bit 7 (audio volume control) */
  DDRB = 0x7F;

  /* wait for 5 clock cycles of the LCD oscillator (2.5 us for a 2 MHz clock) */
  for (delay = 0; delay < LCD_TURN_ON_DELAY; delay++);

  /* bring LCD out of reset */
  LCD_RST = 1;

  /* set LCD contrast */
  success = LCD_SetContrast(LCD_DEFAULT_CONTRAST);
  if (!success)
  {    
    return success;
  }
  
  /* set text home address */
  success = SetTextHomeAddress((UINT8)(LCD_TEXT_HOME_ADDRESS & 0x00FF), (UINT8)(LCD_TEXT_HOME_ADDRESS >> 8));
  if (!success)
  {    
    return success;
  }
  
  /* set text area line length in multiples of 8 pixels */
  success = SendCommand(LCD_CMD_SET_TEXT_AREA, 2, LCD_TEXT_SIZE_X, 0);
  if (!success)
  {    
    return success;
  }
  
  /* set graphic home address */
  success = SetGraphicHomeAddress((UINT8)(LCD_GRAPHIC_HOME_ADDRESS & 0x00FF), (UINT8)(LCD_GRAPHIC_HOME_ADDRESS >> 8));
  if (!success)
  {    
    return success;
  }
  
  /* set graphic area line length in multiples of 8 pixels */
  success = SendCommand(LCD_CMD_SET_GRAPHIC_AREA, 2, LCD_TEXT_SIZE_X, 0);
  if (!success) 
  {    
    return success;
  }
  
  /* set Mode to OR so that text overwrites graphics */
  success = SetTextMode(ORMode);
  if (!success)
  {    
    return success;
  }
  
  /* clear LCD */
  success = LCD_Clear();
  if (!success)
  {    
    return success;
  }
  
  /* set display mode */
  DisplayMode = LCD_CMD_DISPLAY_OFF;
  DisplayMode_CursorBlink = 0;
  DisplayMode_CursorDisplay = 0;
  DisplayMode_TextDisplay = 1;
  DisplayMode_GraphicDisplay = 0;
  success = SendCommand(DisplayMode, 0, 0, 0);
  if (!success)
  {    
    return success;
  }
  
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

/**
 * \fn BOOL WriteAuto(const UINT16 nbBytes, const char *data, const BOOL convert)
 * \brief Writes data to the LCD in auto mode
 * \param nbBytes the number of bytes to write
 * \param data a pointer to the bytes to write
 * \param convert whether the byte is converted from ASCII to ROM0101
 * \return returns bTRUE if the command was successful, otherwise FALSE
 */
BOOL WriteAuto(const UINT16 nbBytes, const char *data, const BOOL convert)
{
  UINT16 byteNb;
  
  /* check for NULL pointer */
  if (!data || (nbBytes == 0))
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_INVALID_ARGUMENT);
#endif    
    return bFALSE;
  }
  
  /* set auto write mode */
  if (!SendCommand(LCD_CMD_SET_DATA_AUTO_WRITE, 0, 0, 0))
  {
    return bFALSE;
  }
      
  /* write data bytes */
  for (byteNb = 0; byteNb < nbBytes; byteNb++)
  {
    if (!StatusCheck(StatusAutoWrite))
    {
      return bFALSE;
    }
    
    if (convert)
    {      
      WriteByte((*data++) - 32, bFALSE);
    }
    else
    {      
      WriteByte(*data++, bFALSE);
    }
  }
  
  /* reset auto write mode */
  return SendCommand(LCD_CMD_SET_DATA_AUTO_RESET, 0, 0, 0);  
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
  
  /* Check for NULL pointer */
  if (!str)
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif    
    return bFALSE;
  }
  
  /* count number of bytes in string */
  while (*strPtr++)
  {    
    nbBytes++;
  }

  /* Write string using auto mode */
  return WriteAuto(nbBytes, str, bTRUE);
}

/**
 * \fn BOOL LCD_OutFrame(const UINT8 frame[8][16])
 * \brief Writes whole text frame to the LCD
 * \param frame Text frame to be written to the LCD
 * \return bTRUE if the command was successful, otherwise FALSE
 * \warning Assumes LCD has been set up
 */
BOOL LCD_OutFrame(const UINT8 frame[LCD_TEXT_SIZE_Y][LCD_TEXT_SIZE_X]) 
{
  return SendCommand(LCD_CMD_SET_ADDRESS_POINTER, 2, 0, 0) && WriteAuto(LCD_TEXT_SIZE, (const char*)frame, bTRUE);
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
  
  if (!SendCommand(LCD_CMD_SET_ADDRESS_POINTER, 2, 0, 0))
  {    
    return bFALSE;
  }
  
  for (lineNb = 0; lineNb < LCD_TEXT_SIZE_Y; lineNb++)
  {
    success = LCD_OutString(LCD_BLANKLINE_STR);
    if (!success)
      return success;
  }
  return success;
}

/**
 * \fn BOOL LCD_SetContrast(const UINT8 contrast)
 * \brief Sets the contrast of the LCD
 * \param contrast a value between 0 and 63 (according to the MAX749 datasheet)
 * \return bTRUE if the contrast was in range, otherwise FALSE
 * \warning Assumes LCD contrast has started in shutdown mode
 */
BOOL LCD_SetContrast(const UINT8 contrast)
{
  UINT8 nbSteps, stepNb;
  
  if (contrast > 63)
    return bFALSE;
  
  /* start at mid-range */
  LCD_ADJ = 1;
  LCD_CTRL = 0;

  /* minimum delay is 100 ns */
  asm nop;
  asm nop;
  asm nop;

  /* set default signals for contrast adjustment */
  LCD_CTRL = 1;
  LCD_ADJ = 0;

  /* increment to the desired contrast */
  nbSteps = contrast + LCD_MAX_CONTRAST / 2;
  if (nbSteps >= LCD_MAX_CONTRAST)
  {
    nbSteps -= LCD_MAX_CONTRAST;
  }
  for (stepNb = 0; stepNb < nbSteps; stepNb++)
  {
    LCD_ADJ = 1;
    /* minimum high is 100 ns */
    asm nop;
    asm nop;
    asm nop;
    LCD_ADJ = 0;
    /* minimum low is 200 ns */
    asm nop;
    asm nop;
    asm nop;
    asm nop;
    asm nop;
  }
  return bTRUE;
}

/**
 * \fn void LCD_Backlight(const BOOL LEDBacklightOn)
 * \brief Sets the LED backlight of the LCD on or off
 * \param LEDBacklightOn  whether the LED backlight is on or off
 */
void LCD_Backlight(const BOOL LEDBacklightOn)
{
  LCD_LED = (UINT8)LEDBacklightOn;
}
