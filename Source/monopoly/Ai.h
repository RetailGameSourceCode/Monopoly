#ifndef __AI_H__
#define __AI_H__

/*************************************************************
*
*   FILE:             AI.H
*
*   (C) Copyright 97 Artech Digital Entertainments.
*                    All rights reserved.
*
*   Monopoly Game AI Header.
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/

/* The following disables trades and auctions for monopoly
   version of the game. */
#if !FORHOTSEAT
//	#define AI_NO_TRADES
//	#define AI_NO_AUCTIONS
#endif

/*   Different tactics for how much cash to keep on hand.  Cash left on hand
     will never be less than min_cash_on_hand.  */

enum AI_Cash_On_Hand_StrategyEnum
{
		/*  Always try to keep minimum amount, no matter what
		    is going on.  */
	AI_CASH_MINIMUM_AMOUNT = 0,
		/*  Always keep enough for expected rent, taking into account
		    rent from housed monopolies.  */
	AI_CASH_EXPECTED_RENT_INC_MONOPOLIES,
		/*  Always keep enough for expected rent, but forget about
		    rent from housed monopolies. */ 
	AI_CASH_EXPECTED_RENT_NOT_INC_MONOPOLIES,
		/*  Will keep enough cash for expenses without monopolies plus
		    the highest rent on a housed monopoly. */
	AI_CASH_HIGHEST_RENT,
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

	AI_CASH_MONOPOLY_DEPENDENT,

	AI_CASH_NUMBER_STRATEGIES

};
typedef int AI_Cash_On_Hand_StrategyRecord;


enum AI_Housing_Purchase_StrategyEnum
{
		/* Normal housing purchase strategy. */
	AI_HOUSE_NORMAL = 0,
		/* When buying houses, do not purchase unless lot will have at least
		   3 houses when finished buying. */
	AI_HOUSE_BUY_ATLEAST_3 = 1 << 0,
		/* Purchase houses only if someone is within 12 spaces of the 
		   property. */
	AI_HOUSE_BUY_WITHIN_12 = 1 << 1
};
typedef int AI_Housing_Purchase_Strategy;


	/* The following enum is used to quantify the importance of
	   a property. */
enum AI_Property_ImportanceEnum
{
		/* Not important = not one of the following 2. */
	AI_PROPERTY_NOT_IMPORTANT = 0,
		/* Property gives a player a direct monopoly. */
	AI_PROPERTY_GIVES_DIRECT_MONOPOLY = 1,	
		/* Property allows some players to trade for a monopoly
		   between themselves and they couldn't do it without it. */
	AI_PROPERTY_ALLOWS_TRADE,
		/* Property is a utility or railroad. */
	AI_PROPERTY_IS_CASH_COW,

		/* This is used only for an auction - not returned by any functions. */
	AI_PROPERTY_IS_A_HOUSE,

		/* This is used only for an auction - not returned by any functions. */
	AI_PROPERTY_IS_A_HOTEL

};
typedef int AI_Property_Importance;

 
enum AI_House_OptionsEnum
{
	AI_HOUSEOPS_NORMAL = 0,
	AI_HOUSEOPS_HOTELS_ONLY = 1,
	AI_HOUSEOPS_HOUSES_ONLY = 1 << 1,
    AI_HOUSEOPS_AUCTION = 1 << 2,
    AI_HOUSEOPS_PLACEMENT = 1 << 3
};
typedef int AI_House_Options;

/* The following classification specifies what kind of property to get/give
   in a trade. */
enum AI_Property_Classification_Enum
{
	AI_Property_None = 0,  /* Don't give anything */
	AI_Property_1 = 1,	  /* Give 1 property. */
	AI_Property_2,		  /* Give 2 properties. */
	AI_Property_3,		  /* ... */
	AI_Property_4,
	AI_Property_5,
	AI_Property_6,
	AI_Property_Worst = 10,	  /* Give your worst. */
	AI_Property_Best   		  /*  Give your best property. */

};
typedef int AI_Property_Classification;

	/* The following structure is used to determine what the AI should give
	   to a player if it got a monopoly from that player as part of a 
	   trade.  What the AI gives is determined by how highly the AI
	   sees the player. */
