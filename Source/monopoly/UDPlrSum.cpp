

// ===========================================================================
// Module:      UDPlrSum.cpp
//
// Description: Player summary screen implementation
//
// Created:     March 23, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================

#include "GameInc.h"

#if !FORNEWPLAYERSELECTSTUFF


// ===========================================================================
// Global variables ==========================================================
// ===========================================================================

PlayerSumScreenDataStruct g_PlayerSumScreen = 
{
  FALSE,                  // bActive
  LE_DATA_EmptyItem,      // idBackDrop

  { EMPTYROW, EMPTYROW, EMPTYROW, EMPTYROW, EMPTYROW, EMPTYROW }, // aidFields
  -1,                     // nSelectedPlayer

  {  17, 492, 166, 536 }, // rctAddPlayer
  { 169, 492, 351, 536 }, // rctRemovePlayer
//  { 462, 492, 583, 536 }, // rctRules
  { 589, 492, 716, 536 }, // rctStart

  EMPTYBUTTON,            // AddPlayerBtn
  EMPTYBUTTON,            // RemovePlayerBtn
//  EMPTYBUTTON             // RulesBtn
  EMPTYBUTTON             // StartBtn
};

  // Column positions for the different player stats
static const int g_aknColPos[]    = {  17, 254, 356, 462, 589 };
static const int g_aknFldWidths[] = { 232,  97,  97, 122, 128 };
static const int g_knStartRow     = 61;
static const int g_knRowHeight    = 46;


// ===========================================================================
// Internal module function prototypes =======================================
// ===========================================================================

void DisplayPlayerSummary();
void HidePlayerSummary();
void DisplayCurrentPlayers();

void SelectPlayer(int nPlayerIdx);
void ClearRow(int nRow);

// Button message handlers ===================================================
void OnAddPlayerButton();
void OnRemovePlayerButton();
//void OnRulesButton()
void OnStartButton();


// ===========================================================================
// FUNCTIONS =================================================================
// ===========================================================================


// ===========================================================================
// Function:    DISPLAY_UDPLRSUM_Initialize
//
// Description: Initializes the contents of the player summary screen
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
BOOL DISPLAY_UDPLRSUM_Initialize()
{
  // Load the backdrop for the screen
  if (!LoadDataFileImage(DAT_LANG2, PLRSUM_BACKDROP, 
      g_PlayerSumScreen.idBackDrop))
  {
    return(FALSE);
  }

  // Create the objects for the fields
  for (int i = 0; i < RULE_MAX_PLAYERS; i++)
  {
    for (int j = 0; j < 5; j++)
    {
      _ASSERT(LE_DATA_EmptyItem == g_PlayerSumScreen.aidFields[i][j]);

      // Create the object
      g_PlayerSumScreen.aidFields[i][j] = 
        LE_GRAFIX_ObjectCreate(g_aknFldWidths[j], g_knRowHeight,
                               LE_GRAFIX_ObjectTransparent);
      _ASSERT(LE_DATA_EmptyItem != g_PlayerSumScreen.aidFields[i][j]);
      if (LE_DATA_EmptyItem == g_PlayerSumScreen.aidFields[i][j]) return(FALSE);

      // Clear the object
      LE_GRAFIX_ColorArea(g_PlayerSumScreen.aidFields[i][j], 0, 0,
                          g_aknFldWidths[j], g_knRowHeight, LEG_MCR(0, 255, 0));
    }
  }

  return(TRUE);
}


// ===========================================================================
// Function:    DISPLAY_UDPLRSUM_Show
//
// Description: Shows the player summary screen
//
// Params:      bShow:  Indicates whether the screen is supposed to be shown
//                      or hidden
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DISPLAY_UDPLRSUM_Show()
{
  if (DISPLAY_SCREEN_PselectLocalNetwork != DISPLAY_state.desired2DView &&
      g_PlayerSumScreen.bActive)
  {
    HidePlayerSummary();
    return;
  }

  if (ST_PLAYERSUMMARY == g_eActiveScreen && !g_PlayerSumScreen.bActive)
    DisplayPlayerSummary();
}


// ===========================================================================
// Function:    DISPLAY_UDPLRSUM_Destroy
//
// Description: Destroys the player summary screen
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DISPLAY_UDPLRSUM_Destroy()
{
  // Free up the field stuff
  for (int i = 0; i < RULE_MAX_PLAYERS; i++)
  {
    for (int j = 0; j < 5; j++)
    {
      _ASSERT(LE_DATA_EmptyItem != g_PlayerSumScreen.aidFields[i][j]);
      LE_DATA_FreeRuntimeDataID(g_PlayerSumScreen.aidFields[i][j]);
      g_PlayerSumScreen.aidFields[i][j] = LE_DATA_EmptyItem;
    }
  }
  g_PlayerSumScreen.bActive = FALSE;
}


