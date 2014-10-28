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

/**
 * \fn BOOL LCD_OutFrame(const UINT8 frame[8][16])
 * \brief Writes whole text frame to the LCD
 * \param frame Text frame to be written to the LCD
 * \return bTRUE if the command was successful, otherwise FALSE
 * \warning Assumes LCD has been set up
 */
BOOL LCD_OutFrame(const UINT8 frame[8][16]); /* TODO: use dfines instead of this fixed value */

/**
 * \fn BOOL LCD_Clear(void)
 * \brief Clears the LCD
 * \return returns bTRUE if the clear was successful, otherwise FALSE
 * \warning Assumes LCD has been set up
 */
BOOL LCD_Clear(void);

/**
 * \fn BOOL LCD_SetContrast(const UINT8 contrast)
 * \brief Sets the contrast of the LCD
 * \param contrast a value between 0 and 63 (according to the MAX749 datasheet)
 * \return bTRUE if the contrast was in range, otherwise FALSE
 * \warning Assumes LCD contrast has started in shutdown mode
 */
BOOL LCD_SetContrast(const UINT8 contrast);

/**
 * \fn void LCD_Backlight(const BOOL LEDBacklightOn)
 * \brief Sets the LED backlight of the LCD on or off
 * \param LEDBacklightOn  whether the LED backlight is on or off
 */
void LCD_Backlight(const BOOL LEDBacklightOn);
