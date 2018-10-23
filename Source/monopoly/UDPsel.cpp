/*****************************************************************************
 *
 * FILE:        UDPSEL.cpp, subfile of Display.cpp & UserIfce.cpp
 * DESCRIPTION: Handles display & User input specific to Player selection.
 *
 * © Copyright 1999 Artech Digital Entertainments.  All rights reserved.
 *****************************************************************************/

#include "gameinc.h"

#include "..\PC3D\PC3DHdr.h"
#include "TexInfo.h"


/****************************************************************************/
/* P U B L I C   G L O B A L S                                              */
/*--------------------------------------------------------------------------*/

UDPSEL_PlayerInfoStruct UDPSEL_PlayerInfo;


/****************************************************************************/
/* P RI V A T E   G L O B A L S                                             */
/*--------------------------------------------------------------------------*/

#define compilefor_TESTWORDWRAP             0

#define udpsel_FONTPOINTSIZE                8
#define udpsel_FONTWEIGHT                   700

#define udpsel_RULESBUTTONWIDTH             43
#define udpsel_RULESBUTTONHEIGHT            16
#define udpsel_RULESBUTTONCOLUMN1X          7
#define udpsel_RULESBUTTONCOLUMN2X          (400 + udpsel_RULESBUTTONCOLUMN1X)
#define udpsel_RULESROWHEIGHT               22
#define udpsel_RULESTOPY                    100
#define udpsel_RULESBOTTOMY                 475
#define udpsel_RULESROWTOPY                 100
#define udpsel_RULESROWBOTTOMY              450
#define udpsel_RULESDESCRIPTIONCOLUMN1X     (udpsel_RULESBUTTONCOLUMN1X + 4*udpsel_RULESBUTTONWIDTH + 10)
#define udpsel_RULESDESCRIPTIONCOLUMN2X     (400 + udpsel_RULESDESCRIPTIONCOLUMN1X)
#define udpsel_RULESDESCRIPTIONTEXTWIDTH    (400 - udpsel_RULESDESCRIPTIONCOLUMN1X)
#define udpsel_HISCOREPLAYERNAMEX           125   // Center of column
#define udpsel_HISCOREWINSX                 365   // Center of column
#define udpsel_HISCOREGREATESTNETWORTHX     660   // Right margin of where values align
#define udpsel_HISCORETOPY                  100

#define udpsel_HISTORYLOG_MAX_PLAYERS       100
#define udpsel_HISTORYLOG_PLAYERKEY         "Player"
#define udpsel_HISTORYLOG_NAMEKEY           "Name"
#define udpsel_HISTORYLOG_WINSKEY           "Wins"
#define udpsel_HISTORYLOG_NETWORTHKEY       "GreatestNetWorth"

#define udpsel_MAX_ENTERNAME_LENGTH         10


enum udpsel_Enum_RulesText
{
  udpsel_RULESTEXT_HOUSESPERHOTEL,
  udpsel_RULESTEXT_TOTALHOUSES,
  udpsel_RULESTEXT_TOTALHOTELS,
  udpsel_RULESTEXT_FREEPARKINGJACKPOTAMOUNT,
  udpsel_RULESTEXT_INITIALCASH,
  udpsel_RULESTEXT_SALARY,
  udpsel_RULESTEXT_INCOMETAXRATE,
  udpsel_RULESTEXT_INCOMETAXAMOUNT,
  udpsel_RULESTEXT_LUXURYTAX,
  udpsel_RULESTEXT_MAXIMUMTURNSINJAIL,
  udpsel_RULESTEXT_JAILFEE,
  udpsel_RULESTEXT_HOUSESREMAININGFORSHORTAGE,
  udpsel_RULESTEXT_HOTELSREMAININGFORSHORTAGE,
  udpsel_RULESTEXT_MORTGAGEINTERESTRATE,
  udpsel_RULESTEXT_AUCTIONTIMEDELAYINSECONDS,
  udpsel_RULESTEXT_PROPERTIESDEALTATSTART,
  udpsel_RULESTEXT_EVENBUILDRULE,
  udpsel_RULESTEXT_COLLECTDOUBLEMONEYFORLANDINGONGO,
  udpsel_RULESTEXT_FREEPARKINGRULE,
  udpsel_RULESTEXT_FUTURESANDIMMUNITIES,
  udpsel_RULESTEXT_DEALTPROPERTIESAREFREE,
  udpsel_MAX_RULESTEXT,
};
static POINT    udpsel_RulesTextPosition[udpsel_MAX_RULESTEXT];  
static wchar_t  *udpsel_RulesWideText[udpsel_MAX_RULESTEXT];
int             udpsel_RulesWideIndex[udpsel_MAX_RULESTEXT];


static BOOL udpsel_ForcedRefresh = FALSE;

typedef struct tag_udpsel_ObjectStruct
{
  LE_DATA_DataId            currentID,    desiredID;
  LE_DATA_DataId            idleID,       pressID,    animInID,   animOutID;
  LE_DATA_DataId            postAnimInID, postAnimOutID;   // switch to these after animating
  int                       priority;
  int                       startX,       startY;
  LE_SEQNCR_EndingAction    endingAction;
  RECT                      rect;
  BOOL                      hotspotEnabled;
} udpsel_ObjectStruct;


enum udpsel_Enum_Object
{
  udpsel_OBJFIRST_HISCORE,
  udpsel_OBJ_HISCORE_STATIC = udpsel_OBJFIRST_HISCORE,
  udpsel_OBJ_HISCORE_TEXTOVERLAY,
  udpsel_OBJ_HISCORE_BUTTON_PLAY,
  udpsel_OBJLAST_HISCORE = udpsel_OBJ_HISCORE_BUTTON_PLAY,
  udpsel_OBJFIRST_LOCALNETWORK,
  udpsel_OBJ_LOCALNETWORK_BUTTON_LOCAL = udpsel_OBJFIRST_LOCALNETWORK,
  udpsel_OBJ_LOCALNETWORK_BUTTON_NETWORK,
  udpsel_OBJ_LOCALNETWORK_BUTTON_SAVED,
  udpsel_OBJLAST_LOCALNETWORK = udpsel_OBJ_LOCALNETWORK_BUTTON_SAVED,
  udpsel_OBJFIRST_SELECTPLAYER,
  udpsel_OBJ_SELECTPLAYER_TITLE = udpsel_OBJFIRST_SELECTPLAYER,
  udpsel_OBJ_SELECTPLAYER_BUTTON_NEW,
  udpsel_OBJ_SELECTPLAYER_BUTTON_MORE,
  udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1,
  udpsel_OBJ_SELECTPLAYER_BUTTON_CARD2,
  udpsel_OBJ_SELECTPLAYER_BUTTON_CARD3,
  udpsel_OBJ_SELECTPLAYER_BUTTON_CARD4,
  udpsel_OBJ_SELECTPLAYER_BUTTON_CARD5,
  udpsel_OBJ_SELECTPLAYER_BUTTON_CARD6,
  udpsel_OBJ_SELECTPLAYER_BUTTON_CARD7,
  udpsel_OBJ_SELECTPLAYER_BUTTON_CARD8,
  udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1,
  udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD2,
  udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD3,
  udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD4,
  udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD5,
  udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD6,
  udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD7,
  udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD8,
  udpsel_OBJLAST_SELECTPLAYER = udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD8,
  udpsel_OBJFIRST_ENTERNAME,
  udpsel_OBJ_ENTERNAME_TITLE = udpsel_OBJFIRST_ENTERNAME,
  udpsel_OBJ_ENTERNAME_TEXTOVERLAY,
  udpsel_OBJ_ENTERNAME_TEXTBOX,
  udpsel_OBJ_ENTERNAME_BUTTON_NEXT,
  udpsel_OBJLAST_ENTERNAME = udpsel_OBJ_ENTERNAME_BUTTON_NEXT,
  udpsel_OBJFIRST_PICKTOKEN,
  udpsel_OBJ_PICKTOKEN_TITLE = udpsel_OBJFIRST_PICKTOKEN,
  udpsel_OBJ_PICKTOKEN_STATIC_ROTATING,
  udpsel_OBJ_PICKTOKEN_ROTATING_CANNON,
  udpsel_OBJ_PICKTOKEN_ROTATING_RACECAR,
  udpsel_OBJ_PICKTOKEN_ROTATING_DOG,
  udpsel_OBJ_PICKTOKEN_ROTATING_TOPHAT,
  udpsel_OBJ_PICKTOKEN_ROTATING_IRON,
  udpsel_OBJ_PICKTOKEN_ROTATING_HORSE,
  udpsel_OBJ_PICKTOKEN_ROTATING_BATTLESHIP,
  udpsel_OBJ_PICKTOKEN_ROTATING_SHOE,
  udpsel_OBJ_PICKTOKEN_ROTATING_THIMBLE,
  udpsel_OBJ_PICKTOKEN_ROTATING_WHEELBARROW,
  udpsel_OBJ_PICKTOKEN_ROTATING_SACKOFMONEY,
  udpsel_OBJ_PICKTOKEN_BUTTON_CANNON,
  udpsel_OBJ_PICKTOKEN_BUTTON_RACECAR,
  udpsel_OBJ_PICKTOKEN_BUTTON_DOG,
  udpsel_OBJ_PICKTOKEN_BUTTON_TOPHAT,
  udpsel_OBJ_PICKTOKEN_BUTTON_IRON,
  udpsel_OBJ_PICKTOKEN_BUTTON_HORSE,
  udpsel_OBJ_PICKTOKEN_BUTTON_BATTLESHIP,
  udpsel_OBJ_PICKTOKEN_BUTTON_SHOE,
  udpsel_OBJ_PICKTOKEN_BUTTON_THIMBLE,
  udpsel_OBJ_PICKTOKEN_BUTTON_WHEELBARROW,
  udpsel_OBJ_PICKTOKEN_BUTTON_SACKOFMONEY,
  udpsel_OBJ_PICKTOKEN_BUTTON_NEXT,
  udpsel_OBJLAST_PICKTOKEN = udpsel_OBJ_PICKTOKEN_BUTTON_NEXT,
  udpsel_OBJFIRST_START,
  udpsel_OBJ_START_BUTTON_ADDHUMAN = udpsel_OBJFIRST_START,
  udpsel_OBJ_START_BUTTON_ADDCOMPUTER,
  udpsel_OBJ_START_BUTTON_REMOVEPLAYER,
  udpsel_OBJ_START_BUTTON_STARTGAME,
  udpsel_OBJLAST_START = udpsel_OBJ_START_BUTTON_STARTGAME,
  udpsel_OBJFIRST_REMOVE,
  udpsel_OBJ_REMOVE_TEXTOVERLAY = udpsel_OBJFIRST_REMOVE,
  udpsel_OBJ_REMOVE_BUTTON_CANCEL,
  udpsel_OBJLAST_REMOVE = udpsel_OBJ_REMOVE_BUTTON_CANCEL,
  udpsel_OBJFIRST_AISTRENGTH,
  udpsel_OBJ_AISTRENGTH_TITLE = udpsel_OBJFIRST_AISTRENGTH,
  udpsel_OBJ_AISTRENGTH_BUTTON_EASY,
  udpsel_OBJ_AISTRENGTH_BUTTON_MEDIUM,
  udpsel_OBJ_AISTRENGTH_BUTTON_HARD,
  udpsel_OBJLAST_AISTRENGTH = udpsel_OBJ_AISTRENGTH_BUTTON_HARD,
  udpsel_OBJFIRST_COUNTRY,
  udpsel_OBJ_CITY_BUTTON_CLASSICBOARD = udpsel_OBJFIRST_COUNTRY,
  udpsel_OBJ_CITY_BUTTON_LOADBOARD,
#if USA_VERSION
  udpsel_OBJ_CITY_STATIC,
  udpsel_OBJ_CITY_TEXTOVERLAY,
  udpsel_OBJ_CITY_BUTTON_LEFT,
  udpsel_OBJ_CITY_BUTTON_RIGHT,
#else
  udpsel_OBJ_COUNTRY_STATIC,
  udpsel_OBJ_COUNTRY_TEXTOVERLAY,
  udpsel_OBJ_COUNTRY_BUTTON_LEFT,
  udpsel_OBJ_COUNTRY_BUTTON_RIGHT,
  udpsel_OBJ_CURRENCY_STATIC,
  udpsel_OBJ_CURRENCY_TEXTOVERLAY,
  udpsel_OBJ_CURRENCY_BUTTON_LEFT,
  udpsel_OBJ_CURRENCY_BUTTON_RIGHT,
#endif
  udpsel_OBJ_CITY_BUTTON_NEXT,
  udpsel_OBJLAST_COUNTRY = udpsel_OBJ_CITY_BUTTON_NEXT,
  udpsel_OBJFIRST_RULESCHOICE,
  udpsel_OBJ_RULESCHOICE_BUTTON_STANDARD = udpsel_OBJFIRST_RULESCHOICE,
  udpsel_OBJ_RULESCHOICE_BUTTON_CUSTOM,
  udpsel_OBJLAST_RULESCHOICE = udpsel_OBJ_RULESCHOICE_BUTTON_CUSTOM,
  udpsel_OBJFIRST_RULES,
  udpsel_OBJ_RULES_TITLE = udpsel_OBJFIRST_RULES,
  udpsel_OBJ_RULES_TEXTOVERLAY,
  udpsel_OBJ_RULES_BUTTON_OKAY,
  udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD,
  udpsel_OBJ_RULES_BUTTON_SHORTGAME,
  udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4,
  udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_5,
  udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_12,
  udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_32,
  udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_60,
  udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_88,
  udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_4,
  udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_12,
  udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_16,
  udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_22,
  udpsel_OBJ_RULES_BUTTON_FREEPARKING_0,
  udpsel_OBJ_RULES_BUTTON_FREEPARKING_250,
  udpsel_OBJ_RULES_BUTTON_FREEPARKING_500,
  udpsel_OBJ_RULES_BUTTON_FREEPARKING_750,
  udpsel_OBJ_RULES_BUTTON_INITIALCASH_500,
  udpsel_OBJ_RULES_BUTTON_INITIALCASH_1000,
  udpsel_OBJ_RULES_BUTTON_INITIALCASH_1500,
  udpsel_OBJ_RULES_BUTTON_INITIALCASH_2000,
  udpsel_OBJ_RULES_BUTTON_SALARY_0,
  udpsel_OBJ_RULES_BUTTON_SALARY_100,
  udpsel_OBJ_RULES_BUTTON_SALARY_200,
  udpsel_OBJ_RULES_BUTTON_SALARY_400,
  udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_0,
  udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_5,
  udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_10,
  udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_15,
  udpsel_OBJ_RULES_BUTTON_FLATTAX_0,
  udpsel_OBJ_RULES_BUTTON_FLATTAX_100,
  udpsel_OBJ_RULES_BUTTON_FLATTAX_200,
  udpsel_OBJ_RULES_BUTTON_FLATTAX_400,
  udpsel_OBJ_RULES_BUTTON_LUXURYTAX_0,
  udpsel_OBJ_RULES_BUTTON_LUXURYTAX_75,
  udpsel_OBJ_RULES_BUTTON_LUXURYTAX_150,
  udpsel_OBJ_RULES_BUTTON_LUXURYTAX_300,
  udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_1,
  udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_2,
  udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_3,
  udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_4,
  udpsel_OBJ_RULES_BUTTON_JAILFEE_0,
  udpsel_OBJ_RULES_BUTTON_JAILFEE_50,
  udpsel_OBJ_RULES_BUTTON_JAILFEE_100,
  udpsel_OBJ_RULES_BUTTON_JAILFEE_200,
  udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_0,
  udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_1,
  udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_6,
  udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_12,
  udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_0,
  udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_1,
  udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_3,
  udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_6,
  udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_0,
  udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_5,
  udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_10,
  udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_20,
  udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_3,
  udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_4,
  udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_5,
  udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_10,
  udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_0,
  udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_2,
  udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_4,
  udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_ALL,
  udpsel_OBJ_RULES_BUTTON_EVENBUILDRULE,
  udpsel_OBJ_RULES_BUTTON_EXTRAGOMONEY,
  udpsel_OBJ_RULES_BUTTON_FREEPARKINGRULE,
  udpsel_OBJ_RULES_BUTTON_FUTURESANDIMMUNITIES,
  udpsel_OBJ_RULES_BUTTON_DEALTPROPERTIESAREFREE,
  udpsel_OBJLAST_RULES = udpsel_OBJ_RULES_BUTTON_DEALTPROPERTIESAREFREE,
  udpsel_OBJ_MAX
};

static udpsel_ObjectStruct udpsel_Objects[udpsel_OBJ_MAX];

#define udpsel_BACKGROUND_PRIORITY                    1000
#define udpsel_FOREGROUND_PRIORITY                    1500


static wchar_t  udpsel_TokenNames[MAX_TOKENS][40];

static int      udpsel_PlayerLogMax;
                    // The number of players that actually exist in the history log.
static int      udpsel_PlayerLogPageStart;
                    // For the 8 that we are actually currently showing on screen,
                    // the first one of these 8's index into the history log.  The 
                    // remaining ones, naturally, immed. follow it in the log.
static wchar_t  udpsel_PlayerLog[udpsel_HISTORYLOG_MAX_PLAYERS][udpsel_MAX_ENTERNAME_LENGTH+2];
                    // A copy of the names read from the ini file.



static RULE_GameOptionsRecord udpsel_PreviousGameOptions;

static BOOL  udpsel_GraphicsShowing = FALSE;
static BOOL  udpsel_AnimInsFinished = FALSE;
static BOOL  udpsel_AnimOutsFinished = FALSE;

static int udpsel_DelayedSwitchView = -1;
              // view (ie DISPLAY_SCREEN_Options) to switch to after the nice anims 
              // are done.

#if USA_VERSION
static char *udpsel_CitiesText[DISPLAY_CITY_MAX] =
{
  "Classic",
  "Atlanta",
  "Boston",
  "Chicago",
  "Dallas",
  "Los Angeles",
  "New York",
  "San Francisco",
  "Seattle",
  "Toronto",
  "Washington",
};
static wchar_t  udpsel_CitiesWideText[DISPLAY_CITY_MAX][100];
#endif

static BOOL     udpsel_WinnerProcessed = FALSE;
                    // Help insure that winner's only get creditted with one
                    // victory and not multiples just because everyone receives
                    // the game over message.

static BOOL     udpsel_WelcomeMessagePlayed = FALSE;
                    // If we skip the hi-score screen we still should play the
                    // welcome voice sound

static int      udpsel_NumberOfPlayers = -1;
                    // Our own copy of how many players are in the game.  Allows us
                    // to ignore messages about number of players that we could get
                    // multiple times even though the count is the same.  Set to
                    // -1 so we don't ignore the count when it is 0.

static int      udpsel_FirstTimeInitializationDone = FALSE;
                    // Take care of some initialization that needs to be done at startup.
                    // The init-ing could get skipped if we were to just look for 
                    // numberofplayers == 0, because you don't always get that message.
                    // For example, when joining a MS Zone game with one or more players
                    // already set to go.


/****************************************************************************/
/* C O D E                                                                  */
/*--------------------------------------------------------------------------*/


/*****************************************************************************
 * All display initialization, showing, hiding and destroying occurs in
 * the following 4 subroutines called from DISPLAY.CPP.

 * No code other than the show module may start nor stop sound
 * and graphic items.  Fields indicating desired actions can be filled in
 * in the display state which the show routine will pick up.  In this way,
 * the show routines can take responsibility for tracking all data
 * required to pause or clear the screen at any time.

 * There will be no locking the computer as a wait for all sequences to finish
 * might do.  Stack processing can be halted until sequences are finished,
 * but you will have to use a timer to poll your sequence status.  This is to
 * allow processing of pause buttons or user camera controls at all times, as
 * well as to make programmers lives more difficult :)
 */


/*****************************************************************************
 */
static int udpsel_NumberOfHumanPlayers( void )
{
  int     i;
  int     humans;

  humans = 0;
  for ( i = 0; i < UICurrentGameState.NumberOfPlayers; i++ )
  {
    if ( UICurrentGameState.Players[i].AIPlayerLevel == 0 )
      humans++;
  }
  return humans;
}


/*****************************************************************************
 * Tests what is showing versus what should be showing and updates accordingly.
 * Also, changes the value of the currentID appropriately.
 */
static void udpsel_GeneralizedAnimUpdate( LE_DATA_DataId *currentID, LE_DATA_DataId desiredID,
 int priority, int startX, int startY, LE_SEQNCR_EndingAction endingAction )
{
  if ( *currentID != desiredID )
  {
    if ( *currentID != LED_EI )
      LE_SEQNCR_Stop( *currentID, priority );
    *currentID = desiredID;
    if ( desiredID != LED_EI )
    {
      LE_SEQNCR_CollectCommands();
      LE_SEQNCR_StartXY( *currentID, priority, startX, startY );
      if ( endingAction != LE_SEQNCR_NoEndingAction )
        LE_SEQNCR_SetEndingAction( *currentID, priority, endingAction );
      LE_SEQNCR_ExecuteCommands();
    }
  }
}


/*****************************************************************************
 */
static int udpsel_ConvertRuleButtonIndexToRuleNumber( int ruleButtonIndex )
{
  int   ruleNumber;

  // Trick is that for the first rule there are two buttons, for the next large bunch
  // there are 4 buttons each and for the last few they are on/offs.
  ruleNumber = -1;
  if ( (ruleButtonIndex >= udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4)
   && (ruleButtonIndex <= udpsel_OBJLAST_RULES) )
  {
    // Convert index into a rule number.  
    // Trick is that for the first rule there are two buttons, for the next large bunch
    // there are 5 buttons each and for the last few they are on/offs.
    if ( (ruleButtonIndex >= udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4)
     && (ruleButtonIndex <= udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_5) )
    {
      ruleNumber = 0;
    }
    else if ( (ruleButtonIndex >= udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_12)
     && (ruleButtonIndex < udpsel_OBJ_RULES_BUTTON_EVENBUILDRULE) )
    {
      ruleNumber = 1 + (ruleButtonIndex - udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_12) / 4;
    }
    else 
    {
      ruleNumber = udpsel_RULESTEXT_EVENBUILDRULE + (ruleButtonIndex - udpsel_OBJ_RULES_BUTTON_EVENBUILDRULE);
    }
  }
  return ruleNumber;
}


/*****************************************************************************
 */
static void udpsel_PrintRemovePlayerTextToOverlay( void )
{
  wchar_t   *wideTextPtr;
  int       x,    y;

  if ( udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].idleID != LED_EI )
  {
    LE_FONTS_SetSize( 2*udpsel_FONTPOINTSIZE );
    LE_FONTS_SetWeight( udpsel_FONTWEIGHT );
    // Clear tab, make it transparent
    LE_GRAFIX_ColorArea( udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].idleID, 0, 0,
     (short)(udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect.left),
     (short)(udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect.top),
     LEG_MCR(0, 255, 0) );
    // Print the "Click at bottom to remove" text.
    wideTextPtr = LANG_GetTextMessage(TMN_REMOVE_PLAYER);
    x = ((udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect.left)
     - LANG_GetStringWidth(wideTextPtr))/2;
    y = 0;
    LANG_Print( udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].idleID, x, y, LEG_MCR(255, 255, 255),
     wideTextPtr );
    // Print the "OR" text (which is above the cancel button).
    wideTextPtr = LANG_GetTextMessage(TMN_OR);
    x = ((udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect.left)
     - LANG_GetStringWidth(wideTextPtr ))/2;
    y = 60;
    LANG_Print( udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].idleID, x, y, LEG_MCR(255, 255, 255),
     wideTextPtr );
  }
}


/*****************************************************************************
 * Given a string, a font, and pixel limit for a width, copies the source
 * string to a two dimensional array of characters, where each row represents
 * the portions of the string as they need to be wrapped to fit.  The caller
 * must provide the space for the wrapped lines.
 * Returns the number of lines that are wrapped in the destination buffer.
 * Note:  Whitespace test only checks for the space character.
 */
static int udpsel_WordWrap( wchar_t *sourceLine, int maxPixelWidth,
 wchar_t *wrappedBuffer, int numberOfLinesInBuffer, int charactersPerLineInBuffer )
{
  int     currentWidth;
  int     rowCount;
  wchar_t *previousPtr,     *currentPtr;
  wchar_t *sourcePtr,       *endOfSourcePtr;
  BOOL    segmentFits;

  rowCount = 0;
  sourcePtr = sourceLine;
  endOfSourcePtr = sourcePtr + wcslen( sourcePtr );
  // While the line is too big, chop it up to make it fit
  while ( (sourcePtr < endOfSourcePtr) && (rowCount < numberOfLinesInBuffer) )
  {
    // Search for a portion of the remaining text that will fit.
    previousPtr = NULL;
    currentPtr = NULL;
    segmentFits = TRUE;
    while ( LE_FONTS_GetStringWidth(sourcePtr) > maxPixelWidth )
    {
      // Find the (next) last bit of whitespace.
      currentPtr = wcsrchr( sourcePtr, L' ' );

      if ( currentPtr == NULL )
      {
        // No whitespace anywhere
        segmentFits = FALSE;
        break;
      }

      // Restore the string if we mucked with it before.
      if ( previousPtr != NULL )
        *previousPtr = L' ';

      // Found some whitespace.  Terminate the string here in order to calculate the 
      // length of this portion.
      *currentPtr = NULL;
      previousPtr = currentPtr;
    }

    // Determine what the result of our search for whitespace was.
    if ( segmentFits )
    {
      // Found a good candidate.  Copy to our buffer and move along..
      wcsncpy( wrappedBuffer, sourcePtr, charactersPerLineInBuffer - 1 );
      *(wrappedBuffer + charactersPerLineInBuffer - 1) = (wchar_t)NULL;

      // Restore the string if we mucked with it before.
      if ( previousPtr != NULL )
        *previousPtr = L' ';

      if ( currentPtr != NULL )
      {
        sourcePtr = currentPtr + 1;
      }
      else
      {
        // Not obvious but, if segmentFits is TRUE and currentPtr is NULL,
        // then this is the last piece.  So, move sourcePtr past the end
        // so we can quit this loop.
        sourcePtr += wcslen( sourcePtr );
      }
    }
    else
    {
      int     i;
      wchar_t savedChar;

      // No whitespace in a portion that will fit, so do an ugly break in the middle 
      // of something.  

      // Restore the string if we mucked with it before.
      if ( previousPtr != NULL )
        *previousPtr = L' ';

      // Start at the end of the string or the first whitespace if there is one -- 
      // we know already that it is too big.
      currentPtr = wcschr( sourcePtr, L' ' );
      if ( currentPtr != NULL )
        i = currentPtr - sourcePtr;
      else
        i = wcslen( sourcePtr ) - 1;

      // Keep trying versions that are one character less until we're under the limit.
      do
      {
        if ( i <= 0 )
          break;
        // Remember the character.
        savedChar = *(sourcePtr + i);
        // Overwrite the character and terminate here.
        *(sourcePtr + i) = (wchar_t)NULL;
        // Remember width.
        currentWidth = LE_FONTS_GetStringWidth( sourcePtr );
        // Restore the character.
        *(sourcePtr + i) = savedChar;
        // Prepare for next iteration.
        i--;
      }
      while ( currentWidth > maxPixelWidth  );

      if ( i >= 0 )
      {
        // i + 1 is the magic point.  Copy that portion to our buffer and move along.
        wcsncpy( wrappedBuffer, sourcePtr, i + 1 );
        *(wrappedBuffer + i + 1) = (wchar_t)NULL;
      }
      else
      {
        i = 0;
      }
      sourcePtr += i + 1;
    }
    rowCount++;
    wrappedBuffer += charactersPerLineInBuffer;
  }

  return rowCount;
}