// ===========================================================================
// Function:    UDPLRSUM_ProcessMessage
//
// Description: Processes the messages passed to the player summary screen
//
// Params:      pUIMessage:   Pointer to a UI message structure
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void UDPLRSUM_ProcessMessage(LE_QUEUE_MessagePointer pUIMessage)
{
  // If this isn't the active dialog, we don't want to do anything
  if (ST_PLAYERSUMMARY != g_eActiveScreen)
    return;

  // Check for mouse input
  if (UIMSG_MOUSE_LEFT_DOWN == pUIMessage->messageCode)
  {
    POINT ptMousePos = { pUIMessage->numberA, pUIMessage->numberB };

    // Figure out if we are over a button
    if (PtInRect(&g_PlayerSumScreen.rctAddPlayer, ptMousePos)) 
      OnAddPlayerButton();
    else if (PtInRect(&g_PlayerSumScreen.rctRemovePlayer, ptMousePos))
      OnRemovePlayerButton();
    else if (PtInRect(&g_PlayerSumScreen.rctStart, ptMousePos))
      OnStartButton();
    else
    {
      // Check for a player selection
      RECT rct = { g_aknColPos[0], g_knStartRow,
                   g_aknColPos[0] + g_aknFldWidths[0],
                   g_knStartRow + g_knRowHeight };

      BOOL bPlayerSelected = FALSE;
      
      for (int i = 0; i < UICurrentGameState.NumberOfPlayers; i++)
      {
        rct.top     = g_knStartRow + (i * g_knRowHeight);
        rct.bottom  = g_knStartRow + ((i + 1) * g_knRowHeight) - 1;

        if (PtInRect(&rct, ptMousePos))
        {
          SelectPlayer(i);
          bPlayerSelected = TRUE;
          break;
        }
      }

      if (!bPlayerSelected) SelectPlayer(-1);
    }
  }

  // Now check for keyboard input
  if (UIMSG_KEYBOARD_PRESSED == pUIMessage->messageCode)
  {
    if (LE_KEYBRD_1 == pUIMessage->numberA)
    {
      // Select the first player
      SelectPlayer(0);
    }
  }
}


// ===========================================================================
// Function:    UDPLRSUM_Update
//
// Description: Updates the state of the player summary screen.  Ensures that
//              the players who have been added to the game are displayed
//              properly on the screen.
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void UDPLRSUM_Update()
{
  if (g_PlayerSumScreen.bActive && ST_PLAYERSUMMARY == g_eActiveScreen)
    DisplayCurrentPlayers();
}


// ===========================================================================
// Function:    DisplayPlayerSummary
//
// Description: Displays the player summary screen
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DisplayPlayerSummary()
{
  // Show the backdrop
  LE_SEQNCR_StartXY(g_PlayerSumScreen.idBackDrop, PLRSUM_BASE_PRIORITY - 1, 0, 0);

  // Start up the fields
  for (int i = 0; i < RULE_MAX_PLAYERS; i++)
  {
    for (int j = 0; j < 5; j++)
    {
      _ASSERT(LE_DATA_EmptyItem != g_PlayerSumScreen.aidFields[i][j]);
      LE_SEQNCR_StartXY(g_PlayerSumScreen.aidFields[i][j], PLRSUM_BASE_PRIORITY,
                        g_aknColPos[j], g_knStartRow + (i * g_knRowHeight));
    }
  }

  g_PlayerSumScreen.bActive = TRUE;

  // Make sure the contents of the screen are updated
  UDPLRSUM_Update();
}


// ===========================================================================
// Function:    HidePlayerSummary
//
// Description: Hides the player summary screen
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void HidePlayerSummary()
{
  // Show the backdrop
  LE_SEQNCR_Stop(g_PlayerSumScreen.idBackDrop, PLRSUM_BASE_PRIORITY - 1);

  // Start up the name fields
  for (int i = 0; i < RULE_MAX_PLAYERS; i++)
  {
    for (int j = 0; j < 5; j++)
    {
      _ASSERT(LE_DATA_EmptyItem != g_PlayerSumScreen.aidFields[i][j]);
      LE_SEQNCR_Stop(g_PlayerSumScreen.aidFields[i][j],
                     PLRSUM_BASE_PRIORITY);
    }
  }

  g_PlayerSumScreen.bActive = FALSE;
}


