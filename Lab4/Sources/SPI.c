/**
 * \file SPI.c
 * \brief I/O routines for MC9S12 serial peripheral interface 
 * \author Xu Waycell
 * \date 14-September-2014
 */
#include "SPI.h"
#include <mc9s12a512.h>

void SPI_Setup(const TSPISetup * const aSPISetup, const UINT32 busClk)
{
  UINT16 divisor = 0;
  UINT8 spr = 0, sppr = 0;
  
  if (aSPISetup)
  {
    SPI0CR1_SPE = 0;

    divisor = (UINT16)(busClk / aSPISetup->baudRate);
    while (divisor % 2 == 0)
    {
      divisor /= 2;
      ++spr;
    }
    sppr = (UINT8)divisor;

    SPI0BR_SPR  = (byte)(spr - 1);
    SPI0BR_SPPR = (byte)(sppr - 1);
    
    SPI0CR1_SPIE  = 0;
    SPI0CR1_MSTR  = (byte)aSPISetup->isMaster;
    SPI0CR1_SPTIE = 0;
    SPI0CR1_CPOL  = (byte)aSPISetup->activeLowClock;
    SPI0CR1_CPHA  = (byte)aSPISetup->evenEdgeClock;
    SPI0CR1_SSOE  = 0;
    SPI0CR1_LSBFE = (byte)aSPISetup->LSBFirst;
    SPI0CR1_SPE   = 1;        
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
  /* send out */
  while(!SPI0SR_SPTEF);
  SPI0DR = dataTx;
  
  /* receive in */
  if (dataRx)
  {
  	while(!SPI0SR_SPIF);
    *dataRx = SPI0DR;
  }
}
