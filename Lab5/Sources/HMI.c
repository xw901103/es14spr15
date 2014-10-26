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

/* TODO: remove this by using pointer outside e.g. HMI_ShowPanel(THMIPanel*) */
static THMIPanel * HMIPanelLookupTable[HMI_PANEL_SIZE];

THMIPopup HMI_BACKLIGHT_POPUP =
{
  "BACKLIGHT",
  {    
    {    
      '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*'
    },
    {    
      '*', ' ', 'B', 'R', 'I', 'G', 'H', 'T', 'N', 'E', 'S', 'S', ' ', '*'
    },
    {    
      '*', ' ', ' ', ' ', ' ', '>', 'O', 'F', 'F', ' ', ' ', ' ', ' ', '*'
    },
    {    
      '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*'
    }
  }
};

THMIPopup HMI_CONTRAST_POPUP =
{
  "CONTRAST",
  {
    {    
      '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*'
    },
    {    
      '*', ' ', ' ', 'C', 'O', 'N', 'T', 'R', 'A', 'S', 'T', ' ', ' ', '*'
    },
    {    
      '*', ' ', ' ', ' ', ' ', '<', '6', '3', '>', ' ', ' ', ' ', ' ', '*'
    },
    {    
      '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*'
    }
  }
};

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

BOOL HMIPopupProcessRoutine(void)
{
  /* TODO: connect variables to ModConHMIBacklight and ModConHMIContrast */
  static BOOL backlight = bFALSE;
  static UINT8 contrast = 15;
  
  if (HMI_KEY5 && HMI_KEY2)
  {
    backlight = !backlight;
    if (backlight)
    {      
      HMI_BACKLIGHT_POPUP.text[2][6] = ' ';
      HMI_BACKLIGHT_POPUP.text[2][7] = 'O';
      HMI_BACKLIGHT_POPUP.text[2][8] = 'N';      
    }
    else
    {
      HMI_BACKLIGHT_POPUP.text[2][6] = 'O';
      HMI_BACKLIGHT_POPUP.text[2][7] = 'F';
      HMI_BACKLIGHT_POPUP.text[2][8] = 'F';      
    }
    LCD_Backlight(backlight);
    HMI_ShowPopup(&HMI_BACKLIGHT_POPUP);
    return bTRUE;
  }
  else if (HMI_KEY5 && (HMI_KEY3 || HMI_KEY4))
  {
    if (HMI_KEY3)
    {
      if (contrast > 0)
      {        
        --contrast;
      }
    }
    if (HMI_KEY4)
    {
      if (contrast < 63)
      {
        ++contrast;
      }
    }
    HMI_CONTRAST_POPUP.text[2][6] = contrast/10 + '0';
    HMI_CONTRAST_POPUP.text[2][7] = contrast%10 + '0';    
    UNUSED(LCD_SetContrast(contrast));
    HMI_ShowPopup(&HMI_CONTRAST_POPUP);
    return bTRUE;
  }
  return bFALSE;
}

