/*****************************************************************************
 * FILE:        L_Keybrd.cpp
 * DESCRIPTION: Keyboard module implementation.
 *
 * See also the L_UIMsg.h keyboard messages.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Keybrd.cpp 8     99-10-15 1:18p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Keybrd.cpp $
 * 
 * 8     99-10-15 1:18p Agmsmith
 * Avoid sign extension problem with high characters, and we don't do
 * Unicode for the keyboard.
 * 
 * 7     7/19/99 3:27p Agmsmith
 * Oops.
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
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemKeybrd



/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

#if CE_ARTLIB_KeyboardPressedAndLatchedArray
BYTE  volatile LE_KEYBRD_KeysLatched[LE_KEYBRD_NUMKEYS];
BYTE  volatile LE_KEYBRD_KeysPressed[LE_KEYBRD_NUMKEYS];
BOOL  volatile LE_KEYBRD_AnyKey;
ACHAR volatile LE_KEYBRD_LastAscii;
#endif

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_EnableSystemSequencer
BOOL LI_KEYBRD_ScrollLockPressedForSequencer;
BOOL LI_KEYBRD_ScrollLockLatchedForSequencer;
BOOL LI_KEYBRD_ScrollLockShiftForSequencer;
#endif



/************************************************************/
/* STATIC VARIABLES                                         */
/*----------------------------------------------------------*/

