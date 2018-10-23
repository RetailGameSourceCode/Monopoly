/*****************************************************************************
 *
 *    FILE:             MESS.CPP
 *
 *   (C) Copyright 97  Artech Digital Entertainments.
 *                     All rights reserved.
 *
 * Messaging queue system and network routines.
 *
 * $Header: /Projects - 1999/Mono 2/Mess.cpp 22    30/09/99 1:49p Agmsmith $
 *
 * $Log: /Projects - 1999/Mono 2/Mess.cpp $
 * 
 * 22    30/09/99 1:49p Agmsmith
 * Now with out of order reception of voice chat messages so that the game
 * can ignore the regular message queue but still receive voice chat.
 * 
 * 21    28/09/99 4:26p Agmsmith
 * Exclude IPX from the list of services.
 * 
 * 20    22/09/99 4:20p Agmsmith
 * Oops.
 * 
 * 19    99/09/10 11:21a Agmsmith
 * Only ask network questions if certain keywords are on the command line,
 * since the command line can be non-empty in other situations.
 *
 * 18    9/07/99 3:48p Philla
 * Live update of rules screen for non-host players.
 *
 * 17    99/09/01 7:06p Agmsmith
 * Don't ask network play questions if no command line.
 *
 * 16    99/09/01 6:46p Agmsmith
 * Lobby startup can pass in a command line, so check for a lobby ignoring
 * the presence or absence of command line data.
 *
 * 15    99/09/01 2:37p Agmsmith
 * Added Lobby score update functions.
 *
 * 14    99/08/30 3:24p Agmsmith
 * Updated to work with DirectX 6.1 (DPlay4, Lobby3).
 *
 * 13    99-07-05 9:52 Timp
 * Removed references to cmdline options in language file
 *
 * 12    6/02/99 8:49a Davide
 * added a case for NOTIFY_ACCEPT_CONFIG
 *
 * 11    27/04/99 19:32 Timp
 * MESS_StartNetworking now takes Unicode strings
 *
 * 10    4/19/99 10:36a Russellk
 * action notification returned to full use - button presses will check
 * these to show graphics (or beep)
 *
 * 9     3/12/99 4:10p Russellk
 * Trade screen started, HIDE functions removed - setting the background
 * to black does the whole job anyway.
 *
 * 8     3/04/99 8:42a Russellk
 * Optimization, action notifications for human players restricted.
 *
 * 7     3/02/99 2:06p Russellk
 *
 * 6     2/25/99 12:54p Russellk
 * Rough patch for Direct Play in, see FIXME
 *
 * 5     2/08/99 2:07p Russellk
 * Removed NOTIFY ACTION COMPLETED for non-AI targets (save 5 to 10 %
 * network traffic)
 *
 * 4     2/08/99 9:11a Russellk
 * Added state NOTIFY_TURN_END - a rather stupid end of turn
 * acknowledgement.  Useful for email monopoly, Hasbro wants it in at this
 * point.
 *
 * 3     1/28/99 10:01a Russellk
 * Networking fixed.  Log could go into a negative index since network
 * queue pushes aren't logged - array subscript violation.
 *
 * 2     1/27/99 10:59a Russellk
 * Basic Mono2 display and user interface operational - no trade editor
 * nor game config editory.
 *
 * 1     1/08/99 2:06p Russellk
 *
 * 1     1/08/99 11:35a Russellk
 * Graphics
 *
 * 6     1/04/99 1:20p Russellk
 * Include file change for gameinc.h
 *
 * 5     1/04/99 11:39a Russellk
 * Game state restore added to log
 *
 * 4     12/24/98 11:09a Russellk
 * Log file bug - maximum levels too deep exceeded spacer variable length.
 *
 * 3     12/23/98 3:41p Russellk
 * Added a comment on GUID
 *
 * 2     12/17/98 4:26p Russellk
 * Obsolete comments removed for new version.  Log file facility entered
 * in Mess.c to record phase and action/error message activity.
 *
 ************************************************************/

#include "gameinc.h"

// Networking stuff.
#include <winsock.h>
#define IDIRECTPLAY2_OR_GREATER 1 /* Don't include obsolete symbols. */
#include <dplobby.h>  /* Also includes dplay.h, available in DirectX 5.0 */



/************************************************************/
/* EXPORTED GLOBALS                                         */
/************************************************************/

BOOL MESS_NetworkMode;
  /* TRUE if we are playing a network game.  FALSE if the network
  is turned off. */

BOOL MESS_ServerMode;
  /* TRUE when this program is running the server (local mode or this
  is the server computer).  FALSE when it is just a client (Rules
  engine not running). */

BOOL MESS_WinSockSystemActive;
  /* TRUE if we have the WinSock package going. */

BOOL MESS_DirectPlaySystemActive;
  /* TRUE if the DirectPlay package is operational. */

BOOL MESS_GameStartedByLobby;
  /* TRUE if the game was started by the Microsoft Zone Lobby or some
  other lobby system.  That means that the player name should be taken
  from MESS_PlayerNameFromNetwork rather than letting the player type
  in their name (otherwise they could forge someone else's name and
  thus mess up the Zone's high score tables).  If there is another
  local human player, then you can ask them to type in their name, or
  don't allow more than one local human. */

BOOL MESS_EnableMessageTrapping = FALSE;
  /* TRUE if trapping messages to the game history is active. */

wchar_t MESS_PlayerNameFromNetwork [RULE_MAX_PLAYER_NAME_LENGTH+1];
  /* The name of the player that we obtained from the network.  In Zone
  networking it is their Zone user id.  With DirectPlay it is their Windows
  logon ID.  It is an empty string under WinSock or non-network play.
  The command line arguments can also set this name, using the
  T_MESS_ARGS_PLAYERNAME keyword.  See also MESS_GameStartedByLobby. */



/****************************************************************************
 * PRIVATE GLOBALS, TYPEDEFS and MACROS                                     *
 ****************************************************************************/

#define MAX_WINSOCK_PLAYERS 30
#define MAX_DIRECTPLAY_PLAYERS 30
#define MAX_CONNECTED_COMPUTERS (MAX_WINSOCK_PLAYERS + MAX_DIRECTPLAY_PLAYERS)
  /* Maximum number of connections allowed, many are spectators. */

static LPDIRECTPLAYLOBBY3 pDPLobbyN;
  /* Version N (latest one we know of) of the lobby COM interface.  Basically
  a table of function pointers to the lobby methods.  Non-NULL if open,
  meaning DirectPlay is running and we started from a lobby pDPlayN will also
  be open in this case).  Will be opened in Unicode mode. */

static BOOL LobbySupportsStandardMessages;
  /* If TRUE then we know the lobby can handle standard messages, such as
  the ones that notify it about the current score.  If FALSE, don't send
  messages to the lobby. */

static BOOL LobbySupportsGameStatusUpdateMessages;
  /* If TRUE then we know the lobby can handle our custom update
  messages, the ones sending a MESS_ZoneGameUpdateInfoRecord
  using the ZoneGameStatusUpdateGUID as the identifier.  Usually
  starts off as TRUE until the lobby tells us it can't handle
  this kind of message. */

static LPDIRECTPLAY4 pDPlayN;
  /* Version N (latest one we know about) of the DirectPlay COM interface.
  Non-NULL if it has been opened and is operational.  Doesn't need to have
  the lobby open to work.  Will be opened in Unicode mode. */

static BOOL DPlaySessionIsOpen;
  /* TRUE if a game session has been opened and needs to be closed on exit. */

static DPID MyDPPlayerID;
  /* The DirectPlay "player" that is representing this computer.
  Zero if none. */

static GUID MyDPLPlayerGuid;
  /* The lobby's reference to the directPlay "player" that is representing
  this computer.  Set to all zeroes if none. */

typedef enum TransmitStatesEnum
    {
    NO_TRANSMISSION_NEEDED = 0,
    /* Used when the message doesn't need to be sent over the network, or when
    it has been completely sent. */

    TRANSMISSION_NEEDED,
    /* Used when it needs to be sent but nothing has been started, messages
    added to the queue with TO addresses that aren't local are marked with
    this value, will switch to TRANSMISSION_IN_PROGRESS when transmission
    actually starts. */

    TRANSMISSION_IN_PROGRESS,
    /* Used while parts of the message are going over the network, stays in
    this state until it has all been sent, then goes to NO_TRANSMISSION_NEEDED. */

    MAX_TRANSMIT_STATES
    }
    TransmitStates;
  /* The various states of transmission.  See the transmitState array in the
  message queue. */

#define MAX_MESSAGES_IN_QUEUE (RULE_MAX_PLAYERS * 3 + 3 * SQ_MAX_SQUARE_TYPES)
  /* We can hold this many pending messages in our queue.  When someone goes
  bankrupt, their houses are removed, cash is transfered for each, and
  ownership is changed for the square - thus 3 * number of squares. */

typedef struct MessageQueueStruct
    {
    RULE_ActionArgumentsRecord messages [MAX_MESSAGES_IN_QUEUE];
    /* The actual message contents. */

    char transmitState [MAX_MESSAGES_IN_QUEUE];
    /* TRUE if this message needs transmitting, FALSE if it has already been
    sent or is a local message.  A message stays at the tail of the queue until
    it has been transmitted by all relevent network protocols, then it is
    available for reception by the local message consumers.  That way we don't
    need extra transmit queue space. */

    int head; /* Points to top of queue (new items added after it), -1 if queue empty. */
    int tail; /* Points to item at bottom of queue (items removed).  Undefined when empty. */
    }
    MessageQueueRecord, *MessageQueuePointer;

static MessageQueueRecord MessageQueue;
  /* All our simulated and actual received messages are in this queue. */

static RULE_NetworkAddressRecord AddressesForPlayers [RULE_BANK_PLAYER+1];
  /* Network addresses of all known players and of the bank (server). */

typedef struct ComputerTrackingStruct
    {
    RULE_NetworkAddressRecord computerID;
    /* A value of zero (normally used for local players) means not connected;
    this entry is free for use. */

    BOOL communicationsAllowed;
    /* TRUE if this computer is allowed to talk and listen.  This is used to
    temporarily block out newly joined computers so that they don't get
    confused by auctions or trades.  They get allowed into the game only
    during dice rolls and other quiet times. */
    }
    ComputerTrackingRecord, *ComputerTrackingPointer;

static ComputerTrackingRecord ComputerList [MAX_CONNECTED_COMPUTERS];
  /* Keeps track of all the computer systems we know about.  Clients only have
  one entry here (the host), the host has lots. */

static BOOL NewComputersJoinedRecently;
  /* Set to TRUE when new computer connections are detected.  Set to FALSE
  when MESS_LetNewComputersIntoTheGame is called. */

typedef struct NetAddressForDirectPlayStruct
    {
    RULE_NetworkSystem  networkSystem; /* Should be NS_DIRECTPLAY. */
    DPID                playerID;
    struct sockaddr_in  filler; /* Not used, just for padding. */
    }
    NetAddressForDirectPlayRecord, *NetAddressForDirectPlayPointer;
    /* A variant of the RULE_NetworkAddressRecord
       which is used for DirectPlay addresses. */

typedef struct NetAddressForWinSockStruct
    {
    RULE_NetworkSystem  networkSystem; /* Should be NS_WINSOCK. */
    DWORD               connectionIndex; /* Index into WinSockPartialData. */
    struct sockaddr_in  IPAddress; /* The IP address and port etc. */
    }
    NetAddressForWinSockRecord, *NetAddressForWinSockPointer;
    /* A variant of the RULE_NetworkAddressRecord
       which is used for WinSock addresses. */

static const char MonopolyMessageHeaderMarker [] = "M1n9p9y9";
  /* A special 8 byte header that appears at the start of a Monopoly1999a
  message that is transmitted over a stream medium, to separate the messages
  and make it easy to find the next message after a transmission error.  This
  magic code is followed by the message size in 4 little endian bytes.  After
  that comes a MonopolyNetMessageHeaderRecord and its associated data.  It
  isn't used for DirectPlay messages, just WinSock and other stream ones. */

#define MONOPOLY_MESSAGE_HEADER_LENGTH 12
  /* Length of MonopolyMessageHeaderMarker + 4 for the following DWORD.
  Here to avoid doing strlen() every time we need it. */

typedef struct MonopolyNetMessageHeaderStruct
    {
    unsigned char action;
    unsigned char fromPlayer;
    unsigned char toPlayer;
    unsigned char filler1;
    long  numberA;
    long  numberB;
    long  numberC;
    long  numberD;
    long  numberE;
    /* Variable length stuff follows after here, a DWORD byte length and data
       for the StringA value then another DWORD length and data for the blob.
       All these use little endian byte ordering (least significant byte first).
       The length fields are here to make it easy to store the typical record,
       which has no string and no blob. */
    DWORD stringALength;
    DWORD blobLength;
    }
    MonopolyNetMessageHeaderRecord, *MonopolyNetMessageHeaderPointer;
    /* This is the format that a RULE_ActionArgumentsRecord message
       is sent in over the network. */

typedef struct NetMessageWithSeparatorStruct
    {
    char                            messageSeparatorHeader [MONOPOLY_MESSAGE_HEADER_LENGTH - sizeof (DWORD)];
    DWORD                           messageSeparatorSize;
    MonopolyNetMessageHeaderRecord  monopolyNetMessage;
    }
    NetMessageWithSeparatorRecord, *NetMessageWithSeparatorPointer;
    /* Same as above but with space for the TCP/IP message separator. */

typedef struct PartialDataStruct
    {
    SOCKET socketID;
    /* The socket that is being used to send and receive data.
    Set to INVALID_SOCKET when this whole record isn't active. */

    BYTE *receiveBuffer;
    /* Points to a buffer where received data is being accumulated until
    a full message has been received.  If the message is short, this points
    at the shortMonopolyMessage field in this record.  If it is a long
    message then it points to storage allocated with GlobalAllocPtr.
    NULL means we should start looking for a message header. */

    DWORD amountReceivedSoFar;
    /* Amount of valid data in receiveBuffer. */

    DWORD totalToReceive;
    /* Set to non-zero if we are in the process of receiving a message or
    collecting a header.  The various bits accumulate until amountReceived
    equals totalToReceive, which is when we have got it all. */

    MonopolyNetMessageHeaderRecord shortMonopolyMessage;
    /* Space to accumulate short messages, receiveBuffer often points to
    this field. */

    BYTE *transmitBuffer;
    /* Points to a buffer where data waiting to be transmitted is being
    held until it has all been sent.  NULL if no data, otherwise points to
    storage allocated by someone else. */

    DWORD amountTransmitted;
    /* Amount of data in transmitBuffer that has already been sent.  Starts
    at zero and grows up to totalToTransmit. */

    DWORD totalToTransmit;
    /* Set to non-zero if we are in the process of sending a delayed message
    (one that TCP/IP can't send all in one shot). */

    NetAddressForWinSockRecord standardAddress;
    /* Address of the computer on the other end in a format that
    the rules engine can use. */
    }
    PartialDataRecord, *PartialDataPointer;
    /* Keeps track of data being received from one particular computer. */

static PartialDataRecord WinSockPartialData [MAX_WINSOCK_PLAYERS];
  /* Keeps track of the data coming in from all the players connected via
  WinSock, and stuff going out.  Some of them may be actual players,
  others may be spectators. */

static short TCPPortNumber = 19991;
  /* Port number to use for TCP/IP connections.  Should be over 1024 to avoid
  the standard ports used by FTP, Telnet, etc.  We use 19991 derived from the
  Monopoly1999a communications protocol name. */

static SOCKET ServerListenSocket = INVALID_SOCKET;
  /* A socket that the server uses to listen for incoming calls.
  INVALID_SOCKET if not allocated yet (socket zero may be valid). */

static HINSTANCE WinsockLibraryInstance;
  /* Identifies the Winsock DLL that we are using.  NULL if Winsock
  is unavailable. */

static BOOL NeedToFreeLibraryWinsock;
  /* TRUE if we had to load the library and thus have to free it
  when the program exits. */

/* Pointers to Winsock functions.  Used instead of link time
   DLLs so that the program can run even if the WinSock DLL isn't there. */

typedef SOCKET (PASCAL FAR *type_accept) (SOCKET s, struct sockaddr FAR *addr, int FAR *addrlen);
typedef int (PASCAL FAR *type_bind) (SOCKET s, const struct sockaddr FAR *addr, int namelen);
typedef int (PASCAL FAR *type_closesocket) (SOCKET s);
typedef int (PASCAL FAR *type_connect) (SOCKET s, const struct sockaddr FAR *name, int namelen);
typedef struct hostent FAR * (PASCAL FAR *type_gethostbyname) (const char FAR * name);
typedef int (PASCAL FAR *type_gethostname) (char FAR * name, int namelen);
typedef u_short (PASCAL FAR *type_htons) (u_short hostshort);
typedef unsigned long (PASCAL FAR *type_inet_addr) (const char FAR * cp);
typedef char FAR * (PASCAL FAR *type_inet_ntoa) (struct in_addr in);
typedef int (PASCAL FAR *type_ioctlsocket) (SOCKET s, long cmd, u_long FAR *argp);
typedef int (PASCAL FAR *type_listen) (SOCKET s, int backlog);
typedef int (PASCAL FAR *type_recv) (SOCKET s, char FAR * buf, int len, int flags);
typedef int (PASCAL FAR *type_send) (SOCKET s, const char FAR * buf, int len, int flags);
typedef int (PASCAL FAR *type_setsockopt) (SOCKET s, int level, int optname, const char FAR * optval, int optlen);
typedef SOCKET (PASCAL FAR *type_socket) (int af, int type, int protocol);
typedef int (PASCAL FAR *type_WSAAsyncSelect) (SOCKET s, HWND hWnd, u_int wMsg, long lEvent);
typedef int (PASCAL FAR *type_WSACleanup) (void);
typedef int (PASCAL FAR *type_WSAGetLastError) (void);
typedef int (PASCAL FAR *type_WSAStartup) (WORD wVersionRequired, LPWSADATA lpWSAData);

static type_accept pfaccept;
static type_bind pfbind;
static type_closesocket pfclosesocket;
static type_connect pfconnect;
static type_gethostbyname pfgethostbyname;
static type_gethostname pfgethostname;
static type_htons pfhtons;
static type_inet_addr pfinet_addr;
static type_inet_ntoa pfinet_ntoa;
static type_ioctlsocket pfioctlsocket;
static type_listen pflisten;
static type_recv pfrecv;
static type_send pfsend;
static type_setsockopt pfsetsockopt;
static type_socket pfsocket;
static type_WSAAsyncSelect pfWSAAsyncSelect;
static type_WSACleanup pfWSACleanup;
static type_WSAGetLastError pfWSAGetLastError;
static type_WSAStartup pfWSAStartup;

#define Callaccept pfaccept
#define Callbind pfbind
#define Callclosesocket pfclosesocket
#define Callconnect pfconnect
#define Callgethostbyname pfgethostbyname
#define Callgethostname pfgethostname
#define Callhtons pfhtons
#define Callinet_addr pfinet_addr
#define Callinet_ntoa pfinet_ntoa
#define Callioctlsocket pfioctlsocket
#define Calllisten pflisten
#define Callrecv pfrecv
#define Callsend pfsend
#define Callsetsockopt pfsetsockopt
#define Callsocket pfsocket
#define CallWSAAsyncSelect pfWSAAsyncSelect
#define CallWSACleanup pfWSACleanup
#define CallWSAGetLastError pfWSAGetLastError
#define CallWSAStartup pfWSAStartup

static const GUID Monopoly1999aGUID =
    {
    /* {00EC277C-3B19-48b6-AA11-04229BEA7717} */
    0xec277c,
    0x3b19,
    0x48b6,
    {0xaa, 0x11, 0x4, 0x22, 0x9b, 0xea, 0x77, 0x17 }
    };
    /* Identifies Monopoly 2000 game sessions in DirectPlay,
    as contrasted to chess games and other games.  In other words,
    games using the Monopoly1999a network protocol. */



static const GUID ZoneGameStatusUpdateGUID =
    {
    /* {6F403220-C5E1-4cb2-A51D-BE0EE512B65C} */
    0x6f403220,
    0xc5e1,
    0x4cb2,
    { 0xa5, 0x1d, 0xbe, 0xe, 0xe5, 0x12, 0xb6, 0x5c }
    };
    /* Identifies the block of data sent to the lobby which contains the
    current game status, so that the lobby can update its display.  See
    also MESS_ZoneGameUpdateInfoRecord. */



static const GUID ZONEPROPERTY_GameStateGUID =
    {
    /* {BDD4B95F-D35C-11d0-B625-00C04FC33EA1} */
    0xbdd4b95f,
    0xd35c,
    0x11d0,
    { 0xb6, 0x25, 0x0, 0xc0, 0x4f, 0xc3, 0x3e, 0xa1}
    };
    /* This one is provided by the Microsoft Zone people, used for telling
    the lobby that the game has really started or really ended.  See
    ZONEPROPERTY_GameState in ZoneLobby.h. */

#define ZSTATE_START  0x00000001
#define ZSTATE_END    0x00000002
    /* Values for use with ZONEPROPERTY_GameState, see ZoneLobby.h. */



#define MESS_MAX_COMMAND_LINE_LENGTH 1000
  /* Number of unicode characters allowed in the command line. */

#define MESS_MAX_PROMPT_TEXT_LENGTH 1000
  /* Length of a message string used in dialog boxes to ask questions. */

#define MAX_DP_NAMES_LENGTH 199 /* Maximum size of a session or player name */
#define DPM_TIMER_ID      16234 /* Timer ID to use in our dialog box. */
#define DPM_TIMER_INTERVAL 3000 /* Timer period in milliseconds. */

static BOOL DPSessionBoxTimerEnabled;
  /* TRUE when the DirectPlay Session selection dialog box timer is allowed to
  update the session list.  FALSE when the session list shouldn't be updated. */



/*****************************************************************************
 * Deallocates any storage associated with the message.  Use this after you
 * have finished with a message you got from MESS_ReceiveActionMessage.
 */
void MESS_FreeStorageAssociatedWithMessage (RULE_ActionArgumentsPointer MessagePntr)
{
    if (MessagePntr->binaryDataHandleA != NULL)
        {
        if (MessagePntr->binaryDataA != NULL)
            RULE_UnlockHandle (MessagePntr->binaryDataHandleA);
        RULE_FreeHandle (MessagePntr->binaryDataHandleA);
        }

  MessagePntr->binaryDataHandleA = NULL;
  MessagePntr->binaryDataA = NULL;
}

/*****************************************************************************
 * Attempt to add the item to the top of the queue.  Returns FALSE if the
 * queue is full (and cleans up the binary attachment if there is one).
 */

static BOOL AddToQueue (MessageQueuePointer Queue, RULE_ActionArgumentsPointer Item)
{
    int NewIndex;

    if (Queue->head < 0)
        {
        /* Adding to an empty queue. */
        Queue->tail = Queue->head = 0;
        }
    else
        {
        NewIndex = Queue->head + 1;
        if (NewIndex >= MAX_MESSAGES_IN_QUEUE)
            NewIndex = 0;
        if (NewIndex == Queue->tail) /* Is Queue full? */
            {
            /* Deallocate attached blob. */
            MESS_FreeStorageAssociatedWithMessage (Item);
            return FALSE;
            }
        Queue->head = NewIndex;
        }

  memcpy (Queue->messages + Queue->head, Item, sizeof (RULE_ActionArgumentsRecord));
  Item->binaryDataHandleA = NULL;  /* It's in the queue now, not in Item. */
  Item->binaryDataA = NULL;
  return TRUE;
}

/*****************************************************************************
 * Attempt to remove an item from the bottom of the queue.  Returns FALSE if
 * the queue is empty.  Otherwise fills in the Item's fields.
 */

static BOOL RemoveFromQueue (MessageQueuePointer Queue, RULE_ActionArgumentsPointer Item)
{
    int NewIndex;

    if (Queue->head < 0)
        return FALSE; /* Empty queue. */

    memcpy (Item, Queue->messages + Queue->tail, sizeof (RULE_ActionArgumentsRecord));

    /* Get a pointer to the blob, if there is one. */

    if (Item->binaryDataHandleA != NULL && Item->binaryDataA == NULL)
        {
        Item->binaryDataA = (unsigned char *) RULE_LockHandle (Item->binaryDataHandleA);
        if (Item->binaryDataA != NULL)
            Item->binaryDataA += Item->binaryDataStartOffsetA;
        }

    /* Update queue pointers. */
    if (Queue->head == Queue->tail)
        Queue->head = -1; /* Queue is now empty. */
    else
        {
        NewIndex = Queue->tail + 1;
        if (NewIndex >= MAX_MESSAGES_IN_QUEUE)
            NewIndex = 0;
        Queue->tail = NewIndex;
        }

    return TRUE;
}

/*****************************************************************************
 * Sends a message from a player or server to the server or a player.  Can
 * broadcast too (send to RULE_ALL_PLAYERS to broadcast, send to
 * RULE_BANK_PLAYER to send to the server).  The message is consumed by this
 * function, which means that the handle to a large binary block will be freed.
 *
 * Returns TRUE if it was enqueued successfully, returns FALSE if the queue
 * is full.  It actually gets sent later on (done in the receive function).
 */
