#ifndef __MESS_H__
#define __MESS_H__

/************************************************************
 *
 *   FILE:             MESS.H
 *
 *   (C) Copyright 97 Artech Digital Entertainments.
 *                    All rights reserved.
 *
 *   Monopoly Game Message System Header
 *
 * $Header: /Projects - 1999/Mono 2/Mess.h 5     30/09/99 1:49p Agmsmith $
 *
 * $Log: /Projects - 1999/Mono 2/Mess.h $
 * 
 * 5     30/09/99 1:49p Agmsmith
 * Now with out of order reception of voice chat messages so that the game
 * can ignore the regular message queue but still receive voice chat.
 * 
 * 4     99/09/01 2:35p Agmsmith
 * Added new lobby score and game status update things.
 * 
 * 3     99/08/30 3:24p Agmsmith
 * Updated to work with DirectX 6.1 (DPlay4, Lobby3).
 *
 * 2     27/04/99 19:32 Timp
 * MESS_StartNetworking now takes Unicode strings
 *
 * 1     1/08/99 2:06p Russellk
 *
 * 1     1/08/99 11:35a Russellk
 * Graphics
 *
 * 3     1/04/99 11:39a Russellk
 * Game state restore added to log
 *
 * 2     12/17/98 4:27p Russellk
 * Obsolete comments removed for new version.  Log file facility entered
 * in Mess.c to record phase and action/error message activity.
 *
 ************************************************************/

/************************************************************/
/* SUBSIDUARY INCLUDES                                      */
/************************************************************/

#include "Mono2Zone.h"


/************************************************************/
/* EXPORTED GLOBALS                                         */
/************************************************************/
extern BOOL MESS_ServerMode;
extern BOOL MESS_NetworkMode;
extern BOOL MESS_WinSockSystemActive;
extern BOOL MESS_DirectPlaySystemActive;
extern BOOL MESS_GameStartedByLobby;
extern BOOL MESS_EnableMessageTrapping;
extern wchar_t MESS_PlayerNameFromNetwork [RULE_MAX_PLAYER_NAME_LENGTH+1];


/************************************************************/
/* PROTOTYPES                                               */
/************************************************************/
extern BOOL MESS_ReceiveActionMessage (RULE_ActionArgumentsPointer MessageToReceive);
extern BOOL MESS_ReceiveVoiceChatMessageOnly (RULE_ActionArgumentsPointer MessageToReceive);
extern void MESS_FreeStorageAssociatedWithMessage (RULE_ActionArgumentsPointer MessagePntr);
extern BOOL MESS_SendActionMessage (RULE_ActionArgumentsPointer MessageToSend);
extern BOOL MESS_SendAction (RULE_ActionType Action, RULE_PlayerNumber FromPlayer, RULE_PlayerNumber ToPlayer, long NumberA, long NumberB, long NumberC, long NumberD, wchar_t *StringA, unsigned int BinarySize, RULE_CharHandle Blob);
extern BOOL MESS_SendAction2 (RULE_ActionType Action, RULE_PlayerNumber FromPlayer, RULE_PlayerNumber ToPlayer, long NumberA, long NumberB, long NumberC, long NumberD, long NumberE, wchar_t *StringA, unsigned int BinarySize, RULE_CharHandle Blob);
extern BOOL MESS_StartNetworking (ACHAR *CommandLine);
extern DWORD MESS_CurrentQueueSize (void);
extern BOOL MESS_SetNetworkName (wchar_t *PlayerName);
extern void MESS_AssociatePlayerWithAddress (RULE_PlayerNumber PlayerNo, RULE_NetworkAddressPointer AddressOfPlayer);
extern void MESS_GetAddressOfPlayer (RULE_PlayerNumber PlayerNo, RULE_NetworkAddressPointer AddressToFillIn);
extern RULE_PlayerNumber MESS_GetPlayerForAddress (RULE_NetworkAddressPointer AddressOfPlayer);
extern BOOL MESS_GetComputerAddressFromIndex (int ComputerIndex, RULE_NetworkAddressPointer AddressToFillIn);
extern BOOL MESS_LetNewComputersIntoTheGame (void);
extern void MESS_UpdateLobbyWithCurrentGameInfo (MESS_ZoneGameUpdateInfoPointer ZoneGameUpdateInfoPntr);
extern void MESS_UpdateLobbyGameStarted (void);
extern void MESS_UpdateLobbyGameFinished (void);
extern void MESS_StopDirectPlay (void);
extern void MESS_StopWinSock (void);
extern void MESS_StopAllNetworking (void);
extern void MESS_CleanAndRemoveSystem (void);
extern BOOL MESS_InitializeSystem (void);

#if MESS_REPORT_ACTION_MESSAGES
extern void MESS_updateMessageLog( RULE_ActionArgumentsPointer MessageToSend, BOOL isAddingToQueue );
extern void MESS_RULE_updateMessageLog( RULE_PendingPhasePointer NewPhasePntr, int direction );
extern void MESS_STATE_updateMessageLog( RULE_ResyncCauses ResyncCause );
#endif

/************************************************************/

#endif // __MESS_H__
