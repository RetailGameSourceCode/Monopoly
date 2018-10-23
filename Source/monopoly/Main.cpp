/*****************************************************************************
 *
 * FILE:        Main.cpp
 * DESCRIPTION: Just shows a bit of what your main program would do.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Projects - 1999/Mono 2/Main.cpp 37    10/20/99 1:16p Davide $
 *****************************************************************************
 * $Log: /Projects - 1999/Mono 2/Main.cpp $
 * 
 * 37    10/20/99 1:16p Davide
 * when playing on a custom board, the DISPLAY_state.city flag is set to
 * -1. This greatly affected the display of deeds, cards and sound files
 * for versions other than USA.  A small check is now made. If
 * DISPLAY_state.city is -1, a temp variable is used instead and it is set
 * to iLandId-2.
 * 
 * 36    10/04/99 2:47p Russellk
 * 
 * 35    9/30/99 3:52p Russellk
 * POSSIBLE FIX for slowdown bug
 * 
 * 34    26/09/99 2:10p Agmsmith
 * Removed old voice chat system, in preparation for new one.
 * 
 * 33    99/09/20 1:45p Timp
 * Fixed problem with garbage deed text.
 * 
 * 32    9/17/99 4:53p Russellk
 * 
 * 31    8/27/99 15:41 Davew
 * Removed the InitSearchPaths() function in favor of the new
 * UDUTILS_GenerateINIFile() call.
 * 
 * 30    8/20/99 3:08p Agmsmith
 * Fix paths to correspond to current CD and hard drive layout.
 * 
 * 29    8/20/99 1:30p Agmsmith
 * Now initialises search paths for data files and videos.
 *
 * 28    8/09/99 13:34 Davew
 * Added a Remove call for the voice stuff.  Fixes some memory leaks.
 *
 * 27    8/09/99 13:19 Davew
 * Re-enabled the removal of the chat system
 *
 * 26    7/20/99 11:56a Russellk
 * Removed datafile close lines - let shutdown close datafiles.
 *
 * 25    7/19/99 3:05p Agmsmith
 * Added limit count to avoid lockup in single tasking mode with too many
 * messages per cycle.
 *
 * 24    7/05/99 4:39p Russellk
 * Mouse render slot removed
 *
 * 23    7/05/99 3:25p Russellk
 * Adjustments to 3d slot init for non 800x600 res
 *
 * 22    6/10/99 3:05p Russellk
 * moved display_shutdown in front of language shutdown stuff
 *
 * 21    6/07/99 9:55a Davide
 *
 * 20    6/03/99 11:56 Davew
 * Moved some render slot initialization calls to the InitRenderSlots
 * function.
 *
 * 19    6/01/99 11:58 Davew
 * Added a new InitRenderSlots() function
 *
 * 18    5/31/99 3:48p Timp
 * Added language and chat cleanup calls.
 *
 * 17    5/31/99 12:20p Davide
 *
 * 16    5/12/99 2:15p Russellk
 * Final E3 modifications complete
 *
 * 15    5/10/99 5:10p Russellk
 *
 * 14    5/07/99 8:44a Russellk
 * E3 demo revisions - first acceptable version.
 *
 * 13    5/05/99 10:15a Philla
 * email version.  trigger an email test.
 *
 * 12    5/03/99 10:59a Philla
 * Save prompt override working properly.
 *
 * 11    5/03/99 10:04a Philla
 * Save Game file prompt override (for email).
 *
 * 10    27/04/99 19:32 Timp
 * Small Unicode fixes
 *
 * 9     4/26/99 4:24p Russellk
 * Some stuff in for E3 demo mode
 *
 * 8     4/20/99 1:45p Kens
 * set proper clipping RECT for slot 2.
 *
 * 7     3/26/99 9:27 Davew
 * Fixed a missing brace
 *
 * 6     3/26/99 9:10a Russellk
 * Restored history version
 *
 * 4     3/17/99 8:48a Russellk
 * Trade viewer partially implemented
 *
 * 3     2/03/99 2:52p Russellk
 * Library update
 *
 * 23    2/01/99 2:20p Agmsmith
 * Simplified way of making a video, using C++ default arguments.
 *
 * 22    1/31/99 3:36p Agmsmith
 *
 * 21    1/20/99 11:19a Agmsmith
 * Use a generic copyright available picture.
 *
 * 20    1/17/99 3:21p Agmsmith
 * Use MAX_PATH rather than _MAX_PATH, they're the same.
 *
 * 19    1/12/99 12:53p Agmsmith
 * Now with a data file.
 *
 * 18    1/04/99 2:31p Agmsmith
 * Update for new global screen size variable names.
 *
 * 17    12/30/98 2:57p Agmsmith
 * Update data file name.
 *
 * 16    12/30/98 2:55p Agmsmith
 * Now does 3D in the example too.
 *
 * 15    12/18/98 5:43p Agmsmith
 * New global names.
 *
 * 14    11/17/98 12:13p Agmsmith
 * Play a sound file.
 *
 * 13    11/10/98 12:32p Agmsmith
 * Single tasking now works with the mouse.  Better separation of standard
 * code parts (startup, process, shutdown).
 *
 * 12    11/06/98 7:13p Agmsmith
 * Adapt sample program to work in single tasking mode or multitasking
 * mode, except for mouse problems with sequencer.
 *
 * 11    11/06/98 4:33p Agmsmith
 * Just the basic no-datafile-needed demo, and initial program.
 *
 * 10    11/02/98 5:21p Agmsmith
 * Added background picture from an external BMP file.
 *
 * 9     10/29/98 6:37p Agmsmith
 * New improved memory and data system added.  Memory and data systems
 * have been separated.  Now have memory pools, and optional corruption
 * check.  The data system has data groups rather than files and
 * individual items can come from any data source. Also has a least
 * recently used data unloading system (rather than programmer set
 * priorities).  And it can all be turned off for AndrewX!
 *
 * 8     10/16/98 2:06p Agmsmith
 *
 * 7     10/16/98 2:04p Agmsmith
 * Cleaned up data file, now called TestData, not CO.
 *
 * 6     10/14/98 3:33p Tomasz
 *
 * 5     9/22/98 4:33p Agmsmith
 * Added video player.
 *
 * 4     9/22/98 4:25p Agmsmith
 * Most features now work.
 *
 * 3     9/16/98 5:02p Agmsmith
 * Main program can now shut down the library!
 *
 * 2     9/16/98 11:08a Agmsmith
 * First version that compiles!  Doesn't do anything, other than exit.
 *
 * 1     9/14/98 4:53p Agmsmith
 * Starting a new library sample program for ArtLib '99.
 ****************************************************************************/

