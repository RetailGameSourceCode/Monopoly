/*****************************************************************************
 *
 * FILE:        UDAUCT.cpp, subfile of Display.cpp & UserIfce.cpp
 * DESCRIPTION: Handles display & User input specific to the statistics
 *    visible only on the status screen.
 *
 * © Copyright 1999 Artech Digital Entertainments.  All rights reserved.
 *****************************************************************************/

#include "gameinc.h"


/****************************************************************************/
/* P U B L I C   G L O B A L S                                              */
/*--------------------------------------------------------------------------*/

#define AUCT_NUMBEROFBILLS 7
#define AUCT_BILLWIDTH  18
#define AUCT_BILLHEIGHT 31
#define AUCT_BACKDROPSMALLWIDTH     134
#define AUCT_BACKDROPBIGWIDTH       200
#define AUCT_BACKDROPHEIGHT         90
#define AUCT_BACKDROPY              (600 - AUCT_BACKDROPHEIGHT)
#define AUCT_BILLTRAYSWIDTH         131
#define AUCT_BILLTRAYSHEIGHT        37
#define AUCT_BILLTRAYSY             (AUCT_BACKDROPY - AUCT_BILLTRAYSHEIGHT)
#define AUCT_BIDTEXTWIDTH           110
#define AUCT_BIDTEXTHEIGHT          27
#define AUCT_BIDTEXTY               (AUCT_BACKDROPY - AUCT_BILLTRAYSHEIGHT - AUCT_BIDTEXTHEIGHT)
#define AUCT_NAMETEXTWIDTH          AUCT_BACKDROPSMALLWIDTH
#define AUCT_NAMETEXTHEIGHT         30
#define AUCT_NAMETEXTY              (AUCT_BACKDROPY + 10)
#define AUCT_TOKENY                 (AUCT_BACKDROPY + 50)
#define AUCT_CASHTEXTWIDTH          AUCT_BACKDROPSMALLWIDTH
#define AUCT_CASHTEXTHEIGHT         30
#define AUCT_CASHTEXTY              (AUCT_BACKDROPY + 55)
#define AUCT_CURRENTBIDTEXTWIDTH    200
#define AUCT_CURRENTBIDTEXTHEIGHT   50

int AUCT_BackdropLocations[RULE_MAX_PLAYERS][2]; // Set at the start of the auction for the number of players.
int AUCT_BackdropWidth = AUCT_BACKDROPSMALLWIDTH;
int AUCT_BillDenominations[AUCT_NUMBEROFBILLS] = { 500, 100, 50, 20, 10, 5, 1 };


enum // Possible pennybags speach lines
{
  AUCT_PENNYBAGS_NONE = 0,   // a state, not an actual line
  AUCT_PENNYBAGS_IDLE,       // a state, not an actual line
  AUCT_PENNYBAGS_INTRO,
  AUCT_PENNYBAGS_INSTRUCTIONS,
  AUCT_PENNYBAGS_STARTBIDDING,
  AUCT_PENNYBAGS_BEGIN,      // a state, not an actual line
  AUCT_PENNYBAGS_NAMEHIGHESTBIDDER,
  AUCT_PENNYBAGS_GOINGONCE,
  AUCT_PENNYBAGS_GOINGTWICE,
  AUCT_PENNYBAGS_SOLD,
  AUCT_PENNYBAGS_CONGRATS,
  AUCT_PENNYBAGS_ENDAUCTION, // a state, not an actual line
};


/****************************************************************************/
/* P RI V A T E   G L O B A L S                                             */
/*--------------------------------------------------------------------------*/

static BOOL udauct_DoneAnAuctionBefore = FALSE;
              // Don't bother with some of the intro penny bags lines if we've heard them before.

static int  udauct_RollCallSerialNumber;
              // Before the auction can actually start, we must respond to
              // the ARE_YOU_THERE message with the appropriate code in our
              // I_AM_HERE message.  If this variable is non-zero this means
              // that the ARE_YOU_THERE message for an auction has been
              // received, and the value is the code to use.  If this variable
              // is 0 then it is not appropriate to give a response, because
              // the call has not been made.
static int  udauct_RollCallRoll;
              // The bit set of players that are expected to respond, although
              // we only worry about local players.

/****************************************************************************/
/* C O D E                                                                  */
/*--------------------------------------------------------------------------*/

/*****************************************************************************
 * All display initialization, showing, hiding and destroying occurs in
 * the following 4 subroutines called from DISPLAY.CPP.

 * No code other than the show  module may start nor stop sound
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


/*****************************************************************************
 * Initialize any permanent display objects.  Avoid temporary ones when possible.
 */
