/*****************************************************************************
 *
 * FILE:        UDIBAR.cpp, subfile of Display.cpp & UserIfce.cpp
 * DESCRIPTION: Handles display & User input specific to the 3 row main
 *    interface control.
 *
 * © Copyright 1999 Artech Digital Entertainments.  All rights reserved.
 *****************************************************************************/

#include "gameinc.h"


/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/
#if !USA_VERSION
  LE_DATA_DataId UDIBAR_Deeds[56];
#endif

TYPE_Tick LastSoundPlayedAt[RULE_MAX_PLAYERS] = { 0, 0, 0, 0, 0, 0 };

// saveaccountinfo is used to write the info to the file acchist.txt
AccountHistoryStruct SaveAccountInfo;
BOOL AI_ButtonRemoteState, last_AI_ButtonRemoteState;
#define IBAR_BUTTON_BASE_SEQ LED_IFT( DAT_LANG2, CNK_iyaaf ) // First button, first mode.
#define IBAR_BUTTON_BASE_AI_ LED_IFT( DAT_LANG2, CNK_iycaf )

int IBARPropertyBarOrder[28] = // Warning - this is external, adjust the subscript in the header file as well
{// The subscript corresponding to a property (med = 0, baltic = 1,... boardw = 27
  //  holds the display position - 0 to 2 first column top to bottom, etc.
  11, 9,  // Med baltic col 4
  2,      // RR
  14, 13, 12, // lt blue
  17,  8, 16, 15, // purple (plus electric co)
  0,  20, 19, 18, // orange
  23, 22, 21,  5, // red
  26, 25, 6,  24, // Yellow
  29, 28, 27,     // green
  3,  32, 30      // blue
};
// Locations for the above are calculated in DISPLAY_UDIBAR_Initialize
RECT IBAR_PropertyLocations[SQ_MAX_SQUARE_TYPES];
//FIXME - the subscript was 28, array overrun in ibar init - why?  if 28 can work, do it.
RECT IBAR_JailCardLocations[2];

RECT IBAR_ButtonGeneralLocations[IBAR_BUTTON_MAX] =
{//OPTIMIZE:  If all of these are on the same row, could skip y checks during mouseover & click.
  {  10, 455,  69, 483 }, // Options
  {  70, 455, 132, 483 }, // Trade
  { 148, 455, 252, 483 }, // General 1
  { 569, 455, 672, 483 }, // General 4
  { 256, 455, 357, 483 }, // General 2
  { 464, 455, 567, 483 }, // General 3
  { 361, 455, 463, 483 }, // Main
  { 690, 456, 727, 483 }, // View 1 - actually just one camera button.
  { 732, 456, 789, 483 } // Status
//  { 384, 456, 409, 480 }, // View 2
 // { 414, 456, 437, 480 }  // View 3 ...
};
// The following are custom button locations since 'we' didn't standardize spacing.
RECT  IBAR_ButtonBuyAuctionLocations[IBAR_BUTTON_MAX];
RECT  IBAR_ButtonTaxChoiceLocations[IBAR_BUTTON_MAX];
RECT  IBAR_ButtonTradeChoiceLocations[IBAR_BUTTON_MAX];
BOOL  IBAR_JustChanged; // Records the change so the graphics know the next pass is only for outgoing buttons (dont start incoming)

//Defines used to store & recall options (buttons) available for a given deed in deed state
#define UDIBAR_DeedStateButtonBuyHouse    1
#define UDIBAR_DeedStateButtonSellHouse   2
#define UDIBAR_DeedStateButtonMortgage    4
#define UDIBAR_DeedStateButtonUnMortgage  8


#if USE_PRESET_DICE_ROLLS
#define IBAR_CHEAT_ROLLS_MAX 6
int IBAR_Cheat_Rolls[IBAR_CHEAT_ROLLS_MAX][2] =
{
#if 1
// Player vs AI
  {2, 1},          // Player: Baltic Avenue
  {1, 1}, {3, 1},  // Player: Reading Railroad then Conneticut Avenue
  {1, 1},          // Player: St. Charles Place then...
  {1, 1}, {1, 2},
#else
// Player vs Player
  {2, 1},         // Player 1: Baltic Avenue
  {5, 1},         // Player 2: Oriental Avenue
  {1, 1}, {3, 1}, // Player 1: Reading railroad then Conneticut Avenue
  {5, 4},         // Player 2: Pennsylvania Railroad
  {1, 1},         // Player 1: St. Charles then...
#endif
};
int IBAR_CHEAT_RollIndex;
#endif


///////////////////////////////////////////////////////////////////////////////////////////
// data objects for the status bar button sequences


int last_sort_result = 0;                   // holds the index into the button that was last pressed
int last_psort_result = 0, last_dsort_result = 0, last_bsort_result = 0;
int last_category_result = 0;
int last_options_result = -1;
int current_options_result = 0;

StatusButtonStruct  SortButton[4], CategoryButton[3], OptionsButton[4];
LE_DATA_DataId          CategoryBarID, CategoryBarID1;
LE_DATA_DataId          SortBarID, SortBarID1;
UNS16                               BarPriority = DISPLAY_IBAR_BasePriority-200;

const UNS16 SButtonIdlePriority[4] = {BarPriority+8, BarPriority+6, BarPriority+4, BarPriority+2};
const   UNS16 SButtonPriority[4] = {BarPriority+9, BarPriority+7, BarPriority+5, BarPriority+3};
const UNS16 CButtonIdlePriority[3] = {BarPriority+2, BarPriority+4, BarPriority+6};
const   UNS16 CButtonPriority[3] = {BarPriority+3, BarPriority+5, BarPriority+7};
const UNS16 OButtonIdlePriority[4] = {BarPriority+6, BarPriority+8, BarPriority+4, BarPriority+1};
const   UNS16 OButtonPriority[4] = {BarPriority+7, BarPriority+9, BarPriority+5, BarPriority+3};
/*
const UNS16 SButtonIdlePriority[4] = {1008, 1006, 1004, 1002};
const   UNS16 SButtonPriority[4] = {1009, 1007, 1005, 1003};
const UNS16 CButtonIdlePriority[3] = {1002, 1004, 1006};
const   UNS16 CButtonPriority[3] = {1003, 1005, 1007};
const UNS16 OButtonIdlePriority[4] = {1006, 1008, 1004, 1001};
const   UNS16 OButtonPriority[4] = {1007, 1009, 1005, 1003};
*/

BOOL        g_bStatusBarDisplayed = FALSE;
BOOL		g_bOptionsButtonsDisplayed = FALSE;
int                     g_Button = 0;
BOOL                    g_bIsScreenUpdated = FALSE;

short   const SortXPosition[4] = {485, 540, 609, 676};
short   const CategoryXPosition[3] = {40, 132, 226};
short   const OptionsXPosition[4] = {31, 180, 478, 615};
short   const OptionsYPosition[4] = {494, 493, 490, 490};
short   const OptionsWidth[4] = {170, 159, 164, 175};
short   const OptionsHeight[4] = {59, 60, 63, 62};


int UDPIECES_PickCameraFor3Squares( int square );
int UDPIECES_PickCameraFor15Squares( int square );
void UDIBar_ProcessStatsButtonPress(int button);
void UDIBar_InitStatusButtons(void);
void UDIBar_SwapStatusButtons(void);
void UDIBar_DisplaySortButtons(void);
void UDIBar_RemoveSortButtons(void);
void UDIBar_InitOptionsButtons(void);
void UDIBar_DisplayOptionsButtons(void);
void UDIBar_RemoveOptionsButtons(void);


////////////////////////////////////////////////////////////////////
// Player Select screen related stuff.
BOOL        g_bPlayerSelectBarDisplayed = FALSE;

// BLEEBLE:  This is a hack.  Trying to fix the problem where
// user selects new game during setup before any players have been
// added to the game.  Since there is no well defined new-game
// message from the rules engine, only a number-of-players-is-zero
// message, the setup code has trouble with this.
BOOL        g_bNewGameRequestedFlagForUDPSELCode = FALSE;


/*****************************************************************************
 * All display initialization, showing, hiding and destroying occurs in
 * the following 4 subroutines called from DISPLAY.CPP.

 * No code other than the show module may start nor stop sound
 * and graphic items.  Fields indicating desired actions can be filled in
 * in the display state which the show routine will pick up.  In this way,
 * the show and routines can take responsibility for tracking all data
 * required to pause or clear the screen at any time.

 * There will be no locking the computer as a wait for all sequences to finish
 * might do.  Stack processing can be halted until sequences are finished,
 * but you will have to use a timer to poll your sequence status.  This is to
 * allow processing of pause buttons or user camera controls at all times, as
 * well as to make programmers lives more difficult :)
 */

void DISPLAY_UDIBAR_Initialize( void )
{ // Initialize any permanent display objects.  Avoid temporary ones when possible.
  register int t;
  int itemp, barOrder, x, y;

  // Cheat list of dicerolls (if used)
#if USE_PRESET_DICE_ROLLS
  IBAR_CHEAT_RollIndex = 0;
#endif


  // TEMP: Indicates no current dialog.
  DISPLAY_state.CurrentDialog = -1;

  // Property pop-up for Buy/Auction
  DISPLAY_state.PropertyBuyAuctionDesired = DISPLAY_state.PropertyBuyAuctionShown = LED_EI;

  // Set up the Action Button bar
  DISPLAY_state.ESC_Menu_Up = FALSE;
//  IBAR_BSSM_LOCK::BSSM_Initialize();
  UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, 0 );
  UDIBAR_setIBarCurrentState( IBAR_STATE_Nothing );
  DISPLAY_state.IBarStateTrackOn = TRUE;
  DISPLAY_state.IBarCurrentPlayer = -1;
  IBAR_JustChanged = FALSE;
  DISPLAY_state.IBarLastActionNotificationFor = -1;
  DISPLAY_state.LE_TickCount_Last_Button_Click = LE_TIME_TickCount;
#define BUTTONBARMESSAGEWITDTH 120
  DISPLAY_state.ButtonBarMessage = LE_GRAFIX_ObjectCreate( BUTTONBARMESSAGEWITDTH, 20, LE_GRAFIX_ObjectTransparent);
  DISPLAY_state.ButtonBarMessageIsUp = FALSE;

  DISPLAY_state.IBarButtonLastMouseOver   = DISPLAY_state.IBarButtonCurrentMouseOver   = -1;
  DISPLAY_state.IBarPropertyLastMouseOver = DISPLAY_state.IBarPropertyCurrentMouseOver = -1;
  DISPLAY_state.IBarPlayerLastMouseOver   = DISPLAY_state.IBarPlayerCurrentMouseOver   = -1;
  for( t = 0; t < IBAR_BUTTON_MAX; t++ )
    DISPLAY_state.IBarButtons[t] = LED_EI;// This parameter may become obsolete, the following array will replace it and handle flying in & out.
  for( t = 0; t < IBAR_BUTTON_DISTINCT_MAX; t++ )
  {// Set up the button state tracker.
      DISPLAY_state.IBarButtonsVisualState[t] = IBAR_BUTTON_ANIM_Off;
    DISPLAY_state.IBarButtonsShown[t] = LED_EI;
  }
  DISPLAY_state.IBarGameJustLoaded = FALSE;
  DISPLAY_state.JustReadACardHack = FALSE;

  for( t = 0; t < IBAR_BUTTON_MAX; t++ )
  {// Set up the custom button bar location arrays.  Start by clearing them.
    if( (t >= IBAR_General1 && t <= IBAR_General4) || t == IBAR_Main )
    {
      IBAR_ButtonBuyAuctionLocations[t].left = IBAR_ButtonBuyAuctionLocations[t].right = 0;
      IBAR_ButtonBuyAuctionLocations[t].top = IBAR_ButtonGeneralLocations[IBAR_Main].top;// Copy Main button y coords.
      IBAR_ButtonBuyAuctionLocations[t].bottom = IBAR_ButtonGeneralLocations[IBAR_Main].bottom;
      IBAR_ButtonTaxChoiceLocations[t].left = IBAR_ButtonTaxChoiceLocations[t].right = 0;
      IBAR_ButtonTaxChoiceLocations[t].top = IBAR_ButtonGeneralLocations[IBAR_Main].top;// Copy Main button y coords.
      IBAR_ButtonTaxChoiceLocations[t].bottom = IBAR_ButtonGeneralLocations[IBAR_Main].bottom;
      IBAR_ButtonTradeChoiceLocations[t].left = IBAR_ButtonTradeChoiceLocations[t].right = 0;
      IBAR_ButtonTradeChoiceLocations[t].top = IBAR_ButtonGeneralLocations[IBAR_Main].top;// Copy Main button y coords.
      IBAR_ButtonTradeChoiceLocations[t].bottom = IBAR_ButtonGeneralLocations[IBAR_Main].bottom;
    } else
    {
      IBAR_ButtonBuyAuctionLocations[t].left  = IBAR_ButtonGeneralLocations[t].left;
      IBAR_ButtonBuyAuctionLocations[t].right = IBAR_ButtonGeneralLocations[t].right;
      IBAR_ButtonBuyAuctionLocations[t].top   = IBAR_ButtonGeneralLocations[t].top;// Copy Main button y coords.
      IBAR_ButtonBuyAuctionLocations[t].bottom = IBAR_ButtonGeneralLocations[t].bottom;
      IBAR_ButtonTaxChoiceLocations[t].left  = IBAR_ButtonGeneralLocations[t].left;
      IBAR_ButtonTaxChoiceLocations[t].right = IBAR_ButtonGeneralLocations[t].right;
      IBAR_ButtonTaxChoiceLocations[t].top   = IBAR_ButtonGeneralLocations[t].top;// Copy Main button y coords.
      IBAR_ButtonTaxChoiceLocations[t].bottom = IBAR_ButtonGeneralLocations[t].bottom;
      IBAR_ButtonTradeChoiceLocations[t].left  = IBAR_ButtonGeneralLocations[t].left;
      IBAR_ButtonTradeChoiceLocations[t].right = IBAR_ButtonGeneralLocations[t].right;
      IBAR_ButtonTradeChoiceLocations[t].top   = IBAR_ButtonGeneralLocations[t].top;// Copy Main button y coords.
      IBAR_ButtonTradeChoiceLocations[t].bottom = IBAR_ButtonGeneralLocations[t].bottom;
    }
  }
  // Set up the active custom button locations.  Hard coded, deal with it...
  IBAR_ButtonBuyAuctionLocations[IBAR_Main].left  = 251;
  IBAR_ButtonBuyAuctionLocations[IBAR_Main].right = 354;
  IBAR_ButtonBuyAuctionLocations[IBAR_General3].left  = 467;
  IBAR_ButtonBuyAuctionLocations[IBAR_General3].right = 569;

  IBAR_ButtonTaxChoiceLocations[IBAR_Main].left  = 254;
  IBAR_ButtonTaxChoiceLocations[IBAR_Main].right = 355;
  IBAR_ButtonTaxChoiceLocations[IBAR_General3].left  = 464;
  IBAR_ButtonTaxChoiceLocations[IBAR_General3].right = 567;

  IBAR_ButtonTradeChoiceLocations[IBAR_Main].left   = 349;
  IBAR_ButtonTradeChoiceLocations[IBAR_Main].right  = 449;
  IBAR_ButtonTradeChoiceLocations[IBAR_Main].top    = 420;
  IBAR_ButtonTradeChoiceLocations[IBAR_Main].bottom = 448;

  IBAR_ButtonTradeChoiceLocations[IBAR_General2].left   = 224;
  IBAR_ButtonTradeChoiceLocations[IBAR_General2].right  = 324;
  IBAR_ButtonTradeChoiceLocations[IBAR_General2].top    = 420;
  IBAR_ButtonTradeChoiceLocations[IBAR_General2].bottom = 448;

  IBAR_ButtonTradeChoiceLocations[IBAR_General3].left   = 473;
  IBAR_ButtonTradeChoiceLocations[IBAR_General3].right  = 573;
  IBAR_ButtonTradeChoiceLocations[IBAR_General3].top    = 420;
  IBAR_ButtonTradeChoiceLocations[IBAR_General3].bottom = 448;

  // Get out of jail is great except for General2 - the left x coord is lefter (239).

  // Set up Property bar
  DISPLAY_state.IBarPropertyCurrentMouseOverCheck = -1;
  DISPLAY_state.IBarPropertyCurrentMouseOverTickCount = 0;//Move to display.h
  DISPLAY_state.IBarPropertyBlownUp = LED_EI;
  for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
  {
    DISPLAY_state.IBarProperties[t] = LED_EI;
    DISPLAY_state.IBarPropertiesPriority[t] = 0;
  }
  for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
  {
    // coord calcs.  Lotsa conversions.
    itemp = DISPLAY_propertyToOwnablePropertyConversion( t );
    if( itemp == -1 ) continue;
    barOrder = IBARPropertyBarOrder[itemp]; // Obtain display order in tray.
    if( barOrder < 18 )
    {
      x =  28 + 54*(int)(barOrder / 3 ) - 5*(barOrder % 3);
      y = 495 + 10*(barOrder % 3);
    } else
    {
      x = 482 + 54*(int)((barOrder - 18) / 3 ) + 5*(barOrder % 3);
      y = 495 + 10*(barOrder % 3);
    }
    IBAR_PropertyLocations[t].left   = x;
    IBAR_PropertyLocations[t].top    = y;
    IBAR_PropertyLocations[t].right  = x + 34;
    IBAR_PropertyLocations[t].bottom = y + 42;
  }

  // Jail cards.  Kinda hacked
  IBAR_JailCardLocations[0].left   = 740;
  IBAR_JailCardLocations[0].top    = 506;
  IBAR_JailCardLocations[0].right  = 540 + 33;
  IBAR_JailCardLocations[0].bottom = 506 + 39;
  IBAR_JailCardLocations[1].left   = IBAR_JailCardLocations[0].left + 10;
  IBAR_JailCardLocations[1].top    = IBAR_JailCardLocations[0].top + 19;
  IBAR_JailCardLocations[1].right  = IBAR_JailCardLocations[0].right + 10;
  IBAR_JailCardLocations[1].bottom = IBAR_JailCardLocations[0].bottom + 19;
  DISPLAY_state.IBarJailCards[0] = DISPLAY_state.IBarJailCards[1] = LED_EI;
  // Create the banks jail cards - they get printed on later.
  DISPLAY_state.IBarBankJailCards[0] = LE_GRAFIX_ObjectCreate(
            LE_GRAFIX_TCBwidth( LE_DATA_IdFromTag( DAT_MAIN, TAB_inhouse )),
            LE_GRAFIX_TCBheight(LE_DATA_IdFromTag( DAT_MAIN, TAB_inhouse )),
            LE_GRAFIX_ObjectTransparent);
  DISPLAY_state.IBarBankJailCards[1] = LE_GRAFIX_ObjectCreate(
            LE_GRAFIX_TCBwidth( LE_DATA_IdFromTag( DAT_MAIN, TAB_inhotel )),
            LE_GRAFIX_TCBheight(LE_DATA_IdFromTag( DAT_MAIN, TAB_inhotel )),
            LE_GRAFIX_ObjectTransparent);
  DISPLAY_state.IBarBankJailCardValuesPrinted[0] = DISPLAY_state.IBarBankJailCardValuesPrinted[1] = -12345;// Force an update before display.


  // Create permanent score objects.
  for( t = 0; t < RULE_MAX_PLAYERS; t++ )
  {
    DISPLAY_state.PlayerTokens[t] = LED_EI;
    DISPLAY_state.PlayerTokenJailUp[t] = FALSE;
    DISPLAY_state.PlayerScoreBoxes[t] = LE_GRAFIX_ObjectCreate(
            LE_GRAFIX_TCBwidth( LE_DATA_IdFromTag( DAT_MAIN, TAB_inpsl0 )),
            LE_GRAFIX_TCBheight(LE_DATA_IdFromTag( DAT_MAIN, TAB_inpsl0 )),
            LE_GRAFIX_ObjectTransparent);
    DISPLAY_state.PlayerScoreBoxesLocX[t] = -1;
  }
  // Scores displayed off
  for( t = 0; t < RULE_MAX_PLAYERS; t++ )
  {
    DISPLAY_state.IsPlayerDisplayed[t] = FALSE;
    DISPLAY_state.LastPlayerScoresPrinted[t] = -1; // Force the first redraw
    Asprintf( DISPLAY_state.namePrinted[t], A_T("") );
    DISPLAY_state.LastPlayerScoreUpdateTime[t] = 0;
    DISPLAY_state.PlayerColorBarShownID[t] = LED_EI;
  }
  DISPLAY_state.IsBankDisplayed = FALSE;

  // Current Player indicators
  for( t = 0; t < RULE_MAX_PLAYERS; t++ )
  {
    DISPLAY_state.CurrentPlayerNameTags[t] = LE_GRAFIX_ObjectCreate(
            LE_GRAFIX_TCBwidth( LE_DATA_IdFromTag( DAT_MAIN, TAB_indsturn0 ) + 2),
            LE_GRAFIX_TCBheight(LE_DATA_IdFromTag( DAT_MAIN, TAB_indsturn0 ) + 2), // A little bigger in case they are not all exactly the same size.
            LE_GRAFIX_ObjectTransparent);
    DISPLAY_state.CurrentPlayerNamesDrawn[t][0] = NULL;
  }
  DISPLAY_state.CurrentPlayerShownID = LED_EI;
  DISPLAY_state.CurrentPlayerColorID = LED_EI;
  DISPLAY_state.CurrentPlayerNameTag = LED_EI;

  // Chance/ Commmunity Chest graphics controles
  DISPLAY_state.ChanceCommChestDesired = -1;
  DISPLAY_state.ChanceCommChestAnim = -1;
  DISPLAY_state.ChanceCommChestDataID = LED_EI;
  
  
  // set the status bar IDs
  UDIBar_InitStatusButtons();
  // set the options buttons
  UDIBar_InitOptionsButtons();

  // For non-usa compiles, clear card set
#if !USA_VERSION
  int s;
  for( t = 0, s = 0; t < 56; t++ )
      UDIBAR_Deeds[s] = LED_EI;
#endif
}

void DISPLAY_UDIBAR_Destroy( void )
{ // Deallocate stuff from Initialize.
  int t;

  // kill the objects
  LE_DATA_FreeRuntimeDataID( DISPLAY_state.ButtonBarMessage );
  for( t = 0; t < RULE_MAX_PLAYERS; t++ )
    LE_DATA_FreeRuntimeDataID( DISPLAY_state.PlayerScoreBoxes[ t ] );

  for( t = 0; t < RULE_MAX_PLAYERS; t++ )
    LE_DATA_FreeRuntimeDataID( DISPLAY_state.CurrentPlayerNameTags[t] );

  for( t = 0; t < 2; t++ )
    LE_DATA_FreeRuntimeDataID( DISPLAY_state.IBarBankJailCards[t] );

}


void DISPLAY_UDIBAR_TickActions( int numberOfTicks )
{// 60ths of a second, this is called before show to allow programmatic movement control

}


