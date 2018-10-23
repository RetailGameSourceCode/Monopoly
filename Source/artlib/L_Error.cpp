/*****************************************************************************
 *
 * FILE:        L_Error.cpp
 *
 * DESCRIPTION: Error handling code - logs errors to a file, and/or pops up
 *              a warning message box, and can exit the program.  If debug
 *              mode is off, it just handles fatal errors and doesn't do
 *              logging.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Error.cpp 13    99/09/08 1:27p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Error.cpp $
 * 
 * 13    99/09/08 1:27p Agmsmith
 * Added a global flag which shows when a dialog box is up.
 * 
 * 12    99/09/07 3:01p Agmsmith
 * Now have a custom dialog box for error messages, with custom exit
 * button choices.
 *
 * 11    7/19/99 16:51 Davew
 * Enabled the debug messages to break into the application when Retry is
 * pressed.
 *
 * 10    7/16/99 10:57a Agmsmith
 * Don't stop the animation engine while displaying a message, even though
 * it may overwrite it, stopping it causes lockups and other problems.
 *
 * 9     2/17/99 1:25p Agmsmith
 * Change default button on error box.
 *
 * 8     2/11/99 12:37p Agmsmith
 * Cleanup.
 *
 * 7     2/10/99 12:54p Timp
 * Added LE_ERROR_Msg, LI_ERROR_Msg2, and LE_ERROR_MsgBox.
 * Changed LE_ERROR_DebugMsg and LE_ERROR_ErrorMsg to use LE_ERROR_Msg.
 * Also changed file pointer from HANDLE to FILE*.
 *
 * 6     1/22/99 12:38p Agmsmith
 * Try again, last version caused a deadlock.
 *
 * 5     1/22/99 12:12p Agmsmith
 * Prevent the error box from disappearing by letting the system know what
 * the parent window is, so that the animation engine stops drawing.
 *
 * 4     12/06/98 12:42p Agmsmith
 * Work even if not initialised, in case a constructor calls the debug
 * message stuff before the module is initialised.
 *
 * 3     9/16/98 4:59p Agmsmith
 * Now compiles under C++.
 *
 * 2     9/15/98 12:28p Agmsmith
 * Now compiles under C++
 ****************************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "l_inc.h"
#include "resource.h" // Error dialog box is in the game's resource file.


/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

char LE_ERROR_DebugMessageBuffer[LE_ERROR_MessageBufferSize];

signed char volatile LI_ERROR_DialogBoxShowing;


/************************************************************/
/* STATIC VARIABLES                                         */
/*----------------------------------------------------------*/

#if CE_ARTLIB_EnableDebugMode
static FILE *LI_ERROR_ErrorFileHandle = NULL;
static BOOL LI_ERROR_DebugTextToFile = TRUE;
#endif



