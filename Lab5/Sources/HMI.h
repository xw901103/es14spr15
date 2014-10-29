/**
 * \file HMI.h
 * \brief Routines to implement human-machine interface
 * \author Xu Waycell
 * \date 19-9-2014
 */
#ifndef HMI_H
#define HMI_H

#include "global.h"
#include "LCD.h"

#define HMI_PANEL_TITLE_SIZE      7
#define HMI_MENU_ITEM_TITLE_SIZE  16
#define HMI_PANEL_SIZE            256

#define HMI_FRAME_MAXIMUM_HEIGHT  LCD_TEXT_SIZE_Y
#define HMI_FRAME_MAXIMUM_WIDTH   LCD_TEXT_SIZE_X

#define HMI_DIALOG_MAXIMUM_HEIGHT 4
#define HMI_DIALOG_MAXIMUM_WIDTH  HMI_FRAME_MAXIMUM_WIDTH

#define HMI_POPUP_TITLE_SIZE      16
#define HMI_POPUP_MAXIMUM_HEIGHT  4
#define HMI_POPUP_MAXIMUM_WIDTH   14

#define HMI_MENU_ITEM_SIZE        16

/**
 * \brief HMI buttons
 */
typedef enum 
{
  HMI_KEY_NULL,
  HMI_KEY_SET,
  HMI_KEY_DATA,
  HMI_KEY_UP,
  HMI_KEY_DOWN,
  HMI_KEY_SELECT
} THMIKey;

/**
 * \brief HMI render mode
 */
typedef enum
{
  HMI_RENDER_MODE_DIRTY,     /* redraw when screen is dirty */
  HMI_RENDER_MODE_CONTINUITY /* redraw continuity           */
} THMIRenderMode;

/**
 * \brief HMI frame template
 */
typedef struct 
{
  UINT8 data[HMI_FRAME_MAXIMUM_HEIGHT][HMI_FRAME_MAXIMUM_WIDTH];

  /* NOTE: usable area size might be different to frame buffer size */
  UINT16 width;
  UINT16 height;
} THMIFrame;

typedef struct HMIMenuItemSTR THMIMenuItem;
typedef struct HMIPanelSTR THMIPanel;

typedef void(*THMIMenuItemUpdateRoutine)(THMIMenuItem*);
typedef void(*THMIMenuItemActionRoutine)(THMIMenuItem*);
typedef BOOL(*THMIPanelInputProcessRoutine)(THMIPanel*, THMIKey);
typedef void(*THMIPanelUpdateRoutine)(THMIPanel*);

/* this will be called once backlight has changed through HMI popup */
typedef void(*THMIBacklightChangedCallback)(BOOL); /* TODO: rename it */
/* this will be called once contrast has changed through HMI popup */
typedef void(*THMIContrastChangedCallback)(UINT8);  /* TODO: rename it */

typedef struct 
{
  UINT8 title[HMI_POPUP_TITLE_SIZE];
  UINT8 text[HMI_POPUP_MAXIMUM_HEIGHT][HMI_POPUP_MAXIMUM_WIDTH];
} THMIPopup;

typedef struct
{
  UINT8 text[HMI_DIALOG_MAXIMUM_HEIGHT][HMI_DIALOG_MAXIMUM_WIDTH];
} THMIDialog;

typedef enum
{
  HMI_MENU_ITEM_TYPE_STATIC,
  HMI_MENU_ITEM_TYPE_ENTRY, /* can be selected and value can be mutated */
  HMI_MENU_ITEM_TYPE_ACTION /* can be selected and its action routine will be executed */
} THMIMenuItemType;

typedef enum 
{
  HMI_MENU_ITEM_VALUE_TYPE_NONE,             /* determines the value is no purpose  */
  HMI_MENU_ITEM_VALUE_TYPE_UNSIGNED_INTEGER, /* in notation of  XXXXX               */
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT,            /* in notation of  -X.XX               */
  HMI_MENU_ITEM_VALUE_TYPE_VERSION_NUMBER,   /* in notation of  VX.XX               */
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN           /* boolean type has multiple notations */
} THMIMenuItemValueType;

typedef enum
{
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT = 0,       /* TRUE when true, FALSE when false */
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,    /* ON when true, OFF when false     */
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_SYNC_ASYNC /* SYNC when true, ASYNC when false */
} THMIMenuItemValueNotation;

