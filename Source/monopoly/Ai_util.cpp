/*************************************************************
*
*   FILE:             AI_UTIL.C
*
*   (C) Copyright 97  Artech Digital Entertainments.
*                     All rights reserved.
*************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/
#include "gameinc.h"


/************************************************************/

/************************************************************/
/* PROTOTYPES                                               */
/************************************************************/
extern RULE_SquareType AI_Expensive_Monopoly_Square[SG_PARK_PLACE + 1];
extern double AI_Monopoly_To_Assets_Importance[SG_PARK_PLACE + 1][5000 / 20 + 1];
extern RULE_GameStateRecord UICurrentGameState;
extern AI_Data_Record AI_Data[RULE_MAX_PLAYERS];
extern AI_Data_Record AI_Expert_AI_Settings;
extern AI_Message_State_Record	AI_State[RULE_MAX_PLAYERS];
extern AI_Global_Record AI_Glob;
extern AI_Group_Start_End_Record AI_Group_Start_End[];
extern AI_Trade_List_Record  Current_Trade_List[RULE_MAX_PLAYERS];
extern AI_Sending_Trade_State AI_PLAYER_SENDING_TRADE;
extern BOOL AI_SEND_TRADE_OFFER_TRADE;
/************************************************************/

/*  Making RULE_PropertyToBitSet a macro to speed up certain operations for
    superfast mode. */

#define RULE_PropertyToBitSet(Square)   (RULE_PropertyToBitsetArray[Square])

//  The following are the average landing frequency for each monopoly square.

const double  AI_Property_Landing_Frequency[SQ_MAX_SQUARE_TYPES] =
{	/* Note: Don't have landing freqs for chance/community squares.  Assuming 1.
	   Probably not important, as they probably won't be used in any calculations. */
	1.14,			// SQ_GO
	0.77,			// SQ_MEDITERRANEAN_AVENUE
	1,				// SQ_COMMUNITY_CHEST_1
    0.8,			// SQ_BALTIC_AVENUE
	0.88,			// SQ_INCOME_TAX
	1.14,			// SQ_READING_RR
	0.86,			// SQ_ORIENTAL_AVENUE
	1,				// SQ_CHANCE_1
	0.89,			// SQ_VERMONT_AVENUE
	0.9,			// SQ_CONNECTICUT_AVENUE
	1,				// SQ_JUST_VISITING
	1.05,			// SQ_ST_CHARLES_PLACE
	1.07,			// SQ_ELECTRIC_COMPANY
	0.87,			// SQ_STATES_AVENUE
	0.95,			// SQ_VIRGINIA_AVENUE
	1.08,			// SQ_PENNSYLVANIA_RR
	1.06,			// SQ_ST_JAMES_PLACE
	1,				// SQ_COMMUNITY_CHEST_2
	1.11,			// SQ_TENNESSEE_AVENUE
	1.14,			// SQ_NEW_YORK_AVENUE
	1.10,			// SQ_FREE_PARKING
	1.07,			// SQ_KENTUCKY_AVENUE
	1,				// SQ_CHANCE_2
	1.02,			// SQ_INDIANA_AVENUE
	1.19,			// SQ_ILLINOIS_AVENUE
	1.14,			// SQ_BnO_RR
	1.01,			// SQ_ATLANTIC_AVENUE
	1.02,			// SQ_VENTNOR_AVENUE
	1.04,			// SQ_WATER_WORKS
	0.98,			// SQ_MARVIN_GARDENS
	0.97,			// SQ_GO_TO_JAIL
	0.99,			// SQ_PACIFIC_AVENUE
	0.97,			// SQ_NORTH_CAROLINA_AVENUE
	1,				// SQ_COMMUNITY_CHEST_3
	0.91,			// SQ_PENNSYLVANIA_AVENUE
	0.88,			// SQ_SHORT_LINE_RR
	1,				// SQ_CHANCE_3
	0.8,			// SQ_PARK_PLACE
	0.8,			// SQ_LUXURY_TAX
	0.98,			// SQ_BOARDWALK
	0,				// SQ_IN_JAIL
	0				// SQ_OFF_BOARD
};



/******************************************************************/
/*					AI_Get_Monopoly_Set							  */
/*																  */
/*   Input:  RULE_SquareType   monopoly							  */
/*   Output: RULE_PropertySet  The property set belonging to the  */
/*			   monopoly containing 'monopoly' square.             */
/******************************************************************/

RULE_PropertySet  AI_Get_Monopoly_Set(RULE_SquareType monopoly)
{
	RULE_SquareGroups	group;
	RULE_SquareType		SquareNo, end_square;
	RULE_PropertySet	property_set = 0;

	group = RULE_SquarePredefinedInfo[monopoly].group;

    SquareNo = AI_Group_Start(group);
    end_square = AI_Group_End(group);
	for(; SquareNo < end_square; ++SquareNo)
	{
		if(RULE_SquarePredefinedInfo[SquareNo].group == group)
			property_set |= RULE_PropertyToBitSet(SquareNo);
	}

return (property_set);
}



/************************************************************/
/*					AI_Number_Properties_In_Set				*/
/*															*/
/*   Input:  RULE_PropertySet   properties   				*/
/*	 Output: # of properties in set.						*/
/************************************************************/

int	AI_Number_Properties_In_Set(RULE_PropertySet properties)
{
	int num_in_set = 0;

	while(properties != 0)
	{
		if(properties & 1)
			num_in_set++;

		properties = properties >> 1;
	}

return num_in_set;
}



/**********************************************************************/
/*					AI_Num_Of_Monopolies_In_Set						  */
/*																	  */
/*   Input: RULE_Property_Set      properties_owned					  */
/*	 Output:  int	- # of monopolies found in properties_owned.	  */
/*										               				  */
/*		Returns the number of monopolies found in properties_owned.   */
/**********************************************************************/

int	 AI_Num_Of_Monopolies_In_Set(RULE_PropertySet properties_owned)
{
	int		monopoly_count = 0;
	int		SquareNo;

	/*  Go through all the monopoly groups. */
	for(SquareNo = 0; SquareNo < SG_PARK_PLACE + 1; SquareNo++)
	{	
		/* Is it a monopoly? */
		if(AI_Test_For_Monopoly(properties_owned, AI_Expensive_Monopoly_Square[SquareNo]))
			monopoly_count++;
	}

return (monopoly_count);
}


/******************************************************************************/
/*					AI_Num_Of_Monopolies_Between_Players					  */
/*																			  */
/*   Input: RULE_GameStatePointer	game_state								  */
/*			RULE_PlayerNumber		*list			List of players to test	  */
/*			int					    players_in_list  How many players?		  */
/*			RULE_SquareType			extra_property   Including this property  */
/*	 Output:  int	- # of monopolies found if all players in *list were to   */
/*					  combine their properties.								  */
/*																			  */
/*		Finds out how many monopolies the players found in *list have between */
/*   themselves + extra_property (set to 0 if none.).  Useful to figure out   */
/*   if a monopoly trade is possible, or once 'extra_property' is gained.     */
/*   'players_in_list' is the number of players found in *list.				  */
/******************************************************************************/

int  AI_Num_Of_Monopolies_Between_Players(RULE_GameStatePointer game_state, RULE_PlayerNumber *list,
										 int players_in_list, RULE_SquareType extra_property)
{
	int	index, monopolies_found = 0;
	RULE_PropertySet properties_owned = 0;

	/*  Get a combined property list.  */
	for(index = 0; index < players_in_list; ++index)
		properties_owned |= RULE_PropertySetOwnedByPlayer(game_state, *list++);

	properties_owned |= RULE_PropertyToBitSet(extra_property);
	/*  Now get the number of monopolies in the list. */
	monopolies_found = AI_Num_Of_Monopolies_In_Set(properties_owned);

return (monopolies_found);
}


/******************************************************************************/
/*					AI_Remove_Players_From_List								  */
/*																			  */
/*   Input:	RULE_PlayerNumber	    *player_list	List of players in a list */
/*			RULE_PlayerNumber		*remove_list	List of players to delete */
/*												   from *player_list.		  */
/*			int						players_in_list		How many in list?     */
/*			int					players_in_remove_list	How many in remove?	  */
/*																			  */
/*		Removes players in *remove_list from *player_list and moves all the   */
/*  remaining players in *player_list up.									  */
/******************************************************************************/

void AI_Remove_Players_From_List(RULE_PlayerNumber *player_list, RULE_PlayerNumber *remove_list,
								int players_in_list, int players_in_remove_list)
{
	RULE_PlayerNumber	cur_player, index;

	while(players_in_remove_list > 0)
	{	/* Go through all the players in the remove list */

		players_in_remove_list--;
		for(cur_player = 0; cur_player < players_in_list; ++cur_player)
		{	/* Check for the current remove player in the list. */

			if(remove_list[players_in_remove_list] == player_list[cur_player])
			{	/* Found him in the list.  Move everybody up in the list. */

				players_in_list--;
				for(index = cur_player; index < players_in_list; ++index)
					player_list[index] = player_list[index + 1];
			}
		}
	}

return;
}

