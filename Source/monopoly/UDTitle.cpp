// ===========================================================================
// Module:      UDTitle.cpp
//
// Description: Main title screen implementation
//
// Created:     March 23, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================

#include "GameInc.h"
#include "UDTitle.h"
#include "UDPlrSum.h"

// ===========================================================================
// Global variables ==========================================================
// ===========================================================================

TitleScreenDataStruct g_TitleScreen = 
{
  FALSE,                  // bActive
  LE_DATA_EmptyItem,      // idBackDrop

  { 37, 100, 281, 148 },  // rctNewGame
  { 37, 159, 281, 207 },  // rctNetworkGame
  { 37, 217, 281, 265 },  // rctLoadGame
  { 37, 364, 281, 412 },  // rctExit

  EMPTYBUTTON,            // NewGameBtn
  EMPTYBUTTON,            // NetworkGameBtn
  EMPTYBUTTON,            // LoadGameBtn
  EMPTYBUTTON             // ExitBtn
};


// ===========================================================================
// Local functions not meant to be called by other modules ===================
// ===========================================================================

// Button message handlers ==================================================
void OnNewGameButton();
void OnNetworkGameButton();
void OnLoadGameButton();


// ===========================================================================
// FUNCTIONS =================================================================
// ===========================================================================


// ===========================================================================
// Function:    DISPLAY_UDTITLE_Initialize
//
// Description: Loads and initializes the contents of the title screen
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    
// ===========================================================================
BOOL DISPLAY_UDTITLE_Initialize()
{
  // Load the backdrop for the screen
  if (!LoadDataFileImage(DAT_LANG2, TITLE_BACKDROP, g_TitleScreen.idBackDrop))
    return(FALSE);

  return(TRUE);
}


// ===========================================================================
// Function:    DISPLAY_UDTITLE_Show
//
// Description: Shows or hides the title screen
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DISPLAY_UDTITLE_Show()
{
}


// ===========================================================================
// Function:    DISPLAY_UDTITLE_Destroy
//
// Description: 
//
// Params:      
//
// Returns:     
//
// Comments:    
// ===========================================================================
void DISPLAY_UDTITLE_Destroy()
{
}


// ===========================================================================
// Function:    UDTITLE_ProcessMessage
//
// Description: Processes the user messages passed in by the message router
//
// Params:      pUIMessage:   Pointer to a message structure
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void UDTITLE_ProcessMessage(LE_QUEUE_MessagePointer pUIMessage)
{
#if !FORNEWPLAYERSELECTSTUFF
  // If this isn't the active dialog, we don't want to do anything
  if (ST_TITLE != g_eActiveScreen) return;
#endif


  if (UIMSG_MOUSE_LEFT_DOWN == pUIMessage->messageCode)
  {
    // Any click, for now.
    OnNewGameButton();
  }

  if (UIMSG_MOUSE_RIGHT_DOWN == pUIMessage->messageCode)
  {
    // Any click, for now.
    OnLoadGameButton();
  }
}


// ===========================================================================
// Function:    OnNewGameButton
//
// Description: Message handler called when the New Game button is pressed
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void OnNewGameButton()
{
#if FORNEWPLAYERSELECTSTUFF
  UDBOARD_SetBackdrop( DISPLAY_SCREEN_PselectToken );
#else
  g_eActiveScreen = g_ePreviousScreen = ST_PLAYERSUMMARY;
#endif
}


// ===========================================================================
// Function:    OnNetworkGameButton
//
// Description: Message handler for when the Network Game button is pressed
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void OnNetworkGameButton()
{
  static ACHAR* szCommandLine = A_T("-directplay");

  ShowCursor(TRUE);
  MESS_StartNetworking(szCommandLine);
  ShowCursor(FALSE);

  OnNewGameButton();
}


// ===========================================================================
// Function:    OnLoadGameButton
//
// Description: Message handler for when the Load Game button is pressed
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void OnLoadGameButton()
{
  // Show the window's system cursor
  ShowCursor(TRUE);

  // Load a game
  LoadGameOrOptions(TRUE);

  // Hide the window's system cursor
  ShowCursor(FALSE);
}

