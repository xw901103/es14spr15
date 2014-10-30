#include "debug.h"

#ifndef NO_DEBUG
/**
 * \fn void LogDebug(const unsigned int lineNumber, const unsigned int err)
 * \brief Logs debug information including line number and error number.
 * \param lineNumber line number of source file
 * \param err error number refers to predefined errors
 */
void LogDebug(const unsigned int lineNumber, const unsigned int err)
{
    /* break point here */
    (void)lineNumber;
    (void)err;
}
#endif
