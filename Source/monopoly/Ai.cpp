/*************************************************************
*
*   FILE:             AI.C
*
*   (C) Copyright 97  Artech Digital Entertainments.
*                     All rights reserved.
*************************************************************/

// TODO:  Put futures and immunities into AI for trade consideration

#include "gameinc.h"

#include <math.h>
#include <time.h>


void AI_Test_Me(RULE_ActionArgumentsPointer NewMessage);

/*  Making RULE_PropertyToBitSet a macro to speed up certain operations for
    superfast mode. */
#define RULE_PropertyToBitSet(Square)   (RULE_PropertyToBitsetArray[Square])

/************************************************************/
/* GLOBALS                                                  */
/************************************************************/

    /* The following array holds information about what messages
       each AI is currently sending, what they can send, and
       other relate stuff. */
AI_Message_State_Record AI_State[RULE_MAX_PLAYERS];

    /* Order in which to mortgage properties if not monopolies
       and if no minor-monopoly in Railroads or utilities.  */
const RULE_SquareType AI_Mortgage_Order[SQ_MAX_SQUARE_TYPES] =
{
  1,        // SQ_MEDITERRANEAN_AVENUE
  6,        // SQ_ORIENTAL_AVENUE
  8,        // SQ_VERMONT_AVENUE
  3,        // SQ_BALTIC_AVENUE
  9,        // SQ_CONNECTICUT_AVENUE
  11,       // SQ_ST_CHARLES_PLACE
  13,       // SQ_STATES_AVENUE
  14,       // SQ_VIRGINIA_AVENUE
  16,       // SQ_ST_JAMES_PLACE
  18,       // SQ_TENNESSEE_AVENUE
  19,       // SQ_NEW_YORK_AVENUE
  21,       // SQ_KENTUCKY_AVENUE
  23,       // SQ_INDIANA_AVENUE
  24,       // SQ_ILLINOIS_AVENUE
  26,       // SQ_ATLANTIC_AVENUE
  27,       // SQ_VENTNOR_AVENUE
  29,       // SQ_MARVIN_GARDENS
  31,       // SQ_PACIFIC_AVENUE
  32,       // SQ_NORTH_CAROLINA_AVENUE
  34,       // SQ_PENNSYLVANIA_AVENUE
  37,       // SQ_PARK_PLACE
  39,       // SQ_BOARDWALK
  35,       // SQ_SHORT_LINE_RR
  25,       // SQ_BnO_RR
  15,       // SQ_PENNSYLVANIA_RR
  5,        // SQ_READING_RR
  12,       // SQ_ELECTRIC_COMPANY
  28,       // SQ_WATER_WORKS

  /*    Not ownable squares.  Leave them to be complete.  */

  7,                // SQ_CHANCE_1
  10,               // SQ_JUST_VISITING
  17,               // SQ_COMMUNITY_CHEST_2
  20,               // SQ_FREE_PARKING
  22,               // SQ_CHANCE_2
  30,               // SQ_GO_TO_JAIL
  33,               // SQ_COMMUNITY_CHEST_3
  36,               // SQ_CHANCE_3
  38,               // SQ_LUXURY_TAX
  40,               // SQ_IN_JAIL
  41,               // SQ_OFF_BOARD
    0,              // SQ_GO
    2,              // SQ_COMMUNITY_CHEST_1
    4               // SQ_INCOME_TAX
};

/*  Average landing frequency per lot on monopoly.  Used to determine
    where houses should be built first.   Might consider making this
    non-constant as part of AI_Data, customized for each AI. */
const double  AI_Monopoly_Average_Landing_Frequency[SG_PARK_PLACE + 1] =
{
    0.785,      //  SG_MEDITERRANEAN_AVENUE
    0.883,      //  SG_ORIENTAL_AVENUE
    0.957,      //  SG_ST_CHARLES_PLACE
    1.103,      //  SG_ST_JAMES_PLACE
    1.093,      //  SG_KENTUCKY_AVENUE
    1.003,      //  SG_ATLANTIC_AVENUE
    0.957,      //  SG_PACIFIC_AVENUE
    0.890       //  SG_PARK_PLACE
};

/* Lists the most expensive property in each monopoly. */
RULE_SquareType AI_Expensive_Monopoly_Square[SG_PARK_PLACE + 1] =
{
  SQ_BALTIC_AVENUE,
  SQ_CONNECTICUT_AVENUE,
  SQ_VIRGINIA_AVENUE,
  SQ_NEW_YORK_AVENUE,
  SQ_ILLINOIS_AVENUE,
  SQ_MARVIN_GARDENS,
  SQ_PENNSYLVANIA_AVENUE,
  SQ_BOARDWALK
};

/*  Darzinskis calculated, by playing out 14,145 games, which monopolies
    won above average if all 8 monopolies were created at the same time by
    different players.  This, of course, depended on liquid assets
    available to each player.  These fractions are encoded here. The array
    is a %chance above normal to win if liquid assets available are
    0, 500, 1000, 1500, 2000, ...., 4500, 5000.  Hence, if = 0, 1/8 of
    the time will win in 8 player monopoly.  Darzinskis does not show numbers
    below 0, hence there are extrapolations/estimates.  */
double AI_Monopoly_To_Assets_Importance[SG_PARK_PLACE + 1][5000 / 20 + 1] =
{
    //  SG_MEDITERRANEAN_AVENUE  - this actually never won above average.
    //  only estimated values right now.
    {-13.4, -13.5,  -13.7,  -14.0,  -14.5,  -14.9,  -15.4,  -16.1, -16.9,   -17.8,
     -19.0, -20.0,  -21.0,  -22.0,  -23.0,  -24.0,  -25.0,  -26.0,  -27.0,  -28.0, -29.0},

    //  SG_ORIENTAL_AVENUE
    {10.0,  8.0,    6.0,    4.0,    2.0,    0.6,    0,      -0.6,   -1.0,   -1.5,
     -2.0,  -3.0,   -4.0,   -5.0,   -6.0,   -7.0,   -8.0,   -9.0,   -10.0,  -11.0,  -12.0},

    //  SG_ST_CHARLES_PLACE
    {0,     0.05,   0.1,    0.15,   0.3,    0.9,    1.5,    0.5,    0.1,    -0.5,
    -1.0,   -1.5,  -2.0,    -2.8,   -3.6,   -4.4,   -5.2,   -6.0,   -6.8,   -7.6,   -8.4},

    //  SG_ST_JAMES_PLACE
    {-1.0,  -0.5,   -0.2,   0.1,    0.4,    1.1,    1.7,    1.3,    0.9,    0.8,
      0.7,  0.665,  0.63,   0.595,  0.560,   0.525, 0.490,  0.455,  0.42,   0.385,  0.35},

    //  SG_KENTUCKY_AVENUE
    {-3.0,  -2.5,   -2.0,   -1.5,  -1.0,    -0.5,   -0.1,   0.3,    0.5,    0.6,
     0.5,   0.5,    0.5,    0.5,    0.5,    0.5,    0.5,    0.4,    0.4,    0.4,    0.5},

    //  SG_ATLANTIC_AVENUE
    {-3.3,  -2.8,   -2.3,   -1.8,  -1.3,    -0.8,   -0.3,   0.1,    0.3,    0.35,
     0.39,  0.4,    0.4,    0.4,    0.4,    0.4,    0.4,    0.35,   0.35,   0.38,   0.45},

    //  SG_PACIFIC_AVENUE
    {-5.0,  -4.5,   -4.0,   -3.5,   -3.0,   -2.5,   -2.0,   -1.5,   -1.0,   -0.5,
        0,   0.15,  0.22,   0.29,   0.4,    0.45,   0.52,   0.6,    0.70,   0.78,   0.85},

    //  SG_PARK_PLACE
    {-2.5,  -2.0,   -1.5,   -1.0,   -0.5,   0,      0.4,    0.48,   0.50,   0.49,
     0.49,  0.49,   0.49,   0.49,   0.49,   0.49,   0.48,   0.39,   0.39,   0.37,   0.36}
};

AI_Group_Start_End_Record AI_Group_Start_End[] =
{   /* Start */         /* End */
        {1,                  3},
        {6,                  9},
        {11,                 14},
        {16,                 19},
        {21,                 24},
        {26,                 29},
        {31,                 34},
        {37,                 39},
        {5,                  35},
        {12,                 28}
};

/*  AI data records. */
AI_Data_Record AI_Data[RULE_MAX_PLAYERS];
AI_Data_Record AI_Expert_AI_Settings;

/* General state of the game. */
AI_Global_Record AI_Glob;

/* Config structure for a drone game. */
AI_Drone_Configs_Record AI_Drone_Configs;

/* Hotseat demo info window. */
BOOL ShowAIInfo;  /* This value keeps track of whether to show
                     text box showing the state of the AIs. */
HWND hdlgAIInfoBox = NULL;  /* Handle for AI info window. */
BOOL ShowOnlyDrone = FALSE; /* Should we only display drone results, or everything? */

/* **** TEMP CODE **** */
int  TEST_total_wins[RULE_NOBODY_PLAYER + 1];
BOOL TEST_first_time_around = TRUE;
BOOL AI_Drone_Game = FALSE;
int AI_Test_Player_Just_Won = 0;
DWORD   AI_TEST_GAME_STARTING_TIME;
long AI_Test_Turns_Taken[RULE_MAX_PLAYERS];
double  AI_Test_Average_Time_Per_Game, AI_Test_Average_Turns_Per_Game;
long    AI_Test_num_games_totalled = 0;
extern float AI_TEST_chances_change, AI_TEST_worth_change, AI_TEST_prop_importance, AI_TEST_evaluation;
/* **** TEMP CODE **** */

/***********************        EXTERNS     **********************/
extern AI_Trade_List_Record  Current_Trade_List[RULE_MAX_PLAYERS];
extern AI_Sending_Trade_State AI_PLAYER_SENDING_TRADE;
extern BOOL AI_SEND_TRADE_OFFER_TRADE;

/************************************************************/
/* AI_InitializeSystem                                    */
/************************************************************/

BOOL AI_InitializeSystem(void)
{
    BOOL return_value = TRUE;

    /*
        - Note: This function call must be able to handle repeated
          calls to itself appropriately.
    */
    AI_Reset_Player_Records();
    AI_Reset_Trade_List();
    AI_Initialize_Loading();
    AI_Initialize_Global();
    AI_Initialize_Test();

    // Load up the expert settings for the AI 'use the force' mode.
    if(!AI_Load_AI(RULE_MAX_PLAYERS, 0, 3))
        return_value = FALSE;

    AI_PLAYER_SENDING_TRADE = AI_Send_Nothing;  /* Let an AI send a trade. */
    return (return_value);
}

/*  The following initializes AI_Glob. */
void AI_Initialize_Global(void)
{
    AI_Glob.PLAYER_CURRENTLY_BIDDING = FALSE;
    AI_Glob.AUCTION_IS_ON = FALSE;
    AI_Glob.Player_Purchasing_Property = RULE_NOBODY_PLAYER;
    AI_Glob.Last_Player_Asked_To_Roll = RULE_NOBODY_PLAYER;
    AI_Glob.Time_Last_Player_Asked_To_Roll = 0;
    AI_Glob.Trade_Just_Rejected_Countered = FALSE;
    AI_Glob.Player_Proposed_Trade = AI_Glob.Player_Last_Edited = RULE_NOBODY_PLAYER;
    AI_Glob.Buy_Sell_Mort_Player = RULE_NOBODY_PLAYER;
    return;
}

/*  The following initializes test variables/temp variables. */
void AI_Initialize_Test(void)
{
    memset(AI_Test_Turns_Taken, 0, sizeof(AI_Test_Turns_Taken));

    /* *** TEST CODE *** */
    if(TEST_first_time_around)
        {
        memset(&AI_Drone_Configs, 0, sizeof(AI_Drone_Configs) );
        AI_Load_Drone_Mode_Configs(AI_DRONE_FILE_NAME);
        memset(TEST_total_wins, 0, sizeof(TEST_total_wins));
        TEST_first_time_around = FALSE;
        AI_Test_num_games_totalled = 0;
        AI_Test_Average_Time_Per_Game = AI_Test_Average_Turns_Per_Game = 0.0;
        }
    /* *** TEST CODE *** */
    return;
}

/************************************************************/
/* AI_CleanAndRemoveSystem                                */
/************************************************************/
void AI_CleanAndRemoveSystem(void)
{
    /*
        - Note: This function call must be able to handle repeated
          calls to itself appropriately.
    */
}

void AI_Reset_Player_Records(void)
{
    /*  need to make changes to GameStateRecord to include AI data first. */
    memset(AI_Data, 0, sizeof(AI_Data_Record) * RULE_MAX_PLAYERS);
    memset(AI_State, 0, sizeof(AI_Message_State_Record) * RULE_MAX_PLAYERS);
    return;
}

/******************************************************************************/
/*                  AI_Choose_Unmortgage_For_Free                             */
/*                                                                            */
/*   Input: RULE_PlayerNumber       player        Whose properties to unmgge? */
/*          RULE_PropertySet    properties    The properties that can be      */
/*                                         unmortgaged without 10% penalty.   */
/*                                                                            */
/*      Called after a player has bankrupt on player's properties, and player */
/*  has payed the 10% penalty for transfering properties.  If unmortgaged now,*/
/*  will not have to pay an extra 10% later.  The routine first spends all    */
/*  excess cash on buying houses, and whatever remains is spent on            */
/*  unmortgaging properties.  Does not use the 10% incentive to alter its     */
/*  unmortgaging tactic, and unmortgages all properties it can whether under  */
/*  this 10% grace or not.                                                    */
/******************************************************************************/

void AI_Choose_Unmortgage_For_Free(RULE_PlayerNumber player,
                                   RULE_PropertySet properties)
{
    if(properties == 0) /* If no properties to unmortgage for free */
        return; /* Quit. bogus call. */

    /* Are we in the middle of doing something?  Then wait until it is done. */
    if(AI_State[player].num_actions)
        {
        /* Make sure we get called back again. */
        AI_State[player].message_to_send.action = ACTION_RESTART_PHASE;
        AI_State[player].message_to_send.fromPlayer = player;
        AI_State[player].message_to_send.toPlayer = RULE_BANK_PLAYER;
        return; /* Player in the middle of doing something. */
        }

    /* Ignore the 10% rule.  We don't want to unmortgage properties we
       otherwise wouldn't have.  Simply unmortage the properties we
       would without the rule. */
    AI_Glob.Player_Unmortgaging_For_Free = TRUE;

    if(AI_Should_Unmortgage_Property(&UICurrentGameState, player))
        {
        if(AI_Unmortgage_Property(&UICurrentGameState, player, FALSE, FALSE) != 0)
            {
            /* Make sure we get called back again. */
            AI_State[player].message_to_send.action = ACTION_RESTART_PHASE;
            AI_State[player].message_to_send.fromPlayer = player;
            AI_State[player].message_to_send.toPlayer = RULE_BANK_PLAYER;
            AI_Glob.Player_Unmortgaging_For_Free = FALSE;
            return; /* Done something for now. */
            }
        }

    AI_Glob.Player_Unmortgaging_For_Free = FALSE;

    /* Didn't unmortgage anything.  Hence finished free unmortgaging. */
    AI_SendAction (ACTION_FREE_UNMORTGAGE_DONE, player, RULE_BANK_PLAYER,
                     0, 0, 0, 0,NULL, 0, NULL);
    return;
}

/******************************************************************************/
/*                  AI_Unmortgage_Property                                    */
/*                                                                            */
/*   Input: RULE_GameStatePointer   game_state                                */
/*          RULE_PlayerNumber       player        Whose properties to unmgge? */
/*          BOOL           only_monopolies      If TRUE, will unmortgage only */
/*                              monopolies player owns.                       */
/*          BOOL                hypothetical    Will alter game_state itself  */
/*                      if TRUE, or send ACTION messages if FALSE.            */
/*   Output:  RULE_SquareType  - Property unmortgaged.  If none found, 0.     */
/*                          If had to unmortgage to get cash, SQ_OFF_BOARD.   */
/*                                                                            */
/*      Will find the best property to unmortgage and send the ACTION message */
/*  to do it.  Will first unmortgage the cheapest monopoly other than BALTIC  */
/*  monopoly.  If none found, will unmortgage the BALTIC monopoly.  If can't, */
/*  will unmortgage the property that will increase most player's expected    */
/*  rent.                                                                     */
/******************************************************************************/

RULE_SquareType AI_Unmortgage_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber player, BOOL only_monopolies, BOOL hypothetical)
{
    RULE_PropertySet    properties_owned = RULE_PropertySetOwnedByPlayer(game_state,player);
    RULE_SquareType     SquareNo;
    RULE_SquarePredefinedInfoRecord     *Square_predefined_info = &RULE_SquarePredefinedInfo[SQ_ORIENTAL_AVENUE];
    RULE_SquareInfoRecord   *Square_info = &game_state->Squares[SQ_ORIENTAL_AVENUE];
    long                excess_cash_mortgage, excess_cash, cash;


    /*  First, calculate how much excess cash we have if we mortgage
        non-monopoly territory and if we consider only cash on hand. */
    excess_cash = AI_Excess_Cash_Available(game_state, player, TRUE);
    excess_cash_mortgage = AI_Excess_Cash_Available(game_state, player, FALSE);
    cash = game_state->Players[player].cash;

    /*  Go through all monopolies and unmortgage the cheapest one, unless of course
        it is the baltic monopoly. */
    for(SquareNo = SQ_ORIENTAL_AVENUE; SquareNo < SQ_IN_JAIL;SquareNo++, Square_predefined_info++, Square_info++ )
        {
        if(!Square_info->mortgaged)
            continue;   /* Not mortgaged - can't unmortgage. */

        if((RULE_PropertyToBitSet(SquareNo) & properties_owned) != 0)
            {   /* We own this square? */
            if(AI_Test_For_Monopoly(properties_owned, SquareNo))
                {   /* Is this a monopoly? */
                /* Can we unmortgage it? */
                if(excess_cash_mortgage >=  ((long) Square_predefined_info->mortgageCost * 1.1))
                    {   /* We found one.  Unmortgage it. */
                    if(hypothetical)
                        {
                        Square_info->mortgaged = FALSE;
                        game_state->Players[player].cash -= (long) (Square_predefined_info->mortgageCost * 1.1);
                        }
                    else
                        {
                        if(cash < (Square_predefined_info->mortgageCost * 1.1))
                            {   /* Not enough cash. Will have to first mortgage something. */
                            AI_Mortgage_Worst_Property(game_state, player, FALSE, FALSE, FALSE);
                            SquareNo = SQ_OFF_BOARD;  /* A flag to say we did something,
                                                         but didn't unmortgage property. */
                            }
                        else
                            {
                            AI_SendAction (ACTION_MORTGAGING, player, RULE_BANK_PLAYER,
                                     SquareNo, 0, 0, 0,NULL, 0, NULL);
                            }
                        }
                    return (SquareNo);
                    }
                }
            }
        }

    /* Now check the baltic monopoly. */
    Square_predefined_info = RULE_SquarePredefinedInfo;
    Square_info = game_state->Squares;

    for(SquareNo = 0; SquareNo <= SQ_BALTIC_AVENUE; SquareNo++, Square_predefined_info++, Square_info++ )
        {
        if(!Square_info->mortgaged)
            continue;   /* Not mortgaged - can't unmortgage. */

        if((RULE_PropertyToBitSet(SquareNo) & properties_owned) != 0)
            {   /* We own this square? */
            if(AI_Test_For_Monopoly(properties_owned, SquareNo))
                {   /* Is this a monopoly? */
                /* Can we unmortgage it? */
                if(excess_cash_mortgage >= ((long) Square_predefined_info->mortgageCost * 1.1))
                    {   /* We found found.  Unmortgage it. */
                    if(hypothetical)
                        {
                        Square_info->mortgaged = FALSE;
                        game_state->Players[player].cash -= (long) (Square_predefined_info->mortgageCost * 1.1);
                        }
                    else
                        {
                        if(cash < (Square_predefined_info->mortgageCost * 1.1))
                            {   /* Not enough cash. Will have to first mortgage something. */
                            AI_Mortgage_Worst_Property(game_state, player, FALSE, FALSE, FALSE);
                            SquareNo = SQ_OFF_BOARD;  /* A flag to say we did something,
                                                         but didn't unmortgage property. */
                            }
                        else
                            {
                            AI_SendAction (ACTION_MORTGAGING, player, RULE_BANK_PLAYER,
                                     SquareNo, 0, 0, 0,NULL, 0, NULL);
                            }
                        }
                    return (SquareNo);
                    }
                }
            }
        }

    /* No monopolies.  Then unmortgage the property that will give us the
       highest rent. */
    if(!only_monopolies)
        {
        SquareNo = AI_Find_Highest_Rent_Mortgaged(game_state, player, properties_owned, excess_cash);

        /* Did we find a property to unmortgage? */
        if(SquareNo != 0)
            {
            if(hypothetical)
                {
                game_state->Squares[SquareNo].mortgaged = FALSE;
                game_state->Players[player].cash -= (long) (Square_predefined_info->mortgageCost * 1.1);
                }
            else
                if(cash >= RULE_SquarePredefinedInfo[SquareNo].mortgageCost * 1.1)
                    {
                    AI_SendAction (ACTION_MORTGAGING, player, RULE_BANK_PLAYER,
                                   SquareNo, 0, 0, 0,NULL, 0, NULL);
                    }
                else
                    SquareNo = 0;

            return (SquareNo);
            }
        }

    /* Didn't find a property we can unmortgage. */
    return 0;
}

/******************************************************************************/
/*                  AI_Give_List_Players_Important_Monopoly                   */
/*                                                                            */
/*   Input: RULE_GameStatePointer   game_state                                */
/*          RULE_SquareType         property        Check this property       */
/*          RULE_PlayerNumber       *player_list    List of players to test   */
/*          RULE_PlayerNumber       *must_list      List of players in *list  */
/*                            which at least one of must appear in the trades.*/
/*          RULE_PlayerNumber *lowest_trade_list    Where to store best trade */
/*          int                     players_in_list  How many players?        */
/*          int                     num_must_players  How many in *must_list? */
/*          int     lowest_size_trade   Largest # of people in group returned */
/*                                      must be at most this.  Used to keep   */
/*                                      track of lowest # of people found.    */
/*          int     get_lowest_trade_number   # of lowest trade to return.    */
/*                      This requires some explanation.  When looking for a   */
/*                      trade, we might find that the lowest trade possible   */
/*                      is between 3 players.  However, we might also find    */
/*                      that there are 4 possible trades between 3 players.   */
/*                      get_lowest_trade_number will make us look for the x'th*/
/*                      possible trade and return it in *lowest_trade_list.   */
/*                      If get_lowest_trade_number is set to a number higher  */
/*                      than possible lowest trades, then no trade will be    */
/*                      found.                                                */
/*          int     *found_x_trades     Where to store how many lowest trades */
/*                      we found.  see get_lowest_trade_number.               */
/*   Output:  int   - Lowest # of people in group trade for which property    */
/*                    gives players in *list an important                     */
/*                    monopoly.  If property == 0, lowest possible # of       */
/*                    players for which a group trade                         */
/*                    between players (or subgroup) is possible.  Returns     */
/*                    in *lowest_trade_list the players in this lowest trade. */
/*                                                                            */
/*      Finds out if 'property' is important to the players in *list.  A      */
/*  property is important if it gives N players who each do not posses a      */
/* monopoly, but who together posses N - 1 monopolies, an extra monopoly,or   */
/* for which this works on a subgroup of *player_list. Also, if it gives a    */
/* direct monopoly, then it is considered important.  Hence this function     */
/* calls itself recursively to find all possibilities.  By setting property to*/
/* 0, will check to see whether a group trade is possible.  Will search out   */
/* all possible trades and find the smallest subgroup for which property is   */
/* important. Each trade considered will have at least one player in          */
/* *must_list.  In *found_x_trades will store how many trades are possible    */
/* between as many players as there are in the smallest subgroup found.  If   */
/* get_lowest_trade_number is larger than this, will return lowest_size_trade.*/
/* Otherwise, will set *lowest_trade_list with the player list for the        */
/* get_lowest_trade_number  trade. (ie. if gltn = 1, will return the first    */
/* list of players it finds.  If gltn = 2, will return the second list of     */
/* players it finds, etc. )                                                   */
/******************************************************************************/

int AI_Give_List_Players_Important_Monopoly(RULE_GameStatePointer game_state, RULE_SquareType property,
                                             RULE_PlayerNumber *player_list, RULE_PlayerNumber *must_list,
                                             RULE_PlayerNumber *lowest_trade_list, int num_players,
                                             int num_must_players, int lowest_size_trade,
                                             int get_lowest_trade_number, int *found_x_trades)
{
    int     num_monopolies, num_monopolies_with_prop;
    int     index, index2, index3;
    RULE_PlayerNumber   swap_temp, *player_point1, *player_point2;
    BOOL    found_match = FALSE;
    int     lowest_trade_found, current_trade;

    lowest_trade_found = lowest_size_trade;

    if(num_players < 1)  /* WHOA! Less than 1 player? Something wrong here! */
        return lowest_trade_found;

    /* See how many properties they have before acquiring property. */
    num_monopolies = AI_Num_Of_Monopolies_Between_Players(game_state, player_list, num_players, 0);

    /* See how many properties they have after acquiring the property */
    num_monopolies_with_prop = AI_Num_Of_Monopolies_Between_Players(game_state, player_list, num_players, property);

    /* Will it allow them a group trade that wasn't there before? */
    if(num_monopolies < num_players  &&  num_monopolies_with_prop >= num_players)
        found_match = TRUE;
    else
        /* Is this property 0?  Then check for group trades only. */
        if(property == 0)
            {
            if(num_monopolies >= num_players)
                {   /* Group trade is possible. */
                found_match = TRUE;
                }
            }

    /*  Wait a sec - are we checking only 1 player?  Then it is important if
        it gives a direct monopoly! */
    if(num_players == 1)
        {
        if(num_monopolies != num_monopolies_with_prop)
            found_match = TRUE;
        }

    /* Did we find a lower trade?  Then save it. */
    if(found_match && (lowest_trade_found >= num_players))
        {
        if(lowest_trade_found > num_players)
            {   /* Is this between fewer players than before? */
            /* Then save the # of players in this trade. */
            lowest_trade_found = num_players;
                /* Restart the counter counting how many such trades we found. */
            *found_x_trades = 1;
            }
        else
            {   /* Is this a trade just as good as our best trade? */
            /* Then increase our counter. */
            *found_x_trades += 1;
            }

        /*  Is this the trade # we are looking for? If yes, save it. */
        if(get_lowest_trade_number == *found_x_trades)
            memcpy(lowest_trade_list, player_list, sizeof(RULE_PlayerNumber) * lowest_trade_found);
        }

    /*  Now check all of the subgroups. */
    for(index = 0; index < num_players; ++index)
        {
        /* Swap current player with the end of the list, so won't be seen. */
        swap_temp = player_list[index];
        player_list[index] = player_list[num_players - 1];
        player_list[num_players - 1] = swap_temp;

        /* Is at least one player in must_list in this new subgroup? */
        player_point1 = player_list;
        found_match = FALSE;

        if(num_must_players <= 0)
            found_match = TRUE; /* Empty must_players - no requirements then. */
        else
            {
            for(index2 = 0; index2 < num_players - 1; ++index2, ++player_point1)
                {
                player_point2 = must_list;
                for(index3 = 0; index3 < num_must_players; ++index3, ++player_point2)
                    {
                    if(*player_point1 == *player_point2)
                        {
                        found_match = TRUE;
                        break;
                        }
                    }

                if(found_match)
                    break;
                }
            }

        if(found_match)
            {
            current_trade = AI_Give_List_Players_Important_Monopoly(game_state, property, player_list, must_list,
                                                       lowest_trade_list, num_players - 1, num_must_players, lowest_trade_found,
                                                       get_lowest_trade_number, found_x_trades);
            if(current_trade < lowest_trade_found)
                lowest_trade_found = current_trade;
            }

        /* Swap back. */
        swap_temp = player_list[index];
        player_list[index] = player_list[num_players - 1];
        player_list[num_players - 1] = swap_temp;
        }

    /*  We were asking for the get_lowest_trade_number trade. Did we find that many? */
    if(lowest_trade_found != lowest_size_trade)
        {
        if(*found_x_trades >= get_lowest_trade_number)
            return (lowest_trade_found);    /* We found one. */
        }

    return (lowest_size_trade); /* There weren't get_lowest_trade_number trades possible
                                   between lowest_trade_found players. */
}

