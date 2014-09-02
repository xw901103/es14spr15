/**
 * \file defs.h
 * \brief Defines common macros
 * \author Xu Waycell
 * \date 06-August-2014
 */
#ifndef DEFS_H
#define DEFS_H

#if !defined(MATH_1_KILO) && !defined(MATH_1_MEGA)
#define MATH_1_KILO 1000
#define MATH_1_MEGA 1000000
#endif

#if !defined(CONTROL_NUL) && !defined(CONTROL_BEL) && !defined(CONTROL_BS) && !defined(CONTROL_TAB) && !defined(CONTROL_LF) && !defined(CONTROL_VT) && !defined(CONTROL_FF) && !defined(CONTROL_CR)
#define CONTROL_NUL 0x00 /* control character null */
#define CONTROL_BEL 0x07 /* control character bell */
#define CONTROL_BS  0x08 /* control character backspace */
#define CONTROL_TAB 0x09 /* control character horizontal tab */
#define CONTROL_LF  0x0A /* control character line feed */
#define CONTROL_VT  0x0B /* control character vertical tab */
#define CONTROL_FF  0x0C /* control character form feed/page break */
#define CONTROL_CR  0x0D /* control character carriage return */
#endif

#if !defined(ERR_INVALID_POINTER) && !defined(ERR_FIFO_PUT) && !defined(ERR_PACKET_SETUP) && !defined(ERR_PACKET_PUT)
#define ERR_NO_ERROR        0x0000 /* all good */
#define ERR_INVALID_POINTER 0xBAD1 /* given pointer is not valid */
#define ERR_FIFO_PUT        0xBAD2 /* cannot put a byte into FIFO buffer */
#define ERR_PACKET_SETUP    0xBAD3 /* packet initialization failure */
#define ERR_PACKET_PUT      0xBAD4 /* cannot put a packet into transmission queue */
#define ERR_CRGPLL_SETUP    0xBAD5 /* clock and reset generator phase-locked loop initialization failure */
#define ERR_CRGCOP_SETUP    0xBAD6 /* clock and reset generator computer operating properly initialization failure */
#define ERR_EEPROM_SETUP    0xBAD7 /* EEPROM initialization failure */
#define ERR_EEPROM_WRITE    0xBAD8 /* EEPROM program failure */
#define ERR_CRITICAL        0xBADC /* critical error */
#define ERR_BAD_FOOD        0xBADF /* 0xBAADF00D if you knew it, you are obsoleted */
#endif

#ifndef UNUSED
#define UNUSED(VAR) (void)VAR /* macro to avoid compiler issue */
#endif

#if !defined(EEPROM_ADDRESS) && !defined(EEPROM_B) && !defined(EEPROM_I) && !defined(EEPROM_W) && !defined(EEPROM_L) && !defined(EEPROM_S)
#define EEPROM_ADDRESS(OFFSET) (OFFSET+0x0400)
#define EEPROM_B(OFFSET) *(UINT8 volatile *)EEPROM_ACCESS(OFFSET)
#define EEPROM_I(OFFSET) *(INT16 volatile *)EEPROM_ACCESS(OFFSET)
#define EEPROM_W(OFFSET) *(UINT16 volatile *)EEPROM_ACCESS(OFFSET)
#define EEPROM_L(OFFSET) *(INT32 volatile *)EEPROM_ACCESS(OFFSET)
#define EEPROM_S(OFFSET) *(UINT32 volatile *)EEPROM_ACCESS(OFFSET)
#endif

/**
 * \brief Macros for enter critical section
 */
#define EnterCritical() { asm tfr ccr,a; asm staa savedCCR; asm sei; }
/**
 * \brief Macros for exit critical section
 */
#define ExitCritical()  { asm ldaa savedCCR; asm tfr a,ccr; }

#endif