void DISPLAY_UDIBAR_Show( void )
{ // Recalculate what should be on the screen, compare to what is on the screen,
  // and take corrective action.  Everything is in the user interface and display states.
  register int t;
  int   player, itemp;
  long  ltemp;
  BOOL  btemp;
  int   x, barOrder; // Hopefully temporary x, y coords to start sequences.
  int   playerWidth, playerHeight;  // w,h of player boxes on score bar.
  ACHAR myBuffer[20]; // printing temp
  RULE_PropertySet PropFullColor, PropLowColor, PropFCMortgaged, PropLCMortgaged;
  LE_DATA_DataId   tempID, tempID1, tempID2;
  LE_DATA_DataId   IBarDesiredButtons[IBAR_BUTTON_MAX];
  // This records the final buttons which should and should not be present by IBAR_ButtonAnimModes.
  int             IBarFinishButtons[IBAR_BUTTON_DISTINCT_MAX];
  LE_DATA_DataId  IBarDesiredButtonsID[IBAR_BUTTON_DISTINCT_MAX];
  BOOL            IBarIsStable;
  BOOL            tokenChange;

// Note the font is set by display_initialize, we just set the weight & size.
  LE_FONTS_SetSize  ( 12 ); // pixels tall.
  LE_FONTS_SetWeight(700);



  // Temporary conversion dialog
/*  if ( (DISPLAY_IsIBarVisible) ||// Not really related to IBAR, temp dialogs.
   (DISPLAY_state.desired2DView == DISPLAY_SCREEN_Pselect) )
    if( (DISPLAY_state.CurrentDialog < 0) || (DISPLAY_state.CurrentDialog >= DISPLAY_Dialog_MAX) )
      tempID = LED_EI;
    else
      tempID = LED_IFT( DAT_LANG2, TAB_temp00 ) + DISPLAY_state.CurrentDialog;
  else
    tempID = LED_EI;
  // Determine if the dialog requires updating.
  if( DISPLAY_state.CurrentDialogDataID != tempID )
  {
    if( DISPLAY_state.CurrentDialogDataID != LED_EI )
    {
      LE_SEQNCR_Stop( DISPLAY_state.CurrentDialogDataID, DISPLAY_state.CurrentDialogPriority );
      DISPLAY_state.CurrentDialogDataID = LED_EI;
    }

    DISPLAY_state.CurrentDialogPriority = DISPLAY_DialogPriority;
    DISPLAY_state.CurrentDialogDataID = tempID;
    if( DISPLAY_state.CurrentDialogDataID != LED_EI )
      LE_SEQNCR_StartXY( DISPLAY_state.CurrentDialogDataID, DISPLAY_state.CurrentDialogPriority,
        DISPLAY_Dialog_OffsetX, DISPLAY_Dialog_OffsetY );
  }*/


  // Identify the correct player to show in the property bar
  if( (DISPLAY_state.IBarStateTrackOn) &&
    ( (DISPLAY_state.IBarCurrentState  != DISPLAY_state.IBarLastRuleState) ||
      (DISPLAY_state.IBarCurrentPlayer != DISPLAY_state.IBarLastRulePlayer) ))
  {// Do not do this unless we must - the justchanged flag is the important one - in buttons don't wait for out without it.
    DISPLAY_state.IBarCurrentState  = DISPLAY_state.IBarLastRuleState;
    DISPLAY_state.IBarCurrentPlayer = DISPLAY_state.IBarLastRulePlayer;
    IBAR_JustChanged = TRUE;
        // set status screen flags to update screen
        //if(fStatusScreen == PLAYER_SCREEN)
        {
            g_bIsScreenUpdated = FALSE;
            g_Button = last_sort_result + B_SORT_1;
        }
  }
  player = DISPLAY_state.IBarCurrentPlayer; // CurrentPlayer would be set when deviation from track started.

  // Put the correct players backdrop up
  if (DISPLAY_IsIBarVisible)
    if( player == RULE_MAX_PLAYERS )
      tempID = LED_IFT(DAT_MAIN, TAB_indsbg7);// Bank player active.
    else
      tempID = LED_IFT(DAT_MAIN, TAB_indsbg0) + UICurrentGameState.Players[player].colour;
  else
    tempID = LED_EI;
  if( DISPLAY_state.CurrentIbarBackdrop != tempID )
  {// Update required
    if( DISPLAY_state.CurrentIbarBackdrop != LED_EI )
    {
        LE_SEQNCR_Stop( DISPLAY_state.CurrentIbarBackdrop, DISPLAY_BackgroundPriority + 1);
        DISPLAY_state.CurrentIbarBackdrop = LED_EI;
    }
    if( tempID != LED_EI )
    {
      DISPLAY_state.CurrentIbarBackdrop = tempID;
      LE_SEQNCR_StartXY( DISPLAY_state.CurrentIbarBackdrop, DISPLAY_BackgroundPriority + 1,
        0, UDIBAR_IBarTop );
    }
        // set status screen flags to update screen
        //if(fStatusScreen == PLAYER_SCREEN)
        {
            g_bIsScreenUpdated = FALSE;
            g_Button = last_sort_result + B_SORT_1;
        }
  }


  // Update the action button bar
  for( t = 0; t < IBAR_BUTTON_MAX; t++ )
    IBarDesiredButtons[t] = LED_EI; // Clear them (old)
  for( t = 0; t < IBAR_BUTTON_DISTINCT_MAX; t++ )
    IBarFinishButtons[t] = IBAR_BUTTON_ANIM_Off; // Clear them

  DISPLAY_state.IBarBSSMButtonsAvail = 0; // Reset BSSM button tracker (Button click code uses this
  //DISPLAY_state.IBarDeedButtonsAvail |= UDIBAR_DeedStateButtonBuyHouse;
  if( DISPLAY_IsIBarVisible )
  {// Button bar is showing.
    IBarFinishButtons[IBAR_BUTTON_ANIM_Camera]  = IBAR_BUTTON_ANIM_Idle;
    if( GameInProgress )
    {
      IBarFinishButtons[IBAR_BUTTON_ANIM_Options] = IBAR_BUTTON_ANIM_Idle;
      // If no trade in progress, show the trade button
      //if( !UICurrentGameState.TradeInProgress )
      if( DISPLAY_state.desired2DView != DISPLAY_SCREEN_TradeA )
      {// Bankrupt players can't go to the trade screen.
        if( UICurrentGameState.Players[DISPLAY_state.IBarCurrentPlayer].currentSquare != 41 )
          if( UDTrade_GetPlayerToTradeFrom() != RULE_MAX_PLAYERS )// Someone here to trade
            IBarFinishButtons[IBAR_BUTTON_ANIM_Trade] = IBAR_BUTTON_ANIM_Idle;
      }
      // Main and Status screen buttons
      if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA ||
        DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA )
        IBarFinishButtons[IBAR_BUTTON_ANIM_Main] = IBAR_BUTTON_ANIM_Idle;
      if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA )
        IBarFinishButtons[IBAR_BUTTON_ANIM_Status] = IBAR_BUTTON_ANIM_Idle;
    }

    //NOTE:  You can move to your own bar when a player is expected to roll.
    //DISPLAY_state.IBarCurrentState should hold the local display state,
    //DISPLAY_state.IBarLastRuleState would hold the 'real' state.  May need more depth before we're done.
    //if( DISPLAY_state.IBarStateTrackOn && (DISPLAY_state.IBarLastRuleState != DISPLAY_state.IBarCurrentState) )
    //  UDIBAR_setIBarCurrentState( DISPLAY_state.IBarLastRuleState );

    // Processor intensive, do when player changes or once per second - calculates if the buy or sell button should show.
    static int BuildSellPlayer = -1;
    static TYPE_Tick BuildSellTime = 0;
    static BOOL CanBuild = FALSE, CanSell = FALSE;

    if( (BuildSellPlayer != player) || (BuildSellTime + 60 < LE_TIME_TickCount) )
    {// Re-evaluate button state - could reduce to only calculate in appropriate button bar states.
      if( player != RULE_MAX_PLAYERS )
      {
        CanBuild = DISPLAY_PlaceBuildingLegalSquares( player );
        CanSell = DISPLAY_SellBuildingLegalSquares( player );
      } else
      {
        CanBuild = FALSE;
        CanSell = FALSE;
      }
      BuildSellPlayer = player;
      BuildSellTime = LE_TIME_TickCount;
    }

    // We store the state of the player.
    AI_ButtonRemoteState = !SlotIsALocalHumanPlayer [DISPLAY_state.IBarCurrentPlayer];
    if( DISPLAY_state.IBarCurrentState == IBAR_STATE_OtherPlayerRemote )
      AI_ButtonRemoteState = FALSE; // Override for this mode (a done button to go back to current player) FIXME - a test.

    // Kick the cash message if in raise money or hotel decomp.
      // Request the button message box - it has the amount to raise.
    if( DISPLAY_state.IBarLastRuleState == IBAR_STATE_RaiseMoney || DISPLAY_state.IBarLastRuleState == IBAR_STATE_HotelDecomposition )
      // Just barely in time to display (so it goes away quick)
      DISPLAY_state.ButtonBarWantedTime = LE_TIME_TickCount - DISPLAY_BUTTONMESSAGE_SECONDS * 60 + 1;

    switch( DISPLAY_state.IBarCurrentState )
    {// Figure out what buttons SHOULD be showing.  Idle or off states only.
    case IBAR_STATE_StartTurn:
    //case IBAR_STATE_AI_StartTurn:
      //IBarFinishButtons[IBAR_BUTTON_ANIM_Options] = IBAR_BUTTON_ANIM_Idle;
      if( CanBuild )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Build] = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonBuyHouse;
      }
      if( CanSell )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Sell] = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonSellHouse;
      }
      PropFullColor = PropFCMortgaged = 0;
      for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
      {
        if( player == UICurrentGameState.Squares[t].owner )
          if( UDIBAR_testMortgagable( t ) )
            PropFullColor |= RULE_PropertyToBitSet( t );
        if( (player == UICurrentGameState.Squares[t].owner ) &&
             UDIBAR_testUnMortgagable(t) )
        {
            PropFCMortgaged |= RULE_PropertyToBitSet( t );
        }
      }
      if( PropFullColor )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Mortgage] = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonMortgage;
      }
      if( PropFCMortgaged )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Unmort]   = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonUnMortgage;
      }
      IBarFinishButtons[IBAR_BUTTON_ANIM_RollDice] = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_Build:
      IBarFinishButtons[IBAR_BUTTON_ANIM_Done] = IBAR_BUTTON_ANIM_Idle;
      break;
    case IBAR_STATE_Sell:
      IBarFinishButtons[IBAR_BUTTON_ANIM_Done] = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_Mortgage:
      IBarFinishButtons[IBAR_BUTTON_ANIM_Done] = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_UnMortgage:
      IBarFinishButtons[IBAR_BUTTON_ANIM_Done] = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_OtherPlayer:
      //IBarFinishButtons[IBAR_BUTTON_ANIM_Options] = IBAR_BUTTON_ANIM_Idle;
      if( CanBuild )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Build] = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonBuyHouse;
      }
      if( CanSell )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Sell] = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonSellHouse;
      }
      PropFullColor = PropFCMortgaged = 0;
      for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
      {
        if( player == UICurrentGameState.Squares[t].owner )
          if( UDIBAR_testMortgagable( t ) )
            PropFullColor |= RULE_PropertyToBitSet( t );
        if( (player == UICurrentGameState.Squares[t].owner ) &&
             UDIBAR_testUnMortgagable(t) )
        {
            PropFCMortgaged |= RULE_PropertyToBitSet( t );
        }
      }
      if( PropFullColor )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Mortgage] = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonMortgage;
      }
      if( PropFCMortgaged )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Unmort]   = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonUnMortgage;
      }
      IBarFinishButtons[IBAR_BUTTON_ANIM_Done] = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_RaiseMoney:
      // Quicky way to restate raise money line
      if( LE_TIME_TickCount > LastSoundPlayedAt[player] + 40 * 60 && DISPLAY_IsIBarVisible)
      {
        if( DISPLAY_state.SoundToPlayPostLock == LED_EI )
        {
          TRACE("Raise cash from Ibar state");
          UDPENNY_PennybagsGeneral( PB_RaisingMoneySuggestion, Sound_WaitForAnyOldSoundThenPlay );
          LastSoundPlayedAt[player] = LE_TIME_TickCount;
        }
      }

      if( CanSell )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Sell] = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonSellHouse;
      }
      if( DISPLAY_state.IBarRaiseCashCanBankrupt )
        IBarFinishButtons[IBAR_BUTTON_ANIM_Bankrupt] = IBAR_BUTTON_ANIM_Idle;
      // show DISPLAY_state.IBarRaiseTotalCashNeeded cash total needed.

      PropFullColor = PropFCMortgaged = 0;
      for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
      {
        if( player == UICurrentGameState.Squares[t].owner )
          if( UDIBAR_testMortgagable( t ) )
            PropFullColor |= RULE_PropertyToBitSet( t );
        if( (player == UICurrentGameState.Squares[t].owner ) &&
             UDIBAR_testUnMortgagable(t) )
        {
            PropFCMortgaged |= RULE_PropertyToBitSet( t );
        }
      }
      if( PropFullColor )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Mortgage] = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonMortgage;
      }
      break;

    case IBAR_STATE_DoneTurn:
      //IBarFinishButtons[IBAR_BUTTON_ANIM_Options] = IBAR_BUTTON_ANIM_Idle;
      if( CanBuild )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Build] = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonBuyHouse;
      }
      if( CanSell )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Sell] = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonSellHouse;
      }
      PropFullColor = PropFCMortgaged = 0;
      for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
      {
        if( player == UICurrentGameState.Squares[t].owner )
          if( UDIBAR_testMortgagable( t ) )
            PropFullColor |= RULE_PropertyToBitSet( t );
        if( (player == UICurrentGameState.Squares[t].owner ) &&
             UDIBAR_testUnMortgagable(t) )
        {
            PropFCMortgaged |= RULE_PropertyToBitSet( t );
        }
      }
      if( PropFullColor )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Mortgage] = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonMortgage;
      }
      if( PropFCMortgaged )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Unmort]   = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonUnMortgage;
      }
      IBarFinishButtons[IBAR_BUTTON_ANIM_Done] = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_DeedActive:
      if( UICurrentGameState.Players[DISPLAY_state.IBarCurrentPlayer].cash != DISPLAY_state.IBarDeedButtonsCash )
        UDIBAR_DeedStateCalcButtons(); // Cash changed, re-evaluate buttons.
      //IBarFinishButtons[IBAR_BUTTON_ANIM_Options] = IBAR_BUTTON_ANIM_Idle;
      if( DISPLAY_state.IBarDeedButtonsAvail & UDIBAR_DeedStateButtonBuyHouse )
        IBarFinishButtons[IBAR_BUTTON_ANIM_Build] = IBAR_BUTTON_ANIM_Idle;
      if( DISPLAY_state.IBarDeedButtonsAvail & UDIBAR_DeedStateButtonSellHouse )
        IBarFinishButtons[IBAR_BUTTON_ANIM_Sell] = IBAR_BUTTON_ANIM_Idle;
      if( DISPLAY_state.IBarDeedButtonsAvail & UDIBAR_DeedStateButtonMortgage )
        IBarFinishButtons[IBAR_BUTTON_ANIM_Mortgage] = IBAR_BUTTON_ANIM_Idle;
      if( DISPLAY_state.IBarDeedButtonsAvail & UDIBAR_DeedStateButtonUnMortgage )
        IBarFinishButtons[IBAR_BUTTON_ANIM_Unmort]   = IBAR_BUTTON_ANIM_Idle;
      IBarFinishButtons[IBAR_BUTTON_ANIM_Done] = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_OtherPlayerRemote:
      if( !DISPLAY_state.IBarStateTrackOn ) // Done button turns tracking on
        IBarFinishButtons[IBAR_BUTTON_ANIM_Done] = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_BuyAuction:
      IBarFinishButtons[IBAR_BUTTON_ANIM_Auction] = IBAR_BUTTON_ANIM_Idle;
      IBarFinishButtons[IBAR_BUTTON_ANIM_Buy]     = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_TaxDecision:
      IBarFinishButtons[IBAR_BUTTON_ANIM_FlatTax]    = IBAR_BUTTON_ANIM_Idle;
      IBarFinishButtons[IBAR_BUTTON_ANIM_Percentage] = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_ViewingCard:
      IBarFinishButtons[IBAR_BUTTON_ANIM_Done] = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_Trading:
      IBarFinishButtons[IBAR_BUTTON_ANIM_TradeAcc] = IBAR_BUTTON_ANIM_Idle;
      IBarFinishButtons[IBAR_BUTTON_ANIM_TradeCnt] = IBAR_BUTTON_ANIM_Idle;
      IBarFinishButtons[IBAR_BUTTON_ANIM_TradeRej] = IBAR_BUTTON_ANIM_Idle;
      /*IBarDesiredButtons[IBAR_General2] = LED_IFT( DAT_LANG2, TAB_ibentaco );
      IBarDesiredButtons[IBAR_General3] = LED_IFT( DAT_LANG2, TAB_ibentaat );
      IBarDesiredButtons[IBAR_Main]     = LED_IFT( DAT_LANG2, TAB_ibentart );*/
      break;

    case IBAR_STATE_JailExitPCR://Jail multi-state logic
    case IBAR_STATE_JailExitPCX:
      IBarFinishButtons[IBAR_BUTTON_ANIM_UseCard] = IBAR_BUTTON_ANIM_Idle;
    case IBAR_STATE_JailExitPXR:
      if( DISPLAY_state.IBarCurrentState != IBAR_STATE_JailExitPCX )
        IBarFinishButtons[IBAR_BUTTON_ANIM_RollDice] = IBAR_BUTTON_ANIM_Idle;
    case IBAR_STATE_JailExitPXX:
      IBarFinishButtons[IBAR_BUTTON_ANIM_Pay] = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_FreeUnmortgage:
      //IBarFinishButtons[IBAR_BUTTON_ANIM_Options] = IBAR_BUTTON_ANIM_Idle;
      if( CanBuild )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Build] = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonBuyHouse;
      }
      if( CanSell )
      {
        IBarFinishButtons[IBAR_BUTTON_ANIM_Sell] = IBAR_BUTTON_ANIM_Idle;
        DISPLAY_state.IBarBSSMButtonsAvail |= UDIBAR_DeedStateButtonSellHouse;
      }
      PropFullColor = PropFCMortgaged = 0;
      for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
      {
        if( player == UICurrentGameState.Squares[t].owner )
          if( UDIBAR_testMortgagable( t ) )
            PropFullColor |= RULE_PropertyToBitSet( t );
        if( (player == UICurrentGameState.Squares[t].owner ) &&
             UDIBAR_testUnMortgagable(t) )
        {
            PropFCMortgaged |= RULE_PropertyToBitSet( t );
        }
      }
      if( PropFullColor )
        IBarFinishButtons[IBAR_BUTTON_ANIM_Mortgage] = IBAR_BUTTON_ANIM_Idle;
      if( PropFCMortgaged )
        IBarFinishButtons[IBAR_BUTTON_ANIM_Unmort]   = IBAR_BUTTON_ANIM_Idle;
      IBarFinishButtons[IBAR_BUTTON_ANIM_Done] = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_HousingShort:
      IBarFinishButtons[IBAR_BUTTON_ANIM_AucHouse]   = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_HotelShort:
      IBarFinishButtons[IBAR_BUTTON_ANIM_AucHotel]   = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_PlaceHouse:
      IBarFinishButtons[IBAR_BUTTON_ANIM_PlaceHouse]   = IBAR_BUTTON_ANIM_Idle;
      break;
      
    case IBAR_STATE_PlaceHotel:
      IBarFinishButtons[IBAR_BUTTON_ANIM_PlaceHotel]   = IBAR_BUTTON_ANIM_Idle; 
      break;

    case IBAR_STATE_GameOver:
      IBarFinishButtons[IBAR_BUTTON_ANIM_NewGame]   = IBAR_BUTTON_ANIM_Idle;
      IBarFinishButtons[IBAR_BUTTON_ANIM_Exit]      = IBAR_BUTTON_ANIM_Idle;
      // FIXME BLEEBLE:  Not working yet...
      // IBarFinishButtons[IBAR_BUTTON_ANIM_PlayAgain] = IBAR_BUTTON_ANIM_Idle;
      break;
      
    case IBAR_STATE_StatusScreen:
      //IBarFinishButtons[IBAR_BUTTON_ANIM_Options] = IBAR_BUTTON_ANIM_Idle;
      break;

    case IBAR_STATE_HotelDecomposition:
      IBarFinishButtons[IBAR_BUTTON_ANIM_Sell] = IBAR_BUTTON_ANIM_Idle;
      DISPLAY_state.IBarBSSMButtonsAvail = UDIBAR_DeedStateButtonSellHouse;
      
    case IBAR_STATE_Nothing:
    default:
      break;
    }

    // Check if our state disallows BSSM stuff for players other than current.
    if( DISPLAY_state.IBarCurrentPlayer != DISPLAY_state.IBarLastRulePlayer )
    {// May have to kill the extra buttons
      if( DISPLAY_state.IBarLastRuleState == IBAR_STATE_HotelDecomposition ||
        DISPLAY_state.IBarLastRuleState == IBAR_STATE_PlaceHouse ||
        DISPLAY_state.IBarLastRuleState == IBAR_STATE_PlaceHotel )
      {// Shut down bssm stuff
        DISPLAY_state.IBarBSSMButtonsAvail = 0;
        IBarFinishButtons[IBAR_BUTTON_ANIM_Build] = IBAR_BUTTON_ANIM_Off;
        IBarFinishButtons[IBAR_BUTTON_ANIM_Sell] = IBAR_BUTTON_ANIM_Off;
        IBarFinishButtons[IBAR_BUTTON_ANIM_Mortgage] = IBAR_BUTTON_ANIM_Off;
        IBarFinishButtons[IBAR_BUTTON_ANIM_Unmort]   = IBAR_BUTTON_ANIM_Off;
      }
    }
  }

  // Affect animations to incrementally correct the button graphics.
  if( (DISPLAY_state.ButtonBarWantedTime > (LE_TIME_TickCount - DISPLAY_BUTTONMESSAGE_SECONDS * 60)) && (DISPLAY_IsIBarVisible) )
  {// We want the messagebox up (raise cash, houses left...?)
    if( !DISPLAY_state.ButtonBarMessageIsUp || DISPLAY_state.ButtonBarForceUpdate )
    {
      if( DISPLAY_state.ButtonBarMessageIsUp )
      {// Force update by shutting off then on.
        LE_SEQNCR_Stop( DISPLAY_state.ButtonBarMessage, DISPLAY_FREAKIN_HIGH );
        DISPLAY_state.ButtonBarForceUpdate = FALSE;
        DISPLAY_state.ButtonBarWantedTime = LE_TIME_TickCount;
      }
      LE_SEQNCR_StartXY( DISPLAY_state.ButtonBarMessage, DISPLAY_FREAKIN_HIGH, 796-BUTTONBARMESSAGEWITDTH, 508 );
      DISPLAY_state.ButtonBarMessageIsUp = TRUE;
    }
  } else
  {// Be sure its off
    if( DISPLAY_state.ButtonBarMessageIsUp )
    {
      LE_SEQNCR_Stop( DISPLAY_state.ButtonBarMessage, DISPLAY_FREAKIN_HIGH );
      DISPLAY_state.ButtonBarMessageIsUp = FALSE;
    }
  }
  // NOTE:  First, we see if anything is flying; let that finish before processing more changes.
  IBarIsStable = TRUE;
  for( t = 0; t < IBAR_BUTTON_DISTINCT_MAX; t++ )
  {
    if( (DISPLAY_state.IBarButtonsVisualState[t] != IBAR_BUTTON_ANIM_Off) &&
        (DISPLAY_state.IBarButtonsVisualState[t] != IBAR_BUTTON_ANIM_Idle) )
    {
      IBarIsStable = FALSE;
      break;
    }
  }

  for( t = 0; t < IBAR_BUTTON_DISTINCT_MAX; t++ )
  {
    LE_SEQNCR_RuntimeInfoRecord      InfoGeneric;
    BOOL useGreyButtons, AiRelated;

    // This is a bit of a hack - we want certain buttons to appear above others, rather than do a table I do this.
    int tempPriority;
    switch( t )
    {
    case IBAR_BUTTON_ANIM_Build:
    case IBAR_BUTTON_ANIM_Unmort:
      tempPriority = DISPLAY_IBAR_ButtonBasePriority + 1;
      break;
    case IBAR_BUTTON_ANIM_Auction:
    case IBAR_BUTTON_ANIM_Sell:
    case IBAR_BUTTON_ANIM_Percentage:
    case IBAR_BUTTON_ANIM_Bankrupt:
    case IBAR_BUTTON_ANIM_Mortgage:
    case IBAR_BUTTON_ANIM_Pay:
    case IBAR_BUTTON_ANIM_UseCard:
      tempPriority = DISPLAY_IBAR_ButtonBasePriority + 2;
      break;
    case IBAR_BUTTON_ANIM_Done:
    case IBAR_BUTTON_ANIM_Buy:
    case IBAR_BUTTON_ANIM_FlatTax:
    case IBAR_BUTTON_ANIM_RollDice:
    case IBAR_BUTTON_ANIM_AucHouse:
    case IBAR_BUTTON_ANIM_AucHotel:
      tempPriority = DISPLAY_IBAR_ButtonBasePriority + 3;
      break;
    case IBAR_BUTTON_ANIM_Camera:
    case IBAR_BUTTON_ANIM_Options:
    case IBAR_BUTTON_ANIM_Status:
    case IBAR_BUTTON_ANIM_Trade:
    case IBAR_BUTTON_ANIM_Main:
    default:
      tempPriority = DISPLAY_IBAR_ButtonBasePriority;
      break;
    }

    // First up - some buttons are full color regardless of an AI or Remote player being in control - use full color.
    useGreyButtons = AI_ButtonRemoteState;
    AiRelated = FALSE;
    if( t == IBAR_BUTTON_ANIM_Camera || t == IBAR_BUTTON_ANIM_Main || t == IBAR_BUTTON_ANIM_Options ||
      t == IBAR_BUTTON_ANIM_Status || t == IBAR_BUTTON_ANIM_Trade ||
      t == IBAR_BUTTON_ANIM_NewGame || t == IBAR_BUTTON_ANIM_Exit || t == IBAR_BUTTON_ANIM_PlayAgain )
    {
      AiRelated = FALSE;
      useGreyButtons = FALSE;
    }

    // We will want to force idle buttons to move out if the Remote/Local state has changed (to get correct color)
    BOOL forceReset = FALSE;
    if( AI_ButtonRemoteState != last_AI_ButtonRemoteState )
      if( DISPLAY_state.IBarButtonsVisualState[t] == IBAR_BUTTON_ANIM_Idle )
        if( t != IBAR_BUTTON_ANIM_Camera && t != IBAR_BUTTON_ANIM_Main && t != IBAR_BUTTON_ANIM_Options &&
          t != IBAR_BUTTON_ANIM_Status && t != IBAR_BUTTON_ANIM_Trade )
          forceReset = TRUE;

    IBarDesiredButtonsID[t] = DISPLAY_state.IBarButtonsShown[t]; // Continue current display unless otherwise indicated.
    if( IBarFinishButtons[t] != DISPLAY_state.IBarButtonsVisualState[t] ||  // Button is in wrong state
      (IBAR_JustChanged && t == DISPLAY_state.IBarLastActionNotificationFor) || // Button stays the same but was pressed.
      (forceReset) ) // Switch from player to AI (NEW)
    {// Continue moving current to desired over time - anims come in and go out.  Respect the IsStable flag - all out before new come in.
      switch( DISPLAY_state.IBarButtonsVisualState[t] )
      {
      case IBAR_BUTTON_ANIM_Off:
        if( IBarIsStable && !IBAR_JustChanged ) // Don't fly in until outgoing get a chance to start & set unstable flag.
        {
          { // Start moving the button in
            DISPLAY_state.IBarButtonsVisualState[t] = IBAR_BUTTON_ANIM_In;
            if( useGreyButtons )
              IBarDesiredButtonsID[t] = IBAR_BUTTON_BASE_AI_ + t*IBAR_BUTTON_ANIMS_PER_SET + IBAR_BUTTON_ANIM_In;
            else
              IBarDesiredButtonsID[t] = IBAR_BUTTON_BASE_SEQ + t*IBAR_BUTTON_ANIMS_PER_SET + IBAR_BUTTON_ANIM_In;
          }
        }
        break;

      case IBAR_BUTTON_ANIM_In:   // Keep checking, idle when done
        //IBarDesiredButtonsID[t] = IBAR_BUTTON_BASE_SEQ + t*0 + IBAR_BUTTON_ANIM_In;
        if( DISPLAY_state.IBarButtonsShown[t] != LED_EI )
        {// In sequence may not have started yet.
          LE_SEQNCR_GetInfo( DISPLAY_state.IBarButtonsShown[t], tempPriority, FALSE,
            &InfoGeneric, NULL, NULL, NULL, NULL );
          if( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
          {// Done animating in
            DISPLAY_state.IBarButtonsVisualState[t] = IBAR_BUTTON_ANIM_Idle;//This will be IBAR_BUTTON_ANIM_In
            if( useGreyButtons )
              IBarDesiredButtonsID[t] = IBAR_BUTTON_BASE_AI_ + t*IBAR_BUTTON_ANIMS_PER_SET + IBAR_BUTTON_ANIM_Idle;
            else
              IBarDesiredButtonsID[t] = IBAR_BUTTON_BASE_SEQ + t*IBAR_BUTTON_ANIMS_PER_SET + IBAR_BUTTON_ANIM_Idle;
          }
        }
        break;
      
      case IBAR_BUTTON_ANIM_Idle: // Start moving the button out.  See UDIBAR_ActionCompleted which sets the pressed button flag.
        if( IBarIsStable || 
          //((AI_ButtonRemoteState != last_AI_ButtonRemoteState) && AiRelated )
          forceReset
          )// working this hard for BSSM state...
        { // Start moving the button out
          DISPLAY_state.IBarButtonsVisualState[t] = IBAR_BUTTON_ANIM_Out;// IBAR_BUTTON_ANIM_Out - pressed flag?
          if( t == DISPLAY_state.IBarLastActionNotificationFor )
          {
            DISPLAY_state.IBarLastActionNotificationFor = -1; // Don't process more than once.
            if( useGreyButtons )
              IBarDesiredButtonsID[t] = IBAR_BUTTON_BASE_AI_ + t*IBAR_BUTTON_ANIMS_PER_SET + IBAR_BUTTON_ANIM_Pressed;
            else
              IBarDesiredButtonsID[t] = IBAR_BUTTON_BASE_SEQ + t*IBAR_BUTTON_ANIMS_PER_SET + IBAR_BUTTON_ANIM_Pressed;
          }
          else
          {
            if( useGreyButtons )
              IBarDesiredButtonsID[t] = IBAR_BUTTON_BASE_AI_ + t*IBAR_BUTTON_ANIMS_PER_SET + IBAR_BUTTON_ANIM_Out;
            else
              IBarDesiredButtonsID[t] = IBAR_BUTTON_BASE_SEQ + t*IBAR_BUTTON_ANIMS_PER_SET + IBAR_BUTTON_ANIM_Out;
          }
        }
        break;

      case IBAR_BUTTON_ANIM_Out:  // Keep checking, off when done
        if( DISPLAY_state.IBarButtonsShown[t] != LED_EI )
        {// In sequence may not have started yet.
          LE_SEQNCR_GetInfo( DISPLAY_state.IBarButtonsShown[t], tempPriority, FALSE,
            &InfoGeneric, NULL, NULL, NULL, NULL );
          if( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
          {// Done animating out ( GetInfo will return false )
            DISPLAY_state.IBarButtonsVisualState[t] = IBAR_BUTTON_ANIM_Off;//This will be IBAR_BUTTON_ANIM_In
            IBarDesiredButtonsID[t] = LED_EI;
          }
        }
        break;
      case IBAR_BUTTON_ANIM_Pressed:  // Keep checking, off when done (like out, but different art)
        if( DISPLAY_state.IBarButtonsShown[t] != LED_EI )
        {// In sequence may not have started yet.
          LE_SEQNCR_GetInfo( DISPLAY_state.IBarButtonsShown[t], tempPriority, FALSE,
            &InfoGeneric, NULL, NULL, NULL, NULL );
          if( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
          {// Done animating out ( GetInfo will return false )
            DISPLAY_state.IBarButtonsVisualState[t] = IBAR_BUTTON_ANIM_Off;//This will be IBAR_BUTTON_ANIM_In
            IBarDesiredButtonsID[t] = LED_EI;
          }
        }
        break;
      }
    }

    // Affect changes
    if( IBarDesiredButtonsID[t] != DISPLAY_state.IBarButtonsShown[t] )
    {// Corrective action.  Mouseover logic dropped in new system.
      // Kill the old, rejoice in the new...
      if( DISPLAY_state.IBarButtonsShown[t] != LED_EI )
        LE_SEQNCR_Stop ( DISPLAY_state.IBarButtonsShown[t], tempPriority );
      DISPLAY_state.IBarButtonsShown[t] = IBarDesiredButtonsID[t];
      if( DISPLAY_state.IBarButtonsShown[t] != LED_EI )
      {
        LE_SEQNCR_StartXYDrop( DISPLAY_state.IBarButtonsShown[t], tempPriority, 0, 0, LE_SEQNCR_DropDropFrames );
        switch( DISPLAY_state.IBarButtonsVisualState[t] )
        {
        /*case IBAR_BUTTON_ANIM_Out:
        case IBAR_BUTTON_ANIM_Pressed:
          LE_SEQNCR_SetEndingAction( DISPLAY_state.IBarButtonsShown[t], tempPriority,
            LE_SEQNCR_EndingActionStop );
          break;*/
        case IBAR_BUTTON_ANIM_Idle:
          //if( t == IBAR_BUTTON_ANIM_Camera || t == IBAR_BUTTON_ANIM_Options || t == IBAR_BUTTON_ANIM_Status ||
          //  t == IBAR_BUTTON_ANIM_Trade || t == IBAR_BUTTON_ANIM_Main )
          // Only default buttons need to loop
          if( t == IBAR_BUTTON_ANIM_Buy || t == IBAR_BUTTON_ANIM_Done || t == IBAR_BUTTON_ANIM_FlatTax ||
            t == IBAR_BUTTON_ANIM_TradeRej || t == IBAR_BUTTON_ANIM_RollDice ||
            t == IBAR_BUTTON_ANIM_Exit || IBAR_BUTTON_ANIM_AucHouse || IBAR_BUTTON_ANIM_AucHotel )
            LE_SEQNCR_SetEndingAction( DISPLAY_state.IBarButtonsShown[t], tempPriority, LE_SEQNCR_EndingActionLoopToBeginning );
          else
            LE_SEQNCR_SetEndingAction( DISPLAY_state.IBarButtonsShown[t], tempPriority, LE_SEQNCR_EndingActionStayAtEnd );
          break;
        default:
          LE_SEQNCR_SetEndingAction( DISPLAY_state.IBarButtonsShown[t], tempPriority,
            LE_SEQNCR_EndingActionStayAtEnd );
          break;
        }
      }
    }

  }
  last_AI_ButtonRemoteState = AI_ButtonRemoteState;
  IBAR_JustChanged = FALSE;

/*  // Affect any changes (old) FIXME - I think only the final new/same/quit dialog uses this.
  for( t = 0; t < IBAR_BUTTON_MAX; t++ )
  {//OPTIMIZE - if this stuff isn't removed (may be obsolete), i think it processes buttons which are not currently in use on the mouseover condition - desired == current == LED_EI, such a waste...
    if( (IBarDesiredButtons[t] != DISPLAY_state.IBarButtons[t]) ||
    // Or is it the before or after MouseOver button
    ( (DISPLAY_state.IBarButtonCurrentMouseOver != DISPLAY_state.IBarButtonLastMouseOver) &&
      ((DISPLAY_state.IBarButtonLastMouseOver == t ) ||
      (DISPLAY_state.IBarButtonCurrentMouseOver == t )) ) )
    {
      // Kill the old, rejoice in the new...
      if( DISPLAY_state.IBarButtons[t] != LED_EI )
      {
        LE_SEQNCR_Stop( DISPLAY_state.IBarButtons[t], DISPLAY_IBAR_ButtonBasePriority );
        DISPLAY_state.IBarButtons[t] = LED_EI;
      }
      if( IBarDesiredButtons[t] != LED_EI )
      {
        DISPLAY_state.IBarButtons[t] = IBarDesiredButtons[t];
        if( LED_IFT( DAT_LANG2, TAB_ibenbaau ) > DISPLAY_state.IBarButtons[t] )
          LE_SEQNCR_StartXY( DISPLAY_state.IBarButtons[t], DISPLAY_IBAR_ButtonBasePriority,
            0,
            (DISPLAY_state.IBarButtonCurrentMouseOver == t) ?
            //IBAR_ButtonGeneralLocations[t].top + 2: IBAR_ButtonGeneralLocations[t].top );
            2: 0 );
        else // old
          LE_SEQNCR_StartXY( DISPLAY_state.IBarButtons[t], DISPLAY_IBAR_ButtonBasePriority,
            IBAR_ButtonGeneralLocations[t].left,
            (DISPLAY_state.IBarButtonCurrentMouseOver == t) ?
            IBAR_ButtonGeneralLocations[t].top + 2: IBAR_ButtonGeneralLocations[t].top );
      }
    }
  }*/
  DISPLAY_state.IBarButtonLastMouseOver = DISPLAY_state.IBarButtonCurrentMouseOver;


  // Chance/ Community chest card display
  {
    LE_SEQNCR_RuntimeInfoRecord      InfoGeneric;
    static int  lastCardIn;  // when no card is desired, this remembers which is up so we get the right out anim.
    static LE_DATA_DataId cardOut;

    if( DISPLAY_state.ChanceCommChestDesired != -1 )
    {// See if we need to remove the card (state change).  It would restart itself when the state pops.
      if( IBAR_STATE_ViewingCard != DISPLAY_state.IBarLastRuleState )
        DISPLAY_state.ChanceCommChestDesired = -1; // State changed.
    }
#if USA_VERSION
    // Move card display along.
    switch ( DISPLAY_state.ChanceCommChestAnim )
    {
#define DISPLAY_CARD_Y_OFFSET_NOT_MAIN 136
    case -1:  // Off
      if( DISPLAY_state.ChanceCommChestDesired != -1 && DISPLAY_IsIBarVisible )
      {
        lastCardIn = DISPLAY_state.ChanceCommChestDesired;
        if( lastCardIn < 16 )
          cardOut = LED_IFT( DAT_MAIN, CNK_cnck01 + DISPLAY_state.desired2DCamera );
        else
          cardOut = LED_IFT( DAT_MAIN, CNK_cnyk01 + DISPLAY_state.desired2DCamera );
        if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA )
          LE_SEQNCR_StartXYDrop( cardOut, DISPLAY_ChanceCommChestPriority, 0, 0, LE_SEQNCR_DropDropFrames );
        else
          LE_SEQNCR_StartXYDrop( cardOut, DISPLAY_ChanceCommChestPriority, 0, DISPLAY_CARD_Y_OFFSET_NOT_MAIN, LE_SEQNCR_DropDropFrames );
        LE_SEQNCR_SetEndingAction( cardOut, DISPLAY_ChanceCommChestPriority,
          LE_SEQNCR_EndingActionStayAtEnd );
        DISPLAY_state.ChanceCommChestAnim++;
      }
      break;

    case 0:   // Monitor Fly off deck, start fly in animation
      if( DISPLAY_IsIBarVisible )
      { // No escalation exit conditions
        if( LE_SEQNCR_GetInfo( cardOut, DISPLAY_ChanceCommChestPriority, FALSE,
          &InfoGeneric, NULL, NULL, NULL, NULL ) )
        {// sequence is running
          if ( InfoGeneric.endTime > InfoGeneric.sequenceClock )
          {// Incoming anim is still running.
            break;
          }
        } else // we will wait for the sequence to start (presumably that would be the only way for getinfo to fail.
          break;
      }
      // If we get here, escalate
      LE_SEQNCR_Stop( cardOut, DISPLAY_ChanceCommChestPriority );
      if( lastCardIn < 16 )
        DISPLAY_state.ChanceCommChestDataID = LED_IFT( DAT_LANG2, CNK_cycai01 + lastCardIn);
      else
        DISPLAY_state.ChanceCommChestDataID = LED_IFT( DAT_LANG2, CNK_cyyi01 + lastCardIn - 16);
      if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA )
        LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, 0, LE_SEQNCR_DropDropFrames );
      else
        LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, DISPLAY_CARD_Y_OFFSET_NOT_MAIN, LE_SEQNCR_DropDropFrames );
      //LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, 0, LE_SEQNCR_DropDropFrames );
      LE_SEQNCR_SetEndingAction( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority,
        LE_SEQNCR_EndingActionStayAtEnd );
      DISPLAY_state.ChanceCommChestAnim++;

    case 1:   // Incoming card, monitor status then start animation
      if( DISPLAY_IsIBarVisible )
      { // No escalation exit conditions
        if( LE_SEQNCR_GetInfo( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, FALSE,
          &InfoGeneric, NULL, NULL, NULL, NULL ) )
        {// sequence is running
          if ( InfoGeneric.endTime > InfoGeneric.sequenceClock )
          {// Incoming anim is still running.
            break;
          }
        } else // we will wait for the sequence to start (presumably that would be the only way for getinfo to fail.
          break;
      }
      // If we get here, escalate
      LE_SEQNCR_Stop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority );
//#if USA_VERSION
      if( lastCardIn < 16 )
        DISPLAY_state.ChanceCommChestDataID = LED_IFT( DAT_LANG2, CNK_ch01 + lastCardIn );
      else
        DISPLAY_state.ChanceCommChestDataID = LED_IFT( DAT_LANG2, CNK_cc01 + lastCardIn - 16);
