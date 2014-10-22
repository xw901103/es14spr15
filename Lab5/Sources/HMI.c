#include "HMI.h"
#include "LCD.h"
#ifndef NO_INTERRUPT
#include "timer.h"
#endif
#include <mc9s12a512.h>     

#define HMI_KEY1 PORTK_BIT2
#define HMI_KEY2 PORTK_BIT3
#define HMI_KEY3 PORTK_BIT4
#define HMI_KEY4 PORTK_BIT5
#define HMI_KEY5 PORTK_BIT7

static THMIContext HMIContext = {0};

static const THMIPanel * HMIPanelLookupTable[HMI_PANEL_SIZE];

#ifndef NO_INTERRUPT

static UINT16 HMIRoutinePeriod = 0;

void HMIRoutine(const TTimerChannel channelNb)
{
  HMI_Poll();
  Timer_ScheduleRoutine(channelNb, HMIRoutinePeriod);
}
#endif

/**
 * \fn HMI_Setup(const THMIContext * const aHMIContext)
 * \brief
 * \param
 * \return
 */
BOOL HMI_Setup(const THMIContext * const aHMIContext) 
{
  UINT16 i = 0, j = 0;
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
      HMIContext.renderMode = aHMIContext->renderMode;
      HMIContext.idlePanelId = aHMIContext->idlePanelId;
      for (j = 0; j < aHMIContext->frameTemplate.height; ++j)
      {
        for (i = 0; i < aHMIContext->frameTemplate.width; ++i)
        {
          HMIContext.frameTemplate.data[j][i] = aHMIContext->frameTemplate.data[j][i];
        }
      }
      HMIContext.idlePanelId = 0;
      HMIContext.parentPanelId = 0;
      HMIContext.currentPanelId = 0;
      HMIContext.previousPanelId = 0;      
#ifndef NO_INTERRUPT
      Timer_Init(TIMER_Ch6, &timerCh6);
      Timer_Set(TIMER_Ch6, 0);
      Timer_Enable(TIMER_Ch6, bTRUE);            
#endif
      DDRK = DDRK & (DDRK_BIT0_MASK | DDRK_BIT1_MASK);
      return bTRUE;
    } else {
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_LCD_SETUP);
#endif
    }
  }
  else 
  {
    DEBUG(__LINE__, ERR_INVALID_POINTER);
  }
  return bFALSE;
}

const THMIContext * const HMI_GetContext(void)
{
  return &HMIContext;
}

THMIKey HMI_GetKeyEvent(void)
{
  if (HMI_KEY1)
  {
    return HMI_KEY_SET;
  }
  if (HMI_KEY2)
  {
    return HMI_KEY_DATA;
  }
  if (HMI_KEY3)
  {
    return HMI_KEY_UP;
  }
  if (HMI_KEY4)
  {
    return HMI_KEY_DOWN;    
  }
  if (HMI_KEY5)
  {
    return HMI_KEY_SELECT;
  }
  return HMI_KEY_NULL;
}

/* return true if display frame successfully */
BOOL HMI_RenderFrame(void)
{
  static THMIFrame frameBuffer[2] = {0};
  static THMIFrame* displayFramePtr;
  static THMIFrame* drawFramePtr;
  
  THMIFrame * framePtr = 0;
  UINT8 i = 0, j = 0;
  
  framePtr = drawFramePtr;
  
  for (j = 0; j < HMIContext.frameTemplate.height; ++j)
  {
    for (i = 0; i < HMIContext.frameTemplate.width; ++i)
    {
      framePtr->data[j][i] = HMIContext.frameTemplate.data[j][i];
    }
  }
  framePtr->width = HMIContext.frameTemplate.width;
  framePtr->height = HMIContext.frameTemplate.height;
    
  if (LCD_OutFrame(framePtr->data))
  {
    drawFramePtr = displayFramePtr;    
    displayFramePtr = framePtr;
    return bTRUE;
  }
  return bFALSE;
}

void HMI_Poll(void) 
{
  const THMIPanel * panelPtr = HMIPanelLookupTable[HMIContext.currentPanelId];
  THMIKey key = HMI_GetKeyEvent();
  
  if (panelPtr)
  {
    if (key != HMI_KEY_NULL && panelPtr->inputHandler)
    {
      panelPtr->inputHandler(key);      
    }
  }
}

void HMI_AppendPanel(const THMIPanel * const aHMIPanel) 
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
  HMIPanelLookupTable[panelId] = (const THMIPanel *)0x0000;
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
