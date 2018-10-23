/* Display.h
 *
 */

#define pi 3.1415926535

// Stores the value the 2D cameras and mouse have been adjusted to (screen res changes)
extern int MASTER_displayWidthAdjustedTo;

extern int BoardLocations[42][2]; // Token location on top view (programmer graphic) board.
extern short int propconv[SQ_MAX_SQUARE_TYPES]; // conversion matrix, 39(boardwalk) = 27(boardwalk no unownables)
extern int RULE_PlayerColourToRGB[MAX_PLAYER_COLOURS];

enum // Screens
{
  DISPLAY_SCREEN_Black = 0,
  DISPLAY_SCREEN_Pselect,
  DISPLAY_SCREEN_PselectRules,
  DISPLAY_SCREEN_Options,
  DISPLAY_SCREEN_PortfolioA,
  DISPLAY_SCREEN_MainA,
  DISPLAY_SCREEN_AuctionA,
  DISPLAY_SCREEN_TradeA,
  //DISPLAY_SCREEN_CGE,
  DISPLAY_SCREEN_MAX
};


enum // dialog values
{
  DISPLAY_Dialog_None = -1,
  DISPLAY_Dialog_AddPlayer  = 0,
  DISPLAY_Dialog_AcceptConfig,
  DISPLAY_Dialog_Roll,
  DISPLAY_Dialog_BuyProperty, // property dialogs below used, this one unused.
  DISPLAY_Dialog_RaiseBid,
  DISPLAY_Dialog_BSSM01, // 5 - note these are the tab name numbers, skipped numbers are commented here.
  DISPLAY_Dialog_BSSM02,
  DISPLAY_Dialog_BSSM03,
  DISPLAY_Dialog_BSSM04,
  DISPLAY_Dialog_TradeOffered,
  DISPLAY_Dialog_JailPCR, //10
  DISPLAY_Dialog_JailPXR,
  DISPLAY_Dialog_JailPCX,
  DISPLAY_Dialog_JailPXX,
  DISPLAY_Dialog_GameOver,
  DISPLAY_Dialog_GoBankrupt,
  DISPLAY_Dialog_RollGreyed,
  DISPLAY_Dialog_Done_Turn,
  DISPLAY_Dialog_Done_Turn_Grey,
//DISPLAY_Dialog_none,
  DISPLAY_Dialog_Chance00, //20
  DISPLAY_Dialog_Chance01,
  DISPLAY_Dialog_Chance02,
  DISPLAY_Dialog_Chance03,
  DISPLAY_Dialog_Chance04,
  DISPLAY_Dialog_Chance05,
  DISPLAY_Dialog_Chance06,
  DISPLAY_Dialog_Chance07,
  DISPLAY_Dialog_Chance08,
  DISPLAY_Dialog_Chance09,
  DISPLAY_Dialog_Chance10,
  DISPLAY_Dialog_Chance11,
  DISPLAY_Dialog_Chance12,
  DISPLAY_Dialog_Chance13,
  DISPLAY_Dialog_Chance14,
  DISPLAY_Dialog_Chance15,
//DISPLAY_Dialog_none,
//DISPLAY_Dialog_none,
//DISPLAY_Dialog_none,
//DISPLAY_Dialog_none,
  DISPLAY_Dialog_CommChest00, // 40
  DISPLAY_Dialog_CommChest01,
  DISPLAY_Dialog_CommChest02,
  DISPLAY_Dialog_CommChest03,
  DISPLAY_Dialog_CommChest04,
  DISPLAY_Dialog_CommChest05,
  DISPLAY_Dialog_CommChest06,
  DISPLAY_Dialog_CommChest07,
  DISPLAY_Dialog_CommChest08,
  DISPLAY_Dialog_CommChest09,
  DISPLAY_Dialog_CommChest10,
  DISPLAY_Dialog_CommChest11,
  DISPLAY_Dialog_CommChest12,
  DISPLAY_Dialog_CommChest13,
  DISPLAY_Dialog_CommChest14,
  DISPLAY_Dialog_CommChest15,
//DISPLAY_Dialog_none,
//DISPLAY_Dialog_none,
//DISPLAY_Dialog_none,
//DISPLAY_Dialog_none,
  DISPLAY_Dialog_Properties00, // 60
  DISPLAY_Dialog_Properties01,
  DISPLAY_Dialog_Properties02,
  DISPLAY_Dialog_Properties03,
  DISPLAY_Dialog_Properties04,
  DISPLAY_Dialog_Properties05,
  DISPLAY_Dialog_Properties06,
  DISPLAY_Dialog_Properties07,
  DISPLAY_Dialog_Properties08,
  DISPLAY_Dialog_Properties09,
  DISPLAY_Dialog_Properties10,
  DISPLAY_Dialog_Properties11,
  DISPLAY_Dialog_Properties12,
  DISPLAY_Dialog_Properties13,
  DISPLAY_Dialog_Properties14,
  DISPLAY_Dialog_Properties15,
  DISPLAY_Dialog_Properties16,
  DISPLAY_Dialog_Properties17,
  DISPLAY_Dialog_Properties18,
  DISPLAY_Dialog_Properties19,
  DISPLAY_Dialog_Properties20,
  DISPLAY_Dialog_Properties21,
  DISPLAY_Dialog_Properties22,
  DISPLAY_Dialog_Properties23,
  DISPLAY_Dialog_Properties24,
  DISPLAY_Dialog_Properties25,
  DISPLAY_Dialog_Properties26,
  DISPLAY_Dialog_Properties27,
  DISPLAY_Dialog_IncomeTax,
  DISPLAY_Dialog_MAX
};
#define DISPLAY_Dialog_OffsetX  74
#define DISPLAY_Dialog_OffsetY  74

