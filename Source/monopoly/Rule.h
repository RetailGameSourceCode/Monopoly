#ifndef __RULE_H__
#define __RULE_H__

/*************************************************************
 *
 *   FILE:             RULE.H
 *
 *   (C) Copyright 97 Artech Digital Entertainments.
 *                    All rights reserved.
 *
 *   Monopoly Game File Location Header.
 *
 *   Rules engine public definitions.
 *
 * $Header: /Projects - 1999/Mono 2/Rule.h 24    10/13/99 10:00a Russellk $
 *
 * $Log: /Projects - 1999/Mono 2/Rule.h $
 * 
 * 24    10/13/99 10:00a Russellk
 * comment changed
 * 
 * 23    10/07/99 1:19p Davide
 * changed the order of the enum for types of futures and immunities hits
 * and also got rid of the specifiying the number of bits per member.  It
 * was giving the game a false impression that an immunitie was set for
 * the first player when loading a game
 * 
 * 22    10/05/99 3:33p Philla
 * New function for detecting "prompt" message type.
 * 
 * 21    29/09/99 1:43p Agmsmith
 * Now send a 4 byte unique network ID of the original sender along with
 * every chat message, so you can determine the sending computer (not just
 * player).
 *
 * 20    26/09/99 2:10p Agmsmith
 * Removed old voice chat system, in preparation for new one.
 *
 * 19    25/09/99 2:40p Agmsmith
 * Comment correction.
 *
 * 18    23/09/99 1:04p Agmsmith
 * Changed get out of jail rules to match the official rules - roll up to
 * 3 times and if you fail to get doubles on the last roll, it will ask
 * you again if you want to pay or use the card.
 *
 * 17    22/09/99 4:55p Agmsmith
 * Nothing.
 *
 * 16    9/07/99 3:48p Philla
 * Live update of rules screen for non-host players.
 *
 * 15    8/09/99 12:11p Mikeh
 * Added function RULE_PlayersFirstMoveWasMade(int player) to
 * return players firstmove status.
 *
 * 14    7/05/99 11:23a Russellk
 * Comment changed
 *
 * 13    7/02/99 6:01p Russellk
 * First pass alterations for BSSM state - numberD flags using a new
 * quicky mode (no lock required, works despite an existing lock).  Our
 * new interface is clunky with the old methods - repeatedly denying
 * requests - this method will correct that and let us leave the AI code
 * alone.
 *
 * 12    6/21/99 3:52p Davide
 * added a member to the RULE_SquareInfoStruct to keep tabs on the total
 * game earnings for each square. The variable is incremented every time a
 * player lands on a square (CollectRent function)
 *
 * 11    6/11/99 9:40a Russellk
 * Changed to 6 colors, 11 tokens.
 *
 * 10    6/02/99 8:48a Davide
 * added a NOTIFY message to help with removing a configuration token
 *
 * 9     5/27/99 3:31p Russellk
 * Removed old variable which made trades send only once - we only send
 * once now anyway, though AI's seem to have trouble now.  If so, they
 * need to be fixed as well - they seemed to send a trade edit done prior
 * to sending the trade items which might have worked with duplicates
 * being sent.
 *
 * 8     5/27/99 3:14p Russellk
 *
 * 7     5/21/99 3:55p Russellk
 * All initial trade function changed to non-locking
 *
 * 6     4/01/99 16:04 Davew
 * Fixed minor colour problems
 *
 * 5     3/12/99 4:10p Russellk
 * Trade screen started, HIDE functions removed - setting the background
 * to black does the whole job anyway.
 *
 * 4     2/12/99 1:36p Russellk
 * PropertySets used for property bar, added mortgaged properties logic
 *
 * 3     2/09/99 11:34a Russellk
 * comments only
 *
 * 2     2/08/99 9:11a Russellk
 * Added state NOTIFY_TURN_END - a rather stupid end of turn
 * acknowledgement.  Useful for email monopoly, Hasbro wants it in at this
 * point.
 *
 * 1     1/08/99 2:06p Russellk
 *
 * 1     1/08/99 11:35a Russellk
 * Graphics
 *
 * 5     12/23/98 3:18p Russellk
 * Changes in comments only
 *
 * 4     12/18/98 4:02p Russellk
 * Changed to use 10 tokens instead of SW 8 - wheelbarrow AI file added
 * and bug fixed for AI filenames (no comma in file name list left tenth
 * pointer null).  Comments changed slightly to look a bit more like
 * monopoly tokens instead of SW tokens.
 *
 * 3     12/17/98 4:27p Russellk
 * Obsolete comments removed for new version.  Log file facility entered
 * in Mess.c to record phase and action/error message activity.
 *
 ************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/

#if FORHOTSEAT
  #define strncpyFixed lstrcpyn
  #define memcpyFixed hmemcpy
  typedef HGLOBAL RULE_CharHandle;
  typedef HGLOBAL CharHandle;
  #define RULE_AllocateHandle(x) GlobalAlloc (GHND, x)
  #define AllocateHandle(x) GlobalAlloc (GHND, x)
  #define RULE_FreeHandle(x) GlobalFree (x)
  #define FreeHandle(x) GlobalFree (x)
  #define RULE_HandleSize(x) GlobalSize (x)
  #define HandleSize(x) GlobalSize (x)
  #define RULE_UnlockHandle(x) GlobalUnlock (x)
  #define UnlockHandle(x) GlobalUnlock (x)
  #define RULE_LockHandle(x) GlobalLock (x)
  #define LockHandle(x) GlobalLock (x)
  #define RULE_PointerToHandle(x) GlobalHandle (x)
  #define RULE_ResizeHandle(x, NewSize) GlobalReAlloc (x, NewSize, GMEM_MOVEABLE | GMEM_ZEROINIT)
#else /* Artech library version.  Bleeble - to do. */
  typedef HGLOBAL RULE_CharHandle;
  #define RULE_AllocateHandle(x) GlobalAlloc (GHND, x)
  #define RULE_FreeHandle(x) GlobalFree (x)
  #define RULE_HandleSize(x) GlobalSize (x)
  #define RULE_UnlockHandle(x) GlobalUnlock (x)
  #define RULE_LockHandle(x) GlobalLock (x)
  #define RULE_PointerToHandle(x) GlobalHandle (x)
  #define RULE_ResizeHandle(x, NewSize) GlobalReAlloc (x, NewSize, GMEM_MOVEABLE | GMEM_ZEROINIT)
#endif
  /* Whatever kind of memory handle you are using and the operations
  for allocating and freeing it. */


#define RULE_MAX_PLAYERS 6
  /* Maximum number of players in the game.  There can of course be
  many spectators, but they don't count.  Note that storage space
  grows as a square of this number, and time for some operations too
  (trading for example).  But that shouldn't be much of a problem
  under 100 players. */
#define RULE_MAX_PLAYERS_CHOOSE_2 15
  // Easier than writing the function.  For trade array.  n!/((n-c)!c!)=720/(24*2)=720/48 = 15

enum RULE_TokenKindEnum
{
  TK_GUN = 0, /* An artillery piece. */
  TK_CAR,     /* Old style racing car. */
  TK_DOG,     /* Some kind of terrier? */
  TK_HAT,     /* A top hat. */
  TK_IRON,    /* An old style cloths iron. */
  TK_HORSE,   /* A rocking horse or horse and rider. */
  TK_SHIP,    /* A battleship. */
  TK_SHOE,    /* An old shoe. */
  TK_THIMBLE, /* Sewing thimble. */
  TK_BARROW,  /* A garden wheelbarrow. */
  TK_MONEYBAG,/* A Money Bag. */
  MAX_TOKENS,

  /* Star Wars equivalent tokens */
//  TK_SW_BOBA_FETT = 0,
//  TK_SW_CHEWBACCA,
//  TK_SW_DARTH_VADER,
//  TK_SW_HAN_SOLO,
//  TK_SW_LUKE,
//  TK_SW_PRINCESS_LEIA,
//  TK_SW_R2D2,
//  TK_SW_STORMTOOPER,
//  MAX_SW_TOKENS
};
typedef int RULE_TokenKind;
  /* The different shapes that the players can choose from. */

enum RULE_PlayerColoursEnum
{
  PC_RED = 0,
  PC_BLUE,
  PC_GREEN,
  PC_YELLOW,
  PC_MAGENTA,
  PC_ORANGE,
  /*PC_WHITE,
  PC_CYAN,
  PC_PINK,
  PC_BLACK,*/
  MAX_PLAYER_COLOURS
};
typedef int RULE_PlayerColour;
  /* This identifies the predefined colours that a player can choose
  for marking their squares as owned by them.  Currently there need
  to be as many colours as tokens (colour same as token number). */

typedef unsigned char RULE_PlayerNumber;
  /* A convenient small sized data type for identifying players, varies
  from 0 to RULE_MAX_PLAYERS - 1 for players, other values for nobody and the
  bank. */

typedef unsigned char RULE_PlayerSet;
#if RULE_MAX_PLAYERS >= 8
  #error "Increase the size of RULE_PlayerSet to hold all the players!"
#endif
  /* A set of players.  Bit 1<<PlayerNumber is 1 if that player is in the
  set and 0 if the player is out.  The bank player is included in the set,
  so need RULE_MAX_PLAYERS+1 bits in the storage space. */

#define RULE_BANK_PLAYER ((RULE_PlayerNumber) RULE_MAX_PLAYERS + 0)
  /* Special code for the bank.  It's a player number which specifies
  that the bank owns something.  Used after a player has gone bankrupt
  to mark his old properties as being up for auction.  Should come
  sequentially after the players so that the communications can check
  for valid player & bank numbers easily. */

#define RULE_NOBODY_PLAYER ((RULE_PlayerNumber) RULE_MAX_PLAYERS + 1)
#define RULE_ALL_PLAYERS RULE_NOBODY_PLAYER
  /* Special code for no player at all.  It's a player number which
  specifies nobody.  Used in squares to show whom they are being traded
  to, RULE_NOBODY_PLAYER means the square isn't being traded.  Also stands for
  all player in networking.  Also unowned squares belong to nobody. */

#define RULE_ESCROW_PLAYER ((RULE_PlayerNumber) RULE_MAX_PLAYERS + 2)
  /* Properties are marked as being owned by the Escrow Player when
  a trade is being resolved.  That way, players can't mortgage them
  while resolving the trade's debts.  Same thing for buying a property,
  it is held in escrow until it is paid for (so the player can't mortgage
  it to pay for it). */

#define RULE_SPECTATOR_PLAYER ((RULE_PlayerNumber) RULE_MAX_PLAYERS + 3)
  /* Special code for non-players who are watching the game.  They
  may chat with the players, thus the need for this code.  They don't
  have individual identities. */

enum RULE_SquareTypeEnum
{
  SQ_GO = 0,
  SQ_MEDITERRANEAN_AVENUE,
  SQ_COMMUNITY_CHEST_1,
  SQ_BALTIC_AVENUE,
  SQ_INCOME_TAX,
  SQ_READING_RR,
  SQ_ORIENTAL_AVENUE,
  SQ_CHANCE_1,
  SQ_VERMONT_AVENUE,
  SQ_CONNECTICUT_AVENUE,
  SQ_JUST_VISITING,  /* The outside of the jail square, see also SQ_IN_JAIL. */
  SQ_ST_CHARLES_PLACE,
  SQ_ELECTRIC_COMPANY,
  SQ_STATES_AVENUE,
  SQ_VIRGINIA_AVENUE,
  SQ_PENNSYLVANIA_RR,
  SQ_ST_JAMES_PLACE,
  SQ_COMMUNITY_CHEST_2,
  SQ_TENNESSEE_AVENUE,
  SQ_NEW_YORK_AVENUE,
  SQ_FREE_PARKING,
  SQ_KENTUCKY_AVENUE,
  SQ_CHANCE_2,
  SQ_INDIANA_AVENUE,
  SQ_ILLINOIS_AVENUE,
  SQ_BnO_RR,
  SQ_ATLANTIC_AVENUE,
  SQ_VENTNOR_AVENUE,
  SQ_WATER_WORKS,
  SQ_MARVIN_GARDENS,
  SQ_GO_TO_JAIL,
  SQ_PACIFIC_AVENUE,
  SQ_NORTH_CAROLINA_AVENUE,
  SQ_COMMUNITY_CHEST_3,
  SQ_PENNSYLVANIA_AVENUE,
  SQ_SHORT_LINE_RR,
  SQ_CHANCE_3,
  SQ_PARK_PLACE,
  SQ_LUXURY_TAX,
  SQ_BOARDWALK,
  SQ_IN_JAIL,  /* The inside part of the in-jail square, see also SQ_JUST_VISITING. */
  SQ_OFF_BOARD, /* If the player has been removed - gone bankrupt. */
  SQ_MAX_SQUARE_TYPES,

  /* Some special codes used when auctioning to describe non-square items. */
  SQ_AUCTION_HOUSE = SQ_IN_JAIL,
  SQ_AUCTION_HOTEL = SQ_OFF_BOARD,
  SQ_AUCTION_COMMUNITY_JAIL, /* A get out of jail free card.  Not used. */
  SQ_AUCTION_CHANCE_JAIL  /* The other card.  Not used. */
};
typedef int RULE_SquareType;

#define SQ_TOTAL_PROPERTY_SQUARES 28
  /* Definition for the total count of the squares on the board that are
  property squares (ie, those who have a deed card). */

typedef unsigned long RULE_PropertySet;
  /* Identifies a set of property type squares using bitfield techniques.
  Bit 0 (least significant) stands for Mediterranean Avenue, bit 1 for
  Baltic Avenue, bit 2 for Reading Railroad and so on.  There are utility
  functions for converting from bits to RULE_SquareType values and vice versa. */

enum RULE_SquareGroupsEnum
{
  SG_MEDITERRANEAN_AVENUE = 0,
  SG_ORIENTAL_AVENUE,
  SG_ST_CHARLES_PLACE,
  SG_ST_JAMES_PLACE,
  SG_KENTUCKY_AVENUE,
  SG_ATLANTIC_AVENUE,
  SG_PACIFIC_AVENUE,
  SG_PARK_PLACE,
  SG_RAILROAD,
  SG_UTILITY,
  SG_MAX_PROPERTY_GROUPS, /* A divider between properties and the rest. */
  SG_COMMUNITY_CHEST = SG_MAX_PROPERTY_GROUPS,
  SG_CHANCE,
  SG_MISCELLANEOUS,
  SG_MAX
};
typedef int RULE_SquareGroups;
  /* The various different groups of properties, named by the first group in
  the property.  There are also groups for non-properties.  If you own all of
  a group (called a monopoly) then you can collect more rent. */



