#ifndef __UDSTATS_H__
#define __UDSTATS_H__

/*************************************************************
*
*   FILE:              UDSTATS.H
*
*   (C) Copyright 1999 Artech Digital Entertainments.
*                      All rights reserved.
*
*   Monopoly Game User Interface/Display Subfile Header.
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/
// defines for coordinates of various status screens buttons and displays
#define	OWNER_BOX_X								514
#define	OWNER_BOX_Y								23
#define	RENT_BOX_X								514
#define	RENT_BOX_Y								62
#define EARNINGS_BOX_X						514
#define EARNINGS_BOX_Y						103
#define EXPECTED_BOX_X						514
#define EXPECTED_BOX_Y						145
#define CURRENT_DEED_X						630
#define CURRENT_DEED_Y						  9

// defines for player screen
#define	BUTTON_1_X								514	//213
#define	BUTTON_1_Y								79	//305
#define	BUTTON_0_X								514	//213
#define	BUTTON_0_Y								169	//398
#define	BUTTON_ENTER_X						540	//247
#define	BUTTON_ENTER_Y						169	//398

#define CALC_BUTTON_WIDTH					24	//29
#define CALC_BUTTON_HEIGHT				25	//23
#define CALC_BUTTON_DX						26	//35
#define CALC_BUTTON_DY						30	//31

#define MISC_BUTTON_WIDTH				47	//89
#define MISC_BUTTON_HEIGHT				29	//27
#define MISC_BUTTON_DX						49	//96
#define MISC_BUTTON_DY						34	//34
#define MISC_DISPLAY_X						403 + 8	//211
#define MISC_DISPLAY_Y						7 + 10	//278
#define MISC_DISPLAY_WIDTH				190 - 8	//211
#define MISC_DISPLAY_HEIGHT			32 - 10	//278
#define MISC_BUTTON_1_X						410	//13
#define MISC_BUTTON_1_Y						47	//272

#define INST_DISPLAY_X						403 + 8	
#define INST_DISPLAY_Y						7 + 10	
#define INST_DISPLAY_WIDTH				190 - 8	
#define INST_DISPLAY_HEIGHT				32 - 10

#define RESULT_DISPLAY_X						403 + 112	
#define RESULT_DISPLAY_Y						7 + 47	
#define RESULT_DISPLAY_WIDTH				185 - 112	
#define RESULT_DISPLAY_HEIGHT			61 - 47	

#define DESCRIPTION_DISPLAY_X						603 + 8
#define DESCRIPTION_DISPLAY_Y						7 + 12
#define DESCRIPTION_DISPLAY_WIDTH				180 - 8
#define DESCRIPTION_DISPLAY_HEIGHT				197 - 12

#define CALC_X										514
#define CALC_Y										79
#define TOKENLIST_X							0
#define	TOKENLIST_Y							0


// defines which screen is currently being displayed
#define	PLAYER_SCREEN							0
#define	DEED_SCREEN								1
#define BANK_SCREEN               2

// limits the number of character per function button description string
#define MAX_DESC_STRLEN						500

//#define			MAXARRAYLENGTH				MaxLength+1
// priorities for the various deed displays thoughout the status screen
#define			STATUS_DEED_Priority	510
#define			FLOATER_Priority			STATUS_DEED_Priority + 90
#define			POPUP_Priority				STATUS_DEED_Priority + 100

// used for the display of the popup box
#define			LEFT_SIDE							0
#define			RIGHT_SIDE						1
// deed popup box coordinates
#define			POP_UP_X							200
#define			POP_UP_Y							230
// used for the justifying function
#define			LEFT_JUSTIFIED					1
#define			RIGHT_JUSTIFIED					2
// used for scrolling arrows
#define     DOWN_ARROW              0
#define     UP_ARROW                1
// used for bank deed signs
#define     SOLD                    0
#define     MORTGAGED               1

// useful sizes
#define STATS_DEED_CardW    36  // Size for mouse click processing.
#define STATS_DEED_CardH    42
#define STATS_DEED_DeltaCol ( boxWidth ) / 12  // Each property column spread.

#define STATS_DEED_DeltaX   4//(STATS_DEED_DeltaCol) / 4
#define STATS_DEED_DeltaY   20



/************************************************************/
/* EXPORTED DATA STRUCTURES AND TYPES                       */
/************************************************************/
enum UDStats_ButtonClickAreaEnum
{
  NOWHERE = 0,
  MISC_BUTTONS_AREA,
  CALC_BUTTONS_AREA,
  TOKEN_AREA,
	DEED_AREA,
	SORT_BUTTONS_AREA,
	CATEGORY_BUTTONS_AREA,
	POP_UP_AREA,
	MORTGAGE_AREA,
	BUILD_SELL_AREA,
  BANK_ARROWS_AREA,
  FUTURES_AREA,
  IMMUNITIES_AREA,
  PLAYER_ARROWS_AREA,
  
