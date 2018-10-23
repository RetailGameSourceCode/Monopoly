/*****************************************************************************
* FILE:        UDTRADE.cpp, subfile of Display.cpp & UserIfce.cpp
* DESCRIPTION: Handles display & User input specific to the trade screen.
*
* © Copyright 1999 Artech Digital Entertainments.  All rights reserved.
*****************************************************************************/
#include "GameInc.h"

#define SU 16
#define SD 51
#define SR 4
#define CASHTRADEINIT 0
#define CLIPACTION Sound_ClipOldSoundIfPlaying
#define MOVESTEP 4
#define width(x) ((short)(x.right - x.left))
#define height(x) ((short)(x.bottom - x.top))
#define AdjustRect1(r1, r, x, y) \
  r1.left = r.left + x;  r1.right = r.right + x;  r1.top = r.top + y;  r1.bottom = r.bottom + y;
//#define DrawBox(box) LE_GRAFIX_ColorArea(tmpoverlay, (short)box.left, (short)box.top, width(box), height(box), LEG_MCRA(255, 255, 255, 128));
#define DrawButton(id, box, colour, text) \
  LE_GRAFIX_ColorArea(id, (short)box.left, (short)box.top, width(box), height(box), LEG_MCR(190,190,190)); \
  LE_GRAFIX_ColorArea(id, (short)(box.right-2), (short)box.top, 2, height(box), LEG_MCR(128-28,128-28,128-28)); \
  LE_GRAFIX_ColorArea(id, (short)box.left, (short)box.top, width(box), 2, LEG_MCR(222,222,222)); \
  LE_GRAFIX_ColorArea(id, (short)box.left, (short)box.top, 2, height(box), LEG_MCR(222,222,222)); \
  LE_GRAFIX_ColorArea(id, (short)box.left, (short)(box.bottom-2), width(box), 2, LEG_MCR(128-28,128-28,128-28)); \
  LE_FONTS_Print(id, (200 - LE_FONTS_GetStringWidth(text)) / 2, (short)box.top+4, LEG_MCR(0,0,0), text);
//RECT all1 = {0, 0, 800, 600};
RECT listbox[5] = {
  { 614, 114 + (0 * 14), 614 + 150, 114 + (1 * 14) },
  { 614, 114 + (1 * 14), 614 + 150, 114 + (2 * 14) },
  { 614, 114 + (2 * 14), 614 + 150, 114 + (3 * 14) },
  { 614, 114 + (3 * 14), 614 + 150, 114 + (4 * 14) },
  { 614, 114 + (4 * 14), 614 + 150, 114 + (5 * 14) } };

/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

int curside, curproperty, curamount;
int AcceptSndTick = 0;

// For now, let A allow only proposer in edit, B all others.  In view, A proposes, B is everyone else involved.
LE_DATA_DataId TradeATokenIDShown, TradeBTokenIDShown;
int TradeAPlayerActive = RULE_MAX_PLAYERS, TradeBPlayerActive = RULE_MAX_PLAYERS;
RULE_PropertySet TradeAStuff, TradeAStuffMort, TradeBStuff, TradeBStuffMort;
RULE_PropertySet TradeAAfter, TradeAAfterMort, TradeBAfter, TradeBAfterMort;
RULE_PropertySet TradePropsA, TradePropsAMort, TradePropsB, TradePropsBMort;

#define TRADE_A_TOKEN_X 243-236
#define TRADE_B_TOKEN_X 556+236 // subtract width of token

//Coordinate arrays for player A & B start, finish and offer/expected windows.
int TRADE_PROPERTY_TopX[6] = { 0,0,0,0,0,0 };
int TRADE_PROPERTY_TopY[6] = { 40,40,45,45,45,45 };
#define TRADE_PROPERTY_BasicW  205 // size of big corner box's items
#define TRADE_PROPERTY_Height  225
#define TRADE_PROPERTY_BasicW1 200 // size of big corner box
#define TRADE_PROPERTY_Height1 225
#define TRADE_PROPERTY_ItemW   205
#define TRADE_PROPERTY_ButtonW 60
#define TRADE_PROPERTY_CardW   36 // Size for mouse click processing.
#define TRADE_PROPERTY_CardH   42
#define TRADE_PROPERTY_DeltaCol (TRADE_PROPERTY_BasicW - TRADE_PROPERTY_ButtonW) / 12  // Each property column spread.
#define TRADE_PROPERTY_DeltaX   (TRADE_PROPERTY_DeltaCol) / 4 // == 3    12
#define TRADE_PROPERTY_DeltaY   10 //15 == ((TRADE_PROPERTY_Height - TRADE_PROPERTY_TopY[0] - 2 - TRADE_PROPERTY_CardH) / 3)

// This array holds the locations relative to the property box.  There are 4 boxes, these locations are the same in each.
short TradeBasicPlayerBoxes[4][2];  // The locations of the player boxes.

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//
#define PointInRect(x, y, r) \
  (!((x < r.left) || (x >= r.right) || (y < r.top) || (y >= r.bottom)))

int cashIconLoc[4][2] = {{5+SR, 344+SD},{605+SR, 344+SD},{205+SR, 374-SU},{405+SR, 374-SU}};
// Buttons to activate cash trade
RECT CashTradeAT1 = {  15+SR, 344+SD,  15+SR+33, 344+SD+30 }, CashTradeAT2 = {   5+SR, 374+SD,   5+SR+52, 374+SD+15 };
RECT CashTradeBT1 = { 615+SR, 344+SD, 615+SR+33, 344+SD+30 }, CashTradeBT2 = { 605+SR, 374+SD, 605+SR+52, 374+SD+15 };
// Buttons to set cash trade to zero
RECT CashTradeAM1 = { 215+SR, 374-SU, 215+SR+33, 374-SU+30 }, CashTradeAM2 = { 205+SR, 404-SU, 205+SR+52, 404-SU+15 };
RECT CashTradeBM1 = { 415+SR, 374-SU, 415+SR+33, 374-SU+30 }, CashTradeBM2 = { 405+SR, 404-SU, 405+SR+52, 404-SU+15 };

int chanceGooJLoc[4][2] = {{62+SR,344+SD   },{662+SR,344+SD   },{262+SR,374-SU   },{462+SR,374-SU   }};
int cmchstGooJLoc[4][2] = {{62+SR,344+SD+25},{662+SR,344+SD+25},{262+SR,374-SU+25},{462+SR,374-SU+25}};
RECT chanceJailBox[4] = {{ 62+SR,344+SD   , 62+SR+33,344+SD+19   },{662+SR,344+SD   ,662+SR+33,344+SD+19},
                         {262+SR,374-SU   ,262+SR+33,374-SU+19   },{462+SR,374-SU   ,462+SR+33,374-SU+19}};
RECT cmchstJailBox[4] = {{ 62+SR,344+25+SD, 62+SR+33,344+SD+25+19},{662+SR,344+SD+25,662+SR+33,344+SD+25+19},
                         {262+SR,374+25-SU,262+SR+33,374-SU+25+19},{462+SR,374-SU+25,462+SR+33,374-SU+25+19}};

// Where to put future/immunity icons
int futureLoc[6][2] = {{100+SR,347+SD},{700+SR,347+SD},{300+SR,377-SU},{500+SR,377-SU},   {274,347},{465,347}};
int immuneLoc[6][2] = {{100+SR,371+SD},{700+SR,371+SD},{300+SR,401-SU},{500+SR,401-SU},   {274,371},{465,371}};
// Buttons to activate future trade
RECT FutureTradeAT = { 100+SR, 347+SD, 100+SR+61, 347+SD+15 }, FutureTradeBT = { 700+SR, 347+SD, 700+SR+61, 347+SD+15 };
// Buttons to remove future trade
RECT FutureTradeAM = { 300+SR, 377-SU, 300+SR+61, 377-SU+15 }, FutureTradeBM = { 500+SR, 377-SU, 500+SR+61, 377-SU+15 };
// Buttons to activate immunity trade
RECT ImmunityTradeAT = { 100+SR, 371+SD, 100+SR+61, 371+SD+15 }, ImmunityTradeBT = { 700+SR, 371+SD, 700+SR+61, 371+SD+15 };
// Buttons to remove immunity trade
RECT ImmunityTradeAM = { 300+SR, 401-SU, 300+SR+61, 401-SU+15 }, ImmunityTradeBM = { 500+SR, 401-SU, 500+SR+61, 401-SU+15 };

RECT FutImmDlg = { 600, 0, 600+200, 0+225 };

// Buttons in cash trade popup (Screen Coordinates/Box Coordinates)
RECT M500SC, M500BC = { 7 + (25 * 0), 10, 7 + (25 * 0) + 24, 10+33 };
RECT M100SC, M100BC = { 7 + (25 * 1), 10, 7 + (25 * 1) + 24, 10+33 };
RECT M050SC, M050BC = { 7 + (25 * 2), 10, 7 + (25 * 2) + 24, 10+33 };
RECT M020SC, M020BC = { 7 + (25 * 3), 10, 7 + (25 * 3) + 24, 10+33 };
RECT M010SC, M010BC = { 7 + (25 * 4), 10, 7 + (25 * 4) + 24, 10+33 };
RECT M005SC, M005BC = { 7 + (25 * 5), 10, 7 + (25 * 5) + 24, 10+33 };
RECT M001SC, M001BC = { 7 + (25 * 6), 10, 7 + (25 * 6) + 26, 10+33 };
RECT OkaySC, OkayBC = {   7, 43,   7+50, 43+16 };
RECT ClrSC,  ClrBC =  {  68, 43,  68+50, 43+16 };
RECT CnclSC, CnclBC = { 129, 43, 129+50, 43+16 };
RECT FIOkaySC, FIOkayBC = { 100-(80/2), 187, 100+(80/2), 187+22 };

RECT FutSC = {   7, 197,   7+94, 197+23 };
RECT ImmSC = { 104, 197, 104+94, 197+23 };

RECT UpSC = { 767, 115, 767+18, 115+26 };
RECT DnSC = { 767, 155, 767+18, 155+26 };

RECT ProposeSC = { 202, 420, 202+101, 420+30 };
RECT CancelSC =  { 306, 420, 306+101, 420+30 };

int TradeAmt = 0, origTradeAmt = 0, origTradeAmt2 = 0;

wchar_t PropertyList[SQ_TOTAL_PROPERTY_SQUARES][64];
int PropertyProperties[SQ_TOTAL_PROPERTY_SQUARES];
int PropertyFlags[SQ_TOTAL_PROPERTY_SQUARES];
int ListOffset = 0;
int NumProperties = 0;

//LE_DATA_DataId tmpoverlay = NULL;
LE_DATA_DataId leftpanel = NULL, rightpanel = NULL, NameA = NULL, NameB = NULL;
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//
BOOL proposed = FALSE, showpropose = FALSE, CashTradeDying = FALSE;
BOOL AiProposing = FALSE;
int tradePanels = -1, desiredTradePanels = -1;

//// added on april 13 by David ethier ////
#define  UNCHANGED 0
#define  CHANGED 1

enum TradeBoxEnum
{
  BEFORE_A = 0,
  BEFORE_B,
  TRADE_A,
  TRADE_B,
  AFTER_A,
  AFTER_B
};

typedef struct _LOCATION_TAG_
{
  short  trade_box;
  short  state;
  short  column[11][3];
  short  numColumns;
  BOOL  isColumnCounted[11];
  LE_DATA_DataId ID[SQ_MAX_SQUARE_TYPES];
  BOOL  mortgaged[SQ_MAX_SQUARE_TYPES];
  BOOL  movingflag[SQ_MAX_SQUARE_TYPES];
  UNS16  Priority[SQ_MAX_SQUARE_TYPES];
  RECT  Location[SQ_MAX_SQUARE_TYPES];
} PropertyPositionStruct;

static PropertyPositionStruct  DesiredBeforePropertyA, DesiredBeforePropertyB;
static PropertyPositionStruct  TradeBeforePropertyA, TradeBeforePropertyB;
static PropertyPositionStruct  DesiredAfterPropertyA, DesiredAfterPropertyB;

LE_DATA_DataId movingcard = LED_EI;
PropertyPositionStruct *movingcardset, *movingcardset1, *movingfromset;
int movingcard0, movingcardcnt;
int movingcardpri, movingcardpos[2] = {0,0}, movingcarddelta[2] = {0,0};

// global that will help processor efficiency
static BOOL g_bFirstTime;
static DWORD LastTime = 0;
static DWORD MoveTime = 0;
int moveflag;

void UDTrade_InitPropertyStruct(void);
void UDTrade_PositionPropertyOwned(PropertyPositionStruct* pps, int order, int index);
void UDTrade_ResetPropertyPositions(PropertyPositionStruct* pps);
void UDTrade_CountColumns(RULE_PropertySet DesiredStuff, RULE_PropertySet DesiredStuff1, PropertyPositionStruct* pps);
void UDTrade_SortPropertyPositions(PropertyPositionStruct* pps);
void UDTrade_RepositionProperties(PropertyPositionStruct* pps, int order, int index, int box);
int UDTrade_MouseinProp(int, int);
int UDTrade_MouseinList(int, int);
void UDTrade_ProcessEverything(void);

//// added on april 22 by David ethier ////
#define DLG_MAX_STR_LENGTH  RULE_MAX_PLAYER_NAME_LENGTH + 15

typedef struct _DIALOG_TAG_
{
  BOOL           isVisible;
  LE_DATA_DataId ID;
  UNS16          priority;
  ACHAR          messageStr[RULE_MAX_PLAYERS][DLG_MAX_STR_LENGTH];
  short          x,y,w,h;
  int            playerToTrade;
  LE_DATA_DataId tokenID;
  RECT           tokenLocation[RULE_MAX_PLAYERS];
  int            mode; //added
} DialogStruct;

static DialogStruct PlayerSelectDlg;
static DialogStruct CashTradeDlg;
static DialogStruct FutureTradeDlg;
static DialogStruct ImmunityTradeDlg;

void UDTrade_InitPlayerSelect(void);
void UDTrade_DisplayPlayerSelectDlg(void);
void UDTrade_RemovePlayerSelectDlg(void);
void UDTrade_ProcessPlayerSelection(int choice);

void UDTrade_InitCashTrade(void);
void UDTrade_DisplayCashTradeDlg(int side);
void UDTrade_RemoveCashTradeDlg(void);
void UDTrade_ProcessCashTrade(int msg, int a, int b);

void UDTrade_InitFutureTrade(void);
void UDTrade_DisplayFutureTradeDlg(int side);
void UDTrade_RemoveFutureTradeDlg(void);
void UDTrade_ProcessFutureTrade(int msg, int a, int b);

void UDTrade_InitImmunityTrade(void);
void UDTrade_DisplayImmunityTradeDlg(int side);
void UDTrade_RemoveImmunityTradeDlg(void);
void UDTrade_ProcessImmunityTrade(int msg, int a, int b);
///////////////////////////////////////////////////////////////////////////

/*****************************************************************************
* UDTrade_GetPlayerToTradeFrom() - Return a valid trade from player.  (local
* non-bankrupt human)  Start from the current IBar player (except the bank)
* and work backwards.  If no valid player exists, return RULE_MAX_PLAYERS.
*****************************************************************************/
int UDTrade_GetPlayerToTradeFrom(void)
{
  int plyr, i;

  plyr = DISPLAY_state.IBarCurrentPlayer;
  if (plyr >= UICurrentGameState.NumberOfPlayers) // Not the bank!
    plyr = UICurrentGameState.NumberOfPlayers - 1;

  i = plyr;
  while ((!SlotIsALocalHumanPlayer[plyr]) ||
         (UICurrentGameState.Players[plyr].currentSquare >= SQ_OFF_BOARD)) {
    plyr--;
    if (plyr < 0)
      plyr = UICurrentGameState.NumberOfPlayers - 1;
    if (plyr == i)
      { plyr = RULE_MAX_PLAYERS; break; }
  }
  return(plyr);
}

/*****************************************************************************
* DISPLAY_UDTRADE_Initialize
*****************************************************************************/
/*****************************************************************************
 * All display initialization, showing, hiding and destroying occurs in
 * the following 4 subroutines called from DISPLAY.CPP.
 *
 * No code other than the show module may start nor stop sound
 * and graphic items.  Fields indicating desired actions can be filled in
 * in the display state which the show routine will pick up.  In this way,
 * the show routine can take responsibility for tracking all data
 * required to pause or clear the screen at any time.
 *
 * There will be no locking the computer as a wait for all sequences to finish
 * might do.  Stack processing can be halted until sequences are finished,
 * but you will have to use a timer to poll your sequence status.  This is to
 * allow processing of pause buttons or user camera controls at all times, as
 * well as to make programmers lives more difficult :)
 */
void DISPLAY_UDTRADE_Initialize(void)
{
  // Initialize any permanent display objects.  Avoid temporary ones when possible.
  register int iTemp;

//////////////////////////////////////////////////////////////////////////
// test stuff to verify clickable bounding box areas /////////////////////
//  if (tmpoverlay == NULL) {
//    tmpoverlay = LE_GRAFIX_ObjectCreateAlpha(800, 600, LE_GRAFIX_ObjectTransparent, 128);
//    LE_SEQNCR_StartXY(tmpoverlay, 3000, 0, 0);
//  }
  if (leftpanel == NULL)   leftpanel = LE_GRAFIX_ObjectCreate(200, 225, LE_GRAFIX_ObjectTransparent);
  LE_GRAFIX_ColorArea(leftpanel, 0, 0, 200, 225, LEG_MCR(0,255,0));

  int i, j = 14, numlines, height;
  wchar_t WrappedLines[60][512];
  LE_FONTS_SetSettings(0);
  LE_FONTS_SetSize(12);
  LE_FONTS_SetWeight(700);
  LE_FONTS_SetUnderline(TRUE);
  LE_FONTS_SetSettings(7);

  numlines = CHAT_WordWrap(LANG_GetTextMessage(TMN_TRADING), 176, (wchar_t **)WrappedLines, 7);
  height = LE_FONTS_GetStringHeight(CHAT_WrappedLines[0]);
  for (i = 0; i < numlines; i++, j += height)
    LE_FONTS_Print(leftpanel, (200 - LE_FONTS_GetStringWidth(CHAT_WrappedLines[i])) / 2, j,
      LEG_MCR(220, 220, 220), CHAT_WrappedLines[i]);
  j += 3;

  LE_FONTS_SetSize(9);
  LE_FONTS_SetWeight(700);
  LE_FONTS_SetUnderline(FALSE);
  LE_FONTS_SetSettings(7);

  numlines = CHAT_WordWrap(LANG_GetTextMessage(TMN_TRADING2), 176, (wchar_t **)WrappedLines, 7);
  height = LE_FONTS_GetStringHeight(CHAT_WrappedLines[0]);
  for (i = 0; i < numlines; i++, j += height)
    LE_FONTS_Print(leftpanel, (200 - LE_FONTS_GetStringWidth(CHAT_WrappedLines[i])) / 2, j,
      LEG_MCR(220, 220, 220), CHAT_WrappedLines[i]);

  LE_FONTS_GetSettings(0);

  if (rightpanel == NULL)  rightpanel = LE_GRAFIX_ObjectCreate(200, 225, LE_GRAFIX_ObjectTransparent);
//  LE_GRAFIX_ColorArea(rightpanel, 0, 0, 200, 225, LEG_MCR(0,255,0));
  if (NameA == NULL)  NameA = LE_GRAFIX_ObjectCreate(135, 35, LE_GRAFIX_ObjectTransparent);
//  LE_GRAFIX_ColorArea(NameA, 0, 0, 135, 35, LEG_MCR(0,255,0));
  if (NameB == NULL)  NameB = LE_GRAFIX_ObjectCreate(135, 35, LE_GRAFIX_ObjectTransparent);
//  LE_GRAFIX_ColorArea(NameB, 0, 0, 135, 35, LEG_MCR(0,255,0));

  LE_FONTS_SetSettings(0);
  LE_FONTS_SetSize(8);
  LE_FONTS_SetWeight(700);
  LE_FONTS_SetSettings(7);
  LE_FONTS_GetSettings(0);

  TradeATokenIDShown = TradeBTokenIDShown = LED_EI;
  TradeAStuff = TradeAStuffMort = TradeBStuff = TradeBStuffMort =
    TradeAAfter = TradeAAfterMort = TradeBAfter = TradeBAfterMort =
    TradePropsA = TradePropsAMort = TradePropsB = TradePropsBMort = 0;

  // Set up the player trade amount boxes
  for (iTemp = 0; iTemp < TradeCashBoxes; iTemp ++)
  {
    DISPLAY_state.TradeCashBoxObject[iTemp] = LE_GRAFIX_ObjectCreate(50, 11, LE_GRAFIX_ObjectTransparent);
    DISPLAY_state.TradeCashBoxValues[iTemp] = CASHTRADEINIT;
    DISPLAY_state.TradeCashBoxDesiredValue[iTemp] = 0;
    DISPLAY_state.TradeCashBoxShown[iTemp] = FALSE;
  }

  // Jail cards
  DISPLAY_state.TradeJailCardsShown[0] = DISPLAY_state.TradeJailCardsShown[1] = 0;
  DISPLAY_state.TradeJailCardsDesired[0] = DISPLAY_state.TradeJailCardsDesired[1] = 0;

  if (UICurrentGameState.Cards[CHANCE_DECK].jailOwner == TradeAPlayerActive)
    DISPLAY_state.TradeJailCardsDesired[0] = (1 << 0);
  else if (UICurrentGameState.Cards[CHANCE_DECK].jailOwner == TradeBPlayerActive)
    DISPLAY_state.TradeJailCardsDesired[0] = (1 << 1);

  if (UICurrentGameState.Cards[COMMUNITY_DECK].jailOwner == TradeAPlayerActive)
    DISPLAY_state.TradeJailCardsDesired[1] = (1 << 0);
  else if (UICurrentGameState.Cards[COMMUNITY_DECK].jailOwner == TradeBPlayerActive)
    DISPLAY_state.TradeJailCardsDesired[1] = (1 << 1);

  // Futures/Immunities
  for (iTemp = 0; iTemp < RULE_MAX_COUNT_HIT_SETS; iTemp++) {
    UICurrentGameState.CountHits[iTemp].toPlayer = RULE_NOBODY_PLAYER;
    UICurrentGameState.CountHits[iTemp].tradedItem = FALSE;
  }

  DISPLAY_state.TradeImmunityFutureShownA[0] = DISPLAY_state.TradeImmunityFutureShownA[1] = 0;
  DISPLAY_state.TradeImmunityFutureDesiredA[0] = DISPLAY_state.TradeImmunityFutureDesiredA[1] = 0;

  // And the 4 boxes locations
  TradeBasicPlayerBoxes[0][0] = 0;   // A before
  TradeBasicPlayerBoxes[0][1] = 225;
  TradeBasicPlayerBoxes[1][0] = 600; // B before
  TradeBasicPlayerBoxes[1][1] = 225;
  TradeBasicPlayerBoxes[2][0] = 200; // A after
  TradeBasicPlayerBoxes[2][1] = 255-11;
  TradeBasicPlayerBoxes[3][0] = 400; // B after
  TradeBasicPlayerBoxes[3][1] = 255-11; // -11?

  UDTrade_InitPropertyStruct(); // initialize the property structures for the trade screen
  UDTrade_InitPlayerSelect();   // initialize the player selection dialog box
  UDTrade_InitCashTrade();      // initialize the cash trade dialog box
  UDTrade_InitFutureTrade();    // initialize the future trade dialog box
  UDTrade_InitImmunityTrade();  // initialize the immunity trade dialog box
}

/*****************************************************************************
* DISPLAY_UDTRADE_Destroy
*****************************************************************************/
void DISPLAY_UDTRADE_Destroy(void)
{ // Deallocate stuff from Initialize.
  for (int i = 0; i < TradeCashBoxes; i++) {
    LE_DATA_FreeRuntimeDataID(DISPLAY_state.TradeCashBoxObject[i]);
    DISPLAY_state.TradeCashBoxObject[i] = LED_EI;
  }
  if (PlayerSelectDlg.ID) {
    LE_DATA_FreeRuntimeDataID(PlayerSelectDlg.ID);
    PlayerSelectDlg.ID = LED_EI;
  }
//  if (tmpoverlay) {
//    LE_DATA_FreeRuntimeDataID(tmpoverlay);
//    tmpoverlay = LED_EI;
//  }
  if (leftpanel) {
    LE_DATA_FreeRuntimeDataID(leftpanel);
    leftpanel = LED_EI;
  }
  if (rightpanel) {
    LE_DATA_FreeRuntimeDataID(rightpanel);
    rightpanel = LED_EI;
  }
  if (NameA) {
    LE_DATA_FreeRuntimeDataID(NameA);
    NameA = LED_EI;
  }
  if (NameB) {
    LE_DATA_FreeRuntimeDataID(NameB);
    NameB = LED_EI;
  }
}

/*****************************************************************************
* DISPLAY_UDTRADE_Show
*****************************************************************************/
void DISPLAY_UDTRADE_Show(void)
{
  if ((DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA) && g_bFirstTime)
  {
//    if ((PlayerSelectDlg.isVisible) || (CashTradeDlg.isVisible) ||
//        (FutureTradeDlg.isVisible) || (ImmunityTradeDlg.isVisible))  return;
    if ((!AiProposing) && (!UICurrentGameState.TradeInProgress))
      UDTrade_DisplayPlayerSelectDlg();
    AiProposing = g_bFirstTime = FALSE;
  } else if ((DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA) && !g_bFirstTime) {
    DWORD CurrentTime, DeltaTime;
    CurrentTime = GetTickCount();
    DeltaTime = CurrentTime - LastTime;

    if ((movingcardcnt != 0) && ((CurrentTime - MoveTime) >= 25)) {
      MoveTime = CurrentTime;
      if (movingcardcnt > 0) {
        if (movingcardcnt == MOVESTEP) {
          LE_SEQNCR_Stop(movingcard, movingcardpri);
          movingcard = LED_EI;
          movingcardcnt = moveflag = 0;
          UDTrade_ProcessEverything();
        } else if (movingcardcnt == 1) {
        } else {
          movingcardpos[0] += movingcarddelta[0];
          movingcardpos[1] += movingcarddelta[1];
          LE_SEQNCR_MoveXY(movingcard, movingcardpri, movingcardpos[0], movingcardpos[1]);
          movingcardcnt++;
        }
      } else /*if (movingcardcnt < 0)*/ {
        if (movingcardcnt == -MOVESTEP) {
          LE_SEQNCR_Stop(movingcard, movingcardpri);
          movingcard = LED_EI;
          movingcardcnt = moveflag = 0;
          UDTrade_ProcessEverything();
        } else if (movingcardcnt == -1) {
        } else {
          movingcardpos[0] += movingcarddelta[0];
          movingcardpos[1] += movingcarddelta[1];
          LE_SEQNCR_MoveXY(movingcard, movingcardpri, movingcardpos[0], movingcardpos[1]);
          movingcardcnt--;
        }
      }
    }

    if (DeltaTime < 500 /* 500 milliseconds */) //LE_TIME_TickCount - 60ths/sec
      return;
    LastTime = CurrentTime;
    UDTrade_ProcessEverything();
  } else if (DISPLAY_state.desired2DView != DISPLAY_SCREEN_TradeA) {
    if (PlayerSelectDlg.isVisible)   UDTrade_RemovePlayerSelectDlg();
    if (CashTradeDlg.isVisible)      UDTrade_RemoveCashTradeDlg();
    if (FutureTradeDlg.isVisible)    UDTrade_RemoveFutureTradeDlg();
    if (ImmunityTradeDlg.isVisible)  UDTrade_RemoveImmunityTradeDlg();
    if (!g_bFirstTime) {
      UDTrade_ProcessEverything(); // this will clear the screen of deeds
      g_bFirstTime = TRUE;
    }
  }
}

