/**
 * \file utils.h
 * \brief Utilities functions to make life easier.
 * \author Xu Waycell
 * \date 16-August-2014
 */
#ifndef UTILS_H
#define UTILS_H

#include "global.h"

/**
 * \fn UINT16 ForgeWord(const UINT8 msb, const UINT8 lsb)
 * \brief Combines two eight bits number into a sixteen bits number.
 * \param msb Most significant byte
 * \param lsb Least significant byte
 * \return A 16-bits word combained by most and least significant parts.
 */
UINT16 ForgeWord(const UINT8 msb, const UINT8 lsb);

/**
 * \fn INT16 FindMedianOfThreeNumbers(const INT16 a, const INT16 b, const INT16 c)
 * \brief Finds the median of three given numbers
 * \param a the first number
 * \param b the second number
 * \param c the third number
 * \return median of these three given numbers
 */
INT16 FindMedianOfThreeNumbers(const INT16 a, const INT16 b, const INT16 c); 

void SwapBytes(UINT8 * const lhs, UINT8 * const rhs);

/**
 * \fn void CopyBytes(UINT8* destination, UINT8* source, UINT16 count)
 * \brief
 * \param
 * \param
 * \param
 */
void CopyBytes(UINT8* const destination, const UINT8* const source, const UINT16 total);

#endif