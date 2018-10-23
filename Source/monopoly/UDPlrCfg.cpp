

// ===========================================================================
// Module:      PlyrCfg.cpp
//
// Description: Implementation of the functions for managing the Player
//              Configuration screen.
//
// Created:     March 18, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================
/*
	*
	*
	*
	*
	*	1. Modified May 26 1999
	*	by Davide
	*	Added the ability to use the shift key and the caps lock
	*
	*/
#include "GameInc.h"
#include "UDPlrCfg.h"
#include "UDPlrSum.h"

#if !FORNEWPLAYERSELECTSTUFF


// ===========================================================================
// Global variables ==========================================================
// ===========================================================================

PlayerCfgScreenDataStruct g_PlayerCfgScreen = 
{
  FALSE,              // bActive
  LE_DATA_EmptyItem,  // idBackDrop
  {                   // player structure
    {                 // player.szName
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    },
    0,                // player.number
    TK_GUN,           // player.eToken
    PC_RED,           // player.eColour
    0,                // player.byAILevel
    FALSE             // player.bTakeOverAI
  },

  { 153,  33, 514,  70 },   // rctNameField
  { 148,  94, 244, 139 },   // rctTokenBtn
  { 403,  94, 499, 139 },   // rctClrBtn
  {  70, 164, 182, 205 },   // rctAIStatusBtn
  { 188, 239, 293, 280 },   // rctAIStrengthBtn
  {  26, 348, 149, 399 },   // rctOKBtn
  { 335, 350, 516, 402 },   // rctCancelBtn

  LE_DATA_EmptyItem,        // idNameField
  {                         // aTokenBtns[11]
    EMPTYBUTTON, EMPTYBUTTON, EMPTYBUTTON, EMPTYBUTTON, EMPTYBUTTON,
    EMPTYBUTTON, EMPTYBUTTON, EMPTYBUTTON, EMPTYBUTTON, EMPTYBUTTON, EMPTYBUTTON
  },
  {                         // aClrBtns[6]
    EMPTYBUTTON, EMPTYBUTTON, EMPTYBUTTON,
    EMPTYBUTTON, EMPTYBUTTON, EMPTYBUTTON
  },
  EMPTYBUTTON,              // AIStatusBtn
  {                         // aAIStrengthBtns[3]
    EMPTYBUTTON, EMPTYBUTTON, EMPTYBUTTON
  },
  EMPTYBUTTON,              // OKBtn
  EMPTYBUTTON               // CancelBtn
};

static const int g_akdwAIStrengths[] = { TAB_psenaie, TAB_psenaim, TAB_psenaih };

static const char g_akszDefaultNames[MAX_TOKENS][RULE_MAX_PLAYER_NAME_LENGTH] = 
{
  "Cannon",
  "Race car",
  "Dog",
  "Top hat",
  "Iron",
  "Horse",
  "Battleship",
  "Shoe",
  "Thimble",
  "Barrow",
  "Money bag"
};

static BOOL g_bUserEnteredName = FALSE;



// ===========================================================================
// Local functions not meant to be called by other modules ===================
// ===========================================================================

void DisplayPlayerConfig();
void HidePlayerConfig();

void AddLetterToNameField(short nLetter);
void RemoveLetterFromNameField();
void UpdateNameField();

RULE_TokenKind    DetermineNextAvailableToken(RULE_TokenKind eCurrentToken);
RULE_PlayerColour DetermineNextAvailableColour(RULE_PlayerColour eCurrentColour);

// Button message handlers ==================================================
void OnTokenButton();
void OnClrButton();
void OnAIStatusButton();
void OnAIStrengthButton();
void OnOKButton();
void OnCancelButton();


// ===========================================================================
// FUNCTIONS =================================================================
// ===========================================================================


