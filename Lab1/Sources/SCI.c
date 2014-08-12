/**
 * \author Xu Waycell 
 * \date
 */
#include "SCI.h"
#include "FIFO.h"
#include <mc9s12a512.h>

static TFIFO RxFIFO, TxFIFO; /* no one can touch them except SCI_ calls */

void SCI_Setup(const UINT32 baudRate, const UINT32 busClk) {
    /*
     * SCI0BD
     * SCI0CR1
     * SCI0CR2
     */
     SCI0BD = (word)(busClk/baudRate/16);
     SCI0CR1_PE = 0; /* no parity */
     SCI0CR1_M = 0; /* 8 bits for data */
     SCI0CR2_RE = 1;
     SCI0CR2_TE = 1; 
     
     FIFO_Init(&RxFIFO);    
     FIFO_Init(&TxFIFO);    
}

void SCI_Poll(void) {
    UINT8 temp = 0;
    
    if (SCI0SR1_RDRF) {
        temp = SCI0DRL;
        if (!FIFO_Put(&RxFIFO, temp)) { /* TODO: CHECK ERROR e.g. Start == End when NbBytes != 0 */
#ifndef NO_DEBUG
            DEBUG(__LINE__, ERR_FIFO_PUT);
#endif
        }                                    
    }
    if (SCI0SR1_TDRE) {
        if (FIFO_Get(&TxFIFO, &temp)) {
            SCI0DRL = temp;            
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

BOOL SCI_OutChar(const UINT8 data) { /* simple wrap for FIFO */
    return FIFO_Put(&TxFIFO, data);
}
