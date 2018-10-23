#ifndef __TICKLER_H__
#define __TICKLER_H__

/*****************************************************************************
 *
 *   FILE:             TICKLER.H
 *
 *   (C) Copyright 97 Artech Digital Entertainments.
 *                    All rights reserved.
 *
 *   This set of subroutines tickles memory to make it move from the swap file
 *   into real memory under Windows 95B, and stay there for a little while.
 *
 * $Header: /Projects - 1999/Mono 2 Hotseat/Tickler.h 1     1/08/99 2:06p Russellk $
 *
 * $Log: /Projects - 1999/Mono 2 Hotseat/Tickler.h $
 * 
 * 1     1/08/99 2:06p Russellk
 * 
 * 1     1/08/99 11:35a Russellk
 * Graphics
 * 
 * 2     12/17/98 4:27p Russellk
 * Obsolete comments removed for new version.  Log file facility entered
 * in Mess.c to record phase and action/error message activity.
 * 
 * Virtual memory functions for moving things to real RAM.
 ************************************************************/


/************************************************************/
/* DEFINES                                                  */
/************************************************************/

#if FORHOTSEAT
  #define LE_MEMORY_TickleByte  TICKLER_Byte
  #define LE_MEMORY_TickleRange TICKLER_Range
  #define LE_MEMORY_TickleFlush TICKLER_Flush
#endif

/************************************************************/
/* EXPORTED GLOBALS                                         */
/************************************************************/



/************************************************************/
/* PROTOTYPES                                               */
/************************************************************/
extern void TICKLER_Byte (volatile BYTE *BytePntr);
extern void TICKLER_Range (void *BufferPntr, long BufferSize);
extern void TICKLER_Flush (void);
/************************************************************/

#endif // __TICKLER_H__
