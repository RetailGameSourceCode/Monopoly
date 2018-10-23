/*****************************************************************************
 * FILE:        L_Mouse.cpp
 * DESCRIPTION: The Mouse user interface device for indicating position.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Mouse.cpp 6     7/19/99 2:12p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Mouse.cpp $
 * 
 * 6     7/19/99 2:12p Agmsmith
 * Added compile time flag to turn off dangerous latched keyboard and
 * mouse buttons code (only use if you know what you are doing).
 *
 * 5     7/19/99 10:43a Agmsmith
 * Mouse pressed and latched added back.
 *
 * 3     7/05/99 3:54p Agmsmith
 * Add an option to use world coordinates for the mouse instead of screen
 * coordinates.
 ****************************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemMouse

/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

short volatile LE_MOUSE_MouseX;
short volatile LE_MOUSE_MouseY;
short volatile LE_MOUSE_MouseDeltaX;
short volatile LE_MOUSE_MouseDeltaY;
INT8           LE_MOUSE_RenderSlotToUseForWorldCoordinates;

#if CE_ARTLIB_MousePressedAndLatchedArray
short volatile LE_MOUSE_LatchedMouseX;
short volatile LE_MOUSE_LatchedMouseY;
char  volatile LE_MOUSE_MousePressed[LI_MOUSE_NUM_BUTTONS];
char  volatile LE_MOUSE_MouseLatched[LI_MOUSE_NUM_BUTTONS];
#endif



/*************************************************************
*
* void LI_MOUSE_InitSystem(void);
*
*   Description:
*       MOUSE system initializion procedure.
*
*************************************************************/
void LI_MOUSE_InitSystem (void)
{
#if CE_ARTLIB_MousePressedAndLatchedArray

  int i;

  LE_MOUSE_LatchedMouseX = 0;
  LE_MOUSE_LatchedMouseY = 0;

  for (i = 0; i < LI_MOUSE_NUM_BUTTONS; i++)
  {
    LE_MOUSE_MousePressed[i] = 0;
    LE_MOUSE_MouseLatched[i] = 0;
  }

#endif // CE_ARTLIB_MousePressedAndLatchedArray

  LE_MOUSE_MouseX = 0;
  LE_MOUSE_MouseY = 0;
  LE_MOUSE_MouseDeltaX = 0;
  LE_MOUSE_MouseDeltaY = 0;
  LE_MOUSE_RenderSlotToUseForWorldCoordinates = -1;

  // Hide the Windows cursor.
  ShowCursor (FALSE);
}



/*************************************************************
*
* void LI_MOUSE_RemoveSystem(void);
*
*   Description:
*       MOUSE system cleanup and removal procedure.
*
*************************************************************/
void LI_MOUSE_RemoveSystem(void)
{
#if CE_ARTLIB_MousePressedAndLatchedArray

  int i;

  LE_MOUSE_LatchedMouseX   = 0;
  LE_MOUSE_LatchedMouseY   = 0;

  for (i = 0; i < LI_MOUSE_NUM_BUTTONS; i++)
  {
    LE_MOUSE_MousePressed[i] = 0;
    LE_MOUSE_MouseLatched[i] = 0;
  }

#endif // CE_ARTLIB_MousePressedAndLatchedArray

  LE_MOUSE_MouseX          = 0;
  LE_MOUSE_MouseY          = 0;
  LE_MOUSE_MouseDeltaX     = 0;
  LE_MOUSE_MouseDeltaY     = 0;
  LE_MOUSE_RenderSlotToUseForWorldCoordinates = -1;

  // Show the Windows cursor.
  ShowCursor (TRUE);
}



