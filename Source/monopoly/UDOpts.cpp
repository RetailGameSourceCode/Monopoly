/*****************************************************************************
 *
 * FILE:        UDOPTS.cpp, subfile of Display.cpp & UserIfce.cpp
 * DESCRIPTION: Handles display & User input specific to game options.
 *
 * © Copyright 1999 Artech Digital Entertainments.  All rights reserved.
 *****************************************************************************/


#include "gameinc.h"
#include <io.h>
#include <fcntl.h>

#if CE_ARTLIB_3DUsingOldFrame
#define USE_OLD_FRAME
#endif

#include "..\PC3D\PC3DHdr.h"
#include "..\pc3d\l_material.h"

#if CE_ARTLIB_3DUsingOldFrame

#include "..\pc3d\hmdstuff.h"
#include "..\pc3d\hmdload.h"
typedef meshx     MESHType;
#define SETTIMBANK  HMD_SetCurrentTimBank

// PC3d hack
extern "C" void HMD_SetCurrentTimBank( int bank );

#else // CE_ARTLIB_3DUsingOldFrame

#include "..\PC3D\GlobalData.h"
#include "..\Pc3D\NewMesh.h"
typedef Mesh     MESHType;
#define SETTIMBANK  SetCurrentTIMBank

#endif // CE_ARTLIB_3DUsingOldFrame

//#include "DDrawDrv.h"
//#include "DDDrvMgr.h"

#include "..\pc3D\DXINI.h"

/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

#define DIALOG_X            200
#define DIALOG_Y            50
#define MAX_STRING_LENGTH   500

// value to hold the security number that was created randomly when the board editor was installed
DWORD						g_SecurityNum = 0;

// buttons for all the options screens
MenuOptionsButton       FileButtons[5], HelpButtons[3], OptionButtons[2];
// boolean to determine if one of the options screen is on
BOOL                    g_bOptionsScreenOn = FALSE;
// boolean to determine if the 4 menu option buttons are displayed
BOOL                    g_bOptionsButtonsOn = TRUE;
int                     last_playerselectphase = -1;
int                     last_playerselectscreen = -1;

// the object that will hold the title for each of the menu option screens
BasicDataIDStruct       OptionsTitle;
LE_DATA_DataId          SoundSubTitleID, DisplaySubTitleID;

// objects that represent the 9 on/off buttons of the OPTION menu option
BasicDataIDStruct       OptionOnButtons[10], OptionOffButtons[10];
// x and y coordinates for the on/off buttons
int                     OnOffXPos[2] = {25, 457};
int                     OnOffYPos[7] = {135, 175, 215, 255, 295, 335, 375};
// booleans to determine if each of the onoff buttons are on or off
BOOL                    IsOnOffButtonON[10];
// objects that will display the text for each of the on/off buttons
BasicDataIDStruct       OptionChoices[10];
// temporary strings that will be printed next to the on/off buttons
ACHAR                   szOptionChoices[10][100];
// objects that will display the tune choices for the background music
BasicDataIDStruct       MusicChoices[5];
// temp variable to hold the music tune index until user clicks OK
int                     tempMusicTuneIndex = 0;
ACHAR                   szMusicChoices[5][100] = {{A_T("Let's Play Monopoly!")}, {A_T("Railroad Serenade")},
                             {A_T("Take a Chance")}, {A_T("Park Place Stroll")}, {A_T("C Notes")}};
// objects that will display the screen resolution choices for the game
BasicDataIDStruct       ScreenResChoices[5];
int                     ChosenWidth[5], ChosenHeight[5], ChosenBit[2];
ACHAR                   szResolutionChoices[5][50];
int                     g_MaxResChoices = 0;
// temp variable to hold the resolution index until user clicks OK
int                     tempResIndex = 0;
// same as above but for the 16 or 32 bit choice
// objects that will display the screen resolution choices for the game
BasicDataIDStruct       BitChoices[2];
ACHAR                   szBitChoices[2][10];
int                     g_MaxBitChoices[5] = {0,0,0,0,0};
// temp variable to hold the resolution index until user clicks OK
int                     tempBitIndex = 0;

// object that will display the credits
BasicDataIDStruct       CreditsObject;

// variable that will hold the quick help
ACHAR                   *szQuickHelp = 0;
BasicDataIDStruct       HelpObject, HelpTextButtons[3];
int                     HelpTextPageNo = 0;                 // keeps track of which page the help file is on
long            HelpTextCurrentHeight = 0;          
long            HelpTextOverallHeight = 0;
BOOL            IsQuickHelpTextFound = FALSE;
int             HelpTextPageNoArray[100];
// set the next, previous and cancel buttons for the help text
ACHAR           szText[3][100];

// new load/save dialog box
BasicDataIDStruct           LoadGameTitle, SaveGameTitle, LoadBoardTitle;
BasicDataIDStruct           GameSlots[5];
BasicDataIDStruct           LoadDialogBox, SaveDialogBox, LoadBoardDialog;
ACHAR                       GameFileNames[5][100] = {{A_T("game1.msv")}, {A_T("game2.msv")},
                                        {A_T("game3.msv")}, {A_T("game4.msv")}, {A_T("game5.msv")}};
ACHAR                       BoardFileNames[100][100];
char                        szBoardFilePath[500];
SaveGameStruct              SaveGameStuff[5];
//ACHAR                       GameFileDesc[5][100];
ACHAR                       szTempGameFileDesc[100];
RECT                        GameFileNameRects[5];
RECT                        BoardFileNameRects[50];
int                         MaxGameFileNames;
int                         MaxBoardFileNames;
int                         CurrentGameFile, PreviousGameFile;
int                         CurrentBoardFile;
BasicDataIDStruct           DialogButtons[4];
BOOL                        IsLoadDialog = FALSE;
int                         g_BoardNameIndex = 0;


ACHAR                       szDialogText[4][100];

// flag used to determine if user hit the option exit button
BOOL                        g_UserRequestedExit = FALSE, g_UserRequestedNewGame = FALSE;
BOOL                        g_UserChoseToExit = FALSE;
// flag used to not process the mouse message for the options screen 
// when clicking yes to the Are you sure message
BOOL                        g_IsYesHitProcessed = TRUE;

// timing variables
TYPE_Tick FirstScrollTime = 0;
TYPE_Tick NewTime = 0;
TYPE_Tick DeltaTime = 0;





void UDOPTIONS_RemoveFileScreen(BOOL IfRemoveDialog);
void UDOPTIONS_RemoveOptionScreen(void);
void UDOPTIONS_RemoveCreditsScreen(void);
void UDOPTIONS_RemoveHelpScreen(BOOL HelpTextOn);
void UDOPTIONS_InitOptionsButtons(void);
void UDOPTIONS_InitOptionOptionsScreen(void);
void UDOPTIONS_InitCreditsOptionsScreen(void);
void UDOPTIONS_InitHelpOptionsScreen(void);
void UDOPTIONS_ReadQuickHelpFile(void);
void UDOPTIONS_DispalyQuickHelpFile(void);
void UDOPTIONS_RemoveQuickHelpFile(void);
void UDOPTIONS_InitCustomBoardDialogBoxes(void);
void UDOPTIONS_DisplayLoadDialogBox(void);
void UDOPTIONS_RemoveLoadDialogBox(void);
BOOL UDOPTIONS_LoadGame (int gameIndex);
void UDOPTIONS_DisplaySaveDialogBox(void);
void UDOPTIONS_RemoveSaveDialogBox(void);
void UDOPTIONS_DisplayBoardDialog(void);
void UDOPTIONS_RemoveBoardDialog(void);
void UDOPTIONS_ProcessLoadBoardDialogButtonPress(int button, short Area);
void UDOPTIONS_DisplayLoadGameScreen(int last_screen);
void UDOPTIONS_DisplayPrevious5Boards(void);
void UDOPTIONS_DisplayNext5Boards(void);
BOOL ReadSecurityNumber (void);
void UDOPTIONS_SetBackdrop(int last_screen );
void UDOPTIONS_DestoryOptionsScreenObjects(void);

/*****************************************************************************
 * All display initialization, showing, hiding and destroying occurs in
 * the following 4 subroutines called from DISPLAY.CPP.

 * No code other than the show module may start nor stop sound
 * and graphic items.  Fields indicating desired actions can be filled in
 * in the display state which the show routine will pick up.  In this way,
 * the show routine can take responsibility for tracking all data
 * required to pause or clear the screen at any time.

 * There will be no locking the computer as a wait for all sequences to finish
 * might do.  Stack processing can be halted until sequences are finished,
 * but you will have to use a timer to poll your sequence status.  This is to
 * allow processing of pause buttons or user camera controls at all times, as
 * well as to make programmers lives more difficult :)
 */

void DISPLAY_UDOPTS_Initialize( void )
{ // Initialize any permanent display objects.  Avoid temporary ones when possible.

    // init the options buttons
    UDOPTIONS_InitOptionsButtons();

    // init option options screen
    UDOPTIONS_InitOptionOptionsScreen();
        
    // init credits options screen
    UDOPTIONS_InitCreditsOptionsScreen();

    // init help options screen
    UDOPTIONS_InitHelpOptionsScreen();

    // initilize load/save dialog boxes
    UDOPTIONS_InitCustomBoardDialogBoxes();     
    
}


void DISPLAY_UDOPTS_Destroy( void )
{ // Deallocate stuff from Initialize.  It is safe to assume Hide has been called.

    UDOPTIONS_DestoryOptionsScreenObjects();

    // free the buffer holding the quick help
    if(szQuickHelp)
    {
        delete (szQuickHelp);
        szQuickHelp = 0;
    }

}


void DISPLAY_UDOPTS_Show( void )
{ // Recalculate what should be on the screen, compare to what is on the screen,
  // and take corrective action.  Everything is in the user interface and display states.

    

    if((DISPLAY_state.desired2DView == DISPLAY_SCREEN_Options) && !g_bOptionsScreenOn)
    {
        //remove the last options screen
        UDOPTIONS_RemoveLastOptionsScreen(last_options_result);
        last_options_result = current_options_result;
        //determine which screen we are displaying
        switch(current_options_result)
        {
        case FILE_SCREEN:       UDOPTIONS_DisplayFileScreen(current_options_result);    break;
        case OPTION_SCREEN:     UDOPTIONS_DisplayOptionScreen(current_options_result);  break;
        case CREDITS_SCREEN:    UDOPTIONS_DisplayCreditsScreen(current_options_result); break;
        case HELP_SCREEN:       UDOPTIONS_DisplayHelpScreen(current_options_result);    break;
        case LOAD_BOARD_SCREEN: UDOPTIONS_DisplayLoadBoardScreen(current_options_result);    break;
        case LOAD_GAME_SCREEN:  UDOPTIONS_DisplayLoadGameScreen(current_options_result);    break;
        }
        g_bOptionsScreenOn = TRUE;
        
    }
    else if((DISPLAY_state.desired2DView != DISPLAY_SCREEN_Options) && g_bOptionsScreenOn)
    {
        //remove the last options screen
        UDIBar_ProcessOptionsButtonPress(-1);
        UDOPTIONS_RemoveLastOptionsScreen(last_options_result);
        last_playerselectphase = -1;
        //last_playerselectscreen = -1;
        g_bOptionsScreenOn = FALSE; 
        
    }
    else if((DISPLAY_state.desired2DView == DISPLAY_SCREEN_Options) && g_bOptionsScreenOn
            && (current_options_result == CREDITS_SCREEN))
    {
      static TYPE_Tick oldDelta;
        NewTime = LE_TIME_TickCount;
        oldDelta = DeltaTime;
        DeltaTime = (int)(NewTime - FirstScrollTime)/5;
        //if(DeltaTime > 3)
        if( DeltaTime != oldDelta )
        {
            
            LE_GRAFIX_ColorArea(CreditsObject.ID, 0, 0, CreditsObject.Width, CreditsObject.Height, 
                                LE_GRAFIX_MakeColorRef(0,255,0));
            // set the coordinates of the scrolling text
            CreditsObject.HotSpot.top = CreditsObject.Height - (DeltaTime*2); 
            CreditsObject.HotSpot.bottom = CreditsObject.HotSpot.top + 
                        LE_GRAFIX_ReturnObjectHeight(CreditsObject.extraID);;
            LE_GRAFIX_ShowTCB_Alpha(CreditsObject.ID, 0, (short)CreditsObject.HotSpot.top, CreditsObject.extraID); 
            LE_SEQNCR_ForceRedraw(CreditsObject.ID, CreditsObject.Priority);
            if(CreditsObject.HotSpot.bottom <= 0)
            {
                // set the scrolling time
                FirstScrollTime = LE_TIME_TickCount;
                CreditsObject.HotSpot.top = UDOPTSBarTop + 100;
                CreditsObject.HotSpot.bottom = CreditsObject.HotSpot.top
                                + LE_GRAFIX_ReturnObjectHeight(CreditsObject.extraID);
            }
        }
    }
}


/*****************************************************************************
 * Standard UI subfunction - Process library message.
 */

void UDOPTS_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr )
{
    int result;

    // check to see if we are exiting the game
    if(g_UserChoseToExit && ((UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN)
        || (UIMessagePntr->messageCode == UIMSG_KEYBOARD_PRESSED))
        && (g_IsYesHitProcessed))
    {
        UDOPTIONS_ProcessCreditsOptionButtonPress(-1);
        return;
    }


    if( (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN) &&
            ( DISPLAY_state.current2DView == DISPLAY_SCREEN_Options )
             && (g_IsYesHitProcessed))
    {
        if(g_bOptionsScreenOn)
        {
            if((result = UDOPTIONS_GetButtonIndex(UIMessagePntr, FILE_OPTION_AREA )) > -1)
            {
                UDOPTIONS_ProcessFileOptionButtonPress(result, FILE_OPTION_AREA);
            }
            else if((result = UDOPTIONS_GetButtonIndex(UIMessagePntr, FILE_LOAD_GAME_DIALOG_AREA )) > -1)
            {
                UDOPTIONS_ProcessFileOptionButtonPress(result, FILE_LOAD_GAME_DIALOG_AREA);
            }
            else if((result = UDOPTIONS_GetButtonIndex(UIMessagePntr, DIALOG_BUTTONS_AREA )) > -1)
            {
                UDOPTIONS_ProcessFileOptionButtonPress(result, DIALOG_BUTTONS_AREA);
            }
            else if((result = UDOPTIONS_GetButtonIndex(UIMessagePntr, OPTION_OPTION_AREA )) > -1)
            {
                UDOPTIONS_ProcessOptionOptionButtonPress(result, OPTION_OPTION_AREA);
            }
            else if((result = UDOPTIONS_GetButtonIndex(UIMessagePntr, OPTION_TUNES_AREA )) > -1)
            {
                UDOPTIONS_ProcessOptionOptionButtonPress(result, OPTION_TUNES_AREA);
            }
            else if((result = UDOPTIONS_GetButtonIndex(UIMessagePntr, OPTION_RESOLUTION_AREA )) > -1)
            {
                UDOPTIONS_ProcessOptionOptionButtonPress(result, OPTION_RESOLUTION_AREA);
            }
            else if((result = UDOPTIONS_GetButtonIndex(UIMessagePntr, OPTION_BIT_AREA )) > -1)
            {
                UDOPTIONS_ProcessOptionOptionButtonPress(result, OPTION_BIT_AREA);
            }
            else if((result = UDOPTIONS_GetButtonIndex(UIMessagePntr, OPTION_ONOFF_BUTTONS_AREA )) > -1)
            {
                UDOPTIONS_ProcessOptionOptionButtonPress(result, OPTION_ONOFF_BUTTONS_AREA);
            }
            else if((result = UDOPTIONS_GetButtonIndex(UIMessagePntr, CREDITS_OPTION_AREA )) > -1)
            {
                UDOPTIONS_ProcessCreditsOptionButtonPress(result);
            }
            else if((result = UDOPTIONS_GetButtonIndex(UIMessagePntr, HELP_OPTION_AREA )) > -1)
            {
                UDOPTIONS_ProcessHelpOptionButtonPress(result, HELP_OPTION_AREA);
            }
            else if((result = UDOPTIONS_GetButtonIndex(UIMessagePntr, HELP_TEXT_AREA )) > -1)
            {
                UDOPTIONS_ProcessHelpOptionButtonPress(result, HELP_TEXT_AREA);
            }
            else if((result = UDOPTIONS_GetButtonIndex(UIMessagePntr, LOAD_BOARD_DIALOG_AREA )) > -1)
            {
                UDOPTIONS_ProcessLoadBoardDialogButtonPress(result, LOAD_BOARD_DIALOG_AREA);
            }
            else if((result = UDOPTIONS_GetButtonIndex(UIMessagePntr, LOAD_BOARD_DIALOG_BUTTONS_AREA )) > -1)
            {
                UDOPTIONS_ProcessLoadBoardDialogButtonPress(result, LOAD_BOARD_DIALOG_BUTTONS_AREA);
            }
            
        }
        
            
    }

    g_IsYesHitProcessed = TRUE;

    if(((UIMessagePntr->messageCode == UIMSG_KEYBOARD_ACHAR) ||
        (UIMessagePntr->messageCode == UIMSG_KEYBOARD_PRESSED))
        && ( DISPLAY_state.desired2DView == DISPLAY_SCREEN_Options )
        && (GameSlots[CurrentGameFile].IsObjectOn) && !IsLoadDialog
        && !LoadBoardDialog.IsObjectOn)
    {
        LE_FONTS_GetSettings(3);

        ACHAR tempString[2] = {(ACHAR)(UIMessagePntr->numberA), NULL};
        ACHAR   szTemp[MAX_STRING_LENGTH];
        Astrcpy (szTemp, szTempGameFileDesc);

        short length = Astrlen(szTempGameFileDesc);

        
        if(UIMessagePntr->messageCode == UIMSG_KEYBOARD_ACHAR)
        {
            // Make sure it is a printable character.
            if (UIMessagePntr->numberA < 32 ||
                (UIMessagePntr->numberA >= 128 && UIMessagePntr->numberA < 128 + 32))
            {
                Astrcpy(tempString, A_T("")); // Not printable.
            }
                            

            if(LE_FONTS_GetStringWidth(szTempGameFileDesc) < (GameSlots[CurrentGameFile].Width - 50))                
            {
                Astrcat(szTempGameFileDesc, tempString);
                Astrcat(szTemp, tempString);
                Astrcat(szTemp, A_T("_"));
            }
            else
            {
                UDSOUND_Warning();
            }        
        }
        else if (UIMessagePntr->messageCode == UIMSG_KEYBOARD_PRESSED)
        {

            // verify to see if user entered the backspace key
            if(UIMessagePntr->numberA == LE_KEYBRD_BACK)        
            {
                if(length > 0)
                {
                    length--;
                    szTempGameFileDesc[length] = 0;
                    Astrcpy(szTemp, szTempGameFileDesc);
                }
                    Astrcat(szTemp, A_T("_"));
            
            }
            else if(UIMessagePntr->numberA == LE_KEYBRD_RET)
            {
                UDOPTIONS_ProcessFileOptionButtonPress(0, DIALOG_BUTTONS_AREA);
                return;
            }
        }
        
        
        
        // modify the description name
        LE_GRAFIX_ColorArea(GameSlots[CurrentGameFile].extraID, 0, 0, (short)GameSlots[CurrentGameFile].Width,
                GameSlots[CurrentGameFile].Height,  LE_GRAFIX_MakeColorRef(0,255,0));         
        LE_FONTS_Print(GameSlots[CurrentGameFile].extraID, 13, 8,
            LE_GRAFIX_MakeColorRef(255,255,255), szTemp);//GameFileNames[button]);
        
        LE_SEQNCR_ForceRedraw(GameSlots[CurrentGameFile].extraID, GameSlots[CurrentGameFile].Priority+1);
        
        
        LE_FONTS_GetSettings(0);
    }
    
}


/*****************************************************************************
 * Standard UI subfunction - Process game message to player.
 */

void UDOPTS_ProcessMessageToPlayer( RULE_ActionArgumentsPointer NewMessage )
{
}


/*****************************************************************************
    * Function:     short UDOPTIONS_GetButtonIndex(LE_QUEUE_MessagePointer UIMessagePntr, short Area )
    * Returns:      the index into the array of the button pressed
    * Parameters:   message pointer which holds the mouse coordinates
    *                           flag that determines if the mouse was clicked on the misc, calculator
    *                           or the token area.
    *                           
    * Purpose:      determine which button was pressed 
    ****************************************************************************
    */