// ===========================================================================
// Function:    DISPLAY_UDPLRCFG_Initialize
//
// Description: Initializes the content for the Player Configuration screen
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if everything is initialized properly and FALSE
//              otherwise.
// ===========================================================================
BOOL DISPLAY_UDPLRCFG_Initialize()
{
  // Create ourselves an object to hold the name field
  if (LE_DATA_EmptyItem == g_PlayerCfgScreen.idNameField)
  {
    g_PlayerCfgScreen.idNameField = LE_GRAFIX_ObjectCreate(
                                      (short)(g_PlayerCfgScreen.rctNameField.right -
                                      g_PlayerCfgScreen.rctNameField.left),
                                      (short)(g_PlayerCfgScreen.rctNameField.bottom - 
                                      g_PlayerCfgScreen.rctNameField.top),
                                      LE_GRAFIX_ObjectTransparent);
    _ASSERT(LE_DATA_EmptyItem != g_PlayerCfgScreen.idNameField);
    if (LE_DATA_EmptyItem == g_PlayerCfgScreen.idNameField) return(FALSE);
  }

  // Load the images for the token button
  for (int i = 0; i < NUM_TOKENS; i++)
  {
    // Load the regular image
    if (!LoadDataFileImage(UI_DATAFILE, BASE_TOKENBTN_ONIMAGE + i,
                           g_PlayerCfgScreen.aTokenBtns[i].idImage))
    {
      return(FALSE);
    }

    // Load the pressed image
    if (!LoadDataFileImage(UI_DATAFILE, BASE_TOKENBTN_PRESSEDIMAGE + i,
                           g_PlayerCfgScreen.aTokenBtns[i].idPressedImage))
    {
      return(FALSE);
    }
  }

  // Load the images for the colour button
  for (i = 0; i < NUM_COLOURS; i++)
  {
    // Load the regular image
    if (!LoadDataFileImage(UI_DATAFILE, BASE_CLRBTN_ONIMAGE + i,
                           g_PlayerCfgScreen.aClrBtns[i].idImage))
    {
      return(FALSE);
    }

    // Load the pressed image
    if (!LoadDataFileImage(UI_DATAFILE, BASE_CLRBTN_PRESSEDIMAGE + i,
                           g_PlayerCfgScreen.aClrBtns[i].idPressedImage))
    {
      return(FALSE);
    }
  }

  // Load the AI Status button on image
  if (!LoadDataFileImage(DAT_LANG2, AISTATUSBTN_ONIMAGE,
                         g_PlayerCfgScreen.AIStatusBtn.idImage))
  {
    return(FALSE);
  }

  // Load the AI Status pressed image
  if (!LoadDataFileImage(DAT_LANG2, AISTATUSBTN_OFFIMAGE,
                         g_PlayerCfgScreen.AIStatusBtn.idPressedImage))
  {
    return(FALSE);
  }

  // Load the images for the colour button
  for (i = 0; i < 3; i++)
  {
    // Load the regular image
    if (!LoadDataFileImage(DAT_LANG2, g_akdwAIStrengths[i],
                           g_PlayerCfgScreen.aAIStrengthBtns[i].idImage))
    {
      return(FALSE);
    }

    // Load the pressed image
//    if (!LoadDataFileImage(DAT_LANG2, BASE_AISTRENGTHBTN_PRESSEDIMAGE + i,
//                           g_PlayerCfgScreen.aAIStrengthBtns[i].idPressedImage))
//    {
//      return(FALSE);
//    }
  }

  // Load the backdrop for the screen
  if (!LoadDataFileImage(DAT_LANG2, PLYRCFG_BACKDROP,
                         g_PlayerCfgScreen.idBackDrop))
  {
    return(FALSE);
  }

  g_bUserEnteredName = FALSE;

  return(TRUE);
}


// ===========================================================================
// Function:    DISPLAY_UDPLRCFG_Show
//
// Description: Display the player configuration screen
//
// Params:      
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DISPLAY_UDPLRCFG_Show()
{
  if (DISPLAY_SCREEN_PselectLocalNetwork != DISPLAY_state.desired2DView &&
      g_PlayerCfgScreen.bActive)
  {
    HidePlayerConfig();
    return;
  }

  if (ST_PLAYERCONFIG == g_eActiveScreen && !g_PlayerCfgScreen.bActive)
    DisplayPlayerConfig();
}