enum RULE_CardTypeEnum
{
  NOT_A_CARD = 0, /* For those no card here situations. */
  CHANCE_GO_DIRECTLY_TO_GO,
  CHANCE_FIRST = CHANCE_GO_DIRECTLY_TO_GO,
  CHANCE_GO_TO_ST_CHARLES_PLACE,
  CHANCE_GO_TO_NEAREST_UTILITY,
  CHANCE_GET_150_FROM_BANK,
  CHANCE_GET_50_FROM_BANK,
  CHANCE_PAY_50_TO_EACH_PLAYER,
  CHANCE_GO_TO_BOARDWALK,
  CHANCE_GO_TO_READING_RAILROAD,
  CHANCE_GO_TO_NEAREST_RAILROAD_PAY_DOUBLE_1,
  CHANCE_GO_TO_NEAREST_RAILROAD_PAY_DOUBLE_2,
  CHANCE_PAY_25_EACH_HOUSE_100_EACH_HOTEL,
  CHANCE_GET_OUT_OF_JAIL_FREE,
  CHANCE_PAY_15_TO_BANK,
  CHANCE_GO_TO_ILLINOIS_AVENUE,
  CHANCE_GO_DIRECTLY_TO_JAIL,
  CHANCE_GO_BACK_THREE_SPACES,
  CHANCE_MAX,
  COMMUNITY_GET_100_FROM_BANK_1 = 65, /* Leave a gap for new cards. */
  COMMUNITY_FIRST = COMMUNITY_GET_100_FROM_BANK_1,
  COMMUNITY_GET_100_FROM_BANK_2,
  COMMUNITY_GET_100_FROM_BANK_3,
  COMMUNITY_GET_10_FROM_BANK,
  COMMUNITY_GET_OUT_OF_JAIL_FREE,
  COMMUNITY_PAY_50_TO_BANK,
  COMMUNITY_GET_200_FROM_BANK,
  COMMUNITY_GET_20_FROM_BANK,
  COMMUNITY_PAY_40_EACH_HOUSE_115_EACH_HOTEL,
  COMMUNITY_GO_DIRECTLY_TO_JAIL,
  COMMUNITY_GET_45_FROM_BANK,
  COMMUNITY_GET_50_FROM_EACH_PLAYER,
  COMMUNITY_GO_DIRECTLY_TO_GO,
  COMMUNITY_PAY_150_TO_BANK,
  COMMUNITY_GET_25_FROM_BANK,
  COMMUNITY_PAY_100_TO_BANK,
  COMMUNITY_MAX,
  CARD_TYPE_MAX = COMMUNITY_MAX
};
typedef int RULE_CardType;

#define CHANCE_SIZE (CHANCE_MAX - CHANCE_FIRST)
#define COMMUNITY_SIZE (COMMUNITY_MAX - COMMUNITY_FIRST)
#define MAX_CARDS_IN_DECK ((CHANCE_SIZE > COMMUNITY_SIZE) ? CHANCE_SIZE : COMMUNITY_SIZE)
  /* The larger of the two deck sizes. */

typedef struct RULE_CardDeckStruct
{
  unsigned char cardCount;
    /* The number of cards in this deck or pile.  Varies from 0 to
    MAX_CARDS_IN_DECK - 1.  Note that the player can keep some cards,
    so you won't always have all the cards in the deck.  Also some
    countries don't use all the cards. */

  unsigned char cardPile [MAX_CARDS_IN_DECK];
    /* The cards are stored here.  The card at the top of the deck is at
    cardPile [0], the bottom one is at cardPile [cardCount-1].  The contents
    of the array element identify which card is at that position, see the
    RULE_CardType enum for the particular values.  Adding a card to the bottom
    of the deck is done by incrementing the cardCount and putting the card
    at the end.  Removing a card from the top is done by moving all the
    cards up one spot and decementing the cardCount (slow but easy to make
    bug free). */

  RULE_PlayerNumber jailOwner;
    /* Identifies who owns the get out of jail free card when it is
    out of the deck.  RULE_NOBODY_PLAYER when the card is in the deck. */

  RULE_PlayerNumber jailOfferedInTradeTo;
    /* Who the get out of jail free card is being offered to during a
    trade.  RULE_NOBODY_PLAYER if it isn't being traded or isn't owned. */

} RULE_CardDeckRecord, *RULE_CardDeckPointer;

enum RULE_CardDeckTypesEnum
{
  CHANCE_DECK = 0,
  COMMUNITY_DECK,
  MAX_DECK_TYPES
};
typedef int RULE_CardDeckTypes;
  /* For identifying the different card decks in the game.  Can be
  used as an array index. */



enum RULE_TradeItemKindEnum
{
  TIK_CASH = 0,
    /* The item being traded is cash, NumberD is the amount. */

  TIK_SQUARE,
    /* Trading a property square, NumberD is the RULE_SquareType. */

  TIK_JAIL_CARD,
    /* Trading jail cards, NumberD is a RULE_CardDeckTypes. */

  TIK_IMMUNITY,
    /* Trades an immunity from rent for the To player when he lands on squares
    in the RULE_PropertySet in NumberD, but only when the From player owns the
    square.  NumberE contains the number of hits, use zero to clear the
    immunity from the trading list, negative to signal cancelling or reducing
    a pre-existing immunity. */

  TIK_FUTURE_RENT,
    /* Trades future rent from the From player to the To player for rent paid
    when anyone lands on any of the listed squares, for a certain number of
    hits.  In the ACTION_TRADE_ITEM message the squares are a RULE_PropertySet
    in the NumberD Amount and the hit count is in NumberE.  In other words,
    when someone lands on one of the squares specified, and the owner is the
    From player, then the rent is collected by the owner as usual and then
    transfered to the person who bought the future from the owner.  Of course,
    if the future holder himself landed on the square, he doesn't have to pay
    anything and one hit is used up.  If the hit count is zero, it clears the
    future from the trading list.  Negative hit counts signal cancelling or
    reducing an existing future (the squares have to match up exactly with
    the existing one, otherwise it doesn't get registered as a trade item). */

  TIK_MAX
};
typedef int RULE_TradeItemKind;
  /* The different kinds of things that can be traded. */



#define RULE_MAX_PLAYER_NAME_LENGTH 39
  /* Length of player's names in characters, not including NUL at end. */

typedef struct RULE_PlayerInfoStruct
{
  long cash;
    /* Number of dollars or credits that the player has.  This dollar
    value is converted to the local currency by multiplying by a constant
    before being displayed.  Makes multiplayer international games easier. */

  DWORD timeOfLastActivity;
    /* Used for seeing if a player is idle for too long.  Copied from
    GameDurationInSeconds whenever they do an action.  See also
    inactivityCount and inactivityWarningTime. */

  wchar_t name [RULE_MAX_PLAYER_NAME_LENGTH + 1];
    /* Name of the player, in Unicode character set. */

  RULE_TokenKind token;
    /* Which token is this player using to move around the board? */

  RULE_PlayerColour colour;
    /* What colour the player wants to use for their ownership marks. */

  RULE_SquareType currentSquare;
    /* Where the player is on the board.  If it is SQ_OFF_BOARD then the
    player has been removed from the game, usually because he is
    bankrupt (the token also isn't drawn in this case).  He's also off the
    board before the game starts.  There used to be a special move from off
    to on board on his first move, but now the players start on GO all the
    time, and there is a flag that tells if the player has moved yet. */

  unsigned char firstMoveMade;
    /* TRUE once the first move has been made.  Just used to trigger the
    graphical effect that shows the player walking from off the board to
    the GO square (but he is really on the GO square all the time). */

  unsigned char turnsInJail;
    /* Number of turns the player has spent in jail, so far.  Set to zero
    on the turn when he goes to jail.  Set to 99 when you hit the last
    turn and must pay right away to get out, or use a jail card.  That
    way it can go back into the roll or pay decision and ask you again
    but with the roll option disabled. */

  unsigned char AIPlayerLevel;
    /* Nonzero if this is an AI player, which means it can be replaced by a human
    player who wants to join the game.  Zero if it is a human.  Actually, it
    indicates the level of the AI player, from 1 to AI_MAX_LEVELS. */

  unsigned char inactivityCount;
    /* Counts the number of warnings issued about the player being
    idle.  Zeroed when the player does something.  See also timeOfLastActivity. */

  union phaseDependentUnion /* The values in here are used in different phases. */
  {
    struct startingStruct
    {
      BOOL acceptedConfiguration;
        /* Set to TRUE when this player agrees to the game option
        configuration settings, during the GF_CONFIGURATION phase. */
    } starting;

    struct orderingStruct
    {
      DWORD diceRollHistory;
        /* When deciding who goes first, the players roll dice.  Ones with
        equal dice rolls do a roll-off between themselves.  This variable
        records all the rolls made, the first roll is in bits 31, 30, 29, 28.
        The next is in the next batch of 4 insignificant bits and so on down
        to the 8th roll being stored in the low 4 bits.  The values of the
        numbers are then sorted so that the higher number goes first. */
    } ordering;

    struct tradingStruct
    {
      long cashGivenInTrade [RULE_MAX_PLAYERS];
        /* Amount of cash that this player will offer to every other
        player in a trade.  Zero for players he isn't trading with. */

      BOOL tradeAccepted;
        /* TRUE once the player has accepted the trade currently on the
        table.  FALSE when we haven't heard from that player yet. */
    } trading;

  } shared; /* For shared, recycled storage space between different phases. */

} RULE_PlayerInfoRecord, *RULE_PlayerInfoPointer;



typedef struct RULE_SquareInfoStruct
{
  RULE_PlayerNumber owner;
    /* Who owns this square.  If it is RULE_BANK_PLAYER then this square will
    be put up for auction at the next opportunity.  If it is RULE_NOBODY_PLAYER
    then nobody owns it.  If it is RULE_ESCROW_PLAYER then it is being held
    in escrow until the trade or purchase is resolved, then it will be
    given to the player specified by offeredInTradeTo. */

  RULE_PlayerNumber offeredInTradeTo;
    /* Who the current owner is offering this square to in trade for
    something else.  Set to RULE_NOBODY_PLAYER if it isn't being traded.
    Also used for properties held in escrow to identify whom the property
    will eventually be given to. */

  unsigned char houses;
    /* Number of houses built on this square.  Varies from 0 to
    HousesPerHotel, with HousesPerHotel representing a hotel instead
    of a house.  This can also be used as an index to the rent array in
    the RULE_SquarePredefinedInfo. */

  BOOL mortgaged;
    /* TRUE if the property is mortgaged.  Note that if it is traded while
    mortgaged then an interest fee is charged when it is handed over (see
    the RULE_ESCROW_PLAYER stuff), */

    unsigned long game_earnings;
        /* holds the cumulative value of each time a player lands on the square
        (ignored if player owns the square already). Used mainly for the deed
        status screen, for the sort by game earnings function.*/

} RULE_SquareInfoRecord, *RULE_SquareInfoPointer;



#define RULE_MAX_SQUARE_NAME_LENGTH 39
  /* Length of square's names in characters, not including NUL at end. */

#define RULE_MAX_HOUSES_PER_SQUARE 5
  /* Maximum number of houses you can have on a square, well this is
  usually a hotel and not a house, but you get the idea. */

typedef struct RULE_SquarePredefinedInfoStruct
{
  long purchaseCost;
    /* How much it costs to buy this square. */

  long mortgageCost;
    /* How much you get when you mortgage this square. */

  long rent [RULE_MAX_HOUSES_PER_SQUARE + 1];
    /* Amount of money to charge for renting this square with the index
    being the number of houses on the square.  HousesPerHotel is really
    a hotel, not a house, but we just treat it like a bunch of houses
    except during display. */

  long housePurchaseCost;
    /* How much does it cost to buy one more house? */

  wchar_t *squareName;
    /* Name of this square in the local country's language.  This string is
    actually in the text messages database, but we put a pointer here
    for convenience. */

  RULE_SquareGroups group;
    /* Identifies the property or other group that this square is in. */

} RULE_SquarePredefinedInfoRecord, *RULE_SquarePredefinedInfoPointer;


enum RULE_ActionTypeEnum
{
  ACTION_NULL = 0,                 /* 0 */
    /* A non-action.  Does nothing. */

  ACTION_DISCONNECTED_PLAYER,         /* 1 */
    /* The network connection to the player in numberA has been lost.
    This message is generated internally by the communications module
    when that happens.  The player should be replaced by a standard
    AI player that can then be replaced at a later time by another
    network player.  NumberB specifies the desired toughness level of
    the AI. */

  ACTION_TICK,                  /* 2 */
    /* A clock tick has gone by.  This action checks for time passing
    during auctions and other states where time is important.  Happens
    at most once per second if no other actions are pending (the automatic
    timer doesn't generate this action if the action queue has something
    in it, which happens if new actions arrive at 20 per second or faster). */

  ACTION_RANDOM_SEED,             /* 3 */
    /* Set the random number seed to the value in numberA.  For debugging. */

  ACTION_RESTART_PHASE,             /* 4 */
    /* After an interrupted phase (like when a trade interrupts someone's
    dice roll) this action can be sent to make the phase restart (the dice
    rolling phase will again tell the player to roll the dice).  Also useable
    from clients for resending the notification for the current phase (which
    pops up the dialog box for that phase). */

  ACTION_RESYNC_CLIENT,             /* 5 */
    /* Some client computer (numberA has player ID or RULE_SPECTATOR_PLAYER),
    needs to be updated with the entire game state (perhaps it is a spectator
    that joined a game in progress or a system that had a communications
    error).  First, a NOTIFY_NUMBER_OF_PLAYERS is sent.  It is followed by
    NOTIFY_NAME_PLAYER for all players.  After that comes a
    NOTIFY_CLIENT_RESYNC_INFO with the entire game state that the client needs
    to know.  Then finally an internal ACTION_RESTART_PHASE is done to resend
    the current prompt. */

  ACTION_NEW_GAME,                /* 6 */
    /* Starting a new game.  The players have to sign up, agree to a
    configuration, play the game, etc.  This starts it all.  Initialises
    things to default values and waits for players to add themselves.
    If NumberA is TRUE then the same players as in the previous game
    are reused. */

  ACTION_GET_GAME_STATE_FOR_SAVE,       /* 7 */
    /* The player wants to get a copy of the current game state.  A
    NOTIFY_GAME_STATE_FOR_SAVE will be sent back, if available (can't
    save during auctions or trades - an error message will be issued
    if you request the save state at the wrong time).  Only players
    can request it, spectators are ignored. */

