#ifndef __L_KEYBRD_H__
#define __L_KEYBRD_H__
/*****************************************************************************
 * FILE:        L_Keybrd.h
 * DESCRIPTION: Keyboard module header file.
 *
 * See also the L_UIMsg.h keyboard messages.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Keybrd.h 6     7/19/99 2:12p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Keybrd.h $
 * 
 * 6     7/19/99 2:12p Agmsmith
 * Added compile time flag to turn off dangerous latched keyboard and
 * mouse buttons code (only use if you know what you are doing).
 *
 * 5     7/19/99 10:41a Agmsmith
 * Put back latched and pressed global key arrays.
 *
 * 3     7/12/99 11:25a Agmsmith
 * Removed latched keys feature in favour of UIMsg events, which have the
 * advantage of not losing keystrokes.
 ****************************************************************************/

/************************************************************/
/* STRUCTURES, ENUMS, ETC.                                  */
/*----------------------------------------------------------*/

typedef enum LE_KEYBRD_ScanCodesEnum
{
    LE_KEYBRD_NOKEY = 0, // No key for those null situations.
    LE_KEYBRD_A,
    LE_KEYBRD_B,
    LE_KEYBRD_C,
    LE_KEYBRD_D,
    LE_KEYBRD_E,
    LE_KEYBRD_F,
    LE_KEYBRD_G,
    LE_KEYBRD_H,
    LE_KEYBRD_I,
    LE_KEYBRD_J,
    LE_KEYBRD_K,
    LE_KEYBRD_L,
    LE_KEYBRD_M,
    LE_KEYBRD_N,
    LE_KEYBRD_O,
    LE_KEYBRD_P,
    LE_KEYBRD_Q,
    LE_KEYBRD_R,
    LE_KEYBRD_S,
    LE_KEYBRD_T,
    LE_KEYBRD_U,
    LE_KEYBRD_V,
    LE_KEYBRD_W,
    LE_KEYBRD_X,
    LE_KEYBRD_Y,
    LE_KEYBRD_Z,
    LE_KEYBRD_BACK,
    LE_KEYBRD_DEL,
    LE_KEYBRD_ESC,
    LE_KEYBRD_RET,
    LE_KEYBRD_SPACE,
    LE_KEYBRD_0,
    LE_KEYBRD_1,
    LE_KEYBRD_2,
    LE_KEYBRD_3,
    LE_KEYBRD_4,
    LE_KEYBRD_5,
    LE_KEYBRD_6,
    LE_KEYBRD_7,
    LE_KEYBRD_8,
    LE_KEYBRD_9,
    LE_KEYBRD_F1,
    LE_KEYBRD_F2,
    LE_KEYBRD_F3,
    LE_KEYBRD_F4,
    LE_KEYBRD_F5,
    LE_KEYBRD_F6,
    LE_KEYBRD_F7,
    LE_KEYBRD_F8,
    LE_KEYBRD_F9,
    LE_KEYBRD_F10,
    LE_KEYBRD_F11,
    LE_KEYBRD_F12,
    LE_KEYBRD_LEFT,
    LE_KEYBRD_UP,
    LE_KEYBRD_RIGHT,
    LE_KEYBRD_DOWN,
    LE_KEYBRD_INS,
    LE_KEYBRD_HOME,
    LE_KEYBRD_END,
    LE_KEYBRD_PAGEDOWN,
    LE_KEYBRD_PAGEUP,
    LE_KEYBRD_TAB,
    LE_KEYBRD_SHIFT,
    LE_KEYBRD_CONTROL,
    LE_KEYBRD_SCROLL_LOCK,
    LE_KEYBRD_HYPHEN,
    // Add more keys here as you need them.
    LE_KEYBRD_NUMKEYS
} LE_KEYBRD_ScanCodes;



/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_EnableSystemSequencer
extern BOOL LI_KEYBRD_ScrollLockPressedForSequencer;
extern BOOL LI_KEYBRD_ScrollLockLatchedForSequencer;
extern BOOL LI_KEYBRD_ScrollLockShiftForSequencer;
#endif



#if CE_ARTLIB_KeyboardPressedAndLatchedArray

extern volatile BYTE LE_KEYBRD_KeysLatched[LE_KEYBRD_NUMKEYS];
extern volatile BYTE LE_KEYBRD_KeysPressed[LE_KEYBRD_NUMKEYS];
// Booleans that are TRUE if the key is down/latched.

extern volatile ACHAR LE_KEYBRD_LastAscii;
// This variable is set to the last ASCII value when a key is pressed.  Note
// that you should really use the UIMSG key messages otherwise you will miss
// keys (if the user types faster than you check this variable, which happens
// annoyingly often). It is a good idea to set LE_KEYBRD_LastAscii to 0 after
// you are finished with it and therefore you will know when a new value is
// there when it is not zero.

extern volatile BOOL LE_KEYBRD_AnyKey;
// This variable is set when any key is pressed.  To use set to zero and
// poll until it is set to 1 (only works in multitasking mode - since if
// you sit there polling in single tasking mode, the game won't be
// processing Windows messages and won't see the key press).  Better to
// use the UIMSG key pressed message.

#endif // CE_ARTLIB_KeyboardPressedAndLatchedArray



/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

void LI_KEYBRD_RemoveSystem(void);
void LI_KEYBRD_InitSystem(void);
void LI_KEYBRD_UpdateKeybrd(UINT msg, WPARAM wParam, LPARAM lParam);

BYTE LE_KEYBRD_ConvertAsciiToOurCode(char ascii);
// Returns internal code for a given ASCII character, upper case required.



#if CE_ARTLIB_KeyboardPressedAndLatchedArray

extern BOOL LE_KEYBRD_CheckLatched (LE_KEYBRD_ScanCodes Scancode);
// Returns TRUE if the given key was down or pressed since the last
// time that key's latched value was cleared.

extern BOOL LE_KEYBRD_CheckPressed (LE_KEYBRD_ScanCodes Scancode);
// Returns TRUE if the given key is currently down or pressed.

extern void LE_KEYBRD_ClearLatchedKey (LE_KEYBRD_ScanCodes Scancode);
// Clears a specific key latched value.

extern void LE_KEYBRD_ClearAllLatched (void);
// Clears all the key latched values.

#endif // CE_ARTLIB_KeyboardPressedAndLatchedArray

#endif // __L_KEYBRD_H__