#if compilefor_TESTWORDWRAP
/*****************************************************************************
 */
static void UDPSEL_TestWordWrapSingle( wchar_t *textPtr, int width, wchar_t *wrapBuffer,
 int allowedRows, int allowedChars )
{
  int       i;
  int       rows;
  char      outBuffer[1024];

  TRACE( "\n" );
  // Print original
  wcstombs( outBuffer, textPtr, 1024 );
  TRACE( outBuffer );
  TRACE( "\n" );
  // Do function call.
  rows = udpsel_WordWrap( textPtr, width, wrapBuffer, allowedRows, allowedChars );
  // Print parameters and return value.
  sprintf( outBuffer, "pixelWidth=%d rows=%d allowedRows=%d allowedChars=%d\n",
   width, rows, allowedRows, allowedChars );
  TRACE( outBuffer );
  // Reprint original to make sure it didn't get changed by the call.
  wcstombs( outBuffer, textPtr, 1024 );
  TRACE( outBuffer );
  TRACE( "\n\n" );
  // Print word wrap output.
  for ( i = 0; i < rows; i++ )
  {
    wcstombs( outBuffer, wrapBuffer + i*allowedChars, 1024 );
    TRACE( outBuffer );
    TRACE( "\n" );
  }
  TRACE( "\n\n\n" );
}


/*****************************************************************************
 */
static void UDPSEL_TestWordWrap( void )
{
  wchar_t   wrapBuffer[50][1024];

  UDPSEL_TestWordWrapSingle( A_T("The quick brown fox jumps over a lazy dog"),
   100, &wrapBuffer[0][0], 10, 25 );
  UDPSEL_TestWordWrapSingle( A_T("The quick brown fox jumps over a lazy dog"),
   10, &wrapBuffer[0][0], 10, 25 );
  UDPSEL_TestWordWrapSingle( A_T("The quick brown fox jumps over a lazy dog"),
   120, &wrapBuffer[0][0], 10, 5 );
  UDPSEL_TestWordWrapSingle( A_T("Xanadu"),
   120, &wrapBuffer[0][0], 10, 5 );
  UDPSEL_TestWordWrapSingle( A_T("Xanadu"),
   120, &wrapBuffer[0][0], 10, 50 );
}
#endif

/*****************************************************************************
 * Create the transparent graphic with al the rules text on it.
 */
static void udpsel_PrintRulesTextToOverlay( void )
{
  int       i,    j;
  int       x,    y;
  int       w,    h;
  wchar_t   buttonWideText[100];
  int       descriptionTextIndex;
  wchar_t   *wideTextPtr;

  if ( udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].idleID != LED_EI )
  {
    // Set the height of the font.  This is smaller than the default.
    LE_FONTS_SetSize( udpsel_FONTPOINTSIZE );
    LE_FONTS_SetWeight( udpsel_FONTWEIGHT );

    // Clear tab, make it transparent
    LE_GRAFIX_ColorArea( udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].idleID, 0, 0,
     (short)(udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.left),
     (short)(udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.top),
     LEG_MCR(0, 255, 0) );

    // Now write in the all the rules text.
    for ( i = udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4; i < udpsel_OBJLAST_RULES; i++ )
    {
      descriptionTextIndex = -1;
      buttonWideText[0] = NULL;
      switch ( i )
      {
      case udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4:
        mbstowcs( buttonWideText, "4", 100 );
        descriptionTextIndex = udpsel_RULESTEXT_HOUSESPERHOTEL;
        break;
      case udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_5:
        mbstowcs( buttonWideText, "5", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_12:
        mbstowcs( buttonWideText, "12", 100 );
        descriptionTextIndex = udpsel_RULESTEXT_TOTALHOUSES;
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_32:
        mbstowcs( buttonWideText, "32", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_60:
        mbstowcs( buttonWideText, "60", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_88:
        mbstowcs( buttonWideText, "88", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_4:
        mbstowcs( buttonWideText, "4", 100 );
        descriptionTextIndex = udpsel_RULESTEXT_TOTALHOTELS;
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_12:
        mbstowcs( buttonWideText, "12", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_16:
        mbstowcs( buttonWideText, "16", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_22:
        mbstowcs( buttonWideText, "22", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_FREEPARKING_0:
        descriptionTextIndex = udpsel_RULESTEXT_FREEPARKINGJACKPOTAMOUNT;
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 0, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_FREEPARKING_250:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 250, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_FREEPARKING_500:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 500, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_FREEPARKING_750:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 750, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_INITIALCASH_500:
        descriptionTextIndex = udpsel_RULESTEXT_INITIALCASH;
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 500, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_INITIALCASH_1000:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 1000, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_INITIALCASH_1500:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 1500, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_INITIALCASH_2000:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 2000, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_SALARY_0:
        descriptionTextIndex = udpsel_RULESTEXT_SALARY;
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 0, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_SALARY_100:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 100, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_SALARY_200:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 200, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_SALARY_400:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 400, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_0:
        descriptionTextIndex = udpsel_RULESTEXT_INCOMETAXRATE;
        mbstowcs( buttonWideText, "0", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_5:
        mbstowcs( buttonWideText, "5", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_10:
        mbstowcs( buttonWideText, "10", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_15:
        mbstowcs( buttonWideText, "15", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_FLATTAX_0:
        descriptionTextIndex = udpsel_RULESTEXT_INCOMETAXAMOUNT;
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 0, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_FLATTAX_100:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 100, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_FLATTAX_200:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 200, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_FLATTAX_400:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 400, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_LUXURYTAX_0:
        descriptionTextIndex = udpsel_RULESTEXT_LUXURYTAX;
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 0, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_LUXURYTAX_75:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 75, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_LUXURYTAX_150:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 150, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_LUXURYTAX_300:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 300, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_1:
        descriptionTextIndex = udpsel_RULESTEXT_MAXIMUMTURNSINJAIL;
        mbstowcs( buttonWideText, "1", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_2:
        mbstowcs( buttonWideText, "2", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_3:
        mbstowcs( buttonWideText, "3", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_4:
        mbstowcs( buttonWideText, "4", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_JAILFEE_0:
        descriptionTextIndex = udpsel_RULESTEXT_JAILFEE;
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 0, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_JAILFEE_50:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 50, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_JAILFEE_100:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 100, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_JAILFEE_200:
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( 200, DISPLAY_state.system, buttonWideText, FALSE );
        break;
      case udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_0:
        descriptionTextIndex = udpsel_RULESTEXT_HOUSESREMAININGFORSHORTAGE;
        mbstowcs( buttonWideText, "0", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_1:
        mbstowcs( buttonWideText, "1", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_6:
        mbstowcs( buttonWideText, "6", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_12:
        mbstowcs( buttonWideText, "12", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_0:
        descriptionTextIndex = udpsel_RULESTEXT_HOTELSREMAININGFORSHORTAGE;
        mbstowcs( buttonWideText, "0", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_1:
        mbstowcs( buttonWideText, "1", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_3:
        mbstowcs( buttonWideText, "3", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_6:
        mbstowcs( buttonWideText, "6", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_0:
        descriptionTextIndex = udpsel_RULESTEXT_MORTGAGEINTERESTRATE;
        mbstowcs( buttonWideText, "0", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_5:
        mbstowcs( buttonWideText, "5", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_10:
        mbstowcs( buttonWideText, "10", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_20:
        mbstowcs( buttonWideText, "20", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_3:
        descriptionTextIndex = udpsel_RULESTEXT_AUCTIONTIMEDELAYINSECONDS;
        mbstowcs( buttonWideText, "3", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_4:
        mbstowcs( buttonWideText, "4", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_5:
        mbstowcs( buttonWideText, "5", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_10:
        mbstowcs( buttonWideText, "10", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_0:
        descriptionTextIndex = udpsel_RULESTEXT_PROPERTIESDEALTATSTART;
        mbstowcs( buttonWideText, "0", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_2:
        mbstowcs( buttonWideText, "2", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_4:
        mbstowcs( buttonWideText, "4", 100 );
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_ALL:
        wcscpy( buttonWideText, LANG_GetTextMessage(TMN_ALL) );
        break;
      case udpsel_OBJ_RULES_BUTTON_EVENBUILDRULE:
        descriptionTextIndex = udpsel_RULESTEXT_EVENBUILDRULE;
        break;
      case udpsel_OBJ_RULES_BUTTON_EXTRAGOMONEY:
        descriptionTextIndex = udpsel_RULESTEXT_COLLECTDOUBLEMONEYFORLANDINGONGO;
        break;
      case udpsel_OBJ_RULES_BUTTON_FREEPARKINGRULE:
        descriptionTextIndex = udpsel_RULESTEXT_FREEPARKINGRULE;
        break;
      case udpsel_OBJ_RULES_BUTTON_FUTURESANDIMMUNITIES:
        descriptionTextIndex = udpsel_RULESTEXT_FUTURESANDIMMUNITIES;
        break;
      case udpsel_OBJ_RULES_BUTTON_DEALTPROPERTIESAREFREE:
        descriptionTextIndex = udpsel_RULESTEXT_DEALTPROPERTIESAREFREE;
        break;
      }
      // Print button text to memory graphic.
      if ( buttonWideText[0] != NULL )
      {
        w = LANG_GetStringWidth( buttonWideText );
        h = LANG_GetStringHeight( buttonWideText );
        x = (udpsel_Objects[i].rect.right - udpsel_Objects[i].rect.left - w)/2;
        y = (udpsel_Objects[i].rect.bottom - udpsel_Objects[i].rect.top - h)/2;
        LANG_Print( udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].idleID,
         udpsel_Objects[i].rect.left - udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.left + x,
         udpsel_Objects[i].rect.top - udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.top + y,
         LEG_MCR(255, 255, 255),
         buttonWideText );
      }
      // Print rule description for a row of buttons
      if ( descriptionTextIndex >= 0 )
      {
        wchar_t   wordWrapBuffer[2][256];
        int       rowsWordWrapped;

        if ( udpsel_RulesTextPosition[descriptionTextIndex].x == udpsel_RULESBUTTONCOLUMN1X )
          x = udpsel_RULESDESCRIPTIONCOLUMN1X;
        else
          x = udpsel_RULESDESCRIPTIONCOLUMN2X;
        y = udpsel_RulesTextPosition[descriptionTextIndex].y;

//        rowsWordWrapped = udpsel_WordWrap( udpsel_RulesWideText[descriptionTextIndex],
        rowsWordWrapped = udpsel_WordWrap( LANG_GetTextMessage(udpsel_RulesWideIndex[descriptionTextIndex]),
         udpsel_RULESDESCRIPTIONTEXTWIDTH, &wordWrapBuffer[0][0], 2, 256 );
        for ( j = 0; j < rowsWordWrapped; j++ )
        {
          LANG_Print( udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].idleID,
           (short)(x - udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.left),
           (short)(y - udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.top),
           LEG_MCR(255, 255, 255),
           &wordWrapBuffer[j][0] );
          y += udpsel_RULESROWHEIGHT - 4; // Space text a bit less than the buttons
        }
      }
    }

    // Host establishes rules message for non-host machines.
    if ( !MESS_ServerMode )
    {
      wideTextPtr = LANG_GetTextMessage(TMN_HOST_IS_CHANGING_THE_RULES);
      w = LANG_GetStringWidth( wideTextPtr );
      h = LANG_GetStringHeight( wideTextPtr );
      x = (udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.left)/2;
      x -= w/2;
      y = udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.top;
      y -= h + 5;
      LANG_Print( udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].idleID,
       x, y, LEG_MCR(255, 255, 255), wideTextPtr );
    }

    // Reset all font characteristics (ie the size that we changed up top) to the default values.
    LE_FONTS_ResetCharacteristics();
  }
}


/*****************************************************************************
 * Called from the standard DISPLAY_UDPSEL_Initialize() at the beginning of the
 * program.  Sets all graphic objects with initial or permanent values.
 */
static void udpsel_InitializeObjects( void )
{
  int   i;
  int   x,    y;
  int   ruleNumber;

  DISPLAY_state.setupPhaseDesiredSoundIndex = NULL;
  DISPLAY_state.setupPhasePlayingSoundIndex = NULL;
  DISPLAY_state.setupPhaseSoundClipAction = Sound_ClipOldSoundIfPlaying;
  // Set defaults for all objects.
  for ( i = 0; i < udpsel_OBJ_MAX; i++ )
  {
    udpsel_Objects[i].currentID = LED_EI;
    udpsel_Objects[i].desiredID = LED_EI;
    udpsel_Objects[i].idleID = LED_EI;
    udpsel_Objects[i].pressID = LED_EI;
    udpsel_Objects[i].animInID = LED_EI;
    udpsel_Objects[i].animOutID = LED_EI;
    udpsel_Objects[i].postAnimInID = LED_EI;
    udpsel_Objects[i].postAnimOutID = LED_EI;
    udpsel_Objects[i].priority = udpsel_BACKGROUND_PRIORITY;
    udpsel_Objects[i].startX = 0;
    udpsel_Objects[i].startY = 0;
    udpsel_Objects[i].endingAction = LE_SEQNCR_EndingActionStayAtEnd;
    SetRect( &udpsel_Objects[i].rect, 0, 0, 0, 0 );
    udpsel_Objects[i].hotspotEnabled = FALSE;
  }

  // Hi Score screen.
  udpsel_Objects[udpsel_OBJ_HISCORE_STATIC].animInID = LED_EI;
  udpsel_Objects[udpsel_OBJ_HISCORE_STATIC].animOutID = LED_EI;
  udpsel_Objects[udpsel_OBJ_HISCORE_STATIC].idleID = LED_IFT(DAT_LANG2,CNK_sytplyr2);
  SetRect( &udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].rect, 9, 170, 9+779, 170+279 );
  udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].idleID = LE_GRAFIX_ObjectCreate(
   (short)(udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].rect.left),
   (short)(udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].rect.top),
   LE_GRAFIX_ObjectTransparent );
  udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].priority = udpsel_FOREGROUND_PRIORITY;
  udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].startX = udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].rect.left;
  udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].startY = udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].rect.top;
  udpsel_Objects[udpsel_OBJ_HISCORE_BUTTON_PLAY].animInID = LED_IFT(DAT_LANG2,CNK_syplay1);
  udpsel_Objects[udpsel_OBJ_HISCORE_BUTTON_PLAY].animOutID = LED_IFT(DAT_LANG2,CNK_syplay0);
  udpsel_Objects[udpsel_OBJ_HISCORE_BUTTON_PLAY].idleID = LED_IFT(DAT_LANG2,CNK_syplay2);
  SetRect( &udpsel_Objects[udpsel_OBJ_HISCORE_BUTTON_PLAY].rect, 341, 452, 341+127, 452+36 );

  // Local or On-line game? screen.
  udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_LOCAL].animInID = LED_IFT(DAT_LANG2,CNK_sypagm01);
  udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_LOCAL].animOutID = LED_IFT(DAT_LANG2,CNK_sypagm00);
  udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_LOCAL].idleID = LED_IFT(DAT_LANG2,CNK_sypagm02);
  SetRect( &udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_LOCAL].rect, 291, 222, 291+220, 222+62 );
  udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_NETWORK].animInID = LED_IFT(DAT_LANG2,CNK_sypogm01);
  udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_NETWORK].animOutID = LED_IFT(DAT_LANG2,CNK_sypogm00);
  udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_NETWORK].idleID = LED_IFT(DAT_LANG2,CNK_sypogm02);
  SetRect( &udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_NETWORK].rect, 291, 294, 291+220, 294+62 );
  udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_SAVED].animInID = LED_IFT(DAT_LANG2,CNK_sylsgm01);
  udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_SAVED].animOutID = LED_IFT(DAT_LANG2,CNK_sylsgm00);
  udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_SAVED].idleID = LED_IFT(DAT_LANG2,CNK_sylsgm02);
  SetRect( &udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_SAVED].rect, 291, 366, 291+220, 366+62 );

  // Select a player screen.
  udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TITLE].idleID = LED_IFT(DAT_LANG2,CNK_sypsyn);
  udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_NEW].animInID = LED_IFT(DAT_LANG2,CNK_synwplr1);
  udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_NEW].animOutID = LED_IFT(DAT_LANG2,CNK_synwplr0);
  udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_NEW].idleID = LED_IFT(DAT_LANG2,CNK_synwplr2);
  SetRect( &udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_NEW].rect, 547, 241, 547+129, 241+147 );
  udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_MORE].animInID = LED_IFT(DAT_LANG2,CNK_symrnms1);
  udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_MORE].animOutID = LED_IFT(DAT_LANG2,CNK_symrnms0);
  udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_MORE].idleID = LED_IFT(DAT_LANG2,CNK_symrnms2);
  SetRect( &udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_MORE].rect, 506, 400, 506+220, 400+62 );
  SetRect( &udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1].rect, 35, 253, 35+97, 253+110 );
  SetRect( &udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD2].rect, 141, 253, 141+97, 253+110 );
  SetRect( &udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD3].rect, 241, 253, 241+97, 253+110 );
  SetRect( &udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD4].rect, 344, 253, 344+97, 253+110 );
  SetRect( &udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD5].rect, 36, 370, 36+97, 370+110 );
  SetRect( &udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD6].rect, 141, 371, 141+97, 371+110 );
  SetRect( &udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD7].rect, 242, 371, 242+97, 371+110 );
  SetRect( &udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD8].rect, 345, 372, 345+97, 372+110 );
  for ( i = 0; i < 8; i++ )
  {
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].animInID = LED_IFT(DAT_PAT,CNK_sn3greyf);
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].animOutID = LED_IFT(DAT_PAT,CNK_sn3greyo);
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].idleID = LED_IFT(DAT_PAT,CNK_sn3greyi);
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].priority = udpsel_FOREGROUND_PRIORITY + i;
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].startX = udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].rect.left - 358;
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].startY = udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].rect.top - 255;
  }

  // Transparent graphics for names of past players.  Gets superimposed over cards.
  for ( i = 0; i < 8; i++ )
  {
    SetRect( &udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect,
     udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].rect.left + 13,
     udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].rect.top + 16,
     udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].rect.left + 80,
     udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].rect.top + 30 );
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].idleID = LE_GRAFIX_ObjectCreate(
     (short)(udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.right - udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.left),
     (short)(udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.bottom - udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.top),
     LE_GRAFIX_ObjectTransparent );
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].priority = udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].priority + 1;
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].startX = udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.left;
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].startY = udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.top;
  }

  // Enter your name screen.
  udpsel_Objects[udpsel_OBJ_ENTERNAME_TITLE].idleID = LED_IFT(DAT_LANG2,CNK_sypeyn);
  // Special transparent graphic for name.  Text will be drawn into this by code.
  SetRect( &udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect, 153, 361, 153+496, 361+54 );
  udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].idleID = LE_GRAFIX_ObjectCreate(
   (short)(udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect.left),
   (short)(udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect.top),
   LE_GRAFIX_ObjectTransparent );
  udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].priority = udpsel_FOREGROUND_PRIORITY;
  udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].startX = udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect.left;
  udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].startY = udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect.top;
  udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTBOX].animInID = LED_IFT(DAT_LANG2,CNK_syentnm0);
  udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTBOX].animOutID = LED_IFT(DAT_LANG2,CNK_syentnm1);
  udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTBOX].idleID = LED_IFT(DAT_LANG2,CNK_syentnm2);
  udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].animInID = LED_EI;
  udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].animOutID = LED_EI;
  udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].idleID = LED_IFT(DAT_LANG2,CNK_synext2);
  SetRect( &udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].rect, 341, 452-18, 341+127, 452-18+36 );
  udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].startY = -18;

  // Pick a token screen.
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_TITLE].idleID = LED_IFT(DAT_LANG2,CNK_sypsyt);
  // Background to the rotating image of the currently selected token
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_STATIC_ROTATING].animInID = LED_IFT(DAT_PAT,CNK_sn5st00f);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_STATIC_ROTATING].animOutID = LED_IFT(DAT_PAT,CNK_sn5st00o);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_STATIC_ROTATING].idleID = LED_IFT(DAT_PAT,CNK_sn5st00i);
  // Not more than one of these rotating tokens is ever shown at the same time.
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_CANNON].idleID = LED_IFT(DAT_PAT,CNK_sn5rot00);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_IRON].idleID = LED_IFT(DAT_PAT,CNK_sn5rot04);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_THIMBLE].idleID = LED_IFT(DAT_PAT,CNK_sn5rot08);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_RACECAR].idleID = LED_IFT(DAT_PAT,CNK_sn5rot01);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_HORSE].idleID = LED_IFT(DAT_PAT,CNK_sn5rot05);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_WHEELBARROW].idleID = LED_IFT(DAT_PAT,CNK_sn5rot09);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_DOG].idleID = LED_IFT(DAT_PAT,CNK_sn5rot02);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_BATTLESHIP].idleID = LED_IFT(DAT_PAT,CNK_sn5rot06);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_SACKOFMONEY].idleID = LED_IFT(DAT_PAT,CNK_sn5rot10);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_TOPHAT].idleID = LED_IFT(DAT_PAT,CNK_sn5rot03);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_SHOE].idleID = LED_IFT(DAT_PAT,CNK_sn5rot07);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_CANNON].animInID = LED_IFT(DAT_PAT,CNK_sn5st08f);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_CANNON].animOutID = LED_IFT(DAT_PAT,CNK_sn5st08o);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_CANNON].idleID = LED_IFT(DAT_PAT,CNK_sn5st08i);
  SetRect( &udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_CANNON].rect, 77, 252, 77+100, 252+41 );
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_IRON].animOutID = LED_IFT(DAT_PAT,CNK_sn5st12o);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_IRON].idleID = LED_IFT(DAT_PAT,CNK_sn5st12i);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_IRON].animInID = LED_IFT(DAT_PAT,CNK_sn5st12f);
  SetRect( &udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_IRON].rect, 225, 252, 225+100, 252+41 );
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_THIMBLE].animInID = LED_IFT(DAT_PAT,CNK_sn5st07f);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_THIMBLE].animOutID = LED_IFT(DAT_PAT,CNK_sn5st07o);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_THIMBLE].idleID = LED_IFT(DAT_PAT,CNK_sn5st07i);
  SetRect( &udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_THIMBLE].rect, 376, 252, 376+100, 252+41 );
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_RACECAR].animInID = LED_IFT(DAT_PAT,CNK_sn5st13f);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_RACECAR].animOutID = LED_IFT(DAT_PAT,CNK_sn5st13o);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_RACECAR].idleID = LED_IFT(DAT_PAT,CNK_sn5st13i);
  SetRect( &udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_RACECAR].rect, 77, 311, 77+100, 311+41 );
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_HORSE].animInID = LED_IFT(DAT_PAT,CNK_sn5st11f);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_HORSE].animOutID = LED_IFT(DAT_PAT,CNK_sn5st11o);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_HORSE].idleID = LED_IFT(DAT_PAT,CNK_sn5st11i);
  SetRect( &udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_HORSE].rect, 225, 311, 225+100, 311+41 );
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_WHEELBARROW].animInID = LED_IFT(DAT_PAT,CNK_sn5st10f);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_WHEELBARROW].animOutID = LED_IFT(DAT_PAT,CNK_sn5st10o);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_WHEELBARROW].idleID = LED_IFT(DAT_PAT,CNK_sn5st10i);
  SetRect( &udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_WHEELBARROW].rect, 376, 311, 376+100, 311+41 );
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_DOG].animInID = LED_IFT(DAT_PAT,CNK_sn5st03f);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_DOG].animOutID = LED_IFT(DAT_PAT,CNK_sn5st03o);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_DOG].idleID = LED_IFT(DAT_PAT,CNK_sn5st03i);
  SetRect( &udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_DOG].rect, 77, 371, 77+100, 371+41 );
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_BATTLESHIP].animInID = LED_IFT(DAT_PAT,CNK_sn5st09f);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_BATTLESHIP].animOutID = LED_IFT(DAT_PAT,CNK_sn5st09o);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_BATTLESHIP].idleID = LED_IFT(DAT_PAT,CNK_sn5st09i);
  SetRect( &udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_BATTLESHIP].rect, 225, 371, 225+100, 371+41 );
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_SACKOFMONEY].animInID = LED_IFT(DAT_PAT,CNK_sn5st06f);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_SACKOFMONEY].animOutID = LED_IFT(DAT_PAT,CNK_sn5st06o);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_SACKOFMONEY].idleID = LED_IFT(DAT_PAT,CNK_sn5st06i);
  SetRect( &udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_SACKOFMONEY].rect, 376, 371, 376+100, 371+41 );
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_TOPHAT].animInID = LED_IFT(DAT_PAT,CNK_sn5st04f);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_TOPHAT].animOutID = LED_IFT(DAT_PAT,CNK_sn5st04o);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_TOPHAT].idleID = LED_IFT(DAT_PAT,CNK_sn5st04i);
  SetRect( &udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_TOPHAT].rect, 77, 432, 77+100, 432+41 );
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_SHOE].animInID = LED_IFT(DAT_PAT,CNK_sn5st05f);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_SHOE].animOutID = LED_IFT(DAT_PAT,CNK_sn5st05o);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_SHOE].idleID = LED_IFT(DAT_PAT,CNK_sn5st05i);
  SetRect( &udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_SHOE].rect, 225, 432, 225+100, 432+41 );
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_NEXT].animInID = LED_IFT(DAT_LANG2,CNK_synext1);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_NEXT].animOutID = LED_IFT(DAT_LANG2,CNK_synext0);
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_NEXT].idleID = LED_IFT(DAT_LANG2,CNK_synext2);
  SetRect( &udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_NEXT].rect, 341+21, 452-18, 341+21+127, 452-18+36 );
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_NEXT].startX = 21;
  udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_NEXT].startY = -18;
  for ( i = 0; i < MAX_TOKENS; i++ )
  {
    udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_CANNON + i].priority = udpsel_FOREGROUND_PRIORITY + i;
    udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_CANNON + i].priority = udpsel_FOREGROUND_PRIORITY + i;
  }

  // Start screen with add human/computer and remove player buttons.
  udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDHUMAN].animInID = LED_IFT(DAT_LANG2,CNK_syahplr1);
  udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDHUMAN].animOutID = LED_IFT(DAT_LANG2,CNK_syahplr0);
  udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDHUMAN].idleID = LED_IFT(DAT_LANG2,CNK_syahplr2);
  SetRect( &udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDHUMAN].rect, 40, 254, 40+220, 254+62 );
  udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDCOMPUTER].animInID = LED_IFT(DAT_LANG2,CNK_syacplr1);
  udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDCOMPUTER].animOutID = LED_IFT(DAT_LANG2,CNK_syacplr0);
  udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDCOMPUTER].idleID = LED_IFT(DAT_LANG2,CNK_syacplr2);
  SetRect( &udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDCOMPUTER].rect, 545, 255, 545+220, 255+62 );
  udpsel_Objects[udpsel_OBJ_START_BUTTON_REMOVEPLAYER].animInID = LED_IFT(DAT_LANG2,CNK_syrmplr1);
  udpsel_Objects[udpsel_OBJ_START_BUTTON_REMOVEPLAYER].animOutID = LED_IFT(DAT_LANG2,CNK_syrmplr0);
  udpsel_Objects[udpsel_OBJ_START_BUTTON_REMOVEPLAYER].idleID = LED_IFT(DAT_LANG2,CNK_syrmplr2);
  SetRect( &udpsel_Objects[udpsel_OBJ_START_BUTTON_REMOVEPLAYER].rect, 290, 252+3, 290+220, 252+3+62 );
  udpsel_Objects[udpsel_OBJ_START_BUTTON_REMOVEPLAYER].startY = 3;
  udpsel_Objects[udpsel_OBJ_START_BUTTON_STARTGAME].animInID = LED_IFT(DAT_LANG2,CNK_systart1);
  udpsel_Objects[udpsel_OBJ_START_BUTTON_STARTGAME].animOutID = LED_IFT(DAT_LANG2,CNK_systart0);
  udpsel_Objects[udpsel_OBJ_START_BUTTON_STARTGAME].idleID = LED_IFT(DAT_LANG2,CNK_systart2);
  SetRect( &udpsel_Objects[udpsel_OBJ_START_BUTTON_STARTGAME].rect, 290, 328, 290+220, 328+62 );

  // Remove player screen.
  udpsel_Objects[udpsel_OBJ_REMOVE_BUTTON_CANCEL].animInID = LED_IFT(DAT_LANG2,CNK_syfcnxl1);
  udpsel_Objects[udpsel_OBJ_REMOVE_BUTTON_CANCEL].animOutID = LED_IFT(DAT_LANG2,CNK_syfcnxl0);
  udpsel_Objects[udpsel_OBJ_REMOVE_BUTTON_CANCEL].idleID = LED_IFT(DAT_LANG2,CNK_syfcnxl2);
  SetRect( &udpsel_Objects[udpsel_OBJ_REMOVE_BUTTON_CANCEL].rect, 291, 401, 291+220, 401+62 );
  SetRect( &udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect, 100, 281, 100+600, 281+100 );
  udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].idleID = LE_GRAFIX_ObjectCreate(
   (short)(udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect.left),
   (short)(udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect.top),
   LE_GRAFIX_ObjectTransparent );
  udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].priority = udpsel_FOREGROUND_PRIORITY;
  udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].startX = udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect.left;
  udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].startY = udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].rect.top;

  // AI strength screen.
  udpsel_Objects[udpsel_OBJ_AISTRENGTH_TITLE].idleID = LED_IFT(DAT_LANG2,CNK_sypcycps);
  udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_EASY].animInID = LED_IFT(DAT_LANG2,CNK_syftbyr1);
  udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_EASY].animOutID = LED_IFT(DAT_LANG2,CNK_syftbyr0);
  udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_EASY].idleID = LED_IFT(DAT_LANG2,CNK_syftbyr2);
  SetRect( &udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_EASY].rect, 52, 254, 52+220, 254+62 );
  udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_MEDIUM].animInID = LED_IFT(DAT_LANG2,CNK_syentrp1);
  udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_MEDIUM].animOutID = LED_IFT(DAT_LANG2,CNK_syentrp0);
  udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_MEDIUM].idleID = LED_IFT(DAT_LANG2,CNK_syentrp2);
  SetRect( &udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_MEDIUM].rect, 291, 254, 291+220, 254+62 );
  udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_HARD].animInID = LED_IFT(DAT_LANG2,CNK_sytycoo1);
  udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_HARD].animOutID = LED_IFT(DAT_LANG2,CNK_sytycoo0);
  udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_HARD].idleID = LED_IFT(DAT_LANG2,CNK_sytycoo2);
  SetRect( &udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_HARD].rect, 532, 254, 532+220, 254+62 );

  // Select Country/Currency (also classic or load custom board) screen.
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_CLASSICBOARD].animInID = LED_IFT(DAT_LANG2,CNK_syclbrd1);
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_CLASSICBOARD].animOutID = LED_IFT(DAT_LANG2,CNK_syclbrd0);
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_CLASSICBOARD].idleID = LED_IFT(DAT_LANG2,CNK_syclbrd2);
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_CLASSICBOARD].startY = -40;
  SetRect( &udpsel_Objects[udpsel_OBJ_CITY_BUTTON_CLASSICBOARD].rect, 73, 254-40, 73+220, 254-40+62 );
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LOADBOARD].animInID = LED_IFT(DAT_LANG2,CNK_sylcbrd1);
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LOADBOARD].animOutID = LED_IFT(DAT_LANG2,CNK_sylcbrd0);
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LOADBOARD].idleID = LED_IFT(DAT_LANG2,CNK_sylcbrd2);
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LOADBOARD].startY = -40;
  SetRect( &udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LOADBOARD].rect, 507, 254-40, 507+220, 254-40+62 );
