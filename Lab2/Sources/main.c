// ----------------------------------------
// Filename: main.c
// Description: Lab 2
//  Implements the ModCon serial protocol
//  Implements EEPROM write support and PLL
// Author: PMcL
// Date: 16-Mar-06

#include <hidef.h>          /* common defines and macros */
#include <mc9s12a512.h>     /* derivative information */
		 
#pragma LINK_INFO DERIVATIVE "mc9s12a512"

// new types
#include "types.h"

// Serial Communication Interface
#include "SCI.h"

// Packet handling
#include "packet.h"

// EEPROM
#include "EEPROM.h"

// Clocks and Reset Generator
#include "CRG.h"

// Version number
const UINT8 MAJOR_VERSION = 1;
const UINT8 MINOR_VERSION = 0;

// ----------------------------------------
// Packet_Commands

const UINT8 CMD_STARTUP     = 0x04;
const UINT8 CMD_EEPROM_PROG	= 0x07;
const UINT8 CMD_EEPROM_GET  = 0x08;
const UINT8 CMD_SPECIAL     = 0x09;
const UINT8 CMD_MODCON_NB   = 0x0b;
const UINT8 CMD_MODCON_MODE = 0x0d;

// ----------------------------------------
// main
//
// Initializes the various peripherals
// Handles received packets endlessly

void main(void)
{
  // Your setup code goe here
  for (;;)
  {
    // Your background loop goes here
  }
}