  ACTION_SET_GAME_STATE,            /* 8 */
    /* The player wants to use a saved game they have just loaded.  The
    saved data blob is stored in binaryDataHandleA as a RIFF formatted file.
    If it does work, it will start a new game, notify people about the game
    options in the data and send NOTIFY_FORCE_LOCAL_PLAYER messages for all
    players to create local players.  NumberC is a version number, the same
    one as in ACTION_ACCEPT_CONFIGURATION, but isn't used. */

  ACTION_NAME_PLAYER,             /* 9 */
    /* Players send this message to the server to sign up for a game.
    The player's name is stored in the stringA field as a string.  The
    suggested player number is in numberA: RULE_NOBODY_PLAYER if this is
    for a new player; an existing number if you want to rename a player
    or change its AI flag (taking over an AI player in other words),
    rename to the empty string to stop playing that player (not recommended
    during game play, also see NOTIFY_PLAYER_DELETED).  NumberB contains an
    AI level number and is non-zero if the player is an AI (which means that
    it can be bumped off by another player, it doesn't keep the slot if
    someone else wants to use it).  NumberC is the desired token and NumberD
    is the desired colour (not used when renaming the player).  If the player
    is accepted (name not already in use in another slot, free slot available
    for new ones, etc) then a NOTIFY_NAME_PLAYER reply will be sent.
    NOTIFY_ERROR_MESSAGE with TMN_ERROR_NAME_IN_USE is sent when you try
    to add a player with the same name as an existing one,
    TMN_ERROR_NO_MORE_PLAYERS is sent when there are no free slots for a
    new player. TMN_ERROR_NOT_YOUR_PLAYER is used for attempts to rename
    someone else's player.  See also NOTIFY_ADD_LOCAL_PLAYER and
    NOTIFY_PLEASE_ADD_PLAYERS. */

  ACTION_ACCEPT_CONFIGURATION,          /* 10 */
    /* All the players have to send this message when they want to change the
    rules at the start of the game.  The configuration they accepted, or the
    new one they propose, is a RIFF file in the binaryDataHandleA and
    associated blob fields (see RULE_ConvertFileToGameOptions()).  If it
    doesn't match the server's configuration, then it replaces the current
    config and everybody has to agree all over again to the new one (also see
    the NOTIFY_PROPOSED_CONFIGURATION action).  If the rules engine doesn't
    expect a configuration from the player, it will be ignored.  NumberC is
    a game client version number: 0 for the original Monopoly Star Wars PC
    game (immunities and futures not supported) and 1 for the updated version
    that has immunities and futures.  NumberD is 0 for final configuration,
    or 1 for intermediary, just letting everyone know temporary changes. */

  ACTION_START_GAME,              /* 11 */
    /* Start playing the game.  Done after all the people have signed up
    to play the game.  Does a vote on the configuration.  Once everyone
    has accepted, the game really starts. */

  ACTION_START_TURN,              /* 12 */
    /* It is the start of a new turn.  This action resets various
    counters (double roll count) and prepares for the turn. */

  ACTION_END_TURN,                /* 13 */
    /* The current player has finished their turn.  Pass the dice onto
    the next player. */

  ACTION_ROLL_DICE,               /* 14 */
    /* This is sent by the player when he wants to roll the dice.  The
    validation stage should make sure that it is the player's turn and
    he is in the dice rolling phase.  This can also be used for rolling
    dice when trying to get out of jail or for that card that asks you
    to roll for the utility rent. */

  ACTION_CHEAT_ROLL_DICE,           /* 15 */
    /* This is sent by the player when he wants to roll the dice and
    specify the dice roll.  NumberA is one die and NumberB is the other. */

  ACTION_CHEAT_CASH,              /* 16 */
    /* Change player NumberA's cash to the value in NumberB. */

  ACTION_CHEAT_OWNER,             /* 17 */
    /* Player numberA becomes the new owner of square NumberB, or if it is
    already owned by him then it becomes unowned. */

  ACTION_KILL_AUCTION_CHEAT,      /* 18 */
    /* A cheat code that ends the auction right now.  Speeds up debugging. */

  ACTION_MOVE_FORWARDS,           /* 19 */
    /* Move current player forwards to square numberA.  If they pass GO then
    they collect $200.  An internal action. */

  ACTION_MOVE_BACKWARDS,          /* 20 */
    /* Move current player backwards to square numberA.  An internal action. */

  ACTION_JUMP_TO_SQUARE,          /* 21 */
    /* Move current player directly to square numberA.  An internal action. */

  ACTION_LANDED_ON_SQUARE,        /* 22 */
    /* The current player has landed on the current square.  Do whatever
    processing is needed for that particular square then ask for another
    dice roll or end the player's turn (depending on state etc).  An
    internal action. */

  ACTION_EXIT_JAIL_DECISION,      /* 23 */
    /* The player sends this when he decides how to get out of jail.  The player
    is implicitly the sender (and current player).  NumberA is 0 for rolling the
    dice, 1 for paying the $50 and 2 for using the card.  See
    NOTIFY_JAIL_EXIT_CHOICE for more. */

  ACTION_CARD_SEEN,               /* 24 */
    /* Sent when a player has finished looking at a community chest or chance
    card.  The card (top of the pile corresponding to the current player's
    square) is now processed.  See also NOTIFY_PICKED_UP_CARD. */

  ACTION_GO_BANKRUPT,             /* 25 */
    /* The player this message is from has decided to go bankrupt over
    the current debt.  Only accepted if the player doesn't have enough
    net wealth to cover the debt. */

  ACTION_BUY_OR_AUCTION_DECISION, /* 26 */
    /* The current player has landed on a square and has decided whether to buy
    the property or to let the bank auction it off.  NumberA is TRUE for buy,
    FALSE for auction.  See NOTIFY_BUY_OR_AUCTION_DECISION. */

  ACTION_BID,                     /* 27 */
    /* The player wants to bid on the currently offered property or building.
    The amount of the bid is in NumberA. */

  ACTION_FREE_UNMORTGAGE_DONE,    /* 28 */
    /* The player has finished unmortgaging properties that were transfered
    in a trade or bankruptcy.  See NOTIFY_FREE_UNMORTGAGING. */

  ACTION_BUY_HOUSE,               /* 29 */
    /* The fromPlayer wants to buy a single house (potentially upgrading the
    place to a hotel if it already has enough houses on it).  NumberA is the
    square where the new house will go.  Will be rejected with an error
    message if it fails the even build rule or if the player doesn't have
    enough cash or if there aren't any more houses (or hotels) available.
    The player should first do a ACTION_PLAYER_BUYSELLMORT to get the game
    into the buying mode (which may get rejected if something else is going
    on).  If there is a shortage of houses/hotels, the bank will ask if anyone
    else wants to buy the house (see NOTIFY_HOUSING_SHORTAGE), a response from
    another player within a few seconds will trigger an auction and eventually
    a NOTIFY_PLACE_BUILDING to put the building on a square. Set numberD TRUE
    to request a quick buy which uses logic similar to that which grants
    ACTION_PLAYER_BUYSELLMORT locks, but does the buy regardless of locks and
    without a phase switch.*/

  ACTION_SELL_BUILDINGS,          /* 30 */
    /* The player wants to sell a building on the square in NumberA.  If
    NumberB is TRUE then everything on the monopoly containing the square
    will be sold simultaneously (so you can sell hotels without going through
    the house stages during a building shortage).  If numberB is FALSE then a
    single house will be sold.  The player has to successfully do an
    ACTION_PLAYER_BUYSELLMORT before they can sell buildings, or be in
    decomposition mode unless numberD is TRUE requesting a quick sell. */

  ACTION_MORTGAGING,              /* 31 */
    /* The player wants to mortgage or unmortgage square NumberA.  As usual,
    the player needs a successful ACTION_PLAYER_BUYSELLMORT before doing it
    unless numberD is TRUE requesting a quick mortgage which is allowed
    in most states. */

  ACTION_TAX_DECISION,            /* 32 */
    /* The player has decided how to pay the taxes.  NumberA is TRUE for
    fractional taxes and FALSE for flat taxes. */

  ACTION_START_TRADE_EDITING,     /* 33 */
    /* The player sending this message is requesting the start of a trade
    or if one is in progress, he wants to edit it (allowed if the trade
    is in the accept phase and the player is one of the acceptors). */

  ACTION_CLEAR_TRADE_ITEMS,       /* 34 */
    /* Empties out the list of trade items in the current trade. */

  ACTION_TRADE_ITEM,              /* 35 */
    /* The fromPlayer wants to change the current trade.  The item being
    traded is from player NumberA to player NumberB.  NumberC describes the
    kind of item (see RULE_TradeItemKind) and NumberD describes the amount of
    the particular item.  Some items don't need a from player, like property
    squares and jail cards (the Engine will put in the correct one).  If you
    want to correct a trade, issue another ACTION_TRADE_ITEM for the
    corrected value (like a different cash value (trade zero to delete),
    or giving a property to someone else, or give it to the owner to delete
    that trade item). */

  ACTION_TRADE_EDITING_DONE,      /* 36 */
    /* The player sending this has finished editing the trade.  If NumberA is
    0 then he wants to put it up for acceptance by the other players,
    NumberB then is FALSE for a public trade (everybody has to accept) and
    TRUE for a private offer (only people involved are asked to accept).  If
    NumberA is 1 then he just wants to pass it on to player NumberB for
    editing.  Finally, if NumberA is 2 he wants to cancel the trade
    completely. */

  ACTION_TRADE_ACCEPT,            /* 37 */
    /* The player accepts the current trade or rejects it.  If NumberA is
    TRUE then it is accepted, FALSE to reject it and stop the trading.
    If he doesn't accept the trade, he could also use ACTION_START_TRADE
    to edit it instead of accepting/rejecting it.  The engine will also
    send back an error message if you accept a trade when it would make
    you go bankrupt (just you, other people get warned when
    they do their acceptances).  If you would go bankrupt, you also
    automatically get to edit the trade again, besides getting the error
    message. */

  ACTION_VOICE_CHAT,              /* 38 */
    /* The player wants to send the voice chat message to the player in
    NumberA, use RULE_ALL_PLAYERS to send to everybody including the
    spectators.  The voice data is in our custom chunky streaming format
    in blocks of at most 64K, stored in binaryDataHandleA.  The player
    sends this to RULE_BANK_PLAYER who then rebroadcasts it in a
    NOTIFY_VOICE_CHAT message to the appropriate players.  So, the
    host computer better have a fast network connection! */

  ACTION_TEXT_CHAT,               /* 39 */
    /* The player wants to send the text chat message to the player in
    NumberA, use RULE_ALL_PLAYERS to send to everybody including the
    spectators.  The NUL terminated text data in Unicode format is in
    binaryDataHandleA.  The player sends this to RULE_BANK_PLAYER who
    then rebroadcasts it in a NOTIFY_TEXT_CHAT message to the appropriate
    players.  If binaryDataHandleA is NULL then the precoded message
    number in NumberC is used instead (see TMN_FLUFF_*). */

  ACTION_GET_OPTIONS_FOR_SAVE,    /* 40 */
    /* The player wants to get a copy of the current game options.  A
    NOTIFY_OPTIONS_FOR_SAVE will be sent back with the options.
    The opposite operation is done with ACTION_ACCEPT_CONFIGURATION. */

  ACTION_AI_SAVE_PARAMETERS,      /* 41 */
    /* The attached blob contains a single RIFF chunk with the internal
    information for a single AI.  This information will be put in the save
    game file and used later when the game is loaded to restore the AI's
    state.  See NOTIFY_AI_NEED_PARAMETERS_FOR_SAVE and NOTIFY_AI_PARAMETERS. */

  ACTION_PLAYER_BUYSELLMORT,      /* 42 */
    /* The player sending this wants to get exclusive control for doing
    certain actions (buying buildings, selling buildings, mortgaging).
    He declares that he is working on the properties in NumberB (a
    RULE_PropertySet usually showing the properties in a monopoly
    group), but that is really only for display purposes.  If you use a value
    of zero for NumberB, it means that the player wants to do a quick
    buy/sell/mortgage without bringing up the big animation (used for AIs
    selling single houses).  You can send a second ACTION_PLAYER_BUYSELLMORT
    if you want to update the NumberB set.  See also NOTIFY_PLAYER_BUYSELLMORT
    and ACTION_PLAYER_DONE_BUYSELLMORT.
    NOTE that the Monopoly 2000 version introduces a quicky way to perform
    buy sell & mortgage functions which do not require exclusive control nor
    stop at someone else having exclusive control.  The lock is left to allow
    the AI code to continue unmodified.*/

  ACTION_PLAYER_DONE_BUYSELLMORT, /* 43 */
    /* The player has finished buying, selling and mortgaging.  Go back to
    the rest of the game. */

  ACTION_CANCEL_DECOMPOSITION,    /* 44 */
    /* The player doesn't want to sell any more houses for hotel
    decomposition during a NOTIFY_DECOMPOSE_SALE.  The hotel(s)
    will be put back to the way they were. */

  ACTION_START_HOUSING_AUCTION,   /* 45 */
    /* Some player objects to the house being sold during a shortage
    and requests an auction.  This message will be accepted only if the
    player has a spot for a new house or hotel. */

  ACTION_STAR_WARS_ANIMATION_INFO,/* 46 */
    /* This message is rebroadcast as NOTIFY_STAR_WARS_ANIMATION_INFO to all
    the players, contents unchanged (kind of like a chat).  The various fields
    are used by the Monopoly Star Wars game to show moving objects on the
    trade screen, and other user actions that aren't rules related.

    numberA: animation function index. See IFACE_ANIM... defines in iface.h
    Functions:
      IFACE_ANIME_TRADE_ITEM:
        - B:button id of item
          For trade tokens: C: source position, D: x-coord, E:dest position (1 to 8)
                    Use -1 for top-left corner, -2 for public, -3 for private
          Other trade items:
                   C: 0
                   D: x coord (-1 to remove)
                   E: y coord (-1 to remove)

      IFACE_ANIME_LOCAL_TRADE_ITEM: same as above, but not for broadcast over
                                    net. */

  ACTION_UPDATE_TRADE_INFO,   /* 47 FIXME - SWM specific, not used in Mono 2 (RK)*/
    /* This message is rebroadcast to all players, contents unchanged.
    Used for managing the trade display across the network.
    See NOTIFY_UPDATE_TRADE_INFO for an explanation of paramaters. */

  ACTION_PAUSE_GAME,          /* 48 */
    /* NumberA is TRUE to request a pause of the game, FALSE to unpause it.
    Anyone may pause or unpause it, except spectators.  Also see
    NOTIFY_GAME_PAUSED.  This is typically used to pause the game while
    someone edits local options. */