/************************************************************/
/* LE_ERROR_EnableDebugText                                 */
/************************************************************/
#if CE_ARTLIB_EnableDebugMode
void LE_ERROR_EnableDebugText(void)
{
  LI_ERROR_DebugTextToFile = TRUE;
  wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
  wsprintf(LE_ERROR_DebugMessageBuffer,"LI_ERROR_EnableDebugText().\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
  wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
}
#endif



/************************************************************/
/* LE_ERROR_DisableDebugText                                */
/************************************************************/
#if CE_ARTLIB_EnableDebugMode
void LE_ERROR_DisableDebugText(void)
{
  LI_ERROR_DebugTextToFile = TRUE;
  wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
  wsprintf(LE_ERROR_DebugMessageBuffer,"LI_ERROR_DisableDebugText().\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
  wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
  LI_ERROR_DebugTextToFile = FALSE;
}
#endif



/*************************************************************
*
* void LI_ERROR_OutToFile(char *MESSAGE)
*
*   Description:
*       Writes MESSAGE out to currently opened debug file.
*
*   Arguments:
*       char                    *MESSAGE
*
*   Returns:
*       None.
*
*   Notes, Bugs, Fixes, Changes, etc:
*       - 13:12:23 Fri  12-15-1995 (MMH)
*       - Initial version.
*************************************************************/
#if CE_ARTLIB_EnableDebugMode
static void LI_ERROR_OutToFile(char *MESSAGE)
{
  if (LI_ERROR_DebugTextToFile)
  {
    if (LI_ERROR_ErrorFileHandle != NULL)
    {
      fprintf(LI_ERROR_ErrorFileHandle, MESSAGE);
    }
  }
}
#endif



/************************************************************
*
* void LI_ERROR_InitSystem(void);
*
*   Description:
*       ERROR system initializion procedure.
*
*   Arguments:
*       None.
*
*   Returns:
*       None.
*
*   Notes, Bugs, Fixes, Changes, etc:
*       - 13:55:59 Tue  11-28-1995 (MMH)
*         Initial empty function.
*************************************************************/
#if CE_ARTLIB_EnableDebugMode
void LI_ERROR_InitSystem(void)
{
  char fname[13];
  short loop1;

  LI_ERROR_ErrorFileHandle = NULL;
  LI_ERROR_DialogBoxShowing = 0;

  memset(fname,0,sizeof(fname));
  for (loop1=0;loop1<=999;loop1++)
  {
    // Generate this file name.
    sprintf(fname,"%s%03d%s",LE_ERROR_DebugOutFilePrefix,loop1,LE_ERROR_DebugOutFilePostfix);

    // Check if file can be created and does not previously exist.
    LI_ERROR_ErrorFileHandle = fopen(fname, "r");

    // If file could not be opened and therefore didn't exist before.
    if (LI_ERROR_ErrorFileHandle == NULL) break;

    fclose(LI_ERROR_ErrorFileHandle);
  }
  LI_ERROR_ErrorFileHandle = fopen(fname, "wb");

  // Add opening text to output file.
  wsprintf(LE_ERROR_DebugMessageBuffer,"/************************************************************/\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
  wsprintf(LE_ERROR_DebugMessageBuffer,"LI_ERROR_InitSystem(): Error file opened.\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
  wsprintf(LE_ERROR_DebugMessageBuffer,"This file can be deleted. It is for debugging only!\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
  wsprintf(LE_ERROR_DebugMessageBuffer,"/************************************************************/\r\n\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);

  LE_ERROR_EnableDebugText();
}
#endif



/*************************************************************
*
* void LI_ERROR_RemoveSystem(void);
*
*   Description:
*       ERROR system cleanup and removal procedure.
*
*   Arguments:
*       None.
*
*   Returns:
*       None.
*
*   Notes, Bugs, Fixes, Changes, etc:
*       - 13:55:59 Tue  11-28-1995 (MMH)
*         Initial empty function.
*************************************************************/
#if CE_ARTLIB_EnableDebugMode
void LI_ERROR_RemoveSystem(void)
{
  // Add closing text to output file.
  wsprintf(LE_ERROR_DebugMessageBuffer,"/************************************************************/\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
  wsprintf(LE_ERROR_DebugMessageBuffer,"LI_ERROR_RemoveSystem(): Error file Closing.\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
  wsprintf(LE_ERROR_DebugMessageBuffer,"/************************************************************/\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);

  if (LI_ERROR_ErrorFileHandle != NULL)
  {
    fclose(LI_ERROR_ErrorFileHandle);          // Close the debug out file handle
  }
  LI_ERROR_ErrorFileHandle = NULL;
}
#endif



/*************************************************************
*
* void LI_ERROR_Msg2 (char *MessageString, LE_ERROR_OutputType OutputType, BOOL Fatal)
*
*   Description:
*       Writes MessageString to various output locations,
*       depending on the value of OutputType.
*       (UDF file, debug log, and/or screen.
*       See comments, and LE_ERROR_OutputType in L_Error.h)
*       Appends an error code to the start of the string.
*
*   Arguments:
*       char *MessageString             Message to output
*       LE_ERROR_OutputType OutputType  Where to send message
*       BOOL Fatal                      Program exits if TRUE
*       void *ReturnAddress             Return address of LE_ERROR_Msg's caller
*
*   Returns:
*       None.
*
*   Notes, Bugs, Fixes, Changes, etc:
*************************************************************/
static void LI_ERROR_Msg2 (char *MessageString, LE_ERROR_OutputType OutputType, BOOL Fatal, void *ReturnAddress)
{
  int i, result;
#if CE_ARTLIB_EnableDebugMode
  char tmp[8];
#endif
  BOOL showmsgflag = TRUE, printnewlineflag = TRUE;
  // Store return addresses of callers to determine if message
  // has been seen before (and the user asked to ignore it)
  static int LI_ERROR_SeenMessageBufferPtr = 0;
  static void *LI_ERROR_SeenMessageBuffer[LI_ERROR_SeenMessageBufferSize] = { NULL };

#if CE_ARTLIB_EnableDebugMode
  if ((MessageString[strlen(MessageString)-1] == '\r') ||
    (MessageString[strlen(MessageString)-1] == '\n'))
    printnewlineflag = FALSE;
  sprintf(tmp, "%d%s: ", (int)OutputType, (Fatal) ? "!" : "");
  if (OutputType >= LE_ERROR_OUTPUT_FILE)
  {
    // Print error to UDF file...
    LI_ERROR_OutToFile(tmp);
    LI_ERROR_OutToFile(MessageString);
    if (printnewlineflag)
      LI_ERROR_OutToFile("\r\n");
  }

  if (OutputType >= LE_ERROR_OUTPUT_LOG)
  {
    // Print error to debug log...
    OutputDebugString(tmp);
    OutputDebugString(MessageString);
    if (printnewlineflag)
      OutputDebugString("\r\n");
  }
#endif

  if (OutputType >= LE_ERROR_OUTPUT_USER)
  {
    // See if message has been seen before and ignored (if so, don't show it)
    for (i = 0; i < LI_ERROR_SeenMessageBufferSize; i++)
      if (LI_ERROR_SeenMessageBuffer[i] == ReturnAddress)
      {
        showmsgflag = FALSE;
        break;
      }

    if (showmsgflag)
    {
      // Display error to user...
      if (Fatal)
      {
        LE_ERROR_MsgBox (MessageString, "Library Code Fatal Message");
        result = IDC_LI_ERROR_ABORT;
      }
      else
        result = LE_ERROR_MsgBox (MessageString,
        "Library Code Debug Message");

      if (result == IDC_LI_ERROR_ABORT)
        exit(-2);
      else if (result == IDC_LI_ERROR_DEBUG)
      {
        DEBUGBreak();
        return;
      }
      else if (result == IDC_LI_ERROR_IGNORE)
      {
        // Remember the Message (return address actually) and don't show it
        // again unless future kinds of messages overwrite this one
        // (Can only ignore LI_ERROR_SeenMessageBufferSize kinds of msgs at a time)
        LI_ERROR_SeenMessageBuffer[LI_ERROR_SeenMessageBufferPtr] = ReturnAddress;
        LI_ERROR_SeenMessageBufferPtr++;
        if (LI_ERROR_SeenMessageBufferPtr >= LI_ERROR_SeenMessageBufferSize)
          LI_ERROR_SeenMessageBufferPtr = 0;
      }
      // Else if continue on, do nothing special.
    }
  }

  if (Fatal)
    exit(-2);
}
/************************************************************/

/*************************************************************
*
* void LE_ERROR_Msg (char *MessageString, LE_ERROR_OutputType OutputType, BOOL Fatal)
*
*   Description:
*       Calls LI_ERROR_Msg2 with arguments and return address of caller.
*
*   Arguments:
*       char *MessageString             Message to output
*       LE_ERROR_OutputType OutputType  Where to send message
*       BOOL Fatal                      Program exits if TRUE
*
*   Returns:
*       None.
*
*   Notes, Bugs, Fixes, Changes, etc:
*************************************************************/
void LE_ERROR_Msg (char *MessageString, LE_ERROR_OutputType OutputType, BOOL Fatal = FALSE)
{
  long i = 0x0ABACABA, *ReturnAddress;
  // Point to return address on stack;
#if _DEBUG
  ReturnAddress = (&i) + 2; // Debug return address offset
#else
  ReturnAddress = (&i) + 2; // Release return address offset
#endif
  // Note: Changing this function will likely result in having to
  // change the above offsets, which can be tricky to determine
  LI_ERROR_Msg2(MessageString, OutputType, Fatal, (void *)(*ReturnAddress));
}
/************************************************************/

/*************************************************************
*
* void LE_ERROR_ErrorMsg(char *E_MESSAGE)
*
*   Description:
*       Writes E_MESSAGE out to currently opened debug file.
*       Appends "3!: " to the start of this string.
*       Will also display error to the user in a message box
*       and wait for user to dismiss messagebox.
*
*   Arguments:
*       char *E_MESSAGE
*
*   Returns:
*       None.
*
*   Notes, Bugs, Fixes, Changes, etc:
*       - 13:55:59 Tue  11-28-1995 (MMH)
*         Initial empty function.
*       - 1999/02/08 (timp) Change to use new LE_ERROR_Msg function
*************************************************************/
void LE_ERROR_ErrorMsg(char *E_MESSAGE)
{
  // Note: Don't have to determine return address for ignore
  // functionality because this function causes program to exit.
  LE_ERROR_Msg(E_MESSAGE, LE_ERROR_OUTPUT_USER, TRUE);
}
/************************************************************/

/*************************************************************
*
* void LE_ERROR_DebugMsg(char *E_MESSAGE, LE_ERROR_DebugMsgDest Dest)
*
*   Description:
*       Writes D_MESSAGE out to currently opened debug file.
*       Appends "2: " or "3: " to the start of this string,
*       depending on if a messagebox is displayed.
*       May display message to the user in a message box
*       and wait for user to continue.
*
*   Arguments:
*       char                    *D_MESSAGE
*       LE_ERROR_DebugMsgDest   Dest
*
*   Returns:
*       None.
*
*   Notes, Bugs, Fixes, Changes, etc:
*       - 13:12:23 Fri  12-15-1995 (MMH)
*       - Initial version.
*       - 1999/02/08 (timp) Change to use new LE_ERROR_Msg function
*************************************************************/
#if CE_ARTLIB_EnableDebugMode
void LE_ERROR_DebugMsg(char *D_MESSAGE, BOOL DisplayOnScreen)
{
  long i = 0x0ABACABA, *ReturnAddress;
  // Point to return address on stack;
#if _DEBUG
  ReturnAddress = (&i) + 2; // Debug return address offset
#else
  ReturnAddress = (&i) + 2; // Release return address offset
#endif
  // Note: Changing this function will likely result in having to
  // change the above offsets, which can be tricky to determine
  LI_ERROR_Msg2(D_MESSAGE, (DisplayOnScreen) ? LE_ERROR_OUTPUT_USER : LE_ERROR_OUTPUT_LOG, FALSE, (void *)(*ReturnAddress));
}
#endif
/************************************************************/


/*****************************************************************************
 * Internal function for handling the error message dialog box.  Set up the
 * text in the box, wait for a button press, close the box and pass back
 * the button pressed.
 */

typedef struct StringListStruct
{
  char *titleString;
  char *messageString;
} StringListRecord, *StringListPointer;

BOOL CALLBACK LI_ERROR_DialogProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  int               ControlID;
  HWND              hwndControl;
  int               NotificationCode;
  StringListPointer StringListPntr;

  if (uMsg == WM_INITDIALOG)
  {
    StringListPntr = (StringListPointer) lParam;
    if (StringListPntr->titleString != NULL)
      SetWindowText (hwndDlg, StringListPntr->titleString);
    if (StringListPntr->messageString != NULL)
      SetDlgItemText (hwndDlg, IDC_LI_ERROR_TEXT, StringListPntr->messageString);
    SetTimer (hwndDlg, 321, 2000 /* Milliseconds */, NULL); // Periodically redraw the box.
    SetFocus (GetDlgItem (hwndDlg, IDC_LI_ERROR_CONTINUE));
    return FALSE; /* Nonzero to set default focus, zero if we set it. */
  }

  if (uMsg == WM_COMMAND)
  {
    ControlID = GET_WM_COMMAND_ID (wParam, lParam);
    NotificationCode = GET_WM_COMMAND_CMD (wParam, lParam);
    hwndControl = GET_WM_COMMAND_HWND (wParam, lParam);

    switch (ControlID)
    {
    case IDC_LI_ERROR_ABORT:
    case IDC_LI_ERROR_DEBUG:
    case IDC_LI_ERROR_CONTINUE:
    case IDC_LI_ERROR_IGNORE:
      KillTimer (hwndDlg, 321);
      EndDialog (hwndDlg, ControlID);
      return TRUE;

    default:
      break;
    }
  }

  if (uMsg == WM_TIMER)
  {
    // Periodically redraw the dialog box in case the game writes over it.
    InvalidateRect (hwndDlg, NULL, TRUE /* Erase background */);
    MessageBeep (-1);
  }

  return FALSE; /* Allow furthur default message handling by the dialog box. */
}



/*************************************************************
*
* int LE_ERROR_MsgBox(char *MESSAGE, char *TITLE, UINT MODE);
*
*   Description:
*       Writes MESSAGE out to Message box to screen with
*       title TITLE.  Passes back the return code which
*       identifies the button pressed (IDC_LI_ERROR_ABORT to
*       IDC_LI_ERROR_IGNORE).
*
*   Arguments:
*       char                    *MESSAGE
*       char                    *TITLE
*
*   Returns:
*       Button pressed.
*
*   Notes, Bugs, Fixes, Changes, etc:
*       - 1999/02/10 timp: Initial version.
*************************************************************/
int LE_ERROR_MsgBox(char *MESSAGE, char *TITLE)
{
  int result;
  StringListRecord  StringList;

  // Show the box, and make the Windows cursor visible too.
  // Note that this can be overwritten by the animation engine
  // in multitasking mode but we can't stop it due to lockup problems.

  LI_ERROR_DialogBoxShowing++;

  ShowCursor (TRUE);

  StringList.titleString = TITLE;
  StringList.messageString = MESSAGE;

  result = DialogBoxParam (
    LE_MAIN_Hinst,
    MAKEINTRESOURCE (IDD_LI_ERROR_MESSAGE_BOX),
    LE_MAIN_HwndMainWindow,
    LI_ERROR_DialogProc,
    (LPARAM) &StringList);

  ShowCursor (FALSE);

  LI_ERROR_DialogBoxShowing--;

  return result;
}
/************************************************************/
