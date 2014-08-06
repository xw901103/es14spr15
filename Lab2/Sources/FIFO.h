// ----------------------------------------
// Filename: FIFO.h
// Description: Routines to implement a FIFO buffer 
// Author: PMcL
// Date: 27-Mar-06

#ifndef FIFO_H
#define FIFO_H

// new types
#include "global.h"

// Number of bytes in a FIFO
#define FIFO_SIZE 256

// ----------------------------------------
// FIFO structure
typedef struct
{
  UINT8 Start, End;
  UINT8 volatile NbBytes;
  UINT8 Buffer[FIFO_SIZE];
} TFIFO;

// ----------------------------------------
// FIFO_Init
// 
// Initialize the FIFO
// Input:
//   FIFO is a pointer to a FIFO struct to initialize
// Output:
//   none
// Conditions:
//  none

void FIFO_Init(TFIFO * const FIFO);

// ----------------------------------------
// FIFO_Put
// 
// Enter one character into the FIFO
// Input:
//   FIFO is a  pointer to a FIFO struct where data is to be stored
//   data is a byte of data to store in the FIFO buffer
// Output:
//   TRUE if data is properly saved
// Conditions:
//   Assumes that FIFO_Init has been called

BOOL FIFO_Put(TFIFO * const FIFO, const UINT8 data); 

// ----------------------------------------
// FIFO_Get
// 
// Remove one character from the FIFO
// Input:
//   FIFO is a  pointer to a FIFO struct with data to be retrieved
//   dataPtr is a pointer to a memory location to place the retrieved byte
// Output:
//   TRUE if the operation was successful and the data is valid
// Conditions:
//   Assumes that FIFO_Init has been called

BOOL FIFO_Get(TFIFO * const FIFO, UINT8 * const dataPtr); 

#endif