void DISPLAY_UDAUCT_Initialize( void )
{
  register int i;

  // Auction screen
  DISPLAY_state.AuctionBottomBar = LED_EI;
  DISPLAY_state.AuctionHighestBidDisplayed = 0;
  DISPLAY_state.AuctionHighestBidder = -1;
  DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_NONE;
  DISPLAY_state.AuctionPennyBagsSwitchState = FALSE;
  DISPLAY_state.Auction3rdGoingReceived = FALSE;
  DISPLAY_state.AuctionPennyBagsAnimReachedEnd = FALSE;
  DISPLAY_state.AuctionPennyBagsDisplayedID = LED_EI;
  DISPLAY_state.AuctionPennyBagsDesiredID = LED_EI;
  DISPLAY_state.AuctionPennyBagsSound = NULL;
  DISPLAY_state.AuctionPennyBagsDesiredSound = NULL;
  DISPLAY_state.AuctionPennyBagsSpecificFunction = FALSE;
  for ( i = 0; i < RULE_MAX_PLAYERS; i++ )
  {
    DISPLAY_state.AuctionPlayerBackdrop[i] = LED_EI;
    DISPLAY_state.AuctionPlayerToken[i] = LED_EI;
    DISPLAY_state.AuctionPlayerNameText[i] = LE_GRAFIX_ObjectCreate(
     AUCT_NAMETEXTWIDTH, AUCT_NAMETEXTHEIGHT, LE_GRAFIX_ObjectTransparent );
    DISPLAY_state.AuctionPlayerBidText[i] = LE_GRAFIX_ObjectCreate(
     AUCT_BIDTEXTWIDTH, AUCT_BIDTEXTHEIGHT, LE_GRAFIX_ObjectTransparent );
    DISPLAY_state.AuctionPlayerCashText[i] = LE_GRAFIX_ObjectCreate(
     AUCT_CASHTEXTWIDTH, AUCT_CASHTEXTHEIGHT, LE_GRAFIX_ObjectTransparent );
    DISPLAY_state.AuctionPlayerDisplayed[i] = FALSE;
    DISPLAY_state.AuctionBids[i] = 0;
    DISPLAY_state.AuctionBidsPrinted[i] = -1;
  }
  DISPLAY_state.AuctionCurrentBidText = LE_GRAFIX_ObjectCreate(
   AUCT_CURRENTBIDTEXTWIDTH, AUCT_CURRENTBIDTEXTHEIGHT, LE_GRAFIX_ObjectTransparent );
  DISPLAY_state.AuctionCurrentBidIsDisplayed = FALSE;
  DISPLAY_state.AuctionPropertyForSale = LED_EI;
  DISPLAY_state.AuctionPropertyForSaleShown = LED_EI;
  DISPLAY_state.AuctionPennyBagsSwitchState = FALSE;
  DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_NONE;
  DISPLAY_state.AuctionPlayersAllowedToBid = 0;
  DISPLAY_state.AuctionPlaceBuildingSet = 0;
}


void DISPLAY_UDAUCT_Destroy( void )
{ // Deallocate stuff from Initialize.  It is safe to assume the screen is black.
  register int iTemp;

  for ( iTemp = 0; iTemp < RULE_MAX_PLAYERS; iTemp++ )
  {
    LE_DATA_FreeRuntimeDataID( DISPLAY_state.AuctionPlayerNameText[ iTemp ] );
    LE_DATA_FreeRuntimeDataID( DISPLAY_state.AuctionPlayerBidText[ iTemp ] );
    LE_DATA_FreeRuntimeDataID( DISPLAY_state.AuctionPlayerCashText[ iTemp ] );
  }
  LE_DATA_FreeRuntimeDataID( DISPLAY_state.AuctionCurrentBidText );

}

