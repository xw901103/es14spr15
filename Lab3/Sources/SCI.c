/**
 * \author Xu Waycell 
 * \date 06-August-2014
 */
#include "SCI.h"
#include "FIFO.h"
#include <mc9s12a512.h>

static TFIFO RxFIFO, TxFIFO; /* no one can touch them except SCI_ calls */

void SCI_Setup(const UINT32 baudRate, const UINT32 busClk)
{
  SCI0BD     = (word)(busClk/baudRate/16); /* baud rate */
  
  /* SCI control register 1 */
  SCI0CR1_PT      = 0; /* Parity Type                   0=event                1=odd                        */
  SCI0CR1_PE      = 0; /* Parity Enable                 0=off                  1=on                         */
  SCI0CR1_ILT     = 0; /* Idle Line Type                0=idle after start bit 1=idle after stop bit        */
  SCI0CR1_WAKE    = 0; /* Wakeup Condition              0=idle line wakeup     1=address mark wakeup        */
  SCI0CR1_M       = 0; /* Data Format Mode              0=8bits data           1=9bits data                 */
  SCI0CR1_RSRC    = 0; /* Receiver Source               0=internal             1=external                   */
  SCI0CR1_SCISWAI = 0; /* SCI Stop in Wait Mode         0=off                  1=on                         */
  SCI0CR1_LOOPS   = 0; /* Loop Select                   0=normal               1=loop                       */
  
  /* SCI control register 2 */
  SCI0CR2_SCTIE = 0;   /* Tx Interrupt Eanbled          0=off                  1=on                         */
  SCI0CR2_TCIE  = 0;   /* Tx Complete Interrupt Enabled 0=off                  1=on                         */
  SCI0CR2_RIE   = 0;   /* Rx Full Interrupt Enabled     0=off                  1=on                         */
  SCI0CR2_ILIE  = 0;   /* Idle Line Interrupt Enabled   0=off                  1=on                         */
  SCI0CR2_TE    = 1;   /* Tx Enabled                    0=off                  1=on                         */
  SCI0CR2_RE    = 1;   /* Rx Enabled                    0=off                  1=on                         */
  SCI0CR2_RWU   = 0;   /* Rx Wakeup                     0=normal               1=enable the wakeup function */
  SCI0CR2_SBK   = 0;   /* Send Break Bit                0=off                  1=on                         */
     
  FIFO_Init(&RxFIFO); /* initialize receive buffer */    
  FIFO_Init(&TxFIFO); /* initialize transmit buffer */   
}

void SCI_Poll(void)
{
  UINT8 temp = 0;
    
  if (SCI0SR1_RDRF)
  { /* check receive data register full flag */
    temp = SCI0DRL; /* read one byte from data register low */
    if (!FIFO_Put(&RxFIFO, temp))
    { /* put it to receive buffer for later use */
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_FIFO_PUT); /* generally, it should not be full. if it does, there is a design issue. */
#endif
    }                                    
  }
  if (SCI0SR1_TDRE)
  {   /* check transmit data register empty flag */
    if (FIFO_Get(&TxFIFO, &temp))
    {   /* try to get one byte from transmit buffer */
      SCI0DRL = temp; /* set it to data register for hardware transmission process */           
    }
  }    
}

BOOL SCI_InChar(UINT8 * const dataPtr)
{
  if (dataPtr)
  {
    return FIFO_Get(&RxFIFO, dataPtr);   
  }
#ifndef NO_DEBUG
  DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif    
  return bFALSE;
}

BOOL SCI_OutChar(const UINT8 data)
{
  /* simple wrap of transmit FIFO buffer */
  return FIFO_Put(&TxFIFO, data);
}