/**********************************************************************/
/*					AI_Get_Monopolies								  */
/*																	  */
/*   Input: RULE_GameStatePointer  game_state						  */
/*			RULE_PlayerNumber	   player      Who's monopolies?	  */		
/*			RULE_SquareType		   *store_squares  Where to store     */
/*				  the most expensive square from each monopoly. NULL  */
/*                if not to store.                                    */
/*		    BOOL  mortgage_counts		Should the fact that one of   */
/*				     the monopoly properties is mortgaged count?	  */
/*	 Output:  long	- # of monopolies found owned by player			  */
/*										               				  */
/*		Returns the number of monopolies 'player' owns, and a list    */
/*   containing the most expensive property from each in			  */
/*   *store_squares.												  */
/**********************************************************************/


long	AI_Get_Monopolies(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
						  RULE_SquareType	*store_squares, BOOL mortgage_counts)
{
	long	monopoly_count = 0;
	int		SquareNo;
    RULE_PropertySet  properties_owned = RULE_PropertySetOwnedByPlayer(game_state,player);

	/*  Go through all the monopoly groups. */
	for(SquareNo = 0; SquareNo < SG_PARK_PLACE + 1;SquareNo++)
	{	
		/* Do we own it? */
		if(AI_Own_Monopoly(game_state, player, AI_Expensive_Monopoly_Square[SquareNo], mortgage_counts,
                           properties_owned))
		{	/* We own it */
            if(store_squares != NULL)
			    *store_squares++ = AI_Expensive_Monopoly_Square[SquareNo];
			monopoly_count++;
		}
	}

return (monopoly_count);
}

/**********************************************************************/
/*					AI_Player_Close_To_Property						  */
/*																	  */
/*   Input: RULE_GameStatePointer  game_state						  */
/*			RULE_PlayerNumber	   ignore_player  Who to ingore		  */		
/*			RULE_SquareType		   property    Where?				  */
/*			long	min_distance  Must be at least this far			  */	
/*			long    max_distance  Must be at most this far			  */
/*	 Output:  BOOL  - TRUE if an opponent is within min/max of        */
/*					  property.  False otherwise.					  */
/*										               				  */
/*		Checks to see whether an opponent (anyone not ignore_player)  */
/*  is within min_distance and max_distance of  'property'.  False    */
/*  otherwise.														  */
/**********************************************************************/

BOOL  AI_Player_Close_To_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber ignore_player,
								  RULE_SquareType property, long min_distance, long max_distance)
{
	RULE_PlayerNumber	player;
	int					distance_away;

	for(player = 0; player < game_state->NumberOfPlayers; ++player)
	{
		if(player == ignore_player)
			continue;

		if(game_state->Players[player].currentSquare > SQ_BOARDWALK)
			continue;	/* Not on the board as we know it. Off, in jail, or something else. */

		distance_away = property - game_state->Players[player].currentSquare;
			/* Need to take care of wrapping - ie. player before GO, property after GO. */
		if(distance_away < -SQ_FREE_PARKING)  /* more than half the board ahead of us? */
			distance_away += SQ_BOARDWALK + 1;

		if((distance_away >= min_distance) && (distance_away <= max_distance))
			return TRUE;	/* Found someone within the right distance. */
	}

	/* D'OH!  Didn't find anyone within min and max distances. */
return FALSE;
}


/**********************************************************************/
/*					AI_Cost_Unmortgage_Monopoly						  */
/*																	  */
/*   Input: RULE_GameStatePointer  game_state						  */
/*			RULE_SquareType  	   SquareNo		Which monopoly?		  */
/*	 Output: long - cost of unmortgaging all the mortgaged properties */
/*					of monopoly containing 'SquareNo'.				  */
/**********************************************************************/

long AI_Cost_Unmortgage_Monopoly(RULE_GameStatePointer game_state, RULE_SquareType SquareNo)
{
	int		monopoly_lots;
	RULE_SquareType	mono_squares[AI_MAX_MONOPOLY_LOTS + 1];
	long	cost = 0;

	/* Get the monopoly squares */
	monopoly_lots = AI_Get_Monopoly_Lots(game_state, mono_squares, SquareNo);

	/* Go through them and check if each is mortgaged. */
	while(monopoly_lots > 0)
	{
		/* Next monopoly square mortaged? If yes, add the unmortgaged cost. */
		if(game_state->Squares[mono_squares[monopoly_lots - 1]].mortgaged == TRUE)
			cost += (long) (RULE_SquarePredefinedInfo[mono_squares[monopoly_lots - 1]].mortgageCost * 1.1);

		monopoly_lots--;
	}

return (cost);
}


/**********************************************************************/
/*					AI_Someone_Close_To_Monopoly					  */
/*																	  */
/*   Input: RULE_GameStatePointer  game_state						  */
/*			RULE_PlayerNumber	   player      Whose monopoly?		  */		
/*			RULE_SquareType		   *monopoly_squares  Properties in   */
/*												monopoly			  */
/*			int	 num_squares_in_monopoly   How large is the monopoly? */
/*	 Output: BOOL  - True if anyone except for player is within a	  */
/*		             dice roll away from any property in              */
/*					 *monopoly_squares.								  */
/**********************************************************************/


BOOL AI_Someone_Close_To_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
								  RULE_SquareType *monopoly_squares, int  num_squares_in_monopoly)
{
	BOOL	within_12_spaces = FALSE;
	int		index;


	within_12_spaces = FALSE;
	for(index = 0; index < num_squares_in_monopoly; ++index)
	{
		if(AI_Player_Close_To_Property(game_state, player, monopoly_squares[index], 2, 12))
		{
			within_12_spaces = TRUE;
			break;
		}
	}

return within_12_spaces;
}


/********************************************************************/
/*					AI_Highest_Rent_Square							*/
/*																	*/
/*   Input: RULE_GameStatePointer  game_state						*/
/*			RULE_PlayerNumber	player		Who's squares should we	*/
/*											ignore					*/
/*			long				*rent   Where to store rent paid.	*/
/*	 Output: RULE_SquareType  - highest rent square					*/		
/*										               				*/
/*		Finds the opponents' property with the highest rent.  If no */
/*  opponents' property exists that is not mortgaged, will return   */
/*  0.  Returns the square number, and stores the rent on this      */
/*  square in *rent.												*/
/********************************************************************/

RULE_SquareType AI_Highest_Rent_Square(RULE_GameStatePointer game_state, RULE_PlayerNumber player, long *rent)
{
	RULE_PropertySet	properties_owned = RULE_PropertySetOwnedByPlayer(game_state,player);
	RULE_SquareType		SquareNo;
	long				highest_rent = 0, current_rent;
	RULE_SquareType		highest_rent_square = 0;
	
	for(SquareNo = 0; SquareNo < SQ_IN_JAIL;SquareNo++)
	{
		if(game_state->Squares[SquareNo].mortgaged)	
			continue;	/* Mortgaged?  No rent to worry about */

		if(game_state->Squares[SquareNo].owner == player)
			continue;	/* Our own - won't pay a thing. */

		current_rent = AI_Rent_If_Stepped_On(game_state, SquareNo, properties_owned);
		
		if(current_rent > highest_rent)
		{
			current_rent = highest_rent;
			highest_rent_square = SquareNo;
		}	
	}

	*rent = highest_rent;
return (highest_rent_square);
}


/********************************************************************/
/*					AI_Best_Current_Rent							*/
/*																	*/
/*   Input: RULE_GameStatePointer  game_state						*/
/*	 Output: RULE_PlayerNumber	   Who is charging the most	rent?	*/		
/*										               				*/
/*		Finds the player who is currently charging, on average, the */
/*  most rent from all properties.									*/
/********************************************************************/

RULE_PlayerNumber AI_Best_Current_Rent(RULE_GameStatePointer game_state)
{
long	highest_income = 0, current_income;
RULE_PlayerNumber highest_player = RULE_NOBODY_PLAYER;
RULE_PlayerNumber	player_counter;

	for(player_counter = 0; player_counter < game_state->NumberOfPlayers; ++player_counter)
	{
		current_income = AI_Average_Rent_Payed(game_state, player_counter, 0, TRUE, 1.0f);

		/* Did we find a new high income?  Save it then. */
		if(current_income > highest_income)
		{
			highest_income = current_income;
			highest_player = player_counter;
		}
	}

return highest_player;
}


/********************************************************************/
/*					AI_Most_Expensive_Potential_Income				*/
/*																	*/
/*   Input: RULE_GameStatePointer  game_state						*/
/*	 Output: RULE_PlayerNumber	   Who could be charging the most	*/		
/*				rent, if monopolies fully developed and everything  */
/*				unmortgaged.										*/
/*										               				*/
/*		Finds the player who could potentially have the highest     */
/*   income, if all properties were unmortgaged and every monopoly  */
/*   fully developed.												*/
/********************************************************************/

