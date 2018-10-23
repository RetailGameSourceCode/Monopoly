#if ! defined( __LANGIDS_H__ )
#define __LANGIDS_H__
/*****************************************************************************
 * First up are all the error messages (see NOTIFY_ERROR_MESSAGE for details).
 * The error strings have special embedded codes that are expanded when the
 * string is displayed.  The codes are made from a caret (^) and a following
 * character:
 *   ^1 expands numberB as a number,
 *   ^2 expands numberC as a number,
 *   ^3 expands numberD as a number,
 *   ^4 expands numberE as a number,
 *   ^A expands stringA,
 *   ^P expands numberC as a player name,
 *   ^Q expands numberB as a player name,
 *   ^S expands numberD as a square name
 *   ^T expands numberE as a set of squares.
 */

#define TMN_NO_MESSAGE                          0
#define WAV_NO_SOUND                            0
  /* The special value of zero stands for no message at all. */

#define TMN_ERROR_NO_MORE_PLAYERS               1
  /* Sent when there are no free slots for a new player.  StringA is the
  name the new player wanted. */

#define TMN_ERROR_NOT_YOUR_PLAYER               2
  /* Sent when someone tries to rename someone else's player.  StringA is
  the new name, numberC is the player that was being renamed. */

#define TMN_ERROR_FUNCTION_NOT_IMPLEMENTED      3
  /* Some piece of code hasn't been written yet or some functionallity is
  missing.  StringA names the function. */

#define TMN_ERROR_NAME_IN_USE                   4
  /* The new player's requested name, token or colour choice is already
  in use by someone else.  StringA contains the requested name and
  numberC the existing player's slot. */

#define TMN_ERROR_WRONG_PHASE                   5
  /* An action was received that was inappropriate for the current game
  phase.  The action code is in numberB, player in numberC and phase
  of the bad action in numberD. */

#define TMN_ERROR_NEED_TWO_PLAYERS              6
  /* You need to have two or more players before starting a game. */

#define TMN_ERROR_WRONG_PLAYER                  7
  /* An action was received from the incorrect player.  The action code
  is in numberB, player in numberC and phase in numberD. */

#define TMN_ERROR_UMIMPLEMENTED_SQUARE          8
  /* The player has landed on a square where we haven't yet implemented
  the things that happen on it.  NumberC is the player, NumberD is
  the square. */

#define TMN_ERROR_DEBTOR_BANKRUPT               9
  /* The player owing money has gone bankrupt before the money can be
  collected, all he owns is forfeit to the debt collector.  NumberB
  is the player who owed the money and is now bankrupt, NumberC is
  the player who was owed money, numberD is the debt amount. */

#define TMN_ERROR_NEED_JAIL_ROLL                10
  /* You can't roll to get out of jail, already used up your rolls.
  Player number in numberC. */

#define TMN_ERROR_NEED_JAIL_MONEY               11
  /* You need more money that that to get out of jail.  Required amount
  in numberB, player number in numberC, actual cash in numberD. */

#define TMN_ERROR_NEED_JAIL_CARD                12
  /* You need a get out of jail card.  Player number in numberC. */

#define TMN_ERROR_UNIMPLEMENTED                 13
  /* Some functionality is unimplemented, description in StringA. */

#define TMN_ERROR_RESTART_NOT_ALLOWED           14
  /* Player NumberC can't do a restart in the phase in numberB. */

#define TMN_ERROR_BUILDING_ON_UNOWNED           15
  /* Player NumberC can't build/sell a house/hotel on square NumberD because
 he doesn't own it. */

#define TMN_ERROR_BUILDING_NEEDS_MONOPOLY       16
  /* Player NumberC can't build/sell a house/hotel on square NumberD because
 he doesn't own all the other squares in the monopoly. */

#define TMN_ERROR_BUILDING_ON_NONPROPERTY       17
  /* Player NumberC can't build/sell a house/hotel on square NumberD because
 it isn't a square that you can build on. */

#define TMN_ERROR_BUILDING_NO_HOTELS            18
  /* Player NumberC can't build a hotel on square NumberD because
 there are no free hotels. */

#define TMN_ERROR_BUILDING_NO_HOUSES            19
  /* Player NumberC can't build a house on square NumberD because
 there aren't enough free houses. */

#define TMN_ERROR_BUILDING_NO_BREAKDOWN_HOUSES  20
  /* Player NumberC can't sell a hotel on square NumberD because
  there aren't enough free houses to break it down into.  NumberB is
  the number of houses needed for breaking down a hotel. */

#define TMN_ERROR_BUILDING_NOT_EVEN             21
  /* Player NumberC can't build or sell houses in an uneven fashion
  from square NumberD.  Needs to buy/sell on property that has
  NumberB houses. */

#define TMN_ERROR_UNIMPLEMENTED_CARD            22
  /* You have picked up an unimplemented chance or chest card.  Player NumberC,
  Card NumberB, square NumberD. */

#define TMN_ERROR_BUILDING_NEEDS_CASH           23
  /* Player NumberC needs NumberB in cash before he can buy a building on
 square NumberD. */

#define TMN_ERROR_BANKRUPT_TO                   24
  /* Player NumberC has gone bankrupt to player NumberB over a debt
  of size NumberD. */

#define TMN_ERROR_UNDO_FOR_BANKRUPTCY_TO        25
  /* Player NumberC has gone bankrupt to player NumberB over a debt
  of size NumberD.  Also, actions that happened between the start of
  the debt collection and the bankruptcy have been undone. */

#define TMN_ERROR_AUCTION_SQUARE_NEEDED         26
  /* Player NumberC can't bid in the auction until he has picked a square
  for his building. */

#define TMN_ERROR_AUCTION_WRONG_BUILDING        27
  /* Player numberC can't build on square numberD because the auction
  is for a house/hotel when he needs the opposite. */

#define TMN_ERROR_JOINED_BUILDING_AUCTION       28
  /* Player NumberC has joined the building auction with a request
  for a building on NumberD. */

#define TMN_ERROR_BUILDING_MAXED_OUT            29
  /* Player NumberC can't build on square NumberD because it
 already has a hotel. */

#define TMN_ERROR_BUILDING_ALREADY_BUYING       30
  /* Player NumberC can't build on square NumberD because he already has a
 pending building shortage question about some other square. */

#define TMN_ERROR_COLLECTOR_BANKRUPT            31
  /* The player in NumberC went bankrupt before he could collect the
  debt in NumberD from player NumberB, so someone doesn't have to pay. */

#define TMN_ERROR_SELL_NO_HOUSES                32
  /* The player in NumberC is trying to sell houses on square NumberD when
  there aren't any houses there. */

#define TMN_ERROR_MORTGAGING_ON_UNOWNED         33
  /* Player NumberC is trying to mortgage square NumberD which
  he doesn't own. */

#define TMN_ERROR_MORTGAGE_NO_CASH              34
  /* Player NumberC is trying to mortgage square NumberD and needs to
  raise NumberB in cash, which he doesn't currently have. */

#define TMN_ERROR_BUILDING_MORTGAGED            35
  /* Player NumberC can't build/sell a house/hotel on square NumberD because
  some of the squares in the monopoly are mortgaged. */

#define TMN_ERROR_MORTGAGING_HOUSES             36
  /* Can't mortgage while there are houses, player in NumberC,
  square in NumberD. */

#define TMN_ERROR_MORTGAGE_INFO                 37
  /* Player NumberC has mortgaged square NumberD for NumberB dollars. */

#define TMN_ERROR_UNMORTGAGE_INFO               38
  /* Player NumberC has unmortgaged square NumberD for NumberB dollars. */