#include "GameInc.h"



/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

static  LE_DATA_DataId  BackgroundPicture = LE_DATA_EmptyItem;
static  int             BackgroundXPosition;
static  LE_DATA_DataId  MousePointer = LE_DATA_EmptyItem;

// Prototypes ================================================================
void InitRenderSlots (void);



/*****************************************************************************
 * Initialise our game.  Returns TRUE if successful.
 */

BOOL GameStartup (void)
{
  // Add the directory with data files and videos on the CD to the
  // search path to use.
  UDUTILS_GenerateINIFile();

  // Initialize the render slots
  InitRenderSlots();

  // Ordinary animations show up in slot 0 (screen) and 2 (sound),
  // 3D ones use the sound and the 3D slot (each slot only accepts
  // animations it can play).

  //LE_SEQNCR_SetDefaultRenderSlotSetForStartSequence ((1 << 0) | (1 << 1) | (1 << 2));
//  LE_SEQNCR_SetDefaultRenderSlotSetForStartSequence ((1 << 2) | (1 << 1) | (1 << 3));//.2d, 3d, sound
//
//  // Set up the default preloading behaviour we want.  Either you have a
//  // pause while all of an animation is preloaded, and then it plays
//  // well, or you have it start right away but play slowly as it loads
//  // while playing.
//
//  LE_SEQNCR_SetDefaultLoaderOptionsForStartSequence (
//    TRUE /* PreloadData */, LE_SEQNCR_DATA_DISPOSE_DO_NOTHING,
//    FALSE /* UseReferenceCounts */);
//
//  // Start up the mouse group.  Everything inside the group
//  // moves with the mouse.
//
//  LE_SEQNCR_InitialiseMouseGroupingSequence ((1<<3) | (1<<2), 1234);

  // Add a graphic for the mouse pointer.  Make a runtime image for the mouse.

  MousePointer = LE_GRAFIX_ObjectCreate (60, 20, LE_GRAFIX_ObjectTransparent);
  if (MousePointer == LE_DATA_EmptyItem)
    return FALSE;
  LE_FONTS_Print (MousePointer, 0, 0,
    LE_GRAFIX_MakeColorRef (255, 255, 0), A_T("Mouse"));
  LE_SEQNCR_AddMouseSubSequenceTheWorks (MousePointer,
    100 /* Priority relative to other mouse pointers */,
    -22 /* HotSpotXOffset */,
    13 /* HotSpotYOffset */,
    FALSE /* DeleteOtherMouseSequences */);

  // Sliding Background picture.  Again made at run time.

  //BackgroundPicture = LE_GRAFIX_ObjectCreate ((short) LE_GRAFIX_VirtualScreenWidthInPixels,
  //  (short) LE_GRAFIX_VirtualScreenHeightInPixels, LE_GRAFIX_ObjectNotTransparent);

  // Timer setup.  Timer #1 moves the sliding background regularly,
  // trying to do it at 20 hz.

  LE_TimerSpeeds [1] = 1; // This one has units of 60hz.
  LE_TimerRestartCount [1] = 3; // Every 3/60 second.
  LE_TimerSendUIMessage[1] = TRUE;
  LE_Timers[1] = 0; /* Fire up the timer, with 10 second initial delay. */

  if( !MainExtendedInitialization() )
    return FALSE;

  return TRUE;
}