/******************************************************************************/
/*                  AI_Give_Players_Important_Monopoly                        */
/*                                                                            */
/*   Input: RULE_GameStatePointer   game_state                                */
/*          RULE_SquareType         property        Check this property       */
/*          RULE_PlayerNumber       exclude_player  Who to exclude?           */
/*          RULE_PlayerNumber       include_player  Who must be in the trade? */
/*   Output: AI_Property_Importance  - how important is this property?        */
/*                                                                            */
/*      Finds out if 'property' is important to any players except for        */
/*  exclude_player, but trade must include include_player. Calls              */
/*  AI_Give_Players_Important_Monopoly.  It first checks to see whether it    */
/*  allows a single or group trade to players who do not have a group trade.  */
/*  Otherwise, it checks whether it gives players with monopolies a direct    */
/*  monopoly.  Returns AI_Property_Importance to signify how important it is. */
/******************************************************************************/

AI_Property_Importance AI_Give_Players_Important_Monopoly(RULE_GameStatePointer game_state, RULE_SquareType property,
                                        RULE_PlayerNumber exclude_player, RULE_PlayerNumber include_player)
{
    RULE_PlayerNumber player_list[RULE_MAX_PLAYERS], lowest_trade_list[RULE_MAX_PLAYERS],
                      cur_player, player_save_list[RULE_MAX_PLAYERS];
    int               num_players_in_list = 0, lowest_trade_found = -1, current_2_player_trade = 1;
    int               found_number_trades, cur_list_size;
    RULE_GameStateRecord    new_state;
    RULE_SquareType   buying_square;
    RULE_PlayerNumber buying_player;

    /* Create a temporary copy of game_state */
    memcpy(&new_state,game_state, sizeof(RULE_GameStateRecord) );

    /* If a player is currently buying a property, act as if the player
       has already bought it, unless, of course, it is the bid property. */
    buying_square = AI_Property_Being_Bought(game_state, &buying_player);

    if((buying_square != SQ_MAX_SQUARE_TYPES) && (buying_square != property))
        {   /* Someone currently buying a property. */
        new_state.Squares[buying_square].owner = buying_player;
        }

    /*  Check if this is a cash cow. */
    if(AI_Is_Cash_Cow(property))
        return (AI_PROPERTY_IS_CASH_COW);

    /*  Check if this property gives someone a DIRECT monopoly. */
    if(include_player != RULE_NOBODY_PLAYER)
        {
        player_list[0] = include_player;
        lowest_trade_found = AI_Give_List_Players_Important_Monopoly(&new_state, property, player_list, &include_player,
                                  lowest_trade_list, 1, 1, RULE_MAX_PLAYERS + 1, 1, &found_number_trades);

        if(lowest_trade_found == 1)
            return AI_PROPERTY_GIVES_DIRECT_MONOPOLY;   /* It is important.  */
        }
    else
        {
        for(cur_player = 0; cur_player < new_state.NumberOfPlayers; ++cur_player)
            {
            if(cur_player == exclude_player)
                continue;

            player_list[0] = cur_player;
            lowest_trade_found = AI_Give_List_Players_Important_Monopoly(&new_state, property, player_list, &include_player,
                                              lowest_trade_list, 1, 0, RULE_MAX_PLAYERS + 1, 1, &found_number_trades);

            if(lowest_trade_found == 1)
                return AI_PROPERTY_GIVES_DIRECT_MONOPOLY;   /* It is important.  */
            }
        }

    /* initialize list. Want to check for a trade with players without a monopoly. */
    for(cur_player = 0; cur_player < new_state.NumberOfPlayers; ++cur_player)
        {
        if(cur_player == exclude_player)
            continue;

        if(!AI_Player_Own_Monopoly(&new_state, cur_player, FALSE))
            {  /* only check players without monopolies. */
            player_list[num_players_in_list] = cur_player;
            num_players_in_list++;
            }
        }

    /* save the list. */
    memcpy(player_save_list, player_list, sizeof(RULE_PlayerNumber) * RULE_MAX_PLAYERS);

    /* Now check for possible trades without this property.  We will assume that the
       trades that can happen now will happen.  Find all these possible trades and
       take out the players affected.  (Note: for an 8 player game, there could
       possibly be 8 choose 1  * 8 choose 2 * ... * 8 choose 7  possible combination
    .  of trades.  This would take too much computation time.  Hence we will try
       out all the possible 2 player trades, but assume the trades that would
       follow are the first we see.  (Note: if no 2 player trades exist, will
       go through all the possible 3 player trades.  If no 3 player trades, etc. ) */

    do  /* Go through all possible 2-player trades. */
        {
        /* Restore the list. */
        memcpy(player_list, player_save_list, sizeof(RULE_PlayerNumber) * RULE_MAX_PLAYERS);
        cur_list_size = num_players_in_list;

        do  {
            /* Go through the next 2-player trade. */
            lowest_trade_found = AI_Give_List_Players_Important_Monopoly(&new_state, 0, player_list, &include_player,
                                      lowest_trade_list, cur_list_size, (include_player == RULE_NOBODY_PLAYER ? 0:1),
                                      RULE_MAX_PLAYERS + 1, current_2_player_trade, &found_number_trades);

            if(lowest_trade_found != (RULE_MAX_PLAYERS + 1))
                {
                /* We found a possible trade.  Take out all players affected. */
                AI_Remove_Players_From_List(player_list, lowest_trade_list, cur_list_size, lowest_trade_found);
                cur_list_size -= lowest_trade_found;
                }

            } while(lowest_trade_found != (RULE_MAX_PLAYERS + 1));

        if((cur_list_size != 0) && (cur_list_size != num_players_in_list))
            {   /* Somebody still in the list.  Excellent.  Check whether this property
               is important to them */
            lowest_trade_found = AI_Give_List_Players_Important_Monopoly(&new_state, property, player_list, &include_player,
                                      lowest_trade_list, cur_list_size, (include_player == RULE_NOBODY_PLAYER ? 0:1),
                                      RULE_MAX_PLAYERS + 1, 1, &found_number_trades);

            if(lowest_trade_found != (RULE_MAX_PLAYERS + 1))
                return AI_PROPERTY_ALLOWS_TRADE;   /* It is important.  */
            }

        current_2_player_trade++;
        }
        while(cur_list_size != num_players_in_list);

    /* Not important. */
    return AI_PROPERTY_NOT_IMPORTANT;
}

/******************************************************************/
/*                  AI_Move_AI_Player                             */
/*                                                                */
/*   Input:  RULE_PlayerNumber  player                            */
/*                                                                */
/*                                                                */
/*      Used to signal that it is time for the AI player to move. */
/*   Will choose the best course of action and complete it.       */
/******************************************************************/

void AI_Move_AI_Player(RULE_PlayerNumber player)
{
    if(AI_State[player].num_actions)
        return;     /* Player in the middle of doing something. */

    if(!AI_Spend_Excess_Assets(&UICurrentGameState, player))
        {
        AI_Roll_Dice(player);   /* Didn't do anything - roll dice. */
        return;
        }

    /* Did something else other than roll dice - then resend the message. */
    AI_State[player].message_to_send.action = NOTIFY_PLEASE_ROLL_DICE;
    AI_State[player].message_to_send.fromPlayer = player;
    AI_State[player].message_to_send.toPlayer = player;
    AI_State[player].message_to_send.numberA = player;
    AI_State[player].message_to_send.numberB = UICurrentGameState.Players[player].currentSquare;
    return;
}

/******************************************************************/
/*                  AI_Done_Turn_AI_Player                        */
/*                                                                */
/*   Input:  RULE_PlayerNumber  player                            */
/*                                                                */
/*                                                                */
/*      Signals the AI player is finished, acknowledge end of     */
/*   turn is the expected response.                               */
/******************************************************************/

void AI_Done_Turn_AI_Player(RULE_PlayerNumber player)
{
    if(AI_State[player].num_actions)
        return;     /* Player in the middle of doing something. */

    if(!AI_Spend_Excess_Assets(&UICurrentGameState, player))
        {
        AI_Done_Turn(player);   /* Didn't do anything - roll dice. */
        return;
        }

    /* Did something else other than roll dice - then resend the message. */
    //AI_SendAction (ACTION_END_TURN, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
    AI_State[player].message_to_send.action = ACTION_END_TURN;
    AI_State[player].message_to_send.fromPlayer = player;
    AI_State[player].message_to_send.toPlayer = RULE_BANK_PLAYER;
    AI_State[player].message_to_send.numberA = player;
    AI_State[player].message_to_send.numberB = 0;
    return;
}

/**********************************************************************/
/*                  AI_Should_Unmortgage_Property                     */
/*                                                                    */
/*   Input: RULE_GameStatePointer  game_state                         */
/*          RULE_PlayerNumber      player      Who should unmortgage? */
/*                                                                    */
/*      Returns TRUE if enough excess cash is available to unmortgage */
/*   a property, and a mortgaged property exists.  If not enough      */
/*   excess cash is available, or if no mortgage property is owned,   */
/*   then returns FALSE.                                              */
/**********************************************************************/

BOOL  AI_Should_Unmortgage_Property(RULE_GameStatePointer  game_state, RULE_PlayerNumber player)
{
    RULE_PropertySet    properties_owned = RULE_PropertySetOwnedByPlayer(game_state,player);
    RULE_SquareType     SquareNo;
    RULE_SquarePredefinedInfoRecord     *Square_predefined_info = RULE_SquarePredefinedInfo;
    RULE_SquareInfoRecord   *Square_info = game_state->Squares;
    long                excess_cash_mortgage, excess_cash;
    RULE_GameStateRecord  new_state;
    BOOL                housing_shortage;

    /*  First, consider the state after we bought all the houses we want. */
    memcpy(&new_state, game_state, sizeof(RULE_GameStateRecord));

    housing_shortage = AI_Housing_Shortage(game_state, AI_CRITICAL_HOUSING_LEVEL);

    /* First, buy houses. */
    if(!housing_shortage)
        {
        /* Keep buying houses as long as we can.  Note: we assume enough houses */
        while(AI_Hypothetical_Buy_Houses(&new_state, player) );
        }

    /*  Calculate how much excess cash we have if we mortgage
        non-monopoly territory and if we consider only cash on hand. */
    excess_cash = AI_Excess_Cash_Available(&new_state, player, TRUE);
    excess_cash_mortgage = AI_Excess_Cash_Available(&new_state, player, FALSE);

    /*  Now find the best property to unmortgage in our price range.
        use excess_cash_mortgage for monopolies, and
        excess_cash for non-monopolies. */
    for(SquareNo = 0; SquareNo < SQ_IN_JAIL;SquareNo++, Square_predefined_info++, Square_info++ )
        {
        if(!Square_info->mortgaged)
            continue;   /* Not mortgaged - can't unmortgage. */

        if((RULE_PropertyToBitSet(SquareNo) & properties_owned) != 0)
            {   /* We own this square? */
            if(AI_Test_For_Monopoly(properties_owned, SquareNo))
                {   /* Is this a monopoly? */
                /* Can we unmortgage it? */
                if(excess_cash_mortgage >= (long) (Square_predefined_info->mortgageCost * 1.1))
                    return TRUE;
                }
            else
                {   /* Not a monopoly.*/
                /* Can we unmortgage it? */
                if(excess_cash >= (long) (Square_predefined_info->mortgageCost * 1.1))
                    return TRUE;
                }
            }
        }

    /* Haven't found anything! */
    return FALSE;
}

/******************************************************************/
/*                  AI_Roll_Dice                                  */
/*                                                                */
/*   Input:  RULE_PlayerNumber  player                            */
/*                                                                */
/*      Sends the action message to roll dice.                    */
/******************************************************************/

void AI_Roll_Dice(RULE_PlayerNumber player)
{
    AI_SendAction (ACTION_ROLL_DICE, player, RULE_BANK_PLAYER,
                             0, 0, 0, 0,NULL, 0, NULL);
    return;
}
/******************************************************************/
/*                  AI_Done_Turn                                  */
/*                                                                */
/*   Input:  RULE_PlayerNumber  player                            */
/*                                                                */
/*      Sends the action message to acknowledge end of turn.      */
/******************************************************************/

void AI_Done_Turn(RULE_PlayerNumber player)
{
    //AI_SendAction (ACTION_END_TURN, player, RULE_BANK_PLAYER,
    AI_SendAction (ACTION_END_TURN, player, RULE_BANK_PLAYER,
                             0, 0, 0, 0,NULL, 0, NULL);
    return;
}

/**********************************************************************/
/*                  AI_Should_Buy_House                               */
/*                                                                    */
/*   Input: RULE_GameStatePointer  game_state                         */
/*          RULE_PlayerNumber      player      Who should buy?        */
/*   Return: EBOOL  - EB_TRUE if should buy houses now, EB_FALSE if   */
/*                    shouldn't buy houses, and EB_LATER if player    */
/*                    should wait with cash to buy in the near future.*/
/*                                                                    */
/*      Returns EB_TRUE if a house is available to buy, enough EXCESS */
/*   cash is available (where excess cash is calculated by            */
/*   AI_Excess_Cash_Available, and includes mortgageable properties.) */
/*   The player must own a monopoly, and there must be a player within*/
/*   12 spaces of the monopoly if the housing strategy for this AI    */
/*   requires it.  There must not be hotels on all our properties,    */
/*   or if 4 houses, there does not exist a housing shortgage.  When  */
/*   these criteria are satisfied, will buy a house.  If all these    */
/*   criteria are satisfied except that no player is within 12 spaces,*/
/*   will return EB_LATER.                                            */
/**********************************************************************/

EBOOL  AI_Should_Buy_House(RULE_GameStatePointer  game_state, RULE_PlayerNumber player)
{
    RULE_SquareType     SquareNo;
    RULE_SquarePredefinedInfoRecord     *Square_predefined_info = RULE_SquarePredefinedInfo;
    RULE_SquareInfoRecord   *Square_info = game_state->Squares;
    long                excess_cash, unmortgage_cost;
    long                monopoly_count;
    RULE_SquareType     monopolies_owned[SG_PARK_PLACE + 1], temp[AI_MAX_MONOPOLY_LOTS + 1];
    int                 index, top_houses_on_lot, buy_x_houses;
    BOOL                found_a_monopoly = FALSE;

    /*  First, calculate how much excess cash we have*/
    excess_cash = AI_Excess_Cash_Available(game_state, player, FALSE);
    if(excess_cash == 0)
        return EB_FALSE;   /* Can't buy houses with dreams.  Need cash. */

    /*  what unmortgaged monopolies do we own?  */
    monopoly_count = AI_Get_Monopolies(game_state, player, monopolies_owned, FALSE);

    if(monopoly_count == 0)
        return EB_FALSE;    /* No monopolies owned - shouldn't buy houses. */

    /*  Are we in a housing shortage?  Will determine how many houses we want on
        each monopoly. */
    if(AI_Housing_Shortage(game_state, AI_CRITICAL_HOUSING_LEVEL))
        top_houses_on_lot = game_state->GameOptions.housesPerHotel - 1;
    else
        top_houses_on_lot = game_state->GameOptions.housesPerHotel;

    /*  Go through the monopolies and see if they are missing houses. */

    for(SquareNo = 0; SquareNo < SQ_IN_JAIL;SquareNo++, Square_predefined_info++, Square_info++ )
        {
        /*  Go through the monopoly groups and see if it belongs to one of our monopolies. */
        for(index = 0; index < monopoly_count; ++index)
            {
            if(Square_predefined_info->group == RULE_SquarePredefinedInfo[monopolies_owned[index]].group)
                {   /* We found one of our own.  Does it have enough houses? */
                if(Square_info->houses < top_houses_on_lot)
                    { /* Not enough houses.  Is anyone close enough to it, if that is important? */
                    if(AI_Data[player].housing_purchase_strat & AI_HOUSE_BUY_WITHIN_12)
                        {
                        if(!AI_Housing_Shortage(game_state, AI_CRITICAL_HOUSING_LEVEL))  /* Ignore 12 spaces for housing shortage */
                            {
                            if(!AI_Player_Close_To_Property(game_state, player, SquareNo, 2, 12))
                                {
                                found_a_monopoly = TRUE;
                                continue;  /* Someone not within 12 spaces? forget it. */
                                }
                            }
                        }

                    /* Good. Can we afford houses here? Make sure at least 3 per lot if AI strategy
                       requires it, as long as housing shortgage does not exist.  */
                    if(AI_Data[player].housing_purchase_strat & AI_HOUSE_BUY_ATLEAST_3  &&
                       !AI_Housing_Shortage(game_state, AI_CRITICAL_HOUSING_LEVEL))
                        {
                        buy_x_houses = AI_Num_Houses_On_Monopoly(game_state, SquareNo);
                        buy_x_houses = AI_Get_Monopoly_Lots(game_state, temp, SquareNo) * 3
                                       - buy_x_houses;

                        if(buy_x_houses < 1)
                            buy_x_houses = 1;
                        }
                    else
                        buy_x_houses = 1;

                    /* Now subtract the money required to unmortgage the monopoly. */
                    unmortgage_cost = AI_Cost_Unmortgage_Monopoly(game_state, SquareNo);

                    if(Square_predefined_info->housePurchaseCost * buy_x_houses <= (excess_cash - unmortgage_cost))
                        return EB_TRUE;  /* YES! Found somewhere to buy! */
                    }
                }
            }
        }

    if(found_a_monopoly)
        return EB_LATER;

    /* Haven't found anything! */
    return FALSE;
}

/**********************************************************************/
/*                  AI_Unmortgage_Monopoly                            */
/*                                                                    */
/*   Input: RULE_GameStatePointer  game_state                         */
/*          RULE_PlayerNumber      player   Who is unmortgaging?      */
/*          RULE_SquareType        SquareNo   What monopoly?          */
/*   Output: BOOL - TRUE if a property was unmortgaged, FALSE if not. */
/*                                                                    */
/*      This function will unmortgage one property from the monopoly  */
/*  containing SquareNo if the monopoly has a mortgaged property.     */
/**********************************************************************/

BOOL AI_Unmortgage_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareType SquareNo)
{
    int     monopoly_lots;
    RULE_SquareType mono_squares[AI_MAX_MONOPOLY_LOTS + 1];

    /* Get the monopoly squares */
    monopoly_lots = AI_Get_Monopoly_Lots(game_state, mono_squares, SquareNo);

    /* Go through them and check if each is mortgaged. */
    while(monopoly_lots > 0)
        {
        /* Next monopoly square mortaged? If yes, unmortgaged it. */
        if(game_state->Squares[mono_squares[monopoly_lots - 1]].mortgaged == TRUE)
            {
            AI_SendAction (ACTION_MORTGAGING, player, RULE_BANK_PLAYER,
                             mono_squares[monopoly_lots - 1], 0, 0, 0, NULL, 0, NULL);
            return TRUE;
            }
        monopoly_lots--;
        }

    return FALSE;
}

/**********************************************************************/
/*                  AI_Buy_Houses                                     */
/*                                                                    */
/*   Input: RULE_GameStatePointer  game_state                         */
/*          RULE_PlayerNumber      player      Who should buy?        */
/*          AI_House_Options       options     specific options?      */
/*   Output: BOOL - TRUE if an action message was sent to buy a house,*/
/*                  FALSE otherwise.                                  */
/*                                                                    */
/*      This routine will buy a house for 'player'.  The following    */
/*  tactic is used:                                                   */
/*      1.  AI_Excess_Cash_Available is used to determine how much    */
/*          the AI is willing to spend on houses.                     */
/*      2.  Obeys AI housing strategy to build only if an opponent    */
/*          is within 12 spaces of a monopoly if housing_purchas_strat*/
/*          has AI_HOUSE_BUY_WITHIN_12 ORed.                          */
/*      3.  If player has multiple monopolies, will try to build      */
/*          on the highest rent monopoly that 'player' can afford to  */
/*          build 3 houses per lot.                                   */
/*      4.  If can't find monopolies with less than 3 houses per lot, */
/*          or player cannot afford it, will build on monopoly with   */
/*          less than 3 houses that has highest landing frequency.    */
/*      5.  If no monopoly with less than 3 houses exists, will build */
/*          on monopoly with highest average landing frequency.       */
/*      6.  When building houses, will first try to build on lot that */
/*          has an enemy player 7 spaces away.                        */
/*      7.  If no enemy is 7 spaces away, will try to build on most   */
/*          expensive lot, and then on the less expensive lots.       */
/**********************************************************************/


