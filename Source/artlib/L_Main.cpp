/*****************************************************************************
 *
 * FILE:        L_Main.cpp
 *
 * DESCRIPTION: Main program loop and entry point.  In Windows this also has
 *              the window message processor.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Main.cpp 22    25/09/99 1:43p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Main.cpp $
 * 
 * 22    25/09/99 1:43p Agmsmith
 * Have an icon for the window so that the task bar shows the icon beside
 * the program name.
 * 
 * 21    9/21/99 15:21 Davew
 * Fixed the F10 minimizing bug
 * 
 * 20    9/20/99 12:35 Davew
 * Fixed some shutdown problems under Win98.  Using ALT+F4 to close the
 * application would bypass any of the apps shutdown code.  This usually
 * means memory leaks.
 * 
 * 19    99/09/08 1:49p Agmsmith
 * Avoid lockups under Win2000 when error message dialog box is showing by
 * not processing certain Windows messages when the box is up.
 *
 * 18    6/25/99 5:08p Agmsmith
 * Need to call the default window proc for some messages, like
 * WM_ACTIVATE, so that DirectX gets a shot at restoring the screen window
 * (so you can alt-tab back to the game).
 *
 * 17    6/25/99 4:34p Agmsmith
 * Update drawing area to match actual window size after the screen mode
 * has changed (since it can shrink the window).
 *
 * 16    6/25/99 11:55 Davew
 * Fixed the PC3D dependencies
 *
 * 15    6/22/99 12:50 Davew
 * Dependency changes from pc3d
 *
 * 14    6/08/99 14:12 Davew
 * Added some code to handle the SC_CLOSE system command and reinstated
 * the default message handling by PC3D when it is being used.
 *
 * 13    6/02/99 14:37 Davew
 * Fixed a bug where the window proc was not handling certain messages
 * properly
 *
 * 12    21/04/99 15:52 Timp
 * Make Unicode command line args look like ASCII command line args.
 *
 * 11    3/22/99 4:23p Agmsmith
 * We don't do double clicks.
 *
 * 10    3/22/99 10:43a Timp
 * Changed GetCommandLine to GetCommandLineW.  (Now compiles in Unicode
 * mode)
 *
 * 9     3/19/99 2:18p Agmsmith
 * Start the timers before updating the screen, otherwise it hangs when in
 * immediate command execution mode.
 *
 * 8     2/02/99 3:59p Agmsmith
 * Have your own device context for the window, so that font size settings
 * don't vary randomly.
 *
 * 7     12/18/98 5:42p Agmsmith
 * New global names.
 *
 * 6     11/27/98 3:14p Agmsmith
 * Update destination rectangle when window moves.  Not that we really use
 * the destination rectangle.
 *
 * 5     11/26/98 5:00p Agmsmith
 * Moving C language DDraw init stuff into PC3D.h rather than L_DDInit.h.
 *
 * 4     11/06/98 7:14p Agmsmith
 * Trying to get the single tasking mode to work.
 *
 * 3     9/22/98 1:44p Agmsmith
 * Disable timer stuff if no timers.
 *
 * 2     9/16/98 11:06a Agmsmith
 * It compiles!
 *
 * 1     9/16/98 10:29a Agmsmith
 * Main program loop, and Windows message processing.  Enter here!
 ****************************************************************************/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemMain

#if CE_ARTLIB_EnableSystemGrafix3D
  #include "../pc3d/PC3DHdr.h"
#endif

#if CE_ARTLIB_PlatformWin95 // See further down for non-Windows games.

#include <resource.h>



/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

HINSTANCE  LE_MAIN_Hinst;             // current instance of this application
HWND       LE_MAIN_HwndMainWindow;    // handle to main application window
char       LE_MAIN_ApplicationNameString[] = CE_ARTLIB_ApplicationName;
                                      // Name of this program, used in window titles etc.
ACHAR     *LE_MAIN_CommandLine;       // Command line from Windows, Unicode or ASCII as requested.
BOOL       LE_MAIN_ApplicationActive; // true if our window is frontmost and active.



/************************************************************/
/* STATIC VARIABLES                                         */
/*----------------------------------------------------------*/

static LPJOYINFOEX joyinfoex;
static short ExitReturnCode = EXIT_SUCCESS; // Return code from the program as a whole.

#if CE_ARTLIB_EnableMultitasking
static HANDLE hGameControlThread; // Handle to user's game thread.
#endif



