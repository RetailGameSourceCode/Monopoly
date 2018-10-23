#ifndef __UDIBAR_H__
#define __UDIBAR_H__


/*************************************************************
*
*   FILE:             UDIBAR.H
*
*   (C) Copyright 1999 Artech Digital Entertainments.
*                      All rights reserved.
*
*   Monopoly Game User Interface/Display Subfile Header.
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/
enum IBAR_STATES
{
  IBAR_STATE_Nothing = 0,
  IBAR_STATE_StartTurn,
  IBAR_STATE_Build,         //* = no ai mode?
  IBAR_STATE_Sell,          //*
  IBAR_STATE_Mortgage,      //*
  IBAR_STATE_UnMortgage,    //*
  IBAR_STATE_OtherPlayer,   //* this means 'human' - otherwise is other remote
  IBAR_STATE_RaiseMoney,
  IBAR_STATE_DoneTurn,
  IBAR_STATE_DeedActive,    //*
  // Added after design document
  IBAR_STATE_OtherPlayerRemote, //* 
  IBAR_STATE_BuyAuction,
  IBAR_STATE_ViewingCard,
  IBAR_STATE_JailExitPCR,
  IBAR_STATE_JailExitPXR,
  IBAR_STATE_JailExitPCX,
  IBAR_STATE_JailExitPXX,
  IBAR_STATE_Trading,
  IBAR_STATE_FreeUnmortgage,
  IBAR_STATE_TaxDecision,
  IBAR_STATE_HousingShort,
  IBAR_STATE_HotelShort,
  IBAR_STATE_PlaceHouse,
  IBAR_STATE_PlaceHotel,
  // Added sept 8 - when you sell a hotel and 4 houses aren't left.
  IBAR_STATE_HotelDecomposition,
  // Added sept 19 - game over
  IBAR_STATE_GameOver,

	// added for player status screen
	IBAR_STATE_StatusScreen,    // Looks unused, but unlikely play related.  Phils?

  IBAR_STATE_MAX // used only to test for valid numbers - no arrays/loops...
};

//enum IBAR_BUTTONS - see display.h, it needed the MAX value.

enum IBAR_DistinctButtons
{// An array of these tracks buttons flying in and out.
  IBAR_BUTTON_ANIM_Auction = 0, //iyaa
  IBAR_BUTTON_ANIM_Buy,         //iyab
  IBAR_BUTTON_ANIM_Camera,      //iyac
  IBAR_BUTTON_ANIM_Done,        //iyad
  IBAR_BUTTON_ANIM_Sell,        //iyae
  IBAR_BUTTON_ANIM_FlatTax,     //iyaf
  IBAR_BUTTON_ANIM_Percentage,  //iyag
  IBAR_BUTTON_ANIM_Build,       //iyah
 IBAR_BUTTON_ANIM_TradeAcc,    //iyai
 IBAR_BUTTON_ANIM_TradeCnt,    //iyaj
  IBAR_BUTTON_ANIM_Bankrupt,    //iyak
 IBAR_BUTTON_ANIM_TradeRej,    //iyal
  IBAR_BUTTON_ANIM_Mortgage,    //iyam
  IBAR_BUTTON_ANIM_Main,        //iyan
  IBAR_BUTTON_ANIM_Options,     //iyao
  IBAR_BUTTON_ANIM_Pay,         //iyap
 IBAR_BUTTON_ANIM_NewGame,    //iyaq    new game (left)
  IBAR_BUTTON_ANIM_RollDice,    //iyar
  IBAR_BUTTON_ANIM_Status,      //iyas
  IBAR_BUTTON_ANIM_Trade,       //iyat
  IBAR_BUTTON_ANIM_Unmort,      //iyau
 IBAR_BUTTON_ANIM_Exit,    //iyav
 IBAR_BUTTON_ANIM_PlayAgain,    //iyaw
  IBAR_BUTTON_ANIM_UseCard,     //iyax
  IBAR_BUTTON_ANIM_AucHouse,    //iyay
  IBAR_BUTTON_ANIM_AucHotel,    //iyaz

 IBAR_BUTTON_ANIM_PlaceHouse,    //iyazS
 IBAR_BUTTON_ANIM_PlaceHotel,    //iyazT

  IBAR_BUTTON_DISTINCT_MAX
};
enum IBAR_ButtonAnimModes
{// These indicate what is happening to a button - in, idle, out, pressed out plus indicators for AI or remote versions (not clickable)
  IBAR_BUTTON_ANIM_Off  = -1,
  IBAR_BUTTON_ANIM_In, // This is used in offsets - so first must be zero.
  IBAR_BUTTON_ANIM_Idle,
  IBAR_BUTTON_ANIM_Out,
  IBAR_BUTTON_ANIM_Pressed,
  IBAR_BUTTON_ANIM_MODE_MAX
};
#define IBAR_BUTTON_ANIMS_PER_SET     4

#define UDIBAR_IBarTopButton          413
#define UDIBAR_IBarTop                450

#define UDIBAR_CurrentPlayerNameTop   530
#define UDIBAR_CurrentPlayerNameLeft  332

// defines the y coordinate for the status buttons
#define	YPOS													510

enum ISTATBAR_ButtonStates
{// These indicate what state the status buttons are in
  ISTATBAR_BUTTON_Off  = -1,
  ISTATBAR_BUTTON_Idle,
  ISTATBAR_BUTTON_Press,
	ISTATBAR_BUTTON_Return,
  ISTATBAR_BUTTON_MAX
};

enum UDStats_SortIndexButtonEnum
{
  B_PLAYER = 0,
  B_DEED,
  B_BANK,
  B_SORT_1,
  B_SORT_2,
  B_SORT_3,
	B_SORT_4,
	MAX_IBAR_BUTTONS
};

/************************************************************/
/* EXPORTED DATA STRUCTURES AND TYPES AND GLOBALS           */
/************************************************************/
#if !USA_VERSION
extern LE_DATA_DataId UDIBAR_Deeds[56];
#endif