#define TMN_ERROR_RENT_INFO                     39
  /* The current player is charged NumberB dollars rent for landing on
  square NumberD which is owned or operated (futures) by player NumberC. */

#define TMN_ERROR_TAXES_CHARGED                 40
  /* Describes player NumberC's taxes as being NumberB dollars. */

#define TMN_ERROR_CANT_TRADE_THAT               41
  /* Player NumberC has tried to trade something they can't trade. */

#define TMN_ERROR_TRADE_ACCEPTED                42
  /* Player NumberC has accepted the trade. */

#define TMN_ERROR_TRADE_REJECTED                43
  /* Player NumberC has rejected the trade. */

#define TMN_ERROR_TRADE_CHANGING                44
  /* Player NumberC is changing the trade. */

#define TMN_ERROR_TRADE_BANKRUPTCY              45
  /* Player NumberC would go bankrupt if the trade went through,
  fix the trade so that doesn't happen.  Expenses in NumberB
  and assets are in NumberD. */

#define TMN_ERROR_TRADE_HOUSE_SALE              46
  /* Houses on player NumberC's monopoly containing square
  NumberD will be automatically sold as part of the trading action. */

#define TMN_ERROR_TRADE_CANT_EDIT_DURING_ACCEPT 47
  /* Player NumberC isn't allowed to edit the trade during the acceptance
  phase because he isn't involved in the deal. */

#define TMN_ERROR_TRADE_GIVE_AND_GET            48
  /* Player numberC needs to both give and get things (or nothing) to have
  a legal trade.  He will now edit it to fix it up. */

#define TMN_ERROR_SAVE_GAME_LATER               49
  /* You can only save the game when nothing is going on, such as at the start
  of a turn.  Please try later.  Player requesting the save is in NumberC. */

#define TMN_ERROR_SAVE_GAME_FAILURE             50
  /* Unable to create a saved game message due to a lack of memory
  or some other system problem.  Player requesting the save is in NumberC. */

#define TMN_ERROR_LOAD_GAME_FAILURE             51
  /* Bad data received in a request to load a game.  Player in NumberC. */

#define TMN_ERROR_LOAD_OPTIONS_FAILURE          52
  /* Unable to load your game options due to bad data.  Player NumberC. */

#define TMN_ERROR_LOAD_OPTIONS_LATER            53
  /* Unable to load your game options at this time (perhaps you are only
  a spectator).  Player NumberC. */

#define TMN_ERROR_IMMUNITY_OUT_OF               54
  /* Sorry, player NumberC, but we can't keep track of any more immunities.
  There are already NumberB of them in use. */

#define TMN_ERROR_IMMUNITY_GRANTED              55
  /* Player NumberB has granted an immunity to player NumberC for a count of
  NumberD hits on the set of squares in NumberE. */

#define TMN_ERROR_IMMUNITY_USED                 56
  /* Player NumberB has used an immunity from player NumberC by landing
  on square NumberD.  There are still NumberE of them left (can be zero). */

#define TMN_ERROR_GAME_WON_NORMAL               57
  /* Player NumberB has won the game because all the other
  players are bankrupt. */

#define TMN_ERROR_GAME_WON_TIME                 58
  /* Player NumberB has won the game because he has the most wealth (total
  wealth in NumberC) after the time limit has expired (the time limit game). */

#define TMN_ERROR_GAME_WON_SHORT                59
  /* Player NumberB has won the game because he has the most wealth (total
  wealth in NumberC) after the second player has gone bankrupt (the short game). */

#define TMN_ERROR_BUILDING_BOUGHT               60
  /* Player NumberB has bought a building on square NumberD for
  NumberC money units. */

#define TMN_ERROR_BUILDING_SOLD                 61
  /* Player NumberB has sold a building on square NumberD for
  NumberC money units. */

#define TMN_ERROR_BUILDING_ALL_SOLD             62
  /* Player NumberB has sold all buildings at once on the monopoly containing
  square NumberD for NumberC money units. */

#define TMN_ERROR_PLAYER_CHEATING               63
  /* Player NumberC is using the cheat functions. */

#define TMN_ERROR_PROPERTY_FEES                 64
  /* Player NumberC has to pay NumberB dollars for NumberD houses and
  NumberE hotels. */

#define TMN_ERROR_WAIT_ADD_OTHER_PLAYERS        65
  /* You can't add any more players until the remaining NumberB out of
  NumberD people have added theirs. */

#define TMN_ERROR_SAVE_OPTIONS_FAILURE          66
  /* Unable to save your game options due to low memory etc.  Player NumberC. */

#define TMN_ERROR_CANNOT_GO_BANKRUPT            67
  /* Player NumberC is not allowed to go bankrupt because he can raise NumberD
  by selling buildings and mortgaging.  Owes the debt to player NumberB. */

#define TMN_ERROR_CANT_TRADE_UNTIL_SETTLED      68
  /* Player NumberC isn't allowed to propose a trade because the previous
  trade hasn't been settled yet. */

#define TMN_ERROR_TIE_ROLLOFF                   69
  /* There are NumberD players in a tie that need to roll off for deciding
  who starts the game, including Player NumberC.  NumberB contains 1 less
  than NumberD. */

#define TMN_ERROR_GAME_STARTING                 70
  /* The game is starting.  Player NumberC goes first. */

#define TMN_ERROR_PLAYER_DISCONNECTED           71
  /* Player NumberC has been disconnected due to a network failure or
  inactivity and replaced with an AI. */

#define TMN_ERROR_PLAYER_REPLACED               72
  /* Player NumberC is taking over from an AI.  Old AIs name in StringA. */

#define TMN_ERROR_PLAYER_INACTIVE_WARNING       73
  /* Player NumberC has been inactive for NumberB seconds and will be
  disconnected in NumberD more seconds. */

#define TMN_ERROR_FUTURE_RENT_GRANTED           74
  /* Player NumberB has granted future rent to player NumberC for a count of
  NumberD hits on the set of squares in NumberE. */

#define TMN_ERROR_FUTURE_RENT_USED              75
  /* Player NumberB has used a future rent hit from player NumberC because
  someone landed on square NumberD.  There are still NumberE of them left
  (can be zero). */

#define TMN_GAME_PAUSED                         76
  /* Player NumberB has paused the game. */

#define TMN_GAME_UNPAUSED                       77
  /* Player NumberB has unpaused the game. */

#define TMN_ADDING_PLAYERS                      78
  /* The game is now accepting new players.  There is room for
  NumberB more players. */

#define TMN_FREE_PARKING_POT_COLLECTED          79
  /* Player NumberC has collected the free parking pot of NumberB dollars. */

#define TMN_ERROR_NEED_HUMAN_PLAYERS            80
  /* The game needs human players on NumberB more computers.  This is because
  the no spectators option is in force. */



/*****************************************************************************
 * These error messages are only displayed locally, never sent over the
 * network.  So, if your program doesn't use them, it doesn't need to
 * have strings for these messages.
 */

#define TMN_ERROR_LOBBY_CONNECT_FAILED          506
  /* The lobby failed to make a DirectPlay connection.  NumberB error code. */

#define TMN_ERROR_DPM_CANT_CREATE               507
  /* Can't create the DirectPlay object.  Error in NumberB. */

#define TMN_ERROR_LOBBY_APP_GUID                509
  /* The DirectPlay lobby wants to start a game session with a different "GUID"
  than the one used for games that understand the "Monopoly1997a" protocol
  (which is what this game uses).  That means that network play probably won't
  work properly. */

#define TMN_ERROR_DPM_NO_SERVER_PLAYER          510
  /* Unable to find the server player. */

