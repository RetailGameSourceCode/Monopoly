#ifndef __USERIFCE_H__
#define __USERIFCE_H__

/* Monopoly - User interface source code.
 * By Artech Digital Entertainments, copyright (c) 1999.
 *
 * Tab stops are every 2 spaces, also save the file with spaces instead of tabs.
 *
 * $Header: /Projects - 1999/Mono 2/Userifce.h 13    10/05/99 4:37p Russellk $
 *
 * $Log: /Projects - 1999/Mono 2/Userifce.h $
 * 
 * 13    10/05/99 4:37p Russellk
 * 
 * 12    29/09/99 5:01p Agmsmith
 * Added voice chat.
 *
 * 11    9/12/99 2:31p Philla
 * First draft at remove player from game (during setup).  Not tested yet.
 *
 * 10    8/13/99 4:25p Russellk
 * ifdefs to avoid redefinitions
 *
 * 9     6/17/99 11:06a Timp
 * Added prototype&define
 *
 * 8     5/31/99 9:13a Russellk
 * Offline trade enabled - F 3 to send trade.
 *
 * 7     4/29/99 1:09p Russellk
 * Sequencer & engine lock for token sequences begun - its hook for
 * pennybags sound is in.
 *
 * 6     4/08/99 11:02a Russellk
 * ifdef to allow loading demo.msv without prompting and f 1 to load
 * demo.msv.
 *
 * 5     3/17/99 8:48a Russellk
 * Trade viewer partially implemented
 *
 * 4     3/12/99 4:10p Russellk
 * Trade screen started, HIDE functions removed - setting the background
 * to black does the whole job anyway.
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
 *
 */


/************************************************************/
/* EXPORTED DATA STRUCTURES AND TYPES                       */
/************************************************************/
extern RULE_PlayerNumber CurrentUIPlayer;
extern long AuctionHighestBid;
extern BOOL GamePaused;
extern BOOL GameQuitRequested;
extern RULE_PlayerNumber RolledPlayer;
extern RULE_PlayerSet PlayersAllowedInAuction;
typedef int TradingState;
extern TradingState CurrentTradingState;


enum
{
  UI_NoCurrentInputState = 0,
  //UI_Board,
  UI_CGE,
  UI_IBar,
  UI_Opts,
  //UI_Pieces,
  UI_PSel,
  //UI_Sound,
  UI_Stats,
  UI_Trade
};

extern int CurrentUIInputEventHandler;


/************************************************************/
/* EXPORTED FUNCTIONS                                       */
/************************************************************/
extern void LoadGameOrOptions (BOOL LoadingGame);
extern void CheckForAcceptingOurNewPlayer (RULE_ActionArgumentsPointer NewMessage);
extern void CopyNewGameOptions (RULE_ActionArgumentsPointer NewMessage);
extern void SetCurrentUIPlayerFromPlayerSet (RULE_PlayerSet PlayerSet);
extern void SetCurrentUIPlayerFromPlayerNumber (RULE_PlayerNumber PlayerNo);
extern void InitializeTradeSequence(void);
//extern void BuildTradingList(void);
extern void FormatErrorNotification (RULE_ActionArgumentsPointer, wchar_t *);
extern void RemoveLocalPlayer( RULE_PlayerNumber PlayerNo );
extern BOOL USERIFCE_StartVoiceChat (void);



 /********************************************************************
 * NOTE:  Everything under here is from the original Hotseat version.
 * It is/was retained for conversion convenience.  Delete anything
 * you are sure we dont need.
 */
#define TRADE_BUFFER_SIZE         10000
#define UI_MAX_ERROR_MESSAGE_LENGTH 2047

enum TradingStateEnum
{
  TRADING_GETTING_PROPERTY,
  TRADING_GETTING_PLAYER,
  TRADING_SPECTATOR,
  MAX_TRADING_STATES

};



#define BB_CORNER_SQUARE_DIMENSION   136
#define BB_REGULAR_SQUARE_WIDTH      83
#define BB_REGULAR_SQUARE_HEIGHT     BB_CORNER_SQUARE_DIMENSION
#define BB_JUST_VISITING_DIMENSION   38
  /* These defines describe the dimensions of the game board as it exists
  in its raw state as a bitmap. These are used to determine locations of
  squares on the game board. */


typedef struct ClickDetectionZoneStruct
{
  RULE_SquareType square;  /* SQ_OFF_BOARD if clicked on background. */
  BOOL chanceJail; /* TRUE if clicked on Chance get out of jail card. */
  BOOL communityJail; /* Community get out of jail card clicked on. */
  BOOL cash;  /* Cash display area. */
  BOOL playerName; /* Player's name area. */
  BOOL tokenArea;  /* Player's token display box. */
} ClickDetectionZoneRecord, *ClickDetectionZonePointer;
  /* For identifying where the user clicked on the board or on the player
  status area. */