// ===========================================================================
// Function:    DISPLAY_UDPLRCFG_Destroy
//
// Description: Closes and cleans up any resources allocated by the player
//              configuration screen.
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DISPLAY_UDPLRCFG_Destroy()
{
  // Free up the name field
  if (LE_DATA_EmptyItem != g_PlayerCfgScreen.idNameField)
  {
    LE_DATA_FreeRuntimeDataID(g_PlayerCfgScreen.idNameField);
    g_PlayerCfgScreen.idNameField = LE_DATA_EmptyItem;
  }
}


// ===========================================================================
// Function:    PlrCfg_ProcessMessage
//
// Description: Message processor for the Player Configuration screen
//
// Params:      pUIMessage:   Pointer to the UI Message structure representing
//                            the event that was generated.
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void UDPLRCFG_ProcessMessage(LE_QUEUE_MessagePointer pUIMessage)
{
  // If this isn't the active dialog, we don't want to do anything
  if (ST_PLAYERCONFIG != g_eActiveScreen) return;
  
  // Check mouse stuff
  if (UIMSG_MOUSE_LEFT_DOWN == pUIMessage->messageCode)
  {
    POINT ptMousePos = { pUIMessage->numberA, pUIMessage->numberB };

    // Figure out if we are over a button
    if (PtInRect(&g_PlayerCfgScreen.rctTokenBtn, ptMousePos)) 
      OnTokenButton();
    else if (PtInRect(&g_PlayerCfgScreen.rctClrBtn, ptMousePos))
      OnClrButton();
    else if (PtInRect(&g_PlayerCfgScreen.rctAIStatusBtn, ptMousePos))
      OnAIStatusButton();
    else if (PtInRect(&g_PlayerCfgScreen.rctAIStrengthBtn, ptMousePos))
      OnAIStrengthButton();
    else if (PtInRect(&g_PlayerCfgScreen.rctOKBtn, ptMousePos))
      OnOKButton();
    else if (PtInRect(&g_PlayerCfgScreen.rctCancelBtn, ptMousePos))
      OnCancelButton();
  }

  // Now check keyboard stuff
  if (UIMSG_KEYBOARD_PRESSED == pUIMessage->messageCode)
  {
    if (LE_KEYBRD_A <= pUIMessage->numberA &&
        LE_KEYBRD_Z >= pUIMessage->numberA)
    {
			BOOL state1 = FALSE, state2 = FALSE;
			

			if(GetKeyState(VK_CAPITAL) & 0x01)		// get caps lock setting
				state1 = TRUE;
			if(GetKeyState(VK_SHIFT) & 0x80)					// is shift key pressed?
				state2 = TRUE;
			
			// Letter keys get processed as input to the name field
			if(state1 ^ state2)     
				AddLetterToNameField((short)(pUIMessage->numberA + 'A' - LE_KEYBRD_A));
			else
				AddLetterToNameField((short)(pUIMessage->numberA + 'a' - LE_KEYBRD_A));
    }
    else if (LE_KEYBRD_SPACE == pUIMessage->numberA)
      AddLetterToNameField((short)(' '));

    else if (LE_KEYBRD_BACK == pUIMessage->numberA)
    {
      // Backspace removes the last character of the player's name
      RemoveLetterFromNameField();
    }
  }  

}


// ===========================================================================
// Function:    UDPLRCFG_SetPlayerInfo
//
// Description: Sets the player info for the player configuration screen
//
// Params:      playerInfo:   A structure containing info on the player
//              nPlayerNum:   The player's number
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void UDPLRCFG_SetPlayerInfo(const RULE_PlayerInfoRecord& playerInfo,
                            int nPlayerNum)
{
  ZeroMemory(g_PlayerCfgScreen.player.szName, sizeof(wchar_t) * RULE_MAX_PLAYER_NAME_LENGTH);
  g_PlayerCfgScreen.player.eToken = DetermineNextAvailableToken(playerInfo.token);
  g_PlayerCfgScreen.player.eColour = DetermineNextAvailableColour(playerInfo.colour);

  // Set the player's name to a default value
  mbstowcs(g_PlayerCfgScreen.player.szName,
           g_akszDefaultNames[g_PlayerCfgScreen.player.eToken],
           RULE_MAX_PLAYER_NAME_LENGTH);
  
  g_PlayerCfgScreen.player.byAILevel      = playerInfo.AIPlayerLevel;
  g_PlayerCfgScreen.player.nPlayerNumber  = nPlayerNum;
}