RULE_PlayerNumber AI_Most_Expensive_Potential_Income(RULE_GameStatePointer game_state)
{
	long	highest_income = 0, current_income;
	RULE_PlayerNumber highest_player = RULE_NOBODY_PLAYER;
	RULE_PlayerNumber	player_counter;

	for(player_counter = 0; player_counter < game_state->NumberOfPlayers; ++player_counter)
	{
		/* Get the potential income of player_counter. */
		current_income = AI_Potential_Income(game_state, player_counter);

		/* Did we find a new high income?  Save it then. */
		if(current_income > highest_income)
		{
			highest_income = current_income;
			highest_player = player_counter;
		}
	}


return (highest_player);
}

/********************************************************************/
/*					AI_Potential_Income								*/
/*																	*/
/*   Input: RULE_GameStatePointer  game_state						*/
/*			RULE_PlayerNumber	   player		Whose income?		*/
/*	 Output: long   - potential income of player.					*/		
/*										               				*/
/*		Finds player's income if all properties were unmortgaged and*/
/*   all monopolies fully developed.							    */
/********************************************************************/

long AI_Potential_Income(RULE_GameStatePointer game_state, RULE_PlayerNumber player)
{
long	current_income;
RULE_PlayerNumber highest_player = RULE_NOBODY_PLAYER;
RULE_SquareType	SquareNo;
RULE_SquareInfoRecord	*cur_square;
RULE_SquarePredefinedInfoRecord		*cur_square_info;

	current_income = 0;
	cur_square = game_state->Squares;
	cur_square_info = RULE_SquarePredefinedInfo;

	for(SquareNo = 0; SquareNo < SQ_IN_JAIL; SquareNo++, cur_square++, cur_square_info++ )
	{
		if(cur_square->owner == player)
		{
			if(AI_Is_Monopoly(game_state, SquareNo) )
			{	/* Is this a monopoly? Then take houses into account. */
				if(cur_square->houses == 0)
					current_income += cur_square_info->rent[0] * 2;
				else
					current_income += cur_square_info->rent[game_state->GameOptions.housesPerHotel];
			}
			else
				current_income += cur_square_info->rent[0];
		}
	}


return (current_income);
}

/********************************************************************/
/*					AI_Exist_Monopoly     							*/
/*																	*/
/*   Input: RULE_GameStatePointer  game_state						*/
/*	 Output: BOOL    TRUE if someone owns a monopoly.				*/		
/********************************************************************/

BOOL  AI_Exist_Monopoly(RULE_GameStatePointer game_state)
{
	int	SquareNo;

	for(SquareNo = 0; SquareNo < SG_PARK_PLACE + 1;SquareNo++)
	{
		if(AI_Is_Monopoly(game_state, AI_Expensive_Monopoly_Square[SquareNo]))
			return TRUE;
	}

return FALSE;
}

/****************************************************************************/
/*					AI_Player_Own_Monopoly 							        */
/*																	        */
/*   Input: RULE_GameStatePointer  game_state						        */
/*			RULE_PlayerNumber	   player						            */
/*          BOOL                   count_baltic  Baltic monopoly counts?    */
/*	 Output: BOOL    TRUE if player owns a monopoly.				        */		
/****************************************************************************/

BOOL  AI_Player_Own_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player, BOOL count_baltic)
{
	int	SquareNo;

    for(SquareNo = (count_baltic ? 1:0); SquareNo <= SG_PARK_PLACE; SquareNo++)
	{
		if(AI_Is_Monopoly(game_state, AI_Expensive_Monopoly_Square[SquareNo]))
		{
			if(game_state->Squares[AI_Expensive_Monopoly_Square[SquareNo]].owner == player)
				return TRUE;
		}
	}

return FALSE;
}

/********************************************************************/
/*					AI_Get_Liquid_Assets  							*/
/*																	*/
/*   Input: RULE_GameStatePointer  game_state						*/
/*			RULE_PlayerNumber	player		Check who owns it?		*/
/*			BOOL count_houses   TRUE if we're going to sell houses	*/
/*								if necessary.						*/
/*			BOOL count_monopolies  TRUE if we're going to mortgage  */
/*								 undeveloped monopolies.			*/
/*	 Output: long    cash available									*/		
/*										               				*/
/*		Returns the cash 'player' would have if mortgaged			*/
/*  all property, except perhaps monopolies and houses (depending	*/
/*  on count_houses and count_monopolies)							*/
/********************************************************************/


long AI_Get_Liquid_Assets(RULE_GameStatePointer game_state, RULE_PlayerNumber player, BOOL count_houses, BOOL count_monopolies)
{
	long total = game_state->Players[player].cash;
	RULE_PropertySet	properties_owned = RULE_PropertySetOwnedByPlayer(game_state,player);

	total += AI_Get_Liquid_Assets_Property(game_state, properties_owned, count_houses, count_monopolies);

    /* Make sure we include the money owed to the bank. */
    if(SlotIsALocalAIPlayer[player])
        total -= AI_State[player].money_owed;

return total;
}


/********************************************************************/
/*					AI_Get_Liquid_Assets_Property					*/
/*																	*/
/*   Input: RULE_GameStatePointer  game_state						*/
/*			RULE_PropertySet	   properties  Sum up what props?   */
/*			BOOL count_houses   TRUE if we're going to sell houses	*/
/*								if necessary.						*/
/*			BOOL count_monopolies  TRUE if we're going to mortgage  */
/*								 undeveloped monopolies.			*/
/*	 Output: long    cash available									*/		
/*										               				*/
/*		Returns the cash one would get for mortgaging properties.	*/
/********************************************************************/


long AI_Get_Liquid_Assets_Property(RULE_GameStatePointer game_state, RULE_PropertySet properties_owned, BOOL count_houses, BOOL count_monopolies)
{
	long total = 0;
	RULE_SquareType		SquareNo;
	

	for(SquareNo = 0; SquareNo < SQ_IN_JAIL;SquareNo++)
	{
		if(game_state->Squares[SquareNo].mortgaged)	
			continue;	/* Mortgaged?  No liquid assets to add then.  */

		if(RULE_PropertyToBitSet(SquareNo) & properties_owned)	/* Do we own this property? */
		{
			if(AI_Num_Houses_On_Monopoly(game_state, SquareNo) == 0)
			{	          /* Does it have houses on it? If no, can unmortgage it. */
				if(AI_Is_Monopoly(game_state, SquareNo) && (!count_monopolies))
					continue;

				total += RULE_SquarePredefinedInfo[SquareNo].mortgageCost;
			}
			else if(count_houses == TRUE)  /* If has houses, count only if count_houses is TRUE. */
			{
				total += RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost / 2 * game_state->Squares[SquareNo].houses;
				total += RULE_SquarePredefinedInfo[SquareNo].mortgageCost;
			}
		}
	}
	
return total;
}


/********************************************************************/
/*					AI_Get_Total_Worth    							*/
/*																	*/
/*   Input: RULE_GameStatePointer  game_state						*/
/*			RULE_PlayerNumber	player		Check who owns it		*/
/*	 Output: long    total worth of player							*/		
/********************************************************************/

long AI_Get_Total_Worth(RULE_GameStatePointer game_state, RULE_PlayerNumber player)
{
	long total = 0;
	RULE_PropertySet	properties_owned = RULE_PropertySetOwnedByPlayer(game_state,player);
	RULE_SquareType		SquareNo;
	

	for(SquareNo = 0; SquareNo < SQ_IN_JAIL;SquareNo++)
	{
		if(RULE_PropertyToBitSet(SquareNo) & properties_owned)	/* Do we own this property? */
		{
			total += RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost * game_state->Squares[SquareNo].houses;
			total += RULE_SquarePredefinedInfo[SquareNo].purchaseCost;
		}
	}
	
	// Now include cash on hand.
	total += game_state->Players[player].cash;
return total;
}

/************************************************************************************/
/*					AI_Get_Monopoly_Lots      										*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*			RULE_SquareType   *storage_for_squares    Buffer to store monopoly #s   */
/*			RULE_SquareType    square		Get lots containing this property		*/
/*											mortgaging							    */
/*	 Output:  int -    # of properties in monopoly containing 'square'              */
/*																				    */
/*		Returns the property numbers in monopoly containing 'square' property.      */
/*   Outputs number of properties found in this monopoly,  Stores the list in       */
/*   'storage_for_squares'.  Due to usefullness later, will place the most          */
/*	 expensive property first.														*/
/************************************************************************************/

