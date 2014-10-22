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

typedef void(*THMIInputHandler)(THMIKey);

/**
 * \brief HMI menu item
 */
typedef struct 
{
  UINT8 title[16];
  UINT16 value;
} THMIMenuItem;

typedef struct
{
  THMIMenuItem* itemPtr[8];
} THMIMenu;

/**
 * \brief HMI panel
 */
typedef struct 
{
  UINT8 id;
  UINT8 title[HMI_PANEL_TITLE_SIZE];
  THMIInputHandler inputHandler;
  THMIMenu* menuPtr;
} THMIPanel;

/**
 * \brief HMI setup
 */
typedef struct
{
  THMIRenderMode renderMode;
  THMIFrame frameTemplate;  
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
  UINT8 beginningMenuItemId;
   
  UINT16 seconds;
  UINT16 minutes;
  UINT16 hours;
  
  UINT16 idleTimeCount;
  
  UINT8 fps;
  BOOL isDirty; // bool for determine the screen needs to redraw
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

void HMI_AppendPanel(const THMIPanel* const aHMIPanel);

void HMI_RemovePanel(const UINT8 panelId);

void HMI_ShowPanel(const UINT8 panelId);

void HMI_ClosePanel(void);

#endif