BOOL MESS_SendActionMessage (RULE_ActionArgumentsPointer MessageToSend)
{

   if (MessageToSend->toPlayer <= RULE_NOBODY_PLAYER)
        {
        #if (LE_CMAIN_EnableDebugMode || (FORHOTSEAT && _DEBUG))
            char  TempString [256];
            /* Check that the player claiming to send the message is truely a local player. */
            if ((MessageToSend->fromPlayer == RULE_BANK_PLAYER && !MESS_ServerMode) ||
                (MessageToSend->fromPlayer < RULE_MAX_PLAYERS && MessageToSend->fromPlayer >= UICurrentGameState.NumberOfPlayers) ||
                (MessageToSend->fromPlayer < RULE_MAX_PLAYERS && !SlotIsALocalPlayer [MessageToSend->fromPlayer]))
                {

                sprintf (TempString,
                    "MESS Message from bad player ID: from:%d to:%d action:%d A:%d B:%d C:%d D:%d E:%d\r\n",
                    (int) MessageToSend->fromPlayer, (int) MessageToSend->toPlayer,
                    (int) MessageToSend->action, MessageToSend->numberA, MessageToSend->numberB,
                MessageToSend->numberC, MessageToSend->numberD, MessageToSend->numberE);
                DBUG_DisplayNonFatalErrorMessage (TempString);
                #if !FORHOTSEAT
                    LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);
                #endif
                }
        #endif

        #if MESS_REPORT_ACTION_MESSAGES
            MESS_updateMessageLog( MessageToSend, TRUE );
        #endif

        if (!AddToQueue (&MessageQueue, MessageToSend))
            return FALSE; /* Out of space in the queue. */

        if (MESS_NetworkMode)
            {
            /* Mark the message for transmission if it is to a remote
               player or if it is a broadcast. */
            if (MessageToSend->toPlayer <= RULE_BANK_PLAYER)
                {
                if (AddressesForPlayers [MessageToSend->toPlayer].networkSystem != NS_LOCAL)
                    MessageQueue.transmitState[MessageQueue.head] = TRANSMISSION_NEEDED;
                else
                    MessageQueue.transmitState[MessageQueue.head] = NO_TRANSMISSION_NEEDED;
                }
            else /* A broadcast. */
                MessageQueue.transmitState[MessageQueue.head] = TRANSMISSION_NEEDED;
            }
        else /* Not running in network mode, don't need to transmit. */
            MessageQueue.transmitState[MessageQueue.head] = NO_TRANSMISSION_NEEDED;

        return TRUE;
        }

    #if _DEBUG
        DBUG_DisplayNonFatalErrorMessage ("SendActionMessage: Called with bad player ID.");
    #endif

    MESS_FreeStorageAssociatedWithMessage (MessageToSend);
    return FALSE;
}

/*****************************************************************************
 * Utility function for telling the players about some action.  The
 * arguments correspond to the fields in an RULE_ActionArgumentsRecord (BinarySize
 * is size of stringA in bytes, zero to use string length instead).  The Blob
 * (an unlocked handle) will be deallocated by the system when the message is
 * consumed, so you shouldn't reference it any more.
 */

BOOL MESS_SendAction2 (RULE_ActionType Action, RULE_PlayerNumber FromPlayer,
RULE_PlayerNumber ToPlayer, long NumberA, long NumberB, long NumberC, long NumberD,
long NumberE, wchar_t *StringA, unsigned int BinarySize, RULE_CharHandle Blob)
{
    RULE_ActionArgumentsRecord NewAction;
    BOOL                       Success;

    memset (&NewAction, 0, sizeof (NewAction));

    NewAction.action = Action;
    NewAction.fromPlayer = FromPlayer;
    #if _DEBUG
        if (ToPlayer == RULE_NOBODY_PLAYER && FromPlayer != RULE_BANK_PLAYER)
            DBUG_DisplayNonFatalErrorMessage ("MESS_SendAction2: Non-server sending message to all players.");
    #endif
    NewAction.toPlayer = ToPlayer;
    NewAction.numberA = NumberA;
    NewAction.numberB = NumberB;
    NewAction.numberC = NumberC;
    NewAction.numberD = NumberD;
    NewAction.numberE = NumberE;


    // FIXME - test - NOTIFY_ACTION_COMPLETED for non-ai's is required
    // for only a few states.  Limiting traffic is good, conditions here...
    //if( (NewAction.action == RULE_MAX_PLAYERS) &&

    /*//Removed - button pressed animations need the notification.  Perhaps specific messages could be skipped?
    if( (NewAction.action == NOTIFY_ACTION_COMPLETED) &&
      //(UICurrentGameState.Players[NewAction.toPlayer].AIPlayerLevel > 0) && // Bank player!  Wrong!
      (UICurrentGameState.Players[NewAction.numberC].AIPlayerLevel == 0) &&
      (NewAction.numberA != ACTION_PLAYER_BUYSELLMORT) &&
      (NewAction.numberA != ACTION_PLAYER_DONE_BUYSELLMORT)
      )
      return( FALSE );  // no need to send message to a human player.*/

    /* Set the string field.  Either copy binary data, copy a string or
       send NULs (leave it zero) if there is nothing to send. */
    if (StringA != NULL)
        {
        if (BinarySize == 0) /* Sending a string. */
            wcsncpy  (NewAction.stringA, StringA, RULE_ACTION_ARGUMENT_STRING_LENGTH);
        else /* Some data to copy. */
            {
            if (BinarySize > sizeof (NewAction.stringA))
                {
                #if _DEBUG
                    DBUG_DisplayNonFatalErrorMessage ("SendAction: Short binary item (StringA) too big to fit.");
                #endif
                BinarySize = sizeof (NewAction.stringA);
                }
            memcpy (NewAction.stringA, StringA, BinarySize);
            }
        }

    /* Set up the blob information. */
    if (Blob != NULL)
        {
        NewAction.binaryDataSizeA = RULE_HandleSize (Blob);
        if (NewAction.binaryDataSizeA > 0)
            {
            NewAction.binaryDataHandleA = Blob;
            /* Redundant - already zeroed
            NewAction.binaryDataStartOffsetA = 0;
            NewAction.binaryDataA = 0; */
            }
        else /* A bad blob. */
            {
            #if _DEBUG
                DBUG_DisplayNonFatalErrorMessage ("SendAction: Bad binary object handle.");
            #endif
            RULE_FreeHandle (Blob);
            }
        }

    Success = MESS_SendActionMessage (&NewAction);

    #if _DEBUG
        if (!Success)
            DBUG_DisplayNonFatalErrorMessage ("SendAction: Couldn't send message.");
    #endif

    return Success;
}

/*****************************************************************************
 * Older version that only goes up to NumberD.
 */
BOOL MESS_SendAction (RULE_ActionType Action, RULE_PlayerNumber FromPlayer,
RULE_PlayerNumber ToPlayer, long NumberA, long NumberB, long NumberC, long NumberD,
wchar_t *StringA, unsigned int BinarySize, RULE_CharHandle Blob)
{
  return MESS_SendAction2 (Action, FromPlayer, ToPlayer, NumberA, NumberB, NumberC,
    NumberD, 0 /* NumberE */, StringA, BinarySize, Blob);
}

/*****************************************************************************
 * Get the number of messages currently in the message queue.  This includes
 * both outgoing and incoming messages.
 */
DWORD MESS_CurrentQueueSize (void)
{
  int QueueSize;

  if (MessageQueue.head < 0)
    QueueSize = 0; /* Empty queue. */
  else
  {
    QueueSize = MessageQueue.head - MessageQueue.tail + 1;
    if (MessageQueue.head < MessageQueue.tail)
      QueueSize += MAX_MESSAGES_IN_QUEUE;
  }

  return (DWORD) QueueSize;
}

/*****************************************************************************
 * Given a block of received data, convert it into the internal message format
 * and add it to the queue.
 *
 * If you specify a non-NULL ReceivedHandle (the handle corresponding to
 * ReceivedBuffer) then the memory for the buffer will be reused and later
 * deallocated by the message queue system (the caller shouldn't deallocate
 * it).  This saves on copying and allocating large buffers for large
 * messages.  If you pass in NULL, space will be allocated if needed.
 */

static void UnmarshallAndQueueMessage (BYTE *ReceivedBuffer, DWORD ReceivedSize,
RULE_CharHandle ReceivedHandle, RULE_NetworkAddressPointer ReceivedAddress)
{
  MonopolyNetMessageHeaderPointer BufferAsNetMessagePntr = (MonopolyNetMessageHeaderStruct *) ReceivedBuffer;
  BYTE                           *BytePntr;
  DWORD                           CopyAmount;
  DWORD                          *DWordPntr;
  DWORD                           FieldLength;
  int                             i;
  RULE_ActionArgumentsRecord      ReceivedMonopolyMessage;
  ComputerTrackingPointer         TempComputerPntr;

  /* Copy the data out of the received Monopoly message into a local
     message and add it to the received messages queue.  Large binary
     objects can be referenced from your buffer or have a separate
     allocation if you don't want to hand over control of your buffer's
     deallocation.  If things go wrong, as much of the message as can
     be processed will be used. */

  memset (&ReceivedMonopolyMessage, 0, sizeof (ReceivedMonopolyMessage));
  memcpy (&ReceivedMonopolyMessage.fromNetworkAddress, ReceivedAddress,
    sizeof (RULE_NetworkAddressRecord));

  if (ReceivedSize >= sizeof (MonopolyNetMessageHeaderRecord))
  {
    ReceivedMonopolyMessage.action = BufferAsNetMessagePntr->action;
    ReceivedMonopolyMessage.fromPlayer = BufferAsNetMessagePntr->fromPlayer;
    ReceivedMonopolyMessage.toPlayer = BufferAsNetMessagePntr->toPlayer;
    ReceivedMonopolyMessage.numberA = BufferAsNetMessagePntr->numberA;
    ReceivedMonopolyMessage.numberB = BufferAsNetMessagePntr->numberB;
    ReceivedMonopolyMessage.numberC = BufferAsNetMessagePntr->numberC;
    ReceivedMonopolyMessage.numberD = BufferAsNetMessagePntr->numberD;
    ReceivedMonopolyMessage.numberE = BufferAsNetMessagePntr->numberE;

    /* Copy the StringA field, if it is there. */

    BytePntr = (unsigned char *) &BufferAsNetMessagePntr->stringALength;

    DWordPntr = (unsigned long *) BytePntr;
    FieldLength = *DWordPntr;
    BytePntr += sizeof (DWORD); /* Point at the field's data. */
    if (((BytePntr - ReceivedBuffer) + FieldLength +
    sizeof (DWORD) /* For next field's size. */) <= ReceivedSize)
    {
      if (FieldLength > 0)
      {
        CopyAmount = FieldLength;
        if (CopyAmount > sizeof (ReceivedMonopolyMessage.stringA))
          CopyAmount = sizeof (ReceivedMonopolyMessage.stringA);
        memcpy (ReceivedMonopolyMessage.stringA, BytePntr, CopyAmount);
      }
      BytePntr += FieldLength; /* Advance to next field. */

      /* Now see if there is a following binary blob. */

      DWordPntr = (unsigned long *) BytePntr;
      FieldLength = *DWordPntr;
      BytePntr += sizeof (DWORD); /* Point at the field's data. */
      if (((BytePntr - ReceivedBuffer) + FieldLength) <= ReceivedSize)
      {
        if (FieldLength > 0)
        {
          /* Yes, there is a legitimate blob.  Allocate space if needed,
             or reuse the user provided buffer. */

          if (ReceivedHandle != NULL)
          {
            /* Reuse the user's buffer.  Just add a reference to
               the location of the blob in it. */

            ReceivedMonopolyMessage.binaryDataHandleA = ReceivedHandle;
            ReceivedMonopolyMessage.binaryDataSizeA = FieldLength;
            ReceivedMonopolyMessage.binaryDataStartOffsetA =
              BytePntr - ReceivedBuffer;
            ReceivedMonopolyMessage.binaryDataA = BytePntr;
          }
          else /* Have to allocate our own space for the blob. */
          {
            ReceivedMonopolyMessage.binaryDataHandleA = RULE_AllocateHandle (FieldLength);
            if (ReceivedMonopolyMessage.binaryDataHandleA != NULL)
            {
              ReceivedMonopolyMessage.binaryDataSizeA = FieldLength;
              ReceivedMonopolyMessage.binaryDataStartOffsetA = 0;
              ReceivedMonopolyMessage.binaryDataA =
                (unsigned char *)RULE_LockHandle (ReceivedMonopolyMessage.binaryDataHandleA);
              memcpy (ReceivedMonopolyMessage.binaryDataA, BytePntr, FieldLength);
              RULE_UnlockHandle (ReceivedMonopolyMessage.binaryDataHandleA);
              ReceivedMonopolyMessage.binaryDataA = NULL;
            }
          }
        }
      }
    }
  }

  /* If we didn't use the caller's memory handle, deallocate it. */

  if (ReceivedHandle != NULL && ReceivedMonopolyMessage.binaryDataHandleA == NULL)
  {
    RULE_UnlockHandle (ReceivedHandle);
    RULE_FreeHandle (ReceivedHandle);
  }

  /* See if we are listening to that computer.  Sometimes, we ignore new
     computers until the game is in a good spot. */

  if (NewComputersJoinedRecently)
  {
    for (i = 0; i < MAX_CONNECTED_COMPUTERS; i++)
    {
      TempComputerPntr = ComputerList + i;
      if (TempComputerPntr->computerID.networkSystem != NS_LOCAL &&
      memcmp (&TempComputerPntr->computerID, ReceivedAddress,
      sizeof (RULE_NetworkAddressRecord)) == 0)
      {
        /* We aren't listening to this new computer. */

        MESS_FreeStorageAssociatedWithMessage (&ReceivedMonopolyMessage);
        return;
      }
    }
  }

  AddToQueue (&MessageQueue, &ReceivedMonopolyMessage);
}



/*****************************************************************************
 * A little utility function for turning a button on and off.
 */

static void EnableDialogControl (HWND hDlg, int nIDDlgItem, BOOL bEnable)
{
  HWND hControl;
  hControl = GetDlgItem (hDlg, nIDDlgItem);
  if (hControl != NULL)
    EnableWindow (hControl, bEnable);
}



/*****************************************************************************
 * When someone gets disconnected, we have to clean up a few things and
 * tell the rules engine about it.
 */

static void UpdateForDisconnectedConversation (
RULE_NetworkAddressPointer DisconnectedAddress)
{
  int                       i;
  RULE_PlayerNumber         PlayerNo;
  RULE_PlayerSet            PlayerSet;
  ComputerTrackingPointer   TempComputerPntr;
  RULE_NetworkAddressRecord TestAddress;

  /* Make a list of all the players at the disconnected address. */

  PlayerSet = 0;
  for (PlayerNo = 0; PlayerNo <= RULE_BANK_PLAYER; PlayerNo++)
  {
    MESS_GetAddressOfPlayer (PlayerNo, &TestAddress);

    if (memcmp (&TestAddress, DisconnectedAddress,
    sizeof (RULE_NetworkAddressRecord)) == 0)
      PlayerSet |= (1 << PlayerNo);
  }

  /* Remove the computer from our list of connected computers. */

  for (i = 0; i < MAX_CONNECTED_COMPUTERS; i++)
  {
    TempComputerPntr = ComputerList + i;
    if (TempComputerPntr->computerID.networkSystem != NS_LOCAL &&
    memcmp (&TempComputerPntr->computerID, DisconnectedAddress,
    sizeof (RULE_NetworkAddressRecord)) == 0)
      memset (TempComputerPntr, 0, sizeof (ComputerTrackingRecord));
  }

  /* Disconnect all the players we had found. */

  for (PlayerNo = 0; PlayerNo <= RULE_BANK_PLAYER; PlayerNo++)
  {
    if (PlayerSet & (1 << PlayerNo))
    {
      /* That player doesn't exist any more.  Wipe out the address
         first to avoid recursive problems (MESS_StopAllNetworking
         calls this function). */

      memset (AddressesForPlayers + PlayerNo, 0, sizeof (RULE_NetworkAddressRecord));

      if (PlayerNo >= RULE_BANK_PLAYER)
      {
        /* If we lost the server, there isn't much you can do.  Shut
           down the network and go back to local operations (starts
           a new local game). */

        MESS_StopAllNetworking ();

        /* Let the local player know about it. */

        MESS_SendAction (NOTIFY_ERROR_MESSAGE, RULE_BANK_PLAYER, RULE_ALL_PLAYERS,
          TMN_ERROR_PLAYER_DISCONNECTED, 0, PlayerNo, 0,
          NULL, 0, NULL);
      }
      else /* A regular player. */
      {
        /* Lost one of the client players.  Tell the others about it and
           replace it with a local AI player. */

        MESS_SendAction (ACTION_DISCONNECTED_PLAYER, RULE_BANK_PLAYER, RULE_BANK_PLAYER,
          PlayerNo, 0, 0, 0, NULL, 0, NULL);
      }
    }
  }
}



/*****************************************************************************
 * When a new computer connects, make a note of it.  Messages won't be sent
 * or received from the new computer until the game is ready for them.
 * Returns TRUE if successful, FALSE if no free entries or something else
 * is wrong.
 */

static BOOL UpdateForNewlyConnectedComputer (RULE_NetworkAddressPointer NewComputerAddress)
{
  int                     FreeIndex;
  int                     i;
  ComputerTrackingPointer TempComputerPntr;

  if (NewComputerAddress == NULL ||
  NewComputerAddress->networkSystem == NS_LOCAL)
    return FALSE;

  /* Look for a free entry for the new computer.  Also make sure that any old
     matching entry is removed (in case someone exited the game and rejoined,
     or is confusingly running two copies of the game on the same computer). */

  FreeIndex = -1;
  for (i = 0; i < MAX_CONNECTED_COMPUTERS; i++)
  {
    TempComputerPntr = ComputerList + i;
    if (TempComputerPntr->computerID.networkSystem == NS_LOCAL)
      FreeIndex = i;
    else if (memcmp (&TempComputerPntr->computerID, NewComputerAddress,
    sizeof (RULE_NetworkAddressRecord)) == 0)
    {
      /* Found a duplicate address.  Presumably this is someone rejoining from
         the same computer as before, and we missed the disconnect.  Wipe it out.
         Well, this probably won't work well but it shouldn't happen often. */

      UpdateForDisconnectedConversation (NewComputerAddress);
      if (!MESS_NetworkMode)
        return FALSE;  /* Oops, we wiped out the host connection. */
      i = -1; /* Restart the loop, free spots will be different. */
    }
  }

  if (FreeIndex < 0)
    return FALSE; /* Unable to add any more computers. */

  TempComputerPntr = ComputerList + FreeIndex;
  memcpy (&TempComputerPntr->computerID, NewComputerAddress,
    sizeof (RULE_NetworkAddressRecord));
  TempComputerPntr->communicationsAllowed = FALSE;

  NewComputersJoinedRecently = TRUE;
  return TRUE;
}



/*****************************************************************************
 * Activates the connections to all the new computers (stops ignoring them
 * when sending and receiving).  Returns TRUE if there were any new computers.
 */

BOOL MESS_LetNewComputersIntoTheGame (void)
{
  int                     i;
  BOOL                    ReturnCode;
  ComputerTrackingPointer TempComputerPntr;

  ReturnCode = NewComputersJoinedRecently;

  if (NewComputersJoinedRecently)
  {
    for (i = 0; i < MAX_CONNECTED_COMPUTERS; i++)
    {
      TempComputerPntr = ComputerList + i;
      if (TempComputerPntr->computerID.networkSystem != NS_LOCAL &&
      !TempComputerPntr->communicationsAllowed)
        TempComputerPntr->communicationsAllowed = TRUE;
    }
    NewComputersJoinedRecently = FALSE;
  }
  return ReturnCode;
}



/*****************************************************************************
 * Turns off DirectPlay networking.  Killing the session should also kill
 * our player.
 */

static void TurnOffDirectPlay (void)
{
  int i;

  /* Spin through all known DirectPlay computers and mark them as disconnected.
     This may recursively call this function if the server player is
     disconnected, but that shouldn't cause any problems. */

  for (i = 0; i < MAX_CONNECTED_COMPUTERS; i++)
  {
    if (ComputerList[i].computerID.networkSystem == NS_DIRECTPLAY)
      UpdateForDisconnectedConversation (&ComputerList[i].computerID);
  }

  if (pDPlayN != NULL)
  {
    if (MyDPPlayerID != 0)
      pDPlayN->DestroyPlayer (MyDPPlayerID);
    MyDPPlayerID = 0;
    if (DPlaySessionIsOpen)
      pDPlayN->Close ();
    DPlaySessionIsOpen = FALSE;
    pDPlayN->Release ();
    pDPlayN = NULL;
  }

  if (pDPLobbyN != NULL)
    pDPLobbyN->Release ();
  pDPLobbyN = NULL;
  LobbySupportsStandardMessages = FALSE;
  LobbySupportsGameStatusUpdateMessages = TRUE;
  memset (&MyDPLPlayerGuid, 0, sizeof (MyDPLPlayerGuid));

  MESS_GameStartedByLobby = FALSE;
  MESS_DirectPlaySystemActive = FALSE;
}



/*****************************************************************************
 * Create a fresh IDirectPlayN interface, so that we can connect it with a
 * different service provider (there doesn't seem to be any easy way of
 * switching between service providers).
 */

static HRESULT CreateIDirectPlayN (void)
{
  HRESULT     hr;
  void       *VoidPntr;

#if _DEBUG
  if (pDPlayN != NULL)
    DBUG_DisplayNonFatalErrorMessage ("CreateIDirectPlayN: pDPlayN interface isn't null!");
#endif

  /* Try to create an IDirectPlayN interface using the COM system. */

  VoidPntr = NULL;
  hr = CoCreateInstance (CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER,
    IID_IDirectPlay4 /* Request Unicode version */, &VoidPntr);

  if (SUCCEEDED (hr) && VoidPntr != NULL)
  {
    // Type conversion here ensures version match with what we think we got
    // from CoCreateInstance above and the type of the global variable (so if
    // someone updates the global, it won't compile without updating here too).

    pDPlayN = (LPDIRECTPLAY4) VoidPntr;
  }

  return hr;
}



/*****************************************************************************
 * Adds the various service providers and connection shortcuts
 * to the list box in the service provider selection dialog.  The data
 * gets saved and can be used past the life of the DirectPlay object.
 * Don't add IPX services since they don't have reliable delivery.
 */

BOOL FAR PASCAL DirectPlayEnumServicesCallback (
  LPCGUID lpguidSP, LPVOID lpConnection, DWORD dwConnectionSize,
  LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
{
  HWND    hWnd = (HWND) lpContext;
  LRESULT iIndex;
  LPVOID  lpConnectionBuffer;
  char    TempString [MAX_DP_NAMES_LENGTH+1];

  if (wcsstr (lpName->lpszShortName, L"IPX") != NULL)
    return TRUE; // Skip IPX and continue enumerating.

  // store service provider name in list box
  wcstombs (TempString, lpName->lpszShortName, sizeof (TempString));
  iIndex = SendDlgItemMessage (hWnd, IDC_MESS_DP_SERVICES_LIST,
    LB_ADDSTRING, 0, (LPARAM) TempString);
  if (iIndex == LB_ERR)
    goto FAILURE_EXIT;

  // make space for connection shortcut
  lpConnectionBuffer = GlobalAllocPtr (GHND, dwConnectionSize);
  if (lpConnectionBuffer == NULL)
    goto FAILURE_EXIT;

  // store pointer to connection shortcut in list box
  memcpy (lpConnectionBuffer, lpConnection, dwConnectionSize);
  SendDlgItemMessage (hWnd, IDC_MESS_DP_SERVICES_LIST,
    LB_SETITEMDATA, (WPARAM) iIndex, (LPARAM) lpConnectionBuffer);

  return (TRUE);

FAILURE_EXIT:
  return FALSE; /* Stop the enumeration. */
}



/*****************************************************************************
 * Clean out the service providers from the list box, and free the
 * associated memory.  hWnd is the dialog box.
 */

static void DeallocateServicesInListBox (HWND hWnd)
{
  WPARAM  i;
  LONG  lpData;

  /* Deallocate the GUID's or connection records stored
     with each service provider or connection name. */

  i = 0;
  while (TRUE)
  {
    // get data pointer stored with item
    lpData = SendDlgItemMessage (hWnd, IDC_MESS_DP_SERVICES_LIST,
      LB_GETITEMDATA, (WPARAM) i, (LPARAM) 0);
    if (lpData == LB_ERR)   // error getting data
      break;

    if (lpData != 0)      // no data to delete
      GlobalFreePtr((LPVOID) lpData);

    i += 1;
  }

  // delete all items in combo box
  SendDlgItemMessage(hWnd, IDC_MESS_DP_SERVICES_LIST,
    LB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0);
}



/*****************************************************************************
 * Find the selected service provider / connection information.
 */

static HRESULT GetSelectedConnectionProvider (HWND hWnd, LPVOID *lplpConnection)
{
  LONG  iIndex;

  // get index of the item currently selected in the combobox
  iIndex = SendDlgItemMessage (hWnd, IDC_MESS_DP_SERVICES_LIST,
    LB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);
  if (iIndex == LB_ERR)
    return (DPERR_GENERIC);

  // get the pointer to the connection shortcut associated with the item
  iIndex = SendDlgItemMessage (hWnd, IDC_MESS_DP_SERVICES_LIST,
    LB_GETITEMDATA, (WPARAM) iIndex, (LPARAM) 0);
  if (iIndex == LB_ERR)
    return (DPERR_GENERIC);

  *lplpConnection = (LPVOID) iIndex;

  return (DP_OK);
}



/*****************************************************************************
 * Run the modal dialog box that asks the user for a service provider or
 * predefined connection.  EndDialog returns FALSE if the user cancelled or it
 * failed.  Otherwise returns TRUE and sets up a DirectPlay object on the
 * service provider.  Modified version of the DirectPlay Chat example, works
 * with DirectX 6.  Excludes IPX since that is unreliable (no retry for errors).
 */

BOOL CALLBACK DPSelectServiceDialogProc (HWND hWnd,
UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  WORD          ControlID;
  HRESULT       hr;
  WORD          NotificationCode;
  LPVOID        ProviderInfoPntr;
  char          TempString [MAX_DP_NAMES_LENGTH+1];

  switch (uMsg)
  {
  case WM_INITDIALOG:
    /* Put the international text into the dialog box title and the OK button. */
    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BUTTON_OK), sizeof (TempString));
    SetDlgItemText (hWnd, IDOK, TempString);

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_DP_SERVICE_TITLE), sizeof (TempString));
    SetWindowText (hWnd, TempString);

    // put all the available connections (service providers and shortcuts) in the combo box

    if (FAILED (CreateIDirectPlayN ()))
      goto FAILURE_EXIT;

    /* Add all service providers and preset connections to the list box.
       Stores a connection record for each item. */

    pDPlayN->EnumConnections (&Monopoly1999aGUID,
      DirectPlayEnumServicesCallback, hWnd, DPCONNECTION_DIRECTPLAY);
    break;


  case WM_DESTROY:
    // delete information stored along with the lists
    DeallocateServicesInListBox (hWnd);
    break;


  case WM_COMMAND:
    ControlID = GET_WM_COMMAND_ID (wParam, lParam);
    NotificationCode = GET_WM_COMMAND_CMD (wParam, lParam);

    if (ControlID == IDCANCEL)
    {
      /* Cancel and the close box. */
      goto FAILURE_EXIT;
    }
    else if (ControlID == IDOK ||
    (ControlID == IDC_MESS_DP_SERVICES_LIST && NotificationCode == LBN_DBLCLK))
    {
      /* The user has selected an item from the box.  Open up DirectPlay
         using that service provider. */

      // get pointer to the selected connection / service provider info
      hr = GetSelectedConnectionProvider (hWnd, &ProviderInfoPntr);
      if (FAILED (hr))
        break; /* Nothing was selected. */

      // initialize the connection (picks service provider)
      hr = pDPlayN->InitializeConnection (ProviderInfoPntr, 0);
      if (FAILED (hr))
      {
        DBUG_DisplayInternationalMessage (T_MESS_ERROR_DPM_CANT_CONNECT, hr, 0, 0, NULL);
        goto FAILURE_EXIT;
      }

      /* We now have a DirectPlay connection!  But no session yet. */
      EndDialog (hWnd, TRUE);
    }
    break;
  }

  // Allow for default processing
  return FALSE;