static const LE_KEYBRD_ScanCodes WinVirtualKeyToOurCode [256] =
{
  /*                   0x00 */ LE_KEYBRD_NOKEY,
  /* VK_LBUTTON        0x01 */ LE_KEYBRD_NOKEY,
  /* VK_RBUTTON        0x02 */ LE_KEYBRD_NOKEY,
  /* VK_CANCEL         0x03 */ LE_KEYBRD_NOKEY,
  /* VK_MBUTTON        0x04 */ LE_KEYBRD_NOKEY,
  /*                   0x05 */ LE_KEYBRD_NOKEY,
  /*                   0x06 */ LE_KEYBRD_NOKEY,
  /*                   0x07 */ LE_KEYBRD_NOKEY,
  /* VK_BACK           0x08 */ LE_KEYBRD_BACK,
  /* VK_TAB            0x09 */ LE_KEYBRD_TAB,
  /*                   0x0A */ LE_KEYBRD_NOKEY,
  /*                   0x0B */ LE_KEYBRD_NOKEY,
  /* VK_CLEAR          0x0C */ LE_KEYBRD_NOKEY,
  /* VK_RETURN         0x0D */ LE_KEYBRD_RET,
  /*                   0x0E */ LE_KEYBRD_NOKEY,
  /*                   0x0F */ LE_KEYBRD_NOKEY,
  /* VK_SHIFT          0x10 */ LE_KEYBRD_SHIFT,
  /* VK_CONTROL        0x11 */ LE_KEYBRD_CONTROL,
  /* VK_MENU           0x12 */ LE_KEYBRD_NOKEY,
  /* VK_PAUSE          0x13 */ LE_KEYBRD_NOKEY,
  /* VK_CAPITAL        0x14 */ LE_KEYBRD_NOKEY,
  /*                   0x15 */ LE_KEYBRD_NOKEY,
  /*                   0x16 */ LE_KEYBRD_NOKEY,
  /*                   0x17 */ LE_KEYBRD_NOKEY,
  /*                   0x18 */ LE_KEYBRD_NOKEY,
  /*                   0x19 */ LE_KEYBRD_NOKEY,
  /*                   0x1A */ LE_KEYBRD_NOKEY,
  /* VK_ESCAPE         0x1B */ LE_KEYBRD_ESC,
  /*                   0x1C */ LE_KEYBRD_NOKEY,
  /*                   0x1D */ LE_KEYBRD_NOKEY,
  /*                   0x1E */ LE_KEYBRD_NOKEY,
  /*                   0x1F */ LE_KEYBRD_NOKEY,
  /* VK_SPACE          0x20 */ LE_KEYBRD_SPACE,
  /* VK_PRIOR          0x21 */ LE_KEYBRD_PAGEUP,
  /* VK_NEXT           0x22 */ LE_KEYBRD_PAGEDOWN,
  /* VK_END            0x23 */ LE_KEYBRD_END,
  /* VK_HOME           0x24 */ LE_KEYBRD_HOME,
  /* VK_LEFT           0x25 */ LE_KEYBRD_LEFT,
  /* VK_UP             0x26 */ LE_KEYBRD_UP,
  /* VK_RIGHT          0x27 */ LE_KEYBRD_RIGHT,
  /* VK_DOWN           0x28 */ LE_KEYBRD_DOWN,
  /* VK_SELECT         0x29 */ LE_KEYBRD_NOKEY,
  /* VK_PRINT          0x2A */ LE_KEYBRD_NOKEY,
  /* VK_EXECUTE        0x2B */ LE_KEYBRD_NOKEY,
  /* VK_SNAPSHOT       0x2C */ LE_KEYBRD_NOKEY,
  /* VK_INSERT         0x2D */ LE_KEYBRD_INS,
  /* VK_DELETE         0x2E */ LE_KEYBRD_DEL,
  /* VK_HELP           0x2F */ LE_KEYBRD_NOKEY,
  /* VK_0              0x30 */ LE_KEYBRD_0,
  /* VK_1              0x31 */ LE_KEYBRD_1,
  /* VK_2              0x32 */ LE_KEYBRD_2,
  /* VK_3              0x33 */ LE_KEYBRD_3,
  /* VK_4              0x34 */ LE_KEYBRD_4,
  /* VK_5              0x35 */ LE_KEYBRD_5,
  /* VK_6              0x36 */ LE_KEYBRD_6,
  /* VK_7              0x37 */ LE_KEYBRD_7,
  /* VK_8              0x38 */ LE_KEYBRD_8,
  /* VK_9              0x39 */ LE_KEYBRD_9,
  /*                   0x3A */ LE_KEYBRD_NOKEY,
  /*                   0x3B */ LE_KEYBRD_NOKEY,
  /*                   0x3C */ LE_KEYBRD_NOKEY,
  /*                   0x3D */ LE_KEYBRD_NOKEY,
  /*                   0x3E */ LE_KEYBRD_NOKEY,
  /*                   0x3F */ LE_KEYBRD_NOKEY,
  /*                   0x40 */ LE_KEYBRD_NOKEY,
  /* VK_A              0x41 */ LE_KEYBRD_A,
  /* VK_B              0x42 */ LE_KEYBRD_B,
  /* VK_C              0x43 */ LE_KEYBRD_C,
  /* VK_D              0x44 */ LE_KEYBRD_D,
  /* VK_E              0x45 */ LE_KEYBRD_E,
  /* VK_F              0x46 */ LE_KEYBRD_F,
  /* VK_G              0x47 */ LE_KEYBRD_G,
  /* VK_H              0x48 */ LE_KEYBRD_H,
  /* VK_I              0x49 */ LE_KEYBRD_I,
  /* VK_J              0x4A */ LE_KEYBRD_J,
  /* VK_K              0x4B */ LE_KEYBRD_K,
  /* VK_L              0x4C */ LE_KEYBRD_L,
  /* VK_M              0x4D */ LE_KEYBRD_M,
  /* VK_N              0x4E */ LE_KEYBRD_N,
  /* VK_O              0x4F */ LE_KEYBRD_O,
  /* VK_P              0x50 */ LE_KEYBRD_P,
  /* VK_Q              0x51 */ LE_KEYBRD_Q,
  /* VK_R              0x52 */ LE_KEYBRD_R,
  /* VK_S              0x53 */ LE_KEYBRD_S,
  /* VK_T              0x54 */ LE_KEYBRD_T,
  /* VK_U              0x55 */ LE_KEYBRD_U,
  /* VK_V              0x56 */ LE_KEYBRD_V,
  /* VK_W              0x57 */ LE_KEYBRD_W,
  /* VK_X              0x58 */ LE_KEYBRD_X,
  /* VK_Y              0x59 */ LE_KEYBRD_Y,
  /* VK_Z              0x5A */ LE_KEYBRD_Z,
  /* VK_LWIN           0x5B */ LE_KEYBRD_NOKEY,
  /* VK_RWIN           0x5C */ LE_KEYBRD_NOKEY,
  /* VK_APPS           0x5D */ LE_KEYBRD_NOKEY,
  /*                   0x5E */ LE_KEYBRD_NOKEY,
  /*                   0x5F */ LE_KEYBRD_NOKEY,
  /* VK_NUMPAD0        0x60 */ LE_KEYBRD_NOKEY,
  /* VK_NUMPAD1        0x61 */ LE_KEYBRD_NOKEY,
  /* VK_NUMPAD2        0x62 */ LE_KEYBRD_NOKEY,
  /* VK_NUMPAD3        0x63 */ LE_KEYBRD_NOKEY,
  /* VK_NUMPAD4        0x64 */ LE_KEYBRD_NOKEY,
  /* VK_NUMPAD5        0x65 */ LE_KEYBRD_NOKEY,
  /* VK_NUMPAD6        0x66 */ LE_KEYBRD_NOKEY,
  /* VK_NUMPAD7        0x67 */ LE_KEYBRD_NOKEY,
  /* VK_NUMPAD8        0x68 */ LE_KEYBRD_NOKEY,
  /* VK_NUMPAD9        0x69 */ LE_KEYBRD_NOKEY,
  /* VK_MULTIPLY       0x6A */ LE_KEYBRD_NOKEY,
  /* VK_ADD            0x6B */ LE_KEYBRD_NOKEY,
  /* VK_SEPARATOR      0x6C */ LE_KEYBRD_NOKEY,
  /* VK_SUBTRACT       0x6D */ LE_KEYBRD_NOKEY,
  /* VK_DECIMAL        0x6E */ LE_KEYBRD_NOKEY,
  /* VK_DIVIDE         0x6F */ LE_KEYBRD_NOKEY,
  /* VK_F1             0x70 */ LE_KEYBRD_F1,
  /* VK_F2             0x71 */ LE_KEYBRD_F2,
  /* VK_F3             0x72 */ LE_KEYBRD_F3,
  /* VK_F4             0x73 */ LE_KEYBRD_F4,
  /* VK_F5             0x74 */ LE_KEYBRD_F5,
  /* VK_F6             0x75 */ LE_KEYBRD_F6,
  /* VK_F7             0x76 */ LE_KEYBRD_F7,
  /* VK_F8             0x77 */ LE_KEYBRD_F8,
  /* VK_F9             0x78 */ LE_KEYBRD_F9,
  /* VK_F10            0x79 */ LE_KEYBRD_F10,
  /* VK_F11            0x7A */ LE_KEYBRD_F11,
  /* VK_F12            0x7B */ LE_KEYBRD_F12,
  /* VK_F13            0x7C */ LE_KEYBRD_NOKEY,
  /* VK_F14            0x7D */ LE_KEYBRD_NOKEY,
  /* VK_F15            0x7E */ LE_KEYBRD_NOKEY,
  /* VK_F16            0x7F */ LE_KEYBRD_NOKEY,
  /* VK_F17            0x80 */ LE_KEYBRD_NOKEY,
  /* VK_F18            0x81 */ LE_KEYBRD_NOKEY,
  /* VK_F19            0x82 */ LE_KEYBRD_NOKEY,
  /* VK_F20            0x83 */ LE_KEYBRD_NOKEY,
  /* VK_F21            0x84 */ LE_KEYBRD_NOKEY,
  /* VK_F22            0x85 */ LE_KEYBRD_NOKEY,
  /* VK_F23            0x86 */ LE_KEYBRD_NOKEY,
  /* VK_F24            0x87 */ LE_KEYBRD_NOKEY,
  /*                   0x88 */ LE_KEYBRD_NOKEY,
  /*                   0x89 */ LE_KEYBRD_NOKEY,
  /*                   0x8A */ LE_KEYBRD_NOKEY,
  /*                   0x8B */ LE_KEYBRD_NOKEY,
  /*                   0x8C */ LE_KEYBRD_NOKEY,
  /*                   0x8D */ LE_KEYBRD_NOKEY,
  /*                   0x8E */ LE_KEYBRD_NOKEY,
  /*                   0x8F */ LE_KEYBRD_NOKEY,
  /* VK_NUMLOCK        0x90 */ LE_KEYBRD_NOKEY,
  /* VK_SCROLL         0x91 */ LE_KEYBRD_SCROLL_LOCK,
  /*                   0x92 */ LE_KEYBRD_NOKEY,
  /*                   0x93 */ LE_KEYBRD_NOKEY,
  /*                   0x94 */ LE_KEYBRD_NOKEY,
  /*                   0x95 */ LE_KEYBRD_NOKEY,
  /*                   0x96 */ LE_KEYBRD_NOKEY,
  /*                   0x97 */ LE_KEYBRD_NOKEY,
  /*                   0x98 */ LE_KEYBRD_NOKEY,
  /*                   0x99 */ LE_KEYBRD_NOKEY,
  /*                   0x9A */ LE_KEYBRD_NOKEY,
  /*                   0x9B */ LE_KEYBRD_NOKEY,
  /*                   0x9C */ LE_KEYBRD_NOKEY,
  /*                   0x9D */ LE_KEYBRD_NOKEY,
  /*                   0x9E */ LE_KEYBRD_NOKEY,
  /*                   0x9F */ LE_KEYBRD_NOKEY,
  /* VK_LSHIFT         0xA0 */ LE_KEYBRD_NOKEY,
  /* VK_RSHIFT         0xA1 */ LE_KEYBRD_NOKEY,
  /* VK_LCONTROL       0xA2 */ LE_KEYBRD_NOKEY,
  /* VK_RCONTROL       0xA3 */ LE_KEYBRD_NOKEY,
  /* VK_LMENU          0xA4 */ LE_KEYBRD_NOKEY,
  /* VK_RMENU          0xA5 */ LE_KEYBRD_NOKEY,
  /*                   0xA6 */ LE_KEYBRD_NOKEY,
  /*                   0xA7 */ LE_KEYBRD_NOKEY,
  /*                   0xA8 */ LE_KEYBRD_NOKEY,
  /*                   0xA9 */ LE_KEYBRD_NOKEY,
  /*                   0xAA */ LE_KEYBRD_NOKEY,
  /*                   0xAB */ LE_KEYBRD_NOKEY,
  /*                   0xAC */ LE_KEYBRD_NOKEY,
  /*                   0xAD */ LE_KEYBRD_NOKEY,
  /*                   0xAE */ LE_KEYBRD_NOKEY,
  /*                   0xAF */ LE_KEYBRD_NOKEY,
  /*                   0xB0 */ LE_KEYBRD_NOKEY,
  /*                   0xB1 */ LE_KEYBRD_NOKEY,
  /*                   0xB2 */ LE_KEYBRD_NOKEY,
  /*                   0xB3 */ LE_KEYBRD_NOKEY,
  /*                   0xB4 */ LE_KEYBRD_NOKEY,
  /*                   0xB5 */ LE_KEYBRD_NOKEY,
  /*                   0xB6 */ LE_KEYBRD_NOKEY,
  /*                   0xB7 */ LE_KEYBRD_NOKEY,
  /*                   0xB8 */ LE_KEYBRD_NOKEY,
  /*                   0xB9 */ LE_KEYBRD_NOKEY,
  /*                   0xBA */ LE_KEYBRD_NOKEY,
  /*                   0xBB */ LE_KEYBRD_NOKEY,
  /*                   0xBC */ LE_KEYBRD_NOKEY,
  /*                   0xBD */ LE_KEYBRD_HYPHEN,
  /*                   0xBE */ LE_KEYBRD_NOKEY,
  /*                   0xBF */ LE_KEYBRD_NOKEY,
  /*                   0xC0 */ LE_KEYBRD_NOKEY,
  /*                   0xC1 */ LE_KEYBRD_NOKEY,
  /*                   0xC2 */ LE_KEYBRD_NOKEY,
  /*                   0xC3 */ LE_KEYBRD_NOKEY,
  /*                   0xC4 */ LE_KEYBRD_NOKEY,
  /*                   0xC5 */ LE_KEYBRD_NOKEY,
  /*                   0xC6 */ LE_KEYBRD_NOKEY,
  /*                   0xC7 */ LE_KEYBRD_NOKEY,
  /*                   0xC8 */ LE_KEYBRD_NOKEY,
  /*                   0xC9 */ LE_KEYBRD_NOKEY,
  /*                   0xCA */ LE_KEYBRD_NOKEY,
  /*                   0xCB */ LE_KEYBRD_NOKEY,
  /*                   0xCC */ LE_KEYBRD_NOKEY,
  /*                   0xCD */ LE_KEYBRD_NOKEY,
  /*                   0xCE */ LE_KEYBRD_NOKEY,
  /*                   0xCF */ LE_KEYBRD_NOKEY,
  /*                   0xD0 */ LE_KEYBRD_NOKEY,
  /*                   0xD1 */ LE_KEYBRD_NOKEY,
  /*                   0xD2 */ LE_KEYBRD_NOKEY,
  /*                   0xD3 */ LE_KEYBRD_NOKEY,
  /*                   0xD4 */ LE_KEYBRD_NOKEY,
  /*                   0xD5 */ LE_KEYBRD_NOKEY,
  /*                   0xD6 */ LE_KEYBRD_NOKEY,
  /*                   0xD7 */ LE_KEYBRD_NOKEY,
  /*                   0xD8 */ LE_KEYBRD_NOKEY,
  /*                   0xD9 */ LE_KEYBRD_NOKEY,
  /*                   0xDA */ LE_KEYBRD_NOKEY,
  /*                   0xDB */ LE_KEYBRD_NOKEY,
  /*                   0xDC */ LE_KEYBRD_NOKEY,
  /*                   0xDD */ LE_KEYBRD_NOKEY,
  /*                   0xDE */ LE_KEYBRD_NOKEY,
  /*                   0xDF */ LE_KEYBRD_NOKEY,
  /*                   0xE0 */ LE_KEYBRD_NOKEY,
  /*                   0xE1 */ LE_KEYBRD_NOKEY,
  /*                   0xE2 */ LE_KEYBRD_NOKEY,
  /*                   0xE3 */ LE_KEYBRD_NOKEY,
  /*                   0xE4 */ LE_KEYBRD_NOKEY,
  /* VK_PROCESSKEY     0xE5 */ LE_KEYBRD_NOKEY,
  /*                   0xE6 */ LE_KEYBRD_NOKEY,
  /*                   0xE7 */ LE_KEYBRD_NOKEY,
  /*                   0xE8 */ LE_KEYBRD_NOKEY,
  /*                   0xE9 */ LE_KEYBRD_NOKEY,
  /*                   0xEA */ LE_KEYBRD_NOKEY,
  /*                   0xEB */ LE_KEYBRD_NOKEY,
  /*                   0xEC */ LE_KEYBRD_NOKEY,
  /*                   0xED */ LE_KEYBRD_NOKEY,
  /*                   0xEE */ LE_KEYBRD_NOKEY,
  /*                   0xEF */ LE_KEYBRD_NOKEY,
  /*                   0xF0 */ LE_KEYBRD_NOKEY,
  /*                   0xF1 */ LE_KEYBRD_NOKEY,
  /*                   0xF2 */ LE_KEYBRD_NOKEY,
  /*                   0xF3 */ LE_KEYBRD_NOKEY,
  /*                   0xF4 */ LE_KEYBRD_NOKEY,
  /*                   0xF5 */ LE_KEYBRD_NOKEY,
  /* VK_ATTN           0xF6 */ LE_KEYBRD_NOKEY,
  /* VK_CRSEL          0xF7 */ LE_KEYBRD_NOKEY,
  /* VK_EXSEL          0xF8 */ LE_KEYBRD_NOKEY,
  /* VK_EREOF          0xF9 */ LE_KEYBRD_NOKEY,
  /* VK_PLAY           0xFA */ LE_KEYBRD_NOKEY,
  /* VK_ZOOM           0xFB */ LE_KEYBRD_NOKEY,
  /* VK_NONAME         0xFC */ LE_KEYBRD_NOKEY,
  /* VK_PA1            0xFD */ LE_KEYBRD_NOKEY,
  /* VK_OEM_CLEAR      0xFE */ LE_KEYBRD_NOKEY,
  /*                   0xFF */ LE_KEYBRD_NOKEY
};



