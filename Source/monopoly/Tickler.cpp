/*************************************************************
 *
 *   FILE:             TICKLER.C
 *
 *   (C) Copyright 97  Artech Digital Entertainments.
 *                     All rights reserved.
 *
 *  Code for "tickling" memory to make it stay in real
 *  RAM rather than out on the swap file.
 *
 *  
 *
 * $Header: /Projects - 1999/Mono 2 Hotseat/Tickler.cpp 1     1/08/99 2:06p Russellk $
 *
 * $Log: /Projects - 1999/Mono 2 Hotseat/Tickler.cpp $
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
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "gameinc.h"


/************************************************************/

/************************************************************/
/* MACRO DEFINITIONS AND TYPEDEFS                           */
/************************************************************/
#define TICKLE_ACCESS_LAG 18
  /* After this many other memory pages have been accessed, tickle the
  page that needs tickling again.  That way Windows will remember to keep
  the page in memory.  Under Windows 95B, you need to do 16 other accesses
  before the previously accessed page comes out of limbo and is worth
  tickling. */

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
  /* Number of bytes in a memory page and the value for shifting to
  get the page number out of an address.  4096 under Windows 95. */

/************************************************************/

/************************************************************/
/* GLOBALS                                                  */
/************************************************************/
static volatile BYTE *TickleHistory [TICKLE_ACCESS_LAG];
  /* This contains pointers to a valid byte somewhere in the pages that are
  in limbo and need to be tickled again.  After enough other pages have been
  loaded (16 of them) then the old page comes out of limbo and Windows will
  accept an access to it as making it more recently used. */

static unsigned char LastTickleIndex;
  /* Index into TickleHistory for the last used tickle pointer. */
/************************************************************/


/*****************************************************************************
 * Tickle a single byte location.  We read it now and then 16 page loads
 * later (when it comes out of limbo) we will read it again to make Windows
 * remember it (keep it in main memory rather than swapping it out).
 */

void TICKLER_Byte (volatile BYTE *BytePntr)
{
  int             CurrentIndex;
  volatile BYTE  *LastTickle;

  *BytePntr; /* Read the byte right now. */

  CurrentIndex = LastTickleIndex;
  LastTickle = TickleHistory [CurrentIndex];

  if ((((DWORD) LastTickle) >> PAGE_SHIFT) != (((DWORD) BytePntr) >> PAGE_SHIFT))
  {
    /* This new address is in a different page than the last done tickle (can
       get the same page if we end up tickling several small items, and that
       won't work since Windows won't do any page loading). */

    if (++CurrentIndex >= TICKLE_ACCESS_LAG)
      CurrentIndex = 0;

    /* Do the second tickle for the entry we are replacing at
       the end of the history. */

    LastTickle = TickleHistory [CurrentIndex];
    if (LastTickle != NULL)
      *LastTickle; /* Read the old byte to tickle it into recently used state. */

    TickleHistory [CurrentIndex] = BytePntr;
    LastTickleIndex = CurrentIndex;
  }
}



/*****************************************************************************
 * Tickle a range of data, moving it all into real memory.  Will access one
 * byte from each page of memory and then access it again later on when
 * enough time has passed (possibly done in future calls to this function).
 */

void TICKLER_Range (void *BufferPntr, long BufferSize)
{
  BYTE  *BytePntr;

  if (BufferPntr == NULL || BufferSize <= 0)
    return;

  BytePntr = (unsigned char *) BufferPntr;

  while (BufferSize > 0)
  {
    TICKLER_Byte (BytePntr);
    BytePntr += PAGE_SIZE;
    BufferSize -= PAGE_SIZE;
  }

  /* Also hit the last byte in the buffer in case we are off half a page. */

  BytePntr = (unsigned char *) BufferPntr;
  BytePntr += BufferSize - 1;
  TICKLER_Byte (BytePntr);
}



/*****************************************************************************
 * If you tickled some memory which will soon be deallocated, it's best to
 * call this function so that delayed tickle access to that memory won't
 * cause a memory exception.  For memory allocated by the Artech library,
 * this doesn't matter as much - allocations are done from a big block that
 * was allocated at the start of the game.
 */

void TICKLER_Flush (void)
{
  int i;

  for (i = 0; i < TICKLE_ACCESS_LAG; i++)
    TickleHistory [i] = NULL;
  LastTickleIndex = 0;
}
