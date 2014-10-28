/**
 * \fn HMI.h
 * \brief Routines to implement human-machine interface
 * \author Xu Waycell
 * \date 
 */
#ifndef HMI_H
#define HMI_H

#include "global.h"

#define HMI_PANEL_TITLE_SIZE 7
#define HMI_MENU_ITEM_TITLE_SIZE 16
#define HMI_PANEL_SIZE 256
#define HMI_FRAME_MAXIMUM_HEIGHT 8
#define HMI_FRAME_MAXIMUM_WIDTH  16

#define HMI_DIALOG_MAXIMUM_HEIGHT 4
#define HMI_DIALOG_MAXIMUM_WIDTH 16

#define HMI_MENU_ITEM_SIZE 16

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
  HMI_RENDER_MODE_CONTINUITY /* redraw continuity */
} THMIRenderMode;

/**
 * \brief HMI frame template
 */
typedef struct 
{
  UINT8 data[HMI_FRAME_MAXIMUM_HEIGHT][HMI_FRAME_MAXIMUM_WIDTH];
  UINT16 width;
  UINT16 height;
} THMIFrame;

typedef struct HMIMenuItemSTR THMIMenuItem;
typedef struct HMIPanelSTR THMIPanel;

typedef void(*THMIMenuItemUpdateRoutine)(THMIMenuItem*);
typedef void(*THMIMenuItemActionRoutine)(THMIMenuItem*);
typedef BOOL(*THMIPanelInputProcessRoutine)(THMIPanel*, THMIKey);
typedef void(*THMIPanelUpdateRoutine)(THMIPanel*);

typedef struct 
{
  UINT8 title[16];
  UINT8 text[4][14];
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
  HMI_MENU_ITEM_VALUE_TYPE_NONE,
  HMI_MENU_ITEM_VALUE_TYPE_UNSIGNED_INTEGER,
  HMI_MENU_ITEM_VALUE_TYPE_FLOAT, /* +- XX.XX */
  HMI_MENU_ITEM_VALUE_TYPE_VERSION_NUMBER,
  HMI_MENU_ITEM_VALUE_TYPE_BOOLEAN
} THMIMenuItemValueType;

typedef enum
{
  HMI_MENU_ITEM_VALUE_NOTATION_DEFAULT = 0,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_ON_OFF,
  HMI_MENU_ITEM_VALUE_NOTATION_BOOLEAN_SYNC_ASYNC
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
    UINT8;
    UINT8 Boolean;    
  } b;
  struct
  {
    UINT8 Major;
    UINT8 Minor;
  } v;
  struct
  {
    INT16 Float;
  } f;
} THMIMenuItemValue;

/**
 * \brief HMI menu item
 */
struct HMIMenuItemSTR 
{
  UINT8 title[16];
  THMIMenuItemType type;
  UINT16 attribute; /* user defined attribute */
  THMIMenuItemValue value; /* TODO: use type THMIMenuItemValue */
  THMIMenuItemValue minimumValue;
  THMIMenuItemValue maximumValue;
  THMIMenuItemValue mutatedValue;
  BOOL useMutatedValue; /* TODO: add mutable boolean indicator */  
  THMIMenuItemValueType valueType;
  THMIMenuItemValueNotation valueNotation;
  THMIMenuItemUpdateRoutine updateRoutine;
  THMIMenuItemActionRoutine actionRoutine;
};

typedef enum
{
  HMI_MENU_TYPE_STATIC,
  HMI_MENU_TYPE_SETTING
} THMIMenuType;

typedef struct
{
  THMIMenuType type;
  THMIMenuItem* itemPtr[HMI_MENU_ITEM_SIZE];
  UINT8 itemCount;
  UINT8 startingMenuItemIndex;
} THMIMenu;

/**
 * \brief HMI panel
 */
struct HMIPanelSTR
{
  /* TODO: add panel type */
  UINT8 id;
  UINT8 title[HMI_PANEL_TITLE_SIZE];
  THMIPanelInputProcessRoutine inputProcessRoutine;
  THMIPanelUpdateRoutine updater;
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
  UINT16 contrast;  
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
  
  //THMIMenuItem* focusedMenuItemPtr;
  //THMIMenuItem* selectedMenuItemPtr;
   
  UINT8 parentPanelId;
  UINT8 previousPanelId;
  UINT8 currentPanelId;
  UINT8 idlePanelId;
 
  /* TODO: think a better way to resolve menu scrolling */
  UINT8 selectedMenuItemId;
  UINT8 focusedMenuItemId;
  
  UINT8 beginningMenuItemId;
   
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
  
} THMIContext;

/**
 * \fn void HMI_Setup(const THMIContext* const aHMIContext)
 * \brief
 * \param aHMIContext
 * \return
 */
BOOL HMI_Setup(const THMISetup* const aHMISetup);

void HMI_Poll(void);

/**
 * \fn const THMIContext * HMI_GetContext(void)
 * \brief
 * \return Current HMI context
 * \warning Assumes HMI setup properly 
 */
const THMIContext * const HMI_GetContext(void);

THMIKey HMI_GetKeyEvent(void);

/**
 * \fn BOOL HMI_RenderFrame(void)
 */
BOOL HMI_RenderFrame(void);

void HMI_SetTime(UINT16 hours, UINT16 minutes, UINT16 seconds);

void HMI_AppendPanel(THMIPanel* const aHMIPanel);

void HMI_RemovePanel(const UINT8 panelId);

void HMI_ShowPanel(const UINT8 panelId);

void HMI_ClosePanel(void);

void HMI_ShowPopup(THMIPopup*);
void HMI_ClosePopup(void);

void HMI_ResetIdleCount(void);

UINT8 HMI_GetSelectedMenuItemIndex(void);
void HMI_SetSelectedMenuItemIndex(UINT8 index);
void HMI_ClearSelectedMenuItemIndex(void);

UINT8 HMI_GetFocusedMenuItemIndex(void);
void HMI_SetFocusedMenuItemIndex(UINT8 index);
void HMI_ClearFocusedMenuItemIndex(void);

void HMI_SetBacklight(BOOL backlight);
void HMI_SetContrast(UINT8 contrast);

#endif
