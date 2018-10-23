/*************************************************************
*
*   FILE:             AI_TRADE.C
*
*   (C) Copyright 97  Artech Digital Entertainments.
*                     All rights reserved.
*************************************************************/

#include "gameinc.h"

float AI_TEST_chances_change, AI_TEST_worth_change, AI_TEST_prop_importance, AI_TEST_evaluation;


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

	/* The trade list - keeps track of what is being offered, and what is
	   being considered to be offered. */
AI_Trade_List_Record  Current_Trade_List[RULE_MAX_PLAYERS], Proposing_trade_list[RULE_MAX_PLAYERS];
	/* The following keeps track of the immunities and futures traded.  */
AI_Immunity_Record   AI_Immunities_Futures_Traded[RULE_MAX_COUNT_HIT_SETS];

	/* The following keeps track of whether an AI is in the process of 
	   sending a trade. */
AI_Sending_Trade_State  AI_PLAYER_SENDING_TRADE = AI_Send_Nothing;	
BOOL                    AI_SEND_TRADE_OFFER_TRADE = FALSE;

/****************************************************************************************/
/*					AI_Propose_Trade													*/
/*																						*/
/*   Input:  RULE_GameStatePointer	game_state											*/
/*			 RULE_PlayerNumber		player		Who is proposing?						*/
/*			 AI_Trade_Importance	importance	How important is this trade?			*/
/*		     RULE_PlayerNumber      bad_player	Who not to trade with.					*/
/*   Output: BOOL - TRUE if a trade has been proposed, FALSE otherwise.					*/
/*																						*/
/*		Proposes a trade for 'player' that exclused bad_player.  The higher the         */
/*  importance, the better the deal proposed.  If importance is set to AI_TRADE_FOR_CASH*/
/*  will try to sell off property for cash.  Will determine whether it is the right     */
/*  time to trade for a monopoly.  If so, will try to get the optimal monopoly, giving  */
/*  away as few as possible (ie. trading with the fewest number of players.).           */
/****************************************************************************************/

BOOL AI_Propose_Trade(RULE_GameStatePointer game_state, RULE_PlayerNumber player, AI_Trade_Importance importance, RULE_PlayerNumber bad_player)
{
	RULE_SquareGroups	monopoly_group[SG_PARK_PLACE + 1];
	int		trade_spot, counter;
	long	liquid_assets;
	int		end_monopoly;
	RULE_PlayerNumber	trade_player = RULE_NOBODY_PLAYER, cur_player;
	RULE_SquareGroups	monopoly_to_trade = SG_MAX_PROPERTY_GROUPS; /* flag - we haven't found one. */
	RULE_PropertySet	properties[RULE_MAX_PLAYERS];
	RULE_PlayerNumber	player_list[RULE_MAX_PLAYERS], backup_list[RULE_MAX_PLAYERS];
	int					num_players_in_list = 0, smallest_trade, ai_level;

#ifdef AI_NO_TRADES
	return FALSE;
#endif 

	/* First check if we can make another trade. */
	trade_spot = AI_Find_Free_Trade_Spot(game_state, player);
	if(trade_spot == -1)
		return FALSE;	/* Can't make a trade - too many trades made already. */

	/* See whether we should be making a monopoly trade.  Figure out which
	   monopoly we want based on our liquid assets, and see whether we can
	   trade for it.  If can't, go down the list until we find a monopoly we can. */

		/* First, rank the monopolies based on our liquid assets. */
    liquid_assets = AI_Get_Liquid_Assets(game_state, player, FALSE, FALSE);
	ai_level = game_state->Players[player].AIPlayerLevel;
    AI_Order_Monopoly_Importance(liquid_assets, monopoly_group, (ai_level == 3 ? AI_Sort_Descending: AI_Sort_Random));

	/* Set up property sets & player list. */
	for(counter = 0; counter < game_state->NumberOfPlayers; ++counter)
	{
		properties[counter] = RULE_PropertySetOwnedByPlayer(game_state, (RULE_PlayerNumber) counter);

		/* need a list of potential players we can trade with. */
		if(counter == player)
			continue;

		/* Be a little less picky if this is a desperate trade. */
		if(!(importance & AI_TRADE_DESPERATE))
		{
			if(AI_Player_Own_Monopoly(game_state, (RULE_PlayerNumber) counter, FALSE))
				continue; /* Owns a monopoly - little incentive to trade with us. */
	
			if(AI_Data[player].player_attitude[counter] < AI_DONT_TRADE_ATTITUDE)
				continue;  /* Hasn't traded in the past. Ignore him. */
		}

		if(counter == bad_player)
			continue; /* Not trading with this one! */

		if(game_state->Players[counter].currentSquare == SQ_OFF_BOARD)
			continue;	/* gone bankrupt! */

		player_list[num_players_in_list++] = counter;
	}

	if(num_players_in_list == 0)
		return FALSE;

	memcpy(backup_list, player_list, sizeof(RULE_PlayerNumber) * RULE_MAX_PLAYERS);

	/* How desperate are we in getting a monopoly? */
	switch(AI_Should_Trade_For_Monopoly(game_state, player))
	{
		case AI_MAYBE:
			/* We can wait, don't have to push it.  Then only check the best two monopolies. */
			end_monopoly = 2; /* check only 2 best monopolies. */
			break;

		case TRUE:
			end_monopoly = SG_PARK_PLACE; /* check all but the worst monopoly. */
			break;

		case FALSE:
			end_monopoly = 0;  /* don't want to trade for monopoly. */
			break;
	}

	/* How desperate is this trade? */
	if(importance & AI_TRADE_FOR_CASH)
		end_monopoly = 0;	/* Don't want a monopoly - want cash! */

    /* First, see if we want to GIVE a monopoly away because of our low cash with a monopoly. */
    if(AI_Should_Give_Away_Monopoly(game_state, player))
    {
        /* Start with the player who has the most liquid assets, and try to give that
           player a monopoly in return for lots of cash. */
        cur_player = AI_Find_Nonmonopoly_Player(game_state, player);

        /* Did we find a player? */
        if(cur_player != RULE_NOBODY_PLAYER)
        {   /* Then try to give a monopoly away. */
            if(AI_Give_Monopoly_For_Cash(game_state, player, cur_player, properties))
            {
                /* We did it!  Then send it. */
		        if(AI_Send_Proposed_Trade(game_state, player, FALSE))
		        {
			        AI_Data[player].time_last_trade[trade_spot] = AI_Data[player].time_forget_trade;
			        return TRUE;
		        }

                return FALSE;  /* Something wrong! Couldn't send the trade. */
            }

            return FALSE;
        }
    }

    if(!(importance & AI_TRADE_GIVE_MONOPOLY))
    {
	    /* Go through all the monopolies we should and find one we can trade for. */
	    for(counter = 0; counter < end_monopoly; ++counter)
	    {
		    /* Reset the trade list. */
		    memset(Proposing_trade_list, 0, sizeof(AI_Trade_List_Record) * RULE_MAX_PLAYERS);

		    /* Can we trade for it?  Ie: Is there a smallest number of people we
		       can setup a group trade so we get it, taking into account that they
		       might have monopolies, or have rejected too many trades of ours? */
		    memcpy(player_list, backup_list, sizeof(RULE_PlayerNumber) * RULE_MAX_PLAYERS);
		    smallest_trade = AI_Find_Smallest_Monopoly_Trade(player, AI_Get_Monopoly_Set(AI_Expensive_Monopoly_Square[monopoly_group[counter]]),
														     player_list, num_players_in_list, properties);

//        if(smallest_trade != 0)  /* Is a trade possible? Set it up then. FIXME - do we allow more than 2 player trades?*/
        if(smallest_trade == 2)  /* Is a trade possible? Set it up then. */
        {
			    monopoly_to_trade = monopoly_group[counter];
	    
			    /* Add the monopoly we want, giving everyone else involved in the deal monopolies
			       as we see fit, padded with cash to make it fair.  */
			    if(!AI_Trade_Monopoly(game_state, player, monopoly_to_trade, AI_Data[player].player_attitude,
								      player_list, smallest_trade, properties, importance))
                    continue;

			    /* How good are our chances to win after?  Depending on the importance of the
			       deal, wait with it if we are not going to win and it's not important enough. */
			    if((importance == AI_TRADE_NOT_IMPORTANT) || (importance == AI_TRADE_SOMEWHAT_IMPORTANT))
			    {
				    if(AI_Evaluate_Trade(game_state, player, player, Proposing_trade_list) 
										      < AI_Data[player].min_evaluation_threshold)
					    continue;  /* It's not the best, and since we can wait, wait for something better. */
			    }

			    /* Is it a good trade?  Ie. does it satisfy all the
			       trade rules? */
			    if(!AI_Trade_Is_Proper(game_state, Proposing_trade_list))
				    continue;

			    if(AI_Send_Proposed_Trade(game_state, player, FALSE))
			    {
				    AI_Data[player].time_last_trade[trade_spot] = AI_Data[player].time_forget_trade;

				    /* Send messages to players involved. */
				    for(cur_player = 0; cur_player < game_state->NumberOfPlayers; ++cur_player)
				    {
					    if(cur_player == player)
						    continue;

					    if(AI_Player_Involved_In_Trade(Proposing_trade_list, cur_player))
						    AI_Send_Message(AI_MESS_Offer_Monopoly_Trade, player, cur_player);
				    }

				    return TRUE;
			    }
			    else
				    return FALSE;
		    }
	    }
    }

	/* Otherwise, try for a non-monopoly trade. */
	if(AI_Trade_SemiImportant_Property(game_state, player, AI_Data[player].player_attitude, 
        importance, bad_player, properties))
	{
		/* Is it a good trade?  Ie. does it satisfy all the
		   trade rules? */
		if(!AI_Trade_Is_Proper(game_state, Proposing_trade_list))
			return FALSE;

		/* Send the trade and use up a trade spot. */
		if(AI_Send_Proposed_Trade(game_state, player, FALSE))
		{
			AI_Data[player].time_last_trade[trade_spot] = AI_Data[player].time_forget_trade;
			return TRUE;
		}
	}

return FALSE;
}

/****************************************************************************************/
/*					AI_Find_Smallest_Monopoly_Trade										*/
/*																						*/
/*   Input:  RULE_PlayerNumber		player		Find smallest trade for who?			*/
/*			 RULE_PropertySet		monopoly    'player' wants which monopoly?			*/
/*		     RULE_PlayerNumber      *list       List of players to to consider.		    */
/*			 int				    num_players_in_list  How many players in the list?  */
/*		     RULE_PropertySet		*properties  An array containing the property set   */
/*										        of every player.					    */
/*   Output: int  -   # of players found for the trade involving the fewest # of players*/
/*                    so that 'player' gets 'monopoly', and every other player in the   */
/*					  trade also gets a monopoly, where other players are taken from    */
/*                    *list. 0 if no such trade found.  *list will contain this smallest*/
/*					  list of players, undefined if 0.									*/
/*																						*/
/*		Figures out the minimal # of players needed from *list so that a group trade    */
/*  is possible, and returns this list in *list.  0 if not possible.				    */
/****************************************************************************************/

int AI_Find_Smallest_Monopoly_Trade(RULE_PlayerNumber player,
									RULE_PropertySet monopoly, RULE_PlayerNumber *list, int num_players_in_list,
									RULE_PropertySet *properties)
{
	RULE_PlayerNumber	smallest_player_list[RULE_MAX_PLAYERS], swap_temp;
	int	smallest_group = 0, current_group;
	RULE_PropertySet	combined_properties = 0;
	int					counter;

	if(num_players_in_list <= 0)
		return 0; /* No players in list - nothing to consider. */

	/* Is this monopoly in this list? */
	for(counter = 0; counter < num_players_in_list; ++counter)
		combined_properties |= properties[list[counter]];

	combined_properties |= properties[player];

	/* Yes, check for subsets. */
	if((combined_properties & monopoly) == monopoly)
	{
		/* Are there enough monopolies for everyone, including ourselves? */
		if(AI_Num_Of_Monopolies_In_Set(combined_properties) >= (num_players_in_list + 1))
		{
			/* Save our smallest group. */
			smallest_group = num_players_in_list;
			memcpy(smallest_player_list, list, sizeof(RULE_PlayerNumber) * num_players_in_list);
		}

		/* Check subsets. */
		for(counter = 0; counter < num_players_in_list; ++counter)
		{	/* Swap the current player with the last player. */
			swap_temp = list[num_players_in_list - 1];
			list[num_players_in_list - 1] = list[counter];
			list[counter] = swap_temp;

			/* Check it. */
			current_group = AI_Find_Smallest_Monopoly_Trade(player, monopoly,
															 list, num_players_in_list - 1, properties);

			/* Save it if it's better. */
			if(current_group != 0)
			{
				if((current_group < smallest_group) || (smallest_group == 0))
				{
					smallest_group = current_group;
					memcpy(smallest_player_list, list, sizeof(RULE_PlayerNumber) * smallest_group);
				}
			}
			
			/* Swap back. */
			list[counter] = list[num_players_in_list - 1];
			list[num_players_in_list - 1] = swap_temp;
		}

		/* Put the smallest list in *list, and return our smallest number. */
		memcpy(list, smallest_player_list, sizeof(RULE_PlayerNumber) * smallest_group);
		return (smallest_group);
	}


	/* No - then nothing to check. */
return 0;
}


/****************************************************************************************/
/*					AI_Trade_Monopoly													*/
/*																						*/
/*   Input:  RULE_GameStatePointer	game_state											*/
/*			 RULE_PlayerNumber		player		Who initiates?							*/
/*			 RULE_SquareGroups		monopoly	What monopoly does player want?			*/
/*			 float					*player_attitudes  Array of AI attitudes towards    */
/*													   all other players.				*/
/*			 RULE_PlayerNumber		*player_list  Who are we trading with for the       */
/*												  monopoly? assumed everyone takes part */
/*			 int					list_size	  How many in *player_list?			    */
/*		     RULE_PropertySet		*properties  An array containing the property set   */
/*										        of every player.					    */
/*			 AI_Trade_Importance    importance   How desperate are we for a trade?      */
/*												 Improves our offer.					*/
/*    BOOL:  TRUE if was able to create a monopoly trade people can afford.  FALSE      */
/*           otherwise.                                                                 */
/*																						*/
/*		Changes Proposing_trade_list so that player gets 'monopoly', and every other    */
/*  player deemed shrewed enough also gets a monopoly from their combined properties.   */
/*  How good a monopoly each player gets depends on how shrewd the computer considers   */
/*  them to be.  Does not trade any other properties.									*/
/****************************************************************************************/

		
BOOL AI_Trade_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareGroups monopoly,
						   float *player_attitudes, RULE_PlayerNumber *player_list, int list_size,
						   RULE_PropertySet *properties, AI_Trade_Importance importance)
{
	int					monopoly_lots, counter;
	RULE_SquareType		monopoly_squares[AI_MAX_MONOPOLY_LOTS + 1];
	long				property_bit;
	RULE_PropertySet	combined_properties;
	RULE_PlayerNumber	cur_player, owner;
	RULE_GameStateRecord new_state;
    BOOL                return_value;

	/* Get the monopoly in question. */
	monopoly_lots = AI_Get_Monopoly_Lots(game_state, monopoly_squares, AI_Expensive_Monopoly_Square[monopoly]);
	combined_properties = properties[player];

	/* Find out the combined properties of all the players in the list. */
	for(counter = 0; counter < list_size; ++counter)
		combined_properties |= properties[player_list[counter]];

	/* Add the monopoly we want so we get it. */
	for(counter = 0; counter < monopoly_lots; ++counter)
	{
		owner = game_state->Squares[monopoly_squares[counter]].owner;

		if(owner == player)
			continue;  /* We already have it.  No need to 'trade' it to ourself. */
	
		property_bit = RULE_PropertyToBitSet(monopoly_squares[counter]);
		Proposing_trade_list[player].properties_received |= property_bit;
		Proposing_trade_list[owner].properties_given |= property_bit;
	}

	/* Remove from the combined property list the properties we received. */
	combined_properties = combined_properties & ( -1l ^ Proposing_trade_list[player].properties_received);

	/* Make a copy of the game state. */
	memcpy(&new_state, game_state, sizeof(RULE_GameStateRecord));

	/*  Now give a monopoly in return, to those players that are deemed shrewd enough to
	    deserve one.  */
	for(counter = 0; counter < list_size; ++counter)
	{
		cur_player = player_list[counter];

		/* Is this us?  We already got what we want - we should not be in the list. */
		if(cur_player == player)
			continue;

        /* Give the player what he/she deserves based on our attitude settings towards them. */
		AI_Update_State(&new_state, Proposing_trade_list);
        AI_Add_Type_Property(game_state, &new_state, cur_player, player, &combined_properties,
                          AI_IMP_MONOPOLY, Proposing_trade_list, 
                          properties, FALSE);
	}

    /* Now pad with cash to make the trade fair. */
	/* Now make the trade fair.  Padd with cash. */
	return_value = AI_Make_Trade_Fair(game_state, player, player_list, list_size, Proposing_trade_list, properties,
                                      AI_MAX_GIVE_IN_TRADE, FALSE);

return (return_value);
}


/****************************************************************************************/
/*					        AI_Give_Monopoly_For_Cash									*/
/*																						*/
/*   Input:  RULE_GameStatePointer	game_state											*/
/*			 RULE_PlayerNumber		player		Who initiates?							*/
/*           RULE_Playernumber      to_player   Which player to first consider?         */
/*		     RULE_PropertySet		*properties  An array containing the property set   */
/*                                              of every player.                        */
/*    BOOL:  TRUE if was able to give someone a monopoly for an acceptable amount of    */
/*           cash, and that player could afford it.  FALSE otherwise.                   */
/*																						*/
/*      This function tries to 'give' away a monopoly to anyone who does not have a     */
/*  monopoly for cash.  Starts considering a trade with to_player.  If cannot find      */
/*  one, will go through all the players who do not own a monopoly until we are happy.  */
/*  Uses the attitude of player towards to_player to consider what to give.             */
/****************************************************************************************/

BOOL AI_Give_Monopoly_For_Cash(RULE_GameStatePointer game_state, RULE_PlayerNumber player, 
						   RULE_PlayerNumber to_player, RULE_PropertySet *properties)
{
	int					monopoly_lots, counter;
	RULE_SquareType		monopoly_squares[SG_PARK_PLACE + 1];
	RULE_PropertySet	combined_properties, our_properties_excluding_monopolies;
	RULE_PlayerNumber	cur_player;
    BOOL                return_value;
   
    /* Find out player's properties without monopolies */
    monopoly_lots = AI_Get_Monopolies(game_state, player, monopoly_squares, FALSE);
    combined_properties = properties[player];

    for(counter = 0; counter < monopoly_lots; ++counter)
        combined_properties ^= AI_Get_Monopoly_Set(monopoly_squares[counter]);

    our_properties_excluding_monopolies = combined_properties;

    /* Go through all the players, starting with to_player, who do not have a monopoly,
       and try to give them a monopoly for cash. Stop if successful. */
    for(counter = 0, cur_player = to_player - 1; counter < game_state->NumberOfPlayers; ++counter)
    {
        if(++cur_player >= game_state->NumberOfPlayers)
            cur_player = 0;

        /* Is this a player we are looking for? */
        if(game_state->Players[cur_player].currentSquare == SQ_OFF_BOARD)
            continue;

        if(cur_player == player)
            continue;
        
        if(AI_Player_Own_Monopoly(game_state, cur_player, FALSE))
            continue;

        /* A player we are looking for.  Then try to give a monopoly. */
            /* Reset trade list first. */
        memset(Proposing_trade_list, 0, sizeof(Proposing_trade_list));

        /* Find our combined property set, excluding our monopolies. */
        combined_properties = our_properties_excluding_monopolies | properties[cur_player];
        if(!AI_Add_Type_Property(game_state, game_state, cur_player, player, &combined_properties,
                          AI_IMP_MONOPOLY, Proposing_trade_list, properties, FALSE))
            continue;

        /* Managed to give a monopoly.  Now make the trade fair. */
    	return_value = AI_Make_Trade_Fair(game_state, player, &cur_player, 1, Proposing_trade_list, properties,
                                      AI_MAX_GIVE_IN_MONOPOLY_CASH_TRADE, TRUE);

        /* Did we succeed? If yes, PASS IT ON! */
        if(return_value)
		{
			// If the trade satisfies all the rules, return it. 
			if(AI_Trade_Is_Proper(game_state, Proposing_trade_list))
				return TRUE;
		}
    }

return FALSE;  /* Did not find a trade. */
}


