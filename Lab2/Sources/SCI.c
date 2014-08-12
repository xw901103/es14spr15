/**
 * \author Xu Waycell 
 * \date 06-August-2014
 */
#include "SCI.h"
#include "FIFO.h"
#include <mc9s12a512.h>

static TFIFO RxFIFO, TxFIFO; /* no one can touch them except SCI_ calls */

void SCI_Setup(const UINT32 baudRate, const UINT32 busClk) {
     SCI0BD = (word)(busClk/baudRate/16); /* baud rate */
     SCI0CR1_PE = 0; /* no parity */
     SCI0CR1_M = 0;  /* 8 bits for data */
     SCI0CR2_RE = 1; /* receive enabled */
     SCI0CR2_TE = 1; /* transmit enabled */
     
     FIFO_Init(&RxFIFO); /* initialize receive buffer */    
     FIFO_Init(&TxFIFO); /* initialize transmit buffer */   
}

void SCI_Poll(void) {
    UINT8 temp = 0;
    
    if (SCI0SR1_RDRF) { /* check receive data register full flag */
        temp = SCI0DRL; /* read one byte from data register low */
        if (!FIFO_Put(&RxFIFO, temp)) { /* put it to receive buffer for later use */
#ifndef NO_DEBUG
            DEBUG(__LINE__, ERR_FIFO_PUT); /* generally, it should not be full. if it does, there is a design issue. */
#endif
        }                                    
    }
    if (SCI0SR1_TDRE) { /* check transmit data register empty flag */
        if (FIFO_Get(&TxFIFO, &temp)) { /* try to get one byte from transmit buffer */
            SCI0DRL = temp; /* set it to data register for hardware transmission process */           
        }
    }
    
}

BOOL SCI_InChar(UINT8 * const dataPtr) {
    if (dataPtr) {
        return FIFO_Get(&RxFIFO, dataPtr);   
    }
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif    
    return bFALSE;
}

BOOL SCI_OutChar(const UINT8 data) { /* simple wrap of transmit FIFO buffer */
    return FIFO_Put(&TxFIFO, data);
}
