/**
 * \file SCI.h
 * \brief I/O routines for MC9S12 serial communication interface 
 * \author Xu Waycell
 * \date
 */
#ifndef SCI_H
#define SCI_H

#include "global.h"

/**
 * \fn void SCI_Setup(const UINT32 baudRate, const UINT32 busClk) 
 * \brief Sets up the Serial Communication Interface including receive and transmit buffers.
 * \param baudRate the baud rate in bits/sec
 * \param busClk the bus clock rate in Hz
 */
void SCI_Setup(const UINT32 baudRate, const UINT32 busClk);
 
/**
 * \fn BOOL SCI_InChar(UINT8 * const dataPtr)
 * \see FIFO_Get
 * \brief Get a character from the receive FIFO if it is not empty.
 * \param dataPtr a pointer to memory to store the retrieved byte
 * \return TRUE if the receive FIFO returned a character
 * \warning Assumes the receive FIFO has been initialized.
 */
BOOL SCI_InChar(UINT8 * const dataPtr);  
 
/**
 * \fn BOOL SCI_OutChar(const UINT8 data)
 * \see FIFO_Put
 * \brief Put a byte in the transmit FIFO if it is not full.
 * \param data is a byte to be placed in the transmit FIFO
 * \return TRUE if the data was placed in the transmit FIFO
 * \warning Assumes the transmit FIFO has been initialized.
 */
BOOL SCI_OutChar(const UINT8 data);

/**
 * \fn void SCI_Poll(void)
 * \brief Poll the SCI status to see whether to receive or transmit a single byte.
 * \warning Assumes transmit and receive FIFOs have been initialized.
 */
void SCI_Poll(void);

#endif