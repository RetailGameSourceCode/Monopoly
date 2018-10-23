/*************************************************************
*
*   FILE:             AI_LOAD.C
*
*   (C) Copyright 97  Artech Digital Entertainments.
*                     All rights reserved.
*************************************************************/

#include "gameinc.h"

/************************************************************/
/* GLOBALS                                                  */
/************************************************************/

extern AI_Data_Record AI_Data[RULE_MAX_PLAYERS];
extern AI_Data_Record AI_Expert_AI_Settings;
extern HWND hwndMain;
extern AI_Drone_Configs_Record AI_Drone_Configs;

/************************************************************/
/* LOCAL PROTOTYPES                                         */
/************************************************************/

AI_ATTRIBUTE  AI_Get_Attribute(char *line);
BOOL AI_Get_User_Filename (char *filename);
AI_Drone_Configs_Types  AI_Get_Drone_Config(char *line);

/************************************************************/

/*  The following is the text we expect to see in the AI data 
    file.  There should be an '=' after the text, followed by
	(a)  number(s). */
char	*AI_Attributes[AI_ATTRIBUTE_MAX] =
{
	"PLAYER ATTITUDE",
	"ATTITUDE LOST FOR REJECTING TRADE",
	"ATTITUDE LOST PER EVALUATION POINT WHEN OBSERVING",
	"ATTITUDE CHANGE PER TICK",
	"ATTITUDE NEUTRAL",
	"TIME TO FORGET TRADE",
	"MAX TRADES",
	"TRADE COUNTER LIMIT",
	"PROBABILITY OF COUNTERING TRADE",
	"MINIMUM CASH ON HAND",
	"CASH STRATEGY",
	"BUYING HOUSE STRATEGY",
	"BUYING STAGE CASH MULTIPLIER",
	"NO MONOPOLY STAGE CASH MULTIPLIER",
	"CASH LIQUID ASSETS DEPENDANCE",
	"MONOPOLY EXISTS BUT NOT OWNER CASH MULTIPLIER",
	"MONOPOLY WAR CASH MULTIPLIER",
	"ODDS OF WINNING FACTOR",
	"MAX WINNING FACTOR LOSS ACCEPTABLE IN TRADE",
	"CASH FACTOR",
	"PROPERTY FACTORS",
	"CASH COW FACTORS",
	"PROPERTY STRATEGIC FACTOR",
	"MINIMUM EVALUATION TRADE BENEFIT",
    "MINIMUM EVALUATION WHEN GIVING A MONOPOLY FOR CASH",
	"MONOPOLY RECEIVED IMPORTANCE",
    "GIVING MONOPOLY IMPORTANCE",
    "BALTIC RECEIVED IMPORTANCE",
	"PROPERTY ALLOWS TRADE IMPORTANCE",
    "PROPERTY ALLOWS ANOTHER TRADE IMPORTANCE",
	"PROPERTY GIVEN WITH ONE UNOWNED IMPORTANCE",
	"PROPERTY GIVEN WITH TWO UNOWNED IMPORTANCE",
    "RAILROAD IMPORTANCE FACTORS",
    "UTILITY IMPORTANCE FACTORS",
    "MONOPOLY VETO IMPORTANCE VALUES",
    "NEGATIVE PROPERTY IMPORTANCE CHANGE MULTIPLIER FOR OPPONENTS",
	"PROPOSE TRADE PROBABILITY",
	"MONOPOLY TRADE PROBABILITY",
    "PROPOSE MONOPOLY GIVE AWAY PROBABILITY",
    "MAXIMUMG HOUSES PER SQUARE FOR MONOPOLY GIVE AWAY",
	"MINIMUM PAUSE FOR ACTION",
	"MAXIMUM PAUSE FOR ACTION",
	"PAUSE WHEN CONSIDERING TRADE",
	"PATIENCE",
    "NUMBER OF TIMES CONSIDER PROPERTY TRADE",
    "TURNS BEFORE FORGETTING PROPERTY WAS TRADED",
    "MINIMUM EVALUATION IF FED UP",
    "MONOPOLY SUICIDE FACTOR",
	"LOWEST PROPERTY STRAT FACTOR FOR COUNTER PROPOSAL",
	"MINIMUM NON-MONOPOLY ATTITUDE",
	"WHAT TO TRADE -0.9",
	"WHAT TO TRADE -0.8",
	"WHAT TO TRADE -0.7",
	"WHAT TO TRADE -0.6",
	"WHAT TO TRADE -0.5",
	"WHAT TO TRADE -0.4",
	"WHAT TO TRADE -0.3",
	"WHAT TO TRADE -0.2",
	"WHAT TO TRADE -0.1",
	"WHAT TO TRADE  0.0",
	"WHAT TO TRADE  0.1",
	"WHAT TO TRADE  0.2",
	"WHAT TO TRADE  0.3",
	"WHAT TO TRADE  0.4",
	"WHAT TO TRADE  0.5",
	"WHAT TO TRADE  0.6",
	"WHAT TO TRADE  0.7",
	"WHAT TO TRADE  0.8",
	"WHAT TO TRADE  0.9",
	"WHAT TO TRADE  1.0",
	"STRATEGY FOR LEVEL 1",
	"STRATEGY FOR LEVEL 2",
	"STRATEGY FOR LEVEL 3",
	"GLOBAL SETTINGS"
};