short UDOPTIONS_GetButtonIndex(LE_QUEUE_MessagePointer UIMessagePntr, short Area )
{
    short i;
    POINT ptMousePos = { UIMessagePntr->numberA, UIMessagePntr->numberB };
    
    // verify for options category buttons
    if(Area == OPTIONS_BUTTONS_AREA)
    {
        if(g_UserChoseToExit)
            return -1;

        for(i=0; i<4; i++)
        {
            if((PtInRect(&(OptionsButton[i].HotSpot), ptMousePos)) &&
                (OptionsButton[i].State != ISTATBAR_BUTTON_Off))
            {
                return i;
            }
        }
    }

    
    // verify for file screen buttons
    if(Area == FILE_OPTION_AREA)
    {
        for(i=0; i<5; i++)
        {
            if((PtInRect(&(FileButtons[i].HotSpot), ptMousePos)) &&
                FileButtons[i].IsButtonOn)
            {
                return i;
            }
        }
    }

    // verify for load dialog area
    if(Area == FILE_LOAD_GAME_DIALOG_AREA)
    {
        for(i=0; i<5; i++)
        {
            if((PtInRect(&GameFileNameRects[i], ptMousePos)) &&
                (GameSlots[i].IsObjectOn) && 
                (SaveDialogBox.IsObjectOn || LoadDialogBox.IsObjectOn))
            {
                return i;
            }
        }
    }

        
    // verify for on buttons in the option option screen
    if(Area == OPTION_OPTION_AREA)
    {
        for(i=0; i<1; i++)
        {
            if((PtInRect(&(OptionButtons[i].HotSpot), ptMousePos)) &&
                OptionButtons[i].IsButtonOn)
            {
                return i;
            }

        }
    }


    // verify for credit option screen buttons
    if(Area == CREDITS_OPTION_AREA)
    {
        RECT tempRect = {0,0, LE_GRAFIX_ScreenDestinationRect.right, UDOPTSBarTop};
        
        if((PtInRect(&tempRect, ptMousePos)) && CreditsObject.IsObjectOn)
        {
            return 0;
        }
        
    }


    // verify for help option screen buttons
    if(Area == HELP_OPTION_AREA)
    {
        for(i=0; i<3; i++)
        {
            if((PtInRect(&(HelpButtons[i].HotSpot), ptMousePos)) &&
                HelpButtons[i].IsButtonOn)
            {
                return i;
            }
        }
    }
    

    // verify for on/off buttons in the option option screen
    if(Area == OPTION_ONOFF_BUTTONS_AREA)
    {
        for(i=0; i<9; i++)
        {
            if((PtInRect(&(OptionOnButtons[i].HotSpot), ptMousePos)) &&
                OptionOnButtons[i].IsObjectOn)
            {
                return i;
            }
            else if((PtInRect(&(OptionOffButtons[i].HotSpot), ptMousePos)) &&
                OptionOffButtons[i].IsObjectOn)
            {
                return i;
            }

        }
    }


    // verify for tune selection area the option option screen
    if(Area == OPTION_TUNES_AREA)
    {
        for(i=0; i<MAX_TUNES; i++)
        {
            if((PtInRect(&(MusicChoices[i].HotSpot), ptMousePos)) &&
                MusicChoices[i].IsObjectOn)
            {
                return i;
            }

        }
    }

    // verify for screen resolution selection area the option option screen
    if(Area == OPTION_RESOLUTION_AREA)
    {
        for(i=0; i<g_MaxResChoices; i++)
        {
            if((PtInRect(&(ScreenResChoices[i].HotSpot), ptMousePos)) &&
                ScreenResChoices[i].IsObjectOn)
            {
                return i;
            }

        }
    }

    // verify for screen resolution selection area the option option screen
    if(Area == OPTION_BIT_AREA)
    {
        for(i=0; i<2; i++)
        {
            if((PtInRect(&(BitChoices[i].HotSpot), ptMousePos)) &&
                BitChoices[i].IsObjectOn)
            {
                return i;
            }

        }
    }
    
    // verify for the previous, next and cancel buttons in the help text file
    if(Area == HELP_TEXT_AREA)
    {
        for(i=0; i<MAX_HELP_TEXT_BUTTONS; i++)
        {
            if((PtInRect(&(HelpTextButtons[i].HotSpot), ptMousePos)) &&
                HelpTextButtons[i].IsObjectOn)
            {
                return i;
            }
        }
    }


    // verify for the ok and cancel buttons in the dialog box text file
    if(Area == DIALOG_BUTTONS_AREA)
    {
        for(i=0; i<2; i++)
        {
            if((PtInRect(&(DialogButtons[i].HotSpot), ptMousePos)) &&
                (DialogButtons[i].IsObjectOn &&
                (SaveDialogBox.IsObjectOn || LoadDialogBox.IsObjectOn)))
            {
                return i;
            }
        }
    }


    // verify for load dialog area
    if(Area == LOAD_BOARD_DIALOG_AREA)
    {
        for(i=0; i<MaxBoardFileNames; i++)
        {
            if((PtInRect(&BoardFileNameRects[i], ptMousePos)) &&
                (GameSlots[i].IsObjectOn) && LoadBoardDialog.IsObjectOn)
            {
                return i;
            }
        }
    }

    // verify for the ok and cancel buttons in the dialog box text file
    if(Area == LOAD_BOARD_DIALOG_BUTTONS_AREA)
    {
        for(i=0; i<4; i++)
        {
            if((PtInRect(&(DialogButtons[i].HotSpot), ptMousePos)) &&
                (DialogButtons[i].IsObjectOn && LoadBoardDialog.IsObjectOn))
            {
                return i;
            }
        }
    }

    

    return -1;
}



/*****************************************************************************
    * Function:     void UDOPTIONS_RemoveLastOptionsScreen(int last_screen)
    * Returns:      nothing
    * Parameters:   index determining which options screen was last showing
    *                           
    * Purpose:      Remove everything contained in the Options screen .
    ****************************************************************************
    */
void UDOPTIONS_RemoveLastOptionsScreen(int last_screen)
{
    // remove everything concerning the previous options screen
    switch(last_screen)
    {
    case FILE_SCREEN:       UDOPTIONS_RemoveFileScreen(TRUE);       break;
    case OPTION_SCREEN:     UDOPTIONS_RemoveOptionScreen();     break;
    case CREDITS_SCREEN:    UDOPTIONS_RemoveCreditsScreen();    break;
    case HELP_SCREEN:       UDOPTIONS_RemoveHelpScreen(1);      break;
    case LOAD_BOARD_SCREEN: UDOPTIONS_RemoveBoardDialog();      break;
    case LOAD_GAME_SCREEN:  UDOPTIONS_RemoveFileScreen(TRUE);   break;
    default:    break;
    }

    //g_bOptionsScreenOn = FALSE;
    
}




/*****************************************************************************
    * Function:     void UDOPTIONS_DisplayFileScreen(int last_screen)
    * Returns:      nothing
    * Parameters:   index determining which options screen was last showing
    *                           
    * Purpose:      Displays everything contained in the file options screen .
    ****************************************************************************
    */
void UDOPTIONS_DisplayFileScreen(int last_screen)
{
    int i;

    if(GameSlots[0].IsObjectOn)
        return;

    // display the title
    if(!OptionsTitle.IsObjectOn)
    {
        OptionsTitle.ID = LED_IFT(DAT_LANG2, CNK_syfiles);
        LE_SEQNCR_Start(OptionsTitle.ID, OptionsTitle.Priority);
        OptionsTitle.IsObjectOn = TRUE;
    }

    // display the file options buttons
    for(i=0; i<5; i++)
    {
        if(!FileButtons[i].IsButtonOn)
        {
            FileButtons[i].ID = LED_IFT(DAT_LANG2, FileButtons[i].inTag);
            LE_SEQNCR_Start(FileButtons[i].ID, FileButtons[i].priority);
            LE_SEQNCR_SetEndingAction (FileButtons[i].ID, FileButtons[i].priority,
                    LE_SEQNCR_EndingActionStayAtEnd);
            FileButtons[i].IsButtonOn = TRUE;
        }
    }

    
}



/*****************************************************************************
    * Function:     void UDOPTIONS_RemoveFileScreen(BOOL IfRemoveDialog)
    * Returns:      nothing
    * Parameters:   flag to determine if dialog is removed or not
    *                           
    * Purpose:      Remove everything contained in the file options screen .
    ****************************************************************************
    */
void UDOPTIONS_RemoveFileScreen(BOOL IfRemoveDialog)
{
    int i;

    if(IfRemoveDialog)
    {
        UDOPTIONS_RemoveLoadDialogBox();
        UDOPTIONS_RemoveSaveDialogBox();
    }

    // remove the title
    if(OptionsTitle.IsObjectOn)
    {
        LE_SEQNCR_Stop(OptionsTitle.ID, OptionsTitle.Priority);
        OptionsTitle.IsObjectOn = FALSE;
    }

    // remove the file options buttons
    for(i=0; i<5; i++)
    {
        if(FileButtons[i].IsButtonOn)
        {
            LE_SEQNCR_Stop(FileButtons[i].ID, FileButtons[i].priority);
            FileButtons[i].IsButtonOn = FALSE;
            /*FileButtons[i].ID = LED_IFT(DAT_LANG2, FileButtons[i].outTag);
            LE_SEQNCR_Start(FileButtons[i].ID, FileButtons[i].priority);
            LE_SEQNCR_SetEndingAction (FileButtons[i].ID, FileButtons[i].priority,
                    LE_SEQNCR_EndingActionStop);*/

        }
    }
    
}




/*****************************************************************************
    * Function:     void UDOPTIONS_DisplayOptionScreen(int last_screen)
    * Returns:      nothing
    * Parameters:   index determining which options screen was last showing
    *                           
    * Purpose:      Displays everything contained in the option options screen .
    ****************************************************************************
    */
void UDOPTIONS_DisplayOptionScreen(int last_screen)
{
    int i;
    DWORD dwWidth, dwHeight, dwBPP;
    
    // display the title
    if(!OptionsTitle.IsObjectOn)
    {
        OptionsTitle.ID = LED_IFT(DAT_LANG2, CNK_syoption);
        LE_SEQNCR_Start(OptionsTitle.ID, OptionsTitle.Priority);
        LE_SEQNCR_Start(SoundSubTitleID, OptionsTitle.Priority);
        LE_SEQNCR_Start(DisplaySubTitleID, OptionsTitle.Priority);
        OptionsTitle.IsObjectOn = TRUE;
    }

    // display the on/off buttons
    for(i=0; i<MAX_ONOFF_BUTTONS; i++)
    {
        switch(i)
        {
        case TOKEN_VOICES:      if(DISPLAY_state.IsOptionTokenVoicesOn)
                                    IsOnOffButtonON[i] = TRUE;
                                else
                                    IsOnOffButtonON[i] = FALSE;
                                break;
        case HOST_COMMENTS:     if(DISPLAY_state.IsOptionHostCommentsOn)
                                     IsOnOffButtonON[i]= TRUE;
                                else
                                    IsOnOffButtonON[i] = FALSE;
                                break;
        case MUSIC:             tempMusicTuneIndex = DISPLAY_state.OptionMusicTuneIndex;
                                if(DISPLAY_state.IsOptionMusicOn)
                                    IsOnOffButtonON[i] = TRUE;
                                else
                                    IsOnOffButtonON[i] = FALSE;                                                                                   
                                break;
        
        case TOKEN_ANIMS:       if(DISPLAY_state.IsOptionTokenAnimationsOn)
                                {
                                    IsOnOffButtonON[i] = TRUE;

                                } else
                                {
                                    IsOnOffButtonON[i] = FALSE;
                                }
                                break;
        case CAMERA:            DISPLAY_state.IsOption3DCameraMovementOn = !DISPLAY_state.manualCamLock; // Existing logic uses this flag.
                                if(DISPLAY_state.IsOption3DCameraMovementOn)
                                    IsOnOffButtonON[i] = TRUE;
                                else
                                    IsOnOffButtonON[i] = FALSE;
                                
                                break;
        case LIGHTING:          if(DISPLAY_state.IsOptionLightingOn)
                                    IsOnOffButtonON[i] = TRUE;
                                else
                                    IsOnOffButtonON[i] = FALSE;
                                break;
        case _3DBOARD:          DISPLAY_state.IsOption3DBoardOn = DISPLAY_RenderSettings.bUse3DBoard;// Daves stuff
                                if(DISPLAY_state.IsOption3DBoardOn)
                                    IsOnOffButtonON[i] = TRUE;
                                else
                                    IsOnOffButtonON[i] = FALSE;
                                /*
                                // Russ' stuff
                                if( DISPLAY_state.game3Don != DISPLAY_state.IsOption3DBoardOn )
                                {
                                  DISPLAY_state.game3Don = DISPLAY_state.IsOption3DBoardOn;
                                  // Invalidate 2D buffers (city/currency may have changed since their compile)
                                  for( int t = 0; t < UDBoard_2D_constructedBoardBufferMax; t++ )
                                    UDBoard_2DLoadedViews[t].ViewLoaded = -1;

                                  if( !DISPLAY_state.game3Don )
                                  {// Force a camera update - camera can be on manual or anywhere
                                    DISPLAY_state.manualMouseCamLock = FALSE;
                                    DISPLAY_state.desiredCameraInvalidatedLock = TRUE;
                                    DISPLAY_state.desiredCameraClearToValidate = TRUE;
            
                                  }
                                }
                                */
                                break;
        case FILTERING:         DISPLAY_state.IsOptionFilteringOn = DISPLAY_RenderSettings.bClassic_UseBilinearFiltering;
                                if(DISPLAY_state.IsOptionFilteringOn)
                                    IsOnOffButtonON[i] = TRUE;
                                else
                                    IsOnOffButtonON[i] = FALSE;
                                
                                break;
        case DITHERING:         DISPLAY_state.IsOptionDitheringOn = DISPLAY_RenderSettings.bClassic_UseDithering;
                                if(DISPLAY_state.IsOptionDitheringOn)
                                    IsOnOffButtonON[i] = TRUE;
                                else
                                    IsOnOffButtonON[i] = FALSE;                                
                                break;

        case RESOLUTION:        /* remove the screen res stuff for now
                                dwBPP = pc3D::GetRenderSurfaceBitDepth();
                                Surface* pPrimary = pc3D::GetPrimarySurface();
                                pPrimary->GetSize(dwWidth, dwHeight);

                                for(int j=0; j<5; j++)
                                {
                                    if((dwWidth == (DWORD)ChosenWidth[j]) &&
                                        (dwHeight == (DWORD)ChosenHeight[j]))
                                    {
                                        DISPLAY_state.OptionScreenResIndex = j;
                                        break;
                                    }
                                }
                                tempResIndex = DISPLAY_state.OptionScreenResIndex;
                                if(dwBPP == 32)
                                    tempBitIndex = 1;
                                else
                                    tempBitIndex = 0;
                                */

                                break;
        }

        if(i == RESOLUTION)
            continue;

        if(IsOnOffButtonON[i])
        {
            if(!(OptionOnButtons[i].IsObjectOn))
            {
                LE_SEQNCR_StartXY(OptionOnButtons[i].ID, OptionOnButtons[i].Priority,
                                OptionOnButtons[i].HotSpot.left, OptionOnButtons[i].HotSpot.top);
                LE_SEQNCR_SetEndingAction (OptionOnButtons[i].ID,  OptionOnButtons[i].Priority,
                                LE_SEQNCR_EndingActionStayAtEnd);
                OptionOnButtons[i].IsObjectOn = TRUE;
            }

            if(!(OptionOffButtons[i].IsObjectOn))
            {
                LE_SEQNCR_StartXY(OptionOffButtons[i].extraID, OptionOffButtons[i].Priority,
                                OptionOffButtons[i].HotSpot.left, OptionOffButtons[i].HotSpot.top);
                LE_SEQNCR_SetEndingAction (OptionOffButtons[i].extraID,  OptionOffButtons[i].Priority,
                                LE_SEQNCR_EndingActionStayAtEnd);
                OptionOffButtons[i].IsObjectOn = TRUE;
            }
        }
        else
        {
            if(!(OptionOnButtons[i].IsObjectOn))
            {
                LE_SEQNCR_StartXY(OptionOnButtons[i].extraID, OptionOnButtons[i].Priority,
                                OptionOnButtons[i].HotSpot.left, OptionOnButtons[i].HotSpot.top);
                LE_SEQNCR_SetEndingAction (OptionOnButtons[i].extraID,  OptionOnButtons[i].Priority,
                                LE_SEQNCR_EndingActionStayAtEnd);
                OptionOnButtons[i].IsObjectOn = TRUE;
            }

            if(!(OptionOffButtons[i].IsObjectOn))
            {
                LE_SEQNCR_StartXY(OptionOffButtons[i].ID, OptionOffButtons[i].Priority,
                                OptionOffButtons[i].HotSpot.left, OptionOffButtons[i].HotSpot.top);
                LE_SEQNCR_SetEndingAction (OptionOffButtons[i].ID,  OptionOffButtons[i].Priority,
                                LE_SEQNCR_EndingActionStayAtEnd);
                OptionOffButtons[i].IsObjectOn = TRUE;
            }
        }
        
    }
#if !USA_VERSION
    // set the font size
    LE_FONTS_GetSettings(0);
    LE_FONTS_SetSize(10);
	LE_FONTS_SetWeight(700);
#endif

    // display the labels
    for(i=0; i<MAX_ONOFF_BUTTONS; i++)
    {
        if( i == RESOLUTION)
            continue;

        if(!OptionChoices[i].IsObjectOn)
        {
            LE_SEQNCR_StartXY(OptionChoices[i].ID, OptionChoices[i].Priority,
                                OptionChoices[i].HotSpot.left, OptionChoices[i].HotSpot.top);
            OptionChoices[i].IsObjectOn = TRUE;
        }
    }


    // display the music tune choices
    ACHAR   szNum[100];
    for(i=0; i<MAX_TUNES; i++)
    {
        if(!MusicChoices[i].IsObjectOn)
        {
#if USA_VERSION
            Asprintf(szNum, A_T("%s"), szMusicChoices[i]);
#else
            Asprintf(szNum, A_T("%i"), i+1);
#endif
            
            LE_GRAFIX_ColorArea(MusicChoices[i].ID, 0, 0, MusicChoices[i].Width, MusicChoices[i].Height,
                    LE_GRAFIX_MakeColorRef(0,255,0));
            if(i == DISPLAY_state.OptionMusicTuneIndex)
            {               
                LE_FONTS_Print(MusicChoices[i].ID, 0, 0, LE_GRAFIX_MakeColorRef(0,0,255), szNum);
                tempMusicTuneIndex = i;
            }
            else
                LE_FONTS_Print(MusicChoices[i].ID, 0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szNum);               
            
            
            LE_SEQNCR_StartXY(MusicChoices[i].ID, MusicChoices[i].Priority,
                                MusicChoices[i].HotSpot.left, MusicChoices[i].HotSpot.top);
            MusicChoices[i].IsObjectOn = TRUE;
        }
    }

/*  remove the screen resolution stuff for now

    // display the screen resolution choices
    for(i=0; i<g_MaxResChoices; i++)
    {
        if(!ScreenResChoices[i].IsObjectOn)
        {
            LE_GRAFIX_ColorArea(ScreenResChoices[i].ID, 0, 0, ScreenResChoices[i].Width, ScreenResChoices[i].Height,
                    LE_GRAFIX_MakeColorRef(0,255,0));
            if(i == tempResIndex)
                LE_FONTS_Print(ScreenResChoices[i].ID, 0, 0, LE_GRAFIX_MakeColorRef(0,0,255), szResolutionChoices[i]);              
               
            else
                LE_FONTS_Print(ScreenResChoices[i].ID, 0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szResolutionChoices[i]);              
            
            
            LE_SEQNCR_StartXY(ScreenResChoices[i].ID, ScreenResChoices[i].Priority,
                                ScreenResChoices[i].HotSpot.left, ScreenResChoices[i].HotSpot.top);
            ScreenResChoices[i].IsObjectOn = TRUE;
        }
    }

    // display the bit choices
    for(i=0; i<2; i++)
    {
        if(!BitChoices[i].IsObjectOn && (g_MaxBitChoices[i] & i+_16_BIT_CHOICE))
        {
            LE_GRAFIX_ColorArea(BitChoices[i].ID, 0, 0, BitChoices[i].Width, BitChoices[i].Height,
                    LE_GRAFIX_MakeColorRef(0,255,0));
            if(i == tempBitIndex)
                LE_FONTS_Print(BitChoices[i].ID, 0, 0, LE_GRAFIX_MakeColorRef(0,0,255), szBitChoices[i]);              
                
            else
                LE_FONTS_Print(BitChoices[i].ID, 0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szBitChoices[i]);              
            
            
            LE_SEQNCR_StartXY(BitChoices[i].ID, BitChoices[i].Priority,
                                BitChoices[i].HotSpot.left, BitChoices[i].HotSpot.top);
            BitChoices[i].IsObjectOn = TRUE;
        }
    }
*/
    // display the ok/cancel buttons
    for(i=0; i<1; i++)
    {
        if(!OptionButtons[i].IsButtonOn)
        {
            LE_SEQNCR_StartXY(OptionButtons[i].ID, OptionButtons[i].priority,
                OptionButtons[O_OK].HotSpot.left, OptionButtons[O_OK].HotSpot.top);
            LE_SEQNCR_SetEndingAction (OptionButtons[i].ID, OptionButtons[i].priority,
                    LE_SEQNCR_EndingActionStayAtEnd);
            OptionButtons[i].IsButtonOn = TRUE;
        }
    }

}





/*****************************************************************************
    * Function:     void UDOPTIONS_RemoveOptionScreen(void)
    * Returns:      nothing
    * Parameters:   nothing
    *                           
    * Purpose:      Remove everything contained in the option options screen .
    ****************************************************************************
    */
