/**
 * \file analog.h
 * \brief Routines for setting up and reading from the ADC/DAC
 * \author Xu Waycell
 * \date 14-September-2014
 */
#ifndef ANALOG_H
#define ANALOG_H

#include "global.h"

/* maximum number of channels */
#define NB_INPUT_CHANNELS  8
#define NB_OUTPUT_CHANNELS 4

#ifndef CONFIG_SPI_BAUDRATE
#define SPI_BAUDRATE MATH_1_MEGA /* fallback plan */
#warning "SPI baudrate using fallback setting 1MHz"
#else
#define SPI_BAUDRATE CONFIG_SPI_BAUDRATE
#endif

typedef enum
{
  /* analog interface output channels */
  ANALOG_OUTPUT_Ch1 = 0x00,
  ANALOG_OUTPUT_Ch2 = 0x01,
  ANALOG_OUTPUT_Ch3 = 0x02,
  ANALOG_OUTPUT_Ch4 = 0x03,
  /* analog interface input channels */
  ANALOG_INPUT_Ch1  = 0x80,
  ANALOG_INPUT_Ch2  = 0x81,
  ANALOG_INPUT_Ch3  = 0x82,
  ANALOG_INPUT_Ch4  = 0x83,
  ANALOG_INPUT_Ch5  = 0x84,
  ANALOG_INPUT_Ch6  = 0x85,
  ANALOG_INPUT_Ch7  = 0x86,
  ANALOG_INPUT_Ch8  = 0x87
} TAnalogChannel;

typedef struct
{
  TINT16 Value, OldValue;
  INT16 Value1, Value2, Value3; /* variables for median filtering */
} TAnalogInput;

typedef struct
{
  TINT16 Value, OldValue;  
} TAnalogOutput;

extern TAnalogInput Analog_Input[NB_INPUT_CHANNELS];
extern TAnalogOutput Analog_Output[NB_OUTPUT_CHANNELS];

/**
 * \fn void Analog_Setup(const UINT32 busClk)
 * \brief Sets up the ADC and DAC
 * \param busClk the bus clock rate in Hz 
 */
void Analog_Setup(const UINT32 busClk);

/**
 * \fn BOOL Analog_Get(const TAnalogChannel channelNb)
 * \brief Gets an analog input channel's value 
 * \param channelNb the number of the analog input channel to read
 * \return a Boolean value indicating if the channel reading was changed
 * \warning Assumes that the ADC has been set up   
 */
BOOL Analog_Get(const TAnalogChannel channelNb);

/**
 * \fn void Analog_Put(const TAnalogChannel channelNb, INT16 value)
 * \brief Sets an analog output channel's value
 * \param channelNb the number of the analog output channel to write
 * \param value the value of the analog output to write
 * \warning Assumes that the DAC has been set up   
 */
void Analog_Put(const TAnalogChannel channelNb, INT16 value);

#endif