/*  The following is the text we expect to see in the drone config 
    file.  There should be an '=' after the text, followed by
	(a)  number(s). */
char	*AI_Drone_Texts[AI_DRONE_MAX] =
{
    "TOKENS PLAYING",
    "CYCLE PLAYERS",
    "MAX GAME TIME",
    "NUMBER OF GAMES TO PLAY"
};

/* Each token has a specific ai file with it. */
wchar_t *AI_Token_Filenames[MAX_TOKENS] =
{
//	L"BOBAFETT.AI",
//	L"CHEWBACC.AI",
//	L"DARTH.AI",
//	L"HANSOLO.AI",
//	L"LUKE.AI",
//	L"PRINCESS.AI",
//	L"R2D2.AI",
//	L"TROOPER.AI",
//	L"BOBAFETT.AI",
//	L"CHEWBACC.AI"
	L"Cannon.AI",
	L"RaceCar.AI",
	L"Dog.AI",
	L"TopHat.AI",
	L"Iron.AI",
	L"Horse.AI",
	L"Ship.AI",
	L"Boot.AI",
	L"Thimble.AI",
	L"Barrow.AI",
  L"Moneybag.AI"
};

/********************************************************************/
/*					AI_Load_AI										*/
/*																	*/
/*	 Input:	 RULE_PlayerNumber	player   Which AI slot to load it	*/
/*										 into?						*/
/*			 int	token		What token is selected?				*/
/*			 int	level		What level of play to load.			*/
/*   Output: BOOL - TRUE if an AI was loaded. FALSE otherwise.		*/
/*																	*/
/*		Will try to load AI parameters from *name filename.			*/
/*  If the file cannot be found, will let the user select it		*/
/*  (Hotseat only.)													*/
/*  If succesful,  returns TRUE.  Otherwise, returns FALSE.			*/
/********************************************************************/

BOOL AI_Load_AI(RULE_PlayerNumber player, int token, int level)
{
    return(AI_Load_AI_File(AI_Token_Filenames[token], player, level));
}

/********************************************************************/
/*					AI_Load_AI_File									*/
/*																	*/
/*   Input:  wchar_t *name		Name of AI to load in Unicode -     */
/*					If cannot find AI filename, will let user select*/
/*					it.												*/
/*			 RULE_PlayerNumber	player   Which AI slot to load it	*/
/*										 into? RULE_MAX_PLAYERS for */
/*										 AI_Expert_AI_Settings.     */
/*			 int	level		What level of play to load.			*/
/*   Output: BOOL - TRUE if an AI was loaded. FALSE otherwise.		*/
/*																	*/
/*		Will try to load AI parameters from *name filename.			*/
/*  If the file cannot be found, will let the user select it		*/
/*  (Hotseat only.)													*/
/*  If succesful,  returns TRUE.  Otherwise, returns FALSE.			*/
/********************************************************************/