void UDOPTIONS_RemoveOptionScreen(void)
{
    int i;

    // remove the title
    if(OptionsTitle.IsObjectOn)
    {
        LE_SEQNCR_Stop(OptionsTitle.ID, OptionsTitle.Priority);
        LE_SEQNCR_Stop(SoundSubTitleID, OptionsTitle.Priority);
        LE_SEQNCR_Stop(DisplaySubTitleID, OptionsTitle.Priority);
        OptionsTitle.IsObjectOn = FALSE;
    }

    for(i=0; i<MAX_ONOFF_BUTTONS; i++)
    {
        // remove screen res stuff for now
        if( i == RESOLUTION)
            continue;

        if(IsOnOffButtonON[i])
        {
            // remove the on/off buttons
            if(OptionOnButtons[i].IsObjectOn)
            {
                LE_SEQNCR_Stop(OptionOnButtons[i].ID, OptionOnButtons[i].Priority);
                OptionOnButtons[i].IsObjectOn = FALSE;
            }

            if(OptionOffButtons[i].IsObjectOn)
            {
                LE_SEQNCR_Stop(OptionOffButtons[i].extraID, OptionOffButtons[i].Priority);
                OptionOffButtons[i].IsObjectOn = FALSE;
            }
        }
        else
        {
            // remove the on/off buttons
            if(OptionOnButtons[i].IsObjectOn)
            {
                LE_SEQNCR_Stop(OptionOnButtons[i].extraID, OptionOnButtons[i].Priority);
                OptionOnButtons[i].IsObjectOn = FALSE;
            }

            if(OptionOffButtons[i].IsObjectOn)
            {
                LE_SEQNCR_Stop(OptionOffButtons[i].ID, OptionOffButtons[i].Priority);
                OptionOffButtons[i].IsObjectOn = FALSE;
            }
        }
    }

    // remove the labels
    for(i=0; i<MAX_ONOFF_BUTTONS; i++)
    {
        // remove res stuff for now
        if(i == RESOLUTION)
            continue;

        if(OptionChoices[i].IsObjectOn)
        {
            LE_SEQNCR_Stop(OptionChoices[i].ID, OptionChoices[i].Priority);
            OptionChoices[i].IsObjectOn = FALSE;
        }
    }

    // remove the music tune choices
    for(i=0; i<MAX_TUNES; i++)
    {
        if(MusicChoices[i].IsObjectOn)
        {
            LE_SEQNCR_Stop(MusicChoices[i].ID, MusicChoices[i].Priority);
            MusicChoices[i].IsObjectOn = FALSE;
        }
    }

    // if user canceled or simply moved off the options option screen, 
    // the tune is the same as when first going into the options screen.
    // Else, if the user hit okay the tune is the one chosen
    DISPLAY_state.OptionMusicTuneIndex = tempMusicTuneIndex;

    // remove the screen resolution choices
    for(i=0; i<g_MaxResChoices; i++)
    {
        if(ScreenResChoices[i].IsObjectOn)
        {
            LE_SEQNCR_Stop(ScreenResChoices[i].ID, ScreenResChoices[i].Priority);
            ScreenResChoices[i].IsObjectOn = FALSE;
        }
    }

    // remove the screen resolution choices
    for(i=0; i<2; i++)
    {
        if(BitChoices[i].IsObjectOn)
        {
            LE_SEQNCR_Stop(BitChoices[i].ID, BitChoices[i].Priority);
            BitChoices[i].IsObjectOn = FALSE;
        }
    }

    // display the ok/cancel buttons
    for(i=0; i<1; i++)
    {
        if(OptionButtons[i].IsButtonOn)
        {
            LE_SEQNCR_Stop(OptionButtons[i].ID, OptionButtons[i].priority);
            OptionButtons[i].IsButtonOn = FALSE;
        }
    }

}



/*****************************************************************************
    * Function:     void UDOPTIONS_DisplayCreditsScreen(int last_screen)
    * Returns:      nothing
    * Parameters:   index determining which options screen was last showing
    *                           
    * Purpose:      Displays everything contained in the credits options screen .
    ****************************************************************************
    */
void UDOPTIONS_DisplayCreditsScreen(int last_screen)
{
    if(g_UserChoseToExit)
    {
        LE_GRAFIX_ColorArea(CreditsObject.ID, 0, 0, CreditsObject.Width, CreditsObject.Height, 
                                LE_GRAFIX_MakeColorRef(0,255,0));            
        CreditsObject.HotSpot.top = CreditsObject.Height;
        CreditsObject.HotSpot.bottom = CreditsObject.Height +
            LE_GRAFIX_ReturnObjectHeight(CreditsObject.extraID);
        LE_GRAFIX_ShowTCB_Alpha(CreditsObject.ID, 0, (short)CreditsObject.HotSpot.top, CreditsObject.extraID); 
    }

    if(!CreditsObject.IsObjectOn)
    {
        LE_SEQNCR_StartXY(CreditsObject.ID, CreditsObject.Priority,
                CreditsObject.var1, CreditsObject.var2);
        CreditsObject.IsObjectOn = TRUE;

        // set the scrolling time
        FirstScrollTime = LE_TIME_TickCount;
    }
    
}




/*****************************************************************************
    * Function:     void UDOPTIONS_RemoveCreditsScreen(void)
    * Returns:      nothing
    * Parameters:   nothing
    *                           
    * Purpose:      Remove everything contained in the credits options screen .
    ****************************************************************************
    */
void UDOPTIONS_RemoveCreditsScreen(void)
{
    
    if(CreditsObject.IsObjectOn)
    {
        LE_GRAFIX_ColorArea(CreditsObject.ID, 0, 0, CreditsObject.Width, CreditsObject.Height, 
                                LE_GRAFIX_MakeColorRef(0,255,0));   
        LE_SEQNCR_Stop(CreditsObject.ID, CreditsObject.Priority);
        CreditsObject.IsObjectOn = FALSE;
    }
    
}


/*****************************************************************************
    * Function:     void UDOPTIONS_DisplayHelpScreen(int last_screen)
    * Returns:      nothing
    * Parameters:   index determining which options screen was last showing
    *                           
    * Purpose:      Displays everything contained in the help options screen .
    ****************************************************************************
    */
void UDOPTIONS_DisplayHelpScreen(int last_screen)
{
    int i;

    // display the title
    if(!OptionsTitle.IsObjectOn)
    {
        OptionsTitle.ID = LED_IFT(DAT_LANG2, CNK_syhelp);
        LE_SEQNCR_Start(OptionsTitle.ID, OptionsTitle.Priority);
        OptionsTitle.IsObjectOn = TRUE;
    }

    // display the file options buttons
    for(i=0; i<3; i++)
    {
        if(!HelpButtons[i].IsButtonOn)
        {
            LE_SEQNCR_Start(HelpButtons[i].ID, HelpButtons[i].priority);
            LE_SEQNCR_SetEndingAction (HelpButtons[i].ID, HelpButtons[i].priority,
                    LE_SEQNCR_EndingActionStayAtEnd);
            HelpButtons[i].IsButtonOn = TRUE;
        }
    }

}



/*****************************************************************************
    * Function:     void UDOPTIONS_RemoveHelpScreen(BOOL HelpTextOn)
    * Returns:      nothing
    * Parameters:   nothing
    *                           
    * Purpose:      Remove everything contained in the help options screen .
    ****************************************************************************
    */
void UDOPTIONS_RemoveHelpScreen(BOOL HelpTextOn)
{
    int i;

    if(HelpTextOn)
    {
        UDOPTIONS_RemoveQuickHelpFile();
    }
    
    // remove the title
    if(OptionsTitle.IsObjectOn)
    {
        LE_SEQNCR_Stop(OptionsTitle.ID, OptionsTitle.Priority);
        OptionsTitle.IsObjectOn = FALSE;
    }

    // remove the file options buttons
    for(i=0; i<3; i++)
    {
        if(HelpButtons[i].IsButtonOn)
        {
            LE_SEQNCR_Stop(HelpButtons[i].ID, HelpButtons[i].priority);
            HelpButtons[i].IsButtonOn = FALSE;
        }
    }

    
}



/*****************************************************************************
    * Function:     void UDOPTIONS_InitOptionsButtons(void)
    * Returns:      nothing
    * Parameters:   nothing
    *                           
    * Purpose:      initialized the buttons use in the options screens 
    ****************************************************************************
    */
void UDOPTIONS_InitOptionsButtons(void)
{
    // set file-new_game button
    FileButtons[F_NEW_GAME].IsButtonOn = FALSE;
    FileButtons[F_NEW_GAME].HotSpot.left = 291;
    FileButtons[F_NEW_GAME].HotSpot.top = 114;
    FileButtons[F_NEW_GAME].HotSpot.right = FileButtons[F_NEW_GAME].HotSpot.left + 220;
    FileButtons[F_NEW_GAME].HotSpot.bottom = FileButtons[F_NEW_GAME].HotSpot.top + 62;
    FileButtons[F_NEW_GAME].priority = 50;
    FileButtons[F_NEW_GAME].idleTag = CNK_syfnwgm2;
    FileButtons[F_NEW_GAME].inTag = CNK_syfnwgm1;
    FileButtons[F_NEW_GAME].outTag = CNK_syfnwgm0;
    //FileButtons[0].state = CGE_OFF;
    FileButtons[F_NEW_GAME].ID = LED_IFT(DAT_LANG2, CNK_syfnwgm1);

    // set file-load button
    FileButtons[F_LOAD].IsButtonOn = FALSE;
    FileButtons[F_LOAD].HotSpot.left = 291;
    FileButtons[F_LOAD].HotSpot.top = 180;
    FileButtons[F_LOAD].HotSpot.right = FileButtons[F_LOAD].HotSpot.left + 220;
    FileButtons[F_LOAD].HotSpot.bottom = FileButtons[F_LOAD].HotSpot.top + 62;
    FileButtons[F_LOAD].priority = 50;
    FileButtons[F_LOAD].idleTag = CNK_syfldgm2;
    FileButtons[F_LOAD].inTag = CNK_syfldgm1;
    FileButtons[F_LOAD].outTag = CNK_syfldgm0;
    //FileButtons[0].state = CGE_OFF;
    FileButtons[F_LOAD].ID = LED_IFT(DAT_LANG2, CNK_syfldgm1);

    // set file-save button
    FileButtons[F_SAVE].IsButtonOn = FALSE;
    FileButtons[F_SAVE].HotSpot.left = 291;
    FileButtons[F_SAVE].HotSpot.top = 247;
    FileButtons[F_SAVE].HotSpot.right = FileButtons[F_SAVE].HotSpot.left + 220;
    FileButtons[F_SAVE].HotSpot.bottom = FileButtons[F_SAVE].HotSpot.top + 62;
    FileButtons[F_SAVE].priority = 50;
    FileButtons[F_SAVE].idleTag = CNK_syfsvgm2;
    FileButtons[F_SAVE].inTag = CNK_syfsvgm1;
    FileButtons[F_SAVE].outTag = CNK_syfsvgm0;
    //FileButtons[0].state = CGE_OFF;
    FileButtons[F_SAVE].ID = LED_IFT(DAT_LANG2, CNK_syfsvgm1);

    // set file-exit button
    FileButtons[F_EXIT].IsButtonOn = FALSE;
    FileButtons[F_EXIT].HotSpot.left = 291;
    FileButtons[F_EXIT].HotSpot.top = 314;
    FileButtons[F_EXIT].HotSpot.right = FileButtons[F_EXIT].HotSpot.left + 220;
    FileButtons[F_EXIT].HotSpot.bottom = FileButtons[F_EXIT].HotSpot.top + 62;
    FileButtons[F_EXIT].priority = 50;
    FileButtons[F_EXIT].idleTag = CNK_syfexit2;
    FileButtons[F_EXIT].inTag = CNK_syfexit1;
    FileButtons[F_EXIT].outTag = CNK_syfexit0;
    //FileButtons[0].state = CGE_OFF;
    FileButtons[F_EXIT].ID = LED_IFT(DAT_LANG2, CNK_syfexit1);

    // set file-cancel button
    FileButtons[F_CANCEL].IsButtonOn = FALSE;
    FileButtons[F_CANCEL].HotSpot.left = 291;
    FileButtons[F_CANCEL].HotSpot.top = 401;
    FileButtons[F_CANCEL].HotSpot.right = FileButtons[F_CANCEL].HotSpot.left + 220;
    FileButtons[F_CANCEL].HotSpot.bottom = FileButtons[F_CANCEL].HotSpot.top + 62;
    FileButtons[F_CANCEL].priority = 50;
    FileButtons[F_CANCEL].idleTag = CNK_syfcnxl2;
    FileButtons[F_CANCEL].inTag = CNK_syfcnxl1;
    FileButtons[F_CANCEL].outTag = CNK_syfcnxl0;
    //FileButtons[0].state = CGE_OFF;
    FileButtons[F_CANCEL].ID = LED_IFT(DAT_LANG2, CNK_syfcnxl1);


    // set help-quick help button
    HelpButtons[H_QUICK_HELP].IsButtonOn = FALSE;
    HelpButtons[H_QUICK_HELP].HotSpot.left = 291;
    HelpButtons[H_QUICK_HELP].HotSpot.top = 180;
    HelpButtons[H_QUICK_HELP].HotSpot.right = HelpButtons[H_QUICK_HELP].HotSpot.left + 220;
    HelpButtons[H_QUICK_HELP].HotSpot.bottom = HelpButtons[H_QUICK_HELP].HotSpot.top + 62;
    HelpButtons[H_QUICK_HELP].priority = 50;
    HelpButtons[H_QUICK_HELP].idleTag = CNK_syhqhlp2;
    HelpButtons[H_QUICK_HELP].inTag = CNK_syhqhlp1;
    HelpButtons[H_QUICK_HELP].outTag = CNK_syhqhlp0;
    //FileButtons[0].state = CGE_OFF;
    HelpButtons[H_QUICK_HELP].ID = LED_IFT(DAT_LANG2, CNK_syhqhlp1);

    // set help-full help button
    HelpButtons[H_FULL_HELP].IsButtonOn = FALSE;
    HelpButtons[H_FULL_HELP].HotSpot.left = 291;
    HelpButtons[H_FULL_HELP].HotSpot.top = 288;
    HelpButtons[H_FULL_HELP].HotSpot.right = HelpButtons[H_FULL_HELP].HotSpot.left + 220;
    HelpButtons[H_FULL_HELP].HotSpot.bottom = HelpButtons[H_FULL_HELP].HotSpot.top + 62;
    HelpButtons[H_FULL_HELP].priority = 50;
    HelpButtons[H_FULL_HELP].idleTag = CNK_syhfhlp2;
    HelpButtons[H_FULL_HELP].inTag = CNK_syhfhlp1;
    HelpButtons[H_FULL_HELP].outTag = CNK_syhfhlp0;
    //FileButtons[0].state = CGE_OFF;
    HelpButtons[H_FULL_HELP].ID = LED_IFT(DAT_LANG2, CNK_syhfhlp1);

    // set help-cancel button
    HelpButtons[H_CANCEL].IsButtonOn = FALSE;
    HelpButtons[H_CANCEL].HotSpot.left = 291;
    HelpButtons[H_CANCEL].HotSpot.top = 401;
    HelpButtons[H_CANCEL].HotSpot.right = HelpButtons[H_CANCEL].HotSpot.left + 220;
    HelpButtons[H_CANCEL].HotSpot.bottom = HelpButtons[H_CANCEL].HotSpot.top + 62;
    HelpButtons[H_CANCEL].priority = 50;
    HelpButtons[H_CANCEL].idleTag = CNK_syfcnxl2;
    HelpButtons[H_CANCEL].inTag = CNK_syfcnxl1;
    HelpButtons[H_CANCEL].outTag = CNK_syfcnxl0;
    //FileButtons[0].state = CGE_OFF;
    HelpButtons[H_CANCEL].ID = LED_IFT(DAT_LANG2, CNK_syfcnxl1);


    // set Option-OK button
    OptionButtons[O_OK].IsButtonOn = FALSE;
    OptionButtons[O_OK].HotSpot.left = 350;
    OptionButtons[O_OK].HotSpot.top = 450;
    OptionButtons[O_OK].HotSpot.right = OptionButtons[O_OK].HotSpot.left + 127;
    OptionButtons[O_OK].HotSpot.bottom = OptionButtons[O_OK].HotSpot.top + 36;
    OptionButtons[O_OK].priority = 50;
    OptionButtons[O_OK].idleTag = LED_EI;
    OptionButtons[O_OK].inTag = CNK_syooki;
    OptionButtons[O_OK].outTag = CNK_syooko;
    //FileButtons[0].state = CGE_OFF;
    OptionButtons[O_OK].ID = LED_IFT(DAT_LANG2, CNK_syooki);

    // set Option-cancel button
    OptionButtons[O_CANCEL].IsButtonOn = FALSE;
    OptionButtons[O_CANCEL].HotSpot.left = 291;
    OptionButtons[O_CANCEL].HotSpot.top = 401;
    OptionButtons[O_CANCEL].HotSpot.right = OptionButtons[O_CANCEL].HotSpot.left + 220;
    OptionButtons[O_CANCEL].HotSpot.bottom = OptionButtons[O_CANCEL].HotSpot.top + 62;
    OptionButtons[O_CANCEL].priority = 50;
    OptionButtons[O_CANCEL].idleTag = CNK_syfcnxl2;
    OptionButtons[O_CANCEL].inTag = CNK_syfcnxl1;
    OptionButtons[O_CANCEL].outTag = CNK_syfcnxl0;
    //FileButtons[0].state = CGE_OFF;
    OptionButtons[O_CANCEL].ID = LED_IFT(DAT_LANG2, CNK_syfcnxl1);
    
}




/*****************************************************************************
    * Function:     void UDOPTIONS_ProcessFileOptionButtonPress(int button, short Area)
    * Returns:      nothing
    * Parameters:   index of button pressed
                    enum determining area clicked
    *
    * Purpose:      Determines the action depending on which File options button
    *                           was pressed.
    ****************************************************************************
    */
void UDOPTIONS_ProcessFileOptionButtonPress(int button, short Area)
{
    LE_QUEUE_MessageRecord UItempMessage;
        
    if(Area == FILE_OPTION_AREA)
    {
        UDSOUND_Click();
        switch(button)
        {
            case F_NEW_GAME:    g_UserRequestedNewGame = TRUE;
                                // hack
                                // fake a esc button hit
                                UItempMessage.messageCode = UIMSG_KEYBOARD_PRESSED;
				                UItempMessage.numberA = LE_KEYBRD_ESC;
				                UDIBAR_ProcessMessage( &UItempMessage );
                                UDIBar_ProcessOptionsButtonPress(-1);
                                /*                                                
                                SetCurrentUIPlayerFromPlayerSet ((RULE_PlayerSet) -1);
                                MESS_SendAction (ACTION_NEW_GAME, AnyLocalPlayer (), RULE_BANK_PLAYER,
                                    0, 0, 0, 0, NULL, 0, NULL);
                                UDPSEL_SwitchPhase(UDPSEL_PHASE_LOCALORNETWORK);
                                UDBOARD_SetBackdrop( DISPLAY_SCREEN_Pselect);
                                */
                                //return;
                                break;
            case F_LOAD:        UDOPTIONS_DisplayLoadDialogBox();
                                IsLoadDialog = TRUE;
                                //LoadGameOrOptions (TRUE); 
                                //UDBOARD_SetBackdrop(DISPLAY_SCREEN_MainA);
                                break;
            case F_SAVE:        UDOPTIONS_DisplaySaveDialogBox();
                                IsLoadDialog = FALSE;                               
                                break;
            case F_EXIT:        g_UserRequestedExit = TRUE;
                                // hack
                                // fake a esc button hit
                                UItempMessage.messageCode = UIMSG_KEYBOARD_PRESSED;
				                UItempMessage.numberA = LE_KEYBRD_ESC;
				                UDIBAR_ProcessMessage( &UItempMessage );                                
                                break;
            case F_CANCEL:      UDOPTIONS_SetBackdrop(last_playerselectscreen);
                                //UDBOARD_SetBackdrop( last_playerselectscreen );
                                UDIBar_ProcessOptionsButtonPress(-1);
                                break;
            default: return;
        }
        
        UDOPTIONS_RemoveFileScreen(FALSE);
    }
    else if(Area == FILE_LOAD_GAME_DIALOG_AREA)
    {
        LE_FONTS_GetSettings(3);
        if(IsLoadDialog)
        {
            // print the previous file in regular colour
            if(CurrentGameFile > -1)
            {
                
                if(GameSlots[CurrentGameFile].IsObjectOn)
                {
                    LE_SEQNCR_Stop(GameSlots[CurrentGameFile].ID, GameSlots[CurrentGameFile].Priority);
                    GameSlots[CurrentGameFile].ID = LED_IFT(DAT_MAIN, GameSlots[CurrentGameFile].var1);
                    LE_SEQNCR_StartXY(GameSlots[CurrentGameFile].ID, GameSlots[CurrentGameFile].Priority,
                            (short)GameSlots[CurrentGameFile].HotSpot.left, (short)GameSlots[CurrentGameFile].HotSpot.top);
                }
                LE_GRAFIX_ColorArea(GameSlots[CurrentGameFile].extraID, 0, 0,
                        (short)GameFileNameRects[CurrentGameFile].right, 
                        (short)GameFileNameRects[CurrentGameFile].bottom,
                        LE_GRAFIX_MakeColorRef(0,255,0));
                LE_FONTS_Print(GameSlots[CurrentGameFile].extraID, 13, 8, 
                        LE_GRAFIX_MakeColorRef(255,255,255), SaveGameStuff[CurrentGameFile].GameFileDesc);

                LE_SEQNCR_ForceRedraw(GameSlots[CurrentGameFile].extraID, GameSlots[CurrentGameFile].Priority+1);
            }
            
            // then highlight the chosen file name
            if(GameSlots[button].IsObjectOn)
            {
                LE_SEQNCR_Stop(GameSlots[button].ID, GameSlots[button].Priority);
                GameSlots[button].ID = LED_IFT(DAT_MAIN, GameSlots[button].var2);
                LE_SEQNCR_StartXY(GameSlots[button].ID, GameSlots[button].Priority,
                        (short)GameSlots[button].HotSpot.left, (short)GameSlots[button].HotSpot.top);
            }
            LE_GRAFIX_ColorArea(GameSlots[button].extraID, 0, 0, (short)GameFileNameRects[button].right,
                    (short)GameFileNameRects[button].bottom, LE_GRAFIX_MakeColorRef(0,255,0));
            
            LE_FONTS_Print(GameSlots[button].extraID, 13, 8,
                    LE_GRAFIX_MakeColorRef(255,255,255), SaveGameStuff[button].GameFileDesc);
        
            LE_SEQNCR_ForceRedraw(GameSlots[button].extraID, GameSlots[button].Priority+1);
            
        }       
        else
        {
  
            // print the previous file in regular colour
            if(CurrentGameFile > -1)
            {
                if(GameSlots[CurrentGameFile].IsObjectOn)
                {
                    LE_SEQNCR_Stop(GameSlots[CurrentGameFile].ID, GameSlots[CurrentGameFile].Priority);
                    GameSlots[CurrentGameFile].ID = LED_IFT(DAT_MAIN, GameSlots[CurrentGameFile].var1);
                    LE_SEQNCR_StartXY(GameSlots[CurrentGameFile].ID, GameSlots[CurrentGameFile].Priority,
                            (short)GameSlots[CurrentGameFile].HotSpot.left, (short)GameSlots[CurrentGameFile].HotSpot.top);
                }
                LE_GRAFIX_ColorArea(GameSlots[CurrentGameFile].extraID, 0, 0,
                        (short)GameFileNameRects[CurrentGameFile].right, 
                        (short)GameFileNameRects[CurrentGameFile].bottom,
                        LE_GRAFIX_MakeColorRef(0,255,0));
                LE_FONTS_Print(GameSlots[CurrentGameFile].extraID, 13, 8, 
                        LE_GRAFIX_MakeColorRef(255,255,255), SaveGameStuff[CurrentGameFile].GameFileDesc);

                LE_SEQNCR_ForceRedraw(GameSlots[CurrentGameFile].extraID, GameSlots[CurrentGameFile].Priority+1);
            }
            
            // then highlight the chosen file name
            if(GameSlots[button].IsObjectOn)
            {
                LE_SEQNCR_Stop(GameSlots[button].ID, GameSlots[button].Priority);
                GameSlots[button].ID = LED_IFT(DAT_MAIN, GameSlots[button].var2);
                LE_SEQNCR_StartXY(GameSlots[button].ID, GameSlots[button].Priority,
                        (short)GameSlots[button].HotSpot.left, (short)GameSlots[button].HotSpot.top);
            }
            LE_GRAFIX_ColorArea(GameSlots[button].extraID, 0, 0, (short)GameFileNameRects[button].right,
                    (short)GameFileNameRects[button].bottom, LE_GRAFIX_MakeColorRef(0,255,0));
            
            LE_FONTS_Print(GameSlots[button].extraID, 13, 8,
                    LE_GRAFIX_MakeColorRef(255,255,255), A_T("_"));//GameFileDesc[button]);
        
            LE_SEQNCR_ForceRedraw(GameSlots[button].extraID, GameSlots[button].Priority+1);
        }
        CurrentGameFile = button;
        ZeroMemory(szTempGameFileDesc, 100);
        LE_FONTS_GetSettings(0);
        
    }
    else if(Area == DIALOG_BUTTONS_AREA)
    {
        UDSOUND_Click();

        if(IsLoadDialog)
        {
            switch(button)
            {
            case 0:     // ok button
                        if(UDOPTIONS_LoadGame (CurrentGameFile))
                        {
                            SetUpLoadedGame();
                            UDBOARD_SetBackdrop(DISPLAY_SCREEN_MainA);
                            break;
                        }
                        
            case 1:     // cancel button
                        UDOPTIONS_RemoveLoadDialogBox();
                        UDOPTIONS_DisplayFileScreen(current_options_result);
                        break;
            }
        }
        else
        {
            switch(button)
            {
            case 0:     // ok button
                        // save the game description
                        Astrcpy(SaveGameStuff[CurrentGameFile].GameFileDesc, szTempGameFileDesc);
                        MESS_SendAction (ACTION_GET_GAME_STATE_FOR_SAVE, AnyLocalPlayer (), 
                                    RULE_BANK_PLAYER, 0, 0, 0, 0, NULL, 0, NULL);                        
                                                
            case 1:     // cancel button
                        ZeroMemory(szTempGameFileDesc, 100);
                        UDOPTIONS_RemoveSaveDialogBox();
                        UDOPTIONS_DisplayFileScreen(current_options_result);
                        break;
            }
        }
    }
}




