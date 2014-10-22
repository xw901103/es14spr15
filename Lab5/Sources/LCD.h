/**
 * \file LCD.h
 * \brief Routines to implement writing and reading from the graphical LCD via PORTA and PORTB.
 * \author
 * \date
 */
#include "global.h"

/**
 * \fn BOOL LCD_Setup(void)
 * \brief Sets up the graphical LCD
 * \return bTRUE if the LCD was successfully set up, else bFALSE
 */
BOOL LCD_Setup(void);

/**
 * \fn BOOL LCD_OutChar(const UINT8 data)
 * \brief Writes a character to the LCD and increments the address pointer
 * \param data A byte to be written to the LCD
 * \return bTRUE if the command was successful, otherwise FALSE
 * \warning Assumes LCD has been set up
 */
BOOL LCD_OutChar(const UINT8 data);

/**
 * \fn BOOL LCD_OutString(const char *str)
 * \brief Writes a string to the LCD
 * \param str A string to be written to the LCD
 * \return bTRUE if the command was successful, otherwise FALSE
 * \warning Assumes LCD has been set up
 * \warning Given string has to end with \0
 */
BOOL LCD_OutString(const char *str);

BOOL LCD_OutFrame(const UINT8 frame[8][16]);

// ----------------------------------------
// LCD_Clear
// 
// Clears the LCD
// Input:
//   none
// Output:
//   returns bTRUE if the clear was successful, otherwise FALSE
// Conditions:
//   Assumes LCD has been set up

BOOL LCD_Clear(void);

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

BOOL LCD_SetContrast(const UINT8 contrast);

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

void LCD_Backlight(const BOOL LEDBacklightOn);