enum IBAR_BUTTONS
{ // These are all the valid button LOCATIONS on the IBar.
  // Note the locations ceased to be standard and are now variable size.
  // We continue to treat buttons as being in slots anyway.
  IBAR_Options = 0,
  IBAR_Trade,
  IBAR_General1,
  IBAR_General4,
  IBAR_General2,
  IBAR_General3,
  IBAR_Main,
  IBAR_View1,
  IBAR_Status,
  IBAR_View2,
  IBAR_View3,
  IBAR_Player1,
  IBAR_Player2,
  IBAR_Player3,
  IBAR_Player4,
  IBAR_Player5,
  IBAR_Player6,
  IBAR_PlayerBank,
  IBAR_BUTTON_MAX
  // NOTE: This enum is used to store current graphics on screen and the property
  // bar is handled elsewhere and so those buttons aren't included here.
};

// Display priorities to orientation front to back
#define DISPLAY_VERYHIGH            1000
#define DISPLAY_FREAKIN_HIGH        5000
#define DISPLAY_EscMenuStartY       83
#define DISPLAY_EscTimeoutSeconds   10
#define DISPLAY_BackgroundPriority  10 // backdrop priority

// Board items are for the 2D viewport.
#define DISPLAY_BoardHighlightPriority   DISPLAY_BackgroundPriority + 2
#define DISPLAY_BoardHighlightPriorityMax  DISPLAY_BoardHighlightPriority + 42

#define DISPLAY_BoardHousingPriority     DISPLAY_BoardHighlightPriorityMax + 1
#define DISPLAY_BoardHousingPriorityMax  DISPLAY_BoardHousingPriority + 42 * 4

#define DISPLAY_TokenPriority            DISPLAY_BoardHousingPriorityMax + 1

#define DISPLAY_BoardMaxPriority         DISPLAY_TokenPriority + 10

// Status screen priorities
#define DISPLAY_SSPropertyPriority       STATUS_DEED_Priority//DISPLAY_BoardMaxPriority + 6

// IBar items are for IBar controle at the bottom of the screen.
#define DISPLAY_IBAR_BasePriority        DISPLAY_BoardMaxPriority + 20
#define DISPLAY_IBAR_GeneralPriority     DISPLAY_IBAR_BasePriority + 2
#define DISPLAY_DialogPriority         DISPLAY_IBAR_BasePriority + 10 // Temp Dialogs
#define DISPLAY_IBARScoreBoxPriority     DISPLAY_IBAR_BasePriority + 1 + 50 // Get over properties
#define DISPLAY_IBAR_ButtonBasePriority  DISPLAY_VERYHIGH - 1  // buttons fly over, but they also sit on bar - go low.

#define DISPLAY_MessageBoxPriority     DISPLAY_DialogPriority - 1
#define DISPLAY_PropertyFoatingBlowUp     DISPLAY_VERYHIGH + 3
#define DISPLAY_ChanceCommChestPriority   DISPLAY_PropertyFoatingBlowUp + 2

// Auction screen priorities
#define DISPLAY_AuctionBasePriority       DISPLAY_IBARScoreBoxPriority + 10 // High, while they overlap IBAR.
#define DISPLAY_AuctionPennybagsPriority  DISPLAY_AuctionBasePriority + 10 //add #players + 3 minimum
#define DISPLAY_AuctionPropertyPriority   (DISPLAY_AuctionPennybagsPriority + 5)