typedef struct AI_Monopoly_Level_Trading_GenerosityStruct
{
		/* How good a monopoly should I give?*/
	AI_Property_Classification		give_monopoly;
		/* how many properties that allow a group trade? */
	AI_Property_Classification		give_group_trades;
		/* How many cash cows should I give? */
	AI_Property_Classification		give_cash_cows;
		/* How many junk properties should I give as well? */
	AI_Property_Classification		give_junk;
		/* How  much cash should the player get/give from optimal
           amount? */
	double							cash_multiplier;
}  AI_Monopoly_Level_Trading_Generosity_Record;


#define AI_MAX_MONOPOLY_LOTS	3		/* What's the largest # of properties in monopoly? */

#define AI_MAX_TRADES		  20

#define AI_WHAT_TO_TRADE_ENTRIES		20		/* How many entries .what_to_trade section
    											   of AI_Data. */
#define AI_MAX_RAILROADS                4       /* How many railroads are there? */
#define AI_MAX_UTILITIES                2       /* How many utilities are there? */
#define AI_MAX_CASH_COWS				6		/* number of rails + utils. */

typedef struct AI_DataStruct
{
		/* How does the computer 'feel' towards a player? Determines
		   likeliness of trades and how much the computer expects
		   or is willing to give up from the player.  Every time a trade
		   is rejected, especially if a monopoly forming trade, will lower attiude.
		   Is a number between -1 and 1 representing the likelihood of trading (1 = 100%,
		   -1 = 0%.).  Also determines how good a trade we propose.  If attitude is
		   low, we are going to make more concessions ('cause player is unlikely to
	       trade) then if a player trades often. */
	float	player_attitude[RULE_MAX_PLAYERS];
		/* The following number (suggested to be 0.2), represents the 
		   attitude lost/gained towards opponent when a trade proposed by
		   AI is rejected or accepted. */
	float	attitude_lost_for_rejected_trade;
		/* The following number indicates the change of attitude per evaluation
		   point when evaluating trades it did not propose or is simply observing 
		   Suggested to be about 0.002. */
	float   attitude_change_trade_observing;
		/* The following is a number that will be added/subtracted the each player_attitude
		   every ACTION_TICK. (approx once every second) */
	float	attitude_tick_change;
		/*  This indicates what attitude the AI will towards a player after a long
		    time.  If the current attitude is above this level, the AI's attitude will
			lower slowly over time.  If the attitude is below this level, the AI's 
			attitude will increase over time. */
	float   player_neutral_attitude;
		/*  Holds time of last trade.  If all slots are filled, no more trades
		    will be proposed.  Each turn the counters decrease.   0 = free slot.
			Each trade fills a counter with time_forget_trade.   */
	long	time_last_trade[AI_MAX_TRADES];
	long	time_forget_trade;
		/*  At most how many trades should an AI be allowed to make before having
		    to wait for trades to expire from time_forget_trade to 0? Cannot be
			more than 20 (AI_MAX_TRADES). */
	long	max_trades;
		/*  How many times will the AI counter a trade during negotiations? */
	int     trade_counter_limit;
		/*  What is the probability of the AI countering a proposed trade? */
	float	trade_counter_prob;
		/*  Minimum amount of liquid assets (besides housed monopolies) to have on hand. */
		/*  Will not buy anything if won't be left with this much cash.  However, if     */
		/*  calculated expenses are greater than this number, will try to keep      */
		/*  more.  */
	long	min_cash_on_hand;	
		/*  What strategy we should use on keeping cash? */
	AI_Cash_On_Hand_StrategyRecord	cash_strategy;
		/*  How should be purchase houses?  Or AI_Housing_Purchase_Strategy 
		    qualifiers.  */
	long	housing_purchase_strat;  

		/*  A positive number representing how important current liquid assets
		    are in evaluating our position.  Importance of cash is divided into
			4 different stages of the game. */
			/* During AI_STAGE_BUYING.  Probably should be set around 0.05. (ie. $20
			   in liquid assets give us as much of a chance in winning as $1 in
			   revenue. */
	float	buying_stage_cash_multiplier;

			/* During AI_STAGE_NO_MONOPOLIES.  Cash losses some importance, and revenue
			   becomes most important. Probably around 0.03 or 0.025. */
	float  no_mono_stage_cash_multiplier;

			/* The following is the number x in 1.0 / x^(liquid_assets) when figuring out
			   how much is cash worth taking into account that we have a lot of it or
			   just a bit. */
	float  cash_liquid_assets_dependance;

			/* During AI_STAGE_MONOPOLIES_NOT_OWNED_ONE.  Liquid cash losses all importance.
			   Most important is getting that monopoly. Probably around 0.01 is good. */
	float mono_not_owned_stage_cash_multiplier;

			/* During AI_STAGE_MONOPOLIES_OWN_ONE.  Suddenly cash becomes important to
			   survive a hit on opponent's monopoly & buying houses.  Probably around 0.1.  */
	float mono_stage_cash_multiplier;
			/* This number indicates how important the chances of success are
			   for evaluating position.  Should be around 100. */
	float chances_factor;
			/* This number indicates at most how much we will accept our chances of
			   success to drop for other benefits. Should be around 15? */
	float chances_threshold;
			/*  This number indicates how important total assets is in evaluating
			    current position.  Should be around 0.01. */
	float cash_factor;
			/*  These numbers indicate how many times the property values we value each 
				monopoly.  This is part of cash_factor. */
	float property_factors[SG_PARK_PLACE + 1];
			/*  These numbers indicate how many times the property values we value each
			    cash cow based on the number we have.  First 4 numbers are for 1,2,3,4 railroads,
				and the next two are for 1,2 utils. */
	float cash_cow_factors[AI_MAX_CASH_COWS];
			/*  This number indicates how important is the fact that we got a monopoly,
			    gave a monopoly, or traded properties that allow trades. Should be around
				?????? */
	float trade_importance_factor;
			/*  This number indicates what is the minimum evaluation value we should get
			    for which we will accept trades. Have no idea. Perhaps 0 
				is the right number. */
	float min_evaluation_threshold;
            /*  When the AI is 'giving' away a monopoly for cash to improve it's standings
                against the other players, what is the minimum evaluation for it to be
                acceptable for the AI to 'risk' giving a monopoly to a player who would
                not otherwise get one? */
    float min_give_monopoly_evaluation;
			/*  This number indicates how important a factor is the fact that
			    we received our first monopoly.  Should be around 20. */
	float monopoly_received_importance;
            /*  This number indicates how important a factor is the fact that
                we gave a monopoly for cash, without getting one back.
				This case is used only after a monopoly trade, when the AI player
				is willing to give away a monopoly for a lot of cash, improving
				his odds of winning. */
    float giving_monopoly_importance;
			/*  This number indicates how important a factor is the fact that
			    we received the Baltic monopoly.  */
	float baltic_received_importance;
			/*  This number indicates how important a factor is the fact that 
				we received/gave a property that allows a group trade for monopolies. 
				Should be around 10? */
	float property_allow_trade_importance;
			/*  This number indicates how important a factor is the fact that 
				we received/gave a property that allows a group trade for a monopoly,
                even though player already has a group trade possibility. */
    float property_allow_more_trade_importance;
			/* The following indicates how important is a property if the
			   player owns all the squares of the monopoly except for one,
			   and that one is unowned. */
	float property_one_unowned_importance;
			/* The following indicates how important is a property if all other
			   squares in the monopoly are unowned. */
	float property_two_unowned_importance;
            /* Importance factors for railroads, depending on how many we have. */
    float railroad_importance[AI_MAX_RAILROADS];
            /* Importance factors for utilities, depending on how many we have. */
    float utility_importance[AI_MAX_UTILITIES];
            /* The following holds a list of importance values for the number of
               monopolies for which the AI holds a veto.  Ie.  it is strategically
               beneficial to contain at least one property from every (or almost
               every) monopoly.  This ensures we are included in any monopoly trade,
               as well as allowing us to block any monopoly trades harmful to us. 
               A list of 9 values - where the player owns from 0 - 8 monopolies. */
    float monopoly_veto_importance[SG_PARK_PLACE + 2];
            /* The following multiplier is used in evaluating property importance
               changes in a trade.  If the player we are trading is losing
               property importance points in the trade (for instance, is
               giving up a cash cow for money), then we multiply the importance
               change by this factor.  Basically, we want people to pay for
               important properties, but we don't want to pay for getting them. 
               This multiplier is not applied to property importance arrived from
               monopolies. */
    float negative_prop_importance_change_multiplier;
			/*  This number indicates the probability of the AI proposing a trade
			    every TICK.  Hence should be around 0.001 or something of the sort. */
	float propose_trade_probability;
			/*  This number indicates the probability of the AI proposing a trade
			    when it decides it desperately needs a monopoly.  */
	float monopoly_propose_trade_probability;
            /* This number indicates the probability of an AI trying to give another
               player a monopoly for cash.  This ocurrs only if the AI owns a monopoly,
               there are other players with monopolies, and the AI does not have at 
               least a minimum number of houses. */
    float propose_monopoly_give_away_probability;
            /*  This number indicates the maximum number of houses the
                AI should have per square on all owned monopolies and still
                consider giving away a monopoly for cash. */
    int   max_houses_per_square_for_give_away;
			/* The following two numbers dictate how long an AI will pause,
			   to simulate thinking, before doing any action.  The numbers are
			   in seconds. */
	int	  min_action_wait;
	int	  max_action_wait;
			/* How long should an AI wait before accepting/rejecting/countering
			   a trade, so that other players have a chance to understand it? */
	int   wait_for_trade_consider;
			/* This number is how long, in milliseconds, will the AI wait before
			   becoming impatient with a player taking too long to roll the dice.
			   After this time, will send a message trying to coax the player to move. */
	unsigned long  patience;
            /* How many trades from the same player on the same property will the AI 
               consider before getting fed up?  After reaching this maximum, the AI
               will be fed up with any trade containing this property coming from that
               player.  This includes properties given as well as received in the trade.
               Can be between 0 - 3, where 0 means the AI will never get fed up. */
    int   number_times_allow_property_trade;
            /* After how many turns (its own or anyone elses) will the AI forget
               that a property has been traded once?  1 - 3 should be right. */
    int   turns_to_forgetting_property_trade;
			/* After calculating the property importance deficit (ie. fact we
			   gave/got a monopoly, or a property that allows a trade), what is 
			   the lowest value so that we will consider counter proposing a trade?
			   Numbers between -15 - 5 are fine. */
	float lowest_prop_importance_for_counter;
            /* When fed up with a player proposing trades with the same properties, 
               what is the minimum evaluation for the AI to accept the trade?
               Perhaps 0.5 - 1.0 is the right value. */
    float min_evaluation_if_fed_up;
            /* How far are we willing to go to stop someone from getting a monopoly
               in an auction?  Depends also on whether we own a monopoly, and
               whether anyone else owns a monopoly.*/
    float monopoly_suicide_factor;
		/* When using WHAT TO TRADE (see below) to figure out how much cash to
		   offer or ask for depending on our evaluation of the player, what is the
		   lowest level to use when proposing a non-monopoly trade? (ie. what is
		   the most we are willing to sacrifice if the trade is not that important?) */
	float minimum_nonmonopoly_trade_attitude;
			/* The following lists what the AI will trade for a monopoly, and how much
			   cash the AI will offer.  The definition of the structure and defines
			   are in ai_trade.h. */
	AI_Monopoly_Level_Trading_Generosity_Record  what_to_trade[AI_WHAT_TO_TRADE_ENTRIES];

} AI_Data_Record;


	/* The following holds the arguments passed to AI_SendAction (which is a copy
	   of MESS_SendAction) for later use.  For a description of the arguments, see
	   MESS_SendAction. */

