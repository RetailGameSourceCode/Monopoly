#ifndef __UDAUCT_H__
#define __UDAUCT_H__

/*************************************************************
*
*   FILE:              UDAUCT.H
*
*   (C) Copyright 1999 Artech Digital Entertainments.
*                      All rights reserved.
*
*   Monopoly Game User Interface/Display Subfile Header.
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/


/************************************************************/
/* EXPORTED DATA STRUCTURES AND TYPES                       */
/************************************************************/


/************************************************************/
/* EXPORTED FUNCTIONS                                       */
/************************************************************/
void DISPLAY_UDAUCT_Initialize( void );
void DISPLAY_UDAUCT_Destroy( void );
void DISPLAY_UDAUCT_Show( void );

void UDAUCT_ProcessMessage( LE_QUEUE_MessagePointer UIMessagePntr );
void UDAUCT_ProcessMessageToPlayer( RULE_ActionArgumentsPointer NewMessage );
void UDAUCT_Process_NOTIFY_ARE_YOU_THERE( RULE_ActionArgumentsPointer NewMessage );



#endif // __UDAUCT_H__
