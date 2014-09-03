/**
 * \file utils.c
 * \brief Implementations of utility functions.
 * \author Xu Waycell
 * \date 16-August-2014
 */
 #include "utils.h"

UINT16 ForgeWord(const UINT8 msb, const UINT8 lsb) {
  TUINT16 temp;
  
  temp.s.Hi = msb;
  temp.s.Lo = lsb;
  return temp.l; 
}
