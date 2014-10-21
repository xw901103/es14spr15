#include "HMI.h"
#include "LCD.h"
#ifndef NO_INTERRUPT
#include "timer.h"
#endif
#include <mc9s12a512.h>     

#define HMI_BUTTON1 PORTK_BIT2
#define HMI_BUTTON2 PORTK_BIT3
#define HMI_BUTTON3 PORTK_BIT4
#define HMI_BUTTON4 PORTK_BIT5
#define HMI_BUTTON5 PORTK_BIT7

static THMIContext HMIContext;

static const THMIPanel* HMIPanelLookupTable[HMI_PANEL_SIZE];

#ifndef NO_INTERRUPT

static UINT16 HMIRoutinePeriod = 0;

void HMIRoutine(const TTimerChannel channelNb)
{
  HMI_Poll();
  Timer_ScheduleRoutine(channelNb, HMIRoutinePeriod);
}
#endif

/**
 * \fn
 * \brief
 * \param
 * \return
 */
BOOL HMI_Setup(const THMIContext* const aHMIContext) 
{
#ifndef NO_INTERRUPT
  TTimerSetup timerCh6;
  
  timerCh6.outputCompare    = bTRUE;
  timerCh6.outputAction     = TIMER_OUTPUT_DISCONNECT;
  timerCh6.inputDetection   = TIMER_INPUT_OFF;
  timerCh6.toggleOnOverflow = bFALSE;
  timerCh6.interruptEnable  = bFALSE;
  timerCh6.pulseAccumulator = bFALSE;
  timerCh6.routine          = &HMIRoutine;
#endif
  if (aHMIContext) 
  {
    if (LCD_Setup())
    {    
      HMIContext.idlePanelId = aHMIContext->idlePanelId;
#ifndef NO_INTERRUPT
      Timer_Init(TIMER_Ch6, &timerCh6);
      Timer_Set(TIMER_Ch6, 0);
      Timer_Enable(TIMER_Ch6, bTRUE);            
#endif
      DDRK = DDRK & (DDRK_BIT0_MASK | DDRK_BIT1_MASK);
      return bTRUE;
    }
  }
  else 
  {
    DEBUG(__LINE__, ERR_INVALID_POINTER);
  }
  return bFALSE;
}

const THMIContext * HMI_GetContext(void)
{
  return &HMIContext;
}

THMIButton HMI_GetButton(void)
{
  if (HMI_BUTTON1)
  {
    return HMI_BUTTON_SET;
  }
  if (HMI_BUTTON2)
  {
    return HMI_BUTTON_DATA;
  }
  if (HMI_BUTTON3)
  {
    return HMI_BUTTON_UP;
  }
  if (HMI_BUTTON4)
  {
    return HMI_BUTTON_DOWN;    
  }
  if (HMI_BUTTON5)
  {
    return HMI_BUTTON_SELECT;
  }
  return HMI_BUTTON_NULL;
}

void HMI_Poll(void) 
{
  const THMIPanel* panelPtr = HMIPanelLookupTable[HMIContext.currentPanelId];
  THMIButton button = HMI_GetButton();
  
  if (panelPtr)
  {
    if (button != HMI_BUTTON_NULL && panelPtr->inputHandler)
    {
      panelPtr->inputHandler(button);      
    }
  }
}

void HMI_AppendPanel(const THMIPanel* const aHMIPanel) 
{
  if (aHMIPanel)
  {
    HMIPanelLookupTable[aHMIPanel->id] = aHMIPanel;
  }
  else
  {
    DEBUG(__LINE__, ERR_INVALID_POINTER);
  }
}

void HMI_RemovePanel(const UINT8 panelId)
{
  HMIPanelLookupTable[panelId] = (const THMIPanel*)0x0000;
}

void HMI_ShowPanel(const UINT8 panelId) 
{
  HMIContext.previousPanelId = HMIContext.currentPanelId;
  HMIContext.currentPanelId = panelId;
}

void HMI_ClosePanel(void) 
{
  HMIContext.previousPanelId = HMIContext.currentPanelId;
  HMIContext.currentPanelId = HMIContext.parentPanelId;
}
