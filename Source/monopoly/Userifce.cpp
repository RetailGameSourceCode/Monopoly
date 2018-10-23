/* Monopoly Program.
 * By Artech Digital Entertainments, copyright (c) 1999.
 *
 * Main interface theory:  First, the RULES engine, on the game host, stores all game state
 * information - scores, who's turn it is, property ownership, trade status - everything
 * primarily in 'RULE_GameStateRecord CurrentRulesState'.  This includes a phase stack (what
 * task is being done, FILO), a message FIFO queue for all actions and notifications the
 * processing of which may pop or push stack phases, and a hard to find state stack which
 * stores the entire game state for the primary purpose of allowing an undo feature in
 * case a player goes bankrupt.  This is achieved by stacking the RULE_GameStateRecord's.
 *
 * Second, the user interface code stores everything that should be reflected in the display.
 * The code responding to messages for setting the user interface variables kept in
 * 'RULE_GameStateRecord UICurrentGameState' is very similar here to the hotseat.  With the
 * 1999 version of Monopoly, the userifce.cpp code is a conversion of the Hotseats version.
 *
 * Third and all new, the display code stores enough detail about everything drawn on the screen
 * to allow it to compare what is up to what should be up.  The display state is in
 * 'DISPLAY_stateType DISPLAY_state'.  All screen updates occur by calling appropriate Display
 * routines which evaluate what should be on the screen by looking at UICurrentGameState, then
 * adjust what is there if needed by looking at and adjusting DISPLAY_state.  A routine which
 * calls each category of display update (DISPLAY_showAll2) will then completely
 * redraw the screen or facilitate a view change.  Note the UD* files can have some display state
 * info for their own module, and DISPLAY_showAll is called by a timer from main, not explicitely.
 *
 *
 * Tab stops are every 2 spaces, also save the file with spaces instead of tabs.
 *
 * $Header: /Projects - 1999/Mono 2/Userifce.cpp 165   10/20/99 1:16p Davide $
 *
 * $Log: /Projects - 1999/Mono 2/Userifce.cpp $
 * 
 * 165   10/20/99 1:16p Davide
 * when playing on a custom board, the DISPLAY_state.city flag is set to
 * -1. This greatly affected the display of deeds, cards and sound files
 * for versions other than USA.  A small check is now made. If
 * DISPLAY_state.city is -1, a temp variable is used instead and it is set
 * to iLandId-2.
 * 
 * 164   10/15/99 3:02p Russellk
 * Scaled Bink back in - library detects bltstretch capability before
 * trying it.
 * 
 * 163   99/10/13 11:18a Timp
 * Don't leave font in italics after printing deed text.
 * 
 * 162   10/12/99 4:30p Davide
 * 
 * 161   10/12/99 4:01p Davide
 * added the ability to save the custom board when saving a game
 * 
 * 160   10/12/99 10:59a Russellk
 * VIdeo scaling crashes Voodoo - avoided
 * 
 * 159   10/08/99 2:36p Russellk
 * end of game lock had a bug
 * 
 * 158   10/07/99 5:25p Russellk
 * file exit - have to kill the game (lock & never unlock)
 * 
 * 157   10/07/99 4:09p Davide
 * 
 * 156   10/07/99 12:31p Davide
 * 
 * 155   10/07/99 11:26a Davide
 * 
 * 154   10/07/99 11:13a Davide
 * added the ability to save and load the board in a saved game
 * 
 * 153   10/06/99 5:21p Russellk
 * 
 * 152   10/06/99 13:42 Davew
 * We now check for failure on DISPLAY_Initialize() and quit if it fails.
 * This was added so we can abort the performance test gracefully.
 * 
 * 151   10/06/99 12:04p Russellk
 * 
 * 150   10/06/99 10:18a Russellk
 * Load game init fix
 * 
 * 149   10/06/99 9:51a Philla
 * UDPSEL needs to see more messages.  Also whatever David did on my
 * machine.
 * 
 * 148   10/05/99 4:37p Russellk
 * Bink player call fix & some cash message fixes
 * 
 * 147   10/01/99 12:23p Davide
 * 
 * 146   30/09/99 1:49p Agmsmith
 * Now with out of order reception of voice chat messages so that the game
 * can ignore the regular message queue but still receive voice chat.
 * 
 * 145   9/30/99 1:21p Russellk
 * Sound for network disconnects
 * 
 * 144   9/30/99 8:57a Russellk
 * Pre - gold 1 update
 * 
 * 143   29/09/99 5:12p Agmsmith
 * 
 * 142   29/09/99 5:01p Agmsmith
 * Added voice chat.
 *
 * 141   9/29/99 4:19p Russellk
 *
 * 140   99/09/28 6:55p Timp
 * Send more messages to trade code.
 *
 * 139   28/09/99 6:43p Agmsmith
 * Voice chat under construction.
 *
 * 138   9/28/99 4:11p Davide
 *
 * 137   9/27/99 5:40p Russellk
 * Display message stuff & hotseat text formatting removed
 *
 * 136   26/09/99 2:10p Agmsmith
 * Removed old voice chat system, in preparation for new one.
 *
 * 135   9/24/99 1:05p Russellk
 *
 * 134   9/24/99 10:49a Philla
 * Allow players to join games.
 *
 * 133   9/23/99 5:15p Philla
 * AI wins don't count on hi-scores.
 *
 * 132   9/23/99 4:54p Russellk
 *
 * 131   9/23/99 1:47p Russellk
 * Faster rules stack evaluation
 *
 * 130   9/22/99 5:18p Davide
 * modified savegame code
 *
 * 129   9/22/99 4:11p Russellk
 *
 * 128   9/22/99 3:51p Philla
 * Auction timing at beginning fixed.  Waits for everyone now before
 * the "going once" is issued.
 *
 * 127   9/22/99 9:49a Russellk
 * Trademark start screen, bankrupt/victory sounds n anims, misc.
 *
 * 126   9/21/99 4:05p Russellk
 * killed f9 hotkey - options menu now
 *
 * 125   9/20/99 4:01p Russellk
 *
 * 124   20/09/99 2:21p Agmsmith
 * Don't use Bink on slow computers.
 *
 * 123   20/09/99 1:37p Agmsmith
 * Added Bink video player.
 *
 * 122   9/20/99 12:17p Philla
 * Skip movies if MS Zone Lobby stuff.
 *
 * 121   9/20/99 11:01a Russellk
 *
 * 120   18/09/99 4:52p Agmsmith
 * Play movies single size on slow computers and in 32 bit mode.
 *
 * 119   9/17/99 4:53p Russellk
 *
 * 118   9/17/99 1:08p Philla
 * Play two sounds at local/network screen if no hiscore info.
 *
 * 117   9/17/99 10:55a Philla
 * Switch phase logic.
 *
 * 116   99/09/17 9:52a Timp
 * Tab (chat window) key won't work unless you're in a network game.
 * And the window's so cute, too.... <sigh>
 *
 * 115   9/15/99 11:06a Philla
 * Credit winner at game over time.
 *
 * 114   9/13/99 2:35p Russellk
 *
 * 113   9/12/99 2:57p Russellk
 * Changed mouse up to mouse down for triggers to speed up response time
 *
 * 112   9/12/99 2:31p Philla
 * First draft at remove player from game (during setup).  Not tested yet.
 *
 * 111   9/12/99 1:51p Russellk
 * Some minor hotkeys reorganization
 *
 * 110   9/09/99 3:09p Russellk
 *
 * 109   9/07/99 3:51p Philla
 * Live update of rules screen for non-host players.
 *
 * 108   9/07/99 4:21p Russellk
 *
 * 107   9/03/99 4:04p Davide
 *
 * 106   9/03/99 1:13p Philla
 * Removed FOREVENNEWERPLAYERSELECTSTUFF compile flag stuff.
 *
 * 105   9/03/99 11:46a Philla
 * Cleaner switching in/out of player setup screens (ie for options
 * stuff).
 *
 * 104   9/01/99 10:14a Russellk
 * Deed generation moved to better location (notify game starting)
 *
 * 103   8/31/99 1:00p Russellk
 * modifed for dat_borde
 *
 * 102   8/30/99 12:30p Russellk
 * Look ma, US Beta.
 *
 * 101   8/27/99 15:42 Davew
 * Removed the UDUTILS_GenerateINIFile() call.  It has been move to
 * Main.cpp.
 *
 * 100   8/27/99 12:35 Davew
 * Added stuff to regenerate the Monopoly.ini file if it doesn't exist
 *
 * 99    99/08/25 1:50p Timp
 * Remove duplicates in UICurrentGameState.CountHits
 *
 * 98    8/23/99 3:53p Russellk
 * Jail anim begun, other improvements including new bezier structure
 *
 * 97    8/20/99 3:29p Russellk
 * paths for videos changed
 *
 * 96    8/19/99 5:00p Russellk
 * Token resting idles done except for refinements & final sequences
 *
 * 95    8/19/99 10:38a Russellk
 * Various enhancements - an interim checkin required to let other modules
 * compile.
 *
 * 94    8/16/99 12:43p Philla
 * Conflict with opening and player select stuff fixed.
 *
 * 93    8/16/99 11:29a Russellk
 * Rough change for when movies are on - Phill will refine.
 *
 * 92    8/16/99 9:43a Philla
 * compile bug with FOREVENNEWERPLAYERSELECTSTUFF turned off.
 *
 * 91    8/15/99 4:46p Philla
 * New phase stuff in setup.
 *
 * 90    8/13/99 10:43a Russellk
 * Hotkey adjustment for SW/HW switch - no longer needs to load low board.
 *
 * 89    99/08/12 11:22a Timp
 * Set a smaller size for the font.
 *
 * 88    99/08/11 6:31p Timp
 * Get the font, so chat init has something to work with. (blah)
 *
 * 87    8/11/99 10:22a Philla
 * Fix compile for new player select stuff.
 *
 * 86    8/11/99 10:16a Russellk
 * f4 & f5
 *
 * 85    8/10/99 9:45a Philla
 * Cleaned up (removed) old player select code.
 *
 * 84    8/09/99 11:44a Russellk
 * hot key for going through all boards.
 *
 * 83    8/09/99 10:49a Philla
 * F2 key works.  Except you don't get START button soon enough.
 *
 * 82    8/06/99 4:36p Russellk
 * City/regular board toggle enhancements
 *
 * 81    8/05/99 3:20p Mikeh
 * Necessary changes made due to splitting parts of UDSOUND.* into
 * UDPENNY.*
 *
 * 80    8/04/99 2:34p Russellk
 * Enhanced game queue lock logic - a timeout feature.
 *
 * 79    7/30/99 9:36a Russellk
 * Minor enhancements
 *
 * 78    7/28/99 4:06p Russellk
 * Updated player start with movies on to phils stuff
 *
 * 77    7/27/99 5:42p Russellk
 * modified f 9 key
 *
 * 76    7/21/99 12:23p Davide
 *
 * 75    7/21/99 10:15a Davide
 *
 * 74    7/20/99 12:34 Davew
 * Added a screen shot handler
 *
 * 73    7/09/99 10:53a Russellk
 * Bug fix - showall2 should only be called from tickactions
 *
 * 72    7/08/99 4:58p Philla
 *
 * 71    7/06/99 2:58p Philla
 * Player Select.  ifdefs so old way still works.
 *
 * 70    6/22/99 3:13p Timp
 * Improved F11/Ctrl-F11 cheat code.
 *
 * 69    6/22/99 12:49p Russellk
 * Removed timer delays
 *
 * 68    6/17/99 11:06a Timp
 * Separated DisplayErrorNotification into DisplayErrorNotification and
 * FormatErrorNotification
 *
 * 67    6/11/99 11:24a Russellk
 * 11th token mods
 *
 * 66    6/10/99 10:54a Timp
 * Changed chat activation key from 5 to tab.  Added F11 debugging key.
 *
 * 65    6/07/99 3:26p Timp
 * Moved dat_lm01.dat to right place.
 *
 * 64    6/03/99 5:05p Russellk
 *
 * 63    6/02/99 1:33p Davide
 * added code to help with switching screens with F8
 *
 * 62    6/02/99 8:51a Davide
 * added a case for NOTIFY_ACCEPT_CONFIG
 *
 * 61    6/01/99 2:28p Davide
 *
 * 60    5/31/99 12:24p Russellk
 *
 * 59    5/31/99 3:48p Timp
 * Added calls to UDChat.cpp functions to initialize and run chat window.
 *
 * 58    5/31/99 10:01a Davide
 * F12 now displays the custom board editor
 *
 * 57    5/31/99 9:13a Russellk
 * Offline trade enabled - F 3 to send trade.
 *
 * 56    5/18/99 11:04a Russellk
 *
 * 55    5/11/99 9:43a Russellk
 *
 * 54    5/07/99 8:45a Russellk
 * E3 demo revisions - first acceptable version.
 *
 * 53    5/05/99 3:46p Philla
 * Save Game stuff.
 *
 * 52    5/05/99 9:00a Russellk
 * Hotkey changes
 *
 * 51    5/04/99 9:02a Philla
 * Working on email send.  Signing in for Russell.
 *
 * 50    5/03/99 10:59a Philla
 * Save prompt override working properly.
 *
 * 49    5/03/99 10:19a Russellk
 *
 * 48    5/03/99 10:04a Philla
 * Save Game file prompt override (for email).
 *
 * 47    4/30/99 3:26p Russellk
 * f 5 key changed to f 3
 *
 * 46    4/29/99 1:09p Russellk
 * Sequencer & engine lock for token sequences begun - its hook for
 * pennybags sound is in.
 *
 * 45    4/28/99 5:00p Russellk
 * E3 demo stuff looking better, added mucho sound.
 *
 * 44    27/04/99 19:31 Timp
 * Send Unicode strings to MESS_StartNetworking
 *
 * 43    4/27/99 5:05p Russellk
 * Demo stuff continued, auction & tax button bar fixes
 *
 * 42    4/27/99 2:56p Russellk
 *
 * 41    4/27/99 12:16p Russellk
 * first E3 demo changes
 *
 * 40    4/26/99 4:24p Russellk
 * Some stuff in for E3 demo mode
 *
 * 39    4/26/99 12:03p Philla
 * Windoze cursor during windoze save dialog.
 *
 * 38    4/22/99 2:17p Kens
 * added F 5 command to turn 3D board on and off: KS
 *
 * 37    4/22/99 9:35a Russellk
 * Added board2 datfile - too many tabs to put the 2d board items into one
 * datafile.
 *
 * 36    4/19/99 6:54p Kens
 * initialize datafile DAT_3D.DAT
 *
 * 35    4/19/99 10:37a Russellk
 * Westwood screenshot associated with function key 1
 *
 * 34    4/16/99 13:01 Davew
 * Fixed the intro movies so they play at better quality
 *
 * 33    4/16/99 10:08 Davew
 * Needed to add in a handler for NOTIFY_NEXT_MOVE.  It was added to make
 * sure that the player selection screen(s) go away after a game is
 * loaded, because no NOTIFY_GAME_STARTING message is sent when a game is
 * loaded from a saved file.
 *
 * 32    4/15/99 3:12p Russellk
 * 2D Board views added to main screen, camera button rotates through em.
 * No frills yet.
 *
 * 31    4/13/99 15:27 Davew
 * Added flag to disable the opening movies
 *
 * 30    4/13/99 1:55p Russellk
 * Modifed to use setbackdrop & other demo defines
 *
 * 29    4/09/99 18:46 Davew
 * Commented out the stuff to blank the screen
 *
 * 28    4/09/99 17:23 Davew
 *
 * 27    4/09/99 14:41 Davew
 * Added the demo trigger call
 *
 * 26    4/09/99 10:43 Davew
 * Added some code to play the opening movies and load the preset game
 *
 * 25    4/08/99 11:02a Russellk
 * ifdef to allow loading demo.msv without prompting and f 1 to load
 * demo.msv.
 *
 * 24    4/06/99 11:57 Davew
 * Added an F2 hot key that will automatically add four players to the
 * game.
 *
 * 23    3/30/99 3:17p Russellk
 * First trade working - dont get to pick who trading with, only
 * properties work.
 *
 * 22    3/30/99 9:57a Russellk
 *
 * 21    3/26/99 14:48 Davew
 * Removed stuff that wasn't supposed to be there
 *
 * 20    3/26/99 9:50 Davew
 * Fixed a potential future bug
 *
 * 19    3/26/99 9:30 Davew
 * Added the message router to the ProcessLibraryMessage() call
 *
 * 18    3/25/99 9:34a Russellk
 *
 * 17    3/23/99 3:30p Russellk
 * Library update.  IBAR updates - graphically weak but it has all states
 * and works.
 *
 * 16    3/18/99 4:01p Russellk
 * Trade viewer property sets in (yuck)
 *
 * 15    3/17/99 8:48a Russellk
 * Trade viewer partially implemented
 *
 * 14    3/12/99 4:10p Russellk
 * Trade screen started, HIDE functions removed - setting the background
 * to black does the whole job anyway.
 *
 * 13    3/10/99 6:10p Russellk
 * Auction screen added - refinements otherwise.
 *
 * 12    3/05/99 2:02p Russellk
 * Auction, trade & Pselect screens set up - new datafile for them.
 *
 * 11    3/02/99 2:06p Russellk
 *
 * 10    2/26/99 12:15p Russellk
 * IBar enhanced, property pop-up enhanced - routine check-in.
 *
 * 9     2/24/99 8:35a Russellk
 * February deliverable candidate - adds pop-up property deeds and board
 * data on the main screen
 *
 * 8     2/19/99 4:14p Russellk
 * IBAR current player support for starter functions.  Colors go with
 * player, property bar running, mouseovers for everything.
 *
 * 7     2/17/99 8:45a Russellk
 *
 * 6     2/11/99 2:08p Russellk
 * New IBAR look & features begun.
 *
 * 5     2/08/99 2:08p Russellk
 * Added DONE TURN support.
 *
 * 4     2/05/99 8:54a Russellk
 * Bug free
 *
 * 3     2/03/99 2:52p Russellk
 * Display/UserIfce broken into UD modules.
 *
 * 2     1/27/99 10:59a Russellk
 * Basic Mono2 display and user interface operational - no trade editor
 * nor game config editory.
 *
 * 1     1/08/99 3:52p Russellk
 * Non-Hotseat files
 *
 * 5     1/04/99 11:40a Russellk
 * Graphics priority problem fixed (drew houses/hotels after pop-up
 * property)
 *
 * 4     12/29/98 11:37a Russellk
 * NOTIFY_JAIL_EXIT_CHOICE - changed to only enable valid buttons.
 *
 * 3     12/17/98 4:27p Russellk
 * Obsolete comments removed for new version.  Log file facility entered
 * in Mess.c to record phase and action/error message activity.
 *
 */

