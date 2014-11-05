/**
 * \file config.h
 * \brief Defines configurations
 * \author Xu Waycell
 * \date 06-August-2014
 */
#ifndef CONFIG_H
#define CONFIG_H

/* uncomment to disable */
//#define NO_DEBUG     /* disable debug */
//#define NO_INTERRUPT /* disable interrupts NOTE: some modules heavily depend on it. */

#ifndef CONFIG_OSCCLK
#define CONFIG_OSCCLK 16000000 /* Oscillator clock in hz */
#else
#warning "Oscillator clock override detected!"
#endif

#ifndef CONFIG_RTI_PRESCALERATE
#define CONFIG_RTI_PRESCALERATE 0x07    /* CRG real-time clock prescale rate */
#else
#warning "Real time interrupt prescale rate override detected!"
#endif

#ifndef CONFIG_RTI_MODULUSCOUNT
#define CONFIG_RTI_MODULUSCOUNT 0x0F    /* CRG real-time clock modulus count */
#else
#warning "Real time interrupt modulus count override detected!"
#endif

#ifndef CONFIG_SCI_BAUDRATE
#define CONFIG_SCI_BAUDRATE 115200      /* SCI baud rate in bits/sec */
#else
#warning "SCI baudrate override detected!"
#endif

#ifndef CONFIG_SPI_BAUDRATE
#define CONFIG_SPI_BAUDRATE 1000000     /* SPI baud rate in bits/sec */
#else
#warning "SPI baudrate override detected!"
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

/* RESERVED BEGIN */
#ifndef CONFIG_TIMER_PERIOD
#define CONFIG_TIMER_PERIOD 10000          /* Timer period in microseconds */
#else
#warning "Timer period override detected!"
#endif
/* RESERVED END */

#ifndef CONFIG_COP_RATE
#define CONFIG_COP_RATE COP_RATE_2_14    /* Predefined COP rate */
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
#define CONFIG_EEPROM_ADDRESS_MODCON_NUMBER 0x0400 /* 16-bits ModCon number EEPROM address */
#else
#warning "ModCon number EEPROM address override detected!"
#endif

#ifndef CONFIG_EEPROM_ADDRESS_MODCON_MODE
#define CONFIG_EEPROM_ADDRESS_MODCON_MODE 0x0402 /* 16-bits ModCon mode EEPROM address */
#else
#warning "ModCon mode EEPROM address override detected!"
#endif

#ifndef CONFIG_EEPROM_ADDRESS_MODCON_PROTOCOL_MODE
#define CONFIG_EEPROM_ADDRESS_MODCON_PROTOCOL_MODE 0x0404 /* 16-bits ModCon protocol mode EEPROM address */
#else
#warning "ModCon control mode EEPROM address override detected!"
#endif

#ifndef CONFIG_EEPROM_ADDRESS_MODCON_ANALOG_INPUT_CHANNEL_SWITCH
#define CONFIG_EEPROM_ADDRESS_MODCON_ANALOG_INPUT_CHANNEL_SWITCH 0x0406 /* 16-bits Analog inputs number EEPROM address */
#else
#warning "ModCon analog input channel switch EEPROM address override detected!"
#endif

#ifndef CONFIG_EEPROM_ADDRESS_MODCON_ANALOG_OUTPUT_CHANNEL_SWITCH
#define CONFIG_EEPROM_ADDRESS_MODCON_ANALOG_OUTPUT_CHANNEL_SWITCH 0x0408 /* 16-bits Analog outputs number EEPROM address */
#else
#warning "ModCon analog output channel switch EEPROM address override detected!"
#endif

#ifndef CONFIG_EEPROM_ADDRESS_MODCON_ANALOG_INPUT_SAMPLING_RATE
#define CONFIG_EEPROM_ADDRESS_MODCON_ANALOG_INPUT_SAMPLING_RATE 0x0410 /* 16-bits Analog period EEPROM address */
#else
#warning "ModCon analog input sampling rate EEPROM address override detected!"
#endif

/* RESERVED BEGIN */
#ifndef CONFIG_EEPROM_ADDRESS_PACKET_PERIOD
#define CONFIG_EEPROM_ADDRESS_PACKET_PERIOD 0x0412 /* 16-bits Packet period EEPROM address */
#else
#warning "Packet period EEPROM address override detected!"
#endif
/* RESERVED END */

#ifndef CONFIG_EEPROM_ADDRESS_MODCON_DEBUG
#define CONFIG_EEPROM_ADDRESS_MODCON_DEBUG 0x0420 /* 16-bits ModCon debug EEPROM address */
#else
#warning "ModCon debug EEPROM address override detected!"
#endif

#ifndef CONFIG_EEPROM_ADDRESS_RTI_DEBUG
#define CONFIG_EEPROM_ADDRESS_RTI_DEBUG CONFIG_EEPROM_ADDRESS_MODCON_DEBUG /* 16-bits ModCon debug EEPROM address */
#else
#warning "RTI debug EEPROM address override detected!"
#endif

#ifndef CONFIG_EEPROM_ADDRESS_MODULUS_DOWN_COUNTER_DEBUG
#define CONFIG_EEPROM_ADDRESS_MODULUS_DOWN_COUNTER_DEBUG CONFIG_EEPROM_ADDRESS_MODCON_DEBUG /* 16-bits ModCon debug EEPROM address */
#else
#warning "Modulus down counter debug EEPROM address override detected!"
#endif

#ifndef CONFIG_EEPROM_ADDRESS_TIMER_CH7_DEBUG
#define CONFIG_EEPROM_ADDRESS_TIMER_CH7_DEBUG CONFIG_EEPROM_ADDRESS_MODCON_DEBUG /* 16-bits ModCon debug EEPROM address */
#else
#warning "Timer channel 7 debug EEPROM address override detected!"
#endif

/* RESERVED BEGIN */
#ifndef CONFIG_EEPROM_ADDRESS_EEPROM_ERASED_FLAG
#define CONFIG_EEPROM_ADDRESS_EEPROM_ERASED_FLAG 0x0430 /* 16-bits EEPROM erased EEPROM address */
#else
#warning "EEPROM erased flag EEPROM address override detected!"
#endif
/* RESERVED END */

#ifndef CONFIG_EEPROM_ADDRESS_HMI_BACKLIGHT
#define CONFIG_EEPROM_ADDRESS_HMI_BACKLIGHT 0x0700 /* 16-bits HMI backlight EEPROM address */
#else
#warning "HMI backlight EEPROM address override detected!"
#endif

#ifndef CONFIG_EEPROM_ADDRESS_HMI_CONTRAST
#define CONFIG_EEPROM_ADDRESS_HMI_CONTRAST 0x0702 /* 16-bits HMI contrast EEPROM address */
#else
#warning "HMI contrast EEPROM address override detected!"
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