/****************************************************************************************/
/*					AI_Trade_Add_Type_Property											*/
/*																						*/
/*   Input:  RULE_GameStatePointer	game_state											*/
/*			 RULE_PlayerNumber		player		Find smallest trade for who?			*/
/*			 RULE_PropertySet		monopoly    'player' wants which monopoly?			*/
/*		     RULE_PlayerNumber      *list       List of players to to consider.		    */
/*			 int				    num_players_in_list  How many players in the list?  */
/*		     RULE_PropertySet		*properties  An array containing the property set   */
/*										        of every player.					    */
/*   Output: int  -   # of players found for the trade involving the fewest # of players*/
/*                    so that 'player' gets 'monopoly', and every other player in the   */
/*					  trade also gets a monopoly, where other players are taken from    */
/*                    *list. 0 if no such trade found.  *list will contain this smalelst*/
/*					  list of players, undefined if 0.									*/
/*																						*/
/*		Figures out the minimal # of players needed from *list so that a group trade    */
/*  is possible, and returns this list in *list.  0 if not possible.				    */
/****************************************************************************************/


BOOL AI_Trade_Add_Type_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber from_player, RULE_PlayerNumber to_player,
								AI_Property_Importance type, AI_Trade_List_Record *trade_list, RULE_PropertySet *properties)
{
	RULE_SquareType		SquareNo;
	long				bit, monopoly_set;

	/* Go through all the properties from_player owns. */
	for(SquareNo = 0; SquareNo < SQ_IN_JAIL;SquareNo++)
	{
		bit = RULE_PropertyToBitSet(SquareNo);
				
			/* Do we own this property and not traded it away already? */
		if((bit & properties[from_player]) && !(bit & trade_list[from_player].properties_given))
		{	
			/* Is it a monopoly we own or are about to get?  Then let's not talk about it. */
			if(AI_Test_For_Monopoly(properties[from_player] | trade_list[from_player].properties_received, SquareNo))
				continue;

			/* Is it part of a monopoly group we want properties for?  It's silly to
			   get one property from a monopoly and give another one away in the same
			   trade. */
			monopoly_set = AI_Get_Monopoly_Set(SquareNo);
			if(monopoly_set & trade_list[from_player].properties_received)
				continue;

			/* Check it's importance - is it what we are looking for? */
			if(AI_Give_Players_Important_Monopoly(game_state, SquareNo, RULE_NOBODY_PLAYER, to_player) == type)	
			{	
				/* It is.  Then give it to to_player. */
				trade_list[from_player].properties_given |= bit;
				trade_list[to_player].properties_received |= bit;
				return TRUE;
			}
		}
	}

return FALSE;
}


/****************************************************************************************/
/*					AI_Add_Trade_Monopoly												*/
/*																						*/
/*   Input:  RULE_GameStatePointer	game_state											*/
/*			 RULE_PlayerNumber		to_player	Who is getting a monopoly?				*/
/*		     RULE_PropertySet		*properties  An array containing the property set   */
/*										        of every player.					    */
/*			 AI_PropertyClassification type     Either AI_Property_Best or _Worst.      */
/*			 AI_Trade_List_Record   *trade_list Where to store the trade?				*/
/*   Output: BOOL  - TRUE if a monopoly was found for player, and properties traded.    */
/*                   FALSE otherwise.													*/
/*																						*/
/*		Gives the best or worst monopoly for player out of *properties and saves this   */
/*  trade in *trade_list array.														    */
/****************************************************************************************/


BOOL AI_Add_Trade_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber to_player,
							 RULE_PropertySet *properties, AI_Property_Classification type,
							 AI_Trade_List_Record *trade_list)
{
	RULE_SquareGroups	monopoly_group[SG_PARK_PLACE + 1];
	int		counter;
	long	liquid_assets;
	RULE_SquareType		SquareNo, end_square;
	long	bit;

		/* First, rank the monopolies based on player's liquid assets. */
	liquid_assets = AI_Get_Liquid_Assets(game_state, to_player, FALSE, FALSE);

    AI_Order_Monopoly_Importance(liquid_assets, monopoly_group, type == AI_Property_Best ? AI_Sort_Descending:AI_Sort_Ascending);

	/* Now go through them until we find one in the property set. */
	for(counter = 0; counter < SG_PARK_PLACE + 1; ++counter)
	{
		if(AI_Test_For_Monopoly(*properties, AI_Expensive_Monopoly_Square[monopoly_group[counter]]))
		{	/* Found the monopoly in the group.  Then give it to to_player and exit. */
            SquareNo = AI_Group_Start(monopoly_group[counter]);
            end_square = AI_Group_End(monopoly_group[counter]);

	        for(; SquareNo < end_square; SquareNo++)
			{
				if(RULE_SquarePredefinedInfo[SquareNo].group == monopoly_group[counter])
				{	/* One of the monopoly squares. Add it to trade list. */

                    if(game_state->Squares[SquareNo].owner != to_player)
                    {
					    bit = RULE_PropertyToBitSet(SquareNo);
					    trade_list[to_player].properties_received |= bit;
					    trade_list[game_state->Squares[SquareNo].owner].properties_given |= bit;
                    }
					
                    /* Delete it from the property list. */
					*properties = *properties & (-1l ^ bit);
				}
			}

			/* given a monopoly. Done our job. */
			return TRUE;
		}
	}
	
/* Didn't find a monopoly. */
return FALSE;
}


/****************************************************************************************/
/*					AI_Order_Monopoly_Importance										*/
/*																						*/
/*   Input:  long			liquid_assets		Sort according to this cash available.	*/
/*			 RULE_SquareGroups	*monopoly_group  Where to store order of monopolies.    */
/*			 AI_Sort_Type   sort_order         Sort descending,ascending, or random?    */
/*																						*/
/*		Sorts the importance of monopolies depending on the liquid assets available.    */
/*  Uses AI_Monopoly_To_Assets_Importance.												*/
/****************************************************************************************/

void	AI_Order_Monopoly_Importance(long liquid_assets, RULE_SquareGroups	*monopoly_group, AI_Sort_Type sort_order)
{
	int	counter, sort_outer, sort_inner, index_num;
	double	monopolies[SG_PARK_PLACE + 1], swap_temp;
	RULE_SquareGroups	swap_group;	
	double  *point_inner, *point_outer;
	
	/* Figure out the index into the array. */
	if(liquid_assets > AI_MAX_DARZINSKIS_CHART)
		index_num = AI_MAX_DARZINSKIS_CHART / AI_DARZINSKIS_CHART_SEPARATION;
	else
		index_num = liquid_assets / AI_DARZINSKIS_CHART_SEPARATION;

	for(counter = 0; counter < SG_PARK_PLACE + 1; ++counter)
	{
        if(sort_order == AI_Sort_Random)
    		monopolies[counter] = rand();
        else
    		monopolies[counter] = AI_Monopoly_To_Assets_Importance[counter][index_num];

        monopoly_group[counter] = counter;
	}

	/* Order them in order depending on type - linear sort. */
	point_outer = monopolies;
	for(sort_outer = 0; sort_outer < SG_PARK_PLACE + 1; ++sort_outer, ++point_outer)
	{	
		point_inner = point_outer;
		++point_inner;
		for(sort_inner = sort_outer + 1; sort_inner < SG_PARK_PLACE + 1; ++sort_inner, ++point_inner)
		{
			if( sort_order == AI_Sort_Descending ? (*point_outer < *point_inner):(*point_outer > *point_inner) )
			{	/* swap them. */
				swap_temp = *point_outer;
				*point_outer = *point_inner;
				*point_inner = swap_temp;
				
				swap_group = monopoly_group[sort_outer];
				monopoly_group[sort_outer] = monopoly_group[sort_inner];
				monopoly_group[sort_inner] = swap_group;
			}
		}
	}

return;
}


/****************************************************************************************/
/*					AI_Trade_SemiImportant_Property										*/
/*																						*/
/*   Input:  RULE_GameStatePointer    game_state										*/
/*			 RULE_PlayerNumber		  player      Who is proposing the trade?			*/
/*			 float			   *player_attitudes  How does player feel towards everyone?*/
/*			 AI_Trade_Importance      importance  How important is this trade? For cash?*/
/*			 RULE_PlayerNumber		  bad_player  Who NOT to trade with.				*/
/*			 RULE_PropertySet		 *properties  Array of who owns what.				*/
/*	 Output:  BOOL  - TRUE if a trade was proposed, false otherwise.					*/
/*																						*/
/*		Tries to set up a non-monopoly forming trade.  Selects one player with who      */
/*  to trade, trying to trade more with players who are easy.  Then tries to get        */
/*  properties for the best 3 monopolies we want, based on our liquid assets.  Otherwise*/
/*  goes for a cash cow, or nothing.  Will try to give a property for one of the worst  */
/*  monopolies for the trading player, based on trading player's liquid assets.         */
/****************************************************************************************/


BOOL AI_Trade_SemiImportant_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
									     float *player_attitudes, AI_Trade_Importance importance,
										 RULE_PlayerNumber bad_player, RULE_PropertySet *properties)
{
	RULE_PlayerNumber	target_player, player_list[RULE_MAX_PLAYERS];
	int					player_list_size = 0;
	double				chance_trade, total_chance = 0.0;
	int					counter, index, num_props_taken = 0, num_props_given = 0, list_index;
	int					props_want_to_trade, props_want_to_give;
	RULE_SquareType		trade_property;
	long				liquid_assets, bit;
	RULE_SquareGroups	monopoly_group[SG_PARK_PLACE + 1], group;
	RULE_PropertySet	target_props, monopoly_set, want_set, set_received;
	int					player_index[RULE_MAX_PLAYERS], ai_level;
	float				temp_hold;
	BOOL				attitude_too_low;

	/* First find a target player to trade with.  We want a large chance of
	   trading with players we have a low attitude towards. */
	
	ai_level = game_state->Players[player].AIPlayerLevel;
	for(target_player = 0; target_player < game_state->NumberOfPlayers; ++target_player)
	{
		if(game_state->Players[target_player].currentSquare == SQ_OFF_BOARD)
			continue; /* gone bankrupt. */

		if(target_player == player)
			continue;

		if(AI_Data[player].player_attitude[target_player] < AI_DONT_TRADE_ATTITUDE)
			continue;  /* Hasn't traded in the past. Ignore him. */

		if(target_player == bad_player)
			continue; /* Not trading with this one! */

		/* Add this player to a list. */
		player_list[player_list_size++] = target_player;

		total_chance += (AI_Data[player].player_attitude[target_player] + 1.0) / 2;
	}

	if(player_list_size == 0)
		return FALSE;	/* no one to trade with - leave. */

	/* Randomly pick a player to trade with, weighted towards the weak players. */
	chance_trade = rand() / ((double) RAND_MAX);

	/* Who did we pick? */
	for(counter = 0; counter < player_list_size; ++counter)
	{
		target_player = player_list[counter];
		chance_trade -= (AI_Data[player].player_attitude[target_player] + 1.0) / 2 / total_chance;
		if(chance_trade <= 0)
			break;
	}

	if(counter >= player_list_size)
		counter = player_list_size - 1;

	list_index = counter;

	/* Now go through all the players in the list, starting with the player
	   we randomly chose, and see if we can propose some kind of trade we
	   are happy with. */
	for(counter = 0; counter < player_list_size; ++counter)
	{
		target_player = player_list[list_index++];
		if(list_index >= player_list_size)
			list_index = 0;
	
		/* Reset the trade list. */
		memset(Proposing_trade_list, 0, sizeof(AI_Trade_List_Record) * RULE_MAX_PLAYERS);

		/* found who we are trading with.  Now figure out what kind of trade we want to
		propose. # of properties traded will be a random number between 1 - 3. */
		/* First, find out what monopolies we want most. */
		liquid_assets = AI_Get_Liquid_Assets(game_state, player, FALSE, FALSE);
	
        if((importance & AI_TRADE_FOR_CASH) || (ai_level != 3))
		    AI_Order_Monopoly_Importance(liquid_assets, monopoly_group, AI_Sort_Random);
        else
		    AI_Order_Monopoly_Importance(liquid_assets, monopoly_group, AI_Sort_Descending);

		props_want_to_trade = rand() % 3 + 1;
        props_want_to_give = rand() % 2 - 1 + props_want_to_trade;

		target_props = properties[target_player];
		set_received = 0;
        num_props_given = num_props_taken = 0;

		/* Try to get properties for the top 4 monopolies we want. */
		if(!(importance & AI_TRADE_FOR_CASH))
		{	/* If we need cash desperately, don't ask for anything. */

			for(index = 0; index < 3; ++index)
			{	/* Look for the top 3 monopolies only. */

				monopoly_set = AI_Get_Monopoly_Set(AI_Expensive_Monopoly_Square[monopoly_group[index]]);

				if((target_props & monopoly_set) != 0)
				{ /* Player owns properties for the monopoly we want. */

					want_set = target_props & monopoly_set;

					if(AI_Test_For_Monopoly(want_set, AI_Expensive_Monopoly_Square[monopoly_group[index]]))
						continue;  /* however, it's a monopoly for him.  forget it then. */

					if(AI_Test_For_Monopoly(properties[player] | want_set, AI_Expensive_Monopoly_Square[monopoly_group[index]]))
						continue;	/* Monopoly for us.  We are not trying to get a monopoly. */

					/* Does anyone who has a monopoly own a property in this monopoly?  
					   Then forget it. They probably have no incentive to deal with us. */
					if(AI_Owned_By_Monopoly_Player(game_state, monopoly_group[index], player))
						continue;

					/* Then GET IT! */
					Proposing_trade_list[target_player].properties_given |= want_set;
					Proposing_trade_list[player].properties_received |= want_set;
					target_props = target_props & (-1l ^ want_set);
					set_received |= want_set;

					/* Count 'em up. */
					num_props_taken += AI_Number_Properties_In_Set(want_set);

					if(num_props_taken >= props_want_to_trade)
						break;	/* We've taken enough properties. */
				}
			}

			/* Did we find anything? If not - try for cash cows at least. */
			if(num_props_taken == 0)
			{	
				for(trade_property = 0; trade_property < SQ_IN_JAIL; ++trade_property)
				{
					group = RULE_SquarePredefinedInfo[trade_property].group;
					if(group == SG_UTILITY || group == SG_RAILROAD)
					{	/* Is the property a cash cow? */

						bit = RULE_PropertyToBitSet(trade_property);
						if(bit & target_props)	
						{	/* Does our target player own it? */
							/* Then get it! */
							Proposing_trade_list[target_player].properties_given |= bit;
							Proposing_trade_list[player].properties_received |= bit;
							num_props_taken++;
						}

						break;
					}
				}

				if(num_props_taken == 0)
					continue; /* Found nothing from this player we want. */
			}
		}

		/* Now offer something in return. */
		/* First, find out what monopolies s/he wants least. */
		liquid_assets = AI_Get_Liquid_Assets(game_state, target_player, FALSE, FALSE);
		AI_Order_Monopoly_Importance(liquid_assets, monopoly_group, AI_Sort_Ascending);	/* Worst first. */

		target_props = properties[player];

		/* need to create an index to the monopoly importance chart. */
		attitude_too_low = FALSE;
		if(player_attitudes[target_player] < AI_Data[player].minimum_nonmonopoly_trade_attitude)
		{
			if(!(importance & AI_TRADE_DESPERATE) &&
			    !(importance & AI_TRADE_SOMEWHAT_IMPORTANT))
			{
				temp_hold = player_attitudes[target_player];
				player_attitudes[target_player] = AI_Data[player].minimum_nonmonopoly_trade_attitude;
				attitude_too_low = TRUE;
			}
		}

		if(player_attitudes[target_player] <= -1.0)
			player_index[target_player] = 0;
		else player_index[target_player] = (int) ((player_attitudes[target_player] + 1) * 10);

			/* Restore the attitude if changed - don't want to disturb the array. */
		if(attitude_too_low)
			player_attitudes[target_player] = temp_hold;


		if(importance & AI_TRADE_SOMEWHAT_IMPORTANT)
			player_index[target_player] -= 2; /* somewhat important - improve our offer. */

		if(importance & AI_TRADE_DESPERATE)
			player_index[target_player] -= 5; /* Desperate - really improve our offer. */

		if(player_index[target_player] >= AI_WHAT_TO_TRADE_ENTRIES)
			player_index[target_player] = AI_WHAT_TO_TRADE_ENTRIES - 1;

		if(player_index[target_player] < 0)
			player_index[target_player] = 0;

			/* Try to give properties for the worst 4 monopolies he/she wants, or go through
		all of them if we are desperate for money. */
		for(index = 0; index < (importance & AI_TRADE_FOR_CASH ? (SG_PARK_PLACE + 1):4); ++index)
		{
			monopoly_set = AI_Get_Monopoly_Set(AI_Expensive_Monopoly_Square[monopoly_group[index]]);

			if((target_props & monopoly_set) != 0)
			{ /* We own properties for the monopoly. */
	
				want_set = target_props & monopoly_set;

				if(AI_Test_For_Monopoly(want_set, AI_Expensive_Monopoly_Square[monopoly_group[index]]))
					continue;  /* however, it's a monopoly for us. */

				/* If we are really desperate for a trade, might have to
				   give someone a monopoly to stay in the game. */
				if(!(importance & AI_TRADE_DESPERATE))
				{
					if(AI_Test_For_Monopoly(properties[target_player] | want_set, AI_Expensive_Monopoly_Square[monopoly_group[index]]))
						continue;	/* Monopoly for target player.  We are not trying to give a monopoly. */
				}

				/* Then give it. */
				Proposing_trade_list[target_player].properties_received |= want_set;
				Proposing_trade_list[player].properties_given |= want_set;
				target_props = target_props & (-1l ^ want_set);

				/* Count 'em up. */
				num_props_given += AI_Number_Properties_In_Set(want_set);

				/* Have we added enough? */
				if(importance & AI_TRADE_FOR_CASH)
					break;	/* Trade one property at a time if trading for cash. */

                if(num_props_given > props_want_to_give)
				{	/* Remove these properties from the list then. */
					Proposing_trade_list[target_player].properties_received ^= want_set;
					Proposing_trade_list[player].properties_given ^= want_set;
					target_props ^= want_set;
					num_props_given -= AI_Number_Properties_In_Set(want_set);
					break;
				}
			}
		}

		if((num_props_given == 0) && (num_props_taken == 0))
			continue;	/* No trade - nothing given nor taken.  */

		/* Make the trade fair by buffering with cash. */

        if( !AI_Make_Trade_Fair(game_state, player,	&target_player, 1,
						Proposing_trade_list, properties, AI_MAX_GIVE_IN_TRADE, FALSE) )
            continue; /* Couldn't get enough cash for the trade. */

		return TRUE;
	}

	/* Nope - no trade found. */
return FALSE;
}

/****************************************************************************************/
/*					AI_Owned_By_Monopoly_Player											*/
/*																						*/
/*   Input:  RULE_GameStatePointer    game_state										*/
/*			 RULE_SquareGroups		  group		Which monopoly are we interest in?		*/
/*			 RULE_PlayerNumber		  player    Who is interested?						*/
/*	 Output:  BOOL  - TRUE if a player other than player owns properties				*/
/*					  in this monopoly, and this player owns a monopoly of his own or   */
/*					  is not trading with us (attitude too low.)						*/
/*																						*/
/*		Determines if a player who owns a monopoly owns a property of the monopoly      */
/* 'group'.  This is used to determine whether it is worth trading with other players   */
/* for properties in a monopoly that is owned by someone who will probably have little  */
/* incentive to trade with us.  Also, if the attitude of player towards the owner is    */
/* too low, will also return TRUE.														*/
/****************************************************************************************/