// ===========================================================================
// Function:    InitRenderSlots
//
// Description: Initializes the render slots for the game
//
// Params:      void
//
// Returns:     void
//
// Comments:
// ===========================================================================
void InitRenderSlots (void)
{
  TYPE_Point2D    CameraCenterWorldPosition;

  CameraCenterWorldPosition.x = LE_GRAFIX_VirtualScreenWidthInPixels / 2;
  CameraCenterWorldPosition.y = LE_GRAFIX_VirtualScreenHeightInPixels / 2;

  // Main animation surface is slot zero.  Used only for the 2D board backdrops.
  LE_REND2D_InstallInRenderSlot(0, CameraCenterWorldPosition, 0.0F /* Rotation */,
                                1.0F /* Scale */,
                                LE_GRAFIX_ScreenSourceRect,
                                1000 /* Off screen distance */,
                                1 /* Camera label */,
                                LE_GRAFIX_DDWorkingSurfaceN, LE_GRAFIX_DDPrimarySurfaceN,
                                (LE_REND_RenderSlotSet)7 /* Overlapping slot set */);

#if CE_ARTLIB_EnableSystemGrafix3D
  // 3D Graphics are in slot 1
  //RECT r3DWorld = { 0, 0, 800, 450 };
  RECT r3DWorld = { 0, 0, LE_GRAFIX_VirtualScreenWidthInPixels, LE_GRAFIX_VirtualScreenHeightInPixels * 3 / 4 };
  LE_REND3D_InstallInRenderSlot(1, r3DWorld, LE_GRAFIX_DDWorkingSurfaceN,
                                LE_GRAFIX_DDPrimarySurfaceN,
                                (LE_REND_RenderSlotSet)7 /* Overlapping slot set */);
#endif

  // 2D Graphics above 3D stuff - all basic 2D stuff runs through here, everything used in the 3D version shows up here.
  LE_REND2D_InstallInRenderSlot(2, CameraCenterWorldPosition, 0.0F,
                                1.0F, LE_GRAFIX_ScreenSourceRect, 1000, 1,
                                LE_GRAFIX_DDWorkingSurfaceN,
                                LE_GRAFIX_DDPrimarySurfaceN,
                                (LE_REND_RenderSlotSet)7);

  // Sound effects are in slot 3.
  LE_REND0D_InstallInRenderSlot(3);

  // Mouse animation surface is slot four.  Nope, put it in 2 now to save on slots.
#if 0 // Unused code.
  LE_REND2D_InstallInRenderSlot(4, CameraCenterWorldPosition, 0.0F /* Rotation */,
                                1.0F /* Scale */,
                                LE_GRAFIX_ScreenSourceRect,
                                1000 /* Off screen distance */,
                                2 /* Camera label */,
                                LE_GRAFIX_DDWorkingSurfaceN,
                                LE_GRAFIX_DDPrimarySurfaceN,
                                (LE_REND_RenderSlotSet) 23 /* Overlapping slot set */);
#endif // Unused code.

  // Let the mouse system use world coordinates from slot 2.
  LE_MOUSE_RenderSlotToUseForWorldCoordinates = 2;

  LE_SEQNCR_SetDefaultRenderSlotSetForStartSequence ((1 << 2) | (1 << 1) | (1 << 3));//.2d, 3d, sound

  // Set up the default preloading behaviour we want.  Either you have a
  // pause while all of an animation is preloaded, and then it plays
  // well, or you have it start right away but play slowly as it loads
  // while playing.

  LE_SEQNCR_SetDefaultLoaderOptionsForStartSequence (
    TRUE /* PreloadData */, LE_SEQNCR_DATA_DISPOSE_DO_NOTHING,
    FALSE /* UseReferenceCounts */);

  // Start up the mouse group.  Everything inside the group
  // moves with the mouse.

  LE_SEQNCR_InitialiseMouseGroupingSequence ((1<<3) | (1<<2), 0xFFFF);
}


