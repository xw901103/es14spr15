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

static THMIFrame HMIFrameBuffer[2] = {0};
static THMIContext HMIContext = {0};

static const THMIPanel * HMIPanelLookupTable[HMI_PANEL_SIZE];

#ifndef NO_INTERRUPT

static UINT16 HMIRoutinePeriod = 0;

void HMIRoutine(const TTimerChannel channelNb)
{
  static UINT16 count = 0; // 2ms every cycle
  //HMI_Poll();
  if (++count == 50) // 10 fps
  {
    HMI_Poll();
    count = 0;
  }
  Timer_ScheduleRoutine(channelNb, HMIRoutinePeriod);
}
#endif

/**
 * \fn HMI_Setup(const THMIContext * const aHMIContext)
 * \brief
 * \param
 * \return
 */
BOOL HMI_Setup(const THMISetup * const aHMISetup) 
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
  if (aHMISetup) 
  {
    if (LCD_Setup())
    {
      HMIContext.renderMode = aHMISetup->renderMode;
      //HMIContext.idlePanelId = aHMISetup->idlePanelId;
      HMIContext.screenFrameBufferPtr = &HMIFrameBuffer[0];
      HMIContext.renderFrameBufferPtr = &HMIFrameBuffer[1];      
      for (j = 0; j < aHMISetup->frameTemplate.height; ++j)
      {
        for (i = 0; i < aHMISetup->frameTemplate.width; ++i)
        {
          HMIContext.frameTemplate.data[j][i] = aHMISetup->frameTemplate.data[j][i];
        }
      }
      HMIContext.frameTemplate.width = aHMISetup->frameTemplate.width;
      HMIContext.frameTemplate.height = aHMISetup->frameTemplate.height;
      HMIContext.idlePanelId = 0;
      HMIContext.parentPanelId = 0;
      HMIContext.currentPanelId = 0;
      HMIContext.previousPanelId = 0;
      
      HMIContext.hours = 0;
      HMIContext.minutes = 0;
      HMIContext.seconds = 0;      
#ifndef NO_INTERRUPT
      HMIRoutinePeriod = (UINT16)(48000); // 2ms
      
      Timer_Init(TIMER_Ch6, &timerCh6);
      Timer_Set(TIMER_Ch6, HMIRoutinePeriod);
      Timer_AttachRoutine(TIMER_Ch6, &HMIRoutine);
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
  static BOOL key1 = bFALSE, key2 = bFALSE, key3 = bFALSE, key4 = bFALSE, key5 = bFALSE;

  if (HMI_KEY1)
  {
    if (!key1)
    {      
      key1 = bTRUE;
      return HMI_KEY_SET;
    }
  }
  else
  {    
    key1 = bFALSE;
  }
  
  if (HMI_KEY2)
  {
    if (!key2)
    {      
      key2 = bTRUE;
      return HMI_KEY_DATA;
    }
  }
  else
  {    
    key2 = bFALSE;
  }
  
  if (HMI_KEY3)
  {
    if (!key3)
    {      
      key3 = bTRUE;
      return HMI_KEY_UP;
    }
  }
  else
  {    
    key3 = bFALSE;
  }
  
  if (HMI_KEY4)
  {
    if (!key4)
    {      
      key4 = bTRUE;
      return HMI_KEY_DOWN; 
    }
  }
  else
  {    
    key4 = bFALSE;
  }
  
  if (HMI_KEY5)
  {
    if (!key5)
    {      
      key5 = bTRUE;
      return HMI_KEY_SELECT;
    }
  }
  else
  {    
    key5 = bFALSE;
  }
  
  return HMI_KEY_NULL;
}

/* return true if display frame successfully */
BOOL HMI_RenderFrame(void)
{ 
  static UINT8 timerTick = ' ';
   
  THMIFrame * frameBufferPtr = 0;
  const THMIPanel * panelPtr = HMIPanelLookupTable[HMIContext.currentPanelId];
  UINT8 i = 0, j = 0;
  
  frameBufferPtr = HMIContext.renderFrameBufferPtr;
  
  for (j = 0; j < HMIContext.frameTemplate.height; ++j)
  {
    for (i = 0; i < HMIContext.frameTemplate.width; ++i)
    {
      frameBufferPtr->data[j][i] = HMIContext.frameTemplate.data[j][i];
    }
  }
  frameBufferPtr->width = HMIContext.frameTemplate.width;
  frameBufferPtr->height = HMIContext.frameTemplate.height;
  
  if (panelPtr)
  {    
    for (i = 0; i < HMI_PANEL_TITLE_SIZE; ++i)
    {
      frameBufferPtr->data[0][i] = panelPtr->title[i];
    }
  }
  
  frameBufferPtr->data[0][15] = HMIContext.seconds % 10 + '0';
  frameBufferPtr->data[0][14] = HMIContext.seconds / 10 + '0';
    
  frameBufferPtr->data[0][12] = HMIContext.minutes % 10 + '0';
  frameBufferPtr->data[0][11] = HMIContext.minutes / 10 + '0';    

  frameBufferPtr->data[0][9] = HMIContext.hours % 10 + '0';
  frameBufferPtr->data[0][8] = HMIContext.hours / 10 + '0';    
      
  if (frameBufferPtr->data[0][15] != HMIContext.screenFrameBufferPtr->data[0][15])
  {
    if (timerTick == ':')
    {
      timerTick = ' ';
    }
    else
    {
      timerTick = ':';
    }
  }

  frameBufferPtr->data[0][13] = timerTick;    
  frameBufferPtr->data[0][10] = timerTick;
  
  if (LCD_OutFrame(frameBufferPtr->data))
  {
    HMIContext.renderFrameBufferPtr = HMIContext.screenFrameBufferPtr;    
    HMIContext.screenFrameBufferPtr = frameBufferPtr;
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
  // TODO: replace unused with a debug macro
  UNUSED(HMI_RenderFrame()); // rendering continuity  
}

void HMI_SetTime(UINT16 hours, UINT16 minutes, UINT16 seconds)
{
  HMIContext.hours = hours;
  HMIContext.minutes = minutes;
  HMIContext.seconds = seconds;
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
