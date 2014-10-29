/**
 * \file HMI.c
 * \brief Routines to implement human-machine interface
 * \author Xu Waycell
 * \date 19-9-2014
 */
#include "HMI.h"
#include "LCD.h"
#ifndef NO_INTERRUPT
#include "timer.h"
#endif
#include "utils.h"
#include <mc9s12a512.h>     

#define HMI_KEY1 PORTK_BIT2
#define HMI_KEY2 PORTK_BIT3
#define HMI_KEY3 PORTK_BIT4
#define HMI_KEY4 PORTK_BIT5
#define HMI_KEY5 PORTK_BIT7

#define HMI_POPUP_INITIAL_X 1
#define HMI_POPUP_INITIAL_Y 2

#define HMI_TIMER_INITIAL_X 8
#define HMI_TIMER_INITIAL_Y 0

#define HMI_PANEL_TITLE_INITIAL_X 0
#define HMI_PANEL_TITLE_INITIAL_Y 0

#define HMI_PANEL_INITIAL_X 0
#define HMI_PANEL_INITIAL_Y 2

#define HMI_PANEL_END_X 15
#define HMI_PANEL_END_Y 5

#define HMI_DIALOG_INITIAL_X 0
#define HMI_DIALOG_INITIAL_Y 2

#define HMI_MAXIMUM_ON_SCREEN_MENU_ITEM 4

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

/**
 * \fn void HMIRoutine(const TTimerChannel channelNb)
 * \brief Default HMI time driven interupt process routine
 * \param channelNb Indicate the parent timer channel
 */
void HMIRoutine(const TTimerChannel channelNb)
{
  static UINT16 count = 0; // 2ms every cycle
  //HMI_Poll();
    
  ++count;
  
  if (count == 49) // 10 fps or 100ms
  {
    count = 0;

    HMI_Poll();
    
    if (HMIContext.renderMode == HMI_RENDER_MODE_CONTINUITY || HMIContext.isDirty)
    {
      // TODO: replace unused with a debug macro
      UNUSED(HMI_RenderFrame()); 
    }
  }

  Timer_ScheduleRoutine(channelNb, HMIRoutinePeriod);
}
#endif

/**
 * \fn BOOL HMIPopupProcessRoutine(void)
 * \brief Default HMI popup process routine
 * \return TRUE if popup related process has been done or FALSE when bypassed
 */
BOOL HMIPopupProcessRoutine(void)
{
  static BOOL pressedKey2 = bFALSE;

  /* TODO: connect variables to ModConHMIBacklight and ModConHMIContrast */
  
  BOOL showBacklight = bFALSE, showContrast = bFALSE;
  
  if (HMI_KEY5)
  {
    if (HMI_KEY2 && !pressedKey2)
    {      
      HMIContext.backlight = !HMIContext.backlight;
      
      showBacklight = bTRUE;
    }
    else if (HMI_KEY3)
    {
      if (HMIContext.contrast > 0)
      {        
        --HMIContext.contrast;
      }
      showContrast = bTRUE;
    }
    else if (HMI_KEY4)
    {
      if (HMIContext.contrast < 63)
      {
        ++HMIContext.contrast;
      }
      showContrast = bTRUE;
    }
  }

  pressedKey2 = HMI_KEY2;
      
  if (showBacklight)
  {
    if (HMIContext.backlight)
    {      
      CopyBytes(&HMI_BACKLIGHT_POPUP.text[2][6], (UINT8*)" ON", 3);      
    }
    else
    {
      CopyBytes(&HMI_BACKLIGHT_POPUP.text[2][6], (UINT8*)"OFF", 3);      
    }
    LCD_Backlight(HMIContext.backlight);
    HMI_ShowPopup(&HMI_BACKLIGHT_POPUP);
    
    if (HMIContext.backlightChangedCallback)
    {
      HMIContext.backlightChangedCallback(HMIContext.backlight);
    }
    
    return bTRUE;
  }
  else if (showContrast)
  {
    HMI_CONTRAST_POPUP.text[2][6] = HMIContext.contrast / 10 + '0';
    HMI_CONTRAST_POPUP.text[2][7] = HMIContext.contrast % 10 + '0';    
    UNUSED(LCD_SetContrast(HMIContext.contrast));
    HMI_ShowPopup(&HMI_CONTRAST_POPUP);
    
    if (HMIContext.contrastChangedCallback)
    {
      HMIContext.contrastChangedCallback(HMIContext.contrast);
    }
    
    return bTRUE;
  }
    
  return bFALSE;
}