/*****************************************************************************
    * Function:     void UDOPTIONS_ProcessOptionOptionButtonPress(int button, short Area)
    * Returns:      nothing
    * Parameters:   index of button pressed
    *               area we are looking at (on, off or other)
    *
    * Purpose:      Determines the action depending on which Option options button
    *                           was pressed.
    ****************************************************************************
    */
void UDOPTIONS_ProcessOptionOptionButtonPress(int button, short Area)
{
    int i;
  
#if !USA_VERSION
    // set the font size
    LE_FONTS_GetSettings(0);
    LE_FONTS_SetSize(10);
	LE_FONTS_SetWeight(700);
#endif

    if(Area == OPTION_OPTION_AREA)
    {
        UDSOUND_Click();

        switch(button)
        {
        case O_OK:  // now set the actual option
                    for(i=0; i<MAX_ONOFF_BUTTONS; i++)
                    {
                        switch(i)
                        {
                        case TOKEN_VOICES:      if(IsOnOffButtonON[i])
                                                    DISPLAY_state.IsOptionTokenVoicesOn = TRUE;
                                                else
                                                    DISPLAY_state.IsOptionTokenVoicesOn = FALSE;
                                                break;
                        case HOST_COMMENTS:     if(IsOnOffButtonON[i])
                                                    DISPLAY_state.IsOptionHostCommentsOn = TRUE;
                                                else
                                                    DISPLAY_state.IsOptionHostCommentsOn = FALSE;
                                                break;
                        case MUSIC:             tempMusicTuneIndex = DISPLAY_state.OptionMusicTuneIndex;
                                                if(IsOnOffButtonON[i])
                                                {
                                                    DISPLAY_state.IsOptionMusicOn = TRUE;
                                                    UDSOUND_BackgroundMusicOn();
                                                }
                                                else
                                                {
                                                    DISPLAY_state.IsOptionMusicOn = FALSE;
                                                    UDSOUND_BackgroundMusicOn();
                                                }                                                   
                                                break;
                        
                        case TOKEN_ANIMS:
                                                if(IsOnOffButtonON[i])
                                                {
                                                    DISPLAY_state.IsOptionTokenAnimationsOn = TRUE;

                                                } else
                                                {
                                                    DISPLAY_state.IsOptionTokenAnimationsOn = FALSE;
                                                }
                                                break;
                        case CAMERA:            
                                                if(IsOnOffButtonON[i])
                                                    DISPLAY_state.IsOption3DCameraMovementOn = TRUE;
                                                else
                                                    DISPLAY_state.IsOption3DCameraMovementOn = FALSE;
                                                DISPLAY_state.manualCamLock = !DISPLAY_state.IsOption3DCameraMovementOn; // Existing logic uses this flag.
                                                break;
                        case LIGHTING:          if(IsOnOffButtonON[i])
                                                    DISPLAY_state.IsOptionLightingOn = TRUE;
                                                else
                                                    DISPLAY_state.IsOptionLightingOn = FALSE;
                                                break;
                        case _3DBOARD:          if(IsOnOffButtonON[i])
                                                    DISPLAY_state.IsOption3DBoardOn = TRUE;
                                                else
                                                    DISPLAY_state.IsOption3DBoardOn = FALSE;
                                                DISPLAY_RenderSettings.bUse3DBoard = DISPLAY_state.IsOption3DBoardOn;// Daves stuff
                                                // Russ' stuff
                                                if( DISPLAY_state.game3Don != DISPLAY_state.IsOption3DBoardOn )
                                                {
                                                  DISPLAY_state.game3Don = DISPLAY_state.IsOption3DBoardOn;
                                                  // Invalidate 2D buffers (city/currency may have changed since their compile)
                                                  for( int t = 0; t < UDBoard_2D_constructedBoardBufferMax; t++ )
                                                    UDBoard_2DLoadedViews[t].ViewLoaded = -1;

                                                  if( !DISPLAY_state.game3Don )
                                                  {// Force a camera update - camera can be on manual or anywhere
                                                    DISPLAY_state.manualMouseCamLock = FALSE;
                                                    DISPLAY_state.desiredCameraInvalidatedLock = TRUE;
                                                    DISPLAY_state.desiredCameraClearToValidate = TRUE;
                            
                                                  }
                                                }
                                                break;
                        case FILTERING:         if(IsOnOffButtonON[i])
                                                    DISPLAY_state.IsOptionFilteringOn = TRUE;
                                                else
                                                    DISPLAY_state.IsOptionFilteringOn = FALSE;
                                                DISPLAY_RenderSettings.bClassic_UseBilinearFiltering = DISPLAY_state.IsOptionFilteringOn;
                                                DISPLAY_RenderSettings.bCity_UseBilinearFiltering = DISPLAY_state.IsOptionFilteringOn;
                                                ENABLE_BILINEARFILTERING(0, DISPLAY_state.IsOptionFilteringOn);
                                                break;
                        case DITHERING:         if(IsOnOffButtonON[i])
                                                    DISPLAY_state.IsOptionDitheringOn = TRUE;
                                                else
                                                    DISPLAY_state.IsOptionDitheringOn = FALSE;
                                                DISPLAY_RenderSettings.bClassic_UseDithering = DISPLAY_state.IsOptionDitheringOn;
                                                DISPLAY_RenderSettings.bCity_UseDithering = DISPLAY_state.IsOptionDitheringOn;
                                                ENABLE_DITHERING(DISPLAY_state.IsOptionDitheringOn);
                                                break;

                        case RESOLUTION:        // get the current surface and compare with the new settings
                                                // Only reset if there is a change

                                                /* remove screen res stuff for now
                                                DWORD dwBPP, dwWidth, dwHeight;
                                                dwBPP = pc3D::GetRenderSurfaceBitDepth();
                                                Surface* pPrimary = pc3D::GetPrimarySurface();
                                                pPrimary->GetSize(dwWidth, dwHeight);
                                                if((dwBPP != (DWORD)ChosenBit[tempBitIndex]) ||
                                                    (dwWidth != (DWORD)ChosenWidth[tempResIndex]) ||
                                                    (dwHeight != (DWORD)ChosenHeight[tempResIndex]))
                                                {
                                                    DISPLAY_state.OptionScreenResIndex = tempResIndex;
                                                    DDRAW_SetINIMode(ChosenWidth[tempResIndex],
                                                        ChosenHeight[tempResIndex], ChosenBit[tempBitIndex]);
                                                }
                                                */
                                                break;
                        }
                }
                UDOPTIONS_SetBackdrop(last_playerselectscreen);
                //UDBOARD_SetBackdrop( last_playerselectscreen );

                break;

    case O_CANCEL:  UDOPTIONS_RemoveOptionScreen(); 
                    break;
    }   

    UDIBar_ProcessOptionsButtonPress(-1);
    UDOPTIONS_RemoveOptionScreen();

    }
  else if(Area == OPTION_ONOFF_BUTTONS_AREA)
  {
    if(button > -1)
    {
        UDSOUND_Click();

      // if the on button is not on
      if(!IsOnOffButtonON[button])
      {
        // stop previous on button
        LE_SEQNCR_Stop(OptionOnButtons[button].extraID, OptionOnButtons[button].Priority);
        //start the new on button
        LE_SEQNCR_StartXY(OptionOnButtons[button].ID, OptionOnButtons[button].Priority,
          OptionOnButtons[button].HotSpot.left, OptionOnButtons[button].HotSpot.top);
        LE_SEQNCR_SetEndingAction (OptionOnButtons[button].ID,  OptionOnButtons[button].Priority,
          LE_SEQNCR_EndingActionStayAtEnd);
                                
        // stop previous off button
        LE_SEQNCR_Stop(OptionOffButtons[button].ID, OptionOffButtons[button].Priority);
        // start the new off button
        LE_SEQNCR_StartXY(OptionOffButtons[button].extraID, OptionOffButtons[button].Priority,
          OptionOffButtons[button].HotSpot.left, OptionOffButtons[button].HotSpot.top);
        LE_SEQNCR_SetEndingAction (OptionOffButtons[button].extraID,  OptionOffButtons[button].Priority,
          LE_SEQNCR_EndingActionStayAtEnd);
        
        // set on button flag to true
        IsOnOffButtonON[button] = TRUE;
                                
      }
      // else if the on button is on
      else
      {
        // stop previous on button
        LE_SEQNCR_Stop(OptionOnButtons[button].ID, OptionOnButtons[button].Priority);
        //start the new on button
        LE_SEQNCR_StartXY(OptionOnButtons[button].extraID, OptionOnButtons[button].Priority,
          OptionOnButtons[button].HotSpot.left, OptionOnButtons[button].HotSpot.top);
        LE_SEQNCR_SetEndingAction (OptionOnButtons[button].extraID,  OptionOnButtons[button].Priority,
          LE_SEQNCR_EndingActionStayAtEnd);
                                
        // stop previous off button
        LE_SEQNCR_Stop(OptionOffButtons[button].extraID, OptionOffButtons[button].Priority);
        // start the new off button
        LE_SEQNCR_StartXY(OptionOffButtons[button].ID, OptionOffButtons[button].Priority,
          OptionOffButtons[button].HotSpot.left, OptionOffButtons[button].HotSpot.top);
        LE_SEQNCR_SetEndingAction (OptionOffButtons[button].ID,  OptionOffButtons[button].Priority,
          LE_SEQNCR_EndingActionStayAtEnd);
        
        // set on button flag to false
        IsOnOffButtonON[button] = FALSE;
                                
      }
    }
  }
  else if(Area == OPTION_TUNES_AREA)
  {
    ACHAR   szNum[100];
    
    // reset the old music index
    if(MusicChoices[DISPLAY_state.OptionMusicTuneIndex].IsObjectOn)
    {
#if USA_VERSION
        Asprintf(szNum, A_T("%s"), szMusicChoices[DISPLAY_state.OptionMusicTuneIndex]);
#else
        Asprintf(szNum, A_T("%i"), DISPLAY_state.OptionMusicTuneIndex+1);
#endif
      LE_GRAFIX_ColorArea(MusicChoices[DISPLAY_state.OptionMusicTuneIndex].ID, 0, 0, 
        MusicChoices[DISPLAY_state.OptionMusicTuneIndex].Width, 
        MusicChoices[DISPLAY_state.OptionMusicTuneIndex].Height,
        LE_GRAFIX_MakeColorRef(0,255,0));
      LE_FONTS_Print(MusicChoices[DISPLAY_state.OptionMusicTuneIndex].ID, 
        0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szNum);
      LE_SEQNCR_ForceRedraw(MusicChoices[DISPLAY_state.OptionMusicTuneIndex].ID,
        MusicChoices[DISPLAY_state.OptionMusicTuneIndex].Priority);
    }
    // set the new music index
    if(MusicChoices[button].IsObjectOn)
    {
#if USA_VERSION
        Asprintf(szNum, A_T("%s"), szMusicChoices[button]);
#else
        Asprintf(szNum, A_T("%i"), button+1);
#endif
      
      LE_GRAFIX_ColorArea(MusicChoices[button].ID, 0, 0, 
        MusicChoices[button].Width, 
        MusicChoices[button].Height,
        LE_GRAFIX_MakeColorRef(0,255,0));
      LE_FONTS_Print(MusicChoices[button].ID, 
        0, 0, LE_GRAFIX_MakeColorRef(0,0,255), szNum);
      LE_SEQNCR_ForceRedraw(MusicChoices[button].ID, MusicChoices[button].Priority);
      
      // set the index to preview the background music
      DISPLAY_state.OptionMusicTuneIndex = button;          
    }
  }
  else if(Area == OPTION_RESOLUTION_AREA)
  {
     
    // reset the old res index
    if(ScreenResChoices[tempResIndex].IsObjectOn)
    {
      LE_GRAFIX_ColorArea(ScreenResChoices[tempResIndex].ID, 0, 0, 
        ScreenResChoices[tempResIndex].Width, 
        ScreenResChoices[tempResIndex].Height,
        LE_GRAFIX_MakeColorRef(0,255,0));
      LE_FONTS_Print(ScreenResChoices[tempResIndex].ID, 
        0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szResolutionChoices[tempResIndex]);
      LE_SEQNCR_ForceRedraw(ScreenResChoices[tempResIndex].ID,
        ScreenResChoices[tempResIndex].Priority);
    }
    // set the new res index
    if(ScreenResChoices[button].IsObjectOn)
    {      
      LE_GRAFIX_ColorArea(ScreenResChoices[button].ID, 0, 0, 
        ScreenResChoices[button].Width, 
        ScreenResChoices[button].Height,
        LE_GRAFIX_MakeColorRef(0,255,0));
      LE_FONTS_Print(ScreenResChoices[button].ID, 
        0, 0, LE_GRAFIX_MakeColorRef(0,0,255), szResolutionChoices[button]);
      LE_SEQNCR_ForceRedraw(ScreenResChoices[button].ID, ScreenResChoices[button].Priority);
      
      // set the index to preview the background music
      tempResIndex = button;            
  
    }
  }
  else if(Area == OPTION_BIT_AREA)
  {
     
    // reset the old bit index
    if(BitChoices[tempBitIndex].IsObjectOn)
    {
      LE_GRAFIX_ColorArea(BitChoices[tempBitIndex].ID, 0, 0, 
        BitChoices[tempBitIndex].Width, 
        BitChoices[tempBitIndex].Height,
        LE_GRAFIX_MakeColorRef(0,255,0));
      LE_FONTS_Print(BitChoices[tempBitIndex].ID, 
        0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szBitChoices[tempBitIndex]);
      LE_SEQNCR_ForceRedraw(BitChoices[tempBitIndex].ID,
        BitChoices[tempBitIndex].Priority);
    }
    // set the new bit index
    if(BitChoices[button].IsObjectOn)
    {      
      LE_GRAFIX_ColorArea(BitChoices[button].ID, 0, 0, 
        BitChoices[button].Width, 
        BitChoices[button].Height,
        LE_GRAFIX_MakeColorRef(0,255,0));
      LE_FONTS_Print(BitChoices[button].ID, 
        0, 0, LE_GRAFIX_MakeColorRef(0,0,255), szBitChoices[button]);
      LE_SEQNCR_ForceRedraw(BitChoices[button].ID, BitChoices[button].Priority);
      
      // set the index to preview the background music
      tempBitIndex = button;            
  
    }
  }
}




/*****************************************************************************
    * Function:     void UDOPTIONS_ProcessCreditsOptionButtonPress(int button)
    * Returns:      nothing
    * Parameters:   index of button pressed
    *
    * Purpose:      Determines the action depending on which Credits options button
    *                           was pressed.
    ****************************************************************************
    */
void UDOPTIONS_ProcessCreditsOptionButtonPress(int button)
{        

    // if this credit roll is during the exit, simply set the exit flag
    // else go back to the previous screen
    if(g_UserChoseToExit)
    {
        GameQuitRequested = TRUE;
        g_UserChoseToExit = FALSE;
        UDOPTIONS_RemoveCreditsScreen();
        UDBOARD_SetBackdrop( DISPLAY_SCREEN_Black );
    }
    else
    {
        // simply remove the screen, no matter where the user clicks
        UDOPTIONS_RemoveCreditsScreen();
        UDOPTIONS_SetBackdrop(last_playerselectscreen);
        //UDBOARD_SetBackdrop( last_playerselectscreen );
        UDIBar_ProcessOptionsButtonPress(-1);
        //UDOPTIONS_RemoveFileScreen();
    }
}




/*****************************************************************************
    * Function:     void UDOPTIONS_ProcessHelpOptionButtonPress(int button, short Area)
    * Returns:      nothing
    * Parameters:   index of button pressed
    *
    * Purpose:      Determines the action depending on which Help options button
    *                           was pressed.
    ****************************************************************************
    */
