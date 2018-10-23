/*****************************************************************************
 *
 * FILE:        UDSTATS.cpp, subfile of Display.cpp & UserIfce.cpp
 * DESCRIPTION: Handles display & User input specific to the statistics
 *    visible only on the status screen.
 *
 * © Copyright 1999 Artech Digital Entertainments.  All rights reserved.
 *****************************************************************************/
/*
	*
	*
	*	3. May 26 1999
	*	modified by Davide
	*	completed the structure for the net worth function button.  The code to
	* get the correct value is still needed.
	*	Added a function to remove the displays caused by the function buttons.
	*
	*
	*	2.	May 25 1999
	* modified by Davide
	*	started to add code for the function buttons.
	*
	*
	*	1.	May 21 1999
	*	modified by Davide
	*	the six player boxes fit on one screen, but the boxes are smaller than if there 4 players
	*	or less. The display of the player deeds coincide with the size of the box.
	*	Also, some of the approved filenames for tabs have been added for faster implementation
	* once the art is ready.
	*
	*
	*/
#include "gameinc.h"

enum UDStats_ButtonClickStepsEnum
{
  ROLL_OVER = 0,
  FIRST_STEP,
  SECOND_STEP,
  THIRD_STEP,
  FOURTH_STEP,
  MAX_STEPS
};



/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/
// structure holds the deeds in the order selected by user
typedef struct _SORT_TAG_
{
	int								Index;
	LE_DATA_DataId		ID;
	UNS16							Priority;
	RECT							Location;
} SortStruct;
static SortStruct	orderArray[SQ_MAX_SQUARE_TYPES];
// struct to hold bank deed locations for sort by property
SortStruct			BankDeeds[SQ_MAX_SQUARE_TYPES];

// stucture representing the buttons on the calculator
typedef struct _CALC_BUTTON_TAG_
{
	int								State;
	LE_DATA_DataId		IdleID;
	LE_DATA_DataId		PressID;
	RECT							HotSpot;
} CalcButtonStruct;
static CalcButtonStruct	CalculatorButton[11], FunctionButton[8];
// priority for the calculator buttons
const	UNS16	CalcButtonPriority = 100;

// button structure for arrow buttons in bank account history
CalcButtonStruct  ArrowButton[2];
UNS16             ArrowButtonPriority;


// structure to sort the players in the player status screen
typedef struct _PLAYER_SORT_TAG_
{
	int										Index;
	LE_DATA_DataId							ColourID;
	LE_DATA_DataId							OthersID, Others2ID;
	UNS16									Priority;
	ACHAR									Name[RULE_MAX_PLAYER_NAME_LENGTH + 1];
	BOOL									IsObjectOn;
} PlayerSortStruct;

static PlayerSortStruct PlayerOrderArray[RULE_MAX_PLAYERS];
// object for the floating deed information
PlayerSortStruct	DeedInfoFloater;



typedef struct _LOCATION_TAG_
{
	short							player_column;
	short							column[11][3];
	short							numColumns;
	BOOL							isColumnCounted[11];
	LE_DATA_DataId		ID[SQ_MAX_SQUARE_TYPES];
	UNS16							Priority[SQ_MAX_SQUARE_TYPES];
	RECT							Location[SQ_MAX_SQUARE_TYPES];
	BOOL							IsObjectOn[SQ_MAX_SQUARE_TYPES];
	
} PlayerDeedPositionStruct;
static PlayerDeedPositionStruct PlayerDeeds[RULE_MAX_PLAYERS], DeedScreenDeeds;


// structure that holds the values of each step of a special function
typedef struct _FUNCTION_TAG_
{
	short							deed;
	short							player;
	short							turns;
	
} FunctionResultStruct;
static FunctionResultStruct FunctionResult;



// objects that will display the info on the function buttons
BasicDataIDStruct ResultDescBox, InstructBox, ResultBox, DescriptionBox, TokenList[RULE_MAX_PLAYERS];
// deedbackground sequence
BasicDataIDStruct	DeedBG;
// array of object to display the owner bar, or value bar
BasicDataIDStruct	DeedBars[SQ_MAX_SQUARE_TYPES];
// structure to hold the bank screen backgrounds
BasicDataIDStruct BankScreenBG = {FALSE,FALSE,LED_EI,LED_EI,50,786,223,
                                  {7,226,793,449},0,0};
// structure to hold the houses and hotels info
BasicDataIDStruct HHPlayerBar[RULE_MAX_PLAYERS];
// sturcture to hold the signs sold and mortgaged for the sort bank by properties
BasicDataIDStruct   BankDeedSign[2];
// array of futures and immunities icons
BasicDataIDStruct   Futures[RULE_MAX_PLAYERS], Immunities[RULE_MAX_PLAYERS];
// object used to display the futures/immunities box
BasicDataIDStruct	fniBox;

// box that will hold all the tokens
static LE_DATA_DataId		TokenListID = LED_EI;
static RECT							tokenLocation[RULE_MAX_PLAYERS];

// cash icon
static LE_DATA_DataId CashIconID;

// calculator background
static LE_DATA_DataId CalcBackgroundID;
UNS16 CalcBGPriority = 50;

// calculator text box
static LE_DATA_DataId CalcTextBoxID;
UNS16 CalcTBPriority = 50;

// background sequence
LE_DATA_DataId	BackgroundID;
UNS16						BGPriority = 10;


// boolean variable to determine if the status screen has just been turned on
static BOOL						bFirstTime = TRUE;
// flag that determines if the status screen is displaying the deed status or the 
// player status
short					fStatusScreen = PLAYER_SCREEN;


// width and height of the temporary boxes that represent the players
int boxWidth, boxHeight, deedBoxWidth;
int deedBoxX, deedBoxY;

// temporary array that holds all the colors available for a player token.
// The colours are placed so that the enum for the colour serves as the index into the array
static DWORD		PlayerColours[MAX_PLAYER_COLOURS] = {LE_GRAFIX_MakeColorRef (128, 0, 0),
																											LE_GRAFIX_MakeColorRef (0, 0, 128),
																											LE_GRAFIX_MakeColorRef (0, 128, 0),
																											LE_GRAFIX_MakeColorRef (255, 255, 0),
																											LE_GRAFIX_MakeColorRef (255, 0, 255),
                                                      LE_GRAFIX_MakeColorRef (255, 128, 0) };
																											//LE_GRAFIX_MakeColorRef (255, 255, 255),
																											//LE_GRAFIX_MakeColorRef (128, 255, 255),
																											//LE_GRAFIX_MakeColorRef (255, 0, 128),
																											//LE_GRAFIX_MakeColorRef (0, 0, 0)};


// step currently trying to implement for a calculator function
static short ButtonClickStep = ROLL_OVER;
// button index to determine which function we are implementing
static short FunctionIndex = -1;

// pop up window that will display the deeds for the user to pick in the function steps
LE_DATA_DataId		PopUpID;
BOOL                IsPopUpIDOn =  FALSE;
// id and flag to determine if the corner deed is still showing
BOOL                IsCornerDeedOn = FALSE;
LE_DATA_DataId      lastDeedID= LED_EI;

// array of rects to hold the location of the deeds so the user can pick one
RECT							DeedLocation[SQ_MAX_SQUARE_TYPES];

// extern global counter to know when the last update was made
//DWORD LastSScreenUpdateTime = 0;

// ids for the house and hotel display in the bank screen
LE_DATA_DataId  BankHouseID = LED_IFT(DAT_MAIN, TAB_iybshogr);
LE_DATA_DataId  BankHotelID = LED_IFT(DAT_MAIN, TAB_iybshord);

// displayaccountinfo is used to display the info in the status screen
AccountHistoryStruct DisplayAccountInfo[20];
// global used for the scrolling of the history
int history_row = 0;
// globals that keep track of the cards bank_pays_50 and bank_pays_200
short             g_Card200Counter = 0;
short             g_Card50Counter  = 0;

// flag to determine if the cards are displayed in the liabilites screen
BOOL                g_LiabCardsOn = FALSE;

// strings used in the display of the futures and immunities
ACHAR namesArray[SQ_MAX_SQUARE_TYPES][100];
ACHAR hitsArray[SQ_MAX_SQUARE_TYPES][50];
ACHAR playerArray[SQ_MAX_SQUARE_TYPES][100];
// property set used in the display of futures and immunities
RULE_PropertySet g_propertyset=0;
// global index into the array of strings for f and i to help scrolling
int g_fniIndex=0;

// global string to hold the complete description of a function. Might be better to
// read this from a txt file later for different languages
static ACHAR						FunctionDescText[8][MAX_DESC_STRLEN]; 
/*
ACHAR             PropertyNames[SQ_MAX_SQUARE_TYPES][30] = 
                  {A_T("GO"), A_T("MEDITERRANEAN AVENUE"), A_T("COMMUNITY CHEST 1"), 
                  A_T("BALTIC AVENUE"), A_T("INCOME TAX"), A_T("READING RR"), 
                  A_T("ORIENTAL AVENUE"), A_T("CHANCE 1"), A_T("VERMONT AVENUE"), 
                  A_T("CONNECTICUT AVENUE"), A_T("JUST VISITING"), A_T("ST CHARLES PLACE"), 
                  A_T("ELECTRIC COMPANY"), A_T("STATES AVENUE"), A_T("VIRGINIA AVENUE"),
                  A_T("PENNSYLVANIA RR"), A_T("ST JAMES PLACE"), A_T("COMMUNITY CHEST 2"), 
                  A_T("TENNESSEE AVENUE"), A_T("NEW YORK AVENUE"), A_T("FREE PARKING"), 
                  A_T("KENTUCKY AVENUE"), A_T("CHANCE 2"), A_T("INDIANA AVENUE"), 
                  A_T("ILLINOIS AVENUE"), A_T("BnO RR"), A_T("ATLANTIC AVENUE"), 
                  A_T("VENTNOR AVENUE"), A_T("WATER WORKS"), A_T("MARVIN GARDENS"), 
                  A_T("GO TO JAIL"), A_T("PACIFIC AVENUE"), A_T("NORTH CAROLINA AVENUE"), 
                  A_T("COMMUNITY CHEST 3"), A_T("PENNSYLVANIA AVENUE"), A_T("SHORT LINE RR"), 
                  A_T("CHANCE 3"), A_T("PARK PLACE"), A_T("LUXURY TAX"), A_T("BOARDWALK"), 
                  A_T("IN JAIL/AUCTION HOUSE"), A_T("OFF BOARD/AUCTION HOTEL")};

ACHAR             CardNames[(CHANCE_MAX-1)*2][50] = 
                  {A_T("GO DIRECTLY TO GO"), A_T("GO TO ST CHARLES PLACE"), A_T("GO TO NEAREST UTILITY"), 
                  A_T("GET 150 FROM BANK"), A_T("GET 50 FROM BANK"), A_T("PAY 50 TO EACH PLAYER"), 
                  A_T("GO TO BOARDWALK"), A_T("GO TO READING RAILROAD"), A_T("GO TO NEAREST RAILROAD PAY DOUBLE 1"), 
                  A_T("GO TO NEAREST RAILROAD PAY DOUBLE 2"), A_T("PAY 25 EACH HOUSE 100 EACH HOTEL"), A_T("GET OUT OF JAIL FREE"), 
                  A_T("PAY 15 TO BANK"), A_T("GO TO ILLINOIS AVENUE"), A_T("GO DIRECTLY TO JAIL"),
                  A_T("GO BACK THREE SPACES"), 
                  A_T("GET 100 FROM BANK 1"), A_T("GET 100 FROM BANK 2"), 
                  A_T("GET 100 FROM BANK 3"), A_T("GET 10 FROM BANK"), A_T("GET OUT OF JAIL FREE"), 
                  A_T("PAY 50 TO BANK"), A_T("GET 200 FROM BANK"), A_T("GET 20 FROM BANK"), 
                  A_T("PAY 40 EACH HOUSE 115 EACH HOTEL"), A_T("GO DIRECTLY TO JAIL"), A_T("GET 45 FROM BANK"), 
                  A_T("GET 50 FROM EACH PLAYER"), A_T("GO DIRECTLY TO GO"), A_T("PAY 150 TO BANK"), 
                  A_T("GET 25 FROM BANK"), A_T("PAY 100 TO BANK")};

  */


// prototypes

void UDStats_InitCalcButtons(void);
void UDStats_InitMiscButtons(void);
void UDStats_InitBankStatusScreen(void);

void UDStats_ProcessButtonRollOver(LE_QUEUE_MessagePointer UIMessagePntr );
void UDStats_DisplayStatusScreen(void);
void UDStats_RemoveStatusScreen(void);
void UDStats_FillPlayerStatusArray(void);

void UDStats_RemovePlayerStatusScreen(void);
void UDStats_RemoveDeedStatusScreen(void);
void UDStats_RemoveBankStatusScreen(void);

void UDStats_DisplayDeedInfo(int index, LE_DATA_DataId tempID, BOOL side);



void UDStats_PositionDeedOwned(PlayerDeedPositionStruct* pps, int order, int index);
void UDStats_RepositionDeeds(PlayerDeedPositionStruct* pps, int index, int column);
void UDStats_ResetDeedPositions(PlayerDeedPositionStruct* pps);
void UDStats_CountColumns(PlayerDeedPositionStruct* pps, int index);
void UDStats_SortDeedPositions(PlayerDeedPositionStruct* pps);


void UDStats_DisplayInstructionText(ACHAR* string );
void UDStats_DisplayResultText(ACHAR* string );
void UDStats_DisplayFunctionDescText(int index);
void UDStats_DisplayTokenList(void);
void UDStats_RemoveTokenList(void);
void UDStats_RemoveFunctionDisplays(void);
void UDStats_ProcessMiscButtonPress(LE_QUEUE_MessagePointer UIMessagePntr );
void UDStats_GetFunctionResult(short function);


void UDStats_CalculateOdds(ACHAR * resultStr);
void UDStats_CalculateNetWorth(ACHAR * resultStr);
void UDStats_CalculateFutVal2You(ACHAR * resultStr);
void UDStats_CalculateFutVal2Other(ACHAR * resultStr);
void UDStats_CalculateMaxExpense(ACHAR * resultStr);
void UDStats_CalculateMaxRevenue(ACHAR * resultStr);
void UDStats_CalculateMinExpense(ACHAR * resultStr);
void UDStats_CalculateMinRevenue(ACHAR * resultStr);
void UDStats_CalculateCurrentIncome(ACHAR * resultStr);
void UDStats_CalculatePotentialIncome(ACHAR * resultStr);

void UDStats_DisplayCalculator(void);
void UDStats_RemoveCalculator(void);
void UDStats_InitDeedInfoFloater(void);
void UDStats_DisplayCalcButtons(void);
void UDStats_RemoveCalcButtons(void);
void UDStats_DisplayFunctionButtons(void);
void UDStats_RemoveFunctionButtons(void);
void UDStats_ReadRollOverFile(void);
void UDStats_ResetCalculator(void);
void UDStats_DisplayPopupDeedWindow(void);
void UDStats_RemovePopupDeedWindow(void);
int UDStats_PrintStringJustified(ACHAR * string, int justification, int width);

void UDStats_DisplayBankDeeds(void);
void UDStats_RemoveBankDeeds(void);
void UDStats_ScrollBankHistory(int direction);
void UDStats_DisplayFuturesNImmunities(int hitType, int index);
void UDStats_RemoveFuturesNImmunities(void);
void UDStats_ScrollFuturesNImmunities(int direction);






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

void DISPLAY_UDSTATS_Initialize( void )
{ // Initialize any permanent display objects.  Avoid temporary ones when possible.
  // Set up Property bar
  register int t;

	PopUpID = LE_GRAFIX_ObjectCreate(400, 200, LE_GRAFIX_ObjectNotTransparent);

  for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
  {
    DISPLAY_state.SSProperties[t] = LED_EI;
    DISPLAY_state.SSPropertiesPriority[t] = 0;
  }

  DISPLAY_state.SSJailCards[0] = DISPLAY_state.SSJailCards[1] = LED_EI;

	// set font settings for the status screen
	// use slot 1 for player names
	LE_FONTS_GetSettings(0);
  
  LE_FONTS_SetSize(14);
	LE_FONTS_SetWeight(500);
  LE_FONTS_SetSettings(1);
  LE_FONTS_GetSettings(0);
	// use slot 2 for cash icons and for description text.
	
  LE_FONTS_SetSize(8);
	LE_FONTS_SetWeight(500);
  LE_FONTS_SetSettings(2);
  LE_FONTS_GetSettings(0);

  // use slot 3 for other tests
	LE_FONTS_SetSize(10);
	LE_FONTS_SetWeight(700);
  LE_FONTS_SetSettings(3);
  LE_FONTS_GetSettings(0);

	// initialize the locations of the calculator and miscellaneous buttons
	UDStats_InitCalcButtons();
	UDStats_InitMiscButtons();
	
	// init the deed info boxes
	UDStats_InitDeedInfoFloater();
	
	// load the cash icon
	CashIconID = LED_IFT( DAT_MAIN, TAB_snpscsh1 );

	// load the calculator background
	CalcBackgroundID = LED_IFT(DAT_MAIN, CNK_sncalbg);

	// load the calculator textbox
	CalcTextBoxID = LED_IFT(DAT_MAIN, CNK_sntextbg);

	// load background sequence
	BackgroundID = LED_IFT(DAT_LANG2, CNK_dndsbg00);

	// load deed background sequence
	DeedBG.ID = LED_IFT(DAT_MAIN, TAB_dndsdpbg);
	DeedBG.Priority = 20;
	

	UDStats_ReadRollOverFile();

	// prepare the deed identifiers
	for(t=0; t<SQ_MAX_SQUARE_TYPES; t++)
	{
		DeedBars[t].ID = LE_GRAFIX_ObjectCreate(52, 13, LE_GRAFIX_ObjectTransparent);
		DeedBars[t].HotSpot.top = 13;
	}

  // initialize the bank status screen info
  UDStats_InitBankStatusScreen();

  // Hack - make sure there is something here (not null) - caused crash (RK)
  // Wasn't this, item was being set to null as opposed to released causing crash - left this anyway.
  for(int i=0; i<UICurrentGameState.NumberOfPlayers; i++)
	  PlayerOrderArray[i].OthersID = LE_GRAFIX_ObjectCreate(2, 2, LE_GRAFIX_ObjectTransparent);

}

void DISPLAY_UDSTATS_Destroy( void )
{ // Deallocate stuff from Initialize.

	int i;

  if( PopUpID )
  {
	  LE_DATA_FreeRuntimeDataID(PopUpID);
    PopUpID = LED_EI;
  }

	// destroy the miscellaneous button description display
	if(ResultDescBox.ID)
	{
		LE_DATA_FreeRuntimeDataID( ResultDescBox.ID );
		ResultDescBox.ID = LED_EI;
	}

	// destroy the instruction display
	if(InstructBox.ID)
	{
		LE_DATA_FreeRuntimeDataID( InstructBox.ID );
		InstructBox.ID = LED_EI;
	}

	// destroy the result display
	if(ResultBox.ID)
	{
		LE_DATA_FreeRuntimeDataID( ResultBox.ID );
		ResultBox.ID = LED_EI;
	}

	// destroy the description display
	if(DescriptionBox.ID)
	{
		LE_DATA_FreeRuntimeDataID( DescriptionBox.ID );
		DescriptionBox.ID = LED_EI;
	}

	// destroy the individual player status displays
	for(i=0; i<RULE_MAX_PLAYERS; i++)
	{
		if(PlayerOrderArray[i].OthersID)
		{
			LE_DATA_FreeRuntimeDataID( PlayerOrderArray[i].OthersID );
			PlayerOrderArray[i].OthersID = LED_EI;
		}

	}
		

	if(TokenListID)
	{
		LE_DATA_FreeRuntimeDataID(TokenListID);
		TokenListID = LED_EI;
	}

	if(DeedInfoFloater.OthersID)
	{
		LE_DATA_FreeRuntimeDataID( DeedInfoFloater.OthersID );
		DeedInfoFloater.OthersID = LED_EI;
	}

	for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
	{
		if(DeedBars[i].ID)
		{
			LE_DATA_FreeRuntimeDataID( DeedBars[i].ID );
			DeedBars[i].ID = LED_EI;
		}
	}


  if(BankScreenBG.extraID)
	{
		LE_DATA_FreeRuntimeDataID( BankScreenBG.extraID );
		BankScreenBG.extraID = LED_EI;
	}
  
}

void DISPLAY_UDSTATS_Show( void )
{ // Recalculate what should be on the screen, compare to what is on the screen,
  // and take corrective action.  Everything is in the user interface and display states.
	

//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////// added by David Ethier on May 5 ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
	// verify that the user has just left the status screen 
	if((DISPLAY_state.desired2DView != DISPLAY_SCREEN_PortfolioA) && (!bFirstTime))
	{
		// in this case, the user has just changed screens, therefore everything
		// must be hidden
		UDStats_RemoveStatusScreen();
		bFirstTime = TRUE;
		
	}
	else if((DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA) && (bFirstTime))
	{
		// in this case, the user has just switched to the status screen, therefore
		// everything must be re-displayed
		UDStats_DisplayStatusScreen();
		bFirstTime = FALSE;
		//LastSScreenUpdateTime = GetTickCount ();
	}
	else if(DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA)
	{
		
		if((DISPLAY_state.IBarCurrentState == IBAR_STATE_Mortgage) ||
			(DISPLAY_state.IBarCurrentState == IBAR_STATE_UnMortgage) ||
      (DISPLAY_state.IBarCurrentState == IBAR_STATE_Build) ||
		  (DISPLAY_state.IBarCurrentState == IBAR_STATE_Sell))
		{
      static RULE_PropertySet LastPropSet = 0;
		  RULE_PropertySet tempPropSet = 0;
			int t;
		  
        
      switch( DISPLAY_state.IBarCurrentState )
      {// Construct correct property set for IBar state
    
      case IBAR_STATE_Build:
        tempPropSet = DISPLAY_PlaceBuildingLegalSquares( DISPLAY_state.IBarCurrentPlayer );
        break;
      case IBAR_STATE_Sell:
        tempPropSet = DISPLAY_SellBuildingLegalSquares( DISPLAY_state.IBarCurrentPlayer );
        break;
      case IBAR_STATE_Mortgage:
        for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
        { // Construct players standard property set
          if( DISPLAY_state.IBarCurrentPlayer == UICurrentGameState.Squares[t].owner )
            if( UDIBAR_testMortgagable( t ) )
              tempPropSet |= RULE_PropertyToBitSet( t );
        }
        break;
      case IBAR_STATE_UnMortgage:
        for( t = 0; t < SQ_MAX_SQUARE_TYPES; t++ )
        { // Construct players standard property set
          if( (DISPLAY_state.IBarCurrentPlayer == UICurrentGameState.Squares[t].owner ) &&
               UDIBAR_testUnMortgagable(t) )
          {
              tempPropSet |= RULE_PropertyToBitSet( t );
          }
        }
        if( (DISPLAY_state.IBarLastRuleState == IBAR_STATE_FreeUnmortgage) &&
          (DISPLAY_state.IBarLastRulePlayer == DISPLAY_state.IBarCurrentPlayer) )// Free Unmortgage is the real state, don't allow fee-free stuff to show up.
          tempPropSet &= ~DISPLAY_state.TradeFreeUnmortgageSet;
        break;
      }// Done constructing property sets.

      // Now we know what we want, lets make it so
      if(tempPropSet != LastPropSet)
      {
        g_bIsScreenUpdated = FALSE;
			  g_Button = last_sort_result + B_SORT_1;
        LastPropSet = tempPropSet;
      }
    }
   
  

  /*
		DWORD CurrentTime;
		DWORD DeltaTime;
		CurrentTime = GetTickCount ();
		DeltaTime = CurrentTime - LastSScreenUpdateTime;
		if (DeltaTime > 5000 /* milliseconds )
		{
			// set status screen flags to update screen
			g_bIsScreenUpdated = FALSE;
			g_Button = last_sort_result + B_SORT_1;
			LastSScreenUpdateTime = CurrentTime;
		}
		*/

	}
  
}


/*****************************************************************************
 * Standard UI subfunction - Process library message.
 */