/*****************************************************************************
 * Process one user interface event message.  Returns FALSE if it
 * is time to end the game.
 */

BOOL ProcessUIMessage (LE_QUEUE_MessagePointer UIMessagePntr)
{
  // Monopoly message handler.
  ProcessLibraryMessage( UIMessagePntr );

  // First see if it is a key pressed message.
  if (UIMessagePntr->messageCode == UIMSG_KEYBOARD_PRESSED)
  {
    // Hit the escape key to end the game.

    if (UIMessagePntr->numberA == LE_KEYBRD_ESC)
    {// Request to back out of current game to menus to windows
    }

    if (UIMessagePntr->numberA == LE_KEYBRD_F4)
    {// With the ALT key means quit
      short filter = -1;
      short test;
      if( LE_KEYBRD_KeysPressed[LE_KEYBRD_CONTROL] )
        if ( (test = GetKeyState( VK_MENU )) & filter )
          return FALSE; // Time to exit!
    }


#if FOREMAILVERSION
    // BLEEBLE:  Cheap hack for now, until proper interface is implemented.
    if ( g_eActiveScreen == ST_GAME )
    {
      // Simulate sending a turn by e-mail.
      if ( UIMessagePntr->numberA == LE_KEYBRD_S )
      {
        wchar_t   wideFileName[50];

        MultiByteSzToWideSz( wideFileName, "move.msv", 50 );
        MESS_SendAction( ACTION_GET_GAME_STATE_FOR_SAVE, AnyLocalPlayer(), RULE_BANK_PLAYER,
         1, 1, 0, 0, wideFileName, 0, NULL );
      }
      // Simulate receiving a turn by e-mail.
      if ( UIMessagePntr->numberA == LE_KEYBRD_R )
      {
      }
    }
#endif

  }

  if( GameQuitRequested ) // Game requests a quit.
    return FALSE;
  else
    return TRUE; // Keep on playing...
}



/*****************************************************************************
 * Deallocate things and otherwise shut down the game.  Called by the library
 * in single tasking code.  Our own game code calls it in multitasking mode
 * to make this example smaller.
 */

void GameShutdown (void)
{
  int i, j;
  int LeftOverCommandLevel;

  DISPLAY_destroy();

  // Flush out all left over commands now.
  LeftOverCommandLevel = LE_SEQNCR_CollectCommands ();
  if (LeftOverCommandLevel != 1)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " GameShutdown: "
      "ExecuteCommands nested %d extra times at exit, should be zero.\r\n",
      LeftOverCommandLevel - 1);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
  }
  while (LE_SEQNCR_ExecuteCommands () > 0)
    ; // Drop the nested command level down to exactly zero.
  LE_SEQNCR_ProcessUserCommands ();

  // Kill the mouse before killing everything left in the render slots,
  // otherwise the mouse code does a redundant stop on the mouse sequence.

  LE_SEQNCR_RemoveMouseGroupingSequence ();

  LE_SEQNCR_StopAllRP (
    (LE_REND_RenderSlotSet) -1 /* All slots */, 0 /* All priorities */);

  LE_SEQNCR_ProcessUserCommands (); // So the stops get done NOW.

  // TIM - Removing these datafiles prior to shutdown can be problematic - if the screen attempts a redraw items attempt to reload, we get PC freezes 'n stuff.
  //LANG_CleanAndRemoveSystem();
  CHAT_CleanAndRemoveSystem();

#if !FORHOTSEAT
  // Remove reference counts for deed names so they can be unloaded
  for(i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
  {
#if USA_VERSION
    j = TMN_SQUARENAME_GO + (42 * max(DISPLAY_state.city, 0)) + i;
#else
    // set up a temp variable to point to the correct city when
    // playing a custom board
    int tempCity = DISPLAY_state.city;
    if(tempCity == -1)
        tempCity = iLangId -2;

    j = TMN_SQUARENAME_GO + (42 * max(tempCity, 0)) + i;
#endif
    LE_DATA_DataId item = LE_DATA_GetIndexedItemId(LED_IFT(DAT_LANG, 0), j);
    if (item != LED_EI)
      RULE_SquarePredefinedInfo[i].squareName = (wchar_t *)LE_DATA_Use(item);
    else
      RULE_SquarePredefinedInfo[i].squareName = A_T("*");

    // If the squarename starts with '*', get the name from the base set.
    // (Although they're not printed, "Chance"/"Community Chest" are like this
    // Also, this is needed for the US board's Electric Company/Water Works)
    if (RULE_SquarePredefinedInfo[i].squareName[0] == A_T('*')) {
      j = TMN_SQUARENAME_GO + i;
      item = LE_DATA_GetIndexedItemId(LED_IFT(DAT_LANG, 0), j);
    }
    if ((item != LED_EI) && (LE_DATA_GetRef(item)))  LE_DATA_RemoveRef(item);
  }
#endif

  if (BackgroundPicture != LE_DATA_EmptyItem)
  {
    LE_DATA_FreeRuntimeDataID (BackgroundPicture);
    BackgroundPicture = LE_DATA_EmptyItem;
  }

  if (MousePointer != LE_DATA_EmptyItem)
  {
    LE_DATA_FreeRuntimeDataID (MousePointer);
    MousePointer = LE_DATA_EmptyItem;
  }
}