typedef struct AI_Action_MessageStruct
{
	RULE_ActionType Action;
	RULE_PlayerNumber FromPlayer;
	RULE_PlayerNumber ToPlayer;
	long NumberA;
	long NumberB;
	long NumberC;
	long NumberD;
	wchar_t *StringA;
	unsigned int BinarySize;
	RULE_CharHandle Blob;

		/* This is new - keeps track of how many seconds are left
		   before the message should be sent. */
	int	 wait_time_left;
} AI_Action_Message_Record;

	/* The following array holds information about what messages
	   each AI is currently sending, what they can send, and
	   other relate stuff. */
typedef struct AI_Message_StateStruct
{
		/*  A counter of how many actions an AI player is doing. Everytime
            AI does something (Ie. buy house), this counter is incremented.
			Right now, only one action per AI player is allowed.  When the
			action is finished, counter is decremented.   */
	int num_actions;

		/* What message to send when finished sending current action? */
	RULE_ActionArgumentsRecord	message_to_send;

	/* Holds the MESS_SendAction message to send an AI just
	   processed.  It is used to delay the sending of the action to simulate
	   human thinking/create a pause. */
	AI_Action_Message_Record   sending_message;

    /* What message to send when we get control? */
    AI_Action_Message_Record   control_message;

    /* Tis keeps track of whether there is a message waiting in control_message. */
    BOOL contrl_msg_waiting;

	/* The following keeps track of whether the AI is currently paying a
	   debt.  Prevents from unmortgaging properties or spending the money. */
	BOOL paying_debt;

	/* The following keeps track of when last the AI sent a message. This is used
	   to determine when to send random or 'impatient' messages to the current player. */
	DWORD last_sent_text_message;

	/* The following keeps track of how many times we have countered
	   the current trade. */
	int	 times_countered_trade;
	
	/* When countering a trade, we need to know whether consecutive offers
	   are getting better. */
	float last_trade_evaluation;

    /* The following keeps track of how many times each player has asked
       the AI about a property, so it can get annoyed if someone keeps asking
       it about the same property over and over.  Two arrays - each one holds
       one bit for each property, so a total of two bits for each property. */
    RULE_PropertySet    properties_traded_bit_1[RULE_MAX_PLAYERS];
    RULE_PropertySet    properties_traded_bit_2[RULE_MAX_PLAYERS];

    /* This holds how many turns passed since we forgot a trade. */
    int turns_after_forgetting_last;

    /* The following hold the last square an ACTION_BUY_HOUSE/ACTION_SELL_HOUSE
       message was sent to.  This is used to know where to place the house/hotel 
       we bought after a housing auction. ie: NOTIFY_PLACE_BUILDING, and to 
       remember which monopoly we were decomposing hotels from. */
    RULE_SquareType  last_house_built_sold;

    /* The following variable keeps track of how much money is owed by the
       AI.*/
    long money_owed;
} AI_Message_State_Record;