#if USA_VERSION
  udpsel_Objects[udpsel_OBJ_CITY_STATIC].animInID = LED_IFT(DAT_LANG2,CNK_sy8cityf);
  udpsel_Objects[udpsel_OBJ_CITY_STATIC].animOutID = LED_IFT(DAT_LANG2,CNK_sy8cityo);
  udpsel_Objects[udpsel_OBJ_CITY_STATIC].idleID = LED_IFT(DAT_LANG2,CNK_sy8cityi);
  SetRect( &udpsel_Objects[udpsel_OBJ_CITY_STATIC].rect, 295, 294, 295+210, 294+133 );
  udpsel_Objects[udpsel_OBJ_CITY_STATIC].priority = udpsel_FOREGROUND_PRIORITY;
  SetRect( &udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect, 335, 408, 465, 408+13 );
  udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].idleID = LE_GRAFIX_ObjectCreate(
   (short)(udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect.left),
   (short)(udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect.top),
   LE_GRAFIX_ObjectTransparent );
  udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].priority = udpsel_FOREGROUND_PRIORITY;
  udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].startX = udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect.left;
  udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].startY = udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect.top;
  // For clicks on the arrow buttons just play an anim once, then return to idle image.
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LEFT].animInID = LED_IFT(DAT_PAT,CNK_sn8alcyp);
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LEFT].idleID = LED_IFT(DAT_PAT,CNK_sn8alcyi);
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LEFT].postAnimInID = 
   udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LEFT].idleID;
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LEFT].priority = udpsel_FOREGROUND_PRIORITY + 1;
  SetRect( &udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LEFT].rect, 320, 408, 320+23, 408+13 );
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_RIGHT].animInID = LED_IFT(DAT_PAT,CNK_sn8arcyp);
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_RIGHT].idleID = LED_IFT(DAT_PAT,CNK_sn8arcyi);
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_RIGHT].postAnimInID = 
   udpsel_Objects[udpsel_OBJ_CITY_BUTTON_RIGHT].idleID;
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_RIGHT].priority = udpsel_FOREGROUND_PRIORITY + 1;
  SetRect( &udpsel_Objects[udpsel_OBJ_CITY_BUTTON_RIGHT].rect, 458, 408, 458+23, 408+13 );
#else
  udpsel_Objects[udpsel_OBJ_COUNTRY_STATIC].animInID = LED_IFT(DAT_LANG2,CNK_sy8counf);
  udpsel_Objects[udpsel_OBJ_COUNTRY_STATIC].animOutID = LED_IFT(DAT_LANG2,CNK_sy8couno);
  udpsel_Objects[udpsel_OBJ_COUNTRY_STATIC].idleID = LED_IFT(DAT_LANG2,CNK_sy8couni);
  SetRect( &udpsel_Objects[udpsel_OBJ_COUNTRY_STATIC].rect, 120, 329, 120+210, 329+133 );
  udpsel_Objects[udpsel_OBJ_COUNTRY_STATIC].priority = udpsel_FOREGROUND_PRIORITY;
  SetRect( &udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect, 145+23, 442, 281, 442+13 );
  udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].idleID = LE_GRAFIX_ObjectCreate(
   (short)(udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect.left),
   (short)(udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect.top),
   LE_GRAFIX_ObjectTransparent );
  udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].priority = udpsel_FOREGROUND_PRIORITY;
  udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].startX = udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect.left;
  udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].startY = udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect.top;
  udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_LEFT].animInID = LED_IFT(DAT_PAT,CNK_sn8alcnp);
  udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_LEFT].idleID = LED_IFT(DAT_PAT,CNK_sn8alcni);
  udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_LEFT].postAnimInID = 
   udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_LEFT].idleID;
  udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_LEFT].priority = udpsel_FOREGROUND_PRIORITY + 1;
  SetRect( &udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_LEFT].rect, 145, 442, 145+23, 442+13 );
  udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_RIGHT].animInID = LED_IFT(DAT_PAT,CNK_sn8arcnp);
  udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_RIGHT].idleID = LED_IFT(DAT_PAT,CNK_sn8arcni);
  udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_RIGHT].postAnimInID = 
   udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_RIGHT].idleID;
  udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_RIGHT].priority = udpsel_FOREGROUND_PRIORITY + 1;
  SetRect( &udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_RIGHT].rect, 281, 442, 281+24, 442+13 );
  udpsel_Objects[udpsel_OBJ_CURRENCY_STATIC].animInID = LED_IFT(DAT_LANG2,CNK_sy8curyf);
  udpsel_Objects[udpsel_OBJ_CURRENCY_STATIC].animOutID = LED_IFT(DAT_LANG2,CNK_sy8curyo);
  udpsel_Objects[udpsel_OBJ_CURRENCY_STATIC].idleID = LED_IFT(DAT_LANG2,CNK_sy8curyi);
  SetRect( &udpsel_Objects[udpsel_OBJ_CURRENCY_STATIC].rect, 471, 329, 471+210, 329+133 );
  udpsel_Objects[udpsel_OBJ_CURRENCY_STATIC].priority = udpsel_FOREGROUND_PRIORITY;

  SetRect( &udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect, 485+23, 442, 663, 442+13 );
  udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].idleID = LE_GRAFIX_ObjectCreate(
   (short)(udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.left),
   (short)(udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.top),
   LE_GRAFIX_ObjectTransparent );
  udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].priority = udpsel_FOREGROUND_PRIORITY;
  udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].startX = udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.left;
  udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].startY = udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.top;

  udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_LEFT].animInID = LED_IFT(DAT_PAT,CNK_sn8alcrp);
  udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_LEFT].idleID = LED_IFT(DAT_PAT,CNK_sn8alcri);
  udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_LEFT].postAnimInID = 
   udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_LEFT].idleID;
  udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_LEFT].priority = udpsel_FOREGROUND_PRIORITY + 1;
  SetRect( &udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_LEFT].rect, 485, 442, 485+23, 442+13 );
  udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_RIGHT].animInID = LED_IFT(DAT_PAT,CNK_sn8arcrp);
  udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_RIGHT].idleID = LED_IFT(DAT_PAT,CNK_sn8arcri);
  udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_RIGHT].postAnimInID = 
   udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_RIGHT].idleID;
  udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_RIGHT].priority = udpsel_FOREGROUND_PRIORITY + 1;
  SetRect( &udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_RIGHT].rect, 663, 442, 663+24, 442+13 );
#endif
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_NEXT].animInID = LED_IFT(DAT_LANG2,CNK_synext1);
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_NEXT].animOutID = LED_IFT(DAT_LANG2,CNK_synext0);
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_NEXT].idleID = LED_IFT(DAT_LANG2,CNK_synext2);
  SetRect( &udpsel_Objects[udpsel_OBJ_CITY_BUTTON_NEXT].rect, 341, 452-18, 341+127, 452-18+36 );
  udpsel_Objects[udpsel_OBJ_CITY_BUTTON_NEXT].startY = -18;

  // Custom or Standard Rules? screen.
  udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_STANDARD].animInID = LED_IFT(DAT_LANG2,CNK_systrul1);
  udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_STANDARD].animOutID = LED_IFT(DAT_LANG2,CNK_systrul0);
  udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_STANDARD].idleID = LED_IFT(DAT_LANG2,CNK_systrul2);
  SetRect( &udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_STANDARD].rect, 75, 254, 75+220, 254+62 );
  udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_CUSTOM].animInID = LED_IFT(DAT_LANG2,CNK_sycurul1);
  udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_CUSTOM].animOutID = LED_IFT(DAT_LANG2,CNK_sycurul0);
  udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_CUSTOM].idleID = LED_IFT(DAT_LANG2,CNK_sycurul2);
  SetRect( &udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_CUSTOM].rect, 507, 254, 507+220, 254+62 );

  // Do you accept these rules? screen.
  udpsel_Objects[udpsel_OBJ_RULES_TITLE].idleID = LED_IFT(DAT_LANG2,CNK_syrules);
  udpsel_Objects[udpsel_OBJ_RULES_TITLE].startY = -163 + 11;  // (doped y offset is 163, h is 78 + 2*11 = 100!)
  SetRect( &udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect,
   udpsel_RULESBUTTONCOLUMN1X,
   udpsel_RULESTOPY,
   udpsel_RULESDESCRIPTIONCOLUMN2X + udpsel_RULESDESCRIPTIONTEXTWIDTH,
   udpsel_RULESBOTTOMY );
  udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].idleID = LE_GRAFIX_ObjectCreate(
   (short)(udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.left),
   (short)(udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.top),
   LE_GRAFIX_ObjectTransparent );
  udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].startX = udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.left;
  udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].startY = udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].rect.top;
  // This priority must be greater than all the various button priorities
  udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].priority = udpsel_FOREGROUND_PRIORITY + udpsel_OBJLAST_RULES + 10;
  // Set graphic and position all the buttons pertaining to a rule.
  for ( i = udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4; i < udpsel_OBJLAST_RULES; i++ )
  {
    ruleNumber = udpsel_ConvertRuleButtonIndexToRuleNumber( i );
    // Set the button coords based on what rule it belongs to.
    x = udpsel_RulesTextPosition[ruleNumber].x;
    y = udpsel_RulesTextPosition[ruleNumber].y;
    if ( ruleNumber == 0 )
      x += (2 + i - udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4)*udpsel_RULESBUTTONWIDTH;
    else if ( (ruleNumber >= 1) && (ruleNumber < udpsel_RULESTEXT_EVENBUILDRULE) )
      x += ((i - udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_12) % 4)*udpsel_RULESBUTTONWIDTH;
    else
      x += 3*udpsel_RULESBUTTONWIDTH;
    SetRect( &udpsel_Objects[i].rect, x, y, x + udpsel_RULESBUTTONWIDTH, y + udpsel_RULESBUTTONHEIGHT );
    // Set graphic, same for all buttons
    // animIn/animOut are transition anims, idle is ON state, press is OFF state
    udpsel_Objects[i].startX = udpsel_Objects[i].rect.left;
    udpsel_Objects[i].startY = udpsel_Objects[i].rect.top;
    udpsel_Objects[i].animInID = LED_IFT( DAT_PAT, CNK_snobuttp );
    udpsel_Objects[i].animOutID = LED_IFT( DAT_PAT, CNK_snobutto );
    udpsel_Objects[i].idleID = LED_IFT( DAT_PAT, CNK_snobutti );
    udpsel_Objects[i].pressID = LED_IFT( DAT_PAT, CNK_snobuttf );
    udpsel_Objects[i].priority = udpsel_FOREGROUND_PRIORITY + i;
  }
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_OKAY].animInID = LED_IFT(DAT_LANG2,CNK_syookf);
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_OKAY].animOutID = LED_IFT(DAT_LANG2,CNK_syooko);
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_OKAY].idleID = LED_IFT(DAT_LANG2,CNK_syooki);
  SetRect( &udpsel_Objects[udpsel_OBJ_RULES_BUTTON_OKAY].rect, 400-127/2, 450, 400-127/2+127, 450+36 );
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_OKAY].startX = udpsel_Objects[udpsel_OBJ_RULES_BUTTON_OKAY].rect.left;
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_OKAY].startY = udpsel_Objects[udpsel_OBJ_RULES_BUTTON_OKAY].rect.top;
  // Just assign width/height for these buttons.  Position will get calculated along
  // with positioning for text.
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].animInID = LED_IFT(DAT_LANG2,CNK_syrrstg1);
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].animOutID = LED_IFT(DAT_LANG2,CNK_syrrstg0);
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].idleID = LED_IFT(DAT_LANG2,CNK_syrrstg2);
  SetRect( &udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].rect, 0, 0, 127, 36 );
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SHORTGAME].animInID = LED_IFT(DAT_LANG2,CNK_syrsrtg1);
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SHORTGAME].animOutID = LED_IFT(DAT_LANG2,CNK_syrsrtg0);
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SHORTGAME].idleID = LED_IFT(DAT_LANG2,CNK_syrsrtg2);
  SetRect( &udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SHORTGAME].rect, 0, 0, 127, 36 );

  // Prepare for when anims finish.
  for ( i = 0; i < udpsel_OBJ_MAX; i++ )
  {
    // For Rules screen, buttons toggle. 
    if ( (i >= udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4) && (i < udpsel_OBJLAST_RULES) )
    {
      udpsel_Objects[i].postAnimInID = udpsel_Objects[i].idleID;
      udpsel_Objects[i].postAnimOutID = udpsel_Objects[i].pressID;
      udpsel_Objects[i].endingAction = LE_SEQNCR_EndingActionStayAtEnd;
    }
    else
    {
      // By default we go to the idle after animating in.
      // and nothing after animating out.
      udpsel_Objects[i].postAnimInID = udpsel_Objects[i].idleID;
      udpsel_Objects[i].postAnimOutID = LED_EI;
    }
  }
}


/*****************************************************************************
 * Changes the values of a game options record to reflect standard monopoly rules.
 */
static void udpsel_SetStandardRules( RULE_GameOptionsPointer rulesPtr )
{
  rulesPtr->housesPerHotel = 5;
  rulesPtr->maximumHouses = 32;
  rulesPtr->maximumHotels = 12;
  rulesPtr->interestRate = 10;
  rulesPtr->initialCash = 1500;
  rulesPtr->passingGoAmount = 200;
  rulesPtr->luxuryTaxAmount = 75;
  rulesPtr->taxRate = 10;
  rulesPtr->flatTaxFee = 200;
  rulesPtr->freeParkingSeed = 500;
  rulesPtr->freeParkingPot = FALSE;
  rulesPtr->doubleSalaryOnGo = FALSE;
  rulesPtr->evenBuildRule = TRUE;
  rulesPtr->futureRentTradingAllowed = FALSE;
  rulesPtr->immunitiesTradingAllowed = FALSE;
  rulesPtr->dealFreePropertiesAtStartup = FALSE;
  rulesPtr->dealNPropertiesAtStartup = 0;
  rulesPtr->maximumTurnsInJail = 3;
  rulesPtr->getOutOfJailFee = 50;
  rulesPtr->houseShortageLevel = 6;
  rulesPtr->hotelShortageLevel = 3;
  rulesPtr->auctionGoingTimeDelay = 5;
}


/*****************************************************************************
 * Changes the values of a game options record to reflect short-game monopoly rules.
 */
static void udpsel_SetShortGameRules( RULE_GameOptionsPointer rulesPtr )
{
  udpsel_SetStandardRules( rulesPtr );
  rulesPtr->housesPerHotel = 4;
  rulesPtr->dealFreePropertiesAtStartup = TRUE;
  rulesPtr->dealNPropertiesAtStartup = 2;
}


/*****************************************************************************
 * Compares one game-options record to another and sets anims going where 
 * there are differences.  If initializeAll is TRUE starts anims going for
 * all settings of the current options.
 */