// ===========================================================================
// Function:    DisplayCurrentPlayers
//
// Description: Displays the current list of players who have joined the game
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DisplayCurrentPlayers()
{
  for (int i = 0; i < RULE_MAX_PLAYERS; i++)
  {
    // If there are no more players, just clear the next rows
    if (i >= UICurrentGameState.NumberOfPlayers)
    {
      ClearRow(i);
      continue;
    }

    // Display the player's name, token, colour, AI status, and AI strength
    if (i == g_PlayerSumScreen.nSelectedPlayer)
    {
      // Clear the name field, with selection highlight
      LE_GRAFIX_ColorArea(g_PlayerSumScreen.aidFields[i][0], 0, 0,
                          g_aknFldWidths[0], g_knRowHeight, LEG_MCR(0, 128, 255));
    }
    else
    {
      // Clear the name field
      LE_GRAFIX_ColorArea(g_PlayerSumScreen.aidFields[i][0], 0, 0,
                          g_aknFldWidths[0], g_knRowHeight, LEG_MCR(0, 255, 0));
    }

    // Now write in the updated text
    LANG_Print(g_PlayerSumScreen.aidFields[i][0], 5, 7, LEG_MCR(0, 0, 0),
               UICurrentGameState.Players[i].name);
    LE_SEQNCR_ForceRedraw(g_PlayerSumScreen.aidFields[i][0],
                          PLRSUM_BASE_PRIORITY);

    // Show the token
    LE_GRAFIX_ShowTCB(g_PlayerSumScreen.aidFields[i][1], 0, 0, 
                      g_PlayerCfgScreen.aTokenBtns[UICurrentGameState.Players[i].token].idImage);
    LE_SEQNCR_ForceRedraw(g_PlayerSumScreen.aidFields[i][1],
                          PLRSUM_BASE_PRIORITY);

    // Show the colour
    LE_GRAFIX_ShowTCB(g_PlayerSumScreen.aidFields[i][2], 0, 0, 
                      g_PlayerCfgScreen.aClrBtns[UICurrentGameState.Players[i].colour].idImage);
    VERIFY(ChangeObjectTransparency(g_PlayerSumScreen.aidFields[i][2], FALSE));
    LE_SEQNCR_ForceRedraw(g_PlayerSumScreen.aidFields[i][2],
                          PLRSUM_BASE_PRIORITY);

    // Show the AI Status
    LE_GRAFIX_ShowTCB(g_PlayerSumScreen.aidFields[i][3], 0, 0, 
                      0 == UICurrentGameState.Players[i].AIPlayerLevel ? 
                      g_PlayerCfgScreen.AIStatusBtn.idImage : 
                      g_PlayerCfgScreen.AIStatusBtn.idPressedImage);
    LE_SEQNCR_ForceRedraw(g_PlayerSumScreen.aidFields[i][3],
                          PLRSUM_BASE_PRIORITY);

    // Show the AI Strength
    if (0 != UICurrentGameState.Players[i].AIPlayerLevel)
    {
      LE_GRAFIX_ShowTCB(g_PlayerSumScreen.aidFields[i][4], 0, 0, 
                        g_PlayerCfgScreen.aAIStrengthBtns[UICurrentGameState.Players[i].AIPlayerLevel - 1].idImage);
      LE_SEQNCR_ForceRedraw(g_PlayerSumScreen.aidFields[i][4],
                            PLRSUM_BASE_PRIORITY);
    }
  }
}


// ===========================================================================
// Function:    SelectPlayer
//
// Description: Selects a player from the list of players added to the game.
//
// Params:      nPlayerIdx: The index of the player to select
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void SelectPlayer(int nPlayerIdx)
{
  // Deselect the old selected player
  if (-1 < g_PlayerSumScreen.nSelectedPlayer)
  {
    LE_GRAFIX_ColorArea(g_PlayerSumScreen.aidFields[g_PlayerSumScreen.nSelectedPlayer][0],
                        0, 0, g_aknFldWidths[0], g_knRowHeight, LEG_MCR(0, 255, 0));
    // Write in the updated text
    LANG_Print(g_PlayerSumScreen.aidFields[g_PlayerSumScreen.nSelectedPlayer][0],
               5, 7, LEG_MCR(0, 0, 0),
               UICurrentGameState.Players[g_PlayerSumScreen.nSelectedPlayer].name);
    LE_SEQNCR_ForceRedraw(g_PlayerSumScreen.aidFields[g_PlayerSumScreen.nSelectedPlayer][0],
                          PLRSUM_BASE_PRIORITY);
  }

  // Can't select someone who doesn't exist,  so select no one
  if (nPlayerIdx >= UICurrentGameState.NumberOfPlayers ||
      0 > nPlayerIdx)
  {
    // Deselect any currently selected player
    g_PlayerSumScreen.nSelectedPlayer = -1;
    return;
  }

  // Set the new selected player
  g_PlayerSumScreen.nSelectedPlayer = nPlayerIdx;

  // Now change the background so it looks like the player is selected
  LE_GRAFIX_ColorArea(g_PlayerSumScreen.aidFields[nPlayerIdx][0], 0, 0,
                      g_aknFldWidths[0], g_knRowHeight, LEG_MCR(0, 128, 255));

  // Now write in the updated text
  LANG_Print(g_PlayerSumScreen.aidFields[nPlayerIdx][0], 5, 7, LEG_MCR(0, 0, 0),
             UICurrentGameState.Players[nPlayerIdx].name);
  LE_SEQNCR_ForceRedraw(g_PlayerSumScreen.aidFields[nPlayerIdx][0],
                        PLRSUM_BASE_PRIORITY);
}


