/**
 * \file utils.c
 * \brief Implementations of utility functions.
 * \author Xu Waycell
 * \date 16-August-2014
 */
#include "utils.h"

/**
 * \fn UINT16 ForgeWord(const UINT8 msb, const UINT8 lsb)
 * \brief Merges two unsigned 8-bits into a unsigned 16-bits number.
 * \param msb Most significant byte
 * \param lsb Least significant byte
 * \return A 16-bits word combained by most and least significant parts.
 */
UINT16 ForgeWord(const UINT8 msb, const UINT8 lsb) {
  TUINT16 temp;
  
  temp.s.Hi = msb;
  temp.s.Lo = lsb;
  return temp.l; 
}

/**
 * \fn INT16 FindMedianOfThreeNumbers(const INT16 a, const INT16 b, const INT16 c)
 * \brief Find the median of three given numbers
 * \param a the first number
 * \param b the second number
 * \param c the third number
 * \return median of these three given numbers
 */
INT16 FindMedianOfThreeNumbers(const INT16 a, const INT16 b, const INT16 c)
{
  if ((a >= b && a <= c) || (a <= b && a >= c))
  {
  	return a;
  }
  else if ((b >= a && b <= c) || (b <= a && b >= c))
  {
  	return b;
  }
  /* if not a or b then c */
  return c;
}
