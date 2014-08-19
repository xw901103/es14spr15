/**
 * \file defs.h
 * \brief Defines common macros
 * \author Xu Waycell
 * \date 06-August-2014
 */
#ifndef DEFS_H
#define DEFS_H

#if !defined(CONTROL_CR)
#define CONTROL_CR 0x0D /* control character carriage return */
#endif

#if !defined(MODCON_COMMAND_STARTUP) && !defined(MODCON_COMMAND_SPECIAL) && !defined(MODCON_COMMAND_NUMBER)
#define MODCON_COMMAND_STARTUP 0x04 /* ModCon protocol startup command */
#define MODCON_COMMAND_SPECIAL 0x09 /* ModCon protocol special command */
#define MODCON_COMMAND_NUMBER  0x0B /* ModCon protocol number command */
#endif

#if !defined(MODCON_VERSION_INITIAL) && !defined(MODCON_VERSION_TOKEN) && !defined(MODCON_VERSION_MAJOR) && !defined(MODCON_VERSION_MINOR)
#define MODCON_VERSION_INITIAL 'v'
#define MODCON_VERSION_TOKEN   'x' /* TODO: rename it for better functionality reflection */
#define MODCON_VERSION_MAJOR    1
#define MODCON_VERSION_MINOR    0
#endif

/* ModCon Number command parameter for get and set */
#if !defined(MODCON_NUMBER_GET) && !defined(MODCON_NUMBER_SET)
#define MODCON_NUMBER_GET 1
#define MODCON_NUMBER_SET 2
#endif

#if !defined(MODCON_COMMAND_ACK_MASK)
#define MODCON_COMMAND_ACK_MASK 0x80 /* ModCon protocol acknowledgement bitwise mask */
#endif

#if !defined(ERR_INVALID_POINTER) && !defined(ERR_FIFO_PUT) && !defined(ERR_PACKET_SETUP) && !defined(ERR_PACKET_PUT)
#define ERR_INVALID_POINTER 0xBAD1 /* given pointer is not valid */
#define ERR_FIFO_PUT        0xBAD2 /* cannot put a byte into FIFO buffer */
#define ERR_PACKET_SETUP    0xBAD3 /* packet initialization failure */
#define ERR_PACKET_PUT      0xBAD4 /* cannot put a packet into transmission queue */
#define ERR_BAD_FOOD        0xBADF /* 0xBAADF00D if you knew it, you are obsoleted */
#endif

#ifndef UNUSED
#define UNUSED(VAR) (void)VAR /* macro to avoid compiler issue */
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