BOOL AI_Load_AI_File(wchar_t *name, RULE_PlayerNumber player, int level)
{
	FILE	*stream;
	char	line[1024], *position;
	double	values[AI_LOAD_MAX_VALUES];
	int		num_values_read, counter;
	AI_Data_Record	*data;
	char	mbs_filename[_MAX_PATH], *expanded_filename;
	int		cur_level = 0;
	
	// Are we loading a dummy expert setting for 'use the force' evaluation,
	// or is this a regular AI player?
	if(player == RULE_MAX_PLAYERS)
		data = &AI_Expert_AI_Settings;
	else
		data = &AI_Data[player];

	/* Want filename in mbs. */
	wcstombs(mbs_filename, name, _MAX_PATH);

//    #if !FORHOTSEAT
//        expanded_filename = FLOC_ExpandFileName(mbs_filename);
//    #else
        expanded_filename = mbs_filename;
//    #endif

	// Did we get a path for the file? 
    if(&mbs_filename[0] == NULL)
		return FALSE;

	/* Can we open it? */
	if((stream = fopen(expanded_filename, "r")) == NULL)
    	{
        #if FORHOTSEAT
		    /* Let the user select a filename. */
		    if(!AI_Get_User_Filename (mbs_filename))
			    return FALSE;
		    /* Now open the new file. */
		    if((stream = fopen(mbs_filename, "r")) == NULL)
			    return FALSE;	/* something went wrong! */
        #else
		    return FALSE; /* Do not allow user to select file in Monopoly version. */
        #endif
	    }

	while(!feof(stream))
	    {
		/* Get the next line in the AI file. */
		fgets(line, 1024, stream);
		line[strlen(line) - 1] = 0;

		if(line[0] == '/')
			continue;  /* comment marker. */

		/* Convert to upper case. */
		_strupr(line);

		/* Where is the '=' ? */
		position = strchr(line, '=');
		if(position == NULL)
			values[0] = 0.0;
		else
		    {
			*position++ = 0;
			/* Read in the data values. */
			num_values_read = 0;
			while( *position != 0)
			    {
				/* Get to the start of the number. */
				while( ((*position == ' ') || (*position == AI_TAB_CHAR)) && *position != 0)	++position;

				/* Read in the value. */
				if( sscanf(position, "%lf", &values[num_values_read]))
					num_values_read++;
				if(num_values_read >= AI_LOAD_MAX_VALUES)
					break;

				/* skip the value. */
				while( ((*position != ' ') && (*position != AI_TAB_CHAR)) && *position != 0) ++position;
			    }
		    }

		/* Each file contains strategies for multiple levels. Which level is this? */
		switch(counter = AI_Get_Attribute(line))
		    {
			case AI_TAG_STRATEGY_LEVEL_1:
			case AI_TAG_STRATEGY_LEVEL_2:
			case AI_TAG_STRATEGY_LEVEL_3:
				cur_level = counter - AI_TAG_STRATEGY_LEVEL_1 + 1;
				break;
			case AI_TAG_GLOBAL_SETTINGS:
				cur_level = -1;
				break;
		    }

		/* Process this section of the file only if this portion of the file describes the level
		   of AI we want to load. */
		if(((cur_level == level) || (cur_level == -1)) && (position != NULL))
		    {
			switch(counter)
			    {
				case AI_ATTRIBUTE_PLAYER_ATTITUDE:
					for(counter = 0; counter < RULE_MAX_PLAYERS; ++counter)
						data->player_attitude[counter] = (float) values[counter];
					break;

				case AI_ATTRIBUTE_ATTITUDE_LOST_REJECT_TRADE:
					data->attitude_lost_for_rejected_trade = (float) values[0];
					break;

				case AI_ATTRIBUTE_ATTITUDE_CHANGE_TRADE_OBSERVING:
					data->attitude_change_trade_observing = (float) values[0];
					break;

				case AI_ATTRIBUTE_ATTITUDE_CHANGE_TICK:
					data->attitude_tick_change = (float) values[0];
					break;

				case AI_ATTRIBUTE_ATTITUDE_NEUTRAL:
					data->player_neutral_attitude = (float) values[0];
					break;

				case AI_ATTRIBUTE_TIME_FORGET_TRADE:
					data->time_forget_trade = (long) values[0];
					break;
				
				case AI_ATTRIBUTE_MAX_TRADES:
					if(values[0] > AI_MAX_TRADES)
						data->max_trades = (long) values[0];
					else if(values[0] < 0)
						data->max_trades = 0;
					else data->max_trades = (long) values[0];
					break;

				case AI_ATTRIBUTE_TRADE_COUNTER_LIMIT:
					data->trade_counter_limit = (int) values[0];
					break;

				case AI_ATTRIBUTE_TRADE_COUNTER_PROB:
					data->trade_counter_prob = (float) values[0];
					break;

				case AI_ATTRIBUTE_MIN_CASH_ON_HAND:
					data->min_cash_on_hand = (long) values[0];
					break;

				case AI_ATTRIBUTE_CASH_STRATEGY:
					data->cash_strategy = (AI_Cash_On_Hand_StrategyRecord) values[0];
					break;

				case AI_ATTRIBUTE_HOUSING_STRAT:
					data->housing_purchase_strat = (long) values[0];
					break;

				case AI_ATTRIBUTE_BUYING_CASH_MULTIPLIER:
					data->buying_stage_cash_multiplier = (float) values[0];
					break;

				case AI_ATTRIBUTE_NO_MONO_STAGE_CASH_MULTIPLIER:
					data->no_mono_stage_cash_multiplier = (float) values[0];
					break;

				case AI_ATTRIBUTE_MONO_NOT_OWNED_CASH_MULTIPLIER:
					data->mono_not_owned_stage_cash_multiplier = (float) values[0];
					break;

				case AI_ATTRIBUTE_CASH_LIQUID_ASSETS_DEPENDANCE:
					data->cash_liquid_assets_dependance = (float) values[0];
					break;

				case AI_ATTRIBUTE_MONO_STAGE_CASH_MULTIPLIER:
					data->mono_stage_cash_multiplier = (float) values[0];
					break;

				case AI_ATTRIBUTE_CHANCES_FACTOR:
					data->chances_factor = (float) values[0];
					break;

				case AI_ATTRIBUTE_CHANCES_THRESHOLD:
					data->chances_threshold = (float) values[0];
					break;

				case AI_ATTRIBUTE_CASH_FACTOR:
					data->cash_factor = (float) values[0];
					break;

				case AI_ATTRIBUTE_PROPERTY_FACTORS:
					for(counter = 0; counter <= SG_PARK_PLACE; ++counter)
						data->property_factors[counter] = (float) values[counter];
					break;

				case AI_ATTRIBUTE_CASH_COW_FACTORS:
					for(counter = 0; counter <= AI_MAX_CASH_COWS; ++counter)
						data->cash_cow_factors[counter] = (float) values[counter];
					break;

				case AI_ATTRIBUTE_TRADE_IMPORTANCE_FACTOR:
					data->trade_importance_factor = (float) values[0];
					break;

				case AI_ATTRIBUTE_MIN_EVALUATION_THRESHOLD:
					data->min_evaluation_threshold = (float) values[0];
                    break;

                case AI_ATTRIBUTE_MIN_GIVE_MONOPOLY_EVALUATION:
                    data->min_give_monopoly_evaluation = (float) values[0];
                    break;

				case AI_ATTRIBUTE_MONOPOLY_RECEIVED_IMPORTANCE:
					data->monopoly_received_importance = (float) values[0];
					break;

				case AI_ATTRIBUTE_GIVING_MONOPOLY_IMPORTANCE:
					data->giving_monopoly_importance = (float) values[0];
					break;

                case AI_ATTRIBUTE_BALTIC_RECEIVED_IMPORTANCE:
					data->baltic_received_importance = (float) values[0];
					break;

				case AI_ATTRIBUTE_PROPERTY_ALLOW_TRADE_IMPORTANCE:
					data->property_allow_trade_importance = (float) values[0];
					break;

                case AI_ATTRIBUTE_PROPERTY_ALLOW_MORE_TRADE_IMPORTANCE:
					data->property_allow_more_trade_importance = (float) values[0];
					break;

				case AI_ATTRIBUTE_PROPERTY_ONE_UNOWNED_IMPORTANCE:
					data->property_one_unowned_importance = (float) values[0];
					break;

				case AI_ATTRIBUTE_PROPERTY_TWO_UNOWNED_IMPORTANCE:
					data->property_two_unowned_importance = (float) values[0];
					break;

                case AI_ATTRIBUTE_RAILROAD_IMPORTANCE:
                    for(counter = 0; counter < AI_MAX_RAILROADS; ++counter)
                        data->railroad_importance[counter] = (float) values[counter];
                    break;

                case AI_ATTRIBUTE_UTILITY_IMPORTANCE:
                    for(counter = 0; counter < AI_MAX_UTILITIES; ++counter)
                        data->utility_importance[counter] = (float) values[counter];
                    break;

                case AI_ATTRIBUTE_MONOPOLY_VETO_IMPORTANCE:
                    for(counter = 0; counter <= SG_PARK_PLACE + 1; ++counter)
                        data->monopoly_veto_importance[counter] = (float) values[counter];
                    break;

                case AI_ATTRIBUTE_NEG_PROP_IMPORTANCE_CHANGE_MULT:
                    data->negative_prop_importance_change_multiplier = (float) values[0];
                    break;

				case AI_ATTRIBUTE_PROPOSE_TRADE_PROBABILITY:
					data->propose_trade_probability = (float) values[0];
					break;

				case AI_ATTRIBUTE_MONOPOLY_PROPOSE_TRADE_PROBABILITY:
					data->monopoly_propose_trade_probability = (float) values[0];
					break;

                case AI_ATTRIBUTE_PROPOSE_MONOPOLY_GIVE_AWAY_PROBABILITY:
					data->propose_monopoly_give_away_probability = (float) values[0];
					break;

                case AI_ATTRIBUTE_MAX_HOUSES_PER_SQUARE_FOR_GIVE_AWAY:
					data->max_houses_per_square_for_give_away = (int) values[0];
					break;

				case AI_ATTRIBUTE_MIN_ACTION_WAIT:
					data->min_action_wait = (int) values[0];
					break;

				case AI_ATTRIBUTE_MAX_ACTION_WAIT:
					data->max_action_wait = (int) values[0];
					break;

				case AI_ATTRIBUTE_WAIT_FOR_TRADE_CONSIDER:
					data->wait_for_trade_consider = (int) values[0];
					break;

				case AI_ATTRIBUTE_PATIENCE:
					data->patience = (long) values[0];
					break;

                case AI_ATTRIBUTE_NUMBER_TIMES_ALLOW_PROPERTY_TRADE:
                    if(values[0] > 3)
                        values[0] = 3;
                    if(values[0] < 0)
                        values[0] = 0;

					data->number_times_allow_property_trade = (int) values[0];
					break;

                case AI_ATTRIBUTE_TURNS_TO_FORGETTING_PROPERTY_TRADE:
					data->turns_to_forgetting_property_trade = (int) values[0];
					break;

                case AI_ATTRIBUTE_MIN_EVALUATION_IF_FED_UP:
					data->min_evaluation_if_fed_up = (float) values[0];
					break;

                case AI_ATTRIBUTE_MONOPOLY_SUICIDE_FACTOR:
					data->monopoly_suicide_factor = (float) values[0];
					break;

				case AI_ATTRIBUTE_LOWEST_IMPORTANCE_FOR_COUNTER:
					data->lowest_prop_importance_for_counter = (float) values[0];
					break;

				case AI_ATTRIBUTE_MINIMUM_NONMONOPOLY_TRADE_ATTITUDE:
					data->minimum_nonmonopoly_trade_attitude = (float) values[0];
					break;

				case AI_ATTRIBUTE_WHAT_TO_TRADE_NEG_9:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_NEG_8:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_NEG_7:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_NEG_6:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_NEG_5:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_NEG_4:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_NEG_3:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_NEG_2:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_NEG_1:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_0:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_1:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_2:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_3:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_4:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_5:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_6:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_7:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_8:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_9:
				case AI_ATTRIBUTE_WHAT_TO_TRADE_10:
					/* Calc an index into AI_Data.what_to_trade[] */
					counter -= AI_ATTRIBUTE_WHAT_TO_TRADE_NEG_9;

					data->what_to_trade[counter].give_monopoly = (AI_Property_Classification) values[0];
					if(data->what_to_trade[counter].give_monopoly != 0)
						data->what_to_trade[counter].give_monopoly += AI_Property_Worst - 1;

					data->what_to_trade[counter].give_group_trades = (AI_Property_Classification) (values[1]);
					data->what_to_trade[counter].give_cash_cows = (AI_Property_Classification) (values[2]);
					data->what_to_trade[counter].give_junk = (AI_Property_Classification) (values[3]);
					data->what_to_trade[counter].cash_multiplier = values[4];
					break;

				case AI_TAG_STRATEGY_LEVEL_1:
				case AI_TAG_STRATEGY_LEVEL_2:
				case AI_TAG_STRATEGY_LEVEL_3:
					break;	/* Already taken care of. */

				default:
					if(MessageBox(NULL, line, "Don't know what this means in AI file.", MB_ICONEXCLAMATION | MB_OKCANCEL)
						 == IDCANCEL)
						return FALSE;
					break;
			    }
		    }
	    }

	/* Finished with the file. */
	fclose(stream);
    return TRUE;
}