void DISPLAY_UDAUCT_Show( void )
{ // Recalculate what should be on the screen, compare to what is on the screen,
  // and take corrective action.  Everything is in the user interface and display states.
  register int                    iTemp;
  LE_DATA_DataId                  tempID;
  ACHAR                           myBuffer[40];
  LE_SEQNCR_RuntimeInfoRecord     InfoGeneric;

  // This is a funny screen.  The backdrop only covers 450 in y-direction.  Bottom part
  // is for this "special" ibar with cash trays for bidding.
  if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_AuctionA )
    tempID = LED_IFT( DAT_PAT, TAB_anbg00 );
  else
    tempID = LED_EI;
  if ( DISPLAY_state.AuctionBottomBar != tempID )
  {
    if ( DISPLAY_state.AuctionBottomBar != LED_EI )
      LE_SEQNCR_Stop( DISPLAY_state.AuctionBottomBar, DISPLAY_AuctionBasePriority - 1 );
    DISPLAY_state.AuctionBottomBar = tempID;
    if ( DISPLAY_state.AuctionBottomBar != LED_EI )
      LE_SEQNCR_StartXY( DISPLAY_state.AuctionBottomBar, DISPLAY_AuctionBasePriority - 1, 0, 450 );
  }

  // Screen specifics
  if ( DISPLAY_state.desired2DView == DISPLAY_SCREEN_AuctionA )
  {
    if ( DISPLAY_state.AuctionPennyBagsSwitchState )
    {
      DISPLAY_state.AuctionPennyBagsSwitchState = FALSE;
      DISPLAY_state.AuctionPennyBagsSpecificFunction = 0;
      switch ( DISPLAY_state.AuctionPennyBagsNextState )
      {
      case AUCT_PENNYBAGS_INTRO:
        if ( udauct_DoneAnAuctionBefore )
        {
          // Skip a bunch of intro lines except for the very first time for the user.
          // Go immediately to the start bidding line.
          DISPLAY_state.AuctionPennyBagsSwitchState = TRUE;
          DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_STARTBIDDING;
        }
        else
        {
          udauct_DoneAnAuctionBefore = TRUE;

          switch ( rand() % 3 )
          {
          case 0:
            DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an01 );
            break;
          case 1:
            DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an10 );
            break;
          case 2:
            DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an11 );
            break;
          }
          DISPLAY_state.AuctionPennyBagsDesiredSound = PB_Auction;
          DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_INSTRUCTIONS;
        }
        DISPLAY_state.AuctionPennyBagsAnimReachedEnd = TRUE;   // Not guaranteed otherwise (make sure it is initialized)
        break;

      case AUCT_PENNYBAGS_INSTRUCTIONS:
        switch ( rand() % 3 )
        {
        case 0:
          DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an02 );
          break;
        case 1:
          DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an12 );
          break;
        case 2:
          DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an13 );
          break;
        }
        DISPLAY_state.AuctionPennyBagsDesiredSound = PB_Auction_ExplanationOnButtonsToIncreaseBid;
        DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_STARTBIDDING;
        break;

      case AUCT_PENNYBAGS_STARTBIDDING:
        switch ( rand() % 3 )
        {
        case 0:
          DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an14 );
          break;
        case 1:
          DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an15 );
          break;
        case 2:
          DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an03 );
          break;
        }
        DISPLAY_state.AuctionPennyBagsDesiredSound = PB_Auction_OpeningBid;
        DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_BEGIN;
        break;

      case AUCT_PENNYBAGS_BEGIN:
        if ( udauct_RollCallSerialNumber != 0 )
        {
          // We should be responding to the roll-call.  Now that we are in the idle
          // state for the first time we are indeed ready to auction.
          for ( iTemp = 0; iTemp < UICurrentGameState.NumberOfPlayers; iTemp++ )
          {
            if ( SlotIsALocalPlayer[iTemp] && (udauct_RollCallRoll & (1 << iTemp)) )
            {
              MESS_SendAction( ACTION_I_AM_HERE, iTemp, RULE_BANK_PLAYER,
               udauct_RollCallSerialNumber,
               0, 0, 0, NULL, 0, NULL );
            }
          }
          // Respond only once to the roll call.
          udauct_RollCallSerialNumber = 0;
        }
        DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_IDLE;
        break;

      case AUCT_PENNYBAGS_IDLE:
        break;

      case AUCT_PENNYBAGS_NAMEHIGHESTBIDDER:
        if ( (DISPLAY_state.AuctionHighestBidder >= 0) && (DISPLAY_state.AuctionHighestBidder < RULE_MAX_PLAYERS) )
        {
          DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an04 );
          // Look up the WAV_ data id from an array, then eventually call a different
          // sound function.
          DISPLAY_state.AuctionPennyBagsDesiredSound = WAV_pb264
           + UICurrentGameState.Players[DISPLAY_state.AuctionHighestBidder].token;
          // Have to use a different sound triggering function call for these,
          // because we don't want it to choose a random one, we want a particular token.
          DISPLAY_state.AuctionPennyBagsSpecificFunction = 1;
        }
        DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_IDLE;
        break;

      case AUCT_PENNYBAGS_GOINGONCE:
        DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an05 );
        DISPLAY_state.AuctionPennyBagsDesiredSound = PB_Auction_GoingOnce;
        DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_IDLE;
        break;

      case AUCT_PENNYBAGS_GOINGTWICE:
        DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an06 );
        DISPLAY_state.AuctionPennyBagsDesiredSound = PB_Auction_GoingTwice;
        DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_IDLE;
        break;

      case AUCT_PENNYBAGS_SOLD:
        if(DISPLAY_state.AuctionHighestBidDisplayed > 0)
        {            
            DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an07 );
            DISPLAY_state.AuctionPennyBagsDesiredSound = PB_Auction_GoingSold;
            DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_CONGRATS;
        }
        else
        {
            DISPLAY_state.AuctionPennyBagsSpecificFunction = 1;
            DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an07 );
            DISPLAY_state.AuctionPennyBagsDesiredSound = WAV_pb277;//PB_Auction_GoingSold;
            DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_ENDAUCTION;
        }
        break;

      case AUCT_PENNYBAGS_CONGRATS:
        switch ( rand() % 3 )
        {
        case 0:
          DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an08 );
          break;
        case 1:
          DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an16 );
          break;
        case 2:
          DISPLAY_state.AuctionPennyBagsDesiredID = LED_IFT( DAT_PAT, CNK_an17 );
          break;
        }
        // Choose pennybags congrats line.
        if ( DISPLAY_state.AuctionPropertyForSale >= 28 )
        {
          // For house or hotel just say pb279.  In some languages, the translation
          // for "property" may signify land only, thus it is inappropriate when
          // a house/hotel is involved.
          DISPLAY_state.AuctionPennyBagsDesiredSound = WAV_pb279;
          // Have to use a different sound triggering function call for these,
          // because we don't want it to choose a random one, we want a particular token.
          DISPLAY_state.AuctionPennyBagsSpecificFunction = 1;
        }
        else
        {
          DISPLAY_state.AuctionPennyBagsDesiredSound = PB_Auction_CongratulateWinner;
        }
        DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_ENDAUCTION;
        break;

      case AUCT_PENNYBAGS_ENDAUCTION:
        // Force a refresh of bid/cash text for the next time we come into an auction.
        for ( iTemp = 0; iTemp < RULE_MAX_PLAYERS; iTemp++ )
          DISPLAY_state.AuctionBidsPrinted[iTemp] = -1;
        DISPLAY_state.AuctionPennyBagsDesiredID = LED_EI;
        DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_NONE;
        // This causes a quick switch of screens.
        UDBOARD_SetBackdrop( DISPLAY_state.former2DView );
        break;
      }
    }
  }
  else
  {
    DISPLAY_state.AuctionPennyBagsDesiredID = LED_EI;
  }

  // Show what is being auctioned
  if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_AuctionA )