#include "gameinc.h"
#include "..\PC3D\PC3DHdr.h"

// PROGRAM NOTE:  ProcessMessageToPlayer is the key response routine.

long AuctionHighestBid;
  /* This variable keeps track of the current highest bid of an auction
  in progress. */

BOOL GamePaused;

BOOL GameQuitRequested;

#if USE_OPENING_MOVIES
int                     g_nMovieIdx       = -1;
static LE_DATA_DataId   g_idVideo         = LE_DATA_EmptyItem;
static int              g_nVideoPriority  = 1100;
#define NUMMOVIES       3

char *g_aszMovies[NUMMOVIES] =
{ "HLogo.avi", "ALogo.avi", "MIntro.avi" };

BOOL PlayMovie (char *szFilename);
void PlayPennyBags();
void StopIntro_LoadGame();
#endif // USE_OPENING_MOVIES

/*****************************************************************************
* This is called from Main.cpp as Main.cpp changes unexpectedly with library upgrades.
*/
BOOL MainExtendedInitialization( void )
{
  LE_DATA_InitDatafile( "Dat_Mon\\dat_main.dat", DAT_MAIN );
  LE_DATA_InitDatafile( "Dat_Mon\\dat_pat.dat" , DAT_PAT );
//  LE_DATA_InitDatafile( "Dat_Mon\\dat_cge.dat" , DAT_CGE );
#if USA_VERSION
  LE_DATA_InitDatafile( "Dat_Mon\\dat_bord.dat", DAT_BOARD );
#else
  LE_DATA_InitDatafile( "Dat_Mon\\dat_borde.dat", DAT_BOARD );
#endif
  LE_DATA_InitDatafile( "Dat_Mon\\dat_brd2.dat", DAT_BOARD2 );
  LE_DATA_InitDatafile( "Dat_Mon\\dat_3d.dat",   DAT_3D );

#if MESS_REPORT_ACTION_MESSAGES
  FILE *handle;

  /* Initialise the message log file(s) */
  handle = fopen( MESS_REPORT_ACTION_FILENAME, "wt" ); // Clear it.
  if( handle != NULL )
    fclose( handle );
#endif

  // Initialize the text message retrieval system.
  if (!LANG_InitializeSystem ())
    return FALSE;

  // Get the font
  if( !LE_FONTS_SetFont("Arial.ttf", "Arial") )
    LE_FONTS_SetFont(ANSI_FIXED_FONT_FILE, NULL);//FIXME Tim - substitute our alternate when ready.
  LE_FONTS_SetSize(10);

  // Initialize the chat system.
  if (!CHAT_InitializeSystem ())
    return FALSE;

    /* Initialise the messaging system, but do language first since it
  may want to display error messages. */
  if (!MESS_InitializeSystem ())
    return FALSE;

  // Initialize our main timer.  This handles calling AdvanceTimeStep which pops
  // messages from the game queue and calling AI routines roughly once per second.
  LE_TimerSpeeds [MAIN_GAME_TIMER] = 2; // This one has units of 60hz.
  LE_TimerRestartCount [MAIN_GAME_TIMER] = 1;
  LE_TimerSendUIMessage[MAIN_GAME_TIMER] = TRUE;
  LE_Timers[MAIN_GAME_TIMER] = 1;

  GamePaused = FALSE;
  GameQuitRequested = FALSE;

  if (!DISPLAY_initialize())
    return(FALSE);

  /* Get the rules engine going. */
  if (!RULE_InitializeSystem ())
    return FALSE;

  // Check for command line arguments, like ones which start up
  // networking automatically.
  ShowCursor( TRUE );
  MESS_StartNetworking (LE_MAIN_CommandLine);
  ShowCursor( FALSE );


#if USE_OPENING_MOVIES
  // For MicroSloth Zone lobby games skip opening movies or the player
  // won't be able to join in time.
  if ( !MESS_GameStartedByLobby )
  {
    // Actually start the opening movie glamour.
    UDBOARD_SetBackdrop( DISPLAY_SCREEN_Black );
    g_nMovieIdx = 0; // Will fire up the movie in the event loop.
    LE_SEQNCR_AddMouseSubSequenceTheWorks(LED_EI, 100, 0, 0, TRUE);
    LE_SEQNCR_ProcessUserCommands (); // So the mouse pointer stop takes effect.
  }
  else
  {
    UDBOARD_SetBackdrop( DISPLAY_SCREEN_Pselect );
    UDPSEL_SwitchPhase( UDPSEL_PHASE_HISCORE );
  }
#else // not USE_OPENING_MOVIES

  UDBOARD_SetBackdrop( DISPLAY_SCREEN_Pselect );
  UDPSEL_SwitchPhase( UDPSEL_PHASE_HISCORE );

#endif // not USE_OPENING_MOVIES
  return TRUE;
}

/*****************************************************************************
* Processes Artlib99 event messages - timers, keyboard, mouse...
*/
void ProcessLibraryMessage( LE_QUEUE_MessagePointer UIMessagePntr )
{
  static int preBlackBackdrop = DISPLAY_SCREEN_MainA;
  static LE_DATA_DataId trademarkScreen = LED_EI;
  static TYPE_Tick screenStartTime;

#if USE_OPENING_MOVIES
  if (-1 != g_nMovieIdx)
  {
    // Hack in the trademark screen ahead of movies
    if( g_nMovieIdx == 0 )
    {// Fire up/wait for trademark screen
      if( trademarkScreen == LED_EI )
      {
        trademarkScreen = LED_IFT( DAT_LANG2, BMP_trademark );
        LE_SEQNCR_Start( trademarkScreen, 0 );
        screenStartTime = LE_TIME_TickCount;
      }
      if( screenStartTime + 60 * 9 < LE_TIME_TickCount )
        g_nMovieIdx++;
    } else
    {
      if( trademarkScreen )
      {
        LE_SEQNCR_Stop( trademarkScreen, 0 );
        trademarkScreen = LE_GRAFIX_ObjectCreate( 800, 600, LE_GRAFIX_ObjectNotTransparent);
        LE_SEQNCR_Start( trademarkScreen, 0 );
        LE_SEQNCR_ProcessUserCommands(); // Clear the screen
        LE_SEQNCR_Stop( trademarkScreen, 0 );
        LE_SEQNCR_ProcessUserCommands(); // Remove the black screen
        LE_DATA_FreeRuntimeDataID( trademarkScreen );
        trademarkScreen = LED_EI;
      }

      // Wait until the video is finished and then free it and start next.
      if (g_idVideo == LE_DATA_EmptyItem ||
        LE_SEQNCR_IsSequenceFinished(g_idVideo, g_nVideoPriority, FALSE))
      {
        if (g_idVideo != LE_DATA_EmptyItem)
        {
          LE_DATA_FreeRuntimeDataID(g_idVideo);
          g_idVideo = LE_DATA_EmptyItem;
        }
        // Now play the next movie if there is one
        // Note that g_idVideo gets set to the sequence used to play
        // the video, but it can be NULL for Bink style videos, which
        // get played inside the PlayMovie function (returns when video
        // finished or key hit).

        if (g_nMovieIdx >= NUMMOVIES + 1 || !PlayMovie(g_aszMovies[g_nMovieIdx++ - 1]))
        {
          // If there are no more movies to play, or something went
          // wrong (user aborted a Bink movie), proceed with the game.
          StopIntro_LoadGame();
        }
      }
    }

    // Bailout - check for keystrokes or mousestrokes to cancel videos.
    //if ((UIMSG_KEYBOARD_PRESSED  == UIMessagePntr->messageCode  &&
    //     LE_KEYBRD_SPACE         == UIMessagePntr->numberA)     ||
    //     UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN)
    if (UIMSG_KEYBOARD_PRESSED  == UIMessagePntr->messageCode  ||
        UIMSG_MOUSE_LEFT_DOWN   == UIMessagePntr->messageCode  ||
        UIMSG_MOUSE_MIDDLE_DOWN == UIMessagePntr->messageCode  ||
        UIMSG_MOUSE_RIGHT_DOWN  == UIMessagePntr->messageCode )
    {
      if( g_nMovieIdx == 0 )
      {
        g_nMovieIdx++;
      }
      else
      {
        if (g_idVideo != LE_DATA_EmptyItem)
        {
          LE_SEQNCR_Stop(g_idVideo, g_nVideoPriority);
          LE_DATA_FreeRuntimeDataID (g_idVideo);
          g_idVideo = LE_DATA_EmptyItem;
        }
        StopIntro_LoadGame();
      }
    }

    return;
  }
#endif // USE_OPENING_MOVIES


  // Handle game message pops and AI 'once a second' updates.
//  if (UIMessagePntr->messageCode == UIMSG_TIMER_REACHED_ZERO)
//    if (UIMessagePntr->numberA == MAIN_GAME_TIMER )
      AdvanceTimeStep(); // No harm in calling this often.


      // Quickie custom debug stuff
  if (UIMessagePntr->messageCode == UIMSG_KEYBOARD_RELEASED)
  {
    /*if( UIMessagePntr->numberA == LE_KEYBRD_F5 )
    {
      static musicOnToggle = TRUE;

      musicOnToggle = !musicOnToggle;
      if( musicOnToggle )
        DSOUND_BackgroundMusicOn();
      else
        DSOUND_BackgroundMusicOff();
    }*/

    // NOTE: This is not a cheat key but a game key.  Is tab the best?
    if ((UIMessagePntr->numberA == LE_KEYBRD_TAB) && (MESS_NetworkMode))
      CHAT_Toggle();

#if USE_HOTKEYS2

/*  if( UIMessagePntr->numberA == LE_KEYBRD_F4 )
    {// 3D board switch
      static BOOL lastHadLargeTextures = FALSE; // We will need to toggle this also.

      DISPLAY_state.board3Don = FALSE;
      LE_SEQNCR_Stop( CurrentBoard, DISPLAY_Board3dPriority ); // FIXME - anytime this is done for real, put it inside the main display_show collect commands.

      switch( LE_DATA_TagFromId( CurrentBoard ) )
      {
      case HMD_board_very_low:
        UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_boardmed ), FALSE );
        break;

      case HMD_boardmed:
        if( lastHadLargeTextures )
        {
          UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_boardhigh ), FALSE );
          lastHadLargeTextures = FALSE;
        }
        else
        {
          UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_boardmed ), TRUE );
          lastHadLargeTextures = TRUE;
        }
        break;

      case HMD_boardhigh:
        if( lastHadLargeTextures )
        {
          UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_board_citymed ), FALSE );
          lastHadLargeTextures = FALSE;
        }
        else
        {
          UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_boardhigh ), TRUE );
          lastHadLargeTextures = TRUE;
        }
        break;

      case HMD_board_citymed:
        if( lastHadLargeTextures )
        {
          UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_board_cityhigh ), FALSE );
          lastHadLargeTextures = FALSE;
        }
        else
        {
          UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_board_citymed ), TRUE );
          lastHadLargeTextures = TRUE;
        }
        break;

      case HMD_board_cityhigh:
        if( lastHadLargeTextures )
        {
          UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_board_very_low ), FALSE );
          lastHadLargeTextures = FALSE;
        }
        else
        {
          UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_board_cityhigh ), TRUE );
          lastHadLargeTextures = TRUE;
        }
        break;
      }

    }*/

    if( UIMessagePntr->numberA == LE_KEYBRD_F9 )
    {// 3D mode toggle
      //DISPLAY_state.game3Don = !DISPLAY_state.game3Don;
      //DISPLAY_state.board3Don = FALSE;
      /*LE_SEQNCR_Stop( CurrentBoard, DISPLAY_Board3dPriority ); // FIXME - anytime this is done for real, put it inside the main display_show collect commands.

      if( DISPLAY_state.game3Don )
        UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_boardmed ), FALSE );
      else
        UDBOARD_SwitchToBoard( LED_IFT( DAT_3D, HMD_board_very_low ), FALSE );*/
    }

    if( UIMessagePntr->numberA == LE_KEYBRD_F2 )
    {
      // Add a default set of players and start the game
      wchar_t szName[RULE_MAX_PLAYER_NAME_LENGTH];

      // Cannon (AI)
      ZeroMemory(szName, sizeof(wchar_t) * RULE_MAX_PLAYER_NAME_LENGTH);
      mbstowcs(szName, "Cannon", 6);
      AddLocalPlayer(szName, TK_GUN, PC_RED, 2, FALSE);

      // Car (AI)
      ZeroMemory(szName, sizeof(wchar_t) * RULE_MAX_PLAYER_NAME_LENGTH);
      mbstowcs(szName, "Car", 3);
      AddLocalPlayer(szName, TK_CAR, PC_BLUE, 2, FALSE);

      // Dog (human player)
      ZeroMemory(szName, sizeof(wchar_t) * RULE_MAX_PLAYER_NAME_LENGTH);
      mbstowcs(szName, "Dog", 3);
      AddLocalPlayer(szName, TK_DOG, PC_GREEN, 0, FALSE);

      // Top Hat (human player)
      ZeroMemory(szName, sizeof(wchar_t) * RULE_MAX_PLAYER_NAME_LENGTH);
      mbstowcs(szName, "Top hat", 7);
      AddLocalPlayer(szName, TK_HAT, PC_YELLOW, 0, FALSE);

      // Switch to start screen.  Here more players can be added or the game can begin.
      UDBOARD_SetBackdrop( DISPLAY_SCREEN_Pselect );
      UDPSEL_SwitchPhase( UDPSEL_PHASE_STARTADDREMOVE );
    }

    if( UIMessagePntr->numberA == LE_KEYBRD_F8 )
    {
      DISPLAY_state.desired2DView = ( DISPLAY_state.desired2DView + 1 ) % DISPLAY_SCREEN_MAX;
      if( !DISPLAY_state.desired2DView ) DISPLAY_state.desired2DView++;
      DISPLAY_showAll;
    }

    // debugging stuff...  F11 assigns properties to Players 0 and 1.
    // Ctrl-F11 gives player 0 all the stuff,
    // to see what it looks like on the trade screen.
    if (UIMessagePntr->numberA == LE_KEYBRD_F11) {
      if (GetKeyState(VK_CONTROL) & 0x80000000) {
        for (int h = 0; h < SQ_MAX_SQUARE_TYPES; h++) {
          MESS_SendAction (ACTION_CHEAT_OWNER, 1,
            RULE_BANK_PLAYER, RULE_NOBODY_PLAYER, h, 0, 0,NULL, 0, NULL);
          MESS_SendAction (ACTION_CHEAT_OWNER, 1,
            RULE_BANK_PLAYER, 0, h, 0, 0,NULL, 0, NULL);
        }
        UICurrentGameState.Cards[CHANCE_DECK].jailOwner = 0;
        UICurrentGameState.Cards[COMMUNITY_DECK].jailOwner = 0;
      } else {
        for (int h = 0; h < SQ_MAX_SQUARE_TYPES; h++) {
          MESS_SendAction (ACTION_CHEAT_OWNER, 1,
            RULE_BANK_PLAYER, RULE_NOBODY_PLAYER, h, 0, 0,NULL, 0, NULL);
          MESS_SendAction (ACTION_CHEAT_OWNER, 1,
            RULE_BANK_PLAYER, (h < 10), h, 0, 0,NULL, 0, NULL);
        }
        UICurrentGameState.Cards[CHANCE_DECK].jailOwner = 0;
        UICurrentGameState.Cards[COMMUNITY_DECK].jailOwner = 1;
      }
    }
#endif // Hotkeys2


#if USE_HOTKEYS
  static int tokenIndex = 0, seqindex = 0;
  int showseq = -1, square = 2;
  int anims = ANIMS_PER_TOKEN;
  int tokensin = 11;
  // Temporary - try to run an HMD
  // Prev/Next token
  if( LE_KEYBRD_CheckLatched(LE_KEYBRD_1) )
  {
    LE_KEYBRD_ClearLatchedKey(LE_KEYBRD_1);
    tokenIndex = (tokenIndex - 1 + tokensin) % tokensin; // # tokens
    //seqindex = 0;
    showseq = seqindex;
  }
  if( LE_KEYBRD_CheckLatched(LE_KEYBRD_2) )
  {
    LE_KEYBRD_ClearLatchedKey(LE_KEYBRD_2);
    tokenIndex = (tokenIndex + 1) % tokensin;
    //seqindex = 0; showseq = 0;
    showseq = seqindex;
  }

  // Prev/Next sequence
  float fXCoordinate;
  float fYCoordinate;
  float fZCoordinate;
  float fYAngle;
  if( LE_KEYBRD_CheckLatched(LE_KEYBRD_9) )
  {
    LE_KEYBRD_ClearLatchedKey(LE_KEYBRD_9);
    seqindex = (seqindex - 1 + anims) % anims;
    showseq = seqindex;
  }
  if( LE_KEYBRD_CheckLatched(LE_KEYBRD_0) )
  {
    LE_KEYBRD_ClearLatchedKey(LE_KEYBRD_0);
    seqindex = (seqindex + 1) % anims;
    showseq = seqindex;
  }
  if( showseq != -1 )
  {// Run the seq
    // Adjust corner sequences to start someplace 'good'
    if( seqindex == 0 ) square = 9;
    if( seqindex == 1 || seqindex == 2 ) square = 9;
    if( seqindex == 3 || seqindex == 4 ) square = 8;
    UDPIECES_GetTokenOrientation( square,
                                  &fXCoordinate,&fYCoordinate,&fZCoordinate,&fYAngle);
    if( seqindex == 0 ) fZCoordinate += 39;// Corner 0

    LE_SEQNCR_StartRySTxz(LED_IFT( DAT_3D, showseq + tokenIndex * anims + CNK_knacx0 ) , 1,
      fYAngle, 1.0f, fXCoordinate,fZCoordinate);
    //LE_SEQNCR_Start( LED_IFT( DAT_3D, showseq + CNK_knbcx0 ), 1 );
    LE_SEQNCR_SetEndingAction( LED_IFT( DAT_3D, showseq + tokenIndex * anims + CNK_knacx0 ), 1,
      LE_SEQNCR_EndingActionStop );
  }


  if( UIMessagePntr->numberA == LE_KEYBRD_S )
    {
      RECT rct = { 0, 0, 800, 450 };
      Surface* pRender = pc3D::GetRenderSurface();
      pRender->SaveToBmpFile("TheBoard.bmp", &rct);
    }

    if( UIMessagePntr->numberA == LE_KEYBRD_F1 )
    // Debug dialog
      DISPLAY_state.MessageBoxSuppressed = !DISPLAY_state.MessageBoxSuppressed;


    if( UIMessagePntr->numberA == LE_KEYBRD_X )
    {// temp - call token anim stack with failing runs to test logic.
      UDPIECES_PlanMoveAnim(0,NOTIFY_MOVE_FORWARDS,23,33);
    }

    if( UIMessagePntr->numberA == LE_KEYBRD_F6 )
    {// 2d board on/off
      if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_Black )
        UDBOARD_SetBackdrop( preBlackBackdrop );
      else
      {
        preBlackBackdrop = DISPLAY_state.desired2DView;
        UDBOARD_SetBackdrop( DISPLAY_SCREEN_Black );
      }
    }

    if( UIMessagePntr->numberA == LE_KEYBRD_F7 )
    {// New game
      // e3 hack - set the screen
      // g_eActiveScreen = g_ePreviousScreen = ST_PLAYERSUMMARY;
      // UDBOARD_SetBackdrop( DISPLAY_SCREEN_PselectA );
      // Not removing above might make odd stuff later in PSELECT.
      MESS_SendAction (ACTION_NEW_GAME, AnyLocalPlayer (), RULE_BANK_PLAYER,
        FALSE, 0, 0, 0, NULL, 0, NULL );
    }

#endif

  }

  if( //(UIMessagePntr->messageCode == UIMSG_MOUSE_MOVED) ||
    //(UIMessagePntr->messageCode == UIMSG_KEYBOARD_RELEASED) ||
    //(UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN) ||
    TRUE ) // FIXME - whats optimal?  Reduce when ready.
  {
    // This first bit is 'secondary' processing - for cheapo conversion interface.
    /*if( UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN )
    {
      mouseClickedButton = -1;
      if(UIMessagePntr->numberA >= 74 &&
        UIMessagePntr->numberA <= 323 &&
        UIMessagePntr->numberB >=  74 &&
        UIMessagePntr->numberB <= 323 )
      {
        mouseClickedButton = 0; // at least the main window hit.
        if( UIMessagePntr->numberB >=  254 ) // got a real button
        {
          if( UIMessagePntr->numberA < 158 )
            mouseClickedButton = 1;
          else if ( UIMessagePntr->numberA < 240 )
            mouseClickedButton = 2;
          else
            mouseClickedButton = 3;
        }
      }
      if( mouseClickedButton == -1 )
      {// check for BSSM button
        if( UIMessagePntr->numberA >= 320 && UIMessagePntr->numberA <= 393 &&
          UIMessagePntr->numberB >= 400 && UIMessagePntr->numberB <= 439 )
          mouseClickedButton = 4;
      }

      // Temporary dialog handler.
      //UserClickedButton( mouseClickedButton );
      DISPLAY_state.tempConversionMouseClickedButton = mouseClickedButton;
    }*/


    // Dispatch the message to the appropriate UD module
    // NOTE:  The message can go to more than one module if more than one
    // has active controls.

    if (UDCHAT_ProcessMessage(UIMessagePntr) == FALSE)
    {
//      UDCGE_ProcessMessage   ( UIMessagePntr );
      UDAUCT_ProcessMessage  ( UIMessagePntr );
      UDBOARD_ProcessMessage ( UIMessagePntr );
      UDIBAR_ProcessMessage  ( UIMessagePntr );
      UDOPTS_ProcessMessage  ( UIMessagePntr );
      UDPIECES_ProcessMessage( UIMessagePntr );
      UDPSEL_ProcessMessage  ( UIMessagePntr );
      UDSTATS_ProcessMessage ( UIMessagePntr );
      UDTRADE_ProcessMessage ( UIMessagePntr );
    }
  }
}


