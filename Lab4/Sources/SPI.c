/**
 * \file SPI.c
 * \brief I/O routines for MC9S12 serial peripheral interface 
 * \author Xu Waycell
 * \date 14-September-2014
 */
#include "SPI.h"
#include <mc9s12a512.h>

/**
 * \fn void SPI_Setup(const TSPISetup * const aSPISetup, const UINT32 busClk)
 * \brief Sets up the Serial Peripheral Interface
 * \param aSPISetup a structure containing the parameters to be used in setting up the SPI:
 * - TSPISetup
 * |- isMaster is a Boolean value indicating whether the SPI is master or slave
 * |- activeLowClocks is a Boolean value indicating whether the clock is active low or active high
 * |- evenEdgeClockPhase is a Boolean value indicating whether the data is clocked on even or odd edges
 * |- LSBFirst is a Boolean value indicating whether the data is transferred LSB first or MSB first
 * |- baudRate the baud rate in bits/sec of the SPI clock
 * \param busClk the bus clock rate in Hz
 */
void SPI_Setup(const TSPISetup * const aSPISetup, const UINT32 busClk)
{
  UINT16 divisor = 0;
  UINT8 spr = 0, sppr = 0;
  
  if (aSPISetup)
  {
    SPI0CR1_SPE = 0; /* SPI System Enable 1= on 0= off */

    divisor = (UINT16)(busClk / aSPISetup->baudRate);
    while (divisor % 2 == 0)
    {
      divisor /= 2;
      ++spr;
    }
    sppr = (UINT8)divisor;

    SPI0BR_SPR  = (byte)(spr - 1);
    SPI0BR_SPPR = (byte)(sppr - 1);
    
    SPI0CR1_SPIE  = 0;                               /* SPI Interrupt Enable          1= on     0= off   */
    SPI0CR1_MSTR  = (byte)aSPISetup->isMaster;       /* SPI Master/Slave Mode Select  1= master 0= slave */
    SPI0CR1_SPTIE = 0;                               /* SPI Transmit Interrupt Enable 1= on     0= off   */
    SPI0CR1_CPOL  = (byte)aSPISetup->activeLowClock; /* SPI Clock Polarity            1= low    0= high  */
    SPI0CR1_CPHA  = (byte)aSPISetup->evenEdgeClock;  /* SPI Clock Phase               1= even   0= odd   */
    SPI0CR1_SSOE  = 0;                               /* Slave Select Output Enable    1= on     0= off   */
    SPI0CR1_LSBFE = (byte)aSPISetup->LSBFirst;       /* SPI LSB-First Enable          1= on     0= off   */
    SPI0CR1_SPE   = 1;                               /* SPI System Enable             1= on     0= off   */
    
    //SPI0CR2_SPC0    = 0; /* Serial Pin Control 0                                 1= on 0= off */
    //SPI0CR2_SPISWAI = 0; /* SPI Stop in Wait Mode                                1= on 0= off */               
    //SPI0CR2_BIDIROE = 0; /* Output enable in the Bidirectional mode of operation 1= on 0= off */               
    //SPI0CR2_MODFEN  = 0; /* Mode Fault Enable                                    1= on 0= off */ 
  }
  else
  {
#ifndef NO_DEBUG
  DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif
  }
}

/**
 * \fn void SPI_ExchangeChar(const UINT8 dataTx, UINT8 * const dataRx)
 * \param dataTx A byte to transmit
 * \param dataRx A pointer to a byte to receive
 * \warning Assumes SPI has been set up 
 */
void SPI_ExchangeChar(const UINT8 dataTx, UINT8 * const dataRx)
{
  /* send byte out */
  while(!SPI0SR_SPTEF);
  SPI0DR = dataTx;
  
  /* receive in byte */
  if (dataRx)
  {
  	while(!SPI0SR_SPIF);
    *dataRx = SPI0DR;
  }
}