int	 AI_Get_Monopoly_Lots(RULE_GameStatePointer game_state, RULE_SquareType *storage_for_squares, RULE_SquareType square)
{
	RULE_SquareType		SquareNo, end_square;
	RULE_SquarePredefinedInfoPointer square_point;
	RULE_SquareGroups	group = RULE_SquarePredefinedInfo[square].group;
	int					num_found_squares = 0;
	RULE_SquareType		temp_swap;

    SquareNo = AI_Group_Start(group);
    end_square = AI_Group_End(group);
    square_point = &RULE_SquarePredefinedInfo[SquareNo];

	for(; SquareNo < end_square; SquareNo++, square_point++)
	{
		if(square_point->group == group)
		{
			storage_for_squares[num_found_squares] = SquareNo;
			num_found_squares++;
		}
	}


	/* I want the most expensive monopoly lot first.  Useful later.  */
	for(SquareNo = 0; SquareNo < num_found_squares; ++SquareNo)
	{
		if(RULE_SquarePredefinedInfo[storage_for_squares[SquareNo]].rent[0] >
			RULE_SquarePredefinedInfo[storage_for_squares[0]].rent[0])
		{
			temp_swap = storage_for_squares[0];
			storage_for_squares[0] = storage_for_squares[SquareNo];
			storage_for_squares[SquareNo] = temp_swap;
		}
	}

return num_found_squares;
}


/************************************************************************************/
/*					AI_Num_Houses_On_Monopoly  										*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*			RULE_SquareType    square		Which monopoly are we interested in?    */
/*	 Output:  int   -    # of houses on monopoly containing 'square'		        */
/************************************************************************************/


int AI_Num_Houses_On_Monopoly(RULE_GameStatePointer game_state,RULE_SquareType Square)
{
	RULE_SquareType		SquareNo, end_square;
	RULE_SquareInfoRecord  *square_point;
	RULE_SquareGroups	group = RULE_SquarePredefinedInfo[Square].group;
	int					num_houses = 0;

    SquareNo = AI_Group_Start(group);
    end_square = AI_Group_End(group);
    square_point = &game_state->Squares[SquareNo];
	for(; SquareNo < end_square; SquareNo++, square_point++)
	{
		if(RULE_SquarePredefinedInfo[SquareNo].group == group)
			num_houses += square_point->houses;
	}

return num_houses;
}

/************************************************************************************/
/*					AI_Is_Monopoly        											*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*			RULE_SquareType    square		Which monopoly are we interested in?    */
/*	 Output:  BOOL   -    True if the monopoly containing 'square' is all owned     */
/*						  by one player.											*/
/************************************************************************************/


BOOL AI_Is_Monopoly(RULE_GameStatePointer game_state,RULE_SquareType Square)
{
	RULE_SquareType		SquareNo, end_square;
	RULE_SquareInfoRecord  *square_point = game_state->Squares;
	RULE_SquareGroups	group = RULE_SquarePredefinedInfo[Square].group;
	RULE_PlayerNumber	owner;

	owner = game_state->Squares[Square].owner;

	if(owner == RULE_NOBODY_PLAYER)
		return FALSE;	/* Nobody bought the square, hence no one owns it. */

	if(group >= SG_MAX_PROPERTY_GROUPS)
		return FALSE;	// Must be a legal group. 

    SquareNo = AI_Group_Start(group);
    end_square = AI_Group_End(group);
    square_point = &game_state->Squares[SquareNo];

	for(; SquareNo < end_square; SquareNo++, square_point++)
	{
		if((square_point->owner != owner) && (RULE_SquarePredefinedInfo[SquareNo].group == group))
			return FALSE;
	}

return TRUE;
}

/************************************************************************************/
/*					AI_XOR_Property_Set												*/
/*																					*/
/*	Input:  RULE_PropertySet	property_set	A starting property_set to XOR.     */
/*			RULE_SquareType     ...				Properties to XOR into property_set */
/*												terminated by a 0 argument.         */
/*  Output: RULE_PropertySet	New property set with the input SquareType lots     */
/*							    XORed.												*/
/************************************************************************************/

RULE_PropertySet  AI_XOR_Property_Set(RULE_PropertySet property_set, ...)
{
	va_list		list;
	RULE_SquareType     square;
	
	va_start(list, property_set);   /* Get the beggining of the extra arguments */

	do
	{
		square = va_arg( list, RULE_SquareType);  /* Get the next argument */
		if(square != 0)		/*  Is it our termination marker?  */
		{	/* No? Then XOR the property from the set */
			property_set ^= RULE_PropertyToBitSet(square);
		}
	}
	while(square != 0);

	va_end(list);

return (property_set);
}


/******************************************************************/
/*					AI_Housing_Shortage							  */
/*																  */
/*   Input: RULE_GameStatePointer  game_state					  */
/*			unsigned char critical_level   How many houses should */
/*										   we panic at?           */
/*	 Output: BOOL  - True if number of houses available is        */
/*			 equal to or less than CRITICAL_LEVEL. FALSE 		  */
/*			 otherwise.											  */
/******************************************************************/


BOOL	AI_Housing_Shortage(RULE_GameStatePointer game_state, unsigned char critical_level)
{
	if(AI_Free_Houses(game_state) <= critical_level)
		return (TRUE);

return (FALSE);
}


/******************************************************************/
/*					AI_Free_Houses  							  */
/*																  */
/*   Input: RULE_GameStatePointer  game_state					  */
/*	 Output: unsigned char -  # of houses available to buy        */
/******************************************************************/


unsigned char  AI_Free_Houses(RULE_GameStatePointer game_state)
{
	RULE_SquareType SquareNo;
	unsigned char total_used_houses = 0, free_houses;


	for(SquareNo = 0; SquareNo < SQ_IN_JAIL;SquareNo++)
	{
		if(game_state->Squares[SquareNo].houses < game_state->GameOptions.housesPerHotel)
			total_used_houses += game_state->Squares[SquareNo].houses;
	}

    if (game_state->GameOptions.maximumHouses < total_used_houses)
        free_houses = 0; /* Probably a hotel decomposition is going on, with phantom houses. */
    else
        free_houses = game_state->GameOptions.maximumHouses - total_used_houses;

return (free_houses);
}

/******************************************************************/
/*					AI_Free_Hotels  							  */
/*																  */
/*   Input: RULE_GameStatePointer  game_state					  */
/*	 Output: unsigned char -  # of hotels available to buy        */
/******************************************************************/


unsigned char  AI_Free_Hotels(RULE_GameStatePointer game_state)
{
	RULE_SquareType SquareNo;
	unsigned char total_used_hotels = 0, free_hotels;


	for(SquareNo = 0; SquareNo < SQ_IN_JAIL;SquareNo++)
	{
		if(game_state->Squares[SquareNo].houses >= game_state->GameOptions.housesPerHotel)
			total_used_hotels++;
	}

	free_hotels = game_state->GameOptions.maximumHotels - total_used_hotels;

return (free_hotels);
}

/******************************************************************/
/*					AI_Own_Monopoly  							  */
/*																  */
/*   Input: RULE_GameStatePointer  game_state					  */
/*			RULE_PlayerNumber	player		Check who owns it?    */
/*			RULE_SquareType     Square		Check monopoly group  */
/*									    belonging to this square  */
/*			BOOL mortgage	  Take mortgaged lots into account?   */
/*          RULE_PropertySet properties_owned  Properties owned   */
/*                           by player.                           */
/*	 Output: BOOL  - True if 'player' owns the monopoly which     */
/*			 has square 'Square'.  False otherwise.				  */
/*										               			  */
/*																  */
/*		Returns TRUE or FALSE depending on whether 'player' owns  */
/*  the monopoly which contains the square 'Square'. If mortgage  */
/*  is TRUE, will return FALSE if one of the monopoly properties  */
/*  is mortgaged.												  */
/******************************************************************/

BOOL AI_Own_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareType Square, BOOL mortgage,
                     RULE_PropertySet properties_owned)
{
	RULE_SquareType		SquareNo, end_square;
	RULE_SquareGroups	group = RULE_SquarePredefinedInfo[Square].group;
	BOOL				owned = TRUE;
    RULE_PropertySet    monopoly_set;

	if(group == SG_RAILROAD || group == SG_UTILITY)	/* Don't handle railroads or utils. */
		return FALSE;

    if(!mortgage)
    {
        monopoly_set = AI_Get_Monopoly_Set(Square);
        if((properties_owned & monopoly_set) == monopoly_set)
            return TRUE;
        else
            return FALSE;
    }

    SquareNo = AI_Group_Start(group);
    end_square = AI_Group_End(group);

	for(; SquareNo < end_square; SquareNo++)
	{
		if(RULE_SquarePredefinedInfo[SquareNo].group == group)
		{
			if((RULE_PropertyToBitSet(SquareNo) & properties_owned) == 0)
				/*  Is this a group square and we do not own it?!!! */
			{
				owned = FALSE;		/* Set return code to false, and finish. */
				break;
			}

			if(game_state->Squares[SquareNo].mortgaged)
			{	/* Is the square mortgaged? */
				owned = FALSE;
				break;
			}
		}
	}

return owned;
}


/******************************************************************/
/*					AI_Number_Railroads_Utils_Owned				  */
/*																  */
/*   Input: RULE_GameStatePointer  game_state					  */
/*			RULE_PlayerNumber	player		Check who owns it?    */
/*			RULE_SquareGroups   group		Check SG_RAILROAD     */
/*									    or SG_UTILITY?			  */
/*			BOOL mortgage	  Take mortgaged lots into account?   */
/*	 Output: int  - Number of Railroads or Utilities owned by     */
/*			 player. If mortgage is TRUE, will subtract number    */
/*           of mortgaged railroads/utilities.					  */
/******************************************************************/

