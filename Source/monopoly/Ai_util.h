#ifndef __AI_UTIL_H__
#define __AI_UTIL_H__

/*************************************************************
*
*   FILE:             AI_UTIL.H
*
*   (C) Copyright 97 Artech Digital Entertainments.
*                    All rights reserved.
*
*   Monopoly Game AI UTILS header.
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/

/*  Describes the stage of game play.  */
enum AI_Monopoly_StageEnum
{
		/*  Lots of properties to buy, and no one owns a monopoly. */
	AI_STAGE_BUYING = 0,
		/*  Almost all property bought, and no one owns a monopoly. */
	AI_STAGE_NO_MONOPOLIES,
		/*  A monopoly exists, but the player asking doesn't have one. */
	AI_STAGE_MONOPOLIES_NOT_OWN_ONE,
		/*  Monopolies exist, and the player asking has one. */
	AI_STAGE_MONOPOLIES_OWN_ONE
};
typedef int AI_Monopoly_Stage;


// The following are possible reactions by tokens, numbered the
// way the sequences are.
enum AI_Token_ReactionsEnum
{
	AI_REACTION_HAPPY = 0,
	AI_REACTION_SAD,
	AI_REACTION_NEUTRAL,
	AI_REACTION_ANGRY
};
typedef int AI_Token_Reactions;

// The following defines tell us what reaction the tokens should have based on
// the rent payed or the rent received when landing on a square. 
#define AI_HAPPY_REACTION_RENT			250
#define AI_SAD_REACTION_RENT			100
#define AI_ANGRY_REACTION_RENT			500

/************************************************************/

/************************************************************/
/* Extern variables											*/
/************************************************************/
extern BOOL AI_Drone_Game;
extern const double  AI_Property_Landing_Frequency[SQ_MAX_SQUARE_TYPES];

/************************************************************/
/* PROTOTYPES                                               */
/************************************************************/
extern RULE_PropertySet  AI_Get_Monopoly_Set(RULE_SquareType monopoly);
extern int	AI_Number_Properties_In_Set(RULE_PropertySet properties);
extern AI_Monopoly_Stage	AI_Get_Monopoly_Stage(RULE_GameStatePointer game_state, RULE_PlayerNumber player);
extern int	 AI_Num_Of_Monopolies_In_Set(RULE_PropertySet properties_owned);
extern int  AI_Num_Of_Monopolies_Between_Players(RULE_GameStatePointer game_state, RULE_PlayerNumber *list, 
										 int players_in_list, RULE_SquareType extra_property);
extern void AI_Remove_Players_From_List(RULE_PlayerNumber *player_list, RULE_PlayerNumber *remove_list,
								int players_in_list, int players_in_remove_list);
extern long	AI_Get_Monopolies(RULE_GameStatePointer game_state, RULE_PlayerNumber player, 
						  RULE_SquareType	*store_squares, BOOL mortgage_counts);
extern BOOL  AI_Player_Close_To_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber ignore_player,
								  RULE_SquareType property, long min_distance, long max_distance);
extern long AI_Cost_Unmortgage_Monopoly(RULE_GameStatePointer game_state, RULE_SquareType SquareNo);
extern BOOL AI_Someone_Close_To_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player, 
								  RULE_SquareType *monopoly_squares, int  num_squares_in_monopoly);
