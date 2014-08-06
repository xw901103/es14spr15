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

#if !defined(MODCON_COMMAND_ACK_MASK)
#define MODCON_COMMAND_ACK_MASK 0x80
#endif

/* quick fix */
#if !defined(MODCON_STARTUP_ACK) && !defined(MODCON_NUMBER_ACK) && !defined(MODCON_VERSION_ACK)
#define MODCON_STARTUP_ACK 0x84
#define MODCON_NUMBER_ACK  0x8B
#define MODCON_VERSION_ACK 0x89
#endif

#ifndef BAUDRATE
#define BAUDRATE 38400
#else
#warning "Baudrate override detected!"
#endif

#ifndef BUSCLK
#define BUSCLK 8000000
#else
#warning "Bus clock override detected!"
#endif

#endif