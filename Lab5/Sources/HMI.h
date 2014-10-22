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
<<<<<<< Updated upstream
  HMI_RENDERER_DIRTY,     /* redraw when screen is dirty */
  HMI_RENDERER_CONTINUITY /* redraw continuity */
=======
<<<<<<< HEAD
  HMI_RENDER_MODE_DIRTY,     /* redraw when screen is dirty */
  HMI_RENDER_MODE_CONTINUITY /* redraw continuity */
=======
  HMI_RENDERER_DIRTY,     /* redraw when screen is dirty */
  HMI_RENDERER_CONTINUITY /* redraw continuity */
>>>>>>> FETCH_HEAD
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream

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

=======
<<<<<<< HEAD

typedef void(*THMIInputHandler)(THMIKey);

/**
 * \brief HMI menu item
 */
typedef struct 
{
  UINT8 title[HMI_PANEL_TITLE_SIZE];
  UINT16 value;
} THMIMenuItem;

typedef struct
{
  THMIMenuItem items[16];
  UINT16 itemCount;
} THMIMenu;
=======

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

>>>>>>> Stashed changes
typedef struct
{
  UINT8 width;
  UINT8 height;
  THMISizePolicy sizePolicy;
  THMIWidgetType type;  
  void* extension;
  THMIInputHandler inputHandler;
} THMIWidget;
<<<<<<< Updated upstream
=======
>>>>>>> FETCH_HEAD
>>>>>>> Stashed changes

/**
 * \brief HMI panel
 */
typedef struct 
{
  UINT8 id;
  UINT8 title[HMI_PANEL_TITLE_SIZE];
  THMIInputHandler inputHandler;
<<<<<<< Updated upstream
  THMIWidget* centralWidget;
  void(*update)();
=======
<<<<<<< HEAD
  THMIMenu* menuPtr;
=======
  THMIWidget* centralWidget;
  void(*update)();
>>>>>>> FETCH_HEAD
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
=======
<<<<<<< HEAD
  
  THMIFrame* renderFrameBufferPtr;
  THMIFrame* screenFrameBufferPtr;
  
=======
>>>>>>> FETCH_HEAD
>>>>>>> Stashed changes
  UINT8 parentPanelId;
  UINT8 previousPanelId;
  UINT8 currentPanelId;
  UINT8 idlePanelId;
  
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
<<<<<<< Updated upstream
=======
<<<<<<< HEAD

/**
 * \fn BOOL HMI_RenderFrame(void)
 */
BOOL HMI_RenderFrame(void);

void HMI_SetTime(UINT16 hours, UINT16 minutes, UINT16 seconds);
=======
>>>>>>> Stashed changes

/**
 * \fn BOOL HMI_RenderFrame(void)
 */
BOOL HMI_RenderFrame(void);
<<<<<<< Updated upstream
=======
>>>>>>> FETCH_HEAD
>>>>>>> Stashed changes

void HMI_AppendPanel(const THMIPanel* const aHMIPanel);

void HMI_RemovePanel(const UINT8 panelId);

void HMI_ShowPanel(const UINT8 panelId);

void HMI_ClosePanel(void);

#endif