BOOL  AI_Owned_By_Monopoly_Player(RULE_GameStatePointer game_state, RULE_SquareGroups group, RULE_PlayerNumber player)
{
	RULE_SquareType	cur_square, end_square;
	RULE_SquarePredefinedInfoPointer	square_pred;
	RULE_SquareInfoPointer	square_info;

    cur_square = AI_Group_Start(group);
    end_square = AI_Group_End(group);

    square_pred = &RULE_SquarePredefinedInfo[cur_square];
	square_info = &game_state->Squares[cur_square];

	/* Go through all the squares in the monopoly 'group' */
	for(; cur_square < end_square; cur_square++, square_pred++, square_info++)
	{
		if(square_pred->group == group)
		{
			if(square_info->owner == player)
				continue;	/* It is the player asking - skip him/her */

			if(AI_Data[player].player_attitude[square_info->owner] < AI_DONT_TRADE_ATTITUDE)
				return TRUE;	/* Owner has shown not to be a willing trader - return TRUE */

			/* Doesthe player who owns this own a monopoly? */
			if(AI_Player_Own_Monopoly(game_state, square_info->owner, TRUE))
				return TRUE;	/* Yes - return TRUE. */
		}
	}

return FALSE;
}


/****************************************************************************************/
/*					AI_Player_Involved_In_Trade											*/
/*																						*/
/*   Input:  AI_Trade_List_Record	*Proposing_trade_list		The trade.				*/
/*			 RULE_PlayerNumber		player					    Player involved?		*/
/*	 Output: BOOL - TRUE if Player has any part in the trade.  FALSE otherwise.			*/
/****************************************************************************************/

BOOL AI_Player_Involved_In_Trade(AI_Trade_List_Record *Proposing_trade_list, RULE_PlayerNumber player)
{
	int counter;
	char	*point;

	point = (char *) &Proposing_trade_list[player];
	for(counter = sizeof(AI_Trade_List_Record); counter > 0; --counter, ++point)
	{
		if(*point != 0)
			return TRUE;
	}

return FALSE;
}



/****************************************************************************************/
/*					AI_Make_Trade_Fair													*/
/*																						*/
/*   Input:  RULE_GameStatePointer	game_state											*/
/*			 RULE_PlayerNumber		player		Who is proposing the trade?				*/
/*           RULE_PlayerNumber      *player_list  List of players in the trade,         */
/*                                                excluding 'player'.                   */
/*           long                   list_size   How many players in player_list?        */
/*			 AI_Trade_List_Record   *Proposing_trade_list   The trade being considered.	*/
/*		     RULE_PropertySet		*properties  An array containing the property set   */
/*										        of every player before the trade.	    */
/*           long give_most         What is the most cash we are willing to give to make*/
/*                                  the trade fair?                                     */
/*           BOOL giving_monopoly   Is this a trade where the AI gives a monopoly for   */
/*                                  cash?                                               */
/*   Output: BOOL - TRUE if each player has enough liquid assets to make the trade fair.*/
/*					FALSE if someone does not have enough.								*/
/*																						*/
/*    This function gives/asks for money from player to players in the trade to make    */
/*  player happy with the trade.  Uses attitude of player towards trading partners to   */
/*  alter the money given to each player.  When asking for money, tries to ask from the */
/*  player who is getting the best deal.  When giving, tries to give to the player who  */
/*  is getting the worst deal.                                                          */
/****************************************************************************************/

BOOL AI_Make_Trade_Fair(RULE_GameStatePointer game_state, RULE_PlayerNumber player,	
                        RULE_PlayerNumber *player_list, long list_size,
						AI_Trade_List_Record *Proposing_trade_list, RULE_PropertySet *properties,
                        long give_most, BOOL giving_monopoly)
{
    long                index, counter;
    long                change, can_pay;
	long                cash_given[RULE_MAX_PLAYERS];
    float               best_evaluation, factor, worst_evaluation;
    float               player_evaluations[RULE_MAX_PLAYERS], min_evaluation;
    RULE_PlayerNumber   best_player, cur_player, worst_player;
    RULE_PropertySet    combined_properties;
    BOOL                done_changing_cash = FALSE;

    if(list_size < 1)
        return FALSE;   /* Something wrong with the player list. */

    /* First, find the most we are willing to pay. */
    memset(cash_given, 0, sizeof(cash_given));
    change = give_most / 2;

    /* We'll be using AI_Evaluate_Trade_Player_List - might as well use it to evaluate ourself. */
    player_list[list_size] = player;

    /* Reset cash exchanges if this is a 2 player trade. */
    if(list_size == 1)
    {
        Proposing_trade_list[player].cash_received = 0;
        Proposing_trade_list[player].cash_given = 0;
        Proposing_trade_list[*player_list].cash_received = 0;
        Proposing_trade_list[*player_list].cash_given = 0;
    }

    /* Use different evaluation limits for the case where we are giving a monopoly, 
       versus any other time. */
    if(!giving_monopoly)
        min_evaluation = AI_Data[player].min_evaluation_threshold;
    else
        min_evaluation = AI_Data[player].min_give_monopoly_evaluation;

    while(!done_changing_cash)
    {
        /* Find the best and worst evaluations in this trade. */
        AI_Evaluate_Trade_Player_List(game_state, player_list, list_size + 1, player,
                                      Proposing_trade_list, player_evaluations, giving_monopoly);

        for(index = 0; index < list_size; ++index)
        {
            cur_player = player_list[index];
            if(index == 0)
            {
                best_evaluation = player_evaluations[index];
                worst_evaluation = player_evaluations[index];
                best_player = cur_player;
                worst_player = cur_player;
            }

            if(player_evaluations[index] > best_evaluation)
            {
                best_evaluation = player_evaluations[index];
                best_player = cur_player;
            }

            if(player_evaluations[index] < worst_evaluation)
            {   
                worst_evaluation = player_evaluations[index];
                worst_player = cur_player;
            }
        }

        /* Should we give cash, or get some?  If evaluation of trade is too low for us,
           or we are getting the shortest end of the stick in this trade, get cash. */
        if((player_evaluations[list_size] < min_evaluation) ||
           (worst_evaluation >= player_evaluations[list_size]))  
        {
            if(change <= 2) /* Do we want to quit? */
            {
                change = 5;  /* Yes - but we want a trade we'll accept. */
                done_changing_cash = TRUE;
            }

            /* We want cash - pick the player who is getting the best deal,
               and take from him/her. */
			Proposing_trade_list[player].cash_received += change;
			Proposing_trade_list[best_player].cash_given += change;
            cash_given[best_player] -= change;
        }
        else
        {
            if(change <= 2) /* Do we want to quit? */
            {
                done_changing_cash = TRUE;
                continue;  /* Yes - and we are satisfied with the evaluation. */
            }

            /* We need to give some cash - pick the worst player, and give appropriately. */
            /* We want cash - pick the player who is getting the best deal,
               and take from him/her. */
			Proposing_trade_list[worst_player].cash_received += change;
			Proposing_trade_list[player].cash_given += change;
            cash_given[worst_player] += change;
        }

        change /=  2;
	} 

    /* Now change the amount of cash we give based on our evaluation of each opponent. */
    for(index = 0; index < list_size; ++index)
    {
        cur_player = player_list[index];
		factor = (float) (1.0 / AI_Get_Cash_Multiplier(player, cur_player));

        if(cash_given[cur_player] > 0)
        {
            change = (long) (cash_given[cur_player] - cash_given[cur_player] * factor);
            Proposing_trade_list[cur_player].cash_received -= change;
            Proposing_trade_list[player].cash_given -= change;
        }
        else
        {
            change = (long) (cash_given[cur_player] - cash_given[cur_player] / factor);
            Proposing_trade_list[cur_player].cash_received -= change;
            Proposing_trade_list[player].cash_given -= change;
        }
    }

    /* Make sure everyone can pay, and everyone gave/got something. */

	/* make sure we can all afford it. */
	for(counter = 0; counter < list_size + 1; ++counter)	
	{
        if(counter == list_size)
            cur_player = player;    /* Make sure we check ourself too. */
        else
		    cur_player = player_list[counter];

        /* even out the cash given and cash received. */
		Proposing_trade_list[cur_player].cash_given -= Proposing_trade_list[cur_player].cash_received;
		Proposing_trade_list[cur_player].cash_received = 0;

		if(Proposing_trade_list[cur_player].cash_given < 0)
		{
			Proposing_trade_list[cur_player].cash_received = -Proposing_trade_list[cur_player].cash_given;
			Proposing_trade_list[cur_player].cash_given = 0;
		}

		/* How much tax are we going to pay in transfer fees? */
		can_pay = -AI_Get_Transfer_Tax(game_state, Proposing_trade_list[cur_player].properties_received);
			
		/* How much money are the properties worth if mortgaged? Don't mortgage monopolies or sell houses. */
		combined_properties = properties[cur_player] & (-1l ^ Proposing_trade_list[cur_player].properties_given);
		can_pay += AI_Get_Liquid_Assets_Property(game_state, combined_properties, FALSE, FALSE);
		can_pay += game_state->Players[cur_player].cash;
        if(SlotIsALocalAIPlayer[cur_player])
            can_pay -= AI_State[cur_player].money_owed;
	
		if(can_pay < Proposing_trade_list[cur_player].cash_given)
        {
            /* Couldn't pay it - but if we are giving a monopoly away, we should
               consider asking for less, to the point where the player can afford
               it.  We are, after all, already evaluating at a higher
               level. */
            if(giving_monopoly)
            {
                if(can_pay >= -cash_given[cur_player])
                {   /* Meet half way, between what can pay, and what we
                       will accept as a minimum. */
                    change = (can_pay - cash_given[cur_player]) / 2 ;
                    change -= Proposing_trade_list[cur_player].cash_given;
                    Proposing_trade_list[cur_player].cash_given += change;
                    Proposing_trade_list[player].cash_received += change;
                    break;
                }

                return FALSE; /* can't afford it! */
            }
            else
			    return FALSE;  /* Someone couldn't pay for the counter proposal - forget it then. */
        }
	}

    /* Now make sure everyone gave/got something. */
	if(!AI_Trade_Is_Proper(game_state, Proposing_trade_list))
		return FALSE;

return TRUE;
}

/****************************************************************************************/
/*					AI_Get_Transfer_Tax													*/
/*																						*/
/*   Input:  RULE_GameStatePointer	game_state											*/
/*		     RULE_PropertySet		properties  What properties to check for tax?		*/
/*	 Output: long  - amount of transfer tax that would be paid if 'properties' were     */
/*					 transfered to a new owner.											*/
/*																						*/
/*		Calculates how much tax a player would pay if were to received properties.		*/
/****************************************************************************************/

long AI_Get_Transfer_Tax(RULE_GameStatePointer game_state, RULE_PropertySet properties)
{
	RULE_PropertySet	mask = 1;
	long				tax_amount = 0;
	RULE_SquareType		square;

	/* Go through all the properties in the property set */
	while(properties != 0)
	{
		if(properties & mask)
		{
			square = RULE_BitSetToProperty(mask);			
			if(game_state->Squares[square].mortgaged)
			{
				tax_amount += RULE_SquarePredefinedInfo[square].purchaseCost *
					          game_state->GameOptions.taxRate / 100;
			}
		}

		/* Delete the property from the list, and increase our mask. */
		properties &= -1l ^ mask;
		mask <<= 1;
	}

return tax_amount;
}


/****************************************************************************************/
/*					AI_Send_Proposed_Trade												*/
/*																						*/
/*   Input:  RULE_GameStatePointer	game_state											*/
/*			 RULE_PlayerNumber		player		Who is proposing the trade?				*/
/*			 BOOL				  trade_accept	Is this request sent during a trade		*/
/*												accept decision?						*/
/*	 Output: BOOL - FALSE if the trade should be killed. TRUE otherwise if sending it.	*/
/*																						*/
/*	Sends the new trade to the rules engine via action_trade_item messages.  Does not   */
/*  do immunities, since the AIs are not changing immunities right now.					*/
/****************************************************************************************/

BOOL AI_Send_Proposed_Trade(RULE_GameStatePointer game_state, RULE_PlayerNumber player, BOOL trade_accept)
{
	RULE_PlayerNumber	cur_player, need_cash_player, cur_cash_player;
	AI_Trade_List_Record	 *trade_point, *cur_trade_point;
	RULE_SquareType		SquareNo;
	long				bit;
	int					counter;
	BOOL				found_giver = FALSE;
	RULE_PropertySet	properties_traded = 0;
	RULE_PlayerNumber	card_received[MAX_DECK_TYPES], cur_card_received[MAX_DECK_TYPES];
  int i, j, numinvolved;
  BOOL involved[RULE_MAX_PLAYERS];

	if(AI_PLAYER_SENDING_TRADE &&  !game_state->TradeInProgress)
		return FALSE;	/* Some AI already proposing trade at this moment. */

  /** Check that this is only a two-player trade *********************/
  numinvolved = 0;
  trade_point = Proposing_trade_list;
  for (i = 0; i < game_state->NumberOfPlayers; i++, trade_point++) {
    involved[i] = FALSE;
    if (trade_point->cash_given || trade_point->cash_received)
      { involved[i] = TRUE; continue; }

    if (trade_point->properties_given || trade_point->properties_received)
      { involved[i] = TRUE; continue; }

    for (j = 0; j < MAX_DECK_TYPES; j++)
      if (trade_point->jail_card_given[j] || trade_point->jail_card_received[j])
        { involved[i] = TRUE; continue; }
  }

  for (i = 0; i < game_state->NumberOfPlayers; i++)
    if (involved[i])
      numinvolved++;

  if (numinvolved != 2)
    return FALSE;
  /*******************************************************************/

	/* First check if we are in editing mode.  If not, send the message to get into it. */
	if(!game_state->TradeInProgress || trade_accept)
	{	/* Trade not in progress or we are counter proposing current trade.
		   Then request to edit it. */
		AI_SendAction( ACTION_START_TRADE_EDITING, player, RULE_BANK_PLAYER,
									0, 0, 0, 0,NULL, 0, NULL);

		AI_PLAYER_SENDING_TRADE = AI_Send_Asking_Trade_Edit;
		return TRUE;
	}
	
	/* First clear the list from all properties not traded, and all cash trades. */
		/* First, delete all cash. */
	for(cur_player = 0; cur_player < game_state->NumberOfPlayers; ++cur_player)
	{
		for(cur_cash_player = 0; cur_cash_player < game_state->NumberOfPlayers; ++cur_cash_player)
		{
			if(game_state->Players[cur_player].shared.trading.cashGivenInTrade[cur_cash_player])
			{
				MESS_SendAction(ACTION_TRADE_ITEM, player, RULE_BANK_PLAYER,
								cur_player, cur_cash_player,   TIK_CASH,
								0, NULL, 0, NULL);
				AI_State[player].num_actions++;
			}
		}
	}

	/* Now send cash. */
        /* Normalize the cash given and received. */
    trade_point = Proposing_trade_list;
	for(cur_player = 0; cur_player < RULE_MAX_PLAYERS; ++cur_player, ++trade_point)
    {
        trade_point->cash_given -= trade_point->cash_received;
        trade_point->cash_received = 0;

        if(trade_point->cash_given < 0)
        {
            trade_point->cash_received = -trade_point->cash_given;
            trade_point->cash_given = 0;
        }
    }

	// Go through all the players wanting cash, and give them from players giving cash.
	do
	{
		trade_point = Proposing_trade_list;

		need_cash_player = AI_Get_Next_Player_Wanting_Cash(trade_point);
	
		/* Did we find someone receiving cash? */
		if(need_cash_player != RULE_NOBODY_PLAYER)
		{
			/* Then find someone giving cash. */
			found_giver = FALSE;
			for(cur_player = 0; cur_player < RULE_MAX_PLAYERS; ++cur_player, ++trade_point)
			{
				if(trade_point->cash_given != 0)
				{
					// Is this player giving enough to satisfy need_cash_player, or do we need
					// to find someone else after?
					if(Proposing_trade_list[need_cash_player].cash_received <= trade_point->cash_given)
					{
						found_giver = TRUE;

						MESS_SendAction(ACTION_TRADE_ITEM, player, RULE_BANK_PLAYER,
										cur_player, need_cash_player, TIK_CASH, 
										Proposing_trade_list[need_cash_player].cash_received,NULL, 0, NULL);
						AI_State[player].num_actions++;
						trade_point->cash_given -= Proposing_trade_list[need_cash_player].cash_received;
						Proposing_trade_list[need_cash_player].cash_received = 0;
						break;
					}
					else
					{
						found_giver = TRUE;

						MESS_SendAction(ACTION_TRADE_ITEM, player, RULE_BANK_PLAYER,
										cur_player, need_cash_player,   TIK_CASH,
										trade_point->cash_given,NULL, 0, NULL);
						AI_State[player].num_actions++;
						Proposing_trade_list[need_cash_player].cash_received -= trade_point->cash_given;
						trade_point->cash_given = 0;
					}
				}
			}
		}

		if(!found_giver)
			break;
	} while (need_cash_player != RULE_NOBODY_PLAYER);

	// Reset card_received and cur_card_received;
	for(counter = 0; counter < MAX_DECK_TYPES; ++counter)
	{
		card_received[counter] = RULE_NOBODY_PLAYER;
		cur_card_received[counter] = RULE_NOBODY_PLAYER;
	}

	/* All cash sent.  Now properties. */
	trade_point = Proposing_trade_list;
	cur_trade_point = Current_Trade_List;
	for(cur_player = 0; cur_player < RULE_MAX_PLAYERS; ++cur_player, ++trade_point, ++cur_trade_point)
	{
		for(SquareNo = 0; SquareNo < SQ_IN_JAIL;SquareNo++)
		{
			bit = RULE_PropertyToBitSet(SquareNo);
				
			/* Player getting this property? */
			if(bit & trade_point->properties_received)
			{	
				// Are we already receiving this property? No need to send
				// a message to confirm this then.
				if(!(bit & cur_trade_point->properties_received))
				{
					/* No. Then send it. */
					MESS_SendAction(ACTION_TRADE_ITEM, player, RULE_BANK_PLAYER,
									game_state->Squares[SquareNo].owner, cur_player, TIK_SQUARE,
									SquareNo, NULL, 0, NULL);

					AI_State[player].num_actions++;
				}
				properties_traded |= bit;
			}
			else if(bit & cur_trade_point->properties_received)
			{	// This property is currently assigned to this player.  Need to clear it.
				// But did we already send it to someone else?
				if(!(bit & properties_traded))
				{
					MESS_SendAction(ACTION_TRADE_ITEM, player, RULE_BANK_PLAYER,
									game_state->Squares[SquareNo].owner, 
									game_state->Squares[SquareNo].owner, TIK_SQUARE,
									SquareNo, NULL, 0, NULL);
					AI_State[player].num_actions++;
				}
			}
		}

		/* Remember who is getting jail cards.*/
		for(counter = 0; counter < MAX_DECK_TYPES; ++counter)
		{
			if(trade_point->jail_card_received[counter])
				card_received[counter] = cur_player;

			if(cur_trade_point->jail_card_received[counter])
				cur_card_received[counter] = cur_player;
		}
	}

	// Send jail cards now, if needed. 
	for(counter = 0; counter < MAX_DECK_TYPES; ++counter)
	{
		if(card_received[counter] != cur_card_received[counter])
		{
			if(card_received[counter] == RULE_NOBODY_PLAYER)
				card_received[counter] = game_state->Cards[counter].jailOwner;

			MESS_SendAction(ACTION_TRADE_ITEM, player, RULE_BANK_PLAYER,
						game_state->Cards[counter].jailOwner, card_received[counter], TIK_JAIL_CARD,
						counter, NULL, 0, NULL);
			AI_State[player].num_actions++;
		}
	}

	/* Now immunities.  - not implemented yet. */

	/* Done. */
    if(AI_SEND_TRADE_OFFER_TRADE)
    {   /* We want to offer the trade. */
	    AI_SendAction(ACTION_TRADE_EDITING_DONE, player, RULE_BANK_PLAYER,
					    1, AI_Glob.Player_Proposed_Trade, 0, 0, NULL, 0, NULL);
    }
    else
    {   /* We want people to accept. */
	    AI_SendAction(ACTION_TRADE_EDITING_DONE, player, RULE_BANK_PLAYER,
					    0, TRUE, 0, 0, NULL, 0, NULL);
    }

	AI_PLAYER_SENDING_TRADE = AI_Send_Trade_Sent;

return TRUE;	/* Did it - done that. */
}

/****************************************************************************************/
/*					AI_Get_Next_Player_Wanting_Cash										*/
/*																						*/
/*   Input:  AI_Trade_List_Record	*trade_point	The trade in question.				*/
/*	 Output: RULE_PlayerNumber	-  first player in the list who wants cash.				*/
/****************************************************************************************/