FAILURE_EXIT:
  TurnOffDirectPlay ();
  EndDialog (hWnd, FALSE);
  return FALSE;
}



/*****************************************************************************
 * Go through all the players in the current session and find the one which
 * is the host of the session (the server).  Look for ones with names
 * starting with "H".
 */

BOOL FAR PASCAL EnumPlayersToFindServer (DPID dpId, DWORD dwPlayerType,
LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
{
  NetAddressForDirectPlayPointer  DPStyleNetAddressPntr;
  RULE_NetworkAddressRecord       TempNetAddress;

  memset (&TempNetAddress, 0, sizeof (TempNetAddress));
  DPStyleNetAddressPntr = (NetAddressForDirectPlayStruct *) &TempNetAddress;
  DPStyleNetAddressPntr->networkSystem = NS_DIRECTPLAY;
  DPStyleNetAddressPntr->playerID = dpId;

  if ((lpName != NULL && lpName->lpszShortName != NULL &&
  lpName->lpszShortName[0] == L'H' && lpName->lpszShortName[1] == L':') ||
  (pDPlayN != NULL && (dwFlags & DPENUMPLAYERS_SERVERPLAYER)))
  {
    AddressesForPlayers [RULE_BANK_PLAYER] = TempNetAddress;
    UpdateForNewlyConnectedComputer (&TempNetAddress);
    MESS_LetNewComputersIntoTheGame (); /* Server is always in the game. */
    return FALSE; /* That's all we need to know. */
  }
  return TRUE; /* Continue on. */
}



/*****************************************************************************
 * Create the player that represents this computer.  It's name will be
 * prepended with "H:" if it is the host (server), or "C:" if it is the
 * client.  Also find the server player if this is a client starting up.
 * Returns TRUE if successful, returns FALSE and closes DirectPlay on
 * failure.  Can display error messages.
 */

static BOOL CreatePlayerAndFindServerPlayer (wchar_t *PlayerName)
{
  DPNAME                  DPName;
  HRESULT                 ErrorCode;
  LANG_TextMessageNumber  ErrorMessageNo = ATR_NO_MESSAGE;
  wchar_t                 FixedUpName [MAX_DP_NAMES_LENGTH+1];

  /* Save the player name for use by the game, if we don't already have one. */

  if (MESS_PlayerNameFromNetwork [0] == 0)
  {
    wcsncpy (MESS_PlayerNameFromNetwork, PlayerName, RULE_MAX_PLAYER_NAME_LENGTH);
    MESS_PlayerNameFromNetwork [RULE_MAX_PLAYER_NAME_LENGTH] = 0;
  }

  /* Make the prepended name. */

  FixedUpName [0] = MESS_ServerMode ? L'H' : L'C';
  FixedUpName [1] = L':';
  wcsncpy (FixedUpName + 2, PlayerName, MAX_DP_NAMES_LENGTH - 2);
  FixedUpName [MAX_DP_NAMES_LENGTH] = 0;

  /* Fill out the DirectPlay name structure. */

  memset (&DPName, 0, sizeof (DPName));
  DPName.dwSize = sizeof (DPName);
  DPName.lpszShortName = FixedUpName;
  DPName.lpszLongName = FixedUpName;

  /* Create a player with this name. */

  if (pDPlayN != NULL)
    ErrorCode = pDPlayN->CreatePlayer (&MyDPPlayerID,
      &DPName /* New name */,
      NULL /* No event to trigger for received messages */,
      NULL /* No associated player data */,
      0 /* Size of associated player data */,
      MESS_ServerMode ? DPPLAYER_SERVERPLAYER : 0);
  else
    ErrorCode = DPERR_GENERIC;

  if (FAILED (ErrorCode))
  {
    ErrorMessageNo = T_MESS_ERROR_DPM_PLAYER_CREATE;
    goto FAILURE_EXIT;
  }

  /* If we are running in client mode, find the address of the server (it's
     the player whose name starts with 'H' for host). */

  if (!MESS_ServerMode)
  {
    if (pDPlayN != NULL)
      ErrorCode = pDPlayN->EnumPlayers (NULL,
      EnumPlayersToFindServer, NULL, 0);
    else
      ErrorCode = DPERR_GENERIC;

    if (FAILED (ErrorCode) ||
    AddressesForPlayers [RULE_BANK_PLAYER].networkSystem != NS_DIRECTPLAY)
    {
      ErrorMessageNo = T_MESS_ERROR_DPM_NO_SERVER_PLAYER;
      goto FAILURE_EXIT;
    }
  }

  return TRUE;


FAILURE_EXIT: /* Display an optional error message, close stuff, return false. */
  if (ErrorMessageNo != ATR_NO_MESSAGE)
    DBUG_DisplayInternationalMessage (ErrorMessageNo, ErrorCode, 0, 0, NULL);
  TurnOffDirectPlay ();
  return FALSE;
}



/*****************************************************************************
 * Start up direct play, hosting a new session or joining an old one.  Uses
 * the current pDPlayN which has a service provider selected.
 */

static BOOL HostOrJoinSession (LPGUID SessionGUID)
{
  DWORD                           dwNameSize;
  LANG_TextMessageNumber          ErrorMessageNo = ATR_NO_MESSAGE;
  HRESULT                         hr;
  DPSESSIONDESC2                  sessionDesc;
  wchar_t                         szSessionName [MAX_DP_NAMES_LENGTH+1];
  wchar_t                         szPlayerName [MAX_DP_NAMES_LENGTH+1];
  char                            TempString [MAX_DP_NAMES_LENGTH+1];

  if (pDPlayN == NULL)
    return FALSE;

  // use computer name for session name
  dwNameSize = MAX_DP_NAMES_LENGTH;
  if (GetComputerName (TempString, &dwNameSize))
    mbstowcs (szSessionName, TempString, MAX_DP_NAMES_LENGTH);
  else
    wcscpy (szSessionName, LANG_GetTextMessageClean (T_MESS_DP_SESSION_NAME_UNKNOWN));

  /* Get current user name for player name. */

  dwNameSize = MAX_DP_NAMES_LENGTH;
  if (GetUserName (TempString, &dwNameSize))
    mbstowcs (szPlayerName, TempString, MAX_DP_NAMES_LENGTH);
  else
    wcscpy (szPlayerName, LANG_GetTextMessageClean (T_MESS_DP_USER_NAME_UNKNOWN));

  // Set up the session description record.
  memset (&sessionDesc, 0, sizeof (DPSESSIONDESC2));
  sessionDesc.dwSize = sizeof (DPSESSIONDESC2);
  if (MESS_ServerMode)
  {
    sessionDesc.dwFlags =
      DPSESSION_KEEPALIVE | /* We like to know when other players get disconnected */
      DPSESSION_CLIENTSERVER; /* This game runs in client server mode, clients only talk to server. */
    sessionDesc.dwMaxPlayers = MAX_DIRECTPLAY_PLAYERS; /* Includes some spectators. */
    sessionDesc.lpszSessionName = szSessionName;
    sessionDesc.guidApplication = Monopoly1999aGUID;
  }
  else /* Client mode - join existing. */
  {
    if (SessionGUID == NULL)
    {
#if _DEBUG
      DBUG_DisplayNonFatalErrorMessage ("HostOrJoinSession: NULL session GUID!");
#endif
      goto FAILURE_EXIT;
    }
    sessionDesc.guidInstance = *SessionGUID;
  }

  /* Open the session. */

  hr = pDPlayN->Open (&sessionDesc,
    MESS_ServerMode ? DPOPEN_CREATE : DPOPEN_JOIN);

  if (FAILED (hr))
  {
    ErrorMessageNo = T_MESS_ERROR_DPM_CANT_OPEN;
    goto FAILURE_EXIT;
  }
  DPlaySessionIsOpen = TRUE;

  /* Now name that player! */

  if (!CreatePlayerAndFindServerPlayer (szPlayerName))
    goto FAILURE_EXIT;

  return TRUE;


FAILURE_EXIT: /* Display an optional error message, close stuff, return false. */
  if (ErrorMessageNo != ATR_NO_MESSAGE)
    DBUG_DisplayInternationalMessage (ErrorMessageNo, hr, 0, 0, NULL);
  TurnOffDirectPlay ();
  return FALSE;
}



/*****************************************************************************
 * Callback function to enumerate the sessions available from the currently
 * selected DirectPlay service provider.  Allocates memory and adds them
 * to the list box.
 */

BOOL FAR PASCAL EnumSessionsCallback (
  LPCDPSESSIONDESC2 lpSessionDesc, LPDWORD lpdwTimeOut,
  DWORD dwFlags, LPVOID lpContext)
{
  HWND    hWnd = (HWND) lpContext;
  LPGUID  lpGuid;
  LONG    iIndex;
  char    TempString [MAX_DP_NAMES_LENGTH+1];

  // see if last session has been enumerated
  if ((dwFlags & DPESC_TIMEDOUT) || lpSessionDesc == NULL)
    return (FALSE);  /* Stop the enumeration. */

  // store session name in list
  wcstombs (TempString, lpSessionDesc->lpszSessionName, sizeof (TempString));
  iIndex = SendDlgItemMessage (hWnd, IDC_MESS_DP_SESSIONS_LIST,
    LB_ADDSTRING, (WPARAM) 0, (LPARAM) TempString);

  if (iIndex == LB_ERR)
    return FALSE;

  // make space for session instance guid
  lpGuid = (LPGUID) GlobalAllocPtr (GHND, sizeof (GUID));
  if (lpGuid == NULL)
    return FALSE;

  // store pointer to guid in list
  *lpGuid = lpSessionDesc->guidInstance;
  SendDlgItemMessage (hWnd, IDC_MESS_DP_SESSIONS_LIST,
    LB_SETITEMDATA, (WPARAM) iIndex, (LPARAM) lpGuid);

  return (TRUE); /* Continue enumerating. */
}



/*****************************************************************************
 * Deallocate memory and clean out the list of sessions in the list box.
 */

static void DeallocateSessionsInListBox (HWND hWnd)
{
  WPARAM  i;
  LONG  lpData;

  // destroy the GUID's stored with each session name
  i = 0;
  while (TRUE)
  {
    // get data pointer stored with item
    lpData = SendDlgItemMessage (hWnd, IDC_MESS_DP_SESSIONS_LIST,
      LB_GETITEMDATA, (WPARAM) i, (LPARAM) 0);
    if (lpData == LB_ERR)   // error getting data
      break;

    if (lpData == 0)      // no data to delete
      continue;

    GlobalFreePtr ((LPVOID) lpData);
    i += 1;
  }

  // delete all items in list
  SendDlgItemMessage (hWnd, IDC_MESS_DP_SESSIONS_LIST,
    LB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0);
}



/*****************************************************************************
 * Utility function to find the current session in the list box.  Copies
 * the whole GUID into your waiting GUID record.  Returns DP_OK if successful.
 */

static HRESULT GetSelectedSessionGUID (HWND hWnd, LPGUID lpguidSessionInstance)
{
  LONG  iIndex;

  // get guid for session
  iIndex = SendDlgItemMessage (hWnd, IDC_MESS_DP_SESSIONS_LIST,
    LB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);
  if (iIndex == LB_ERR)
    return (DPERR_GENERIC);

  iIndex = SendDlgItemMessage (hWnd, IDC_MESS_DP_SESSIONS_LIST,
    LB_GETITEMDATA, (WPARAM) iIndex, (LPARAM) 0);
  if (iIndex == LB_ERR || iIndex == 0 /* NULL */)
    return (DPERR_GENERIC);

  *lpguidSessionInstance = *((LPGUID) iIndex); /* Copy whole record. */
  return DP_OK;
}



/*****************************************************************************
 * Select the session in the list box that matches the GUID, if any.
 */

static void SelectSessionInstance (HWND hWnd, LPGUID lpguidSessionInstance)
{
  WPARAM  i;
  WPARAM  iIndex;
  LONG    lpData;

  // loop over the GUID's stored with each session name
  // to find the one that matches what was passed in
  i = 0;
  iIndex = 0;
  while (TRUE)
  {
    // get data pointer stored with item
    lpData = SendDlgItemMessage (hWnd, IDC_MESS_DP_SESSIONS_LIST,
      LB_GETITEMDATA, (WPARAM) i, (LPARAM) 0);
    if (lpData == LB_ERR)   // error getting data
      break;
    if (lpData == 0)      // no data to compare to
      continue;

    // guid matches
    if (memcmp (lpguidSessionInstance, (void *) lpData, sizeof (GUID)) == 0)
    {
      iIndex = i;       // store index of this string
      break;
    }
    i += 1;
  }

  // select this item
  SendDlgItemMessage (hWnd, IDC_MESS_DP_SESSIONS_LIST,
    LB_SETCURSEL, (WPARAM) iIndex, (LPARAM) 0);
}



/*****************************************************************************
 * Refresh the list of sessions in the dialog box.  Reselect the previously
 * selected one if it reappears.  If AsyncMode is TRUE then it will just
 * ask DirectPlay to get the ones it currently has in its cache, if FALSE
 * then it will wait a while for DirectPlay to dig up the sessions.
 */

static BOOL EnumSessionsRecursiveCallDetected;

static HRESULT EnumDirectPlaySessions (HWND hWnd, BOOL AsyncMode)
{
  DPSESSIONDESC2  sessionDesc2;
  GUID            guidSessionInstance;
  LONG            iIndex;
  HRESULT         hr;

  if (EnumSessionsRecursiveCallDetected)
      return DPERR_GENERIC;

  EnumSessionsRecursiveCallDetected = TRUE;

  // get guid of currently selected session, if any.
  guidSessionInstance = GUID_NULL;
  hr = GetSelectedSessionGUID (hWnd, &guidSessionInstance);

  // delete existing session list
  DeallocateSessionsInListBox (hWnd);

  // add sessions to session list, if any.
  memset (&sessionDesc2, 0, sizeof (DPSESSIONDESC2));
  sessionDesc2.dwSize = sizeof (DPSESSIONDESC2);
  sessionDesc2.guidApplication = Monopoly1999aGUID;

  if (pDPlayN != NULL)
  {
    hr = pDPlayN->EnumSessions (&sessionDesc2,
      0 /* Use default timeout */,
      EnumSessionsCallback,
      hWnd /* Our own parameter */,
      DPENUMSESSIONS_AVAILABLE /* Sessions accepting players  */ |
      (AsyncMode ? DPENUMSESSIONS_ASYNC : 0) /* Collect sessions running in the background */);
  }
  else
    hr = DPERR_GENERIC;

  // select the session that was previously selected, if it is there.
  SelectSessionInstance (hWnd, &guidSessionInstance);

  /* Activate the OK button only if there are sessions to join. */
  iIndex = SendDlgItemMessage (hWnd, IDC_MESS_DP_SESSIONS_LIST,
    LB_GETCOUNT, (WPARAM) 0, (LPARAM) 0);

  EnableDialogControl (hWnd, IDOK, iIndex > 0);

  EnumSessionsRecursiveCallDetected = FALSE;
  return (hr);
}



/*****************************************************************************
 * Run the modal dialog box that asks the user for a session to join.
 * EndDialog returns FALSE if the user cancelled or it failed.  Otherwise
 * opens the session and returns TRUE.  Modified version of the DirectPlay
 * Chat example that works with DirectX 3 and DirectX 5.
 */

BOOL CALLBACK DPSelectSessionDialogProc (HWND hWnd,
UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  WORD        ControlID;
  static UINT idTimer;
  LPVOID      lpConnection = NULL;
  WORD        NotificationCode;
  GUID        SessionGUID;
  BOOL        Success;
  char        TempString [MAX_DP_NAMES_LENGTH+1];

  switch (uMsg)
  {
  case WM_INITDIALOG:
    DPSessionBoxTimerEnabled = FALSE;

    /* Put the international text into the dialog box title and the OK button. */

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BUTTON_OK), sizeof (TempString));
    SetDlgItemText (hWnd, IDOK, TempString);

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_DP_SESSION_TITLE), sizeof (TempString));
    SetWindowText (hWnd, TempString);

    // put all the available sessions in the list box.
    EnumDirectPlaySessions (hWnd, FALSE /* Synchronous */);

    /* Activate a timer to update the session list, since
       DirectX 5 and later support asynchronous session list building. */

    SetTimer (hWnd, DPM_TIMER_ID, DPM_TIMER_INTERVAL, NULL);

    // Flag for our code to pay attention to timer messages.  Not the
    // existance of the timer.  Gets set to FALSE when we have had
    // enough, even though the timer still keeps ticking.
    DPSessionBoxTimerEnabled = TRUE;
    break;


  case WM_DESTROY:
    KillTimer (hWnd, DPM_TIMER_ID);
    // delete information stored along with the lists
    DeallocateSessionsInListBox (hWnd);
    break;


  case WM_TIMER:
    // refresh the session list from the DirectPlay async cache.
    if (DPSessionBoxTimerEnabled)
      EnumDirectPlaySessions (hWnd, TRUE);
    break;


  case WM_COMMAND:
    ControlID = GET_WM_COMMAND_ID (wParam, lParam);
    NotificationCode = GET_WM_COMMAND_CMD (wParam, lParam);

    switch (ControlID)
    {
    case IDCANCEL: /* Includes the close box */
      DPSessionBoxTimerEnabled = FALSE;
      TurnOffDirectPlay ();
      EndDialog (hWnd, FALSE);
      break;


    case IDC_MESS_DP_SESSIONS_LIST:
      if (NotificationCode != LBN_DBLCLK)
        break;
      /* Else fall through to IDOK. */
    case IDOK:
      DPSessionBoxTimerEnabled = FALSE;
      Success = FALSE;
      if (DP_OK == GetSelectedSessionGUID (hWnd, &SessionGUID))
        Success = HostOrJoinSession (&SessionGUID);
      EndDialog (hWnd, Success);
      break;
    }
  }

  // Allow for default processing
  return FALSE;
}



/*****************************************************************************
 * Starts up DirectPlay.  If address and other information (like service
 * provider and address) can be found on the command line, we will use it.
 * Otherwise ask the user.
 */

static void MESS_DirectPlayManualStartup (wchar_t *CommandLine)
{
  BOOL  Success;

  if (wcsstr (CommandLine, L"-directplay") == NULL)
    return; /* DirectPlay not requested. */

  /* Some bug detection checks. */

#if _DEBUG
  if (pDPlayN != NULL)
    DBUG_DisplayNonFatalErrorMessage ("Starting manual networking but pDPlayN interface isn't null!");
  if (pDPLobbyN != NULL)
    DBUG_DisplayNonFatalErrorMessage ("Starting manual networking but pDPLobbyN interface isn't null!");
  if (MyDPPlayerID != 0)
    DBUG_DisplayNonFatalErrorMessage ("Starting manual networking but MyDPPlayerID isn't zero!");
#endif

  /* Pick the service provider (TCP / Modem / IPX and others). */

  Success = DialogBox (ApplicationInstance,
    MAKEINTRESOURCE (IDD_MESS_DP_SERVICE_PICK),
    MainWindowHandle, DPSelectServiceDialogProc);

  /* Open a DirectPlay session. */

  if (Success)
  {
    if (MESS_ServerMode)
    {
      /* Server mode.  Create a new session. */
      Success = HostOrJoinSession (NULL);
    }
    else /* Client, pick a session to join. */
    {
      Success = DialogBox (ApplicationInstance,
        MAKEINTRESOURCE (IDD_MESS_DP_SESSION_PICK),
        MainWindowHandle, DPSelectSessionDialogProc);
    }
  }

  if (!Success)
    TurnOffDirectPlay ();

  MESS_DirectPlaySystemActive = (pDPlayN != NULL);
}



/*****************************************************************************
 * Try to open a DirectPlay lobby initiated session.  Return FALSE if
 * something went wrong or if we weren't started by a lobby.
 */

static BOOL MESS_LobbyStartup (void)
{
  DWORD                           dwSize;
  LANG_TextMessageNumber          ErrorMessageNo = ATR_NO_MESSAGE;
  HRESULT                         ErrorCode;
  DPLMSG_GETPROPERTY              LobbyMessageGetProp;
  LPDPLCONNECTION                 pConnectionSettings = NULL;
  wchar_t                         PlayerName [MAX_DP_NAMES_LENGTH+1];
  BOOL                            ReturnCode = TRUE;
  void                           *VoidPntr;

  /* Some bug detection checks. */

  if (pDPlayN != NULL)
    return FALSE; /* DirectPlay already running. */

#if _DEBUG
  if (pDPLobbyN != NULL)
    DBUG_DisplayNonFatalErrorMessage ("Starting lobby networking but pDPLobbyN interface isn't null!");
  if (MyDPPlayerID != 0)
    DBUG_DisplayNonFatalErrorMessage ("Starting lobby networking but MyDPPlayerID isn't zero!");
#endif

  /* Create the lobby using the UNICODE interface, even though this program
     is compiled in ANSI mode, since we are doing international versions.
     Note the use of VoidPntr so that type checking gets done so that we
     can be sure that pDPLobbyN is the same type as we expect to get. */

  VoidPntr = NULL;
  ErrorCode = CoCreateInstance (CLSID_DirectPlayLobby, NULL,
    CLSCTX_INPROC_SERVER, IID_IDirectPlayLobby3, &VoidPntr);
  if (FAILED (ErrorCode))
    goto FAILURE_EXIT;
  pDPLobbyN = (LPDIRECTPLAYLOBBY3) VoidPntr;

  /* Find out the size of the connection settings structure and
     allocate space for it. */

  dwSize = 0;
  ErrorCode = pDPLobbyN->GetConnectionSettings (0, NULL, &dwSize);
  if (ErrorCode != DPERR_BUFFERTOOSMALL)
    goto FAILURE_EXIT;  /* Not started by a lobby or other error, exit quietly. */

  pConnectionSettings = (DPLCONNECTION *) GlobalAllocPtr (GHND, dwSize);
  if (pConnectionSettings == NULL)
    goto FAILURE_EXIT;

  /* Try to get the connection settings. */

  ErrorCode = pDPLobbyN->GetConnectionSettings (0, pConnectionSettings, &dwSize);
  if (FAILED (ErrorCode))
    goto FAILURE_EXIT;

  /* Warn people about the wrong GUID.  Lobby has started the wrong program. */

  if (memcmp (&pConnectionSettings->lpSessionDesc->guidApplication,
  &Monopoly1999aGUID, sizeof (Monopoly1999aGUID)) != 0)
  {
    ErrorMessageNo = T_MESS_ERROR_LOBBY_APP_GUID;
    goto FAILURE_EXIT;
  }

  /* See if we are the server or if we are a client. */

  if (pConnectionSettings->dwFlags & DPLCONNECTION_CREATESESSION)
    MESS_ServerMode = TRUE;
  else
    MESS_ServerMode = FALSE;

  /* Change the connection settings to the ones we need. */

  /* We like to know when other players get disconnected */
  pConnectionSettings->lpSessionDesc->dwFlags |= DPSESSION_KEEPALIVE;

  /* This game runs in client server mode, clients only talk to server. */
  pConnectionSettings->lpSessionDesc->dwFlags |= DPSESSION_CLIENTSERVER;

  /* We want to know who sent a message, for authentication. */
  pConnectionSettings->lpSessionDesc->dwFlags &= ~DPSESSION_NOMESSAGEID;

  /* Allow other players to join in later. */
  pConnectionSettings->lpSessionDesc->dwFlags &= ~DPSESSION_JOINDISABLED;

  /* There is only one server, don't allow it to migrate. */
  pConnectionSettings->lpSessionDesc->dwFlags &= ~DPSESSION_MIGRATEHOST;

  /* Allow creation of new players. */
  pConnectionSettings->lpSessionDesc->dwFlags &= ~DPSESSION_NEWPLAYERSDISABLED;

  /* Allow for at least as many players as we can handle. */
  pConnectionSettings->lpSessionDesc->dwMaxPlayers = MAX_DIRECTPLAY_PLAYERS;

  /* Grab the suggested name out of the lobby settings. */

  if (pConnectionSettings->lpPlayerName->lpszShortName != NULL)
    wcsncpy (PlayerName, pConnectionSettings->lpPlayerName->lpszShortName,
    MAX_DP_NAMES_LENGTH);
  else
    PlayerName [0] = 0;

  /* Save the new connection settings into the lobby. */

  ErrorCode = pDPLobbyN->SetConnectionSettings (
    0, 0, pConnectionSettings);
  if (FAILED (ErrorCode))
    goto FAILURE_EXIT;

  /* Now connect to or create the DirectPlay session.  Generates a
     the latest DirectPlay COM interface we know about. */

  VoidPntr = NULL;
  ErrorCode = pDPLobbyN->ConnectEx (
    0, // Zero flags - return after connected.
    IID_IDirectPlay4, // Want the Unicode version (so 4, not 4A).
    &VoidPntr, NULL);
  if (ErrorCode == DPERR_NOTLOBBIED)
    goto FAILURE_EXIT;  /* Not started by a lobby, exit quietly. */
  if (FAILED (ErrorCode))
  {
    ErrorMessageNo = T_MESS_ERROR_LOBBY_CONNECT_FAILED;
    goto FAILURE_EXIT;
  }
  pDPlayN = (LPDIRECTPLAY4) VoidPntr; // Make sure global matches type we think we got.
  DPlaySessionIsOpen = TRUE;

  /* Create our player, with the name the user provided. */

  if (!CreatePlayerAndFindServerPlayer (PlayerName))
    goto FAILURE_EXIT;

  /* Ask the lobby if it supports lobby messages.  The reply message will come
     back later, and set the LobbySupportsStandardMessages flag. */

  memset (&LobbyMessageGetProp, 0, sizeof (LobbyMessageGetProp));
  LobbyMessageGetProp.dwType = DPLSYS_GETPROPERTY;
  LobbyMessageGetProp.dwRequestID = 1;
  LobbyMessageGetProp.guidPlayer = GUID_NULL;
  LobbyMessageGetProp.guidPropertyTag = DPLPROPERTY_MessagesSupported;

  ErrorCode = pDPLobbyN->SendLobbyMessage (
    DPLMSG_STANDARD, 0, &LobbyMessageGetProp, sizeof (LobbyMessageGetProp));

  /* Don't care if the lobby message send fails, we just treat it the same as
     there being no lobby or a lobby which can't communicate. */

  /* Succeeded! */

  MESS_GameStartedByLobby = TRUE;
  goto NORMAL_EXIT;


FAILURE_EXIT: /* Display an optional error message, close lobbies, return false. */
  ReturnCode = FALSE;

  if (ErrorMessageNo != ATR_NO_MESSAGE)
    DBUG_DisplayInternationalMessage (ErrorMessageNo, ErrorCode, 0, 0, NULL);

  TurnOffDirectPlay ();

NORMAL_EXIT: /* Deallocate temporary allocations. */

  if (pConnectionSettings != NULL)
    GlobalFreePtr (pConnectionSettings);

  MESS_DirectPlaySystemActive = (pDPlayN != NULL);

  return ReturnCode;
}



