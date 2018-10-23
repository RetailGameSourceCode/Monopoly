// ===========================================================================
// Module:      UDPlrSum.h
//
// Description: Player summary screen header stuff
//
// Created:     March 23, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================

#ifndef INC_UDPLRSUM
#define INC_UDPLRSUM

struct PlayerSumScreenDataStruct
{
  BOOL            bActive;
  LE_DATA_DataId  idBackDrop;                 // Backdrop of the summary screen
  LE_DATA_DataId  aidFields[RULE_MAX_PLAYERS][5]; // IDs of the fields
  int             nSelectedPlayer;

  RECT            rctAddPlayer;
  RECT            rctRemovePlayer;
//  RECT            rctRules;
  RECT            rctStart;
//  RECT            rctChatBox;

  BtnStruct       AddPlayerBtn;
  BtnStruct       RemovePlayerBtn;
//  BtnStruct       RulesBtn;
  BtnStruct       StartBtn;
};

// ===========================================================================
// GLOBALS ===================================================================
// ===========================================================================

extern PlayerSumScreenDataStruct    g_PlayerSumScreen;

// ===========================================================================
// Function prototypes =======================================================
// ===========================================================================


extern BOOL DISPLAY_UDPLRSUM_Initialize();
extern void DISPLAY_UDPLRSUM_Show();
extern void DISPLAY_UDPLRSUM_Destroy();

extern void UDPLRSUM_ProcessMessage(LE_QUEUE_MessagePointer pUIMessage);
extern void UDPLRSUM_Update();


extern void DisplayCurrentPlayers();

// ===========================================================================
// Defines ===================================================================
// ===========================================================================

#define PLRSUM_BASE_PRIORITY        1000

#define PLRSUM_BACKDROP             TAB_psenplbi

#define EMPTYROW { LE_DATA_EmptyItem, LE_DATA_EmptyItem, LE_DATA_EmptyItem, \
                   LE_DATA_EmptyItem, LE_DATA_EmptyItem }

#endif // INC_UDPLRSUM