/* Following is an extension of BOOL, EBOOL.  Has 3 states, where the third
   is used to postpone actions for a later time.  It is used by routines like
   AI_Should_Buy_House where the third option signifies that money should
   be reserved for buying houses, but don't buy at this momement. */
enum EBOOLEnum
{
	EB_FALSE = 0,
	EB_TRUE,
	EB_LATER
};		
typedef int EBOOL;

/*  The following is a list of messsages each character can send. */
enum AI_MessageEnum
{
    /* These are messages that have more than 1 different responses. */
	AI_MESS_Counter = 0,
	AI_MESS_Reject,
	AI_MESS_Impatient,
	AI_MESS_Offer_Monopoly_Trade,
	AI_MESS_Too_Many_Counters,
	AI_MESS_Not_Serious_Trader,
    AI_MESS_Annoyed_With_Trader,

    /* Any messages that have only 1 response should be placed after Cant_Suggest_Trade */
    AI_MESS_Cant_Suggest_Trade,
    AI_MESS_Not_Involved_In_Trade,
    AI_MESS_Ask_Me_Later
};
typedef int AI_Message_Record;


/* this arrays holds general global information about the state of the 
   game. */
typedef struct AI_GlobalStruct
{
    /* The following keeps track of who started the trade and who is editing it. */
    RULE_PlayerNumber Player_Proposed_Trade;
    RULE_PlayerNumber Player_Last_Edited;

	/* This tells us if the trade just started is a new trade, or someone
	   countering a trade. */
	BOOL    Trade_Started;
	BOOL	Trade_offered_for_acceptance;

    /* This keeps track of whether a trade is currently being rejected or
       edited.  Prevents AIs from sending more trade accepts/rejects. */
    BOOL    Trade_Just_Rejected_Countered;
    RULE_PlayerNumber   Player_Just_Rejected_Countered;

    /* The following hold information about whether an auction is taking place,
       and the stats of the auction. */
    BOOL	AUCTION_IS_ON;
    long	AUCTION_HIGH_BID;
    RULE_PlayerNumber AUCTION_HIGH_BID_PLAYER;
    RULE_SquareType AUCTION_PROPERTY;
    BOOL	PLAYER_CURRENTLY_BIDDING;

    /* The following two keep track of whether a player is in the middle
       of buying a property. */
    RULE_PlayerNumber Player_Purchasing_Property;
    RULE_SquareType Purchasing_Property;

    /* The following is used to determine how long the current player has sitting
       around doing nothing after a roll dice message. */
    RULE_PlayerNumber	Last_Player_Asked_To_Roll;
    DWORD				Time_Last_Player_Asked_To_Roll;

    /* A similar grace period counter that marks the time when the trade
       finished or someone exited from buy/sell/mortgage mode or other similar
       activity.  It is used to make the AI wait before buying houses,
       proposing trades, etc, otherwise the Humans can't get access at all. */
    DWORD GracePeriodForHumanActivityTickCount;

    /* The following keeps track of which player currently has control for
       mortgaging/buying/selling. */
    RULE_PlayerNumber   Buy_Sell_Mort_Player;

    /* The following tells us whether an AI is currently considering
       an unmortgage for free choice.  We do not need control in this situation,
       and hence should not ask for it. */
    BOOL  Player_Unmortgaging_For_Free;

	/* The following tells us if the rules engine is asking whether someone
	   wants to start an auction because of a housing shortage. */
	BOOL housing_shortage;

	/* This variable keeps track of whether a player is currently placing
	   a building after a house auction.  Can't do anything until this
	   player is done. */
	BOOL place_building_phase;

} AI_Global_Record;