// ===========================================================================
// Function:    ClearRow
//
// Description: Clears all of the fields of a particular player (row)
//
// Params:      nRow:   The row of fields to clear
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void ClearRow(int nRow)
{
  for (int nCol = 0; nCol < 5; nCol++)
  {
    // If this is the colour column we need to make sure that it is
    //  transparent
    if (2 == nCol)
      ChangeObjectTransparency(g_PlayerSumScreen.aidFields[nRow][nCol], TRUE);

    // Clear the field
    LE_GRAFIX_ColorArea(g_PlayerSumScreen.aidFields[nRow][nCol], 0, 0,
                        g_aknFldWidths[nCol], g_knRowHeight, LEG_MCR(0, 255, 0));

    // Redraw the field
    if (ST_PLAYERSUMMARY == g_eActiveScreen && g_PlayerSumScreen.bActive)
    {
      LE_SEQNCR_ForceRedraw(g_PlayerSumScreen.aidFields[nRow][nCol],
                            PLRSUM_BASE_PRIORITY);
    }
  }
}

// ===========================================================================
// Message handling functions ================================================
// ===========================================================================


// ===========================================================================
// Function:    OnAddPlayerButton
//
// Description: Message handler called when the Add Player button is pressed
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void OnAddPlayerButton()
{
  // We can only add a player if there is a player slot left
  if (RULE_MAX_PLAYERS > UICurrentGameState.NumberOfPlayers)
  {
    // Hide this screen
    HidePlayerSummary();

    if (0 == UICurrentGameState.NumberOfPlayers)
    {
      UICurrentGameState.Players[0].token = TK_MONEYBAG;
      UICurrentGameState.Players[0].colour = PC_ORANGE;
    }

    // Initialize the new screen
    UDPLRCFG_SetPlayerInfo(UICurrentGameState.Players[UICurrentGameState.NumberOfPlayers],
                           UICurrentGameState.NumberOfPlayers);
    g_eActiveScreen = ST_PLAYERCONFIG;
  }
  else
  {
    // There are already the maximum number of players allowed

  }
}


// ===========================================================================
// Function:    OnRemovePlayerButton
//
// Description: Message handler called when the Remove Player button is pressed
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void OnRemovePlayerButton()
{
  // If there is no selected player, we can't remove them
  if (-1 == g_PlayerSumScreen.nSelectedPlayer) return;

  // Clear the player's name.  This will tell the rules engine to remove
  //  the player.
  ZeroMemory(UICurrentGameState.Players[g_PlayerSumScreen.nSelectedPlayer].name,
             sizeof(wchar_t) * RULE_MAX_PLAYER_NAME_LENGTH);

  // Ask the rules engine to remove the player
  MESS_SendAction(ACTION_NAME_PLAYER, RULE_SPECTATOR_PLAYER, RULE_BANK_PLAYER,
    g_PlayerSumScreen.nSelectedPlayer,
    UICurrentGameState.Players[g_PlayerSumScreen.nSelectedPlayer].AIPlayerLevel,
    UICurrentGameState.Players[g_PlayerSumScreen.nSelectedPlayer].token,
    UICurrentGameState.Players[g_PlayerSumScreen.nSelectedPlayer].colour,
    UICurrentGameState.Players[g_PlayerSumScreen.nSelectedPlayer].name,
    0, NULL);

  // Reset the selection
  g_PlayerSumScreen.nSelectedPlayer = -1;
}


// ===========================================================================
// Function:    OnStartButton
//
// Description: Message handler called when the start button is pressed
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void OnStartButton()
{
  // We only want to start the game if there have been at least two players
  //  added
  if (2 <= UICurrentGameState.NumberOfPlayers)
  {
    // Close the player summary screen and start the game
    HidePlayerSummary();
    g_eActiveScreen = g_ePreviousScreen = ST_RULESOPTIONS;
    DISPLAY_state.CurrentDialog = DISPLAY_Dialog_AcceptConfig;

    MESS_SendAction(ACTION_START_GAME, AnyLocalPlayer(), RULE_BANK_PLAYER,
                    0, 0, 0, 0, NULL, 0, NULL);
  }
}


#endif