BOOL AI_Buy_Houses(RULE_GameStatePointer  game_state,  RULE_PlayerNumber player, AI_House_Options options)
{
    int                 num_monopolies, num_squares_in_monopoly;
    RULE_SquareType     monopolies[SG_PARK_PLACE + 1];
    float               best_rent = (float) 0.0;
    RULE_SquareType     best_square = 0;
    RULE_SquareType     monopoly_squares[AI_MAX_MONOPOLY_LOTS + 1];
    BOOL                within_12_spaces;
    int                 index, index2, top_houses_on_lot, buy_x_houses, temp;
    long                excess_cash, unmortgage_cost;
    RULE_SquareGroups   group;
    int                 free_houses, free_hotels, num_houses;


    /* How much do we have to spend? */
    excess_cash = AI_Excess_Cash_Available(game_state, player, FALSE);
    if(excess_cash == 0)
        return FALSE;   /* Can't buy houses with dreams.  Need cash. */

    free_houses = AI_Free_Houses(game_state);
    free_hotels = AI_Free_Hotels(game_state);

    /*  find out what monopolies player has.  */
    num_monopolies = AI_Get_Monopolies(game_state, player, monopolies, FALSE);

    /*  Are we in a housing shortage?  Will determine how many houses we want on
        each monopoly. */
    if(AI_Housing_Shortage(game_state, AI_CRITICAL_HOUSING_LEVEL))
        top_houses_on_lot = game_state->GameOptions.housesPerHotel - 1;
    else
        top_houses_on_lot = game_state->GameOptions.housesPerHotel;

    /*  Shorten list to those monopolies which have someone within 12 spaces if needed */
    if((AI_Data[player].housing_purchase_strat & AI_HOUSE_BUY_WITHIN_12) &&
        !(options & AI_HOUSEOPS_AUCTION))
        {
        for(index = 0; index < num_monopolies; ++index)
            {
            /* Get the monopoly. */
            num_squares_in_monopoly = AI_Get_Monopoly_Lots(game_state, monopoly_squares, monopolies[index]);

            /* Make sure someone is within 12 spaces of the monopoly, if needed. */
            within_12_spaces = AI_Someone_Close_To_Monopoly(game_state, player,monopoly_squares, num_squares_in_monopoly);

            if(!within_12_spaces)
                {   /* Not within 12 spaces.  delete the entry. */
                for(index2 = index; index2 < num_monopolies - 1; ++index2)
                    monopolies[index2] = monopolies[index2 + 1];

                index--;
                num_monopolies--;
                }
            }
        }

    /*  First, go through all the monopolies with less than 3 houses per lot that we can build up to
        3 houses per lot.  Pick the most expensive.  */
    if((free_houses > 0) && !(options & AI_HOUSEOPS_HOTELS_ONLY))
        {
        for(index = 0; index < num_monopolies; ++index)
            {
            /* Get the monopoly. */
            num_squares_in_monopoly = AI_Get_Monopoly_Lots(game_state, monopoly_squares, monopolies[index]);

            buy_x_houses = AI_Num_Houses_On_Monopoly(game_state, monopolies[index]);
            buy_x_houses = num_squares_in_monopoly * 3 - buy_x_houses;
            if(buy_x_houses < 1)
                continue;   /* built-up.  Consider it later. */

            /* Now figure out if we can build all the houses on it.  Subtract unmortgage costs. */
            unmortgage_cost = AI_Cost_Unmortgage_Monopoly(game_state, monopoly_squares[0]);

            if(unmortgage_cost > 0 && (options & AI_HOUSEOPS_AUCTION))
                continue; /* Housing shortage - can't buy on mortgaged monopolies. */

            if(buy_x_houses * RULE_SquarePredefinedInfo[monopolies[index]].housePurchaseCost > (excess_cash - unmortgage_cost))
                continue;  /* Can't afford to buy so many houses. */

            /*  pick the most expensive monopoly that we can buildup 3 houses on each lot. */
            if(best_rent < RULE_SquarePredefinedInfo[monopolies[index]].rent[0])
                {
                best_rent = (float) RULE_SquarePredefinedInfo[monopolies[index]].rent[0];
                best_square = monopolies[index];
                }
            }

        if(best_square == 0)
            {   /* We didn't find anything with less than 3 houses we can develop.
               We should develop the monopoly with less than 3 houses
               per lot that is landed on the most. */
            for(index = 0; index < num_monopolies; ++index)
                {
                /* Get the monopoly. */
                num_squares_in_monopoly = AI_Get_Monopoly_Lots(game_state, monopoly_squares, monopolies[index]);
                group = RULE_SquarePredefinedInfo[monopolies[index]].group;

                if(AI_Num_Houses_On_Monopoly(game_state, monopolies[index]) >= num_squares_in_monopoly * 3)
                    continue; /* built up to 3 or more houses per lot. consider it later. */

                /* Is this a housing shortage? Ignore mortgaged monopolies. */
                if(options & AI_HOUSEOPS_AUCTION)
                    {
                    unmortgage_cost = AI_Cost_Unmortgage_Monopoly(game_state, monopoly_squares[0]);
                    if(unmortgage_cost > 0)
                        continue; /* Housing shortage - can't buy on mortgaged monopolies. */
                    }

                if(RULE_SquarePredefinedInfo[monopolies[index]].housePurchaseCost > excess_cash)
                    continue;  /* Can't afford a house on this lot. */

                if(best_rent <  AI_Monopoly_Average_Landing_Frequency[group])
                    {
                    best_rent = (float) AI_Monopoly_Average_Landing_Frequency[group];
                    best_square = monopolies[index];
                    }
                }
            }
        }

    if(best_square == 0)
        {   /* We didn't find any monopoly with less than 3 houses, or just couldn't afford
           those that did.  Hence now simply build those that aren't completely built up yet
           in order of landing frequency. */
        for(index = 0; index < num_monopolies; ++index)
            {
            /* Get the monopoly. */
            num_squares_in_monopoly = AI_Get_Monopoly_Lots(game_state, monopoly_squares, monopolies[index]);
            group = RULE_SquarePredefinedInfo[monopolies[index]].group;

            num_houses = AI_Num_Houses_On_Monopoly(game_state, monopolies[index]);
            if(num_houses >= num_squares_in_monopoly * top_houses_on_lot )
                continue; /* built up to maximum (depending on housing shortage.) Won't build here. */

            /* Now figure out if we can build all the houses on it.  Subtract unmortgage costs. */
            unmortgage_cost = AI_Cost_Unmortgage_Monopoly(game_state, monopoly_squares[0]);

            if(unmortgage_cost > 0 && (options & AI_HOUSEOPS_AUCTION))
                continue; /* Housing shortage - can't buy on mortgaged monopolies. */

            if(RULE_SquarePredefinedInfo[monopolies[index]].housePurchaseCost > (excess_cash - unmortgage_cost))
                continue;  /* Can't afford a house on this lot. */

            if((num_houses >= (game_state->GameOptions.housesPerHotel - 1) * num_squares_in_monopoly)
                && ((free_hotels == 0) || (options & AI_HOUSEOPS_HOUSES_ONLY)))
                continue;  /* Out of hotels - can't buy here. */

            if((num_houses < (game_state->GameOptions.housesPerHotel - 1) * num_squares_in_monopoly)
                && ((free_houses == 0) || (options & AI_HOUSEOPS_HOTELS_ONLY)))
                continue;  /* We are out of houses! */

            if(best_rent < AI_Monopoly_Average_Landing_Frequency[group])
                {
                best_rent = (float) AI_Monopoly_Average_Landing_Frequency[group];
                best_square = monopolies[index];
                }
            }
        }

    if(best_square == 0)
        return FALSE;   /* Couldn't find a monopoly we can afford that is not built up! */

    /* Get the monopoly. */
    num_squares_in_monopoly = AI_Get_Monopoly_Lots(game_state, monopoly_squares, best_square);
    group = RULE_SquarePredefinedInfo[best_square].group;

    /* If mortgaged, unmortgage a property.  Otherwise, buy a house. */
    unmortgage_cost = AI_Cost_Unmortgage_Monopoly(game_state, best_square);

    if(unmortgage_cost > 0 && (options & AI_HOUSEOPS_AUCTION))
        return FALSE; /* Housing shortage - can't buy on mortgaged monopolies. */

    if(unmortgage_cost > excess_cash)
        return FALSE;   /* Can't unmortgage the monopoly first. */

    if(!(options & AI_HOUSEOPS_AUCTION))
        {
        if(game_state->Players[player].cash < unmortgage_cost)
            {   /* First get enough cash to buy the building. */
            AI_Mortgage_Worst_Property(game_state, player, FALSE, FALSE, FALSE);
            return TRUE;
            }
        if(AI_Unmortgage_Monopoly(game_state, player, best_square))
            return TRUE;
        }

    /* Next, shorten the list of properties to those we can build
       a house on to satisfy the even build rule. */
    if(game_state->GameOptions.evenBuildRule)
        {
        temp = AI_Num_Houses_On_Monopoly(game_state, best_square) / num_squares_in_monopoly + 1;
        if(temp < top_houses_on_lot)
            top_houses_on_lot = temp;
        }

    for(index = 0; index < num_squares_in_monopoly; ++index)
        {
        if(game_state->Squares[monopoly_squares[index]].houses >= top_houses_on_lot)
            {   /* Lot has as many houses as it should.  Can't build on it. Take it out of the list */
            for(index2 = index; index2 < num_squares_in_monopoly; ++index2)
                monopoly_squares[index2] = monopoly_squares[index2 + 1];

            num_squares_in_monopoly--;
            index--;
            }
        }

    /*  See if someone is 7 spaces away from a monopoly square.  If is, build on
        that square since 7 is the most probable roll. */
    if(!(options & AI_HOUSEOPS_AUCTION))
        {
        for(index = 0; index < num_squares_in_monopoly; ++index)
            {
            if(AI_Player_Close_To_Property(game_state, player, monopoly_squares[index], 7, 7))
                {
                if(game_state->Players[player].cash < RULE_SquarePredefinedInfo[monopoly_squares[index]].housePurchaseCost)
                    AI_Mortgage_Worst_Property(game_state, player, FALSE, FALSE, FALSE);
                else
                    {
                    AI_SendAction (ACTION_BUY_HOUSE, player, RULE_BANK_PLAYER,
                                    monopoly_squares[index], 0, 0, 0,NULL, 0, NULL);
                    AI_State[player].last_house_built_sold = monopoly_squares[index];
                    }
                return TRUE;
                }
            }
        }

    /*  Then no other preference really.  If the most expensive property
        can be built on, it will be the first property in the list.  Otherwise,
        there is no significant difference in building on the other two squares. */
    if((game_state->Players[player].cash < RULE_SquarePredefinedInfo[monopoly_squares[0]].housePurchaseCost)
        && !(options & AI_HOUSEOPS_AUCTION))
        AI_Mortgage_Worst_Property(game_state, player, FALSE, FALSE, FALSE);
    else
        {
        if(options & AI_HOUSEOPS_AUCTION)
            {   /* This is a housing shortage.  No need to ask for control, and
               no need to wait.  Just send requests immediately. */
            if(options & AI_HOUSEOPS_PLACEMENT)
                MESS_SendAction (ACTION_BUY_HOUSE, player, RULE_BANK_PLAYER,
                                 monopoly_squares[0], 0, 0, 0,NULL, 0, NULL);
            else
                MESS_SendAction (ACTION_START_HOUSING_AUCTION, player, RULE_BANK_PLAYER,
                                 0, 0, 0, 0,NULL, 0, NULL);
            AI_State[player].num_actions++;
            }
        else
            AI_SendAction (ACTION_BUY_HOUSE, player, RULE_BANK_PLAYER,
                            monopoly_squares[0], 0, 0, 0,NULL, 0, NULL);

        AI_State[player].last_house_built_sold = monopoly_squares[0];
        }

    return TRUE;
}

/**********************************************************************/
/*                  AI_Hypothetical_Buy_Houses                        */
/*                                                                    */
/*   Input: RULE_GameStatePointer  game_state  copy of game state.    */
/*          RULE_PlayerNumber      player      Who should buy?        */
/*   Output: BOOL - TRUE if an action message was sent to buy a house,*/
/*                  FALSE otherwise.                                  */
/*                                                                    */
/*      This routine will 'hypothetically' buy the best house for     */
/*  player.  'hypothetically' meaning it won't send the actual        */
/*  action, and will modify game_state to reflect the fact. Uses a    */
/*  similar algorithm as AI_Buy_Houses, except does not obey an AI    */
/*  strategy, buy simply tries to optimize rent received.  Does not   */
/*  mortgage property to buy houses, but will decrement player's cash */
/*  even if not enough cash is on hand.                               */
/**********************************************************************/


BOOL AI_Hypothetical_Buy_Houses(RULE_GameStatePointer  game_state,  RULE_PlayerNumber player)
{
    int                 num_monopolies, num_squares_in_monopoly;
    RULE_SquareType     monopolies[SG_PARK_PLACE + 1];
    float               best_rent = (float) 0.0;
    RULE_SquareType     best_square = 0;
    RULE_SquareType     monopoly_squares[AI_MAX_MONOPOLY_LOTS + 1];
    int                 index, index2, top_houses_on_lot, buy_x_houses, temp;
    long                excess_cash;
    RULE_SquareGroups   group;

    /* We will ignore a housing shortage. */
    top_houses_on_lot = game_state->GameOptions.housesPerHotel;

    /* How much do we have to spend? */
    excess_cash = AI_Get_Liquid_Assets(game_state, player, FALSE, FALSE);

    if(excess_cash == 0)
        return FALSE;   /* Can't buy houses with dreams.  Need cash. */

    /*  find out what monopolies player has.  */
    num_monopolies = AI_Get_Monopolies(game_state, player, monopolies, TRUE);

    /*  First, go through all the monopolies with less than 3 houses per lot that we can build up to
        3 houses per lot.  Pick the most expensive.  */
    for(index = 0; index < num_monopolies; ++index)
        {
        /* Get the monopoly. */
        num_squares_in_monopoly = AI_Get_Monopoly_Lots(game_state, monopoly_squares, monopolies[index]);

        buy_x_houses = AI_Num_Houses_On_Monopoly(game_state, monopolies[index]);
        buy_x_houses = num_squares_in_monopoly * 3 - buy_x_houses;
        if(buy_x_houses < 1)
            continue;   /* built-up.  Consider it later. */

        if(buy_x_houses * RULE_SquarePredefinedInfo[monopolies[index]].housePurchaseCost > excess_cash)
            continue;  /* Can't afford to buy so many houses. */

        /*  pick the most expensive monopoly that we can buildup 3 houses on each lot. */
        if(best_rent < RULE_SquarePredefinedInfo[monopolies[index]].rent[0])
            {
            best_rent = (float) RULE_SquarePredefinedInfo[monopolies[index]].rent[0];
            best_square = monopolies[index];
            }
        }

    if(best_square == 0)
        {   /* We didn't find anything with less than 3 houses we can develop.
           We should develop the monopoly with less than 3 houses
           per lot that is landed on the most. */
        for(index = 0; index < num_monopolies; ++index)
            {
            /* Get the monopoly. */
            num_squares_in_monopoly = AI_Get_Monopoly_Lots(game_state, monopoly_squares, monopolies[index]);
            group = RULE_SquarePredefinedInfo[monopolies[index]].group;

            if(AI_Num_Houses_On_Monopoly(game_state, monopolies[index]) >= num_squares_in_monopoly * 3)
                continue; /* built up to 3 or more houses per lot. consider it later. */

            if(RULE_SquarePredefinedInfo[monopolies[index]].housePurchaseCost > excess_cash)
                continue;  /* Can't afford a house on this lot. */

            if(best_rent <  AI_Monopoly_Average_Landing_Frequency[group])
                {
                best_rent = (float) AI_Monopoly_Average_Landing_Frequency[group];
                best_square = monopolies[index];
                }
            }
        }

    if(best_square == 0)
        {   /* We didn't find any monopoly with less than 3 houses, or just couldn't afford
           those that did.  Hence now simply build those that aren't completely built up yet
           in order of landing frequency. */
        for(index = 0; index < num_monopolies; ++index)
            {
            /* Get the monopoly. */
            num_squares_in_monopoly = AI_Get_Monopoly_Lots(game_state, monopoly_squares, monopolies[index]);
            group = RULE_SquarePredefinedInfo[monopolies[index]].group;

            if(AI_Num_Houses_On_Monopoly(game_state, monopolies[index]) >= num_squares_in_monopoly * top_houses_on_lot )
                continue; /* built up to maximum (depending on housing shortage.) Won't build here. */

            if(RULE_SquarePredefinedInfo[monopolies[index]].housePurchaseCost > excess_cash)
                continue;  /* Can't afford a house on this lot. */

            if(best_rent < AI_Monopoly_Average_Landing_Frequency[group])
                {
                best_rent = (float) AI_Monopoly_Average_Landing_Frequency[group];
                best_square = monopolies[index];
                }
            }
        }

    if(best_square == 0)
        return FALSE;   /* Couldn't find a monopoly we can afford that is not built up! */

    /* Get the monopoly. */
    num_squares_in_monopoly = AI_Get_Monopoly_Lots(game_state, monopoly_squares, best_square);
    group = RULE_SquarePredefinedInfo[best_square].group;

    /* Next, shorten the list of properties to those we can build
       a house on to satisfy the even build rule. */
    if(game_state->GameOptions.evenBuildRule)
        {
        temp = AI_Num_Houses_On_Monopoly(game_state, best_square) / num_squares_in_monopoly + 1;
        if(temp < top_houses_on_lot)
            top_houses_on_lot = temp;
        }

    for(index = 0; index < num_squares_in_monopoly; ++index)
        {
        if(game_state->Squares[monopoly_squares[index]].houses >= top_houses_on_lot)
            {   /* Lot has as many houses as it should.  Can't build on it. Take it out of the list */
            for(index2 = index; index2 < num_squares_in_monopoly; ++index2)
                monopoly_squares[index2] = monopoly_squares[index2 + 1];

            num_squares_in_monopoly--;
            index--;
            }
        }

    /* Hypothetically buy it. */
    game_state->Squares[monopoly_squares[0]].houses++;

    /* Subtract from cash.  Not important to mortgage, since we are only interested in a general
       picture, and hence only liquid assets available. The rents from the not mortgaged properties
       shouldn't make too much a difference, especially since we are talking about monopolies. */
    game_state->Players[player].cash -= RULE_SquarePredefinedInfo[monopolies[0]].housePurchaseCost;
    return TRUE;
}

/********************************************************************/
/*                  AI_Pay_Debt                                     */
/*                                                                  */
/*  Input:  RULE_PlayerNumber   player      Who is paying?          */
/*          RULE_PlayerNumber   target      To whom are we paying?  */
/*          long                owed        How much we need to pay?*/
/*          long                to_raise    How much left to raise? */
/*                                                                  */
/*      Tries to pay the debt owed by player to target.  Will       */
/*  try to create enough cash in player's hand to pay the debt, so  */
/*  when the routine finished, the debt will be payed.  Tries first */
/*  to mortgage non-monopoly properties, then proposes trades if    */
/*  that did not succeed, and finally tries to mortgage monopolies  */
/*  and sell houses.  If nothing works, will declare bankruptcy.    */
/********************************************************************/

void AI_Pay_Debt(RULE_PlayerNumber player, RULE_PlayerNumber target, long owed, long to_raise)
{
    long    liquid_assets = AI_Get_Liquid_Assets(&UICurrentGameState, player, FALSE, FALSE);

    #if _DEBUG
        /* Do we have enough cash to cover it?  Ie. is this a bogus call? */
        if(UICurrentGameState.Players[player].cash >= owed)
            {
            LE_ERROR_ErrorMsg ("AI_Pay_Debt called with enough cash to pay debt.");
            return;
            }
    #endif

    /*  Set AI paying_debt flag. */
    AI_State[player].paying_debt = TRUE;
    AI_State[player].money_owed = owed;

    /*  First, try to sell an unimportant property. */
    if(!AI_Mortgage_Worst_Property(&UICurrentGameState, player, FALSE, TRUE, FALSE))
        {   /* Did we sell anything?  If not, maybe we can trade something. */
        if(AI_Should_Trade(&UICurrentGameState, player, AI_TRADE_SOMEWHAT_IMPORTANT | AI_TRADE_FOR_CASH) )
            {   /* Can we propose a trade? */
            if(AI_Propose_Trade(&UICurrentGameState, player, AI_TRADE_SOMEWHAT_IMPORTANT | AI_TRADE_FOR_CASH, target))
                { /* Proposed a trade. Done something for now. */
                return;
                }
            }
        /* Didn't sell anything nor trade anything!  time to sell buildings. */
        if(!AI_Mortgage_Worst_Property(&UICurrentGameState, player, TRUE, TRUE, FALSE))
            {   /* Dind't sell ANYTHING! It's almost over. try a last-ditch trade. */
            if(AI_Propose_Trade(&UICurrentGameState, player, AI_TRADE_DESPERATE | AI_TRADE_FOR_CASH, target))
                { /* Proposed a trade. Done something for now. */
                return;
                }
            /* Didn't propose - time for bankruptcy. */
            AI_SendAction (ACTION_GO_BANKRUPT, player, RULE_BANK_PLAYER,
                             0, 0, 0, 0,NULL, 0, NULL);
            return;
            }
        }

    /* Make sure we get called again if we still don't have enough. */
    AI_State[player].message_to_send.action = ACTION_RESTART_PHASE;
    AI_State[player].message_to_send.fromPlayer = player;
    AI_State[player].message_to_send.toPlayer = RULE_BANK_PLAYER;

    return;
}

/*****************************************************************************
 * void AI_Process_AIs                                                       *
 *                                                                           *
 * DESCRIPTION:                                                              *
 *      Allows AIs to propose trades, buy houses, and do all the actions     *
 * human players do when it is not their turn.                               *
 *****************************************************************************/

void AI_Process_AIs(void)
{
    RULE_PlayerNumber   player;
    BOOL                return_code;
    AI_Action_Message_Record   *point;
    BOOL                auction_bid = FALSE;
    DWORD               time_change, cur_time;

    /* Is there an auction going on? */
    if(AI_Bid_In_Auction(&UICurrentGameState))
        auction_bid = TRUE;
    else
        {
        if(RunFastWithNoDisplay && AI_Glob.AUCTION_IS_ON)
            {
            /* No new high bid - wants us to kill the auction. */
            MESS_SendAction(ACTION_KILL_AUCTION_CHEAT, RULE_NOBODY_PLAYER, RULE_BANK_PLAYER,
                             0, 0, 0, 0,  NULL, 0, NULL);
            }
        }

    /* Make sure the game doesn't play for too long if playing in fast mode. */
    if(RunFastWithNoDisplay && AI_Drone_Game)
        {
        cur_time = GetTickCount();
        time_change = cur_time - AI_TEST_GAME_STARTING_TIME;

        if(time_change >= ((unsigned long) AI_Drone_Configs.max_game_time))
            AI_Player_Won(RULE_NOBODY_PLAYER);
        }

    for(player = 0; player < UICurrentGameState.NumberOfPlayers; ++player)
        {
        if(!SlotIsALocalAIPlayer[player])
            continue;  /* Not an AI - don't do a thing. */

        /* Has the AI sent an action, and is waiting for the right amount
           of pause? */
        if(AI_State[player].sending_message.wait_time_left)
            {
            /* Count down - is anything left? */
            if(--AI_State[player].sending_message.wait_time_left == 0)
                {
                /* Nope - time is up - send the action then. */
                point = &AI_State[player].sending_message;

                return_code = MESS_SendAction(point->Action, point->FromPlayer,
                                              point->ToPlayer, point->NumberA, point->NumberB,
                                              point->NumberC, point->NumberD, point->StringA,
                                              point->BinarySize, point->Blob);
                if(!return_code)
                    {   /* we didn't send the action - que is full. try next turn. */
                    AI_State[player].sending_message.wait_time_left++;
                    }
                }
            }
        else
            if(AI_State[player].contrl_msg_waiting)
                {
                /* Count down - is anything left? */
                point = &AI_State[player].control_message;
                if(point->wait_time_left)
                    point->wait_time_left--;

                if(point->wait_time_left == 0)
                    {//BSSM UPGRADE - here we need to remember for each AI are doing a BSSM
                    if(AI_Glob.Buy_Sell_Mort_Player == player)
                        {
                        /* Nope - time is up - send the action then. */
                        return_code = MESS_SendAction(point->Action, point->FromPlayer,
                                                      point->ToPlayer, point->NumberA, point->NumberB,
                                                      point->NumberC, point->NumberD, point->StringA,
                                                      point->BinarySize, point->Blob);
                        if(!return_code)
                            {   /* we didn't send the action - que is full. try next turn. */
                            AI_State[player].control_message.wait_time_left++;
                            }
                        else
                            AI_State[player].contrl_msg_waiting = FALSE;
                        }
                    else
                        if(AI_Glob.Buy_Sell_Mort_Player == RULE_NOBODY_PLAYER)
                            {   /* Need to ask for control! */
                            if(AI_State[player].num_actions == 1)
                                AI_SendAction (ACTION_PLAYER_BUYSELLMORT, player, RULE_BANK_PLAYER,
                                                 0, 0, 0, 0,NULL, 0, NULL);
                            }
                    }
                }
            else
                {
                /* Spend cash. Make sure we are not trying to pay a debt, though. */
                if(!auction_bid && !AI_Is_Paying_Fee(&UICurrentGameState, player))
                    AI_Spend_Excess_Assets(&UICurrentGameState, player);
                }

            /* See if the current player being asked to roll dice is taking his
               sweet little time.  Make the bankrupt AIs stay sillent. */
            if(AI_Glob.Last_Player_Asked_To_Roll != RULE_NOBODY_PLAYER &&
                UICurrentGameState.Players[player].currentSquare != SQ_OFF_BOARD)
                {
                /* Send an impatient message if we need to. */
                cur_time = GetTickCount();
                time_change = cur_time - AI_Glob.Time_Last_Player_Asked_To_Roll;
                if(time_change > AI_Data[player].patience)
                    {   /* Player is taking too long to roll the dice.  Send a message expressing
                        our impatience.  But don't swamp the player with constant messages - only
                        one every once in a while. */
                    time_change = cur_time - AI_State[player].last_sent_text_message;
                    if(time_change > AI_Data[player].patience)
                        {
                        if(rand() < (RAND_MAX / 30))    /* One in 30 chance of sending the message. */
                            {
                            AI_Send_Message(AI_MESS_Impatient, player, AI_Glob.Last_Player_Asked_To_Roll);
                            AI_State[player].last_sent_text_message = cur_time;
                            }
                        }
                    }
                }
            }

    return;
}

/********************************************************************/
/*                  AI_Spend_Excess_Assets                          */
/*                                                                  */
/*   Input: RULE_GameStatePointer  game_state                       */
/*          RULE_PlayerNumber      player      Who is spending cash?*/
/*                                                                  */
/*      Makes player do something.  First will try to trade, then   */
/*   build houses, and finally unmortgage property.                 */
/********************************************************************/

BOOL    AI_Spend_Excess_Assets(RULE_GameStatePointer game_state, RULE_PlayerNumber player)
{
    DWORD   ElapsedTime;
    EBOOL   return_value;

    if(AI_State[player].num_actions || game_state->TradeInProgress ||
        AI_PLAYER_SENDING_TRADE || AI_State[player].paying_debt)
        return FALSE;   /* Player in the middle of doing something. */

    if(game_state->Players[player].currentSquare == SQ_OFF_BOARD)
        return FALSE;   // Bankrupt AI - nothing to do.

    /*  Are we in an auction situation?  Is someone placing a building? Then don't buy now! */
    if(AI_Glob.AUCTION_IS_ON || AI_Glob.place_building_phase)
        return FALSE;

    /* Does someone else have control right now?  Then don't try to do anything
       that requires control. */
    if((AI_Glob.Buy_Sell_Mort_Player != player) &&
       (AI_Glob.Buy_Sell_Mort_Player != RULE_NOBODY_PLAYER))
        return FALSE;

    /* Is this a housing shortage question phase? If yes, can't do anything. */
    if(AI_Glob.housing_shortage)
        return FALSE;

    /* Should we wait a little while, a grace period of a few seconds,
       to let those slow Humans get their oar in. */

    if (game_state->GameOptions.AITakesTimeToThink)
        {
        ElapsedTime = GetTickCount () - AI_Glob.GracePeriodForHumanActivityTickCount;
        if (ElapsedTime < 10000 /* Number of milliseconds to wait */)
            return FALSE;
        }

    /* Try to make a trade first. */
    if(AI_Should_Trade(game_state, player, AI_TRADE_NOT_IMPORTANT))
        {
        if(AI_Propose_Trade(game_state, player, AI_TRADE_NOT_IMPORTANT, RULE_NOBODY_PLAYER))
            return TRUE;
        }

    /* Try to buy houses. */
    if(return_value = AI_Should_Buy_House(game_state, player))
        {
        if(return_value == EB_LATER)
            return FALSE;

        if(AI_Buy_Houses(game_state, player, AI_HOUSEOPS_NORMAL))
            return TRUE;
        }

    /* See if we should unmortgage property. */
    if(AI_Should_Unmortgage_Property(game_state, player))
        {
        if(AI_Unmortgage_Property(game_state, player, FALSE, FALSE) != 0)
            return TRUE;
        }

    return FALSE;
}

/********************************************************************/
/*                  AI_Should_Pause                                 */
/*                                                                  */
/*   Input:  RULE_GameStatePointer   game_state                     */
/*           RULE_ActionType  Action    What action is being sent?  */
/*           RULE_PlayerNumber FromPlayer   Who is sending it?      */
/*   Output: BOOL - TRUE if computer should pause before sending    */
/*                  the action to simulate human thinking.  FALSE   */
/*                  otherwise.                                      */
/*                                                                  */
/*      Determines whether a pause should be inserted before an AI  */
/* sends an action.  This is used to simulate human thinking and to */
/* allow human players, if any exist, to see what is going on.      */
/********************************************************************/

BOOL AI_Should_Pause(RULE_GameStatePointer game_state, RULE_ActionType Action, RULE_PlayerNumber FromPlayer)
{
    if(FromPlayer == RULE_NOBODY_PLAYER)
        return FALSE;

    if(!UICurrentGameState.GameOptions.AITakesTimeToThink)
        return FALSE;

    if(AI_Data[FromPlayer].min_action_wait == 0)
        return FALSE;   /* AI set not to wait. */

    if(Action == ACTION_PLAYER_BUYSELLMORT ||
       Action == ACTION_PLAYER_DONE_BUYSELLMORT)
        return FALSE;

    if(game_state->CurrentPlayer == FromPlayer ||
        AI_Glob.Buy_Sell_Mort_Player == FromPlayer)
        return TRUE;

    if(game_state->TradeInProgress)
        return TRUE;

    return FALSE;
}