#if USA_VERSION
    if( DISPLAY_state.AuctionPropertyForSale >= 28 )
    {
      tempID = LED_IFT( DAT_LANG2, TAB_deedhs ) + DISPLAY_state.AuctionPropertyForSale - 28;
    } else
    {
      tempID = LED_IFT( DAT_LANG2, TAB_deed01 ) + DISPLAY_state.AuctionPropertyForSale
        + DISPLAY_DEEDS_PER_SET*max(DISPLAY_state.city, 0);
    }
#else
    if( DISPLAY_state.AuctionPropertyForSale >= 28 )
    {
      tempID = LED_IFT( DAT_LANG2, TAB_deedhs ) + DISPLAY_state.AuctionPropertyForSale - 28;
    } else
    {
      tempID = UDIBAR_Deeds[ DISPLAY_state.AuctionPropertyForSale ];
    }
#endif
  else
    tempID = LED_EI;
  //Affect changes
  if( DISPLAY_state.AuctionPropertyForSaleShown != tempID )
  {
    if( DISPLAY_state.AuctionPropertyForSaleShown != LED_EI )
      LE_SEQNCR_Stop( DISPLAY_state.AuctionPropertyForSaleShown, DISPLAY_AuctionPropertyPriority );
    DISPLAY_state.AuctionPropertyForSaleShown = tempID;
    if ( DISPLAY_state.AuctionPropertyForSaleShown != LED_EI )
      LE_SEQNCR_StartXY( DISPLAY_state.AuctionPropertyForSaleShown, DISPLAY_AuctionPropertyPriority,
       20, 20 );
  }
  if ( DISPLAY_state.desired2DView == DISPLAY_SCREEN_AuctionA )
  {
    // Get the (empty) current bid text graphic going.
    if ( !DISPLAY_state.AuctionCurrentBidIsDisplayed )
    {
      LE_SEQNCR_StartXY( DISPLAY_state.AuctionCurrentBidText, DISPLAY_AuctionPropertyPriority + 1,
       20, 280 );
      // Draw the "current bid" part just once at the top.
      LE_GRAFIX_ColorArea(DISPLAY_state.AuctionCurrentBidText, 0, 0,
       AUCT_CURRENTBIDTEXTWIDTH, AUCT_CURRENTBIDTEXTHEIGHT,
       LEG_MCR( 0, 255, 0) ); // Green, transparent
      LE_FONTS_Print( DISPLAY_state.AuctionCurrentBidText, 10, 5,
       LEG_MCR( 255, 255, 255 ), LANG_GetTextMessage(TMN_AUCTION_CURRENT_BID) );
    }
    // Update current bid text.
    if ( (AuctionHighestBid != DISPLAY_state.AuctionHighestBidDisplayed)
     || !DISPLAY_state.AuctionCurrentBidIsDisplayed )
    {
      DISPLAY_state.AuctionCurrentBidIsDisplayed = TRUE;
      DISPLAY_state.AuctionHighestBidDisplayed = AuctionHighestBid;
      // Now draw the new value into the bottom part.
      LE_GRAFIX_ColorArea(DISPLAY_state.AuctionCurrentBidText, 0, AUCT_CURRENTBIDTEXTHEIGHT/2,
       AUCT_CURRENTBIDTEXTWIDTH, AUCT_CURRENTBIDTEXTHEIGHT/2,
       LEG_MCR( 0, 255, 0) ); // Green, transparent
      UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( DISPLAY_state.AuctionHighestBidDisplayed,
       DISPLAY_state.system, myBuffer, TRUE );
      LE_FONTS_Print( DISPLAY_state.AuctionCurrentBidText, 10, 30,
       LEG_MCR( 255, 255, 255 ), myBuffer );
      LE_SEQNCR_ForceRedraw( DISPLAY_state.AuctionCurrentBidText, DISPLAY_AuctionPropertyPriority + 1 );
    }
  }
  else
  {
    if ( DISPLAY_state.AuctionCurrentBidIsDisplayed )
    {
      DISPLAY_state.AuctionCurrentBidIsDisplayed = FALSE;
      LE_SEQNCR_Stop( DISPLAY_state.AuctionCurrentBidText, DISPLAY_AuctionPropertyPriority + 1 );
    }
  }

  // Player specific items
  for( iTemp = 0; iTemp < UICurrentGameState.NumberOfPlayers; iTemp++ )
  {
    if( DISPLAY_state.desired2DView == DISPLAY_SCREEN_AuctionA )
    { // We're on, make sure the object is up to date.
      if( DISPLAY_state.AuctionBidsPrinted[iTemp] != DISPLAY_state.AuctionBids[iTemp] )
      { // Update it OPTIMIZE: Name probably did not change.
        LE_GRAFIX_ColorArea(DISPLAY_state.AuctionPlayerNameText[iTemp], 0, 0,
         AUCT_NAMETEXTWIDTH, AUCT_NAMETEXTHEIGHT, LEG_MCR( 0, 255, 0) ); // Green, transparent
        LE_FONTS_Print( DISPLAY_state.AuctionPlayerNameText[iTemp],
         AUCT_NAMETEXTWIDTH/2 - LE_FONTS_GetStringWidth(UICurrentGameState.Players[iTemp].name)/2,
         0,
         LEG_MCR( 0, 0, 0 ),
         UICurrentGameState.Players[iTemp].name );
        // We wont mark the text as redrawn yet, the screen update will handle it (below).
        LE_GRAFIX_ColorArea(DISPLAY_state.AuctionPlayerBidText[iTemp], 0, 0,
         AUCT_BIDTEXTWIDTH, AUCT_BIDTEXTHEIGHT, LEG_MCR( 0, 255, 0) ); // Green, transparent
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( DISPLAY_state.AuctionBids[iTemp],
         DISPLAY_state.system, myBuffer, FALSE );
        LE_FONTS_Print( DISPLAY_state.AuctionPlayerBidText[iTemp],
         AUCT_BIDTEXTWIDTH/2 - LE_FONTS_GetStringWidth(myBuffer)/2,
         5,
         LEG_MCR( 255, 255, 255 ), myBuffer );
        LE_GRAFIX_ColorArea(DISPLAY_state.AuctionPlayerCashText[iTemp], 0, 0,
         AUCT_CASHTEXTWIDTH, AUCT_CASHTEXTHEIGHT, LEG_MCR( 0, 255, 0) ); // Green, transparent
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( UICurrentGameState.Players[iTemp].cash - DISPLAY_state.AuctionBids[iTemp],
         DISPLAY_state.system, myBuffer, TRUE );
        LE_FONTS_Print( DISPLAY_state.AuctionPlayerCashText[iTemp],
         AUCT_CASHTEXTWIDTH/2 - LE_FONTS_GetStringWidth(myBuffer)/2,
         5,
         LEG_MCR( 0, 0, 0 ), myBuffer );
      }
    }
  } //endfor

  //Affect changes
  if ( DISPLAY_state.AuctionPennyBagsDisplayedID != LED_EI )
  {
    if ( !DISPLAY_state.AuctionPennyBagsAnimReachedEnd )
    {
      LE_SEQNCR_GetInfo( DISPLAY_state.AuctionPennyBagsDisplayedID, DISPLAY_AuctionPennybagsPriority, FALSE,
       &InfoGeneric, NULL, NULL, NULL, NULL );
      if ( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
      {
        // We're at the end, switch penny bags anim if that is what we're supposed to do.
        DISPLAY_state.AuctionPennyBagsSwitchState =
         DISPLAY_state.AuctionPennyBagsNextState != AUCT_PENNYBAGS_IDLE;
        // If we don't end up switching, we're idling.
        DISPLAY_state.AuctionPennyBagsAnimReachedEnd = TRUE;
      }
    }
  }
  if ( DISPLAY_state.AuctionPennyBagsDisplayedID != DISPLAY_state.AuctionPennyBagsDesiredID )
  {
    if ( DISPLAY_state.AuctionPennyBagsDisplayedID != LED_EI )
    {
      LE_SEQNCR_Stop( DISPLAY_state.AuctionPennyBagsDisplayedID, DISPLAY_AuctionPennybagsPriority );
      DISPLAY_state.AuctionPennyBagsAnimReachedEnd = TRUE;
    }
    DISPLAY_state.AuctionPennyBagsDisplayedID = DISPLAY_state.AuctionPennyBagsDesiredID;
    if ( DISPLAY_state.AuctionPennyBagsDisplayedID != LED_EI )
    {
      LE_SEQNCR_Start( DISPLAY_state.AuctionPennyBagsDisplayedID, DISPLAY_AuctionPennybagsPriority );
      DISPLAY_state.AuctionPennyBagsAnimReachedEnd = FALSE;
    }
  }
  // Note:  Sound should always be changed in synch with the anim
  if ( DISPLAY_state.AuctionPennyBagsSound != DISPLAY_state.AuctionPennyBagsDesiredSound )
  {
    DISPLAY_state.AuctionPennyBagsSound = DISPLAY_state.AuctionPennyBagsDesiredSound;
    // Note: with the "specific" call the value passed is a dataid,
    // with the "general" call it is an enum index.
    if ( DISPLAY_state.AuctionPennyBagsSpecificFunction )
      UDPENNY_PennybagsSpecific( DISPLAY_state.AuctionPennyBagsSound, Sound_ClipOldSoundIfPlaying );
    else
      UDPENNY_PennybagsGeneral( DISPLAY_state.AuctionPennyBagsSound, Sound_ClipOldSoundIfPlaying );
  }

  for( iTemp = 0; iTemp < RULE_MAX_PLAYERS; iTemp++ )
  { // Correct Player specifics
    if( (DISPLAY_state.desired2DView == DISPLAY_SCREEN_AuctionA) &&
        (iTemp < UICurrentGameState.NumberOfPlayers) &&
        (DISPLAY_state.AuctionPlayersAllowedToBid & (1 << iTemp)) )
    { // Its on
      if( DISPLAY_state.AuctionBidsPrinted[iTemp] != DISPLAY_state.AuctionBids[iTemp] )
      {
        DISPLAY_state.AuctionBidsPrinted[iTemp] = DISPLAY_state.AuctionBids[iTemp];
        // Force the rectangle update now iff the object is already displayed.
        if ( DISPLAY_state.AuctionPlayerDisplayed[iTemp] )
        {
          LE_SEQNCR_ForceRedraw( DISPLAY_state.AuctionPlayerBidText[iTemp], DISPLAY_AuctionBasePriority + iTemp + 2 );
          LE_SEQNCR_ForceRedraw( DISPLAY_state.AuctionPlayerCashText[iTemp], DISPLAY_AuctionBasePriority + iTemp + 2 );
        }
      }
      if( !DISPLAY_state.AuctionPlayerDisplayed[iTemp] )
      {
        LE_DATA_DataId  tokenIDArray[] = { LED_IFT(DAT_PAT,CNK_aa01n),
                                           LED_IFT(DAT_PAT,CNK_aa02n),
                                           LED_IFT(DAT_PAT,CNK_aa03n),
                                           LED_IFT(DAT_PAT,CNK_aa07n),
                                           LED_IFT(DAT_PAT,CNK_aa05n),
                                           LED_IFT(DAT_PAT,CNK_aa04n),
                                           LED_IFT(DAT_PAT,CNK_aa00n),
                                           LED_IFT(DAT_PAT,CNK_aa09n),
                                           LED_IFT(DAT_PAT,CNK_aa06n),
                                           LED_IFT(DAT_PAT,CNK_aa10n),
                                           LED_IFT(DAT_PAT,CNK_aa08n) };

        // Initializing display.
        DISPLAY_state.AuctionPlayerDisplayed[iTemp] = TRUE;
        //Everything else goes with this flag...  We record token & color, they wont auto update but will clear on exit even if changed (they dont change)
        LE_SEQNCR_StartXY( DISPLAY_state.AuctionPlayerBackdrop[iTemp], DISPLAY_AuctionBasePriority + iTemp,
         AUCT_BackdropLocations[iTemp][0] - AUCT_BackdropWidth/2, AUCT_BACKDROPY );
        LE_SEQNCR_StartXY( LED_IFT( DAT_PAT, TAB_antr00 ), DISPLAY_AuctionBasePriority + iTemp,
         AUCT_BackdropLocations[iTemp][0] - AUCT_BILLTRAYSWIDTH/2, AUCT_BILLTRAYSY );
        LE_SEQNCR_StartXY( LED_IFT( DAT_PAT, TAB_antx01 ), DISPLAY_AuctionBasePriority + iTemp,
         AUCT_BackdropLocations[iTemp][0] - AUCT_BIDTEXTWIDTH/2, AUCT_BIDTEXTY );
        DISPLAY_state.AuctionPlayerToken[iTemp] = tokenIDArray[UICurrentGameState.Players[iTemp].token];
        LE_SEQNCR_StartXY( DISPLAY_state.AuctionPlayerToken[iTemp], DISPLAY_AuctionBasePriority + iTemp + 1,
         AUCT_BackdropLocations[iTemp][0], AUCT_TOKENY );
        LE_SEQNCR_StartXY( DISPLAY_state.AuctionPlayerNameText[iTemp], DISPLAY_AuctionBasePriority + iTemp + 2,
         AUCT_BackdropLocations[iTemp][0] - AUCT_NAMETEXTWIDTH/2, AUCT_NAMETEXTY );
        LE_SEQNCR_StartXY( DISPLAY_state.AuctionPlayerBidText[iTemp], DISPLAY_AuctionBasePriority + iTemp + 2,
         AUCT_BackdropLocations[iTemp][0] - AUCT_BIDTEXTWIDTH/2, AUCT_BIDTEXTY );
        LE_SEQNCR_StartXY( DISPLAY_state.AuctionPlayerCashText[iTemp], DISPLAY_AuctionBasePriority + iTemp + 2,
         AUCT_BackdropLocations[iTemp][0] - AUCT_CASHTEXTWIDTH/2, AUCT_CASHTEXTY );
      }
    }
    else
    { // its off, no auction to display.
      if( DISPLAY_state.AuctionPlayerDisplayed[iTemp] )
      {// Kill the bid box
        DISPLAY_state.AuctionPlayerDisplayed[iTemp] = FALSE;
        LE_SEQNCR_Stop( DISPLAY_state.AuctionPlayerNameText[iTemp], DISPLAY_AuctionBasePriority + iTemp + 2 );
        LE_SEQNCR_Stop( DISPLAY_state.AuctionPlayerBidText[iTemp], DISPLAY_AuctionBasePriority + iTemp + 2 );
        LE_SEQNCR_Stop( DISPLAY_state.AuctionPlayerCashText[iTemp], DISPLAY_AuctionBasePriority + iTemp + 2 );
        LE_SEQNCR_Stop( DISPLAY_state.AuctionPlayerBackdrop[iTemp], DISPLAY_AuctionBasePriority + iTemp );
        LE_SEQNCR_Stop( DISPLAY_state.AuctionPlayerToken[iTemp], DISPLAY_AuctionBasePriority + iTemp + 1 );
        LE_SEQNCR_Stop( LED_IFT( DAT_PAT, TAB_antr00 ), DISPLAY_AuctionBasePriority + iTemp );
        LE_SEQNCR_Stop( LED_IFT( DAT_PAT, TAB_antx01 ), DISPLAY_AuctionBasePriority + iTemp );
      }
    }

  } // endfor

}