/******************************************************************/
/*					AI_Get_Attribute							  */
/*																  */
/*   Input:  char *line											  */
/*	 Output: AI_ATTRIBUTE found in *line, where the corresponding */
/*				          text is in AI_Attributes[]			  */
/******************************************************************/

AI_ATTRIBUTE  AI_Get_Attribute(char *line)
{
	int	counter;

	/* go through all the possible attributes. */
	for(counter = 0; counter < AI_ATTRIBUTE_MAX; ++counter)
    	{
		/* Did we find the attribute text? */
		if(strstr(line, AI_Attributes[counter]) != 0)
			return counter; /* Yes. Return this attribute. */
	    }

    return AI_ATTRIBUTE_NONE_FOUND;
}

/******************************************************************/
/*					AI_Get_Drone_Config 						  */
/*																  */
/*   Input:  char *line											  */
/*	 Output: AI_Drone_Configs_Types found in *line.               */
/******************************************************************/

AI_Drone_Configs_Types  AI_Get_Drone_Config(char *line)
{
	int	counter;

	/* go through all the possible attributes. */
	for(counter = 0; counter < AI_DRONE_MAX; ++counter)
	    {
		/* Did we find the attribute text? */
		if(strstr(line, AI_Drone_Texts[counter]) != 0)
			return counter; /* Yes. Return this attribute. */
	    }

    return AI_DRONE_NONE_FOUND;
}