/*****************************************************************************
 * This function is called periodically to process incoming DirectPlay
 * messages (it unloads them from the network buffers and sticks them in our
 * queue) and other DirectPlay events (like new connections and disconnections).
 */

static void CheckForDirectPlayReceivedData (void)
{
  typedef union MessageBufferUnion
  {
    char                            asString [1];
    DPMSG_CHAT                      chatDPMessage;
    DPMSG_CREATEPLAYERORGROUP       createDPMessage;
    DPMSG_DESTROYPLAYERORGROUP      destroyDPMessage;
    DPMSG_GENERIC                   genericDPMessage;
    MonopolyNetMessageHeaderRecord  monopolyMessage;
  } MessageBufferRecord, *MessageBufferPointer;

  BOOL                           *BoolPntr;
  DWORD                           ChatLength;
  RULE_CharHandle                 ChatMemoryHandle;
  wchar_t                        *ChatMemoryPntr;
  HRESULT                         ErrorCode;
  DPID                            FromID;
  LPDPLDATA_PLAYERGUID            LobbyGuidOfPlayerPntr;
  DWORD                           LobbyMessageFlags;
  DPLMSG_GETPROPERTY              LobbyMessageGetProp;
  MessageBufferRecord             MessageBufferOnStack;
  MessageBufferPointer            MessageBufferPntr;
  long                            MessageLength;
  LPDPLMSG_GETPROPERTYRESPONSE    PropertyResponseMsgPntr;
  LPDPLMSG_SETPROPERTYRESPONSE    SetResponseMsgPntr;
  NetAddressForDirectPlayRecord   TempNetAddr;
  DPID                            ToID;

  /************************************************************/
  /* While there are lobby messages to process, process them. */
  /************************************************************/

  while (TRUE)
  {
    /* See if the lobby is still open. */

    if (pDPLobbyN == NULL)
      break; /* Lobby not in use. */

    MessageBufferPntr = NULL;
    MessageLength = 0;
    ErrorCode = DPERR_BUFFERTOOSMALL;

    /* Repeat until we have enough memory to hold the message,
       or something goes wrong. */

    while (ErrorCode == DPERR_BUFFERTOOSMALL)
    {
      LobbyMessageFlags = 0;
      ErrorCode = pDPLobbyN->ReceiveLobbyMessage (0, 0, &LobbyMessageFlags,
        MessageBufferPntr, (unsigned long *) &MessageLength);

      if (ErrorCode == DPERR_BUFFERTOOSMALL ||
      (ErrorCode == DP_OK && MessageBufferPntr == NULL))
      {
        if (MessageBufferPntr != NULL)
          GlobalFreePtr (MessageBufferPntr);
        MessageBufferPntr = (MessageBufferPointer) GlobalAllocPtr (GHND, MessageLength); /* Allocates and locks */
        if (MessageBufferPntr == NULL)
          ErrorCode = DPERR_OUTOFMEMORY;
        else /* Try again with this larger buffer. */
          ErrorCode = DPERR_BUFFERTOOSMALL;
      }
    }

    if (ErrorCode != DP_OK)
    {
      /* Stop processing when we hit a normal error like DPERR_NOMESSAGES
         or any other error too. */

      if (MessageBufferPntr != NULL)
        GlobalFreePtr (MessageBufferPntr);
      break;
    }

    if (LobbyMessageFlags == DPLMSG_STANDARD)
    {
      PropertyResponseMsgPntr = (LPDPLMSG_GETPROPERTYRESPONSE) MessageBufferPntr;

      if (PropertyResponseMsgPntr->dwType == DPLSYS_GETPROPERTYRESPONSE)
      {
        if (IsEqualGUID (PropertyResponseMsgPntr->guidPropertyTag,
        DPLPROPERTY_MessagesSupported))
        {
          if (PropertyResponseMsgPntr->hr == DP_OK &&
          PropertyResponseMsgPntr->dwDataSize >= sizeof (BOOL))
          {
            BoolPntr = (BOOL *) &PropertyResponseMsgPntr->dwPropertyData;
            if (*BoolPntr)
            {
              LobbySupportsStandardMessages = TRUE;

              /* Fire off a message asking for the GUID of our representing player. */

              memset (&LobbyMessageGetProp, 0, sizeof (LobbyMessageGetProp));
              LobbyMessageGetProp.dwType = DPLSYS_GETPROPERTY;
              LobbyMessageGetProp.dwRequestID = 2;
              LobbyMessageGetProp.guidPlayer = GUID_NULL;
              LobbyMessageGetProp.guidPropertyTag = DPLPROPERTY_PlayerGuid;

              ErrorCode = pDPLobbyN->SendLobbyMessage (
                DPLMSG_STANDARD, 0, &LobbyMessageGetProp, sizeof (LobbyMessageGetProp));
            }
          }
        }
        else if (IsEqualGUID (PropertyResponseMsgPntr->guidPropertyTag,
        DPLPROPERTY_PlayerGuid))
        {
          if (PropertyResponseMsgPntr->hr == DP_OK &&
          PropertyResponseMsgPntr->dwDataSize >= sizeof (DPLDATA_PLAYERGUID))
          {
            LobbyGuidOfPlayerPntr = (LPDPLDATA_PLAYERGUID) &PropertyResponseMsgPntr->dwPropertyData;
            MyDPLPlayerGuid = LobbyGuidOfPlayerPntr->guidPlayer;
          }
        }
      }
      else if (PropertyResponseMsgPntr->dwType == DPLSYS_SETPROPERTYRESPONSE)
      {
        SetResponseMsgPntr = (LPDPLMSG_SETPROPERTYRESPONSE) MessageBufferPntr;

        if (IsEqualGUID (SetResponseMsgPntr->guidPropertyTag,
        ZoneGameStatusUpdateGUID))
        {
          if (SetResponseMsgPntr->hr != DP_OK)
            LobbySupportsGameStatusUpdateMessages = FALSE;
        }
      }
    }

    /* Finished processing that message, deallocate temporary allocations. */

    if (MessageBufferPntr != NULL)
      GlobalFreePtr (MessageBufferPntr);
  }

  /*****************************************************************/
  /* While there are DirectPlay messages to process, process them. */
  /*****************************************************************/

  while (TRUE)
  {
    /* See if DirectPlay is still open, can be closed by
       UpdateForDisconnectedConversation when the server connection is dropped. */

    if (pDPlayN == NULL || MyDPPlayerID == 0)
      return; /* DirectPlay not in use or not fully open. */

    MessageBufferPntr = &MessageBufferOnStack;
    MessageLength = sizeof (MessageBufferOnStack);
    ErrorCode = DPERR_BUFFERTOOSMALL;

    /* Repeat until we have enough memory to hold the message,
       or something goes wrong. */

    while (ErrorCode == DPERR_BUFFERTOOSMALL)
    {
      FromID = 0;
      ToID = 0;

      ErrorCode = pDPlayN->Receive (&FromID, &ToID,
        (MessageBufferPntr == NULL) ? (DPRECEIVE_ALL | DPRECEIVE_PEEK) : DPRECEIVE_ALL,
        MessageBufferPntr, (unsigned long *) &MessageLength);

      if (ErrorCode == DPERR_BUFFERTOOSMALL ||
      (ErrorCode == DP_OK && MessageBufferPntr == NULL))
      {
        if (MessageBufferPntr != &MessageBufferOnStack)
          GlobalFreePtr (MessageBufferPntr);
        if (MessageLength < sizeof (MonopolyNetMessageHeaderRecord))
          MessageLength = sizeof (MonopolyNetMessageHeaderRecord);
        MessageBufferPntr = (MessageBufferPointer) GlobalAllocPtr (GHND, MessageLength); /* Allocates and locks */
        if (MessageBufferPntr == NULL)
          ErrorCode = DPERR_OUTOFMEMORY;
        else /* Try again with this larger buffer. */
          ErrorCode = DPERR_BUFFERTOOSMALL;
      }
    }

    if (ErrorCode != DP_OK)
    {
      /* Stop processing when we hit a normal error like DPERR_NOMESSAGES
        or any other error too. */

      if (MessageBufferPntr != &MessageBufferOnStack &&
      MessageBufferPntr != NULL)
        GlobalFreePtr (MessageBufferPntr);
      break;
    }

    if (FromID == DPID_SYSMSG) /* A system message. */
    {
      switch (MessageBufferPntr->genericDPMessage.dwType)
      {
      case DPSYS_CREATEPLAYERORGROUP:
        if (MessageBufferPntr->createDPMessage.dwPlayerType != DPPLAYERTYPE_PLAYER)
          break; /* A group was added, ignore it. */

        /* Make a note of the new player, and ignore him for now (don't send or
           receive from him).  See MESS_LetNewComputersIntoTheGame. */

        memset (&TempNetAddr, 0, sizeof (TempNetAddr));
        TempNetAddr.networkSystem = NS_DIRECTPLAY;
        TempNetAddr.playerID = MessageBufferPntr->createDPMessage.dpId;
        UpdateForNewlyConnectedComputer ((RULE_NetworkAddressPointer) &TempNetAddr);
        break;


      case DPSYS_DESTROYPLAYERORGROUP: /* A player was deleted, remove from active list. */
        if (MessageBufferPntr->destroyDPMessage.dwPlayerType != DPPLAYERTYPE_PLAYER)
          break; /* A group was destroyed, ignore it. */

        memset (&TempNetAddr, 0, sizeof (TempNetAddr));
        TempNetAddr.networkSystem = NS_DIRECTPLAY;
        TempNetAddr.playerID = MessageBufferPntr->destroyDPMessage.dpId;
        UpdateForDisconnectedConversation ((RULE_NetworkAddressPointer) &TempNetAddr);
        break;


      case DPSYS_SESSIONLOST: /* Whole communications media lost. */
        DBUG_DisplayInternationalMessage (T_MESS_ERROR_DP_SESSION_LOST, 0, 0, 0, NULL);
        MESS_StopDirectPlay ();
        return;  /* DirectPlay now invalid, nothing more to do. */


      case DPSYS_CHAT:
        /* Received some chat text.  Presumably from some lobby application
           external to the game.  Forward it to the server for retransmission
           as if from a spectator.  To avoid multiple copies, don't forward it
           if it is a broadcast and we aren't the server. */

        if (MessageBufferPntr->chatDPMessage.idToPlayer == 0 && !MESS_ServerMode)
          break; /* A broadcast and we aren't the server. */
        if (MessageBufferPntr->chatDPMessage.idFromPlayer == MyDPPlayerID)
          break; /* Ignore messages sent by us. */
        if (MessageBufferPntr->chatDPMessage.lpChat == NULL ||
        MessageBufferPntr->chatDPMessage.lpChat->lpszMessage == NULL)
          break; /* Bad pointers. */
        ChatLength = wcslen (MessageBufferPntr->chatDPMessage.lpChat->lpszMessage);
        ChatMemoryHandle = RULE_AllocateHandle (ChatLength + 1);
        if (ChatMemoryHandle == NULL)
          break; /* Out of memory. */
        ChatMemoryPntr = (unsigned short *) RULE_LockHandle (ChatMemoryHandle);
        wcscpy (ChatMemoryPntr, MessageBufferPntr->chatDPMessage.lpChat->lpszMessage);
        RULE_UnlockHandle (ChatMemoryHandle);
        ChatMemoryPntr = NULL;
        MESS_SendAction (ACTION_TEXT_CHAT, RULE_SPECTATOR_PLAYER, RULE_BANK_PLAYER,
          RULE_ALL_PLAYERS, 0, 0, 0,
          NULL, 0, ChatMemoryHandle);
        ChatMemoryHandle = NULL;
        break;
      }
    }
    else if (ToID == MyDPPlayerID) /* A real message to us. */
    {
      memset (&TempNetAddr, 0, sizeof (TempNetAddr));
      TempNetAddr.networkSystem = NS_DIRECTPLAY;
      TempNetAddr.playerID = FromID;
      UnmarshallAndQueueMessage ((BYTE *) MessageBufferPntr, MessageLength,
        (MessageBufferPntr == &MessageBufferOnStack) ?
          NULL :
          GlobalPtrHandle (MessageBufferPntr),
        (RULE_NetworkAddressPointer) &TempNetAddr);
      MessageBufferPntr = NULL; /* So it doesn't get deallocated. */
    }
    else /* Message to someone else? */
    {
#if _DEBUG
      DBUG_DisplayNonFatalErrorMessage ("CheckForDirectPlayReceivedData: DirectPlay message received for someone else.");
#endif
    }

    /* Finished processing that message, deallocate temporary allocations. */

    if (MessageBufferPntr != NULL && MessageBufferPntr != &MessageBufferOnStack)
      GlobalFreePtr (MessageBufferPntr);
  }
}



/*****************************************************************************
 * Send the buffer to the given DirectPlay network address.
 */

static void StartSendingBufferViaDirectPlay (BYTE *BufferPntr, DWORD BufferSize, DPID ToID)
{
  HRESULT                       ErrorCode;
  NetAddressForDirectPlayRecord TempNetAddr;

  if (pDPlayN != NULL && ToID != MyDPPlayerID)
  {
    ErrorCode = pDPlayN->Send (MyDPPlayerID, ToID, DPSEND_GUARANTEED,
      BufferPntr, BufferSize);

    /* Ignore non-fatal errors, mark the player we were sending to
       as disconnected for other errors. */

    if (ErrorCode != DP_OK &&
    ErrorCode != DPERR_CONNECTING &&
    ErrorCode != DPERR_SENDTOOBIG &&
    ErrorCode != DPERR_BUSY)
    {
      /* Oops, player we were sending to is no longer there.  Could be
         the server disconnecting unexpectedly too. */

      memset (&TempNetAddr, 0, sizeof (TempNetAddr));
      TempNetAddr.networkSystem = NS_DIRECTPLAY;
      TempNetAddr.playerID = ToID;

      UpdateForDisconnectedConversation ((RULE_NetworkAddressPointer) &TempNetAddr);
    }
  }
}



/******************************************************************************
 * Update the scores on the Zone or other network lobby.  This one sends
 * pretty much the whole game state using a custom message.  If the lobby
 * doesn't understand it, it merely wastes bandwidth (this is usually called
 * at the start of a player's turn and also when the game is finished).
 */

void MESS_UpdateLobbyWithCurrentGameInfo (MESS_ZoneGameUpdateInfoPointer ZoneGameUpdateInfoPntr)
{
  struct PropScoreStruct
  {
    DPLMSG_SETPROPERTY  lobbyMessageSetProp;
    char                data [sizeof (MESS_ZoneGameUpdateInfoRecord) - 4];
      // Data actually starts 4 bytes earlier, at lobbyMessageSetProp.dwPropertyData[0],
      // if they had used a zero sized array then there wouldn't be this problem.
  } UpdateInfo;

  MESS_ZoneGameUpdateInfoPointer  InfoPntr;

  if (pDPLobbyN != NULL &&
  LobbySupportsStandardMessages &&
  LobbySupportsGameStatusUpdateMessages)
  {
    memset (&UpdateInfo, 0, sizeof (UpdateInfo));
    UpdateInfo.lobbyMessageSetProp.dwType = DPLSYS_SETPROPERTY;
    UpdateInfo.lobbyMessageSetProp.dwRequestID = DPL_NOCONFIRMATION;
    UpdateInfo.lobbyMessageSetProp.guidPlayer = GUID_NULL; // Doesn't apply to a particular player.
    UpdateInfo.lobbyMessageSetProp.guidPropertyTag = ZoneGameStatusUpdateGUID;
    UpdateInfo.lobbyMessageSetProp.dwDataSize = sizeof (MESS_ZoneGameUpdateInfoRecord);

    InfoPntr = (MESS_ZoneGameUpdateInfoPointer) &UpdateInfo.lobbyMessageSetProp.dwPropertyData[0];
    *InfoPntr = *ZoneGameUpdateInfoPntr; // Copy the whole status record.

    pDPLobbyN->SendLobbyMessage (
      DPLMSG_STANDARD, 0, &UpdateInfo, sizeof (UpdateInfo));
  }
}



/*****************************************************************************
 * Tell the Zone or other lobby that the game has started.  Started means
 * that it has finished game configuration and play really has started.
 */

void MESS_UpdateLobbyGameStarted (void)
{
  DPLMSG_SETPROPERTY  ZoneMsg;

  if (pDPLobbyN != NULL &&
  LobbySupportsStandardMessages)
  {
    // Fill in set property header
    memset (&ZoneMsg, 0, sizeof (ZoneMsg));

    ZoneMsg.dwType = DPLSYS_SETPROPERTY;
    ZoneMsg.dwRequestID = DPL_NOCONFIRMATION;
    ZoneMsg.guidPlayer = GUID_NULL;
    ZoneMsg.guidPropertyTag = ZONEPROPERTY_GameStateGUID;
    ZoneMsg.dwDataSize = sizeof (DWORD);
    ZoneMsg.dwPropertyData[0] = ZSTATE_START;

    pDPLobbyN->SendLobbyMessage (
      DPLMSG_STANDARD, 0, &ZoneMsg, sizeof (ZoneMsg));
  }
}



/*****************************************************************************
 * Tell the Zone or other lobby that the game has finished.  Presumably
 * the last game status update contained the final scores.  Don't send
 * any more scores until the next game start.
 */

void MESS_UpdateLobbyGameFinished (void)
{
  DPLMSG_SETPROPERTY  ZoneMsg;

  if (pDPLobbyN != NULL &&
  LobbySupportsStandardMessages)
  {
    // Fill in set property header
    memset (&ZoneMsg, 0, sizeof (ZoneMsg));

    ZoneMsg.dwType = DPLSYS_SETPROPERTY;
    ZoneMsg.dwRequestID = DPL_NOCONFIRMATION;
    ZoneMsg.guidPlayer = GUID_NULL;
    ZoneMsg.guidPropertyTag = ZONEPROPERTY_GameStateGUID;
    ZoneMsg.dwDataSize = sizeof (DWORD);
    ZoneMsg.dwPropertyData[0] = ZSTATE_END;

    pDPLobbyN->SendLobbyMessage (
      DPLMSG_STANDARD, 0, &ZoneMsg, sizeof (ZoneMsg));
  }
}



/******************************************************************************
 * Utility function for extracting the port number from a given string.
 * It is the number after the ':' character.  Returns 0 if it isn't there.
 */

static short PortNumberFromString (wchar_t *String)
{
   wchar_t  *ColonPntr;

   /* Reverse search for the colon symbol. */
   ColonPntr = wcsrchr (String, L':');
   if (ColonPntr == NULL)
      return 0; /* No port specified. */

   return (short) wcstol (ColonPntr + 1 /* Skip over ':' character to get to number */, NULL, 10);
}



/******************************************************************************
 * Prints this computer's IP address into the given string buffer,
 * and returns TRUE if successful.  Returns FALSE and sets the string to
 * an error message if it fails.  BufferSize counts characters, not bytes.
 * Uses just the numeric format, DNS values probably won't work for dialup
 * users.
 */

BOOL GetMyTCPAddress (wchar_t *Buffer, DWORD BufferSize)
{
  PHOSTENT       HostEntryPntr;
  char           HostName [80];
  SOCKADDR_IN    InternetAddress;
  char           InternetString [80];
  char          *StringPntr;

  wcsncpy (Buffer, LANG_GetTextMessageClean (T_MESS_WSOCK_NOT_AVAILABLE), BufferSize);

  if (!MESS_WinSockSystemActive)
    return FALSE;

  if (Callgethostname (HostName, sizeof (HostName)))
    return FALSE;

  InternetString [0] = 0;
  HostEntryPntr = Callgethostbyname (HostName);
  if (HostEntryPntr != NULL)
  {
    memcpy (&InternetAddress.sin_addr, HostEntryPntr->h_addr,
      HostEntryPntr->h_length);
    StringPntr = Callinet_ntoa (InternetAddress.sin_addr);
    if (StringPntr != NULL)
      strcpy (InternetString, StringPntr);
  }
  if (InternetString [0] == 0)
    return FALSE; /* Didn't get anything. */

  mbstowcs (Buffer, InternetString, BufferSize);
  if (wcslen (Buffer) + 7 >= BufferSize)
    return FALSE; /* Not enough string space for ":99999" */

  wcscat (Buffer, L":");

  _ltow ((long) TCPPortNumber, Buffer + wcslen (Buffer), 10);

  return TRUE;
}



/******************************************************************************
 * Disconnect the given entity (index to WinSockPartialData), returns
 * TRUE if successful.  Note that the socket may linger on (the default),
 * transmitting outstanding send data unless the SO_LINGER option is set with a
 * zero time delay.
 */

static BOOL DisconnectSock (int Index)
{
  BOOL                ErrorHappened;
  SOCKET              MySocket;
  PartialDataPointer  PartialPntr;

  if (Index < 0 || Index >= MAX_WINSOCK_PLAYERS)
    return FALSE;
  PartialPntr = WinSockPartialData + Index;

  ErrorHappened = FALSE;
  MySocket = PartialPntr->socketID;

  if (MySocket != INVALID_SOCKET && MESS_WinSockSystemActive)
    ErrorHappened = Callclosesocket (MySocket);
  PartialPntr->socketID = INVALID_SOCKET;

  /* Deallocate any dynamically allocated receive buffers. */

  if (PartialPntr->receiveBuffer != NULL &&
  PartialPntr->receiveBuffer != (BYTE *) &PartialPntr->shortMonopolyMessage)
    GlobalFreePtr (PartialPntr->receiveBuffer);
  PartialPntr->receiveBuffer = NULL;

  return !ErrorHappened;
}



/*****************************************************************************
 * Run the modal dialog box that asks the user for an IP address.  The user
 * can cancel.  Returns TRUE when successful, returns FALSE if cancelled.
 * Result returned in UserIPString, initial value there too.
 */

#define USER_IP_STRING_LENGTH 79
static wchar_t UserIPString [USER_IP_STRING_LENGTH + 1];

BOOL CALLBACK GetIPAddressDialogProc (
HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
  int           ControlID;
  HWND          hwndControl;
  int           NotificationCode;
  char          TempString [2 * MESS_MAX_PROMPT_TEXT_LENGTH + 2];

  if (Message == WM_INITDIALOG)
  {
    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_GETIP_TITLE), sizeof (TempString));
    SetWindowText (hwndDlg, TempString);

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_GETIP_PROMPT), sizeof (TempString));
    SetDlgItemText (hwndDlg, IDC_MESS_PROMPT_TEXT, TempString);

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BUTTON_OK), sizeof (TempString));
    SetDlgItemText (hwndDlg, IDOK, TempString);

    wcstombs (TempString, UserIPString, sizeof (TempString));
    SetDlgItemText (hwndDlg, IDC_MESS_GETIP_ADDRESS, TempString);

    return TRUE; /* Nonzero to set default focus. */
  }

  if (Message == WM_COMMAND)
  {
    ControlID = GET_WM_COMMAND_ID (wParam, lParam);
    NotificationCode = GET_WM_COMMAND_CMD (wParam, lParam);
    hwndControl = GET_WM_COMMAND_HWND (wParam, lParam);

    switch (ControlID)
    {
    case IDCANCEL: /* Includes the close box */
      EndDialog (hwndDlg, FALSE);
      break;

    case IDOK:
      GetDlgItemText (hwndDlg, IDC_MESS_GETIP_ADDRESS, TempString, sizeof (TempString));
      mbstowcs (UserIPString, TempString, USER_IP_STRING_LENGTH);
      UserIPString [USER_IP_STRING_LENGTH] = 0;
      EndDialog (hwndDlg, TRUE);
      break;

    default:
      break;
    }
  }
  return FALSE; /* Allow furthur default message handling by the dialog box. */
}



/*****************************************************************************
 * Run the modal dialog box that shows the user the current IP address,
 * taken from the UserIPString variable.
 */