void UDSTATS_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr )
{

	// temporary variables to hold mouse click coordinates
	int tempx, tempy;
	int iTemp, result;
	LE_DATA_DataId	tempID;
	POINT ptMousePos = { UIMessagePntr->numberA, UIMessagePntr->numberB };
	
	if(DISPLAY_state.desired2DView != DISPLAY_SCREEN_PortfolioA)
		return;

	
	if( UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN )
	{
				
		// get mouse x,y
		tempx = UIMessagePntr->numberA;
		tempy = UIMessagePntr->numberB;
		
	}

	// if we are in the status screen and the mouse button has been pressed
	if( (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN) &&
			( DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA ))
	{
		// check for the clear button
		if(UDStats_GetButtonIndex(UIMessagePntr, CALC_BUTTONS_AREA) == 10/* clear button*/)
		{
			if(CalculatorButton[10].State != ISTATBAR_BUTTON_Off)
				UDStats_ResetCalculator();
		}
		else if((DISPLAY_state.IBarCurrentState == IBAR_STATE_Mortgage) ||
						(DISPLAY_state.IBarCurrentState == IBAR_STATE_UnMortgage))
		{
			if((result = UDStats_GetButtonIndex(UIMessagePntr, MORTGAGE_AREA)) > -1)
			{
				LE_QUEUE_MessageRecord UItempMessage;
				UItempMessage.messageCode = UIMSG_MOUSE_LEFT_DOWN;
				UItempMessage.numberA = IBAR_PropertyLocations[result].left;
				UItempMessage.numberB = IBAR_PropertyLocations[result].top;
				UItempMessage.numberE = DISPLAY_SCREEN_PortfolioA;
				UDIBAR_ProcessMessage( &UItempMessage );
			}
		}
		else if((DISPLAY_state.IBarCurrentState == IBAR_STATE_Build) ||
						(DISPLAY_state.IBarCurrentState == IBAR_STATE_Sell))
		{
			if((result = UDStats_GetButtonIndex(UIMessagePntr, BUILD_SELL_AREA)) > -1)
			{
				LE_QUEUE_MessageRecord UItempMessage;
				UItempMessage.messageCode = UIMSG_MOUSE_LEFT_DOWN;
				UItempMessage.numberA = IBAR_PropertyLocations[result].left;
				UItempMessage.numberB = IBAR_PropertyLocations[result].top;
				UItempMessage.numberE = DISPLAY_SCREEN_PortfolioA;
				UDIBAR_ProcessMessage( &UItempMessage );
			}
		}
		else if(fStatusScreen == BANK_SCREEN)
		{
			if((result = UDStats_GetButtonIndex(UIMessagePntr, BANK_ARROWS_AREA)) > -1)
			{
				UDStats_ScrollBankHistory(result);
			}
		}
		else if(fStatusScreen == PLAYER_SCREEN)
		{
			if(fniBox.IsObjectOn)
			{
				if((result = UDStats_GetButtonIndex(UIMessagePntr, PLAYER_ARROWS_AREA)) > -1)
				{
					UDStats_ScrollFuturesNImmunities(result);
				}
				/*else if((result = UDStats_GetButtonIndex(UIMessagePntr, POP_UP_AREA)) > -1)
				{
					UDStats_RemoveFuturesNImmunities();
				}*/
			}
			else
			{
				if((result = UDStats_GetButtonIndex(UIMessagePntr, IMMUNITIES_AREA)) > -1)
				{
					UDStats_DisplayFuturesNImmunities(CHT_RENT_IMMUNITY, result);
				}
                else if((result = UDStats_GetButtonIndex(UIMessagePntr, FUTURES_AREA)) > -1)
				{
					UDStats_DisplayFuturesNImmunities(CHT_FUTURE_RENT, result);
				}				
			}
		}
		
		// check for a miscellaneous button press
		UDStats_ProcessMiscButtonPress(UIMessagePntr);

	}


	// if we are in the status screen and the mouse has been moved
	if( (UIMessagePntr->messageCode == UIMSG_MOUSE_MOVED) &&
			( DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA ))
	{
		static int d_last_time = -1;
		int found = -1;
        
		// if we are currently in the deed status screen
		if((fStatusScreen == DEED_SCREEN) && (!IsPopUpIDOn))
		{
			for( iTemp = 0; iTemp < SQ_MAX_SQUARE_TYPES; iTemp++ )
			{
				//if( (tempID = DISPLAY_propertyToOwnablePropertyConversion( orderArray[iTemp].Index )) == -1 )
					//continue;
											
				// if the cursor is located over a deed, display a larger version of the deed
				if(PtInRect(&(orderArray[orderArray[iTemp].Index].Location), ptMousePos))
				{
					found = iTemp;
				}
			}
			if(found > -1)
			{
				//tempID = DISPLAY_propertyToOwnablePropertyConversion( orderArray[found].Index );
				//tempID += LED_IFT( DAT_LANG2, TAB_deed01 ) ;
#if USA_VERSION
        tempID = LED_IFT( DAT_LANG2, TAB_iyf00x00 ) + DISPLAY_propertyToOwnablePropertyConversion( orderArray[found].Index )
          + DISPLAY_DEEDS_PER_SET*max(DISPLAY_state.city, 0);
#else
        tempID = UDIBAR_Deeds[DISPLAY_propertyToOwnablePropertyConversion(orderArray[found].Index)];
#endif
				
				if(d_last_time != found)
				{
					if(DeedInfoFloater.IsObjectOn)
					{
						LE_SEQNCR_Stop(DeedInfoFloater.Others2ID, DeedInfoFloater.Priority+2);
						LE_SEQNCR_Stop(DeedInfoFloater.ColourID, DeedInfoFloater.Priority);
						LE_SEQNCR_Stop(DeedInfoFloater.OthersID, DeedInfoFloater.Priority+1);
						DeedInfoFloater.IsObjectOn = FALSE;
					}
					// also display all the info related to that deed
					if(ptMousePos.x > 400/*half the screen width*/)
						UDStats_DisplayDeedInfo(orderArray[found].Index, tempID, LEFT_SIDE);
					else
						UDStats_DisplayDeedInfo(orderArray[found].Index, tempID, RIGHT_SIDE);

					d_last_time = found;
				}						
			}
			else
			{
				d_last_time = -1;
				if(DeedInfoFloater.IsObjectOn)
				{
					LE_SEQNCR_Stop(DeedInfoFloater.Others2ID, DeedInfoFloater.Priority+2);
					LE_SEQNCR_Stop(DeedInfoFloater.ColourID, DeedInfoFloater.Priority);
					LE_SEQNCR_Stop(DeedInfoFloater.OthersID, DeedInfoFloater.Priority+1);
					DeedInfoFloater.IsObjectOn = FALSE;
				}
			}
		
		}
        else if((IsPopUpIDOn) &&
            ((ptMousePos.x > POP_UP_X) && (ptMousePos.x < (POP_UP_X + 400))
            && (ptMousePos.y > POP_UP_Y) && (ptMousePos.y < (POP_UP_Y + 200))))
		{
                    
            ptMousePos.x -= POP_UP_X;
		    ptMousePos.y -= POP_UP_Y;
			
            for( iTemp = 0; iTemp < SQ_MAX_SQUARE_TYPES; iTemp++ )
			{
				//if( (tempID = DISPLAY_propertyToOwnablePropertyConversion( orderArray[iTemp].Index )) == -1 )
					//continue;
											
				// if the cursor is located over a deed, display a larger version of the deed
				if(PtInRect(&(DeedLocation[iTemp]), ptMousePos))
				{
					found = iTemp;
                    break;
				}
			}
			if(found > -1)
			{
				//tempID = DISPLAY_propertyToOwnablePropertyConversion( orderArray[found].Index );
				//tempID += LED_IFT( DAT_LANG2, TAB_deed01 ) ;
#if USA_VERSION
        tempID = LED_IFT( DAT_LANG2, TAB_iyf00x00 ) + DISPLAY_propertyToOwnablePropertyConversion(found)
          + DISPLAY_DEEDS_PER_SET*max(DISPLAY_state.city, 0);
#else
        tempID = UDIBAR_Deeds[DISPLAY_propertyToOwnablePropertyConversion(found)];
#endif
				
				if(d_last_time != found)
				{
					if(IsCornerDeedOn)
					{
						LE_SEQNCR_Stop(lastDeedID, POPUP_Priority);
						IsCornerDeedOn = FALSE;
					}
					// also display all the info related to that deed
					//LE_SEQNCR_StartCXYSR (tempID, POPUP_Priority, LED_EI, 0, FALSE,
                        //610, 11, 0.88f, 0);
                    LE_SEQNCR_StartXY (tempID, POPUP_Priority, 600, -2);
                    IsCornerDeedOn = TRUE;

					d_last_time = found;
                    lastDeedID = tempID;
				}						
			}
            else
            {
                d_last_time = -1;
                if(IsCornerDeedOn)
		        {
		            LE_SEQNCR_Stop(lastDeedID, POPUP_Priority);
			        IsCornerDeedOn = FALSE;						            
		        }
            }
        }
        else if(IsCornerDeedOn)
        {
            d_last_time = -1;
            LE_SEQNCR_Stop(lastDeedID, POPUP_Priority);
			IsCornerDeedOn = FALSE;						            
	    }


		// process the buttons move message
		if((ButtonClickStep < SECOND_STEP) && !fniBox.IsObjectOn)
			UDStats_ProcessButtonRollOver( UIMessagePntr );

	}

}


/*****************************************************************************
 * Standard UI subfunction - Process game message to player.
 */

void UDSTATS_ProcessMessageToPlayer( RULE_ActionArgumentsPointer NewMessage )
{
}





/*****************************************************************************
	* Function:		void UDStats_SortDeedsByPurchaceValue(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the deeds in order of their purchase values.
	****************************************************************************
	*/
void UDStats_SortDeedsByPurchaceValue(void)
{
	int valueArray[SQ_MAX_SQUARE_TYPES];
	int indexArray[SQ_MAX_SQUARE_TYPES];
	int i, j, maxValue, maxIndex, tempValue, tempIndex, xoffset;
	ACHAR		tempStr[20];

	// set the font
	LE_FONTS_GetSettings(2);

	// initialize the array with the property values;
	for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
	{
		valueArray[i] = RULE_SquarePredefinedInfo [i].purchaseCost;
		indexArray[i] = i;
		//DeedBars[i].ID = LED_EI;
		LE_GRAFIX_ColorArea(DeedBars[i].ID, 0, 0, 52, 13, LE_GRAFIX_MakeColorRef (0, 0, 0));
	}

	int object_width = LE_GRAFIX_ReturnObjectWidth(DeedBars[0].ID);

	// sort the temporary arrays from smallest to largest
	for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
	{
		maxValue = valueArray[i];
		maxIndex = i;

		for(j=i; j<SQ_MAX_SQUARE_TYPES; j++)
		{
			if(valueArray[j] > maxValue)
			{
				maxValue = valueArray[j];
				maxIndex = j;
			}
		}

		tempValue = valueArray[i];
		tempIndex = indexArray[i];

		valueArray[i] = valueArray[maxIndex];
		indexArray[i] = indexArray[maxIndex];

		valueArray[maxIndex] = tempValue;
		indexArray[maxIndex] = tempIndex;

		orderArray[i].Index = indexArray[i];

		//DeedBars[i].ID = LE_GRAFIX_ObjectCreate(52, 13, LE_GRAFIX_ObjectTransparent);
		LE_GRAFIX_ShowTCB(DeedBars[indexArray[i]].ID, 0, 0, LED_IFT(DAT_MAIN, TAB_dvaldisp));
		DeedBars[indexArray[i]].Priority = STATUS_DEED_Priority;
		DeedBars[indexArray[i]].ToBeDisplayed = TRUE;
		
		//Asprintf(tempStr, A_T("%*i"), 10, valueArray[i]);
		UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(valueArray[i], DISPLAY_state.system,
				tempStr, FALSE);
		// print the amount 
		xoffset = UDStats_PrintStringJustified(tempStr, RIGHT_JUSTIFIED, object_width);
		LE_FONTS_Print(DeedBars[indexArray[i]].ID, xoffset, 0, LE_GRAFIX_MakeColorRef (200, 200, 200), tempStr);

	}

	// reset the fonts
	LE_FONTS_GetSettings(0);
}




/*****************************************************************************
	* Function:		void UDStats_SortDeedsByOwner(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the deeds in order of their owner.
	****************************************************************************
	*/
void UDStats_SortDeedsByOwner(void)
{
	int ownerArray[SQ_MAX_SQUARE_TYPES];
	int indexArray[SQ_MAX_SQUARE_TYPES];
	int i, j, tempValue, tempIndex;

	// initialize the array with the property values;
	for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
	{
		ownerArray[i] = UICurrentGameState.Squares[i].owner;
		indexArray[i] = i;
		//DeedBars[i].ID = LED_EI;
		LE_GRAFIX_ColorArea(DeedBars[i].ID, 0, 0, 52, 13, LE_GRAFIX_MakeColorRef (0, 0, 0));
	}
	// sort the temporary arrays from smallest to largest
	for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
	{

		for(j=0; j<SQ_MAX_SQUARE_TYPES-1; j++)
		{
			if(ownerArray[j] > ownerArray[j+1])
			{
				tempValue = ownerArray[j];
				tempIndex = indexArray[j];

				ownerArray[j] = ownerArray[j+1];
				indexArray[j] = indexArray[j+1];

				ownerArray[j+1] = tempValue;
				indexArray[j+1] = tempIndex;
			}
		}
	}

	for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
	{
		orderArray[i].Index = indexArray[i];
		
		//DeedBars[i].ID = LE_GRAFIX_ObjectCreate(52, 13, LE_GRAFIX_ObjectTransparent);
		if(ownerArray[i] < 6)
			LE_GRAFIX_ShowTCB(DeedBars[indexArray[i]].ID, 0, 0, LED_IFT(DAT_MAIN, 
											TAB_dndsdp00 + UICurrentGameState.Players[ownerArray[i]].colour));
		DeedBars[indexArray[i]].Priority = STATUS_DEED_Priority;
		DeedBars[indexArray[i]].ToBeDisplayed = TRUE;
	}
/*
	// sort the temporary arrays from smallest to largest
	for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
	{
		minOwner = ownerArray[i];
		minIndex = i;

		for(j=i; j<SQ_MAX_SQUARE_TYPES; j++)
		{
			if(ownerArray[j] < minOwner)
			{
				minOwner = ownerArray[j];
				minIndex = j;
			}
		}

		tempValue = ownerArray[i];
		tempIndex = indexArray[i];

		ownerArray[i] = ownerArray[minIndex];
		indexArray[i] = indexArray[minIndex];

		ownerArray[minIndex] = tempValue;
		indexArray[minIndex] = tempIndex;

		orderArray[i].Index = indexArray[i];
		
		DeedBars[i].ID = LE_GRAFIX_ObjectCreate(52, 13, LE_GRAFIX_ObjectTransparent);
		LE_GRAFIX_ShowTCB(DeedBars[i].ID, 0, 0, LED_IFT(DAT_MAIN, 
											TAB_dndsdp00 + UICurrentGameState.Players[ownerArray[i]].colour));
		DeedBars[i].Priority = STATUS_DEED_Priority;
		DeedBars[i].Ypos = 0;
		
	}
*/
}




/*****************************************************************************
	* Function:		void UDStats_SortDeedsByEstCurrentValue(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the deeds in order of their estimated current values.
	****************************************************************************
	*/
void UDStats_SortDeedsByEstCurrentValue(void)
{
	int valueArray[SQ_MAX_SQUARE_TYPES];
	int indexArray[SQ_MAX_SQUARE_TYPES];
	int i, j, maxValue, maxIndex, tempValue, tempIndex, xoffset;
	RULE_PropertySet    properties_owned;
	ACHAR		tempStr[20];

	// set the font
	LE_FONTS_GetSettings(2);

	// initialize the array with the property values;
	for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
	{
		// not sure how to use the properties_owned argument
		if(UICurrentGameState.Squares[i].owner < RULE_MAX_PLAYERS)
			properties_owned = RULE_PropertySetOwnedByPlayer(&UICurrentGameState, UICurrentGameState.Squares[i].owner);
		else
			properties_owned = RULE_PropertySetOwnedByPlayer(&UICurrentGameState, RULE_BANK_PLAYER);

		valueArray[i] = AI_Rent_If_Stepped_On(&UICurrentGameState, i, properties_owned);
		indexArray[i] = i;
		//DeedBars[i].ID = LED_EI;
		LE_GRAFIX_ColorArea(DeedBars[i].ID, 0, 0, 52, 13, LE_GRAFIX_MakeColorRef (0, 0, 0));
	}

	int object_width = LE_GRAFIX_ReturnObjectWidth(DeedBars[0].ID);

	// sort the temporary arrays from smallest to largest
	for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
	{
		maxValue = valueArray[i];
		maxIndex = i;

		for(j=i; j<SQ_MAX_SQUARE_TYPES; j++)
		{
			if(valueArray[j] > maxValue)
			{
				maxValue = valueArray[j];
				maxIndex = j;
			}
		}

		tempValue = valueArray[i];
		tempIndex = indexArray[i];

		valueArray[i] = valueArray[maxIndex];
		indexArray[i] = indexArray[maxIndex];

		valueArray[maxIndex] = tempValue;
		indexArray[maxIndex] = tempIndex;

		orderArray[i].Index = indexArray[i];

		//DeedBars[i].ID = LE_GRAFIX_ObjectCreate(52, 13, LE_GRAFIX_ObjectTransparent);
		LE_GRAFIX_ShowTCB(DeedBars[indexArray[i]].ID, 0, 0, LED_IFT(DAT_MAIN, TAB_dvaldisp));
		DeedBars[indexArray[i]].Priority = STATUS_DEED_Priority;
		DeedBars[indexArray[i]].ToBeDisplayed = TRUE;

		//Asprintf(tempStr, A_T("%*i"), 10, valueArray[i]);
		UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(valueArray[i], DISPLAY_state.system,
				tempStr, FALSE);
		// print the amount 
		xoffset = UDStats_PrintStringJustified(tempStr, RIGHT_JUSTIFIED, object_width);
		LE_FONTS_Print(DeedBars[indexArray[i]].ID, xoffset, 0, LE_GRAFIX_MakeColorRef (200, 200, 200), tempStr);

	}

	// reset the fonts
	LE_FONTS_GetSettings(0);
}




/*****************************************************************************
	* Function:		void UDStats_SortDeedsByMostValuable(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the deeds in order of the most valuable ones this game.
	****************************************************************************
	*/
void UDStats_SortDeedsByMostValuable(void)
{
	int valueArray[SQ_MAX_SQUARE_TYPES];
	int indexArray[SQ_MAX_SQUARE_TYPES];
	int i, j, maxValue, maxIndex, tempValue, tempIndex, xoffset;
	ACHAR		tempStr[20];

	// set the font
	LE_FONTS_GetSettings(2);

	// initialize the array with the property values;
	for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
	{
		valueArray[i] = UICurrentGameState.Squares[i].game_earnings;
		indexArray[i] = i;
		//DeedBars[i].ID = LED_EI;
		LE_GRAFIX_ColorArea(DeedBars[i].ID, 0, 0, 52, 13, LE_GRAFIX_MakeColorRef (0, 0, 0));
	}

	int object_width = LE_GRAFIX_ReturnObjectWidth(DeedBars[0].ID);

	// sort the temporary arrays from smallest to largest
	for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
	{
		maxValue = valueArray[i];
		maxIndex = i;

		for(j=i; j<SQ_MAX_SQUARE_TYPES; j++)
		{
			if(valueArray[j] > maxValue)
			{
				maxValue = valueArray[j];
				maxIndex = j;
			}
		}

		tempValue = valueArray[i];
		tempIndex = indexArray[i];

		valueArray[i] = valueArray[maxIndex];
		indexArray[i] = indexArray[maxIndex];

		valueArray[maxIndex] = tempValue;
		indexArray[maxIndex] = tempIndex;

		orderArray[i].Index = indexArray[i];
		if(valueArray[i] == 0)
			continue;

		//DeedBars[i].ID = LE_GRAFIX_ObjectCreate(52, 13, LE_GRAFIX_ObjectTransparent);
		LE_GRAFIX_ShowTCB(DeedBars[indexArray[i]].ID, 0, 0, LED_IFT(DAT_MAIN, TAB_dvaldisp));
		DeedBars[indexArray[i]].Priority = STATUS_DEED_Priority;
		DeedBars[indexArray[i]].ToBeDisplayed = TRUE;

		//Asprintf(tempStr, A_T("%*i"), 10, valueArray[i]);
		UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(valueArray[i], DISPLAY_state.system, tempStr, FALSE);
		// print the amount 
		xoffset = UDStats_PrintStringJustified(tempStr, RIGHT_JUSTIFIED, object_width);
		LE_FONTS_Print(DeedBars[indexArray[i]].ID, xoffset, 0, LE_GRAFIX_MakeColorRef (200, 200, 200), tempStr);

	}

	// reset the fonts
	LE_FONTS_GetSettings(0);
}




/*****************************************************************************
	* Function:		void UDStats_SortPlayersByOrderOfTurn(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the players in order of their order of turn.
	****************************************************************************
	*/
void UDStats_SortPlayersByOrderOfTurn(void)
{
	int i; 
	//int temp;
	
	// if the status screen is turn on before any players are set 
	// (shouldn't happen in final release)
	if(UICurrentGameState.NumberOfPlayers == 0)
	{
		MessageBeep(-1);
		return;
	}

	// fill the sturcture array
	UDStats_FillPlayerStatusArray();

	// sort the array by order of turn
	for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
	{
		// this sorts by whose turn it is and who follows
		//temp = (UICurrentGameState.CurrentPlayer + i) % UICurrentGameState.NumberOfPlayers;
		//PlayerOrderArray[i].Index = temp;
		
		// this displays the boxes in the order of the player bar at the bottom of the screen
		PlayerOrderArray[i].Index = i;
	}
}




/*****************************************************************************
	* Function:		void UDStats_SortPlayersByNetWorth(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the players in order of their net worth(greatest to lowest).
	****************************************************************************
	*/
void UDStats_SortPlayersByNetWorth(void)
{
	int i, j, maximum;
	long tempArray[RULE_MAX_PLAYERS], tempworth;
	int tempIndex[RULE_MAX_PLAYERS], temp;

	// if the status screen is turn on before any players are set 
	// (shouldn't happen in final release)
	if(UICurrentGameState.NumberOfPlayers == 0)
	{
		MessageBeep(-1);
		return;
	}

	// fill the sturcture array
	UDStats_FillPlayerStatusArray();

	// fill the temp array to sort by total worth
	for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
	{
		tempArray[i] = AI_Get_Total_Worth(&UICurrentGameState, i);
		tempIndex[i] = i;
	}

	// sort the array from player who is worth the least to the one worth the most
	for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
	{
		maximum = i;

		for(j=(i+1); j<UICurrentGameState.NumberOfPlayers; j++)
		{
			
			if(tempArray[j] > tempArray[maximum])
			{
				maximum = j;
			}
		}
		
		if(maximum != i)
		{
			tempworth = tempArray[i];
			temp = tempIndex[i];
			tempArray[i] = tempArray[maximum];
			tempIndex[i] = tempIndex[maximum];
			tempArray[maximum] = tempworth;
			tempIndex[maximum] = temp;
		}

		PlayerOrderArray[i].Index = tempIndex[i];

	}
}






/*****************************************************************************
	* Function:		void UDStats_SortPlayersByHighFutureValue(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the players in order of their highest future value.
								Note: Right now (may 13) this function is a shot in the dark.
								Will need to be revised for proper calculations
	****************************************************************************
	*/
void UDStats_SortPlayersByHighFutureValue(void)
{
	
	int i, j, max;
	long tempArray[RULE_MAX_PLAYERS], tempvalue;
	int tempIndex[RULE_MAX_PLAYERS], temp;

	// if the status screen is turn on before any players are set 
	// (shouldn't happen in final release)
	if(UICurrentGameState.NumberOfPlayers == 0)
	{
		MessageBeep(-1);
		return;
	}

	// fill the sturcture array
	UDStats_FillPlayerStatusArray();

	// fill the temp array to sort by highest future value
	for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
	{
		tempArray[i] = AI_Potential_Income(&UICurrentGameState, i);
	
		tempIndex[i] = i;
	}

	// sort the array from player who is worth the least to the one worth the most
	for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
	{
		max = i;

		for(j=(i+1); j<UICurrentGameState.NumberOfPlayers; j++)
		{
			
			if(tempArray[j] > tempArray[max])
			{
				max = j;
			}
		}
		
		if(max != i)
		{
			tempvalue = tempArray[i];
			temp = tempIndex[i];
			tempArray[i] = tempArray[max];
			tempIndex[i] = tempIndex[max];
			tempArray[max] = tempvalue;
			tempIndex[max] = temp;
		}

		PlayerOrderArray[i].Index = tempIndex[i];

	}
}







/*****************************************************************************
	* Function:		void UDStats_SortPlayersByCashAmount(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the players in order of their total amount of cash.
	****************************************************************************
	*/
void UDStats_SortPlayersByCashAmount(void)
{
	int i, j, maximum;
	long tempArray[RULE_MAX_PLAYERS], tempcash;
	int tempIndex[RULE_MAX_PLAYERS], temp;

	// if the status screen is turn on before any players are set 
	// (shouldn't happen in final release)
	if(UICurrentGameState.NumberOfPlayers == 0)
	{
		MessageBeep(-1);
		return;
	}

	// fill the sturcture array
	UDStats_FillPlayerStatusArray();

	// fill the temp array to sort by cash amount
	for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
	{
		tempArray[i] = UICurrentGameState.Players[i].cash;
		tempIndex[i] = i;
	}

	// sort the array by cash amount
	for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
	{
		maximum = i;

		for(j=(i+1); j<UICurrentGameState.NumberOfPlayers; j++)
		{
			
			if(tempArray[j] > tempArray[maximum])
			{
				maximum = j;
			}
		}
		
		if(maximum != i)
		{
			tempcash = tempArray[i];
			temp = tempIndex[i];
			tempArray[i] = tempArray[maximum];
			tempIndex[i] = tempIndex[maximum];
			tempArray[maximum] = tempcash;
			tempIndex[maximum] = temp;
		}

		PlayerOrderArray[i].Index = tempIndex[i];

	}
}





/*****************************************************************************
	* Function:		void UDStats_InitCalcButtons(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Sets the coordinates for the calculator buttons.
	****************************************************************************
	*/
void UDStats_InitCalcButtons(void)
{
	int i;
	
	for(i=0; i<10; i++)
	{
		CalculatorButton[i].HotSpot.left = BUTTON_1_X + (CALC_BUTTON_DX*(i%3));
		CalculatorButton[i].HotSpot.top = BUTTON_1_Y + (CALC_BUTTON_DY*(i/3));
		CalculatorButton[i].HotSpot.right = CalculatorButton[i].HotSpot.left + CALC_BUTTON_WIDTH;
		CalculatorButton[i].HotSpot.bottom = CalculatorButton[i].HotSpot.top + CALC_BUTTON_HEIGHT;
		CalculatorButton[i].IdleID = LED_IFT( DAT_MAIN, CNK_snnumi00+i);
		CalculatorButton[i].PressID = LED_IFT( DAT_MAIN, CNK_snnump00+i);
		CalculatorButton[i].State = ISTATBAR_BUTTON_Off;
	}

	SetRect(&(CalculatorButton[10].HotSpot), BUTTON_ENTER_X, BUTTON_ENTER_Y, 
					(BUTTON_ENTER_X+CALC_BUTTON_WIDTH+CALC_BUTTON_DX), BUTTON_ENTER_Y+CALC_BUTTON_HEIGHT); 
	CalculatorButton[10].IdleID = LED_IFT( DAT_LANG2, CNK_snnumi10 );
	CalculatorButton[10].PressID = LED_IFT( DAT_LANG2, CNK_snnump10 );
	CalculatorButton[10].State = ISTATBAR_BUTTON_Off;

	// set the calculator that changes to a list of tokens
	TokenListID = LE_GRAFIX_ObjectCreate(76, 115, LE_GRAFIX_ObjectTransparent);

}




/*****************************************************************************
	* Function:		void UDStats_InitMiscButtons(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Sets the coordinates for the miscellaneous calculating buttons.
	****************************************************************************
	*/
void UDStats_InitMiscButtons(void)
{

	for(int i=0; i<8; i++)
	{
		FunctionButton[i].HotSpot.left = MISC_BUTTON_1_X + (MISC_BUTTON_DX*(i%2));
		FunctionButton[i].HotSpot.top = MISC_BUTTON_1_Y + (MISC_BUTTON_DY*(i/2));
		FunctionButton[i].HotSpot.right = FunctionButton[i].HotSpot.left + MISC_BUTTON_WIDTH;
		FunctionButton[i].HotSpot.bottom = FunctionButton[i].HotSpot.top + MISC_BUTTON_HEIGHT;
		FunctionButton[i].IdleID = LED_IFT( DAT_MAIN, CNK_snfuni00 + i );
		FunctionButton[i].PressID = LED_IFT( DAT_MAIN, CNK_snfunp00 + i );
		FunctionButton[i].State = ISTATBAR_BUTTON_Off;
	}

	// also initialize the display for the text over
	if(ResultDescBox.ID)
	{
		LE_DATA_FreeRuntimeDataID( ResultDescBox.ID );
		ResultDescBox.ID = LED_EI;
	}

	ResultDescBox.ID = LE_GRAFIX_ObjectCreate(MISC_DISPLAY_WIDTH, MISC_DISPLAY_HEIGHT,
									LE_GRAFIX_ObjectNotTransparent);
	ResultDescBox.Priority = 100;
	ResultDescBox.IsObjectOn = FALSE;

	// also initialize the display for the instructions
	if(InstructBox.ID)
	{
		LE_DATA_FreeRuntimeDataID( InstructBox.ID );
		InstructBox.ID = LED_EI;
	}

	InstructBox.ID = LE_GRAFIX_ObjectCreate(INST_DISPLAY_WIDTH, INST_DISPLAY_HEIGHT,
									LE_GRAFIX_ObjectNotTransparent);
	InstructBox.Priority = 101;
	InstructBox.IsObjectOn = FALSE;

	// also initialize the display for the results of the functions
	if(ResultBox.ID)
	{
		LE_DATA_FreeRuntimeDataID( ResultBox.ID );
		ResultBox.ID = LED_EI;
	}

	ResultBox.ID = LE_GRAFIX_ObjectCreate(RESULT_DISPLAY_WIDTH, RESULT_DISPLAY_HEIGHT,
									LE_GRAFIX_ObjectNotTransparent);
	ResultBox.Priority = 100;
	ResultBox.IsObjectOn = FALSE;

	// also initialize the display for the description of the functions
	if(DescriptionBox.ID)
	{
		LE_DATA_FreeRuntimeDataID( DescriptionBox.ID );
		DescriptionBox.ID = LED_EI;
	}

	DescriptionBox.ID = LE_GRAFIX_ObjectCreate(DESCRIPTION_DISPLAY_WIDTH, DESCRIPTION_DISPLAY_HEIGHT,
									LE_GRAFIX_ObjectNotTransparent);
	DescriptionBox.Priority = 100;
	DescriptionBox.IsObjectOn = FALSE;

	
}







/*****************************************************************************
	* Function:		void UDStats_ProcessButtonRollOver(LE_QUEUE_MessagePointer UIMessagePntr )
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Determines the action depending on the location of the mouse for the
	*							roll over.
	****************************************************************************
	*/
void UDStats_ProcessButtonRollOver(LE_QUEUE_MessagePointer UIMessagePntr )
{
	static b_last_time = -1;
	// get the index of the button we are currently over;
	short iTemp = UDStats_GetButtonIndex(UIMessagePntr, MISC_BUTTONS_AREA);
	if(iTemp > -1)
	{
		if(iTemp != b_last_time)
		{
			ButtonClickStep = FIRST_STEP;
			// display the appropriate text
			UDStats_DisplayFunctionDescText(iTemp);
			b_last_time = iTemp;
		}
	}
	else
	{
		UDStats_RemoveFunctionDisplays();
		b_last_time = -1;
	}
	
}


/*****************************************************************************
	* Function:		void UDStats_DisplayStatusScreen(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays everything contained in the status screen .
	****************************************************************************
	*/