/*************************************************************
*
* void LI_KEYBRD_InitSystem(void);
*
*   Description:
*       KEYBRD system initializion procedure.
*
*************************************************************/
void LI_KEYBRD_InitSystem(void)
{
#if CE_ARTLIB_KeyboardPressedAndLatchedArray
  int i;

  for (i = 0; i < LE_KEYBRD_NUMKEYS; i++)
  {
    LE_KEYBRD_KeysLatched[i] = FALSE;
    LE_KEYBRD_KeysPressed[i] = FALSE;
  }

  LE_KEYBRD_AnyKey = FALSE;
  LE_KEYBRD_LastAscii = 0;
#endif

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_EnableSystemSequencer
  LI_KEYBRD_ScrollLockPressedForSequencer = FALSE;
  LI_KEYBRD_ScrollLockLatchedForSequencer = FALSE;
#endif
}



/*************************************************************
*
* void LI_KEYBRD_RemoveSystem(void);
*
*   Description:
*       KEYBRD system cleanup and removal procedure.
*
*************************************************************/
void LI_KEYBRD_RemoveSystem(void)
{
}



/*************************************************************
*
* BYTE LI_KEYBRD_ConvertAsciiToOurCode(char ascii);
*
*   Description:
*       Converts an ASCII character to internal format.  Note
*       that upper case letters are used for letter keys.
*
*************************************************************/
BYTE LE_KEYBRD_ConvertAsciiToOurCode(char ascii)
{
  return( WinVirtualKeyToOurCode[ascii] );
}