/*****************************************************************************
* UDTRADE_ProcessMessage
*****************************************************************************/
/*****************************************************************************
* Standard UI subfunction - Process library message. */
void UDTRADE_ProcessMessage(LE_QUEUE_MessagePointer UIMessagePntr)
{
  // NOTE: IBar trade functions are handled in IBAR.cpp except for the trade bar alone which is here.
  // The trade screen is also handled here - just the general IBAR functions are not since
  // IBar has to check all the general buttons for mouseover and click anyway.
  register int iTemp, x = UIMessagePntr->numberA, y = UIMessagePntr->numberB;
  int s, t, box, deed;
  BOOL Flag = FALSE;
  RULE_ActionArgumentsRecord TradeArguments = {
    ACTION_TRADE_ITEM, 0, {0, ""}, RULE_BANK_PLAYER, 0, 0, 0, 0, 0, A_T(""), NULL, 0, 0, NULL };


    ////////////////////////////////////////////////////////////////////////
    // added by David Ethier on Septemboer 30
    
    // if g_firsttime flag has been set, then we are not in the trade screen
    // this is to prevent a mouse message in UDOPTS that returns to the trade screen
    // from affecting the process message of the trade screen
    if(g_bFirstTime && (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA)
        && (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN))
        return;

///////////////////////////////////////////////////////////////
// test stuff to verify clickable bounding box areas
/*
DrawBox(CashTradeAT1); DrawBox(CashTradeAT2);
DrawBox(CashTradeBT1); DrawBox(CashTradeBT2);
DrawBox(CashTradeAM1); DrawBox(CashTradeAM2);
DrawBox(CashTradeBM1); DrawBox(CashTradeBM2);
DrawBox(chanceJailBox[0]); DrawBox(chanceJailBox[1]); DrawBox(chanceJailBox[2]); DrawBox(chanceJailBox[3]);
DrawBox(cmchstJailBox[0]); DrawBox(cmchstJailBox[1]); DrawBox(cmchstJailBox[2]); DrawBox(cmchstJailBox[3]);
DrawBox(FutureTradeAT); DrawBox(ImmunityTradeAT);
DrawBox(FutureTradeBT); DrawBox(ImmunityTradeBT);
DrawBox(FutureTradeAM); DrawBox(ImmunityTradeAM);
DrawBox(FutureTradeBM); DrawBox(ImmunityTradeBM);
DrawBox(UpSC); DrawBox(DnSC);
*/
///////////////////////////////////////////////////////////////

  if ((DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA) && (!PlayerSelectDlg.isVisible)) {
    if ((UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN) && (CashTradeDlg.isVisible)) {
      UDTrade_ProcessCashTrade(UIMSG_MOUSE_LEFT_DOWN, x, y);       return;
    } else if ((UIMessagePntr->messageCode == UIMSG_KEYBOARD_ACHAR) && (CashTradeDlg.isVisible)) {
      UDTrade_ProcessCashTrade(UIMSG_KEYBOARD_ACHAR, x, y);        return;
    } else if ((UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN) && (FutureTradeDlg.isVisible)) {
      UDTrade_ProcessFutureTrade(UIMSG_MOUSE_LEFT_DOWN, x, y);     return;
    } else if ((UIMessagePntr->messageCode == UIMSG_KEYBOARD_ACHAR) && (FutureTradeDlg.isVisible)) {
      UDTrade_ProcessFutureTrade(UIMSG_KEYBOARD_ACHAR, x, y);      return;
    } else if ((UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN) && (ImmunityTradeDlg.isVisible)) {
      UDTrade_ProcessImmunityTrade(UIMSG_MOUSE_LEFT_DOWN, x, y);   return;
    } else if ((UIMessagePntr->messageCode == UIMSG_KEYBOARD_ACHAR) && (ImmunityTradeDlg.isVisible)) {
      UDTrade_ProcessImmunityTrade(UIMSG_KEYBOARD_ACHAR, x, y);    return;
    }
  }
/////////////////////////////////////foozle

  if (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN &&
      DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA &&
      //UICurrentGameState.TradeInProgress &&
      !TradeDisplayInEditModeNotView && // remove this condition when using our own buttons (not IBAR's).
      !PlayerSelectDlg.isVisible)
  {
    // View Futures and Immunities
    if ((PointInRect(x, y, FutureTradeAM)) && (DISPLAY_state.TradeImmunityFutureDesiredA[0] & (1 << 2))) {
      FutureTradeDlg.mode = 6;    UDTrade_DisplayFutureTradeDlg(0);
    } else if ((PointInRect(x, y, ImmunityTradeAM)) && (DISPLAY_state.TradeImmunityFutureDesiredA[1] & (1 << 2))) {
      ImmunityTradeDlg.mode = 6;  UDTrade_DisplayImmunityTradeDlg(0);
    } else if ((PointInRect(x, y, FutureTradeBM)) && (DISPLAY_state.TradeImmunityFutureDesiredA[0] & (1 << 3))) {
      FutureTradeDlg.mode = 6;    UDTrade_DisplayFutureTradeDlg(1);
    } else if ((PointInRect(x, y, ImmunityTradeBM)) && (DISPLAY_state.TradeImmunityFutureDesiredA[1] & (1 << 3))) {
      ImmunityTradeDlg.mode = 6;  UDTrade_DisplayImmunityTradeDlg(1);
    }
  }

  if (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN &&
      DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA &&
      //UICurrentGameState.TradeInProgress &&
      TradeDisplayInEditModeNotView && // remove this condition when using our own buttons (not IBAR's).
      !PlayerSelectDlg.isVisible)
  {
    // *** Cash ***********************************************************

    if (((PointInRect(x, y, CashTradeAT1)) ||
         (PointInRect(x, y, CashTradeAT2))) && (!CashTradeDlg.isVisible))
      UDTrade_DisplayCashTradeDlg(0);
    else if (((PointInRect(x, y, CashTradeBT1)) ||
             (PointInRect(x, y, CashTradeBT2))) && (!CashTradeDlg.isVisible))
      UDTrade_DisplayCashTradeDlg(1);

    if (((PointInRect(x, y, CashTradeAM1)) ||
         (PointInRect(x, y, CashTradeAM2))) && (!CashTradeDlg.isVisible)) {
      for (s = -1, t = 0; t < TradeItemListTop; t++)
        if ((UDTradeItemList[t].numberC == TIK_CASH) && (UDTradeItemList[t].numberA == TradeAPlayerActive))
          { s = t; break; }
      if (s != -1) {
        DISPLAY_state.TradeCashBoxDesiredValue[2] = 0;
        UDTradeItemList[s].numberD = 0;
      }
    } else if (((PointInRect(x, y, CashTradeBM1)) ||
                (PointInRect(x, y, CashTradeBM2))) && (!CashTradeDlg.isVisible)) {
      for (s = -1, t = 0; t < TradeItemListTop; t++)
        if ((UDTradeItemList[t].numberC == TIK_CASH) && (UDTradeItemList[t].numberA == TradeBPlayerActive))
          { s = t; break; }
      if (s != -1) {
        DISPLAY_state.TradeCashBoxDesiredValue[3] = 0;
        UDTradeItemList[s].numberD = 0;
      }
    }

    DISPLAY_state.TradeCashBoxDesiredValue[0] =
      UICurrentGameState.Players[TradeAPlayerActive].cash - DISPLAY_state.TradeCashBoxDesiredValue[2];
    DISPLAY_state.TradeCashBoxDesiredValue[1] =
      UICurrentGameState.Players[TradeBPlayerActive].cash - DISPLAY_state.TradeCashBoxDesiredValue[3];

    // *** Properties *****************************************************

    iTemp = UDTrade_MouseinProp(x, y);
    if ((movingcardcnt == 0) && (iTemp >= 0)) {
      box = iTemp / 100;
      deed = iTemp % 100;

      if ((box == 0) || (box == 1))
      { // We still have to see if there is an item in this position before registering the hit.
        TradeArguments.fromPlayer = TradeFromPlayer;
        TradeArguments.numberA = (int)UICurrentGameState.Squares[deed].owner; // This should be tradeAPlayer, of course... or FromPlayer;
        TradeArguments.numberC = (int)TIK_SQUARE;
        TradeArguments.numberD = deed;
        movingcard0 = deed;
        movingcardcnt = 1;

        if (box == 0) {
          if ((TradeAAfter | TradeAAfterMort) & RULE_PropertyToBitSet(deed)) // Use the after array to avoid duplicate items
          {
            TradeBeforePropertyA.movingflag[deed] = TRUE;
            TradeArguments.numberB = TradeBPlayerActive;
            Trade_AddItem(&TradeArguments);
          }
        } else {
          if ((TradeBAfter | TradeBAfterMort) & RULE_PropertyToBitSet(deed))
          {
            TradeBeforePropertyB.movingflag[deed] = TRUE;
            TradeArguments.numberB = TradeAPlayerActive;
            Trade_AddItem(&TradeArguments);
          }
        }
        UDTrade_ResetPropertyPositions(&DesiredBeforePropertyA);
        UDTrade_ResetPropertyPositions(&DesiredBeforePropertyB);
        UDTrade_ResetPropertyPositions(&TradeBeforePropertyA);
        UDTrade_ResetPropertyPositions(&TradeBeforePropertyB);
        UDTrade_CountColumns(TradeAStuff, TradeAStuffMort, &DesiredBeforePropertyA);
        UDTrade_CountColumns(TradeBStuff, TradeBStuffMort, &DesiredBeforePropertyB);
        UDTrade_CountColumns(TradePropsA, TradePropsAMort, &TradeBeforePropertyA);
        UDTrade_CountColumns(TradePropsB, TradePropsBMort, &TradeBeforePropertyB);
        UDTrade_RepositionProperties(&DesiredBeforePropertyA, 0, 0, 0);
        UDTrade_RepositionProperties(&DesiredBeforePropertyB, 0, 0, 1);
        UDTrade_RepositionProperties(&TradeBeforePropertyA, 0, 0, 2);
        UDTrade_RepositionProperties(&TradeBeforePropertyB, 0, 0, 3);
      }
      if ((box == 2) || (box == 3))
      {
        for (s = -1, t = 0; t < TradeItemListTop; t++)
          if ((UDTradeItemList[t].numberC == TIK_SQUARE) &&
              (UDTradeItemList[t].numberD == deed))
          { s = t;  break; }
        if (s != -1) {
          for (int qlx = s; qlx < TradeItemListTop-1; qlx++)
            memcpy(&UDTradeItemList[qlx], &UDTradeItemList[qlx+1], sizeof(RULE_ActionArgumentsRecord));
          TradeItemListTop--;
          movingcard0 = deed;
          movingcardcnt = -1;
        }
        if (box == 2)  DesiredBeforePropertyA.movingflag[deed] = TRUE;
        else           DesiredBeforePropertyB.movingflag[deed] = TRUE;

        UDTrade_ResetPropertyPositions(&DesiredBeforePropertyA);
        UDTrade_ResetPropertyPositions(&DesiredBeforePropertyB);
        UDTrade_ResetPropertyPositions(&TradeBeforePropertyA);
        UDTrade_ResetPropertyPositions(&TradeBeforePropertyB);
        UDTrade_CountColumns(TradeAStuff, TradeAStuffMort, &DesiredBeforePropertyA);
        UDTrade_CountColumns(TradeBStuff, TradeBStuffMort, &DesiredBeforePropertyB);
        UDTrade_CountColumns(TradePropsA, TradePropsAMort, &TradeBeforePropertyA);
        UDTrade_CountColumns(TradePropsB, TradePropsBMort, &TradeBeforePropertyB);
        UDTrade_RepositionProperties(&DesiredBeforePropertyA, 0, 0, 0);
        UDTrade_RepositionProperties(&DesiredBeforePropertyB, 0, 0, 1);
        UDTrade_RepositionProperties(&TradeBeforePropertyA, 0, 0, 2);
        UDTrade_RepositionProperties(&TradeBeforePropertyB, 0, 0, 3);
      }
    }

    // *** Jail cards *****************************************************

    if (PointInRect(x, y, chanceJailBox[0])) {
      if (DISPLAY_state.TradeJailCardsDesired[0] & (1 << 0)) {
        DISPLAY_state.TradeJailCardsDesired[0] = (1 << 2);

        TradeArguments.fromPlayer = TradeFromPlayer;
        TradeArguments.numberA = TradeAPlayerActive;
        TradeArguments.numberB = TradeBPlayerActive;
        TradeArguments.numberC = (int)TIK_JAIL_CARD;
        TradeArguments.numberD = CHANCE_DECK; //Amount or square (cash/prop);
        Trade_AddItem(&TradeArguments);
      }
    } else if (PointInRect(x, y, chanceJailBox[1])) {
      if (DISPLAY_state.TradeJailCardsDesired[0] & (1 << 1)) {
        DISPLAY_state.TradeJailCardsDesired[0] = (1 << 3);

        TradeArguments.fromPlayer = TradeFromPlayer;
        TradeArguments.numberA = TradeBPlayerActive;
        TradeArguments.numberB = TradeAPlayerActive;
        TradeArguments.numberC = (int)TIK_JAIL_CARD;
        TradeArguments.numberD = CHANCE_DECK; //Amount or square (cash/prop);
        Trade_AddItem(&TradeArguments);
      }
    } else if (PointInRect(x, y, chanceJailBox[2])) {
      if (DISPLAY_state.TradeJailCardsDesired[0] & (1 << 2)) {
        DISPLAY_state.TradeJailCardsDesired[0] = (1 << 0);

        for (s = -1, t = 0; t < TradeItemListTop; t++)
          if (UDTradeItemList[t].numberC == TIK_JAIL_CARD)
            if (UDTradeItemList[t].numberD == CHANCE_DECK)
              { s = t; break; }
        if (s != -1)
        {
          for (int qlx = s; qlx < TradeItemListTop-1; qlx++)
            memcpy(&UDTradeItemList[qlx], &UDTradeItemList[qlx+1], sizeof(RULE_ActionArgumentsRecord));
          TradeItemListTop--;
        }
      }
    } else if (PointInRect(x, y, chanceJailBox[3])) {
      if (DISPLAY_state.TradeJailCardsDesired[0] & (1 << 3)) {
        DISPLAY_state.TradeJailCardsDesired[0] = (1 << 1);

        for (s = -1, t = 0; t < TradeItemListTop; t++)
          if (UDTradeItemList[t].numberC == TIK_JAIL_CARD)
            if (UDTradeItemList[t].numberD == CHANCE_DECK)
              { s = t; break; }
        if (s != -1)
        {
          for (int qlx = s; qlx < TradeItemListTop-1; qlx++)
            memcpy(&UDTradeItemList[qlx], &UDTradeItemList[qlx+1], sizeof(RULE_ActionArgumentsRecord));
          TradeItemListTop--;
        }
      }
    } else if (PointInRect(x, y, cmchstJailBox[0])) {
      if (DISPLAY_state.TradeJailCardsDesired[1] & (1 << 0)) {
        DISPLAY_state.TradeJailCardsDesired[1] = (1 << 2);

        TradeArguments.fromPlayer = TradeFromPlayer;
        TradeArguments.numberA = TradeAPlayerActive;
        TradeArguments.numberB = TradeBPlayerActive;
        TradeArguments.numberC = (int)TIK_JAIL_CARD;
        TradeArguments.numberD = COMMUNITY_DECK; //Amount or square (cash/prop);
        Trade_AddItem(&TradeArguments);
      }
    } else if (PointInRect(x, y, cmchstJailBox[1])) {
      if (DISPLAY_state.TradeJailCardsDesired[1] & (1 << 1)) {
        DISPLAY_state.TradeJailCardsDesired[1] = (1 << 3);

        TradeArguments.fromPlayer = TradeFromPlayer;
        TradeArguments.numberA = TradeBPlayerActive;
        TradeArguments.numberB = TradeAPlayerActive;
        TradeArguments.numberC = (int)TIK_JAIL_CARD;
        TradeArguments.numberD = COMMUNITY_DECK; //Amount or square (cash/prop);
        Trade_AddItem(&TradeArguments);
      }
    } else if (PointInRect(x, y, cmchstJailBox[2])) {
      if (DISPLAY_state.TradeJailCardsDesired[1] & (1 << 2)) {
        DISPLAY_state.TradeJailCardsDesired[1] = (1 << 0);

        for (s = -1, t = 0; t < TradeItemListTop; t++)
          if (UDTradeItemList[t].numberC == TIK_JAIL_CARD)
            if (UDTradeItemList[t].numberD == COMMUNITY_DECK)
              { s = t; break; }
        if (s != -1)
        {
          for (int qlx = s; qlx < TradeItemListTop-1; qlx++)
            memcpy(&UDTradeItemList[qlx], &UDTradeItemList[qlx+1], sizeof(RULE_ActionArgumentsRecord));
          TradeItemListTop--;
        }
      }
    } else if (PointInRect(x, y, cmchstJailBox[3])) {
      if (DISPLAY_state.TradeJailCardsDesired[1] & (1 << 3)) {
        DISPLAY_state.TradeJailCardsDesired[1] = (1 << 1);

        for (s = -1, t = 0; t < TradeItemListTop; t++)
          if (UDTradeItemList[t].numberC == TIK_JAIL_CARD)
            if (UDTradeItemList[t].numberD == COMMUNITY_DECK)
              { s = t; break; }
        if (s != -1)
        {
          for (int qlx = s; qlx < TradeItemListTop-1; qlx++)
            memcpy(&UDTradeItemList[qlx], &UDTradeItemList[qlx+1], sizeof(RULE_ActionArgumentsRecord));
          TradeItemListTop--;
        }
      }
    }

    // *** Futures ********************************************************

    if ((PointInRect(x, y, FutureTradeAT)) && (!FutureTradeDlg.isVisible) &&
        (DISPLAY_state.TradeImmunityFutureDesiredA[0] & (1 << 0))) {
      FutureTradeDlg.mode = 4;
      UDTrade_DisplayFutureTradeDlg(0);
    } else if ((PointInRect(x, y, FutureTradeBT)) && (!FutureTradeDlg.isVisible) &&
               (DISPLAY_state.TradeImmunityFutureDesiredA[0] & (1 << 1))) {
      FutureTradeDlg.mode = 4;
      UDTrade_DisplayFutureTradeDlg(1);
    }

    if (PointInRect(x, y, FutureTradeAM)) {
      if (DISPLAY_state.TradeImmunityFutureDesiredA[0] & (1 << 2)) {
        FutureTradeDlg.mode = 5;
        UDTrade_DisplayFutureTradeDlg(0);
      }
    } else if (PointInRect(x, y, FutureTradeBM)) {
      if (DISPLAY_state.TradeImmunityFutureDesiredA[0] & (1 << 3)) {
        FutureTradeDlg.mode = 5;
        UDTrade_DisplayFutureTradeDlg(1);
      }
    }

    // *** Immunities *****************************************************

    if ((PointInRect(x, y, ImmunityTradeAT)) && (!ImmunityTradeDlg.isVisible) &&
        (DISPLAY_state.TradeImmunityFutureDesiredA[1] & (1 << 0))) {
      ImmunityTradeDlg.mode = 4;  UDTrade_DisplayImmunityTradeDlg(0);
    } else if ((PointInRect(x, y, ImmunityTradeBT)) && (!ImmunityTradeDlg.isVisible) &&
               (DISPLAY_state.TradeImmunityFutureDesiredA[1] & (1 << 1))) {
      ImmunityTradeDlg.mode = 4;  UDTrade_DisplayImmunityTradeDlg(1);
    }

    if (PointInRect(x, y, ImmunityTradeAM)) {
      if (DISPLAY_state.TradeImmunityFutureDesiredA[1] & (1 << 2)) {
        ImmunityTradeDlg.mode = 5;  UDTrade_DisplayImmunityTradeDlg(0);
      }
    } else if (PointInRect(x, y, ImmunityTradeBM)) {
      if (DISPLAY_state.TradeImmunityFutureDesiredA[1] & (1 << 3)) {
        ImmunityTradeDlg.mode = 5;  UDTrade_DisplayImmunityTradeDlg(1);
      }
    }

    // *** New Futures/Immunities *****************************************

    if ((PointInRect(x, y, FutSC)) &&
        (UICurrentGameState.GameOptions.futureRentTradingAllowed)) {
      FutureTradeDlg.mode = 0;    UDTrade_DisplayFutureTradeDlg(1);  UDSOUND_Click();
    }

    if ((PointInRect(x, y, ImmSC)) &&
        (UICurrentGameState.GameOptions.immunitiesTradingAllowed)) {
      ImmunityTradeDlg.mode = 0;  UDTrade_DisplayImmunityTradeDlg(1);  UDSOUND_Click();
    }

    // *** Propose ********************************************************

    if (PointInRect(x, y, ProposeSC)) {
      BOOL give = FALSE, get = FALSE;
      for (iTemp = 0; iTemp < TradeItemListTop; iTemp++) {
        if (UDTradeItemList[iTemp].numberA == TradeAPlayerActive) {
          if (UDTradeItemList[iTemp].numberC == TIK_CASH) {
            if (UDTradeItemList[iTemp].numberD != 0)
              give = TRUE;
          } else give = TRUE;
        }
        if (UDTradeItemList[iTemp].numberB == TradeAPlayerActive) {
          if (UDTradeItemList[iTemp].numberC == TIK_CASH) {
            if (UDTradeItemList[iTemp].numberD != 0)
              get = TRUE;
          } else get = TRUE;
        }
      }

      if (give && get) {
        if (showpropose) {
          UDSOUND_Click();
          MESS_SendAction (ACTION_START_TRADE_EDITING, TradeFromPlayer,
            RULE_BANK_PLAYER, 1, 0, 0, 0, NULL, 0, NULL);
        }
      } else {
        UDPENNY_PennybagsGeneral(PB_TradeScreen_ProposeClickedButOneSideHasNotOfferedAnything, CLIPACTION);
      }
    }

    // *** Cancel *********************************************************

    if (PointInRect(x, y, CancelSC)) {
      UDSOUND_Click();
      TradeFromPlayer = -1; // Forget who started the trade
      UDTrade_ClearTrade();
      TradeAPlayerActive = TradeBPlayerActive = RULE_MAX_PLAYERS;
UDTrade_ProcessEverything();
      DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Main;
      UDBOARD_SetBackdrop( DISPLAY_SCREEN_MainA );
      DISPLAY_showAll;
    }
  }

  //// added by David Ethier on April 22 ////
  // check for keyboard press
  if (UIMessagePntr->messageCode == UIMSG_KEYBOARD_ACHAR)
    if (PlayerSelectDlg.isVisible)
      UDTrade_ProcessPlayerSelection((x - '0'));

  //// added by David Ethier on May 19 ////
  // check to see if player is ai or if there are only two players
//  if (((UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].AIPlayerLevel > 0) ||
//       (UICurrentGameState.NumberOfPlayers < 3)) && (PlayerSelectDlg.isVisible))
//following if needed for two-player same-computer trades, it seems
//////  if (((UICurrentGameState.Players[TradeAPlayerActive].AIPlayerLevel > 0) ||
//////       (UICurrentGameState.NumberOfPlayers < 3)) && (PlayerSelectDlg.isVisible)) {
//////    TradeBPlayerActive = 1 - TradeAPlayerActive; // FIXME - hack to get a second in trade.
//////    UDTrade_ProcessPlayerSelection(TradeBPlayerActive + 1);
//////  }

  //// added by David Ethier on May 20 ////
  // check to see if user clicked on token to choose trading partner
  if ((UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN) &&
      (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA) && (PlayerSelectDlg.isVisible))
    for (int i = 0; i < UICurrentGameState.NumberOfPlayers; i++)
    {
      if (i == TradeAPlayerActive)  continue;
      if (UICurrentGameState.Players[i].currentSquare >= SQ_OFF_BOARD)  continue;
      if ((x >= (PlayerSelectDlg.x + PlayerSelectDlg.tokenLocation[i].left)) &&
          (x <  (PlayerSelectDlg.x + PlayerSelectDlg.tokenLocation[i].right)) &&
          (y >= (PlayerSelectDlg.y + PlayerSelectDlg.tokenLocation[i].top)) &&
          (y <  (PlayerSelectDlg.y + PlayerSelectDlg.tokenLocation[i].bottom)))
        UDTrade_ProcessPlayerSelection(i+1);
    }
}

/*****************************************************************************
* UDTRADE_ProcessMessageToPlayer
* Standard UI subfunction - Process game message to player.
*****************************************************************************/
void UDTRADE_ProcessMessageToPlayer(RULE_ActionArgumentsPointer NewMessage)
{
  int i;
#if USA_VERSION
  int temp;
#endif
//  register int iTemp;

  switch (NewMessage->action)
  {
    case NOTIFY_ACTION_COMPLETED:
      if (NewMessage->numberA == ACTION_NEW_GAME) {
        UICurrentGameState.TradeInProgress = FALSE;
        TradeFromPlayer = -1; // Forget who started the trade
        UDTrade_ClearTrade();
        TradeAPlayerActive = TradeBPlayerActive = RULE_MAX_PLAYERS;
      } else if (NewMessage->numberA == ACTION_START_TRADE_EDITING) {
        if (NewMessage->numberB) {
          if (PlayerSelectDlg.isVisible)   UDTrade_RemovePlayerSelectDlg();
          if (CashTradeDlg.isVisible)      UDTrade_RemoveCashTradeDlg();
          if (FutureTradeDlg.isVisible)    UDTrade_RemoveFutureTradeDlg();
          if (ImmunityTradeDlg.isVisible)  UDTrade_RemoveImmunityTradeDlg();
          if (showpropose) {
            LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_iytprpf), 202);
            LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_iytprpo), 202, 0, 0);//-106, 10);
            LE_SEQNCR_SetEndingAction(LED_IFT(DAT_LANG2, CNK_iytprpo), 202, LE_SEQNCR_EndingActionStop);

            LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_iytcxlf), 201);
            LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_iytcxlo), 201, 0, 0);//-106, 10);
            LE_SEQNCR_SetEndingAction(LED_IFT(DAT_LANG2, CNK_iytcxlo), 201, LE_SEQNCR_EndingActionStop);
            showpropose = FALSE;
          }
        } else {
          // Warn the user if they can't propose.  (ie: someone's building)
          // Only play warning sound on computer the message came from.
          if (SlotIsALocalHumanPlayer[(RULE_PlayerNumber)NewMessage->numberC])
            UDSOUND_Warning();
        }
      }
      break;

    case NOTIFY_TRADE_STARTED:
//      if ((DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA) &&
//          (UICurrentGameState.Players[NewMessage->numberA].AIPlayerLevel != 0))
      proposed = TRUE;

      if (!UICurrentGameState.TradeInProgress)
      { // Probably just started the trade - we get notified whenever the trade is resent.
        //UDIBAR_setDialog(DISPLAY_Dialog_None); // FIXME - this is used for CHANCE/CC CARDS, raise money
        UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, NewMessage->numberA );
        DISPLAY_state.former2DView = DISPLAY_state.desired2DView;
        UDBOARD_SetBackdrop(DISPLAY_SCREEN_TradeA);
        InitializeTradeSequence();
        AiProposing = FALSE;
        if (UICurrentGameState.Players[(RULE_PlayerNumber)NewMessage->numberA].AIPlayerLevel > 0)
          AiProposing = TRUE;
//        MESS_SendAction (ACTION_START_TRADE_EDITING, (RULE_PlayerNumber)NewMessage->numberA,
//          RULE_BANK_PLAYER, (RULE_PlayerNumber)NewMessage->numberA, 0, 0, 0, NULL, 0, NULL);
      } else {
        // Without this, if your trade is interrupted with a new trade involving
        // the same two traders, the new items are added on the display only.
        if (!SlotIsALocalHumanPlayer[(RULE_PlayerNumber)NewMessage->numberA]) {
          InitializeTradeSequence();
          UDTrade_ClearTrade();
        }
      }

///.///      InitializeTradeSequence();
      TradeAPlayerActive = (RULE_PlayerNumber)NewMessage->numberA;
      TradeFromPlayer = TradeAPlayerActive; // variable may be redundant - from player initiated trade, at least in edit (hotseat)
      TradeBPlayerActive = RULE_MAX_PLAYERS;// The playerset involved is not available yet.