BOOL CALLBACK ShowIPAddressDialogProc (
HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
  int           ControlID;
  HWND          hwndControl;
  int           NotificationCode;
  char          TempString [2 * MESS_MAX_PROMPT_TEXT_LENGTH + 2];

  if (Message == WM_INITDIALOG)
  {
    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_SHOWIP_TITLE), sizeof (TempString));
    SetWindowText (hwndDlg, TempString);

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_SHOWIP_PROMPT), sizeof (TempString));
    SetDlgItemText (hwndDlg, IDC_MESS_PROMPT_TEXT, TempString);

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BUTTON_OK), sizeof (TempString));
    SetDlgItemText (hwndDlg, IDOK, TempString);

    wcstombs (TempString, UserIPString, sizeof (TempString));
    SetDlgItemText (hwndDlg, IDC_MESS_SHOWIP_ADDRESS, TempString);

    return TRUE; /* Nonzero to set default focus. */
  }

  if (Message == WM_COMMAND)
  {
    ControlID = GET_WM_COMMAND_ID (wParam, lParam);
    NotificationCode = GET_WM_COMMAND_CMD (wParam, lParam);
    hwndControl = GET_WM_COMMAND_HWND (wParam, lParam);

    switch (ControlID)
    {
    case IDCANCEL: /* Includes the close box */
      EndDialog (hwndDlg, FALSE);
      break;

    case IDOK:
      EndDialog (hwndDlg, TRUE);
      break;

    default:
      break;
    }
  }
  return FALSE; /* Allow furthur default message handling by the dialog box. */
}



/*****************************************************************************
 * Turns off WinSock socket based TCP/IP networking.
 */

static void TurnOffWinSock (void)
{
  int Index;

  if (MESS_WinSockSystemActive)
  {
    for (Index = 0; Index < MAX_WINSOCK_PLAYERS; Index++)
    {
      if (WinSockPartialData[Index].socketID == INVALID_SOCKET)
        continue;

      UpdateForDisconnectedConversation ((RULE_NetworkAddressPointer)
        &WinSockPartialData[Index].standardAddress);

      DisconnectSock (Index);
    }

    if (ServerListenSocket != INVALID_SOCKET)
    {
      Callclosesocket (ServerListenSocket);
      ServerListenSocket = INVALID_SOCKET;
    }
    CallWSACleanup ();
    MESS_WinSockSystemActive = FALSE;
  }

  if (WinsockLibraryInstance != NULL && NeedToFreeLibraryWinsock)
    FreeLibrary (WinsockLibraryInstance);

  WinsockLibraryInstance = NULL;
  NeedToFreeLibraryWinsock = FALSE;
}



/*****************************************************************************
 * Open a WinSock connection if the "-winsock" keyword is present in the
 * command line.  Use the "-server" keyword in the command line to decide
 * if we should be a server (listening socket) or "-client" to connect to
 * some external server.  The address is also in the command line using
 * "-ipaddress" and "-tcpport".  See MESS_StartNetworking for more comments.
 */

static void MESS_SocketStartup (wchar_t *CommandLine)
{
#if _DEBUG
  char          TempString [80];
#endif

  char          AnsiStrippedName [80];
  int           ErrorCode;
  unsigned long FourByteAddress;
  PHOSTENT      HostEntryPntr;
  int           i;
  char          MyBooleanC;
  unsigned long MyBoolean; /* Special WinSock boolean */
  SOCKET        MySocket = INVALID_SOCKET;
  SOCKADDR_IN   SocketInternetAddress;
  wchar_t      *StringPntr;
  wchar_t       StrippedName [80];
  short         SuggestedPortNumber;
  WSADATA       WSAData;

  if (wcsstr (CommandLine, L"-winsock") == NULL)
    return; /* Don't want to turn it on, leave it as is. */

  if (MESS_WinSockSystemActive)
    return; /* Already on, leave it on. */

  /* Initialise the array that keeps track of open connections. */

  memset (WinSockPartialData, 0, sizeof (WinSockPartialData));
  for (i = 0; i < MAX_WINSOCK_PLAYERS; i++)
    WinSockPartialData[i].socketID = INVALID_SOCKET;

  /* Load the Winsock DLL, searches the standard paths.  If it is loaded,
     fill in our pointers to functions in Winsock. */

  ErrorCode = -1;

  if (WinsockLibraryInstance == NULL)
  {
    WinsockLibraryInstance = LoadLibrary ("WSOCK32");

    if (WinsockLibraryInstance == NULL)
      ErrorCode = GetLastError ();

    if (WinsockLibraryInstance != NULL)
      NeedToFreeLibraryWinsock = TRUE;
  }

  if (WinsockLibraryInstance == NULL)
  {
    /* As a last ditch attempt, could try GetModuleHandle.  But we don't. */

#if _DEBUG
    sprintf (TempString, "Unable to load TCP/IP's WSOCK32.DLL, code %d.",
      ErrorCode);
    DBUG_DisplayNonFatalErrorMessage (TempString);
#endif
    goto FAILURE_EXIT;
  }

  pfaccept = (type_accept) GetProcAddress (WinsockLibraryInstance, "accept");
  pfbind = (type_bind) GetProcAddress (WinsockLibraryInstance, "bind");
  pfclosesocket = (type_closesocket) GetProcAddress (WinsockLibraryInstance, "closesocket");
  pfconnect = (type_connect) GetProcAddress (WinsockLibraryInstance, "connect");
  pfgethostbyname = (type_gethostbyname) GetProcAddress (WinsockLibraryInstance, "gethostbyname");
  pfgethostname = (type_gethostname) GetProcAddress (WinsockLibraryInstance, "gethostname");
  pfhtons = (type_htons) GetProcAddress (WinsockLibraryInstance, "htons");
  pfinet_addr = (type_inet_addr) GetProcAddress (WinsockLibraryInstance, "inet_addr");
  pfinet_ntoa = (type_inet_ntoa) GetProcAddress (WinsockLibraryInstance, "inet_ntoa");
  pfioctlsocket = (type_ioctlsocket) GetProcAddress (WinsockLibraryInstance, "ioctlsocket");
  pflisten = (type_listen) GetProcAddress (WinsockLibraryInstance, "listen");
  pfrecv = (type_recv) GetProcAddress (WinsockLibraryInstance, "recv");
  pfsend = (type_send) GetProcAddress (WinsockLibraryInstance, "send");
  pfsetsockopt = (type_setsockopt) GetProcAddress (WinsockLibraryInstance, "setsockopt");
  pfsocket = (type_socket) GetProcAddress (WinsockLibraryInstance, "socket");
  pfWSAAsyncSelect = (type_WSAAsyncSelect) GetProcAddress (WinsockLibraryInstance, "WSAAsyncSelect");
  pfWSACleanup = (type_WSACleanup) GetProcAddress (WinsockLibraryInstance, "WSACleanup");
  pfWSAGetLastError = (type_WSAGetLastError) GetProcAddress (WinsockLibraryInstance, "WSAGetLastError");
  pfWSAStartup = (type_WSAStartup) GetProcAddress (WinsockLibraryInstance, "WSAStartup");

  if (pfWSAStartup == NULL) /* Happens if you load 16 bit DLL in 32 bit mode. */
  {
    ErrorCode = GetLastError ();
#if _DEBUG
    sprintf (TempString,
      "Unable to get function pointers for WinSock, code %d.", ErrorCode);
    DBUG_DisplayNonFatalErrorMessage (TempString);
#endif
    goto FAILURE_EXIT;
  }

  /* Got the WinSock DLL, now initialise it. */

  memset (&WSAData, 0, sizeof (WSAData));
  ErrorCode = CallWSAStartup (0x101 /* We need version 1.1 */, &WSAData);
  if (ErrorCode != 0)
  {
#if _DEBUG
    sprintf (TempString, "Error %d in WSAStartup.", ErrorCode);
    DBUG_DisplayNonFatalErrorMessage (TempString);
#endif
    goto FAILURE_EXIT;
  }

  /* Did we get version 1.1?  Reject if we didn't. */

  if (LOBYTE (WSAData.wVersion) != 1 || HIBYTE (WSAData.wVersion) != 1)
  {
#if _DEBUG
    DBUG_DisplayNonFatalErrorMessage (
      "The current Winsock library doesn't support the version 1.1 interface.");
#endif
    CallWSACleanup ();
    goto FAILURE_EXIT;
  }

  MESS_WinSockSystemActive = TRUE;

  /* Decode the command line.  See if the user specified a
     port number to use (used in both client and server modes,
     though the client mode IP address can specify it too). */

  StringPntr = wcsstr (CommandLine, L"-tcpport");
  if (StringPntr != NULL)
  {
    StringPntr += wcslen (L"-tcpport");
    SuggestedPortNumber = (short) wcstol (StringPntr, NULL, 10);
    if (SuggestedPortNumber > 0) /* Yes, address overrides port number. */
      TCPPortNumber = SuggestedPortNumber;
  }

  /* See if the user specified an IP address on the command line for
     client mode.  If not, ask them for it.  Store it in StrippedName. */

  StrippedName [0] = 0;
  if (!MESS_ServerMode)
  {
    StringPntr = wcsstr (CommandLine, L"-ipaddress");
    if (StringPntr != NULL) /* If found it, skip over the keyword. */
      StringPntr += wcslen (L"-ipaddress");

    if (StringPntr == NULL)
    {
      /* Need to get an IP address from the user, none on the command line.
         The initial value is your own address. */

      if (!GetMyTCPAddress (UserIPString, USER_IP_STRING_LENGTH))
        UserIPString [0] = 0;
      if (!DialogBox (ApplicationInstance, MAKEINTRESOURCE (IDD_MESS_ASK_IPADDRESS),
      MainWindowHandle, GetIPAddressDialogProc))
        goto FAILURE_EXIT; /* User cancelled. */
      StringPntr = UserIPString;
    }

    /* Skip leading spaces. */

    while (*StringPntr == L' ')
      StringPntr++;

    /* Copy the address to the local string.  Stop at a space or
       colon separator, or if we run out of data. */

    i = 0;
    while (*StringPntr != 0 && *StringPntr != L' ' &&
    *StringPntr != L':' && i < 78)
      StrippedName [i++] = *StringPntr++;
    StrippedName [i] = 0;

    /* Check for a port number after the main address,
       a colon character separates them. */

    if (*StringPntr == L':')
    {
      SuggestedPortNumber = (short) wcstol (StringPntr + 1, NULL, 10);
      if (SuggestedPortNumber > 0) /* Yes, address overrides port number. */
        TCPPortNumber = SuggestedPortNumber;
    }
  }

  /* Open a listening socket if this is a server. */

  if (MESS_ServerMode)
  {
    /* Try to create a socket. */

    ServerListenSocket = Callsocket (AF_INET /* Use Internet address family */,
      SOCK_STREAM /* Want stream TCP based kind (has error correction) */,
      0 /* Use protocol that matches address family */);

    if (ServerListenSocket == INVALID_SOCKET)
    {
      ErrorCode = CallWSAGetLastError ();
#if _DEBUG
      sprintf (TempString,
        "Creation of Server's listening socket failed, code %d.", ErrorCode);
      DBUG_DisplayNonFatalErrorMessage (TempString);
#endif
      goto FAILURE_EXIT;
    }

    /* Set the socket option to turn on routing, we want to allow
       connections to systems outside of our LAN. */

    MyBooleanC = FALSE;
    Callsetsockopt (ServerListenSocket, SOL_SOCKET, SO_DONTROUTE,
      &MyBooleanC, sizeof (MyBooleanC));

    /* Bind the local address and our port number to the listen socket.
       Use the wildcard address, so it will accept calls on any network
       interface in this computer. */

    memset (&SocketInternetAddress, 0, sizeof (SocketInternetAddress));
    SocketInternetAddress.sin_family = AF_INET;
    SocketInternetAddress.sin_port = Callhtons (TCPPortNumber);
    SocketInternetAddress.sin_addr.s_addr = INADDR_ANY;

    if (Callbind (ServerListenSocket,
      (struct sockaddr FAR *) &SocketInternetAddress,
      sizeof (SocketInternetAddress)) != 0)
    {
      ErrorCode = CallWSAGetLastError ();
#if _DEBUG
      sprintf (TempString,
        "Unable to bind local WinSock address to server's listening socket, code %d.",
        ErrorCode);
      DBUG_DisplayNonFatalErrorMessage (TempString);
#endif
      goto FAILURE_EXIT;
    }

    /* Tell the socket to start listening for incoming connection requests. */

    if (Calllisten (ServerListenSocket, 5 /* Max pending connects */) != 0)
    {
      ErrorCode = CallWSAGetLastError ();
#if _DEBUG
      sprintf (TempString,
        "Server's listening socket refuses to listen, code %d.", ErrorCode);
      DBUG_DisplayNonFatalErrorMessage (TempString);
#endif
      goto FAILURE_EXIT;
    }

    /* Don't send a notification message when a connection request comes in...

      if (CallWSAAsyncSelect (ServerListenSocket, MainWindowFrameHandle,
      WM_MSGIO_SOCKET_CONNECTION, FD_ACCEPT) != 0)
      {
      ErrorCode = CallWSAGetLastError ();
      sprintf (TempString,
      "Can't set up notification for server's listening socket, code %d.",
      ErrorCode);
      DisplayMessage (ApplicationName, TempString);
      return FALSE;
      }
      ...End of unused code, we poll rather than use messages.
    */

    /* Also make it a non-blocking socket. */

    MyBoolean = TRUE;
    Callioctlsocket (ServerListenSocket, FIONBIO, &MyBoolean);

    /* Tell the user about the address now that we are listening. */

    if (!GetMyTCPAddress (UserIPString, USER_IP_STRING_LENGTH))
      wcscpy (UserIPString, L"?");

    DialogBox (ApplicationInstance, MAKEINTRESOURCE (IDD_MESS_SHOW_IPADDRESS),
      MainWindowHandle, ShowIPAddressDialogProc);
  }
  else /* Client mode, call up the server. */
  {
    /* Try to create a socket. */

    MySocket = Callsocket (AF_INET /* Use Internet address family */,
      SOCK_STREAM /* Want stream TCP based kind (has error correction) */,
      0 /* Use protocol that matches address family */);

    if (MySocket == INVALID_SOCKET)
    {
      ErrorCode = CallWSAGetLastError ();
#if _DEBUG
      sprintf (TempString,
        "Creation of client Winsock socket failed, code %d.", ErrorCode);
      DBUG_DisplayNonFatalErrorMessage (TempString);
#endif
      goto FAILURE_EXIT;
    }

    /* Set the socket option to turn on routing, we want to allow
       connections to systems outside of our LAN. */

    MyBooleanC = FALSE;
    Callsetsockopt (MySocket, SOL_SOCKET, SO_DONTROUTE,
      &MyBooleanC, sizeof (MyBooleanC));

    /* Find the address of the entity we want to connect to.  Try looking
       it up in the host name database first.  If not found, try interpreting
       it as a 1.2.3.4 format address. */

    wcstombs (AnsiStrippedName, StrippedName, sizeof (AnsiStrippedName));
    memset (&SocketInternetAddress, 0, sizeof (SocketInternetAddress));

    HostEntryPntr = Callgethostbyname (AnsiStrippedName);
    if (HostEntryPntr == NULL)
      ErrorCode = CallWSAGetLastError ();
    else
    {
      SocketInternetAddress.sin_family = AF_INET;
      SocketInternetAddress.sin_port = Callhtons (TCPPortNumber);
      memcpy (&SocketInternetAddress.sin_addr, HostEntryPntr->h_addr,
        HostEntryPntr->h_length);
      ErrorCode = 0;
    }

    if (ErrorCode != 0)
    {
      /* Not in hosts file, probably a raw address. */

      FourByteAddress = Callinet_addr (AnsiStrippedName);
      if (FourByteAddress != INADDR_NONE) /* Converted ok? */
      {
        SocketInternetAddress.sin_family = AF_INET;
        SocketInternetAddress.sin_port = Callhtons (TCPPortNumber);
        SocketInternetAddress.sin_addr.s_addr = FourByteAddress;
        ErrorCode = 0;
      }
    }

    if (ErrorCode)
    {
#if _DEBUG
      sprintf (TempString,
        "\"%s\" doesn't appear to be a valid Internet address, code %d.",
        AnsiStrippedName, ErrorCode);
      DBUG_DisplayNonFatalErrorMessage (TempString);
#endif
      goto FAILURE_EXIT;
    }

    /* Now that we have an address, try making the connection.  Maybe we
       should put up a dialog box here to tell the user to wait?  The
       connect also binds a default local address to the socket (so we
       don't have to do a separate bind operation). */

    if (Callconnect (MySocket, (PSOCKADDR) &SocketInternetAddress,
    sizeof (SocketInternetAddress)) != 0)
    {
      ErrorCode = CallWSAGetLastError ();
#if _DEBUG
      sprintf (TempString,
        "Unable to connect to \"%s\", code %d.", AnsiStrippedName, ErrorCode);
      DBUG_DisplayNonFatalErrorMessage (TempString);
#endif
      goto FAILURE_EXIT;
    }

    /* Set up asynchronous window message notification for incoming data
       and disconnection events... which we don't use any more.

#if DEBUGART
    if (MainWindowFrameHandle == NULL)
      ErrorExit ("No main window for MsgioSok to post to.");
#endif

    if (CallWSAAsyncSelect (MySocket, MainWindowFrameHandle,
      WM_MSGIO_SOCKET_RECEIVE, FD_READ | FD_CLOSE) != 0)
    {
      ErrorCode = CallWSAGetLastError ();
      sprintf (TempString,
        "Unable to set up notification system for socket to \"%s\", code %d.",
        AnsiStrippedName, ErrorCode);
      DisplayMessage (ApplicationName, TempString);
      goto FAILURE_EXIT;
    }
    */

    /* Also make it a non-blocking socket. */

    MyBoolean = TRUE;
    Callioctlsocket (MySocket, FIONBIO, &MyBoolean);

    /* Remember our connection. */

    WinSockPartialData[0].socketID = MySocket;
    MySocket = INVALID_SOCKET; /* Avoid double close in case failure code gets called. */
    WinSockPartialData[0].standardAddress.networkSystem = NS_WINSOCK;
    WinSockPartialData[0].standardAddress.connectionIndex = 0;
    WinSockPartialData[0].standardAddress.IPAddress = SocketInternetAddress;

    /* It is a connection to the server, so store the server's address. */

    memcpy (AddressesForPlayers + RULE_BANK_PLAYER,
      &WinSockPartialData[0].standardAddress,
      sizeof (RULE_NetworkAddressRecord));

    UpdateForNewlyConnectedComputer (
      (RULE_NetworkAddressPointer) &WinSockPartialData[0].standardAddress);
    MESS_LetNewComputersIntoTheGame (); /* Server is always in the game. */
  }

  /* Finished initialisation successfully. */

  return;

FAILURE_EXIT:
  if (MySocket != INVALID_SOCKET)
    Callclosesocket (MySocket);
  TurnOffWinSock ();
}



/*****************************************************************************
 * Receive any WinSock messages from the given connection.  Collect partial
 * data until we have a full message.
 */

static void WinSockReceivePartialOne (int PartialIndex)
{
#if _DEBUG
  char                TempString [120];
#endif

  int                 AmountReceived;
  DWORD              *DWordPntr;
  int                 ErrorCode;
  int                 i, j;
  PartialDataPointer  PartialPntr;

  PartialPntr = WinSockPartialData + PartialIndex;

  if (PartialPntr->receiveBuffer == NULL)
  {
    /* Just starting out, begin by accumulating the header. */

    PartialPntr->totalToReceive = MONOPOLY_MESSAGE_HEADER_LENGTH;
    PartialPntr->amountReceivedSoFar = 0;
    PartialPntr->receiveBuffer = (BYTE *) &PartialPntr->shortMonopolyMessage;
  }

  AmountReceived = Callrecv (PartialPntr->socketID,
    (char *)(PartialPntr->receiveBuffer + PartialPntr->amountReceivedSoFar),
    PartialPntr->totalToReceive - PartialPntr->amountReceivedSoFar,
    0 /* No special flags */);

  if (AmountReceived == SOCKET_ERROR ||
  AmountReceived == 0 /* Receive zero if socket closed */)
  {
    if (AmountReceived == 0)
      ErrorCode = -1;
    else
      ErrorCode = CallWSAGetLastError ();

    if (ErrorCode != WSAEWOULDBLOCK)
    {
      /* A bad error, close this connection.  Get WSAEWOULDBLOCK
         when reading when there isn't any data, so don't treat
         that as a fatal error. */

      UpdateForDisconnectedConversation (
        (RULE_NetworkAddressPointer) &PartialPntr->standardAddress);
      DisconnectSock (PartialIndex);
    }
    return;  /* Read my lips - no new data. */
  }

  /* Ok, have some more data. */

  PartialPntr->amountReceivedSoFar += AmountReceived;

  if (PartialPntr->amountReceivedSoFar < PartialPntr->totalToReceive)
    return; /* Still collecting data, do nothing yet. */

  /* Now have enough data.  Decide what kind it is based on the size received.
     If it is small (MONOPOLY_MESSAGE_HEADER_LENGTH) then it is the header
     between messages that we are looking for.  If it is larger then it must
     be a Monopoly message, which will be decoded and added to the queue. */

  if (PartialPntr->totalToReceive == MONOPOLY_MESSAGE_HEADER_LENGTH)
  {
    /* Check if we have the header.  If not, throw away the oldest byte
       and shift everything over and keep on trying. */

    if (memcmp (PartialPntr->receiveBuffer, MonopolyMessageHeaderMarker, 8) == 0)
    {
      /* Yes, have the header.  Read the size and start receiving the body. */

      DWordPntr = (unsigned long *) (PartialPntr->receiveBuffer + 8); /* Point to size DWORD. */
      PartialPntr->totalToReceive = *DWordPntr;
#if _DEBUG
      if (PartialPntr->totalToReceive == MONOPOLY_MESSAGE_HEADER_LENGTH)
        DBUG_DisplayNonFatalErrorMessage ("WinSockReceivePartialOne bug: Receiving message "
        "with same length as inter-message header.  Could be confusing.");
#endif
      PartialPntr->amountReceivedSoFar = 0;

      if (PartialPntr->totalToReceive <= sizeof (PartialPntr->shortMonopolyMessage))
      {
        /* Small enough to fit in our local buffer, use it rather than
           doing memory allocations. */

        PartialPntr->receiveBuffer = (BYTE *) &PartialPntr->shortMonopolyMessage;
      }
      else /* Really large message, allocate some space for it. */
      {
        PartialPntr->receiveBuffer = (unsigned char *)
          GlobalAllocPtr (GMEM_MOVEABLE, PartialPntr->totalToReceive);

        if (PartialPntr->receiveBuffer == NULL)
        {
          /* If it is NULL we will automatically go back to looking for the
             next message header. */
#if _DEBUG
          sprintf (TempString, "Ran out of memory while receiving %ld byte message.",
            (long) PartialPntr->totalToReceive);
          DBUG_DisplayNonFatalErrorMessage (TempString);
#endif
        }
      }
    }
    else /* Don't have the right header, drop and shift. */
    {
      /* Got a pile of stuff.  Drop off letters until we get the first letter
         in the header, if any.  Then resume collecting. */

      for (i = 0; i < MONOPOLY_MESSAGE_HEADER_LENGTH; i++)
      {
        if (PartialPntr->receiveBuffer [i] == MonopolyMessageHeaderMarker [0])
          break;
      }
      for (j = 0; j < MONOPOLY_MESSAGE_HEADER_LENGTH - i; j++)
        PartialPntr->receiveBuffer [j] = PartialPntr->receiveBuffer [i + j];

      PartialPntr->amountReceivedSoFar = MONOPOLY_MESSAGE_HEADER_LENGTH - i;
    }
  }
  else /* A Monopoly message, not the between messages header. */
  {
    UnmarshallAndQueueMessage (PartialPntr->receiveBuffer,
      PartialPntr->totalToReceive,
      (PartialPntr->receiveBuffer == (BYTE *) &PartialPntr->shortMonopolyMessage) ?
        NULL /* Don't hand over control of the buffer deallocation */ :
        GlobalPtrHandle (PartialPntr->receiveBuffer) /* Let it deallocate it */,
      (RULE_NetworkAddressPointer) &PartialPntr->standardAddress);

    PartialPntr->receiveBuffer = NULL; /* Start searching for intra-message header. */
  }
}



/*****************************************************************************
 * See if anyone wants to connect.  Add a new WinSock partial data entry if
 * they do.
 */

static void WinSockCheckForNewConnections (void)
{
  int                        AddressLength;
  int                        ErrorCode;
  int                        Index;
  SOCKET                     NewSocket;
  SOCKADDR_IN                SocketInternetAddress;

  /* An incoming call may be waiting on a socket that is listening.
     Accept the new connection, this creates a new socket.  We
     also find out the address of the caller here. */

  memset (&SocketInternetAddress, 0, sizeof (SocketInternetAddress));
  SocketInternetAddress.sin_family = AF_INET;
  SocketInternetAddress.sin_port = Callhtons (TCPPortNumber);
  SocketInternetAddress.sin_addr.s_addr = INADDR_ANY;

  AddressLength = sizeof (SocketInternetAddress);

  NewSocket = Callaccept (ServerListenSocket,
    (struct sockaddr FAR *) &SocketInternetAddress,
    &AddressLength);

  if (NewSocket == INVALID_SOCKET)
  {
    ErrorCode = CallWSAGetLastError ();
    if (ErrorCode != WSAEWOULDBLOCK)
    {
      DBUG_DisplayInternationalMessage (T_MESS_ERROR_WS_CONNECT_ACCEPT_FAILURE, ErrorCode, 0, 0, NULL);
      Callclosesocket (ServerListenSocket);
      ServerListenSocket = INVALID_SOCKET;
    }
    return; /* No new connections. */
  }

  /* See if there is an available receive entry. */

  for (Index = 0; Index < MAX_WINSOCK_PLAYERS; Index++)
    if (WinSockPartialData[Index].socketID == INVALID_SOCKET)
      break;
  if (Index >= MAX_WINSOCK_PLAYERS)
  {
    /* Didn't find an empty spot, reject the connection. */

    Callclosesocket (NewSocket);
    return;
  }

  /* Remember the socket in our list of connected computers. */

  WinSockPartialData[Index].standardAddress.networkSystem = NS_WINSOCK;
  WinSockPartialData[Index].standardAddress.connectionIndex = Index;
  WinSockPartialData[Index].standardAddress.IPAddress = SocketInternetAddress;

  if (!UpdateForNewlyConnectedComputer ((RULE_NetworkAddressStruct *) &WinSockPartialData[Index].standardAddress))
  {
    /* Didn't find an empty spot in other array, reject the connection. */

    Callclosesocket (NewSocket);
    return;
  }

  /* Ok, it's legitimate. */

  WinSockPartialData[Index].socketID = NewSocket;
}



/*****************************************************************************
 * Receive any pending WinSock data.
 */

