#ifndef __L_ERROR_H__
#define __L_ERROR_H__

/*****************************************************************************
 *
 * FILE:        L_Error.h
 *
 * DESCRIPTION: Error handling code header.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * The library has a complete error and debug system that is compile time
 * defined (turned on or off).  This means that most functions perform
 * extensive testing and reporting about usage while in debug mode and all of
 * this testing can be removed from a final product with a change to
 * CE_ARTLIB_EnableDebugMode and a re-compile.  While in debug mode some of
 * this reporting may be done to the screen in the form of message boxes but
 * ALL of it will be added to a UDF???.txt file found in the current
 * executable location.
 *
 * Programmers are encouraged to add appropriate debug code to any functions
 * they add to the library and to use similar type functions throughout their
 * game code.
 *
 * $Header: /Artlib_99/ArtLib/L_Error.h 9     99/09/08 1:27p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Error.h $
 * 
 * 9     99/09/08 1:27p Agmsmith
 * Added a global flag which shows when a dialog box is up.
 * 
 * 8     99/09/07 3:01p Agmsmith
 * Now have a custom dialog box for error messages, with custom exit
 * button choices.
 * 
 * 7     7/20/99 8:25a Agmsmith
 * Have DebugBreak do nothing in release mode.
 *
 * 6     7/19/99 16:51 Davew
 * Enabled the debug messages to break into the application when Retry is
 * pressed.
 *
 * 5     2/11/99 12:37p Agmsmith
 * Cleanup.
 *
 * 4     2/10/99 12:53p Timp
 * Added LE_ERROR_Msg, LI_ERROR_Msg2, and LE_ERROR_MsgBox.
 *
 * 3     10/09/98 3:13p Agmsmith
 * Boost error string to 2K.
 *
 * 2     9/16/98 4:59p Agmsmith
 * Now compiles under C++.
 ****************************************************************************/



/************************************************************/
/* DEFINES                                                  */
/*----------------------------------------------------------*/

#define LE_ERROR_MessageBufferSize      2500
  // Leave enough space for DataID descriptions, which are limited to 2K.


#if CE_ARTLIB_EnableDebugMode
    #define LE_ERROR_DebugOutFilePrefix     "UDF"
    #define LE_ERROR_DebugOutFilePostfix    ".TXT"
#endif // CE_ARTLIB_EnableDebugMode


#ifdef _DEBUG
  #if defined(_M_IX86)
    #define DEBUGBreak() __asm { int 3 }
  #else
    #define DEBUGBreak() DebugBreak()
  #endif // defined(_M_IX86)
#else
  #define DEBUGBreak()
#endif // _DEBUG


#define LI_ERROR_SeenMessageBufferSize 10



/************************************************************/
/* STRUCTURES, ENUMS, ETC.                                  */
/*----------------------------------------------------------*/

#if CE_ARTLIB_EnableDebugMode
  #define LE_ERROR_DebugMsgToFile           0
  #define LE_ERROR_DebugMsgToFileAndScreen  1
#endif

// Types of output for LE_ERROR_Msg
typedef enum LE_ERROR_OutputTypesEnum
{
  LE_ERROR_OUTPUT_NONE, // Not displayed or printed - really unimportant error.
  LE_ERROR_OUTPUT_FILE, // Output to the UDF log file.
  LE_ERROR_OUTPUT_LOG,  // Display in debug log and put in UDF file.
  LE_ERROR_OUTPUT_USER, // Display in messagebox, output to UDF and debug logs.
} LE_ERROR_OutputType;


/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

extern char LE_ERROR_DebugMessageBuffer[LE_ERROR_MessageBufferSize];
// - This buffer is available for temporary storage and string manipulation.
// - LE_ERROR_MessageBufferSize is currently 2500 bytes.
// - This buffer is available for use in both debug and non-debug modes of the library.


extern signed char volatile LI_ERROR_DialogBoxShowing;
// A counter which is non-zero when a dialog box (or more than one) is
// showing on the screen.  Due to problems in Win2000, other message
// processing threads should do nothing while the box is up.



/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

#if CE_ARTLIB_EnableDebugMode
extern void LI_ERROR_InitSystem(void);
// Completly sets up an initializes the error system. If in
// CE_ARTLIB_EnableDebugMode then this should find and open
// the next debug out file using LE_ERROR_DebugOutFilePrefix
// and LE_ERROR_DebugOutFilePostfix defines. Write to that
// file its creation data and time.

extern void LI_ERROR_RemoveSystem(void);
// Completely cleans up and removes the error system. If in
// CE_ARTLIB_EnableDebugMode then this should write a closing
// message to the currently opened debug out file and close it.

extern void LE_ERROR_DebugMsg(char *D_MESSAGE, BOOL DisplayOnScreen);
// Obsolete function for displaying non-fatal error messages.
// This will write the D_MESSAGE string to the currently opened
// debug out file.  It will append "2: " (or "3: " if DisplayOnScreen
// is set) to the front of this string when writing to the file for
// easy searching.  If DisplayOnScreen is set display the debug
// message on screen as well and wait until the user has closed
// the message box.  If a message box was used make sure any window
// it overlapped is properly refreshed after it is closed.


extern void LE_ERROR_EnableDebugText(void);
// If debug text has been disabled this will enable it again.

extern void LE_ERROR_DisableDebugText(void);
// Disables debug output to text file.  Use this and the
// previous function to control debug output for specific
// sections of code only.  Initial start up of debug text
// is enabled.
#endif



extern int LE_ERROR_MsgBox(char *MESSAGE, char *TITLE);
// Allow the user to put a MessageBox on the screen with a specified
// title, message.  Returns the button pressed.

extern void LE_ERROR_Msg (char *MessageString, LE_ERROR_OutputType OutputType, BOOL Fatal);
// This will write the E_MESSAGE string to various locations, as defined by
// LE_ERROR_OutputType.  A numeric status code will be prepended to the string,
// also defined by LE_ERROR_OutputType.  The function will cause the program
// to exit if Fatal is TRUE.

extern void LE_ERROR_ErrorMsg(char *E_MESSAGE);
// Obsolete function for fatal error messages.  This will write the
// E_MESSAGE string to the currently opened debug out file.  It will
// append "3!: " to the front of this string when writing to the file
// for easy searching.  Will also display the error message on screen
// in a message box and wait for user to close the box before continuing.
// After the user has closed the dialog box this will force the
// application to exit making sure processes the atexit commands
// so everything is properly cleaned up.

#endif // __L_ERROR_H__
