/**
 * \author Xu Waycell
 * \date 05-August-2014
 */
#include "FIFO.h"

void FIFO_Init(TFIFO * const FIFO) 
{
  if (FIFO) 
  {
    /* ready given FIFO by setting zeros */
    FIFO->Start = 0;
    FIFO->End   = 0;
    FIFO->NbBytes = 0;
    return;
  }
#ifndef NO_DEBUG
  DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif
}

BOOL FIFO_Put(TFIFO * const FIFO, const UINT8 data) 
{
  if (FIFO) 
  {
    if (FIFO->NbBytes < FIFO_SIZE) 
    {          
      FIFO->Buffer[FIFO->End++] = data;
      if (FIFO->End >= FIFO_SIZE) /* once End reaches boundry, go back to zero */
        FIFO->End = 0;            
      FIFO->NbBytes++;
      return bTRUE;
    }
    return bFALSE;
  }
#ifndef NO_DEBUG
  DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif
  return bFALSE;
}

BOOL FIFO_Get(TFIFO * const FIFO, UINT8 * const dataPtr) 
{
  if (FIFO && dataPtr) 
  {
    if (FIFO->NbBytes != 0) 
    {          
      *dataPtr = FIFO->Buffer[FIFO->Start++];
      if (FIFO->Start >= FIFO_SIZE) /* once Start reaches boundry, go back to zero */
        FIFO->Start = 0;            
      FIFO->NbBytes--;
      return bTRUE;
    }
    return bFALSE;
  }
#ifndef NO_DEBUG
  DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif
  return bFALSE;
}