void UDStats_DisplayStatusScreen(void)
{
	
	// display the background
	LE_SEQNCR_Start(BackgroundID, BGPriority);

	// display the calculator
	UDStats_DisplayCalculator();

	// display the proper screen depending on last screen shown
	if(fStatusScreen == PLAYER_SCREEN)
		UDStats_DisplayPlayerStatusScreen();
	else if(fStatusScreen == DEED_SCREEN)
		UDStats_DisplayDeedStatusScreen();
  else if(fStatusScreen == BANK_SCREEN)
    UDStats_DisplayBankStatusScreen();

}




/*****************************************************************************
	* Function:		void UDStats_RemoveStatusScreen(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Removes everything contained in the status screen .
	****************************************************************************
	*/
void UDStats_RemoveStatusScreen(void)
{
	// remove the displays, according the current status screen
	//if(fStatusScreen == PLAYER_SCREEN)
		UDStats_RemovePlayerStatusScreen();
	//else if(fStatusScreen == DEED_SCREEN)
		UDStats_RemoveDeedStatusScreen();
  //else if(fStatusScreen == BANK_SCREEN)
  	UDStats_RemoveBankStatusScreen();
  
	// stop the calculator display
	UDStats_RemoveCalculator();

	// remove the background
	LE_SEQNCR_Stop(BackgroundID, BGPriority);

}




/*****************************************************************************
	* Function:		void UDStats_FillPlayerStatusArray(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Fills the Player Sort Structure Array before doing any sorting.
	****************************************************************************
	*/
void UDStats_FillPlayerStatusArray(void)
{
	int i;
	ACHAR		cashStr[10];

	// set the font
	LE_FONTS_GetSettings(2);
	
	for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
	{
		// set the font to print the name
		LE_FONTS_GetSettings(1);
		// set the index to the order the players joined in
		PlayerOrderArray[i].Index = i;
		// set the appropriate names
		Astrcpy(PlayerOrderArray[i].Name, UICurrentGameState.Players[i].name); 
		// set the priority
		PlayerOrderArray[i].Priority = 500;
		PlayerOrderArray[i].IsObjectOn = FALSE;
		
		
		// load different tabs depending on more or less than 4 players
    // Hack - RK, dynamic use of this is bad - make em max size needed & rewrite them.  Hacked to fix:
    if( PlayerOrderArray[i].OthersID != LED_EI )
    {
      LE_DATA_FreeRuntimeDataID( PlayerOrderArray[i].OthersID );
      PlayerOrderArray[i].OthersID = LED_EI;
    }

		if(UICurrentGameState.NumberOfPlayers > 4)
		{
			boxWidth = 130;
			boxHeight = 226;
			deedBoxWidth = 120;
			deedBoxX = 5;
			deedBoxY = 300;
			PlayerOrderArray[i].OthersID = LE_GRAFIX_ObjectCreate(boxWidth, boxHeight, LE_GRAFIX_ObjectTransparent);
			PlayerOrderArray[i].ColourID = LED_IFT( DAT_MAIN, (TAB_snpsbgs0 + UICurrentGameState.Players[i].colour) );
		}
		else
		{
			boxWidth = 198;
			boxHeight = 222;
			deedBoxWidth = 130;
			deedBoxX = 61;
			deedBoxY = 275;
			PlayerOrderArray[i].OthersID = LE_GRAFIX_ObjectCreate(boxWidth, boxHeight, LE_GRAFIX_ObjectTransparent);
			PlayerOrderArray[i].ColourID = LED_IFT( DAT_MAIN, (TAB_snpsbg10 + UICurrentGameState.Players[i].colour) );
		}
		
		// clear the background
		LE_GRAFIX_ColorArea(PlayerOrderArray[i].OthersID, 0, 0, boxWidth, boxHeight, LE_GRAFIX_MakeColorRef (0, 255, 0));
								
		// print the player's name in the box
        int currentfontsize = 14;
        while(LE_FONTS_GetStringWidth(PlayerOrderArray[i].Name) > boxWidth)
        {
            currentfontsize--;
            LE_FONTS_SetSize(currentfontsize);
	        LE_FONTS_SetWeight(700);
        }
        
        int tempStartX = boxWidth/2 - LE_FONTS_GetStringWidth(PlayerOrderArray[i].Name)/2;

		LE_FONTS_Print(PlayerOrderArray[i].OthersID, tempStartX/*10*/, 5, LE_GRAFIX_MakeColorRef (255, 255, 255), 
								PlayerOrderArray[i].Name);

		// print the player's cash amount into the cash icon
		LE_FONTS_GetSettings(2);

		//Asprintf(cashStr, A_T("%*i"), 5, UICurrentGameState.Players[i].cash);
		UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(UICurrentGameState.Players[i].cash,
				DISPLAY_state.system, cashStr, FALSE);
		// now display the cash icon in the player box tab
		LE_GRAFIX_ShowTCB_Alpha(PlayerOrderArray[i].OthersID, 5, 30, CashIconID);
		//LE_GRAFIX_ShowTCB(PlayerOrderArray[i].ID, 10, 10, CashIconID);
		// print the cash amount over the cash icon into the player box
		LE_FONTS_Print(PlayerOrderArray[i].OthersID, 15, 62, LE_GRAFIX_MakeColorRef (200, 200, 200), cashStr);
    
	}

	// reset the fonts
	LE_FONTS_GetSettings(0);
}




/*****************************************************************************
	* Function:		void UDStats_DisplayPlayerStatusScreen(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays everything contained in the player status screen .
	****************************************************************************
	*/
void UDStats_DisplayPlayerStatusScreen(void)
{
	// remove everything concerning the previous status screen
  if(fStatusScreen == DEED_SCREEN)
    UDStats_RemoveDeedStatusScreen();
  else if(fStatusScreen == BANK_SCREEN)
    UDStats_RemoveBankStatusScreen();

  // set screen flag
	fStatusScreen = PLAYER_SCREEN;
}




/*****************************************************************************
	* Function:		void UDStats_RemovePlayerStatusScreen(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Removes everything contained in the player status screen .
	****************************************************************************
	*/
void UDStats_RemovePlayerStatusScreen(void)
{
	
	// remove the player names, whatever their order
	UDStats_RemovePlayerBoxes();

	// remove the deeds owned by each player
	UDStats_RemovePlayerDeeds();
	

  // remove the popup window and arrow buttons
  UDStats_RemoveFuturesNImmunities();
}



/*****************************************************************************
	* Function:		void UDStats_DisplayDeedStatusScreen(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays everything contained in the Deed status screen .
	****************************************************************************
	*/
void UDStats_DisplayDeedStatusScreen(void)
{
	// remove everything concerning the previous status screen
  if(fStatusScreen == PLAYER_SCREEN)
	  UDStats_RemovePlayerStatusScreen();
  else if(fStatusScreen == BANK_SCREEN)
	  UDStats_RemoveBankStatusScreen();

	// display the deed background
	LE_SEQNCR_StartXY(DeedBG.ID, DeedBG.Priority, 5, 229);
	DeedBG.IsObjectOn = TRUE;

	fStatusScreen = DEED_SCREEN;
}




/*****************************************************************************
	* Function:		void UDStats_RemoveDeedStatusScreen(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Removes everything contained in the deed status screen .
	****************************************************************************
	*/
void UDStats_RemoveDeedStatusScreen(void)
{
	// remove the deed info displays
	if(DeedInfoFloater.IsObjectOn)
	{
		LE_SEQNCR_Stop(DeedInfoFloater.Others2ID, DeedInfoFloater.Priority+2);
		LE_SEQNCR_Stop(DeedInfoFloater.ColourID, DeedInfoFloater.Priority);
		LE_SEQNCR_Stop(DeedInfoFloater.OthersID, DeedInfoFloater.Priority+1);
		DeedInfoFloater.IsObjectOn = FALSE;
	}
	
	// remove the display of the deeds
	UDStats_RemoveDeeds();

	// display the deed background
	if(DeedBG.IsObjectOn)
	{
		LE_SEQNCR_Stop(DeedBG.ID, DeedBG.Priority);
		DeedBG.IsObjectOn = FALSE;
	}

}




/*****************************************************************************
	* Function:		void UDStats_DisplayDeeds(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Display all deeds in order or previous sort.
	****************************************************************************
	*/
void UDStats_DisplayDeeds(void)
{

  register int i;
  int x, y; // Hopefully temporary x, y coords to start sequences.
  LE_DATA_DataId tempID;
  UNS16 tempPriority;
	int width, height, cnt=-1;
	RULE_PropertySet build_set = DISPLAY_PlaceBuildingLegalSquares( DISPLAY_state.IBarCurrentPlayer );
	RULE_PropertySet sell_set = DISPLAY_SellBuildingLegalSquares( DISPLAY_state.IBarCurrentPlayer );

	
	// Update the Property Display
  for( i = 0; i < SQ_MAX_SQUARE_TYPES; i++ )
  {// Calculate the current propery ID
    if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA )
    {// Property is visible
			
      if( (tempID = DISPLAY_propertyToOwnablePropertyConversion( orderArray[i].Index )) == -1 )
        tempID = LED_EI;
					
      else
      {
	        if((DISPLAY_state.IBarCurrentState == IBAR_STATE_Mortgage) ||
				 (DISPLAY_state.IBarCurrentState == IBAR_STATE_UnMortgage) ||
				 (DISPLAY_state.IBarCurrentState == IBAR_STATE_Build) ||
				 (DISPLAY_state.IBarCurrentState == IBAR_STATE_Sell))
			{
				if((SlotIsALocalHumanPlayer [DISPLAY_state.IBarCurrentPlayer]) &&
				(DISPLAY_state.IBarCurrentPlayer == UICurrentGameState.Squares[orderArray[i].Index].owner))
				{
                    switch(DISPLAY_state.IBarCurrentState)
                    {
                    case IBAR_STATE_Mortgage:   if( UDIBAR_testMortgagable( orderArray[i].Index ) )
                                                {
#if USA_VERSION
                                                    tempID += LED_IFT(DAT_PAT, TAB_trprfp0000);
#else
                                                    if(iLangId == NOTW_LANG_FRENCH)
                                                        tempID += LED_IFT(DAT_PAT, TAB_xrprfp0000);
                                                    else
                                                        tempID += LED_IFT(DAT_PAT, TAB_zrprfp0000);
#endif
                                                }
                                                else
                                                    continue;
                                                break;

                    case IBAR_STATE_UnMortgage: if(UICurrentGameState.Squares[orderArray[i].Index].mortgaged)
                                                {
#if USA_VERSION
                                                    tempID += LED_IFT(DAT_PAT, TAB_trprfm0000);
#else
                                                    if(iLangId == NOTW_LANG_FRENCH)
                                                        tempID += LED_IFT(DAT_PAT, TAB_xrprfm0000);
                                                    else
                                                        tempID += LED_IFT(DAT_PAT, TAB_zrprfm0000);
#endif
                                                }
                                                else
                                                    continue;
                                                break;

                    case IBAR_STATE_Build:      if(RULE_PropertyToBitSet(orderArray[i].Index) & build_set)
                                                {
#if USA_VERSION
                                                    tempID += LED_IFT(DAT_PAT, TAB_trprfp0000);
#else
                                                    if(iLangId == NOTW_LANG_FRENCH)
                                                        tempID += LED_IFT(DAT_PAT, TAB_xrprfp0000);
                                                    else
                                                        tempID += LED_IFT(DAT_PAT, TAB_zrprfp0000);
#endif
                                                }
                                                else
                                                    continue;
                                                break;

                    case IBAR_STATE_Sell:       if(RULE_PropertyToBitSet(orderArray[i].Index) & sell_set)
                                                {
#if USA_VERSION
                                                    tempID += LED_IFT(DAT_PAT, TAB_trprfp0000);
#else
                                                    if(iLangId == NOTW_LANG_FRENCH)
                                                        tempID += LED_IFT(DAT_PAT, TAB_xrprfp0000);
                                                    else
                                                        tempID += LED_IFT(DAT_PAT, TAB_zrprfp0000);
#endif
                                                }
                                                else
                                                    continue;
                                                break;

                    default:                    tempID = LED_EI;    break;
                    }
				}
				else
				{
					tempID = LED_EI;
					continue;
				}
			}
			else
			{
#if USA_VERSION
				if(UICurrentGameState.Squares[orderArray[i].Index].mortgaged)
					tempID += LED_IFT(DAT_PAT, TAB_trprfm0000);
				else
					tempID += LED_IFT( DAT_PAT, TAB_trprfp0000 );
#else
                if(UICurrentGameState.Squares[orderArray[i].Index].mortgaged)
                {
                    if(iLangId == NOTW_LANG_FRENCH)
					    tempID += LED_IFT(DAT_PAT, TAB_xrprfm0000);
                    else
                        tempID += LED_IFT(DAT_PAT, TAB_zrprfm0000);
                }
				else
                {
                    if(iLangId == NOTW_LANG_FRENCH)
					    tempID += LED_IFT( DAT_PAT, TAB_xrprfp0000 );
                    else
                        tempID += LED_IFT( DAT_PAT, TAB_zrprfp0000 );
                }
#endif
			}
				
			// Crappy coord calcs - get sequences with final graphics.
			if(!DeedBars[orderArray[i].Index].ToBeDisplayed)
				continue;
								

			cnt++;
			x = 22 + 110*(cnt%7);
			y = 234 + 54*(cnt/7);
			tempPriority = DISPLAY_SSPropertyPriority;
				 			
			DeedBars[orderArray[i].Index].HotSpot.top = 13+y;
			DeedBars[orderArray[i].Index].HotSpot.left = x+42;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if ALTERNATE_DEED_SCREEN	
							                          
			// check to see if the property has been mortgaged
			if(UICurrentGameState.Squares[orderArray[i].Index].mortgaged)
			{
				// display the mortgaged sign accordingly
				LE_SEQNCR_StartXY(BankDeedSign[MORTGAGED].ID, BankDeedSign[MORTGAGED].Priority, 
					x+42-5,//+BankDeedSign[MORTGAGED].HotSpot.left,
					14+y+ BankDeedSign[MORTGAGED].HotSpot.top);
				BankDeedSign[MORTGAGED].IsObjectOn = TRUE;
			}
			 // check to see if the property has been sold
			else if(UICurrentGameState.Squares[orderArray[i].Index].owner < RULE_MAX_PLAYERS)
			{
				// display the sold sign accordingly
				LE_SEQNCR_StartXY(BankDeedSign[SOLD].ID, BankDeedSign[SOLD].Priority, 
					x+42-5,//+BankDeedSign[SOLD].HotSpot.left,
					14+y+BankDeedSign[SOLD].HotSpot.top);
				BankDeedSign[SOLD].IsObjectOn = TRUE;
			}
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	  				 
	
			// set remainder of order structure
			width = STATS_DEED_CardW; //LE_GRAFIX_AnyBitmapWidth (tempID) ;			// does not work for chunks  
			height = STATS_DEED_CardH;
			
			SetRect(&(orderArray[orderArray[i].Index].Location), x, y, x+width, y+height);
			orderArray[orderArray[i].Index].Priority = tempPriority;
      }
    } 
		else
    {// not visible.
      tempID = LED_EI;
    }

		if( DISPLAY_state.SSProperties[orderArray[i].Index] != tempID )
    {// Need corrective action
      if( DISPLAY_state.SSProperties[orderArray[i].Index] != LED_EI )
      {// Shut down old property
        LE_SEQNCR_Stop( DISPLAY_state.SSProperties[orderArray[i].Index], DISPLAY_state.SSPropertiesPriority[orderArray[i].Index] );
        DISPLAY_state.SSProperties[orderArray[i].Index] = LED_EI;
      }

      DISPLAY_state.SSProperties[orderArray[i].Index] = tempID;
      DISPLAY_state.SSPropertiesPriority[orderArray[i].Index] = tempPriority;

      if( DISPLAY_state.SSProperties[orderArray[i].Index] != LED_EI )
      {// Bring up new property
        LE_SEQNCR_StartXY( DISPLAY_state.SSProperties[orderArray[i].Index], DISPLAY_state.SSPropertiesPriority[orderArray[i].Index], 
														orderArray[orderArray[i].Index].Location.left,
														orderArray[orderArray[i].Index].Location.top );
				LE_SEQNCR_StartXY(DeedBars[orderArray[i].Index].ID, DeedBars[orderArray[i].Index].Priority,
							DeedBars[orderArray[i].Index].HotSpot.left, DeedBars[orderArray[i].Index].HotSpot.top);
				DeedBars[orderArray[i].Index].IsObjectOn = TRUE;
      }
    }
  }
/*
  //Update the jail cards
  for( iTemp = 0; iTemp < 2; iTemp++ )
  {
    if( (UICurrentGameState.Cards[iTemp].jailOwner == UICurrentGameState.CurrentPlayer) &&
        (DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA) )
      tempID = LED_IFT( DAT_MAIN, TAB_ibjlcdf0 ) + iTemp;/*LED_IFT( DAT_LANG2, CNK_jlcd00 )
    else
      tempID = LED_EI;

    if( DISPLAY_state.SSJailCards[iTemp] != tempID )
    {//Corrective action
      if( DISPLAY_state.SSJailCards[iTemp] != LED_EI )
				;
//        LE_SEQNCR_Stop( DISPLAY_state.SSJailCards[iTemp], DISPLAY_SSPropertyPriority );
      DISPLAY_state.SSJailCards[iTemp] = tempID;
      if( DISPLAY_state.SSJailCards[iTemp] != LED_EI )
				;
//        LE_SEQNCR_StartXY( DISPLAY_state.SSJailCards[iTemp], DISPLAY_SSPropertyPriority, 700, 280 + iTemp * 40 );
    }
  }
*/
}




/*****************************************************************************
	* Function:		void UDStats_RemoveDeeds(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Remove the display of all deeds.
	****************************************************************************
	*/
void UDStats_RemoveDeeds(void)
{

  register int iTemp;
  
	// Update the Property Display
  for( iTemp = 0; iTemp < SQ_MAX_SQUARE_TYPES; iTemp++ )
  {// Calculate the current propery ID

		if( DISPLAY_state.SSProperties[iTemp] != LED_EI )
    {// Shut down old property
//      if(!LE_SEQNCR_IsSequenceFinished (DISPLAY_state.SSProperties[orderArray[iTemp].Index], 
//						DISPLAY_state.SSPropertiesPriority[orderArray[iTemp].Index], TRUE))
//			{
				LE_SEQNCR_Stop( DISPLAY_state.SSProperties[iTemp], 
										DISPLAY_state.SSPropertiesPriority[iTemp] );
				DISPLAY_state.SSProperties[iTemp] = LED_EI;
				
//			}
    }

		if(DeedBars[iTemp].IsObjectOn)
		{
			LE_SEQNCR_Stop(DeedBars[iTemp].ID, DeedBars[iTemp].Priority);
			DeedBars[iTemp].IsObjectOn = FALSE;
			
		}
		DeedBars[iTemp].ToBeDisplayed = FALSE;
	
    orderArray[iTemp].Index			=	0;
		orderArray[iTemp].Priority	=	0;
    SetRect(&(orderArray[iTemp].Location), 0, 0, 0, 0);

  }

#if ALTERNATE_DEED_SCREEN
	if(BankDeedSign[SOLD].IsObjectOn)
	{
		LE_SEQNCR_Stop(BankDeedSign[SOLD].ID, BankDeedSign[SOLD].Priority); 
		BankDeedSign[SOLD].IsObjectOn = FALSE;
	}
	if(BankDeedSign[MORTGAGED].IsObjectOn)
	{
		LE_SEQNCR_Stop(BankDeedSign[MORTGAGED].ID, BankDeedSign[MORTGAGED].Priority); 
		BankDeedSign[MORTGAGED].IsObjectOn = FALSE;
	}
#endif
/*
  //Update the jail cards
  for( iTemp = 0; iTemp < 2; iTemp++ )
  {
    if( (UICurrentGameState.Cards[iTemp].jailOwner == UICurrentGameState.CurrentPlayer) &&
        (DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA) )
      tempID = LED_IFT( DAT_LANG2, CNK_jlcd00 ) + iTemp;
    else
      tempID = LED_EI;

    if( DISPLAY_state.SSJailCards[iTemp] != tempID )
    {//Corrective action
      if( DISPLAY_state.SSJailCards[iTemp] != LED_EI )
				;
//        LE_SEQNCR_Stop( DISPLAY_state.SSJailCards[iTemp], DISPLAY_SSPropertyPriority );
      DISPLAY_state.SSJailCards[iTemp] = tempID;
      if( DISPLAY_state.SSJailCards[iTemp] != LED_EI )
				;
//        LE_SEQNCR_StartXY( DISPLAY_state.SSJailCards[iTemp], DISPLAY_SSPropertyPriority, 700, 280 + iTemp * 40 );
    }
  }
*/
}





/*****************************************************************************
	* Function:		void UDStats_DisplayDeedInfo(int index, LE_DATA_DataId tempID, BOOL side)
	* Returns:		nothing
	* Parameters:	index into array of deeds
	*							id of deed to be blown up
	*							flag to know which side the floater should appear
	*							
	* Purpose:		Display info on deed selected.
	****************************************************************************
	*/
void UDStats_DisplayDeedInfo(int index, LE_DATA_DataId tempID, BOOL side)
{
	ACHAR		strOwner[30], strRent[10], strEarnings[10], strExpected[10];
	int width = 85, height = 21, xoffset;
	
	// set the font
	LE_FONTS_GetSettings(1);

	LE_GRAFIX_ColorArea(DeedInfoFloater.OthersID, 0, 0, 400, 235, LE_GRAFIX_MakeColorRef (0, 255, 0));
	LE_GRAFIX_ColorArea(DeedInfoFloater.OthersID, 193, 5, 200, 230, LE_GRAFIX_MakeColorRef (0, 0, 0));
	
	////////////////////////////////////////////////////////////////////////////////////////
	// determine owner of selected deed
    LE_FONTS_GetSettings(3);
    LE_FONTS_Print(DeedInfoFloater.OthersID, 15, 10,
        LE_GRAFIX_MakeColorRef (255, 255, 255), LANG_GetTextMessage(TMN_OWNER));
	int ownerIndex = UICurrentGameState.Squares[index].owner;
	// copy the owners name into a temp string variable
	if(ownerIndex >= RULE_MAX_PLAYERS)
		Astrcpy(strOwner, LANG_GetTextMessage(TMN_BANK_NAME));
	else
		Astrcpy(strOwner, UICurrentGameState.Players[ownerIndex].name); 
    LE_FONTS_GetSettings(1);
    int currentfontsize = 14;
    while(LE_FONTS_GetStringWidth(strOwner) > 164)
    {
        currentfontsize--;
        LE_FONTS_SetSize(currentfontsize);
	    LE_FONTS_SetWeight(700);
    }
	// ready the object to display the owner
	xoffset = UDStats_PrintStringJustified(strOwner, RIGHT_JUSTIFIED, 164);
	LE_FONTS_Print(DeedInfoFloater.OthersID, xoffset+10, 28, LE_GRAFIX_MakeColorRef (255, 255, 255), strOwner);


	////////////////////////////////////////////////////////////////////////////////////////
	// determine rent of selected deed
    LE_FONTS_GetSettings(3);
	RULE_PropertySetOwnedByPlayer(&UICurrentGameState, RULE_BANK_PLAYER);
    LE_FONTS_Print(DeedInfoFloater.OthersID, 15, 65,
        LE_GRAFIX_MakeColorRef (255, 255, 255), LANG_GetTextMessage(TMN_CURRENT_RENT));
	int tempRent = AI_Rent_If_Stepped_On(&UICurrentGameState, index, RULE_PropertySetOwnedByPlayer(&UICurrentGameState, ownerIndex));
	// convert the rent into a temp string variable
	//Asprintf( strRent, A_T("%*i"), 6, tempRent );
	UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(tempRent,	DISPLAY_state.system, strRent, FALSE);	
	// ready the object to display the owner
    LE_FONTS_GetSettings(1);
	xoffset = UDStats_PrintStringJustified(strRent, RIGHT_JUSTIFIED, 164);
	LE_FONTS_Print(DeedInfoFloater.OthersID, xoffset, 83, LE_GRAFIX_MakeColorRef (255, 255, 255), strRent);


	///////////////////////////////////////////////////////////////////////////////////////////
	// determine earnings of selected deed
    LE_FONTS_GetSettings(3);
    LE_FONTS_Print(DeedInfoFloater.OthersID, 15, 118,
        LE_GRAFIX_MakeColorRef (255, 255, 255), LANG_GetTextMessage(TMN_GAME_EARNINGS));
	int tempEarnings = UICurrentGameState.Squares[index].game_earnings;
	// convert the rent into a temp string variable
	//Asprintf( strEarnings, A_T("%*i"), 6, tempEarnings);
	UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(tempEarnings,	DISPLAY_state.system, strEarnings, FALSE);
	// ready the object to display the owner
    LE_FONTS_GetSettings(1);
	xoffset = UDStats_PrintStringJustified(strEarnings, RIGHT_JUSTIFIED, 164);
	LE_FONTS_Print(DeedInfoFloater.OthersID, xoffset, 136, LE_GRAFIX_MakeColorRef (255, 255, 255), strEarnings);


	///////////////////////////////////////////////////////////////////////////////////////////
	// determine expected earnings of selected deed
    LE_FONTS_GetSettings(3);
    int tempStrHeight = LE_FONTS_GetStringHeight(A_T("TEST"));
    if(LE_FONTS_GetStringWidth(LANG_GetTextMessage(TMN_FUTURE_VALUE_TO_YOU)) > 164)
    {
        UDStats_PrintString(DeedInfoFloater.OthersID, LANG_GetTextMessage(TMN_FUTURE_VALUE_TO_YOU),
            15, 167, 164, 167+tempStrHeight*2);
    }
    else
    {
	    LE_FONTS_Print(DeedInfoFloater.OthersID, 15, 182,
            LE_GRAFIX_MakeColorRef (255, 255, 255), LANG_GetTextMessage(TMN_FUTURE_VALUE_TO_YOU));
    }
	// determine the properties owned by the current player
	RULE_PropertySet properties_owned = RULE_PropertySetOwnedByPlayer(&UICurrentGameState,	
																							UICurrentGameState.Squares[index].owner);

	tempRent = AI_Rent_If_Stepped_On(&UICurrentGameState, index,	properties_owned);
	// convert the rent into a temp string variable
	//Asprintf( strExpected, A_T("%*i"), 6, tempRent);
	UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(tempRent,	DISPLAY_state.system, strExpected, FALSE);
	// ready the object to display the owner
    LE_FONTS_GetSettings(1);
	xoffset = UDStats_PrintStringJustified(strExpected, RIGHT_JUSTIFIED, 164);
	LE_FONTS_Print(DeedInfoFloater.OthersID, xoffset, 200, LE_GRAFIX_MakeColorRef (255, 255, 255), strExpected);



	// display the deed info
	//LE_GRAFIX_ShowTCB_Alpha(DeedInfoFloater.OthersID, 193, 5, tempID);
	DeedInfoFloater.Others2ID = tempID;
	LE_SEQNCR_StartXY(DeedInfoFloater.Others2ID, DeedInfoFloater.Priority+2, (side*400)+10+193, 220+5);
	LE_SEQNCR_StartXY(DeedInfoFloater.ColourID, DeedInfoFloater.Priority, (side*400)+10, 220);
	LE_SEQNCR_StartXY(DeedInfoFloater.OthersID, DeedInfoFloater.Priority+1, (side*400)+10, 220);
	DeedInfoFloater.IsObjectOn = TRUE;

	// reset the fonts
	LE_FONTS_GetSettings(0);
}





/*****************************************************************************
	* Function:		void UDStats_DisplayPlayerDeeds(void)
	* Returns:		nothing
	* Parameters:	boolean value that determines if we are displaying
	*							players 1 to 4 (FALSE) or players 5 and 6 (TRUE)
	*							
	* Purpose:		Display the deeds currently owned by each player.
	****************************************************************************
	*/