static void udpsel_UpdateRulesScreenButtons( RULE_GameOptionsPointer currentPtr, 
 RULE_GameOptionsPointer previousPtr, BOOL initializeAll )
{
  int     i;

  if ( initializeAll )
  {
    // First turn to off all buttons.
    for ( i = udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4; i < udpsel_OBJLAST_RULES; i++ )
      udpsel_Objects[i].desiredID = udpsel_Objects[i].animOutID;
    // Then set to on those buttons that deserve it.
  }

  /* If initializing, just start those ones that are on. */
  /* If not initializing, turn off the old ones, start the new. */

  if ( initializeAll || (currentPtr->housesPerHotel != previousPtr->housesPerHotel) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->housesPerHotel )
      {
      case 4:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4].animOutID;
        break;
      case 5:
      default:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_5].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_5].animOutID;
        break;
      }
    }
    switch ( currentPtr->housesPerHotel )
    {
    case 4:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4].animInID;
      break;
    case 5:
    default:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_5].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_5].animInID;
      break;
    }
  }

  if ( initializeAll || (currentPtr->maximumHouses != previousPtr->maximumHouses) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->maximumHouses )
      {
      case 12:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_12].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_12].animOutID;
        break;
      case 32:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_32].desiredID = 
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_32].animOutID;
        break;
      case 60:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_60].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_60].animOutID;
        break;
      case 88:
      default:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_88].desiredID = 
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_88].animOutID;
        break;
      }
    }
    switch ( currentPtr->maximumHouses )
    {
    case 12:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_12].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_12].animInID;
      break;
    case 32:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_32].desiredID = 
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_32].animInID;
      break;
    case 60:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_60].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_60].animInID;
      break;
    case 88:
    default:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_88].desiredID = 
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_88].animInID;
      break;
    }
  }
  if ( initializeAll || (currentPtr->maximumHotels != previousPtr->maximumHotels) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->maximumHotels )
      {
      case 4:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_4].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_4].animOutID;
        break;
      case 12:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_12].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_12].animOutID;
        break;
      case 16:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_16].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_16].animOutID;
        break;
      case 22:
      default:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_22].desiredID = 
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_22].animOutID;
        break;
      }
    }
    switch ( currentPtr->maximumHotels )
    {
    case 4:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_4].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_4].animInID;
      break;
    case 12:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_12].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_12].animInID;
      break;
    case 16:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_16].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_16].animInID;
      break;
    case 22:
    default:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_22].desiredID = 
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_22].animInID;
      break;
    }
  }
  if ( initializeAll || (currentPtr->interestRate != previousPtr->interestRate) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->interestRate )
      {
      case 0:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_0].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_0].animOutID;
        break;
      case 5:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_5].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_5].animOutID;
        break;
      case 10:
      default:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_10].desiredID = 
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_10].animOutID;
        break;
      case 20:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_20].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_20].animOutID;
        break;
      }
    }
    switch ( currentPtr->interestRate )
    {
    case 0:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_0].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_0].animInID;
      break;
    case 5:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_5].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_5].animInID;
      break;
    case 10:
    default:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_10].desiredID = 
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_10].animInID;
      break;
    case 20:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_20].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_20].animInID;
      break;
    }
  }
  if ( initializeAll || (currentPtr->initialCash != previousPtr->initialCash) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->initialCash )
      {
      case 500:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_500].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_500].animOutID;
        break;
      case 1000:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_1000].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_1000].animOutID;
        break;
      case 1500:
      default:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_1500].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_1500].animOutID;
        break;
      case 2000:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_2000].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_2000].animOutID;
        break;
      }
    }
    switch ( currentPtr->initialCash )
    {
    case 500:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_500].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_500].animInID;
      break;
    case 1000:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_1000].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_1000].animInID;
      break;
    case 1500:
    default:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_1500].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_1500].animInID;
      break;
    case 2000:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_2000].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_INITIALCASH_2000].animInID;
      break;
    }
  }
  if ( initializeAll || (currentPtr->passingGoAmount != previousPtr->passingGoAmount) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->passingGoAmount )
      {
      case 0:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_0].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_0].animOutID;
        break;
      case 100:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_100].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_100].animOutID;
        break;
      case 200:
      default:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_200].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_200].animOutID;
        break;
      case 400:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_400].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_400].animOutID;
        break;
      }
    }
    switch ( currentPtr->passingGoAmount )
    {
    case 0:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_0].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_0].animInID;
      break;
    case 100:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_100].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_100].animInID;
      break;
    case 200:
    default:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_200].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_200].animInID;
      break;
    case 400:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_400].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SALARY_400].animInID;
      break;
    }
  }

  if ( initializeAll || (currentPtr->luxuryTaxAmount != previousPtr->luxuryTaxAmount) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->luxuryTaxAmount )
      {
      case 0:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_0].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_0].animOutID;
        break;
      default:
      case 75:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_75].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_75].animOutID;
        break;
      case 150:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_150].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_150].animOutID;
        break;
      case 300:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_300].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_300].animOutID;
        break;
      }
    }
    switch ( currentPtr->luxuryTaxAmount )
    {
    case 0:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_0].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_0].animInID;
      break;
    default:
    case 75:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_75].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_75].animInID;
      break;
    case 150:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_150].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_150].animInID;
      break;
    case 300:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_300].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_LUXURYTAX_300].animInID;
      break;
    }
  }

  if ( initializeAll || (currentPtr->taxRate != previousPtr->taxRate) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->taxRate )
      {
      case 0:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_0].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_0].animOutID;
        break;
      case 5:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_5].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_5].animOutID;
        break;
      case 10:
      default:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_10].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_10].animOutID;
        break;
      case 15:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_15].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_15].animOutID;
        break;
      }
    }
    switch ( currentPtr->taxRate )
    {
    case 0:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_0].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_0].animInID;
      break;
    case 5:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_5].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_5].animInID;
      break;
    case 10:
    default:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_10].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_10].animInID;
      break;
    case 15:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_15].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_15].animInID;
      break;
    }
  }
  if ( initializeAll || (currentPtr->flatTaxFee != previousPtr->flatTaxFee) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->flatTaxFee )
      {
      case 0:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_0].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_0].animOutID;
        break;
      case 100:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_100].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_100].animOutID;
        break;
      case 200:
      default:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_200].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_200].animOutID;
        break;
      case 400:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_400].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_400].animOutID;
        break;
      }
    }
    switch ( currentPtr->flatTaxFee )
    {
    case 0:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_0].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_0].animInID;
      break;
    case 100:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_100].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_100].animInID;
      break;
    case 200:
    default:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_200].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_200].animInID;
      break;
    case 400:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_400].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FLATTAX_400].animInID;
      break;
    }
  }
  if ( initializeAll || (currentPtr->freeParkingSeed != previousPtr->freeParkingSeed) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->freeParkingSeed )
      {
      case 0:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_0].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_0].animOutID;
        break;
      case 250:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_250].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_250].animOutID;
        break;
      case 500:
      default:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_500].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_500].animOutID;
        break;
      case 750:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_750].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_750].animOutID;
        break;
      }
    }
    switch ( currentPtr->freeParkingSeed )
    {
    case 0:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_0].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_0].animInID;
      break;
    case 250:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_250].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_250].animInID;
      break;
    case 500:
    default:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_500].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_500].animInID;
      break;
    case 750:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_750].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKING_750].animInID;
      break;
    }
  }
  if ( initializeAll || (currentPtr->freeParkingPot != previousPtr->freeParkingPot) )
  {
    if ( currentPtr->freeParkingPot )
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKINGRULE].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKINGRULE].animInID;
    else
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKINGRULE].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FREEPARKINGRULE].animOutID;
  }

  if ( initializeAll || (currentPtr->doubleSalaryOnGo != previousPtr->doubleSalaryOnGo) )
  {
    if ( currentPtr->doubleSalaryOnGo )
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_EXTRAGOMONEY].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_EXTRAGOMONEY].animInID;
    else
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_EXTRAGOMONEY].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_EXTRAGOMONEY].animOutID;
  }

  if ( initializeAll || (currentPtr->evenBuildRule != previousPtr->evenBuildRule) )
  {
    if ( currentPtr->evenBuildRule )
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_EVENBUILDRULE].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_EVENBUILDRULE].animInID;
    else
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_EVENBUILDRULE].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_EVENBUILDRULE].animOutID;
  }

  if ( initializeAll || ((currentPtr->futureRentTradingAllowed != previousPtr->futureRentTradingAllowed)
   || (currentPtr->immunitiesTradingAllowed != previousPtr->immunitiesTradingAllowed)) )
  {
    if ( currentPtr->futureRentTradingAllowed && currentPtr->immunitiesTradingAllowed )
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FUTURESANDIMMUNITIES].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FUTURESANDIMMUNITIES].animInID;
    else
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FUTURESANDIMMUNITIES].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_FUTURESANDIMMUNITIES].animOutID;
  }

  if ( initializeAll || (currentPtr->dealFreePropertiesAtStartup != previousPtr->dealFreePropertiesAtStartup) )
  {
    if ( currentPtr->dealFreePropertiesAtStartup )
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_DEALTPROPERTIESAREFREE].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_DEALTPROPERTIESAREFREE].animInID;
    else
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_DEALTPROPERTIESAREFREE].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_DEALTPROPERTIESAREFREE].animOutID;
  }

  if ( initializeAll || (currentPtr->dealNPropertiesAtStartup != previousPtr->dealNPropertiesAtStartup) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->dealNPropertiesAtStartup )
      {
      default:
      case 0:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_0].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_0].animOutID;
        break;
      case 2:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_2].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_2].animOutID;
        break;
      case 4:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_4].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_4].animOutID;
        break;
      case SQ_TOTAL_PROPERTY_SQUARES:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_ALL].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_ALL].animOutID;
        break;
      }
    }
    switch ( currentPtr->dealNPropertiesAtStartup )
    {
    default:
    case 0:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_0].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_0].animInID;
      break;
    case 2:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_2].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_2].animInID;
      break;
    case 4:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_4].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_4].animInID;
      break;
    case SQ_TOTAL_PROPERTY_SQUARES:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_ALL].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_ALL].animInID;
      break;
    }
  }
  if ( initializeAll || (currentPtr->maximumTurnsInJail != previousPtr->maximumTurnsInJail) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->maximumTurnsInJail )
      {
      case 1:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_1].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_1].animOutID;
        break;
      case 2:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_2].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_2].animOutID;
        break;
      case 3:
      default:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_3].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_3].animOutID;
        break;
      case 4:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_4].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_4].animOutID;
        break;
      }
    }
    switch ( currentPtr->maximumTurnsInJail )
    {
    case 1:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_1].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_1].animInID;
      break;
    case 2:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_2].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_2].animInID;
      break;
    case 3:
    default:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_3].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_3].animInID;
      break;
    case 4:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_4].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_4].animInID;
      break;
    }
  }

  if ( initializeAll || (currentPtr->getOutOfJailFee != previousPtr->getOutOfJailFee) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->getOutOfJailFee )
      {
      case 0:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_0].desiredID = 
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_0].animOutID;
        break;
      case 50:
      default:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_50].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_50].animOutID;
        break;
      case 100:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_100].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_100].animOutID;
        break;
      case 200:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_200].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_200].animOutID;
        break;
      }
    }
    switch ( currentPtr->getOutOfJailFee )
    {
    case 0:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_0].desiredID = 
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_0].animInID;
      break;
    case 50:
    default:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_50].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_50].animInID;
      break;
    case 100:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_100].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_100].animInID;
      break;
    case 200:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_200].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_JAILFEE_200].animInID;
      break;
    }
  }

  if ( initializeAll || (currentPtr->houseShortageLevel != previousPtr->houseShortageLevel) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->houseShortageLevel )
      {
      case 0:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_0].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_0].animOutID;
        break;
      case 1:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_1].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_1].animOutID;
        break;
      case 6:
      default:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_6].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_6].animOutID;
        break;
      case 12:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_12].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_12].animOutID;
        break;
      }
    }
    switch ( currentPtr->houseShortageLevel )
    {
    case 0:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_0].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_0].animInID;
      break;
    case 1:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_1].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_1].animInID;
      break;
    case 6:
    default:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_6].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_6].animInID;
      break;
    case 12:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_12].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_12].animInID;
      break;
    }
  }

  if ( initializeAll || (currentPtr->hotelShortageLevel != previousPtr->hotelShortageLevel) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->hotelShortageLevel )
      {
      case 0:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_0].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_0].animOutID;
        break;
      case 1:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_1].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_1].animOutID;
        break;
      case 3:
      default:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_3].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_3].animOutID;
        break;
      case 6:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_6].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_6].animOutID;
        break;
      }
    }
    switch ( currentPtr->hotelShortageLevel )
    {
    case 0:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_0].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_0].animInID;
      break;
    case 1:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_1].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_1].animInID;
      break;
    case 3:
    default:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_3].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_3].animInID;
      break;
    case 6:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_6].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_6].animInID;
      break;
    }
  }
  if ( initializeAll || (currentPtr->auctionGoingTimeDelay != previousPtr->auctionGoingTimeDelay) )
  {
    if ( !initializeAll  )
    {
      switch ( previousPtr->auctionGoingTimeDelay )
      {
      case 3:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_3].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_3].animOutID;
        break;
      case 4:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_4].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_4].animOutID;
        break;
      case 5:
      default:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_5].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_5].animOutID;
        break;
      case 10:
        udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_10].desiredID =
         udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_10].animOutID;
        break;
      }
    }
    switch ( currentPtr->auctionGoingTimeDelay )
    {
    case 3:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_3].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_3].animInID;
      break;
    case 4:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_4].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_4].animInID;
      break;
    case 5:
    default:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_5].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_5].animInID;
      break;
    case 10:
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_10].desiredID =
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_10].animInID;
      break;
    }
  }

  // Make them equal.  Until next update.
  *previousPtr = *currentPtr;

}


/*****************************************************************************
 * This function receives the pointers to 3 different 2 dimensional arrays.
 * The first dimension of all arrays is assumed to be of length "numberOfHiScores".
 * The second dimension of each array is given as well, independently.  These
 * arrays will hold wide-character format strings.  The function will fill these
 * arrays in parallel, with the names, wins, and greatest net worth of the top 
 * "numberOfHiScores" players as currently saved in Monopoly.ini.  This is a file
 * with player name, wins, and greatest net worth info.  It contains info on players
 * who have at least started a monopoly game.  The top players list is derived from 
 * this.
 * Returns the number of hi scores added to the list.
 */
static int udpsel_GetHiScoreInfo( wchar_t *namesArray, int maxNamesLength,
 wchar_t *winsArray, int maxWinsLength, wchar_t *greatestNetWorthArray,
 int maxGreatestNetWorthLength, int numberOfHiScores )
{
  int     i,      j;
  int     rank;
  int     wins,              netWorth;
            // Score values for the player we are currently considering.
  int     ceilingForWins,    ceilingForNetWorth;
            // When searching for the nth best player we ignore scores that are higher
            // than these.
  int     highestWins,       highestNetWorth;
            // In our search for the nth best player, these are the scores of the nth
            // best so far.
  char    playerKeyString[100];
  char    tempString[256];
  char    nameString[256];
  wchar_t tempWideString[256];
  wchar_t *namesPtr,         *winsPtr,      *netWorthPtr;
  wchar_t *tempWidePtr;
  BOOL    nameFound;
  int     playersAdded;

  playersAdded = 0;

  namesPtr = namesArray;
  winsPtr = winsArray;
  netWorthPtr = greatestNetWorthArray;
  if ( namesPtr == NULL )
    return playersAdded;
  if ( winsPtr == NULL )
    return playersAdded;
  if ( netWorthPtr == NULL )
    return playersAdded;

  /* Clear results. */

  for ( rank = 0; rank < numberOfHiScores; rank++ )
  {
    mbstowcs( namesPtr, "", maxNamesLength );
    mbstowcs( winsPtr, "", maxWinsLength );
    mbstowcs( netWorthPtr, "", maxGreatestNetWorthLength );
    namesPtr += maxNamesLength;
    winsPtr += maxWinsLength;
    netWorthPtr += maxGreatestNetWorthLength;
  }
  namesPtr = namesArray;
  winsPtr = winsArray;
  netWorthPtr = greatestNetWorthArray;

  /* Search the ini file and copy the top 8 (or so) players into our arrays. */

  ceilingForWins = INT_MAX;
  ceilingForNetWorth = INT_MAX;
  for ( rank = 0; rank < numberOfHiScores; rank++ )
  {
    // Prepare search for (rank+1)th best.
    highestWins = 0;
    highestNetWorth = 0;
    // Loop thru all players searching for the next highest and add them at this spot.
    for ( i = 0; i < udpsel_HISTORYLOG_MAX_PLAYERS; i++ )
    {
      // Get wins, net worth info for this player.
      sprintf( playerKeyString, "%s%d", udpsel_HISTORYLOG_PLAYERKEY, i+1 );  // BLEEBLE: Not checking for string overrun here.
      GetPrivateProfileString( playerKeyString, udpsel_HISTORYLOG_NAMEKEY, "", nameString, 256,
       LE_DIRINI_INI_PathName );
      // Ignore anything but real names.
      if ( strcmp(nameString,"") == 0 )
        continue;
      GetPrivateProfileString( playerKeyString, udpsel_HISTORYLOG_WINSKEY, "", tempString, 256,
       LE_DIRINI_INI_PathName );
      // Must have at least one win to be on hi-score table.
      wins = atoi( tempString );
      if ( wins < 1 )
        continue;
      GetPrivateProfileString( playerKeyString, udpsel_HISTORYLOG_NETWORTHKEY, "", tempString, 256,
       LE_DIRINI_INI_PathName );
      netWorth = atoi( tempString );
      if ( (wins > ceilingForWins) || ((wins == ceilingForWins) && (netWorth > ceilingForNetWorth)) )
      {
        // Skip ones that should already have been added to the list by now.
        continue;
      }
      else if ( (wins == ceilingForWins) && (netWorth == ceilingForNetWorth) )
      {
        // It's a tie.  Make sure it is not a player we've already accounted
        // for, and then add it to the list.
        nameFound = FALSE;
        // Remember the name.
        mbstowcs( tempWideString, nameString, maxNamesLength );
        // Compare it to names already in our top list.
        for ( j = 0; j < rank + 1; j++ )
        {
          tempWidePtr = namesArray + j*maxNamesLength;
          if ( wcscmp(tempWidePtr,tempWideString) == 0 )
          {
            nameFound = TRUE;
            break;  // from the little search for ties.
          }
        }
        if ( !nameFound )
        {
          // New player, so add it to the list
          mbstowcs( namesPtr, nameString, maxNamesLength );
          sprintf( tempString, "%d", wins );
          mbstowcs( winsPtr, tempString, maxWinsLength );
          UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( netWorth, DISPLAY_state.system,
           netWorthPtr, TRUE );
          // Remember the high-water mark.
          highestWins = wins;
          highestNetWorth = netWorth;
          break;  // break from the search for this (rank+1)th highest and move directly
                  // on to the search for the next highest.
        }
      }
      else /*  (wins < ceilingForWins)  (netWorth < ceilingForNetWorth) */
      {
        if ( (wins > highestWins) || ((wins == highestWins) && (netWorth > highestNetWorth)) )
        {
          // Found a new nth best.  
          // Copy the info, including name, to our final array.  The last copy we 
          // end up doing will then truly represent the nth best.
          mbstowcs( namesPtr, nameString, maxNamesLength );
          sprintf( tempString, "%d", wins );
          mbstowcs( winsPtr, tempString, maxWinsLength );
          UDPENNY_ReturnMoneyValueBasedOnMonatarySystem( netWorth, DISPLAY_state.system,
           netWorthPtr, TRUE );
          // This is the new high-water mark.
          highestWins = wins;
          highestNetWorth = netWorth;
        }
      }
    }
    // Exclude nth best scores when searching for (n+1)th best score.
    ceilingForWins = highestWins;
    ceilingForNetWorth = highestNetWorth;
    // If we've added a real name, move along to the next spots in our arrays.
    if ( wcslen(namesPtr) > 0 )
    {
      playersAdded++;
      namesPtr += maxNamesLength;
      winsPtr += maxWinsLength;
      netWorthPtr += maxGreatestNetWorthLength;
    }
  }
  return playersAdded;
}


/*****************************************************************************
 * Returns TRUE if there was hiscore actually printed to the text overlay,
 * FALSE otherwise.
 */
static BOOL udpsel_PrintHiScoreInfo( void )
{
#define udpsel_READBUFFERSIZE       256
#define udpsel_NUMBEROFHISCORES     8
  wchar_t namesArray[udpsel_NUMBEROFHISCORES][udpsel_READBUFFERSIZE];
  wchar_t winsArray[udpsel_NUMBEROFHISCORES][udpsel_READBUFFERSIZE];
  wchar_t greatesNetWorthArray[udpsel_NUMBEROFHISCORES][udpsel_READBUFFERSIZE];
  int     i;
  int     x,      y;

  if ( udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].idleID != LED_EI )
  {
    // First get the latest hi-score info.
    if ( udpsel_GetHiScoreInfo(&namesArray[0][0],udpsel_READBUFFERSIZE,
     &winsArray[0][0],udpsel_READBUFFERSIZE,&greatesNetWorthArray[0][0],
     udpsel_READBUFFERSIZE,udpsel_NUMBEROFHISCORES) > 0 )
    {
      // Set the height of the font.  This is smaller than the default.
      LE_FONTS_SetSize( 2*udpsel_FONTPOINTSIZE );
      LE_FONTS_SetWeight( udpsel_FONTWEIGHT );
      // Clear tab, make it transparent
      LE_GRAFIX_ColorArea( udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].idleID, 0, 0,
       (short)(udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].rect.left),
       (short)(udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].rect.top),
       LEG_MCR(0, 255, 0) );
      y = udpsel_HISCORETOPY;
      for ( i = 0; i < udpsel_NUMBEROFHISCORES; i++ )
      {
        x = udpsel_HISCOREPLAYERNAMEX - LE_FONTS_GetStringWidth(&namesArray[i][0])/2;
        LANG_Print( udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].idleID,
         x, y, LEG_MCR(255, 255, 255), &namesArray[i][0] );
        x = udpsel_HISCOREWINSX - LE_FONTS_GetStringWidth(&winsArray[i][0])/2;
        LANG_Print( udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].idleID,
         x, y, LEG_MCR(255, 255, 255), &winsArray[i][0] );
        x = udpsel_HISCOREGREATESTNETWORTHX - LE_FONTS_GetStringWidth(&greatesNetWorthArray[i][0]);
        LANG_Print( udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].idleID,
         x, y, LEG_MCR(255, 255, 255), &greatesNetWorthArray[i][0] );
        y += 2*udpsel_FONTPOINTSIZE + 5;
      }
      LE_SEQNCR_ForceRedraw( udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].idleID,
       udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].priority );

      return TRUE;
    }
  }
  return FALSE;
}


/*****************************************************************************
 * Initialize any permanent display objects.  Avoid temporary ones when possible.
 */
void DISPLAY_UDPSEL_Initialize( void )
{
  int   i;
  int   w,      h;
  int   x,      y;
  int   lastY;

#if compilefor_TESTWORDWRAP
  UDPSEL_TestWordWrap();
#endif


  DISPLAY_state.previousPlayerSetupPhase = UDPSEL_PHASE_NONE;
  DISPLAY_state.currentPlayerSetupPhase = UDPSEL_PHASE_NONE;
  DISPLAY_state.desiredPlayerSetupPhase = UDPSEL_PHASE_NONE;

  // Initialize rules screen text strings.
  udpsel_RulesWideText[udpsel_RULESTEXT_HOUSESPERHOTEL]                     = LANG_GetTextMessage(TMN_HOUSES_PER_HOTEL);
  udpsel_RulesWideText[udpsel_RULESTEXT_TOTALHOUSES]                        = LANG_GetTextMessage(TMN_TOTAL_HOUSES);
  udpsel_RulesWideText[udpsel_RULESTEXT_TOTALHOTELS]                        = LANG_GetTextMessage(TMN_TOTAL_HOTELS);
  udpsel_RulesWideText[udpsel_RULESTEXT_FREEPARKINGJACKPOTAMOUNT]           = LANG_GetTextMessage(TMN_FREE_PARKING_JACKPOT_AMOUNT);
  udpsel_RulesWideText[udpsel_RULESTEXT_INITIALCASH]                        = LANG_GetTextMessage(TMN_INITIAL_CASH);
  udpsel_RulesWideText[udpsel_RULESTEXT_SALARY]                             = LANG_GetTextMessage(TMN_SALARY);
  udpsel_RulesWideText[udpsel_RULESTEXT_INCOMETAXRATE]                      = LANG_GetTextMessage(TMN_INCOME_TAX_RATE);
  udpsel_RulesWideText[udpsel_RULESTEXT_INCOMETAXAMOUNT]                    = LANG_GetTextMessage(TMN_INCOME_TAX_AMOUNT);
  udpsel_RulesWideText[udpsel_RULESTEXT_LUXURYTAX]                          = LANG_GetTextMessage(TMN_LUXURY_TAX);
  udpsel_RulesWideText[udpsel_RULESTEXT_MAXIMUMTURNSINJAIL]                 = LANG_GetTextMessage(TMN_MAXIMUM_TURNS_IN_JAIL);
  udpsel_RulesWideText[udpsel_RULESTEXT_JAILFEE]                            = LANG_GetTextMessage(TMN_JAIL_FEE);
  udpsel_RulesWideText[udpsel_RULESTEXT_HOUSESREMAININGFORSHORTAGE]         = LANG_GetTextMessage(TMN_HOUSES_REMAINING_FOR_SHORTAGE);
  udpsel_RulesWideText[udpsel_RULESTEXT_HOTELSREMAININGFORSHORTAGE]         = LANG_GetTextMessage(TMN_HOTELS_REMAINING_FOR_SHORTAGE);
  udpsel_RulesWideText[udpsel_RULESTEXT_MORTGAGEINTERESTRATE]               = LANG_GetTextMessage(TMN_MORTGAGE_INTEREST_RATE_PERCENTAGE);
  udpsel_RulesWideText[udpsel_RULESTEXT_AUCTIONTIMEDELAYINSECONDS]          = LANG_GetTextMessage(TMN_AUCTION_TIME_DELAY);
  udpsel_RulesWideText[udpsel_RULESTEXT_PROPERTIESDEALTATSTART]             = LANG_GetTextMessage(TMN_PROPERTIES_DEALT_AT_START);
  udpsel_RulesWideText[udpsel_RULESTEXT_EVENBUILDRULE]                      = LANG_GetTextMessage(TMN_EVEN_BUILD_RULE);
  udpsel_RulesWideText[udpsel_RULESTEXT_COLLECTDOUBLEMONEYFORLANDINGONGO]   = LANG_GetTextMessage(TMN_COLLECT_DOUBLE_MONEY_FOR_GO);
  udpsel_RulesWideText[udpsel_RULESTEXT_FREEPARKINGRULE]                    = LANG_GetTextMessage(TMN_FREE_PARKING_RULE);
  udpsel_RulesWideText[udpsel_RULESTEXT_FUTURESANDIMMUNITIES]               = LANG_GetTextMessage(TMN_FUTURES_AND_IMMUNITIES);
  udpsel_RulesWideText[udpsel_RULESTEXT_DEALTPROPERTIESAREFREE]             = LANG_GetTextMessage(TMN_DEALT_PROPERTIES_ARE_FREE);

  udpsel_RulesWideIndex[udpsel_RULESTEXT_HOUSESPERHOTEL]                     = (TMN_HOUSES_PER_HOTEL);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_TOTALHOUSES]                        = (TMN_TOTAL_HOUSES);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_TOTALHOTELS]                        = (TMN_TOTAL_HOTELS);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_FREEPARKINGJACKPOTAMOUNT]           = (TMN_FREE_PARKING_JACKPOT_AMOUNT);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_INITIALCASH]                        = (TMN_INITIAL_CASH);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_SALARY]                             = (TMN_SALARY);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_INCOMETAXRATE]                      = (TMN_INCOME_TAX_RATE);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_INCOMETAXAMOUNT]                    = (TMN_INCOME_TAX_AMOUNT);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_LUXURYTAX]                          = (TMN_LUXURY_TAX);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_MAXIMUMTURNSINJAIL]                 = (TMN_MAXIMUM_TURNS_IN_JAIL);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_JAILFEE]                            = (TMN_JAIL_FEE);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_HOUSESREMAININGFORSHORTAGE]         = (TMN_HOUSES_REMAINING_FOR_SHORTAGE);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_HOTELSREMAININGFORSHORTAGE]         = (TMN_HOTELS_REMAINING_FOR_SHORTAGE);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_MORTGAGEINTERESTRATE]               = (TMN_MORTGAGE_INTEREST_RATE_PERCENTAGE);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_AUCTIONTIMEDELAYINSECONDS]          = (TMN_AUCTION_TIME_DELAY);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_PROPERTIESDEALTATSTART]             = (TMN_PROPERTIES_DEALT_AT_START);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_EVENBUILDRULE]                      = (TMN_EVEN_BUILD_RULE);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_COLLECTDOUBLEMONEYFORLANDINGONGO]   = (TMN_COLLECT_DOUBLE_MONEY_FOR_GO);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_FREEPARKINGRULE]                    = (TMN_FREE_PARKING_RULE);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_FUTURESANDIMMUNITIES]               = (TMN_FUTURES_AND_IMMUNITIES);
  udpsel_RulesWideIndex[udpsel_RULESTEXT_DEALTPROPERTIESAREFREE]             = (TMN_DEALT_PROPERTIES_ARE_FREE);

  // Initialize token name text strings.
  for ( i = 0; i < MAX_TOKENS; i++ )
    wcscpy(udpsel_TokenNames[i], LANG_GetTextMessage(ATR_TOKEN_NAME_0 + i));

  // Calculate positioning of text for rules screen, as well
  // as positioning of standard/short game buttons.
  LE_FONTS_SetSize( udpsel_FONTPOINTSIZE );
  x = udpsel_RULESBUTTONCOLUMN1X;
  y = udpsel_RULESROWTOPY;
  for ( i = 0; i < udpsel_MAX_RULESTEXT; i++ )
  {
    // Assign position.
    udpsel_RulesTextPosition[i].x = x;
    udpsel_RulesTextPosition[i].y = y;

    // Calculate next row positioning.  When bottom reached move to second column
//    w = LANG_GetStringWidth( udpsel_RulesWideText[i] );
    w = LANG_GetStringWidth( LANG_GetTextMessage(udpsel_RulesWideIndex[i]) );
    y += (1 + w/udpsel_RULESDESCRIPTIONTEXTWIDTH)*udpsel_RULESROWHEIGHT;
    if ( y > udpsel_RULESROWBOTTOMY )
    {
      x = udpsel_RULESBUTTONCOLUMN2X;
      y = udpsel_RULESROWTOPY;
    }
    lastY = y;
  }
  LE_FONTS_ResetCharacteristics();

#if USA_VERSION
  for ( i = 0; i < DISPLAY_CITY_MAX; i++ )
  {
    // Convert text to wide.  BLEEBLE: Eventually to be replaced
    // by non-hard-coded, language dependent code.
    mbstowcs( &udpsel_CitiesWideText[i][0], udpsel_CitiesText[i], 200 );
  }
#endif

  udpsel_InitializeObjects();

  // For Rules screen:
  // Buttons go x-centered in second column, and y-centered in remaining space.
  // For these calculations, assume both are same height and width.
  // Note: must do these calculations after the udpsel_InitializeObjects() call
  // above or it gets reset to 0.
  w = udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].rect.right -
   udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].rect.left;
  h = udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].rect.bottom -
   udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].rect.top;
  x = udpsel_RULESBUTTONCOLUMN2X + (800 - udpsel_RULESBUTTONCOLUMN2X - w)/2;
  y = lastY + (udpsel_RULESROWBOTTOMY - lastY - h*2)/3;
  OffsetRect( &udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].rect, x, y );
  y = lastY + 2*(udpsel_RULESROWBOTTOMY - lastY - h*2)/3 + h;
  OffsetRect( &udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SHORTGAME].rect, x, y );
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].startX = udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].rect.left;
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].startY = udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].rect.top;
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SHORTGAME].startX = udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SHORTGAME].rect.left;
  udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SHORTGAME].startY = udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SHORTGAME].rect.top;

  udpsel_PrintRemovePlayerTextToOverlay();

}