RULE_PlayerNumber	AI_Get_Next_Player_Wanting_Cash(AI_Trade_List_Record *trade_point)
{
	RULE_PlayerNumber  cur_player;

	for(cur_player = 0; cur_player < RULE_MAX_PLAYERS; ++cur_player, ++trade_point)
	{
		if(trade_point->cash_received != 0)
			return cur_player;
	}

return RULE_NOBODY_PLAYER;
}



/****************************************************************************************/
/*					AI_Should_Trade_For_Monopoly										*/
/*																						*/
/*   Input:  RULE_GameStatePointer	game_state											*/
/*			 RULE_PlayerNumber		player		Who should trade?						*/
/*	 Output: int  -  Either TRUE, meaing a monopoly trade has to be proposed,			*/
/*					 FALSE, meaning a monopoly trade should be avoided, or				*/
/*					 AI_Maybe, meaning a monopoly should be tried, but can wait for a   */
/*					 while.																*/
/*																						*/
/*		Determines whether it is time for player to propose a trade aiming at getting a */
/*  monopoly.  If a monopoly exists, and the player does not have one, returns TRUE.    */
/*  If a monopoly does not exist, but the player is far behind in income, or close in   */
/*  income but far in assets, then returns TRUE.  If the player has the highest income, */
/*  then should wait as long as possible.  If the player does not have the highest      */
/*  income, but is close, and is not far behind in liquid assets, then the player has   */
/*  some time, but should keep his/her eyes open.										*/
/****************************************************************************************/

int AI_Should_Trade_For_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player)
{
	float average_income[RULE_MAX_PLAYERS], largest_income = (float) 0.0;
	RULE_PlayerNumber	current_player, largest_player = RULE_NOBODY_PLAYER;
    RULE_PropertySet    properties_owned;

#ifdef AI_NO_TRADES
	return FALSE;
#endif 

	/* Are we in a monopoly situation? */
	if(AI_Exist_Monopoly(game_state))
	{	/* Yes we are.  Then trade for monopoly if we don't have one. */
		if( !AI_Player_Own_Monopoly(game_state, player, FALSE))
			return TRUE;

		return FALSE;   /* In monopoly situation and we have one. Don't need to
						   focus on monopoly trades. */
	}

	/* Are we still in the buying stage?  Then don't worry about monopolies yet. */
	if(AI_Get_Monopoly_Stage(game_state, player) == AI_STAGE_BUYING)
		return FALSE;

	/* No one has a monopoly.  Then figure out who has the best income.  If it is
	   us, postpone monopoly trades as long as possible (and hence we get
	   richer and richer. */

	for(current_player = 0; current_player < game_state->NumberOfPlayers; ++current_player)
	{
        properties_owned = RULE_PropertySetOwnedByPlayer(game_state, current_player);
		/* get average rent received */
		average_income[current_player] = AI_Average_Rent_Received(game_state, current_player, 0, FALSE, 1.0f,
                                                                  properties_owned);
		/* This is an average per player - hence multiply by number of players in game. */
		average_income[current_player] *= (game_state->NumberOfPlayers - 1);

		/* subtract from that average rent payed.*/
		average_income[current_player] -= (float) AI_Average_Rent_Payed(game_state, current_player, 0, TRUE, 1.0f);

		/* Only thing we did not take into account is the $200 Go square. */
		average_income[current_player] += game_state->GameOptions.passingGoAmount;

		if(average_income[current_player] >= largest_income)
		{
			largest_income = average_income[current_player];
			largest_player = current_player;
		}

	}

	if(largest_player != current_player)
	{
		/* We are not receiving the best income.  If we are within 20% of largest_player's income,
		   then we should wait for the monopoly we want.   Otherwise, trade for any
		   monopoly whatsoever, since the longer we wait, the lower our chances are. */
		if( (average_income[largest_player] - average_income[current_player]) / average_income[current_player]
			> (0.2f))
			return TRUE;
	}

	/* We are either the top earner, or near the top.  Hence we do not have a high priority
	   of getting a monopoly trade (ie. we can be picky, waiting only for the best.) */
return (AI_MAYBE);
}


/******************************************************************/
/*					AI_Find_Nonmonopoly_Player  				  */
/*																  */
/*   Input:	 RULE_GameStatePointer	game_state					  */
/*			 RULE_PlayerNumber	player   Who is searching?        */
/*   Output: RULE_PlayerNumber - player with most liquid assets   */
/*                  not owning a monopoly. RULE_NOBODY_PLAYER     */
/*                  otherwise.  Does not count BALTIC as a        */
/*                  monopoly.                                     */
/******************************************************************/

RULE_PlayerNumber  AI_Find_Nonmonopoly_Player(RULE_GameStatePointer game_state,
                                              RULE_PlayerNumber player)
{
    RULE_PlayerNumber cur_player, highest_player = RULE_NOBODY_PLAYER;
    long  liquid_assets, highest_assets = 0;

    for(cur_player = 0; cur_player < game_state->NumberOfPlayers; ++cur_player)
    {   
        /* Player still in the game? */
        if(game_state->Players[cur_player].currentSquare == SQ_OFF_BOARD)
            continue;

        /* Skip us. */
        if(cur_player == player)
            continue;

        /* Skip if owns a monopoly. */
        if(AI_Player_Own_Monopoly(game_state, cur_player, FALSE))
            continue;
            
        /* Rank the players based on liquid assets */
    	liquid_assets = AI_Get_Liquid_Assets(game_state, cur_player, FALSE, FALSE);
        if(liquid_assets > highest_assets)
        {
            highest_assets = liquid_assets;
            highest_player = cur_player;
        }
    }

return highest_player;
}


/******************************************************************/
/*					AI_Find_Free_Trade_Spot						  */
/*																  */
/*   Input:	 RULE_GameStatePointer	game_state					  */
/*			 RULE_PlayerNumber	player   Who wants to propose the */
/*										 trade?					  */
/*   Output: int   - -1 if no spot found, otherwise the			  */
/*			  AI_Data[player].time_last_trade[] entry which is 0. */
/*																  */
/*		Will find a 0 entry in time_last_trade which signifies    */
/*  that player can make a trade (hasn't made too many trades     */
/*  lately.)													  */
/******************************************************************/

int AI_Find_Free_Trade_Spot(RULE_GameStatePointer game_state, RULE_PlayerNumber player)
{
	int	counter;
	long	*last_trade_point;

	last_trade_point = AI_Data[player].time_last_trade;

	for(counter = 0; counter < AI_Data[player].max_trades; ++counter, ++last_trade_point)
	{
		if(*last_trade_point == 0)
			return counter;
	}

return -1;
}

/****************************************************************************************/
/*					AI_Should_Trade														*/
/*																						*/
/*   Input:  RULE_GameStatePointer	game_state											*/
/*			 RULE_PlayerNumber		player		Who should trade?						*/
/*	 Output: BOOL - TRUE if a trade should be proposed, FALSE otherwise.				*/
/*																						*/
/*		Determines whether it is time for player to propose a trade.  Decision is based */
/*  on whether a trade is in progress, the AI has a free trade spot (hasn't proposed    */
/*  too many trades in a short period of time), the importance of the trade, and        */
/*  the probability of the AI trading.													*/
/****************************************************************************************/

BOOL AI_Should_Trade(RULE_GameStatePointer game_state, RULE_PlayerNumber player, AI_Trade_Importance importance)
{

	if(AI_PLAYER_SENDING_TRADE || game_state->TradeInProgress ||
        (AI_Glob.Buy_Sell_Mort_Player != player && AI_Glob.Buy_Sell_Mort_Player != RULE_NOBODY_PLAYER))
		return FALSE;	/* Trade is happening, or an AI just stacked a trade
						   message.  Wait till it is resolved. */

	/* Find a free spot. */
	if(AI_Find_Free_Trade_Spot(game_state, player) == -1)
		return FALSE;	/* Proposed too many as it is. */

	/* How important is this trade? */
	if(AI_TRADE_SOMEWHAT_IMPORTANT & importance)
	{	/* Important - probably paying a debt.  Then go for it! */
		return TRUE;
	}

    /* Check if we should propose a monopoly give away. */
    if(AI_Should_Give_Away_Monopoly(game_state, player))
    {
        /* Different probability for giveaways. */
        if( ((float) rand())/ RAND_MAX <= AI_Data[player].propose_monopoly_give_away_probability)
            return TRUE;
    }

	/* Not that important.  Then randomly return TRUE based on the probability
	   of the AI sending a trade. */
	if(AI_Should_Trade_For_Monopoly(game_state, player) == TRUE)
	{	/* We MUST trade for a monopoly - hence increase our
		   probability of trading. */
		if( ((float) rand())/ RAND_MAX   <= AI_Data[player].monopoly_propose_trade_probability )
			return TRUE;
	}
	else
	{
		if( ((float) rand())/ RAND_MAX   <= AI_Data[player].propose_trade_probability )
			return TRUE;
	}


return FALSE;
}


/**********************************************************************************/
/*					AI_Add_Trade_Item											  */
/*																				  */
/*	 Input:  RULE_TradeItemKind		item_kind	What kind of item is being traded?*/
/*			 long					amount      How much/which/etc.				  */
/*			 RULE_PlayerNumber	  from_player	Who is in the giving mood?		  */
/*			 RULE_PlayerNumber	    to_player   Who is taking?				      */
/*																				  */
/*		Adds the item to the current trade list, as asked by NOTIFY_TRADE_ITEM.   */
/**********************************************************************************/

void AI_Add_Trade_Item(RULE_TradeItemKind item_kind, long amount, RULE_PlayerNumber from_player,
					   RULE_PlayerNumber to_player, RULE_PropertySet property_set)
{
	long	property_bit;
	RULE_PlayerNumber	current_player;
	AI_Trade_List_Record	*list_point = Current_Trade_List;
	int		count;
	AI_Immunity_Record  *immunity_point;

	/* What is the item we are adding to the list? */
	switch(item_kind)
	{
		case TIK_CASH:   /* MONEY....  it's a crime.... */
			Current_Trade_List[from_player].cash_given = amount;
			Current_Trade_List[to_player].cash_received = amount;
			break;

		case TIK_SQUARE: /* Property! */

			property_bit = RULE_PropertyToBitSet((RULE_SquareType) amount);
			if(to_player == RULE_NOBODY_PLAYER)
			{	
				/* Going to nobody - hence clear this property from the trade. */
				property_bit = -1l ^ property_bit;
				for(current_player = 0; current_player < RULE_MAX_PLAYERS; ++current_player)
				{
					Current_Trade_List[current_player].properties_received &= property_bit;
					Current_Trade_List[current_player].properties_given &= property_bit;
				}
			}
			else
			{
				Current_Trade_List[from_player].properties_given |= property_bit;
				Current_Trade_List[to_player].properties_received |= property_bit;
			}
			break;

		case TIK_JAIL_CARD:   /* Pardon from the prime minister. */
			if(to_player == RULE_NOBODY_PLAYER)
			{	
				/* Going to nobody - hence clear this jail card from trade. */
				for(current_player = 0; current_player < RULE_MAX_PLAYERS; ++current_player)
				{
					Current_Trade_List[current_player].jail_card_given[amount] = FALSE;
					Current_Trade_List[current_player].jail_card_received[amount] = FALSE;
				}
			}
			else
			{
				Current_Trade_List[from_player].jail_card_given[amount] = TRUE;
				Current_Trade_List[to_player].jail_card_received[amount] = TRUE;
			}
			break;

		case TIK_IMMUNITY:
		case TIK_FUTURE_RENT:
			/* First, see if this property set is identical to one in the list. */
			immunity_point = AI_Immunities_Futures_Traded;
			for(count = 0; count < RULE_MAX_COUNT_HIT_SETS; ++count, ++immunity_point)
			{
				if((immunity_point->count == 0) || (immunity_point->hitType != item_kind))
					continue;	/* empty entry, or not an immunity */

				if((immunity_point->properties == property_set) &&
					(immunity_point->toPlayer == to_player))
				{
					/* Found an identical set.  Then change its count to the
					   new count. */
					immunity_point->count = (unsigned char) amount;
					break;
				}
			}

			if(count != RULE_MAX_COUNT_HIT_SETS)
				break;  /* Already took care of it. just exit the function. */

			/* Did not find an identical set.  Hence a new immunity.  Find a free spot. */
			immunity_point = AI_Immunities_Futures_Traded;
			for(count = 0; count < RULE_MAX_COUNT_HIT_SETS; ++count, ++immunity_point)
			{
				if(immunity_point->count == 0)
					break;
			}

			/* Did we find a free spot? */
			if(count == RULE_MAX_COUNT_HIT_SETS)
				break;  /* Nope. */

			/* Add the immunity/future. */
			immunity_point->properties = property_set;
			immunity_point->fromPlayer = from_player;
			immunity_point->toPlayer = to_player;
			immunity_point->count = (unsigned char) amount;
			immunity_point->hitType = item_kind;
			break;
	}


	/* Normalize the cash given - cash received. */
	list_point = Current_Trade_List;
	for(current_player = 0; current_player < RULE_MAX_PLAYERS; ++current_player)
	{
		if(list_point->cash_given > list_point->cash_received)
		{
			list_point->cash_given -= list_point->cash_received;
			list_point->cash_received = 0;
		}
		else
		{
			list_point->cash_received -= list_point->cash_given;
			list_point->cash_given = 0;
		}
	}

return;
}


/******************************************************************************/
/*					AI_Update_State											  */
/*																			  */
/*   Input: RULE_GameStatePointer	game_state								  */
/*			AI_Trade_List_Record	*list		A trade.					  */
/*																			  */
/*		Goes through *list and changes game_state to reflect the trade having */
/*  taken place.  Right now, does not take into account immunities.			  */
/******************************************************************************/

void AI_Update_State(RULE_GameStatePointer game_state, AI_Trade_List_Record *list)
{
	RULE_PlayerNumber	current_player;
	RULE_SquareInfoRecord	*square_point;
	RULE_SquareType		square;

	/* go through the list - looking at each player's received properties. */
	for(current_player = 0; current_player < game_state->NumberOfPlayers; ++current_player, ++list)
	{
		square_point = game_state->Squares;
		for (square = 0; square < SQ_IN_JAIL; ++square, ++square_point)
		{	
			/* Are we receiving this property through the trade? */
			if(RULE_PropertyToBitSet(square) & list->properties_received)
			{	/* Yes.  Then change game_state to reflect that. */
				square_point->owner = current_player;
			}
		}

		game_state->Players[current_player].cash += list->cash_received - list->cash_given;

		/* Update jail cards. */
		if(list->jail_card_received[CHANCE_DECK])
			game_state->Cards[CHANCE_DECK].jailOwner = current_player;
		if(list->jail_card_received[COMMUNITY_DECK])
			game_state->Cards[COMMUNITY_DECK].jailOwner = current_player;

	}

return;
}

/******************************************************************************/
/*					AI_Calc_Trade_Property_Importance						  */
/*																			  */
/*   Input: RULE_GameStatePointer	game_state	game state before the trade	  */
/*			RULE_GameStatePointer   new_state   game state after the trade    */
/*			RULE_PlayerNumber		player		Who to consider?			  */
/*			RULE_PlayerNumber		ai_strat    Whose logic?				  */
/*			AI_Trade_List_Record	*trade_list	What is the trade?  		  */
/*          BOOL           giving_monopoly_away Are we giving a monopoly away?*/
/*                                  Use different monopoly value for that.    */
/*   Output: float  -  a relative number based on weights representing the    */
/*					   importance of the properties received versus those     */
/*					   given away.											  */
/*																			  */
/*		This routine is used to figure out how important are the properties   */
/*  we received and given away with respect to monopoly trades or monopolies  */
/*  received.  If a monopoly is received, Darzinskis' chart of monopoly       */
/*  importance versus current liquid assets is used to modify the importance  */
/*  of this monopoly (between 0.5 - 1.5 of normal value).  Weights are stored */
/*  in AI_Data of ai_strat player.											  */
/******************************************************************************/

float AI_Calc_Trade_Property_Importance(RULE_GameStatePointer game_state, RULE_GameStatePointer new_state,
										RULE_PlayerNumber player, RULE_PlayerNumber ai_strat,
										AI_Trade_List_Record *trade_list, BOOL giving_monopoly_away)
{
	RULE_PlayerNumber	counter;
	long	            liquid_assets[RULE_MAX_PLAYERS];
	double				importance = 0.0, importance_before, importance_after, importance_change;
	int					chart_index[RULE_MAX_PLAYERS];
    RULE_GameStateRecord    new_state_without_our_props;
	RULE_PropertySet    properties_given, player_props_received;
	RULE_PropertySet    properties_received[RULE_MAX_PLAYERS];
    long                monopolies_before, monopolies_after, player_monopoly_change, monopoly_change;
    double              monopoly_importance_total_before, monopoly_importance_total_after, monopoly_importance_change, change;
    double              mono_player_importance_change;
    double              total_monopoly_importance = 0;
    BOOL                monopoly_given = FALSE;
    int                 num_players_in_trade = 0;

	/* Get all the liquid assets of all players after the trade. */
	for(counter = 0; counter < game_state->NumberOfPlayers; ++counter)
	{
		liquid_assets[counter] = AI_Get_Liquid_Assets(new_state, counter, FALSE, FALSE);

		/* For monopoly evaluation, we want to look up Darzinskis chart to consider
		   relative monopoly importance depending on liquid assets available. */
		if(liquid_assets[counter] > AI_MAX_DARZINSKIS_CHART)
			chart_index[counter] = AI_MAX_DARZINSKIS_CHART / AI_DARZINSKIS_CHART_SEPARATION;
		else
			chart_index[counter] = liquid_assets[counter] / AI_DARZINSKIS_CHART_SEPARATION;
	}

    /* Create a new state, that includes all the properties received from all other players,
       except for us. */
    	/* Make a copy of the game state. */
	memcpy(&new_state_without_our_props, game_state, sizeof(RULE_GameStateRecord));

        /* Update the 'before the trade' game state with the trade excluding our properties. */
    properties_given = trade_list[player].properties_given;
    properties_given ^= -1;
    player_props_received = trade_list[player].properties_received;
    trade_list[player].properties_received = 0;

    for(counter = 0; counter < game_state->NumberOfPlayers; ++counter)
    {
        properties_received[counter] = trade_list[counter].properties_received;
        trade_list[counter].properties_received &= properties_given;
    }

    AI_Update_State(&new_state_without_our_props, trade_list);

        /* Restore the trade list. */
    for(counter = 0; counter < game_state->NumberOfPlayers; ++counter)
        trade_list[counter].properties_received = properties_received[counter];

    trade_list[player].properties_received = player_props_received;

    /* Find out the change of importance factors for each player, resulting from the properties
       we gave/received. */
    importance_before = AI_Find_Players_Property_Importance(game_state, player, ai_strat, chart_index[player],
                                                            &monopolies_before, &monopoly_importance_total_before);
    importance_after = AI_Find_Players_Property_Importance(new_state, player, ai_strat, chart_index[player],
                                                            &monopolies_after, &monopoly_importance_total_after);
    importance += importance_after - importance_before;
    player_monopoly_change = monopolies_after - monopolies_before;
    mono_player_importance_change = monopoly_importance_total_after - monopoly_importance_total_before;

	for(counter = 0; counter < game_state->NumberOfPlayers; ++counter)
	{
        if(counter == player)
            continue;   /* Did ourself already. */

        if(!AI_Player_Involved_In_Trade(trade_list, counter))
            continue;   /* not part of this trade. */

        num_players_in_trade++;

        importance_before = AI_Find_Players_Property_Importance(&new_state_without_our_props, counter, ai_strat, chart_index[counter],
                                                                &monopolies_before, &monopoly_importance_total_before);
        importance_after = AI_Find_Players_Property_Importance(new_state, counter, ai_strat, chart_index[counter],
                                                                &monopolies_after, &monopoly_importance_total_after);
        monopolies_before = AI_Get_Monopolies(game_state, counter, NULL, FALSE);
        monopoly_change = monopolies_after - monopolies_before;
        monopoly_importance_change = monopoly_importance_total_after - monopoly_importance_total_before;

        /* Find out the importance change without monopolies. */
        importance_change = importance_after - importance_before - monopoly_importance_change;

        /* If it is negative, use a special multiplier.  Ie. we are not so worried about
           the other player losing out in trade, only gaining */
        if(importance_change < 0)
            importance_change *= AI_Data[ai_strat].negative_prop_importance_change_multiplier;

        importance -= importance_change;

        /* In group trades, we might give properties that give 2+ other players a monopoly.
           However, what is important in a group trade is whether each player ends up with
           more monopolies at the end than they started with, and how many more. */
        if(monopoly_change > player_monopoly_change) /* Wait a sec... someone getting more monopolies than us! */
        {
            /* If we are giving another monopoly away, consider it as a 'second' monopoly.  */
            if(giving_monopoly_away)
            {
                change = (monopoly_change - player_monopoly_change) * AI_Data[ai_strat].giving_monopoly_importance * (1.0 + 0.75) / 2;
                importance -= change;
                monopoly_importance_change = 0;   /* Don't count the extra monopoly twice. */
            }
            else
            {
                change = (monopoly_change - player_monopoly_change) * AI_Data[ai_strat].monopoly_received_importance * (1.0 + 0.75) / 2;
                importance -= change;
                monopoly_importance_change -= change;   /* Don't count the extra monopoly twice. */
            }
        }

        /* Subtract monopoly data - don't want to overcount how many monopolies are given. */
        total_monopoly_importance += monopoly_importance_change;
    }

    /* We have not dealt with monopoly importances yet, except for those players getting too
       much.  Hence now consider the average monopoly importance. */
    importance -= total_monopoly_importance / num_players_in_trade;

return ((float) importance);
}