/* The following is a list of configurations used for an AI drone game
   (ie. almost no graphics, just AIs plugging away as fast as possible.) */
typedef struct AI_Drone_ConfigsStruct
{
    /* List of player tokens playing. */
    RULE_PlayerNumber  players_playing[RULE_MAX_PLAYERS];
    /* how many in the list? */
    int     num_players_playing;
    /* Should we cycle the player order every game? */
    BOOL    cycle_players;
    /* What is the longest a game should take? */
    long    max_game_time;
    /* How many games are we playing? */
    long    number_games_to_play;
} AI_Drone_Configs_Record;

/*  The following holds a RIFF chunk for saving AI parameters. */
typedef struct AI_RIFF_ChunkStruct
{
    char    identifier[4];
    DWORD   length;
    AI_Data_Record   ai_data;
} AI_RIFF_Chunk_Record;

/*  The following holds the start and end squares of a monopoly.
    Used for speeding up searches for monopoly squares. */
typedef struct AI_Group_Start_EndStruct
{
    RULE_SquareType start_square;
    RULE_SquareType end_square;
} AI_Group_Start_End_Record;


#define  AI_DRONE_FILE_NAME                 L"drone.cfg" /* What filename to use for configs for
                                                            an AI drone game? */
#define  AI_DRONE_RESULT_FILE_NAME          "drone.res"  /* Where to store results of a drone
                                                            game. */