void HMIPanelInputHandler(THMIPanel* const panelPtr, THMIKey key)
{
  UINT8 focusedMenuItemIndex = HMI_GetFocusedMenuItemIndex();
  UINT8 selectedMenuItemIndex = HMI_GetSelectedMenuItemIndex();
  
  if (panelPtr)
  {    
    switch(key)
    {
      case HMI_KEY_UP:
        if (panelPtr->menuPtr)
        {
          if (panelPtr->menuPtr->type == HMI_MENU_TYPE_STATIC)
          {
            /* TODO: use startingMenuItemIndex inside the HMIContext instead of HMIMenu.startingMenuItemIndex */
            if (panelPtr->menuPtr->startingMenuItemIndex > 0)
            {              
              panelPtr->menuPtr->startingMenuItemIndex--;
            }
          }
          else if (selectedMenuItemIndex != focusedMenuItemIndex)
          {          
            if (focusedMenuItemIndex > 0)
            {
              if (--focusedMenuItemIndex < panelPtr->menuPtr->startingMenuItemIndex)
              {
                panelPtr->menuPtr->startingMenuItemIndex--;  
              }
            }
            HMI_SetFocusedMenuItemIndex(focusedMenuItemIndex);
          }
          else
          {
            /* update value */
            if (!panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->useMutatedValue)
            {
              panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->mutatedValue = panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->value;  
              panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->useMutatedValue = bTRUE;
            }
            switch(panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->valueType)
            {
              case HMI_MENU_ITEM_VALUE_TYPE_UNSIGNED_INTEGER:
                if (panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->mutatedValue.l < panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->maximumValue.l)
                {                  
                  panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->mutatedValue.l++; 
                }
                break;
              case HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN:
                panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->mutatedValue.b.Boolean = (UINT8)bTRUE;          
                break;
              default:
                break;
            }
          }
        }
        break;
      case HMI_KEY_DOWN:
        if (panelPtr->menuPtr)
        {
          if (panelPtr->menuPtr->type == HMI_MENU_TYPE_STATIC)
          {
            /* TODO: use startingMenuItemIndex inside the HMIContext instead of HMIMenu.startingMenuItemIndex */          
            if ((panelPtr->menuPtr->itemCount - panelPtr->menuPtr->startingMenuItemIndex) > 4)
            {              
              panelPtr->menuPtr->startingMenuItemIndex++;
            }
          }        
          else if (selectedMenuItemIndex != focusedMenuItemIndex)
          {          
            if (focusedMenuItemIndex < (panelPtr->menuPtr->itemCount - 1)) /* from 0 to n-1 total n items*/
            {
              if (++focusedMenuItemIndex >= panelPtr->menuPtr->startingMenuItemIndex + 4)
              {
                panelPtr->menuPtr->startingMenuItemIndex++;  
              }
            }
            HMI_SetFocusedMenuItemIndex(focusedMenuItemIndex);
          }
          else
          {
            /* update value */
            if (!panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->useMutatedValue)
            {
              panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->mutatedValue = panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->value;  
              panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->useMutatedValue = bTRUE;
            }
            switch(panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->valueType)
            {
              case HMI_MENU_ITEM_VALUE_TYPE_UNSIGNED_INTEGER:
                if (panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->mutatedValue.l > panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->minimumValue.l)
                {                  
                  panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->mutatedValue.l--; 
                }
                break;
              case HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN:
                panelPtr->menuPtr->itemPtr[selectedMenuItemIndex]->mutatedValue.b.Boolean = (UINT8)bFALSE;          
                break;
              default:
                break;
            }
          }
        }      
        break;
      case HMI_KEY_DATA:
        break;
      case HMI_KEY_SET:
        break;
      case HMI_KEY_SELECT:
        if (panelPtr->menuPtr)
        {
          if (selectedMenuItemIndex == focusedMenuItemIndex)
          {
            HMI_ClearSelectedMenuItemIndex();
          }
          else
          {
            HMI_SetSelectedMenuItemIndex(focusedMenuItemIndex);
          }
        }
      default:
#ifndef NO_DEBUG
        DEBUG(__LINE__, ERR_INVALID_ARGUMENT);
#endif
        break;
    }
  }
  else
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif
  }
}