#define TMN_ERROR_DPM_CANT_OPEN                 511
  /* Can't open a new or existing DirectPlay session.  Error in NumberB. */

#define TMN_ERROR_DPM_CANT_CONNECT              512
  /* Can't connect to a service provider or shortcut connection.  Error in NumberB. */

#define TMN_ERROR_DPM_PLAYER_CREATE             513
  /* Unable to create your player using the manually provided name and
  settings.  DirectPlay error in NumberB. */

#define TMN_ERROR_DPNAMESET                     514
  /* Unable to rename the DirectPlay player.  Error in NumberB, name in StringA. */

#define TMN_ERROR_DP_SESSION_LOST               515
  /* The DirectPlay session has been lost. */

#define TMN_WSOCK_NOT_AVAILABLE                 550
  /* This string is displayed for the network address of your computer when
  the WinSock system isn't available. */

#define TMN_ERROR_WS_CONNECT_ACCEPT_FAILURE     551
  /* This string is shown when an error happens that stops the server from
  accepting new connections.  The error code is in NumberB. */


/****************************************************************************/

#define TMN_MESSAGE_BOX_TITLE                   800
  /* Title used for standard Windows messages boxes that are displaying an
  error message or some other warning to the user.  The name of the program
  is a good choice for this one. */

#define TMN_BUTTON_CANCEL                       801
  /* The text on the button that cancels dialog boxes. */

#define TMN_BUTTON_OK                           802
  /* The text on the button that accecpts dialog boxes. */

#define TMN_ARGS_SERVER                         810
  /* Command line arguments string that tells the program it is to run
  networking in server mode.  First character should be a dash.  All charcters
  should be lower case (or whatever is the equivalent for lazy typers).
  "-server" in English. */

#define TMN_ARGS_CLIENT                         811
  /* Asks the program to run networking in client mode.  "-client"
  in English. */

#define TMN_ARGS_DIRECTPLAY                     812
  /* Specifies DirectPlay networking.  "-directplay" */

#define TMN_ARGS_SOCKET                         813
  /* Specifies WinSock networking.  "-winsock" */

#define TMN_ARGS_IP_ADDRESS                     814
  /* Specifies the IP address to use for connecting with a
  server.  "-ipaddress" */

#define TMN_ARGS_TCP_PORT                       815
  /* Specifies the TCP port number to use for server mode.
  "-tcpport" */

#define TMN_ARGS_NETSYS_CHOOSE                  816
  /* This keyword makes the system ask the user which network
  system they want to use.  -netchoose */

#define TMN_ARGS_PLAYERNAME                     817
  /* The externally specified player name.  -playername */

#define TMN_BOX_SERVER_TITLE                    820
  /* The title of the dialog box that asks the user if
  they want to be a client or a server. */

#define TMN_BOX_SERVER_PROMPT                   821
  /* The body of the text in the dialog box explaining what client
  and server are. */

#define TMN_BOX_SERVER_SERVER_BUTTON            822
  /* The text on the button that selects server mode. */

#define TMN_BOX_SERVER_CLIENT_BUTTON            823
  /* The text on the button that selects client mode. */

#define TMN_BOX_DP_SESSION_TITLE                824
  /* The window title for the dialog box that asks for
  the DirectPlay session to join. */

#define TMN_DP_SESSION_NAME_UNKNOWN             825
  /* The session name used when we don't have one. */

#define TMN_DP_USER_NAME_UNKNOWN                826
  /* The user name used when we can't get one. */

#define TMN_BOX_DP_SERVICE_TITLE                827
  /* The window title for the dialog box that asks for
  the DirectPlay service provider. */

#define TMN_BOX_GETIP_TITLE                     828
  /* The window title for the dialog box that asks for the IP address
  when connecting with WinSock. */

#define TMN_BOX_GETIP_PROMPT                    829
  /* Descriptive text to ask the user to enter an IP address for WinSock. */

#define TMN_BOX_SHOWIP_TITLE                    830
  /* The window title for the dialog box that shows the host their
  own IP address when connecting with WinSock. */

#define TMN_BOX_SHOWIP_PROMPT                   831
  /* Descriptive text explaining what to do with the host's
  IP address for WinSock. */

#define TMN_BOX_NETSYS_TITLE                    832
  /* Title for the dialog box that asks the user to choose which network
  systems to use.  Needed for both hosting and joining. */

#define TMN_BOX_NETSYS_PROMPT                   833
  /* Descriptive text for the dialog box that asks the user to choose
  which network systems to use.  Needed for both hosting and joining. */

#define TMN_BOX_NETSYS_DIRECTPLAY               834
  /* Label for the checkbox that lets you select DirectPlay. */

#define TMN_BOX_NETSYS_WINSOCK                  835
  /* Label for the checkbox that lets you pick WinSock. */

#define TMN_BOX_NETSYS_PORT                     836
  /* Label for the edit box that lets you type in the port number for
  Winsock hosting.  The port box isn't there when running as a client.*/



/*****************************************************************************
 * Other miscellaneous things.
 */

#define TMN_BANK_NAME                           900
#define TMN_SPECTATOR_NAME                      901
#define TMN_NOBODY_NAME                         902
  /* Predefined names of various non-players, like the bank. */

#define ATR_TOKEN_NAME_0                        920
#define ATR_TOKEN_NAME_1                        921
#define ATR_TOKEN_NAME_2                        922
#define ATR_TOKEN_NAME_3                        923
#define ATR_TOKEN_NAME_4                        924
#define ATR_TOKEN_NAME_5                        925
#define ATR_TOKEN_NAME_6                        926
#define ATR_TOKEN_NAME_7                        927
#define ATR_TOKEN_NAME_8                        928
#define ATR_TOKEN_NAME_9                        929
#define ATR_TOKEN_NAME_10                       930
  /* Predefined names of the players. */


/*****************************************************************************
 * Here's a batch of square names.  The numbering must be the same
 * order as in the SquareType data structure.  There are
 * pointers in the SquarePredefinedInfo array to these strings for
 * convenience.
 */

#define TMN_SQUARENAME_GO                       1001
#define TMN_SQUARENAME_MEDITERRANEAN_AVENUE     1002
#define TMN_SQUARENAME_COMMUNITY_CHEST_1        1003
#define TMN_SQUARENAME_BALTIC_AVENUE            1004
#define TMN_SQUARENAME_INCOME_TAX               1005
#define TMN_SQUARENAME_READING_RR               1006
#define TMN_SQUARENAME_ORIENTAL_AVENUE          1007
#define TMN_SQUARENAME_CHANCE_1                 1008
#define TMN_SQUARENAME_VERMONT_AVENUE           1009
#define TMN_SQUARENAME_CONNECTICUT_AVENUE       1010
#define TMN_SQUARENAME_JUST_VISITING            1011
#define TMN_SQUARENAME_ST_CHARLES_PLACE         1012
#define TMN_SQUARENAME_ELECTRIC_COMPANY         1013
#define TMN_SQUARENAME_STATES_AVENUE            1014
#define TMN_SQUARENAME_VIRGINIA_AVENUE          1015
#define TMN_SQUARENAME_PENNSYLVANIA_RR          1016
#define TMN_SQUARENAME_ST_JAMES_PLACE           1017
#define TMN_SQUARENAME_COMMUNITY_CHEST_2        1018
#define TMN_SQUARENAME_TENNESSEE_AVENUE         1019
#define TMN_SQUARENAME_NEW_YORK_AVENUE          1020
#define TMN_SQUARENAME_FREE_PARKING             1021
#define TMN_SQUARENAME_KENTUCKY_AVENUE          1022
#define TMN_SQUARENAME_CHANCE_2                 1023
#define TMN_SQUARENAME_INDIANA_AVENUE           1024
#define TMN_SQUARENAME_ILLINOIS_AVENUE          1025
#define TMN_SQUARENAME_BnO_RR                   1026
#define TMN_SQUARENAME_ATLANTIC_AVENUE          1027
#define TMN_SQUARENAME_VENTNOR_AVENUE           1028
#define TMN_SQUARENAME_WATER_WORKS              1029
#define TMN_SQUARENAME_MARVIN_GARDENS           1030
#define TMN_SQUARENAME_GO_TO_JAIL               1031
#define TMN_SQUARENAME_PACIFIC_AVENUE           1032
#define TMN_SQUARENAME_NORTH_CAROLINA_AVENUE    1033
#define TMN_SQUARENAME_COMMUNITY_CHEST_3        1034
#define TMN_SQUARENAME_PENNSYLVANIA_AVENUE      1035
#define TMN_SQUARENAME_SHORT_LINE_RR            1036
#define TMN_SQUARENAME_CHANCE_3                 1037
#define TMN_SQUARENAME_PARK_PLACE               1038
#define TMN_SQUARENAME_LUXURY_TAX               1039
#define TMN_SQUARENAME_BOARDWALK                1040
#define TMN_SQUARENAME_IN_JAIL                  1041
#define TMN_SQUARENAME_OFF_BOARD                1042

