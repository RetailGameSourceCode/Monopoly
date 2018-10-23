#ifndef __L_MOUSE_H__
#define __L_MOUSE_H__
/*****************************************************************************
 * FILE:        L_Mouse.h
 * DESCRIPTION: The Mouse user interface device for indicating position.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Mouse.h 6     7/19/99 2:12p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Mouse.h $
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
/* STRUCTURES, ENUMS, ETC.                                  */
/*----------------------------------------------------------*/

#if CE_ARTLIB_MousePressedAndLatchedArray

typedef enum MouseButtonsEnum
{
  LEFT_BUTTON = 0,
  CENTRE_BUTTON,
  RIGHT_BUTTON,
  LI_MOUSE_NUM_BUTTONS
} MouseButtons;
// Use these enums in LE_MOUSE_MousePressed and LE_MOUSE_MouseLatched arrays
// below for distinguishing various mouse buttons.

#endif // CE_ARTLIB_MousePressedAndLatchedArray



/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

extern short volatile LE_MOUSE_MouseX;
extern short volatile LE_MOUSE_MouseY;
// Global variable containing the mouse cursor current x and y screen
// location.  These should be treated as a read-only variables.

extern short volatile LE_MOUSE_MouseDeltaX;
extern short volatile LE_MOUSE_MouseDeltaY;
// Global variable containing the distance in the x and y direction the mouse
// cursor has moved.  Use this by setting the variable to zero and then read
// the current value when appropriate.

extern INT8 LE_MOUSE_RenderSlotToUseForWorldCoordinates;
// If this is set to -1 then the mouse uses screen coordinates.  If you
// set this to a render slot number, for a 2D slot, then the slot's
// inverse matrix is applied to the screen coordinates turning the
// mouse position into world coordinates.  That way you can scale
// and rotate the slot's camera and still have the mouse pointing
// somewhere reasonable.


#if CE_ARTLIB_MousePressedAndLatchedArray

extern short volatile LE_MOUSE_LatchedMouseX;
extern short volatile LE_MOUSE_LatchedMouseY;
// Global variable containing the x and y location for the mouse cursor the
// last time any button was latched.  These should be treated as read-only
// variables.  Clear this with the appropriate ClearLatched function below.
// Note that it is better to use the UIMSG mouse clicked messages since you
// may miss clicks if you don't poll these variables often enough, and they
// won't work at all if you busy wait in a single tasking game.

extern char volatile LE_MOUSE_MousePressed[LI_MOUSE_NUM_BUTTONS];
extern char volatile LE_MOUSE_MouseLatched[LI_MOUSE_NUM_BUTTONS];
// Global arrays used with the enum's above to test status of mouse buttons.

#endif // CE_ARTLIB_MousePressedAndLatchedArray



/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

void LI_MOUSE_RemoveSystem (void);
void LI_MOUSE_InitSystem (void);
void LI_MOUSE_UpdateMouse (UINT msg, LPARAM lparam);



#if CE_ARTLIB_MousePressedAndLatchedArray

void LE_MOUSE_ClearAllLatched (void);
// Clears latched status of all buttons.

void LE_MOUSE_ClearLatched (MouseButtons MB);
// Clears latched status of a specific button.

#endif // CE_ARTLIB_MousePressedAndLatchedArray

#endif // __L_MOUSE_H__