/********************************************************************/
/*                  AI_SendAction                                   */
/*                                                                  */
/*   Input: same as MESS_SendAction                                 */
/*   Output: same as MESS_SendAction if 'FromPlayer' does not have  */
/*           an action pending, otherwise FALSE.                    */
/*                                                                  */
/*      This is a mirror of MESS_SendAction - calls MESS_SendAction */
/*  with the same paramters.  Only exception is that it first checks*/
/*  AI_Player_Doing_Actions to see if 'FromPlayer' is not in the    */
/*  middle of an action.  If 'player' is, then returns FALSE (allows*/
/*  only one action per player at a time.  If MESS_SendAction       */
/*  succeeds, increases 'FromPlayer's message counter.              */
/********************************************************************/

BOOL AI_SendAction (RULE_ActionType Action, RULE_PlayerNumber FromPlayer, RULE_PlayerNumber ToPlayer,
                    long NumberA, long NumberB, long NumberC, long NumberD, wchar_t *StringA,
                    unsigned int BinarySize, RULE_CharHandle Blob)
{
    BOOL    return_code = FALSE;
    AI_Action_Message_Record   *point;

    /*  We don't want to send a message if the AI is in the middle of doing
        something.  Our calculations must have been based on obsolete
        data, and so we should wait for the last action the AI sent so
        that things like cash and properties are updated, and our next
        moves will reflect that.  */

    if((FromPlayer != RULE_NOBODY_PLAYER) && AI_Should_Pause(&UICurrentGameState, Action, FromPlayer))
    {
        /* Is there a message waiting to be sent? If yes, send it first. */
        if(AI_State[FromPlayer].sending_message.wait_time_left != 0)
        {
            point = &AI_State[FromPlayer].sending_message;

            return_code = MESS_SendAction(point->Action, point->FromPlayer,
                                          point->ToPlayer, point->NumberA, point->NumberB,
                                          point->NumberC, point->NumberD, point->StringA,
                                          point->BinarySize, point->Blob);
            if(!return_code)
            {   /* Didn't send it - queue is full? */
                /* Is this a bid message by an AI?  Then allow an AI to send
                   a new bid. */
                if((point->Action == ACTION_BID) && (SlotIsALocalAIPlayer[point->FromPlayer]))
                    AI_Glob.PLAYER_CURRENTLY_BIDDING = FALSE;

                AI_State[FromPlayer].num_actions--;
                return FALSE;
            }

            point->wait_time_left = 0;
        }

        /* Store the message for sending - the AI needs to pause to simulate human
           thinking. */
        point = &AI_State[FromPlayer].sending_message;

        /* Will we have to wait for control before sending this message? */
        if((Action == ACTION_BUY_HOUSE && !AI_Glob.AUCTION_IS_ON) ||
            Action == ACTION_SELL_BUILDINGS ||
            (Action == ACTION_MORTGAGING && !AI_Glob.Player_Unmortgaging_For_Free))
        {
            point = &AI_State[FromPlayer].control_message;

            /* Is there a message waiting? Then remove an action count,
               since we won't be sending it. */
            if(AI_State[FromPlayer].contrl_msg_waiting)
                AI_State[FromPlayer].num_actions--;

            AI_State[FromPlayer].contrl_msg_waiting = TRUE;
        }

        point->Action = Action;
        point->FromPlayer = FromPlayer;
        point->ToPlayer = ToPlayer;
        point->NumberA = NumberA;
        point->NumberB = NumberB;
        point->NumberC = NumberC;
        point->NumberD = NumberD;
        point->StringA = StringA;
        point->BinarySize = BinarySize;
        point->Blob = Blob;

        /* Randomize the wait before this message is sent. */
        point->wait_time_left = AI_Get_Random_Action_Wait(Action, FromPlayer);

        /* Increase player's action counter. */
        AI_State[FromPlayer].num_actions++;

        return_code = TRUE;
    }

    /* Did we que the message? If not, we should send it. */
    if(!return_code)
    {
        /* Send the message, if can. */

        /* Will we have to wait for control before sending this message? */
        if(((Action == ACTION_BUY_HOUSE && !AI_Glob.AUCTION_IS_ON) ||
            Action == ACTION_SELL_BUILDINGS ||
            (Action == ACTION_MORTGAGING && !AI_Glob.Player_Unmortgaging_For_Free))
            && (FromPlayer != RULE_NOBODY_PLAYER))
        {
            if(AI_Glob.Buy_Sell_Mort_Player != FromPlayer)
            {
                /* Is there a message waiting? Then remove an action count,
                   since we won't be sending it. */
                if(AI_State[FromPlayer].contrl_msg_waiting)
                    AI_State[FromPlayer].num_actions--;

                AI_State[FromPlayer].contrl_msg_waiting = TRUE;

                point = &AI_State[FromPlayer].control_message;
                point->Action = Action;
                point->FromPlayer = FromPlayer;
                point->ToPlayer = ToPlayer;
                point->NumberA = NumberA;
                point->NumberB = NumberB;
                point->NumberC = NumberC;
                point->NumberD = NumberD;
                point->StringA = StringA;
                point->BinarySize = BinarySize;
                point->Blob = Blob;
                point->wait_time_left = 0;

                AI_State[FromPlayer].num_actions++;

                    /* Do we need to ask for control? */
                if(AI_State[FromPlayer].num_actions == 1)
                {   /* Yes, then ask for it. */
                    AI_SendAction (ACTION_PLAYER_BUYSELLMORT, FromPlayer, RULE_BANK_PLAYER,
                                     0, 0, 0, 0,NULL, 0, NULL);
                }
                return TRUE;
            }
        }

        return_code = MESS_SendAction(Action, FromPlayer, ToPlayer, NumberA, NumberB,
                                      NumberC, NumberD, StringA, BinarySize, Blob);
        if(return_code)
        {
                /* Then we sent an action.  Increase player's action counter. */
            if(FromPlayer != RULE_NOBODY_PLAYER)
            {
                AI_State[FromPlayer].num_actions++;
                AI_State[FromPlayer].sending_message.Action = Action;
            }
        }
    }

return (return_code);
}

/********************************************************************/
/*                  AI_Get_Random_Action_Wait                       */
/*                                                                  */
/*   Input:  RULE_ActionType Action   What are we trying to do?     */
/*           RULE_PlayerNumber        Who is doing this action?     */
/*   Output: int - a number indicating how long the AI should wait  */
/*           before sending the Action message.  This function      */
/*           assumes a wait will occurr.  Only randomizes how long. */
/********************************************************************/

int AI_Get_Random_Action_Wait(RULE_ActionType Action, RULE_PlayerNumber player)
{
    float   random_number;
    float   wait_variance;
    int     wait_time;

    /* Check what kind of message this is.  Different wait times depending on what
       we are doing. */
    switch(Action)
    {
//      case ACTION_TRADE_EDITING_DONE:
        case ACTION_TRADE_ACCEPT:
            return (AI_Data[player].wait_for_trade_consider);
    }

    random_number = rand() / ((float) RAND_MAX);

    wait_variance = (float) (AI_Data[player].max_action_wait - AI_Data[player].min_action_wait + 1);
    wait_time = (int) (AI_Data[player].min_action_wait + floor(wait_variance * random_number));

return wait_time;
}

/********************************************************************/
/*                  AI_Excess_Cash_Available                        */
/*                                                                  */
/*   Input: RULE_GameStatePointer  game_state                       */
/*          RULE_PlayerNumber      player       Whose cash?         */
/*          BOOL    cash_only       Only take cash into account?    */
/*   Output: long      Amount of excess cash, according to AI       */
/*              specifications/strategy, that is available for      */
/*              spending and should be spent.                       */
/*                                                                  */
/*      Finds out how much money the 'player' AI has available for  */
/*   purchasing houses or unmortgaging property.  If trying to      */
/*   unmortgage property, should use TRUE for cash_only.  However,  */
/*   if wanting to buy houses, should use FALSE for cash_only.      */
/********************************************************************/

long    AI_Excess_Cash_Available(RULE_GameStatePointer game_state, RULE_PlayerNumber player, BOOL cash_only)
{
    long  excess_cash = 0, payments = 0;
    long  liquid_assets = AI_Get_Liquid_Assets(game_state, player, FALSE, FALSE);
    RULE_SquareType     square;


    if(cash_only)
        liquid_assets = game_state->Players[player].cash;

    switch(AI_Data[player].cash_strategy)
    {
        /*  Always try to keep minimum amount, no matter what
            is going on.  */
        case AI_CASH_MINIMUM_AMOUNT:
            excess_cash = liquid_assets - AI_Data[player].min_cash_on_hand;
            break;

        /*  Always keep enough for expected rent, taking into account
            rent from housed monopolies.  */
        case AI_CASH_EXPECTED_RENT_INC_MONOPOLIES:
            payments = AI_Average_Rent_Payed(game_state, player, 0, TRUE, 1.0f);
            if(payments < AI_Data[player].min_cash_on_hand)
                payments = AI_Data[player].min_cash_on_hand;

            excess_cash = liquid_assets - payments;
            break;

        /*  Always keep enough for expected rent, but forget about
            rent from housed monopolies. */
        case AI_CASH_EXPECTED_RENT_NOT_INC_MONOPOLIES:
            payments = AI_Average_Rent_Payed(game_state, player, 0, FALSE, 1.0f);
            if(payments < AI_Data[player].min_cash_on_hand)
                payments = AI_Data[player].min_cash_on_hand;

            excess_cash = liquid_assets - payments;
            break;

        /*  Will keep enough cash for expenses without monopolies plus
            the highest rent on a housed monopoly. */
        case AI_CASH_HIGHEST_RENT:

                /* First, find out the highest rent from a housed monopoly. */
            square = AI_Highest_Rent_Square(game_state, player, &payments);

            if(square == 0)
                payments = 0;
            else
            {
                if(AI_Num_Houses_On_Monopoly(game_state, square) == 0)
                    payments = 0;
            }

            /*  Add to that average expenses without monopolies. */
            payments += AI_Average_Rent_Payed(game_state, player, 0, FALSE, 1.0f);

            if(payments < AI_Data[player].min_cash_on_hand)
                payments = AI_Data[player].min_cash_on_hand;

            excess_cash = liquid_assets - payments;
            break;

        /*  Use a variant of the above three.  Will keep enough for average
            rent, but will try to keep enough for expenses plus highest
            monopoly rent if:
                - The total sum of the rents from fully housed monopolies
                  plus rents from all other owned properties is more than
                  any other player (ie. has the most expensive monopolies)
                              AND
                - current average rent intake is greater than any other
                  players'.
        The basic strategy here is to play safe if owning the most expensive
        properties, and at least keeping in par with rents of opponents,
        otherwise go all out and buy like crazy.  */
        case AI_CASH_MONOPOLY_DEPENDENT:
            /* Are we in a monopoly situation? */
            if(AI_Exist_Monopoly(game_state))
            {
                /* Then find out if we are with the best monopoly.  If yes,
                   play it safe.  If no - all or nothing - build it up. */
                if( AI_Most_Expensive_Potential_Income(game_state) == player)
                {   /*  We've got the best.  Now see if we have the best actual rent */

                    if( AI_Best_Current_Rent(game_state) == player )
                    {  /* We're doing okay.  Make sure we have enough to step on
                          the most expensive lot.  */

                        /* First, find out the highest rent from a housed monopoly. */
                        square = AI_Highest_Rent_Square(game_state, player, &payments);

                        if(square == 0)
                            payments = 0;
                        else
                        {
                            if(AI_Num_Houses_On_Monopoly(game_state, square) == 0)
                                payments = 0;
                        }
                    }
                }
            }

            /*  Add to that average expenses without monopolies. */
            payments += AI_Average_Rent_Payed(game_state, player, 0, FALSE, 1.0f);

            if(payments < AI_Data[player].min_cash_on_hand)
                payments = AI_Data[player].min_cash_on_hand;

            excess_cash = liquid_assets - payments;
            break;

        default:    /* DEFAULT?  Shouldn't happen. */
            break;
    }


    if(excess_cash < 0)
        excess_cash = 0;

return (excess_cash);
}


/************************************************************************************/
/*                  AI_Get_Total_Worth_With_Factors                                 */
/*                                                                                  */
/*   Input: RULE_GameStatePointer  game_state                                       */
/*          RULE_PlayerNumber   player      Check who owns it                       */
/*          RULE_PlayerNumber   ai_strat   Whose AI_Data.property_factors to use,   */
/*                                                                                  */
/*   Output: long    total worth of player, where property values are multiplied by */
/*              by AI_Data.property_factors.                                        */
/************************************************************************************/

long AI_Get_Total_Worth_With_Factors(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_PlayerNumber ai_strat)
{
    long total = 0;
    RULE_PropertySet    properties_owned = RULE_PropertySetOwnedByPlayer(game_state,player);
    RULE_SquareType     SquareNo;
    RULE_SquareGroups   group;
    int                 num_railroads, num_utilities;

    /* First get the number of railroads and utilities owned, as this will make a difference
       in the factors we use. */
    num_railroads = AI_Number_Railroads_Utils_Owned(game_state, player, SG_RAILROAD,
                                                    FALSE, properties_owned) - 1;
    num_utilities = AI_Number_Railroads_Utils_Owned(game_state, player, SG_UTILITY,
                                                    FALSE, properties_owned) - 1;

    for(SquareNo = 0; SquareNo < SQ_IN_JAIL;SquareNo++)
    {
        if(RULE_PropertyToBitSet(SquareNo) & properties_owned)  /* Do we own this property? */
        {
            total += RULE_SquarePredefinedInfo[SquareNo].housePurchaseCost * game_state->Squares[SquareNo].houses;
            group = RULE_SquarePredefinedInfo[SquareNo].group;
            if(group == SG_RAILROAD)
                total += (long) (RULE_SquarePredefinedInfo[SquareNo].purchaseCost * AI_Data[ai_strat].cash_cow_factors[num_railroads]);
            else if(group == SG_UTILITY)
                total += (long) (RULE_SquarePredefinedInfo[SquareNo].purchaseCost * AI_Data[ai_strat].cash_cow_factors[num_utilities + AI_MAX_RAILROADS]);
            else
                total += (long) (RULE_SquarePredefinedInfo[SquareNo].purchaseCost * AI_Data[ai_strat].property_factors[group]);
        }
    }

    // Now include cash on hand.
    total += game_state->Players[player].cash;
return total;
}


/************************************************************************************/
/*                  AI_Mortgage_Worst_Property                                      */
/*                                                                                  */
/*   Input: RULE_GameStatePointer  game_state                                       */
/*          RULE_PlayerNumber   player      Mortgage whose property?                */
/*          BOOL sell_houses        Should we sell houses if we must?               */
/*          BOOL mortgage_monopoly  Should we mortgage a monopoly?                  */
/*          BOOL hypothetical       True if not to send out action messages, but    */
/*                                  do it quick and dirty.                          */
/*                                                                                  */
/*   Output:  BOOL  -  TRUE if a property was found to mortgage.  Otherwise FALSE.  */
/*                                                                                  */
/*      Finds the best property to mortgage and mortgages it.                       */
/*  - Lowests ROI properties first:  Mediterranean, Oriental, Vermont, Baltic,      */
/*    Connecticut, St. Charles, States, Virginia, St. James, Tennessee, New York,   */
/*    Kentucky, Indiana, Illinois, Atlantic, Ventnor, Marvin Gardens, Pacific,      */
/*    North Carolina, Pennsylvania, Park Place, Boardwalk, Short-Line RR, B&0 RR,   */
/*    Pennsylvania RR, Reading RR, Electric Co., Water Works.                       */
/*  - Exception with minor monopolies: Leave R&R just before monopoly in utilities, */
/*    last if no monopoly in utilities.  If 3+ R&R, leave R&R last.                 */
/*  - Will sell houses if needed as well.                                           */
/*  - If a monopoly exists, will sell houses/mortgage property that will give       */
/*    highest rent income.  If housing shortgage, will sell houses last, no matter  */
/*    what has to be given up. (ie. 4 railroad monopoly. )  since property can be   */
/*    unmortgaged, while an auction to buy houses will most probably cost more.     */
/************************************************************************************/

BOOL AI_Mortgage_Worst_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber player, BOOL sell_houses,
                               BOOL mortgage_monopoly, BOOL hypothetical)
{
    RULE_PropertySet    properties_owned = RULE_PropertySetOwnedByPlayer(game_state,player);
    RULE_PropertySet    monopoliless_props, housingless_props;
    RULE_SquareType     SquareNo, mortgage_square;
    RULE_SquareType     monopoly_squares[AI_MAX_MONOPOLY_LOTS + 1];
    int                 squares_in_monopoly;

        /* Need to create a new property list without houses or monopolies. */
    monopoliless_props = housingless_props = properties_owned;

    /*  First, unmortgage all the properties that are not monopolies.  */
    for(SquareNo = 0; SquareNo < SQ_IN_JAIL;SquareNo++)
    {
        mortgage_square = AI_Mortgage_Order[SquareNo];
        if(RULE_PropertyToBitSet(mortgage_square) & properties_owned)   /* Do we own this property? */
        {
            /* While we're here, update our new property lists */
            if(game_state->Squares[mortgage_square].houses >= 0)
                housingless_props = AI_XOR_Property_Set(housingless_props, mortgage_square,0);

                    /*  If we are allowed to sell monopolies, if part of a monopoly is already
                        mortgaged, treat it as if it wasn't a monopoly. */
            if(AI_Own_Monopoly(game_state,player,mortgage_square, mortgage_monopoly, properties_owned))
                monopoliless_props = AI_XOR_Property_Set(monopoliless_props, mortgage_square,0);

                /* Is it already mortgaged?  Skip it then. */
            if(game_state->Squares[mortgage_square].mortgaged)
                continue;

            /*  Is this a monopoly, or 2+ utils or railroads?  If yes, skip it for now.  */
            if( !( AI_Own_Monopoly(game_state,player,mortgage_square, mortgage_monopoly, properties_owned) ||
                AI_Number_Railroads_Utils_Owned(game_state,player,RULE_SquarePredefinedInfo[mortgage_square].group,
                                                game_state->GameOptions.mortgagedCountsInGroupRent,
                                                properties_owned ) >= 2) )
            {
                if(hypothetical)
                {
                    game_state->Players[player].cash += RULE_SquarePredefinedInfo[mortgage_square].mortgageCost;
                    game_state->Squares[mortgage_square].mortgaged = TRUE;
                }
                else
                {
                    /* Alrighty then, we have found our property to mortgage.  mortgage it then. */
                    AI_SendAction (ACTION_MORTGAGING, player, RULE_BANK_PLAYER,
                                     (long) mortgage_square, 0, 0, 0,
                                     NULL, 0, NULL);
                }

                return TRUE;   /* No need to stick around.  done what we came to do. */
            }
        }
    }

    if(!sell_houses && !mortgage_monopoly)
    {   /* Can't sell monopolies or houses.  Then all we are left with is utilities and railroads. */
        mortgage_square = AI_Find_Lowest_Rent(game_state,player, monopoliless_props);
    }
    else
    if(!sell_houses)
    {  /* Can sell monopolies, as long as they are not housed.  */
        mortgage_square = AI_Find_Lowest_Rent(game_state,player, housingless_props);
    }
    else
    {
        /*  What we mortgage next depends on the housing situation.  If housing is      */
        /*  low, lose houses as a LAST resort, no matter what we have to mortgage (ie.  */
        /*  4 railroad monopoly.)  Otherwise, sell houses/mortgage which will yield     */
        /*  highest rent income.                                                        */

        if(AI_Housing_Shortage(game_state,AI_CRITICAL_HOUSING_LEVEL))
        {
            mortgage_square = AI_Find_Lowest_Rent(game_state, player, housingless_props);
            if(mortgage_square == 0)   /* Couldn't find non-housed properties to mortgage?!! */
                mortgage_square = AI_Find_Lowest_Rent(game_state, player, properties_owned);
        }
        else
            mortgage_square = AI_Find_Lowest_Rent(game_state,player, properties_owned);
    }

    if(mortgage_square == 0)    /* Did we find a property to mortgage? */
        return FALSE;   /* NO?  Oh well.  Return a false then.  */

    /* Otherwise, mortgage this property, or sell houses.  */
    if(AI_Num_Houses_On_Monopoly(game_state, mortgage_square) > 0)
    {   /* We have houses to sell.  */
        if(hypothetical)
        {
            squares_in_monopoly = AI_Get_Monopoly_Lots(game_state, monopoly_squares, mortgage_square);
            AI_Test_Sell_Houses(game_state, monopoly_squares, squares_in_monopoly, mortgage_square);
        }
        else
            AI_Sell_Houses(game_state, player, mortgage_square, TRUE);
    }
    else
    {   /* Just plain ordinary mortgage. */
        if(hypothetical)
        {
            game_state->Players[player].cash += RULE_SquarePredefinedInfo[mortgage_square].mortgageCost;
            game_state->Squares[mortgage_square].mortgaged = TRUE;
        }
        else
        {
            AI_SendAction (ACTION_MORTGAGING, player, RULE_BANK_PLAYER,
                            (long) mortgage_square, 0, 0, 0,
                            NULL, 0, NULL);
        }
    }

return TRUE;
}


/************************************************************************************/
/*                  AI_Test_Sell_Houses                                             */
/*                                                                                  */
/*   Input: RULE_GameStatePointer  game_state                                       */
/*          RULE_SquareType   *monopoly_lots    A list of lots belonging to the     */
/*                                             monopoly we want to sell houses on.  */
/*          int  squares_in_monopoly        # of lots in monopoly.                  */
/*          RULE_SquareType  test_square    Which property in particular are we     */
/*                                          trying to sell from?  Will try to put   */
/*                                          the minimum # of houses on this lot.    */
/*                                                                                  */
/*      This function is used to consider hypothetical situations in which a house  */
/*   is sold from the 'test_square' property.  Takes into account housing shortages,*/
/*   this routine will modify game_state so that the monopoly lots in               */
/*   'monopoly_lots' lose a house.  This will not send any messages to the system,  */
/*   so game_state should be a copy and not the actual state of the game.  It is    */
/*   assumed that the most expensive property in the monopoly is first in the list. */
/*   This is used to make sure that the most expensive property will have the       */
/*   most houses, unless of course test_square is the most expensive property.      */
/************************************************************************************/

void AI_Test_Sell_Houses(RULE_GameStatePointer game_state, RULE_SquareType *monopoly_lots, int squares_in_monopoly, RULE_SquareType test_square)
{
    char free_houses = AI_Free_Houses(game_state), hotels_on_monopoly = 0, houses_on_monopoly = 0, houses_need_to_sell = 1;
    char hotels_and_houses_on_monopoly;
    char  min_houses_on_lot,  max_houses_on_lot, max_house_lots;
    int  index;

    /*  Count our available houses.  Trying to determine how many houses we are forced to sell in housing shortage */
    for(index = 0; index < squares_in_monopoly; ++index)
    {
        if(game_state->Squares[monopoly_lots[index]].houses == game_state->GameOptions.housesPerHotel)
            hotels_on_monopoly++;
        else
            houses_on_monopoly += game_state->Squares[monopoly_lots[index]].houses;
    }

    hotels_and_houses_on_monopoly = hotels_on_monopoly * game_state->GameOptions.housesPerHotel + houses_on_monopoly;

    if(hotels_on_monopoly > 0)  /* Do we have hotels on the monopoly? Will have to sell a hotel. */
    {
        if(free_houses < game_state->GameOptions.housesPerHotel - 1)    /* Are there enough houses in the bank to sell? */
        {   /* No. find out how many houses we need to sell. */
            houses_need_to_sell = hotels_on_monopoly * game_state->GameOptions.housesPerHotel - free_houses;
        }
    }

    /* Calculate the minimum and maximum # of houses we should have per lot. */
    max_houses_on_lot = (char) ceil( ((double) (hotels_and_houses_on_monopoly - houses_need_to_sell))/squares_in_monopoly);
    min_houses_on_lot = (char) floor( ((double) (hotels_and_houses_on_monopoly - houses_need_to_sell))/squares_in_monopoly);

    /*  How many max_house lots will be in this monopoly? */
    max_house_lots = (hotels_and_houses_on_monopoly - houses_need_to_sell) % squares_in_monopoly;

    /* Assign the remaining houses to the lots.  'test_square' will have the minimum number.  Maximum number
       of houses will be assigned to a first come - first serve basis.  Since we placed the most expensive lot
       first, we will have the maximum number of houses on the first lot, unless the first lot is our test_square.
       If it is, then we really don't care on which square will be the maximum, if there is only one.  */

    for(index = 0; index < squares_in_monopoly; ++index)
    {
        if(monopoly_lots[index] == test_square)
            game_state->Squares[test_square].houses = min_houses_on_lot;
        else
        {
            if(max_house_lots > 0)
            {
                max_house_lots--;
                game_state->Squares[monopoly_lots[index]].houses = max_houses_on_lot;
            }
            else
                game_state->Squares[monopoly_lots[index]].houses = min_houses_on_lot;
        }
    }

return;
}

/************************************************************************************/
/*                  AI_Sell_Houses                                                  */
/*                                                                                  */
/*   Input: RULE_GameStatePointer  game_state                                       */
/*          RULE_PlayerNumber   player      Lowest rent for who?                    */
/*          RULE_SquareType  sell_square    Which property in particular are we     */
/*                                          trying to sell from?  Will try to put   */
/*                                          the minimum # of houses on this lot.    */
/*          BOOL         remember_square    Should we set last_house_built_sold to  */
/*                                          sell_square? Used for decomposition.    */
/*                                                                                  */
/*      This function will send the system messages to sell a house on lot          */
/*   'sell_square'. If the monopoly has more houses than 'sell_square' on some      */
/*    other lot, will sell from that other lot.  Will always try to put the most    */
/*    houses on the most expensive property.  Ignores housing shortages.            */
/************************************************************************************/