#define TMN_GROUPNAME_1                         1043
#define TMN_GROUPNAME_2                         1044
#define TMN_GROUPNAME_3                         1045
#define TMN_GROUPNAME_4                         1046
#define TMN_GROUPNAME_5                         1047
#define TMN_GROUPNAME_6                         1048
#define TMN_GROUPNAME_7                         1049
#define TMN_GROUPNAME_8                         1050


/*******************************************/
/*  AI messages for each token.        */
/*******************************************/

#define TMN_AI_MESSAGE_START                    2000  /* message # of first message. */
#define TMN_AI_MESSAGE_TOKEN_SEPARATOR          100   /* difference between start of each character. */
#define TMN_AI_MESSAGE_TYPE_SEPARATOR           5   /* Difference between different messages. */
#define TMN_AI_START_SINGLE_MESSAGES            2080   /* Single messages start.*/

// NOTE:  The following describe the message content
#define TMN_GUN_TRADE_COUNTER_1           2000
#define TMN_GUN_TRADE_COUNTER_2           2001
#define TMN_GUN_TRADE_COUNTER_3           2002
#define TMN_GUN_TRADE_COUNTER_4           2003
#define TMN_GUN_TRADE_COUNTER_5           2004
#define TMN_GUN_TRADE_REJECT_1            2005
#define TMN_GUN_TRADE_REJECT_2            2006
#define TMN_GUN_TRADE_REJECT_3            2007
#define TMN_GUN_TRADE_REJECT_4            2008
#define TMN_GUN_TRADE_REJECT_5            2009
#define TMN_GUN_IMPATIENT_1               2010
#define TMN_GUN_IMPATIENT_2               2011
#define TMN_GUN_IMPATIENT_3               2012
#define TMN_GUN_IMPATIENT_4               2013
#define TMN_GUN_IMPATIENT_5               2014
#define TMN_GUN_OFFER_MONOPOLY_TRADE_1    2015
#define TMN_GUN_OFFER_MONOPOLY_TRADE_2    2016
#define TMN_GUN_OFFER_MONOPOLY_TRADE_3    2017
#define TMN_GUN_OFFER_MONOPOLY_TRADE_4    2018
#define TMN_GUN_OFFER_MONOPOLY_TRADE_5    2019
#define TMN_GUN_TOO_MANY_COUNTERS_1       2020
#define TMN_GUN_TOO_MANY_COUNTERS_2       2021
#define TMN_GUN_TOO_MANY_COUNTERS_3       2022
#define TMN_GUN_TOO_MANY_COUNTERS_4       2023
#define TMN_GUN_TOO_MANY_COUNTERS_5       2024
#define TMN_GUN_NOT_SERIOUS_TRADER_1      2025
#define TMN_GUN_NOT_SERIOUS_TRADER_2      2026
#define TMN_GUN_NOT_SERIOUS_TRADER_3      2027
#define TMN_GUN_NOT_SERIOUS_TRADER_4      2028
#define TMN_GUN_NOT_SERIOUS_TRADER_5      2029
#define TMN_GUN_ANNOYED_WITH_TRADER_1     2030
#define TMN_GUN_ANNOYED_WITH_TRADER_2     2031
#define TMN_GUN_ANNOYED_WITH_TRADER_3     2032
#define TMN_GUN_ANNOYED_WITH_TRADER_4     2033
#define TMN_GUN_ANNOYED_WITH_TRADER_5     2034
#define TMN_GUN_CANT_SUGGEST_TRADE        2080
#define TMN_GUN_NOT_INVOLVED_IN_TRADE     2081
#define TMN_GUN_ASK_ME_LATER              2082

#define TMN_CAR_TRADE_COUNTER_1           2100
#define TMN_CAR_TRADE_COUNTER_2           2101
#define TMN_CAR_TRADE_COUNTER_3           2102
#define TMN_CAR_TRADE_COUNTER_4           2103
#define TMN_CAR_TRADE_COUNTER_5           2104
#define TMN_CAR_TRADE_REJECT_1            2105
#define TMN_CAR_TRADE_REJECT_2            2106
#define TMN_CAR_TRADE_REJECT_3            2107
#define TMN_CAR_TRADE_REJECT_4            2108
#define TMN_CAR_TRADE_REJECT_5            2109
#define TMN_CAR_IMPATIENT_1               2110
#define TMN_CAR_IMPATIENT_2               2111
#define TMN_CAR_IMPATIENT_3               2112
#define TMN_CAR_IMPATIENT_4               2113
#define TMN_CAR_IMPATIENT_5               2114
#define TMN_CAR_OFFER_MONOPOLY_TRADE_1    2115
#define TMN_CAR_OFFER_MONOPOLY_TRADE_2    2116
#define TMN_CAR_OFFER_MONOPOLY_TRADE_3    2117
#define TMN_CAR_OFFER_MONOPOLY_TRADE_4    2118
#define TMN_CAR_OFFER_MONOPOLY_TRADE_5    2119
#define TMN_CAR_TOO_MANY_COUNTERS_1       2120
#define TMN_CAR_TOO_MANY_COUNTERS_2       2121
#define TMN_CAR_TOO_MANY_COUNTERS_3       2122
#define TMN_CAR_TOO_MANY_COUNTERS_4       2123
#define TMN_CAR_TOO_MANY_COUNTERS_5       2124
#define TMN_CAR_NOT_SERIOUS_TRADER_1      2125
#define TMN_CAR_NOT_SERIOUS_TRADER_2      2126
#define TMN_CAR_NOT_SERIOUS_TRADER_3      2127
#define TMN_CAR_NOT_SERIOUS_TRADER_4      2128
#define TMN_CAR_NOT_SERIOUS_TRADER_5      2129
#define TMN_CAR_ANNOYED_WITH_TRADER_1     2130
#define TMN_CAR_ANNOYED_WITH_TRADER_2     2131
#define TMN_CAR_ANNOYED_WITH_TRADER_3     2132
#define TMN_CAR_ANNOYED_WITH_TRADER_4     2133
#define TMN_CAR_ANNOYED_WITH_TRADER_5     2134
#define TMN_CAR_CANT_SUGGEST_TRADE        2180
#define TMN_CAR_NOT_INVOLVED_IN_TRADE     2181
#define TMN_CAR_ASK_ME_LATER              2182