void UDOPTIONS_ProcessHelpOptionButtonPress(int button, short Area)
{        

    static long tempHeight = 0;
    char szFile[MAX_PATH];
    char szPath[MAX_PATH];
    char szTemp[20], *szTmp;

#if !USA_VERSION
    // set the font size
    LE_FONTS_GetSettings(0);
    LE_FONTS_SetSize(12);
	LE_FONTS_SetWeight(700);
#endif

    if(Area == HELP_OPTION_AREA)
    {
        if(button > -1)
        {
            UDSOUND_Click();

            switch(button)
            {
                case H_QUICK_HELP:  if(!IsQuickHelpTextFound)
                                    {   // read the help file
                                        UDOPTIONS_ReadQuickHelpFile();
                                        if(!IsQuickHelpTextFound)
                                        {
                                            //MessageBox(NULL, "No Help File Found", "ERROR", MB_OK);
                                            UDSOUND_Warning();
                                            return;
                                        }
                                    }
                                    UDOPTIONS_DispalyQuickHelpFile(); 
                                    UDOPTIONS_RemoveHelpScreen(0);
                                    break;
                case H_FULL_HELP:   // Format the path to the help file
                                    sprintf(szTemp, "mono%02d%s", iLangId, ".hlp");
                                    sprintf(szFile, "\\mono%02d%s", iLangId, ".hlp");
                                    //sprintf(szTemp, "mono_us.hlp");
                                    //sprintf(szFile, "\\%s", szTemp);

                                    // Try to reference the help file
                                    if (!LE_DIRINI_ExpandFileName(szFile, szPath, LE_DIRINI_GlobalPathList))
                                    {
                                        //MessageBox(NULL, "No Help File Found", "ERROR", MB_OK);
                                        UDSOUND_Warning();
                                        return;
                                    }

                                    // Okay, we found the first template.  Extract the path from it.
                                    szTmp = strrchr(szPath, '\\');
                                    szTmp[1] = '\0';

                                    strcat(szPath, szTemp);

                                    if(!WinHelp(NULL,szPath, HELP_CONTENTS, 0))
                                    {
                                        UDSOUND_Warning();
                                        //MessageBox(NULL, "No Help File Found", "ERROR", MB_OK);
                                        return;
                                    }
                                    break;
                case H_CANCEL:      UDOPTIONS_SetBackdrop(last_playerselectscreen);
                                    //UDBOARD_SetBackdrop( last_playerselectscreen );
                                    UDIBar_ProcessOptionsButtonPress(-1);
                                    UDOPTIONS_RemoveHelpScreen(1);
                                    break;
                default: return;
            }
            
        }
    }
    else if(Area == HELP_TEXT_AREA)
    {
        if(button > -1)
        {
            UDSOUND_Click();

            switch(button)
            {
                case PREVIOUS:  if(HelpTextPageNo > 0)
                                {
                                    HelpTextPageNo--;
                                    // display help file
                                    if(HelpObject.IsObjectOn)
                                    {
                                        LE_GRAFIX_ColorArea(HelpObject.ID, 0, 0, 
                                                HelpObject.Width, HelpObject.Height,
                                                LE_GRAFIX_MakeColorRef(0,255,0));

                                        
                                        HelpTextCurrentHeight =     HelpTextPageNoArray[HelpTextPageNo];

                                        if(HelpTextCurrentHeight < 0)
                                            HelpTextCurrentHeight = 0;

                                        tempHeight = UDStats_GetStringHeight(szQuickHelp, 0,
                                                -HelpTextCurrentHeight,HelpObject.Width, HelpObject.var1);
                                        if((tempHeight-HelpTextCurrentHeight) > HelpObject.var1)
                                            tempHeight = HelpObject.var1 - LE_FONTS_GetStringHeight(A_T("TEST"));
                                        else
                                            tempHeight = HelpObject.var1;

                                        HelpTextOverallHeight = UDStats_PrintString(HelpObject.ID, 
                                                szQuickHelp, 0, -HelpTextCurrentHeight, 
                                                HelpObject.Width, tempHeight);
                                

                                        LE_SEQNCR_ForceRedraw(HelpObject.ID, HelpObject.Priority);

                                        // print the next button
                                        LE_FONTS_Print(HelpTextButtons[NEXT].ID, 0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szText[NEXT]);
                                        LE_SEQNCR_ForceRedraw(HelpTextButtons[NEXT].ID, HelpTextButtons[NEXT].Priority);

                                        if(HelpTextPageNo == 0)
                                        {
                                            LE_GRAFIX_ColorArea(HelpTextButtons[button].ID, 0, 0, HelpTextButtons[button].Width,
                                                HelpTextButtons[button].Height, LE_GRAFIX_MakeColorRef(0,255,0));
                                            LE_SEQNCR_ForceRedraw(HelpTextButtons[button].ID, HelpTextButtons[button].Priority);
                                        }
                                    }
                                }
                                
                                break;
                case NEXT:      if((HelpTextOverallHeight-HelpTextCurrentHeight) >= tempHeight)
                                {
                                    HelpTextPageNo++;
                                                                    
                                    // display help file
                                    if(HelpObject.IsObjectOn)
                                    {
                                        LE_GRAFIX_ColorArea(HelpObject.ID, 0, 0, 
                                                HelpObject.Width, HelpObject.Height,
                                                LE_GRAFIX_MakeColorRef(0,255,0));

                                        HelpTextCurrentHeight = HelpTextOverallHeight;

                                        tempHeight = UDStats_GetStringHeight(szQuickHelp, 0,
                                                -HelpTextCurrentHeight,HelpObject.Width, HelpObject.var1);
                                        if((tempHeight-HelpTextCurrentHeight) > HelpObject.var1)
                                            tempHeight = HelpObject.var1 - LE_FONTS_GetStringHeight(A_T("TEST"));
                                        else
                                            tempHeight = HelpObject.var1;

                                        HelpTextOverallHeight = UDStats_PrintString(HelpObject.ID, 
                                                szQuickHelp, 0, -HelpTextCurrentHeight, 
                                                HelpObject.Width, tempHeight);
                                
                                        // save the starting y coordinate
                                        HelpTextPageNoArray[HelpTextPageNo] = HelpTextCurrentHeight;

                                        LE_SEQNCR_ForceRedraw(HelpObject.ID, HelpObject.Priority);
                                        
                                        // print the previous button
                                        LE_FONTS_Print(HelpTextButtons[PREVIOUS].ID, 0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szText[PREVIOUS]);
                                        LE_SEQNCR_ForceRedraw(HelpTextButtons[PREVIOUS].ID, HelpTextButtons[PREVIOUS].Priority);

                                        if((HelpTextOverallHeight-HelpTextCurrentHeight) < tempHeight)
                                        {
                                            LE_GRAFIX_ColorArea(HelpTextButtons[button].ID, 0, 0, HelpTextButtons[button].Width,
                                                HelpTextButtons[button].Height, LE_GRAFIX_MakeColorRef(0,255,0));
                                            LE_SEQNCR_ForceRedraw(HelpTextButtons[button].ID, HelpTextButtons[button].Priority);
                                        }
                                    }
                                }                               
                                break;
                case CANCEL:    UDOPTIONS_RemoveQuickHelpFile(); 
                                UDOPTIONS_DisplayHelpScreen(current_options_result);
                                break;
                default: return;
            }
            
        }
    }
}




/*****************************************************************************
    * Function:     void UDOPTIONS_InitOptionOptionsScreen(void)
    * Returns:      nothing
    * Parameters:   nothing
    *                           
    * Purpose:      initialized the buttons and labels used in the option screen 
    ****************************************************************************
    */
void UDOPTIONS_InitOptionOptionsScreen(void)
{
    int i;

#if !USA_VERSION
    // set the font size
    LE_FONTS_GetSettings(0);
    LE_FONTS_SetSize(10);
	LE_FONTS_SetWeight(700);
#endif

    // initialize common values
    OptionsTitle.Priority = 50;
    OptionsTitle.IsObjectOn = FALSE;
    SoundSubTitleID = LED_IFT(DAT_LANG2, CNK_syosndoi);
    DisplaySubTitleID = LED_IFT(DAT_LANG2, CNK_syodispi);
    
    for(i=0; i<MAX_ONOFF_BUTTONS; i++)
    {
        // set the buttons
        OptionOnButtons[i].Priority = 50+i;
        OptionOnButtons[i].ID = LED_IFT(DAT_LANG2, CNK_syoonp);
        OptionOnButtons[i].extraID = LED_IFT(DAT_LANG2, CNK_syoono);
        OptionOnButtons[i].Width = 59;
        OptionOnButtons[i].Height = 33;
        OptionOnButtons[i].IsObjectOn = FALSE;

        OptionOffButtons[i].Priority = 50+i;
        OptionOffButtons[i].ID = LED_IFT(DAT_LANG2, CNK_syooffp);
        OptionOffButtons[i].extraID = LED_IFT(DAT_LANG2, CNK_syooffo);
        OptionOffButtons[i].Width = 59;
        OptionOffButtons[i].Height = 33;
        OptionOffButtons[i].IsObjectOn = FALSE;

        IsOnOffButtonON[i] = TRUE;

        // set the x and width
        switch(i)
        {
        case TOKEN_VOICES:
        case HOST_COMMENTS:
        case MUSIC:         OptionOnButtons[i].HotSpot.left = OnOffXPos[0];
                            OptionOnButtons[i].HotSpot.right = OnOffXPos[0]+OptionOnButtons[i].Width;
                            OptionOffButtons[i].HotSpot.left = OnOffXPos[0]+59;                         
                            OptionOffButtons[i].HotSpot.right = OnOffXPos[0]+59+OptionOnButtons[i].Width;
                            break;

        default:            OptionOnButtons[i].HotSpot.left = OnOffXPos[1];
                            OptionOnButtons[i].HotSpot.right = OnOffXPos[1]+OptionOnButtons[i].Width;
                            OptionOffButtons[i].HotSpot.left = OnOffXPos[1]+59;                         
                            OptionOffButtons[i].HotSpot.right = OnOffXPos[1]+59+OptionOnButtons[i].Width;
                            break;
        }
    }

    // set the y and height
    OptionOnButtons[TOKEN_VOICES].HotSpot.top = OptionOffButtons[TOKEN_VOICES].HotSpot.top = OnOffYPos[0];
    OptionOnButtons[TOKEN_VOICES].HotSpot.bottom = OptionOffButtons[TOKEN_VOICES].HotSpot.bottom = OnOffYPos[0]+33;
    OptionOnButtons[HOST_COMMENTS].HotSpot.top = OptionOffButtons[HOST_COMMENTS].HotSpot.top = OnOffYPos[1];
    OptionOnButtons[HOST_COMMENTS].HotSpot.bottom = OptionOffButtons[HOST_COMMENTS].HotSpot.bottom = OnOffYPos[1]+33;
    OptionOnButtons[MUSIC].HotSpot.top = OptionOffButtons[MUSIC].HotSpot.top = OnOffYPos[2];
    OptionOnButtons[MUSIC].HotSpot.bottom = OptionOffButtons[MUSIC].HotSpot.bottom = OnOffYPos[2]+33;
    OptionOnButtons[TOKEN_ANIMS].HotSpot.top = OptionOffButtons[TOKEN_ANIMS].HotSpot.top = OnOffYPos[0];
    OptionOnButtons[TOKEN_ANIMS].HotSpot.bottom = OptionOffButtons[TOKEN_ANIMS].HotSpot.bottom = OnOffYPos[0]+33;
    OptionOnButtons[CAMERA].HotSpot.top = OptionOffButtons[CAMERA].HotSpot.top = OnOffYPos[1];
    OptionOnButtons[CAMERA].HotSpot.bottom = OptionOffButtons[CAMERA].HotSpot.bottom = OnOffYPos[1]+33;
    OptionOnButtons[LIGHTING].HotSpot.top = OptionOffButtons[LIGHTING].HotSpot.top = OnOffYPos[2];
    OptionOnButtons[LIGHTING].HotSpot.bottom = OptionOffButtons[LIGHTING].HotSpot.bottom = OnOffYPos[2]+33;
    OptionOnButtons[_3DBOARD].HotSpot.top = OptionOffButtons[_3DBOARD].HotSpot.top = OnOffYPos[3];
    OptionOnButtons[_3DBOARD].HotSpot.bottom = OptionOffButtons[_3DBOARD].HotSpot.bottom = OnOffYPos[3]+33;
    OptionOnButtons[FILTERING].HotSpot.top = OptionOffButtons[FILTERING].HotSpot.top = OnOffYPos[4];
    OptionOnButtons[FILTERING].HotSpot.bottom = OptionOffButtons[FILTERING].HotSpot.bottom = OnOffYPos[4]+33;
    OptionOnButtons[DITHERING].HotSpot.top = OptionOffButtons[DITHERING].HotSpot.top = OnOffYPos[5];
    OptionOnButtons[DITHERING].HotSpot.bottom = OptionOffButtons[DITHERING].HotSpot.bottom = OnOffYPos[5]+33;
    OptionOnButtons[RESOLUTION].HotSpot.top = OptionOffButtons[RESOLUTION].HotSpot.top = OnOffYPos[6];
    OptionOnButtons[RESOLUTION].HotSpot.bottom = OptionOffButtons[RESOLUTION].HotSpot.bottom = OnOffYPos[6]+33;

    // set the button labels
    // set the labels
    Astrcpy(szOptionChoices[TOKEN_VOICES], LANG_GetTextMessage(TMN_TOKEN_VOICES));
    Astrcpy(szOptionChoices[MUSIC], LANG_GetTextMessage(TMN_MUSIC));
    Astrcpy(szOptionChoices[_3DBOARD], LANG_GetTextMessage(TMN_3D_BOARD_DISPLAY));
    Astrcpy(szOptionChoices[FILTERING], LANG_GetTextMessage(TMN_BILINEAR_FILTERING));
    Astrcpy(szOptionChoices[DITHERING], LANG_GetTextMessage(TMN_DITHERING));
    Astrcpy(szOptionChoices[RESOLUTION], LANG_GetTextMessage(TMN_SCREEN_RESOLUTION));
    Astrcpy(szOptionChoices[HOST_COMMENTS], LANG_GetTextMessage(TMN_HOST_COMMENTS));
    Astrcpy(szOptionChoices[TOKEN_ANIMS], LANG_GetTextMessage(TMN_TOKEN_ANIMATIONS));
    Astrcpy(szOptionChoices[CAMERA], LANG_GetTextMessage(TMN_3D_CAMERA_MOVEMENT));
    Astrcpy(szOptionChoices[LIGHTING], LANG_GetTextMessage(TMN_LIGHTING));
    

    
        
    for(i=0; i<MAX_ONOFF_BUTTONS; i++)
    {
        OptionChoices[i].Priority = 50;
        OptionChoices[i].Width = 200;//LE_FONTS_GetStringWidth(szOptionChoices[i]);
        OptionChoices[i].Height = LE_FONTS_GetStringHeight(szOptionChoices[i])*2;
        OptionChoices[i].ID = LE_GRAFIX_ObjectCreate(OptionChoices[i].Width, OptionChoices[i].Height,
                                LE_GRAFIX_ObjectTransparent);
        LE_GRAFIX_ColorArea(OptionChoices[i].ID, 0, 0, OptionChoices[i].Width, OptionChoices[i].Height,
                        LE_GRAFIX_MakeColorRef(0,255,0));

        OptionChoices[i].HotSpot.left = OptionOffButtons[i].HotSpot.right + 10;
#if USA_VERSION
        OptionChoices[i].HotSpot.top = OptionOffButtons[i].HotSpot.top + 5;
#else
        OptionChoices[i].HotSpot.top = OptionOffButtons[i].HotSpot.top;
#endif
        OptionChoices[i].HotSpot.right = OptionChoices[i].HotSpot.left + OptionChoices[i].Width;
        OptionChoices[i].HotSpot.bottom = OptionChoices[i].HotSpot.top + OptionChoices[i].Height;
        OptionChoices[i].IsObjectOn = FALSE;
#if USA_VERSION
        LE_FONTS_Print(OptionChoices[i].ID, 0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szOptionChoices[i]);
#else
        if((UDStats_GetStringHeight(szOptionChoices[i], 0, 0, OptionChoices[i].Width, OptionChoices[i].Height))
            > (OptionChoices[i].Height/2))
        {
            UDStats_PrintString(OptionChoices[i].ID, szOptionChoices[i], 0, 0, 
                OptionChoices[i].Width, OptionChoices[i].Height);
        }
        else
        {
            UDStats_PrintString(OptionChoices[i].ID, szOptionChoices[i], 0, 5, 
                OptionChoices[i].Width, OptionChoices[i].Height);
        }
#endif
    }

    // set the background music choices
    ACHAR   szNum[100];
    int x = OptionOffButtons[MUSIC].HotSpot.right + 10;
    int y = OptionOffButtons[MUSIC].HotSpot.bottom;

    for(i=0; i<MAX_TUNES; i++)
    {
        MusicChoices[i].Priority = 50;
#if USA_VERSION
        Asprintf(szNum, A_T("%s"), szMusicChoices[i]);
#else
        Asprintf(szNum, A_T("%i"), i+1);
#endif
        
        MusicChoices[i].Width = LE_FONTS_GetStringWidth(szNum);
        MusicChoices[i].Height = LE_FONTS_GetStringHeight(szNum);
        MusicChoices[i].ID = LE_GRAFIX_ObjectCreate(MusicChoices[i].Width, MusicChoices[i].Height,
                                LE_GRAFIX_ObjectTransparent);
        LE_GRAFIX_ColorArea(MusicChoices[i].ID, 0, 0, MusicChoices[i].Width, MusicChoices[i].Height,
                        LE_GRAFIX_MakeColorRef(0,255,0));

        MusicChoices[i].HotSpot.left = x + 10;
        MusicChoices[i].HotSpot.top = y + 5;
        MusicChoices[i].HotSpot.right = MusicChoices[i].HotSpot.left + MusicChoices[i].Width;
        MusicChoices[i].HotSpot.bottom = MusicChoices[i].HotSpot.top + MusicChoices[i].Height;
        MusicChoices[i].IsObjectOn = FALSE;

        LE_FONTS_Print(MusicChoices[i].ID, 0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szNum);

        y+= MusicChoices[i].Height;
    }

    // set the screen resolution choices

    // first get the possible screen resolutions
    DDrawDriver*    pDDDriver = pc3D::GetDDrawDriver();
    ASSERT(NULL != pDDDriver);

    SurfDescArray   aDisplayModes;
    pDDDriver->GetDisplayModeList(aDisplayModes, 800/*Min width*/,1600/*Max width*/,
                        600/*Min height*/,1280/*Max height*/, _16BIT | _32BIT);

    
    //int x = OptionChoices[MUSIC].HotSpot.right;
    y = 395;

    // counter to keep track of the amount of display choices
    i = 0;
    int cnt, cnt2;
    int result;

    // set the dimension display
    ACHAR szTempRes[20];
    
    BOOL bHardware = pc3D::Using3DHardware();
    DWORD dwMemRequired = 0; 
    
    for(cnt=0; cnt < aDisplayModes.GetSize(); cnt++)
    {
        if((aDisplayModes[cnt].dwWidth == 1152) || (aDisplayModes[cnt].dwWidth == 1280))
            continue;

        // Calculate the required amount of memory
        dwMemRequired = aDisplayModes[cnt].dwWidth * aDisplayModes[cnt].dwHeight *
                        ((aDisplayModes[cnt].ddpfPixelFormat.dwRGBBitCount / 4) + 2);
                        
        // If the requested mode does not have enough memory, we don't want to include it
        if (bHardware && dwMemRequired >= pDDDriver->GetDriverMemory())
          continue;


        ZeroMemory(szTempRes, 20);

        // copy the the dimension into the string for comparaison
        Asprintf(szTempRes, A_T("%ix%i"), aDisplayModes[cnt].dwWidth, aDisplayModes[cnt].dwHeight);

        result = -1;
        for(cnt2=0; cnt2 < 5; cnt2++)
        {
            if(Astrcmp(szTempRes, szResolutionChoices[cnt2]) == 0)
            {
                result = cnt2;
            }
        }

        if(result == -1)
        {
            ScreenResChoices[i].Priority = 50;
            Asprintf(szResolutionChoices[i], A_T("%ix%i"), aDisplayModes[cnt].dwWidth, aDisplayModes[cnt].dwHeight);
            ScreenResChoices[i].Width = LE_FONTS_GetStringWidth(szResolutionChoices[i]);
            ScreenResChoices[i].Height = LE_FONTS_GetStringHeight(szResolutionChoices[i]);
            ScreenResChoices[i].ID = LE_GRAFIX_ObjectCreate(ScreenResChoices[i].Width, ScreenResChoices[i].Height,
                                LE_GRAFIX_ObjectTransparent);
            LE_GRAFIX_ColorArea(ScreenResChoices[i].ID, 0, 0, ScreenResChoices[i].Width, ScreenResChoices[i].Height,
                        LE_GRAFIX_MakeColorRef(0,255,0));

            ScreenResChoices[i].HotSpot.left = 570;
            ScreenResChoices[i].HotSpot.top = y + 5;
            ScreenResChoices[i].HotSpot.right = ScreenResChoices[i].HotSpot.left + ScreenResChoices[i].Width;
            ScreenResChoices[i].HotSpot.bottom = ScreenResChoices[i].HotSpot.top + ScreenResChoices[i].Height;
            ScreenResChoices[i].IsObjectOn = FALSE;

            LE_FONTS_Print(ScreenResChoices[i].ID, 0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szResolutionChoices[i]);

            y+= ScreenResChoices[i].Height;

            // set the actuall screen width and height
            ChosenWidth[i] = aDisplayModes[cnt].dwWidth;
            ChosenHeight[i] = aDisplayModes[cnt].dwHeight;

            if(aDisplayModes[cnt].ddpfPixelFormat.dwRGBBitCount == 16)
            {
                g_MaxBitChoices[i] += _16_BIT_CHOICE;
            }
            else if(aDisplayModes[cnt].ddpfPixelFormat.dwRGBBitCount == 32)
            {
                g_MaxBitChoices[i] += _32_BIT_CHOICE;
            }

            i++;
        }
        else
        {
            if(aDisplayModes[cnt].ddpfPixelFormat.dwRGBBitCount == 16)
            {
                g_MaxBitChoices[result] += _16_BIT_CHOICE;
            }
            else if(aDisplayModes[cnt].ddpfPixelFormat.dwRGBBitCount == 32)
            {
                g_MaxBitChoices[result] += _32_BIT_CHOICE;
            }
        }

    }

    g_MaxResChoices = i;

    // set the bit displays
    y = 395;
    Astrcpy(szBitChoices[0], LANG_GetTextMessage(TMN_16_BIT));
    Astrcpy(szBitChoices[1], LANG_GetTextMessage(TMN_32_BIT));

    for(i=0; i<2; i++)
    {        
        BitChoices[i].Width = LE_FONTS_GetStringWidth(szBitChoices[i]);
        BitChoices[i].Height = LE_FONTS_GetStringHeight(szBitChoices[i]);
        BitChoices[i].ID = LE_GRAFIX_ObjectCreate(BitChoices[i].Width, BitChoices[i].Height,
                            LE_GRAFIX_ObjectTransparent);
        LE_GRAFIX_ColorArea(BitChoices[i].ID, 0, 0, BitChoices[i].Width, BitChoices[i].Height,
                    LE_GRAFIX_MakeColorRef(0,255,0));

        BitChoices[i].HotSpot.left = 700;
        BitChoices[i].HotSpot.top = y + 5;
        BitChoices[i].HotSpot.right = BitChoices[i].HotSpot.left + BitChoices[i].Width;
        BitChoices[i].HotSpot.bottom = BitChoices[i].HotSpot.top + BitChoices[i].Height;
        BitChoices[i].IsObjectOn = FALSE;

        LE_FONTS_Print(BitChoices[i].ID, 0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szBitChoices[i]);

        y+= BitChoices[i].Height;

        ChosenBit[i] = 16 + 16*i;

    }
                        
}





/*****************************************************************************
    * Function:     void UDOPTIONS_InitCreditsOptionsScreen(void)
    * Returns:      nothing
    * Parameters:   nothing
    *                           
    * Purpose:      initialized the buttons and labels used in the credits screen 
    ****************************************************************************
    */