//#else
//        DISPLAY_state.ChanceCommChestDataID = LED_IFT( DAT_LANG2, UDPENNY_GetCorrectCardCNK(lastCardIn));
//#endif
      // Now we need to fire up the appropriate sound
      UDPENNY_PB_ReadCard( lastCardIn );
      if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA )
        LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, 0, LE_SEQNCR_DropDropFrames );
      else
        LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, DISPLAY_CARD_Y_OFFSET_NOT_MAIN, LE_SEQNCR_DropDropFrames );
      //LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, 0, LE_SEQNCR_DropDropFrames );
      LE_SEQNCR_SetEndingAction( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority,
        LE_SEQNCR_EndingActionStayAtEnd );
      DISPLAY_state.ChanceCommChestAnim++;
      break;

    case 2:   // Monitor Animation then start idle
      if( DISPLAY_IsIBarVisible )
      { // No escalation exit conditions
        if( LE_SEQNCR_GetInfo( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, FALSE,
          &InfoGeneric, NULL, NULL, NULL, NULL ) )
        {// sequence is running
          if ( InfoGeneric.endTime > InfoGeneric.sequenceClock )
          {// Incoming anim is still running.
            break;
          }
        } else // we will wait for the sequence to start (presumably that would be the only way for getinfo to fail.
          break;
      }
      // If we get here, escalate
      LE_SEQNCR_Stop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority );

      if( lastCardIn < 16 )
        DISPLAY_state.ChanceCommChestDataID = LED_IFT( DAT_LANG2, CNK_cycad01 + lastCardIn );
      else
        DISPLAY_state.ChanceCommChestDataID = LED_IFT( DAT_LANG2, CNK_cyyd01 + lastCardIn - 16);
      if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA )
        LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, 0, LE_SEQNCR_DropDropFrames );
      else
        LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, DISPLAY_CARD_Y_OFFSET_NOT_MAIN, LE_SEQNCR_DropDropFrames );
      //LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, 0, LE_SEQNCR_DropDropFrames );
      LE_SEQNCR_SetEndingAction( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority,
        LE_SEQNCR_EndingActionStayAtEnd );
      DISPLAY_state.ChanceCommChestAnim++;
      break;

    case 3:   // Idle
      //if( DISPLAY_state.ChanceCommChestDesired == -1 || !DISPLAY_IsIBarVisible )
      if( DISPLAY_state.ChanceCommChestDesired != lastCardIn || !DISPLAY_IsIBarVisible )
      {// We don't want the card anymore.
        LE_SEQNCR_Stop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority );
        if( lastCardIn < 16 )
          DISPLAY_state.ChanceCommChestDataID = LED_IFT( DAT_LANG2, CNK_cycao01 + lastCardIn );
        else
          DISPLAY_state.ChanceCommChestDataID = LED_IFT( DAT_LANG2, CNK_cyyo01 + lastCardIn - 16);
        if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA )
          LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, 0, LE_SEQNCR_DropDropFrames );
        else
          LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, DISPLAY_CARD_Y_OFFSET_NOT_MAIN, LE_SEQNCR_DropDropFrames );
        //LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, 0, LE_SEQNCR_DropDropFrames );
        LE_SEQNCR_SetEndingAction( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority,
          LE_SEQNCR_EndingActionStop );
        DISPLAY_state.ChanceCommChestAnim++;
      }
      break;

    case 4:   // Outgoing.  If we ever care we can watch the outgoing anim, til then just close up shop.
      DISPLAY_state.ChanceCommChestDataID = LED_EI;
      DISPLAY_state.ChanceCommChestAnim = -1;
      break;
    }
  }
#else

  
    // Move card display along.
    switch ( DISPLAY_state.ChanceCommChestAnim )
    {
#define DISPLAY_CARD_Y_OFFSET_NOT_MAIN 136
    case -1:  // Off
      if( DISPLAY_state.ChanceCommChestDesired != -1 && DISPLAY_IsIBarVisible )
      {
          lastCardIn = DISPLAY_state.ChanceCommChestDesired;
        DISPLAY_state.ChanceCommChestAnim += 2;
      }
      break;

    case 0:
    case 1:   // Incoming card, monitor status then start animation
      
      DISPLAY_state.ChanceCommChestDataID = LED_IFT( DAT_LANG2, UDPENNY_GetCorrectCardCNK(lastCardIn));

      // Now we need to fire up the appropriate sound
      UDPENNY_PB_ReadCard( lastCardIn );
      if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA )
        LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, 0, LE_SEQNCR_DropDropFrames );
      else
        LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, DISPLAY_CARD_Y_OFFSET_NOT_MAIN, LE_SEQNCR_DropDropFrames );
      //LE_SEQNCR_StartXYDrop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, 0, 0, LE_SEQNCR_DropDropFrames );
      LE_SEQNCR_SetEndingAction( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority,
        LE_SEQNCR_EndingActionStayAtEnd );
      DISPLAY_state.ChanceCommChestAnim++;
      break;

    case 2:
        if( DISPLAY_IsIBarVisible )
      { // No escalation exit conditions
        if( LE_SEQNCR_GetInfo( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority, FALSE,
          &InfoGeneric, NULL, NULL, NULL, NULL ) )
        {// sequence is running
          if ( InfoGeneric.endTime > InfoGeneric.sequenceClock )
          {// Incoming anim is still running.
            break;
          }
        } else // we will wait for the sequence to start (presumably that would be the only way for getinfo to fail.
          break;
      }
      

      DISPLAY_state.ChanceCommChestAnim++;
      break;

    case 3:
        if( DISPLAY_state.ChanceCommChestDesired != lastCardIn || !DISPLAY_IsIBarVisible )
      {// We don't want the card anymore.
        DISPLAY_state.ChanceCommChestAnim++;
        // If we get here, escalate
        LE_SEQNCR_Stop( DISPLAY_state.ChanceCommChestDataID, DISPLAY_ChanceCommChestPriority );
      }
      break;

    case 4:   // Outgoing.  If we ever care we can watch the outgoing anim, til then just close up shop.
      DISPLAY_state.ChanceCommChestDataID = LED_EI;
      DISPLAY_state.ChanceCommChestAnim = -1;
      break;
    }
  }
#endif


  // Update the Property Display
  // First, construct the controlling property sets OPTIMIZE: May want to do this only when a change is expected
  PropFullColor = PropLowColor = PropFCMortgaged = PropLCMortgaged = 0;
  if( DISPLAY_IsPropertyBarAvailable )
  {// The property bar is displayed
    switch( DISPLAY_state.IBarCurrentState )
    {// Construct correct property set for IBar state
    case IBAR_STATE_DeedActive:
      // Just show the one...
      if( UICurrentGameState.Squares[DISPLAY_state.IBarDeedForDeedState].mortgaged )
        PropFCMortgaged = RULE_PropertyToBitSet( DISPLAY_state.IBarDeedForDeedState );
      else
        PropFullColor   = RULE_PropertyToBitSet( DISPLAY_state.IBarDeedForDeedState );
      break;
    case IBAR_STATE_Build:
      PropFullColor = DISPLAY_PlaceBuildingLegalSquares( player );
      break;
    case IBAR_STATE_Sell:
    case IBAR_STATE_HotelDecomposition:
      PropFullColor = DISPLAY_SellBuildingLegalSquares( player );
      break;
    case IBAR_STATE_Mortgage:
      for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
      { // Construct players standard property set
        if( player == UICurrentGameState.Squares[t].owner )
          if( UDIBAR_testMortgagable( t ) )
            PropFullColor |= RULE_PropertyToBitSet( t );
      }
      break;
    case IBAR_STATE_UnMortgage:
      for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
      { // Construct players standard property set
        if( (player == UICurrentGameState.Squares[t].owner ) &&
             UDIBAR_testUnMortgagable(t) )
        {
            PropFCMortgaged |= RULE_PropertyToBitSet( t );
        }
      }
      if( (DISPLAY_state.IBarLastRuleState == IBAR_STATE_FreeUnmortgage) &&
        (DISPLAY_state.IBarLastRulePlayer == player) )// Free Unmortgage is the real state, don't allow fee-free stuff to show up.
        PropFCMortgaged &= ~DISPLAY_state.TradeFreeUnmortgageSet;
      break;

    case IBAR_STATE_FreeUnmortgage:
      PropFCMortgaged = DISPLAY_state.TradeFreeUnmortgageSet;
      break;

    case IBAR_STATE_PlaceHouse:
    case IBAR_STATE_PlaceHotel:
      PropFullColor = DISPLAY_state.AuctionPlaceBuildingSet;
      break;

    case IBAR_STATE_Nothing: // This means no buttons, not no IBAR.  Show properties.
    default: // All regular plus mort/unmort
      for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
      {
        /* Construct players standard property set */
        if( player == UICurrentGameState.Squares[t].owner || (player == RULE_MAX_PLAYERS && UICurrentGameState.Squares[t].owner == RULE_NOBODY_PLAYER) )
        { // owned by player (or unowned and we are viewing the bank)
          if( UICurrentGameState.Squares[t].mortgaged )
            PropFCMortgaged |= RULE_PropertyToBitSet( t );
          else
            PropFullColor |= RULE_PropertyToBitSet( t );
        }
        else
        { // not owned by player
          if( UICurrentGameState.Squares[t].mortgaged )
            PropLCMortgaged |= RULE_PropertyToBitSet( t );
          else
            PropLowColor |= RULE_PropertyToBitSet( t );
        }
      }
      // .houses?
      if( DISPLAY_state.IBarCurrentState == IBAR_STATE_UnMortgage )
      {// Remove all but full color mortgaged.
          PropFullColor = PropLowColor = PropLCMortgaged = 0;
          // Remove any we don't have cash for
      }

      break;
    }
  }// Done constructing property sets.

  for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
  {// Take corrective action to change the display to the current property sets
    if( (ltemp = RULE_PropertyToBitSet( t )) == 0 )
      continue;
    itemp = DISPLAY_propertyToOwnablePropertyConversion( t );
#if USA_VERSION
    if     ( ltemp & PropFullColor )
      tempID = LED_IFT( DAT_MAIN, TAB_indsstc00 ) + itemp;
    else if( ltemp & PropFCMortgaged )
      //tempID = LED_IFT( DAT_MAIN, TAB_indsstc00 ) + itemp;
      tempID = LED_IFT( DAT_MAIN, TAB_indsstm00 ) + itemp;
    else if( ltemp & PropLowColor )
      tempID = LED_IFT( DAT_MAIN, TAB_indsstg00 ) + itemp;
    else if( ltemp & PropLCMortgaged )
      tempID = LED_IFT( DAT_MAIN, TAB_indsstg00 ) + itemp;
    else
      tempID = LED_EI;
#else
    if(iLangId == NOTW_LANG_FRENCH)
    {
        if     ( ltemp & PropFullColor )
          tempID = LED_IFT( DAT_PAT, TAB_xndsstc00 ) + itemp;
        else if( ltemp & PropFCMortgaged )
          tempID = LED_IFT( DAT_PAT, TAB_xndsstm00 ) + itemp;
        else if( ltemp & PropLowColor )
          tempID = LED_IFT( DAT_PAT, TAB_xndsstg00 ) + itemp;
        else if( ltemp & PropLCMortgaged )
          tempID = LED_IFT( DAT_PAT, TAB_xndsstg00 ) + itemp;
        else
          tempID = LED_EI;
    }
    else
    {
        if     ( ltemp & PropFullColor )
          tempID = LED_IFT( DAT_PAT, TAB_zndsstc00 ) + itemp;
        else if( ltemp & PropFCMortgaged )
          tempID = LED_IFT( DAT_PAT, TAB_zndsstm00 ) + itemp;
        else if( ltemp & PropLowColor )
          tempID = LED_IFT( DAT_PAT, TAB_zndsstg00 ) + itemp;
        else if( ltemp & PropLCMortgaged )
          tempID = LED_IFT( DAT_PAT, TAB_zndsstg00 ) + itemp;
        else
          tempID = LED_EI;
    }
#endif

    // Now we know what we want, lets make it so
    if(  (DISPLAY_state.IBarProperties[t] != tempID ) //||
       //( (DISPLAY_state.IBarPropertyCurrentMouseOver != DISPLAY_state.IBarPropertyLastMouseOver) &&
       // ((DISPLAY_state.IBarPropertyLastMouseOver == t) ||
       //  (DISPLAY_state.IBarPropertyCurrentMouseOver == t)) )
       )
    {// Need corrective action
      if( DISPLAY_state.IBarProperties[t] != LED_EI )
      {// Shut down old property
        LE_SEQNCR_Stop( DISPLAY_state.IBarProperties[t], DISPLAY_state.IBarPropertiesPriority[t] );
        DISPLAY_state.IBarProperties[t] = LED_EI;
      }

      DISPLAY_state.IBarProperties[t] = tempID;

      if( DISPLAY_state.IBarProperties[t] != LED_EI)
      {// Bring up new property
        barOrder = IBARPropertyBarOrder[itemp]; // Obtain display order in tray.
        DISPLAY_state.IBarPropertiesPriority[t] = DISPLAY_IBAR_GeneralPriority + (UNS16)(barOrder % 3);
        //LE_SEQNCR_StartXY( DISPLAY_state.IBarProperties[t], DISPLAY_state.IBarPropertiesPriority[t], x, y );
        LE_SEQNCR_StartXY( DISPLAY_state.IBarProperties[t], DISPLAY_state.IBarPropertiesPriority[t],
          IBAR_PropertyLocations[t].left,
          //(DISPLAY_state.IBarPropertyCurrentMouseOver == t) ?
          //IBAR_PropertyLocations[t].top + 1:
          IBAR_PropertyLocations[t].top );
      }
    }
  } // Done displaying properties.
  DISPLAY_state.IBarPropertyLastMouseOver = DISPLAY_state.IBarPropertyCurrentMouseOver;

  //Update the jail cards
  int houses = UICurrentGameState.GameOptions.maximumHouses, hotels = UICurrentGameState.GameOptions.maximumHotels; // Used for bank stats
  BOOL  forceChange = FALSE;
  //ACHAR myBuffer[40];
  int   Count;
  for( t = 0; t < 2; t++ )
  {
    if( (UICurrentGameState.Cards[t].jailOwner == player) &&
        (DISPLAY_IsPropertyBarAvailable) )
      //tempID = LED_IFT( DAT_MAIN, TAB_ibjlcdf0 ) + t;
      tempID = LED_IFT( DAT_LANG2, TAB_indsgoojc01 ) + t;
    else
      tempID = LED_EI;
    if( player == RULE_MAX_PLAYERS &&
      ( DISPLAY_IsPropertyBarAvailable) )
    {// Bank player, show remaining houses/hotels. FIXME - user objects, record & write the numbers.
      if( t == 0 )
      {

        for( int hcounter = 0; hcounter < SQ_MAX_SQUARE_TYPES; hcounter++)
        {
          Count = UICurrentGameState.Squares[hcounter].houses;
          if (Count < UICurrentGameState.GameOptions.housesPerHotel)
            houses -= Count;
          else
            hotels--;
        }

        if( houses != DISPLAY_state.IBarBankJailCardValuesPrinted[0] )
        { // Redraw house card.
          forceChange = TRUE;
          DISPLAY_state.IBarBankJailCardValuesPrinted[0] = houses;
          LE_GRAFIX_ShowTCB( DISPLAY_state.IBarBankJailCards[0], 0, 0,
            LED_IFT( DAT_MAIN, TAB_inhouse ) );
          Asprintf( myBuffer, A_T("%*i"), 3, houses );
          LE_FONTS_Print( DISPLAY_state.IBarBankJailCards[0],
            (LE_GRAFIX_ReturnObjectWidth( DISPLAY_state.IBarBankJailCards[0] ) - LE_FONTS_GetStringWidth(myBuffer))/2, 4,   // x,y
            LE_GRAFIX_MakeColorRef( 255, 255, 255 ), myBuffer );
        }
        tempID = DISPLAY_state.IBarBankJailCards[0];
      } else
      {
        if( hotels != DISPLAY_state.IBarBankJailCardValuesPrinted[1] )
        { // Redraw hotel card
          forceChange = TRUE;
          DISPLAY_state.IBarBankJailCardValuesPrinted[1] = hotels;
          LE_GRAFIX_ShowTCB( DISPLAY_state.IBarBankJailCards[1], 0, 0,
            LED_IFT( DAT_MAIN, TAB_inhotel ) );
          Asprintf( myBuffer, A_T("%*i"), 3, hotels );
          LE_FONTS_Print(DISPLAY_state.IBarBankJailCards[1],
            (LE_GRAFIX_ReturnObjectWidth( DISPLAY_state.IBarBankJailCards[1] ) - LE_FONTS_GetStringWidth(myBuffer))/2, 4,   // x,y
            LE_GRAFIX_MakeColorRef( 255, 255, 255 ), myBuffer );
        }
        tempID = DISPLAY_state.IBarBankJailCards[1];
      }
    }

    if( DISPLAY_state.IBarJailCards[t] != tempID || forceChange )
    {//Corrective action
      if( DISPLAY_state.IBarJailCards[t] != LED_EI )
        LE_SEQNCR_Stop( DISPLAY_state.IBarJailCards[t], DISPLAY_IBAR_GeneralPriority + t);
      DISPLAY_state.IBarJailCards[t] = tempID;
      if( DISPLAY_state.IBarJailCards[t] != LED_EI )
      {
        LE_SEQNCR_StartXY( DISPLAY_state.IBarJailCards[t], DISPLAY_IBAR_GeneralPriority + t,
          IBAR_JailCardLocations[t].left, IBAR_JailCardLocations[t].top );
      }
    }
  }

  // Property pop-ups (buy/auction)
  tempID = LED_EI;
  //if( DISPLAY_IsPropertyBarAvailable )
  if( DISPLAY_IsBoardVisible )
    tempID = DISPLAY_state.PropertyBuyAuctionDesired;
  if( tempID != DISPLAY_state.PropertyBuyAuctionShown )//test, demo)
  {// Buy Auction pop-up
    if( DISPLAY_state.PropertyBuyAuctionShown != LED_EI )
    {
      LE_SEQNCR_Stop( DISPLAY_state.PropertyBuyAuctionShown, DISPLAY_PropertyFoatingBlowUp - 1 );
      DISPLAY_state.PropertyBuyAuctionOnLeft = TRUE; // It blocks mouseovers while false.
    }

    DISPLAY_state.PropertyBuyAuctionShown = tempID;
    if( DISPLAY_state.PropertyBuyAuctionShown != LED_EI )
    {// Figure out if it should be on the left or right - assume the 3-square shot is active (a corner is not buy/auction)
      if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA || DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA )
      { // Trade screen
        LE_SEQNCR_StartXY( DISPLAY_state.PropertyBuyAuctionShown, DISPLAY_PropertyFoatingBlowUp - 1, DISPLAY_BuyAuctProp_OffsetXTrade, DISPLAY_BuyAuctProp_OffsetY );
        DISPLAY_state.PropertyBuyAuctionOnLeft = FALSE;
      } else  if( (UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].currentSquare%10) %3 != 0 )

      { // on left Main
        LE_SEQNCR_StartXY( DISPLAY_state.PropertyBuyAuctionShown, DISPLAY_PropertyFoatingBlowUp - 1, DISPLAY_BuyAuctProp_OffsetXLeft, DISPLAY_BuyAuctProp_OffsetY );
        DISPLAY_state.PropertyBuyAuctionOnLeft = TRUE;
      } else

      { // on right Main
        LE_SEQNCR_StartXY( DISPLAY_state.PropertyBuyAuctionShown, DISPLAY_PropertyFoatingBlowUp - 1, DISPLAY_BuyAuctProp_OffsetXRight, DISPLAY_BuyAuctProp_OffsetY );
        DISPLAY_state.PropertyBuyAuctionOnLeft = FALSE;
      }
    }
  }

  // Property pop-ups (trade)
  tempID = LED_EI;
  //Check for Mouseover Property, update or remove blow-up as appropriate.
  if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA)
  {// Eligible for a floating property blow up
    if( DISPLAY_state.IBarTradePropertyCurrentMouseOver >= 0 )
    {// Mouse over and a property is there (visible)
      if( DISPLAY_state.IBarTradePropertyCurrentMouseOver == DISPLAY_state.IBarTradePropertyCurrentMouseOverCheck )
      {
        // First, leave a time delay before bringing up a new property to avoid flutter.  Supress it if a buy/auction deed is on the right.
        if (LE_TIME_TickCount - DISPLAY_state.IBarTradePropertyCurrentMouseOverTickCount > DISPLAY_MouseOverPopUpDelay )// && DISPLAY_state.PropertyBuyAuctionOnLeft )
        {
          if( UICurrentGameState.Squares[DISPLAY_state.IBarTradePropertyCurrentMouseOver % 100].mortgaged )
#if USA_VERSION
            tempID = LED_IFT( DAT_LANG2, TAB_iyb00x00 ) + DISPLAY_propertyToOwnablePropertyConversion( DISPLAY_state.IBarTradePropertyCurrentMouseOver % 100 )
            + DISPLAY_DEEDS_PER_SET*max(DISPLAY_state.city, 0);
#else
            tempID = UDIBAR_Deeds[28 + DISPLAY_propertyToOwnablePropertyConversion( DISPLAY_state.IBarTradePropertyCurrentMouseOver % 100 )];
#endif
          else
#if USA_VERSION
            tempID = LED_IFT( DAT_LANG2, TAB_iyf00x00 ) + DISPLAY_propertyToOwnablePropertyConversion( DISPLAY_state.IBarTradePropertyCurrentMouseOver % 100 )
            + DISPLAY_DEEDS_PER_SET*max(DISPLAY_state.city, 0);
#else
            tempID = UDIBAR_Deeds[DISPLAY_propertyToOwnablePropertyConversion( DISPLAY_state.IBarTradePropertyCurrentMouseOver % 100 )];
#endif
        }
      } else
      {// Track the property & time.
        if( DISPLAY_state.IBarTradePropertyCurrentMouseOverCheck == -1 )// We aren't just moving between properties
          DISPLAY_state.IBarTradePropertyCurrentMouseOverTickCount = LE_TIME_TickCount;
        DISPLAY_state.IBarTradePropertyCurrentMouseOverCheck = DISPLAY_state.IBarTradePropertyCurrentMouseOver;
      }
    } else
      DISPLAY_state.IBarTradePropertyCurrentMouseOverCheck = -1; // Record that we have nothing (there will be a delay for the same property next time)
  } else
  {// Reset the indicators here - mousemoves wont get processed to reset this.
    DISPLAY_state.IBarTradePropertyCurrentMouseOver = -1;
    DISPLAY_state.IBarTradePropertyCurrentMouseOverCheck = -1;
  }
  if( tempID != DISPLAY_state.IBarTradePropertyBlownUp )
  {// Adjust property blow up
    if( DISPLAY_state.IBarTradePropertyBlownUp != LED_EI )
      LE_SEQNCR_Stop( DISPLAY_state.IBarTradePropertyBlownUp, DISPLAY_PropertyFoatingBlowUp - 2);
    DISPLAY_state.IBarTradePropertyBlownUp = tempID;
    if( DISPLAY_state.IBarTradePropertyBlownUp != LED_EI )
    {
      LE_SEQNCR_StartXY( DISPLAY_state.IBarTradePropertyBlownUp, DISPLAY_PropertyFoatingBlowUp - 2,
        (600 * (DISPLAY_state.IBarTradePropertyCurrentMouseOver >= 1000)), -2 );
    }
  }

  // Property pop-ups (mouseover)
  tempID = LED_EI;
  //Check for Mouseover Property, update or remove blow-up as appropriate.
  if( DISPLAY_IsPropertyBarAvailable )
  {// Eligible for a floating property blow up
    if( DISPLAY_state.IBarPropertyCurrentMouseOver >= 0 )
    {// Mouse over and a property is there (visible)
      if( DISPLAY_state.IBarPropertyCurrentMouseOver == DISPLAY_state.IBarPropertyCurrentMouseOverCheck )
      {
        // First, leave a time delay before bringing up a new property to avoid flutter.
        if( (LE_TIME_TickCount - DISPLAY_state.IBarPropertyCurrentMouseOverTickCount > DISPLAY_MouseOverPopUpDelay) &&
          (PropFullColor | PropFCMortgaged) & RULE_PropertyToBitSet( DISPLAY_state.IBarPropertyCurrentMouseOver ) )
        {
          if( UICurrentGameState.Squares[DISPLAY_state.IBarPropertyCurrentMouseOver].mortgaged )
#if USA_VERSION
            tempID = LED_IFT( DAT_LANG2, TAB_iyb00x00 ) + DISPLAY_propertyToOwnablePropertyConversion( DISPLAY_state.IBarPropertyCurrentMouseOver )
            + DISPLAY_DEEDS_PER_SET*max(DISPLAY_state.city, 0);
#else
            tempID = UDIBAR_Deeds[28 + DISPLAY_propertyToOwnablePropertyConversion( DISPLAY_state.IBarPropertyCurrentMouseOver )];
#endif
          else
#if USA_VERSION
            tempID = LED_IFT( DAT_LANG2, TAB_iyf00x00 ) + DISPLAY_propertyToOwnablePropertyConversion( DISPLAY_state.IBarPropertyCurrentMouseOver )
            + DISPLAY_DEEDS_PER_SET*max(DISPLAY_state.city, 0);
#else
            tempID = UDIBAR_Deeds[DISPLAY_propertyToOwnablePropertyConversion( DISPLAY_state.IBarPropertyCurrentMouseOver )];
#endif
        }
      } else
      {// Track the property & time.
        if( DISPLAY_state.IBarPropertyCurrentMouseOverCheck == -1 )// We aren't just moving between properties
          DISPLAY_state.IBarPropertyCurrentMouseOverTickCount = LE_TIME_TickCount;
        DISPLAY_state.IBarPropertyCurrentMouseOverCheck = DISPLAY_state.IBarPropertyCurrentMouseOver;
      }
    } else
      DISPLAY_state.IBarPropertyCurrentMouseOverCheck = -1; // Record that we have nothing (there will be a delay for the same property next time)
  } else
  {// Reset the indicators here - mousemoves wont get processed to reset this.
    DISPLAY_state.IBarPropertyCurrentMouseOver = -1;
    DISPLAY_state.IBarPropertyCurrentMouseOverCheck = -1;
  }
  if( tempID != DISPLAY_state.IBarPropertyBlownUp )
  {// Adjust property blow up
    if( DISPLAY_state.IBarPropertyBlownUp != LED_EI )
      LE_SEQNCR_Stop( DISPLAY_state.IBarPropertyBlownUp, DISPLAY_PropertyFoatingBlowUp );
    DISPLAY_state.IBarPropertyBlownUp = tempID;
    if( DISPLAY_state.IBarPropertyBlownUp != LED_EI )
    {
      LE_SEQNCR_StartXY( DISPLAY_state.IBarPropertyBlownUp, DISPLAY_PropertyFoatingBlowUp,
        DISPLAY_MouseOver_OffsetX, DISPLAY_MouseOver_OffsetY );
    }
  }

  // Update the status(score) bar
  for( t = 0; t < RULE_MAX_PLAYERS; t++ )
  {
    BOOL scoreReprinted = FALSE; // Flag - easiest way to know whether to invalidate the box.

    // In certain situations only show a subset of all players.  eg. Remove player should 
    // only allow clicks on the bar for local players.  Keep their positions the
    // same as if all were visible, its just easier.
    if ( DISPLAY_state.ShowOnlyLocalPlayersOnIBar && !SlotIsALocalPlayer[t] )
      DISPLAY_state.IsPlayerVisible[t] = FALSE;
    else if ( DISPLAY_state.ShowOnlyLocalAIPlayersOnIBar && !SlotIsALocalAIPlayer[t] ) 
      DISPLAY_state.IsPlayerVisible[t] = FALSE;
    else
      DISPLAY_state.IsPlayerVisible[t] = TRUE;

    // If the player is used && shown, update the object
    if ( (t < UICurrentGameState.NumberOfPlayers)
     && DISPLAY_state.IsPlayerVisible[t]
     && ((DISPLAY_state.desired2DView == DISPLAY_SCREEN_Pselect) || (DISPLAY_state.desired2DView == DISPLAY_SCREEN_PselectRules)
      || DISPLAY_IsIBarVisible) )
    {
      // Update the players token & color (don't record X changes yet)
      if ( UICurrentGameState.NumberOfPlayers <= 4 )
        x = UDIBAR_Calc_ScoreX( t, UICurrentGameState.NumberOfPlayers, UDIBAR_ScoreBoxLargeWidth );
      else
        x = UDIBAR_Calc_ScoreX( t, UICurrentGameState.NumberOfPlayers, UDIBAR_ScoreBoxSmallWidth );

      tempID = LED_IFT( DAT_MAIN, TAB_inpsa) + UICurrentGameState.Players[t].token;
      // Color
      if( UICurrentGameState.NumberOfPlayers <= 4 )
        tempID2 = LED_IFT( DAT_MAIN, TAB_inpsl0 ) + UICurrentGameState.Players[t].colour;
      else
        tempID2 = LED_IFT( DAT_MAIN, TAB_inpss0 ) + UICurrentGameState.Players[t].colour;
      btemp = UICurrentGameState.Players[t].currentSquare == 40 && GameInProgress;// The square isn't updated til later - bars stayed up.

      tokenChange = FALSE;
      if ( (DISPLAY_state.PlayerTokens[t] != tempID  ) ||  // Image change
        (DISPLAY_state.PlayerTokenJailUp[t] != btemp ) ||  // jail status changed
        (tempID2 != DISPLAY_state.PlayerColorBarShownID[t]) ||
        (DISPLAY_state.PlayerScoreBoxesLocX[t] != x) )     // Location Change
      {
        tokenChange = TRUE;
      }
      if(  tokenChange ||
         ( (DISPLAY_state.IBarPlayerCurrentMouseOver != DISPLAY_state.IBarPlayerLastMouseOver) &&
          ((DISPLAY_state.IBarPlayerLastMouseOver    == t) ||    // Or mouseover changed it.
           (DISPLAY_state.IBarPlayerCurrentMouseOver == t)) ) )
      {// Corrective action required (token or color or jail - do em all.)
        if( DISPLAY_state.PlayerColorBarShownID[t] != LED_EI )
        {
          LE_SEQNCR_Stop( DISPLAY_state.PlayerColorBarShownID[t], DISPLAY_IBAR_GeneralPriority + 1 + t);
          DISPLAY_state.PlayerColorBarShownID[t] = LED_EI;
        }
        if( DISPLAY_state.PlayerTokens[t] != LED_EI )
        {
          LE_SEQNCR_Stop( DISPLAY_state.PlayerTokens[t], DISPLAY_IBARScoreBoxPriority + t );
          DISPLAY_state.PlayerTokens[t] = LED_EI;
        }
        if( tempID != LED_EI )// we know it is...
        {// Token Fire it up - use x not PlayerScoreBoxesLocX which is not up to date yet.
          DISPLAY_state.PlayerTokens[t] = tempID;
          LE_SEQNCR_StartXY( DISPLAY_state.PlayerTokens[t], DISPLAY_IBARScoreBoxPriority + t,
            x + 5, (DISPLAY_state.IBarPlayerCurrentMouseOver == t) ?
            DISPLAY_ScoreY + 5 + 1: DISPLAY_ScoreY + 5);
        }
        // Color, fire it up
        DISPLAY_state.PlayerColorBarShownID[t] = tempID2;
        LE_SEQNCR_StartXY( DISPLAY_state.PlayerColorBarShownID[t], DISPLAY_IBAR_GeneralPriority + 1 + t,
         x, (DISPLAY_state.IBarPlayerCurrentMouseOver == t) ?
         DISPLAY_ScoreY + 1: DISPLAY_ScoreY );

        // Jail bars
        DISPLAY_state.PlayerTokenJailUp[t] = FALSE;
        LE_SEQNCR_Stop( LED_IFT( DAT_MAIN, TAB_inpbj ), DISPLAY_IBARScoreBoxPriority + 1 + t );
        if( btemp )
        {// be sure they are on
          if( !DISPLAY_state.PlayerTokenJailUp[t] )
          {
            DISPLAY_state.PlayerTokenJailUp[t] = TRUE;
            LE_SEQNCR_StartXY( LED_IFT( DAT_MAIN, TAB_inpbj ), DISPLAY_IBARScoreBoxPriority + 1 + t,
              x + 1, (DISPLAY_state.IBarPlayerCurrentMouseOver == t) ?
              DISPLAY_ScoreY - 1 + 1: DISPLAY_ScoreY - 1);
          }
        }
      }

      // Update the score object
      if ( tokenChange || (UICurrentGameState.Players[t].cash != DISPLAY_state.LastPlayerScoresPrinted[t]) ||
        ( 0 != Astrcmp( UICurrentGameState.Players[t].name, DISPLAY_state.namePrinted[t])) )
      {// Note: When adding players, the number goes up before the player is defined (color wrong)

        // Set new cash value for display.
        if( DISPLAY_state.LastPlayerScoresPrinted[t] > UICurrentGameState.Players[t].cash && DISPLAY_state.LastPlayerScoreUpdateTime[t] <= LE_TIME_TickCount - 20 ) // 60ths of a second per bill.
        {
          DISPLAY_state.LastPlayerScoreUpdateTime[t] = LE_TIME_TickCount;
          /*if( DISPLAY_state.LastPlayerScoresPrinted[t] >= UICurrentGameState.Players[t].cash + 500 )
            DISPLAY_state.LastPlayerScoresPrinted[t] -= 500;
          else if( DISPLAY_state.LastPlayerScoresPrinted[t] >= UICurrentGameState.Players[t].cash + 100 )
            DISPLAY_state.LastPlayerScoresPrinted[t] -= 100;
          else if( DISPLAY_state.LastPlayerScoresPrinted[t] >= UICurrentGameState.Players[t].cash + 50 )
            DISPLAY_state.LastPlayerScoresPrinted[t] -= 50;
          else if( DISPLAY_state.LastPlayerScoresPrinted[t] >= UICurrentGameState.Players[t].cash + 20 )
            DISPLAY_state.LastPlayerScoresPrinted[t] -= 20;
          else if( DISPLAY_state.LastPlayerScoresPrinted[t] >= UICurrentGameState.Players[t].cash + 10 )
            DISPLAY_state.LastPlayerScoresPrinted[t] -= 10;
          else if( DISPLAY_state.LastPlayerScoresPrinted[t] >= UICurrentGameState.Players[t].cash + 5 )
            DISPLAY_state.LastPlayerScoresPrinted[t] -= 5;
          else if( DISPLAY_state.LastPlayerScoresPrinted[t] >= UICurrentGameState.Players[t].cash + 1 )
            DISPLAY_state.LastPlayerScoresPrinted[t] -= 1;*/
          DISPLAY_state.LastPlayerScoresPrinted[t] = UICurrentGameState.Players[t].cash;
          UDSOUND_CashDownFx();
        }
        if( DISPLAY_state.LastPlayerScoresPrinted[t] < UICurrentGameState.Players[t].cash && DISPLAY_state.LastPlayerScoreUpdateTime[t] <= LE_TIME_TickCount - 20 ) // 60ths of a second per bill.
        {
          /*DISPLAY_state.LastPlayerScoreUpdateTime[t] = LE_TIME_TickCount;
          if( DISPLAY_state.LastPlayerScoresPrinted[t] <= UICurrentGameState.Players[t].cash - 500 )
            DISPLAY_state.LastPlayerScoresPrinted[t] += 500;
          else if( DISPLAY_state.LastPlayerScoresPrinted[t] <= UICurrentGameState.Players[t].cash - 100 )
            DISPLAY_state.LastPlayerScoresPrinted[t] += 100;
          else if( DISPLAY_state.LastPlayerScoresPrinted[t] <= UICurrentGameState.Players[t].cash - 50 )
            DISPLAY_state.LastPlayerScoresPrinted[t] += 50;
          else if( DISPLAY_state.LastPlayerScoresPrinted[t] <= UICurrentGameState.Players[t].cash - 20 )
            DISPLAY_state.LastPlayerScoresPrinted[t] += 20;
          else if( DISPLAY_state.LastPlayerScoresPrinted[t] <= UICurrentGameState.Players[t].cash - 10 )
            DISPLAY_state.LastPlayerScoresPrinted[t] += 10;
          else if( DISPLAY_state.LastPlayerScoresPrinted[t] <= UICurrentGameState.Players[t].cash - 5 )
            DISPLAY_state.LastPlayerScoresPrinted[t] += 5;
          else if( DISPLAY_state.LastPlayerScoresPrinted[t] <= UICurrentGameState.Players[t].cash - 1 )
            DISPLAY_state.LastPlayerScoresPrinted[t] += 1;*/
          DISPLAY_state.LastPlayerScoresPrinted[t] = UICurrentGameState.Players[t].cash;
          UDSOUND_CashUpFx();
        }
        scoreReprinted = TRUE;

        // Remember w,h of display object
        playerWidth = LE_GRAFIX_ReturnObjectWidth( DISPLAY_state.PlayerColorBarShownID[t] );
        playerHeight = LE_GRAFIX_ReturnObjectHeight( DISPLAY_state.PlayerColorBarShownID[t] );

        // Clear box first.  (Use true (large) w,h for erasing.)
        LE_GRAFIX_ColorArea( DISPLAY_state.PlayerScoreBoxes[t], 0, 0,
         LE_GRAFIX_ReturnObjectWidth(DISPLAY_state.PlayerScoreBoxes[t]),
         LE_GRAFIX_ReturnObjectHeight(DISPLAY_state.PlayerScoreBoxes[t]),
         LE_GRAFIX_MakeColorRef( 0, 255, 0 ) );

        LE_FONTS_SetUnderline( FALSE );
        LE_FONTS_SetItalic( FALSE );
        if ( UICurrentGameState.NumberOfPlayers <= 4 )
        {
          LE_FONTS_SetSize( 11 );
          LE_FONTS_SetWeight( 600 );
        } else
        {
          LE_FONTS_SetSize( 7 );
          LE_FONTS_SetWeight( 400 );
        }
        // Print name left justified to graphic.
        LANG_Print( DISPLAY_state.PlayerScoreBoxes[t],
         //playerWidth - LANG_GetStringWidth(UICurrentGameState.Players[t].name) - 5, 4,  // x,y
         57, 5,  // x,y FIXME - better way to find name start coord?  prints under token if to far left.
         LE_GRAFIX_MakeColorRef( 0, 0, 0 ), UICurrentGameState.Players[t].name );
        Astrcpy( DISPLAY_state.namePrinted[t], UICurrentGameState.Players[t].name );

        // Print cash right justified to graphic.
        //if( DISPLAY_IsIBarVisible )
        //{// Dont put the score during pselect - trust the cash change to reprint when the game starts.
          UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(DISPLAY_state.LastPlayerScoresPrinted[t], 
            DISPLAY_state.system, myBuffer, TRUE);
          if ( UICurrentGameState.NumberOfPlayers <= 4 )
          {// These will probably need tweaking for specific currencies.
            if( DISPLAY_IsWideCurrency )
            // Big numbers mean smaller font
              LE_FONTS_SetSize( 10 );
            else
              LE_FONTS_SetSize( 11 );
            LE_FONTS_SetWeight( 600 );
          } else
          {
            if( DISPLAY_IsWideCurrency )
              LE_FONTS_SetSize( 8 ); // Big numbers
            else
              LE_FONTS_SetSize( 8 );
            LE_FONTS_SetWeight( 400 );
          }
          LE_FONTS_Print(DISPLAY_state.PlayerScoreBoxes[t],
            playerWidth - LE_FONTS_GetStringWidth(myBuffer) - 7, 18,   // x,y
            LE_GRAFIX_MakeColorRef( 0, 0, 0 ), myBuffer );
        //}
      }

      // Update the score display (record LocX changes here)
      if ( !DISPLAY_state.IsPlayerDisplayed[t] )
      { // Fire it up
        DISPLAY_state.PlayerScoreBoxesLocX[t] = x;
        LE_SEQNCR_StartXY( DISPLAY_state.PlayerScoreBoxes[t],
          DISPLAY_IBARScoreBoxPriority,
          DISPLAY_state.PlayerScoreBoxesLocX[t],
          (DISPLAY_state.IBarPlayerCurrentMouseOver == t) ? DISPLAY_ScoreY + 1 : DISPLAY_ScoreY );
        DISPLAY_state.IsPlayerDisplayed[t] = TRUE;
      }
      else // Just invalidate the box
      {
        if ( (DISPLAY_state.IBarPlayerCurrentMouseOver == DISPLAY_state.IBarPlayerLastMouseOver)
         && (DISPLAY_state.PlayerScoreBoxesLocX[t] == x) )
        {// Didn't move
          if( scoreReprinted ) // need to update it anyway.
            LE_SEQNCR_ForceRedraw( DISPLAY_state.PlayerScoreBoxes[t], DISPLAY_IBARScoreBoxPriority );
        } else
        {// Moved.  Move it and record.
          DISPLAY_state.PlayerScoreBoxesLocX[t] = x;
          LE_SEQNCR_MoveXY( DISPLAY_state.PlayerScoreBoxes[t],
            DISPLAY_IBARScoreBoxPriority,
            DISPLAY_state.PlayerScoreBoxesLocX[t],
            (DISPLAY_state.IBarPlayerCurrentMouseOver == t) ? DISPLAY_ScoreY + 1 : DISPLAY_ScoreY );
        }
      }
    }
    else
    {
      // Turn off the score object, token & color
      if( DISPLAY_state.PlayerColorBarShownID[t] != LED_EI )
      {
        LE_SEQNCR_Stop( DISPLAY_state.PlayerColorBarShownID[t], DISPLAY_IBAR_GeneralPriority + 1 + t);
        DISPLAY_state.PlayerColorBarShownID[t] = LED_EI;
      }
      if( DISPLAY_state.PlayerTokens[t] != LED_EI )
      {
        LE_SEQNCR_Stop( DISPLAY_state.PlayerTokens[t], DISPLAY_IBARScoreBoxPriority + t );
        DISPLAY_state.PlayerTokens[t] = LED_EI;
      }
      if( DISPLAY_state.IsPlayerDisplayed[t] )
      {
        LE_SEQNCR_Stop( DISPLAY_state.PlayerScoreBoxes[t],
          DISPLAY_IBARScoreBoxPriority );
        DISPLAY_state.IsPlayerDisplayed[t] = FALSE;
      }
      // Jail bars
      if( DISPLAY_state.PlayerTokenJailUp[t] )
      {
        DISPLAY_state.PlayerTokenJailUp[t] = FALSE;
        LE_SEQNCR_Stop( LED_IFT( DAT_MAIN, TAB_inpbj ), DISPLAY_IBARScoreBoxPriority + 1 + t );
      }
    }
  }// Done updating score bar.
  //Add the bank token
  if( (DISPLAY_IsIBarVisible) ^ DISPLAY_state.IsBankDisplayed )
  {
    if ( DISPLAY_state.IsBankDisplayed )
    {
      LE_SEQNCR_Stop( LED_IFT( DAT_LANG2, TAB_bank ), DISPLAY_IBAR_GeneralPriority );
    }
    else
    {
      // Assign the x position for the bank button.
      DISPLAY_state.PlayerScoreBoxesLocX[RULE_MAX_PLAYERS] = LE_GRAFIX_VirtualScreenWidthInPixels
       - DISPLAY_IBAR_BankWidth;
      DISPLAY_state.IsBankMouseOver = FALSE;
      // Show the bank button
      LE_SEQNCR_StartXY( LED_IFT( DAT_LANG2, TAB_bank ), DISPLAY_IBAR_GeneralPriority,
        DISPLAY_state.PlayerScoreBoxesLocX[RULE_MAX_PLAYERS], DISPLAY_ScoreY );
    }
    DISPLAY_state.IsBankDisplayed = !DISPLAY_state.IsBankDisplayed;
  }
  // Special section for mouseover the bank icon
  if( DISPLAY_state.IsBankDisplayed &&
    ( DISPLAY_state.IsBankMouseOver != ( DISPLAY_state.IBarPlayerCurrentMouseOver == RULE_MAX_PLAYERS ) ))
  {// Update the mouseover status
    DISPLAY_state.IsBankMouseOver = !DISPLAY_state.IsBankMouseOver;
    LE_SEQNCR_MoveXY( LED_IFT( DAT_LANG2, TAB_bank ), DISPLAY_IBAR_GeneralPriority,
        DISPLAY_state.PlayerScoreBoxesLocX[RULE_MAX_PLAYERS], DISPLAY_ScoreY + DISPLAY_state.IsBankMouseOver );
  }

  DISPLAY_state.IBarPlayerLastMouseOver = DISPLAY_state.IBarPlayerCurrentMouseOver;


  // Update the current player in control token/color/name
  if( (DISPLAY_IsIBarVisible) &&
      (UICurrentGameState.CurrentPlayer >= 0) &&
      (UICurrentGameState.CurrentPlayer < UICurrentGameState.NumberOfPlayers) )
  {
    tempID  = LED_IFT( DAT_MAIN, CNK_indstra ) + UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].token;
    tempID1 = LED_IFT( DAT_MAIN, TAB_indsturn0 ) + UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].colour;
    tempID2 = DISPLAY_state.CurrentPlayerNameTags[UICurrentGameState.CurrentPlayer];
  }
  else
    tempID = tempID1 = tempID2 = LED_EI;

  if( DISPLAY_state.CurrentPlayerShownID != tempID )
  {// Needs to be updated
    if( DISPLAY_state.CurrentPlayerShownID != LED_EI )
      LE_SEQNCR_Stop( DISPLAY_state.CurrentPlayerShownID, DISPLAY_IBAR_GeneralPriority + 2 );
    DISPLAY_state.CurrentPlayerShownID = tempID;
    if( DISPLAY_state.CurrentPlayerShownID != LED_EI )
    {// New token
      LE_SEQNCR_StartXY( DISPLAY_state.CurrentPlayerShownID, DISPLAY_IBAR_GeneralPriority + 2, 0, -4 );
      LE_SEQNCR_SetEndingAction( DISPLAY_state.CurrentPlayerShownID, DISPLAY_IBAR_GeneralPriority + 2,
        LE_SEQNCR_EndingActionLoopToBeginning );
    }
  }