/********************************************************************
*
* void LI_KEYBRD_UpdateKeybrd(UINT msg, WPARAM wParam, LPARAM lParam)
*
*   Description:
*       Watches the windows keyboard messages and updates the
*       system keyboard variables.
*
********************************************************************/
void LI_KEYBRD_UpdateKeybrd (UINT msg, WPARAM wParam, LPARAM lParam)
{
  LE_KEYBRD_ScanCodes OurKeyCode;

  if (msg == WM_CHAR)
  {
#if CE_ARTLIB_KeyboardPressedAndLatchedArray
    LE_KEYBRD_LastAscii = (UNS8) wParam;  /* Always given an 8 bit character. */
#endif

#if CE_ARTLIB_EnableSystemUIMsg
    LE_UIMSG_SendEvent (UIMSG_KEYBOARD_ACHAR, (UNS8) wParam, 0, 0, 0, 0, NULL, 0);
#endif // CE_ARTLIB_EnableSystemUIMsg
    return; /* This way we get lower case and upper case etc. */
  }

  if (msg != WM_KEYDOWN && msg != WM_KEYUP)
    return; /* Not a key pressed message.  Ignoring WM_SYSKEYDOWN alt-tab etc. */

  if (msg == WM_KEYDOWN && (lParam & 0x40000000))
    return; /* This is an auto-repeat key down, ignore it. */

  /* Convert the key code into our internal key code system. */

  OurKeyCode = (LE_KEYBRD_ScanCodes) (BYTE) wParam;
  OurKeyCode = WinVirtualKeyToOurCode [OurKeyCode];

  /* Update the key pressed and latched arrays. */

  if (msg == WM_KEYDOWN)
  {
#if CE_ARTLIB_KeyboardPressedAndLatchedArray
    LE_KEYBRD_AnyKey = 1;
    LE_KEYBRD_KeysLatched [OurKeyCode] = TRUE;
    LE_KEYBRD_KeysPressed [OurKeyCode] = TRUE;
#endif

#if CE_ARTLIB_EnableSystemUIMsg
    LE_UIMSG_SendEvent (UIMSG_KEYBOARD_PRESSED, OurKeyCode, wParam, 0, 0, 0, NULL, 0);
#endif //CE_ARTLIB_EnableSystemUIMsg

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_EnableSystemSequencer
    if (OurKeyCode == LE_KEYBRD_SCROLL_LOCK)
    {
      LI_KEYBRD_ScrollLockPressedForSequencer = TRUE;
      LI_KEYBRD_ScrollLockLatchedForSequencer = TRUE;
      LI_KEYBRD_ScrollLockShiftForSequencer = (GetKeyState (VK_SHIFT) < 0);
    }
#endif // CE_ARTLIB_EnableSystemSequencer
  }
  else if (msg == WM_KEYUP)
  {
#if CE_ARTLIB_KeyboardPressedAndLatchedArray
    LE_KEYBRD_KeysPressed [OurKeyCode] = FALSE;
#endif

#if CE_ARTLIB_EnableSystemUIMsg
    LE_UIMSG_SendEvent (UIMSG_KEYBOARD_RELEASED, OurKeyCode, wParam, 0, 0, 0, NULL, 0);
#endif // CE_ARTLIB_EnableSystemUIMsg

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_EnableSystemSequencer
    if (OurKeyCode == LE_KEYBRD_SCROLL_LOCK)
      LI_KEYBRD_ScrollLockPressedForSequencer = FALSE;
#endif // CE_ARTLIB_EnableSystemSequencer
  }
}