// ===========================================================================
// Function:    DisplayPlayerConfig
//
// Description: Displays the player configuration screen
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DisplayPlayerConfig()
{
  // Show the backdrop
  LE_SEQNCR_StartXY(g_PlayerCfgScreen.idBackDrop, PLRCFG_BASE_PRIORITY - 1, 0, 0);

  // Start the name field
  _ASSERT(LE_DATA_EmptyItem != g_PlayerCfgScreen.idNameField);
  LE_SEQNCR_StartXY(g_PlayerCfgScreen.idNameField,
                    PLRCFG_BASE_PRIORITY, g_PlayerCfgScreen.rctNameField.left,
                    g_PlayerCfgScreen.rctNameField.top);

  // Now update the name field
  UpdateNameField();

  // Token button
  LE_SEQNCR_StartXY(g_PlayerCfgScreen.aTokenBtns[g_PlayerCfgScreen.player.eToken].idImage,
                    PLRCFG_BASE_PRIORITY, g_PlayerCfgScreen.rctTokenBtn.left,
                    g_PlayerCfgScreen.rctTokenBtn.top);

  // Colour button
  LE_SEQNCR_StartXY(g_PlayerCfgScreen.aClrBtns[g_PlayerCfgScreen.player.eColour].idImage,
                    PLRCFG_BASE_PRIORITY, g_PlayerCfgScreen.rctClrBtn.left,
                    g_PlayerCfgScreen.rctClrBtn.top);

  // AI Status button
  LE_SEQNCR_StartXY(g_PlayerCfgScreen.AIStatusBtn.idImage,
                    PLRCFG_BASE_PRIORITY, g_PlayerCfgScreen.rctAIStatusBtn.left,
                    g_PlayerCfgScreen.rctAIStatusBtn.top);

  g_bUserEnteredName                  = FALSE;
  g_eActiveScreen                     = ST_PLAYERCONFIG;
  g_PlayerCfgScreen.bActive           = TRUE;
  g_PlayerCfgScreen.player.byAILevel  = 0;
}


// ===========================================================================
// Function:    HidePlayerConfig
//
// Description: Hides the player configuration screen
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void HidePlayerConfig()
{
  // Hide the backdrop
  LE_SEQNCR_Stop(g_PlayerCfgScreen.idBackDrop, PLRCFG_BASE_PRIORITY - 1);
  LE_SEQNCR_Stop(g_PlayerCfgScreen.idNameField, PLRCFG_BASE_PRIORITY);
  LE_SEQNCR_Stop(g_PlayerCfgScreen.aTokenBtns[g_PlayerCfgScreen.player.eToken].idImage,
                 PLRCFG_BASE_PRIORITY);
  LE_SEQNCR_Stop(g_PlayerCfgScreen.aClrBtns[g_PlayerCfgScreen.player.eColour].idImage,
                 PLRCFG_BASE_PRIORITY);


  // If the AI Strength button is not visible, don't bother doing anything
  if (0 < g_PlayerCfgScreen.player.byAILevel)
  {
    // Stop the sequence for the AI Strength selection
    LE_SEQNCR_Stop(g_PlayerCfgScreen.AIStatusBtn.idPressedImage,
                   PLRCFG_BASE_PRIORITY);

    // Stop the sequence for the current token selection
    LE_SEQNCR_Stop(g_PlayerCfgScreen.aAIStrengthBtns[g_PlayerCfgScreen.player.byAILevel - 1].idImage,
                   PLRCFG_BASE_PRIORITY);
  }
  else
  {
    // Stop the sequence for the AI Strength selection
    LE_SEQNCR_Stop(g_PlayerCfgScreen.AIStatusBtn.idImage, PLRCFG_BASE_PRIORITY);
  }

  g_PlayerCfgScreen.bActive = FALSE;
  g_bUserEnteredName        = FALSE;
}