#define TMN_DOG_TRADE_COUNTER_1               2200
#define TMN_DOG_TRADE_COUNTER_2               2201
#define TMN_DOG_TRADE_COUNTER_3               2202
#define TMN_DOG_TRADE_COUNTER_4               2203
#define TMN_DOG_TRADE_COUNTER_5               2204
#define TMN_DOG_TRADE_REJECT_1                2205
#define TMN_DOG_TRADE_REJECT_2                2206
#define TMN_DOG_TRADE_REJECT_3                2207
#define TMN_DOG_TRADE_REJECT_4                2208
#define TMN_DOG_TRADE_REJECT_5                2209
#define TMN_DOG_IMPATIENT_1                   2210
#define TMN_DOG_IMPATIENT_2                   2211
#define TMN_DOG_IMPATIENT_3                   2212
#define TMN_DOG_IMPATIENT_4                   2213
#define TMN_DOG_IMPATIENT_5                   2214
#define TMN_DOG_OFFER_MONOPOLY_TRADE_1        2215
#define TMN_DOG_OFFER_MONOPOLY_TRADE_2        2216
#define TMN_DOG_OFFER_MONOPOLY_TRADE_3        2217
#define TMN_DOG_OFFER_MONOPOLY_TRADE_4        2218
#define TMN_DOG_OFFER_MONOPOLY_TRADE_5        2219
#define TMN_DOG_TOO_MANY_COUNTERS_1           2220
#define TMN_DOG_TOO_MANY_COUNTERS_2           2221
#define TMN_DOG_TOO_MANY_COUNTERS_3           2222
#define TMN_DOG_TOO_MANY_COUNTERS_4           2223
#define TMN_DOG_TOO_MANY_COUNTERS_5           2224
#define TMN_DOG_NOT_SERIOUS_TRADER_1          2225
#define TMN_DOG_NOT_SERIOUS_TRADER_2          2226
#define TMN_DOG_NOT_SERIOUS_TRADER_3          2227
#define TMN_DOG_NOT_SERIOUS_TRADER_4          2228
#define TMN_DOG_NOT_SERIOUS_TRADER_5          2229
#define TMN_DOG_ANNOYED_WITH_TRADER_1         2230
#define TMN_DOG_ANNOYED_WITH_TRADER_2         2231
#define TMN_DOG_ANNOYED_WITH_TRADER_3         2232
#define TMN_DOG_ANNOYED_WITH_TRADER_4         2233
#define TMN_DOG_ANNOYED_WITH_TRADER_5         2234
#define TMN_DOG_CANT_SUGGEST_TRADE            2280
#define TMN_DOG_NOT_INVOLVED_IN_TRADE         2281
#define TMN_DOG_ASK_ME_LATER                  2282

#define TMN_HAT_TRADE_COUNTER_1             2300
#define TMN_HAT_TRADE_COUNTER_2             2301
#define TMN_HAT_TRADE_COUNTER_3             2302
#define TMN_HAT_TRADE_COUNTER_4             2303
#define TMN_HAT_TRADE_COUNTER_5             2304
#define TMN_HAT_TRADE_REJECT_1              2305
#define TMN_HAT_TRADE_REJECT_2              2306
#define TMN_HAT_TRADE_REJECT_3              2307
#define TMN_HAT_TRADE_REJECT_4              2308
#define TMN_HAT_TRADE_REJECT_5              2309
#define TMN_HAT_IMPATIENT_1                 2310
#define TMN_HAT_IMPATIENT_2                 2311
#define TMN_HAT_IMPATIENT_3                 2312
#define TMN_HAT_IMPATIENT_4                 2313
#define TMN_HAT_IMPATIENT_5                 2314
#define TMN_HAT_OFFER_MONOPOLY_TRADE_1      2315
#define TMN_HAT_OFFER_MONOPOLY_TRADE_2      2316
#define TMN_HAT_OFFER_MONOPOLY_TRADE_3      2317
#define TMN_HAT_OFFER_MONOPOLY_TRADE_4      2318
#define TMN_HAT_OFFER_MONOPOLY_TRADE_5      2319
#define TMN_HAT_TOO_MANY_COUNTERS_1         2320
#define TMN_HAT_TOO_MANY_COUNTERS_2         2321
#define TMN_HAT_TOO_MANY_COUNTERS_3         2322
#define TMN_HAT_TOO_MANY_COUNTERS_4         2323
#define TMN_HAT_TOO_MANY_COUNTERS_5         2324
#define TMN_HAT_NOT_SERIOUS_TRADER_1        2325
#define TMN_HAT_NOT_SERIOUS_TRADER_2        2326
#define TMN_HAT_NOT_SERIOUS_TRADER_3        2327
#define TMN_HAT_NOT_SERIOUS_TRADER_4        2328
#define TMN_HAT_NOT_SERIOUS_TRADER_5        2329
#define TMN_HAT_ANNOYED_WITH_TRADER_1       2330
#define TMN_HAT_ANNOYED_WITH_TRADER_2       2331
#define TMN_HAT_ANNOYED_WITH_TRADER_3       2332
#define TMN_HAT_ANNOYED_WITH_TRADER_4       2333
#define TMN_HAT_ANNOYED_WITH_TRADER_5       2334
#define TMN_HAT_CANT_SUGGEST_TRADE          2380
#define TMN_HAT_NOT_INVOLVED_IN_TRADE       2381
#define TMN_HAT_ASK_ME_LATER                2382

#define TMN_IRON_TRADE_COUNTER_1                2400
#define TMN_IRON_TRADE_COUNTER_2                2401
#define TMN_IRON_TRADE_COUNTER_3                2402
#define TMN_IRON_TRADE_COUNTER_4                2403
#define TMN_IRON_TRADE_COUNTER_5                2404
#define TMN_IRON_TRADE_REJECT_1                 2405
#define TMN_IRON_TRADE_REJECT_2                 2406
#define TMN_IRON_TRADE_REJECT_3                 2407
#define TMN_IRON_TRADE_REJECT_4                 2408
#define TMN_IRON_TRADE_REJECT_5                 2409
#define TMN_IRON_IMPATIENT_1                    2410
#define TMN_IRON_IMPATIENT_2                    2411
#define TMN_IRON_IMPATIENT_3                    2412
#define TMN_IRON_IMPATIENT_4                    2413
#define TMN_IRON_IMPATIENT_5                    2414
#define TMN_IRON_OFFER_MONOPOLY_TRADE_1         2415
#define TMN_IRON_OFFER_MONOPOLY_TRADE_2         2416
#define TMN_IRON_OFFER_MONOPOLY_TRADE_3         2417
#define TMN_IRON_OFFER_MONOPOLY_TRADE_4         2418
#define TMN_IRON_OFFER_MONOPOLY_TRADE_5         2419
#define TMN_IRON_TOO_MANY_COUNTERS_1            2420
#define TMN_IRON_TOO_MANY_COUNTERS_2            2421
#define TMN_IRON_TOO_MANY_COUNTERS_3            2422
#define TMN_IRON_TOO_MANY_COUNTERS_4            2423
#define TMN_IRON_TOO_MANY_COUNTERS_5            2424
#define TMN_IRON_NOT_SERIOUS_TRADER_1           2425
#define TMN_IRON_NOT_SERIOUS_TRADER_2           2426
#define TMN_IRON_NOT_SERIOUS_TRADER_3           2427
#define TMN_IRON_NOT_SERIOUS_TRADER_4           2428
#define TMN_IRON_NOT_SERIOUS_TRADER_5           2429
#define TMN_IRON_ANNOYED_WITH_TRADER_1          2430
#define TMN_IRON_ANNOYED_WITH_TRADER_2          2431
#define TMN_IRON_ANNOYED_WITH_TRADER_3          2432
#define TMN_IRON_ANNOYED_WITH_TRADER_4          2433
#define TMN_IRON_ANNOYED_WITH_TRADER_5          2434
#define TMN_IRON_CANT_SUGGEST_TRADE             2480
#define TMN_IRON_NOT_INVOLVED_IN_TRADE          2481
#define TMN_IRON_ASK_ME_LATER                   2482