/**
 * \fn void HMIPanelInputProcessRoutine(THMIPanel* const panelPtr, THMIKey key)
 * \brief Default HMI input process routine
 * \param panelPtr current focused panel
 * \param key current pressed key
 */
void HMIPanelInputProcessRoutine(THMIPanel* const panelPtr, THMIKey key)
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
            if ((panelPtr->menuPtr->itemCount - panelPtr->menuPtr->startingMenuItemIndex) > HMI_MAXIMUM_ON_SCREEN_MENU_ITEM)
            {              
              panelPtr->menuPtr->startingMenuItemIndex++;
            }
          }        
          else if (selectedMenuItemIndex != focusedMenuItemIndex)
          {          
            if (focusedMenuItemIndex < (panelPtr->menuPtr->itemCount - 1)) /* from 0 to n-1 total n items*/
            {
              if (++focusedMenuItemIndex >= (panelPtr->menuPtr->startingMenuItemIndex + HMI_MAXIMUM_ON_SCREEN_MENU_ITEM))
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
          else if (panelPtr->menuPtr->itemPtr[focusedMenuItemIndex]->type == HMI_MENU_ITEM_TYPE_ENTRY)
          {              
            HMI_SetSelectedMenuItemIndex(focusedMenuItemIndex);
          }
          else if(panelPtr->menuPtr->itemPtr[focusedMenuItemIndex]->type == HMI_MENU_ITEM_TYPE_ACTION)
          {
            if (panelPtr->menuPtr->itemPtr[focusedMenuItemIndex]->actionRoutine)
            {
              panelPtr->menuPtr->itemPtr[focusedMenuItemIndex]->actionRoutine(panelPtr->menuPtr->itemPtr[focusedMenuItemIndex]);
            }
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

/**
 * \fn void HMIClearMenuItemMutatedValues(THMIMenu* const menuPtr)
 * \brief Clears useMutatedValue of all menu items of given menu.
 * \param menuPtr A pointer of THMIMenu containing menu items
 */
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
 * \fn HMI_Setup(const THMISetup * const aHMISetup)
 * \brief Setup HMI system.
 * \param aHMIContext A pointer of HMI configuration structure
 * \return TRUE if HMI is set properly or FALSE when failed
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
      
      HMIContext.oldHours = 0;
      HMIContext.oldMinutes = 0;
      HMIContext.oldSeconds = 0;      

      HMIContext.hours = 0;
      HMIContext.minutes = 0;
      HMIContext.seconds = 0;
      
      HMIContext.backlight = aHMISetup->backlight;
      HMIContext.backlightChangedCallback = aHMISetup->backlightChangedCallback;
      HMIContext.contrast = aHMISetup->contrast;
      HMIContext.contrastChangedCallback = aHMISetup->contrastChangedCallback;      
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

/**
 * \fn const THMIContext * const HMI_GetContext(void)
 * \brief Gets the current using HMI context
 * \return A pointer of current using HMI context
 * \warning Assumes HMI setup properly 
 */
const THMIContext * const HMI_GetContext(void)
{
  return &HMIContext;
}

/**
 * \fn THMIKey HMI_GetKeyEvent(void)
 * \brief Gets current pressed key
 * \return the key which has been pressed by user
 */
THMIKey HMI_GetKeyEvent(void)
{
  static BOOL pressedKey1 = bFALSE, pressedKey2 = bFALSE, pressedKey3 = bFALSE, pressedKey4 = bFALSE, pressedKey5 = bFALSE;

  if (HMI_KEY1)
  {
    if (!pressedKey1)
    {      
      pressedKey1 = bTRUE;
      return HMI_KEY_SET;
    }
  }
  else
  {    
    pressedKey1 = bFALSE;
  }
  
  if (HMI_KEY2)
  {
    if (!pressedKey2)
    {      
      pressedKey2 = bTRUE;
      return HMI_KEY_DATA;
    }
  }
  else
  {    
    pressedKey2 = bFALSE;
  }
  
  if (HMI_KEY3)
  {
    if (!pressedKey3)
    {      
      pressedKey3 = bTRUE;
      return HMI_KEY_UP;
    }
  }
  else
  {    
    pressedKey3 = bFALSE;
  }
  
  if (HMI_KEY4)
  {
    if (!pressedKey4)
    {      
      pressedKey4 = bTRUE;
      return HMI_KEY_DOWN; 
    }
  }
  else
  {    
    pressedKey4 = bFALSE;
  }
  
  if (HMI_KEY5)
  {
    if (!pressedKey5)
    {      
      pressedKey5 = bTRUE;
      return HMI_KEY_SELECT;
    }
  }
  else
  {    
    pressedKey5 = bFALSE;
  }
  
  return HMI_KEY_NULL;
}

/**
 * \fn void HMIRenderPopup(THMIFrame * const frameBufferPtr)
 * \brief Renders a popup on given framebuffer
 * \param frameBufferPtr Targeted rendering framebuffer
 */
void HMIRenderPopup(THMIFrame * const frameBufferPtr)
{
  UINT16 y = 0, x = 0;
  
  if (frameBufferPtr)
  {    
    if (HMIContext.popupPtr)
    {
      for (y = 0; y < HMI_POPUP_MAXIMUM_HEIGHT; ++y)
      {
        for (x = 0; x < HMI_POPUP_MAXIMUM_WIDTH; ++x)
        {
          frameBufferPtr->data[HMI_POPUP_INITIAL_Y + y][HMI_POPUP_INITIAL_X + x] = HMIContext.popupPtr->text[y][x];
        }
      }
    }
  } 
  else 
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif  
  }
}

/**
 * \fn void HMIRenderTimer(THMIFrame * const frameBufferPtr)
 * \brief Renders timer to targeted framebuffer
 * \param frameBufferPtr framebuffer to use
 */
void HMIRenderTimer(THMIFrame * const frameBufferPtr)
{
  UINT16 i = 0, j = 0;
  
  if (frameBufferPtr)
  {
    /*        89ABCDEF */
    /* format HH:MM:SS */
    frameBufferPtr->data[HMI_TIMER_INITIAL_Y][HMI_TIMER_INITIAL_X + 7] = HMIContext.seconds % 10 + '0';
    frameBufferPtr->data[HMI_TIMER_INITIAL_Y][HMI_TIMER_INITIAL_X + 6] = HMIContext.seconds / 10 + '0';
    
    frameBufferPtr->data[HMI_TIMER_INITIAL_Y][HMI_TIMER_INITIAL_X + 4] = HMIContext.minutes % 10 + '0';
    frameBufferPtr->data[HMI_TIMER_INITIAL_Y][HMI_TIMER_INITIAL_X + 3] = HMIContext.minutes / 10 + '0';    

    frameBufferPtr->data[HMI_TIMER_INITIAL_Y][HMI_TIMER_INITIAL_X + 1] = HMIContext.hours % 10 + '0';
    frameBufferPtr->data[HMI_TIMER_INITIAL_Y][HMI_TIMER_INITIAL_X] = HMIContext.hours / 10 + '0';
  }
  else
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif  
  }
}

/**
 * \fn void HMIRenderPanel(THMIFrame * const frameBufferPtr, const THMIPanel* const panelPtr)
 * \brief Renders given panel to targeted framebuffer
 * \param frameBufferPtr framebuffer to use
 * \param panelPtr panel to render
 */
void HMIRenderPanel(THMIFrame * const frameBufferPtr, const THMIPanel* const panelPtr)
{
  static UINT8 animationCount = 0; /* TODO: move them into HMIContext */
  UINT16 i = 0, j = 0, menuItemIndex = 0, menuItemTitleInitialPosition = 0, menuItemValueInitialPosition = 0;
  THMIMenuItemValue menuItemValue;
  THMIMenuItemValueNotation menuItemValueNotation = HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT;

  ++animationCount; /* TODO: move it to HMIContext */

  if (frameBufferPtr && panelPtr)
  {  
    for (i = 0; i < HMI_PANEL_TITLE_SIZE; ++i)
    {
      frameBufferPtr->data[HMI_PANEL_TITLE_INITIAL_Y][HMI_PANEL_TITLE_INITIAL_X + i] = panelPtr->title[i];
    }
    
    if (panelPtr->dialogPtr)
    {
      /* y coordinate */
      for (i = 0; i < HMI_DIALOG_MAXIMUM_HEIGHT; ++i)
      {
        /* x coordinate */
        for (j = 0; j < HMI_DIALOG_MAXIMUM_WIDTH; ++j)
        {
          frameBufferPtr->data[HMI_DIALOG_INITIAL_Y + i][HMI_DIALOG_INITIAL_X + j] = panelPtr->dialogPtr->text[i][j];
        }
      }
    }
    else if (panelPtr->menuPtr)
    {
      switch(panelPtr->menuPtr->type)
      {
        /* TODO: move them to configurable defines */
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
      
      /* rendering menu items */
      for(i = 0; i < HMI_MAXIMUM_ON_SCREEN_MENU_ITEM; ++i)
      {
        menuItemIndex = panelPtr->menuPtr->startingMenuItemIndex + i;
        
        if (panelPtr->menuPtr->itemPtr[menuItemIndex])
        {
          /* render menu item title */
          /* NOTE: text left align */
          for (j = 0; j < HMI_MENU_ITEM_TITLE_SIZE; ++j)
          {
            /* TODO: use configurable setting for panel area starting row */
            if (panelPtr->menuPtr->itemPtr[menuItemIndex]->title[j] != '\0')
            {              
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_INITIAL_X + menuItemTitleInitialPosition + j] = panelPtr->menuPtr->itemPtr[menuItemIndex]->title[j];
            }
          }
          
          /* determine rendering menu item value */
          if (panelPtr->menuPtr->itemPtr[menuItemIndex]->useMutatedValue)
          {
            menuItemValue = panelPtr->menuPtr->itemPtr[menuItemIndex]->mutatedValue;
          }
          else
          {
            menuItemValue = panelPtr->menuPtr->itemPtr[menuItemIndex]->value;
          }
          
          menuItemValueNotation = panelPtr->menuPtr->itemPtr[menuItemIndex]->valueNotation;
          
          /* rendering menu item value */
          /* NOTE: text right align */
          switch(panelPtr->menuPtr->itemPtr[menuItemIndex]->valueType)
          {
            case HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN:
              switch (menuItemValueNotation)
              {
                case HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF:
                  if (menuItemValue.b.Boolean)
                  {
                    CopyBytes(&frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 1 - menuItemValueInitialPosition], (const UINT8*)"ON", 2);
                  }
                  else
                  {
                    CopyBytes(&frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 2 - menuItemValueInitialPosition], (const UINT8*)"OFF", 3);
                  }                
                  break;
                case HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_SYNC_ASYNC:
                  if (menuItemValue.b.Boolean)
                  {
                    CopyBytes(&frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 3 - menuItemValueInitialPosition], (const UINT8*)"SYNC", 4);
                  }
                  else
                  {
                    CopyBytes(&frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 4 - menuItemValueInitialPosition], (const UINT8*)"ASYNC", 5);
                  }                
                  break;
                default:
                  if (menuItemValue.b.Boolean)
                  {
                    CopyBytes(&frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 3 - menuItemValueInitialPosition], (const UINT8*)"TRUE", 4);
                  }
                  else
                  {
                    CopyBytes(&frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 4 - menuItemValueInitialPosition], (const UINT8*)"FALSE", 5);
                  }                
                  break;
              }
              break;
            case HMI_MENU_ITEM_VALUE_TYPE_VERSION_NUMBER:
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 4 - menuItemValueInitialPosition] = 'V';
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 3 - menuItemValueInitialPosition] = menuItemValue.v.Major + '0';
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 2 - menuItemValueInitialPosition] = '.';
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 1 - menuItemValueInitialPosition] = menuItemValue.v.Minor / 10 + '0';
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - menuItemValueInitialPosition] = (menuItemValue.v.Minor % 10) + '0';                                                    
              break;
            case HMI_MENU_ITEM_VALUE_TYPE_FLOAT:
              if (menuItemValue.f.Float < 0)
              {
                frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 4 - menuItemValueInitialPosition] = '-';
                menuItemValue.f.Float = -menuItemValue.f.Float;
              }
              else
              {
                //frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 4 - menuItemValueInitialPosition] = '+';
              }
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - menuItemValueInitialPosition] = (menuItemValue.f.Float % 10) + '0';
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 1 - menuItemValueInitialPosition] = (menuItemValue.f.Float % 100) / 10 + '0';
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 2 - menuItemValueInitialPosition] = '.';
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 3 - menuItemValueInitialPosition] = (menuItemValue.f.Float % 1000) / 100 + '0';
              break;                                                           
            case HMI_MENU_ITEM_VALUE_TYPE_NONE:
              break;
            case HMI_MENU_ITEM_VALUE_TYPE_UNSIGNED_INTEGER:              
            default:
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][15 - menuItemValueInitialPosition] = (menuItemValue.l % 10) + '0';
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][14 - menuItemValueInitialPosition] = (menuItemValue.l % 100) / 10 + '0';
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][13 - menuItemValueInitialPosition] = (menuItemValue.l % 1000) / 100 + '0';
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][12 - menuItemValueInitialPosition] = (menuItemValue.l % 10000) /1000 + '0';
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][11 - menuItemValueInitialPosition] = (menuItemValue.l % 100000) /10000 + '0';            
              break;
          }
          
          /* put [] beside focused menu item */
          if (HMIContext.selectedMenuItemId == menuItemIndex)
          {
            if (animationCount % 4 <= 1)
            {
              /* NOTE: menu item value is right align */              
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X - 6] = '[';
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X] = ']';
            }
          }
          else if (HMIContext.focusedMenuItemId == menuItemIndex)
          {
            if (animationCount % 4 <= 1)
            {              
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_INITIAL_X] = '[';
              frameBufferPtr->data[HMI_PANEL_INITIAL_Y + i][HMI_PANEL_END_X] = ']'; 
            }
          }
          
        } /* if itemPtr valid */
      } /* for loop iterate menu items */
    } /* if menuPtr valid */
  } /* if panelPtr valid */
  else
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif
  }
}

