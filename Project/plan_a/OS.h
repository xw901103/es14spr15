/**
 * \file OS.h
 * \brief
 * \author
 * \date
 */
#ifndef OS_H
#define OS_H

#include "global.h"

#ifdef __HC12__
#define OS_CPU_MOTOROLA_68HC11 /* for Motorola 68HC11 family */
#elif defined(__arm__) || defined(_M_ARM)
#define OS_CPU_ARM             /* for ARM family */
#error "ARM is not supported yet."
#elif defined(__i386) || defined(_M_I86) || defined(__X86__)
#define OS_CPU_INTEL_X86       /* for intel x86 and AMD compatibles */
#error "x86 is not supported yet."
#else
#error "Unsupported CPU family/type."
#endif

#define OS_LIMIT_PROCESS_SIZE 2
#define OS_LIMIT_THREAD_SIZE 16

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
  OSERROR_OUT_OF_RESOURCE,
  OSERROR_NOT_EXISTS
} TOSError;

typedef UINT16 TOSMutex;
typedef UINT16 TOSThread;
typedef UINT16 TOSProcess;

typedef void*(*TOSThreadRoutine)(void*);
typedef void(*TOSProcessRoutine)(void);

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
 * \fn TOSError OS_ThreadMutexInitialize(TOSMutex* mutex)
 * \brief
 * \param mutex
 * \return
 * \see pthread_mutex_init
 */
TOSError OS_ThreadMutexInitialize(TOSMutex* mutex);

/**
 * \fn TOSError OS_ThreadMutexDestroy(TOSMutex* mutex)
 * \brief
 * \param mutex
 * \return
 * \see pthread_mutex_destroy
 */
TOSError OS_ThreadMutexDestroy(TOSMutex* mutex);

/**
 * \fn TOSError OS_ThreadMutexLock(TOSMutex* mutex)
 * \brief
 * \param mutex
 * \return
 * \see pthread_mutex_lock
 */
TOSError OS_ThreadMutexLock(TOSMutex* mutex);

/**
 * \fn TOSError OS_ThreadMutexTryLock(TOSMutex* mutex)
 * \brief
 * \param mutex
 * \return
 * \see pthread_mutex_trylock
 */
TOSError OS_ThreadMutexTryLock(TOSMutex* mutex);

/**
 * \fn TOSError OS_ThreadMutexUnlock(TOSMutex* mutex)
 * \brief
 * \param mutex
 * \return
 * \see pthread_mutex_unlock
 */
TOSError OS_ThreadMutexUnlock(TOSMutex* mutex);

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