#if USE_OPENING_MOVIES
/*****************************************************************************
 * Start the movie playing.  Try to use the Bink video if the computer is
 * fast enough (2D board mode off), otherwise use the Indeo video.  Also
 * fall back to Indeo if the Bink file can't be found.  Returns TRUE
 * if the movie was found and successfully started, returns FALSE if
 * an error happened or the user pressed a key (in Bink mode) to abort
 * the movie.
 */

BOOL PlayMovie (char *szFilename)
{
  char  FileNameWithExtension [MAX_PATH];
  BOOL  FoundFile = FALSE;
  char  PathName [MAX_PATH];
  RECT  rctMovie = { 0, 0, 800, 600 };
  char *StringPntr;
  BOOL  UseLibraryPlayer = FALSE;

  if (g_idVideo != LE_DATA_EmptyItem)
  {
    LE_DATA_FreeRuntimeDataID (g_idVideo);
    g_idVideo = LE_DATA_EmptyItem;
  }

  // Search for the Bink video file first.  Except on slow systems,
  // where Bink can't work (sound breaks up etc).

  strcpy (FileNameWithExtension, szFilename);
  StringPntr = strrchr (FileNameWithExtension, '.');
  if (StringPntr != NULL)
  {
    strcpy (StringPntr, ".bik");

    if (DISPLAY_RenderSettings.bUse3DBoard /* If fast computer */ &&
    LE_DIRINI_ExpandFileName (FileNameWithExtension, PathName, NULL))
      FoundFile = TRUE;
    else // Default to AVI file format.
    {
      UseLibraryPlayer = TRUE;
      strcpy (StringPntr, ".avi");
      if (LE_DIRINI_ExpandFileName (FileNameWithExtension, PathName, NULL))
        FoundFile = TRUE;
    }
  }

  if (!FoundFile)
  {
    // Just use the file extension the user gave us.
    UseLibraryPlayer = TRUE;
    if (!LE_DIRINI_ExpandFileName (szFilename, PathName, NULL))
      return FALSE; // Couldn't find the video file.
  }

  if (!UseLibraryPlayer)
  {
    return LE_SEQNCR_TakeOverAndPlayBinkVideo (PathName, TRUE /*Not Double size*/ );
  }
  else // AVI style file, play with the library player.
  {
    // Indeo doesn't play double size in 32 bit depth for some buggy reason,
    // so only do single size in that mode.  Also single size on slow systems.

    if (LE_GRAFIX_ScreenBitsPerPixel >= 32
    /* || !DISPLAY_RenderSettings.bUse3DBoard : allow double size even on slow systems, works well enough */)
    {
      rctMovie.left = 200;
      rctMovie.right = 600;
      rctMovie.top = 150;
      rctMovie.bottom = 450;
    }

    // Open the movie file
    g_idVideo = LE_SEQNCR_CreateVideoObject (
      PathName,
      TRUE /* DrawSolid */,
      ALPHA_OPAQUE100_0,
      &rctMovie,
      FALSE, TRUE, TRUE,
      TRUE /* DrawDirectlyToScreen */,
      FALSE /* DoubleAlternateLines */);

    // If we failed to load the video, there's no point in continuing
    if (LE_DATA_EmptyItem == g_idVideo)
      return FALSE;

    // Fill the screen with black, but without using the sequencer since
    // we don't want any sequencer things active while drawing directly
    // to the screen in the video player.

    // Alex or Dave?  Doesn't work this way (RK)
    //LE_GRAFIX_ColorArea (LE_DATA_EmptyItem, 0, 0,
    //  LE_GRAFIX_RealScreenWidthInPixels, LE_GRAFIX_RealScreenHeightInPixels,
    //  0 /* color */);

    LE_SEQNCR_CollectCommands();

    // Start the video
    LE_SEQNCR_Start(g_idVideo, g_nVideoPriority);

    // Set the ending action of the video
    LE_SEQNCR_SetEndingAction(g_idVideo, g_nVideoPriority, LE_SEQNCR_EndingActionStop);

    LE_SEQNCR_ExecuteCommands();
  }
  return TRUE;
}


void StopIntro_LoadGame()
{
  if (g_nMovieIdx != -1)
  {
    g_nMovieIdx = -1;
    //LoadGameOrOptions(TRUE); // Not real graceful, but only do this auto load in a demo mode.
    //g_eActiveScreen             = ST_GAME;
    DISPLAY_state.desired2DView = DISPLAY_SCREEN_Pselect;
    UDPSEL_SwitchPhase( UDPSEL_PHASE_HISCORE );

    // Run the penny bags audio
    //PlayPennyBags();

    // Turn on the mouse pointer again.
    LE_SEQNCR_AddMouseSubSequenceTheWorks(LED_IFT(DAT_MAIN, TAB_pointer),
                                          100, 0, 0, TRUE);
  }
}
#endif // USE_OPENING_MOVIES



#if CE_ARTLIB_SoundEnableVoiceChat
/*****************************************************************************
 * Callback function for sending a voice chat buffer.  Returns TRUE if
 * the message was successfully sent.  Doesn't send it if the output
 * queue is full and the message isn't important.
 */

BOOL SendVoiceChatCallback (void *Buffer, UNS16 BufferSize, BOOL ImportantDataDoNotDiscard)
{
  RULE_CharHandle BufferHandle;
  BYTE           *BufferPntr;
  RULE_ActionArgumentsRecord MessageToSend;

  if (!ImportantDataDoNotDiscard && MESS_CurrentQueueSize () > 50)
    return FALSE; // Queue is slightly full and this is unimportant.

  BufferHandle = RULE_AllocateHandle (BufferSize);
  if (BufferHandle == NULL)
    return FALSE; // Out of memory.
  BufferPntr = (LPBYTE) RULE_LockHandle (BufferHandle);
  if (BufferPntr == NULL)
  {
    RULE_FreeHandle (BufferHandle);
    return FALSE; // Out of real memory.
  }

  memcpy (BufferPntr, Buffer, BufferSize);
  RULE_UnlockHandle (BufferHandle);

  /* Ok, send the blob to the rules engine for
     redistribution and later deallocation. */

  memset (&MessageToSend, 0, sizeof (MessageToSend));
  MessageToSend.action = ACTION_VOICE_CHAT;
  MessageToSend.fromPlayer = RULE_SPECTATOR_PLAYER;
  MessageToSend.toPlayer = RULE_BANK_PLAYER;
  MessageToSend.numberA = RULE_ALL_PLAYERS; // Chat to player number.
  MessageToSend.binaryDataHandleA = BufferHandle;
  MessageToSend.binaryDataSizeA = BufferSize;

  return MESS_SendActionMessage (&MessageToSend);
}



/*****************************************************************************
 * Compress recorded voice chat data and transmit it.  Does nothing if
 * voice chat is disabled, or very little if the user is silent.
 */

static void PeriodicUpdateSendVoiceChat (void)
{
  LE_SOUND_ChatSend (0.0F, 0.0F, 0.0F);
}



/*****************************************************************************
 * Deompress and play voice chat data.
 */

static void ReceiveVoiceChat (RULE_ActionArgumentsPointer NewMessagePntr)
{
  if (NewMessagePntr->binaryDataHandleA != NULL)
  {
    if (NewMessagePntr->binaryDataA == NULL)
    {
      NewMessagePntr->binaryDataA =
        (LPBYTE) RULE_LockHandle (NewMessagePntr->binaryDataHandleA);
      if (NewMessagePntr->binaryDataA != NULL)
        NewMessagePntr->binaryDataA += NewMessagePntr->binaryDataStartOffsetA;
    }
  }

  if (NewMessagePntr->binaryDataA != NULL)
  {
    LE_SOUND_ChatReceive (
      NewMessagePntr->binaryDataA,
      (UNS16) NewMessagePntr->binaryDataSizeA,
      NewMessagePntr->numberD /* Identifies the sending computer */);
  }
}



/*****************************************************************************
 * Glue function to start and stop voice chat.  Only starts when
 * this is a network game.
 */

BOOL USERIFCE_StartVoiceChat (void)
{
  if (MESS_NetworkMode)
    return LE_SOUND_ChatOn (SendVoiceChatCallback, 0 /* PositionDimensionality */);

  LE_SOUND_ChatOff ();
  return FALSE;
}
#endif // CE_ARTLIB_SoundEnableVoiceChat



/*****************************************************************************
* User clicked a property.  What did they want?  Find out and generate a message if appropriate.
*/
void UserClickedProperty( int property )
{
}

/*****************************************************************************
* User clicked a dialog button.  Look at the dialog and UI - what did they want?
*/
void UserClickedButton( int mouseClickedButton )
{ // 0-3 are cheap-ass conversion buttons, 4 is BSSM button (cheap ass also...)
  //RULE_ActionArgumentsRecord  MyMessage;

  if( mouseClickedButton >= 0 && mouseClickedButton < 4 )
  {
    switch( DISPLAY_state.CurrentDialog )
    { // All controls moved to subfunctions.
    case 0:
    default:
      break;
    };
  }

};


/*****************************************************************************
* Called periodically to advance the display etc.
*/
static DWORD LastTimeStepTime;
static DWORD LastTickTime;

// These functions allow stopping processing of the rules message queue (for display delay purposes)
void gameQueueLock( void )
{
  DISPLAY_state.LockGameQueueRequested++;
  DISPLAY_state.LockGameQueueLastAction = LE_TIME_TickCount;
};
void gameQueueUnLock( void )
{
  DISPLAY_state.LockGameQueueRequested--;
  if( DISPLAY_state.LockGameQueueRequested < 0 )
    DISPLAY_state.LockGameQueueRequested = 0;// This would be an error, can be caused by lock timeouts (clears this to 0)
  DISPLAY_state.LockGameQueueLastAction = LE_TIME_TickCount;
};

void AdvanceTimeStep (void)
{
  DWORD CurrentTime;
  DWORD DeltaTime;
  RULE_ActionArgumentsRecord NewMessage;

  /* Allow a bit of time to go by before processing the next event.  This
  gives the system some time to run the networking and other programs. */
  CurrentTime = GetTickCount ();
  DeltaTime = CurrentTime - LastTimeStepTime;
  //if (!RunFastWithNoDisplay && DeltaTime < 20 /* milliseconds - 50 in hotseat*/)
  //  return; // REMOVED (RK) - windows timer limits to 19 per second.
  LastTimeStepTime = CurrentTime;

  #if CE_ARTLIB_SoundEnableVoiceChat
  PeriodicUpdateSendVoiceChat ();
  #endif

  // Engine processing can be disabled for short periods - usually to wait for animations (ie token moves, don't want to process what happens until we land.)
  if( DISPLAY_state.LockGameQueueRequested > 0 )
  {
    // Test the time - failsafe is on the game lock
    if( (DISPLAY_state.LockGameQueueLastAction + DISPLAY_GAME_QUEUE_MAX_LOCK_SECONDS * 60 < LE_TIME_TickCount) && !g_UserChoseToExit )
    {// Timeout - force a free lock.
      DISPLAY_state.LockGameQueueRequested = 0;
      // Clear posted sounds (they would issue an extra unlock later)
      DISPLAY_state.SoundToPlayPostLock = LED_EI;
      //DISPLAY_state.SoundLockGameQueueLockActive = FALSE;// More to it than this
      gameQueueUnLock();
    }
    else
    {
      // Check for voice chat messages in the queue and just process
      // those, since delaying them causes the chat to break up,
      // and they are independent of the rest of the game.

      if (MESS_ReceiveVoiceChatMessageOnly (&NewMessage))
      {
        if ((NewMessage.toPlayer == RULE_ALL_PLAYERS ||
        NewMessage.toPlayer == RULE_BANK_PLAYER) && MESS_ServerMode)
          RULE_ProcessRules (&NewMessage);
        
        if (NewMessage.toPlayer == RULE_ALL_PLAYERS ||
        (NewMessage.toPlayer < RULE_MAX_PLAYERS && SlotIsALocalPlayer [NewMessage.toPlayer]))
        {
          ProcessPlayersUI (&NewMessage);
          AI_ProcessMessage (&NewMessage);
        }
        
        MESS_FreeStorageAssociatedWithMessage (&NewMessage);
      }
      return;
    }
  }


  /* Get the next action to process, if any.  Then let the intended
  recipients process it.  Thus simulating a communications network. */
  //NOTE: Processing can be locked by DISPLAY for things like token movement.  After processing
  //the move message processing is locked until the token arrives at destination.  Then
  //we unlock and can process what happens at destination.
  if (MESS_ReceiveActionMessage (&NewMessage))
  {
    /* Pass the message on to the bank if it is for the bank. */

    if ((NewMessage.toPlayer == RULE_ALL_PLAYERS ||
      NewMessage.toPlayer == RULE_BANK_PLAYER) && MESS_ServerMode)
      RULE_ProcessRules (&NewMessage);

      /* Let the human and AI player(s) see it if it is for a player.
      They should ignore messages to other players than themselves,
    since those messages don't get transmitted over the network. */
    if (NewMessage.toPlayer == RULE_ALL_PLAYERS ||
      (NewMessage.toPlayer < RULE_MAX_PLAYERS && SlotIsALocalPlayer [NewMessage.toPlayer]))
    {
      ProcessPlayersUI (&NewMessage);
      AI_ProcessMessage (&NewMessage);
    }

    MESS_FreeStorageAssociatedWithMessage (&NewMessage);
  }
  else /* No messages waiting in the action queue. */
  {
    // See if it is time for the once in a while (once per second)
    // tick action (used for auction timing etc).

    DeltaTime = CurrentTime - LastTickTime;
    if (DeltaTime > TICK_TIME_DELAY /* milliseconds */)
    {
      LastTickTime += TICK_TIME_DELAY;
      DeltaTime = CurrentTime - LastTickTime;
      if (DeltaTime > TICK_TIME_DELAY)
        LastTickTime = CurrentTime; /* Missed more than a full tick, catch up. */
      memset (&NewMessage, 0, sizeof (NewMessage));
      NewMessage.action = ACTION_TICK;
      NewMessage.fromPlayer = RULE_BANK_PLAYER;
      NewMessage.toPlayer = RULE_ALL_PLAYERS;
      if (MESS_ServerMode)
        RULE_ProcessRules (&NewMessage);
      ProcessPlayersUI (&NewMessage);
      AI_ProcessMessage (&NewMessage);
    }
    else /* No messages. */
      ProcessPlayersUI (NULL); /* Flash the cursor. */
  }
}


