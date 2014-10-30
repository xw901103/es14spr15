/**
 * \file OS.h
 * \brief
 * \author
 * \date
 */
#ifndef OS_H
#define OS_H

#include "global.h"

#define OS_LIMIT_THREAD_SIZE 1024

typedef struct 
{
  UINT32 oscClk;
  UINT32 busClk;
  void(*startupProcessRoutine)(void); /* first thread to be created */
  void(*panicRoutine)(void); /* it will be called once thing goes bad */
} TOSEnvironment;

typedef enum
{
  OSERROR_NO_ERROR,
  OSERROR_INVALID_ENVIRONMENT,
  OSERROR_INVALID_ARGUMENT,
  OSERROR_NO_RESOURCE,
  OSERROR_NOT_EXISTS
} TOSError;

typedef UINT16 TOSThread;
typedef UINT16 TOSProcess;

typedef void*(*TOSThreadRoutine)(void*);

/**
 * \fn TOSError OS_SetEnvironment(TOSEnvironment* aOSEnvironment)
 * \brief
 * \param
 * \return
 */
TOSError OS_SetEnvironment(TOSEnvironment* aOSEnvironment);

/**
 * \fn TOSError OS_Execute(void)
 * \brief Starts RTOS.
 * \return
 */
TOSError OS_Execute(void);

/**
 * \fn void OS_Terminate(void)
 * \brief Terminates RTOS.
 */
void OS_Terminate(void);

/**
 * \fn void OS_Panic(void)
 * \brief Sets RTOS on panic status.
 */
void OS_Panic(void);

/**
 * \fn TOSThread OS_ThreadSelf(void)
 * \brief
 * \return
 * \see pthread_self 
 */
TOSThread OS_ThreadSelf(void);

/**
 * \fn
 * \brief
 * \return
 * \see pthread_create
 */
TOSError OS_ThreadCreate(TOSThread* threadPtr, TOSThreadRoutine threadRoutinePtr, void* dataPtr);

/**
 * \fn TOSError OS_ThreadCancel(TOSThread thread)
 * \brief Requests that given thread be canceled.
 * \param thread
 * \return
 * \see pthread_cancel
 */
TOSError OS_ThreadCancel(TOSThread thread);

/**
 * \fn void OS_ThreadExit(void* dataPtr)
 * \brief Terminates the calling thread.
 * \param dataPtr
 * \see pthread_exit
 */
void OS_ThreadExit(void* dataPtr);

#endif