  ACTION_I_AM_HERE,           /* 49 */
    /* The players send this when they get a NOTIFY_ARE_YOU_THERE message.
    NumberA is the serial number that was in the NOTIFY_ARE_YOU_THERE.  It
    tells the rules engine that the player is still alive - useful for
    synchronizing the start of an auction and other things. */

  ACTION_CLEAR_TRADED_IMMUNITIES_OR_FUTURES, /* 50 */
    /* Remove all the immunities or futures currently being traded from
    player NumberA to player NumberB.  NumberC is CHT_RENT_IMMUNITY for
    immunities, CHT_FUTURE_RENT for futures. */



  NOTIFY_ERROR_MESSAGE = 80, /* Leave a gap for future actions */
    /* The error message number is in numberA.  Three optional numbers to
    be inserted in the message are in numberB, numberC and numberD, and
    an optional string is in stringA.  Human players see the message
    in their local language.  AIs ignore this notification most of the time,
    except for things like checking if their name request has failed.
    The error strings have special embedded codes that are expanded when the
    string is displayed.  The codes are made from a caret (^) and a following
    character:
      ^1 expands numberB as a number,
      ^2 expands numberC as a number,
      ^3 expands numberD as a number,
      ^4 expands numberE as a number,
      ^A expands stringA,
      ^P expands numberC as a player name,
      ^Q expands numberB as a player name,
      ^S expands numberD as a square name,
      ^T expands numberE as a set of squares. */

  NOTIFY_CLIENT_RESYNC_INFO,  /* 81 */
    /* The entire game state that is relevant to clients is being transfered.
    StringA contains a RULE_ClientResyncInfoRecord with the current game state.
    See ACTION_RESYNC_CLIENT for details. */

  NOTIFY_NUMBER_OF_PLAYERS,   /* 82 */
    /* Tells everyone how many players are currently signed up to play
    a game.  The number is in numberA.  If it is zero, then a new game
    is starting and the UI should wipe out all local players and prepare
    to add new ones (some done locally, some done with
    NOTIFY_FORCE_LOCAL_PLAYER). */

  NOTIFY_NAME_PLAYER,         /* 83 */
    /* Tells everybody that the player slot number in numberA is now assigned
    to someone named stringA.  The token choice is in numberB and the colour
    choice is in numberC.  NumberD is non-zero for an AI (actually it is the
    AI difficulty level number), zero (0) for a Human player.  AIs also look
    for this message when first signing up to see if they have signed up (they
    should check that the name is their name).  If you see this message for
    some other name than the one you are using for that player slot, remove
    your local player.  See also NOTIFY_ADD_LOCAL_PLAYER. */

  NOTIFY_ADD_LOCAL_PLAYER,    /* 84 */
    /* An old game has been loaded or a player has been dropped.  The user
    interface code on the server (not the clients) should add the player name
    in StringA to its list of local players that it will be handling (can be
    an AI or Human), if it isn't already there (please check!).  If you are
    a client computer with the same name (happens when loading a game but not
    during drops), deallocate your local name.  NumberA contains the future
    slot number (also the number of the player being replaced).
    A NOTIFY_NAME_PLAYER will be sent soon after to set the token, colour
    and AI levels. */

  NOTIFY_PROPOSED_CONFIGURATION,  /* 85 */
    /* The game sends this message to players when it is waiting to
    start a new game.  The proposed configuration is a RIFF file in
    the binaryDataHandleA and associated blob fields (see
    RULE_ConvertFileToGameOptions()).  NumberA identifies the
    player who proposed it, can be RULE_NOBODY_PLAYER.  NumberB is a
    RULE_PlayerSet that identifies the players whom we are still waiting for
    answers from (zero if this is the new options - used during resync and
    when voting is over - don't display any UI stuff when you get this with
    zero).  NumberC is a game host version number, 0 for the original
    Monopoly Star Wars PC game (immunities and futures not supported) and 1
    for the updated version that has immunities and futures.  The players
    listed in the set should reply back with an ACTION_ACCEPT_CONFIGURATION. */

  NOTIFY_GAME_STATE_FOR_SAVE, /* 86 */
    /* The player has sent an ACTION_GET_GAME_STATE_FOR_SAVE to get this
    notification.  The binary large object (blob) in binaryDataHandleA
    is a RIFF format saved game file reflecting the current game state.
    This message is sent only to the player who requested the save. */

  NOTIFY_END_TURN,            /* 87 */
    /* The player in numberA has completed their turn - acknowledge. */

  NOTIFY_START_TURN,          /* 88 */
    /* It is now the turn of the player in numberA. */

  NOTIFY_JAIL_EXIT_CHOICE,    /* 89 */
    /* The player in numberA has to choose how to get out of jail.
    If numberB is TRUE then he can attempt to roll doubles.  If NumberC is
    true then he can pay $50.  If NumberD is true then he can use a card.
    Naturally, trading is a possibility too (to get the $50 or the card).
    See also ACTION_EXIT_JAIL_DECISION for possible responses. */

  NOTIFY_PLEASE_ROLL_DICE,    /* 90 */
    /* It is time for player numberA to roll the dice.  For demo convenience,
    the square the player is currently on is in NumberB. */

  NOTIFY_DICE_ROLLED,         /* 91 */
    /* A player has rolled the dice.  The particular values rolled are
    in numberA and numberB.  The player who rolled is in numberC. */

  NOTIFY_MOVE_FORWARDS,       /* 92 */
    /* Move forwards to square numberA.  Previous square is in numberB.
    Player moving is in numberC (should be current player anyways).
    If the player is landing on a community chest or chance card, numberD
    will be a RULE_CardType identifying the card.  NumberE is TRUE if this
    is the very first move the player has made (so the walk-on animation
    can be played), FALSE the rest of the time (the first move is always
    a move forwards from GO so the other moves don't have this flag). */

  NOTIFY_MOVE_BACKWARDS,      /* 93 */
    /* Move backwards to square numberA.  Previous square is in numberB.
    Player moving is in numberC (should be current player anyways).
    If the player is landing on a community chest or chance card, numberD
    will be a RULE_CardType identifying the card. */

  NOTIFY_JUMP_TO_SQUARE,      /* 94 */
    /* Move directly to square numberA.  Previous square is in numberB.
    Player moving is in numberC (should be current player anyways).
    Does not pass go, this is a jump!  Usually a jump to in-jail square.
    Never to a community chest or chance card, so NumberD isn't used and
    will be zero. */

  NOTIFY_PASSED_GO,           /* 95 */
    /* In case you can't tell from the move forwards notification, this
    message will also be sent (after the forwards move) if the current
    player (in NumberA too) passes go. */

  NOTIFY_CASH_AMOUNT,         /* 96 */
    /* Player numberA has a different amount of cash now.  The change is
    in numberB (positive for an increase, negative for a decrease) and the
    new total is in numberC.  If the cash is hidden, the value is still
    passed here, the user interface is responsible for not displaying it. */

  NOTIFY_CASH_ANIMATION,      /* 97 */
    /* This message is used to trigger animations of cash being transfered.
    The usual NOTIFY_CASH_AMOUNTs are sent after this message for the new
    amount, sometimes several if several players are involved, which makes
    it hard to decide what to animate, thus this message.  NumberA is the From
    player (can be a player or RULE_BANK_PLAYER).  NumberB is the To player
    (same choices as From).  NumberC is the amount transfered.
    StringA contains a RULE_CashAnimationRecord which describes the amount
    of cash each player has after the transfer. */

  NOTIFY_PLEASE_PAY,          /* 98 */
    /* Player NumberA has a debt to player numberB (which can also be the
    bank) for the amount in numberC.  He still has to raise numberD in
    cash (above his existing cash) to meet it.  NumberE is TRUE if the
    player is allowed to go bankrupt, FALSE if he has enough assets to
    cover the debt. */

  NOTIFY_SQUARE_OWNERSHIP,    /* 99 */
    /* Square numberA has changed ownership.  It is now owned by player
    numberB (can be the bank or the escrow player).  It was previously
    owned by player numberC (also can be RULE_NOBODY_PLAYER with
    RULE_BANK_PLAYER as the previous owner if a property is auctioned
    and nobody buys).  Previous owner is RULE_ESCROW_PLAYER when
    properties transfered successfully (payment received). */

  NOTIFY_SQUARE_MORTGAGE,     /* 100 */
    /* Square numberA is mortgaged if numberB is TRUE, unmortgaged if
    numberB is FALSE (zero).  The prior mortgage state is sent in
    numberC (usually the opposite of the new one, but may not be if
    someone is refreshing their board). */

  NOTIFY_SQUARE_HOUSES,       /* 101 */
    /* Square NumberA has NumberB houses on it.  NumberC is the number
    of houses in a hotel, so show a hotel if NumberB == NumberC. */

  NOTIFY_JAIL_CARD_OWNERSHIP, /* 102 */
    /* Player numberA has now got a get out of jail free card.  If numberA
    is RULE_NOBODY_PLAYER then the card is back in the deck.  NumberB identifies
    the particular card deck, see RULE_CardDeckTypes for details. */

  NOTIFY_IMMUNITY_COUNT,      /* 103 */
    /* The player in numberA has numberB turns of free rent remaining.
    He got it by landing on square numberC (set to SQ_OFF_BOARD when
    this is just telling you about the current immunity status, not an actual
    hit being used).  He is getting immunity from player NumberD but only
    when NumberD owns the square.  The related immunity property group is a
    RULE_PropertySet in numberE.  This notification is sent every time an
    immunity is used, usually when the player lands on square numberC.
    Note that group immunities drive this, so hitting one square of a group
    will make the counts for all squares in the set go down. */

  NOTIFY_PICKED_UP_CARD,      /* 104 */
    /* The player has landed on a chance or community chest square and has
    picked up a card.  The player number is in numberA, the card deck (see
    RULE_CardDeckTypes) is in numberB, the particular card (RULE_CardType) is in
    numberC.  See also ACTION_CARD_SEEN. */

  NOTIFY_PUT_AWAY_CARD,       /* 105 */
    /* The player has put away the chance or community chest card.
    The player number is in numberA, the card deck (see RULE_CardDeckTypes)
    is in numberB, the particular card (RULE_CardType) is in
    numberC.  See also ACTION_CARD_SEEN. */

  NOTIFY_BUY_OR_AUCTION_DECISION,  /* 106 */
    /* The current player has landed on a square and is being asked to
    decide whether to buy the property or to let the bank auction it off.
    NumberA is the player number, NumberB is the square, NumberC is the
    cost to buy the square.  See ACTION_BUY_OR_AUCTION_DECISION. */

  NOTIFY_NEW_HIGH_BID,        /* 107 */
    /* An auction has been started or a new high bid has been made in
    a property or house auction.  The player number is in NumberA
    (RULE_BANK_PLAYER if this is the start of an auction) and the bid is
    in NumberB (the bank bids zero) and numberC is the property square
    or SQ_IN_JAIL == SQ_AUCTION_HOUSE for a house or SQ_OFF_BOARD ==
    SQ_AUCTION_HOTEL for a hotel.  NumberE is a RULE_PlayerSet that
    identifies the people allowed to bid in the auction. */

  NOTIFY_AUCTION_GOING,       /* 108 */
    /* Nobody has bid recently.  This is call NumberD for the end of the
    auction (numberD=1 -> going once, numberD=2 -> going twice,
    numberD=3 -> sold).  NumberA is the current high bidder and NumberB
    is the bid and numberC is the square or SQ_IN_JAIL == SQ_AUCTION_HOUSE
    for a house or SQ_OFF_BOARD == SQ_AUCTION_HOTEL for a hotel.  Once the
    item is sold, the normal property transfer notifications will also be
    sent (or a NOTIFY_PLACE_BUILDING for houses and hotels). */

  NOTIFY_HOUSING_SHORTAGE,    /* 109 */
    /* The bank has detected a housing shortage and the player in NumberA
    wants to buy a house or hotel.  Other players are prompted to respond with
    ACTION_START_HOUSING_AUCTION to start an auction.  NumberA is the player
    originally requesting the building, numberB is the square he wants to put
    it on.  NumberC is the number of houses/hotels left (positive for hotels
    and negative for houses), NumberD is a going, going, gone countdown like
    the NOTIFY_AUCTION_GOING one.  NumberE is a RULE_PlayerSet listing the
    players who can buy the same kind of building as the shortage is for. */

  NOTIFY_FREE_UNMORTGAGING,   /* 110 */
    /* The player has paid the tranfer interest fee for mortgaged
    properties in a trade or bankruptcy.  He can now unmortgage them
    with no extra penalty.  To raise cash for unmortgaging, the player
    can also mortgage other properties and sell buildings, but not
    do trading.  See ACTION_FREE_UNMORTGAGE_DONE.  The player number
    is in NumberA, the list of free unmortgageable properties is a
    RULE_PropertySet in NumberB (if it is zero then the free unmortgaging
    phase is ending). */

  NOTIFY_FLAT_OR_FRACTION_TAX_DECISION, /* 111 */
    /* The player in NumberA has to decide whether to pay the flat
    tax or to pay the fractional tax.  The flat tax is in NumberB and
    the percentage tax rate is in NumberC. */

  NOTIFY_TRADE_STARTED,       /* 112 */
    /* Clear the current trade list when you receive this.  It is also the
    first message you get when a trade starts, but you also get it every
    time the trading list is resent, which includes during the acceptance
    phase.  Player NumberA is the one proposing the trade (not that
    this matters much to the user interface).  Zero or more NOTIFY_TRADE_ITEM
    will follow this message, and they will be followed either by
    NOTIFY_TRADE_EDITOR (if in editing mode) or
    NOTIFY_TRADE_ACCEPTANCE_DECISION if in acceptance mode or by
    NOTIFY_ARE_YOU_THERE if this is the very start of the trade (once it has
    been synchronised, it will then resend NOTIFY_TRADE_STARTED, the items and
    then the missing NOTIFY_TRADE_EDITOR). */

  NOTIFY_TRADE_FINISHED,      /* 113 */
    /* The trading stuff has all been done (or cancelled), debts have been
    paid.  You could have some debt collections, mortgagings etc, before
    the trade is considered finished.  Back to normal game play.  You can now
    propose another trade if you want to (which is what the AI uses this
    message for). */

  NOTIFY_TRADE_ITEM,          /* 114 */
    /* Add an item in the current trade list.  The item being traded is
    from player NumberA to player NumberB (can be RULE_NOBODY_PLAYER if
    the trade item is being deleted for property ownerships; usually
    an amount of zero signals deletion for other kinds of things).
    NumberC describes the  kind of item (see RULE_TradeItemKind) and
    NumberD describes the amount or the particular item.  NumberE is a
    RULE_PropertySet for immunities. */