int	AI_Number_Railroads_Utils_Owned(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareGroups group,
									BOOL mortgage, RULE_PropertySet properties_owned)
{
	RULE_SquareType		SquareNo, end_square;
	int					owned = 0;

	if((group != SG_RAILROAD) && (group != SG_UTILITY))	/* Only handle railroads or utils. */
		return 0;

    SquareNo = AI_Group_Start(group);
    end_square = AI_Group_End(group);

	for(; SquareNo < end_square; SquareNo++)
	{
		if(RULE_SquarePredefinedInfo[SquareNo].group == group)
		{
			if((RULE_PropertyToBitSet(SquareNo) & properties_owned) != 0)
			   /* We found one railroad/utility.  Add our counter if not mortgaged and
			      mortgaged state is important.  */
			{
				if( (mortgage != TRUE) || (game_state->Squares[SquareNo].mortgaged != TRUE))
					owned++;	
			}
		}
	}

return owned;
}


/******************************************************************/
/*					AI_Rent_If_Stepped_On						  */
/*																  */
/*   Input: RULE_GameStatePointer  game_state					  */
/*			RULE_SquareType     Square		Check monopoly group  */
/*									    belonging to this square  */
/*	 Output: long  - Rent owned if stepped on 'Square',           */
/*										               			  */
/*																  */
/*		Returns the rent owned for property 'Square' if stepped	  */
/*	on.  Takes into account houses, mortgaging, etc.  Uses        */
/*  average roll dice of 7 for utilities, and does not take into  */
/*  acount chance/community cards.								  */
/******************************************************************/

long	AI_Rent_If_Stepped_On(RULE_GameStatePointer game_state, RULE_SquareType Square,
                              RULE_PropertySet properties_owned)
{
	RULE_SquareInfoPointer	square_pointer = &game_state->Squares[Square];
	int  number_rail_utils;

	/*  If mortgaged, don't have to pay a cent.  */
	if( square_pointer->mortgaged)
		return(0);

	/*  If nobody owns it, won't have to pay a cent! */
	if( square_pointer->owner == RULE_NOBODY_PLAYER)
		return(0);

	/*  If owns a monopoly, return a number dependent on the number of houses on the property */
	if(AI_Own_Monopoly(game_state, square_pointer->owner, Square, game_state->GameOptions.mortgagedCountsInGroupRent,
                       properties_owned) )
	{
		if(square_pointer->houses == 0)	/* No houses?  2* the rent.  Note: if mortgaging of monopoly
										   decides whether to pay 2* rent or 1* rent, AI_Own_Monopoly
										   will return TRUE if no properties mortgaged, FALSE otherwise. */
		{
				return(RULE_SquarePredefinedInfo[Square].rent[0] * 2);  /* 2* the rent.  */
		}

		return( RULE_SquarePredefinedInfo[Square].rent[square_pointer->houses] );
	}

	/*  If a utility or railroad, calculate depending on # owned.  */
	if( (number_rail_utils = AI_Number_Railroads_Utils_Owned(game_state,square_pointer->owner,
		                                                    RULE_SquarePredefinedInfo[Square].group,
															game_state->GameOptions.mortgagedCountsInGroupRent,
                                                            properties_owned)) > 0)
	{
		if(RULE_SquarePredefinedInfo[Square].group == SG_UTILITY)
		{		/*  Utility?  Then return an average rent based on a roll of 7. * 4 for 1, *10 for 2. */
			return( number_rail_utils == 2 ? 70:28);
		}
		else
		{		/* Ah! Railroads.  Then return the rent in RULE_SquarePredefinedInfo */
			return( RULE_SquarePredefinedInfo[Square].rent[number_rail_utils] );	
		}
	}

	/* Otherwise, return regular rent.  */
return(RULE_SquarePredefinedInfo[Square].rent[0]);
}


/******************************************************************/
/*					AI_Average_Rent_Received					  */
/*																  */
/*   Input: RULE_GameStatePointer  game_state					  */
/*			RULE_PlayerNumber	player		Rent for who?         */
/*			RULE_SquareType     SquareNo  Start at which square,  */
/*									    calculating to end (Go)   */
/*			BOOL dont_count_developed_monopolies				  */
/*			float cash_cow_multiplier  How much should cash cow   */
/*									value be multiplied by?       */
/*			RULE_PropertySet  properties_owned					  */
/*	 Output: float  - Average rent per player 'player' should     */
/*			          expect.									  */
/*										               			  */
/*   Calculates the average rent per player per cycle 'player'    */
/*   should receive from all owned properties.  SquareNo is the   */
/*   square the calculation should begin, finishing at GO (hence  */
/*   ignoring all properties before SquareNo.).  For entire       */
/*   board, set SquareNo to 0.  If count_developed_monopolies is  */
/*   TRUE, will not take into account monopolies with houses on   */
/*   them.														  */
/******************************************************************/


float AI_Average_Rent_Received(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareType SquareNo,
							   BOOL dont_count_developed_monopolies, float cash_cow_multiplier, RULE_PropertySet properties_owned)
{
	float total_rent = (float) 0.0;

	if(SquareNo < 0)	/* Invalid number. */
		return (float) 0.0;

		/* Start at SquareNo passed.  Go until end.  */
	for(; SquareNo < SQ_IN_JAIL; SquareNo++)
	{
		if((RULE_PropertyToBitSet(SquareNo) & properties_owned) != 0)   /* Do we own it? */
		{
			/*  Should we be skipping over developed moopolies? */
			if(dont_count_developed_monopolies)
			{	
				if(AI_Num_Houses_On_Monopoly(game_state,SquareNo) > 0)	
					continue;	/* Housed?  skip it then. */
			}
			
			/* Should we add cash cows to the calculation? */
			/*  add the rent to our total, multiplied by landing frequency rent modifier.          */
			if(AI_Is_Cash_Cow(SquareNo))
				total_rent += (float) (AI_Rent_If_Stepped_On(game_state, SquareNo, properties_owned) *
							  AI_Property_Landing_Frequency[SquareNo]) * cash_cow_multiplier;
			else
				total_rent += (float) (AI_Rent_If_Stepped_On(game_state, SquareNo, properties_owned) *
							  AI_Property_Landing_Frequency[SquareNo]);
		}
	}

	/*  We want an average rent.  On average, if rolled 7, 1/7 chance of landing on any property,
	    since landing frequency already taken into account.  */
	total_rent = total_rent / 7;

return total_rent;
}

/******************************************************************/
/*					AI_Average_Rent_Payed						  */
/*																  */
/*   Input: RULE_GameStatePointer  game_state					  */
/*			RULE_PlayerNumber	player		Who is paying?        */
/*			RULE_SquareType     SquareNo  Start at which square,  */
/*									    calculating to end (Go)   */
/*			BOOL consider_developed_monopolies					  */
/*			float cash_cow_multiplier  Cash cows worth how much?  */
/*	 Output: float  - Average rent payed per cycle, including     */
/*			          tax squares.								  */
/*										               			  */
/*   Calculates the average rent player will pay er cycle. SquarNo*/
/*   is the square the calculation should begin, finishing at GO  */
/*   (hence ignoring all properties before SquareNo.). For entire */
/*   board, set SquareNo to 0.									  */
/******************************************************************/

long AI_Average_Rent_Payed(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareType SquareNo,
						   BOOL consider_developed_monopolies, float cash_cow_multiplier)
{
	float   total = (float) 0.0;
	long	worth;
	RULE_PlayerNumber counter;
    RULE_PropertySet    properties_owned;

	/*  First sum up all the non-developed lots between ourselves and GO  */
	for(counter = 0; counter < game_state->NumberOfPlayers; ++counter)
	{
		if(counter == player)  continue;

		if(game_state->Players[counter].currentSquare != SQ_OFF_BOARD)
        {
            properties_owned = RULE_PropertySetOwnedByPlayer(game_state, counter);
			total += AI_Average_Rent_Received(game_state, counter, SquareNo,
			                                   !consider_developed_monopolies, cash_cow_multiplier,
                                               properties_owned);
        }
	}

	/*  Now add up tax squares if we need to. */
	if(SQ_INCOME_TAX >= SquareNo)  /* Is the income tax square before or after go?  */
	{	/* Before.  need to take it into account. */

		worth = AI_Get_Total_Worth(game_state, player);

		if(game_state->GameOptions.luxuryTaxAmount > 0)
		{
			if(worth / ((float) game_state->GameOptions.luxuryTaxAmount)  > game_state->GameOptions.flatTaxFee)
				total += (float)( game_state->GameOptions.flatTaxFee / 7.0);
			else
				total += (float) (worth / ((float) game_state->GameOptions.luxuryTaxAmount) / 7.0);
		}
	}

	if(SQ_LUXURY_TAX >= SquareNo)
	{
		total += (float) (game_state->GameOptions.luxuryTaxAmount / 7.0);	
	}



return ((long) total);	
}