//      desiredTradePanels = (TradeAPlayerActive * 10) + RULE_MAX_PLAYERS;

      TradeDisplayInEditModeNotView = FALSE; // Disable all editing during view/accept.
      UICurrentGameState.TradeInProgress = TRUE; // Mostly for the AI to use. Old comment, I don't see any AI referrence to this.
      break;

    case NOTIFY_TRADE_FINISHED:
      if (NewMessage->numberA != -1) {
        if (NewMessage->numberA == 1) //accepting
          UDPENNY_PennybagsGeneral(PB_TradeScreen_TradeComplete, CLIPACTION);
        else if (NewMessage->numberA == 0) //rejecting
          UDPENNY_PennybagsGeneral(PB_TradeScreen_TradeCancelled, CLIPACTION);
//        if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA) // We are on the trade screen, return to former view.
//          UDBOARD_SetBackdrop(DISPLAY_state.former2DView);
        UICurrentGameState.TradeInProgress = FALSE; // Mostly for the AI to use.
        TradeDisplayInEditModeNotView = TRUE; // Can edit again.

        AiProposing = FALSE;
        TradeFromPlayer = -1; // Forget who started the trade
        UDTrade_ClearTrade();
        TradeAPlayerActive = TradeBPlayerActive = RULE_MAX_PLAYERS;
        UDTrade_ProcessEverything();
//        if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA) { // Still on trade screen?  Guess we started here - go to main.  Remove this when we have methods of selecting from & to.
          DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Main;
          UDBOARD_SetBackdrop(DISPLAY_SCREEN_MainA);
//        }
      } else {
        // countering
        TradeFromPlayer = TradeBPlayerActive;
        UICurrentGameState.TradeInProgress = FALSE; // Mostly for the AI to use.
if (SlotIsALocalHumanPlayer[TradeFromPlayer]) {
        for (i = 0; i < TradeItemListTop; i++)
          UDTradeItemList[i].fromPlayer = TradeBPlayerActive;

        int tmp = TradeAPlayerActive;
        TradeAPlayerActive = TradeBPlayerActive;
        TradeBPlayerActive = tmp;
        desiredTradePanels = (TradeAPlayerActive * 10) + TradeBPlayerActive;

        TradeDisplayInEditModeNotView = TRUE; // Can edit again.
        DISPLAY_state.desired2DView = DISPLAY_SCREEN_TradeA;
        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Trade;
//////////////////UDIBAR_setIBarRulesState(IBAR_STATE_Nothing, TradeBPlayerActive);
showpropose = FALSE;
//UDTrade_ProcessEverything();
//DISPLAY_state.IBarStateTrackOn = FALSE;
//UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayer );
////        LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_iytprpf), 202, 0, 0);//-106, 10);
////        LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_iytcxlf), 201, 0, 0);//-106, 10);
} else {
      TradeFromPlayer = -1; // Forget who started the trade
      UDTrade_ClearTrade();
      UDTrade_ProcessEverything();
      TradeAPlayerActive = TradeBPlayerActive = RULE_MAX_PLAYERS;
      DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Main;
      UDBOARD_SetBackdrop( DISPLAY_SCREEN_MainA );
      DISPLAY_showAll;
}
      }
//SetCurrentUIPlayerFromPlayerNumber ((RULE_PlayerNumber)UICurrentGameState.CurrentPlayer);
////SetCurrentUIPlayerFromPlayerNumber (0);
DISPLAY_state.IBarLastRulePlayer = NewMessage->numberB;
////////////////DISPLAY_state.IBarCurrentPlayer = UICurrentGameState.CurrentPlayer;
UDIBAR_setIBarRulesState(IBAR_STATE_Nothing, UICurrentGameState.CurrentPlayer);
      proposed = FALSE;


      if (UICurrentGameState.CurrentPlayer == DISPLAY_state.IBarLastRulePlayer)
        DISPLAY_state.IBarStateTrackOn = TRUE;
      else
      {
        DISPLAY_state.IBarStateTrackOn = FALSE;
        if (UICurrentGameState.CurrentPlayer == RULE_MAX_PLAYERS)
          UDIBAR_setIBarCurrentState(IBAR_STATE_OtherPlayer);
        else if (UICurrentGameState.CurrentPlayer < RULE_MAX_PLAYERS && SlotIsALocalHumanPlayer[UICurrentGameState.CurrentPlayer])
          UDIBAR_setIBarCurrentState(IBAR_STATE_OtherPlayer);
        else
          UDIBAR_setIBarCurrentState(IBAR_STATE_OtherPlayerRemote);
      }
      // Play it safe for now.  Don't set our display current player to be the bank
      // until we've take the time to study that code.
      //if ( PlayerOrBankHit != RULE_MAX_PLAYERS )
      DISPLAY_state.IBarCurrentPlayer = UICurrentGameState.CurrentPlayer;
      break;

    case NOTIFY_TRADE_EDITOR:
      if ((TradeAPlayerActive != RULE_MAX_PLAYERS) && (TradeAPlayerActive != NewMessage->numberA)) {
        // Countering
        for (i = 0; i < TradeItemListTop; i++)
          UDTradeItemList[i].fromPlayer = (RULE_PlayerNumber)NewMessage->numberA;
        showpropose = FALSE;
        proposed = TRUE;
      }

      TradeAPlayerActive = (RULE_PlayerNumber)NewMessage->numberA;
      TradeFromPlayer = TradeAPlayerActive; // variable may be redundant - from player initiated trade, at least in edit (hotseat)
//      TradeBPlayerActive = RULE_MAX_PLAYERS;// The playerset involved is not available yet.


      // NOTE: The setting of currentUI player and TradeFromPlayer is TEMPORARY - numberA may not be involved in trade, they just proposed it.
      // Also, the from player, particulary activeplayerA, will go through each local player in turn for multi-player trades.
      SetCurrentUIPlayerFromPlayerNumber ((RULE_PlayerNumber) NewMessage->numberA);
///////////DISPLAY_state.IBarLastRulePlayer = DISPLAY_state.IBarCurrentPlayer;
DISPLAY_state.IBarLastRulePlayer = UICurrentGameState.CurrentPlayer;
DISPLAY_state.IBarCurrentPlayer = NewMessage->numberA;

//DISPLAY_state.IBarStateTrackOn = FALSE;
//UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayer );

      TradeFromPlayer = (RULE_PlayerNumber) NewMessage->numberA;
      //if (CurrentUIPlayer < RULE_MAX_PLAYERS)//&& !UICurrentGameState.TradeDataSent)
      if (SlotIsALocalHumanPlayer[(RULE_PlayerNumber) NewMessage->numberA])//&& !UICurrentGameState.TradeDataSent)
      { // If the proposer is local, send the trade.
        extern RULE_PlayerSet TradeDestinationPlayer;// Stores the players to recieve(view) the trade, or 0 if it is a regular proposal.

        Trade_SendItems();
        if (TradeDestinationPlayer) // WARNING: This is actually the player number, not a set, as of May 99.
        { // Just passing the trade along
          MESS_SendAction (ACTION_TRADE_EDITING_DONE, TradeFromPlayer, RULE_BANK_PLAYER,
                       1, TradeDestinationPlayer, 0, 0, NULL, 0, NULL);
        } else { // Offer the trade.
          MESS_SendAction (ACTION_TRADE_EDITING_DONE, TradeFromPlayer, RULE_BANK_PLAYER,
            0, (int)TRUE, 0, 0, NULL, 0, NULL); // TRUE means private, public wont be supported.
        }
        CurrentTradingState = TRADING_GETTING_PROPERTY;
      }

///.///      UDTrade_ClearTrade(); // The trade will be (re)constructed

      if (CurrentUIPlayer < RULE_MAX_PLAYERS) // If this is local player, set the trading state accordingly.
        CurrentTradingState = TRADING_GETTING_PROPERTY; // FIXME - betcha this variable is of no use here, from hotseat.
      else
        CurrentTradingState = TRADING_SPECTATOR;
      break;

    case NOTIFY_TRADE_ACCEPTANCE_DECISION:
      if (PlayerSelectDlg.isVisible)   UDTrade_RemovePlayerSelectDlg();
      if (CashTradeDlg.isVisible)      UDTrade_RemoveCashTradeDlg();
      if ((FutureTradeDlg.isVisible) && (FutureTradeDlg.mode != 6))      UDTrade_RemoveFutureTradeDlg();
      if ((ImmunityTradeDlg.isVisible) && (ImmunityTradeDlg.mode != 6))  UDTrade_RemoveImmunityTradeDlg();
      if (showpropose) {
        LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_iytprpf), 202);
        LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_iytprpo), 202, 0, 0);//-106, 10);
        LE_SEQNCR_SetEndingAction(LED_IFT(DAT_LANG2, CNK_iytprpo), 202, LE_SEQNCR_EndingActionStop);

        LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_iytcxlf), 201);
        LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_iytcxlo), 201, 0, 0);//-106, 10);
        LE_SEQNCR_SetEndingAction(LED_IFT(DAT_LANG2, CNK_iytcxlo), 201, LE_SEQNCR_EndingActionStop);
        showpropose = FALSE;
      }

//      SetCurrentUIPlayerFromPlayerSet ((RULE_PlayerSet) NewMessage->numberA);
      SetCurrentUIPlayerFromPlayerSet(1 << TradeBPlayerActive);
      if (CurrentUIPlayer < RULE_MAX_PLAYERS)
      { // A local player is accepting/rejecting.
#if USA_VERSION
//        if (NewMessage->numberA != (1 << TradeAPlayerActive)) {  // only play once (two NTAD msgs)
        temp = GetTickCount();
        if ((temp - AcceptSndTick) > 5000) {
          AcceptSndTick = temp;
          if (UICurrentGameState.Players[TradeAPlayerActive].AIPlayerLevel == 0) {
            if ((!SlotIsALocalHumanPlayer[TradeAPlayerActive]) && (SlotIsALocalHumanPlayer[TradeBPlayerActive]))
              UDPENNY_PennybagsGeneral(PB_HumanInitiatesTrade, CLIPACTION);
          } else
            UDPENNY_PennybagsGeneral(PB_AIInitiatesTrade, CLIPACTION);
        }
//        }
#endif
        UDIBAR_setIBarRulesState(IBAR_STATE_Trading, CurrentUIPlayer);//WARNING: its a playerset not a player in numberA
//DISPLAY_state.IBarLastRulePlayer = DISPLAY_state.IBarCurrentPlayer;
//DISPLAY_state.IBarLastRuleState = DISPLAY_state.IBarCurrentState;

DISPLAY_state.IBarCurrentPlayer = TradeBPlayerActive;
DISPLAY_state.IBarCurrentState = IBAR_STATE_Trading;

//DISPLAY_state.IBarStateTrackOn = FALSE;
//UDIBAR_setIBarCurrentState( IBAR_STATE_OtherPlayer );

      } else { // Special handling - pick the first (is non-local to be here) in the playerset to 'watch'
        //UDIBAR_setIBarRulesState(IBAR_STATE_Nothing, TradeBPlayerActive);//WARNING: its a playerset not a player in numberA
#if USA_VERSION
//        if (NewMessage->numberA != (1 << TradeAPlayerActive)) {  // only play once (two NTAD msgs)
        temp = GetTickCount();
        if ((temp - AcceptSndTick) > 5000) {
          AcceptSndTick = temp;
          if ((UICurrentGameState.Players[TradeAPlayerActive].AIPlayerLevel != 0) &&
              (UICurrentGameState.Players[TradeBPlayerActive].AIPlayerLevel != 0))
            UDPENNY_PennybagsGeneral(PB_AIInitiatesTrade, CLIPACTION);
        }
//        }
#endif
/*
arrgh.
        sorry dude, it was the right thing to do!(RK)*/
        int iTemp = RULE_NOBODY_PLAYER;
        for (iTemp = 0; iTemp < UICurrentGameState.NumberOfPlayers; iTemp++)
          if (NewMessage->numberA & (1 << iTemp))
          {// No local players involved, gimme whatevers first to watch.
            UDIBAR_setIBarRulesState(IBAR_STATE_Trading, iTemp);
            //UDIBAR_setIBarRulesState(IBAR_STATE_Nothing, iTemp);
            break;
          }
      }
      DISPLAY_showAll;
      break;

    case NOTIFY_TRADE_ITEM:
      // Update the Display state for tokens involved in the trade TEMP - not going to work like this...
      Trade_AddItem(NewMessage); // the list must be maintained for display purposes, below registers it for trade if accepted.
      if      (NewMessage->numberA == TradeAPlayerActive)  TradeBPlayerActive = NewMessage->numberB;
      else if (NewMessage->numberB == TradeAPlayerActive)  TradeBPlayerActive = NewMessage->numberA;
      PlayerSelectDlg.playerToTrade = TradeBPlayerActive;
      desiredTradePanels = (TradeAPlayerActive * 10) + TradeBPlayerActive;
//UDTrade_ProcessEverything();
      UDIBAR_setIBarRulesState(IBAR_STATE_Nothing, TradeBPlayerActive);

      // Update the displayed state pointer to reflect the trade request.
      switch (NewMessage->numberC)
      {
        case TIK_CASH:
          // Set the receiver.
          UICurrentGameState.Players[NewMessage->numberA].shared.trading.cashGivenInTrade[NewMessage->numberB] =
            NewMessage->numberD;
          if (NewMessage->numberA == TradeAPlayerActive)
            DISPLAY_state.TradeCashBoxDesiredValue[2] = NewMessage->numberD;
          if (NewMessage->numberA == TradeBPlayerActive)
            DISPLAY_state.TradeCashBoxDesiredValue[3] = NewMessage->numberD;
          break;

        case TIK_SQUARE:
          // Set the new receiver for the square. Can be RULE_NOBODY_PLAYER;
          UICurrentGameState.Squares[NewMessage->numberD].offeredInTradeTo = (RULE_PlayerNumber)NewMessage->numberB;
          break;

        case TIK_JAIL_CARD:
          // Set the receiver.
          UICurrentGameState.Cards[NewMessage->numberD].jailOfferedInTradeTo = (RULE_PlayerNumber)NewMessage->numberB;
          break;

        case TIK_IMMUNITY:
        case TIK_FUTURE_RENT:
          AddUiImmunity ((RULE_PlayerNumber) NewMessage->numberA,
            (RULE_PlayerNumber) NewMessage->numberB,
            (NewMessage->numberC == TIK_FUTURE_RENT) ? CHT_FUTURE_RENT : CHT_RENT_IMMUNITY,
            NewMessage->numberD, NewMessage->numberE, TRUE);
          break;

        default:
          break;
      }
      // Show the trading list in the dialog box.
      //BuildTradingList(); // Old.
      //DISPLAY_showAll;
      UDTrade_ProcessEverything();
      break;
  }
}

/*****************************************************************************
* UDTRADE_ClearTrade
* Setup Trade - clear the trade state, usually by request or because a new trade is coming in.
*****************************************************************************/
void UDTrade_ClearTrade(void)
{
  TradeItemListTop = 0;
    TradeFromPlayer = -1; // This would flag the trade absolutely clear (nobody started it), likely causing problems.
    TradeAPlayerActive = RULE_MAX_PLAYERS;
    TradeBPlayerActive = RULE_MAX_PLAYERS;
    desiredTradePanels = -1;//(RULE_MAX_PLAYERS * 10) + RULE_MAX_PLAYERS;
  DISPLAY_state.TradeCashBoxDesiredValue[0] = 0; // Player A cash
  DISPLAY_state.TradeCashBoxDesiredValue[1] = 0; // Player B cash
  DISPLAY_state.TradeCashBoxDesiredValue[2] = 0; // Player A cash offered
  DISPLAY_state.TradeCashBoxDesiredValue[3] = 0; // Player B cash offered

//  for (iTemp = 0; iTemp < RULE_MAX_COUNT_HIT_SETS; iTemp++)
//  {
//    DISPLAY_state.TradeImmunityFutureDesiredA[iTemp] = LED_EI;
//    DISPLAY_state.TradeImmunityFutureDesiredB[iTemp] = LED_EI;
//  }
  curside = curproperty = curamount = 0;
  UDTrade_ProcessEverything();
}

/*****************************************************************************
* Function:   void UDTrade_InitPropertyStruct(void)
* Returns:    nothing
* Parameters: nothing
* Purpose:    initialize the structures used in the trade screen
*****************************************************************************/
void UDTrade_InitPropertyStruct(void)
{
  UDTrade_ResetPropertyPositions(&DesiredBeforePropertyA);
  UDTrade_ResetPropertyPositions(&DesiredBeforePropertyB);
  UDTrade_ResetPropertyPositions(&TradeBeforePropertyA);
  UDTrade_ResetPropertyPositions(&TradeBeforePropertyB);
  UDTrade_ResetPropertyPositions(&DesiredAfterPropertyA);
  UDTrade_ResetPropertyPositions(&DesiredAfterPropertyB);

  DesiredBeforePropertyA.trade_box = BEFORE_A;
  DesiredBeforePropertyB.trade_box = BEFORE_B;
  TradeBeforePropertyA.trade_box = TRADE_A;
  TradeBeforePropertyB.trade_box = TRADE_B;
  DesiredAfterPropertyA.trade_box = AFTER_A;
  DesiredAfterPropertyB.trade_box = AFTER_B;
}

/*****************************************************************************
* Function:   void UDTrade_PositionPropertyOwned(PropertyLocationStruct *pps,
*                                                int order, int index)
* Returns:    nothing
* Parameters: pointer to structure to fill
*             integer representing the order of the property
*             index into array of boardpieces
* Purpose:    Sets the position of the property in the trade screen
*****************************************************************************/
void UDTrade_PositionPropertyOwned(PropertyPositionStruct* pps, int order, int index)
{
  int x, y, dim_two = order % 3, dim_one = order / 3;
  short width_apart = 0;

  if (pps->trade_box <= BEFORE_B)
  {
    if (pps->numColumns)
      width_apart = (TRADE_PROPERTY_BasicW-(TRADE_PROPERTY_DeltaX*2)-TRADE_PROPERTY_CardW)/6;
    if (width_apart > (TRADE_PROPERTY_CardW * 2))
      width_apart = TRADE_PROPERTY_CardW * 2;
  } else {
    if (pps->numColumns)
      width_apart = (TRADE_PROPERTY_ItemW-(((TRADE_PROPERTY_BasicW - TRADE_PROPERTY_ButtonW) / 12)/2)-TRADE_PROPERTY_CardW) / pps->numColumns;
  }

  y = TRADE_PROPERTY_TopY[pps->trade_box] + (TRADE_PROPERTY_DeltaY*dim_two);
  if (pps->trade_box <= BEFORE_B) {
    x = 11 + TRADE_PROPERTY_TopX[pps->trade_box] + (dim_one * width_apart) + (TRADE_PROPERTY_DeltaX*dim_two);
    if (order > 14) {
      x = 11 + TRADE_PROPERTY_TopX[pps->trade_box] + ((dim_one - 5) * width_apart) + (TRADE_PROPERTY_DeltaX*dim_two);
      y += 65;
    }
  } else
    x = 11 + TRADE_PROPERTY_TopX[pps->trade_box] + (pps->column[dim_one][dim_two] * width_apart) + (TRADE_PROPERTY_DeltaX*dim_two);

  // set the locations for the owned properties
  pps->Location[index].left   = x;
  pps->Location[index].top    = y;
  pps->Location[index].right  = x + TRADE_PROPERTY_CardW;
  pps->Location[index].bottom = y + TRADE_PROPERTY_CardH;

  int tmptmp = (UNS16)order;
  if ((tmptmp % 3) == 0) tmptmp += 2;
  else if (((tmptmp - 2) % 3) == 0) tmptmp -= 2;
  pps->Priority[index] = 32 - tmptmp;
}

/*****************************************************************************
* Function:   void UDTrade_RepositionProperties(PropertyPositionStruct* pps, int order,
*                                               int index, int box)
* Returns:    nothing
* Parameters: pointer to structure to modify
*             integer representing the order of the property
*             index into array of boardpieces
*             integer representing which trading box is being dealt with
* Purpose:    Modifies all info relating to the Properties for the
*             trade box specified and redisplays any changes made
*****************************************************************************/
void UDTrade_RepositionProperties(PropertyPositionStruct* pps, int order, int index, int box)
{
  int t;
  // calculate the priority which is dependant on the trade box
  // calculation is based on previously set defines in Display.h
  int temp_priority = DISPLAY_TradeBoxAStuffPriority + (box * 50);

LE_SEQNCR_ProcessUserCommands();
  for (int i = SQ_MAX_SQUARE_TYPES - 1; i >= 0; i--) //reverse
  {
    // if the pieces object id is set, then there is a change
    if ((pps->ID[i] != LED_EI) || (pps->movingflag[i]))
    {
//      // if the object is currently being displayed, stop it
//      if (!LE_SEQNCR_IsSequenceFinished (pps->ID[i], temp_priority + pps->Priority[i], FALSE))
//        LE_SEQNCR_Stop(pps->ID[i], temp_priority + pps->Priority[i]);

      t = DISPLAY_propertyToOwnablePropertyConversion(i);
      order = IBARPropertyBarOrder[t]; // Obtain display order in tray.

      // set the position of the property relevent to it's trade box
      UDTrade_PositionPropertyOwned(pps, order, i);

//      LE_SEQNCR_StartXY(pps->ID[i], temp_priority + pps->Priority[i],
//        TradeBasicPlayerBoxes[box][0] + pps->Location[i].left,
//        TradeBasicPlayerBoxes[box][1] + pps->Location[i].top);

      // if the object is currently being displayed, move it
      if (!pps->movingflag[i]) {
        if (!LE_SEQNCR_IsSequenceFinished (pps->ID[i], temp_priority + pps->Priority[i], FALSE))
          LE_SEQNCR_MoveXY(pps->ID[i], temp_priority + pps->Priority[i],
            TradeBasicPlayerBoxes[box][0] + pps->Location[i].left,
            TradeBasicPlayerBoxes[box][1] + pps->Location[i].top);
        else
          LE_SEQNCR_StartXY(pps->ID[i], temp_priority + pps->Priority[i],
            TradeBasicPlayerBoxes[box][0] + pps->Location[i].left,
            TradeBasicPlayerBoxes[box][1] + pps->Location[i].top);
      }
      pps->movingflag[i] = FALSE;
    }
  }
  // reset trade box state to unchanged
  pps->state = UNCHANGED;
}

/*****************************************************************************
* Function:   void UDTrade_ResetPropertyPositions(PropertyPositionStruct* pps)
* Returns:    nothing
* Parameters: pointer to structure to be reset to zero
* Purpose:    Resets all info relating to the Properties for the trade screen
*****************************************************************************/
void UDTrade_ResetPropertyPositions(PropertyPositionStruct* pps)
{
  register int i, j;

  for (i = 0; i < 11; i++)
  {
    for (j = 0; j < 3; j++)
      pps->column[i][j] = -1; // reset the columns
    pps->isColumnCounted[i] = FALSE; // reset the columns flag
  }

  if (DISPLAY_state.desired2DView != DISPLAY_SCREEN_TradeA)
    for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
      pps->ID[i] = LED_EI;
//      SetRect (&(pps->Location[i]), 0, 0, 0, 0);

  // reset the number of columns
  pps->numColumns = -1;
}

/*****************************************************************************
* Function:   void UDTrade_CountColumns(RULE_PropertySet DesiredStuff,
*                                       PropertyPositionStruct *pps)
* Returns:    nothing
* Parameters: set of bits that determine which property is owned
*             array of property position structures to pass to the Sort
*             function being called
* Purpose:    Determines the number of columns of properties
*             for a player and sorts the properties in the proper order
*             to maintain a uniform display
*****************************************************************************/
void UDTrade_CountColumns(RULE_PropertySet DesiredStuff, RULE_PropertySet DesiredStuff1, PropertyPositionStruct* pps)
{
  register short i, index, column_counter = 0;
  RULE_PropertySet lTemp;

  for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
  {
    if ((lTemp = RULE_PropertyToBitSet(i)) == 0)  continue;

    // calculate the column the property should be in if player owned
    // all properties
    index = IBARPropertyBarOrder
      [DISPLAY_propertyToOwnablePropertyConversion(i)] / 3; // Obtain display order in tray.

    // Special: Something's moving in.  Make sure there's room.
    if (pps->movingflag[i])
      if (!pps->isColumnCounted[index])
      {
        column_counter++;
        pps->isColumnCounted[index] = TRUE;
      }

    // if this property is part of the trade box we are dealing with
    if ((DesiredStuff & lTemp) || (DesiredStuff1 & lTemp)) //noonoo
    {
      // if the columns hasn't already been counted
      if (!pps->isColumnCounted[index])
      {
        column_counter++;
        pps->isColumnCounted[index] = TRUE;
      }
    }
  }

  // sort properties so they maintain a similar position as the other
  // interfaces
  UDTrade_SortPropertyPositions(pps);

  // set number of columns for that trade box
  pps->numColumns = column_counter;
}

/*****************************************************************************
* Function:   void UDTrade_SortPropertyPositions(PropertyPositionStruct* pps)
* Returns:    nothing
* Parameters: array of property positions so we can set the column member
* Purpose:    Knowing which properties are owned, we can determine the
*             order in which they should appear on screen.
*****************************************************************************/
void UDTrade_SortPropertyPositions(PropertyPositionStruct* pps)
{
  register short i, temp_counter = 0;

  for (i = 0; i < 11; i++)
    if (pps->isColumnCounted[i])
    {
      pps->column[i][0] = pps->column[i][1] = pps->column[i][2] = temp_counter;
      temp_counter++;
    }
}

/*****************************************************************************
* Function:   void UDTrade_InitPlayerSelect(void)
* Returns:    nothing
* Parameters: nothing
* Purpose:    Prepares a dialog box to let user choose which player he or she
*             would like to trade with.
*****************************************************************************/
void UDTrade_InitPlayerSelect(void)
{
  // set dialog flag so that the only valid messages are the ones relating to the dialog box
  PlayerSelectDlg.isVisible = FALSE;
  // set the player's choice to an out of range value
  PlayerSelectDlg.playerToTrade = -1;
  // set the dialog box position
  PlayerSelectDlg.x = 306;  PlayerSelectDlg.y = 120+114;
  // set the dialog box dimensions
  PlayerSelectDlg.w = 188;  PlayerSelectDlg.h = 209;
  //create the dialog box
  PlayerSelectDlg.ID = LE_GRAFIX_ObjectCreate(PlayerSelectDlg.w, PlayerSelectDlg.h, LE_GRAFIX_ObjectNotTransparent);
  PlayerSelectDlg.priority = 205;
}