static void CheckForWinSockReceivedData (void)
{
  int Index;

  if (!MESS_WinSockSystemActive)
    return;

  for (Index = 0; Index < MAX_WINSOCK_PLAYERS; Index++)
  {
    if (WinSockPartialData[Index].socketID == INVALID_SOCKET)
      continue;

    WinSockReceivePartialOne (Index);
  }

  /* Also see if anyone else wants to connect to this server. */

  if (ServerListenSocket != INVALID_SOCKET)
    WinSockCheckForNewConnections ();
}



/*****************************************************************************
 * Start sending the buffer to the given WinSock computer.
 */

static void StartSendingBufferViaWinSock (BYTE *BufferPntr, DWORD BufferSize, int Index)
{
  PartialDataPointer  PartialPntr;

  PartialPntr = WinSockPartialData + Index;

  PartialPntr->totalToTransmit = BufferSize;
  PartialPntr->amountTransmitted = 0;
  PartialPntr->transmitBuffer = BufferPntr;
}



/******************************************************************************
 * If there is a partially sent message, try sending the unsent part.  If the
 * TCP/IP queues are full, sends part of the message and leave the rest for
 * next time.  Returns TRUE if it is still transmitting, FALSE when it is
 * all sent.
 */

static BOOL WinSockSendPartialData (void)
{
  int                 AmountSent;
  int                 AmountToSend;
  int                 ErrorCode;
  int                 Index;
  PartialDataPointer  PartialPntr;
  BOOL                ReturnCode = FALSE;

  if (!MESS_WinSockSystemActive)
    return FALSE;

  for (Index = 0; Index < MAX_WINSOCK_PLAYERS; Index++)
  {
    PartialPntr = WinSockPartialData + Index;

    if (PartialPntr->socketID == INVALID_SOCKET)
      continue; /* This one is inactive. */

    AmountToSend = PartialPntr->totalToTransmit - PartialPntr->amountTransmitted;

    if (AmountToSend <= 0)
      continue; /* Nothing left to transmit here. */

    /* Send the remaining dregs in the buffer. */

    if (!MESS_WinSockSystemActive)
      return FALSE; /* System has been closed, can happen with disconnections. */

    AmountSent = Callsend (PartialPntr->socketID,
      (char *)(PartialPntr->transmitBuffer + PartialPntr->amountTransmitted),
      AmountToSend, 0 /* flags */);

    /* Get the error code. */

    if (AmountSent == SOCKET_ERROR)
    {
      ErrorCode = CallWSAGetLastError ();
      /* Note - will get WSAEWOULDBLOCK when buffers are totally full. */
      AmountSent = 0;
    }
    else
      ErrorCode = 0;

    /* Update the size remaining to send. */

    PartialPntr->amountTransmitted += AmountSent;

    AmountToSend -= AmountSent;
    if (AmountToSend > 0)
      ReturnCode = TRUE; /* Yes, still more to send later. */

    /* Handle disconnections. */

    if (ErrorCode != 0 && ErrorCode != WSAEWOULDBLOCK)
    {
      /* Oops, player we were sending to is no longer there.  Could be
         the server disconnecting unexpectedly too.  The update will
         then close WinSock as a side effect. */

      UpdateForDisconnectedConversation (
        (RULE_NetworkAddressPointer) &PartialPntr->standardAddress);
    }
  }
  return ReturnCode;
}



/*****************************************************************************
 * Run the modal dialog box that asks the user if they want to be a server
 * or a client.  The user can cancel.  Sets MESS_ServerMode and makes the
 * DialogBox() call return TRUE when successful, returns FALSE if cancelled.
 */

BOOL CALLBACK ServerModeDialogProc (
HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
  int           ControlID;
  HWND          hwndControl;
  int           NotificationCode;
  char          TempString [2 * MESS_MAX_PROMPT_TEXT_LENGTH + 2];

  if (Message == WM_INITDIALOG)
  {
    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_SERVER_TITLE), sizeof (TempString));
    SetWindowText (hwndDlg, TempString);

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_SERVER_PROMPT), sizeof (TempString));
    SetDlgItemText (hwndDlg, IDC_MESS_PROMPT_TEXT, TempString);

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_SERVER_SERVER_BUTTON), sizeof (TempString));
    SetDlgItemText (hwndDlg, IDC_MESS_HOST, TempString);

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_SERVER_CLIENT_BUTTON), sizeof (TempString));
    SetDlgItemText (hwndDlg, IDC_MESS_JOIN, TempString);

    return TRUE; /* Nonzero to set default focus. */
  }

  if (Message == WM_COMMAND)
  {
    ControlID = GET_WM_COMMAND_ID (wParam, lParam);
    NotificationCode = GET_WM_COMMAND_CMD (wParam, lParam);
    hwndControl = GET_WM_COMMAND_HWND (wParam, lParam);

    switch (ControlID)
    {
    case IDCANCEL: /* Includes the close box */
      EndDialog (hwndDlg, FALSE);
      break;

    case IDC_MESS_HOST:
    case IDC_MESS_JOIN:
      MESS_ServerMode = (ControlID == IDC_MESS_HOST);
      EndDialog (hwndDlg, TRUE);
      break;

    default:
      break;
    }
  }
  return FALSE; /* Allow furthur default message handling by the dialog box. */
}



/*****************************************************************************
 * Ask the user if we should start a new game or join an existing one.  If
 * the command line exists and specifies "-server" or "-client" then don't
 * ask the user.  Returns TRUE if successful, FALSE if the use cancelled.
 */

static BOOL PickClientOrServerMode (wchar_t *CommandLine)
{
  /* If network is already running, don't switch client/server mode. */

  if (MESS_NetworkMode)
    return TRUE;

  /* See if the command line has the client or server keywords. */

  if (wcsstr (CommandLine, L"-server") != NULL)
    MESS_ServerMode = TRUE;
  else if (wcsstr (CommandLine, L"-client") != NULL)
    MESS_ServerMode = FALSE;
  else /* No keywords. */
  {
    /* Ask the user if they want to be the server or a
       client.  Sets MESS_ServerMode. */

    if (!DialogBox (ApplicationInstance, MAKEINTRESOURCE (IDD_MESS_ASK_SERVER),
    MainWindowHandle, ServerModeDialogProc))
      return FALSE; /* User cancelled. */
  }
  return TRUE;
}



/*****************************************************************************
 * Run the modal dialog box that asks the user to choose WinSock and/or
 * DirectPlay, and specifies the TCP Port number.  The user can cancel.
 * Sets TCPPortNumber and makes the DialogBox() call return -1 when it
 * is cancelled, or bit 0 (value 1) for Winsock plus bit 1 (value 2)
 * for DirectPlay.  In client mode you can only pick one of Winsock or
 * DirectPlay.
 */

BOOL CALLBACK SystemChoiceDialogProc (
HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
  int   ControlID;
  HWND  hwndControl;
  int   NotificationCode;
  int   ReturnCode;
  char  TempString [2 * MESS_MAX_PROMPT_TEXT_LENGTH + 2];

  if (Message == WM_INITDIALOG)
  {
    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_NETSYS_TITLE), sizeof (TempString));
    SetWindowText (hwndDlg, TempString);

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_NETSYS_PROMPT), sizeof (TempString));
    SetDlgItemText (hwndDlg, IDC_MESS_PROMPT_TEXT, TempString);

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_NETSYS_WINSOCK), sizeof (TempString));
    SetDlgItemText (hwndDlg, IDC_MESS_SYSTEM_WINSOCK, TempString);

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_NETSYS_PORT), sizeof (TempString));
    SetDlgItemText (hwndDlg, IDC_MESS_SYSTEM_PORT_TITLE, TempString);

    SetDlgItemInt (hwndDlg, IDC_MESS_SYSTEM_TCPPORT, TCPPortNumber, FALSE);

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BOX_NETSYS_DIRECTPLAY), sizeof (TempString));
    SetDlgItemText (hwndDlg, IDC_MESS_SYSTEM_DIRECTPLAY, TempString);

    wcstombs (TempString, LANG_GetTextMessageClean (T_MESS_BUTTON_OK), sizeof (TempString));
    SetDlgItemText (hwndDlg, IDOK, TempString);

    CheckDlgButton (hwndDlg, IDC_MESS_SYSTEM_DIRECTPLAY, BST_CHECKED);

    return TRUE; /* Nonzero to set default focus. */
  }

  if (Message == WM_COMMAND)
  {
    ControlID = GET_WM_COMMAND_ID (wParam, lParam);
    NotificationCode = GET_WM_COMMAND_CMD (wParam, lParam);
    hwndControl = GET_WM_COMMAND_HWND (wParam, lParam);

    switch (ControlID)
    {
    case IDC_MESS_SYSTEM_DIRECTPLAY:
      /* Uncheck the other system if we are in client mode. */
      if (!MESS_ServerMode &&
      IsDlgButtonChecked (hwndDlg, IDC_MESS_SYSTEM_DIRECTPLAY) == BST_CHECKED)
        CheckDlgButton (hwndDlg, IDC_MESS_SYSTEM_WINSOCK, BST_UNCHECKED);
      break;

    case IDC_MESS_SYSTEM_WINSOCK:
      /* Uncheck the other system if we are in client mode. */
      if (!MESS_ServerMode &&
      IsDlgButtonChecked (hwndDlg, IDC_MESS_SYSTEM_WINSOCK) == BST_CHECKED)
        CheckDlgButton (hwndDlg, IDC_MESS_SYSTEM_DIRECTPLAY, BST_UNCHECKED);
      break;

    case IDCANCEL: /* Includes the close box */
      EndDialog (hwndDlg, -1 /* Return code for cancel */);
      break;

    case IDOK:
      /* Suck the settings out of the check boxes. */
      TCPPortNumber = GetDlgItemInt (hwndDlg, IDC_MESS_SYSTEM_TCPPORT, NULL, FALSE);
      ReturnCode = 0;
      if (IsDlgButtonChecked (hwndDlg, IDC_MESS_SYSTEM_WINSOCK) == BST_CHECKED)
        ReturnCode += 1;
      if (IsDlgButtonChecked (hwndDlg, IDC_MESS_SYSTEM_DIRECTPLAY) == BST_CHECKED)
        ReturnCode += 2;
      EndDialog (hwndDlg, ReturnCode);
      break;

    default:
      break;
    }
  }
  return FALSE; /* Allow furthur default message handling by the dialog box. */
}



/*****************************************************************************
 * If the command line contains "-netchoose", ask the user to choose the
 * network systems to use.  Then append them to the command line.
 */

static BOOL AddManualNetSystemChoiceToCommandLine (wchar_t *UnicodeCommandLine)
{
  int ReturnCode;

  /* See if the command line has the magic keyword. */

  if (wcsstr (UnicodeCommandLine, L"-netchoose") != NULL)
  {
    /* Ask the user for their choices of network. */

    ReturnCode = DialogBox (ApplicationInstance,
      MAKEINTRESOURCE (IDD_MESS_CHOOSE_SYSTEM), MainWindowHandle,
      SystemChoiceDialogProc);

    if (ReturnCode == -1)
      return FALSE; /* User cancelled. */

    if (ReturnCode & 1) /* Winsock choosen? */
    {
      wcscat (UnicodeCommandLine, L" ");
      wcscat (UnicodeCommandLine, L"-winsock");
    }

    if (ReturnCode & 2) /* DirectPlay choosen? */
    {
      wcscat (UnicodeCommandLine, L" ");
      wcscat (UnicodeCommandLine, L"-directplay");
    }
  }
  return TRUE;
}



/*****************************************************************************
 * Extracts the player name from the command line.  Returns TRUE if it finds
 * one.
 */

static BOOL ExtractPlayerName (wchar_t *UnicodeCommandLine)
{
  wchar_t  *KeywordPlace;
  int       Length;

  /* See if the command line has the magic keyword. */

  KeywordPlace = wcsstr (UnicodeCommandLine, L"-playername");

  if (KeywordPlace == NULL)
    return FALSE;

  /* Skip past the keyword. */

  KeywordPlace += wcslen (L"-playername");

  /* Skip leading spaces. */

  while (*KeywordPlace == L' ')
    KeywordPlace++;

  /* Copy text until a control character or dash is encountered. */

  Length = 0;
  while (*KeywordPlace >= 32 && *KeywordPlace != L'-')
  {
    MESS_PlayerNameFromNetwork [Length] = *KeywordPlace++;
    Length++;
    if (Length >= RULE_MAX_PLAYER_NAME_LENGTH)
      break;
  }

  /* Remove trailing spaces and control characters. */

  Length--; /* Point at last character in string. */
  while (Length >= 0 &&
  (MESS_PlayerNameFromNetwork [Length] == L' ' ||
  MESS_PlayerNameFromNetwork [Length] < 32))
  {
    Length--;
  }

  Length++;
  MESS_PlayerNameFromNetwork [Length] = 0; /* End of string. */

  return TRUE;
}



/*****************************************************************************
 * Updates the status flags to show the current networking state.  Also,
 * if we turned off networking and we weren't the server, reinitialise
 * the rules engine.
 */

static void UpdateNetworkingStatusFlags (void)
{
  MESS_NetworkMode = FALSE;

  if (MESS_DirectPlaySystemActive)
    MESS_NetworkMode = TRUE;

  if (MESS_WinSockSystemActive)
    MESS_NetworkMode = TRUE;

  if (MESS_NetworkMode)
  {
    /* Ask the server to resend the entire game state, useful if joining an
       existing game.  Harmless if the server does it to itself.
       MESS_ServerMode was set in the code that makes the connection.

       Nope, don't do it here.  The Rules engine now detects new players
       and sends a resync when it lets them into the game.  Each resync takes
       about two minutes on a 16M machine.

    MESS_SendAction (ACTION_RESYNC_CLIENT, RULE_SPECTATOR_PLAYER, RULE_BANK_PLAYER,
      RULE_SPECTATOR_PLAYER, 0, 0, 0,
      NULL, 0, NULL);
    */
  }
  else /* Running a local game. */
  {
    if (!MESS_ServerMode)
    {
      /* Turning on server mode, need to activate rules engine.
         Initialising it will start a new game. */

      MESS_ServerMode = TRUE;
      RULE_InitializeSystem ();
    }
  }
}



/*****************************************************************************
 * Turn on networking.  The game can run with networking turned off, for
 * local games.  Messages sent are sent both to the queue and out to the
 * network.  Messages received from the network are put in the queue.
 *
 * If the command line is empty then a check for a DirectLobby started game
 * is done.
 *
 * Otherwise, the command line specifies what to do.  If it contains the
 * keyword "-server" then it starts up in server mode (this computer runs
 * the game and waits for other computers to connect to it).  If it
 * contains the keyword "-client", it starts up in client mode and attempts
 * to connect to some other computer.  If you don't specify, a dialog box
 * pops up to ask the user (using international text).
 *
 * A "-directplay" keyword specifies that DirectPlay will be used, a
 * "-socket" specifies that the standard sockets API will be used (which is
 * Mac and other computer compatible).  The server can use both at once.
 *
 * The address of the server computer is specified by a "-address" keyword
 * and following value (delimited by the next keyword).  If "-address" is
 * specified for the server, it will be used for the TCP/IP port number and
 * maybe to find the particular network interface if there are several.
 * TCP/IP addresses are formatted as four period separated numbers followed
 * by a colon and the port number (use over 1024 to avoid standard ports).
 * The address may also be a DNS computer name followed by a colon and the
 * port number.
 *
 * If the command line doesn't specify server or client, the code asks the
 * user in an international dialog box.  Similarly, it asks the user for a
 * server address if one isn't specified in client mode.
 */

BOOL MESS_StartNetworking (ACHAR *CommandLine)
{
  wchar_t UnicodeCommandLine [MESS_MAX_COMMAND_LINE_LENGTH+1];

  /* Convert the command line into UniCode. */

  if (CommandLine == NULL)
    UnicodeCommandLine [0] = 0; /* Make it an empty string. */
  else
  {
#if FORHOTSEAT
    mbstowcs (UnicodeCommandLine, CommandLine, MESS_MAX_COMMAND_LINE_LENGTH);
#else
    Astrncpy (UnicodeCommandLine, CommandLine, MESS_MAX_COMMAND_LINE_LENGTH);
#endif
    UnicodeCommandLine [MESS_MAX_COMMAND_LINE_LENGTH] = 0;
  }

  /* Check for Lobby startup. */

  MESS_LobbyStartup ();

  /* If no lobby, then look at the command line for networking startup
     arguments if there is a command line. */

  if (!MESS_GameStartedByLobby &&
  UnicodeCommandLine [0] != 0 &&
  (wcsstr (UnicodeCommandLine, L"-directplay") != NULL ||
  wcsstr (UnicodeCommandLine, L"-winsock") != NULL ||
  wcsstr (UnicodeCommandLine, L"-netchoose") != NULL))
  {
    ExtractPlayerName (UnicodeCommandLine);

    if (PickClientOrServerMode (UnicodeCommandLine))
    {
      if (AddManualNetSystemChoiceToCommandLine (UnicodeCommandLine))
      {
        MESS_DirectPlayManualStartup (UnicodeCommandLine);
        MESS_SocketStartup (UnicodeCommandLine);
      }
    }
  }

  UpdateNetworkingStatusFlags ();
  return MESS_NetworkMode;
}



/*****************************************************************************
 * Sets the name of the main player that this computer uses in network games.
 * Since the program only runs one DirectPlay player for the network
 * connection, you can only set one name (for compatability if we need to
 * use Winsock instead of DirectPlay - we only use one network entity per
 * computer).  The name is only used for display to outside spectators
 * examining a DirectLobby game.
 */

BOOL MESS_SetNetworkName (wchar_t *PlayerName)
{
  HRESULT ErrorCode;
  DPNAME  NameRecord;

  memset (&NameRecord, 0, sizeof (NameRecord));
  NameRecord.dwSize = sizeof (NameRecord);
  NameRecord.lpszShortName = PlayerName;

  if (pDPlayN != NULL)
    ErrorCode = pDPlayN->SetPlayerName (MyDPPlayerID, &NameRecord, DPSET_GUARANTEED);
  else
    ErrorCode = DPERR_GENERIC;

  if (FAILED (ErrorCode) && MESS_DirectPlaySystemActive)
  {
    DBUG_DisplayInternationalMessage (T_MESS_ERROR_DPNAMESET, ErrorCode, 0, 0, PlayerName);
    return FALSE;
  }

  wcsncpy (MESS_PlayerNameFromNetwork, PlayerName, RULE_MAX_PLAYER_NAME_LENGTH);
  MESS_PlayerNameFromNetwork [RULE_MAX_PLAYER_NAME_LENGTH] = 0;

  return TRUE;
}



/*****************************************************************************
 * Mark the given address as being the one to use when sending messages
 * to that player.
 */

void MESS_AssociatePlayerWithAddress (RULE_PlayerNumber PlayerNo,
                                      RULE_NetworkAddressPointer AddressOfPlayer)
{
  if (PlayerNo <= RULE_BANK_PLAYER)
    AddressesForPlayers [PlayerNo] = *AddressOfPlayer;
}



/*****************************************************************************
 * Given a player, find his network address.
 */

void MESS_GetAddressOfPlayer (RULE_PlayerNumber PlayerNo,
                              RULE_NetworkAddressPointer AddressToFillIn)
{
  if (PlayerNo <= RULE_BANK_PLAYER)
    *AddressToFillIn = AddressesForPlayers [PlayerNo];
  else /* Set to zero - a local address. */
    memset (AddressToFillIn, 0, sizeof (RULE_NetworkAddressRecord));
}



/*****************************************************************************
 * Given an address, find the player.  Returns RULE_NOBODY_PLAYER if it fails.
 */

RULE_PlayerNumber MESS_GetPlayerForAddress (
RULE_NetworkAddressPointer AddressOfPlayer)
{
  RULE_PlayerNumber PlayerNo;

  for (PlayerNo = 0; PlayerNo <= RULE_BANK_PLAYER; PlayerNo++)
  {
    if (memcmp (AddressesForPlayers + PlayerNo, AddressOfPlayer,
    sizeof (RULE_NetworkAddressRecord)) == 0)
      return PlayerNo;
  }
  return RULE_NOBODY_PLAYER;
}



/*****************************************************************************
 * Find the n'th known computer.  The zeroth one is the local computer.  The
 * other ones are the ones currently connected (just the server if this is a
 * client, otherwise all the clients if this is a server).  If a new one
 * connects or one disconnects, or communications are enabled with a newly
 * connected system, then the index numbers all change.  Returns TRUE if
 * there is a computer at that index, FALSE if there isn't (usually because
 * you have gone past the number of connected and enabled computers).
 * Also returns FALSE if AddressToFillIn is NULL.
 */

BOOL MESS_GetComputerAddressFromIndex (int ComputerIndex,
RULE_NetworkAddressPointer AddressToFillIn)
{
  int                     i;
  int                     MatchingIndex;
  ComputerTrackingPointer TempComputerPntr;

  if (!MESS_NetworkMode || AddressToFillIn == NULL ||
  ComputerIndex < 0 || ComputerIndex > MAX_CONNECTED_COMPUTERS)
    return FALSE;

  /* Index zero is always the local computer's address. */

  if (ComputerIndex == 0)
  {
    memset (AddressToFillIn, 0, sizeof (RULE_NetworkAddressRecord));
    AddressToFillIn->networkSystem = NS_LOCAL;
    return TRUE;
  }

  /* Spin through the known connected computers. */

  for (MatchingIndex = 0, i = 0; i < MAX_CONNECTED_COMPUTERS; i++)
  {
    TempComputerPntr = ComputerList + i;

    if (TempComputerPntr->computerID.networkSystem == NS_LOCAL)
      continue; /* This is an empty entry. */

    if (!TempComputerPntr->communicationsAllowed)
      continue; /* Ignore ones we aren't allowed to talk to. */

    MatchingIndex++; /* The first found computer is number 1, then 2, etc. */

    if (MatchingIndex == ComputerIndex)
    {
      memcpy (AddressToFillIn, &TempComputerPntr->computerID,
      sizeof (RULE_NetworkAddressRecord));

      return TRUE;
    }
  }
  return FALSE; /* Didn't find it. */
}



/*****************************************************************************
 * Turns off DirectPlay networking and update various things to reflect it.
 */

void MESS_StopDirectPlay (void)
{
  TurnOffDirectPlay ();
  UpdateNetworkingStatusFlags ();
}



/*****************************************************************************
 * Turns off WinSock networking and update various things to reflect it.
 */

void MESS_StopWinSock (void)
{
  TurnOffWinSock ();
  UpdateNetworkingStatusFlags ();
}



/*****************************************************************************
 * Turns off the networking system, making this a local game again.
 * Disconnect messages are queued for all known players on the network.
 */

void MESS_StopAllNetworking (void)
{
  TurnOffDirectPlay ();
  TurnOffWinSock ();
  memset (&AddressesForPlayers, 0, sizeof (AddressesForPlayers));
  UpdateNetworkingStatusFlags ();
}



/*****************************************************************************
 * Transmits the message at the tail of MessageQueue over the network, if it
 * needs transmitting.  Does incremental transmits of large messages too.
 * Returns TRUE if it is still being sent, FALSE if it is OK to remove the
 * tail message from the queue.  If the tail message doesn't need to be
 * sent, it will move towards the head of the queue until it finds a message
 * which needs to be sent (will then return FALSE since it is safe to remove
 * the tail message because it has been already sent long ago).
 */

static NetMessageWithSeparatorPointer MessageBeingSentPntr;
  /* Points to the buffer currently being sent.  May point to
  StandardMessageBeingSent (for short messages) or point to dynamically
  allocated storage, or be NULL. */

static NetMessageWithSeparatorRecord StandardMessageBeingSent;
  /* Storage space for the normal kind of short message, used to avoid doing
  dynamic allocations for small messages. */

