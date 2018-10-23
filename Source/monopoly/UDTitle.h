// ===========================================================================
// Module:      UDTitle.h
//
// Description: Main title screen header stuff
//
// Created:     March 23, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================

#ifndef INC_UDTITLE
#define INC_UDTITLE

// ===========================================================================
// Structures and enums ======================================================
// ===========================================================================

struct TitleScreenDataStruct
{
  BOOL            bActive;
  LE_DATA_DataId  idBackDrop; // Backdrop of the title screen

  RECT            rctNewGame;
  RECT            rctNetworkGame;
  RECT            rctLoadGame;
  RECT            rctExit;

  BtnStruct       NewGameBtn;
  BtnStruct       NetworkGameBtn;
  BtnStruct       LoadGameBtn;
  BtnStruct       ExitBtn;
};

// ===========================================================================
// GLOBALS ===================================================================
// ===========================================================================

extern TitleScreenDataStruct    g_TitleScreen;

// ===========================================================================
// Function prototypes =======================================================
// ===========================================================================


extern BOOL DISPLAY_UDTITLE_Initialize();
extern void DISPLAY_UDTITLE_Show();
extern void DISPLAY_UDTITLE_Destroy();

extern void UDTITLE_ProcessMessage(LE_QUEUE_MessagePointer pUIMessage);


// ===========================================================================
// Defines ===================================================================
// ===========================================================================

#define TITLE_BASE_PRIORITY     1000

#define TITLE_BACKDROP          TAB_psentboi

#endif // INC_UDTITLE