/*****************************************************************************
* Function:   void UDTrade_DisplayPlayerSelectDlg(void)
* Returns:    nothing
* Parameters: nothing
* Purpose:    Displays the player selection dialog box.
*****************************************************************************/
void UDTrade_DisplayPlayerSelectDlg(void)
{
  int i, tempy;

  proposed = FALSE;

  // If we're asking who to trade with, the trade should be clear.
  InitializeTradeSequence();
  UDTrade_ClearTrade();
  NumProperties = curproperty = 0;
  for (i = 0; i < TradeCashBoxes; i++) {
    DISPLAY_state.TradeCashBoxValues[i] = CASHTRADEINIT;
    DISPLAY_state.TradeCashBoxDesiredValue[i] = 0;
  }

/*
  TradeAPlayerActive = DISPLAY_state.IBarCurrentPlayer;
  if (TradeAPlayerActive >= RULE_MAX_PLAYERS)
    TradeAPlayerActive = UICurrentGameState.NumberOfPlayers - 1;
  int j = TradeAPlayerActive;
  while ((!SlotIsALocalHumanPlayer[TradeAPlayerActive]) ||
         (UICurrentGameState.Players[TradeAPlayerActive].currentSquare >= SQ_OFF_BOARD))
  {
    TradeAPlayerActive--;
    if (TradeAPlayerActive < 0)   TradeAPlayerActive = UICurrentGameState.NumberOfPlayers - 1;
    if (TradeAPlayerActive == j) {
      TradeFromPlayer = -1; // Forget who started the trade
      UDTrade_ClearTrade();
      TradeAPlayerActive = TradeBPlayerActive = RULE_MAX_PLAYERS;
      DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Main;
      UDBOARD_SetBackdrop( DISPLAY_SCREEN_MainA );
      DISPLAY_showAll;
      return;
    }
  }
*/
  TradeAPlayerActive = UDTrade_GetPlayerToTradeFrom();
  if (TradeAPlayerActive == RULE_MAX_PLAYERS) {
    TradeFromPlayer = -1; // Forget who started the trade
    UDTrade_ClearTrade();
    TradeAPlayerActive = TradeBPlayerActive = RULE_MAX_PLAYERS;
    DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Main;
    UDBOARD_SetBackdrop( DISPLAY_SCREEN_MainA );
    DISPLAY_showAll;
    return;
  }
  TradeFromPlayer = TradeAPlayerActive;
  TradeBPlayerActive = RULE_MAX_PLAYERS;

/*
  LE_GRAFIX_ColorArea(NameA, 0, 0, 135, 35, LEG_MCR(0,255,0));
  LE_GRAFIX_ColorArea(NameB, 0, 0, 135, 35, LEG_MCR(0,255,0));
  LE_FONTS_SetSettings(0);
  LE_FONTS_SetSize(12);
  LE_FONTS_SetWeight(700);
  LE_FONTS_Print(NameA, 8, 9, LEG_MCR(220, 220, 220), UICurrentGameState.Players[TradeAPlayerActive].name);
  LE_FONTS_GetSettings(0);
*/

  // if there are only 2 players, there is no point to have a dialog box
  if (UICurrentGameState.NumberOfPlayers < 3) {
    TradeBPlayerActive = 1 - TradeAPlayerActive;
    desiredTradePanels = (TradeAPlayerActive * 10) + TradeBPlayerActive;
    DISPLAY_state.TradeCashBoxValues[1] = CASHTRADEINIT;
    DISPLAY_state.TradeCashBoxValues[3] = CASHTRADEINIT;
    DISPLAY_state.TradeCashBoxDesiredValue[1] = 0;
    DISPLAY_state.TradeCashBoxDesiredValue[3] = 0;
    UDPENNY_PennybagsGeneral(PB_TradeScreen, CLIPACTION);
    LE_FONTS_SetSettings(0);
    LE_FONTS_SetSize(12);
    LE_FONTS_SetWeight(700);
    LE_FONTS_Print(NameB, 8, 9, LEG_MCR(220, 220, 220), UICurrentGameState.Players[TradeBPlayerActive].name);
    LE_FONTS_GetSettings(0);
    UDTrade_ProcessEverything();
    return;
  }

  desiredTradePanels = (TradeAPlayerActive * 10) + TradeBPlayerActive;
  UDTrade_ProcessEverything();

  // set dialog flag so that the only valid messages are the ones relating to the dialog box
  PlayerSelectDlg.isVisible = TRUE;

  LE_FONTS_SetSettings(0);
  LE_FONTS_SetSize(8);
  LE_FONTS_SetWeight(400);

  // set the message
  // first find out how many players are currently playing
  for (i = 0; i < UICurrentGameState.NumberOfPlayers; i++)
  {
    PlayerSelectDlg.messageStr[i][0] = '\0';
    if (i == TradeAPlayerActive)  continue;
    if (UICurrentGameState.Players[i].currentSquare >= SQ_OFF_BOARD)  continue;
    // set the string to be "n: PlayerName"
    Asprintf(PlayerSelectDlg.messageStr[i], A_T("%*i: %s"), 6, (i+1), UICurrentGameState.Players[i].name);
  }

  LE_GRAFIX_ShowTCB(PlayerSelectDlg.ID, 0, 0, LED_IFT(DAT_PAT, TAB_tnfi0));
  // print intial line which should always be the same
  LE_FONTS_Print(PlayerSelectDlg.ID, (188 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(TMN_TRADE_SELECT))) / 2,
    12, LEG_MCR(220, 220, 220), LANG_GetTextMessage(TMN_TRADE_SELECT));

  // print the names of the players
  tempy = 1;
  for (i = 0; i < UICurrentGameState.NumberOfPlayers; i++)
  {
    if (i == TradeAPlayerActive)  continue;
    if (UICurrentGameState.Players[i].currentSquare >= SQ_OFF_BOARD)  continue;
    LE_FONTS_Print(PlayerSelectDlg.ID, 5, 4+tempy*32, LEG_MCR(220, 220, 220), PlayerSelectDlg.messageStr[i]);
    tempy++;
  }

  // display the tokens representing each player
  tempy = 1;
  for (i = 0; i < UICurrentGameState.NumberOfPlayers; i++)
  {
    if (i == TradeAPlayerActive)  continue;
    if (UICurrentGameState.Players[i].currentSquare >= SQ_OFF_BOARD)  continue;

    PlayerSelectDlg.tokenID = LED_IFT(DAT_MAIN, TAB_inpsa+UICurrentGameState.Players[i].token);
    // set token location for mouse clicks
    PlayerSelectDlg.tokenLocation[i].left = 126;
//PlayerSelectDlg.tokenLocation[i].left += (40 - LE_GRAFIX_AnyBitmapWidth(PlayerSelectDlg.tokenID)) / 2;
    PlayerSelectDlg.tokenLocation[i].top = -5+tempy*32;
    PlayerSelectDlg.tokenLocation[i].right = PlayerSelectDlg.tokenLocation[i].left + 53;
    PlayerSelectDlg.tokenLocation[i].bottom = PlayerSelectDlg.tokenLocation[i].top + 29;

    LE_GRAFIX_ShowTCB_Alpha(PlayerSelectDlg.ID, (short)PlayerSelectDlg.tokenLocation[i].left + 
        (44 - LE_GRAFIX_AnyBitmapWidth(PlayerSelectDlg.tokenID)) / 2,
      (short)PlayerSelectDlg.tokenLocation[i].top +
        (28 - LE_GRAFIX_AnyBitmapHeight(PlayerSelectDlg.tokenID)) / 2, PlayerSelectDlg.tokenID);
    tempy++;
  }
  LE_SEQNCR_StartXY(PlayerSelectDlg.ID, PlayerSelectDlg.priority,
                    PlayerSelectDlg.x, PlayerSelectDlg.y);
  UDPENNY_PennybagsGeneral(PB_TradeScreen_PickTradePartner, CLIPACTION);
  LE_FONTS_GetSettings(0);
}

/*****************************************************************************
* Function:   void UDTrade_RemovePlayerSelectDlg(void)
* Returns:    nothing
* Parameters: nothing
* Purpose:    Removes the player selection dialog box.
*****************************************************************************/
void UDTrade_RemovePlayerSelectDlg(void)
{
  // remove the dialog box
  PlayerSelectDlg.isVisible = FALSE;
  if (!LE_SEQNCR_IsSequenceFinished (PlayerSelectDlg.ID, PlayerSelectDlg.priority, FALSE))
    LE_SEQNCR_Stop(PlayerSelectDlg.ID, PlayerSelectDlg.priority);

  // reset the token rects
  for (int i = 0; i < UICurrentGameState.NumberOfPlayers; i++)
    SetRect(&(PlayerSelectDlg.tokenLocation[i]),0,0,0,0);
}

/*****************************************************************************
* Function:   void UDTrade_ProcessPlayerSelection(int choice)
* Returns:    returns true or false
* Parameters: holds the player number to trade with
* Purpose:    Selects the player to trade with
*****************************************************************************/
void UDTrade_ProcessPlayerSelection(int choice)
{
  // at this point, we know message is a keyboard message and the dialog box is visible
  switch(choice)
  {
    case 1:  PlayerSelectDlg.playerToTrade = 0;  break;
    case 2:  PlayerSelectDlg.playerToTrade = 1;  break;
    case 3:  PlayerSelectDlg.playerToTrade = 2;  break;
    case 4:  PlayerSelectDlg.playerToTrade = 3;  break;
    case 5:  PlayerSelectDlg.playerToTrade = 4;  break;
    case 6:  PlayerSelectDlg.playerToTrade = 5;  break;
    default: PlayerSelectDlg.playerToTrade = -1; return;
  }

  // verify that user didn't choose him/herself
  if (TradeAPlayerActive == PlayerSelectDlg.playerToTrade) {
    PlayerSelectDlg.playerToTrade = -1;
    return;
  }
  // verify that the user didn't choose a player that doesn't exist
  if (PlayerSelectDlg.playerToTrade >= UICurrentGameState.NumberOfPlayers) {
    PlayerSelectDlg.playerToTrade = -1;
    return;
  }
  // No bankrupt players
  if (UICurrentGameState.Players[PlayerSelectDlg.playerToTrade].currentSquare >= SQ_OFF_BOARD) {
    PlayerSelectDlg.playerToTrade = -1;
    return;
  }

////////////////// Trade with self bug
//  TradeAPlayerActive = DISPLAY_state.IBarCurrentPlayer;
//  int j = TradeAPlayerActive;
//  while (!SlotIsALocalHumanPlayer[TradeAPlayerActive])
//  {
//    TradeAPlayerActive--;
//    if (TradeAPlayerActive < 0)   TradeAPlayerActive = UICurrentGameState.NumberOfPlayers - 1;
//    //this shouldn't happen...
//    if (TradeAPlayerActive == j)  { TradeAPlayerActive = RULE_SPECTATOR_PLAYER; break; }
//  }
//////////////////////////////////////
  TradeFromPlayer = TradeAPlayerActive;
  TradeBPlayerActive = PlayerSelectDlg.playerToTrade;
  LE_FONTS_SetSettings(0);
  LE_FONTS_SetSize(12);
  LE_FONTS_SetWeight(700);
  LE_FONTS_Print(NameB, 8, 9, LEG_MCR(220, 220, 220), UICurrentGameState.Players[TradeBPlayerActive].name);
  LE_FONTS_GetSettings(0);

  DISPLAY_state.TradeJailCardsDesired[0] = DISPLAY_state.TradeJailCardsDesired[1] = 0;
  if (UICurrentGameState.Cards[CHANCE_DECK].jailOwner == TradeAPlayerActive)
    DISPLAY_state.TradeJailCardsDesired[0] = (1 << 0);
  else if (UICurrentGameState.Cards[CHANCE_DECK].jailOwner == TradeBPlayerActive)
    DISPLAY_state.TradeJailCardsDesired[0] = (1 << 1);

  if (UICurrentGameState.Cards[COMMUNITY_DECK].jailOwner == TradeAPlayerActive)
    DISPLAY_state.TradeJailCardsDesired[1] = (1 << 0);
  else if (UICurrentGameState.Cards[COMMUNITY_DECK].jailOwner == TradeBPlayerActive)
    DISPLAY_state.TradeJailCardsDesired[1] = (1 << 1);

  DISPLAY_state.TradeImmunityFutureShownA[0] = DISPLAY_state.TradeImmunityFutureShownA[1] = 0;
  DISPLAY_state.TradeImmunityFutureDesiredA[0] = DISPLAY_state.TradeImmunityFutureDesiredA[1] = 0;

  desiredTradePanels = (TradeAPlayerActive * 10) + TradeBPlayerActive;
  DISPLAY_state.TradeCashBoxValues[1] = CASHTRADEINIT;
  DISPLAY_state.TradeCashBoxValues[3] = CASHTRADEINIT;
  DISPLAY_state.TradeCashBoxDesiredValue[1] = 0;
  DISPLAY_state.TradeCashBoxDesiredValue[3] = 0;
  UDTrade_ProcessEverything();
  UDTrade_RemovePlayerSelectDlg();
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//
/*****************************************************************************
* Function:   void UDTrade_InitCashTrade(void)
* Returns:    Nothing
* Parameters: Nothing
* Purpose:    Prepares a dialog box to let user pick how much cash to trade.
*****************************************************************************/
void UDTrade_InitCashTrade(void)
{
  // set dialog flag so that the only valid messages are the ones relating to the dialog box
  CashTradeDlg.isVisible = FALSE;
  // set the player's choice to an out of range value
  CashTradeDlg.playerToTrade = -1;
  // set the dialog box position
  CashTradeDlg.x = 4;  CashTradeDlg.y = 273+SD;
  // set the dialog box dimensions
  CashTradeDlg.w = 188;  CashTradeDlg.h = 209;
  CashTradeDlg.priority = DISPLAY_TradeBoxBItemsPriority + 1500;
}

/*****************************************************************************
* Function:   void UDTrade_DisplayCashTradeDlg(void)
* Returns:    Nothing
* Parameters: Nothing
* Purpose:    Displays the cash trade dialog box.
*****************************************************************************/
void UDTrade_DisplayCashTradeDlg(int side)
{
  // set dialog flag so that the only valid messages are the ones relating to the dialog box
  origTradeAmt = DISPLAY_state.TradeCashBoxDesiredValue[2];
  origTradeAmt2 = DISPLAY_state.TradeCashBoxDesiredValue[3];
  curside = side;
  CashTradeDlg.isVisible = TRUE;
  CashTradeDlg.x = 4 + (side * 600);

  TradeAmt = 0;
  AdjustRect1(M001SC, M001BC, CashTradeDlg.x, CashTradeDlg.y);
  AdjustRect1(M005SC, M005BC, CashTradeDlg.x, CashTradeDlg.y);
  AdjustRect1(M010SC, M010BC, CashTradeDlg.x, CashTradeDlg.y);
  AdjustRect1(M020SC, M020BC, CashTradeDlg.x, CashTradeDlg.y);
  AdjustRect1(M050SC, M050BC, CashTradeDlg.x, CashTradeDlg.y);
  AdjustRect1(M100SC, M100BC, CashTradeDlg.x, CashTradeDlg.y);
  AdjustRect1(M500SC, M500BC, CashTradeDlg.x, CashTradeDlg.y);
  AdjustRect1(OkaySC, OkayBC, CashTradeDlg.x, CashTradeDlg.y);
  AdjustRect1(ClrSC,  ClrBC,  CashTradeDlg.x, CashTradeDlg.y);
  AdjustRect1(CnclSC, CnclBC, CashTradeDlg.x, CashTradeDlg.y);

  LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, TAB_tnmtry00), CashTradeDlg.priority+1, CashTradeDlg.x, CashTradeDlg.y);
  LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tytsmnci), CashTradeDlg.priority+2, CashTradeDlg.x-310, CashTradeDlg.y-353);
  LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tytsmnoi), CashTradeDlg.priority+2, CashTradeDlg.x-310, CashTradeDlg.y-353);
  LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tytsmnxi), CashTradeDlg.priority+2, CashTradeDlg.x-310, CashTradeDlg.y-353);
}

/*****************************************************************************
* Function:   void UDTrade_RemoveCashTradeDlg(void)
* Returns:    Nothing
* Parameters: Nothing
* Purpose:    Removes the cash trade dialog box.
*****************************************************************************/
void UDTrade_RemoveCashTradeDlg(void)
{
  // remove the dialog box
  CashTradeDlg.isVisible = FALSE;
  if (!LE_SEQNCR_IsSequenceFinished(LED_IFT(DAT_LANG2, TAB_tnmtry00), CashTradeDlg.priority+1, FALSE)) {
    LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, TAB_tnmtry00), CashTradeDlg.priority+1);
    LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tytsmnci), CashTradeDlg.priority+2);
    LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tytsmnoi), CashTradeDlg.priority+2);
    LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tytsmnxi), CashTradeDlg.priority+2);
  }
}

/*****************************************************************************
* Function:   void UDTrade_ProcessCashTrade(int msg, int a, int b)
* Returns:    returns true or false
* Parameters: holds the player number to trade with
* Purpose:    Selects the player to trade with
*****************************************************************************/
void UDTrade_ProcessCashTrade(int msg, int a, int b)
{
#define tradeMoney \
  TradeArguments.fromPlayer = TradeFromPlayer; \
  TradeArguments.numberA = (plyr ? TradeBPlayerActive : TradeAPlayerActive); \
  TradeArguments.numberB = (plyr ? TradeAPlayerActive : TradeBPlayerActive); \
  TradeArguments.numberC = (int)TIK_CASH; \
  TradeArguments.numberD = TradeAmt; \
  Trade_AddItem(&TradeArguments);

  int plyr = (CashTradeDlg.x > 400);
  RULE_ActionArgumentsRecord TradeArguments = {
    ACTION_TRADE_ITEM, 0, {0, ""}, RULE_BANK_PLAYER, 0, 0, 0, 0, 0, A_T(""), NULL, 0, 0, NULL };

  if (msg == UIMSG_MOUSE_LEFT_DOWN) {
    if (PointInRect(a, b, M001SC)) {
      if ((TradeAmt + 1) <= 999999)    TradeAmt += 1;
      tradeMoney;
    } else if (PointInRect(a, b, M005SC)) {
      if ((TradeAmt + 5) <= 999999)    TradeAmt += 5;
      tradeMoney;
    } else if (PointInRect(a, b, M010SC)) {
      if ((TradeAmt + 10) <= 999999)   TradeAmt += 10;
      tradeMoney;
    } else if (PointInRect(a, b, M020SC)) {
      if ((TradeAmt + 20) <= 999999)   TradeAmt += 20;
      tradeMoney;
    } else if (PointInRect(a, b, M050SC)) {
      if ((TradeAmt + 50) <= 999999)   TradeAmt += 50;
      tradeMoney;
    } else if (PointInRect(a, b, M100SC)) {
      if ((TradeAmt + 100) <= 999999)  TradeAmt += 100;
      tradeMoney;
    } else if (PointInRect(a, b, M500SC)) {
      if ((TradeAmt + 500) <= 999999)  TradeAmt += 500;
      tradeMoney;
    } else if (PointInRect(a, b, OkaySC)) {
      UDSOUND_Click();
      LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tytsmnop), CashTradeDlg.priority+3, -310+CashTradeDlg.x, -353+CashTradeDlg.y);
      LE_SEQNCR_SetEndingAction(LED_IFT(DAT_LANG2, CNK_tytsmnop), CashTradeDlg.priority+3, LE_SEQNCR_EndingActionStop);
      LE_SEQNCR_ProcessUserCommands();
      DISPLAY_state.TradeCashBoxDesiredValue[plyr + 2] = TradeAmt;
      DISPLAY_state.TradeCashBoxDesiredValue[(1 - plyr) + 2] = 0;
      DISPLAY_state.TradeCashBoxDesiredValue[0] =
        UICurrentGameState.Players[TradeAPlayerActive].cash - DISPLAY_state.TradeCashBoxDesiredValue[2] + DISPLAY_state.TradeCashBoxDesiredValue[3];
      DISPLAY_state.TradeCashBoxDesiredValue[1] =
        UICurrentGameState.Players[TradeBPlayerActive].cash - DISPLAY_state.TradeCashBoxDesiredValue[3] + DISPLAY_state.TradeCashBoxDesiredValue[2];
      tradeMoney;
      CashTradeDying = TRUE;
    } else if (PointInRect(a, b, CnclSC)) {
      UDSOUND_Click();
      LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tytsmnxp), CashTradeDlg.priority+3, -310+CashTradeDlg.x, -353+CashTradeDlg.y);
      LE_SEQNCR_SetEndingAction(LED_IFT(DAT_LANG2, CNK_tytsmnxp), CashTradeDlg.priority+3, LE_SEQNCR_EndingActionStop);
      LE_SEQNCR_ProcessUserCommands();
      if ((origTradeAmt == 0) && (origTradeAmt2 == 0)){
        TradeAmt = 0;
      } else if (origTradeAmt == 0) {
        TradeAmt = origTradeAmt2;
        if (!plyr)  plyr = 1 - plyr;
      } else {
        TradeAmt = origTradeAmt;
        if (plyr)  plyr = 1 - plyr;
      }
      tradeMoney;
      CashTradeDying = TRUE;
    } else if (PointInRect(a, b, ClrSC)) {
      UDSOUND_Click();
      LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tytsmncp), CashTradeDlg.priority+3, -310+CashTradeDlg.x, -353+CashTradeDlg.y);
      LE_SEQNCR_SetEndingAction(LED_IFT(DAT_LANG2, CNK_tytsmncp), CashTradeDlg.priority+3, LE_SEQNCR_EndingActionStop);
      TradeAmt = 0;

      int t, s = -1;
      for (t = 0; t < TradeItemListTop; t++)
        if (UDTradeItemList[t].numberC == TIK_CASH)
          { s = t; break; }
      if (s != -1)
      {
        for (int qlx = s; qlx < TradeItemListTop-1; qlx++)
          memcpy(&UDTradeItemList[qlx], &UDTradeItemList[qlx+1], sizeof(RULE_ActionArgumentsRecord));
        TradeItemListTop--;
      }
    }
  } else if (msg == UIMSG_KEYBOARD_ACHAR) {
    if (a == 8)
      { TradeAmt /= 10;  tradeMoney; }
    else if ((a >= '0') && (a <= '9') && (TradeAmt < 100000))
      { TradeAmt = (TradeAmt * 10) + (a - '0');  tradeMoney; }
  }

  DISPLAY_state.TradeCashBoxDesiredValue[plyr + 2] = TradeAmt;
//  DISPLAY_state.TradeCashBoxDesiredValue[(1 - plyr) + 2] = 0;
  DISPLAY_state.TradeCashBoxDesiredValue[0] =
    UICurrentGameState.Players[TradeAPlayerActive].cash - DISPLAY_state.TradeCashBoxDesiredValue[2] + DISPLAY_state.TradeCashBoxDesiredValue[3];
  DISPLAY_state.TradeCashBoxDesiredValue[1] =
    UICurrentGameState.Players[TradeBPlayerActive].cash - DISPLAY_state.TradeCashBoxDesiredValue[3] + DISPLAY_state.TradeCashBoxDesiredValue[2];

  if ((msg == UIMSG_KEYBOARD_ACHAR) && (a == 13))
    UDTrade_ProcessCashTrade(UIMSG_MOUSE_LEFT_DOWN, OkaySC.left, OkaySC.top);
}

/*****************************************************************************
* Function:   void UDTrade_InitFutureTrade(void)
* Returns:    Nothing
* Parameters: Nothing
* Purpose:    Prepares a dialog box to let user pick what future to trade.
*****************************************************************************/
void UDTrade_InitFutureTrade(void)
{
  // set dialog flag so that the only valid messages are the ones relating to the dialog box
  FutureTradeDlg.isVisible = FALSE;
  // set the dialog box position
  FutureTradeDlg.x = 600;  FutureTradeDlg.y = 0;
  // set the dialog box dimensions
  FutureTradeDlg.w = 200;  FutureTradeDlg.h = 225;
  FutureTradeDlg.mode = 0;
  //create the dialog box
  FutureTradeDlg.ID = rightpanel;
  FutureTradeDlg.priority = DISPLAY_TradeBoxBItemsPriority + 50;
}

/*****************************************************************************
* Function:   void UDTrade_DisplayFutureTradeDlg(void)
* Returns:    Nothing
* Parameters: Nothing
* Purpose:    Displays the future trade dialog box.
*****************************************************************************/
void UDTrade_DisplayFutureTradeDlg(int side)
{
  int i, j = 15, height, numlines;
  wchar_t WrappedLines[60][512], string[10];
  RULE_ActionArgumentsRecord msg;
  wchar_t tmpstring[4096];
  wchar_t *strings[7] = {
    LANG_GetTextMessage(TMN_TRADE_FUTURE_0),
    LANG_GetTextMessage(TMN_TRADE_FUTURE_A1+side),
    LANG_GetTextMessage(TMN_TRADE_FUTURE_2),
    NULL,
    LANG_GetTextMessage(TMN_TRADE_FUTURE_4),
    LANG_GetTextMessage(TMN_TRADE_FUTURE_5),
    LANG_GetTextMessage(TMN_TRADE_FUTURE_6) };

  curside = side;
  FutureTradeDlg.isVisible = TRUE;

  LE_SEQNCR_StartXY(LED_IFT(DAT_MAIN, CNK_byahaupi), FutureTradeDlg.priority+1, 22, -168);
  LE_SEQNCR_StartXY(LED_IFT(DAT_MAIN, CNK_byahadni), FutureTradeDlg.priority+1, 22, -247);
  LE_GRAFIX_ColorArea(FutureTradeDlg.ID, 0, 0, 200, 225, LEG_MCR(0,255,0));
  for (i = 0; i < SQ_TOTAL_PROPERTY_SQUARES; i++)
    PropertyProperties[i] = 0;

  if (FutureTradeDlg.mode == 0) {
    NumProperties = ListOffset = 0;
    for (i = 0; i < SQ_TOTAL_PROPERTY_SQUARES; i++)
      PropertyProperties[i] = PropertyList[i][0] = PropertyFlags[i] = 0;

    curproperty = curamount = NumProperties = 0;
  } else if (FutureTradeDlg.mode == 1) {
  } else if (FutureTradeDlg.mode == 2) {
    LE_FONTS_SetSettings(0);  LE_FONTS_GetSettings(7);
    Asprintf(string, A_T("%2i"), curamount);
    LE_GRAFIX_ColorArea(FutureTradeDlg.ID, 20, 104, 20, 9, LEG_MCR(0,255,0));
    LE_FONTS_Print(FutureTradeDlg.ID, ((20-LE_FONTS_GetStringWidth(string))/2)+20,
      101, LEG_MCR(220, 220, 220), string);
    LE_FONTS_GetSettings(0);
  } else if (FutureTradeDlg.mode == 3) {
    msg.numberA = TMN_TRADE_FUTURE_3;
    msg.numberB = curamount;
    msg.numberC = (side ? TradeAPlayerActive : TradeBPlayerActive);
    FormatErrorNotification(&msg, tmpstring);
    strings[FutureTradeDlg.mode] = tmpstring;
  } else if (FutureTradeDlg.mode == 4) {
    NumProperties = ListOffset = 0;
    for (i = 0; i < SQ_TOTAL_PROPERTY_SQUARES; i++)
      PropertyProperties[i] = PropertyList[i][0] = PropertyFlags[i] = 0;
    for (int iTemp1 = 0; iTemp1 < RULE_MAX_COUNT_HIT_SETS; iTemp1++) {
      if (UICurrentGameState.CountHits[iTemp1].toPlayer == (RULE_PlayerNumber)(side ? TradeBPlayerActive : TradeAPlayerActive)) {
        if ((UICurrentGameState.CountHits[iTemp1].hitType == CHT_FUTURE_RENT) &&
            (UICurrentGameState.CountHits[iTemp1].hitCount) &&
            (!UICurrentGameState.CountHits[iTemp1].tradedItem)) {
          PropertyFlags[NumProperties] = 0;
          PropertyProperties[NumProperties] = UICurrentGameState.CountHits[iTemp1].properties;
          Asprintf(PropertyList[NumProperties++], A_T("%02d %s"),
            UICurrentGameState.CountHits[iTemp1].hitCount,
RULE_SquarePredefinedInfo[RULE_BitSetToProperty(UICurrentGameState.CountHits[iTemp1].properties)].squareName);
//            LANG_GetTextMessageClean(ATR_PlaceName_00 +
//            RULE_BitSetToProperty(UICurrentGameState.CountHits[iTemp1].properties)));
        }
      }
    }
    UDTrade_ProcessFutureTrade(UIMSG_MOUSE_LEFT_DOWN, 0, 0);
  } else if ((FutureTradeDlg.mode == 5) ||
             (FutureTradeDlg.mode == 6)) {
    NumProperties = ListOffset = 0;
    for (int iTemp1 = 0; iTemp1 < TradeItemListTop; iTemp1++) {
      if ((UDTradeItemList[iTemp1].numberC == TIK_FUTURE_RENT) &&
          (UDTradeItemList[iTemp1].numberD) &&
          (UDTradeItemList[iTemp1].numberB == (RULE_PlayerNumber)(side ? TradeAPlayerActive : TradeBPlayerActive))) {
        PropertyFlags[NumProperties] = 0;
        PropertyProperties[NumProperties] = UDTradeItemList[iTemp1].numberE;
        Asprintf(PropertyList[NumProperties++], A_T("%02d %s"),
          UDTradeItemList[iTemp1].numberD,
RULE_SquarePredefinedInfo[RULE_BitSetToProperty(UDTradeItemList[iTemp1].numberE)].squareName);
//          LANG_GetTextMessageClean(ATR_PlaceName_00 +
//          RULE_BitSetToProperty(UDTradeItemList[iTemp1].numberE)));
      }
    }
    UDTrade_ProcessFutureTrade(UIMSG_MOUSE_LEFT_DOWN, 0, 0);
  }

  numlines = CHAT_WordWrap(LANG_GetTextMessage(TMN_TRADE_FUTURE), 176, (wchar_t **)WrappedLines, 7);
  LE_FONTS_SetSettings(0);  LE_FONTS_GetSettings(7);
  height = LE_FONTS_GetStringHeight(CHAT_WrappedLines[0]);
  for (i = 0; i < numlines; i++, j += height)
    LE_FONTS_Print(FutureTradeDlg.ID, (200 - LE_FONTS_GetStringWidth(CHAT_WrappedLines[i])) / 2, j,
      LEG_MCR(220, 220, 220), CHAT_WrappedLines[i]);
  j += 3;

  numlines = CHAT_WordWrap(strings[FutureTradeDlg.mode], 176, (wchar_t **)WrappedLines, 7);
  LE_FONTS_SetSettings(0);  LE_FONTS_GetSettings(7);
  for (i = 0; i < numlines; i++, j += height)
    LE_FONTS_Print(FutureTradeDlg.ID, (200 - LE_FONTS_GetStringWidth(CHAT_WrappedLines[i])) / 2, j,
      LEG_MCR(220, 220, 220), CHAT_WrappedLines[i]);

  AdjustRect1(FIOkaySC, FIOkayBC, FutureTradeDlg.x, FutureTradeDlg.y);
  DrawButton(FutureTradeDlg.ID, FIOkayBC, LEG_MCR(220, 220, 220), LANG_GetTextMessage(TMN_BUTTON_OK));

  LE_FONTS_GetSettings(0);
  if (FutureTradeDlg.mode != 0)
    UDTrade_ProcessFutureTrade(UIMSG_MOUSE_LEFT_DOWN, 0, 0); // Refresh small name / list
  LE_GRAFIX_InvalidateRect(&FutImmDlg);
}