typedef int GenericBitmapPointer; // WRONG - just shuts up compiler during conversion (RK)
typedef struct CommonBitmapsStruct /* Must contain only GenericBitmapPointer fields! */
{
  GenericBitmapPointer BigBoard;
    /* The whole Star Wars board. */

  GenericBitmapPointer CommonPalette;
    /* The standard palette to use in 8 bit mode.  Loaded but not used in 24. */

  GenericBitmapPointer GameDice[6];
    /* Pictures of each face of die. */

  GenericBitmapPointer CommunityChestCards[COMMUNITY_SIZE];
    /* Pictures of every CommunityChest (Rebel) cards in that deck. */

  GenericBitmapPointer ChanceCards[CHANCE_SIZE];
    /* Pictures of every Chance (Empire) cards in that deck. */

  GenericBitmapPointer GameBoardSquares[SQ_BOARDWALK + 1];
    /* Picture of every gameboard square. Used when player wants a detailed look
    at a particular square. */

  GenericBitmapPointer GameBoardDeeds[SQ_TOTAL_PROPERTY_SQUARES];
    /* Picture of the deed card for all squares on the board which have them. */

  GenericBitmapPointer GameBoardMortgages[SQ_TOTAL_PROPERTY_SQUARES];
    /* Pictures of the mortgage values (the back of the deed cards). */

  GenericBitmapPointer GameTokens[MAX_TOKENS];
    /* Pictures of all the possible game tokens the players can choose. */

  GenericBitmapPointer MiniatureRailRoad[2 /* For off = 0 and on = 1 */];
  GenericBitmapPointer MiniatureElectric[2];
  GenericBitmapPointer MiniatureWater[2];
  GenericBitmapPointer MiniatureGetOutOfJail[2];
  GenericBitmapPointer MiniatureProperty[2][8 /* For each property group */];
    /* These little pictures are used to build up the player's status area. */

  GenericBitmapPointer MortgageIndicator;
    /* This bitmap is displayed on the gameboard to indicate that a property
    has been mortgaged. */

  GenericBitmapPointer HousePicture;
    /* Small bitmap of house used to place on board when player buys a house. */

  GenericBitmapPointer HotelPicture[2];
    /* Small picture of hotel used to place on board when player buys hotel.
    The second picture is a hotel sideways for vertically aligned rows on
    the board. */

  GenericBitmapPointer MiniaturePlayerStatusArea;
    /* A preallocated bitmap that is 100 pixels tall and 400 wide, for building
    up pictures of the players' status area.  All you have to do is to clear
    it to blank pixels before using.  Leave it allocated to avoid wasting time
    constantly allocating and deallocating it. */

  GenericBitmapPointer MainBoardScreenArea;
    /* A preallocated bitmap that is the same size of the main board area (the
    size changes as the window gets resized).  Build up the picture of the main
    board here before drawing it. */

  GenericBitmapPointer ScreenSizeBigBoard;
    /* A copy of the BigBoard image scaled down to the current screen size.
    Useful for copying to MainBoardScreenArea to erase tokens that have
    moved.  Avoids having to scale the whole BigBoard every time, it is
    only scaled when the window size changes. */

} CommonBitmapsRecord, *CommonBitmapsPointer;
extern CommonBitmapsRecord CommonBitmaps;


//extern RULE_CardType CardToDisplay;
  /* ID of the chance or community chest card to display. If this value is not NOT_A_CARD,
  it is assumed it contains a valid card identifier and that card will be displayed in the
  center of the board when the board is updated next. */

#define MAX_LOCAL_PLAYERS (RULE_MAX_PLAYERS + 3)
  /* Need one extra so we can sign up a new player in a 6 player game when
  we want to replace an old player.  Due to bugs, replaced players stay in
  the list, so add a few more than 1 extra. */

extern int NumberOfLocalPlayers;
  /* Number of players this user interface is running, 0 to MAX_LOCAL_PLAYERS-1. */

extern wchar_t LocalPlayerNames [MAX_LOCAL_PLAYERS][RULE_MAX_PLAYER_NAME_LENGTH+1];
  /* Names of the local players.  These names are the authorative player
  identification.  If the computer tells us that player named so-and-so
  is playing slot 3, we have to adjust our player to use slot 3 (happens
  when dice are rolled to decide who goes first).  The other data structures
  (SlotIsALocalPlayer, SlotIsALocalHumanPlayer, SlotIsALocalAIPlayer,
  CurrentUIPlayer, LocalPlayerSlots) are all derived from these names. */

extern RULE_PlayerNumber LocalPlayerSlots [MAX_LOCAL_PLAYERS];
  /* Identifies which slot each of our local players has been assigned.  Set
  to RULE_NOBODY_PLAYER if we don't know. */