/**
 * \fn BOOL HMI_RenderFrame(void)
 * \brief Renders a frame based on current shown panel, menu and/or dialog.
 * \return TRUE if the frame has been sent to display or FALSE it failed.
 */
BOOL HMI_RenderFrame(void)
{
  static UINT8 timerTick = ' ';
   
  THMIFrame * const frameBufferPtr = HMIContext.renderFrameBufferPtr;
  const THMIPanel * panelPtr = HMIPanelLookupTable[HMIContext.currentPanelId];
  UINT8 x = 0, y = 0, menuItemIndex = 0, menuItemTitleInitialPosition = 0, menuItemValueInitialPosition = 0;
  
  /* rendering bottom layer */    
  for (y = 0; y < HMIContext.frameTemplate.height; ++y)
  {
    for (x = 0; x < HMIContext.frameTemplate.width; ++x)
    {
      frameBufferPtr->data[y][x] = HMIContext.frameTemplate.data[y][x];
    }
  }
  
  frameBufferPtr->width = HMIContext.frameTemplate.width;
  frameBufferPtr->height = HMIContext.frameTemplate.height;
  
  HMIRenderPanel(frameBufferPtr, panelPtr);  
  HMIRenderTimer(frameBufferPtr);

  /* TODO: move timer column update out of this function */
  /* update time notation by compare on screen framebuffer */
  if (frameBufferPtr->data[HMI_TIMER_INITIAL_Y][HMI_TIMER_INITIAL_X + 7] != HMIContext.screenFrameBufferPtr->data[HMI_TIMER_INITIAL_Y][HMI_TIMER_INITIAL_X + 7])
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

  frameBufferPtr->data[HMI_TIMER_INITIAL_Y][HMI_TIMER_INITIAL_X + 5] = timerTick;    
  frameBufferPtr->data[HMI_TIMER_INITIAL_Y][HMI_TIMER_INITIAL_X + 2] = timerTick;
  
  HMIRenderPopup(frameBufferPtr);
    
  if (LCD_OutFrame(frameBufferPtr->data))
  {
    HMIContext.renderFrameBufferPtr = HMIContext.screenFrameBufferPtr;    
    HMIContext.screenFrameBufferPtr = frameBufferPtr;
    
    HMIContext.isDirty = bFALSE; /* screen is up to date */

    return bTRUE;
  }
  return bFALSE;
}