/*  if( DISPLAY_state.CurrentPlayerColorID != tempID1 )
  {// Needs to be updated
    if( DISPLAY_state.CurrentPlayerColorID != LED_EI )
      LE_SEQNCR_Stop( DISPLAY_state.CurrentPlayerColorID, DISPLAY_IBAR_GeneralPriority + 1 );
    DISPLAY_state.CurrentPlayerColorID = tempID1;
    if( DISPLAY_state.CurrentPlayerColorID != LED_EI )
    {// New color bar
      LE_SEQNCR_StartXY( DISPLAY_state.CurrentPlayerColorID, DISPLAY_IBAR_GeneralPriority + 1,
        UDIBAR_CurrentPlayerNameLeft, UDIBAR_CurrentPlayerNameTop );
      LE_SEQNCR_SetEndingAction( DISPLAY_state.CurrentPlayerColorID, DISPLAY_IBAR_GeneralPriority + 1,
        LE_SEQNCR_EndingActionLoopToBeginning );
    }
  }*/
  if( DISPLAY_state.CurrentPlayerNameTag != tempID2 ||
    wcscmp( DISPLAY_state.CurrentPlayerNamesDrawn[UICurrentGameState.CurrentPlayer], UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].name) != 0 )
  {// Needs to be updated
    if( DISPLAY_state.CurrentPlayerNameTag != LED_EI )
      LE_SEQNCR_Stop( DISPLAY_state.CurrentPlayerNameTag, DISPLAY_IBAR_GeneralPriority + 2 );
    DISPLAY_state.CurrentPlayerNameTag = tempID2;
    if( DISPLAY_state.CurrentPlayerNameTag != LED_EI )
    {// New nametag
    // Update the main game current player box as well
      if( wcscmp( DISPLAY_state.CurrentPlayerNamesDrawn[UICurrentGameState.CurrentPlayer], UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].name) != 0 )
      {
        LE_GRAFIX_ColorArea( DISPLAY_state.CurrentPlayerNameTags[UICurrentGameState.CurrentPlayer], 0, 0,
          LE_GRAFIX_ReturnObjectWidth ( DISPLAY_state.CurrentPlayerNameTags[0] ),
          LE_GRAFIX_ReturnObjectHeight( DISPLAY_state.CurrentPlayerNameTags[0] ), LEG_MCR(0, 255, 0));
        //LE_GRAFIX_ShowTCB( DISPLAY_state.CurrentPlayerNameTags[UICurrentGameState.CurrentPlayer], 0, 0,
        //  LED_IFT( DAT_MAIN, TAB_indsturn0 ) + UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].colour ); // Color backdrop
//        LANG_Print(DISPLAY_state.CurrentPlayerNameTags[UICurrentGameState.CurrentPlayer], 8, 0,
//          LE_GRAFIX_MakeColorRef( 0, 0, 0 ), UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].name );
        LANG_Print(DISPLAY_state.CurrentPlayerNameTags[UICurrentGameState.CurrentPlayer],
          max(0, (140-LANG_GetStringWidth(UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].name))/2), 0,
          LE_GRAFIX_MakeColorRef( 0, 0, 0 ), UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].name );
        wcscpy( DISPLAY_state.CurrentPlayerNamesDrawn[UICurrentGameState.CurrentPlayer], UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].name );
      };
      LE_SEQNCR_StartXY( DISPLAY_state.CurrentPlayerNameTag, DISPLAY_IBAR_GeneralPriority + 2,
        UDIBAR_CurrentPlayerNameLeft + 4, UDIBAR_CurrentPlayerNameTop );
    }
  }


    // this section is dedicated to the display of the player status bar
    if((DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA) && (!g_bStatusBarDisplayed))
    {
        // display the title bar for category
        LE_SEQNCR_Start(CategoryBarID, BarPriority);
		LE_SEQNCR_Start(CategoryBarID1, BarPriority+100);

        UDIBar_DisplaySortButtons();

        for(int i=0; i<3; i++)
        {
            if(i == last_category_result)
            {
                if(CategoryButton[i].State == ISTATBAR_BUTTON_Off)
                {
                    LE_SEQNCR_Start(CategoryButton[i].PressID, CButtonPriority[i]);
                    CategoryButton[i].State = ISTATBAR_BUTTON_Press;
                }
            }
            else
            {
                if(CategoryButton[i].State == ISTATBAR_BUTTON_Off)
                {
                    LE_SEQNCR_Start(CategoryButton[i].IdleID, CButtonIdlePriority[i]);
                    CategoryButton[i].State = ISTATBAR_BUTTON_Idle;
                }
            }

        }

        g_bStatusBarDisplayed = TRUE;
        g_bIsScreenUpdated = FALSE;
        g_Button = last_category_result;

    }
    else if((DISPLAY_state.desired2DView != DISPLAY_SCREEN_PortfolioA) && (g_bStatusBarDisplayed))
    {
        // remove the title bar for category
        LE_SEQNCR_Stop(CategoryBarID, BarPriority);
		LE_SEQNCR_Stop(CategoryBarID1, BarPriority+100);

        UDIBar_RemoveSortButtons();

        for(int i=0; i<3; i++)
        {
            switch(CategoryButton[i].State)
            {
            case ISTATBAR_BUTTON_Idle:      LE_SEQNCR_Stop(CategoryButton[i].IdleID, CButtonIdlePriority[i]); break;
            case ISTATBAR_BUTTON_Press:     LE_SEQNCR_Stop(CategoryButton[i].PressID, CButtonPriority[i]); break;
            case ISTATBAR_BUTTON_Return:    LE_SEQNCR_Stop(CategoryButton[i].ReturnID, CButtonIdlePriority[i]); break;
            default: break;
            }
        CategoryButton[i].State = ISTATBAR_BUTTON_Off;
        }

        g_bStatusBarDisplayed = FALSE;
    }


	// this section is dedicated to the display of the options button
    if((!DISPLAY_IsIBarVisible) && (!g_bOptionsButtonsDisplayed)
		&& g_bOptionsButtonsOn)
    {
        // display the options buttons
		UDIBar_DisplayOptionsButtons();
        g_bOptionsButtonsDisplayed = TRUE;

		// huge hack to get the file button to press itself when OPTIONS is clicked
		if(last_playerselectscreen > DISPLAY_SCREEN_Options)
		{
            if(g_UserChoseToExit)
                UDIBar_ProcessOptionsButtonPress(CREDITS_SCREEN);
            else
                UDIBar_ProcessOptionsButtonPress(FILE_SCREEN);
        }
    }
    else if(DISPLAY_IsIBarVisible && (g_bOptionsButtonsDisplayed))
    {
		// remove the options buttons
        UDIBar_RemoveOptionsButtons();
        g_bOptionsButtonsDisplayed = FALSE;
    }
}


/*****************************************************************************
 * Standard UI subfunction - Process library message.
 */