void UDStats_DisplayPlayerDeeds(void)
{		
	int i, temp, j;
  int tempDX = 3;
  BOOL bFuturesOn = FALSE, bImmunitiesOn = FALSE;
  RULE_PropertySet propertyset=0, tempset=0;

	for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
	{
		temp = PlayerOrderArray[i/*index*/].Index;

		if((DISPLAY_state.IBarCurrentState == IBAR_STATE_Mortgage) ||
			(DISPLAY_state.IBarCurrentState == IBAR_STATE_UnMortgage) ||
			(DISPLAY_state.IBarCurrentState == IBAR_STATE_Build) ||
			(DISPLAY_state.IBarCurrentState == IBAR_STATE_Sell))
		{
			if(SlotIsALocalHumanPlayer [DISPLAY_state.IBarCurrentPlayer])
			{
				if(temp != DISPLAY_state.IBarCurrentPlayer)
					continue;
			}
		}

		// set the number of columns of deeds for each player
		UDStats_CountColumns(&PlayerDeeds[temp], temp);
		// reposition those deeds
		UDStats_RepositionDeeds(&PlayerDeeds[temp], temp, i);

    // display the futures and immunities icons if they are needed
    for(j=0; j<RULE_MAX_COUNT_HIT_SETS; j++)
    {
      if((UICurrentGameState.CountHits[j].toPlayer == (unsigned int)temp) &&
          (UICurrentGameState.CountHits[j].hitType == CHT_RENT_IMMUNITY))
      {
//        if(propertyset & UICurrentGameState.CountHits[j].properties)
//        {            
          Immunities[i].ID = LED_IFT(DAT_LANG2, TAB_syimm);
//        }
        propertyset |= UICurrentGameState.CountHits[j].properties;
      }
    }

    propertyset = 0;
    for(j=0; j<RULE_MAX_COUNT_HIT_SETS; j++)
    {
      if((UICurrentGameState.CountHits[j].toPlayer == (unsigned int)temp) &&
          (UICurrentGameState.CountHits[j].hitType == CHT_FUTURE_RENT))
      {
//        if(propertyset & UICurrentGameState.CountHits[j].properties)
//        {            
          Futures[i].ID = LED_IFT(DAT_LANG2, TAB_syfut);
//        }
        propertyset |= UICurrentGameState.CountHits[j].properties;
      }
    }

    // if the ids exist, then display them
    if(Immunities[i].ID != LED_EI)
    {
      Immunities[i].HotSpot.left = i*(boxWidth) + boxWidth-LE_GRAFIX_AnyBitmapWidth(Immunities[i].ID)-10+tempDX;
      Immunities[i].HotSpot.top = 224 + boxHeight-LE_GRAFIX_AnyBitmapHeight(Immunities[i].ID);
      Immunities[i].HotSpot.right = Immunities[i].HotSpot.left + LE_GRAFIX_AnyBitmapWidth(Immunities[i].ID);
      Immunities[i].HotSpot.bottom = Immunities[i].HotSpot.top + LE_GRAFIX_AnyBitmapHeight(Immunities[i].ID);
      Immunities[i].Priority = STATUS_DEED_Priority;
      // save the player number in var1
      Immunities[i].var1 = temp;
      LE_SEQNCR_StartXY(Immunities[i].ID, Immunities[i].Priority,
                      Immunities[i].HotSpot.left, Immunities[i].HotSpot.top);
    }
    else
    {
      Immunities[i].ID = LED_EI;
      SetRect(&(Immunities[i].HotSpot), 0, 0, 0, 0);
    }

    if(Futures[i].ID != LED_EI)
    {
      Futures[i].HotSpot.left = i*(boxWidth) + boxWidth-LE_GRAFIX_AnyBitmapWidth(Futures[i].ID)-10+tempDX;
      Futures[i].HotSpot.top = 224 + boxHeight-LE_GRAFIX_AnyBitmapHeight(Futures[i].ID)-20;
      Futures[i].HotSpot.right = Futures[i].HotSpot.left + LE_GRAFIX_AnyBitmapWidth(Futures[i].ID);
      Futures[i].HotSpot.bottom = Futures[i].HotSpot.top + LE_GRAFIX_AnyBitmapHeight(Futures[i].ID);
      Futures[i].Priority = STATUS_DEED_Priority;
      // save the player number in var1
      Futures[i].var1 = temp;
      LE_SEQNCR_StartXY(Futures[i].ID, Futures[i].Priority,
                      Futures[i].HotSpot.left, Futures[i].HotSpot.top);
    }
    else
    {
      SetRect(&(Futures[i].HotSpot), 0, 0, 0, 0);
      Futures[i].ID = LED_EI;
    }
    tempDX += 3;
	}	
	
}





/*****************************************************************************
	* Function:		void UDStats_RemovePlayerDeeds(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Remove the deeds currently owned by each player that are displayed.
	****************************************************************************
	*/
void UDStats_RemovePlayerDeeds(void)
{
	int i,j;
	BOOL found1=FALSE, found2=FALSE;

	for(i=0; i<RULE_MAX_PLAYERS/*UICurrentGameState.NumberOfPlayers*/; i++)
	{
		for( j = 0; j < SQ_MAX_SQUARE_TYPES; j++ )
		{// Take corrective action to change the display to the current property sets
			/*if( UICurrentGameState.Squares[j].owner != PlayerOrderArray[i].Index )
				continue;
		
			if( UICurrentGameState.Squares[j].mortgaged )
				tempID = LED_IFT( DAT_PAT, TAB_trprfm0000 );
			else
				tempID = LED_IFT( DAT_PAT, TAB_trprfp0000 );

			tempID += DISPLAY_propertyToOwnablePropertyConversion( j ); 
*/
			if(PlayerDeeds[i].IsObjectOn[j])
			{
				LE_SEQNCR_Stop(PlayerDeeds[i].ID[j], PlayerDeeds[i].Priority[j]);
				PlayerDeeds[i].IsObjectOn[j] = FALSE;
			}
			
		}
	
		// reset the DeedPosition structure to 0 values
		UDStats_ResetDeedPositions(&PlayerDeeds[i]);
    
    // stop all displays of immunity icons in one shot
    if(!found1 && (Immunities[i].ID != LED_EI))
    {
      LE_SEQNCR_Stop(Immunities[i].ID, Immunities[i].Priority);
      found1 = TRUE;
    }
    // stop all displays of future icons in one shot
    if(!found2 && (Futures[i].ID != LED_EI))
    {
      LE_SEQNCR_Stop(Futures[i].ID, Futures[i].Priority);
      found2 = TRUE;
    }

    // reset the immunities and futures icons
    SetRect(&(Immunities[i].HotSpot), 0, 0, 0, 0);
    Immunities[i].ID = LED_EI;
    SetRect(&(Futures[i].HotSpot), 0, 0, 0, 0);
    Futures[i].ID = LED_EI;

	}
  
	
	//Update the jail cards
  for( i = 0; i < 2; i++ )
  {    
    if( DISPLAY_state.SSJailCards[i] != LED_EI )
		{
			LE_SEQNCR_Stop( DISPLAY_state.SSJailCards[i], DISPLAY_SSPropertyPriority );
      DISPLAY_state.SSJailCards[i] = LED_EI;
		}
  }
}






/*****************************************************************************
	* Function:		void UDStats_DisplayPlayerBoxes(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the players boxes is a pre-determined order.
	****************************************************************************
	*/
void UDStats_DisplayPlayerBoxes(void)
{
	int i, temp;
	
	int tempDX = 3;
	for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
	{
		temp = PlayerOrderArray[i/*index*/].Index;

		if((DISPLAY_state.IBarCurrentState == IBAR_STATE_Mortgage) ||
			(DISPLAY_state.IBarCurrentState == IBAR_STATE_UnMortgage) ||
			(DISPLAY_state.IBarCurrentState == IBAR_STATE_Build) ||
			(DISPLAY_state.IBarCurrentState == IBAR_STATE_Sell))
		{
			if(SlotIsALocalHumanPlayer [DISPLAY_state.IBarCurrentPlayer])
			{
				if(temp != DISPLAY_state.IBarCurrentPlayer)
					continue;
			}
		}
		
		LE_SEQNCR_StartXY(PlayerOrderArray[temp].OthersID, PlayerOrderArray[temp].Priority+1, (i*(boxWidth))+tempDX, 224);
		LE_SEQNCR_StartXY(PlayerOrderArray[temp].ColourID, PlayerOrderArray[temp].Priority, (i*(boxWidth))+tempDX, 224);
		PlayerOrderArray[temp].IsObjectOn = TRUE;
		tempDX += 3;
	}
}





/*****************************************************************************
	* Function:		void UDStats_RemovePlayerBoxes(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Removes the display of players boxes.
	****************************************************************************
	*/
void UDStats_RemovePlayerBoxes(void)
{
	int i;
	
	// stop all previous displays of player boxes
	for(i=0; i<RULE_MAX_PLAYERS/*UICurrentGameState.NumberOfPlayers*/; i++)
	{
		if(PlayerOrderArray[i].IsObjectOn)
		{
			LE_SEQNCR_Stop(PlayerOrderArray[i].ColourID, PlayerOrderArray[i].Priority);
			LE_SEQNCR_Stop(PlayerOrderArray[i].OthersID, PlayerOrderArray[i].Priority+1);
			//LE_DATA_FreeRuntimeDataID( PlayerOrderArray[i].OthersID );
			//PlayerOrderArray[i].OthersID = LED_EI;
			PlayerOrderArray[i].IsObjectOn = FALSE;
		}		
	}		
}







/*****************************************************************************/
/*****************************************************************************
	* Function:		void UDStats_PositionDeedOwned(PropertyLocationStruct *pps, int order,
	*													int index)
	* Returns:		nothing
	* Parameters:	pointer to structure to fill
	*							integer representing the order of the property
	*							index into array of boardpieces
	* Purpose:		Sets the position of the property in the trade screen
	****************************************************************************
	*/
void UDStats_PositionDeedOwned(PlayerDeedPositionStruct* pps, int order, int index)
{		
	int x, y;
	int dim_two = order%3;
	int dim_one = order/3;
	
	short width_apart=0;

	if(pps->numColumns)
		width_apart = (deedBoxWidth-(STATS_DEED_DeltaX*2)-STATS_DEED_CardW) / pps->numColumns;

	if(width_apart > (STATS_DEED_CardW * 2))
		width_apart = STATS_DEED_CardW * 2;
	
	int tempDX = 3 +(3*pps->player_column);
	x = (pps->player_column)*(boxWidth)+tempDX +deedBoxX + (pps->column[dim_one][dim_two] * width_apart) + (STATS_DEED_DeltaX*dim_two);
	y = deedBoxY + (STATS_DEED_DeltaY*dim_two);

	// set the locations for the owned properties
  pps->Location[index].left   = x;
  pps->Location[index].top    = y;
  pps->Location[index].right  = x + STATS_DEED_CardW;
  pps->Location[index].bottom = y + STATS_DEED_CardH;

	pps->Priority[index] = (UNS16)order + STATUS_DEED_Priority;

}



/*****************************************************************************/
/*****************************************************************************
	* Function:		void UDStats_RepositionDeeds(PropertyPositionStruct* pps, 
	*																		int pindex, int column)
	*
	* Returns:		nothing
	* Parameters:	pointer to structure to modify
	*							index into array of players
	*							player box column 
	*							
	* Purpose:		Modifies all info relating to the deeds owned by each player
	*							and redisplays any changes made
	****************************************************************************
	*/
void UDStats_RepositionDeeds(PlayerDeedPositionStruct* pps, int pindex, int column)
{
	int t;
	UNS16 order;
	LE_DATA_DataId	tempID;
	int i;
	RULE_PropertySet build_set = DISPLAY_PlaceBuildingLegalSquares( DISPLAY_state.IBarCurrentPlayer );
	RULE_PropertySet sell_set = DISPLAY_SellBuildingLegalSquares( DISPLAY_state.IBarCurrentPlayer );

	for( i = 0; i < SQ_MAX_SQUARE_TYPES; i++ )
  {
		
		if( UICurrentGameState.Squares[i].owner != pindex/*PlayerOrderArray[pindex].Index*/ )
				continue;
		
		t = DISPLAY_propertyToOwnablePropertyConversion( i ); //we will assume this is always valid if used.
					
		if(DISPLAY_state.IBarCurrentState == IBAR_STATE_Mortgage)
		{
			if( UDIBAR_testMortgagable( i ) )
            {
#if USA_VERSION
				tempID = LED_IFT(DAT_PAT, TAB_trprfp0000);
#else
                if(iLangId == NOTW_LANG_FRENCH)
                    tempID = LED_IFT(DAT_PAT, TAB_xrprfp0000);
                else
                    tempID = LED_IFT(DAT_PAT, TAB_zrprfp0000);
#endif
            }
			else
				continue;
		}
		else if(DISPLAY_state.IBarCurrentState == IBAR_STATE_UnMortgage)
		{
			if(UICurrentGameState.Squares[i].mortgaged)
            {
#if USA_VERSION
				tempID = LED_IFT(DAT_PAT, TAB_trprfm0000);
#else
                if(iLangId == NOTW_LANG_FRENCH)
                    tempID = LED_IFT(DAT_PAT, TAB_xrprfm0000);
                else
                    tempID = LED_IFT(DAT_PAT, TAB_zrprfm0000);
#endif
            }
			else
				continue;
		}
		else if(DISPLAY_state.IBarCurrentState == IBAR_STATE_Build)
		{
			if(RULE_PropertyToBitSet(i) & build_set)
			{
#if USA_VERSION
				tempID = LED_IFT(DAT_PAT, TAB_trprfp0000);
#else
                if(iLangId == NOTW_LANG_FRENCH)
                    tempID = LED_IFT(DAT_PAT, TAB_xrprfp0000);
                else
                    tempID = LED_IFT(DAT_PAT, TAB_zrprfp0000);
#endif
			}
			else
				continue;

		}
		else if(DISPLAY_state.IBarCurrentState == IBAR_STATE_Sell)
		{
			if(RULE_PropertyToBitSet(i) & sell_set)
			{
#if USA_VERSION
				tempID = LED_IFT(DAT_PAT, TAB_trprfp0000);
#else
                if(iLangId == NOTW_LANG_FRENCH)
                    tempID = LED_IFT(DAT_PAT, TAB_xrprfp0000);
                else
                    tempID = LED_IFT(DAT_PAT, TAB_zrprfp0000);
#endif
			}
			else
				continue;

		}
		else
		{
			if( UICurrentGameState.Squares[i].mortgaged )
            {
#if USA_VERSION
				tempID = LED_IFT( DAT_PAT, TAB_trprfm0000 );
#else
                if(iLangId == NOTW_LANG_FRENCH)
                    tempID = LED_IFT(DAT_PAT, TAB_xrprfm0000);
                else
                    tempID = LED_IFT(DAT_PAT, TAB_zrprfm0000);
#endif
            }
			else
            {
#if USA_VERSION
				tempID = LED_IFT( DAT_PAT, TAB_trprfp0000 );
#else
                if(iLangId == NOTW_LANG_FRENCH)
                    tempID = LED_IFT(DAT_PAT, TAB_xrprfp0000);
                else
                    tempID = LED_IFT(DAT_PAT, TAB_zrprfp0000);
#endif
            }
		}

		pps->ID[i] = tempID + t; 

			
		order = IBARPropertyBarOrder[t]; // Obtain display order in tray.

		pps->player_column = column;
		// set the position of the property relevent to it's trade box
		UDStats_PositionDeedOwned(pps, order, i);
				
		LE_SEQNCR_StartXY( pps->ID[i], pps->Priority[i],
			pps->Location[i].left,pps->Location[i].top );
		pps->IsObjectOn[i] = TRUE;
				
	}		

	//Update the jail cards
	int tempDX, tempx;
  for( i = 0; i < 2; i++ )
  {
    if(UICurrentGameState.Cards[i].jailOwner == pindex) 
      tempID = LED_IFT(DAT_LANG2, TAB_indsgoojc01) + i;//LED_IFT( DAT_MAIN, TAB_ibjlcdf0 ) + i;
    else
			continue;

    DISPLAY_state.SSJailCards[i] = tempID;
    if( DISPLAY_state.SSJailCards[i] != LED_EI )
		{
			tempDX = 3 +(3*column);
			tempx = column*boxWidth+tempDX+10;
			LE_SEQNCR_StartXY( DISPLAY_state.SSJailCards[i], DISPLAY_SSPropertyPriority, tempx, 385 + i * 30 );
		}
    
  }
	
}



/*****************************************************************************/
/*****************************************************************************
	* Function:		void UDStats_ResetDeedPositions(PropertyPositionStruct* pps)
	* Returns:		nothing
	* Parameters:	pointer to structure to be reset to zero
	*							
	* Purpose:		Resets all info relating to the Properties for the
	*							trade screen
	****************************************************************************
	*/
void UDStats_ResetDeedPositions(PlayerDeedPositionStruct* pps)
{
	for(int i=0; i<11; i++)
	{
		for(int j=0; j<3; j++)
		{
			// reset the columns
			pps->column[i][j] = -1;
		}
		// reset the columns flag
		pps->isColumnCounted[i] = FALSE; 
	}

	if(  DISPLAY_state.desired2DView != DISPLAY_SCREEN_TradeA  )
	{
		for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
		{
			//pps->Priority[i] = 0;
			SetRect (&(pps->Location[i]), 0, 0, 0, 0);
			pps->ID[i] = LED_EI;
		}
	}

	// reset the number of columns
	pps->numColumns = -1;
	
}



/*****************************************************************************/
/*****************************************************************************
	* Function:		void UDStats_CountColumns(PropertyPositionStruct *pps, int pindex)
	* Returns:		nothing
	* Parameters:	array of property position structures to pass to the Sort fucntion
	*									being called
	*							index into array of players
	* Purpose:		Determines the number of columns of deeds 
	*							for a player and sorts the deeds in the proper order
	*							to maintain a uniform display
	****************************************************************************
	*/
void UDStats_CountColumns(PlayerDeedPositionStruct* pps, int pindex)
{
	short column_counter = 0;
		
	int cindex, order, t;
	
	for( int i = 0; i < SQ_MAX_SQUARE_TYPES; i++ )
  {
    if( UICurrentGameState.Squares[i].owner != pindex/*PlayerOrderArray[pindex].Index*/ )
				continue;
    
		t = DISPLAY_propertyToOwnablePropertyConversion( i );
    order = IBARPropertyBarOrder[t]; // Obtain display order in tray.
  
		// calculate the column the property should be in if player owned
		// all properties
		cindex = order/3;
		
		// if the columns hasn't already been counted	
		if(!pps->isColumnCounted[cindex])
		{
			column_counter++;
			pps->isColumnCounted[cindex] = TRUE;
		}
			
	}

	// sort properties so they maintain a similar position as the other
	// interfaces
	UDStats_SortDeedPositions(pps);

	// set number of columns for that trade box
	pps->numColumns = column_counter;

}



/*****************************************************************************/
/*****************************************************************************
	* Function:		void UDStats_SortPropertyPositions(PropertyPositionStruct* pps)
	* Returns:		nothing
	* Parameters:	array of property positions so we can set the column member
	*							
	* Purpose:		Knowing which deeds are owned, we can determine the
	*							order in which they should appear on screen for each player.
	****************************************************************************
	*/
void UDStats_SortDeedPositions(PlayerDeedPositionStruct* pps)
{
	short temp_counter = 0;
	
	for(int i=0; i< 11; i++)
	{
		if(pps->isColumnCounted[i])
		{
			pps->column[i][0] = pps->column[i][1] = pps->column[i][2] = temp_counter;
			temp_counter++;
		}
	}
}




/*****************************************************************************
	* Function:		short UDStats_GetButtonIndex(LE_QUEUE_MessagePointer UIMessagePntr, short Area )
	* Returns:		the index into the array of the button pressed
	* Parameters:	message pointer which holds the mouse coordinates
	*							flag that determines if the mouse was clicked on the misc, calculator
	*							or the token area.
	*							
	* Purpose:		determine which button was pressed 
	****************************************************************************
	*/
short UDStats_GetButtonIndex(LE_QUEUE_MessagePointer UIMessagePntr, short Area )
{
	short i;
	POINT ptMousePos = { UIMessagePntr->numberA, UIMessagePntr->numberB };
    
	// verify for calculator buttons
	if(Area == CALC_BUTTONS_AREA)
	{
		for(i=0; i<11; i++)
		{
			if(PtInRect(&(CalculatorButton[i].HotSpot), ptMousePos))
			{
				return i;
			}
		}
	}
	
	// verify for miscellaneous buttons
	if(Area == MISC_BUTTONS_AREA)
	{
		for(i=0; i<8; i++)
		{
			if(PtInRect(&(FunctionButton[i].HotSpot), ptMousePos))
			{
				return i;
			}
		}
	}


	// verify for tokens
	if(Area == TOKEN_AREA)
	{
		// go through all the token locations and return the player number or -1 if
		// user clicked somewhere other than a token
				
		for(i=0; i <	UICurrentGameState.NumberOfPlayers; i++)
		{
			//if(i == UICurrentGameState.CurrentPlayer)
				//continue;
/*
			// check token location for mouse clicks
			if( (UIMessagePntr->numberA >= CALC_X + tokenLocation[i].left) &&
					(UIMessagePntr->numberA <  CALC_X + tokenLocation[i].right) &&
					(UIMessagePntr->numberB >= CALC_Y + tokenLocation[i].top) &&
					(UIMessagePntr->numberB <  CALC_Y + tokenLocation[i].bottom) )
			{
				return i;
			}
*/
			if(PtInRect(&tokenLocation[i], ptMousePos))
			{
				return i;
			}
		}
	}


	// verify for sort status buttons
	if(Area == SORT_BUTTONS_AREA)
	{
		for(i=0; i<4; i++)
		{
			if(PtInRect(&(SortButton[i].HotSpot), ptMousePos))
			{
				return i;
			}
		}
	}
	

	// verify for category status buttons
	if(Area == CATEGORY_BUTTONS_AREA)
	{
		for(i=0; i<3; i++)
		{
			if(PtInRect(&(CategoryButton[i].HotSpot), ptMousePos))
			{
				return i;
			}
		}
	}
	
	
	// verify for popup window
	if(Area == POP_UP_AREA)
	{
    if(ButtonClickStep == ROLL_OVER)
    {
      // check for click inside the popup window box
      RECT  temp = {POP_UP_X, POP_UP_Y, POP_UP_X+400, POP_UP_Y+200};
      if(PtInRect(&temp, ptMousePos))
        return 1;
    }
    else
    {
		  ptMousePos.x -= POP_UP_X;
		  ptMousePos.y -= POP_UP_Y;

		  for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
		  {
			  if(PtInRect(&(DeedLocation[i]), ptMousePos))
			  {
				  return i;
			  }
		  }
    }
	}


	// verify for mortgage deed clicking
	if(Area == MORTGAGE_AREA)
	{
		int max = -1, max_index = -1;
		
		for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
		{
			if(UICurrentGameState.Squares[i].owner != DISPLAY_state.IBarCurrentPlayer )
				continue;
		
			if(DISPLAY_state.IBarCurrentState == IBAR_STATE_Mortgage)
			{
				if( !UDIBAR_testMortgagable( i ) )
					continue;
			}
			else
			{
				if(!UICurrentGameState.Squares[i].mortgaged)
					continue;
			}

			if(fStatusScreen == PLAYER_SCREEN)
			{
				if(PtInRect(&(PlayerDeeds[DISPLAY_state.IBarCurrentPlayer].Location[i]), ptMousePos))
				{
					if(max < PlayerDeeds[DISPLAY_state.IBarCurrentPlayer].Priority[i])
					{
						max = PlayerDeeds[DISPLAY_state.IBarCurrentPlayer].Priority[i];
						max_index = i;
					}
					
				}
			}
			else if(fStatusScreen == DEED_SCREEN)
			{
				if(PtInRect(&(orderArray[i].Location), ptMousePos))
					return i;									
			}
		}
		return max_index;		
	}



	// verify for build deed clicking
	if(Area == BUILD_SELL_AREA)
	{
		int max = -1, max_index = -1;
		RULE_PropertySet build_set = DISPLAY_PlaceBuildingLegalSquares( DISPLAY_state.IBarCurrentPlayer );
		RULE_PropertySet sell_set = DISPLAY_SellBuildingLegalSquares( DISPLAY_state.IBarCurrentPlayer );

		for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
		{
			if(UICurrentGameState.Squares[i].owner != DISPLAY_state.IBarCurrentPlayer )
				continue;
		
			if(DISPLAY_state.IBarCurrentState == IBAR_STATE_Build)
			{
				if( !(RULE_PropertyToBitSet(i) & build_set) )
					continue;
			}
			else
			{
				if( !(RULE_PropertyToBitSet(i) & sell_set) )
					continue;
			}
			
			if(fStatusScreen == PLAYER_SCREEN)
			{
				if(PtInRect(&(PlayerDeeds[DISPLAY_state.IBarCurrentPlayer].Location[i]), ptMousePos))
				{
					if(max < PlayerDeeds[DISPLAY_state.IBarCurrentPlayer].Priority[i])
					{
						max = PlayerDeeds[DISPLAY_state.IBarCurrentPlayer].Priority[i];
						max_index = i;
					}					
				}
			}
			else if(fStatusScreen == DEED_SCREEN)
			{
				if(PtInRect(&(orderArray[i].Location), ptMousePos))
					return i;
			}
		}
		return max_index;
	}



  // verify for arrows in bank account history sort
	if(Area == BANK_ARROWS_AREA)
	{
		for(i=0; i<2; i++)
		{
			if((PtInRect(&(ArrowButton[i].HotSpot), ptMousePos)) 
        && (ArrowButton[i].State != ISTATBAR_BUTTON_Off))
			{
				return i;
			}
		}
	}


    // verify for immunities icon in player screen
	if(Area == IMMUNITIES_AREA)
	{
		for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
		{
			if((PtInRect(&(Immunities[i].HotSpot), ptMousePos)))
			{
				return i;
			}
		}
	}

    // verify for futures icon in player screen
	if(Area == FUTURES_AREA)
	{
		for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
		{
			if((PtInRect(&(Futures[i].HotSpot), ptMousePos)))
			{
				return i;
			}
		}
	}  


  // verify for arrows in player futures and immunities popup window
	if(Area == PLAYER_ARROWS_AREA)
	{
		for(i=0; i<2; i++)
		{
			if((PtInRect(&(ArrowButton[i].HotSpot), ptMousePos)) 
        && (ArrowButton[i].State != ISTATBAR_BUTTON_Off))
			{
				return i;
			}
		}
	}

	return -1;
}






/*****************************************************************************
	* Function:		void UDStats_DisplayInstructionText(ACHAR* string )
	* Returns:		nothing
	* Parameters:	string that will hold instruction
	*							
	* Purpose:		display the instructions 
	****************************************************************************
	*/
void UDStats_DisplayInstructionText(ACHAR* string )
{
	// check for previous display
	if(InstructBox.IsObjectOn)
	{
		LE_SEQNCR_Stop(InstructBox.ID,	InstructBox.Priority);
		InstructBox.IsObjectOn = FALSE;
	}

	// erase the current content of the instruction box
	LE_GRAFIX_ColorArea(InstructBox.ID, 0, 0, INST_DISPLAY_WIDTH, INST_DISPLAY_HEIGHT, LE_GRAFIX_MakeColorRef (0, 0, 0));
	
#if USA_VERSION
	// set the font
	LE_FONTS_GetSettings(2);

    LE_FONTS_Print(InstructBox.ID, 5, 0, LE_GRAFIX_MakeColorRef (255, 255, 255), 
							string);
#else

    if(iLangId == NOTW_LANG_FINNISH)
    {
        // set the font
	    LE_FONTS_GetSettings(0);    
        LE_FONTS_SetSize(7);
	    LE_FONTS_SetWeight(500);  
	
        UDStats_PrintString(InstructBox.ID, string, 5, -1, INST_DISPLAY_WIDTH, INST_DISPLAY_HEIGHT);
    }
    else
    {
        // set the font
	    LE_FONTS_GetSettings(2);

        LE_FONTS_Print(InstructBox.ID, 5, 0, LE_GRAFIX_MakeColorRef (255, 255, 255), 
							string);
    }
#endif
    	
    LE_SEQNCR_StartXY(InstructBox.ID, InstructBox.Priority, INST_DISPLAY_X, INST_DISPLAY_Y);
	InstructBox.IsObjectOn = TRUE;

	
	// reset the fonts
	LE_FONTS_GetSettings(0);
	return;
}






/*****************************************************************************
	* Function:		void UDStats_DisplayResultText(ACHAR* string )
	* Returns:		nothing
	* Parameters:	string that will hold the result to be displayed
	*							
	* Purpose:		display the results of function 
	****************************************************************************
	*/
