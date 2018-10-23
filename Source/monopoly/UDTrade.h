#ifndef __UDTRADE_H__
#define __UDTRADE_H__

/*************************************************************
*
*   FILE:             UDTRADE.H
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

extern int TradeAPlayerActive, TradeBPlayerActive, TradeBPlayersAvailable[RULE_MAX_PLAYERS];
extern RULE_PropertySet  TradeAStuff, TradeAStuffMort, TradeBStuff, TradeBStuffMort;



/************************************************************/
/* EXPORTED FUNCTIONS                                       */
/************************************************************/
void DISPLAY_UDTRADE_Initialize( void );
void DISPLAY_UDTRADE_Destroy( void );
void DISPLAY_UDTRADE_Show( void );

void UDTRADE_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr );
void UDTRADE_ProcessMessageToPlayer( RULE_ActionArgumentsPointer NewMessage );

void UDTrade_ClearTrade( void );
int UDTrade_GetPlayerToTradeFrom(void);


extern void UDTrade_DisplayPlayerSelectDlg(void);
#endif // __UDTRADE_H__