void UDOPTIONS_InitCreditsOptionsScreen(void)
{
    // get the credits tab and load it
    CreditsObject.extraID = LED_IFT(DAT_LANG2, TAB_credits);
    LE_DATA_Load (CreditsObject.extraID);
    CreditsObject.Priority = 10;
    // set the object to display on screen
    CreditsObject.Width = LE_GRAFIX_ReturnObjectWidth(CreditsObject.extraID);
    CreditsObject.Height = UDOPTSBarTop;
    CreditsObject.ID = LE_GRAFIX_ObjectCreate(CreditsObject.Width, CreditsObject.Height,
                        LE_GRAFIX_ObjectTransparent);
    LE_GRAFIX_ColorArea(CreditsObject.ID, 0, 0, CreditsObject.Width, CreditsObject.Height, 
            LE_GRAFIX_MakeColorRef(0,255,0));
        

     CreditsObject.IsObjectOn = FALSE;
    // set it's position on screen
    CreditsObject.var1 = 800/2 - CreditsObject.Width/2;
    //CreditsObject.var1 = (LE_GRAFIX_ScreenDestinationRect.right - 
    //                    LE_GRAFIX_ScreenDestinationRect.left)/2 - CreditsObject.Width/2;

    
    CreditsObject.var2 = 0;

    // set the coordinates of the scrolling text
    CreditsObject.HotSpot.left = CreditsObject.var1;
    CreditsObject.HotSpot.top = CreditsObject.Height;
    CreditsObject.HotSpot.bottom = CreditsObject.Height + LE_GRAFIX_ReturnObjectHeight(CreditsObject.extraID);
    LE_GRAFIX_ShowTCB_Alpha(CreditsObject.ID, 0, (short)CreditsObject.HotSpot.top, CreditsObject.extraID); 
}





/*****************************************************************************
    * Function:     void UDOPTIONS_InitHelpOptionsScreen(void)
    * Returns:      nothing
    * Parameters:   nothing
    *                           
    * Purpose:      initialized the object that will display the help file
    ****************************************************************************
    */
void UDOPTIONS_InitHelpOptionsScreen(void)
{
    int i;

#if !USA_VERSION
    // set the font size
    LE_FONTS_GetSettings(0);
    LE_FONTS_SetSize(12);
	LE_FONTS_SetWeight(700);
#endif

    HelpObject.Priority = 10;
    // set the object to display on screen
    HelpObject.Width = 600;
    HelpObject.Height = UDIBAR_IBarTop;
    HelpObject.ID = LE_GRAFIX_ObjectCreate(HelpObject.Width, HelpObject.Height,
                        LE_GRAFIX_ObjectTransparent);
    LE_GRAFIX_ColorArea(HelpObject.ID, 0, 0, HelpObject.Width, HelpObject.Height, 
            LE_GRAFIX_MakeColorRef(0,255,0));
        
    HelpObject.IsObjectOn = FALSE;

    SetRect(&(HelpObject.HotSpot), 100, 0, 700, HelpObject.Height);

    // set an extra variable to hold the allowed display height
    HelpObject.var1 = HelpObject.Height - LE_FONTS_GetStringHeight(A_T("TEST"));

    
    int     tempX[3] = {100, 650, 400};
     
    Astrcpy(szText[0], LANG_GetTextMessage(TMN_BACK));
    Astrcpy(szText[1], LANG_GetTextMessage(TMN_NEXT));
    Astrcpy(szText[2], LANG_GetTextMessage(TMN_BUTTON_CANCEL));
                     
    for(i=0; i<3; i++)
    {
        HelpTextButtons[i].Priority = 50;
        HelpTextButtons[i].Width = LE_FONTS_GetStringWidth(szText[i]);
        HelpTextButtons[i].Height = LE_FONTS_GetStringHeight(szText[i]);
        HelpTextButtons[i].ID = LE_GRAFIX_ObjectCreate(HelpTextButtons[i].Width, HelpTextButtons[i].Height,
                                LE_GRAFIX_ObjectTransparent);
        LE_GRAFIX_ColorArea(HelpTextButtons[i].ID, 0, 0, HelpTextButtons[i].Width, HelpTextButtons[i].Height,
                        LE_GRAFIX_MakeColorRef(0,255,0));

        HelpTextButtons[i].HotSpot.left = tempX[i];
        HelpTextButtons[i].HotSpot.top = 455;
        HelpTextButtons[i].HotSpot.right = HelpTextButtons[i].HotSpot.left + HelpTextButtons[i].Width;
        HelpTextButtons[i].HotSpot.bottom = HelpTextButtons[i].HotSpot.top + HelpTextButtons[i].Height;
        HelpTextButtons[i].IsObjectOn = FALSE;

        LE_FONTS_Print(HelpTextButtons[i].ID, 0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szText[i]);
        
    }
}






/*****************************************************************************
    * Function:     void UDOPTIONS_ReadQuickHelpFile(ACHAR *buffer)
    * Returns:      nothing
    * Parameters:   nothing
    *                           
    * Purpose:      reads the text file created for the quick help option 
    ****************************************************************************
    */
void UDOPTIONS_ReadQuickHelpFile(void)
{
    int fh;
    FILE*   fileptr = NULL;
    unsigned int nbytes = 0, i=0;
    char szFile[MAX_PATH];
    char szPath[MAX_PATH];
    char szTemp[20];
    char *tempBuffer;
    
    // Format the path to the help file
    sprintf(szTemp, "qhelp%02d%s", iLangId, ".txt");
    sprintf(szFile, "\\qhelp%02d%s", iLangId, ".txt");
    //strcpy(szFile, "quick help.txt");

    // Try to reference the help file
    if (!LE_DIRINI_ExpandFileName(szFile, szPath, LE_DIRINI_GlobalPathList))
    {
        IsQuickHelpTextFound = FALSE;
        return;
    }

    // Okay, we found the first template.  Extract the path from it.
    char* szTmp = strrchr(szPath, '\\');
    szTmp[1] = '\0';

    strcat(szPath, szTemp);

    
    if( (fh = _open( szPath, _O_RDONLY))  != -1 )
    {
        // get the size of the file
        nbytes = _filelength( fh ) + 1;
        
        //create the buffer
        tempBuffer = (char*)malloc(nbytes);
        _close(fh);
    }

    // open the file
    if((fileptr = fopen(szPath, "rb")) == NULL)
    {
        UDSOUND_Warning();
        //MessageBox(NULL, szPath, "COULD NOT OPEN...", MB_OK);
        IsQuickHelpTextFound = FALSE;
        return;
    }
    
    // try to read the file
    //fread(tempBuffer, sizeof(char), nbytes, fileptr);
    /* Read in first 80 characters and place them in "buffer": */
   int ch = fgetc( fileptr );
   for( i=0; (i < (nbytes-1) ) && ( feof( fileptr ) == 0 ); i++ )
   {
      tempBuffer[i] = (char)ch;
      ch = fgetc( fileptr );
   }

   /* Add null to end string */
   tempBuffer[i] = '\0';
   

    /*result = fread(szQuickHelp, sizeof(char), 1, fileptr);
    while(result > 0)
        result = fread(szQuickHelp, sizeof(char), 1, fileptr);
    */
    fclose(fileptr);

    //unsigned long size = strlen(tempBuffer) * sizeof(ACHAR);
    szQuickHelp = new ACHAR[nbytes];//(ACHAR*)malloc(size);
    ZeroMemory(szQuickHelp, nbytes);

    mbstowcs( szQuickHelp, tempBuffer, nbytes);

    free(tempBuffer);

    IsQuickHelpTextFound = TRUE;
}




/*****************************************************************************
    * Function:     void UDOPTIONS_DispalyQuickHelpFile(void)
    * Returns:      nothing
    * Parameters:   nothing
    *                           
    * Purpose:      displays the text file created for the quick help option 
    ****************************************************************************
    */
void UDOPTIONS_DispalyQuickHelpFile(void)
{
    int i;

#if !USA_VERSION
    // set the font size
    LE_FONTS_GetSettings(0);
    LE_FONTS_SetSize(12);
	LE_FONTS_SetWeight(700);
#endif

    // clear the helpdisplay
    LE_GRAFIX_ColorArea(HelpObject.ID, 0, 0, HelpObject.Width, HelpObject.Height, 
            LE_GRAFIX_MakeColorRef(0,255,0));
    // set to first page
    HelpTextPageNo = 0;
    HelpTextCurrentHeight = 0;
    HelpTextOverallHeight = UDStats_PrintString(HelpObject.ID, szQuickHelp, 0, 0, 
            HelpObject.Width, HelpObject.var1);

    // save the starting y coordinate
    HelpTextPageNoArray[HelpTextPageNo] = 0;
    // display help file
    if(!HelpObject.IsObjectOn)
    {
        LE_SEQNCR_StartXY(HelpObject.ID, HelpObject.Priority,
                    HelpObject.HotSpot.left, HelpObject.HotSpot.top);
        HelpObject.IsObjectOn = TRUE;
    }

    // display helptext buttons
    for(i=0; i<3; i++)
    {
        if(!HelpTextButtons[i].IsObjectOn)
        {
            if(i == PREVIOUS)
            {
                LE_GRAFIX_ColorArea(HelpTextButtons[i].ID, 0, 0, HelpTextButtons[i].Width,
                        HelpTextButtons[i].Height, LE_GRAFIX_MakeColorRef(0,255,0));
            }
            else if(i == NEXT)
            {
                LE_FONTS_Print(HelpTextButtons[i].ID, 0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szText[i]);
            }
            
            LE_SEQNCR_StartXY(HelpTextButtons[i].ID, HelpTextButtons[i].Priority,
                        HelpTextButtons[i].HotSpot.left, HelpTextButtons[i].HotSpot.top);
            HelpTextButtons[i].IsObjectOn = TRUE;
        }
    }
}



/*****************************************************************************
    * Function:     void UDOPTIONS_RemoveQuickHelpFile(void)
    * Returns:      nothing
    * Parameters:   nothing
    *                           
    * Purpose:      displays the text file created for the quick help option 
    ****************************************************************************
    */
void UDOPTIONS_RemoveQuickHelpFile(void)
{
    int i;

    // remove text file
    if(HelpObject.IsObjectOn)
    {
        LE_SEQNCR_Stop(HelpObject.ID, HelpObject.Priority);
        HelpObject.IsObjectOn = FALSE;
    }

    // remove helptext buttons
    for(i=0; i<3; i++)
    {
        if(HelpTextButtons[i].IsObjectOn)
        {
            LE_SEQNCR_Stop(HelpTextButtons[i].ID, HelpTextButtons[i].Priority);
            HelpTextButtons[i].IsObjectOn = FALSE;
        }
    }
}




/*****************************************************************************/
/*****************************************************************************
    * Function:     void UDOPTIONS_InitCustomBoardDialogBoxes(void)
    *
    * Returns:      nothing
    * Parameters:   nothing
    * Purpose:      initialize custom board dialogs
    *               
    ****************************************************************************
    */
void UDOPTIONS_InitCustomBoardDialogBoxes(void)
{
    int i;

    // set load dialog box]
    LoadDialogBox.Width = 500;
    LoadDialogBox.Height = 400;
    LoadDialogBox.extraID = LE_GRAFIX_ObjectCreate(500, 400, LE_GRAFIX_ObjectNotTransparent);
    LE_GRAFIX_ColorArea(LoadDialogBox.extraID, 0,0,500, 400,  LE_GRAFIX_MakeColorRef(0,0,0));
    LoadDialogBox.Priority = 100;

    // set save dialog box
    SaveDialogBox.Width = 500;
    SaveDialogBox.Height = 400;
    SaveDialogBox.extraID = LE_GRAFIX_ObjectCreate(500, 400, LE_GRAFIX_ObjectNotTransparent);
    LE_GRAFIX_ColorArea(SaveDialogBox.extraID, 0,0,500, 400,  LE_GRAFIX_MakeColorRef(0,0,0));
    SaveDialogBox.Priority = 100;

    // set load board dialog 
    LoadBoardDialog.Width = 500;
    LoadBoardDialog.Height = 400;
    LoadBoardDialog.extraID = LE_GRAFIX_ObjectCreate(500, 400, LE_GRAFIX_ObjectNotTransparent);
    LE_GRAFIX_ColorArea(LoadBoardDialog.extraID, 0,0,500, 400,  LE_GRAFIX_MakeColorRef(0,0,0));
    LoadBoardDialog.Priority = 100;


    // set the dialog buttons
    Astrcpy(szDialogText[0], LANG_GetTextMessage(TMN_BUTTON_OK));
    Astrcpy(szDialogText[1], LANG_GetTextMessage(TMN_BUTTON_CANCEL));
    Astrcpy(szDialogText[2], LANG_GetTextMessage(TMN_BACK));
    Astrcpy(szDialogText[3], LANG_GetTextMessage(TMN_NEXT));

    for(i=0; i<2; i++)
    {
        DialogButtons[i].Priority = 150;
        DialogButtons[i].Width = LE_FONTS_GetStringWidth(szDialogText[i]);
        DialogButtons[i].Height = LE_FONTS_GetStringHeight(szDialogText[i]);
        DialogButtons[i].ID = LE_GRAFIX_ObjectCreate(DialogButtons[i].Width, DialogButtons[i].Height,
                                LE_GRAFIX_ObjectTransparent);
        LE_GRAFIX_ColorArea(DialogButtons[i].ID, 0, 0, DialogButtons[i].Width, DialogButtons[i].Height,
                        LE_GRAFIX_MakeColorRef(0,255,0));

        DialogButtons[i].HotSpot.left = 500 + i*100;
        DialogButtons[i].HotSpot.top = 430;
        DialogButtons[i].HotSpot.right = DialogButtons[i].HotSpot.left + DialogButtons[i].Width;
        DialogButtons[i].HotSpot.bottom = DialogButtons[i].HotSpot.top + DialogButtons[i].Height;
        DialogButtons[i].IsObjectOn = FALSE;

        LE_FONTS_Print(DialogButtons[i].ID, 0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szDialogText[i]);
        
    }

    // initialize the load title, save title and the game slots
    LoadGameTitle.Priority = 100;
    LoadGameTitle.ID = LED_IFT(DAT_LANG2, CNK_syldgm00);

    SaveGameTitle.Priority = 100;
    SaveGameTitle.ID = LED_IFT(DAT_LANG2, CNK_sysvgm00);

    LoadBoardTitle.Priority = 100;
    LoadBoardTitle.ID = LED_IFT(DAT_LANG2, CNK_ldbrd);

    int y = 100;
    // game slots
    for(i=0; i<5; i++)
    {
        GameSlots[i].Priority = 100+i;
        GameSlots[i].var1 = CNK_snfileb0;
        GameSlots[i].var2 = CNK_snfilegl;
        GameSlots[i].ID = LED_IFT(DAT_MAIN, CNK_snfileb0);
        GameSlots[i].Width = 496;
        GameSlots[i].Height = 34;
        GameSlots[i].extraID = LE_GRAFIX_ObjectCreate(496, 34, LE_GRAFIX_ObjectTransparent);
        LE_GRAFIX_ColorArea(GameSlots[i].extraID, 0, 0, 496, 34, LE_GRAFIX_MakeColorRef(0,255,0));
        GameSlots[i].IsObjectOn = FALSE;
        GameSlots[i].HotSpot.left = 100;
        GameSlots[i].HotSpot.top = y + i*GameSlots[i].Height*2;
        GameSlots[i].HotSpot.right = GameSlots[i].HotSpot.left + GameSlots[i].Width;
        GameSlots[i].HotSpot.bottom = GameSlots[i].HotSpot.top + GameSlots[i].Height;
    }

    // initialize the previous and cancel buttons for the scrolling of the board file names
    for(i=2; i<4; i++)
    {
        DialogButtons[i].Priority = 150;
        DialogButtons[i].Width = LE_FONTS_GetStringWidth(szDialogText[i]);
        DialogButtons[i].Height = LE_FONTS_GetStringHeight(szDialogText[i]);
        DialogButtons[i].ID = LE_GRAFIX_ObjectCreate(DialogButtons[i].Width, DialogButtons[i].Height,
                                LE_GRAFIX_ObjectTransparent);
        LE_GRAFIX_ColorArea(DialogButtons[i].ID, 0, 0, DialogButtons[i].Width, DialogButtons[i].Height,
                        LE_GRAFIX_MakeColorRef(0,255,0));

        DialogButtons[i].HotSpot.left = 100 + (i-2)*600;
        DialogButtons[i].HotSpot.top = 430;
        DialogButtons[i].HotSpot.right = DialogButtons[i].HotSpot.left + DialogButtons[i].Width;
        DialogButtons[i].HotSpot.bottom = DialogButtons[i].HotSpot.top + DialogButtons[i].Height;
        DialogButtons[i].IsObjectOn = FALSE;

        LE_FONTS_Print(DialogButtons[i].ID, 0, 0, LE_GRAFIX_MakeColorRef(255,255,255), szDialogText[i]);
        
    }

}




/*****************************************************************************/
/*****************************************************************************
    * Function:     void UDOPTIONS_DisplayLoadDialogBox(void)
    *
    * Returns:      nothing
    * Parameters:   nothing
    * Purpose:      Load a game using the new dialog box art
    *               
    ****************************************************************************
    */
void UDOPTIONS_DisplayLoadDialogBox(void)
{
    char        szFileInfo[5][500];
    int         counter = 0, i;
    //char        szTempDesc[500];
    FILE*       fileptr = NULL;
    char        szSlotNumber;
    int         slotNumber[5] = {-1,-1,-1,-1,-1};

    // switch the font size
    LE_FONTS_GetSettings(3);

    //char szFile[MAX_PATH];
    char szPath[MAX_PATH];
    char szPath2[MAX_PATH];
/*
    // Format the path to the texture templates for medium mesh textures
    sprintf(szFile, "\\%s", "directx.ini");

    // Try to reference the first texture
    if (!LE_DIRINI_ExpandFileName(szFile, szPath, LE_DIRINI_GlobalPathList))
      szPath[0] = '\0';
    
    // Okay, we found the first template.  Extract the path from it.
    char* szTmp = strrchr(szPath, '\\');
    szTmp[1] = '\0';
*/
    UDUTILS_GetModulePath(szPath, MAX_PATH);

    strcpy(szPath2, szPath);
    strcat(szPath2, "savegame\\");
    strcat(szPath, "savegame\\*.sgd");
    

    struct _finddata_t Game_files;
    long hFile;
    
    // Find first .brd file in current directory 
    if( (hFile = _findfirst( szPath, &Game_files )) != -1L )
    {
        strcpy( szFileInfo[counter], szPath2);
        strcat( szFileInfo[counter], Game_files.name);
        strncpy( &szSlotNumber, Game_files.name+4, 1 );
        slotNumber[counter] = atoi(&szSlotNumber)-1;
        counter++;
        /* Find the rest of the .c files */
        while(( _findnext( hFile, &Game_files ) == 0 ) && (counter < 5))
        {
            strcpy( szFileInfo[counter], szPath2);
            strcat( szFileInfo[counter], Game_files.name);
            strncpy( &szSlotNumber, Game_files.name+4, 1 );
            slotNumber[counter] = atoi(&szSlotNumber)-1;
            counter++;
        }

        _findclose( hFile );
    }

    // read the description of the same game contained in the *.sgd files
    for(i=0; i<counter; i++)
    {
        ZeroMemory(&(SaveGameStuff[slotNumber[i]]), sizeof(SaveGameStruct));

        fileptr = fopen(szFileInfo[i], "rb");
        if(fileptr == NULL)
            continue;        

        fread(&(SaveGameStuff[slotNumber[i]]), sizeof(SaveGameStruct), 1, fileptr );
        //if(fgets(szTempDesc, 500, fileptr))
        //    mbstowcs( SaveGameStuff[slotNumber[i]].GameFileDesc, szTempDesc, strlen(szTempDesc) );

        fclose(fileptr);
    }

    int tempHeight = LE_FONTS_GetStringHeight(A_T("TEST"));

    // clear the text and the rects
    for(i=0; i<5; i++)
    {
        LE_GRAFIX_ColorArea(GameSlots[i].extraID, 0, 0, (short)GameSlots[i].Width,
                GameSlots[i].Height,  LE_GRAFIX_MakeColorRef(0,255,0));  
        SetRect(&GameFileNameRects[i], 0,0,0,0);
    }
    
    for(i=0; i<counter; i++)
    {
        
        //LE_GRAFIX_ColorArea(GameSlots[slotNumber[i]].extraID, 0, 0, (short)GameSlots[slotNumber[i]].Width,
        //        GameSlots[slotNumber[i]].Height,  LE_GRAFIX_MakeColorRef(0,255,0));  

        LE_FONTS_Print(GameSlots[slotNumber[i]].extraID, 13, 8, LE_GRAFIX_MakeColorRef(255,255,255),
                SaveGameStuff[slotNumber[i]].GameFileDesc);
        
        SetRect(&GameFileNameRects[slotNumber[i]], GameSlots[slotNumber[i]].HotSpot.left + 52,
            GameSlots[slotNumber[i]].HotSpot.top + 12, GameSlots[slotNumber[i]].HotSpot.right+52,
            GameSlots[slotNumber[i]].HotSpot.bottom + 12);
        
    }

    for(i=0; i<5; i++)
    {
        if(!GameSlots[i].IsObjectOn)
        {
            if((slotNumber[i]==0) && (counter > 0))
            {
                GameSlots[i].ID = LED_IFT(DAT_MAIN, GameSlots[i].var2);
                LE_SEQNCR_StartXY(GameSlots[i].ID, GameSlots[i].Priority,
                    GameSlots[i].HotSpot.left, GameSlots[i].HotSpot.top);
            }
            else
            {
                GameSlots[i].ID = LED_IFT(DAT_MAIN, GameSlots[i].var1);
                LE_SEQNCR_StartXY(GameSlots[i].ID, GameSlots[i].Priority,
                    GameSlots[i].HotSpot.left, GameSlots[i].HotSpot.top);
            }
            
            LE_SEQNCR_StartXY(GameSlots[i].extraID, GameSlots[i].Priority+1,
                    GameFileNameRects[i].left, GameFileNameRects[i].top);
            GameSlots[i].IsObjectOn = TRUE;
        }

    }

    if(!LoadDialogBox.IsObjectOn)
    {
        //LE_SEQNCR_StartXY(LoadDialogBox.extraID, LoadDialogBox.Priority, DIALOG_X, DIALOG_Y);
        LoadDialogBox.IsObjectOn = TRUE;
    }

    if(counter == 0)
        CurrentGameFile = -1;
    else
        CurrentGameFile = 0;

    MaxGameFileNames = counter;

    LE_FONTS_GetSettings(0);

    // display the ok and cancel buttons
    for(i=0; i<2; i++)
    {
        if(!DialogButtons[i].IsObjectOn)
        {
            LE_SEQNCR_StartXY(DialogButtons[i].ID, DialogButtons[i].Priority,
                DialogButtons[i].HotSpot.left, DialogButtons[i].HotSpot.top);
            DialogButtons[i].IsObjectOn = TRUE;
        }
    }

    // display the title
    if(!LoadGameTitle.IsObjectOn)
    {
        LE_SEQNCR_StartXY(LoadGameTitle.ID, LoadGameTitle.Priority, 70, 0);
        LoadGameTitle.IsObjectOn = TRUE;
    }


}