void UDStats_DisplayResultText(ACHAR* string )
{
	int xoffset;

	// check for previous display
	if(ResultBox.IsObjectOn)
	{
		LE_SEQNCR_Stop(ResultBox.ID,	ResultBox.Priority);
		ResultBox.IsObjectOn = FALSE;
	}

	// erase the current content of the result box
	LE_GRAFIX_ColorArea(ResultBox.ID, 0, 0, RESULT_DISPLAY_WIDTH,RESULT_DISPLAY_HEIGHT, LE_GRAFIX_MakeColorRef (0, 0, 0));

	// set the font
	LE_FONTS_GetSettings(2);

	xoffset = UDStats_PrintStringJustified(string, RIGHT_JUSTIFIED, RESULT_DISPLAY_WIDTH);
	LE_FONTS_Print(ResultBox.ID, xoffset, 0, LE_GRAFIX_MakeColorRef (154, 154, 154), 
							string);

	LE_SEQNCR_StartXY(ResultBox.ID, ResultBox.Priority, RESULT_DISPLAY_X, RESULT_DISPLAY_Y);
	ResultBox.IsObjectOn = TRUE;
	
	// reset the fonts
	LE_FONTS_GetSettings(0);
	return;
}





/*****************************************************************************
	* Function:		void UDStats_DisplayFunctionDescText(int index)
	* Returns:		nothing
	* Parameters:	indexes into the array of description strings
	*							
	* Purpose:		display the description  of function 
	****************************************************************************
	*/
void UDStats_DisplayFunctionDescText(int index)
{	
	// check for previous display
	if(DescriptionBox.IsObjectOn)
	{
		LE_SEQNCR_Stop(DescriptionBox.ID,	DescriptionBox.Priority);
		DescriptionBox.IsObjectOn = FALSE;
	}

	// erase the current content of the description box
	LE_GRAFIX_ColorArea(DescriptionBox.ID, 0, 0, DESCRIPTION_DISPLAY_WIDTH, 
                  DESCRIPTION_DISPLAY_HEIGHT, LE_GRAFIX_MakeColorRef (0, 0, 0));

	// set the font
	LE_FONTS_GetSettings(2);

  UDStats_PrintString(DescriptionBox.ID, FunctionDescText[index],
                            5, 0, DESCRIPTION_DISPLAY_WIDTH-10, DESCRIPTION_DISPLAY_HEIGHT);

  /*
	int MaxWidth = LE_FONTS_GetStringWidth(FunctionDescText[index]);
	int MaxLength = Astrlen(FunctionDescText[index]) - 1;	// -1 because we read the new line character into string
	int size = 0;
	int SrcIndex = 0;
	int tempLength = 0;
	int lineCnt = 0;
	int tmpHeight = 0;
	ACHAR*  tmpString;

	while( MaxLength > 0 )
	{
		size = sizeof(ACHAR) * (MaxLength+1);

		tmpString = (ACHAR*)malloc(/*sizeof(ACHAR) * (MaxLength+1)// size);
		ZeroMemory(tmpString, size);

		UDStats_GetCharsFromString(FunctionDescText[index], SrcIndex,	DESCRIPTION_DISPLAY_WIDTH-10,
									MaxLength, tmpString);

		tempLength = Astrlen(tmpString);
		MaxLength -= tempLength;
		SrcIndex += tempLength;
	
		tmpHeight = LE_FONTS_GetStringHeight(tmpString);

		LE_FONTS_Print(DescriptionBox.ID, 5, lineCnt*tmpHeight, LE_GRAFIX_MakeColorRef (154, 154, 154), 
							tmpString);

		lineCnt++;
		free(tmpString);

	}
*/
	LE_SEQNCR_StartXY(DescriptionBox.ID, DescriptionBox.Priority, DESCRIPTION_DISPLAY_X, DESCRIPTION_DISPLAY_Y);
	DescriptionBox.IsObjectOn = TRUE;

	// reset the fonts
	LE_FONTS_GetSettings(0);
	return;

}





/*****************************************************************************
	* Function:		void UDStats_GetCharsFromString(ACHAR* SrcString, int SrcIndex,
	*																			int MaxWidth, int MaxCnt, ACHAR* DestString)
	* Returns:		nothing
	* Parameters:	source string to get the characters from
	*							index into the source string from where we should start copying chars
	*							Max width the new string may have ( font dependent)
	*							max count of chars in the string
	*							destination string
	*							
	* Purpose:		returns part of a string to ease the printing into a DataID 
	****************************************************************************
	*/
int UDStats_GetCharsFromString(ACHAR* SrcString, int SrcIndex, int MaxWidth, int MaxCnt, ACHAR* DestString)
{
	int			tmpStrLen = 0;								// length of temp string so far
	int			tmpIndex = 0;											// # 
	int			cnt = 0;
    
			
	while((MaxWidth > tmpStrLen) && (cnt < MaxCnt) 
			&& (SrcString[SrcIndex] > 31))// && (SrcString[SrcIndex] < 176))
	{
    	DestString[tmpIndex] = SrcString[SrcIndex];
		tmpIndex++;
		SrcIndex++;
		cnt++;
		tmpStrLen = LE_FONTS_GetStringWidth(DestString); 
	}

	
	// we now need to check if we changed lines on a space character
	// if not, we need to find the last space character and end the string there
	if(MaxWidth <= tmpStrLen)
	{
		// backup to the last character in the array
		tmpIndex--;

		while(DestString[tmpIndex] != 32)
			tmpIndex--;											// back up

		// once we've found the last space character, insert a null in the next array element
		tmpIndex++;
		DestString[tmpIndex] = 0;

	}

	return tmpIndex;
}







/*****************************************************************************
	* Function:		void UDStats_DisplayTokenList(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		display the list of tokens so that user can pick one
	****************************************************************************
	*//*
void UDStats_DisplayTokenList(void)
{

	LE_DATA_DataId	tempID;
		
	// display all tokens except current player's
	// display the tokens representing each player
	int tempx = 0;
	
	for(int i=0; i <	UICurrentGameState.NumberOfPlayers; i++)
	{
		if(i == UICurrentGameState.CurrentPlayer)
			continue;

		tempID = LED_IFT( DAT_MAIN, TAB_inpsa+UICurrentGameState.Players[i].token );
		
		// set token location for mouse clicks
		tokenLocation[i].left = TOKENLIST_X + 48*(tempx%2); 
		tokenLocation[i].top =  TOKENLIST_X + 29*(tempx/2);
		tokenLocation[i].right = tokenLocation[i].left + 53; 
		tokenLocation[i].bottom = tokenLocation[i].top + 29;
		
		LE_GRAFIX_ShowTCB_Alpha(TokenListID, (short)tokenLocation[i].left, 
									(short)tokenLocation[i].top, tempID);
				
		tempx++;
	}

	//display the box that will hold the tokens
	LE_SEQNCR_StartXY(TokenListID, 110, CALC_X, CALC_Y);

}
*/




/*****************************************************************************
	* Function:		void UDStats_DisplayTokenList(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the calculator buttons.
	****************************************************************************
	*/
void UDStats_DisplayTokenList(void)
{
	int i, width, height, tempx = 0;
	float scale = (float)0.8;
	height = 20;

	width = 42;

	// display the tokens
	for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
	{
		//if(i == UICurrentGameState.CurrentPlayer)
			//continue;

		TokenList[i].ID = LED_IFT( DAT_MAIN, TAB_inpsa+UICurrentGameState.Players[i].token );
		TokenList[i].Priority = CalcButtonPriority;
		//width = (int)(LE_GRAFIX_ReturnObjectWidth(TokenList[i].ID)*scale);
		//height = (int)(LE_GRAFIX_ReturnObjectHeight(TokenList[i].ID)*scale);

		// set token location for mouse clicks
		tokenLocation[i].left = (long)(CALC_X + 1.1*width*(tempx%2)); 
		tokenLocation[i].top =  (long)(CALC_Y + 1.5*height*(tempx/2));
		tokenLocation[i].right = tokenLocation[i].left + width; 
		tokenLocation[i].bottom = tokenLocation[i].top + height;
		
		LE_SEQNCR_StartCXYSR (TokenList[i].ID, TokenList[i].Priority, LED_EI, 0, FALSE,
							(short)tokenLocation[i].left, (short)tokenLocation[i].top, scale, 0);
		TokenList[i].IsObjectOn = TRUE;				
		tempx++;
	}
			
}






/*****************************************************************************
	* Function:		void UDStats_RemoveTokenList(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Removes the calculator buttons.
	****************************************************************************
	*/
void UDStats_RemoveTokenList(void)
{
	int i;

	// remove the tokens
	for(i=0; i<RULE_MAX_PLAYERS/*UICurrentGameState.NumberOfPlayers*/; i++)
	{
		if(TokenList[i].IsObjectOn)
		{
			LE_SEQNCR_Stop(TokenList[i].ID, TokenList[i].Priority);
			TokenList[i].IsObjectOn = FALSE;				
		}
		
	}
	
}






/*****************************************************************************
	* Function:		void UDStats_RemoveFunctionDisplays(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Removes the display of anything the function buttons may have
	*							put on the screen and resets everything that is related to zero.
	****************************************************************************
	*/
void UDStats_RemoveFunctionDisplays(void)
{
	int i;

	// reset flags
	ButtonClickStep = ROLL_OVER;
	FunctionIndex = -1;

	// remove all function displays if any are running
	if(ResultDescBox.IsObjectOn)
	{
		LE_SEQNCR_Stop(ResultDescBox.ID, ResultDescBox.Priority);
		ResultDescBox.IsObjectOn = FALSE;
	}

	if(InstructBox.IsObjectOn)
	{
		LE_SEQNCR_Stop(InstructBox.ID, InstructBox.Priority);
		InstructBox.IsObjectOn = FALSE;
	}

	if(ResultBox.IsObjectOn)
	{
		LE_SEQNCR_Stop(ResultBox.ID, ResultBox.Priority);
		ResultBox.IsObjectOn = FALSE;
	}

	if(DescriptionBox.IsObjectOn)
	{
		LE_SEQNCR_Stop(DescriptionBox.ID, DescriptionBox.Priority);
		DescriptionBox.IsObjectOn = FALSE;
	}

	if(!LE_SEQNCR_IsSequenceFinished (TokenListID, 110, FALSE))
	{
		LE_SEQNCR_Stop(TokenListID, 110);
	}

	// reset all token location for the token list
	for(i=0; i<RULE_MAX_PLAYERS/*UICurrentGameState.NumberOfPlayers*/; i++)
		SetRect(&tokenLocation[i], 0,0,0,0);

}






/*****************************************************************************
	* Function:		void UDStats_ProcessMiscButtonPress(LE_QUEUE_MessagePointer UIMessagePntr )
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Determines the action depending on the location of the cursor
	*							for the function buttons.
	****************************************************************************
	*/
void UDStats_ProcessMiscButtonPress(LE_QUEUE_MessagePointer UIMessagePntr )
{
	
	short tempIndex;
		
	switch(ButtonClickStep)
	{
		
		case FIRST_STEP:	tempIndex = UDStats_GetButtonIndex(UIMessagePntr, MISC_BUTTONS_AREA);
							switch(tempIndex)
							{
							case 0:	UDStats_DisplayInstructionText(LANG_GetTextMessage(TMN_CALCULATOR_CLICK_DEED));
									UDStats_DisplayPopupDeedWindow();
									break;
							case 1:	UDStats_DisplayInstructionText(LANG_GetTextMessage(TMN_CALCULATOR_PICK_TOKEN));
									UDStats_RemoveCalcButtons();
									UDStats_DisplayTokenList();
									break;
							case 2:	UDStats_DisplayInstructionText(LANG_GetTextMessage(TMN_CALCULATOR_CLICK_DEED));
									UDStats_DisplayPopupDeedWindow();
									break;
							case 3:	UDStats_DisplayInstructionText(LANG_GetTextMessage(TMN_CALCULATOR_CLICK_DEED));
									UDStats_DisplayPopupDeedWindow();
                                    //UDStats_DisplayInstructionText(LANG_GetTextMessage(TMN_CALCULATOR_PICK_TOKEN));
									//UDStats_RemoveCalcButtons();
									//UDStats_DisplayTokenList();
									break;
							case 4:	UDStats_DisplayInstructionText(LANG_GetTextMessage(TMN_CALCULATOR_PICK_TOKEN));
									UDStats_RemoveCalcButtons();
									UDStats_DisplayTokenList();
									break;
							case 5:	UDStats_DisplayInstructionText(LANG_GetTextMessage(TMN_CALCULATOR_PICK_TOKEN));
									UDStats_RemoveCalcButtons();
									UDStats_DisplayTokenList();
									break;
							case 6:	UDStats_DisplayInstructionText(LANG_GetTextMessage(TMN_CALCULATOR_PICK_TOKEN));
									UDStats_RemoveCalcButtons();
									UDStats_DisplayTokenList();
									break;
							case 7:	UDStats_DisplayInstructionText(LANG_GetTextMessage(TMN_CALCULATOR_PICK_TOKEN));
									UDStats_RemoveCalcButtons();
									UDStats_DisplayTokenList();
									break;
							default:	ButtonClickStep--;
										break;
							}
							FunctionIndex = tempIndex;
							ButtonClickStep++;
							// display the press button
							LE_SEQNCR_Stop( FunctionButton[tempIndex].IdleID, CalcButtonPriority );
							LE_SEQNCR_Start( FunctionButton[tempIndex].PressID, CalcButtonPriority );
							FunctionButton[tempIndex].State = ISTATBAR_BUTTON_Press;
							break;

		// steps 2 and 3 for max/min expense/revenue will have to be expanded
		// once we have the proper formula to calculate everything
		// possible ways to expand is to write a function for pieces of code that are repeated
		// for several cases or write 2 switch statements that are seperated with the common code.

		case SECOND_STEP:		switch(FunctionIndex)
								{
									case 0:	
									case 2:	tempIndex = UDStats_GetButtonIndex(UIMessagePntr, POP_UP_AREA); 
											if(tempIndex < 0)
												ButtonClickStep--;
											else
											{
												FunctionResult.deed = tempIndex;
												UDStats_RemovePopupDeedWindow();
												ButtonClickStep++;
												UDStats_GetFunctionResult(FunctionIndex);
											}
											break;
									
									case 1:	tempIndex = UDStats_GetButtonIndex(UIMessagePntr, TOKEN_AREA);
											if(tempIndex < 0)
												ButtonClickStep--;
											else
											{
												FunctionResult.player = tempIndex;
												UDStats_GetFunctionResult(FunctionIndex);
												UDStats_RemoveTokenList();
												UDStats_DisplayCalcButtons();
											}
											break;

									case 3: tempIndex = UDStats_GetButtonIndex(UIMessagePntr, POP_UP_AREA); 
											if(tempIndex < 0)
												ButtonClickStep--;
											else
											{
												FunctionResult.deed = tempIndex;
												UDStats_RemovePopupDeedWindow();
												//ButtonClickStep++;
												UDStats_DisplayInstructionText(LANG_GetTextMessage(TMN_CALCULATOR_PICK_TOKEN));
									            UDStats_RemoveCalcButtons();
									            UDStats_DisplayTokenList();
											}
											break;
                                            /*
                                            tempIndex = UDStats_GetButtonIndex(UIMessagePntr, TOKEN_AREA); 
											if(tempIndex < 0)
												ButtonClickStep--;
											else
											{
												FunctionResult.player = tempIndex;
												UDStats_RemoveTokenList();
												UDStats_DisplayCalcButtons();
												UDStats_DisplayPopupDeedWindow();																		
												UDStats_DisplayInstructionText(LANG_GetTextMessage(TMN_CALCULATOR_CLICK_DEED));
											}
											break;
                                            */

									case 4:	
									case 5:
									case 6:
									case 7:	tempIndex = UDStats_GetButtonIndex(UIMessagePntr, TOKEN_AREA);
											if(tempIndex < 0)
												ButtonClickStep--;
											else
											{
												UDStats_RemoveTokenList();
												UDStats_DisplayCalcButtons();
												
												FunctionResult.player = tempIndex;
												// updated code June 25
												//UDStats_DisplayInstructionText(A_T("Please click a number"));
												FunctionResult.turns = 1;
												ButtonClickStep++;
												UDStats_GetFunctionResult(FunctionIndex);
												// have program press the number one to show user that we
												// are considering only one turn
												LE_SEQNCR_Stop( CalculatorButton[0].IdleID, CalcButtonPriority );
												LE_SEQNCR_Start( CalculatorButton[0].PressID, CalcButtonPriority );
												CalculatorButton[0].State = ISTATBAR_BUTTON_Press;
											}
											break;
									default:	break;
								}
								ButtonClickStep++;
								break;

		case THIRD_STEP:		switch(FunctionIndex)
								{
									case 1:	break;
									case 3: tempIndex = UDStats_GetButtonIndex(UIMessagePntr, TOKEN_AREA); 
											if(tempIndex < 0)
												ButtonClickStep--;
											else
											{
												FunctionResult.player = tempIndex;
												UDStats_RemoveTokenList();
												UDStats_DisplayCalcButtons();												
											}
											break;
                                            /*
                                            tempIndex = UDStats_GetButtonIndex(UIMessagePntr, POP_UP_AREA); 
											if(tempIndex < 0)
												ButtonClickStep--;
											else
											{
												FunctionResult.deed = tempIndex;
												UDStats_RemovePopupDeedWindow();
												tempIndex = 0;
											}
											break;
                                            */
									case 0: tempIndex = 0; break;
									case 2: tempIndex = 0; break;
									case 4:
									case 5:
									case 6:
									case 7:	tempIndex = 0; break;
								}
								if((tempIndex >= 0) && (tempIndex <= 8))
								{
									UDStats_GetFunctionResult(FunctionIndex);
									// display the press button
									LE_SEQNCR_Stop( CalculatorButton[tempIndex].IdleID, CalcButtonPriority );
									LE_SEQNCR_Start( CalculatorButton[tempIndex].PressID, CalcButtonPriority );
									CalculatorButton[tempIndex].State = ISTATBAR_BUTTON_Press;
									
								}
								ButtonClickStep++;
								break;


		case FOURTH_STEP:		ButtonClickStep++;	break;

		default:	break;
		
	}

}




/*****************************************************************************
	* Function:		void UDStats_GetFunctionResult(short function)
	* Returns:		nothing
	* Parameters:	integer determining which function we are implementing
	*							
	* Purpose:		Calls the proper function to get the result of the user's 
	*							calculations.
	****************************************************************************
	*/
void UDStats_GetFunctionResult(short function)
{
	ACHAR		resultStr[50];

	switch(function)
	{
		case 0:		// call the "ODDS" button result function
							UDStats_CalculateOdds(resultStr);
							break;

		case 1:		// call the "NET WORTH" button result function
							UDStats_CalculateNetWorth(resultStr);
							break;

		case 2:		// call the "FUTURE VALUE TO YOU" button result function
							UDStats_CalculateFutVal2You(resultStr);
							break;

		case 3:		// call the "FUTURE VALUE TO OTHER" button result function
							UDStats_CalculateFutVal2Other(resultStr);
							break;

		case 4:		// call the "MAXIMUM EXPENSE" button result function
							UDStats_CalculateMaxExpense(resultStr);
							break;

		case 5:		// call the "MINIMUM EXPENSE" button result function
							//UDStats_CalculateMinExpense(resultStr);
							UDStats_CalculateCurrentIncome(resultStr);
							break;

		case 6:		// call the "MAXIMUM REVENUE" button result function
							UDStats_CalculateMaxRevenue(resultStr);
							break;

		case 7:		// call the "MINIMUM REVENUE" button result function
							//UDStats_CalculateMinRevenue(resultStr);
							UDStats_CalculatePotentialIncome(resultStr);
							break;

		default:	resultStr[0] = NULL;	break;
	}

	// clear the instruction text box
	UDStats_DisplayInstructionText(A_T(""));
	// display result in the result text box
	UDStats_DisplayResultText(resultStr);
}





/*****************************************************************************
	* Function:		void UDStats_CalculateOdds(ACHAR * resultStr)
	* Returns:		nothing
	* Parameters:	result string
	*							
	* Purpose:		Calculates the odds of landing on a property after a number
	*							of turns.
	****************************************************************************
	*/
void UDStats_CalculateOdds(ACHAR * resultStr)
{

	float probability = 0.0;
	// first, determine the # of squares (or spaces) the player must move from his/her
	// spot to land on the selected property
	int current_player = UICurrentGameState.CurrentPlayer; 
	int player_square = UICurrentGameState.Players[current_player].currentSquare;
	int delta_square = FunctionResult.deed - player_square;
	// check to see if player picked a deed located behind him/her
	if(delta_square < 0)
		delta_square += 40;  // add the number of squares on the board to get proper result
	
	
	int max_sum = 12;						// max sum for one roll of 2 dice is 12
	int min_sum = 2;						// min sum for one roll of 2 dice is 2
	
	// if #squares is greater than the max sum, then it is impossible to roll 
	// the correct sum, therefore set # of squares to a value that will give a
	// probability of 0
	if(max_sum < delta_square)
		delta_square = 1;
	else if(min_sum > delta_square)
		delta_square = 1;
	
	
	probability = (float)((6 - abs(delta_square - 7)) / 36.0);
	// make it a percentage
	probability *= 100;
	Asprintf(resultStr, A_T("%3.1f"), probability);
	Astrcat(resultStr, A_T("%"));
	
}




/*****************************************************************************
	* Function:		void UDStats_CalculateNetWorth(ACHAR * resultStr)
	* Returns:		nothing
	* Parameters:	result string
	*							
	* Purpose:		Calculates the player's net worth.
	*							
	****************************************************************************
	*/
void UDStats_CalculateNetWorth(ACHAR * resultStr)
{
	long tmpNet;

	// get the liquid assets for the player selected
	// this call needs to be check to make sure it's the correct calculation
	//tmpNet = AI_Get_Liquid_Assets(&UICurrentGameState, (UCHAR)FunctionResult.player, FALSE, FALSE);
	tmpNet = AI_Get_Total_Worth(&UICurrentGameState, (UCHAR)FunctionResult.player);
	// set result string to be able to print
	//Asprintf(resultStr, A_T("%*li"), 0, tmpNet);
	UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(tmpNet, DISPLAY_state.system, resultStr, FALSE);

}




/*****************************************************************************
	* Function:		void UDStats_CalculateFutVal2You(ACHAR * resultStr)
	* Returns:		nothing
	* Parameters:	result string
	*							
	* Purpose:		Estimates the future value to you of a property after a
	*							number of turns.
	*							
	****************************************************************************
	*/
void UDStats_CalculateFutVal2You(ACHAR * resultStr)
{
	float rent=0;
	RULE_PropertySet    properties_owned;
	int current_player = UICurrentGameState.CurrentPlayer;
	int next_player;
	int deed = FunctionResult.deed;
	int square;

	// determine the properties owned by the current player
	properties_owned = RULE_PropertySetOwnedByPlayer(&UICurrentGameState,	current_player);

	// determine the next player to have his turn
	next_player = current_player + 1;
	// if we reach the last player, loop back to player #1
	if(next_player >= UICurrentGameState.NumberOfPlayers)
		next_player = 0;
	
	// determine if it is possible for this player to land on the chosen property
	square = UICurrentGameState.Players[next_player].currentSquare + 2;
	
	for(int i=0; i<11; i++)
	{
		square++;
		
		// make corrections if we wrapped around the board
		if(square >= 40)
			square -= 40;

		if(square==deed)
		{
			rent += (float)(AI_Rent_If_Stepped_On(&UICurrentGameState, deed,	properties_owned));/* * 
															AI_Property_Landing_Frequency[deed]);*/
			break;
		}
	}

	// set result string to be able to print
	//Asprintf(resultStr, A_T("%10.0f"), rent);
	UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((int)rent, DISPLAY_state.system, resultStr, FALSE);

}




/*****************************************************************************
	* Function:		void UDStats_CalculateFutVal2Other(ACHAR * resultStr)
	* Returns:		nothing
	* Parameters:	result string
	*							
	* Purpose:		Estimates the future value to another player of a property 
	*							after a number of turns.
	*							
	****************************************************************************
	*/
void UDStats_CalculateFutVal2Other(ACHAR * resultStr)
{

	float rent=0;
	RULE_PropertySet    properties_owned;
	int current_player = FunctionResult.player;
	int next_player;
	int deed = FunctionResult.deed;
	int square;
	
	// determine the properties owned by the current player
	properties_owned = RULE_PropertySetOwnedByPlayer(&UICurrentGameState,	current_player);
	
	// determine the next player to have his turn
	next_player = UICurrentGameState.CurrentPlayer + 1;
	// if we reach the last player, loop back to player #1
	if(next_player >= UICurrentGameState.NumberOfPlayers)
		next_player = 0;
	
	// determine if it is possible for this player to land on the chosen property
	square = UICurrentGameState.Players[next_player].currentSquare + 2;
	
	for(int i=0; i<11; i++)
	{
		square++;
		
		// make corrections if we wrapped around the board
		if(square >= 40)
			square -= 40;

		if(square==deed)
		{
			rent += (float)(AI_Rent_If_Stepped_On(&UICurrentGameState, deed,	properties_owned) * 
															AI_Property_Landing_Frequency[deed]);
			break;
		}
	}


	// set result string to be able to print
	//Asprintf(resultStr, A_T("%10.0f"), rent);
	UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((int)rent, DISPLAY_state.system, resultStr, FALSE);
}




/*****************************************************************************
	* Function:		void UDStats_CalculateMaxExpense(ACHAR * resultStr)
	* Returns:		nothing
	* Parameters:	result string
	*							
	* Purpose:		Calculates the maximum possible costs for a token after
	*							a number of turns.
	*							
	****************************************************************************
	*/
void UDStats_CalculateMaxExpense(ACHAR * resultStr)
{
	RULE_PropertySet    properties_owned;
	int	square, total, max_amount, temp_amount;
	int next_player;

	// get the square that the player is located on
	square = UICurrentGameState.Players[FunctionResult.player].currentSquare;

	total = 0;
	
	// determine the next player to have his turn
	next_player = UICurrentGameState.CurrentPlayer + 1;
	// if we reach the last player, loop back to player #1
	if(next_player >= UICurrentGameState.NumberOfPlayers)
		next_player = 0;

	// if it is not the token's turn, then he can not spend money
	if(next_player == FunctionResult.player)
	{
		// set result string to be able to print
		//Asprintf(resultStr, A_T("%*li"), 0, total);	
		UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(total, DISPLAY_state.system, resultStr, FALSE);
		return;
	}

	square+=2; // first roll can be no smaller than 2
	// loop for the number of turns user picked
	for(int i=0; i<FunctionResult.turns; i++)
	{
		
		max_amount = 0;
		
		// find the highes rent from player position+2 to player position+12
		for(int j=0; j<11; j++)
		{
			temp_amount = 0;
			
			if(square >= 40)
				square -= 40;

			// no need to check the rent if player owns it or of none owns it
			if((UICurrentGameState.Squares[square].owner < RULE_BANK_PLAYER) &&
				(UICurrentGameState.Squares[square].owner != FunctionResult.player))
			{
				properties_owned = RULE_PropertySetOwnedByPlayer(&UICurrentGameState,
																		UICurrentGameState.Squares[square].owner);
				temp_amount = AI_Rent_If_Stepped_On(&UICurrentGameState, square, properties_owned);

				properties_owned = 0;
			}

			if( temp_amount > max_amount)
				max_amount = temp_amount;

			// increment square position
			square++;
		}

		total += max_amount;
	}

	// set result string to be able to print
	//Asprintf(resultStr, A_T("%*li"), 0, total);
	UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(total, DISPLAY_state.system, resultStr, FALSE);

}





/*****************************************************************************
	* Function:		void UDStats_CalculateMaxRevenue(ACHAR * resultStr)
	* Returns:		nothing
	* Parameters:	result string
	*							
	* Purpose:		Calculates the maximum possible income for a token after
	*							a number of turns.
	*							
	****************************************************************************
	*/