void AI_Sell_Houses(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareType sell_square,
                    BOOL remember_square)
{
    RULE_SquareType monopoly_squares[AI_MAX_MONOPOLY_LOTS + 1], cur_square, sold_square = SQ_IN_JAIL;
    int squares_in_monopoly, index;
    RULE_GameStateRecord    test_state;
    int houses_to_sell, max_houses_on_sold_square;

    squares_in_monopoly = AI_Get_Monopoly_Lots(game_state, monopoly_squares, sell_square);

    /* Going to sell houses.  Might as well use AI_Test_Sell_Houses.  */
    memcpy(&test_state, game_state, sizeof( RULE_GameStateRecord) );
    AI_Test_Sell_Houses(&test_state, monopoly_squares, squares_in_monopoly, sell_square);

    /*  Now send a message telling which square we sold a house from. */
    for(index = 0; index < squares_in_monopoly; ++index)
    {
        cur_square = monopoly_squares[index];
        houses_to_sell = game_state->Squares[cur_square].houses - test_state.Squares[cur_square].houses;

        /* Pick the property that has the most houses out of the ones we would sell from. */
        if(houses_to_sell > 0)
        {
            if((sold_square == SQ_IN_JAIL) || (game_state->Squares[cur_square].houses > max_houses_on_sold_square))
            {
                max_houses_on_sold_square = game_state->Squares[cur_square].houses;
                sold_square = cur_square;
            }
        }
    }

    if(sold_square != SQ_IN_JAIL)
    {
        AI_SendAction (ACTION_SELL_BUILDINGS, player, RULE_BANK_PLAYER,
                        (long) sold_square, FALSE, 0, 0,  NULL, 0, NULL);
    }

    if(remember_square)
        AI_State[player].last_house_built_sold = sell_square;

return;
}

/******************************************************************/
/*                  AI_Buy_Or_Auction                             */
/*                                                                */
/*   Input:  RULE_PlayerNumber  player      Who is deciding?      */
/*           RULE_SquareType    square      Which lots are we     */
/*                                          buying?               */
/*           long purchase_cost             How much?             */
/*                                                                */
/*      Decides whether to buy or auction a property, and sends   */
/*   the appropriate action message.  Even if AI wants to buy the */
/*   property, will auction it if no player has enough liquid     */
/*   assets to purchase the property.                             */
/******************************************************************/

void AI_Buy_Or_Auction(RULE_PlayerNumber player, RULE_SquareType square, long purchase_cost)
{
    long most_cash = 0;

    if(AI_Should_Buy_Property(&UICurrentGameState, player, square))
    {
        /* We want to buy it, but need to pay the whole price. */
        AI_SendAction(ACTION_BUY_OR_AUCTION_DECISION, player, RULE_BANK_PLAYER,
                         TRUE, 0, 0, 0,  NULL, 0, NULL);
    }
    else
    {
        /* Don't want to buy it.  Put it on auction. */
        AI_SendAction(ACTION_BUY_OR_AUCTION_DECISION, player, RULE_BANK_PLAYER,
                         FALSE, 0, 0, 0,  NULL, 0, NULL);
    }

return;
}


/************************************************************************************/
/*                  AI_Should_Buy_Property                                          */
/*                                                                                  */
/*   Input: RULE_GameStatePointer  game_state                                       */
/*          RULE_PlayerNumber   player      Who is buying?                          */
/*          RULE_SquareType     property  Which property are we                     */
/*                                        buying?                                   */
/*   Output: BOOL  -  TRUE if we should buy it, FALSE, false otherwise.             */
/*                                                                                  */
/*      Determines whether 'player' should buy 'property'. Choice                   */
/*  determined by the following criteria:                                           */
/*                                                                                  */
/*  - If a Railroad or utility, do not hesitate to buy even if it means             */
/*    mortgaging property                                                           */
/*  - If baltic or boardwalk monopolies, buy.  This prevents or sets up for a       */
/*    monopoly.                                                                     */
/*  - If no more than one other player owns property in the monopoly, buy. This     */
/*    allows for a future deal for a monopoly.                                      */
/*  - If two different players own a property each, buy if these two                */
/*    opponents share only 1 monopoly between themselves, or if no monopoly is      */
/*    owned, don't have an opportunity to get one by dealing with 1 player,         */
/*    AND if the property would allow a "group trade" (group trade being a trade    */
/*    between 3+ people which allows each person to gain a monopoly).  Exception    */
/*    to this in New York property.  Getting Veto power of this monopoly is         */
/*    important enough.                                                             */
/*                                                                                  */
/*      If the computer will not have enough cash left over for expenses, it won't  */
/*   buy the property.                                                              */
/************************************************************************************/


BOOL AI_Should_Buy_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareType property)
{
    long    liquid_assets = AI_Get_Liquid_Assets(game_state, player, FALSE, FALSE);
    long    assets_with_houses = AI_Get_Liquid_Assets(game_state, player, TRUE, TRUE);
    long    assets_not_sell_bare_monopolies = AI_Get_Liquid_Assets(game_state, player, TRUE, FALSE);
    long    must_have_cash_left = 0, prop_cost, most_cash, most_cash_house;
    long    assets_after_buying_houses = AI_Cash_Available_After_Housing(game_state, player);
    RULE_SquareGroups   group = RULE_SquarePredefinedInfo[property].group;
    RULE_PlayerNumber   mono_player;
    BOOL                buy = FALSE;
    AI_Property_Importance  importance_to_others, importance_to_us;
    long    num_monopolies, num_houses;
    RULE_SquareType    store_squares[SG_PARK_PLACE + 1];
    long    counter;

    prop_cost = RULE_SquarePredefinedInfo[property].purchaseCost;

        /* How much liquid assets must we have left after buying? */
    must_have_cash_left = AI_Min_Calculated_Expenses(game_state,player);

    if(must_have_cash_left == -1)  /* Player in unknown state.  error.  Should never happen. */
        return(FALSE);

        /*  Is this less than the AI's minimum amount? */
    if(must_have_cash_left < AI_Data[player].min_cash_on_hand)
        must_have_cash_left = AI_Data[player].min_cash_on_hand;

        /* Subtract from this the mortgage value of the property. */
    must_have_cash_left -= RULE_SquarePredefinedInfo[property].mortgageCost;

    /* Check whether we have enough liquid assets to buy it, or whether we will have enough
       liquid assets left to pay for expenses. */
    if(assets_with_houses < prop_cost)
        return FALSE; /* don't have enough to buy it. */

    if((assets_with_houses - prop_cost) < must_have_cash_left)  /* Will we have enough cash left? */
        return FALSE;

    /* What is the most cash other people have to buy a property without selling
       houses or mortgaging a monopoly? */
    most_cash = AI_Get_Most_Liquid_Assets(game_state, player, FALSE);
    most_cash_house = AI_Get_Most_Liquid_Assets(game_state, player, TRUE);

    if(most_cash_house < prop_cost)
        return FALSE; /* No one has enough to buy it even if mortgage everything - hence
                         auction it. */

    /*  Figure out the importance of this property.  Better be REALLY important
        to be worth selling houses for. If not THAT important, use only
        other liquid assets, or don't buy it at all.  */
    importance_to_others = AI_Give_Players_Important_Monopoly(game_state, property, player, RULE_NOBODY_PLAYER);
    importance_to_us = AI_Give_Players_Important_Monopoly(game_state, property, RULE_NOBODY_PLAYER, player);

    if((liquid_assets - prop_cost) < must_have_cash_left)
    {   /* Have to sell houses to buy it.  This better BE IMPORTANT! */

        /*  Check whether this will give one of the players a DIRECT monopoly. */
        if(importance_to_others == AI_PROPERTY_GIVES_DIRECT_MONOPOLY)
        {   /* gives someone a direct monopoly.  However, if this someone has to
               sell houses or mortgage a monopoly to get it, auction it. */
            mono_player = AI_Who_Owns_Monopoly(game_state, property);
            liquid_assets = AI_Get_Liquid_Assets(game_state, mono_player, TRUE, FALSE);
            if(liquid_assets < prop_cost)
            {
                /* Player has to sell houses - however, if this a one of the cheapest
                   monopolies, then the cost won't set him back far, and will be able
                   to build very quickly on it. */
                if(RULE_SquarePredefinedInfo[property].group <=  SG_ORIENTAL_AVENUE)
                    return TRUE;

                return FALSE;   /* Player who owns this monopoly has to sell house and it is
                                   a more expensive monopoly. Hence won't
                                   have enough to build on this monopoly. */
            }

            return TRUE;
        }

        if(importance_to_us == AI_PROPERTY_GIVES_DIRECT_MONOPOLY)
        {   /* We get a monopoly.  Only case we would want to buy it is if we
               are almost full on all our other monopolies and buying this won't
               force us to sell too many houses, or if this is a very cheap
               monopoly. */

            if(RULE_SquarePredefinedInfo[property].group <=  SG_ORIENTAL_AVENUE)
                return TRUE;  /* Cheap cheap cheap, cheap cheap cheap - get it. */

            num_monopolies = AI_Get_Monopolies(game_state, player,  store_squares, FALSE);

            /* If we don't have 3 houses on each of our monopolies, then forget it. */
            for(counter = 0; counter < num_monopolies; ++counter)
            {
                num_houses = AI_Num_Houses_On_Monopoly(game_state, store_squares[counter]);
                num_houses -= AI_Number_Properties_In_Set(AI_Get_Monopoly_Set(store_squares[counter])) * 3;

                if(num_houses < 0)
                    return FALSE;
            }

            /* How many houses will we need to sell? */
            liquid_assets = prop_cost + must_have_cash_left - liquid_assets;

            if(liquid_assets > RULE_SquarePredefinedInfo[store_squares[num_monopolies - 1]].housePurchaseCost * 1.5)
                return FALSE;  /* would have to sell more than 3 houses. */

            return TRUE;
        }

        /* Check if we are in a situation where we can take chances - ie.  If we are the only
           ones who own a monopoly.  If so, we should have at least one property from each
           monopoly to prevent others from getting them, even if it means selling a house or
           two to buy them. */
        if(AI_Only_Player_Have_Monopoly(game_state, player))
        {
            if(!AI_Owns_Prop_From_Monopoly(game_state, player, property) &&
               !AI_Is_Cash_Cow(property))
                return TRUE;
        }

        /*  Not a monopoly forming property?  Then it's not important enough. */
        return FALSE;
    }
    else
    {
        /*  This better be really unimportant not to buy, */
        if(AI_Is_Cash_Cow(property))    /* utility or Railroad?  buy. */
        {
            buy = TRUE;
        }

        /*  Do we get a monopoly? Then buy it. */
        if(importance_to_us == AI_PROPERTY_GIVES_DIRECT_MONOPOLY)
            return TRUE;

        /*  Check whether this will give one of the players a monopoly, or allow
            a group trade that was not possible before.    */
        if(importance_to_others == AI_PROPERTY_ALLOWS_TRADE)
        {
            return TRUE;
        }

        if(importance_to_others == AI_PROPERTY_GIVES_DIRECT_MONOPOLY)
            return TRUE;   /* Got the cash - prevent the monopoly. */


        if(group == SG_PARK_PLACE || group == SG_MEDITERRANEAN_AVENUE || group == SG_ST_JAMES_PLACE)
        {
            buy = TRUE;
        }

        /*  Not important for other players.  How about for us?  Will it give
            us a group trade where one was not possible before?  */
        if((importance_to_us != AI_PROPERTY_NOT_IMPORTANT) && !buy)
        { /* Group trade is possible that was not possible before. */
            buy = TRUE;
        }

        /* Check if we are in a situation where we can take chances - ie.  If we are the only
           ones who own a monopoly.  If so, we should have at least one property from each
           monopoly to prevent others from getting them, even if it means selling a house or
           two to buy them. */
        if(AI_Only_Player_Have_Monopoly(game_state, player))
        {
            if(!AI_Owns_Prop_From_Monopoly(game_state, player, property) &&
               !AI_Is_Cash_Cow(property))
                return TRUE;
        }


        /*  Otherwise, must not mortgage a bare monopoly.  not worth it.  */
        if(((assets_not_sell_bare_monopolies - prop_cost) < must_have_cash_left) && !buy)
            return(FALSE);
    }

    if(AI_Cash_Available_After_Housing(game_state, player) < prop_cost)
        return FALSE;  /* Not important enough to use money that can be used for
                              buying houses. */
return TRUE;
}


/******************************************************************/
/*                  AI_Have_Monopoly_Trade                        */
/*                                                                */
/*   Input: RULE_GameStatePointer   game_state                    */
/*          RULE_PlayerNumber       player    Find trade for who? */
/*          RULE_PlayerNumber exclude_player  Who not to include  */
/*   Output:  BOOL - TRUE if 'player' has a possible monopoly     */
/*                   trade with everyone but exclude_player.      */
/******************************************************************/

BOOL AI_Have_Monopoly_Trade(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
                            RULE_PlayerNumber exclude_player)
{
    int                 index, num_players_in_list = 0;
    RULE_PlayerNumber   player_list[RULE_MAX_PLAYERS + 1];
    RULE_PropertySet    property_set, properties[RULE_MAX_PLAYERS + 1];
    RULE_SquareType     cur_square;

   /* Prepare property array. */
    for(index = 0; index < game_state->NumberOfPlayers; ++index)
    {
        properties[index] = RULE_PropertySetOwnedByPlayer(game_state, (RULE_PlayerNumber) index);

        /* need a list of potential players we can trade with. */
        if(index == player)
            continue;

        if(game_state->Players[index].currentSquare == SQ_OFF_BOARD)
            continue;   /* gone bankrupt! */

        if(index == exclude_player)
            continue;

        player_list[num_players_in_list++] = index;
    }

    /* If we have someone we can trade with, find out if a monopoly trade is
       possible. */
    if(num_players_in_list > 0)
    {
        /* Go through all the monopolies. */
        for(cur_square = 0; cur_square < SG_PARK_PLACE + 1; ++cur_square)
        {
            property_set = AI_Get_Monopoly_Set(AI_Expensive_Monopoly_Square[cur_square]);

            if(AI_Find_Smallest_Monopoly_Trade(player, property_set, player_list,
                                               num_players_in_list, properties))
                return TRUE;    /* We have a trade! */
        }
    }

return FALSE;  /* No trade found. */
}

/******************************************************************/
/*                  AI_Choose_Tax_Decision                        */
/*                                                                */
/*   Input:                                                       */
/*          RULE_PlayerNumber   player      Who is paying?        */
/*                                                                */
/*     Decides whether to pay 10% or $200 flat fee.  Sends the    */
/*   appropriate action message for the choice.                   */
/******************************************************************/


void AI_Choose_Tax_Decision(RULE_PlayerNumber player)
{
    if(AI_Get_Total_Worth(&UICurrentGameState, player) * UICurrentGameState.GameOptions.taxRate / 100
        > UICurrentGameState.GameOptions.flatTaxFee)
    {   /* We are worth more than $2000.  Pay flat free.  */
        AI_SendAction (ACTION_TAX_DECISION, player, RULE_BANK_PLAYER,
                        FALSE, 0, 0, 0,  NULL, 0, NULL);

    }
    else
    {   /* We are worth less than $2000.  Pay 10%. */
        AI_SendAction (ACTION_TAX_DECISION, player, RULE_BANK_PLAYER,
                        TRUE, 0, 0, 0,  NULL, 0, NULL);
    }

return;
}


/******************************************************************/
/*                  AI_Choose_Exit_Choice                         */
/*                                                                */
/*   Input:                                                       */
/*          RULE_PlayerNumber   player      Who is paying?        */
/*          BOOL  can_roll_doubles      True if can roll dobules  */
/*          BOOL  can_pay_fifty      True if can pay $50 to leave */
/*          BOOL  can_use_card       True if have jail card.      */
/*                                                                */
/*      Decides whether to stay in jail or leave.  Will stay if   */
/*  the computer doesn't desperately want ELECTRIC CO, Virginia,  */
/*  ST. JAMES or TENNESSEE.  Otherwise, will stay in jail if the  */
/*  average rent paid per circuit of the board is less than       */
/*  AI_JAIL_STAY_AVERAGE_RENT ($71). (see Darzinskis, P. 86-90)   */
/******************************************************************/


void AI_Choose_Exit_Choice(RULE_PlayerNumber player, BOOL can_roll_doubles,
                           BOOL can_pay_fifty, BOOL can_use_card)
{
    float average_rent = (float) 0.0;

    /* If it is the third turn and we have a card, use it.  Can't do much about it. */
    if(can_use_card && (UICurrentGameState.Players[player].turnsInJail == 2))
    {   /*  Use Card. 3rd roll, might as well use our card. */
        AI_SendAction (ACTION_EXIT_JAIL_DECISION, player, RULE_BANK_PLAYER,
                        2, 0, 0, 0,  NULL, 0, NULL);

        return;
    }


    if(!can_roll_doubles)   /* Can we roll?  If not, there is no choice. */
    {
        if(can_use_card)
        {   /* Use the card if we have it.  Otherwise, pay the $50.  */
            AI_SendAction (ACTION_EXIT_JAIL_DECISION, player, RULE_BANK_PLAYER,
                        2, 0, 0, 0,  NULL, 0, NULL);
            return;
        }

        AI_SendAction (ACTION_EXIT_JAIL_DECISION, player, RULE_BANK_PLAYER,
                        1, 0, 0, 0,  NULL, 0, NULL);
        return;
    }

    /*  Determine whether we want ELECTRIC CO, VIRGINIA AVE,  ST. JAMES, or TENNESSEE, a lot more than
        any other unowned property, and whether they are unowned.  Staying in jail will raise
        our chance of stepping on one of these.  If we really want one of them, stay in jail.  */

        /* Wait for Electric Co. only if there are very few properties left to buy,
           it is unowned, and we have the other utility. */
    if((UICurrentGameState.Squares[SQ_ELECTRIC_COMPANY].owner == RULE_NOBODY_PLAYER) &&
        (UICurrentGameState.Squares[SQ_WATER_WORKS].owner == player) &&
        (AI_Num_Properties_Left_To_Buy(&UICurrentGameState) <= 3))
    {  /* electric CO would be nice.  stay in jail to get it. */
        AI_SendAction (ACTION_EXIT_JAIL_DECISION, player, RULE_BANK_PLAYER,
                        0, 0, 0, 0,  NULL, 0, NULL);
        return;
    }

    if(UICurrentGameState.Squares[SQ_VIRGINIA_AVENUE].owner == RULE_NOBODY_PLAYER)
    {
        if( AI_Give_Players_Important_Monopoly(&UICurrentGameState, SQ_VIRGINIA_AVENUE,
                            RULE_NOBODY_PLAYER, RULE_NOBODY_PLAYER) == AI_PROPERTY_GIVES_DIRECT_MONOPOLY)
        { /* St. James Place gives a direct monopoly for someone. GET IT! */
            AI_SendAction (ACTION_EXIT_JAIL_DECISION, player, RULE_BANK_PLAYER,
                            0, 0, 0, 0,  NULL, 0, NULL);
            return;
        }
    }

    if(UICurrentGameState.Squares[SQ_TENNESSEE_AVENUE].owner == RULE_NOBODY_PLAYER)
    {
        if( AI_Give_Players_Important_Monopoly(&UICurrentGameState, SQ_TENNESSEE_AVENUE,
                            RULE_NOBODY_PLAYER, RULE_NOBODY_PLAYER) == AI_PROPERTY_GIVES_DIRECT_MONOPOLY)
        { /* Tennessee Avenue gives a direct monopoly for someone. GET IT! */
            AI_SendAction (ACTION_EXIT_JAIL_DECISION, player, RULE_BANK_PLAYER,
                            0, 0, 0, 0,  NULL, 0, NULL);
            return;
        }
    }

    if(UICurrentGameState.Squares[SQ_ST_JAMES_PLACE].owner == RULE_NOBODY_PLAYER)
    {
        if( AI_Give_Players_Important_Monopoly(&UICurrentGameState, SQ_ST_JAMES_PLACE,
                            RULE_NOBODY_PLAYER, RULE_NOBODY_PLAYER) == AI_PROPERTY_GIVES_DIRECT_MONOPOLY)
        { /* Tennessee Avenue gives a direct monopoly for someone. GET IT! */
            AI_SendAction (ACTION_EXIT_JAIL_DECISION, player, RULE_BANK_PLAYER,
                            0, 0, 0, 0,  NULL, 0, NULL);
            return;
        }
    }

    /* The properties are not important enough to justify staying in jail. How about
       rent? */

    average_rent = (float) (UICurrentGameState.GameOptions.passingGoAmount
                     - AI_Average_Rent_Payed(&UICurrentGameState, player, 0, TRUE, 1.0f));

    /* It takes on average 5 turns to go around, hence divide by 5 to get
       what we are losing per turn. */
    average_rent /= 5;

    /* On average, get out of jail in 2.5 turns - hence on average, save
       JailFee for 2.5 turns of average rent. */
    average_rent *= (float) 2.5;

    /* Would we be saving money by staying in jail? */
    if(average_rent < UICurrentGameState.GameOptions.getOutOfJailFee)
    {
        /*  Choose stay in jail.  Roll the dice. */
        AI_SendAction (ACTION_EXIT_JAIL_DECISION, player, RULE_BANK_PLAYER,
                        0, 0, 0, 0,  NULL, 0, NULL);
        return;
    }

    /* exit jail by paying or using card. */
    if(can_use_card)
    {
        /* use card. */
        AI_SendAction (ACTION_EXIT_JAIL_DECISION, player, RULE_BANK_PLAYER,
                        2, 0, 0, 0,  NULL, 0, NULL);

    }
    else
    {
        /*  Pay $50.  */
        AI_SendAction (ACTION_EXIT_JAIL_DECISION, player, RULE_BANK_PLAYER,
                        1, 0, 0, 0,  NULL, 0, NULL);
    }

return;
}


/******************************************************************/
/*                    AI_Send_Config_Accepts                      */
/*                                                                */
/*   Input  RULE_PlayerSet    players   which players need to     */
/*                  accepts configurations.                       */
/*      wchar_t       *stringA  The configs to accept.            */
/*                                                                */
/*    Sends an accepts message for each AI who needs to accept    */
/*  current configurations.                                       */
/******************************************************************/

void AI_Send_Config_Accepts(RULE_ActionArgumentsPointer NewMessage)
{
  RULE_PlayerSet players;
  RULE_PlayerNumber cur_player;
  RULE_ActionArgumentsRecord  MyMessage;
  static RULE_GameOptionsRecord options;
  BOOL changed = FALSE;

  players = (RULE_PlayerSet) NewMessage->numberB;

  RULE_ConvertFileToGameOptions (NewMessage->binaryDataA,
    NewMessage->binaryDataSizeA, &options);

  /* If the host is ignorant about immunities and futures, turn them off. */

  if (NewMessage->numberC < 1)
  {
    options.futureRentTradingAllowed = FALSE;
    options.immunitiesTradingAllowed = FALSE;
  }

  /* If this is a super-fast drone game, make sure to set the game state
     to really fast. */
  if(AI_Drone_Game)
  {
    if( options.AITakesTimeToThink != FALSE ||
       options.houseShortageLevel != 0  ||
       options.hotelShortageLevel != 0)
       changed = TRUE;

    options.AITakesTimeToThink = FALSE;
    options.houseShortageLevel = 0;
    options.hotelShortageLevel = 0;
  }

  for(cur_player = 0; cur_player < RULE_MAX_PLAYERS; ++cur_player)
  {
    if(SlotIsALocalAIPlayer[cur_player] && !AI_State[cur_player].num_actions)
    {
      if( (1 << cur_player)  & players)
      { /* A local AI player needs to accept configs. */

      /* Send the game options to the rules engine.  But don't close this box
         because it stays up to show the other player's configuration changes. */

        memset (&MyMessage, 0, sizeof (MyMessage));
        MyMessage.action = ACTION_ACCEPT_CONFIGURATION;
        MyMessage.fromPlayer = cur_player;
        MyMessage.toPlayer = RULE_BANK_PLAYER;
        MyMessage.numberC = 1;  /* Version of the client: 1 means Monopoly Star Wars which does futures. */
        if (AddGameOptionsToMessageBlob (&options, &MyMessage))
          MESS_SendActionMessage (&MyMessage);

        AI_State[cur_player].num_actions++;

        if(changed) /* Send only once if changed the options. */
          break;
      }
    }
  }

return;
}

/******************************************************************/
/*                  AI_Picked_Up_Card                             */
/*                                                                */
/*   Input:  RULE_PlayerNumber  player                            */
/*                                                                */
/*      Sends a ACTION_CARD_SEEN message.                         */
/******************************************************************/

void AI_Picked_Up_Card(RULE_PlayerNumber player)
{

    /* Send message that we've looked at the card. */
    AI_SendAction (ACTION_CARD_SEEN, player, RULE_BANK_PLAYER, 0, 0, 0, 0, NULL, 0, NULL);

return;
}


/******************************************************************/
/*                  AI_Resend_State                               */
/*                                                                */
/*   Input:  RULE_PlayerNumber  player                            */
/*                                                                */
/*      Sends a ACTION_RESTART_PHASE message.                     */
/******************************************************************/

void AI_Resend_State(RULE_PlayerNumber player)
{
    MESS_SendAction (ACTION_RESTART_PHASE, player, RULE_BANK_PLAYER, 0, 0, 0, 0,NULL, 0, NULL);
return;
}

/**********************************************************************************/
/*                  AI_Choose_Trade_Acceptance                                    */
/*                                                                                */
/*  Input:  RULE_PlayerSet   players_involved    Who are we waiting for to accept?*/
/*                                                                                */
/*      Considers whether to accept the current offer.  Sends the appropriate     */
/*  ACTION message for all AI players in players_involved.                        */
/**********************************************************************************/