static BOOL SendTailOrLaterMessageOverNetwork (void)
{
  DWORD                           BlobSize;
  BYTE                           *BytePntr;
  DPCHAT                          DPChat;
  DWORD                          *DWordPntr;
  BYTE                           *FirstBytePntr;
  int                             i;
  int                             Index;
  DWORD                           MaxSize;
  DWORD                           MessageLength;
  int                             MessageQueueIndex;
  DWORD                           StringSize;
  RULE_ActionArgumentsPointer     TailMessagePntr;
  ComputerTrackingPointer         TempComputerPntr;
  RULE_NetworkAddressRecord       ToAddress;

  if (!MESS_NetworkMode)
    return FALSE; // Network is turned off, no need to delay while transmitting.

  // Find the first message which needs transmitting (may already be in
  // progress).  Start at the tail and go towards the head.

  MessageQueueIndex = MessageQueue.tail;

  while (MessageQueueIndex >= 0)
  {
    if (MessageQueue.transmitState[MessageQueueIndex] != NO_TRANSMISSION_NEEDED)
      break; // Found a message which needs to be transmitted.

    if (MessageQueueIndex == MessageQueue.head)
      MessageQueueIndex = -1; // Didn't find any messages needing transmission.
    else
    {
      MessageQueueIndex++;
      if (MessageQueueIndex >= MAX_MESSAGES_IN_QUEUE)
        MessageQueueIndex = 0;
    }
  }

  if (MessageQueueIndex < 0)
    return FALSE; // No messages needing transmission.

  if (MessageQueue.transmitState[MessageQueueIndex] == TRANSMISSION_NEEDED)
  {
    /* This is the first time for this message.  Reformat the message into
       network transmission format. */

    TailMessagePntr = MessageQueue.messages + MessageQueueIndex;

    /* Find the length of the string argument.  Since it may be a binary object,
       search from the end for the last non-zero byte.  Trailing zeros will be
       recreated when the message is received. */

    FirstBytePntr = (unsigned char *) &TailMessagePntr->stringA;

    BytePntr = FirstBytePntr + sizeof (TailMessagePntr->stringA);
    while (--BytePntr >= FirstBytePntr)
    {
      if (*BytePntr != 0)
        break;
    }
    StringSize = (BytePntr - FirstBytePntr) + 1;

    if (TailMessagePntr->binaryDataHandleA == NULL)
      BlobSize = 0;
    else
    {
      /* Lock down the blob to get its address, if it is there and needs it. */

      if (TailMessagePntr->binaryDataA == NULL)
      {
        TailMessagePntr->binaryDataA = (unsigned char *) RULE_LockHandle (TailMessagePntr->binaryDataHandleA);
        if (TailMessagePntr->binaryDataA != NULL)
          TailMessagePntr->binaryDataA += TailMessagePntr->binaryDataStartOffsetA;
      }

      if (TailMessagePntr->binaryDataA == NULL)
        BlobSize = 0;
      else
      {
        /* Verify the user provided blob size.  Note that negative offsets
           show up as large numbers so check them too. */

        MaxSize = RULE_HandleSize (TailMessagePntr->binaryDataHandleA);

        if (TailMessagePntr->binaryDataSizeA > MaxSize ||
        TailMessagePntr->binaryDataStartOffsetA > MaxSize ||
        TailMessagePntr->binaryDataSizeA + TailMessagePntr->binaryDataStartOffsetA > MaxSize)
        {
          /* User's size is bigger than memory chunk size, or offset is wrong. */
#if _DEBUG
          DBUG_DisplayNonFatalErrorMessage ("SendTailOrLaterMessageOverNetwork: bad blob size or offset.");
#endif
          BlobSize = 0;
        }
        else /* Given size of memory space actually exists. */
          BlobSize = TailMessagePntr->binaryDataSizeA;
      }
    }

    /* Make sure the message buffer is deallocated, could be left allocated
       if the network shuts down unexpectedly. */

    if (MessageBeingSentPntr != NULL && MessageBeingSentPntr != &StandardMessageBeingSent)
      GlobalFreePtr (MessageBeingSentPntr);

    /* If there is extra data, can't use the standard message record we have
       on the stack, need to allocate a larger one. */

    if (StringSize > 0 || BlobSize > 0)
    {
      MessageLength = sizeof (StandardMessageBeingSent) +
        StringSize + BlobSize;

      MessageBeingSentPntr = (NetMessageWithSeparatorPointer) GlobalAllocPtr (GHND, MessageLength);
    }
    else /* Fits in standard message. */
    {
      MessageLength = sizeof (StandardMessageBeingSent);
      MessageBeingSentPntr = &StandardMessageBeingSent;
    }

    if (MessageBeingSentPntr == NULL)
      return FALSE; /* Couldn't allocate memory for the message buffer. */

    /* Fill in the header that the TCP/IP and other streaming protocols
       use to identify the start of a message. */

    memcpy (MessageBeingSentPntr->messageSeparatorHeader, MonopolyMessageHeaderMarker,
      sizeof (MessageBeingSentPntr->messageSeparatorHeader));
    MessageBeingSentPntr->messageSeparatorSize = MessageLength - MONOPOLY_MESSAGE_HEADER_LENGTH;

    /* Fill in the standard message fields. */

    MessageBeingSentPntr->monopolyNetMessage.action = TailMessagePntr->action;
    MessageBeingSentPntr->monopolyNetMessage.fromPlayer = TailMessagePntr->fromPlayer;
    MessageBeingSentPntr->monopolyNetMessage.toPlayer = TailMessagePntr->toPlayer;
    MessageBeingSentPntr->monopolyNetMessage.numberA = TailMessagePntr->numberA;
    MessageBeingSentPntr->monopolyNetMessage.numberB = TailMessagePntr->numberB;
    MessageBeingSentPntr->monopolyNetMessage.numberC = TailMessagePntr->numberC;
    MessageBeingSentPntr->monopolyNetMessage.numberD = TailMessagePntr->numberD;
    MessageBeingSentPntr->monopolyNetMessage.numberE = TailMessagePntr->numberE;

    /* Do the optional string / short binary object. */

    MessageBeingSentPntr->monopolyNetMessage.stringALength = StringSize;
    BytePntr = (unsigned char *) &MessageBeingSentPntr->monopolyNetMessage.stringALength;
    BytePntr += sizeof (DWORD);
    if (StringSize > 0)
    {
      memcpy (BytePntr, TailMessagePntr->stringA, StringSize);
      BytePntr += StringSize;
    }

    /* Do the optional large binary object. */

    DWordPntr = (unsigned long *) BytePntr;
    *DWordPntr = BlobSize;
    BytePntr += sizeof (DWORD);
    if (BlobSize > 0)
    {
      memcpy (BytePntr, TailMessagePntr->binaryDataA, BlobSize);
      BytePntr += BlobSize;
    }

#if _DEBUG
    if ((DWORD) (BytePntr - (BYTE *) MessageBeingSentPntr) != MessageLength)
      DBUG_DisplayNonFatalErrorMessage ("SendTailOrLaterMessageOverNetwork: Bad message length calculation.");
#endif

    /* Send the message over the various networks. */

    if (TailMessagePntr->toPlayer == RULE_ALL_PLAYERS)
    {
      /* Broadcast to all players on all networks. */

      if (NewComputersJoinedRecently)
      {
        /* Oops, can only broadcast to computers we are talking to, not to the
           ones we are ignoring.  Send individual messages. */

        for (i = 0; i < MAX_CONNECTED_COMPUTERS; i++)
        {
          TempComputerPntr = ComputerList + i;
          if (TempComputerPntr->computerID.networkSystem != NS_LOCAL &&
          TempComputerPntr->communicationsAllowed)
          {
            if (TempComputerPntr->computerID.networkSystem == NS_DIRECTPLAY)
              StartSendingBufferViaDirectPlay (
              (BYTE *) MessageBeingSentPntr + MONOPOLY_MESSAGE_HEADER_LENGTH,
              MessageLength - MONOPOLY_MESSAGE_HEADER_LENGTH,
              ((NetAddressForDirectPlayPointer) &TempComputerPntr->computerID)->playerID);
            else if (TempComputerPntr->computerID.networkSystem == NS_WINSOCK)
              StartSendingBufferViaWinSock ((BYTE *) MessageBeingSentPntr, MessageLength,
              ((NetAddressForWinSockPointer) &TempComputerPntr->computerID)->connectionIndex);
          }
        }
      }
      else /* No new computers, can broadcast normally, efficiently. */
      {
        StartSendingBufferViaDirectPlay (
          ((BYTE *) MessageBeingSentPntr) + MONOPOLY_MESSAGE_HEADER_LENGTH,
          MessageLength - MONOPOLY_MESSAGE_HEADER_LENGTH,
          DPID_ALLPLAYERS);

        for (Index = 0; Index < MAX_WINSOCK_PLAYERS; Index++)
        {
          if (WinSockPartialData[Index].socketID == INVALID_SOCKET)
            continue;

          StartSendingBufferViaWinSock ((BYTE *) MessageBeingSentPntr,
            MessageLength, Index);
        }
      }
    }
    else /* Sending to a single player. */
    {
      MESS_GetAddressOfPlayer (TailMessagePntr->toPlayer, &ToAddress);

      if (ToAddress.networkSystem == NS_DIRECTPLAY)
        StartSendingBufferViaDirectPlay (
        (BYTE *) MessageBeingSentPntr + MONOPOLY_MESSAGE_HEADER_LENGTH,
        MessageLength - MONOPOLY_MESSAGE_HEADER_LENGTH,
        ((NetAddressForDirectPlayPointer) &ToAddress)->playerID);

      if (ToAddress.networkSystem == NS_WINSOCK)
        StartSendingBufferViaWinSock ((BYTE *) MessageBeingSentPntr, MessageLength,
        ((NetAddressForWinSockPointer) &ToAddress)->connectionIndex);
    }

    /* If this is a broadcast chat message, send a copy via the DirectPlay
       system chat message so that lobbies and other watchers can read it too. */

    if (TailMessagePntr->action == NOTIFY_TEXT_CHAT &&
    TailMessagePntr->numberA == RULE_ALL_PLAYERS &&
    BlobSize > 0 && pDPlayN != NULL && MESS_ServerMode)
    {
      memset (&DPChat, 0, sizeof (DPChat));
      DPChat.dwSize = sizeof (DPChat);
      DPChat.lpszMessage = (wchar_t *) TailMessagePntr->binaryDataA;
      pDPlayN->SendChatMessage (
        MyDPPlayerID, DPID_ALLPLAYERS, DPSEND_GUARANTEED, &DPChat);
    }

    /* Unlock the blob so the system has a bit more flexability.  But leave
       the send buffer locked so that it the sending library routines can
       continue to send it in the background. */

    if (TailMessagePntr->binaryDataHandleA != NULL &&
    TailMessagePntr->binaryDataA != NULL)
    {
      RULE_UnlockHandle (TailMessagePntr->binaryDataHandleA);
      TailMessagePntr->binaryDataA = NULL;
    }

    /* Send the first part of the data right now. */

    WinSockSendPartialData ();

    /* Always busy sending first part, tell caller to wait.  This may be
       needed for DirectPlay which needs time to copy the data out of
       the buffer. */

    MessageQueue.transmitState[MessageQueueIndex] = TRANSMISSION_IN_PROGRESS;
    return TRUE;
  }

  if (MessageQueue.transmitState[MessageQueueIndex] == TRANSMISSION_IN_PROGRESS)
  {
    /* Continue sending the data.  Assume DirectPlay has finished by
       now (can't really tell, but it is supposed to do it right away). */

    if (WinSockSendPartialData ())
      return TRUE; /* Still busy sending data. */

    /* Finished sending, deallocate the transmit buffer. */

    if (MessageBeingSentPntr != NULL && MessageBeingSentPntr != &StandardMessageBeingSent)
      GlobalFreePtr (MessageBeingSentPntr);
    MessageBeingSentPntr = NULL;

    MessageQueue.transmitState[MessageQueueIndex] = NO_TRANSMISSION_NEEDED;
    return FALSE; /* Done sending. */
  }

  /* If we get here, it is an unknown state. */

#if _DEBUG
  DBUG_DisplayNonFatalErrorMessage ("SendTailOrLaterMessageOverNetwork: Unknown transmission state.");
#endif
  return FALSE;
}



/*****************************************************************************
 * Gets the next message waiting for us, from any other computer or ourself.
 *
 * The blank message MessageToReceive is filled in, large binary objects will
 * have a handle allocated that should be deallocated when you have finished
 * with the message (see MESS_FreeStorageAssociatedWithMessage).
 *
 * A special ACTION_DISCONNECTED_PLAYER message is received when someone is
 * disconnected.
 */

BOOL MESS_ReceiveActionMessage (RULE_ActionArgumentsPointer MessageToReceive)
{
  BOOL bRet;

  CheckForDirectPlayReceivedData ();
  CheckForWinSockReceivedData ();

  if (MessageQueue.head < 0)
    return FALSE; /* The queue is empty. */

  /* Send the message at the tail, if needed.  Returns TRUE if the
     send is still in progress, so we don't process it until it has
     been completely sent. */

  if (SendTailOrLaterMessageOverNetwork ())
    return FALSE;

  bRet = RemoveFromQueue (&MessageQueue, MessageToReceive);

#if MESS_REPORT_ACTION_MESSAGES
  MESS_updateMessageLog( MessageToReceive, FALSE );
#endif

  return bRet;
}



/*****************************************************************************
 * Gets the next voice chat related message waiting for us, from any other
 * computer or ourself.  Skips over other messages and may have to rearrange
 * the message queue if the message is in the middle.  Useful for paused
 * games where you still want voice chat running while the rest of the game
 * isn't fetching messages.
 *
 * The blank message MessageToReceive is filled in, large binary objects will
 * have a handle allocated that should be deallocated when you have finished
 * with the message (see MESS_FreeStorageAssociatedWithMessage).
 *
 * A special ACTION_DISCONNECTED_PLAYER message is received when someone is
 * disconnected.
 */

BOOL MESS_ReceiveVoiceChatMessageOnly (RULE_ActionArgumentsPointer MessageToReceive)
{
  BOOL                        bRet;
  RULE_ActionArgumentsPointer MessagePntr;
  int                         MessageQueueIndex;

  CheckForDirectPlayReceivedData ();
  CheckForWinSockReceivedData ();

  if (MessageQueue.head < 0)
    return FALSE; /* The queue is empty. */

  /* Send the message at the tail, if needed. */

  SendTailOrLaterMessageOverNetwork ();

  // Look for the next voice chat message in the queue.

  MessageQueueIndex = MessageQueue.tail;

  while (MessageQueueIndex >= 0)
  {
    MessagePntr = MessageQueue.messages + MessageQueueIndex;

    if (MessagePntr->action == ACTION_VOICE_CHAT ||
    MessagePntr->action == NOTIFY_VOICE_CHAT)
      break; // Found the next voice chat message.

    // Advance to the next message in the queue.

    if (MessageQueueIndex == MessageQueue.head)
      MessageQueueIndex = -1; // No more messages.
    else
    {
      MessageQueueIndex++;
      if (MessageQueueIndex >= MAX_MESSAGES_IN_QUEUE)
        MessageQueueIndex = 0;
    }
  }

  if (MessageQueueIndex < 0 ||
  MessageQueue.transmitState[MessageQueueIndex] != NO_TRANSMISSION_NEEDED)
    return FALSE; // No available voice chat messages to retrieve.

  // Extract the voice chat message from the queue.

  if (MessageQueueIndex == MessageQueue.tail)
    // Removing from the tail.
    bRet = RemoveFromQueue (&MessageQueue, MessageToReceive);
  else
  {
    // Remove from the middle or head of the queue, and know queue
    // has at least two items in it (won't be empty after removing).

    memcpy (MessageToReceive, MessagePntr, sizeof (RULE_ActionArgumentsRecord));
    bRet = TRUE;

    /* Get a pointer to the blob, if there is one. */

    if (MessageToReceive->binaryDataHandleA != NULL &&
    MessageToReceive->binaryDataA == NULL)
    {
      MessageToReceive->binaryDataA = (unsigned char *)
        RULE_LockHandle (MessageToReceive->binaryDataHandleA);

      if (MessageToReceive->binaryDataA != NULL)
        MessageToReceive->binaryDataA +=
        MessageToReceive->binaryDataStartOffsetA;
    }

    if (MessageQueueIndex == MessageQueue.head)
    {
      // Removing from the head of the queue.

      MessageQueue.head--;
      if (MessageQueue.head < 0)
        MessageQueue.head = MAX_MESSAGES_IN_QUEUE - 1;
    }
    else // Removing from the middle.
    {
      // Move items from the head down towards the middle,
      // with extra complications for wrap-around.

      if (MessageQueue.head >= MessageQueueIndex)
      {
        memmove (
          MessageQueue.messages + MessageQueueIndex,
          MessageQueue.messages + (MessageQueueIndex + 1),
          (MessageQueue.head - MessageQueueIndex) * sizeof (RULE_ActionArgumentsRecord));

        memmove (
          MessageQueue.transmitState + MessageQueueIndex,
          MessageQueue.transmitState + (MessageQueueIndex + 1),
          (MessageQueue.head - MessageQueueIndex) * sizeof (char));

        MessageQueue.head--;
      }
      else // Wraparound delete in the middle.
      {
        // Move down from end of queue to the removed message.

        if (MessageQueueIndex < MAX_MESSAGES_IN_QUEUE - 1)
        {
          memmove (
            MessageQueue.messages + MessageQueueIndex,
            MessageQueue.messages + (MessageQueueIndex + 1),
            (MAX_MESSAGES_IN_QUEUE - 1 - MessageQueueIndex) * sizeof (RULE_ActionArgumentsRecord));

          memmove (
            MessageQueue.transmitState + MessageQueueIndex,
            MessageQueue.transmitState + (MessageQueueIndex + 1),
            (MAX_MESSAGES_IN_QUEUE - 1 - MessageQueueIndex) * sizeof (char));
        }

        // Move the wrapped around element.

        MessageQueue.messages [MAX_MESSAGES_IN_QUEUE - 1] = MessageQueue.messages [0];
        MessageQueue.transmitState [MAX_MESSAGES_IN_QUEUE - 1] = MessageQueue.transmitState [0];

        // Move the stuff at the lower part of the queue.

        if (MessageQueue.head > 0)
        {
          memmove (
            MessageQueue.messages + 0,
            MessageQueue.messages + 1,
            (MessageQueue.head) * sizeof (RULE_ActionArgumentsRecord));

          memmove (
            MessageQueue.transmitState + 0,
            MessageQueue.transmitState + 1,
            (MessageQueue.head) * sizeof (char));
        }

        // Update the head pointer to be one less.

        MessageQueue.head--;
        if (MessageQueue.head < 0)
          MessageQueue.head = MAX_MESSAGES_IN_QUEUE - 1;
      }
    }
  }

#if MESS_REPORT_ACTION_MESSAGES
  MESS_updateMessageLog( MessageToReceive, FALSE );
#endif

  return bRet;
}



/*****************************************************************************
 * MESS_InitializeSystem
 */

BOOL MESS_InitializeSystem(void)
{
  /* Initialise all the message queue to empty. */
  MessageQueue.head = -1;

  /* Deactivate networking if it was on, and set flags for local mode. */
  TurnOffDirectPlay ();
  TurnOffWinSock ();
  MESS_ServerMode = TRUE;
  MESS_NetworkMode = FALSE;
  memset (&AddressesForPlayers, 0, sizeof (AddressesForPlayers));
  memset (&ComputerList, 0, sizeof (ComputerList));
  NewComputersJoinedRecently = FALSE;

  return TRUE;
}



/*****************************************************************************
 * MESS_CleanAndRemoveSystem
 */

void MESS_CleanAndRemoveSystem(void)
{
  RULE_ActionArgumentsRecord DeadMessage;

  /* Remove all messages from the queues, and deallocate blobs. */

  while (RemoveFromQueue (&MessageQueue, &DeadMessage))
  {
    MESS_FreeStorageAssociatedWithMessage (&DeadMessage);
  }

  /* Close networking and reset stuff. */
  MESS_InitializeSystem ();
}


/*****************************************************************************
 * Update the message log, RuleLog.txt, with the sent or received message.
 *
 * The log is useful in learning the event model and debugging (it closes after
 * each entry to it remains intact through a crash).
 *
 * The phase stack is also logged to this text file
 */
