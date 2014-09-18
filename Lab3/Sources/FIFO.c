/**
 * \file FIFO.c
 * \brief Routines to implement a first in first out byte buffer. 
 * \author Xu Waycell
 * \date 05-August-2014
 */
#include "FIFO.h"

/**
 * \fn void FIFO_Init(TFIFO * const FIFO)
 * \brief Initialize the FIFO.
 * \param FIFO a pointer to a FIFO struct to initialize
 */
void FIFO_Init(TFIFO * const FIFO) 
{
  UINT8 savedCCR;
  if (FIFO) 
  {
    /* ready given FIFO by setting zeros */
    EnterCritical();
    FIFO->Start = 0;
    FIFO->End   = 0;
    FIFO->NbBytes = 0;
    ExitCritical();
    return;
  }
#ifndef NO_DEBUG
  DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif
}

/**
 * \fn BOOL FIFO_Put(TFIFO * const FIFO, const UINT8 data)
 * \brief Enter one character into the FIFO.
 * \param FIFO a  pointer to a FIFO struct where data is to be stored
 * \param data a byte of data to store in the FIFO buffer
 * \return TRUE if data is properly saved
 * \warning Assumes that FIFO_Init has been called.
 */
BOOL FIFO_Put(TFIFO * const FIFO, const UINT8 data) 
{
  UINT8 savedCCR;
  if (FIFO) 
  {
    if (FIFO->NbBytes < FIFO_SIZE) 
    {
      EnterCritical();          
      FIFO->Buffer[FIFO->End++] = data;
      if (FIFO->End >= FIFO_SIZE) /* once End reaches boundry, go back to zero */
        FIFO->End = 0;            
      FIFO->NbBytes++;
      ExitCritical();
      return bTRUE;
    }
    return bFALSE;
  }
#ifndef NO_DEBUG
  DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif
  return bFALSE;
}

/**
 * \fn BOOL FIFO_Get(TFIFO * const FIFO, UINT8 * const dataPtr)
 * \brief Remove one character from the FIFO.
 * \param FIFO a  pointer to a FIFO struct with data to be retrieved
 * \param dataPtr a pointer to a memory location to place the retrieved byte
 * \return TRUE if the operation was successful and the data is valid
 * \warning Assumes that FIFO_Init has been called.
 */
BOOL FIFO_Get(TFIFO * const FIFO, UINT8 * const dataPtr) 
{
  UINT8 savedCCR;
  if (FIFO && dataPtr) 
  {
    if (FIFO->NbBytes != 0) 
    {          
      EnterCritical();          
      *dataPtr = FIFO->Buffer[FIFO->Start++];
      if (FIFO->Start >= FIFO_SIZE) /* once Start reaches boundry, go back to zero */
        FIFO->Start = 0;            
      FIFO->NbBytes--;
      ExitCritical();          
      return bTRUE;
    }
    return bFALSE;
  }
#ifndef NO_DEBUG
  DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif
  return bFALSE;
}
