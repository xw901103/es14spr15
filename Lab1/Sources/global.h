#ifndef GLOBAL_H
#define GLOBAL_H

#if !defined(MODCON_STARTUP) && !defined(MODCON_NUMBER) && !defined(MODCON_VERSION)
#define MODCON_STARTUP 0x04
#define MODCON_NUMBER  0x0B
#define MODCON_VERSION 0x09
#endif

/* quick fix */
#if !defined(MODCON_STARTUP_ACK) && !defined(MODCON_NUMBER_ACK) && !defined(MODCON_VERSION_ACK)
#define MODCON_STARTUP_ACK 0x84
#define MODCON_NUMBER_ACK  0x8B
#define MODCON_VERSION_ACK 0x89
#endif

#ifndef BAUDRATE
#define BAUDRATE 38400
#endif

#ifndef BUSCLK
#define BUSCLK 8000000
#endif

#endif