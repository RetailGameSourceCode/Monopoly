#ifndef __L_MEMORY_H__
#define __L_MEMORY_H__

/*****************************************************************************
 * FILE:        L_Memory.h
 *
 * DESCRIPTION: Memory allocation and deallocation.  Starting with ArtLib99,
 *              it will just do that and nothing else.  Multiple pools allow
 *              you to keep things contiguous in memory (less swapping and
 *              cache misses) and bulk deallocate groups of things.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Memory.h 8     1/17/99 5:26p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Memory.h $
 * 
 * 8     1/17/99 5:26p Agmsmith
 * Cosmetic changes.
 * 
 * 7     11/24/98 11:22a Agmsmith
 * Add some convenient defines for standard memory operations.
 * 
 * 6     11/04/98 4:44p Agmsmith
 * Less error messages for corrupt memory blocks.
 * 
 * 5     11/03/98 3:10p Agmsmith
 * Add the ability to reallocate a memory block.  Also put in a critical
 * section to guard the global variables.  Also fix up the find size
 * function to check for memory corruption.
 * 
 * 4     10/29/98 6:36p Agmsmith
 * New improved memory and data system added.  Memory and data systems
 * have been separated.  Now have memory pools, and optional corruption
 * check.  The data system has data groups rather than files and
 * individual items can come from any data source. Also has a least
 * recently used data unloading system (rather than programmer set
 * priorities).  And it can all be turned off for AndrewX!
 *
 * 1     9/28/98 11:24a Agmsmith
 * Under construction.
 ****************************************************************************/



/****************************************************************************/
/* C O N V E N I E N C E   D E F I N E S                                    */
/*--------------------------------------------------------------------------*/

#define LE_MEMORY_malloc LE_MEMORY_AllocUserBlock
#define LE_MEMORY_free LE_MEMORY_DeallocUserBlock
#define LE_MEMORY_realloc LE_MEMORY_ReallocUserBlock
#define LE_MEMORY_size LE_MEMORY_GetUserBlockSize



/****************************************************************************/
/* M E M O R Y   T I C K L E R                                              */
/*--------------------------------------------------------------------------*/

extern void LE_MEMORY_TickleByte (volatile BYTE *BytePntr);
extern void LE_MEMORY_TickleRange (void *BufferPntr, long BufferSize);
// Makes the Windows 95B virtual memory system swap the given memory areas
// into real memory and makes them stick there, otherwise if you write to
// memory, it gets swapped out too quickly.

extern void LE_MEMORY_TickleFlush (void);
// The tickler stuff accesses memory locations a second time after a bit of
// a delay, so if you deallocated the memory that you tickled recently, you
// should call this function to avoid accessing deallocated memory.



/****************************************************************************/
/* M E M O R Y   P O O L S                                                  */
/*--------------------------------------------------------------------------*/

typedef HANDLE LE_MEMORY_PoolID;

// Functions for dealing with pools.
extern LE_MEMORY_PoolID LE_MEMORY_PoolInit (UNS32 InitialSize, UNS32 MaxSize);
extern void LE_MEMORY_PoolDestroy (LE_MEMORY_PoolID Pool);

// Functions for memory block allocation using a pool.
extern void * LE_MEMORY_PoolAllocBlock (LE_MEMORY_PoolID Pool, UNS32 Size);
extern void * LE_MEMORY_PoolReallocBlock (LE_MEMORY_PoolID Pool, void *OldBlockPntr, UNS32 NewSize);
extern BOOL LE_MEMORY_PoolDeallocBlock (LE_MEMORY_PoolID Pool, void *BlockPntr);
extern UNS32 LE_MEMORY_PoolGetBlockSize (LE_MEMORY_PoolID Pool, void *BlockPntr);

// Some default pools created by the memory system and their access functions.
extern LE_MEMORY_PoolID LE_MEMORY_PoolForUserDynamicAllocation;
extern LE_MEMORY_PoolID LI_MEMORY_PoolForDataItemsOnly;
extern LE_MEMORY_PoolID LI_MEMORY_PoolForLibraryOnly;

extern void * LE_MEMORY_AllocUserBlock (UNS32 Size);
extern void * LE_MEMORY_ReallocUserBlock (void *OldBlockPntr, UNS32 NewSize);
extern BOOL LE_MEMORY_DeallocUserBlock (void *BlockPntr);
extern UNS32 LE_MEMORY_GetUserBlockSize (void *BlockPntr);

extern void * LI_MEMORY_AllocDataBlock (UNS32 Size);
extern void * LI_MEMORY_ReallocDataBlock (void *OldBlockPntr, UNS32 NewSize);
extern BOOL LI_MEMORY_DeallocDataBlock (void *BlockPntr);
extern UNS32 LI_MEMORY_GetDataBlockSize (void *BlockPntr);
extern UNS32 LI_MEMORY_GetDataTotalUsed (void);

extern void * LI_MEMORY_AllocLibraryBlock (UNS32 Size);
extern void * LI_MEMORY_ReallocLibraryBlock (void *OldBlockPntr, UNS32 NewSize);
extern BOOL LI_MEMORY_DeallocLibraryBlock (void *BlockPntr);
extern UNS32 LI_MEMORY_GetLibraryBlockSize (void *BlockPntr);



/****************************************************************************/
/* S Y S T E M   I N I T   A N D   S H U T D O W N                          */
/*--------------------------------------------------------------------------*/

extern void LI_MEMORY_InitSystem (void);
extern void LI_MEMORY_RemoveSystem (void);


#endif // __L_MEMORY_H__