/*****************************************************************************
 */
void DISPLAY_UDPSEL_Destroy( void )
{
  int     i;

  // Free up resources for all transparent graphics into which special text
  // was drawn, on exiting program.
  if ( udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].idleID != LED_EI )
    LE_DATA_FreeRuntimeDataID( udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].idleID );
  if ( udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].idleID != LED_EI )
    LE_DATA_FreeRuntimeDataID( udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].idleID );
#if USA_VERSION
  if ( udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].idleID != LED_EI )
    LE_DATA_FreeRuntimeDataID( udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].idleID );
#else
  if ( udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].idleID != LED_EI )
    LE_DATA_FreeRuntimeDataID( udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].idleID );
  if ( udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].idleID != LED_EI )
    LE_DATA_FreeRuntimeDataID( udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].idleID );
#endif
  if ( udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].idleID != LED_EI )
    LE_DATA_FreeRuntimeDataID( udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].idleID );
  if ( udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].idleID != LED_EI )
    LE_DATA_FreeRuntimeDataID( udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].idleID );
  for ( i = 0; i < 8; i++ )
  {
    if ( udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].idleID != LED_EI )
      LE_DATA_FreeRuntimeDataID( udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].idleID );
  }
}


/*****************************************************************************
 * Checks the state of all objects for a given phase to see if one is
 * animating in or out depending on the flag.
 */
static BOOL udpsel_PhaseIsAnimating( int phase, BOOL animatingIn )
{
  int   i;
  int   lowIndex,   highIndex;
  switch ( phase )
  {
  case UDPSEL_PHASE_HISCORE:
    lowIndex = udpsel_OBJFIRST_HISCORE;
    highIndex = udpsel_OBJLAST_HISCORE;
    break;
  case UDPSEL_PHASE_LOCALORNETWORK:
    lowIndex = udpsel_OBJFIRST_LOCALNETWORK;
    highIndex = udpsel_OBJLAST_LOCALNETWORK;
    break;
  case UDPSEL_PHASE_SELECTPLAYER:
    lowIndex = udpsel_OBJFIRST_SELECTPLAYER;
    highIndex = udpsel_OBJLAST_SELECTPLAYER;
    break;
  case UDPSEL_PHASE_ENTERNAME:
    lowIndex = udpsel_OBJFIRST_ENTERNAME;
    highIndex = udpsel_OBJLAST_ENTERNAME;
    break;
  case UDPSEL_PHASE_SELECTTOKEN:
    lowIndex = udpsel_OBJFIRST_PICKTOKEN;
    highIndex = udpsel_OBJLAST_PICKTOKEN;
    break;
  case UDPSEL_PHASE_STARTADDREMOVE:
    lowIndex = udpsel_OBJFIRST_START;
    highIndex = udpsel_OBJLAST_START;
    break;
  case UDPSEL_PHASE_REMOVEPLAYER:
    lowIndex = udpsel_OBJFIRST_REMOVE;
    highIndex = udpsel_OBJLAST_REMOVE;
    break;
  case UDPSEL_PHASE_SELECTAISTRENGTH:
    lowIndex = udpsel_OBJFIRST_AISTRENGTH;
    highIndex = udpsel_OBJLAST_AISTRENGTH;
    break;
  case UDPSEL_PHASE_SELECTCITY:
    lowIndex = udpsel_OBJFIRST_COUNTRY;
    highIndex = udpsel_OBJLAST_COUNTRY;
    break;
  case UDPSEL_PHASE_STANDARDORCUSTOMRULES:
    lowIndex = udpsel_OBJFIRST_RULESCHOICE;
    highIndex = udpsel_OBJLAST_RULESCHOICE;
    break;
  case UDPSEL_PHASE_CUSTOMIZERULES:
    lowIndex = udpsel_OBJFIRST_RULES;
    highIndex = udpsel_OBJLAST_RULES;
    break;
  default:
    lowIndex = 0;
    highIndex = udpsel_OBJ_MAX;
    break;
  }

  for ( i = lowIndex; i <= highIndex; i++ )
  {
    if ( animatingIn )
    {
      if ( (udpsel_Objects[i].currentID != LED_EI)
       && (udpsel_Objects[i].currentID == udpsel_Objects[i].animInID) )
      {
          return TRUE;
      }
      // If an anim is pending that's good enough too.
      if ( (udpsel_Objects[i].desiredID != LED_EI)
       && (udpsel_Objects[i].desiredID == udpsel_Objects[i].animInID) )
      {
          return TRUE;
      }
    }
    else
    {
      if ( (udpsel_Objects[i].currentID != LED_EI)
       && (udpsel_Objects[i].currentID == udpsel_Objects[i].animOutID) )
      {
          return TRUE;
      }
      // If an anim is pending that's good enough too.
      if ( (udpsel_Objects[i].desiredID != LED_EI)
       && (udpsel_Objects[i].desiredID == udpsel_Objects[i].animOutID) )
      {
          return TRUE;
      }
    }
  }
  return FALSE;
}



/*****************************************************************************
 * Checks through all players to see if any of them have already chosen the
 * given token.
 */
static BOOL udpsel_TokenIsAvailable( RULE_TokenKind token )
{
  int   i;

  for ( i = 0; i < UICurrentGameState.NumberOfPlayers; i++ )
    if ( UICurrentGameState.Players[i].token == token )
      return FALSE;
  return TRUE;
}


/*****************************************************************************
 * Given the current token under consideration determines which tokens are 
 * still available to be selected and returns the next available one.
 */
static RULE_TokenKind udpsel_DetermineNextAvailableToken( RULE_TokenKind eCurrentToken )
{
  // Determine the next token in the list
  RULE_TokenKind nToken = (eCurrentToken + 1) % MAX_TOKENS;

  // If there aren't any players yet, return the first token
  if (0 == UICurrentGameState.NumberOfPlayers)
    return(nToken);

  BOOL bTokenFound = FALSE;

  while (!bTokenFound)
  {
    // Is nToken available?  Check if any of the players have selected it.
    if ( udpsel_TokenIsAvailable(nToken) )
      return nToken;

    // Move to the next token
    nToken = (nToken + 1) % MAX_TOKENS;

    // If we come back to the token we started with, it must be the only one
    //  that is available
    if (nToken == eCurrentToken)
      bTokenFound = TRUE;
  }

  // In the unexpected event that we aren't able to find an available token
  //  we will just return the one we were given
  return(eCurrentToken);
}


/*****************************************************************************
 * Given a name checks it against the default player names, which are just 
 * token names, if it finds a match, returns the appropriate token enum.  If
 * no match can be found returns some next available token.
 */
static RULE_TokenKind udpsel_GetTokenByName( wchar_t *name )
{
  int     i;

  for ( i = 0; i < MAX_TOKENS; i++ )
  {
    if ( wcscmp(name,udpsel_TokenNames[i]) == 0 )
    {
      return (RULE_TokenKind)i;
      break;
    }
  }
  return udpsel_DetermineNextAvailableToken( (RULE_TokenKind)TK_GUN );
}


/*****************************************************************************
 * Given the current colour under consideration determines which colours are 
 * still available to be selected and returns the one passed or the next 
 * available one.
 */
static RULE_PlayerColour udpsel_DetermineNextAvailableColour(RULE_PlayerColour eCurrentColour)
{
  // Check colour range.
  RULE_PlayerColour nClr = eCurrentColour % MAX_PLAYER_COLOURS;

  // If there aren't any players yet, return the first token
  if (0 == UICurrentGameState.NumberOfPlayers)
    return(nClr);

  BOOL bClrFound = FALSE;

  while (!bClrFound)
  {
    // Is colour available?  Check if any of the players have selected it.
    for (int i = 0; i < UICurrentGameState.NumberOfPlayers; i++)
      if (UICurrentGameState.Players[i].colour == nClr) break;

    // If we iterated through all of the current players, and no one has this
    //  colour, we can safely return this colour as being available
    if (i == UICurrentGameState.NumberOfPlayers)
      return(nClr);

    // Move to the next colour
    nClr = (nClr + 1) % MAX_PLAYER_COLOURS;

    // If we have come back to the same colour as we started with, it must be
    //  the only one that is available
    if (nClr == eCurrentColour) 
      bClrFound = TRUE;
  }

  // In the unexpected event that we aren't able to find an available colour
  //  we will just return the one we were given
  return(eCurrentColour);
}


/*****************************************************************************
 * Change display of currently rotating token.
 */
static void udpsel_TokenScreen_ShowRotatingToken( RULE_TokenKind token )
{
  int   i;

  if ( (token >= 0) && (token < MAX_TOKENS) )
  {
    for ( i = 0; i < MAX_TOKENS; i++ )
      udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_CANNON + i].desiredID = LED_EI;

    udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_CANNON + token].desiredID = 
     udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_CANNON + token].idleID;
    udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_CANNON + token].endingAction = 
     LE_SEQNCR_EndingActionLoopToBeginning;
  }
}


#if USA_VERSION
/*****************************************************************************
 * Updates the display of the city field.
 */
static void udpsel_ShowCityName( int city )
{
  if ( udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].idleID != LED_EI )
  {
    // Clear the old field
    LE_GRAFIX_ColorArea( udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].idleID, 0, 0,
     (short)(udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect.left),
     (short)(udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect.top),
     LEG_MCR(0, 255, 0) );
    if ( (city >= 0) && (city < DISPLAY_CITY_MAX) )
    {
      // Now write in the updated text, center justified.
      LE_FONTS_SetSize( udpsel_FONTPOINTSIZE );
      LANG_Print( udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].idleID,
       (udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect.left)/2
        - LE_FONTS_GetStringWidth(&udpsel_CitiesWideText[city][0])/2,
       (udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].rect.top)/2
        - LE_FONTS_GetStringHeight(&udpsel_CitiesWideText[city][0])/2,
       LEG_MCR(255, 255, 255),
       &udpsel_CitiesWideText[city][0] );
    }
    LE_SEQNCR_ForceRedraw( udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].idleID,
     udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].priority );
  }
}

#else

/*****************************************************************************
 * Updates the display of the country field.
 */
static void udpsel_ShowCountryName( int country )
{
  wchar_t *countryNames[DISPLAY_CITY_MAX] = 
  {
    LANG_GetTextMessage(TMN_UK),
    LANG_GetTextMessage(TMN_FRANCE),
    LANG_GetTextMessage(TMN_GERMANY),
    LANG_GetTextMessage(TMN_SPAIN),
    LANG_GetTextMessage(TMN_HOLLAND),
    LANG_GetTextMessage(TMN_SWEDEN),
    LANG_GetTextMessage(TMN_FINLAND),
    LANG_GetTextMessage(TMN_DENMARK),
    LANG_GetTextMessage(TMN_NORWAY),
    LANG_GetTextMessage(TMN_BELGIUM),
    LANG_GetTextMessage(TMN_SINGAPORE),
    LANG_GetTextMessage(TMN_AUSTRALIA),
  };

  if ( udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].idleID != LED_EI )
  {
    // Clear the old field
    LE_GRAFIX_ColorArea( udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].idleID, 0, 0,
     (short)(udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect.left),
     (short)(udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect.top),
     LEG_MCR(0, 255, 0) );
    if ( (country >= 0) && (country < DISPLAY_CITY_MAX) )
    {
      // Now write in the updated text, center justified.
      LE_FONTS_SetSize( udpsel_FONTPOINTSIZE );
      LANG_Print( udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].idleID,
       (udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect.left)/2
        - LE_FONTS_GetStringWidth(countryNames[country])/2,
       (udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].rect.top)/2
        - LE_FONTS_GetStringHeight(countryNames[country])/2,
       LEG_MCR(255, 255, 255),
       countryNames[country] );
    }
    LE_SEQNCR_ForceRedraw( udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].idleID,
     udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].priority );
  }
}


/*****************************************************************************
 * Updates the display of the currency field.
 */
static void udpsel_ShowCurrencyText( int currency )
{
  int     width;
  wchar_t *currencyNames[DISPLAY_CITY_MAX + 1] =    // The Euro is extra.
  {
    LANG_GetTextMessage(TMN_POUNDS_UK),
    LANG_GetTextMessage(TMN_FRANCS_F),
    LANG_GetTextMessage(TMN_DEUTSCH_MARKS_D),
    LANG_GetTextMessage(TMN_PESETA_E),
    LANG_GetTextMessage(TMN_GUILDERS_NL),
    LANG_GetTextMessage(TMN_KRONA_SW),
    LANG_GetTextMessage(TMN_MARKA_FIN),
    LANG_GetTextMessage(TMN_KRONERS_DK),
    LANG_GetTextMessage(TMN_KRONER_N),
    LANG_GetTextMessage(TMN_FRANCS_BELGIUM),
    LANG_GetTextMessage(TMN_DOLLARS_SINGAPORE),
    LANG_GetTextMessage(TMN_DOLLARS_AUSTRALIA),   /* 11th in this list (0-based) */
    LANG_GetTextMessage(TMN_EURO),
  };

  if ( udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].idleID != LED_EI )
  {
    // Clear the old field
    LE_GRAFIX_ColorArea( udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].idleID, 0, 0,
     (short)(udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.left),
     (short)(udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.top),
     LEG_MCR(0, 255, 0) );
    if ( (currency >= 0) && (currency < DISPLAY_CITY_MAX + 1) )
    {
      LE_FONTS_SetSize( udpsel_FONTPOINTSIZE );

      // Test to make sure text will fit.
      width = LE_FONTS_GetStringWidth( currencyNames[currency] );
      if ( width >= (udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.left) )
      {
        // Assume 1 pt size less will fit
        LE_FONTS_SetSize( udpsel_FONTPOINTSIZE - 1 );
      }
      // Now write in the updated text, center justified.
      LANG_Print( udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].idleID,
       (udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.left)/2
        - LE_FONTS_GetStringWidth(currencyNames[currency])/2,
       (udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].rect.top)/2
        - LE_FONTS_GetStringHeight(currencyNames[currency])/2,
       LEG_MCR(255, 255, 255),
       currencyNames[currency] );
      // Restore font size.
      LE_FONTS_SetSize( udpsel_FONTPOINTSIZE );
    }
    LE_SEQNCR_ForceRedraw( udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].idleID,
     udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].priority );
  }
}
#endif


/*****************************************************************************
 * Updates the display of the name field.
 */
static void udpsel_NameScreen_ReShowName( BOOL forceRedraw )
{
  // Clear the old field
  if ( udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].idleID != LED_EI )
  {
    LE_GRAFIX_ColorArea( udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].idleID, 0, 0,
     (short)(udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect.left),
     (short)(udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect.top),
     LEG_MCR(0, 255, 0) );
    // Now write in the updated text, center justified.
    LANG_Print( udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].idleID,
     (udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect.right - udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect.left)/2
      - LE_FONTS_GetStringWidth(UDPSEL_PlayerInfo.name)/2,
     (udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect.bottom - udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].rect.top)/2
      - LE_FONTS_GetStringHeight(UDPSEL_PlayerInfo.name)/2,
     LEG_MCR(255, 255, 255),
     UDPSEL_PlayerInfo.name );
    if ( forceRedraw )
      LE_SEQNCR_ForceRedraw( udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].idleID,
       udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].priority );
  }
}


/*****************************************************************************
 */
static void udpsel_ShowRulesScreenText( void )
{
  if ( udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].idleID != LED_EI )
  {
    LE_SEQNCR_ForceRedraw( udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].idleID,
     udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].priority );
  }
}


/*****************************************************************************
 * Triggers sounds for starting a new phase.
 */
static void udpsel_StartSoundForPhase( int phase )
{
  DISPLAY_state.setupPhaseSoundClipAction = Sound_ClipOldSoundIfPlaying;
  switch ( phase )
  {
  case UDPSEL_PHASE_NONE:
    DISPLAY_state.setupPhaseDesiredSoundIndex = NULL;
    break;

  case UDPSEL_PHASE_HISCORE:
  case UDPSEL_PHASE_LOCALORNETWORK:
    // Play welcome sound if we haven't heard it yet, then play sound for this screen.
    // Note:  If New Game button pressed don't neeed to here welcome.
    if ( !udpsel_WelcomeMessagePlayed )
    {
      udpsel_WelcomeMessagePlayed = TRUE;
      // Do an extra call here, so we can queue up the sounds for this special case.
      UDPENNY_PennybagsGeneral( PB_WelcomeGame, Sound_ClipOldSoundIfPlayingWithLock );
      DISPLAY_state.setupPhasePlayingSoundIndex = PB_WelcomeGame;
      DISPLAY_state.setupPhaseDesiredSoundIndex = PB_WelcomeGame;
    }
    if ( phase == UDPSEL_PHASE_HISCORE )
      DISPLAY_state.setupPhaseDesiredSoundIndex = PB_AnnouncePreviousHighScoresIfAny;
    else
      DISPLAY_state.setupPhaseDesiredSoundIndex = PB_ChooseNetworkOrLocalGame;
    DISPLAY_state.setupPhaseSoundClipAction = Sound_WaitForAnyOldSoundThenPlay;
    break;

  case UDPSEL_PHASE_SELECTPLAYER:
    DISPLAY_state.setupPhaseDesiredSoundIndex = PB_NewPlayerClickNameOrPressButton;
    break;

  case UDPSEL_PHASE_ENTERNAME:
    DISPLAY_state.setupPhaseDesiredSoundIndex = PB_NewPlayerEnterName;
    break;

  case UDPSEL_PHASE_SELECTTOKEN:
    if ( UDPSEL_PlayerInfo.aiLevel == 0 )
      DISPLAY_state.setupPhaseDesiredSoundIndex = PB_HumanPlayerPickToken;
    else
      DISPLAY_state.setupPhaseDesiredSoundIndex = PB_ChooseTokenForComputerPlayer;
    break;

  case UDPSEL_PHASE_STARTADDREMOVE:
    if ( UICurrentGameState.NumberOfPlayers < 2 )
      DISPLAY_state.setupPhaseDesiredSoundIndex = PB_SummaryScreenWithLessThanTwoPlayers;
    else if ( (UICurrentGameState.NumberOfPlayers >= 2) && (UICurrentGameState.NumberOfPlayers < 6) )
      DISPLAY_state.setupPhaseDesiredSoundIndex = PB_SummaryScreenWithTwoToFivePlayers;
    else
      DISPLAY_state.setupPhaseDesiredSoundIndex = PB_SummaryScreenWithSixPlayers;
    break;

  case UDPSEL_PHASE_REMOVEPLAYER:
    DISPLAY_state.setupPhaseDesiredSoundIndex = NULL;
    break;

  case UDPSEL_PHASE_SELECTAISTRENGTH:
    DISPLAY_state.setupPhaseDesiredSoundIndex = PB_ChooseAIDifficultyLevel;
    break;

  case UDPSEL_PHASE_SELECTCITY:
    DISPLAY_state.setupPhaseDesiredSoundIndex = PB_ChooseClassicOrCityBoard_AnyVersion;
    break;

  case UDPSEL_PHASE_STANDARDORCUSTOMRULES:
    DISPLAY_state.setupPhaseDesiredSoundIndex = PB_ChooseStandardOrCustomRules;
    break;

  case UDPSEL_PHASE_CUSTOMIZERULES:
    DISPLAY_state.setupPhaseDesiredSoundIndex = NULL;
    break;
  }
}

#if !USA_VERSION
/*****************************************************************************
 * Puts appropriate currency values for the current country in the currency 
 * selection array.
 */
static void udpsel_ValidateCurrencySelections( void )
{
  // In position 0, put the currency which matches the currently selected country.
  UDPSEL_PlayerInfo.currencySelection[0] = UDPSEL_PlayerInfo.citySelected;
  // In position 1, put the currency which matches the language installed.
  switch ( iLangId )
  {
  case NOTW_LANG_ENGLISH_UK:
    UDPSEL_PlayerInfo.currencySelection[1] = NOTW_MONA_UK;
    break;
  case NOTW_LANG_FRENCH:
    UDPSEL_PlayerInfo.currencySelection[1] = NOTW_MONA_FRENCH;
    break;
  case NOTW_LANG_GERMAN:
    UDPSEL_PlayerInfo.currencySelection[1] = NOTW_MONA_GERMAN;
    break;
  case NOTW_LANG_SPANISH:
    UDPSEL_PlayerInfo.currencySelection[1] = NOTW_MONA_SPANISH;
    break;
  case NOTW_LANG_DUTCH:
    UDPSEL_PlayerInfo.currencySelection[1] = NOTW_MONA_DUTCH;
    break;
  case NOTW_LANG_SWEDISH:
    UDPSEL_PlayerInfo.currencySelection[1] = NOTW_MONA_SWEDISH;
    break;
  case NOTW_LANG_FINNISH:
    UDPSEL_PlayerInfo.currencySelection[1] = NOTW_MONA_FINNISH;
    break;
  case NOTW_LANG_DANISH:
    UDPSEL_PlayerInfo.currencySelection[1] = NOTW_MONA_DANISH;
    break;
  case NOTW_LANG_NORWEGIAN:
    UDPSEL_PlayerInfo.currencySelection[1] = NOTW_MONA_NORWEGIAN;
    break;
  }
  // In position 2, put Euros.
  UDPSEL_PlayerInfo.currencySelection[2] = NOTW_MONA_EURO;


}
#endif

/*****************************************************************************
 * Reads the ini file and cull the necessary player log info from it.
 */
static void udpsel_SelectScreen_ReadPlayerLog( void )
{
  int     i,      j;
  char    playerKeyString[100];
  char    tempString[128];
  wchar_t tempWideString[256];
  BOOL    alreadyExists;

  // Attempt to read all possible player keys from the ini file.
  // (Use "udpsel_PlayerLogMax" as an index, at the end it will also
  // represent a max).
  udpsel_PlayerLogMax = 0;
  for ( i = 0; i < udpsel_HISTORYLOG_MAX_PLAYERS; i++ )
  {
    // Get name for this player.
    sprintf( playerKeyString, "%s%d", udpsel_HISTORYLOG_PLAYERKEY, i+1 );  // BLEEBLE: Not checking for string overrun here.
    GetPrivateProfileString( playerKeyString, udpsel_HISTORYLOG_NAMEKEY, "", tempString,
     udpsel_MAX_ENTERNAME_LENGTH + 1, LE_DIRINI_INI_PathName );
    if ( strcmp(tempString,"") != 0 )
    {
      // It's a real player.
      // However, we shouldn't include the player in the list if they've already
      // been selected and added to the game.
      alreadyExists = FALSE;
      mbstowcs( tempWideString, tempString, udpsel_MAX_ENTERNAME_LENGTH );
      for ( j = 0; j < UICurrentGameState.NumberOfPlayers; j++)
      {
        if ( wcscmp(&UICurrentGameState.Players[j].name[0],tempWideString) == 0 )
        {
          alreadyExists = TRUE;
          break;
        }
      }
      if ( !alreadyExists )
      {
        wcsncpy( &udpsel_PlayerLog[udpsel_PlayerLogMax][0], tempWideString, udpsel_MAX_ENTERNAME_LENGTH );
        udpsel_PlayerLog[udpsel_PlayerLogMax][udpsel_MAX_ENTERNAME_LENGTH] = (wchar_t)NULL;
        udpsel_PlayerLogMax++;
      }
    }
  }
  udpsel_PlayerLogPageStart = 0;
}


/*****************************************************************************
 */
static void udpsel_SelectScreen_PrintCardText( void )
{
  int     i,    j;
  int     x,    y;
  int     w,    h;

  for ( i = 0; i < 8; i++ )
  {
    if ( udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].idleID != LED_EI )
    {
      LE_FONTS_SetSize( udpsel_FONTPOINTSIZE );
      LE_FONTS_SetWeight( udpsel_FONTWEIGHT );
      // Clear overlay first.
      LE_GRAFIX_ColorArea( udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].idleID, 0, 0,
       (short)(udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.right - udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.left),
       (short)(udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.bottom - udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.top),
       LEG_MCR(0, 255, 0) );
      // Write a player name from the log onto the card.
      if ( udpsel_PlayerLogPageStart + i < udpsel_PlayerLogMax )
      {
        // Make sure the text will fit on the card.
        for ( j = 0; j < udpsel_FONTPOINTSIZE; j++ )
        {
          LE_FONTS_SetSize( udpsel_FONTPOINTSIZE - j );
          w = LE_FONTS_GetStringWidth(&udpsel_PlayerLog[udpsel_PlayerLogPageStart+i][0]);
          if ( w < udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.right
           - udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.left )
            break;
        }
        h = LE_FONTS_GetStringHeight(&udpsel_PlayerLog[udpsel_PlayerLogPageStart+i][0]);
        x = (udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.right
         - udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.left - w)/2;
        y = (udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.bottom
         - udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].rect.top - h)/2;
        // Note:  print black text.
        LANG_Print( udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].idleID,
         x, y, LEG_MCR(0,0,0), &udpsel_PlayerLog[udpsel_PlayerLogPageStart+i][0] );
        // Restore font size.
        LE_FONTS_SetSize( udpsel_FONTPOINTSIZE );
      }
    }
  }
}


/*****************************************************************************
 * Sets the desired phase variable so that a switch to a new in phase will
 * occur.  For some phases, does some checks to see if the switch should be
 * to something more appropriate depending on circumstances.
 */