/*****************************************************************************
* Function:   void UDTrade_RemoveFutureTradeDlg(void)
* Returns:    Nothing
* Parameters: Nothing
* Purpose:    Removes the Future trade dialog box.
*****************************************************************************/
void UDTrade_RemoveFutureTradeDlg(void)
{
  // remove the dialog box
  FutureTradeDlg.isVisible = FALSE;
  LE_GRAFIX_ColorArea(FutureTradeDlg.ID, 0, 0, 200, 225, LEG_MCR(0,255,0));
  LE_GRAFIX_InvalidateRect(&FutImmDlg);
  LE_SEQNCR_Stop(LED_IFT(DAT_MAIN, CNK_byahaupi), FutureTradeDlg.priority+1);
  LE_SEQNCR_Stop(LED_IFT(DAT_MAIN, CNK_byahadni), FutureTradeDlg.priority+1);
  FutureTradeDlg.mode = 0;
}

/*****************************************************************************
* Function:   void UDTrade_ProcessFutureTrade(int msg, int a, int b)
* Returns:    returns true or false
* Parameters: holds the player number to trade with
* Purpose:    Selects the player to trade with
*****************************************************************************/
void UDTrade_ProcessFutureTrade(int msg, int a, int b)
{
  wchar_t string[10];
  int i, x = a, y = b, deed, box;
  int plyr = curside;
  RULE_ActionArgumentsRecord TradeArguments = {
    ACTION_TRADE_ITEM, 0, {0, ""}, RULE_BANK_PLAYER, 0, 0, 0, 0, 0, A_T(""), NULL, 0, 0, NULL };

  if (FutureTradeDlg.mode == 0) {
    if (msg == UIMSG_MOUSE_LEFT_DOWN) {
      if (PointInRect(a, b, FIOkaySC)) {
        UDSOUND_Click();
        UDTrade_RemoveFutureTradeDlg();
        return;
      }

      for (i = 0; i < 4; i++)
        if ((x >= TradeBasicPlayerBoxes[i][0]) &&
            (x <  TradeBasicPlayerBoxes[i][0] + TRADE_PROPERTY_BasicW1) &&
            (y >= TradeBasicPlayerBoxes[i][1]) &&
            (y <  TradeBasicPlayerBoxes[i][1] + TRADE_PROPERTY_Height1))
        {
          UDSOUND_Click();
          FutureTradeDlg.mode++;
          UDTrade_DisplayFutureTradeDlg(1 - (i % 2));
          return;
        }
    }
  } else if (FutureTradeDlg.mode == 1) {
    if (msg == UIMSG_MOUSE_LEFT_DOWN) {
      if (PointInRect(a, b, FIOkaySC)) {
        UDSOUND_Click();
        if (curproperty == 0) {
          UDTrade_RemoveFutureTradeDlg();
          return;
        } else {
          FutureTradeDlg.mode++;
          UDTrade_DisplayFutureTradeDlg(curside);
        }
      }

      deed = UDTrade_MouseinProp(x, y);
      if (deed >= 0) {
        box = deed / 100;
        deed %= 100;
        if (curside == 0) {
          if ((box == 0) || (box == 3)) {
            if (!((TradeAAfter | TradeAAfterMort) & RULE_PropertyToBitSet(deed))) // Use the after array to avoid duplicate items
              deed = -1;
          } else deed = -1;
        } else if (curside == 1) {
          if ((box == 1) || (box == 2)) {
            if (!((TradeBAfter | TradeBAfterMort) & RULE_PropertyToBitSet(deed)))
              deed = -1;
          } else deed = -1;
        }

        if (deed >= 0)
        {
          LE_FONTS_SetSettings(0);  LE_FONTS_GetSettings(7);
          int added = 0;
          if (curproperty & RULE_PropertyToBitSet(deed)) {
            curproperty &= ~(RULE_PropertyToBitSet(deed));
            NumProperties--;
          } else {
            curproperty |= RULE_PropertyToBitSet(deed);
            added = RULE_PropertyToBitSet(deed);
            NumProperties++;
          }
          if (ListOffset >= NumProperties)
            ListOffset = NumProperties - 1;
          if (ListOffset < 0)
            ListOffset = 0;
          NumProperties = 0;
          for (int i = 0; i < SQ_TOTAL_PROPERTY_SQUARES; i++)
          {
            if (curproperty & (1 << i)) {
              if (added == (1 << i))
                ListOffset = NumProperties;
              wcscpy(PropertyList[NumProperties],
RULE_SquarePredefinedInfo[RULE_BitSetToProperty(1 << i)].squareName);
//                LANG_GetTextMessageClean(ATR_PlaceName_00 + RULE_BitSetToProperty(1 << i)));
              PropertyProperties[NumProperties] = 1 << i;
              NumProperties++;
            }
          }
        }
      }
    }
  } else if (FutureTradeDlg.mode == 2) {
    if (msg == UIMSG_MOUSE_LEFT_DOWN) {
      if (PointInRect(a, b, FIOkaySC)) {
        UDSOUND_Click();
        if (curamount <= 0) {
          UDTrade_RemoveFutureTradeDlg();
          return;
        } else {
          FutureTradeDlg.mode++;
          UDTrade_DisplayFutureTradeDlg(curside);
        }
      }
    } else if (msg == UIMSG_KEYBOARD_ACHAR) {
      if (a == 8)
        curamount /= 10;
      else if ((a >= '0') && (a <= '9') && (curamount < 10))
        curamount = (curamount * 10) + (a - '0');
      else if (a == 13) {
        UDTrade_ProcessFutureTrade(UIMSG_MOUSE_LEFT_DOWN, FIOkaySC.left, FIOkaySC.top);
        return;
      }

      LE_FONTS_SetSettings(0);  LE_FONTS_GetSettings(7);
      Asprintf(string, A_T("%2i"), curamount);
      LE_GRAFIX_ColorArea(FutureTradeDlg.ID, 20, 104, 20, 8, LEG_MCR(0,255,0));
      LE_FONTS_Print(FutureTradeDlg.ID, ((20-LE_FONTS_GetStringWidth(string))/2)+20,
        101, LEG_MCR(220, 220, 220), string);
      LE_FONTS_GetSettings(0);
    }
  } else if (FutureTradeDlg.mode == 3) {
    if (msg == UIMSG_MOUSE_LEFT_DOWN) {
      if (PointInRect(a, b, FIOkaySC)) {
        UDSOUND_Click();
        TradeArguments.fromPlayer = TradeFromPlayer;
        TradeArguments.numberC = (int)TIK_FUTURE_RENT;
        if (plyr == 0) {
          TradeArguments.numberA = TradeAPlayerActive;
          TradeArguments.numberB = TradeBPlayerActive;
          DISPLAY_state.TradeImmunityFutureDesiredA[0] |= (1 << 2);
        } else {
          TradeArguments.numberA = TradeBPlayerActive;
          TradeArguments.numberB = TradeAPlayerActive;
          DISPLAY_state.TradeImmunityFutureDesiredA[0] |= (1 << 3);
        }
        for (i = 0; i < SQ_TOTAL_PROPERTY_SQUARES; i++)
          if (curproperty & (1 << i)) {
            TradeArguments.numberD = curamount;
            TradeArguments.numberE = (1 << i);
            Trade_AddItem(&TradeArguments);
          }
        UDTrade_RemoveFutureTradeDlg();
        return;
      }
    }
  } else if (FutureTradeDlg.mode == 4) {
    if (msg == UIMSG_MOUSE_LEFT_DOWN) {
      if (PointInRect(a, b, FIOkaySC)) {
        UDSOUND_Click();
        for (i = 0; i < NumProperties; i++)
          if (PropertyFlags[i]) {
            TradeArguments.fromPlayer = TradeFromPlayer;
            TradeArguments.numberC = (int)TIK_FUTURE_RENT;
            if (plyr == 0) {
              TradeArguments.numberA = TradeAPlayerActive;
              TradeArguments.numberB = TradeBPlayerActive;
            } else {
              TradeArguments.numberA = TradeBPlayerActive;
              TradeArguments.numberB = TradeAPlayerActive;
            }
            TradeArguments.numberD = _wtoi(PropertyList[i]);
            TradeArguments.numberE = PropertyProperties[i];
            Trade_AddItem(&TradeArguments);

            TradeArguments.fromPlayer = TradeFromPlayer;
            TradeArguments.numberC = (int)TIK_FUTURE_RENT;
            if (plyr == 0) {
              TradeArguments.numberA = TradeBPlayerActive;
              TradeArguments.numberB = TradeAPlayerActive;
            } else {
              TradeArguments.numberA = TradeAPlayerActive;
              TradeArguments.numberB = TradeBPlayerActive;
            }
            TradeArguments.numberD = -_wtoi(PropertyList[i]);
            TradeArguments.numberE = PropertyProperties[i];
            Trade_AddItem(&TradeArguments);
          }
        UDTrade_RemoveFutureTradeDlg();
        return;
      }
      int tmp = UDTrade_MouseinList(a, b);
      if (tmp > -1)
        PropertyFlags[tmp] = 1 - PropertyFlags[tmp];
    }
  } else if (FutureTradeDlg.mode == 5) {
    if (msg == UIMSG_MOUSE_LEFT_DOWN) {
      if (PointInRect(a, b, FIOkaySC)) {
        UDSOUND_Click();
        for (i = 0; i < NumProperties; i++)
          if (PropertyFlags[i]) {
            TradeArguments.fromPlayer = TradeFromPlayer;
            TradeArguments.numberC = (int)TIK_FUTURE_RENT;
            if (plyr == 0) {
              TradeArguments.numberA = TradeAPlayerActive;
              TradeArguments.numberB = TradeBPlayerActive;
            } else {
              TradeArguments.numberA = TradeBPlayerActive;
              TradeArguments.numberB = TradeAPlayerActive;
            }
            TradeArguments.numberD = 0;
            TradeArguments.numberE = PropertyProperties[i];
            Trade_AddItem(&TradeArguments);

            // Ick.  Get rid of negative if it's there
            if (plyr == 0) {
              TradeArguments.numberA = TradeBPlayerActive;
              TradeArguments.numberB = TradeAPlayerActive;
            } else {
              TradeArguments.numberA = TradeAPlayerActive;
              TradeArguments.numberB = TradeBPlayerActive;
            }
            Trade_AddItem(&TradeArguments);
          }
        UDTrade_RemoveFutureTradeDlg();
        return;
      }
      int tmp = UDTrade_MouseinList(a, b);
      if (tmp > -1)
        PropertyFlags[tmp] = 1 - PropertyFlags[tmp];
    }
  } else if (FutureTradeDlg.mode == 6) {
    if (msg == UIMSG_MOUSE_LEFT_DOWN) {
      if (PointInRect(a, b, FIOkaySC)) {
        UDSOUND_Click();
        UDTrade_RemoveFutureTradeDlg();
        return;
      }
    }
  }

  if (msg == UIMSG_MOUSE_LEFT_DOWN) {
    if (PointInRect(a, b, UpSC))
      ListOffset--;
    else if (PointInRect(a, b, DnSC))
      ListOffset++;
  }

  if (ListOffset < 0)  ListOffset = 0;
  if (ListOffset >= NumProperties)  ListOffset = NumProperties - 1;
#if USA_VERSION
  LE_GRAFIX_ColorArea(FutureTradeDlg.ID, 14, 115, 152, 69, LEG_MCR(0,255,0));
#else
  LE_GRAFIX_ColorArea(FutureTradeDlg.ID, 14, 113, 152, 72, LEG_MCR(0,255,0));
#endif
  int j = 113;
  LE_FONTS_SetSettings(0);  LE_FONTS_GetSettings(7);
  int height = LE_FONTS_GetStringHeight(PropertyList[0]);
  for (i = ListOffset; i < ListOffset + 5; i++, j += height)
    if (i < NumProperties) {
      LE_GRAFIX_ColorArea(FutureTradeDlg.ID, (short)listbox[i - ListOffset].left-600, (short)listbox[i - ListOffset].top,
        width(listbox[i - ListOffset]), height(listbox[i - ListOffset]), (PropertyFlags[i]?LEG_MCR(0, 0, 180):LEG_MCR(0, 255, 0)));
      LE_FONTS_Print(FutureTradeDlg.ID, 16, j,
        (PropertyFlags[i]?LEG_MCR(255, 255, 255):LEG_MCR(220, 220, 220)), PropertyList[i]);
    }
#if USA_VERSION
  LE_GRAFIX_ColorArea(FutureTradeDlg.ID, 14+152, 115, 50, 69, LEG_MCR(0,255,0));//100 / 73
#else
  LE_GRAFIX_ColorArea(FutureTradeDlg.ID, 14+152, 113, 50, 72, LEG_MCR(0,255,0));//100 / 73
#endif
  LE_FONTS_GetSettings(0);

  if (FutureTradeDlg.mode != 3) {
    LE_FONTS_SetSettings(0);  LE_FONTS_GetSettings(7);
    LE_FONTS_Print(FutureTradeDlg.ID, 42, 101, LEG_MCR(220, 220, 220),
      UICurrentGameState.Players[(plyr)?TradeAPlayerActive:TradeBPlayerActive].name);
    LE_FONTS_GetSettings(0);
  }
//  }
  LE_GRAFIX_InvalidateRect(&FutImmDlg);
}

/*****************************************************************************
* Function:   void UDTrade_InitImmunityTrade(void)
* Returns:    Nothing
* Parameters: Nothing
* Purpose:    Prepares a dialog box to let user pick what Immunity to trade.
*****************************************************************************/
void UDTrade_InitImmunityTrade(void)
{
  // set dialog flag so that the only valid messages are the ones relating to the dialog box
  ImmunityTradeDlg.isVisible = FALSE;
  // set the dialog box position
  ImmunityTradeDlg.x = 600;  ImmunityTradeDlg.y = 0;
  // set the dialog box dimensions
  ImmunityTradeDlg.w = 200;  ImmunityTradeDlg.h = 225;
  ImmunityTradeDlg.mode = 0;
  ImmunityTradeDlg.ID = rightpanel;
  ImmunityTradeDlg.priority = DISPLAY_TradeBoxBItemsPriority + 50;
}

/*****************************************************************************
* Function:   void UDTrade_DisplayImmunityTradeDlg(void)
* Returns:    Nothing
* Parameters: Nothing
* Purpose:    Displays the immunity trade dialog box.
*****************************************************************************/
void UDTrade_DisplayImmunityTradeDlg(int side)
{
  int i, j = 15, height, numlines;
  wchar_t WrappedLines[60][512], string[10];
  RULE_ActionArgumentsRecord msg;
  wchar_t tmpstring[4096];
  wchar_t *strings[7] = {
    LANG_GetTextMessage(TMN_TRADE_IMMUNITY_0),
    LANG_GetTextMessage(TMN_TRADE_IMMUNITY_A1+side),
    LANG_GetTextMessage(TMN_TRADE_IMMUNITY_2),
    NULL,
    LANG_GetTextMessage(TMN_TRADE_IMMUNITY_4),
    LANG_GetTextMessage(TMN_TRADE_IMMUNITY_5),
    LANG_GetTextMessage(TMN_TRADE_IMMUNITY_6) };

  curside = side;
  ImmunityTradeDlg.isVisible = TRUE;

  LE_SEQNCR_StartXY(LED_IFT(DAT_MAIN, CNK_byahaupi), ImmunityTradeDlg.priority+1, 22, -168);
  LE_SEQNCR_StartXY(LED_IFT(DAT_MAIN, CNK_byahadni), ImmunityTradeDlg.priority+1, 22, -247);
  LE_GRAFIX_ColorArea(ImmunityTradeDlg.ID, 0, 0, 200, 225, LEG_MCR(0,255,0));
  for (i = 0; i < SQ_TOTAL_PROPERTY_SQUARES; i++)
    PropertyProperties[i] = 0;

  if (ImmunityTradeDlg.mode == 0) {
    NumProperties = ListOffset = 0;
    for (i = 0; i < SQ_TOTAL_PROPERTY_SQUARES; i++)
      PropertyProperties[i] = PropertyList[i][0] = PropertyFlags[i] = 0;

    curproperty = curamount = NumProperties = 0;
  } else if (ImmunityTradeDlg.mode == 1) {
  } else if (ImmunityTradeDlg.mode == 2) {
    LE_FONTS_SetSettings(0);  LE_FONTS_GetSettings(7);
    Asprintf(string, A_T("%2i"), curamount);
    LE_GRAFIX_ColorArea(ImmunityTradeDlg.ID, 20, 104, 20, 9, LEG_MCR(0,255,0));
    LE_FONTS_Print(ImmunityTradeDlg.ID, ((20-LE_FONTS_GetStringWidth(string))/2)+20,
      101, LEG_MCR(220, 220, 220), string);
    LE_FONTS_GetSettings(0);
  } else if (ImmunityTradeDlg.mode == 3) {
    msg.numberA = TMN_TRADE_IMMUNITY_3;
    msg.numberB = curamount; //number of times
    msg.numberC = (side ? TradeAPlayerActive : TradeBPlayerActive);
    FormatErrorNotification(&msg, tmpstring);
    strings[ImmunityTradeDlg.mode] = tmpstring;
  } else if (ImmunityTradeDlg.mode == 4) {
    NumProperties = ListOffset = 0;
    for (i = 0; i < SQ_TOTAL_PROPERTY_SQUARES; i++)
      PropertyProperties[i] = PropertyList[i][0] = PropertyFlags[i] = 0;
    for (int iTemp1 = 0; iTemp1 < RULE_MAX_COUNT_HIT_SETS; iTemp1++) {
      if (UICurrentGameState.CountHits[iTemp1].toPlayer == (RULE_PlayerNumber)(side ? TradeBPlayerActive : TradeAPlayerActive)) {
        if ((UICurrentGameState.CountHits[iTemp1].hitType == CHT_RENT_IMMUNITY) &&
            (UICurrentGameState.CountHits[iTemp1].hitCount) &&
            (!UICurrentGameState.CountHits[iTemp1].tradedItem)) {
          PropertyFlags[NumProperties] = 0;
          PropertyProperties[NumProperties] = UICurrentGameState.CountHits[iTemp1].properties;
          Asprintf(PropertyList[NumProperties++], A_T("%02d %s"),
            UICurrentGameState.CountHits[iTemp1].hitCount,
RULE_SquarePredefinedInfo[RULE_BitSetToProperty(UICurrentGameState.CountHits[iTemp1].properties)].squareName);
//            LANG_GetTextMessageClean(ATR_PlaceName_00 +
//            RULE_BitSetToProperty(UICurrentGameState.CountHits[iTemp1].properties)));
        }
      }
    }
    UDTrade_ProcessImmunityTrade(UIMSG_MOUSE_LEFT_DOWN, 0, 0);
  } else if ((ImmunityTradeDlg.mode == 5) ||
             (ImmunityTradeDlg.mode == 6)) {
    NumProperties = ListOffset = 0;
    for (int iTemp1 = 0; iTemp1 < TradeItemListTop; iTemp1++) {
      if ((UDTradeItemList[iTemp1].numberC == TIK_IMMUNITY) &&
          (UDTradeItemList[iTemp1].numberD) &&
          (UDTradeItemList[iTemp1].numberB == (RULE_PlayerNumber)(side ? TradeAPlayerActive : TradeBPlayerActive))) {
        PropertyFlags[NumProperties] = 0;
        PropertyProperties[NumProperties] = UDTradeItemList[iTemp1].numberE;
        Asprintf(PropertyList[NumProperties++], A_T("%02d %s"),
          UDTradeItemList[iTemp1].numberD,
RULE_SquarePredefinedInfo[RULE_BitSetToProperty(UDTradeItemList[iTemp1].numberE)].squareName);
//          LANG_GetTextMessageClean(ATR_PlaceName_00 +
//          RULE_BitSetToProperty(UDTradeItemList[iTemp1].numberE)));
      }
    }
    UDTrade_ProcessImmunityTrade(UIMSG_MOUSE_LEFT_DOWN, 0, 0);
  }

  numlines = CHAT_WordWrap(LANG_GetTextMessage(TMN_TRADE_IMMUNITY), 176, (wchar_t **)WrappedLines, 7);
  LE_FONTS_SetSettings(0);  LE_FONTS_GetSettings(7);
  height = LE_FONTS_GetStringHeight(CHAT_WrappedLines[0]);
  for (i = 0; i < numlines; i++, j += height)
    LE_FONTS_Print(ImmunityTradeDlg.ID, (200 - LE_FONTS_GetStringWidth(CHAT_WrappedLines[i])) / 2, j,
      LEG_MCR(220, 220, 220), CHAT_WrappedLines[i]);
  j += 3;

  numlines = CHAT_WordWrap(strings[ImmunityTradeDlg.mode], 176, (wchar_t **)WrappedLines, 7);
  LE_FONTS_SetSettings(0);  LE_FONTS_GetSettings(7);
  for (i = 0; i < numlines; i++, j += height)
    LE_FONTS_Print(ImmunityTradeDlg.ID, (200 - LE_FONTS_GetStringWidth(CHAT_WrappedLines[i])) / 2, j,
      LEG_MCR(220, 220, 220), CHAT_WrappedLines[i]);

  AdjustRect1(FIOkaySC, FIOkayBC, ImmunityTradeDlg.x, ImmunityTradeDlg.y);
  DrawButton(ImmunityTradeDlg.ID, FIOkayBC, LEG_MCR(220, 220, 220), LANG_GetTextMessage(TMN_BUTTON_OK));

  LE_FONTS_GetSettings(0);
  if (ImmunityTradeDlg.mode != 0)
    UDTrade_ProcessImmunityTrade(UIMSG_MOUSE_LEFT_DOWN, 0, 0); // Refresh small name / list
  LE_GRAFIX_InvalidateRect(&FutImmDlg);
}

/*****************************************************************************
* Function:   void UDTrade_RemoveImmunityTradeDlg(void)
* Returns:    Nothing
* Parameters: Nothing
* Purpose:    Removes the Immunity trade dialog box.
*****************************************************************************/
void UDTrade_RemoveImmunityTradeDlg(void)
{
  // remove the dialog box
  ImmunityTradeDlg.isVisible = FALSE;
  LE_GRAFIX_ColorArea(ImmunityTradeDlg.ID, 0, 0, 200, 225, LEG_MCR(0,255,0));
  LE_GRAFIX_InvalidateRect(&FutImmDlg);
  LE_SEQNCR_Stop(LED_IFT(DAT_MAIN, CNK_byahaupi), ImmunityTradeDlg.priority+1);
  LE_SEQNCR_Stop(LED_IFT(DAT_MAIN, CNK_byahadni), ImmunityTradeDlg.priority+1);
  ImmunityTradeDlg.mode = 0;
}