#define TMN_HORSE_TRADE_COUNTER_1            2500
#define TMN_HORSE_TRADE_COUNTER_2            2501
#define TMN_HORSE_TRADE_COUNTER_3            2502
#define TMN_HORSE_TRADE_COUNTER_4            2503
#define TMN_HORSE_TRADE_COUNTER_5            2504
#define TMN_HORSE_TRADE_REJECT_1             2505
#define TMN_HORSE_TRADE_REJECT_2             2506
#define TMN_HORSE_TRADE_REJECT_3             2507
#define TMN_HORSE_TRADE_REJECT_4             2508
#define TMN_HORSE_TRADE_REJECT_5             2509
#define TMN_HORSE_IMPATIENT_1                2510
#define TMN_HORSE_IMPATIENT_2                2511
#define TMN_HORSE_IMPATIENT_3                2512
#define TMN_HORSE_IMPATIENT_4                2513
#define TMN_HORSE_IMPATIENT_5                2514
#define TMN_HORSE_OFFER_MONOPOLY_TRADE_1     2515
#define TMN_HORSE_OFFER_MONOPOLY_TRADE_2     2516
#define TMN_HORSE_OFFER_MONOPOLY_TRADE_3     2517
#define TMN_HORSE_OFFER_MONOPOLY_TRADE_4     2518
#define TMN_HORSE_OFFER_MONOPOLY_TRADE_5     2519
#define TMN_HORSE_TOO_MANY_COUNTERS_1        2520
#define TMN_HORSE_TOO_MANY_COUNTERS_2        2521
#define TMN_HORSE_TOO_MANY_COUNTERS_3        2522
#define TMN_HORSE_TOO_MANY_COUNTERS_4        2523
#define TMN_HORSE_TOO_MANY_COUNTERS_5        2524
#define TMN_HORSE_NOT_SERIOUS_TRADER_1       2525
#define TMN_HORSE_NOT_SERIOUS_TRADER_2       2526
#define TMN_HORSE_NOT_SERIOUS_TRADER_3       2527
#define TMN_HORSE_NOT_SERIOUS_TRADER_4       2528
#define TMN_HORSE_NOT_SERIOUS_TRADER_5       2529
#define TMN_HORSE_ANNOYED_WITH_TRADER_1      2530
#define TMN_HORSE_ANNOYED_WITH_TRADER_2      2531
#define TMN_HORSE_ANNOYED_WITH_TRADER_3      2532
#define TMN_HORSE_ANNOYED_WITH_TRADER_4      2533
#define TMN_HORSE_ANNOYED_WITH_TRADER_5      2534
#define TMN_HORSE_CANT_SUGGEST_TRADE         2580
#define TMN_HORSE_NOT_INVOLVED_IN_TRADE      2581
#define TMN_HORSE_ASK_ME_LATER               2582

#define TMN_SHIP_TRADE_COUNTER_1                2600
#define TMN_SHIP_TRADE_COUNTER_2                2601
#define TMN_SHIP_TRADE_COUNTER_3                2602
#define TMN_SHIP_TRADE_COUNTER_4                2603
#define TMN_SHIP_TRADE_COUNTER_5                2604
#define TMN_SHIP_TRADE_REJECT_1                 2605
#define TMN_SHIP_TRADE_REJECT_2                 2606
#define TMN_SHIP_TRADE_REJECT_3                 2607
#define TMN_SHIP_TRADE_REJECT_4                 2608
#define TMN_SHIP_TRADE_REJECT_5                 2609
#define TMN_SHIP_IMPATIENT_1                    2610
#define TMN_SHIP_IMPATIENT_2                    2611
#define TMN_SHIP_IMPATIENT_3                    2612
#define TMN_SHIP_IMPATIENT_4                    2613
#define TMN_SHIP_IMPATIENT_5                    2614
#define TMN_SHIP_OFFER_MONOPOLY_TRADE_1         2615
#define TMN_SHIP_OFFER_MONOPOLY_TRADE_2         2616
#define TMN_SHIP_OFFER_MONOPOLY_TRADE_3         2617
#define TMN_SHIP_OFFER_MONOPOLY_TRADE_4         2618
#define TMN_SHIP_OFFER_MONOPOLY_TRADE_5         2619
#define TMN_SHIP_TOO_MANY_COUNTERS_1            2620
#define TMN_SHIP_TOO_MANY_COUNTERS_2            2621
#define TMN_SHIP_TOO_MANY_COUNTERS_3            2622
#define TMN_SHIP_TOO_MANY_COUNTERS_4            2623
#define TMN_SHIP_TOO_MANY_COUNTERS_5            2624
#define TMN_SHIP_NOT_SERIOUS_TRADER_1           2625
#define TMN_SHIP_NOT_SERIOUS_TRADER_2           2626
#define TMN_SHIP_NOT_SERIOUS_TRADER_3           2627
#define TMN_SHIP_NOT_SERIOUS_TRADER_4           2628
#define TMN_SHIP_NOT_SERIOUS_TRADER_5           2629
#define TMN_SHIP_ANNOYED_WITH_TRADER_1          2630
#define TMN_SHIP_ANNOYED_WITH_TRADER_2          2631
#define TMN_SHIP_ANNOYED_WITH_TRADER_3          2632
#define TMN_SHIP_ANNOYED_WITH_TRADER_4          2633
#define TMN_SHIP_ANNOYED_WITH_TRADER_5          2634
#define TMN_SHIP_CANT_SUGGEST_TRADE             2680
#define TMN_SHIP_NOT_INVOLVED_IN_TRADE          2681
#define TMN_SHIP_ASK_ME_LATER                   2682