/*****************************************************************************
 * Processes Windows Messages.  Quickly.
 *
 * Note that there is a glitch in Windows itself where the internal kernel
 * message queue (not the application one) grows rather than overflowing
 * and throwing away messages.  Normally this is OK, but their queue code
 * is really inefficient (probably moving the whole queue array down by one
 * whenever a message is removed, rather than using a linked list or head/tail
 * indices) so when the queue gets too large, Windows gets really slow (mouse
 * movements are very sluggish, etc).  If the game thread doesn't process
 * messages fast enough, they back up and the queue gets too large.  This
 * is made a lot worse by the multimedia timer, which I suspect also goes
 * through the kernel's internal queue (and thus pumps out 60 messages per
 * second).  If you leave the system idle for a minute, it reduces the size
 * of the queue and swap file to match actual needs.  But the game is never
 * idle, so...  Also, you may notice extreme sluggishness while sitting at
 * a breakpoint in the debugger - yes the timer keeps on running while your
 * program is stopped (and gets a zillion timer callbacks in a row when you
 * leave the breakpoint).
 */

LRESULT CALLBACK LE_MAIN_WinProc (HWND hWnd, UINT message, WPARAM uParam, LPARAM lParam)
{
  PAINTSTRUCT       MyPaint;

#if CE_ARTLIB_EnableSystemMouse
  #if CE_ARTLIB_EnableSystemTimers
  TYPE_Tick         ElapsedMouseMoveTime;
  #endif
  static TYPE_Tick  LastMouseMoveTime;
#endif

#if CE_ARTLIB_EnableSystemJoypad
  joySetCapture(hWnd, JOYSTICKID1, 1, FALSE);
  joyGetPosEx(JOYSTICKID1, joyinfoex);
#endif

  switch (message)
  {
  case WM_ENDGAME:
    ExitReturnCode = (short) lParam;

#if !CE_ARTLIB_EnableMultitasking
    GameShutdown ();
#endif

    // Kill the sequencer while the window is still up, so that it doesn't
    // draw on the desktop after the window is closed.
    LE_INIT_SystemRemoval ();

#if CE_ARTLIB_EnableMultitasking
    CloseHandle (hGameControlThread);
    hGameControlThread = NULL;
#endif

    // Note that shutting down the system makes DirectDraw send some internal
    // messages to the window to shut itself down (it hooks into the window),
    // so rather than destroying it here, post a close message which will get
    // done after the DirectDraw shutdown is complete.
    PostMessage (hWnd, WM_CLOSE, 0, 0);
    return 0;


  case WM_SYSCOMMAND:
    switch (uParam)
    {
      case SC_CLOSE:
        PostMessage (LE_MAIN_HwndMainWindow, WM_ENDGAME, 0, 0);
        return 0;
      case SC_KEYMENU:  // Avoid the F10 minimize bug
        return(0);
    }
    break;

  case WM_CLOSE:
    DestroyWindow (hWnd);
    return 0; // No DefWndProc exists after this finishes!


  case WM_DESTROY:
    PostQuitMessage (ExitReturnCode);
    break;


  case WM_PAINT:
    // Happens when a dialog box pops up in debug mode on our screen.
    BeginPaint (hWnd, &MyPaint);
#if CE_ARTLIB_EnableSystemGrafix
    if (LE_MAIN_ApplicationActive)
      LE_GRAFIX_InvalidateRect (&MyPaint.rcPaint);
#endif
    EndPaint (hWnd, &MyPaint);
    return 0; // We do all the drawing, don't need DefWndProc.


  case WM_SIZE:
  case WM_MOVE:
    // Update our "screen" coodinates in which all drawing takes place
    // to match the current size of the window's client area.
    GetClientRect (hWnd, &LE_GRAFIX_ScreenDestinationRect);
    ClientToScreen (hWnd, (LPPOINT) &LE_GRAFIX_ScreenDestinationRect.left);
    ClientToScreen (hWnd, (LPPOINT) &LE_GRAFIX_ScreenDestinationRect.right);
    break;


  case WM_ACTIVATEAPP:
    LE_MAIN_ApplicationActive = (BOOL) uParam;
    if (LI_ERROR_DialogBoxShowing > 0)
      break; // Do nothing if a dialog box is showing.
    if (LE_MAIN_ApplicationActive)
    {
      // Take over the keyboard, otherwise in release mode we lose keystrokes
      // after dialog boxes pop up etc.  This may also force Windows to get
      // rid of the native mouse pointer after alt-tabs.
      SetFocus (hWnd);
      // Restart the clock (unpause the game in effect).
#if CE_ARTLIB_EnableSystemTimers
      LE_TIMERS_Suspend (FALSE);
#endif
      // Refresh the display when we get control again.
#if CE_ARTLIB_EnableSystemGrafix
      LE_GRAFIX_InvalidateRect (&LE_GRAFIX_ScreenDestinationRect);
#endif
#if CE_ARTLIB_EnableSystemSequencer
      LE_SEQNCR_SuspendEngine (FALSE);
#endif
    }
    else // Deactivating this program.
    {
#if CE_ARTLIB_EnableSystemTimers
      LE_TIMERS_Suspend (FALSE); // Need timers running for Sequencer suspend.
#endif
#if CE_ARTLIB_EnableSystemSequencer
      LE_SEQNCR_SuspendEngine (TRUE); // Returns when suspend complete.
#endif
#if CE_ARTLIB_EnableSystemTimers
      LE_TIMERS_Suspend (TRUE); // Stop the timers so game doesn't run much.
#endif
    }
    break;

  case WM_CHAR:
#if CE_ARTLIB_EnableSystemKeybrd
    LI_KEYBRD_UpdateKeybrd(message, uParam, lParam);
#endif
    break;


#if CE_ARTLIB_EnableSystemKeybrd
  case WM_KEYDOWN:
  case WM_KEYUP:
    if (LI_ERROR_DialogBoxShowing > 0)
      break; // Do nothing if a dialog box is showing.
    if (!LE_MAIN_ApplicationActive)
      break; // Ignore user input while inactive.  Avoids message queue growth when sequencer paused.
#if CE_ARTLIB_EnableBitmapCapture
    switch((char)uParam)
    {
    case 'O':
    case 'o':
      if (GetKeyState(VK_CONTROL) & 0x80000000)
      {
        if ((nScreenBitsPerPixel == 24) || (nScreenBitsPerPixel == 32))
          LI_ANIMbOutputBitmapsToCDrive = TRUE;
      }
      break;
    case VK_SPACE:
      if (LI_ANIMbOutputBitmapsToCDrive)
      {
        LI_ANIMbOutputBitmapsToCDrive = FALSE;
        LI_ANIMnOutputBitmapNumber = ((LI_ANIMnOutputBitmapNumber + 999) / 1000) * 1000;
      }
      break;
    }
#endif
    LI_KEYBRD_UpdateKeybrd(message, uParam, lParam);
#if CE_ARTLIB_EnableSystemButton && !CE_ARTLIB_ButtonsUseUIMSGInput
    LI_BUTTON_ProcessWindowsMessage( message, uParam, lParam );
#endif
    return 0L;


  case WM_SYSKEYDOWN:
  case WM_SYSKEYUP:
    if (LI_ERROR_DialogBoxShowing > 0)
      break; // Do nothing if a dialog box is showing.
    if (!LE_MAIN_ApplicationActive)
      break; // Ignore user input while inactive.  Avoids message queue growth when sequencer paused.
    LI_KEYBRD_UpdateKeybrd(message, uParam, lParam);
    break;
#endif


#if CE_ARTLIB_EnableSystemMouse
  case WM_MOUSEMOVE:
    if (LI_ERROR_DialogBoxShowing > 0)
      break; // Do nothing if a dialog box is showing.
    if (!LE_MAIN_ApplicationActive)
      break; // Ignore user input while inactive.  Avoids message queue growth when sequencer paused.
    // These mouse updates can be very expensive (spinning through a list
    // of 75 buttons, twice, looking for intersecting rectangles).  So
    // only give it the move messages occasionally.  This is particularly
    // useful for skipping zillions of messages that back up in the queue
    // sometimes and then get processed in a bunch.
#if CE_ARTLIB_EnableSystemTimers
    ElapsedMouseMoveTime = LE_TIME_TickCount - LastMouseMoveTime;
    if (ElapsedMouseMoveTime < 1) // Only update the mouse at 60hz tops.
      break;
#endif
    LI_MOUSE_UpdateMouse(message, lParam);
#if CE_ARTLIB_EnableSystemButton && !CE_ARTLIB_ButtonsUseUIMSGInput
    LI_BUTTON_ProcessWindowsMessage( message, uParam, lParam );
#endif
#if CE_ARTLIB_EnableSystemTimers
    LastMouseMoveTime = LE_TIME_TickCount;
#endif
    break;
#endif // CE_ARTLIB_EnableSystemMouse


#if CE_ARTLIB_EnableSystemMouse
  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_RBUTTONDOWN:
  case WM_RBUTTONUP:
    if (LI_ERROR_DialogBoxShowing > 0)
      break; // Do nothing if a dialog box is showing.
    if (!LE_MAIN_ApplicationActive)
      break; // Ignore user input while inactive.  Avoids message queue growth when sequencer paused.
    LI_MOUSE_UpdateMouse(message, lParam);
#if CE_ARTLIB_EnableSystemButton && !CE_ARTLIB_ButtonsUseUIMSGInput
    LI_BUTTON_ProcessWindowsMessage( message, uParam, lParam );
#endif
    return 0L;
#endif // CE_ARTLIB_EnableSystemMouse


#if CE_ARTLIB_EnableSystemJoypad
  case MM_JOY1MOVE:
  case MM_JOY1BUTTONDOWN:
  case MM_JOY1BUTTONUP:
    LI_JOYPAD_UpdateJoypad(message, uParam, lParam);
    return 0L;
#endif


#if CE_ARTLIB_EnableSystemMidi
  case MM_MCINOTIFY:
    LE_MIDI_ProcessMidiNotifies(uParam, lParam);
    return 0L;
#endif
  }

  // If you fall through the case statement (use "return" to avoid it) then
  // the message gets passed on to the default window message processor.

  #if CE_ARTLIB_EnableSystemGrafix && CE_ARTLIB_EnableSystemGrafix3D
    return(DDRAW_WindowProc(hWnd, message, uParam, lParam));
  #else
    // Passes it on if unproccessed
    return(DefWindowProc(hWnd, message, uParam, lParam));
  #endif
}