#if CE_ARTLIB_KeyboardPressedAndLatchedArray
/*************************************************************
*   Description:
*       Checks if a particular key is latched or not.
*
*   Arguments:
*       Scancode: The scancode for the key, as defined in
*                 l_keybrd.h
*
*   Returns:
*       1:  The key is latched.
*       0: The key is not latched.
*
*************************************************************/
BOOL LE_KEYBRD_CheckLatched (LE_KEYBRD_ScanCodes Scancode)
{
#if CE_ARTLIB_EnableDebugMode
  if ((Scancode >= LE_KEYBRD_NUMKEYS) || (Scancode == LE_KEYBRD_NOKEY))
  {
    sprintf (LE_ERROR_DebugMessageBuffer,
      "LE_KEYBRD_CheckLatched(): Scancode '0x%x' not found.\r\n",
      (int) Scancode);
    LE_ERROR_ErrorMsg (LE_ERROR_DebugMessageBuffer);
  }
#endif

  return LE_KEYBRD_KeysLatched [Scancode];
}
#endif // CE_ARTLIB_KeyboardPressedAndLatchedArray



#if CE_ARTLIB_KeyboardPressedAndLatchedArray
/*************************************************************
*   Description:
*       Unlatches a particular latched key.
*
*   Arguments:
*       Scancode: The scancode for the key, as defined in
*                 l_keybrd.h
*
*   Returns:
*       Nothing.
*
*************************************************************/
void LE_KEYBRD_ClearLatchedKey (LE_KEYBRD_ScanCodes Scancode)
{
#if CE_ARTLIB_EnableDebugMode
  if ((Scancode >= LE_KEYBRD_NUMKEYS) || (Scancode == LE_KEYBRD_NOKEY))
  {
    sprintf(LE_ERROR_DebugMessageBuffer,
      "LE_KEYBRD_ClearLatchedKey(): Scancode '0x%x' not found.\r\n",
      (int) Scancode);
    LE_ERROR_ErrorMsg (LE_ERROR_DebugMessageBuffer);
  }
#endif

  LE_KEYBRD_KeysLatched [Scancode] = 0;
}
#endif // CE_ARTLIB_KeyboardPressedAndLatchedArray