void UDPSEL_SwitchPhase( int phase )
{
  if ( phase == DISPLAY_state.currentPlayerSetupPhase )
    return;

  switch ( phase )
  {
  case UDPSEL_PHASE_HISCORE:
    if ( MESS_GameStartedByLobby && (wcslen(MESS_PlayerNameFromNetwork) > 0) )
    {
      // Okay.  This is the MicroSloth Zone Lobby stuff.  We need to get
      // the player's name automagically.  Cut it down to (our) size.
      wcscpy( UDPSEL_PlayerInfo.name, MESS_PlayerNameFromNetwork );
      // And automagically go immed to choose a token screen.
      DISPLAY_state.desiredPlayerSetupPhase = UDPSEL_PHASE_SELECTTOKEN;
    }
    else
    {
      // If there is no hi-score info then go immediately to the next phase.
      if ( udpsel_PrintHiScoreInfo() )
        DISPLAY_state.desiredPlayerSetupPhase = UDPSEL_PHASE_HISCORE;
      else
        DISPLAY_state.desiredPlayerSetupPhase = UDPSEL_PHASE_LOCALORNETWORK;
    }
    break;

  case UDPSEL_PHASE_SELECTPLAYER:
    // If there is no one in the select player list, go to enter name automatically.
    udpsel_SelectScreen_ReadPlayerLog();
    if ( udpsel_PlayerLogMax > 0 )
      DISPLAY_state.desiredPlayerSetupPhase = UDPSEL_PHASE_SELECTPLAYER;
    else
      DISPLAY_state.desiredPlayerSetupPhase = UDPSEL_PHASE_ENTERNAME;
    break;

  default:
    DISPLAY_state.desiredPlayerSetupPhase = phase;
    break;
  }
}


/*****************************************************************************
 * Triggers non object/anim stuff for a new phase -- as soon as the new phase
 * is requested, ie. before anim-ins are finished.
 */
static void udpsel_StartPhase( int phase )
{
  DISPLAY_state.ShowOnlyLocalPlayersOnIBar = FALSE;
  DISPLAY_state.ShowOnlyLocalAIPlayersOnIBar = FALSE;

  switch ( phase )
  {
  case UDPSEL_PHASE_HISCORE:
    // If there is no hi-score info then go immediately to the next phase.
    if ( !udpsel_PrintHiScoreInfo() )
    {
      DISPLAY_state.previousPlayerSetupPhase = DISPLAY_state.currentPlayerSetupPhase;
      DISPLAY_state.currentPlayerSetupPhase = UDPSEL_PHASE_LOCALORNETWORK;
    }
    break;

  case UDPSEL_PHASE_LOCALORNETWORK:
    break;

  case UDPSEL_PHASE_SELECTPLAYER:
    udpsel_SelectScreen_PrintCardText();
    break;

  case UDPSEL_PHASE_ENTERNAME:
    UDPSEL_PlayerInfo.aiLevel = 0;
    // Show the player name right away, even before animating-in has finished,
    // so impatient players (Tim) can start typing right away.
    udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].desiredID = 
     udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].idleID;
    mbstowcs( UDPSEL_PlayerInfo.name, "_", RULE_MAX_PLAYER_NAME_LENGTH );
    break;

  case UDPSEL_PHASE_SELECTTOKEN:
    if ( UDPSEL_PlayerInfo.aiLevel > 0 )
    {
      // Default for ai's is the next available one.
      UDPSEL_PlayerInfo.token = udpsel_DetermineNextAvailableToken(UDPSEL_PlayerInfo.token);
    }
    else
    {
      // For human's first see if they typed in a recognized token name (ie. Dog).  If they
      // did, make the default that one, otherwise they get the next available one also.
      UDPSEL_PlayerInfo.token = udpsel_GetTokenByName( UDPSEL_PlayerInfo.name );
    }
    break;

  case UDPSEL_PHASE_STARTADDREMOVE:
    break;

  case UDPSEL_PHASE_REMOVEPLAYER:
    if ( MESS_GameStartedByLobby )
      DISPLAY_state.ShowOnlyLocalAIPlayersOnIBar = TRUE;
    else
      DISPLAY_state.ShowOnlyLocalPlayersOnIBar = TRUE;
    break;

  case UDPSEL_PHASE_SELECTAISTRENGTH:
    break;

  case UDPSEL_PHASE_SELECTCITY:
#if !USA_VERSION
    // In Euro version default country/currency to the install language.
    switch ( iLangId )
    {
    default:
    case NOTW_LANG_ENGLISH_UK:
      UDPSEL_PlayerInfo.citySelected = NOTW_MONA_UK;
      break;
    case NOTW_LANG_FRENCH:
      UDPSEL_PlayerInfo.citySelected = NOTW_MONA_FRENCH;
      break;
    case NOTW_LANG_GERMAN:
      UDPSEL_PlayerInfo.citySelected = NOTW_MONA_GERMAN;
      break;
    case NOTW_LANG_SPANISH:
      UDPSEL_PlayerInfo.citySelected = NOTW_MONA_SPANISH;
      break;
    case NOTW_LANG_DUTCH:
      UDPSEL_PlayerInfo.citySelected = NOTW_MONA_DUTCH;
      break;
    case NOTW_LANG_SWEDISH:
      UDPSEL_PlayerInfo.citySelected = NOTW_MONA_SWEDISH;
      break;
    case NOTW_LANG_FINNISH:
      UDPSEL_PlayerInfo.citySelected = NOTW_MONA_FINNISH;
      break;
    case NOTW_LANG_DANISH:
      UDPSEL_PlayerInfo.citySelected = NOTW_MONA_DANISH;
      break;
    case NOTW_LANG_NORWEGIAN:
      UDPSEL_PlayerInfo.citySelected = NOTW_MONA_NORWEGIAN;
      break;
    }
    udpsel_ValidateCurrencySelections();
    UDPSEL_PlayerInfo.currencySelectionIndex = 0;
#endif
    break;

  case UDPSEL_PHASE_STANDARDORCUSTOMRULES:
    break;

  case UDPSEL_PHASE_CUSTOMIZERULES:
    udpsel_PrintRulesTextToOverlay();
    break;
  }
}


/*****************************************************************************
 * Triggers anims etc for starting a new phase.
 */
static void udpsel_StartObjectsForPhase( int phase )
{
  int   i;

  switch ( phase )
  {
  case UDPSEL_PHASE_HISCORE:
    udpsel_Objects[udpsel_OBJ_HISCORE_STATIC].desiredID = 
     udpsel_Objects[udpsel_OBJ_HISCORE_STATIC].idleID;
    udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].desiredID = 
     udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].idleID;
    udpsel_Objects[udpsel_OBJ_HISCORE_BUTTON_PLAY].desiredID = 
     udpsel_Objects[udpsel_OBJ_HISCORE_BUTTON_PLAY].animInID;
    udpsel_Objects[udpsel_OBJ_HISCORE_BUTTON_PLAY].hotspotEnabled = TRUE;
    break;

  case UDPSEL_PHASE_LOCALORNETWORK:
    udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_LOCAL].desiredID = 
     udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_LOCAL].animInID;
    udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_NETWORK].desiredID = 
     udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_NETWORK].animInID;
    udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_SAVED].desiredID = 
     udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_SAVED].animInID;
    udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_LOCAL].hotspotEnabled = TRUE;
    udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_NETWORK].hotspotEnabled = TRUE;
    udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_SAVED].hotspotEnabled = TRUE;
    break;

  case UDPSEL_PHASE_SELECTPLAYER:
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TITLE].desiredID = 
     udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TITLE].idleID;
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_NEW].desiredID = 
     udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_NEW].animInID;
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_NEW].hotspotEnabled = TRUE; 
    if ( udpsel_PlayerLogMax > 8 )
    {
      udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_MORE].desiredID = 
       udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_MORE].animInID;
      udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_MORE].hotspotEnabled = TRUE; 
    }
    for ( i = 0; i < 8; i++ )
    {
      if ( udpsel_PlayerLogPageStart+i < udpsel_PlayerLogMax )
      {
        udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].desiredID = 
         udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].animInID;
        udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].hotspotEnabled = TRUE; 
      }
    }
    break;

  case UDPSEL_PHASE_ENTERNAME:
    udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].desiredID = 
     udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].idleID;
    udpsel_Objects[udpsel_OBJ_ENTERNAME_TITLE].desiredID = 
     udpsel_Objects[udpsel_OBJ_ENTERNAME_TITLE].idleID;
    udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTBOX].desiredID = 
     udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTBOX].animInID;
    // If there are already characters in the name, then bring up the NEXT button,
    // since it is valid.  (Don't forget that our cursor is hard-coded into the name).
    if ( wcslen(UDPSEL_PlayerInfo.name) >= 2 )
    {
      udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].desiredID = 
       udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].idleID;
      udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].hotspotEnabled = TRUE;
    }
    break;

  case UDPSEL_PHASE_SELECTTOKEN:
    udpsel_Objects[udpsel_OBJ_PICKTOKEN_TITLE].desiredID = 
     udpsel_Objects[udpsel_OBJ_PICKTOKEN_TITLE].idleID;
    udpsel_Objects[udpsel_OBJ_PICKTOKEN_STATIC_ROTATING].desiredID = 
     udpsel_Objects[udpsel_OBJ_PICKTOKEN_STATIC_ROTATING].animInID;
    for ( i = 0; i < MAX_TOKENS; i++ )
    {
      udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_CANNON + i].desiredID = LED_EI;
      if ( udpsel_TokenIsAvailable(i) )
      {
        udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_CANNON + i].desiredID = 
         udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_CANNON + i].animInID;
        udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_CANNON + i].hotspotEnabled = TRUE;
      }
    }
    udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_NEXT].desiredID = 
     udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_NEXT].animInID;
    udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_NEXT].hotspotEnabled = TRUE;
    break;

  case UDPSEL_PHASE_STARTADDREMOVE:
    if ( UICurrentGameState.NumberOfPlayers < 6 )
    {
      // MS Zone games can only have one human per machine.  A few AIs,
      // though, are allowed, but only the host can add those.
      if ( !MESS_GameStartedByLobby )
      {
        udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDHUMAN].desiredID = 
         udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDHUMAN].animInID;
        udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDHUMAN].hotspotEnabled = TRUE;
      }
      if ( !MESS_GameStartedByLobby || MESS_ServerMode )
      {
        udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDCOMPUTER].desiredID = 
         udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDCOMPUTER].animInID;
        udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDCOMPUTER].hotspotEnabled = TRUE;
      }
    }
    // In MS Zone games you can't remove anything unless you are the host, and
    // even then you can only remove AIs.  Other than that there is one
    // human player (you) which you can't remove.
    if ( (!MESS_GameStartedByLobby && (NumberOfLocalPlayers > 0))
     || (MESS_ServerMode && (NumberOfLocalPlayers > 1)) )
    {
      udpsel_Objects[udpsel_OBJ_START_BUTTON_REMOVEPLAYER].desiredID = 
       udpsel_Objects[udpsel_OBJ_START_BUTTON_REMOVEPLAYER].animInID;
      udpsel_Objects[udpsel_OBJ_START_BUTTON_REMOVEPLAYER].hotspotEnabled = TRUE;
    }
    // Only host can start the game.
    if ( MESS_ServerMode && (UICurrentGameState.NumberOfPlayers >= 2) && (udpsel_NumberOfHumanPlayers() >= 1) )
    {
      udpsel_Objects[udpsel_OBJ_START_BUTTON_STARTGAME].desiredID = 
       udpsel_Objects[udpsel_OBJ_START_BUTTON_STARTGAME].animInID;
      udpsel_Objects[udpsel_OBJ_START_BUTTON_STARTGAME].hotspotEnabled = TRUE;
    }
    break;

  case UDPSEL_PHASE_REMOVEPLAYER:
    udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].desiredID = 
     udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].idleID;
    udpsel_Objects[udpsel_OBJ_REMOVE_BUTTON_CANCEL].desiredID = 
     udpsel_Objects[udpsel_OBJ_REMOVE_BUTTON_CANCEL].animInID;
    udpsel_Objects[udpsel_OBJ_REMOVE_BUTTON_CANCEL].hotspotEnabled = TRUE;
    break;

  case UDPSEL_PHASE_SELECTAISTRENGTH:
    udpsel_Objects[udpsel_OBJ_AISTRENGTH_TITLE].desiredID = 
     udpsel_Objects[udpsel_OBJ_AISTRENGTH_TITLE].idleID;
    udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_EASY].desiredID = 
     udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_EASY].animInID;
    udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_MEDIUM].desiredID = 
     udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_MEDIUM].animInID;
    udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_HARD].desiredID = 
     udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_HARD].animInID;
    udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_EASY].hotspotEnabled = TRUE;
    udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_MEDIUM].hotspotEnabled = TRUE;
    udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_HARD].hotspotEnabled = TRUE;
    break;

  case UDPSEL_PHASE_SELECTCITY:
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_CLASSICBOARD].desiredID = 
     udpsel_Objects[udpsel_OBJ_CITY_BUTTON_CLASSICBOARD].animInID;
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LOADBOARD].desiredID = 
     udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LOADBOARD].animInID;
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_CLASSICBOARD].hotspotEnabled = TRUE;
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LOADBOARD].hotspotEnabled = TRUE;
#if USA_VERSION
    udpsel_Objects[udpsel_OBJ_CITY_STATIC].desiredID = 
     udpsel_Objects[udpsel_OBJ_CITY_STATIC].animInID;
    udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].desiredID = 
     udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].idleID;
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LEFT].desiredID = 
     udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LEFT].animInID;
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_RIGHT].desiredID = 
     udpsel_Objects[udpsel_OBJ_CITY_BUTTON_RIGHT].animInID;
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LEFT].hotspotEnabled = TRUE;
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_RIGHT].hotspotEnabled = TRUE;
#else
    udpsel_Objects[udpsel_OBJ_COUNTRY_STATIC].desiredID = 
     udpsel_Objects[udpsel_OBJ_COUNTRY_STATIC].animInID;
    udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].desiredID = 
     udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].idleID;
    udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_LEFT].desiredID = 
     udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_LEFT].animInID;
    udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_RIGHT].desiredID = 
     udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_RIGHT].animInID;
    udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_LEFT].hotspotEnabled = TRUE;
    udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_RIGHT].hotspotEnabled = TRUE;
    udpsel_Objects[udpsel_OBJ_CURRENCY_STATIC].desiredID = 
     udpsel_Objects[udpsel_OBJ_CURRENCY_STATIC].animInID;
    udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].desiredID = 
     udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].idleID;
    udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_LEFT].desiredID = 
     udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_LEFT].animInID;
    udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_RIGHT].desiredID = 
     udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_RIGHT].animInID;
    udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_LEFT].hotspotEnabled = TRUE;
    udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_RIGHT].hotspotEnabled = TRUE;
#endif
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_NEXT].desiredID = 
     udpsel_Objects[udpsel_OBJ_CITY_BUTTON_NEXT].animInID;
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_NEXT].hotspotEnabled = TRUE;
    break;

  case UDPSEL_PHASE_STANDARDORCUSTOMRULES:
    udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_STANDARD].desiredID = 
     udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_STANDARD].animInID;
    udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_CUSTOM].desiredID = 
     udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_CUSTOM].animInID;
    udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_STANDARD].hotspotEnabled = TRUE;
    udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_CUSTOM].hotspotEnabled = TRUE;
    break;

  case UDPSEL_PHASE_CUSTOMIZERULES:
    // Set up for an animated toggle.
    for ( i = udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4; i < udpsel_OBJLAST_RULES; i++ )
    {
      udpsel_Objects[i].postAnimInID = udpsel_Objects[i].idleID;
      udpsel_Objects[i].postAnimOutID = udpsel_Objects[i].pressID;
    }
    udpsel_Objects[udpsel_OBJ_RULES_TITLE].desiredID = 
     udpsel_Objects[udpsel_OBJ_RULES_TITLE].idleID;
    udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].desiredID = 
     udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].idleID; 
    udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].desiredID = 
     udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].animInID; 
    udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].hotspotEnabled = TRUE;
    udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SHORTGAME].desiredID = 
     udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SHORTGAME].animInID; 
    udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SHORTGAME].hotspotEnabled = TRUE;
    if ( MESS_ServerMode )
    {
      for ( i = udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4; i < udpsel_OBJLAST_RULES; i++ )
        udpsel_Objects[i].hotspotEnabled = TRUE;
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_OKAY].desiredID = 
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_OKAY].animInID;
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_OKAY].hotspotEnabled = TRUE;
    }
    UDBOARD_SetBackdrop( DISPLAY_SCREEN_PselectRules );
    udpsel_UpdateRulesScreenButtons( &UICurrentGameState.GameOptions, &udpsel_PreviousGameOptions, TRUE );
    break;
  }
}


/*****************************************************************************
 * Triggers the end of all anims and disables all hotspots.
 */
static void udpsel_KillAllObjects( void )
{
  int   i;

  for ( i = 0; i < udpsel_OBJ_MAX; i++ )
  {
    udpsel_Objects[i].desiredID = LED_EI;
    udpsel_Objects[i].hotspotEnabled = FALSE;
  }
}


/*****************************************************************************
 * Triggers anims etc for a ending a phase.
 */
static void udpsel_EndObjectsForPhase( int phase )
{
  int   i;

  switch ( phase )
  {
  case UDPSEL_PHASE_HISCORE:
    udpsel_Objects[udpsel_OBJ_HISCORE_STATIC].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_HISCORE_TEXTOVERLAY].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_HISCORE_BUTTON_PLAY].desiredID = 
     udpsel_Objects[udpsel_OBJ_HISCORE_BUTTON_PLAY].animOutID;
    break;

  case UDPSEL_PHASE_LOCALORNETWORK:
    udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_LOCAL].desiredID = 
     udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_LOCAL].animOutID;
    udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_NETWORK].desiredID = 
     udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_NETWORK].animOutID;
    udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_SAVED].desiredID = 
     udpsel_Objects[udpsel_OBJ_LOCALNETWORK_BUTTON_SAVED].animOutID;
    break;

  case UDPSEL_PHASE_SELECTPLAYER:
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TITLE].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_NEW].desiredID = 
     udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_NEW].animOutID;
    if ( udpsel_PlayerLogMax > 8 )
    {
      udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_MORE].desiredID = 
       udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_MORE].animOutID;
    }
    for ( i = 0; i < 8; i++ )
    {
      if ( udpsel_PlayerLogPageStart+i < udpsel_PlayerLogMax )
      {
        udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].desiredID = 
         udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].animOutID;
      }
      udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].desiredID = LED_EI;
    }
    break;

  case UDPSEL_PHASE_ENTERNAME:
    udpsel_Objects[udpsel_OBJ_ENTERNAME_TITLE].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTBOX].desiredID = 
     udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTBOX].animOutID;
    udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].desiredID = LED_EI;
    break;

  case UDPSEL_PHASE_SELECTTOKEN:
    udpsel_Objects[udpsel_OBJ_PICKTOKEN_TITLE].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_PICKTOKEN_STATIC_ROTATING].desiredID = 
     udpsel_Objects[udpsel_OBJ_PICKTOKEN_STATIC_ROTATING].animOutID;
    for ( i = 0; i < MAX_TOKENS; i++ )
    {
      udpsel_Objects[udpsel_OBJ_PICKTOKEN_ROTATING_CANNON + i].desiredID = LED_EI;
      if ( udpsel_TokenIsAvailable(i) )
      {
        udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_CANNON + i].desiredID = 
         udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_CANNON + i].animOutID;
      }
      else
      {
        udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_CANNON + i].desiredID = LED_EI;
      }
    }
    udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_NEXT].desiredID = 
     udpsel_Objects[udpsel_OBJ_PICKTOKEN_BUTTON_NEXT].animOutID;
    break;

  case UDPSEL_PHASE_STARTADDREMOVE:
    if ( UICurrentGameState.NumberOfPlayers < 6 )
    {
      if ( !MESS_GameStartedByLobby )
      {
        udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDHUMAN].desiredID = 
         udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDHUMAN].animOutID;
      }
      if ( !MESS_GameStartedByLobby || MESS_ServerMode )
      {
        udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDCOMPUTER].desiredID = 
         udpsel_Objects[udpsel_OBJ_START_BUTTON_ADDCOMPUTER].animOutID;
      }
    }
    if ( (!MESS_GameStartedByLobby && (NumberOfLocalPlayers > 0))
     || (MESS_ServerMode && (NumberOfLocalPlayers > 1)) )
    {
      udpsel_Objects[udpsel_OBJ_START_BUTTON_REMOVEPLAYER].desiredID = 
       udpsel_Objects[udpsel_OBJ_START_BUTTON_REMOVEPLAYER].animOutID;
    }
    if ( MESS_ServerMode && (UICurrentGameState.NumberOfPlayers >= 2) && (udpsel_NumberOfHumanPlayers() >= 1) )
    {
      udpsel_Objects[udpsel_OBJ_START_BUTTON_STARTGAME].desiredID = 
       udpsel_Objects[udpsel_OBJ_START_BUTTON_STARTGAME].animOutID;
    }
    break;

  case UDPSEL_PHASE_REMOVEPLAYER:
    udpsel_Objects[udpsel_OBJ_REMOVE_TEXTOVERLAY].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_REMOVE_BUTTON_CANCEL].desiredID = 
     udpsel_Objects[udpsel_OBJ_REMOVE_BUTTON_CANCEL].animOutID;
    break;

  case UDPSEL_PHASE_SELECTAISTRENGTH:
    udpsel_Objects[udpsel_OBJ_AISTRENGTH_TITLE].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_EASY].desiredID = 
     udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_EASY].animOutID;
    udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_MEDIUM].desiredID = 
     udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_MEDIUM].animOutID;
    udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_HARD].desiredID = 
     udpsel_Objects[udpsel_OBJ_AISTRENGTH_BUTTON_HARD].animOutID;
    break;

  case UDPSEL_PHASE_SELECTCITY:
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_CLASSICBOARD].desiredID = 
     udpsel_Objects[udpsel_OBJ_CITY_BUTTON_CLASSICBOARD].animOutID;
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LOADBOARD].desiredID = 
     udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LOADBOARD].animOutID;
#if USA_VERSION
    udpsel_Objects[udpsel_OBJ_CITY_STATIC].desiredID = 
     udpsel_Objects[udpsel_OBJ_CITY_STATIC].animOutID;
    udpsel_Objects[udpsel_OBJ_CITY_TEXTOVERLAY].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_LEFT].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_RIGHT].desiredID = LED_EI;
#else
    udpsel_Objects[udpsel_OBJ_COUNTRY_STATIC].desiredID = 
     udpsel_Objects[udpsel_OBJ_COUNTRY_STATIC].animOutID;
    udpsel_Objects[udpsel_OBJ_COUNTRY_TEXTOVERLAY].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_LEFT].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_COUNTRY_BUTTON_RIGHT].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_CURRENCY_STATIC].desiredID = 
     udpsel_Objects[udpsel_OBJ_CURRENCY_STATIC].animOutID;
    udpsel_Objects[udpsel_OBJ_CURRENCY_TEXTOVERLAY].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_LEFT].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_CURRENCY_BUTTON_RIGHT].desiredID =  LED_EI;
#endif
    udpsel_Objects[udpsel_OBJ_CITY_BUTTON_NEXT].desiredID = 
     udpsel_Objects[udpsel_OBJ_CITY_BUTTON_NEXT].animOutID;
    break;

  case UDPSEL_PHASE_STANDARDORCUSTOMRULES:
    udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_STANDARD].desiredID = 
     udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_STANDARD].animOutID;
    udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_CUSTOM].desiredID = 
     udpsel_Objects[udpsel_OBJ_RULESCHOICE_BUTTON_CUSTOM].animOutID;
    break;

  case UDPSEL_PHASE_CUSTOMIZERULES:
    udpsel_Objects[udpsel_OBJ_RULES_TITLE].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_RULES_TEXTOVERLAY].desiredID = LED_EI;
    udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].desiredID = 
     udpsel_Objects[udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD].animOutID;
    udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SHORTGAME].desiredID = 
     udpsel_Objects[udpsel_OBJ_RULES_BUTTON_SHORTGAME].animOutID;
    // Turn off all the buttons which are beside a rule option
    for ( i = udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4; i < udpsel_OBJLAST_RULES; i++ )
      udpsel_Objects[i].desiredID = LED_EI;
    if ( MESS_ServerMode )
    {
      udpsel_Objects[udpsel_OBJ_RULES_BUTTON_OKAY].desiredID = 
       udpsel_Objects[udpsel_OBJ_RULES_BUTTON_OKAY].animOutID;
    }
    break;
  }
}


/*****************************************************************************
 * Checks everything in our array of what could possibly be on screen and change
 * to newly desired stuff if required.  
 */
static void udpsel_UpdateRuntimeGraphics( void )
{
  int                               i;
  LE_SEQNCR_RuntimeInfoRecord       InfoGeneric;

  for ( i = 0; i < udpsel_OBJ_MAX; i++ )
  {
    if ( (udpsel_Objects[i].currentID != LED_EI)
     && ((udpsel_Objects[i].currentID == udpsel_Objects[i].animInID)
     || (udpsel_Objects[i].currentID == udpsel_Objects[i].animOutID)) )
    {
      // For flying in and out anims, let them finish first.
      LE_SEQNCR_GetInfo( udpsel_Objects[i].currentID, udpsel_Objects[i].priority, FALSE,
       &InfoGeneric, NULL, NULL, NULL, NULL );
      if ( InfoGeneric.endTime <= InfoGeneric.sequenceClock )
      {
        // The anim has now finished.
        // Cause the anim-in to automagically switch to its next state.
        if ( udpsel_Objects[i].currentID == udpsel_Objects[i].animInID )
          udpsel_Objects[i].desiredID = udpsel_Objects[i].postAnimInID;
        if ( udpsel_Objects[i].currentID == udpsel_Objects[i].animOutID )
          udpsel_Objects[i].desiredID = udpsel_Objects[i].postAnimOutID;
      }
    }
    udpsel_GeneralizedAnimUpdate( &(udpsel_Objects[i].currentID),
     udpsel_Objects[i].desiredID,
     udpsel_Objects[i].priority,
     udpsel_Objects[i].startX, udpsel_Objects[i].startY,
     udpsel_Objects[i].endingAction );
  }
}

/*****************************************************************************
 * Called from UDIBar code.  A player button at the bottom was clicked.
 * See if we are at the remove player screen where this means something.
 */
void UDPSEL_PlayerButtonClicked( int playerNumber )
{
  if ( DISPLAY_state.currentPlayerSetupPhase == UDPSEL_PHASE_REMOVEPLAYER )
  {
    RemoveLocalPlayer( playerNumber );
    UDPSEL_SwitchPhase( UDPSEL_PHASE_STARTADDREMOVE );
  }
}


/*****************************************************************************
 * Recalculate what should be on the screen, compared to what is on the screen,
 * and take corrective action.  Everything is in the user interface and display states.
 */