  NOTIFY_TRADE_EDITOR,        /* 115 */
    /* It's the turn of the player in NumberA to edit the current trade.
    He should change it with ACTION_TRADE_ITEM and then send an
    ACTION_TRADE_EDITING_DONE when done. 
    WARNING: This is sent AFTER the items on a phase restart, and prior
    to the items under normal circumstances. */

  NOTIFY_TRADE_ACCEPTANCE_DECISION, /* 116 */
    /* The players in NumberA (a RULE_PlayerSet) are being asked if they want
    to accept or reject the current trade (with a ACTION_TRADE_ACCEPT) or edit
    it (with ACTION_START_TRADE_EDITING).  NumberB (another RULE_PlayerSet) is
    TRUE for players that are involved in the trade (so you can turn on the
    Reject button for players in the trade and off for nonparticipants).  The
    rules engine accepts answers in any order.  This message is preceeded by
    NOTIFY_TRADE_STARTED and NOTIFY_TRADE_ITEM for all the items involved. */

  NOTIFY_ACTION_COMPLETED,    /* 117 */
    /* The action in NumberA has been completed.  NumberB is TRUE if it was
    successful, FALSE if something went wrong.  NumberC is the player who
    requested the action.  This notification is used for Artificial
    Intelligences so that they can tell if their request got accepted or
    rejected, without having to monitor a multitude of error messages and
    other notifications.  This notification is sent in addition to the
    usual notifications appropriate for the action.  Also, it is currently
    only implemented for the actions the AIs use.
    numberD: optional parameter.  For BUY_OR_AUCTION msgs, it indicates a
             buy (TRUE) or auction(F) decision
    */

  NOTIFY_VOICE_CHAT,          /* 118 */
    /* The player in NumberB (can be RULE_SPECTATOR_PLAYER) is saying
    something to the player in NumberA (or to everybody including
    spectators if it is RULE_ALL_PLAYERS).  NumberC is used in text
    chat for fluff messages, not used for voice.  NumberD is the hopefully
    unique network address of the original sender (IP address for Winsock,
    player ID for DirectPlay, and hopefully they don't overlap).  The voice
    data in our custom chunky streaming format is in binaryDataHandleA. */

  NOTIFY_TEXT_CHAT,           /* 119 */
    /* The player in NumberB (can be RULE_SPECTATOR_PLAYER) is saying
    something to the player in NumberA (or to everybody including
    spectators if it is RULE_ALL_PLAYERS).  The NUL terminated text
    data in Unicode format is in binaryDataHandleA.  If binaryDataHandleA
    is NULL then the precoded message number in NumberC is used instead
    (see TMN_FLUFF_*).  NumberD is the unique network address of the
    original sender. */

  NOTIFY_OPTIONS_FOR_SAVE,    /* 120 */
    /* The player has sent an ACTION_GET_OPTIONS_FOR_SAVE to get this
    notification.  The binary large object (blob) in binaryDataHandleA
    is a RIFF format saved options file reflecting the current options. */

  NOTIFY_AI_PARAMETERS,       /* 121 */
    /* The RIFF file in the attached blob contains data for initialising an
    AI after a game is loaded.  The player number is in NumberA.  This will
    be sent some time after the NOTIFY_NAME_PLAYER for the AI. */

  NOTIFY_AI_NEED_PARAMETERS_FOR_SAVE, /* 122 */
    /* The rules engine is trying to save a game and needs the parameter
    settings from the AI players to put in the save game file.  NumberA
    contains a RULE_PlayerSet listing the AIs that still need to send back
    an ACTION_AI_SAVE_PARAMETERS message.  NumberB is the player who is
    requesting the save. */

  NOTIFY_PLAYER_BUYSELLMORT,  /* 123 */
    /* The player in NumberA has exclusive control now, can be
    RULE_NOBODY_PLAYER if the previous player has finished doing stuff
    (signals the end of the phase).  In this state, the player is the only
    one who can do certain actions (buying buildings, selling buildings,
    mortgaging).  He is presumably working on the properties in NumberB
    (a RULE_PropertySet usually showing the properties in a monopoly group),
    but that is really only for display purposes.  NumberB of zero means
    don't display anything or skip the big time animation because the player
    is only buying a single house or doing something quick - just flash up the
    buy mode screen rather than doing a transition.  It is used for AIs buying
    a single house and then going back to the payment mode, to let other
    people get a chance to propose a trade.  If the player was busy paying a
    debt when he entered this phase, the total cash needed will be in NumberC,
    or zero if he wasn't busy paying when he started this phase.  NumberD will
    be the player he owes money to, or RULE_NOBODY_PLAYER.  See also
    ACTION_PLAYER_BUYSELLMORT and ACTION_PLAYER_DONE_BUYSELLMORT. */

  NOTIFY_DECOMPOSE_SALE,      /* 124 */
    /* The player in NumberA has sold a hotel and there aren't enough houses
    to replace it (but some phantom houses are shown on the board).  He has to
    sell NumberB more houses, but they can be anywhere on his properties, not
    just the monopoly that is being decomposed.  An ACTION_CANCEL_DECOMPOSITION
    message can be used to cancel the operation, the only other possible action
    is ACTION_SELL_BUILDINGS until the number of houses on the board is reduced
    to the total number of houses available. */

  NOTIFY_PLACE_BUILDING,      /* 125 */
    /* The player in NumberA is being asked to place the building he bought in
    an auction.  NumberB is +1 for a hotel, -1 for a house.  NumberC is a
    RULE_PropertySet listing the possible squares where he can put it (the
    usual call to RULE_TestBuildingPlacement won't work since he has already
    paid for it).  The player should send an ACTION_BUY_HOUSE to place it. */

  NOTIFY_STAR_WARS_ANIMATION_INFO, /* 126 */
    /* This broadcast message describes user interface animations that show
    what the current player is doing.  Things like moving trade items around
    on the screen during trade editing.  See ACTION_STAR_WARS_ANIMATION_INFO
    for details on what the various fields mean. */

  NOTIFY_GAME_OVER,           /* 127 */
    /* The game is over.  Player NumberA is the winner, with NumberB dollars
    in net wealth.  Anyone can start a new game in this state (useful for AI
    games).  See also ACTION_NEW_GAME. */

  NOTIFY_GAME_STARTING,       /* 128 */
    /* The game is starting now.  Put the players on the board, display their
    cash, etc.  This may be followed by some property sales for the short game
    (deals out 2 properties to each player) before the first player's turn. */

  NOTIFY_FUTURE_RENT_COUNT,   /* 129 */
    /* The player in numberA has numberB turns of intercepted rent remaining.
    He got it when someone landed on square numberC (set to SQ_OFF_BOARD when
    this is just telling you about the current future status, not an actual
    hit being used).  He is intercepting rent from player NumberD but only
    when NumberD owns the square.  The related property group is a
    RULE_PropertySet in numberE.  This notification is sent every time an
    rent intercept is used, usually when someone lands on square numberC.
    Note that the square set drive this, so hitting one square in the set
    will make the counts for all squares in the set go down. */

  NOTIFY_UPDATE_TRADE_INFO,   /* 130 FIXME - this and related fn;' not used in Mono 2 (RK)*/
    /* Interface message to update the trade information for a given item.
    This information is used only to maintain the visiable state of a trade,
    and includes the button id of the item, item TIK type, value and x,y coords.
    numberD: size of stringA data in bytes. Must not exceed 160.
    stringA: data of type IFACE_TradeTableRecord (SWM interface code) */

  NOTIFY_NEXT_MOVE,           /* 131 */
    /* This is a hint as to what the next move will be.  Of course, the player
    may decide to sell a house, answer a buy or auction question, or do
    something other than a move.  NumberA is one of NOTIFY_MOVE_FORWARDS,
    NOTIFY_MOVE_BACKWARDS or NOTIFY_JUMP_TO_SQUARE.  NumberB is the square
    moved to, previous square in NumberC, player in NumberD.  If the landing
    will be on a chance or community chest square, the card the player
    will pick up is in NumberE (a RULE_CardType).  This message is sent
    when the player is waiting for a move jail or utility roll, a jail
    decision, a wait for card seen, auction or buy decision, tax decision.
    Dice are rolled in advance for the next move and stored, so it can be
    pretty accurate :-). */

  NOTIFY_PLAYER_DELETED,      /* 132 */
    /* This message is sent just before a player is deleted (use
    ACTION_NAME_PLAYER with an empty string to do it).  NumberA is the slot
    the player was using, the token is in numberB and the colour choice is
    in numberC, and the AI level is in NumberD.  StringA is the player's name.
    Shortly after this  message one of the players will be reassigned to a
    new slot (NOTIFY_NAME_PLAYER for an existing player) and the number of
    players will be reduced by one (NOTIFY_NUMBER_OF_PLAYERS). */

  NOTIFY_GAME_PAUSED,         /* 133 */
    /* The game is paused, and will stay that way until someone sends an
    ACTION_PAUSE_GAME message with the right argument.  All other action
    messages will be ignored. */

  NOTIFY_PLEASE_ADD_PLAYERS,  /* 134 */
    /* The game is waiting for you to add players.  Use ACTION_NAME_PLAYER
    to add them and then ACTION_START_GAME when they are all there.
    NumberA is non-zero (actually the count of computers without Humans) if
    you need to add more Human players on other computers (happens when the
    no spectators compile time option is turned on). */

  NOTIFY_FREE_PARKING_POT,    /* 135 */
    /* The free parking jackpot's cash value has changed to the amount in
    NumberA.  NumberB is the amount of the increase. */

  NOTIFY_ARE_YOU_THERE,       /* 136 */
    /* The rules engine wants to know if you are still alive.  NumberA is
    a RULE_PlayerSet listing the players it wants to hear an ACTION_I_AM_HERE
    response from.  NumberB is a serial number to distinguish between
    different notifications (so that stale ACTION_I_AM_HERE messages don't
    confuse things).  NumberC is a NOTIFY_ message code that identifies the
    reason for this synchronization.  This is used to synchronize the start
    of auctions and other things. */


  /*
   * DISPLAY messages. Must all appear after DISPLAY_NULL
   * NOTE: These will likely all be changing/appearing/disappearing for a while.
   * for now they are mostly dupes of NOTIFIES.
   */

  DISPLAY_NULL = 160,
    /* Start of display messages
    NOTE:  These were used in the Star Wars interface, not used elsewhere.*/

  DISPLAY_MOVE_FORWARDS,
    /* Tell display system that a player is moving forward.
    NumberA: source square
    NumberB: dest square
    NumberC: Card if they are landing on a chance/com.chest
    NumberD: player number
    NumberE: 0
    StringA*: Null
    BinarySize: Null
   */

 DISPLAY_MOVE_BACKWARDS,
    /* Tell display system that a player is moving backwards.
    NumberA: dest square
    NumberB: src square
    NumberC: Card if they are landing on a chance/com.chest
    NumberD: player number
    StringA*: Null
    BinarySize: Null
   */

  DISPLAY_CHANGE_SCREEN,
    /* Change to the screen with the given id. Also for initial display of a given screen.
     NumberA: current screen id
     NumberB: new screen id
     NumberC: optional parameter.  Square number when changing to auction.
     NumberD: optional parameter.  Player set for auction.
   */

  DISPLAY_NAME_PLAYER,
    /* Display name for a player.
     numberA: player #
     stringA: player name
  */

  DISPLAY_ROLL_DICE,
    /* Display the initial 'get ready to roll dice' anime
     numberA: player #
  */

  DISPLAY_DICE_ROLLED,
    /* Display the dice rolling anime.
     numberA: die 1
     numberB: die 2
     numberC: player who rolled
     numberD: current square of player *before* roll
  */

  DISPLAY_DICE_ROLLED_QUICK,
    /* Display the dice rolling anime, without shaking the ship.
     numberA: die 1
     numberB: die 2
     numberC: player who rolled
     numberD: current square of player *before* roll
     numberE: variation # (0 = default)
  */

 DISPLAY_JUMP_TO_SQUARE,
    /* Tell display system to jump a player directly to a square. Also used to get players to start position.
    NumberA: dest square
    NumberB: src square
    NumberC: screen id
    NumberD: player number
    StringA*: RULE_PlayerInfoRecord
    BinarySize: sizeof(StringA)
   */

  DISPLAY_UPDATE_CASH,
     /* Update the cash display for a player
      numberA: player
    numberB: change in cash
    numberC: new total
   */

  DISPLAY_PLEASE_PAY,
     /* Show the screen askng a player to pay another player rent
      numberA: player with debt
    numberB: player who is owed
    numberC: amount owed
    numberD: amount they have to raise still to meet debt
   */

  DISPLAY_BUY_OR_AUCTION_DECISION,
  /* Display the 'buy or auction prompt.
    numberA: player
    numberB: square
    numberC: cost
  */
  DISPLAY_BUTTON,
  /* Tell the display engine to enable/disable buttons .
        numberA: IFACE_ButtonState
        numberB: IFACE_ButtonId
  */

  DISPLAY_SQUARE_OWNERSHIP,
    /* Tell the display to set/change ownership for a square
       numberA: square in question
       numberB: new owner
       numberC: old owner
    */

  DISPLAY_PICKED_UP_CARD,
    /* Tell display engine to display a card.
    numberA:player number
    number B: deck
    number C: card
    */
  DISPLAY_PUT_AWAY_CARD,
    /* Tell the display to make a card disappear.
    numberA: player number
    numberB: the card deck (see RULE_CardDeckTypes)
    numberC:the particular card (RULE_CardType)
    */

  DISPLAY_FLAT_OR_FRACTION_TAX_DECISION,
    /* Display the 'flat or fraction tax' option screen.
    numberA: The player
    numberB: flat tax
    numberC: the percentage
    */

  DISPLAY_RESET_GAME,
    /*
     Tells Display engine that we are beginning a new game.
     */
  DISPLAY_JAIL_EXIT_CHOICE,
  /* Displays the 'how to exit jail' info
     numberA: player
     numberB: TRUE to attempt to roll doubles.
     numberC: TRUE then he can pay $50.
     numberD: TRUE then he can use a card.
    */

  DISPLAY_SQUARE_MORTGAGE,
    /* Update square mortgage state.
    numberA: square
    numberB: TRUE = mortgaged, FALSE = unmortgaged
    numberC: the owner of the square
    */

  DISPLAY_SQUARE_HOUSES,
    /* Displayes bluildings on square.
    numberA:square
    numberB: # houses
    numberC: # houses required for a hotel
  numberD: owner of property
    */
  DISPLAY_JAIL_CARD_OWNERSHIP,
    /* A player gets a get out of jail card.
    numberA: player. If numberA is RULE_NOBODY_PLAYER then the card is back in the deck.
    numberB: the particular card deck
    */