/*****************************************************************************
 * Windows recognizes this function by name as the initial entry point
 * for the program.  This function calls the application initialization
 * routine.  It then executes a message retrieval and dispatch loop that
 * is the top-level control structure for the remainder of execution.
 * The loop is terminated when a WM_QUIT message is received, at which time
 * this function exits the application instance by returning the value passed
 * by PostQuitMessage().
 */

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  MSG           msg;
  HMENU         hMenu;
  unsigned int  nIDThread = 0;
  WNDCLASS      wc;
  RECT          WindowRect;
  DWORD         WindowStyle;
  BOOL          bGameAborted  = FALSE;

  // Set up MENU_EXISTS to be TRUE if there is a menu bar for our window.

#ifdef CE_RESRCE_MAIN_MENU
  #define MENU_EXISTS 1
#else
  #define MENU_EXISTS 0
#endif

  // Get the command line string, Unicode or ASCII.

#if CE_ARTLIB_UnicodeAlphabet
  LE_MAIN_CommandLine = GetCommandLineW ();

  // lpCmdLine doesn't include program name.
  // Skip over it for Unicode command string version.
  while ((!iswspace(*LE_MAIN_CommandLine)) &&
         (*LE_MAIN_CommandLine != 0))
    LE_MAIN_CommandLine++;
  while (iswspace(*LE_MAIN_CommandLine))
    LE_MAIN_CommandLine++;