// Trade screen priorities
#define DISPLAY_TradeBasePriority         DISPLAY_BoardMaxPriority + 40 // Above message box (temp, back to + 0 later)
#define DISPLAY_TradeBoxAStuffPriority    DISPLAY_TradeBasePriority + 50 //These boxes share tabs, priority must be higher (though an offset of 1 might do it?)
#define DISPLAY_TradeBoxBStuffPriority    DISPLAY_TradeBoxAStuffPriority + 50
#define DISPLAY_TradeBoxAItemsPriority    DISPLAY_TradeBoxBStuffPriority + 50
#define DISPLAY_TradeBoxBItemsPriority    DISPLAY_TradeBoxAItemsPriority + 50
#define DISPLAY_TradeBoxADoneSPriority    DISPLAY_TradeBoxBItemsPriority + 50
#define DISPLAY_TradeBoxBDoneSPriority    DISPLAY_TradeBoxADoneSPriority + 50


// ChatBox stuff
#define DISPLAY_MessageMaxLen      200
#define DISPLAY_MessageBoxW        292
#define DISPLAY_MessageBoxH        150
#define DISPLAY_MessageBoxX        400 + ( (400 - DISPLAY_MessageBoxW) / 2 )
#define DISPLAY_MessageBoxY        580 - 147 - DISPLAY_MessageBoxH
#define DISPLAY_MessageBoxTextBorder  5

// 3D priorities (don't think they do anything)
#define DISPLAY_Generic3dPriority 100
#define DISPLAY_Board3dPriority   90


// Miscellaneous or temporary stuff
#define DISPLAY_ScoreY    560
#define DISPLAY_View2XOff 195
#define DISPLAY_View2YOff 38
#define TAB_deed01 TAB_iyf00x00
#define DISPLAY_DEEDS_PER_SET 28 //(TAB_iyf01x00 - TAB_iyf00x00)
#define ANIMS_PER_TOKEN (CNK_knbcx0 - CNK_knacx0)

struct ANIMINFO_3D // Used in token move anims
{
  int isCameraMoveOnly;             // Flags the entry moves the camera, doesn't stop/start sequences
  TYPE_Tick timeForCameraMoveOnly;  // NOT IMPLEMENTED Tickcount to move camera on, based on TokenMoveAnimStartTime (typically the last sequence start)

  int PresetCameraView;
  LE_DATA_DataId NewSequenceID;
  int SquareStartingNewSequence;  // The square which the token will be on when this sequence starts.  Used to evaluate starting location of seq.
  int SquareLandingNewSequence;   // Custom Cam might be interested
  BOOL IsCorner;                  // Custom Cam might be
};
#define TOKEN_ANIM_ITEMS_MAX  12 // Maximum instructions for one move.  FIXME - only long trips from cards or to jail? could do more than a few sequences.  Do they have special anims allowing this number to be reduced?

// Some 3D stuff
#define CAMERA_BASE_MOVE_TIME 75  // Usual camera move time in ticks.
#define FRAME_RATE_MINIMUM    14  // When we fall below this we try a faster board.
#define FRAME_RATE_SAMPLE_SECONDS 5 // Seconds per sample

#define CLIPPING_PLANE_NEAR   10.0f
#define CLIPPING_PLANE_FAR    1540.0f