void AI_Choose_Trade_Acceptance(RULE_PlayerSet players_involved)
{
    RULE_PlayerNumber   cur_player;

    /* Have we just responded to this trade with a rejection/counter?  Then shouldn't
       repeat the signals. */
    if(AI_Glob.Trade_Just_Rejected_Countered)
        return;

    for(cur_player = 0; cur_player < UICurrentGameState.NumberOfPlayers; ++cur_player)
    {
        /* Are we asked to accept this trade? If not.. we really have nothing to say. */
        if( !(players_involved & (1 << cur_player)))
            continue;

        if(!SlotIsALocalAIPlayer[cur_player])
            continue;   /* Not an AI. */

        /* Are we involved in this trade?  If not - why are we asked to
           accept it?  Just pass it on - accept the trade. */
        if(AI_Player_Involved_In_Trade(Current_Trade_List, cur_player))
        {   /* Involved in the trade.  Then decide. */
            if(AI_State[cur_player].num_actions != 0)
            {   /* We're doing something already. Allow only one action */
                    /* Make sure we return back here once we
                       are done whatever we are doing.  */
                if(!(AI_State[cur_player].sending_message.Action == ACTION_TRADE_ACCEPT))
                {   /* Did we JUST send a trade accept message?  If not, make
                       sure we come back here. */
                    AI_State[cur_player].message_to_send.action = ACTION_RESTART_PHASE;
                    AI_State[cur_player].message_to_send.fromPlayer = cur_player;
                    AI_State[cur_player].message_to_send.toPlayer = RULE_BANK_PLAYER;
                }
                return;
            }

            /* See if we accept the trade.  Note: we should check whether we are fed up with
               the player proposing the trade because he/she is constantly proposing the
               same trade. */
                /* But first, see if the AI is giving immunities.  If yes, don't
                   even consider the trade since the AIs do not do immunities/futures yet. */
            if(AI_Player_Give_Immunities_Futures(cur_player))
            {
                AI_SendAction (ACTION_TRADE_ACCEPT, cur_player, RULE_BANK_PLAYER,
                            FALSE, 0, 0, 0, NULL, 0, NULL);
                AI_Send_Message(AI_MESS_Reject, cur_player, AI_Glob.Player_Proposed_Trade);
                break;
            }

            if(AI_Properties_Already_Traded(cur_player, AI_Glob.Player_Proposed_Trade, Current_Trade_List,
                                            AI_Data[cur_player].number_times_allow_property_trade))
            {
                if(AI_Evaluate_Trade(&UICurrentGameState, cur_player, cur_player, Current_Trade_List)
                                    >= AI_Data[cur_player].min_evaluation_if_fed_up)
                {  /* We accepted the trade.  Send the appropriate action. */
                    AI_SendAction (ACTION_TRADE_ACCEPT, cur_player, RULE_BANK_PLAYER,
                                TRUE, 1, 0, 0, NULL, 0, NULL);
                }
                else
                {
                    AI_SendAction (ACTION_TRADE_ACCEPT, cur_player, RULE_BANK_PLAYER,
                                FALSE, 0, 0, 0, NULL, 0, NULL);
                    AI_Send_Message(AI_MESS_Ask_Me_Later, cur_player, AI_Glob.Player_Proposed_Trade);
                    break;  /* No need to send more acceptances/rejections. */
                }
            }
            else
            {
                if(AI_Evaluate_Trade(&UICurrentGameState, cur_player, cur_player, Current_Trade_List)
                                    >= AI_Data[cur_player].min_evaluation_threshold)
                {  /* We accepted the trade.  Send the appropriate action. */
                    AI_SendAction (ACTION_TRADE_ACCEPT, cur_player, RULE_BANK_PLAYER,
                                TRUE, 1, 0, 0, NULL, 0, NULL);
                }
                else
                {
                    /* We are not accepting the trade, but how about counter proposing? */
                    AI_Glob.Trade_Just_Rejected_Countered = TRUE;
                    AI_Glob.Player_Just_Rejected_Countered = cur_player;
                    if(AI_Counter_Propose_Trade(&UICurrentGameState, cur_player, TRUE))
                    {
                        break;
                    }

                    AI_SendAction (ACTION_TRADE_ACCEPT, cur_player, RULE_BANK_PLAYER,
                                FALSE, 0, 0, 0, NULL, 0, NULL);
                    break;  /* No need to send more acceptances/rejections. */
                }
            }
        }
        else
        {   /* Not involved in the trade - not going to be a pain in the butt
               and reject it.  */
            AI_SendAction (ACTION_TRADE_ACCEPT, cur_player, RULE_BANK_PLAYER,
                            TRUE, 3, 0, 0,NULL, 0, NULL);
        }
    }

#if FORHOTSEAT
    #if _DEBUG
        if(ShowAIInfo && !ShowOnlyDrone)
            AI_Show_AI_Info_Box();
    #endif
#endif

return;
}


/**********************************************************************************/
/*                  AI_Load_Save_Parameters                                       */
/*                                                                                */
/*  Input:  RULE_ActionArgumentsPointer NewMessage  NOTIFY_AI_PARAMETERS message. */
/*                                                                                */
/*   Loads AI data from the RIFF binary block in NewMessage.                      */
/**********************************************************************************/

void AI_Load_Save_Parameters(RULE_ActionArgumentsPointer NewMessage)
{
    /* Make sure the player in numberA is legal. */
    if(NewMessage->numberA > RULE_MAX_PLAYERS)
    {
#if _DEBUG
        LE_ERROR_ErrorMsg ("Bad player number when trying to load AI parameters.");
#endif
        return; /* Something wrong - bad player number. */
    }

    /* Is this a local AI player? Load only local players. */
    if(SlotIsALocalAIPlayer[NewMessage->numberA])
    { /* This is one of the local AI players - hence copy the data block. */
        memcpy(&AI_Data[NewMessage->numberA], NewMessage->binaryDataA + AI_RIFF_IDENTIFIER_LENGTH
                + AI_RIFF_SIZE_LENGTH, sizeof(AI_Data_Record) );
    }

return;
}

/**********************************************************************************/
/*                  AI_Send_Save_Parameters                                       */
/*                                                                                */
/*  Input:  RULE_PlayerSet   players_to_save     Who are we waiting for to accept?*/
/*                                                                                */
/*   Save AI data using RIFF format.  AI players to save are in RULE_PlayerSet.   */
/**********************************************************************************/

void AI_Send_Save_Parameters(RULE_PlayerSet players_to_save)
{
    RULE_PlayerNumber   cur_player;
    AI_RIFF_Chunk_Record    *riff_record;
    long                riff_size;
    RULE_CharHandle   MemoryHandle = NULL;

    for(cur_player = 0; cur_player < UICurrentGameState.NumberOfPlayers; ++cur_player)
    {
        /* Are we asked to save our parameters? If not, don't do it. */
        if( !(players_to_save & (1 << cur_player)))
            continue;

        if(!SlotIsALocalAIPlayer[cur_player])
            continue;   /* Not an AI on this computer. */

        /* Allocate memory for a RIFF block.  It is going to be the size of AI_Data
           plus 4 bytes for chunk identifier, 4 bytes for chunk size, + up to 3 bytes
           to make the total a multiple of 4. */
        riff_size = sizeof(AI_RIFF_Chunk_Record);
        if(riff_size % 4)
            riff_size += 4 - (riff_size % 4);

        /* Allocate the memory. */
        MemoryHandle = RULE_AllocateHandle (riff_size);
        if (MemoryHandle == NULL)
            return; /* Failled to allocate the memory! */

        /* Loc it - get a pointer to it. */
        riff_record = (AI_RIFF_Chunk_Record *) RULE_LockHandle (MemoryHandle);
        if (riff_record == NULL)
        {
            RULE_FreeHandle (MemoryHandle);
            return; /* Failed to loc it! */
        }

        /* Copy the necessary information into the riff block. */
        memcpy(riff_record->identifier, AI_RIFF_SAVE_IDENTIFIER, AI_RIFF_IDENTIFIER_LENGTH);
        riff_record->length = riff_size - AI_RIFF_IDENTIFIER_LENGTH - AI_RIFF_SIZE_LENGTH;  /* size of data. */
        memcpy(&riff_record->ai_data, &AI_Data[cur_player], sizeof(AI_Data_Record) );

        /* Send the block. */
        RULE_UnlockHandle (MemoryHandle);

        MESS_SendAction(ACTION_AI_SAVE_PARAMETERS, cur_player, RULE_BANK_PLAYER,
                        0, 0, 0, 0,  NULL, 0, MemoryHandle);
    }

return;
}


/**********************************************************************************/
/*                  AI_Send_Housing_Shortage_Purchase_Requests                    */
/*                                                                                */
/*  Input:  RULE_PlayerNumber    ignore_player   Who not to send a request for -  */
/*                                          ie. the person starting the auction   */
/*          long     num_buildings_left      # of buildings left to purchase,     */
/*                                      + if houses are being auctioned, -        */
/*                                      if hotels.                                */
/*          RULE_PlayerNumber   player_started   Who caused this shortage?        */
/*                                                                                */
/*      Sends house purchase messages for all AI players when an house/hotel is   */
/*  to be auctioned.  num_buildings_left should be - if a hotel is being auctioned*/
/*  and + if a house.                                                             */
/**********************************************************************************/

void AI_Send_Housing_Shortage_Purchase_Requests(RULE_PlayerSet players, long num_buildings_left,
                                                RULE_PlayerNumber player_started)
{
    RULE_PlayerNumber   cur_player;

    /* Go through all players. */
    for(cur_player = 0; cur_player < RULE_MAX_PLAYERS; ++cur_player)
    {   /* Is this player in the player set? */
        if(! ( (1 << cur_player) & players))
            continue;

        if(cur_player == player_started)
            continue;   // Player who started this need not send one again. */

            /* Is it a local AI player? */
        if(SlotIsALocalAIPlayer[cur_player] && !AI_State[cur_player].num_actions)
        {
            /* Should we buy a house/hotel? */
            if(AI_Should_Buy_House(&UICurrentGameState, cur_player))
            {
                /* Then try to buy a house/hotel, depending on what is being
                   auctioned. */
                if( AI_Buy_Houses(&UICurrentGameState, cur_player,
                              (num_buildings_left < 0 ? AI_HOUSEOPS_HOUSES_ONLY:AI_HOUSEOPS_HOTELS_ONLY) |
                               AI_HOUSEOPS_AUCTION))
                    break;  /* No need to send auction requests. */
            }
        }
    }

return;
}

/**********************************************************************************/
/*                  AI_Trap_Trade_Message                                         */
/*                                                                                */
/*  Input:  RULE_ActionArgumentsPointer    Message   The error message            */
/*                                                                                */
/*      Traps TMN_ERROR_TRADE_ACCEPTED and TMN_ERROR_TRADE_REJECTED messages and  */
/*  updates the AI's attitude towards the player rejecting/accepting.             */
/**********************************************************************************/

void AI_Trap_Trade_Message(RULE_ActionArgumentsPointer Message)
{

    switch(Message->numberA)
    {
        case TMN_ERROR_TRADE_CHANGING:
            // Trade changed - then consider the trade as proposed by the player changing it.
            AI_Glob.Player_Proposed_Trade = AI_Glob.Player_Last_Edited;
            break;

        case TMN_ERROR_TRADE_REJECTED:
            AI_Player_Accepted_Rejected_Trade(AI_Glob.Player_Proposed_Trade,
                                              (RULE_PlayerNumber) Message->numberC, FALSE);
            break;

        case TMN_ERROR_TRADE_ACCEPTED:
            AI_Player_Accepted_Rejected_Trade(AI_Glob.Player_Proposed_Trade,
                                              (RULE_PlayerNumber) Message->numberC, TRUE);
            break;
    }

return;
}

/**********************************************************************************/
/*                  AI_Player_Accepted_Rejected_Trade                             */
/*                                                                                */
/*  Input:  RULE_PlayerNumber   proposed_player   Who proposed the trade?         */
/*          RULE_PlayerNumber   player            Who accepted/rejected it?       */
/*          BOOL                accepted          TRUE if trade was accepted.     */
/*                                                                                */
/*      This function updates all the AIs player evaluation towards player        */
/*  as a result of his accepting/rejecting the last trade.                        */
/**********************************************************************************/

void AI_Player_Accepted_Rejected_Trade(RULE_PlayerNumber proposed_player, RULE_PlayerNumber player, BOOL accepted)
{
    RULE_PlayerNumber   cur_player;
    float               evaluation;

    if(proposed_player >= RULE_MAX_PLAYERS)
            return; /* Not a valid player_started_trade */

    /* Go through all the AIs, and evaluate the trade. */
    if(!AI_Player_Give_Immunities_Futures(player))
    {   /* evaluate only if immunities are not taking part.  AIs can't
           evaluate immunities/futures. */
        for(cur_player = 0; cur_player < UICurrentGameState.NumberOfPlayers; ++cur_player)
        {
            if(SlotIsALocalAIPlayer[cur_player])
            {   /* Found an AI.  What does the AI think of the trade? */
                evaluation = AI_Evaluate_Trade(&UICurrentGameState, player, cur_player, Current_Trade_List);
                evaluation *= AI_Data[cur_player].attitude_change_trade_observing;

                if(evaluation < 0)
                {
                    if(evaluation < -AI_Data[cur_player].attitude_lost_for_rejected_trade)
                        evaluation = -AI_Data[cur_player].attitude_lost_for_rejected_trade;
                }
                else
                {
                    if(evaluation > AI_Data[cur_player].attitude_lost_for_rejected_trade)
                        evaluation = AI_Data[cur_player].attitude_lost_for_rejected_trade;
                }

                if(accepted)
                {
                    if(evaluation < 0)
                    {   /* A bad trade was accepted! */
                        AI_Data[cur_player].player_attitude[player] -= evaluation;
                    }
                }
                else
                {
                    if(evaluation > 0)
                    {   /* A good trade was rejected! */
                        AI_Data[cur_player].player_attitude[player] -= evaluation;
                    }
                }
            }
        }
    }

    /* Take it personally for the AI proposing the trade. */
    if(SlotIsALocalAIPlayer[proposed_player])
    {
        if(accepted)
        {
            AI_Data[proposed_player].player_attitude[player] +=
                    AI_Data[proposed_player].attitude_lost_for_rejected_trade;
        }
        else
        {
            AI_Data[proposed_player].player_attitude[player] -=
                    AI_Data[proposed_player].attitude_lost_for_rejected_trade;
        }
    }

return;
}


/**********************************************************************************/
/*                  AI_Trap_Other_Errors                                          */
/*                                                                                */
/*  Input:  RULE_ActionArgumentsPointer    Message   The error message            */
/*                                                                                */
/*  Responds to any important errors (other than trade messages) for the AIs.     */
/**********************************************************************************/

void AI_Trap_Other_Errors(RULE_ActionArgumentsPointer Message)
{
    int num_players_left = 0, counter;
    RULE_PlayerNumber   player_left;

    switch(Message->numberA)
    {
        case TMN_ERROR_BANKRUPT_TO:
        case TMN_ERROR_UNDO_FOR_BANKRUPTCY_TO:
            for(counter = 0; counter < UICurrentGameState.NumberOfPlayers; ++counter)
            {
                if(counter == Message->numberC)
                    continue; /* Is this the bankrupt player? */

                if(UICurrentGameState.Players[counter].currentSquare != SQ_OFF_BOARD)
                {
                    num_players_left++;
                    player_left = (RULE_PlayerNumber) counter;
                }
            }

            if(num_players_left <= 1)
            {
                //AI_Player_Won(player_left);
            }
            break;
    }

return;
}


/************************************************************************************/
/*                  AI_Get_Trade_Bid_For_Property                                   */
/*                                                                                  */
/*   Input: RULE_GameStatePointer  game_state                                       */
/*          RULE_PlayerNumber player  Who is bidding?                               */
/*          RULE_SquareType property   Bidding on which property?                   */
/*          long current_bid           what is the current bid?                     */
/*          RULE_PlayerNumber cur_bid_player   Who has the current bid?             */
/*   Ouput: long     -  Bid player should place.  0 if outbid.                      */
/*                                                                                  */
/*      This function evaluates the current bid from the point of view of a trade.  */
/*  It supposes the bid was a trade between us and the current highest bid player,  */
/*  and sees if we would go for it.  Returns a new bid, slightly increased, or 0.   */
/************************************************************************************/

long AI_Get_Trade_Bid_For_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
                             RULE_SquareType property, long current_bid, RULE_PlayerNumber cur_bid_player)
{
    float   chances_before, chances_after;
    RULE_GameStateRecord    state_after_bid, state_after_our_bid;
    long    worth_before, worth_after, change;
    float   evaluation;
    float   trade_property_importance;
    AI_Trade_List_Record    temp_trade[RULE_MAX_PLAYERS];
    long    mortgage_cost = RULE_SquarePredefinedInfo[property].mortgageCost;

    if(cur_bid_player == player)
        return 0;  /* No need to outbid ourselves. */

    change = 10;    /* Consider a $10 increase. */

    /*  Create a new game state after the auction the way it is now, and if we bid extra. */
    memcpy(&state_after_bid, game_state, sizeof( RULE_GameStateRecord));
    memcpy(&state_after_our_bid, game_state, sizeof( RULE_GameStateRecord));

    state_after_bid.Squares[property].owner = cur_bid_player;
    state_after_bid.Players[cur_bid_player].cash -= current_bid;

    state_after_our_bid.Squares[property].owner = player;
    state_after_our_bid.Players[player].cash -= current_bid + change;

    /* Go through all the players and mortgage/sell properties to meet the
       cash demands of their bids. */
    AI_Mortgage_Negative_Cash_Players(&state_after_bid);
    AI_Mortgage_Negative_Cash_Players(&state_after_our_bid);

    if((AI_Get_Liquid_Assets(&state_after_bid, cur_bid_player, TRUE, TRUE) - mortgage_cost) < 0)
        return 0;   /* Current player has outbid her assets - let her go bankrupt. */

    if((AI_Get_Liquid_Assets(&state_after_our_bid, player, TRUE, TRUE) - mortgage_cost) < 0)
        return 0;  /* We've outbid ourselves - forget it. */

    /*  What are our chances if we let the auction slide vs bidding? */
    chances_before =  AI_Evaluate_Winning_Chances(&state_after_bid, player, player, NULL);
    chances_after = AI_Evaluate_Winning_Chances(&state_after_our_bid, player, player, NULL);

    /*  Evaluate the cash logistics of this trade. (ie. how much
        are we worth before and after.) */
    worth_before = AI_Get_Total_Worth_With_Factors(&state_after_bid, player, player);
    worth_after = AI_Get_Total_Worth_With_Factors(&state_after_our_bid, player, player);

        /* Subtract/add value of jail card. */
    if(state_after_bid.Cards[CHANCE_DECK].jailOwner == player)
        worth_before += AI_JAIL_CARD_VALUE;

    if(state_after_bid.Cards[COMMUNITY_DECK].jailOwner == player)
        worth_before += AI_JAIL_CARD_VALUE;

    if(state_after_our_bid.Cards[CHANCE_DECK].jailOwner == player)
        worth_after += AI_JAIL_CARD_VALUE;

    if(state_after_our_bid.Cards[COMMUNITY_DECK].jailOwner == player)
        worth_after += AI_JAIL_CARD_VALUE;

    /*  Did our chance of winning drop too much to make any other
        factors not worth considering? */
    if((chances_before - chances_after) > AI_Data[player].chances_threshold)
        return (0);

    /*  Calculate the effect of giving away or receiving important
        properties.  Use AI_Calc_Trade_Property_Importance. */
    memset(temp_trade, 0, sizeof(temp_trade) );
    temp_trade[cur_bid_player].properties_given =
            temp_trade[player].properties_received = RULE_PropertyToBitSet(property);

    trade_property_importance = AI_Calc_Trade_Property_Importance(&state_after_bid, &state_after_our_bid,
                                        player, player, temp_trade, FALSE);

    evaluation = (chances_after - chances_before) * AI_Data[player].chances_factor +
                 (worth_after - worth_before) * AI_Data[player].cash_factor +
                 trade_property_importance * AI_Data[player].trade_importance_factor;

    if(evaluation >= 0)  /* Is this a *good* bid? */
        return (current_bid + change);

return 0;
}



/************************************************************************************/
/*                  AI_Get_Bid_For_Property                                         */
/*                                                                                  */
/*   Input: RULE_GameStatePointer  game_state                                       */
/*          RULE_PlayerNumber player  Who is bidding?                               */
/*          RULE_SquareType property   Bidding on which property?                   */
/*          long current_bid           what is the current bid?                     */
/*          RULE_PlayerNumber cur_bid_player   Who has the current bid?             */
/*   Ouput: long     -  Bid player should place.  0 if outbid.                      */
/*                                                                                  */
/*      This function determines what player should bid next for 'property'.        */
/************************************************************************************/

long AI_Get_Bid_For_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
                             RULE_SquareType property, long current_bid, RULE_PlayerNumber cur_bid_player)
{
    AI_Property_Importance importance;
    long liquid_assets, prop_mortg_cash, cash_after_housing, excess_cash;
    long prop_cost, change, liquid_assets_selling_houses, trade_bid;
    RULE_PlayerNumber   owner;
    double  bid;
    BOOL    increase_bid = FALSE;
    int     squares_unowned, houses_wanted, num_monopolies, counter, free_houses;
    RULE_SquareType monopoly_squares[SG_PARK_PLACE + 1];

    if(cur_bid_player == player)
        return 0;   /* We have the highest bid - no need to outbid ourself! */

    /* First calculate the cash available. */
    liquid_assets = AI_Get_Liquid_Assets(game_state, player, FALSE, FALSE);
    liquid_assets_selling_houses = AI_Get_Liquid_Assets(game_state, player, TRUE, FALSE);
    cash_after_housing = AI_Cash_Available_After_Housing(game_state, player);
    excess_cash = AI_Excess_Cash_Available(game_state, player, FALSE);

    /* Now see how important this property is. */
    if(property < SQ_IN_JAIL)  /* Is this a hotel or house? */
    {   /* No - regular property. */
        prop_mortg_cash = RULE_SquarePredefinedInfo[property].mortgageCost;
        prop_cost   = RULE_SquarePredefinedInfo[property].purchaseCost;
        importance = AI_Give_Players_Important_Monopoly(game_state, property, RULE_NOBODY_PLAYER, RULE_NOBODY_PLAYER);
        trade_bid = AI_Get_Trade_Bid_For_Property(game_state, player, property, current_bid, cur_bid_player);
    }
    else
    {   /* A house or hotel. */
        if(property == SQ_IN_JAIL)
            importance = AI_PROPERTY_IS_A_HOUSE;
        else
            importance = AI_PROPERTY_IS_A_HOTEL;
    }

    switch(importance)
    {
        case AI_PROPERTY_IS_A_HOUSE:
        case AI_PROPERTY_IS_A_HOTEL:
            if(AI_Should_Buy_House(game_state, player))
            {
                /* Bid should be our excess assets / how many houses we want,
                   or how many houses are free, whichever is smaller. */
                    /* Get the houses we can buy. */
                num_monopolies = (int) AI_Get_Monopolies(game_state, player, monopoly_squares, FALSE);
                houses_wanted = 0;
                for(counter = 0; counter < num_monopolies; ++counter)
                    houses_wanted += AI_Num_House_Can_Buy_On_Monopoly(game_state, monopoly_squares[counter]);

                free_houses = AI_Free_Houses(game_state);
                if(free_houses < houses_wanted)
                    houses_wanted = free_houses;

                bid = excess_cash / houses_wanted;
            }
            else
                return 0;
            break;

        case AI_PROPERTY_GIVES_DIRECT_MONOPOLY:
            /* Gives a direct monopoly - but for who?  Make sure we out bid the person
               who would get it. */
            owner = AI_Who_Owns_Monopoly(game_state, property);
            if(cur_bid_player == owner)
            {   /* The player who is going to get the monopoly has the highest bid.
                   We need to outbid him/her at almost all costs. */
                /* We cannot kill ourself with the bid - we must have some
                   chances of winning after.  */

                /* How many properties are left to buy? */
                squares_unowned = AI_Num_Properties_Left_To_Buy(game_state);

                /* Do we own a monopoly? If yes, do not sacrifice it unless
                   no one else has a monopoly. */
                if(AI_Player_Own_Monopoly(game_state, player, FALSE))
                {
                    /* Does anyone else have a monopoly? */
                    if(AI_Only_Player_Have_Monopoly(game_state, player))
                    {   /* YES! We are the only ones with a monopoly. Hence
                           we can take a chance - sacrifice some houses */
                        bid = (liquid_assets + liquid_assets_selling_houses * AI_Data[player].monopoly_suicide_factor)/2;
                    }
                    else bid = excess_cash;
                        /* Someone else has a monopoly - can't suicide myself for
                           their benefit. */
                }
                else
                {
                    /* First, calculate what we are willing to bid if we have
                       no trade possibilities, and no monopolies.  */
                    /* number should be based on the number of properties left
                       to buy (more properties left to buy, the higher the probability
                       that we can get a trade, or a natural monopoly.) */
                    bid = excess_cash * AI_Data[player].monopoly_suicide_factor;
                        /* Multiply by a number from 0.5 - 1.0, depending on how many squares are
                           unowned. */
                    bid *= 0.5 + (0.5  -  squares_unowned / ((double) AI_NUMBER_MONOPOLY_SQUARES) / 2);

                    /* No monopoly - but how about a trade for a monopoly? */
                    if(AI_Have_Monopoly_Trade(game_state, player, owner) )
                    {   /* We can get a monopoly trade. Hence there is a higher
                           probability of success if the player gets the monopoly - means
                           we should bet less. */
                        bid = excess_cash * AI_Data[player].monopoly_suicide_factor * 0.5;
                    }

                    /* Make sure our bid is no less than what we would bid for a trade property. */
                    if(trade_bid > bid)
                        bid = trade_bid;
                }
                break;
            }
            else
            {   /* Is it us who would get the monopoly if we purchased it? */
                if(owner == player)
                {   /* YES! Use liquid assets left AFTER buying houses. */
                    bid = cash_after_housing;
                    break;
                }

                /* Not us - then treat it as a trade property. */
                bid = trade_bid;
            }

            break;

        default:
            bid = trade_bid;
            break;
    }

    if(bid > excess_cash)
        bid = excess_cash;

    /* Make sure we bid at least the mortgage value of the property + a bit if we can. */
    if(bid < prop_mortg_cash && property < SQ_IN_JAIL)
    {
        if(liquid_assets >= (prop_mortg_cash + bid/2))
        {   /* Can we bid the mortgage cost + a bit?  Then bid it. */
            bid = prop_mortg_cash + bid/2;
        }
        else
            bid = liquid_assets; /* Then bid what we can. */
    }

    if(bid < prop_cost && property < SQ_IN_JAIL)
    {
        /* Check if we are in a situation where we can take chances - ie.  If we are the only
           ones who own a monopoly.  If so, we should have at least one property from each
           monopoly to prevent others from getting them, even if it means selling a house or
           two to buy them. */
        if(AI_Only_Player_Have_Monopoly(game_state, player))
        {
            if(!AI_Owns_Prop_From_Monopoly(game_state, player, property)
                && !AI_Is_Cash_Cow(property))
            {
                if(excess_cash >= prop_cost)
                    bid = prop_cost;
            }
        }
    }


    if(((long) bid) > current_bid)
    {   /* We want to bid more than the current bid.  Then make our new bid
           just a bit more than the existing one. Increase the bid by a lot more
           if we are far away from what we are prepared to bid for it.  Increase
           it by just a little if we are close to what we would pay for it. */

        /* Randomly add 10 or 5. */
        change = (rand() % 2 ? 10:5);
        if((current_bid + change) > bid)
            bid = 0;    /* Do not bid more than we are willing. */
        else
            bid = current_bid + change;
    }
    else bid = 0;  /* We got outbid.  Then don't bid anything. */

return ((long) bid);
}