/******************************************************************/
/*					AI_Get_Most_Liquid_Assets					  */
/*																  */
/*   Input: RULE_GameStatePointer  game_state					  */
/*			RULE_PlayerNumber	player		Who not to count?     */
/*			BOOL		   count_monopolies	 Count monopolies?    */
/*	 Output: long   -  the largest amount of liquid assets        */
/*			          oppenents have.							  */
/*										               			  */
/*		Figures out how much liquid assets is available to each   */
/*  opponent and returns the largest amount.					  */
/******************************************************************/

long  AI_Get_Most_Liquid_Assets(RULE_GameStatePointer  game_state, RULE_PlayerNumber player, BOOL count_monopolies)
{
	RULE_PlayerNumber counter;
	long			  most_cash = 0, current_cash;

	for(counter = 0; counter < game_state->NumberOfPlayers; ++counter)
	{
		if(counter == player)  continue;
	
		if(game_state->Players[counter].currentSquare != SQ_OFF_BOARD)
		{
			current_cash = AI_Get_Liquid_Assets(game_state, counter, count_monopolies, count_monopolies);
			if(current_cash > most_cash)
				most_cash = current_cash;
		}
	}

return (most_cash);
}

/******************************************************************/
/*					AI_Is_Cash_Cow          					  */
/*																  */
/*   Input: RULE_SquareType     property						  */
/*	 Output: BOOL  -  True if property is a utility or railroad,  */
/*			          false otherwise							  */
/******************************************************************/

BOOL AI_Is_Cash_Cow(RULE_SquareType property)
{
	RULE_SquareGroups	group = RULE_SquarePredefinedInfo[property].group;

	if((group == SG_UTILITY) || (group == SG_RAILROAD))	/* A utility or railroad? */
		return (TRUE);		/* Yup.  It's a cash cow. */

return(FALSE);
}


/************************************************************************************/
/*					AI_Min_Calculated_Expenses										*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*			RULE_PlayerNumber	player		Who'se expenses?						*/
/*	 Ouput: long   - minimum expenses												*/
/*																					*/
/*		Calculates the expected expenses from the player's position to go.  Will not*/
/*   take into account developed monopolies.										*/
/************************************************************************************/

long  AI_Min_Calculated_Expenses(RULE_GameStatePointer game_state, RULE_PlayerNumber player)
{
	RULE_SquareType	cur_square;
	long	average_rent_paid;

	cur_square = game_state->Players[player].currentSquare;
	if(cur_square == SQ_IN_JAIL)
		cur_square = SQ_JUST_VISITING;

	if(cur_square > SQ_BOARDWALK)
		return (-1);		/* Not on the board as we know it! */

	/*  What is our average rent from all players and squares, not taking into
	    account developed monopolies? */
	average_rent_paid = AI_Average_Rent_Payed(game_state, player, cur_square, FALSE, 1.0f);


return average_rent_paid;
}



/******************************************************************/
/*			AI_Only_Player_Have_Monopoly						  */
/*																  */
/*   Input: RULE_GameStatePointer   game_state					  */
/*			RULE_PlayerNumber		player    Who owns monopoly?  */
/*   Output:  BOOL - TRUE if 'player' is the only player who	  */
/*                   owns a monopoly, and the rest of the players */
/*					 cannot trade for one.  False otherwise.	  */
/******************************************************************/

BOOL AI_Only_Player_Have_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player)
{
	RULE_PlayerNumber	cur_player;
	BOOL			    return_value = FALSE;
	int					index, num_players_in_list = 0;
	RULE_PlayerNumber	player_list[RULE_MAX_PLAYERS + 1];
	RULE_PropertySet	property_set, properties[RULE_MAX_PLAYERS + 1];
	RULE_SquareType		cur_square;

	/* go through all players */
	for(cur_player = 0; cur_player < game_state->NumberOfPlayers; ++cur_player)
	{
		/* Does this player own a monopoly? */
		if(AI_Player_Own_Monopoly(game_state, cur_player, FALSE))
		{
			/* Yes.  Is this player us? */
			if(cur_player == player)
				return_value = TRUE;
			else
			{
				return_value = FALSE;
				break;
			}
		}
		else	/* No - doesn't own a monopoly. If it is us, return FALSE. */
		{	
			if(cur_player == player)
			{
				return_value = FALSE;
				break;
			}
		}
	}

	if(return_value == TRUE)
	{	/* No one owns a monopoly but us.  But can they trade for a monopoly? 
		   Prepare property array. */
		for(index = 0; index < game_state->NumberOfPlayers; ++index)
		{
			properties[index] = RULE_PropertySetOwnedByPlayer(game_state, (RULE_PlayerNumber) index);

			/* need a list of potential players we can trade with. */
			if(index == player)
				continue;

			if(game_state->Players[index].currentSquare == SQ_OFF_BOARD)
				continue;	/* gone bankrupt! */

			player_list[num_players_in_list++] = index;
		}

		properties[RULE_MAX_PLAYERS] = 0;

		/* Now check whether a monopoly trade is possible between any subset of all player except
		   us. */
		for(cur_square = 0; cur_square < SG_PARK_PLACE + 1; ++cur_square)
		{
			property_set = AI_Get_Monopoly_Set(AI_Expensive_Monopoly_Square[cur_square]);
	
			if(AI_Find_Smallest_Monopoly_Trade(RULE_MAX_PLAYERS, property_set, player_list, 
											   num_players_in_list, properties))
				return FALSE;	/* Opponents can trade for a monopoly. */
		}
	}

return (return_value);
}

/******************************************************************/
/*			AI_Owns_Prop_From_Monopoly							  */
/*																  */
/*   Input: RULE_GameStatePointer   game_state					  */
/*			RULE_PlayerNumber		player   Who owns monopoly?   */
/*			RULE_SquareType 		property  What monopoly?	  */
/*   Output:  BOOL - TRUE if 'player' owns a property in the      */
/*					 monopoly containing 'property'.  FALSE       */
/*					 otherwise.									  */
/******************************************************************/

BOOL AI_Owns_Prop_From_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player, 
								 RULE_SquareType property)
{
	RULE_PropertySet owned, of_monopoly;

	owned = RULE_PropertySetOwnedByPlayer(game_state, player);
	of_monopoly = AI_Get_Monopoly_Set(property);

	if(owned & of_monopoly)
		return TRUE;

return FALSE;
}

/******************************************************************/
/*			AI_Num_Houses_Can_Buy_On_Monopoly					  */
/*																  */
/*   Input: RULE_GameStatePointer   game_state					  */
/*			RULE_SquareType 		square  Build houses where?   */
/*   Output:  long - # of houses that still can be bough on       */
/*                   monopoly before maximum number is hit.		  */
/******************************************************************/

long AI_Num_House_Can_Buy_On_Monopoly(RULE_GameStatePointer game_state, RULE_SquareType square)
{
	long num_squares;

	num_squares = AI_Number_Properties_In_Set(AI_Get_Monopoly_Set(square));

	num_squares = (game_state->GameOptions.housesPerHotel) * num_squares -
					AI_Num_Houses_On_Monopoly(game_state, square);

return num_squares;
}

/******************************************************************/
/*					AI_Test_For_Monopoly    					  */
/*																  */
/*   Input: RULE_PropertySet    properties_owned			      */
/*			RULE_SquareType 	Square						      */
/*	 Output: BOOL   -  TRUE if the entire monopoly containing	  */
/*					   'Square'  is in properties_owned.		  */
/*										               			  */
/*		Used to determine whether someone owns a monopoly.        */
/******************************************************************/

BOOL	AI_Test_For_Monopoly(RULE_PropertySet properties_owned, RULE_SquareType Square)
{
	RULE_SquareType		SquareNo, end_square;
	RULE_SquareGroups	group = RULE_SquarePredefinedInfo[Square].group;

    SquareNo = AI_Group_Start(group);
    end_square = AI_Group_End(group);

	for(; SquareNo < end_square; SquareNo++)
	{
		if(RULE_SquarePredefinedInfo[SquareNo].group == group)
		{
			if((RULE_PropertyToBitSet(SquareNo) & properties_owned) == 0)
				return FALSE;
		}
	}

return TRUE;
}

/************************************************************************************/
/*					AI_Who_Owns_Monopoly											*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*			RULE_SquareType        Square       Which monopoly?						*/
/*	 Output: RULE_PlayerNumber  -  First player found owning the monopoly.			*/
/*						       RULE_NOBODY_PLAYER if no square of monopoly is owned.*/
/*										               								*/
/*		This function will return the player who owns a monopoly.  It searchs the   */
/*  monopoly associated with Square for a non RULE_NOBODY_PLAYER and returns the    */
/*  first one found.  If no player owns a square on the monopoly, returns           */
/*  RULE_NOBODY_PLAYER.
/************************************************************************************/