// (BMP_mybs0101 - BMP_mybs0001) would be better, but the names were not the same for all languages.  39 works...
#define DISPLAY_2D_BOARDS_PER_CITY  39
// Display state structure
typedef struct DISPLAY_stateStruct
{ // Remember this is only for display purposes - track game states elsewhere
//Animation sequencing stuff
  // Game (rules) Queue lock/unlock requests (to stop processing while animations play
  int LockGameQueueRequested; // Note this will be cleared if it stays locked too long
#define DISPLAY_GAME_QUEUE_MAX_LOCK_SECONDS  15
  TYPE_Tick LockGameQueueLastAction;

  //UICurrentGameState.Players[#].currentSquare // holds the rules engine token location, we move to it with anims
  //int TokenShownOnSquareNo[RULE_MAX_PLAYERS];
  ANIMINFO_3D TokenAnimStack[TOKEN_ANIM_ITEMS_MAX];
  LE_DATA_DataId TokenCurrent3DSequence;
  int TokenAnimStackTop, TokenAnimStackIndex;
  // Special flags for victory looping animation
  int   TokenLoopAnimTop, TokenLoopAnimEnd;
  BOOL  IsVictoryAnim, isVictoryGameLockReleased;
  int   VictoryToken;


//Interface/Display stuff
  int tempConversionMouseClickedButton; // Generic dialog button - remove when possible.
  LE_DATA_DataId  currentBackdropID;
  //LE_DATA_DataId  currentBoardViewID; // Have to use backdrop ID (or custom code) so other backdrops can turn us off.
  LE_DATA_DataId  currentBoardOverlayID;
  int VirtualTimPageForAnythingNotPreloaded;  // Tim page to leave active after preloading all expected hmd's.

  // IBar Button bar(dialog) control
  int IBarCurrentState; // IBAR_STATES - Logic will need refinement.
  int IBarCurrentPlayer;// Goes with state
  int IBarLastRuleState;// IBAR_STATES - applies to current game player
  int IBarLastRulePlayer;// goes with rules state (not current player if raise money, trade,...)
  int IBarStateTrackOn; // True means current state is tracking rules, otherwise yer wandering.
  // The currentstate flag is used exclusively when tracking is off, when tracking is on it
  // is still used when you move to a non-rules state such as clicking 'mortgage'.  So if
  // tracking is on Current state will follow lastRuleState when
  int IBarDeedForDeedState; // Deed selected entering deed state
  int IBarDeedButtonsAvail; // Actions which can be performed on deed (by cash/state)
  int IBarBSSMButtonsAvail; // Which of these buttons are on screen (hence can be clicked)
  int IBarDeedButtonsCash;  // How much cash was the button evaluation based on.
  BOOL IBarRaiseCashCanBankrupt;
  int IBarRaiseTotalCashNeeded;
  LE_DATA_DataId ButtonBarMessage;  // Little object we can write on.
  TYPE_Tick ButtonBarWantedTime;    // Makes it more useful - will shut down after we stop asking for it
  BOOL ButtonBarMessageIsUp;
  BOOL ButtonBarForceUpdate;        // Set to true when reprinting in case its already up.
#define DISPLAY_BUTTONMESSAGE_SECONDS 4
  int IBarLastActionNotificationFor; // Store the button anim state - see IBAR_DistinctButtons
  TYPE_Tick LE_TickCount_Last_Button_Click; // For demo mode (& other stuff?) - time of last user action.
  BOOL IBarGameJustLoaded;           // Flags a new game loaded - first IBar state change will set up player.
  BOOL JustReadACardHack;             // last minute hack used to avoid pay cash line when landing on a utility from a card (you roll before paying)

  int DemoModeOn, DemoModeDesired;

  int CurrentDialog;
  LE_DATA_DataId CurrentDialogDataID;
  int CurrentDialogPriority;
  LE_DATA_DataId  IBarButtons[IBAR_BUTTON_MAX];//Old
  LE_DATA_DataId  IBarButtonsShown[IBAR_BUTTON_DISTINCT_MAX];//New
  int             IBarButtonsVisualState[IBAR_BUTTON_DISTINCT_MAX];
  int IBarButtonLastMouseOver,   IBarButtonCurrentMouseOver;
  int IBarPropertyLastMouseOver, IBarPropertyCurrentMouseOver;
  int IBarPropertyCurrentMouseOverCheck;
  TYPE_Tick IBarPropertyCurrentMouseOverTickCount;
  int IBarTradePropertyCurrentMouseOver, IBarTradePropertyCurrentMouseOverCheck;
  TYPE_Tick IBarTradePropertyCurrentMouseOverTickCount;
  int IBarPlayerLastMouseOver,   IBarPlayerCurrentMouseOver;
#define DISPLAY_MouseOverPopUpDelay   36  // Delay before a status bar property will 'pop up'
  LE_DATA_DataId  IBarProperties[SQ_MAX_SQUARE_TYPES];
  UNS16           IBarPropertiesPriority[SQ_MAX_SQUARE_TYPES];
  LE_DATA_DataId  IBarPropertyBlownUp; // If mouseover show it.
  LE_DATA_DataId  IBarTradePropertyBlownUp; // If mouseover show it.
  LE_DATA_DataId  IBarJailCards[2];
  LE_DATA_DataId  IBarBankJailCards[2];// User Objects with remaining houses & hotels printed on em.
  int             IBarBankJailCardValuesPrinted[2];

  // Escape key stuff
  BOOL      ESC_Menu_Up;
  TYPE_Tick ESC_Menu_Up_Time;

  // Status screen property display
  LE_DATA_DataId  SSJailCards[2];
  LE_DATA_DataId  SSProperties[SQ_MAX_SQUARE_TYPES];
  UNS16           SSPropertiesPriority[SQ_MAX_SQUARE_TYPES];

  // Player scorebox & token control.
  BOOL            ShowOnlyLocalPlayersOnIBar; // Required for remove-player code during setup
  BOOL            ShowOnlyLocalAIPlayersOnIBar;    // ditto as above but for MS Zone Lobby.
  BOOL            IsPlayerVisible[RULE_MAX_PLAYERS];  // goes with above
  LE_DATA_DataId  CurrentIbarBackdrop;
  int             LastPlayerScoresPrinted[RULE_MAX_PLAYERS];
  wchar_t         namePrinted[RULE_MAX_PLAYERS][RULE_MAX_PLAYER_NAME_LENGTH + 1];
  TYPE_Tick       LastPlayerScoreUpdateTime[RULE_MAX_PLAYERS];
  LE_DATA_DataId  PlayerScoreBoxes[RULE_MAX_PLAYERS];
  LE_DATA_DataId  PlayerTokens[RULE_MAX_PLAYERS];
  BOOL            PlayerTokenJailUp[RULE_MAX_PLAYERS];
  LE_DATA_DataId  CurrentPlayerShownID;  // IBAR, current player token spinning.
  LE_DATA_DataId  CurrentPlayerColorID;  // Color under name, current player
  LE_DATA_DataId  CurrentPlayerNameTag;  // Name shown
  LE_DATA_DataId  CurrentPlayerNameTags[RULE_MAX_PLAYERS];      // Player name objects
  wchar_t         CurrentPlayerNamesDrawn[RULE_MAX_PLAYERS][RULE_MAX_PLAYER_NAME_LENGTH + 1];  // Tracks when to redraw names
  int             PlayerScoreBoxesLocX[RULE_MAX_PLAYERS + 1];// Last one is for the Bank icon.
#define DISPLAY_IBAR_ScoreBorder 2
#define DISPLAY_IBAR_BankWidth  45 // 39 plus a little free space.

  LE_DATA_DataId  Player3DTokenShown[RULE_MAX_PLAYERS]; // last displayed board token
  int             Player3DTokenIdlePos[SQ_MAX_SQUARE_TYPES][RULE_MAX_PLAYERS]; // -1 = non or center idle, 0 - 5 are the resting idles
  int             CurrentPlayerInCenterIdle; // -1 means none (start of game)
  int             IdleToCenterMoveStatus; // Last goes in, current out.  0 = none, 1 = queue locked, 2 = anims started (waiting to finish), '3' shuts down and returns to 0.
  //BOOL            JailAnimsOn;        // FALSE means trim/skip animations. using Davids option screen flag
  int             GoingToJailStatus;  // 0 is none, 1 queue locked, remaining are anim specific
  int             GoingToJailFromSquare; // The square we went to jail from
  int             PlayerMovingIn, PlayerMovingOut, PlayerInPaddywagon; // -1 means none.
  LE_DATA_DataId  PlayerMovingInID, PlayerMovingOutID;

  LE_DATA_DataId  PlayerColorBarShownID[RULE_MAX_PLAYERS];// Different -4.5+ - store don't count on calculation.
  float TokenCur3DLocX[RULE_MAX_PLAYERS];
  float TokenCur3DLocY[RULE_MAX_PLAYERS];
  float TokenCur3DLocZ[RULE_MAX_PLAYERS];
  float TokenCur3DAngleY[RULE_MAX_PLAYERS];
  BOOL IsPlayerDisplayed[RULE_MAX_PLAYERS]; // flag if it was LE_SEQNCR_Start'ed
  BOOL IsBankDisplayed; // At right of player score bar.
  BOOL IsBankMouseOver; // True if the mouseover is active (icon lowered).
  BOOL FlashCurrentToken;

  // Ownership, Mortgaging & housing.
  LE_DATA_DataId  HighlightDataIDs[SQ_MAX_SQUARE_TYPES];  // Will be mortgaged value if the property was mortgaged when last drawn.
  UNS16           HighlightPriority[SQ_MAX_SQUARE_TYPES];
  LE_DATA_DataId  HousingDataIDs[SQ_MAX_SQUARE_TYPES][4]; // The first element will hold a hotel ID if appropriate.
  BOOL            HotelFlag[SQ_MAX_SQUARE_TYPES];
  BOOL            HouseFlags[SQ_MAX_SQUARE_TYPES][4];
  UNS16           HousingPriority[SQ_MAX_SQUARE_TYPES][4];

  // Message box stuff
  char  DesiredMessageText[DISPLAY_MessageMaxLen];
  char  CurrentMessageText[DISPLAY_MessageMaxLen];
  int   MessageCurLocX, MessageCurLocY; // Where it was last plotted
  int   MessageDesiredX, MessageDesiredY;
  LE_DATA_DataId MessageBoxID;
  BOOL isMessageBoxDisplayed;
  BOOL MessageBoxSuppressed;

  // Dice
  int QueueLockedForDiceAnim;       // Important - the game queue has been locked for a dice animation, clear lock when done so move (and game!) will continue.
  TYPE_Tick QueueLockTime;          // Important - this will give me a dice bail-out time for the lock.
  TYPE_Tick DiceCameraSetTime;      // Used like a flag to move the dice cam just once (then set equal to lock time).
  LE_DATA_DataId Current3DDiceAnim; // Handle of current 3D anim - held at end.
  LE_DATA_DataId CurrentDiceID[2];
  int RollDiceAnimDesired; // This turns off the regular dice display to show bobbing dice (roll)
  LE_DATA_DataId CurrentBobDice;
  BOOL DiceRollNotification;  // Flags the notify so dice will change even if bobbing was skipped

  // Auction screen
  LE_DATA_DataId  AuctionBottomBar;
  LE_DATA_DataId  AuctionPlayerBackdrop[RULE_MAX_PLAYERS];
  int   AuctionPlayerDisplayed[RULE_MAX_PLAYERS];
  int   AuctionHighestBidDisplayed;
  int   AuctionHighestBidder;
  int   AuctionPennyBagsNextState;
  BOOL  AuctionPennyBagsSwitchState;
  BOOL  Auction3rdGoingReceived;
  BOOL  AuctionPennyBagsAnimReachedEnd;
  LE_DATA_DataId  AuctionPennyBagsDisplayedID;
  LE_DATA_DataId  AuctionPennyBagsDesiredID;
  LE_DATA_DataId  AuctionPennyBagsSound;
  LE_DATA_DataId  AuctionPennyBagsDesiredSound;
  BOOL            AuctionPennyBagsSpecificFunction;   // Which sound function to call:
                          // either UDPENNY_PennybagsGeneral() or UDPENNY_PennybagsSpecific()
  // Color and token are stored when displayed in case they change during auction (they shouldn't so it wont auto-update, but we will clear right)
  LE_DATA_DataId  AuctionPlayerToken[RULE_MAX_PLAYERS];
  LE_DATA_DataId  AuctionPlayerNameText[RULE_MAX_PLAYERS];
  LE_DATA_DataId  AuctionPlayerBidText[RULE_MAX_PLAYERS];
  LE_DATA_DataId  AuctionPlayerCashText[RULE_MAX_PLAYERS];
  LE_DATA_DataId  AuctionCurrentBidText;
  BOOL            AuctionCurrentBidIsDisplayed;
  long  AuctionBids[RULE_MAX_PLAYERS];
  long  AuctionBidsPrinted[RULE_MAX_PLAYERS];
  // Following tracks whether bills are displayed - a player can become AI during an auction.
  RULE_PlayerSet AuctionPlayersAllowedToBid; // Bitset
  int   AuctionPropertyForSale;
  LE_DATA_DataId AuctionPropertyForSaleShown;
  // The button click animation plays over the button, we only need to start it and let it stop itself.
  RULE_PropertySet AuctionPlaceBuildingSet;

  // Trade stuff
  RULE_PropertySet  TradeFreeUnmortgageSet;//Trade & bankruptcy
#define TradeCashBoxes  4
  LE_DATA_DataId    TradeCashBoxObject[TradeCashBoxes];
  int               TradeCashBoxValues[TradeCashBoxes];
  int               TradeCashBoxDesiredValue[TradeCashBoxes];
  int               TradeCashBoxShown[TradeCashBoxes];
  LE_DATA_DataId    TradeJailCardsShown[2];
  LE_DATA_DataId    TradeJailCardsDesired[2];

  int               TradeImmunityFutureTopA;
  LE_DATA_DataId    TradeImmunityFutureShownA[RULE_MAX_COUNT_HIT_SETS];
  LE_DATA_DataId    TradeImmunityFutureDesiredA[RULE_MAX_COUNT_HIT_SETS];
  int               TradeImmunityFutureHitsA [RULE_MAX_COUNT_HIT_SETS];
  RULE_PropertySet  TradeImmunityFuturePropsA[RULE_MAX_COUNT_HIT_SETS];
  int               TradeImmunityNotFutureA[RULE_MAX_COUNT_HIT_SETS];

  int               TradeImmunityFutureTopB;
  LE_DATA_DataId    TradeImmunityFutureShownB[RULE_MAX_COUNT_HIT_SETS];
  LE_DATA_DataId    TradeImmunityFutureDesiredB[RULE_MAX_COUNT_HIT_SETS];
  int               TradeImmunityFutureHitsB [RULE_MAX_COUNT_HIT_SETS];
  RULE_PropertySet  TradeImmunityFuturePropsB[RULE_MAX_COUNT_HIT_SETS];
  int               TradeImmunityNotFutureB[RULE_MAX_COUNT_HIT_SETS];

  // Board n lighting stuff
#if USA_VERSION
#define DISPLAY_CITY_MAX 11
#else
//#define DISPLAY_CITY_MAX 13
#define DISPLAY_CITY_MAX 12
#endif
  // City & custom board control stuff
  int               city;       // -1 means use path, 0 means classic, otherwise is an index for the city - Deeds n 2Dbackdrops
  int               system;     // Monatary System to use for money calculations. Refer to defines found in UDPENNY.H (ex. NOTW_MONA_UK)
                                // Default value is NOTW_MONA_US
  TCHAR             customBoardPath[MAX_PATH]; // Folder to load 2D boards from.
  int               cityTextureSet; // Indicates which city texture set we are
                                    // using.  1 - 10 are built-in NorthAmerican
                                    // cities, 0 is standard.  -1 indicates a custom board.
  BOOL              board3Don;  // TRUE when 3D board is active - FALSE we use the 2D renders.
  BOOL              game3Don;   // TRUE if we are to run 3D full time (board & background on main screen)
  BOOL              manualCamLock;      // TRUE indicates we will ignore regular camera orders
  int               manualCamDesired;   // The camera to 'force'.
  BOOL              manualCamRequested; // Indicates process one waiting camera move - meaning it was manually requested.
  BOOL              manualMouseCamLock; // TRUE indicates the user is moving the camera using the mouse.
  TYPE_Tick         manualMouseCamTime; // Last time used
#define DISPLAY_MOUSE_CAM_RELEASE_SECONDS 20

  BOOL              supports8BitPalTextures;  // Indicates that the 3D Device
                                              // being used supports an 8bit
                                              // palettized texture.

  int               current2DView, desired2DView, former2DView; // enum starts DISPLAY_SCREEN_Black
  int               viewPriorToBSSMCam, viewBSSMCamState, viewLastRequested;// State 0 off, 1 issued, 2 under way, 3goes to 0 reset.
  TYPE_Tick         viewBSSMCamRequestTime;
  BOOL              desiredCameraInvalidatedLock; // True indicates 'something' is moving the camera in a custom manner
  BOOL              desiredCameraClearToValidate; // True means the regular camera control can (& should) move the camera back to the valid location.
  BOOL              desiredCameraCustomMoveIgnoreStandardCommands; // Will cause ActivateDesiredCamera to skip setting
#define DISPLAY_CUSTOM_MOVE_MAX_SECONDS 20  // After that, ActivateDesiredCamera
  TYPE_Tick         desiredCameraCustomMoveStartTime; // So we can timeout (failsafe)
  BOOL              viewportBackgroundFillOn;
  int               viewportInUse; // VIEWPORTS enum value.
  int               current2DCamera, desired2DCamera;
  BOOL              lightSpotlightOn, lightSun, lightBoardGlow;
  int               frameRateTrackerIndex; // the next track about to be overwritten
  int               frameCount; // frames this track
  TYPE_Tick         frameRateTrackerStartTime; // For this track
  int               frameRateTracker[12]; // 5 second counts for the last minute.

  // Sound stuff
  LE_DATA_DataId    CurrentBackgroundMusic;
  LE_DATA_DataId    PennybagsLastSoundStarted;
  LE_DATA_DataId    TokenLastSoundStarted[RULE_MAX_PLAYERS];
#define DISPLAY_MAX_SOUND_LOCKS 5
  LE_DATA_DataId    SoundLocks[DISPLAY_MAX_SOUND_LOCKS];
  int               SoundLockIndex; // Empty spaces are not filled - we can stack sound locks, when all are finished the stack index goes to 0 and we free the lock.
  BOOL              SoundLockGameQueueLockActive;   // Flag indicating this sound system has issued a game queue lock.
  LE_DATA_DataId    SoundToPlayPostLock;            // Awaiting sound
  int               SoundToPlayPostLockToken;       // So we can record who is talking.
  LE_DATA_DataId    SoundPlayedPostLock;            // Last awaiting sound which was started - don't know if we care, but recorded anyway.
  int               SoundPlayedPostVolume;

  // Misc stuff, self explanatory I hope.
  LE_DATA_DataId    PropertyBuyAuctionDesired, PropertyBuyAuctionShown;
  BOOL              PropertyBuyAuctionOnLeft; // Records the left/right side status of deed displayed.
#define DISPLAY_BuyAuctProp_OffsetXLeft   20
#define DISPLAY_BuyAuctProp_OffsetXRight  560
#define DISPLAY_BuyAuctProp_OffsetXTrade  594
//#define DISPLAY_BuyAuctProp_OffsetXStatus 560
#define DISPLAY_BuyAuctProp_OffsetY       110
#define DISPLAY_MouseOver_OffsetX         DISPLAY_BuyAuctProp_OffsetXRight - 20
#define DISPLAY_MouseOver_OffsetY         DISPLAY_BuyAuctProp_OffsetY + 20
  int               ChanceCommChestDesired; // -1 for none, 0-31 requests display.
  int               ChanceCommChestAnim;    // -1 none (ID == NULL), 0 coming in, 1 idle, 2 going out
  LE_DATA_DataId    ChanceCommChestDataID;  // ID showing.

  int               desiredPlayerSetupPhase;
  int               currentPlayerSetupPhase;
  int               previousPlayerSetupPhase;
                      // Which stage of player setup (choose token, rules etc) are we in.  See udpsel.h
  int               setupPhaseDesiredSoundIndex;
  int               setupPhasePlayingSoundIndex;
  int               setupPhaseSoundClipAction;
                      // What (usually pennybags) sound are we playing during player setup at
                      // beginning.  An enum into an array, used by UDPENNY_PennybagsGeneral()

  BOOL        SoundSystemClearToTest;

  // booleans used to determine if option was chosen or not in the options screen
  BOOL				IsOptionTokenVoicesOn;
  BOOL				IsOptionHostCommentsOn;
  BOOL				IsOptionMusicOn;
  int				OptionMusicTuneIndex;	// used to determine which background tune is playing
													//(see enum in  UDOpts.h)
  BOOL				IsOptionTokenAnimationsOn;
  BOOL				IsOption3DCameraMovementOn;
  BOOL				IsOptionLightingOn;
  BOOL				IsOption3DBoardOn;
  BOOL				IsOptionFilteringOn;
  BOOL				IsOptionDitheringOn;
  int				OptionScreenResIndex;	// used to determine which resolution the user chose
  BOOL				IsOptionGameStarted;	// used with the background music option


} DISPLAY_stateType;
extern DISPLAY_stateType DISPLAY_state;