// ===========================================================================
// Function:    AddLetterToNameField
//
// Description: Adds a letter to the name field of the player and updates it
//              on the screen
//
// Params:      nLetter:  The letter to add
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void AddLetterToNameField(short nLetter)
{
  // Make sure there is enough room to add the new character
  int nLength = wcslen(g_PlayerCfgScreen.player.szName);
  if (RULE_MAX_PLAYER_NAME_LENGTH <= nLength)
    return;

  if (!g_bUserEnteredName)
  {
    // Since the user hasn't yet entered anything, clear the name field
    ZeroMemory(g_PlayerCfgScreen.player.szName, nLength);
    // Add the letter
    g_PlayerCfgScreen.player.szName[0] = nLetter;
    g_PlayerCfgScreen.player.szName[1] = 0;
  }
  else
  {
    // Add the letter
    g_PlayerCfgScreen.player.szName[nLength]      = nLetter;
    g_PlayerCfgScreen.player.szName[nLength + 1]  = 0;
  }
  g_bUserEnteredName = TRUE;

  UpdateNameField();
}


// ===========================================================================
// Function:    RemoveLetterFromNameField
//
// Description: Removes the last character of the name field and updates the
//              screen
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void RemoveLetterFromNameField()
{
  // Make sure there is at least one character in the name
  int nLength = wcslen(g_PlayerCfgScreen.player.szName);
  if (0 == nLength) return;

  if (!g_bUserEnteredName)
  {
    // Since the user hasn't yet entered anything, clear the name field
    ZeroMemory(g_PlayerCfgScreen.player.szName, nLength);
  }
  else
  {
    // Remove only the last character
    g_PlayerCfgScreen.player.szName[nLength - 1] = 0;
  }

  g_bUserEnteredName = TRUE;

  UpdateNameField();
}


// ===========================================================================
// Function:    UpdateNameField
//
// Description: Updates the display of the name field
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void UpdateNameField()
{
  // Clear the old field
  LE_GRAFIX_ColorArea(g_PlayerCfgScreen.idNameField, 0, 0,
                      (short)(g_PlayerCfgScreen.rctNameField.right -
                      g_PlayerCfgScreen.rctNameField.left),
                      (short)(g_PlayerCfgScreen.rctNameField.bottom -
                      g_PlayerCfgScreen.rctNameField.top), LEG_MCR(0, 255, 0));

  // Now write in the updated text
  LANG_Print(g_PlayerCfgScreen.idNameField, 0, 0, LEG_MCR(255, 255, 255),
             g_PlayerCfgScreen.player.szName);
  LE_SEQNCR_ForceRedraw(g_PlayerCfgScreen.idNameField, PLRCFG_BASE_PRIORITY);
}


// ===========================================================================
// Function:    DetermineNextAvailableToken
//
// Description: Determines which tokens are available to be selected and
//              returns the next available one.
//
// Params:      eCurrentToken:  The current token we are using
//
// Returns:     RULE_TokenKind
//
// Comments:    Returns the next available token that has not yet been
//              selected.
// ===========================================================================
RULE_TokenKind DetermineNextAvailableToken(RULE_TokenKind eCurrentToken)
{
  // Determine the next token in the list
  RULE_TokenKind nToken = (eCurrentToken + 1) % MAX_TOKENS;

  // If there aren't any players yet, return the first token
  if (0 == UICurrentGameState.NumberOfPlayers)
    return(nToken);

  BOOL bTokenFound = FALSE;

  while (!bTokenFound)
  {
     // Is nToken available?  Check if any of the players have selected it.
    for (int i = 0; i < UICurrentGameState.NumberOfPlayers; i++)
      if (UICurrentGameState.Players[i].token == nToken) break;

    // If we iterated through all of the current players, and no one has this
    //  token, we can safely return this token as being available
    if (i == UICurrentGameState.NumberOfPlayers)
      return(nToken);

    // Move to the next token
    nToken = (nToken + 1) % MAX_TOKENS;

    // If we come back to the token we started with, it must be the only one
    //  that is available
    if (nToken == eCurrentToken)
      bTokenFound = TRUE;
  }

  // In the unexpected event that we aren't able to find an available token
  //  we will just return the one we were given
  return(eCurrentToken);
}


