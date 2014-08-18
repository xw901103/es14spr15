#include "timer.h"
#include "mc9s12a512.h"

void Timer_SetupPeriodicTimer(const UINT16 microSeconds, const UINT32 busClk) {
  
}

void Timer_PeriodicTimerEnable(const BOOL enable) {
  
}

void Timer_Setup(void) {
  
}

void Timer_Init(const TTimerChannel channelNb, const TTimerSetup * const aTimerSetup) {
}

void Timer_Enable(const TTimerChannel channelNb, const BOOL enableInt) {
}

BOOL Timer_Enabled(const TTimerChannel channelNb) {
    return bFALSE;  
}

void Timer_Set(const TTimerChannel channelNb, const UINT16 busClkCyclesDelay) {
}
