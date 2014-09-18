/**
 * \file SPI.h
 * \brief I/O routines for MC9S12 serial peripheral interface 
 * \author Xu Waycell
 * \date 14-September-2014
 */
#ifndef SPI_H
#define SPI_H

#include "global.h"

typedef struct
{
  BOOL isMaster;
  BOOL activeLowClock;
  BOOL evenEdgeClock;
  BOOL LSBFirst;
  UINT32 baudRate;
} TSPISetup;

// ----------------------------------------
// SPI_Setup
// 
// Sets up the Serial Peripheral Interface
// Input:
//   aSPISetup is a structure containing the parameters to 
//     be used in setting up the SPI:
//       isMaster is a Boolean value indicating whether the SPI is master or slave
//       activeLowClocks is a Boolean value indicating whether the clock is active
//         low or active high
//       evenEdgeClockPhase is a Boolean value indicating whether the data is clocked
//         on even or odd edges
//       LSBFirst is a Boolean value indicating whether the data is transferred LSB
//         first or MSB first
//       baudRate is the baud rate in bits/sec of the SPI clock
//   busClk is the bus clock rate in Hz
// Output:
//   none
// Conditions:
//   none
// ----------------------------------------
/**
 * \fn void SPI_Setup(const TSPISetup * const aSPISetup, const UINT32 busClk)
 * \brief
 * \param aSPISetup
 * \param busClk the bus clock rate in Hz
 */
void SPI_Setup(const TSPISetup * const aSPISetup, const UINT32 busClk);
 
/**
 * \fn void SPI_ExchangeChar(const UINT8 dataTx, UINT8 * const dataRx)
 * \param dataTx A byte to transmit
 * \param dataRx A pointer to a byte to receive
 * \warning Assumes SPI has been set up 
 */
void SPI_ExchangeChar(const UINT8 dataTx, UINT8 * const dataRx);

#endif