/*************************************************************
*
* void LI_MOUSE_UpdateMouse (UINT msg, LPARAM lparam)
*
*   Description:
*       Watches the windows mouse messages and updates the
*       system mouse variables.
*
*************************************************************/
void LI_MOUSE_UpdateMouse (UINT msg, LPARAM lParam)
{
  POINT ptNewMouse;
  LONG  DeltaX, DeltaY;

  // First update the mouse movement, which comes in all mouse messages.

  ptNewMouse.x = LOWORD (lParam);
  ptNewMouse.y = HIWORD (lParam);
  LE_GRAFIX_PointDisplayToLogical (&ptNewMouse);

#if CE_ARTLIB_EnableSystemRend2D
  if (LE_MOUSE_RenderSlotToUseForWorldCoordinates >= 0)
  {
    // Convert from screen coordinates to coordinates relative to a
    // particular render slot's world.  This undoes scaling and
    // camera rotations done by the slot.

    if (!LE_REND2D_ScreenToWorldCoordinates ((LE_REND_RenderSlot)
    LE_MOUSE_RenderSlotToUseForWorldCoordinates, &ptNewMouse, &ptNewMouse))
      LE_MOUSE_RenderSlotToUseForWorldCoordinates = -1; // Something wrong.
  }
#endif // CE_ARTLIB_EnableSystemRend2D

  DeltaX = ptNewMouse.x - LE_MOUSE_MouseX;
  DeltaY = ptNewMouse.y - LE_MOUSE_MouseY;
  if (DeltaX != 0 || DeltaY != 0)
  {
    LE_MOUSE_MouseDeltaX = (short) (LE_MOUSE_MouseDeltaX + DeltaX);
    LE_MOUSE_MouseDeltaY = (short) (LE_MOUSE_MouseDeltaY + DeltaY);
    LE_MOUSE_MouseX = (short) ptNewMouse.x;
    LE_MOUSE_MouseY = (short) ptNewMouse.y;

#if CE_ARTLIB_MousePositionMessages && CE_ARTLIB_EnableSystemUIMsg
    LE_UIMSG_SendEvent (UIMSG_MOUSE_MOVED,
      ptNewMouse.x, ptNewMouse.y, DeltaX, DeltaY, 0, NULL, 0);
#endif
  }

  switch (msg)
  {
  case WM_MOUSEMOVE:
    break; // Already done above.


  case WM_LBUTTONDOWN:
#if CE_ARTLIB_MousePressedAndLatchedArray
    LE_MOUSE_LatchedMouseX = LE_MOUSE_MouseX;
    LE_MOUSE_LatchedMouseY = LE_MOUSE_MouseY;
    LE_MOUSE_MousePressed[LEFT_BUTTON] = 1;
    LE_MOUSE_MouseLatched[LEFT_BUTTON] = 1;
#endif

#if CE_ARTLIB_EnableSystemUIMsg
    LE_UIMSG_SendEvent (UIMSG_MOUSE_LEFT_DOWN,
      LE_MOUSE_MouseX, LE_MOUSE_MouseY, DeltaX, DeltaY, 0, NULL, 0);
#endif // CE_ARTLIB_EnableSystemUIMsg
    break;


  case WM_LBUTTONUP:
#if CE_ARTLIB_MousePressedAndLatchedArray
    LE_MOUSE_MousePressed[LEFT_BUTTON] = 0;
#endif

#if CE_ARTLIB_EnableSystemUIMsg
    LE_UIMSG_SendEvent (UIMSG_MOUSE_LEFT_UP,
      LE_MOUSE_MouseX, LE_MOUSE_MouseY, DeltaX, DeltaY, 0, NULL, 0);
#endif // CE_ARTLIB_EnableSystemUIMsg
    break;


  case WM_MBUTTONDOWN:
#if CE_ARTLIB_MousePressedAndLatchedArray
    LE_MOUSE_LatchedMouseX = LE_MOUSE_MouseX;
    LE_MOUSE_LatchedMouseY = LE_MOUSE_MouseY;
    LE_MOUSE_MousePressed[CENTRE_BUTTON] = 1;
    LE_MOUSE_MouseLatched[CENTRE_BUTTON] = 1;
#endif

#if CE_ARTLIB_EnableSystemUIMsg
    LE_UIMSG_SendEvent (UIMSG_MOUSE_MIDDLE_DOWN,
      LE_MOUSE_MouseX, LE_MOUSE_MouseY, DeltaX, DeltaY, 0, NULL, 0);
#endif // CE_ARTLIB_EnableSystemUIMsg
    break;


  case WM_MBUTTONUP:
#if CE_ARTLIB_MousePressedAndLatchedArray
    LE_MOUSE_MousePressed[CENTRE_BUTTON] = 0;
#endif

#if CE_ARTLIB_EnableSystemUIMsg
    LE_UIMSG_SendEvent (UIMSG_MOUSE_MIDDLE_UP,
      LE_MOUSE_MouseX, LE_MOUSE_MouseY, DeltaX, DeltaY, 0, NULL, 0);
#endif // CE_ARTLIB_EnableSystemUIMsg
    break;


  case WM_RBUTTONDOWN:
#if CE_ARTLIB_MousePressedAndLatchedArray
    LE_MOUSE_LatchedMouseX = LE_MOUSE_MouseX;
    LE_MOUSE_LatchedMouseY = LE_MOUSE_MouseY;
    LE_MOUSE_MousePressed[RIGHT_BUTTON] = 1;
    LE_MOUSE_MouseLatched[RIGHT_BUTTON] = 1;
#endif

#if CE_ARTLIB_EnableSystemUIMsg
    LE_UIMSG_SendEvent (UIMSG_MOUSE_RIGHT_DOWN,
      LE_MOUSE_MouseX, LE_MOUSE_MouseY, DeltaX, DeltaY, 0, NULL, 0);
#endif // CE_ARTLIB_EnableSystemUIMsg
    break;


  case WM_RBUTTONUP:
#if CE_ARTLIB_MousePressedAndLatchedArray
    LE_MOUSE_MousePressed[RIGHT_BUTTON] = 0;
#endif

#if CE_ARTLIB_EnableSystemUIMsg
    LE_UIMSG_SendEvent (UIMSG_MOUSE_RIGHT_UP,
      LE_MOUSE_MouseX, LE_MOUSE_MouseY, DeltaX, DeltaY, 0, NULL, 0);
#endif // CE_ARTLIB_EnableSystemUIMsg
    break;
  }
}



#if CE_ARTLIB_MousePressedAndLatchedArray
/************************************************************/
/* LE_MOUSE_ClearAllLatched                                 */
/************************************************************/
void LE_MOUSE_ClearAllLatched (void)
{
  LE_MOUSE_MouseLatched[LEFT_BUTTON]   = 0;
  LE_MOUSE_MouseLatched[RIGHT_BUTTON]  = 0;
  LE_MOUSE_MouseLatched[CENTRE_BUTTON] = 0;
}
#endif // CE_ARTLIB_MousePressedAndLatchedArray



#if CE_ARTLIB_MousePressedAndLatchedArray
/************************************************************/
/* LE_MOUSE_ClearLatched                                    */
/************************************************************/
void LE_MOUSE_ClearLatched (MouseButtons MB)
{
  LE_MOUSE_MouseLatched[MB] = 0;
}
#endif // CE_ARTLIB_MousePressedAndLatchedArray

#endif // if CE_ARTLIB_EnableSystemMouse
