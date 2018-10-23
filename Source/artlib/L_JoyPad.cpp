/*************************************************************
*
*   FILE:           L_JOYPAD.C
*   DESCRIPTION:    Main joypad system source file.
*
*   (C) Copyright 1995/96 Artech Digital Entertainments.
*                         All rights reserved.
*
*************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "l_inc.h"
/************************************************************/

#if CE_ARTLIB_EnableSystemJoypad

/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

#if CE_ARTLIB_PlatformWin95 // these variables are not supportable for PSX
short LE_JOYPAD_JoypadX;
short LE_JOYPAD_JoypadY;
short LE_JOYPAD_LatchedJoypadX;
short LE_JOYPAD_LatchedJoypadY;
short LE_JOYPAD_JoypadDeltaX;
short LE_JOYPAD_JoypadDeltaY;
#endif

char  LE_JOYPAD_JoypadPressed[LI_JOYPAD_MAX_BUTTONS];
char  LE_JOYPAD_JoypadLatched[LI_JOYPAD_MAX_BUTTONS];
char  LE_JOYPAD_JoypadReleased[LI_JOYPAD_MAX_BUTTONS];

/************************************************************/

/************************************************************/
/* STATIC VARIABLES                                         */
/*----------------------------------------------------------*/

/************************************************************/

/*************************************************************
*
* void LI_JOYPAD_InitSystem(void);
*
*   Description:
*       JOYPAD system initializion procedure.
*
*   Arguments:
*       None.
*
*************************************************************/
void LI_JOYPAD_InitSystem(void)
{
    short i;

#if CE_ARTLIB_PlatformPSX
	PadInit(0); // only controller type 0 is supported by PSX library
#else if CE_ARTLIB_PlatformWin95
    LE_JOYPAD_JoypadX          = 0;
    LE_JOYPAD_JoypadY          = 0;
    LE_JOYPAD_LatchedJoypadX   = 0;
    LE_JOYPAD_LatchedJoypadY   = 0;
    LE_JOYPAD_JoypadDeltaX     = 0;
    LE_JOYPAD_JoypadDeltaY     = 0;
#endif

    for (i = 0; i < LI_JOYPAD_MAX_BUTTONS; i++)
    {
        LE_JOYPAD_JoypadPressed[i] = 0;
        LE_JOYPAD_JoypadLatched[i] = 0;
        LE_JOYPAD_JoypadReleased[i] = 0;
    }
}
/************************************************************/

/*************************************************************
*
* void LI_JOYPAD_RemoveSystem(void);
*
*   Description:
*       JOYPAD system cleanup and removal procedure.
*
*************************************************************/
void LI_JOYPAD_RemoveSystem(void)
{
    short i;

#if CE_ARTLIB_PlatformPSX
	PadStop();
#else if CE_ARTLIB_PlatformWin95
    LE_JOYPAD_JoypadX          = 0;
    LE_JOYPAD_JoypadY          = 0;
    LE_JOYPAD_LatchedJoypadX   = 0;
    LE_JOYPAD_LatchedJoypadY   = 0;
    LE_JOYPAD_JoypadDeltaX     = 0;
    LE_JOYPAD_JoypadDeltaY     = 0;
#endif
    for (i = 0; i < LI_JOYPAD_MAX_BUTTONS; i++)
    {
        LE_JOYPAD_JoypadPressed[i] = 0;
        LE_JOYPAD_JoypadLatched[i] = 0;
        LE_JOYPAD_JoypadReleased[i] = 0;
    }
}
/************************************************************/

/************************************************************/
/* LI_JOYPAD_UpdateJoypad                                   */
/************************************************************/
#if CE_ARTLIB_PlatformWin95

void LI_JOYPAD_UpdateJoypad(UINT msg, WPARAM wParam, LPARAM lParam)
{
    unsigned short NewJoypadX, NewJoypadY;

    switch (msg)
    {
        case MM_JOY1MOVE:
            NewJoypadX = LOWORD(lParam) - ORIGIN;
            NewJoypadY = HIWORD(lParam) - ORIGIN;
            LE_JOYPAD_JoypadDeltaX += (NewJoypadX - LE_JOYPAD_JoypadX);
            LE_JOYPAD_JoypadDeltaY += (NewJoypadY - LE_JOYPAD_JoypadY);
            LE_JOYPAD_JoypadX = NewJoypadX;
            LE_JOYPAD_JoypadY = NewJoypadY;
            break;

        case MM_JOY1BUTTONDOWN:
            LE_JOYPAD_LatchedJoypadX = LOWORD(lParam) - ORIGIN;
            LE_JOYPAD_LatchedJoypadY = HIWORD(lParam) - ORIGIN;
            switch (wParam)
            {
                case JOY_BUTTON1:
                    LE_JOYPAD_JoypadPressed[JOYBUTTON1] = 1;
                    LE_JOYPAD_JoypadLatched[JOYBUTTON1] = 1;
                    break;

                case JOY_BUTTON2:
                    LE_JOYPAD_JoypadPressed[JOYBUTTON2] = 1;
                    LE_JOYPAD_JoypadLatched[JOYBUTTON2] = 1;
                    break;

                case JOY_BUTTON3:
                    LE_JOYPAD_JoypadPressed[JOYBUTTON3] = 1;
                    LE_JOYPAD_JoypadLatched[JOYBUTTON3] = 1;
                    break;

                case JOY_BUTTON4:
                    LE_JOYPAD_JoypadPressed[JOYBUTTON4] = 1;
                    LE_JOYPAD_JoypadLatched[JOYBUTTON4] = 1;
                    break;
            }
            break;

        case MM_JOY1BUTTONUP:
            switch (wParam)
            {
                case JOY_BUTTON1:
                    LE_JOYPAD_JoypadPressed[JOYBUTTON1] = 0;
                    LE_JOYPAD_JoypadReleased[JOYBUTTON1] = 1;
                    break;

                case JOY_BUTTON2:
                    LE_JOYPAD_JoypadPressed[JOYBUTTON2] = 0;
                    LE_JOYPAD_JoypadReleased[JOYBUTTON2] = 1;
                    break;

                case JOY_BUTTON3:
                    LE_JOYPAD_JoypadPressed[JOYBUTTON3] = 0;
                    LE_JOYPAD_JoypadReleased[JOYBUTTON3] = 1;
                    break;

                case JOY_BUTTON4:
                    LE_JOYPAD_JoypadPressed[JOYBUTTON4] = 0;
                    LE_JOYPAD_JoypadReleased[JOYBUTTON4] = 1;
                    break;
            }
            break;
    }
}

#else if CE_ARTLIB_PlatformPSX

void LE_JOYPAD_UpdateJoypad(void)
{
	short i;
	unsigned long PadValue = PadRead(0); // parameter to PadRead() has no meaning

	// check each bit in the pad button mask
    for (i = 0; i < LI_JOYPAD_MAX_BUTTONS; i++)
    {	
		if (!(PadValue & (1 << i))) // button is not pressed
		{
			if (LE_JOYPAD_JoypadPressed[i])
				LE_JOYPAD_JoypadReleased[i] = 1; // set the released flag if just entered unpressed state

			LE_JOYPAD_JoypadPressed[i] = 0; // clear the pressed flag
		}
		else // button is pressed
		{
			if (!LE_JOYPAD_JoypadPressed[i])
				LE_JOYPAD_JoypadLatched[i] = 1; // set if just entered pressed state

			LE_JOYPAD_JoypadPressed[i] = 1; // set the pressed flag
		}
    }
}

#endif
/************************************************************/

#endif // if CE_ARTLIB_EnableSystemJoypad