/********************************************************************
 * NOTE:  Everything under here is from the original Hotseat version.
 * It is/was retained for conversion convenience.  Move up stuff whenever
 * it becomes a permanent, delete it when newer systems replace the old.
 */



#include <String.h>
#include <dsound.h>

#define SAVE_DICE_ROLLS_IN_A_FILE 1
  /* Controls optional compilation of the save dice rolls function. */

#if SAVE_DICE_ROLLS_IN_A_FILE
  static FILE *DiceRollFile;
  static BOOL DiceRollFileOpenHasFailedSoDoNotTryAgain;
  static long TotalDie1;
  static long TotalDie2;
  static long TotalDice;
  static long NumberOfRollsMade;
  static long NumberOfDoublesMade;
#endif

CommonBitmapsRecord CommonBitmaps;
  /* These are loaded when the game starts, so you can use them any time. */

static int SquareNumberClicked = -1;
  /* This number determines what square was clicked on by the user. It is
     used in the Paint routine to show an enlarged picture of the square
     the user selected. This value is calculated in the routine MouseDownInMainBoard(). */


static BOOL CurrentTokenIsOn;
  /* TRUE when the token is in the ON part of its blink cycle, FALSE when the
  token is in the OFF part of the cycle. */

//RULE_CardType CardToDisplay;
  /* ID of the chance or community chest card to display. If this value is not NOT_A_CARD,
  it is assumed it contains a valid card identifier and that card will be displayed in the
  center of the board when the board is updated next. */

int NumberOfLocalPlayers;
  /* Number of players this user interface is running, 0 to MAX_LOCAL_PLAYERS-1. */

wchar_t LocalPlayerNames [MAX_LOCAL_PLAYERS][RULE_MAX_PLAYER_NAME_LENGTH+1];
  /* Names of the local players.  These names are the authorative player
  identification.  If the computer tells us that player named so-and-so
  is playing slot 3, we have to adjust our player to use slot 3 (happens
  when dice are rolled to decide who goes first).  The other data structures
  (SlotIsALocalPlayer, SlotIsALocalHumanPlayer, SlotIsALocalAIPlayer,
  CurrentUIPlayer, LocalPlayerSlots) are all derived from these names. */

RULE_PlayerNumber LocalPlayerSlots [MAX_LOCAL_PLAYERS];
  /* Identifies which slot each of our local players has been assigned.  Set
  to RULE_NOBODY_PLAYER if we don't know. */

BOOL SlotIsALocalPlayer [RULE_MAX_PLAYERS];
  /* Identifies which players the user interface is responsible for (TRUE for
  local, FALSE for network player or AI).  The index is the player number
  (also known as slot number) that the server uses. */

BOOL SlotIsALocalHumanPlayer [RULE_MAX_PLAYERS];
  /* Identifies which local players are humans (or whatever is pounding
  the keyboard). */

BOOL SlotIsALocalAIPlayer [RULE_MAX_PLAYERS];
  /* Identifies which local players are AIs. */

RULE_PlayerNumber CurrentUIPlayer = RULE_NOBODY_PLAYER;
  /* Who the UI is currently simulating.  Should be RULE_NOBODY_PLAYER or
  one of the players marked as TRUE in SlotIsALocalHumanPlayer. */

BOOL GameInProgress;
  /* TRUE if the game is being played, FALSE if at the end of the game
  or signing up players. */

RULE_PlayerSet PlayersAllowedInAuction;
  /* When auctioning stuff, this identifies the players that are allowed to
  start the auction or that are allowed to bid.  Other players are bankrupt
  or don't have a spot to build a house (during house auctions). */

RULE_GameStateRecord UICurrentGameState;
  /* The current state of the game, as the user interface code thinks it is. */

RULE_PlayerNumber RolledPlayer;
  /* Identifies the player who last rolled the dice.  Controls whose name
  is printed out under the dice. */

static const int RULE_PlayerColourToPaletteIndex[MAX_PLAYER_COLOURS] =
{// Note:  I think only the hotseat uses this, but here it is anyway...
  234 /* Red */,
   26 /* Blue */,
   65 /* Green */,
  218 /* Yellow */,
  //196 /* White */
  //245 /* Black */
  217 /* Magenta */,
  //214 /* Cyan */
  //154 /* Pink */
  186 /* Orange */
};
  /* This array maps the colours defined in RULE_PlayerColour enum to palette
  indexes in the 8 bit palette. */

static char * RULE_PlayerColourNames [] = {
  "Red",
  "Blue",
  "Green",
  "Yellow",
  //"White",
  //"Black",
  "Magenta",
  //"Cyan",
  //"Pink",
  "Orange"
};
  /* Matching names for the colours. */

static BOOL ShowColouredOwnershipOnSquares = TRUE;
  /* This value determines if squares owned by players are dithered in that
  players colour. This value is set by a menu item. */

BOOL RunFastWithNoDisplay;
  /* If this is TRUE then there won't be any delays to update the display
  between actions.  That means that the AIs can run at full speed.  The normal
  FALSE mode makes the message system dole out messages once per tick
  (1/18 second). */

enum DialogKindEnum
{
  DK_NO_BOX_OPEN,
  DK_GAME_OPTIONS,
  DK_ROLL_DICE,
  DK_CHEAT_DICE,
  DK_JAIL_EXIT_CHOICE,
  DK_ENTER_NAME,
  DK_PLEASE_PAY,
  DK_SEEN_CARD,
  DK_PROPOSE_TRADE,
  DK_AUCTION,
  DK_BUYORAUCTION,
  DK_HOUSING_SHORTAGE,
  DK_FREE_UNMORTGAGING,
  DK_TAX_DECISION,
  DK_PLACE_BUILDING,
  DK_DECOMPOSE_HOTEL,
  DK_BUY_SELL_MORTGAGE,
  DK_GAME_OVER,
  DK_WAITING_FOR_PLAYERS,
  /* The dialog boxes after this point are ones that are
  triggered by internal actions, not by the rules engine. */
  DK_INTERNAL_ACTION_BOXES,
  DK_SET_OWNER,
  DK_SET_CASH,
  DK_MAX
};
typedef int DialogKind;

static DialogKind CurrentDialogBoxKind;
  /* Describes what kind of dialog box is currently open.  There can only be one
  open at a time, but it could be any type from this list. */

static void ShowDialogBox (DialogKind BoxType);
  /* Forward reference. */

static int DefaultBuySellMortChoice;
  /* One of ID_ACTION_BUY, ID_ACTION_SELL, ID_ACTION_MORTGAGE to control what
  the default initial radio button is in the BUYSELLMORT dialog box. */

TradingState               CurrentTradingState;
RULE_PlayerSet             TradeDestinationPlayer = NULL;// Stores the players to recieve(view) the trade, or 0 if it is a regular proposal.
static RULE_ActionArgumentsRecord TradingArguments;
static char                      *TradeBuffer;  /* A very long string, when allocated. */
  /* These values keep track of a trade proposal in progress. */



static RULE_PlayerNumber ChatToPlayer;
  /* Which player we are currently chatting to,
  or RULE_ALL_PLAYERS for everybody. */

static RULE_PlayerNumber ChatFromPlayer;
  /* Which of the local players is sending chat messages. */

/* The following is a list of AI names for PLAYER Notify message box. */
static char *AI_AILevelNames[AI_MAX_LEVELS + 1] =
{
  "None",
  "Dumb",
  "Average",
  "Expert"
};


/*****************************************************************************
 * Sets the current user interface player to the first local Human
 * player in the player set.  If none, sets it to RULE_NOBODY_PLAYER.
 */

void SetCurrentUIPlayerFromPlayerSet (RULE_PlayerSet PlayerSet)
{
  RULE_PlayerNumber PlayerNo;

  CurrentUIPlayer = RULE_NOBODY_PLAYER;
  for (PlayerNo = 0; PlayerNo < UICurrentGameState.NumberOfPlayers; PlayerNo++)
  {
    if ((PlayerSet & (1 << PlayerNo)) && SlotIsALocalHumanPlayer [PlayerNo])
    {
      CurrentUIPlayer = PlayerNo;
      break;
    }
  }
}


/*****************************************************************************
 * Sets the current user interface player to the given player if it is
 * a local Human player.  Otherwise sets the user interface to use
 * RULE_NOBODY_PLAYER.
 */

void SetCurrentUIPlayerFromPlayerNumber (RULE_PlayerNumber PlayerNo)
{
  CurrentUIPlayer = RULE_NOBODY_PLAYER;
  if (PlayerNo < RULE_MAX_PLAYERS && SlotIsALocalHumanPlayer [PlayerNo])
    CurrentUIPlayer = PlayerNo;
}



/*****************************************************************************
 * Finds an active local player or returns RULE_NOBODY_PLAYER.  Includes AIs.
 */

RULE_PlayerNumber AnyLocalPlayer (void)
{
  RULE_PlayerNumber PlayerNo;

  for (PlayerNo = 0; PlayerNo < UICurrentGameState.NumberOfPlayers; PlayerNo++)
    if (SlotIsALocalPlayer [PlayerNo])
      return PlayerNo;

  return RULE_NOBODY_PLAYER;
}



/*****************************************************************************
 * Adds a local player and requests that the rules engine enter it in the
 * game.  Later on, the rules engine may accept or deny the request.
 *
 * If TakeOverAI is TRUE then it will attempt to replace an AI player (with
 * preference for the one with the same token).
 */

void AddLocalPlayer (wchar_t *Name, RULE_TokenKind Token,
RULE_PlayerColour Colour, unsigned char AILevel, BOOL TakeOverAI)
{
  RULE_PlayerNumber BestPlayer;
  BOOL              LocalNameAlreadyAllocated;
  int               NameLength;
  RULE_PlayerNumber Player;

  if (NumberOfLocalPlayers < MAX_LOCAL_PLAYERS)
  {
    NameLength = wcslen (Name);
    if (NameLength <= 0 || NameLength > RULE_MAX_PLAYER_NAME_LENGTH)
      return; /* Don't use weird strings for names. */

    /* Make sure this is a new local name. */

    LocalNameAlreadyAllocated = FALSE;
    for (Player = 0; Player < NumberOfLocalPlayers; Player++)
    {
      if (wcscmp (Name, LocalPlayerNames [Player]) == 0)
      {
        LocalNameAlreadyAllocated = TRUE; /* Oops, found a duplicate name. */
        break;
      }
    }

    /* Remember the name as one we are responsible for. */

    if (!LocalNameAlreadyAllocated)
    {
      wcsncpy (LocalPlayerNames [NumberOfLocalPlayers], Name, RULE_MAX_PLAYER_NAME_LENGTH);
      LocalPlayerNames [NumberOfLocalPlayers] [RULE_MAX_PLAYER_NAME_LENGTH] = 0;
      LocalPlayerSlots [NumberOfLocalPlayers] = RULE_NOBODY_PLAYER;
      NumberOfLocalPlayers++;
    }

    /* Find the player we are replacing.  Try taking over an AI player.
       First find one.  If possible, use one with a matching token.  Also
       take any non-bankrupt one in preference to a bankrupt one. */

    BestPlayer = RULE_NOBODY_PLAYER; /* Means add a new player. */
    if (TakeOverAI)
    {
      for (Player = 0; Player < UICurrentGameState.NumberOfPlayers; Player++)
      {
        if (UICurrentGameState.Players[Player].AIPlayerLevel != 0 /* An AI! */)
        {
          if (UICurrentGameState.Players[Player].currentSquare == SQ_OFF_BOARD)
          { /* This one is bankrupt. */
            if (BestPlayer == RULE_NOBODY_PLAYER ||
            (UICurrentGameState.Players[BestPlayer].currentSquare == SQ_OFF_BOARD &&
            UICurrentGameState.Players[BestPlayer].token != Token))
              BestPlayer = Player; /* Only take a bankrupt AI if nothing better. */
          }
          else /* Not bankrupt. */
            BestPlayer = Player;

          if (UICurrentGameState.Players[BestPlayer].currentSquare != SQ_OFF_BOARD &&
          UICurrentGameState.Players[BestPlayer].token == Token)
            break; /* Found an exact match that isn't bankrupt. */
        }
      }
    }

    MESS_SendAction (ACTION_NAME_PLAYER, RULE_SPECTATOR_PLAYER, RULE_BANK_PLAYER,
      BestPlayer /* Requested ID */, AILevel, Token, Colour,
      Name, 0, NULL);
  }
}


/*****************************************************************************
 * This routine initializes a trade sequence. What is involved is going through
 * the display state and initializing all the "traded to" fields to
 * RULE_NOBODY_PLAYER.
 */

void InitializeTradeSequence(void)
{
  int i,j;

  // Set the cursor.
  //SetTradeItemCursor();

    // Initialize the trading buffer.
  if (!TradeBuffer)
    TradeBuffer = (char *) malloc(TRADE_BUFFER_SIZE);

  if (!TradeBuffer)
    return;

  // All the board squares.
  for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
    UICurrentGameState.Squares[i].offeredInTradeTo = RULE_NOBODY_PLAYER;

  // Money
  for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
    for(j=0; j<RULE_MAX_PLAYERS; j++)
      UICurrentGameState.Players[i].shared.trading.cashGivenInTrade[j] = 0;

  // Get out of jail cards.
  for(i=0; i<MAX_DECK_TYPES; i++)
    UICurrentGameState.Cards[i].jailOfferedInTradeTo = RULE_NOBODY_PLAYER;

  // Futures and immunities.
  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
    if (UICurrentGameState.CountHits[i].tradedItem)
    {
      UICurrentGameState.CountHits[i].toPlayer = RULE_NOBODY_PLAYER;
      UICurrentGameState.CountHits[i].tradedItem = FALSE;
    }
}


static void EndTradeSequence(void)
{
  int i;

  // Deallocate the trading buffer.
  if (TradeBuffer)
  {
    free(TradeBuffer);
    TradeBuffer = NULL;
  }

  /* Remove temporary immunities used during trading. */

  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
  {
    if (UICurrentGameState.CountHits[i].tradedItem)
    {
      UICurrentGameState.CountHits[i].tradedItem = FALSE;
      UICurrentGameState.CountHits[i].toPlayer = RULE_NOBODY_PLAYER;
    }
  }

  //SetArrowCursor();
}



/*****************************************************************************
 * Routine for creating the trade list buffer. This method is kind of slow
 * and kludgy but it is the only way of handling variable trading, ie
 * someone may propose a trade item and another will counter by removing
 * that item from the trade. We have no easy way of removing the old one,
 * so just remake the list every time.
 */
/*****************************************************************************
 * New Commment:  We're going to clean this puppy up, you betcha!
 */
static void ShowTradeInTradeBuffer(int FromPlayer, int ToPlayer, char *ItemString)
{
  char  FromPlayerString [RULE_MAX_PLAYER_NAME_LENGTH*2+2];
  char  TempString [RULE_MAX_PLAYER_NAME_LENGTH * 4 +
                   RULE_MAX_SQUARE_NAME_LENGTH * SQ_MAX_SQUARE_TYPES * 2];
  char  ToPlayerString [RULE_MAX_PLAYER_NAME_LENGTH*2+2];
  int   TradeBufLen;

  wcstombs( FromPlayerString,
            UICurrentGameState.Players[ FromPlayer ].name,
            sizeof (FromPlayerString) );
  wcstombs( ToPlayerString,
            UICurrentGameState.Players[ ToPlayer ].name,
            sizeof (ToPlayerString) );

  // Write the new item in a temporary string.
  sprintf (TempString, "%s from %s to %s\r\n", ItemString, FromPlayerString, ToPlayerString);

  /* Append the TempString to the end of the TradeBuffer. */

  TradeBufLen = strlen (TradeBuffer);
  if (TradeBufLen + strlen (TempString) >= TRADE_BUFFER_SIZE)
  {
#if _DEBUG
    LE_ERROR_ErrorMsg ("ShowTradeInTradeBuffer: trade buffer is full!");
#endif
  }
  else
  {
    strcpy (TradeBuffer + TradeBufLen, TempString);
  }
}


/*****************************************************************************
 * Blink the token on and off if desired.  Also, if you turn off the flashing,
 * it will blink on the token at the right time and then leave it on.
 */

static DWORD TimeOfLastFlash;

void DoTokenBlinking (void)
{
  DWORD CurrentTime;
  DWORD DeltaTime;
  DWORD TimeDelay;

  CurrentTime = GetTickCount ();
  DeltaTime = CurrentTime - TimeOfLastFlash;

  /* Flash rapidly if it is one of our pieces, otherwise slowly. */
  TimeDelay = (CurrentUIPlayer == UICurrentGameState.CurrentPlayer) ? 300 : 1000;

  if (RunFastWithNoDisplay || DeltaTime < TimeDelay /* milliseconds */)
    return; /* Not time yet to flash on or off.  Or running fast. */
  TimeOfLastFlash = CurrentTime;

  if (DISPLAY_state.FlashCurrentToken || !CurrentTokenIsOn)
  {
    int Player = UICurrentGameState.CurrentPlayer;
    int Square = UICurrentGameState.Players[Player].currentSquare;

    CurrentTokenIsOn = !CurrentTokenIsOn;

    // Invalidate only the rectangle the player is on and their status bar.
 //   InvalidateRect( hwndMain, &GameSquareRects[Square], FALSE );
 //   InvalidateRect( hwndMain, &PlayerStatusRects[Player], FALSE );
  }
}



/*****************************************************************************
 * Validate the from and to players.  The From player has to be a local human
 * or a spectator.  The To player can be anybody in the game or everybody.
 * If the current settings aren't valid, pick reasonable defaults.
 */