void DISPLAY_UDPSEL_Show( void )
{
  int     i;

  if ( (DISPLAY_state.desired2DView == DISPLAY_SCREEN_Pselect)
   || (DISPLAY_state.desired2DView == DISPLAY_SCREEN_PselectRules) )
  {
    /* Check phase and trigger stuff if required. */

    if ( DISPLAY_state.desiredPlayerSetupPhase != DISPLAY_state.currentPlayerSetupPhase )
    {
      // Start the switch over to the new phase.  Trigger anim-outs.
      udpsel_EndObjectsForPhase( DISPLAY_state.currentPlayerSetupPhase );
      udpsel_AnimOutsFinished = FALSE;
      DISPLAY_state.currentPlayerSetupPhase = DISPLAY_state.desiredPlayerSetupPhase;
    }
    else if ( DISPLAY_state.previousPlayerSetupPhase != DISPLAY_state.currentPlayerSetupPhase )
    {
      // curr and prev are not same, so must be in the process of switching (animating out).
      // See if anim outs are done yet.
      if ( !udpsel_AnimOutsFinished )
      {
        if ( !udpsel_PhaseIsAnimating(DISPLAY_state.previousPlayerSetupPhase,FALSE) )
        {
          udpsel_AnimOutsFinished = TRUE;
          if ( udpsel_DelayedSwitchView != -1 )
          {
            // Switch to a completely different view.
            UDBOARD_SetBackdrop( udpsel_DelayedSwitchView );
            udpsel_GraphicsShowing = FALSE;
            udpsel_DelayedSwitchView = -1;
          }
          else
          {
            // Official switch over point to the new phase.  Anim-outs are done,
            // start the anim-ins etc.
            DISPLAY_state.previousPlayerSetupPhase = DISPLAY_state.currentPlayerSetupPhase;
            udpsel_KillAllObjects();
            udpsel_StartPhase( DISPLAY_state.currentPlayerSetupPhase );
            udpsel_StartObjectsForPhase( DISPLAY_state.currentPlayerSetupPhase );
            udpsel_StartSoundForPhase( DISPLAY_state.currentPlayerSetupPhase );
            udpsel_AnimInsFinished = FALSE;
            udpsel_GraphicsShowing = TRUE;
          }
        }
      }
    }
    else
    {
      // See if we are in the anim-in portion of a phase switch
      if ( !udpsel_AnimInsFinished )
      {
        if ( !udpsel_PhaseIsAnimating(DISPLAY_state.currentPlayerSetupPhase,TRUE) )
        {
          // Do any special stuff that must wait until the "in" anims are done.
          udpsel_AnimInsFinished = TRUE;
          switch ( DISPLAY_state.currentPlayerSetupPhase )
          {
          case UDPSEL_PHASE_SELECTPLAYER:
            // Don't show the text until the anim-ins are finished.  Make them
            // all active -- if nothing is to be seen it will have no text in it.
            for ( i = 0; i < 8; i++ )
            {
              udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].desiredID = 
               udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].idleID;
            }
            break;
          case UDPSEL_PHASE_ENTERNAME:
            // Special update call so we have text to see the very first time.
            udpsel_GeneralizedAnimUpdate( &(udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].currentID),
             udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].desiredID,
             udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].priority,
             udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].startX, udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].startY,
             udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].endingAction );
            udpsel_NameScreen_ReShowName( TRUE );
            break;
          case UDPSEL_PHASE_SELECTTOKEN:
            // Start the appropriate rotating image for the currently selected token
            // after the anim ins have settled down.
            udpsel_TokenScreen_ShowRotatingToken( UDPSEL_PlayerInfo.token );
            break;
          case UDPSEL_PHASE_CUSTOMIZERULES:
            udpsel_ShowRulesScreenText();
            break;
          case UDPSEL_PHASE_SELECTCITY:
#if USA_VERSION
            udpsel_ShowCityName( UDPSEL_PlayerInfo.citySelected );
#else
            udpsel_ShowCountryName( UDPSEL_PlayerInfo.citySelected );
            udpsel_ValidateCurrencySelections();
            udpsel_ShowCurrencyText( UDPSEL_PlayerInfo.currencySelection[UDPSEL_PlayerInfo.currencySelectionIndex] );
#endif
            break;
          }
        }
      }
      if ( !udpsel_GraphicsShowing )
      {
        // We suddenly switched away with one of the options buttons (or F8 programmer hotkey)
        // and we are now switching back.
        udpsel_GraphicsShowing = TRUE;
        // Get the current phase going again since we switched away from it.
        udpsel_KillAllObjects();
        udpsel_StartObjectsForPhase( DISPLAY_state.currentPlayerSetupPhase );
        udpsel_StartSoundForPhase( DISPLAY_state.currentPlayerSetupPhase );
        udpsel_AnimInsFinished = FALSE;
      }
    }

    if ( udpsel_ForcedRefresh )
    {
      udpsel_ForcedRefresh = FALSE;
      // Forced refresh, probably due to a rules engine message that we got more players
      // or something.  Restart the setup screen.
      udpsel_KillAllObjects();
      udpsel_StartObjectsForPhase( DISPLAY_state.currentPlayerSetupPhase );
      udpsel_StartSoundForPhase( DISPLAY_state.currentPlayerSetupPhase );
      udpsel_StartPhase( DISPLAY_state.currentPlayerSetupPhase );
      udpsel_AnimInsFinished = FALSE;
      udpsel_GraphicsShowing = TRUE;
    }


    /* Runtime update.  Check desired versus current and start stop anims
       as required including fly-ins and fly-outs. */

    udpsel_UpdateRuntimeGraphics();

    /* Check sound. */

    if ( DISPLAY_state.setupPhasePlayingSoundIndex != DISPLAY_state.setupPhaseDesiredSoundIndex )
    {
      DISPLAY_state.setupPhasePlayingSoundIndex = DISPLAY_state.setupPhaseDesiredSoundIndex;
      if ( DISPLAY_state.setupPhasePlayingSoundIndex != NULL )
      {
        UDPENNY_PennybagsGeneral( DISPLAY_state.setupPhasePlayingSoundIndex, 
         DISPLAY_state.setupPhaseSoundClipAction );
      }
    }
  }
  else
  {
    if ( udpsel_GraphicsShowing )
    {
      udpsel_GraphicsShowing = FALSE;

      // Turn everything off.
      udpsel_KillAllObjects();

      // Extra call to actually stop everything.
      udpsel_UpdateRuntimeGraphics();
    }
  }
}


/*****************************************************************************
 * Force an engine update of the display for the card text overlays.
 */
static void udpsel_SelectScreen_ShowCardText( void )
{
  int     i;

  for ( i = 0; i < 8; i++ )
  {
    if ( udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].idleID != LED_EI )
    {
      LE_SEQNCR_ForceRedraw( udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].idleID,
       udpsel_Objects[udpsel_OBJ_SELECTPLAYER_TEXTOVERLAY_CARD1+i].priority );
    }
  }
}


/*****************************************************************************
 * This function is for coders who need to switch away from the player select
 * screens, but want to wait for the nice animations to finish first.
 */
void UDPSEL_SwitchViewAfterAnimOuts( int desiredView )
{
  udpsel_EndObjectsForPhase( DISPLAY_state.currentPlayerSetupPhase );
  udpsel_AnimOutsFinished = FALSE;
  udpsel_DelayedSwitchView = desiredView;
}


/*****************************************************************************
 * Returns TRUE if a letter was actually added, FALSE otherwise.
 */
static BOOL udpsel_NameScreen_AddLetterToNameField( wchar_t nLetter )
{
  int nLength;

  // Make sure it is a printable character.
  if ( (nLetter < 32) || ((nLetter >= 128) && (nLetter < 128 + 32)) )
    return FALSE;

  // Make sure there is enough room to add the new character.
  // Remember that the cursor takes up one spot.
  nLength = wcslen(UDPSEL_PlayerInfo.name);
  if ( udpsel_MAX_ENTERNAME_LENGTH <= (nLength - 1) )
    return FALSE;

  UDPSEL_PlayerInfo.name[nLength - 1] = nLetter;
  UDPSEL_PlayerInfo.name[nLength]     = '_';
  UDPSEL_PlayerInfo.name[nLength + 1] = 0;

  return TRUE;
}


/*****************************************************************************
 * Returns TRUE if a letter was actually removed, FALSE otherwise.
 */
static BOOL udpsel_NameScreen_RemoveLetterFromNameField( void )
{
  int nLength;
  
  nLength = wcslen(UDPSEL_PlayerInfo.name);

  // If it is just the cursor, don't bother.
  if ( (1 == nLength) && (UDPSEL_PlayerInfo.name[0] == '_') )
    return FALSE;
  // Take care of simple case and unlikely stuff. eg. "a", "bb"
  if ( nLength <= 2 )
  {
    UDPSEL_PlayerInfo.name[0] = '_';
    UDPSEL_PlayerInfo.name[1] = 0;
    return TRUE;
  }

  if ( UDPSEL_PlayerInfo.name[nLength - 1] == '_' )
  {
    // Remove the character before the cursor.
    UDPSEL_PlayerInfo.name[nLength - 2] = '_';
    UDPSEL_PlayerInfo.name[nLength - 1] = 0;
  }
  else
  {
    // For some strange reason there was no cursor.
    // So, remove the last character and then add a cursor.
    UDPSEL_PlayerInfo.name[nLength - 1] = '_';
    UDPSEL_PlayerInfo.name[nLength] = 0;
  }

  return TRUE;
}


/*****************************************************************************
 * One of the tokens has been selected.  Change display and tracking variables
 * appropriately.
 */
static void udpsel_TokenScreen_OnATokenButton( RULE_TokenKind token )
{
  if ( (token >= 0) && (token < MAX_TOKENS) )
  {
    if ( UDPSEL_PlayerInfo.token != token )
    {
      UDPSEL_PlayerInfo.token = token;
      udpsel_TokenScreen_ShowRotatingToken( token );
    }
  }
}


/*****************************************************************************
 * Does stuff that the player select needs to do when the game is starting.
 */
void UDPSEL_GameHasJustStarted( void )
{
  int     i,    j;
  int     freePlayerIndex;
  char    playerKeyString[100];
  char    tempString[128];
  wchar_t tempWideString[256];
  BOOL    alreadyExists;

  // Clear the flag for winner's info processed.
  udpsel_WinnerProcessed = FALSE;

  udpsel_NumberOfPlayers = -1;

  // Add each of our players to the player log.  Make it official.
  for ( i = 0; i < UICurrentGameState.NumberOfPlayers; i++ )
  {
    // Don't bother adding AIs or non-local players.
    if ( (UICurrentGameState.Players[i].AIPlayerLevel != 0) && SlotIsALocalPlayer[i] )
      continue;

    // See if they already exist, first.
    alreadyExists = FALSE;
    freePlayerIndex = -1;
    for ( j = 0; j < udpsel_HISTORYLOG_MAX_PLAYERS; j++ )
    {
      // Get name for this player.
      sprintf( playerKeyString, "%s%d", udpsel_HISTORYLOG_PLAYERKEY, j+1 );  // BLEEBLE: Not checking for string overrun here.
      GetPrivateProfileString( playerKeyString, udpsel_HISTORYLOG_NAMEKEY, "", tempString, 256,
       LE_DIRINI_INI_PathName );
      if ( strcmp(tempString,"") == 0 )
      {
        // Found an unused slot.  Remember the first such one we enccounter.
        // We will need it if we end up adding the player.
        if ( freePlayerIndex < 0 )
            freePlayerIndex = j;
      }
      else
      {
        // It's a real name, so see if it's already been selected
        mbstowcs( tempWideString, tempString, 255 );
        if ( wcscmp(&UICurrentGameState.Players[i].name[0],tempWideString) == 0 )
        {
          alreadyExists = TRUE;
          break;
        }
      }
    }
    if ( !alreadyExists )
    {
      // Need to add the player.
      // Use an unused spot, if possible.  BLEEBLE:  Later when date last played info
      // is kept we can overwrite the oldest player.
      if ( freePlayerIndex < 0 )
        freePlayerIndex = 0;
      sprintf( playerKeyString, "%s%d", udpsel_HISTORYLOG_PLAYERKEY, freePlayerIndex+1 );  // BLEEBLE: Not checking for string overrun here.
      wcstombs( tempString, &UICurrentGameState.Players[i].name[0], RULE_MAX_PLAYER_NAME_LENGTH );
      WritePrivateProfileString( playerKeyString, udpsel_HISTORYLOG_NAMEKEY, tempString, 
       LE_DIRINI_INI_PathName );
      sprintf( tempString, "%d", 0 );
      WritePrivateProfileString( playerKeyString, udpsel_HISTORYLOG_WINSKEY, tempString, 
       LE_DIRINI_INI_PathName );
      WritePrivateProfileString( playerKeyString, udpsel_HISTORYLOG_NETWORTHKEY, tempString, 
       LE_DIRINI_INI_PathName );
    }
  }
}


/*****************************************************************************
 * Credits the winner in the ini file where hi-score info is kept with an
 * additional win, and possibly, a new greatest net worth.
 */
void UDPSEL_GameOver( wchar_t *name, int winnerNetWorth, int aiLevel )
{
  int     i;
  char    playerKeyString[100];
  char    tempString[128];
  wchar_t tempWideString[256];
  int     currentWins;
  int     currentGreatestNetWorth;

  // The game over message is broadcast to all players and unfortunately there
  // is not a cleaner way to detect if the winner has been creditted with his
  // win already.
  if ( udpsel_WinnerProcessed )
    return;
  udpsel_WinnerProcessed = TRUE;

  // Only record hi-scores of humans.
  if ( aiLevel == 0 )
  {
    // First find the player's name in the ini.
    for ( i = 0; i < udpsel_HISTORYLOG_MAX_PLAYERS; i++ )
    {
      // Get wins, net worth info for this player.
      sprintf( playerKeyString, "%s%d", udpsel_HISTORYLOG_PLAYERKEY, i+1 );  // BLEEBLE: Not checking for string overrun here.
      GetPrivateProfileString( playerKeyString, udpsel_HISTORYLOG_NAMEKEY, "", tempString, 128,
       LE_DIRINI_INI_PathName );
      mbstowcs( tempWideString, tempString, 256 );
      if ( wcscmp(name,tempWideString) == 0 )
      {
        // Found the player.  Now get wins.
        GetPrivateProfileString( playerKeyString, udpsel_HISTORYLOG_WINSKEY, "", tempString, 128,
         LE_DIRINI_INI_PathName );
        currentWins = atoi( tempString );
        // Get players greatest net worth.
        GetPrivateProfileString( playerKeyString, udpsel_HISTORYLOG_NETWORTHKEY, "", tempString, 256,
         LE_DIRINI_INI_PathName );
        currentGreatestNetWorth = atoi( tempString );
        // Credit an additional win.
        currentWins++;
        sprintf( tempString, "%d", currentWins );
        WritePrivateProfileString( playerKeyString, udpsel_HISTORYLOG_WINSKEY, tempString, LE_DIRINI_INI_PathName );
        // Check the greatest net worth and see if the player has achieved a new high.
        if ( winnerNetWorth > currentGreatestNetWorth )
        {
          sprintf( tempString, "%d", winnerNetWorth );
          WritePrivateProfileString( playerKeyString, udpsel_HISTORYLOG_NETWORTHKEY, tempString, LE_DIRINI_INI_PathName );
        }
        break;
      }
    }
  }
}


/*****************************************************************************
 * Standard UI subfunction - Process library message.
 */
void UDPSEL_ProcessMessage( LE_QUEUE_MessagePointer pUIMessage )
{
  int   i;
  int   buttonIndex;
  POINT mousePoint;

  if ( (DISPLAY_state.desired2DView == DISPLAY_SCREEN_Pselect)
   || (DISPLAY_state.desired2DView == DISPLAY_SCREEN_PselectRules) )
  {
    buttonIndex = -1;
    if ( UIMSG_MOUSE_LEFT_DOWN == pUIMessage->messageCode )
    {
      mousePoint.x = pUIMessage->numberA;
      mousePoint.y = pUIMessage->numberB;

      // Figure out if we are over a button
      for ( i = 0; i < udpsel_OBJ_MAX; i++ )
      {
        if ( udpsel_Objects[i].hotspotEnabled )
        {
          if ( PtInRect(&udpsel_Objects[i].rect,mousePoint) ) 
          {
            buttonIndex = i;
            break;
          }
        }
      }
      switch ( buttonIndex )
      {
      case udpsel_OBJ_HISCORE_BUTTON_PLAY:
        UDPSEL_SwitchPhase( UDPSEL_PHASE_LOCALORNETWORK );
        break;

      case udpsel_OBJ_LOCALNETWORK_BUTTON_LOCAL:
          // Shut down any previous network connection.
        CHAT_Close();
        MESS_StopAllNetworking();
        // Now do it.
        UDPSEL_SwitchPhase( UDPSEL_PHASE_SELECTPLAYER );
        break;

      case udpsel_OBJ_LOCALNETWORK_BUTTON_NETWORK:
        {
          static ACHAR* szCommandLine = A_T("-directplay");

          // Shut down any previous network connection.
          CHAT_Close();
          MESS_StopAllNetworking();

          // Start new one.
          pc3D::Pause(TRUE);
          ShowCursor(TRUE);
          if ( MESS_StartNetworking(szCommandLine) )
          {
            // Automatically start chatting as soon as network mode established.
            CHAT_Open();
          }
          ShowCursor(FALSE);
          pc3D::Pause(FALSE);
        }
        if ( MESS_NetworkMode )
          UDPSEL_SwitchPhase( UDPSEL_PHASE_SELECTPLAYER );
        else
          UDPSEL_SwitchPhase( UDPSEL_PHASE_LOCALORNETWORK );
        break;

      case udpsel_OBJ_LOCALNETWORK_BUTTON_SAVED:
        last_playerselectscreen = DISPLAY_state.desired2DView;
        last_playerselectphase = DISPLAY_state.currentPlayerSetupPhase;

        last_options_result = -1;
        UDBOARD_SetBackdrop( DISPLAY_SCREEN_Options );
        UDIBar_ProcessOptionsButtonPress(FILE_SCREEN);
        current_options_result = LOAD_GAME_SCREEN;   
        break;

      case udpsel_OBJ_SELECTPLAYER_BUTTON_NEW:
        UDPSEL_PlayerInfo.aiLevel = 0;
        UDPSEL_SwitchPhase( UDPSEL_PHASE_ENTERNAME );
        break;

      case udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1:
      case udpsel_OBJ_SELECTPLAYER_BUTTON_CARD2:
      case udpsel_OBJ_SELECTPLAYER_BUTTON_CARD3:
      case udpsel_OBJ_SELECTPLAYER_BUTTON_CARD4:
      case udpsel_OBJ_SELECTPLAYER_BUTTON_CARD5:
      case udpsel_OBJ_SELECTPLAYER_BUTTON_CARD6:
      case udpsel_OBJ_SELECTPLAYER_BUTTON_CARD7:
      case udpsel_OBJ_SELECTPLAYER_BUTTON_CARD8:
        // Remember the name.
        wcsncpy( UDPSEL_PlayerInfo.name,
         &udpsel_PlayerLog[udpsel_PlayerLogPageStart+buttonIndex-udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1][0],
         udpsel_MAX_ENTERNAME_LENGTH+2 );
        // Guarantee termination.
        UDPSEL_PlayerInfo.name[udpsel_MAX_ENTERNAME_LENGTH+1] = 0;
        // Make player human.
        UDPSEL_PlayerInfo.aiLevel = 0;
        // Go directly to token select string.  We have the name now.
        UDPSEL_SwitchPhase( UDPSEL_PHASE_SELECTTOKEN );
        break;

      case udpsel_OBJ_SELECTPLAYER_BUTTON_MORE:
        // Page ahead
        udpsel_PlayerLogPageStart += 8;
        if ( udpsel_PlayerLogPageStart >= udpsel_PlayerLogMax )
          udpsel_PlayerLogPageStart = 0;
        // Redisplay text overlays.
        udpsel_SelectScreen_PrintCardText();
        udpsel_SelectScreen_ShowCardText();
        // Redisplay card backdrops (and fix up hotspots).
        for ( i = 0; i < 8; i++ )
        {
          if ( udpsel_PlayerLogPageStart+i < udpsel_PlayerLogMax )
          {
            udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].desiredID = 
             udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].idleID;
            udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].hotspotEnabled = TRUE; 
          }
          else
          {
            udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].desiredID = LED_EI;
            udpsel_Objects[udpsel_OBJ_SELECTPLAYER_BUTTON_CARD1+i].hotspotEnabled = FALSE; 
          }
        }
        break;

      case udpsel_OBJ_ENTERNAME_BUTTON_NEXT:
        udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].desiredID = LED_EI;
        udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].desiredID = LED_EI;
        UDPSEL_SwitchPhase( UDPSEL_PHASE_SELECTTOKEN );
        break;

      case udpsel_OBJ_PICKTOKEN_BUTTON_CANNON:
        udpsel_TokenScreen_OnATokenButton( TK_GUN );
        break;
      case udpsel_OBJ_PICKTOKEN_BUTTON_IRON:
        udpsel_TokenScreen_OnATokenButton( TK_IRON );
        break;
      case udpsel_OBJ_PICKTOKEN_BUTTON_THIMBLE:
        udpsel_TokenScreen_OnATokenButton( TK_THIMBLE );
        break;
      case udpsel_OBJ_PICKTOKEN_BUTTON_RACECAR:
        udpsel_TokenScreen_OnATokenButton( TK_CAR );
        break;
      case udpsel_OBJ_PICKTOKEN_BUTTON_HORSE:
        udpsel_TokenScreen_OnATokenButton( TK_HORSE );
        break;
      case udpsel_OBJ_PICKTOKEN_BUTTON_WHEELBARROW:
        udpsel_TokenScreen_OnATokenButton( TK_BARROW );
        break;
      case udpsel_OBJ_PICKTOKEN_BUTTON_DOG:
        udpsel_TokenScreen_OnATokenButton( TK_DOG );
        break;
      case udpsel_OBJ_PICKTOKEN_BUTTON_BATTLESHIP:
        udpsel_TokenScreen_OnATokenButton( TK_SHIP );
        break;
      case udpsel_OBJ_PICKTOKEN_BUTTON_SACKOFMONEY:
        udpsel_TokenScreen_OnATokenButton( TK_MONEYBAG );
        break;
      case udpsel_OBJ_PICKTOKEN_BUTTON_TOPHAT:
        udpsel_TokenScreen_OnATokenButton( TK_HAT );
        break;
      case udpsel_OBJ_PICKTOKEN_BUTTON_SHOE:
        udpsel_TokenScreen_OnATokenButton( TK_SHOE );
        break;

      case udpsel_OBJ_PICKTOKEN_BUTTON_NEXT:
        {
          int   nLength,    maxLength;

          if ( UDPSEL_PlayerInfo.aiLevel > 0 )
          {
            // For thimble allow 11 characters, since there is no happy
            // 10 character translation for thimble in French.
            maxLength = udpsel_MAX_ENTERNAME_LENGTH;
            if ( UDPSEL_PlayerInfo.token == TK_THIMBLE )
              maxLength++;
            // Select an appropriate name for the AI token.
            wcsncpy( UDPSEL_PlayerInfo.name, udpsel_TokenNames[UDPSEL_PlayerInfo.token], maxLength );
            UDPSEL_PlayerInfo.name[maxLength] = (wchar_t)NULL;
          }
          // Get rid of the hard-coded cursor, if it exists.
          nLength = wcslen(UDPSEL_PlayerInfo.name);
          if ( UDPSEL_PlayerInfo.name[nLength - 1] == '_' )
            UDPSEL_PlayerInfo.name[nLength - 1] = 0;

          // Here's where the player is officially added.
          AddLocalPlayer( UDPSEL_PlayerInfo.name, UDPSEL_PlayerInfo.token,
           udpsel_DetermineNextAvailableColour(PC_RED), UDPSEL_PlayerInfo.aiLevel, FALSE );

          UDPSEL_SwitchPhase( UDPSEL_PHASE_STARTADDREMOVE );

          // Reset player name, so its clear before we get anywhere near doing
          // the next player.
          mbstowcs( UDPSEL_PlayerInfo.name, "_", RULE_MAX_PLAYER_NAME_LENGTH );
          udpsel_NameScreen_ReShowName( FALSE );
        }
        break;

      case udpsel_OBJ_START_BUTTON_ADDHUMAN:
        UDPSEL_PlayerInfo.aiLevel = 0;
        UDPSEL_SwitchPhase( UDPSEL_PHASE_SELECTPLAYER );
        break;

      case udpsel_OBJ_START_BUTTON_ADDCOMPUTER:
        UDPSEL_SwitchPhase( UDPSEL_PHASE_SELECTAISTRENGTH );
        break;

      case udpsel_OBJ_START_BUTTON_REMOVEPLAYER:
        UDPSEL_SwitchPhase( UDPSEL_PHASE_REMOVEPLAYER );
        break;

      case udpsel_OBJ_START_BUTTON_STARTGAME:
        UDPSEL_PlayerInfo.startButtonPressed = TRUE;
        // Now the rules engine will do the "accept configuration dance".
        MESS_SendAction( ACTION_START_GAME, AnyLocalPlayer(), RULE_BANK_PLAYER,
         0, 0, 0, 0, NULL, 0, NULL );
        if ( MESS_ServerMode )
        {
          // The host must select and city/country/currency, then possibly customize the rules.
          UDPSEL_SwitchPhase( UDPSEL_PHASE_SELECTCITY );
        }
        else
        {
          // Everyone else just waits at the rules screen staring at the "host will choose rules"
          // screen.
          UDPSEL_SwitchPhase( UDPSEL_PHASE_CUSTOMIZERULES );
        }
        break;

      case udpsel_OBJ_REMOVE_BUTTON_CANCEL:
        UDPSEL_SwitchPhase( UDPSEL_PHASE_STARTADDREMOVE );
        break;

      case udpsel_OBJ_AISTRENGTH_BUTTON_EASY:
      case udpsel_OBJ_AISTRENGTH_BUTTON_MEDIUM:
      case udpsel_OBJ_AISTRENGTH_BUTTON_HARD:
        UDPSEL_PlayerInfo.aiLevel = 1 + buttonIndex - udpsel_OBJ_AISTRENGTH_BUTTON_EASY;
        UDPSEL_SwitchPhase( UDPSEL_PHASE_SELECTTOKEN );
        break;

      case udpsel_OBJ_CITY_BUTTON_CLASSICBOARD:
#if USA_VERSION
        UDPSEL_PlayerInfo.citySelected = 0;
        DISPLAY_state.city = 0;
#else
        UDPSEL_PlayerInfo.citySelected = iLangId - 2;
        DISPLAY_state.city = iLangId - 2;
        udpsel_ValidateCurrencySelections();
        DISPLAY_state.system = UDPSEL_PlayerInfo.currencySelection[UDPSEL_PlayerInfo.currencySelectionIndex];