/*****************************************************************************/
/*****************************************************************************
    * Function:     void UDOPTIONS_RemoveLoadDialogBox(void)
    *
    * Returns:      nothing
    * Parameters:   nothing
    * Purpose:      Remove the display of the new dialog box art
    *               
    ****************************************************************************
    */
void UDOPTIONS_RemoveLoadDialogBox(void)
{   
    int i;

    if(LoadDialogBox.IsObjectOn)
    {
        //LE_SEQNCR_Stop(LoadDialogBox.extraID, LoadDialogBox.Priority);
        LoadDialogBox.IsObjectOn = FALSE;
    }

    // remove the ok and cancel buttons
    for(i=0; i<2; i++)
    {
        if(DialogButtons[i].IsObjectOn)
        {
            LE_SEQNCR_Stop(DialogButtons[i].ID, DialogButtons[i].Priority);
            DialogButtons[i].IsObjectOn = FALSE;
        }
    }

    // remove title
    if(LoadGameTitle.IsObjectOn)
    {
        LE_SEQNCR_Stop(LoadGameTitle.ID, LoadGameTitle.Priority);
        LoadGameTitle.IsObjectOn = FALSE;
    }

    for(i=0; i<5; i++)
    {
        // remove game slots
        if(GameSlots[i].IsObjectOn)
        {
            LE_SEQNCR_Stop(GameSlots[i].ID, GameSlots[i].Priority);
            LE_SEQNCR_Stop(GameSlots[i].extraID, GameSlots[i].Priority+1);
            GameSlots[i].IsObjectOn = FALSE;
        }
    }
}




/*****************************************************************************/
/*****************************************************************************
    * Function:     BOOL UDOPTIONS_LoadGame(int gameIndex)
    *
    * Returns:      true if loaded a game succesfully, false if there was a problem
    * Parameters:   index for games 1,2,3,4 or 5
    * Purpose:      Load a saved monopoly game
    *               
    ****************************************************************************
    */
BOOL UDOPTIONS_LoadGame (int gameIndex)
{
  long                  FileSize;
  FILE                 *InputFile;//, *fileptr;
  RULE_CharHandle       MemoryHandle = NULL;
  BYTE                 *MemoryPntr = NULL;
  char                  PathName [_MAX_PATH];
  
 
    //char szFile[MAX_PATH];
    char szPath[MAX_PATH];
/*
    // Format the path to the texture templates for medium mesh textures
    sprintf(szFile, "\\%s", "directx.ini");

    // Try to reference the first texture
    if (!LE_DIRINI_ExpandFileName(szFile, szPath, LE_DIRINI_GlobalPathList))
      szPath[0] = '\0';
    
    // Okay, we found the first template.  Extract the path from it.
    char* szTmp = strrchr(szPath, '\\');
    szTmp[1] = '\0';
*/
    UDUTILS_GetModulePath(szPath, MAX_PATH);

    strcat(szPath, "savegame\\");

    // set the game to load
    sprintf(PathName, "%sgame%i.msv", szPath, gameIndex+1);
  
  
  /* Try opening the file. */

  InputFile = fopen (PathName, "rb");
  if (InputFile == NULL)
    goto FAILURE_EXIT;

  /* Find the size of the file. */

  if (fseek (InputFile, 0, SEEK_END) != 0)
    goto FAILURE_EXIT;

  FileSize = ftell (InputFile);
  if (FileSize < 12 /* RIFF files are at least 12 bytes long, -1 for errors */)
    goto FAILURE_EXIT;

  if (fseek (InputFile, 0, SEEK_SET) != 0)
    goto FAILURE_EXIT;

  /* Allocate a buffer to hold the entire file. */

  MemoryHandle = RULE_AllocateHandle (FileSize);
  if (MemoryHandle == NULL)
    goto FAILURE_EXIT;

  MemoryPntr = (unsigned char *) RULE_LockHandle (MemoryHandle);
  if (MemoryPntr == NULL)
    goto FAILURE_EXIT;

  /* Read in the file. */

  if (fread (MemoryPntr, FileSize, 1, InputFile) != 1)
    goto FAILURE_EXIT;

  /* Send it as a blob to the rules engine. */

  RULE_UnlockHandle (MemoryHandle);
  MemoryPntr = NULL; /* Has to be unlocked before handover. */

  MESS_SendAction (ACTION_SET_GAME_STATE,
    AnyLocalPlayer (), RULE_BANK_PLAYER,
    0, 1/* set for load game*/, 1 /* Version 1 for immunities & futures */, 0, NULL, 0, MemoryHandle);

  MemoryHandle = NULL; /* Message queue takes care of it now. */
     
  goto NORMAL_EXIT;

FAILURE_EXIT:
  //LE_ERROR_ErrorMsg ("Error while loading file.");
  UDSOUND_Warning();
  return 0;
NORMAL_EXIT:
  if (MemoryPntr != NULL)
    RULE_UnlockHandle (MemoryHandle);
  if (MemoryHandle != NULL)
    RULE_FreeHandle (MemoryHandle);
  if (InputFile != NULL)
    fclose (InputFile);
  return 1;
}







/*****************************************************************************/
/*****************************************************************************
    * Function:     void UDOPTIONS_DisplaySaveDialogBox(void)
    *
    * Returns:      nothing
    * Parameters:   nothing
    * Purpose:      Save a game using the new dialog box art
    *               
    ****************************************************************************
    */
void UDOPTIONS_DisplaySaveDialogBox(void)
{
    char        szFileInfo[5][500];
    int         counter = 0, i;
    //char        szTempDesc[500];
    FILE*       fileptr = NULL;
    char        szSlotNumber;
    int         slotNumber[5] = {-1,-1,-1,-1,-1};

    // switch the font size
    LE_FONTS_GetSettings(3);

    //char szFile[MAX_PATH];
    char szPath[MAX_PATH];
    char szPath2[MAX_PATH];
/*
    // Format the path to the texture templates for medium mesh textures
    sprintf(szFile, "\\%s", "directx.ini");

    // Try to reference the first texture
    if (!LE_DIRINI_ExpandFileName(szFile, szPath, LE_DIRINI_GlobalPathList))
      szPath[0] = '\0';
    
    // Okay, we found the first template.  Extract the path from it.
    char* szTmp = strrchr(szPath, '\\');
    szTmp[1] = '\0';
*/
    UDUTILS_GetModulePath(szPath, MAX_PATH);

    strcpy(szPath2, szPath);
    strcat(szPath2, "savegame\\");
    strcat(szPath, "savegame\\*.sgd");

    struct _finddata_t Game_files;
    long hFile;
    
    // Find first .brd file in current directory 
    if( (hFile = _findfirst( szPath, &Game_files )) != -1L )
    {
        strcpy( szFileInfo[counter], szPath2);
        strcat( szFileInfo[counter], Game_files.name);
        strncpy( &szSlotNumber, Game_files.name+4, 1 );
        slotNumber[counter] = atoi(&szSlotNumber)-1;
        counter++;
        /* Find the rest of the .c files */
        while(( _findnext( hFile, &Game_files ) == 0 ) && (counter < 5))
        {
            strcpy( szFileInfo[counter], szPath2);
            strcat( szFileInfo[counter], Game_files.name);
            strncpy( &szSlotNumber, Game_files.name+4, 1 );
            slotNumber[counter] = atoi(&szSlotNumber)-1;
            counter++;
        }

        _findclose( hFile );
    }

    // read the description of the same game contained in the *.sgd files
    for(i=0; i<counter; i++)
    {
        ZeroMemory(&(SaveGameStuff[slotNumber[i]]), sizeof(SaveGameStruct));

        fileptr = fopen(szFileInfo[i], "rb");
        if(fileptr == NULL)
            continue;        

        fread(&(SaveGameStuff[slotNumber[i]]), sizeof(SaveGameStruct), 1, fileptr );

        //if(fgets(szTempDesc, 500, fileptr))
        //    mbstowcs( SaveGameStuff[slotNumber[i]].GameFileDesc, szTempDesc, strlen(szTempDesc) );
        
        fclose(fileptr);
    }
    
    int tempHeight = LE_FONTS_GetStringHeight(A_T("TEST"));

    for(i=0; i<5; i++)
        LE_GRAFIX_ColorArea(GameSlots[i].extraID, 0, 0, (short)GameSlots[i].Width,
                GameSlots[i].Height,  LE_GRAFIX_MakeColorRef(0,255,0));  
    

    for(i=0; i<counter; i++)
    {
        /*
        if(i == 0)
        {
            //LE_FONTS_Print(SaveDialogBox.extraID, 0, tempHeight*i, LE_GRAFIX_MakeColorRef(255,0,0),
            //    A_T("_"));//GameFileDesc[i]);
            LE_FONTS_Print(GameSlots[i].extraID, 13, 8, LE_GRAFIX_MakeColorRef(255,255,255),
                A_T("_"));//GameFileDesc[i]);
        }
        else*/
        {
            //LE_FONTS_Print(SaveDialogBox.extraID, 0, tempHeight*i, LE_GRAFIX_MakeColorRef(255,255,255),
            //    GameFileDesc[i]);
            
            LE_FONTS_Print(GameSlots[slotNumber[i]].extraID, 13, 8, LE_GRAFIX_MakeColorRef(255,255,255),
                SaveGameStuff[slotNumber[i]].GameFileDesc);
        }
        //SetRect(&GameFileNameRects[i], DIALOG_X, DIALOG_Y + tempHeight*i,
        //    DIALOG_X+ SaveDialogBox.Width, DIALOG_Y + tempHeight*i+tempHeight);
/*
        
        CopyRect(&GameFileNameRects[i], &(GameSlots[i].HotSpot));
*/
    }
    
    for(i=0; i<5; i++)
    {
        SetRect(&GameFileNameRects[i], GameSlots[i].HotSpot.left + 52, GameSlots[i].HotSpot.top + 12,
            GameSlots[i].HotSpot.right+52, GameSlots[i].HotSpot.bottom +12);

        if(!GameSlots[i].IsObjectOn)
        {
            /*if((i==0) && (counter > 0))
            {
                GameSlots[i].ID = LED_IFT(DAT_MAIN, GameSlots[i].var2);
                LE_SEQNCR_StartXY(GameSlots[i].ID, GameSlots[i].Priority,
                    GameSlots[i].HotSpot.left, GameSlots[i].HotSpot.top);
            }
            else*/
            {
                GameSlots[i].ID = LED_IFT(DAT_MAIN, GameSlots[i].var1);
                LE_SEQNCR_StartXY(GameSlots[i].ID, GameSlots[i].Priority,
                    GameSlots[i].HotSpot.left, GameSlots[i].HotSpot.top);
            }
            
            LE_SEQNCR_StartXY(GameSlots[i].extraID, GameSlots[i].Priority+1,
                    GameFileNameRects[i].left, GameFileNameRects[i].top);
            GameSlots[i].IsObjectOn = TRUE;
        }

    }

    if(!SaveDialogBox.IsObjectOn)
    {
        //LE_SEQNCR_StartXY(SaveDialogBox.extraID, SaveDialogBox.Priority, DIALOG_X, DIALOG_Y);
        SaveDialogBox.IsObjectOn = TRUE;
    }


    if(counter == 0)
        CurrentGameFile = -1;
    else
        CurrentGameFile = 0;

    MaxGameFileNames = 5;//counter;

    LE_FONTS_GetSettings(0);

    // display the ok and cancel buttons
    for(i=0; i<2; i++)
    {
        if(!DialogButtons[i].IsObjectOn)
        {
            LE_SEQNCR_StartXY(DialogButtons[i].ID, DialogButtons[i].Priority,
                DialogButtons[i].HotSpot.left, DialogButtons[i].HotSpot.top);
            DialogButtons[i].IsObjectOn = TRUE;
        }
    }

    // display the title
    if(!SaveGameTitle.IsObjectOn)
    {
        LE_SEQNCR_StartXY(SaveGameTitle.ID, SaveGameTitle.Priority, 70, 0);
        SaveGameTitle.IsObjectOn = TRUE;
    }

}



/*****************************************************************************/
/*****************************************************************************
    * Function:     void UDOPTIONS_RemoveSaveDialogBox(void)
    *
    * Returns:      nothing
    * Parameters:   nothing
    * Purpose:      Remove the display of the new dialog box art
    *               
    ****************************************************************************
    */
void UDOPTIONS_RemoveSaveDialogBox(void)
{   
    int i;

    if(SaveDialogBox.IsObjectOn)
    {
        //LE_SEQNCR_Stop(SaveDialogBox.extraID, SaveDialogBox.Priority);
        SaveDialogBox.IsObjectOn = FALSE;
    }

    // remove the ok and cancel buttons
    for(i=0; i<2; i++)
    {
        if(DialogButtons[i].IsObjectOn)
        {
            LE_SEQNCR_Stop(DialogButtons[i].ID, DialogButtons[i].Priority);
            DialogButtons[i].IsObjectOn = FALSE;
        }
    }

    // remove title
    if(SaveGameTitle.IsObjectOn)
    {
        LE_SEQNCR_Stop(SaveGameTitle.ID, SaveGameTitle.Priority);
        SaveGameTitle.IsObjectOn = FALSE;
    }

    for(i=0; i<5; i++)
    {
        // remove game slots
        if(GameSlots[i].IsObjectOn)
        {
            LE_SEQNCR_Stop(GameSlots[i].ID, GameSlots[i].Priority);
            LE_SEQNCR_Stop(GameSlots[i].extraID, GameSlots[i].Priority+1);
            GameSlots[i].IsObjectOn = FALSE;
        }
    }
}






/*****************************************************************************/
/*****************************************************************************
    * Function:     void UDOPTIONS_DisplayBoardDialog(void)
    *
    * Returns:      nothing
    * Parameters:   nothing
    * Purpose:      Load a board using the new dialog box art
    *               
    ****************************************************************************
    */
void UDOPTIONS_DisplayBoardDialog(void)
{
    char        szFileInfo[MAX_BOARD_FILE_NAMES][100];
    int         counter = 0, i;
    FILE*       fileptr = NULL;

    // switch the font size
    LE_FONTS_GetSettings(3);

    //char szFile[MAX_PATH];
    char szPath[MAX_PATH];
    char szPath2[MAX_PATH];
/*
    // Format the path to the texture templates for medium mesh textures
    sprintf(szFile, "\\%s", "directx.ini");

    // Try to reference the first texture
    if (!LE_DIRINI_ExpandFileName(szFile, szPath, LE_DIRINI_GlobalPathList))
      szPath[0] = '\0';
    
    // Okay, we found the first template.  Extract the path from it.
    char* szTmp = strrchr(szPath, '\\');
    szTmp[1] = '\0';
*/
    UDUTILS_GetModulePath(szPath, MAX_PATH);

    strcpy(szPath2, szPath);
    strcat(szPath2, "custbrds\\");
    strcpy(szBoardFilePath, szPath2);
    strcat(szPath, "custbrds\\*.brd");
    

    struct _finddata_t Game_files;
    long hFile;
    
    // Find first .brd file in current directory 
    if( (hFile = _findfirst( szPath, &Game_files )) != -1L )
    {
        //strcpy( szFileInfo[counter], szPath2);
        strcpy( szFileInfo[counter], Game_files.name);
        counter++;
        /* Find the rest of the .c files */
        while(( _findnext( hFile, &Game_files ) == 0 ) && (counter < MAX_BOARD_FILE_NAMES))
        {
            //strcpy( szFileInfo[counter], szPath2);
            strcpy( szFileInfo[counter], Game_files.name);
            counter++;
        }

        _findclose( hFile );
    }

    // save the maximum amount of boards
    MaxBoardFileNames = counter;

    // read the description of the same game contained in the *.sgd files
    for(i=0; i<counter; i++)
    {
        ZeroMemory(BoardFileNames[i], 100);
        mbstowcs( BoardFileNames[i], szFileInfo[i], strlen(szFileInfo[i]));     
    }

    int tempHeight = LE_FONTS_GetStringHeight(A_T("TEST"));

    // only display 5 files at a time
    if(counter >= 5)
        counter = 5;

    // clear all text ids for game slots
    for(i=0; i<5; i++)
        LE_GRAFIX_ColorArea(GameSlots[i].extraID, 0, 0, (short)GameSlots[i].Width,
                GameSlots[i].Height,  LE_GRAFIX_MakeColorRef(0,255,0)); 

    for(i=0; i<counter; i++)
    {
        LE_FONTS_Print(GameSlots[i].extraID, 13, 8, LE_GRAFIX_MakeColorRef(255,255,255),
                BoardFileNames[i]);
        
        SetRect(&BoardFileNameRects[i], GameSlots[i].HotSpot.left + 52,
            GameSlots[i].HotSpot.top + 12, GameSlots[i].HotSpot.right+52,
            GameSlots[i].HotSpot.bottom + 12);
/*
        if(i == 0)
        {
            LE_FONTS_Print(LoadBoardDialog.extraID, 0, tempHeight*i, LE_GRAFIX_MakeColorRef(255,0,0),
                BoardFileNames[i]);
        }
        else
        {
            LE_FONTS_Print(LoadBoardDialog.extraID, 0, tempHeight*i, LE_GRAFIX_MakeColorRef(255,255,255),
                BoardFileNames[i]);
        }
        SetRect(&BoardFileNameRects[i], DIALOG_X, DIALOG_Y + tempHeight*i,
            DIALOG_X+ LE_FONTS_GetStringWidth(BoardFileNames[i]), DIALOG_Y + tempHeight*i+tempHeight);
*/
    }

    g_BoardNameIndex = counter;

    for(i=0; i<5; i++)
    {
        if(!GameSlots[i].IsObjectOn)
        {
            if((0==i) && (counter > 0))
            {
                GameSlots[i].ID = LED_IFT(DAT_MAIN, GameSlots[i].var2);
                LE_SEQNCR_StartXY(GameSlots[i].ID, GameSlots[i].Priority,
                    GameSlots[i].HotSpot.left, GameSlots[i].HotSpot.top);
            }
            else
            {
                GameSlots[i].ID = LED_IFT(DAT_MAIN, GameSlots[i].var1);
                LE_SEQNCR_StartXY(GameSlots[i].ID, GameSlots[i].Priority,
                    GameSlots[i].HotSpot.left, GameSlots[i].HotSpot.top);
            }
            
            LE_SEQNCR_StartXY(GameSlots[i].extraID, GameSlots[i].Priority+1,
                    BoardFileNameRects[i].left, BoardFileNameRects[i].top);
            GameSlots[i].IsObjectOn = TRUE;
        }

    }

    if(!LoadBoardDialog.IsObjectOn)
    {
        //LE_SEQNCR_StartXY(LoadBoardDialog.extraID, LoadBoardDialog.Priority, DIALOG_X, DIALOG_Y);
        LoadBoardDialog.IsObjectOn = TRUE;
    }

    if(counter == 0)
        CurrentBoardFile = -1;
    else
        CurrentBoardFile = 0;    

    LE_FONTS_GetSettings(0);

    // display the ok and cancel buttons
    for(i=0; i<4; i++)
    {
        if(i==2)
            continue;

        if((g_BoardNameIndex < 5) && (i==3))
            continue;

        if(!DialogButtons[i].IsObjectOn)
        {
            LE_SEQNCR_StartXY(DialogButtons[i].ID, DialogButtons[i].Priority,
                DialogButtons[i].HotSpot.left, DialogButtons[i].HotSpot.top);
            DialogButtons[i].IsObjectOn = TRUE;
        }
    }

    // display the title
    if(!LoadBoardTitle.IsObjectOn)
    {
        LE_SEQNCR_StartXY(LoadBoardTitle.ID, LoadBoardTitle.Priority, 70, 0);
        LoadBoardTitle.IsObjectOn = TRUE;
    }

}



/*****************************************************************************/
/*****************************************************************************
    * Function:     void UDOPTIONS_RemoveBoardDialog(void)
    *
    * Returns:      nothing
    * Parameters:   nothing
    * Purpose:      Remove the display of the new dialog box art
    *               
    ****************************************************************************
    */