void UDStats_CalculateMaxRevenue(ACHAR * resultStr)
{
	RULE_PropertySet    properties_owned;
	int	square, total, max_amount, temp_amount;
	int current_player = UICurrentGameState.CurrentPlayer;
	int next_player;
	int min_square[RULE_MAX_PLAYERS];
	int	max_square[RULE_MAX_PLAYERS];

	// set the variables
	for(int i=0; i<UICurrentGameState.NumberOfPlayers; i++)
	{
		min_square[i] = UICurrentGameState.Players[i].currentSquare;
		max_square[i] = 0;
	}

	// determine the properties owned by the player
	properties_owned = RULE_PropertySetOwnedByPlayer(&UICurrentGameState, (UCHAR)FunctionResult.player);

	
	// reset the total for the current player
	total = 0;		

	// loop for the number of turns user picked
	for(int turn=1; turn<=FunctionResult.turns; turn++)
	{			
		// determine the next player to have his turn
		next_player = current_player + turn;
		// if we reach the last player, loop back to player #1
		if(next_player >= UICurrentGameState.NumberOfPlayers)
			next_player = 0;
		// if the next player is the player making the calculation, skip this turn
		if(next_player == current_player)
			continue;

		max_amount = 0;
		
		// determine if it is possible for this player to land on the chosen property
		min_square[next_player] += max_square[next_player] + 2;
		max_square[next_player] = min_square[next_player] + 12;

		// make corrections if we wrapped around the board
		if(min_square[next_player] >= 40)
				min_square[next_player] -= 40;
		if(max_square[next_player] >= 40)
				max_square[next_player] -= 40;

		
		square = min_square[next_player];

		// find the highes rent from player position+2 to player position+12
		for(int j=0; j<11; j++)
		{
			temp_amount = 0;
			
			if(square >= 40)
				square -= 40;

			// no need to check the rent if player does not own it 
			if((UICurrentGameState.Squares[square].owner == FunctionResult.player))
			{					
				temp_amount = AI_Rent_If_Stepped_On(&UICurrentGameState, square, properties_owned);
			}

			if( temp_amount > max_amount)
				max_amount = temp_amount;

			// increment square position
			square++;
		}

		total += max_amount;
	}

		
	// set result string to be able to print
	//Asprintf(resultStr, A_T("%*li"), 0, total);
	UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(total, DISPLAY_state.system, resultStr, FALSE);

}




/*****************************************************************************
	* Function:		void UDStats_CalculateMinExpense(ACHAR * resultStr)
	* Returns:		nothing
	* Parameters:	result string
	*							
	* Purpose:		Calculates the minimum possible costs for a token after
	*							a number of turns.
	*							
	****************************************************************************
	*/
void UDStats_CalculateMinExpense(ACHAR * resultStr)
{
	RULE_PropertySet    properties_owned;
	int	square, total, min_amount, temp_amount;
	BOOL	first_time = TRUE;

	// get the square that the player is located on
	square = UICurrentGameState.Players[FunctionResult.player].currentSquare;

	total = 0;
	square+=2; // first roll can be no smaller than 2
	if(square >= 40)
		square -= 40;

	// loop for the number of turns user picked
	for(int i=0; i<FunctionResult.turns; i++)
	{		
		min_amount = 0;
		first_time = TRUE;

		// find the highes rent from player position+2 to player position+12
		for(int j=0; j<11; j++)
		{	
			temp_amount = 0;  // if one of the six spots is free, then no expense and then
												// technically you pay 0;

			// no need to check the rent if player owns it or of none owns it
			if((UICurrentGameState.Squares[square].owner < RULE_BANK_PLAYER) &&
				(UICurrentGameState.Squares[square].owner != FunctionResult.player))
			{
				properties_owned = RULE_PropertySetOwnedByPlayer(&UICurrentGameState,
																		UICurrentGameState.Squares[square].owner);
				temp_amount = AI_Rent_If_Stepped_On(&UICurrentGameState, square, properties_owned);

				properties_owned = 0;
			}

			// if this is the first amount we calculate properly, assign it to min_amount
			if(first_time)
			{
				min_amount = temp_amount;
				first_time = FALSE;
			}

			// check to see which is the minimum
			if( temp_amount < min_amount)
				min_amount = temp_amount;

			// increment square position
			square++;
			if(square >= 40)
				square -= 40;
		}

		total += min_amount;
	}

	// set result string to be able to print
	//Asprintf(resultStr, A_T("%*li"), 0, total);
	UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(total, DISPLAY_state.system, resultStr, FALSE);
}




/*****************************************************************************
	* Function:		void UDStats_CalculateMinRevenue(ACHAR * resultStr)
	* Returns:		nothing
	* Parameters:	result string
	*							
	* Purpose:		Calculates the minimum possible income for a token after
	*							a number of turns.
	*							
	****************************************************************************
	*/
void UDStats_CalculateMinRevenue(ACHAR * resultStr)
{
	RULE_PropertySet    properties_owned;
	int	square, total, min_amount, temp_amount;
	int current_player = UICurrentGameState.CurrentPlayer;
	int next_player;
	int min_square[RULE_MAX_PLAYERS];
	int	max_square[RULE_MAX_PLAYERS];

	// set the variables
	for(int i=0; i<UICurrentGameState.NumberOfPlayers; i++)
	{
		min_square[i] = UICurrentGameState.Players[i].currentSquare;
		max_square[i] = 0;
	}

	// determine the properties owned by the player
	properties_owned = RULE_PropertySetOwnedByPlayer(&UICurrentGameState, (UCHAR)FunctionResult.player);

	
	// reset the total for the current player
	total = 0;		

	// loop for the number of turns user picked
	for(int turn=1; turn<=FunctionResult.turns; turn++)
	{			
		// determine the next player to have his turn
		next_player = current_player + turn;
		// if we reach the last player, loop back to player #1
		if(next_player >= UICurrentGameState.NumberOfPlayers)
			next_player = 0;
		// if the next player is the player making the calculation, skip this turn
		if(next_player == current_player)
			continue;

		min_amount = (int)99999999999;
		
		// determine if it is possible for this player to land on the chosen property
		min_square[next_player] += max_square[next_player] + 2;
		max_square[next_player] = min_square[next_player] + 12;

		// make corrections if we wrapped around the board
		if(min_square[next_player] >= 40)
				min_square[next_player] -= 40;
		if(max_square[next_player] >= 40)
				max_square[next_player] -= 40;

		
		square = min_square[next_player];

		// find the highes rent from player position+2 to player position+12
		for(int j=0; j<11; j++)
		{
			temp_amount = 0;
			
			if(square >= 40)
				square -= 40;

			// no need to check the rent if player does not own it 
			if((UICurrentGameState.Squares[square].owner == FunctionResult.player))
			{					
				temp_amount = AI_Rent_If_Stepped_On(&UICurrentGameState, square, properties_owned);
			}

			if( temp_amount < min_amount)
				min_amount = temp_amount;

			// increment square position
			square++;
		}

		total += min_amount;
	}

		
	// set result string to be able to print
	//Asprintf(resultStr, A_T("%*li"), 0, total);
	UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(total, DISPLAY_state.system, resultStr, FALSE);

}





/*****************************************************************************
	* Function:		void UDStats_CalculateCurrentIncome(ACHAR * resultStr)
	* Returns:		nothing
	* Parameters:	result string
	*							
	* Purpose:		Calculates the current income for a token .
	*							
	****************************************************************************
	*/
void UDStats_CalculateCurrentIncome(ACHAR * resultStr)
{
	float rent;

		// get Average Rent from Go to Go, counting developed monopolies with a cash_cow_multiplier
		// of 1.0 (cause I'm not sure what it's for)
		rent = AI_Average_Rent_Received(&UICurrentGameState, (UCHAR)FunctionResult.player, 0, TRUE, 1.0f,
											RULE_PropertySetOwnedByPlayer(&UICurrentGameState, (UCHAR)FunctionResult.player));

	// set result string to be able to print
	//Asprintf(resultStr, A_T("%10.2f"), rent);
	UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((int)rent, DISPLAY_state.system, resultStr, FALSE);

}



/*****************************************************************************
	* Function:		void UDStats_CalculatePotentialIncome(ACHAR * resultStr)
	* Returns:		nothing
	* Parameters:	result string
	*							
	* Purpose:		Calculates the potential income for a token .
	*							
	****************************************************************************
	*/
void UDStats_CalculatePotentialIncome(ACHAR * resultStr)
{
	long rent;

	rent = AI_Potential_Income(&UICurrentGameState, (UCHAR)FunctionResult.player);
	// set result string to be able to print
	//Asprintf(resultStr, A_T("%*li"), 0, rent);
	UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(rent,	DISPLAY_state.system, resultStr, FALSE);

}






/*****************************************************************************
	* Function:		void UDStats_DisplayCalculator(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the calculator.
	****************************************************************************
	*/
void UDStats_DisplayCalculator(void)
{
	// reinitialize the buttons so they have the proper values for the player status screen
	//UDStats_InitMiscButtons();
	//UDStats_InitCalcButtons();

	// display the calculator background
	LE_SEQNCR_Start( CalcBackgroundID, CalcBGPriority );

	// display the calculator textbox
	LE_SEQNCR_Start( CalcTextBoxID, CalcTBPriority );

	// display the number buttons
	UDStats_DisplayCalcButtons();

	// display the function buttons
	UDStats_DisplayFunctionButtons();

}






/*****************************************************************************
	* Function:		void UDStats_RemoveCalculator(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Removes the calculator.
	****************************************************************************
	*/
void UDStats_RemoveCalculator(void)
{
	// remove the calculator background
	LE_SEQNCR_Stop( CalcBackgroundID, CalcBGPriority );

	// remove the calculator textbox
	LE_SEQNCR_Stop( CalcTextBoxID, CalcTBPriority );

	// remove the number buttons or token list
	UDStats_RemoveCalcButtons();
	UDStats_RemoveTokenList();
																		
	// remove the function buttons
	UDStats_RemoveFunctionButtons();

	// remove the deed pop up window if necessary
	UDStats_RemovePopupDeedWindow();

	// remove any displays left by the function buttons
	UDStats_RemoveFunctionDisplays();
			
}







/*****************************************************************************
	* Function:		void UDStats_InitDeedInfoFloater(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Sets the Data objects that will print the deed info.
	****************************************************************************
	*/
void UDStats_InitDeedInfoFloater(void)
{
	// load the deed info floater object
	DeedInfoFloater.ColourID = LED_IFT( DAT_MAIN, TAB_deedinfo );
	DeedInfoFloater.OthersID = LE_GRAFIX_ObjectCreate(400, 235, LE_GRAFIX_ObjectTransparent);
	LE_GRAFIX_ColorArea(DeedInfoFloater.OthersID, 0, 0, 400, 235, LE_GRAFIX_MakeColorRef (0, 255, 0));
	DeedInfoFloater.Priority = FLOATER_Priority;
	DeedInfoFloater.Others2ID = LED_EI;
}





/*****************************************************************************
	* Function:		void UDStats_DisplayCalcButtons(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the calculator buttons.
	****************************************************************************
	*/
void UDStats_DisplayCalcButtons(void)
{
	int i;

	// display the number buttons
	for(i=0; i<11; i++)
	{
		LE_SEQNCR_Start( CalculatorButton[i].IdleID, CalcButtonPriority );
		CalculatorButton[i].State = ISTATBAR_BUTTON_Idle;
	}
	
}






/*****************************************************************************
	* Function:		void UDStats_RemoveCalcButtons(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Removes the calculator buttons.
	****************************************************************************
	*/
void UDStats_RemoveCalcButtons(void)
{
	int i;

	// remove the number buttons
	for(i=0; i<11; i++)
	{
		if(CalculatorButton[i].State == ISTATBAR_BUTTON_Idle)
			LE_SEQNCR_Stop( CalculatorButton[i].IdleID, CalcButtonPriority );
		else if(CalculatorButton[i].State == ISTATBAR_BUTTON_Press)
			LE_SEQNCR_Stop( CalculatorButton[i].PressID, CalcButtonPriority );

		CalculatorButton[i].State = ISTATBAR_BUTTON_Off;
	}

}





/*****************************************************************************
	* Function:		void UDStats_DisplayFunctionButtons(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the function buttons.
	****************************************************************************
	*/
void UDStats_DisplayFunctionButtons(void)
{
	int i;

	// display the number buttons
	for(i=0; i<8; i++)
	{
		LE_SEQNCR_Start( FunctionButton[i].IdleID, CalcButtonPriority );
		FunctionButton[i].State = ISTATBAR_BUTTON_Idle;
	}
	
}






/*****************************************************************************
	* Function:		void UDStats_RemoveFunctionButtons(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Removes the function buttons.
	****************************************************************************
	*/
void UDStats_RemoveFunctionButtons(void)
{
	int i;

	// remove the number buttons
	for(i=0; i<8; i++)
	{
		if(FunctionButton[i].State == ISTATBAR_BUTTON_Idle)
			LE_SEQNCR_Stop( FunctionButton[i].IdleID, CalcButtonPriority );
		else if(FunctionButton[i].State == ISTATBAR_BUTTON_Press)
			LE_SEQNCR_Stop( FunctionButton[i].PressID, CalcButtonPriority );

		FunctionButton[i].State = ISTATBAR_BUTTON_Off;
	}

}






/*****************************************************************************
	* Function:		void UDStats_ReadRollOverFile(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Reads the strings that will be displayed for mouse roll overs
	*							and stores them in memory for future game use.
	****************************************************************************
	*/
void UDStats_ReadRollOverFile(void)
{
	int i = 0;	
/*
	FILE*		file_ptr;										// handle to file
	char  	file_name[MAX_PATH] = "\0";
	

	// open the file
	_getcwd(file_name, MAX_PATH);
	strcat(file_name, "/rollover.eng");
	file_ptr = fopen(file_name, "r");

	// if couldn't open file
	if(file_ptr == NULL)
	{
		MessageBox(NULL, file_name, "COULD NOT OPEN...", MB_OK);
		return;
	}

	for(i=0; i<8; i++)
		Afgets( FunctionDescText[i], MAX_DESC_STRLEN, file_ptr );

		
	// close the file
	fclose(file_ptr);
	*/
	
	Astrcpy(FunctionDescText[0], LANG_GetTextMessage(TMN_CALCULATOR_ODDS));
	Astrcpy(FunctionDescText[1], LANG_GetTextMessage(TMN_CALCULATOR_NET_WORTH));
	Astrcpy(FunctionDescText[2], LANG_GetTextMessage(TMN_CALCULATOR_FUTURE_VALUE_TO_YOU));
	Astrcpy(FunctionDescText[3], LANG_GetTextMessage(TMN_CALCULATOR_FUTURE_VALUE_TO_OTHER));
	Astrcpy(FunctionDescText[4], LANG_GetTextMessage(TMN_CALCULATOR_MAXIMUM_EXPENSE));
	Astrcpy(FunctionDescText[5], LANG_GetTextMessage(TMN_CALCULATOR_TOTAL_CURRENT_INCOME));
	Astrcpy(FunctionDescText[6], LANG_GetTextMessage(TMN_CALCULATOR_MAXIMUM_INCOME));
	Astrcpy(FunctionDescText[7], LANG_GetTextMessage(TMN_CALCULATOR_TOTAL_POTENTIAL_INCOME));
	


}






/*****************************************************************************
	* Function:		void UDStats_ResetCalculator(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Resets the calculator buttons.
	****************************************************************************
	*/
void UDStats_ResetCalculator(void)
{
	// reset the function buttons
	UDStats_RemoveFunctionButtons();
	UDStats_DisplayFunctionButtons();

	// reset the calculator buttons
	UDStats_RemoveTokenList();														
	UDStats_RemoveCalcButtons();
	UDStats_DisplayCalcButtons();

	// reset the text displays
	UDStats_RemoveFunctionDisplays();

	// remove the popup window
	UDStats_RemovePopupDeedWindow();

	// reset the steps counter
	ButtonClickStep = ROLL_OVER;

	// remove futures and immunities display box if necessary
	UDStats_RemoveFuturesNImmunities();
}







/*****************************************************************************
	* Function:		void UDStats_DisplayPopupDeedWindow(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the pop up deed window for the function buttons.
	****************************************************************************
	*/
void UDStats_DisplayPopupDeedWindow(void)
{
	int iTemp, x, y, width, height;
	LE_DATA_DataId tempID;

	//PopUpID = LE_GRAFIX_ObjectCreate(400, 200, LE_GRAFIX_ObjectNotTransparent);
	LE_GRAFIX_ColorArea(PopUpID, 0, 0, 400, 200, LE_GRAFIX_MakeColorRef (0, 0, 0));
								
	for( iTemp = 0; iTemp < SQ_MAX_SQUARE_TYPES; iTemp++ )
  {// Calculate the current propery ID
    if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_PortfolioA )
    {// Property is visible
			
      if( (tempID = DISPLAY_propertyToOwnablePropertyConversion(iTemp)) == -1 )
        tempID = LED_EI;
					
      else
      {
				// Crappy coord calcs - get sequences with final graphics.
		    x = 10 + 57*(tempID%7);
            y = 10 + 45*(tempID/7);
                 				
        //if( UICurrentGameState.CurrentPlayer == UICurrentGameState.Squares[iTemp].owner )
		    if( UICurrentGameState.CurrentPlayer == UICurrentGameState.Squares[orderArray[iTemp].Index].owner )
            {
#if USA_VERSION
              tempID += LED_IFT( DAT_PAT, TAB_trprfp0000 );//LED_IFT( DAT_LANG2, CNK_prfc00 );
#else
              if(iLangId == NOTW_LANG_FRENCH)
                  tempID += LED_IFT( DAT_PAT, TAB_xrprfp0000 );
              else
                  tempID += LED_IFT( DAT_PAT, TAB_zrprfp0000 );
#endif
            }
            else
            {
#if USA_VERSION
              tempID += LED_IFT( DAT_PAT, TAB_trprfp0000 );//LED_IFT( DAT_LANG2, CNK_prlc00 );
#else
              if(iLangId == NOTW_LANG_FRENCH)
                  tempID += LED_IFT( DAT_PAT, TAB_xrprfp0000 );
              else
                  tempID += LED_IFT( DAT_PAT, TAB_zrprfp0000 );
#endif
            }

			// set remainder of order structure
			width = STATS_DEED_CardW; //LE_GRAFIX_AnyBitmapWidth (tempID) ;			// does not work for chunks  
			height = STATS_DEED_CardH;
			
			SetRect(&(DeedLocation[iTemp]), x, y, x+width, y+height);
	    }
    } else
    {// not visible.
      tempID = LED_EI;
    }

    if( tempID != LED_EI )
    {// Bring up new property in the popup window
			LE_GRAFIX_ShowTCB(PopUpID, x, y, tempID);      
    }

  }

	// display the popup window
    if(!IsPopUpIDOn)
    {
	    LE_SEQNCR_StartXY(PopUpID, POPUP_Priority, POP_UP_X, POP_UP_Y);
        IsPopUpIDOn = TRUE;
    }
}






/*****************************************************************************
	* Function:		void UDStats_RemovePopupDeedWindow(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays the pop up deed window for the function buttons.
	****************************************************************************
	*/
void UDStats_RemovePopupDeedWindow(void)
{
	if(IsPopUpIDOn)
	{
		LE_SEQNCR_Stop(PopUpID, POPUP_Priority);
		IsPopUpIDOn = FALSE;
	}

    if(IsCornerDeedOn)
    {
        LE_SEQNCR_Stop(lastDeedID, POPUP_Priority);
		IsCornerDeedOn = FALSE;	
    }
}





/*****************************************************************************
	* Function:		int UDStats_PrintStringJustified(ACHAR * string, int justification, 
																					int width)
	* Returns:		horizontal offset of string positon
	* Parameters:	pointer to string to print
	*							flag that determines if left or right justified
	*							width of object to print string in.
	*							
	* Purpose:		Displays the passed string in the data object with proper justification.
	****************************************************************************
	*/
int UDStats_PrintStringJustified(ACHAR * string, int justification, int width)
{
	int str_width = LE_FONTS_GetStringWidth(string);
	
	// if left justified
	if(justification == LEFT_JUSTIFIED)
	{
		return 0;
	}

	else if(justification == RIGHT_JUSTIFIED)
	{
		return (width - str_width);
	}

	return 0;
}



/*****************************************************************************
	* Function:		void UDStats_InitBankStatusScreen(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Init all info related to bank status screen.
	****************************************************************************
	*/
void UDStats_InitBankStatusScreen(void)
{
  int i, t, col=0, row=1;

  // create the extra background that will be transparent and used to print text
  BankScreenBG.extraID = LE_GRAFIX_ObjectCreate(BankScreenBG.Width, BankScreenBG.Height, 
                        LE_GRAFIX_ObjectTransparent);

  // load the player house and hotels bars
  for(i=0; i<RULE_MAX_PLAYERS; i++)
  {    
    HHPlayerBar[i].IsObjectOn = FALSE;
    HHPlayerBar[i].Priority = 55;
    // set top left coordinates of left most box
    SetRect(&(HHPlayerBar[i].HotSpot), 38, 300 + (i*22), 38+346, 320 + (i*22));
    
    // the width will represent the x coordinate of the second box
    HHPlayerBar[i].Width = 416;
  }

  // set the location of the bank deeds in sort by property as well as 
  // loading the tabs
  for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
  {
    // make sure property is ownable
    if((t = DISPLAY_propertyToOwnablePropertyConversion(i)) == -1 )
      continue;

    switch(i)
    {
    case SQ_READING_RR:         SetRect(&(BankDeeds[i].Location), 583,100,
                                  583+STATS_DEED_CardW,100+STATS_DEED_CardH); 
                                break;

    case SQ_ELECTRIC_COMPANY:   SetRect(&(BankDeeds[i].Location), 714,100,
                                  714+STATS_DEED_CardW,100+STATS_DEED_CardH); 
                                break;

    case SQ_PENNSYLVANIA_RR:    SetRect(&(BankDeeds[i].Location), 583,49,
                                  583+STATS_DEED_CardW,49+STATS_DEED_CardH); 
                                break;

    case SQ_BnO_RR:             SetRect(&(BankDeeds[i].Location), 648,100,
                                  648+STATS_DEED_CardW,100+STATS_DEED_CardH); 
                                break;

    case SQ_WATER_WORKS:        SetRect(&(BankDeeds[i].Location), 714,49,
                                  714+STATS_DEED_CardW,49+STATS_DEED_CardH); 
                                break;

    case SQ_SHORT_LINE_RR:      SetRect(&(BankDeeds[i].Location), 648,49,
                                  648+STATS_DEED_CardW,49+STATS_DEED_CardH); 
                                break;

    case SQ_PARK_PLACE:         row--;
                                SetRect(&(BankDeeds[i].Location), 38+(col*69),49+(row*51),
                                  38+(col*69)+STATS_DEED_CardW,49+(row*51)+STATS_DEED_CardH);
                                row--;
                                break;
                                
    default:                    SetRect(&(BankDeeds[i].Location), 38+(col*69),49+(row*51),
                                  38+(col*69)+STATS_DEED_CardW,49+(row*51)+STATS_DEED_CardH);
                                row--;
                                if(row < 0)
                                {
                                  col++;
                                  row = 2;
                                }
                                break;
    }

    // now load the tab
#if USA_VERSION
    BankDeeds[i].ID = LED_IFT( DAT_PAT, TAB_trprfp0000 ) + t;
#else
    if(iLangId == NOTW_LANG_FRENCH)
        BankDeeds[i].ID = LED_IFT( DAT_PAT, TAB_xrprfp0000 ) + t;
    else
        BankDeeds[i].ID = LED_IFT( DAT_PAT, TAB_zrprfp0000 ) + t;
#endif
    BankDeeds[i].Priority = 55;
    // the index member will serve as a flag to determine if it is on or off
    BankDeeds[i].Index = 0; /* tab not displayed*/
  }

  // load the tabs for the bank deed signs (sold and mortgaged)
  BankDeedSign[SOLD].ID = LED_IFT(DAT_LANG2, TAB_soldsign);
  BankDeedSign[MORTGAGED].ID = LED_IFT(DAT_LANG2, TAB_mortsign);
  BankDeedSign[0].Priority = BankDeedSign[1].Priority = 60;
  BankDeedSign[0].IsObjectOn = BankDeedSign[1].IsObjectOn = FALSE;
  BankDeedSign[0].HotSpot.left = (STATS_DEED_CardW - LE_GRAFIX_AnyBitmapWidth(BankDeedSign[SOLD].ID))/2;
  BankDeedSign[0].HotSpot.top = (STATS_DEED_CardH - LE_GRAFIX_AnyBitmapHeight(BankDeedSign[SOLD].ID))/2;
  BankDeedSign[1].HotSpot.left = BankDeedSign[0].HotSpot.left;
  BankDeedSign[1].HotSpot.top = BankDeedSign[0].HotSpot.top;

  // load the sequences for the arrow buttons used in the account history screen
  for(i=0; i<2; i++)
  {
    ArrowButton[i].IdleID = LED_IFT(DAT_MAIN, CNK_byahadni+(i*2));
    ArrowButton[i].PressID = LED_IFT(DAT_MAIN, CNK_byahadnp+(i*2));
    ArrowButton[i].State = ISTATBAR_BUTTON_Off;
    SetRect(&(ArrowButton[i].HotSpot), 745, 402-(i*119), 745+18, 402+26-(i*119));
  }
  ArrowButtonPriority = POPUP_Priority + 10;
 
}




/*****************************************************************************
	* Function:		void UDStats_DisplayBankStatusScreen(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Displays bank status screen.
	****************************************************************************
	*/
void UDStats_DisplayBankStatusScreen(void)
{
  // remove everything concerning the previous status screen
  if(fStatusScreen == PLAYER_SCREEN)
	  UDStats_RemovePlayerStatusScreen();
  else if(fStatusScreen == DEED_SCREEN)
	  UDStats_RemoveDeedStatusScreen();

  fStatusScreen = BANK_SCREEN;
}




/*****************************************************************************
	* Function:		void UDStats_RemoveBankStatusScreen(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Removes everything contained in the bank status screen .
	****************************************************************************
	*/
void UDStats_RemoveBankStatusScreen(void)
{
  // remove any displays left by the sort screens
  UDStats_RemoveSortBankResults(last_sort_result);
  
  // remove the background
  if(BankScreenBG.IsObjectOn)
  {
    LE_SEQNCR_Stop(BankScreenBG.ID, BankScreenBG.Priority);
    BankScreenBG.IsObjectOn = FALSE;
  }
    
}




/*****************************************************************************
	* Function:		void UDStats_SortBankByHouses(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Sort by houses and hotels.
	****************************************************************************
	*/
