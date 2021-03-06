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
#define OS_LIMIT_THREAD_STACK_SIZE 0x100

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
  TOSProcessRoutine processRoutine;
} TOSProcessContext;

typedef enum 
{
  OS_THREAD_STATE_UNKNOW,
  OS_THREAD_STATE_WAITING,
  OS_THREAD_STATE_READY,
  OS_THREAD_STATE_RUNNING,
  OS_THREAD_STATE_INTERRUPTED,
  OS_THREAD_STATE_ZOMBIE  
} TOSThreadState;

typedef struct
{
  BOOL isValid;
  BOOL isRunning;
  TOSProcess  process;
  TOSThreadState state;
  TOSThreadRoutine threadRoutine;

#if defined(OS_CPU_MOTOROLA_68HC11)
  volatile TUINT16 accumulator;
  volatile UINT16  indexRegisterX;
  volatile UINT16  indexRegisterY;  
  volatile UINT16  programCounter;
  volatile UINT8   conditionCodeRegister;
#else
#error "Unsupported CPU family/type."
#endif
  
  void* stackPtr;
  UINT8 stack[OS_LIMIT_THREAD_STACK_SIZE];
} TOSThreadContext;

TOSEnvironment OSEnvironment = {0};
TOSRuntimeContext OSRuntimeContext = {0};
TOSProcessContext OSProcessContextArray[OS_LIMIT_PROCESS_SIZE];
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

void OSThreadScheduleRoutine(void)
{
  UINT16 savedSP, accD, irX, irY, PC;
  UINT8 savedCCR;
  
  CRG_ResetCOP();
  
  asm std accD;
  
  //asm tfr pc, d;
  //asm std PC;

  asm tfr ccr, a;
  asm staa savedCCR;
  
  asm stx irX;
  asm sty irY;
  asm sts savedSP;
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
    
    OSProcessContextArray[0].isRunning = bFALSE;
    OSProcessContextArray[0].isValid = bTRUE;
    OSProcessContextArray[0].processRoutine = (TOSProcessRoutine)0x0000;
    
    thread = OSGetThread();
        
    OSRuntimeContext.isInitialized = bTRUE;
  
    return OSERROR_NO_ERROR;
  }
  return OSERROR_INVALID_ARGUMENT;
}

TOSError OS_Execute(void)
{
  UINT16 stackPtr;
  UINT16 routinePtr;

  if (!OSRuntimeContext.isInitialized)
  {
    return OSERROR_INVALID_ENVIRONMENT;
  }

  OSRuntimeContext.currentThread = (TOSThread)0x0000;
  OSRuntimeContext.isPanic = bFALSE;
  OSRuntimeContext.isRunning = bTRUE;
  
  Timer_AttachPeriodicTimerRoutine(&OSThreadScheduleRoutine);
  Timer_PeriodicTimerEnable(bTRUE);
  
  EnableInterrupts;
  
  if (OSEnvironment.startupProcessRoutine)
  {
    OSThreadContextArray[1].isValid = bTRUE;
    OSThreadContextArray[1].stackPtr = &OSThreadContextArray[1].stack[OS_LIMIT_THREAD_STACK_SIZE - 1];
    
    stackPtr = (UINT16)OSThreadContextArray[1].stackPtr;
    routinePtr = (UINT16)OSEnvironment.startupProcessRoutine;
    
    asm lds stackPtr;
    //asm jsr OSEnvironment.startupProcessRoutine;
    
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

TOSError OS_ThreadMutexInitialize(TOSMutex* mutex)
{
  if (mutex)
  {
    return OSERROR_NO_ERROR;
  }
  return OSERROR_INVALID_ARGUMENT;
}

TOSError OS_ThreadMutexDestroy(TOSMutex* mutex)
{
  if (mutex)
  {
    return OSERROR_NO_ERROR;
  }
  return OSERROR_INVALID_ARGUMENT;
}

TOSError OS_ThreadMutexLock(TOSMutex* mutex)
{
  if (mutex)
  {
    return OSERROR_NO_ERROR;
  }
  return OSERROR_INVALID_ARGUMENT;
}

TOSError OS_ThreadMutexTryLock(TOSMutex* mutex)
{
  if (mutex)
  {
    return OSERROR_NO_ERROR;
  }
  return OSERROR_INVALID_ARGUMENT;
}

TOSError OS_ThreadMutexUnlock(TOSMutex* mutex)
{
  if (mutex)
  {
    return OSERROR_NO_ERROR;
  }
  return OSERROR_INVALID_ARGUMENT;
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
      return OSERROR_OUT_OF_RESOURCE;
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