/******************************************************************************/
/*					AI_Find_Players_Property_Importance						  */
/*																			  */
/*   Input: RULE_GameStatePointer	game_state	game state before the trade	  */
/*			RULE_PlayerNumber		player		Who to consider?			  */
/*			RULE_PlayerNumber		ai_strat    Whose logic?				  */
/*			int chart_index         index into Darzinskis chart for player    */
/*   Output: double  -  a relative number based on weights representing the   */
/*					   importance of the properties owned by player.          */
/*																			  */
/*		This routine is used to figure out how important are the properties   */
/*  owned by player.  Darzinskis' chart of monopoly importance versus current */
/*  liquid assets is used to modify the importance of monopolies.   Weights   */
/*  are stored in AI_Data of ai_strat player.					              */
/******************************************************************************/

double AI_Find_Players_Property_Importance(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
                                           RULE_PlayerNumber ai_strat, int chart_index,
                                           long *monopolies, double *monopoly_importance)
{
	RULE_SquareGroups	monopoly_group[SG_PARK_PLACE + 1];
    RULE_SquareGroups   cur_group;
    RULE_SquareType     square;
    double              importance = 0.0;
    int                 num_cash_cows, num_unowned_squares, counter;
    double              monopoly_factor;
	long	liquid_assets;
	RULE_PlayerNumber	player_list[RULE_MAX_PLAYERS], backup_list[RULE_MAX_PLAYERS];
	int					num_players_in_list = 0, smallest_trade, num_veto_monopolies;
	RULE_PropertySet	properties[RULE_MAX_PLAYERS];
    BOOL                first_property = TRUE;
	int					ai_level = game_state->Players[ai_strat].AIPlayerLevel;

    *monopolies = 0;
    *monopoly_importance = 0.0;

    /* figure out the importance factor for each monopoly. */
    for(cur_group = 0; cur_group <= SG_PARK_PLACE; ++cur_group)
    {
        square = AI_Expensive_Monopoly_Square[cur_group];

        if(AI_Is_Monopoly(game_state, square))
        {   /* Do we own this monopoly? */
			// Get the liquid asset dependant factor for this group. 
			if(ai_level != 3)
				monopoly_factor = 1.0;
			else
			{
				monopoly_factor = AI_Monopoly_To_Assets_Importance[cur_group][chart_index];
				monopoly_factor = exp(monopoly_factor * log(2));  /* calc 2 ^ x */
				monopoly_factor = (monopoly_factor + 3.0)/(monopoly_factor + 4.0);
			}

            if(game_state->Squares[square].owner == player)
            {   /* Yes.  Then add importance factor. */
                *monopolies += 1;
                if(cur_group == SG_MEDITERRANEAN_AVENUE)	/* Can't win with mediterranean */
                {
					importance += AI_Data[ai_strat].property_allow_trade_importance;  
                    *monopoly_importance += AI_Data[ai_strat].property_allow_trade_importance;
                }
				else
				{
                    if(cur_group == SG_MEDITERRANEAN_AVENUE)
                    {
					    importance += AI_Data[ai_strat].baltic_received_importance * monopoly_factor;
                        *monopoly_importance += AI_Data[ai_strat].baltic_received_importance * monopoly_factor;
                    }
                    else
                    {
					    importance += AI_Data[ai_strat].monopoly_received_importance * monopoly_factor;
                        *monopoly_importance += AI_Data[ai_strat].monopoly_received_importance * monopoly_factor;
                    }
				}
            }
        }

        /* See if we own squares on this monopoly, with no one else owning them. */
		num_unowned_squares = AI_Is_Possible_Monopoly(game_state, player, square);

		if(num_unowned_squares != -1)
		{	/* Player can get a monopoly if s/he were to step on the unowned squares! */

			// A different factor for less important properties.
			if(ai_level != 3)
				monopoly_factor = 1.0;
			else
			{
				monopoly_factor = AI_Monopoly_To_Assets_Importance[cur_group][chart_index];
				monopoly_factor = exp(monopoly_factor * log(2));  /* calc 2 ^ x */
				monopoly_factor = (monopoly_factor + 2.0)/(monopoly_factor + 3.0);
			}

			if(num_unowned_squares == 1)
			{
				if((square == SQ_MEDITERRANEAN_AVENUE) || (square == SQ_BALTIC_AVENUE) ||
				   (square == SQ_PARK_PLACE)  ||  (square == SQ_BOARDWALK))
					importance += AI_Data[ai_strat].property_two_unowned_importance * monopoly_factor;
				else
					importance += AI_Data[ai_strat].property_one_unowned_importance * monopoly_factor;
			}

			if(num_unowned_squares == 2)
				importance += AI_Data[ai_strat].property_two_unowned_importance * monopoly_factor;
		}
    }

    /* First, rank the monopolies based on player's liquid assets. */
	liquid_assets = AI_Get_Liquid_Assets(game_state, player, FALSE, FALSE);
	AI_Order_Monopoly_Importance(liquid_assets, monopoly_group, AI_Sort_Descending);

    /* Now find players we can trade with. */
	for(counter = 0; counter < game_state->NumberOfPlayers; ++counter)
	{
		properties[counter] = RULE_PropertySetOwnedByPlayer(game_state, (RULE_PlayerNumber) counter);

        /* need a list of potential players we can trade with. */
		if(counter == player)
			continue;

		if(AI_Player_Own_Monopoly(game_state, (RULE_PlayerNumber) counter, FALSE))
			continue; /* Owns a monopoly - little incentive to trade with us. */
	
		if(game_state->Players[counter].currentSquare == SQ_OFF_BOARD)
			continue;	/* gone bankrupt! */

		player_list[num_players_in_list++] = counter;
	}

	memcpy(backup_list, player_list, sizeof(player_list));

    /* Now figure out cash cow importance. */
	num_cash_cows = AI_Number_Railroads_Utils_Owned(game_state, player, SG_RAILROAD, FALSE, 
                                                    properties[player]);
    if(num_cash_cows != 0)
        importance += AI_Data[ai_strat].railroad_importance[num_cash_cows - 1];

	num_cash_cows = AI_Number_Railroads_Utils_Owned(game_state, player, SG_UTILITY, FALSE,
                                                    properties[player]);
    if(num_cash_cows != 0)
        importance += AI_Data[ai_strat].utility_importance[num_cash_cows - 1];

    /* Figure out whether we have a trade opportunity for each monopoly,
       ranked based on our liquid assets. The first trade opportunity is worth
       normal value.  All subsequent trade opportunities are worth much less.
       Skip Baltic, as it is too weak a monopoly to consider important. */

    first_property = TRUE;
    for(cur_group = 0; cur_group < SG_PARK_PLACE; ++cur_group)
    {
		smallest_trade = AI_Find_Smallest_Monopoly_Trade(player, 
                            AI_Get_Monopoly_Set(AI_Expensive_Monopoly_Square[monopoly_group[cur_group]]),
                            player_list, num_players_in_list, properties);

        if(smallest_trade != 0)
        {   /* Found a trade property. */
			if(ai_level != 3)
				monopoly_factor = 1.0;
			else
			{
				monopoly_factor = AI_Monopoly_To_Assets_Importance[cur_group][chart_index];
				monopoly_factor = exp(monopoly_factor * log(2));  /* calc 2 ^ x */
				monopoly_factor = (monopoly_factor + 1.0)/(monopoly_factor + 2.0);
			}

            if(first_property)
            {
                importance += AI_Data[ai_strat].property_allow_trade_importance * monopoly_factor;
                first_property = FALSE;
            }
            else
                importance += AI_Data[ai_strat].property_allow_more_trade_importance * monopoly_factor;
        }

    	memcpy(player_list, backup_list, sizeof(player_list));
    }

    /* Finally, count the number of monopolies we have a veto over (ie. we owe at least one
       property of). */
    num_veto_monopolies = AI_Num_Veto_Monopolies(properties[player]);
    importance += AI_Data[ai_strat].monopoly_veto_importance[num_veto_monopolies];
    
return (importance);
}

/******************************************************************************/
/*					AI_Num_Veto_Monopolies                                    */
/*																			  */
/*   Input: RULE_PropertySet    properties_owned                              */
/*   Output: int - number of monopolies that properties_owned contains at     */
/*                 least one property of.                                     */
/******************************************************************************/

int AI_Num_Veto_Monopolies(RULE_PropertySet properties_owned)
{
    int count = 0;
    RULE_SquareGroups cur_group;

    for(cur_group = SG_ORIENTAL_AVENUE; cur_group <= SG_PARK_PLACE; ++cur_group)
    {
        if(AI_Get_Monopoly_Set(AI_Expensive_Monopoly_Square[cur_group]) & properties_owned)
            count++;
    }

return count;
}

/******************************************************************************/
/*					AI_Mortgage_Negative_Cash_Players  						  */
/*																			  */
/*   Input: RULE_GameStatePointer	game_state	game state before the trade	  */
/*																			  */
/*      This function goes through the players in game_state and mortgages/   */
/* sell houses until the player does not have a negative cash balance.  No    */
/* action messages are sent, but game_state is changed directly.              */
/******************************************************************************/

void AI_Mortgage_Negative_Cash_Players(RULE_GameStatePointer game_state)
{
    RULE_PlayerNumber   cur_player;

    for(cur_player = 0; cur_player < game_state->NumberOfPlayers; ++cur_player)
    {
        if(game_state->Players[cur_player].currentSquare == SQ_OFF_BOARD)
            continue; /* player gone from the game. */

            /* Does this player have negative cash? */
        while(game_state->Players[cur_player].cash < 0)
        {
            if(!AI_Mortgage_Worst_Property(game_state, cur_player, TRUE, TRUE, TRUE))
                break;
        } 
    }

return;
}

/******************************************************************************/
/*					AI_Evaluate_Trade										  */
/*																			  */
/*   Input: RULE_GameStatePointer	game_state								  */
/*			RULE_PlayerNumber		player		Evaluate trade for who?		  */
/*			RULE_PlayerNumber		ai_strat	According to whose logic?	  */
/*			AI_Trade_List_Record *trade_list	What is the trade?			  */
/*	 Output: float  - > 0 if a good trade, < 0 if a bad trade.			      */
/*																			  */
/*		Evaluate's the trade for player and returns a float representing      */
/*  how good or bad the trade is.  The lower the evaluation, the worse the    */
/*  trade.  The higher the number, the better the trade.  0 is neutral.       */
/*  Uses 'ai_strat' settings in evaluating the importance					  */
/*  of the trade.  Considers the chances of winning							  */
/*  before and after, the importance of the properties and the change in total*/
/*  worth.  Takes all these numbers, multiplies by weights.					  */
/******************************************************************************/

float	AI_Evaluate_Trade(RULE_GameStatePointer game_state, RULE_PlayerNumber player, 
						  RULE_PlayerNumber ai_strat, AI_Trade_List_Record *trade_list)
{
	float	chances_before, chances_after;
	RULE_GameStateRecord	new_state, old_state;
	long	worth_before, worth_after;
	float	evaluation;
	float   trade_property_importance;
	RULE_SquareType	bought_property;
	RULE_PlayerNumber	purchase_player;

	/*  Create a new game state after the trade. */
	memcpy(&new_state, game_state, sizeof( RULE_GameStateRecord));
	memcpy(&old_state, game_state, sizeof( RULE_GameStateRecord));
	AI_Update_State(&new_state, trade_list);

    /* Go through all the players and mortgage/sell properties to meet the 
       cash demands of this trade. */
    AI_Mortgage_Negative_Cash_Players(&new_state);

	/* If a player is currently buying a property, act as if that player
	   has already bought it. */
	bought_property	= AI_Property_Being_Bought(game_state, &purchase_player);
	if(bought_property != SQ_MAX_SQUARE_TYPES)
	{
		new_state.Squares[bought_property].owner = purchase_player;
		old_state.Squares[bought_property].owner = purchase_player;
	}

	/*  What are our chances of winning before we trade? */
	chances_before =  AI_Evaluate_Winning_Chances(&old_state, player, ai_strat, NULL);

	/*  Now our chances of winning after the trade? */
	chances_after = AI_Evaluate_Winning_Chances(&new_state, player, ai_strat, NULL);

	/*  Did our chance of winning drop too much to make any other
	    factors not worth considering? */
	if((chances_before - chances_after) > AI_Data[ai_strat].chances_threshold)
		return ((float) -50.0);

	/*  Evaluate the cash logistics of this trade. (ie. how much
		are we worth before and after.) */
	worth_before = AI_Get_Total_Worth_With_Factors(&old_state, player, ai_strat);
	worth_after = AI_Get_Total_Worth_With_Factors(&new_state, player, ai_strat);

	/*  Will we go bankrupt if we accept? */
	if(AI_Get_Liquid_Assets(&new_state, player, TRUE, TRUE) <= 0)
		return ((float) -50);

		/* Subtract/add value of jail card. */
	if(old_state.Cards[CHANCE_DECK].jailOwner == player)
		worth_before += AI_JAIL_CARD_VALUE;

	if(old_state.Cards[COMMUNITY_DECK].jailOwner == player)
		worth_before += AI_JAIL_CARD_VALUE;

	if(new_state.Cards[CHANCE_DECK].jailOwner == player)
		worth_after += AI_JAIL_CARD_VALUE;

	if(new_state.Cards[COMMUNITY_DECK].jailOwner == player)
		worth_after += AI_JAIL_CARD_VALUE;


	/*  Calculate the effect of giving away or receiving important
	    properties.  */

	trade_property_importance = AI_Calc_Trade_Property_Importance(&old_state, &new_state,
										player, ai_strat, trade_list, FALSE);

	evaluation = (chances_after - chances_before) * AI_Data[ai_strat].chances_factor +
				 (worth_after - worth_before) * AI_Data[ai_strat].cash_factor +
				 trade_property_importance * AI_Data[ai_strat].trade_importance_factor;

/*** TEST CODE ***/
    AI_TEST_chances_change = chances_after - chances_before;
    AI_TEST_worth_change = (float) (worth_after - worth_before);
    AI_TEST_prop_importance = trade_property_importance;
    AI_TEST_evaluation = evaluation;
/*** TEST CODE ***/

return evaluation;
}

/******************************************************************************/
/*					AI_Evaluate_Trade_For_Player_List						  */
/*																			  */
/*   Input: RULE_GameStatePointer	game_state								  */
/*			RULE_PlayerNumber		*player_list   Evaluate trade for who?	  */
/*          int                     list_size      How many in list?          */
/*			RULE_PlayerNumber		ai_strat	According to whose logic?	  */
/*			AI_Trade_List_Record *trade_list	What is the trade?			  */
/*          float                  *evaluation   Where to store the results.  */
/*          BOOL      giving_monopoly_for_cash  Are we giving a monopoly away?*/
/*																			  */
/*		Evaluate's the trade for each player in the list.  See                */
/*  AI_Evaluate_Trade.                                                        */
/******************************************************************************/

void	AI_Evaluate_Trade_Player_List(RULE_GameStatePointer game_state, RULE_PlayerNumber *player_list, 
                          int list_size, RULE_PlayerNumber ai_strat, AI_Trade_List_Record *trade_list,
                          float *evaluation, BOOL giving_monopoly_for_cash)
{
	float	chances_before, chances_after;
	RULE_GameStateRecord	new_state, old_state;
	long	worth_before, worth_after;
	float   trade_property_importance;
	RULE_SquareType	bought_property;
	RULE_PlayerNumber	purchase_player, cur_player;
    float   player_chances_before[RULE_MAX_PLAYERS], player_chances_after[RULE_MAX_PLAYERS];
    int     index;

   	/*  Create a new game state after the trade. */
	memcpy(&new_state, game_state, sizeof( RULE_GameStateRecord));
	memcpy(&old_state, game_state, sizeof( RULE_GameStateRecord));
	AI_Update_State(&new_state, trade_list);

    /* Go through all the players and mortgage/sell properties to meet the 
       cash demands of this trade. */
    AI_Mortgage_Negative_Cash_Players(&new_state);

	/* If a player is currently buying a property, act as if that player
	   has already bought it. */
	bought_property	= AI_Property_Being_Bought(game_state, &purchase_player);
	if(bought_property != SQ_MAX_SQUARE_TYPES)
	{
		new_state.Squares[bought_property].owner = purchase_player;
		old_state.Squares[bought_property].owner = purchase_player;
	}

	/*  What are our chances of winning before we trade? */
	AI_Evaluate_Winning_Chances(&old_state, RULE_NOBODY_PLAYER, ai_strat, player_chances_before);

	/*  Now our chances of winning after the trade? */
	AI_Evaluate_Winning_Chances(&new_state, RULE_NOBODY_PLAYER, ai_strat, player_chances_after);

    /*  For each player, calculate their chances of winning. */
    for(index = 0; index < list_size; ++index)
    {
        cur_player = player_list[index];

        chances_before = player_chances_before[cur_player];
        chances_after = player_chances_after[cur_player];

	    /*  Did our chance of winning drop too much to make any other
	        factors not worth considering? */
	    if((chances_before - chances_after) > AI_Data[ai_strat].chances_threshold)
        {
            evaluation[index] = (float) -50.0;
            continue;
        }

	    /*  Evaluate the cash logistics of this trade. (ie. how much
		    are we worth before and after.) */
	    worth_before = AI_Get_Total_Worth_With_Factors(&old_state, cur_player, ai_strat);
	    worth_after = AI_Get_Total_Worth_With_Factors(&new_state, cur_player, ai_strat);

	    /*  Will we go bankrupt if we accept? */
	    if(AI_Get_Liquid_Assets(&new_state, cur_player, TRUE, TRUE) <= 0)
        {
            evaluation[index] = (float) -50.0;
            continue;
        }

		    /* Subtract/add value of jail card. */
	    if(old_state.Cards[CHANCE_DECK].jailOwner == cur_player)
		    worth_before += AI_JAIL_CARD_VALUE;

	    if(old_state.Cards[COMMUNITY_DECK].jailOwner == cur_player)
		    worth_before += AI_JAIL_CARD_VALUE;

	    if(new_state.Cards[CHANCE_DECK].jailOwner == cur_player)
		    worth_after += AI_JAIL_CARD_VALUE;

	    if(new_state.Cards[COMMUNITY_DECK].jailOwner == cur_player)
		    worth_after += AI_JAIL_CARD_VALUE;

	    /*  Calculate the effect of giving away or receiving important
	        properties.  */

	    trade_property_importance = AI_Calc_Trade_Property_Importance(&old_state, &new_state,
										    cur_player, ai_strat, trade_list, giving_monopoly_for_cash);

	    evaluation[index] = (chances_after - chances_before) * AI_Data[ai_strat].chances_factor +
				         (worth_after - worth_before) * AI_Data[ai_strat].cash_factor +
				         trade_property_importance * AI_Data[ai_strat].trade_importance_factor;
    }

return;
}


