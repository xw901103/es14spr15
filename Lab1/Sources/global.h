/**
 * \file global.h
 * \brief Global header
 * \author Xu Waycell
 * \date
 */
#ifndef GLOBAL_H
#define GLOBAL_H

#include "defs.h"
#include "types.h"
#include "config.h"

extern TUINT16 ModConNumber;

#if !defined(ModConNumberLSB) && !defined(ModConNumberMSB)
#define ModConNumberLSB ModConNumber.s.Lo
#define ModConNumberMSB ModConNumber.s.Hi
#endif

#ifndef NO_DEBUG
void LogDebug(const UINT16 lineNumber, const UINT16 err);
#define DEBUG(LINE, ERR) LogDebug(LINE, ERR)
#endif

#endif
