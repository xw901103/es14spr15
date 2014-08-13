/**
 * \mainpage Xu's Embedded Software Lab 5
 *
 * \section intro_sec Introduction
 * Lab 2
 *
 * \file main.c
 * \brief Program main entry file.
 * \author Xu Waycell
 * \date 13-August-2014
 */
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