/* TODO: assign menu item value to mutatedValue once the panel is shown */
void HMIClearMenuItemMutatedValues(THMIMenu* const menuPtr)
{
  UINT8 i = 0;
  
  if (menuPtr)
  {
    for (i = 0; i < 8; ++i)
    {
      if (menuPtr->itemPtr[i])
      {
        menuPtr->itemPtr[i]->useMutatedValue = bFALSE;
      }
    }
  }
}

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
      
      HMIContext.focusedMenuItemId = 0xFF;
      HMIContext.selectedMenuItemId = 0xFF;
      
      HMIContext.idlePanelId = aHMISetup->idlePanelId;
      HMIContext.idleTimeCount = 0;
      HMIContext.maxIdleTimeCount = aHMISetup->maxIdleTimeCount;
      HMIContext.parentPanelId = 0;
      HMIContext.currentPanelId = 0;
      HMIContext.previousPanelId = 0;
      
      HMIContext.popupPtr = (THMIPopup*)0x00;
      HMIContext.maxPopupTimeCount = 10;
      HMIContext.popupTimeCount = 0;
      
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
      
      UNUSED(LCD_Backlight(aHMISetup->backlight));
      UNUSED(LCD_SetContrast((UINT8)aHMISetup->contrast));
      
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
  static UINT8 animationCount = 0;
  static UINT8 timerTick = ' ';
  //static UINT8 beginningMenuItemIndex = 0;
   
  THMIFrame * frameBufferPtr = 0;
  const THMIPanel * panelPtr = HMIPanelLookupTable[HMIContext.currentPanelId];
  THMIMenuItemValue menuItemValue;
  UINT8 i = 0, j = 0, menuItemIndex = 0, menuItemTitleInitialPosition = 0, menuItemValueInitialPosition = 0;
  
  frameBufferPtr = HMIContext.renderFrameBufferPtr;
  ++animationCount;
    
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
    
    if (panelPtr->dialogPtr)
    {
      for (i = 0; i < HMI_DIALOG_MAXIMUM_HEIGHT; ++i)
      {
        for (j = 0; j < HMI_DIALOG_MAXIMUM_WIDTH; ++j)
        {
          frameBufferPtr->data[2+i][0+j] = panelPtr->dialogPtr->text[i][j];
        }
      }
    }
    else if (panelPtr->menuPtr)
    {
      switch(panelPtr->menuPtr->type)
      {
        case HMI_MENU_TYPE_STATIC:
          menuItemTitleInitialPosition = 0;
          menuItemValueInitialPosition = 0;
          break;
        case HMI_MENU_TYPE_SETTING:
          menuItemTitleInitialPosition = 1;
          menuItemValueInitialPosition = 1;
          break;
        default:
          menuItemTitleInitialPosition = 0;
          menuItemValueInitialPosition = 0;
          break;
      };
      
      for(i = 0; i < 4; ++i)
      {
        menuItemIndex = panelPtr->menuPtr->startingMenuItemIndex + i;
        if (panelPtr->menuPtr->itemPtr[menuItemIndex])
        {
          for (j = 0; j < HMI_MENU_ITEM_TITLE_SIZE; ++j)
          {
            /* TODO: use configurable setting for panel area starting row */
            if (panelPtr->menuPtr->itemPtr[menuItemIndex]->title[j] > 0)
            {              
              frameBufferPtr->data[2+i][menuItemTitleInitialPosition + j] = panelPtr->menuPtr->itemPtr[menuItemIndex]->title[j];
            }
          }
          if (panelPtr->menuPtr->itemPtr[menuItemIndex]->useMutatedValue)
          {
            menuItemValue = panelPtr->menuPtr->itemPtr[menuItemIndex]->mutatedValue;
          }
          else
          {
            menuItemValue = panelPtr->menuPtr->itemPtr[menuItemIndex]->value;
          }
          switch(panelPtr->menuPtr->itemPtr[menuItemIndex]->valueType)
          {
            case HMI_MENU_ITEM_VALUE_TYPE_UNSIGNED_INTEGER:
              frameBufferPtr->data[2+i][15 - menuItemValueInitialPosition] = (menuItemValue.l % 10) + '0';
              frameBufferPtr->data[2+i][14 - menuItemValueInitialPosition] = (menuItemValue.l % 100) / 10 + '0';
              frameBufferPtr->data[2+i][13 - menuItemValueInitialPosition] = (menuItemValue.l % 1000) / 100 + '0';
              frameBufferPtr->data[2+i][12 - menuItemValueInitialPosition] = (menuItemValue.l % 10000) /1000 + '0';
              frameBufferPtr->data[2+i][11 - menuItemValueInitialPosition] = (menuItemValue.l % 100000) /10000 + '0';          
              break;
            case HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN:
              if (menuItemValue.b.Boolean)
              {
                frameBufferPtr->data[2+i][11 - menuItemValueInitialPosition] = 'T';                          
                frameBufferPtr->data[2+i][12 - menuItemValueInitialPosition] = 'R';
                frameBufferPtr->data[2+i][13 - menuItemValueInitialPosition] = 'U';
                frameBufferPtr->data[2+i][14 - menuItemValueInitialPosition] = 'E';
              }
              else
              {
                frameBufferPtr->data[2+i][11 - menuItemValueInitialPosition] = 'F';                          
                frameBufferPtr->data[2+i][12 - menuItemValueInitialPosition] = 'A';
                frameBufferPtr->data[2+i][13 - menuItemValueInitialPosition] = 'L';
                frameBufferPtr->data[2+i][14 - menuItemValueInitialPosition] = 'S';
                frameBufferPtr->data[2+i][15 - menuItemValueInitialPosition] = 'E';
              }
              break;
            case HMI_MENU_ITEM_VALUE_TYPE_VERSION_NUMBER:
              frameBufferPtr->data[2+i][11 - menuItemValueInitialPosition] = 'V';
              frameBufferPtr->data[2+i][12 - menuItemValueInitialPosition] = menuItemValue.v.Major + '0';
              frameBufferPtr->data[2+i][13 - menuItemValueInitialPosition] = '.';
              frameBufferPtr->data[2+i][14 - menuItemValueInitialPosition] = menuItemValue.v.Minor + '0';                                                    
              break;
            default:
              break;
          }
          
          /* put [] beside focused menu item */
          if (HMIContext.selectedMenuItemId == menuItemIndex)
          {
            if (animationCount % 4 <= 1)
            {              
              frameBufferPtr->data[2+i][9] = '[';
              frameBufferPtr->data[2+i][15] = ']';
            }
          }
          else if (HMIContext.focusedMenuItemId == menuItemIndex)
          {
            if (animationCount % 4 <= 1)
            {              
              frameBufferPtr->data[2+i][0] = '[';
              frameBufferPtr->data[2+i][15] = ']'; 
            }
          }
        }
      }
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
  
  if (HMIContext.popupPtr)
  {
    for (i = 0; i < 4; ++i)
    {
      for (j = 0; j < 14; ++j)
      {
        frameBufferPtr->data[2+i][1+j] = HMIContext.popupPtr->text[i][j];
      }
    }
  }
  
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
  //static THMIMenu* cachedMenuPtr = (THMIMenu*)0x00;
  THMIPanel * panelPtr = HMIPanelLookupTable[HMIContext.currentPanelId];
  THMIKey key = HMI_GetKeyEvent();
  UINT8 i = 0;
  
  /* TODO: declare a function to increase idleCount */
  if (++HMIContext.idleTimeCount >= HMIContext.maxIdleTimeCount)
  {
    HMI_ResetIdleCount();
    HMI_ShowPanel(HMIContext.idlePanelId);
    panelPtr = HMIPanelLookupTable[HMIContext.currentPanelId];
  }
  
  if (HMIContext.popupPtr)
  {
    if (++HMIContext.popupTimeCount >= HMIContext.maxPopupTimeCount)
    {
      HMI_ClosePopup();
    }
  }
  
  if (panelPtr)
  {
    if (panelPtr->menuPtr)
    {
      //if (cachedMenuPtr != panelPtr->menuPtr)
      //{
      //  cachedMenuPtr = panelPtr->menuPtr; 
        if (panelPtr->menuPtr->type == HMI_MENU_TYPE_SETTING)
        {
          if (HMIContext.focusedMenuItemId == 0xFF)
          {
            HMIContext.focusedMenuItemId = panelPtr->menuPtr->startingMenuItemIndex;
          }
        }
        for (i = 0; i < 8; ++i)
        {
          if (panelPtr->menuPtr->itemPtr[i])
          {
            if (panelPtr->menuPtr->itemPtr[i]->updater)
            {
              if (!panelPtr->menuPtr->itemPtr[i]->useMutatedValue)
              {                
                panelPtr->menuPtr->itemPtr[i]->updater();
                panelPtr->menuPtr->itemPtr[i]->mutatedValue = panelPtr->menuPtr->itemPtr[i]->value;
              }
            }
          }
        }
      //}
    }
    
    if (!HMIPopupProcessRoutine())
    {      
      if (key != HMI_KEY_NULL)
      {      
        HMI_ResetIdleCount();
        if (panelPtr->inputHandler)
        {
          if (!panelPtr->inputHandler(key))
          {
            HMIPanelInputHandler(panelPtr, key);
          }
        }
        else
        {
          HMIPanelInputHandler(panelPtr, key);
        }
      }
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

void HMI_AppendPanel(THMIPanel * const aHMIPanel) 
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
  
  HMIContext.focusedMenuItemId = 0xFF;
  HMIContext.selectedMenuItemId = 0xFF;
  
  if (HMIPanelLookupTable[HMIContext.currentPanelId])
  {
    if (HMIPanelLookupTable[HMIContext.currentPanelId]->menuPtr)
    {
      HMIClearMenuItemMutatedValues(HMIPanelLookupTable[HMIContext.currentPanelId]->menuPtr);
    }
    if (HMIPanelLookupTable[HMIContext.currentPanelId]->updater)
    {      
      HMIPanelLookupTable[HMIContext.currentPanelId]->updater();
    }
  }
}

void HMI_ClosePanel(void) 
{
  if (HMIPanelLookupTable[HMIContext.currentPanelId]->menuPtr)
  {
    HMIClearMenuItemMutatedValues(HMIPanelLookupTable[HMIContext.currentPanelId]->menuPtr);
  }
 
  HMIContext.previousPanelId = HMIContext.currentPanelId;
  HMIContext.currentPanelId = HMIContext.parentPanelId;

  HMIContext.focusedMenuItemId = 0xFF;
  HMIContext.selectedMenuItemId = 0xFF;
}

void HMI_ResetIdleCount(void)
{
  HMIContext.idleTimeCount = 0;
}

UINT8 HMI_GetSelectedMenuItemIndex(void)
{
  return HMIContext.selectedMenuItemId;
}

void HMI_SetSelectedMenuItemIndex(UINT8 index)
{
  HMIContext.selectedMenuItemId = index;
}

void HMI_ClearSelectedMenuItemIndex(void)
{
  HMIContext.selectedMenuItemId = 0xFF;
}

UINT8 HMI_GetFocusedMenuItemIndex(void)
{
  return HMIContext.focusedMenuItemId;
}

void HMI_SetFocusedMenuItemIndex(UINT8 index)
{
  HMIContext.focusedMenuItemId = index;
}

void HMI_ClearFocusedMenuItemIndex(void)
{
  HMIContext.focusedMenuItemId = 0xFF;
}

void HMI_ShowPopup(THMIPopup* popupPtr)
{
  HMIContext.popupPtr = popupPtr;
  HMIContext.popupTimeCount = 0;
}

void HMI_ClosePopup(void)
{
  HMIContext.popupPtr = (THMIPopup*)0x0000;
  HMIContext.popupTimeCount = 0;
}

void HMI_SetBacklight(BOOL backlight) 
{
  UNUSED(LCD_Backlight(backlight));
}

void HMI_SetContrast(UINT8 contrast)
{
  UNUSED(LCD_SetContrast(contrast));
}