/*****************************************************************************
 * Standard UI subfunction - Process library message.
 */

void UDAUCT_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr )
{//ACTION_BUY_OR_AUCTION_DECISION ACTION_BID (ACTION_START_HOUSING_AUCTION from IBAR)
  // NOTE:  The big ugly green dialog is handled in IBAR, new functions go here.
  register int iTemp;
           int iTemp2;

  if( (DISPLAY_state.desired2DView == DISPLAY_SCREEN_AuctionA) &&
      (UIMessagePntr->messageCode == UIMSG_MOUSE_LEFT_DOWN ) )
  {//&& could check for an auction active, though if the bills are up we are fairly safe.
    //Process local human bills if the auction screen is up
    for( iTemp = 0; iTemp < UICurrentGameState.NumberOfPlayers; iTemp++ )
    {// For each player
      if ( DISPLAY_state.AuctionPlayerDisplayed[iTemp] && SlotIsALocalHumanPlayer[iTemp] )
      {// For each bill
        for( iTemp2 = 0; iTemp2 < AUCT_NUMBEROFBILLS; iTemp2++ )
        {
          if ( (UIMessagePntr->numberA >= (AUCT_BackdropLocations[iTemp][0] - AUCT_BILLTRAYSWIDTH/2 + 2 + AUCT_BILLWIDTH*iTemp2))
           && (UIMessagePntr->numberA <  (AUCT_BackdropLocations[iTemp][0] - AUCT_BILLTRAYSWIDTH/2 + 2 + AUCT_BILLWIDTH*(iTemp2+1)))
           && (UIMessagePntr->numberB >= (AUCT_BILLTRAYSY + 5))
           && (UIMessagePntr->numberB <  (AUCT_BILLTRAYSY + 5 + AUCT_BILLHEIGHT)) )
          {// Send it, flag the animation.
            MESS_SendAction( ACTION_BID, (RULE_PlayerNumber)iTemp,
             RULE_BANK_PLAYER, AuctionHighestBid + AUCT_BillDenominations[iTemp2], 0, 0, 0, NULL, 0, NULL );
            break;
          }
        }// End for each bill
      }
    }//End for each player
  }
}