/******************************************************************/
/*					AI_Get_User_Filename						  */
/*																  */
/*   Input:  char    *filename	Where to store the filename      .*/
/*	 Output: BOOL  - TRUE if an AI filename was chosen.  FALSE if */
/*                   not,  *filename is set to the new path. 	  */
/*																  */
/*		This function gives allows the user to choose an AI file. */
/*  The path of this AI file is store in *filename.	Thank you     */
/*  Alex for most of the code (from LoadUserSpecifiedBitmap).     */
/******************************************************************/

BOOL AI_Get_User_Filename (char *filename)
{
    OPENFILENAME          ofnTemp;

    strcpy (filename, "normal.ai");

    memset (&ofnTemp, 0, sizeof(ofnTemp));
    ofnTemp.lStructSize = sizeof(OPENFILENAME);
    #if FORHOTSEAT
        ofnTemp.hwndOwner = hwndMain;
    #else
        ofnTemp.hwndOwner = NULL;
    #endif

    ofnTemp.lpstrFilter = "AI Strategies\0*.AI\0";
    ofnTemp.lpstrFile = filename;
    ofnTemp.nMaxFile = _MAX_PATH;
    ofnTemp.lpstrInitialDir = ".";
    ofnTemp.lpstrTitle = "Choose AI Strategy";
    ofnTemp.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES |
                    OFN_PATHMUSTEXIST;
    ofnTemp.lpstrDefExt = "AI";

    if (!GetOpenFileName (&ofnTemp))
        return FALSE;

    return TRUE;
}