/******************************************************************************/
/*					AI_Evaluate_Winning_Chances								  */
/*																			  */
/*   Input: RULE_GameStatePointer	game_state								  */
/*			RULE_PlayerNumber		player			 Whose chances?			  */
/*			RULE_PlayerNumber		ai_strat		 Whose logic?			  */
/*          float          *save_player_chance   Optional pointer to array    */
/*                                          where to save each player chance. */
/*                                          NULL if don't want to.            */
/*	 Output:  float  -  a relative number between 0 and 1 giving a rough	  */
/*					  idea of odds of player winning						  */
/*																			  */
/*		Calculates the odds of 'player' winning.  This number is based on the */
/*  net income of 'player' if 'player' was to spend all liquid assets on      */
/*  building houses.  Uses ai_strat settings to evaluate it.  Note: this      */
/*  function will ignore cash cows, as cash cows' high rent makes their       */
/*  evaluation too high (since cash cows do not have the potential of         */
/*  becoming really good monopolies - too hard to win with cash cows.		  */
/******************************************************************************/

float AI_Evaluate_Winning_Chances(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
								  RULE_PlayerNumber	ai_strat, float *save_player_chance)
{
	float hold_player_chance[RULE_MAX_PLAYERS], total_absolute_chance = (float) 0.0, normalize_constant = (float) 0.0;
    float               multiplier, *player_chance, multiplier2;
	RULE_PlayerNumber	current_player;
	long				liquid_assets;
	RULE_GameStateRecord	new_state;
    RULE_PropertySet    properties_owned;
    BOOL                housing_shortage;
	
    /* First, should we save the player chance or not? */
    if(save_player_chance != NULL)
        player_chance = save_player_chance;
    else
        player_chance = hold_player_chance;

	/* Create a copy of game_state.  */
	memcpy(&new_state, game_state, sizeof(RULE_GameStateRecord));

	/* Reset the player_chance array. */
	memset(player_chance, 0, sizeof(float) * RULE_MAX_PLAYERS);

	/* Go through all the players and evaluate their income. We want to assume they will
		spend all their liquid assets in buying houses, unmortgaging monopolies,
	    and buying more houses. */
		
	/* First, need to unmortgage all the properties that give monopolies and
	   build them up one at a time.  */
	for(current_player = 0; current_player < new_state.NumberOfPlayers; ++current_player)
	{
        if(game_state->Players[current_player].currentSquare == SQ_OFF_BOARD)
            continue;

        housing_shortage = AI_Housing_Shortage(game_state, AI_CRITICAL_HOUSING_LEVEL);
		do
		{
			/* First, buy houses. */
			if(!housing_shortage)
			{
				/* Keep buying houses as long as we can.  Note: we assume enough houses
				exist for each player to buy houses for their monopolies */
				while(AI_Hypothetical_Buy_Houses(&new_state, current_player) )  ;
			}		

			/* Keep unmortgaging monopolies. */
		} while(AI_Unmortgage_Property(&new_state, current_player, TRUE, TRUE) != 0);
	}

	/*  Now unmortgage all the other properties. */
	for(current_player = 0; current_player < new_state.NumberOfPlayers; ++current_player)
	{
        if(game_state->Players[current_player].currentSquare == SQ_OFF_BOARD)
            continue;

        while(AI_Unmortgage_Property(&new_state, current_player, FALSE, TRUE) != 0);
	}

    /* Finally, mortgage as long as we have negative cash. */
	for(current_player = 0; current_player < new_state.NumberOfPlayers; ++current_player)
	{
        if(game_state->Players[current_player].currentSquare == SQ_OFF_BOARD)
            continue;

        while(new_state.Players[current_player].cash < 0)
        {
            if(!AI_Mortgage_Worst_Property(&new_state, current_player, FALSE, FALSE, TRUE))
                if(!AI_Mortgage_Worst_Property(&new_state, current_player, FALSE, TRUE, TRUE))
                    if(!AI_Mortgage_Worst_Property(&new_state, current_player, TRUE, TRUE, TRUE))
                        break;
        }
	}

		/* Now get the rents. */
	for(current_player = 0; current_player < new_state.NumberOfPlayers; ++current_player)
	{
        properties_owned = RULE_PropertySetOwnedByPlayer(game_state, current_player);
			/* get average rent received */
		player_chance[current_player] = AI_Average_Rent_Received(&new_state, current_player, 0, FALSE, 0.07f,
                                                                 properties_owned);
			/* This is an average per player - hence multiply by number of players in game. */
		player_chance[current_player] *= (new_state.NumberOfPlayers - 1);

			/* subtract from that average rent payed.*/
		player_chance[current_player] -= (float) AI_Average_Rent_Payed(&new_state, current_player, 0, TRUE, 0.07f);

			/* Only thing we did not take into account is the $200 Go square. */
		player_chance[current_player] += game_state->GameOptions.passingGoAmount;

			/* Cash left over should also affect our winning chances.  It allows us to buy
			   better monopolies through trades, allows buying new property and building
			   houses.  */
		liquid_assets = AI_Get_Liquid_Assets(&new_state, current_player, FALSE, FALSE);
		switch(AI_Get_Monopoly_Stage(&new_state, current_player))
		{
			case AI_STAGE_BUYING:
			case AI_STAGE_NO_MONOPOLIES:
                /* How many unowned properties are there? */
                multiplier = (float) AI_Num_Properties_Left_To_Buy(game_state);
                multiplier /= (float) AI_NUMBER_MONOPOLY_SQUARES;
                multiplier = AI_Data[ai_strat].buying_stage_cash_multiplier * multiplier +
                             AI_Data[ai_strat].no_mono_stage_cash_multiplier * (((float) 1.0) - multiplier);

				//  The more assets we have, the smaller the multiplier should be.
				multiplier2 = (float) (1.0 / exp(log(AI_Data[ai_strat].cash_liquid_assets_dependance) * liquid_assets / 1000));
				if(multiplier2 > 1.0f)
					multiplier2 = 1.0f;
	
				if(multiplier2 < 0.25f)
					multiplier2 = 0.25f;

				multiplier *= multiplier2;
				player_chance[current_player] += multiplier * liquid_assets;
				break;

			case AI_STAGE_MONOPOLIES_NOT_OWN_ONE:
				player_chance[current_player] += AI_Data[ai_strat].mono_not_owned_stage_cash_multiplier * liquid_assets;
				break;

			case AI_STAGE_MONOPOLIES_OWN_ONE:
				player_chance[current_player] += AI_Data[ai_strat].mono_stage_cash_multiplier * liquid_assets;
				break;
		};


		if(player_chance[current_player] > 0)
			total_absolute_chance += player_chance[current_player];
	}

	/*  Now the player with the largest income should win.  Figure out the % for each player,
    on an exponential curve. */

	if(total_absolute_chance == 0)
		total_absolute_chance = (float) 1.0;

	/*  Now get everyone on the exponential curve.  */
	for(current_player = 0; current_player < new_state.NumberOfPlayers; ++current_player)
	{
		player_chance[current_player] /= total_absolute_chance;
		player_chance[current_player] = (float) exp(player_chance[current_player]); 
		normalize_constant += player_chance[current_player];
	}

	/* Normalize our chance between 0 and 1. */
    for(current_player = 0; current_player < new_state.NumberOfPlayers; ++current_player)
	    player_chance[current_player] /= normalize_constant;

    /* Don't return a 'player' value since player may be undefined if save_player_chance
       is given. */
    if(save_player_chance)
        return (float) 0.0;

return (player_chance[player]);
}



/******************************************************************************/
/*					AI_Reset_Trade_List										  */
/*																			  */
/*		Resets Current_Trade_List.  Called whenever NOTIFY_TRADE_STARTED is   */
/*  called.																	  */
/******************************************************************************/


void AI_Reset_Trade_List(void)
{
	memset(Current_Trade_List, 0, sizeof(AI_Trade_List_Record) * RULE_MAX_PLAYERS);
	memset(AI_Immunities_Futures_Traded, 0, sizeof(AI_Immunities_Futures_Traded));

return;
}

/************************************************************************************/
/*					AI_Is_Possible_Monopoly        									*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*		    RULE_PlayerNumber	   player      Check for who owning the monopoly?   */
/*			RULE_SquareType    square		Which monopoly are we interested in?    */
/*	 Output:  int   -     Number of properties that are not owned on monopoly if    */
/*						  player would obtain a monopoly when getting them.   -1    */
/*						  if some properties of monopoly are owned by some other    */
/*						  player.  Used to determine if we have a chance at getting */
/*						  this monopoly by stepping on the unowned squares.         */
/************************************************************************************/

int AI_Is_Possible_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber owner, RULE_SquareType Square)
{
	RULE_SquareType		SquareNo, end_square;
	RULE_SquareInfoRecord  *square_point;
	RULE_SquareGroups	group = RULE_SquarePredefinedInfo[Square].group;
	int					num_unowned_props = 0;

    SquareNo = AI_Group_Start(group);
    end_square = AI_Group_End(group);
    square_point = &game_state->Squares[SquareNo];

	for(; SquareNo < SQ_IN_JAIL;SquareNo++, square_point++)
	{
		if((square_point->owner != owner) && (RULE_SquarePredefinedInfo[SquareNo].group == group))
		{
			if(square_point->owner != RULE_NOBODY_PLAYER)
				return -1;
			else
				num_unowned_props++;
		}
	}

return num_unowned_props;
}

/************************************************************************************/
/*					AI_Trade_Is_Proper	        									*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*			AI_Trade_List_Record   *Proposing_trade_list   The trade list.  		*/
/*	 Output:  BOOL -  TRUE if the trade satisfies the rules for a proper trade.	    */
/*					  FALSE otherwise.												*/
/*																				    */
/*		This function determines whether the trade Proposing_trade_list is a proper */
/* trade that can be proposed.														*/
/************************************************************************************/

BOOL AI_Trade_Is_Proper(RULE_GameStatePointer game_state, AI_Trade_List_Record *Proposing_trade_list)
{
	RULE_PlayerNumber	cur_player;
	BOOL				given_something, received_something, someone_in_trade = FALSE;
	AI_Trade_List_Record *list_point;
	int					index;
	AI_Immunity_Record  *immunity_point;

	/* Go through all the players. */
	list_point = Proposing_trade_list;
	for(cur_player = 0; cur_player < game_state->NumberOfPlayers; ++cur_player, ++list_point)
	{
		/* Is this player involved in the trade? */
		if(!AI_Player_Involved_In_Trade(Proposing_trade_list, cur_player))
			continue;

		/* Remember that a player is in this trade. */
		someone_in_trade = TRUE;

		/* Make sure player is both getting and giving something. */
		given_something = received_something = FALSE;

			/* First properties and cash. */
		if(list_point->properties_given || list_point->cash_given)
			given_something = TRUE;

		if(list_point->properties_received || list_point->cash_received)
			received_something = TRUE;

			/* Jail cards. */
		for(index = 0; index < MAX_DECK_TYPES; ++index)
		{
			if(list_point->jail_card_received[index])
				received_something = TRUE;

			if(list_point->jail_card_given[index])
				given_something = TRUE;
		}

		/* Now immunities and futures. */
		immunity_point = AI_Immunities_Futures_Traded;
		for(index = 0; index < RULE_MAX_COUNT_HIT_SETS; ++index, ++immunity_point)
		{
			if(immunity_point->count == 0)
				continue;	/* empty entry */

			if(immunity_point->fromPlayer == cur_player)
				given_something = TRUE;

			if(immunity_point->toPlayer == cur_player)
				received_something = TRUE;
		}

		/* Check if player gave and got. */
		if(!given_something || !received_something)
			return FALSE;
	}

	// Is this an empty trade?  If yes, don't send it. 
	if(!someone_in_trade)
		return FALSE;

return TRUE;	/* Everything checks out. */
}

/************************************************************************************/
/*					AI_Make_Trade_Proper	        							    */
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*			AI_Trade_List_Record   *Proposing_trade_list   The trade list.   		*/
/*																				    */
/*      This function tries to make sure the trade is proper, by making everyone    */
/* give and get something.                                                          */
/************************************************************************************/

void AI_Make_Trade_Proper(RULE_GameStatePointer game_state, AI_Trade_List_Record *Proposing_trade_list)
{
	RULE_PlayerNumber	cur_player;
	BOOL				given_something, received_something;
	AI_Trade_List_Record *list_point;
	int					index;
    RULE_PlayerNumber   dollar_player = RULE_NOBODY_PLAYER;
    int                 dollar_player_needs_to_give_get = 0;
	AI_Immunity_Record  *immunity_point;

	/* Go through all the players. */
	list_point = Proposing_trade_list;
	for(cur_player = 0; cur_player < game_state->NumberOfPlayers; ++cur_player, ++list_point)
	{
		/* Is this player involved in the trade? */
		if(!AI_Player_Involved_In_Trade(Proposing_trade_list, cur_player))
			continue;

		/* Make sure player is both getting and giving something. */
		given_something = received_something = FALSE;

			/* First properties and cash. */
		if(list_point->properties_given || list_point->cash_given)
			given_something = TRUE;

		if(list_point->properties_received || list_point->cash_received)
			received_something = TRUE;

			/* Jail cards. */
		for(index = 0; index < MAX_DECK_TYPES; ++index)
		{
			if(list_point->jail_card_received[index])
				received_something = TRUE;

			if(list_point->jail_card_given[index])
				given_something = TRUE;
		}

		/* Now immunities and futures. */
		immunity_point = AI_Immunities_Futures_Traded;
		for(index = 0; index < RULE_MAX_COUNT_HIT_SETS; ++index, ++immunity_point)
		{
			if(immunity_point->count == 0)
				continue;	/* empty entry */

			if(immunity_point->fromPlayer == cur_player)
				given_something = TRUE;

			if(immunity_point->toPlayer == cur_player)
				received_something = TRUE;
		}

		/* Check if player gave and got. */
        if(!given_something)
        {
            if(dollar_player == RULE_NOBODY_PLAYER)
                dollar_player_needs_to_give_get = 1;
            else
            {
                Proposing_trade_list[dollar_player].cash_received += 1;
                list_point->cash_given += 1; /* Give a dollar. */
            }
        }
        
		if(!received_something)
        {
            if(dollar_player == RULE_NOBODY_PLAYER)
                dollar_player_needs_to_give_get = 2;
            else
            {
                Proposing_trade_list[dollar_player].cash_given += 1;
                list_point->cash_received += 1; /* Give a dollar. */
            }
        }

        if(dollar_player == RULE_NOBODY_PLAYER)
            dollar_player = cur_player;
        else if(dollar_player_needs_to_give_get)
        {
            if(dollar_player_needs_to_give_get == 1)
            {
                Proposing_trade_list[dollar_player].cash_given += 1;
                list_point->cash_received += 1;
            } else
            {
                Proposing_trade_list[dollar_player].cash_received += 1;
                list_point->cash_given += 1;
            }
        }
	}

return;
}

/************************************************************************************/
/*					AI_Is_Monopoly_Trade            							    */
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*			AI_Trade_List_Record   *Proposing_trade_list   The trade list.   		*/
/*   Output: BOOL - TRUE if monopolies have been created, exchanged, or destroyed   */
/*                  by the trade. FALSE otherwise.                                  */
/************************************************************************************/

BOOL AI_Is_Monopoly_Trade(RULE_GameStatePointer game_state, AI_Trade_List_Record *trade_list)
{
	RULE_GameStateRecord	new_state;
    RULE_SquareGroups       cur_group;
    RULE_PlayerNumber       new_owner, old_owner;

    /* First, get the state of the game after the trade. */
    memcpy(&new_state, game_state, sizeof( RULE_GameStateRecord));
	AI_Update_State(&new_state, trade_list);
    
    /* Now go through monopolies and see if their owner changes. */
    for(cur_group = 0; cur_group <= SG_PARK_PLACE; ++cur_group)
    {
        if(AI_Is_Monopoly(game_state, AI_Expensive_Monopoly_Square[cur_group]))
            old_owner = AI_Who_Owns_Monopoly(game_state, AI_Expensive_Monopoly_Square[cur_group]);
        else
            old_owner = RULE_NOBODY_PLAYER;

        if(AI_Is_Monopoly(&new_state, AI_Expensive_Monopoly_Square[cur_group]))
            new_owner = AI_Who_Owns_Monopoly(&new_state, AI_Expensive_Monopoly_Square[cur_group]);
        else
            new_owner = RULE_NOBODY_PLAYER;

        if(new_owner != old_owner)
            return TRUE;
    }

return FALSE;
}

/************************************************************************************/
/*					AI_Counter_Propose_Trade      									*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*			RULE_PlayerNumber	   player	    Who is counter-proposing?			*/
/*			BOOL				 trade_accept	Is this request sent during a trade */
/*												accept decision?					*/
/*	 Output:  BOOL -  TRUE if a counter-proposal has been proposed.  FALSE otherwise*/
/*																				    */
/*		This function tries to counter propose the current trade.  Will not try     */
/*  to counter propose proposals whose AI_Calc_Trade_Property_Importance is too     */
/*  low.  Will increase the amount of cash or unimportant properties received until */
/*  the trade is satisfactory.														*/
/************************************************************************************/

