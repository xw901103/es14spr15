// ----------------------------------------
// Filename: main.c
// Description: Lab 5
//  Example LCD interface
// Author: PMcL
// Date: 29-Oct-12

#include <hidef.h>          /* common defines and macros */
#include <mc9s12a512.h>     /* derivative information */
		 
#pragma LINK_INFO DERIVATIVE "mc9s12a512"

// new types
#include "types.h"

// Graphical LCD
#include "LCD.h"

// ----------------------------------------
// main
//
// Writes "Hello world!" to the LCD

void main(void)
{
  (void)LCD_Setup();
  (void)LCD_OutString("Hello world!");
  for (;;);
}