#if MESS_REPORT_ACTION_MESSAGES
void MESS_updateMessageLog( RULE_ActionArgumentsPointer MessageToSend, BOOL isAddingToQueue )
{
#define LOG_MAX_DEPTH_INDENTED 20
    FILE *Stream;
    static char textEnums[DISPLAY_NULL][50], textPlayers[RULE_MAX_PLAYERS + 4][30],
        propertyEnums[SQ_AUCTION_CHANCE_JAIL + 1][50];
    char TempString[500], TempString2[500], directionIndicator[5], spacers[LOG_MAX_DEPTH_INDENTED * 2 + 1];
    wchar_t WString[200];
    static BOOL initialised = FALSE;
    static int indent = 0;
    int itemp, itemp2;


    if( !initialised ) // Do this the hard but flexible way.
    {
        // actions < NOTIFY_ERROR_MESSAGE=80, notifications < DISPLAY_NULL=160.
        memset( textEnums, 0, sizeof( textEnums ) );
        for( itemp = 0; itemp < NOTIFY_ERROR_MESSAGE; itemp++ )
            strcpy(textEnums[itemp], "ACTION_******BAD" );
        for( itemp = NOTIFY_ERROR_MESSAGE; itemp < DISPLAY_NULL; itemp++ )
            strcpy(textEnums[itemp], "NOTIFY_******BAD" );

        // Set up text values for enums.  We don't want them in release compiles (space).
        strcpy(textEnums[ACTION_NULL], "ACTION_NULL" );
        strcpy(textEnums[ACTION_DISCONNECTED_PLAYER], "ACTION_DISCONNECTED_PLAYER" );
        strcpy(textEnums[ACTION_TICK], "ACTION_TICK" );
        strcpy(textEnums[ACTION_RANDOM_SEED], "ACTION_RANDOM_SEED" );
        strcpy(textEnums[ACTION_RESTART_PHASE], "ACTION_RESTART_PHASE" );
        strcpy(textEnums[ACTION_RESYNC_CLIENT], "ACTION_RESYNC_CLIENT" );
        strcpy(textEnums[ACTION_NEW_GAME], "ACTION_NEW_GAME" );
        strcpy(textEnums[ACTION_GET_GAME_STATE_FOR_SAVE], "ACTION_GET_GAME_STATE_FOR_SAVE" );
        strcpy(textEnums[ACTION_SET_GAME_STATE], "ACTION_SET_GAME_STATE" );
        strcpy(textEnums[ACTION_NAME_PLAYER], "ACTION_NAME_PLAYER" );
        strcpy(textEnums[ACTION_ACCEPT_CONFIGURATION], "ACTION_ACCEPT_CONFIGURATION" );
        strcpy(textEnums[ACTION_START_GAME], "ACTION_START_GAME" );
        strcpy(textEnums[ACTION_START_TURN], "ACTION_START_TURN" );
        strcpy(textEnums[ACTION_END_TURN], "ACTION_END_TURN" );
        strcpy(textEnums[ACTION_ROLL_DICE], "ACTION_ROLL_DICE" );
        strcpy(textEnums[ACTION_CHEAT_ROLL_DICE], "ACTION_CHEAT_ROLL_DICE" );
        strcpy(textEnums[ACTION_CHEAT_CASH], "ACTION_CHEAT_CASH" );
        strcpy(textEnums[ACTION_CHEAT_OWNER], "ACTION_CHEAT_OWNER" );
        strcpy(textEnums[ACTION_KILL_AUCTION_CHEAT], "ACTION_KILL_AUCTION_CHEAT" );
        strcpy(textEnums[ACTION_MOVE_FORWARDS], "ACTION_MOVE_FORWARDS" );
        strcpy(textEnums[ACTION_MOVE_BACKWARDS], "ACTION_MOVE_BACKWARDS" );
        strcpy(textEnums[ACTION_JUMP_TO_SQUARE], "ACTION_JUMP_TO_SQUARE" );
        strcpy(textEnums[ACTION_LANDED_ON_SQUARE], "ACTION_LANDED_ON_SQUARE" );
        strcpy(textEnums[ACTION_EXIT_JAIL_DECISION], "ACTION_EXIT_JAIL_DECISION" );
        strcpy(textEnums[ACTION_CARD_SEEN], "ACTION_CARD_SEEN" );
        strcpy(textEnums[ACTION_GO_BANKRUPT], "ACTION_GO_BANKRUPT" );
        strcpy(textEnums[ACTION_BUY_OR_AUCTION_DECISION], "ACTION_BUY_OR_AUCTION_DECISION" );
        strcpy(textEnums[ACTION_BID], "ACTION_BID" );
        strcpy(textEnums[ACTION_FREE_UNMORTGAGE_DONE], "ACTION_FREE_UNMORTGAGE_DONE" );
        strcpy(textEnums[ACTION_BUY_HOUSE], "ACTION_BUY_HOUSE" );
        strcpy(textEnums[ACTION_SELL_BUILDINGS], "ACTION_SELL_BUILDINGS" );
        strcpy(textEnums[ACTION_MORTGAGING], "ACTION_MORTGAGING" );
        strcpy(textEnums[ACTION_TAX_DECISION], "ACTION_TAX_DECISION" );
        strcpy(textEnums[ACTION_START_TRADE_EDITING], "ACTION_START_TRADE_EDITING" );
        strcpy(textEnums[ACTION_CLEAR_TRADE_ITEMS], "ACTION_CLEAR_TRADE_ITEMS" );
        strcpy(textEnums[ACTION_TRADE_ITEM], "ACTION_TRADE_ITEM" );
        strcpy(textEnums[ACTION_TRADE_EDITING_DONE], "ACTION_TRADE_EDITING_DONE" );
        strcpy(textEnums[ACTION_TRADE_ACCEPT], "ACTION_TRADE_ACCEPT" );
        strcpy(textEnums[ACTION_VOICE_CHAT], "ACTION_VOICE_CHAT" );
        strcpy(textEnums[ACTION_TEXT_CHAT], "ACTION_TEXT_CHAT" );
        strcpy(textEnums[ACTION_GET_OPTIONS_FOR_SAVE], "ACTION_GET_OPTIONS_FOR_SAVE" );
        strcpy(textEnums[ACTION_AI_SAVE_PARAMETERS], "ACTION_AI_SAVE_PARAMETERS" );
        strcpy(textEnums[ACTION_PLAYER_BUYSELLMORT], "ACTION_PLAYER_BUYSELLMORT" );
        strcpy(textEnums[ACTION_PLAYER_DONE_BUYSELLMORT], "ACTION_PLAYER_DONE_BUYSELLMORT" );
        strcpy(textEnums[ACTION_CANCEL_DECOMPOSITION], "ACTION_CANCEL_DECOMPOSITION" );
        strcpy(textEnums[ACTION_START_HOUSING_AUCTION], "ACTION_START_HOUSING_AUCTION" );
        strcpy(textEnums[ACTION_STAR_WARS_ANIMATION_INFO], "ACTION_STAR_WARS_ANIMATION_INFO" );
        strcpy(textEnums[ACTION_UPDATE_TRADE_INFO], "ACTION_UPDATE_TRADE_INFO" );
        strcpy(textEnums[ACTION_PAUSE_GAME], "ACTION_PAUSE_GAME" );
        strcpy(textEnums[ACTION_I_AM_HERE], "ACTION_I_AM_HERE" );
        strcpy(textEnums[ACTION_CLEAR_TRADED_IMMUNITIES_OR_FUTURES], "ACTION_CLEAR_TRADED_IMMUNITIES_OR_FUTURES" );

        strcpy(textEnums[NOTIFY_ERROR_MESSAGE ], "NOTIFY_ERROR_MESSAGE" );
        strcpy(textEnums[NOTIFY_CLIENT_RESYNC_INFO], "NOTIFY_CLIENT_RESYNC_INFO" );
        strcpy(textEnums[NOTIFY_NUMBER_OF_PLAYERS], "NOTIFY_NUMBER_OF_PLAYERS" );
        strcpy(textEnums[NOTIFY_NAME_PLAYER], "NOTIFY_NAME_PLAYER" );
        strcpy(textEnums[NOTIFY_ADD_LOCAL_PLAYER], "NOTIFY_ADD_LOCAL_PLAYER" );
        strcpy(textEnums[NOTIFY_PROPOSED_CONFIGURATION], "NOTIFY_PROPOSED_CONFIGURATION" );
        strcpy(textEnums[NOTIFY_GAME_STATE_FOR_SAVE], "NOTIFY_GAME_STATE_FOR_SAVE" );
        strcpy(textEnums[NOTIFY_END_TURN], "NOTIFY_END_TURN" );
        strcpy(textEnums[NOTIFY_START_TURN], "NOTIFY_START_TURN" );
        strcpy(textEnums[NOTIFY_JAIL_EXIT_CHOICE], "NOTIFY_JAIL_EXIT_CHOICE" );
        strcpy(textEnums[NOTIFY_PLEASE_ROLL_DICE], "NOTIFY_PLEASE_ROLL_DICE" );
        strcpy(textEnums[NOTIFY_DICE_ROLLED], "NOTIFY_DICE_ROLLED" );
        strcpy(textEnums[NOTIFY_MOVE_FORWARDS], "NOTIFY_MOVE_FORWARDS" );
        strcpy(textEnums[NOTIFY_MOVE_BACKWARDS], "NOTIFY_MOVE_BACKWARDS" );
        strcpy(textEnums[NOTIFY_JUMP_TO_SQUARE], "NOTIFY_JUMP_TO_SQUARE" );
        strcpy(textEnums[NOTIFY_PASSED_GO], "NOTIFY_PASSED_GO" );
        strcpy(textEnums[NOTIFY_CASH_AMOUNT], "NOTIFY_CASH_AMOUNT" );
        strcpy(textEnums[NOTIFY_CASH_ANIMATION], "NOTIFY_CASH_ANIMATION" );
        strcpy(textEnums[NOTIFY_PLEASE_PAY], "NOTIFY_PLEASE_PAY" );
        strcpy(textEnums[NOTIFY_SQUARE_OWNERSHIP], "NOTIFY_SQUARE_OWNERSHIP" );
        strcpy(textEnums[NOTIFY_SQUARE_MORTGAGE], "NOTIFY_SQUARE_MORTGAGE" );
        strcpy(textEnums[NOTIFY_SQUARE_HOUSES], "NOTIFY_SQUARE_HOUSES" );
        strcpy(textEnums[NOTIFY_JAIL_CARD_OWNERSHIP], "NOTIFY_JAIL_CARD_OWNERSHIP" );
        strcpy(textEnums[NOTIFY_IMMUNITY_COUNT], "NOTIFY_IMMUNITY_COUNT" );
        strcpy(textEnums[NOTIFY_PICKED_UP_CARD], "NOTIFY_PICKED_UP_CARD" );
        strcpy(textEnums[NOTIFY_PUT_AWAY_CARD], "NOTIFY_PUT_AWAY_CARD" );
        strcpy(textEnums[NOTIFY_BUY_OR_AUCTION_DECISION], "NOTIFY_BUY_OR_AUCTION_DECISION" );
        strcpy(textEnums[NOTIFY_NEW_HIGH_BID], "NOTIFY_NEW_HIGH_BID" );
        strcpy(textEnums[NOTIFY_AUCTION_GOING], "NOTIFY_AUCTION_GOING" );
        strcpy(textEnums[NOTIFY_HOUSING_SHORTAGE], "NOTIFY_HOUSING_SHORTAGE" );
        strcpy(textEnums[NOTIFY_FREE_UNMORTGAGING], "NOTIFY_FREE_UNMORTGAGING" );
        strcpy(textEnums[NOTIFY_FLAT_OR_FRACTION_TAX_DECISION], "NOTIFY_FLAT_OR_FRACTION_TAX_DECISION" );
        strcpy(textEnums[NOTIFY_TRADE_STARTED], "NOTIFY_TRADE_STARTED" );
        strcpy(textEnums[NOTIFY_TRADE_FINISHED], "NOTIFY_TRADE_FINISHED" );
        strcpy(textEnums[NOTIFY_TRADE_ITEM], "NOTIFY_TRADE_ITEM" );
        strcpy(textEnums[NOTIFY_TRADE_EDITOR], "NOTIFY_TRADE_EDITOR" );
        strcpy(textEnums[NOTIFY_TRADE_ACCEPTANCE_DECISION], "NOTIFY_TRADE_ACCEPTANCE_DECISION" );
        strcpy(textEnums[NOTIFY_ACTION_COMPLETED], "NOTIFY_ACTION_COMPLETED" );
        strcpy(textEnums[NOTIFY_VOICE_CHAT], "NOTIFY_VOICE_CHAT" );
        strcpy(textEnums[NOTIFY_TEXT_CHAT], "NOTIFY_TEXT_CHAT" );
        strcpy(textEnums[NOTIFY_OPTIONS_FOR_SAVE], "NOTIFY_OPTIONS_FOR_SAVE" );
        strcpy(textEnums[NOTIFY_AI_PARAMETERS], "NOTIFY_AI_PARAMETERS" );
        strcpy(textEnums[NOTIFY_AI_NEED_PARAMETERS_FOR_SAVE], "NOTIFY_AI_NEED_PARAMETERS_FOR_SAVE" );
        strcpy(textEnums[NOTIFY_PLAYER_BUYSELLMORT], "NOTIFY_PLAYER_BUYSELLMORT" );
        strcpy(textEnums[NOTIFY_DECOMPOSE_SALE], "NOTIFY_DECOMPOSE_SALE" );
        strcpy(textEnums[NOTIFY_PLACE_BUILDING], "NOTIFY_PLACE_BUILDING" );
        strcpy(textEnums[NOTIFY_STAR_WARS_ANIMATION_INFO], "NOTIFY_STAR_WARS_ANIMATION_INFO" );
        strcpy(textEnums[NOTIFY_GAME_OVER], "NOTIFY_GAME_OVER" );
        strcpy(textEnums[NOTIFY_GAME_STARTING], "NOTIFY_GAME_STARTING" );
        strcpy(textEnums[NOTIFY_FUTURE_RENT_COUNT], "NOTIFY_FUTURE_RENT_COUNT" );
        strcpy(textEnums[NOTIFY_UPDATE_TRADE_INFO], "NOTIFY_UPDATE_TRADE_INFO" );
        strcpy(textEnums[NOTIFY_NEXT_MOVE], "NOTIFY_NEXT_MOVE" );
        strcpy(textEnums[NOTIFY_PLAYER_DELETED], "NOTIFY_PLAYER_DELETED" );
        strcpy(textEnums[NOTIFY_GAME_PAUSED], "NOTIFY_GAME_PAUSED" );
        strcpy(textEnums[NOTIFY_PLEASE_ADD_PLAYERS], "NOTIFY_PLEASE_ADD_PLAYERS" );
        strcpy(textEnums[NOTIFY_FREE_PARKING_POT], "NOTIFY_FREE_PARKING_POT" );
        strcpy(textEnums[NOTIFY_ARE_YOU_THERE], "NOTIFY_ARE_YOU_THERE" );

        for( itemp = 0; itemp<DISPLAY_NULL; itemp++ )
        {
            sprintf( TempString, " (%d)", itemp );
            strcat ( textEnums[itemp], TempString );
            for( itemp2 = 0; itemp2< 44; itemp2++ )
            {
                if( textEnums[itemp][itemp2] == 0 ) textEnums[itemp][itemp2] = '.';
            }
            textEnums[itemp][44] = 0;
        }

        // Now do player names
        for( itemp = 0; itemp<RULE_MAX_PLAYERS; itemp++ )
            sprintf ( textPlayers[itemp], "Player %d        ", itemp );
        sprintf ( textPlayers[RULE_MAX_PLAYERS  ], "THE BANK        " );
        sprintf ( textPlayers[RULE_MAX_PLAYERS+1], "ALL/NO PLAYER(S)" );
        sprintf ( textPlayers[RULE_MAX_PLAYERS+2], "ESCROW PLAYER   " );
        sprintf ( textPlayers[RULE_MAX_PLAYERS+3], "SPECTATOR       " );

        // Now do property names
        sprintf ( propertyEnums[SQ_GO], "SQ_GO" );
        sprintf ( propertyEnums[SQ_MEDITERRANEAN_AVENUE], "SQ_MEDITERRANEAN_AVENUE" );
        sprintf ( propertyEnums[SQ_COMMUNITY_CHEST_1], "SQ_COMMUNITY_CHEST_1" );
        sprintf ( propertyEnums[SQ_BALTIC_AVENUE], "SQ_BALTIC_AVENUE" );
        sprintf ( propertyEnums[SQ_INCOME_TAX], "SQ_INCOME_TAX" );
        sprintf ( propertyEnums[SQ_READING_RR], "SQ_READING_RR" );
        sprintf ( propertyEnums[SQ_ORIENTAL_AVENUE], "SQ_ORIENTAL_AVENUE" );
        sprintf ( propertyEnums[SQ_CHANCE_1], "SQ_CHANCE_1" );
        sprintf ( propertyEnums[SQ_VERMONT_AVENUE], "SQ_VERMONT_AVENUE" );
        sprintf ( propertyEnums[SQ_CONNECTICUT_AVENUE], "SQ_CONNECTICUT_AVENUE" );
        sprintf ( propertyEnums[SQ_JUST_VISITING], "SQ_JUST_VISITING" );
        sprintf ( propertyEnums[SQ_ST_CHARLES_PLACE], "SQ_ST_CHARLES_PLACE" );
        sprintf ( propertyEnums[SQ_ELECTRIC_COMPANY], "SQ_ELECTRIC_COMPANY" );
        sprintf ( propertyEnums[SQ_STATES_AVENUE], "SQ_STATES_AVENUE" );
        sprintf ( propertyEnums[SQ_VIRGINIA_AVENUE], "SQ_VIRGINIA_AVENUE" );
        sprintf ( propertyEnums[SQ_PENNSYLVANIA_RR], "SQ_PENNSYLVANIA_RR" );
        sprintf ( propertyEnums[SQ_ST_JAMES_PLACE], "SQ_ST_JAMES_PLACE" );
        sprintf ( propertyEnums[SQ_COMMUNITY_CHEST_2], "SQ_COMMUNITY_CHEST_2" );
        sprintf ( propertyEnums[SQ_TENNESSEE_AVENUE], "SQ_TENNESSEE_AVENUE" );
        sprintf ( propertyEnums[SQ_NEW_YORK_AVENUE], "SQ_NEW_YORK_AVENUE" );
        sprintf ( propertyEnums[SQ_FREE_PARKING], "SQ_FREE_PARKING" );
        sprintf ( propertyEnums[SQ_KENTUCKY_AVENUE], "SQ_KENTUCKY_AVENUE" );
        sprintf ( propertyEnums[SQ_CHANCE_2], "SQ_CHANCE_2" );
        sprintf ( propertyEnums[SQ_INDIANA_AVENUE], "SQ_INDIANA_AVENUE" );
        sprintf ( propertyEnums[SQ_ILLINOIS_AVENUE], "SQ_ILLINOIS_AVENUE" );
        sprintf ( propertyEnums[SQ_BnO_RR], "SQ_BnO_RR" );
        sprintf ( propertyEnums[SQ_ATLANTIC_AVENUE], "SQ_ATLANTIC_AVENUE" );
        sprintf ( propertyEnums[SQ_VENTNOR_AVENUE], "SQ_VENTNOR_AVENUE" );
        sprintf ( propertyEnums[SQ_WATER_WORKS], "SQ_WATER_WORKS" );
        sprintf ( propertyEnums[SQ_MARVIN_GARDENS], "SQ_MARVIN_GARDENS" );
        sprintf ( propertyEnums[SQ_GO_TO_JAIL], "SQ_GO_TO_JAIL" );
        sprintf ( propertyEnums[SQ_PACIFIC_AVENUE], "SQ_PACIFIC_AVENUE" );
        sprintf ( propertyEnums[SQ_NORTH_CAROLINA_AVENUE], "SQ_NORTH_CAROLINA_AVENUE" );
        sprintf ( propertyEnums[SQ_COMMUNITY_CHEST_3], "SQ_COMMUNITY_CHEST_3" );
        sprintf ( propertyEnums[SQ_PENNSYLVANIA_AVENUE], "SQ_PENNSYLVANIA_AVENUE" );
        sprintf ( propertyEnums[SQ_SHORT_LINE_RR], "SQ_SHORT_LINE_RR" );
        sprintf ( propertyEnums[SQ_CHANCE_3], "SQ_CHANCE_3" );
        sprintf ( propertyEnums[SQ_PARK_PLACE], "SQ_PARK_PLACE" );
        sprintf ( propertyEnums[SQ_LUXURY_TAX], "SQ_LUXURY_TAX" );
        sprintf ( propertyEnums[SQ_BOARDWALK], "SQ_BOARDWALK" );

        sprintf ( propertyEnums[SQ_IN_JAIL], "SQ_IN_JAIL/SQ_AUCTION_HOUSE" );
        sprintf ( propertyEnums[SQ_OFF_BOARD], "SQ_OFF_BOARD/SQ_AUCTION_HOTEL" );
        sprintf ( propertyEnums[SQ_MAX_SQUARE_TYPES], "SQ_MAX_SQUARE_TYPES/SQ_AUCTION_COMMUNITY_JAIL" );
        sprintf ( propertyEnums[SQ_AUCTION_CHANCE_JAIL], "SQ_AUCTION_CHANCE_JAIL" );

        for( itemp = 0; itemp <= SQ_AUCTION_CHANCE_JAIL; itemp++ )
        {
            sprintf( TempString, " (%d)", itemp );
            strcat ( propertyEnums[itemp], TempString );
        }

        initialised = TRUE;
    }

    memset( spacers, 32, sizeof(spacers) );
    if( isAddingToQueue )
    { // NOTE: indent can go below zero in network games.
      if( indent < 0 )
      {
        indent = 0;
        strcpy( directionIndicator, "0>" );
      }
      else
        strcpy( directionIndicator, "|>" );
      spacers[min(LOG_MAX_DEPTH_INDENTED, indent) * 2] = 0;
      indent++;
    }
    else
    {
      if( indent <= 0 )
      {
        indent = 1; // we are popping, after all
        strcpy( directionIndicator, "0<" );
      } else
        strcpy( directionIndicator, "|<" );
      indent--;
      spacers[min(LOG_MAX_DEPTH_INDENTED, indent)*2] = 0;
    }

    // Line across top
    strcpy (TempString, spacers );
    //strcat (TempString, directionIndicator );
    strcat (TempString, "_________________________________________________________________________________________\n" );

    // Prep the basic data
    sprintf (TempString2,
        "%s%s%s From: %sTo: %s ",
        spacers, directionIndicator,
        textEnums[MessageToSend->action],
        textPlayers[MessageToSend->fromPlayer], textPlayers[MessageToSend->toPlayer] );
    strcat (TempString, TempString2);

    if( isAddingToQueue)
    { // only show details if adding (put a ! in front of criteria to have details on POP instead)
        sprintf (TempString2,
            "\n%s%s[A:%4d] [B:%4d] [C:%4d] [D:%4d] [E:%4d] ", spacers, directionIndicator,
            MessageToSend->numberA, MessageToSend->numberB,
            MessageToSend->numberC, MessageToSend->numberD, MessageToSend->numberE );
        strcat( TempString, TempString2 );

        // Add property (by parameter sets)
        if( MessageToSend->action == ACTION_MOVE_FORWARDS ||
            MessageToSend->action == NOTIFY_SQUARE_OWNERSHIP )
        { // Tack on the property name
            strcat (TempString, propertyEnums[MessageToSend->numberA] );
        }
        if( MessageToSend->action == NOTIFY_BUY_OR_AUCTION_DECISION )
        { // Tack on the property name
            strcat (TempString, "*" );
            strcat (TempString, propertyEnums[MessageToSend->numberB] );
            strcat (TempString, "*" );
        }

        // Make a note if binary data is attached
        if( MessageToSend->binaryDataHandleA != NULL )
            strcat (TempString, " *BLOB attached*");

        // See if the string was used - print it out if it was.
        if( MessageToSend->stringA[0] != 0 )
        {
            wcstombs( TempString2, MessageToSend->stringA, 200 );
            strcat( TempString, "\n" );
            strcat( TempString, spacers );
            strcat( TempString, directionIndicator );
            strcat( TempString, "StringA: " );
            strcat( TempString, TempString2 );
        }

        // Error messages (also just general info messages)
        if( MessageToSend->action == NOTIFY_ERROR_MESSAGE )
        {
            RULE_FormatErrorByMessage (MessageToSend, WString, 200 );
            wcstombs( TempString2, WString, 200 );
            strcat( TempString, "\n" );
            strcat( TempString, spacers );
            strcat( TempString, directionIndicator );
            strcat( TempString, "Message: " );
            strcat( TempString, TempString2 );
        }
   }

    // Add a spacer line
    strcat (TempString, "\n" );
    //strcat (TempString, spacers );
    //strcat (TempString, directionIndicator );
    //strcat (TempString, "  _________________________________________________________________________________________" );
    //strcat (TempString, "\n");

    // Output the string
    Stream = fopen( MESS_REPORT_ACTION_FILENAME, "a+" );
    if (Stream != NULL)
    {
        fwrite( TempString, 1, strlen(TempString), Stream );
        fclose( Stream );
    }
}
#endif


/*****************************************************************************
 * Update the message log, RuleLog.txt, with the poped, pushed or switched phase.
 *
 * The log is useful in learning the event model and debugging (it closes after
 * each entry to it remains intact through a crash).
 *
 * The message stack is also logged to this text file.
 * This function would be more appropriate in Rule.c, but I chose to put it with
 * the message log function (here in Mess.c)
 */

#if MESS_REPORT_ACTION_MESSAGES
void MESS_RULE_updateMessageLog( RULE_PendingPhasePointer PhasePntr, int direction /*0=pop, switch push=2*/ )
{
    FILE *Stream;
    static char textEnums[DISPLAY_NULL][60], textPlayers[RULE_MAX_PLAYERS + 4][30];
    char TempString[500], TempString2[500], directionIndicator[20], spacers[100];
    //wchar_t WString[200];
    static BOOL initialised = FALSE;
    static int indent = 0;
    int itemp, itemp2;


    if( !initialised ) // Do this the hard but flexible way.
    {
        memset( textEnums, 0, sizeof( textEnums ) );
        for( itemp = 0; itemp < GF_MAX; itemp++ )
            strcpy(textEnums[itemp], "GF_******BAD" );

        strcpy(textEnums[GF_ADDING_NEW_PLAYERS], "GF_ADDING_NEW_PLAYERS" );
        strcpy(textEnums[GF_CONFIGURATION], "GF_CONFIGURATION" );
        strcpy(textEnums[GF_PICKING_STARTING_ORDER], "GF_PICKING_STARTING_ORDER" );
        strcpy(textEnums[GF_WAIT_START_TURN], "GF_WAIT_START_TURN" );
        strcpy(textEnums[GF_WAIT_END_TURN], "GF_WAIT_END_TURN" );
        strcpy(textEnums[GF_PREROLL], "GF_PREROLL" );
        strcpy(textEnums[GF_WAIT_MOVE_ROLL], "GF_WAIT_MOVE_ROLL" );
        strcpy(textEnums[GF_MOVING_TOKEN], "GF_MOVING_TOKEN" );
        strcpy(textEnums[GF_WAIT_JAIL_ROLL], "GF_WAIT_JAIL_ROLL" );
        strcpy(textEnums[GF_WAIT_UTILITY_ROLL], "GF_WAIT_UTILITY_ROLL" );
        strcpy(textEnums[GF_WAIT_UNTIL_CARD_SEEN], "GF_WAIT_UNTIL_CARD_SEEN" );
        strcpy(textEnums[GF_JAIL_ROLL_OR_PAY_OR_CARD_DECISION], "GF_JAIL_ROLL_OR_PAY_OR_CARD_DECISION" );
        strcpy(textEnums[GF_GET_OUT_OF_JAIL], "GF_GET_OUT_OF_JAIL" );
        strcpy(textEnums[GF_FLAT_OR_FRACTION_TAX_DECISION], "GF_FLAT_OR_FRACTION_TAX_DECISION" );
        strcpy(textEnums[GF_AUCTION_OR_BUY_DECISION], "GF_AUCTION_OR_BUY_DECISION" );
        strcpy(textEnums[GF_EDITING_TRADE], "GF_EDITING_TRADE" );
        strcpy(textEnums[GF_TRADE_ACCEPTANCE], "GF_TRADE_ACCEPTANCE" );
        strcpy(textEnums[GF_TRADE_FINISHED], "GF_TRADE_FINISHED" );
        strcpy(textEnums[GF_AUCTION], "GF_AUCTION" );
        strcpy(textEnums[GF_HOUSING_SHORTAGE_QUESTION], "GF_HOUSING_SHORTAGE_QUESTION" );
        strcpy(textEnums[GF_COLLECTING_PAYMENT], "GF_COLLECTING_PAYMENT" );
        strcpy(textEnums[GF_TRANSFER_ESCROW_PROPERTY], "GF_TRANSFER_ESCROW_PROPERTY" );
        strcpy(textEnums[GF_FREE_UNMORTGAGE], "GF_FREE_UNMORTGAGE" );
        strcpy(textEnums[GF_GAME_FINISHED], "GF_GAME_FINISHED" );
        strcpy(textEnums[GF_BUYSELLMORT], "GF_BUYSELLMORT" );
        strcpy(textEnums[GF_DECOMPOSE_HOTEL], "GF_DECOMPOSE_HOTEL" );
        strcpy(textEnums[GF_PLACE_BUILDING], "GF_PLACE_BUILDING" );
        strcpy(textEnums[GF_COLLECT_AI_PARAMETERS_FOR_SAVE], "GF_COLLECT_AI_PARAMETERS_FOR_SAVE" );
        strcpy(textEnums[GF_PAUSED], "GF_PAUSED" );
        strcpy(textEnums[GF_WAIT_FOR_EVERYBODY_READY], "GF_WAIT_FOR_EVERYBODY_READY" );

        for( itemp = 0; itemp<GF_MAX; itemp++ )
        {
            sprintf( TempString, " (%d)", itemp );
            strcat ( textEnums[itemp], TempString );
            for( itemp2 = 0; itemp2< 44; itemp2++ )
            {
                if( textEnums[itemp][itemp2] == 0 ) textEnums[itemp][itemp2] = '.';
            }
            textEnums[itemp][44] = 0;
        }

        // Now do player names
        for( itemp = 0; itemp<RULE_MAX_PLAYERS; itemp++ )
            sprintf ( textPlayers[itemp], "Player %d", itemp );
        sprintf ( textPlayers[RULE_MAX_PLAYERS  ], "THE BANK"         );
        sprintf ( textPlayers[RULE_MAX_PLAYERS+1], "ALL/NO PLAYER(S)" );
        sprintf ( textPlayers[RULE_MAX_PLAYERS+2], "ESCROW PLAYER"    );
        sprintf ( textPlayers[RULE_MAX_PLAYERS+3], "SPECTATOR"        );

        initialised = TRUE;
    }

    memset( spacers, ' ', sizeof(spacers) );
    if( indent < 0 ) indent = 0;
    switch( direction )
    {
    case 0: // pop
        strcpy( directionIndicator, "__PHASE_POP___" );
        indent--;
        if( indent < 0 ) indent = 0;
        spacers[min(LOG_MAX_DEPTH_INDENTED, indent)*2] = 0;
        break;
    case 1: // switch
        strcpy( directionIndicator, "__PHASE_SWITCH" );
        spacers[indent*2] = 0;
       break;
    case 2: // push
        if( PhasePntr->phase == GF_ADDING_NEW_PLAYERS )
            indent = 0; // hard reset of phase level on new game, see ActionNewGame.
        strcpy( directionIndicator, "__PHASE_PUSH__" );
        spacers[min(LOG_MAX_DEPTH_INDENTED, indent)*2] = 0;
        indent++;
        break;
    }

    strcpy ( TempString, "\n" );
    strcat ( TempString, spacers );
    strcat ( TempString, directionIndicator );
    strcat ( TempString, "___________________________________________________________________________\n" );
    sprintf (TempString2, "%s| %s From: %s To: %s, Amount: %d\n",
        spacers,
        textEnums[PhasePntr->phase], textPlayers[PhasePntr->fromPlayer], textPlayers[PhasePntr->toPlayer],
        PhasePntr->phase );
    strcat (TempString, TempString2);

    // Output the string
    Stream = fopen( MESS_REPORT_ACTION_FILENAME, "a+" );
    if (Stream != NULL)
    {
        fwrite( TempString, 1, strlen(TempString), Stream );
        fclose( Stream );
    }
}
#endif

/*****************************************************************************
 * Update the message log, RuleLog.txt, with a note STATE RESTORED
 */
#if MESS_REPORT_ACTION_MESSAGES
void MESS_STATE_updateMessageLog( RULE_ResyncCauses ResyncCause )
{
#define LOG_MAX_DEPTH_INDENTED 20
    FILE *Stream;
    static char resyncEnums[MAX_RESYNC_CAUSES][50];
    char TempString[500], TempString2[500];
    static BOOL initialised = FALSE;
    int itemp;


    if( !initialised ) // Do this the hard but flexible way.
    {
        memset( resyncEnums, 0, sizeof( resyncEnums ) );
        for( itemp = 0; itemp < MAX_RESYNC_CAUSES; itemp++ )
            strcpy(resyncEnums[itemp], "RESYNC_******BAD" );

        // Set up text values for enums.
        strcpy(resyncEnums[RESYNC_NET_REFRESH], "RESYNC_NET_REFRESH" );
        strcpy(resyncEnums[RESYNC_GAME_START], "RESYNC_GAME_START" );
        strcpy(resyncEnums[RESYNC_LOAD_GAME], "RESYNC_LOAD_GAME" );
        strcpy(resyncEnums[RESYNC_UNDO_BANKRUPTCY], "RESYNC_UNDO_BANKRUPTCY" );
        strcpy(resyncEnums[RESYNC_UNDO_HOTEL_DECOMPOSITION], "RESYNC_UNDO_HOTEL_DECOMPOSITION" );

        initialised = TRUE;
    }


    // Line across top
    strcpy (TempString, "**_______________________________________________\n" );

    // Prep the basic data
    sprintf (TempString2,
        "State restore about to be issued by reason of %s",
        resyncEnums[ResyncCause] );
    strcat (TempString, TempString2);


    // Add a spacer line
    strcat (TempString, "\n" );


    // Output the string
    Stream = fopen( MESS_REPORT_ACTION_FILENAME, "a+" );
    if (Stream != NULL)
    {
        fwrite( TempString, 1, strlen(TempString), Stream );
        fclose( Stream );
    }
}
#endif


