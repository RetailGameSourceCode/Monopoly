#ifndef __L_JOYPAD_H__
#define __L_JOYPAD_H__

/*************************************************************
*
*   FILE:           l_joypad.h
*   DESCRIPTION:    Joypad system header.
*
*   (C) Copyright 1995/96 Artech Digital Entertainments.
*                         All rights reserved.
*
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/*----------------------------------------------------------*/

/************************************************************/

#if CE_ARTLIB_PlatformPSX

#define LI_JOYPAD_MAX_BUTTONS       32 // max of 16 for each joypad

#elif CE_ARTLIB_PlatformWin95

#define LI_JOYPAD_MAX_BUTTONS       10
#define ORIGIN                      32767

#endif

/************************************************************/
/* STRUCTURES, ENUMS, ETC.                                  */
/*----------------------------------------------------------*/

// Start Doc
// L_JOYPAD

#if CE_ARTLIB_PlatformPSX

typedef enum JoyPad
{
    JOY1_L2 = 0,		// 4 shoulder buttons
    JOY1_R2,
    JOY1_L1,
    JOY1_R1,
    JOY1_R_UP,			// triangle
    JOY1_R_RIGHT,		// circle
    JOY1_R_DOWN,		// x
    JOY1_R_LEFT,		// square
    JOY1_SELECT,
    JOY1_NOVALUEHERE1,	// no buttons corresponding to these 2 values
    JOY1_NOVALUEHERE2,
    JOY1_START,
    JOY1_L_UP,			// 4 arrow buttons
    JOY1_L_RIGHT,
    JOY1_L_DOWN,
    JOY1_L_LEFT,
    JOY2_L2,			// buttons for controller 2
    JOY2_R2,
    JOY2_L1,
    JOY2_R1,
    JOY2_R_UP,
    JOY2_R_RIGHT,
    JOY2_R_DOWN,
    JOY2_R_LEFT,
    JOY2_SELECT,
    JOY2_NOVALUEHERE1,
    JOY2_NOVALUEHERE2,
    JOY2_START,
    JOY2_L_UP,
    JOY2_L_RIGHT,
    JOY2_L_DOWN,
    JOY2_L_LEFT,
}JOY_PAD;

#elif CE_ARTLIB_PlatformWin95

typedef enum JoyPad
{
    JOYBUTTON1 = 0,
    JOYBUTTON2,
    JOYBUTTON3,
    JOYBUTTON4
}JOY_PAD;

#endif
// - Used to distinguish which button you are specifying when referring to the LE_JOYPAD_JoypadPressed and LE_JOYPAD_JoypadLatched arrays below.

// End Doc

/************************************************************/

/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

// Start Doc
#if CE_ARTLIB_PlatformWin95 // these position vars not supported by PSX

extern short LE_JOYPAD_JoypadX;
extern short LE_JOYPAD_JoypadY;
// - Global variables holding the current x and y location of the joypad.

extern short LE_JOYPAD_LatchedJoypadX;
extern short LE_JOYPAD_LatchedJoypadY;
// - Global variables holding the current x,y position for the last time any of the JOY_PAD buttons were pressed.

extern short LE_JOYPAD_JoypadDeltaX;
extern short LE_JOYPAD_JoypadDeltaY;
// - Global variables holding the x and y distance the joypad has moved.

#endif

extern char  LE_JOYPAD_JoypadPressed[LI_JOYPAD_MAX_BUTTONS];
extern char  LE_JOYPAD_JoypadLatched[LI_JOYPAD_MAX_BUTTONS];
// - Global variable holding the current pressed and latched status of the joypad buttons.
// - If the value is 1 the button was pressed or latched respectively.
extern char  LE_JOYPAD_JoypadReleased[LI_JOYPAD_MAX_BUTTONS];
// - Global variables set to TRUE when a button is released.

// End Doc
/************************************************************/

/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

void LI_JOYPAD_RemoveSystem(void);
void LI_JOYPAD_InitSystem(void);
#if CE_ARTLIB_PlatformPSX
void LE_JOYPAD_UpdateJoypad(void);
#elif CE_ARTLIB_PlatformWin95
void LI_JOYPAD_UpdateJoypad(UINT msg, WPARAM wParam, LPARAM lParam);
#endif

/************************************************************/

#endif // __L_JOYPAD_H__