/****************************************************************/
/*                  AI_Resync_Client                            */
/*                                                              */
/*  This functions resets flags if a NOTIFY_CLIENT_RESYNC_INFO  */
/*  message was sent (ie. if a packet was lost.)                */
/****************************************************************/

void AI_Resync_Client(void)
{
    AI_Initialize_Global();
return;
}

/************************************************************************************/
/*                  AI_Bid_In_Auction                                               */
/*                                                                                  */
/*   Input: RULE_GameStatePointer  game_state                                       */
/*   Ouput: BOOL - TRUE if an auction is going on, FALSE otherwise.                 */
/*                                                                                  */
/*      This function will randomly find an AI willing to bid in the auction        */
/*  and send its new bid.  Will only allow one AI to send a bid at a time           */
/*  not to swamp the bidding with AI calls.  Returns TRUE if an auction is going    */
/*  on, and FALSE if no auction is on.                                              */
/************************************************************************************/

BOOL AI_Bid_In_Auction(RULE_GameStatePointer game_state)
{
    RULE_PlayerNumber   cur_player;
    int                 players_checked = 0;
    long                bid;

#ifdef AI_NO_AUCTIONS
    return FALSE;
#endif

    if(AI_Glob.AUCTION_IS_ON)
    {   /* Auction is on.*/
        /* Are we waiting for an AI to finish its bid? */
        if(AI_Glob.PLAYER_CURRENTLY_BIDDING)
            return TRUE;  /* Yes - then wait for that message to get thru. */

        /* Pick an AI player randomly to start. */
        cur_player = ((long) rand()) * game_state->NumberOfPlayers / RAND_MAX;

        for(players_checked = game_state->NumberOfPlayers; players_checked > 0; --players_checked)
        {
            if(SlotIsALocalAIPlayer[cur_player] &&
               (game_state->Players[cur_player].currentSquare != SQ_OFF_BOARD))
            {   /* We found an AI player able to bid. */
                bid = AI_Get_Bid_For_Property(game_state, cur_player, AI_Glob.AUCTION_PROPERTY,
                                              AI_Glob.AUCTION_HIGH_BID, AI_Glob.AUCTION_HIGH_BID_PLAYER);
                if(bid != 0)
                {   /* It's a real bid!  Send it. */
                    AI_SendAction(ACTION_BID, cur_player, RULE_BANK_PLAYER,
                                  bid, 0, 0, 0,  NULL, 0, NULL);
                    AI_Glob.PLAYER_CURRENTLY_BIDDING = TRUE;
                    return TRUE;
                }
            }

            if(++cur_player >= game_state->NumberOfPlayers)
                cur_player = 0;
        }

        /* No AI wants to bid at this moment.  Wait for the auction to be over, or kill
           it if playing in super-fast mode. */
        if(RunFastWithNoDisplay)
            return FALSE;

        return TRUE;
    }

/* No auction going on. */
return FALSE;
}


/************************************************************************************/
/*                  AI_Is_Paying_Fee                                                */
/*                                                                                  */
/*   Input: RULE_GameStatePointer  game_state                                       */
/*          RULE_PlayerNumber      player                                           */
/*   Ouput: BOOL - TRUE if player has some unresolved payment left.                 */
/*                                                                                  */
/*      This function is used to check whether player has some payment left that    */
/* has not yet been resolved.  Used to make sure player does not spend its cash.    */
/************************************************************************************/

BOOL    AI_Is_Paying_Fee(RULE_GameStatePointer game_state, RULE_PlayerNumber player)
{
    int index;

    /* Go through all the pending phases. */
    for(index = 0; index < game_state->NumberOfPendingPhases; ++index)
    {
        if((game_state->phaseStack[index].phase == GF_COLLECTING_PAYMENT) &&
            (game_state->phaseStack[index].fromPlayer == player) )
        {   /* Player owes some money - return TRUE. */
            return TRUE;
        }
    }

return FALSE;
}


/************************************************************************************/
/*                  AI_Process_AIs_At_Start_Turn                                    */
/*                                                                                  */
/*      This function, to be called every time a new turn is started, will make AIs */
/*  forget they traded properties.                                                  */
/************************************************************************************/

void AI_Process_AIs_At_Start_Turn(void)
{
    RULE_PlayerNumber cur_player;
    int counter;

    /* Go through the AIs and forget, if need be, which properties we traded in the past. */
    for(cur_player = 0; cur_player < UICurrentGameState.NumberOfPlayers; ++cur_player)
    {
        if(SlotIsALocalAIPlayer[cur_player])
        {
            /* Increate counter. */
            AI_State[cur_player].turns_after_forgetting_last++;

            /* Is it time to forget? */
            if(AI_State[cur_player].turns_after_forgetting_last >= AI_Data[cur_player].turns_to_forgetting_property_trade)
            {   /* Yes. then forget. */
                AI_Properties_Forget_Traded(cur_player);
                AI_State[cur_player].turns_after_forgetting_last = 0;
            }
                        /* Update trade timers. */
            for(counter = 0; counter < AI_MAX_TRADES; ++counter)
            {
                if(AI_Data[cur_player].time_last_trade[counter] > 0)
                    AI_Data[cur_player].time_last_trade[counter]--;
            }

            /* Update attitudes. */
            for(counter = 0; counter < RULE_MAX_PLAYERS; ++counter)
            {
                if(AI_Data[cur_player].player_attitude[counter] > AI_Data[cur_player].player_neutral_attitude)
                    AI_Data[cur_player].player_attitude[counter] -= AI_Data[cur_player].attitude_tick_change;
                else
                    AI_Data[cur_player].player_attitude[counter] += AI_Data[cur_player].attitude_tick_change;
            }
        }
    }

return;
}

/************************************************************************************/
/*                  AI_Finish_Trade                                                 */
/*                                                                                  */
/*   This function resets variables when a trade is finished, including counters    */
/*  keeping track of how many times each AI countered the trade.                    */
/************************************************************************************/

void AI_Finish_Trade(void)
{
    RULE_PlayerNumber   cur_player;

    for(cur_player = 0; cur_player < UICurrentGameState.NumberOfPlayers; ++cur_player)
        AI_State[cur_player].times_countered_trade = 0;

    AI_PLAYER_SENDING_TRADE = AI_Send_Nothing;
    AI_Glob.Trade_Started = FALSE;
    AI_Glob.Trade_offered_for_acceptance = FALSE;

return;
}

/********************************************************************************
 *                  AI_Process_BuySellMort                                      *
 *                                                                              *
 *   Input:  RULE_PlayerNumber player   Player having control                   *
 *                                                                              *
 *  This function processes the player in control, or tests to see whether      *
 *  any other AI wants control if someone is just finished with it.             *
 ********************************************************************************/

void AI_Process_BuySellMort(RULE_PlayerNumber player)
{
    RULE_PlayerNumber cur_player;
    AI_Action_Message_Record    *point;
    int return_value;

    /* First, keep track of who has control. */
    AI_Glob.Buy_Sell_Mort_Player = player;

    /* If no one has control, check if someone wants it. */
    if(player == RULE_NOBODY_PLAYER)
    {
        for(cur_player = 0; cur_player < UICurrentGameState.NumberOfPlayers; ++cur_player)
        {
            if(SlotIsALocalAIPlayer[cur_player] && UICurrentGameState.Players[cur_player].currentSquare != SQ_OFF_BOARD)
            {   /* This local player wants control? */
                if(AI_State[cur_player].contrl_msg_waiting && AI_State[cur_player].num_actions == 1 &&
                    AI_State[cur_player].control_message.wait_time_left == 0)
                {   /* Yes, then ask for it. */
                    AI_SendAction (ACTION_PLAYER_BUYSELLMORT, cur_player, RULE_BANK_PLAYER,
                                     0, 0, 0, 0,NULL, 0, NULL);
                    return; /* No need to process more requests. */
                }
            }
        }
    }
    else
    {
        /* Someone has control - one of our boys? */
        if(SlotIsALocalAIPlayer[player])
        {   /* Yes - send a message? */
            point = &AI_State[player].control_message;

            if(AI_State[player].contrl_msg_waiting)
            {   /* message waiting - time to send? */
                if(point->wait_time_left == 0)
                {   /* Then end it. */
                    return_value = MESS_SendAction(point->Action, point->FromPlayer,
                                                  point->ToPlayer, point->NumberA, point->NumberB,
                                                  point->NumberC, point->NumberD, point->StringA,
                                                  point->BinarySize, point->Blob);

                    if(!return_value)
                    {   /* we didn't send the action - que is full. try next turn. */
                        AI_State[player].sending_message.wait_time_left++;
                    }
                    else
                        AI_State[player].contrl_msg_waiting = FALSE;
                }
            }
            else if(!AI_State[player].num_actions)  /* Are we free to send another action? */
            {   /* No message waiting - but do we still want to do something? */
                if(!AI_Glob.AUCTION_IS_ON && !AI_State[player].paying_debt)
                {
                    /* Try to buy houses. */
                    if(return_value = AI_Should_Buy_House(&UICurrentGameState, player))
                    {
                        if(return_value != EB_LATER)
                        {
                            if(AI_Buy_Houses(&UICurrentGameState, player, AI_HOUSEOPS_NORMAL))
                                return; /* did something. Still want control. */
                        }
                    }

                    /* See if we should unmortgage property. */
                    if(!return_value)
                    {
                        if(AI_Should_Unmortgage_Property(&UICurrentGameState, player))
                        {
                            if(AI_Unmortgage_Property(&UICurrentGameState, player, FALSE, FALSE) != 0)
                                return;  /* Did something - still want control. */
                        }
                    }
                }

                /* Did not buy any more houses, nor mortgage.  Time to yield control */
                AI_SendAction (ACTION_PLAYER_DONE_BUYSELLMORT, player, RULE_BANK_PLAYER,
                                 0, 0, 0, 0,NULL, 0, NULL);
            }
        }
    }

return;
}

/********************************************************************************
 *                  AI_Process_Action_Completed                                 *
 *                                                                              *
 *   Input:  RULE_ActionArgumentsPointer NewMessage   NOTIFY_ACTION_COMPLETED   *
 *                          message as sent by Rules engine.                    *
 *                                                                              *
 *  This function processes the NOTIFY_ACTION_COMPLETED messages sent from the  *
 *  rules engine.  It keeps track of how many messages each AI sent, making sure*
 *  they don't flood the rules engine with duplicate messages.                  *
 ********************************************************************************/

void AI_Process_Action_Completed(RULE_ActionArgumentsPointer Message)
{
    if(Message->numberC >= RULE_MAX_PLAYERS)
        return;  /* Not a know player as we know it. */

        /* Keep track of when each player rolls their dice. Allows AIs to become
           impatient. */
    if(Message->numberA == ACTION_ROLL_DICE)
        AI_Glob.Last_Player_Asked_To_Roll = RULE_NOBODY_PLAYER;

    if(Message->numberA == ACTION_BUY_HOUSE)
        AI_Glob.place_building_phase = FALSE;

    if(SlotIsALocalAIPlayer [Message->numberC])
    {
        if(Message->numberA == NOTIFY_PLEASE_PAY)
        {   /* Finished collecting money. Reset collection flag. */
            AI_State[Message->numberC].paying_debt = FALSE;
            AI_State[Message->numberC].money_owed = 0;
            return;
        }

        if(--AI_State[Message->numberC].num_actions < 0)
            AI_State[Message->numberC].num_actions = 0; /* Should never happen */

        /* Allow other AIs to send if the action just completed
           was a trade trade editing done message. */
        if(Message->numberA == ACTION_TRADE_EDITING_DONE)
        {
            AI_PLAYER_SENDING_TRADE = AI_Send_Nothing;
            AI_SEND_TRADE_OFFER_TRADE = FALSE;
        }

        if(Message->numberA == ACTION_START_TRADE_EDITING)
        {   /* Did we successfully start the trade? If not,
               clear the AI_PLAYER_SENDING_TRADE flag. */
            if(Message->numberB == FALSE)
                AI_PLAYER_SENDING_TRADE = AI_Send_Nothing;
            else
                AI_PLAYER_SENDING_TRADE = AI_Send_Trade_Items;

            if(AI_Glob.Player_Just_Rejected_Countered == Message->numberC)
                AI_Glob.Trade_Just_Rejected_Countered = FALSE;
        }

        /* Is there an action we should be sending once done? */
        if(AI_State[Message->numberC].message_to_send.action != 0)
        {
            /* Sometimes there is a notification in the held message,
               which only the Rules engine should be sending.
               Bleeble: since I don't have time to figure out the AI code,
               ignore things that aren't actions. */

            if (AI_State[Message->numberC].message_to_send.action < NOTIFY_ERROR_MESSAGE)
            {
                MESS_SendActionMessage(&AI_State[Message->numberC].message_to_send);
            }
            memset(&AI_State[Message->numberC].message_to_send,0, sizeof(RULE_ActionArgumentsRecord));
        }

        /* Is this a bid message? Then allow an AI to bid now. */
        if(Message->numberA == ACTION_BID)
        {
            AI_Glob.PLAYER_CURRENTLY_BIDDING = FALSE;
            /* If playing in super-fast mode, send another bid immediately.*/
            if(RunFastWithNoDisplay)
            {
                if(!AI_Bid_In_Auction(&UICurrentGameState) && AI_Glob.AUCTION_IS_ON)
                {   /* No new high bid - wants us to kill the auction. */
                    MESS_SendAction(ACTION_KILL_AUCTION_CHEAT, RULE_NOBODY_PLAYER, RULE_BANK_PLAYER,
                                 0, 0, 0, 0,  NULL, 0, NULL);
                }
            }
        }

        /* Allow other AIs to accept/reject trades if just rejected/countered the trade. */
        if((Message->numberA == ACTION_TRADE_ACCEPT) &&
             AI_Glob.Trade_Just_Rejected_Countered)
        {
            if(AI_Glob.Player_Just_Rejected_Countered == Message->numberC)
                AI_Glob.Trade_Just_Rejected_Countered = FALSE;
        }

        /* If processing a message requiring control, make sure we reconsider the control
           business. */
        if((Message->numberA == ACTION_BUY_HOUSE && !AI_Glob.AUCTION_IS_ON) ||
            Message->numberA == ACTION_SELL_BUILDINGS || Message->numberA == ACTION_MORTGAGING)
        {
            if(AI_Glob.Buy_Sell_Mort_Player != RULE_NOBODY_PLAYER)
                AI_Resend_State(AI_Glob.Buy_Sell_Mort_Player);
        }

        if(Message->numberA == ACTION_PLAYER_BUYSELLMORT && Message->numberB == FALSE)
        {   /* We tried to get control, but did not succeed.  Cancel the action then. */
            if(AI_State[Message->numberC].contrl_msg_waiting)
            {
                AI_State[Message->numberC].contrl_msg_waiting = FALSE;
                if(AI_State[Message->numberC].num_actions)
                    AI_State[Message->numberC].num_actions--;
            }
        }
    }

return;
}

/*****************************************************************************
 * void AI_ProcessMessage                                                    *
 *                                                                           *
 *  Input:  RULE_ActionArgumentsPointer  NewMessage                          *
 *                  - Pointer to system message structure to be processed    *
 *                                                                           *
 * DESCRIPTION:                                                              *
 *      Processes game messages and sends AI player messages as needed.      *
 * Figures out the strategy for AI players and sends appropriate system      *
 * messages to IFACE_ProcessPlayersUI.                                       *
 *****************************************************************************/

void AI_ProcessMessage (RULE_ActionArgumentsPointer NewMessage)
{
    if(NewMessage == NULL)
        return;

    switch (NewMessage->action)
    {
        case NOTIFY_PLEASE_ROLL_DICE:
            if (SlotIsALocalAIPlayer [NewMessage->numberA])
            {

#if FORHOTSEAT
    #if _DEBUG
                if(ShowAIInfo && !ShowOnlyDrone)
                    AI_Show_AI_Info_Box();
    #endif
#endif
                if(!AI_State[NewMessage->numberA].num_actions)
                {   /* Player not busy. */
                    AI_Move_AI_Player((RULE_PlayerNumber) NewMessage->numberA);
                }
            }

            /* Keep track of when this player had been asked to roll dice first. */
            if(AI_Glob.Last_Player_Asked_To_Roll != NewMessage->numberA)
            {
                AI_Glob.Time_Last_Player_Asked_To_Roll = GetTickCount();
                AI_Glob.Last_Player_Asked_To_Roll = (RULE_PlayerNumber) NewMessage->numberA;
            }
            break;

        case NOTIFY_TEXT_CHAT:
            break;

        case NOTIFY_PLEASE_PAY:
            /* Ask a player to pay another -- say who owes who, and how much they need to raise */
            // Allow trading before someone goes bankrupt
            AI_Glob.GracePeriodForHumanActivityTickCount = 0;
            if (SlotIsALocalAIPlayer [NewMessage->numberA])
            {
                if(!AI_State[NewMessage->numberA].num_actions)
                {   /* Player not busy. */
                    AI_Pay_Debt((RULE_PlayerNumber) NewMessage->numberA, (RULE_PlayerNumber) NewMessage->numberB,
                                 NewMessage->numberC, NewMessage->numberD);
                }
            }
            break;

        case NOTIFY_BUY_OR_AUCTION_DECISION:
            if (SlotIsALocalAIPlayer [NewMessage->numberA])
            {
                if(!AI_State[NewMessage->numberA].num_actions)
                {   /* Player not busy. */
                    AI_Buy_Or_Auction((RULE_PlayerNumber) NewMessage->numberA, (RULE_SquareType) NewMessage->numberB,
                                       NewMessage->numberC);
                }
            }

            AI_Glob.Player_Purchasing_Property = (RULE_SquareType) NewMessage->numberA;
            AI_Glob.Purchasing_Property = NewMessage->numberB;
            break;

        case NOTIFY_FLAT_OR_FRACTION_TAX_DECISION:
            if (SlotIsALocalAIPlayer [NewMessage->numberA])
            {
                if(!AI_State[NewMessage->numberA].num_actions)
                {   /* Player not busy. */
                    AI_Choose_Tax_Decision((RULE_PlayerNumber) NewMessage->numberA);
                }
            }
            break;

        case NOTIFY_JAIL_EXIT_CHOICE:
            if (SlotIsALocalAIPlayer [NewMessage->numberA])
            {
                if(!AI_State[NewMessage->numberA].num_actions)
                {   /* Player not busy. */
                    AI_Choose_Exit_Choice((RULE_PlayerNumber) NewMessage->numberA,
                                          (BOOL) NewMessage->numberB,
                                          (BOOL) NewMessage->numberC,
                                          (BOOL) NewMessage->numberD);
                }
            }
            break;

        case NOTIFY_FREE_UNMORTGAGING:
            if (SlotIsALocalAIPlayer [NewMessage->numberA])
            {
                if(!AI_State[NewMessage->numberA].num_actions)
                {   /* Player not busy. */
                    AI_Choose_Unmortgage_For_Free((RULE_PlayerNumber) NewMessage->numberA,
                                                  (RULE_PropertySet) NewMessage->numberB);
                }
            }
            break;

        case NOTIFY_TRADE_ACCEPTANCE_DECISION:
            AI_Glob.Player_Proposed_Trade = AI_Glob.Player_Last_Edited;
            AI_Glob.Trade_offered_for_acceptance = TRUE;

            AI_Choose_Trade_Acceptance((RULE_PlayerSet) NewMessage->numberA);
            break;

        case NOTIFY_ACTION_COMPLETED:
            AI_Process_Action_Completed(NewMessage);
            break;

            /*  Do some AI calculations - see if they want to trade, etc. */
        case ACTION_TICK:
            AI_Process_AIs();
            break;

        case NOTIFY_TRADE_STARTED:
            AI_Reset_Trade_List();
            if(!AI_Glob.Trade_Started)
            {
                AI_Glob.Player_Proposed_Trade = (RULE_PlayerNumber) NewMessage->numberA;
                AI_Glob.Player_Last_Edited = (RULE_PlayerNumber) NewMessage->numberA;
                AI_Glob.Trade_Started = TRUE;
            }
            break;

        case NOTIFY_TRADE_EDITOR:

            if(AI_Glob.Trade_offered_for_acceptance)
            {
                AI_Player_Accepted_Rejected_Trade(AI_Glob.Player_Last_Edited,
                                                  (RULE_PlayerNumber) NewMessage->numberA, FALSE);
                AI_Glob.Trade_offered_for_acceptance = FALSE;
            }

            if(AI_Glob.Player_Last_Edited != NewMessage->numberA)
            {
                AI_Glob.Player_Proposed_Trade = AI_Glob.Player_Last_Edited;
                AI_Glob.Player_Last_Edited = (RULE_PlayerNumber) NewMessage->numberA;
            }

            if (SlotIsALocalAIPlayer [NewMessage->numberA])
            {
                /* Player not busy. */
                if(AI_PLAYER_SENDING_TRADE && (AI_PLAYER_SENDING_TRADE != AI_Send_Trade_Sent))
                {   /* In the middle of sending a trade - send it. */
                    AI_Send_Proposed_Trade(&UICurrentGameState, (RULE_PlayerNumber) NewMessage->numberA, FALSE);
                    break;
                }

                /* Not sending a trade - hence asked to edit it.  Consider
                   countering the trade. */
                if(!AI_PLAYER_SENDING_TRADE)
                {
                    if(!AI_Counter_Propose_Trade(&UICurrentGameState,(RULE_PlayerNumber) NewMessage->numberA, FALSE))
                    {   /* AI did not want to consider the trade. Cancel the trade. */
                        AI_SendAction (ACTION_TRADE_EDITING_DONE, (RULE_PlayerNumber) NewMessage->numberA,
                                        RULE_BANK_PLAYER, 2, 0, 0, 0,NULL, 0, NULL);
                        break;
                    }
                }
            }

            break;

        case NOTIFY_TRADE_ITEM:
            AI_Add_Trade_Item(NewMessage->numberC, NewMessage->numberD,
                               (RULE_PlayerNumber) NewMessage->numberA,(RULE_PlayerNumber) NewMessage->numberB,
                               (RULE_PropertySet) NewMessage->numberE);
            break;

        case NOTIFY_TRADE_FINISHED:
            AI_Glob.GracePeriodForHumanActivityTickCount = GetTickCount ();
            AI_Finish_Trade();
            break;

        case NOTIFY_PROPOSED_CONFIGURATION:
            AI_Send_Config_Accepts(NewMessage);
            break;

        case NOTIFY_END_TURN:
            if (SlotIsALocalAIPlayer [NewMessage->numberA])
            {
                if(!AI_State[NewMessage->numberA].num_actions)
                {   /* Player not busy. */
                    AI_Done_Turn_AI_Player((RULE_PlayerNumber) NewMessage->numberA);
                }
            }
            break;

        case NOTIFY_START_TURN:
            if(!UICurrentGameState.GameOptions.AITakesTimeToThink)
            {
                AI_Process_AIs();   /* AIs not stopping to think, hence process
                                       their commands inbetween turns here. */
            }

            AI_Process_AIs_At_Start_Turn();
            AI_Test_Turns_Taken[NewMessage->numberA]++;
            break;

        case NOTIFY_NUMBER_OF_PLAYERS:
                /* Is this a super-fast AI vs AI game? */
            if(AI_Drone_Game && AI_Drone_Configs.number_games_to_play)
            {
                if(NewMessage->numberA == 0 && AI_Test_Player_Just_Won)
                {
                    if(AI_Test_Player_Just_Won == 1)
                        AI_Test_Add_Players();
                    else
                        NumberOfLocalPlayers = AI_Drone_Configs.num_players_playing;   /* Hack to make bug work. */
                    AI_Test_Player_Just_Won++;
                }

                if((NewMessage->numberA == AI_Drone_Configs.num_players_playing)
                    && AI_Test_Player_Just_Won) /* Have we added the drone players we wanted? */
                {   /* Start the game then! */
                    MESS_SendAction (ACTION_START_GAME, 0 /* RULE_NOBODY_PLAYER */, RULE_BANK_PLAYER,
                                      0, 0, 0, 0,NULL, 0, NULL);
                    AI_Test_Player_Just_Won = 0;
                    AI_TEST_GAME_STARTING_TIME = GetTickCount();
                }
            }
            break;

        case NOTIFY_PICKED_UP_CARD:
            if (SlotIsALocalAIPlayer [NewMessage->numberA])
            {
                AI_Picked_Up_Card((RULE_PlayerNumber) NewMessage->numberA);
            }

            break;

        case NOTIFY_HOUSING_SHORTAGE:
            // If this is a GONE message, make sure we know we can now send buy requests again.
            // If not, make sure we know we can't send buy requests.
            if(NewMessage->numberD == 3)
                AI_Glob.housing_shortage = FALSE;
            else
            {
                AI_Glob.housing_shortage = TRUE;
                AI_Send_Housing_Shortage_Purchase_Requests((RULE_PlayerSet) NewMessage->numberE,
                                                            NewMessage->numberC, (RULE_PlayerNumber) NewMessage->numberA);
            }
            break;

        case NOTIFY_ERROR_MESSAGE:
            AI_Trap_Trade_Message(NewMessage);
            AI_Trap_Other_Errors(NewMessage);
            break;

        case NOTIFY_CLIENT_RESYNC_INFO:
            AI_Resync_Client();
            break;

        case NOTIFY_NEW_HIGH_BID:
            AI_Glob.AUCTION_IS_ON = TRUE;
            AI_Glob.AUCTION_HIGH_BID = NewMessage->numberB;
            AI_Glob.AUCTION_HIGH_BID_PLAYER = (RULE_PlayerNumber) NewMessage->numberA;
            AI_Glob.AUCTION_PROPERTY = (RULE_SquareType) NewMessage->numberC;

                /* If playing in fast mode, and this is the signal
                   that the auction just began, start it with a bid. */
            if(RunFastWithNoDisplay)
            {
                if(!AI_Bid_In_Auction(&UICurrentGameState))
                {   /* No new high bid - wants us to kill the auction. */
                        MESS_SendAction(ACTION_KILL_AUCTION_CHEAT, RULE_NOBODY_PLAYER, RULE_BANK_PLAYER,
                                     0, 0, 0, 0,  NULL, 0, NULL);
                }
            }
            break;

        case NOTIFY_AUCTION_GOING:
            if(NewMessage->numberD >= 3)
                AI_Glob.AUCTION_IS_ON = FALSE;

            // If this auction was caused by a housing shortage, make sure we know
            // the shortage question is over.
            AI_Glob.housing_shortage = FALSE;

            /* If playing in super-fast mode, get a bid in immediately. */
            if(RunFastWithNoDisplay)
            {
                if(!AI_Bid_In_Auction(&UICurrentGameState) && AI_Glob.AUCTION_IS_ON)
                {   /* No new high bid - wants us to kill the auction. */
                        MESS_SendAction(ACTION_KILL_AUCTION_CHEAT, RULE_NOBODY_PLAYER, RULE_BANK_PLAYER,
                                     0, 0, 0, 0,  NULL, 0, NULL);
                }
            }
            break;

        case NOTIFY_SQUARE_OWNERSHIP:
            /* Clear the purchase flags if someone was in the middle
               of buying a property. */
            if(AI_Glob.Player_Purchasing_Property != RULE_NOBODY_PLAYER)
            {
                if(NewMessage->numberA == AI_Glob.Purchasing_Property)
                    AI_Glob.Player_Purchasing_Property = RULE_NOBODY_PLAYER;
            }
            break;

        case NOTIFY_NAME_PLAYER:
            break;

        case NOTIFY_AI_NEED_PARAMETERS_FOR_SAVE:
            AI_Send_Save_Parameters((RULE_PlayerSet) NewMessage->numberA);
            break;

        case NOTIFY_AI_PARAMETERS:
            AI_Load_Save_Parameters(NewMessage);
            break;

        case NOTIFY_PLACE_BUILDING:
            // Make sure the AIs stop sending messages at this point.
            AI_Glob.place_building_phase = TRUE;
            if(SlotIsALocalAIPlayer[NewMessage->numberA] && !AI_State[NewMessage->numberA].num_actions)
                AI_Buy_Houses(&UICurrentGameState, (RULE_PlayerNumber) NewMessage->numberA,
                              AI_HOUSEOPS_AUCTION | AI_HOUSEOPS_PLACEMENT);
            break;

        case NOTIFY_PLAYER_BUYSELLMORT:
            if (NewMessage->numberA == RULE_NOBODY_PLAYER)
                AI_Glob.GracePeriodForHumanActivityTickCount = GetTickCount ();
            AI_Process_BuySellMort((RULE_PlayerNumber) NewMessage->numberA);
            break;

        case NOTIFY_DECOMPOSE_SALE:
            /* This particular player has exclusive control during decomposition,
            essentially the same as in buy/sell/mort mode, but he can only sell.
            A new BuySellMort notification will be sent after the decomposition is
            done (the player who sold the decomposed hotel is still in that mode)
            so setting AI_Glob.Buy_Sell_Mort_Player shouldn't be a problem.  Need
            this otherwise it thinks it has to get into buysellmort mode before it
            can sell.  AGMS19971007 */
            //1998 WARNING:  This code runs for any player, not just AI.  Andrew, whats wrong?
            AI_Glob.Buy_Sell_Mort_Player = (RULE_PlayerNumber) NewMessage->numberA;
            if(SlotIsALocalAIPlayer[NewMessage->numberA] && !AI_State[NewMessage->numberA].num_actions)
            {
              AI_Sell_Houses(&UICurrentGameState, (RULE_PlayerNumber) NewMessage->numberA,
                              AI_State[NewMessage->numberA].last_house_built_sold, FALSE);
            }
            break;

        case NOTIFY_GAME_OVER:
            AI_Player_Won((RULE_PlayerNumber) NewMessage->numberA);
            break;

        case NOTIFY_JUMP_TO_SQUARE:
            if(SlotIsALocalAIPlayer[NewMessage->numberC])
            {
                if(NewMessage->numberA == SQ_OFF_BOARD)
                {   /* AI player is out of the game - reset AI settings. */
                    memset(&AI_State[NewMessage->numberC], 0, sizeof(AI_Message_State_Record) );
                }
            }
            break;

        case NOTIFY_DICE_ROLLED:
        case NOTIFY_MOVE_FORWARDS:
        case NOTIFY_MOVE_BACKWARDS:
        case NOTIFY_CASH_AMOUNT:
        case NOTIFY_PUT_AWAY_CARD:
            break;

        case NOTIFY_SQUARE_MORTGAGE:
            break;

        case NOTIFY_SQUARE_HOUSES:
        case NOTIFY_JAIL_CARD_OWNERSHIP:

        default:
            break;
    }

}

