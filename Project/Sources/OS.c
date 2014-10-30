/**
 * \file OS.c
 */
#include "OS.h"
#include "CRG.h"
#include "analog.h"
#include "SCI.h"
#include "timer.h"
#include "EEPROM.h"

#define OS_WATCHDOG_RATE     COP_RATE_2_24
#define OS_SCHEDULE_PERIOD   1000

typedef struct
{
  BOOL isInitialized;
  TOSThread currentThread;
  BOOL isPanic;
  BOOL isRunning;
} TOSRuntimeContext;

typedef struct
{
  BOOL isValid;
  BOOL isRunning;
  TOSThreadRoutine threadRoutine;
} TOSThreadContext;

TOSEnvironment OSEnvironment = {0};
TOSRuntimeContext OSRuntimeContext = {0};
TOSThreadContext OSThreadContextArray[OS_LIMIT_THREAD_SIZE];

/**
 * \fn TOSThread OSGetThread(void)
 * \brief Gets next free thread
 */
TOSThread OSGetThread(void)
{
  static TOSThread thread = 1;
  
  if (thread < (OS_LIMIT_THREAD_SIZE - 1))
  {
    return thread++;
  }
  return (TOSThread)0x0000;
}

void OSScheduleRoutine(void)
{
  CRG_ResetCOP();
}

void OSPanicRoutine(void)
{
  for(;;);
}

TOSError OS_SetEnvironment(TOSEnvironment* aOSEnvironment)
{
  TOSThread thread;

  if (aOSEnvironment)
  {
    OSEnvironment.oscClk = aOSEnvironment->oscClk;
    OSEnvironment.busClk = aOSEnvironment->busClk;
    OSEnvironment.startupProcessRoutine = aOSEnvironment->startupProcessRoutine;
    OSEnvironment.panicRoutine = aOSEnvironment->panicRoutine;
  
    /* TODO: use return values */
    (void)CRG_SetupPLL(OSEnvironment.busClk, OSEnvironment.oscClk, OSEnvironment.oscClk);
    //(void)CRG_SetupCOP(OS_WATCHDOG_RATE);

    Analog_Setup(OSEnvironment.busClk);

    Timer_SetupPeriodicTimer(OS_SCHEDULE_PERIOD, OSEnvironment.busClk);
    
    thread = OSGetThread();
        
    OSRuntimeContext.isInitialized = bTRUE;
  
    return OSERROR_NO_ERROR;
  }
  return OSERROR_INVALID_ARGUMENT;
}

TOSError OS_Execute(void)
{
  if (!OSRuntimeContext.isInitialized)
  {
    return OSERROR_INVALID_ENVIRONMENT;
  }

  OSRuntimeContext.currentThread = (TOSThread)0x0000;
  OSRuntimeContext.isPanic = bFALSE;
  OSRuntimeContext.isRunning = bTRUE;
  
  Timer_AttachPeriodicTimerRoutine(&OSScheduleRoutine);
  
  EnableInterrupts;
  
  if (OSEnvironment.startupProcessRoutine)
  {
    OSEnvironment.startupProcessRoutine();
  }
    
  while(OSRuntimeContext.isRunning)
  {
    if (OSRuntimeContext.isPanic)
    {
      if (OSEnvironment.panicRoutine)
      {
        OSEnvironment.panicRoutine();
      }
      else
      {
        OSPanicRoutine();
      }
    }
  }

  DisableInterrupts;

  (void)CRG_SetupCOP(COP_DISABLED);
    
  Timer_DetachPeriodicTimerRoutine();
  
  return OSERROR_NO_ERROR;
}

void OS_Terminate(void)
{
  OSRuntimeContext.isRunning = bFALSE;

  DisableInterrupts;

  (void)CRG_SetupCOP(COP_DISABLED);

  Timer_DetachPeriodicTimerRoutine();
}

void OS_Panic(void)
{
  OSRuntimeContext.isPanic = bTRUE;
  
  DisableInterrupts;

  (void)CRG_SetupCOP(COP_DISABLED);
  
  Timer_DetachPeriodicTimerRoutine();
}

TOSThread OS_ThreadSelf(void)
{
  return (TOSThread)OSRuntimeContext.currentThread;
}

TOSError OS_ThreadCreate(TOSThread* threadPtr, TOSThreadRoutine threadRoutinePtr, void* dataPtr)
{
  TOSThread thread = 0;
  
  if (threadPtr && threadRoutinePtr)
  {
    thread = OSGetThread();
    if (thread == 0)
    {
      return OSERROR_NO_RESOURCE;
    }
    *threadPtr = thread;
    OSThreadContextArray[thread].isRunning = bFALSE;
    OSThreadContextArray[thread].isValid = bTRUE;
    OSThreadContextArray[thread].threadRoutine = threadRoutinePtr;
    return OSERROR_NO_ERROR;
  }
  
  return OSERROR_INVALID_ARGUMENT;
}

TOSError OS_ThreadCancel(TOSThread thread)
{
  return OSERROR_NOT_EXISTS;
}

void OS_ThreadExit(void* dataPtr)
{
  OSThreadContextArray[OSRuntimeContext.currentThread].isRunning = bFALSE;
  OSRuntimeContext.currentThread = (TOSThread)0x0000;
}