void UDStats_SortBankByHouses(void)
{
  int i, j, k, squarehouses=0, totalhouses=0, totalhotels=0;
  // cnt for the total of player and bank houses and hotels
  int playerTotalHouses = 0;
  int playerTotalHotels = 0;
  int bankTotalHouses = UICurrentGameState.GameOptions.maximumHouses;
  int bankTotalHotels = UICurrentGameState.GameOptions.maximumHotels;
  int housesPerRow = bankTotalHouses / 2;   // used in case one player has all houses
  int hotelsPerRow = bankTotalHotels / 2;
  int houseWidth = (HHPlayerBar[0].HotSpot.right - HHPlayerBar[0].HotSpot.left)/ housesPerRow;//LE_GRAFIX_AnyBitmapWidth (BankHouseID);
  int hotelWidth = LE_GRAFIX_AnyBitmapWidth (BankHotelID);
  int player;
  ACHAR   tempStr[500];

  // display the houses and hotels background
  if(BankScreenBG.IsObjectOn)
  {
    LE_SEQNCR_Stop(BankScreenBG.ID, BankScreenBG.Priority);
    BankScreenBG.IsObjectOn = FALSE;
  }

  // display main background
  BankScreenBG.ID = LED_IFT(DAT_MAIN, CNK_byhhbr00);
  LE_SEQNCR_Start(BankScreenBG.ID, BankScreenBG.Priority);
  BankScreenBG.IsObjectOn = TRUE;

  // clear extra background
  LE_GRAFIX_ColorArea(BankScreenBG.extraID, 0, 0, BankScreenBG.Width, BankScreenBG.Height, 
                      LE_GRAFIX_MakeColorRef (0, 255, 0));

  // now display the player information
  for(i=0; i<UICurrentGameState.NumberOfPlayers; i++)
  {
    // check to see if bar already displayed
    if(HHPlayerBar[i].IsObjectOn)
      continue;

    // choose the proper player
    player = UICurrentGameState.Players[i].colour;

    HHPlayerBar[i].ID = LED_IFT(DAT_MAIN, CNK_bnhh00+player);
    LE_SEQNCR_Start(HHPlayerBar[i].ID, HHPlayerBar[i].Priority);
    HHPlayerBar[i].IsObjectOn = TRUE;

    // reset number of houses and hotels
    totalhouses=0;
    totalhotels=0;
    
    // ready the display of houses and hotels for current player (i)
    for(j=0; j<SQ_MAX_SQUARE_TYPES; j++)
    {
      // make sure property is ownable
      if(DISPLAY_propertyToOwnablePropertyConversion(j) == -1 )
        continue;

      // make sure player owns property
      if(UICurrentGameState.Squares[j].owner != i)
        continue;

      // check to see if there are any houses on property
      squarehouses = UICurrentGameState.Squares[j].houses; 
      if(squarehouses > 0)
      {
        // check to see if the square has a hotel
        if(squarehouses == UICurrentGameState.GameOptions.housesPerHotel)
          totalhotels++;
          
        else
        {
          // we have houses on the square
          for(k=0; k<squarehouses; k++)
            totalhouses++;           
        }
      } // end if houses > 0

    } // end loop square types

    // display hotels for current player i
    for(j=0; j<totalhotels; j++)
    {
      LE_SEQNCR_StartXY(BankHotelID, HHPlayerBar[player].Priority + 1,
                          HHPlayerBar[player].Width+(j*hotelWidth)+2, HHPlayerBar[player].HotSpot.top+2);
      // set extra variable in structure to show that there is at 
      // least one hotel to remove later
      HHPlayerBar[0].var1 = 1;

    }

    // display houses for current player i
    for(j=0; j<totalhouses; j++)
    {
      if(j >= housesPerRow)
      {
        LE_SEQNCR_StartXY(BankHouseID, HHPlayerBar[player].Priority + 1,
            HHPlayerBar[player].HotSpot.left+((j-housesPerRow)*houseWidth)+12, 
            HHPlayerBar[player].HotSpot.top+6);
      }
      else
      {
        LE_SEQNCR_StartXY(BankHouseID, HHPlayerBar[player].Priority + 1,
            HHPlayerBar[player].HotSpot.left+(j*houseWidth)+2,
            HHPlayerBar[player].HotSpot.top+1);
      }
      // set extra variable in structure to show that there is at 
      // least one house to remove later
      HHPlayerBar[0].var2 = 1;

    }

    // increment the player grand total of houses and hotels
    playerTotalHouses += totalhouses;
    playerTotalHotels += totalhotels;

  } // end player loop
  
  // determine the number of houses and hotels left to buy
  bankTotalHouses -= playerTotalHouses;
  bankTotalHotels -= playerTotalHotels;

  LE_FONTS_GetSettings(3);
  int tempx=0;

	// print the titles
	LE_FONTS_GetSettings(2);
    // print the bankTotalHouses 
    Asprintf(tempStr, A_T("%s:  %i"), LANG_GetTextMessage(BANK_HOUSES_REMAINING), bankTotalHouses);
    tempx = 274/2 - LE_FONTS_GetStringWidth(tempStr)/2;
    LE_FONTS_Print(BankScreenBG.extraID, 20+tempx, 23, LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);
	//LE_FONTS_Print(BankScreenBG.extraID, 255, 23, LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);

    // print the bankTotalHotels 
    Asprintf(tempStr, A_T("%s:  %i"), LANG_GetTextMessage(BANK_HOTELS_REMAINING), bankTotalHotels);
    tempx = 274/2 - LE_FONTS_GetStringWidth(tempStr)/2;
	LE_FONTS_Print(BankScreenBG.extraID, 491+tempx, 23, LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);
	//LE_FONTS_Print(BankScreenBG.extraID, 722, 23, LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);

    // print the playerTotalHouses 
    Asprintf(tempStr, A_T("%s:  %i"), LANG_GetTextMessage(HOUSES_PURCHASED_BY_PLAYERS), playerTotalHouses);
    tempx = 367/2 - LE_FONTS_GetStringWidth(tempStr)/2;
	LE_FONTS_Print(BankScreenBG.extraID, 20+tempx, 56, LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);
	//LE_FONTS_Print(BankScreenBG.extraID, 315, 56, LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);

    // print the playerTotalHotels
    Asprintf(tempStr, A_T("%s:  %i"), LANG_GetTextMessage(HOTELS_PURCHASED_BY_PLAYERS), playerTotalHotels);
    tempx = 367/2 - LE_FONTS_GetStringWidth(tempStr)/2;
	LE_FONTS_Print(BankScreenBG.extraID, 398+tempx, 56, LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);
	//LE_FONTS_Print(BankScreenBG.extraID, 698, 56, LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);

    /*
    
	// print the titles
	tempx = 274/2 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(BANK_HOUSES_REMAINING))/2;
	LE_FONTS_Print(BankScreenBG.extraID, 20+tempx, 23, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(BANK_HOUSES_REMAINING));
	
	tempx = 274/2 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(BANK_HOTELS_REMAINING))/2;
	LE_FONTS_Print(BankScreenBG.extraID, 491+tempx, 23, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(BANK_HOTELS_REMAINING));
	
	tempx = 367/2 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(HOUSES_PURCHASED_BY_PLAYERS))/2;
	LE_FONTS_Print(BankScreenBG.extraID, 20+tempx, 56, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(HOUSES_PURCHASED_BY_PLAYERS));
	
	tempx = 367/2 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(HOTELS_PURCHASED_BY_PLAYERS))/2;
	LE_FONTS_Print(BankScreenBG.extraID, 398+tempx, 56, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(HOTELS_PURCHASED_BY_PLAYERS));


	LE_FONTS_GetSettings(2);
  // print the bankTotalHouses 
  Asprintf(tempStr, A_T("%i"), bankTotalHouses);
	LE_FONTS_Print(BankScreenBG.extraID, 255, 23, LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);

  // print the bankTotalHotels 
  Asprintf(tempStr, A_T("%i"), bankTotalHotels);
	LE_FONTS_Print(BankScreenBG.extraID, 722, 23, LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);

  // print the playerTotalHouses 
  Asprintf(tempStr, A_T("%i"), playerTotalHouses);
	LE_FONTS_Print(BankScreenBG.extraID, 315, 56, LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);

  // print the playerTotalHotels
  Asprintf(tempStr, A_T("%i"), playerTotalHotels);
	LE_FONTS_Print(BankScreenBG.extraID, 698, 56, LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);
*/
  // display the extra background
  LE_SEQNCR_StartXY(BankScreenBG.extraID, BankScreenBG.Priority + 1,
                    BankScreenBG.HotSpot.left, BankScreenBG.HotSpot.top);


	// reset the fonts
	LE_FONTS_GetSettings(0);

}




/*****************************************************************************
	* Function:		void UDStats_SortBankByProperties(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Sort by properties.
	****************************************************************************
	*/
void UDStats_SortBankByProperties(void)
{
  // remove any previouse display
  if(BankScreenBG.IsObjectOn)
  {
    LE_SEQNCR_Stop(BankScreenBG.ID, BankScreenBG.Priority);
    BankScreenBG.IsObjectOn = FALSE;
  }

  // display sort by properties background
  BankScreenBG.ID = LED_IFT(DAT_MAIN, CNK_byprbr00);
  LE_SEQNCR_Start(BankScreenBG.ID, BankScreenBG.Priority);
  BankScreenBG.IsObjectOn = TRUE;

   // clear extra background
  LE_GRAFIX_ColorArea(BankScreenBG.extraID, 0, 0, BankScreenBG.Width, BankScreenBG.Height, 
                      LE_GRAFIX_MakeColorRef (0, 255, 0));


  LE_FONTS_GetSettings(3);
  int tempx=0;

  // print the titles
	tempx = 734/2 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(BANK_PROPERTIES))/2;
	LE_FONTS_Print(BankScreenBG.extraID, 26+tempx, 27, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(BANK_PROPERTIES));

	// display the extra background
  LE_SEQNCR_StartXY(BankScreenBG.extraID, BankScreenBG.Priority + 1,
                    BankScreenBG.HotSpot.left, BankScreenBG.HotSpot.top);

  // display the bank properties
  UDStats_DisplayBankDeeds();

  	// reset the fonts
	LE_FONTS_GetSettings(0);

}





/*****************************************************************************
	* Function:		void UDStats_SortBankByLiabilities(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Sort by liabilities.
	****************************************************************************
	*/
void UDStats_SortBankByLiabilities(void)
{
  ACHAR tempStr[20];

  // remove any previous backgrounds
  if(BankScreenBG.IsObjectOn)
  {
    LE_SEQNCR_Stop(BankScreenBG.ID, BankScreenBG.Priority);
    BankScreenBG.IsObjectOn = FALSE;
  }
  
  // display main background
  BankScreenBG.ID = LED_IFT(DAT_MAIN, CNK_bylibr00);
  LE_SEQNCR_Start(BankScreenBG.ID, BankScreenBG.Priority);
  BankScreenBG.IsObjectOn = TRUE;

  // clear extra background
  LE_GRAFIX_ColorArea(BankScreenBG.extraID, 0, 0, BankScreenBG.Width, BankScreenBG.Height, 
                      LE_GRAFIX_MakeColorRef (0, 255, 0));

	  LE_FONTS_GetSettings(3);
  int tempx=0;
  int tempHeight = LE_FONTS_GetStringHeight(A_T("TEST"));

  // print the titles
	tempx = 250/2 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(BANK_LIABILITIES))/2;
	LE_FONTS_Print(BankScreenBG.extraID, 271+tempx, 20, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(BANK_LIABILITIES));
	
	LE_FONTS_Print(BankScreenBG.extraID, 38, 55, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(DIVIDENDS_PAID));
		
#if USA_VERSION
	//LE_FONTS_Print(BankScreenBG.extraID, 35, 85, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(TMN_BANK_PAYS_YOU_DIVIDEND_OF_50));
	UDStats_PrintString(BankScreenBG.extraID, LANG_GetTextMessage(TMN_BANK_PAYS_YOU_DIVIDEND_OF_50),
            35, 85, 320, tempHeight*2+85);
#else
  unsigned int i;
  ACHAR tempStr1[256];
  RULE_ActionArgumentsRecord msg;

  msg.numberA = TMN_BANK_PAYS_YOU_DIVIDEND_OF_50;
  UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(50, DISPLAY_state.system, msg.stringA, TRUE);
  for (i = 0; i < Astrlen(msg.stringA); i++)
    if (msg.stringA[i] == A_T(' '))
      msg.stringA[i] = A_T('_');
  FormatErrorNotification(&msg, tempStr1);
  UDStats_PrintString(BankScreenBG.extraID, tempStr1, 35, 85, 320, tempHeight*2+85);
#endif

	LE_FONTS_Print(BankScreenBG.extraID, 38, 139, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(BANK_ERRORS));
	
#if USA_VERSION
	//LE_FONTS_Print(BankScreenBG.extraID, 35, 170, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(TMN_BANK_ERROR_IN_YOUR_FAVOR_COLLECT_200));
    UDStats_PrintString(BankScreenBG.extraID, LANG_GetTextMessage(TMN_BANK_ERROR_IN_YOUR_FAVOR_COLLECT_200),
            35, 170, 320, tempHeight*2+170);
#else
  msg.numberA = TMN_BANK_ERROR_IN_YOUR_FAVOR_COLLECT_200;
  UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(200, DISPLAY_state.system, msg.stringA, TRUE);
  for (i = 0; i < Astrlen(msg.stringA); i++)
    if (msg.stringA[i] == A_T(' '))
      msg.stringA[i] = A_T('_');
  FormatErrorNotification(&msg, tempStr1);
  UDStats_PrintString(BankScreenBG.extraID, tempStr1, 35, 170, 320, tempHeight*2+170);
#endif

  // set the font size
  LE_FONTS_GetSettings(2);

  // print the total amount that the bank as paid in $50 cards 
  //Asprintf(tempStr, A_T("%15.2f"), (g_Card50Counter * 50.0f));
  // the 50 and 200 values have to be converted later
  int tempCardValue;
  //ACHAR wsztempCardValue[20];
  //char sztempCardValue[20];
  //UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(50, DISPLAY_state.system, wsztempCardValue, FALSE);
  //wcstombs( sztempCardValue, wsztempCardValue, Astrlen(wsztempCardValue));
  tempCardValue = 50;//atoi(sztempCardValue);
  UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((int)(g_Card50Counter * tempCardValue), DISPLAY_state.system, tempStr, FALSE);
	LE_FONTS_Print(BankScreenBG.extraID, 560, 77, LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);

  // print the total amount that the bank as paid in $200 cards
  //Asprintf(tempStr, A_T("%15.2f"), (g_Card200Counter * 200.0f));
  //UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(200, DISPLAY_state.system, wsztempCardValue, FALSE);
  //wcstombs( sztempCardValue, wsztempCardValue, Astrlen(wsztempCardValue));
  tempCardValue = 200;//atoi(sztempCardValue);
	UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((int)(g_Card200Counter * tempCardValue), DISPLAY_state.system, tempStr, FALSE);
	LE_FONTS_Print(BankScreenBG.extraID, 560, 155, LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);

    // display the cards for the background
#if USA_VERSION
    if(!g_LiabCardsOn)
    {
        LE_SEQNCR_StartCXYSR (LED_IFT(DAT_LANG2, TAB_cycad05), BankScreenBG.Priority + 1,
                LED_EI, 0, FALSE,  365, 275, 0.3f, 0);
        LE_SEQNCR_StartCXYSR (LED_IFT(DAT_LANG2, TAB_cyyo07), BankScreenBG.Priority + 1,
                LED_EI, 0, FALSE,  365, 358, 0.3f, 0);
        g_LiabCardsOn = TRUE;
    }
#endif
  // display the extra background
  LE_SEQNCR_StartXY(BankScreenBG.extraID, BankScreenBG.Priority + 1,
                    BankScreenBG.HotSpot.left, BankScreenBG.HotSpot.top);

	// reset the fonts
	LE_FONTS_GetSettings(0);

}






/*****************************************************************************
	* Function:		void UDStats_SortBankByHistory(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Sort by Account history.
	****************************************************************************
	*/
void UDStats_SortBankByHistory(void)
{
  int i;

  // remove any previous backgrounds
  if(BankScreenBG.IsObjectOn)
  {
    LE_SEQNCR_Stop(BankScreenBG.ID, BankScreenBG.Priority);
    BankScreenBG.IsObjectOn = FALSE;
  }
  
  // display main background
  BankScreenBG.ID = LED_IFT(DAT_MAIN, CNK_byahbr00);
  LE_SEQNCR_Start(BankScreenBG.ID, BankScreenBG.Priority);
  BankScreenBG.IsObjectOn = TRUE;

  // clear extra background
  LE_GRAFIX_ColorArea(BankScreenBG.extraID, 0, 0, BankScreenBG.Width, BankScreenBG.Height, 
                      LE_GRAFIX_MakeColorRef (0, 255, 0));

  // display the extra background
  LE_SEQNCR_StartXY(BankScreenBG.extraID, BankScreenBG.Priority + 1,
                    BankScreenBG.HotSpot.left, BankScreenBG.HotSpot.top);
  
  for(i=0; i<2; i++)
  {
    SetRect(&(ArrowButton[i].HotSpot), 745, 402-(i*119), 745+18, 402+26-(i*119));
    // display the scroll buttons
    LE_SEQNCR_Start(ArrowButton[i].IdleID, ArrowButtonPriority);
    ArrowButton[i].State = ISTATBAR_BUTTON_Idle;
  }

    LE_FONTS_GetSettings(3);
  int tempx=0;

  // print the titles
	tempx = 189/2 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(ACCOUNT_HISTORY))/2;
	LE_FONTS_Print(BankScreenBG.extraID, 39+tempx, 27, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(ACCOUNT_HISTORY));
	
	tempx = 197/2 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(PLAYER))/2;
	LE_FONTS_Print(BankScreenBG.extraID, 27+tempx, 59, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(PLAYER));
	
	tempx = 58/2 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(TURN))/2;
	LE_FONTS_Print(BankScreenBG.extraID, 228+tempx, 59, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(TURN));
	
	tempx = 442/2 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(TRANSACTIONS))/2;
	LE_FONTS_Print(BankScreenBG.extraID, 290+tempx, 59, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(TRANSACTIONS));

  // set the font size
  LE_FONTS_GetSettings(2);

  // offset the row so that our call to scroll does not actually scroll
  history_row -= LE_FONTS_GetStringHeight(A_T("Test"));
  
  // reset the font size
  LE_FONTS_GetSettings(0);

  // call the scroll function to display the bank history
  UDStats_ScrollBankHistory(1);

}






/*****************************************************************************
	* Function:		void UDStats_RemoveSortBankResults(int result)
	* Returns:		nothing
	* Parameters:	integer representing the sort that was last displayed on
  *             the bank status screen
	*							
	* Purpose:		Remove the displays left by the last bank sort.
	****************************************************************************
	*/
void UDStats_RemoveSortBankResults(int result)
{
  int i;

  // clear the extra id background
  if(BankScreenBG.IsObjectOn)// && result != 1)
    LE_SEQNCR_Stop(BankScreenBG.extraID, BankScreenBG.Priority + 1);
    
  switch(result)
  {
  case 0:   // houses and hotel sort
            // remove the player house and hotels bars
            for(i=0; i<RULE_MAX_PLAYERS; i++)
            {
              if(HHPlayerBar[i].IsObjectOn)
              {
                LE_SEQNCR_Stop(HHPlayerBar[i].ID, HHPlayerBar[i].Priority);
                HHPlayerBar[i].IsObjectOn = FALSE;        
              }
            }
            // remove all hotels and houses in one shot
            if(HHPlayerBar[0].var1)
            {
              LE_SEQNCR_Stop(BankHotelID, HHPlayerBar[0].Priority+1);
              HHPlayerBar[0].var1 = 0;        
            }
            if(HHPlayerBar[0].var2)
            {
              LE_SEQNCR_Stop(BankHouseID, HHPlayerBar[0].Priority+1);
              HHPlayerBar[0].var2 = 0;        
            }
  
            //break;

  case 1:   // properties sort
            UDStats_RemoveBankDeeds();
            //break;

  case 2:   // liabilities sort
#if USA_VERSION
            if(g_LiabCardsOn)
            {
                LE_SEQNCR_Stop (LED_IFT(DAT_LANG2, TAB_cycad05), BankScreenBG.Priority + 1);
                LE_SEQNCR_Stop (LED_IFT(DAT_LANG2, TAB_cyyo07), BankScreenBG.Priority + 1);
                g_LiabCardsOn = FALSE;
            }
#endif
            //break;

  case 3:   // account history sort
            // remove the arrows
            for(i=0; i<2; i++)
            {
              if(ArrowButton[i].State == ISTATBAR_BUTTON_Idle)
                LE_SEQNCR_Stop(ArrowButton[i].IdleID, ArrowButtonPriority);
              else if(ArrowButton[i].State == ISTATBAR_BUTTON_Press)
                LE_SEQNCR_Stop(ArrowButton[i].PressID, ArrowButtonPriority);

              ArrowButton[i].State = ISTATBAR_BUTTON_Off;
            }
            //break;
  }

}






/*****************************************************************************
	* Function:		void UDStats_DisplayBankDeeds(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Display all deeds in the bank status screen.
	****************************************************************************
	*/
void UDStats_DisplayBankDeeds(void)
{

  register int i;
  LE_DATA_DataId tempID;
  	
	RULE_PropertySet build_set = DISPLAY_PlaceBuildingLegalSquares( DISPLAY_state.IBarCurrentPlayer );
	RULE_PropertySet sell_set = DISPLAY_SellBuildingLegalSquares( DISPLAY_state.IBarCurrentPlayer );

  int offsetx = BankScreenBG.HotSpot.left;
  int offsety = BankScreenBG.HotSpot.top;
	
	// Update the Property Display
  for( i = 0; i < SQ_MAX_SQUARE_TYPES; i++ )
  {// Calculate the current propery ID
    	
    if( (tempID = DISPLAY_propertyToOwnablePropertyConversion( i )) == -1 )
      continue;
				
    else
    {
      LE_SEQNCR_StartXY(BankDeeds[i].ID, BankDeeds[i].Priority, 
              offsetx+BankDeeds[i].Location.left, offsety+BankDeeds[i].Location.top);
      BankDeeds[i].Index = 1; /* tab displayed*/
      
                              
      // check to see if the property has been mortgaged
      if(UICurrentGameState.Squares[i].mortgaged)
      {
        // display the mortgaged sign accordingly
        LE_SEQNCR_StartXY(BankDeedSign[MORTGAGED].ID, BankDeedSign[MORTGAGED].Priority, 
              offsetx+BankDeeds[i].Location.left+BankDeedSign[MORTGAGED].HotSpot.left,
              offsety+BankDeeds[i].Location.top + BankDeedSign[MORTGAGED].HotSpot.top);
        BankDeedSign[MORTGAGED].IsObjectOn = TRUE;
      }
      // check to see if the property has been sold
      else if(UICurrentGameState.Squares[i].owner < RULE_MAX_PLAYERS)
      {
        // display the sold sign accordingly
        LE_SEQNCR_StartXY(BankDeedSign[SOLD].ID, BankDeedSign[SOLD].Priority, 
              offsetx+BankDeeds[i].Location.left+BankDeedSign[SOLD].HotSpot.left,
              offsety+BankDeeds[i].Location.top+BankDeedSign[SOLD].HotSpot.top);
        BankDeedSign[SOLD].IsObjectOn = TRUE;
      }
      
                              /*
			if((DISPLAY_state.IBarCurrentState == IBAR_STATE_Mortgage) ||
				 (DISPLAY_state.IBarCurrentState == IBAR_STATE_UnMortgage) ||
				 (DISPLAY_state.IBarCurrentState == IBAR_STATE_Build) ||
				 (DISPLAY_state.IBarCurrentState == IBAR_STATE_Sell))
			{
				if((SlotIsALocalHumanPlayer [DISPLAY_state.IBarCurrentPlayer]) &&
				(DISPLAY_state.IBarCurrentPlayer == UICurrentGameState.Squares[orderArray[i].Index].owner))
				{
					switch(DISPLAY_state.IBarCurrentState)
					{
					case IBAR_STATE_Mortgage:		if( UDIBAR_testMortgagable( orderArray[i].Index ) )
																				tempID += LED_IFT(DAT_PAT, TAB_trprfp0000);
																			else
																				continue;
																			break;

					case IBAR_STATE_UnMortgage:	if(UICurrentGameState.Squares[orderArray[i].Index].mortgaged)
																				tempID += LED_IFT(DAT_PAT, TAB_trprfm0000);
																			else
																				continue;
																			break;

					case IBAR_STATE_Build:			if(RULE_PropertyToBitSet(orderArray[i].Index) & build_set)
																				tempID += LED_IFT(DAT_PAT, TAB_trprfp0000);
																			else
																				continue;
																			break;

					case IBAR_STATE_Sell:				if(RULE_PropertyToBitSet(orderArray[i].Index) & sell_set)
																				tempID += LED_IFT(DAT_PAT, TAB_trprfp0000);
																			else
																				continue;
																			break;

					default:										tempID = LED_EI;	break;
					}
				}
				else
				{
					tempID = LED_EI;
					continue;
				}
			}
			else
			{
				if(UICurrentGameState.Squares[i].mortgaged)
					tempID += LED_IFT(DAT_PAT, TAB_trprfm0000);
				else
					tempID += LED_IFT( DAT_PAT, TAB_trprfp0000 );
			}
		*/
    } // end else
			
  }// end for loop

}




/*****************************************************************************
	* Function:		void UDStats_RemoveBankDeeds(void)
	* Returns:		nothing
	* Parameters:	nothing
	*							
	* Purpose:		Remove all deeds in the bank status screen.
	****************************************************************************
	*/
void UDStats_RemoveBankDeeds(void)
{
  int i;

  for( i = 0; i < SQ_MAX_SQUARE_TYPES; i++ )
  {// Calculate the current propery ID
    	
    if(DISPLAY_propertyToOwnablePropertyConversion( i ) == -1 )
      continue;

    if(BankDeeds[i].Index == 1) /* is tab displayed*/
    {
      LE_SEQNCR_Stop(BankDeeds[i].ID, BankDeeds[i].Priority);
      BankDeeds[i].Index = 0; /* tab not displayed*/
    }
  }

  // remove the signs, if any
  for( i = 0; i < 2; i++ )
  {    	
    if(BankDeedSign[i].IsObjectOn)
    {
      LE_SEQNCR_Stop(BankDeedSign[i].ID, BankDeedSign[i].Priority);
      BankDeedSign[i].IsObjectOn = FALSE;
    }
  }

}






/*****************************************************************************
	* Function:		int UDStats_PrintString(LE_DATA_DataId DestObject, ACHAR *string,
  *                                       int x, int y, int max_width, max_height)
	* Returns:		the height in pixels of the text just printed
	* Parameters:	object to print string in
  *             string to be printed in limited area with LE_FONTS_Print
  *             x offset to start printing
  *             y offset to start printing
  *             max width allowed for string (font dependant)
	*							
	* Purpose:		print string onto data object DestObject 
	****************************************************************************
	*/
int UDStats_PrintString(LE_DATA_DataId DestObject, ACHAR *string,int x, int y, int max_width, int max_height)
{	

	int MaxWidth = LE_FONTS_GetStringWidth(string);
	int MaxLength = Astrlen(string);	
	int size = 0;
	int SrcIndex = 0;
	int tempLength = 0;
	int lineCnt = 0;
	int tmpHeight = 0;
	ACHAR*  tmpString;

  // while there are still characters left in our string
	while((MaxLength > 0) && ((y+lineCnt*tmpHeight) < max_height))
	{
    // determine the length of the string
		size = sizeof(ACHAR) * (MaxLength+1);

		tmpString = (ACHAR*)malloc(/*sizeof(ACHAR) * (MaxLength+1)*/size);
		ZeroMemory(tmpString, size);

		// get the characters that will fit from X to X+max_width
		tempLength = UDStats_GetCharsFromString(string, SrcIndex,	max_width, MaxLength, tmpString);

		// if tempLength is zero, that means we read a character not part of the regular alphabet
		// therefore, increment to the next character
		if(tempLength == 0)
		{
			tempLength = 1;			
		}
		// update our counters
		//tempLength = Astrlen(tmpString);
		MaxLength -= tempLength;
		SrcIndex += tempLength;
		
		// any characte other than line feed or higher than 176 in ascii codes
		if((string[(SrcIndex-1)] != 10))//	&& (string[(SrcIndex-1)]  < 176))
		{		
			tmpHeight = LE_FONTS_GetStringHeight(A_T("TEST"));

#if !USA_VERSION
      unsigned int i;
      for (i = 0; i < Astrlen(tmpString); i++)
        if (tmpString[i] == A_T('_'))
          tmpString[i] = A_T(' ');
#endif
			// print one line of the total string
			LE_FONTS_Print(DestObject, x, y+(lineCnt*tmpHeight), LE_GRAFIX_MakeColorRef (255, 255, 255), 
								tmpString);

			// move to the next line
			lineCnt++;
		}
		free(tmpString);
	}
	
  // return the total height of the string
  return (lineCnt*tmpHeight);
}





/*****************************************************************************
	* Function:		int UDStats_GetStringHeight(ACHAR *string, int max_width)
  *
	* Returns:		the height in pixels of the string 
	* Parameters:	string to be printed in limited area with LE_FONTS_Print
  *             max width allowed for string (font dependant)
	*							
	* Purpose:		determine height of string if it were to be printed onto a 
  *             data object of width max_width (basically the same as
  *             PrintString only it doesn't print)
  ****************************************************************************
	*/
int UDStats_GetStringHeight(ACHAR *string, int x, int y, int max_width, int max_height)
{	
	
	int MaxWidth = LE_FONTS_GetStringWidth(string);
	int MaxLength = Astrlen(string) - 1;	// -1 because we read the new line character into string
	int size = 0;
	int SrcIndex = 0;
	int tempLength = 0;
	int lineCnt = 0;
	int tmpHeight = 0;
	ACHAR*  tmpString;

  // while there are characters left to print
	while( MaxLength > 0  && ((y+lineCnt*tmpHeight) < max_height))
	{
		size = sizeof(ACHAR) * (MaxLength+1);

		tmpString = (ACHAR*)malloc(/*sizeof(ACHAR) * (MaxLength+1)*/size);
		ZeroMemory(tmpString, size);

		tempLength = UDStats_GetCharsFromString(string, SrcIndex,	max_width, MaxLength, tmpString);

		// if tempLength is zero, that means we read a character not part of the regular alphabet
		// therefore, increment to the next character
		if(tempLength == 0)
		{
			tempLength = 1;			
		}
		// update our counters
		//tempLength = Astrlen(tmpString);
		MaxLength -= tempLength;
		SrcIndex += tempLength;
		
		// any characte other than line feed or higher than 176 in ascii codes
		if((string[(SrcIndex-1)] != 10)	&& (string[(SrcIndex-1)]  < 176))
		{		
			tmpHeight = LE_FONTS_GetStringHeight(tmpString);
			// move to the next line
			lineCnt++;
		}
		free(tmpString);

	}
	
  // return total height of string
  return (lineCnt*tmpHeight);
}