  MAX_AREAS
};

/************************************************************/
/* EXPORTED FUNCTIONS AND GLOBALS                           */
/************************************************************/
extern short					fStatusScreen;
// extern global counter to know when the last update was made
//extern DWORD					LastSScreenUpdateTime;

// globals that keep track of the cards bank_pays_50 and bank_pays_200
extern short             g_Card200Counter;
extern short             g_Card50Counter;

// structure that will hold the information required for the bank status
// account history screen
typedef struct _ACCOUNT_HISTORY_STRUCT_
{
  int     player_number;      //(to get the player name from the game state)
  int     turn_number;        
  ACHAR   turn_description[MAX_DESC_STRLEN];  // holds the description of everything
                                            // that happened during the turn.
} AccountHistoryStruct;

// saveaccountinfo is used to write the info to the file acchist.txt
extern AccountHistoryStruct SaveAccountInfo;

// basic dataid structure to slightly improve processor efficiency
// so instead of using IsSequenceFinished, use a flag instead
typedef struct _BASIC_DATA_ID_TAG_
{
	BOOL				IsObjectOn;
	BOOL				ToBeDisplayed;				// used when impossible to use LED_EI
	LE_DATA_DataId		ID;
	LE_DATA_DataId      extraID;
	UNS16				Priority;
	int					Width, Height;
	RECT                HotSpot;
	int                 var1, var2;       // extra variables if needed
} BasicDataIDStruct;

// extern for the property names and card names
//extern ACHAR             PropertyNames[SQ_MAX_SQUARE_TYPES][30];
//extern ACHAR             CardNames[(CHANCE_MAX-1)*2][50];


void DISPLAY_UDSTATS_Initialize( void );
void DISPLAY_UDSTATS_Destroy( void );
void DISPLAY_UDSTATS_Show( void );

void UDSTATS_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr );
void UDSTATS_ProcessMessageToPlayer( RULE_ActionArgumentsPointer NewMessage );

void UDStats_SortDeedsByPurchaceValue(void);
void UDStats_SortDeedsByOwner(void);
void UDStats_SortDeedsByEstCurrentValue(void);
void UDStats_SortDeedsByMostValuable(void);
void UDStats_SortPlayersByOrderOfTurn(void);
void UDStats_SortPlayersByNetWorth(void);
void UDStats_SortPlayersByHighFutureValue(void);
void UDStats_SortPlayersByCashAmount(void);
void UDStats_DisplayPlayerStatusScreen(void);
void UDStats_DisplayDeedStatusScreen(void);
void UDStats_DisplayDeeds(void);
void UDStats_RemoveDeeds(void);
void UDStats_DisplayPlayerDeeds(void);
void UDStats_RemovePlayerDeeds(void);
void UDStats_DisplayPlayerBoxes(void);
void UDStats_RemovePlayerBoxes(void);
short UDStats_GetButtonIndex(LE_QUEUE_MessagePointer UIMessagePntr, short Area );

void UDStats_DisplayBankStatusScreen(void);
void UDStats_SortBankByHouses(void);
void UDStats_SortBankByProperties(void);
void UDStats_SortBankByHistory(void);
void UDStats_SortBankByLiabilities(void);
void UDStats_RemoveSortBankResults(int result);
int UDStats_GetCharsFromString(ACHAR* SrcString, int SrcIndex, int MaxWidth, int MaxCnt, ACHAR* DestString);
int  UDStats_PrintString(LE_DATA_DataId DestObject, ACHAR *string,int x, int y, int max_width, int max_height);
int UDStats_GetStringHeight(ACHAR *string, int x, int y, int max_width, int max_height);

#endif // __UDSTATS_H__