RULE_PlayerNumber  AI_Who_Owns_Monopoly(RULE_GameStatePointer game_state, RULE_SquareType Square)
{
	RULE_SquareType SquareNo, end_square;
	RULE_SquareGroups	group = RULE_SquarePredefinedInfo[Square].group;

    SquareNo = AI_Group_Start(group);
    end_square = AI_Group_End(group);

	for(; SquareNo < end_square; SquareNo++)
	{
		if(RULE_SquarePredefinedInfo[SquareNo].group == group)
		{
			if(game_state->Squares[SquareNo].owner != RULE_NOBODY_PLAYER)
				return (game_state->Squares[SquareNo].owner);
		}
	}


return (RULE_NOBODY_PLAYER);
}

/************************************************************************************/
/*					AI_Cash_Available_After_Housing									*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*	        RULE_PlayerNumber player  Whose cash are we interested in?				*/
/*	 Ouput: long	 -  Cash left after purchasing all the houses we want.			*/
/*										               								*/
/*		This functions returns the amount of cash left if player was to unmortgage  */
/*  all his monopolies and buy all the houses he needs to.  If very few houses      */
/*  remain, assumes no houses will be bought, but that the monopolies will be   	*/
/*  unmortgaged.																	*/
/************************************************************************************/

long	AI_Cash_Available_After_Housing(RULE_GameStatePointer game_state, RULE_PlayerNumber player)
{
	RULE_GameStateRecord	new_state;
    BOOL                    housing_shortage;

	/* Create a copy of game_state.  */
	memcpy(&new_state, game_state, sizeof(RULE_GameStateRecord));

    housing_shortage = AI_Housing_Shortage(game_state, AI_CRITICAL_HOUSING_LEVEL);
	/* First, need to unmortgage all the properties that give monopolies and
	   build them up one at a time.  */
	do
	{
		/* First, buy houses. */
		if(!housing_shortage)
		{
			/* Keep buying houses as long as we can.  Note: we assume enough houses
			exist for each player to buy houses for their monopolies */
			while(AI_Hypothetical_Buy_Houses(&new_state, player) )  ;
		}		
			/* Keep unmortgaging monopolies. */
	} while(AI_Unmortgage_Property(&new_state, player, TRUE, TRUE) != 0);


	/* How much cash do we have left? */
return 	AI_Excess_Cash_Available(&new_state, player, FALSE);
}

/****************************************************/
/*				AI_Num_Properties_Left_To_Buy		*/
/*													*/
/*   Input: RULE_GameStatePointer  game_state		*/
/*	 Ouput: int - # of monopoly properties not owned*/
/****************************************************/

int	AI_Num_Properties_Left_To_Buy(RULE_GameStatePointer game_state)
{
	RULE_SquareType current_square;
	int				squares_unowned = 0;

	/*  Count how many squares are not owned. */
	for(current_square = 0; current_square < SQ_IN_JAIL; ++current_square)
	{
		if(game_state->Squares[current_square].owner == RULE_NOBODY_PLAYER)
		{
			/* Is this a monopoly property one can own? */
			if(RULE_SquarePredefinedInfo[current_square].group <= SG_PARK_PLACE)
				squares_unowned++;
		}
	}

return squares_unowned;
}


/************************************************************************************/
/*					AI_Property_Being_Bought       									*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*		    RULE_PlayerNumber	   *player      Store the player buying.			*/
/*	 Output:  RULE_SquareType - the property currently being bought.				*
/*			                     SQ_MAX_SQUARE_TYPES if none.						*/
/*																					*/
/*		This functions determines whether a player is currently choosing to buy a   */
/*  property.  If yes, returns this property and set *player to that player.        */
/*  Used to make sure a human player cannot land on a property, and before buying   */
/*  it, fool the AI into giving it all the other properties to the monopoly.		*/
/************************************************************************************/

RULE_SquareType		AI_Property_Being_Bought(RULE_GameStatePointer game_state, RULE_PlayerNumber *player)
{
	if(AI_Glob.Player_Purchasing_Property != RULE_NOBODY_PLAYER)
	{	/* Someone purchasing a property. */
		*player = AI_Glob.Player_Purchasing_Property;
		return (AI_Glob.Purchasing_Property);
	}

return SQ_MAX_SQUARE_TYPES;
}


/******************************************************************************/
/*					AI_Get_Monopoly_Stage									  */
/*																			  */
/*   Input: RULE_GameStatePointer	game_state								  */
/*			RULE_PlayerNumber		player		Stage depends on player.	  */
/*	 Output: AI_Monopoly_Stage  - 											  */
/*	  AI_STAGE_BUYING: Lots of properties to buy, and no one owns a monopoly. */
/*	  AI_STAGE_NO_MONOPOLIES:  Almost all property bought, and no one owns a  */
/*							   monopoly.									  */
/*	  AI_STAGE_MONOPOLIES_NOT_OWN_ONE:  A monopoly exists, but the player	  */
/*									  asking doesn't have one.				  */
/*	  AI_STAGE_MONOPOLIES_OWN_ONE:  Monopolies exist, and the player asking   */
/*								    has one.								  */
/*																			  */
/*		Figures out what stage is the game at, and hence what strategy the    */
/*  AI should follow.														  */
/******************************************************************************/

AI_Monopoly_Stage	AI_Get_Monopoly_Stage(RULE_GameStatePointer game_state, RULE_PlayerNumber player)
{
	RULE_SquareType		temp[SG_PARK_PLACE + 1];
	int					squares_unowned = 0;

	if(AI_Exist_Monopoly(game_state))
	{	/* A monopoly exists. Does the player own one? */
        if(AI_Get_Monopolies(game_state, player, temp, 0))
			return (AI_STAGE_MONOPOLIES_OWN_ONE);

		return (AI_STAGE_MONOPOLIES_NOT_OWN_ONE);
	}

	squares_unowned = AI_Num_Properties_Left_To_Buy(game_state);

	if(squares_unowned <= AI_CRITICAL_UNOWNED_SQUARES)
		return (AI_STAGE_NO_MONOPOLIES);	/* Few squares left to buy. Almost static stage. */

return (AI_STAGE_BUYING);
}


/************************************************************************************/
/*					AI_Find_Lowest_Rent       										*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*			RULE_PlayerNumber	player		Lowest rent for who?    				*/
/*			RULE_PropertySet properties		List of properties to consider			*/
/*											mortgaging							    */
/*	 Output:  RULE_SquareType	  The best property to sell to keep rent high       */
/*																				    */
/*			Finds the best property, out of a list in 'properties', to mortgage/sell*/
/*   to keep average rent income highest.  If property has houses, will sell one    */
/*   house from the monopoly.  If a housing shortage exists, and must sell a hotel, */
/*   will sell as many houses as needed from the monopoly.							*/
/************************************************************************************/


RULE_SquareType	AI_Find_Lowest_Rent(RULE_GameStatePointer game_state,RULE_PlayerNumber player, RULE_PropertySet properties)
{
	RULE_SquareType	Lowest_property = 0;
	float	highest_rent = (float) -1.0, current_rent;
	RULE_SquareType		SquareNo,  monopoly_squares[AI_MAX_MONOPOLY_LOTS + 1];	
	int					squares_in_monopoly, index;
	RULE_GameStateRecord		new_state;
	RULE_SquareInfoRecord	old_squares[AI_MAX_MONOPOLY_LOTS + 1], *current_square;
    RULE_PropertySet    properties_owned = RULE_PropertySetOwnedByPlayer(game_state, player);
	

	memcpy(&new_state,game_state, sizeof(RULE_GameStateRecord) );

	for(SquareNo = 0; SquareNo < SQ_IN_JAIL;SquareNo++)
	{
		if(RULE_PropertyToBitSet(SquareNo) & properties)	/* Is it part of properties?  */
		{
			current_square = &new_state.Squares[SquareNo];
	
			if(current_square->mortgaged)
				continue;	/* Mortgaged?  Skip it.  Can't mortgage it.  */

			/* Mortgage/sell house  */
			if(AI_Num_Houses_On_Monopoly(&new_state, SquareNo) > 0)
			{
				squares_in_monopoly = AI_Get_Monopoly_Lots(&new_state,monopoly_squares, SquareNo);

				/* Save the current square information */
				for(index = 0; index < squares_in_monopoly; ++index)	
					memcpy(old_squares + index, &new_state.Squares[monopoly_squares[index]], sizeof(RULE_SquareInfoRecord) );

				AI_Test_Sell_Houses(&new_state, monopoly_squares, squares_in_monopoly, SquareNo);
			}
			else
			{
				squares_in_monopoly = 1;
				monopoly_squares[0] = SquareNo;
				/* Save the current square information */
				memcpy(old_squares, current_square, sizeof(RULE_SquareInfoRecord) );

				current_square->mortgaged = TRUE;
			}

			/*  Find the new avg. rent per cycle.  */
			current_rent =  AI_Average_Rent_Received(&new_state, player, 0, FALSE, 1.0f, 
                                                     properties_owned);

			if((highest_rent == -1) ||  (highest_rent < current_rent) )
			{
				highest_rent = current_rent;
				Lowest_property = SquareNo;
			}

			/* restore the current square information */
			for(index = 0; index < squares_in_monopoly; ++index)	
				memcpy( &new_state.Squares[monopoly_squares[index]], old_squares + index, sizeof(RULE_SquareInfoRecord) );

		}  /* if */
	}  /* for */


return(Lowest_property);
}