void UDIBAR_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr )
{
  register int t;
  int ButtonHit, PropertyHit, PlayerOrBankHit;
  LE_QUEUE_MessageRecord UIMessageRecord;

  // Make a copy of the message for our own use.  If we modify the original the next
  // ProcessMessage routine will be affected.
  memcpy( &UIMessageRecord, UIMessagePntr, sizeof(UIMessageRecord) );
  UIMessagePntr = &UIMessageRecord;

  if( UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN )
  {// Reset demo mode 
    DISPLAY_state.LE_TickCount_Last_Button_Click = LE_TIME_TickCount;
  }

  // Check for esc key press
  if( UIMessagePntr->messageCode == UIMSG_KEYBOARD_PRESSED )
  { // Now we handle ESC key - quit current game/quit application
    DISPLAY_state.LE_TickCount_Last_Button_Click = LE_TIME_TickCount;
    if( (UIMessagePntr->numberA == LE_KEYBRD_ESC) && !g_UserChoseToExit)
    {// Toggle ESC menu
                
      DISPLAY_state.ESC_Menu_Up = !DISPLAY_state.ESC_Menu_Up;
      if( DISPLAY_state.ESC_Menu_Up )
      {// Raise the menu
        LE_SEQNCR_StartXY( LED_IFT( DAT_LANG2, TAB_areyousure), DISPLAY_FREAKIN_HIGH, 300 - 11, DISPLAY_EscMenuStartY );
        LE_SEQNCR_StartXY( LED_IFT( DAT_LANG2, TAB_yes), DISPLAY_FREAKIN_HIGH + 1, 310 - 11, DISPLAY_EscMenuStartY + 100 );
        LE_SEQNCR_StartXY( LED_IFT( DAT_LANG2, TAB_no ), DISPLAY_FREAKIN_HIGH + 1, 430 - 11, DISPLAY_EscMenuStartY + 100 );
        DISPLAY_state.ESC_Menu_Up_Time = LE_TIME_TickCount;
      } else
      {// Drop the menu
        LE_SEQNCR_Stop( LED_IFT( DAT_LANG2, TAB_areyousure), DISPLAY_FREAKIN_HIGH );
        LE_SEQNCR_Stop( LED_IFT( DAT_LANG2, TAB_yes), DISPLAY_FREAKIN_HIGH + 1 );
        LE_SEQNCR_Stop( LED_IFT( DAT_LANG2, TAB_no ), DISPLAY_FREAKIN_HIGH + 1 );
      }
    }
    else if( UIMessagePntr->numberA == LE_KEYBRD_ESC && g_UserChoseToExit)
        UDOPTIONS_ProcessCreditsOptionButtonPress(-1);
        
  }
  // Process escape menu
  if( DISPLAY_state.ESC_Menu_Up )
  {
    BOOL hitY = FALSE, hitN = FALSE;
    // First up, did we hit Y, N or click a button - if so do something.
    if( UIMessagePntr->messageCode == UIMSG_KEYBOARD_PRESSED )
    {
      if( UIMessagePntr->numberA == LE_KEYBRD_N )
        hitN = TRUE;
      if( UIMessagePntr->numberA == LE_KEYBRD_Y )
        hitY = TRUE;
    }
    if( UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN )
    {// See if its our buttons
      if( (UIMessagePntr->numberA >= 310 - 11 ) &&
          (UIMessagePntr->numberA <= 392 - 11 ) &&
          (UIMessagePntr->numberB >= DISPLAY_EscMenuStartY + 100) &&
          (UIMessagePntr->numberB <= DISPLAY_EscMenuStartY + 136) )
        hitY = TRUE;
      if( (UIMessagePntr->numberA >= 430 - 11 ) &&
          (UIMessagePntr->numberA <= 512 - 11 ) &&
          (UIMessagePntr->numberB >= DISPLAY_EscMenuStartY + 100 ) &&
          (UIMessagePntr->numberB <= DISPLAY_EscMenuStartY + 136) )
        hitN = TRUE;
    }

    if( hitY )
    {// return to menu/quit app
      UDSOUND_Click();
        if( g_UserRequestedNewGame )
        {
            MESS_SendAction (ACTION_NEW_GAME, AnyLocalPlayer (), RULE_BANK_PLAYER,
              FALSE, 0, 0, 0, NULL, 0, NULL ); // New game
            // BLEEBLE:  Hack for setup code, see declaration of this flag.
            g_bNewGameRequestedFlagForUDPSELCode = TRUE;
        } else
        {
          if( (DISPLAY_IsIBarVisible || GameInProgress) & !g_UserRequestedExit )
          {// a new game is not quite what we want in network play
            MESS_StopAllNetworking();
            MESS_SendAction (ACTION_NEW_GAME, AnyLocalPlayer (), RULE_BANK_PLAYER,
              FALSE, 0, 0, 0, NULL, 0, NULL ); // New game
            // BLEEBLE:  Hack for setup code, see declaration of this flag.
            g_bNewGameRequestedFlagForUDPSELCode = TRUE;

          } else
          {
            
            // set the user chose to exit flag
            g_UserChoseToExit = TRUE;

            // Issue a lock to permanently kill the game (which IS running).  The g_UserChoseToExit tells it not to unlock (timeout).
            MESS_StopAllNetworking();//BLEEBLE untested last minute fix
            gameQueueLock();

            // roll the credits
            current_options_result = CREDITS_SCREEN;
            UDIBar_ProcessOptionsButtonPress(CREDITS_SCREEN); 
            if(DISPLAY_state.desired2DView != DISPLAY_SCREEN_Options)
              UDBOARD_SetBackdrop( DISPLAY_SCREEN_Options );
            else
            {
              UDOPTIONS_RemoveLastOptionsScreen(last_options_result);
              UDOPTIONS_DisplayCreditsScreen(CREDITS_SCREEN);
            }
            
          }
        }
       
        // reset the playerselect screen variable to not have the file menu be
        // pressed when starting a new game
        last_playerselectscreen = DISPLAY_SCREEN_Pselect;

      hitN = TRUE;
      g_IsYesHitProcessed = FALSE;
    }

    // quick check - timeout the menu
    if( DISPLAY_state.ESC_Menu_Up_Time + 60 * DISPLAY_EscTimeoutSeconds < LE_TIME_TickCount )
      hitN = TRUE;
    if( hitN )
    { // Drop the menu
      UDSOUND_Click();
      LE_SEQNCR_Stop( LED_IFT( DAT_LANG2, TAB_areyousure), DISPLAY_FREAKIN_HIGH );
      LE_SEQNCR_Stop( LED_IFT( DAT_LANG2, TAB_yes), DISPLAY_FREAKIN_HIGH + 1 );
      LE_SEQNCR_Stop( LED_IFT( DAT_LANG2, TAB_no ), DISPLAY_FREAKIN_HIGH + 1 );
      DISPLAY_state.ESC_Menu_Up = FALSE;

      g_UserRequestedExit = FALSE;
      g_UserRequestedNewGame = FALSE;
      // if we were in the options menu, bring back the file menu
      if((DISPLAY_state.desired2DView == DISPLAY_SCREEN_Options)
          && !g_UserChoseToExit)
      {
        //UDOPTIONS_DisplayFileScreen(FILE_SCREEN);
        UDBOARD_SetBackdrop( last_playerselectscreen );
      }
    }
    
  }

  // Temporary dialog conversion (crap)
  /*if( UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN )
  {
    DISPLAY_state.LE_TickCount_Last_Button_Click = LE_TIME_TickCount;
    switch( DISPLAY_state.CurrentDialog )
    {
      extern BOOL GameQuitRequested;
    case DISPLAY_Dialog_GameOver: // NOTIFY_GAME_OVER response
      if( DISPLAY_state.tempConversionMouseClickedButton == 1 ) // new game
      {
        MESS_SendAction (ACTION_NEW_GAME, AnyLocalPlayer (), RULE_BANK_PLAYER,
          FALSE, 0, 0, 0, NULL, 0, NULL ); // New game
      }
      if( DISPLAY_state.tempConversionMouseClickedButton == 2 ) // play again
      {
       UDPENNY_PennybagsGeneral( PB_GameStarted_old, Sound_SkipIfOldSoundPlaying );
       MESS_SendAction (ACTION_NEW_GAME, AnyLocalPlayer (), RULE_BANK_PLAYER,
          TRUE, 0, 0, 0, NULL, 0, NULL );
      }
      if( DISPLAY_state.tempConversionMouseClickedButton == 3 ) // quit
      {
        GameQuitRequested = TRUE;
      }
      break;
    };
  }*/



  //Mouseover checks.
  if( (UIMessagePntr->messageCode == UIMSG_MOUSE_MOVED) &&
      (DISPLAY_IsIBarVisible) )
  { // Check the action button bar
    DISPLAY_state.IBarButtonCurrentMouseOver = -1;
    for( t = 0; t < IBAR_BUTTON_MAX; t++ )
    {// Pain in the butt, since we use non-standard coords we have to switch for the right buttons.
      switch( DISPLAY_state.IBarCurrentState )
      {
      case IBAR_STATE_BuyAuction:
        if( (UIMessagePntr->numberA >= IBAR_ButtonBuyAuctionLocations[t].left  ) &&
            (UIMessagePntr->numberA <  IBAR_ButtonBuyAuctionLocations[t].right ) &&
            (UIMessagePntr->numberB >= IBAR_ButtonBuyAuctionLocations[t].top   ) &&
            (UIMessagePntr->numberB <  IBAR_ButtonBuyAuctionLocations[t].bottom) )
            DISPLAY_state.IBarButtonCurrentMouseOver = t;
        break;
      case IBAR_STATE_TaxDecision:
        if( (UIMessagePntr->numberA >= IBAR_ButtonTaxChoiceLocations[t].left  ) &&
            (UIMessagePntr->numberA <  IBAR_ButtonTaxChoiceLocations[t].right ) &&
            (UIMessagePntr->numberB >= IBAR_ButtonTaxChoiceLocations[t].top   ) &&
            (UIMessagePntr->numberB <  IBAR_ButtonTaxChoiceLocations[t].bottom) )
            DISPLAY_state.IBarButtonCurrentMouseOver = t;
        break;
      case IBAR_STATE_Trading:
        if( (UIMessagePntr->numberA >= IBAR_ButtonTradeChoiceLocations[t].left  ) &&
            (UIMessagePntr->numberA <  IBAR_ButtonTradeChoiceLocations[t].right ) &&
            (UIMessagePntr->numberB >= IBAR_ButtonTradeChoiceLocations[t].top   ) &&
            (UIMessagePntr->numberB <  IBAR_ButtonTradeChoiceLocations[t].bottom) )
            DISPLAY_state.IBarButtonCurrentMouseOver = t;
        break;
      default:
        if( (UIMessagePntr->numberA >= IBAR_ButtonGeneralLocations[t].left  ) &&
            (UIMessagePntr->numberA <  IBAR_ButtonGeneralLocations[t].right ) &&
            (UIMessagePntr->numberB >= IBAR_ButtonGeneralLocations[t].top   ) &&
            (UIMessagePntr->numberB <  IBAR_ButtonGeneralLocations[t].bottom) )
            DISPLAY_state.IBarButtonCurrentMouseOver = t;
        break;
      }
    }
    if( DISPLAY_IsPropertyBarAvailable )
    {// Check the Property bar also
      DISPLAY_state.IBarPropertyCurrentMouseOver = -1;
      for( t = SQ_MAX_SQUARE_TYPES -1; t >= 0; t-- )
      {//Check back cards to front (works out that way anyway)
        if( (UIMessagePntr->numberA >= IBAR_PropertyLocations[t].left   ) &&
          (UIMessagePntr->numberA <  IBAR_PropertyLocations[t].right  ) &&
          (UIMessagePntr->numberB >= IBAR_PropertyLocations[t].top    ) &&
          (UIMessagePntr->numberB <  IBAR_PropertyLocations[t].bottom ) )
          // One last check - is the property visible?  If not, it should not be eligible.
          if( DISPLAY_state.IBarProperties[t] != LED_EI )
            DISPLAY_state.IBarPropertyCurrentMouseOver = t;
      }
    }
    DISPLAY_state.IBarPlayerCurrentMouseOver = -1;
    for( t = 0; t < UICurrentGameState.NumberOfPlayers; t++ )
    {// Check the score box
      if ( (UIMessagePntr->numberA >= DISPLAY_state.PlayerScoreBoxesLocX[t])
       && (UIMessagePntr->numberA < DISPLAY_state.PlayerScoreBoxesLocX[t]
       + LE_GRAFIX_ReturnObjectWidth(DISPLAY_state.PlayerColorBarShownID[t]))
       && (UIMessagePntr->numberB >= DISPLAY_ScoreY)
       && (UIMessagePntr->numberB < DISPLAY_ScoreY
       + LE_GRAFIX_ReturnObjectWidth(DISPLAY_state.PlayerColorBarShownID[t])) )
      {
          DISPLAY_state.IBarPlayerCurrentMouseOver = t;
      }
    }
    // Special case for the bank player.
    if( (UIMessagePntr->numberA >= DISPLAY_state.PlayerScoreBoxesLocX[RULE_MAX_PLAYERS] ) &&
        (UIMessagePntr->numberA <  DISPLAY_state.PlayerScoreBoxesLocX[RULE_MAX_PLAYERS] + DISPLAY_IBAR_BankWidth  ) &&
        (UIMessagePntr->numberB >= DISPLAY_ScoreY      ) &&
        (UIMessagePntr->numberB <  DISPLAY_ScoreY + 32 ) )
        DISPLAY_state.IBarPlayerCurrentMouseOver = RULE_MAX_PLAYERS;
  }


  //Mouse click checks (buttons)
  if( (DISPLAY_IsIBarVisible) &&
      (UIMessagePntr->messageCode == UIMSG_KEYBOARD_PRESSED) )
  {// Check for characters which map to mouse clicks.  Modify the message to look like a click.
    if( UIMessagePntr->numberA == LE_KEYBRD_SPACE )
    {// Pretend to be a main button click.
      switch( DISPLAY_state.IBarCurrentState )
      {
      case IBAR_STATE_BuyAuction:
          UIMessagePntr->messageCode = UIMSG_MOUSE_LEFT_DOWN;
          UIMessagePntr->numberA = IBAR_ButtonBuyAuctionLocations[IBAR_Main].left;
          UIMessagePntr->numberB = IBAR_ButtonBuyAuctionLocations[IBAR_Main].top;
        break;
      case IBAR_STATE_TaxDecision:
          UIMessagePntr->messageCode = UIMSG_MOUSE_LEFT_DOWN;
          UIMessagePntr->numberA = IBAR_ButtonTaxChoiceLocations[IBAR_Main].left;
          UIMessagePntr->numberB = IBAR_ButtonTaxChoiceLocations[IBAR_Main].top;
        break;
      case IBAR_STATE_Trading:
          UIMessagePntr->messageCode = UIMSG_MOUSE_LEFT_DOWN;
          UIMessagePntr->numberA = IBAR_ButtonTradeChoiceLocations[IBAR_Main].left;
          UIMessagePntr->numberB = IBAR_ButtonTradeChoiceLocations[IBAR_Main].top;
        break;
      default:
        UIMessagePntr->messageCode = UIMSG_MOUSE_LEFT_DOWN;
        UIMessagePntr->numberA = IBAR_ButtonGeneralLocations[IBAR_Main].left;
        UIMessagePntr->numberB = IBAR_ButtonGeneralLocations[IBAR_Main].top;
      }
    }
  }
  // And one to allow clicking spinning dice to mean 'roll' (click default)
  if( (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN) &&
    (DISPLAY_IsIBarVisible) &&
    DISPLAY_state.IBarCurrentState == IBAR_STATE_StartTurn )
  {
    if( UIMessagePntr->numberA >= 390 &&
      UIMessagePntr->numberB   >=483 &&
      UIMessagePntr->numberA   <= 459 &&
      UIMessagePntr->numberB   <= 530 )
    {
      UIMessagePntr->numberA = IBAR_ButtonGeneralLocations[IBAR_Main].left;
      UIMessagePntr->numberB = IBAR_ButtonGeneralLocations[IBAR_Main].top;
    }
  }

  if ( (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN)
   && (UIMessagePntr->numberB >= UDIBAR_IBarTopButton)
   && (DISPLAY_IsIBarVisible || (DISPLAY_state.desired2DView == DISPLAY_SCREEN_Pselect)) )
                                /* Could be clicking on player to remove during setup. */
  {// Ignore clicks above first button and hope it is highest...
    ButtonHit = PropertyHit = PlayerOrBankHit = -1;
    // Check the action button bar
    for( t = 0; t < IBAR_BUTTON_MAX; t++ )
    {
      switch( DISPLAY_state.IBarCurrentState )
      {
      case IBAR_STATE_BuyAuction:
        if( (UIMessagePntr->numberA >= IBAR_ButtonBuyAuctionLocations[t].left  ) &&
            (UIMessagePntr->numberA <  IBAR_ButtonBuyAuctionLocations[t].right ) &&
            (UIMessagePntr->numberB >= IBAR_ButtonBuyAuctionLocations[t].top   ) &&
            (UIMessagePntr->numberB <  IBAR_ButtonBuyAuctionLocations[t].bottom) )
            ButtonHit = t;
        break;
      case IBAR_STATE_TaxDecision:
        if( (UIMessagePntr->numberA >= IBAR_ButtonTaxChoiceLocations[t].left  ) &&
            (UIMessagePntr->numberA <  IBAR_ButtonTaxChoiceLocations[t].right ) &&
            (UIMessagePntr->numberB >= IBAR_ButtonTaxChoiceLocations[t].top   ) &&
            (UIMessagePntr->numberB <  IBAR_ButtonTaxChoiceLocations[t].bottom) )
            ButtonHit = t;
        break;
      case IBAR_STATE_Trading:
        if( (UIMessagePntr->numberA >= IBAR_ButtonTradeChoiceLocations[t].left  ) &&
            (UIMessagePntr->numberA <  IBAR_ButtonTradeChoiceLocations[t].right ) &&
            (UIMessagePntr->numberB >= IBAR_ButtonTradeChoiceLocations[t].top   ) &&
            (UIMessagePntr->numberB <  IBAR_ButtonTradeChoiceLocations[t].bottom) )
            ButtonHit = t;
        break;
      default:
      if( (UIMessagePntr->numberA >= IBAR_ButtonGeneralLocations[t].left  ) &&
          (UIMessagePntr->numberA <  IBAR_ButtonGeneralLocations[t].right ) &&
          (UIMessagePntr->numberB >= IBAR_ButtonGeneralLocations[t].top   ) &&
          (UIMessagePntr->numberB <  IBAR_ButtonGeneralLocations[t].bottom) )
          ButtonHit = t;
      }
    }

    // Check the Property bar
    for( t = SQ_MAX_SQUARE_TYPES -1; t >= 0; t-- )
    {
      //DISPLAY_state.IBarProperties[t] = LED_EI;
      if( (UIMessagePntr->numberA >= IBAR_PropertyLocations[t].left   ) &&
        (UIMessagePntr->numberA <  IBAR_PropertyLocations[t].right  ) &&
        (UIMessagePntr->numberB >= IBAR_PropertyLocations[t].top    ) &&
        (UIMessagePntr->numberB <  IBAR_PropertyLocations[t].bottom ) )
        if( (DISPLAY_state.IBarProperties[t] != LED_EI ) ||
                        (   UIMessagePntr->numberE == DISPLAY_SCREEN_PortfolioA))
          PropertyHit = t; // Only acknowledge the click if the property is visible.
    }

    // Check the Player bar
    for( t = 0; t < UICurrentGameState.NumberOfPlayers; t++ )
    {
      if ( (UIMessagePntr->numberA >= DISPLAY_state.PlayerScoreBoxesLocX[t])
       && (UIMessagePntr->numberA < DISPLAY_state.PlayerScoreBoxesLocX[t]
       + LE_GRAFIX_ReturnObjectWidth(DISPLAY_state.PlayerColorBarShownID[t]))
       && (UIMessagePntr->numberB >= DISPLAY_ScoreY)
       && (UIMessagePntr->numberB < DISPLAY_ScoreY
       + LE_GRAFIX_ReturnObjectHeight(DISPLAY_state.PlayerColorBarShownID[t])) )
      {
          PlayerOrBankHit = t;
      }
    }
    // Special case for the bank player.
    if ( (UIMessagePntr->numberA >= DISPLAY_state.PlayerScoreBoxesLocX[RULE_MAX_PLAYERS] )
     && (UIMessagePntr->numberA <  DISPLAY_state.PlayerScoreBoxesLocX[RULE_MAX_PLAYERS] + DISPLAY_IBAR_BankWidth )
     && (UIMessagePntr->numberB >= DISPLAY_ScoreY      )
     && (UIMessagePntr->numberB <  DISPLAY_ScoreY + 32 ) )
    {
        PlayerOrBankHit = RULE_MAX_PLAYERS;
    }


    // Timer flag for button clicks
    if( ButtonHit + PropertyHit + PlayerOrBankHit != -3)
      DISPLAY_state.LE_TickCount_Last_Button_Click = LE_TIME_TickCount;


   // Action Button Handling
    switch( ButtonHit )
    {
    case IBAR_Options: // Go to option screen.
      if( (DISPLAY_state.desired2DView != DISPLAY_SCREEN_Black) &&
          GameInProgress &&
          (DISPLAY_state.desired2DView != DISPLAY_SCREEN_AuctionA) )//&&
          //(DISPLAY_state.desired2DView != DISPLAY_SCREEN_TradeA) )
        //DISPLAY_IsIBarVisible          
      {
        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Options; // Not an engine button - mark the press here.
        //UDSOUND_Warning();

        //if(last_playerselectphase == -1)
        //{	
          last_playerselectphase = UDPSEL_PHASE_NONE;
          last_playerselectscreen = DISPLAY_state.current2DView;
          // remove the player select stuff from the screen
          // display the options background screen
        //  UDBOARD_SetBackdrop( DISPLAY_SCREEN_Options ); removed(RK) - do this regardless, we know the IBar was up.
          last_options_result = -1;			
        //}
        UDBOARD_SetBackdrop( DISPLAY_SCREEN_Options );
      }
      break;

    case IBAR_Trade:
      //if( DISPLAY_state.desired2DView != DISPLAY_SCREEN_TradeA && GameInProgress)
      if( DISPLAY_state.desired2DView != DISPLAY_SCREEN_TradeA && GameInProgress )
      {// Bankrupt players can't go to the trade screen.
        if( UICurrentGameState.Players[DISPLAY_state.IBarCurrentPlayer].currentSquare != 41 )
          if( UDTrade_GetPlayerToTradeFrom() != RULE_MAX_PLAYERS )// Someone here to trade
            
          {
            DISPLAY_state.desired2DView = DISPLAY_SCREEN_TradeA;
            DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Trade;
            if( TradeFromPlayer == -1 || TradeAPlayerActive == -1 || TradeBPlayerActive == -1 || // Not valid players
              ( TradeItemListTop == 0 ) ) // trade is empty
            {
              if( CurrentUIPlayer < RULE_MAX_PLAYERS )
                TradeFromPlayer = CurrentUIPlayer;
              else
                TradeFromPlayer = AnyLocalPlayer();
              UDTrade_ClearTrade();
              TradeAPlayerActive = TradeFromPlayer;
            }
          }
      }
      break;

    case IBAR_Status: // Status button area - Status or Main view.
      if(( DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA  ||
         DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA ) &&
         GameInProgress )

      {
        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Main; // Not an engine button - mark the press here.
        UDBOARD_SetBackdrop( DISPLAY_SCREEN_MainA );
        DISPLAY_showAll;
      } else if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA )
      {
        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Status; // Not an engine button - mark the press here.
        UDBOARD_SetBackdrop( DISPLAY_SCREEN_PortfolioA );
        DISPLAY_showAll;
      }
      break;

    case IBAR_View1: // The Camera button
      {
        static topview = 0;

        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Camera; // Not an engine button - mark the press here.
        // Identify the current camera 'category'
        IBAR_JustChanged = TRUE;// Special - no state change, so this will force an animation.
        int tempCategory = 0; // Top view
        if( DISPLAY_state.desired2DCamera >= VIEW2D04_3TILES01  ) tempCategory++; // 3 view
        if( DISPLAY_state.desired2DCamera >= VIEW2D16_CORNER_GO ) tempCategory++; // Corner view O what to do
        if( DISPLAY_state.desired2DCamera >= VIEW2D20_5TILES01  ) tempCategory++;  // 5 view
        if( DISPLAY_state.desired2DCamera >= VIEW2D28_15TILES01 ) tempCategory++;  // 15 view

        if( LE_MOUSE_MousePressed[ RIGHT_BUTTON ] || LE_KEYBRD_CheckPressed( LE_KEYBRD_CONTROL ) )
          DISPLAY_state.desired2DCamera = (DISPLAY_state.desired2DCamera + 1) % VIEW2DMAX;
        else
        {
          switch( (tempCategory+1) % 5 )
          {
          case 0: // top
            DISPLAY_state.desired2DCamera = topview++%3;//rand()%(VIEW2D03_TOPDOWN_STARWARS + 1);
            break;
          case 1: // 3 view
            DISPLAY_state.desired2DCamera = UDPIECES_PickCameraFor3Squares( UICurrentGameState.Players[DISPLAY_state.IBarCurrentPlayer].currentSquare );
            break;
          case 2: // corner
          case 3: // 5 view
          case 4: // 15 view
          default:
            DISPLAY_state.desired2DCamera = UDPIECES_PickCameraFor15Squares( UICurrentGameState.Players[DISPLAY_state.IBarCurrentPlayer].currentSquare );
            if( DISPLAY_state.desired2DCamera == VIEW2D17_CORNER_JAIL ) // We are in jail, duplicate view.
              DISPLAY_state.desired2DCamera = topview++%3;
            break;
          }
        }
        // Now, we consider this a manual override - if the camera has a manual lock, flag this change is to be processed.
        if( DISPLAY_state.manualCamLock || DISPLAY_state.manualMouseCamLock )
        {
          DISPLAY_state.manualCamDesired   = DISPLAY_state.desired2DCamera;
          DISPLAY_state.manualCamRequested = TRUE;
        }
      }
      break;
    case IBAR_General1: // Build
      if( (DISPLAY_state.IBarBSSMButtonsAvail & UDIBAR_DeedStateButtonBuyHouse) &&
           !AI_ButtonRemoteState &&
         ((DISPLAY_state.IBarCurrentState == IBAR_STATE_StartTurn) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_OtherPlayer) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_FreeUnmortgage) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_DoneTurn)) )
      {
        // Be sure the player and tracker are set
        if( DISPLAY_state.IBarStateTrackOn )
        {
          DISPLAY_state.IBarStateTrackOn = FALSE;
          DISPLAY_state.IBarCurrentPlayer = DISPLAY_state.IBarLastRulePlayer;
        };
        UDPENNY_PennybagsGeneral( PB_PlayerClicks_BuyHouseHotel, Sound_SkipIfOldSoundPlaying );
        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Build; // Not an engine button - mark the press here.
        UDIBAR_setIBarCurrentState( IBAR_STATE_Build );
                // set status screen flags to update screen
                //if(fStatusScreen == PLAYER_SCREEN)
                {
                    g_bIsScreenUpdated = FALSE;
                    g_Button = last_sort_result + B_SORT_1;
                }
        DISPLAY_showAll;
      }
      if( (DISPLAY_state.IBarCurrentState == IBAR_STATE_DeedActive) &&
           !AI_ButtonRemoteState &&
          (DISPLAY_state.IBarDeedButtonsAvail & UDIBAR_DeedStateButtonBuyHouse) ) // Bar allowed
      {
        /*tempMessage.action = ACTION_BUY_HOUSE;
        tempMessage.fromPlayer = DISPLAY_state.IBarCurrentPlayer;
        tempMessage.toPlayer = RULE_BANK_PLAYER;
        tempMessage.numberA = DISPLAY_state.IBarDeedForDeedState;
        tempMessage.numberB = FALSE;
        tempMessage.numberC = 0;
        tempMessage.numberD = 0;
        tempMessage.numberE = 0;
        tempMessage.stringA[0] = 0;
        tempMessage.binaryDataSizeA = 0,
        tempMessage.binaryDataHandleA = NULL;
        if( !IBAR_BSSM_LOCK::BSSM_GetLock( &tempMessage) )
          UDSOUND_Warning(); // Interface is already waiting for a lock!*/
        MESS_SendAction (ACTION_BUY_HOUSE, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
          DISPLAY_state.IBarDeedForDeedState, 0, 0, TRUE, // TRUE - quicky rule (no BSSM lock nor state)
          NULL, 0, NULL);
      };
      break;

    case IBAR_General2: // Sell
      if( (DISPLAY_state.IBarBSSMButtonsAvail & UDIBAR_DeedStateButtonSellHouse) &&
           !AI_ButtonRemoteState &&
         ((DISPLAY_state.IBarCurrentState == IBAR_STATE_StartTurn) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_OtherPlayer) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_FreeUnmortgage) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_DoneTurn) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_RaiseMoney)) )
      {
        // Be sure the player and tracker are set
        if( DISPLAY_state.IBarStateTrackOn )
        {
          DISPLAY_state.IBarStateTrackOn = FALSE;
          DISPLAY_state.IBarCurrentPlayer = DISPLAY_state.IBarLastRulePlayer;
        };
        UDPENNY_PennybagsGeneral( PB_PlayerClicks_SellHouseHotel, Sound_SkipIfOldSoundPlaying );
        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Sell; // Not an engine button - mark the press here.
        UDIBAR_setIBarCurrentState( IBAR_STATE_Sell );
                // set status screen flags to update screen
                //if(fStatusScreen == PLAYER_SCREEN)
                {
                    g_bIsScreenUpdated = FALSE;
                    g_Button = last_sort_result + B_SORT_1;
                }
        DISPLAY_showAll;
      }
      if( !AI_ButtonRemoteState &&
        DISPLAY_state.IBarCurrentState == IBAR_STATE_Trading )
      { //Counter offer
        MESS_SendAction ( ACTION_TRADE_ACCEPT, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
          (int)FALSE, -1, 0, 0, NULL, 0, NULL );
      }
      if(( (DISPLAY_state.IBarCurrentState == IBAR_STATE_JailExitPCR) ||
        (DISPLAY_state.IBarCurrentState == IBAR_STATE_JailExitPXR) ||
        (DISPLAY_state.IBarCurrentState == IBAR_STATE_JailExitPCX) ||
        (DISPLAY_state.IBarCurrentState == IBAR_STATE_JailExitPXX) ) && 
        !AI_ButtonRemoteState )
      { // Jail Exit choice PAY
        MESS_SendAction( ACTION_EXIT_JAIL_DECISION, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER, // 0 for roll, 1 for pay, 2 for card
          1, 0, 0, 0, NULL, 0, NULL );
      }
      if( (DISPLAY_state.IBarCurrentState == IBAR_STATE_DeedActive) &&
          !AI_ButtonRemoteState &&
          (DISPLAY_state.IBarDeedButtonsAvail & UDIBAR_DeedStateButtonSellHouse) ) // Bar allowed
      {
        /*tempMessage.action = ACTION_SELL_BUILDINGS;
        tempMessage.fromPlayer = DISPLAY_state.IBarCurrentPlayer;
        tempMessage.toPlayer = RULE_BANK_PLAYER;
        tempMessage.numberA = DISPLAY_state.IBarDeedForDeedState;
        tempMessage.numberB = FALSE;// Just one house (not the set)
        tempMessage.numberC = 0;
        tempMessage.numberD = 0;
        tempMessage.numberE = 0;
        tempMessage.stringA[0] = 0;
        tempMessage.binaryDataSizeA = 0,
        tempMessage.binaryDataHandleA = NULL;
        if( !IBAR_BSSM_LOCK::BSSM_GetLock( &tempMessage) )
          UDSOUND_Warning(); // Interface is already waiting for a lock!*/
        MESS_SendAction (ACTION_SELL_BUILDINGS, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
          DISPLAY_state.IBarDeedForDeedState, 0, 0, TRUE, // TRUE - quicky rule (no BSSM lock nor state)
          NULL, 0, NULL);
      };
      if( DISPLAY_state.IBarCurrentState == IBAR_STATE_GameOver )
      {// New game
        MESS_SendAction (ACTION_NEW_GAME, AnyLocalPlayer (), RULE_BANK_PLAYER,
          FALSE, 0, 0, 0, NULL, 0, NULL ); // New game
      }
      break;

    case IBAR_General3: // Mortgage + Roll doubles + Auction + tax%
      if( (DISPLAY_state.IBarBSSMButtonsAvail & UDIBAR_DeedStateButtonMortgage) &&
          !AI_ButtonRemoteState &&
         ((DISPLAY_state.IBarCurrentState == IBAR_STATE_StartTurn) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_OtherPlayer) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_FreeUnmortgage) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_DoneTurn) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_RaiseMoney)) )
      { // Mortgage
        // Be sure the player and tracker are set
        if( DISPLAY_state.IBarStateTrackOn )
        {
          DISPLAY_state.IBarStateTrackOn = FALSE;
          DISPLAY_state.IBarCurrentPlayer = DISPLAY_state.IBarLastRulePlayer;
        };
        UDPENNY_PennybagsGeneral( PB_PlayerClicks_Mortgage, Sound_SkipIfOldSoundPlaying );
        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Mortgage; // Not an engine button - mark the press here.
        UDIBAR_setIBarCurrentState( IBAR_STATE_Mortgage );
                // set status screen flags to update screen
                //if(fStatusScreen == PLAYER_SCREEN)
                {
                    g_bIsScreenUpdated = FALSE;
                    g_Button = last_sort_result + B_SORT_1;
                }
        DISPLAY_showAll;
      }
      if( !AI_ButtonRemoteState &&
        DISPLAY_state.IBarCurrentState == IBAR_STATE_BuyAuction )
      { // Property Buy/Auction - this is auction button.
        //UDSOUND_Warning();
        MESS_SendAction( ACTION_BUY_OR_AUCTION_DECISION, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
          FALSE, 0, 0, 0, NULL, 0, NULL ); // False for Auction.
      }
      if( !AI_ButtonRemoteState &&
        DISPLAY_state.IBarCurrentState == IBAR_STATE_TaxDecision ) // Tax decision, here is fractional.
      {
        MESS_SendAction (ACTION_TAX_DECISION, CurrentUIPlayer, RULE_BANK_PLAYER,
          TRUE, 0, 0, 0, NULL, 0, NULL); // FALSE is flat.
      }
      if( !AI_ButtonRemoteState &&
        DISPLAY_state.IBarCurrentState == IBAR_STATE_Trading )
      {//Accept trade
        MESS_SendAction ( ACTION_TRADE_ACCEPT, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
          (int)TRUE, 1, 0, 0, NULL, 0, NULL ); // TRUE accept.
      }
      if(!AI_ButtonRemoteState &&
        ( (DISPLAY_state.IBarCurrentState == IBAR_STATE_JailExitPCR) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_JailExitPCX) ))
      { // Jail Exit choice CARD
        MESS_SendAction (ACTION_EXIT_JAIL_DECISION, CurrentUIPlayer, RULE_BANK_PLAYER, // 0 for roll, 1 for pay, 2 for card
          2, 0, 0, 0, NULL, 0, NULL);
      }
      if( (DISPLAY_state.IBarCurrentState == IBAR_STATE_DeedActive) &&
        !AI_ButtonRemoteState &&
          (DISPLAY_state.IBarDeedButtonsAvail & UDIBAR_DeedStateButtonMortgage) ) // Bar allowed
      {
        /*tempMessage.action = ACTION_MORTGAGING;
        tempMessage.fromPlayer = DISPLAY_state.IBarCurrentPlayer;
        tempMessage.toPlayer = RULE_BANK_PLAYER;
        tempMessage.numberA = DISPLAY_state.IBarDeedForDeedState;
        tempMessage.numberB = 0;
        tempMessage.numberC = 0;
        tempMessage.numberD = 0;
        tempMessage.numberE = 0;
        tempMessage.stringA[0] = 0;
        tempMessage.binaryDataSizeA = 0,
        tempMessage.binaryDataHandleA = NULL;
        if( !IBAR_BSSM_LOCK::BSSM_GetLock( &tempMessage) )
          UDSOUND_Warning(); // Interface is already waiting for a lock!*/
        // New - lets send a 'quicky' mortgage which shouldn't be refused.
        MESS_SendAction (ACTION_MORTGAGING, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
          DISPLAY_state.IBarDeedForDeedState, 0, 0, TRUE, // TRUE - quicky rule (no BSSM lock nor state)
          NULL, 0, NULL);
      }
      //if( DISPLAY_state.IBarCurrentState == IBAR_STATE_GameOver )
      //{// Play again
      //  UDPENNY_PennybagsGeneral( PB_GameStarted_old, Sound_SkipIfOldSoundPlaying );
      //  MESS_SendAction (ACTION_NEW_GAME, AnyLocalPlayer (), RULE_BANK_PLAYER,
      //    TRUE, 0, 0, 0, NULL, 0, NULL );
      //}
      break;

    case IBAR_General4: // UnMortgage
      if( (DISPLAY_state.IBarBSSMButtonsAvail & UDIBAR_DeedStateButtonUnMortgage) &&
        !AI_ButtonRemoteState &&
         ((DISPLAY_state.IBarCurrentState == IBAR_STATE_StartTurn) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_OtherPlayer) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_FreeUnmortgage) ||
          (DISPLAY_state.IBarCurrentState == IBAR_STATE_DoneTurn)) )
      { // Mortgage
        // Be sure the player and tracker are set
        if( DISPLAY_state.IBarStateTrackOn )
        {
          DISPLAY_state.IBarStateTrackOn = FALSE;
          DISPLAY_state.IBarCurrentPlayer = DISPLAY_state.IBarLastRulePlayer;
        };
        UDPENNY_PennybagsGeneral( PB_PlayerClicks_UnMortgage, Sound_SkipIfOldSoundPlaying );
        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Unmort; // Not an engine button - mark the press here.
        UDIBAR_setIBarCurrentState( IBAR_STATE_UnMortgage );
                // set status screen flags to update screen
                //if(fStatusScreen == PLAYER_SCREEN)
                {
                    g_bIsScreenUpdated = FALSE;
                    g_Button = last_sort_result + B_SORT_1;
                }
        DISPLAY_showAll;
      }
      if( !AI_ButtonRemoteState &&
        (DISPLAY_state.IBarCurrentState == IBAR_STATE_DeedActive) &&
          (DISPLAY_state.IBarDeedButtonsAvail & UDIBAR_DeedStateButtonUnMortgage) ) // Bar allowed
      {
        /*tempMessage.action = ACTION_MORTGAGING;
        tempMessage.fromPlayer = DISPLAY_state.IBarCurrentPlayer;
        tempMessage.toPlayer = RULE_BANK_PLAYER;
        tempMessage.numberA = DISPLAY_state.IBarDeedForDeedState;
        tempMessage.numberB = 0;
        tempMessage.numberC = 0;
        tempMessage.numberD = 0;
        tempMessage.numberE = 0;
        tempMessage.stringA[0] = 0;
        tempMessage.binaryDataSizeA = 0,
        tempMessage.binaryDataHandleA = NULL;
        if( !IBAR_BSSM_LOCK::BSSM_GetLock( &tempMessage) )
          UDSOUND_Warning(); // Interface is already waiting for a lock!*/
        MESS_SendAction (ACTION_MORTGAGING, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
          DISPLAY_state.IBarDeedForDeedState, 0, 0, TRUE, // TRUE - quicky rule (no BSSM lock nor state)
          NULL, 0, NULL);

      }
      break;

    case IBAR_Main: // MAIN acknowledgement/action button.
      // We need to know what the button meant, switch on IBAR state.
      if( !AI_ButtonRemoteState )
      {
        switch( DISPLAY_state.IBarCurrentState )
        {
        case IBAR_STATE_StartTurn: // Roll Dice
#if USE_HOTKEYS2
          if( LE_MOUSE_MousePressed[ RIGHT_BUTTON ] )
          {
            if( !LE_KEYBRD_CheckPressed( LE_KEYBRD_UP ) )
              MESS_SendAction (ACTION_CHEAT_ROLL_DICE, CurrentUIPlayer, RULE_BANK_PLAYER,
              2, 3, 0, 0, NULL, 0, NULL); // 1,1 - will do a regular roll if cheating is disabled.
            else
              MESS_SendAction (ACTION_CHEAT_ROLL_DICE, CurrentUIPlayer, RULE_BANK_PLAYER,
              5, 5, 0, 0, NULL, 0, NULL); // 6,6 - will do a regular roll if cheating is disabled.
          }
          else
#endif
            //Check for CheatStack dice rolls (used in demos)
          {
#if USE_PRESET_DICE_ROLLS
            if ( IBAR_CHEAT_RollIndex < IBAR_CHEAT_ROLLS_MAX )
            {
              MESS_SendAction( ACTION_CHEAT_ROLL_DICE, CurrentUIPlayer, RULE_BANK_PLAYER,
               IBAR_Cheat_Rolls[IBAR_CHEAT_RollIndex][0], IBAR_Cheat_Rolls[IBAR_CHEAT_RollIndex][1], 0, 0, NULL, 0, NULL );
              IBAR_CHEAT_RollIndex++;
            }
            else
#endif
              MESS_SendAction (ACTION_ROLL_DICE, CurrentUIPlayer, RULE_BANK_PLAYER,
              0, 0, 0, 0, NULL, 0, NULL);
          }
          break;
        case IBAR_STATE_DoneTurn: // Done Turn
          MESS_SendAction (ACTION_END_TURN, CurrentUIPlayer, RULE_BANK_PLAYER,
            0, 0, 0, 0, NULL, 0, NULL);
          break;
        case IBAR_STATE_ViewingCard:
          MESS_SendAction (ACTION_CARD_SEEN, CurrentUIPlayer, RULE_BANK_PLAYER,
            0, 0, 0, 0, NULL, 0, NULL);
          break;

        case IBAR_STATE_Build:
          if( DISPLAY_state.IBarCurrentPlayer == DISPLAY_state.IBarLastRulePlayer )
          {// Return to tracking state
            DISPLAY_state.IBarStateTrackOn = TRUE;
            DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Done; // Not an engine button - mark the press here.
                      // set status screen flags to update screen
                      //if(fStatusScreen == PLAYER_SCREEN)
                      {
                          g_bIsScreenUpdated = FALSE;
                          g_Button = last_sort_result + B_SORT_1;
                      }
          } else
          {// This logic is as per clicking a player button, only with that recorded player
            if( SlotIsALocalHumanPlayer [DISPLAY_state.IBarCurrentPlayer] )
              UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayer );
            else
              UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayerRemote );
            DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Done; // Not an engine button - mark the press here.
          }
          DISPLAY_showAll;
          break;

        case IBAR_STATE_Sell:
          if( DISPLAY_state.IBarCurrentPlayer == DISPLAY_state.IBarLastRulePlayer )
          {// Return to tracking state
            DISPLAY_state.IBarStateTrackOn = TRUE;
            DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Done; // Not an engine button - mark the press here.
                      // set status screen flags to update screen
                      //if(fStatusScreen == PLAYER_SCREEN)
                      {
                          g_bIsScreenUpdated = FALSE;
                          g_Button = last_sort_result + B_SORT_1;
                      }
          } else
          {// This logic is as per clicking a player button, only with that recorded player
            if( SlotIsALocalHumanPlayer [DISPLAY_state.IBarCurrentPlayer] )
              UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayer );
            else
              UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayerRemote );
            DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Done; // Not an engine button - mark the press here.
          }
          DISPLAY_showAll;
          break;

        case IBAR_STATE_Mortgage:
          if( DISPLAY_state.IBarCurrentPlayer == DISPLAY_state.IBarLastRulePlayer )
          {// Return to tracking state
            DISPLAY_state.IBarStateTrackOn = TRUE;
            DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Done; // Not an engine button - mark the press here.
                      // set status screen flags to update screen
                      //if(fStatusScreen == PLAYER_SCREEN)
                      {
                          g_bIsScreenUpdated = FALSE;
                          g_Button = last_sort_result + B_SORT_1;
                      }
          } else
          {// This logic is as per clicking a player button, only with that recorded player
            if( SlotIsALocalHumanPlayer [DISPLAY_state.IBarCurrentPlayer] )
              UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayer );
            else
              UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayerRemote );
            DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Done; // Not an engine button - mark the press here.
          }
          DISPLAY_showAll;
          break;

        case IBAR_STATE_UnMortgage:
          if( DISPLAY_state.IBarCurrentPlayer == DISPLAY_state.IBarLastRulePlayer )
          {// Return to tracking state
            DISPLAY_state.IBarStateTrackOn = TRUE;
            DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Done; // Not an engine button - mark the press here.
                      // set status screen flags to update screen
                      //if(fStatusScreen == PLAYER_SCREEN)
                      {
                          g_bIsScreenUpdated = FALSE;
                          g_Button = last_sort_result + B_SORT_1;
                      }
          } else
          {// This logic is as per clicking a player button, only with that recorded player
            if( SlotIsALocalHumanPlayer [DISPLAY_state.IBarCurrentPlayer] )
              UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayer );
            else
              UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayerRemote );
            DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Done; // Not an engine button - mark the press here.
          }
          DISPLAY_showAll;
          break;

        case IBAR_STATE_DeedActive:
          if( DISPLAY_state.IBarCurrentPlayer == DISPLAY_state.IBarLastRulePlayer )
          {// Return to tracking state
            DISPLAY_state.IBarStateTrackOn = TRUE;
            DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Done; // Not an engine button - mark the press here.
          } else
          {// This logic is as per clicking a player button, only with that recorded player
            if( SlotIsALocalHumanPlayer [DISPLAY_state.IBarCurrentPlayer] )
              UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayer );
            else
              UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayerRemote );
            DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Done; // Not an engine button - mark the press here.
          }
          DISPLAY_showAll;
          break;

        case IBAR_STATE_RaiseMoney: // Bankrupt request
          MESS_SendAction( ACTION_GO_BANKRUPT, DISPLAY_state.IBarCurrentPlayer, //CurrentUIPlayer,
            RULE_BANK_PLAYER, 0, 0, 0, 0, NULL, 0, NULL );
          break;

        case IBAR_STATE_BuyAuction: // Property Buy/Auction - this is buy button.
          MESS_SendAction( ACTION_BUY_OR_AUCTION_DECISION, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
            TRUE, 0, 0, 0, NULL, 0, NULL ); // False for Auction.
          break;

        case IBAR_STATE_TaxDecision: // Tax decision, main here is flat.
          MESS_SendAction( ACTION_TAX_DECISION, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
            FALSE, 0, 0, 0, NULL, 0, NULL ); // FALSE is flat.
          break;

        case IBAR_STATE_Trading: //Reject trade
          MESS_SendAction( ACTION_TRADE_ACCEPT, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
            (int)FALSE, 0, 0, 0, NULL, 0, NULL ); // FALSE- reject
          break;

        case IBAR_STATE_FreeUnmortgage:
          MESS_SendAction( ACTION_FREE_UNMORTGAGE_DONE, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
            0, 0, 0, 0, NULL, 0, NULL );
          break;

        case IBAR_STATE_JailExitPCR:
        case IBAR_STATE_JailExitPXR:
          MESS_SendAction (ACTION_EXIT_JAIL_DECISION, CurrentUIPlayer, RULE_BANK_PLAYER, // 0 for roll, 1 for pay, 2 for card
          0, 0, 0, 0, NULL, 0, NULL);
          break;

        case IBAR_STATE_HousingShort:
        case IBAR_STATE_HotelShort:
          MESS_SendAction( ACTION_START_HOUSING_AUCTION,
            DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
            0, 0, 0, 0, NULL, 0, NULL );
          break;

        case IBAR_STATE_GameOver: // Exit
          //GameQuitRequested = TRUE;
            g_UserRequestedExit = TRUE;
            // hack
            // fake a esc button hit
            LE_QUEUE_MessageRecord UItempMessage;
            UItempMessage.messageCode = UIMSG_KEYBOARD_PRESSED;
			UItempMessage.numberA = LE_KEYBRD_ESC;
			UDIBAR_ProcessMessage( &UItempMessage );
          break;

        case IBAR_STATE_OtherPlayer:
        case IBAR_STATE_OtherPlayerRemote:
          // Done wandering, back to IBAR tracking the game.
          DISPLAY_state.IBarStateTrackOn = TRUE;
          DISPLAY_showAll;
          break;

        }// End switch on IBAR state when IBAR_Main clicked
      } else
      {// One special case - if AI won, we still can exit.
        if( DISPLAY_state.IBarCurrentState == IBAR_STATE_GameOver)
        {
            //GameQuitRequested = TRUE;
            g_UserRequestedExit = TRUE;
            // hack
            // fake a esc button hit
            LE_QUEUE_MessageRecord UItempMessage;
            UItempMessage.messageCode = UIMSG_KEYBOARD_PRESSED;
			UItempMessage.numberA = LE_KEYBRD_ESC;
			UDIBAR_ProcessMessage( &UItempMessage );
        }
      }

    }


    // Property Button Handling
    if( PropertyHit >= 0 )
    {// What to do about the click?
      switch( DISPLAY_state.IBarCurrentState )
      {// Exclude IBAR_STATE_ViewingCard from going to deed state - can be used to cheat.

      // Check if we can go to deed state
      case IBAR_STATE_StartTurn:
      case IBAR_STATE_OtherPlayer:
      case IBAR_STATE_DoneTurn:
      case IBAR_STATE_JailExitPCR:
      case IBAR_STATE_JailExitPXR:
      case IBAR_STATE_JailExitPCX:
      case IBAR_STATE_JailExitPXX:
      case IBAR_STATE_BuyAuction:
      case IBAR_STATE_RaiseMoney: //Testing if this one is OK
        // Be sure the property is owned by the requesting player
        if( DISPLAY_state.IBarStateTrackOn )
          t = DISPLAY_state.IBarLastRulePlayer;
        else
          t = DISPLAY_state.IBarCurrentPlayer;

        if( UICurrentGameState.Squares[PropertyHit].owner == //t ) Not very time critical, but the current player is probably the only one used here.
          ( (DISPLAY_state.IBarStateTrackOn) ? DISPLAY_state.IBarLastRulePlayer : DISPLAY_state.IBarCurrentPlayer) )

        {
          // Enter Deed state
          if( DISPLAY_state.IBarStateTrackOn )
          {
            DISPLAY_state.IBarStateTrackOn = FALSE;
            DISPLAY_state.IBarCurrentPlayer = DISPLAY_state.IBarLastRulePlayer;
          };
          DISPLAY_state.IBarDeedForDeedState = PropertyHit;
          UDIBAR_setIBarCurrentState( IBAR_STATE_DeedActive );
          UDIBAR_DeedStateCalcButtons();
          DISPLAY_showAll;
        }
        break;
      case IBAR_STATE_DeedActive: // Only one deed showing...
        break;

      case IBAR_STATE_FreeUnmortgage:
        // NOTE:  This mode does not require an explicit lock - an explicit lock forces a regular unmortgage fee!
        MESS_SendAction( ACTION_MORTGAGING, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
          PropertyHit, 0, 0, 0, NULL, 0, NULL );
        break;

      case IBAR_STATE_UnMortgage: // Exactly like mortgage.
      case IBAR_STATE_Mortgage:
        //MESS_SendAction (ACTION_MORTGAGING, CurrentUIPlayer, RULE_BANK_PLAYER,
        //  property, 0, 0, 0, NULL, 0, NULL);
        /*tempMessage.action = ACTION_MORTGAGING;
        tempMessage.fromPlayer = DISPLAY_state.IBarCurrentPlayer;
        tempMessage.toPlayer = RULE_BANK_PLAYER;
        tempMessage.numberA = PropertyHit;
        tempMessage.numberB = 0;
        tempMessage.numberC = 0;
        tempMessage.numberD = 0;
        tempMessage.numberE = 0;
        tempMessage.stringA[0] = 0;
        tempMessage.binaryDataSizeA = 0,
        tempMessage.binaryDataHandleA = NULL;
        if( !IBAR_BSSM_LOCK::BSSM_GetLock( &tempMessage) )
          UDSOUND_Warning(); // Interface is already waiting for a lock!*/
        MESS_SendAction (ACTION_MORTGAGING, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
          PropertyHit, 0, 0, TRUE, // TRUE - quicky rule (no BSSM lock nor state)
          NULL, 0, NULL);

        break;

      case IBAR_STATE_Build:
        /*tempMessage.action = ACTION_BUY_HOUSE;
        tempMessage.fromPlayer = DISPLAY_state.IBarCurrentPlayer;
        tempMessage.toPlayer = RULE_BANK_PLAYER;
        tempMessage.numberA = PropertyHit;
        tempMessage.numberB = FALSE;
        tempMessage.numberC = 0;
        tempMessage.numberD = 0;
        tempMessage.numberE = 0;
        tempMessage.stringA[0] = 0;
        tempMessage.binaryDataSizeA = 0,
        tempMessage.binaryDataHandleA = NULL;
        if( !IBAR_BSSM_LOCK::BSSM_GetLock( &tempMessage) )
          UDSOUND_Warning(); // Interface is already waiting for a lock!*/
        MESS_SendAction (ACTION_BUY_HOUSE, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
          PropertyHit, 0, 0, TRUE, // TRUE - quicky rule (no BSSM lock nor state)
          NULL, 0, NULL);
        break;

      case IBAR_STATE_Sell:
      case IBAR_STATE_HotelDecomposition:
        /*tempMessage.action = ACTION_SELL_BUILDINGS;
        tempMessage.fromPlayer = DISPLAY_state.IBarCurrentPlayer;
        tempMessage.toPlayer = RULE_BANK_PLAYER;
        tempMessage.numberA = PropertyHit;
        tempMessage.numberB = FALSE;// Just one house (not the set)
        tempMessage.numberC = 0;
        tempMessage.numberD = 0;
        tempMessage.numberE = 0;
        tempMessage.stringA[0] = 0;
        tempMessage.binaryDataSizeA = 0,
        tempMessage.binaryDataHandleA = NULL;
        if( !IBAR_BSSM_LOCK::BSSM_GetLock( &tempMessage) )
          UDSOUND_Warning(); // Interface is already waiting for a lock!*/
        MESS_SendAction (ACTION_SELL_BUILDINGS, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
          PropertyHit, 0, 0, TRUE, // TRUE - quicky rule (no BSSM lock nor state)
          NULL, 0, NULL);
        break;

      case IBAR_STATE_PlaceHouse:
      case IBAR_STATE_PlaceHotel:
        MESS_SendAction( ACTION_BUY_HOUSE, DISPLAY_state.IBarCurrentPlayer, RULE_BANK_PLAYER,
          PropertyHit, 0, 0, 0, NULL, 0, NULL);
        break;
      }
    }

    // Player Button Handling
    if ( PlayerOrBankHit >= 0 )
    {
      if ( (DISPLAY_state.desired2DView == DISPLAY_SCREEN_Pselect)
       || (DISPLAY_state.desired2DView == DISPLAY_SCREEN_PselectRules) )
      {
        // Player setup
        if ( DISPLAY_state.IsPlayerVisible[PlayerOrBankHit] )
          UDPSEL_PlayerButtonClicked( PlayerOrBankHit );
      }
      else
      {
        // After setup, during real game
        if ( PlayerOrBankHit == DISPLAY_state.IBarLastRulePlayer )
        {
          DISPLAY_state.IBarStateTrackOn = TRUE;
        }
        else
        {
          DISPLAY_state.IBarStateTrackOn = FALSE;
          if ( PlayerOrBankHit == RULE_MAX_PLAYERS )
            UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayer );
          else if ( PlayerOrBankHit < RULE_MAX_PLAYERS && SlotIsALocalHumanPlayer[PlayerOrBankHit] )
            UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayer );
          else
            UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayerRemote );
        }
        DISPLAY_state.IBarCurrentPlayer = PlayerOrBankHit;
      }
    }
  }
  
  if( (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN) &&
    (UIMessagePntr->numberB >= UDIBAR_IBarTopButton) && (!DISPLAY_IsPropertyBarAvailable) &&
    (DISPLAY_IsIBarVisible) && (g_bStatusBarDisplayed) && (PropertyHit == -1))//&&
    //(!IBAR_BSSM_LOCK::BSSM_IslockRequested()) ) // While a request is pending, IBAR is locked.
  {
    // verify if any of the status bar buttons were pressed
    int result = UDStats_GetButtonIndex(UIMessagePntr, CATEGORY_BUTTONS_AREA );
    if(result > -1)
      UDIBar_ProcessStatsButtonPress(result);
    else
    {
      result = UDStats_GetButtonIndex(UIMessagePntr, SORT_BUTTONS_AREA );
      if(result > -1)
        UDIBar_ProcessStatsButtonPress(result+B_SORT_1);
    }
  }

  if( !g_bIsScreenUpdated &&
    (DISPLAY_IsIBarVisible) && (g_bStatusBarDisplayed) )//&&
    //(!IBAR_BSSM_LOCK::BSSM_IslockRequested()) ) // While a request is pending, IBAR is locked.
  {
    // verify if any of the status bar buttons were pressed
        UDIBar_ProcessStatsButtonPress( g_Button );
    }


  	// check for press on Options buttons
  if( (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN) &&
    (g_bOptionsButtonsOn) && (!DISPLAY_IsPropertyBarAvailable) &&
    (g_bOptionsButtonsDisplayed))//&&
    //(!IBAR_BSSM_LOCK::BSSM_IslockRequested()) ) // While a request is pending, IBAR is locked.
  {
    // verify if any of the status bar buttons were pressed
    int result = UDOPTIONS_GetButtonIndex(UIMessagePntr, OPTIONS_BUTTONS_AREA );
    if(result > -1)
	{
		// save the current player select phase and screen
	  if(last_playerselectphase == -1)
	  {
		last_playerselectphase = DISPLAY_state.currentPlayerSetupPhase;
		last_playerselectscreen = DISPLAY_state.current2DView;
		// remove the player select stuff from the screen
		//UDPSEL_EndObjectsAndSwitchPhase( udpsel_PHASE_NONE );
		// display the options background screen
		UDBOARD_SetBackdrop( DISPLAY_SCREEN_Options );		
	  }

      UDIBar_ProcessOptionsButtonPress(result);
	  
	}
	
  }

}