#else // ASCII single byte per character.
  LE_MAIN_CommandLine = lpCmdLine;
#endif

  // Save the instance handle in static variable, which will be used for
  // loading resources, DLLs and other Windows things.

  LE_MAIN_Hinst = hInstance;

  // Create (register) our window class.

  wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc   = LE_MAIN_WinProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = hInstance;
  wc.hIcon         = LoadIcon (LE_MAIN_Hinst, MAKEINTRESOURCE (RESOURCE_ICON_PROGRAM));
  wc.hCursor       = NULL;
  wc.hbrBackground = (HBRUSH) GetStockObject (GRAY_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = LE_MAIN_ApplicationNameString; // Reuse game name as class name.

  if (!RegisterClass (&wc))
    return EXIT_FAILURE;

  // Create a main window for this application.  Later on, DirectDraw will
  // hook into it for interfacing with the Windows system (minimizing it
  // when the program is deactivated, taking over the screen when it is
  // reactivated, etc).  We also get our mouse and keyboard messages from it.
  // And if we need a dialog box (like when printing), we can make the
  // dialog a child of it.  In debug mode, DirectDraw draws into a window,
  // with a title bar.  In release mode it takes over the whole screen.

  memset (&WindowRect, 0, sizeof (WindowRect));

#if CE_ARTLIB_EnableDebugMode
  #if MENU_EXISTS // If a menu exists, need a title bar etc.
  WindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_VISIBLE;
  #else
  WindowStyle = WS_POPUP | WS_VISIBLE | WS_SYSMENU;
  #endif
  WindowRect.right = CE_ARTLIB_InitialScreenWidth;
  WindowRect.bottom = CE_ARTLIB_InitialScreenHeight;
  AdjustWindowRect (&WindowRect, WindowStyle, MENU_EXISTS);
#else // Full screen window with no title or borders or menu.
  WindowStyle = WS_POPUP | WS_VISIBLE | WS_SYSMENU;
  WindowRect.right = CE_ARTLIB_InitialScreenWidth;
  WindowRect.bottom = CE_ARTLIB_InitialScreenHeight;
#endif

#ifdef RESOURCE_MENU_MAIN
    hMenu = LoadMenu (LE_MAIN_Hinst, MAKEINTRESOURCE (RESOURCE_MENU_MAIN));
#else // No menu defined.
    hMenu = NULL;
#endif

  LE_MAIN_HwndMainWindow = CreateWindow (
    LE_MAIN_ApplicationNameString, /* Window class name. */
    LE_MAIN_ApplicationNameString, /* Text for window title. */
    WindowStyle,
    WindowRect.left,
    WindowRect.top,
    WindowRect.right - WindowRect.left,
    WindowRect.bottom - WindowRect.top,
    NULL, /* Overlapped windows have no parent. */
    hMenu,
    LE_MAIN_Hinst, /* This instance owns this window. */
    NULL /* We don't use any data in our WM_CREATE */
    );

  if (LE_MAIN_HwndMainWindow == NULL)
    return EXIT_FAILURE;

  ShowWindow (LE_MAIN_HwndMainWindow, nCmdShow);

  // Now that the window is up, start up the Artech library.

  LE_INIT_SystemInit();

  // Since it may have changed screen mode, and thus window size,
  // make the library update to use the current window size by
  // sending a fake WM_MOVE message.

  PostMessage (LE_MAIN_HwndMainWindow, WM_MOVE, 0, 0);

#if CE_ARTLIB_EnableMultitasking
  hGameControlThread = (HANDLE) _beginthreadex (NULL, 0,
    &GameLoop, NULL, 0, &nIDThread);
  if (hGameControlThread == NULL)
    return EXIT_FAILURE;

  SetThreadPriority (hGameControlThread, CE_ARTLIB_GameThreadPriority);

  // Acquire and dispatch messages until a WM_QUIT message is received.

  while (GetMessage (&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
#else // Not a multitasking game, do an update loop instead.
  if (GameStartup ())
  {
    BOOL bFinished = FALSE;

    while (!bFinished)
    {
      // Receive input.
      while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
      {
        if (msg.message == WM_QUIT)
        {
          bFinished       = TRUE;
          bGameAborted    = TRUE;
          ExitReturnCode  = msg.wParam; // Abrupt exit caused by system shutdown or kill.
          break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }

      // If we're not finished, update the game
      if (!bFinished)
      {
        // Calculate new game state.
        if (GameUpdateCycle())
        {
          // Draw the graphics on the screen.
          LI_TIMER_CallCyclicFunctions ();
        }
        else
          bFinished = TRUE; // Game wants to exit.
      }
    }
  }

  // If the game wasn't aborted, do the regular shutdown processing
  if (!bGameAborted)
  {
    // Tell the window to close and also shut down the library.
    PostMessage (LE_MAIN_HwndMainWindow, WM_ENDGAME, 0, 0);

    // Wait for the WM_QUIT message, which gives DirectDraw time to close down.
    while (GetMessage (&msg, NULL, 0, 0))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
#endif // CE_ARTLIB_EnableMultitasking

  return ExitReturnCode;
}




/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#else // Not the Windows platform.

/*****************************************************************************
 * Main program entry point for non-Windows games.
 */

int main (int argc, char *argv[])
{
  return 0;
}

#endif // CE_ARTLIB_PlatformWin95

#endif // CE_ARTLIB_EnableSystemMain