/*****************************************************************************
* Function:   void UDTrade_ProcessImmunityTrade(int msg, int a, int b)
* Returns:    returns true or false
* Parameters: holds the player number to trade with
* Purpose:    Selects the player to trade with
*****************************************************************************/
void UDTrade_ProcessImmunityTrade(int msg, int a, int b)
{
  wchar_t string[10];
  int i, x = a, y = b, deed, box;
  int plyr = curside;
  RULE_ActionArgumentsRecord TradeArguments = {
    ACTION_TRADE_ITEM, 0, {0, ""}, RULE_BANK_PLAYER, 0, 0, 0, 0, 0, A_T(""), NULL, 0, 0, NULL };

  if (ImmunityTradeDlg.mode == 0) {
    if (msg == UIMSG_MOUSE_LEFT_DOWN) {
      if (PointInRect(a, b, FIOkaySC)) {
        UDSOUND_Click();
        UDTrade_RemoveImmunityTradeDlg();
        return;
      }

      for (i = 0; i < 4; i++)
        if ((x >= TradeBasicPlayerBoxes[i][0]) &&
            (x <  TradeBasicPlayerBoxes[i][0] + TRADE_PROPERTY_BasicW1) &&
            (y >= TradeBasicPlayerBoxes[i][1]) &&
            (y <  TradeBasicPlayerBoxes[i][1] + TRADE_PROPERTY_Height1))
        {
          ImmunityTradeDlg.mode++;
          UDTrade_DisplayImmunityTradeDlg(1 - (i % 2));
          return;
        }
    }
  } else if (ImmunityTradeDlg.mode == 1) {
    if (msg == UIMSG_MOUSE_LEFT_DOWN) {
      if (PointInRect(a, b, FIOkaySC)) {
        UDSOUND_Click();
        if (curproperty == 0) {
          UDTrade_RemoveImmunityTradeDlg();
          return;
        } else {
          ImmunityTradeDlg.mode++;
          UDTrade_DisplayImmunityTradeDlg(curside);
        }
      }

      deed = UDTrade_MouseinProp(x, y);
      if (deed >= 0) {
        box = deed / 100;
        deed %= 100;
        if (curside == 0) {
          if ((box == 0) || (box == 3)) {
            if (!((TradeAAfter | TradeAAfterMort) & RULE_PropertyToBitSet(deed))) // Use the after array to avoid duplicate items
              deed = -1;
          } else deed = -1;
        } else if (curside == 1) {
          if ((box == 1) || (box == 2)) {
            if (!((TradeBAfter | TradeBAfterMort) & RULE_PropertyToBitSet(deed)))
              deed = -1;
          } else deed = -1;
        }

        if (deed >= 0)
        {
          LE_FONTS_SetSettings(0);  LE_FONTS_GetSettings(7);
          int added = 0;
          if (curproperty & RULE_PropertyToBitSet(deed)) {
            curproperty &= ~(RULE_PropertyToBitSet(deed));
            NumProperties--;
          } else {
            curproperty |= RULE_PropertyToBitSet(deed);
            added = RULE_PropertyToBitSet(deed);
            NumProperties++;
          }
          if (ListOffset >= NumProperties)
            ListOffset = NumProperties - 1;
          if (ListOffset < 0)
            ListOffset = 0;
          NumProperties = 0;
          for (int i = 0; i < SQ_TOTAL_PROPERTY_SQUARES; i++)
          {
            if (curproperty & (1 << i)) {
              if (added == (1 << i))
                ListOffset = NumProperties;
              wcscpy(PropertyList[NumProperties],
RULE_SquarePredefinedInfo[RULE_BitSetToProperty(1 << i)].squareName);
//                LANG_GetTextMessageClean(ATR_PlaceName_00 + RULE_BitSetToProperty(1 << i)));
              PropertyProperties[NumProperties] = 1 << i;
              NumProperties++;
            }
          }
        }
      }
    }

  } else if (ImmunityTradeDlg.mode == 2) {
    if (msg == UIMSG_MOUSE_LEFT_DOWN) {
      if (PointInRect(a, b, FIOkaySC)) {
        UDSOUND_Click();
        if (curamount <= 0) {
          UDTrade_RemoveImmunityTradeDlg();
          return;
        } else {
          ImmunityTradeDlg.mode++;
          UDTrade_DisplayImmunityTradeDlg(curside);
        }
      }
    } else if (msg == UIMSG_KEYBOARD_ACHAR) {
      if (a == 8)
        curamount /= 10;
      else if ((a >= '0') && (a <= '9') && (curamount < 10))
        curamount = (curamount * 10) + (a - '0');
      else if (a == 13) {
        UDTrade_ProcessImmunityTrade(UIMSG_MOUSE_LEFT_DOWN, FIOkaySC.left, FIOkaySC.top);
        return;
      }

      LE_FONTS_SetSettings(0);  LE_FONTS_GetSettings(7);
      Asprintf(string, A_T("%2i"), curamount);
      LE_GRAFIX_ColorArea(ImmunityTradeDlg.ID, 20, 104, 20, 8, LEG_MCR(0,255,0));
      LE_FONTS_Print(ImmunityTradeDlg.ID, ((20-LE_FONTS_GetStringWidth(string))/2)+20,
        101, LEG_MCR(220, 220, 220), string);
      LE_FONTS_GetSettings(0);
    }
  } else if (ImmunityTradeDlg.mode == 3) {
    if (msg == UIMSG_MOUSE_LEFT_DOWN) {
      if (PointInRect(a, b, FIOkaySC)) {
        UDSOUND_Click();
        TradeArguments.fromPlayer = TradeFromPlayer;
        TradeArguments.numberC = (int)TIK_IMMUNITY;
        if (plyr == 0) {
          TradeArguments.numberA = TradeAPlayerActive;
          TradeArguments.numberB = TradeBPlayerActive;
          DISPLAY_state.TradeImmunityFutureDesiredA[1] |= (1 << 2);
        } else {
          TradeArguments.numberA = TradeBPlayerActive;
          TradeArguments.numberB = TradeAPlayerActive;
          DISPLAY_state.TradeImmunityFutureDesiredA[1] |= (1 << 3);
        }
        for (i = 0; i < SQ_TOTAL_PROPERTY_SQUARES; i++)
          if (curproperty & (1 << i)) {
            TradeArguments.numberD = curamount;
            TradeArguments.numberE = (1 << i);
            Trade_AddItem(&TradeArguments);
          }
        UDTrade_RemoveImmunityTradeDlg();
        return;
      }
    }
  } else if (ImmunityTradeDlg.mode == 4) {
    if (msg == UIMSG_MOUSE_LEFT_DOWN) {
      if (PointInRect(a, b, FIOkaySC)) {
        UDSOUND_Click();
        for (i = 0; i < NumProperties; i++)
          if (PropertyFlags[i]) {
            TradeArguments.fromPlayer = TradeFromPlayer;
            TradeArguments.numberC = (int)TIK_IMMUNITY;
            if (plyr == 0) {
              TradeArguments.numberA = TradeAPlayerActive;
              TradeArguments.numberB = TradeBPlayerActive;
            } else {
              TradeArguments.numberA = TradeBPlayerActive;
              TradeArguments.numberB = TradeAPlayerActive;
            }
            TradeArguments.numberD = _wtoi(PropertyList[i]);
            TradeArguments.numberE = PropertyProperties[i];
            Trade_AddItem(&TradeArguments);

            TradeArguments.fromPlayer = TradeFromPlayer;
            TradeArguments.numberC = (int)TIK_IMMUNITY;
            if (plyr == 0) {
              TradeArguments.numberA = TradeBPlayerActive;
              TradeArguments.numberB = TradeAPlayerActive;
            } else {
              TradeArguments.numberA = TradeAPlayerActive;
              TradeArguments.numberB = TradeBPlayerActive;
            }
            TradeArguments.numberD = -_wtoi(PropertyList[i]);
            TradeArguments.numberE = PropertyProperties[i];
            Trade_AddItem(&TradeArguments);
          }
        UDTrade_RemoveImmunityTradeDlg();
        return;
      }
      int tmp = UDTrade_MouseinList(a, b);
      if (tmp > -1)
        PropertyFlags[tmp] = 1 - PropertyFlags[tmp];
    }
  } else if (ImmunityTradeDlg.mode == 5) {
    if (msg == UIMSG_MOUSE_LEFT_DOWN) {
      if (PointInRect(a, b, FIOkaySC)) {
        UDSOUND_Click();
        for (i = 0; i < NumProperties; i++)
          if (PropertyFlags[i]) {
            TradeArguments.fromPlayer = TradeFromPlayer;
            TradeArguments.numberC = (int)TIK_IMMUNITY;
            if (plyr == 0) {
              TradeArguments.numberA = TradeAPlayerActive;
              TradeArguments.numberB = TradeBPlayerActive;
            } else {
              TradeArguments.numberA = TradeBPlayerActive;
              TradeArguments.numberB = TradeAPlayerActive;
            }
            TradeArguments.numberD = 0;
            TradeArguments.numberE = PropertyProperties[i];
            Trade_AddItem(&TradeArguments);

            // Ick.  Get rid of negative if it's there
            if (plyr == 0) {
              TradeArguments.numberA = TradeBPlayerActive;
              TradeArguments.numberB = TradeAPlayerActive;
            } else {
              TradeArguments.numberA = TradeAPlayerActive;
              TradeArguments.numberB = TradeBPlayerActive;
            }
            Trade_AddItem(&TradeArguments);
          }
        UDTrade_RemoveImmunityTradeDlg();
        return;
      }
      int tmp = UDTrade_MouseinList(a, b);
      if (tmp > -1)
        PropertyFlags[tmp] = 1 - PropertyFlags[tmp];
    }
  } else if (ImmunityTradeDlg.mode == 6) {
    if (msg == UIMSG_MOUSE_LEFT_DOWN) {
      if (PointInRect(a, b, FIOkaySC)) {
        UDSOUND_Click();
        UDTrade_RemoveImmunityTradeDlg();
        return;
      }
    }
  }

  if (msg == UIMSG_MOUSE_LEFT_DOWN) {
    if (PointInRect(a, b, UpSC))
      ListOffset--;
    else if (PointInRect(a, b, DnSC))
      ListOffset++;
  }

  if (ListOffset < 0)  ListOffset = 0;
  if (ListOffset >= NumProperties)  ListOffset = NumProperties - 1;
#if USA_VERSION
  LE_GRAFIX_ColorArea(ImmunityTradeDlg.ID, 14, 115, 152, 69, LEG_MCR(0,255,0));
#else
  LE_GRAFIX_ColorArea(ImmunityTradeDlg.ID, 14, 113, 152, 72, LEG_MCR(0,255,0));
#endif
  int j = 113;
  LE_FONTS_SetSettings(0);  LE_FONTS_GetSettings(7);
  int height = LE_FONTS_GetStringHeight(PropertyList[0]);
  for (i = ListOffset; i < ListOffset + 5; i++, j += height)
    if (i < NumProperties) {
      LE_GRAFIX_ColorArea(ImmunityTradeDlg.ID, (short)listbox[i - ListOffset].left-600, (short)listbox[i - ListOffset].top,
        width(listbox[i - ListOffset]), height(listbox[i - ListOffset]), (PropertyFlags[i]?LEG_MCR(0, 0, 180):LEG_MCR(0, 255, 0)));
      LE_FONTS_Print(ImmunityTradeDlg.ID, 16, j,
        (PropertyFlags[i]?LEG_MCR(255, 255, 255):LEG_MCR(220, 220, 220)), PropertyList[i]);
    }
#if USA_VERSION
  LE_GRAFIX_ColorArea(ImmunityTradeDlg.ID, 14+152, 115, 50, 69, LEG_MCR(0,255,0));
#else
  LE_GRAFIX_ColorArea(ImmunityTradeDlg.ID, 14+152, 113, 50, 72, LEG_MCR(0,255,0));
#endif
  LE_FONTS_GetSettings(0);

  if (ImmunityTradeDlg.mode != 3) {
    LE_FONTS_SetSettings(0);  LE_FONTS_GetSettings(7);
    LE_FONTS_Print(ImmunityTradeDlg.ID, 42, 101, LEG_MCR(220, 220, 220),
      UICurrentGameState.Players[(plyr)?TradeAPlayerActive:TradeBPlayerActive].name);
    LE_FONTS_GetSettings(0);
  }
  LE_GRAFIX_InvalidateRect(&FutImmDlg);
}
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//
/*****************************************************************************
* Function:   int UDTrade_MouseinProp(int, int)
* Returns:    square * 100 + property (or -1)
* Parameters: x, y
* Purpose:    see if the mouse is in a deed
*****************************************************************************/
int UDTrade_MouseinProp(int x, int y)
{
  register int i;
  int j, box = -1;
  // Map properties in array order to priority order on screen
  static int ItemMapFn[SQ_TOTAL_PROPERTY_SQUARES] =
   { 5, 15, 25, 35, 12, 28, 1, 3, 6, 8, 9, 11, 13, 14, 16, 18, 19, 21, 23, 24, 26, 27, 29, 31, 32, 34, 37, 39 };
  static PropertyPositionStruct *pps[4] = {
    &DesiredBeforePropertyA, &DesiredBeforePropertyB,
    &TradeBeforePropertyA, &TradeBeforePropertyB };

  for (i = 0; i < 4; i++)
    if ((x >= TradeBasicPlayerBoxes[i][0]) &&
        (x <  TradeBasicPlayerBoxes[i][0] + TRADE_PROPERTY_BasicW1) &&
        (y >= TradeBasicPlayerBoxes[i][1]) &&
        (y <  TradeBasicPlayerBoxes[i][1] + TRADE_PROPERTY_Height1))
    {
      box = i;
      break;
    }

  if (box >= 0) //Find the property within the box.
    for (i = 0; i < SQ_TOTAL_PROPERTY_SQUARES; i++)
    {
      j = ItemMapFn[i];
      if ((pps[box]->ID[j]) &&
          (x >= TradeBasicPlayerBoxes[box][0] + pps[box]->Location[j].left) &&
          (x <  TradeBasicPlayerBoxes[box][0] + pps[box]->Location[j].right) &&
          (y >= TradeBasicPlayerBoxes[box][1] + pps[box]->Location[j].top) &&
          (y <  TradeBasicPlayerBoxes[box][1] + pps[box]->Location[j].bottom))
        return((box * 100) + j);
    }
  return(-1);
}

/*****************************************************************************
* Function:   int UDTrade_MouseinList(int, int)
* Returns:    0 - numitems (or -1)
* Parameters: x, y
* Purpose:    see if the mouse is in a list item
*****************************************************************************/
int UDTrade_MouseinList(int x, int y)
{
  register int i;
//  LE_GRAFIX_ColorArea(ImmunityTradeDlg.ID, 16, 115, 150, 67, LEG_MCR(0,255,0));

  for (i = 0; i < 5; i++)
    if (PointInRect(x, y, listbox[i]))
      break;
  if (i != 5)
    if ((ListOffset + i) < NumProperties)
      return(ListOffset + i);
  return(-1);
}

/*****************************************************************************
* Function:   void UDTrade_ProcessEverything(void)
* Returns:    nothing
* Parameters: nothing
* Purpose:    process every calculation needed to display or remove the deeds.
*****************************************************************************/
void UDTrade_ProcessEverything(void)
{
  // Recalculate what should be on the screen, compare to what is on the screen,
  // and take corrective action.
  register int iTemp;
  int t, order, box, deed;
  RULE_PropertySet lTemp;
  RULE_PropertySet DesiredAStuff, DesiredBStuff, DesiredAAfter, DesiredBAfter;
  RULE_PropertySet DesiredAStuffMort, DesiredBStuffMort, DesiredAAfterMort, DesiredBAfterMort;
  RULE_PropertySet DesiredTradePropsA, DesiredTradePropsAMort, DesiredTradePropsB, DesiredTradePropsBMort;
  LE_DATA_DataId tempID;
  LE_DATA_DataId TradeADesiredTokenID, TradeBDesiredTokenID;
  ACHAR myBuffer[40];
  static RECT *CashRects[4] = { &CashTradeAT2, &CashTradeBT2, &CashTradeAM2, &CashTradeBM2 };
  static int leftBakaColour, rightBakaColour;

  // Token bar - Set to nothing desired and upgrade from there.
  TradeADesiredTokenID = TradeBDesiredTokenID = LED_EI;

  if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA)
  {
    // Yikes.  A trading player went bankrupt while in the trade editor.
    if ((TradeAPlayerActive != RULE_MAX_PLAYERS) && (TradeBPlayerActive != RULE_MAX_PLAYERS)) {
      if ((UICurrentGameState.Players[TradeAPlayerActive].currentSquare >= SQ_OFF_BOARD) ||
          (UICurrentGameState.Players[TradeBPlayerActive].currentSquare >= SQ_OFF_BOARD))
      {
        TradeFromPlayer = -1; // Forget who started the trade
        UDTrade_ClearTrade();
        TradeAPlayerActive = TradeBPlayerActive = RULE_MAX_PLAYERS;
        DISPLAY_state.IBarLastActionNotificationFor = IBAR_BUTTON_ANIM_Main;
        UDBOARD_SetBackdrop( DISPLAY_SCREEN_MainA );
        DISPLAY_showAll;
      }
    }

    if ((CashTradeDying) &&
        ((LE_SEQNCR_IsSequenceFinished(LED_IFT(DAT_LANG2, CNK_tytsmncp), CashTradeDlg.priority+3, FALSE)) &&
         (LE_SEQNCR_IsSequenceFinished(LED_IFT(DAT_LANG2, CNK_tytsmnop), CashTradeDlg.priority+3, FALSE)) &&
         (LE_SEQNCR_IsSequenceFinished(LED_IFT(DAT_LANG2, CNK_tytsmnxp), CashTradeDlg.priority+3, FALSE)))) {
      CashTradeDying = FALSE;
      UDTrade_RemoveCashTradeDlg();
    }

    if ((TradeAPlayerActive >= 0) && (TradeAPlayerActive < RULE_MAX_PLAYERS))
      TradeADesiredTokenID = LED_IFT(DAT_MAIN, TAB_inpsa+UICurrentGameState.Players[TradeAPlayerActive].token);
    if ((TradeBPlayerActive >= 0) && (TradeBPlayerActive < RULE_MAX_PLAYERS))
      TradeBDesiredTokenID = LED_IFT(DAT_MAIN, TAB_inpsa+UICurrentGameState.Players[TradeBPlayerActive].token);
  }
  // Affect changes
  if (TradeADesiredTokenID != TradeATokenIDShown)
  {
    if (TradeATokenIDShown != LED_EI)
      LE_SEQNCR_Stop(TradeATokenIDShown, DISPLAY_TradeBasePriority);
    TradeATokenIDShown = TradeADesiredTokenID;
    if (TradeATokenIDShown != LED_EI)
      LE_SEQNCR_StartXY(TradeATokenIDShown, DISPLAY_TradeBasePriority, 10, 235);
  }
  if (TradeBDesiredTokenID != TradeBTokenIDShown)
  {
    if (TradeBTokenIDShown != LED_EI)
      LE_SEQNCR_Stop(TradeBTokenIDShown, DISPLAY_TradeBasePriority + 1);
    TradeBTokenIDShown = TradeBDesiredTokenID;
    if (TradeBTokenIDShown != LED_EI)
      LE_SEQNCR_StartXY(TradeBDesiredTokenID, DISPLAY_TradeBasePriority + 1,
        790 - LE_GRAFIX_AnyBitmapWidth(TradeBDesiredTokenID), 235);
  }


  int offered1 = 0, offered2 = 0;
  if (DISPLAY_state.TradeJailCardsDesired[0] & (1 << 2))  offered1 = 1;
  if (DISPLAY_state.TradeJailCardsDesired[0] & (1 << 3))  offered1 = 2;
  if (DISPLAY_state.TradeJailCardsDesired[1] & (1 << 2))  offered2 = 1;
  if (DISPLAY_state.TradeJailCardsDesired[1] & (1 << 3))  offered2 = 2;

  DISPLAY_state.TradeJailCardsDesired[0] = 0;
  if (UICurrentGameState.Cards[CHANCE_DECK].jailOwner == (RULE_PlayerNumber)TradeAPlayerActive) {
    if (offered1 == 0)  DISPLAY_state.TradeJailCardsDesired[0] = (1 << 0);
    else                DISPLAY_state.TradeJailCardsDesired[0] = (1 << 2);
  } else if (UICurrentGameState.Cards[CHANCE_DECK].jailOwner == (RULE_PlayerNumber)TradeBPlayerActive) {
    if (offered1 == 0)  DISPLAY_state.TradeJailCardsDesired[0] = (1 << 1);
    else                DISPLAY_state.TradeJailCardsDesired[0] = (1 << 3);
  }

  DISPLAY_state.TradeJailCardsDesired[1] = 0;
  if (UICurrentGameState.Cards[COMMUNITY_DECK].jailOwner == TradeAPlayerActive) {
    if (offered2 == 0)  DISPLAY_state.TradeJailCardsDesired[1] = (1 << 0);
    else                DISPLAY_state.TradeJailCardsDesired[1] = (1 << 2);
  } else if (UICurrentGameState.Cards[COMMUNITY_DECK].jailOwner == TradeBPlayerActive) {
    if (offered2 == 0)  DISPLAY_state.TradeJailCardsDesired[1] = (1 << 1);
    else                DISPLAY_state.TradeJailCardsDesired[1] = (1 << 3);
  }

  if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA) {
    if ((!proposed) && (!showpropose)) {
      LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_iytprpf), 202, 0, 0);//-106, 10);
      LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_iytcxlf), 201, 0, 0);//-106, 10);
      showpropose = TRUE;
    }


    for (int iTemp1 = 0; iTemp1 < RULE_MAX_COUNT_HIT_SETS; iTemp1++)
    {
      if (UICurrentGameState.CountHits[iTemp1].toPlayer == (RULE_PlayerNumber)TradeAPlayerActive) {
        if (UICurrentGameState.CountHits[iTemp1].hitType == CHT_FUTURE_RENT)
          DISPLAY_state.TradeImmunityFutureDesiredA[0] |= (1 << 0);
        else if (UICurrentGameState.CountHits[iTemp1].hitType == CHT_RENT_IMMUNITY)
          DISPLAY_state.TradeImmunityFutureDesiredA[1] |= (1 << 0);
      }
      if (UICurrentGameState.CountHits[iTemp1].toPlayer == (RULE_PlayerNumber)TradeBPlayerActive) {
        if (UICurrentGameState.CountHits[iTemp1].hitType == CHT_FUTURE_RENT)
          DISPLAY_state.TradeImmunityFutureDesiredA[0] |= (1 << 1);
        else if (UICurrentGameState.CountHits[iTemp1].hitType == CHT_RENT_IMMUNITY)
          DISPLAY_state.TradeImmunityFutureDesiredA[1] |= (1 << 1);
      }
    }

    if (TradeAPlayerActive != RULE_MAX_PLAYERS)
      DISPLAY_state.TradeCashBoxDesiredValue[0] = UICurrentGameState.Players[TradeAPlayerActive].cash -
        DISPLAY_state.TradeCashBoxDesiredValue[2] + DISPLAY_state.TradeCashBoxDesiredValue[3];
    else
      DISPLAY_state.TradeCashBoxDesiredValue[0] = 0;

    if (TradeBPlayerActive != RULE_MAX_PLAYERS)
      DISPLAY_state.TradeCashBoxDesiredValue[1] = UICurrentGameState.Players[TradeBPlayerActive].cash -
        DISPLAY_state.TradeCashBoxDesiredValue[3] + DISPLAY_state.TradeCashBoxDesiredValue[2];
    else
      DISPLAY_state.TradeCashBoxDesiredValue[1] = 0;

    if (desiredTradePanels == -1)
      desiredTradePanels = (TradeAPlayerActive * 10) + TradeBPlayerActive;


    if (proposed) {
//iytprf00  iytpri00  iytprp00
//                        LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_iytprpf), 202);
//      if (!LE_SEQNCR_IsSequenceFinished (LED_IFT(DAT_LANG2, CNK_iytcxlf), 201, FALSE)) {
      if (PlayerSelectDlg.isVisible)   UDTrade_RemovePlayerSelectDlg();
      if (CashTradeDlg.isVisible)      UDTrade_RemoveCashTradeDlg();
      if ((FutureTradeDlg.isVisible) && (FutureTradeDlg.mode != 6))      UDTrade_RemoveFutureTradeDlg();
      if ((ImmunityTradeDlg.isVisible) && (ImmunityTradeDlg.mode != 6))  UDTrade_RemoveImmunityTradeDlg();
      if (showpropose) {
        LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_iytprpf), 202);
        LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_iytprpo), 202, 0, 0);//-106, 10);
        LE_SEQNCR_SetEndingAction(LED_IFT(DAT_LANG2, CNK_iytprpo), 202, LE_SEQNCR_EndingActionStop);

        LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_iytcxlf), 201);
        LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_iytcxlo), 201, 0, 0);//-106, 10);
        LE_SEQNCR_SetEndingAction(LED_IFT(DAT_LANG2, CNK_iytcxlo), 201, LE_SEQNCR_EndingActionStop);
        showpropose = FALSE;
      }
    }

    if (desiredTradePanels != tradePanels) {
      if (tradePanels == -1) {
        if ((desiredTradePanels / 10) == RULE_MAX_PLAYERS) {
          leftBakaColour = RULE_MAX_PLAYERS;
          LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tnclrl06), 95, 1, 0);
        } else {
          leftBakaColour = UICurrentGameState.Players[desiredTradePanels / 10].colour;
          LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tnclrl00 + leftBakaColour), 95, 0, 0);
        }

        if ((desiredTradePanels % 10) == RULE_MAX_PLAYERS) {
          rightBakaColour = RULE_MAX_PLAYERS;
          LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tnclrr06), 95, 0, 0);
        } else {
          rightBakaColour = UICurrentGameState.Players[desiredTradePanels % 10].colour;
          LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tnclrr00 + rightBakaColour), 95, 0, 0);
        }

        LE_GRAFIX_ColorArea(NameA, 0, 0, 135, 35, LEG_MCR(0,255,0));
        LE_GRAFIX_ColorArea(NameB, 0, 0, 135, 35, LEG_MCR(0,255,0));
        LE_FONTS_SetSettings(0);
        LE_FONTS_SetSize(12);
        LE_FONTS_SetWeight(700);
        LE_FONTS_Print(NameA, 8, 9, LEG_MCR(220, 220, 220), UICurrentGameState.Players[TradeAPlayerActive].name);
        LE_FONTS_Print(NameB, 8, 9, LEG_MCR(220, 220, 220), UICurrentGameState.Players[TradeBPlayerActive].name);
        LE_FONTS_GetSettings(0);

        LE_SEQNCR_StartXY(leftpanel, 148, 0, 0);
        LE_SEQNCR_StartXY(rightpanel, 148, 600, 0);
        LE_SEQNCR_StartXY(NameA, 148, 56, 229);
        LE_SEQNCR_StartXY(NameB, 148, 604, 229);
        tradePanels = desiredTradePanels;
//        LE_SEQNCR_StartXY(tmpoverlay, 1111, 0, 0);

        LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tradebg), 80, 0, 0);
        LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tndispbd), DISPLAY_PropertyFoatingBlowUp - 3, 0, 0);
        LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tyttft00), 130, 1, 0);
        LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tnarrow0), 140, 0, 0);
        LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tydispll), 145, 0, 0);
        LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tydisplr), 145, 0, 0);
//        LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tngbrdr1), 145, 0, 0);
        if ((UICurrentGameState.GameOptions.futureRentTradingAllowed) ||
            (UICurrentGameState.GameOptions.immunitiesTradingAllowed)) {
          LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tycrtnew), 155, 0, 0);
          LE_SEQNCR_SetEndingAction(LED_IFT(DAT_LANG2, CNK_tycrtnew), 155, LE_SEQNCR_EndingActionLoopToBeginning);
        }
        if (UICurrentGameState.GameOptions.futureRentTradingAllowed) {
          LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tyfutres), 150, 0, 0);
          LE_SEQNCR_SetEndingAction(LED_IFT(DAT_LANG2, CNK_tyfutres), 150, LE_SEQNCR_EndingActionLoopToBeginning);
        }
        if (UICurrentGameState.GameOptions.immunitiesTradingAllowed) {
          LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tyimmty), 160, 0, 0);
          LE_SEQNCR_SetEndingAction(LED_IFT(DAT_LANG2, CNK_tyimmty), 160, LE_SEQNCR_EndingActionLoopToBeginning);
        }

        DISPLAY_state.TradeCashBoxValues[0] = CASHTRADEINIT;
        DISPLAY_state.TradeCashBoxValues[2] = CASHTRADEINIT;
        if (TradeAPlayerActive != RULE_MAX_PLAYERS)
          DISPLAY_state.TradeCashBoxDesiredValue[0] = UICurrentGameState.Players[TradeAPlayerActive].cash -
            DISPLAY_state.TradeCashBoxDesiredValue[2] + DISPLAY_state.TradeCashBoxDesiredValue[3];
        else
          DISPLAY_state.TradeCashBoxDesiredValue[0] = 0;
        //DISPLAY_state.TradeCashBoxDesiredValue[0] = 0;
        DISPLAY_state.TradeCashBoxDesiredValue[2] = 0;

//        if ((TradeBPlayerActive != -1) && (TradeBPlayerActive != RULE_MAX_PLAYERS)) {
          DISPLAY_state.TradeCashBoxValues[1] = CASHTRADEINIT;
          DISPLAY_state.TradeCashBoxValues[3] = CASHTRADEINIT;
          DISPLAY_state.TradeCashBoxDesiredValue[1] = 0;
          DISPLAY_state.TradeCashBoxDesiredValue[3] = 0;
//        }

//        DISPLAY_state.TradeCashBoxDesiredValue[0] =
//          UICurrentGameState.Players[TradeAPlayerActive].cash - DISPLAY_state.TradeCashBoxDesiredValue[2];
//        DISPLAY_state.TradeCashBoxDesiredValue[1] =
//          UICurrentGameState.Players[TradeBPlayerActive].cash - DISPLAY_state.TradeCashBoxDesiredValue[3];
        tradePanels = desiredTradePanels;
      } else {
//        if ((tradePanels / 10) == RULE_MAX_PLAYERS)
//          LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tnclrl06), 95);
//        else
          LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tnclrl00 + leftBakaColour), 95);

//        if ((tradePanels % 10) == RULE_MAX_PLAYERS)
//          LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tnclrr06), 95);
//        else
          LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tnclrr00 + rightBakaColour), 95);

        LE_SEQNCR_Stop(leftpanel, 148);
        LE_SEQNCR_Stop(rightpanel, 148);
        LE_SEQNCR_Stop(NameA, 148);
        LE_SEQNCR_Stop(NameB, 148);
        tradePanels = desiredTradePanels;
        LE_SEQNCR_StartXY(leftpanel, 148, 0, 0);
        LE_SEQNCR_StartXY(rightpanel, 148, 600, 0);
        LE_SEQNCR_StartXY(NameA, 148, 56, 229);
        LE_SEQNCR_StartXY(NameB, 148, 604, 229);

        if ((tradePanels / 10) == RULE_MAX_PLAYERS) {
          leftBakaColour = RULE_MAX_PLAYERS;
          LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tnclrl06), 95, 1, 0);
        } else {
          leftBakaColour = UICurrentGameState.Players[tradePanels / 10].colour;
          LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tnclrl00 + leftBakaColour), 95, 0, 0);
        }

        if ((tradePanels % 10) == RULE_MAX_PLAYERS) {
          rightBakaColour = RULE_MAX_PLAYERS;
          LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tnclrr06), 95, 0, 0);
        } else {
          rightBakaColour = UICurrentGameState.Players[tradePanels % 10].colour;
          LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_tnclrr00 + rightBakaColour), 95, 0, 0);
        }

        LE_GRAFIX_ColorArea(NameA, 0, 0, 135, 35, LEG_MCR(0,255,0));
        LE_GRAFIX_ColorArea(NameB, 0, 0, 135, 35, LEG_MCR(0,255,0));
        LE_FONTS_SetSettings(0);
        LE_FONTS_SetSize(12);
        LE_FONTS_SetWeight(700);
        LE_FONTS_Print(NameA, 8, 9, LEG_MCR(220, 220, 220), UICurrentGameState.Players[TradeAPlayerActive].name);
        LE_FONTS_Print(NameB, 8, 9, LEG_MCR(220, 220, 220), UICurrentGameState.Players[TradeBPlayerActive].name);
        LE_FONTS_GetSettings(0);
      }
    }
  } else {
    if (tradePanels != -1) {
//      if ((tradePanels / 10) == RULE_MAX_PLAYERS)
//        LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tnclrl06), 95);
//      else
        LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tnclrl00 + leftBakaColour), 95);

//      if ((tradePanels % 10) == RULE_MAX_PLAYERS)
//        LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tnclrr06), 95);
//      else
        LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tnclrr00 + rightBakaColour), 95);

      LE_SEQNCR_Stop(leftpanel, 148);
      LE_SEQNCR_Stop(rightpanel, 148);
      LE_SEQNCR_Stop(NameA, 148);
      LE_SEQNCR_Stop(NameB, 148);