static void ValidateChatPlayers (void)
{
  RULE_PlayerNumber PlayerNo;

  if (ChatToPlayer >= UICurrentGameState.NumberOfPlayers)
    ChatToPlayer = RULE_ALL_PLAYERS;

  if (ChatFromPlayer == RULE_SPECTATOR_PLAYER)
    return; /* Avoid array index problems by returning now. */

  if (ChatFromPlayer >= UICurrentGameState.NumberOfPlayers ||
  !SlotIsALocalHumanPlayer[ChatFromPlayer])
  {
    /* From player is invalid.  Pick a local human if possible, or
       use the spectator if there aren't any local humans. */

    ChatFromPlayer = RULE_SPECTATOR_PLAYER;
    for (PlayerNo = 0; PlayerNo < UICurrentGameState.NumberOfPlayers; PlayerNo++)
    {
      if (SlotIsALocalHumanPlayer[PlayerNo])
      {
        ChatFromPlayer = PlayerNo;
        break;
      }
    }
  }
}



/*****************************************************************************
 * Display an error message in a modeless dialog box.  Any previous error box
 * is closed before the new one pops up.  Doesn't affect other non-error
 * dialog boxes on the screen.
 */

static void DisplayErrorNotification (RULE_ActionArgumentsPointer NewMessage)
{
  LANG_TextMessageNumber  ErrorCode;
  //char                    TempString [UI_MAX_ERROR_MESSAGE_LENGTH*2+1];
  //char                    TempString2 [UI_MAX_ERROR_MESSAGE_LENGTH*2+1];
  wchar_t                 TempWString [UI_MAX_ERROR_MESSAGE_LENGTH+1];

  ErrorCode = NewMessage->numberA;

  /* Open the box, if needed.  Otherwise just move to front. */

//  if (hdlgErrorDialogBox == NULL)
//  {
    if (RunFastWithNoDisplay)
      return; /* Don't pop up a box while in fast mode. */

    //CreateDialog (hAppInstance, MAKEINTRESOURCE (IDD_ERROR_MESSAGE),
    //  hwndMain, ErrorMessageBoxMsgProcessor);
//  }
  //else
  //  BringWindowToTop (hdlgErrorDialogBox);

  /* Get the error message from the text messages system. */

  FormatErrorNotification(NewMessage, TempWString);

  // NEW CODE - display to temporary message box
  //wcstombs (TempString, TempWString, UI_MAX_ERROR_MESSAGE_LENGTH * 2);
  //sprintf( TempString2, "NOTIFY_ERROR_MESSAGE(%d): ", ErrorCode );
  //strcat( TempString2, TempString );
  //DISPLAY_setMessageBox( TempString );

}



/*****************************************************************************
 * Display an error message in a modeless dialog box.  Any previous error box
 * is closed before the new one pops up.  Doesn't affect other non-error
 * dialog boxes on the screen.
 */

void FormatErrorNotification (RULE_ActionArgumentsPointer NewMessage, wchar_t *string)
{
  LANG_TextMessageNumber  ErrorCode;
  wchar_t                *InputStringPntr;
  long                    Length;
  long                    OutputIndex;
  RULE_PlayerNumber       PlayerNo;
  wchar_t                 TempWString [UI_MAX_ERROR_MESSAGE_LENGTH+1];

  ErrorCode = NewMessage->numberA;

  /* Get the error message from the text messages system. */

  InputStringPntr = LANG_GetTextMessage (ErrorCode);

  /* Substitute numbers and strings for the special codes.  All special
     codes begin with a "^".  Do processing in UniCode so that we don't
     get confused by escape codes that contain "^". */

  OutputIndex = 0;
  while (*InputStringPntr != 0 && OutputIndex < UI_MAX_ERROR_MESSAGE_LENGTH)
  {
    /* Copy all ordinary characters until we hit a control character. */

    if (*InputStringPntr != L'^')
    {
      string [OutputIndex++] = *InputStringPntr++;
      continue;
    }

    /* We hit a control character, figure out what it is, if it exists. */

    if (InputStringPntr[1] == 0)
      break; /* Funny situation - input ends with a ^. */

    wcscpy (TempWString, L"?"); /* For those undefined situations. */

    switch (towupper (InputStringPntr[1]))
    {
    case L'1':
      // Insert numberB as a number.
      swprintf (TempWString, L"%d", (int) NewMessage->numberB);
      break;

    case L'2':
      // Insert numberC as a number.
      swprintf (TempWString, L"%d", (int) NewMessage->numberC);
      break;

    case L'3':
      // Insert numberD as a number.
      swprintf (TempWString, L"%d", (int) NewMessage->numberD);
      break;

    case L'4':
      // Insert numberE as a number.
      swprintf (TempWString, L"%d", (int) NewMessage->numberE);
      break;

    case L'A':
      // Use the provided string argument.
      wcsncpy (TempWString, NewMessage->stringA, UI_MAX_ERROR_MESSAGE_LENGTH);
      break;

    case L'P':
    case L'Q':
      // Use player name given by numberC (or numberB for ^Q) index.
      if (towupper (InputStringPntr[1]) == L'P')
        PlayerNo = (RULE_PlayerNumber) NewMessage->numberC;
      else /* Got ^Q. */
        PlayerNo = (RULE_PlayerNumber) NewMessage->numberB;

      if (PlayerNo < RULE_MAX_PLAYERS)
        wcsncpy (TempWString, UICurrentGameState.Players[PlayerNo].name, UI_MAX_ERROR_MESSAGE_LENGTH);
      else if (PlayerNo == RULE_BANK_PLAYER)
        wcsncpy (TempWString, LANG_GetTextMessage (TMN_BANK_NAME), UI_MAX_ERROR_MESSAGE_LENGTH);
      else if (PlayerNo == RULE_NOBODY_PLAYER)
        wcsncpy (TempWString, LANG_GetTextMessage (TMN_NOBODY_NAME), UI_MAX_ERROR_MESSAGE_LENGTH);
      else
        wcsncpy (TempWString, LANG_GetTextMessage (TMN_SPECTATOR_NAME), UI_MAX_ERROR_MESSAGE_LENGTH);
      break;

    case L'S':
      // Convert the Square number given in numberD to the Square name.
      if (NewMessage->numberD >= 0 && NewMessage->numberD < SQ_MAX_SQUARE_TYPES)
        wcsncpy (TempWString,
        RULE_SquarePredefinedInfo[NewMessage->numberD].squareName,
        UI_MAX_ERROR_MESSAGE_LENGTH);
      break;

    case L'T':
      // Insert list of square names from NumberE.
      RULE_PrintSetOfPropertyNames (NewMessage->numberE, TempWString,
        sizeof (TempWString) / sizeof (wchar_t));
      break;

    default:
      TempWString [0] = L'^';
      TempWString [1] = *InputStringPntr;
      TempWString [2] = 0;
      break;
    }
    InputStringPntr += 2; /* Used up the ^ and the following character. */

    // Copy the string to insert to our output.

    Length = wcslen (TempWString);
    if (OutputIndex + Length > UI_MAX_ERROR_MESSAGE_LENGTH)
    {
      Length = UI_MAX_ERROR_MESSAGE_LENGTH - OutputIndex;
      if (Length < 0)
        Length = 0;
    }

    wcsncpy (string + OutputIndex, TempWString, Length);
    OutputIndex += Length;
  }
  string [OutputIndex] = 0; /* End of string marker. */
}



/*****************************************************************************
 * DBUG_DisplayInternationalMessage
 *
 * DESCRIPTION:
 * Display a general purpose message to the user in a Windows message box.
 * This replaces MessageBox() in effect, allowing us to use multilingual
 * text instead.  The usual message string codes are expanded into the
 * numeric and string parameters supplied.  Note that this isn't for
 * debugging only, it should work even if compiled with debug turned off.
 *****************************************************************************/

void DBUG_DisplayInternationalMessage (LANG_TextMessageNumber MessageNumber,
long Number1, long Number2, long Number3, wchar_t *StringParameter)
{
  RULE_ActionArgumentsRecord FakeRuleMessage;

  memset (&FakeRuleMessage, 0, sizeof (FakeRuleMessage));
  FakeRuleMessage.action = NOTIFY_ERROR_MESSAGE;
  FakeRuleMessage.numberA = MessageNumber;
  FakeRuleMessage.numberB = Number1;
  FakeRuleMessage.numberC = Number2;
  FakeRuleMessage.numberD = Number3;
  if (StringParameter != NULL)
  {
    wcsncpy (FakeRuleMessage.stringA, StringParameter, RULE_ACTION_ARGUMENT_STRING_LENGTH);
    FakeRuleMessage.stringA[RULE_ACTION_ARGUMENT_STRING_LENGTH] = 0;
  }

  DisplayErrorNotification (&FakeRuleMessage);
}



/*****************************************************************************
 * A utility function for removing a name from our list of local players.
 */

static void RemoveLocalPlayerByName (wchar_t *DeadName)
{
  int i;
  int LastPlayerNo;
  int OldSlot;

  LastPlayerNo = NumberOfLocalPlayers - 1;

  for (i = 0; i < NumberOfLocalPlayers; i++)
  {
    if (wcscmp (DeadName, LocalPlayerNames [i]) == 0)
    {
      /* Wipe out the dead player. */

      LocalPlayerNames [i] [0] = 0;
      OldSlot = LocalPlayerSlots [i];
      LocalPlayerSlots [i] = RULE_NOBODY_PLAYER;
      if (OldSlot != RULE_NOBODY_PLAYER)
      {
        SlotIsALocalPlayer [OldSlot] = FALSE;
        SlotIsALocalHumanPlayer [OldSlot] = FALSE;
        SlotIsALocalAIPlayer [OldSlot] = FALSE;
      }

      if (i < LastPlayerNo)
      {
        /* Move the one at the end into the vacated spot. */

        wcscpy (LocalPlayerNames [i], LocalPlayerNames [LastPlayerNo]);
        LocalPlayerSlots [i] = LocalPlayerSlots [LastPlayerNo];

        LocalPlayerNames [LastPlayerNo] [0] = 0;
        LocalPlayerSlots [LastPlayerNo] = RULE_NOBODY_PLAYER;
      }

      NumberOfLocalPlayers--;
      return;
    }
  }
}



/*****************************************************************************
 */
void RemoveLocalPlayer( RULE_PlayerNumber PlayerNo )
{
  wchar_t   nullName[_MAX_PATH];

  if ( (PlayerNo < 0) || (PlayerNo >= UICurrentGameState.NumberOfPlayers) )
    return;

  if ( NumberOfLocalPlayers <= 0 )
    return;

  RemoveLocalPlayerByName( UICurrentGameState.Players[PlayerNo].name );

  // Naming with a null string effects a delete.
  nullName[0] = 0;
  MESS_SendAction( ACTION_NAME_PLAYER, PlayerNo, RULE_BANK_PLAYER,
   PlayerNo,
   UICurrentGameState.Players[PlayerNo].AIPlayerLevel,
   UICurrentGameState.Players[PlayerNo].token,
   UICurrentGameState.Players[PlayerNo].colour,
   nullName,
   0, NULL);
}


/*****************************************************************************
 * Got a new name in the system.  If it matches one of the names we were
 * trying to register, keep our local name.  If the action was an error
 * message saying the name was rejected, forget our local name.
 */

void CheckForAcceptingOurNewPlayer (RULE_ActionArgumentsPointer NewMessage)
{
  BOOL                          ForgetThisAI;
  int                           i, j;
  RULE_PlayerNumber             NewSlot;
  unsigned char                 OldAIPlayerLevel;
  RULE_PlayerColour             OldColour;
  wchar_t                       OldName [RULE_MAX_PLAYER_NAME_LENGTH+1];
  RULE_PlayerNumber             OldSlot;
  RULE_TokenKind                OldToken;
  #define   TEMP_AI_NAME_LENGTH _MAX_PATH
  wchar_t                       TempAIName [TEMP_AI_NAME_LENGTH];

  if (NewMessage->action == NOTIFY_ERROR_MESSAGE)
  {
    if (NewMessage->numberA == TMN_ERROR_NAME_IN_USE ||
    NewMessage->numberA == TMN_ERROR_NO_MORE_PLAYERS ||
    NewMessage->numberA == TMN_ERROR_NOT_YOUR_PLAYER)
    {
      /* An error message about a failed naming action.  If we
         already own that player, ignore it.  If we were trying
         to sign it up, forget that name. */

      for (i = 0; i < NumberOfLocalPlayers; i++)
      {
        if (wcscmp (NewMessage->stringA, LocalPlayerNames [i]) == 0 &&
        LocalPlayerSlots [i] == RULE_NOBODY_PLAYER)
          RemoveLocalPlayerByName (NewMessage->stringA);
      }
    }
    return;
  }

  if (NewMessage->action == NOTIFY_NAME_PLAYER)
  {
    NewSlot = (RULE_PlayerNumber) NewMessage->numberA;

    if (NewSlot >= RULE_MAX_PLAYERS)
      return; /* Ignore garbage values. */

    /* Save the previous settings, in case it is a change of player. */

    OldAIPlayerLevel = UICurrentGameState.Players[NewSlot].AIPlayerLevel;
    OldColour = UICurrentGameState.Players[NewSlot].colour;
    OldToken = UICurrentGameState.Players[NewSlot].token;
    wcsncpy (OldName, UICurrentGameState.Players[NewSlot].name,
      RULE_MAX_PLAYER_NAME_LENGTH);
    OldName [RULE_MAX_PLAYER_NAME_LENGTH] = 0;

    /* Update the global settings for the new player name etc. */

    UICurrentGameState.Players[NewSlot].token =
      NewMessage->numberB;
    UICurrentGameState.Players[NewSlot].colour =
      NewMessage->numberC;
    UICurrentGameState.Players[NewSlot].AIPlayerLevel =
      (unsigned char) NewMessage->numberD;
    wcsncpy (UICurrentGameState.Players[NewSlot].name, NewMessage->stringA,
      RULE_MAX_PLAYER_NAME_LENGTH);
    UICurrentGameState.Players[NewSlot].name[RULE_MAX_PLAYER_NAME_LENGTH] = 0;

    /* Someone has joined the game.  Or is getting a fresh copy of all the
       state information.  Or has replaced someone else.  Is it our name? */

    for (i = 0; i < NumberOfLocalPlayers; i++)
    {
      if (wcscmp (NewMessage->stringA, LocalPlayerNames [i]) == 0)
      {
        /* If we have already registered our name previously, ignore the
           duplicate name notification (to avoid loading the AI during an
           active game when the names are rebroadcast). */

        OldSlot = LocalPlayerSlots [i];

        if (SlotIsALocalPlayer [NewSlot] &&
        OldSlot == NewSlot &&
        OldAIPlayerLevel == NewMessage->numberD &&
        OldToken == NewMessage->numberB &&
        OldColour == NewMessage->numberC)
          return;

        /* Make sure none of our local players are using
           the new slot or the old one. */

        for (j = 0; j < NumberOfLocalPlayers; j++)
        {
          if (LocalPlayerSlots [j] == NewSlot || LocalPlayerSlots [j] == OldSlot)
            LocalPlayerSlots [j] = RULE_NOBODY_PLAYER;
        }

        /* Remove our player from the previous slot it was in, if any. */

        if (OldSlot != RULE_NOBODY_PLAYER)
        {
          SlotIsALocalPlayer [OldSlot] = FALSE;
          SlotIsALocalHumanPlayer [OldSlot] = FALSE;
          SlotIsALocalAIPlayer [OldSlot] = FALSE;
        }

        /* Assign the player to the new slot. */

        SlotIsALocalPlayer [NewSlot] = TRUE;
        LocalPlayerSlots [i] = NewSlot;

        if (NewMessage->numberD) /* Is this an AI player? */
        {
          SlotIsALocalAIPlayer[NewSlot] = TRUE;
          SlotIsALocalHumanPlayer[NewSlot] = FALSE;

          /* Try loading the AI.  The loader will associate an AI file name with
             the user defined name and remember it for later reuse, if the AI gets
             renamed to a different slot. */

          wcsncpy (TempAIName, NewMessage->stringA, TEMP_AI_NAME_LENGTH);

          ForgetThisAI = !AI_Load_AI (NewSlot, NewMessage->numberB /* token */,
            NewMessage->numberD /* level */);

          if (ForgetThisAI)
          {
            /* Failed to load the AI, forget this player. */
            RemoveLocalPlayerByName (NewMessage->stringA);

            TempAIName [0] = 0; /* Make it an empty string. */

            /* Tell the rules to deallocate the failed AI player. */

            MESS_SendAction (ACTION_NAME_PLAYER, (RULE_PlayerNumber) NewSlot, RULE_BANK_PLAYER,
              NewSlot /* Requested ID */,
              NewMessage->numberD /* Is it an AI */,
              NewMessage->numberB /* Token */,
              NewMessage->numberC /* Colour */,
              TempAIName /* Empty string for name */, 0, NULL);
          }
        }
        else /* A Human player. */
        {
          SlotIsALocalHumanPlayer [NewSlot] = TRUE;
          SlotIsALocalAIPlayer[NewSlot] = FALSE;
        }
        DISPLAY_showAll;
        return;
      }
    }

    /* Nope, someone else got lucky.  Make sure none of our local
       players are using the new slot (keep them around and slotless,
       in case this was a reordering of players). */

    for (j = 0; j < NumberOfLocalPlayers; j++)
    {
      if (LocalPlayerSlots [j] == NewSlot)
        LocalPlayerSlots [j] = RULE_NOBODY_PLAYER;
    }

    SlotIsALocalPlayer [NewSlot] = FALSE;
    SlotIsALocalAIPlayer [NewSlot] = FALSE;
    SlotIsALocalHumanPlayer [NewSlot] = FALSE;
    DISPLAY_showAll;
    return;
  }

  if (NewMessage->action == NOTIFY_PLAYER_DELETED)
  {
    RemoveLocalPlayerByName (NewMessage->stringA);
    DISPLAY_showAll;
    return;
  }

  if (NewMessage->action == NOTIFY_ADD_LOCAL_PLAYER)
  {
    NewSlot = (RULE_PlayerNumber) NewMessage->numberA;

    if (NewSlot >= RULE_MAX_PLAYERS)
      return; /* Ignore garbage stuff. */

    /* Adding a new player on the server.  First remove any old one that is
       using that name, including on clients. */

    RemoveLocalPlayerByName (NewMessage->stringA);

    /* Also remove any local players that are using the slot where
       the new local player will go. */

    for (j = 0; j < NumberOfLocalPlayers; j++)
    {
      if (LocalPlayerSlots [j] == NewSlot)
      {
        RemoveLocalPlayerByName (LocalPlayerNames [j]);
        j = -1; /* Restart the loop since name list has changed. */
      }
    }

    /* If this is a server computer, add the name to the ones we will be
       running.  It will be assigned a slot soon via NOTIFY_NAME_PLAYER. */

    if (MESS_ServerMode && NumberOfLocalPlayers < MAX_LOCAL_PLAYERS)
    {
      LocalPlayerSlots [NumberOfLocalPlayers] = RULE_NOBODY_PLAYER;
      wcsncpy (LocalPlayerNames [NumberOfLocalPlayers], NewMessage->stringA,
        RULE_MAX_PLAYER_NAME_LENGTH);
      LocalPlayerNames [NumberOfLocalPlayers] [RULE_MAX_PLAYER_NAME_LENGTH] = 0;
      NumberOfLocalPlayers++;
    }
    DISPLAY_showAll;
    return;
  }
}