/**
 * \fn void HMI_Poll(void)
 * \brief Polls update of shown HMI components.
 */
void HMI_Poll(void) 
{
  //static THMIMenu* cachedMenuPtr = (THMIMenu*)0x00;
  THMIPanel * panelPtr = HMIPanelLookupTable[HMIContext.currentPanelId];
  THMIMenuItem* menuItemPtr = (THMIMenuItem*)0x0000;
  THMIKey key = HMI_GetKeyEvent();
  UINT8 i = 0;
  
  /* TODO: declare a function to increase idleCount */
  if (++HMIContext.idleTimeCount >= HMIContext.maxIdleTimeCount)
  {
    //if (panelPtr && panelPtr->menuPtr)
    //{
    //  HMIClearMenuItemMutatedValues(panelPtr->menuPtr);
    //}
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
      if (panelPtr->menuPtr->type == HMI_MENU_TYPE_SETTING)
      {
        if (HMIContext.focusedMenuItemId == 0xFF)
        {
          HMIContext.focusedMenuItemId = panelPtr->menuPtr->startingMenuItemIndex;
        }
      }
      
      for (i = 0; i < panelPtr->menuPtr->itemCount; ++i)
      {
        menuItemPtr = panelPtr->menuPtr->itemPtr[i + panelPtr->menuPtr->startingMenuItemIndex];
        
        if (menuItemPtr && menuItemPtr->type == HMI_MENU_ITEM_TYPE_ENTRY)
        {
          if (menuItemPtr->updateRoutine)
          {
            if (!menuItemPtr->useMutatedValue)
            {                
              menuItemPtr->updateRoutine(menuItemPtr);
              menuItemPtr->mutatedValue = menuItemPtr->value;

              HMI_MarkDirty();
            }
          }
        }
      }
    }
    
    if (!HMIPopupProcessRoutine())
    {      
      if (key != HMI_KEY_NULL)
      {      
        HMI_ResetIdleCount();
        if (panelPtr->inputProcessRoutine)
        {
          if (!panelPtr->inputProcessRoutine(panelPtr, key))
          {
            HMIPanelInputProcessRoutine(panelPtr, key);
          }
        }
        else
        {
          HMIPanelInputProcessRoutine(panelPtr, key);
        }
      }
    }
  }
  else
  {
    /* key processed for popup */
    HMI_ResetIdleCount();
  }
}

