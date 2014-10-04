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

BOOL Analog_Get(const UINT8 channelNb) {
  UINT8 data1 = 0, data2 = 0, data3 = 0;
  TINT16 value;
    
  data1 = 0x06 | (0x01 & (channelNb >> 2)); /* 0 0 0 0 0 START SGL/DIFF D2 */
  data2 = 0xC0 & (channelNb << 6); 			/* D1 D0 X X X X X X */  
  
  PTH = 0x30;
  SPI_ExchangeChar(data1, &data1);
  SPI_ExchangeChar(data2, &data2);
  SPI_ExchangeChar(data3, &data3);    
  PTH = 0x70;
  
  value.s.Hi = data2 & 0x0F;
  value.s.Lo = data3;
  value.l = ADC_OFFSET - value.l;
      
  Analog_Input[channelNb].Value3 = Analog_Input[channelNb].Value2;  
  Analog_Input[channelNb].Value2 = Analog_Input[channelNb].Value1;  
  Analog_Input[channelNb].Value1 = value.l;  
  Analog_Input[channelNb].OldValue.l = Analog_Input[channelNb].Value.l;
      
  Analog_Input[channelNb].Value.l = FindMedianOfThreeNumbers(Analog_Input[channelNb].Value1,
  															 Analog_Input[channelNb].Value2,
  															 Analog_Input[channelNb].Value3);
  
  return Analog_Input[channelNb].Value.l != Analog_Input[channelNb].OldValue.l;  
}
