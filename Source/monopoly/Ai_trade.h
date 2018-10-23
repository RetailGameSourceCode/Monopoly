#ifndef __AI_TRADE_H__
#define __AI_TRADE_H__

/*************************************************************
*
*   FILE:             AI_TRADE.H
*
*   (C) Copyright 97 Artech Digital Entertainments.
*                    All rights reserved.
*
*   Monopoly Game AI Header for Trading
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/

/* Immunity record.  not full implemented yet. */
typedef struct AI_FutureImmunityStruct
{
    RULE_PropertySet  properties;     /* A set of properties you can hit. */
    RULE_PlayerNumber fromPlayer;
    RULE_PlayerNumber toPlayer;
    unsigned char     count;
    RULE_TradeItemKind  hitType;  /* Immunity or future? */
} AI_Immunity_Record;

/*  When trying to figure out if to make a trade and what concessions
    we should get, this tells how desperate is the AI for money. */
enum AI_Trade_ImportanceEnum
{
	AI_TRADE_NOT_IMPORTANT = 0,
	AI_TRADE_SOMEWHAT_IMPORTANT = 1,
	AI_TRADE_DESPERATE = 1 << 1,
	AI_TRADE_FOR_CASH = 1 << 2,
    AI_TRADE_GIVE_MONOPOLY = 1 << 3
};
typedef int AI_Trade_Importance;

/* Record for keeping track of the trade offered/considered. */
typedef struct  AI_Trade_ListStruct
{

	RULE_PropertySet properties_given;
	RULE_PropertySet properties_received;
	long			 cash_received;
	long			 cash_given;
	BOOL			 jail_card_given[MAX_DECK_TYPES];
	BOOL			 jail_card_received[MAX_DECK_TYPES];
}  AI_Trade_List_Record;

	/* The following describes the current state of sending
	   a trade.  It helps keep track of what to send next
	   so the trade list is sent correctly. */
enum AI_Sending_Trade_StateEnum
{
	AI_Send_Nothing = 0,
	AI_Send_Asking_Trade_Edit,
	AI_Send_Clearing_Trade,
	AI_Send_Trade_Items,
	AI_Send_Trade_Sent
};
typedef int AI_Sending_Trade_State;

enum AI_ImportanceEnum
{
    AI_IMP_MONOPOLY = 0,
    AI_IMP_TRADE,
    AI_IMP_ONE_UNOWNED,
    AI_IMP_TWO_UNOWNED,
    AI_IMP_RAILROAD,
    AI_IMP_UTILITY,
    AI_IMP_MAX,
    AI_IMP_JUNK
};
typedef int AI_Importance_Record;

typedef struct AI_Imp_ListStruct
{
    AI_Importance_Record  item;
    double                importance;
} AI_Imp_List_Record;

enum AI_Sort_TypeEnum
{
    AI_Sort_Ascending,
    AI_Sort_Descending,
    AI_Sort_Random
};
typedef int AI_Sort_Type;

#define  AI_DONT_TRADE_ATTITUDE			-1	   /* How low must player_attitude in AI_Data_Record be for
											      AI to give up trading with this player? */

#define  AI_MAX_GIVE_IN_TRADE           2500   /* Most cash we are willing to give to make a trade fair.
                                                  The larger the cash, the longer the time it will take
                                                  to find the optimal cash flow.  Also, if too large, we
                                                  might try to give TOO much cash to players we do not want
                                                  to give a monopoly to. */
#define  AI_MAX_GIVE_IN_MONOPOLY_CASH_TRADE  16000   
                                                  /* Most cash we are willing to give to make a trade fair
                                                  where we are giving the player a monopoly for cash. */

/************************************************************/

/************************************************************/
/* PROTOTYPES                                               */
/************************************************************/
extern BOOL AI_Propose_Trade(RULE_GameStatePointer game_state, RULE_PlayerNumber player, AI_Trade_Importance importance, RULE_PlayerNumber bad_player);
extern int AI_Find_Smallest_Monopoly_Trade(RULE_PlayerNumber player,
									RULE_PropertySet monopoly, RULE_PlayerNumber *list, int num_players_in_list,
									RULE_PropertySet *properties);
extern BOOL AI_Trade_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareGroups monopoly,
						   float *player_attitudes, RULE_PlayerNumber *player_list, int list_size,
						   RULE_PropertySet *properties, AI_Trade_Importance importance);
extern BOOL AI_Give_Monopoly_For_Cash(RULE_GameStatePointer game_state, RULE_PlayerNumber player, 
						   RULE_PlayerNumber to_player, RULE_PropertySet *properties);
extern BOOL AI_Trade_Add_Type_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber from_player, RULE_PlayerNumber to_player,
								AI_Property_Importance type, AI_Trade_List_Record *trade_list, RULE_PropertySet *properties);
extern BOOL AI_Add_Trade_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber to_player,
							 RULE_PropertySet *properties, AI_Property_Classification type,
							 AI_Trade_List_Record *trade_list);
extern void	AI_Order_Monopoly_Importance(long liquid_assets, RULE_SquareGroups	*monopoly_group, AI_Sort_Type sort_order);
extern BOOL AI_Trade_SemiImportant_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
									     float *player_attitudes, AI_Trade_Importance importance,
										 RULE_PlayerNumber bad_player, RULE_PropertySet *properties);