/*****************************************************************************
 * Standard UI subfunction - Process game message to player.
 * Auction Notification actions.
 */

void UDAUCT_ProcessMessageToPlayer( RULE_ActionArgumentsPointer NewMessage )
{
  int   iTemp;
  int             spacing;
  int             width;
  LE_DATA_DataId  tempID;

  switch (NewMessage->action)
  {
  case NOTIFY_AUCTION_GOING:
  case NOTIFY_NEW_HIGH_BID:
    PlayersAllowedInAuction = (RULE_PlayerSet) NewMessage->numberE;
    AuctionHighestBid = NewMessage->numberB; /* Save for internal use. */

    if ( NewMessage->action == NOTIFY_NEW_HIGH_BID &&
     NewMessage->numberA == RULE_BANK_PLAYER )
    {
      if ( UICurrentGameState.NumberOfPlayers > 4 )
      {
        width = 134;
        tempID = LED_IFT( DAT_PAT, TAB_anc07 );
        AUCT_BackdropWidth = AUCT_BACKDROPSMALLWIDTH;
      }
      else
      {
        width = 201;
        tempID = LED_IFT( DAT_PAT, TAB_anc01 );
        AUCT_BackdropWidth = AUCT_BACKDROPBIGWIDTH;
      }
      spacing = (800 - UICurrentGameState.NumberOfPlayers*width) / (UICurrentGameState.NumberOfPlayers + 1);

      // Just starting the auction (OR a gong with no current bid).
      UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, UICurrentGameState.CurrentPlayer );
      for( iTemp = 0; iTemp < UICurrentGameState.NumberOfPlayers; iTemp++ )
      {//We assume the player list wont change, though graphically we assure the screen will clear properly if it does.
        // The base x position will be the center of the player's backdrop.
        AUCT_BackdropLocations[iTemp][0] = spacing + iTemp*(width + spacing) + width/2;
        AUCT_BackdropLocations[iTemp][1] = 450;
        DISPLAY_state.AuctionPlayerBackdrop[iTemp] = tempID + UICurrentGameState.Players[iTemp].colour;
        DISPLAY_state.AuctionBids[iTemp] = 0;
      }
      DISPLAY_state.AuctionHighestBidder = -1;
      DISPLAY_state.AuctionPlayersAllowedToBid = (RULE_PlayerSet)NewMessage->numberE;
      DISPLAY_state.AuctionPropertyForSale = DISPLAY_propertyToOwnablePropertyConversion( NewMessage->numberC );
      if( NewMessage->numberC == SQ_AUCTION_HOUSE )
        DISPLAY_state.AuctionPropertyForSale = 28;
      if( NewMessage->numberC == SQ_AUCTION_HOTEL )
        DISPLAY_state.AuctionPropertyForSale = 29;

      if( DISPLAY_state.desired2DView != DISPLAY_SCREEN_AuctionA ||
      DISPLAY_state.Auction3rdGoingReceived)
      { // If so, this should be an auction start.  Otherwise was a new high bid of 0 - no bids (Going).
        // Note tricky situation for an auction right after another auction
        // which happens when a player goes bankrupt and the bank auctions off
        // all his properties.  Use the Auction3rdGoingReceived to realise
        // that the high bid of zero means a new auction starting.
        if (DISPLAY_state.desired2DView != DISPLAY_SCREEN_AuctionA)
        {
          // The real first time into the auction screen.
          DISPLAY_state.former2DView = DISPLAY_state.desired2DView;
        }
        UDBOARD_SetBackdrop( DISPLAY_SCREEN_AuctionA );
        DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_INTRO;
        DISPLAY_state.Auction3rdGoingReceived = FALSE;
        DISPLAY_state.AuctionPennyBagsSwitchState = TRUE;
      }
    }
    else if (NewMessage->action == NOTIFY_AUCTION_GOING)
    {
      if ( DISPLAY_state.desired2DView == DISPLAY_SCREEN_AuctionA )
      {
        switch ( NewMessage->numberD )
        {
        case 1:
          DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_GOINGONCE;
          DISPLAY_state.AuctionPennyBagsSwitchState = TRUE;
          break;
        case 2:
          DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_GOINGTWICE;
          DISPLAY_state.AuctionPennyBagsSwitchState = TRUE;
          break;
        case 3:
          DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_SOLD;
          DISPLAY_state.AuctionPennyBagsSwitchState = TRUE;
          DISPLAY_state.Auction3rdGoingReceived = TRUE;
          break;
        }
      }
    }
    else
    {// NOTIFY_NEW_HIGH_BID unconditional
      // Set highest bid info.
      if( NewMessage->numberA < RULE_BANK_PLAYER )
      { // Call me paranoid, but I don't like risking array subscripts out of range.
        DISPLAY_state.AuctionBids[NewMessage->numberA] = AuctionHighestBid;
        DISPLAY_state.AuctionHighestBidder = NewMessage->numberA;
        DISPLAY_state.AuctionPennyBagsNextState = AUCT_PENNYBAGS_NAMEHIGHESTBIDDER;
        DISPLAY_state.AuctionPennyBagsSwitchState = TRUE;
      }
    }
    break;
  }
}


/*****************************************************************************
 * The rules engine is trying to start an auction and wants everyone to respond
 * when they are ready.  Wait till all the important penny bags intro lines are
 * said before we acknowledge that we are ready.
 */
void UDAUCT_Process_NOTIFY_ARE_YOU_THERE( RULE_ActionArgumentsPointer NewMessage )
{
  udauct_RollCallRoll = NewMessage->numberA;
  udauct_RollCallSerialNumber = NewMessage->numberB;
}
