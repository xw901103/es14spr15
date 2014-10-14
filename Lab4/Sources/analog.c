/**
 * \file analog.c
 * \brief
 * \author Xu Waycell
 * \date 14-September-2014
 */
#include "analog.h"
#include "SPI.h"
#include "utils.h"
#include <mc9s12a512.h>

TUINT16 NbAnalogInputs;
TUINT16 NbAnalogOutputs;
TAnalogInput Analog_Input[NB_INPUT_CHANNELS];

#define ADC_OFFSET 0x0800

/**
 * \fn void Analog_Setup(const UINT32 busClk)
 * \brief Sets up the ADC and DAC
 * \param busClk the bus clock rate in Hz 
 */
void Analog_Setup(const UINT32 busClk) {
  TSPISetup spiSetup;
  
  spiSetup.isMaster = bTRUE;
  spiSetup.activeLowClock = bFALSE;
  spiSetup.evenEdgeClock = bTRUE;
  spiSetup.LSBFirst = bFALSE;
  spiSetup.baudRate = MATH_1_MEGA; // TODO: set it in config.h
  
  DDRH = DDRH | (DDRH_DDRH4_MASK | DDRH_DDRH5_MASK | DDRH_DDRH6_MASK);
  /* select SPICS3 */
  //PTH_PTH4 = 1;
  //PTH_PTH5 = 1;
  //PTH_PTH6 = 0;
  PTH = 0x70;
  SPI_Setup(&spiSetup, busClk);

  NbAnalogInputs.l = 0;
  NbAnalogOutputs.l = 0;
}

/**
 * \fn BOOL Analog_Get(const TAnalogChannel channelNb)
 * \brief Gets an analog input channel's value 
 * \param channelNb the number of the anlog input channel to read
 * \return a Boolean value indicating if the channel reading was changed
 * \warning Assumes that the ADC has been set up   
 */
BOOL Analog_Get(const TAnalogChannel channelNb) {
  UINT8 index = 0xFF, data1 = 0, data2 = 0, data3 = 0;
  TINT16 value;
  
  switch(channelNb) 
  {
    case ANALOG_INPUT_Ch1:
      index = 0;
      data1 = 0b00000110; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      data2 = 0b00000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    case ANALOG_INPUT_Ch2:
      index = 1;
      data1 = 0b00000110; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      data2 = 0b01000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    case ANALOG_INPUT_Ch3:
      index = 2;
      data1 = 0b00000110; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      data2 = 0b10000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    case ANALOG_INPUT_Ch4:
      index = 3;
      data1 = 0b00000110; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      data2 = 0b11000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    case ANALOG_INPUT_Ch5:
      index = 4;
      data1 = 0b00000111; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      data2 = 0b00000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    case ANALOG_INPUT_Ch6:
      index = 5;
      data1 = 0b00000111; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      data2 = 0b01000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    case ANALOG_INPUT_Ch7:
      index = 6;
      data1 = 0b00000111; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      data2 = 0b10000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    case ANALOG_INPUT_Ch8:
      index = 7;
      data1 = 0b00000111; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      data2 = 0b11000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    default:
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_INVALID_ARGUMENT);
#endif    
      return bFALSE;
      break;
  }
    
  PTH = 0x30;
  SPI_ExchangeChar(data1, &data1); /* X  | X  | X  | X  | X   | X   | X  | X  */
  SPI_ExchangeChar(data2, &data2); /* X  | X  | X  | 0  | B11 | B10 | B9 | B8 */
  SPI_ExchangeChar(data3, &data3); /* B7 | B6 | B5 | B4 | B3  | B2  | B1 | B0 */   
  PTH = 0x70;
  
  value.s.Hi = data2 & 0b00001111;
  value.s.Lo = data3;
  value.l = ADC_OFFSET - value.l;
      
  Analog_Input[index].Value3 = Analog_Input[index].Value2;  
  Analog_Input[index].Value2 = Analog_Input[index].Value1;  
  Analog_Input[index].Value1 = value.l;  
  Analog_Input[index].OldValue.l = Analog_Input[index].Value.l;
      
  Analog_Input[index].Value.l = FindMedianOfThreeNumbers(Analog_Input[index].Value1,
  															                         Analog_Input[index].Value2,
  															                         Analog_Input[index].Value3);
  
  return Analog_Input[index].Value.l != Analog_Input[index].OldValue.l;  
}