extern BOOL SlotIsALocalPlayer [RULE_MAX_PLAYERS];
  /* Identifies which players the user interface is responsible for (TRUE for
  local, FALSE for network player or AI).  The index is the player number
  (also known as slot number) that the server uses. */

extern BOOL SlotIsALocalHumanPlayer [RULE_MAX_PLAYERS];
  /* Identifies which local players are humans (or whatever is pounding
  the keyboard). */

extern BOOL SlotIsALocalAIPlayer [RULE_MAX_PLAYERS];
  /* Identifies which local players are AIs. */

extern RULE_GameStateRecord UICurrentGameState;
  /* The current state of the game, as the user interface code thinks it is. */

extern BOOL RunFastWithNoDisplay;
  /* Set to TRUE to turn off the time delay between messages. */

extern BOOL GameInProgress;
  /* TRUE if the game is being played, FALSE if at the end of the game
  or signing up players. */


extern BOOL MainExtendedInitialization( void ); // Custom extension to Main - GameStartup
extern void ProcessLibraryMessage( LE_QUEUE_MessagePointer UIMessagePntr );
void UserClickedProperty( int property );
void UserClickedButton( int mouseClickedButton );

void gameQueueLock( void );
void gameQueueUnLock( void );
extern void AdvanceTimeStep (void);

extern void CleanUpUserInterface (void);
extern BOOL ClickedWhereOnBoard(float X, float Y, ClickDetectionZonePointer Zone);
extern void DrawCentralBoard (HWND hWnd, HDC hdc, LPRECT ChildRectangle, LPRECT ClipRectangle);
extern void DrawPlayerStatus (HWND hWnd, HDC hdc, LPRECT ChildRectangle, RULE_PlayerNumber Player);
extern BOOL LoadUpCommonGraphics (void);
extern BOOL ClickedWhereOnBoard(float X, float Y, ClickDetectionZonePointer Zone);
extern BOOL ClickedWhereInPlayerStatus(float X, float Y, ClickDetectionZonePointer Zone);
extern BOOL ProcessLeftMouseButtonPress(UINT mess, POINT MousePoint);
extern BOOL ProcessMenuItem (int wmID);
extern void ProcessPlayersUI (RULE_ActionArgumentsPointer NewMessage);
extern void RightMouseDownInMainBoard (float X, float Y, HWND hWnd, LPRECT BoardRectangle);
extern void RightMouseUpInMainBoard (HWND hWnd, LPRECT BoardRectangle);
extern void DBUG_DisplayInternationalMessage (LANG_TextMessageNumber MessageNumber, long Number1, long Number2, long Number3, wchar_t *StringParameter);
RULE_PlayerNumber AnyLocalPlayer (void);

extern void AddLocalPlayer (wchar_t *Name, RULE_TokenKind Token,
  RULE_PlayerColour Colour, unsigned char AILevel, BOOL TakeOverAI);

/*****************************************************************************
 * Globals from the main program.
 */

//extern BOOL bDisplayUsesPalette;
  /* True if running in 8 bit mode. */

//extern HMENU hmenuMain;
  /* In case you want to check and uncheck menu items. */

//extern HINSTANCE hAppInstance;
  /* Application instance handle, for loading resources from. */

//extern HWND hwndMain;
  /* Main window handle, NULL when window is closed. */

//extern HWND hdlgErrorDialogBox;
  /* NULL or a handle to the current error displaying dialog box.  This
  one operates independently of the other dialog boxes, so you could
  have two boxes on the screen simultaneously. */

//extern HWND hdlgLastDiceRollDialogBox;
  /* NULL or handle to the currently open modeless dialog box showing the
  previously rolled dice.  This one stays up independently of the other
  boxes. */

//extern HWND hdlgCurrentDialogBox;
  /* NULL or handle to the currently open modeless dialog box showing the
  current proposed game options, or a box prompting you to roll the dice,
  or one that lets you type in a name and pick a token too.  See
  CurrentDialogBoxKind for more. */

//extern HWND hdlgTextChatDialogBox;
  /* Handle to the currently open chat box for text, or NULL if it
  is closed.  This box also has controls for starting a voice chat. */

//extern HWND hdlgVoiceChatProgressBox;
  /* Shows the current voice chat progress and has a button for stopping
  the chat early.  NULL if not open. */

//extern HWND hdlgTradeCashImmunitiesFutures;
  /* Lets other people see if the box is open or closed. */

extern RECT MainBoardRect;
extern RECT GameSquareRects[SQ_MAX_SQUARE_TYPES];
extern RECT PlayerStatusRects[RULE_MAX_PLAYERS];
extern RECT DiceRolledRect;
  /* Subwindow boundaries of the various areas in the main window.  Pixel
  coordinates relative to the main window. */

extern void SetUpLoadedGame(void);

#endif  // __USERIFCE_H__
