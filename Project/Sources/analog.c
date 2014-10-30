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

TAnalogInput Analog_Input[NB_INPUT_CHANNELS] = { 0 };
TAnalogOutput Analog_Output[NB_OUTPUT_CHANNELS] = { 0 };

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
  spiSetup.baudRate = SPI_BAUDRATE;

  EnableSPI0CS();  
  SPI0CS = SPI0CS_NULL;
  SPI_Setup(&spiSetup, busClk);
}

/**
 * \fn BOOL Analog_Get(const TAnalogChannel channelNb)
 * \brief Gets an analog input channel's value 
 * \param channelNb the number of the anlog input channel to read
 * \return a Boolean value indicating if the channel reading was changed
 * \warning Assumes that the ADC has been set up   
 */
BOOL Analog_Get(const TAnalogChannel channelNb) {
  UINT8 index = 0xFF, cache1 = 0, cache2 = 0, cache3 = 0;
  TINT16 value;
  
  /* load hardware related data into caches for SPI exchanging */
  /* NOTE: channel enums might not be in numeric order         */
  switch(channelNb) 
  {
    case ANALOG_INPUT_Ch1:
      index = 0;
      cache1 = 0b00000110; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      cache2 = 0b00000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    case ANALOG_INPUT_Ch2:
      index = 1;
      cache1 = 0b00000110; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      cache2 = 0b01000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    case ANALOG_INPUT_Ch3:
      index = 2;
      cache1 = 0b00000110; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      cache2 = 0b10000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    case ANALOG_INPUT_Ch4:
      index = 3;
      cache1 = 0b00000110; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      cache2 = 0b11000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    case ANALOG_INPUT_Ch5:
      index = 4;
      cache1 = 0b00000111; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      cache2 = 0b00000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    case ANALOG_INPUT_Ch6:
      index = 5;
      cache1 = 0b00000111; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      cache2 = 0b01000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    case ANALOG_INPUT_Ch7:
      index = 6;
      cache1 = 0b00000111; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      cache2 = 0b10000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    case ANALOG_INPUT_Ch8:
      index = 7;
      cache1 = 0b00000111; /* 0  | 0  | 0 | 0 | 0 | START | SGL/DIFF | D2 */
      cache2 = 0b11000000; /* D1 | D0 | X | X | X | X     | X        | X  */
      break;
    default:
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_INVALID_ARGUMENT);
#endif    
      return bFALSE;
      break;
  }
    
  SPI0CS = SPI0CS_ADC;     /* select ADC chip as our listener */
  SPI_ExchangeChar(cache1, &cache1); /* X  | X  | X  | X  | X   | X   | X  | X  */
  SPI_ExchangeChar(cache2, &cache2); /* X  | X  | X  | 0  | B11 | B10 | B9 | B8 */
  SPI_ExchangeChar(cache3, &cache3); /* B7 | B6 | B5 | B4 | B3  | B2  | B1 | B0 */   
  SPI0CS = SPI0CS_NULL;    /* deselect any chip */
  
  value.s.Hi = cache2 & 0b00001111;
  value.s.Lo = cache3;
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

/**
 * \fn void Analog_Set(const TAnalogChannel channelNb, INT16 value)
 * \brief Sets an analog output channel's value
 * \param channelNb the number of the analog output channel to write
 * \param value the value of the analog output to write
 * \warning Assumes that the DAC has been set up   
 */
void Analog_Set(const TAnalogChannel channelNb, INT16 value) {
  UINT8 index = 0xFF, cache1 = 0, cache2 = 0;
  TINT16 cache;
  
  /* load hardware related data into caches for SPI exchanging */
  /* NOTE: channel enums might not be in numeric order         */
  switch(channelNb) 
  {
    case ANALOG_OUTPUT_Ch1:
      index = 0;
      cache1 = 0b00100000; /* A1 | A0 | /PD | /LDAC | D11 | D10 | D9 | D8 */
      cache2 = 0b00000000; /* D7 | D6 | D5  | D4    | D3  | D2  | D1 | D0 */
      break;
    case ANALOG_OUTPUT_Ch2:
      index = 1;
      cache1 = 0b00100000; /* A1 | A0 | /PD | /LDAC | D11 | D10 | D9 | D8 */
      cache2 = 0b00000000; /* D7 | D6 | D5  | D4    | D3  | D2  | D1 | D0 */
      break;
    case ANALOG_OUTPUT_Ch3:
      index = 2;
      cache1 = 0b10100000; /* A1 | A0 | /PD | /LDAC | D11 | D10 | D9 | D8 */
      cache2 = 0b00000000; /* D7 | D6 | D5  | D4    | D3  | D2  | D1 | D0 */
      break;
    case ANALOG_OUTPUT_Ch4:
      index = 3;
      cache1 = 0b11100000; /* A1 | A0 | /PD | /LDAC | D11 | D10 | D9 | D8 */
      cache2 = 0b00000000; /* D7 | D6 | D5  | D4    | D3  | D2  | D1 | D0 */
      break;
    default:
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_INVALID_ARGUMENT);
#endif    
      return;
      break;
  }
  
  cache.l = value;
  
  cache1 = cache1 | (cache.s.Hi & 0b00001111);
  cache2 = cache.s.Lo;
    
  SPI0CS = SPI0CS_DAC;     /* select DAC chip as our listener */
  SPI_ExchangeChar(cache1, &cache1); /* A1 | A0 | /PD | /LDAC | D11 | D10 | D9 | D8 */
  SPI_ExchangeChar(cache2, &cache2); /* D7 | D6 | D5  | D4    | D3  | D2  | D1 | D0 */
  SPI0CS = SPI0CS_NULL;    /* deselect any chip */
        
  Analog_Output[index].OldValue.l = Analog_Output[index].Value.l;
      
  Analog_Output[index].Value.l = value;
}