#if !CE_ARTLIB_EnableMultitasking
/*****************************************************************************
 * The game's update cycle processing function.  This is called by the main
 * program loop in L_MAIN.cpp, once per screen update, if you are running
 * in non-multitasking mode (CE_ARTLIB_EnableMultitasking is off).  Note
 * that you should return quickly when in non-multitasking mode since the
 * screen won't be updated until you are finished here.  Return FALSE when
 * you want the game to exit.
 */

BOOL GameUpdateCycle (void)
{
  BOOL                    GameRunning = TRUE;
  int                     LimitCount = 50; // Avoid lockup when too many messages.
  LE_QUEUE_MessageRecord  UIMessage;
  TYPE_Tick               holdActionTick;
  static TYPE_Tick        nextActionTick = LE_TIME_TickCount;

  holdActionTick = LE_TIME_TickCount;
  if (GameRunning)
  {
    while( GameRunning && --LimitCount > 0 &&
    LE_UIMSG_ReceiveEvent (&UIMessage) != 0 )
    {// 'if' changed to 'while' - we should be moving fast enough to do this OK.
      GameRunning = ProcessUIMessage (&UIMessage);
      LE_QUEUE_FreeStorageAssociatedWithMessage (&UIMessage);
    }
    if( (nextActionTick < holdActionTick) && GameRunning )
    {
      DISPLAY_tickActions( holdActionTick - nextActionTick );
      nextActionTick = holdActionTick;
      if( (holdActionTick & 0xff) == 0 )
      {
        LE_TIMER_Delay(1);
        //TRACE("time %Xh\n", holdActionTick );
      }
    }
    else
    {
      if (LE_MAIN_ApplicationActive)
        Sleep (0); // Let other programs run - if framerate is always too low this will never get called.
      else // Game is inactive, don't use much CPU time.
        Sleep(20);
    }
  }

  return GameRunning;
}
#endif // !CE_ARTLIB_EnableMultitasking



#if CE_ARTLIB_EnableMultitasking
/*****************************************************************************
 * This game loop is used in Multitasking mode (CE_ARTLIB_EnableMultitasking
 * is TRUE).  The main program in L_MAIN.cpp fires off this code as a
 * separate thread, which runs independently of the animation engine, though
 * you can wait for a screen update by using LE_SEQNCR_WaitForFullUpdateCycle.
 * Note that the GameLoop function header matches what _beginthreadex expects
 * (see the actual Windows.h file, not the docs).
 */

unsigned int __stdcall GameLoop (void *ArgumentList)
{
  BOOL                    GameRunning = TRUE;
  LE_QUEUE_MessageRecord  UIMessage;
  TYPE_Tick               holdActionTick;
  TYPE_Tick       nextActionTick = LE_TIME_TickCount;


  if (GameStartup ())
  {
    while (GameRunning)
    {
      // In multitasking mode this returns once a message is ready,
      // going to sleep and not wasting CPU time while waiting.

      LE_UIMSG_WaitEvent (&UIMessage);

      // Now handle the message.

      GameRunning = ProcessUIMessage (&UIMessage);
      LE_QUEUE_FreeStorageAssociatedWithMessage (&UIMessage);

      // Monopoly display update
      holdActionTick = LE_TIME_TickCount;
      if( (nextActionTick < holdActionTick) && GameRunning )
      {
        DISPLAY_tickActions( holdActionTick - nextActionTick );
        nextActionTick = holdActionTick;
      }
    }

    GameShutdown ();
  }

  // Tell the library threads to exit.

  PostMessage (LE_MAIN_HwndMainWindow, WM_ENDGAME, 0, 0);

  return 0x1111; // Self destruct this thread.
}
#endif // CE_ARTLIB_EnableMultitasking
