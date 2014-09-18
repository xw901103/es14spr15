/**
 * \file analog.c
 * \brief
 * \author Xu Waycell
 * \date 14-September-2014
 */
#include "analog.h"
#include "SPI.h"

TUINT16 NbAnalogInputs;
TUINT16 NbAnalogOutputs;
TAnalogInput Analog_Input[NB_INPUT_CHANNELS];

void Analog_Setup(const UINT32 busClk) {
  TSPISetup spiSetup;
  
  spiSetup.isMaster = bTRUE;
  spiSetup.activeLowClock = bFALSE;
  spiSetup.evenEdgeClock = bTRUE;
  spiSetup.LSBFirst = bFALSE;
  spiSetup.baudRate = MATH_1_MEGA;
  
  SPI_Setup(&spiSetup, busClk);
  NbAnalogInputs.l = 0;
  NbAnalogOutputs.l = 0;
}

BOOL Analog_Get(const UINT8 channelNb) {
  Analog_Input[channelNb].OldValue.l = 0;  
  Analog_Input[channelNb].Value.l = 0;  
  Analog_Input[channelNb].Value1 = 0;  
  Analog_Input[channelNb].Value2 = 0;  
  Analog_Input[channelNb].Value3 = 0;  
  return bFALSE;  
}