/*****************************************************************************
 * Process the resynchronisation information, which can change the whole
 * board.  It includes cash, token positions, ownership, mortgages.
 */

static void ProcessResyncInfo (RULE_ActionArgumentsPointer NewMessage)
{
  RULE_CardDeckTypes            DeckNo;
  RULE_PropertySet              MortgagedProperties;
  RULE_PlayerNumber             PlayerNo;
  RULE_PropertySet              OwnedProperties;
  RULE_ClientResyncInfoPointer  ResyncPntr;
  RULE_SquareType               SquareNo;
  RULE_SquareInfoPointer        SquarePntr;

  ResyncPntr = (RULE_ClientResyncInfoPointer) (&NewMessage->stringA);

  UICurrentGameState.CurrentPlayer = ResyncPntr->currentPlayer;

  /* Reset all ownership and mortgages and house counts. */

  for (SquareNo = 0; SquareNo <= SQ_MAX_SQUARE_TYPES; SquareNo++)
  {
    SquarePntr = UICurrentGameState.Squares + SquareNo;
    SquarePntr->owner = RULE_NOBODY_PLAYER;
    SquarePntr->mortgaged = FALSE;
    SquarePntr->houses = 0;
  }

  /* Set the ownerships. */

  for (PlayerNo = 0; PlayerNo < RULE_MAX_PLAYERS; PlayerNo++)
  {
    OwnedProperties = ResyncPntr->propertiesOwned[PlayerNo];
    while ((SquareNo = RULE_BitSetToProperty (OwnedProperties)) <= SQ_BOARDWALK)
    {
      UICurrentGameState.Squares[SquareNo].owner = PlayerNo;
      OwnedProperties &= ~RULE_PropertyToBitSet (SquareNo);
    }
  }

  /* Set the mortgaged statuses. */

  MortgagedProperties = ResyncPntr->mortgagedProperties;
  while ((SquareNo = RULE_BitSetToProperty (MortgagedProperties)) <= SQ_BOARDWALK)
  {
    UICurrentGameState.Squares[SquareNo].mortgaged = TRUE;
    MortgagedProperties &= ~RULE_PropertyToBitSet (SquareNo);
  }

  /* Set the house counts. */

  for (SquareNo = 0; SquareNo <= SQ_BOARDWALK; SquareNo++)
  {
    SquarePntr = UICurrentGameState.Squares + SquareNo;
    SquarePntr->houses = ResyncPntr->houseCounts[SquareNo];
  }

  /* Set the player's cash amounts and current square. */

  for (PlayerNo = 0; PlayerNo < RULE_MAX_PLAYERS; PlayerNo++)
  {
    UICurrentGameState.Players[PlayerNo].cash = ResyncPntr->cash[PlayerNo];
    UICurrentGameState.Players[PlayerNo].currentSquare = ResyncPntr->playerSquare[PlayerNo];
  }

  /* Set the jail cards. */

  for (DeckNo = 0; DeckNo < MAX_DECK_TYPES; DeckNo++)
    UICurrentGameState.Cards[DeckNo].jailOwner = ResyncPntr->jailOwners [DeckNo];

  /* Redraw the whole display. */
  DISPLAY_showAll;
}



/*****************************************************************************
 * Optionally, ask the user for a file name to use for saving game options or
 * whole games, then writes the blob in the given message to the file.
 */
#if 0
static BOOL SaveGameOrOptions (RULE_ActionArgumentsPointer NewMessage)
{
  BOOL                  SavingGame;
  BOOL                  FileNameChosen;
  OPENFILENAME          ofnTemp;
  FILE                 *OutputFile = NULL;
  char                  PathName [_MAX_PATH];
  BOOL                  savedSuccessfully;

  // Default return code of success.
  savedSuccessfully = TRUE;

  if (NewMessage == NULL ||
  (NewMessage->action != NOTIFY_GAME_STATE_FOR_SAVE &&
  NewMessage->action != NOTIFY_OPTIONS_FOR_SAVE) ||
  NewMessage->binaryDataA == NULL)
    return FALSE;

  SavingGame = (NewMessage->action == NOTIFY_GAME_STATE_FOR_SAVE);

  if ( NewMessage->numberA == 1 )
  {
    // Override prompting for a save file name.  A null-terminated wide-char
    // name has been provided in the stringA member of the message.
    WideSzToMultiByteSz( PathName, NewMessage->stringA, sizeof(PathName) );
  }
  else
  {
    // Prompt for file name requested.

    strcpy (PathName, SavingGame ? "*.msv" : "*.mop");

    memset (&ofnTemp, 0, sizeof (ofnTemp));
    ofnTemp.lStructSize = sizeof (OPENFILENAME);
    ofnTemp.lpstrFilter = SavingGame ?
     "Monopoly Games\0*.MSV\0All Files (*.*)\0*.*\0" :
     "Monopoly Options\0*.MOP\0All Files (*.*)\0*.*\0";
    ofnTemp.lpstrFile = PathName;
    ofnTemp.nMaxFile = sizeof (PathName);
    ofnTemp.lpstrInitialDir = ".";
    ofnTemp.lpstrTitle = SavingGame ? "Save Game" : "Save Options";
    ofnTemp.Flags = OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_NOREADONLYRETURN |
     OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    ofnTemp.lpstrDefExt = SavingGame ? "MSV" : "MOP";

    // Show a windoze dialog for choosing a save file name.  Enable/disable
    // the windoze cursor while dialog is up.
    ShowCursor( TRUE );
    FileNameChosen = GetSaveFileName( &ofnTemp );
    ShowCursor( FALSE );

    if ( !FileNameChosen )
      goto FAILURE_EXIT; /* Cancelled or didn't get file name. */
  }

  OutputFile = fopen (PathName, "wb");
  if (OutputFile == NULL)
    goto FAILURE_EXIT;

  if (fwrite (NewMessage->binaryDataA, NewMessage->binaryDataSizeA, 1,
  OutputFile) != 1)
    goto FAILURE_EXIT;
  goto NORMAL_EXIT;

FAILURE_EXIT:
  if ( FileNameChosen )
    LE_ERROR_ErrorMsg ("Error while saving file.");
  savedSuccessfully = FALSE;

NORMAL_EXIT:
  if (OutputFile != NULL)
    fclose (OutputFile);

  return savedSuccessfully;
}


#else
static BOOL SaveGameOrOptions (RULE_ActionArgumentsPointer NewMessage)
{
  BOOL                  SavingGame;
  //BOOL                  FileNameChosen;
  FILE                 *OutputFile = NULL;
  char                  PathName [_MAX_PATH];
  char                  PathName2 [_MAX_PATH];
  BOOL                  savedSuccessfully;
  char                  szTempText [500];
  ZeroMemory(szTempText, 500);

  // Default return code of success.
  savedSuccessfully = TRUE;

  if (NewMessage == NULL ||
  (NewMessage->action != NOTIFY_GAME_STATE_FOR_SAVE &&
  NewMessage->action != NOTIFY_OPTIONS_FOR_SAVE) ||
  NewMessage->binaryDataA == NULL)
    return FALSE;

  SavingGame = (NewMessage->action == NOTIFY_GAME_STATE_FOR_SAVE);


    //char szFile[MAX_PATH];
  char szPath[MAX_PATH];
/*
    // Format the path to the texture templates for medium mesh textures
    sprintf(szFile, "\\%s", "directx.ini");

    // Try to reference the first texture
    if (!LE_DIRINI_ExpandFileName(szFile, szPath, LE_DIRINI_GlobalPathList))
      szPath[0] = '\0';

    // Okay, we found the first template.  Extract the path from it.
    char* szTmp = strrchr(szPath, '\\');
    szTmp[1] = '\0';
*/
  UDUTILS_GetModulePath(szPath, MAX_PATH);


  strcat(szPath, "savegame\\");

    // try to create the directory
    CreateDirectory( szPath, NULL);

  // set the game to load
  sprintf(PathName, "%sgame%i.msv", szPath, CurrentGameFile+1);
  sprintf(PathName2, "%sgame%i.sgd", szPath, CurrentGameFile+1);

  FILE* fileptr = fopen(PathName2, "wb");
  if(fileptr != NULL)
  {    
    SaveGameStuff[CurrentGameFile].city = DISPLAY_state.city;
    SaveGameStuff[CurrentGameFile].system = DISPLAY_state.system;

    // save the custom board if any
    if(DISPLAY_state.city == -1)
    {
        //wcstombs( szTempText, BoardFileNames[CurrentBoardFile],
        //    Astrlen(BoardFileNames[CurrentBoardFile]));
        strcpy(SaveGameStuff[CurrentGameFile].CustomBoardName, 
                DISPLAY_state.customBoardPath);
    }
    else
    {
        strcpy(SaveGameStuff[CurrentGameFile].CustomBoardName, "");
    }


    for(int i=0; i<SQ_MAX_SQUARE_TYPES; i++)
	{
		SaveGameStuff[CurrentGameFile].square_game_earnings[i] = UICurrentGameState.Squares[i].game_earnings;
    }
    
    fwrite(&(SaveGameStuff[CurrentGameFile]), sizeof(SaveGameStruct), 1, fileptr); 
        
    fclose(fileptr);
  }

  OutputFile = fopen (PathName, "wb");
  if (OutputFile == NULL)
    goto FAILURE_EXIT;

  if (fwrite (NewMessage->binaryDataA, NewMessage->binaryDataSizeA, 1,
  OutputFile) != 1)
    goto FAILURE_EXIT;
  goto NORMAL_EXIT;

FAILURE_EXIT:
  UDSOUND_Warning();
  savedSuccessfully = FALSE;

NORMAL_EXIT:
  if (OutputFile != NULL)
    fclose (OutputFile);

  return savedSuccessfully;
}
#endif


/*****************************************************************************
 * Ask the user for a file name then load it and fire off a message with
 * the contents in a blob telling the rules engine to use it.
 * If LoadingGame is TRUE then we load a game, otherwise we do options.
 */

void LoadGameOrOptions (BOOL LoadingGame)
{
  long                  FileSize;
  FILE                 *InputFile;
  RULE_CharHandle       MemoryHandle = NULL;
  BYTE                 *MemoryPntr = NULL;

  OPENFILENAME          ofnTemp;
  char                  PathName [_MAX_PATH];

  /* Get the name of the file to load from the user. */

  strcpy (PathName, LoadingGame ? "*.msv" : "*.mop");

  memset (&ofnTemp, 0, sizeof (ofnTemp));
  ofnTemp.lStructSize = sizeof (OPENFILENAME);
//  ofnTemp.hwndOwner = hwndMain;
  ofnTemp.lpstrFilter = LoadingGame ?
    "Monopoly Games\0*.MSV\0All Files (*.*)\0*.*\0" :
    "Monopoly Options\0*.MOP\0All Files (*.*)\0*.*\0";
  ofnTemp.lpstrFile = PathName;
  ofnTemp.nMaxFile = sizeof (PathName);
  ofnTemp.lpstrInitialDir = ".";
  ofnTemp.lpstrTitle = LoadingGame ? "Load Game" : "Load Options";
  ofnTemp.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES |
    OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
  ofnTemp.lpstrDefExt = LoadingGame ? "MSV" : "MOP";

  if (!GetOpenFileName (&ofnTemp)) // FIXME - this hard codes the load name for demos.
    return; /* Cancelled or didn't get file name. */

  /* Try opening the file. */

  InputFile = fopen (PathName, "rb");
  if (InputFile == NULL)
    goto FAILURE_EXIT;

  /* Find the size of the file. */

  if (fseek (InputFile, 0, SEEK_END) != 0)
    goto FAILURE_EXIT;

  FileSize = ftell (InputFile);
  if (FileSize < 12 /* RIFF files are at least 12 bytes long, -1 for errors */)
    goto FAILURE_EXIT;

  if (fseek (InputFile, 0, SEEK_SET) != 0)
    goto FAILURE_EXIT;

  /* Allocate a buffer to hold the entire file. */

  MemoryHandle = RULE_AllocateHandle (FileSize);
  if (MemoryHandle == NULL)
    goto FAILURE_EXIT;

  MemoryPntr = (unsigned char *) RULE_LockHandle (MemoryHandle);
  if (MemoryPntr == NULL)
    goto FAILURE_EXIT;

  /* Read in the file. */

  if (fread (MemoryPntr, FileSize, 1, InputFile) != 1)
    goto FAILURE_EXIT;

  /* Send it as a blob to the rules engine. */

  RULE_UnlockHandle (MemoryHandle);
  MemoryPntr = NULL; /* Has to be unlocked before handover. */

  MESS_SendAction (LoadingGame ? ACTION_SET_GAME_STATE : ACTION_ACCEPT_CONFIGURATION,
    AnyLocalPlayer (), RULE_BANK_PLAYER,
    0, 0, 1 /* Version 1 for immunities & futures */, 0, NULL, 0, MemoryHandle);

  MemoryHandle = NULL; /* Message queue takes care of it now. */
  goto NORMAL_EXIT;

FAILURE_EXIT:
  LE_ERROR_ErrorMsg ("Error while loading file.");
NORMAL_EXIT:
  if (MemoryPntr != NULL)
    RULE_UnlockHandle (MemoryHandle);
  if (MemoryHandle != NULL)
    RULE_FreeHandle (MemoryHandle);
  if (InputFile != NULL)
    fclose (InputFile);
}


/*****************************************************************************
 * Handle the NOTIFY_PROPOSED_CONFIGURATION message.  Update our local game
 * options and set the current UI player to one of the ones who hasn't
 * accepted yet.
 */

void CopyNewGameOptions (RULE_ActionArgumentsPointer NewMessage)
{
  /* Get the new game options settings. */

  RULE_ConvertFileToGameOptions (NewMessage->binaryDataA,
    NewMessage->binaryDataSizeA, &UICurrentGameState.GameOptions);

  /* If the host is ignorant about immunities and futures, turn them off.
     Possibly should disable/enable immunities & future buttons here. */

  if (NewMessage->numberC < 1)
  {
    UICurrentGameState.GameOptions.futureRentTradingAllowed = FALSE;
    UICurrentGameState.GameOptions.immunitiesTradingAllowed = FALSE;
  }
}



/*****************************************************************************
 * Display the known network addresses of the various computers.
 */

static char *NetSysToString [NS_MAX] =
{
  "NS_LOCAL",
  "NS_DIRECTPLAY",
  "NS_WINSOCK"
};

static void ShowNetworkAddresses (void)
{
  typedef struct NetAddressForDirectPlayStruct
  {
    RULE_NetworkSystem  networkSystem; /* Should be NS_DIRECTPLAY. */
    DWORD               playerID;
    struct sockaddr_in  filler; /* Not used, just for padding. */
  } NetAddressForDirectPlayRecord, *NetAddressForDirectPlayPointer;
    /* A variant of the RULE_NetworkAddressRecord
    which is used for DirectPlay addresses.  Copied from MESS.H */


  typedef struct NetAddressForWinSockStruct
  {
    RULE_NetworkSystem  networkSystem; /* Should be NS_WINSOCK. */
    DWORD               connectionIndex; /* Index into WinSockPartialData. */
    struct sockaddr_in  IPAddress; /* The IP address and port etc. */
  } NetAddressForWinSockRecord, *NetAddressForWinSockPointer;
    /* A variant of the RULE_NetworkAddressRecord
    which is used for WinSock addresses.  Copied from MESS.H */

  int                       ComputerIndex;
  char                     *StringPntr;
  RULE_NetworkAddressRecord TempNetAddr;
  char                      TempString [5000];

  TempString [0] = 0;
  StringPntr = TempString;
  ComputerIndex = 0;

  while (MESS_GetComputerAddressFromIndex (ComputerIndex, &TempNetAddr) &&
  (StringPntr - TempString) < 4900)
  {
    sprintf (StringPntr, "%2d: %s",
      ComputerIndex, NetSysToString[TempNetAddr.networkSystem]);
    StringPntr += strlen (StringPntr);

    if (TempNetAddr.networkSystem == NS_DIRECTPLAY)
    {
      sprintf (StringPntr, ", Player #%d",
        (int) ((NetAddressForDirectPlayPointer)(&TempNetAddr))->playerID);
      StringPntr += strlen (StringPntr);
    }

    if (TempNetAddr.networkSystem == NS_WINSOCK)
    {
      sprintf (StringPntr, ", #%2d, %d.%d.%d.%d",
        (int) ((NetAddressForWinSockPointer)(&TempNetAddr))->connectionIndex,
        (int) ((NetAddressForWinSockPointer)(&TempNetAddr))->IPAddress.sin_addr.S_un.S_un_b.s_b1,
        (int) ((NetAddressForWinSockPointer)(&TempNetAddr))->IPAddress.sin_addr.S_un.S_un_b.s_b2,
        (int) ((NetAddressForWinSockPointer)(&TempNetAddr))->IPAddress.sin_addr.S_un.S_un_b.s_b3,
        (int) ((NetAddressForWinSockPointer)(&TempNetAddr))->IPAddress.sin_addr.S_un.S_un_b.s_b4);
      StringPntr += strlen (StringPntr);
    }

    strcat (StringPntr, "\r\n");
    StringPntr += strlen (StringPntr);

    ComputerIndex++;
  }

  if (TempString [0] == 0)
    strcpy (TempString, "None.");

//  MessageBox (hwndMain, TempString, "Known Network Addresses", MB_OK);
}



/*****************************************************************************
 * A menu item was selected.  Return TRUE if we handled it.
 */