typedef union
{
  UINT16 l;
  struct
  {
    UINT8 Hi;
    UINT8 Lo;
  } s;
  struct
  {
    UINT8;         /* this byte has no use for boolean */
    UINT8 Boolean;    
  } b;
  struct
  {
    /* it will be represent in VMajor.Minor(VX.XX) */
    UINT8 Major;
    UINT8 Minor;
  } v;
  struct
  {
    /* e.g. -114 will be represent as -1.14 */
    INT16 Float;
  } f;
} THMIMenuItemValue;

/**
 * \brief HMI menu item
 */
struct HMIMenuItemSTR 
{
  UINT8 title[16];                         /* menu item title */
  THMIMenuItemType type;                   /* menu item type. this could be static, entry or action */
  UINT16 attribute;                        /* user defined attribute */
  THMIMenuItemValue value;                 /* item value when the type is entry */
  THMIMenuItemValue minimumValue;          /* minimum value */
  THMIMenuItemValue maximumValue;          /* maximum value */
  THMIMenuItemValue mutatedValue;          /* mutated value of the menu item */
  BOOL useMutatedValue;                    /* use muated value instead of original */  
  THMIMenuItemValueType valueType;         /* menu item value type */
  THMIMenuItemValueNotation valueNotation; /* this determines how the HMI display the value */
  THMIMenuItemUpdateRoutine updateRoutine; /* routine to poll new value */
  THMIMenuItemActionRoutine actionRoutine; /* routine for action type menu item */
};

typedef enum
{
  HMI_MENU_TYPE_STATIC, /* static menu will be read only */
  HMI_MENU_TYPE_SETTING /* values of menu item of setting type menu can be mutated */
} THMIMenuType;

typedef struct
{
  THMIMenuType type;                         /* menu type. this could be static or setting */
  THMIMenuItem* itemPtr[HMI_MENU_ITEM_SIZE]; /* menu item array */
  UINT8 itemCount;                           /* count of menu items in the array */
  UINT8 startingMenuItemIndex;               /* TODO: move this to THMIContext */
} THMIMenu;

/**
 * \brief HMI panel
 */
struct HMIPanelSTR
{
  /* TODO: add panel type */
  UINT8 id; /* TODO: remove this unused attribute */
  UINT8 title[HMI_PANEL_TITLE_SIZE];                /* title of panel */
  THMIPanelInputProcessRoutine inputProcessRoutine; /* routine for button input processing */
  THMIPanelUpdateRoutine updateRoutine;             /* routine to update panel */
  THMIMenu* menuPtr;
  THMIDialog* dialogPtr;
} THMIPanel;

/**
 * \brief HMI setup
 */
typedef struct
{
  THMIRenderMode renderMode;
  THMIFrame frameTemplate;
  UINT8 idlePanelId;
  UINT16 maxIdleTimeCount;
  
  BOOL backlight;
  THMIBacklightChangedCallback backlightChangedCallback;
  UINT8 contrast;  
  THMIContrastChangedCallback contrastChangedCallback;
} THMISetup;

/**
 * \brief HMI context
 */
typedef struct 
{
  THMIRenderMode renderMode;
  THMIFrame frameTemplate;
  
  THMIFrame* renderFrameBufferPtr;
  THMIFrame* screenFrameBufferPtr;
     
  UINT8 parentPanelId;
  UINT8 previousPanelId;
  UINT8 currentPanelId;
  UINT8 idlePanelId;
 
  /* TODO: think a better way to resolve menu scrolling */
  UINT8 selectedMenuItemId;
  UINT8 focusedMenuItemId;

  //THMIMenuItem* focusedMenuItemPtr;
  //THMIMenuItem* selectedMenuItemPtr;
  
  //UINT8 beginningMenuItemId;
   
  UINT16 oldSeconds;
  UINT16 oldMinutes;
  UINT16 oldHours;

  UINT16 seconds;
  UINT16 minutes;
  UINT16 hours;
  
  UINT16 maxIdleTimeCount;
  UINT16 idleTimeCount;
  
  UINT8 fps;
  BOOL isDirty; // bool for determine the screen needs to redraw

  BOOL isFocusedOnMenuItem;
  UINT8 focusedMenuItemIndex;
  
  THMIPopup* popupPtr;
  UINT16 maxPopupTimeCount;
  UINT16 popupTimeCount;
  
  BOOL backlight;
  THMIBacklightChangedCallback backlightChangedCallback;
  UINT8 contrast;  
  THMIContrastChangedCallback contrastChangedCallback;
  
} THMIContext;

/**
 * \fn void HMI_Setup(const THMIContext* const aHMIContext)
 * \brief
 * \param aHMIContext
 * \return
 */
BOOL HMI_Setup(const THMISetup* const aHMISetup);

/**
 * \fn void HMI_Poll(void)
 * \brief Polls update of shown HMI components.
 */