extern int IBARPropertyBarOrder[28];


typedef struct _STATUS_BUTTON_TAG_
{
	LE_DATA_DataId		IdleID;
	LE_DATA_DataId		PressID;
	LE_DATA_DataId		ReturnID;
	RECT							HotSpot;
	short							Xpos;
	short							State;
}	StatusButtonStruct;
extern	StatusButtonStruct	SortButton[4], CategoryButton[3], OptionsButton[4];

extern int					last_sort_result;
extern int                  last_category_result;
extern int					g_Button;
extern BOOL					g_bIsScreenUpdated;
extern int					last_options_result;
extern int					current_options_result;


extern RECT IBAR_PropertyLocations[SQ_MAX_SQUARE_TYPES];


extern BOOL g_bNewGameRequestedFlagForUDPSELCode;


/************************************************************/
/* EXPORTED FUNCTIONS                                       */
/************************************************************/
void DISPLAY_UDIBAR_Initialize( void );
void DISPLAY_UDIBAR_Destroy( void );
void DISPLAY_UDIBAR_TickActions( int numberOfTicks );
void DISPLAY_UDIBAR_Show( void );

void UDIBAR_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr );
void UDIBAR_ProcessMessageToPlayer( RULE_ActionArgumentsPointer NewMessage );

#define UDIBAR_ScoreBoxSmallWidth         127
#define UDIBAR_ScoreBoxLargeWidth         184
// It helps the ibar display code, especially during setup, if the x position
// of the left-most token varies with the number of players in the game.
// i -> i-th token, n -> total tokens, w -> current playerbox width
#define UDIBAR_Calc_ScoreX(i,n,w) (DISPLAY_IBAR_ScoreBorder + \
        ((((800 - DISPLAY_IBAR_ScoreBorder*2 - DISPLAY_IBAR_BankWidth) - n*w)/(n+1))*(i+1) + w*i))

void UDIBAR_setDialog( int index );
void UDIBAR_setIBarRulesState( int index, int player );
void UDIBAR_setIBarCurrentState( int index );

void UDIBAR_ActionCompleted( RULE_ActionArgumentsPointer NewMessage );

void UDIBAR_DeedStateCalcButtons( void );
int UDIBAR_testMortgagable( int deed );
#define UDIBAR_testUnMortgagable( x ) \
           ((UICurrentGameState.Squares[x].mortgaged ) && \
            (UICurrentGameState.Players[UICurrentGameState.Squares[x].owner].cash >=   \
            (RULE_SquarePredefinedInfo[x].mortgageCost +  \
            (RULE_SquarePredefinedInfo[x].mortgageCost *  \
             UICurrentGameState.GameOptions.interestRate + 50) / 100)) )

void UDIBar_ResetStatusBarButtons(void);
void UDIBar_ProcessOptionsButtonPress(int button);
void UDIBar_SwapStatusButtons(void);

#endif // __UDIBAR_H__

