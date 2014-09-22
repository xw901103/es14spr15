/**
 * \file analog.c
 * \brief
 * \author Xu Waycell
 * \date 14-September-2014
 */
#include "analog.h"
#include "SPI.h"
#include <mc9s12a512.h>

TUINT16 NbAnalogInputs;
TUINT16 NbAnalogOutputs;
TAnalogInput Analog_Input[NB_INPUT_CHANNELS];

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
  PTH = 0x00;
  SPI_Setup(&spiSetup, busClk);

  NbAnalogInputs.l = 0;
  NbAnalogOutputs.l = 0;
}

BOOL Analog_Get(const UINT8 channelNb) {
  UINT8 data = 0x80 | (channelNb<<4);
  
  PTH = 0x30;
  SPI_ExchangeChar(data, &data);  
  PTH = 0x00;
      
  Analog_Input[channelNb].Value2 = Analog_Input[channelNb].Value1;  
  Analog_Input[channelNb].Value3 = Analog_Input[channelNb].Value2;  
  Analog_Input[channelNb].Value1 = data;  
  Analog_Input[channelNb].OldValue.l = Analog_Input[channelNb].Value.l;  
  Analog_Input[channelNb].Value.l = data;  
  return bFALSE;  
}
