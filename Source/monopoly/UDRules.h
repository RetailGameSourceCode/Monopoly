// ===========================================================================
// Module:      UDRules.h
//
// Description: Rules options screen for the player selection.  Header file
//              definition stuff.
//
// Created:     March 31, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================


#ifndef INC_UDRULES
#define INC_UDRULES


// ===========================================================================
// Structures and other types ================================================
// ===========================================================================


struct RulesScreenDataStruct
{
  BOOL            bActive;
  LE_DATA_DataId  idBackDrop;           // Backdrop of the rules screen
//  LE_DATA_DataId  aidFields[][];        // IDs of the fields
//  int             nSelectedField;       // Indicates which field is selected

  RECT            rctAccept;
  RECT            rctDecline;

  BtnStruct       AcceptBtn;
  BtnStruct       DeclineBtn;
};


// ===========================================================================
// GLOBALS ===================================================================
// ===========================================================================

extern RulesScreenDataStruct    g_RulesScreen;

// ===========================================================================
// Function prototypes =======================================================
// ===========================================================================

extern BOOL DISPLAY_UDRULES_Initialize();
extern void DISPLAY_UDRULES_Show();
extern void DISPLAY_UDRULES_Destroy();

extern void UDRULES_ProcessMessage(LE_QUEUE_MessagePointer pUIMessage);


// ===========================================================================
// Defines ===================================================================
// ===========================================================================

#define RULES_BASE_PRIORITY       1000

#define RULES_BACKDROP            TAB_psrules



#endif // INC_UDRULES