// ===========================================================================
// Function:    DetermineNextAvailableColour
//
// Description: Determines the next avaliable colour, i.e. one that has not
//              already been chosen by another player
//
// Params:      eCurrentColour: The current colour
//
// Returns:     RULE_PlayerColour
//
// Comments:    Returns the next available colour.
// ===========================================================================
RULE_PlayerColour DetermineNextAvailableColour(RULE_PlayerColour eCurrentColour)
{
  // Determine the next token in the list
  RULE_PlayerColour nClr = (eCurrentColour + 1) % NUM_COLOURS/*MAX_PLAYER_COLOURS*/;

  // If there aren't any players yet, return the first token
  if (0 == UICurrentGameState.NumberOfPlayers)
    return(nClr);

  BOOL bClrFound = FALSE;

  while (!bClrFound)
  {
    // Is nToken available?  Check if any of the players have selected it.
    for (int i = 0; i < UICurrentGameState.NumberOfPlayers; i++)
      if (UICurrentGameState.Players[i].colour == nClr) break;

    // If we iterated through all of the current players, and no one has this
    //  token, we can safely return this token as being available
    if (i == UICurrentGameState.NumberOfPlayers)
      return(nClr);

    // Move to the next token
    nClr = (nClr + 1) % NUM_COLOURS/*MAX_PLAYER_COLOURS*/;

    // If we have come back to the same colour as we started with, it must be
    //  the only one that is available
    if (nClr == eCurrentColour) 
      bClrFound = TRUE;
  }

  // In the unexpected event that we aren't able to find an available colour
  //  we will just return the one we were given
  return(eCurrentColour);
}


// ===========================================================================
// Input handling messages ===================================================
// ===========================================================================


// ===========================================================================
// Function:    OnTokenButton
//
// Description: Called when the token button is pressed
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void OnTokenButton()
{
  // Stop the sequence for the current token selection
  LE_SEQNCR_Stop(g_PlayerCfgScreen.aTokenBtns[g_PlayerCfgScreen.player.eToken].idImage,
                 PLRCFG_BASE_PRIORITY);

  // Set the player token to the next one in the cycle
  g_PlayerCfgScreen.player.eToken = 
    DetermineNextAvailableToken(g_PlayerCfgScreen.player.eToken);

  if (!g_bUserEnteredName)
  {
    // The user hasn't entered their own name so we will provide a default
    mbstowcs(g_PlayerCfgScreen.player.szName,
             g_akszDefaultNames[g_PlayerCfgScreen.player.eToken],
             RULE_MAX_PLAYER_NAME_LENGTH);
    UpdateNameField();
  }

  // Set the button to display the next available token
  LE_SEQNCR_StartXY(g_PlayerCfgScreen.aTokenBtns[g_PlayerCfgScreen.player.eToken].idImage,
                    PLRCFG_BASE_PRIORITY,
                    g_PlayerCfgScreen.rctTokenBtn.left,
                    g_PlayerCfgScreen.rctTokenBtn.top);
}


// ===========================================================================
// Function:    OnClrButton
//
// Description: Called when the color button is pressed
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void OnClrButton()
{
  // Stop the sequence for the current token selection
  LE_SEQNCR_Stop(g_PlayerCfgScreen.aClrBtns[g_PlayerCfgScreen.player.eColour].idImage,
                 PLRCFG_BASE_PRIORITY);

  // Set the player token to the next one in the cycle
  g_PlayerCfgScreen.player.eColour = 
    DetermineNextAvailableColour(g_PlayerCfgScreen.player.eColour);

  // Set the button to display the next available token
  LE_SEQNCR_StartXY(g_PlayerCfgScreen.aClrBtns[g_PlayerCfgScreen.player.eColour].idImage,
                    PLRCFG_BASE_PRIORITY, g_PlayerCfgScreen.rctClrBtn.left,
                    g_PlayerCfgScreen.rctClrBtn.top);
}