#define  AI_CRITICAL_HOUSING_LEVEL    3    /* How many houses should remain available
                                              before we change our strategy to refelct
											  it?  Right now a define. Might be included
											  in AI_DATA.   */
#define  AI_RENT_RECEIVED_FOR_GO	  200

#define  AI_MAX_DARZINSKIS_CHART	 5000	/* The maximum liquid assets for which we 
											   entered Darzinskis' monopoly success rate
											   vs liquid assets chart. */

#define  AI_DARZINSKIS_CHART_SEPARATION  250	/* The chart is entered in increments of
											       this much liquid assets. */
#define  AI_JAIL_CARD_VALUE			49		/* How much cash is the jail card worth? Less
											   than 50 because 50 cash is more versatile
											   than jail card. */

	/* This defines how many unowned squares should there be for the
	   computer to change its strategy to a static non-monopoly
	   stage. */
#define  AI_CRITICAL_UNOWNED_SQUARES	2

#define	 AI_MAYBE						2	   /* Used in BOOL type functions as an alternative. */

#define  AI_MAX_STACK_MESSAGES			(RULE_MAX_PLAYERS * 3 + 3 * SQ_MAX_SQUARE_TYPES)
										/* How large a stack of AI messages to store?
										   As in define in MESS.C. */
		
