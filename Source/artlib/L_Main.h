#ifndef __L_MAIN_H__
#define __L_MAIN_H__

/*****************************************************************************
 *
 * FILE:        L_Main.h
 *
 * DESCRIPTION: Main program loop and entry point.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/LibraryCode/L_Main.h 3     11/06/98 7:14p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/LibraryCode/L_Main.h $
 * 
 * 3     11/06/98 7:14p Agmsmith
 * Trying to get the single tasking mode to work.
 * 
 * 2     9/16/98 11:06a Agmsmith
 * GameLoop declaration now matches what BeginThread expects.
 * 
 * 1     9/16/98 10:29a Agmsmith
 * Main program loop, and Windows message processing.  Enter here!
 ****************************************************************************/


/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

#if CE_ARTLIB_PlatformWin95
extern HINSTANCE  LE_MAIN_Hinst;                    // current instance of this application
extern HWND       LE_MAIN_HwndMainWindow;           // handle to main application window
extern char       LE_MAIN_ApplicationNameString[];  // Name of this program, used in window titles etc.
extern ACHAR     *LE_MAIN_CommandLine;              // Command line from Windows, Unicode or ASCII as requested.
extern BOOL       LE_MAIN_ApplicationActive;        // true if our window is frontmost and active.
#endif


/************************************************************/
/* E X P O R T E D   F U N C T I O N S                      */
/*----------------------------------------------------------*/

#if CE_ARTLIB_PlatformWin95
  extern int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
#else
  extern int main (int argc, char *argv[]);
#endif



/************************************************************/
/* I M P O R T E D   F U N C T I O N S                      */
/*----------------------------------------------------------*/

#if CE_ARTLIB_EnableMultitasking
  extern unsigned int __stdcall GameLoop (void *ArgumentList);
  // Post a WM_ENDGAME message to end the game.  A separate thread
  // will be running this function, so don't return unless you want
  // to die.  Your game startup, loop, and shutdown are all done
  // in this function by your code.  Generally it will use UIMsg
  // to get user input & timer events, and go to sleep when nothing
  // is happening to avoid wasting CPU time.
#else
  extern BOOL GameStartup (void);
  // Initialise your game, return TRUE if it went OK.

  extern BOOL GameUpdateCycle (void);
  // Your code does what it needs to do, then returns after a short time
  // (otherwise the animations will slow down).  Returns TRUE if successful,
  // FALSE to exit the game.

  extern void GameShutdown (void);
  // The game is shutting down, deallocate your stuff.
#endif

#endif // __L_MAIN_H__