/******************************************************************/
/*					AI_Initialize_Loading						  */
/*																  */
/*		Resets/initializes arrays used by AI_Load_AI.			  */
/******************************************************************/

void AI_Initialize_Loading(void)
{
    return;
}

/********************************************************************/
/*					AI_Load_Drone_Mode_Configs						*/
/*																	*/
/*   Input:  wchar_t *name		Name of AI to load in Unicode -     */
/*   Output: BOOL - TRUE if configs were loaded.FALSE otherwise.    */
/*																	*/
/*		Will try to load game parameters for a drone game.          */
/********************************************************************/

BOOL AI_Load_Drone_Mode_Configs(wchar_t *name)
{
	FILE	*stream;
	char	line[1024], *position;
	double	values[AI_LOAD_MAX_VALUES];
	int		num_values_read, counter;
	char	mbs_filename[_MAX_PATH], *expanded_filename;
	
	/* Want filename in mbs. */
	wcstombs(mbs_filename, name, _MAX_PATH);

//    #if !FORHOTSEAT
//        expanded_filename = FLOC_ExpandFileName(mbs_filename);
//    #else
        expanded_filename = mbs_filename;
//    #endif

	/* Can we open it? */
	if((stream = fopen(expanded_filename, "r")) == NULL)
	    {
        LE_ERROR_ErrorMsg ("Cannot open drone config file!");
		return FALSE; 
	    }

	while(!feof(stream))
	    {
		/* Get the next line in the config file. */
		fgets(line, 1024, stream);
		line[strlen(line) - 1] = 0;

		if(line[0] == '/')
			continue;  /* comment marker. */

		/* Convert to upper case. */
		_strupr(line);

		/* Where is the '=' ? */
		position = strchr(line, '=');
		if(position == NULL)
			values[0] = 0.0;
		else
		    {
			*position++ = 0;
			/* Read in the data values. */
			num_values_read = 0;
			while( *position != 0)
			    {
				/* Get to the start of the number. */
				while( ((*position == ' ') || (*position == AI_TAB_CHAR)) && *position != 0)	++position;

				/* Read in the value. */
				if( sscanf(position, "%lf", &values[num_values_read]))
					num_values_read++;
				if(num_values_read >= AI_LOAD_MAX_VALUES)
					break;

				/* skip the value. */
				while( ((*position != ' ') && (*position != AI_TAB_CHAR)) && *position != 0) ++position;
			    }
		    }

        counter = AI_Get_Drone_Config(line);
		if(position != NULL)
		    {
			switch(counter)
			    {
				case AI_DRONE_PLAYERS_TOKENS_PLAYING:
					for(counter = 0; counter < num_values_read; ++counter)
                        AI_Drone_Configs.players_playing[counter] = (RULE_PlayerNumber) values[counter];
                    AI_Drone_Configs.num_players_playing = counter;
					break;

                case AI_DRONE_CYCLE_PLAYERS:
                    if(values[0])
                        AI_Drone_Configs.cycle_players = TRUE;
                    else
                        AI_Drone_Configs.cycle_players = FALSE;
                    break;

                case AI_DRONE_MAX_GAME_TIME:
                    AI_Drone_Configs.max_game_time = (long) values[0];
                    break;

                case AI_DRONE_GAMES_TO_PLAY:
                    AI_Drone_Configs.number_games_to_play = (long) values[0];
                    break;

				default:
					if(MessageBox(NULL, line, "Don't know what this means in Drone config file.", MB_ICONEXCLAMATION | MB_OKCANCEL)
						 == IDCANCEL)
						return FALSE;
					break;
			    }
		    }
	    }

	/* Finished with the file. */
	fclose(stream);
    return TRUE;
}