/**
 * \fn void HMI_SetTime(UINT16 hours, UINT16 minutes, UINT16 seconds)
 * \brief Sets HMI timer time
 * \param hours
 * \param minutes
 * \param seconds
 */
void HMI_SetTime(UINT16 hours, UINT16 minutes, UINT16 seconds)
{
  HMIContext.oldHours = HMIContext.hours;
  HMIContext.oldMinutes = HMIContext.minutes;
  HMIContext.oldSeconds = HMIContext.seconds;
  
  HMIContext.hours = hours;
  HMIContext.minutes = minutes;
  HMIContext.seconds = seconds;

  HMI_MarkDirty();
}

/**
 * \fn void HMI_AppendPanel(THMIPanel* const aHMIPanel)
 * \brief add a panel to HMI panel list
 * \param aHMIPanel A pointer of THMIPanel
 */
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

/**
 * \fn void HMI_RemovePanel(const UINT8 panelId)
 * \brief remove a panel from HMI panel list
 * \param panelId array index of THMIPanel in panel list
 */
void HMI_RemovePanel(const UINT8 panelId)
{
  HMIPanelLookupTable[panelId] = (const THMIPanel *)0x0000;
}

/**
 * \fn void HMI_ShowPanel(const UINT8 panelId)
 * \brief Exposes a panel on screen
 * \param popupPtr array index of THMIPanel in panel list
 */
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
    if (HMIPanelLookupTable[HMIContext.currentPanelId]->updateRoutine)
    {      
      HMIPanelLookupTable[HMIContext.currentPanelId]->updateRoutine(HMIPanelLookupTable[HMIContext.currentPanelId]);
    }
  }

  HMI_MarkDirty();
}

