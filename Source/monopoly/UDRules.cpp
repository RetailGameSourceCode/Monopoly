// ===========================================================================
// Module:      UDRules.cpp
//
// Description: Rules options screen for the player selection.  Functional
//              implmentation.
//
// Created:     March 31, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================
/*
	*
	*
	*
	*	1. May 27 1999
	*			modified by Davide
	*			quickly added a token to know which player is accepting the configurations
	*
	*
	*/
#include "GameInc.h"

// ===========================================================================
// Global variables ==========================================================
// ===========================================================================

RulesScreenDataStruct g_RulesScreen = 
{
  FALSE,                  // bActive
  LE_DATA_EmptyItem,      // idBackDrop

  { 447, 435, 568, 479 }, // rctAccept
  { 584, 435, 706, 479 }, // rctCancel

  EMPTYBUTTON,            // AcceptBtn
  EMPTYBUTTON             // CancelBtn
};

// ===========================================================================
// Internal module function prototypes =======================================
// ===========================================================================

void HideRules();
void DisplayRules();

void OnAcceptButton();
void OnDeclineButton();

// ===========================================================================
// FUNCTIONS =================================================================
// ===========================================================================


// ===========================================================================
// Function:    DISPLAY_UDRULES_Initialize
//
// Description: Initializes all of the components of the rules options screen
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the screen was properly initialized and FALSE
//              otherwise
// ===========================================================================
BOOL DISPLAY_UDRULES_Initialize()
{
  // Load the backdrop for the screen
  if (!LoadDataFileImage(DAT_LANG2, RULES_BACKDROP, g_RulesScreen.idBackDrop))
    return(FALSE);

  return(TRUE);
}


// ===========================================================================
// Function:    DISPLAY_UDRULES_Show
//
// Description: Shows or hides the Rules options screen depending on the state
//              of the display
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DISPLAY_UDRULES_Show()
{
#if !FORNEWPLAYERSELECTSTUFF
  if (/*DISPLAY_SCREEN_PselectStart != DISPLAY_state.desired2DView*/ST_GAME == g_eActiveScreen &&
      g_RulesScreen.bActive)
  {
    HideRules();
    return;
  }

  if (ST_RULESOPTIONS == g_eActiveScreen && !g_RulesScreen.bActive)
    DisplayRules();
#endif
}


// ===========================================================================
// Function:    DISPLAY_UDRULES_Destroy
//
// Description: 
//
// Params:      
//
// Returns:     
//
// Comments:    
// ===========================================================================
void DISPLAY_UDRULES_Destroy()
{

}


// ===========================================================================
// Function:    UDRULES_ProcessMessage
//
// Description: Processes any user input messages routed to the rules options
//              screen.
//
// Params:      pUIMessage:   Pointer to a UIMessage structure
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void UDRULES_ProcessMessage(LE_QUEUE_MessagePointer pUIMessage)
{
#if !FORNEWPLAYERSELECTSTUFF
  // If this isn't the active dialog, we don't want to do anything
  if (ST_RULESOPTIONS != g_eActiveScreen)
    return;

  // Check for mouse input
  if (UIMSG_MOUSE_LEFT_DOWN == pUIMessage->messageCode)
  {
    POINT ptMousePos = { pUIMessage->numberA, pUIMessage->numberB };

    // Figure out if we are over a button
    if (PtInRect(&g_RulesScreen.rctAccept, ptMousePos)) 
      OnAcceptButton();
    if (PtInRect(&g_RulesScreen.rctDecline, ptMousePos)) 
      OnDeclineButton();
  }
#endif
}


// ===========================================================================
// Function:    HideRules
//
// Description: Hides the rules options screen.
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void HideRules()
{
  // Show the backdrop
  LE_SEQNCR_Stop(g_RulesScreen.idBackDrop, RULES_BASE_PRIORITY - 1);

  g_RulesScreen.bActive = FALSE;

	// stop display of configring icon
	if(CurrentToken > -1)
	{
#if !FORNEWPLAYERSELECTSTUFF
		LE_SEQNCR_Stop(g_PlayerCfgScreen.aTokenBtns[CurrentToken].idImage,
										RULES_BASE_PRIORITY +1);
#endif
  }
}


// ===========================================================================
// Function:    DisplayRules
//
// Description: Displays the rules options screen and makes it active
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DisplayRules()
{
  // Show the backdrop
  LE_SEQNCR_StartXY(g_RulesScreen.idBackDrop, RULES_BASE_PRIORITY - 1, 0, 0);

  g_RulesScreen.bActive = TRUE;

	// for now, display an icon to determin who is accepting
#if !FORNEWPLAYERSELECTSTUFF
	if(CurrentToken > -1)
		LE_SEQNCR_StartXY(g_PlayerCfgScreen.aTokenBtns[CurrentToken].idImage,
						RULES_BASE_PRIORITY +1, 300, 200);
#endif
}


// ===========================================================================
// Message handling functions ================================================
// ===========================================================================


// ===========================================================================
// Function:    OnAcceptButton
//
// Description: Message handler called when the Accept button is clicked
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void OnAcceptButton()
{
  RULE_ActionArgumentsRecord msg;

  ZeroMemory(&msg, sizeof(RULE_ActionArgumentsRecord));
  msg.action      = /*ACTION_ACCEPT_CONFIGURATION*/NOTIFY_ACCEPT_CONFIGURATION;
  msg.fromPlayer  = CurrentUIPlayer;
  msg.toPlayer    = /*RULE_BANK_PLAYER*/RULE_NOBODY_PLAYER;
  msg.numberC     = 1;

  if (AddGameOptionsToMessageBlob(&UICurrentGameState.GameOptions, &msg))
    MESS_SendActionMessage(&msg);

	// global used for switching screens with F8 
	CurrentToken = -1;
	
}


// ===========================================================================
// Function:    OnDeclineButton
//
// Description: Message handler called when the Cancel button is clicked
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void OnDeclineButton()
{
  UDSOUND_Warning();      
}