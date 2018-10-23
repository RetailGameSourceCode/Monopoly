/*************************************************************
 *
 *   FILE:             RULE.C
 *
 *   (C) Copyright 97  Artech Digital Entertainments.
 *                     All rights reserved.
 *
 * $Header: /Projects - 1999/Mono 2/Rule.cpp 56    10/20/99 1:16p Davide $
 *
 * $Log: /Projects - 1999/Mono 2/Rule.cpp $
 * 
 * 56    10/20/99 1:16p Davide
 * when playing on a custom board, the DISPLAY_state.city flag is set to
 * -1. This greatly affected the display of deeds, cards and sound files
 * for versions other than USA.  A small check is now made. If
 * DISPLAY_state.city is -1, a temp variable is used instead and it is set
 * to iLandId-2.
 * 
 * 55    10/19/99 1:03p Russellk
 * Modified so loading a game will set the next dice roll to 0 indicating
 * reroll the dice
 * 
 * 54    10/07/99 5:31p Philla
 * host/ai bug last one???
 * 
 * 53    10/05/99 3:33p Philla
 * New function for detecting "prompt" message type.
 * 
 * 52    29/09/99 1:43p Agmsmith
 * Now send a 4 byte unique network ID of the original sender along with
 * every chat message, so you can determine the sending computer (not just
 * player).
 *
 * 51    28/09/99 2:26p Agmsmith
 * Avoid array overflow when sending game status to the lobby.
 *
 * 50    99/09/28 2:21p Timp
 * No more extern
 *
 * 49    23/09/99 4:25p Agmsmith
 * Only option when failed last roll for getting out of jail is to pay
 * cash, exclude the get out of jail card.
 *
 * 48    23/09/99 2:21p Agmsmith
 * Don't have a housing shortage prompt when only one player can buy,
 * since the text for notifying the other players that one player is
 * getting the last few houses doesn't exist for Monopoly 2000.
 *
 * 47    23/09/99 1:06p Agmsmith
 * Changed get out of jail rules to match the official rules - roll up to
 * 3 times and if you fail to get doubles on the last roll, it will ask
 * you again if you want to pay or use the card.
 *
 * 46    99/09/22 7:36p Timp
 * Fixed accept/reject sounds.
 *
 * 45    9/22/99 5:24p Davide
 * modified the notifyplayer message in the actionsetgamestate function
 * to pass an extra parameter that will determine if the game is
 * restarting
 * because of a loaded game or a new game
 *
 * 44    22/09/99 4:55p Agmsmith
 * Nothing.
 *
 * 43    22/09/99 4:24p Agmsmith
 * Get it working for Hotseat again.
 *
 * 42    9/22/99 3:50p Philla
 * Auction timing at beginning fixed.  Waits for everyone now before
 * the "going once" is issued.
 *
 * 41    9/21/99 6:26p Philla
 * Fixed having to click more than once on okay button in rules screen.
 *
 * 40    99/09/20 1:45p Timp
 * Fixed problem with garbage deed text.
 *
 * 39    9/17/99 4:11p Philla
 * Standard Rules button works in non-network and network mode.
 *
 * 38    99/09/17 1:05p Timp
 * Changed f/i handling.  (from player isn't checked)
 *
 * 37    9/12/99 1:20p Russellk
 * Set defaults correctly - cheats on only if hotkeys is defined as true.
 *
 * 36    9/08/99 3:39p Russellk
 *
 * 35    99/09/08 2:51p Agmsmith
 * Fix up pre-approval of config data to use network mode.
 *
 * 34    9/08/99 2:01p Philla
 * Fixed deadlock.  Can't okay rules and move on to game.
 *
 * 33    9/07/99 3:48p Philla
 * Live update of rules screen for non-host players.
 *
 * 32    99/09/02 5:23p Agmsmith
 * Let the lobby know the game status at appropriate times.
 *
 * 31    99/09/01 7:11p Agmsmith
 * Sending game status to the lobby under construction.
 *
 * 30    99/09/01 4:11p Agmsmith
 * Get Hotseat working again.
 *
 * 29    99/09/01 2:15p Timp
 * Support property name text for multiple cities
 *
 * 28    8/25/99 4:08p Philla
 * Latest version of Setup stuff.  New graphics.  Host only rules for
 * network...
 *
 * 27    8/12/99 9:20a Russellk
 * if defs used for Hotseat compatibility
 *
 * 26    8/09/99 12:11p Mikeh
 * Added function RULE_PlayersFirstMoveWasMade(int player) to
 * return players firstmove status.
 *
 * 25    7/21/99 10:16a Davide
 *
 * 24    7/16/99 3:15p Davide
 *
 * 23    7/05/99 3:26p Russellk
 * Adjusted to issue a phase refresh for collection phases using quicky
 * mortgage/sell feature (since with a BSSM lock it was automatic) to
 * allow detection that enough cash has been raised - take da money.
 *
 * 22    7/05/99 11:22a Russellk
 *
 * 21    7/02/99 6:01p Russellk
 * First pass alterations for BSSM state - numberD flags using a new
 * quicky mode (no lock required, works despite an existing lock).  Our
 * new interface is clunky with the old methods - repeatedly denying
 * requests - this method will correct that and let us leave the AI code
 * alone.
 *
 * 20    6/24/99 9:55a Davide
 *
 * 19    6/23/99 10:36a Davide
 * added a function call to reset the variables for the status screen when
 * restarting a game
 *
 * 18    6/22/99 11:16a Davide
 *
 * 17    6/21/99 3:52p Davide
 * added a member to the RULE_SquareInfoStruct to keep tabs on the total
 * game earnings for each square. The variable is incremented every time a
 * player lands on a square (CollectRent function)
 *
 * 16    5/27/99 3:31p Russellk
 * Removed old variable which made trades send only once - we only send
 * once now anyway, though AI's seem to have trouble now.  If so, they
 * need to be fixed as well - they seemed to send a trade edit done prior
 * to sending the trade items which might have worked with duplicates
 * being sent.
 *
 * 15    5/27/99 3:14p Russellk
 *
 * 14    5/26/99 8:44a Russellk
 * Offline trading operational, though some more testing would be
 * valuable.
 *
 * 13    5/21/99 3:55p Russellk
 * All initial trade function changed to non-locking
 *
 * 9     3/25/99 2:05p Russellk
 * OTIFY_FREE_UNMORTGAGING was sending messages with no property sets.
 * Fixed.
 *
 * 5     2/08/99 2:08p Russellk
 * Added DONE TURN support.
 *
 * 4     2/08/99 9:11a Russellk
 * Added state NOTIFY_TURN_END - a rather stupid end of turn
 * acknowledgement.  Useful for email monopoly, Hasbro wants it in at this
 * point.
 *
 * 3     1/28/99 4:07p Russellk
 * No restrictions on when new players join during phases
 *
 * 2     1/27/99 10:59a Russellk
 * Basic Mono2 display and user interface operational - no trade editor
 * nor game config editory.
 *
 * 1     1/08/99 2:06p Russellk
 *
 * 1     1/08/99 11:35a Russellk
 * Graphics
 *
 * 6     1/04/99 1:20p Russellk
 * Include file change for gameinc.h
 *
 * 5     1/04/99 11:39a Russellk
 * Game state restore added to log, excessive immunity notification at
 * player bankrupt fixed (Alex)
 *
 * 4     12/24/98 11:41a Russellk
 * Don't send useless immunity changes during bankruptcy.
 *
 * 3     12/18/98 10:03a Russellk
 * Modified property array comments to Monopoly names
 *
 * 2     12/17/98 4:27p Russellk
 * Obsolete comments removed for new version.  Log file facility entered
 * in Mess.c to record phase and action/error message activity.
 *
 ************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/
#include "gameinc.h"


/************************************************************/

/************************************************************/
/* COMPILATION CONTROL SWITCHES                             */
/************************************************************/
#if FORHOTSEAT
  #define RULE_REPORT_BAD_MESSAGES          0
  #define RULE_SOMEBODY_WINS_AUCTION_HACK   0
  #define RULE_MAX_BID_99999_HACK           0
  #define RULE_LOADSAVE_GAME_ONLY_WHEN_SAFE 0
  #define RULE_TAKE_OVER_AI_ONLY_WHEN_SAFE  0
  #define RULE_NO_SPECTATORS_ALLOWED        0
  #define RULE_TIME_LIMITED_DEMO            0
  #define RULE_DEMO_TIME_LIMIT_SECONDS      (5 * 60)
  #define RULE_PLAYERSVOTEONRULES           0
#else /* Monopoly PC */
  #define RULE_REPORT_BAD_MESSAGES          (LE_CMAIN_EnableDebugMode && 1)
  #define RULE_SOMEBODY_WINS_AUCTION_HACK   0
  #define RULE_MAX_BID_99999_HACK           0
  #define RULE_LOADSAVE_GAME_ONLY_WHEN_SAFE 0
  #define RULE_TAKE_OVER_AI_ONLY_WHEN_SAFE  1
  #define RULE_NO_SPECTATORS_ALLOWED        0
  #define RULE_TIME_LIMITED_DEMO            FLOC_CompileAsDemo
  #define RULE_DEMO_TIME_LIMIT_SECONDS      (1 * 60)
  #define RULE_PLAYERSVOTEONRULES           0
#endif

/************************************************************/

/************************************************************/
/* STRUCTURES                                               */
/************************************************************/
/************************************************************/

/************************************************************/
/* PROTOTYPES                                               */
/************************************************************/
/************************************************************/

/************************************************************/
/* GLOBALS                                                  */
/************************************************************/


RULE_PropertySet RULE_PropertyToBitsetArray[SQ_MAX_SQUARE_TYPES] =
{
  0,        // SQ_GO
  (1<<0),   // SQ_MEDITERRANEAN_AVENUE
  0,        // SQ_COMMUNITY_CHEST_1
  (1<<1),   // SQ_BALTIC_AVENUE
  0,        // SQ_INCOME_TAX
  (1<<2),   // SQ_READING_RR
  (1<<3),   // SQ_ORIENTAL_AVENUE
  0,        // SQ_CHANCE_1
  (1<<4),   // SQ_VERMONT_AVENUE
  (1<<5),   // SQ_CONNECTICUT_AVENUE
  0,        // SQ_JUST_VISITING
  (1<<6),   // SQ_ST_CHARLES_PLACE
  (1<<7),   // SQ_ELECTRIC_COMPANY
  (1<<8),   // SQ_STATES_AVENUE
  (1<<9),   // SQ_VIRGINIA_AVENUE
  (1<<10),  // SQ_PENNSYLVANIA_RR
  (1<<11),  // SQ_ST_JAMES_PLACE
  0,        // SQ_COMMUNITY_CHEST_2
  (1<<12),  // SQ_TENNESSEE_AVENUE
  (1<<13),  // SQ_NEW_YORK_AVENUE
  0,        // SQ_FREE_PARKING
  (1<<14),  // SQ_KENTUCKY_AVENUE
  0,        // SQ_CHANCE_2
  (1<<15),  // SQ_INDIANA_AVENUE
  (1<<16),  // SQ_ILLINOIS_AVENUE
  (1<<17),  // SQ_BnO_RR
  (1<<18),  // SQ_ATLANTIC_AVENUE
  (1<<19),  // SQ_VENTNOR_AVENUE
  (1<<20),  // SQ_WATER_WORKS
  (1<<21),  // SQ_MARVIN_GARDENS
  0,        // SQ_GO_TO_JAIL
  (1<<22),  // SQ_PACIFIC_AVENUE
  (1<<23),  // SQ_NORTH_CAROLINA_AVENUE
  0,        // SQ_COMMUNITY_CHEST_3
  (1<<24),  // SQ_PENNSYLVANIA_AVENUE
  (1<<25),  // SQ_SHORT_LINE_RR
  0,        // SQ_CHANCE_3
  (1<<26),  // SQ_PARK_PLACE
  0,        // SQ_LUXURY_TAX
  (1<<27),  // SQ_BOARDWALK
  0,        // SQ_IN_JAIL
  0,        // SQ_OFF_BOARD
};
  /* This array assigns a bit number to every property that can be owned. These bits will
  in turn be used to identify properties in sets. Two routines defined below will allow
  conversion between a PropertySet bit to a SquareType and vice versa. */



RULE_SquarePredefinedInfoRecord RULE_SquarePredefinedInfo [SQ_MAX_SQUARE_TYPES] =
{
  /* Purchase cost,
          Mortgage cost,
                   Rent table for 0 to 5 houses, note that 4 and 5 get swapped in short game.  Railroads watch out!
                                                      House purchase cost
                                                           Square name
                                                                 Group */
  // GO
  {   0,   0, {    0,    0,    0,    0,    0,    0 },   0, NULL, SG_MISCELLANEOUS },

  // SG_MEDITERRANEAN_AVENUE
  // SQ_MEDITERRANEAN_AVENUE
  {  60,  30, {    2,   10,   30,   90,  160,  250 },  50, NULL, SG_MEDITERRANEAN_AVENUE },
  // SQ_COMMUNITY_CHEST_1
  {   0,   0, {    0,    0,    0,    0,    0,    0 },  50, NULL, SG_COMMUNITY_CHEST },
  // SQ_BALTIC_AVENUE
  {  60,  30, {    4,   20,   60,  180,  320,  450 },  50, NULL, SG_MEDITERRANEAN_AVENUE },

  // SQ_INCOME_TAX.
  {   0,   0, {    0,    0,    0,    0,    0,    0 },   0, NULL, SG_MISCELLANEOUS },

  // SQ_READING_RR
  { 200, 100, {     0,  25,   50,  100,  200,  200 },   0, NULL, SG_RAILROAD },

  // SG_ORIENTAL_AVENUE
  // SQ_ORIENTAL_AVENUE
  { 100,  50, {    6,   30,   90,  270,  400,  550 },  50, NULL, SG_ORIENTAL_AVENUE },
  // SQ_CHANCE_1
  {   0,   0, {    0,    0,    0,    0,    0,    0 },  50, NULL, SG_CHANCE },
  // SQ_VERMONT_AVENUE
  { 100,  50, {    6,   30,   90,  270,  400,  550 },  50, NULL, SG_ORIENTAL_AVENUE },
  // SQ_CONNECTICUT_AVENUE
  { 120,  60, {    8,   40,  100,  300,  450,  600 },  50, NULL, SG_ORIENTAL_AVENUE },

  // SQ_JUST_VISITING
  {   0,   0, {    0,    0,    0,    0,    0,    0 },   0, NULL, SG_MISCELLANEOUS },

  // SG_ST_CHARLES_PLACE
  // SQ_ST_CHARLES_PLACE
  { 140,  70, {   10,   50,  150,  450,  625,  750 }, 100, NULL, SG_ST_CHARLES_PLACE },
  // SQ_ELECTRIC_COMPANY
  { 150,  75, {    0,    0,    0,    0,    0,    0 },   0, NULL, SG_UTILITY },
  // SQ_STATES_AVENUE
  { 140,  70, {   10,   50,  150,  450,  625,  750 }, 100, NULL, SG_ST_CHARLES_PLACE },
  // SQ_VIRGINIA_AVENUE
  { 160,  80, {   12,   60,  180,  500,  700,  900 }, 100, NULL, SG_ST_CHARLES_PLACE },

  // SQ_PENNSYLVANIA_RR
  { 200, 100, {    0,   25,   50,  100,  200,  200 },   0, NULL, SG_RAILROAD },

  // SG_ST_JAMES_PLACE
  // SQ_ST_JAMES_PLACE
  { 180,  90, {   14,   70,  200,  550,  750,  950 }, 100, NULL, SG_ST_JAMES_PLACE },
  // SQ_COMMUNITY_CHEST_2
  {   0,   0, {    0,    0,    0,    0,    0,    0 },   0, NULL, SG_COMMUNITY_CHEST },
  // SQ_TENNESSEE_AVENUE
  { 180,  90, {   14,   70,  200,  550,  750,  950 }, 100, NULL, SG_ST_JAMES_PLACE },
  // SQ_NEW_YORK_AVENUE
  { 200, 100, {   16,   80,  220,  600,  800, 1000 }, 100, NULL, SG_ST_JAMES_PLACE },

  // SQ_FREE_PARKING
  {   0,   0, {    0,    0,    0,    0,    0,    0 },   0, NULL, SG_MISCELLANEOUS },

  // SG_KENTUCKY_AVENUE
  // SQ_KENTUCKY_AVENUE
  { 220, 110, {   18,   90,  250,  700,  875, 1050 }, 150, NULL, SG_KENTUCKY_AVENUE },
  // SQ_CHANCE_2
  {   0,   0, {    0,    0,    0,    0,    0,    0 },   0, NULL, SG_CHANCE },
  // SQ_INDIANA_AVENUE
  { 220, 110, {   18,   90,  250,  700,  875, 1050 }, 150, NULL, SG_KENTUCKY_AVENUE },
  // SQ_ILLINOIS_AVENUE
  { 240, 120, {   20,  100,  300,  750,  925, 1100 }, 150, NULL, SG_KENTUCKY_AVENUE },

  // SQ_BnO_RR
  { 200, 100, {    0,   25,   50,  100,  200,  200 },   0, NULL, SG_RAILROAD },

  // SG_ATLANTIC_AVENUE
  // SQ_ATLANTIC_AVENUE
  { 260, 130, {   22,  110,  330,  800,  975, 1150 }, 150, NULL, SG_ATLANTIC_AVENUE },
  // SQ_VENTNOR_AVENUE
  { 260, 130, {   22,  110,  330,  800,  975, 1150 }, 150, NULL, SG_ATLANTIC_AVENUE },
  // SQ_WATER_WORKS
  { 150,  75, {    0,    0,    0,    0,    0,    0 },   0, NULL, SG_UTILITY },
  // SQ_MARVIN_GARDENS
  { 280, 140, {   24,  120,  360,  850, 1025, 1200 }, 150, NULL, SG_ATLANTIC_AVENUE },

  // SQ_GO_TO_JAIL
  {   0,   0, {    0,    0,    0,    0,    0,    0 },   0, NULL, SG_MISCELLANEOUS },

  // SG_PACIFIC_AVENUE
  // SQ_PACIFIC_AVENUE
  { 300, 150, {   26,  130,  390,  900, 1100, 1275 }, 200, NULL, SG_PACIFIC_AVENUE },
  // SQ_NORTH_CAROLINA_AVENUE
  { 300, 150, {   26,  130,  390,  900, 1100, 1275 }, 200, NULL, SG_PACIFIC_AVENUE },
  // SQ_COMMUNITY_CHEST_3
  {   0,   0, {    0,    0,    0,    0,    0,    0 },   0, NULL, SG_COMMUNITY_CHEST },
  // SQ_PENNSYLVANIA_AVENUE
  { 320, 160, {   28,  150,  450, 1000, 1200, 1400 }, 200, NULL, SG_PACIFIC_AVENUE },

  // SQ_SHORT_LINE_RR
  { 200, 100, {    0,   25,   50,  100,  200,  200 },   0, NULL, SG_RAILROAD },

  // SQ_CHANCE_3
  {   0,   0, {    0,    0,    0,    0,    0,    0 },   0, NULL, SG_CHANCE },

  // SG_PARK_PLACE
  // SQ_PARK_PLACE
  { 350, 175, {   35,  175,  500, 1100, 1300, 1500 }, 200, NULL, SG_PARK_PLACE },
  // SQ_LUXURY_TAX
  {   0,   0, {    0,    0,    0,    0,    0,    0 },   0, NULL, SG_MISCELLANEOUS },
  // SQ_BOARDWALK
  { 400, 200, {   50,  200,  600, 1400, 1700, 2000 }, 200, NULL, SG_PARK_PLACE },

  // SQ_IN_JAIL
  {   0,   0, {    0,    0,    0,    0,    0,    0 },   0, NULL, SG_MISCELLANEOUS },

  // SQ_OFF_BOARD
  {   0,   0, {    0,    0,    0,    0,    0,    0 },   0, NULL, SG_MISCELLANEOUS }
};
  /* This array is loaded with pretty much constant data.  Only the language
  of the text parts may change, and some other game option related things
  like rent tables for the short game. NOTE: The rent tables and the text
  strings will be changed and loaded respectively when the
  InitialisePredefinedData() function is called. */

static BOOL LastGameWasAShortGame = FALSE;
  /* Keep track of whether or not the last game was a short game. If it
  wasn't and we start a short game, or if it was and we start a regular
  game, we need to swap the fourth house rent and the hotel rent. */


static BOOL IgnoreWaitForEverybodyReady;
  /* If TRUE then the WaitForEverybodyReady function isn't called.
  Useful for avoiding an infinite loop of them. */


static RULE_GameStateRecord CurrentRulesState;
  /* The state that the rules engine is using and changing.  This is a private
  variable so that other code doesn't try to mistakenly use it - it only
  exists on the host computer in network games. */


#define CurrentPhaseM (CurrentRulesState.phaseStack[0].phase)
  /* A quick shortcut for getting the most frequently accessed field,
  which should compile as one instruction since it's a constant
  array index into a global variable.  M for Macro. */


static RULE_GameStateRecord StackedRulesStates [RULE_MAX_PENDING_PHASES];
  /* Each item on the phase stack can have an associated saved game state,
  for undoing actions.  Naturally, the undo stack can't be inside the
  game state because it contains game states.  This structure is actually
  quite large, at about 120K, so it is put after all the other variables
  to avoid thrashing the cache (not that speed matters for rules). */


typedef struct SavedGameDataCollectionStruct
{
  BYTE *AIStateDataPntr;
    /* Points to a GlobalAlloc'd area of memory with the AI's info.
    The AI sends us a RIFF style chunk that gets included in the
    saved game RIFF file.  The length is implied by the DWORD
    starting at byte 4 (the first 4 bytes are the chunk type).
    NULL when the AI hasn't supplied any information yet. */
} SavedGameDataCollectionRecord, *SavedGameDataCollectionPointer;
  /* Contains information that the AI players provide when saving a game. */

static SavedGameDataCollectionRecord CollectedAISaveStates [RULE_MAX_PLAYERS];
  /* When saving a game, we collect data from the AIs and put it in here.
  When we have it from all of them then the saved game is written. */

static int TradeStatus = 2;  // -1: counter  0: reject  1: accept  2: nothing  3: non-involved ai accepted


/*****************************************************************************
 * Set up the values in SquarePredefinedInfo for the current country (use the
 * setlocale function to find the country).  Also initialise other constant
 * tables as appropriate for the given game options (rent tables in particular
 * vary for the short game).  Returns TRUE if successful.
 */

static BOOL InitialisePredefinedData (RULE_GameOptionsPointer GameOptionsPntr)
{
  int i;
  RULE_SquareType TempSquare;
#if !FORHOTSEAT
  int j;
  static int prevcity = -1;
  LE_DATA_DataId item;
#endif

  // First, if we are playing a short game, we have to adjust the rent tables.
  // This is just going through all the squares and switching the fourth house
  // cost with the hotel cost. Also, if the previous game was a short game and
  // we are starting a regular game, we need to switch the two back to
  // their original places.
  if ( (GameOptionsPntr->housesPerHotel == 4 && !LastGameWasAShortGame) ||
       (GameOptionsPntr->housesPerHotel != 4 && LastGameWasAShortGame) )
  {
    for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
    {
      TempSquare = RULE_SquarePredefinedInfo[i].rent[RULE_MAX_HOUSES_PER_SQUARE - 1];
      RULE_SquarePredefinedInfo[i].rent[RULE_MAX_HOUSES_PER_SQUARE - 1] = RULE_SquarePredefinedInfo[i].rent[RULE_MAX_HOUSES_PER_SQUARE];
      RULE_SquarePredefinedInfo[i].rent[RULE_MAX_HOUSES_PER_SQUARE] = TempSquare;
    }
  }
  LastGameWasAShortGame = (GameOptionsPntr->housesPerHotel == 4);


  // Now, we need to read in all the property group names and the individual
  // property names. We read from the Text Database which has all the items
  // in consecutive order for easy reading.  Don't use Clean version since
  // that doesn't have static storage for each string.
#if !FORHOTSEAT
  if (iLangId == NOTW_LANG_NOT_INITIALIZED)  return(FALSE);
  if (prevcity != -1)
    for(i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
    {
      j = TMN_SQUARENAME_GO + (42 * max(prevcity, 0)) + i;
      item = LE_DATA_GetIndexedItemId(LED_IFT(DAT_LANG, 0), j);
      if (item == LED_EI)  return(FALSE);
      RULE_SquarePredefinedInfo[i].squareName = (wchar_t *)LE_DATA_Use(item);

      // If the squarename starts with '*', get the name from the base set.
      // (Although they're not printed, "Chance"/"Community Chest" are like this.
      // Also, this is needed for the US board's Electric Company/Water Works)
      if (RULE_SquarePredefinedInfo[i].squareName[0] == A_T('*')) {
        j = TMN_SQUARENAME_GO + i;
        item = LE_DATA_GetIndexedItemId(LED_IFT(DAT_LANG, 0), j);
        if (item == LED_EI)  return(FALSE);
      }
      if (LE_DATA_GetRef(item))  LE_DATA_RemoveRef(item);
    } /* For i */
#endif

  for(i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
  {
#if FORHOTSEAT
    RULE_SquarePredefinedInfo[i].squareName = LANG_GetTextMessage (TMN_SQUARENAME_GO + i);
#else

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
    item = LE_DATA_GetIndexedItemId(LED_IFT(DAT_LANG, 0), j);
    if (item == LED_EI)  return(FALSE);
    RULE_SquarePredefinedInfo[i].squareName = (wchar_t *)LE_DATA_Use(item);

    // If the squarename starts with '*', get the name from the base set.
    // (Although they're not printed, "Chance"/"Community Chest" are like this.
    // Also, this is needed for the US board's Electric Company/Water Works)
    if (RULE_SquarePredefinedInfo[i].squareName[0] == A_T('*')) {
      j = TMN_SQUARENAME_GO + i;
      item = LE_DATA_GetIndexedItemId(LED_IFT(DAT_LANG, 0), j);
      if (item == LED_EI)  return(FALSE);
      RULE_SquarePredefinedInfo[i].squareName = (wchar_t *)LE_DATA_Use(item);
    }
    LE_DATA_AddRef(item);

//    RULE_SquarePredefinedInfo[i].squareName = LANG_GetTextMessage(TMN_SQUARENAME_GO + (42 * max(DISPLAY_state.city, 0)) + i);
    // If the squarename starts with '*', get the name from the base set.
    // (Although they're not printed, "Chance"/"Community Chest" are like this.
    // Also, this is needed for the US board's Electric Company/Water Works)
//    if (RULE_SquarePredefinedInfo[i].squareName[0] == A_T('*'))
//      RULE_SquarePredefinedInfo[i].squareName = LANG_GetTextMessage(TMN_SQUARENAME_GO + i);
#endif
  } /* For i */

#if !FORHOTSEAT
  prevcity = DISPLAY_state.city;
#endif

  return TRUE;
}



/*****************************************************************************
 * Internal routine. This routine will take a pointer to a card deck and
 * shuffle it. Shuffling is done by going through the deck and for each card,
 * replacing it with another random card from the deck.
 */

static void ShuffleCardDeck(RULE_CardDeckPointer WorkingDeck, int FirstCardOffset)
{
  int i, TempCard, RandomNumber;

  // First, assign the cards in sorted order.
  for(i=0; i<WorkingDeck->cardCount; i++)
    WorkingDeck->cardPile[i] = (unsigned char) (i + FirstCardOffset);

  // Now, shuffle them in a simple fashion. Swap the current card
  // with a random card in the deck.
  for(i=0; i<WorkingDeck->cardCount; i++)
  {
    RandomNumber = rand() % WorkingDeck->cardCount;
    TempCard = WorkingDeck->cardPile[i];
    WorkingDeck->cardPile[i] = WorkingDeck->cardPile[RandomNumber];
    WorkingDeck->cardPile[RandomNumber] = (unsigned char) TempCard;
  }
}



/*****************************************************************************
 * Internal routine. This routine will remove the top card from a passed
 * deck and decrement the card count. All remaining cards will be moved to
 * the top of the deck.
 */

static RULE_CardType DealCardFromTopOfDeck(RULE_CardDeckPointer WorkingDeck)
{
  RULE_CardType  Card;
  int       i;

  // If there are no cards in the deck, error.
  if (WorkingDeck->cardCount == 0)
  {
#if _DEBUG
      DBUG_DisplayNonFatalErrorMessage ("No more cards in deck!");
#endif
      return NOT_A_CARD;
  }

  // Get the top card off the deck.
  Card = WorkingDeck->cardPile[0];
  WorkingDeck->cardCount--;

  // Move all remaining cards up one to the top.
  for(i=1; i<=WorkingDeck->cardCount; i++)
    WorkingDeck->cardPile[i-1] = WorkingDeck->cardPile[i];

  // Return the card selected.
  return( Card );
}



/*****************************************************************************
 * Set up the Chance and Community decks. Determines what cards are in each
 * deck and shuffles them.
 */

void InitialiseDecks (RULE_GameStatePointer GameStatePntr)
{
  RULE_CardDeckPointer WorkingDeck;

  // Set up the CommunityChest cards.
  WorkingDeck = &(GameStatePntr->Cards[COMMUNITY_DECK]);

  // For the real version, we will have some way of determining how many
  // cards and what if any cards are excluded from the deck. For now,
  // we include all the cards.
  WorkingDeck->cardCount = COMMUNITY_SIZE;
  WorkingDeck->jailOwner = RULE_NOBODY_PLAYER;
  WorkingDeck->jailOfferedInTradeTo = RULE_NOBODY_PLAYER;

  // Shuffle the deck.
  ShuffleCardDeck(WorkingDeck, COMMUNITY_FIRST);

  // Set up the Chance cards.
  WorkingDeck = &(GameStatePntr->Cards[CHANCE_DECK]);

  // Similarly to the Community Chest deck, for now assign all cards.
  WorkingDeck->cardCount = CHANCE_SIZE;
  WorkingDeck->jailOwner = RULE_NOBODY_PLAYER;
  WorkingDeck->jailOfferedInTradeTo = RULE_NOBODY_PLAYER;

  // Shuffle the deck.
  ShuffleCardDeck(WorkingDeck, CHANCE_FIRST);
}



/*****************************************************************************
 * These routines deal a card from the top of the deck. If the deck has no
 * more cards remaining, an error occurs.
 */

RULE_CardType DealChanceCard (RULE_GameStatePointer GameStatePntr)
{
  // Remove the top card from the deck.
  return ( DealCardFromTopOfDeck( &(GameStatePntr->Cards[CHANCE_DECK]) ) );
}

RULE_CardType DealCommunityCard (RULE_GameStatePointer GameStatePntr)
{
  // Remove the top card from the deck.
  return ( DealCardFromTopOfDeck( &(GameStatePntr->Cards[COMMUNITY_DECK]) ) );
}



/*****************************************************************************
 * These routines return a card to the bottom of a deck. If the deck is
 * already full, an error occurs.
 */

void ReturnCard (RULE_GameStatePointer GameStatePntr, RULE_CardType OldCard)
{
  RULE_CardDeckPointer WorkingDeck;

  if (OldCard >= CHANCE_FIRST && OldCard < CHANCE_MAX)
    WorkingDeck = &(GameStatePntr->Cards[CHANCE_DECK]);
  else if (OldCard >= COMMUNITY_FIRST && OldCard < COMMUNITY_MAX)
    WorkingDeck = &(GameStatePntr->Cards[COMMUNITY_DECK]);
  else
  {
#if _DEBUG
    DBUG_DisplayNonFatalErrorMessage ("ReturnCard: trying to return an unknown kind of card.");
#endif
    return;
  }

  // If there is too many cards already on the deck, error.
  if (WorkingDeck->cardCount >= MAX_CARDS_IN_DECK)
  {
#if _DEBUG
    DBUG_DisplayNonFatalErrorMessage ("ReturnCard: Card deck already full! Cannot return card.");
#endif
    return;
  }

  // Return the card to the bottom of the deck.
  WorkingDeck->cardPile[ WorkingDeck->cardCount++ ] = (unsigned char) OldCard;
}



/*****************************************************************************
 * Validate the game options.  Peg invalid ones to reasonable values.
 */

static void ValidateGameOptions (RULE_GameOptionsPointer GameOptionsPntr)
{
  if (GameOptionsPntr == NULL)
    return;

  /* Only 4 or 5 houses per hotel. */
  if (GameOptionsPntr->housesPerHotel != 5)
    GameOptionsPntr->housesPerHotel = 4;

  /* Have enough houses to build hotels on three properties. */
  if (GameOptionsPntr->maximumHouses <
    3 * (GameOptionsPntr->housesPerHotel - 1))
    GameOptionsPntr->maximumHouses =
    3 * (GameOptionsPntr->housesPerHotel - 1);

  /* Don't care how many hotels there are (zero is fine). */
  /* GameOptionsPntr->maximumHotels */

  /* Mortgage interest rate shouldn't be negative.  Or too big (stop at 10X). */
  if (GameOptionsPntr->interestRate < 0)
    GameOptionsPntr->interestRate = 0;
  if (GameOptionsPntr->interestRate > 1000)
    GameOptionsPntr->interestRate = 1000;

  /* Need non-negative initial cash.  Also cap at 10 million. */
  if (GameOptionsPntr->initialCash < 0)
    GameOptionsPntr->initialCash = 0;
  if (GameOptionsPntr->initialCash > 10000000)
    GameOptionsPntr->initialCash = 10000000;

  /* Can't have negative salary.  Or more than a million dollars. */
  if (GameOptionsPntr->passingGoAmount < 0)
    GameOptionsPntr->passingGoAmount = 0;
  if (GameOptionsPntr->passingGoAmount > 1000000)
    GameOptionsPntr->passingGoAmount = 1000000;

  /* Luxury tax has to be positive.  Or up to a million. */
  if (GameOptionsPntr->luxuryTaxAmount < 0)
    GameOptionsPntr->luxuryTaxAmount = 0;
  if (GameOptionsPntr->luxuryTaxAmount > 1000000)
    GameOptionsPntr->luxuryTaxAmount = 1000000;

  /* Tax rate should be from 0 to 100%.  Zero is a special code value
     meaning always flat. */
  if (GameOptionsPntr->taxRate < 0)
    GameOptionsPntr->taxRate = 0;
  if (GameOptionsPntr->taxRate > 100)
    GameOptionsPntr->taxRate = 100;

  /* Flat tax fee can be 0 to a million. */
  if (GameOptionsPntr->flatTaxFee < 0)
    GameOptionsPntr->flatTaxFee = 0;
  if (GameOptionsPntr->flatTaxFee > 1000000)
    GameOptionsPntr->flatTaxFee = 1000000;

  /* Can only have as many bankruptcies as there are players.  Zero is special. */
  if (GameOptionsPntr->stopAtNthBankruptcy > RULE_MAX_PLAYERS)
    GameOptionsPntr->stopAtNthBankruptcy = RULE_MAX_PLAYERS;

  /* Can only deal as many properties as there are.  Well, if it was a one
     player game then you could deal this many to each player. */
  if (GameOptionsPntr->dealNPropertiesAtStartup > SQ_TOTAL_PROPERTY_SQUARES)
    GameOptionsPntr->dealNPropertiesAtStartup = SQ_TOTAL_PROPERTY_SQUARES;

  /* Allow from 0 to 100 turns in jail.  Zero is a special code value
  for Holly's short game rule where you get out of jail the next turn. */
  if (GameOptionsPntr->maximumTurnsInJail > 100)
    GameOptionsPntr->maximumTurnsInJail = 100;

  /* From 0 to a million dollars to get out of jail. */
  if (GameOptionsPntr->getOutOfJailFee < 0)
    GameOptionsPntr->getOutOfJailFee = 0;
  if (GameOptionsPntr->getOutOfJailFee > 1000000)
    GameOptionsPntr->getOutOfJailFee = 1000000;

  /* Free parking seed from 0 to a million. */
  if (GameOptionsPntr->freeParkingSeed < 0)
    GameOptionsPntr->freeParkingSeed = 0;
  if (GameOptionsPntr->freeParkingSeed > 1000000)
    GameOptionsPntr->freeParkingSeed = 1000000;

  /* Housing shortage when 0 to maxhouses left. */
  if (GameOptionsPntr->houseShortageLevel > GameOptionsPntr->maximumHouses)
    GameOptionsPntr->houseShortageLevel = GameOptionsPntr->maximumHouses;

  /* Hotel shortage when 0 to maxhotels left. */
  if (GameOptionsPntr->hotelShortageLevel > GameOptionsPntr->maximumHotels)
    GameOptionsPntr->hotelShortageLevel = GameOptionsPntr->maximumHotels;

  /* Time delay for auctions is 1 to 240 seconds. */
  if (GameOptionsPntr->auctionGoingTimeDelay < 1)
    GameOptionsPntr->auctionGoingTimeDelay = 1;
  if (GameOptionsPntr->auctionGoingTimeDelay > 240)
    GameOptionsPntr->auctionGoingTimeDelay = 240;

  /* Inactivity warning time delay is 0 to 600 seconds, or zero for off. */
  if (GameOptionsPntr->inactivityWarningTime < 0)
    GameOptionsPntr->inactivityWarningTime = 0;
  if (GameOptionsPntr->inactivityWarningTime > 600)
    GameOptionsPntr->inactivityWarningTime = 600;

  /* Short game time limit is 60 to 43200 seconds. */
  if (GameOptionsPntr->gameOverTimeLimit < 60)
    GameOptionsPntr->gameOverTimeLimit = 0; /* Meaning no time limit. */
  if (GameOptionsPntr->gameOverTimeLimit > 43200)
    GameOptionsPntr->gameOverTimeLimit = 43200;

  /* Convert the boolean value to the standard for TRUE or FALSE. */

  GameOptionsPntr->hideCash =
    (GameOptionsPntr->hideCash != 0);

  GameOptionsPntr->evenBuildRule =
    (GameOptionsPntr->evenBuildRule != 0);

  GameOptionsPntr->rollDiceToDecideStartingOrder =
    (GameOptionsPntr->rollDiceToDecideStartingOrder != 0);

  GameOptionsPntr->cheatingAllowed =
    (GameOptionsPntr->cheatingAllowed != 0);

  GameOptionsPntr->AITakesTimeToThink =
    (GameOptionsPntr->AITakesTimeToThink != 0);

  GameOptionsPntr->futureRentTradingAllowed =
    (GameOptionsPntr->futureRentTradingAllowed != 0);

  GameOptionsPntr->immunitiesTradingAllowed =
    (GameOptionsPntr->immunitiesTradingAllowed != 0);

  GameOptionsPntr->dealFreePropertiesAtStartup =
    (GameOptionsPntr->dealFreePropertiesAtStartup != 0);

  GameOptionsPntr->freeParkingPot =
    (GameOptionsPntr->freeParkingPot != 0);

  GameOptionsPntr->doubleSalaryOnGo =
    (GameOptionsPntr->doubleSalaryOnGo != 0);

  GameOptionsPntr->allowPlayersToTakeOverAIs =
    (GameOptionsPntr->allowPlayersToTakeOverAIs != 0);

  GameOptionsPntr->mortgagedCountsInGroupRent =
    (GameOptionsPntr->mortgagedCountsInGroupRent != 0);
}



/*****************************************************************************
 * Define the various chunk types and their contents used in saving game
 * options, and some for the whole game too.  See Rules.h for more detailed
 * descriptions of the field values.
 */

/* RIFF/MPSV chunk.  An entire saved game state is stored in this chunk.  It
   contains a couple of CARD chunks describing the card decks, some GFS1 phase
   stack entries in top of stack to bottom of stack order,  a bunch of IMU1
   chunks describing immunities, a RIFF/MOPT chunk with the game options,
   saved player states in a bunch of RIFF/PLYR chunks, some RIFF/SQRE chunks
   for square information, and a SVG1 with the global saved game info.  In
   general, the order of the chunks in the RIFF is alphabetical order. */


/* RIFF/MOPT chunk.  The game options are stored in this chunk.  It contains
   RUL1, USR1, USR2, VOC1 and VOC2 chunks. */


/* RIFF/PLYR chunk.  Saved player information.  Includes AISV, NAME, PLY1. */


/* RIFF/SQRE chunk.  Saved square information.  Includes NAME, SQR1. */


/* AISV chunk.  Saved AI internal state information.  See AI.h */


/* "CARD" chunk.  Variable sized saved card deck. */
typedef struct SavedCardsStruct
{
  unsigned char cardDeckType;  /* CHANCE_DECK or COMMUNITY_DECK */
  unsigned char jailOwner;
  unsigned char cardCount;
  unsigned char cardPile [1];
    /* The cards are stored in this variable length array (cardCount entries),
    the following data in the chunk contains more unsigned char fields as
    needed for the array. */
} SavedCardsRecord, *SavedCardsPointer;


/* "GFS1" chunk.  Game phase saved stack.  Stored in order, top of the stack
   first (index 0) and bottom last (larger index) in the file. */
typedef struct GamePhaseSaved1Struct
{
  long          amount;
  unsigned char stackIndex;
  unsigned char phase;
  unsigned char fromPlayer;
  unsigned char toPlayer;
} GamePhaseSaved1Record, *GamePhaseSaved1Pointer;


/* "IMU1" chunk.  Immunities. */
typedef struct SavedImmunity1Struct
{
  unsigned long properties;
  unsigned char fromPlayer;
  unsigned char toPlayer;
  unsigned char hitType;
  signed char   hitCount;
} SavedImmunity1Record, *SavedImmunity1Pointer;


/* "NAME" chunk.  Variable size chunk containing a player name
    as a NUL terminated Unicode string. */


/* "PLY1" chunk.  Saved player standard state information. */
typedef struct SavedPlayerInfo1Struct
{
  long          cash;
  unsigned char playerIndex;
  unsigned char token;
  unsigned char colour;
  unsigned char currentSquare;
  unsigned char firstMoveMade;
  unsigned char turnsInJail;
  unsigned char AIPlayerLevel;
} SavedPlayerInfo1Record, *SavedPlayerInfo1Pointer;


/* "RUL1" chunk.   Rules Engine options. */
typedef struct RuleEngine1OptionsStruct
{
  long            initialCash;
  long            passingGoAmount;
  long            interestRate;
  long            taxRate;
  long            flatTaxFee;
  long            luxuryTaxAmount;
  long            getOutOfJailFee;
  long            freeParkingSeed;
  unsigned short  inactivityWarningTime;
  unsigned short  gameOverTimeLimit;
  unsigned char   housesPerHotel;
  unsigned char   maximumHouses;
  unsigned char   maximumHotels;
  unsigned char   evenBuildRule;
  unsigned char   rollDiceToDecideStartingOrder;
  unsigned char   cheatingAllowed;
  unsigned char   mortgagedCountsInGroupRent;
  unsigned char   houseShortageLevel;
  unsigned char   hotelShortageLevel;
  unsigned char   auctionGoingTimeDelay;
  unsigned char   maximumTurnsInJail;
  unsigned char   futureRentTradingAllowed;
  unsigned char   immunitiesTradingAllowed;
  unsigned char   allowPlayersToTakeOverAIs;
  unsigned char   dealTwoPropertiesAtStartup;
  unsigned char   stopAtSecondBankruptcy;
  unsigned char   freeParkingPot;
  unsigned char   doubleSalaryOnGo;
} RuleEngine1OptionsRecord, *RuleEngine1OptionsPointer;


/* "RUL2" chunk.   Newer batch of Rules Engine options. */
typedef struct RuleEngine2OptionsStruct
{
  unsigned char dealFreePropertiesAtStartup;
  unsigned char dealNPropertiesAtStartup;
  unsigned char stopAtNthBankruptcy;
} RuleEngine2OptionsRecord, *RuleEngine2OptionsPointer;


/* SQR1 chunk.  Saved game square information. */
typedef struct SavedSquare1Struct
{
  unsigned char squareNumber; /* RULE_SquareType identifying this square. */
  unsigned char owner;
  unsigned char houses;
  unsigned char mortgaged;
} SavedSquare1Record, *SavedSquare1Pointer;


/* "SVG1" chunk.  Saved game global state. */
typedef struct SavedGame1Struct
{
  unsigned long GameDurationInSeconds;
  unsigned char Dice[2];
  unsigned char NextDice[2];
  unsigned char UtilityDice[2];
  unsigned char NumberOfPlayers;
  unsigned char CurrentPlayer;
  unsigned char NumberOfDoublesRolled;
  unsigned char JustRolledOutOfJail;
  unsigned char PendingCard;
  unsigned char NumberOfPendingPhases;
} SavedGame1Record, *SavedGame1Pointer;


/* "USR1" chunk.  Generic user interface options. */
typedef struct UserPrefs1OptionsStruct
{
  unsigned char hideCash;
  unsigned char AITakesTimeToThink;
} UserPrefs1OptionsRecord, *UserPrefs1OptionsPointer;


/* "VOC1" chunk.  Voice chat options 1. */
typedef struct VoiceChat1OptionsStruct
{
  DWORD         recordingHz;
  unsigned char recordingBits;
} VoiceChat1OptionsRecord, *VoiceChat1OptionsPointer;


/* "VOC2" chunk.  It is variable size and contains a NUL terminated
   Unicode string naming the voice compressor Codec to use. */



/*****************************************************************************
 * Convert the given file contents to game options.  The file format is
 * a RIFF/MOPT chunk with various subchunks for the options.  Any options
 * in the UpdatedOptions that aren't in the file are left unchanged (but
 * are validated).
 *
 * Returns TRUE if at least partially successful, FALSE if a file format
 * error is detected (probably not a game options file then).
 */

BOOL RULE_ConvertFileToGameOptions (BYTE *FileContents, DWORD FileSize,
RULE_GameOptionsPointer UpdatedOptions)
{
  LONG                      AmountRead;
  MMIOINFO                  MMIOInfoRec;
  MMRESULT                  MMErrorCode;
  LONG                      NameLength;
  BOOL                      ReturnCode = TRUE;
  MMCKINFO                  RIFFChunk;
  HMMIO                     RIFFFile = NULL;
  RuleEngine1OptionsRecord  Rul1;
  RuleEngine2OptionsRecord  Rul2;
  MMCKINFO                  SubChunk;
  UserPrefs1OptionsRecord   Usr1;
  VoiceChat1OptionsRecord   Voc1;

  if (UpdatedOptions == NULL)
    return FALSE;

  if (FileContents == NULL)
    goto FAILURE_EXIT; /* At least UpdatedOptions gets validated. */

  /* Open a MMIO stream on the buffer and look for the RIFF/MOPT
     chunk which contains the Monopoly game options. */

  memset (&MMIOInfoRec, 0, sizeof (MMIOInfoRec));
  MMIOInfoRec.fccIOProc = FOURCC_MEM;
  MMIOInfoRec.pchBuffer = (char *)FileContents;
  MMIOInfoRec.cchBuffer = FileSize;
  RIFFFile = mmioOpen (NULL, &MMIOInfoRec, MMIO_READ);
  if (RIFFFile == NULL) goto FAILURE_EXIT;

  memset (&RIFFChunk, 0, sizeof (RIFFChunk));
  RIFFChunk.fccType = MAKEFOURCC ('M', 'O', 'P', 'T');
  MMErrorCode = mmioDescend (RIFFFile, &RIFFChunk, NULL, MMIO_FINDRIFF);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  /* Look for the "RUL1" chunk.  First one of the subchunks, which are read in
     alphabetical order.  Actually, they don't need to be since we seek back
     to the beginning of the main RIFF data area after each item. */

  memset (&SubChunk, 0, sizeof (SubChunk));
  SubChunk.ckid = MAKEFOURCC ('R', 'U', 'L', '1');
  MMErrorCode = mmioDescend (RIFFFile, &SubChunk, &RIFFChunk, MMIO_FINDCHUNK);
  if (MMErrorCode == MMSYSERR_NOERROR)
  {
    AmountRead = mmioRead (RIFFFile, (char *) &Rul1, sizeof (Rul1));
    if (AmountRead != sizeof (Rul1)) goto FAILURE_EXIT;

    UpdatedOptions->initialCash = Rul1.initialCash;
    UpdatedOptions->passingGoAmount = Rul1.passingGoAmount;
    UpdatedOptions->interestRate = Rul1.interestRate;
    UpdatedOptions->taxRate = Rul1.taxRate;
    UpdatedOptions->flatTaxFee = Rul1.flatTaxFee;
    UpdatedOptions->luxuryTaxAmount = Rul1.luxuryTaxAmount;
    UpdatedOptions->getOutOfJailFee = Rul1.getOutOfJailFee;
    UpdatedOptions->freeParkingSeed = Rul1.freeParkingSeed;
    UpdatedOptions->housesPerHotel = Rul1.housesPerHotel;
    UpdatedOptions->maximumHouses = Rul1.maximumHouses;
    UpdatedOptions->maximumHotels = Rul1.maximumHotels;
    UpdatedOptions->evenBuildRule = Rul1.evenBuildRule;
    UpdatedOptions->rollDiceToDecideStartingOrder = Rul1.rollDiceToDecideStartingOrder;
    UpdatedOptions->cheatingAllowed = Rul1.cheatingAllowed;
    UpdatedOptions->mortgagedCountsInGroupRent = Rul1.mortgagedCountsInGroupRent;
    UpdatedOptions->houseShortageLevel = Rul1.houseShortageLevel;
    UpdatedOptions->hotelShortageLevel = Rul1.hotelShortageLevel;
    UpdatedOptions->auctionGoingTimeDelay = Rul1.auctionGoingTimeDelay;
    UpdatedOptions->inactivityWarningTime = Rul1.inactivityWarningTime;
    UpdatedOptions->gameOverTimeLimit = Rul1.gameOverTimeLimit;
    UpdatedOptions->maximumTurnsInJail = Rul1.maximumTurnsInJail;
    UpdatedOptions->futureRentTradingAllowed = Rul1.futureRentTradingAllowed;
    UpdatedOptions->immunitiesTradingAllowed = Rul1.immunitiesTradingAllowed;
    UpdatedOptions->allowPlayersToTakeOverAIs = Rul1.allowPlayersToTakeOverAIs;
    if (Rul1.dealTwoPropertiesAtStartup)
    {
      UpdatedOptions->dealNPropertiesAtStartup = 2;
      UpdatedOptions->dealFreePropertiesAtStartup = FALSE;
    }
    if (Rul1.stopAtSecondBankruptcy)
      UpdatedOptions->stopAtNthBankruptcy = 2;
    UpdatedOptions->freeParkingPot = Rul1.freeParkingPot;
    UpdatedOptions->doubleSalaryOnGo = Rul1.doubleSalaryOnGo;

    mmioAscend (RIFFFile, &SubChunk, 0); /* Just for style. */
  }
  mmioSeek (RIFFFile, RIFFChunk.dwDataOffset + 4, SEEK_SET); /* Rewind to start. */

  /* Look for the "RUL2" chunk. */

  memset (&SubChunk, 0, sizeof (SubChunk));
  SubChunk.ckid = MAKEFOURCC ('R', 'U', 'L', '2');
  MMErrorCode = mmioDescend (RIFFFile, &SubChunk, &RIFFChunk, MMIO_FINDCHUNK);
  if (MMErrorCode == MMSYSERR_NOERROR)
  {
    AmountRead = mmioRead (RIFFFile, (char *) &Rul2, sizeof (Rul2));
    if (AmountRead != sizeof (Rul2)) goto FAILURE_EXIT;

    UpdatedOptions->dealFreePropertiesAtStartup = Rul2.dealFreePropertiesAtStartup;
    UpdatedOptions->dealNPropertiesAtStartup = Rul2.dealNPropertiesAtStartup;
    UpdatedOptions->stopAtNthBankruptcy = Rul2.stopAtNthBankruptcy;

    mmioAscend (RIFFFile, &SubChunk, 0); /* Just for style. */
  }
  mmioSeek (RIFFFile, RIFFChunk.dwDataOffset + 4, SEEK_SET); /* Rewind to start. */

  /* Look for the "USR1" chunk.  Generic user interface options. */

  memset (&SubChunk, 0, sizeof (SubChunk));
  SubChunk.ckid = MAKEFOURCC ('U', 'S', 'R', '1');
  MMErrorCode = mmioDescend (RIFFFile, &SubChunk, &RIFFChunk, MMIO_FINDCHUNK);
  if (MMErrorCode == MMSYSERR_NOERROR)
  {
    AmountRead = mmioRead (RIFFFile, (char *) &Usr1, sizeof (Usr1));
    if (AmountRead != sizeof (Usr1)) goto FAILURE_EXIT;

    UpdatedOptions->hideCash = Usr1.hideCash;
    UpdatedOptions->AITakesTimeToThink = Usr1.AITakesTimeToThink;

    mmioAscend (RIFFFile, &SubChunk, 0); /* Just for style. */
  }
  mmioSeek (RIFFFile, RIFFChunk.dwDataOffset + 4, SEEK_SET); /* Rewind to start. */

  /* Look for the "VOC1" chunk.  Voice chat options 1. */

  memset (&SubChunk, 0, sizeof (SubChunk));
  SubChunk.ckid = MAKEFOURCC ('V', 'O', 'C', '1');
  MMErrorCode = mmioDescend (RIFFFile, &SubChunk, &RIFFChunk, MMIO_FINDCHUNK);
  if (MMErrorCode == MMSYSERR_NOERROR)
  {
    AmountRead = mmioRead (RIFFFile, (char *) &Voc1, sizeof (Voc1));
    if (AmountRead != sizeof (Voc1)) goto FAILURE_EXIT;

    UpdatedOptions->voiceChat.recordingHz = Voc1.recordingHz;
    UpdatedOptions->voiceChat.recordingBits = Voc1.recordingBits;

    mmioAscend (RIFFFile, &SubChunk, 0); /* Just for style. */
  }
  mmioSeek (RIFFFile, RIFFChunk.dwDataOffset + 4, SEEK_SET); /* Rewind to start. */

  /* Look for the "VOC2" chunk.  Voice compressor codec name. */

  memset (&SubChunk, 0, sizeof (SubChunk));
  SubChunk.ckid = MAKEFOURCC ('V', 'O', 'C', '2');
  MMErrorCode = mmioDescend (RIFFFile, &SubChunk, &RIFFChunk, MMIO_FINDCHUNK);
  if (MMErrorCode == MMSYSERR_NOERROR)
  {
    NameLength = SubChunk.cksize;
    if (NameLength > RULE_MAX_VOC_COMRESSOR_NAME_LENGTH * sizeof (wchar_t))
      NameLength = RULE_MAX_VOC_COMRESSOR_NAME_LENGTH * sizeof (wchar_t);

    AmountRead = mmioRead (RIFFFile,
      (char *) UpdatedOptions->voiceChat.compressorName, NameLength);
    if (AmountRead != NameLength) goto FAILURE_EXIT;
    UpdatedOptions->voiceChat.compressorName [RULE_MAX_VOC_COMRESSOR_NAME_LENGTH] = 0;

    mmioAscend (RIFFFile, &SubChunk, 0); /* Just for style. */
  }
  mmioSeek (RIFFFile, RIFFChunk.dwDataOffset + 4, SEEK_SET); /* Rewind to start. */

  goto NORMAL_EXIT;


FAILURE_EXIT:
  ReturnCode = FALSE;

NORMAL_EXIT:
  if (RIFFFile != NULL)
  {
    mmioClose (RIFFFile, 0);
    RIFFFile = NULL;
  }

  ValidateGameOptions (UpdatedOptions);

  return ReturnCode;
}



/*****************************************************************************
 * Write the game options in RIFF format (A RIFF/MOPT chunk and subchunks) to
 * the given multimedia IO handle.  Returns number of bytes in the RIFF chunk,
 * including headers, zero on failure.
 */

static DWORD ConvertGameOptionsToRIFF (RULE_GameOptionsPointer OptionsToPack,
HMMIO RIFFFile)
{
  LONG                      AmountWritten;
  LONG                      BufferSizeIncrement = 4096;
  MMRESULT                  MMErrorCode;
  LONG                      NameLength;
  DWORD                     ReturnCode = 0;
  MMCKINFO                  RIFFChunk;
  RuleEngine1OptionsRecord  Rul1;
  RuleEngine2OptionsRecord  Rul2;
  MMCKINFO                  SubChunk;
  UserPrefs1OptionsRecord   Usr1;
  VoiceChat1OptionsRecord   Voc1;

  if (OptionsToPack == NULL || RIFFFile == NULL)
    goto FAILURE_EXIT;

  /* Create the RIFF chunk with subtype MOPT for Monopoly Options. */

  memset (&RIFFChunk, 0, sizeof (RIFFChunk));
  RIFFChunk.fccType = mmioFOURCC ('M', 'O', 'P', 'T');
  MMErrorCode = mmioCreateChunk (RIFFFile, &RIFFChunk, MMIO_CREATERIFF);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  /* Write the  RUL1 chunk.  Writing done in alphabetical order. */

  memset (&SubChunk, 0, sizeof (SubChunk));
  SubChunk.ckid = mmioFOURCC ('R', 'U', 'L', '1');
  MMErrorCode = mmioCreateChunk (RIFFFile, &SubChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  Rul1.initialCash = OptionsToPack->initialCash;
  Rul1.passingGoAmount = OptionsToPack->passingGoAmount;
  Rul1.interestRate = OptionsToPack->interestRate;
  Rul1.taxRate = OptionsToPack->taxRate;
  Rul1.flatTaxFee = OptionsToPack->flatTaxFee;
  Rul1.luxuryTaxAmount = OptionsToPack->luxuryTaxAmount;
  Rul1.getOutOfJailFee = OptionsToPack->getOutOfJailFee;
  Rul1.freeParkingSeed = OptionsToPack->freeParkingSeed;
  Rul1.housesPerHotel = OptionsToPack->housesPerHotel;
  Rul1.maximumHouses = OptionsToPack->maximumHouses;
  Rul1.maximumHotels = OptionsToPack->maximumHotels;
  Rul1.evenBuildRule = OptionsToPack->evenBuildRule;
  Rul1.rollDiceToDecideStartingOrder = OptionsToPack->rollDiceToDecideStartingOrder;
  Rul1.cheatingAllowed = OptionsToPack->cheatingAllowed;
  Rul1.mortgagedCountsInGroupRent = OptionsToPack->mortgagedCountsInGroupRent;
  Rul1.houseShortageLevel = OptionsToPack->houseShortageLevel;
  Rul1.hotelShortageLevel = OptionsToPack->hotelShortageLevel;
  Rul1.auctionGoingTimeDelay = OptionsToPack->auctionGoingTimeDelay;
  Rul1.inactivityWarningTime = OptionsToPack->inactivityWarningTime;
  Rul1.gameOverTimeLimit = OptionsToPack->gameOverTimeLimit;
  Rul1.maximumTurnsInJail = OptionsToPack->maximumTurnsInJail;
  Rul1.futureRentTradingAllowed = OptionsToPack->futureRentTradingAllowed;
  Rul1.immunitiesTradingAllowed = OptionsToPack->immunitiesTradingAllowed;
  Rul1.allowPlayersToTakeOverAIs = OptionsToPack->allowPlayersToTakeOverAIs;
  Rul1.dealTwoPropertiesAtStartup =
    (2 == OptionsToPack->dealNPropertiesAtStartup &&
    !OptionsToPack->dealFreePropertiesAtStartup);
  Rul1.stopAtSecondBankruptcy =
    (2 == OptionsToPack->stopAtNthBankruptcy);
  Rul1.freeParkingPot = OptionsToPack->freeParkingPot;
  Rul1.doubleSalaryOnGo = OptionsToPack->doubleSalaryOnGo;

  AmountWritten = mmioWrite (RIFFFile, (const char *) &Rul1, sizeof (Rul1));
  if (AmountWritten != sizeof (Rul1)) goto FAILURE_EXIT;

  MMErrorCode = mmioAscend (RIFFFile, &SubChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  /* Write the  RUL2 chunk. */

  memset (&SubChunk, 0, sizeof (SubChunk));
  SubChunk.ckid = mmioFOURCC ('R', 'U', 'L', '2');
  MMErrorCode = mmioCreateChunk (RIFFFile, &SubChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  Rul2.dealFreePropertiesAtStartup = OptionsToPack->dealFreePropertiesAtStartup;
  Rul2.dealNPropertiesAtStartup = OptionsToPack->dealNPropertiesAtStartup;
  Rul2.stopAtNthBankruptcy = OptionsToPack->stopAtNthBankruptcy;

  AmountWritten = mmioWrite (RIFFFile, (const char *) &Rul2, sizeof (Rul2));
  if (AmountWritten != sizeof (Rul2)) goto FAILURE_EXIT;

  MMErrorCode = mmioAscend (RIFFFile, &SubChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  /* Write the  USR1 chunk. */

  memset (&SubChunk, 0, sizeof (SubChunk));
  SubChunk.ckid = mmioFOURCC ('U', 'S', 'R', '1');
  MMErrorCode = mmioCreateChunk (RIFFFile, &SubChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  Usr1.hideCash = OptionsToPack->hideCash;
  Usr1.AITakesTimeToThink = OptionsToPack->AITakesTimeToThink;

  AmountWritten = mmioWrite (RIFFFile, (char *) &Usr1, sizeof (Usr1));
  if (AmountWritten != sizeof (Usr1)) goto FAILURE_EXIT;

  MMErrorCode = mmioAscend (RIFFFile, &SubChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  /* Write the  VOC1 chunk. */

  memset (&SubChunk, 0, sizeof (SubChunk));
  SubChunk.ckid = mmioFOURCC ('V', 'O', 'C', '1');
  MMErrorCode = mmioCreateChunk (RIFFFile, &SubChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  Voc1.recordingHz = OptionsToPack->voiceChat.recordingHz;
  Voc1.recordingBits = (unsigned char) OptionsToPack->voiceChat.recordingBits;

  AmountWritten = mmioWrite (RIFFFile, (char *) &Voc1, sizeof (Voc1));
  if (AmountWritten != sizeof (Voc1)) goto FAILURE_EXIT;

  MMErrorCode = mmioAscend (RIFFFile, &SubChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  /* "VOC2" chunk.  NUL terminated Unicode string naming the voice
     compressor Codec to use. */

  memset (&SubChunk, 0, sizeof (SubChunk));
  SubChunk.ckid = mmioFOURCC ('V', 'O', 'C', '2');
  MMErrorCode = mmioCreateChunk (RIFFFile, &SubChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  NameLength = (wcslen (OptionsToPack->voiceChat.compressorName) + 1) *
    sizeof (wchar_t);

  AmountWritten = mmioWrite (RIFFFile,
    (char *) OptionsToPack->voiceChat.compressorName, NameLength);
  if (AmountWritten != NameLength) goto FAILURE_EXIT;

  MMErrorCode = mmioAscend (RIFFFile, &SubChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  /* Wrap it up - end the main RIFF chunk. */

  MMErrorCode = mmioAscend (RIFFFile, &RIFFChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  return RIFFChunk.cksize + 8 /* For RIFF header itself */;

FAILURE_EXIT:
  return 0;
}



/*****************************************************************************
 * Write the game options in RIFF format to the given GlobalAlloc'd buffer,
 * which gets reallocated if more space is needed.  Returns the number of
 * bytes written.  You will have to relock the buffer handle to get a pointer
 * to it, and the pointer you pass in has to be from the one and only lock
 * done to the handle (otherwise it can't get unlocked and grown).  Returns
 * zero if something goes wrong.
 */

DWORD RULE_ConvertGameOptionsToFile (RULE_GameOptionsPointer OptionsToPack,
BYTE *OutputBuffer, DWORD BufferInitialSize)
{
  LONG                      BufferSizeIncrement = 4096;
  MMIOINFO                  MMIOInfoRec;
  DWORD                     ReturnCode = 0;
  HMMIO                     RIFFFile = NULL;

  if (OptionsToPack == NULL || OutputBuffer == NULL)
    goto FAILURE_EXIT;

  /* Open a MMIO stream on the buffer in growth mode. */

  memset (&MMIOInfoRec, 0, sizeof (MMIOInfoRec));
  MMIOInfoRec.fccIOProc = FOURCC_MEM;
  MMIOInfoRec.pchBuffer = (char *)OutputBuffer;
  MMIOInfoRec.cchBuffer = BufferInitialSize; /* Initial size */
  MMIOInfoRec.adwInfo[0] = BufferSizeIncrement; /* Size increment */
  RIFFFile = mmioOpen (NULL, &MMIOInfoRec,
    MMIO_READWRITE /* Often want to read what we wrote */ | MMIO_CREATE);
  if (RIFFFile == NULL) goto FAILURE_EXIT;

  ReturnCode = ConvertGameOptionsToRIFF (OptionsToPack, RIFFFile);

FAILURE_EXIT:
  if (RIFFFile != NULL)
  {
    mmioClose (RIFFFile, 0);
    RIFFFile = NULL;
  }

  return ReturnCode;
}



/*****************************************************************************
 * Format the game options or game state into a RIFF file and put them in the
 * blob in the given message.  Returns TRUE if successful.
 */

BOOL AddGameOptionsToMessageBlob (RULE_GameOptionsPointer OptionsPntr,
RULE_ActionArgumentsPointer MessagePntr)
{
  DWORD             FileSize;
  RULE_CharHandle   MemoryHandle = NULL;
  BYTE             *MemoryPntr = NULL;
  BOOL              ReturnCode = FALSE;

  if (OptionsPntr == NULL || MessagePntr == NULL ||
  MessagePntr->binaryDataHandleA != NULL)
    return FALSE;

  /* Allocate memory to store the binary large object. */

  MemoryHandle = RULE_AllocateHandle (1024 /* Initial size, can be grown later */);
  if (MemoryHandle == NULL)
    goto FAILURE_EXIT;

  MemoryPntr = (unsigned char *) RULE_LockHandle (MemoryHandle);
  if (MemoryPntr == NULL)
    goto FAILURE_EXIT;

  FileSize = RULE_ConvertGameOptionsToFile (OptionsPntr, MemoryPntr, 1024);

  /* Unlock the buffer before handover, also pointer invalid after buffer growth. */

  RULE_UnlockHandle (MemoryHandle);
  MemoryPntr = NULL;

  if (FileSize < 12) /* RIFF files are at least 12 bytes long. */
    goto FAILURE_EXIT;

  /* Use a message directly so we can set the size explicitly,
     not just using the allocated size which will be too big. */

  MessagePntr->binaryDataHandleA = MemoryHandle;
  MemoryHandle = NULL; /* Message queue takes care of it now. */
  MessagePntr->binaryDataSizeA = FileSize;

  ReturnCode = TRUE;

FAILURE_EXIT:
  if (MemoryPntr != NULL)
    RULE_UnlockHandle (MemoryHandle);
  if (MemoryHandle != NULL)
    RULE_FreeHandle (MemoryHandle);
  return ReturnCode;
}



/*****************************************************************************
 * Convert the given file contents into a game state.  The file format is
 * a RIFF/MPSV chunk with various subchunks.  Default values are used for
 * unspecified things.  SavedAIStatesArray is optional.
 *
 * Returns TRUE if at least partially successful, FALSE if a file format
 * error is detected (probably not a saved game file then).
 */

BOOL RULE_ConvertFileToGameState (BYTE *FileContents, DWORD FileSize,
RULE_GameStatePointer LoadStatePntr,
SavedGameDataCollectionPointer SavedAIStatesArray)
{
  LONG                      AmountRead;
  LONG                      AmountToRead;
  int                       CardCount;
  RULE_CardDeckTypes        CardDeckIndex;
  struct CardStuffStruct {
    SavedCardsRecord          header; /* All but card storage in here. */
    BYTE                      filler [MAX_CARDS_IN_DECK]; /* Card storage. */
  }                         CardStuff;
  DWORD                    *DWordPntr;
  GamePhaseSaved1Record     Gfs1;
  int                       i;
  SavedImmunity1Record      Imu1;
  MMIOINFO                  MMIOInfoRec;
  MMRESULT                  MMErrorCode;
  int                       NextFreeImmunityIndex;
  RULE_PlayerNumber         PlayerNo;
  SavedPlayerInfo1Record    Ply1;
  BOOL                      PLY1ChunkReceived;
  BOOL                      ReturnCode = TRUE;
  MMCKINFO                  RIFFChunk;
  HMMIO                     RIFFFile = NULL;
  SavedSquare1Record        Sqr1;
  RULE_SquareType           SquareNo;
  MMCKINFO                  SubChunk;
  MMCKINFO                  SubSubChunk;
  SavedGame1Record          Svg1;
  BOOL                      SVG1ChunkReceived = FALSE;

  /* Some sanity checks. */

  if (LoadStatePntr == NULL)
    goto FAILURE_EXIT;

  if (FileContents == NULL)
    goto FAILURE_EXIT;

  /* Clear some collections that are used when adding items and clear the
     game state to the new game status, in case some chunks are missing. */

  memset (LoadStatePntr, 0, sizeof (RULE_GameStateRecord));

  for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
    LoadStatePntr->Squares[SquareNo].owner = RULE_NOBODY_PLAYER;

  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
    LoadStatePntr->CountHits[i].toPlayer = RULE_NOBODY_PLAYER;
  NextFreeImmunityIndex = 0;

  PlayerNo = 0; /* Presumably the players are in order from 0 to n. */

  /* Open a MMIO stream on the buffer and look for the RIFF/MPSV
     chunk which contains the Monopoly saved game. */

  memset (&MMIOInfoRec, 0, sizeof (MMIOInfoRec));
  MMIOInfoRec.fccIOProc = FOURCC_MEM;
  MMIOInfoRec.pchBuffer = (char *) FileContents;
  MMIOInfoRec.cchBuffer = FileSize;
  RIFFFile = mmioOpen (NULL, &MMIOInfoRec, MMIO_READ);
  if (RIFFFile == NULL) goto FAILURE_EXIT;

  memset (&RIFFChunk, 0, sizeof (RIFFChunk));
  RIFFChunk.fccType = MAKEFOURCC ('M', 'P', 'S', 'V');
  MMErrorCode = mmioDescend (RIFFFile, &RIFFChunk, NULL, MMIO_FINDRIFF);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  /* Iterate through the chunks in the RIFF/MPSV chunk, processing them
     in the order of occurance and skipping over unknown ones. */

  while (TRUE)
  {
    memset (&SubChunk, 0, sizeof (SubChunk));
    MMErrorCode = mmioDescend (RIFFFile, &SubChunk, &RIFFChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR)
      break; /* Finished the whole thing, or an error. */

    switch (SubChunk.ckid)
    {
    case mmioFOURCC ('R', 'I', 'F', 'F'):
      /* This sub-chunk has sub-sub-chunks. */
      switch (SubChunk.fccType)
      {
      case mmioFOURCC ('M', 'O', 'P', 'T'):
        if (!RULE_ConvertFileToGameOptions (FileContents +
        (SubChunk.dwDataOffset - 8), SubChunk.cksize + 8,
        &LoadStatePntr->GameOptions))
          goto FAILURE_EXIT;
        break;


      case mmioFOURCC ('P', 'L', 'Y', 'R'):
        /* Load information for player number PlayerNo. */
        if (PlayerNo >= RULE_MAX_PLAYERS)
          break; /* Can't load any more players. */
        PLY1ChunkReceived = FALSE;
        while (TRUE)
        {
          /* Enter the next sub-sub-chunk. */

          memset (&SubSubChunk, 0, sizeof (SubSubChunk));
          MMErrorCode = mmioDescend (RIFFFile, &SubSubChunk, &SubChunk, 0);
          if (MMErrorCode != MMSYSERR_NOERROR)
            break;  /* No more sub-sub-chunks. */

          switch (SubSubChunk.ckid)
          {
          case mmioFOURCC ('A', 'I', 'S', 'V'):
            if (SavedAIStatesArray == NULL)
              break; /* Currently not loading AI data. */
            if (SavedAIStatesArray[PlayerNo].AIStateDataPntr != NULL)
              break; /* Already have some data for this player. */
            AmountToRead = SubSubChunk.cksize;
            DWordPntr = (unsigned long *) GlobalAllocPtr (GHND, AmountToRead + 8 /* bytes */);
            if (DWordPntr == NULL)
              goto FAILURE_EXIT; /* Out of memory. */
            AmountRead = mmioRead (RIFFFile,
              (char *) (DWordPntr + 2 /* DWords */), AmountToRead);
            if (AmountRead != AmountToRead)
            {
              GlobalFreePtr (DWordPntr);
              goto FAILURE_EXIT;
            }
            DWordPntr[0] = mmioFOURCC ('A', 'I', 'S', 'V');
            DWordPntr[1] = AmountToRead; /* Chunk data portion size. */
            SavedAIStatesArray[PlayerNo].AIStateDataPntr = (unsigned char *) DWordPntr;
            break;


          case mmioFOURCC ('N', 'A', 'M', 'E'):
            AmountToRead = SubSubChunk.cksize;
            if (AmountToRead > (RULE_MAX_PLAYER_NAME_LENGTH + 1) * sizeof (wchar_t))
              AmountToRead = (RULE_MAX_PLAYER_NAME_LENGTH + 1) * sizeof (wchar_t);
            AmountRead = mmioRead (RIFFFile,
              (char *) LoadStatePntr->Players[PlayerNo].name, AmountToRead);
            if (AmountRead != AmountToRead)
              goto FAILURE_EXIT;
            LoadStatePntr->Players[PlayerNo].name[RULE_MAX_PLAYER_NAME_LENGTH] = 0;
            break;


          case mmioFOURCC ('P', 'L', 'Y', '1'):
            AmountToRead = sizeof (Ply1);
            AmountRead = mmioRead (RIFFFile, (char *) &Ply1, AmountToRead);
            if (AmountRead != AmountToRead)
              goto FAILURE_EXIT;
            if (Ply1.playerIndex != PlayerNo)
              goto FAILURE_EXIT; /* Players weren't stored in order! */
            LoadStatePntr->Players[PlayerNo].cash = Ply1.cash;
            LoadStatePntr->Players[PlayerNo].token = Ply1.token;
            LoadStatePntr->Players[PlayerNo].colour = Ply1.colour;
            LoadStatePntr->Players[PlayerNo].currentSquare = Ply1.currentSquare;
            LoadStatePntr->Players[PlayerNo].firstMoveMade = Ply1.firstMoveMade;
            LoadStatePntr->Players[PlayerNo].turnsInJail = Ply1.turnsInJail;
            LoadStatePntr->Players[PlayerNo].AIPlayerLevel = Ply1.AIPlayerLevel;
            PLY1ChunkReceived = TRUE;
            break;


          default:
            break;
          } /* End switch on SubSubChunk ID. */

          /* End of the sub-sub-chunk, this will advance to the next sub-sub-chunk. */

          MMErrorCode = mmioAscend (RIFFFile, &SubSubChunk, 0);
          if (MMErrorCode != MMSYSERR_NOERROR)
            goto FAILURE_EXIT;
        }
        if (!PLY1ChunkReceived)
          goto FAILURE_EXIT; /* Didn't receive mandatory chunk. */
        PlayerNo += 1; /* Yes, we have successfully finished another player. */
        break;


      case mmioFOURCC ('S', 'Q', 'R', 'E'):
        /* Load information for a square. */
        while (TRUE)
        {
          /* Enter the next sub-sub-chunk. */

          memset (&SubSubChunk, 0, sizeof (SubSubChunk));
          MMErrorCode = mmioDescend (RIFFFile, &SubSubChunk, &SubChunk, 0);
          if (MMErrorCode != MMSYSERR_NOERROR)
            break;  /* No more sub-sub-chunks. */

          switch (SubSubChunk.ckid)
          {
          case mmioFOURCC ('S', 'Q', 'R', '1'):
            AmountToRead = sizeof (Sqr1);
            AmountRead = mmioRead (RIFFFile, (char *) &Sqr1, AmountToRead);
            if (AmountRead != AmountToRead)
              goto FAILURE_EXIT;
            SquareNo = Sqr1.squareNumber;
            if (SquareNo >= SQ_MAX_SQUARE_TYPES)
              break; /* Ignore out of bounds squares. */
            LoadStatePntr->Squares[SquareNo].owner = Sqr1.owner;
            LoadStatePntr->Squares[SquareNo].houses = Sqr1.houses;
            LoadStatePntr->Squares[SquareNo].mortgaged = Sqr1.mortgaged;
            break;


          default:
            break;
          } /* End switch on SubSubChunk ID. */

          /* End of the sub-sub-chunk, this will advance to the next sub-sub-chunk. */

          MMErrorCode = mmioAscend (RIFFFile, &SubSubChunk, 0);
          if (MMErrorCode != MMSYSERR_NOERROR)
            goto FAILURE_EXIT;
        }
        if (!PLY1ChunkReceived)
          goto FAILURE_EXIT; /* Didn't receive mandatory chunk. */
        PlayerNo += 1; /* Yes, we have successfully finished another player. */
        break;


      default: /* An unknown RIFF type in a subchunk that we don't recognise. */
        break;
      }
      break;


    case mmioFOURCC ('C', 'A', 'R', 'D'):
      /* A variable sized array of cards. */
      AmountRead = mmioRead (RIFFFile, (char *) &CardStuff, sizeof (CardStuff));
      if (AmountRead < sizeof (SavedCardsRecord) /* Minimal size */) goto FAILURE_EXIT;
      CardCount = CardStuff.header.cardCount;
      if (CardCount > MAX_CARDS_IN_DECK)
          goto FAILURE_EXIT;
      CardDeckIndex = CardStuff.header.cardDeckType;
      if (CardDeckIndex < 0 || CardDeckIndex >= MAX_DECK_TYPES)
          goto FAILURE_EXIT;
      LoadStatePntr->Cards[CardDeckIndex].cardCount = CardCount;
      LoadStatePntr->Cards[CardDeckIndex].jailOwner = CardStuff.header.jailOwner;
      LoadStatePntr->Cards[CardDeckIndex].jailOfferedInTradeTo = RULE_NOBODY_PLAYER;
      for (i = 0; i < CardCount; i++)
        LoadStatePntr->Cards[CardDeckIndex].cardPile[i] = CardStuff.header.cardPile[i];
      break;


    case mmioFOURCC ('G', 'F', 'S', '1'):
      /* A game phase stack entry. */
      AmountRead = mmioRead (RIFFFile, (char *) &Gfs1, sizeof (Gfs1));
      if (AmountRead != sizeof (Gfs1)) goto FAILURE_EXIT;
      i = Gfs1.stackIndex;
      if (i >= RULE_MAX_PENDING_PHASES) goto FAILURE_EXIT;
      LoadStatePntr->phaseStack[i].amount = Gfs1.amount;
      LoadStatePntr->phaseStack[i].fromPlayer = Gfs1.fromPlayer;
      LoadStatePntr->phaseStack[i].toPlayer = Gfs1.toPlayer;
      LoadStatePntr->phaseStack[i].phase = Gfs1.phase;
      break;


    case mmioFOURCC ('I', 'M', 'U', '1'):
      /* Immunities and futures. */
      if (NextFreeImmunityIndex >= RULE_MAX_COUNT_HIT_SETS)
        break; /* Skip this immunity if out of space. */
      AmountRead = mmioRead (RIFFFile, (char *) &Imu1, sizeof (Imu1));
      if (AmountRead != sizeof (Imu1)) goto FAILURE_EXIT;
      if (NextFreeImmunityIndex >= RULE_MAX_COUNT_HIT_SETS) goto FAILURE_EXIT;
      LoadStatePntr->CountHits[NextFreeImmunityIndex].properties = Imu1.properties;
      LoadStatePntr->CountHits[NextFreeImmunityIndex].fromPlayer = Imu1.fromPlayer;
      LoadStatePntr->CountHits[NextFreeImmunityIndex].toPlayer = Imu1.toPlayer;
      LoadStatePntr->CountHits[NextFreeImmunityIndex].hitType = Imu1.hitType;
      LoadStatePntr->CountHits[NextFreeImmunityIndex].hitCount= Imu1.hitCount;
      NextFreeImmunityIndex++;
      break;


    case mmioFOURCC ('S', 'V', 'G', '1'):
      /* Basic saved game info.  A required chunk. */
      AmountRead = mmioRead (RIFFFile, (char *) &Svg1, sizeof (Svg1));
      if (AmountRead != sizeof (Svg1)) goto FAILURE_EXIT;

      LoadStatePntr->GameDurationInSeconds = Svg1.GameDurationInSeconds;
      for (i = 0; i < 2; i++)
      {
        // Modification - don't use the saved next dice roll because people like to CHEAT (Damn AQ whiners).
        //LoadStatePntr->Dice[i] = Svg1.Dice[i];
        //LoadStatePntr->NextDice[i] = Svg1.NextDice[i];
        LoadStatePntr->Dice[i] = 0;
        LoadStatePntr->NextDice[i] = 0;
        LoadStatePntr->UtilityDice[i] = Svg1.UtilityDice[i];
      }
      LoadStatePntr->NumberOfPlayers = Svg1.NumberOfPlayers;
      LoadStatePntr->CurrentPlayer = Svg1.CurrentPlayer;
      LoadStatePntr->NumberOfDoublesRolled = Svg1.NumberOfDoublesRolled;
      LoadStatePntr->JustRolledOutOfJail = Svg1.JustRolledOutOfJail;
      LoadStatePntr->PendingCard = Svg1.PendingCard;
      LoadStatePntr->NumberOfPendingPhases = Svg1.NumberOfPendingPhases;
      SVG1ChunkReceived = TRUE;
      break;


    default: /* Unknown subchunk in the main game state. */
      break;
    }

    /* End of the subchunk, this will advance the file to the next chunk. */

    MMErrorCode = mmioAscend (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR)
      goto FAILURE_EXIT;
  }
  mmioAscend (RIFFFile, &RIFFChunk, 0);

  if (!SVG1ChunkReceived)
    goto FAILURE_EXIT; /* Didn't get the basic game information. */

  if (PlayerNo < LoadStatePntr->NumberOfPlayers)
    goto FAILURE_EXIT; /* Didn't load all the players. */

  goto NORMAL_EXIT;


FAILURE_EXIT:
  ReturnCode = FALSE;

NORMAL_EXIT:
  if (RIFFFile != NULL)
  {
    mmioClose (RIFFFile, 0);
    RIFFFile = NULL;
  }

  return ReturnCode;
}



/*****************************************************************************
 * Write the game state in RIFF format (A RIFF/MPSV chunk and subchunks) to
 * the given multimedia IO handle.  Returns number of bytes in the RIFF chunk,
 * including headers, zero on failure.
 */

static DWORD ConvertGameStateToRIFF (RULE_GameStatePointer SaveStatePntr,
SavedGameDataCollectionPointer SavedAIStatesArray, HMMIO RIFFFile)
{
  LONG                      AmountToWrite;
  LONG                      AmountWritten;
  int                       CardCount;
  RULE_CardDeckTypes        CardDeckIndex;
  struct CardStuffStruct {
    SavedCardsRecord          header; /* All but card storage in here. */
    BYTE                      filler [MAX_CARDS_IN_DECK]; /* Card storage. */
  }                         CardStuff;
  CountHitSquareSetPointer  CountHitPntr;
  DWORD                    *DWordPntr;
  GamePhaseSaved1Record     Gfs1;
  int                       i;
  SavedImmunity1Record      Imu1;
  MMRESULT                  MMErrorCode;
  RULE_PlayerNumber         PlayerNo;
  RULE_PlayerInfoPointer    PlayerPntr;
  SavedPlayerInfo1Record    Ply1;
  DWORD                     ReturnCode = 0;
  MMCKINFO                  RIFFChunk;
  RULE_SquareType           SquareNo;
  RULE_SquareInfoPointer    SquarePntr;
  SavedSquare1Record        Sqr1;
  MMCKINFO                  SubChunk;
  MMCKINFO                  SubRIFFChunk;
  SavedGame1Record          Svg1;
  RULE_NetworkAddressRecord TempNetAddress;

  if (SaveStatePntr == NULL || SavedAIStatesArray == NULL || RIFFFile == NULL)
    goto FAILURE_EXIT;

  /* Create the RIFF chunk with subtype MPSV for Monopoly Saved Game. */

  memset (&RIFFChunk, 0, sizeof (RIFFChunk));
  RIFFChunk.fccType = mmioFOURCC ('M', 'P', 'S', 'V');
  MMErrorCode = mmioCreateChunk (RIFFFile, &RIFFChunk, MMIO_CREATERIFF);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  /* Write the "CARD" chunks.  Variable sized saved card deck. */

  for (CardDeckIndex = 0; CardDeckIndex < MAX_DECK_TYPES; CardDeckIndex++)
  {
    memset (&SubChunk, 0, sizeof (SubChunk));
    SubChunk.ckid = MAKEFOURCC ('C', 'A', 'R', 'D');
    MMErrorCode = mmioCreateChunk (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

    CardStuff.header.cardDeckType = CardDeckIndex;
    CardStuff.header.jailOwner = SaveStatePntr->Cards[CardDeckIndex].jailOwner;
    CardCount = CardStuff.header.cardCount = SaveStatePntr->Cards[CardDeckIndex].cardCount;
    if (CardCount > MAX_CARDS_IN_DECK)
    {
#if _DEBUG
      DBUG_DisplayNonFatalErrorMessage ("ConvertGameStateToRIFF: too many cards in the deck.  Can't save it.");
#endif
      continue; /* Skip this bad deck. */
    }
    for (i = 0; i < CardCount; i++)
      CardStuff.header.cardPile[i] = SaveStatePntr->Cards[CardDeckIndex].cardPile[i];

    AmountToWrite = sizeof (SavedCardsRecord) + CardCount - 1;
    AmountWritten = mmioWrite (RIFFFile, (char *) &CardStuff, AmountToWrite);
    if (AmountWritten != AmountToWrite) goto FAILURE_EXIT;

    MMErrorCode = mmioAscend (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;
  }

  /* Write the GFS1 chunks.  Game phase stack entries. */

  for (i = 0; i < SaveStatePntr->NumberOfPendingPhases; i++)
  {
    memset (&SubChunk, 0, sizeof (SubChunk));
    SubChunk.ckid = MAKEFOURCC ('G', 'F', 'S', '1');
    MMErrorCode = mmioCreateChunk (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

    Gfs1.stackIndex = i;
    Gfs1.amount = SaveStatePntr->phaseStack[i].amount;
    Gfs1.phase = SaveStatePntr->phaseStack[i].phase;
    Gfs1.fromPlayer = SaveStatePntr->phaseStack[i].fromPlayer;
    Gfs1.toPlayer = SaveStatePntr->phaseStack[i].toPlayer;
    AmountToWrite = sizeof (Gfs1);

    AmountWritten = mmioWrite (RIFFFile, (char *) &Gfs1, AmountToWrite);
    if (AmountWritten != AmountToWrite) goto FAILURE_EXIT;

    MMErrorCode = mmioAscend (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;
  }

  /* Write the IMU1 chunks.  Immunities and futures. */

  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
  {
    CountHitPntr = SaveStatePntr->CountHits + i;
    if (CountHitPntr->toPlayer == RULE_NOBODY_PLAYER ||
    CountHitPntr->tradedItem)
      continue; /* Skip unallocated immunities, or temp trade items. */

    memset (&SubChunk, 0, sizeof (SubChunk));
    SubChunk.ckid = MAKEFOURCC ('I', 'M', 'U', '1');
    MMErrorCode = mmioCreateChunk (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

    Imu1.properties = CountHitPntr->properties;
    Imu1.fromPlayer = CountHitPntr->fromPlayer;
    Imu1.toPlayer = CountHitPntr->toPlayer;
    Imu1.hitType = CountHitPntr->hitType;
    Imu1.hitCount = CountHitPntr->hitCount;
    AmountToWrite = sizeof (Imu1);

    AmountWritten = mmioWrite (RIFFFile, (char *) &Imu1, AmountToWrite);
    if (AmountWritten != AmountToWrite) goto FAILURE_EXIT;

    MMErrorCode = mmioAscend (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;
  }

  /* Write the game options chunk.  RIFF/MOPT. */

  if (ConvertGameOptionsToRIFF (&SaveStatePntr->GameOptions, RIFFFile) < 12)
    goto FAILURE_EXIT;

  /* Write the RIFF/PLYR chunks.  Player information for each player. */

  for (PlayerNo = 0; PlayerNo < SaveStatePntr->NumberOfPlayers; PlayerNo++)
  {
    PlayerPntr = SaveStatePntr->Players + PlayerNo;

    memset (&SubRIFFChunk, 0, sizeof (SubRIFFChunk));
    SubRIFFChunk.fccType = MAKEFOURCC ('P', 'L', 'Y', 'R');
    MMErrorCode = mmioCreateChunk (RIFFFile, &SubRIFFChunk, MMIO_CREATERIFF);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

    /* Write the AISV chunk.  Saved AI state information. */

    if (SavedAIStatesArray[PlayerNo].AIStateDataPntr != NULL)
    {
      DWordPntr = (unsigned long *) SavedAIStatesArray[PlayerNo].AIStateDataPntr;

      AmountToWrite = DWordPntr [1] + 8; /* Second DWORD is size of chunk contents. */

      AmountWritten = mmioWrite (RIFFFile, (char *) DWordPntr, AmountToWrite);
      if (AmountWritten != AmountToWrite) goto FAILURE_EXIT;
    }

    /* Write the NAME chunk.  Name of the player in Unicode, NUL at end of string. */

    memset (&SubChunk, 0, sizeof (SubChunk));
    SubChunk.ckid = MAKEFOURCC ('N', 'A', 'M', 'E');
    MMErrorCode = mmioCreateChunk (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

    AmountToWrite = (wcslen (PlayerPntr->name) + 1) * sizeof (wchar_t);

    AmountWritten = mmioWrite (RIFFFile, (char *) PlayerPntr->name, AmountToWrite);
    if (AmountWritten != AmountToWrite) goto FAILURE_EXIT;

    MMErrorCode = mmioAscend (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

    /* Write the PLY1 chunk.  Basic player information. */

    memset (&SubChunk, 0, sizeof (SubChunk));
    SubChunk.ckid = MAKEFOURCC ('P', 'L', 'Y', '1');
    MMErrorCode = mmioCreateChunk (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

    Ply1.cash = PlayerPntr->cash;
    Ply1.playerIndex = PlayerNo;
    Ply1.token = PlayerPntr->token;
    Ply1.colour = PlayerPntr->colour;
    Ply1.currentSquare = PlayerPntr->currentSquare;
    Ply1.firstMoveMade = PlayerPntr->firstMoveMade;
    Ply1.turnsInJail = PlayerPntr->turnsInJail;
    Ply1.AIPlayerLevel = PlayerPntr->AIPlayerLevel;

    /* If the player was a network Human player, turn them into an AI when
       they get reloaded, so that other players can take them over (the
       production version can't convert humans to AIs). */

    if (Ply1.AIPlayerLevel == 0 && SaveStatePntr == &CurrentRulesState)
    {
      MESS_GetAddressOfPlayer (PlayerNo, &TempNetAddress);
      if (TempNetAddress.networkSystem != NS_LOCAL)
        Ply1.AIPlayerLevel = AI_MAX_LEVELS;
    }

    AmountToWrite = sizeof (Ply1);

    AmountWritten = mmioWrite (RIFFFile, (char *) &Ply1, AmountToWrite);
    if (AmountWritten != AmountToWrite) goto FAILURE_EXIT;

    MMErrorCode = mmioAscend (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

    /* Finish the enclosing RIFF/PLYR chunk. */

    MMErrorCode = mmioAscend (RIFFFile, &SubRIFFChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;
  }

  /* Write the RIFF/SQRE chunk.  Saved square information for some squares. */

  for (SquareNo = 0; SquareNo <= SQ_BOARDWALK; SquareNo++)
  {
    SquarePntr = SaveStatePntr->Squares + SquareNo;

    memset (&SubRIFFChunk, 0, sizeof (SubRIFFChunk));
    SubRIFFChunk.fccType = MAKEFOURCC ('S', 'Q', 'R', 'E');
    MMErrorCode = mmioCreateChunk (RIFFFile, &SubRIFFChunk, MMIO_CREATERIFF);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

    /* Write the NAME chunk.  Name of the square in Unicode, NUL at end of
       string.  This is just used for debugging the data file, nothing else. */

    memset (&SubChunk, 0, sizeof (SubChunk));
    SubChunk.ckid = MAKEFOURCC ('N', 'A', 'M', 'E');
    MMErrorCode = mmioCreateChunk (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

    AmountToWrite = (wcslen (RULE_SquarePredefinedInfo[SquareNo].squareName) + 1) * sizeof (wchar_t);

    AmountWritten = mmioWrite (RIFFFile, (char *) RULE_SquarePredefinedInfo[SquareNo].squareName, AmountToWrite);
    if (AmountWritten != AmountToWrite) goto FAILURE_EXIT;

    MMErrorCode = mmioAscend (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

    /* Write the SQR1 chunk.  Basic square information. */

    memset (&SubChunk, 0, sizeof (SubChunk));
    SubChunk.ckid = MAKEFOURCC ('S', 'Q', 'R', '1');
    MMErrorCode = mmioCreateChunk (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

    Sqr1.squareNumber = SquareNo;
    Sqr1.owner = SquarePntr->owner;
    Sqr1.houses = SquarePntr->houses;
    Sqr1.mortgaged = SquarePntr->mortgaged;
    AmountToWrite = sizeof (Sqr1);

    AmountWritten = mmioWrite (RIFFFile, (char *) &Sqr1, AmountToWrite);
    if (AmountWritten != AmountToWrite) goto FAILURE_EXIT;

    MMErrorCode = mmioAscend (RIFFFile, &SubChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

    /* Finish the enclosing RIFF/SQRE chunk. */

    MMErrorCode = mmioAscend (RIFFFile, &SubRIFFChunk, 0);
    if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;
  }

  /* Write the SVG1 chunk.  Saved game general information. */

  memset (&SubChunk, 0, sizeof (SubChunk));
  SubChunk.ckid = MAKEFOURCC ('S', 'V', 'G', '1');
  MMErrorCode = mmioCreateChunk (RIFFFile, &SubChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  Svg1.GameDurationInSeconds = SaveStatePntr->GameDurationInSeconds;
  for (i = 0; i < 2; i++)
  {
    Svg1.Dice[i] = SaveStatePntr->Dice[i];
    Svg1.NextDice[i] = SaveStatePntr->NextDice[i];
    Svg1.UtilityDice[i] = SaveStatePntr->UtilityDice[i];
  }
  Svg1.NumberOfPlayers = SaveStatePntr->NumberOfPlayers;
  Svg1.CurrentPlayer = SaveStatePntr->CurrentPlayer;
  Svg1.NumberOfDoublesRolled = SaveStatePntr->NumberOfDoublesRolled;
  Svg1.JustRolledOutOfJail = SaveStatePntr->JustRolledOutOfJail;
  Svg1.PendingCard = SaveStatePntr->PendingCard;
  Svg1.NumberOfPendingPhases = SaveStatePntr->NumberOfPendingPhases;
  AmountToWrite = sizeof (Svg1);

  AmountWritten = mmioWrite (RIFFFile, (char *) &Svg1, AmountToWrite);
  if (AmountWritten != AmountToWrite) goto FAILURE_EXIT;

  MMErrorCode = mmioAscend (RIFFFile, &SubChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  /* Wrap it up - end the main RIFF chunk. */

  MMErrorCode = mmioAscend (RIFFFile, &RIFFChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR) goto FAILURE_EXIT;

  return RIFFChunk.cksize + 8 /* For RIFF header itself */;

FAILURE_EXIT:
  return 0;
}



/*****************************************************************************
 * Deallocates the collected AI save game info.
 */

static void FreeAISaveGameInfo (void)
{
  RULE_PlayerNumber PlayerNo;

  for (PlayerNo = 0; PlayerNo < RULE_MAX_PLAYERS; PlayerNo++)
  {
    if (CollectedAISaveStates[PlayerNo].AIStateDataPntr != NULL)
    {
      GlobalFreePtr (CollectedAISaveStates[PlayerNo].AIStateDataPntr);
      CollectedAISaveStates[PlayerNo].AIStateDataPntr = NULL;
    }
  }
}



/*****************************************************************************
 * Format the game state state into a RIFF file and put them in the
 * blob in the given message.  Returns TRUE if successful.
 */

BOOL AddGameStateToMessageBlob (RULE_GameStatePointer SaveStatePntr,
SavedGameDataCollectionPointer SavedAIStatesArray,
RULE_ActionArgumentsPointer MessagePntr)
{
  DWORD             FileSize;
  RULE_CharHandle   MemoryHandle = NULL;
  BYTE             *MemoryPntr = NULL;
  MMIOINFO          MMIOInfoRec;
  BOOL              ReturnCode = FALSE;
  HMMIO             RIFFFile = NULL;

  if (SaveStatePntr == NULL || MessagePntr == NULL ||
  MessagePntr->binaryDataHandleA != NULL)
    return FALSE;

  /* Allocate memory to store the binary large object. */

  MemoryHandle = RULE_AllocateHandle (1024 /* Initial size, can be grown later */);
  if (MemoryHandle == NULL)
    goto FAILURE_EXIT;

  MemoryPntr = (unsigned char *) RULE_LockHandle (MemoryHandle);
  if (MemoryPntr == NULL)
    goto FAILURE_EXIT;

  /* Open a MMIO stream on the buffer in growth mode. */

  memset (&MMIOInfoRec, 0, sizeof (MMIOInfoRec));
  MMIOInfoRec.fccIOProc = FOURCC_MEM;
  MMIOInfoRec.pchBuffer = (char *) MemoryPntr;
  MMIOInfoRec.cchBuffer = 1024; /* Initial size */
  MMIOInfoRec.adwInfo[0] = 4096; /* Size increment */
  RIFFFile = mmioOpen (NULL, &MMIOInfoRec,
    MMIO_READWRITE /* Often want to read what we wrote */ | MMIO_CREATE);
  if (RIFFFile == NULL) goto FAILURE_EXIT;

  FileSize = ConvertGameStateToRIFF (SaveStatePntr, SavedAIStatesArray, RIFFFile);

  mmioClose (RIFFFile, 0);
  RIFFFile = NULL;

  /* Unlock the buffer before handover, also pointer invalid after buffer growth. */

  RULE_UnlockHandle (MemoryHandle);
  MemoryPntr = NULL;

  if (FileSize < 12) /* RIFF files are at least 12 bytes long. */
    goto FAILURE_EXIT;

  /* Use a message directly so we can set the size explicitly,
     not just using the allocated size which will be too big. */

  MessagePntr->binaryDataHandleA = MemoryHandle;
  MemoryHandle = NULL; /* Message queue takes care of it now. */
  MessagePntr->binaryDataSizeA = FileSize;

  ReturnCode = TRUE;

FAILURE_EXIT:
  if (RIFFFile != NULL)
  {
    mmioClose (RIFFFile, 0);
    RIFFFile = NULL;
  }
  if (MemoryPntr != NULL)
    RULE_UnlockHandle (MemoryHandle);
  if (MemoryHandle != NULL)
    RULE_FreeHandle (MemoryHandle);
  return ReturnCode;
}



/*****************************************************************************
 * Utility function to collect the game status and send it to the lobby,
 * if there is a lobby network connection.  If you specify a game over
 * type which isn't GAME_RUNNING then it will set the current player to
 * the one you specify as the player.
 */

static void SendGameStatusToLobby (MESS_ZoneGameStatus GameOverType,
RULE_PlayerNumber WinningPlayer)
{
  long                                          NetWealth;
  RULE_PlayerNumber                             PlayerNo;
  RULE_PlayerInfoPointer                        RulePlayerPntr;
  RULE_SquareType                               SquareNo;
  RULE_SquareInfoPointer                        RuleSquarePntr;
  MESS_ZoneGameUpdateInfoRecord                 ZoneInfo;
  MESS_ZoneGameUpdateInfoRecord::PlayerStruct  *ZonePlayerPntr;
  MESS_ZoneGameUpdateInfoRecord::SquareStruct  *ZoneSquarePntr;

  if (!MESS_GameStartedByLobby)
    return; // Nothing to do if no lobby.

  memset (&ZoneInfo, 0, sizeof (ZoneInfo));

  // Game over info, a variation of the usual game status info.

  ZoneInfo.gameStatus = GameOverType;
  if (GameOverType == GAME_RUNNING)
    ZoneInfo.currentPlayer = CurrentRulesState.CurrentPlayer;
  else
    ZoneInfo.currentPlayer = WinningPlayer;

  ZoneInfo.numberOfPlayers = CurrentRulesState.NumberOfPlayers;
  ZoneInfo.language = MDEF_CurrentLanguage;

  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    RulePlayerPntr = CurrentRulesState.Players + PlayerNo;
    ZonePlayerPntr = ZoneInfo.players + PlayerNo;

    // Copy information all players, even bankrupt ones, have in common.

    wcsncpy (ZonePlayerPntr->name, RulePlayerPntr->name, RULE_MAX_PLAYER_NAME_LENGTH);
    ZonePlayerPntr->currentSquare = RulePlayerPntr->currentSquare;
    ZonePlayerPntr->token = RulePlayerPntr->token;
    ZonePlayerPntr->aiPlayer = RulePlayerPntr->AIPlayerLevel;

    if (RulePlayerPntr->currentSquare < SQ_OFF_BOARD)
    {
      ZonePlayerPntr->cash = RulePlayerPntr->cash;

      // Calculate net wealth for game winning conditions, which
      // includes the full price of the houses.

      NetWealth = RulePlayerPntr->cash;
      for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
      {
        RuleSquarePntr = CurrentRulesState.Squares + SquareNo;

        if (RuleSquarePntr->owner == PlayerNo)
        {
          NetWealth += RuleSquarePntr->mortgaged ?
            RULE_SquarePredefinedInfo[SquareNo].mortgageCost :
            RULE_SquarePredefinedInfo[SquareNo].purchaseCost;
          NetWealth += RuleSquarePntr->houses *
            RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost;
        }
      }
      ZonePlayerPntr->netWealth = NetWealth;

      // Count the get out of jail cards.

      if (CurrentRulesState.Cards[CHANCE_DECK].jailOwner == PlayerNo)
        ZonePlayerPntr->jailCards++;
      if (CurrentRulesState.Cards[COMMUNITY_DECK].jailOwner == PlayerNo)
        ZonePlayerPntr->jailCards++;
    }
  }

  // Now collect information on the real squares.

  for (SquareNo = 0; SquareNo < 40; SquareNo++)
  {
    RuleSquarePntr = CurrentRulesState.Squares + SquareNo;
    ZoneSquarePntr = ZoneInfo.squares + SquareNo;

    ZoneSquarePntr->owner = RuleSquarePntr->owner;
    if (RuleSquarePntr->houses >= CurrentRulesState.GameOptions.housesPerHotel)
      ZoneSquarePntr->houses = -1; // Mark it as a hotel.
    else
      ZoneSquarePntr->houses = RuleSquarePntr->houses;
    ZoneSquarePntr->mortgaged = RuleSquarePntr->mortgaged;
    ZoneSquarePntr->ownable = (RULE_SquarePredefinedInfo[SquareNo].group < SG_MAX_PROPERTY_GROUPS);
  }

  MESS_UpdateLobbyWithCurrentGameInfo (&ZoneInfo);
}



/*****************************************************************************
 * Utility function for pushing a new phase on the phase stack.  Returns
 * TRUE if successful, FALSE if the stack is full (doesn't push it then).
 */

static BOOL PushPhase (RULE_GamePhase Phase, RULE_PlayerNumber FromPlayer,
RULE_PlayerNumber ToPlayer, long Amount)
{
  int                 i;
  RULE_PendingPhasePointer NewPhasePntr;

  if (CurrentRulesState.NumberOfPendingPhases >= RULE_MAX_PENDING_PHASES)
  {
#if _DEBUG
    DBUG_DisplayNonFatalErrorMessage ("PushPhase: Too many pending phases.");
#endif
    return FALSE;
  }

  /* Shuffle down all the other elements in the stack.  Also keep the stacked
     states in matching order (yeah, kind of slow moving all that data). */

  for (i = CurrentRulesState.NumberOfPendingPhases; i >= 1; i--)
  {
    CurrentRulesState.phaseStack[i] = CurrentRulesState.phaseStack[i-1];

    if (StackedRulesStates[i-1].ThisStateIsValid)
      StackedRulesStates[i] = StackedRulesStates[i-1]; /* Copy saved state. */
    else /* No need to copy all of it. */
      StackedRulesStates[i].ThisStateIsValid = FALSE;
  }
  CurrentRulesState.NumberOfPendingPhases++;

  NewPhasePntr = CurrentRulesState.phaseStack + 0 /* Top-of-stack element */;
  NewPhasePntr->phase = Phase;
  NewPhasePntr->fromPlayer = FromPlayer;
  NewPhasePntr->toPlayer = ToPlayer;
  NewPhasePntr->amount = Amount;

  /* By default there is no associated state information with a pushed
     phase.  This should save some CPU time copying those 2K state records. */

  StackedRulesStates[0].ThisStateIsValid = FALSE;

#if MESS_REPORT_ACTION_MESSAGES
  MESS_RULE_updateMessageLog( NewPhasePntr, 2/*push*/ );
#endif

  return TRUE;
}



/*****************************************************************************
 * Utility function for popping a phase from the phase stack.  Returns
 * TRUE if successful.  Stack may be empty after calling this function.
 */

static BOOL PopPhase (void)
{
  int i;

  if (CurrentRulesState.NumberOfPendingPhases < 1)
  {
#if _DEBUG
    DBUG_DisplayNonFatalErrorMessage ("PopPhase: Stack already empty, no more phases left!");
#endif
    /* Add a dummy phase so we don't access garbage. */
    CurrentRulesState.NumberOfPendingPhases = 0;
    PushPhase (GF_ADDING_NEW_PLAYERS, 0, 0, 0);
    return FALSE;
  }

  CurrentRulesState.NumberOfPendingPhases--;
  for (i = 0; i < CurrentRulesState.NumberOfPendingPhases; i++)
  {
    CurrentRulesState.phaseStack[i] = CurrentRulesState.phaseStack[i+1];

    if (StackedRulesStates[i+1].ThisStateIsValid)
      StackedRulesStates[i] = StackedRulesStates[i+1];
    else
      StackedRulesStates[i].ThisStateIsValid = FALSE;
  }

#if MESS_REPORT_ACTION_MESSAGES
  MESS_RULE_updateMessageLog( CurrentRulesState.phaseStack, 0/*pop*/ );
#endif

  return TRUE;
}



/*****************************************************************************
 * Utility function for popping and restarting the popped phase.
 */

static BOOL PopAndRestartPhase (void)
{
  BOOL  ReturnCode;

  ReturnCode = PopPhase ();
  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);
  return ReturnCode;
}



/*****************************************************************************
 * Switch the current phase to a new one.  Functionally identical to popping
 * the old phase and pushing the new one, but a lot faster.  Well, not quite
 * the same - it keeps the saved state record intact instead of clearing it.
 */

static void SwitchPhase (RULE_GamePhase Phase, RULE_PlayerNumber FromPlayer,
RULE_PlayerNumber ToPlayer, long Amount)
{
  RULE_PendingPhasePointer NewPhasePntr;

  if (CurrentRulesState.NumberOfPendingPhases < 1)
  {
#if _DEBUG
    DBUG_DisplayNonFatalErrorMessage ("SwitchPhase: Stack empty!");
#endif
    CurrentRulesState.NumberOfPendingPhases = 1;
  }
  NewPhasePntr = CurrentRulesState.phaseStack + 0 /* Top-of-stack element */;
  NewPhasePntr->phase = Phase;
  NewPhasePntr->fromPlayer = FromPlayer;
  NewPhasePntr->toPlayer = ToPlayer;
  NewPhasePntr->amount = Amount;

#if MESS_REPORT_ACTION_MESSAGES
  MESS_RULE_updateMessageLog( NewPhasePntr, 1/*switch*/ );
#endif

}



/*****************************************************************************
 * Send the game state that you need to resynchronise a client.  Just the
 * basic stuff, not the internal things that a client doesn't need.
 */

static void SendClientResyncGameState (RULE_PlayerNumber ToPlayer,
RULE_ResyncCauses ResyncCause)
{
  RULE_CardDeckTypes          DeckNo;
  RULE_PlayerNumber           PlayerNo;
  RULE_ClientResyncInfoRecord ResyncInfo;
  RULE_SquareType             SquareNo;
  RULE_SquareInfoPointer      SquarePntr;

  memset (&ResyncInfo, 0, sizeof (ResyncInfo));  /* Default value is zero. */

  ResyncInfo.currentPlayer = CurrentRulesState.CurrentPlayer;

  ResyncInfo.resyncCause = ResyncCause;

  ResyncInfo.gamePhase = CurrentPhaseM;

  for (SquareNo = 0; SquareNo <= SQ_BOARDWALK; SquareNo++)
  {
    SquarePntr = CurrentRulesState.Squares + SquareNo;

    if (SquarePntr->owner < RULE_MAX_PLAYERS)
    {
      ResyncInfo.propertiesOwned[SquarePntr->owner] |=
        RULE_PropertyToBitSet (SquareNo);

      if (SquarePntr->mortgaged)
        ResyncInfo.mortgagedProperties |=
        RULE_PropertyToBitSet (SquareNo);

      ResyncInfo.houseCounts[SquareNo] = SquarePntr->houses;
    }
  }

  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    ResyncInfo.cash[PlayerNo] = CurrentRulesState.Players[PlayerNo].cash;
    ResyncInfo.playerSquare[PlayerNo] = CurrentRulesState.Players[PlayerNo].currentSquare;

    if (CurrentRulesState.Players[PlayerNo].firstMoveMade)
      ResyncInfo.firstMovesMade |= (1 << PlayerNo);
  }

  for (DeckNo = 0; DeckNo < MAX_DECK_TYPES; DeckNo++)
    ResyncInfo.jailOwners [DeckNo] = CurrentRulesState.Cards[DeckNo].jailOwner;

#if _DEBUG
  if (sizeof (ResyncInfo) > sizeof (wchar_t) * RULE_ACTION_ARGUMENT_STRING_LENGTH)
    DBUG_DisplayNonFatalErrorMessage ("SendClientResyncGameState: State info too big to fit in message.");
#endif

  MESS_SendAction (NOTIFY_CLIENT_RESYNC_INFO, RULE_BANK_PLAYER, ToPlayer, 0, 0, 0, 0,
    (unsigned short *) &ResyncInfo, sizeof (ResyncInfo), NULL);
}



/*****************************************************************************
 * Saves the current game state in the spot that matches the current phase
 * in the phase stack.
 */

static void SaveGameStateInCurrentPhase (void)
{
  memcpy (StackedRulesStates + 0, &CurrentRulesState, sizeof (RULE_GameStateRecord));
  StackedRulesStates[0].ThisStateIsValid = TRUE;
}



/*****************************************************************************
 * Restores the current game state from the saved state corresponding to the
 * current phase stack entry.  Presumably the state was saved while the
 * current phase was at the top of the stack, so you'll have to pop it off
 * if you want to get rid of it.
 */

static void RestoreGameStateFromCurrentPhase (RULE_ResyncCauses ResyncCause)
{
  memcpy (&CurrentRulesState, StackedRulesStates + 0, sizeof (RULE_GameStateRecord));
  StackedRulesStates[0].ThisStateIsValid = FALSE;
#if MESS_REPORT_ACTION_MESSAGES
  MESS_STATE_updateMessageLog( ResyncCause ); // Log the restore.
#endif

  /* Resend only information that usually changes, assume player names
     are unchanged etc. */

  SendClientResyncGameState (RULE_ALL_PLAYERS, ResyncCause);
}



/*****************************************************************************
 * Got an action in the wrong phase, display the generic error message.
 */

static void ErrorWrongPhase (RULE_ActionArgumentsPointer BadAction)
{
   RULE_PlayerNumber  ReportErrorToPlayer;

#if RULE_REPORT_BAD_MESSAGES
  sprintf (LE_ERROR_DebugMessageBuffer,
    "\r\nRULE Wrong Phase Message: action:%d A:%d B:%d C:%d D:%d E:%d\r\n",
    BadAction->action, BadAction->numberA, BadAction->numberB,
    BadAction->numberC, BadAction->numberD, BadAction->numberE);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    BadAction->action,
    FALSE, /* Failed something.  Let the AIs know. */
    BadAction->fromPlayer,
    0,
    NULL, 0, NULL);

  if (BadAction->fromPlayer != RULE_BANK_PLAYER)
  {
    ReportErrorToPlayer = BadAction->fromPlayer;
    if (ReportErrorToPlayer >= RULE_MAX_PLAYERS)
      ReportErrorToPlayer = RULE_ALL_PLAYERS;

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, ReportErrorToPlayer,
      TMN_ERROR_WRONG_PHASE, BadAction->action, BadAction->fromPlayer, CurrentPhaseM,
      NULL, 0, NULL);

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
  }
}



/*****************************************************************************
 * Got an action from the wrong player, display the generic error message.
 */

static void ErrorWrongPlayer (RULE_ActionArgumentsPointer BadAction)
{
   RULE_PlayerNumber  ReportErrorToPlayer;

#if RULE_REPORT_BAD_MESSAGES
  sprintf (LE_ERROR_DebugMessageBuffer,
    "\r\nRULE Wrong Player Message: action:%d A:%d B:%d C:%d D:%d E:%d\r\n",
    BadAction->action, BadAction->numberA, BadAction->numberB,
    BadAction->numberC, BadAction->numberD, BadAction->numberE);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    BadAction->action,
    FALSE, /* Failed something.  Let the AIs know. */
    BadAction->fromPlayer,
    0,
    NULL, 0, NULL);

  if (BadAction->fromPlayer != RULE_BANK_PLAYER)
  {
    ReportErrorToPlayer = BadAction->fromPlayer;
    if (ReportErrorToPlayer >= RULE_MAX_PLAYERS)
      ReportErrorToPlayer = RULE_ALL_PLAYERS;

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, ReportErrorToPlayer,
      TMN_ERROR_WRONG_PLAYER, BadAction->action, BadAction->fromPlayer, CurrentPhaseM,
      NULL, 0, NULL);

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
  }
}



/*****************************************************************************
 * Display an umimplemented message.
 */

static void ErrorUnimplemented (char *String)
{
  wchar_t TempString [80];

  mbstowcs (TempString, String, sizeof (TempString) / sizeof (wchar_t));

  MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    TMN_ERROR_UNIMPLEMENTED, 0, 0, 0,
    TempString, 0, NULL);
}



/*****************************************************************************
 * Notifies everybody about the given player's new cash total and the
 * change.  Hides the notification from appropriate players.  Well,
 * not any more.
 */

static void NotifyCashChange (RULE_PlayerNumber Player, long Delta)
{
  RULE_PlayerInfoPointer PlayerPntr = CurrentRulesState.Players + Player;

  MESS_SendAction (NOTIFY_CASH_AMOUNT, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    Player, Delta, PlayerPntr->cash, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * Moves some cash between players or the bank.  Doesn't do any checking.
 * Notifies the relevant people about the changes.  If SendAnimationMessage
 * is TRUE then it also sends a NOTIFY_CASH_ANIMATION to make the client
 * show the cash being handed over.
 */

static void BlindlyTransferCash (RULE_PlayerNumber FromPlayer,
RULE_PlayerNumber ToPlayer, long Amount, BOOL SendAnimationMessage)
{
  RULE_CashAnimationRecord  CashInfo;
  int                       i;

#if _DEBUG
  if (Amount < 0)
    DBUG_DisplayNonFatalErrorMessage ("BlindlyTransferCash: Transfer amount is negative.");
#endif

  if (Amount <= 0)
    return; /* Do nothing for zero cash transfers. */

  if (FromPlayer < CurrentRulesState.NumberOfPlayers)
  {
    CurrentRulesState.Players[FromPlayer].cash -= Amount;
#if _DEBUG
    if (CurrentRulesState.Players[FromPlayer].cash < 0)
      DBUG_DisplayNonFatalErrorMessage ("BlindlyTransferCash: Cash is negative for FromPlayer.");
#endif
  }

  if (ToPlayer < CurrentRulesState.NumberOfPlayers)
    CurrentRulesState.Players[ToPlayer].cash += Amount;

  if (SendAnimationMessage)
  {
    memset (&CashInfo, 0, sizeof (CashInfo));
    for (i = 0; i < CurrentRulesState.NumberOfPlayers; i++)
      CashInfo.cashAfter[i] = CurrentRulesState.Players[i].cash;

    MESS_SendAction (NOTIFY_CASH_ANIMATION, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      FromPlayer,
      ToPlayer,
      Amount,
      0,
      (unsigned short *) &CashInfo, sizeof (CashInfo), NULL);
  }

  /* Send out the notifications of the amount of cash the players now have,
     after the animation notification. */

  if (FromPlayer < CurrentRulesState.NumberOfPlayers)
    NotifyCashChange (FromPlayer, Amount);

  if (ToPlayer < CurrentRulesState.NumberOfPlayers)
    NotifyCashChange (ToPlayer, Amount);
}



/*****************************************************************************
 * Transfer funds from someone (or the bank) to someone else (or the bank).
 * If the debt can't be filled from the player's cash supply, the player
 * will be given the option to raise funds by mortgaging and trading.
 *
 * Just pushes a debt phase on the stack.  You need to do the restart manually.
 */

static void StackDebt (RULE_PlayerNumber FromPlayer,
RULE_PlayerNumber ToPlayer, long Amount)
{
  PushPhase (GF_COLLECTING_PAYMENT, FromPlayer, ToPlayer, Amount);
}



/*****************************************************************************
 * Transfer funds from someone (or the bank) to someone else (or the bank).
 * If the debt can't be filled from the player's cash supply, the player
 * will be given the option to raise funds by mortgaging and trading.
 *
 * Pushes a debt phase on the stack and queues up a restart action.
 */

static void StackDebtAndRestart (RULE_PlayerNumber FromPlayer,
RULE_PlayerNumber ToPlayer, long Amount)
{
  StackDebt (FromPlayer, ToPlayer, Amount);

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * Transfers property from anyone to a given player via the RULE_ESCROW_PLAYER
 * system (so interest is charged on mortgaged properties and the bank will
 * auction stuff if the new owner goes bankrupt).  Can also transfer stuff
 * to the bank.  Remember to use a GF_TRANSFER_ESCROW_PROPERTY phase
 * to do the transfer.
 */

static void TransferProperty (RULE_SquareType SquareNo, RULE_PlayerNumber ToPlayer)
{
  RULE_SquareInfoPointer SquarePntr;

  SquarePntr = CurrentRulesState.Squares + SquareNo;

  if (ToPlayer >= CurrentRulesState.NumberOfPlayers ||
  CurrentRulesState.Players[ToPlayer].currentSquare >= SQ_OFF_BOARD)
  {
     /* Send it to the bank if it is going to a bankrupt player or
        to the bank.  The bank will auction it off later on. */

    MESS_SendAction (NOTIFY_SQUARE_OWNERSHIP, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      SquareNo, RULE_BANK_PLAYER, SquarePntr->owner, 0,
      NULL, 0, NULL);
    SquarePntr->owner = RULE_BANK_PLAYER;
    SquarePntr->offeredInTradeTo = RULE_NOBODY_PLAYER;

    if (SquarePntr->mortgaged) /* Bank unmortgages automatically. */
    {
      MESS_SendAction (NOTIFY_SQUARE_MORTGAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        SquareNo, FALSE, TRUE, 0, NULL, 0, NULL);
      SquarePntr->mortgaged = FALSE;
    }
  }
  else /* Transfer to another player via escrow. */
  {
    MESS_SendAction (NOTIFY_SQUARE_OWNERSHIP, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      SquareNo, RULE_ESCROW_PLAYER, SquarePntr->owner, 0,
      NULL, 0, NULL);
    SquarePntr->owner = RULE_ESCROW_PLAYER;
    SquarePntr->offeredInTradeTo = ToPlayer;
  }
}



/*****************************************************************************
 * Transfers a get out of jail card between players or between a
 * player and the bank.  Returns TRUE if successful.
 */

static BOOL TransferGetOutOfJail (RULE_PlayerNumber FromPlayer,
RULE_PlayerNumber ToPlayer, RULE_CardDeckTypes Deck)
{
  /* When it isn't owned, RULE_NOBODY_PLAYER gets the card. */

  if (FromPlayer >= RULE_MAX_PLAYERS)
    FromPlayer = RULE_NOBODY_PLAYER;

  if (ToPlayer >= RULE_MAX_PLAYERS)
    ToPlayer = RULE_NOBODY_PLAYER;

  if (Deck >= MAX_DECK_TYPES ||
  CurrentRulesState.Cards[Deck].jailOwner != FromPlayer)
    return FALSE;

  if (CurrentRulesState.Cards[Deck].jailOwner < RULE_MAX_PLAYERS &&
  ToPlayer == RULE_NOBODY_PLAYER) /* If going back in the deck. */
    ReturnCard (&CurrentRulesState, (Deck == CHANCE_DECK) ?
    CHANCE_GET_OUT_OF_JAIL_FREE : COMMUNITY_GET_OUT_OF_JAIL_FREE);

  CurrentRulesState.Cards[Deck].jailOwner = ToPlayer;
  CurrentRulesState.Cards[Deck].jailOfferedInTradeTo = RULE_NOBODY_PLAYER;

  MESS_SendAction (NOTIFY_JAIL_CARD_OWNERSHIP, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    CurrentRulesState.Cards[Deck].jailOwner, Deck, 0, 0,
    NULL, 0, NULL);

  return TRUE;
}



/*****************************************************************************
 * Add some money to the Free Parking Jackpot.  The correct way to do this
 * would be to have a "FreeParkingPot" player number and collect money
 * the usual way for it.  That way bankruptcies would work, etc.  But we
 * don't have time for that right now...
 */

static void AddMoneyToFreeParkingPot (long Amount)
{
  CurrentRulesState.FreeParkingJackpotAmount += Amount;

  if (CurrentRulesState.GameOptions.freeParkingPot)
    MESS_SendAction (NOTIFY_FREE_PARKING_POT, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.FreeParkingJackpotAmount, Amount, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * Wait for all currently active players to respond to an are-you-there
 * request.
 */

static void WaitForEverybodyReady (RULE_ActionType HintCode)
{
  RULE_PlayerNumber PlayerNo;
  RULE_PlayerSet    SetToWaitFor;

  SetToWaitFor = 0;
  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    if (CurrentRulesState.Players[PlayerNo].currentSquare < SQ_OFF_BOARD)
      SetToWaitFor |= (1 << PlayerNo);
  }

  PushPhase (GF_WAIT_FOR_EVERYBODY_READY, SetToWaitFor, (RULE_PlayerNumber) HintCode,
    CurrentRulesState.GameDurationInSeconds /* A serial number and timer */);

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * Build a house on the given square.  It's already been approved etc.
 */

static void BuildHouse (RULE_SquareType SquareNo)
{
  int Houses;

  Houses = ++CurrentRulesState.Squares[SquareNo].houses;
#if _DEBUG
  if (Houses > CurrentRulesState.GameOptions.housesPerHotel)
    DBUG_DisplayNonFatalErrorMessage ("BuildHouse: Built more houses than possible!");
#endif

  MESS_SendAction (NOTIFY_SQUARE_HOUSES, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    SquareNo, Houses, CurrentRulesState.GameOptions.housesPerHotel, 0,
    NULL, 0, NULL);
}



/*****************************************************************************
 * Sells all the buildings on the monopoly attached to the given square.
 * This is a desparation move players can do when there is a housing shortage
 * that prevents them from breaking down a hotel.  The money goes to the
 * square's current owner.
 */

static void SellAllBuildingsOnMonopoly (RULE_SquareType SquareNo)
{
  long              Cash;
  RULE_SquareGroups Group;
  int               Houses;
  int               i;
  RULE_PlayerNumber PlayerNo;

  PlayerNo = CurrentRulesState.Squares[SquareNo].owner;
  Group = RULE_SquarePredefinedInfo[SquareNo].group;
  Cash = 0;

  for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
  {
    if (RULE_SquarePredefinedInfo[i].group == Group)
    {
      Houses = CurrentRulesState.Squares[i].houses;
      CurrentRulesState.Squares[i].houses = 0;

      if (Houses != 0)
      {
        MESS_SendAction (NOTIFY_SQUARE_HOUSES, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          i, 0, CurrentRulesState.GameOptions.housesPerHotel, 0,
          NULL, 0, NULL);

        Cash += (Houses * RULE_SquarePredefinedInfo[i].housePurchaseCost + 1) / 2;
      }
    }
  }

  if (Cash > 0)
  {
    BlindlyTransferCash (RULE_BANK_PLAYER, PlayerNo,
    Cash, TRUE /* Show the cash transfer animation */);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_BUILDING_ALL_SOLD,
      PlayerNo,
      Cash,
      SquareNo,
      NULL, 0, NULL);
  }
}



/*****************************************************************************
 * Retransmit the whole current trade proposal to all the players.  Sends a
 * NOTIFY_TRADE_STARTED followed by NOTIFY_TRADE_ITEM for all the items.
 */

static void ResendTradeProposal (void)
{
  int i, j;

  /* Send the start of the trade info marker. */

  MESS_SendAction (NOTIFY_TRADE_STARTED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    CurrentRulesState.phaseStack[0].fromPlayer /* Proposer in both edit and accept phases */,
    TradeStatus, 0, 0, NULL, 0, NULL);

  /* Send the traded property squares info. */

  for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
  {
    if (CurrentRulesState.Squares[i].owner < RULE_MAX_PLAYERS &&
    CurrentRulesState.Squares[i].offeredInTradeTo != RULE_NOBODY_PLAYER)
      MESS_SendAction (NOTIFY_TRADE_ITEM, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        CurrentRulesState.Squares[i].owner,  /* From this player. */
        CurrentRulesState.Squares[i].offeredInTradeTo, /* To this one. */
        TIK_SQUARE, /* It's a square, Todd! */
        i, /* Which particular square. */
        NULL, 0, NULL);
  }

  /* Send the player's cash trading information. */

  for (i = 0; i < RULE_MAX_PLAYERS; i++)
    for (j = 0; j < RULE_MAX_PLAYERS; j++)
      if (CurrentRulesState.Players[i].shared.trading.cashGivenInTrade[j] > 0)
        MESS_SendAction (NOTIFY_TRADE_ITEM, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          i,  /* From this player. */
          j, /* To this one. */
          TIK_CASH,
          CurrentRulesState.Players[i].shared.trading.cashGivenInTrade[j],
          NULL, 0, NULL);

  /* Do the jail card trading. */

  for (i = 0; i < MAX_DECK_TYPES; i++)
    if (CurrentRulesState.Cards[i].jailOfferedInTradeTo != RULE_NOBODY_PLAYER)
      MESS_SendAction (NOTIFY_TRADE_ITEM, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        CurrentRulesState.Cards[i].jailOwner,
        CurrentRulesState.Cards[i].jailOfferedInTradeTo,
        TIK_JAIL_CARD,
        i,
        NULL, 0, NULL);

  /* And lastly the immunities. */

  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
    if (CurrentRulesState.CountHits[i].toPlayer != RULE_NOBODY_PLAYER &&
    CurrentRulesState.CountHits[i].tradedItem)
      MESS_SendAction2 (NOTIFY_TRADE_ITEM, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        CurrentRulesState.CountHits[i].fromPlayer,
        CurrentRulesState.CountHits[i].toPlayer,
        (CurrentRulesState.CountHits[i].hitType == CHT_RENT_IMMUNITY) ? TIK_IMMUNITY : TIK_FUTURE_RENT,
        CurrentRulesState.CountHits[i].hitCount,
        CurrentRulesState.CountHits[i].properties,
        NULL, 0, NULL);
}



/*****************************************************************************
 * Checks to see whether the given player is involved in the trade.  Just
 * goes through all the trade items and sees if the player is on the receiving
 * or sending end.
 */

static BOOL PlayerInvolvedInTrade (RULE_PlayerNumber PlayerNo)
{
  int i, j;

  for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
    if (CurrentRulesState.Squares[i].owner < RULE_MAX_PLAYERS &&
    CurrentRulesState.Squares[i].offeredInTradeTo != RULE_NOBODY_PLAYER)
    {
      if (CurrentRulesState.Squares[i].offeredInTradeTo == PlayerNo)
        return TRUE;
      if (CurrentRulesState.Squares[i].owner == PlayerNo)
        return TRUE;
    }

  for (i = 0; i < RULE_MAX_PLAYERS; i++)
    for (j = 0; j < RULE_MAX_PLAYERS; j++)
      if (CurrentRulesState.Players[i].shared.trading.cashGivenInTrade[j] > 0)
        if (i == PlayerNo || j == PlayerNo)
          return TRUE;

  for (i = 0; i < MAX_DECK_TYPES; i++)
    if (CurrentRulesState.Cards[i].jailOfferedInTradeTo != RULE_NOBODY_PLAYER)
    {
      if (CurrentRulesState.Cards[i].jailOfferedInTradeTo == PlayerNo)
        return TRUE;
      if (CurrentRulesState.Cards[i].jailOwner == PlayerNo)
        return TRUE;
    }

  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
    if (CurrentRulesState.CountHits[i].toPlayer != RULE_NOBODY_PLAYER &&
    CurrentRulesState.CountHits[i].tradedItem)
    {
      if (CurrentRulesState.CountHits[i].fromPlayer == PlayerNo)
        return TRUE;
      if (CurrentRulesState.CountHits[i].toPlayer == PlayerNo)
        return TRUE;
    }

  return FALSE;
}



/*****************************************************************************
 * Makes a list of all the squares a player can put a building on when in the
 * GF_PLACE_BUILDING phase.  So, if he is placing a hotel, only allow it
 * on squares were building would result in a hotel.
 */

static RULE_PropertySet PlaceBuildingLegalSquares (void)
{
  BOOL                    BuildingAHotel;
  LANG_TextMessageNumber  ErrorCode;
  RULE_PropertySet        LegalSet;
  RULE_SquareType         SquareNo;

  if (CurrentPhaseM != GF_PLACE_BUILDING)
    return 0;

  LegalSet = 0;
  for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
  {
    ErrorCode = RULE_TestBuildingPlacement (&CurrentRulesState,
      CurrentRulesState.phaseStack[0].fromPlayer,
      SquareNo,
      TRUE /* Adding a building */,
      &BuildingAHotel,
      NULL,
      NULL);

    if (ErrorCode == TMN_NO_MESSAGE) /* If ok to build there. */
    {
      /* If building there would use the kind of building the
         player has bought at the auction. */

      if ((CurrentRulesState.phaseStack[0].amount <= 0 && !BuildingAHotel) ||
      (CurrentRulesState.phaseStack[0].amount > 0 && BuildingAHotel))
      {
        LegalSet |= RULE_PropertyToBitSet (SquareNo);
      }
    }
  }

  return LegalSet;
}



/*****************************************************************************
 * Makes a list of all the players that are currently allowed to buy the
 * specified kind of building.  Tests for cash availability etc.  Results
 * are different in pre-auction or auction phases, or when placing a building.
 */

static RULE_PlayerSet PlayersWhoCanBuyABuilding (BOOL BuyAHotel)
{
  BOOL                    BuildingAHotel;
  LANG_TextMessageNumber  ErrorCode;
  RULE_PlayerSet          PlayersAllowed;
  RULE_PlayerNumber       PlayerNo;
  RULE_SquareType         SquareNo;

  PlayersAllowed = 0;
  for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
  {
    PlayerNo = CurrentRulesState.Squares[SquareNo].owner;
    if (PlayerNo < RULE_MAX_PLAYERS &&
    (PlayersAllowed & (1 << PlayerNo)) == 0)
    {
      ErrorCode = RULE_TestBuildingPlacement (&CurrentRulesState,
        PlayerNo,
        SquareNo,
        TRUE /* Adding a building */,
        &BuildingAHotel,
        NULL,
        NULL);

      if (ErrorCode == TMN_NO_MESSAGE && BuildingAHotel == BuyAHotel)
        PlayersAllowed |= (1 << PlayerNo);
    }
  }
  return PlayersAllowed;
}



/*****************************************************************************
 * Find the players who are allowed to bid in the current auction.
 */

static RULE_PlayerSet PlayersAllowedToBid (void)
{
  int             i;
  RULE_PlayerSet  PlayersAllowed;

  if (CurrentPhaseM != GF_AUCTION)
    return 0;

  /* Find out who is allowed to bid. */

  if (CurrentRulesState.shared.auction.propertyBeingAuctioned >= SQ_IN_JAIL)
  {
    /* Auctioning off a building.  Can they place that kind on the board? */

    PlayersAllowed = PlayersWhoCanBuyABuilding (CurrentRulesState.
    shared.auction.propertyBeingAuctioned == SQ_AUCTION_HOTEL);
  }
  else /* Auctioning a property - all nonbankrupts can bid. */
  {
    PlayersAllowed = 0;
    for (i = 0; i < CurrentRulesState.NumberOfPlayers; i++)
      if (CurrentRulesState.Players[i].currentSquare < SQ_OFF_BOARD)
        PlayersAllowed |= (1 << i);
  }
  return PlayersAllowed;
}



/*****************************************************************************
 * Initialises the auction specific data structures for the start of
 * an auction (no high bidder, reset clock).
 */

static void InitialiseAuctionDataAndRestart (RULE_PlayerNumber PlayerWhoStartedAuction)
{
  if (CurrentPhaseM != GF_AUCTION)
  {
#if _DEBUG
    DBUG_DisplayNonFatalErrorMessage ("InitialiseAuctionData: Not called in auction phase!");
#endif
    return;
  }

  CurrentRulesState.shared.auction.tickCount = 0;
  CurrentRulesState.shared.auction.goingCount = 0;
  CurrentRulesState.shared.auction.propertyBeingAuctioned =
    CurrentRulesState.phaseStack[0].amount;
  CurrentRulesState.shared.auction.highestBidder = RULE_BANK_PLAYER;
  CurrentRulesState.shared.auction.highestBid = 0;

#if RULE_SOMEBODY_WINS_AUCTION_HACK
  /* Make the person who started the auction win the auction even if they
     don't bid.  This is done since the animation system doesn't know what
     to do when nobody wins the auction. */

  if (PlayerWhoStartedAuction >= CurrentRulesState.NumberOfPlayers ||
  CurrentRulesState.Players[PlayerWhoStartedAuction].currentSquare >= SQ_OFF_BOARD)
  {
    RULE_PlayerNumber PlayerNo;

    for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
    {
      if (CurrentRulesState.Players[PlayerNo].currentSquare < SQ_OFF_BOARD)
      {
        PlayerWhoStartedAuction = PlayerNo;
        break;
      }
    }
  }

  /* Need to send the high bid of zero/bank player to start the auction,
     normally the restart will do it, but in this case the restart does
     the 1 dollar bid, so we fake it here. */

  MESS_SendAction2 (NOTIFY_NEW_HIGH_BID, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    CurrentRulesState.shared.auction.highestBidder,
    CurrentRulesState.shared.auction.highestBid,
    CurrentRulesState.shared.auction.propertyBeingAuctioned,
    0,
    PlayersAllowedToBid (),
    NULL, 0, NULL);

  CurrentRulesState.shared.auction.highestBid = 1;  /* Bid a buck to make it obvious. */
  CurrentRulesState.shared.auction.highestBidder = PlayerWhoStartedAuction;
#endif

  /* Yes to waiting after the first "going once" message to keep the AIs in synch. */

  IgnoreWaitForEverybodyReady = FALSE;

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * A utility function to find out how much money a player could raise if
 * he cashed in everything.  Doesn't include trading, or properties that
 * are held in escrow, just immediate cash-ins.
 */

static long CashPlayerCouldRaiseIfNeeded (RULE_PlayerNumber PlayerNo)
{
  long                        Cash;
  RULE_SquareType             SquareNo;
  RULE_SquareInfoPointer      SquarePntr;

  if (PlayerNo >= CurrentRulesState.NumberOfPlayers)
    return 0; /* This isn't a player. */

  if (CurrentRulesState.Players[PlayerNo].currentSquare >= SQ_OFF_BOARD)
    return 0; /* It is a bankrupt player. */

  Cash = CurrentRulesState.Players[PlayerNo].cash;

  /* Add on mortgagable squares and house sales. */

  for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
  {
    SquarePntr = CurrentRulesState.Squares + SquareNo;
    if (SquarePntr->owner == PlayerNo)
    {
      if (!SquarePntr->mortgaged)
        Cash += RULE_SquarePredefinedInfo[SquareNo].mortgageCost;

      Cash += SquarePntr->houses *
        ((RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost + 1) / 2);
    }
  }
  return Cash;
}



/*****************************************************************************
 * Count up the number of computer systems that don't have a Human player
 * assigned to them.  Includes the local system as well as all systems
 * connected over the network.  If networking is turned off, returns zero.
 */

static int CountSpectatorComputers (void)
{
  RULE_NetworkAddressRecord   ComputerAddress;
  int                         ComputerIndex;
  BOOL                        HumanFound;
  RULE_NetworkAddressRecord   HumanPlayerAddresses [RULE_MAX_PLAYERS];
  int                         i;
  int                         NumberOfHumanPlayers;
  RULE_PlayerNumber           PlayerNo;
  int                         SpectatorCount;

  if (!MESS_NetworkMode)
    return 0;

  /* Find the addresses of all the Human players for quick reference. */

  NumberOfHumanPlayers = 0;
  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    if (CurrentRulesState.Players[PlayerNo].AIPlayerLevel == 0)
    {
      MESS_GetAddressOfPlayer (PlayerNo, HumanPlayerAddresses + NumberOfHumanPlayers);
      NumberOfHumanPlayers++;
    }
  }

  /* Go through all computers and count the ones that don't have a Human. */

  ComputerIndex = 0;
  SpectatorCount = 0;
  while (MESS_GetComputerAddressFromIndex (ComputerIndex++, &ComputerAddress))
  {
    HumanFound = FALSE;
    for (i = 0; i < NumberOfHumanPlayers; i++)
    {
      if (memcmp (&ComputerAddress, HumanPlayerAddresses + i,
      sizeof (RULE_NetworkAddressRecord)) == 0)
      {
        HumanFound = TRUE;
        break;
      }
    }
    if (!HumanFound)
      SpectatorCount++;
  }

  return SpectatorCount;
}



/*****************************************************************************
 * Give N randomly picked available properties to each player.  Stops
 * when a player can't afford to buy any more.
 */

static void DealNProperties (void)
{
  RULE_SquareType             AffordableProperties [SQ_MAX_SQUARE_TYPES];
  BOOL                        AlreadyOwned;
  long                        Cash [RULE_MAX_PLAYERS];
  long                        CashOwed [RULE_MAX_PLAYERS];
  BOOL                        FinishedAFullRound;
  RULE_SquareType             HypotheticalSquarePurchase [RULE_MAX_PLAYERS];
  int                         NumberOfAffordableProperties;
  int                         NumberOfRounds;
  RULE_PlayerNumber           OtherPlayerNo;
  RULE_PlayerNumber           PlayerNo;
  RULE_PropertySet            PropertiesBought [RULE_MAX_PLAYERS];
  RULE_SquareType             SquareNo;

  /* We need to keep track of the cash, since it will be paid for later on
     in one lump sum (to avoid zillions of payments overflowing the state
     queue) once the game actually starts running. */

  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    Cash [PlayerNo] = CurrentRulesState.Players[PlayerNo].cash;
    CashOwed [PlayerNo] = 0;
    PropertiesBought [PlayerNo] = 0;
  }

  /* Dish out the properties in several rounds, each round gives one property
     to each player.  Stop when somebody can't buy a property (no cash or no
     more properties), and throw away the partial round (we want everybody to
     get the same number of properties). */

  NumberOfRounds = 0;
  while (NumberOfRounds < CurrentRulesState.GameOptions.dealNPropertiesAtStartup)
  {
    /* Start a new round. */

    FinishedAFullRound = TRUE;
    for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
      HypotheticalSquarePurchase [PlayerNo] = SQ_MAX_SQUARE_TYPES; /* Meaning none. */

    for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
    {
      /* Only select from the properties available that the player can afford. */

      NumberOfAffordableProperties = 0;
      for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
      {
        /* The square must be a property type of square that isn't already owned. */

        if (CurrentRulesState.Squares[SquareNo].owner != RULE_NOBODY_PLAYER ||
        RULE_SquarePredefinedInfo[SquareNo].group >= SG_MAX_PROPERTY_GROUPS)
          continue; /* This square is already owned or isn't a property square. */

        /* The square must not already be hypothetically sold to another player. */

        AlreadyOwned = FALSE;
        for (OtherPlayerNo = 0; OtherPlayerNo < CurrentRulesState.NumberOfPlayers; OtherPlayerNo++)
        {
          if (HypotheticalSquarePurchase [OtherPlayerNo] == SquareNo)
          {
            AlreadyOwned = TRUE;
            break;
          }
        }
        if (AlreadyOwned)
          continue; /* Can't buy this already allocated square, try next one. */

        /* The player must be able to afford this square. */

        if (!CurrentRulesState.GameOptions.dealFreePropertiesAtStartup &&
        RULE_SquarePredefinedInfo[SquareNo].purchaseCost > Cash [PlayerNo])
          continue;

        /* Yes, this one is a possible candidate for purchase! */

        AffordableProperties [NumberOfAffordableProperties] = SquareNo;
        NumberOfAffordableProperties++;
      }

      if (NumberOfAffordableProperties <= 0)
      {
        /* Ran out of properties.  Discard this partial round and just sell
           the stuff from the complete rounds. */

        FinishedAFullRound = FALSE;
        break;
      }

      /* Hypothetically buy this square for this player in this round. */

      SquareNo = AffordableProperties [rand () % NumberOfAffordableProperties];
      HypotheticalSquarePurchase [PlayerNo] = SquareNo;
    }

    if (!FinishedAFullRound)
      break; /* Only got a partial round.  Skip the rest. */

    /* Ok, we now have a hypothetical square for every player.  Update the
       local cash estimate and transfer their ownership to escrow for real. */

    for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
    {
      SquareNo = HypotheticalSquarePurchase [PlayerNo];
      TransferProperty (SquareNo, PlayerNo); /* Puts it in escrow ownership. */
      if (!CurrentRulesState.GameOptions.dealFreePropertiesAtStartup)
      {
        CashOwed [PlayerNo] += RULE_SquarePredefinedInfo[SquareNo].purchaseCost;
        Cash [PlayerNo] -= RULE_SquarePredefinedInfo[SquareNo].purchaseCost;
      }
      PropertiesBought [PlayerNo] |= RULE_PropertyToBitSet (SquareNo);
    }
    NumberOfRounds++;
  }

  /* Accounting time - pay for the properties they have bought. */

  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    if (PropertiesBought [PlayerNo])
      PushPhase (GF_TRANSFER_ESCROW_PROPERTY, 0, PlayerNo, PropertiesBought [PlayerNo]);

    if (CashOwed [PlayerNo])
      StackDebt (PlayerNo, RULE_BANK_PLAYER, CashOwed [PlayerNo]);
  }
}



/*****************************************************************************
 * Do the things that are needed for starting the game.  Shuffle cards,
 * deal out properties, initialise some data structures.  This is done
 * after we have the game options and have decided the player order.
 */

static void StartGameInitialisation (void)
{
  RULE_PlayerNumber           PlayerNo;
  RULE_SquareType             SquareNo;
  RULE_SquareInfoPointer      SquarePntr;

  InitialisePredefinedData (&CurrentRulesState.GameOptions);

  InitialiseDecks (&CurrentRulesState); /* Shuffle cards. */

  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    /* Give them some money. */

    CurrentRulesState.Players[PlayerNo].cash = CurrentRulesState.GameOptions.initialCash;

    /* Put their token on the GO square and mark them as not moved yet. */

    CurrentRulesState.Players[PlayerNo].currentSquare = SQ_GO;
    CurrentRulesState.Players[PlayerNo].firstMoveMade = FALSE;
  }

  /* Reset the square ownerships and other data. */

  for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
  {
    SquarePntr = CurrentRulesState.Squares + SquareNo;
    SquarePntr->owner = RULE_NOBODY_PLAYER;
    SquarePntr->offeredInTradeTo = RULE_NOBODY_PLAYER;
    SquarePntr->houses = 0;
    SquarePntr->mortgaged = 0;
        SquarePntr->game_earnings = 0;
  }

  CurrentRulesState.CurrentPlayer = 0; /* Starting player. */

  CurrentRulesState.GameDurationInSeconds = 0; /* Start the clock. */

  MESS_UpdateLobbyGameStarted ();

  SendClientResyncGameState (RULE_ALL_PLAYERS, RESYNC_GAME_START);

  MESS_SendAction (NOTIFY_GAME_STARTING, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    0, 0, 0, 0, NULL, 0, NULL);

  MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    TMN_ERROR_GAME_STARTING,
    0,
    CurrentRulesState.CurrentPlayer,
    0,
    NULL, 0, NULL);

  SwitchPhase (GF_WAIT_START_TURN, 0, 0, 0);

  /* Deal some property deeds to the players, if the options call for it. */

  DealNProperties ();

  /* Seed the Free Parking Jackpot with the seed money. */

  AddMoneyToFreeParkingPot (CurrentRulesState.GameOptions.freeParkingSeed);

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * Sort the players by their roll histories and then move them around to
 * their new starting order.
 */

static void SortPlayersByRollHistory (void)
{
  struct OrderStruct
  {
    RULE_PlayerNumber player;
    DWORD             rollHistory;
  } NewPlayerOrder [RULE_MAX_PLAYERS], TempOrder;

  RULE_NetworkAddressRecord OriginalNetworkAddresses [RULE_MAX_PLAYERS];
  RULE_PlayerInfoRecord     OriginalPlayerInfo [RULE_MAX_PLAYERS];
  RULE_PlayerNumber         OtherNo;
  RULE_PlayerNumber         PlayerNo;

  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    NewPlayerOrder[PlayerNo].player = PlayerNo;
    NewPlayerOrder[PlayerNo].rollHistory =
      CurrentRulesState.Players[PlayerNo].shared.ordering.diceRollHistory;
  }

  /* Do a bubble sort. */

  for (PlayerNo = CurrentRulesState.NumberOfPlayers - 1; PlayerNo >= 1; PlayerNo--)
  {
    for (OtherNo = 0; OtherNo < PlayerNo; OtherNo++)
    {
      if (NewPlayerOrder[OtherNo].rollHistory < NewPlayerOrder[OtherNo+1].rollHistory)
      {
        /* Swap the values, percolating the smaltest number to the back. */

        TempOrder = NewPlayerOrder [OtherNo];
        NewPlayerOrder [OtherNo] = NewPlayerOrder [OtherNo+1];
        NewPlayerOrder [OtherNo+1] = TempOrder;
      }
    }
  }

  /* Move the players around in the slots to match. */

  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    OriginalPlayerInfo[PlayerNo] = CurrentRulesState.Players[PlayerNo];
    MESS_GetAddressOfPlayer (PlayerNo, OriginalNetworkAddresses + PlayerNo);
  }

  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    CurrentRulesState.Players[PlayerNo] = OriginalPlayerInfo[NewPlayerOrder[PlayerNo].player];
    MESS_AssociatePlayerWithAddress (PlayerNo,
      OriginalNetworkAddresses + NewPlayerOrder[PlayerNo].player);
  }

  /* Tell the rest of the world about the new player order. */

  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    MESS_SendAction (NOTIFY_NAME_PLAYER, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      PlayerNo,
      CurrentRulesState.Players[PlayerNo].token,
      CurrentRulesState.Players[PlayerNo].colour,
      CurrentRulesState.Players[PlayerNo].AIPlayerLevel,
      CurrentRulesState.Players[PlayerNo].name, 0, NULL);
  }
}



/*****************************************************************************
 * This handles restarts in the GF_PICKING_STARTING_ORDER phase.  If the
 * entire order is ready, it rearranges the players and starts the game.
 * Otherwise, it looks for players who are in a tie and asks them to roll.
 */

static void AskForStartingOrderDiceRolls (void)
{
  DWORD                     DieValue;
  int                       NumberOfTiedPlayers;
  RULE_PlayerNumber         OtherNo;
  RULE_PlayerNumber         PlayerNo;
  DWORD                     TieDieValue;
  RULE_PlayerSet            TiePlayerSet;

  if (CurrentRulesState.phaseStack[0].fromPlayer != 0)
  {
    for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
    {
      if (CurrentRulesState.phaseStack[0].fromPlayer & (1 << PlayerNo))
      {
        /* Still need to ask this player for a roll before
           completing this tie roll-off. */

        CurrentRulesState.CurrentPlayer = PlayerNo;

        MESS_SendAction (NOTIFY_PLEASE_ROLL_DICE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          CurrentRulesState.CurrentPlayer,
          SQ_OFF_BOARD,
          0, 0, NULL, 0, NULL);

        return;
      }
    }
  }

  /* Need to decide if we have enough ordering info.  See if there are any
     ties remaining. */

  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    TieDieValue = CurrentRulesState.Players[PlayerNo].shared.ordering.diceRollHistory;
    NumberOfTiedPlayers = 0;
    TiePlayerSet = 0;

    for (OtherNo = 0; OtherNo < CurrentRulesState.NumberOfPlayers; OtherNo++)
    {
      DieValue = CurrentRulesState.Players[OtherNo].shared.ordering.diceRollHistory;
      if (DieValue == TieDieValue)
      {
        NumberOfTiedPlayers++; /* Can include the original player. */
        TiePlayerSet |= (1 << OtherNo);
      }
    }

    if (NumberOfTiedPlayers > 1 && (TieDieValue & 0xF) == 0)
    {
      /* Found a tie and there still is tie storage space in the history.
         So ask the players to roll again. */

      CurrentRulesState.phaseStack[0].fromPlayer = TiePlayerSet;

      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);

      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_TIE_ROLLOFF,
        NumberOfTiedPlayers - 1,
        PlayerNo,
        NumberOfTiedPlayers,
        NULL, 0, NULL);

      return;
    }
  }

  /* Ok, have the final order or something close enough.  Sort the players
     by their roll histories. */

  SortPlayersByRollHistory ();

  /* Start the game. */

  StartGameInitialisation ();
}



/*****************************************************************************
 * Randomly permute the players.  This is done to randomize the starting
 * order when the players aren't doing it manually with dice rolls.
 */

static void RandomizePlayerOrder ()
{
  RULE_PlayerNumber PlayerNo;

  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
    CurrentRulesState.Players[PlayerNo].shared.ordering.diceRollHistory =
      rand ();

  SortPlayersByRollHistory ();
}



/*****************************************************************************
 * Use up one "hit" on the immunity or future.  Deallocate it when depleted.
 * Notifies the clients about the change in the immunity.
 */

static void UseUpOneHit (CountHitSquareSetPointer CountHitPntr,
RULE_SquareType SquareHit)
{
  int Count;

  if (CountHitPntr == NULL)
    return;

  Count = CountHitPntr->hitCount - 1;
  if (Count < 0)
    Count = 0;
  CountHitPntr->hitCount = Count;

  MESS_SendAction2 ((CountHitPntr->hitType == CHT_RENT_IMMUNITY) ?
    NOTIFY_IMMUNITY_COUNT : NOTIFY_FUTURE_RENT_COUNT,
    RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    CountHitPntr->toPlayer,
    Count,
    SquareHit,
    CountHitPntr->fromPlayer,
    CountHitPntr->properties,
    NULL, 0, NULL);

  if (Count <= 0) /* Was last one used? */
    CountHitPntr->toPlayer = RULE_NOBODY_PLAYER;
}



/*****************************************************************************
 * Collects the rent money (pushes debt collection phases).  Checks for
 * immunities and rent futures too, and handles mortgaged squares and utility
 * rolls.  Assumes the current player landed on his square and is the one
 * paying the rent.
 */

static void CollectRent (void)
{
  CountHitSquareSetPointer  CountHitPntr;
  CountHitSquareSetPointer  FuturePntr = NULL;
  CountHitSquareSetPointer  ImmunityPntr = NULL;
  int                       i;
  RULE_SquareGroups         MonopolyGroup;
  BOOL                      NeedUtilityRoll = FALSE;
  RULE_PlayerNumber         Owner;
  RULE_PlayerNumber         LandingPlayer;
  long                      Rent = 0;
  RULE_PlayerNumber         RentToPlayerNo;
  RULE_PropertySet          SquareBit;
  RULE_SquareType           SquareNo;
  int                       SquaresOwnedInMonopoly;
  RULE_SquareInfoPointer    SquarePntr;
  int                       TotalSquaresInMonopoly;

  LandingPlayer = CurrentRulesState.CurrentPlayer;
  SquareNo = CurrentRulesState.Players[LandingPlayer].currentSquare;
  SquareBit = RULE_PropertyToBitSet (SquareNo);
  SquarePntr = CurrentRulesState.Squares + SquareNo;
  MonopolyGroup = RULE_SquarePredefinedInfo[SquareNo].group;
  Owner = SquarePntr->owner;

  if (!SquarePntr->mortgaged)
  {
    /* Figure out how much rent is charged.  How many other squares
       does the owner own in this group?  Also find out how big the
       group is.  Ownership also doesn't count when the square is
       mortgaged, unless American rules are in effect (then
       mortgaged counts towards ownership). */

    SquaresOwnedInMonopoly = 0;
    TotalSquaresInMonopoly = 0;
    for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
    {
      if (RULE_SquarePredefinedInfo[i].group == MonopolyGroup)
      {
        TotalSquaresInMonopoly++;

        if (CurrentRulesState.Squares[i].owner == Owner &&
        (!CurrentRulesState.Squares[i].mortgaged ||
        CurrentRulesState.GameOptions.mortgagedCountsInGroupRent))
          SquaresOwnedInMonopoly++;
      }
    }

    /* If it's an ordinary piece of land type of group.  Charge double
       rent for unimproved land in a monopoly, charge house and hotel
       rent for squares with buildings. */

    if (MonopolyGroup <= SG_PARK_PLACE)
    {
      Rent = RULE_SquarePredefinedInfo[SquareNo].rent[SquarePntr->houses];
      if (SquarePntr->houses == 0 &&
      SquaresOwnedInMonopoly == TotalSquaresInMonopoly)
        Rent += Rent; /* Double rent for undeveloped land in a monopoly. */
    }

    /* If it's a railroad, charge rent based on the number of other railroads
       owned.  Also watch out for the chance cards that double the rent charged. */

    if (MonopolyGroup == SG_RAILROAD)
    {
      Rent = RULE_SquarePredefinedInfo [SquareNo].rent[SquaresOwnedInMonopoly];

      if (CurrentRulesState.PendingCard == CHANCE_GO_TO_NEAREST_RAILROAD_PAY_DOUBLE_1 ||
      CurrentRulesState.PendingCard == CHANCE_GO_TO_NEAREST_RAILROAD_PAY_DOUBLE_2)
      {
        Rent += Rent;
        CurrentRulesState.PendingCard = NOT_A_CARD; /* Used the pending card. */
      }
    }

    /* For utilities, charge an amount based on the dice rolled and the number of
       utilities.  There's also a card that makes the lander roll separately. */

    if (MonopolyGroup == SG_UTILITY)
    {
      if (CurrentRulesState.PendingCard == CHANCE_GO_TO_NEAREST_UTILITY)
      {
        if (CurrentRulesState.UtilityDice[0] == 0)
        {
          /* If the utility roll hasn't been made yet.  Signal it so we exit
             early from the CollectRent subroutine to do the roll, then call
             CollectRent again when the roll is ready.  Needed because of
             interactions between futures/immunities and the need to roll. */

          NeedUtilityRoll = TRUE;
          Rent = 12345; /* Just a non-zero value. */
        }
        else /* Got a dice roll to use. */
        {
          CurrentRulesState.PendingCard = NOT_A_CARD; /* Used the pending card. */
          Rent = CurrentRulesState.UtilityDice[0] + CurrentRulesState.UtilityDice[1];
          Rent *= 10; /* Always 10 times for utility rolls. */
#if _DEBUG
          if (Rent <= 0)
            DBUG_DisplayNonFatalErrorMessage ("CollectRent: utility dice roll hasn't been made?");
#endif
          /* Used up the utility dice roll. */
          CurrentRulesState.UtilityDice[0] = CurrentRulesState.UtilityDice[1] = 0;
        }
      }
      else /* Ordinary landing on a utility.  Movement's dice roll times a factor. */
      {
        Rent = CurrentRulesState.Dice[0] + CurrentRulesState.Dice[1];

        if (SquaresOwnedInMonopoly <= 1)
          Rent *= 4; /* Four times roll if own just one utility. */
        else
          Rent *= 10; /* Ten times if both are owned. */
      }
    }
  }

  /* Check for futures that use this square.  A hit counts even if the square
     is mortgaged.  Futures and immunities can both apply to a square
     (relevant pointer not null if it was found).  Also, someone can be
     immune to a future. */

  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
  {
    CountHitPntr = CurrentRulesState.CountHits + i;

    if (CountHitPntr->toPlayer != RULE_NOBODY_PLAYER &&
    CountHitPntr->hitType == CHT_FUTURE_RENT &&
    (CountHitPntr->properties & SquareBit))// &&
//    CountHitPntr->fromPlayer == Owner)
    {
      FuturePntr = CountHitPntr;
      break;
    }
  }

  /* Figure out who gets the money. */

  if (FuturePntr != NULL)
    RentToPlayerNo = FuturePntr->toPlayer;
  else
    RentToPlayerNo = Owner;

  /* See if that rent collector has granted immunity from rent to the lander. */

  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
  {
    CountHitPntr = CurrentRulesState.CountHits + i;

    if (CountHitPntr->toPlayer == LandingPlayer &&
    CountHitPntr->hitType == CHT_RENT_IMMUNITY &&
    (CountHitPntr->properties & SquareBit))// &&
//    CountHitPntr->fromPlayer == RentToPlayerNo)
    {
      ImmunityPntr = CountHitPntr;
      break;
    }
  }

  /* Look for immunity or the rent collector is the lander. */

  if (LandingPlayer == RentToPlayerNo)
  {
    Rent = 0; /* Don't charge rent to yourself. */
    ImmunityPntr = NULL; /* Also don't use up your own immunities. */
  }

  if (ImmunityPntr != NULL)
    Rent = 0;

  /* Oops, go back and get the dice roll, then collect the rent again,
     if we were actually going to charge utility rent on someone who
     needs to roll for it. */

  if (NeedUtilityRoll)
  {
    if (Rent > 0)
    {
      PushPhase (GF_WAIT_UTILITY_ROLL, 0, 0, 0);
      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
      return;
    }
    else /* Not collecting rent, turns out we don't need to roll. */
      CurrentRulesState.PendingCard = NOT_A_CARD; /* Used the pending card. */
  }

  /* Charge the rent (stack up debt). */

  if (Rent > 0)
  {
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_RENT_INFO, Rent, RentToPlayerNo, SquareNo,
      NULL, 0, NULL);

    StackDebt (LandingPlayer, RentToPlayerNo, Rent);

        // add to the game_earnings variable
        UICurrentGameState.Squares[SquareNo].game_earnings += Rent;
        //SquarePntr->game_earnings += Rent;
  }

  /* Update the futures usage here, so that the player sees the futures
     message if there is one (it comes after the rent owed message).
     Gets counted every time someone lands on the square, no matter who
     (otherwise would be like an infinite immunity for the future holder). */

  if (FuturePntr != NULL)
  {
    UseUpOneHit (FuturePntr, SquareNo);

    MESS_SendAction2 (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_FUTURE_RENT_USED,
      RentToPlayerNo,
      Owner,
      SquareNo,
      (long) FuturePntr->hitCount,
      NULL, 0, NULL);
  }

  /* Display the immunities message last, since it is most important. */

  if (ImmunityPntr != NULL)
  {
    UseUpOneHit (ImmunityPntr, SquareNo);

    MESS_SendAction2 (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_IMMUNITY_USED,
      LandingPlayer,
      RentToPlayerNo,
      SquareNo,
      (long) ImmunityPntr->hitCount,
      NULL, 0, NULL);
  }
}



/*****************************************************************************
 * Predict who the next player to move will be.  If the current player isn't
 * going to move (not doubles) then it will be the next unbankrupt player.
 * Returns RULE_NOBODY_PLAYER if it can't figure it out.
 */

static RULE_PlayerNumber PredictPlayerWhoWillMoveNext (void)
{
  RULE_PlayerNumber PlayerNo;

  if (CurrentPhaseM != GF_WAIT_MOVE_ROLL &&
  CurrentPhaseM != GF_WAIT_JAIL_ROLL &&
  CurrentPhaseM != GF_WAIT_UTILITY_ROLL &&
  CurrentPhaseM != GF_WAIT_UNTIL_CARD_SEEN &&
  CurrentPhaseM != GF_JAIL_ROLL_OR_PAY_OR_CARD_DECISION &&
  CurrentPhaseM != GF_FLAT_OR_FRACTION_TAX_DECISION &&
  CurrentPhaseM != GF_AUCTION_OR_BUY_DECISION)
    return RULE_NOBODY_PLAYER;

  PlayerNo = CurrentRulesState.CurrentPlayer;
  if (PlayerNo >= CurrentRulesState.NumberOfPlayers)
    return RULE_NOBODY_PLAYER;

  /* If he rolled doubles, he definitely will go again (except in the get out
     of jail case, which we can safely ignore).  Or if he is just starting his
     turn (both dice zero). */

  if (CurrentRulesState.Dice[0] == CurrentRulesState.Dice[1])
    return PlayerNo;

  /* Ok, the current player's turn will be ending without any more moves
     (not counting community or chance cards here), the next active player
     will be the next one moving (well, unless he goes bankrupt in a house
     auction or debt collection). */

  while (TRUE)
  {
    PlayerNo++;
    if (PlayerNo >= CurrentRulesState.NumberOfPlayers)
      PlayerNo = 0; /* Wrapped around to the first player. */

    if (PlayerNo == CurrentRulesState.CurrentPlayer)
      break; /* Ran out of players, no other unbankrupt ones. */

    if (CurrentRulesState.Players[PlayerNo].currentSquare < SQ_OFF_BOARD)
      break;
  }
  return PlayerNo;
}



/*****************************************************************************
 * Predict the next movement given the current game state and next dice
 * roll.  Sends a NOTIFY_NEXT_MOVE message with the results.
 */

static void PredictNextMove (void)
{
  RULE_CardType       Card = NOT_A_CARD;
  RULE_CardDeckTypes  Deck;
  BOOL                DoublesRolled;
  RULE_ActionType     MovingAction = ACTION_NULL;
  RULE_PlayerNumber   MovingPlayer;
  RULE_SquareType     MovingFromSquare;
  RULE_SquareType     MovingToSquare = SQ_OFF_BOARD;
  RULE_PlayerNumber   PlayerNo;
  RULE_SquareType     SquareNo;
  unsigned char       TotalRolled;

  PlayerNo = CurrentRulesState.CurrentPlayer;
  if (PlayerNo >= CurrentRulesState.NumberOfPlayers) return;
  MovingPlayer = PlayerNo;
  SquareNo = CurrentRulesState.Players[PlayerNo].currentSquare;
  if (SquareNo >= SQ_OFF_BOARD) return;
  MovingFromSquare = SquareNo;

  /* See what the next dice roll will be. */

  if (CurrentRulesState.NextDice[0] == 0)
  {
    CurrentRulesState.NextDice[0] = (unsigned char) ((rand () % 6) + 1);
    rand(); rand (); rand (); /* Reduce chance of doubles. */
    CurrentRulesState.NextDice[1] = (unsigned char) ((rand () % 6) + 1);
  }

  TotalRolled = (unsigned char) (CurrentRulesState.NextDice[0] + CurrentRulesState.NextDice[1]);
  DoublesRolled = (CurrentRulesState.NextDice[0] == CurrentRulesState.NextDice[1]);

  if (CurrentPhaseM == GF_WAIT_UNTIL_CARD_SEEN)
  {
    /* If the player is picking up a card, look at the one he will be picking
       up.  If it isn't a moving card, then the next player's move is considered. */

    Deck = ((RULE_SquarePredefinedInfo[SquareNo].group == SG_COMMUNITY_CHEST) ?
      COMMUNITY_DECK : CHANCE_DECK);
    Card = CurrentRulesState.Cards[Deck].cardPile[0];
    switch (Card)
    {
    case COMMUNITY_GO_DIRECTLY_TO_JAIL:
    case CHANCE_GO_DIRECTLY_TO_JAIL:
      MovingAction = NOTIFY_JUMP_TO_SQUARE;
      MovingToSquare = SQ_IN_JAIL;
      break;

    case COMMUNITY_GO_DIRECTLY_TO_GO:
    case CHANCE_GO_DIRECTLY_TO_GO:
      MovingAction = NOTIFY_JUMP_TO_SQUARE;
      MovingToSquare = SQ_GO;
      break;

    case CHANCE_GO_TO_READING_RAILROAD:
      MovingAction = NOTIFY_MOVE_FORWARDS;
      MovingToSquare = SQ_READING_RR;
      break;

    case CHANCE_GO_TO_NEAREST_UTILITY:
      MovingAction = NOTIFY_MOVE_FORWARDS;
      if (SQ_ELECTRIC_COMPANY <= SquareNo && SquareNo < SQ_WATER_WORKS)
        MovingToSquare = SQ_WATER_WORKS;
      else
        MovingToSquare= SQ_ELECTRIC_COMPANY;
      break;

    case CHANCE_GO_TO_BOARDWALK:
      MovingAction = NOTIFY_MOVE_FORWARDS;
      MovingToSquare = SQ_BOARDWALK;
      break;

    case CHANCE_GO_TO_ST_CHARLES_PLACE:
      MovingAction = NOTIFY_MOVE_FORWARDS;
      MovingToSquare = SQ_ST_CHARLES_PLACE;
      break;

    case CHANCE_GO_TO_NEAREST_RAILROAD_PAY_DOUBLE_1:
    case CHANCE_GO_TO_NEAREST_RAILROAD_PAY_DOUBLE_2:
      MovingAction = NOTIFY_MOVE_FORWARDS;
      if (SquareNo >= SQ_SHORT_LINE_RR || SquareNo < SQ_READING_RR)
        MovingToSquare = SQ_READING_RR;
      else if (SquareNo < SQ_PENNSYLVANIA_RR)
        MovingToSquare = SQ_PENNSYLVANIA_RR;
      else if (SquareNo < SQ_BnO_RR)
        MovingToSquare = SQ_BnO_RR;
      else
        MovingToSquare = SQ_SHORT_LINE_RR;
      break;

    case CHANCE_GO_TO_ILLINOIS_AVENUE:
      MovingAction = NOTIFY_MOVE_FORWARDS;
      MovingToSquare = SQ_ILLINOIS_AVENUE;
      break;

    case CHANCE_GO_BACK_THREE_SPACES:
      MovingAction = NOTIFY_MOVE_BACKWARDS;
      MovingToSquare = SquareNo - 3;
      if (MovingToSquare < 0)
        MovingToSquare += (SQ_BOARDWALK + 1); /* Wrap around. */
      break;

    default:
      /* The card doesn't do any movement.  If the player has doubles, he will
         move next, otherwise some other player will be going. */
      PlayerNo = PredictPlayerWhoWillMoveNext ();
      if (PlayerNo < RULE_MAX_PLAYERS)
      {
        MovingPlayer = PlayerNo;
        MovingFromSquare = CurrentRulesState.Players[PlayerNo].currentSquare;
        if (MovingFromSquare < SQ_OFF_BOARD)
        {
          MovingAction = NOTIFY_MOVE_FORWARDS;
          MovingToSquare = MovingFromSquare + TotalRolled;
          if (MovingToSquare > SQ_BOARDWALK)
            MovingToSquare -= (SQ_BOARDWALK + 1); /* Wrap around when past last square. */
        }
      }
      break;
    }
  }
  else if (CurrentPhaseM == GF_WAIT_MOVE_ROLL ||
  CurrentPhaseM == GF_WAIT_JAIL_ROLL ||
  CurrentPhaseM == GF_JAIL_ROLL_OR_PAY_OR_CARD_DECISION)
  {
    /* We are still waiting for the current player to roll the dice. */

    if (DoublesRolled && CurrentRulesState.NumberOfDoublesRolled >= 2)
    {
      MovingAction = NOTIFY_JUMP_TO_SQUARE;
      MovingToSquare = SQ_IN_JAIL;
    }
    else /* Advance to a square. */
    {
      if (SquareNo == SQ_IN_JAIL)
        SquareNo = SQ_JUST_VISITING;
      MovingAction = NOTIFY_MOVE_FORWARDS;
      MovingToSquare = SquareNo + TotalRolled;
      if (MovingToSquare > SQ_BOARDWALK)
        MovingToSquare -= (SQ_BOARDWALK + 1); /* Wrap around when past last square. */
    }
  }
  else if (CurrentPhaseM == GF_FLAT_OR_FRACTION_TAX_DECISION ||
  CurrentPhaseM == GF_WAIT_UTILITY_ROLL ||
  CurrentPhaseM == GF_AUCTION_OR_BUY_DECISION)
  {
    /* The current player may have finished his turn,
       unless he rolled doubles last time. */

    PlayerNo = PredictPlayerWhoWillMoveNext ();
    if (PlayerNo < RULE_MAX_PLAYERS)
    {
      MovingPlayer = PlayerNo;
      MovingFromSquare = CurrentRulesState.Players[PlayerNo].currentSquare;
      if (MovingFromSquare < SQ_OFF_BOARD)
      {
        MovingAction = NOTIFY_MOVE_FORWARDS;
        MovingToSquare = MovingFromSquare + TotalRolled;
        if (MovingToSquare > SQ_BOARDWALK)
          MovingToSquare -= (SQ_BOARDWALK + 1); /* Wrap around when past last square. */
      }
    }
  }

  /* Ok, if the player will be landing on a community chest or chance square
     then describe the card that will be picked up. */

  if (RULE_SquarePredefinedInfo[MovingToSquare].group == SG_COMMUNITY_CHEST ||
  RULE_SquarePredefinedInfo[MovingToSquare].group == SG_CHANCE)
  {
    /* Watch out for the case where he is in the middle of picking up a chance
       card that moves back 3 squares to a community card.  Fortunately he
       picks up the second card from the other deck, never the same one,
       so we don't have to watch out for it. */

    if (RULE_SquarePredefinedInfo[MovingToSquare].group == SG_COMMUNITY_CHEST)
      Deck = COMMUNITY_DECK;
    else
      Deck = CHANCE_DECK;

    Card = CurrentRulesState.Cards[Deck].cardPile[0];
  }
  else /* Not landing on a chance or community chest square. */
    Card = NOT_A_CARD;

  if (MovingAction != ACTION_NULL)
  {
    MESS_SendAction2 (NOTIFY_NEXT_MOVE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      MovingAction, MovingToSquare, MovingFromSquare, MovingPlayer, Card, NULL, 0, NULL);
  }
}



/*****************************************************************************
 * Restart the current phase.  This is done when some other phase has
 * finished and we want to go back to an old phase.  It is also used when
 * a communications failure has lost data, it will resend the information
 * and requests needed for the current phase (but only some phases do
 * that kind of restart - the ones where we are waiting for the player and
 * send a notification message, other phases just ignore the restart).
 */

static void ActionRestartPhase (RULE_ActionArgumentsPointer NewActionPntr)
{
  BOOL                        Bankrupt;
  long                        Cash;
  int                         Count;
  CountHitSquareSetPointer    CurrentHitPntr;
  RULE_CardDeckTypes          Deck;
  int                         i, j;
  BOOL                        IsInvolved;
  RULE_PlayerSet              InvolvedSet;
  RULE_ActionArgumentsRecord  MyMessage;
  RULE_PlayerNumber           PlayerNo;
  RULE_PlayerInfoPointer      PlayerPntr;
  RULE_PlayerSet              PlayerSet;
  RULE_PropertySet            SomeProperties;
  RULE_SquareType             SquareNo;
  RULE_SquareInfoPointer      SquarePntr;
  CountHitSquareSetPointer    TestHitPntr;

  if (CurrentRulesState.NumberOfPendingPhases == 0)
  {
    /* If no pending phases, can't restart.  So, flush messages and
       start a new game. */
#if _DEBUG
    DBUG_DisplayNonFatalErrorMessage ("RestartPhase: No pending phases!");
#endif
    while (MESS_ReceiveActionMessage (&MyMessage))
      MESS_FreeStorageAssociatedWithMessage (&MyMessage);

    MESS_SendAction (ACTION_NEW_GAME, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      0, 0, 0, 0, NULL, 0, NULL);
    return;
  }

  PlayerPntr = CurrentRulesState.Players + CurrentRulesState.CurrentPlayer;
  Bankrupt = (PlayerPntr->currentSquare >= SQ_OFF_BOARD);

  switch (CurrentPhaseM)
  {
  case GF_ADDING_NEW_PLAYERS:
    MESS_SendAction (NOTIFY_PLEASE_ADD_PLAYERS, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      RULE_NO_SPECTATORS_ALLOWED ? CountSpectatorComputers () : 0,
      0, 0, 0, NULL, 0, NULL);

    MESS_SendAction (NOTIFY_NUMBER_OF_PLAYERS, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.NumberOfPlayers, 0, 0, 0, NULL, 0, NULL);

    for (i = 0; i < CurrentRulesState.NumberOfPlayers; i++)
    {
      MESS_SendAction (NOTIFY_NAME_PLAYER, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        i /* Player number */,
        CurrentRulesState.Players[i].token,
        CurrentRulesState.Players[i].colour,
        CurrentRulesState.Players[i].AIPlayerLevel,
        CurrentRulesState.Players[i].name, 0, NULL);
    }
    break;


  case GF_CONFIGURATION:
    PlayerSet = 0;  /* Make a set of the players who haven't accepted yet. */
    for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
    {
      if (!CurrentRulesState.Players[PlayerNo].shared.starting.acceptedConfiguration)
        PlayerSet |= (1 << PlayerNo);
    }

    /* Tell everybody about the new configuration. */

    memset (&MyMessage, 0, sizeof (MyMessage));
    MyMessage.action = NOTIFY_PROPOSED_CONFIGURATION;
    MyMessage.fromPlayer = RULE_BANK_PLAYER;
    MyMessage.toPlayer = RULE_ALL_PLAYERS;
    MyMessage.numberA = CurrentRulesState.shared.starting.proposer;
    MyMessage.numberB = PlayerSet;  /* Who hasn't accepted it yet. */
    MyMessage.numberC = 1;  /* Version of the host: 1 means Monopoly Star Wars which does futures. */

    if (AddGameOptionsToMessageBlob (&CurrentRulesState.GameOptions, &MyMessage))
      MESS_SendActionMessage (&MyMessage);

    /* If all players have accepted the configuration, start the game. */

    if (PlayerSet == 0)
    {
      if (CurrentRulesState.GameOptions.rollDiceToDecideStartingOrder)
      {
        for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
          CurrentRulesState.Players[PlayerNo].shared.ordering.diceRollHistory = 0;
        SwitchPhase (GF_PICKING_STARTING_ORDER, 0, 0, 0);
        MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
          0, 0, 0, 0, NULL, 0, NULL);
      }
      else
      {
        RandomizePlayerOrder ();
        StartGameInitialisation ();
      }
    }
    break;


  case GF_PICKING_STARTING_ORDER:
    AskForStartingOrderDiceRolls ();
    break;


  case GF_WAIT_START_TURN:
  case GF_MOVING_TOKEN:
      /* These phases don't do restarts, but may get one anyways.  The game also
        pauses in GF_WAIT_START_TURN until the message queue is empty. */
    break;


  case GF_WAIT_END_TURN:
    MESS_SendAction (NOTIFY_END_TURN, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.CurrentPlayer, 0, 0, 0, NULL, 0, NULL);
    break;


  case GF_PREROLL:
    if (NewActionPntr->fromPlayer != RULE_BANK_PLAYER)
      return; /* Ignore external restarts. */

    if (Bankrupt)
    {
      /* This guy went bankrupt, end his turn and move on to the next. */

      SwitchPhase (GF_WAIT_END_TURN, 0, 0, 0);
      // RK - New, dont end the turn, NOTIFY_END_OF_TURN.
      MESS_SendAction (NOTIFY_END_TURN, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        CurrentRulesState.CurrentPlayer, 0, 0, 0, NULL, 0, NULL);
      //MESS_SendAction (ACTION_END_TURN, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      //  0, 0, 0, 0, NULL, 0, NULL);
      break;
    }

    /* If the bank has properties it is waiting to auction off,
       auction them now. */

    SomeProperties = RULE_PropertySetOwnedByPlayer (&CurrentRulesState, RULE_BANK_PLAYER);
    if (SomeProperties != 0)
    {
      PushPhase (GF_AUCTION, 0, 0, RULE_BitSetToProperty (SomeProperties));
      InitialiseAuctionDataAndRestart (CurrentRulesState.CurrentPlayer);
      break;
    }

    if (PlayerPntr->currentSquare == SQ_IN_JAIL)
    {
      /* While in jail, either stay there (if you just arrived or
         failed to get out), or attempt to get out. */

      if (CurrentRulesState.Dice[0] == 0)  /* First roll? */
      {
        PushPhase (GF_JAIL_ROLL_OR_PAY_OR_CARD_DECISION, 0, 0, 0);
        MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
          0, 0, 0, 0, NULL, 0, NULL);
      }
      else
      {
         /* Arrived in jail during middle of this turn or staying
            in jail after failing to get out.  End turn. */

        SwitchPhase (GF_WAIT_END_TURN, 0, 0, 0);
        // RK - New, dont end the turn, NOTIFY_END_OF_TURN.
        MESS_SendAction (NOTIFY_END_TURN, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          CurrentRulesState.CurrentPlayer, 0, 0, 0, NULL, 0, NULL);
        //MESS_SendAction (ACTION_END_TURN, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        //  0, 0, 0, 0, NULL, 0, NULL);
      }
    }
    else /* Not in jail. */
    {
      if (CurrentRulesState.Dice[0] == CurrentRulesState.Dice[1] &&
      !CurrentRulesState.JustRolledOutOfJail)
      {

        /* First roll of the turn (dice both zero) or rolled
           doubles previously.  Roll the dice again.  */

        PushPhase (GF_WAIT_MOVE_ROLL, 0, 0, 0);
        MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
          0, 0, 0, 0, NULL, 0, NULL);
      }
      else /* Not doubles or first time, normal end of turn. */
      {
        SwitchPhase (GF_WAIT_END_TURN, 0, 0, 0);
        // RK - New, dont end the turn, NOTIFY_END_OF_TURN.
        MESS_SendAction (NOTIFY_END_TURN, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          CurrentRulesState.CurrentPlayer, 0, 0, 0, NULL, 0, NULL);
        //MESS_SendAction (ACTION_END_TURN, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        //  0, 0, 0, 0, NULL, 0, NULL);
      }
    }
    break;


  case GF_WAIT_MOVE_ROLL:
  case GF_WAIT_JAIL_ROLL:
  case GF_WAIT_UTILITY_ROLL:
    if (Bankrupt)
    {
      PopAndRestartPhase ();
      break;
    }
    PredictNextMove ();
    MESS_SendAction (NOTIFY_PLEASE_ROLL_DICE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.CurrentPlayer,
      (CurrentRulesState.Players + CurrentRulesState.CurrentPlayer)->currentSquare,
      0, 0, NULL, 0, NULL);
    break;


  case GF_JAIL_ROLL_OR_PAY_OR_CARD_DECISION:
    if (Bankrupt)
    {
      PopAndRestartPhase ();
      break;
    }
    PredictNextMove ();
    MESS_SendAction (NOTIFY_JAIL_EXIT_CHOICE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.CurrentPlayer,
      PlayerPntr->turnsInJail < 99 /* Roll allowed flag.  Magic value 99 for force to pay only */,
      PlayerPntr->cash >= CurrentRulesState.GameOptions.getOutOfJailFee,
      (PlayerPntr->turnsInJail < 99) && /* If force to pay then can't use card either */
      (CurrentRulesState.Cards[CHANCE_DECK].jailOwner == CurrentRulesState.CurrentPlayer ||
      CurrentRulesState.Cards[COMMUNITY_DECK].jailOwner == CurrentRulesState.CurrentPlayer),
      NULL, 0, NULL);
    break;


  case GF_GET_OUT_OF_JAIL:
    if (Bankrupt)
    {
      /* This guy went bankrupt trying to pay the fees, stop trying to move. */

      PopAndRestartPhase ();
    }
    else
    {
      SwitchPhase (GF_MOVING_TOKEN, 0, 0, 0);
      MESS_SendAction (ACTION_JUMP_TO_SQUARE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        SQ_JUST_VISITING, 0, 0, 0, NULL, 0, NULL);
    }
    break;


  case GF_WAIT_UNTIL_CARD_SEEN:
    if (Bankrupt)
    {
      PopAndRestartPhase ();
      break;
    }
    PredictNextMove ();
    Deck = ((RULE_SquarePredefinedInfo[PlayerPntr->currentSquare].group ==
    SG_COMMUNITY_CHEST) ? COMMUNITY_DECK : CHANCE_DECK);
    MESS_SendAction (NOTIFY_PICKED_UP_CARD, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.CurrentPlayer, Deck,
      CurrentRulesState.Cards[Deck].cardPile[0], 0, NULL, 0, NULL);
    break;


  case GF_COLLECTING_PAYMENT:
    if (CurrentRulesState.phaseStack[0].toPlayer < RULE_MAX_PLAYERS &&
    CurrentRulesState.Players[CurrentRulesState.phaseStack[0].
    toPlayer].currentSquare >= SQ_OFF_BOARD)
    {
      /* The guy (not the bank) collecting the money has gone bankrupt.
         Don't bother collecting this debt then. */

      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_COLLECTOR_BANKRUPT,
        CurrentRulesState.phaseStack[0].fromPlayer,
        CurrentRulesState.phaseStack[0].toPlayer,
        CurrentRulesState.phaseStack[0].amount,
        NULL, 0, NULL);

      PopAndRestartPhase ();
      break;
    }

    if (CurrentRulesState.phaseStack[0].fromPlayer < RULE_MAX_PLAYERS &&
    CurrentRulesState.Players[CurrentRulesState.phaseStack[0].
    fromPlayer].currentSquare >= SQ_OFF_BOARD)
    {
      /* The player we are collecting from has gone bankrupt,
         not much we can do now except write it off (if he
         went bankrupt to us then we already got his stuff and
         this phase would get switched to an escrow transfer
         before we even get here). */

      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_DEBTOR_BANKRUPT,
        CurrentRulesState.phaseStack[0].fromPlayer,
        CurrentRulesState.phaseStack[0].toPlayer,
        CurrentRulesState.phaseStack[0].amount,
        NULL, 0, NULL);

      PopAndRestartPhase ();
      break;
    }

    if ((CurrentRulesState.phaseStack[0].fromPlayer >= RULE_MAX_PLAYERS) ||
    (CurrentRulesState.Players[CurrentRulesState.phaseStack[0].fromPlayer].cash
    >= CurrentRulesState.phaseStack[0].amount))
    {
      /* Yes, the player can afford to pay the debt now, or is the bank. */

      BlindlyTransferCash (CurrentRulesState.phaseStack[0].fromPlayer,
        CurrentRulesState.phaseStack[0].toPlayer,
        CurrentRulesState.phaseStack[0].amount,
        TRUE /* Show the cash transfer animation */);

      /* Now make sure the AIs know we finished collecting the money.  Andrew S. 23/7/97 */
      MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER,
        CurrentRulesState.phaseStack[0].fromPlayer,
        NOTIFY_PLEASE_PAY, TRUE,
        CurrentRulesState.phaseStack[0].fromPlayer,
        0, NULL, 0, NULL);

      PopAndRestartPhase ();
    }
    else
    {
      /* Save the current game state if this is the first time.  That way we
         can undo everything if the player goes bankrupt (so he doesn't trade
         his stuff to some other player before going bankrupt). */

      if (!StackedRulesStates[0].ThisStateIsValid)
        SaveGameStateInCurrentPhase ();

      /* Tell everybody the details about the debt. */

      MESS_SendAction2 (NOTIFY_PLEASE_PAY, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        CurrentRulesState.phaseStack[0].fromPlayer,
        CurrentRulesState.phaseStack[0].toPlayer,
        CurrentRulesState.phaseStack[0].amount,
        CurrentRulesState.phaseStack[0].amount -
        CurrentRulesState.Players[CurrentRulesState.phaseStack[0].fromPlayer].cash,
        (CashPlayerCouldRaiseIfNeeded (CurrentRulesState.phaseStack[0].fromPlayer) <
          CurrentRulesState.phaseStack[0].amount), /* TRUE if allowed to go bankrupt. */
        NULL, 0, NULL);
    }
    break;


  case GF_AUCTION_OR_BUY_DECISION:
    if (Bankrupt)
    {
      PopAndRestartPhase ();
      break;
    }
    PredictNextMove ();
    MESS_SendAction (NOTIFY_BUY_OR_AUCTION_DECISION, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.CurrentPlayer,
      PlayerPntr->currentSquare,
      RULE_SquarePredefinedInfo[PlayerPntr->currentSquare].purchaseCost,
      0,
      NULL, 0, NULL);
    break;


  case GF_FLAT_OR_FRACTION_TAX_DECISION:
    if (Bankrupt)
    {
      PopAndRestartPhase ();
      break;
    }
    PredictNextMove ();
    MESS_SendAction (NOTIFY_FLAT_OR_FRACTION_TAX_DECISION, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.CurrentPlayer,
      CurrentRulesState.GameOptions.flatTaxFee,
      CurrentRulesState.GameOptions.taxRate,
      0, NULL, 0, NULL);
    break;


  case GF_TRANSFER_ESCROW_PROPERTY:
    if (NewActionPntr->fromPlayer != RULE_BANK_PLAYER)
      return; /* Ignore external restarts. */

    Bankrupt = (CurrentRulesState.Players[CurrentRulesState.phaseStack[0].
      toPlayer].currentSquare >= SQ_OFF_BOARD);

    /* Look for escrowed properties for the given player that are also marked
       in this phase's properties set.  Transfer their ownership to the player.
       Watch out for players that have gone bankrupt. */

    for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
    {
      if (RULE_PropertyToBitSet (i) & (CurrentRulesState.phaseStack[0].amount))
      {
        SquarePntr = CurrentRulesState.Squares + i;
        if (SquarePntr->owner == RULE_ESCROW_PLAYER &&
        SquarePntr->offeredInTradeTo == CurrentRulesState.phaseStack[0].toPlayer)
        {
          /* Ok, this is one of our escrowed properties.  Transfer ownership
             to the player.  Or to the bank if the player has gone bankrupt,
             though that is really a safety case since escrowed stuff is removed
             when someone goes bankrupt. */

          if (Bankrupt)
            SquarePntr->owner = RULE_BANK_PLAYER;
          else
            SquarePntr->owner = CurrentRulesState.phaseStack[0].toPlayer;
          SquarePntr->offeredInTradeTo = RULE_NOBODY_PLAYER;

          MESS_SendAction (NOTIFY_SQUARE_OWNERSHIP, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
            i, SquarePntr->owner, RULE_ESCROW_PLAYER, 0,
            NULL, 0, NULL);
        }
      }
    }

    /* And on to the next phase, which does nothing if there is nothing to do. */

    SwitchPhase (GF_FREE_UNMORTGAGE, 0, CurrentRulesState.phaseStack[0].toPlayer,
      CurrentRulesState.phaseStack[0].amount);

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
    break;


  case GF_FREE_UNMORTGAGE:
    Bankrupt = (CurrentRulesState.Players[CurrentRulesState.phaseStack[0].
      toPlayer].currentSquare >= SQ_OFF_BOARD);
    if (Bankrupt)
    {
      PopAndRestartPhase ();
      break;
    }

    /* Update our set of transfered mortgaged properties (ones that are being
       unmortgaged for free).  Just remove the ones from our list that are no
       longer mortgaged or aren't owned by the player any more. */

    SomeProperties = 0;
    for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
    {
      if (RULE_PropertyToBitSet (i) & (CurrentRulesState.phaseStack[0].amount))
      {
        SquarePntr = CurrentRulesState.Squares + i;

        if (SquarePntr->owner == CurrentRulesState.phaseStack[0].toPlayer &&
        SquarePntr->mortgaged)
        {
          /* Ok, this is one of our transfered mortgaged properties. */

          SomeProperties |= RULE_PropertyToBitSet (i);
        }
      }
    }
    CurrentRulesState.phaseStack[0].amount = SomeProperties;

    /* Tell players about free unmortgaging.  Either starts or
       continues it, alternatively ends it, depending on the set
       of mortgaged properties being empty or not. */

    if( CurrentRulesState.phaseStack[0].amount != 0 ) // New, don't send if empty.
    {
      MESS_SendAction (NOTIFY_FREE_UNMORTGAGING, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        CurrentRulesState.phaseStack[0].toPlayer,
        CurrentRulesState.phaseStack[0].amount,
        0, 0, NULL, 0, NULL);
    }

    if (CurrentRulesState.phaseStack[0].amount == 0)
      PopAndRestartPhase (); /* Done if nothing left to unmortgage. */
    break;


  case GF_AUCTION:  /* (Re)start the auction.  ACTION_TICK runs it. */
    MESS_SendAction2( NOTIFY_NEW_HIGH_BID, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
     CurrentRulesState.shared.auction.highestBidder,
     CurrentRulesState.shared.auction.highestBid,
     CurrentRulesState.shared.auction.propertyBeingAuctioned,
     0,
     PlayersAllowedToBid(),
     NULL, 0, NULL );
    if (!IgnoreWaitForEverybodyReady)
    {
      IgnoreWaitForEverybodyReady = TRUE;
      WaitForEverybodyReady( NOTIFY_NEW_HIGH_BID );
    }
    break;


  case GF_HOUSING_SHORTAGE_QUESTION:
    CurrentRulesState.shared.auction.tickCount = 0;
    CurrentRulesState.shared.auction.goingCount = 0;
    MESS_SendAction2 (NOTIFY_HOUSING_SHORTAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.phaseStack[0].fromPlayer, /* Player doing building. */
      CurrentRulesState.phaseStack[0].toPlayer,   /* Square for building. */
      CurrentRulesState.phaseStack[0].amount,     /* Number of free hotels or negative free houses. */
      CurrentRulesState.shared.auction.goingCount,
      PlayersWhoCanBuyABuilding (CurrentRulesState.phaseStack[0].amount > 0 /* TRUE for hotel */),
      NULL, 0, NULL);
    break;


  case GF_PLACE_BUILDING:
    Bankrupt = (CurrentRulesState.Players[CurrentRulesState.phaseStack[0].
      fromPlayer].currentSquare >= SQ_OFF_BOARD);
    SomeProperties = PlaceBuildingLegalSquares ();
    if (Bankrupt || SomeProperties == 0)
    {
      PopAndRestartPhase ();
      break;
    }
    MESS_SendAction (NOTIFY_PLACE_BUILDING, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.phaseStack[0].fromPlayer,
      CurrentRulesState.phaseStack[0].amount, /* +1 for hotel, -1 for house. */
      SomeProperties,
      0,
      NULL, 0, NULL);
    break;


  case GF_EDITING_TRADE:
    ResendTradeProposal ();

    /* The first time through, synchronize all the players.  This will cause a
       restart after synchronization, which will resend all the trade items
       (this function gets called again).  To avoid having the AIs go nuts
       trying to edit the trade, don't specify the editor until then. */

    if (!IgnoreWaitForEverybodyReady)
    {
      IgnoreWaitForEverybodyReady = TRUE;
      WaitForEverybodyReady (NOTIFY_TRADE_STARTED);
    }
    else /* Not the first time. */
    {
      /* Ok, tell them who is supposed to be editing this mess. */

      MESS_SendAction (NOTIFY_TRADE_EDITOR, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        CurrentRulesState.phaseStack[0].toPlayer,
        0, 0, 0, NULL, 0, NULL);
    }
    break;


  case GF_TRADE_ACCEPTANCE:  /* Ask players if they accept the trade. */

    /* First resend the whole trade, the animation system sometimes loses things
       so this may fix it up. */
    //ResendTradeProposal ();

    PlayerSet = 0;  /* Make a set of the players who haven't accepted yet. */
    InvolvedSet = 0; /* Set of players actually involved in the trade. */

    for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
    {
      PlayerPntr = CurrentRulesState.Players + PlayerNo;

      /* Only ask non-bankrupt players who are involved in this deal. */

      if (PlayerPntr->currentSquare < SQ_OFF_BOARD)
      {
        IsInvolved = PlayerInvolvedInTrade (PlayerNo);

        if (IsInvolved)
          InvolvedSet |= (1 << PlayerNo);

        if (!PlayerPntr->shared.trading.tradeAccepted &&
        (!CurrentRulesState.phaseStack[0].amount /* Not private trade */ ||
        IsInvolved))
          PlayerSet |= (1 << PlayerNo);
      }
    }

    MESS_SendAction (NOTIFY_TRADE_ACCEPTANCE_DECISION, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          PlayerSet,
          InvolvedSet,
          0,
          0,
          NULL, 0, NULL);
    break;


  case GF_TRADE_FINISHED:
    if (NewActionPntr->fromPlayer != RULE_BANK_PLAYER)
      return; /* Ignore external restarts. */

    /* Tell everybody that the trade is over.  But wait until
       the message queue has emptied out first. */

    if (MESS_CurrentQueueSize () == 0)
    {
      CurrentRulesState.TradeInProgress = FALSE;

      /* Activate all the traded immunities and futures.  The tradedItem
         flag marks the new ones. */

      for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
      {
        CurrentHitPntr = CurrentRulesState.CountHits + i;

        if (CurrentHitPntr->toPlayer != RULE_NOBODY_PLAYER &&
        CurrentHitPntr->tradedItem)
        {
          /* Check if this cancels out or supplements any other immunities. */

          Count = CurrentHitPntr->hitCount;

          for (j = 0; j < RULE_MAX_COUNT_HIT_SETS; j++)
          {
            if (j == i)
              continue;

            TestHitPntr = CurrentRulesState.CountHits + j;

            if (TestHitPntr->properties == CurrentHitPntr->properties &&
//            TestHitPntr->fromPlayer == CurrentHitPntr->fromPlayer &&
            TestHitPntr->toPlayer == CurrentHitPntr->toPlayer &&
            TestHitPntr->hitType == CurrentHitPntr->hitType)
            {
              /* Duplicate found, combine it with the new one. */

              Count += TestHitPntr->hitCount;
              TestHitPntr->toPlayer = RULE_NOBODY_PLAYER;
            }
          }

          if (Count <= 0)
            Count = 0;
          else if (Count > 127)
            Count = 127;
          CurrentHitPntr->hitCount = Count;
          CurrentHitPntr->tradedItem = FALSE;

          MESS_SendAction2 ((CurrentHitPntr->hitType == CHT_RENT_IMMUNITY) ?
            NOTIFY_IMMUNITY_COUNT : NOTIFY_FUTURE_RENT_COUNT,
            RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
            CurrentHitPntr->toPlayer,
            Count,
            SQ_OFF_BOARD, /* This isn't a use of a hit, just an update. */
            CurrentHitPntr->fromPlayer,
            CurrentHitPntr->properties,
            NULL, 0, NULL);

          MESS_SendAction2 (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
            (CurrentHitPntr->hitType == CHT_RENT_IMMUNITY) ? TMN_ERROR_IMMUNITY_GRANTED : TMN_ERROR_FUTURE_RENT_GRANTED,
            CurrentHitPntr->fromPlayer,
            CurrentHitPntr->toPlayer,
            Count,
            CurrentHitPntr->properties,
            NULL, 0, NULL);

          if (Count <= 0) /* Deallocate it if it was reduced to zero or less. */
            CurrentHitPntr->toPlayer = RULE_NOBODY_PLAYER;
        }
      }

#if FORHOTSEAT
      MESS_SendAction (NOTIFY_TRADE_FINISHED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        0, 0, 0, 0, NULL, 0, NULL);
#else
      MESS_SendAction (NOTIFY_TRADE_FINISHED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TradeStatus, 0, 0, 0, NULL, 0, NULL);
      TradeStatus = 2;
#endif
      PopAndRestartPhase ();
    }
    break;


  case GF_BUYSELLMORT:
    Bankrupt = (CurrentRulesState.Players[CurrentRulesState.phaseStack[0].
      fromPlayer].currentSquare >= SQ_OFF_BOARD);
    if (Bankrupt)
    {
      MESS_SendAction (NOTIFY_PLAYER_BUYSELLMORT, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        RULE_NOBODY_PLAYER, /* Player with exclusive mode - signal an end to it */
        0, /* Property set he is working on. */
        0, /* Cash owing in previous debt. */
        RULE_NOBODY_PLAYER, /* Player cash is owed to. */
        NULL, 0, NULL);

      PopAndRestartPhase ();
      break;
    }
    /* If the previous phase was a debt collection for the same player,
       include the amount of cash the player needs to raise. */

    if (CurrentRulesState.NumberOfPendingPhases >= 2 &&
    CurrentRulesState.phaseStack[1].phase == GF_COLLECTING_PAYMENT &&
    CurrentRulesState.phaseStack[1].fromPlayer == CurrentRulesState.phaseStack[0].fromPlayer)
    {
      Cash = CurrentRulesState.phaseStack[1].amount;
      PlayerNo = CurrentRulesState.phaseStack[1].toPlayer;
    }
    else /* Previous phase not a debt collection or for some other player. */
    {
      Cash = 0;
      PlayerNo = RULE_NOBODY_PLAYER;
    }

    MESS_SendAction (NOTIFY_PLAYER_BUYSELLMORT, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.phaseStack[0].fromPlayer,
      CurrentRulesState.phaseStack[0].amount, /* Property set he is working on. */
      Cash, /* Cash owing in previous debt. */
      PlayerNo, /* Player cash is owed to. */
      NULL, 0, NULL);
    break;


  case GF_DECOMPOSE_HOTEL:
    Count = 0;
    for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
    {
      i = CurrentRulesState.Squares[SquareNo].houses;
      if (i < CurrentRulesState.GameOptions.housesPerHotel)
        Count += i;
    }
    MESS_SendAction (NOTIFY_DECOMPOSE_SALE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.phaseStack[0].fromPlayer,
      Count - CurrentRulesState.GameOptions.maximumHouses, /* Number of houses over the maximum. */
      0, 0, NULL, 0, NULL);
    break;


  case GF_GAME_FINISHED:
    MESS_SendAction (NOTIFY_GAME_OVER, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.phaseStack[0].fromPlayer,
      CurrentRulesState.phaseStack[0].amount,
      0, 0, NULL, 0, NULL);
    break;


  case GF_COLLECT_AI_PARAMETERS_FOR_SAVE:
    MESS_SendAction (NOTIFY_AI_NEED_PARAMETERS_FOR_SAVE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.phaseStack[0].fromPlayer,
      CurrentRulesState.phaseStack[0].toPlayer,
      0, 0, NULL, 0, NULL);
    if (CurrentRulesState.phaseStack[0].fromPlayer == 0)
    {
      /* Got all the AI states, can now save.  First get the address we are
         sending to out of the current phase, before we pop the phase! */

      memset (&MyMessage, 0, sizeof (MyMessage));
      MyMessage.action = NOTIFY_GAME_STATE_FOR_SAVE;
      MyMessage.fromPlayer = RULE_BANK_PLAYER;
      MyMessage.toPlayer = CurrentRulesState.phaseStack[0].toPlayer;

      /* Pop the phase so it doesn't get saved, otherwise the game gets saved
         again as soon as it is loaded. */

      PopAndRestartPhase ();

      /* Make the saved game info and send it out. */

      if (AddGameStateToMessageBlob (&CurrentRulesState,
      CollectedAISaveStates, &MyMessage))
        MESS_SendActionMessage (&MyMessage);
      else /* Save failed. */
        MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          TMN_ERROR_SAVE_GAME_FAILURE, 0,
          CurrentRulesState.phaseStack[0].toPlayer, 0, NULL, 0, NULL);

      FreeAISaveGameInfo ();
    }
    break;


  case GF_PAUSED:
    MESS_SendAction (NOTIFY_GAME_PAUSED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      0, 0, 0, 0, NULL, 0, NULL);
    break;


  case GF_WAIT_FOR_EVERYBODY_READY:
    MESS_SendAction (NOTIFY_ARE_YOU_THERE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.phaseStack[0].fromPlayer, /* Set of players we are waiting for */
      CurrentRulesState.phaseStack[0].amount, /* Serial number. */
      CurrentRulesState.phaseStack[0].toPlayer, /* NOTIFY_ message hint code. */
      0,
      NULL, 0, NULL);

    if (CurrentRulesState.phaseStack[0].fromPlayer == 0)
      PopAndRestartPhase ();  /* Everybody has signed in.  Finished waiting. */
    break;


  default:
#if _DEBUG
    DBUG_DisplayNonFatalErrorMessage ("Restart Phase: Unimplemented phase!  Starting new game.");
#endif
    MESS_SendAction (ACTION_NEW_GAME, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
    break;
  }
}



/*****************************************************************************
 * Start a completely new game.  This wipes out everything.  Does an
 * unconditional new game if the action pointer is NULL.  If NumberA is TRUE,
 * start a new game with the same old players.
 */

static void ActionNewGame (RULE_ActionArgumentsPointer NewActionPntr)
{
  int                     i;
  int                     SavedNumberOfPlayers;
  RULE_GameOptionsRecord  SavedGameOptions;
  RULE_PlayerInfoRecord   SavedPlayerInfo [RULE_MAX_PLAYERS];
  RULE_SquareType         SquareNo;

  if (NewActionPntr != NULL &&
  CurrentRulesState.GameOptions.cheatingAllowed == FALSE &&
  NewActionPntr->fromPlayer >= CurrentRulesState.NumberOfPlayers)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  if (NewActionPntr != NULL && CurrentPhaseM == GF_GAME_FINISHED && NewActionPntr->numberA)
  {
    /* Starting a new game with the old players.  Save their names. */
    SavedNumberOfPlayers = CurrentRulesState.NumberOfPlayers;
    for (i = 0; i < SavedNumberOfPlayers; i++)
      SavedPlayerInfo [i] = CurrentRulesState.Players[i];
    SavedGameOptions = CurrentRulesState.GameOptions;
  }
  else /* A really new game. */
    SavedNumberOfPlayers = 0;

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    ACTION_NEW_GAME,
    TRUE,
    (NewActionPntr == NULL) ? RULE_BANK_PLAYER : NewActionPntr->fromPlayer,
    SavedNumberOfPlayers,
    NULL, 0, NULL);

  memset (&CurrentRulesState, 0, sizeof (CurrentRulesState));  /* Clears stacks etc. */
  FreeAISaveGameInfo ();

  CurrentRulesState.CurrentPlayer = RULE_NOBODY_PLAYER;

  for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
    {
    CurrentRulesState.Squares[SquareNo].owner = RULE_NOBODY_PLAYER;
        CurrentRulesState.Squares[SquareNo].game_earnings = 0;
    }

  if (SavedNumberOfPlayers > 0)
  {
    /* Restore old player names and game options. */

    CurrentRulesState.NumberOfPlayers = SavedNumberOfPlayers;
    CurrentRulesState.GameOptions = SavedGameOptions;

    for (i = 0; i < SavedNumberOfPlayers; i++)
    {
      wcsncpy (CurrentRulesState.Players[i].name, SavedPlayerInfo[i].name,
        RULE_MAX_PLAYER_NAME_LENGTH);
      CurrentRulesState.Players[i].name[RULE_MAX_PLAYER_NAME_LENGTH] = 0;
      CurrentRulesState.Players[i].token = SavedPlayerInfo[i].token;
      CurrentRulesState.Players[i].colour = SavedPlayerInfo[i].colour;
      CurrentRulesState.Players[i].AIPlayerLevel = SavedPlayerInfo[i].AIPlayerLevel;
    }
  }
  else /* A fresh game needs a fresh configuration. */
  {
    /* Set up a default game configuration. */

    CurrentRulesState.GameOptions.housesPerHotel = 5;
    CurrentRulesState.GameOptions.maximumHouses = 32;
    CurrentRulesState.GameOptions.maximumHotels = 12;
    CurrentRulesState.GameOptions.interestRate = 10;
    CurrentRulesState.GameOptions.initialCash = 1500;
    CurrentRulesState.GameOptions.passingGoAmount = 200;
    CurrentRulesState.GameOptions.luxuryTaxAmount = 75;
    CurrentRulesState.GameOptions.taxRate = 10;
    CurrentRulesState.GameOptions.flatTaxFee = 200;
    CurrentRulesState.GameOptions.hideCash = FALSE;
    CurrentRulesState.GameOptions.evenBuildRule = TRUE;
    CurrentRulesState.GameOptions.rollDiceToDecideStartingOrder = FALSE;
#if !FORHOTSEAT && USE_HOTKEYS2
    CurrentRulesState.GameOptions.cheatingAllowed = TRUE; /* Testers complaining about cheating AIs - even though they don't. */
#else
    CurrentRulesState.GameOptions.cheatingAllowed = FALSE; /* Testers complaining about cheating AIs - even though they don't. */
#endif
    CurrentRulesState.GameOptions.AITakesTimeToThink = TRUE;
    CurrentRulesState.GameOptions.maximumTurnsInJail = 3;
    CurrentRulesState.GameOptions.getOutOfJailFee = 50;
    CurrentRulesState.GameOptions.mortgagedCountsInGroupRent = TRUE;
    CurrentRulesState.GameOptions.houseShortageLevel = 5;
    CurrentRulesState.GameOptions.hotelShortageLevel = 3;
    CurrentRulesState.GameOptions.auctionGoingTimeDelay = 5;
    CurrentRulesState.GameOptions.inactivityWarningTime = 0;
    CurrentRulesState.GameOptions.gameOverTimeLimit = 0;
    CurrentRulesState.GameOptions.futureRentTradingAllowed = FALSE;
    CurrentRulesState.GameOptions.immunitiesTradingAllowed = FALSE;
    CurrentRulesState.GameOptions.freeParkingSeed = 500;
    CurrentRulesState.GameOptions.freeParkingPot = FALSE;
    CurrentRulesState.GameOptions.doubleSalaryOnGo = FALSE;
    CurrentRulesState.GameOptions.allowPlayersToTakeOverAIs = TRUE;
    CurrentRulesState.GameOptions.dealNPropertiesAtStartup = 0;
    CurrentRulesState.GameOptions.dealFreePropertiesAtStartup = FALSE;
    CurrentRulesState.GameOptions.stopAtNthBankruptcy = 0;
    CurrentRulesState.GameOptions.voiceChat.recordingHz = 11025;
    CurrentRulesState.GameOptions.voiceChat.recordingBits = 8;
    wcscpy (CurrentRulesState.GameOptions.voiceChat.compressorName, L"GSM 6.10");

#if RULE_TIME_LIMITED_DEMO
    CurrentRulesState.GameOptions.gameOverTimeLimit = RULE_DEMO_TIME_LIMIT_SECONDS;
#endif
  }

  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
    CurrentRulesState.CountHits[i].toPlayer = RULE_NOBODY_PLAYER;

  PushPhase (GF_ADDING_NEW_PLAYERS, 0, 0, 0);

  if (SavedNumberOfPlayers > 0) /* Start right away if desired. */
    MESS_SendAction (ACTION_START_GAME, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
  else /* Wait for players to sign up. */
  {
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ADDING_PLAYERS, RULE_MAX_PLAYERS - CurrentRulesState.NumberOfPlayers, 0, 0,
      NULL, 0, NULL);

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
  }
}



/*****************************************************************************
 * Resend the information needed for a computer that has gotten out of
 * synchronisation.  This includes optional AI state info if present.
 */

static void SendResyncMessages (RULE_PlayerNumber ToPlayer,
RULE_ResyncCauses ResyncCause)
{
  RULE_CharHandle             BlobHandle;
  DWORD                      *BlobPntr;
  DWORD                       BlobSize;
  CountHitSquareSetPointer    CountHitPntr;
  int                         i;
  RULE_ActionArgumentsRecord  MyMessage;
  RULE_PlayerNumber           PlayerNo;

  if (ToPlayer >= CurrentRulesState.NumberOfPlayers)
    ToPlayer = RULE_ALL_PLAYERS;

  MESS_SendAction (NOTIFY_NUMBER_OF_PLAYERS, RULE_BANK_PLAYER, ToPlayer,
  CurrentRulesState.NumberOfPlayers, 0, 0, 0, NULL, 0, NULL);

  /* Send the current game configuration - number of houses per hotel etc. */

  memset (&MyMessage, 0, sizeof (MyMessage));
  MyMessage.action = NOTIFY_PROPOSED_CONFIGURATION;
  MyMessage.fromPlayer = RULE_BANK_PLAYER;
  MyMessage.toPlayer = ToPlayer;
  MyMessage.numberA = RULE_NOBODY_PLAYER; /* Proposer. */
  MyMessage.numberB = 0;  /* Who hasn't accepted it yet. */
  MyMessage.numberC = 1;  /* Version of the host: 1 means Monopoly Star Wars which does futures. */
  if (AddGameOptionsToMessageBlob (&CurrentRulesState.GameOptions, &MyMessage))
    MESS_SendActionMessage (&MyMessage);

  /* Name the players who are playing and send AI info if available. */

  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    MESS_SendAction (NOTIFY_NAME_PLAYER, RULE_BANK_PLAYER, ToPlayer,
      PlayerNo /* Player number */,
      CurrentRulesState.Players[PlayerNo].token,
      CurrentRulesState.Players[PlayerNo].colour,
      CurrentRulesState.Players[PlayerNo].AIPlayerLevel,
      CurrentRulesState.Players[PlayerNo].name, 0, NULL);

    /* Send AI specific state information if it is available. */

    if (CollectedAISaveStates[PlayerNo].AIStateDataPntr != NULL)
    {
      BlobPntr = (unsigned long *) CollectedAISaveStates[PlayerNo].AIStateDataPntr;
      BlobSize = BlobPntr[1] + 8;
      BlobHandle = RULE_AllocateHandle (BlobSize);
      if (BlobHandle != NULL)
      {
        BlobPntr = (unsigned long *) RULE_LockHandle (BlobHandle);
        if (BlobPntr != NULL)
        {
          memcpy (BlobPntr, CollectedAISaveStates[PlayerNo].AIStateDataPntr, BlobSize);
          RULE_UnlockHandle (BlobHandle);
          BlobPntr = NULL;
          MESS_SendAction (NOTIFY_AI_PARAMETERS, RULE_BANK_PLAYER, ToPlayer,
            PlayerNo, 0, 0, 0, NULL, 0, BlobHandle);
        }
        else /* Lock failed. */
          RULE_FreeHandle (BlobHandle);
      }
    }
  }

  SendClientResyncGameState (ToPlayer, ResyncCause);

  /* Send the various immunities and futures. */

  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
  {
    CountHitPntr = CurrentRulesState.CountHits + i;

    if (CountHitPntr->toPlayer != RULE_NOBODY_PLAYER && !CountHitPntr->tradedItem)
    {
      MESS_SendAction2 ((CountHitPntr->hitType == CHT_RENT_IMMUNITY) ?
        NOTIFY_IMMUNITY_COUNT : NOTIFY_FUTURE_RENT_COUNT,
        RULE_BANK_PLAYER, ToPlayer,
        CountHitPntr->toPlayer,
        CountHitPntr->hitCount,
        SQ_OFF_BOARD, /* This isn't a use of a hit, just an update. */
        CountHitPntr->fromPlayer,
        CountHitPntr->properties,
        NULL, 0, NULL);
    }
  }

  /* Also do a restart so that he gets the current prompt again. */

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
  0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * Resend the information needed for a computer that has gotten out of
 * synchronisation.
 */

static void ActionResyncClient (RULE_ActionArgumentsPointer NewActionPntr)
{
  SendResyncMessages ((RULE_PlayerNumber) NewActionPntr->numberA,
    RESYNC_NET_REFRESH);
}



/*****************************************************************************
 * The player either wants to sign up or to change a name (which implies
 * taking over an AI).
 */

static void ActionNamePlayer (RULE_ActionArgumentsPointer NewActionPntr)
{
  BOOL                      BadNameRequest;
  RULE_PlayerNumber         i;
  int                       Length;
  wchar_t                   OldName [RULE_MAX_PLAYER_NAME_LENGTH+1];
  RULE_PlayerNumber         PlayerToReplace;
  RULE_NetworkAddressRecord TempNetworkAddress;

  if (NewActionPntr->numberA == RULE_NOBODY_PLAYER)
  {
    /* This person wants to sign up a new player. */

    if (CurrentRulesState.NumberOfPlayers >= RULE_MAX_PLAYERS ||
    CurrentPhaseM != GF_ADDING_NEW_PLAYERS)
    {
      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_NO_MORE_PLAYERS, 0, 0, 0, NewActionPntr->stringA, 0, NULL);
      return;
    }

    /* Look for duplicate names, colours or tokens. */

    Length = wcslen (NewActionPntr->stringA);
    BadNameRequest = (Length < 1 || Length > RULE_MAX_PLAYER_NAME_LENGTH ||
      (unsigned long) NewActionPntr->numberC >= MAX_TOKENS ||
      (unsigned long) NewActionPntr->numberD >= MAX_PLAYER_COLOURS);
    for (i = 0; i < CurrentRulesState.NumberOfPlayers && !BadNameRequest; i++)
    {
      if (CurrentRulesState.Players[i].token == NewActionPntr->numberC)
        BadNameRequest = TRUE;
      if (CurrentRulesState.Players[i].colour == NewActionPntr->numberD)
        BadNameRequest = TRUE;
      if (_wcsicmp (CurrentRulesState.Players[i].name, NewActionPntr->stringA) == 0)
        BadNameRequest = TRUE;
    }
    if (BadNameRequest)
    {
      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_NAME_IN_USE, 0, i /* Player using it, maybe */, 0,
        NewActionPntr->stringA, 0, NULL);
      return;
    }

    /* It looks ok.  Add the new player. */

    i = CurrentRulesState.NumberOfPlayers++;
    CurrentRulesState.Players[i].token = NewActionPntr->numberC;
    CurrentRulesState.Players[i].colour = NewActionPntr->numberD;
    wcsncpy (CurrentRulesState.Players[i].name, NewActionPntr->stringA,
      RULE_MAX_PLAYER_NAME_LENGTH);
    CurrentRulesState.Players[i].name[RULE_MAX_PLAYER_NAME_LENGTH] = 0;
    CurrentRulesState.Players[i].AIPlayerLevel = (unsigned char) NewActionPntr->numberB;

    MESS_AssociatePlayerWithAddress (i, &NewActionPntr->fromNetworkAddress);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ADDING_PLAYERS, RULE_MAX_PLAYERS - CurrentRulesState.NumberOfPlayers, 0, 0,
      NULL, 0, NULL);

    /* Do a restart so that GF_ADDING_NEW_PLAYERS can resend the list
       of all players and update the NOTIFY_PLEASE_ADD_PLAYERS to show
       how many spectators there now are. */

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
  }
  else /* Player slot specified: renaming an existing player. */
  {
    /* Renaming an existing player.  OK if the sender is the player or host,
       or if the old one is an AI (renaming an AI is how you take it over).
       Can also rename to nothing to leave the game (rather  abruptly, leaving
       strangely owned properties in play, so don't do it except during
       startup when players are being picked). */

    PlayerToReplace = (RULE_PlayerNumber) NewActionPntr->numberA;

    if (PlayerToReplace >= CurrentRulesState.NumberOfPlayers ||
    (CurrentRulesState.Players[PlayerToReplace].AIPlayerLevel == 0 &&
    NewActionPntr->fromNetworkAddress.networkSystem != NS_LOCAL &&
    NewActionPntr->fromPlayer != PlayerToReplace))
    {
      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_NOT_YOUR_PLAYER, 0, PlayerToReplace, 0,
        NewActionPntr->stringA, 0, NULL);
      return;
    }

    /* If the new name is an empty string then delete the player. */

    if (NewActionPntr->stringA [0] == 0)
    {
      if (CurrentPhaseM != GF_ADDING_NEW_PLAYERS)
      {
        MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          TMN_ERROR_NAME_IN_USE, 0, PlayerToReplace, 0,
          NewActionPntr->stringA, 0, NULL);
        return;
      }

      /* OK, now deleting the player.  Tell everybody about it
         before it gets deleted. */

      MESS_SendAction (NOTIFY_PLAYER_DELETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          PlayerToReplace /* Player number */,
          CurrentRulesState.Players[PlayerToReplace].token,
          CurrentRulesState.Players[PlayerToReplace].colour,
          CurrentRulesState.Players[PlayerToReplace].AIPlayerLevel,
          CurrentRulesState.Players[PlayerToReplace].name, 0, NULL);

      if (PlayerToReplace != CurrentRulesState.NumberOfPlayers - 1)
      {
        /* Not the last player being dropped, exchange the droppee
           and the last player.  The renumbered player is broadcast
           to the multitudes, so everyone knows the new slot number. */

        CurrentRulesState.Players[PlayerToReplace] =
        CurrentRulesState.Players[CurrentRulesState.NumberOfPlayers - 1];

        /* Also have to move the associated network address. */

        MESS_GetAddressOfPlayer (
          (RULE_PlayerNumber) (CurrentRulesState.NumberOfPlayers - 1),
          &TempNetworkAddress);

        MESS_AssociatePlayerWithAddress (
          (RULE_PlayerNumber) PlayerToReplace, &TempNetworkAddress);
      }

      /* Remove the deleted player's network address, to avoid future bugs. */

      memset (&TempNetworkAddress, 0, sizeof (TempNetworkAddress));

      MESS_AssociatePlayerWithAddress (
        (RULE_PlayerNumber) (CurrentRulesState.NumberOfPlayers - 1),
        &TempNetworkAddress);

      CurrentRulesState.NumberOfPlayers--;

      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ADDING_PLAYERS, RULE_MAX_PLAYERS - CurrentRulesState.NumberOfPlayers, 0, 0,
        NULL, 0, NULL);

      /* Do a restart so that GF_ADDING_NEW_PLAYERS can resend the list
         of all players (including renumbered one) and update the
         NOTIFY_PLEASE_ADD_PLAYERS to show how many spectators there are. */

      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
    }
    else /* Rename the player and update their AI status flag. */
    {
      if (!CurrentRulesState.GameOptions.allowPlayersToTakeOverAIs &&
      CurrentRulesState.Players[PlayerToReplace].AIPlayerLevel != 0)
      {
        /* Sorry, can't take over an AI right now. */

        MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          TMN_ERROR_NOT_YOUR_PLAYER, 0, PlayerToReplace, 0,
          NewActionPntr->stringA, 0, NULL);
        return;
      }

#if RULE_TAKE_OVER_AI_ONLY_WHEN_SAFE
      /* If you take over an AI in the production game while it has buttons up,
         it will leave the buttons disabled, so nobody can click on them. */

      if (CurrentPhaseM != GF_WAIT_MOVE_ROLL)
      {
        ErrorWrongPhase (NewActionPntr);
        return;
      }
#endif

      /* Copy the name and other info. */

      wcsncpy (OldName, CurrentRulesState.Players[PlayerToReplace].name,
        RULE_MAX_PLAYER_NAME_LENGTH);
      OldName [RULE_MAX_PLAYER_NAME_LENGTH] = 0;

      wcsncpy (CurrentRulesState.Players[PlayerToReplace].name,
        NewActionPntr->stringA, RULE_MAX_PLAYER_NAME_LENGTH);
      CurrentRulesState.Players[PlayerToReplace].name[RULE_MAX_PLAYER_NAME_LENGTH] = 0;

      CurrentRulesState.Players[PlayerToReplace].AIPlayerLevel =
        (unsigned char) NewActionPntr->numberB;

      /* Remember the new network address for the player,
         happens if someone takes over an AI. */

      MESS_AssociatePlayerWithAddress (PlayerToReplace,
        &NewActionPntr->fromNetworkAddress);

      /* Tell the other players about the new player info. */

      MESS_SendAction (NOTIFY_NAME_PLAYER, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        PlayerToReplace /* Player number */,
        CurrentRulesState.Players[PlayerToReplace].token,
        CurrentRulesState.Players[PlayerToReplace].colour,
        CurrentRulesState.Players[PlayerToReplace].AIPlayerLevel,
        CurrentRulesState.Players[PlayerToReplace].name, 0, NULL);

      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_PLAYER_REPLACED, 0, PlayerToReplace, 0,
        OldName, 0, NULL);

      /* If the new player was joining in, this should show him
         the dice roll request etc. */

      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
    }
  }
}



/*****************************************************************************
 * The game is starting the accept configuration phase or is re-starting it.
 * Clear the accept flag for each player as appropriate.
 */
static void ClearAcceptConfigurationForAllPlayers( void )
{
  RULE_PlayerNumber       playerNo;

  // First stage. Turn approval off for everyone, or pre-approve everyone, depending...
  for ( playerNo = 0; playerNo < RULE_MAX_PLAYERS; playerNo++ )
  {
#if RULE_PLAYERSVOTEONRULES
    CurrentRulesState.Players[playerNo].shared.starting.acceptedConfiguration = FALSE;
#else
    CurrentRulesState.Players[playerNo].shared.starting.acceptedConfiguration = TRUE;
#endif
  }
  // Now, if only host sets the rules, turn approval off for the host only.
#if !RULE_PLAYERSVOTEONRULES
  for ( playerNo = 0; playerNo < RULE_MAX_PLAYERS; playerNo++ )
  {
    // Calculate the host to be the first local player we encounter in this
    // array that is running the rules engine (ie. this code!).
    if ( SlotIsALocalPlayer[playerNo] && !SlotIsALocalAIPlayer[playerNo] )
    {
      CurrentRulesState.Players[playerNo].shared.starting.acceptedConfiguration = FALSE;
      break;
    }
  }
#endif
}


/*****************************************************************************
 * Start the game, all the players are here now.  This leads to a vote on
 * the game configuration / rules.
 */

static void ActionStartGame (RULE_ActionArgumentsPointer NewActionPntr)
{
  int               SpectatorCount;

  if (CurrentPhaseM != GF_ADDING_NEW_PLAYERS)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  /* Only allow actual players to start the game, or the bank.
     If cheating is on, anyone can start it (but it is never on). */

  if (CurrentRulesState.GameOptions.cheatingAllowed == FALSE &&
  (NewActionPntr->fromPlayer >= CurrentRulesState.NumberOfPlayers &&
  NewActionPntr->fromPlayer != RULE_BANK_PLAYER))
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  /* Need at least two players. */

  if (CurrentRulesState.NumberOfPlayers < 2)
  {
    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action,
      FALSE,
      NewActionPntr->fromPlayer,
      0,
      NULL, 0, NULL);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_NEED_TWO_PLAYERS, 0, 0, 0,
      NULL, 0, NULL);

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);

    return;
  }

  /* Optionally make sure there are no spectators. */

  SpectatorCount = CountSpectatorComputers ();
#if RULE_NO_SPECTATORS_ALLOWED
  if (SpectatorCount > 0)
  {
    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action,
      FALSE,
      NewActionPntr->fromPlayer,
      0,
      NULL, 0, NULL);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_NEED_HUMAN_PLAYERS, SpectatorCount, 0, 0,
      NULL, 0, NULL);

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);

    return;
  }
#endif /* RULE_NO_SPECTATORS_ALLOWED */

  /* Ok, we can now start the game! */

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE, /* Succesfully starting the game. */
    NewActionPntr->fromPlayer,
    0,
    NULL, 0, NULL);

  /* Now start asking the players to choose the game configuration. */

  CurrentRulesState.shared.starting.proposer = RULE_NOBODY_PLAYER;

  SwitchPhase (GF_CONFIGURATION, 0, 0, 0);

  ClearAcceptConfigurationForAllPlayers();

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);

}


/*****************************************************************************
 * The player has looked at the game options and either accepts the current
 * settings or suggests some new ones.  If they are new then everyone has
 * to accept them over again.
 */

static void ActionAcceptConfiguration (RULE_ActionArgumentsPointer NewActionPntr)
{
  BOOL                    NeedToResendNotification;
  RULE_GameOptionsRecord  OptionsReceived;

  if (CurrentPhaseM != GF_CONFIGURATION)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  if (NewActionPntr->fromPlayer >= CurrentRulesState.NumberOfPlayers)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE, /* Succesfully accept configurations. */
    NewActionPntr->fromPlayer,
    0,
    NULL, 0, NULL);

  NeedToResendNotification = FALSE;

  /* Default to current settings, if they send us garbage these will get used
     instead.  The options are loaded from a RIFF format file and validated. */

  OptionsReceived = CurrentRulesState.GameOptions;

  RULE_ConvertFileToGameOptions (NewActionPntr->binaryDataA,
    NewActionPntr->binaryDataSizeA, &OptionsReceived);

  /* If this is an acceptance from an earlier version of the client, that
     doesn't support immunities and futures, turn off that option. */

  if (NewActionPntr->numberC < 1 /* Version 1 supports immunities and futures */)
  {
    OptionsReceived.futureRentTradingAllowed = FALSE;
    OptionsReceived.immunitiesTradingAllowed = FALSE;
  }

#if RULE_TIME_LIMITED_DEMO
  if (OptionsReceived.gameOverTimeLimit <= 0 ||
  OptionsReceived.gameOverTimeLimit > RULE_DEMO_TIME_LIMIT_SECONDS)
    OptionsReceived.gameOverTimeLimit = RULE_DEMO_TIME_LIMIT_SECONDS;
#endif

  /* Did the user change some of the game options? */

  if (memcmp (&CurrentRulesState.GameOptions, &OptionsReceived,
  sizeof (OptionsReceived)) != 0)
  {
    NeedToResendNotification = TRUE;

    if ( !NewActionPntr->numberD )
    {
      // Ok, this is NOT the interim type case so we need to get all players to accept again.
      ClearAcceptConfigurationForAllPlayers();
    }

    CurrentRulesState.shared.starting.proposer = NewActionPntr->fromPlayer;

    /* Update our game options with the new ones. */

    CurrentRulesState.GameOptions = OptionsReceived;
  }

  /* This user has accepted the game options.  Restart to check if everyone is
     done (but only if something has changed to avoid triggering an avalanche
     of duplicate acknowledgements), and to broadcast the current game options.
     If this was the final accept (not an interim one, NumberD == 0 for final),
     then mark the player as having accepted it. */

  if (NewActionPntr->numberD == 0 &&
  CurrentRulesState.Players[NewActionPntr->fromPlayer].shared.starting.
  acceptedConfiguration != TRUE)
  {
    NeedToResendNotification = TRUE;

    CurrentRulesState.Players[NewActionPntr->fromPlayer].shared.starting.
      acceptedConfiguration = TRUE;
  }

  if (NeedToResendNotification)
    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * Start of a player's turn.  Clear various counters and start rolling.
 */

static void ActionStartTurn (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerInfoPointer PlayerPntr;

  if (CurrentPhaseM != GF_WAIT_START_TURN)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  if (NewActionPntr->fromPlayer != RULE_BANK_PLAYER)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  PlayerPntr = CurrentRulesState.Players + CurrentRulesState.CurrentPlayer;

  if (CurrentRulesState.CurrentPlayer >= CurrentRulesState.NumberOfPlayers ||
  PlayerPntr->currentSquare >= SQ_OFF_BOARD)
  {
#if _DEBUG
    DBUG_DisplayNonFatalErrorMessage ("ActionStartTurn: Current player isn't valid!  Starting a new game.");
#endif
    RULE_InitializeSystem (); /* Dump everything and start a new game. */
    return;
  }

  CurrentRulesState.NumberOfDoublesRolled = 0;
  CurrentRulesState.Dice[0] = CurrentRulesState.Dice[1] = 0;
  CurrentRulesState.UtilityDice[0] = CurrentRulesState.UtilityDice[1] = 0;
  CurrentRulesState.JustRolledOutOfJail = FALSE;
  CurrentRulesState.PendingCard = NOT_A_CARD;

  SendGameStatusToLobby (GAME_RUNNING, 0);

  MESS_SendAction (NOTIFY_START_TURN, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    CurrentRulesState.CurrentPlayer, 0, 0, 0,
    NULL, 0, NULL);

  SwitchPhase (GF_PREROLL, 0, 0, 0);

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);

}



/*****************************************************************************
 * The player wants to roll the dice.  See if it is the right player and time,
 * then simulate a dice roll and carry on to the next state (keep in mind
 * that this is a stacked phase that will pop down to GF_PREROLL when the
 * move is finished).  This function also handles ACTION_CHEAT_ROLL_DICE.
 */

static void ActionRollDice (RULE_ActionArgumentsPointer NewActionPntr)
{
  BOOL                    DoublesRolled;
  int                     i;
  RULE_SquareType         NewSquare;
  RULE_PlayerInfoPointer  PlayerPntr;
  DWORD                   ShiftValue;
  unsigned char           TempDice[2];
  unsigned char           TotalRolled;

  if (CurrentPhaseM != GF_WAIT_MOVE_ROLL &&
  CurrentPhaseM != GF_WAIT_JAIL_ROLL &&
  CurrentPhaseM != GF_WAIT_UTILITY_ROLL &&
  CurrentPhaseM != GF_PICKING_STARTING_ORDER)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  if (NewActionPntr->fromPlayer != CurrentRulesState.CurrentPlayer)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  PlayerPntr = CurrentRulesState.Players + CurrentRulesState.CurrentPlayer;

  if (NewActionPntr->action == ACTION_CHEAT_ROLL_DICE &&
  CurrentRulesState.GameOptions.cheatingAllowed)
  {
    /* Use the cheating roll instead of actually rolling the dice. */
    TempDice[0] = (unsigned char) NewActionPntr->numberA;
    if (TempDice[0] < 1)
      TempDice[0] = 1;
    if (TempDice[0] > 6)
      TempDice[0] = 6;

    TempDice[1] = (unsigned char) NewActionPntr->numberB;
    if (TempDice[1] < 1)
      TempDice[1] = 1;
    if (TempDice[1] > 6)
      TempDice[1] = 6;

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_PLAYER_CHEATING, 0, NewActionPntr->fromPlayer, 0,
      NULL, 0, NULL);
  }
  else
  {
    /* Do the real dice roll.  Well, grab it from the next dice roll. */

    if (CurrentRulesState.NextDice[0] == 0)  /* If no next roll, make one now. */
    {
      TempDice[0] = (unsigned char) ((rand () % 6) + 1);
      rand(); rand (); rand (); /* Reduce chance of doubles. */
      TempDice[1] = (unsigned char) ((rand () % 6) + 1);
    }
    else
    {
      TempDice[0] = CurrentRulesState.NextDice[0];
      TempDice[1] = CurrentRulesState.NextDice[1];
    }

    CurrentRulesState.NextDice[0] = (unsigned char) ((rand () % 6) + 1);
    rand(); rand (); rand (); /* Reduce chance of doubles. */
    CurrentRulesState.NextDice[1] = (unsigned char) ((rand () % 6) + 1);
  }

  /* Calculate roll related values and tell the players about the roll. */

  TotalRolled = (unsigned char) (TempDice[0] + TempDice[1]);
  DoublesRolled = (TempDice[0] == TempDice[1]);

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE, /* Successfully rolled the dice. */
    NewActionPntr->fromPlayer,
    0,
    NULL, 0, NULL);

  MESS_SendAction (NOTIFY_DICE_ROLLED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    TempDice[0], TempDice[1],
    CurrentRulesState.CurrentPlayer, 0,
    NULL, 0, NULL);

  /* Do whatever the roll does in the current phase. */

  switch (CurrentPhaseM)
  {
  case GF_WAIT_MOVE_ROLL:  /* Do ordinary movement roll. */
    CurrentRulesState.Dice [0] = TempDice [0];
    CurrentRulesState.Dice [1] = TempDice [1];

    /* If you rolled 3 doubles, you go to jail.  Otherwise move forwards
       the amount rolled. */

    if (DoublesRolled)
      CurrentRulesState.NumberOfDoublesRolled++;

    SwitchPhase (GF_MOVING_TOKEN, 0, 0, 0);

    if (CurrentRulesState.NumberOfDoublesRolled >= 3)
    {
      MESS_SendAction (ACTION_JUMP_TO_SQUARE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        SQ_IN_JAIL, 0, 0, 0, NULL, 0, NULL);
    }
    else /* Advance to a square. */
    {
      NewSquare = PlayerPntr->currentSquare + TotalRolled;
      if (NewSquare > SQ_BOARDWALK)
        NewSquare -= (SQ_BOARDWALK + 1); /* Wrap around when past last square. */

      MESS_SendAction (ACTION_MOVE_FORWARDS, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        NewSquare, 0, 0, 0, NULL, 0, NULL);
    }
    break;


  case GF_WAIT_JAIL_ROLL:
    CurrentRulesState.Dice [0] = TempDice [0];
    CurrentRulesState.Dice [1] = TempDice [1];
    CurrentRulesState.JustRolledOutOfJail = TRUE;

    /* If you rolled doubles while in jail, you get out of jail and
       advance the amount rolled.  If you didn't then you stay in jail,
       except for the last turn when you pay up and get out after failing
       to roll doubles. */

    if (DoublesRolled)
    {
      SwitchPhase (GF_GET_OUT_OF_JAIL, 0, 0, 0);
      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
    }
    else /* End of dice rolling. */
    {
      if (CurrentRulesState.GameOptions.maximumTurnsInJail == 0 ||
      PlayerPntr->turnsInJail >= CurrentRulesState.GameOptions.maximumTurnsInJail)
      {
        /* Holly R's jail rule for shorter games (maximumTurnsInJail == 0).
           If you roll and fail, you pay $50 and get out right away.
           In effect, you always get out of jail on the turn after you
           went in.  Same thing for the official rules on the last turn in
           jail (if you don't get doubles then you must pay (no card)).
           So, go back to the roll or pay decision but with the number of
           turns in jail set to 99 so that it disables the roll option
           and the get out of jail card option. */

        PlayerPntr->turnsInJail = 99;

        SwitchPhase (GF_JAIL_ROLL_OR_PAY_OR_CARD_DECISION, 0, 0, 0);

        MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
          0, 0, 0, 0, NULL, 0, NULL);
      }
      else /* End of turn for normal jail rules when you don't roll doubles. */
        PopAndRestartPhase ();
    }
    break;


  case GF_WAIT_UTILITY_ROLL:
    CurrentRulesState.UtilityDice [0] = TempDice [0];
    CurrentRulesState.UtilityDice [1] = TempDice [1];
    PopPhase ();
    CollectRent (); /* Stacks more phases for debt collection. */
    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
    break;


  case GF_PICKING_STARTING_ORDER:
    CurrentRulesState.Dice [0] = TempDice [0];
    CurrentRulesState.Dice [1] = TempDice [1];

    /* Append the dice roll to this player's rolls in the encoded 32 bit
       history (oldest roll in highest bits). */

    ShiftValue = CurrentRulesState.Players[CurrentRulesState.CurrentPlayer].
      shared.ordering.diceRollHistory;
    i = 0;
    while ((ShiftValue & 0xF) == 0 && i < 32)
    {
      i += 4;
      ShiftValue >>= 4;
    }
    if (i >= 4)
    {
      CurrentRulesState.Players[CurrentRulesState.CurrentPlayer].
      shared.ordering.diceRollHistory |= (((DWORD) TotalRolled) << (i - 4));
    }

    /* Remove the player from the set of players being asked for dice rolls. */

    CurrentRulesState.phaseStack[0].fromPlayer &=
      ~(1 << CurrentRulesState.CurrentPlayer);

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
    break;
  }

}



/*****************************************************************************
 * Move the player's piece forwards to a square.
 */

static void ActionMoveForwards (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_CardType           CardPickedUp;
  RULE_SquareGroups       LandedOnGroup;
  RULE_PlayerInfoPointer  PlayerPntr;

  if (NewActionPntr->fromPlayer != RULE_BANK_PLAYER)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  if (CurrentPhaseM != GF_MOVING_TOKEN)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  PlayerPntr = CurrentRulesState.Players + CurrentRulesState.CurrentPlayer;

  /* If we are moving forwards to a chance or community chest square, also
     tell the user interface which card will be picked up when the player
     lands there. */

  LandedOnGroup = RULE_SquarePredefinedInfo[NewActionPntr->numberA].group;
  if (LandedOnGroup == SG_COMMUNITY_CHEST)
    CardPickedUp = CurrentRulesState.Cards[COMMUNITY_DECK].cardPile[0];
  else if (LandedOnGroup == SG_CHANCE)
    CardPickedUp = CurrentRulesState.Cards[CHANCE_DECK].cardPile[0];
  else /* Just landing on an ordinary square, no card to pick up. */
    CardPickedUp = NOT_A_CARD;

  MESS_SendAction2 (NOTIFY_MOVE_FORWARDS, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->numberA,
    PlayerPntr->currentSquare,
    CurrentRulesState.CurrentPlayer,
    CardPickedUp,
    !PlayerPntr->firstMoveMade, /* TRUE if this is the first move. */
    NULL, 0, NULL);

  PlayerPntr->firstMoveMade = TRUE;

  /* Did the player pass go?  If they did, they collect $200.  You can tell
     that GO was passed when the coordinates wrap around.  The amount is
     optionally doubled if the player actually stops on GO. */

  if (NewActionPntr->numberA <= PlayerPntr->currentSquare)
  {
    MESS_SendAction (NOTIFY_PASSED_GO, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.CurrentPlayer, 0, 0, 0, NULL, 0, NULL);

    BlindlyTransferCash (RULE_BANK_PLAYER, CurrentRulesState.CurrentPlayer,
      (CurrentRulesState.GameOptions.doubleSalaryOnGo && NewActionPntr->numberA == SQ_GO)
        ? CurrentRulesState.GameOptions.passingGoAmount * 2
        : CurrentRulesState.GameOptions.passingGoAmount,
      TRUE /* Show the cash transfer animation */);
  }

  PlayerPntr->currentSquare = NewActionPntr->numberA;

  MESS_SendAction (ACTION_LANDED_ON_SQUARE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * Move the player's piece directly to a square.
 */

static void ActionJumpToSquare (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerInfoPointer PlayerPntr;

  if (NewActionPntr->fromPlayer != RULE_BANK_PLAYER)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  if (CurrentPhaseM != GF_MOVING_TOKEN)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  PlayerPntr = CurrentRulesState.Players + CurrentRulesState.CurrentPlayer;

  MESS_SendAction (NOTIFY_JUMP_TO_SQUARE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->numberA,
    PlayerPntr->currentSquare,
    CurrentRulesState.CurrentPlayer,
    NOT_A_CARD,
    NULL, 0, NULL);

  /* Did the player go to GO?  If they did, they collect $200.  Note that it
     doesn't count for double salary if you are playing with the landing on
     GO doubles the salary option.  Well, due to animation limits, they
     always play the happy animation when they are at GO, so we double it
     even if they jump to GO. */

  if (NewActionPntr->numberA == SQ_GO)
  {
    MESS_SendAction (NOTIFY_PASSED_GO, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.CurrentPlayer, 0, 0, 0, NULL, 0, NULL);

    BlindlyTransferCash (RULE_BANK_PLAYER, CurrentRulesState.CurrentPlayer,
      (CurrentRulesState.GameOptions.doubleSalaryOnGo && NewActionPntr->numberA == SQ_GO)
        ? CurrentRulesState.GameOptions.passingGoAmount * 2
        : CurrentRulesState.GameOptions.passingGoAmount,
      TRUE /* Show the cash transfer animation */);
  }

  PlayerPntr->currentSquare = NewActionPntr->numberA;

  MESS_SendAction (ACTION_LANDED_ON_SQUARE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * Move the player's piece backwards to a square.
 */

static void ActionMoveBackwards (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_CardType           CardPickedUp;
  RULE_SquareGroups       LandedOnGroup;
  RULE_PlayerInfoPointer  PlayerPntr;

  if (NewActionPntr->fromPlayer != RULE_BANK_PLAYER)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  if (CurrentPhaseM != GF_MOVING_TOKEN)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  PlayerPntr = CurrentRulesState.Players + CurrentRulesState.CurrentPlayer;

  /* If we are moving forwards to a chance or community chest square, also
     tell the user interface which card will be picked up when the player
     lands there. */

  LandedOnGroup = RULE_SquarePredefinedInfo[NewActionPntr->numberA].group;
  if (LandedOnGroup == SG_COMMUNITY_CHEST)
    CardPickedUp = CurrentRulesState.Cards[COMMUNITY_DECK].cardPile[0];
  else if (LandedOnGroup == SG_CHANCE)
    CardPickedUp = CurrentRulesState.Cards[CHANCE_DECK].cardPile[0];
  else /* Just landing on an ordinary square, no card to pick up. */
    CardPickedUp = NOT_A_CARD;

  MESS_SendAction (NOTIFY_MOVE_BACKWARDS, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->numberA,
    PlayerPntr->currentSquare,
    CurrentRulesState.CurrentPlayer,
    CardPickedUp,
    NULL, 0, NULL);

  PlayerPntr->currentSquare = NewActionPntr->numberA;

  MESS_SendAction (ACTION_LANDED_ON_SQUARE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * The player has seen the chance or community chest card.  Now take the card
 * off the top of the deck and put the card's actions into effect.  The card
 * is from the card deck for the square the current player is on.  Assumes we
 * are in a stacked phase, so pop the phase stack when the card finishes the
 * player's dice roll activity.
 */

static void ActionCardSeen (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_CardDeckTypes      Deck;
  int                     HotelCount;
  int                     HouseCount;
  RULE_SquareType         NewSquare;
  RULE_PlayerNumber       OtherPlayer;
  RULE_CardType           PickedCard;
  RULE_PlayerInfoPointer  PlayerPntr;

  if (CurrentPhaseM != GF_WAIT_UNTIL_CARD_SEEN)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  if (NewActionPntr->fromPlayer != CurrentRulesState.CurrentPlayer)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  PlayerPntr = CurrentRulesState.Players + CurrentRulesState.CurrentPlayer;

  Deck = ((RULE_SquarePredefinedInfo[PlayerPntr->currentSquare].group ==
    SG_COMMUNITY_CHEST) ? COMMUNITY_DECK : CHANCE_DECK);

  if (Deck == COMMUNITY_DECK)
  {
    PickedCard = DealCommunityCard (&CurrentRulesState);
  }
  else
    PickedCard = DealChanceCard (&CurrentRulesState);

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE, /* Successfully looked at the card. */
    NewActionPntr->fromPlayer,
    0,
    NULL, 0, NULL);

  MESS_SendAction (NOTIFY_PUT_AWAY_CARD, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    CurrentRulesState.CurrentPlayer, Deck, PickedCard, 0,
    NULL, 0, NULL);

  /* Remember the card that was picked.  This is for cards that affect later
     actions, such as doubling the rent on a railroad square _after_ the move
     to the railroad has been done. */

  CurrentRulesState.PendingCard = PickedCard;

  switch (PickedCard)
  {
    case COMMUNITY_GET_100_FROM_BANK_1:
    case COMMUNITY_GET_100_FROM_BANK_2:
    case COMMUNITY_GET_100_FROM_BANK_3:
      BlindlyTransferCash (RULE_BANK_PLAYER, CurrentRulesState.CurrentPlayer, 100, TRUE);
      PopAndRestartPhase (); /* End of movement. */
      break;

    case COMMUNITY_GET_10_FROM_BANK:
      BlindlyTransferCash (RULE_BANK_PLAYER, CurrentRulesState.CurrentPlayer, 10, TRUE);
      PopAndRestartPhase (); /* End of movement. */
      break;

    case CHANCE_GET_OUT_OF_JAIL_FREE:
    case COMMUNITY_GET_OUT_OF_JAIL_FREE:
      TransferGetOutOfJail (RULE_NOBODY_PLAYER, CurrentRulesState.CurrentPlayer, Deck);
      PopAndRestartPhase (); /* End of movement. */
      break;

    case COMMUNITY_PAY_50_TO_BANK:
      PopPhase (); /* End movement before debt activity stacks up. */
      StackDebtAndRestart (CurrentRulesState.CurrentPlayer, RULE_BANK_PLAYER, 50);
      AddMoneyToFreeParkingPot (50);
      break;

    case COMMUNITY_GET_200_FROM_BANK:
      BlindlyTransferCash (RULE_BANK_PLAYER, CurrentRulesState.CurrentPlayer, 200, TRUE);
      PopAndRestartPhase (); /* End of movement. */
#if !FORHOTSEAT
      // increment the card200 counter
      g_Card200Counter++;
#endif
      break;

    case COMMUNITY_GET_20_FROM_BANK:
      BlindlyTransferCash (RULE_BANK_PLAYER, CurrentRulesState.CurrentPlayer, 20, TRUE);
      PopAndRestartPhase (); /* End of movement. */
      break;

    case COMMUNITY_PAY_40_EACH_HOUSE_115_EACH_HOTEL:
      RULE_CountHousesAndHotels (&CurrentRulesState,
        CurrentRulesState.CurrentPlayer, &HouseCount, &HotelCount);
      PopPhase (); /* End movement before debt activity stacks up. */
      StackDebtAndRestart (CurrentRulesState.CurrentPlayer, RULE_BANK_PLAYER,
        40 * HouseCount + 115 * HotelCount);
      AddMoneyToFreeParkingPot (40 * HouseCount + 115 * HotelCount);
      MESS_SendAction2 (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_PROPERTY_FEES,
        40 * HouseCount + 115 * HotelCount,
        CurrentRulesState.CurrentPlayer,
        HouseCount,
        HotelCount,
        NULL, 0, NULL);
      break;

    case COMMUNITY_GO_DIRECTLY_TO_JAIL:
    case CHANCE_GO_DIRECTLY_TO_JAIL:
      SwitchPhase (GF_MOVING_TOKEN, 0, 0, 0);
      MESS_SendAction (ACTION_JUMP_TO_SQUARE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        SQ_IN_JAIL, 0, 0, 0, NULL, 0, NULL);
      break;

    case COMMUNITY_GET_45_FROM_BANK:
      BlindlyTransferCash (RULE_BANK_PLAYER, CurrentRulesState.CurrentPlayer, 45, TRUE);
      PopAndRestartPhase (); /* End of movement. */
      break;

    case COMMUNITY_GET_50_FROM_EACH_PLAYER:
      PopPhase (); /* End movement before debt activity stacks up. */
      /* Cycle through the players starting at the current one, in reverse
      order, so that the payments stack up in the correct order. */
      OtherPlayer = CurrentRulesState.CurrentPlayer;
      while (TRUE)
      {
        OtherPlayer--; /* Go in backwards order. */
        if (OtherPlayer >= RULE_MAX_PLAYERS) /* If wrapped around. */
          OtherPlayer = (RULE_PlayerNumber) (CurrentRulesState.NumberOfPlayers - 1);
        if (OtherPlayer == CurrentRulesState.CurrentPlayer)
          break; /* All done, exit the while loop. */
        if (CurrentRulesState.Players[OtherPlayer].currentSquare < SQ_OFF_BOARD)
        {
          /* Charge this non-bankrupt player.  Stacks up a debt phase, but
             don't do a restart otherwise a bunch of restarts gets queued
             up (one for each player) and they all get applied to the top
             level debt, making debt notifications get sent several times
             for the same debt, which may cause user interface problems. */

          StackDebt (OtherPlayer, CurrentRulesState.CurrentPlayer, 50);
        }
      }

      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
      break;

    case COMMUNITY_GO_DIRECTLY_TO_GO:
    case CHANCE_GO_DIRECTLY_TO_GO:
      SwitchPhase (GF_MOVING_TOKEN, 0, 0, 0);
      MESS_SendAction (ACTION_JUMP_TO_SQUARE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        SQ_GO, 0, 0, 0, NULL, 0, NULL);
      break;

    case COMMUNITY_PAY_150_TO_BANK:
      PopPhase (); /* End movement before debt activity stacks up. */
      StackDebtAndRestart (CurrentRulesState.CurrentPlayer, RULE_BANK_PLAYER, 150);
      AddMoneyToFreeParkingPot (150);
      break;

    case COMMUNITY_GET_25_FROM_BANK:
      BlindlyTransferCash (RULE_BANK_PLAYER, CurrentRulesState.CurrentPlayer, 25, TRUE);
      PopAndRestartPhase (); /* End of movement. */
      break;

    case COMMUNITY_PAY_100_TO_BANK:
      PopPhase (); /* End movement before debt activity stacks up. */
      StackDebtAndRestart (CurrentRulesState.CurrentPlayer, RULE_BANK_PLAYER, 100);
      AddMoneyToFreeParkingPot (100);
      break;

    case CHANCE_GO_TO_READING_RAILROAD:
      SwitchPhase (GF_MOVING_TOKEN, 0, 0, 0);
      MESS_SendAction (ACTION_MOVE_FORWARDS, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        SQ_READING_RR, 0, 0, 0, NULL, 0, NULL);
      break;

    case CHANCE_GO_TO_NEAREST_UTILITY:
      if (SQ_ELECTRIC_COMPANY <= PlayerPntr->currentSquare &&
      PlayerPntr->currentSquare < SQ_WATER_WORKS)
        NewSquare = SQ_WATER_WORKS;
      else
        NewSquare = SQ_ELECTRIC_COMPANY;
      SwitchPhase (GF_MOVING_TOKEN, 0, 0, 0);
      MESS_SendAction (ACTION_MOVE_FORWARDS, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        NewSquare, 0, 0, 0, NULL, 0, NULL);
      break;

    case CHANCE_GET_150_FROM_BANK:
      BlindlyTransferCash (RULE_BANK_PLAYER, CurrentRulesState.CurrentPlayer, 150, TRUE);
      PopAndRestartPhase (); /* End of movement. */
      break;

    case CHANCE_GET_50_FROM_BANK:
      BlindlyTransferCash (RULE_BANK_PLAYER, CurrentRulesState.CurrentPlayer, 50, TRUE);
      PopAndRestartPhase (); /* End of movement. */
#if !FORHOTSEAT
      // increment get50 counter
      g_Card50Counter++;
#endif
      break;

    case CHANCE_PAY_50_TO_EACH_PLAYER:
      PopPhase (); /* End movement before debt activity stacks up. */
      /* Cycle through the players starting at the current one, in reverse
      order, so that the payments stack up in the correct order. */
      OtherPlayer = CurrentRulesState.CurrentPlayer;
      while (TRUE)
      {
        OtherPlayer--; /* Go in backwards order. */
        if (OtherPlayer >= RULE_MAX_PLAYERS) /* If wrapped around. */
          OtherPlayer = (RULE_PlayerNumber) (CurrentRulesState.NumberOfPlayers - 1);
        if (OtherPlayer == CurrentRulesState.CurrentPlayer)
          break; /* All done, exit the while loop. */
        if (CurrentRulesState.Players[OtherPlayer].currentSquare < SQ_OFF_BOARD)
        {
          /* Pay this non-bankrupt player.  Stacks up a debt phase, but
             don't do a restart otherwise a bunch of restarts gets queued
             up (one for each player) and they all get applied to the top
             level debt, making debt notifications get sent several times
             for the same debt, which may cause user interface problems. */

          StackDebt (CurrentRulesState.CurrentPlayer, OtherPlayer, 50);
        }
      }
      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
      break;

    case CHANCE_GO_TO_BOARDWALK:
      SwitchPhase (GF_MOVING_TOKEN, 0, 0, 0);
      MESS_SendAction (ACTION_MOVE_FORWARDS, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        SQ_BOARDWALK, 0, 0, 0, NULL, 0, NULL);
      break;

    case CHANCE_GO_TO_ST_CHARLES_PLACE:
      SwitchPhase (GF_MOVING_TOKEN, 0, 0, 0);
      MESS_SendAction (ACTION_MOVE_FORWARDS, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        SQ_ST_CHARLES_PLACE, 0, 0, 0, NULL, 0, NULL);
      break;

    case CHANCE_GO_TO_NEAREST_RAILROAD_PAY_DOUBLE_1:
    case CHANCE_GO_TO_NEAREST_RAILROAD_PAY_DOUBLE_2:
      if (PlayerPntr->currentSquare >= SQ_SHORT_LINE_RR ||
      PlayerPntr->currentSquare < SQ_READING_RR)
        NewSquare = SQ_READING_RR;
      else if (PlayerPntr->currentSquare < SQ_PENNSYLVANIA_RR)
        NewSquare = SQ_PENNSYLVANIA_RR;
      else if (PlayerPntr->currentSquare < SQ_BnO_RR)
        NewSquare = SQ_BnO_RR;
      else
        NewSquare = SQ_SHORT_LINE_RR;
      SwitchPhase (GF_MOVING_TOKEN, 0, 0, 0);
      MESS_SendAction (ACTION_MOVE_FORWARDS, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        NewSquare, 0, 0, 0, NULL, 0, NULL);
      break;

    case CHANCE_PAY_25_EACH_HOUSE_100_EACH_HOTEL:
      RULE_CountHousesAndHotels (&CurrentRulesState,
        CurrentRulesState.CurrentPlayer, &HouseCount, &HotelCount);
      PopPhase (); /* End movement before debt activity stacks up. */
      StackDebtAndRestart (CurrentRulesState.CurrentPlayer, RULE_BANK_PLAYER,
        25 * HouseCount + 100 * HotelCount);
      AddMoneyToFreeParkingPot (25 * HouseCount + 100 * HotelCount);
      MESS_SendAction2 (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_PROPERTY_FEES,
        25 * HouseCount + 100 * HotelCount,
        CurrentRulesState.CurrentPlayer,
        HouseCount,
        HotelCount,
        NULL, 0, NULL);
      break;

    case CHANCE_PAY_15_TO_BANK:
      PopPhase (); /* End movement before debt activity stacks up. */
      StackDebtAndRestart (CurrentRulesState.CurrentPlayer, RULE_BANK_PLAYER, 15);
      AddMoneyToFreeParkingPot (15);
      break;

    case CHANCE_GO_TO_ILLINOIS_AVENUE:
      SwitchPhase (GF_MOVING_TOKEN, 0, 0, 0);
      MESS_SendAction (ACTION_MOVE_FORWARDS, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        SQ_ILLINOIS_AVENUE, 0, 0, 0, NULL, 0, NULL);
      break;

    case CHANCE_GO_BACK_THREE_SPACES:
      NewSquare = PlayerPntr->currentSquare - 3;
      if (NewSquare < 0)
        NewSquare += (SQ_BOARDWALK + 1); /* Wrap around. */
      SwitchPhase (GF_MOVING_TOKEN, 0, 0, 0);
      MESS_SendAction (ACTION_MOVE_BACKWARDS, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        NewSquare, 0, 0, 0, NULL, 0, NULL);
      break;

    default:
      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_UNIMPLEMENTED_CARD, PickedCard,
        CurrentRulesState.CurrentPlayer,
        CurrentRulesState.Players[CurrentRulesState.CurrentPlayer].currentSquare,
        NULL, 0, NULL);
      PopAndRestartPhase ();
      break;
  }

  /* Put the card back on the bottom of the deck,
     if the player isn't keeping it. */

  if (PickedCard != CHANCE_GET_OUT_OF_JAIL_FREE &&
  PickedCard != COMMUNITY_GET_OUT_OF_JAIL_FREE)
    ReturnCard (&CurrentRulesState, PickedCard);
}



/*****************************************************************************
 * The current player's token has landed on the current square.  Do whatever
 * processing is needed for that square then go back to the previous
 * phase (usually GF_PREROLL).
 */

static void ActionLandedOnSquare (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerNumber      Owner;
  RULE_PlayerInfoPointer PlayerPntr;

  if (NewActionPntr->fromPlayer != RULE_BANK_PLAYER)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  if (CurrentPhaseM != GF_MOVING_TOKEN)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  // Let the lobby know whenever a token moves, so people watching a game
  // can see when someone moves several times during a turn (for double
  // dice or special cards).

  SendGameStatusToLobby (GAME_RUNNING, 0);

  PlayerPntr = CurrentRulesState.Players + CurrentRulesState.CurrentPlayer;

  switch (PlayerPntr->currentSquare)
  {
  case SQ_CHANCE_1:
  case SQ_CHANCE_2:
  case SQ_CHANCE_3:
  case SQ_COMMUNITY_CHEST_1:
  case SQ_COMMUNITY_CHEST_2:
  case SQ_COMMUNITY_CHEST_3:
    SwitchPhase (GF_WAIT_UNTIL_CARD_SEEN, 0, 0, 0);
    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
    break;


  case SQ_JUST_VISITING:
    if (CurrentRulesState.JustRolledOutOfJail)
    {
      /* Move the rolled amount if we just popped out of jail (prisoners are
         released to the Just Visiting square so that they can then move
         forwards normally from there).  Prisoners that pay to get out do
         so at the start of their turn and are handled elsewhere (they are
         treated like regular players landing on this square and get to
         roll the dice normally). */

      MESS_SendAction (ACTION_MOVE_FORWARDS, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        SQ_JUST_VISITING + CurrentRulesState.Dice[0] + CurrentRulesState.Dice[1],
        0, 0, 0, NULL, 0, NULL);
    }
    else /* Nothing to do on this square.  Roll again or end your turn. */
      PopAndRestartPhase ();
    break;


  case SQ_IN_JAIL:
    PlayerPntr->turnsInJail = 0;
    PopAndRestartPhase (); /* Your move ends when you hit jail. */
    break;


  case SQ_GO_TO_JAIL:
    MESS_SendAction (ACTION_JUMP_TO_SQUARE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      SQ_IN_JAIL, 0, 0, 0, NULL, 0, NULL);
    break;


  case SQ_MEDITERRANEAN_AVENUE:
  case SQ_BALTIC_AVENUE:
  case SQ_READING_RR:
  case SQ_ORIENTAL_AVENUE:
  case SQ_VERMONT_AVENUE:
  case SQ_CONNECTICUT_AVENUE:
  case SQ_ST_CHARLES_PLACE:
  case SQ_ELECTRIC_COMPANY:
  case SQ_STATES_AVENUE:
  case SQ_VIRGINIA_AVENUE:
  case SQ_PENNSYLVANIA_RR:
  case SQ_ST_JAMES_PLACE:
  case SQ_TENNESSEE_AVENUE:
  case SQ_NEW_YORK_AVENUE:
  case SQ_KENTUCKY_AVENUE:
  case SQ_INDIANA_AVENUE:
  case SQ_ILLINOIS_AVENUE:
  case SQ_BnO_RR:
  case SQ_ATLANTIC_AVENUE:
  case SQ_VENTNOR_AVENUE:
  case SQ_WATER_WORKS:
  case SQ_MARVIN_GARDENS:
  case SQ_PACIFIC_AVENUE:
  case SQ_NORTH_CAROLINA_AVENUE:
  case SQ_PENNSYLVANIA_AVENUE:
  case SQ_SHORT_LINE_RR:
  case SQ_PARK_PLACE:
  case SQ_BOARDWALK:
    Owner = CurrentRulesState.Squares[PlayerPntr->currentSquare].owner;
    if (Owner < RULE_MAX_PLAYERS)
    {
      /* Square owned by somebody, collect the rent. */
      PopPhase ();
      CollectRent (); /* Stacks debt collection phases. */
      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
    }
    else if (Owner == RULE_NOBODY_PLAYER)
    {
      /* Unowned square, can buy it. */
      SwitchPhase (GF_AUCTION_OR_BUY_DECISION, 0, 0, 0);
      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
    }
    else /* Weird square ownership. */
    {
#if _DEBUG
      if (Owner == RULE_BANK_PLAYER)
        DBUG_DisplayNonFatalErrorMessage ("ActionLandedOnSquare: Property square has bank as owner.");
      else if (Owner == RULE_ESCROW_PLAYER)
        DBUG_DisplayNonFatalErrorMessage ("ActionLandedOnSquare: Property square has escrow as owner.");
      else
        DBUG_DisplayNonFatalErrorMessage ("ActionLandedOnSquare: Property square has garbage data as owner.");
#endif
      PopAndRestartPhase (); /* Just ignore the bad square. */
    }
    break;


  case SQ_INCOME_TAX:
    if (CurrentRulesState.GameOptions.taxRate == 0)
    {
      /* Special case for short games, tax rate zero means always pay
         the flat rate without asking. */

      PopPhase ();
      StackDebtAndRestart (CurrentRulesState.CurrentPlayer, RULE_BANK_PLAYER,
      CurrentRulesState.GameOptions.flatTaxFee);
      AddMoneyToFreeParkingPot (CurrentRulesState.GameOptions.flatTaxFee);
      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_TAXES_CHARGED, CurrentRulesState.GameOptions.flatTaxFee,
        CurrentRulesState.CurrentPlayer, 0, NULL, 0, NULL);
    }
    else /* Ordinary tax situation, ask the user for a flat or fraction decision. */
    {
      SwitchPhase (GF_FLAT_OR_FRACTION_TAX_DECISION, 0, 0, 0);
      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
    }
    break;


  case SQ_LUXURY_TAX:
    PopPhase ();
    StackDebtAndRestart (CurrentRulesState.CurrentPlayer, RULE_BANK_PLAYER,
    CurrentRulesState.GameOptions.luxuryTaxAmount);
    AddMoneyToFreeParkingPot (CurrentRulesState.GameOptions.luxuryTaxAmount);
    break;


  case SQ_FREE_PARKING:
    if (CurrentRulesState.GameOptions.freeParkingPot)
    {
      /* Hand over the pot of money on Free Parking. */

      BlindlyTransferCash (RULE_BANK_PLAYER, CurrentRulesState.CurrentPlayer,
        CurrentRulesState.FreeParkingJackpotAmount,
        TRUE /* Show the cash transfer animation */);

      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_FREE_PARKING_POT_COLLECTED, CurrentRulesState.FreeParkingJackpotAmount,
        CurrentRulesState.CurrentPlayer, 0, NULL, 0, NULL);

      CurrentRulesState.FreeParkingJackpotAmount = 0;
      //AddMoneyToFreeParkingPot (0);  /* Force an update of the new zero value. */
      AddMoneyToFreeParkingPot (CurrentRulesState.GameOptions.freeParkingSeed); /* Someone said that the parking pot gets reseeded every time. */
    }
    PopAndRestartPhase ();
    break;


  case SQ_GO:
    PopAndRestartPhase ();
    break;


  default:
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_UMIMPLEMENTED_SQUARE, 0, CurrentRulesState.CurrentPlayer,
      PlayerPntr->currentSquare, NULL, 0, NULL);
    PopAndRestartPhase ();
    break;
  }

}



/*****************************************************************************
 * The user has decided how they will get out of jail.
 */

static void ActionExitJailDecision (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerInfoPointer PlayerPntr;

  if (CurrentPhaseM != GF_JAIL_ROLL_OR_PAY_OR_CARD_DECISION)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  if (NewActionPntr->fromPlayer != CurrentRulesState.CurrentPlayer)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  PlayerPntr = CurrentRulesState.Players + CurrentRulesState.CurrentPlayer;

  /* You can roll the dice if you haven't used up your turns in jail.  The Holly R's
     special short game jail rule allows you to roll always (if you fail the roll,
     you pay $50 immediately and get out anyways).  maximumTurnsInJail is zero when
     Holly's jail rule is enabled.  turnsInJail gets set to 99 when you can't roll
     any more. */

  if (NewActionPntr->numberA == 0 /* Wants to roll dice */ &&
  PlayerPntr->turnsInJail < 99 /* Dice rolling is allowed */)
  {
    /* Choose to roll the dice, and is still allowed to roll. */

    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action,
      TRUE, /* Rolling dice is a valid choice. */
      NewActionPntr->fromPlayer,
      NewActionPntr->numberA,
      NULL, 0, NULL);

    SwitchPhase (GF_WAIT_JAIL_ROLL, 0, 0, 0);
    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
  }
  else if (NewActionPntr->numberA == 1)
  {
    /* Allow the player to raise cash or go bankrupt, otherwise they will
       never get out of jail and the game will get stuck. */

    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action,
      TRUE, /* Paying money is a valid choice. */
      NewActionPntr->fromPlayer,
      NewActionPntr->numberA,
      NULL, 0, NULL);

    if (PlayerPntr->cash < CurrentRulesState.GameOptions.getOutOfJailFee)
      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_NEED_JAIL_MONEY,
        CurrentRulesState.GameOptions.getOutOfJailFee,
        CurrentRulesState.CurrentPlayer,
        PlayerPntr->cash,
        NULL, 0, NULL);

    /* When debt payment is done, will be in GF_GET_OUT_OF_JAIL phase,
       which will move the token out of jail. */

    SwitchPhase (GF_GET_OUT_OF_JAIL, 0, 0, 0);

    /* Add the debt payment to the phase stack. */

    StackDebtAndRestart (CurrentRulesState.CurrentPlayer, RULE_BANK_PLAYER,
      CurrentRulesState.GameOptions.getOutOfJailFee);
    AddMoneyToFreeParkingPot (CurrentRulesState.GameOptions.getOutOfJailFee);
  }
  else if (NewActionPntr->numberA == 2 &&
  PlayerPntr->turnsInJail < 99 && /* Not being forced to pay only */
  (CurrentRulesState.Cards[CHANCE_DECK].jailOwner == CurrentRulesState.CurrentPlayer ||
  CurrentRulesState.Cards[COMMUNITY_DECK].jailOwner == CurrentRulesState.CurrentPlayer))
  {
    /* Using the get out of jail card.  Return the card and go visiting. */

    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action,
      TRUE, /* Using a jail card is OK. */
      NewActionPntr->fromPlayer,
      NewActionPntr->numberA,
      NULL, 0, NULL);

    if (CurrentRulesState.Cards[CHANCE_DECK].jailOwner == CurrentRulesState.CurrentPlayer)
      TransferGetOutOfJail (CurrentRulesState.CurrentPlayer, RULE_NOBODY_PLAYER, CHANCE_DECK);
    else /* Using a card from the community deck. */
      TransferGetOutOfJail (CurrentRulesState.CurrentPlayer, RULE_NOBODY_PLAYER, COMMUNITY_DECK);

    SwitchPhase (GF_GET_OUT_OF_JAIL, 0, 0, 0);
    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
  }
  else /* Can't get out of jail using that action.  Try again (will prompt user). */
  {
    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action,
      FALSE, /* Try something else. */
      NewActionPntr->fromPlayer,
      0,
      NULL, 0, NULL);

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_NEED_JAIL_ROLL + NewActionPntr->numberA,
      CurrentRulesState.GameOptions.getOutOfJailFee,
      CurrentRulesState.CurrentPlayer,
      PlayerPntr->cash,
      NULL, 0, NULL);
  }
}



/*****************************************************************************
 * The game is over.  Find the winning player and tell everyone.  It is the
 * richest surviving player.
 */

static void GameOverDeclareWinner (void)
{
  long                    HighestWealth;
  int                     NumberOfBankruptPlayers;
  RULE_PlayerNumber       PlayerNo;
  RULE_PlayerNumber       RichestPlayer;
  RULE_SquareType         SquareNo;
  RULE_SquareInfoPointer  SquarePntr;
  long                    Wealth;

  CurrentRulesState.CurrentPlayer = RULE_NOBODY_PLAYER;

  NumberOfBankruptPlayers = 0;
  HighestWealth = -1; /* So winner with zero cash can win. */
  RichestPlayer = RULE_NOBODY_PLAYER;
  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    if (CurrentRulesState.Players[PlayerNo].currentSquare >= SQ_OFF_BOARD)
    {
      NumberOfBankruptPlayers++;
      continue; /* Ignore bankrupt players. */
    }

    Wealth = CurrentRulesState.Players[PlayerNo].cash;
    for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
    {
      SquarePntr = CurrentRulesState.Squares + SquareNo;

      if (SquarePntr->owner == PlayerNo)
      {
        Wealth += SquarePntr->mortgaged ?
          RULE_SquarePredefinedInfo[SquareNo].mortgageCost :
          RULE_SquarePredefinedInfo[SquareNo].purchaseCost;
        Wealth += SquarePntr->houses *
          RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost;
      }
    }
    if (Wealth > HighestWealth)
    {
      HighestWealth = Wealth;
      RichestPlayer = PlayerNo;
    }
  }

  /* We have a winner! */

  PushPhase (GF_GAME_FINISHED, RichestPlayer, RULE_NOBODY_PLAYER, HighestWealth);

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);

  /* Move all the players off the board, so that the AIs know they
     shouldn't be trying to do things. */

  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    if (CurrentRulesState.Players[PlayerNo].currentSquare < SQ_OFF_BOARD)
    {
      MESS_SendAction (NOTIFY_JUMP_TO_SQUARE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        SQ_OFF_BOARD,
        CurrentRulesState.Players[PlayerNo].currentSquare,
        PlayerNo,
        0,
        NULL, 0, NULL);

      CurrentRulesState.Players[PlayerNo].currentSquare = SQ_OFF_BOARD;
    }
  }

  /* Send an error message diagnosing the reason why in more detail. */

  if (CurrentRulesState.GameOptions.stopAtNthBankruptcy &&
  NumberOfBankruptPlayers >= CurrentRulesState.GameOptions.stopAtNthBankruptcy)
  {
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_GAME_WON_SHORT, RichestPlayer, HighestWealth,
      NumberOfBankruptPlayers, NULL, 0, NULL);

    SendGameStatusToLobby (GAME_OVER_BY_SHORT_GAME_RULES, RichestPlayer);
  }
  else if (CurrentRulesState.GameOptions.gameOverTimeLimit != 0 &&
  CurrentRulesState.GameDurationInSeconds >= CurrentRulesState.GameOptions.gameOverTimeLimit)
  {
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_GAME_WON_TIME, RichestPlayer, HighestWealth,
      CurrentRulesState.GameOptions.gameOverTimeLimit, NULL, 0, NULL);

    SendGameStatusToLobby (GAME_OVER_BY_TIME_LIMIT, RichestPlayer);
  }
  else /* Must be a normal win. */
  {
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_GAME_WON_NORMAL, RichestPlayer, HighestWealth, 0, NULL, 0, NULL);

    SendGameStatusToLobby (GAME_OVER_NORMAL, RichestPlayer);
  }

  MESS_UpdateLobbyGameFinished ();
}



/*****************************************************************************
 * The player wants to go bankrupt.
 */

static void ActionGoBankrupt (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_GameStateRecord      ComparisonGameState;
  CountHitSquareSetPointer  CountHitPntr;
  int                       i;
  long                      MortgageAmount;
  int                       NumberOfBankruptPlayers;
  RULE_PlayerNumber         PlayerNo;
  RULE_PropertySet          OldProperties;
  RULE_SquareType           SquareNo;
  RULE_SquareInfoPointer    SquarePntr;
  RULE_PlayerNumber         TestPlayer;
  BOOL                      UndoDone;

  PlayerNo = NewActionPntr->fromPlayer;

  if (CurrentPhaseM != GF_COLLECTING_PAYMENT)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  if (PlayerNo != CurrentRulesState.phaseStack[0].fromPlayer)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  if (!StackedRulesStates[0].ThisStateIsValid)
  {
#if _DEBUG
    DBUG_DisplayNonFatalErrorMessage ("ActionGoBankrupt: collecting payment didn't store initial game state on stack!");
#endif
    SaveGameStateInCurrentPhase ();
  }

  /* See if the player can avoid bankruptcy by mortgaging and selling houses,
     reject bankruptcy if so. */

  if (CashPlayerCouldRaiseIfNeeded (PlayerNo) >= CurrentRulesState.phaseStack[0].amount)
  {
    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action, FALSE, NewActionPntr->fromPlayer, 0, NULL, 0, NULL);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_CANNOT_GO_BANKRUPT,
      CurrentRulesState.phaseStack[0].toPlayer,
      PlayerNo,
      CurrentRulesState.phaseStack[0].amount,
      NULL, 0, NULL);

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);

    return;
  }

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE, /* Successfully went bankrupt. */
    NewActionPntr->fromPlayer,
    0,
    NULL, 0, NULL);

  /* Revert things to the way they were when the debt payment was first
     requested, if needed.  Don't want people to give away all their
     stuff before going bankrupt. */

  ComparisonGameState = StackedRulesStates[0];
  ComparisonGameState.GameDurationInSeconds = CurrentRulesState.GameDurationInSeconds;

  UndoDone = (memcmp (&CurrentRulesState, &ComparisonGameState,
    sizeof (RULE_GameStateRecord)) != 0);

  if (UndoDone)
    RestoreGameStateFromCurrentPhase (RESYNC_UNDO_BANKRUPTCY);

  /* Transfer all his property (via escrow) and the cash to the person or bank
     he went bankrupt to.  Remember to transfer old escrowed properties too. */

  MortgageAmount = 0;
  OldProperties = 0;
  for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
  {
    SquarePntr = CurrentRulesState.Squares + SquareNo;

    if (SquarePntr->owner == PlayerNo ||
    (SquarePntr->owner == RULE_ESCROW_PLAYER && SquarePntr->offeredInTradeTo ==
    PlayerNo))
    {
      OldProperties |= RULE_PropertyToBitSet (SquareNo);

      SellAllBuildingsOnMonopoly (SquareNo);

      if (SquarePntr->mortgaged)
        MortgageAmount += RULE_SquarePredefinedInfo[SquareNo].mortgageCost;

      TransferProperty (SquareNo, CurrentRulesState.phaseStack[0].toPlayer);
    }
  }

  /* Hand over all his cash.  Includes stuff from selling houses. */

  BlindlyTransferCash (PlayerNo, CurrentRulesState.phaseStack[0].toPlayer,
  CurrentRulesState.Players[PlayerNo].cash, TRUE);

  /* Send over the get out of jail cards too, if any.  If it is to the bank,
     the card goes back into the deck (not auctioned off, DaveP confirmed). */

  TransferGetOutOfJail (PlayerNo,
    CurrentRulesState.phaseStack[0].toPlayer, CHANCE_DECK);
  TransferGetOutOfJail (PlayerNo,
    CurrentRulesState.phaseStack[0].toPlayer, COMMUNITY_DECK);

  /* Cancel all immunities and futures he is involved in. */

  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
  {
    CountHitPntr = CurrentRulesState.CountHits + i;

    if (!CountHitPntr->tradedItem &&
    CountHitPntr->toPlayer != RULE_NOBODY_PLAYER &&
    (CountHitPntr->toPlayer == PlayerNo))// || CountHitPntr->fromPlayer == PlayerNo))
    {
      MESS_SendAction2 ((CountHitPntr->hitType == CHT_RENT_IMMUNITY) ?
        NOTIFY_IMMUNITY_COUNT : NOTIFY_FUTURE_RENT_COUNT,
        RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        CountHitPntr->toPlayer,
        0, /* Zero hits, means delete it. */
        SQ_OFF_BOARD, /* This isn't a use of a hit, just an update. */
        CountHitPntr->fromPlayer,
        CountHitPntr->properties,
        NULL, 0, NULL);

      CurrentRulesState.CountHits[i].toPlayer = RULE_NOBODY_PLAYER;
    }
  }

  /* Tell people about it.  In Star Wars Monopoly this triggers
     the Death Star animation. */

  if (UndoDone)
  {
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_UNDO_FOR_BANKRUPTCY_TO,
      CurrentRulesState.phaseStack[0].toPlayer,
      PlayerNo,
      CurrentRulesState.phaseStack[0].amount,
      NULL, 0, NULL);
  }
  else /* Hasn't done anything since the bankruptcy request.  No restore. */
  {
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_BANKRUPT_TO,
      CurrentRulesState.phaseStack[0].toPlayer,
      PlayerNo,
      CurrentRulesState.phaseStack[0].amount,
      NULL, 0, NULL);
  }

  /* Remove the bankrupt player from the board.  This way he dies with his
     ship in Star Wars Monopoly. */

  MESS_SendAction (NOTIFY_JUMP_TO_SQUARE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    SQ_OFF_BOARD,
    CurrentRulesState.Players[PlayerNo].currentSquare,
    PlayerNo,
    0,
    NULL, 0, NULL);

  CurrentRulesState.Players[PlayerNo].currentSquare = SQ_OFF_BOARD;

  /* Charge the mortgage transfer fee.  The de-escrowing will happen once
     the stacked up mortgage fee has been paid.  Don't do anything special
     if the player went bankrupt to the bank (the bank doesn't pay interest
     on transfers etc). */

  if (CurrentRulesState.phaseStack[0].toPlayer < RULE_MAX_PLAYERS)
  {
    SwitchPhase (GF_TRANSFER_ESCROW_PROPERTY, 0,
      CurrentRulesState.phaseStack[0].toPlayer, OldProperties);

    StackDebtAndRestart (CurrentRulesState.phaseStack[0].toPlayer, RULE_BANK_PLAYER,
      (MortgageAmount * CurrentRulesState.GameOptions.interestRate + 50) / 100);
  }
  else /* Went bankrupt to the bank.  Bank doesn't have to do anything special. */
    PopAndRestartPhase ();

  /* If this is the last surviving player, the game is over.  Note that we
     don't check for the other game over conditions here, since they rely on
     a comparison of wealth, which should be done at the end of a turn.  Also,
     this check is here because a player can go bankrupt and force the last
     player to go bankrupt too (if he can't pay mortgage fees), so we declare
     the remaining player a winner right now. */

  NumberOfBankruptPlayers = 0;
  for (TestPlayer = 0; TestPlayer < CurrentRulesState.NumberOfPlayers; TestPlayer++)
  {
    if (CurrentRulesState.Players[TestPlayer].currentSquare >= SQ_OFF_BOARD)
      NumberOfBankruptPlayers++;
  }
  if (NumberOfBankruptPlayers >= CurrentRulesState.NumberOfPlayers - 1)
    GameOverDeclareWinner (); /* Game over! */
}



/*****************************************************************************
 * The player who landed on an unowned square has finally made up their
 * mind about whether to buy the square or let the bank auction it off.
 */

static void ActionBuyOrAuctionDecision (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerInfoPointer PlayerPntr;
  RULE_SquareInfoPointer SquarePntr;
  RULE_SquareType        SquareNo;

  if (CurrentPhaseM != GF_AUCTION_OR_BUY_DECISION)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  if (NewActionPntr->fromPlayer != CurrentRulesState.CurrentPlayer)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE, /* Successfully requested purchase. */
    NewActionPntr->fromPlayer,
    NewActionPntr->numberA,     // true if we bought
    NULL, 0, NULL);

  PlayerPntr = CurrentRulesState.Players + CurrentRulesState.CurrentPlayer;
  SquareNo = PlayerPntr->currentSquare;
  SquarePntr = CurrentRulesState.Squares + SquareNo;

  if (NewActionPntr->numberA)
  {
    /* Buy the square.  Stack up a property transfer phase under a debt
       payment phase so that the transfer gets done after the payment is
       complete (no mortgaging the new property to raise cash - held
       by the RULE_ESCROW_PLAYER until the debt is paid). */

    TransferProperty (SquareNo, CurrentRulesState.CurrentPlayer);

    SwitchPhase (GF_TRANSFER_ESCROW_PROPERTY,
      0, CurrentRulesState.CurrentPlayer, RULE_PropertyToBitSet (SquareNo));

    StackDebtAndRestart (CurrentRulesState.CurrentPlayer, RULE_BANK_PLAYER,
      RULE_SquarePredefinedInfo[SquareNo].purchaseCost);

  }
  else /* Start an auction later on. */
  {
    SquarePntr->owner = RULE_BANK_PLAYER;
    PopAndRestartPhase ();
  }
}



/*****************************************************************************
 * The player wants to enter a bid in the current auction.
 */

static void ActionBid (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerSet    PlayersAllowed;
  RULE_PlayerNumber PlayerNo;

  PlayerNo = NewActionPntr->fromPlayer;

  if (CurrentPhaseM != GF_AUCTION)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  if (PlayerNo >= CurrentRulesState.NumberOfPlayers)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  PlayersAllowed = PlayersAllowedToBid ();

  if ((PlayersAllowed & (1 << PlayerNo)) == 0)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE, /* Placed a bid, maybe not a winning one. */
    NewActionPntr->fromPlayer,
    0,
    NULL, 0, NULL);

  if (NewActionPntr->numberA > CurrentRulesState.shared.auction.highestBid)
  {
    CurrentRulesState.shared.auction.highestBidder = NewActionPntr->fromPlayer;
    CurrentRulesState.shared.auction.highestBid = NewActionPntr->numberA;
    CurrentRulesState.shared.auction.goingCount = 0;
    CurrentRulesState.shared.auction.tickCount = 0;

#if RULE_MAX_BID_99999_HACK
    /* Make the highest bid possible in an auction be 99999.  Peg the values
       to that and end the auction right away if it is reached. */

    if (CurrentRulesState.shared.auction.highestBid >= 99999)
    {
      CurrentRulesState.shared.auction.highestBid = 99999;
      CurrentRulesState.shared.auction.tickCount =
        CurrentRulesState.GameOptions.auctionGoingTimeDelay;
      CurrentRulesState.shared.auction.goingCount = 2;
    }
#endif

    MESS_SendAction2 (NOTIFY_NEW_HIGH_BID, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      CurrentRulesState.shared.auction.highestBidder,
      CurrentRulesState.shared.auction.highestBid,
      CurrentRulesState.shared.auction.propertyBeingAuctioned,
      0,
      PlayersAllowed,
      NULL, 0, NULL);
  }
}



/*****************************************************************************
 * Approximately once a second this action happens, but there can be large
 * delays as animations play and the system event loop isn't accessed.
 * Used for running an auction.  Ignored most of the rest of the time.
 */

static DWORD PreviousTickCount;

static void ActionTick (void)
{
  RULE_GamePhase          CurrentPhase;
  DWORD                   CurrentTime;
  DWORD                   ElapsedSeconds;
  DWORD                   ElapsedTime;
  RULE_PlayerNumber       PlayerNo;
  RULE_PlayerSet          PlayersAllowed;
  RULE_SquareType         SquareNo;
  RULE_SquareInfoPointer  SquarePntr;

  CurrentPhase = CurrentPhaseM;

  /* Update the elapsed game time. */

  CurrentTime = GetTickCount ();
  ElapsedTime = CurrentTime - PreviousTickCount;
  if (ElapsedTime > 300000 /* 5 minutes */)
    PreviousTickCount = CurrentTime; /* Clock just started or game reloaded. */
  else
  {
    ElapsedSeconds = ElapsedTime / 1000;
    CurrentRulesState.GameDurationInSeconds += ElapsedSeconds;
    PreviousTickCount += ElapsedSeconds * 1000;
  }

  /* Allow new remote computers into the game if things are quiet.  Since
     a TICK doesn't get generated unless there are no messages pending,
     it should be quite quiet.*/

  // Restrict acceptance time if the user interface can't keep up.
  if (TRUE /* CurrentPhase == GF_WAIT_MOVE_ROLL || CurrentPhase == GF_ADDING_NEW_PLAYERS */)
  {
    /* OK, everything is safe and quiet.  Let in any new players and resync them. */

    if (MESS_LetNewComputersIntoTheGame ())
    {
      /* Yes, there were some new computers added. */

      SendResyncMessages (RULE_ALL_PLAYERS, RESYNC_NET_REFRESH);
    }
  }

  /* Check for idle players, only during game play. */

  if (CurrentRulesState.GameOptions.inactivityWarningTime > 0 &&
  CurrentPhase != GF_ADDING_NEW_PLAYERS &&
  CurrentPhase != GF_CONFIGURATION &&
  CurrentPhase != GF_PICKING_STARTING_ORDER &&
  CurrentPhase != GF_GAME_FINISHED &&
  CurrentPhase != GF_COLLECT_AI_PARAMETERS_FOR_SAVE)
  {
    for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
    {
      if (CurrentRulesState.GameDurationInSeconds -
      CurrentRulesState.Players[PlayerNo].timeOfLastActivity >=
      CurrentRulesState.GameOptions.inactivityWarningTime)
      {
        CurrentRulesState.Players[PlayerNo].inactivityCount++;
        CurrentRulesState.Players[PlayerNo].timeOfLastActivity =
          CurrentRulesState.GameDurationInSeconds;

        if (CurrentRulesState.Players[PlayerNo].inactivityCount < 6)
        {
          if (CurrentRulesState.Players[PlayerNo].inactivityCount >= 3)
          {
            /* Warn the player that time is running out.  Don't display the
               first couple of warnings to avoid annoying people. */

            MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
              TMN_ERROR_PLAYER_INACTIVE_WARNING,
              CurrentRulesState.Players[PlayerNo].inactivityCount * CurrentRulesState.GameOptions.inactivityWarningTime,
              PlayerNo,
              (6 - CurrentRulesState.Players[PlayerNo].inactivityCount) * CurrentRulesState.GameOptions.inactivityWarningTime,
              NULL, 0, NULL);
          }
        }
        else /* Time is up!  Replace this player with an AI. */
        {
          CurrentRulesState.Players[PlayerNo].inactivityCount = 0;
          CurrentRulesState.Players[PlayerNo].timeOfLastActivity =
            CurrentRulesState.GameDurationInSeconds;

          MESS_SendAction (ACTION_DISCONNECTED_PLAYER, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
            PlayerNo, 0, 0, 0, NULL, 0, NULL);
        }
      }
    }
  }

  /* Do timing actions in the relevant states. */

  if (CurrentPhase == GF_AUCTION)
  {
    if (++CurrentRulesState.shared.auction.tickCount >=
    CurrentRulesState.GameOptions.auctionGoingTimeDelay &&
    MESS_CurrentQueueSize () == 0 /* Also wait for queue to empty out here */)
    {
      CurrentRulesState.shared.auction.tickCount = 0;
      CurrentRulesState.shared.auction.goingCount++;

      MESS_SendAction (NOTIFY_AUCTION_GOING, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        CurrentRulesState.shared.auction.highestBidder,
        CurrentRulesState.shared.auction.highestBid,
        CurrentRulesState.shared.auction.propertyBeingAuctioned,
        CurrentRulesState.shared.auction.goingCount,
        NULL, 0, NULL);

      /* Insert a wait for everybody after the first "going" so that the
         frantic AIs can bid while others catch up (they keep on trying
         to bid while the game is waiting for sync - which results in a
         lot of unnecessary network traffic).  Slow computers could
         otherwise spend all their time loading the graphics and miss the
         whole auction. */

      if (!IgnoreWaitForEverybodyReady)
      {
        WaitForEverybodyReady( NOTIFY_AUCTION_GOING );
        IgnoreWaitForEverybodyReady = TRUE; /* Only wait once per auction. */
        return;
      }

      if (CurrentRulesState.shared.auction.goingCount >= 3)
      {
        /* End of auction.  Transfer the property after cash has been
           collected. */

        if (CurrentRulesState.shared.auction.highestBidder < RULE_MAX_PLAYERS)
        {
          if (CurrentRulesState.shared.auction.propertyBeingAuctioned <
          SQ_IN_JAIL) /* If not a building, it is a property. */
          {
            SquarePntr = CurrentRulesState.Squares +
              CurrentRulesState.shared.auction.propertyBeingAuctioned;

            /* Transfer an ordinary piece of property, after the cash
               is obtailed (put property in escrow, collect cash,
               unescrow it). */

            TransferProperty (CurrentRulesState.shared.auction.propertyBeingAuctioned,
              CurrentRulesState.shared.auction.highestBidder);

            SwitchPhase (GF_TRANSFER_ESCROW_PROPERTY,
              0, CurrentRulesState.shared.auction.highestBidder,
              RULE_PropertyToBitSet (CurrentRulesState.shared.auction.propertyBeingAuctioned));

            StackDebtAndRestart (CurrentRulesState.shared.auction.highestBidder,
              RULE_BANK_PLAYER, CurrentRulesState.shared.auction.highestBid);
          }
          else /* Sold a building to someone. */
          {
            /* Switch to the phase that collects the debt, then stack on top
               (it gets done first) one that asks them where to put the
               building.  That way the building count is correct during the
               debt collection (so nobody else can buy it). */

            PopPhase ();

            StackDebt (CurrentRulesState.shared.auction.highestBidder,
              RULE_BANK_PLAYER, CurrentRulesState.shared.auction.highestBid);

            PushPhase (GF_PLACE_BUILDING,
              CurrentRulesState.shared.auction.highestBidder,
              RULE_NOBODY_PLAYER,
              (CurrentRulesState.shared.auction.propertyBeingAuctioned == SQ_AUCTION_HOTEL) ?
                +1 : -1);

            MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
              0, 0, 0, 0, NULL, 0, NULL);
          }
        }
        else /* Nobody bought it, put it back into general use. */
        {
          if (CurrentRulesState.shared.auction.propertyBeingAuctioned <
          SQ_IN_JAIL) /* If not a building. */
          {
            SquarePntr = CurrentRulesState.Squares +
              CurrentRulesState.shared.auction.propertyBeingAuctioned;

            /* The property goes back to being unowned (not back to the
               bank for re-auction, but to nobody). */

            MESS_SendAction (NOTIFY_SQUARE_OWNERSHIP, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
              CurrentRulesState.shared.auction.propertyBeingAuctioned,
              RULE_NOBODY_PLAYER, /* New owner */
              SquarePntr->owner, /* Old owner */
              0,
              NULL, 0, NULL);

            SquarePntr->owner = RULE_NOBODY_PLAYER;
            SquarePntr->offeredInTradeTo = RULE_NOBODY_PLAYER;
            PopAndRestartPhase ();
          }
          else /* A building. */
          {
            /* Building didn't get sold.  Nothing special to do. */
            PopAndRestartPhase ();
          }
        }
      } /* If auction is over. */
    } /* If enough ticks have gone by. */
  }
  else if (CurrentPhase == GF_HOUSING_SHORTAGE_QUESTION)
  {
    /* Insert a wait for everybody at the start of a housing shortage
       question to make sure that everybody has a chance to see it. */

    if (CurrentRulesState.shared.auction.tickCount == 0 &&
    CurrentRulesState.shared.auction.goingCount == 0 &&
    !IgnoreWaitForEverybodyReady)
    {
      WaitForEverybodyReady (NOTIFY_HOUSING_SHORTAGE);
      IgnoreWaitForEverybodyReady = TRUE; /* Only do this once per housing shortage. */
      return;
    }

    if (++CurrentRulesState.shared.auction.tickCount >=
    CurrentRulesState.GameOptions.auctionGoingTimeDelay)
    {
      CurrentRulesState.shared.auction.tickCount = 0;
      CurrentRulesState.shared.auction.goingCount++;

      PlayerNo = CurrentRulesState.phaseStack[0].fromPlayer;
      SquareNo = CurrentRulesState.phaseStack[0].toPlayer;
      PlayersAllowed = PlayersWhoCanBuyABuilding (CurrentRulesState.
        phaseStack[0].amount > 0 /* TRUE for hotel */);

      /* Reduce the time delay if only one person can buy the building.  This
         is done here since the houses are free in this phase because it is
         almost an auction, otherwise players with little cash but empty
         building spots wouldn't show up (the bid price can be less than the
         normal building price). */

      if (PlayersAllowed == (1 << PlayerNo))
        CurrentRulesState.shared.auction.goingCount = 3;

      MESS_SendAction2 (NOTIFY_HOUSING_SHORTAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        PlayerNo,
        SquareNo,
        CurrentRulesState.phaseStack[0].amount, /* Free building count */
        CurrentRulesState.shared.auction.goingCount,
        PlayersAllowed,
        NULL, 0, NULL);


      if (CurrentRulesState.shared.auction.goingCount >= 3)
      {
        /* Guess nobody else wants this building, sell it to the original
           buyer.  Since the shortage question isn't interruptable, we
           already know he has enough money. */

        BlindlyTransferCash (PlayerNo, RULE_BANK_PLAYER,
          RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost, TRUE);

        BuildHouse (SquareNo);

        MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          TMN_ERROR_BUILDING_BOUGHT,
          PlayerNo,
          RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost,
          SquareNo,
          NULL, 0, NULL);

        PopAndRestartPhase ();
      }
    }
  }
  else if (CurrentPhase == GF_WAIT_START_TURN)
  {
    /* Waiting for the message queue to become empty before continuing on
       with the game (messages are held until they are transmitted in network
       play).  Otherwise the message queue could overflow, particularly for
       fast AI games. */

    if (MESS_CurrentQueueSize () == 0)
      MESS_SendAction (ACTION_START_TURN, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0,
        NULL, 0, NULL);
  }
  else if (CurrentPhase == GF_TRADE_FINISHED)
  {
    /* The game sits in the GF_TRADE_FINISHED state until the queue is empty. */

    if (MESS_CurrentQueueSize () == 0)
      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
  }
  else if (CurrentPhase == GF_COLLECT_AI_PARAMETERS_FOR_SAVE)
  {
    if (CurrentRulesState.GameDurationInSeconds -
    (DWORD) CurrentRulesState.phaseStack[0].amount > 100)
    {
      /* Ran out of time while waiting for AI players to send their states. */

      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_SAVE_GAME_FAILURE, 0,
        CurrentRulesState.phaseStack[0].toPlayer, 0, NULL, 0, NULL);
      FreeAISaveGameInfo ();
      PopAndRestartPhase ();
    }
  }
  else if (CurrentPhase == GF_WAIT_FOR_EVERYBODY_READY)
  {
    if (CurrentRulesState.GameDurationInSeconds -
    (DWORD) CurrentRulesState.phaseStack[0].amount >= 60)
    {
      /* Ran out of time while waiting for players to synchronize.  Stop
         waiting (set set of players to empty and notify people about it so
         that they know the wait has ended too). */

      CurrentRulesState.phaseStack[0].fromPlayer = 0;

      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
    }
  }
}



/*****************************************************************************
 * Kill any auction in progress.
 */

static void ActionKillAuctionCheat (RULE_ActionArgumentsPointer NewActionPntr)
{
  if (CurrentPhaseM != GF_AUCTION)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  if (CurrentRulesState.GameOptions.cheatingAllowed)
  {
    CurrentRulesState.shared.auction.tickCount =
      CurrentRulesState.GameOptions.auctionGoingTimeDelay;
    CurrentRulesState.shared.auction.goingCount = 2;

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_PLAYER_CHEATING, 0, NewActionPntr->fromPlayer, 0,
      NULL, 0, NULL);
  }
}



/*****************************************************************************
 * Change the amount of cash a player has.
 */

static void ActionCheatCash (RULE_ActionArgumentsPointer NewActionPntr)
{
  long          NewCash;
  long          OldCash;
  RULE_PlayerNumber  PlayerNo;

  PlayerNo = (RULE_PlayerNumber) NewActionPntr->numberA;
  NewCash = NewActionPntr->numberB;

  if (CurrentRulesState.GameOptions.cheatingAllowed &&
  PlayerNo < RULE_MAX_PLAYERS && NewCash >= 0)
  {
    OldCash = CurrentRulesState.Players[PlayerNo].cash;
    CurrentRulesState.Players[PlayerNo].cash = NewCash;
    NotifyCashChange (PlayerNo, NewCash - OldCash);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_PLAYER_CHEATING, 0, NewActionPntr->fromPlayer, 0,
      NULL, 0, NULL);
  }
}



/*****************************************************************************
 * Change the ownership of a square.
 */

static void ActionCheatOwner (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerNumber      PlayerNo;
  RULE_SquareType        SquareNo;
  RULE_SquareInfoPointer SquarePntr;

  PlayerNo = (RULE_PlayerNumber) NewActionPntr->numberA;
  SquareNo = NewActionPntr->numberB;
  SquarePntr = CurrentRulesState.Squares + SquareNo;

  if (CurrentRulesState.GameOptions.cheatingAllowed && PlayerNo < RULE_MAX_PLAYERS &&
  SquareNo >= 0 && SquareNo < SQ_MAX_SQUARE_TYPES &&
  RULE_SquarePredefinedInfo[SquareNo].group < SG_MAX_PROPERTY_GROUPS)
  {
    if (SquarePntr->owner == PlayerNo)
      PlayerNo = RULE_NOBODY_PLAYER; /* Already owned, so disown it. */

    MESS_SendAction (NOTIFY_SQUARE_OWNERSHIP, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      SquareNo, PlayerNo, SquarePntr->owner, 0,
      NULL, 0, NULL);

    SquarePntr->owner = PlayerNo;
    SquarePntr->offeredInTradeTo = RULE_NOBODY_PLAYER;

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_PLAYER_CHEATING, 0, NewActionPntr->fromPlayer, 0,
      NULL, 0, NULL);
  }
}



/*****************************************************************************
 * Test if it is possible to add or remove a house from the given square.
 * It looks at the even build rule, sees if the player has enough cash (unless
 * this is a sale or prepaid building from an auction), sees if the player
 * owns the square, sees if there is a monopoly, sees if the monopoly is
 * unmortgaged.  If Adding is TRUE then we're adding a house, FALSE for
 * removing a house.
 *
 * Also returns the number of free buildings in RemainingBuildingsPntr and
 * whether they are building a house or hotel in BuildingAHotelPntr (TRUE
 * for a hotel).  Both are only set if the overall result is TRUE.
 * Note that *RemainingBuildingsPntr can be negative if you are selling
 * phantom houses while in the hotel decomposition phase.
 *
 * The overall return value is TMN_NO_MESSAGE if successful, otherwise it
 * is an error message number and MessageArgumentPntr contains an optional
 * message argument (presumably you know the square and purchaser arguments).
 */

LANG_TextMessageNumber RULE_TestBuildingPlacement (
  RULE_GameStatePointer GameStatePntr, RULE_PlayerNumber Purchaser,
  RULE_SquareType SquareNo, BOOL Adding, BOOL *BuildingAHotelPntr,
  int *RemainingBuildingsPntr, long *MessageArgumentPntr)
{
  BOOL              BuildingAHotel;
  BOOL              NoCharge;
  int               FreeHouses;
  int               FreeHotels;
  int               HighestHousesInMonopoly;
  int               HouseCount;
  int               HotelCount;
  int               LowestHousesInMonopoly;
  int               i;
  RULE_SquareGroups MonopolyGroup;
  int               SquareHouses;
  RULE_SquareInfoPointer SquarePntr;

  SquarePntr = GameStatePntr->Squares + SquareNo;
  MonopolyGroup = RULE_SquarePredefinedInfo[SquareNo].group;

  /* See if the player has enough money to buy the building.
     Don't care about money if selling, or placing a building
     already bought in an auction.  Or if just before or during
     an auction and trying to find out if they allowed to bid. */

  if (GameStatePntr == &CurrentRulesState)
  {
    NoCharge = (CurrentPhaseM == GF_PLACE_BUILDING ||
    CurrentPhaseM == GF_AUCTION ||
    CurrentPhaseM == GF_HOUSING_SHORTAGE_QUESTION);
  }
  else
  {
    /* Some other partial game state without phases (probably user
       interface code).  Just assume there are no free buildings. */

    NoCharge = FALSE;
  }

  if (Adding && !NoCharge)
  {
    if (RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost >
    GameStatePntr->Players[Purchaser].cash)
    {
      if (MessageArgumentPntr != NULL)
      {
        *MessageArgumentPntr =
        RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost;
      }
      return TMN_ERROR_BUILDING_NEEDS_CASH;
    }
  }

  /* See if the square is one where houses can be built. */

  if (MonopolyGroup > SG_PARK_PLACE)
    return TMN_ERROR_BUILDING_ON_NONPROPERTY;

  /* See if the buyer of houses owns the square. */

  if (SquarePntr->owner != Purchaser)
    return TMN_ERROR_BUILDING_ON_UNOWNED;

  /* Has to own the other squares in the monopoly too.  They also can't
     be mortgaged.  Also find the lowest and highest number of houses
     on the monopoly squares. */

  LowestHousesInMonopoly = GameStatePntr->GameOptions.housesPerHotel;
  HighestHousesInMonopoly = 0;
  for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
  {
    if (RULE_SquarePredefinedInfo[i].group == MonopolyGroup)
    {
      if (GameStatePntr->Squares[i].owner != Purchaser)
        return TMN_ERROR_BUILDING_NEEDS_MONOPOLY;

      if (GameStatePntr->Squares[i].mortgaged)
        return TMN_ERROR_BUILDING_MORTGAGED;

      if (GameStatePntr->Squares[i].houses < LowestHousesInMonopoly)
        LowestHousesInMonopoly = GameStatePntr->Squares[i].houses;
      if (GameStatePntr->Squares[i].houses > HighestHousesInMonopoly)
        HighestHousesInMonopoly = GameStatePntr->Squares[i].houses;
    }
  }

  /* See if there is already a hotel on that
     square.  Can't add any more in that case. */

  if (Adding &&
  SquarePntr->houses >= GameStatePntr->GameOptions.housesPerHotel)
    return TMN_ERROR_BUILDING_MAXED_OUT;

  /* Count up the houses and hotels currently on the board. */

  HouseCount = HotelCount = 0;
  for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
  {
    SquareHouses = GameStatePntr->Squares[i].houses;
    if (SquareHouses < GameStatePntr->GameOptions.housesPerHotel)
      HouseCount += SquareHouses;
    else
      HotelCount++;
  }

  /* So how many are free?  Can have a negative FreeHouses if there are
     phantom houses during a hotel decomposition. */

  FreeHouses = GameStatePntr->GameOptions.maximumHouses - HouseCount;
  FreeHotels = GameStatePntr->GameOptions.maximumHotels - HotelCount;

  if (Adding)
  {
    /* Enforce the even build rule. */

    if (GameStatePntr->GameOptions.evenBuildRule &&
    SquarePntr->houses > LowestHousesInMonopoly)
    {
      if (MessageArgumentPntr != NULL)
        *MessageArgumentPntr = LowestHousesInMonopoly;
      return TMN_ERROR_BUILDING_NOT_EVEN;
    }

    if (SquarePntr->houses >= GameStatePntr->GameOptions.housesPerHotel - 1)
    {
      /* Trying to build a hotel. */

      BuildingAHotel = TRUE;
      if (FreeHotels < 1)
        return TMN_ERROR_BUILDING_NO_HOTELS;
    }
    else /* Building a house. */
    {
      BuildingAHotel = FALSE;
      if (FreeHouses < 1)
        return TMN_ERROR_BUILDING_NO_HOUSES;
    }
  }
  else /* Removing a hotel or house. */
  {
    if (SquarePntr->houses >= GameStatePntr->GameOptions.housesPerHotel)
    {
      /* Removing a hotel by breaking it down into houses. */

      BuildingAHotel = TRUE;

      /* See if there are enough houses to break down the hotel.
         If in the decomposition phase, ignore this test and temporarily
         allow more houses on the board than there should be. */

      if ((GameStatePntr != &CurrentRulesState || CurrentPhaseM != GF_DECOMPOSE_HOTEL) &&
      (FreeHouses < GameStatePntr->GameOptions.housesPerHotel - 1))
      {
        if (MessageArgumentPntr != NULL)
          *MessageArgumentPntr = GameStatePntr->GameOptions.housesPerHotel - 1;
        return TMN_ERROR_BUILDING_NO_BREAKDOWN_HOUSES;
      }
    }
    else /* Selling a house. */
    {
      BuildingAHotel = FALSE;

      /* Needs to be a house there before you can sell it. */

      if (SquarePntr->houses <= 0)
        return TMN_ERROR_SELL_NO_HOUSES;

      /* Enforce the even build rule while selling. */

      if (GameStatePntr->GameOptions.evenBuildRule &&
      SquarePntr->houses < HighestHousesInMonopoly)
      {
        if (MessageArgumentPntr != NULL)
          *MessageArgumentPntr = HighestHousesInMonopoly;
        return TMN_ERROR_BUILDING_NOT_EVEN;
      }
    }
  }

  if (BuildingAHotelPntr != NULL)
    *BuildingAHotelPntr = BuildingAHotel;

  if (RemainingBuildingsPntr != NULL)
    *RemainingBuildingsPntr = BuildingAHotel ? FreeHotels : FreeHouses;

  return TMN_NO_MESSAGE;
}



/*****************************************************************************
 * The user wants to buy a house (or maybe the last house that makes a hotel,
 * so it is really buying any kind of building).
 */

static void ActionBuyHouse (RULE_ActionArgumentsPointer NewActionPntr)
{
  BOOL              BuildingAHotel;
  long              ErrorArgument;
  LANG_TextMessageNumber ErrorNo;
  int               FreeBuildings;
  RULE_PlayerNumber PlayerNo;
  RULE_PlayerSet    PlayersAllowed;
  RULE_SquareType   SquareNo;

  PlayerNo = NewActionPntr->fromPlayer;
  SquareNo = NewActionPntr->numberA;

  /* Can only buy houses in certain phases. */
  if( NewActionPntr->numberD )
  {// Quicky requested - no BSSM lock nor state change.
    if (CurrentPhaseM != GF_WAIT_MOVE_ROLL &&
      CurrentPhaseM != GF_WAIT_END_TURN &&
      CurrentPhaseM != GF_WAIT_JAIL_ROLL &&
      CurrentPhaseM != GF_WAIT_UTILITY_ROLL &&
      CurrentPhaseM != GF_WAIT_UNTIL_CARD_SEEN &&
      CurrentPhaseM != GF_JAIL_ROLL_OR_PAY_OR_CARD_DECISION &&
      CurrentPhaseM != GF_FLAT_OR_FRACTION_TAX_DECISION &&
      CurrentPhaseM != GF_AUCTION_OR_BUY_DECISION &&
      CurrentPhaseM != GF_COLLECTING_PAYMENT &&
      CurrentPhaseM != GF_FREE_UNMORTGAGE &&
      CurrentPhaseM != GF_BUYSELLMORT &&
      CurrentPhaseM != GF_PLACE_BUILDING)
    {
      ErrorWrongPhase (NewActionPntr);
      return;
    }
  } else
  {// Regular rule, no lock no go.
    if (CurrentPhaseM != GF_BUYSELLMORT && CurrentPhaseM != GF_PLACE_BUILDING)
    {
      ErrorWrongPhase (NewActionPntr);
      return;
    }
    // Check for the right player - does not apply to a quicky sell
    if (CurrentPhaseM == GF_BUYSELLMORT && PlayerNo != CurrentRulesState.phaseStack[0].fromPlayer)
    {
      ErrorWrongPlayer (NewActionPntr);
      return;
    }

    if (CurrentPhaseM == GF_PLACE_BUILDING && PlayerNo != CurrentRulesState.phaseStack[0].fromPlayer)
    {
      ErrorWrongPlayer (NewActionPntr);
      return;
    }
  }


  if (SquareNo > SQ_BOARDWALK)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }


  /* See if it is legal to build there. */

  ErrorNo = RULE_TestBuildingPlacement (&CurrentRulesState, PlayerNo, SquareNo,
    TRUE, &BuildingAHotel, &FreeBuildings, &ErrorArgument);

  if (ErrorNo != TMN_NO_MESSAGE)
  {
    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action,
      FALSE, /* Can't build. */
      NewActionPntr->fromPlayer,
      0,
      NULL, 0, NULL);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      ErrorNo,
      ErrorArgument,
      PlayerNo,
      SquareNo,
      NULL, 0, NULL);

    return;
  }

  /* Make sure the guy is building the kind of building he bought at the auction. */

  if (CurrentPhaseM == GF_PLACE_BUILDING &&
  CurrentRulesState.phaseStack[0].amount != (BuildingAHotel ? 1 : -1))
  {
    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action,
      FALSE, /* Can't build. */
      NewActionPntr->fromPlayer,
      0,
      NULL, 0, NULL);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_AUCTION_WRONG_BUILDING,
      0,
      PlayerNo,
      SquareNo,
      NULL, 0, NULL);

    return;
  }

  /* Build, but check for a housing shortage first.  Ignore shortages while
     placing a building won at an auction.  Also skip the shortage question
     if only the given player can build - for Monopoly 2000 which doesn't
     have the text for notifying the other players that one player is getting
     away with the last few houses. */

  PlayersAllowed = PlayersWhoCanBuyABuilding (BuildingAHotel);

  if (PlayersAllowed != (1 << PlayerNo) &&
  CurrentPhaseM != GF_PLACE_BUILDING &&
  (FreeBuildings <= (BuildingAHotel ?
  CurrentRulesState.GameOptions.hotelShortageLevel :
  CurrentRulesState.GameOptions.houseShortageLevel)))
  {
    /* Interrupt whatever's going on and ask if anyone wants to
       buy the house/hotel.  Later on, will ask for a square if
       an auction happened, otherwise will let the guy buy it. */

    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action,
      FALSE, /* Didn't get a house right away. */
      NewActionPntr->fromPlayer,
      0,
      NULL, 0, NULL);

    if (CurrentPhaseM == GF_BUYSELLMORT)
      MESS_SendAction (NOTIFY_PLAYER_BUYSELLMORT, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        RULE_NOBODY_PLAYER, /* Player with exclusive mode - signal an end to it */
        0, /* Property set he is working on. */
        0, /* Cash owing in previous debt. */
        RULE_NOBODY_PLAYER, /* Player cash is owed to. */
        NULL, 0, NULL);

    PushPhase (GF_HOUSING_SHORTAGE_QUESTION, PlayerNo, (RULE_PlayerNumber) SquareNo,
      BuildingAHotel ? FreeBuildings : -FreeBuildings);

    IgnoreWaitForEverybodyReady = FALSE; /* Yes, we want to wait at least once. */

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
  }
  else /* No shortage, just buy the building. */
  {
    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action,
      TRUE, /* Got a building. */
      NewActionPntr->fromPlayer,
      0,
      NULL, 0, NULL);

    if (CurrentPhaseM != GF_PLACE_BUILDING)
      BlindlyTransferCash (PlayerNo, RULE_BANK_PLAYER,
        RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost, TRUE);

    BuildHouse (SquareNo);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_BUILDING_BOUGHT,
      PlayerNo,
      (CurrentPhaseM == GF_PLACE_BUILDING) ? 0 : RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost,
      SquareNo,
      NULL, 0, NULL);

    if (CurrentPhaseM == GF_PLACE_BUILDING)
      PopAndRestartPhase (); /* End of building placement phase. */
  }
}



/*****************************************************************************
 * The user wants to sell a house or all houses on a monopoly.
 */

static void ActionSellBuildings (RULE_ActionArgumentsPointer NewActionPntr)
{
  long                    ErrorArgument;
  LANG_TextMessageNumber  ErrorNo;
  int                     HouseCount;
  int                     Houses;
  int                     i;
  RULE_PlayerNumber       PlayerNo;
  RULE_SquareType         SquareNo;

  PlayerNo = NewActionPntr->fromPlayer;
  SquareNo = NewActionPntr->numberA;

  /* Can only sell houses in certain phases. */
  if( NewActionPntr->numberD )
  {// Quicky requested - no BSSM lock nor state change.
    if (CurrentPhaseM != GF_WAIT_MOVE_ROLL &&
      CurrentPhaseM != GF_WAIT_END_TURN &&
      CurrentPhaseM != GF_WAIT_JAIL_ROLL &&
      CurrentPhaseM != GF_WAIT_UTILITY_ROLL &&
      CurrentPhaseM != GF_WAIT_UNTIL_CARD_SEEN &&
      CurrentPhaseM != GF_JAIL_ROLL_OR_PAY_OR_CARD_DECISION &&
      CurrentPhaseM != GF_FLAT_OR_FRACTION_TAX_DECISION &&
      CurrentPhaseM != GF_AUCTION_OR_BUY_DECISION &&
      CurrentPhaseM != GF_COLLECTING_PAYMENT &&
      CurrentPhaseM != GF_FREE_UNMORTGAGE &&
      CurrentPhaseM != GF_BUYSELLMORT &&
      CurrentPhaseM != GF_DECOMPOSE_HOTEL)
    {
      ErrorWrongPhase (NewActionPntr);
      return;
    }
  } else
  {// Regular rule, no lock no go.
    if (CurrentPhaseM != GF_BUYSELLMORT && CurrentPhaseM != GF_DECOMPOSE_HOTEL)
    {
      ErrorWrongPhase (NewActionPntr);
      return;
    }
    // Check for the right player - does not apply to a quicky sell
    if (CurrentPhaseM == GF_BUYSELLMORT && PlayerNo != CurrentRulesState.phaseStack[0].fromPlayer)
    {
      ErrorWrongPlayer (NewActionPntr);
      return;
    }

    if (CurrentPhaseM == GF_DECOMPOSE_HOTEL && PlayerNo != CurrentRulesState.phaseStack[0].fromPlayer)
    {
      ErrorWrongPlayer (NewActionPntr);
      return;
    }
  }


  if (SquareNo > SQ_BOARDWALK)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }


  if (NewActionPntr->numberB) /* If selling everything. */
  {
    if (CurrentRulesState.Squares[SquareNo].owner != PlayerNo)
    {
      MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        NewActionPntr->action,
        FALSE, /* Can't sell buildings. */
        NewActionPntr->fromPlayer,
        0,
        NULL, 0, NULL);

      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_BUILDING_ON_UNOWNED, 0, PlayerNo, SquareNo,
        NULL, 0, NULL);
      return;
    }

    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action, TRUE, NewActionPntr->fromPlayer, 0,
      NULL, 0, NULL);

    SellAllBuildingsOnMonopoly (SquareNo);
  }
  else /* Just selling a single building. */
  {
    ErrorNo = RULE_TestBuildingPlacement (&CurrentRulesState, PlayerNo, SquareNo,
    FALSE, NULL, NULL, &ErrorArgument);

    if (ErrorNo == TMN_ERROR_BUILDING_NO_BREAKDOWN_HOUSES &&
    CurrentPhaseM != GF_DECOMPOSE_HOTEL)
    {
      /* Push a hotel decomposition phase and sell the hotel
         to get a bunch of phantom houses. */

      if (CurrentPhaseM == GF_BUYSELLMORT)
        MESS_SendAction (NOTIFY_PLAYER_BUYSELLMORT, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          RULE_NOBODY_PLAYER, /* Player with exclusive mode - signal an end to it */
          0, /* Property set he is working on. */
          0, /* Cash owing in previous debt. */
          RULE_NOBODY_PLAYER, /* Player cash is owed to. */
          NULL, 0, NULL);

      PushPhase (GF_DECOMPOSE_HOTEL, PlayerNo,  RULE_NOBODY_PLAYER, SquareNo);

      SaveGameStateInCurrentPhase (); /* Save unsold buildings state for cancel. */

      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
    }
    else if (ErrorNo != TMN_NO_MESSAGE)
    {
      MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        NewActionPntr->action,
        FALSE, /* Can't build. */
        NewActionPntr->fromPlayer,
        0,
        NULL, 0, NULL);

      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        ErrorNo,
        ErrorArgument,
        PlayerNo,
        SquareNo,
        NULL, 0, NULL);

      return;
    }

    Houses = --CurrentRulesState.Squares[SquareNo].houses;

#if _DEBUG
    if (Houses < 0)
      DBUG_DisplayNonFatalErrorMessage ("ActionSellBuildings: Negative number of houses!");
#endif

    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action, TRUE, NewActionPntr->fromPlayer, 0,
      NULL, 0, NULL);

    BlindlyTransferCash (RULE_BANK_PLAYER, PlayerNo,
      (RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost + 1) / 2, TRUE);

    MESS_SendAction (NOTIFY_SQUARE_HOUSES, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      SquareNo, Houses, CurrentRulesState.GameOptions.housesPerHotel, 0,
      NULL, 0, NULL);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_BUILDING_SOLD,
      PlayerNo,
      (RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost + 1) / 2,
      SquareNo,
      NULL, 0, NULL);

    if (CurrentPhaseM == GF_DECOMPOSE_HOTEL)
    {
      /* See if this makes the number of houses on the board drop down to the
         total number of houses in the game.  If so, turn off the hotel
         decomposition mode. */

      HouseCount = 0;
      for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
      {
        Houses = CurrentRulesState.Squares[i].houses;
        if (Houses < CurrentRulesState.GameOptions.housesPerHotel)
          HouseCount += Houses;
      }

      if (HouseCount <= CurrentRulesState.GameOptions.maximumHouses)
        PopAndRestartPhase ();
      else /* Just restart to let the player know how many more houses he must sell. */
        MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
    }

    // If the player is raising cash without a BSSM lock, restart phase (to detect sufficient money)
    // Note that an AI could have a BSSM lock - the funds wont transfer until they are done (phase pops)
    if( NewActionPntr->numberD && (CurrentPhaseM == GF_COLLECTING_PAYMENT) )
      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);

  }
}



/*****************************************************************************
 * The user wants to cancel the forced sale of houses to decompose a hotel.
 */

static void ActionCancelDecomposition (RULE_ActionArgumentsPointer NewActionPntr)
{
  if (CurrentPhaseM != GF_DECOMPOSE_HOTEL)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  /* Make sure it is the right player for the phase. */

  if (NewActionPntr->fromPlayer != CurrentRulesState.phaseStack[0].fromPlayer)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE,
    NewActionPntr->fromPlayer,
    0,
    NULL, 0, NULL);

  /* Restore the game state to the way it was before the hotel decomposition. */

  if (StackedRulesStates[0].ThisStateIsValid)
    RestoreGameStateFromCurrentPhase (RESYNC_UNDO_HOTEL_DECOMPOSITION);
#if _DEBUG
  else
    DBUG_DisplayNonFatalErrorMessage ("ActionCancelDecomposition: No saved game state to restore!");
#endif

  /* Get rid of the decomposition phase and go back to whatever
     we were doing before. */

  PopAndRestartPhase ();
}



/*****************************************************************************
 * The user wants to mortgage or unmortgage a property.
 */

static void ActionMortgaging (RULE_ActionArgumentsPointer NewActionPntr)
{
  long          Fees;
  int           i;
  RULE_SquareGroups  MonopolyGroup;
  BOOL          NoInterestFee;
  RULE_PlayerNumber  PlayerNo;
  RULE_SquareType    SquareNo;

  PlayerNo = NewActionPntr->fromPlayer;
  SquareNo = NewActionPntr->numberA;

  if( NewActionPntr->numberD )
  {// Quicky requested - no BSSM lock nor state change.
    if (CurrentPhaseM != GF_WAIT_MOVE_ROLL && // This is copied from the logic which accepts a BSSM lock request.
      CurrentPhaseM != GF_WAIT_END_TURN &&
      CurrentPhaseM != GF_WAIT_JAIL_ROLL &&
      CurrentPhaseM != GF_WAIT_UTILITY_ROLL &&
      CurrentPhaseM != GF_WAIT_UNTIL_CARD_SEEN &&
      CurrentPhaseM != GF_JAIL_ROLL_OR_PAY_OR_CARD_DECISION &&
      CurrentPhaseM != GF_FLAT_OR_FRACTION_TAX_DECISION &&
      CurrentPhaseM != GF_AUCTION_OR_BUY_DECISION &&
      CurrentPhaseM != GF_COLLECTING_PAYMENT &&
      CurrentPhaseM != GF_FREE_UNMORTGAGE &&
      CurrentPhaseM != GF_BUYSELLMORT)
    {
      ErrorWrongPhase (NewActionPntr);
      return;
    }
  } else
  {// Regular rule, no lock no go.
    if (CurrentPhaseM != GF_BUYSELLMORT && CurrentPhaseM != GF_FREE_UNMORTGAGE)
    {
      ErrorWrongPhase (NewActionPntr);
      return;
    }
    // Check for the right player - does not apply to a quicky mortgage.
    if ((CurrentPhaseM == GF_BUYSELLMORT && PlayerNo != CurrentRulesState.phaseStack[0].fromPlayer) ||
      (CurrentPhaseM == GF_FREE_UNMORTGAGE && PlayerNo != CurrentRulesState.phaseStack[0].toPlayer))
    {
      ErrorWrongPlayer (NewActionPntr);
      return;
    }
  }


  if (CurrentRulesState.Squares[SquareNo].owner != PlayerNo)
  {
    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action,
      FALSE, /* Can't mortgage or unmortgage. */
      NewActionPntr->fromPlayer,
      0,
      NULL, 0, NULL);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_MORTGAGING_ON_UNOWNED, 0, PlayerNo, SquareNo,
      NULL, 0, NULL);
    return;
  }

  /* All buildings must be gone from all of the squares in the monopoly before
     it can be mortgaged.  Fortunately railroads and utilities never have
     houses so they fall through here naturally. */

  MonopolyGroup = RULE_SquarePredefinedInfo[SquareNo].group;
  for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
  {
    if (RULE_SquarePredefinedInfo[i].group == MonopolyGroup)
    {
      if (CurrentRulesState.Squares[i].houses > 0)
      {
        MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          NewActionPntr->action,
          FALSE, /* Can't mortgage or unmortgage. */
          NewActionPntr->fromPlayer,
          0,
          NULL, 0, NULL);

        MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          TMN_ERROR_MORTGAGING_HOUSES, 0, PlayerNo, SquareNo,
          NULL, 0, NULL);
        return;
      }
    }
  }

  if (CurrentRulesState.Squares[SquareNo].mortgaged)
  {
    /* Unmortgage it.  Free of interest charge if in escow mode. */

    Fees = RULE_SquarePredefinedInfo[SquareNo].mortgageCost;

    NoInterestFee = (CurrentPhaseM == GF_FREE_UNMORTGAGE &&
    (RULE_PropertyToBitSet (SquareNo) & (CurrentRulesState.phaseStack[0].amount)));

    if (!NoInterestFee)
      Fees += (Fees * CurrentRulesState.GameOptions.interestRate + 50 /* Rounding */) / 100;

    /* If he can't pay the fee, then don't do the unmortgage. */

    if (CurrentRulesState.Players[PlayerNo].cash < Fees)
    {
      MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        NewActionPntr->action,
        FALSE, /* Can't mortgage or unmortgage. */
        NewActionPntr->fromPlayer,
        0,
        NULL, 0, NULL);

      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_MORTGAGE_NO_CASH, Fees, PlayerNo, SquareNo,
        NULL, 0, NULL);
      return;
    }

    /* Bill him. */

    BlindlyTransferCash (PlayerNo, RULE_BANK_PLAYER, Fees, TRUE);

    if (NoInterestFee)
    {
      /* Mark off this one as used escrow freebie.  Also restart to let
         the user interfaces know about the change. */

      CurrentRulesState.phaseStack[0].amount &= ~RULE_PropertyToBitSet (SquareNo);
      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
    }
  }
  else
  {
    /* Mortgage the square.  Give money to the player. */

    Fees = RULE_SquarePredefinedInfo[SquareNo].mortgageCost;
    BlindlyTransferCash (RULE_BANK_PLAYER, PlayerNo, Fees, TRUE);
  }

  /* Mortgage status has changed if we get here.  Flip it over. */

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE, /* Did it. */
    NewActionPntr->fromPlayer,
    !CurrentRulesState.Squares[SquareNo].mortgaged,
    NULL, 0, NULL);

  MESS_SendAction (NOTIFY_SQUARE_MORTGAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    SquareNo,
    !CurrentRulesState.Squares[SquareNo].mortgaged, /* New state */
    CurrentRulesState.Squares[SquareNo].mortgaged,  /* Old state */
    0, NULL, 0, NULL);

  /* Tell everybody about it, just for user interface feedback. */

  if (CurrentRulesState.Squares[SquareNo].mortgaged)
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_UNMORTGAGE_INFO, Fees, PlayerNo, SquareNo,
      NULL, 0, NULL);
  else
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_MORTGAGE_INFO, Fees, PlayerNo, SquareNo,
      NULL, 0, NULL);

  // If the player is raising cash without a BSSM lock, restart phase (to detect sufficient money)
  // Note that an AI could have a BSSM lock - the funds wont transfer until they are done (phase pops)
  if( NewActionPntr->numberD && !CurrentRulesState.Squares[SquareNo].mortgaged &&
    (CurrentPhaseM == GF_COLLECTING_PAYMENT) )
      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);


  CurrentRulesState.Squares[SquareNo].mortgaged =
    !CurrentRulesState.Squares[SquareNo].mortgaged;
}



/*****************************************************************************
 * Let the calling program see if it can unmortgage the given square.
 * FreeUnMortgagingSet is the set of properties that can be unmortgaged
 * for no extra fee (get it from the NOTIFY_FREE_UNMORTGAGING message
 * and leave it zero for normal unmortgaging).  Returns TRUE if the given
 * player can currently unmortgage the given square (assuming that he
 * has exclusive buy/sell/mortgage control).
 */

BOOL RULE_TestUnMortgaging (RULE_PlayerNumber PlayerNo, RULE_SquareType SquareNo,
RULE_GameStatePointer GameStatePntr, RULE_PropertySet FreeUnMortgagingSet)
{
  long  Fees;
  BOOL  NoInterestFee;

  if (SquareNo < 0 || SquareNo >= SQ_IN_JAIL ||
  PlayerNo >= RULE_MAX_PLAYERS || GameStatePntr == NULL)
    return FALSE; /* Rudimentary safety checks for array bounds. */

  if (GameStatePntr->Squares[SquareNo].owner != PlayerNo ||
  !GameStatePntr->Squares[SquareNo].mortgaged)
    return FALSE;  /* Have to own it and it has to be mortgaged. */

  /* Free of interest charge if in escow mode. */

  Fees = RULE_SquarePredefinedInfo[SquareNo].mortgageCost;

  NoInterestFee = (RULE_PropertyToBitSet (SquareNo) & FreeUnMortgagingSet);

  if (!NoInterestFee)
    Fees += (Fees * GameStatePntr->GameOptions.interestRate + 50 /* Rounding */) / 100;

  /* If he can't pay the fee, then don't allow the unmortgage. */

  if (GameStatePntr->Players[PlayerNo].cash < Fees)
    return FALSE;

  return TRUE;
}



/*****************************************************************************
 * The user has finished unmortgaging things that were transfered from
 * some other player.
 */

static void ActionFreeUnmortgageDone (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerNumber  PlayerNo;

  PlayerNo = NewActionPntr->fromPlayer;

  if (PlayerNo != CurrentRulesState.phaseStack[0].toPlayer)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  if (CurrentPhaseM != GF_FREE_UNMORTGAGE)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE /* Did it. */,
    NewActionPntr->fromPlayer,
    0,
    NULL, 0, NULL);

  PopAndRestartPhase ();
}



/*****************************************************************************
 * The user has decided how to pay taxes.
 */

static void ActionTaxDecision (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerNumber  PlayerNo;
  RULE_SquareType    SquareNo;
  long          TaxFee;

  PlayerNo = NewActionPntr->fromPlayer;
  TaxFee = 0;

  if (PlayerNo != CurrentRulesState.CurrentPlayer)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  if (CurrentPhaseM != GF_FLAT_OR_FRACTION_TAX_DECISION)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  if (NewActionPntr->numberA)
  {
    /* Do fractional taxes.  Add up the player's tax assets, mortgaged
       properties count at full value, as do houses. */

    for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
    {
      if (CurrentRulesState.Squares[SquareNo].owner == PlayerNo)
      {
        TaxFee += RULE_SquarePredefinedInfo[SquareNo].purchaseCost;
        TaxFee += RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost *
          CurrentRulesState.Squares[SquareNo].houses;
      }
    }
    TaxFee += CurrentRulesState.Players[PlayerNo].cash;

    /* Ok, now that we have his total wealth, find the tax percentage.
       Use normal rounding rules. */

    TaxFee = (TaxFee * CurrentRulesState.GameOptions.taxRate + 50) / 100;
  }
  else /* Flat fee tax.  Note: also charged elsewhere in code for short game situation. */
  {
    TaxFee = CurrentRulesState.GameOptions.flatTaxFee;
  }

  /* Pay the taxes. */

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE /* Taxes paid. */,
    NewActionPntr->fromPlayer,
    NewActionPntr->numberA, // TRUE if fraction
    NULL, 0, NULL);

  PopPhase ();
  StackDebtAndRestart (PlayerNo, RULE_BANK_PLAYER, TaxFee);
  AddMoneyToFreeParkingPot (TaxFee);

  MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    TMN_ERROR_TAXES_CHARGED, TaxFee, PlayerNo, 0,
    NULL, 0, NULL);
}



/*****************************************************************************
 * A little utility for switching the trading mode back to editing, with
 * the given player as the editor.
 */

static void SwitchToTradeEditing (RULE_PlayerNumber NewEditor)
{
  SwitchPhase (GF_EDITING_TRADE,
    CurrentRulesState.phaseStack[0].fromPlayer /* Old proposer */,
    NewEditor /* Editor */,
    0 /* Not used */);

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * A utility for clearing out the current trade.  Making it a blank slate.
 */

static void SetEmptyTrade (void)
{
  int i, j;

  /* Clear the trading information fields to set up a null trade.  Also don't
     touch any escrowed squares that need to be handed out, left over from the
     previous trade or purchase. */

  for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
  {
    if (CurrentRulesState.Squares[i].owner < RULE_MAX_PLAYERS)
      CurrentRulesState.Squares[i].offeredInTradeTo = RULE_NOBODY_PLAYER;
  }

  /* Reset the player's cash trading information. */

  for (i = 0; i < RULE_MAX_PLAYERS; i++)
  {
    for (j = 0; j < RULE_MAX_PLAYERS; j++)
      CurrentRulesState.Players[i].shared.trading.cashGivenInTrade[j] = 0;
  }

  /* Reset the jail card trading info. */

  for (i = 0; i < MAX_DECK_TYPES; i++)
    CurrentRulesState.Cards[i].jailOfferedInTradeTo = RULE_NOBODY_PLAYER;

  /* Reset immunity trading stuff. */

  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
  {
    if (CurrentRulesState.CountHits[i].tradedItem)
    {
      CurrentRulesState.CountHits[i].toPlayer = RULE_NOBODY_PLAYER;
      CurrentRulesState.CountHits[i].tradedItem = FALSE;
    }
  }
}



/*****************************************************************************
 * The user wants to start trading or restart editing if trading is already
 * going on.
 */

static void ActionStartTradeEditing (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerNumber  PlayerNo;

  PlayerNo = NewActionPntr->fromPlayer;

  /* Only allow valid players to start a trade. */

  if (PlayerNo >= CurrentRulesState.NumberOfPlayers ||
  CurrentRulesState.Players[PlayerNo].currentSquare >= SQ_OFF_BOARD)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  /* Start in a phase which isn't trading related and can be restarted later. */

  if (CurrentPhaseM != GF_WAIT_MOVE_ROLL &&
  CurrentPhaseM != GF_WAIT_END_TURN && // new for TURN DONE
  CurrentPhaseM != GF_WAIT_JAIL_ROLL &&
  CurrentPhaseM != GF_WAIT_UTILITY_ROLL &&
  CurrentPhaseM != GF_WAIT_UNTIL_CARD_SEEN &&
  CurrentPhaseM != GF_JAIL_ROLL_OR_PAY_OR_CARD_DECISION &&
  CurrentPhaseM != GF_FLAT_OR_FRACTION_TAX_DECISION &&
  CurrentPhaseM != GF_AUCTION_OR_BUY_DECISION &&
  CurrentPhaseM != GF_COLLECTING_PAYMENT &&
  CurrentPhaseM != GF_FREE_UNMORTGAGE &&
  CurrentPhaseM != GF_TRADE_ACCEPTANCE /* Special for trade resume */)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  /* Is this guy going back to trading during the acceptance phase?  If so, he
     has to be one of the guys involved in the trade. */

  if (CurrentPhaseM == GF_TRADE_ACCEPTANCE)
  {
    if (CurrentRulesState.phaseStack[0].amount /* Private trade */ &&
    !PlayerInvolvedInTrade (PlayerNo))
    {
      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_TRADE_CANT_EDIT_DURING_ACCEPT, 0, PlayerNo, 0,
        NULL, 0, NULL);

      MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        NewActionPntr->action,  FALSE, /* Error in starting trade editing. */
        NewActionPntr->fromPlayer, 0,NULL, 0, NULL);
      return;
    }

    /* OK, restart the trade editing. */

    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        NewActionPntr->action,  TRUE, /* Succes in starting trade editing */
        NewActionPntr->fromPlayer, 0,NULL, 0, NULL);

    SwitchToTradeEditing (PlayerNo);

    return;
  }

  /* Don't allow trading until the current trade is entirely finished,
     including debt resolution and escrow. */

  if (CurrentRulesState.TradeInProgress)
  {
    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        NewActionPntr->action, FALSE, NewActionPntr->fromPlayer, 0,NULL, 0, NULL);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, PlayerNo,
      TMN_ERROR_CANT_TRADE_UNTIL_SETTLED, 0, PlayerNo, 0,
      NULL, 0, NULL);

    return;
  }

  /* Starting a new trade. */

  SetEmptyTrade ();

  /* Start trade editing mode, interrupting some other mode. */

  PushPhase (GF_EDITING_TRADE, PlayerNo /* Proposer by default */,
    PlayerNo /* Editor */, 0 /* Not used */);

  CurrentRulesState.TradeInProgress = TRUE;

  IgnoreWaitForEverybodyReady = FALSE; /* Do synchronization just once. */

  /* Restart the trading phase after making sure everybody is ready so that it
     will tell the other players about the current trade and who is editing it. */

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action, TRUE, /* Success in starting trade editing */
    NewActionPntr->fromPlayer, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * The player wants to change the current trade.
 */

static void ActionTradeItem (RULE_ActionArgumentsPointer NewActionPntr)
{
  CountHitSquareSetPointer  CountHitPntr;
  CountHitTypes             CountHitType;
  CountHitSquareSetPointer  FoundHitPntr;
  CountHitSquareSetPointer  FreeHitPntr;
  RULE_PlayerNumber         FromPlayer;
  RULE_SquareGroups         GroupNo;
  int                       i;
  RULE_PlayerNumber         PlayerNo;
  RULE_PropertySet          PropertySet;
  BOOL                      ShowTheNoMoreImmunitiesErrorMessage = FALSE;
  RULE_SquareType           SquareNo;
  RULE_PlayerNumber         ToPlayer;
  RULE_TradeItemKind        TradeItem;
  long                      Value;

  PlayerNo = NewActionPntr->fromPlayer;

  /* Can only edit the trade in the right phase. */

  if (CurrentPhaseM != GF_EDITING_TRADE)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  /* Only allow the editor to do the editing. */

  if (PlayerNo != CurrentRulesState.phaseStack[0].toPlayer)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  FromPlayer = (RULE_PlayerNumber) NewActionPntr->numberA;
  ToPlayer = (RULE_PlayerNumber) NewActionPntr->numberB;
  TradeItem = NewActionPntr->numberC;
  Value = NewActionPntr->numberD;
  PropertySet = NewActionPntr->numberE;

  if (ToPlayer >= CurrentRulesState.NumberOfPlayers ||
  CurrentRulesState.Players[ToPlayer].currentSquare >= SQ_OFF_BOARD)
    goto ErrorMessageExit; /* Destination player isn't valid. */

  switch (TradeItem)
  {
  case TIK_CASH:
    if (FromPlayer >= CurrentRulesState.NumberOfPlayers ||
    CurrentRulesState.Players[FromPlayer].currentSquare >= SQ_OFF_BOARD ||
    Value < 0)
      goto ErrorMessageExit;

    /* Check that the other player isn't trading back cash to the first
       player - wipe out any cash going in the other direction.  This
       avoids a player trading some cash for more cash - illegal under
       the rules since only the bank is allowed to loan money. */

    if (CurrentRulesState.Players[ToPlayer].shared.trading.
    cashGivenInTrade[FromPlayer] != 0)
    {
      CurrentRulesState.Players[ToPlayer].shared.trading.
        cashGivenInTrade[FromPlayer] = 0;
      MESS_SendAction (NOTIFY_TRADE_ITEM, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        ToPlayer, FromPlayer, TIK_CASH, 0 /* New amount in other direction */,
        NULL, 0, NULL);
    }

    CurrentRulesState.Players[FromPlayer].shared.trading.
      cashGivenInTrade[ToPlayer] = Value;
    break;


  case TIK_SQUARE: /* FromPlayer is implied by square owner. */
    if (Value < 0 || Value >= SQ_MAX_SQUARE_TYPES ||
    RULE_PropertyToBitSet (Value) == 0 ||
    (FromPlayer = CurrentRulesState.Squares[Value].owner) >= RULE_MAX_PLAYERS)
      goto ErrorMessageExit; /* Bad inputs or unowned or escrowed square. */

    if (FromPlayer == ToPlayer)
    {
      CurrentRulesState.Squares[Value].offeredInTradeTo = RULE_NOBODY_PLAYER;
      ToPlayer = RULE_NOBODY_PLAYER;
    }
    else
    {
      CurrentRulesState.Squares[Value].offeredInTradeTo = ToPlayer;
    }
    break;


  case TIK_JAIL_CARD: /* FromPlayer is implied by card owner. */
    if (Value < 0 || Value >= MAX_DECK_TYPES ||
    (FromPlayer = CurrentRulesState.Cards[Value].jailOwner) >= RULE_MAX_PLAYERS)
      goto ErrorMessageExit; /* Bad inputs or unowned jail card. */

    if (FromPlayer == ToPlayer)
    {
      CurrentRulesState.Cards[Value].jailOfferedInTradeTo = RULE_NOBODY_PLAYER;
      ToPlayer = RULE_NOBODY_PLAYER;
    }
    else
      CurrentRulesState.Cards[Value].jailOfferedInTradeTo = ToPlayer;
    break;


  case TIK_IMMUNITY:
  case TIK_FUTURE_RENT:
    if (FromPlayer >= CurrentRulesState.NumberOfPlayers ||
    CurrentRulesState.Players[FromPlayer].currentSquare >= SQ_OFF_BOARD ||
    PropertySet == 0)
      goto ErrorMessageExit;

    if (TradeItem == TIK_IMMUNITY &&
    !CurrentRulesState.GameOptions.immunitiesTradingAllowed)
      goto ErrorMessageExit;

    if (TradeItem == TIK_FUTURE_RENT &&
    !CurrentRulesState.GameOptions.futureRentTradingAllowed)
      goto ErrorMessageExit;

    /* Make it fit in the signed byte used for trade item storage. */

    if (Value > 127) Value = 127;
    if (Value < -128) Value = -128;

    if (TradeItem == TIK_FUTURE_RENT)
      CountHitType = CHT_FUTURE_RENT;
    else
      CountHitType = CHT_RENT_IMMUNITY;

    /* Find an existing matching trade item. */

    FoundHitPntr = FreeHitPntr = NULL;
    for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
    {
      CountHitPntr = CurrentRulesState.CountHits + i;

      if (CountHitPntr->properties == PropertySet &&
      CountHitPntr->toPlayer == ToPlayer &&
//      CountHitPntr->fromPlayer == FromPlayer &&
      ((CountHitTypes) CountHitPntr->hitType) == CountHitType &&
      CountHitPntr->tradedItem)
      {
        FoundHitPntr = CountHitPntr;
        break;
      }

      if (FreeHitPntr == NULL && CountHitPntr->toPlayer == RULE_NOBODY_PLAYER)
        FreeHitPntr = CountHitPntr;
    }

    if (FoundHitPntr)
    {
      /* Change an existing traded immunity / future's count. */

      if (Value != 0)
        FoundHitPntr->hitCount = Value;
      else /* Remove this trading item. */
      {
        FoundHitPntr->toPlayer = RULE_NOBODY_PLAYER;
        FoundHitPntr->tradedItem = FALSE;
      }
    }
    else if (FreeHitPntr)
    {
      /* Create a new traded immunity / future. */

      FreeHitPntr->properties = PropertySet;
      FreeHitPntr->fromPlayer = FromPlayer;
      FreeHitPntr->toPlayer = ToPlayer;
      FreeHitPntr->hitType = CountHitType;
      FreeHitPntr->tradedItem = TRUE;
      FreeHitPntr->hitCount = Value;
    }
    else /* Ran out of immunities / futures. */
    {
      ShowTheNoMoreImmunitiesErrorMessage = TRUE;
      goto ErrorMessageExit;
    }
    break;


  default:
    goto ErrorMessageExit;
  }

  /* Ok, it's a good trade.  */

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action, TRUE, NewActionPntr->fromPlayer, 0,
    NULL, 0, NULL);

  if (CurrentRulesState.phaseStack[0].fromPlayer != PlayerNo)
  {
    /* We have a new proposer. */

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, PlayerNo,
      TMN_ERROR_TRADE_CHANGING, 0, PlayerNo, 0,
      NULL, 0, NULL);

    CurrentRulesState.phaseStack[0].fromPlayer = PlayerNo;
  }

  /* Tell everybody about the new item.  Do this after the
     TMN_ERROR_TRADE_CHANGING so that the AI can do some processing. */

  MESS_SendAction2 (NOTIFY_TRADE_ITEM, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    FromPlayer, ToPlayer, TradeItem, Value, PropertySet,
    NULL, 0, NULL);

  /* Warning about trading squares where there are houses. */

  if (TradeItem == TIK_SQUARE &&
  CurrentRulesState.Squares[Value].offeredInTradeTo != RULE_NOBODY_PLAYER)
  {
    GroupNo = RULE_SquarePredefinedInfo[Value].group;
    for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
    {
      if (RULE_SquarePredefinedInfo[SquareNo].group == GroupNo &&
      CurrentRulesState.Squares[SquareNo].houses > 0)
      {
        MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, PlayerNo,
          TMN_ERROR_TRADE_HOUSE_SALE,
          0,
          CurrentRulesState.Squares[SquareNo].owner,
          SquareNo,
          NULL, 0, NULL);
        break;
      }
    }
  }

  return; /* Successful exit here. */


ErrorMessageExit:

  /* Failed if we get here.  Tell the player they can't do that. */

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action, FALSE, NewActionPntr->fromPlayer, 0,
    NULL, 0, NULL);

  if (ShowTheNoMoreImmunitiesErrorMessage)
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_IMMUNITY_OUT_OF, RULE_MAX_COUNT_HIT_SETS, PlayerNo, 0,
      NULL, 0, NULL);
  else /* Ordinary error message. */
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, PlayerNo,
      TMN_ERROR_CANT_TRADE_THAT, 0, PlayerNo, 0,
      NULL, 0, NULL);
}



/*****************************************************************************
 * The player wants to clear out the trading list.
 */

static void ActionClearTradeItems (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerNumber   PlayerNo;

  PlayerNo = NewActionPntr->fromPlayer;

  /* Can only edit the trade in the right phase. */

  if (CurrentPhaseM != GF_EDITING_TRADE)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  /* Only allow the editor to do the editing. */

  if (PlayerNo != CurrentRulesState.phaseStack[0].toPlayer)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  SetEmptyTrade ();

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action, TRUE, NewActionPntr->fromPlayer, 0,
    NULL, 0, NULL);

  /* Restart the trade editing to tell everybody about the trade state. */

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * Just clear out the immunities or futures that are being traded between
 * two particular players.  Needed because the Star Wars game uses one
 * icon to hold all the immunities (or futures) between two players.
 */

static void ActionClearTradedImmunitiesOrFutures (RULE_ActionArgumentsPointer NewActionPntr)
{
  CountHitSquareSetPointer  CountHitPntr;
  int                       i;
  RULE_PlayerNumber         PlayerNo;

  PlayerNo = NewActionPntr->fromPlayer;

  /* Can only edit the trade in the right phase. */

  if (CurrentPhaseM != GF_EDITING_TRADE)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  /* Only allow the editor to do the editing. */

  if (PlayerNo != CurrentRulesState.phaseStack[0].toPlayer)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action, TRUE, NewActionPntr->fromPlayer, 0,
    NULL, 0, NULL);

  /* Ignore invalid TO player numbers, so we don't accidentally wipe out records
     marked with RULE_NOBODY_PLAYER which means something special. */

  if (NewActionPntr->numberB >= RULE_MAX_PLAYERS)
    return;

  /* Remove the relevant immunities or futures.  But only traded ones. */

  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
  {
    CountHitPntr = CurrentRulesState.CountHits + i;

    if (//CountHitPntr->fromPlayer == (RULE_PlayerNumber) NewActionPntr->numberA &&
    CountHitPntr->toPlayer == (RULE_PlayerNumber) NewActionPntr->numberB &&
    CountHitPntr->hitType == (unsigned int) NewActionPntr->numberC &&
    CountHitPntr->tradedItem)
    {
      /* Tell the Star Wars game about the immunity being deleted, it doesn't
         fully use the lists of active items sent by ResendTradeProposal. */

      MESS_SendAction2 (NOTIFY_TRADE_ITEM, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        CountHitPntr->fromPlayer,
        CountHitPntr->toPlayer,
        CountHitPntr->hitType == CHT_FUTURE_RENT ? TIK_FUTURE_RENT : TIK_IMMUNITY,
        0 /* Count of zero for deletion hint */,
        CountHitPntr->properties,
        NULL, 0, NULL);

      /* Deallocate it from our internal state. */

      CountHitPntr->toPlayer = RULE_NOBODY_PLAYER;
    }
  }
}



/*****************************************************************************
 * The player has decided whether to accept or reject the trade.  Enter
 * his vote.  Ignore players who aren't supposed to vote.
 */

static void ActionTradeAccept (RULE_ActionArgumentsPointer NewActionPntr)
{
  BOOL                    AllAccepted;
  long                    Assets;
  long                    Cash;
  RULE_CardDeckTypes      Deck;
  long                    Expenses;
  BOOL                    GettingSomething;
  BOOL                    GivingSomething;
  int                     i;
  long                    MortgageValueTransferedToPlayer [RULE_MAX_PLAYERS];
  RULE_PlayerNumber       PlayerNo;
  RULE_PlayerNumber       OtherPlayer;
  RULE_PlayerInfoPointer  PlayerPntr;
  RULE_PropertySet        PropertyTransferedToPlayer [RULE_MAX_PLAYERS];
  RULE_PlayerNumber       ProposingPlayer;
  RULE_PlayerNumber       SomePlayer;
  RULE_SquareType         SquareNo;
  RULE_SquareInfoPointer  SquarePntr;

  PlayerNo = NewActionPntr->fromPlayer;
  PlayerPntr = CurrentRulesState.Players + PlayerNo;
  ProposingPlayer = CurrentRulesState.phaseStack[0].fromPlayer;

  /* Can only accept in the right phase. */

  if (CurrentPhaseM != GF_TRADE_ACCEPTANCE)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  /* Only allow players that are allowed to vote. */

  if (PlayerNo >= RULE_MAX_PLAYERS || PlayerPntr->currentSquare >= SQ_OFF_BOARD ||
  (CurrentRulesState.phaseStack[0].amount /* Private trade */ &&
  !PlayerInvolvedInTrade (PlayerNo)))
  {
    /* Don't tell them about it.  This happens normally if you propose a trade
       for some other people, and you automatically accept it when the system
       doesn't care what you do. */

    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action,
      FALSE, /* You were ignored. */
      NewActionPntr->fromPlayer,
      0,
      NULL, 0, NULL);

    return;
  }

  /* Don't allow non-participants to reject the trade.  They can only
     accept or edit it. */

  if (!PlayerInvolvedInTrade (PlayerNo) &&
  NewActionPntr->numberA == FALSE)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  /* If we get here, your trade acceptance will be used. */

#if !FORHOTSEAT
  // Shouldn't happen, but 3 means a non-involved AI accepted.
  if (NewActionPntr->numberB != 3)
    TradeStatus = NewActionPntr->numberB;
#endif
  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE, /* Your action was used. */
    NewActionPntr->fromPlayer,
    0,
    NULL, 0, NULL);

  /* Did the player accept it?  If he rejected it, cancel the whole thing. */

  if (!NewActionPntr->numberA)
  {
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_TRADE_REJECTED, 0, PlayerNo, 0,
      NULL, 0, NULL);

    /* Cancel the trade. */

    SetEmptyTrade ();

    SwitchPhase (GF_TRADE_FINISHED, 0, 0, 0);

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);

    return;
  }

  /* See if the player involved is actually doing a trade.  He has to
     both get something and give something, but it doesn't have to be
     to the same person.  While we spin through the traded items, keep
     track of tradeable assets. */

  GettingSomething = FALSE;
  GivingSomething = FALSE;
  Assets = CurrentRulesState.Players[PlayerNo].cash;
  Expenses = 0;

  for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
  {
    SquarePntr = CurrentRulesState.Squares + SquareNo;
    if (SquarePntr->owner < RULE_MAX_PLAYERS &&
    SquarePntr->offeredInTradeTo == PlayerNo)
      GettingSomething = TRUE;
    else if (SquarePntr->offeredInTradeTo < RULE_MAX_PLAYERS &&
    SquarePntr->owner == PlayerNo)
      GivingSomething = TRUE;

    /* Have to pay the mortgage transfer fee on
       received mortgaged property. */

    if (SquarePntr->owner < RULE_MAX_PLAYERS &&
    SquarePntr->offeredInTradeTo == PlayerNo &&
    SquarePntr->mortgaged)
      Expenses += (RULE_SquarePredefinedInfo[SquareNo].mortgageCost *
      CurrentRulesState.GameOptions.interestRate + 50) / 100;

    /* Can theoretically mortgage properties that weren't traded.
       Can't count the ones you get in a trade (held in escrow),
       or ones already held in escrow. */

    if (SquarePntr->owner == PlayerNo &&
    SquarePntr->offeredInTradeTo >= RULE_MAX_PLAYERS &&
    !SquarePntr->mortgaged)
      Assets += RULE_SquarePredefinedInfo[SquareNo].mortgageCost;

    /* Can always sell your own houses, even on squares you are trading
       away (they will be automatically sold anyways). */

    if (SquarePntr->owner == PlayerNo)
      Assets += (SquarePntr->houses *
      RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost + 1) / 2;
  }

  /* Check who is getting and giving the jail cards. */

  for (Deck = 0; Deck < MAX_DECK_TYPES; Deck++)
  {
    if (CurrentRulesState.Cards[Deck].jailOfferedInTradeTo == PlayerNo)
      GettingSomething = TRUE;
    else if (CurrentRulesState.Cards[Deck].jailOfferedInTradeTo < RULE_MAX_PLAYERS &&
    CurrentRulesState.Cards[Deck].jailOwner == PlayerNo)
      GivingSomething = TRUE;
  }

  /* Who is getting and giving immunities? */

  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
  {
    if (CurrentRulesState.CountHits[i].toPlayer != RULE_NOBODY_PLAYER &&
    CurrentRulesState.CountHits[i].tradedItem)
    {
      if (CurrentRulesState.CountHits[i].toPlayer == PlayerNo)
        GettingSomething = TRUE;

      if (CurrentRulesState.CountHits[i].fromPlayer == PlayerNo)
        GivingSomething = TRUE;
    }
  }

  /* How about cash?  Who is getting and how is giving? */

  for (SomePlayer = 0;
  SomePlayer < CurrentRulesState.NumberOfPlayers; SomePlayer++)
    for (OtherPlayer = 0;
    OtherPlayer < CurrentRulesState.NumberOfPlayers; OtherPlayer++)
    {
      Cash = CurrentRulesState.Players[SomePlayer].shared.trading.
        cashGivenInTrade[OtherPlayer];
      if (Cash > 0)
      {
        if (SomePlayer == PlayerNo)
          GivingSomething = TRUE;
        if (OtherPlayer == PlayerNo)
          GettingSomething = TRUE;

        /* Only giving cash counts, the stuff you get may arrive too late
           to be useful, depending on debt resolution order. */

        if (SomePlayer == PlayerNo)
          Expenses += Cash;
      }
    }

  /* Legal trades have all players either both giving and getting, or the
     player is totally out of the trade (give nothing and get nothing).
     You can't only get or only give. */

  if (GivingSomething != GettingSomething)
  {
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_TRADE_GIVE_AND_GET, 0, PlayerNo, 0,
      NULL, 0, NULL);
    //SwitchToTradeEditing (PlayerNo);//Old - just cancel everything!

    // Cancel the trade.
    SetEmptyTrade ();
    SwitchPhase (GF_TRADE_FINISHED, 0, 0, 0);
    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
    return;
  }

  /* Check if the trade would make the player go bankrupt. */

  if (Expenses > Assets)
  {
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, PlayerNo,
      TMN_ERROR_TRADE_BANKRUPTCY, Expenses, PlayerNo, Assets,
      NULL, 0, NULL);
    //SwitchToTradeEditing (PlayerNo);

    // Cancel the trade.
    SetEmptyTrade ();
    SwitchPhase (GF_TRADE_FINISHED, 0, 0, 0);
    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
    return;
  }

  /* Mark down this player's acceptance. */

  PlayerPntr->shared.trading.tradeAccepted = TRUE;

  MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    TMN_ERROR_TRADE_ACCEPTED, 0, PlayerNo, 0,
    NULL, 0, NULL);

  /* See if this means everybody has accepted. */

  AllAccepted = TRUE;
  for (SomePlayer = 0;
  AllAccepted && (SomePlayer < CurrentRulesState.NumberOfPlayers);
  SomePlayer++)
  {
    PlayerPntr = CurrentRulesState.Players + SomePlayer;

    if (PlayerPntr->currentSquare < SQ_OFF_BOARD &&
    (!CurrentRulesState.phaseStack[0].amount /* Not private trade */ ||
    PlayerInvolvedInTrade (SomePlayer)) &&
    !PlayerPntr->shared.trading.tradeAccepted)
      AllAccepted = FALSE;
  }

  if (!AllAccepted)
  {
    /* Redisplay the acceptance status. */

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);

    return; /* Waiting for more acceptances to come in. */
  }

  /* Ok, the trade has been accepted, carry it out.  First set up
     things so that the trade is marked as finished after all the
     debts get unstacked.  Note that Immunities will be activated
     in the GF_TRADE_FINISHED phase. */

  SwitchPhase (GF_TRADE_FINISHED, 0, 0, 0);

  /* Move property squares to escrow and add an escrow phase and maybe
     a debt payment phase for transfer fees for mortgaged property. */

  for (SomePlayer = 0; SomePlayer < RULE_MAX_PLAYERS; SomePlayer++)
  {
    PropertyTransferedToPlayer [SomePlayer] = 0;
    MortgageValueTransferedToPlayer [SomePlayer] = 0;
  }

  /* Transfer the traded squares to escrow.  Also sell any houses
     on traded squares. */

  for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++)
  {
    SquarePntr = CurrentRulesState.Squares + SquareNo;
    SomePlayer = SquarePntr->offeredInTradeTo;

    if (SquarePntr->owner < RULE_MAX_PLAYERS &&
    SomePlayer < RULE_MAX_PLAYERS /* If it was traded */)
    {
      PropertyTransferedToPlayer [SomePlayer] |= RULE_PropertyToBitSet (SquareNo);

      SellAllBuildingsOnMonopoly (SquareNo);

      if (SquarePntr->mortgaged)
      {
        MortgageValueTransferedToPlayer [SomePlayer] +=
          RULE_SquarePredefinedInfo[SquareNo].mortgageCost;
      }

      TransferProperty (SquareNo, SomePlayer);
    }
  }

  /* Charge the players who had mortgaged squares transfered, and
     schedule a de-escrow for after the fees have been paid.  Do
     this in order from the proposing (last changer of the trade)
     player. */

  SomePlayer = ProposingPlayer;
  while (TRUE)
  {
    /* Requesition an escrow phase for the player so he gets
       his new property, but stacked under the debt payment phase. */

    if (PropertyTransferedToPlayer [SomePlayer] != 0)
      PushPhase (GF_TRANSFER_ESCROW_PROPERTY, 0, SomePlayer,
      PropertyTransferedToPlayer [SomePlayer]);

    /* Pay any interest fees on transfered property, this happens before
       the de-escrow because it is nearer the top of the stack. */

    if (MortgageValueTransferedToPlayer [SomePlayer] > 0)
      StackDebt (SomePlayer, RULE_BANK_PLAYER,
      (MortgageValueTransferedToPlayer [SomePlayer] *
      CurrentRulesState.GameOptions.interestRate + 50) / 100);

    SomePlayer--;
    if (SomePlayer >= RULE_MAX_PLAYERS) /* Wrapped around. */
      SomePlayer = (RULE_PlayerNumber) (CurrentRulesState.NumberOfPlayers - 1);
    if (SomePlayer == ProposingPlayer)
      break; /* All done. */
  }

  /* Move jail cards to destined people. */

  for (Deck = 0; Deck < MAX_DECK_TYPES; Deck++)
  {
    SomePlayer = CurrentRulesState.Cards[Deck].jailOfferedInTradeTo;
    if (SomePlayer < RULE_MAX_PLAYERS)
      TransferGetOutOfJail (CurrentRulesState.Cards[Deck].jailOwner,
      SomePlayer, Deck);
  }

  /* Move cash to destined people.  Again in a certain order. This happens
     first since cash isn't escrowed (the bank doesn't have an interest). */

  SomePlayer = ProposingPlayer;
  while (TRUE)
  {
    OtherPlayer = SomePlayer;
    while (TRUE)
    {
      Cash = CurrentRulesState.Players[SomePlayer].shared.trading.
        cashGivenInTrade[OtherPlayer];
      if (Cash > 0)
        StackDebt (SomePlayer, OtherPlayer, Cash);

      OtherPlayer--;
      if (OtherPlayer >= RULE_MAX_PLAYERS) /* Wrapped? */
        OtherPlayer = (RULE_PlayerNumber) (CurrentRulesState.NumberOfPlayers - 1);
      if (OtherPlayer == SomePlayer)
        break;
    }

    SomePlayer--;
    if (SomePlayer >= RULE_MAX_PLAYERS) /* Wrapped around. */
      SomePlayer = (RULE_PlayerNumber) (CurrentRulesState.NumberOfPlayers - 1);
    if (SomePlayer == ProposingPlayer)
      break; /* All done. */
  }

  /* Ok, let it rip! */

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * The player has finished editing the trade and either wants to pass it
 * on to someone else for editing or wants to have people vote on it.  He
 * can also cancel the whole trade.
 */

static void ActionTradeEditingDone (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_CardDeckTypes          Deck;
  RULE_ActionArgumentsRecord  FakeActionMessage;
  int                         i;
  int                         OpCode;
  RULE_PlayerNumber           OtherPlayer;
  RULE_PlayerNumber           PlayerNo;
  RULE_PlayerNumber           SomePlayer;
  RULE_SquareType             SquareNo;
  BOOL                        TradingSomething;

  PlayerNo = NewActionPntr->fromPlayer;

  /* Can only finish editing the trade in the right phase. */

  if (CurrentPhaseM != GF_EDITING_TRADE)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  /* Only allow the editor to stop editing. */

  if (PlayerNo != CurrentRulesState.phaseStack[0].toPlayer)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE, /* Your action was used. */
    NewActionPntr->fromPlayer,
    0,
    NULL, 0, NULL);

  /* See if the trade contains anything.  If it's an empty trade, just
     forget it. */

  TradingSomething = FALSE;
  for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES && !TradingSomething; SquareNo++)
    if (CurrentRulesState.Squares[SquareNo].owner < RULE_MAX_PLAYERS &&
    CurrentRulesState.Squares[SquareNo].offeredInTradeTo < RULE_MAX_PLAYERS)
      TradingSomething = TRUE;

  for (Deck = 0; Deck < MAX_DECK_TYPES && !TradingSomething; Deck++)
    if (CurrentRulesState.Cards[Deck].jailOfferedInTradeTo < RULE_MAX_PLAYERS)
      TradingSomething = TRUE;

  for (SomePlayer = 0; SomePlayer < CurrentRulesState.NumberOfPlayers && !TradingSomething; SomePlayer++)
    for (OtherPlayer = 0; OtherPlayer < CurrentRulesState.NumberOfPlayers && !TradingSomething; OtherPlayer++)
      if (CurrentRulesState.Players[SomePlayer].shared.trading.
      cashGivenInTrade[OtherPlayer] > 0)
        TradingSomething = TRUE;

  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS && !TradingSomething; i++)
    if (CurrentRulesState.CountHits[i].toPlayer != RULE_NOBODY_PLAYER &&
    CurrentRulesState.CountHits[i].tradedItem)
      TradingSomething = TRUE;

  /* Don't allow people to vote on empty trades. */

  OpCode = NewActionPntr->numberA;
  if (OpCode == 0 && !TradingSomething)
    OpCode = 2; /* Cancel the deal. */

  switch (OpCode)
  {
  case 0: /* Put it up for a vote. */
    for (i = 0; i < RULE_MAX_PLAYERS; i++)
      CurrentRulesState.Players[i].shared.trading.tradeAccepted = FALSE;

    SwitchPhase (GF_TRADE_ACCEPTANCE,
      CurrentRulesState.phaseStack[0].fromPlayer, /* Proposer */
      0, /* Not used. */
      NewActionPntr->numberB != 0 /* TRUE for private offer. */);

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);

    /* Proposing player automatically accepts the trade.  Since it may
       be an illegal trade, process a fake accept message for him rather
       than blindly accepting. */

    memset (&FakeActionMessage, 0, sizeof (FakeActionMessage));
    FakeActionMessage.fromPlayer = PlayerNo;
    FakeActionMessage.toPlayer = RULE_BANK_PLAYER;
    FakeActionMessage.numberA = TRUE; /* Accept the trade, not reject. */
    FakeActionMessage.numberB = 1;
    ActionTradeAccept (&FakeActionMessage);
    break;


  case 1: /* Pass editorship on to another player.  Anyone non-bankrupt is ok. */
    PlayerNo = (RULE_PlayerNumber) NewActionPntr->numberB;
    if (PlayerNo < CurrentRulesState.NumberOfPlayers &&
    CurrentRulesState.Players[PlayerNo].currentSquare < SQ_OFF_BOARD)
    {
      CurrentRulesState.phaseStack[0].toPlayer = PlayerNo;

      MESS_SendAction (NOTIFY_TRADE_EDITOR, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        PlayerNo, 0, 0, 0, NULL, 0, NULL);
    }
    else /* Some sort of bug. */
      ErrorWrongPlayer (NewActionPntr);
    break;


  case 2: /* Cancel the trade. */
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_TRADE_REJECTED, 0, PlayerNo, 0,
      NULL, 0, NULL);

    SetEmptyTrade ();

    SwitchPhase (GF_TRADE_FINISHED, 0, 0, 0);

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
    break;


  default: /* Huh?  Restart - will resend whole trade. */
    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
    break;
  }
}



/*****************************************************************************
 * The player wants to save the game.  Eventually send back the full game
 * state in a NOTIFY_GAME_STATE_FOR_SAVE message.  Fails if the game isn't
 * in a consistant state (during a trade or debt collection when auxiliary
 * undo information is stored external to the state record).
 */

static void ActionGetGameState (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerSet  FromSet;
  int             i;

  /* Someone else is already saving a game. */

  if (CurrentPhaseM == GF_COLLECT_AI_PARAMETERS_FOR_SAVE)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

#if RULE_LOADSAVE_GAME_ONLY_WHEN_SAFE
  /* Avoid saving the game when the restart may make the
     Star Wars Monopoly animation system get confused. */

  if (CurrentPhaseM != GF_WAIT_MOVE_ROLL)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }
#endif

  /* Can't save the game in phases where restart doesn't work. */

  if (CurrentPhaseM == GF_WAIT_START_TURN ||
  CurrentPhaseM == GF_MOVING_TOKEN ||
  //CurrentPhaseM == GF_WAIT_END_TURN ||
  CurrentPhaseM == GF_WAIT_FOR_EVERYBODY_READY)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  /* Only real players can save games.  Need a valid player ID to send the
     saved game info back to.  Sending it to spectators would broadcast it. */

  if (NewActionPntr->fromPlayer >= RULE_MAX_PLAYERS)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  /* Can't save game during auctions or trading.  Temporary shared data
     fields don't get saved.  It also makes game loading confusing.*/

  if (CurrentRulesState.TradeInProgress || CurrentPhaseM == GF_AUCTION)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  /* Only allow saves in states where we know nothing is on the external
     undo state stack (because it doesn't get saved).  So, most games
     with debt collections in progress can't be saved. */

  for (i = 0; i < CurrentRulesState.NumberOfPendingPhases; i++)
    if (StackedRulesStates [i] . ThisStateIsValid)
    {
      MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        NewActionPntr->action,
        FALSE, /* Failed something.  Let the AIs know. */
        NewActionPntr->fromPlayer,
        0,
        NULL, 0, NULL);

      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_ERROR_SAVE_GAME_LATER, 0, NewActionPntr->fromPlayer, 0, NULL, 0, NULL);

      return;
    }

  /* Ok, start collecting the saved game state information from the AI players. */

  FromSet = 0;
  for (i = 0; i < CurrentRulesState.NumberOfPlayers ; i++)
  {
    if (CollectedAISaveStates[i].AIStateDataPntr != NULL)
    {
#if _DEBUG
      DBUG_DisplayNonFatalErrorMessage ("ActionGetGameState: Someone forgot to deallocate the previous saved game info.");
#endif
      GlobalFreePtr (CollectedAISaveStates[i].AIStateDataPntr);
    }
    CollectedAISaveStates[i].AIStateDataPntr = NULL;

    if (CurrentRulesState.Players[i].AIPlayerLevel != 0)
      FromSet |= (1 << i);
  }

  PushPhase (GF_COLLECT_AI_PARAMETERS_FOR_SAVE,
    FromSet /* Set of players which we are waiting for */,
    NewActionPntr->fromPlayer /* Player requesting the save */,
    CurrentRulesState.GameDurationInSeconds /* Time when save started */);

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * An AI is providing its RIFF encoded state for saving in the game.
 */

static void ActionAISaveParameters (RULE_ActionArgumentsPointer NewActionPntr)
{
  DWORD            *DWordPntr;
  BYTE             *NewMemoryPntr;
  RULE_PlayerNumber PlayerNo;
  DWORD             SaveSize;

  PlayerNo = NewActionPntr->fromPlayer;

  if (CurrentPhaseM != GF_COLLECT_AI_PARAMETERS_FOR_SAVE)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  if (PlayerNo >= CurrentRulesState.NumberOfPlayers ||
  CurrentRulesState.Players[PlayerNo].AIPlayerLevel == 0)
  {
    /* Only AI players can provide state. */
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  /* Remove this AI from the bunch we are waiting for. */

  CurrentRulesState.phaseStack[0].fromPlayer &= ~(1 << PlayerNo);

  /* Add its data to our stored data. */

  if (NewActionPntr->binaryDataA == NULL ||
  NewActionPntr->binaryDataSizeA < 8)
    SaveSize = 0; /* No useable data.  Chunk is at least 8 bytes. */
  else
  {
    DWordPntr = (unsigned long *) (NewActionPntr->binaryDataA + 4);
    SaveSize = *DWordPntr + 8;
    if (SaveSize > NewActionPntr->binaryDataSizeA)
      SaveSize = 0; /* Probably contains garbage, not RIFF data. */
  }

  /* Deallocate previous saved data, if any. */

  if (CollectedAISaveStates[PlayerNo].AIStateDataPntr != NULL)
  {
    GlobalFreePtr (CollectedAISaveStates[PlayerNo].AIStateDataPntr);
    CollectedAISaveStates[PlayerNo].AIStateDataPntr = NULL;
  }

  /* Allocate new saved data, if needed. */

  if (SaveSize > 0)
  {
    NewMemoryPntr = (unsigned char *) GlobalAllocPtr (GHND, SaveSize);
    if (NewMemoryPntr != NULL)
    {
      CollectedAISaveStates[PlayerNo].AIStateDataPntr = NewMemoryPntr;
      memcpy (NewMemoryPntr, NewActionPntr->binaryDataA, SaveSize);
    }
  }

  /* If this is the last one, restart the phase so that it realises that it
     has them all and can continue with the save.  Don't do restarts for ones
     in the middle since that would make the AIs send their data several times
     over.  Unlike the configuration accepting phase, the AIs don't need to
     know who is still pending. */

  if (CurrentRulesState.phaseStack[0].fromPlayer == 0)
    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * The player wants to load the game.
 */

static void ActionSetGameState (RULE_ActionArgumentsPointer NewActionPntr)
{
  BOOL                        HumanPlayerFound;
  RULE_PlayerNumber           LastNonbankruptAI;
  RULE_PlayerNumber           PlayerNo;
  RULE_GameStateRecord        TempGameState;
  RULE_NetworkAddressRecord   TempNetworkAddress;

  PlayerNo = NewActionPntr->fromPlayer;

  /* Only allow real players to load a game while there is a game
     in progress.  If there are no players, anyone can load it. */

  if (CurrentRulesState.NumberOfPlayers > 0 &&
  PlayerNo >= CurrentRulesState.NumberOfPlayers)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

#if RULE_LOADSAVE_GAME_ONLY_WHEN_SAFE
  /* Can't load the game when things are going on that might make the
     Star Wars Monopoly animation system get confused.  So only do
     it when adding players (player selection screen) or when
     the top down view is displayed (rolling dice to move). */

  if (CurrentPhaseM != GF_WAIT_MOVE_ROLL &&
  CurrentPhaseM != GF_ADDING_NEW_PLAYERS)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }
#endif

  /* Test load the game state, just to see if it is legal. */

  if (!RULE_ConvertFileToGameState (NewActionPntr->binaryDataA,
  NewActionPntr->binaryDataSizeA, &TempGameState, NULL))
  {
    /* Failed to load, tell the player doing the loading. */

    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action,
      FALSE,
      NewActionPntr->fromPlayer,
      0,
      NULL, 0, NULL);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, NewActionPntr->fromPlayer,
      TMN_ERROR_LOAD_GAME_FAILURE, 0, NewActionPntr->fromPlayer, 0,
      NULL, 0, NULL);

    return;
  }

  /* Ok, do it for real now.  If it fails now, just start a new game. */

  memset (&CurrentRulesState, 0, sizeof (CurrentRulesState));
  FreeAISaveGameInfo ();

  if (!RULE_ConvertFileToGameState (NewActionPntr->binaryDataA,
  NewActionPntr->binaryDataSizeA, &CurrentRulesState, CollectedAISaveStates))
  {
    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action,
      FALSE,
      NewActionPntr->fromPlayer,
      0,
      NULL, 0, NULL);

    ActionNewGame (NULL);
    return;
  }

  /* Let people know that a game is being loaded. */

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE,
    NewActionPntr->fromPlayer,
    0,
    NULL, 0, NULL);

  /* Update the user interface code with the new game state. */

  MESS_SendAction (NOTIFY_NUMBER_OF_PLAYERS, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
  0 /* No players so that other systems reset */, NewActionPntr->numberB, 0, 0, NULL, 0, NULL);

  /* Wipe out the network addresses, all the players are local now. */

  memset (&TempNetworkAddress, 0, sizeof (TempNetworkAddress));
  for (PlayerNo = 0; PlayerNo < RULE_MAX_PLAYERS; PlayerNo++)
    MESS_AssociatePlayerWithAddress (PlayerNo, &TempNetworkAddress);

  /* Add local names for the players. */

  HumanPlayerFound = FALSE;
  LastNonbankruptAI = 0;
  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    if (CurrentRulesState.Players[PlayerNo].currentSquare < SQ_OFF_BOARD)
    {
      if (CurrentRulesState.Players[PlayerNo].AIPlayerLevel == 0)
        HumanPlayerFound = TRUE;
      else
        LastNonbankruptAI = PlayerNo;
    }

    MESS_SendAction (NOTIFY_ADD_LOCAL_PLAYER, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      PlayerNo, 0, 0, 0,
      CurrentRulesState.Players[PlayerNo].name, 0, NULL);
  }

  if (!HumanPlayerFound) /* Force one human player. */
    CurrentRulesState.Players[LastNonbankruptAI].AIPlayerLevel = 0;

  /* Send the rest of the game status. */

  SendResyncMessages (RULE_ALL_PLAYERS, RESYNC_LOAD_GAME);

  FreeAISaveGameInfo ();

  /* Reset the table of rents to match the loaded short game status. */

  InitialisePredefinedData (&CurrentRulesState.GameOptions);
}



/*****************************************************************************
 * The player wants to save the options.  Send them back as a RIFF file in
 * the blob of a NOTIFY_OPTIONS_FOR_SAVE to the requesting player.
 */

static void ActionGetOptionsForSave (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_ActionArgumentsRecord  MyMessage;

  if (NewActionPntr->fromPlayer >= RULE_MAX_PLAYERS)
  {
    /* Only real players can save options.  Need their address
       to send it back with, would be annoying with spectators
       (all computers would save the options). */

    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  /* Make our own message so we can fiddle with blob stuff. */

  memset (&MyMessage, 0, sizeof (MyMessage));
  MyMessage.action = NOTIFY_OPTIONS_FOR_SAVE;
  MyMessage.fromPlayer = RULE_BANK_PLAYER;
  MyMessage.toPlayer = NewActionPntr->fromPlayer;

  if (AddGameOptionsToMessageBlob (&CurrentRulesState.GameOptions, &MyMessage))
    MESS_SendActionMessage (&MyMessage);
  else /* Out of memory etc. */
    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, NewActionPntr->fromPlayer,
      TMN_ERROR_SAVE_OPTIONS_FAILURE, 0, NewActionPntr->fromPlayer, 0,
      NULL, 0, NULL);
}



/*****************************************************************************
 * The user wants to send some chat data to another user or to everybody.
 * Just copy it and pass it on.  The blob doesn't have to exist.
 */

static void ActionEchoChat (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_CharHandle             Blob;
  BYTE                       *BlobPntr;
  RULE_ActionArgumentsRecord  MessageToSend;

  if (NewActionPntr->binaryDataA != NULL && NewActionPntr->binaryDataSizeA > 0)
  {
    /* Make a copy of the data being chatted, so we can send it. */

    Blob = RULE_AllocateHandle (NewActionPntr->binaryDataSizeA);
    if (Blob == NULL)
      return;

    BlobPntr = (unsigned char *) RULE_LockHandle (Blob);
    memcpy (BlobPntr, NewActionPntr->binaryDataA, NewActionPntr->binaryDataSizeA);
    RULE_UnlockHandle (Blob);
  }
  else
    Blob = NULL;


  memset (&MessageToSend, 0, sizeof (MessageToSend));
  MessageToSend.action = (NewActionPntr->action == ACTION_VOICE_CHAT) ?
    NOTIFY_VOICE_CHAT : NOTIFY_TEXT_CHAT;
  MessageToSend.fromPlayer = RULE_BANK_PLAYER;
  MessageToSend.toPlayer = (RULE_PlayerNumber) NewActionPntr->numberA;
  MessageToSend.numberA = NewActionPntr->numberA;
  MessageToSend.numberB = NewActionPntr->fromPlayer;
  MessageToSend.numberC = NewActionPntr->numberC; /* Fluff message code. */

  // Unique network address of sending computer, used for distinguishing
  // between chats coming from different computers.  Note that only one
  // chat at a time comes from a computer, no matter how many players
  // there are (presumably only one microphone).

  if (NewActionPntr->fromNetworkAddress.networkSystem == NS_LOCAL)
    MessageToSend.numberD = 0;
  else if (NewActionPntr->fromNetworkAddress.networkSystem == NS_DIRECTPLAY)
    MessageToSend.numberD = * (LPDWORD) (NewActionPntr->fromNetworkAddress.address.directPlay + 0);
  else if (NewActionPntr->fromNetworkAddress.networkSystem == NS_WINSOCK)
    MessageToSend.numberD = * (LPDWORD) (NewActionPntr->fromNetworkAddress.address.winSock + 8);
  else
    MessageToSend.numberD = -1;

  if (Blob != NULL)
  {
    MessageToSend.binaryDataHandleA = Blob;
    MessageToSend.binaryDataSizeA = NewActionPntr->binaryDataSizeA;
  }

  MESS_SendActionMessage (&MessageToSend);
}



/*****************************************************************************
 * The Trade Info Update message is used to maintain the trade displays over the network.
 * The message is just rebroadcast to all players, unchanged.
 */

static void ActionUpdateTradeInfo (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_CharHandle             Blob;
  BYTE                       *BlobPntr;
  RULE_ActionArgumentsRecord  MessageToSend;

  MessageToSend = *NewActionPntr; /* Copy whole message record. */
  MessageToSend.action = NOTIFY_UPDATE_TRADE_INFO;
  MessageToSend.fromPlayer = RULE_BANK_PLAYER;
  MessageToSend.toPlayer = RULE_ALL_PLAYERS;
  memset (&MessageToSend.fromNetworkAddress, 0, sizeof (MessageToSend.fromNetworkAddress));

  /* Copy the optional blob of binary information. */

  if (NewActionPntr->binaryDataA != NULL && NewActionPntr->binaryDataSizeA > 0)
  {
    Blob = RULE_AllocateHandle (NewActionPntr->binaryDataSizeA);
    if (Blob == NULL)
      return; /* Out of memory, so ignore the message. */

    BlobPntr = (unsigned char *) RULE_LockHandle (Blob);
    memcpy (BlobPntr, NewActionPntr->binaryDataA, NewActionPntr->binaryDataSizeA);
    RULE_UnlockHandle (Blob);
  }
  else
    Blob = NULL;

  MessageToSend.binaryDataHandleA = Blob;
  MessageToSend.binaryDataStartOffsetA = 0;
  MessageToSend.binaryDataSizeA = (Blob == NULL) ? 0 : NewActionPntr->binaryDataSizeA;
  MessageToSend.binaryDataA = NULL;

  MESS_SendActionMessage (&MessageToSend);
}



/*****************************************************************************
 * The Monopoly Star Wars user interface wants to send some animation data to
 * all the other players to show trade items being dragged around the board.
 * Just copy the whole message and pass it on to all players.
 */

static void ActionStarWarsAnimationInfo (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_CharHandle             Blob;
  BYTE                       *BlobPntr;
  RULE_ActionArgumentsRecord  MessageToSend;

  MessageToSend = *NewActionPntr; /* Copy whole message record. */
  MessageToSend.action = NOTIFY_STAR_WARS_ANIMATION_INFO;
  MessageToSend.fromPlayer = RULE_BANK_PLAYER;
  MessageToSend.toPlayer = RULE_ALL_PLAYERS;
  memset (&MessageToSend.fromNetworkAddress, 0, sizeof (MessageToSend.fromNetworkAddress));

  /* Copy the optional blob of binary information. */

  if (NewActionPntr->binaryDataA != NULL && NewActionPntr->binaryDataSizeA > 0)
  {
    Blob = RULE_AllocateHandle (NewActionPntr->binaryDataSizeA);
    if (Blob == NULL)
      return; /* Out of memory, so ignore the message. */

    BlobPntr = (unsigned char *) RULE_LockHandle (Blob);
    memcpy (BlobPntr, NewActionPntr->binaryDataA, NewActionPntr->binaryDataSizeA);
    RULE_UnlockHandle (Blob);
  }
  else
    Blob = NULL;

  MessageToSend.binaryDataHandleA = Blob;
  MessageToSend.binaryDataStartOffsetA = 0;
  MessageToSend.binaryDataSizeA = (Blob == NULL) ? 0 : NewActionPntr->binaryDataSizeA;
  MessageToSend.binaryDataA = NULL;

  MESS_SendActionMessage (&MessageToSend);
}



/*****************************************************************************
 * The user wants to buy or sell buildings, or do some mortgaging or
 * unmortgaging.  If nobody else is busy doing stuff, let the player have
 * exclusive access to the bank.
 */

static void ActionPlayerBuySellMort (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerNumber  PlayerNo;

  PlayerNo = NewActionPntr->fromPlayer;

  /* If someone else is requesting buy / sell / etc when already doing it, his
     user interface is probably out of date.  Refresh him with a restart.  If
     the same player is requesting it, update the property set he is working
     on, and restart too. */

  if (CurrentPhaseM == GF_BUYSELLMORT)
  {
    if (PlayerNo == CurrentRulesState.phaseStack[0].fromPlayer)
    {
      CurrentRulesState.phaseStack[0].amount = NewActionPntr->numberB;
      MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        NewActionPntr->action, TRUE, NewActionPntr->fromPlayer, 0, NULL, 0, NULL);
    }
    else
      ErrorWrongPhase (NewActionPntr);

    MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0, NULL, 0, NULL);
    return;
  }

  /* Only allow valid non-bankrupt players to start this exclusive action. */

  if (PlayerNo >= CurrentRulesState.NumberOfPlayers ||
  CurrentRulesState.Players[PlayerNo].currentSquare >= SQ_OFF_BOARD)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  /* Start in a phase which can be restarted later and isn't particularly busy. */

  if (CurrentPhaseM != GF_WAIT_MOVE_ROLL &&
  CurrentPhaseM != GF_WAIT_END_TURN && // new for TURN DONE
  CurrentPhaseM != GF_WAIT_JAIL_ROLL &&
  CurrentPhaseM != GF_WAIT_UTILITY_ROLL &&
  CurrentPhaseM != GF_WAIT_UNTIL_CARD_SEEN &&
  CurrentPhaseM != GF_JAIL_ROLL_OR_PAY_OR_CARD_DECISION &&
  CurrentPhaseM != GF_FLAT_OR_FRACTION_TAX_DECISION &&
  CurrentPhaseM != GF_AUCTION_OR_BUY_DECISION &&
  CurrentPhaseM != GF_COLLECTING_PAYMENT &&
  CurrentPhaseM != GF_FREE_UNMORTGAGE)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action, TRUE, NewActionPntr->fromPlayer, 0, NULL, 0, NULL);

  PushPhase (GF_BUYSELLMORT, PlayerNo, RULE_NOBODY_PLAYER, NewActionPntr->numberB);

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * The user has finished buying selling and mortgaging.
 */

static void ActionPlayerDoneBuySellMort (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerNumber  PlayerNo;

  PlayerNo = NewActionPntr->fromPlayer;

  if (CurrentPhaseM != GF_BUYSELLMORT)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  if (PlayerNo != CurrentRulesState.phaseStack[0].fromPlayer)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE,
    NewActionPntr->fromPlayer,
    0,
    NULL, 0, NULL);

  MESS_SendAction (NOTIFY_PLAYER_BUYSELLMORT, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    RULE_NOBODY_PLAYER,
    0, /* Property set he is working on. */
    0, /* Cash owing. */
    RULE_NOBODY_PLAYER, /* Player it is owed to. */
    NULL, 0, NULL);

  PopAndRestartPhase ();
}



/*****************************************************************************
 * Some other player wants to buy the building during a shortage.
 */

static void ActionStartHousingAuction (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerNumber   PlayerNo;

  PlayerNo = NewActionPntr->fromPlayer;

  /* Can only start an auction when the bank is asking. */

  if (CurrentPhaseM != GF_HOUSING_SHORTAGE_QUESTION)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  /* Don't allow the original building buyer to start the auction. */

  if (PlayerNo == CurrentRulesState.phaseStack[0].fromPlayer)
  {
    MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      NewActionPntr->action, FALSE, NewActionPntr->fromPlayer, 0,
      NULL, 0, NULL);

    MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
      TMN_ERROR_BUILDING_ALREADY_BUYING,
      0,
      PlayerNo,
      CurrentRulesState.phaseStack[0].toPlayer, /* Square being bought. */
      NULL, 0, NULL);

    return;
  }

  /* Make sure the player is one who can buy the same kind of building.
     Normally the UI stuff won't send us this message, but just in case... */

  if (((1 << PlayerNo) & PlayersWhoCanBuyABuilding (CurrentRulesState.
  phaseStack[0].amount > 0 /* TRUE for hotel */)) == 0)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action, TRUE, NewActionPntr->fromPlayer, 0,
    NULL, 0, NULL);

  /* Start the auction. */

  SwitchPhase (GF_AUCTION, 0, 0, (CurrentRulesState.phaseStack[0].amount <= 0) ?
    SQ_AUCTION_HOUSE : SQ_AUCTION_HOTEL);

  InitialiseAuctionDataAndRestart (PlayerNo);
}



/*****************************************************************************
 * It's the end of the current player's turn.  Do any final bookkeeping and
 * pass the dice onto the next player.
 */

static void ActionEndTurn (RULE_ActionArgumentsPointer NewActionPntr)
{
  int                     NumberOfBankruptPlayers;
  RULE_PlayerNumber       PlayerNo;
  RULE_PlayerInfoPointer  PlayerPntr;

  //if (NewActionPntr->fromPlayer != RULE_BANK_PLAYER) // Modify for new DONE rule
  if (NewActionPntr->fromPlayer != CurrentRulesState.CurrentPlayer )
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  if (CurrentPhaseM != GF_WAIT_END_TURN)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  PlayerPntr = CurrentRulesState.Players + CurrentRulesState.CurrentPlayer;

  if (PlayerPntr->currentSquare == SQ_IN_JAIL)
    PlayerPntr->turnsInJail++;

  /* Find the next player - has to be non-bankrupt and in the game. */

  PlayerNo = CurrentRulesState.CurrentPlayer;
  while (TRUE)
  {
    PlayerNo++;
    if (PlayerNo >= CurrentRulesState.NumberOfPlayers)
      PlayerNo = 0; /* Wrapped around to the first player. */

    if (PlayerNo == CurrentRulesState.CurrentPlayer)
      break; /* Ran out of players, no other unbankrupt ones. */

    if (CurrentRulesState.Players[PlayerNo].currentSquare < SQ_OFF_BOARD)
      break;
  }
  CurrentRulesState.CurrentPlayer = PlayerNo;

  /* See if the game is over because of time or two bankruptcy rule.  Also
     check for there only being one player left, but that is kind of redundant
     since it would be picked up at the actual moment of bankruptcy (see the
     comments in ActionGoBankrupt for reasons). */

  NumberOfBankruptPlayers = 0;
  for (PlayerNo = 0; PlayerNo < CurrentRulesState.NumberOfPlayers; PlayerNo++)
  {
    if (CurrentRulesState.Players[PlayerNo].currentSquare >= SQ_OFF_BOARD)
      NumberOfBankruptPlayers++;
  }

  if ((CurrentRulesState.GameOptions.stopAtNthBankruptcy &&
  NumberOfBankruptPlayers >= CurrentRulesState.GameOptions.stopAtNthBankruptcy) ||
  (CurrentRulesState.GameOptions.gameOverTimeLimit != 0 &&
  CurrentRulesState.GameDurationInSeconds >= CurrentRulesState.GameOptions.gameOverTimeLimit) ||
  NumberOfBankruptPlayers >= CurrentRulesState.NumberOfPlayers - 1)
  {
      GameOverDeclareWinner (); /* Game over! */
      return;
  }

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE, /* If we got here, the action was accepted. */
    NewActionPntr->fromPlayer,
    NewActionPntr->numberA,
    NULL, 0, NULL);

  SwitchPhase (GF_WAIT_START_TURN, 0, 0, 0);

  /* If the queue is empty, then we can continue on right away.
  Otherwise wait for it to empty out (ACTION_TICK will
  send an ACTION_START_TURN when it is ready). */

  if (MESS_CurrentQueueSize () == 0)
    MESS_SendAction (ACTION_START_TURN, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
      0, 0, 0, 0,
      NULL, 0, NULL);

}



/*****************************************************************************
 * A player has been lost (or dropped by himself (suicide) or dropped under
 * the command of a player running on the server).  Replace him with a local
 * AI player.
 */

static void ActionDisconnectedPlayer (RULE_ActionArgumentsPointer NewActionPntr)
{
  wchar_t                  *NamePntr;
  RULE_PlayerNumber         OtherNo;
  RULE_PlayerNumber         PlayerNo;
  RULE_NetworkAddressRecord TempNetworkAddress;
  BOOL                      UniqueNameFound;

  PlayerNo = (RULE_PlayerNumber) NewActionPntr->numberA; /* The disconnected one. */

  if (PlayerNo >= CurrentRulesState.NumberOfPlayers)
  {
    ErrorWrongPlayer (NewActionPntr); /* Can't disconnect a non-player. */
    return;
  }

  /* Verify that the requester is authorised to do the disconnect.  Note that
     it comes from the bank if the network software detects a failure. */

  if (NewActionPntr->fromPlayer != PlayerNo /* Suicide */ &&
  NewActionPntr->fromNetworkAddress.networkSystem != NS_LOCAL)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action, TRUE, NewActionPntr->fromPlayer, 0,
    NULL, 0, NULL);

  MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    TMN_ERROR_PLAYER_DISCONNECTED, 0, PlayerNo, 0,
    NULL, 0, NULL);

  /* Set up the replacement player.  First give it a local address. */

  memset (&TempNetworkAddress, 0, sizeof (TempNetworkAddress));
  MESS_AssociatePlayerWithAddress (PlayerNo, &TempNetworkAddress);

  /* It will be a tough AI unless otherwise specified. */

  CurrentRulesState.Players[PlayerNo].AIPlayerLevel = (unsigned char) NewActionPntr->numberB;

  if (CurrentRulesState.Players[PlayerNo].AIPlayerLevel < 1 ||
  CurrentRulesState.Players[PlayerNo].AIPlayerLevel > AI_MAX_LEVELS)
    CurrentRulesState.Players[PlayerNo].AIPlayerLevel = AI_MAX_LEVELS;

  /* Give it a new unique name.  Helps players see that the AI has taken over.
     Also the new name in the old slot will deallocate the previous player. */

  NamePntr = CurrentRulesState.Players[PlayerNo].name;


  /* Make sure that the replacement name is unique. */

  while (TRUE)
  {
    UniqueNameFound = TRUE;
    for (OtherNo = 0; OtherNo < CurrentRulesState.NumberOfPlayers; OtherNo++)
    {
      if (OtherNo == PlayerNo)
        continue;

      if (_wcsicmp (CurrentRulesState.Players[OtherNo].name, NamePntr) == 0)
      {
        UniqueNameFound = FALSE;
        break;
      }
    }

    if (UniqueNameFound)
      break;

    /* Not unique, add a question mark to the end. */

    if (wcslen (NamePntr) >= RULE_MAX_PLAYER_NAME_LENGTH)
      wcscpy (NamePntr, L"?");
    else
      wcscat (NamePntr, L"?");
  }

  /* Make the local user interface code register a new local player name.
     This is also a hint to other systems to remove the player previously
     in that slot. */

  MESS_SendAction (NOTIFY_ADD_LOCAL_PLAYER, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    PlayerNo, 0, 0, 0,
    CurrentRulesState.Players[PlayerNo].name, 0, NULL);

  /* This will make the local user interface realise that it is running
     a particular slot. */

  MESS_SendAction (NOTIFY_NAME_PLAYER, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    PlayerNo,
    CurrentRulesState.Players[PlayerNo].token,
    CurrentRulesState.Players[PlayerNo].colour,
    CurrentRulesState.Players[PlayerNo].AIPlayerLevel,
    CurrentRulesState.Players[PlayerNo].name, 0, NULL);

  /* And in case it is waiting for a dice roll or something, this will let the
     newly started AI know what's up. */

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
  0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * Someone wants to pause or unpause the game.  Just does a restart if the
 * game is already unpaused or paused.
 */

static void ActionPauseGame (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerNumber  PlayerNo;

  PlayerNo = NewActionPntr->fromPlayer;

  if (PlayerNo >= CurrentRulesState.NumberOfPlayers)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  MESS_SendAction (NOTIFY_ACTION_COMPLETED, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
    NewActionPntr->action,
    TRUE,
    NewActionPntr->fromPlayer,
    0,
    NULL, 0, NULL);

  if (NewActionPntr->numberA)
  {
    /* The user wants to pause the game. */

    if (CurrentPhaseM != GF_PAUSED)
    {
      /* Don't allow pauses in phases that can't restart.  There may also
         be pending actions in the queue that require these phases. */

      if (CurrentPhaseM == GF_MOVING_TOKEN ||
      CurrentPhaseM == GF_WAIT_END_TURN)
      {
        ErrorWrongPhase (NewActionPntr);
        return;
      }

      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_GAME_PAUSED, PlayerNo, 0, 0,
        NULL, 0, NULL);

      PushPhase (GF_PAUSED, 0, 0, 0);
    }
  }
  else /* Unpausing the game. */
  {
    if (CurrentPhaseM == GF_PAUSED)
    {
      MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
        TMN_GAME_UNPAUSED, PlayerNo, 0, 0,
        NULL, 0, NULL);

      PopPhase ();
    }
  }

  MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    0, 0, 0, 0, NULL, 0, NULL);
}



/*****************************************************************************
 * A player is announcing that they are present.  Remove him from the set
 * of players we are waiting for.
 */

static void ActionIAmHere (RULE_ActionArgumentsPointer NewActionPntr)
{
  RULE_PlayerNumber  PlayerNo;

  PlayerNo = NewActionPntr->fromPlayer;

  if (PlayerNo >= CurrentRulesState.NumberOfPlayers)
  {
    ErrorWrongPlayer (NewActionPntr);
    return;
  }

  if (CurrentPhaseM != GF_WAIT_FOR_EVERYBODY_READY)
  {
    ErrorWrongPhase (NewActionPntr);
    return;
  }

  /* Remove this player from the set we are waiting for, but only if he has
     the right serial number and is in the set (ignore duplicates and bad ones
     so that we don't get a flurry of retransmissions). */

  if (CurrentRulesState.phaseStack[0].amount == NewActionPntr->numberA &&
  (CurrentRulesState.phaseStack[0].fromPlayer & (1 << PlayerNo)))
  {
    CurrentRulesState.phaseStack[0].fromPlayer &= ~(1 << PlayerNo);

    if( CurrentRulesState.phaseStack[0].fromPlayer == 0 )
      MESS_SendAction (ACTION_RESTART_PHASE, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
        0, 0, 0, 0, NULL, 0, NULL);
  }
}



/*****************************************************************************
 * This function is called periodically by the main program loop.  Process
 * a received message and generate new ones (run the game in other words).
 */

void RULE_ProcessRules (RULE_ActionArgumentsPointer NewActionPntr)
{
#if RULE_REPORT_BAD_MESSAGES
  int                       i;
#endif
  RULE_NetworkAddressRecord NetworkAddress;
  RULE_PlayerNumber         PlayerNo;

  if (NewActionPntr == NULL)
    return; /* Nothing to do. */

  /* Verify the address of players and the bank in network games. */

  PlayerNo = NewActionPntr->fromPlayer;

  if (MESS_NetworkMode)
  {
    if (PlayerNo < CurrentRulesState.NumberOfPlayers || PlayerNo == RULE_BANK_PLAYER)
    {
      MESS_GetAddressOfPlayer (PlayerNo, &NetworkAddress);
      if (memcmp (&NetworkAddress, &NewActionPntr->fromNetworkAddress,
      sizeof (NetworkAddress)) != 0)
      {
        /* We have a phoney network address.  Ignore the message. */

#if _DEBUG
        char TempString [100];
        sprintf (TempString, "RULE_ProcessRules: Message received from player "
          "%d comes from wrong network address.", (int) PlayerNo);
        DBUG_DisplayNonFatalErrorMessage (TempString);
#endif /* _DEBUG */

#if RULE_REPORT_BAD_MESSAGES
      sprintf (LE_ERROR_DebugMessageBuffer,
        "\r\nRULE Bad Address Message: action:%d A:%d B:%d C:%d D:%d E:%d  Addr:",
        NewActionPntr->action, NewActionPntr->numberA, NewActionPntr->numberB,
        NewActionPntr->numberC, NewActionPntr->numberD, NewActionPntr->numberE);
      for (i = 0; i < sizeof (NetworkAddress); i++)
        sprintf (LE_ERROR_DebugMessageBuffer + strlen (LE_ERROR_DebugMessageBuffer),
        " %02X", (int) ((unsigned char *) &NewActionPntr->fromNetworkAddress) [i]);
      strcat (LE_ERROR_DebugMessageBuffer, "\r\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif /* RULE_REPORT_BAD_MESSAGES */

        return;
      }
    }
  }

  /* Record the time of last activity for players, so we can see
     when they are idle for too long. */

  if (PlayerNo < RULE_MAX_PLAYERS)
  {
    CurrentRulesState.Players[PlayerNo].timeOfLastActivity =
      CurrentRulesState.GameDurationInSeconds;
    CurrentRulesState.Players[PlayerNo].inactivityCount = 0;
  }

  /* Do the action processing (action specific tests go here).  See the
     Rule.h file for descriptions of all the actions.  In alphabetical order
     in this switch statement. */

  switch (NewActionPntr->action)
  {
  case ACTION_ACCEPT_CONFIGURATION:
    ActionAcceptConfiguration (NewActionPntr);
    break;

  case ACTION_AI_SAVE_PARAMETERS:
    ActionAISaveParameters (NewActionPntr);
    break;

  case ACTION_BID:
    ActionBid (NewActionPntr);
    break;

  case ACTION_BUY_HOUSE:
    ActionBuyHouse (NewActionPntr);
    break;

  case ACTION_BUY_OR_AUCTION_DECISION:
    ActionBuyOrAuctionDecision (NewActionPntr);
    break;

  case ACTION_CANCEL_DECOMPOSITION:
    ActionCancelDecomposition (NewActionPntr);
    break;

  case ACTION_CARD_SEEN:
    ActionCardSeen (NewActionPntr);
    break;

  case ACTION_CHEAT_CASH:
    ActionCheatCash (NewActionPntr);
    break;

  case ACTION_CHEAT_OWNER:
    ActionCheatOwner (NewActionPntr);
    break;

  case ACTION_CHEAT_ROLL_DICE:
    ActionRollDice (NewActionPntr);
    break;

  case ACTION_CLEAR_TRADE_ITEMS:
    ActionClearTradeItems (NewActionPntr);
    break;

  case ACTION_CLEAR_TRADED_IMMUNITIES_OR_FUTURES:
    ActionClearTradedImmunitiesOrFutures (NewActionPntr);
    break;

  case ACTION_DISCONNECTED_PLAYER:
    ActionDisconnectedPlayer (NewActionPntr);
    break;

  case ACTION_END_TURN:
    ActionEndTurn (NewActionPntr);
    break;

  case ACTION_EXIT_JAIL_DECISION:
    ActionExitJailDecision (NewActionPntr);
    break;

  case ACTION_FREE_UNMORTGAGE_DONE:
    ActionFreeUnmortgageDone (NewActionPntr);
    break;

  case ACTION_GET_GAME_STATE_FOR_SAVE:
    ActionGetGameState (NewActionPntr);
    break;

  case ACTION_GET_OPTIONS_FOR_SAVE:
    ActionGetOptionsForSave (NewActionPntr);
    break;

  case ACTION_GO_BANKRUPT:
    ActionGoBankrupt (NewActionPntr);
    break;

  case ACTION_I_AM_HERE:
    ActionIAmHere (NewActionPntr);
    break;

  case ACTION_JUMP_TO_SQUARE:
    ActionJumpToSquare (NewActionPntr);
    break;

  case ACTION_KILL_AUCTION_CHEAT:
    ActionKillAuctionCheat (NewActionPntr);
    break;

  case ACTION_LANDED_ON_SQUARE:
    ActionLandedOnSquare (NewActionPntr);
    break;

  case ACTION_MORTGAGING:
    ActionMortgaging (NewActionPntr);
    break;

  case ACTION_MOVE_BACKWARDS:
    ActionMoveBackwards (NewActionPntr);
    break;

  case ACTION_MOVE_FORWARDS:
    ActionMoveForwards (NewActionPntr);
    break;

  case ACTION_NAME_PLAYER:
    ActionNamePlayer (NewActionPntr);
    break;

  case ACTION_NEW_GAME:
    ActionNewGame (NewActionPntr);
    break;

  case ACTION_PAUSE_GAME:
    ActionPauseGame (NewActionPntr);
    break;

  case ACTION_PLAYER_BUYSELLMORT:
    ActionPlayerBuySellMort (NewActionPntr);
    break;

  case ACTION_PLAYER_DONE_BUYSELLMORT:
    ActionPlayerDoneBuySellMort (NewActionPntr);
    break;

  case ACTION_RANDOM_SEED:
    srand (NewActionPntr->numberA);
    break;

  case ACTION_RESTART_PHASE:
    ActionRestartPhase (NewActionPntr);
    break;

  case ACTION_RESYNC_CLIENT:
    ActionResyncClient (NewActionPntr);
    break;

  case ACTION_ROLL_DICE:
    ActionRollDice (NewActionPntr);
    break;

  case ACTION_SELL_BUILDINGS:
    ActionSellBuildings (NewActionPntr);
    break;

  case ACTION_SET_GAME_STATE:
    ActionSetGameState (NewActionPntr);
    break;

  case ACTION_START_GAME:
    ActionStartGame (NewActionPntr);
    break;

  case ACTION_START_HOUSING_AUCTION:
    ActionStartHousingAuction (NewActionPntr);
    break;

  case ACTION_START_TRADE_EDITING:
    ActionStartTradeEditing (NewActionPntr);
    break;

  case ACTION_START_TURN:
    ActionStartTurn (NewActionPntr);
    break;

  case ACTION_UPDATE_TRADE_INFO:
    ActionUpdateTradeInfo (NewActionPntr);
    break;

  case ACTION_STAR_WARS_ANIMATION_INFO:
    ActionStarWarsAnimationInfo (NewActionPntr);
    break;

  case ACTION_TAX_DECISION:
    ActionTaxDecision (NewActionPntr);
    break;

  case ACTION_TICK:
    ActionTick ();
    break;

  case ACTION_TRADE_ACCEPT:
    ActionTradeAccept (NewActionPntr);
    break;

  case ACTION_TRADE_EDITING_DONE:
    ActionTradeEditingDone (NewActionPntr);
    break;

  case ACTION_TRADE_ITEM:
    ActionTradeItem (NewActionPntr);
    break;

  case ACTION_VOICE_CHAT:
  case ACTION_TEXT_CHAT:
    ActionEchoChat (NewActionPntr);
    break;

  /* Note - items are in alphabetical order above. */

  default: /* Notification messages and other things we ignore. */
    break;
  } /* End switch on action */
}



/******************************************************************************
 * These routines will allow conversion between a RULE_PropertySet bit and a
 * RULE_SquareType ID. For routine RULE_BitSetToProperty(), the lowest bit is used
 * if more than 1 bit is set. Also if the bit in the RULE_PropertySet does not
 * specify a valid property square, SQ_OFF_BOARD is returned.
 */
// Replaced by global macro (RK)
//RULE_PropertySet RULE_PropertyToBitSet(RULE_SquareType Square)
//{
//  return RULE_PropertyToBitsetArray [Square];
//}

RULE_SquareType RULE_BitSetToProperty (RULE_PropertySet Set)
{
  RULE_SquareType i;

  // If the set specified is empty, return off board.
  if (Set == 0)
    return SQ_OFF_BOARD;

  // Finally, go through the array and find the matching bit.
  for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
    if (RULE_PropertyToBitsetArray[i] & Set)
      return i;

  // None found, therefore illegal bit.
#if _DEBUG
  DBUG_DisplayNonFatalErrorMessage ("Illegal square specified in PropertySet.");
#endif
  return SQ_OFF_BOARD;
}



/*****************************************************************************
 * Count up the number of houses and hotels owned by the player.
 */

void RULE_CountHousesAndHotels (RULE_GameStatePointer GamePntr, RULE_PlayerNumber Player,
                           int *HouseCount, int *HotelCount)
{
  int         Count;
  RULE_SquareType  Square;
  int         TotalHotels;
  int         TotalHouses;

  TotalHouses = TotalHotels = 0;

  for (Square = 0; Square < SQ_MAX_SQUARE_TYPES; Square++)
  {
    /* Only count property type squares owned by the player. */

    if (RULE_SquarePredefinedInfo[Square].group < SG_MAX_PROPERTY_GROUPS &&
    GamePntr->Squares[Square].owner == Player)
    {
       Count = GamePntr->Squares[Square].houses;
       if (Count < GamePntr->GameOptions.housesPerHotel)
         TotalHouses += Count;
       else
         TotalHotels++;
    }
  }
  *HouseCount = TotalHouses;
  *HotelCount = TotalHotels;
}



/*****************************************************************************
 * Find the properties that are owned by the player.
 */

RULE_PropertySet RULE_PropertySetOwnedByPlayer (RULE_GameStatePointer GamePntr,
                                      RULE_PlayerNumber Player)
{
  RULE_PropertySet PropertiesOwned;
  RULE_SquareType  SquareNo;
  RULE_SquareInfoPointer SquarePoint;

  PropertiesOwned = 0;
  SquarePoint = GamePntr->Squares;

  for (SquareNo = 0; SquareNo < SQ_MAX_SQUARE_TYPES; SquareNo++, SquarePoint++)
  {
    if (SquarePoint->owner == Player)
      PropertiesOwned |= RULE_PropertyToBitSet (SquareNo);
  }
  return PropertiesOwned;
}



/*****************************************************************************
 * Writes a list of the square names from PropertySet into the Buffer.  In
 * English, they are comma and space separated.  Stops early if your buffer
 * is too small.  BufferLength is number of wchars in the buffer, not bytes.
 */

void RULE_PrintSetOfPropertyNames (RULE_PropertySet PropertySet,
wchar_t *Buffer, DWORD BufferLength)
{
  DWORD             NextWCharIndex;
  RULE_PropertySet  RemainingProperties;
  wchar_t          *SquareName;
  DWORD             SquareNameLength;
  RULE_SquareType   SquareNo;

  if (Buffer == NULL || BufferLength == 0)
    return;

  RemainingProperties = PropertySet;
  NextWCharIndex = 0;
  while (RemainingProperties != 0 && NextWCharIndex < BufferLength - 1)
  {
    SquareNo = RULE_BitSetToProperty (RemainingProperties);
    if (SquareNo >= SQ_OFF_BOARD)
      break;
    RemainingProperties &= ~RULE_PropertyToBitSet (SquareNo);

    SquareName = RULE_SquarePredefinedInfo[SquareNo].squareName;
    SquareNameLength = wcslen (SquareName);
    if (SquareNameLength + NextWCharIndex + 2 /* For comma space */ >= BufferLength - 1 /* For NUL */)
      break; /* This one won't fit. */
    if (NextWCharIndex > 0)
    {
      wcscpy (Buffer + NextWCharIndex, L", ");
      NextWCharIndex += 2;
    }
    wcscpy (Buffer + NextWCharIndex, SquareName);
    NextWCharIndex += SquareNameLength;
  }
  if (RemainingProperties != 0 && NextWCharIndex + 3 < BufferLength - 1)
  {
    wcscpy (Buffer + NextWCharIndex, L"...");
    NextWCharIndex += 3;
  }
  Buffer [NextWCharIndex] = 0; /* End of string marker. */
}



/*****************************************************************************
 * Convert an error message code number and values embedded in a rules message
 * into a Unicode string.  BufferSize is the number of 16 bit characters in
 * Buffer, not the number of bytes.
 */

void RULE_FormatErrorByMessage (RULE_ActionArgumentsPointer NewMessage,
wchar_t *Buffer, long BufferSize)
{
  LANG_TextMessageNumber  ErrorCode;
  wchar_t                *InputStringPntr;
  long                    Length;
  long                    OutputIndex;
  RULE_PlayerNumber       PlayerNo;
  wchar_t                 TempWString [2048];
  long                    TempLength = sizeof (TempWString) / sizeof (wchar_t);

  if (Buffer == NULL || BufferSize <= 0)
  {
#if _DEBUG
    DBUG_DisplayNonFatalErrorMessage ("RULE_FormatErrorMessage: Called with NULL arguments or zero size!");
#endif
    return;
  }

  ErrorCode = NewMessage->numberA;

  /* Get the error message in the current language from the
     text messages system. */

  InputStringPntr = LANG_GetTextMessageClean (ErrorCode);

  /* Substitute numbers and strings for the special codes.  All special
     codes begin with a "^".  Do processing in UniCode so that we don't
     get confused by escape codes that happen to contain "^". */

  OutputIndex = 0;
  while (*InputStringPntr != 0 && OutputIndex < (BufferSize - 1))
  {
    /* Copy all ordinary characters until we hit a control character. */

    if (*InputStringPntr != L'^')
    {
      Buffer [OutputIndex++] = *InputStringPntr++;
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
      wcsncpy (TempWString, NewMessage->stringA, TempLength - 1);
      break;

    case L'P':
    case L'Q':
      // Use player name given by numberC (or numberB for ^Q) index.
      if (towupper (InputStringPntr[1]) == L'P')
        PlayerNo = (RULE_PlayerNumber) NewMessage->numberC;
      else /* Got ^Q. */
        PlayerNo = (RULE_PlayerNumber) NewMessage->numberB;

      if (PlayerNo < RULE_MAX_PLAYERS)
        wcsncpy (TempWString, UICurrentGameState.Players[PlayerNo].name, TempLength - 1);
      else if (PlayerNo == RULE_BANK_PLAYER)
        wcsncpy (TempWString, LANG_GetTextMessageClean (TMN_BANK_NAME), TempLength - 1);
      else if (PlayerNo == RULE_NOBODY_PLAYER)
        wcsncpy (TempWString, LANG_GetTextMessageClean (TMN_NOBODY_NAME), TempLength - 1);
      else
        wcsncpy (TempWString, LANG_GetTextMessageClean (TMN_SPECTATOR_NAME), TempLength - 1);
      break;

    case L'S':
      // Convert the Square number given in numberD to the Square name.
      if (NewMessage->numberD >= 0 && NewMessage->numberD < SQ_MAX_SQUARE_TYPES)
        wcsncpy (TempWString,
        RULE_SquarePredefinedInfo[NewMessage->numberD].squareName,
        TempLength - 1);
      break;

    case L'T':
      // Insert list of square names from NumberE.
      RULE_PrintSetOfPropertyNames (NewMessage->numberE, TempWString, TempLength);
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
    if (OutputIndex + Length > (BufferSize - 1))
    {
      Length = (BufferSize - 1) - OutputIndex;
      if (Length < 0)
        Length = 0;
    }

    wcsncpy (Buffer + OutputIndex, TempWString, Length);
    OutputIndex += Length;
  }
  Buffer [OutputIndex] = 0; /* End of string marker. */
}



/*****************************************************************************
 * Converts an error message and a bunch of values to a Unicode string with
 * the values substituted in the appropriate spots.  BufferSize is the number
 * of 16 bit characters in Buffer, not the number of bytes.
 */

void RULE_FormatErrorByArguments (LANG_TextMessageNumber MessageNumber,
long NumberB, long NumberC, long NumberD, long NumberE,
wchar_t *Buffer, long BufferSize)
{
  RULE_ActionArgumentsRecord FakeRuleMessage;

  memset (&FakeRuleMessage, 0, sizeof (FakeRuleMessage));
  FakeRuleMessage.action = NOTIFY_ERROR_MESSAGE;
  FakeRuleMessage.numberA = MessageNumber;
  FakeRuleMessage.numberB = NumberB;
  FakeRuleMessage.numberC = NumberC;
  FakeRuleMessage.numberD = NumberD;
  FakeRuleMessage.numberE = NumberE;

  RULE_FormatErrorByMessage (&FakeRuleMessage, Buffer, BufferSize);
}


/*****************************************************************************
 * Given a number which represents one of the action enums, returns TRUE
 * if the action is considered a prompt to the player that she could expect
 * to get at the main screen..  Otherwise, returns FALSE.  For example, during 
 * the setup phase if a machine receives such a "please roll dice" message,
 * it should immediately join the game in progress.
 */
BOOL RULE_ActionIsOfMainGameScreenPromptType( RULE_ActionType actionNumber )
{
  switch ( actionNumber )
  {
  case NOTIFY_END_TURN:
  case NOTIFY_JAIL_EXIT_CHOICE:
  case NOTIFY_PLEASE_ROLL_DICE:
  case NOTIFY_PLEASE_PAY:
  case NOTIFY_PICKED_UP_CARD:
  case NOTIFY_BUY_OR_AUCTION_DECISION:
  case NOTIFY_HOUSING_SHORTAGE:
  case NOTIFY_FREE_UNMORTGAGING:
  case NOTIFY_FLAT_OR_FRACTION_TAX_DECISION:
  case NOTIFY_PLAYER_BUYSELLMORT:
  case NOTIFY_DECOMPOSE_SALE:
  case NOTIFY_PLACE_BUILDING:
    return TRUE;
  }

  return FALSE;
}


/*****************************************************************************
 * Set up the rules engine, basically get into the new game state.
 */

BOOL RULE_InitializeSystem(void)
{
  RULE_GameOptionsRecord     FakeGameOptions;

  srand ((int) GetTickCount ());  /* Initialize random number generator. */

  memset (&FakeGameOptions, 0, sizeof (FakeGameOptions));
  FakeGameOptions.housesPerHotel = 5;
  if (!InitialisePredefinedData (&FakeGameOptions))
    return FALSE;

  ActionNewGame (NULL);
  return TRUE;
}



/************************************************************/

/************************************************************/
/* RULE_CleanAndRemoveSystem                                */
/************************************************************/
void RULE_CleanAndRemoveSystem(void)
{
}
/************************************************************/


/************************************************************/
/* RULE_ReturnFirstMoveStatus                               */
/************************************************************/
BOOL RULE_PlayersFirstMoveWasMade(int player)
{
    return (CurrentRulesState.Players[player].firstMoveMade);
};
/************************************************************/