#if CE_ARTLIB_KeyboardPressedAndLatchedArray
/*************************************************************
*
* void LE_KEYBRD_ClearAllLatched(void)
*
*   Description:
*       Unlatches all latched keys.
*
*   Arguments:
*       None.
*
*   Returns:
*       Nothing.
*
*************************************************************/
void LE_KEYBRD_ClearAllLatched(void)
{
  memset ((void *) (LE_KEYBRD_KeysLatched + 0), 0, sizeof (LE_KEYBRD_KeysLatched));
}
#endif // CE_ARTLIB_KeyboardPressedAndLatchedArray



#if CE_ARTLIB_KeyboardPressedAndLatchedArray
/*************************************************************
*   Description:
*       Checks if a particular key is pressed or not.
*
*   Arguments:
*       Scancode: The scancode for the key, as defined in
*                 l_keybrd.h
*
*   Returns:
*       1:  The key is pressed.
*       0: The key is not pressed.
*
*************************************************************/
BOOL LE_KEYBRD_CheckPressed (LE_KEYBRD_ScanCodes Scancode)
{
#if CE_ARTLIB_EnableDebugMode
  if ((Scancode >= LE_KEYBRD_NUMKEYS) || (Scancode == LE_KEYBRD_NOKEY))
  {
    sprintf(LE_ERROR_DebugMessageBuffer,
      "LE_KEYBRD_CheckPressed(): Scancode '0x%x' not found.\r\n",
      (int) Scancode);
    LE_ERROR_ErrorMsg(LE_ERROR_DebugMessageBuffer);
  }
#endif

  return LE_KEYBRD_KeysPressed [Scancode];
}
#endif // CE_ARTLIB_KeyboardPressedAndLatchedArray

#endif // if CE_ARTLIB_EnableSystemKeybrd