/*****************************************************************************
 * Standard UI subfunction - Process game message to player.
 */

void UDIBAR_ProcessMessageToPlayer( RULE_ActionArgumentsPointer NewMessage )
{
  int               iTemp;
  //char              NameString [RULE_MAX_PLAYER_NAME_LENGTH+2];
  //RULE_SquareType   SquareNo;
  //RULE_PropertySet  SquareSet;
  //char              TempString [2 * RULE_MAX_PLAYER_NAME_LENGTH + 160];
  FILE * fileptr;
  ACHAR tempStr1[100];
  int tempHouses;
#if !USA_VERSION
  RULE_ActionArgumentsRecord msg;
#endif

  // Debatable decision - this is primarily a flag for demo mode, here means 'dont sleep while AI playing' - but chat messages and such would pull us out of demo.
  DISPLAY_state.LE_TickCount_Last_Button_Click = LE_TIME_TickCount;

  switch (NewMessage->action)
  {
  case NOTIFY_END_TURN:
    UICurrentGameState.CurrentPlayer = (unsigned char) NewMessage->numberA;
    SetCurrentUIPlayerFromPlayerNumber (UICurrentGameState.CurrentPlayer);
    DISPLAY_state.FlashCurrentToken = TRUE;
    if( CurrentUIPlayer < RULE_MAX_PLAYERS )
    {// local player
      UDIBAR_setIBarRulesState( IBAR_STATE_DoneTurn, UICurrentGameState.CurrentPlayer );
      UDIBAR_setDialog( DISPLAY_Dialog_None );
    } else
    {
      UDIBAR_setIBarRulesState( IBAR_STATE_DoneTurn, UICurrentGameState.CurrentPlayer );
//      UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, UICurrentGameState.CurrentPlayer );
      UDIBAR_setDialog( DISPLAY_Dialog_None );
    }

    DISPLAY_showAll;
    //ShowDialogBox (DK_DONE_TURN);
    //if (hdlgCurrentDialogBox != NULL)
    //{
    //  wcstombs (TempString,
    //    UICurrentGameState.Players[NewMessage->numberA].name, sizeof (TempString));
    //  strcat( TempString, "'s turn is over." );
     // SetDlgItemText (hdlgCurrentDialogBox, IDC_PLAYER_TURN_OVER, TempString);
     // EnableWindow (GetDlgItem (hdlgCurrentDialogBox, ID_TURN_DONE_OK), CurrentUIPlayer < RULE_MAX_PLAYERS);
    //}

    break;

  case NOTIFY_START_TURN:
    UDPENNY_NextPlayer(NewMessage->numberA);        // Mike: Changed this so it passes in the player number not the token
//    UDPENNY_NextPlayer( UICurrentGameState.Players[NewMessage->numberA].token );
    DISPLAY_state.desired2DCamera = UDPIECES_PickCameraFor3Squares( UICurrentGameState.Players[NewMessage->numberA].currentSquare );
    DISPLAY_state.IdleToCenterMoveStatus = 1; // Flag the status as 'lock ready'
    gameQueueLock(); // Locked for token resting to center idle animation.

    GameInProgress = TRUE;
    UICurrentGameState.CurrentPlayer = (unsigned char) NewMessage->numberA;
    SetCurrentUIPlayerFromPlayerNumber (UICurrentGameState.CurrentPlayer);
    DISPLAY_state.FlashCurrentToken = TRUE;

    UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, UICurrentGameState.CurrentPlayer );

    // this is where we would stop recording for a player's turn
    if(SaveAccountInfo.turn_number == 0)
    {
      fileptr = fopen( "acchist.txt", "wb" );
      if (fileptr != NULL)
      {
        fclose( fileptr );
      }
    }
    // insert the code to keep tabs on player account history here
    // increment the number of turns
    // Output the string
    //SaveAccountInfo.player_number = UICurrentGameState.CurrentPlayer;
    SaveAccountInfo.turn_number++;
    //SaveAccountInfo.turn_description[0] = 0;

    break;

  case NOTIFY_PLEASE_ROLL_DICE:
    // NOTE: a Start turn camera move may still be waiting - can we wait for it somehow?
    SetCurrentUIPlayerFromPlayerNumber ((RULE_PlayerNumber) NewMessage->numberA);
    DISPLAY_state.desired2DCamera = UDBOARD_SelectAppropriateView( VIEW_ROLLDICE, 0 );// view type, chance of changing if view is already acceptable.

    UDIBAR_setIBarRulesState( IBAR_STATE_StartTurn, NewMessage->numberA );
    DISPLAY_state.DiceRollNotification = TRUE;
/*    if( CurrentUIPlayer < RULE_MAX_PLAYERS )
    {// local player
      UDIBAR_setIBarRulesState( IBAR_STATE_StartTurn, NewMessage->numberA );
      //UDIBAR_setDialog( DISPLAY_Dialog_Roll );
      UDIBAR_setDialog( DISPLAY_Dialog_None );
    } else
    {
      //UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, NewMessage->numberA );
      UDIBAR_setIBarRulesState( IBAR_STATE_StartTurn, NewMessage->numberA );
      UDIBAR_setDialog( DISPLAY_Dialog_None );
    }*/

    DISPLAY_showAll;
    GameInProgress = TRUE;
    GamePaused = FALSE;

    //wcstombs (TempString,  UICurrentGameState.Players[NewMessage->numberA].name, sizeof (TempString));
    break;

  case NOTIFY_DICE_ROLLED:
    UICurrentGameState.Dice[0] = (unsigned char) NewMessage->numberA;
    UICurrentGameState.Dice[1] = (unsigned char) NewMessage->numberB;
    UDIBAR_setDialog( DISPLAY_Dialog_None );
    UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, DISPLAY_state.IBarCurrentPlayer );//Dont change player
    DISPLAY_showAll;

    RolledPlayer = (RULE_PlayerNumber) NewMessage->numberC;

    // Prep a dice animation, stop stack processing (the move is up next, dont let it process until dice anim is done)
    DISPLAY_state.QueueLockedForDiceAnim = TRUE; // Skipping a check that it might already be true, wont happen.
    DISPLAY_state.QueueLockTime = LE_TIME_TickCount;
    gameQueueLock();


#if SAVE_DICE_ROLLS_IN_A_FILE
    if (DiceRollFile == NULL && !DiceRollFileOpenHasFailedSoDoNotTryAgain)
    {
      DiceRollFile = fopen ("DiceRollHistory.txt", "wt");
      if (DiceRollFile == NULL)
      {
        DiceRollFileOpenHasFailedSoDoNotTryAgain = TRUE;
        LE_ERROR_ErrorMsg ("NOTIFY_DICE_ROLLED: can't open dice roll history file for writing.");
      }
      else
        fprintf (DiceRollFile,
        "Dice Roll History for Monopoly\nPlayer\tDie1\tDie2\tTotal\tDoubles\n");
    }

    /* Keep some statistics. */

    TotalDie1 += UICurrentGameState.Dice[0];
    TotalDie2 += UICurrentGameState.Dice[1];
    TotalDice += UICurrentGameState.Dice[0] + UICurrentGameState.Dice[1];
    NumberOfRollsMade++;
    if (UICurrentGameState.Dice[0] == UICurrentGameState.Dice[1])
      NumberOfDoublesMade++;

    if (DiceRollFile != NULL)
      fprintf (DiceRollFile, "%d\t%d\t%d\t%d\t%d\n",
      (int) RolledPlayer,
      (int) UICurrentGameState.Dice[0],
      (int) UICurrentGameState.Dice[1],
      (int) (UICurrentGameState.Dice[0] + UICurrentGameState.Dice[1]),
      (int) (UICurrentGameState.Dice[0] == UICurrentGameState.Dice[1]));
#endif /* SAVE_DICE_ROLLS_IN_A_FILE */

    // let's try inserting the bank account status part here
    // save the dice roll
    //ACHAR tempStr1[50], tempStr2[50];
    /*Astrcpy(tempStr1, UICurrentGameState.Players[RolledPlayer].name);
    Astrcat(tempStr1, A_T(" rolled a "));
    Asprintf(tempStr2, A_T("%i"), UICurrentGameState.Dice[0]);
    Astrcat(tempStr1, tempStr2);
    Astrcat(tempStr1, A_T(" and a "));
    Asprintf(tempStr2, A_T("%i"), UICurrentGameState.Dice[1]);
    Astrcat(tempStr1, tempStr2);
    Astrcat(tempStr1, A_T(". "));

    Astrcat(SaveAccountInfo.turn_description, tempStr1);*/
    break;

  case NOTIFY_MOVE_FORWARDS:
  case NOTIFY_MOVE_BACKWARDS:
  case NOTIFY_JUMP_TO_SQUARE:
    UDPIECES_PlanMoveAnim( NewMessage->numberC, NewMessage->action, 
      UICurrentGameState.Players[NewMessage->numberC].currentSquare, NewMessage->numberA ); // Will plan the users move
    if( NewMessage->numberA != 40 )
      UICurrentGameState.Players[NewMessage->numberC].currentSquare = NewMessage->numberA;
    else// If its jail, let the anim register the move - do the sound too.
      UDPENNY_GoToJail();
    DISPLAY_showAll;

    // let's try inserting the bank account status part here
    // save the square landed on
    /*Astrcpy(tempStr1, A_T(" Landed on "));
    Astrcat(tempStr1, PropertyNames[NewMessage->numberA]);
    Astrcat(tempStr1, A_T(". "));
    Astrcat(SaveAccountInfo.turn_description, tempStr1);*/
    break;

  case NOTIFY_BUY_OR_AUCTION_DECISION:
    UDPENNY_BuyOrAuction( (RULE_PlayerNumber) NewMessage->numberA );
    SetCurrentUIPlayerFromPlayerNumber ((RULE_PlayerNumber) NewMessage->numberA);
    //UDIBAR_setDialog( DISPLAY_Dialog_Properties00 + DISPLAY_propertyToOwnablePropertyConversion(NewMessage->numberB) );
#if USA_VERSION
    DISPLAY_state.PropertyBuyAuctionDesired = LED_IFT( DAT_LANG2, TAB_iyf00x00 ) + DISPLAY_propertyToOwnablePropertyConversion(NewMessage->numberB)
      + DISPLAY_DEEDS_PER_SET*max(DISPLAY_state.city, 0);
#else
    DISPLAY_state.PropertyBuyAuctionDesired = UDIBAR_Deeds[DISPLAY_propertyToOwnablePropertyConversion(NewMessage->numberB)];
#endif
    if( CurrentUIPlayer < RULE_MAX_PLAYERS )
    {// local player
      UDIBAR_setIBarRulesState( IBAR_STATE_BuyAuction, NewMessage->numberA );
    } else
    {
      UDIBAR_setIBarRulesState( IBAR_STATE_BuyAuction, NewMessage->numberA );
//      UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, NewMessage->numberA );
    }
    DISPLAY_showAll;
    //wcstombs (TempString, UICurrentGameState.Players[NewMessage->numberA].
    //  name, sizeof (TempString));
    //strcat (TempString, "'s Decision");
    //SetWindowText (hdlgCurrentDialogBox, TempString);

    //wcstombs (TempString, RULE_SquarePredefinedInfo[NewMessage->numberB].
    //  squareName, sizeof (TempString));
    //wsprintf (TempString + strlen (TempString), ": $%d",
    //  (int) NewMessage->numberC);
    //SetDlgItemText (hdlgCurrentDialogBox, IDC_BUYAUCTION_TEXT, TempString);

    //EnableWindow (GetDlgItem (hdlgCurrentDialogBox, IDC_BUYAUCTION_BUY), CurrentUIPlayer < RULE_MAX_PLAYERS);
    //EnableWindow (GetDlgItem (hdlgCurrentDialogBox, IDC_BUYAUCTION_AUCTION), CurrentUIPlayer < RULE_MAX_PLAYERS);
    break;

  case NOTIFY_PASSED_GO:
    if( UICurrentGameState.Players[NewMessage->numberA].currentSquare != 0 )
    {
      //TRACE( "Passed go\n" );
      //UDPENNY_PassedGo(); // Land on square handles this if we land on go.
    }
    break;

  case NOTIFY_CASH_AMOUNT:
    UICurrentGameState.Players[NewMessage->numberA].cash = NewMessage->numberC;
        if(fStatusScreen == PLAYER_SCREEN)
        {
            g_bIsScreenUpdated = FALSE;
            g_Button = last_sort_result + B_SORT_1;
        }
        else if((fStatusScreen == DEED_SCREEN) && ((last_sort_result + B_SORT_1) == B_SORT_4))
        {
            g_bIsScreenUpdated = FALSE;
            g_Button = B_SORT_4;
        }
    DISPLAY_showAll;
    break;

  case NOTIFY_PLEASE_PAY:
    SetCurrentUIPlayerFromPlayerNumber ((RULE_PlayerNumber) NewMessage->numberA);
    //ShowDialogBox (DK_PLEASE_PAY);
    if( CurrentUIPlayer < RULE_MAX_PLAYERS )
    {// local player
      UDIBAR_setIBarRulesState( IBAR_STATE_RaiseMoney, NewMessage->numberA );
    } else
    {
      UDIBAR_setIBarRulesState( IBAR_STATE_RaiseMoney, NewMessage->numberA );
    }

    // This gets tedious each time it is called - time it so as not to repeat the message excessively.
    {// FIXME it should repeat after time since sometimes the mode is confusing
      if( LE_TIME_TickCount > LastSoundPlayedAt[NewMessage->numberA] + 40 * 60 && DISPLAY_IsIBarVisible)
      {
        if( DISPLAY_state.SoundToPlayPostLock == LED_EI )
        {
          TRACE("Raise cash from notification");
          UDPENNY_PennybagsGeneral( PB_RaisingMoneySuggestion, Sound_WaitForAnyOldSoundThenPlay );
          LastSoundPlayedAt[NewMessage->numberA] = LE_TIME_TickCount;
        }
      }
    }

    DISPLAY_state.IBarRaiseTotalCashNeeded = NewMessage->numberC;
    DISPLAY_state.ButtonBarForceUpdate = TRUE;
    LE_GRAFIX_ColorArea( DISPLAY_state.ButtonBarMessage, 0, 0, BUTTONBARMESSAGEWITDTH, 20, LE_GRAFIX_MakeColorRef( 0, 255, 0 ) );
    LE_FONTS_SetUnderline( FALSE );
    LE_FONTS_SetItalic( FALSE );
    LE_FONTS_SetSize( 12 );
    LE_FONTS_SetWeight( 900 );
    {
      ACHAR                           myBuffer[40];
      UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( -DISPLAY_state.IBarRaiseTotalCashNeeded, 
        DISPLAY_state.system, myBuffer, TRUE);
      // Print cash 
      LE_FONTS_Print(DISPLAY_state.ButtonBarMessage,
        BUTTONBARMESSAGEWITDTH - LE_FONTS_GetStringWidth(myBuffer), 0, 
        LE_GRAFIX_MakeColorRef( 255, 64, 64 ), myBuffer );
    }
    
    DISPLAY_state.IBarRaiseCashCanBankrupt = NewMessage->numberE;
    //UDIBAR_setDialog( DISPLAY_Dialog_GoBankrupt );
    DISPLAY_showAll;

    //if (hdlgCurrentDialogBox != NULL)
    //{
    /* Change the dialog box title and contents to reflect the debt details. */

    /*wcstombs (NameString, UICurrentGameState.Players[NewMessage->numberA].name,
      sizeof (NameString));
    strcpy (TempString, NameString);
    strcat (TempString, "'s Debt");
    //SetWindowText (hdlgCurrentDialogBox, TempString);

    wsprintf (TempString, "%s owes $%d to ", NameString, NewMessage->numberC);
    if (NewMessage->numberB >= RULE_MAX_PLAYERS)
      wcstombs (NameString, LANG_GetTextMessage (TMN_BANK_NAME), sizeof (NameString));
    else
      wcstombs (NameString, UICurrentGameState.Players[NewMessage->numberB].name,
      sizeof (NameString));
    wsprintf (TempString + strlen (TempString) /* Append to string ,
      "%s.  Still need to raise $%d, or go bankrupt.", NameString,
      NewMessage->numberD);


    //wcstombs ( TempString2, TempString, strlen (TempString) );
    DISPLAY_setMessageBox( TempString );
    DISPLAY_showAll;*/

    //SetDlgItemText (hdlgCurrentDialogBox, IDC_PLEASE_PAY_TEXT, TempString);

    /* Turn on the bankrupt button only if we are playing the bankrupt player. */

    //EnableWindow (GetDlgItem (hdlgCurrentDialogBox, IDC_PLEASE_PAY_BANKRUPT),
    //  CurrentUIPlayer < RULE_MAX_PLAYERS);
    //}
    break;

  case NOTIFY_JAIL_EXIT_CHOICE:
    SetCurrentUIPlayerFromPlayerNumber ((RULE_PlayerNumber) NewMessage->numberA);
    DISPLAY_state.desired2DCamera = UDBOARD_SelectAppropriateView( VIEW_JAILCHOICE, 1 );// view type, chance of changing if view is already acceptable.

    //if( CurrentUIPlayer < RULE_MAX_PLAYERS )
    //{// local player
      if( NewMessage->numberB )
      { // can roll out
        if( SlotIsALocalHumanPlayer[NewMessage->numberA] )
        {
#if !USA_VERSION
          UDPENNY_PennybagsSpecific( WAV_pb235, Sound_WaitForAnyOldSoundThenPlay );
#else
          UDPENNY_PennybagsGeneral( PB_JailPayMoneyOrTryForDoubles, Sound_WaitForAnyOldSoundThenPlay );
#endif
        }
        if( NewMessage->numberD ) // have card
          UDIBAR_setIBarRulesState( IBAR_STATE_JailExitPCR, NewMessage->numberA );
        else
          UDIBAR_setIBarRulesState( IBAR_STATE_JailExitPXR, NewMessage->numberA );
      } else
      { // can't roll out
        if( NewMessage->numberD ) // have card
          UDIBAR_setIBarRulesState( IBAR_STATE_JailExitPCX, NewMessage->numberA );
        else // can only pay
          UDIBAR_setIBarRulesState( IBAR_STATE_JailExitPXX, NewMessage->numberA );
      }
    //} else
    //{
    //  UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, NewMessage->numberA );
    //}
    DISPLAY_showAll;
    break;

  case NOTIFY_PICKED_UP_CARD:
    if( SlotIsALocalHumanPlayer[NewMessage->numberA] && NewMessage->numberA < RULE_MAX_PLAYERS )
    {// local player
      UDIBAR_setIBarRulesState( IBAR_STATE_ViewingCard, NewMessage->numberA );
    } else
    {
      UDIBAR_setIBarRulesState( IBAR_STATE_ViewingCard, NewMessage->numberA );
//      UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, NewMessage->numberA );
    }
    if( NewMessage->numberB == 0 ) // CHANCE_DECK
      //UDIBAR_setDialog( DISPLAY_Dialog_Chance00 + NewMessage->numberC - CHANCE_GO_DIRECTLY_TO_GO); // subtract off the first card.
      DISPLAY_state.ChanceCommChestDesired = NewMessage->numberC - CHANCE_GO_DIRECTLY_TO_GO; // Convert to 0-31 range from rule engine amounts.
    else  // COMMUNITY_DECK
      //UDIBAR_setDialog( DISPLAY_Dialog_CommChest00 + NewMessage->numberC - COMMUNITY_GET_100_FROM_BANK_1); // subtract off the first card.
      DISPLAY_state.ChanceCommChestDesired = NewMessage->numberC  - COMMUNITY_GET_100_FROM_BANK_1 + 16;

    DISPLAY_showAll;

    SetCurrentUIPlayerFromPlayerNumber ((RULE_PlayerNumber) NewMessage->numberA);

    //wcstombs (TempString, UICurrentGameState.Players[NewMessage->numberA].name,
    //  sizeof (TempString));
    //strcat (TempString, "'s Card");

    // let's try inserting the bank account status part here
    // save the card picked up
    /*switch(NewMessage->numberC)
    {
    case CHANCE_GET_150_FROM_BANK:
    case CHANCE_GET_50_FROM_BANK:
    case COMMUNITY_GET_100_FROM_BANK_1:
    case COMMUNITY_GET_100_FROM_BANK_2:
    case COMMUNITY_GET_100_FROM_BANK_3:
    case COMMUNITY_GET_10_FROM_BANK:
    case COMMUNITY_GET_200_FROM_BANK:
    case COMMUNITY_GET_20_FROM_BANK:
    case COMMUNITY_GET_45_FROM_BANK:
    case COMMUNITY_GET_25_FROM_BANK:
                    Astrcpy(tempStr1, A_T(" Picked up "));
                    if(NewMessage->numberB == 0) // chance_deck
                    {
                      Astrcat(tempStr1, CardNames[NewMessage->numberC - CHANCE_FIRST]);
                      Astrcat(tempStr1, A_T(" card from the CHANCE deck. "));
                    }
                    else
                    {
                      Astrcat(tempStr1, CardNames[NewMessage->numberC - COMMUNITY_FIRST + 16]);
                      Astrcat(tempStr1, A_T(" card from the COMMUNITY CHEST deck. "));
                    }

                    Astrcpy(SaveAccountInfo.turn_description, tempStr1);
                    SaveAccountInfo.player_number = DISPLAY_state.IBarCurrentPlayer;
                    fileptr = fopen( "acchist.txt", "ab" );
                    if (fileptr != NULL)
                    {
                      fwrite( &SaveAccountInfo, sizeof(SaveAccountInfo), 1, fileptr );
                      fclose( fileptr );
                    }
                    break;
    default: break;
    }*/

    break;

  case NOTIFY_PUT_AWAY_CARD:
    //if( NewMessage->numberB == 0 ) // CHANCE_DECK
    UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, NewMessage->numberA );
    UDIBAR_setDialog( DISPLAY_Dialog_None );
    DISPLAY_showAll;
    break;

  case NOTIFY_SQUARE_OWNERSHIP:
    // Shut down the cash message (may have just left raise cash phase)
    //DISPLAY_state.ButtonBarWantedTime = 0;

    UICurrentGameState.Squares[NewMessage->numberA].owner = (RULE_PlayerNumber) NewMessage->numberB;
    DISPLAY_showAll;
        // owning a square will affect the deeds status screen on every sort except the price sort
        //if(((last_sort_result+B_SORT_1) > B_SORT_1) && (fStatusScreen == DEED_SCREEN))

    // do not display account history twice for same thing
    // only save for actual ownership
    if(NewMessage->numberB < RULE_MAX_PLAYERS)
        {
            g_bIsScreenUpdated = FALSE;
            g_Button = last_sort_result + B_SORT_1;

      // let's try inserting the bank account status part here
      // save the square bought, either directly or auctioned
#if USA_VERSION
      Astrcpy(tempStr1, A_T("Purchased property "));
      Astrcat(tempStr1, RULE_SquarePredefinedInfo[NewMessage->numberA].squareName);
      Astrcat(tempStr1, A_T(". "));
#else
      msg.numberA = TMN_PURCHASED_PROPERTY;
      msg.numberD = NewMessage->numberA;
      FormatErrorNotification(&msg, tempStr1);
#endif
      Astrcpy(SaveAccountInfo.turn_description, tempStr1);
      SaveAccountInfo.player_number = NewMessage->numberB;
      fileptr = fopen( "acchist.txt", "ab" );
      if (fileptr != NULL)
      {
        fwrite( &SaveAccountInfo, sizeof(SaveAccountInfo), 1, fileptr );
        fclose( fileptr );
      }
    }

    if (RunFastWithNoDisplay)
      break;
    break;

  case NOTIFY_SQUARE_MORTGAGE:
    UICurrentGameState.Squares[NewMessage->numberA].mortgaged = NewMessage->numberB;
    if( DISPLAY_state.IBarCurrentState == IBAR_STATE_DeedActive )
      UDIBAR_DeedStateCalcButtons();
    // Sound/cam notification
    if( NewMessage->numberB )
    {// Mortgaged
      UDSOUND_HandleBSSMSoundNCam( UICurrentGameState.Squares[NewMessage->numberA].owner, NewMessage->numberA, 2 );
    } else
    {
      UDSOUND_HandleBSSMSoundNCam( UICurrentGameState.Squares[NewMessage->numberA].owner, NewMessage->numberA, 3 );
    }

    DISPLAY_showAll;

    // let's try inserting the bank account status part here
    // save the mortgage/unmortgage step
