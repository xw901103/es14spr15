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

typedef enum 
{
  HMI_BUTTON_NULL,
  HMI_BUTTON_SET,
  HMI_BUTTON_DATA,
  HMI_BUTTON_UP,
  HMI_BUTTON_DOWN,
  HMI_BUTTON_SELECT
} THMIButton;

/**
 * \brief HMI frame template
 */
typedef struct 
{
  UINT8* data;
} THMIFrameTemplate;

typedef void(*THMIInputHandler)(THMIButton);

/**
 * \brief HMI panel
 */
typedef struct 
{
  UINT8 id;
  UINT8 title[HMI_PANEL_TITLE_SIZE];
  THMIInputHandler inputHandler;
  UINT8* contents;
} THMIPanel;

/**
 * \brief HMI context
 */
typedef struct 
{
  UINT8 verticalCharCount;
  UINT8 horizentalCharCount;
  THMIFrameTemplate frameTemplate;
  UINT8 parentPanelId;
  UINT8 previousPanelId;
  UINT8 currentPanelId;
  UINT8 idlePanelId;
  UINT16 idleTimeCount;
  UINT8 fps;
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
 * \fn const THMIContext * HMI_GetContext()
 * \brief
 * \return Current HMI context
 * \warning Assumes HMI setup properly 
 */
const THMIContext * HMI_GetContext(void);

THMIButton HMI_GetButton(void);

void HMI_AppendPanel(const THMIPanel* const aHMIPanel);

void HMI_RemovePanel(const UINT8 panelId);

void HMI_ShowPanel(const UINT8 panelId);

void HMI_ClosePanel(void);

#endif
