#ifndef GLOBAL_H
#define GLOBAL_H

#include "defs.h"
#include "types.h"

extern TUINT16 ModConNumber;

#if !defined(ModConNumberLSB) && !defined(ModConNumberMSB)
#define ModConNumberLSB ModConNumber.s.Lo
#define ModConNumberMSB ModConNumber.s.Hi
#endif

void LogDebug(const UINT16 lineNumber, const UINT16 err);

#endif