/**
 * \fn void HMI_ClosePanel(void)
 * \brief Hides shown panel
 */
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

  HMI_MarkDirty();
}

/**
 * \fn void HMI_ResetIdleCount(void)
 * \brief Resets idle timeout counter to zero.
 */
void HMI_ResetIdleCount(void)
{
  HMIContext.idleTimeCount = 0;
}

/**
 * \fn void HMI_MarkDirty(void)
 * \brief Marks screen that needs to update.
 */
void HMI_MarkDirty(void)
{
  HMIContext.isDirty = bTRUE;
}

/**
 * \fn UINT8 HMI_GetSelectedMenuItemIndex(void)
 * \brief Gets current selected menu item index
 * \return the selected menu item index in menu item list of focused menu
 */
UINT8 HMI_GetSelectedMenuItemIndex(void)
{
  return HMIContext.selectedMenuItemId;
}

/**
 * \fn void HMI_SetSelectedMenuItemIndex(UINT8 index)
 * \brief Sets current selected menu item index
 * \param index the menu item index in menu item list of focused menu
 */
void HMI_SetSelectedMenuItemIndex(UINT8 index)
{
  HMIContext.selectedMenuItemId = index;
  HMI_MarkDirty();
}

/**
 * \fn void HMI_ClearSelectedMenuItemIndex(void)
 * \brief Resets current selected menu item index to zero
 */
