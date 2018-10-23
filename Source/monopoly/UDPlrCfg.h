// ===========================================================================
// Module:      UDPlrCfg.h
//
// Description: Structure definitions and function prototypes for the Player
//              Configuration screen.
//
// Created:     March 18, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================


#ifndef INC_UDPLRCFG
#define INC_UDPLRCFG

// ===========================================================================
// Structures and enums ======================================================
// ===========================================================================


// Structure used to add new players to the game
struct PlayerStruct
{
  wchar_t           szName[RULE_MAX_PLAYER_NAME_LENGTH];// Name of the player
  int               nPlayerNumber;// The player's number
  RULE_TokenKind    eToken;       // Which token the player will be using
  RULE_PlayerColour eColour;      // The colour of the player
  BYTE              byAILevel;    // The strength of the AI
  BOOL              bTakeOverAI;  // indicates if the player is taking over
                                  // an AI controlled player
};


// Stores info for the Add New Player screen
struct PlayerCfgScreenDataStruct
{
  BOOL            bActive;    // Indicates the player config screen is active
  LE_DATA_DataId  idBackDrop; // Backdrop of the configuration screen
  PlayerStruct    player;

  RECT            rctNameField;
  RECT            rctTokenBtn;
  RECT            rctClrBtn;
  RECT            rctAIStatusBtn;
  RECT            rctAIStrengthBtn;
  RECT            rctOKBtn;
  RECT            rctCancelBtn;

  LE_DATA_DataId  idNameField;
  BtnStruct       aTokenBtns[MAX_TOKENS];
  BtnStruct       aClrBtns[MAX_PLAYER_COLOURS];
  BtnStruct       AIStatusBtn;
  BtnStruct       aAIStrengthBtns[3];
  BtnStruct       OKBtn;
  BtnStruct       CancelBtn;
};


// ===========================================================================
// GLOBALS ===================================================================
// ===========================================================================

extern PlayerCfgScreenDataStruct    g_PlayerCfgScreen;


// ===========================================================================
// Function prototypes =======================================================
// ===========================================================================


extern BOOL DISPLAY_UDPLRCFG_Initialize();
extern void DISPLAY_UDPLRCFG_Show();
extern void DISPLAY_UDPLRCFG_Destroy();

extern void UDPLRCFG_SetPlayerInfo(const RULE_PlayerInfoRecord& plyrInfo,
                                   int nPlayerNum);

extern void UDPLRCFG_ProcessMessage(LE_QUEUE_MessagePointer pUIMessage);


// ===========================================================================
// DEFINES ===================================================================
// ===========================================================================



#define PLRCFG_BASE_PRIORITY                1050

#define PLYRCFG_BACKDROP                    TAB_psaddplr

#define NUM_TOKENS                          MAX_TOKENS
#define BASE_TOKENBTN_ONIMAGE               TAB_psentk00
#define BASE_TOKENBTN_PRESSEDIMAGE          TAB_psentk00

#define NUM_COLOURS                         MAX_PLAYER_COLOURS
#define BASE_CLRBTN_ONIMAGE                 TAB_psencl00
#define BASE_CLRBTN_PRESSEDIMAGE            TAB_psencl00

#define AISTATUSBTN_ONIMAGE                 TAB_psenaino
#define AISTATUSBTN_OFFIMAGE                TAB_psenaiye

#define BASE_AISTRENGTHBTN_ONIMAGE          TAB_psenaie
#define BASE_AISTRENGTHBTN_PRESSEDIMAGE     TAB_psenaie

#define OKBTN_OFFIMAGE                      0
#define OKBTN_PRESSEDIMAGE                  0
#define CANCELBTN_OFFIMAGE                  0
#define CANCELBTN_PRESSEDIMAGE              0



#endif // INC_UDPLRCFG