void HMI_Poll(void);

/**
 * \fn const THMIContext * const HMI_GetContext(void)
 * \brief Gets the current using HMI context
 * \return A pointer of current using HMI context
 * \warning Assumes HMI setup properly 
 */
const THMIContext * const HMI_GetContext(void);

/**
 * \fn THMIKey HMI_GetKeyEvent(void)
 * \brief Gets current pressed key
 * \return the key which has been pressed by user
 */
THMIKey HMI_GetKeyEvent(void);

/**
 * \fn BOOL HMI_RenderFrame(void)
 * \brief Renders a frame based on current shown panel, menu and/or dialog.
 * \return TRUE if the frame has been sent to display or FALSE it failed.
 */
BOOL HMI_RenderFrame(void);

/**
 * \fn void HMI_SetTime(UINT16 hours, UINT16 minutes, UINT16 seconds)
 * \brief Sets HMI timer time
 * \param hours
 * \param minutes
 * \param seconds
 */
void HMI_SetTime(UINT16 hours, UINT16 minutes, UINT16 seconds);

/**
 * \fn void HMI_AppendPanel(THMIPanel* const aHMIPanel)
 * \brief add a panel to HMI panel list
 * \param aHMIPanel A pointer of THMIPanel
 */
void HMI_AppendPanel(THMIPanel* const aHMIPanel);

/**
 * \fn void HMI_RemovePanel(const UINT8 panelId)
 * \brief remove a panel from HMI panel list
 * \param panelId array index of THMIPanel in panel list
 */
void HMI_RemovePanel(const UINT8 panelId);

/**
 * \fn void HMI_ShowPanel(const UINT8 panelId)
 * \brief Exposes a panel on screen
 * \param popupPtr array index of THMIPanel in panel list
 */
void HMI_ShowPanel(const UINT8 panelId);

/**
 * \fn void HMI_ClosePanel(void)
 * \brief Hides shown panel
 */
void HMI_ClosePanel(void);

/**
 * \fn void HMI_ShowPopup(THMIPopup* popupPtr)
 * \brief Exposes a popup on screen
 * \param popupPtr A pointer of THMIPopup
 */
void HMI_ShowPopup(THMIPopup*);

/**
 * \fn void HMI_ClosePopup(void)
 * \brief Hides shown popup
 */
void HMI_ClosePopup(void);

/**
 * \fn void HMI_ResetIdleCount(void)
 * \brief Resets idle timeout counter to zero.
 */
void HMI_ResetIdleCount(void);

/**
 * \fn void HMI_MarkDirty(void)
 * \brief Marks screen that needs to update.
 */
void HMI_MarkDirty(void);

/**
 * \fn UINT8 HMI_GetSelectedMenuItemIndex(void)
 * \brief Gets current selected menu item index
 * \return the selected menu item index in menu item list of focused menu
 */
UINT8 HMI_GetSelectedMenuItemIndex(void);

/**
 * \fn void HMI_SetSelectedMenuItemIndex(UINT8 index)
 * \brief Sets current selected menu item index
 * \param index the menu item index in menu item list of focused menu
 */
void HMI_SetSelectedMenuItemIndex(UINT8 index);

/**
 * \fn void HMI_ClearSelectedMenuItemIndex(void)
 * \brief Resets current selected menu item index to zero
 */
void HMI_ClearSelectedMenuItemIndex(void);

/**
 * \fn UINT8 HMI_GetFocusedMenuItemIndex(void)
 * \brief Gets current focused menu item index
 * \return the focused menu item index in menu item list of focused menu
 */
UINT8 HMI_GetFocusedMenuItemIndex(void);

/**
 * \fn void HMI_SetFocusedMenuItemIndex(UINT8 index)
 * \brief Sets current focused menu item index
 * \param index the menu item index in menu item list of focused menu
 */
void HMI_SetFocusedMenuItemIndex(UINT8 index);

/**
 * \fn void HMI_ClearFocusedMenuItemIndex(void)
 * \brief Resets current focused menu item index to zero
 */
void HMI_ClearFocusedMenuItemIndex(void);

/**
 * \fn void HMI_SetBacklight(BOOL backlight)
 * \brief Sets the display backlight on or off
 * \param backlight whether the backlight is on or off 
 */
void HMI_SetBacklight(BOOL backlight);

/**
 * \fn void HMI_SetContrast(UINT8 contrast)
 * \brief Sets the contrast of the display
 * \param contrast contrast a value between 0 and 63
 */
void HMI_SetContrast(UINT8 contrast);

#endif
