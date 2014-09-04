/**
 * \file FIFO.h
 * \brief Routines to implement a first in first out byte buffer. 
 * \author Xu Waycell
 * \date 05-August-2014
 */
#ifndef FIFO_H
#define FIFO_H

#include "global.h"

/**
 * FIFO buffer capacity
 */
#ifndef CONFIG_FIFO_SIZE
#define FIFO_SIZE 256 /* fallback plan */
#warning "FIFO_SIZE using fallback setting 256"
#else
#define FIFO_SIZE CONFIG_FIFO_SIZE
#endif

/**
 * \brief FIFO buffer structure
 */
typedef struct
{
  UINT16 Start, End;        /* \brief boundary and iterator, it should be from 0 to FIFO_SIZE - 1. */
  UINT16 volatile NbBytes;  /* \brief count of bytes inside the buffer */
  UINT8 Buffer[FIFO_SIZE];  /* \brief byte array serving as container */
} TFIFO;

/**
 * \fn void FIFO_Init(TFIFO * const FIFO)
 * \brief Initialize the FIFO.
 * \param FIFO a pointer to a FIFO struct to initialize
 */
void FIFO_Init(TFIFO * const FIFO);

/**
 * \fn BOOL FIFO_Put(TFIFO * const FIFO, const UINT8 data)
 * \brief Enter one character into the FIFO.
 * \param FIFO a  pointer to a FIFO struct where data is to be stored
 * \param data a byte of data to store in the FIFO buffer
 * \return TRUE if data is properly saved
 * \warning Assumes that FIFO_Init has been called.
 */
BOOL FIFO_Put(TFIFO * const FIFO, const UINT8 data); 

/**
 * \fn BOOL FIFO_Get(TFIFO * const FIFO, UINT8 * const dataPtr)
 * \brief Remove one character from the FIFO.
 * \param FIFO a  pointer to a FIFO struct with data to be retrieved
 * \param dataPtr a pointer to a memory location to place the retrieved byte
 * \return TRUE if the operation was successful and the data is valid
 * \warning Assumes that FIFO_Init has been called.
 */
BOOL FIFO_Get(TFIFO * const FIFO, UINT8 * const dataPtr); 

#endif