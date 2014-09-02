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

#ifndef CONFIG_COPRATE
#define CONFIG_COPRATE COP_RATE_2_14    /* Predefined COP rate */
#else
#warning "COP rate override detected!"
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


#ifndef CONFIG_EEPROM_ADDRESS_MODCON_NUMBER
#define CONFIG_EEPROM_ADDRESS_MODCON_NUMBER  0x0400 /* 16-bits ModCon number EEPROM address */
#else
#warning "ModCon number EEPROM address override detected!"
#endif

#ifndef CONFIG_EEPROM_ADDRESS_MODCON_MODE
#define CONFIG_EEPROM_ADDRESS_MODCON_MODE  0x0402 /* 16-bits ModCon mode EEPROM address */
#else
#warning "ModCon mode EEPROM address override detected!"
#endif

#ifndef CONFIG_EEPROM_ADDRESS_BEGIN
#define CONFIG_EEPROM_ADDRESS_BEGIN 0x0400 /* Begin boundary of EEPROM */
#else
#warning "EEPROM address begin boundary override detected!"
#endif

#ifndef CONFIG_EEPROM_ADDRESS_END
#define CONFIG_EEPROM_ADDRESS_END 0x07FF /* End boundary of EEPROM */
#else
#warning "EEPROM address end boundary override detected!"
#endif

#ifndef CONFIG_MODCON_EEPROM_ADDRESS_BEGIN
#define CONFIG_MODCON_EEPROM_ADDRESS_BEGIN 0x0400 /* Acceptable ModCon EEPROM begin boundary */
#else
#warning "ModCon acceptable EEPROM address begin boundary override detected!"
#endif

#ifndef CONFIG_MODCON_EEPROM_ADDRESS_END
#define CONFIG_MODCON_EEPROM_ADDRESS_END 0x1000 /* Acceptable ModCon EEPROM end boundary */
#else
#warning "ModCon acceptable EEPROM address end boundary override detected!"
#endif

#endif
