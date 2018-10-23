/*************************************************************
*
*   FILE:             TRADE.CPP
*
*   (C) Copyright 99  Artech Digital Entertainments.
*                     All rights reserved.
*************************************************************/


/************************************************************/
/* INCLUDE FILES                                            */
/************************************************************/
#include "gameinc.h"

/************************************************************/
/* VARIABLES                                                */
/************************************************************/
int TradeItemListTop = 0, TradeFromPlayer = -1, TradeToPlayer = -1, TradeDisplayInEditModeNotView = TRUE;
RULE_ActionArgumentsRecord  UDTradeItemList[TRADE_MaxMessages];

/************************************************************/
/* FUNCTIONS                                                */
/************************************************************/

/*****************************************************************************
 * Add Item to the user interface trade list.
 */
void Trade_AddItem( RULE_ActionArgumentsPointer NewMessage )
{
  int t, s, j;
  RULE_ActionArgumentsRecord  TempMessage;

  s = -1; // Indicates a new item, as opposed to an index to overwrite an existing item

  // Disqualifications - invalid items
  if( NewMessage->numberC < 0 || NewMessage->numberC >= TIK_MAX )
    return; // Not a valid trade item.

  //Check if this is an update rather than new entry.
  for( t = 0; t < TradeItemListTop; t++ )
  {// NOTE:  This is barely started.  Add additional checks as they come up filling in the interface (RK).
    // Also note this file is shared with the Hotseat - don't use interface variables here FIXME.
    switch( UDTradeItemList[t].numberC )
    {
    case TIK_CASH:
      if (NewMessage->numberC == TIK_CASH)
        s = t;
      break;
    case TIK_SQUARE:
      if (NewMessage->numberC == TIK_SQUARE)
        if( UDTradeItemList[t].numberD == NewMessage->numberD )
          if (UDTradeItemList[t].numberB == NewMessage->numberB)
            s = t; // The property can only have one trade entry. 
      break;
    case TIK_JAIL_CARD:
      //if( NewMessage.numberA == NewMessage.numberB )
        // FIXME Clear it - delete.
      if (NewMessage->numberC == TIK_JAIL_CARD)
        if( UDTradeItemList[t].numberD == NewMessage->numberD )
          if (UDTradeItemList[t].numberB == NewMessage->numberB)
            s = t; // Same card
      break;
    case TIK_IMMUNITY:
      if (NewMessage->numberC == TIK_IMMUNITY)
        if ((UDTradeItemList[t].numberB == NewMessage->numberB) &&
            (UDTradeItemList[t].numberE == NewMessage->numberE))
          s = t;
      break;
    case TIK_FUTURE_RENT:
      if (NewMessage->numberC == TIK_FUTURE_RENT)
        if ((UDTradeItemList[t].numberB == NewMessage->numberB) &&
            (UDTradeItemList[t].numberE == NewMessage->numberE))
          s = t;
      break;
    }
  }

  /* Yuck.  If trading, don't want to allow the total to be more than 99. */
  if ((NewMessage->numberC == TIK_FUTURE_RENT) ||
      (NewMessage->numberC == TIK_IMMUNITY)) {
    for (j = 0; j < RULE_MAX_COUNT_HIT_SETS; j++)
    {
      if (UICurrentGameState.CountHits[j].toPlayer == (RULE_PlayerNumber)NewMessage->numberB &&
//      UICurrentGameState.CountHits[j].fromPlayer == FromPlayer &&
      UICurrentGameState.CountHits[j].properties == (RULE_PropertySet)NewMessage->numberE &&
      (UICurrentGameState.CountHits[j].hitType + TIK_IMMUNITY) == (unsigned int)NewMessage->numberC &&
      UICurrentGameState.CountHits[j].tradedItem == FALSE)
        break;
    }
    if (j < RULE_MAX_COUNT_HIT_SETS)
      if ((NewMessage->numberD + UICurrentGameState.CountHits[j].hitCount) > 99)
        NewMessage->numberD = 99 - UICurrentGameState.CountHits[j].hitCount;
  }

  // Update the item stack
  // Note - make a copy of the message, don't hack up the original one since
  // it gets passed on to the AI system for furthur examination.

  TempMessage = *NewMessage;
  TempMessage.binaryDataHandleA = NULL; // Zero blob data to avoid deallocation errors.
  TempMessage.binaryDataStartOffsetA = 0;
  TempMessage.binaryDataSizeA = 0;
  TempMessage.binaryDataA = NULL;
  TempMessage.action = ACTION_TRADE_ITEM;   // Note - it always is on sending, when recieving this would be NOTIFY_, screws up counter offers.
  TempMessage.fromPlayer = TradeFromPlayer; // Again, in a counter offer, this would be the former player.
  TempMessage.toPlayer = RULE_BANK_PLAYER;

  if( s == -1 ) { // New valid item, add to top of stack.
    if( TradeItemListTop < TRADE_MaxMessages )
    { // There should always be room for more unless our define is too small somehow.
//      if (!((UDTradeItemList[TradeItemListTop-1].numberD == 0) &&
//            ((UDTradeItemList[TradeItemListTop-1].numberC == TIK_IMMUNITY) ||
//             (UDTradeItemList[TradeItemListTop-1].numberC == TIK_FUTURE_RENT))))
      if (!((TempMessage.numberD == 0) &&
            ((TempMessage.numberC == TIK_IMMUNITY) ||
             (TempMessage.numberC == TIK_FUTURE_RENT))))
        memcpy( &UDTradeItemList[TradeItemListTop++], &TempMessage, sizeof(RULE_ActionArgumentsRecord) );
    }
  } else { // Overwrite existing entry
//    if ((UDTradeItemList[s].numberD == 0) &&
//        ((UDTradeItemList[s].numberC == TIK_IMMUNITY) ||
//         (UDTradeItemList[s].numberC == TIK_FUTURE_RENT)))
    if ((TempMessage.numberD == 0) &&
        ((TempMessage.numberC == TIK_IMMUNITY) ||
         (TempMessage.numberC == TIK_FUTURE_RENT)))
    {
      memcpy( &UDTradeItemList[s], &UDTradeItemList[TradeItemListTop-1], sizeof(RULE_ActionArgumentsRecord) );
      TradeItemListTop--;
    } else
      memcpy( &UDTradeItemList[s], &TempMessage, sizeof(RULE_ActionArgumentsRecord) );
  }
}