// This structure is used to hold values for which 3D boards we are using and
//  other 3D render settings
struct DISPLAY_RenderSettingsStruct
{
  BYTE    bClassic_UseHiResMesh         : 1;
  BYTE    bClassic_UseHiResTextures     : 1;
  BYTE    bClassic_UseDithering         : 1;
  BYTE    bClassic_UseBilinearFiltering : 1;

  BYTE    bCity_UseHiResMesh            : 1;
  BYTE    bCity_UseHiResTextures        : 1;
  BYTE    bCity_UseDithering            : 1;
  BYTE    bCity_UseBilinearFiltering    : 1;

  BYTE    bUse3DBoard                   : 1;
  BYTE    bUseBackground                : 1;
};
extern DISPLAY_RenderSettingsStruct DISPLAY_RenderSettings;


// Trade storage stuff
#define TRADE_MaxMessages 28 + RULE_MAX_PLAYERS_CHOOSE_2 + RULE_MAX_COUNT_HIT_SETS + 2 //Properties + players(cash)! + ImmunitiesMax + jail cards - count hit sets is large, probably more than needed.


// Prototypes 'n Macros
#define DISPLAY_IsIBarVisible \
((DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA) || \
 (DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA) || \
 (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA))


// The property bar may be available but not visible if a player
// has hit the options button and the file/credits/help/options
// buttons are showing instead.
#define DISPLAY_IsPropertyBarAvailable \
((DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA )  || \
 (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA))
 //(DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA))
 //(DISPLAY_state.desired2DView == DISPLAY_SCREEN_AuctionA)||