// ===========================================================================
// Function:    OnAIStatusButton
//
// Description: Called when the AI Status button is pressed
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void OnAIStatusButton()
{
  if (0 == g_PlayerCfgScreen.player.byAILevel)
  {
    // Stop the sequence for the AI Strength selection
    LE_SEQNCR_Stop(g_PlayerCfgScreen.AIStatusBtn.idImage, PLRCFG_BASE_PRIORITY);

    g_PlayerCfgScreen.player.byAILevel = 1;

    // Show the toggled button and the AI Strength button
    LE_SEQNCR_StartXY(g_PlayerCfgScreen.AIStatusBtn.idPressedImage,
                      PLRCFG_BASE_PRIORITY, g_PlayerCfgScreen.rctAIStatusBtn.left,
                      g_PlayerCfgScreen.rctAIStatusBtn.top);

    LE_SEQNCR_StartXY(g_PlayerCfgScreen.aAIStrengthBtns[g_PlayerCfgScreen.player.byAILevel - 1].idImage,
                      PLRCFG_BASE_PRIORITY, g_PlayerCfgScreen.rctAIStrengthBtn.left,
                      g_PlayerCfgScreen.rctAIStrengthBtn.top);

  }
  else
  {
    // Stop the sequence for the AI Strength selection
    LE_SEQNCR_Stop(g_PlayerCfgScreen.AIStatusBtn.idPressedImage,
                   PLRCFG_BASE_PRIORITY);

    LE_SEQNCR_Stop(g_PlayerCfgScreen.aAIStrengthBtns[g_PlayerCfgScreen.player.byAILevel - 1].idImage,
                   PLRCFG_BASE_PRIORITY);

    g_PlayerCfgScreen.player.byAILevel = 0;

    // Show the toggled button and the AI Strength button
    LE_SEQNCR_StartXY(g_PlayerCfgScreen.AIStatusBtn.idImage, 
                      PLRCFG_BASE_PRIORITY,
                      g_PlayerCfgScreen.rctAIStatusBtn.left,
                      g_PlayerCfgScreen.rctAIStatusBtn.top);
  }
}


// ===========================================================================
// Function:    OnAIStrengthButton
//
// Description: Called when the AI Strength button is pressed
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void OnAIStrengthButton()
{
  // If the AI Strength button is not visible, don't bother doing anything
  if (0 == g_PlayerCfgScreen.player.byAILevel) return;

  // Stop the sequence for the current token selection
  LE_SEQNCR_Stop(g_PlayerCfgScreen.aAIStrengthBtns[g_PlayerCfgScreen.player.byAILevel - 1].idImage,
                 PLRCFG_BASE_PRIORITY);

  // Set the player token to the next one in the cycle
  g_PlayerCfgScreen.player.byAILevel = 
    (g_PlayerCfgScreen.player.byAILevel + 1) % 4;
  if (0 == g_PlayerCfgScreen.player.byAILevel)
    g_PlayerCfgScreen.player.byAILevel = 1;

  // Set the button to display the next available token
  LE_SEQNCR_StartXY(g_PlayerCfgScreen.aAIStrengthBtns[g_PlayerCfgScreen.player.byAILevel - 1].idImage,
                    PLRCFG_BASE_PRIORITY, g_PlayerCfgScreen.rctAIStrengthBtn.left,
                    g_PlayerCfgScreen.rctAIStrengthBtn.top);
}


// ===========================================================================
// Function:    OnOKButton
//
// Description: Called when the OK button is pressed
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void OnOKButton()
{
  // Close the title screen and open the player summary screen
  g_eActiveScreen = g_ePreviousScreen = ST_PLAYERSUMMARY;
  HidePlayerConfig();

  // Add the player to the game engine
  AddLocalPlayer(g_PlayerCfgScreen.player.szName, 
                 g_PlayerCfgScreen.player.eToken,
                 g_PlayerCfgScreen.player.eColour,
                 g_PlayerCfgScreen.player.byAILevel,
                 g_PlayerCfgScreen.player.bTakeOverAI);

}


// ===========================================================================
// Function:    OnCancelButton
//
// Description: Called when the Cancel button is pressed
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void OnCancelButton()
{
  g_eActiveScreen = g_ePreviousScreen = ST_PLAYERSUMMARY;
  HidePlayerConfig();
}


#endif