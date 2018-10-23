#ifndef __UDOPTS_H__
#define __UDOPTS_H__

/*************************************************************
*
*   FILE:             UDOPTS.H
*
*   (C) Copyright 1999 Artech Digital Entertainments.
*                      All rights reserved.
*
*   Monopoly Game User Interface/Display Subfile Header.
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/
#define		UDOPTSBarTop				486
#define     MAX_BOARD_FILE_NAMES        100

/************************************************************/
/* EXPORTED DATA STRUCTURES AND TYPES                       */
/************************************************************/
enum UDOPTIONS_ButtonClickAreaEnum
{
  OPTIONS_NOWHERE = 0,
  OPTIONS_BUTTONS_AREA,
  FILE_OPTION_AREA,
  OPTION_OPTION_AREA,
  CREDITS_OPTION_AREA,
  HELP_OPTION_AREA,
  FILE_LOAD_GAME_DIALOG_AREA,
  OPTION_ONOFF_BUTTONS_AREA,
  OPTION_TUNES_AREA,
  OPTION_RESOLUTION_AREA,
  OPTION_BIT_AREA,
  HELP_TEXT_AREA,
  DIALOG_BUTTONS_AREA,
  LOAD_BOARD_DIALOG_AREA,
  LOAD_BOARD_DIALOG_BUTTONS_AREA,
    
  MAX_OPTIONS_AREAS
};

enum OptionsScreensEnum
{
	FILE_SCREEN = 0,
	OPTION_SCREEN,
	CREDITS_SCREEN,
	HELP_SCREEN,
    LOAD_BOARD_SCREEN,
    LOAD_GAME_SCREEN,

	MAX_OPTIONS_SCREENS
};

enum OptionsFileButtonsEnum
{
	F_NEW_GAME = 0,
	F_LOAD,
	F_SAVE,
	F_EXIT,
	F_CANCEL,

	MAX_FILE_BUTTONS
};

enum OptionsHelpButtonsEnum
{
	H_QUICK_HELP = 0,
	H_FULL_HELP,
	H_CANCEL,

	MAX_HELP_BUTTONS
};

enum OptionsOptionButtonsEnum
{
	O_OK = 0,
	O_CANCEL,

	MAX_OPTION_BUTTONS
};


enum MusicTunesEnum
{
	TUNE_1 = 0,
	TUNE_2,
	TUNE_3,
	TUNE_4,
	TUNE_5,

	MAX_TUNES
};

enum OptionsOptionOnOffButtonsEnum
{
	TOKEN_VOICES = 0,
	HOST_COMMENTS,
	MUSIC,
	TOKEN_ANIMS,
	CAMERA,
	LIGHTING,
	_3DBOARD,
	FILTERING,
	DITHERING,
    RESOLUTION,

	MAX_ONOFF_BUTTONS
};


enum HelpTextButtonsEnum
{
	PREVIOUS = 0,
	NEXT,
	CANCEL,

	MAX_HELP_TEXT_BUTTONS
};


enum BitChoicesEnum
{
	NO_BIT_CHOICE = 0,
	_16_BIT_CHOICE,
	_32_BIT_CHOICE,
    BOTH_BIT_CHOICE,

	MAX_BIT_CHOICES
};



// defines the structure for a button on the options screen
typedef struct OPTIONS_BUTTON_TAG
{
	BOOL IsButtonOn;					// flag to know if it is displayed or not
	int   state;						// state to know if button is off, idle, in, or out
	
	RECT HotSpot;						// coordinates to know where to click

	LE_DATA_DataId  ID;
	short priority;						// button priority

	// images that represent the button in its different states
	unsigned short inTag, idleTag, outTag;

} MenuOptionsButton;

typedef struct SAVEGAME_TAG
{
    ACHAR				GameFileDesc[100];
    int                 city; 
    int                 system;
    unsigned long       square_game_earnings[SQ_MAX_SQUARE_TYPES];
    char                CustomBoardName[100];                
} SaveGameStruct;


extern BOOL					g_bOptionsScreenOn;
extern BOOL					g_bOptionsButtonsOn;
extern int					last_playerselectphase;
extern int					last_playerselectscreen;
extern int					CurrentGameFile, PreviousGameFile;

extern BOOL                 g_UserRequestedExit, g_UserRequestedNewGame;
extern BOOL                 g_UserChoseToExit;
extern BOOL                 g_IsYesHitProcessed;
extern SaveGameStruct       SaveGameStuff[5];
extern ACHAR                BoardFileNames[100][100];
extern int                  CurrentBoardFile;


/************************************************************/
/* EXPORTED FUNCTIONS                                       */
/************************************************************/
void DISPLAY_UDOPTS_Initialize( void );
void DISPLAY_UDOPTS_Destroy( void );
void DISPLAY_UDOPTS_Show( void );

void UDOPTS_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr );
void UDOPTS_ProcessMessageToPlayer( RULE_ActionArgumentsPointer NewMessage );

short UDOPTIONS_GetButtonIndex(LE_QUEUE_MessagePointer UIMessagePntr, short Area );
void UDOPTIONS_RemoveLastOptionsScreen(int last_screen);
void UDOPTIONS_DisplayFileScreen(int last_screen);
void UDOPTIONS_DisplayOptionScreen(int last_screen);
void UDOPTIONS_DisplayCreditsScreen(int last_screen);
void UDOPTIONS_DisplayHelpScreen(int last_screen);
void UDOPTIONS_ProcessFileOptionButtonPress(int button, short Area);
void UDOPTIONS_ProcessOptionOptionButtonPress(int button, short Area);
void UDOPTIONS_ProcessCreditsOptionButtonPress(int button);
void UDOPTIONS_ProcessHelpOptionButtonPress(int button, short Area);
void UDOPTIONS_DisplayLoadBoardScreen(int last_screen);


#endif // __UDOPTS_H__
