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
  HMI_RENDERER_DIRTY,     /* redraw when screen is dirty */
  HMI_RENDERER_CONTINUITY /* redraw continuity */
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
 * \brief HMI widget
 */
 
typedef enum
{
  STATIC,
  LABEL,
  TIME,
  MENU,
  MENUITEM
} THMIWidgetType;

typedef enum
{
  STATIC_SIZE,
  FILL_PARENT
} THMISizePolicy;

typedef struct
{
  UINT8 width;
  UINT8 height;
  THMISizePolicy sizePolicy;
  THMIWidgetType type;  
  void* extension;
  THMIInputHandler inputHandler;
} THMIWidget;

/**
 * \brief HMI panel
 */
typedef struct 
{
  UINT8 id;
  UINT8 title[HMI_PANEL_TITLE_SIZE];
  THMIInputHandler inputHandler;
  THMIWidget* centralWidget;
  void(*update)();
} THMIPanel;

/**
 * \brief HMI context
 */
typedef struct 
{
  THMIRenderMode renderMode;
  THMIFrame frameTemplate;
  UINT8 parentPanelId;
  UINT8 previousPanelId;
  UINT8 currentPanelId;
  UINT8 idlePanelId;
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
BOOL HMI_Setup(const THMIContext* const aHMIContext);

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

void HMI_AppendPanel(const THMIPanel* const aHMIPanel);

void HMI_RemovePanel(const UINT8 panelId);

void HMI_ShowPanel(const UINT8 panelId);

void HMI_ClosePanel(void);

#endif