/*BOOL ProcessMenuItem (int wmID)
{
  int   i;
  char  TempString [2 * RULE_MAX_PLAYER_NAME_LENGTH + 100];

  switch (wmID)
  {
    case ID_FILE_EXIT:
      PostMessage (hwndMain, WM_CLOSE, 0, 0);
      return TRUE;

    case ID_FILE_NEWGAME:
      SetCurrentUIPlayerFromPlayerSet ((RULE_PlayerSet) -1 // Any local human
        );
      MESS_SendAction (ACTION_NEW_GAME, AnyLocalPlayer (), RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
      return TRUE;

    case ID_FILE_ADDPLAYER:
      ShowDialogBox (DK_ENTER_NAME);
      return TRUE;

    case ID_FILE_REPLACEPLAYER:
      i = DialogBoxParam (hAppInstance, MAKEINTRESOURCE (IDD_SELECT_PLAYER),
        hwndMain, SelectPlayerBoxMsgProcessor,
        !MESS_ServerMode // Just local humans on clients, anyone on server
            );
      if (i < RULE_MAX_PLAYERS) // If didn't cancel.
      {
        if (GameInProgress)
        {
          MESS_SendAction (ACTION_DISCONNECTED_PLAYER,
            (RULE_PlayerNumber) (MESS_ServerMode ? RULE_SPECTATOR_PLAYER : i),
            RULE_BANK_PLAYER,
            i, 0, 0, 0, NULL, 0, NULL);
        }
        else // In player selection - really delete the player, not replace him.
        {
          MESS_SendAction (ACTION_NAME_PLAYER,
            (RULE_PlayerNumber) ((SlotIsALocalPlayer [i]) ? i : RULE_SPECTATOR_PLAYER),
            RULE_BANK_PLAYER,
            i // Requested ID
            .UICurrentGameState.Players[i].AIPlayerLevel,
            UICurrentGameState.Players[i].token,
            UICurrentGameState.Players[i].colour,
            L"" // Empty new name to delete the player
            , 0, NULL);
        }
      }
      return TRUE;

    case ID_FILE_STARTGAME:
      MESS_SendAction (ACTION_START_GAME, AnyLocalPlayer(), RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
      return TRUE;

    case ID_FILE_STARTDRONEGAME:
      AI_Drone_Game = !AI_Drone_Game;
      if(AI_Drone_Game) // Is this an AI vs AI super-fast marathon?
        AI_Player_Won (RULE_NOBODY_PLAYER); // Then start it!
      CheckMenuItem (hmenuMain, ID_FILE_STARTDRONEGAME,
        AI_Drone_Game ? MF_CHECKED : MF_UNCHECKED);
      return TRUE;

    case ID_FILE_SAVEGAME:  // Note that spectators can't save games.
      MESS_SendAction (ACTION_GET_GAME_STATE_FOR_SAVE, AnyLocalPlayer (), RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
      return TRUE;

    case ID_FILE_LOADGAME:
      LoadGameOrOptions (TRUE // Loading a game
      );
      return TRUE;

    case ID_FILE_SAVEOPTIONS: // Note that spectators can't save options.
      MESS_SendAction (ACTION_GET_OPTIONS_FOR_SAVE, AnyLocalPlayer (), RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
      return TRUE;

    case ID_FILE_LOADOPTIONS:
      LoadGameOrOptions (FALSE // Loading options
      );
      return TRUE;

    case ID_ACTION_PROPOSE_TRADE:
      i = DialogBoxParam (hAppInstance, MAKEINTRESOURCE (IDD_SELECT_PLAYER),
        hwndMain, SelectPlayerBoxMsgProcessor, TRUE // Just local humans
        );
      if (i < RULE_MAX_PLAYERS) // If didn't cancel.
      {
        CurrentUIPlayer = i;
        MESS_SendAction (ACTION_START_TRADE_EDITING, CurrentUIPlayer, RULE_BANK_PLAYER,
            1, 0, 0, 0, NULL, 0, NULL);
      }
      return TRUE;

    case ID_ACTION_EVALUATE_TRADE:
      if(AI_Use_The_Force(&UICurrentGameState, UICurrentGameState.CurrentPlayer))
        MessageBox(NULL, "Good trade!", "Evaluate Trade", MB_OK);
      else
        MessageBox(NULL, "Bad trade.", "Evaluate Trade", MB_OK);

      return TRUE;

    case ID_ACTION_BUY:
    case ID_ACTION_SELL:
    case ID_ACTION_MORTGAGE:
      i = DialogBoxParam (hAppInstance, MAKEINTRESOURCE (IDD_SELECT_PLAYER),
        hwndMain, SelectPlayerBoxMsgProcessor, TRUE // Just local humans
            );
      if (i < RULE_MAX_PLAYERS) // If didn't cancel.
      {
        CurrentUIPlayer = i;
        MESS_SendAction (ACTION_PLAYER_BUYSELLMORT, CurrentUIPlayer, RULE_BANK_PLAYER,
            0, 0 // Claim to be working on no properties
                , 0, 0, NULL, 0, NULL);
        DefaultBuySellMortChoice = wmID;
      }
      return TRUE;


    case ID_ACTION_TEXT_CHAT:
      CHAT_Toggle();
//      CreateDialog (hAppInstance, MAKEINTRESOURCE (IDD_TEXT_CHAT),
//        hwndMain, TextChatBoxMsgProcessor);
      return TRUE;


    case ID_ACTION_VOICE_CHAT:
      // Start a voice chat that uses the settings set by the text chat box.

      if (LE_VOICE_RECORD_START (VOICE_CHAT_BUFFER_SIZE))
      {
        CreateDialog (hAppInstance, MAKEINTRESOURCE (IDD_VOICE_CHAT),
          hwndMain, VoiceChatBoxMsgProcessor);
      }
      return TRUE;


    case ID_ACTION_PAUSEGAME:
      GamePaused = !GamePaused;
      MESS_SendAction (ACTION_PAUSE_GAME, AnyLocalPlayer (), RULE_BANK_PLAYER,
        GamePaused, 0, 0, 0, NULL, 0, NULL);
      return TRUE;


    case ID_CHEAT_SETSEED:
      DialogBox (hAppInstance, MAKEINTRESOURCE (IDD_RANDOM_SEED), hwndMain,
      RandomSeedBoxMsgProcessor);
      return TRUE;


    case ID_CHEAT_SETCASH:
      i = DialogBoxParam (hAppInstance, MAKEINTRESOURCE (IDD_SELECT_PLAYER),
        hwndMain, SelectPlayerBoxMsgProcessor, FALSE // Any players ok
            );
      if (i < RULE_MAX_PLAYERS) // If didn't cancel.
      {
        CurrentUIPlayer = i;
        ShowDialogBox (DK_SET_CASH);
        if (hdlgCurrentDialogBox != NULL)
        {
          wcstombs (TempString, UICurrentGameState.Players[CurrentUIPlayer].name,
          sizeof (TempString));
          strcat (TempString, " is Stealing Cash");
          SetWindowText (hdlgCurrentDialogBox, TempString);
        }
      }
      break;


    case ID_CHEAT_SETOWNER:
      i = DialogBoxParam (hAppInstance, MAKEINTRESOURCE (IDD_SELECT_PLAYER),
        hwndMain, SelectPlayerBoxMsgProcessor, FALSE // Any players ok
        );
      if (i < RULE_MAX_PLAYERS) // If didn't cancel.
      {
        CurrentUIPlayer = i;
        ShowDialogBox (DK_SET_OWNER);
        if (hdlgCurrentDialogBox != NULL)
        {
          wcstombs (TempString, UICurrentGameState.Players[CurrentUIPlayer].name,
          sizeof (TempString));
          strcat (TempString, " is Stealing Land");
          SetWindowText (hdlgCurrentDialogBox, TempString);
        }
      }
      return TRUE;


    case ID_OPTIONS_SHOWOWN:
      if (ShowColouredOwnershipOnSquares)
      {
        ShowColouredOwnershipOnSquares = FALSE;
        CheckMenuItem(hmenuMain, ID_OPTIONS_SHOWOWN, MF_UNCHECKED);
        InvalidateRect (hwndMain, &MainBoardRect, FALSE);
      }
      else
      {
        ShowColouredOwnershipOnSquares = TRUE;
        CheckMenuItem(hmenuMain, ID_OPTIONS_SHOWOWN, MF_CHECKED);
        InvalidateRect (hwndMain, &MainBoardRect, FALSE);
      }
      return TRUE;


    case ID_OPTIONS_RUNFAST:
      RunFastWithNoDisplay = !RunFastWithNoDisplay;
      CheckMenuItem (hmenuMain, ID_OPTIONS_RUNFAST,
        RunFastWithNoDisplay ? MF_CHECKED : MF_UNCHECKED);
      InvalidateRect (hwndMain, NULL, FALSE);
      return TRUE;


    case ID_OPTIONS_SHOW_AI_DRONE_INFO:
      AI_Toggle_AI_Info(TRUE);
      return TRUE;


    case ID_OPTIONS_SHOW_AI_INFO:
      AI_Toggle_AI_Info(FALSE);
      return TRUE;


    case ID_OPTIONS_RELOAD_AIS:
      for(i = 0; i < UICurrentGameState.NumberOfPlayers; ++i)
      {
        if(SlotIsALocalAIPlayer[i])
          AI_Load_AI((RULE_PlayerNumber) i, UICurrentGameState.Players[i].token,
          UICurrentGameState.Players[i].AIPlayerLevel);
      }
      return TRUE;


    case ID_OPTIONS_SHOW3DOPENING:
      SIZZPLAY_ShowSizzler (hwndMain);
      SP3DT_ShowScrollingText (hwndMain, SP3DT_AS_OPENING_TEXT);
      return TRUE;


    case ID_NETWORK_DIRECTPLAY:
      if (MESS_DirectPlaySystemActive)
        MESS_StopAllNetworking ();
      else
      {
        MESS_StartNetworking (LANG_GetTextMessage (TMN_ARGS_DIRECTPLAY));
      }
      CheckMenuItem (hmenuMain, ID_NETWORK_DIRECTPLAY,
        MESS_DirectPlaySystemActive ? MF_CHECKED : MF_UNCHECKED);
      CheckMenuItem (hmenuMain, ID_NETWORK_SOCKET,
        MESS_WinSockSystemActive ? MF_CHECKED : MF_UNCHECKED);
      return TRUE;


    case ID_NETWORK_SOCKET:
      if (MESS_WinSockSystemActive)
        MESS_StopAllNetworking ();
      else
      {
        MESS_StartNetworking (LANG_GetTextMessage (TMN_ARGS_SOCKET));
      }
      CheckMenuItem (hmenuMain, ID_NETWORK_DIRECTPLAY,
        MESS_DirectPlaySystemActive ? MF_CHECKED : MF_UNCHECKED);
      CheckMenuItem (hmenuMain, ID_NETWORK_SOCKET,
        MESS_WinSockSystemActive ? MF_CHECKED : MF_UNCHECKED);
      return TRUE;


    case ID_NETWORK_SHOW_ADDRESSES:
      ShowNetworkAddresses ();
      return TRUE;
  }
  return FALSE;
}*/



/*****************************************************************************
 * Process a left mouse button press. The current version will check to see
 * where on the board the user clicked. If the user clicked on a square on
 * the board or on something in the player status areas the appropriate message
 * will be sent to the current dialog box.
 */

BOOL ProcessLeftMouseButtonPress(UINT mess, POINT MousePoint)
{
//  ClickDetectionZoneRecord Zone;
//  int WhatPlayer = -1;
//  int i;

/*  // If the mouse in inside the main game board, see what square we hit.
  if (PtInRect (&MainBoardRect, MousePoint))
  {
    ClickedWhereOnBoard(
        (MousePoint.x - MainBoardRect.left) / (float) (MainBoardRect.right - MainBoardRect.left),
        (MousePoint.y - MainBoardRect.top) / (float) (MainBoardRect.bottom - MainBoardRect.top),
        &Zone );
  }
  // Otherwise check for a hit somewhere in the player status window.
  else
  {
    for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
    {
      if (PtInRect (&PlayerStatusRects[i], MousePoint))
      {
        ClickedWhereInPlayerStatus(
          (MousePoint.x - PlayerStatusRects[i].left) / (float) (PlayerStatusRects[i].right - PlayerStatusRects[i].left),
          (MousePoint.y - PlayerStatusRects[i].top) / (float) (PlayerStatusRects[i].bottom - PlayerStatusRects[i].top),
          &Zone );

        // Keep track of what player we hit.
        WhatPlayer = i;

        break;
      }
    }
  }

  // First, check inside the player status areas.
  if (WhatPlayer != -1)
  {
    // Check to see if we hit any get out of jail cards.
    if (Zone.chanceJail)
      SendMessage( hdlgCurrentDialogBox, WM_CLICKED_ON_GETOUTOFJAIL, (WPARAM)WhatPlayer, (LPARAM)0 );
    else if (Zone.communityJail)
      SendMessage( hdlgCurrentDialogBox, WM_CLICKED_ON_GETOUTOFJAIL, (WPARAM)WhatPlayer, (LPARAM)1 );
    // Check to see if we hit the cash bar.
    else if (Zone.cash)
      SendMessage( hdlgCurrentDialogBox, WM_CLICKED_ON_CASH, (WPARAM)WhatPlayer, (LPARAM)0 );
    // We hit a valid deed card.
    else if (Zone.square < SQ_OFF_BOARD)
      SendMessage( hdlgCurrentDialogBox, WM_CLICKED_ON_SQUARE, (WPARAM)WhatPlayer, (LPARAM)Zone.square);
    // Otherwise we hit somewhere in the status.
    else
      SendMessage( hdlgCurrentDialogBox, WM_CLICKED_ON_PLAYER_STATUS, (WPARAM)WhatPlayer, (LPARAM)0);
  }
  // If not in player status, clicked somewhere on main board.
  else if (Zone.square < SQ_OFF_BOARD)
    SendMessage( hdlgCurrentDialogBox, WM_CLICKED_ON_SQUARE, (WPARAM)CurrentUIPlayer, (LPARAM)Zone.square);
  else
    return FALSE;

*/  // Processed this message.
  return(TRUE);
}



/*****************************************************************************
 * Respond to a NOTIFY_ARE_YOU_THERE with appropriate ACTION_I_AM_HEREs.
 */

static void Process_NOTIFY_ARE_YOU_THERE (RULE_ActionArgumentsPointer NewMessage)
{
  RULE_PlayerNumber PlayerNo;

  for (PlayerNo = 0; PlayerNo < UICurrentGameState.NumberOfPlayers; PlayerNo++)
  {
    if (SlotIsALocalPlayer [PlayerNo] && (NewMessage->numberA & (1 << PlayerNo)))
    {
      MESS_SendAction (ACTION_I_AM_HERE, PlayerNo, RULE_BANK_PLAYER,
        NewMessage->numberB, /* Serial number */
        0, 0, 0, NULL, 0, NULL);
    }
  }
}



/*****************************************************************************
 * Got a new game message, do any actions needed.
 */