/*****************************************************************************
 * Add Item to the user interface trade list.
 */
void Trade_SendItems( void )
{
  int t;

  for( t = 0; t < TradeItemListTop; t++ )
  {
    //memcpy( &UDTradeItemList[TradeItemListTop++], NewMessage, sizeof(RULE_ActionArgumentsRecord) );
    //MESS_SendActionMessage( &UDTradeItemList[t] );
    MESS_SendAction2( UDTradeItemList[t].action, UDTradeItemList[t].fromPlayer, UDTradeItemList[t].toPlayer,
      UDTradeItemList[t].numberA, UDTradeItemList[t].numberB, UDTradeItemList[t].numberC, UDTradeItemList[t].numberD, UDTradeItemList[t].numberE,
      UDTradeItemList[t].stringA, 0, NULL );
  }
}

/*****************************************************************************
 * Add an immunity of some sort
 */
void AddUiImmunity (RULE_PlayerNumber FromPlayer, RULE_PlayerNumber ToPlayer,
                    CountHitTypes HitType, int HitCount, RULE_PropertySet SquareSet, BOOL TradingItem)
{
  int i, j, freespot = -1;

  if (HitCount > 99)  HitCount = 99;
  if ((HitCount < 0) && (!TradingItem))  HitCount = 0;
  /* Look for the same immunity in the collection. */
  for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
  {
    if ((freespot == -1) &&
        (UICurrentGameState.CountHits[i].toPlayer == RULE_NOBODY_PLAYER))
      freespot = i;
    if (UICurrentGameState.CountHits[i].toPlayer == ToPlayer &&
//    UICurrentGameState.CountHits[i].fromPlayer == FromPlayer &&
    UICurrentGameState.CountHits[i].properties == SquareSet &&
    UICurrentGameState.CountHits[i].hitType == (unsigned int) HitType &&
    UICurrentGameState.CountHits[i].tradedItem == (unsigned int) TradingItem)
      break;
  }

  if (i >= RULE_MAX_COUNT_HIT_SETS)
  {
    /* Not an existing immunity/future. */
    if (freespot == -1) /* Something wrong, uninitialized? */
    {
      /* Clear the immunities collection. */
      for (i = 0; i < RULE_MAX_COUNT_HIT_SETS; i++)
      {
        UICurrentGameState.CountHits[i].toPlayer = RULE_NOBODY_PLAYER;
        UICurrentGameState.CountHits[i].tradedItem = FALSE;
      }
      i = 0;
    } else
      i = freespot;
  }

  /* Yuck.  If trading, don't want to allow the total to be more than 99. */
  if (TradingItem) {
    for (j = 0; j < RULE_MAX_COUNT_HIT_SETS; j++)
    {
      if (UICurrentGameState.CountHits[j].toPlayer == ToPlayer &&
//      UICurrentGameState.CountHits[j].fromPlayer == FromPlayer &&
      UICurrentGameState.CountHits[j].properties == SquareSet &&
      UICurrentGameState.CountHits[j].hitType == (unsigned int)HitType &&
      UICurrentGameState.CountHits[j].tradedItem == FALSE)
        break;
    }
    if (j < RULE_MAX_COUNT_HIT_SETS)
      if ((HitCount + UICurrentGameState.CountHits[j].hitCount) > 99)
        HitCount = 99 - UICurrentGameState.CountHits[j].hitCount;
  }


  /* Add or update the immunity/future. */

  if (HitCount == 0)
  {
    /* This one has run down and is empty now. */
    UICurrentGameState.CountHits[i].toPlayer = RULE_NOBODY_PLAYER;
    UICurrentGameState.CountHits[i].tradedItem = FALSE;
  }
  else /* A new count - can be negative during trading. */
  {
    UICurrentGameState.CountHits[i].tradedItem = TradingItem;
    UICurrentGameState.CountHits[i].toPlayer = ToPlayer;
    UICurrentGameState.CountHits[i].fromPlayer = FromPlayer;
    UICurrentGameState.CountHits[i].hitCount = HitCount;
    UICurrentGameState.CountHits[i].properties = SquareSet;
    UICurrentGameState.CountHits[i].hitType = HitType;
  }
}
