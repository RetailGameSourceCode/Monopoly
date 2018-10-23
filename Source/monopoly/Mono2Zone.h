/*****************************************************************************
 * FILE:        Mono2Zone.h
 *
 * DESCRIPTION: Headers for the data Monopoly 2000 exchanges with the
 *              Microsoft Zone lobby system.  Things like high score lists.
 *
 * © Copyright 1999 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Projects - 1999/Mono 2 Hotseat/Mono2Zone.h 4     99/09/02 5:23p Agmsmith $
 *****************************************************************************
 * $Log: /Projects - 1999/Mono 2 Hotseat/Mono2Zone.h $
 * 
 * 4     99/09/02 5:23p Agmsmith
 *
 * 3     99/09/01 7:06p Agmsmith
 * Comment updated.
 *
 * 2     99/09/01 2:28p Agmsmith
 * Comment changed.
 *
 * 1     99/08/30 2:42p Agmsmith
 * Information exchanged with the Microsoft Zone lobby.
 */

#ifndef MONO2ZONE_H
#define MONO2ZONE_H

// Zone needs to know your byte alignment
#pragma pack( push, 1 )



// The GUID for the Monopoly 2000 game as a whole.
// {00EC277C-3B19-48b6-AA11-04229BEA7717}



// Game update information - sent at the start of every player's turn and
// after every movement, and also finally when the game is over.  This
// structure is sent to the lobby with a message GUID of
// {6F403220-C5E1-4cb2-A51D-BE0EE512B65C}, see also ZoneGameStatusUpdateGUID
// which is a global containing that GUID.

// Maximum of 6 players.
#if RULE_MAX_PLAYERS != 6
  #error Maximum players doesn't match the value given to the Microsoft Zone people.
#endif

// Length of player's names in wide characters, not including NUL at end is 39.
#if RULE_MAX_PLAYER_NAME_LENGTH != 39
  #error Player name length doesn't match the value given to the Microsoft Zone people.
#endif

typedef enum MESS_ZoneGameStatusEnum
{
  GAME_RUNNING = 0,
  GAME_OVER_BY_SHORT_GAME_RULES, // Richest person at second bankruptcy.
  GAME_OVER_BY_TIME_LIMIT, // Richest person when time ran out.
  GAME_OVER_NORMAL, // Last person standing.
  GAME_STATUS_MAX
} MESS_ZoneGameStatus;

typedef struct MESS_ZoneGameUpdateInfoStruct
{
  unsigned char numberOfPlayers;
  unsigned char currentPlayer; // And the winner if the game is over.
  unsigned char gameStatus; // See values from MESS_ZoneGameStatus.
  unsigned char language; // Which language the server player is using.  Values to be defined...
  struct PlayerStruct
  {
    wchar_t       name[RULE_MAX_PLAYER_NAME_LENGTH + 1];
    DWORD         cash; // In dollars, multiply by currency conversion factor for others.
    DWORD         netWealth; // Cash and properties combined.
    unsigned char currentSquare; // 0 is GO to 39 for Boardwalk, 40 is in jail, others mean off board (bankrupt).
    unsigned char jailCards; // Number of get out of jail cards the player has (0 to 2).
    unsigned char token; // Which token the player is using.  GUN = 0, CAR, DOG, HAT, IRON, HORSE, SHIP, SHOE, THIMBLE, BARROW, MONEYBAG.
    unsigned char aiPlayer; // Non-zero if this is an AI player (actually the AI level), zero for humans.
  } players [RULE_MAX_PLAYERS];

  struct SquareStruct
  {
    unsigned char owner; // Player index or RULE_MAX_PLAYERS and above for unowned.
    signed char   houses; // Number of houses on the square, or -1 for a hotel, 0 for nothing.
    unsigned char mortgaged; // Non-zero if mortgaged.
    unsigned char ownable; // Just to make the whole structure a DWORD.  Non-zero if square can be owned.
  } squares [40];

} MESS_ZoneGameUpdateInfoRecord, *MESS_ZoneGameUpdateInfoPointer;



#pragma pack( pop )

#endif // MONO2ZONE_H