extern RULE_SquareType AI_Highest_Rent_Square(RULE_GameStatePointer game_state, RULE_PlayerNumber player, long *rent);
extern RULE_PlayerNumber AI_Best_Current_Rent(RULE_GameStatePointer game_state);
extern RULE_PlayerNumber AI_Most_Expensive_Potential_Income(RULE_GameStatePointer game_state);
extern long AI_Potential_Income(RULE_GameStatePointer game_state, RULE_PlayerNumber player);
extern BOOL  AI_Exist_Monopoly(RULE_GameStatePointer game_state);
extern BOOL  AI_Player_Own_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player, BOOL count_baltic);
extern long AI_Get_Liquid_Assets(RULE_GameStatePointer game_state, RULE_PlayerNumber player, BOOL count_houses, BOOL count_monopolies);
extern long AI_Get_Liquid_Assets_Property(RULE_GameStatePointer game_state, RULE_PropertySet properties_owned, BOOL count_houses, BOOL count_monopolies);
extern long AI_Get_Total_Worth(RULE_GameStatePointer game_state, RULE_PlayerNumber player);
extern RULE_SquareType	AI_Find_Lowest_Rent(RULE_GameStatePointer game_state,RULE_PlayerNumber player, RULE_PropertySet properties);
extern RULE_SquareType	AI_Find_Highest_Rent_Mortgaged(RULE_GameStatePointer game_state,RULE_PlayerNumber player, RULE_PropertySet properties,
											   long top_unmortgage_cost);
extern int	 AI_Get_Monopoly_Lots(RULE_GameStatePointer game_state, RULE_SquareType *storage_for_squares, RULE_SquareType square);
extern int AI_Num_Houses_On_Monopoly(RULE_GameStatePointer game_state,RULE_SquareType Square);
extern BOOL AI_Is_Monopoly(RULE_GameStatePointer game_state,RULE_SquareType Square);
extern RULE_PropertySet  AI_XOR_Property_Set(RULE_PropertySet property_set, ...);
extern BOOL	AI_Housing_Shortage(RULE_GameStatePointer game_state, unsigned char critical_level);
extern unsigned char  AI_Free_Houses(RULE_GameStatePointer game_state);
extern unsigned char  AI_Free_Hotels(RULE_GameStatePointer game_state);
extern BOOL AI_Own_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareType Square, BOOL mortgage,
                            RULE_PropertySet properties_owned);
extern int	AI_Number_Railroads_Utils_Owned(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareGroups group, 
									BOOL mortgage, RULE_PropertySet properties_owned);
extern long	AI_Rent_If_Stepped_On(RULE_GameStatePointer game_state, RULE_SquareType Square,
                              RULE_PropertySet properties_owned);
extern float AI_Average_Rent_Received(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareType SquareNo,
							   BOOL dont_count_developed_monopolies, float cash_cow_multiplier, RULE_PropertySet properties_owned);
extern long AI_Average_Rent_Payed(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareType SquareNo,
						   BOOL consider_developed_monopolies, float cash_cow_multiplier);
extern long  AI_Get_Most_Liquid_Assets(RULE_GameStatePointer  game_state, RULE_PlayerNumber player, BOOL count_monopolies);
extern BOOL AI_Is_Cash_Cow(RULE_SquareType property);
extern long  AI_Min_Calculated_Expenses(RULE_GameStatePointer game_state, RULE_PlayerNumber player);
extern BOOL AI_Only_Player_Have_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player);
extern BOOL AI_Owns_Prop_From_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player, 
								 RULE_SquareType property);
extern long AI_Num_House_Can_Buy_On_Monopoly(RULE_GameStatePointer game_state, RULE_SquareType square);
extern BOOL	AI_Test_For_Monopoly(RULE_PropertySet properties_owned, RULE_SquareType Square);
extern RULE_PlayerNumber  AI_Who_Owns_Monopoly(RULE_GameStatePointer game_state, RULE_SquareType Square);
extern long	AI_Cash_Available_After_Housing(RULE_GameStatePointer game_state, RULE_PlayerNumber player);
extern int	AI_Num_Properties_Left_To_Buy(RULE_GameStatePointer game_state);
extern RULE_SquareType AI_Property_Being_Bought(RULE_GameStatePointer game_state, RULE_PlayerNumber *player);
extern AI_Token_Reactions AI_Square_Move_Reaction(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
								  		   RULE_SquareType land_square, long misc);
/************************************************************/

#endif // __AI_UTIL_H__