#if USA_VERSION
    if(NewMessage->numberB)
      Astrcpy(tempStr1, A_T("Mortgaged property "));
    else
      Astrcpy(tempStr1, A_T("Unmortgaged property "));
    Astrcat(tempStr1, RULE_SquarePredefinedInfo[NewMessage->numberA].squareName);
    Astrcat(tempStr1, A_T(". "));
#else
    if(NewMessage->numberB)
      msg.numberA = TMN_MORTGAGED_PROPERTY_S;
    else
      msg.numberA = TMN_UNMORTGAGED_PROPERTY_S;
    msg.numberD = NewMessage->numberA;
    FormatErrorNotification(&msg, tempStr1);
#endif
    Astrcpy(SaveAccountInfo.turn_description, tempStr1);
    SaveAccountInfo.player_number = DISPLAY_state.IBarCurrentPlayer;
    fileptr = fopen( "acchist.txt", "ab" );
    if (fileptr != NULL)
    {
      fwrite( &SaveAccountInfo, sizeof(SaveAccountInfo), 1, fileptr );
      fclose( fileptr );
    }

    //if (!RunFastWithNoDisplay)
    //  InvalidateRect (hwndMain, GameSquareRects + NewMessage->numberA, FALSE);
    break;

  case NOTIFY_FREE_UNMORTGAGING:
    UDIBAR_setDialog( DISPLAY_Dialog_None );
    SetCurrentUIPlayerFromPlayerNumber ((RULE_PlayerNumber) NewMessage->numberA);
    if (NewMessage->numberB == 0)  // Nothing being unmortgaged.
      break;
    DISPLAY_state.IBarStateTrackOn = TRUE;
    if( NewMessage->numberA < RULE_MAX_PLAYERS ) // Checking bug - an AI interrupted my viewing and the Nothing state was active.  it offered a trade and on rejection proper bar appeared - probably this UI variable is the culprit.
    {// local player
      UDIBAR_setIBarRulesState( IBAR_STATE_FreeUnmortgage, NewMessage->numberA );
    } else
    {
      UDIBAR_setIBarRulesState( IBAR_STATE_FreeUnmortgage, NewMessage->numberA );
//      UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, NewMessage->numberA );
    }
    DISPLAY_state.TradeFreeUnmortgageSet = NewMessage->numberB;


    // Old leftovers - anything we wanna keep?
    /*wcstombs (TempString, UICurrentGameState.Players[NewMessage->numberA].name,
      sizeof (TempString));
    strcat (TempString, " can unmortgage these properties for no additional interest charge: ");
    SquareSet = NewMessage->numberB;
    while (SquareSet != 0 &&
      strlen (TempString) < sizeof (TempString) - RULE_MAX_SQUARE_NAME_LENGTH)
    {
      SquareNo = RULE_BitSetToProperty (SquareSet);
      SquareSet &= ~RULE_PropertyToBitSet (SquareNo);
      wcstombs (TempString + strlen (TempString), RULE_SquarePredefinedInfo[SquareNo].squareName, sizeof (TempString) - strlen (TempString));
      if (SquareSet != 0)
        strcat (TempString, ", ");
    }*/
    //SetDlgItemText (hdlgCurrentDialogBox, IDC_FREE_MORTGAGE_TEXT, TempString);

    //EnableWindow (GetDlgItem (hdlgCurrentDialogBox, IDOK), CurrentUIPlayer < RULE_MAX_PLAYERS);
    //}
    break;

  case NOTIFY_SQUARE_HOUSES:
    tempHouses = UICurrentGameState.Squares[NewMessage->numberA].houses;
    UICurrentGameState.Squares[NewMessage->numberA].houses = (unsigned char) NewMessage->numberB;
    UICurrentGameState.GameOptions.housesPerHotel = (unsigned char) NewMessage->numberC;

    // Notify sound/cam system
    if( tempHouses < UICurrentGameState.Squares[NewMessage->numberA].houses )
    {// Bought house
      UDSOUND_HandleBSSMSoundNCam( UICurrentGameState.Squares[NewMessage->numberA].owner, NewMessage->numberA, 0 );
    } else
    {// Sold
      UDSOUND_HandleBSSMSoundNCam( UICurrentGameState.Squares[NewMessage->numberA].owner, NewMessage->numberA, 1 );
    }

    if( DISPLAY_state.IBarCurrentState == IBAR_STATE_DeedActive )
      UDIBAR_DeedStateCalcButtons();

        // adding a house adds to the rent amount therefore update the status screen if we are
        // sorting by rent
        //if(((last_sort_result+B_SORT_1) == B_SORT_3) && (fStatusScreen == DEED_SCREEN))
        {
            g_bIsScreenUpdated = FALSE;
            g_Button = last_sort_result + B_SORT_1;
        }

    // let's try inserting the bank account status part here
    // check to see if house was sold or bought
    if(tempHouses > NewMessage->numberB)
    {
      // save the selling of a house or hotel
#if USA_VERSION
      if(tempHouses == NewMessage->numberC)
        Astrcpy(tempStr1, A_T("Sold a hotel. "));
      else
        Astrcpy(tempStr1, A_T("Sold a house. "));
#else
      if(tempHouses == NewMessage->numberC)
        Astrcpy(tempStr1, LANG_GetTextMessage(TMN_SOLD_A_HOTEL));
      else
        Astrcpy(tempStr1, LANG_GetTextMessage(TMN_SOLD_A_HOUSE));
#endif
    }
    else
    {
      // save the placement of a house or hotel
#if USA_VERSION
      if(NewMessage->numberB == NewMessage->numberC)
        Astrcpy(tempStr1, A_T("Purchased a hotel. "));
      else
        Astrcpy(tempStr1, A_T("Purchased a house. "));
#else
      if(NewMessage->numberB == NewMessage->numberC)
        Astrcpy(tempStr1, LANG_GetTextMessage(TMN_PURCHASED_A_HOTEL));
      else
        Astrcpy(tempStr1, LANG_GetTextMessage(TMN_PURCHASED_A_HOUSE));
#endif
    }
    //Astrcat(tempStr1, PropertyNames[NewMessage->numberA]);
    //Astrcat(tempStr1, A_T(". "));
    Astrcpy(SaveAccountInfo.turn_description, tempStr1);
    SaveAccountInfo.player_number = DISPLAY_state.IBarCurrentPlayer;
    fileptr = fopen( "acchist.txt", "ab" );
    if (fileptr != NULL)
    {
      fwrite( &SaveAccountInfo, sizeof(SaveAccountInfo), 1, fileptr );
      fclose( fileptr );
    }

    DISPLAY_showAll;
    break;

  case NOTIFY_JAIL_CARD_OWNERSHIP:
    UICurrentGameState.Cards[NewMessage->numberB].jailOwner = (RULE_PlayerNumber) NewMessage->numberA;

    // let's try inserting the bank account status part here
    // save the gain or loss of jail card
    /*if(RULE_NOBODY_PLAYER != NewMessage->numberA)
      Astrcpy(tempStr1, A_T(" Received a jail card. "));
    else
      Astrcpy(tempStr1, A_T(" Used a jail card. "));
    Astrcat(SaveAccountInfo.turn_description, tempStr1);*/

    break;

  case NOTIFY_HOUSING_SHORTAGE:
    PlayersAllowedInAuction = (RULE_PlayerSet) NewMessage->numberE;
    // Remove the original buyer from the auction set - it screws up selection of current player to show Auction button to.
    PlayersAllowedInAuction -= (1 << NewMessage->numberA);

    iTemp = 0;
    while( iTemp < UICurrentGameState.NumberOfPlayers )
    { // FIXME - First local human given control, but each local player should get the button.  Save it and add logic in SHOW & ProcessMessage.
      if( (PlayersAllowedInAuction & (1 << iTemp)) &&
        SlotIsALocalHumanPlayer[iTemp] )
        break;
      iTemp++;
    }
    if( iTemp == UICurrentGameState.NumberOfPlayers )
    { // No local players can initiate bid (can't place house/hotel)
      // Special handling - pick the first (is non-local to be here) in the playerset to 'watch'
      iTemp = RULE_NOBODY_PLAYER;
      for (iTemp = 0; iTemp < UICurrentGameState.NumberOfPlayers; iTemp++)
      {
        if( PlayersAllowedInAuction & (1 << iTemp) )
        {
          DISPLAY_state.IBarStateTrackOn = TRUE;
          if( NewMessage->numberC < 0 ) // Negative is number of houses
            UDIBAR_setIBarRulesState( IBAR_STATE_HousingShort, iTemp );
          else
            UDIBAR_setIBarRulesState( IBAR_STATE_HotelShort, iTemp );
          //UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, iTemp ); old
          break;
        }
      }
    } else
    { // Set up the bar.  Turn tracking on if off, this is time dependant.
      DISPLAY_state.IBarStateTrackOn = TRUE;
      if( NewMessage->numberC < 0 ) // Negative is number of houses
        UDIBAR_setIBarRulesState( IBAR_STATE_HousingShort, iTemp );
      else
        UDIBAR_setIBarRulesState( IBAR_STATE_HotelShort, iTemp );
      // Sound
      static lastCount = 2; // we get duplicate first messages
      if( NewMessage->numberD == 0 )// There is a countdown - only give instructions the first message.
      {
        if( lastCount != NewMessage->numberD )
          UDPENNY_PennybagsGeneral( PB_HousingShortage_NotFirstTime, Sound_WaitForAnyOldSoundThenPlay );
      }
      lastCount = NewMessage->numberD;
    }

    /*wcstombs (NameString, UICurrentGameState.Players[NewMessage->numberA].name,
      sizeof (NameString));

    wsprintf (TempString, "%s wants to buy a %s on ",
      NameString, NewMessage->numberC > 0 ? "hotel" : "house");
    wcstombs (TempString + strlen (TempString),
      RULE_SquarePredefinedInfo[NewMessage->numberB].squareName,
      sizeof (TempString) - strlen (TempString));
    wsprintf (TempString + strlen (TempString), " when there are only %d left.",
      (int) labs (NewMessage->numberC));

    //SetDlgItemText (hdlgCurrentDialogBox, IDC_SHORTAGE_TEXT, TempString);

    wsprintf (TempString, "Building Shortage - Going %s",
      (NewMessage->numberD == 0) ? "Soon" :
    (NewMessage->numberD == 1) ? "Once" :
    (NewMessage->numberD == 2) ? "Twice" : "Gone");*/

    //SetWindowText (hdlgCurrentDialogBox, TempString);
    break;

  case NOTIFY_FLAT_OR_FRACTION_TAX_DECISION:
    SetCurrentUIPlayerFromPlayerNumber ((RULE_PlayerNumber) NewMessage->numberA);
    if( CurrentUIPlayer < RULE_MAX_PLAYERS )
    {// local player
      UDIBAR_setIBarRulesState( IBAR_STATE_TaxDecision, NewMessage->numberA );
    } else
    {
      UDIBAR_setIBarRulesState( IBAR_STATE_TaxDecision, NewMessage->numberA );
//      UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, NewMessage->numberA );
    }
    DISPLAY_showAll;

    /*wcstombs (TempString, UICurrentGameState.Players[NewMessage->numberA].
      name, sizeof (TempString));
    strcat (TempString, "'s Decision");
    //SetWindowText (hdlgCurrentDialogBox, TempString);

    wsprintf (TempString, "The flat tax is $%d while the percentage tax is "
      "%d%% of your total wealth.  Which one do you want to pay?",
      (int) NewMessage->numberB, (int) NewMessage->numberC);*/
    break;

  case NOTIFY_PLACE_BUILDING:
    SetCurrentUIPlayerFromPlayerNumber ((RULE_PlayerNumber) NewMessage->numberA);
    //if( NewMessage->numberA < RULE_MAX_PLAYERS )
    //{// local player
      if( NewMessage->numberB < 0 ) // house
        UDIBAR_setIBarRulesState( IBAR_STATE_PlaceHouse, NewMessage->numberA );
      else // hotel
        UDIBAR_setIBarRulesState( IBAR_STATE_PlaceHotel, NewMessage->numberA );
    //} else
    //{
    //  UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, NewMessage->numberA );
    //}
    DISPLAY_state.AuctionPlaceBuildingSet = NewMessage->numberC;


    /*wcstombs (TempString, UICurrentGameState.Players[NewMessage->numberA].
      name, sizeof (TempString));
    strcat (TempString, ", please click on the square where you want your new ");
    if (NewMessage->numberB <= 0)
      strcat (TempString, "house.");
    else
      strcat (TempString, "hotel.");
    //SetDlgItemText (hdlgCurrentDialogBox, IDC_PLACE_BUILDING_PROMPT, TempString);*/
    break;

  case NOTIFY_DECOMPOSE_SALE:
    SetCurrentUIPlayerFromPlayerNumber ((RULE_PlayerNumber) NewMessage->numberA);
    UDIBAR_setIBarRulesState( IBAR_STATE_HotelDecomposition, NewMessage->numberA );

    DISPLAY_state.ButtonBarForceUpdate = TRUE;
    LE_GRAFIX_ColorArea( DISPLAY_state.ButtonBarMessage, 0, 0, BUTTONBARMESSAGEWITDTH, 20, LE_GRAFIX_MakeColorRef( 0, 255, 0 ) );
    LE_FONTS_SetUnderline( FALSE );
    LE_FONTS_SetItalic( FALSE );
    LE_FONTS_SetSize( 12 );
    LE_FONTS_SetWeight( 900 );
    {
      ACHAR                           myBuffer[40];//, myB2[40];
      Asprintf(myBuffer,A_T("%d"), -NewMessage->numberB);
      // Print cash 
#if USA_VERSION
      Astrcat( myBuffer, A_T(" houses") );
#else
#endif
      LE_FONTS_Print(DISPLAY_state.ButtonBarMessage,
        BUTTONBARMESSAGEWITDTH - LE_FONTS_GetStringWidth(myBuffer), 0, 
        //LE_GRAFIX_MakeColorRef( 32, 255, 32 ), myBuffer );
        LE_GRAFIX_MakeColorRef( 255, 0, 0 ), myBuffer );
    }
    //ShowDialogBox (DK_DECOMPOSE_HOTEL);
    //if (hdlgCurrentDialogBox != NULL)
    //{
    /*wcstombs (TempString, UICurrentGameState.Players[NewMessage->numberA].
      name, sizeof (TempString));
    wsprintf (TempString + strlen (TempString), ", click on squares to sell "
      "%d more houses.  You need to do this to decompose your hotel without "
      "using more than the available houses.", (int) NewMessage->numberB);
    //SetDlgItemText (hdlgCurrentDialogBox, IDC_DECOMPOSE_PROMPT, TempString);
    //}*/
    break;

  case NOTIFY_PLAYER_BUYSELLMORT:
    // FIXME Syntax saved for later, no UI facility now for sell all.
    //MESS_SendAction (ACTION_SELL_BUILDINGS, CurrentUIPlayer, RULE_BANK_PLAYER,
    //  property // Square number
    //  , TRUE // All one
    //  , 0, 0, NULL, 0, NULL);

    SetCurrentUIPlayerFromPlayerNumber ((RULE_PlayerNumber) NewMessage->numberA);

    // New IBAR handler - lock should have been requested by BSSM_GetLock
    //IBAR_BSSM_LOCK::BSSM_LockAffirmedFor( NewMessage );
    if( NewMessage->numberA == RULE_NOBODY_PLAYER)
    // Releasing lock
      UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, DISPLAY_state.IBarCurrentPlayer );
    else
    {// Set up button bar
      if( SlotIsALocalHumanPlayer[NewMessage->numberA] )
        UDIBAR_setIBarRulesState( IBAR_STATE_OtherPlayer, NewMessage->numberA );// (Not used) May not be the best state to show.
      else
      {
        //IBAR_JustChanged = TRUE;// This is just for a clicked button.
        UDIBAR_setIBarRulesState( IBAR_STATE_OtherPlayer, NewMessage->numberA ); //AI buttons - while engine is locked.
      }
    }


    // BIG UGLY GREEN CONVERSION DIALOG CRAP
    // RK - New logic here.  The dialog should be killed when < RULE_MAX_PLAYERS though
    // the hotseat was bringing it up regardless.  Perhaps the phase restart fixed it up.
    /*if (NewMessage->numberA < RULE_MAX_PLAYERS)
      wcstombs (TempString, UICurrentGameState.Players[NewMessage->numberA].
      name, sizeof (TempString));
    else
      strcpy (TempString, "Nobody");
    strcat (TempString, ", click on squares to...");
    //SetDlgItemText (hdlgCurrentDialogBox, IDC_BSM_PLAYER_CLICK_PROMPT, TempString);

    //EnableWindow (GetDlgItem (hdlgCurrentDialogBox, IDC_BSM_BUY), CurrentUIPlayer < RULE_MAX_PLAYERS);
    //EnableWindow (GetDlgItem (hdlgCurrentDialogBox, IDC_BSM_SELL), CurrentUIPlayer < RULE_MAX_PLAYERS);
    //EnableWindow (GetDlgItem (hdlgCurrentDialogBox, IDC_BSM_SELL_ALL), CurrentUIPlayer < RULE_MAX_PLAYERS);
    //EnableWindow (GetDlgItem (hdlgCurrentDialogBox, IDC_BSM_MORTGAGE), CurrentUIPlayer < RULE_MAX_PLAYERS);

    if (NewMessage->numberD != RULE_NOBODY_PLAYER)
    {
      wsprintf (TempString, "You owe $%d to ", (int) NewMessage->numberC);
      if (NewMessage->numberD < RULE_MAX_PLAYERS)
        wcstombs (TempString + strlen (TempString),
        UICurrentGameState.Players[NewMessage->numberD].name,
        sizeof( TempString ) - strlen( TempString ));
      else /* Must be to the bank. 
        strcat( TempString, "the bank" );
      strcat( TempString, "." );
    }
    else
    {
      strcpy( TempString, "No recent debt owed." );
    }
    DISPLAY_setMessageBox( TempString );
    //SetDlgItemText (hdlgCurrentDialogBox, IDC_BSM_STILL_OWE, TempString);*/
    //}
    break;

  case NOTIFY_IMMUNITY_COUNT:
  case NOTIFY_FUTURE_RENT_COUNT:
    AddUiImmunity ((RULE_PlayerNumber) NewMessage->numberD,
      (RULE_PlayerNumber) NewMessage->numberA,
      (NewMessage->action == NOTIFY_FUTURE_RENT_COUNT) ? CHT_FUTURE_RENT : CHT_RENT_IMMUNITY,
      NewMessage->numberB, NewMessage->numberE, FALSE);
    break;

  case NOTIFY_FREE_PARKING_POT:
    UICurrentGameState.FreeParkingJackpotAmount = NewMessage->numberA;
    break;

  case NOTIFY_CASH_ANIMATION:
    if( DISPLAY_state.IBarLastRuleState != IBAR_STATE_HotelDecomposition && 
      DISPLAY_state.IBarLastRuleState != IBAR_STATE_RaiseMoney)
    {// Put up display unless it holds the number of houses to sell
      DISPLAY_state.ButtonBarForceUpdate = TRUE;
      DISPLAY_state.ButtonBarWantedTime = LE_TIME_TickCount;
      LE_GRAFIX_ColorArea( DISPLAY_state.ButtonBarMessage, 0, 0, BUTTONBARMESSAGEWITDTH, 20, LE_GRAFIX_MakeColorRef( 0, 255, 0 ) );
      LE_FONTS_SetUnderline( FALSE );
      LE_FONTS_SetItalic( FALSE );
      LE_FONTS_SetSize( 12 );
      LE_FONTS_SetWeight( 900 );
      {
        ACHAR                           myBuffer[40];
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(NewMessage->numberC, 
          DISPLAY_state.system, myBuffer, TRUE);
        // Print cash 
        LE_FONTS_Print(DISPLAY_state.ButtonBarMessage,
          BUTTONBARMESSAGEWITDTH - LE_FONTS_GetStringWidth(myBuffer), 0, 
          LE_GRAFIX_MakeColorRef( 255, 255, 255 ), myBuffer );
      }
    }
    break;      
  }
}


/****************************************************************************/
/*****************************************************************************
 * Set a dialog (display is updated next call to showDialog
 */
void UDIBAR_setDialog( int index )
{
  //if(index < DISPLAY_Dialog_None || index >= DISPLAY_Dialog_MAX )
  //  return; // Could report the error

  //DISPLAY_state.CurrentDialog = index;
};


/*****************************************************************************
 * Set the state of the IBAR control from the rules engine.
 */
void UDIBAR_setIBarRulesState( int index, int player )
{
  if(index < IBAR_STATE_Nothing || index >= IBAR_STATE_MAX )
    return; // Could report the error

  DISPLAY_state.IBarLastRuleState = index;
  DISPLAY_state.IBarLastRulePlayer = player;

  // Check if this is the first call after loading a game, if so set up the player
  if( DISPLAY_state.IBarGameJustLoaded && index != IBAR_STATE_Nothing)
  {// We want to init the player, but not before they are defined (we get a nothing message first).
    DISPLAY_state.IBarGameJustLoaded = FALSE;

    GameInProgress = TRUE; // Notify players crap reset this to false - sheesh.

    DISPLAY_state.CurrentPlayerInCenterIdle = player;
    last_AI_ButtonRemoteState = !SlotIsALocalPlayer[player];// May not be set
    DISPLAY_state.desired2DCamera = UDPIECES_PickCameraFor3Squares(UICurrentGameState.Players[player].currentSquare);

    // Put all tokens on resting idle positions.
    int i, t, s;
    for( i = 0; i < UICurrentGameState.NumberOfPlayers; i++ )
    {
      for( t = 5, s = 0; t >= 0; t-- ) 
      {
        if( DISPLAY_state.Player3DTokenIdlePos[UICurrentGameState.Players[i].currentSquare][t] == -1 ) s = t; // Available
      }
      if( i != DISPLAY_state.CurrentPlayerInCenterIdle )// Dont take the resting idle position for the center guy.
        DISPLAY_state.Player3DTokenIdlePos[UICurrentGameState.Players[i].currentSquare][s] = i;
      
      //  DISPLAY_state.Player3DTokenIdlePos[0][i] = UICurrentGameState.NumberOfPlayers - i -1; // Spread em around in reverse order (so the 'deepest' come out first)
    }
    
  }
};

/*****************************************************************************
 * Set the state of the IBAR control from UI - may not be current player.
 */
void UDIBAR_setIBarCurrentState( int index )
{
  if(index < IBAR_STATE_Nothing || index >= IBAR_STATE_MAX )
    return; // Could report the error

  DISPLAY_state.IBarCurrentState = index;
  IBAR_JustChanged = TRUE;
};

/*****************************************************************************
 * Process NOTIFY_ACTION_COMPLETED for IBAR.
 */
void UDIBAR_ActionCompleted( RULE_ActionArgumentsPointer NewMessage )
{
  if( NewMessage->action == NOTIFY_ACTION_COMPLETED )
  {
    /*// This lock will be removed.
    if( (NewMessage->numberA == ACTION_PLAYER_BUYSELLMORT) &&
        (NewMessage->numberB == FALSE) ) // indicates a failure
    {
      IBAR_BSSM_LOCK::BSSM_LockDenied( NewMessage);
    }
    if( (NewMessage->numberA == ACTION_PLAYER_DONE_BUYSELLMORT) &&
        (NewMessage->numberB == TRUE) ) // indicates success releasing BSM lock
    {
      IBAR_BSSM_LOCK::BSSM_UnLocked( NewMessage);
    }*/

    // These represent button clicks - indicate the button hit.  Just store it - if the buttons go out, this variable will be checked.
    switch( NewMessage->numberA )
    {
    case ACTION_END_TURN:
      if(  NewMessage->numberB ) // Action accepted
      { // We know it was accepted, remove the bar NOW
        UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, DISPLAY_state.IBarCurrentPlayer );
        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Done;
      }
      break;
    case ACTION_ROLL_DICE:
      TRACE("\nRoll Dice\n");
      if(  NewMessage->numberB ) // Action accepted
      { // We know it was accepted, remove the bar NOW
        UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, DISPLAY_state.IBarCurrentPlayer );
        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_RollDice;
      }
      break;
    case ACTION_EXIT_JAIL_DECISION:// Check numberD (or others) for indication of what button was pressed.
      if(  NewMessage->numberB ) // Action accepted
      { // We know it was accepted, remove the bar NOW
        UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, DISPLAY_state.IBarCurrentPlayer );
        if( NewMessage->numberD == 0 )
          DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_RollDice;
        if( NewMessage->numberD == 1 )
          DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Pay;
        if( NewMessage->numberD == 2 )
          DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_UseCard;
      }
      break;
    case ACTION_CARD_SEEN:
      if(  NewMessage->numberB ) // Action accepted
      { // We know it was accepted, remove the bar NOW
        UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, DISPLAY_state.IBarCurrentPlayer );
        DISPLAY_state.ChanceCommChestDesired = -1; // Kill the card
        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Done;
        DISPLAY_state.JustReadACardHack = TRUE;
      }
      break;
    case ACTION_GO_BANKRUPT:
      if(  NewMessage->numberB ) // Action accepted
      { // We know it was accepted, remove the bar NOW
        UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, DISPLAY_state.IBarCurrentPlayer );
        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Bankrupt;
      } else
      {// Make a noise
        UDSOUND_Warning();
      }
      break;
    case ACTION_BUY_OR_AUCTION_DECISION:// Check for indication of what button was pressed
      if(  NewMessage->numberB ) // Action accepted
      { // We know it was accepted, remove the bar NOW
        UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, DISPLAY_state.IBarCurrentPlayer );
        if( NewMessage->numberD )
        {
          DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Buy;
          // THIS IS CHEATING - If the notification is successful, we know we will be informed of this, however, and the function needs the data.
          // A more elaborate sound function would do.
          int t = UICurrentGameState.Squares[UICurrentGameState.Players[NewMessage->numberC].currentSquare].owner;
          UICurrentGameState.Squares[UICurrentGameState.Players[NewMessage->numberC].currentSquare].owner = 
            (RULE_PlayerNumber) NewMessage->numberC;
          UDPENNY_BoughtProperty( UICurrentGameState.Players[NewMessage->numberC].currentSquare, NewMessage->numberC );
          UICurrentGameState.Squares[UICurrentGameState.Players[NewMessage->numberC].currentSquare].owner = t;
        }
        if( NewMessage->numberD == 0 )//auction
        {
          DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Auction;
          UDPENNY_ChooseToAuctionProperty( UICurrentGameState.Players[NewMessage->numberC].currentSquare, NewMessage->numberC );
        }
      }
      break;
    case ACTION_BID:// Check for amount bid
      break;
    case ACTION_FREE_UNMORTGAGE_DONE:
      if(  NewMessage->numberB ) // Action accepted
      { // We know it was accepted, remove the bar NOW
        UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, DISPLAY_state.IBarCurrentPlayer );
        
        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Done;
      }
      break;
    case ACTION_BUY_HOUSE:
      break;
    case ACTION_SELL_BUILDINGS:
      break;
    case ACTION_MORTGAGING:
      break;
    case ACTION_TAX_DECISION:// Check for which button
      if(  NewMessage->numberB ) // Action accepted
      { // We know it was accepted, remove the bar NOW
        UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, DISPLAY_state.IBarCurrentPlayer );
        if( NewMessage->numberD == 0 )
          DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_FlatTax;
        if( NewMessage->numberD == 1 )
          DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Percentage;
      }
      break;
    case ACTION_START_HOUSING_AUCTION:
      if(  NewMessage->numberB ) // Action accepted
      { // We know it was accepted, remove the bar NOW
        UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, DISPLAY_state.IBarCurrentPlayer );
        
        if( DISPLAY_state.IBarCurrentState == IBAR_STATE_HousingShort )
          DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_AucHouse;
        if( DISPLAY_state.IBarCurrentState == IBAR_STATE_HotelShort )
          DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_AucHotel;
      }
      break;

    // Couple trade items
    case ACTION_START_TRADE_EDITING:
      //DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Trade;
      break;
    case ACTION_TRADE_ACCEPT:// True or false, set tracking back on.
      DISPLAY_state.IBarStateTrackOn = TRUE;
      break;
      
    // Save game request - just a handy place to beep on failure.
    case ACTION_GET_GAME_STATE_FOR_SAVE:
      if(  !NewMessage->numberB ) // Failed, but it may not have been our machine requesting the save
      {// Make a noise
        if( SlotIsALocalPlayer[NewMessage->numberC] )
          //UDSOUND_Warning();
          LE_SEQNCR_Start( LED_IFT(DAT_MAIN, WAV_tmpnext), 0 ); // Different sound
      }
      break;

    }
  }
}