//      LE_SEQNCR_Stop(tmpoverlay, 1111);

      LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tradebg), 80);
      LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tndispbd), DISPLAY_PropertyFoatingBlowUp - 3);
      LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tyttft00), 130);
      LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tnarrow0), 140);
      LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tydispll), 145);
      LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tydisplr), 145);
//      LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tngbrdr1), 145);
//      if ((UICurrentGameState.GameOptions.futureRentTradingAllowed) ||
//          (UICurrentGameState.GameOptions.immunitiesTradingAllowed))
      if (!LE_SEQNCR_IsSequenceFinished(LED_IFT(DAT_LANG2, CNK_tycrtnew), 155, FALSE))
        LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tycrtnew), 155);
//      if (UICurrentGameState.GameOptions.futureRentTradingAllowed)
      if (!LE_SEQNCR_IsSequenceFinished(LED_IFT(DAT_LANG2, CNK_tyfutres), 150, FALSE))
        LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tyfutres), 150);
//      if (UICurrentGameState.GameOptions.immunitiesTradingAllowed)
      if (!LE_SEQNCR_IsSequenceFinished(LED_IFT(DAT_LANG2, CNK_tyimmty), 160, FALSE))
        LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tyimmty), 160);

      if (movingcardcnt) {
        if (movingcard != LED_EI)
          LE_SEQNCR_Stop(movingcard, movingcardpri);
      }
      movingcard = LED_EI;
      movingcardcnt = moveflag = 0;

      if (!proposed) {
        if (showpropose) {
          LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_iytprpf), 202);
          LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_iytprpo), 202, 0, 0);//-106, 10);
          LE_SEQNCR_SetEndingAction(LED_IFT(DAT_LANG2, CNK_iytprpo), 202, LE_SEQNCR_EndingActionStop);

          LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_iytcxlf), 201);
          LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, CNK_iytcxlo), 201, 0, 0);//-106, 10);
          LE_SEQNCR_SetEndingAction(LED_IFT(DAT_LANG2, CNK_iytcxlo), 201, LE_SEQNCR_EndingActionStop);
          showpropose = FALSE;
        }
      }
//      LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tyifpri0), 201);

      desiredTradePanels = tradePanels;
      tradePanels = -1;
    }
  }

  // Update Scoreboxes
  if (!PlayerSelectDlg.isVisible) //newnewnew
    for (iTemp = 0; iTemp < TradeCashBoxes; iTemp ++)
    {
      //Show em
      int flag = 0;
      if (iTemp < 2) flag = 1;
      else if (DISPLAY_state.TradeCashBoxDesiredValue[iTemp] != 0) flag = 1;
      if (((iTemp == 1) || (iTemp == 3)) && (TradeBPlayerActive == RULE_MAX_PLAYERS)) flag = 0;

      if ((flag) && (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA))
      {
        if (DISPLAY_state.TradeCashBoxShown[iTemp])
        { // invalidate the box if we must
          if (DISPLAY_state.TradeCashBoxValues[iTemp] != DISPLAY_state.TradeCashBoxDesiredValue[iTemp])
            LE_SEQNCR_ForceRedraw(DISPLAY_state.TradeCashBoxObject[iTemp], DISPLAY_TradeBoxBItemsPriority);
        } else { // Fire it up
          DISPLAY_state.TradeCashBoxShown[iTemp] = TRUE;
          if ((iTemp == 0) || (iTemp == 1))
            LE_SEQNCR_StartXY(DISPLAY_state.TradeCashBoxObject[iTemp], DISPLAY_TradeBoxBItemsPriority,
              TradeBasicPlayerBoxes[iTemp][0] + 6+SR, TradeBasicPlayerBoxes[iTemp][1] + 152+SD);
          else if ((iTemp == 2) || (iTemp == 3))
            LE_SEQNCR_StartXY(DISPLAY_state.TradeCashBoxObject[iTemp], DISPLAY_TradeBoxBItemsPriority,
              TradeBasicPlayerBoxes[iTemp][0] + 6+SR, TradeBasicPlayerBoxes[iTemp][1] + 152+11-SU);
          LE_SEQNCR_StartXY(LED_IFT(DAT_MAIN, TAB_snpscsh1), 100+iTemp, cashIconLoc[iTemp][0], cashIconLoc[iTemp][1]);
          DISPLAY_state.TradeCashBoxValues[iTemp] = -1;
        }
      } else { // Off
        if (DISPLAY_state.TradeCashBoxShown[iTemp])
        {
          DISPLAY_state.TradeCashBoxShown[iTemp] = FALSE;
          LE_SEQNCR_Stop(DISPLAY_state.TradeCashBoxObject[iTemp], DISPLAY_TradeBoxBItemsPriority);
          LE_SEQNCR_Stop(LED_IFT(DAT_MAIN, TAB_snpscsh1), 100+iTemp);
        }
      }

//LE_SEQNCR_ProcessUserCommands();
    if (DISPLAY_state.TradeCashBoxValues[iTemp] != DISPLAY_state.TradeCashBoxDesiredValue[iTemp])
    {
      LE_GRAFIX_ColorArea(DISPLAY_state.TradeCashBoxObject[iTemp], 0, 0,
        LE_GRAFIX_ReturnObjectWidth(DISPLAY_state.TradeCashBoxObject[iTemp]),
        LE_GRAFIX_ReturnObjectHeight(DISPLAY_state.TradeCashBoxObject[iTemp]),
        LEG_MCR(0, 255, 0)); // Clear it
//      if (!LE_SEQNCR_IsSequenceFinished (LED_IFT(DAT_MAIN, TAB_snpscsh1), 100+iTemp, FALSE))
      if (DISPLAY_state.TradeCashBoxShown[iTemp])
      {
//        Asprintf(myBuffer, A_T("%i"), DISPLAY_state.TradeCashBoxDesiredValue[iTemp]); //$
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(
          DISPLAY_state.TradeCashBoxDesiredValue[iTemp], DISPLAY_state.system, myBuffer, TRUE);
        LE_FONTS_SetSettings(0);
        LE_FONTS_GetSettings(8);  // Same as chat title bar font
        LE_FONTS_Print(DISPLAY_state.TradeCashBoxObject[iTemp], (50 - LE_FONTS_GetStringWidth(myBuffer)) / 2, 0,
          LEG_MCR(255, 255, 255), myBuffer);
        LE_FONTS_GetSettings(0);
        LE_SEQNCR_ForceRedraw(DISPLAY_state.TradeCashBoxObject[iTemp], DISPLAY_TradeBoxBItemsPriority);
        DISPLAY_state.TradeCashBoxValues[iTemp] = DISPLAY_state.TradeCashBoxDesiredValue[iTemp];
      }
      LE_GRAFIX_InvalidateRect(CashRects[iTemp]);  //bleeble blah
    }
//    DISPLAY_state.TradeCashBoxValues[iTemp] = DISPLAY_state.TradeCashBoxDesiredValue[iTemp];
  }




  // Properties owned before trade
  DesiredAStuff = DesiredAStuffMort = DesiredBStuff = DesiredBStuffMort = 0;
  if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA)
  {
    for (iTemp = 0; iTemp < SQ_MAX_SQUARE_TYPES; iTemp++)
    {
      // Construct players standard property set
      if (TradeAPlayerActive == UICurrentGameState.Squares[iTemp].owner)
      { // owned by trader A
        if (!((movingcardcnt) && (iTemp == movingcard0))) {
          if (UICurrentGameState.Squares[iTemp].mortgaged)
            DesiredAStuffMort |= RULE_PropertyToBitSet(iTemp);
          else
            DesiredAStuff     |= RULE_PropertyToBitSet(iTemp);
        }
      }
      if (TradeBPlayerActive == UICurrentGameState.Squares[iTemp].owner)
      { // owned by trader B
        if (!((movingcardcnt) && (iTemp == movingcard0))) {
          if (UICurrentGameState.Squares[iTemp].mortgaged)
            DesiredBStuffMort |= RULE_PropertyToBitSet(iTemp);
          else
            DesiredBStuff     |= RULE_PropertyToBitSet(iTemp);
        }
      }
    }
  }

//**********************************************************************************
//////////////////////////////////////////////////////////////////////////
// Property Item sets traded, left & right side.
// Things to remember:  numberC holds trade item type (TIK_CASH, TIK_SQUARE, TIK_JAIL_CARD, TIK_IMMUNITY or  TIK_FUTURE_RENT
// numberA is the from player, numberB is the to player and can be RULE_NOBODY_PLAYER if an item is being deleted.
// numberD is an amount or property, numberE is a RULE_PropertySet for immunities.
  DesiredTradePropsA = DesiredTradePropsAMort = DesiredTradePropsB = DesiredTradePropsBMort = 0;
//ick  DISPLAY_state.TradeImmunityFutureTopA = DISPLAY_state.TradeImmunityFutureTopB = 0;

//blah  memset(DISPLAY_state.TradeImmunityFutureDesiredA, LED_EI, RULE_MAX_COUNT_HIT_SETS * sizeof(LE_DATA_DataId));
//blah  memset(DISPLAY_state.TradeImmunityFutureDesiredB, LED_EI, RULE_MAX_COUNT_HIT_SETS * sizeof(LE_DATA_DataId));
  if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA)
  { // Lots of storage required - this 'parses' the trade, all desired options must be saved in the state.
    // OPTIMIZE - parsing every frame.  This could be a sub under the Add_Item message, reset and others?
    if (UICurrentGameState.Cards[CHANCE_DECK].jailOwner == TradeAPlayerActive)
      DISPLAY_state.TradeJailCardsDesired[0] = (1 << 0);
    else if (UICurrentGameState.Cards[CHANCE_DECK].jailOwner == TradeBPlayerActive)
      DISPLAY_state.TradeJailCardsDesired[0] = (1 << 1);

    if (UICurrentGameState.Cards[COMMUNITY_DECK].jailOwner == TradeAPlayerActive)
      DISPLAY_state.TradeJailCardsDesired[1] = (1 << 0);
    else if (UICurrentGameState.Cards[COMMUNITY_DECK].jailOwner == TradeBPlayerActive)
      DISPLAY_state.TradeJailCardsDesired[1] = (1 << 1);

    for (iTemp = 0; iTemp < TradeItemListTop; iTemp++)
    {
      switch(UDTradeItemList[iTemp].numberC)
      {
      case TIK_CASH:
        if (UDTradeItemList[iTemp].numberA == TradeAPlayerActive) {
          DISPLAY_state.TradeCashBoxDesiredValue[2] = UDTradeItemList[iTemp].numberD;
          DISPLAY_state.TradeCashBoxDesiredValue[3] = 0;
        } else if (UDTradeItemList[iTemp].numberA == TradeBPlayerActive) {
          DISPLAY_state.TradeCashBoxDesiredValue[3] = UDTradeItemList[iTemp].numberD;
          DISPLAY_state.TradeCashBoxDesiredValue[2] = 0;
        }
        DISPLAY_state.TradeCashBoxDesiredValue[0] =
          UICurrentGameState.Players[TradeAPlayerActive].cash - DISPLAY_state.TradeCashBoxDesiredValue[2];
        DISPLAY_state.TradeCashBoxDesiredValue[1] =
          UICurrentGameState.Players[TradeBPlayerActive].cash - DISPLAY_state.TradeCashBoxDesiredValue[3];
        break;
      case TIK_SQUARE:
        if (UDTradeItemList[iTemp].numberA == TradeAPlayerActive)
          if (!((movingcardcnt) && (UDTradeItemList[iTemp].numberD == movingcard0))) {
            if (UICurrentGameState.Squares[UDTradeItemList[iTemp].numberD].mortgaged)
              DesiredTradePropsAMort |= RULE_PropertyToBitSet(UDTradeItemList[iTemp].numberD);
            else
              DesiredTradePropsA |= RULE_PropertyToBitSet(UDTradeItemList[iTemp].numberD);
          } else
            moveflag = 2;
        if (UDTradeItemList[iTemp].numberA == TradeBPlayerActive)
          if (!((movingcardcnt) && (UDTradeItemList[iTemp].numberD == movingcard0))) {
            if (UICurrentGameState.Squares[UDTradeItemList[iTemp].numberD].mortgaged)
              DesiredTradePropsBMort |= RULE_PropertyToBitSet(UDTradeItemList[iTemp].numberD);
            else
              DesiredTradePropsB |= RULE_PropertyToBitSet(UDTradeItemList[iTemp].numberD);
          } else
            moveflag = 1;
        break;
      case TIK_JAIL_CARD:
        if ((UICurrentGameState.Cards[CHANCE_DECK].jailOwner == TradeAPlayerActive) && (UDTradeItemList[iTemp].numberD == CHANCE_DECK))
          DISPLAY_state.TradeJailCardsDesired[0] = (1 << 2);
        else if ((UICurrentGameState.Cards[CHANCE_DECK].jailOwner == TradeBPlayerActive) && (UDTradeItemList[iTemp].numberD == CHANCE_DECK))
          DISPLAY_state.TradeJailCardsDesired[0] = (1 << 3);

        if ((UICurrentGameState.Cards[COMMUNITY_DECK].jailOwner == TradeAPlayerActive) && (UDTradeItemList[iTemp].numberD == COMMUNITY_DECK))
          DISPLAY_state.TradeJailCardsDesired[1] = (1 << 2);
        else if ((UICurrentGameState.Cards[COMMUNITY_DECK].jailOwner == TradeBPlayerActive) && (UDTradeItemList[iTemp].numberD == COMMUNITY_DECK))
          DISPLAY_state.TradeJailCardsDesired[1] = (1 << 3);

//        if (UDTradeItemList[iTemp].numberA == TradeAPlayerActive)
//          DISPLAY_state.TradeJailCardsDesired[0] = LED_IFT(DAT_MAIN, TAB_ibjlcdf0);
//        if (UDTradeItemList[iTemp].numberA == TradeBPlayerActive)
//          DISPLAY_state.TradeJailCardsDesired[1] = LED_IFT(DAT_MAIN, TAB_ibjlcdf1);
        break;
      case TIK_IMMUNITY:
// Handled at the end of the file
/*
        if ((UDTradeItemList[iTemp].numberA == TradeAPlayerActive) && (DISPLAY_state.TradeImmunityFutureTopA < RULE_MAX_COUNT_HIT_SETS))
        {
          DISPLAY_state.TradeImmunityNotFutureA  [DISPLAY_state.TradeImmunityFutureTopA] = TRUE;
          DISPLAY_state.TradeImmunityFutureDesiredA[DISPLAY_state.TradeImmunityFutureTopA] = LED_IFT(DAT_MAIN, TAB_hotelb);
          DISPLAY_state.TradeImmunityFuturePropsA[DISPLAY_state.TradeImmunityFutureTopA]   = UDTradeItemList[iTemp].numberD;
          DISPLAY_state.TradeImmunityFutureHitsA [DISPLAY_state.TradeImmunityFutureTopA++]  = UDTradeItemList[iTemp].numberE;
        }
        if ((UDTradeItemList[iTemp].numberA == TradeBPlayerActive) && (DISPLAY_state.TradeImmunityFutureTopB < RULE_MAX_COUNT_HIT_SETS))
        {
          DISPLAY_state.TradeImmunityNotFutureB  [DISPLAY_state.TradeImmunityFutureTopB] = TRUE;
          DISPLAY_state.TradeImmunityFutureDesiredB[DISPLAY_state.TradeImmunityFutureTopB] = LED_IFT(DAT_MAIN, TAB_hotelb);
          DISPLAY_state.TradeImmunityFuturePropsB[DISPLAY_state.TradeImmunityFutureTopB]   = UDTradeItemList[iTemp].numberD;
          DISPLAY_state.TradeImmunityFutureHitsB [DISPLAY_state.TradeImmunityFutureTopB++]  = UDTradeItemList[iTemp].numberE;
        }
*/
        break;
      case TIK_FUTURE_RENT:
// Handled at the end of the file
/*
        if ((UDTradeItemList[iTemp].numberA == TradeAPlayerActive) && (DISPLAY_state.TradeImmunityFutureTopA < RULE_MAX_COUNT_HIT_SETS))
        {
          DISPLAY_state.TradeImmunityNotFutureA  [DISPLAY_state.TradeImmunityFutureTopA] = FALSE;
          DISPLAY_state.TradeImmunityFutureDesiredA[DISPLAY_state.TradeImmunityFutureTopA] = LED_IFT(DAT_MAIN, TAB_housea);
          DISPLAY_state.TradeImmunityFuturePropsA[DISPLAY_state.TradeImmunityFutureTopA]   = UDTradeItemList[iTemp].numberD;
          DISPLAY_state.TradeImmunityFutureHitsA [DISPLAY_state.TradeImmunityFutureTopA++]  = UDTradeItemList[iTemp].numberE;
        }
        if ((UDTradeItemList[iTemp].numberA == TradeBPlayerActive) && (DISPLAY_state.TradeImmunityFutureTopB < RULE_MAX_COUNT_HIT_SETS))
        {
          DISPLAY_state.TradeImmunityNotFutureB  [DISPLAY_state.TradeImmunityFutureTopB] = FALSE;
          DISPLAY_state.TradeImmunityFutureDesiredB[DISPLAY_state.TradeImmunityFutureTopB] = LED_IFT(DAT_MAIN, TAB_housea);
          DISPLAY_state.TradeImmunityFuturePropsB[DISPLAY_state.TradeImmunityFutureTopB]   = UDTradeItemList[iTemp].numberD;
          DISPLAY_state.TradeImmunityFutureHitsB [DISPLAY_state.TradeImmunityFutureTopB++]  = UDTradeItemList[iTemp].numberE;
        }
*/
        break;
      default:
        break;
      }
    }
  }




  DesiredAStuff &= ~(DesiredTradePropsA);
  DesiredAStuffMort &= ~(DesiredTradePropsAMort);
  DesiredBStuff &= ~(DesiredTradePropsB);
  DesiredBStuffMort &= ~(DesiredTradePropsBMort);
//**********************************************************************************

  deed = UDTrade_MouseinProp(LE_MOUSE_MouseX, LE_MOUSE_MouseY);
  if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA) {
    DISPLAY_state.IBarTradePropertyCurrentMouseOver = -1;
    // Deed rollover in trade while cashbox open is annoying
    if ((deed >= 0) && (!CashTradeDlg.isVisible)) {
      box = deed / 100;
      deed %= 100;
      DISPLAY_state.IBarTradePropertyCurrentMouseOver = deed + (box*100) +
        (1000 * ((!FutureTradeDlg.isVisible) && (!ImmunityTradeDlg.isVisible)));
    }
  }

//////////////////////////////////////////////

if ((DesiredAStuff != TradeAStuff) ||
    (DesiredBStuff != TradeBStuff) ||
    (DesiredAStuffMort != TradeAStuffMort) ||
    (DesiredBStuffMort != TradeBStuffMort)) {
  //// modified by David Ethier on April 20 ////
  if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA)
  {
    UDTrade_ResetPropertyPositions(&DesiredBeforePropertyA);
    UDTrade_ResetPropertyPositions(&DesiredBeforePropertyB);

    // determine the number of columns of properties owned for each player
    UDTrade_CountColumns(DesiredAStuff, DesiredAStuffMort, &DesiredBeforePropertyA);
    UDTrade_CountColumns(DesiredBStuff, DesiredBStuffMort, &DesiredBeforePropertyB);
  }

  // Affect properties owned graphics
  for (iTemp = 0; iTemp < SQ_MAX_SQUARE_TYPES; iTemp++)
  { // Take corrective action to change the display to the current property sets
    if ((lTemp = RULE_PropertyToBitSet(iTemp)) == 0)
      continue;
    t = DISPLAY_propertyToOwnablePropertyConversion(iTemp);
#if USA_VERSION
    tempID = LED_IFT(DAT_PAT, TAB_trprfp0000) + t; //we will assume this is always valid if used.
#else
    if(iLangId == NOTW_LANG_FRENCH)
        tempID = LED_IFT(DAT_PAT, TAB_xrprfp0000) + t;
    else
        tempID = LED_IFT(DAT_PAT, TAB_zrprfp0000) + t;
#endif


    order = IBARPropertyBarOrder[t]; // Obtain display order in tray.
    if ((order % 3) == 0) order += 2;
    else if (((order - 2) % 3) == 0) order -= 2;
    order = 32 - order;

    // We know what we want, lets make it so
    if ((DesiredAStuff & lTemp) != (TradeAStuff & lTemp))
    { // Need corrective action
      if (DesiredAStuff & lTemp)
      { // Bring up new property
        ////modified by David Ethier on april 20////
#if USA_VERSION
        DesiredBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfp0000) + t; //we will assume this is always valid if used.;
#else
        if(iLangId == NOTW_LANG_FRENCH)
            DesiredBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfp0000) + t;
        else
            DesiredBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfp0000) + t;
#endif
        DesiredBeforePropertyA.mortgaged[iTemp] = FALSE;
        DesiredBeforePropertyA.state = CHANGED;
        TradeAStuff |= lTemp;
      } else { // Shut down old property
        LE_SEQNCR_Stop(DesiredBeforePropertyA.ID[iTemp], DISPLAY_TradeBoxAStuffPriority + (UNS16)order);
        DesiredBeforePropertyA.ID[iTemp] = LED_EI;
        DesiredBeforePropertyA.state = CHANGED;
        TradeAStuff &= ~lTemp;
      }
/////////////////////////////////////////////////////////////////////////////
    }
    if ((DesiredBStuff & lTemp) != (TradeBStuff & lTemp))
    { // Need corrective action
      if (DesiredBStuff & lTemp)
      { // Bring up new property
        //// modified by David Ethier on April 20 ////
#if USA_VERSION
        DesiredBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfp0000) + t; //we will assume this is always valid if used.;
#else
        if(iLangId == NOTW_LANG_FRENCH)
            DesiredBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfp0000) + t;
        else
            DesiredBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfp0000) + t;
#endif
        DesiredBeforePropertyB.mortgaged[iTemp] = FALSE;
        DesiredBeforePropertyB.state = CHANGED;
        TradeBStuff |= lTemp;
      } else { // Shut down old property
        LE_SEQNCR_Stop(DesiredBeforePropertyB.ID[iTemp], DISPLAY_TradeBoxBStuffPriority + (UNS16)order);
        DesiredBeforePropertyB.ID[iTemp] = LED_EI;
        DesiredBeforePropertyB.state = CHANGED;
        TradeBStuff &= ~lTemp;
      }
///////////////////////////////////////////////////////////////////////////
    }

    if ((DesiredAStuffMort & lTemp) != (TradeAStuffMort & lTemp))
    { // Need corrective action
      if (DesiredAStuffMort & lTemp)
      { // Bring up new property
        //// modified by David Ethier on April 20 ////
#if USA_VERSION
        DesiredBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfm0000) + t; // Mortgaged stuff
#else
        if(iLangId == NOTW_LANG_FRENCH)
            DesiredBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfm0000) + t;
        else
            DesiredBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfm0000) + t;
#endif
        DesiredBeforePropertyA.mortgaged[iTemp] = TRUE;
        DesiredBeforePropertyA.state = CHANGED;
        TradeAStuffMort |= lTemp;
      } else { // Shut down old property
#if USA_VERSION
        LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_trprfm0000) + t, DISPLAY_TradeBoxAStuffPriority + (UNS16)order);
#else
        if(iLangId == NOTW_LANG_FRENCH)
            LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_xrprfm0000) + t, DISPLAY_TradeBoxAStuffPriority + (UNS16)order);
        else
            LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_zrprfm0000) + t, DISPLAY_TradeBoxAStuffPriority + (UNS16)order);
#endif
        if (DesiredAStuff & lTemp)
        {
#if USA_VERSION
            DesiredBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfp0000) + t;
#else
            if(iLangId == NOTW_LANG_FRENCH)
                DesiredBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfp0000) + t;
            else
                DesiredBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfp0000) + t;
#endif

        }
        else
            DesiredBeforePropertyA.ID[iTemp] = LED_EI;
        DesiredBeforePropertyA.state = CHANGED;
        TradeAStuffMort &= ~lTemp;
      }
///////////////////////////////////////////////////////////////////////////
    }
    if ((DesiredBStuffMort & lTemp) != (TradeBStuffMort & lTemp))
    { // Need corrective action
      if (DesiredBStuffMort & lTemp)
      { // Bring up new property
        //// modified by David Ethier on April 20 ////
#if USA_VERSION
        DesiredBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfm0000) + t; // Mortgaged stuff
#else
        if(iLangId == NOTW_LANG_FRENCH)
            DesiredBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfm0000) + t;
        else
            DesiredBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfm0000) + t;
#endif
        DesiredBeforePropertyB.mortgaged[iTemp] = TRUE;
        DesiredBeforePropertyB.state = CHANGED;
        TradeBStuffMort |= lTemp;
      } else { // Shut down old property
#if USA_VERSION
        LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_trprfm0000) + t, DISPLAY_TradeBoxBStuffPriority + (UNS16)order);
#else
        if(iLangId == NOTW_LANG_FRENCH)
            LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_xrprfm0000) + t, DISPLAY_TradeBoxBStuffPriority + (UNS16)order);
        else
            LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_zrprfm0000) + t, DISPLAY_TradeBoxBStuffPriority + (UNS16)order);
#endif
        if (DesiredBStuff & lTemp)
        {
#if USA_VERSION
            DesiredBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfp0000) + t;
#else
            if(iLangId == NOTW_LANG_FRENCH)
                DesiredBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfp0000) + t;
            else
                DesiredBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfp0000) + t;
#endif
        }
        else 
            DesiredBeforePropertyB.ID[iTemp] = LED_EI;

        DesiredBeforePropertyB.state = CHANGED;
        TradeBStuffMort &= ~lTemp;
      }
//////////////////////////////////////////////////////////////////////////
    }
  } // Done displaying properties.
}

  //// added by David Ethier on April 20 ////
  if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA)
  {
    // Reposition, if necessary, all properties and display
    if (DesiredBeforePropertyA.state)
      UDTrade_RepositionProperties(&DesiredBeforePropertyA, 0, 0, 0);
    if (DesiredBeforePropertyB.state)
      UDTrade_RepositionProperties(&DesiredBeforePropertyB, 0, 0, 1);
  }

//////////////////////////////////////////////////////////////////////////

if ((DesiredTradePropsA != TradePropsA) ||
    (DesiredTradePropsB != TradePropsB) ||
    (DesiredTradePropsAMort != TradePropsAMort) ||
    (DesiredTradePropsBMort != TradePropsBMort)) {
  //// modified by David Ethier on April 20 ////
  if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA)
  {
    UDTrade_ResetPropertyPositions(&TradeBeforePropertyA);
    UDTrade_ResetPropertyPositions(&TradeBeforePropertyB);

    // determine the number of columns of properties owned for each player
    UDTrade_CountColumns(DesiredTradePropsA, DesiredTradePropsAMort, &TradeBeforePropertyA);
    UDTrade_CountColumns(DesiredTradePropsB, DesiredTradePropsBMort, &TradeBeforePropertyB);
  }

  // Affect item property sets code - using basic coords for trade items for now.
  for (iTemp = 0; iTemp < SQ_MAX_SQUARE_TYPES; iTemp++)
  { // Take corrective action to change the display to the current property sets
    if ((lTemp = RULE_PropertyToBitSet(iTemp)) == 0)
      continue;
    t = DISPLAY_propertyToOwnablePropertyConversion(iTemp);
//    tempID = LED_IFT(DAT_PAT, TAB_trprfp0000) + t;

    order = IBARPropertyBarOrder[t]; // Obtain display order in tray.
    if ((order % 3) == 0) order += 2;
    else if (((order - 2) % 3) == 0) order -= 2;
    order = 32 - order;

    // We know what we want, lets make it so
    if ((DesiredTradePropsA & lTemp) != (TradePropsA & lTemp))
    { // Need corrective action
      if (DesiredTradePropsA & lTemp)
      { // Bring up new property
        //// modified by David Ethier on april 20 ////
//DesiredBeforePropertyA.ID[iTemp] = LED_IFT(DAT_MAIN, TAB_indsstg00) + t; //we will assume this is always valid if used.;
#if USA_VERSION
        TradeBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfp0000) + t; //we will assume this is always valid if used.;
#else
        if(iLangId == NOTW_LANG_FRENCH)
            TradeBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfp0000) + t;
        else
            TradeBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfp0000) + t;
