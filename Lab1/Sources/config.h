/**
 * \file config.h
 * \brief Defines configurations
 * \author Xu Waycell
 * \date
 */
#ifndef CONFIG_H
#define CONFIG_H

#ifndef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE 38400
#else
#warning "Baudrate override detected!"
#endif

#ifndef CONFIG_BUSCLK
#define CONFIG_BUSCLK 8000000
#else
#warning "Bus clock override detected!"
#endif

#ifndef CONFIG_FIFO_SIZE
#define CONFIG_FIFO_SIZE 256
#else
#warning "General FIFO buffer size override detected!"
#endif

#endif
