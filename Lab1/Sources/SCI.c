#include "SCI.h"
#include "FIFO.h"
#include <mc9s12a512.h>

TFIFO RxFIFO, TxFIFO;

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

void SCI_Poll() {
    UINT8 Cache = 0;
    
    if (SCI0SR1_RDRF) {
        Cache = SCI0DRL;
        if (!FIFO_Put(&RxFIFO, Cache)) { /* TODO: CHECK ERROR e.g. Start == End when NbBytes != 0 */
        }                                    
    }
    if (SCI0SR1_TDRE) {
        if (FIFO_Get(&TxFIFO, &Cache)) {
            SCI0DRL = Cache;            
        }
    }
    
}

BOOL SCI_InChar(UINT8 * const dataPtr) {
    if (dataPtr) {
        return FIFO_Get(&RxFIFO, dataPtr);   
    }
    return bFALSE;
}

BOOL SCI_OutChar(const UINT8 data) { /* simple wrap for FIFO */
    return FIFO_Put(&TxFIFO, data);
}