void UDOPTIONS_RemoveBoardDialog(void)
{   
    int i;

    // remove the title
    if(LoadBoardTitle.IsObjectOn)
    {
        LE_SEQNCR_Stop(LoadBoardTitle.ID, LoadBoardTitle.Priority);
        LoadBoardTitle.IsObjectOn = FALSE;
    }

    if(LoadBoardDialog.IsObjectOn)
    {
        //LE_SEQNCR_Stop(LoadBoardDialog.extraID, LoadBoardDialog.Priority);
        LoadBoardDialog.IsObjectOn = FALSE;
    }

    // remove the ok and cancel buttons
    for(i=0; i<4; i++)
    {
        if(DialogButtons[i].IsObjectOn)
        {
            LE_SEQNCR_Stop(DialogButtons[i].ID, DialogButtons[i].Priority);
            DialogButtons[i].IsObjectOn = FALSE;
        }
    }

    for(i=0; i<5; i++)
    {
        // remove game slots
        if(GameSlots[i].IsObjectOn)
        {
            LE_SEQNCR_Stop(GameSlots[i].ID, GameSlots[i].Priority);
            LE_SEQNCR_Stop(GameSlots[i].extraID, GameSlots[i].Priority+1);
            GameSlots[i].IsObjectOn = FALSE;
        }
    }

}






/*****************************************************************************
    * Function:     void UDOPTIONS_ProcessLoadBoardDialogButtonPress(int button, short Area)
    * Returns:      nothing
    * Parameters:   index of button pressed
                    enum determining area clicked
    *
    * Purpose:      Determines the action depending on which File options button
    *                           was pressed.
    ****************************************************************************
    */
void UDOPTIONS_ProcessLoadBoardDialogButtonPress(int button, short Area)
{
    char    szTemp[100];
    ZeroMemory(szTemp, 100);

    int tempCurrentFile, tempCalculation;
    int tempButton;

    // set the index
    if((g_BoardNameIndex == MaxBoardFileNames) && ((g_BoardNameIndex%5) != 0))
    {
        tempCalculation = (g_BoardNameIndex/5);
        tempCalculation = tempCalculation*5;

        tempCurrentFile = CurrentBoardFile + tempCalculation;
        tempButton = button + tempCalculation;
    }
    else
    {
        tempCurrentFile = CurrentBoardFile + (g_BoardNameIndex-5);
        tempButton = button + (g_BoardNameIndex-5);
    }        
    
     
    if(Area == LOAD_BOARD_DIALOG_AREA)
    {
        LE_FONTS_GetSettings(3);
        if(CurrentGameFile > -1)
        {
            if(GameSlots[CurrentGameFile].IsObjectOn)
            {
                LE_SEQNCR_Stop(GameSlots[CurrentBoardFile].ID, GameSlots[CurrentBoardFile].Priority);
                GameSlots[CurrentBoardFile].ID = LED_IFT(DAT_MAIN, GameSlots[CurrentBoardFile].var1);
                LE_SEQNCR_StartXY(GameSlots[CurrentBoardFile].ID, GameSlots[CurrentBoardFile].Priority,
                        (short)GameSlots[CurrentBoardFile].HotSpot.left, (short)GameSlots[CurrentBoardFile].HotSpot.top);
            }
            LE_GRAFIX_ColorArea(GameSlots[CurrentBoardFile].extraID, 0, 0,
                    (short)BoardFileNameRects[CurrentBoardFile].right, 
                    (short)BoardFileNameRects[CurrentBoardFile].bottom,
                    LE_GRAFIX_MakeColorRef(0,255,0));
            LE_FONTS_Print(GameSlots[CurrentBoardFile].extraID, 13, 8, 
                    LE_GRAFIX_MakeColorRef(255,255,255), BoardFileNames[tempCurrentFile]);

            LE_SEQNCR_ForceRedraw(GameSlots[CurrentBoardFile].extraID, GameSlots[CurrentBoardFile].Priority+1);
        }
            // then highlight the chosen file name
            if(GameSlots[button].IsObjectOn)
            {
                LE_SEQNCR_Stop(GameSlots[button].ID, GameSlots[button].Priority);
                GameSlots[button].ID = LED_IFT(DAT_MAIN, GameSlots[button].var2);
                LE_SEQNCR_StartXY(GameSlots[button].ID, GameSlots[button].Priority,
                        (short)GameSlots[button].HotSpot.left, (short)GameSlots[button].HotSpot.top);
            }
            LE_GRAFIX_ColorArea(GameSlots[button].extraID, 0, 0, (short)BoardFileNameRects[button].right,
                    (short)BoardFileNameRects[button].bottom, LE_GRAFIX_MakeColorRef(0,255,0));
            
            LE_FONTS_Print(GameSlots[button].extraID, 13, 8,
                    LE_GRAFIX_MakeColorRef(255,255,255), BoardFileNames[tempButton]);
        
            LE_SEQNCR_ForceRedraw(GameSlots[button].extraID, GameSlots[button].Priority+1);
            /*
        // print the previous file in regular colour
        LE_GRAFIX_ColorArea(LoadBoardDialog.extraID, 0,
                (short)BoardFileNameRects[CurrentBoardFile].top-DIALOG_Y,
                (short)BoardFileNameRects[CurrentBoardFile].right - DIALOG_X, 
                (short)BoardFileNameRects[CurrentBoardFile].bottom -(short)BoardFileNameRects[CurrentBoardFile].top,
                LE_GRAFIX_MakeColorRef(0,0,0));
        LE_FONTS_Print(LoadBoardDialog.extraID, 0, (short)BoardFileNameRects[CurrentBoardFile].top-DIALOG_Y, 
                LE_GRAFIX_MakeColorRef(255,255,255), BoardFileNames[CurrentBoardFile]);
        
        // then highlight the chosen file name
        LE_GRAFIX_ColorArea(LoadBoardDialog.extraID, 0,
                (short)BoardFileNameRects[button].top-DIALOG_Y, (short)BoardFileNameRects[button].right-DIALOG_X,
                (short)BoardFileNameRects[button].bottom - (short)BoardFileNameRects[button].top,
                LE_GRAFIX_MakeColorRef(0,0,0));
        
        LE_FONTS_Print(LoadBoardDialog.extraID, 0,(short)BoardFileNameRects[button].top-DIALOG_Y,
                LE_GRAFIX_MakeColorRef(255,0,0), BoardFileNames[button]);
    
        LE_SEQNCR_ForceRedraw(LoadBoardDialog.extraID, LoadBoardDialog.Priority);
      */
        CurrentBoardFile = button;
        LE_FONTS_GetSettings(0);        
    }
    else if(Area == LOAD_BOARD_DIALOG_BUTTONS_AREA)
    {
        UDSOUND_Click();

        switch(button)
        {
        case 0:     // ok button
                    if(CurrentBoardFile > -1)
                    {
                        // check the security code first
                        if(!ReadSecurityNumber ())
                        {
                            UDSOUND_Warning();
			                break;
                        }

                        // read the first id saved in the board file
                        DWORD r1;
                        FILE*   file_ptr = NULL;
                        char    szPath[MAX_PATH];
                        wcstombs( szTemp, BoardFileNames[tempCurrentFile],
                                Astrlen(BoardFileNames[tempCurrentFile]));
                        sprintf(szPath, "%s%s", szBoardFilePath, szTemp);
                        file_ptr = fopen(szPath, "rb");
		
                        // if couldn't open file
		                if(file_ptr == NULL)
		                {
			                //MessageBox(NULL, "Could not open file newboard.brd", "Attention", MB_OK);
			                break;
		                }

		                fread(&r1, sizeof(DWORD), 1, file_ptr);

                        // if codes are not the same, user can not load the board
		                if(g_SecurityNum != r1)
		                {
			                fclose(file_ptr);
			                //MessageBox(NULL, "You may only load a board that you have created on your computer!", "Attention", MB_OK);
                            UDSOUND_Warning();
			                break;
		                }

                        // Set the current board and other display state values
                        CurrentBoard = LED_IFT(DAT_3D, HMD_board_citymed);
                        strcpy(DISPLAY_state.customBoardPath, szBoardFilePath);
                        wcstombs( szTemp, BoardFileNames[tempCurrentFile],
                                Astrlen(BoardFileNames[tempCurrentFile]));
                        strncat(DISPLAY_state.customBoardPath, szTemp, strlen(szTemp)-4);
                        strcat(DISPLAY_state.customBoardPath, "\\");
                        UDPSEL_PlayerInfo.citySelected = -1;
                        #if !USA_VERSION
                        UDPSEL_PlayerInfo.currencySelectionIndex = 0;
                        DISPLAY_state.system = iLangId - 2; // default currency for installed language 
                        #endif
                        DISPLAY_state.city = -1;

                        // Load the 2d Boards
                        UDUTILS_Load2DBoardSet();

                        // switch phase
                        UDPSEL_SwitchPhase(UDPSEL_PHASE_STANDARDORCUSTOMRULES);
                    }                    
        case 1:     // cancel button
                    UDOPTIONS_RemoveBoardDialog();
                    UDOPTIONS_SetBackdrop(last_playerselectscreen);
                    //UDBOARD_SetBackdrop(last_playerselectscreen);
                    break;

        case 2:     // previous button
                    UDOPTIONS_DisplayPrevious5Boards();
                    break;

        case 3:     // next button
                    UDOPTIONS_DisplayNext5Boards();
                    break;

        }
        
    }
}


/*****************************************************************************
    * Function:     void UDOPTIONS_DisplayLoadBoardScreen(int last_screen)
    * Returns:      nothing
    * Parameters:   index determining which options screen was last showing
    *                           
    * Purpose:      Displays everything contained in the load board screen .
    ****************************************************************************
    */
void UDOPTIONS_DisplayLoadBoardScreen(int last_screen)
{
    UDOPTIONS_DisplayBoardDialog();   
}





/*****************************************************************************
    * Function:     void UDOPTIONS_DisplayLoadGameScreen(int last_screen)
    * Returns:      nothing
    * Parameters:   index determining which options screen was last showing
    *                           
    * Purpose:      Displays everything contained in the load game screen .
                    but only used when loading from the player select screen,
                       not the option screen
    ****************************************************************************
    */
void UDOPTIONS_DisplayLoadGameScreen(int last_screen)
{
    UDOPTIONS_ProcessFileOptionButtonPress(F_LOAD, FILE_OPTION_AREA);
    last_options_result = FILE_SCREEN;
}




/*****************************************************************************
    * Function:     void UDOPTIONS_DisplayPrevious5Boards(void)
    * Returns:      nothing
    * Parameters:   nothing
    *                           
    * Purpose:      Displays the previous 5 custom boards in the directory.
    ****************************************************************************
    */
void UDOPTIONS_DisplayPrevious5Boards(void)
{
    int i, offset;

    // set the index
    if((g_BoardNameIndex == MaxBoardFileNames) && ((g_BoardNameIndex%5) != 0))
    {
        g_BoardNameIndex = (g_BoardNameIndex/5);
        g_BoardNameIndex = g_BoardNameIndex*5;
    }
    else
    {
        g_BoardNameIndex -= 5;
    }

    if(g_BoardNameIndex < 5)
        g_BoardNameIndex = 5;

    LE_FONTS_GetSettings(3);

    // clear the text and the rects
    for(i=0; i<5; i++)
    {
        LE_GRAFIX_ColorArea(GameSlots[i].extraID, 0, 0, (short)GameSlots[i].Width,
                GameSlots[i].Height,  LE_GRAFIX_MakeColorRef(0,255,0));  
        SetRect(&BoardFileNameRects[i], 0,0,0,0);
    }

    for(i=(g_BoardNameIndex-5); i<g_BoardNameIndex; i++)
    {
        offset = i-(g_BoardNameIndex-5);

        if(GameSlots[offset].IsObjectOn)
        {   
            SetRect(&BoardFileNameRects[offset], GameSlots[offset].HotSpot.left + 52,
            GameSlots[offset].HotSpot.top + 12, GameSlots[offset].HotSpot.right+52,
            GameSlots[offset].HotSpot.bottom + 12);
            
            LE_SEQNCR_Stop(GameSlots[offset].ID, GameSlots[offset].Priority);

            if(offset==0)
            {
                GameSlots[offset].ID = LED_IFT(DAT_MAIN, GameSlots[offset].var2);
            }
            else
            {
                GameSlots[offset].ID = LED_IFT(DAT_MAIN, GameSlots[offset].var1);
            }

            LE_SEQNCR_StartXY(GameSlots[offset].ID, GameSlots[offset].Priority,
                    (short)GameSlots[offset].HotSpot.left, (short)GameSlots[offset].HotSpot.top);
            
            /*LE_GRAFIX_ColorArea(GameSlots[offset].extraID, 0, 0,
                    (short)BoardFileNameRects[offset].right, 
                    (short)BoardFileNameRects[offset].bottom,
                    LE_GRAFIX_MakeColorRef(0,255,0));
                    */
            LE_FONTS_Print(GameSlots[offset].extraID, 13, 8, 
                    LE_GRAFIX_MakeColorRef(255,255,255), BoardFileNames[i]);

            LE_SEQNCR_ForceRedraw(GameSlots[offset].extraID, GameSlots[offset].Priority+1);
        }
    }            

    LE_FONTS_GetSettings(0);

    CurrentBoardFile = 0;    

    // determine if the previous button should be displayed and also display the next button
    if(g_BoardNameIndex <= 5)
    {
        if(DialogButtons[2].IsObjectOn)
        {
            LE_SEQNCR_Stop(DialogButtons[2].ID, DialogButtons[2].Priority);
            DialogButtons[2].IsObjectOn = FALSE;
        }
    }

    if(!DialogButtons[3].IsObjectOn)
    {
        LE_SEQNCR_StartXY(DialogButtons[3].ID, DialogButtons[3].Priority,
            DialogButtons[3].HotSpot.left, DialogButtons[3].HotSpot.top);
        DialogButtons[3].IsObjectOn = TRUE;
    }

}



/*****************************************************************************
    * Function:     void UDOPTIONS_DisplayNextBoards(void)
    * Returns:      nothing
    * Parameters:   nothing
    *                           
    * Purpose:      Displays the next 5 custom boards in the directory.
    ****************************************************************************
    */
void UDOPTIONS_DisplayNext5Boards(void)
{
    int i, offset;

    
    LE_FONTS_GetSettings(3);

    // clear the text and the rects
    for(i=0; i<5; i++)
    {
        LE_GRAFIX_ColorArea(GameSlots[i].extraID, 0, 0, (short)GameSlots[i].Width,
                GameSlots[i].Height,  LE_GRAFIX_MakeColorRef(0,255,0));  
        SetRect(&BoardFileNameRects[i], 0,0,0,0);
    }
    
    for(i=g_BoardNameIndex; i<g_BoardNameIndex+5; i++)
    {
        offset = i - g_BoardNameIndex;

        if(GameSlots[offset].IsObjectOn)
        {         
            
            LE_SEQNCR_Stop(GameSlots[offset].ID, GameSlots[offset].Priority);

            if(offset==0)
            {
                GameSlots[offset].ID = LED_IFT(DAT_MAIN, GameSlots[offset].var2);
            }
            else
            {
                GameSlots[offset].ID = LED_IFT(DAT_MAIN, GameSlots[offset].var1);
            }

            LE_SEQNCR_StartXY(GameSlots[offset].ID, GameSlots[offset].Priority,
                    (short)GameSlots[offset].HotSpot.left, (short)GameSlots[offset].HotSpot.top);
            
           /* LE_GRAFIX_ColorArea(GameSlots[offset].extraID, 0, 0,
                    (short)BoardFileNameRects[offset].right, 
                    (short)BoardFileNameRects[offset].bottom,
                    LE_GRAFIX_MakeColorRef(0,255,0));
                    */

            if(i < MaxBoardFileNames)
            {
                SetRect(&BoardFileNameRects[offset], GameSlots[offset].HotSpot.left + 52,
                    GameSlots[offset].HotSpot.top + 12, GameSlots[offset].HotSpot.right+52,
                    GameSlots[offset].HotSpot.bottom + 12);
                LE_FONTS_Print(GameSlots[offset].extraID, 13, 8, 
                    LE_GRAFIX_MakeColorRef(255,255,255), BoardFileNames[i]);
            }

            LE_SEQNCR_ForceRedraw(GameSlots[offset].extraID, GameSlots[offset].Priority+1);
        }
    }            

    LE_FONTS_GetSettings(0);

    if(i > MaxBoardFileNames)
        g_BoardNameIndex = MaxBoardFileNames;    
    else
        g_BoardNameIndex = i;

    CurrentBoardFile = 0;

    // determine if the next button should be displayed and also display the previous button
    if(MaxBoardFileNames == g_BoardNameIndex)
    {
        if(DialogButtons[3].IsObjectOn)
        {
            LE_SEQNCR_Stop(DialogButtons[3].ID, DialogButtons[3].Priority);
            DialogButtons[3].IsObjectOn = FALSE;
        }
    }

    if(!DialogButtons[2].IsObjectOn)
    {
        LE_SEQNCR_StartXY(DialogButtons[2].ID, DialogButtons[2].Priority,
            DialogButtons[2].HotSpot.left, DialogButtons[2].HotSpot.top);
        DialogButtons[2].IsObjectOn = TRUE;
    }

}


#if FORWIN95
   static char MAYBEFAR INIUserPrefsSection [] = "Software\\Hasbro Interactive\\Monopoly\\2.00.101";
#else
   static char MAYBEFAR INIUserPrefsSection [] = "Custom Board Editor";
   static char MAYBEFAR CorelCDHomeIniFileName [] = "CDHOME.INI";
#endif
static char MAYBEFAR INIHomeDir [] = "TEXTURES DIR";
static char MAYBEFAR INISecurityNumber [] = "Version";
static char MAYBEFAR INISectionName [] = "General Configuration";

BOOL ReadSecurityNumber (void)
{
   BOOL  Success;

   LONG  ErrorCode;
   HKEY  PreferencesKey;
   DWORD ValueSize;
   DWORD ValueType;
   DWORD temp=-1;
   
   /* Get the user's special board editor security code. */
   ErrorCode = RegOpenKeyEx (HKEY_LOCAL_MACHINE, INIUserPrefsSection,
   0 /* Options */, KEY_READ /* Security */, &PreferencesKey);
   Success = (ErrorCode == ERROR_SUCCESS);
   if (Success)
   {
      ValueSize = sizeof(DWORD);
      ValueType = REG_BINARY; /* Want a string.  Probably ignored on input. */
      ErrorCode = RegQueryValueEx (PreferencesKey, INISecurityNumber, NULL,
      &ValueType, (BYTE*)(&temp), &ValueSize);
      Success = (ErrorCode == ERROR_SUCCESS && ValueType == REG_BINARY);
      RegCloseKey (PreferencesKey);
   }
   g_SecurityNum = temp;
   return Success;
}



void UDOPTIONS_SetBackdrop(int last_screen )
{
    if(last_screen > DISPLAY_SCREEN_MainA)
        UDBOARD_SetBackdrop( DISPLAY_SCREEN_MainA );
    else
        UDBOARD_SetBackdrop( last_screen );
}
    


void UDOPTIONS_DestoryOptionsScreenObjects(void)
{
    int i;

    // remove the load and save screen objects
    if(LoadDialogBox.extraID)
	{
		LE_DATA_FreeRuntimeDataID (LoadDialogBox.extraID);
		LoadDialogBox.extraID = LED_EI;
	}

    if(SaveDialogBox.extraID)
	{
		LE_DATA_FreeRuntimeDataID (SaveDialogBox.extraID);
		SaveDialogBox.extraID = LED_EI;
	}

    if(LoadBoardDialog.extraID)
	{
		LE_DATA_FreeRuntimeDataID (LoadBoardDialog.extraID);
		LoadBoardDialog.extraID = LED_EI;
	}

    // remove the dialog buttons objects
    for(i=0; i<4; i++)
    {
        if(DialogButtons[i].ID)
	    {
		    LE_DATA_FreeRuntimeDataID (DialogButtons[i].ID);
		    DialogButtons[i].ID = LED_EI;
	    }
    }

    // remove the game slots objects
    for(i=0; i<5; i++)
    {
        if(GameSlots[i].extraID)
	    {
		    LE_DATA_FreeRuntimeDataID (GameSlots[i].extraID);
		    GameSlots[i].extraID = LED_EI;
	    }
    }


    // remove the options music choices
    for(i=0; i<MAX_TUNES; i++)
    {
        if(MusicChoices[i].ID)
	    {
		    LE_DATA_FreeRuntimeDataID (MusicChoices[i].ID);
		    MusicChoices[i].ID = LED_EI;
	    }
    }

    // remove the options option choices
    for(i=0; i<MAX_ONOFF_BUTTONS; i++)
    {
        if(OptionChoices[i].ID)
	    {
		    LE_DATA_FreeRuntimeDataID (OptionChoices[i].ID);
		    OptionChoices[i].ID = LED_EI;
	    }
    }

    // remove the options screen res choices and bit choices
    for(i=0; i<5; i++)
    {
        if(ScreenResChoices[i].ID)
	    {
		    LE_DATA_FreeRuntimeDataID (ScreenResChoices[i].ID);
		    ScreenResChoices[i].ID = LED_EI;
	    }
    }

    for(i=0; i<2; i++)
    {
        if(BitChoices[i].ID)
	    {
		    LE_DATA_FreeRuntimeDataID (BitChoices[i].ID);
		    BitChoices[i].ID = LED_EI;
	    }
    }

    // remove the credit screen object
    if(CreditsObject.ID)
	{
		LE_DATA_FreeRuntimeDataID (CreditsObject.ID);
		CreditsObject.ID = LED_EI;
	}

    // remove the help screen object
    if(HelpObject.ID)
	{
		LE_DATA_FreeRuntimeDataID (HelpObject.ID);
		HelpObject.ID = LED_EI;
	}

    for(i=0; i<3; i++)
    {
        if(HelpTextButtons[i].ID)
	    {
		    LE_DATA_FreeRuntimeDataID (HelpTextButtons[i].ID);
		    HelpTextButtons[i].ID = LED_EI;
	    }
    }
}