#define DISPLAY_IsBoardVisible \
((DISPLAY_state.desired2DView == DISPLAY_SCREEN_MainA ) || \
 (DISPLAY_state.desired2DView == DISPLAY_SCREEN_TradeA) || \
 (DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA))


#define DISPLAY_IsWideCurrency \
( DISPLAY_state.system == 2 ) // UK = 0, germ = 2


BOOL __cdecl DISPLAY_initialize( void );
void DISPLAY_destroy( void );
// Show all originally was called to update the display.  It is now on a timer
// but for performance, we could lower the timer (to twice a second from 60) and
// use this as a flag.  OPTIMIZE only if necessary.
#define DISPLAY_showAll
void DISPLAY_showAll2( void );
void DISPLAY_tickActions( int numberOfTicks );

void DISPLAY_showMessageBox( void );
void DISPLAY_hideMessageBox( void );
void DISPLAY_setMessageBox( char *string );

//int DISPLAY_propertyToOwnablePropertyConversion( int property );
#define DISPLAY_propertyToOwnablePropertyConversion( x ) propconv[x]

// House buy/sell determination stuff
RULE_PropertySet DISPLAY_PlaceBuildingLegalSquares( int player );
RULE_PropertySet DISPLAY_SellBuildingLegalSquares( int player );
LANG_TextMessageNumber DISPLAY_TestBuildingPlacement (
  RULE_GameStatePointer GameStatePntr, RULE_PlayerNumber Purchaser,
  RULE_SquareType SquareNo, BOOL Adding, BOOL AddHouseOnly,
  BOOL AddHotelOnly, BOOL *BuildingAHotelPntr,
  int *RemainingBuildingsPntr, long *MessageArgumentPntr);

BOOL DISLAY_NewScreenSizeNDepth( int x, int y, int depth );
