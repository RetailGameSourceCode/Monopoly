#ifndef __UDPSEL_H__
#define __UDPSEL_H__

/*************************************************************
*
*   FILE:             UDPSEL.H
*
*   (C) Copyright 1999 Artech Digital Entertainments.
*                      All rights reserved.
*
*   Monopoly Game User Interface/Display Subfile Header.
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/


/************************************************************/
/* EXPORTED DATA STRUCTURES AND TYPES                       */
/************************************************************/

enum udpsel_SetupPhase
{
  UDPSEL_PHASE_NONE,                // Not doing setup.
  UDPSEL_PHASE_HISCORE,             // Intro screen, only appears at beginning of game.
  UDPSEL_PHASE_LOCALORNETWORK,
  UDPSEL_PHASE_SELECTPLAYER,        // Select from a list of previous players, or select "new player"
  UDPSEL_PHASE_ENTERNAME,
  UDPSEL_PHASE_SELECTTOKEN,
  UDPSEL_PHASE_STARTADDREMOVE,
  UDPSEL_PHASE_REMOVEPLAYER,
  UDPSEL_PHASE_SELECTAISTRENGTH,
  UDPSEL_PHASE_SELECTCITY,       // For USA: host selects a city, For Euro: host selects country/currency
  UDPSEL_PHASE_STANDARDORCUSTOMRULES,   // Does host want custom rules?
  UDPSEL_PHASE_CUSTOMIZERULES,      // A screenful of rules options for the host to fiddle with.
  UDPSEL_PHASE_MAX,
};


typedef struct tag_UDPSEL_PlayerInfoStruct
{
  wchar_t         name[RULE_MAX_PLAYER_NAME_LENGTH];
  RULE_TokenKind  token;
  int             aiLevel;
  int             citySelected;
#if !USA_VERSION
  int             currencySelectionIndex;
  int             currencySelection[3];
                      // 0 -> the country selected, 1 -> the language installed, 2 -> Euros
#endif
  BOOL            customRulesDesired;
  BOOL            startButtonPressed;

} UDPSEL_PlayerInfoStruct;

/************************************************************/
/* EXTERNAL GLOBALS										                      */
/************************************************************/
extern UDPSEL_PlayerInfoStruct UDPSEL_PlayerInfo;


/************************************************************/
/* EXPORTED FUNCTIONS                                       */
/************************************************************/
void DISPLAY_UDPSEL_Initialize( void );
void DISPLAY_UDPSEL_Destroy( void );
void DISPLAY_UDPSEL_Show( void );

void UDPSEL_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr );
void UDPSEL_ProcessMessageToPlayer( RULE_ActionArgumentsPointer NewMessage );

void UDPSEL_SwitchViewAfterAnimOuts( int desiredView );
void UDPSEL_PlayerButtonClicked( int playerNumber );
void UDPSEL_GameHasJustStarted( void );
void UDPSEL_GameOver( wchar_t *name, int winnerNetWorth, int aiLevel );
void UDPSEL_SwitchPhase( int phase );


#endif // __UDPSEL_H__
