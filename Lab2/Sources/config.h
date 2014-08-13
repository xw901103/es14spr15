/**
 * \file config.h
 * \brief Defines configurations
 * \author Xu Waycell
 * \date 06-August-2014
 */
#ifndef CONFIG_H
#define CONFIG_H

#ifndef CONFIG_OSCCLK
#define CONFIG_OSCCLK 16000000 /* Oscillator clock in hz */
#else
#warning "Oscillator clock override detected!"
#endif

#ifndef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE 115200          /* SCI baud rate in bits/sec */
#else
#warning "Baudrate override detected!"
#endif

#ifndef CONFIG_REFCLK
#define CONFIG_REFCLK 8000000           /* Reference clock in hz */
#else
#warning "Reference clock override detected!"
#endif

#ifndef CONFIG_BUSCLK
#define CONFIG_BUSCLK 24000000          /* Bus clock in hz */
#else
#warning "Bus clock override detected!"
#endif

#ifndef CONFIG_BUSCLK_MAXIMUM
#define CONFIG_BUSCLK_MAXIMUM 25000000  /* Maximum bus clock in hz */
#else
#warning "Maximum bus clock override detected!"
#endif

#ifndef CONFIG_FIFO_SIZE
#define CONFIG_FIFO_SIZE 256 /* Capacity of FIFO buffer */
#else
#warning "General FIFO buffer size override detected!"
#endif

#endif
