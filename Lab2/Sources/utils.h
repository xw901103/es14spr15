/**
 * \file utils.h
 * \brief Utilities
 */
#ifndef UTILS_H
#define UTILS_H

#include "global.h"

/**
 * \fn UINT16 ForgeWord(const UINT8 msb, const UINT8 lsb)
 * \brief
 * \param msb Most significant byte
 * \param lsb Least significant byte
 * \return A word
 */
UINT16 ForgeWord(const UINT8 msb, const UINT8 lsb);

#endif