  DISPLAY_START_TURN,
    /* A player is starting their turn.
    numberA: Player #
    numberB: Current square
    */

   DISPLAY_PROPOSED_CONFIGURATION,
    /* Someone is proposing a configuration (also gets sent at start).
      numberA: identifies the player who proposed it, can be RULE_NOBODY_PLAYER.
      numberB: RULE_PlayerSet that identifies the players whom we are still waiting for answers from.
      The current configuration is in the global variable UICurrentGameState.GameOptions.

    The players should reply back with a ACTION_ACCEPT_CONFIGURATION. */

    DISPLAY_PROPERTY_DEED,
    /* Shows a property deed.
        numberA: The property to display (see RULE_SquareType)
        numberB: bitmask of property faces to display: 1 = front, 0 = back
        numberC: TRUE to display; FALSE to remove
        numberD: TRUE to display the property set, FALSE to display a single property
    */

    DISPLAY_CASH_ANIMATION,
    /* Display an animation for getting/taking cash.
    The usual NOTIFY_CASH_AMOUNTs are sent after this message for the new
    amount, sometimes several if several players are involved, which makes
    it hard to decide what to animate, thus this message.
        NumberA is the From player (can be a player or RULE_BANK_PLAYER).
        NumberB is the To player  (same choices as From).
        NumberC is the amount transfered.
        NumberD: True = animate, false = no droid
        StringA contains a RULE_CashAnimationRecord which describes the amount
        of cash each player has after the transfer.
     See NOTIFY_CASH_ANIMATION
    */

    DISPLAY_IDLE,
    /* Displays one of the idling animations. Sent periodically at random intervals.

    */
    DISPLAY_START_GAME,
    /* Sends 'start game' info to the display engine, so it can play any initial animes
        numberA: number of players
        numberB: Initial cash as per game options
        StringA: (IFACE_StartGameInfo) starting game info
    */

    DISPLAY_GAME_MENU,
    /* Tells the display engine to display one of the game options menu animes.
     * Used when the user clicks on one of the planet buttons on the screen.
         numberA: Button id corresponding to option.  Currently BUTTON_FILE_MENU, BUTTON_HELP_MENU,
                  BUTTON_OPTIONS_MENU, BUTTON_TRADE_MENU
         numberB: FALSE - remove the file menu
     */

     DISPLAY_FREE_UNMORTGAGING,
     /* Displays the free unmortgaging info.
        numberA: player #
        numberB: set of squares in question.  See RULE_PropertySet, and use
            SquareNo = RULE_BitSetToProperty (SquareSet);
            SquareSet &= ~RULE_PropertyToBitSet (SquareNo);
        to convert to an actual square until square set == 0

        numberC: TRUE to display, FALSE to remove
      */

     DISPLAY_AUCTION_STATS,
     /* Update the current auction stats.
        numberA: high bidder player number
        numberB: high bid
        numberC: square for auction
        numberC is the property square or SQ_IN_JAIL for a house or SQ_OFF_BOARD for a hotel
        numberD: 'going' count (0 for n/a)
       */

      DISPLAY_BANK,
      /* Display banking information when user clicks on one of the bank buttons.
         numberA: is the bank button pressed. Currently:
        BUTTON_BANK_OWNERSHIP,
        BUTTON_BANK_HOUSE,
        BUTTON_BANK_BANK_PROPERTIES,
        BUTTON_BANK_OTHER

        Other parameters vary depending on the id of the button.
        BUTTON_BANK_OWNERSHIP: none
        BUTTON_BANK_HOUSE: B- number free houses; C- free hotels
        BUTTON_BANK_BANK_PROPERTIES:
            numberB: number free houses
            numberC: number free hotels
            numberD: number of elements in StringA
            StringA: byte array of square numbers owned by bank;
        BUTTON_BANK_OTHER: none
       */

    DISPLAY_AUCTION_DEED,
    /* Shows a property deed in the auction screen.
        numberA: The property to display (see RULE_SquareType) or SQ_IN_JAIL for a house or SQ_OFF_BOARD for a hotel
        numberB: TRUE - front, FALSE - back
        numberC: TRUE to display; FALSE to remove
        numberD: Player set to display platforms - 0 if all players
    */

    DISPLAY_PROPERTY_DEED_QUICK,
    /* Shows a property deed without the starting/ending animation.
        numberA: The property to display (see RULE_SquareType)
        numberB: bitmask of property faces to display: 1 = front, 0 = back
        numberC: TRUE to display; FALSE to remove
        numberD: TRUE to display the property set, FALSE to display a single property
    */

    DISPLAY_MINI_DEED,
    /* Perform display changes on the mini-deeds.  Used during a trade to add/remove the mini-property
       deeds.
       numberA: property square of deed being affected.
       numberB: owner
       numberC: x - coord to display, -1 to remove from screen
       numberD: y - coord to display, -1 to remove from screen
       */

    DISPLAY_TRADE_BUTTON,
    /* Perform display changes on the trade-items.  Used during a trade to add/remove the cash icon, jail card icon,
      future and immunities icons which can be placed anywhere on a player's area.
      Also used for misc buttons, such as cancel and review.
      numberA: valid trade button id
      numberB: optional
                Used for cash value for BUTTON_TRADE_TRAY#_CASH# items.
                Trade token:  source position (artist coords). destination is indicated by token#
                -1 means from top-left, -2 means public, -3 means private
                For review button, TRUE means display, FALSE means remove
      numberC: x - coord to display, -1 to remove from screen
      numberD: y - coord to display, -1 to remove from screen
      numberE: opt. Used for dest. position for trade tokens (artist coords)
      */

    DISPLAY_TRADE_CASH,
    /* Display the given amount on the trade screen cash object (large version if needed)
       numberA: cash amount
     */

     DISPLAY_BANKRUPTCY,
     /* Plays the bankruptcy anime for the given player.
        numberA: player gone bankrupt
      */

    DISPLAY_TRADE_CASH_TRAY,
    /* Open or close the cash tray in the trade screen.
       numberA: TRUE to open, false to close
     */

    DISPLAY_TRADE_BUTTON_HILIGHT,
    /* Perform display changes on the trade-items.  This is the same as DISPLAY_TRADE_BUTTON, but
       we use the 'hilighted' version of the given item, for those that have one.
       numberA: BUTTON_TRADE_TRAY1_CASH to BUTTON_TRADE_TRAY8_FUTURE, plus tokens
       numberB: optional.  Used for cash value for BUTTON_TRADE_TRAY#_CASH# items.
       numberC: x - coord to display, -1 to remove from screen
       numberD: y - coord to display, -1 to remove from screen
       */

    DISPLAY_TRADE_ICON_INTRO,
    /* Displays the anime of trade tray icons 'popping' onto the screen. Can be used to transfer
     * one set of icons at a time.
     * numberA: number of icons in this set.
     * numberB: player #
     * StringA: array of UInt_16's containing: button id, x coord, y coord.
     */

    DISPLAY_TRADE_ITEM_MOVEMENT,
    /* Displays the anime of a trade item at the gtiven location.  Used when 'dragging' an item
     * to a new location.
     * numberA: BUTTON_TRADE_TRAY1_CASH to BUTTON_TRADE_TRAY8_FUTURE, plus tokens
     * numberC: new x location
     * numberD: new y location
     */

    DISPLAY_TRADE_MINI_OWNERSHIP,
    /* Display/change a property ownership on the trade screen mini-board.
     *  numberA: deed in question, from 0 to 27
     *  numberB: new proposed owner
     */

     DISPLAY_TRADE_MINI_MORTGAGE,
    /* Update square mortgage state for the trade screen mini-board.
     * On the trade screen, mortgages have only 1 symbol regardless of owner.
     *  numberA: number of deeds in this set
     *  numberB: TRUE to mortgage, FALSE to unmortgage
     *  stringA: set of property deed values, each from 0 to 27
     */

    DISPLAY_TRADE_OWNER_SET,
    /* Update the ownerships on the trade screen mini-board for a set of squares
     * for a given player.
     *  numberA: number of deeds in question, each from 0 to 27
     *  numberB: Player who owns the squares
     *  stringA: The set of ownership data, as bytes in the form deed #, # houses
     */

    DISPLAY_REFRESH_PROPERTY_DEED,
    /* Refreshes a property deed that si already visible on the screen.
        numberA: The property to display (see RULE_SquareType)
        numberB: bitmask of property faces to display: 1 = front, 0 = back
        numberC: ---
        numberD: TRUE to display the property set, FALSE to display a single property
    */

  DISPLAY_NEXT_MOVE,
    /* This is a hint as to what the next move will be.
       See NOTIFY_NEXT_MOVE
     */

  DISPLAY_WIN_ANIME,
    /* Plays the 'you win' anime.
     * numberA: player who won.
     * numberB: net wealth of that player.
     */


  DISPLAY_NET_CHAT_DROID,
    /* Displays the network chat droid.
       numberA: TRUE display msg waiting anime, FALSE is normal
     */

  DISPLAY_PLAY_WAVE,
    /* Plays a sound.
          numberA: action id. These identify the sound type (ie. ACTION_BUY_AUCTION, param 0 = buy dialogue)
          numberB: option param to help identify the sound
       Most sounds willbe handled with the graphics automatically.  This msgs can be used when
       there is no corresponding display message that could have played the sound explicitly.
     */

  DISPLAY_CLOSEUP_ACTION,
    /* Performs a display related action pertaining to the closeup view.
       This relates mostly for cleanup, especially when there is no corresponding
       display message for the top-down view.
       numberA: Rules action
     */

  DISPLAY_REMOVE_PLAYER_SHIPS,
    /* Performs the display action of removing all the players ships from the screen
       including the number display, token and symbol
       No parameters
    */

  DISPLAY_SHOW_SQUARE_FLASH,
    /* Displays a flashing square around the given board square.
       numberA: PropertySet. 0 is ignored
       numberB: player # (0 to RULE_MAX_PLAYER) - used for colour
       numberC: TRUE - display, FALSE - remove. Currently, a FALSE will remove any flashing
                squares from the board.
      */
  MAX_ACTIONS
};
typedef int RULE_ActionType;
  /* Each action is processed by the rules engine and generally does something
  to the current state and may generate more actions.  The players can also
  generate some actions (like roll dice, buy house, propose trade), though
  most actions are internal ones.  Associated with each action are several
  arguments. */



enum RULE_NetworkSystemEnum
{
  NS_LOCAL = 0,
  NS_DIRECTPLAY,
  NS_WINSOCK,
  NS_MAX
};
typedef int RULE_NetworkSystem;
  /* Identifies the various different kinds of networking system we can use.
     In here rather than MESS.H since it is needed here. */



typedef struct RULE_NetworkAddressStruct
{
  RULE_NetworkSystem networkSystem;
  union
  {
    unsigned char directPlay [4];
      /* DirectPlay addresses are 4 bytes, a DPID. */

    unsigned char winSock [20];
      /* WinSock addresses are a 4 byte connection number followed by 16 bytes,
      a sockaddr or sockaddr_in which has the IP address and other stuff. */

  } address;
} RULE_NetworkAddressRecord, *RULE_NetworkAddressPointer;
  /* A network address.  In here rather than MESS.H since it is needed here.
  An address of all zeros means no network connection (local player). */



#define RULE_ACTION_ARGUMENT_STRING_LENGTH 79
  /* Number of characters, not bytes, in the string.  Doesn't include
  the NUL at the end of the string. */

typedef struct RULE_ActionArgumentsStruct
{
  RULE_ActionType action;
    /* The action code for this action and arguments packet. */

  RULE_PlayerNumber fromPlayer;
    /* Who this message is from.  Messages received over the network
    will automatically have this field verified against their network
    address, so you can't pretend to be someone else. */

  RULE_NetworkAddressRecord fromNetworkAddress;
    /* Some sort of network address.  This field is filled in by the network
    software when it receives the message.  Useful for checking that a
    message from a player is really from that player. */

  RULE_PlayerNumber toPlayer;
    /* Who this message is to.  Equals RULE_NOBODY_PLAYER if it is a broadcast
    to everybody.  RULE_BANK_PLAYER for messages to the server. */

  long  numberA;
    /* The first numeric argument, usage depends on the action.  Is often the
    error message number. */

  long  numberB;
  long  numberC;
  long  numberD;
  long  numberE;
    /* More numeric arguments. */

  wchar_t stringA [RULE_ACTION_ARGUMENT_STRING_LENGTH + 1];
    /* A string in Unicode characters.  May also be used for small binary
    objects (less than 160 bytes). */

  RULE_CharHandle binaryDataHandleA;
    /* Usually NULL, but can be set to point to a big hunk of binary data,
    which is owned by this action arguments record (it will be deallocated
    when the action argument is processed).  This is used for large things
    like voice sample data in an audible chat message. */

  unsigned long binaryDataStartOffsetA;
    /* The meaningful binary data starts at this offset in the
    binaryDataHandleA blob.  This is used when receiving a big network message
    so that the header stuff can be skipped over, rather than allocating a
    second huge hunk of memory to hold the trimmed binary. */

  unsigned long binaryDataSizeA;
    /* Number of bytes of useable data, starting at the start offset. */

  BYTE *binaryDataA;
    /* The handle is locked and a pointer is placed here for your convenience
    (includes the start offset), when this message surfaces from the message
    queue.  NULL when not locked. */

} RULE_ActionArgumentsRecord, *RULE_ActionArgumentsPointer;
  /* These action + arguments records are stored in a queue and processed
  by the rules engine.  They are also used for communicating with the
  player programs. */



enum RULE_GamePhaseEnum
{
  GF_ADDING_NEW_PLAYERS,      /* 0 */
    /* The game isn't in play yet.  We're busy collecting participants
    along with their names, colour and token choices. */

  GF_CONFIGURATION,           /* 1 */
    /* The game isn't in play yet.  The players are choosing a
    configuration (all players have to agree). */

  GF_PICKING_STARTING_ORDER,  /* 2 */
    /* The players are rolling dice to pick the starting order.   We are
    waiting for the players in the RULE_PlayerSet in the phase's fromPlayer
    field.  If it is zero, it reconsiders whether the order is complete or
    whether some ties need to be broken. */

  GF_WAIT_START_TURN,         /* 3 */
    /* Waits for the start turn message, tossing out restarts that
    may have been placed by enthusiastic configuration acceptors.
    Also, the ACTION_START_TURN doesn't get sent until the message
    queue has emptied out (ACTION_TICK sends it when the queue is
    empty).  That way overly enthusiastic AIs don't overfill the
    queue when there are network spectators (messages are held
    in the queue until they have been sent). */