/*****************************************************************************
	* Function:		void UDStats_ScrollBankHistory(int direction)
	* Returns:		nothing
	* Parameters:	integer representing the direction of the scroll (up or down)
	*							
	* Purpose:		Remove all deeds in the bank status screen.
	****************************************************************************
	*/
void UDStats_ScrollBankHistory(int direction)
{
  int i, width=0, tempIndex=0, scrollDY=0;
  int max=0, diff = 0;
  ACHAR tempStr[10];
  
  long           g_fileIndex = 0;           // offset of info in file
  static short   g_historyBottomIndex = 0;  // last record printed on screen
  static short   g_historyMax = 0;          // last record read into array
  static short   g_historyMin = 1;          // first record read into array
  static short   g_historyTopIndex=0;       // first record printed on screen
  
  // clear extra background
  LE_GRAFIX_ColorArea(BankScreenBG.extraID, 0, 80, BankScreenBG.Width, BankScreenBG.Height, 
                      LE_GRAFIX_MakeColorRef (0, 255, 0));
 
  LE_FONTS_GetSettings(2);

  
  // switch on the direction
  switch(direction)
  {
    
  case 0:       // case DOWN:     
                // always have 20 history transactions loaded, but
                // assume that no more than 10 will ever be showing
                // at one time.
                diff = g_historyMax - g_historyBottomIndex;
                if(diff < 5)
                {
                  // clear the accounthistorystruct
                  ZeroMemory(DisplayAccountInfo, sizeof(AccountHistoryStruct) * 20);

                  // reload 20 transactions
                  // read the file acchist.txt and fill the structure members
                  FILE * fileptr = fopen( "acchist.txt", "rb" );
                  if (fileptr != NULL)
                  {
                    // set file pointer
                    g_fileIndex = g_historyTopIndex * sizeof(AccountHistoryStruct);
                    fseek(fileptr, g_fileIndex, SEEK_SET);
                    for(i=0; i<20; i++)
                    {      
                      if((fread(&(DisplayAccountInfo[i]), sizeof(AccountHistoryStruct), 1, fileptr)) == 0)
                        break;
                    }
                    fclose( fileptr );
                    // update the two extreme counters
                    g_historyMax = g_historyTopIndex + i;
                    g_historyMin = g_historyTopIndex;
                  }
                }

                // there is no way that more than 10 records will be printed
                max = g_historyMax - g_historyTopIndex;
                if(max > 10)
                  max = 10;                 
                
                // offset the new line to print on
                history_row -= LE_FONTS_GetStringHeight(A_T("Test"));

                // the first visible print line is 80 pixels from the top of the 
                // background box
                scrollDY = 80+history_row;
                tempIndex = g_historyTopIndex - g_historyMin;

                for(i=0; i<max; i++)
                {
                  // print the player name 
                  LE_FONTS_Print(BankScreenBG.extraID, 30, scrollDY, LE_GRAFIX_MakeColorRef (250, 250, 250), 
                    UICurrentGameState.Players[DisplayAccountInfo[tempIndex+i].player_number].name);
  
                  // print the turn number 
                  Asprintf(tempStr, A_T("%i"), DisplayAccountInfo[tempIndex+i].turn_number);
	                LE_FONTS_Print(BankScreenBG.extraID, 230, scrollDY, 
                                  LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);

                  // print the turn description 
                  width = UDStats_PrintString(BankScreenBG.extraID, DisplayAccountInfo[tempIndex+i].turn_description,
                              290, scrollDY, 442, BankScreenBG.Height);
                
                  scrollDY += width;

                  // if row < 80 , we can remove the current record from memory, or
                  // at least increment the topindex
                  if(scrollDY < 80)
                  {
                    history_row = scrollDY-80;
                    g_historyTopIndex++;
                  }

                  // if we have printed past the last visible line, exit loop
                  if(scrollDY >= 200)
                    break;
                }
                
                // if we exited the loop normally, then disable the scroll down
                if((i+g_historyMin) >= g_historyMax)//if(i==max)
                {
                  if(ArrowButton[0].State == ISTATBAR_BUTTON_Idle)
                    LE_SEQNCR_Stop(ArrowButton[0].IdleID, ArrowButtonPriority);
                  else if(ArrowButton[0].State == ISTATBAR_BUTTON_Press)
                    LE_SEQNCR_Stop(ArrowButton[0].PressID, ArrowButtonPriority);
                  ArrowButton[0].State = ISTATBAR_BUTTON_Off;
                }
                
                
                break;

  case 1:       // case UP:     
                // check to see if this is a screen update and not a scroll
                // if it is a screen update, and we are we were showing the last records,
                // we need to reload the info
                if(!g_bIsScreenUpdated && (g_historyMax == g_historyBottomIndex))
                {
                  // set the condition to reload the info
                  g_historyMin = g_historyTopIndex;
                  if(g_historyMin == 0)
                    g_historyMin = 1;
                }

                // always have 20 history transactions loaded, but
                // assume that no more than 10 will ever be showing
                // at one time.
                diff = g_historyTopIndex - g_historyMin;
                if((diff < 5) && (g_historyMin != 0))
                {
                  // clear the accounthistorystruct
                  ZeroMemory(DisplayAccountInfo, sizeof(AccountHistoryStruct) * 20);

                  // reload 20 transactions
                  // read the file acchist.txt and fill the structure members
                  FILE * fileptr = fopen( "acchist.txt", "rb" );
                  if (fileptr != NULL)
                  {
                    // if there are less than 20 records from the top,
                    // set counters to begining of records
                    if((g_historyMin = g_historyBottomIndex-20) < 0)
                      g_historyMin = 0;
                    
                    g_fileIndex = g_historyMin * sizeof(AccountHistoryStruct);
                    fseek(fileptr, g_fileIndex, SEEK_SET);
                    for(i=0; i<20; i++)
                    {      
                      if((fread(&(DisplayAccountInfo[i]), sizeof(AccountHistoryStruct), 1, fileptr)) == 0)
                        break;
                    }
                    fclose( fileptr );
                    g_historyMax = g_historyMin + i;
                  }
                }

                max = g_historyMax - g_historyTopIndex;
                if(max > 10)
                  max = 10;
 
                // offset line to start printing
                history_row+= LE_FONTS_GetStringHeight(A_T("Test"));
                tempIndex = g_historyTopIndex - g_historyMin;

                // if first row start past the top of the box
                if(history_row >= 0)
                {                  
                  // decrement the first record printed counter
                  tempIndex--;
                  g_historyTopIndex--;
                  if(g_historyTopIndex == -1)
                  {
                    // if we were already at the top of the records, reset to
                    // zero and remove the arrow
                    g_historyTopIndex = tempIndex = 0;
                    
                    // if we exited the loop normally, then disable the scroll down
                    if(ArrowButton[1].State == ISTATBAR_BUTTON_Idle)
                      LE_SEQNCR_Stop(ArrowButton[1].IdleID, ArrowButtonPriority);
                    else if(ArrowButton[1].State == ISTATBAR_BUTTON_Press)
                      LE_SEQNCR_Stop(ArrowButton[1].PressID, ArrowButtonPriority);
                    ArrowButton[1].State = ISTATBAR_BUTTON_Off;                 
                  }
                  else
                  {
                    // else, offset the starting line by one the height of the new 
                    // record to start printing at
                    history_row -= UDStats_GetStringHeight(DisplayAccountInfo[tempIndex].turn_description,
						0,0,442,BankScreenBG.Height);
                  }
                }
                
                scrollDY = 80+history_row;

                for(i=0; i<max; i++)
                {
                  // print the player name 
                  LE_FONTS_Print(BankScreenBG.extraID, 30, scrollDY, LE_GRAFIX_MakeColorRef (250, 250, 250), 
                                UICurrentGameState.Players[DisplayAccountInfo[tempIndex+i].player_number].name);
  
                  // print the turn number 
                  Asprintf(tempStr, A_T("%i"), DisplayAccountInfo[tempIndex+i].turn_number);
	                LE_FONTS_Print(BankScreenBG.extraID, 230, scrollDY, 
                                  LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);

                  // print the turn description 
                  width = UDStats_PrintString(BankScreenBG.extraID, DisplayAccountInfo[tempIndex+i].turn_description,
                              290, scrollDY, 442, BankScreenBG.Height);
                
                  scrollDY += width;

                  // exit loop if printing past bottom of box
                  if(scrollDY >= 200)
                    break;
                }

                
                break;
  }


  // set the global current history index
  g_historyBottomIndex = g_historyTopIndex + i;

  // cover the bottom area and the top area of the extra background 
  // with the color green to erase
  // any text that may appear over the border of the box
  LE_GRAFIX_ColorArea(BankScreenBG.extraID, 0, 200,
        BankScreenBG.Width, BankScreenBG.Height, LE_GRAFIX_MakeColorRef (0, 255, 0));
  LE_GRAFIX_ColorArea(BankScreenBG.extraID, 0, 0,
        BankScreenBG.Width, 80, LE_GRAFIX_MakeColorRef (0, 255, 0));

  // reprint titles
    LE_FONTS_GetSettings(3);
  int tempx=0;

  // print the titles
	tempx = 189/2 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(ACCOUNT_HISTORY))/2;
	LE_FONTS_Print(BankScreenBG.extraID, 39+tempx, 27, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(ACCOUNT_HISTORY));
	
	tempx = 197/2 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(PLAYER))/2;
	LE_FONTS_Print(BankScreenBG.extraID, 27+tempx, 59, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(PLAYER));
	
	tempx = 58/2 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(TURN))/2;
	LE_FONTS_Print(BankScreenBG.extraID, 228+tempx, 59, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(TURN));
	
	tempx = 442/2 - LE_FONTS_GetStringWidth(LANG_GetTextMessage(TRANSACTIONS))/2;
	LE_FONTS_Print(BankScreenBG.extraID, 290+tempx, 59, LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(TRANSACTIONS));


  // redraw the extra background
  LE_SEQNCR_ForceRedraw (BankScreenBG.extraID, BankScreenBG.Priority + 1);

  // if the other arrow was disabled, re-enable it
  if(ArrowButton[!direction].State == ISTATBAR_BUTTON_Off)
    LE_SEQNCR_Start(ArrowButton[!direction].IdleID, ArrowButtonPriority);
  ArrowButton[!direction].State = ISTATBAR_BUTTON_Idle;

	// reset the fonts
	LE_FONTS_GetSettings(0);
}





/*****************************************************************************
	* Function:		void UDStats_DisplayFuturesNImmunities(int hitType, int index)
	* Returns:		nothing
	* Parameters:	
	*							
	* Purpose:		Display the information on the players futures or immunities.
	****************************************************************************
	*/
void UDStats_DisplayFuturesNImmunities(int hitType, int index)
{
  int player, i, j, linecnt=1, DY=15, c1=15, c2=150, c3=300;
  RULE_PropertySet tempset=0;
  ACHAR tempStr[100];
  g_propertyset = 0;
  g_fniIndex = 0;
  int tempIndex = 0;
  RULE_ActionArgumentsRecord msg;
  
  LE_FONTS_GetSettings(2);

	// prepare a window to display the info
	fniBox.ID = LE_GRAFIX_ObjectCreate(197, 223, LE_GRAFIX_ObjectTransparent);
	LE_GRAFIX_ColorArea(fniBox.ID, 0, 0, 197, 223, LE_GRAFIX_MakeColorRef (0, 255, 0));
	SetRect(&(fniBox.HotSpot), 601, 3, 601+197, 3+223);
	fniBox.Priority = POPUP_Priority;
  //PopUpID = LE_GRAFIX_ObjectCreate(400, 200, LE_GRAFIX_ObjectNotTransparent);
	//LE_GRAFIX_ColorArea(PopUpID, 0, 0, 400, 200, LE_GRAFIX_MakeColorRef (0, 0, 0));

  // print proper info in box depending on future or immunity
  switch(hitType)
  {
  case CHT_FUTURE_RENT:   
        player = Futures[index].var1;
        // print the title of the pop up box
        //Astrcpy(tempStr, A_T("FUTURES for "));
        
        msg.numberA = TMN_FUTURES_FOR;
        msg.numberC = player;
      
        FormatErrorNotification(&msg, tempStr);
        //Astrcpy(tempStr, LANG_GetTextMessage(TMN_FUTURES_FOR));
        //Astrcat(tempStr, UICurrentGameState.Players[player].name);
        LE_FONTS_Print(fniBox.ID, 98-(LE_FONTS_GetStringWidth(tempStr)/2), linecnt*DY-6, 
          LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr); 
        linecnt++;
        // print the column titles
        LE_FONTS_Print(fniBox.ID, c1, linecnt*DY, 
                  LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(TMN_PROPERTY)); 
        if(LE_FONTS_GetStringWidth(LANG_GetTextMessage(TMN_HITS_REMAINING)) > 80)
        {
            int tempStringHeight = LE_FONTS_GetStringHeight(A_T("TEST"));
            if(iLangId == 9)    // dannish
            {
                UDStats_PrintString(fniBox.ID, LANG_GetTextMessage(TMN_HITS_REMAINING),
                    110, linecnt*DY-5, 80, (linecnt*DY-5)+tempStringHeight*2);
            }
            else
            {
                UDStats_PrintString(fniBox.ID, LANG_GetTextMessage(TMN_HITS_REMAINING),
                    120, linecnt*DY-5, 80, (linecnt*DY-5)+tempStringHeight*2);
            }
        }
        else
        {
        
            LE_FONTS_Print(fniBox.ID, c2-(LE_FONTS_GetStringWidth(LANG_GetTextMessage(TMN_HITS_REMAINING))/2), linecnt*DY, 
                  LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(TMN_HITS_REMAINING)); 
        }
        //LE_FONTS_Print(fniBox.ID, c3, linecnt*DY, 
        //          LE_GRAFIX_MakeColorRef (250, 250, 250), A_T("From Player")); 
        linecnt+=2;

        // determine the properties that have a future for current player
        for(j=0; j<RULE_MAX_COUNT_HIT_SETS; j++)
        {
          if((UICurrentGameState.CountHits[j].toPlayer == (unsigned char)player) &&
              (UICurrentGameState.CountHits[j].hitType == (unsigned char)hitType))
          {
            // set the properties
            // there seems to be something not quite right with the setting
            // of the CountHits member (the same future is added in the array
            // twice, reducing the amount of free space very quickly)
            // so put in a check for the same property set
//            if(tempset & UICurrentGameState.CountHits[j].properties)
//            {            
              tempset=0;
              tempset |= UICurrentGameState.CountHits[j].properties;

              for(i=0; i < SQ_MAX_SQUARE_TYPES; i++ )
              {
                if(RULE_PropertyToBitSet(i) & tempset)
                {
                  Astrcpy(namesArray[tempIndex], RULE_SquarePredefinedInfo[i].squareName);//LANG_GetTextMessage(TMN_SQUARENAME_GO + i)/*PropertyNames[i]*/);
                  Asprintf(tempStr, A_T("%2i"), UICurrentGameState.CountHits[j].hitCount);
                  Astrcpy(hitsArray[tempIndex], tempStr);
                  Astrcpy(playerArray[tempIndex], UICurrentGameState.Players[UICurrentGameState.CountHits[j].fromPlayer].name);
                  tempIndex++;
                }
              }
              g_propertyset |= UICurrentGameState.CountHits[j].properties;
//            }
            tempset |= UICurrentGameState.CountHits[j].properties;

            /*LE_FONTS_Print(PopUpID, c1, linecnt*DY, 
                  LE_GRAFIX_MakeColorRef (250, 250, 250), PropertyNames[RULE_BitSetToProperty(UICurrentGameState.CountHits[j].properties)]);
            Asprintf(tempStr, A_T("%i"), UICurrentGameState.CountHits[j].hitCount);
            LE_FONTS_Print(PopUpID, c2, linecnt*DY, 
                  LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr);
            LE_FONTS_Print(PopUpID, c3, linecnt*DY, 
                  LE_GRAFIX_MakeColorRef (250, 250, 250), UICurrentGameState.Players[UICurrentGameState.CountHits[j].fromPlayer].name);
            linecnt++;*/
          }
        }
        break;
  case CHT_RENT_IMMUNITY: 
        player = Immunities[index].var1;
        // print the title of the pop up box
        
        msg.numberA = TMN_IMMUNITIES_FOR;
        msg.numberC = player;
      
        FormatErrorNotification(&msg, tempStr);
        //Astrcpy(tempStr, LANG_GetTextMessage(TMN_IMMUNITIES_FOR));
        //Astrcat(tempStr, UICurrentGameState.Players[player].name);
        LE_FONTS_Print(fniBox.ID, 98-(LE_FONTS_GetStringWidth(tempStr)/2), linecnt*DY-5, 
          LE_GRAFIX_MakeColorRef (250, 250, 250), tempStr); 
        linecnt++;
        // print the column titles
        LE_FONTS_Print(fniBox.ID, c1, linecnt*DY, 
                  LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(TMN_PROPERTY)); 
        if(LE_FONTS_GetStringWidth(LANG_GetTextMessage(TMN_HITS_REMAINING)) > 80)
        {
            int tempStringHeight = LE_FONTS_GetStringHeight(A_T("TEST"));
            if(iLangId == 9)    // dannish
            {
                UDStats_PrintString(fniBox.ID, LANG_GetTextMessage(TMN_HITS_REMAINING),
                    110, linecnt*DY-5, 80, (linecnt*DY-5)+tempStringHeight*2);
            }
            else
            {
                UDStats_PrintString(fniBox.ID, LANG_GetTextMessage(TMN_HITS_REMAINING),
                    120, linecnt*DY-5, 80, (linecnt*DY-5)+tempStringHeight*2);
            }
            //linecnt++;
        }
        else
        {
        
            LE_FONTS_Print(fniBox.ID, c2-(LE_FONTS_GetStringWidth(LANG_GetTextMessage(TMN_HITS_REMAINING))/2), linecnt*DY, 
                  LE_GRAFIX_MakeColorRef (250, 250, 250), LANG_GetTextMessage(TMN_HITS_REMAINING)); 
        }
        //LE_FONTS_Print(fniBox.ID, c3, linecnt*DY, 
        //          LE_GRAFIX_MakeColorRef (250, 250, 250), A_T("From Player")); 
        linecnt+=2;
        // determine the properties that have a future for current player
        for(j=0; j<RULE_MAX_COUNT_HIT_SETS; j++)
        {
          if((UICurrentGameState.CountHits[j].toPlayer == (unsigned char)player) &&
              (UICurrentGameState.CountHits[j].hitType == (unsigned char)hitType))
          {
            // set the properties
            // there seems to be something not quite right with the setting
            // of the CountHits member (the same future is added in the array
            // twice, reducing the amount of free space very quickly)
            // so put in a check for the same property set
//            if(tempset & UICurrentGameState.CountHits[j].properties)
//            {            
              tempset=0;
              tempset |= UICurrentGameState.CountHits[j].properties;

              for(i=0; i < SQ_MAX_SQUARE_TYPES; i++ )
              {
                if(RULE_PropertyToBitSet(i) & tempset)
                {
                  Astrcpy(namesArray[tempIndex], RULE_SquarePredefinedInfo[i].squareName);//LANG_GetTextMessage(TMN_SQUARENAME_GO + i)/*PropertyNames[i]*/);
                  Asprintf(tempStr, A_T("%2i"), UICurrentGameState.CountHits[j].hitCount);
                  Astrcpy(hitsArray[tempIndex], tempStr);
                  Astrcpy(playerArray[tempIndex], UICurrentGameState.Players[UICurrentGameState.CountHits[j].fromPlayer].name);
                  tempIndex++;                  
                }
              }
              g_propertyset |= UICurrentGameState.CountHits[j].properties;
//            }
            tempset |= UICurrentGameState.CountHits[j].properties;
          }
        }                          
        break;

  default:        return;
  }

  // at this point, we have all the properties that have either a future or immunity
  tempIndex = 0;                          
  for(j=0; j < SQ_MAX_SQUARE_TYPES; j++ )
  {		
	  // check to see if the current square in the loop is part of the properties in
    // the future or immunity set
    if(RULE_PropertyToBitSet(j) & g_propertyset)
    {
      LE_FONTS_Print(fniBox.ID, c1, linecnt*DY, 
          LE_GRAFIX_MakeColorRef (250, 250, 250), namesArray[tempIndex]);
      // erase the text if it is too long
      LE_GRAFIX_ColorArea(fniBox.ID, 120, linecnt*DY, 197, (linecnt+1)*DY, LE_GRAFIX_MakeColorRef (0, 255, 0));

	    LE_FONTS_Print(fniBox.ID, c2, linecnt*DY, 
          LE_GRAFIX_MakeColorRef (250, 250, 250), hitsArray[tempIndex]);
      //LE_FONTS_Print(fniBox.ID, c3, linecnt*DY, 
      //    LE_GRAFIX_MakeColorRef (250, 250, 250), playerArray[tempIndex]);
      tempIndex++;
      linecnt++;
    }
  }	

  for(i=0; i<2; i++)
  {
    SetRect(&(ArrowButton[i].HotSpot), fniBox.HotSpot.right-18-10, fniBox.HotSpot.bottom-26-12-(i*125),
					fniBox.HotSpot.right, fniBox.HotSpot.bottom-(i*127));
  }

  // display the scrolling arrows
  if(linecnt > 11)
  {
    LE_SEQNCR_StartXY(ArrowButton[DOWN_ARROW].IdleID, ArrowButtonPriority,
                      -(745 - ArrowButton[DOWN_ARROW].HotSpot.left), -(402 - ArrowButton[DOWN_ARROW].HotSpot.top));
    ArrowButton[DOWN_ARROW].State = ISTATBAR_BUTTON_Idle;
  }

	// display the popup window
	LE_GRAFIX_ShowTCB(fniBox.ID, 0, 0, LED_IFT(DAT_LANG2, TAB_tndisbrd));
	LE_SEQNCR_StartXY(fniBox.ID, fniBox.Priority+1,601,3);
	LE_SEQNCR_Start(LED_IFT(DAT_LANG2, CNK_tydisplr), fniBox.Priority);
	fniBox.IsObjectOn = TRUE;
  
}





/*****************************************************************************
	* Function:		void UDStats_RemoveFuturesNImmunities(void)
	* Returns:		nothing
	* Parameters:	
	*							
	* Purpose:		Remove the pop up window with the futures and immunities.
	****************************************************************************
	*/
void UDStats_RemoveFuturesNImmunities(void)
{
  if(fniBox.IsObjectOn)
  {
	  LE_SEQNCR_Stop(fniBox.ID, fniBox.Priority+1);
	  LE_SEQNCR_Stop(LED_IFT(DAT_LANG2, CNK_tydisplr), fniBox.Priority);
	  fniBox.IsObjectOn = FALSE;
    
	  // remove the arrow buttons
	  for(int i=0; i<2; i++)
	  {
		if(ArrowButton[i].State == ISTATBAR_BUTTON_Idle)
		  LE_SEQNCR_Stop(ArrowButton[i].IdleID, ArrowButtonPriority);
		else if(ArrowButton[i].State == ISTATBAR_BUTTON_Press)
		  LE_SEQNCR_Stop(ArrowButton[i].PressID, ArrowButtonPriority);

		ArrowButton[i].State = ISTATBAR_BUTTON_Off;
	  }
  }  
}





/*****************************************************************************
	* Function:		void UDStats_ScrollFuturesNImmunities(int direction)
	* Returns:		nothing
	* Parameters:	direction of scroll
	*							
	* Purpose:		scroll the text in the pop up window with the 
  *             futures and immunities.
	****************************************************************************
	*/
void UDStats_ScrollFuturesNImmunities(int direction)
{
  int linecnt = 4, j, DY=15, c1=15, c2=150, c3=300;
  int tempIndex = 0;

  LE_FONTS_GetSettings(2);

  if(direction == UP_ARROW)
  {
    g_fniIndex--;
    if(g_fniIndex <= 0)
      g_fniIndex = 0;
  }
  else
  {
    g_fniIndex++;
    if(g_fniIndex >= SQ_MAX_SQUARE_TYPES)
      g_fniIndex = SQ_MAX_SQUARE_TYPES-1;
  }

  tempIndex = 0;

  LE_GRAFIX_ColorArea(fniBox.ID, 0, linecnt*DY, 197, 223, LE_GRAFIX_MakeColorRef (0, 255, 0));

  for(j=0; j < SQ_MAX_SQUARE_TYPES; j++ )
  {		
	  // check to see if the current square in the loop is part of the properties in
    // the future or immunity set

    if(RULE_PropertyToBitSet(j) & g_propertyset)
    {
      if(tempIndex >= g_fniIndex)
      {
        LE_FONTS_Print(fniBox.ID, c1, linecnt*DY, 
            LE_GRAFIX_MakeColorRef (250, 250, 250), namesArray[tempIndex]);
        // erase the text if it is too long
        LE_GRAFIX_ColorArea(fniBox.ID, 120, linecnt*DY, 197, (linecnt+1)*DY, LE_GRAFIX_MakeColorRef (0, 255, 0));
	      LE_FONTS_Print(fniBox.ID, c2, linecnt*DY, 
            LE_GRAFIX_MakeColorRef (250, 250, 250), hitsArray[tempIndex]);
        //LE_FONTS_Print(fniBox.ID, c3, linecnt*DY, 
        //    LE_GRAFIX_MakeColorRef (250, 250, 250), playerArray[tempIndex]);
        linecnt++;
      }
      tempIndex++;      
		}
  }	
 
  if(g_fniIndex <= 0)
  {
    if(ArrowButton[direction].State == ISTATBAR_BUTTON_Idle)
      LE_SEQNCR_Stop(ArrowButton[direction].IdleID, ArrowButtonPriority);
    else if(ArrowButton[direction].State == ISTATBAR_BUTTON_Press)
      LE_SEQNCR_Stop(ArrowButton[direction].PressID, ArrowButtonPriority);

    ArrowButton[direction].State = ISTATBAR_BUTTON_Off;
  }
  else
  {
    if(ArrowButton[UP_ARROW].State == ISTATBAR_BUTTON_Off)
    {
      LE_SEQNCR_StartXY(ArrowButton[UP_ARROW].IdleID, ArrowButtonPriority,
                      -(745 - ArrowButton[UP_ARROW].HotSpot.left), -(283 - ArrowButton[UP_ARROW].HotSpot.top));
      ArrowButton[UP_ARROW].State = ISTATBAR_BUTTON_Idle;
    }
  }
  
  if(linecnt < (11+3))
  {
    if(ArrowButton[DOWN_ARROW].State == ISTATBAR_BUTTON_Idle)
      LE_SEQNCR_Stop(ArrowButton[DOWN_ARROW].IdleID, ArrowButtonPriority);
    else if(ArrowButton[DOWN_ARROW].State == ISTATBAR_BUTTON_Press)
      LE_SEQNCR_Stop(ArrowButton[DOWN_ARROW].PressID, ArrowButtonPriority);

    ArrowButton[DOWN_ARROW].State = ISTATBAR_BUTTON_Off;
  }
  else
  {
    if(ArrowButton[DOWN_ARROW].State == ISTATBAR_BUTTON_Off)
    {
      LE_SEQNCR_StartXY(ArrowButton[DOWN_ARROW].IdleID, ArrowButtonPriority,
                      -(745 - ArrowButton[DOWN_ARROW].HotSpot.left), -(402 - ArrowButton[DOWN_ARROW].HotSpot.top));
      ArrowButton[DOWN_ARROW].State = ISTATBAR_BUTTON_Idle;
    }
  }


	// display the popup window
	LE_GRAFIX_ShowTCB(fniBox.ID, 0, 0, LED_IFT(DAT_LANG2, TAB_tndisbrd));
	LE_SEQNCR_ForceRedraw(fniBox.ID, fniBox.Priority+1);
	
}