BOOL AI_Counter_Propose_Trade(RULE_GameStatePointer game_state, RULE_PlayerNumber player, BOOL trade_accept)
{
	int					player_index[RULE_MAX_PLAYERS];
	RULE_PropertySet	player_properties[RULE_MAX_PLAYERS], combined_properties = 0,
                        combined_traded_props = 0;
	RULE_GameStateRecord	new_state, old_state;
	RULE_SquareType	bought_property;
	RULE_PlayerNumber	purchase_player, target_player;
	RULE_PlayerNumber	trade_player_list[RULE_MAX_PLAYERS];
	int					num_players_in_list = 0, counter;
	float               trade_property_importance;
	float	            evaluation;
    BOOL                monopoly_trade = FALSE;
    float               total_attitude = (float) 0.0, gave_monopoly_value = (float) 0.0;
    AI_Imp_List_Record    imp_record[AI_IMP_MAX];
    DWORD               mike_kludge_counter_to_avoid_lock = 0;
#define MAX_MIKE_MKLUDGE_COUNTER_VALUE_THEN_EXIT            10000

#ifdef AI_NO_TRADES
	return FALSE;	/* No trades yet for monopoly game. */
#endif 

	/* First, make sure this is a valid time for countering a trade. */
	if(AI_State[player].num_actions || !game_state->TradeInProgress ||
		AI_PLAYER_SENDING_TRADE || AI_Glob.AUCTION_IS_ON)	
		return FALSE;	/* Player in the middle of doing something or trade not going on. */

	if(!AI_Player_Involved_In_Trade(Current_Trade_List, player))
    {
		AI_Send_Message(AI_MESS_Not_Involved_In_Trade, player, AI_Glob.Player_Proposed_Trade);
	    AI_SendAction(ACTION_TRADE_EDITING_DONE, player, RULE_BANK_PLAYER,
					    1, AI_Glob.Player_Proposed_Trade, 0, 0, NULL, 0, NULL);
		return TRUE;  /* Being offered a trade we are not involved in - gave it back to player. */
    }

	/* Is the AI giving or getting immunities/futures? If yes, reject.  Don't do
	   futures or immunities yet. */
	if(AI_Player_Give_Immunities_Futures(player))
	{
		AI_Send_Message(AI_MESS_Reject, player, AI_Glob.Player_Proposed_Trade);
		return FALSE;
	}

	if(AI_State[player].times_countered_trade >= AI_Data[player].trade_counter_limit)
	{
		AI_Send_Message(AI_MESS_Too_Many_Counters, player, AI_Glob.Player_Proposed_Trade);
		return FALSE;	/* Offer been countered too many times. */
	}

    /*  Are we pissed off at the player for proposing these properties? If yes, reject the trade. */
    if(AI_Properties_Already_Traded(player, AI_Glob.Player_Proposed_Trade, Current_Trade_List, 
                                    AI_Data[player].number_times_allow_property_trade) &&
		!AI_State[player].times_countered_trade)
    {
        AI_Send_Message(AI_MESS_Ask_Me_Later, player, AI_Glob.Player_Proposed_Trade);
        return FALSE;
    }

    /*  Remember the trade if this is the first time we got it. */
    if(!AI_State[player].times_countered_trade)
        AI_Set_Properties_Traded(player, AI_Glob.Player_Proposed_Trade, Current_Trade_List);


	/*  Randomize whether we will counter or not.  If asked to edit, edit it. */
	if(( rand() / ((float) RAND_MAX) > AI_Data[player].trade_counter_prob) && trade_accept
		&& !AI_State[player].times_countered_trade)
		return FALSE;

	/*  Create a new game state after the trade. */
	memcpy(&new_state, game_state, sizeof( RULE_GameStateRecord));
	memcpy(&old_state, game_state, sizeof( RULE_GameStateRecord));
	memcpy(Proposing_trade_list, Current_Trade_List, sizeof(Proposing_trade_list));
	AI_Update_State(&new_state, Proposing_trade_list);

	/* If a player is currently buying a property, act as if that player
	   has already bought it. */
	bought_property	= AI_Property_Being_Bought(game_state, &purchase_player);
	if(bought_property != SQ_MAX_SQUARE_TYPES)
	{
		new_state.Squares[bought_property].owner = purchase_player;
		old_state.Squares[bought_property].owner = purchase_player;
	}

    /* Are the offers getting better? */
	evaluation = AI_Evaluate_Trade(&old_state, player, player, Proposing_trade_list);
	if(AI_State[player].times_countered_trade)
	{	
		if(evaluation <= AI_State[player].last_trade_evaluation)
		{	/* No change in the offer, or a worse offer.  Not a serious trader. */
			AI_Send_Message(AI_MESS_Not_Serious_Trader, player, AI_Glob.Player_Proposed_Trade);
			return FALSE;	
		}
	}

    /* Save the evaluation. */
	AI_State[player].last_trade_evaluation = evaluation;

    /* need to create an index to the monopoly importance chart, a property list and
       a player list. */
    num_players_in_list = AI_Create_Player_Property_Attitude_List(&old_state, player, Proposing_trade_list,
                                                       player_index, player_properties, trade_player_list,
                                                       &total_attitude);
	if(num_players_in_list < 1)
		return FALSE;  /* Something wrong with the list - shouldn't happen. */

    if(num_players_in_list == 1)
    {
        /*  If this is a 2 player trade, make sure we are not annoyed at the other trader. */
        if(AI_Data[player].player_attitude[trade_player_list[0]] <= -1.0)
        {   /* We are too annoyed at this player to counter. */
			AI_Send_Message(AI_MESS_Annoyed_With_Trader, player, AI_Glob.Player_Proposed_Trade);
			return FALSE;	
        }
    }

    /* We need to figure out the average attitude of players we are
       trading with, so we can find out how good a deal we want to offer 
       based on our average attitude towards all players in the trade list.*/
    total_attitude /= num_players_in_list;
    if(total_attitude < -1.0)
        total_attitude = (float) -1.0;

    /*  Find out the combined properties of players in the trade, minus those
        already traded. */
    for(counter = 0; counter < num_players_in_list; ++counter)
    {
        target_player = trade_player_list[counter];
        combined_properties |= player_properties[target_player];
        combined_traded_props |= Proposing_trade_list[target_player].properties_received;
        combined_traded_props |= Proposing_trade_list[target_player].properties_given;
    }

    combined_properties |= player_properties[player];

    /* Remove from combined_properties all properties in the trade list. */
    combined_properties &= -1 ^ combined_traded_props;

    /*  Until the property importance is well balanced, or we are happy with the trade,
        keep adding important properties. */
    do
    {
        /* Is this a good trade? If yes, stop adding properties. */
		evaluation = AI_Evaluate_Trade(&old_state, player, player, Proposing_trade_list);

		if(evaluation >= AI_Data[player].min_evaluation_threshold)
            break;  /* Good enough. */

        /* Find out the difference in property importance of properties received versus
           those given away. */
    	AI_Update_State(&new_state, Proposing_trade_list);
	    trade_property_importance = AI_Calc_Trade_Property_Importance(&old_state, &new_state,
										    player, player, Proposing_trade_list, FALSE);
        
        /* Depending on how far away we are, give us a monopoly, or something smaller. */
            /* First, figure out the order of items we should be giving. */
        AI_Create_Item_Importance_List(&old_state, player, player, imp_record);

        for(counter = 0; counter < AI_IMP_MAX; ++counter)
        {
            if(-trade_property_importance >= imp_record[counter].importance)
            {
                if(AI_Add_Type_Property(game_state, &new_state, player, player, &combined_properties, 
                                        imp_record[counter].item, Proposing_trade_list, 
                                        player_properties, FALSE))
                    break;
            }
        }

        mike_kludge_counter_to_avoid_lock ++;

        if(counter == AI_IMP_MAX)
            break;  /* Didn't manage to add an important property. */

        // If we get stuck in this do loop forever!
        if (mike_kludge_counter_to_avoid_lock >= MAX_MIKE_MKLUDGE_COUNTER_VALUE_THEN_EXIT)
        {
            break;
        }

    } while (trade_property_importance < 0);

    /* We have tried to balance the property importance.  Did our trade partner have
       enough? */
    if((trade_property_importance < AI_Data[player].lowest_prop_importance_for_counter) &&
        (evaluation < AI_Data[player].min_evaluation_threshold))
	{
		AI_Send_Message(AI_MESS_Reject, player, AI_Glob.Player_Proposed_Trade);
		return FALSE;	/* Deal is too poor to counter. */
	}

    /* We gave ourselves important properties.  However, if this is a 2 player trade,
       give back important properties. */
	if(num_players_in_list == 1)
    {   
        target_player = trade_player_list[0];

        /* Give properties in return if importance is too low. */

        /* Depending on how far away we are, and what our attitude is, give a monopoly, 
           or something smaller. */
            /* First, figure out the order of items we should be giving. */
        AI_Create_Item_Importance_List(&old_state, target_player, player, imp_record);

        for(counter = 0; counter < AI_IMP_MAX; ++counter)
        {
            /* What is the property importance difference for the player we are trading with? */
        	AI_Update_State(&new_state, Proposing_trade_list);
	        trade_property_importance = AI_Calc_Trade_Property_Importance(&old_state, &new_state,
										        target_player, player, Proposing_trade_list, FALSE);
        
            trade_property_importance += gave_monopoly_value;

            if(-trade_property_importance >= imp_record[counter].importance)
            {
                if(imp_record[counter].item == AI_IMP_MONOPOLY)
                {
                    /* There is a monopoly difference.  We must clear the properties
                       received by target_player so we do not end up giving a monopoly
                       twice, especially if our attitude towards the player is so low we do
                       not want to give a monopoly in return. */
                    Proposing_trade_list[player].properties_given = 0;
                    Proposing_trade_list[target_player].properties_received = 0;

                    memcpy(&new_state, game_state, sizeof(new_state) );
                	AI_Update_State(&new_state, Proposing_trade_list);

                    trade_property_importance = AI_Calc_Trade_Property_Importance(&old_state, &new_state,
										        target_player, player, Proposing_trade_list, FALSE);
                }

                if(AI_Add_Type_Property(game_state, &new_state, target_player, player, &combined_properties, 
                                        imp_record[counter].item, Proposing_trade_list, 
                                        player_properties, TRUE))
                {
                    if(imp_record[counter].item == AI_IMP_MONOPOLY)
                    {   /* We succesfully gave a monopoly.  We need to offset its value artificially
                           since if our attitude towards the player is low enough, we might not
                           have given a monopoly back! */
                    	AI_Update_State(&new_state, Proposing_trade_list);
                        gave_monopoly_value = AI_Calc_Trade_Property_Importance(&old_state, &new_state,
										        target_player, player, Proposing_trade_list, FALSE);
                        gave_monopoly_value -= trade_property_importance;
                        gave_monopoly_value = (float) imp_record[counter].importance - gave_monopoly_value;
                        if(gave_monopoly_value < 0)
                            gave_monopoly_value = (float) 0.0;
                    }
                }
                else if(imp_record[counter].item == AI_IMP_MONOPOLY)
                {
					if(trade_accept)
	                    AI_Send_Message(AI_MESS_Reject, player, AI_Glob.Player_Proposed_Trade);
					else
	                    AI_Send_Message(AI_MESS_Cant_Suggest_Trade, player, AI_Glob.Player_Proposed_Trade);
                    return FALSE;   /* Couldn't add a monopoly! Can't do this trade. */
                }
            }
        }
    }

    /* Make sure we can offer this trade directly. */
    AI_Make_Trade_Proper(game_state, Proposing_trade_list);

    /* Pad with cash so we are satisfied, and make sure everyone can pay this. */
    if(!AI_Make_Trade_Fair(&old_state, player, trade_player_list, num_players_in_list,
						   Proposing_trade_list, player_properties, AI_MAX_GIVE_IN_TRADE, FALSE) )
    {
		if(trade_accept)
	        AI_Send_Message(AI_MESS_Reject, player, AI_Glob.Player_Proposed_Trade);
		else
	        AI_Send_Message(AI_MESS_Cant_Suggest_Trade, player, AI_Glob.Player_Proposed_Trade);
		return FALSE;
    }

	/* Everyone can pay.  Then send it. */
    if(!trade_accept)
        AI_SEND_TRADE_OFFER_TRADE = TRUE;

	/* Make sure, after all this, that the trade satisfies all the rules. */
	if(!AI_Trade_Is_Proper(game_state, Proposing_trade_list))
		return FALSE;

	if(AI_Send_Proposed_Trade(game_state, player, trade_accept))
	{
		if(!AI_State[player].times_countered_trade)
			AI_Send_Message(AI_MESS_Counter, player, AI_Glob.Player_Proposed_Trade);
		AI_State[player].times_countered_trade++;
		return TRUE;
	}


	/* For some reason, can't send counter proposal. */
return FALSE;
}


/************************************************************************************/
/*					AI_Create_Player_Property_Attitude_List 						*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*			RULE_PlayerNumber	   player	    Who is creating the list?			*/
/*          AI_Trade_List_Record   *trade_list  Create a list of players involved   */
/*                                              in this trade.                      */
/*          int                    *player_index  Where to store indexes into       */
/*                                      what_to_trade entires.                      */
/*          RULE_PropertySet       *player_properties  Where to store properties    */
/*                                         owned of players in list.                */
/*          RULE_PlayerNumber      *trade_player_list  Where to store players in    */
/*                                              trade (exluding player )            */
/*          float                  *total_attitude  Where to store total attitude   */
/*                                      towards all players combined.               */
/*	 Output:  int - number of players in list.                                      */
/*																				    */
/*  This function creates 3 lists - a list of players in the trade, excluding       */
/*  player, a list of properties owned by each player in the list (including player)*/
/*  , and a list of indices into what_to_trade for each player in the trade.        */
/************************************************************************************/

int AI_Create_Player_Property_Attitude_List(RULE_GameStatePointer game_state, RULE_PlayerNumber player, 
                    AI_Trade_List_Record *trade_list, int *player_index, 
                    RULE_PropertySet *player_properties, RULE_PlayerNumber *trade_player_list,
                    float *total_attitude)
{
	RULE_PlayerNumber	target_player;
	float	*player_attitudes;
	int					num_players_in_list = 0;

    *total_attitude = (float) 0.0;

    player_attitudes = AI_Data[player].player_attitude;
	for(target_player = 0; target_player < game_state->NumberOfPlayers; ++target_player, ++player_attitudes)
	{
		if(*player_attitudes <= -1.0)
			player_index[target_player] = 0;
		else player_index[target_player] = (int) ((*player_attitudes + 1) * 10);

		if(player_index[target_player] >= AI_WHAT_TO_TRADE_ENTRIES)
			player_index[target_player] = AI_WHAT_TO_TRADE_ENTRIES - 1;

		player_properties[target_player] = RULE_PropertySetOwnedByPlayer(game_state, target_player);

		if(target_player == player)
			continue;

		if(AI_Player_Involved_In_Trade(trade_list, target_player))
        {
			trade_player_list[num_players_in_list++] = target_player;
            *total_attitude += *player_attitudes;
        }
	}

return num_players_in_list;
}


/************************************************************************************/
/*					AI_Create_Player_Property_Attitude_List 						*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state										*/
/*			RULE_PlayerNumber	   player	    Who is creating the list?			*/
/*			RULE_PlayerNumber	   ai_strat     Whose logic are we using?       	*/
/*          AI_Imp_List_Record     *imp_record  Where to  store the list            */
/*																				    */
/*  This function creates a list of types of properties that can be traded, sorted  */
/*  in order of importance as set by AI settings if ai_strat.                       */
/************************************************************************************/

void AI_Create_Item_Importance_List(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
                                    RULE_PlayerNumber ai_strat, AI_Imp_List_Record *imp_record)
{
    int head, tail;
    int num_rails;
    int num_utils;
    AI_Imp_List_Record  swap;
    double  factor;
    RULE_PropertySet    properties_owned = RULE_PropertySetOwnedByPlayer(game_state, player);

    /* If ai_strat is not the same as player, it means ai_strat is giving something
       to player.  Hence we should use the attitude of ai_strat towards player. */
    if(player != ai_strat)
		factor = AI_Get_Cash_Multiplier(ai_strat, player);
    else factor = 1.0;

    imp_record[0].item = AI_IMP_MONOPOLY;
    imp_record[0].importance = AI_Data[ai_strat].monopoly_received_importance * 0.75 * factor;

    imp_record[1].item = AI_IMP_TRADE;
    imp_record[1].importance = AI_Data[ai_strat].property_allow_trade_importance * factor;

    imp_record[2].item = AI_IMP_TWO_UNOWNED;
    imp_record[2].importance = AI_Data[ai_strat].property_two_unowned_importance * factor;

    imp_record[3].item = AI_IMP_ONE_UNOWNED;
    imp_record[3].importance = AI_Data[ai_strat].property_one_unowned_importance * factor;

    /* We want to see how important is the NEXT railroad or utility we would get. */
    num_rails = AI_Number_Railroads_Utils_Owned(game_state, player, SG_RAILROAD, FALSE, properties_owned);
    num_utils = AI_Number_Railroads_Utils_Owned(game_state, player, SG_UTILITY, FALSE, properties_owned);

    if(num_rails == AI_MAX_RAILROADS)
        num_rails--;
    if(num_utils == AI_MAX_UTILITIES)
        num_utils--;

    imp_record[4].item = AI_IMP_RAILROAD;
    imp_record[4].importance = (AI_Data[ai_strat].railroad_importance[num_rails] - 
                               (num_rails ? AI_Data[ai_strat].railroad_importance[num_rails - 1]:0)) * factor;
 
    imp_record[5].item = AI_IMP_UTILITY;
    imp_record[5].importance = (AI_Data[ai_strat].utility_importance[num_utils] - 
                                (num_utils ? AI_Data[ai_strat].railroad_importance[num_utils - 1]:0)) * factor;

    /* If ai_strat is giving something to player, then we need to also
       take into account the loss of a railroad or utility for ai_strat. */
    if(ai_strat != player)
    {
        properties_owned = RULE_PropertySetOwnedByPlayer(game_state, ai_strat);
        num_rails = AI_Number_Railroads_Utils_Owned(game_state, ai_strat, SG_RAILROAD, FALSE, properties_owned);
        num_utils = AI_Number_Railroads_Utils_Owned(game_state, ai_strat, SG_UTILITY, FALSE, properties_owned);

        if(num_rails)
        {
            num_rails--;
            imp_record[4].importance += (AI_Data[ai_strat].railroad_importance[num_rails] - 
                                       (num_rails ? AI_Data[ai_strat].railroad_importance[num_rails - 1]:0)) * factor;
        }

        if(num_utils)
        {
            num_utils--;
            imp_record[5].importance += (AI_Data[ai_strat].utility_importance[num_utils] - 
                                        (num_utils ? AI_Data[ai_strat].railroad_importance[num_utils - 1]:0)) * factor;
        }
    }

    /* Now linear sort the list. */
    for(head = 0; head < AI_IMP_MAX; ++head)
    {
        for(tail = head + 1; tail < AI_IMP_MAX; ++tail)
        {
            if(imp_record[head].importance < imp_record[tail].importance)
            {
                swap = imp_record[head];
                imp_record[head] = imp_record[tail];
                imp_record[tail] = swap;
            }
        }
    }

return;
}


/************************************************************************************/
/*					AI_Add_Type_Property                    						*/
/*																					*/
/*   Input: RULE_GameStatePointer  game_state	State before the trade				*/
/*          RULE_GameStatePointer  new_state	State after the trade				*/
/*			RULE_PlayerNumber	   player	    Who is getting something?   		*/
/*			RULE_PlayerNumber	   ai_strat	    Whose strategy to use?         		*/
/*          RULE_PropertySet       *combined_properties  List of properties owned   */
/*                                      by players we are trading with.             */
/*          AI_Importance_Record   item         What to give player.                */
/*          AI_Trade_List_Record   *trade_list  The trade so far.                   */
/*          RULE_PropertySet       *properties   Properties owned by each player.   */
/*          BOOL                   give_descending  Give properties from best to    */
/*                                      worst (TRUE), or vice versa?                */
/*	 Output:  BOOL - TRUE if we managed to give 'item' property to player from the  */
/*                   combined properties.                                           */
/*																				    */
/*      This function tries to give player a property/ies of type 'item'.  The      */
/*  properties also depend on whether ai_strat is the same as player.  If           */
/*  they are, tries to give only the best.  If not, uses the attitude of ai_strat   */
/*  towards player to consider what should be given, in the case of a monopoly.     */
/************************************************************************************/

