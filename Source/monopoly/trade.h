#ifndef __TRADE_H__
#define __TRADE_H__
#include <stdlib.h>
/*************************************************************
*
*   FILE:             TRADE.H
*
*   (C) Copyright 99 Artech Digital Entertainments.
*                    All rights reserved.
*
*   Monopoly Game Trade Edit/Process Functions.
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/

// These items store trade editor interface items - the itemlist 
#define TRADE_MaxMessages 28 + RULE_MAX_PLAYERS_CHOOSE_2 + RULE_MAX_COUNT_HIT_SETS + 2 //Properties + players(cash)! + ImmunitiesMax + jail cards - count hit sets is large, probably more than needed.
extern int     TradeItemListTop, TradeFromPlayer, TradeToPlayer, TradeDisplayInEditModeNotView;
extern RULE_ActionArgumentsRecord  UDTradeItemList[TRADE_MaxMessages];


/************************************************************/
/* PROTOTYPES                                               */
/************************************************************/

#if defined( __cplusplus )
	extern "C" {
#endif

void Trade_AddItem( RULE_ActionArgumentsPointer NewMessage );
void Trade_SendItems( void );
void AddUiImmunity (RULE_PlayerNumber FromPlayer, RULE_PlayerNumber ToPlayer,
  CountHitTypes HitType, int HitCount, RULE_PropertySet SquareSet, BOOL TradingItem);

#if defined( __cplusplus )
		}
#endif
#endif /* __TRADE_H__ */