  GF_WAIT_END_TURN,           /* 4 */
    /* Waits for the end turn message, tossing out restarts and other
    junk from the queue. */

  GF_PREROLL,                 /* 5 */
    /* The player is starting his turn or has finished a dice roll
    and the subsequent actions.  This phase will decide if he should
    roll again or end his turn.  Rolling and other phases are stacked
    on top of this one, so that when they finish their actions,
    they restart and the decision to roll or end is made again. */

  GF_WAIT_MOVE_ROLL,          /* 6 */
    /* We are waiting for the player to roll the dice for moving
    his token around the board. */

  GF_MOVING_TOKEN,            /* 7 */
    /* The token is being moved around the board.  This is a transitory phase
    that happens after the dice roll and only lasts a very short time (until
    calculations are complete). */

  GF_WAIT_JAIL_ROLL,          /* 8 */
    /* We are waiting for the player to roll the dice in an attempt
    to get out of jail. */

  GF_WAIT_UTILITY_ROLL,       /* 9 */
    /* Waiting for the player to roll for paying the random
    utility rent. */

  GF_WAIT_UNTIL_CARD_SEEN,    /* 10 */
    /* Wait for the player to look at the chance or community chest card.
    The card gets processed after they have finished looking at it. */

  GF_JAIL_ROLL_OR_PAY_OR_CARD_DECISION,     /* 11 */
    /* Waiting for the player to decide what to do to get out of jail.
    He can perhaps decide to roll the dice, or can pay the money or
    maybe use a card to get out of jail.  If turnsInJail is 99 then
    the roll and jail card options are disabled - for those situations
    where you roll and fail to get doubles on the last turn and now must
    pay (the rules say pay - no get out of jail cards allowed). */

  GF_GET_OUT_OF_JAIL,         /* 12 */
    /* When this action is restarted, the player will move out of jail
    into the Just Visiting square and continue his turn from there.
    This phase is stacked under a debt payment phase for the exit fee,
    so that the player starts moving _after_ the debt is paid. */

  GF_FLAT_OR_FRACTION_TAX_DECISION, /* 13 */
    /* We are asking the player if they want to pay the flat tax or the
    percentage of income tax. */

  GF_AUCTION_OR_BUY_DECISION, /* 14 */
    /* Ask the current player if they want to buy the current square,
    or have the bank auction it to everybody. */

  GF_EDITING_TRADE,           /* 15 */
    /* A trade is in progress.  One player has proposed a set of transactions
    that some other one is currently editing.  The proposing player (actually,
    the one who made the most recent changes) is the FromPlayer argument and
    the current one who is editing it is the ToPlayer.  The toPlayer can also
    cancel the trade, pass it on to someone else for editing it, or go into
    a trade acceptance vote. */

  GF_TRADE_ACCEPTANCE,        /* 16 */
    /* People are voting on the trade.  All relevant parties have to send
    back an acceptance (via ACTION_TRADE_ACCEPT/TRUE) before it goes through.
    It can be rejected (via ACTION_TRADE_ACCEPT/FALSE) and it can also go
    back to editing (via ACTION_START_TRADE_EDITING).  On restart, this phase
    asks all people who haven't accepted if they accept (with a
    NOTIFY_TRADE_ACCEPTANCE_DECISION).  As before, the fromPlayer proposed
    the trade (last one to change it), the toPlayer isn't used and the Amount
    is TRUE for a private offer (only people involved can accept or reject or
    edit), FALSE for public (anyone can butt in). */

  GF_TRADE_FINISHED,          /* 17 */
    /* When this phase gets restarted (after a pile of debts and escrows
    have completed), the trade is marked as being over, so new trades can
    be allowed to start. */

  GF_AUCTION,                 /* 18 */
    /* An auction for a piece of land or building that the bank is trying
    to sell.  The Amount field is the property square or SQ_IN_JAIL ==
    SQ_AUCTION_HOUSE for a house or SQ_OFF_BOARD == SQ_AUCTION_HOTEL for
    a hotel. */

  GF_HOUSING_SHORTAGE_QUESTION,   /* 19 */
    /* The bank is asking if other players want to buy a house.  If nobody
    responds after a certain time, the bank sells the house to the original
    house buyer.  If someone else wants a house too, then the bank starts
    a house auction.  FromPlayer is the person who tried to buy a building,
    ToPlayer is the square they want to put it on, Amount is negative number
    of free houses and positive number of free hotels. */

  GF_COLLECTING_PAYMENT,      /* 20 */
    /* Some amount of money is being collected from some player to pay
    some other player or the bank.  From and to players and amount being
    collected are specified by the similarly named phase stack fields.
    The player can get rid of this state by raising enough cash (if there
    is enough, it automagically gets gobbled up and the game goes on).
    If the player doesn't have enough net wealth to cover the debt, he
    can use ACTION_GO_BANKRUPT, but should really try trading etc. to raise
    the money. */

  GF_TRANSFER_ESCROW_PROPERTY,    /* 21 */
    /* This is a transient phase that is run after the toPlayer has paid off
    his debts (they were done before this phase, including interest charges
    for land transfer) so the previously escrowed property can now be
    transfered to the new owner when this phase restarts.  A GF_FREE_UNMORTGAGE
    phase will follow this one to handle unmortgaging of transfered mortgaged
    properties.  The arguments are the new owner player in toPlayer and the set
    of properties transfered as a RULE_PropertySet in Amount.  This is used for
    resolving trades, auctions and ordinary property purchases. */

  GF_FREE_UNMORTGAGE,       /* 22 */
    /* If any of the transfered escrowed land is mortgaged, the player can
    lift the mortgages right away without further interest charges (money can
    be raised by selling buildings and mortgaging - but not trading).  The
    arguments are the new owner player in toPlayer and the set of recently
    transfered mortgaged properties as a RULE_PropertySet in Amount.  As
    properties are unmortgaged, or if they were initially unmortgaged, they
    get removed from the set.  The phase ends when the player says he's done
    unmortgaging or when there are no mortgaged properties left. */

  GF_GAME_FINISHED,         /* 23 */
    /* The game is over and the players are looking at the final score.
    They can still chat to each other, perhaps to start a new game, or
    perhaps to take back moves and try again.  The fromPlayer is the winner,
    with the net wealth in amount. */

  GF_BUYSELLMORT,           /* 24 */
    /* A particular player (the fromPlayer) is busy buying, selling or
    mortgaging.  Other players have to wait for him to finish.  The
    amount is a RULE_PropertySet identifying the properties that he
    claims to be concentrating on (used for display purposes only). */

  GF_DECOMPOSE_HOTEL,       /* 25 */
    /* A particular player (the fromPlayer) has to finish decomposing the
    hotels on the monopoly containing the square in "amount".  At the
    start of the phase, the game state is saved, so that the player can
    cancel the whole thing. */

  GF_PLACE_BUILDING,        /* 26 */
    /* The user (fromPlayer) has bought a building (amount is -1 for a
    house, +1 for hotel) and has to place it on a square.  Available
    squares dynamically calculated, exit this phase with an
    ACTION_BUY_HOUSE or if there are no squares to put the building
    on (something went wrong). */

  GF_COLLECT_AI_PARAMETERS_FOR_SAVE, /* 27 */
    /* The rules engine is busy trying to save the game and is asking the AI
    players for their internal state information.  "from" contains a
    RULE_PlayerSet of the AIs which haven't supplied their save parameters
    state yet.  "to" is the player who requested the save.  "amount" contains
    a game time stamp for the time when the save was started, so that it can
    be aborted after a minute of no useful response from bad AIs. */

  GF_PAUSED,                /* 28 */
    /* The game is in a paused state.  Almost all actions except unpause
    will be ignored.  However, anyone may unpause. */

  GF_WAIT_FOR_EVERYBODY_READY, /* 29 */
    /* The game is waiting for all active players to check in.  This is done
    before time critical things, like the start of an auction, where you have
    to be sure that everyone is ready for the auction.  The original game
    clock is in the amount field - it is used for both a serial number for
    the syncronization and to abort the synchronization if nobody answers
    after enough time has passed.  Thre is also a hint code (a NOTIFY_ message
    code) is in the toPlayer field (why this synchronization is being done).
    The fromPlayer is a RULE_PlayerSet that identifies the players who haven't
    replied yet (bankrupt players are not asked).  See also ACTION_I_AM_HERE
    and NOTIFY_ARE_YOU_THERE. */

  GF_MAX
};
typedef int RULE_GamePhase;

typedef struct RULE_PendingPhaseStruct
{
  RULE_GamePhase    phase;
  RULE_PlayerNumber fromPlayer;
  RULE_PlayerNumber toPlayer;
  long              amount;
} RULE_PendingPhaseRecord, *RULE_PendingPhasePointer;
  /* Various activities can be interrupted by others.  This record is used
  to remember the activities that are suspended until the interruption
  is dealt with. */

#define RULE_MAX_PENDING_PHASES (RULE_MAX_PLAYERS * RULE_MAX_PLAYERS + 2 * RULE_MAX_PLAYERS + 6)
  /* The stack of pending actions can contain this many items.  Note that
  some actions, like get money from all other players, can store as many
  pending phases as there are players.  In trading, players can get a debt
  phase to all other players and a debt to the bank and an escrow phase
  stacked up. */


#define RULE_MAX_VOC_COMRESSOR_NAME_LENGTH 19
  /* Length of the name of the audio compressor codec to use, in characters
  not bytes.  Doesn't include the NUL at the end. */

typedef struct RULE_GameOptionsStruct
{
  unsigned char housesPerHotel;
    /* The number of houses a hotel is equivalent to, in the current game.
    Normally this will be 5, reduced to 4 for short games. */

  unsigned char maximumHouses;
  unsigned char maximumHotels;
    /* These limit the number of houses and hotels that can be on the board.
    If this many are used up then you can't build any more until someone
    sells one. */

  long interestRate;
    /* Interest rate for mortgages in percentage units.  The normal value
    for this is 10. */

  long initialCash;
    /* How much cash the players start the game with. */

  long passingGoAmount;
    /* How much cash do you get when passing go? */

  long luxuryTaxAmount;
    /* How much do you pay for Luxury Tax in this game? */

  long taxRate;
    /* When you pay tax, how much is the tax rate?  This is in
    percentage units.  Usually 10.  If 0%, it just charges you
    the flat tax fee and doesn't ask you for a decision (special
    short game variation). */

  long flatTaxFee;
    /* If you choose to pay the flat tax, this is how much you pay.
    Normally 200. */

  long freeParkingSeed;
    /* Initial amount of cash put by the bank into the Free Parking pot,
    done only once at the start of the game.  Usually $500. */

  BOOL freeParkingPot;
    /* TRUE if the Free Parking jackpot is on.  The money collected in the
    pot gets paid to the person who lands on Free Parking.  The pot gets
    replenished by money that would otherwise be paid to the bank for taxes
    (but not building fees or mortgage fees). */

  BOOL doubleSalaryOnGo;
    /* When this is TRUE, you get double the normal salary if you
    land on GO. */

  BOOL hideCash;
    /* Don't tell the other players what your cash total is.  The rules
    engine still sends the cash amounts, it is up to the user interface
    to not display the amounts (after all, a computer assisted player
    could always figure out the cash). */

  BOOL evenBuildRule;
    /* TRUE if we observe the even build rule. */

  BOOL rollDiceToDecideStartingOrder;
    /* TRUE if there is an extra phase at the start to decide who goes
    first.  FALSE to just use the order the players signed up in. */

  BOOL cheatingAllowed;
    /* All those cheating actions, for dice rolls and cash awarding and
    square ownership are turned on when this is TRUE. */

  BOOL AITakesTimeToThink;
    /* TRUE if the AI simulates "thinking" by doing a time delay between
    actions.  FALSE if you want the AIs to run at full speed. */

  BOOL futureRentTradingAllowed;
    /* Enables the futures in rent trading feature. */

  BOOL immunitiesTradingAllowed;
    /* Enables the immunity trading feature. */

  BOOL allowPlayersToTakeOverAIs;
    /* Turn this on if you want to be able to resume saved games and rejoin a
    network game.  Off for games where you want to play AIs and don't want
    spectators to get in the way. */

  BOOL dealFreePropertiesAtStartup;
    /* If TRUE then it doesn't charge for the properties dealt.  If FALSE,
    then the players pay for them.  The dealing can also stop when the players
    run out of cash if this option is off. */

  unsigned char dealNPropertiesAtStartup;
    /* Enables the short game feature to deal out some cards.  Zero means no
    cards, 2 means two cards to each player, and so on.  Will also stop when
    it can't give a card to everybody (if they can't afford them or there
    simply are no more), but it will leave each player with the same number
    of cards (seems fair though it isn't - guy with low cash will get cheap
    properties and is more likely to get Mediterranean and Baltic). */

  unsigned char stopAtNthBankruptcy;
    /* Another short game feature.  Game stops and the richest one wins
    after this many bankruptcies.  Zero turns off this feature. */

  unsigned char maximumTurnsInJail;
    /* Normally 3.  Number of turns you can spend in jail, including the
    turn you got put in but not including the turn you get out.  Zero enables
    a special short game mode where you leave jail on the next turn, using
    any of the normal three methods (if you roll doubles and fail then you are
    immediately charged $50 and get out). */

  long getOutOfJailFee;
    /* How much does it cost to get out of jail?  Usually $50. */

  BOOL mortgagedCountsInGroupRent;
    /* TRUE if American rules where the mortgaged properties count in boosting
    the rent for a monopoly.  FALSE for European where you lose the monopoly
    if you don't have them all.  Also affects railroad and utility rent. */

  unsigned char houseShortageLevel;
  unsigned char hotelShortageLevel;
    /* When there are this many houses or hotels left and someone wants to
    buy one, the bank will ask everyone if someone else wants to buy before
    allowing the purchase.  If someone else wants a house or hotel too then
    the bank auctions it off.  The bank then stays in auction or non-auction
    mode until the next dice roll, when it will ask again if needed. */

  unsigned char auctionGoingTimeDelay;
    /* Number of seconds of inactivity (ACTION_TICKs counted) before
    the auctioneer will say Going Nth. */

  unsigned short inactivityWarningTime;
    /* If a player doesn't do anything for this many seconds when it is
    his turn, warn him.  On the third time, bump him off and replace him
    with a local AI.  A value of zero disables the timer. */

  unsigned short gameOverTimeLimit;
    /* Sets a time limit to the game.  Zero if no limit. */

  struct voiceChatStruct
  {
    long    recordingHz;
      /* Usually 11025 samples per second.  Not used any more. */
    short   recordingBits;
      /* Either 8 or 16 bits per sample.  Not used any more. */
    wchar_t compressorName [RULE_MAX_VOC_COMRESSOR_NAME_LENGTH+1];
      /* "GSM 6.10" is a good default.  The largest is about 15 letters.  Not used any more. */
  } voiceChat;

} RULE_GameOptionsRecord, *RULE_GameOptionsPointer;