#define TMN_SHOE_TRADE_COUNTER_1             2700
#define TMN_SHOE_TRADE_COUNTER_2             2701
#define TMN_SHOE_TRADE_COUNTER_3             2702
#define TMN_SHOE_TRADE_COUNTER_4             2703
#define TMN_SHOE_TRADE_COUNTER_5             2704
#define TMN_SHOE_TRADE_REJECT_1              2705
#define TMN_SHOE_TRADE_REJECT_2              2706
#define TMN_SHOE_TRADE_REJECT_3              2707
#define TMN_SHOE_TRADE_REJECT_4              2708
#define TMN_SHOE_TRADE_REJECT_5              2709
#define TMN_SHOE_IMPATIENT_1                 2710
#define TMN_SHOE_IMPATIENT_2                 2711
#define TMN_SHOE_IMPATIENT_3                 2712
#define TMN_SHOE_IMPATIENT_4                 2713
#define TMN_SHOE_IMPATIENT_5                 2714
#define TMN_SHOE_OFFER_MONOPOLY_TRADE_1      2715
#define TMN_SHOE_OFFER_MONOPOLY_TRADE_2      2716
#define TMN_SHOE_OFFER_MONOPOLY_TRADE_3      2717
#define TMN_SHOE_OFFER_MONOPOLY_TRADE_4      2718
#define TMN_SHOE_OFFER_MONOPOLY_TRADE_5      2719
#define TMN_SHOE_TOO_MANY_COUNTERS_1         2720
#define TMN_SHOE_TOO_MANY_COUNTERS_2         2721
#define TMN_SHOE_TOO_MANY_COUNTERS_3         2722
#define TMN_SHOE_TOO_MANY_COUNTERS_4         2723
#define TMN_SHOE_TOO_MANY_COUNTERS_5         2724
#define TMN_SHOE_NOT_SERIOUS_TRADER_1        2725
#define TMN_SHOE_NOT_SERIOUS_TRADER_2        2726
#define TMN_SHOE_NOT_SERIOUS_TRADER_3        2727
#define TMN_SHOE_NOT_SERIOUS_TRADER_4        2728
#define TMN_SHOE_NOT_SERIOUS_TRADER_5        2729
#define TMN_SHOE_ANNOYED_WITH_TRADER_1       2730
#define TMN_SHOE_ANNOYED_WITH_TRADER_2       2731
#define TMN_SHOE_ANNOYED_WITH_TRADER_3       2732
#define TMN_SHOE_ANNOYED_WITH_TRADER_4       2733
#define TMN_SHOE_ANNOYED_WITH_TRADER_5       2734
#define TMN_SHOE_CANT_SUGGEST_TRADE          2780
#define TMN_SHOE_NOT_INVOLVED_IN_TRADE       2781
#define TMN_SHOE_ASK_ME_LATER                2782

#define TMN_THIMBLE_TRADE_COUNTER_1             2800
#define TMN_THIMBLE_TRADE_COUNTER_2             2801
#define TMN_THIMBLE_TRADE_COUNTER_3             2802
#define TMN_THIMBLE_TRADE_COUNTER_4             2803
#define TMN_THIMBLE_TRADE_COUNTER_5             2804
#define TMN_THIMBLE_TRADE_REJECT_1              2805
#define TMN_THIMBLE_TRADE_REJECT_2              2806
#define TMN_THIMBLE_TRADE_REJECT_3              2807
#define TMN_THIMBLE_TRADE_REJECT_4              2808
#define TMN_THIMBLE_TRADE_REJECT_5              2809
#define TMN_THIMBLE_IMPATIENT_1                 2810
#define TMN_THIMBLE_IMPATIENT_2                 2811
#define TMN_THIMBLE_IMPATIENT_3                 2812
#define TMN_THIMBLE_IMPATIENT_4                 2813
#define TMN_THIMBLE_IMPATIENT_5                 2814
#define TMN_THIMBLE_OFFER_MONOPOLY_TRADE_1      2815
#define TMN_THIMBLE_OFFER_MONOPOLY_TRADE_2      2816
#define TMN_THIMBLE_OFFER_MONOPOLY_TRADE_3      2817
#define TMN_THIMBLE_OFFER_MONOPOLY_TRADE_4      2818
#define TMN_THIMBLE_OFFER_MONOPOLY_TRADE_5      2819
#define TMN_THIMBLE_TOO_MANY_COUNTERS_1         2820
#define TMN_THIMBLE_TOO_MANY_COUNTERS_2         2821
#define TMN_THIMBLE_TOO_MANY_COUNTERS_3         2822
#define TMN_THIMBLE_TOO_MANY_COUNTERS_4         2823
#define TMN_THIMBLE_TOO_MANY_COUNTERS_5         2824
#define TMN_THIMBLE_NOT_SERIOUS_TRADER_1        2825
#define TMN_THIMBLE_NOT_SERIOUS_TRADER_2        2826
#define TMN_THIMBLE_NOT_SERIOUS_TRADER_3        2827
#define TMN_THIMBLE_NOT_SERIOUS_TRADER_4        2828
#define TMN_THIMBLE_NOT_SERIOUS_TRADER_5        2829
#define TMN_THIMBLE_ANNOYED_WITH_TRADER_1       2830
#define TMN_THIMBLE_ANNOYED_WITH_TRADER_2       2831
#define TMN_THIMBLE_ANNOYED_WITH_TRADER_3       2832
#define TMN_THIMBLE_ANNOYED_WITH_TRADER_4       2833
#define TMN_THIMBLE_ANNOYED_WITH_TRADER_5       2834
#define TMN_THIMBLE_CANT_SUGGEST_TRADE          2880
#define TMN_THIMBLE_NOT_INVOLVED_IN_TRADE       2881
#define TMN_THIMBLE_ASK_ME_LATER                2882

#define TMN_BARROW_TRADE_COUNTER_1             2900
#define TMN_BARROW_TRADE_COUNTER_2             2901
#define TMN_BARROW_TRADE_COUNTER_3             2902
#define TMN_BARROW_TRADE_COUNTER_4             2903
#define TMN_BARROW_TRADE_COUNTER_5             2904
#define TMN_BARROW_TRADE_REJECT_1              2905
#define TMN_BARROW_TRADE_REJECT_2              2906
#define TMN_BARROW_TRADE_REJECT_3              2907
#define TMN_BARROW_TRADE_REJECT_4              2908
#define TMN_BARROW_TRADE_REJECT_5              2909
#define TMN_BARROW_IMPATIENT_1                 2910
#define TMN_BARROW_IMPATIENT_2                 2911
#define TMN_BARROW_IMPATIENT_3                 2912
#define TMN_BARROW_IMPATIENT_4                 2913
#define TMN_BARROW_IMPATIENT_5                 2914
#define TMN_BARROW_OFFER_MONOPOLY_TRADE_1      2915
#define TMN_BARROW_OFFER_MONOPOLY_TRADE_2      2916
#define TMN_BARROW_OFFER_MONOPOLY_TRADE_3      2917
#define TMN_BARROW_OFFER_MONOPOLY_TRADE_4      2918
#define TMN_BARROW_OFFER_MONOPOLY_TRADE_5      2919
#define TMN_BARROW_TOO_MANY_COUNTERS_1         2920
#define TMN_BARROW_TOO_MANY_COUNTERS_2         2921
#define TMN_BARROW_TOO_MANY_COUNTERS_3         2922
#define TMN_BARROW_TOO_MANY_COUNTERS_4         2923
#define TMN_BARROW_TOO_MANY_COUNTERS_5         2924
#define TMN_BARROW_NOT_SERIOUS_TRADER_1        2925
#define TMN_BARROW_NOT_SERIOUS_TRADER_2        2926
#define TMN_BARROW_NOT_SERIOUS_TRADER_3        2927
#define TMN_BARROW_NOT_SERIOUS_TRADER_4        2928
#define TMN_BARROW_NOT_SERIOUS_TRADER_5        2929
#define TMN_BARROW_ANNOYED_WITH_TRADER_1       2930
#define TMN_BARROW_ANNOYED_WITH_TRADER_2       2931
#define TMN_BARROW_ANNOYED_WITH_TRADER_3       2932
#define TMN_BARROW_ANNOYED_WITH_TRADER_4       2933
#define TMN_BARROW_ANNOYED_WITH_TRADER_5       2934
#define TMN_BARROW_CANT_SUGGEST_TRADE          2980
#define TMN_BARROW_NOT_INVOLVED_IN_TRADE       2981
#define TMN_BARROW_ASK_ME_LATER                2982