#define	 AI_MAX_SQUARES_OWNABLE			28	/* How many properties can be bought? */

#define  AI_MAX_CHANGE_BID				50  /* Most an AI will increase a bid by. */

#define  AI_MAX_LEVELS					3   /* How many levels of play are there? */

#define  AI_NUMBER_MONOPOLY_SQUARES     22  /* Number of properties belonging to a monopoly. */

     /* The following functions are used to find the start and end of a monopoly
        group, using group as a source. */
#define  AI_Group_Start(x)              (AI_Group_Start_End[x].start_square)
#define  AI_Group_End(x)                (AI_Group_Start_End[x].end_square + 1)

#define  AI_RIFF_IDENTIFIER_LENGTH      4   /* Size of identifier part of a RIFF chunk. */
#define  AI_RIFF_SIZE_LENGTH            4   /* How many bytes is a RIFF length specifier? */
#define  AI_RIFF_SAVE_IDENTIFIER        "AISV"   /* Riff identifier for an AI save block. */

/************************************************************/

/************************************************************/
/* Extern variables											*/
/************************************************************/
extern BOOL AI_Drone_Game;


/************************************************************/
/* PROTOTYPES                                               */
/************************************************************/
extern BOOL AI_InitializeSystem(void);
extern void AI_Initialize_Global(void);
extern void AI_Initialize_Test(void);
extern void AI_CleanAndRemoveSystem(void);
extern void AI_Reset_Player_Records(void);
extern void AI_Choose_Unmortgage_For_Free(RULE_PlayerNumber player, RULE_PropertySet properties);
extern RULE_SquareType AI_Unmortgage_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber player, BOOL only_monopolies, BOOL hypothetical);
extern int AI_Give_List_Players_Important_Monopoly(RULE_GameStatePointer game_state, RULE_SquareType property,
											 RULE_PlayerNumber *player_list, RULE_PlayerNumber *must_list,
											 RULE_PlayerNumber *lowest_trade_list, int num_players, 
											 int num_must_players, int lowest_size_trade, 
											 int get_lowest_trade_number, int *found_x_trades);
extern AI_Property_Importance AI_Give_Players_Important_Monopoly(RULE_GameStatePointer game_state, RULE_SquareType property,
										RULE_PlayerNumber exclude_player, RULE_PlayerNumber include_player);
extern void AI_Move_AI_Player(RULE_PlayerNumber player);
extern BOOL  AI_Should_Unmortgage_Property(RULE_GameStatePointer  game_state, RULE_PlayerNumber player);
extern void AI_Roll_Dice(RULE_PlayerNumber player);
extern void AI_Done_Turn(RULE_PlayerNumber player);
extern EBOOL AI_Should_Buy_House(RULE_GameStatePointer  game_state, RULE_PlayerNumber player);
extern BOOL AI_Unmortgage_Monopoly(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareType SquareNo);
extern BOOL AI_Buy_Houses(RULE_GameStatePointer  game_state,  RULE_PlayerNumber player, AI_House_Options options);
extern BOOL AI_Hypothetical_Buy_Houses(RULE_GameStatePointer  game_state,  RULE_PlayerNumber player);
extern void AI_Pay_Debt(RULE_PlayerNumber player, RULE_PlayerNumber target, long owed, long to_raise);
extern void AI_Process_AIs(void);
extern BOOL	AI_Spend_Excess_Assets(RULE_GameStatePointer game_state, RULE_PlayerNumber player);
extern BOOL AI_Should_Pause(RULE_GameStatePointer game_state, RULE_ActionType Action, RULE_PlayerNumber FromPlayer);
extern BOOL AI_SendAction (RULE_ActionType Action, RULE_PlayerNumber FromPlayer, RULE_PlayerNumber ToPlayer, 
					long NumberA, long NumberB, long NumberC, long NumberD, wchar_t *StringA, 
					unsigned int BinarySize, RULE_CharHandle Blob);
