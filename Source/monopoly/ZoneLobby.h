/***************************************************************************
 *
 *  Copyright (c) 1994-1998 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:    ZoneLobby.h
 *  Content: Internet Gaming Zone supplement to dplobby.h include file.
 *
 *  WARNING: This header file is under development and subject to change.
 *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//
// ZONEPROPERTY_LobbyGuid3
//
// Identifying GUID for IGZ Lobby v3.0.  See DPLPROPERTY_LobbyGuid.
//
// {BDD4B95C-D35C-11d0-B625-00C04FC33EA1}
DEFINE_GUID(ZONEPROPERTY_LobbyGuid3, 
0xbdd4b95c, 0xd35c, 0x11d0, 0xb6, 0x25, 0x0, 0xc0, 0x4f, 0xc3, 0x3e, 0xa1);


//
// ZONEPROPERTY_LobbyOptions
//
// Used to set lobby options for the game session.
//
// Property data is a single DWORD.
//
// {33B64CA7-D8EB-11d0-B62B-00C04FC33EA1}
DEFINE_GUID(ZONEPROPERTY_GameOptions, 
0x33b64ca7, 0xd8eb, 0x11d0, 0xb6, 0x2b, 0x0, 0xc0, 0x4f, 0xc3, 0x3e, 0xa1);


//
// Allow players to join the game session. (DEFAULT)
//
#define ZOPTION_ALLOW_JOINERS		0x00000001

//
// Bar any more players to join the game session.
//
#define ZOPTION_DISALLOW_JOINERS	0x00000002


//
// ZONEPROPERTY_GameState
//
// Informs lobby of game's current state.  The lobby uses this
// information to decide when to clear / save game defined
// properties (e.g., scores).
//
// If multiple games are played within a single direct play session, the
// application MUST send this property.  It is optional, although strongly
// recommended, if the game only allows a single play per session.
//
// Property data is a single DWORD.
//
// {BDD4B95F-D35C-11d0-B625-00C04FC33EA1}
DEFINE_GUID(ZONEPROPERTY_GameState, 
0xbdd4b95f, 0xd35c, 0x11d0, 0xb6, 0x25, 0x0, 0xc0, 0x4f, 0xc3, 0x3e, 0xa1);

//
// Game has finished initialization is ready to accept players.  Typically
// sent before game enters it's initial staging area.
//
#define ZSTATE_START	0x00000001

//
// Game is over and all the properties (e.g., scores) are final.  Once this
// state has been sent, the application should not send any more game defined
// defined properties until it sends a ZSTART_START property.
//
#define ZSTATE_END		0x00000002


//
// ZONEPROPERTY_GameNewHost
//
// Informs the lobby of host migration.  The client setting the property
// is assumed to be taking on the role of host.
//
// There is not property data
//
// {058ebd64-1373-11d1-968f-00c04fc2db04}
DEFINE_GUID(ZONEPROPERTY_GameNewHost,
0x058ebd64, 0x1373, 0x11d1, 0x96, 0x8f, 0x0, 0xc0, 0x4f, 0xc2, 0xdb, 0x04);


#ifdef __cplusplus
};
#endif /* __cplusplus */