#endif
        TradeBeforePropertyA.mortgaged[iTemp] = FALSE;
        TradeBeforePropertyA.state = CHANGED;
        TradePropsA |= lTemp;
      } else { // Shut down old property
        movingcard = TradeBeforePropertyA.ID[iTemp];//jkl
        LE_SEQNCR_Stop(TradeBeforePropertyA.ID[iTemp], DISPLAY_TradeBoxAItemsPriority + (UNS16)order);
        TradeBeforePropertyA.ID[iTemp] = LED_EI;
        TradeBeforePropertyA.state = CHANGED;
        TradePropsA &= ~lTemp;
      }
///////////////////////////////////////////////////////////////////////////
    }
    if ((DesiredTradePropsB & lTemp) != (TradePropsB & lTemp))
    { // Need corrective action
      if (DesiredTradePropsB & lTemp)
      { // Bring up new property
        //// modified by David Ethier on april 20 ////
#if USA_VERSION
        TradeBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfp0000) + t; //we will assume this is always valid if used.;
#else
        if(iLangId == NOTW_LANG_FRENCH)
            TradeBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfp0000) + t;
        else
            TradeBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfp0000) + t;
#endif
        TradeBeforePropertyB.mortgaged[iTemp] = FALSE;
        TradeBeforePropertyB.state = CHANGED;
        TradePropsB |= lTemp;
      } else { // Shut down old property
        movingcard = TradeBeforePropertyB.ID[iTemp];//jkl
        LE_SEQNCR_Stop(TradeBeforePropertyB.ID[iTemp], DISPLAY_TradeBoxBItemsPriority + (UNS16)order);
        TradeBeforePropertyB.ID[iTemp] = LED_EI;
        TradeBeforePropertyB.state = CHANGED;
        TradePropsB &= ~lTemp;
      }
/////////////////////////////////////////////////////////////////////////////
    }
    //Do the mortgaged ones
//    tempID = LED_IFT(DAT_PAT, TAB_trprfm0000) + t;
    if ((DesiredTradePropsAMort & lTemp) != (TradePropsAMort & lTemp))
    { // Need corrective action
      if (DesiredTradePropsAMort & lTemp)
      { // Bring up new property
        //// modified by David Ethier on april 20 ////
#if USA_VERSION
        TradeBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfm0000) + t;
#else
        if(iLangId == NOTW_LANG_FRENCH)
            TradeBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfm0000) + t;
        else
            TradeBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfm0000) + t;
#endif
        TradeBeforePropertyA.mortgaged[iTemp] = TRUE;
        TradeBeforePropertyA.state = CHANGED;
        TradePropsAMort |= lTemp;
      } else { // Shut down old property
//        LE_SEQNCR_Stop(TradeBeforePropertyA.ID[iTemp], DISPLAY_TradeBoxAItemsPriority + (UNS16)order);
#if USA_VERSION
        LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_trprfm0000) + t, DISPLAY_TradeBoxAItemsPriority + (UNS16)order);
#else
        if(iLangId == NOTW_LANG_FRENCH)
            LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_xrprfm0000) + t, DISPLAY_TradeBoxAItemsPriority + (UNS16)order);
        else
            LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_zrprfm0000) + t, DISPLAY_TradeBoxAItemsPriority + (UNS16)order);
#endif
        if (DesiredTradePropsA & lTemp)
        {
#if USA_VERSION
            TradeBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfp0000) + t;
#else
            if(iLangId == NOTW_LANG_FRENCH)
                TradeBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfp0000) + t;
            else
                TradeBeforePropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfp0000) + t;
#endif
        }
        else {
          movingcard = TradeBeforePropertyA.ID[iTemp];//jkl
          TradeBeforePropertyA.ID[iTemp] = LED_EI;
        }
        TradeBeforePropertyA.state = CHANGED;
        TradePropsAMort &= ~lTemp;
      }
//////////////////////////////////////////////////////////////////////////
    }
    if ((DesiredTradePropsBMort & lTemp) != (TradePropsBMort & lTemp))
    { // Need corrective action
      if (DesiredTradePropsBMort & lTemp)
      { // Bring up new property
        //// Modified by David Ethier on april 20 ////
#if USA_VERSION
        TradeBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfm0000) + t;
#else
            if(iLangId == NOTW_LANG_FRENCH)
                TradeBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfm0000) + t;
            else
                TradeBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfm0000) + t;
#endif
        TradeBeforePropertyB.mortgaged[iTemp] = TRUE;
        TradeBeforePropertyB.state = CHANGED;
        TradePropsBMort |= lTemp;
      } else { // Shut down old property
//        LE_SEQNCR_Stop(TradeBeforePropertyB.ID[iTemp], DISPLAY_TradeBoxBItemsPriority + (UNS16)order);
#if USA_VERSION
        LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_trprfm0000) + t, DISPLAY_TradeBoxBItemsPriority + (UNS16)order);
#else
            if(iLangId == NOTW_LANG_FRENCH)
                LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_xrprfm0000) + t, DISPLAY_TradeBoxBItemsPriority + (UNS16)order);
            else
                LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_zrprfm0000) + t, DISPLAY_TradeBoxBItemsPriority + (UNS16)order);
#endif

        if (DesiredTradePropsB & lTemp)
        {
#if USA_VERSION
            TradeBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfp0000) + t;
#else
            if(iLangId == NOTW_LANG_FRENCH)
                TradeBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfp0000) + t;
            else
                TradeBeforePropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfp0000) + t;
#endif
        }
        else {
          movingcard = TradeBeforePropertyB.ID[iTemp];//jkl
          TradeBeforePropertyB.ID[iTemp] = LED_EI;
        }
        TradeBeforePropertyB.state = CHANGED;
        TradePropsBMort &= ~lTemp;
      }
///////////////////////////////////////////////////////////////////////////
    }
  } // Done displaying properties.
}

  ////  added by David Ethier on April 20 ////
  // Reposition, if necessary, all properties and display
  if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA)
  {
    if (TradeBeforePropertyA.state)
      UDTrade_RepositionProperties(&TradeBeforePropertyA, 0, 0, 2);
    if (TradeBeforePropertyB.state)
      UDTrade_RepositionProperties(&TradeBeforePropertyB, 0, 0, 3);
  }
//////////////////////////////////////////////////////////////////////////

  // Properties owned after trade, should it be accepted
  DesiredAAfter     = (DesiredAStuff | TradePropsB) & ~TradePropsA;
  DesiredAAfterMort = (DesiredAStuffMort | TradePropsBMort) & ~TradePropsAMort;
  DesiredBAfter     = (DesiredBStuff | TradePropsA) & ~TradePropsB;
  DesiredBAfterMort = (DesiredBStuffMort | TradePropsAMort) & ~TradePropsBMort;

if ((DesiredAAfter != TradeAAfter) ||
    (DesiredBAfter != TradeBAfter) ||
    (DesiredAAfterMort != TradeAAfterMort) ||
    (DesiredBAfterMort != TradeBAfterMort)) {
  //// modified by David Ethier on April 20 ////
  if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA)
  {
    UDTrade_ResetPropertyPositions(&DesiredAfterPropertyA);
    UDTrade_ResetPropertyPositions(&DesiredAfterPropertyB);

    // determine the number of columns of properties owned for each player
    UDTrade_CountColumns(DesiredAAfter, DesiredAAfterMort, &DesiredAfterPropertyA);
    UDTrade_CountColumns(DesiredBAfter, DesiredBAfterMort, &DesiredAfterPropertyB);
  }
/////////////////////////////////////////////////////////////////////////////

  // Affect properties owned after graphics
  for (iTemp = 0; iTemp < SQ_MAX_SQUARE_TYPES; iTemp++)
  { // Take corrective action to change the display to the current property sets
    if ((lTemp = RULE_PropertyToBitSet(iTemp)) == 0)
      continue;
    t = DISPLAY_propertyToOwnablePropertyConversion(iTemp);
//    tempID = LED_IFT(DAT_PAT, TAB_trprfp0000) + t; //we will assume this is always valid if used.
    order = IBARPropertyBarOrder[t]; // Obtain display order in tray.
    if ((order % 3) == 0) order += 2;
    else if (((order - 2) % 3) == 0) order -= 2;
    order = 32 - order;

    // We know what we want, lets make it so
    if ((DesiredAAfter & lTemp) != (TradeAAfter & lTemp))
    { // Need corrective action
      if (DesiredAAfter & lTemp)
      { // Bring up new property
        //// modified by David Ethier on april 20 ////
        if (movingcardcnt > 0) moveflag = 1;
#if USA_VERSION
        DesiredAfterPropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfp0000) + t; //we will assume this is always valid if used.
#else
        if(iLangId == NOTW_LANG_FRENCH)
            DesiredAfterPropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfp0000) + t;
        else
            DesiredAfterPropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfp0000) + t;
#endif
//////        DesiredAfterPropertyA.mortgaged[iTemp] = FALSE;
//////        DesiredAfterPropertyA.state    = CHANGED;
        TradeAAfter |= lTemp;
      } else { // Shut down old property
        movingcard = DesiredAfterPropertyA.ID[iTemp];//jkl
        if (movingcardcnt < 0) moveflag = 1;
        LE_SEQNCR_Stop(DesiredAfterPropertyA.ID[iTemp], DISPLAY_TradeBoxADoneSPriority + (UNS16)order);
//        if (DesiredAAfter & lTemp) DesiredAfterPropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfp0000) + t;
//        else {
        DesiredAfterPropertyA.ID[iTemp] = LED_EI;
//        }
//////        DesiredAfterPropertyA.state = CHANGED;
        TradeAAfter &= ~lTemp;
      }
/////////////////////////////////////////////////////////////////////////
    }
    if ((DesiredBAfter & lTemp) != (TradeBAfter & lTemp))
    { // Need corrective action
      if (DesiredBAfter & lTemp)
      { // Bring up new property
        //// modified by David Ethier on april 20 ////
        if (movingcardcnt > 0) moveflag = 2;
#if USA_VERSION
        DesiredAfterPropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfp0000) + t; //we will assume this is always valid if used.
#else
        if(iLangId == NOTW_LANG_FRENCH)
            DesiredAfterPropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfp0000) + t;
        else
            DesiredAfterPropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfp0000) + t;
#endif
//////        DesiredAfterPropertyB.mortgaged[iTemp] = FALSE;
//////        DesiredAfterPropertyB.state    = CHANGED;
        TradeBAfter |= lTemp;
      } else { // Shut down old property
        movingcard = DesiredAfterPropertyB.ID[iTemp];//jkl
        if (movingcardcnt < 0) moveflag = 2;
        LE_SEQNCR_Stop(DesiredAfterPropertyB.ID[iTemp], DISPLAY_TradeBoxBDoneSPriority + (UNS16)order);
        DesiredAfterPropertyB.ID[iTemp] = LED_EI;
//////        DesiredAfterPropertyB.state    = CHANGED;
        TradeBAfter &= ~lTemp;
      }
//////////////////////////////////////////////////////////////////////////
    }

//    tempID = LED_IFT(DAT_PAT, TAB_trprfm0000) + t; // Mortgaged stuff
    if ((DesiredAAfterMort & lTemp) != (TradeAAfterMort & lTemp))
    { // Need corrective action
      if (DesiredAAfterMort & lTemp)
      { // Bring up new property
        //// modified by David Ethier on april 20 ////
        if (movingcardcnt > 0) moveflag = 1;
#if USA_VERSION
        DesiredAfterPropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfm0000) + t;
#else
        if(iLangId == NOTW_LANG_FRENCH)
            DesiredAfterPropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfm0000) + t;
        else
            DesiredAfterPropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfm0000) + t;
#endif
//////        DesiredAfterPropertyA.mortgaged[iTemp] = TRUE;
//////        DesiredAfterPropertyA.state    = CHANGED;
        TradeAAfterMort |= lTemp;
      } else { // Shut down old property
//        LE_SEQNCR_Stop(DesiredAfterPropertyA.ID[iTemp], DISPLAY_TradeBoxADoneSPriority + (UNS16)order);
        if (movingcardcnt < 0) moveflag = 1;
#if USA_VERSION
        LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_trprfm0000) + t, DISPLAY_TradeBoxADoneSPriority + (UNS16)order);
#else
        if(iLangId == NOTW_LANG_FRENCH)
            LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_xrprfm0000) + t, DISPLAY_TradeBoxADoneSPriority + (UNS16)order);
        else
            LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_zrprfm0000) + t, DISPLAY_TradeBoxADoneSPriority + (UNS16)order);
#endif
        if (DesiredAAfter & lTemp)
        {
#if USA_VERSION
            DesiredAfterPropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfp0000) + t;
#else
        if(iLangId == NOTW_LANG_FRENCH)
            DesiredAfterPropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfp0000) + t;
        else
            DesiredAfterPropertyA.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfp0000) + t;
#endif
        }
        else {
          movingcard = DesiredAfterPropertyA.ID[iTemp];//jkl
          DesiredAfterPropertyA.ID[iTemp] = LED_EI;
        }
//////        DesiredAfterPropertyA.state    = CHANGED;
        TradeAAfterMort &= ~lTemp;
      }
///////////////////////////////////////////////////////////////////////////
    }
    if ((DesiredBAfterMort & lTemp) != (TradeBAfterMort & lTemp))
    { // Need corrective action
      if (DesiredBAfterMort & lTemp)
      { // Bring up new property
        //// modified by David Ethier on april 20 ////
        if (movingcardcnt > 0) moveflag = 2;
#if USA_VERSION
        DesiredAfterPropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfm0000) + t;
#else
        if(iLangId == NOTW_LANG_FRENCH)
            DesiredAfterPropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfm0000) + t;
        else
            DesiredAfterPropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfm0000) + t;
#endif
//////        DesiredAfterPropertyB.mortgaged[iTemp] = TRUE;
//////        DesiredAfterPropertyB.state    = CHANGED;
        TradeBAfterMort |= lTemp;
      } else { // Shut down old property
//        LE_SEQNCR_Stop(DesiredAfterPropertyB.ID[iTemp], DISPLAY_TradeBoxBDoneSPriority + (UNS16)order);
        if (movingcardcnt < 0) moveflag = 2;
#if USA_VERSION
        LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_trprfm0000) + t, DISPLAY_TradeBoxBDoneSPriority + (UNS16)order);
#else
        if(iLangId == NOTW_LANG_FRENCH)
            LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_xrprfm0000) + t, DISPLAY_TradeBoxBDoneSPriority + (UNS16)order);
        else
            LE_SEQNCR_Stop(LED_IFT(DAT_PAT, TAB_zrprfm0000) + t, DISPLAY_TradeBoxBDoneSPriority + (UNS16)order);
#endif
        if (DesiredBAfter & lTemp)
        {
#if USA_VERSION
            DesiredAfterPropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_trprfp0000) + t;
#else
        if(iLangId == NOTW_LANG_FRENCH)
            DesiredAfterPropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_xrprfp0000) + t;
        else
            DesiredAfterPropertyB.ID[iTemp] = LED_IFT(DAT_PAT, TAB_zrprfp0000) + t;
#endif
        }
        else {
          movingcard = DesiredAfterPropertyB.ID[iTemp];//jkl
          DesiredAfterPropertyB.ID[iTemp] = LED_EI;
        }
//////        DesiredAfterPropertyB.state    = CHANGED;
        TradeBAfterMort &= ~lTemp;
      }
////////////////////////////////////////////////////////////////////////////
    }
  } // Done displaying properties.
}

  ////  added by David Ethier on April 20 ////
  // Reposition, if necessary, all properties and display
//  if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA)
//  {
//    if (DesiredAfterPropertyA.state)
//      UDTrade_RepositionProperties(&DesiredAfterPropertyA, 0, 0, 4);
//    if (DesiredAfterPropertyB.state)
//      UDTrade_RepositionProperties(&DesiredAfterPropertyB, 0, 0, 5);
//  }

  if (movingcardcnt > 0) {
    if (movingcardcnt == 1) {
      MoveTime = GetTickCount();
      if (moveflag == 1) {
        movingfromset = &DesiredBeforePropertyB;
        movingcardset = &TradeBeforePropertyB;
      } else if (moveflag == 2) {
        movingfromset = &DesiredBeforePropertyA;
        movingcardset = &TradeBeforePropertyA;
      }

      if (moveflag) {
        if (UICurrentGameState.Squares[movingcard0].mortgaged)
        {
#if USA_VERSION
          movingcard = LED_IFT(DAT_PAT, TAB_trprfm0000) + DISPLAY_propertyToOwnablePropertyConversion(movingcard0);
#else
            if(iLangId == NOTW_LANG_FRENCH)
                movingcard = LED_IFT(DAT_PAT, TAB_xrprfm0000) + DISPLAY_propertyToOwnablePropertyConversion(movingcard0);
            else
                movingcard = LED_IFT(DAT_PAT, TAB_zrprfm0000) + DISPLAY_propertyToOwnablePropertyConversion(movingcard0);
#endif
        }
        else
        {
#if USA_VERSION
          movingcard = LED_IFT(DAT_PAT, TAB_trprfp0000) + DISPLAY_propertyToOwnablePropertyConversion(movingcard0);
#else
            if(iLangId == NOTW_LANG_FRENCH)
                movingcard = LED_IFT(DAT_PAT, TAB_xrprfp0000) + DISPLAY_propertyToOwnablePropertyConversion(movingcard0);
            else
                movingcard = LED_IFT(DAT_PAT, TAB_zrprfp0000) + DISPLAY_propertyToOwnablePropertyConversion(movingcard0);
#endif
        }
        movingcardpri = DISPLAY_TradeBoxAStuffPriority + 302 + (movingcardset->trade_box * 50);
        movingcarddelta[0] = ((movingcardset->Location[movingcard0].left + TradeBasicPlayerBoxes[movingcardset->trade_box][0]) -
                              (movingfromset->Location[movingcard0].left + TradeBasicPlayerBoxes[movingfromset->trade_box][0])) / MOVESTEP;
        movingcarddelta[1] = ((movingcardset->Location[movingcard0].top  + TradeBasicPlayerBoxes[movingcardset->trade_box][1]) -
                              (movingfromset->Location[movingcard0].top  + TradeBasicPlayerBoxes[movingfromset->trade_box][1])) / MOVESTEP;
        movingcardpos[0] = movingfromset->Location[movingcard0].left + TradeBasicPlayerBoxes[movingfromset->trade_box][0];
        movingcardpos[1] = movingfromset->Location[movingcard0].top  + TradeBasicPlayerBoxes[movingfromset->trade_box][1];
        movingcardpos[0] += movingcarddelta[0];
        movingcardpos[1] += movingcarddelta[1];
        LE_SEQNCR_StartXY(movingcard, movingcardpri, movingcardpos[0], movingcardpos[1]);
      }
      movingcardcnt++;
    }
  } else if (movingcardcnt < 0) {
    if (movingcardcnt == -1) {
      MoveTime = GetTickCount();
      if (moveflag == 1) {
        movingfromset = &DesiredBeforePropertyB;
        movingcardset = &TradeBeforePropertyB;
      } else if (moveflag == 2) {
        movingfromset = &DesiredBeforePropertyA;
        movingcardset = &TradeBeforePropertyA;
      }

      if (moveflag) {
        movingcardpri = DISPLAY_TradeBoxAStuffPriority + 302 + (movingfromset->trade_box * 50);
        movingcarddelta[0] = ((movingfromset->Location[movingcard0].left + TradeBasicPlayerBoxes[movingfromset->trade_box][0]) -
                              (movingcardset->Location[movingcard0].left + TradeBasicPlayerBoxes[movingcardset->trade_box][0])) / MOVESTEP;
        movingcarddelta[1] = ((movingfromset->Location[movingcard0].top  + TradeBasicPlayerBoxes[movingfromset->trade_box][1]) -
                              (movingcardset->Location[movingcard0].top  + TradeBasicPlayerBoxes[movingcardset->trade_box][1])) / MOVESTEP;
        movingcardpos[0] = movingcardset->Location[movingcard0].left + TradeBasicPlayerBoxes[movingcardset->trade_box][0];
        movingcardpos[1] = movingcardset->Location[movingcard0].top  + TradeBasicPlayerBoxes[movingcardset->trade_box][1];
        movingcardpos[0] += movingcarddelta[0];
        movingcardpos[1] += movingcarddelta[1];
        LE_SEQNCR_StartXY(movingcard, movingcardpri, movingcardpos[0], movingcardpos[1]);
      }
      movingcardcnt--;
    }
  }

////////////////////////////////////////////////////////////////////////////

  int i;
  if (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA)
  {
    // Jail cards (original, now part of property list?
    for (iTemp = 0; iTemp< 2; iTemp++)
    {
      if (DISPLAY_state.TradeJailCardsShown[iTemp] != DISPLAY_state.TradeJailCardsDesired[iTemp])
      {
        for (i = 0; i < 4; i++) {
          if (DISPLAY_state.TradeJailCardsDesired[iTemp] & (1 << i))
            LE_SEQNCR_StartXY(LED_IFT(DAT_MAIN, (iTemp ? TAB_ibjlcdf1 : TAB_ibjlcdf0)),
              DISPLAY_TradeBasePriority + i + 1,
              (iTemp ? cmchstGooJLoc[i][0] : chanceGooJLoc[i][0]),
              (iTemp ? cmchstGooJLoc[i][1] : chanceGooJLoc[i][1]));
          else if (DISPLAY_state.TradeJailCardsShown[iTemp] & (1 << i))
            LE_SEQNCR_Stop(LED_IFT(DAT_MAIN, (iTemp ? TAB_ibjlcdf1 : TAB_ibjlcdf0)),
              DISPLAY_TradeBasePriority + i + 1);
        }
        DISPLAY_state.TradeJailCardsShown[iTemp] = DISPLAY_state.TradeJailCardsDesired[iTemp];
      }
    }

    if ((UICurrentGameState.GameOptions.futureRentTradingAllowed) ||
        (UICurrentGameState.GameOptions.immunitiesTradingAllowed)) {
      // Calculate what should be shown (future icons)
      DISPLAY_state.TradeImmunityFutureDesiredA[0] = DISPLAY_state.TradeImmunityFutureDesiredA[1] = 0;
      for (int which = 0; which < 2; which++) {
        for (int side = 0; side < 2; side++) {
          // If future owned (outer boxes)
          for (int iTemp1 = 0; iTemp1 < RULE_MAX_COUNT_HIT_SETS; iTemp1++) {
            if (UICurrentGameState.CountHits[iTemp1].toPlayer == (RULE_PlayerNumber)(side ? TradeBPlayerActive : TradeAPlayerActive)) {
              if ((UICurrentGameState.CountHits[iTemp1].hitType == (unsigned int)(which ? CHT_RENT_IMMUNITY : CHT_FUTURE_RENT)) &&
                  (!UICurrentGameState.CountHits[iTemp1].tradedItem)) {
                DISPLAY_state.TradeImmunityFutureDesiredA[which] |= (1 << side);
              }
            }
          }

          // If future being traded (inner boxes)
          for (iTemp1 = 0; iTemp1 < TradeItemListTop; iTemp1++) {
            if ((UDTradeItemList[iTemp1].numberC == (which ? TIK_IMMUNITY : TIK_FUTURE_RENT)) &&
                (UDTradeItemList[iTemp1].numberB == (RULE_PlayerNumber)(side ? TradeAPlayerActive : TradeBPlayerActive)) &&
                (UDTradeItemList[iTemp1].numberD > 0)) {

              if ((TradeAPlayerActive != RULE_MAX_PLAYERS) && (TradeBPlayerActive != RULE_MAX_PLAYERS)) {
                // If the to-be owner of the f/i owns (or will own) the property, delete the f/i
//                if (UDTradeItemList[iTemp1].numberE & (side ? (TradeBAfter | TradeBAfterMort) : (TradeAAfter | TradeAAfterMort)))
                if (!(UDTradeItemList[iTemp1].numberE & (side ? (TradeAAfter | TradeAAfterMort) : (TradeBAfter | TradeBAfterMort))))
                  DISPLAY_state.TradeImmunityFutureDesiredA[which] |= (1 << (side + 2));
                else {
                  memcpy(&UDTradeItemList[iTemp1], &UDTradeItemList[TradeItemListTop-1], sizeof(RULE_ActionArgumentsRecord));
                  TradeItemListTop--;
                  iTemp1--; // need to check this one again

                  for (int q = 0; q < TradeItemListTop; q++) {
                    if ((UDTradeItemList[q].numberC == (which ? TIK_IMMUNITY : TIK_FUTURE_RENT)) &&
                        (UDTradeItemList[q].numberB == (RULE_PlayerNumber)(side ? TradeAPlayerActive : TradeBPlayerActive)) &&
                        (UDTradeItemList[q].numberD < 0)) {
                      if (UDTradeItemList[q].numberE & (side ? (TradeBAfter | TradeBAfterMort) : (TradeAAfter | TradeAAfterMort))) {
                        memcpy(&UDTradeItemList[q], &UDTradeItemList[TradeItemListTop-1], sizeof(RULE_ActionArgumentsRecord));
                        TradeItemListTop--;
                        break;
                      }
                    }
                  }
                }
              } else DISPLAY_state.TradeImmunityFutureDesiredA[which] |= (1 << (side + 2));
//          DISPLAY_state.TradeImmunityFutureDesiredA[which] |= (1 << (side + 2));
            }
          }
        }
      }

      // Show (or stop) future/immunity icons
      for (iTemp = 0; iTemp < 2; iTemp++) {
        if (DISPLAY_state.TradeImmunityFutureShownA[iTemp] != DISPLAY_state.TradeImmunityFutureDesiredA[iTemp]) {
          for (i = 0; i < 4; i++) {
            if (DISPLAY_state.TradeImmunityFutureDesiredA[iTemp] & (1 << i))
              LE_SEQNCR_StartXY(LED_IFT(DAT_LANG2, (iTemp ? TAB_syimm : TAB_syfut)),
                DISPLAY_TradeBasePriority + i + 1,
                (iTemp ? immuneLoc[i][0] : futureLoc[i][0]),
                (iTemp ? immuneLoc[i][1] : futureLoc[i][1]));
            else if (DISPLAY_state.TradeImmunityFutureShownA[iTemp] & (1 << i))
              LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, (iTemp ? TAB_syimm : TAB_syfut)),
                DISPLAY_TradeBasePriority + i + 1);
          }
          DISPLAY_state.TradeImmunityFutureShownA[iTemp] = DISPLAY_state.TradeImmunityFutureDesiredA[iTemp];
        }
      }
    }
  } else {
    for (iTemp = 0; iTemp< 2; iTemp++) {
      if (DISPLAY_state.TradeJailCardsShown[iTemp]) {
//blah          DISPLAY_state.TradeJailCardsDesired[iTemp] = DISPLAY_state.TradeJailCardsShown[iTemp];
        DISPLAY_state.TradeJailCardsDesired[iTemp] = 0;
        for (i = 0; i < 4; i++) {
          if (DISPLAY_state.TradeJailCardsShown[iTemp] & (1 << i))
            LE_SEQNCR_Stop(LED_IFT(DAT_MAIN, (iTemp ? TAB_ibjlcdf1 : TAB_ibjlcdf0)),
              DISPLAY_TradeBasePriority + i + 1);
        }
        DISPLAY_state.TradeJailCardsShown[iTemp] = 0;
      }
    }

    if ((UICurrentGameState.GameOptions.futureRentTradingAllowed) ||
        (UICurrentGameState.GameOptions.immunitiesTradingAllowed)) {
      for (iTemp = 0; iTemp< 2; iTemp++) {
        if (DISPLAY_state.TradeImmunityFutureShownA[iTemp]) {
//blah          DISPLAY_state.TradeImmunityFutureDesiredA[iTemp] = DISPLAY_state.TradeImmunityFutureShownA[iTemp];
          DISPLAY_state.TradeImmunityFutureDesiredA[iTemp] = 0;
          for (i = 0; i < 4; i++) {
            if (DISPLAY_state.TradeImmunityFutureShownA[iTemp] & (1 << i))
              LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, (iTemp ? TAB_syimm : TAB_syfut)),
                DISPLAY_TradeBasePriority + i + 1);
          }
          DISPLAY_state.TradeImmunityFutureShownA[iTemp] = 0;
        }
      }
    }
  }
}