extern int AI_Get_Random_Action_Wait(RULE_ActionType Action, RULE_PlayerNumber player);
extern long	AI_Excess_Cash_Available(RULE_GameStatePointer game_state, RULE_PlayerNumber player, BOOL cash_only);
extern long AI_Get_Total_Worth_With_Factors(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_PlayerNumber ai_strat);
extern BOOL AI_Mortgage_Worst_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber player, BOOL sell_houses,
							   BOOL mortgage_monopoly, BOOL hypothetical);
extern void AI_Test_Sell_Houses(RULE_GameStatePointer game_state, RULE_SquareType *monopoly_lots, int squares_in_monopoly, RULE_SquareType test_square);
extern void AI_Sell_Houses(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareType sell_square,
                    BOOL remember_square);
extern void AI_Buy_Or_Auction(RULE_PlayerNumber player, RULE_SquareType square, long purchase_cost);
extern BOOL AI_Should_Buy_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber player, RULE_SquareType property);
extern BOOL AI_Have_Monopoly_Trade(RULE_GameStatePointer game_state, RULE_PlayerNumber player,
                            RULE_PlayerNumber exclude_player);
extern void AI_Choose_Tax_Decision(RULE_PlayerNumber player);
extern void AI_Choose_Exit_Choice(RULE_PlayerNumber player, BOOL can_roll_doubles,
						   BOOL can_pay_fifty, BOOL can_use_card);
extern void AI_Send_Config_Accepts(RULE_ActionArgumentsPointer NewMessage);
extern void AI_Picked_Up_Card(RULE_PlayerNumber player);
extern void AI_Resend_State(RULE_PlayerNumber player);
extern void AI_Choose_Trade_Acceptance(RULE_PlayerSet players_involved);
extern void AI_Load_Save_Parameters(RULE_ActionArgumentsPointer NewMessage);
extern void AI_Send_Save_Parameters(RULE_PlayerSet players_to_save);
extern void AI_Send_Housing_Shortage_Purchase_Requests(RULE_PlayerSet players, long num_buildings_left,
												RULE_PlayerNumber player_started);
extern void AI_Trap_Trade_Message(RULE_ActionArgumentsPointer Message);
extern void AI_Player_Accepted_Rejected_Trade(RULE_PlayerNumber proposed_player, RULE_PlayerNumber player, BOOL accepted);
extern void AI_Trap_Other_Errors(RULE_ActionArgumentsPointer Message);
extern long AI_Get_Trade_Bid_For_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber player, 
							 RULE_SquareType property, long current_bid, RULE_PlayerNumber cur_bid_player);
extern long AI_Get_Bid_For_Property(RULE_GameStatePointer game_state, RULE_PlayerNumber player, 
							 RULE_SquareType property, long current_bid, RULE_PlayerNumber cur_bid_player);
extern BOOL AI_Bid_In_Auction(RULE_GameStatePointer game_state);
extern BOOL	AI_Is_Paying_Fee(RULE_GameStatePointer game_state, RULE_PlayerNumber player);
extern void AI_Process_AIs_At_Start_Turn(void);
extern void AI_Finish_Trade(void);
extern void AI_Process_BuySellMort(RULE_PlayerNumber player);
extern void AI_Process_Action_Completed(RULE_ActionArgumentsPointer Message);
extern void AI_ProcessMessage (RULE_ActionArgumentsPointer NewMessage);
extern void AI_Send_Text_Message(LANG_TextMessageNumber MessageID, RULE_PlayerNumber player, RULE_PlayerNumber target);
extern void AI_Send_Message(AI_Message_Record message, RULE_PlayerNumber player, RULE_PlayerNumber to_player);

/* Following functions are strictly temporary. */
extern void AI_Test_Me(RULE_ActionArgumentsPointer NewMessage);
extern void AI_Toggle_AI_Info(BOOL only_drone_results);
extern void AI_Show_AI_Info_Box(void);
extern void AI_Player_Won(RULE_PlayerNumber player);
extern void AI_Test_Add_Players(void);

/************************************************************/

#endif // __AI_H__
