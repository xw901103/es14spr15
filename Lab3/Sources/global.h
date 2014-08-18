/**
 * \file global.h
 * \brief Global head file is the nexus for predefined types, macros and configurations.
 * \author Xu Waycell
 * \date 05-August-2014
 */
#ifndef GLOBAL_H
#define GLOBAL_H

#include "defs.h"
#include "types.h"
#include "config.h"

#ifndef NO_DEBUG
/**
 * \fn void LogDebug(const UINT16 lineNumber, const UINT16 err)
 * \brief Logs debug information including line number and error number.
 * \param lineNumber line number of source file
 * \param err error number refers to predefined errors
 */
void LogDebug(const UINT16 lineNumber, const UINT16 err);
#define DEBUG(LINE, ERR) LogDebug(LINE, ERR)
#endif

#endif