void HMI_ClearSelectedMenuItemIndex(void)
{
  HMIContext.selectedMenuItemId = 0xFF;
  HMI_MarkDirty();
}

/**
 * \fn UINT8 HMI_GetFocusedMenuItemIndex(void)
 * \brief Gets current focused menu item index
 * \return the focused menu item index in menu item list of focused menu
 */
UINT8 HMI_GetFocusedMenuItemIndex(void)
{
  return HMIContext.focusedMenuItemId;
}

/**
 * \fn void HMI_SetFocusedMenuItemIndex(UINT8 index)
 * \brief Sets current focused menu item index
 * \param index the menu item index in menu item list of focused menu
 */
void HMI_SetFocusedMenuItemIndex(UINT8 index)
{
  HMIContext.focusedMenuItemId = index;
  HMI_MarkDirty();
}

/**
 * \fn void HMI_ClearFocusedMenuItemIndex(void)
 * \brief Resets current focused menu item index to zero
 */
void HMI_ClearFocusedMenuItemIndex(void)
{
  HMIContext.focusedMenuItemId = 0xFF;
  HMI_MarkDirty();
}

/**
 * \fn void HMI_ShowPopup(THMIPopup* popupPtr)
 * \brief Exposes a popup on screen
 * \param popupPtr A pointer of THMIPopup
 */
void HMI_ShowPopup(THMIPopup* popupPtr)
{
  HMIContext.popupPtr = popupPtr;
  HMIContext.popupTimeCount = 0;
  HMI_MarkDirty();
}

/**
 * \fn void HMI_ClosePopup(void)
 * \brief Hides shown popup
 */
void HMI_ClosePopup(void)
{
  HMIContext.popupPtr = (THMIPopup*)0x0000;
  HMIContext.popupTimeCount = 0;
  HMI_MarkDirty();
}

/**
 * \fn void HMI_SetBacklight(BOOL backlight)
 * \brief Sets the display backlight on or off
 * \param backlight whether the backlight is on or off 
 */
void HMI_SetBacklight(BOOL backlight) 
{
  UNUSED(LCD_Backlight(backlight));
}

/**
 * \fn void HMI_SetContrast(UINT8 contrast)
 * \brief Sets the contrast of the display
 * \param contrast contrast a value between 0 and 63
 */
void HMI_SetContrast(UINT8 contrast)
{
  UNUSED(LCD_SetContrast(contrast));
}
