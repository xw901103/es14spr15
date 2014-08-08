/**
 * \file defs.h
 * \brief Defines common macros
 * \author Xu Waycell
 * \date
 */
#ifndef DEFS_H
#define DEFS_H

#if !defined(MODCON_COMMAND_STARTUP) && !defined(MODCON_COMMAND_NUMBER) && !defined(MODCON_COMMAND_VERSION)
#define MODCON_COMMAND_STARTUP 0x04
#define MODCON_COMMAND_NUMBER  0x0B
#define MODCON_COMMAND_VERSION 0x09
#endif

#if !defined(MODCON_VERSION_INITIAL) && !defined(MODCON_VERSION_MAJOR) && !defined(MODCON_VERSION_MINOR)
#define MODCON_VERSION_INITIAL 'V'
#define MODCON_VERSION_MAJOR    1
#define MODCON_VERSION_MINOR    0
#endif

#if !defined(MODCON_NUMBER_GET) && !defined(MODCON_NUMBER_SET)
#define MODCON_NUMBER_GET 1
#define MODCON_NUMBER_SET 2
#endif

#if !defined(MODCON_COMMAND_ACK_MASK)
#define MODCON_COMMAND_ACK_MASK 0x80
#endif

#if !defined(ERR_INVALID_POINTER) && !defined(ERR_FIFO_PUT) && !defined(ERR_PACKET_SETUP) && !defined(ERR_PACKET_PUT)
#define ERR_INVALID_POINTER 0xBAD1
#define ERR_FIFO_PUT        0xBAD2
#define ERR_PACKET_SETUP    0xBAD3
#define ERR_PACKET_PUT      0xBAD4
#endif

#ifndef UNUSED /* macro to avoid compiler issues */
#define UNUSED(VAR) (void)VAR
#endif

#endif