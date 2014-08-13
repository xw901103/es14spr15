// ----------------------------------------
// Filename: SPI.h
// Description: I/O routines for MC9S12 serial
//   peripheral interface 
// Author: PMcL
// Date: 13-Feb-07

#ifndef SPI_H
#define SPI_H

// new types
#include "types.h"

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

void SPI_Setup(const TSPISetup * const aSPISetup, const UINT32 busClk);
 
// ----------------------------------------
// SPI_ExchangeChar
//
// Transmits a byte and retrieves a received byte from the SPI
// Input:
//   dataTx is a byte to transmit
//   dataRx is a pointer to a byte to receive
// Output:
//   none
// Conditions:
//   Assumes SPI has been set up

void SPI_ExchangeChar(const UINT8 dataTx, UINT8 * const dataRx);

#endif