#define TMN_MONEYBAG_TRADE_COUNTER_1             3000
#define TMN_MONEYBAG_TRADE_COUNTER_2             3001
#define TMN_MONEYBAG_TRADE_COUNTER_3             3002
#define TMN_MONEYBAG_TRADE_COUNTER_4             3003
#define TMN_MONEYBAG_TRADE_COUNTER_5             3004
#define TMN_MONEYBAG_TRADE_REJECT_1              3005
#define TMN_MONEYBAG_TRADE_REJECT_2              3006
#define TMN_MONEYBAG_TRADE_REJECT_3              3007
#define TMN_MONEYBAG_TRADE_REJECT_4              3008
#define TMN_MONEYBAG_TRADE_REJECT_5              3009
#define TMN_MONEYBAG_IMPATIENT_1                 3010
#define TMN_MONEYBAG_IMPATIENT_2                 3011
#define TMN_MONEYBAG_IMPATIENT_3                 3012
#define TMN_MONEYBAG_IMPATIENT_4                 3013
#define TMN_MONEYBAG_IMPATIENT_5                 3014
#define TMN_MONEYBAG_OFFER_MONOPOLY_TRADE_1      3015
#define TMN_MONEYBAG_OFFER_MONOPOLY_TRADE_2      3016
#define TMN_MONEYBAG_OFFER_MONOPOLY_TRADE_3      3017
#define TMN_MONEYBAG_OFFER_MONOPOLY_TRADE_4      3018
#define TMN_MONEYBAG_OFFER_MONOPOLY_TRADE_5      3019
#define TMN_MONEYBAG_TOO_MANY_COUNTERS_1         3020
#define TMN_MONEYBAG_TOO_MANY_COUNTERS_2         3021
#define TMN_MONEYBAG_TOO_MANY_COUNTERS_3         3022
#define TMN_MONEYBAG_TOO_MANY_COUNTERS_4         3023
#define TMN_MONEYBAG_TOO_MANY_COUNTERS_5         3024
#define TMN_MONEYBAG_NOT_SERIOUS_TRADER_1        3025
#define TMN_MONEYBAG_NOT_SERIOUS_TRADER_2        3026
#define TMN_MONEYBAG_NOT_SERIOUS_TRADER_3        3027
#define TMN_MONEYBAG_NOT_SERIOUS_TRADER_4        3028
#define TMN_MONEYBAG_NOT_SERIOUS_TRADER_5        3029
#define TMN_MONEYBAG_ANNOYED_WITH_TRADER_1       3030
#define TMN_MONEYBAG_ANNOYED_WITH_TRADER_2       3031
#define TMN_MONEYBAG_ANNOYED_WITH_TRADER_3       3032
#define TMN_MONEYBAG_ANNOYED_WITH_TRADER_4       3033
#define TMN_MONEYBAG_ANNOYED_WITH_TRADER_5       3034
#define TMN_MONEYBAG_CANT_SUGGEST_TRADE          3080
#define TMN_MONEYBAG_NOT_INVOLVED_IN_TRADE       3081
#define TMN_MONEYBAG_ASK_ME_LATER                3082



/* Stuff for compatability with the new language system,
   which the HotSeat itself currently doesn't use.*/

//#if FORHOTSEAT
  #define ATR_NO_MESSAGE                          TMN_NO_MESSAGE
  #define ATR_PlaceName_00                        TMN_SQUARENAME_GO
  #define T_MESS_ERROR_LOBBY_CONNECT_FAILED       TMN_ERROR_LOBBY_CONNECT_FAILED
  #define T_MESS_ERROR_LOBBY_APP_GUID             TMN_ERROR_LOBBY_APP_GUID
  #define T_MESS_ERROR_DPM_CANT_CREATE            TMN_ERROR_DPM_CANT_CREATE
  #define T_MESS_ERROR_DPM_NO_SERVER_PLAYER       TMN_ERROR_DPM_NO_SERVER_PLAYER
  #define T_MESS_ERROR_DPM_CANT_OPEN              TMN_ERROR_DPM_CANT_OPEN
  #define T_MESS_ERROR_DPM_CANT_CONNECT           TMN_ERROR_DPM_CANT_CONNECT
  #define T_MESS_ERROR_DPM_PLAYER_CREATE          TMN_ERROR_DPM_PLAYER_CREATE
  #define T_MESS_ERROR_DPNAMESET                  TMN_ERROR_DPNAMESET
  #define T_MESS_ERROR_DP_SESSION_LOST            TMN_ERROR_DP_SESSION_LOST
  #define T_MESS_WSOCK_NOT_AVAILABLE              TMN_WSOCK_NOT_AVAILABLE
  #define T_MESS_ERROR_WS_CONNECT_ACCEPT_FAILURE  TMN_ERROR_WS_CONNECT_ACCEPT_FAILURE
  #define T_MESS_ARGS_SERVER                      TMN_ARGS_SERVER
  #define T_MESS_ARGS_CLIENT                      TMN_ARGS_CLIENT
  #define T_MESS_ARGS_DIRECTPLAY                  TMN_ARGS_DIRECTPLAY
  #define T_MESS_ARGS_SOCKET                      TMN_ARGS_SOCKET
  #define T_MESS_ARGS_IP_ADDRESS                  TMN_ARGS_IP_ADDRESS
  #define T_MESS_ARGS_TCP_PORT                    TMN_ARGS_TCP_PORT
  #define T_MESS_ARGS_NETSYS_CHOOSE               TMN_ARGS_NETSYS_CHOOSE
  #define T_MESS_ARGS_PLAYERNAME                  TMN_ARGS_PLAYERNAME
  #define T_MESS_BUTTON_CANCEL                    TMN_BUTTON_CANCEL
  #define T_MESS_BUTTON_OK                        TMN_BUTTON_OK
  #define T_MESS_BOX_SERVER_TITLE                 TMN_BOX_SERVER_TITLE
  #define T_MESS_BOX_SERVER_PROMPT                TMN_BOX_SERVER_PROMPT
  #define T_MESS_BOX_SERVER_SERVER_BUTTON         TMN_BOX_SERVER_SERVER_BUTTON
  #define T_MESS_BOX_SERVER_CLIENT_BUTTON         TMN_BOX_SERVER_CLIENT_BUTTON
  #define T_MESS_DP_SESSION_NAME_UNKNOWN          TMN_DP_SESSION_NAME_UNKNOWN
  #define T_MESS_DP_USER_NAME_UNKNOWN             TMN_DP_USER_NAME_UNKNOWN
  #define T_MESS_BOX_DP_SERVICE_TITLE             TMN_BOX_DP_SERVICE_TITLE
  #define T_MESS_BOX_DP_SESSION_TITLE             TMN_BOX_DP_SESSION_TITLE
  #define T_MESS_BOX_GETIP_TITLE                  TMN_BOX_GETIP_TITLE
  #define T_MESS_BOX_GETIP_PROMPT                 TMN_BOX_GETIP_PROMPT
  #define T_MESS_BOX_SHOWIP_TITLE                 TMN_BOX_SHOWIP_TITLE
  #define T_MESS_BOX_SHOWIP_PROMPT                TMN_BOX_SHOWIP_PROMPT
  #define T_MESS_BOX_NETSYS_TITLE                 TMN_BOX_NETSYS_TITLE
  #define T_MESS_BOX_NETSYS_PROMPT                TMN_BOX_NETSYS_PROMPT
  #define T_MESS_BOX_NETSYS_DIRECTPLAY            TMN_BOX_NETSYS_DIRECTPLAY
  #define T_MESS_BOX_NETSYS_WINSOCK               TMN_BOX_NETSYS_WINSOCK
  #define T_MESS_BOX_NETSYS_PORT                  TMN_BOX_NETSYS_PORT
  #define T_MESS_OPT3_monopoly_star_wars          TMN_MESSAGE_BOX_TITLE
//#endif /* FORHOTSEAT */

/************************************************************/

#endif /* __LANGIDS_H__ */