extern BOOL  AI_Owned_By_Monopoly_Player(RULE_GameStatePointer game_state, RULE_SquareGroups group, RULE_PlayerNumber player);
extern BOOL AI_Player_Involved_In_Trade(AI_Trade_List_Record *Proposing_trade_list, RULE_PlayerNumber player);
extern BOOL AI_Make_Trade_Fair(RULE_GameStatePointer game_state, RULE_PlayerNumber player,	
                        RULE_PlayerNumber *player_list, long list_size,
						AI_Trade_List_Record *Proposing_trade_list, RULE_PropertySet *properties,
                        long give_most, BOOL giving_monopoly);
extern long AI_Get_Transfer_Tax(RULE_GameStatePointer game_state, RULE_PropertySet properties);
extern BOOL AI_Send_Proposed_Trade(RULE_GameStatePointer game_state, RULE_PlayerNumber player, BOOL trade_accept);
extern RULE_PlayerNumber	AI_Get_Next_Player_Wanting_Cash(AI_Trade_List_Record *trade_point);
extern int AI_Should_Trade_For_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player);
extern RULE_PlayerNumber  AI_Find_Nonmonopoly_Player(RULE_GameStatePointer game_state,
                                              RULE_PlayerNumber player);
extern int AI_Find_Free_Trade_Spot(RULE_GameStatePointer game_state, RULE_PlayerNumber player);
extern BOOL AI_Should_Trade(RULE_GameStatePointer game_state, RULE_PlayerNumber player, AI_Trade_Importance importance);
extern void AI_Add_Trade_Item(RULE_TradeItemKind item_kind, long amount, RULE_PlayerNumber from_player,
					   RULE_PlayerNumber to_player, RULE_PropertySet property_set);
extern void AI_Update_State(RULE_GameStatePointer game_state, AI_Trade_List_Record *list);
extern float AI_Calc_Trade_Property_Importance(RULE_GameStatePointer game_state, RULE_GameStatePointer new_state,
										RULE_PlayerNumber player, RULE_PlayerNumber ai_strat,
										AI_Trade_List_Record *trade_list, BOOL giving_monopoly_away);
extern double AI_Find_Players_Property_Importance(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
                                           RULE_PlayerNumber ai_strat, int chart_index,
                                           long *monopolies, double *monopoly_importance);
extern int AI_Num_Veto_Monopolies(RULE_PropertySet properties_owned);
extern void AI_Mortgage_Negative_Cash_Players(RULE_GameStatePointer game_state);
extern float AI_Evaluate_Trade(RULE_GameStatePointer game_state, RULE_PlayerNumber player, 
						  RULE_PlayerNumber ai_strat, AI_Trade_List_Record *trade_list);
extern float AI_Evaluate_Winning_Chances(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
								  RULE_PlayerNumber	ai_strat, float *save_player_chance);
extern void  AI_Evaluate_Trade_Player_List(RULE_GameStatePointer game_state, RULE_PlayerNumber *player_list, 
                          int list_size, RULE_PlayerNumber ai_strat, AI_Trade_List_Record *trade_list,
                          float *evaluation, BOOL giving_monopoly_for_cash);
extern void AI_Reset_Trade_List(void);
extern int AI_Is_Possible_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber owner, RULE_SquareType Square);
extern RULE_SquareType AI_Property_Being_Bought(RULE_GameStatePointer game_state, RULE_PlayerNumber	*player);
extern BOOL AI_Trade_Is_Proper(RULE_GameStatePointer game_state, AI_Trade_List_Record *Proposing_trade_list);
extern void AI_Make_Trade_Proper(RULE_GameStatePointer game_state, AI_Trade_List_Record *Proposing_trade_list);
extern BOOL AI_Is_Monopoly_Trade(RULE_GameStatePointer game_state, AI_Trade_List_Record *trade_list);
extern BOOL AI_Counter_Propose_Trade(RULE_GameStatePointer game_state,RULE_PlayerNumber player, BOOL trade_accept);
extern int AI_Create_Player_Property_Attitude_List(RULE_GameStatePointer game_state, RULE_PlayerNumber player, 
                    AI_Trade_List_Record *trade_list, int *player_index, 
                    RULE_PropertySet *player_properties, RULE_PlayerNumber *trade_player_list,
                    float *total_attitude);
extern void AI_Create_Item_Importance_List(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
                                    RULE_PlayerNumber ai_strat, AI_Imp_List_Record *imp_record);
extern BOOL AI_Add_Type_Property(RULE_GameStatePointer game_state, RULE_GameStatePointer new_state,
                          RULE_PlayerNumber player, RULE_PlayerNumber ai_strat,
                          RULE_PropertySet *combined_properties, 
                          AI_Importance_Record item, AI_Trade_List_Record *trade_list, 
                          RULE_PropertySet *properties, BOOL give_descending);
extern void AI_Set_Properties_Traded(RULE_PlayerNumber player, RULE_PlayerNumber proposed_player,
                              AI_Trade_List_Record *trade_list);
extern BOOL AI_Properties_Already_Traded(RULE_PlayerNumber player, RULE_PlayerNumber proposed_player,
                                  AI_Trade_List_Record *trade_list, int danger_level);
extern void AI_Properties_Forget_Traded(RULE_PlayerNumber player);
extern BOOL AI_Should_Give_Away_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player);
extern double  AI_Get_Cash_Multiplier(RULE_PlayerNumber ai_strat, RULE_PlayerNumber player);
extern BOOL AI_Player_Give_Immunities_Futures(RULE_PlayerNumber player);
extern BOOL AI_Use_The_Force(RULE_GameStatePointer game_state, RULE_PlayerNumber player);
/************************************************************/


#endif // __AI_TRADE_H__