/************************************************************************************/
/*                  AI_Send_Text_Message                                            */
/*                                                                                  */
/*   Input: wchar_T *Message          Message to send in wide char format.          */
/*          RULE_PlayerNumber player  Who is sending the message?                   */
/*          RULE_PlayerNumber target  To whom is the message being sent?            */
/*                                                                                  */
/*  This function sends a text message out to 'target' player using ACTION_TEXT_CHAT*/
/************************************************************************************/

void AI_Send_Text_Message(LANG_TextMessageNumber MessageID, RULE_PlayerNumber player, RULE_PlayerNumber target)
{
  BOOL              FoundHumanPlayer;
  int               length;
  RULE_CharHandle   MemoryHandle = NULL;
  wchar_t          *MemoryPntr = NULL;
  wchar_t           NewMessage[2000];
  RULE_PlayerNumber PlayerNo;
  
  /* Only send chat messages when there is at least one human player,
     otherwise the chat 'droid beeps incessantly and annoyingly. */

  FoundHumanPlayer = FALSE;
  for (PlayerNo = 0; PlayerNo < UICurrentGameState.NumberOfPlayers; PlayerNo++)
  {
    if (UICurrentGameState.Players[PlayerNo].AIPlayerLevel == 0)
      FoundHumanPlayer = TRUE;
  }
  if (!FoundHumanPlayer)
    return; /* No chat when no humans to listen to it. */

  /* Make a printable message, expand player numbers embedded in the message. */
  
  RULE_FormatErrorByArguments (MessageID, player, target, 0, 0,
    NewMessage, sizeof (NewMessage) / sizeof (wchar_t));
  
  /* Now send the message. */
  length = (wcslen (NewMessage) + 1) * sizeof(wchar_t);   /* each wchar_t is larger than 1 byte */
  MemoryHandle = RULE_AllocateHandle (length);
  
  if (MemoryHandle == NULL)
    return; /* Failed to allocate memory handle. */
  
  MemoryPntr = (wchar_t *) RULE_LockHandle(MemoryHandle);
  if (MemoryPntr == NULL)
  {
    RULE_FreeHandle (MemoryHandle);
    return; /* Failed. */
  }
  
  memcpy(MemoryPntr, NewMessage, length);
  
  /* Ok, send the blob to the rules engine for redistribution. */
  RULE_UnlockHandle (MemoryHandle);
  
  MESS_SendAction (ACTION_TEXT_CHAT, player, RULE_BANK_PLAYER,
    target, player, target, 0, NULL, 0, MemoryHandle);
  
  return;
}



/************************************************************************************/
/*                  AI_Send_Message                                                 */
/*                                                                                  */
/*   Input: AI_Message_Record message   Message # to send for the player.           */
/*          RULE_PlayerNumber player  Who is sending the message?                   */
/*          RULE_PlayerNumber to_player  To whom is the message being sent?         */
/*                                                                                  */
/*  This function sends a text message out to 'to_player'.  The message # is in     */
/*  message.  The actual message sent is based on the token of player.              */
/************************************************************************************/

void AI_Send_Message(AI_Message_Record message, RULE_PlayerNumber player, RULE_PlayerNumber to_player)
{
    LANG_TextMessageNumber  mess_number;
    int token;

    // DISABLED FUNCTION - Monopoly AI's don't use the chat box this way.
    return;

    if(SlotIsALocalAIPlayer[to_player])
        return; /* Don't send messages from AIs to AIs. */

    token = UICurrentGameState.Players[player].token;
    //if(token >= MAX_SW_TOKENS)
    //    token -= MAX_SW_TOKENS; /* Only 8 different tokens in SW, but 10 allowed. */
    while(token >= MAX_TOKENS)
        token -= MAX_TOKENS; /* assures valid values. */

    if(message >= AI_MESS_Cant_Suggest_Trade)
        mess_number = message - AI_MESS_Cant_Suggest_Trade + TMN_AI_START_SINGLE_MESSAGES
                    + TMN_AI_MESSAGE_TOKEN_SEPARATOR * token;
    else
    {
        mess_number = message * TMN_AI_MESSAGE_TYPE_SEPARATOR + TMN_AI_MESSAGE_START
                    + TMN_AI_MESSAGE_TOKEN_SEPARATOR * token;

        /* There are TMN_AI_MESSAGE_TYPE_SEPARATOR different messages for each message type. */
        mess_number += rand() / (RAND_MAX / TMN_AI_MESSAGE_TYPE_SEPARATOR);
    }

    AI_Send_Text_Message(mess_number, player, to_player);

return;
}



/*****************************************************************************
 * Toggles the AI Info dialog box.
 */

void AI_Toggle_AI_Info(BOOL only_drone_results)
{

#if FORHOTSEAT

    if (ShowAIInfo)
    {
        if(only_drone_results && !ShowOnlyDrone)
        {
            CheckMenuItem(hmenuMain, ID_OPTIONS_SHOW_AI_INFO, MF_UNCHECKED);
            CheckMenuItem(hmenuMain, ID_OPTIONS_SHOW_AI_DRONE_INFO, MF_CHECKED);
            ShowOnlyDrone = TRUE;
        }
        else if(!only_drone_results && ShowOnlyDrone)
        {
            CheckMenuItem(hmenuMain, ID_OPTIONS_SHOW_AI_INFO, MF_CHECKED);
            CheckMenuItem(hmenuMain, ID_OPTIONS_SHOW_AI_DRONE_INFO, MF_UNCHECKED);
            ShowOnlyDrone = FALSE;
        }
        else
        {
            ShowAIInfo = FALSE;
            CheckMenuItem(hmenuMain, ID_OPTIONS_SHOW_AI_INFO, MF_UNCHECKED);
            CheckMenuItem(hmenuMain, ID_OPTIONS_SHOW_AI_DRONE_INFO, MF_UNCHECKED);
            InvalidateRect (hwndMain, &MainBoardRect, FALSE);
        }
    }
    else
    {
        ShowAIInfo = TRUE;
        if(only_drone_results)
        {
            CheckMenuItem(hmenuMain, ID_OPTIONS_SHOW_AI_DRONE_INFO, MF_CHECKED);
        }
        else
            CheckMenuItem(hmenuMain, ID_OPTIONS_SHOW_AI_INFO, MF_CHECKED);

        ShowOnlyDrone = only_drone_results;
        InvalidateRect (hwndMain, &MainBoardRect, FALSE);
    }

    AI_Show_AI_Info_Box();

#endif

return;
}

/*****************************************************************************
 * Handle the AI message display dialog box.  Returns TRUE if the
 * message was processed, FALSE if not handled (opposite from windows).
 * Independent from the other dialog boxes.
 */

BOOL CALLBACK AIInfoBoxMsgProcessor (HWND hDlg, UINT message,
                                        WPARAM wParam, LPARAM lParam)
{

#if FORHOTSEAT

  if (message == WM_INITDIALOG)
    return TRUE; /* Nonzero to set default focus. */

  if (message == WM_CLOSE)
  {
    DestroyWindow (hDlg);
    return TRUE; /* Claim it was processed. */
  }

  if (message == WM_DESTROY)
  {
    hdlgAIInfoBox = NULL;
    ShowAIInfo = FALSE;
    return FALSE; /* Claim we didn't process it. */
  }

#endif

return FALSE; /* Allow furthur default message handling by the dialog box. */
}

/*****************************************************************************
 * Shows information about AIs in the AI dialog box.
 */

void AI_Show_AI_Info_Box(void)
{
#if FORHOTSEAT
    char     String [5000];
    char     *string_point;
    int      counter, cur_player, num_chars_wrote;


  /* Open the box, if needed. Close if not needed. */

    if(ShowAIInfo)
    {   /* Create the window. */
        if (hdlgAIInfoBox == NULL)
        {
            hdlgAIInfoBox = CreateDialog (hAppInstance,
            MAKEINTRESOURCE (IDD_AI_INFO), hwndMain,
            AIInfoBoxMsgProcessor);
            BringWindowToTop (hdlgAIInfoBox);
        }
    }
    else
    {   /* Kill the window. */
        if(hdlgAIInfoBox)
            DestroyWindow (hdlgAIInfoBox);

        return;
    }

    /* Set the message. */
    string_point = String;

    if(!ShowOnlyDrone)
    {
            /* For each AI, show the attitudes. */
        for(counter = 0; counter < UICurrentGameState.NumberOfPlayers; ++counter)
        {

            if (SlotIsALocalAIPlayer[counter])
            {   /* Show only local AIs. */
                num_chars_wrote = sprintf(string_point, "AI Player #%i Attitudes:\r\n",counter);
                string_point += num_chars_wrote;

                for(cur_player = 0; cur_player < UICurrentGameState.NumberOfPlayers; ++cur_player)
                {
                    num_chars_wrote = sprintf(string_point,"PL%i->%.2f   ",cur_player + 1, AI_Data[counter].player_attitude[cur_player]);
                    string_point += num_chars_wrote;
                }

                num_chars_wrote = sprintf(string_point, "\r\n");
                string_point += num_chars_wrote;
            }
        }
    }

    if(AI_Drone_Game)
    {
            /* Now add total wins for super fast mode. */
        num_chars_wrote = sprintf(string_point, "\r\nTotal wins (game %li/%li): \r\n",
                                                AI_Test_num_games_totalled - 1,
                                                AI_Test_num_games_totalled + AI_Drone_Configs.number_games_to_play - 1);
        string_point += num_chars_wrote;

        for(counter = 0; counter < RULE_MAX_PLAYERS; ++counter)
        {
            num_chars_wrote = sprintf(string_point, "PL%i: %i ", counter, TEST_total_wins[counter]);
            string_point += num_chars_wrote;
        }

        num_chars_wrote = sprintf(string_point, "\r\nDraws (monopolies): %i ", TEST_total_wins[RULE_MAX_PLAYERS]);
        string_point += num_chars_wrote;

        num_chars_wrote = sprintf(string_point, "\r\nDraws (without): %i ", TEST_total_wins[RULE_NOBODY_PLAYER]);
        string_point += num_chars_wrote;

        num_chars_wrote = sprintf(string_point, "\r\nAv. Time per Game (s): %lf ", AI_Test_Average_Time_Per_Game / 1000.0);
        string_point += num_chars_wrote;
        num_chars_wrote = sprintf(string_point, "\r\nAv. Turns per Game: %lf ", AI_Test_Average_Turns_Per_Game);
        string_point += num_chars_wrote;
    }
    else
    {   /* Add results from last trade. */
        num_chars_wrote = sprintf(string_point, "\r\nLast trade evaluation: ");
        string_point += num_chars_wrote;
        num_chars_wrote = sprintf(string_point, "\r\nchance delta: %f, worth delta: %f, prop import. delta: %f, evaluation: %f",
                                  AI_TEST_chances_change, AI_TEST_worth_change, AI_TEST_prop_importance, AI_TEST_evaluation);
        string_point += num_chars_wrote;
    }

  /* Display it. */

    if (hdlgAIInfoBox != NULL)
    {
        SetDlgItemText (hdlgAIInfoBox, IDC_AI_INFO_STRING, String);
    }

#endif

return;
}


/*****************************************************************************
 * Updates the total wins scores and resets the game for another run!
 */

void AI_Player_Won(RULE_PlayerNumber player)
{
    static FILE *stream = NULL;
    static BOOL tried_opening_file = FALSE;
    long total_worth;
    RULE_TokenKind token;
    DWORD   cur_time, time_change;
    AI_Monopoly_Stage   stage;
    RULE_PlayerNumber   cur_player;
#if !FORHOTSEAT
    char buffer[4000], *string_point;
    int  num_chars_wrote, counter;
#endif

        /* Restart only for a Drone game. */
    if(!AI_Drone_Game)
        return;

    if(AI_Test_Player_Just_Won)
        return; /* Already processed the message. */

        /* Is this the start of a drone series? */
//    if((player >= RULE_MAX_PLAYERS) && (UICurrentGameState.NumberOfPlayers == 0))
    if(AI_Test_num_games_totalled == 0)
    {   /* Yup, then just to some initialization. */
        if(AI_Drone_Configs.number_games_to_play == 0)
        {   /* finished all the drone games. */
#if _DEBUG
            LE_ERROR_ErrorMsg("Finished all drone games already. Restart program to try again.");
#endif
            return;
        }

        if(!AI_Test_Player_Just_Won)
        {
            /* Delete all ais from the game. */
            memset(SlotIsALocalAIPlayer, 0, sizeof(SlotIsALocalAIPlayer));
        }

        AI_Test_Player_Just_Won = 1;
        memset(AI_Test_Turns_Taken, 0, sizeof(AI_Test_Turns_Taken) );
        AI_Test_num_games_totalled = 1;

        if(UICurrentGameState.NumberOfPlayers != 0)
        {
            /* Reset the game and start again and send our new players! */
            MESS_SendAction (ACTION_NEW_GAME, UICurrentGameState.CurrentPlayer, RULE_BANK_PLAYER,
                             0, 0, 0, 0, NULL, 0, NULL);
        }
        else
        {
            /* Send NOTIFY_NUMBER_OF_PLAYERS message, which will start adding players.  */
            MESS_SendAction (NOTIFY_NUMBER_OF_PLAYERS, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
                             0, 0, 0, 0, NULL, 0, NULL);
        }
        return;
    }

    if(AI_Drone_Configs.number_games_to_play == 0)
    {   /* finished all the drone games. */
#if _DEBUG
        LE_ERROR_ErrorMsg("Finished all drone games already. Restart program to try again.");
#endif
        MESS_SendAction (ACTION_NEW_GAME, UICurrentGameState.CurrentPlayer, RULE_BANK_PLAYER,
                         0, 0, 0, 0, NULL, 0, NULL);
        return;
    }

    /* Open result file if can.*/
    if(!tried_opening_file || (stream != NULL))
    {
        if((stream = fopen(AI_DRONE_RESULT_FILE_NAME, "a")) == NULL)
            LE_ERROR_ErrorMsg ("Cannot open drone.res, the result file for drone game.");

        if(!tried_opening_file)
            fprintf(stream,"\n\nSeries of %li games started:\n",AI_Drone_Configs.number_games_to_play);
        tried_opening_file = TRUE;
    }

    token = UICurrentGameState.Players[player].token;

    cur_time = GetTickCount();
    time_change = cur_time - AI_TEST_GAME_STARTING_TIME;
    AI_Test_Average_Time_Per_Game = (AI_Test_Average_Time_Per_Game * (AI_Test_num_games_totalled - 1)
                                     + time_change) / (AI_Test_num_games_totalled);

    /* Is this a valid player? */
    if(player < RULE_MAX_PLAYERS)
    {   /* Yes, then increase total wins. */
        TEST_total_wins[token]++;

        /* Save the game results if can. */
        if(stream)
        {
            total_worth = AI_Get_Total_Worth(&UICurrentGameState, player);
            fprintf(stream,"\nPlayer %i won, after %li turns, worth %li cash, taking %li milliseconds.",
                           token, AI_Test_Turns_Taken[player], total_worth, time_change);

            AI_Test_Average_Turns_Per_Game = (AI_Test_Average_Turns_Per_Game * (AI_Test_num_games_totalled - 1)
                                            + AI_Test_Turns_Taken[player]) / (AI_Test_num_games_totalled);
        }
    }
    else if(UICurrentGameState.NumberOfPlayers != 0)
    {   /* Was it a draw, or game gone for too long? */

        /* Check what stage the game was at. */
        stage = AI_Get_Monopoly_Stage(&UICurrentGameState, player);
        if((stage == AI_STAGE_BUYING) || (stage == AI_STAGE_NO_MONOPOLIES))
        {   /* No monopolies!  No one must have wanted to trade. */
            if(stream)
                fprintf(stream,"\nDraw, with no monopolies owned taking ");
            TEST_total_wins[RULE_NOBODY_PLAYER]++;
        }
        else
        {   /* Monopolies owned - probably a draw. */
            if(stream)
                fprintf(stream,"\nDraw, with monopolies owned taking ");
            TEST_total_wins[RULE_MAX_PLAYERS]++;
        }

        /* Save the number of turns the game took - have to find a player still in the game. */
        if(stream)
        {
            for(cur_player = 0; cur_player < RULE_MAX_PLAYERS; ++cur_player)
            {
                if(UICurrentGameState.Players[cur_player].currentSquare != SQ_OFF_BOARD)
                {
                    fprintf(stream,"%li turns.",AI_Test_Turns_Taken[cur_player]);
                    break;
                }
            }
        }

        AI_Test_Average_Turns_Per_Game = (AI_Test_Average_Turns_Per_Game * (AI_Test_num_games_totalled - 1)
                                           + AI_Test_Turns_Taken[cur_player]) / (AI_Test_num_games_totalled);
    }

    AI_Test_num_games_totalled++;

    /* Reset the game and start again and send our new players! */
    MESS_SendAction (ACTION_NEW_GAME, UICurrentGameState.CurrentPlayer, RULE_BANK_PLAYER,
                     0, 0, 0, 0, NULL, 0, NULL);

    AI_Test_Player_Just_Won = 1;

    /* Delete all ais from the game. */
    memset(SlotIsALocalAIPlayer, 0, sizeof(SlotIsALocalAIPlayer));

    /* Subtract one from the games played and reset test variables. */
    if(AI_Drone_Configs.number_games_to_play)
        AI_Drone_Configs.number_games_to_play--;

    memset(AI_Test_Turns_Taken, 0, sizeof(AI_Test_Turns_Taken) );

    /* Is this the last game of the series, or a mulitple of 100? */
    if(!(AI_Drone_Configs.number_games_to_play % 100) ||  !AI_Drone_Configs.number_games_to_play)
    {   /* Yup, then save general results. */
        if(stream)
        {
            fprintf(stream, "\n\nFinished %li games.  Results:", AI_Test_num_games_totalled - 1);
            for(cur_player = 0; cur_player < AI_Drone_Configs.num_players_playing; ++cur_player)
            {
                 token = AI_Drone_Configs.players_playing[cur_player];
                 fprintf(stream,"\nPlayer %i won %li games, or %i%%.",token, TEST_total_wins[token],
                           (int) (TEST_total_wins[token] * 100 / (AI_Test_num_games_totalled - 1)));
            }

            fprintf(stream,"\n%li draws, where no monopolies were owned at end of game.",
                            TEST_total_wins[RULE_NOBODY_PLAYER]);
            fprintf(stream,"\n%li draws, where monopolies were owned at end of game.",
                            TEST_total_wins[RULE_MAX_PLAYERS]);

            fprintf(stream, "\n\nAverage turns per game (per player): %lf\nAverage time per game (s): %lf\n",
                    AI_Test_Average_Turns_Per_Game, AI_Test_Average_Time_Per_Game / 1000.0);
        }

#if _DEBUG
        if(!AI_Drone_Configs.number_games_to_play)
            LE_ERROR_ErrorMsg("Finished drone games.");
#endif
        if(!AI_Drone_Configs.number_games_to_play)
            AI_Test_Player_Just_Won = 0;
    }

    /* We are done with the file. Close it. */
    if(stream)
        fclose(stream);

    /* Update info box if needed. */

#if FORHOTSEAT
    if(ShowAIInfo)
        AI_Show_AI_Info_Box();
#else
    string_point = buffer;
    memset(buffer, 0, sizeof(buffer));

        /* add total wins for super fast mode. */
    num_chars_wrote = sprintf(string_point, "\r\nTotal wins (game %li/%li): \r\n",
                                            AI_Test_num_games_totalled - 1,
                                            AI_Test_num_games_totalled + AI_Drone_Configs.number_games_to_play - 1);
    string_point += num_chars_wrote;

    for(counter = 0; counter < RULE_MAX_PLAYERS; ++counter)
    {
        num_chars_wrote = sprintf(string_point, "PL%i: %i ", counter, TEST_total_wins[counter]);
        string_point += num_chars_wrote;
    }

    num_chars_wrote = sprintf(string_point, "\r\nDraws (monopolies): %i ", TEST_total_wins[RULE_MAX_PLAYERS]);
    string_point += num_chars_wrote;

    num_chars_wrote = sprintf(string_point, "\r\nDraws (without): %i ", TEST_total_wins[RULE_NOBODY_PLAYER]);
    string_point += num_chars_wrote;

    num_chars_wrote = sprintf(string_point, "\r\nAv. Time per Game (s): %lf ", AI_Test_Average_Time_Per_Game / 1000.0);
    string_point += num_chars_wrote;
    num_chars_wrote = sprintf(string_point, "\r\nAv. Turns per Game: %lf ", AI_Test_Average_Turns_Per_Game);
    string_point += num_chars_wrote;

    MessageBox(NULL, buffer, "AI Drone Results", MB_OK);
#endif

return;
}

/*****************************************************************************
 * Adds a bunch of test players for the Hotseat demo.
 */

void AI_Test_Add_Players(void)
{
    RULE_PlayerColour Colour;
    wchar_t       *TempWString;
    RULE_TokenKind     Token;
    int           counter, cur_count;
    static int      start_player = 0;

    /* Load a new set of players to take on the challenge. */
    cur_count = start_player;
    for(counter = 0; counter < AI_Drone_Configs.num_players_playing; ++counter)
    {
        /* Get the token for this player. */
        Colour = Token = AI_Drone_Configs.players_playing[cur_count];

        /* Get the player name corresponding to this token. */
        TempWString = LANG_GetTextMessage (ATR_TOKEN_NAME_0 + Token);
        wcscpy(LocalPlayerNames[NumberOfLocalPlayers++], TempWString);

        /* Send the message */
        MESS_SendAction (ACTION_NAME_PLAYER,RULE_SPECTATOR_PLAYER /* my id */, RULE_BANK_PLAYER,
        RULE_NOBODY_PLAYER /* Requested ID */, 3, Token, Colour,
        TempWString, 0, NULL);

        /* Increase the player counter. */
        cur_count++;
        if(cur_count >= AI_Drone_Configs.num_players_playing)
            cur_count = 0;
    }

    if(AI_Drone_Configs.cycle_players)
    {
        if(++start_player >= AI_Drone_Configs.num_players_playing)
            start_player = 0;
    }

return;
}