/*****************************************************************************
 * Upon entering the deed state or the players finances changing, calculate
 * & record available buttons (actions) for the selected deed.
 */
void UDIBAR_DeedStateCalcButtons( void )
{// We are fairly safe assuming the correct player is current, just check valid buttons
  int cash;
  LANG_TextMessageNumber  ErrorCode;
  BOOL                    BuildingAHotel;

  //DISPLAY_state.IBarDeedForDeedState
  DISPLAY_state.IBarDeedButtonsAvail = 0;
  cash = UICurrentGameState.Players[DISPLAY_state.IBarCurrentPlayer].cash;
  DISPLAY_state.IBarDeedButtonsCash = cash; // Record cash calcs will be based on

  // Buy House?
  ErrorCode = DISPLAY_TestBuildingPlacement (&UICurrentGameState,
    DISPLAY_state.IBarCurrentPlayer,
    DISPLAY_state.IBarDeedForDeedState,
    TRUE,     // Adding a building
    FALSE,    // Not forced to house only (add)
    FALSE,    // Not forced to hotel only (add)
    &BuildingAHotel, // Are we building a hotel (or house)
    NULL,     // Remaining buildings pointer
    NULL);    // house cost, lowest/highest houses in monopoly,
  if (ErrorCode == TMN_NO_MESSAGE)
    DISPLAY_state.IBarDeedButtonsAvail |= UDIBAR_DeedStateButtonBuyHouse;

  // Sell House?
  ErrorCode = DISPLAY_TestBuildingPlacement (&UICurrentGameState,
    DISPLAY_state.IBarCurrentPlayer,
    DISPLAY_state.IBarDeedForDeedState,
    FALSE,     // Adding a building
    FALSE,    // Not forced to house only (add)
    FALSE,    // Not forced to hotel only (add)
    &BuildingAHotel, // Are we building a hotel (or house)
    NULL,     // Remaining buildings pointer
    NULL);    // house cost, lowest/highest houses in monopoly,
  if (ErrorCode == TMN_NO_MESSAGE)
    DISPLAY_state.IBarDeedButtonsAvail |= UDIBAR_DeedStateButtonSellHouse;

  // Mortgagable?
  if( UDIBAR_testMortgagable( DISPLAY_state.IBarDeedForDeedState ) )
    DISPLAY_state.IBarDeedButtonsAvail |= UDIBAR_DeedStateButtonMortgage;

  // Unmortgagable?
  if( UDIBAR_testUnMortgagable( DISPLAY_state.IBarDeedForDeedState ) )
    DISPLAY_state.IBarDeedButtonsAvail |= UDIBAR_DeedStateButtonUnMortgage;


}

int UDIBAR_testMortgagable( int deed )
{
  RULE_SquareGroups MonopolyGroup;
  int player, test, i;

  player = UICurrentGameState.Squares[deed].owner;

  test = FALSE;
  if( !UICurrentGameState.Squares[deed].mortgaged )
  {
    // Check - any houses on the monopoly group?
    MonopolyGroup = RULE_SquarePredefinedInfo[deed].group;
    test = TRUE;
    for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++) // Great, n squared algorythm.
    {// Check each for a house OPTIMIZE: Be smart, one pass per monopoly and just check the monopoly not all properties.
      if (RULE_SquarePredefinedInfo[i].group == MonopolyGroup)
      {
        if ((UICurrentGameState.Squares[i].owner == player) &&
          (UICurrentGameState.Squares[i].houses != 0) )
          test = FALSE;
      }
    }
  }
  return( test );
};


/*****************************************************************************
    * Function:     void UDIBar_ProcessStatsButtonPress(LE_QUEUE_MessagePointer UIMessagePntr )
    * Returns:      nothing
    * Parameters:   pointer to message
    *
    * Purpose:      Determines the action depending on which status bar button
    *                           was pressed.
    ****************************************************************************
    */
void UDIBar_ProcessStatsButtonPress(int button)
{

    int result;

    // verify for category status buttons
    if(button >= B_PLAYER && button <= B_BANK)
    {
        //result = UDStats_GetButtonIndex(UIMessagePntr, CATEGORY_BUTTONS_AREA );
        result = button;

        if(result > -1)
        {
            // return any pressed buttons to idle position
            if(result != last_category_result)
            {
                LE_SEQNCR_Stop( CategoryButton[last_category_result].PressID, CButtonPriority[last_category_result]);
                LE_SEQNCR_Start(CategoryButton[last_category_result].ReturnID,  CButtonIdlePriority[last_category_result]);
                LE_SEQNCR_SetEndingAction (CategoryButton[last_category_result].ReturnID,   CButtonIdlePriority[last_category_result],
                                                                        LE_SEQNCR_EndingActionStayAtEnd);
                CategoryButton[last_category_result].State = ISTATBAR_BUTTON_Return;

            }
        }
        switch(result)
        {
            case 0: UDStats_DisplayPlayerStatusScreen();        break;
            case 1: UDStats_DisplayDeedStatusScreen();          break;
      case 2: UDStats_DisplayBankStatusScreen();      break;
            default: break;
        }
        if(result > -1)
        {
            if(result != last_category_result)
            {
                if(CategoryButton[result].State == ISTATBAR_BUTTON_Idle)
                    LE_SEQNCR_Stop(CategoryButton[result].IdleID, CButtonIdlePriority[result]);
                else
                    LE_SEQNCR_Stop(CategoryButton[result].ReturnID, CButtonIdlePriority[result]);

                LE_SEQNCR_Start(CategoryButton[result].PressID, CButtonPriority[result]);
                LE_SEQNCR_SetEndingAction (CategoryButton[result].PressID,  CButtonPriority[result],
                                                                            LE_SEQNCR_EndingActionStayAtEnd);
                CategoryButton[result].State = ISTATBAR_BUTTON_Press;

                // remove the current sort buttons
                UDIBar_RemoveSortButtons();
                // determine which sort buttons will be showing and display them
                UDIBar_SwapStatusButtons();
                UDIBar_DisplaySortButtons();
            }
                // force the program to redraw the buttons on default
    //          g_bStatusBarDisplayed = FALSE;
            g_bIsScreenUpdated = FALSE;
            last_category_result = result;
            g_Button = last_sort_result+B_SORT_1;
        }

            // display the last view by sending a message with the appropriate mouse coordinates
            //UIMessage.numberA = SortButton[last_sort_result].HotSpot.left;
            //UIMessage.numberB = SortButton[last_sort_result].HotSpot.top;
            //UDIBar_ProcessStatsButtonPress(&UIMessage);
        return;
    }
    // verify for deed status buttons
    if(fStatusScreen == DEED_SCREEN)
    {
        //result = UDStats_GetButtonIndex(UIMessagePntr, SORT_BUTTONS_AREA );
        if(button >= B_SORT_1 && button <= B_SORT_4)
        {
            result = button - B_SORT_1;
            if(result > -1)
            {
                // remove the display of the deeds
                UDStats_RemoveDeeds();

                // return any pressed buttons to idle position
                if((SortButton[last_sort_result].State == ISTATBAR_BUTTON_Press) &&
                        (last_sort_result != result))
                {
                    LE_SEQNCR_Stop( SortButton[last_sort_result].PressID, SButtonPriority[last_sort_result]);
                    LE_SEQNCR_Start(SortButton[last_sort_result].ReturnID,  SButtonIdlePriority[last_sort_result]);
                    LE_SEQNCR_SetEndingAction (SortButton[last_sort_result].ReturnID,   SButtonIdlePriority[last_sort_result],
                                                                            LE_SEQNCR_EndingActionStayAtEnd);
                    SortButton[last_sort_result].State = ISTATBAR_BUTTON_Return;

                }
            }
            switch(result)
            {
                case 0: UDStats_SortDeedsByPurchaceValue();     break;
                case 1: UDStats_SortDeedsByOwner();                     break;
                case 2: UDStats_SortDeedsByEstCurrentValue();   break;
                case 3: UDStats_SortDeedsByMostValuable();      break;
                default: return;
            }

            if(SortButton[result].State != ISTATBAR_BUTTON_Press)
            {
                if(SortButton[result].State == ISTATBAR_BUTTON_Idle)
                    LE_SEQNCR_Stop(SortButton[result].IdleID, SButtonIdlePriority[result]);
                else
                    LE_SEQNCR_Stop(SortButton[result].ReturnID, SButtonIdlePriority[result]);

                LE_SEQNCR_Start(SortButton[result].PressID, SButtonPriority[result]);
                LE_SEQNCR_SetEndingAction (SortButton[result].PressID,  SButtonPriority[result],
                                                                            LE_SEQNCR_EndingActionStayAtEnd);
                SortButton[result].State = ISTATBAR_BUTTON_Press;
            }

            UDStats_DisplayDeeds();
            last_dsort_result = last_sort_result = result;
            g_bIsScreenUpdated = TRUE;
            return;
        }

    }

    // verify for player status buttons
    if(fStatusScreen == PLAYER_SCREEN)
    {
        //result = UDStats_GetButtonIndex(UIMessagePntr, SORT_BUTTONS_AREA );
        if(button >= B_SORT_1 && button <= B_SORT_4)
        {
            result = button - B_SORT_1;

            if(result > -1)
            {
                UDStats_RemovePlayerDeeds();
                UDStats_RemovePlayerBoxes();

                // return any pressed buttons to idle position
                if((SortButton[last_sort_result].State == ISTATBAR_BUTTON_Press) &&
                    (last_sort_result != result))
                {
                    LE_SEQNCR_Stop( SortButton[last_sort_result].PressID, SButtonPriority[last_sort_result]);
                    LE_SEQNCR_Start(SortButton[last_sort_result].ReturnID,  SButtonIdlePriority[last_sort_result]);
                    LE_SEQNCR_SetEndingAction (SortButton[last_sort_result].ReturnID,   SButtonIdlePriority[last_sort_result],
                                                                            LE_SEQNCR_EndingActionStayAtEnd);
                    SortButton[last_sort_result].State = ISTATBAR_BUTTON_Return;
                }
            }

            switch(result)
            {
                case 0: UDStats_SortPlayersByOrderOfTurn();         break;
                case 1: UDStats_SortPlayersByNetWorth();                break;
                case 2: UDStats_SortPlayersByHighFutureValue(); break;
                case 3: UDStats_SortPlayersByCashAmount();          break;
                default: return;
            }
            if(SortButton[result].State != ISTATBAR_BUTTON_Press)
            {
                if(SortButton[result].State == ISTATBAR_BUTTON_Idle)
                    LE_SEQNCR_Stop(SortButton[result].IdleID, SButtonIdlePriority[result]);
                else
                    LE_SEQNCR_Stop(SortButton[result].ReturnID, SButtonIdlePriority[result]);

                LE_SEQNCR_Start(SortButton[result].PressID, SButtonPriority[result]);
                LE_SEQNCR_SetEndingAction (SortButton[result].PressID,  SButtonPriority[result],
                                                                            LE_SEQNCR_EndingActionStayAtEnd);
                SortButton[result].State = ISTATBAR_BUTTON_Press;
            }
            UDStats_DisplayPlayerBoxes();
            UDStats_DisplayPlayerDeeds();
            last_psort_result = last_sort_result = result;
            g_bIsScreenUpdated = TRUE;
            return;
        }
    }


  // verify for deed status buttons
    if(fStatusScreen == BANK_SCREEN)
    {
        //result = UDStats_GetButtonIndex(UIMessagePntr, SORT_BUTTONS_AREA );
        if(button >= B_SORT_1 && button <= B_SORT_4)
        {
            result = button - B_SORT_1;
            if(result > -1)
            {
        // remove the display of the last bank sort
                UDStats_RemoveSortBankResults(last_sort_result);

                // return any pressed buttons to idle position
                if((SortButton[last_sort_result].State == ISTATBAR_BUTTON_Press) &&
                        (last_sort_result != result))
                {
                    LE_SEQNCR_Stop( SortButton[last_sort_result].PressID, SButtonPriority[last_sort_result]);
                    LE_SEQNCR_Start(SortButton[last_sort_result].ReturnID,  SButtonIdlePriority[last_sort_result]);
                    LE_SEQNCR_SetEndingAction (SortButton[last_sort_result].ReturnID,   SButtonIdlePriority[last_sort_result],
                                                                            LE_SEQNCR_EndingActionStayAtEnd);
                    SortButton[last_sort_result].State = ISTATBAR_BUTTON_Return;

                }
            }
            switch(result)
            {
                case 0: UDStats_SortBankByHouses();         break;
                case 1: UDStats_SortBankByProperties();     break;
                case 2: UDStats_SortBankByLiabilities();    break;
                case 3: UDStats_SortBankByHistory();        break;
                default: return;
            }

            if(SortButton[result].State != ISTATBAR_BUTTON_Press)
            {
                if(SortButton[result].State == ISTATBAR_BUTTON_Idle)
                    LE_SEQNCR_Stop(SortButton[result].IdleID, SButtonIdlePriority[result]);
                else
                    LE_SEQNCR_Stop(SortButton[result].ReturnID, SButtonIdlePriority[result]);

                LE_SEQNCR_Start(SortButton[result].PressID, SButtonPriority[result]);
                LE_SEQNCR_SetEndingAction (SortButton[result].PressID,  SButtonPriority[result],
                                                                            LE_SEQNCR_EndingActionStayAtEnd);
                SortButton[result].State = ISTATBAR_BUTTON_Press;
            }

            //UDStats_DisplayDeeds();
            last_bsort_result = last_sort_result = result;
            g_bIsScreenUpdated = TRUE;
            return;
        }

    }

}





/*****************************************************************************
    * Function:     void UDIBar_InitStatusButtons(void)
    * Returns:      nothing
    * Parameters:   nothing
    *
    * Purpose:      Sets the coordinates for the player status buttons.
    ****************************************************************************
    */
void UDIBar_InitStatusButtons(void)
{
    int i;

    // set the sort buttons for the player sort initially
    for(i=0; i<4; i++)
    {
        SortButton[i].HotSpot.left = 495 + (70*i);
        SortButton[i].HotSpot.top = 510;
        SortButton[i].HotSpot.right = SortButton[i].HotSpot.left + (56+i*2);
        SortButton[i].HotSpot.bottom = SortButton[i].HotSpot.top + 40;
        SortButton[i].Xpos = SortXPosition[i];
        SortButton[i].State = ISTATBAR_BUTTON_Off;
    }
    UDIBar_SwapStatusButtons();
    //last_sort_result = 0;

    // load the category title
    CategoryBarID = LED_IFT( DAT_LANG2, CNK_syssbo );
	CategoryBarID1 = LED_IFT( DAT_LANG2, CNK_syssbo1 );

    // set the category buttons
    for(i=0; i<3; i++)
    {
        CategoryButton[i].HotSpot.left = 54 + (93*i);
        CategoryButton[i].HotSpot.top = 510;
        CategoryButton[i].HotSpot.right =   CategoryButton[i].HotSpot.left + (77-i*3);
        CategoryButton[i].HotSpot.bottom =  CategoryButton[i].HotSpot.top + 40;
        CategoryButton[i].Xpos = CategoryXPosition[i];
        CategoryButton[i].State = ISTATBAR_BUTTON_Off;
    }

    CategoryButton[0].IdleID = LED_IFT( DAT_LANG2, CNK_iysplyki );
    CategoryButton[1].IdleID = LED_IFT( DAT_LANG2, CNK_iysddski );
    CategoryButton[2].IdleID = LED_IFT( DAT_LANG2, CNK_iysbnkki );

    CategoryButton[0].PressID = LED_IFT( DAT_LANG2, CNK_iysplykp );
    CategoryButton[1].PressID = LED_IFT( DAT_LANG2, CNK_iysddskp );
    CategoryButton[2].PressID = LED_IFT( DAT_LANG2, CNK_iysbnkkp );

    CategoryButton[0].ReturnID = LED_IFT( DAT_LANG2, CNK_iysplykr );
    CategoryButton[1].ReturnID = LED_IFT( DAT_LANG2, CNK_iysddskr );
    CategoryButton[2].ReturnID = LED_IFT( DAT_LANG2, CNK_iysbnkkr );

}






/*****************************************************************************
    * Function:     void UDIBar_SwapStatusButtons(void)
    * Returns:      nothing
    * Parameters:   nothing
    *
    * Purpose:      Set the sequences for the sort buttons.
    ****************************************************************************
    */
void UDIBar_SwapStatusButtons(void)
{
    if(fStatusScreen == PLAYER_SCREEN)
    {
        SortBarID = LED_IFT( DAT_LANG2, CNK_sypsbo );
		SortBarID1 = LED_IFT( DAT_LANG2, CNK_sypsbo1 );
        SortButton[0].IdleID = LED_IFT( DAT_LANG2, CNK_iystrnki );
        SortButton[1].IdleID = LED_IFT( DAT_LANG2, CNK_iysntwki );
        SortButton[2].IdleID = LED_IFT( DAT_LANG2, CNK_iysftvki );
        SortButton[3].IdleID = LED_IFT( DAT_LANG2, CNK_iyscshki );

        SortButton[0].PressID = LED_IFT( DAT_LANG2, CNK_iystrnkp );
        SortButton[1].PressID = LED_IFT( DAT_LANG2, CNK_iysntwkp );
        SortButton[2].PressID = LED_IFT( DAT_LANG2, CNK_iysftvkp );
        SortButton[3].PressID = LED_IFT( DAT_LANG2, CNK_iyscshkp );

        SortButton[0].ReturnID = LED_IFT( DAT_LANG2, CNK_iystrnkr );
        SortButton[1].ReturnID = LED_IFT( DAT_LANG2, CNK_iysntwkr );
        SortButton[2].ReturnID = LED_IFT( DAT_LANG2, CNK_iysftvkr );
        SortButton[3].ReturnID = LED_IFT( DAT_LANG2, CNK_iyscshkr );

        last_sort_result = last_psort_result;
    }
    else if(fStatusScreen == DEED_SCREEN)
    {

        SortBarID = LED_IFT( DAT_LANG2, CNK_sydsbo );
		SortBarID1 = LED_IFT( DAT_LANG2, CNK_sydsbo1 );
        SortButton[0].IdleID = LED_IFT( DAT_LANG2, CNK_iyspriki );
        SortButton[1].IdleID = LED_IFT( DAT_LANG2, CNK_iysownki );
        SortButton[2].IdleID = LED_IFT( DAT_LANG2, CNK_iyscrnki );
        SortButton[3].IdleID = LED_IFT( DAT_LANG2, CNK_iysernki );

        SortButton[0].PressID = LED_IFT( DAT_LANG2, CNK_iysprikp );
        SortButton[1].PressID = LED_IFT( DAT_LANG2, CNK_iysownkp );
        SortButton[2].PressID = LED_IFT( DAT_LANG2, CNK_iyscrnkp );
        SortButton[3].PressID = LED_IFT( DAT_LANG2, CNK_iysernkp );

        SortButton[0].ReturnID = LED_IFT( DAT_LANG2, CNK_iysprikr );
        SortButton[1].ReturnID = LED_IFT( DAT_LANG2, CNK_iysownkr );
        SortButton[2].ReturnID = LED_IFT( DAT_LANG2, CNK_iyscrnkr );
        SortButton[3].ReturnID = LED_IFT( DAT_LANG2, CNK_iysernkr );

        last_sort_result = last_dsort_result;
    }
  else if(fStatusScreen == BANK_SCREEN)
  {
		SortBarID = LED_IFT( DAT_LANG2, CNK_bybssoi0 );
		SortBarID1 = LED_IFT( DAT_LANG2, CNK_bybssoi1 );
        SortButton[0].IdleID = LED_IFT( DAT_LANG2, CNK_iybhhi00 );
        SortButton[1].IdleID = LED_IFT( DAT_LANG2, CNK_iybpri00 );
        SortButton[2].IdleID = LED_IFT( DAT_LANG2, CNK_iyblii00 );
        SortButton[3].IdleID = LED_IFT( DAT_LANG2, CNK_iybahi00 );

        SortButton[0].PressID = LED_IFT( DAT_LANG2, CNK_iybhhp00 );
        SortButton[1].PressID = LED_IFT( DAT_LANG2, CNK_iybprp00 );
        SortButton[2].PressID = LED_IFT( DAT_LANG2, CNK_iyblip00 );
        SortButton[3].PressID = LED_IFT( DAT_LANG2, CNK_iybahp00 );

        SortButton[0].ReturnID = LED_IFT( DAT_LANG2, CNK_iybhhf00 );
        SortButton[1].ReturnID = LED_IFT( DAT_LANG2, CNK_iybprf00 );
        SortButton[2].ReturnID = LED_IFT( DAT_LANG2, CNK_iyblif00 );
        SortButton[3].ReturnID = LED_IFT( DAT_LANG2, CNK_iybahf00 );

        last_sort_result = last_bsort_result;
  }
}





/*****************************************************************************
    * Function:     void UDIBar_DisplaySortButtons(void)
    * Returns:      nothing
    * Parameters:   nothing
    *
    * Purpose:      Displays the Sort buttons only.
    ****************************************************************************
    */
void UDIBar_DisplaySortButtons(void)
{
    // display the sort status bar
    LE_SEQNCR_Start(SortBarID, BarPriority);
	LE_SEQNCR_Start(SortBarID1, BarPriority+100);

    for(int i=0; i<4; i++)
    {
        switch(SortButton[i].State)
        {
        case ISTATBAR_BUTTON_Idle:      LE_SEQNCR_Stop(SortButton[i].IdleID, SButtonIdlePriority[i]); break;
        case ISTATBAR_BUTTON_Press:     LE_SEQNCR_Stop(SortButton[i].PressID, SButtonPriority[i]); break;
        case ISTATBAR_BUTTON_Return:    LE_SEQNCR_Stop(SortButton[i].ReturnID, SButtonIdlePriority[i]); break;
        default: break;
        }

        LE_SEQNCR_Start(SortButton[i].IdleID, SButtonIdlePriority[i]);
        SortButton[i].State = ISTATBAR_BUTTON_Idle;
    }

}






/*****************************************************************************
    * Function:     void UDIBar_RemoveSortButtons(void)
    * Returns:      nothing
    * Parameters:   nothing
    *
    * Purpose:      Removes the Sort buttons only.
    ****************************************************************************
    */
void UDIBar_RemoveSortButtons(void)
{
    for(int i=0; i<4; i++)
    {
        switch(SortButton[i].State)
        {
        case ISTATBAR_BUTTON_Idle:      LE_SEQNCR_Stop(SortButton[i].IdleID, SButtonIdlePriority[i]); break;
        case ISTATBAR_BUTTON_Press:     LE_SEQNCR_Stop(SortButton[i].PressID, SButtonPriority[i]); break;
        case ISTATBAR_BUTTON_Return:    LE_SEQNCR_Stop(SortButton[i].ReturnID, SButtonIdlePriority[i]); break;
        default: break;
        }
        SortButton[i].State = ISTATBAR_BUTTON_Off;

    }

    // remove the sort status bar
    LE_SEQNCR_Stop(SortBarID, BarPriority);
	LE_SEQNCR_Stop(SortBarID1, BarPriority+100);
/*
    for(i=0; i<3; i++)
    {
        switch(CategoryButton[i].State)
        {
        case ISTATBAR_BUTTON_Idle:      LE_SEQNCR_Stop(CategoryButton[i].IdleID, SButtonIdlePriority[i]); break;
        case ISTATBAR_BUTTON_Press:     LE_SEQNCR_Stop(CategoryButton[i].PressID, SButtonPriority[i]); break;
        case ISTATBAR_BUTTON_Return:    LE_SEQNCR_Stop(CategoryButton[i].ReturnID, SButtonIdlePriority[i]); break;
        default: break;
        }
        //LE_SEQNCR_Stop(DeedStatusButtonID[i], SButtonPriority);
    }
*/
}






/*****************************************************************************
    * Function:     void UDIBar_ResetStatusBarButtons(void)
    * Returns:      nothing
    * Parameters:   nothing
    *
    * Purpose:      Resets the status bar buttons to sort by player and turns.
    *                           Used for when the player resets the game.
    ****************************************************************************
    */
void UDIBar_ResetStatusBarButtons(void)
{
    //reset the sorting globals
    last_sort_result = last_psort_result = last_dsort_result =
                      last_bsort_result = last_category_result = 0;
    //g_bStatusBarDisplayed = FALSE;
    g_Button = 0;
    g_bIsScreenUpdated = FALSE;

    fStatusScreen = PLAYER_SCREEN;

    // swap the buttons back to player status if they were in deed status
    //UDIBar_SwapStatusButtons();
}




/*****************************************************************************
    * Function:     void UDIBar_InitOptionsButtons(void)
    * Returns:      nothing
    * Parameters:   nothing
    *
    * Purpose:      Sets the coordinates for the options buttons.
    ****************************************************************************
    */
void UDIBar_InitOptionsButtons(void)
{
    int i;

    // set the sort buttons for the player sort initially
    for(i=0; i<4; i++)
    {
        OptionsButton[i].HotSpot.left = OptionsXPosition[i];
        OptionsButton[i].HotSpot.top = OptionsYPosition[i];
        OptionsButton[i].HotSpot.right = OptionsXPosition[i] + OptionsWidth[i];
        OptionsButton[i].HotSpot.bottom = OptionsYPosition[i] + OptionsHeight[i];
        OptionsButton[i].Xpos = OptionsXPosition[i];
        OptionsButton[i].State = ISTATBAR_BUTTON_Off;
    }
    

    OptionsButton[0].IdleID = LED_IFT( DAT_LANG2, CNK_iysfili );
    OptionsButton[1].IdleID = LED_IFT( DAT_LANG2, CNK_iysfopti );
    OptionsButton[2].IdleID = LED_IFT( DAT_LANG2, CNK_iysfcrdi );
    OptionsButton[3].IdleID = LED_IFT( DAT_LANG2, CNK_iysfhlpi );

    OptionsButton[0].PressID = LED_IFT( DAT_LANG2, CNK_iysfilp );
    OptionsButton[1].PressID = LED_IFT( DAT_LANG2, CNK_iysfoptp );
    OptionsButton[2].PressID = LED_IFT( DAT_LANG2, CNK_iysfcrdp );
    OptionsButton[3].PressID = LED_IFT( DAT_LANG2, CNK_iysfhlpp );

    OptionsButton[0].ReturnID = LED_IFT( DAT_LANG2, CNK_iysfilo );
    OptionsButton[1].ReturnID = LED_IFT( DAT_LANG2, CNK_iysfopto );
    OptionsButton[2].ReturnID = LED_IFT( DAT_LANG2, CNK_iysfcrdo );
    OptionsButton[3].ReturnID = LED_IFT( DAT_LANG2, CNK_iysfhlpo );

}


/*****************************************************************************
    * Function:     void UDIBar_DisplayOptionsButtons(void)
    * Returns:      nothing
    * Parameters:   nothing
    *
    * Purpose:      Displays the Options buttons only.
    ****************************************************************************
    */
void UDIBar_DisplayOptionsButtons(void)
{
    for(int i=0; i<4; i++)
    {
        switch(OptionsButton[i].State)
        {
        case ISTATBAR_BUTTON_Idle:      LE_SEQNCR_Stop(OptionsButton[i].IdleID, OButtonIdlePriority[i]); break;
        case ISTATBAR_BUTTON_Press:     LE_SEQNCR_Stop(OptionsButton[i].PressID, OButtonPriority[i]); break;
        case ISTATBAR_BUTTON_Return:    LE_SEQNCR_Stop(OptionsButton[i].ReturnID, OButtonIdlePriority[i]); break;
        default: break;
        }

		if(g_bOptionsScreenOn && (i==FILE_SCREEN))
		{
			LE_SEQNCR_Start(OptionsButton[i].PressID, OButtonPriority[i]);
			OptionsButton[i].State = ISTATBAR_BUTTON_Press;
		}
        else
		{
			LE_SEQNCR_Start(OptionsButton[i].IdleID, OButtonIdlePriority[i]);
			OptionsButton[i].State = ISTATBAR_BUTTON_Idle;
		}
    }

}






/*****************************************************************************
    * Function:     void UDIBar_RemoveOptionsButtons(void)
    * Returns:      nothing
    * Parameters:   nothing
    *
    * Purpose:      Removes the Options buttons only.
    ****************************************************************************
    */
void UDIBar_RemoveOptionsButtons(void)
{
    for(int i=0; i<4; i++)
    {
        switch(OptionsButton[i].State)
        {
        case ISTATBAR_BUTTON_Idle:      LE_SEQNCR_Stop(OptionsButton[i].IdleID, OButtonIdlePriority[i]); break;
        case ISTATBAR_BUTTON_Press:     LE_SEQNCR_Stop(OptionsButton[i].PressID, OButtonPriority[i]); break;
        case ISTATBAR_BUTTON_Return:    LE_SEQNCR_Stop(OptionsButton[i].ReturnID, OButtonIdlePriority[i]); break;
        default: break;
        }
        OptionsButton[i].State = ISTATBAR_BUTTON_Off;

    }

    
}





/*****************************************************************************
    * Function:     void UDIBar_ProcessOptionsButtonPress(int button)
    * Returns:      nothing
    * Parameters:   pointer to message
    *
    * Purpose:      Determines the action depending on which Options button
    *                           was pressed.
    ****************************************************************************
    */
void UDIBar_ProcessOptionsButtonPress(int button)
{

	current_options_result = button;

    // return any pressed buttons to idle position
	if(last_options_result > -1)
	{
		if((OptionsButton[last_options_result].State == ISTATBAR_BUTTON_Press) &&
			(last_options_result != button))
		{
			LE_SEQNCR_Stop( OptionsButton[last_options_result].PressID, OButtonPriority[last_options_result]);
			LE_SEQNCR_Start(OptionsButton[last_options_result].ReturnID,  OButtonIdlePriority[last_options_result]);
			LE_SEQNCR_SetEndingAction (OptionsButton[last_options_result].ReturnID,   OButtonIdlePriority[last_options_result],
																	LE_SEQNCR_EndingActionStayAtEnd);
			OptionsButton[last_options_result].State = ISTATBAR_BUTTON_Return;
		}
	}

/*
	switch(button)
	{
    case FILE_SCREEN:		UDOPTIONS_DisplayFileScreen(last_options_result);		break;
    case OPTION_SCREEN:		UDOPTIONS_DisplayOptionScreen(last_options_result);		break;
    case CREDITS_SCREEN:	UDOPTIONS_DisplayCreditsScreen(last_options_result);	break;
    case HELP_SCREEN:		UDOPTIONS_DisplayHelpScreen(last_options_result);		break;
    default: return;
	}
*/
	if(button > -1)
	{
		if(OptionsButton[button].State != ISTATBAR_BUTTON_Press)
		{
			if(OptionsButton[button].State == ISTATBAR_BUTTON_Idle)
				LE_SEQNCR_Stop(OptionsButton[button].IdleID, OButtonIdlePriority[button]);
			else
				LE_SEQNCR_Stop(OptionsButton[button].ReturnID, OButtonIdlePriority[button]);

			LE_SEQNCR_Start(OptionsButton[button].PressID, OButtonPriority[button]);
			LE_SEQNCR_SetEndingAction (OptionsButton[button].PressID,  OButtonPriority[button],
																		LE_SEQNCR_EndingActionStayAtEnd);
			OptionsButton[button].State = ISTATBAR_BUTTON_Press;
		}
		
		// set option screen off so that the options screen will update itself
		g_bOptionsScreenOn = FALSE;
		
	}
	
	return;
}