#define RULE_MAX_COUNT_HIT_SETS (RULE_MAX_PLAYERS * SG_MAX_PROPERTY_GROUPS)
  /* There can be at most this many active immunities and futures granted.
  Enough for one from every player on every group.  If you actually have
  that many, then the game gets pretty silly. */

#if (RULE_MAX_PLAYERS + 1) /* RULE_NOBODY_PLAYER */ > 7
  #error "Increase bitfield size to hold a RULE_NOBODY_PLAYER in CountHitSquareSet."
#endif

enum CountHitEnum
{
  CHT_NOTHING = 0,
  CHT_RENT_IMMUNITY,
  CHT_FUTURE_RENT,
  
  MAX_COUNT_HIT_TYPES
};
typedef int CountHitTypes;

// using this bit method would have the immunities icon appear for no reason when loading a game
// the reason is you can not represent 4 types with 1 bit
// we will try withoutht the bits 
/*
typedef struct CountHitSquareSetStruct
{
  RULE_PropertySet  properties;     // A set of properties you can hit. 
  unsigned int      fromPlayer : 3; // Who owns the property. 
  unsigned int      toPlayer : 3;   // RULE_NOBODY_PLAYER if unallocated. 
  unsigned int      hitType : 1;    // One of the CountHitTypes. 
  unsigned int      tradedItem : 1; // A boolean, TRUE if this is being traded. 
  signed int        hitCount : 8;   // Number of hits remaining.  Can be negative during trading to cancel others. 
}
CountHitSquareSetRecord, *CountHitSquareSetPointer;
*/
typedef struct CountHitSquareSetStruct
{
  RULE_PropertySet  properties;     /* A set of properties you can hit. */
  unsigned int      fromPlayer; /* Who owns the property. */
  unsigned int      toPlayer;   /* RULE_NOBODY_PLAYER if unallocated. */
  unsigned int      hitType;    /* One of the CountHitTypes. */
  unsigned int      tradedItem; /* A boolean, TRUE if this is being traded. */
  signed int        hitCount;   /* Number of hits remaining.  Can be negative during trading to cancel others. */
}
CountHitSquareSetRecord, *CountHitSquareSetPointer;
  /* When someone lands on a square in properties and it is currently owned by
  fromPlayer (mortgaged or not) then something happens and the count gets
  decremented (deallocated at zero).  hitType determines what happens:
  CHT_RENT_IMMUNITY means that toPlayer doesn't have to pay rent if he is the
    one landing on the square.
  CHT_FUTURE_RENT means that the rent collected by fromPlayer is transfered
    to toPlayer.  Except that if the person landing is toPlayer, he gets
    free rent and nothing is transfered.  One quirk is that if the lander
    goes bankrupt, the square owner still owes the full rent money to the
    toPlayer.
  */


typedef struct RULE_GameStateStruct
{
  unsigned char NumberOfPlayers;
    /* Number of players in this game, from 0 to RULE_MAX_PLAYERS - 1.  Stays
    constant once the game has started.  Includes bankrupt players. */

  RULE_PlayerNumber CurrentPlayer;
    /* The person whose turn it is.  Set to RULE_NOBODY_PLAYER during non-gaming
    states (pre-game and post-game). */

  RULE_PlayerInfoRecord Players [RULE_MAX_PLAYERS];
    /* All the player specific info.  The players are in order of their
    turns.  During signup, the order is the signup order, then they roll
    dice to decide who goes first, then they get reassigned to different
    slots in this array (using the renaming action). */

  RULE_SquareInfoRecord Squares [SQ_MAX_SQUARE_TYPES];
    /* All the square specific info. */

  RULE_CardDeckRecord Cards [MAX_DECK_TYPES];
    /* The various cards in little piles on the board.  Includes information
    on where the get out of jail free card is. */

  RULE_GameOptionsRecord GameOptions;
    /* The current game options settings. */

  unsigned char Dice[2];
    /* The two dice of the current player's dice roll.  Set to zero if the
    dice haven't been rolled yet. */

  unsigned char NextDice[2];
    /* The two dies for the next roll to happen.  Set to zero if they have been
    used up and not regenerated.  These are copied to Dice when they are used. */

  unsigned char UtilityDice[2];
    /* The two dice of the current player's utility dice roll.  Kept separate
    from the movement dice roll since that would affect rolling doubles.
    Set to zero if the dice haven't been rolled yet. */

  unsigned char NumberOfDoublesRolled;
    /* Number of double dice rolls made so far. */

  BOOL JustRolledOutOfJail;
    /* TRUE if the player has just rolled doubles to get out of jail.  That
    means they shouldn't continue rolling to move, even though they got
    doubles. */

  BOOL TradeInProgress;
    /* TRUE while a trade is in progress or being resolved, used for
    preventing multiple trades and other things during a trade (can't tell
    from the current phase since it may be a debt resolution phase or
    escrow phase). */

  BOOL ThisStateIsValid;
    /* TRUE if this whole record is valid.  FALSE if it hasn't been
    initialised yet.  This is used by the StackedRulesStates to tell if
    the state record in a particular stack element is empty or has data. */

  RULE_CardType PendingCard;
    /* The chance or community chest card that will have an effect on an
    upcoming action - usually things like pay double rent on the railroads.
    This gets set when the card is picked.  Then the token is moved to
    the railroad.  When it gets there, the normal rent collection will
    check to see if there is a pending card that affects it - and will
    charge double if it sees the right card.  When the card has been
    consumed, this gets set back to NOT_A_CARD. */

  DWORD GameDurationInSeconds;
    /* How long the game has been running so far.  Set to zero when a game
    starts, incremented by the TICK count.  Used for the timed game. */

  long FreeParkingJackpotAmount;
    /* Amount of cash in the Free Parking Jackpot. */

  CountHitSquareSetRecord CountHits [RULE_MAX_COUNT_HIT_SETS];
    /* Keeps track of futures and immunities. */

  RULE_PendingPhaseRecord phaseStack [RULE_MAX_PENDING_PHASES];
  unsigned char NumberOfPendingPhases;
    /* The current phase is at the top of the stack (at index 0).  Pushing
    and popping is slightly inefficient, but accessing the top element is
    a lot easier.  A stack keeps track of pending things that need to be
    resolved.  For example, if you get a collect $50 from every player,
    there will be a bunch of actions for collecting money stored here
    (in the appropriate clockwise player order).  Then when one of the
    collectees goes bankrupt, trades and house sales can be done (and more
    cascading  bankruptcies) before finally getting back to collecting money
    from the next player.  The phase is removed from the stack once it has been
    resolved.  Switching phases involves removing the old and adding the new.
    If the stack goes empty, display a program bug detected message
    and switch to the GF_ADDING_NEW_PLAYERS phase. */

  union phaseDependentUnion2 /* The values in here are used in different phases. */
  {
    struct startingStruct2
    {
      RULE_PlayerNumber proposer;
        /* Who last changed the proposed configuration, or RULE_NOBODY_PLAYER. */
    } starting;

    struct tradingStruct2
    {
      char junk;
    } trading;

    struct auctionStruct2
    {
      unsigned char tickCount;
        /* Used during auctions to decide how long the auction has been going
        on without any activity.  Counts the ACTION_TICK events and when this
        reaches auctionGoingTimeDelay, it issues another Going Nth request.
        Also used in the GF_HOUSING_SHORTAGE_QUESTION phase. */

      unsigned char goingCount;
        /* Counts the number of Going Nth requests issued so far.  Resets to
        zero whenever a new bid comes in (also resets the tickCount).  The
        auction is over when this reaches 3. */

      RULE_PlayerNumber highestBidder;
        /* Who has the highest bid, can be RULE_BANK_PLAYER if nobody has made
        a bid yet (item isn't sold if the bank wins the auction). */

      long highestBid;
        /* Cash amount of the highest bid.  Starts at zero. */

      RULE_SquareType propertyBeingAuctioned;
        /* Identifies what is being auctioned.  It is the square or SQ_IN_JAIL
        for a house or SQ_OFF_BOARD for a hotel. */

    } auction;

  } shared; /* For shared, recycled storage space between different phases. */

} RULE_GameStateRecord, *RULE_GameStatePointer;



enum RULE_ResyncCausesEnum
{
  RESYNC_NET_REFRESH,
  RESYNC_GAME_START,
  RESYNC_LOAD_GAME,
  RESYNC_UNDO_BANKRUPTCY,
  RESYNC_UNDO_HOTEL_DECOMPOSITION,
  MAX_RESYNC_CAUSES
};
typedef int RULE_ResyncCauses;
  /* The various reasons a resync message may be sent. */



typedef struct RULE_ClientResyncInfoStruct
{
  /* Note that this record gets transmited in binary form over the network,
     so make sure the fields are packed together, avoid alignment problems.
     So, longer sized primitives come first. */

  long cash [RULE_MAX_PLAYERS];
    /* Cash each player has.  The user interface shouldn't display it if the
    hideCash game option is on. */

  RULE_PropertySet propertiesOwned [RULE_MAX_PLAYERS];
    /* The bit is set for all the properties that the indexed player owns.
    All the unclaimed properties belong to nobody. */

  RULE_PropertySet mortgagedProperties;
    /* A set bit marks a mortgaged property, clear (zero) marks
    unmortgaged.  All players properties are included here. */

  RULE_SquareType playerSquare [RULE_MAX_PLAYERS];
    /* Identifies which square each player is on. */

  RULE_PlayerNumber jailOwners [MAX_DECK_TYPES];
    /* Who owns each get out of jail card, or RULE_NOBODY_PLAYER if unowned. */

  unsigned char houseCounts [SQ_MAX_SQUARE_TYPES];
    /* Number of houses on each square.  Should be zero for nonproperties! */

  unsigned char resyncCause;
    /* What caused this resync.  See RULE_ResyncCauses. */

  unsigned char gamePhase;
    /* A RULE_GamePhase describing what phase the game is in when it sent the
    resync.  If it is GF_ADDING_NEW_PLAYERS or GF_CONFIGURATION then you
    probably joined a game that is being set up.  Otherwise you probably
    joined a game in progress. */

  RULE_PlayerSet firstMovesMade;
    /* Identifies the players who have already made a move.  Used for showing
    the walk-on animation when the player makes the first move. */

  RULE_PlayerNumber currentPlayer;
    /* Index to current player (it's his turn). */

} RULE_ClientResyncInfoRecord, *RULE_ClientResyncInfoPointer;
  /* This tells a client that came in late to the game the entire game state
  that the client needs to know.  It isn't everything, some items are hidden
  from clients to hinder cheaters.  The names and tokens of the players are
  sent separately, so they aren't in this message record.  Also, the current
  game state is sent separately with a phase restart. */



typedef struct RULE_CashAnimationStruct
{
  long cashAfter [RULE_MAX_PLAYERS];
    /* Amount of cash each player has after the transfer. */

} RULE_CashAnimationRecord, *RULE_CashAnimationPointer;



/************************************************************/
/* EXPORTED GLOBALS                                         */
/************************************************************/

extern RULE_SquarePredefinedInfoRecord RULE_SquarePredefinedInfo [SQ_MAX_SQUARE_TYPES];
  /* This array is loaded with pretty much constant data.  Only the language
  of the text parts may change and the rents when the number of houses per
  hotel is changed. */

extern RULE_PropertySet RULE_PropertyToBitsetArray [SQ_MAX_SQUARE_TYPES];
  /* For faster inline code that finds out the bit assigned to a property.
  Equivalent to using the RULE_PropertyToBitSet function. */


/************************************************************/
/* PROTOTYPES                                               */
/************************************************************/

//extern RULE_PropertySet RULE_PropertyToBitSet (RULE_SquareType Square);
#define RULE_PropertyToBitSet(Square)   (RULE_PropertyToBitsetArray[Square])

extern RULE_SquareType RULE_BitSetToProperty (RULE_PropertySet Set);

extern void RULE_CountHousesAndHotels (RULE_GameStatePointer GamePntr,
  RULE_PlayerNumber Player, int *HouseCount, int *HotelCount);

extern RULE_PropertySet RULE_PropertySetOwnedByPlayer (
  RULE_GameStatePointer GamePntr, RULE_PlayerNumber Player);

extern void RULE_PrintSetOfPropertyNames (RULE_PropertySet PropertySet,
wchar_t *Buffer, DWORD BufferLength);

extern void RULE_FormatErrorByMessage (RULE_ActionArgumentsPointer NewMessage,
wchar_t *Buffer, long BufferSize);

extern void RULE_FormatErrorByArguments (LANG_TextMessageNumber MessageNumber,
long NumberB, long NumberC, long NumberD, long NumberE,
wchar_t *Buffer, long BufferSize);

extern LANG_TextMessageNumber RULE_TestBuildingPlacement (
  RULE_GameStatePointer GameStatePntr, RULE_PlayerNumber Purchaser,
  RULE_SquareType SquareNo, BOOL Adding, BOOL *BuildingAHotelPntr,
  int *RemainingBuildingsPntr, long *MessageArgumentPntr);

extern BOOL RULE_TestUnMortgaging (RULE_PlayerNumber PlayerNo,
RULE_SquareType SquareNo, RULE_GameStatePointer GameStatePntr,
RULE_PropertySet FreeUnMortgagingSet);

extern BOOL RULE_ConvertFileToGameOptions (BYTE *FileContents, DWORD FileSize,
  RULE_GameOptionsPointer UpdatedOptions);

extern DWORD RULE_ConvertGameOptionsToFile (RULE_GameOptionsPointer OptionsToPack,
  BYTE *OutputBuffer, DWORD BufferInitialSize);

extern BOOL AddGameOptionsToMessageBlob (RULE_GameOptionsPointer OptionsPntr,
RULE_ActionArgumentsPointer MessagePntr);

extern void RULE_ProcessRules (RULE_ActionArgumentsPointer NewActionPntr);

extern void RULE_CleanAndRemoveSystem(void);

extern BOOL RULE_InitializeSystem(void);

extern BOOL RULE_PlayersFirstMoveWasMade(int player);

extern BOOL RULE_ActionIsOfMainGameScreenPromptType( RULE_ActionType actionNumber );


/************************************************************/

#endif // __RULE_H__
