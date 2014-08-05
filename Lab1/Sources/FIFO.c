#include "FIFO.h"

void FIFO_Init(TFIFO * const FIFO) {
    if (FIFO) {
        FIFO->Start = 0;
        FIFO->End = 0;
        FIFO->NbBytes = 0;
    }
}

BOOL FIFO_Put(TFIFO * const FIFO, const UINT8 data) {
    if (FIFO) { /* TODO: ERROR CHECK e.g. Start == End when NbBytes != 0 */
        FIFO->Buffer[FIFO->End++] = data;
        ++FIFO->NbBytes;
        return bTRUE;
    }
    return bFALSE;
}

BOOL FIFO_Get(TFIFO * const FIFO, UINT8 * const dataPtr) {
    if (FIFO && dataPtr && FIFO->NbBytes != 0) { /* TODO: ERROR CHECK e.g. NbBytes == 0 */
        *dataPtr = FIFO->Buffer[FIFO->Start++];
        --FIFO->NbBytes;
        return bTRUE;
    }
    return bFALSE;
}