/************************************************************************************/
/*					AI_Find_Highest_Rent_Mortgaged									*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*			RULE_PlayerNumber	player		Lowest rent for who?    				*/
/*			RULE_PropertySet properties		List of properties to consider			*/
/*											unmortgaging							*/
/*			long   top_unmortgage_cost		How much money do we have to unmortgage?*/
/*	 Output:  RULE_SquareType	  The best property to unmortgage. 0 if can't find  */
/*								  one within our price range.						*/
/*																				    */
/*			Finds the best property, out of a list in 'properties', to unmortgage   */
/*   to keep average rent income highest.											*/
/************************************************************************************/


RULE_SquareType	AI_Find_Highest_Rent_Mortgaged(RULE_GameStatePointer game_state,RULE_PlayerNumber player, RULE_PropertySet properties,
											   long top_unmortgage_cost)
{
	RULE_SquareType	Highest_property = 0;
	float	highest_rent = (float) -1.0, current_rent;
	RULE_SquareType		SquareNo;
	RULE_SquareInfoRecord	*current_square;
    RULE_PropertySet    properties_owned = RULE_PropertySetOwnedByPlayer(game_state, player);
	
	for(SquareNo = 0; SquareNo < SQ_IN_JAIL;SquareNo++)
	{
		if(RULE_PropertyToBitSet(SquareNo) & properties)	/* Is it part of properties?  */
		{
			current_square = &game_state->Squares[SquareNo];
	
			if(!current_square->mortgaged)
				continue;	/* Not mortgaged?  Skip it.  Can't unmortgage it.  */

			if((RULE_SquarePredefinedInfo[SquareNo].mortgageCost * 1.1) > top_unmortgage_cost)
				continue;	/* Can't afford to unmortgage it. */

			current_square->mortgaged = FALSE;

			/*  Find the new avg. rent per cycle.  */
			current_rent =  AI_Average_Rent_Received(game_state, player, 0, FALSE, 1.0f,
                                                     properties_owned);

			if((highest_rent == -1) ||  (highest_rent < current_rent) )
			{
				highest_rent = current_rent;
				Highest_property = SquareNo;
			}

			current_square->mortgaged = TRUE;
		}  /* if */
	}  /* for */


return(Highest_property);
}


/************************************************************************************/
/*					AI_Square_Move_Reaction											*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*			RULE_PlayerNumber	   player		Who moved?							*/
/*			RULE_SquareType        land_square	Where did player land?				*/
/*			long				   misc         Used to send RULE_CardType when     */
/*												stepped on community or chance.     */
/*	 Output:  AI_Token_Reactions - a number, 0,1,2,3, depending on whether player   */
/*				is happy,sad, neutral or angry to have stepped on from_square.		*/
/*																				    */
/*		This function determines the reaction of player to having stepped on        */
/*  from_square.  If player stepepd on a chance or community square, misc should    */
/*  have the card player picked up.  To set when a player gets happy, sad, angry,   */
/*  change AI_ANGRY_REACTION_RENT, AI_SAD_REACTION_RENT, and AI_HAPPY_REACTION_RENT */
/*  in ai_util.h.																	*/
/************************************************************************************/

AI_Token_Reactions AI_Square_Move_Reaction(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
								  		   RULE_SquareType land_square, long misc)
{
	RULE_SquareInfoRecord  *square;
	long				   rent_payed, num_monopolies, counter, total_houses;
	RULE_PropertySet	   properties_owned = RULE_PropertySetOwnedByPlayer(game_state, player);
	RULE_SquareType		   store_squares[SG_PARK_PLACE + 1];

	square = &game_state->Squares[land_square];

	// Is this a special square requiring special consideration? 
	switch(land_square)
	{
		case SQ_COMMUNITY_CHEST_1:
		case SQ_COMMUNITY_CHEST_2:
		case SQ_COMMUNITY_CHEST_3:
		case SQ_CHANCE_1:
		case SQ_CHANCE_2:
		case SQ_CHANCE_3:
		case SQ_GO:
			// Consider the non-neutral cards.
			switch((RULE_CardType) misc)
			{
				case CHANCE_GET_150_FROM_BANK:
					if(AI_HAPPY_REACTION_RENT <= 150)
						return AI_REACTION_HAPPY;
					return AI_REACTION_NEUTRAL;
											
				case CHANCE_GO_TO_BOARDWALK:
					return AI_Square_Move_Reaction(game_state, player, SQ_BOARDWALK, 0);

				case CHANCE_GO_TO_ST_CHARLES_PLACE:
					return AI_Square_Move_Reaction(game_state, player, SQ_ST_CHARLES_PLACE, 0);

				case CHANCE_PAY_25_EACH_HOUSE_100_EACH_HOTEL:
				case COMMUNITY_PAY_40_EACH_HOUSE_115_EACH_HOTEL:
					// Count how many houses we have.  
					num_monopolies = AI_Get_Monopolies(game_state, player, store_squares, TRUE);
					for(counter = 0, total_houses = 0; counter < num_monopolies; ++counter)
						total_houses += AI_Num_Houses_On_Monopoly(game_state, store_squares[counter]);

					if(misc == CHANCE_PAY_25_EACH_HOUSE_100_EACH_HOTEL)
						rent_payed = total_houses * 25;		// rough idea, though not completely correct.  However, this
					else									// is wrong when we hit AI_ANGRY_REACTION_RENT or so.
						rent_payed = total_houses * 40;

					if(rent_payed >= AI_ANGRY_REACTION_RENT)
						return AI_REACTION_ANGRY;

					if(rent_payed >= AI_SAD_REACTION_RENT)
						return AI_REACTION_SAD;

					return AI_REACTION_NEUTRAL;

				case CHANCE_GO_TO_ILLINOIS_AVENUE:
					return AI_Square_Move_Reaction(game_state, player, SQ_ILLINOIS_AVENUE, 0);

				case CHANCE_GO_DIRECTLY_TO_JAIL:
				case COMMUNITY_GO_DIRECTLY_TO_JAIL:
					// Check if our average rent payed is large or not.
					rent_payed = AI_Average_Rent_Payed(game_state, player, 0, TRUE, 1.0f);
					if(rent_payed > AI_REACTION_ANGRY)
						return AI_REACTION_HAPPY;

					return AI_REACTION_NEUTRAL;

				case CHANCE_GO_BACK_THREE_SPACES:
					// Check if we are stepping back onto SQ_COMMUNITY_CHEST_3.  If yes,
					// return neutral;
					land_square -= 3;
					if(land_square == SQ_COMMUNITY_CHEST_3)
						return AI_REACTION_NEUTRAL;

					return AI_Square_Move_Reaction(game_state, player, land_square, 0);

				case COMMUNITY_GET_100_FROM_BANK_1:
				case COMMUNITY_GET_100_FROM_BANK_2:
				case COMMUNITY_GET_100_FROM_BANK_3:
					if(AI_HAPPY_REACTION_RENT <= 100)
						return AI_REACTION_HAPPY;
					return AI_REACTION_NEUTRAL;

				case COMMUNITY_GET_200_FROM_BANK:
					if(AI_HAPPY_REACTION_RENT <= 200)
						return AI_REACTION_HAPPY;
					return AI_REACTION_NEUTRAL;

				case COMMUNITY_PAY_150_TO_BANK:
					if(AI_ANGRY_REACTION_RENT <= 150)
						return AI_REACTION_ANGRY;

					if(AI_SAD_REACTION_RENT <= 150)
						return AI_REACTION_SAD;
					return AI_REACTION_NEUTRAL;

				case COMMUNITY_PAY_100_TO_BANK:
					if(AI_SAD_REACTION_RENT <= 100)
						return AI_REACTION_SAD;
					return AI_REACTION_NEUTRAL;
			}

			// Not a special card - then return neutral.
			return AI_REACTION_NEUTRAL;
	}

	// Then base our result on the amount of cash we're going to pay - below 100 is neutral,
	// above 100 is sad, above 500 is angry, and happy if we are receiving cash above 250.

	// What is the rent we are paying?
	rent_payed = AI_Rent_If_Stepped_On(game_state, land_square, properties_owned);

	// Check if the square we are stepping on is owned by us.
	if(square->owner == player)
		return AI_REACTION_NEUTRAL;

	if(rent_payed >= AI_ANGRY_REACTION_RENT)
		return AI_REACTION_ANGRY;

	if(rent_payed >= AI_SAD_REACTION_RENT)
		return AI_REACTION_SAD;


return AI_REACTION_NEUTRAL;
}