BOOL AI_Add_Type_Property(RULE_GameStatePointer game_state, RULE_GameStatePointer new_state,
                          RULE_PlayerNumber player, RULE_PlayerNumber ai_strat,
                          RULE_PropertySet *combined_properties, 
                          AI_Importance_Record item, AI_Trade_List_Record *trade_list, 
                          RULE_PropertySet *properties, BOOL give_descending)
{
    RULE_PropertySet  bit, monopolies_traded = 0, combined_traded = 0, save_properties[RULE_MAX_PLAYERS];
    RULE_PropertySet  group_bit, mask_bit;
    RULE_SquareGroups group, monopoly_group[SG_PARK_PLACE + 1], index;
    RULE_PlayerNumber   player_list[RULE_MAX_PLAYERS], cur_player;
    int               num_players_in_list = 0, counter, smallest_trade;
    float             attitude;


    /* Find out the combined items traded/given away. */
    for(cur_player = 0; cur_player < game_state->NumberOfPlayers; ++cur_player)
    {   
        combined_traded |= Proposing_trade_list[cur_player].properties_given;
        combined_traded |= Proposing_trade_list[cur_player].properties_received;
    }

    /* Find out which monopolies have properties traded in. */
    for(group = 0; group <= SG_PARK_PLACE; ++group)
    {
        bit = AI_Get_Monopoly_Set(AI_Expensive_Monopoly_Square[group]);
        if(combined_traded & bit)
            monopolies_traded |= 1 << group;
    }

    /* We need to create a player list after the trade who we can trade with
       for monopolies. */
    for(cur_player = 0; cur_player < new_state->NumberOfPlayers; ++cur_player)
    {
 		if(new_state->Players[cur_player].currentSquare == SQ_OFF_BOARD)
			continue; /* gone bankrupt. */

		if(cur_player == player)
			continue;

        if(!AI_Player_Own_Monopoly(new_state, cur_player, FALSE))
        {
		    /* Add this player to a list. */
		    player_list[num_players_in_list++] = cur_player;
        }
    }

    /* Order the monopolies in order of liquid assets at the end
       of the trade. */
    AI_Order_Monopoly_Importance(AI_Get_Liquid_Assets(new_state, player, FALSE, FALSE),
                                 monopoly_group, give_descending ? AI_Sort_Descending:AI_Sort_Ascending);

    /* What kind of item are we adding for player? */
    switch(item)
    {
        case AI_IMP_MONOPOLY:  
            /* Here, if the ai_strat is not the same as player, we need to take into
               account ai_strat attitude towards player. */
            if(player != ai_strat)
            {
                /* First, find an index into what_to_trade to figure out what to
                   give for the monopoly offered. */
                attitude = AI_Data[ai_strat].player_attitude[player];
                if(attitude < -1.0)
                    index = 0;
                else index = (int) ((attitude + 1) * 10);

		        if(index >= AI_WHAT_TO_TRADE_ENTRIES)
			        index = AI_WHAT_TO_TRADE_ENTRIES - 1;
		        if(index < 0)
			        index = 0;

		        /* Give the monopoly the player deserves, if any. */
		        switch(AI_Data[ai_strat].what_to_trade[index].give_monopoly)
		        {
			        case AI_Property_Best:
				        /* Go through all the properties that will give player
				           a direct monopoly and give the best, according to his/her
				           liquid assets. */
				        if(!AI_Add_Trade_Monopoly(game_state, player, combined_properties, AI_Property_Best, trade_list))
                            return FALSE;
				        break;

			        case AI_Property_Worst:
				        if(!AI_Add_Trade_Monopoly(game_state, player, combined_properties, AI_Property_Worst, trade_list))
                            return FALSE;
				        break;
		        }

		        /* Add other properties, to sweaten the deal or instead of a monopoly. */
		        for(counter = AI_Data[ai_strat].what_to_trade[index].give_group_trades; counter > 0; --counter)
                {
                    if(!AI_Add_Type_Property(game_state, new_state, player, ai_strat, combined_properties, AI_IMP_TRADE,
                                         trade_list, properties, give_descending))
                    {
                        if(!AI_Add_Type_Property(game_state, new_state, player, ai_strat, combined_properties, AI_IMP_ONE_UNOWNED,
                                             trade_list, properties, give_descending))
                        {
                            if(!AI_Add_Type_Property(game_state, new_state, player, ai_strat, combined_properties, AI_IMP_TWO_UNOWNED,
                                                 trade_list, properties, give_descending))
                                break;
                        }
                    }
                }

		        for(counter += AI_Data[ai_strat].what_to_trade[index].give_cash_cows; counter > 0; --counter)
		        {
                    if(!AI_Add_Type_Property(game_state, new_state, player, ai_strat, combined_properties, AI_IMP_RAILROAD,
                                         trade_list, properties, give_descending))
                    {
                        if(!AI_Add_Type_Property(game_state, new_state, player, ai_strat, combined_properties, AI_IMP_UTILITY,
                                             trade_list, properties, give_descending))
                            break;
                    }
		        }

		        for(counter += AI_Data[ai_strat].what_to_trade[index].give_junk; counter > 0; --counter)
		        {
                    if(!AI_Add_Type_Property(game_state, new_state, player, ai_strat, combined_properties, AI_IMP_JUNK,
                                         trade_list, properties, give_descending))
                        break;
		        }

                return TRUE;
            }
            else    
                return(AI_Add_Trade_Monopoly(game_state, player, combined_properties, AI_Property_Best, 
                                             trade_list));
        case AI_IMP_TRADE:
            for(index = 0; index < SG_PARK_PLACE + 1; ++index)
            {
                group = monopoly_group[index];

                /* Skip over baltic if we're giving us something. */
                if((ai_strat == player) && (group == SG_MEDITERRANEAN_AVENUE))
                    continue;

                if(!(monopolies_traded & (1 << group)) )
                {   
                    /* Not already traded. */
                    group_bit = AI_Get_Monopoly_Set(AI_Expensive_Monopoly_Square[group]);
                    if((*combined_properties & group_bit) != group_bit &&
                        (*combined_properties & group_bit) != 0)
                    {   
                        /* This is not a monopoly owned between us. Is it ALL owned by us? */
                        if( (*combined_properties & group_bit) == (properties[player] & group_bit))
                            continue;  /* Yes - nothing to get here. */

                        /* Get the property then, and see if we can trade for a monopoly with it. */
                        bit = *combined_properties & group_bit;
                        bit &= -1 ^ (properties[player] & group_bit);
                        memcpy(save_properties, properties, sizeof(save_properties));

                        for(cur_player = 0; cur_player < game_state->NumberOfPlayers; ++cur_player)
                            save_properties[cur_player] &= -1 ^ bit;

                        save_properties[player] |= bit;
                        
             		    smallest_trade = AI_Find_Smallest_Monopoly_Trade(player, group_bit,
                                        player_list, num_players_in_list, save_properties);

                        if(smallest_trade != 0)
                            break;
                    }
                }
            }

            if(index == SG_PARK_PLACE + 1)
                return FALSE;

            break;

        case AI_IMP_TWO_UNOWNED:
        case AI_IMP_ONE_UNOWNED:

            item -= AI_IMP_ONE_UNOWNED + 1;
            for(index = 0; index < SG_PARK_PLACE + 1; ++index)
            {
                group = monopoly_group[index];

                /* Skip over baltic if we're giving us something. */
                if((ai_strat == player) && (group == SG_MEDITERRANEAN_AVENUE))
                    continue;

                if(!(monopolies_traded & (1 << group)) )
                {   
                    /* Not already traded. */
                    group_bit = AI_Get_Monopoly_Set(AI_Expensive_Monopoly_Square[group]);
                    if((*combined_properties & group_bit) != group_bit &&
                        (*combined_properties & group_bit) != 0)
                    {   
                        /* This is not a monopoly owned between us. Is it ALL owned by us? */
                        if( (*combined_properties & group_bit) == (properties[player] & group_bit))
                            continue;  /* Yes - nothing to get here. */

                        /* Are there 2 or 1 unowned properties? */
                        bit = *combined_properties & group_bit;
                        bit &= -1 ^ (properties[player] & group_bit);
                        if(AI_Is_Possible_Monopoly(new_state, player, AI_Expensive_Monopoly_Square[group]) == item)
                            break;
                    }
                }
            }

            if(index == SG_PARK_PLACE + 1)
                return FALSE;

            break;

        case AI_IMP_RAILROAD:
            /* First, make sure we are not giving a railroad in the trade. */
            group_bit = AI_Get_Monopoly_Set(SQ_READING_RR);

            if(group_bit & trade_list[player].properties_given)
                return FALSE;   /* Yup - don't want to give and get railroads. */

            if(*combined_properties & group_bit)
            {   
				/* Try to get a railroad. */
                bit = *combined_properties & group_bit;
                bit &= -1 ^ (properties[player] & group_bit);

				/* Get one? */
				if(bit == 0)
					return FALSE;

				/* Isolate one property */
				mask_bit = 1;
				do
				{
					if(bit & mask_bit)
					{
						bit = mask_bit;
						break;
					}

					mask_bit <<= 1;
				} while(TRUE);

                break;
            }

            return FALSE;
        case AI_IMP_UTILITY:
            /* First, make sure we are not giving a utility in the trade. */
            group_bit = AI_Get_Monopoly_Set(SQ_ELECTRIC_COMPANY);

            if(group_bit & trade_list[player].properties_given)
                return FALSE;   /* Yup - don't want to give and get railroads. */

            if(*combined_properties & group_bit)
            {   
                /* Try to get some utilities. */
                bit = *combined_properties & group_bit;
                bit &= -1 ^ (properties[player] & group_bit);

				/* Get one? */
				if(bit == 0)
					return FALSE;

				/* Isolate one property */
				mask_bit = 1;
				do
				{
					if(bit & mask_bit)
					{
						bit = mask_bit;
						break;
					}

					mask_bit <<= 1;
				} while(TRUE);

                break;
            }

            return FALSE;

        case AI_IMP_JUNK:
            for(index = 0; index < SG_PARK_PLACE + 1; ++index)
            {
                group = monopoly_group[index];
                if(!(monopolies_traded & (1 << group)) )
                {   
                    /* Not already traded. */
                    group_bit = AI_Get_Monopoly_Set(AI_Expensive_Monopoly_Square[group]);
                    if((*combined_properties & group_bit) != group_bit &&
                        (*combined_properties & group_bit) != 0)
                    {   
                        /* This is not a monopoly owned between us. Is it ALL owned by us? */
                        if( (*combined_properties & bit) == (properties[player] & group_bit))
                            continue;  /* Yes - nothing to get here. */

                        /* Are there no unowned properties, and won't give a monopoly? */
                        bit = *combined_properties & group_bit;
                        bit &= -1 ^ (properties[player] & group_bit);
                        if(AI_Is_Possible_Monopoly(new_state, player, AI_Expensive_Monopoly_Square[group]) == -1)
                            break;
                    }
                }
            }
            return FALSE;

        default:
            return FALSE;
    }

    /* Found a property we can trade for! Give it then. */
    trade_list[player].properties_received |= bit;
    
    /* Make sure each player who owns a property in bit gives it. */
    for(cur_player = 0; cur_player < game_state->NumberOfPlayers; ++cur_player)
    {
        if(bit & properties[cur_player])
        {
            trade_list[cur_player].properties_given |= bit & properties[cur_player];
            *combined_properties &= -1 ^ bit;
        }
    }

return TRUE;
}

/************************************************************************************/
/*					AI_Set_Properties_Traded                  						*/
/*																					*/
/*   Input: RULE_PlayerNumber	   player	    For whom are we setting properties? */
/*          RULE_PlayerNumber proposed_player   Who proposed the trade?             */
/*          AI_Trade_List_Record   *trade_list  The trade so far.                   */
/*																				    */
/*    This function tries to remember the properties traded by player in a recent   */
/*  traded.  This allows the AI to reject consecutive offers for a property, and    */
/*  force players to make real offers.  Has a counter for each property which goes  */
/*  up to 3.  Every time this function is called, increases the counter by 1 if not */
/*  3 already.                                                                      */
/************************************************************************************/

void AI_Set_Properties_Traded(RULE_PlayerNumber player, RULE_PlayerNumber proposed_player,
                              AI_Trade_List_Record *trade_list)
{
    RULE_PropertySet  properties_traded, mask, mask2;
    RULE_PropertySet  *bit1, *bit2;

    bit1 = &AI_State[player].properties_traded_bit_1[proposed_player];
    bit2 = &AI_State[player].properties_traded_bit_2[proposed_player];

    /* First, combine a list of properties traded. */
    properties_traded = trade_list[player].properties_received | trade_list[player].properties_given;

    /* Now set the bits - 1st + 2nd bit be a binary number, from 0 - 3. Hence
       if the first bit of a property traded is not set, set it.  If it is set,
       set the 2nd bit and clear the first if the 2nd bit is not set. */
    mask = -1 ^ *bit1;
    mask2 = *bit1 & properties_traded & (-1 ^ *bit2);
    *bit1 =  *bit1 | (mask & properties_traded);
    *bit2 |= mask2;
    *bit1 &= -1 ^ mask2;

return;
}


/************************************************************************************/
/*					AI_Properties_Already_Traded              						*/
/*																					*/
/*   Input: RULE_PlayerNumber	   player	    For whom are we checking?           */
/*          RULE_PlayerNumber proposed_player   Who proposed the trade?             */
/*          AI_Trade_List_Record   *trade_list  The trade so far.                   */
/*          int                   danger_level  Number between 0 - 3. 0 meaning     */
/*                                              never remembers.                    */
/*   Output: RULE_PropertySet - Properties traded by player in trade_list at least  */
/*           danger_level or more times in the near past to proposed_player. NULL   */
/*           if none.                                                               */
/*																				    */
/*  This function tests to see whether the properties given or received by player   */
/*  in the current trade 'trade_list' have been traded at least danger_level times  */
/*  before.  If yes, returns these properties.  If no, or if danger_level is set to */
/*  0, returns NULL.                                                                */
/************************************************************************************/

BOOL AI_Properties_Already_Traded(RULE_PlayerNumber player, RULE_PlayerNumber proposed_player,
                                  AI_Trade_List_Record *trade_list, int danger_level)
{
    RULE_PropertySet combined_properties;
    RULE_PropertySet  *bit1, *bit2;
    RULE_PropertySet properties_bit1_set, properties_bit2_set;

    /* Figure out what properties were traded by player. */
    bit1 = &AI_State[player].properties_traded_bit_1[proposed_player];
    bit2 = &AI_State[player].properties_traded_bit_2[proposed_player];

    combined_properties = trade_list[player].properties_received | trade_list[player].properties_given;

    /* Now see whether there are any properties at danger_level or higher. */
    properties_bit1_set = combined_properties & *bit1;
    properties_bit2_set = combined_properties & *bit2;

    combined_properties = 0;
    if(danger_level == 1)
        combined_properties = properties_bit1_set | properties_bit2_set;

    if(danger_level == 2)
        combined_properties = properties_bit2_set;

    if(danger_level == 3)
        combined_properties = properties_bit1_set & properties_bit2_set;

return combined_properties;
}


/************************************************************************************/
/*					AI_Properties_Forget_Traded             						*/
/*																					*/
/*   Input: RULE_PlayerNumber	   player	    For whom are we forgetting?         */
/*																				    */
/*   This function makes player forget s/he has traded each property to each        */
/*  player by 1 count.  See AI_Set_Properties_Traded and                            */
/*  AI_Properties_Already_Traded.                                                   */
/************************************************************************************/

void AI_Properties_Forget_Traded(RULE_PlayerNumber player)
{
    RULE_PropertySet  mask, mask2;
    RULE_PropertySet  *bit1, *bit2;
    RULE_PlayerNumber cur_player;

    if(player >= RULE_MAX_PLAYERS)
    {
#if _DEBUG
		LE_ERROR_ErrorMsg ("AI_Properties_Forget_Traded called will illegal player number.");
#endif
        return; /* should not happen! */
    }

    for(cur_player = 0; cur_player < RULE_MAX_PLAYERS; ++cur_player)
    {
        bit1 = &AI_State[player].properties_traded_bit_1[cur_player];
        bit2 = &AI_State[player].properties_traded_bit_2[cur_player];

        /* Lower the bits by 1.  Hence all 1st bits that are 1 go to 0.  All
           second bits whose first bits are 0 go to 0, and first bits go to 1. */
        mask = -1 ^ *bit1;
        mask2 = *bit2 & mask;
        *bit2 ^= mask2;
        *bit1 =  mask2;
    }
    
return;
}


/************************************************************************************/
/*					AI_Should_Give_Away_Monopoly                                    */
/*																					*/
/*   Input: RULE_GameStatePointer   game_state                                      */
/*          RULE_PlayerNumber       player      Who wants to give one away?         */
/*   Output: BOOL - TRUE if 'player' should try to trade a monopoly to a weak       */
/*                  opponent for cash.  FALSE otherwise.                            */
/*																				    */
/*      This functions tests whether 'player' has a monopoly, but not at least      */
/*  3 houses per monopoly.                                                          */
/************************************************************************************/

BOOL AI_Should_Give_Away_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player)
{
    long    num_monopolies, counter, num_lots, num_houses;
    RULE_SquareType  monopoly_squares[SG_PARK_PLACE + 1];
	RULE_GameStateRecord	new_state;
    RULE_SquareType  monopoly_lots[AI_MAX_MONOPOLY_LOTS + 1];

    /* Do we have a monopoly? If not, nothing to consider. */
    if(!AI_Player_Own_Monopoly(game_state, player, FALSE))
        return FALSE;  /* Don't own a monopoly. Nothing to consider. */

	if(AI_Housing_Shortage(game_state, AI_CRITICAL_HOUSING_LEVEL))
        return FALSE;  /* Not enough houses available to make this worth it. */

    /* Is there a player we can give this monopoly away to? */
    if(AI_Find_Nonmonopoly_Player(game_state, player) == RULE_NOBODY_PLAYER)
        return FALSE;

	/* Does anyone else own a monopoly? If not, don't bother. */
	if(AI_Only_Player_Have_Monopoly(game_state, player))
		return FALSE;

    /* Create a copy of the game state. */
	memcpy(&new_state, game_state, sizeof(RULE_GameStateRecord));

	/* First, need to unmortgage all the properties that give monopolies and
	   build them up.  */
    while(AI_Unmortgage_Property(&new_state, player, TRUE, TRUE));

    /* Now buy houses. */
    while(AI_Hypothetical_Buy_Houses(&new_state, player));

    /* Do we have at least 3 houses on each monopoly? */
    num_monopolies = AI_Get_Monopolies(&new_state, player, monopoly_squares, FALSE);

    for(counter = 0; counter < num_monopolies; ++counter)
    {
        num_houses = AI_Num_Houses_On_Monopoly(&new_state, monopoly_squares[counter]);
        num_lots = AI_Get_Monopoly_Lots(&new_state, monopoly_lots, monopoly_squares[counter]);

        if(num_houses <= num_lots * AI_Data[player].max_houses_per_square_for_give_away)
            return TRUE;
    }

return FALSE;
}

/************************************************************************************/
/*					AI_Get_Cash_Multiplier											*/
/*																					*/
/*   Input: RULE_PlayerNumber		ai_strat    Whose AI settings are we using?		*/
/*          RULE_PlayerNumber       player      For who do we want a multiplier?    */
/*   Output: double - the factor we should multiply the money wanted in a trade		*/
/*                  where ai_strat wants money from player.							*/
/*																				    */
/*		Uses ai_strat's AI settings to linear extrapolate the cash factors which    */
/*  tell us by how much should we multiply the lowest amount of cash we would       */
/*  accept in a trade.																*/
/************************************************************************************/

double  AI_Get_Cash_Multiplier(RULE_PlayerNumber ai_strat, RULE_PlayerNumber player)
{
	float	evaluation;
	long	attitude_index;
	double  cash_multiplier;

	// Get the evaluation of ai_strat of player's ability. 
    evaluation = AI_Data[ai_strat].player_attitude[player];

	// Get an index into the factor table.
	if(evaluation <= -1.0f)
		evaluation = -0.999f;

	if(evaluation >= 1.0f)
		evaluation = 0.999f;

	attitude_index = (long) ((evaluation + 1) * 10);

    if(attitude_index >= AI_WHAT_TO_TRADE_ENTRIES)
		attitude_index = AI_WHAT_TO_TRADE_ENTRIES - 1;

	evaluation = (float) (evaluation - floor(evaluation));

	// Linear extrapolate the factor.
	if(attitude_index == 0)
		cash_multiplier = AI_Data[ai_strat].what_to_trade[attitude_index].cash_multiplier;
	else
		//Linear extrapolation of the cash multipliers. 
		cash_multiplier = AI_Data[ai_strat].what_to_trade[attitude_index].cash_multiplier * evaluation
						  + AI_Data[ai_strat].what_to_trade[attitude_index - 1].cash_multiplier * (1.0 - evaluation);

return cash_multiplier;
}


/************************************************************************************/
/*					AI_Player_Give_Get_Immunities									*/
/*																					*/
/*   Input: RULE_PlayerNumber		player   Test who is giving futures/immunities? */
/*   Output: BOOL - TRUE if player is giving immunities or futures					*/
/*					in the current trade. FALSE otherwise.							*/
/************************************************************************************/

BOOL AI_Player_Give_Immunities_Futures(RULE_PlayerNumber player)
{
	int count;
  AI_Immunity_Record  *immunity_point;

	/* Go through all the immunities/futures and see if player is involved in them. */
  immunity_point = AI_Immunities_Futures_Traded;
  for(count = 0; count < RULE_MAX_COUNT_HIT_SETS; ++count, ++immunity_point)
	{
    if(immunity_point->count == 0)
			continue;

		if(immunity_point->fromPlayer == player)
		   return TRUE;	/* Player giving immunities/futures. */

		if(immunity_point->toPlayer == player)
		   return TRUE;	/* Player getting immunities/futures. */
	}

return FALSE;
}


/************************************************************************************/
/*					AI_Use_The_Force												*/
/*																					*/
/*	Input:  RULE_GameStatePointer	game_state   Current state of the game - send   */
/*												 &UICurrentGameState.				*/
/*			RULE_PlayerNumber		player		 For whom to evaluate the current   */
/*												 trade?								*/
/*	Output:  BOOL - TRUE if the current trade is good for player, FALSE if bad.		*/
/************************************************************************************/

BOOL AI_Use_The_Force(RULE_GameStatePointer game_state, RULE_PlayerNumber player)
{
	AI_Data_Record  temp_hold;
	float fEvaluation;

	// First, load up an expert AI into AI_Data[0].
	memcpy(&temp_hold, &AI_Data[0], sizeof(temp_hold));	// save the old AI_Data[0].

	// Now put in the expert settings.
	memcpy(&AI_Data[0], &AI_Expert_AI_Settings, sizeof(AI_Expert_AI_Settings) );

	// Evaluate the current trade using expert settings. 
	fEvaluation = AI_Evaluate_Trade(game_state, player, 0 /* AI player strategy # */, Current_Trade_List);

	// Now return AI_Data[0] back to normal. 
	memcpy(&AI_Data[0], &temp_hold, sizeof(temp_hold));

	// Was it a good or bad trade? 
	if(fEvaluation < 0.0f)
		return FALSE;

return TRUE;
}
