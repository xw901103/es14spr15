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
 * \brief
 * \param msb Most significant byte
 * \param lsb Least significant byte
 * \return A 16-bits word combained by most and least significant parts.
 */
UINT16 ForgeWord(const UINT8 msb, const UINT8 lsb);

#endif