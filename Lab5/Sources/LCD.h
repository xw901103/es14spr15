// ----------------------------------------
// Filename: LCD.h
// Description: Routines to implement 
//   writing and reading from the graphical
//   LCD via PORTA and PORTB
// Author: PMcL
// Date: 08-Jun-12

#include "types.h"


// ----------------------------------------
// LCD_Setup
// 
// Sets up the graphical LCD
// Input:
//   none
// Output:
//   return bTRUE if the LCD was successfully set up, else bFALSE
// Conditions:
//   none

BOOL LCD_Setup(void);

// ----------------------------------------
// LCD_OutChar
// 
// Writes a character to the LCD and increments the address pointer
// Input:
//   data is a byte to be written to the LCD
// Output:
//   returns bTRUE if the command was successful, otherwise FALSE
// Conditions:
//   none

BOOL LCD_OutChar(const UINT8 data);

// ----------------------------------------
// LCD_OutString
// 
// Writes a string to the LCD
// Input:
//   str is a string to be written to the LCD
// Output:
//   returns bTRUE if the command was successful, otherwise FALSE
// Conditions:
//   none

BOOL LCD_OutString(const char *str);

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