void ProcessMessageToPlayer( RULE_ActionArgumentsPointer NewMessage )
{
  int               i;
  FILE* fileptr;
  // Most messages are dispatched to sub UD modules.  Since they have a switch
  // statement all their own, you could argue it is inefficient to check twice.
  // However, this is not a time critical function AND I can't think of a better
  // way to make this a multi-file function to facilitate multiple programmers (RK).
  extern BOOL last_AI_ButtonRemoteState;

  // Setup needs to know if the game has already started on another machine,
  // in which case it will join the player as a spectator.
  if ( RULE_ActionIsOfMainGameScreenPromptType( NewMessage->action) )
    UDPSEL_ProcessMessageToPlayer( NewMessage );


  switch (NewMessage->action)
  {
  case NOTIFY_ERROR_MESSAGE:
    if( NewMessage->numberA == 71 )// Hack - just give me a sound
    { // Host left
      //|>NOTIFY_ERROR_MESSAGE (80)................... From: THE BANK        To: ALL/NO PLAYER(S) 
      //|>[A:  71] [B:   0] [C:   6] [D:   0] [E:   0] 
      //|>Message: The Banke Bank
      // This one, client left
      //|>NOTIFY_ERROR_MESSAGE (80)................... From: THE BANK        To: ALL/NO PLAYER(S) 
      //|>[A:  71] [B:   0] [C:   1] [D:   0] [E:   0] 
      //|>Message: gafield has been dropped from the game and replaced by an AI.

      if( NewMessage->numberC == 6 )
        UDSOUND_Warning();
      else
        UDPENNY_PennybagsGeneral( PB_HumanReplacedByComputerPlayer, Sound_WaitForAnyOldSoundThenPlay );
    }
    CheckForAcceptingOurNewPlayer (NewMessage);
    DisplayErrorNotification (NewMessage);
    break;

  case NOTIFY_CLIENT_RESYNC_INFO:
    ProcessResyncInfo (NewMessage);
    ///SetUpLoadedGame(); - nope, we resync lots (doh)!
    // Get the resync when joining a network game, or when another
    // computer joins our game.  A good place to see if chat should
    // be turned on.
#if CE_ARTLIB_SoundEnableVoiceChat
    USERIFCE_StartVoiceChat ();
#endif // CE_ARTLIB_SoundEnableVoiceChat
    break;

  case NOTIFY_GAME_STARTING:
    // WARNING: SetUpLoadedGame has related operations, update both.

    // This is a key point - PSelect stops and the game (IBar mostly) starts.
    // FIXME - this is very time consuming - we might need a special display OR to call display updates ourselves!
    // The computer will appear locked while generating deeds, not even the mouse will move.
#if !USA_VERSION
    {LE_FONTS_SetSettings(0);
    for( int t = 0, s = 0; t < 40; t++ )
    {// Generate a deed set
      if( DISPLAY_propertyToOwnablePropertyConversion( t ) >= 0 )
      {
        if( UDIBAR_Deeds[s]!= LED_EI )
          LE_DATA_FreeRuntimeDataID(UDIBAR_Deeds[s]);
        UDIBAR_Deeds[s] = UDPENNY_CreateDeed(t, -1, -1, -1, TRUE );
        if( UDIBAR_Deeds[28 + s]!= LED_EI )
          LE_DATA_FreeRuntimeDataID(UDIBAR_Deeds[28 + s]);
        UDIBAR_Deeds[28 + s++] = UDPENNY_CreateDeed(t, -1, -1, -1, FALSE);
      }
    }
    LE_FONTS_GetSettings(0);}
#endif

    // Start up voice chat if it isn't already going and this is
    // a network game.
#if CE_ARTLIB_SoundEnableVoiceChat
    USERIFCE_StartVoiceChat ();
#endif // CE_ARTLIB_SoundEnableVoiceChat

    // Do any player select stuff that needs to be done as the game starts.
    UDPSEL_GameHasJustStarted();

    UDBOARD_SetBackdrop( DISPLAY_SCREEN_MainA );

    UDIBAR_setDialog( DISPLAY_Dialog_None );
    DISPLAY_state.IBarStateTrackOn = TRUE; // Force IBar to show 'current' player
    //UDPENNY_StartGame(); // Not the right place for this.
    for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
    {
      UICurrentGameState.CountHits[i].tradedItem = FALSE;
      UICurrentGameState.CountHits[i].toPlayer = RULE_NOBODY_PLAYER;
    }
    // place bank history stuff here
    // reset the file acchist.txt
    SaveAccountInfo.player_number = -1;
    SaveAccountInfo.turn_number = 0;
    SaveAccountInfo.turn_description[0] = 0;
    fileptr = fopen( "acchist.txt", "wb" );
    if (fileptr != NULL)
    {
        fclose( fileptr );
    }
    // reset the card globals
    g_Card200Counter = g_Card50Counter = 0;

    // Put all tokens on resting idle positions.
    for( i = 0; i < UICurrentGameState.NumberOfPlayers; i++ )
    {
      DISPLAY_state.Player3DTokenIdlePos[0][i] = UICurrentGameState.NumberOfPlayers - i -1; // Spread em around in reverse order (so the 'deepest' come out first)
    }
    // Reset or initialize victory animation
    DISPLAY_state.IsVictoryAnim = FALSE;
    DISPLAY_state.isVictoryGameLockReleased = TRUE;

    // set the game is starting flag to be used with the options screen
    DISPLAY_state.IsOptionGameStarted = TRUE;
    // player sort is over.
    last_AI_ButtonRemoteState = !SlotIsALocalPlayer[0];

    break;

  case NOTIFY_NEXT_MOVE:
    // Shut down any cash message
    //DISPLAY_state.ButtonBarWantedTime = 0;

    // Just after loading a game, the view is probably wrong.  We have to be fairly sure the game was just loaded (by checking for reasonable game screens)
    /*if( !DISPLAY_IsIBarVisible //&&
      //(DISPLAY_state.desired2DView != DISPLAY_SCREEN_PselectA)
      )
    {
      DISPLAY_state.desired2DCamera = 0; // FIXME - for demo - probably not the best place to do this.
      UDBOARD_SetBackdrop( DISPLAY_SCREEN_MainA );
      DISPLAY_showAll2();  // Force a screen update before PB gets mouthy.
    }*/
    break;

  case NOTIFY_GAME_STATE_FOR_SAVE:
  case NOTIFY_OPTIONS_FOR_SAVE:
    if ( SaveGameOrOptions(NewMessage) )
    {
        //ShowCursor( TRUE );
        //MessageBox( NULL, "Game saved in file move.msv.  Use load game option to restart.", "Game Saved", MB_OK );
        //ShowCursor( FALSE );
    }
    break;

#if CE_ARTLIB_SoundEnableVoiceChat
  case NOTIFY_VOICE_CHAT:
    if (NewMessage->numberA >= RULE_MAX_PLAYERS ||
    SlotIsALocalPlayer [NewMessage->numberA])
    {
      ReceiveVoiceChat (NewMessage);
    }
    break;
#endif // CE_ARTLIB_SoundEnableVoiceChat


  case NOTIFY_TEXT_CHAT:
    if (NewMessage->numberA >= RULE_MAX_PLAYERS ||
    SlotIsALocalPlayer [NewMessage->numberA])
    {
      /* This text chat message is for us or our AI players.  If the Blob
         exists, it contains unicode text.  Otherwise this is a canned
         message, identified by NumberC. */

      CHAT_ReceiveMessage((ACHAR *)NewMessage->binaryDataA,
        NewMessage->numberC, NewMessage->numberB, NewMessage->numberA);
    }
    break;

  case NOTIFY_GAME_OVER:
    if( GameInProgress )
    {// We usually get this message a few times
      GameInProgress = FALSE;
      SetCurrentUIPlayerFromPlayerSet ((RULE_PlayerSet) -1 /* Any local human */);
      UDPENNY_PennybagsGeneral( PB_PlayAgain, Sound_WaitForAnyOldSoundThenPlay );

      UDIBAR_setIBarRulesState( IBAR_STATE_GameOver, NewMessage->numberA );
      UDBOARD_SetBackdrop( DISPLAY_SCREEN_MainA ); // Other screens can die during victory animation.


      // Update hi-score info in ini file.  Unfortunately this gets called for
      // every player irregardless whether they are the winner or not.
      UDPSEL_GameOver( UICurrentGameState.Players[NewMessage->numberA].name, (int)NewMessage->numberB,
       UICurrentGameState.Players[NewMessage->numberA].AIPlayerLevel );
      DISPLAY_showAll;
    }
    break;

  case NOTIFY_GAME_PAUSED:
    GamePaused = TRUE;
    break;

  case NOTIFY_ARE_YOU_THERE:
    if ( NewMessage->numberC == NOTIFY_NEW_HIGH_BID )
    {
      // Doing roll-call before starting an auction.  Make sure everyone has loaded
      // all their auction graphics etc.
      UDAUCT_Process_NOTIFY_ARE_YOU_THERE( NewMessage );
    }
    else
    {
      Process_NOTIFY_ARE_YOU_THERE( NewMessage );
    }
    break;

  case NOTIFY_ACTION_COMPLETED:
    // IBar uses these for any engine-related button clicks (ie, Roll is engine related, camera is not)
    UDIBAR_ActionCompleted( NewMessage );
    if ((NewMessage->numberA == ACTION_NEW_GAME) ||
        (NewMessage->numberA == ACTION_START_TRADE_EDITING)) // New game, go to PSelect.  This is fired on startup.
    { // This might justify a subroutine all its own eventually.
      UDTRADE_ProcessMessageToPlayer( NewMessage );
    }
    // Setup needs to know if the host has pressed the start button so that it
    // can move non-hosts over to the rules screen.
    if ( (NewMessage->numberA == ACTION_START_GAME) && NewMessage->numberB )
      UDPSEL_ProcessMessageToPlayer( NewMessage );
    break;

  case NOTIFY_NAME_PLAYER:
  case NOTIFY_ADD_LOCAL_PLAYER:
  case NOTIFY_PLAYER_DELETED:
  case NOTIFY_PLEASE_ADD_PLAYERS:
  case NOTIFY_NUMBER_OF_PLAYERS:
  case NOTIFY_PROPOSED_CONFIGURATION:
    DISPLAY_state.PropertyBuyAuctionDesired = LED_EI;
    UDPSEL_ProcessMessageToPlayer( NewMessage );
    break;

  case NOTIFY_AUCTION_GOING:
  case NOTIFY_NEW_HIGH_BID:
    DISPLAY_state.PropertyBuyAuctionDesired = LED_EI;
    UDAUCT_ProcessMessageToPlayer( NewMessage );
    break;

  // Special case:  two pieces of code need to process this message.
  case NOTIFY_PLEASE_ROLL_DICE:
    DISPLAY_state.PropertyBuyAuctionDesired = LED_EI;
    UDIBAR_ProcessMessageToPlayer( NewMessage );
    break;

  case ACTION_LANDED_ON_SQUARE://Special.  Used for sound
  case NOTIFY_END_TURN:
  case NOTIFY_START_TURN:
  case NOTIFY_DICE_ROLLED:
  case NOTIFY_BUY_OR_AUCTION_DECISION:
  case NOTIFY_MOVE_FORWARDS:
  case NOTIFY_MOVE_BACKWARDS:
  case NOTIFY_JUMP_TO_SQUARE:
  case NOTIFY_CASH_AMOUNT:
  case NOTIFY_PASSED_GO:
  case NOTIFY_PLEASE_PAY:
  case NOTIFY_JAIL_EXIT_CHOICE:
  case NOTIFY_PICKED_UP_CARD:
  case NOTIFY_PUT_AWAY_CARD:
  case NOTIFY_SQUARE_OWNERSHIP:
  case NOTIFY_SQUARE_MORTGAGE:
  case NOTIFY_FREE_UNMORTGAGING:
  case NOTIFY_SQUARE_HOUSES:
  case NOTIFY_JAIL_CARD_OWNERSHIP:
  case NOTIFY_HOUSING_SHORTAGE:
  case NOTIFY_FLAT_OR_FRACTION_TAX_DECISION:
  case NOTIFY_PLACE_BUILDING:
  case NOTIFY_DECOMPOSE_SALE:
  case NOTIFY_PLAYER_BUYSELLMORT:
  case NOTIFY_IMMUNITY_COUNT:
  case NOTIFY_FUTURE_RENT_COUNT:
  case NOTIFY_FREE_PARKING_POT:
  case NOTIFY_CASH_ANIMATION:
    DISPLAY_state.PropertyBuyAuctionDesired = LED_EI;
    UDIBAR_ProcessMessageToPlayer( NewMessage );
    break;

  case NOTIFY_TRADE_FINISHED:
    DISPLAY_state.PropertyBuyAuctionDesired = LED_EI;
    UDTRADE_ProcessMessageToPlayer( NewMessage );
    EndTradeSequence();
    break;

  case NOTIFY_TRADE_STARTED:
  case NOTIFY_TRADE_EDITOR:
  case NOTIFY_TRADE_ACCEPTANCE_DECISION:
  case NOTIFY_TRADE_ITEM:
    DISPLAY_state.PropertyBuyAuctionDesired = LED_EI;
    UDTRADE_ProcessMessageToPlayer( NewMessage );
    break;

  default:
    break;
  }
}



/*****************************************************************************
 * This function is called periodically by the main game loop.  Process the
 * various things needed to keep the user interface active.
 */

void ProcessPlayersUI (RULE_ActionArgumentsPointer NewMessage)
{
  /* Process the message. */

  if (NewMessage != NULL)
    ProcessMessageToPlayer (NewMessage);

  DoTokenBlinking ();
}



/*****************************************************************************
 * Deallocate things which were allocated.
 */

void CleanUpUserInterface (void)
{
  /* Close the optional dice roll history file. */

#if SAVE_DICE_ROLLS_IN_A_FILE
  if (DiceRollFile != NULL)
  {
    fprintf (DiceRollFile, "Number of rolls: %ld\n", NumberOfRollsMade);
    fprintf (DiceRollFile, "Number of doubles: %ld, average: %f\n", NumberOfDoublesMade, (double) NumberOfDoublesMade / NumberOfRollsMade);
    fprintf (DiceRollFile, "Die 1 total: %ld, average: %f\n", TotalDie1, (double) TotalDie1 / NumberOfRollsMade);
    fprintf (DiceRollFile, "Die 2 total: %ld, average: %f\n", TotalDie2, (double) TotalDie2 / NumberOfRollsMade);
    fprintf (DiceRollFile, "Dice total: %ld, average: %f\n", TotalDice, (double) TotalDice / NumberOfRollsMade);
    fclose (DiceRollFile);
    DiceRollFile = NULL;
    TotalDie1 = 0;
    TotalDie2 = 0;
    TotalDice = 0;
    NumberOfRollsMade = 0;
    NumberOfDoublesMade = 0;
  }
#endif /* SAVE_DICE_ROLLS_IN_A_FILE */
}


/*****************************************************************************
 * Do what must be done after loading a game, some similarity to NOTIFY_GAME_STARTING
 */
void SetUpLoadedGame(void)
{
  int   i;
  FILE* fileptr;
  extern BOOL last_AI_ButtonRemoteState;

  GameInProgress = TRUE; // We may not start with a roll_dice to set this for us.
#if !USA_VERSION
    if(SaveGameStuff[CurrentGameFile].city < 0)
    {
        //DISPLAY_state.city = iLangId;// Classic board
        DISPLAY_state.city = -1;// Classic board
        DISPLAY_state.system = SaveGameStuff[CurrentGameFile].system;
        // set the custom board if needed and if it exists
        char szFile[MAX_PATH];
        char szPath[MAX_PATH];

        // Format the path 
        sprintf(szFile, "%s%s\\%s", SaveGameStuff[CurrentGameFile].CustomBoardName,
                "2dboards", "2dview01.bmp");

        // Try to reference the first texture
        if (!LE_DIRINI_ExpandFileName(szFile, szPath, LE_DIRINI_GlobalPathList))
        {
            // if file not found, then custom board must have been deleted by user
            DISPLAY_state.city = iLangId-2;
            strcpy(DISPLAY_state.customBoardPath, "");
        }
        else
        {
            // Okay, we found the file.
            char* szTmp = strrchr(szPath, '\\');
            szTmp[1] = '\0';
    
            strcpy(DISPLAY_state.customBoardPath,
                SaveGameStuff[CurrentGameFile].CustomBoardName);
            // Set the current board and other display state values
            CurrentBoard = LED_IFT(DAT_3D, HMD_board_citymed);
            UDPSEL_PlayerInfo.citySelected = -1;
            #if !USA_VERSION
                UDPSEL_PlayerInfo.currencySelectionIndex = 0;
            #endif
            // Load the 2d Boards
            UDUTILS_Load2DBoardSet();
        }
    }
    else
    {
        DISPLAY_state.city = SaveGameStuff[CurrentGameFile].city;
        DISPLAY_state.system = SaveGameStuff[CurrentGameFile].system;
    }
    
    {LE_FONTS_SetSettings(0);
    for( int t = 0, s = 0; t < 40; t++ )
    {// Generate a deed set
      if( DISPLAY_propertyToOwnablePropertyConversion( t ) >= 0 )
      {
        if( UDIBAR_Deeds[s]!= LED_EI )
          LE_DATA_FreeRuntimeDataID(UDIBAR_Deeds[s]);
        UDIBAR_Deeds[s] = UDPENNY_CreateDeed(t, -1, -1, -1, TRUE );
        if( UDIBAR_Deeds[28 + s]!= LED_EI )
          LE_DATA_FreeRuntimeDataID(UDIBAR_Deeds[28 + s]);
        UDIBAR_Deeds[28 + s++] = UDPENNY_CreateDeed(t, -1, -1, -1, FALSE);
      }
    }
    LE_FONTS_GetSettings(0);}
#else
    // now set the city and system
  if(SaveGameStuff[CurrentGameFile].city < 0)
  {
    DISPLAY_state.city = -1; // custom board
    // set the custom board if needed and if it exists
    char szFile[MAX_PATH];
    char szPath[MAX_PATH];

    // Format the path 
    sprintf(szFile, "%s%s\\%s", SaveGameStuff[CurrentGameFile].CustomBoardName,
                "2dboards", "2dview01.bmp");

    // Try to reference the first texture
    if (!LE_DIRINI_ExpandFileName(szFile, szPath, LE_DIRINI_GlobalPathList))
    {
        // if file not found, then custom board must have been deleted by user
        DISPLAY_state.city = 0;
        strcpy(DISPLAY_state.customBoardPath, "");
    }
    else
    {
        // Okay, we found the file.
        char* szTmp = strrchr(szPath, '\\');
        szTmp[1] = '\0';
    
        strcpy(DISPLAY_state.customBoardPath,
            SaveGameStuff[CurrentGameFile].CustomBoardName);
        // Set the current board and other display state values
        CurrentBoard = LED_IFT(DAT_3D, HMD_board_citymed);
        UDPSEL_PlayerInfo.citySelected = -1;
        #if !USA_VERSION
            UDPSEL_PlayerInfo.currencySelectionIndex = 0;
        #endif
        // Load the 2d Boards
        UDUTILS_Load2DBoardSet();
    }
  }
  else
    DISPLAY_state.city = SaveGameStuff[CurrentGameFile].city;
  
   DISPLAY_state.system = NOTW_MONA_US;
#endif

   

    // Start up voice chat if it isn't already going and this is
    // a network game.
#if CE_ARTLIB_SoundEnableVoiceChat
    USERIFCE_StartVoiceChat ();
#endif // CE_ARTLIB_SoundEnableVoiceChat

    // Do any player select stuff that needs to be done as the game starts.
    UDPSEL_GameHasJustStarted();

    UDBOARD_SetBackdrop( DISPLAY_SCREEN_MainA );

    DISPLAY_state.IBarStateTrackOn = TRUE; // Force IBar to show 'current' player
    //UDPENNY_StartGame(); // Not the right place for this.
    for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
    {
      UICurrentGameState.CountHits[i].tradedItem = FALSE;
      UICurrentGameState.CountHits[i].toPlayer = RULE_NOBODY_PLAYER;
    }
    // place bank history stuff here
    // reset the file acchist.txt
    SaveAccountInfo.player_number = -1;
    SaveAccountInfo.turn_number = 0;
    SaveAccountInfo.turn_description[0] = 0;
    fileptr = fopen( "acchist.txt", "wb" );
    if (fileptr != NULL)
    {
        fclose( fileptr );
    }
    // reset the card globals
    g_Card200Counter = g_Card50Counter = 0;

    // Reset or initialize victory animation
    DISPLAY_state.IsVictoryAnim = FALSE;
    DISPLAY_state.isVictoryGameLockReleased = TRUE;

    // set the game is starting flag to be used with the options screen
    DISPLAY_state.IsOptionGameStarted = TRUE;

    // IBAR state may not have been set yet, so all this setup is a bit iffy.
    UDPIECES_StartOfGameSetup();

    DISPLAY_state.CurrentPlayerInCenterIdle = -1;//DISPLAY_state.IBarLastRulePlayer; hope that works
    last_AI_ButtonRemoteState = !SlotIsALocalPlayer[DISPLAY_state.IBarLastRulePlayer%6];// May not be set
    UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, 0 ); // Clear the bar - we are expecing an IBar change to fix this.
    //DISPLAY_state.desired2DCamera = UDPIECES_PickCameraFor3Squares(UICurrentGameState.Players[DISPLAY_state.IBarLastRulePlayer%6].currentSquare);
    DISPLAY_state.IBarGameJustLoaded = TRUE;    // This will do a proper player init next ibar change.

}