#endif
        UDPSEL_SwitchPhase( UDPSEL_PHASE_STANDARDORCUSTOMRULES );
        break;

      case udpsel_OBJ_CITY_BUTTON_LOADBOARD:
      {
        TRACE("Loading a custom board\n");

        last_playerselectphase = UDPSEL_PHASE_NONE;
        last_playerselectscreen = DISPLAY_state.current2DView;

        // remove the player select stuff from the screen
        // display the options background screen
        UDBOARD_SetBackdrop( DISPLAY_SCREEN_Options );
        last_options_result = -1;	
        current_options_result = LOAD_BOARD_SCREEN;
        break;
      }

#if USA_VERSION
      case udpsel_OBJ_CITY_BUTTON_LEFT:
      case udpsel_OBJ_CITY_BUTTON_RIGHT:
        udpsel_Objects[buttonIndex].desiredID = udpsel_Objects[buttonIndex].animInID;
        if ( buttonIndex == udpsel_OBJ_CITY_BUTTON_LEFT )
        {
          if ( UDPSEL_PlayerInfo.citySelected > 0 )
           UDPSEL_PlayerInfo.citySelected--;
          else
           UDPSEL_PlayerInfo.citySelected = DISPLAY_CITY_MAX - 1;
        }
        else
        {
          if ( UDPSEL_PlayerInfo.citySelected < (DISPLAY_CITY_MAX - 1) )
           UDPSEL_PlayerInfo.citySelected++;
          else
           UDPSEL_PlayerInfo.citySelected = 0;
        }
        udpsel_ShowCityName( UDPSEL_PlayerInfo.citySelected );
        break;
#else
      case udpsel_OBJ_COUNTRY_BUTTON_LEFT:
      case udpsel_OBJ_COUNTRY_BUTTON_RIGHT:
        udpsel_Objects[buttonIndex].desiredID = udpsel_Objects[buttonIndex].animInID;
        if ( buttonIndex == udpsel_OBJ_COUNTRY_BUTTON_LEFT )
        {
          if ( UDPSEL_PlayerInfo.citySelected > 0 )
           UDPSEL_PlayerInfo.citySelected--;
          else
           UDPSEL_PlayerInfo.citySelected = DISPLAY_CITY_MAX - 1;
        }
        else
        {
          if ( UDPSEL_PlayerInfo.citySelected < (DISPLAY_CITY_MAX - 1) )
           UDPSEL_PlayerInfo.citySelected++;
          else
           UDPSEL_PlayerInfo.citySelected = 0;
        }
        udpsel_ShowCountryName( UDPSEL_PlayerInfo.citySelected );
        udpsel_ValidateCurrencySelections();
        udpsel_ShowCurrencyText( UDPSEL_PlayerInfo.currencySelection[UDPSEL_PlayerInfo.currencySelectionIndex] );
        break;

      case udpsel_OBJ_CURRENCY_BUTTON_LEFT:
      case udpsel_OBJ_CURRENCY_BUTTON_RIGHT:
        {
          int   previous;

          udpsel_Objects[buttonIndex].desiredID = udpsel_Objects[buttonIndex].animInID;

          // If the country chosen and the language installed are the same, two of our
          // array values are the same, if this happens skip the duplicate and always
          // present our poor user with new choices for every mouse click.
          do
          {
            previous = UDPSEL_PlayerInfo.currencySelection[UDPSEL_PlayerInfo.currencySelectionIndex];
            if ( buttonIndex == udpsel_OBJ_CURRENCY_BUTTON_LEFT )
              UDPSEL_PlayerInfo.currencySelectionIndex--;
            else
              UDPSEL_PlayerInfo.currencySelectionIndex++;
            if ( UDPSEL_PlayerInfo.currencySelectionIndex < 0 )
              UDPSEL_PlayerInfo.currencySelectionIndex = 2;
            if ( UDPSEL_PlayerInfo.currencySelectionIndex > 2 )
              UDPSEL_PlayerInfo.currencySelectionIndex = 0;
          }
          while ( previous == UDPSEL_PlayerInfo.currencySelection[UDPSEL_PlayerInfo.currencySelectionIndex] );
          udpsel_ShowCurrencyText( UDPSEL_PlayerInfo.currencySelection[UDPSEL_PlayerInfo.currencySelectionIndex] );
        }
        break;
#endif

      case udpsel_OBJ_CITY_BUTTON_NEXT:
        DISPLAY_state.city = UDPSEL_PlayerInfo.citySelected;
#if !USA_VERSION
        DISPLAY_state.system = UDPSEL_PlayerInfo.currencySelection[UDPSEL_PlayerInfo.currencySelectionIndex];
#endif
        UDPSEL_SwitchPhase( UDPSEL_PHASE_STANDARDORCUSTOMRULES );
        break;

      case udpsel_OBJ_RULESCHOICE_BUTTON_CUSTOM:
        // Should have only reached the "Standard or Custom?" screen in the first place,
        // if we are in server mode and therefore the host.
        UDPSEL_SwitchPhase( UDPSEL_PHASE_CUSTOMIZERULES );
        break;

      case udpsel_OBJ_RULESCHOICE_BUTTON_STANDARD:
        // Should have only reached the "Standard or Custom?" screen in the first place,
        // if we are in server mode and therefore the host.
        udpsel_SetStandardRules( &UICurrentGameState.GameOptions );

      /* PLEASE NOTE: deliberate fall thru from above case to here. */

      case udpsel_OBJ_RULES_BUTTON_OKAY:
        // BLEEBLE: this is kind of klugee.  This code is cut/paste from Rule.cpp
        // It should decide on a slot index for the host, and it should come up
        // with the same one that the rules engine did.
        for ( i = 0; i < UICurrentGameState.NumberOfPlayers; i++ )
        {
          if ( MESS_ServerMode && SlotIsALocalPlayer[i] && !SlotIsALocalAIPlayer[i] )
          {
            RULE_ActionArgumentsRecord msg;

            ZeroMemory( &msg, sizeof(RULE_ActionArgumentsRecord) );
            msg.action      = ACTION_ACCEPT_CONFIGURATION;
            msg.fromPlayer  = i;
            msg.toPlayer    = RULE_BANK_PLAYER;
            msg.numberC     = 1;

            if ( AddGameOptionsToMessageBlob(&UICurrentGameState.GameOptions,&msg) )
              MESS_SendActionMessage(&msg);

            break; // for loop
          }
        }
        break;











      case udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD:
        udpsel_SetStandardRules( &UICurrentGameState.GameOptions );
        break;
      case udpsel_OBJ_RULES_BUTTON_SHORTGAME:
        udpsel_SetShortGameRules( &UICurrentGameState.GameOptions );
        break;
      case udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_4:
        UICurrentGameState.GameOptions.housesPerHotel = 4;
        break;
      case udpsel_OBJ_RULES_BUTTON_HOUSESPERPROPERTY_5:
        UICurrentGameState.GameOptions.housesPerHotel = 5;
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_12:
        UICurrentGameState.GameOptions.maximumHouses = 12;
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_32:
        UICurrentGameState.GameOptions.maximumHouses = 32;
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_60:
        UICurrentGameState.GameOptions.maximumHouses = 60;
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALHOUSES_88:
        UICurrentGameState.GameOptions.maximumHouses = 88;
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_4:
        UICurrentGameState.GameOptions.maximumHotels = 4;
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_12:
        UICurrentGameState.GameOptions.maximumHotels = 12;
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_16:
        UICurrentGameState.GameOptions.maximumHotels = 16;
        break;
      case udpsel_OBJ_RULES_BUTTON_TOTALPROPERTIES_22:
        UICurrentGameState.GameOptions.maximumHotels = 22;
        break;
      case udpsel_OBJ_RULES_BUTTON_FREEPARKING_0:
        UICurrentGameState.GameOptions.freeParkingSeed = 0;
        break;
      case udpsel_OBJ_RULES_BUTTON_FREEPARKING_250:
        UICurrentGameState.GameOptions.freeParkingSeed = 250;
        break;
      case udpsel_OBJ_RULES_BUTTON_FREEPARKING_500:
        UICurrentGameState.GameOptions.freeParkingSeed = 500;
        break;
      case udpsel_OBJ_RULES_BUTTON_FREEPARKING_750:
        UICurrentGameState.GameOptions.freeParkingSeed = 750;
        break;
      case udpsel_OBJ_RULES_BUTTON_INITIALCASH_500:
        UICurrentGameState.GameOptions.initialCash = 500;
        break;
      case udpsel_OBJ_RULES_BUTTON_INITIALCASH_1000:
        UICurrentGameState.GameOptions.initialCash = 1000;
        break;
      case udpsel_OBJ_RULES_BUTTON_INITIALCASH_1500:
        UICurrentGameState.GameOptions.initialCash = 1500;
        break;
      case udpsel_OBJ_RULES_BUTTON_INITIALCASH_2000:
        UICurrentGameState.GameOptions.initialCash = 2000;
        break;
      case udpsel_OBJ_RULES_BUTTON_SALARY_0:
        UICurrentGameState.GameOptions.passingGoAmount = 0;
        break;
      case udpsel_OBJ_RULES_BUTTON_SALARY_100:
        UICurrentGameState.GameOptions.passingGoAmount = 100;
        break;
      case udpsel_OBJ_RULES_BUTTON_SALARY_200:
        UICurrentGameState.GameOptions.passingGoAmount = 200;
        break;
      case udpsel_OBJ_RULES_BUTTON_SALARY_400:
        UICurrentGameState.GameOptions.passingGoAmount = 400;
        break;
      case udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_0:
        UICurrentGameState.GameOptions.taxRate = 0;
        break;
      case udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_5:
        UICurrentGameState.GameOptions.taxRate = 5;
        break;
      case udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_10:
        UICurrentGameState.GameOptions.taxRate = 10;
        break;
      case udpsel_OBJ_RULES_BUTTON_TAXRATEPERCENTAGE_15:
        UICurrentGameState.GameOptions.taxRate = 15;
        break;
      case udpsel_OBJ_RULES_BUTTON_FLATTAX_0:
        UICurrentGameState.GameOptions.flatTaxFee = 0;
        break;
      case udpsel_OBJ_RULES_BUTTON_FLATTAX_100:
        UICurrentGameState.GameOptions.flatTaxFee = 100;
        break;
      case udpsel_OBJ_RULES_BUTTON_FLATTAX_200:
        UICurrentGameState.GameOptions.flatTaxFee = 200;
        break;
      case udpsel_OBJ_RULES_BUTTON_FLATTAX_400:
        UICurrentGameState.GameOptions.flatTaxFee = 400;
        break;
      case udpsel_OBJ_RULES_BUTTON_LUXURYTAX_0:
        UICurrentGameState.GameOptions.luxuryTaxAmount = 0;
        break;
      case udpsel_OBJ_RULES_BUTTON_LUXURYTAX_75:
        UICurrentGameState.GameOptions.luxuryTaxAmount = 75;
        break;
      case udpsel_OBJ_RULES_BUTTON_LUXURYTAX_150:
        UICurrentGameState.GameOptions.luxuryTaxAmount = 150;
        break;
      case udpsel_OBJ_RULES_BUTTON_LUXURYTAX_300:
        UICurrentGameState.GameOptions.luxuryTaxAmount = 300;
        break;
      case udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_1:
        UICurrentGameState.GameOptions.maximumTurnsInJail = 1;
        break;
      case udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_2:
        UICurrentGameState.GameOptions.maximumTurnsInJail = 2;
        break;
      case udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_3:
        UICurrentGameState.GameOptions.maximumTurnsInJail = 3;
        break;
      case udpsel_OBJ_RULES_BUTTON_TURNSINJAIL_4:
        UICurrentGameState.GameOptions.maximumTurnsInJail = 4;
        break;
      case udpsel_OBJ_RULES_BUTTON_JAILFEE_0:
        UICurrentGameState.GameOptions.getOutOfJailFee = 0;
        break;
      case udpsel_OBJ_RULES_BUTTON_JAILFEE_50:
        UICurrentGameState.GameOptions.getOutOfJailFee = 50;
        break;
      case udpsel_OBJ_RULES_BUTTON_JAILFEE_100:
        UICurrentGameState.GameOptions.getOutOfJailFee = 100;
        break;
      case udpsel_OBJ_RULES_BUTTON_JAILFEE_200:
        UICurrentGameState.GameOptions.getOutOfJailFee = 200;
        break;
      case udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_0:
        UICurrentGameState.GameOptions.houseShortageLevel = 0;
        break;
      case udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_1:
        UICurrentGameState.GameOptions.houseShortageLevel = 1;
        break;
      case udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_6:
        UICurrentGameState.GameOptions.houseShortageLevel = 6;
        break;
      case udpsel_OBJ_RULES_BUTTON_HOUSESHORTAGE_12:
        UICurrentGameState.GameOptions.houseShortageLevel = 12;
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_0:
        UICurrentGameState.GameOptions.hotelShortageLevel = 0;
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_1:
        UICurrentGameState.GameOptions.hotelShortageLevel = 1;
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_3:
        UICurrentGameState.GameOptions.hotelShortageLevel = 3;
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTYSHORTAGE_6:
        UICurrentGameState.GameOptions.hotelShortageLevel = 6;
        break;
      case udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_0:
        UICurrentGameState.GameOptions.interestRate = 0;
        break;
      case udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_5:
        UICurrentGameState.GameOptions.interestRate = 5;
        break;
      case udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_10:
        UICurrentGameState.GameOptions.interestRate = 10;
        break;
      case udpsel_OBJ_RULES_BUTTON_MORTGAGERATE_20:
        UICurrentGameState.GameOptions.interestRate = 20;
        break;
      case udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_3:
        UICurrentGameState.GameOptions.auctionGoingTimeDelay = 3;
        break;
      case udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_4:
        UICurrentGameState.GameOptions.auctionGoingTimeDelay = 4;
        break;
      case udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_5:
        UICurrentGameState.GameOptions.auctionGoingTimeDelay = 5;
        break;
      case udpsel_OBJ_RULES_BUTTON_AUCTIONDELAY_10:
        UICurrentGameState.GameOptions.auctionGoingTimeDelay = 10;
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_0:
        UICurrentGameState.GameOptions.dealNPropertiesAtStartup = 0;
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_2:
        UICurrentGameState.GameOptions.dealNPropertiesAtStartup = 2;
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_4:
        UICurrentGameState.GameOptions.dealNPropertiesAtStartup = 4;
        break;
      case udpsel_OBJ_RULES_BUTTON_PROPERTIESDEALTATSTART_ALL:
        UICurrentGameState.GameOptions.dealNPropertiesAtStartup = SQ_TOTAL_PROPERTY_SQUARES;
        break;
      case udpsel_OBJ_RULES_BUTTON_EVENBUILDRULE:
        UICurrentGameState.GameOptions.evenBuildRule = !UICurrentGameState.GameOptions.evenBuildRule;
        break;
      case udpsel_OBJ_RULES_BUTTON_EXTRAGOMONEY:
        UICurrentGameState.GameOptions.doubleSalaryOnGo = !UICurrentGameState.GameOptions.doubleSalaryOnGo;
        break;
      case udpsel_OBJ_RULES_BUTTON_FREEPARKINGRULE:
        UICurrentGameState.GameOptions.freeParkingPot = !UICurrentGameState.GameOptions.freeParkingPot;
        break;
      case udpsel_OBJ_RULES_BUTTON_FUTURESANDIMMUNITIES:
        UICurrentGameState.GameOptions.futureRentTradingAllowed = !UICurrentGameState.GameOptions.futureRentTradingAllowed;
        UICurrentGameState.GameOptions.immunitiesTradingAllowed = !UICurrentGameState.GameOptions.immunitiesTradingAllowed;
        break;
      case udpsel_OBJ_RULES_BUTTON_DEALTPROPERTIESAREFREE:
        UICurrentGameState.GameOptions.dealFreePropertiesAtStartup = !UICurrentGameState.GameOptions.dealFreePropertiesAtStartup;
        break;
      }
      if ( (buttonIndex >= udpsel_OBJ_RULES_BUTTON_RESTORESTANDARD)
       && (buttonIndex <= udpsel_OBJLAST_RULES) )
      {
        RULE_ActionArgumentsRecord msg;

        // Update this screen.
        udpsel_UpdateRulesScreenButtons( &(UICurrentGameState.GameOptions), &udpsel_PreviousGameOptions, FALSE );

        // Send message so other player's screens are updated.
        ZeroMemory( &msg, sizeof(RULE_ActionArgumentsRecord) );
        msg.action      = ACTION_ACCEPT_CONFIGURATION;
        msg.fromPlayer  = CurrentUIPlayer;
        msg.toPlayer    = RULE_BANK_PLAYER;
        msg.numberC     = 1;
        msg.numberD     = 1;  // Set the flag for interim change.  Okay button is for final.

        if ( AddGameOptionsToMessageBlob(&UICurrentGameState.GameOptions,&msg) )
          MESS_SendActionMessage(&msg);
      }
    }
    // Now check keyboard stuff on the enter name screen.
    if ( DISPLAY_state.currentPlayerSetupPhase == UDPSEL_PHASE_ENTERNAME )
    {
      if ( UIMSG_KEYBOARD_ACHAR == pUIMessage->messageCode )
      {
        if ( udpsel_NameScreen_AddLetterToNameField((short)pUIMessage->numberA) )
        {
          udpsel_NameScreen_ReShowName( TRUE );
          // If this is the first character in the name bring up the NEXT button,
          // since it is now valid.  (Don't forget that our cursor is hard-coded
          // into the name).
          if ( wcslen(UDPSEL_PlayerInfo.name) == 2 )
          {
            udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].desiredID = 
             udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].idleID;
            udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].hotspotEnabled = TRUE;
          }
        }
      }
      else if ( UIMSG_KEYBOARD_PRESSED == pUIMessage->messageCode )
      {
        if ( LE_KEYBRD_BACK == pUIMessage->numberA )
        {
          // Backspace removes the last character of the player's name.
          if ( udpsel_NameScreen_RemoveLetterFromNameField() )
          {
            udpsel_NameScreen_ReShowName( TRUE );
            // If there are no more characters in the name, get rid of the NEXT button
            // since it is now no longer valid.  (Don't forget that our cursor is hard-coded
            // into the name).
            if ( wcslen(UDPSEL_PlayerInfo.name) < 2 )
            {
              udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].desiredID = LED_EI;
              udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].hotspotEnabled = FALSE;
            }
          }
        }
        else if ( LE_KEYBRD_RET == pUIMessage->numberA )
        {
          // Enter key accepts the name and changes screens.
          // But only if the name is non-null.
          if ( wcslen(UDPSEL_PlayerInfo.name) > 1 )
          {
            udpsel_Objects[udpsel_OBJ_ENTERNAME_TEXTOVERLAY].desiredID = LED_EI;
            udpsel_Objects[udpsel_OBJ_ENTERNAME_BUTTON_NEXT].desiredID = LED_EI;
            UDPSEL_SwitchPhase( UDPSEL_PHASE_SELECTTOKEN );
          }
        }
      }
    }  
  }
}


/*****************************************************************************
 * Standard UI subfunction - Process game message to player.
 */

void UDPSEL_ProcessMessageToPlayer( RULE_ActionArgumentsPointer NewMessage )
{
  int               i;

  switch (NewMessage->action)
  {
    case NOTIFY_NAME_PLAYER:
    case NOTIFY_ADD_LOCAL_PLAYER:
    case NOTIFY_PLAYER_DELETED:
      /* Note that UICurrentGameState is updated in CheckForAcceptingOurNewPlayer,
         in case we have to change the AI level of an existing player (need to
         know before and after values). */
      CheckForAcceptingOurNewPlayer (NewMessage);
      // Player status may have changed, do a forced update.
      udpsel_ForcedRefresh = TRUE;

      DISPLAY_state.IBarStateTrackOn = TRUE;

      break;

    case NOTIFY_ACTION_COMPLETED:
      // If the host has pressed the start button during setup, then
      // everybody should go to the custom rules screen where it will
      // say "host is establishing rules" and you can't do anything but
      // watch -- until the host sets the rules and starts the game.
      if ( (NewMessage->numberA == ACTION_START_GAME) && NewMessage->numberB )
      {
        if ( !MESS_ServerMode )
          UDPSEL_SwitchPhase( UDPSEL_PHASE_CUSTOMIZERULES );
      }
      break;

    case NOTIFY_NUMBER_OF_PLAYERS:
      DISPLAY_state.FlashCurrentToken = FALSE;  // Probably not someone's turn right now.
      // Ignore if we already know.
      if ( !g_bNewGameRequestedFlagForUDPSELCode )
      {
        if ( udpsel_NumberOfPlayers == NewMessage->numberA )
          break;
      }
      udpsel_NumberOfPlayers = NewMessage->numberA;
      g_bNewGameRequestedFlagForUDPSELCode = FALSE;   // Reset this flag

      UICurrentGameState.NumberOfPlayers = (unsigned char) NewMessage->numberA;
      if ( UICurrentGameState.NumberOfPlayers == 0 || !udpsel_FirstTimeInitializationDone )
      {
        udpsel_FirstTimeInitializationDone = TRUE;

        if ( UICurrentGameState.NumberOfPlayers == 0 )
        {
          /* Starting a new game.  Wipe out almost everything. */
          memset (&UICurrentGameState, 0, sizeof (UICurrentGameState));
        }

        // We don't want the board to appear with hotels everywhere so we
        // need to set an amount of houses per hotel
        UICurrentGameState.GameOptions.housesPerHotel = 5;

        for (i = 0; i < SQ_MAX_SQUARE_TYPES; i++)
        {
          UICurrentGameState.Squares[i].owner  = RULE_NOBODY_PLAYER;
          UICurrentGameState.Squares[i].houses = 0;
        }

        NumberOfLocalPlayers = 0;
        GameInProgress = FALSE;
        for (i = 0; i < RULE_MAX_PLAYERS; i++)
        {
          SlotIsALocalPlayer [i] = FALSE;
          SlotIsALocalAIPlayer [i] = FALSE;
          SlotIsALocalHumanPlayer [i] = FALSE;
          LocalPlayerSlots [i] = RULE_NOBODY_PLAYER;
          UICurrentGameState.Players[i].currentSquare = SQ_OFF_BOARD;
        }

        AI_InitializeSystem (); /* Reset AI arrays - Andrew */

        // Display system - make sure these 'object up' indicators are cleared (drop objects)
        DISPLAY_state.ChanceCommChestDesired = -1;
        DISPLAY_state.PropertyBuyAuctionDesired = LED_EI;
        DISPLAY_state.IBarPropertyCurrentMouseOver = -1;
        UDIBAR_setIBarRulesState( IBAR_STATE_Nothing, 0 ); // Until engine processing takes over.
        DISPLAY_state.IBarStateTrackOn = TRUE;

#if !USA_VERSION
        // Default the currency to the install language, so that as new players
        // are added they will appear on the bar with 0 dollars but in the right
        // (or reasonable) currency.
        udpsel_ValidateCurrencySelections();
        DISPLAY_state.system = UDPSEL_PlayerInfo.currencySelection[1];
#endif

        UDPIECES_StartOfGameSetup();

        // reset the globals for the status screen 
        UDIBar_ResetStatusBarButtons();
        

        if ( UICurrentGameState.NumberOfPlayers == 0 )
        {
          // numberB == 1 signifies this message was generated by a load game,
          // so we ignore those.  numberB == 0 should be either the start,
          // a new game, or removing the only player.
          if ( NewMessage->numberB == 0 )
          {
            if ( (DISPLAY_state.desired2DView == DISPLAY_SCREEN_Pselect)
             || (DISPLAY_state.desired2DView == DISPLAY_SCREEN_PselectRules) )
            {
              if ( !MESS_GameStartedByLobby )
              {
                // Detect whether the user has removed the only player, if so,
                // force them back to the "play a game", "play on-line game" screen
                // to start over (bypass hi-score screen).
                if ( (DISPLAY_state.currentPlayerSetupPhase != UDPSEL_PHASE_NONE)
                 && (DISPLAY_state.currentPlayerSetupPhase != UDPSEL_PHASE_HISCORE)
                 && (DISPLAY_state.currentPlayerSetupPhase != UDPSEL_PHASE_LOCALORNETWORK) )
                {
                  UDBOARD_SetBackdrop( DISPLAY_SCREEN_Pselect );
                  UDPSEL_SwitchPhase( UDPSEL_PHASE_LOCALORNETWORK );
                }
              }
            }
            else
            {
              // Restart setup (for a new game) if we're not already there..
              UDBOARD_SetBackdrop( DISPLAY_SCREEN_Pselect );
              UDPSEL_SwitchPhase( UDPSEL_PHASE_HISCORE );
            }
          }
#if !USA_VERSION
          else
          {
              // set the city and system flags for a saved game
              DISPLAY_state.city = SaveGameStuff[CurrentGameFile].city;
              DISPLAY_state.system = SaveGameStuff[CurrentGameFile].system;
          }
#endif
        }
      }
      else
      {
        udpsel_ForcedRefresh = TRUE;

        // set the game earnings for all the squares
        for(i=0; i<SQ_MAX_SQUARE_TYPES; i++)
        {
          UICurrentGameState.Squares[i].game_earnings = SaveGameStuff[CurrentGameFile].square_game_earnings[i];
        }
      }
      break;


    case NOTIFY_PROPOSED_CONFIGURATION:
      // Unless we are at the rules screen, we know that we are
      // still adding players and stuff like that so don't bother.
      CopyNewGameOptions(NewMessage);
      if ( UDPSEL_PlayerInfo.startButtonPressed )
      {
        if ( NewMessage->numberB != 0 )
        {
          /* Display the dialog box if still in accepting phase, not just a resync
             configuration update during a network game. */

          SetCurrentUIPlayerFromPlayerSet ((RULE_PlayerSet) NewMessage->numberB);

          // for now, display an icon to determin who is accepting
          if ( CurrentUIPlayer < RULE_MAX_PLAYERS )
          {
            UDPSEL_PlayerInfo.token = UICurrentGameState.Players[CurrentUIPlayer].token;
          }
        }
      }
      if ( !MESS_ServerMode && (DISPLAY_state.desired2DView == DISPLAY_SCREEN_PselectRules) )
        udpsel_UpdateRulesScreenButtons( &UICurrentGameState.GameOptions, &udpsel_PreviousGameOptions, FALSE );
      break;

    case NOTIFY_DICE_ROLLED:
    case NOTIFY_PLEASE_ROLL_DICE:
      break;

    default:  // this should never happen.
      break;
  }

  // NOTIFY_GAME_STARTING: moves out of Player Selection.
}

