/*****************************************************************************
 * FILE:        L_Memory.cpp
 *
 * DESCRIPTION: Memory allocation and deallocation.  Starting with ArtLib99,
 *              it will just do that and nothing else.  Multiple pools allow
 *              you to keep things contiguous in memory (less swapping and
 *              cache misses) and bulk deallocate groups of things.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Memory.cpp 8     1/17/99 5:26p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Memory.cpp $
 * 
 * 8     1/17/99 5:26p Agmsmith
 * Cosmetic changes.
 * 
 * 7     11/06/98 7:14p Agmsmith
 * Trying to get the single tasking mode to work.
 * 
 * 6     11/04/98 4:44p Agmsmith
 * Less error messages for corrupt memory blocks.
 * 
 * 5     11/03/98 3:10p Agmsmith
 * Add the ability to reallocate a memory block.  Also put in a critical
 * section to guard the global variables.  Also fix up the find size
 * function to check for memory corruption.
 * 
 * 4     11/02/98 5:22p Agmsmith
 * Only fill memory with $BB if it isn't corrupt.
 * 
 * 3     10/29/98 6:36p Agmsmith
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

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemMemory

/****************************************************************************/
/* M A C R O S   A N D   T Y P E   D E F I N I T I O N S                    */
/*--------------------------------------------------------------------------*/

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



/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

LE_MEMORY_PoolID LE_MEMORY_PoolForUserDynamicAllocation;
LE_MEMORY_PoolID LI_MEMORY_PoolForDataItemsOnly;
LE_MEMORY_PoolID LI_MEMORY_PoolForLibraryOnly;
  // Some default pools created by the memory system.



/****************************************************************************/
/* L O C A L   V A R I A B L E S                                            */
/*--------------------------------------------------------------------------*/

static volatile BYTE *LI_MEMORY_TickleHistory [TICKLE_ACCESS_LAG];
  /* This contains pointers to a valid byte somewhere in the pages that are
  in limbo and need to be tickled again.  After enough other pages have been
  loaded (16 of them) then the old page comes out of limbo and Windows will
  accept an access to it as making it more recently used. */

static unsigned char LI_MEMORY_TickleLastIndex;
  /* Index into LI_MEMORY_TickleHistory for the last used tickle pointer. */

static LE_MEMORY_PoolID LI_MEMORY_OpenPools [CE_ARTLIB_MemoryMaxNumberOfPools];
  /* A collection of all the known memory pools, zero marks unused entries.
  Bleeble: turn this into an array of Structs and keep statistical info
  on allocations and deallocations, if desired.  You could also store some
  hidden pointers in the allocation (boost the size by a few bytes and return
  an address just after your secret stuff for every memory alloc) and use that
  to keep a list of all the allocated memory blocks. */

static UNS32 TotalDataPoolMemoryUsed;
  /* Keeps track of how much memory is currently in allocations in the
  data pool, so that the data system can tell when it is getting full. */

#if CE_ARTLIB_EnableMultitasking
static CRITICAL_SECTION LI_MEMORY_CriticalSection;
static LPCRITICAL_SECTION L_TYPE_volatile LI_MEMORY_CriticalSectionPntr;
  /* Guards the access to the various memory system global variables, so
  that multiple threads can change them safely.  LI_MEMORY_CriticalSectionPntr
  is NULL if uninitialised, or points to LI_MEMORY_CriticalSection if it
  has been set up. */
#endif



/*****************************************************************************
 * Tickle a single byte location.  We read it now and then 16 page loads
 * later (when it comes out of limbo) we will read it again to make Windows
 * remember it (keep it in main memory rather than swapping it out).
 */
void LE_MEMORY_TickleByte (volatile BYTE *BytePntr)
{
  int             CurrentIndex;
  volatile BYTE  *LastTickle;

  *BytePntr; /* Read the byte right now. */

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr != NULL)
    EnterCriticalSection (LI_MEMORY_CriticalSectionPntr);
#endif

  CurrentIndex = LI_MEMORY_TickleLastIndex;
  LastTickle = LI_MEMORY_TickleHistory [CurrentIndex];

  if ((((DWORD) LastTickle) >> PAGE_SHIFT) != (((DWORD) BytePntr) >> PAGE_SHIFT))
  {
    /* This new address is in a different page than the last done tickle (can
       get the same page if we end up tickling several small items, and that
       won't work since Windows won't do any page loading). */

    if (++CurrentIndex >= TICKLE_ACCESS_LAG)
      CurrentIndex = 0;

    /* Do the second tickle for the entry we are replacing at
       the end of the history. */

    LastTickle = LI_MEMORY_TickleHistory [CurrentIndex];
    if (LastTickle != NULL)
      *LastTickle; /* Read the old byte to tickle it into recently used state. */

    LI_MEMORY_TickleHistory [CurrentIndex] = BytePntr;
    LI_MEMORY_TickleLastIndex = CurrentIndex;
  }

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr != NULL)
    LeaveCriticalSection (LI_MEMORY_CriticalSectionPntr);
#endif
}



/*****************************************************************************
 * Tickle a range of data, moving it all into real memory.  Will access one
 * byte from each page of memory and then access it again later on when
 * enough time has passed (possibly done in future calls to this function).
 */

void LE_MEMORY_TickleRange (void *BufferPntr, long BufferSize)
{
  BYTE  *BytePntr;

  if (BufferPntr == NULL || BufferSize <= 0)
    return;

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr != NULL)
    EnterCriticalSection (LI_MEMORY_CriticalSectionPntr);
#endif

  /* Hit the last byte in the buffer in case we are off half a page. */

  BytePntr = (BYTE *) BufferPntr;
  BytePntr += BufferSize - 1;
  LE_MEMORY_TickleByte (BytePntr);

  /* Hit one byte in each 4K page of memory to make sure it is swapped in. */

  BytePntr = (BYTE *) BufferPntr;

  while (BufferSize > 0)
  {
    LE_MEMORY_TickleByte (BytePntr);
    BytePntr += PAGE_SIZE;
    BufferSize -= PAGE_SIZE;
  }

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr != NULL)
    LeaveCriticalSection (LI_MEMORY_CriticalSectionPntr);
#endif
}



/*****************************************************************************
 * If you tickled some memory which will soon be deallocated, it's best to
 * call this function so that delayed tickle access to that memory won't
 * cause a memory exception.
 */

void LE_MEMORY_TickleFlush (void)
{
  int i;

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr != NULL)
    EnterCriticalSection (LI_MEMORY_CriticalSectionPntr);
#endif

  for (i = 0; i < TICKLE_ACCESS_LAG; i++)
    LI_MEMORY_TickleHistory [i] = NULL;
  LI_MEMORY_TickleLastIndex = 0;

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr != NULL)
    LeaveCriticalSection (LI_MEMORY_CriticalSectionPntr);
#endif
}



/*****************************************************************************
 * Describes the pool with a readable name, for debugging.  Returns a read
 * only string with the name.
 */

static char * GetPoolName (LE_MEMORY_PoolID Pool)
{
  if (Pool == NULL)
    return "NULL";
  if (Pool == LE_MEMORY_PoolForUserDynamicAllocation)
    return "User";
  if (Pool == LI_MEMORY_PoolForDataItemsOnly)
    return "Data";
  if (Pool == LI_MEMORY_PoolForLibraryOnly)
    return "Library";
  return "Custom";
}



/*****************************************************************************
 * Initialise a new pool.  Currently this is just a wrapper around the
 * Windows pool system, but we could do our own VirtualAlloc and manage
 * the memory ourselves - which would be useful for debugging since we
 * could then change the protection on memory pages to be read-only so
 * that accidental memory trashing of data items would be detected.
 *
 * If you specify a zero MaxSize then you can go on allocating memory until
 * the swap file fills up.  If you specify non-zero, then it will preallocate
 * memory pages (address range allocated but corresponding swap file pages
 * not allocated), and you won't be able to allocate items larger than 512K
 * due to a Windows limitation.
 */

LE_MEMORY_PoolID LE_MEMORY_PoolInit (UNS32 InitialSize, UNS32 MaxSize)
{
  int               i;
  LE_MEMORY_PoolID  NewPool;

  // First look for an empty slot in the array of known pools.

  for (i = 0; i < CE_ARTLIB_MemoryMaxNumberOfPools; i++)
  {
    if (LI_MEMORY_OpenPools [i] == NULL)
      break;
  }

  if (i >= CE_ARTLIB_MemoryMaxNumberOfPools)
  {
    // Didn't find a free spot.

#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LE_MEMORY_PoolInit: The maximum number of pools "
      "is already open, can't create another one.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL;
  }

  // Create the pool!

#if CE_ARTLIB_EnableMultitasking
  NewPool = HeapCreate (0, InitialSize, MaxSize);
#else
  NewPool = HeapCreate (HEAP_NO_SERIALIZE, InitialSize, MaxSize);
#endif

  if (NewPool == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LE_MEMORY_PoolInit: HeapCreate has failed!\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL;
  }

  LI_MEMORY_OpenPools [i] = NewPool;
  return NewPool;
}



/*****************************************************************************
 * Just deallocate the pool and all the memory that was allocated in it.
 */

void LE_MEMORY_PoolDestroy (LE_MEMORY_PoolID Pool)
{
  int i;

  for (i = 0; i < CE_ARTLIB_MemoryMaxNumberOfPools; i++)
  {
    if (LI_MEMORY_OpenPools [i] == Pool)
      break;
  }

  if (i >= CE_ARTLIB_MemoryMaxNumberOfPools)
  {
    // Didn't find the heap in our known heaps list.

#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_MEMORY_PoolDestroy: "
      "Called with unknown PoolID $%08X, ignoring it.\r\n",
      (int) Pool);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
  }
  else // OK to deallocate the whole thing.
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_MEMORY_PoolDestroy: "
      "Deallocating entire memory pool ID $%08X (index %d, %s pool).\r\n",
      (int) Pool, i, GetPoolName (Pool));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif

    LI_MEMORY_OpenPools [i] = NULL;

    if (Pool == LE_MEMORY_PoolForUserDynamicAllocation)
      LE_MEMORY_PoolForUserDynamicAllocation = NULL;
    else if (Pool == LI_MEMORY_PoolForDataItemsOnly)
      LI_MEMORY_PoolForDataItemsOnly = NULL;
    else if (Pool == LI_MEMORY_PoolForLibraryOnly)
      LI_MEMORY_PoolForLibraryOnly = NULL;

    HeapDestroy (Pool);
  }
}



/*****************************************************************************
 * Allocate a memory block from a pool.  Returns NULL if it fails.
 * If debugging, it gets filled with $AA value bytes and an extra
 * header is written before the user data containing the size of
 * the allocation plus CE_ARTLIB_MemoryBoundsCheckMargin bytes of
 * serial number.  The user data is also followed by serial number
 * bytes.  This is done so that we can detect some out of bounds
 * memory trashing errors.
 */

void * LE_MEMORY_PoolAllocBlock (LE_MEMORY_PoolID Pool, UNS32 Size)
{
#if CE_ARTLIB_MemoryFillWithGarbage
  int     i,j;
  BYTE   *MarginBack;
  BYTE   *MarginFront;
  BYTE   *SerialPntr;
  BYTE   *UserMemory;
#endif
  BYTE   *NewMemory;

  NewMemory = (BYTE *) HeapAlloc (Pool, 0,
    (CE_ARTLIB_MemoryFillWithGarbage)
    ? Size + 2 * CE_ARTLIB_MemoryBoundsCheckMargin + sizeof (UNS32)
    : Size);

  if (NewMemory == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_MEMORY_PoolAllocBlock: "
      "Unable to allocate %u bytes of memory from pool $%08X (%s pool).\r\n",
      (int) Size, (int) Pool, GetPoolName (Pool));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL;
  }

#if CE_ARTLIB_MemoryFillWithGarbage

  // Fill the user accessible part of the memory with $AA.

  UserMemory = NewMemory + CE_ARTLIB_MemoryBoundsCheckMargin + sizeof (UNS32);
  memset (UserMemory, 0xAA, Size);

  // Write the user's size at the very front of the area.

  *((UNS32 *) NewMemory) = Size;

  // Fill the margin area before and after the user's data with a serial
  // number (the serial number is just the user's address value).
  // Later on (deallocation time) we can verify that the margin data is
  // unchanged to see if the block has been overwritten by someone.

  MarginFront = NewMemory + sizeof (UNS32);
  MarginBack = NewMemory + (Size + CE_ARTLIB_MemoryBoundsCheckMargin + sizeof (UNS32));
  SerialPntr = (BYTE *) &UserMemory;
  j = 0;
  for (i = 0; i < CE_ARTLIB_MemoryBoundsCheckMargin; i++)
  {
    *MarginFront++ = *MarginBack++ = *SerialPntr++;

    if (++j >= sizeof (UserMemory))
    {
      j = 0;
      SerialPntr = (BYTE *) &UserMemory;
    }
  }
  return UserMemory;
#else // Not filling memory etc.
  return NewMemory;
#endif
}



/*****************************************************************************
 * Reallocate a memory block - just change the size and return a new pointer
 * to the same data.  Returns NULL if it fails, and loses your old data.
 */

void * LE_MEMORY_PoolReallocBlock (LE_MEMORY_PoolID Pool,
void *OldBlockPntr, UNS32 NewSize)
{
#if CE_ARTLIB_MemoryFillWithGarbage
  UNS32 CopySize;
  UNS32 OldSize;
#endif
  void *NewMemory;

#if CE_ARTLIB_MemoryFillWithGarbage
  NewMemory = LE_MEMORY_PoolAllocBlock (Pool, NewSize);
#else
  NewMemory = HeapReAlloc (Pool, 0, OldBlockPntr, NewSize);
#endif

  if (NewMemory == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_MEMORY_PoolReallocBlock: "
      "Unable to allocate %u bytes of memory from pool $%08X (%s pool).\r\n",
      (unsigned int) NewSize, (unsigned int) Pool, GetPoolName (Pool));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

#if CE_ARTLIB_MemoryFillWithGarbage
    LE_MEMORY_PoolDeallocBlock (Pool, OldBlockPntr);
#endif

    return NULL;
  }

  // Copy the old data to the new block and delete the old block, if we are
  // using our corruption checks.  Otherwise we are just using the operating
  // system realloc which has already done that for us.

#if CE_ARTLIB_MemoryFillWithGarbage
  OldSize = LE_MEMORY_PoolGetBlockSize (Pool, OldBlockPntr);
  if (OldSize != (UNS32) -1)
  {
    CopySize = OldSize;
    if (CopySize > NewSize)
      CopySize = NewSize;

    memcpy (NewMemory, OldBlockPntr, CopySize);

    LE_MEMORY_PoolDeallocBlock (Pool, OldBlockPntr);
  }
#endif

  return NewMemory;
}



/*****************************************************************************
 * Free a memory block, putting it back in the pool it came from.  Returns
 * TRUE if successful, FALSE if it fails.  If requested, it can fill it
 * with $BB byte values for debugging and check if the margins have been
 * overwritten.
 */

BOOL LE_MEMORY_PoolDeallocBlock (LE_MEMORY_PoolID Pool, void *BlockPntr)
{
#if CE_ARTLIB_MemoryFillWithGarbage
  int     i,j;
  BYTE   *MarginBack;
  BYTE   *MarginFront;
  BYTE   *RealMemory;
  BYTE   *SerialPntr;
  LPBYTE  SerialNumber;
  UNS32   UserSize;
#endif
  BOOL  Successful;

  if (BlockPntr == NULL)
    return FALSE;

#if CE_ARTLIB_MemoryFillWithGarbage
  RealMemory = (BYTE *) BlockPntr;
  RealMemory -= CE_ARTLIB_MemoryBoundsCheckMargin + sizeof (UNS32);

  // Read the hidden user specified size of the block.

  UserSize = *((UNS32 *) RealMemory);

  // Verify the serial numbers in the margins around the user data.

  MarginFront = RealMemory + sizeof (UNS32);
  MarginBack = RealMemory + (UserSize + CE_ARTLIB_MemoryBoundsCheckMargin + sizeof (UNS32));
  SerialNumber = (BYTE *) BlockPntr;
  SerialPntr = (BYTE *) &SerialNumber;
  j = 0;
  for (i = 0; i < CE_ARTLIB_MemoryBoundsCheckMargin; i++)
  {
    if (*MarginFront++ != *SerialPntr)
      break; // Serial number mismatch.

    if (*MarginBack++ != *SerialPntr++)
      break; // Serial number mismatch.

    if (++j >= sizeof (SerialNumber))
    {
      j = 0;
      SerialPntr = (BYTE *) &SerialNumber;
    }
  }

  if (i < CE_ARTLIB_MemoryBoundsCheckMargin)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_MEMORY_PoolDeallocBlock: "
      "Memory corruption detected before trying to free block at "
      "0x%08X in pool %u (%s pool).  Your program is overwriting "
      "memory it hasn't allocated!  Won't try to free that memory "
      "since that could cause a crash.\r\n",
      (unsigned int) BlockPntr, (unsigned int) Pool, GetPoolName (Pool));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);

    Successful = FALSE;
  }
  else
  {
    // Fill it all with $BB before freeing it.

    memset (RealMemory, 0xBB, HeapSize (Pool, 0, RealMemory));
    Successful = HeapFree (Pool, 0, RealMemory);
  }
#else // Not filling memory with garbage etc.
  Successful = HeapFree (Pool, 0, BlockPntr);
#endif

#if CE_ARTLIB_EnableDebugMode
  if (!Successful)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_MEMORY_PoolDeallocBlock: "
      "Unable to free pointer $%08X from pool $%08X (%s pool).\r\n",
      (int) BlockPntr, (int) Pool, GetPoolName (Pool));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

  return Successful;
}



/*****************************************************************************
 * Returns the size of an existing memory block, or rather the space allocated
 * for it in the pool, which can be larger than the size requested.  If it
 * can't find it, it returns -1 (0xFFFFFFFF in other words).
 */

UNS32 LE_MEMORY_PoolGetBlockSize (LE_MEMORY_PoolID Pool, void *BlockPntr)
{
#if CE_ARTLIB_MemoryFillWithGarbage
  int     i,j;
  BYTE   *MarginBack;
  BYTE   *MarginFront;
  BYTE   *RealMemory;
  BYTE   *SerialPntr;
  LPBYTE  SerialNumber;
#endif
  UNS32 Size;

  if (BlockPntr == NULL)
    return (UNS32) -1;

#if CE_ARTLIB_MemoryFillWithGarbage
  RealMemory = (BYTE *) BlockPntr;
  RealMemory -= CE_ARTLIB_MemoryBoundsCheckMargin + sizeof (UNS32);

  // Read the hidden user specified size of the block.

  Size = *((UNS32 *) RealMemory);

  // Verify the serial numbers in the margins around the user data.

  MarginFront = RealMemory + sizeof (UNS32);
  MarginBack = RealMemory + (Size + CE_ARTLIB_MemoryBoundsCheckMargin + sizeof (UNS32));
  SerialNumber = (BYTE *) BlockPntr;
  SerialPntr = (BYTE *) &SerialNumber;
  j = 0;
  for (i = 0; i < CE_ARTLIB_MemoryBoundsCheckMargin; i++)
  {
    if (*MarginFront++ != *SerialPntr)
      break; // Serial number mismatch.

    if (*MarginBack++ != *SerialPntr++)
      break; // Serial number mismatch.

    if (++j >= sizeof (SerialNumber))
    {
      j = 0;
      SerialPntr = (BYTE *) &SerialNumber;
    }
  }

  if (i < CE_ARTLIB_MemoryBoundsCheckMargin)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_MEMORY_PoolGetBlockSize: "
      "Memory corruption detected while trying to find size of block at "
      "0x%08X in pool %u (%s pool).  Your program is overwriting "
      "memory it hasn't allocated!\r\n",
      (unsigned int) BlockPntr, (unsigned int) Pool, GetPoolName (Pool));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);

    Size = (UNS32) -1; // Signal an error, size not known.
  }
#else // Normal operations, just use the Windows block size call.
  Size = HeapSize (Pool, 0, BlockPntr);
  #if CE_ARTLIB_EnableDebugMode
  if (Size == (UNS32) -1)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_MEMORY_PoolGetBlockSize: "
      "Unable to find size of pointer $%08X from pool $%08X (%s pool).\r\n",
      (int) BlockPntr, (int) Pool, GetPoolName (Pool));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
  #endif // CE_ARTLIB_EnableDebugMode
#endif // CE_ARTLIB_MemoryFillWithGarbage

  return Size;
}



/*****************************************************************************
 * Convenience functions for allocating stuff from the standard user pool.
 */

void * LE_MEMORY_AllocUserBlock (UNS32 Size)
{
  return
    LE_MEMORY_PoolAllocBlock (LE_MEMORY_PoolForUserDynamicAllocation, Size);
}

void * LE_MEMORY_ReallocUserBlock (void *OldBlockPntr, UNS32 NewSize)
{
  return
    LE_MEMORY_PoolReallocBlock (
      LE_MEMORY_PoolForUserDynamicAllocation, OldBlockPntr, NewSize);
}

BOOL LE_MEMORY_DeallocUserBlock (void *BlockPntr)
{
  return LE_MEMORY_PoolDeallocBlock (
    LE_MEMORY_PoolForUserDynamicAllocation, BlockPntr);
}

UNS32 LE_MEMORY_GetUserBlockSize (void *BlockPntr)
{
  return LE_MEMORY_PoolGetBlockSize (
    LE_MEMORY_PoolForUserDynamicAllocation, BlockPntr);
}



/*****************************************************************************
 * Convenience functions for allocating stuff from the standard data pool.
 * These ones also keep track of how much has been allocated, so that the
 * data system can start removing stuff if it gets too full.  Need a critical
 * section to avoid messing up the total count.
 */

void * LI_MEMORY_AllocDataBlock (UNS32 Size)
{
  void *ReturnValue;

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr != NULL)
    EnterCriticalSection (LI_MEMORY_CriticalSectionPntr);
#endif

  ReturnValue =
    LE_MEMORY_PoolAllocBlock (LI_MEMORY_PoolForDataItemsOnly, Size);

  if (ReturnValue != NULL)
    TotalDataPoolMemoryUsed +=
    LE_MEMORY_PoolGetBlockSize (LI_MEMORY_PoolForDataItemsOnly, ReturnValue);

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr != NULL)
    LeaveCriticalSection (LI_MEMORY_CriticalSectionPntr);
#endif

  return ReturnValue;
}

void * LI_MEMORY_ReallocDataBlock (void *OldBlockPntr, UNS32 NewSize)
{
  UNS32 OldSize;
  void *ReturnValue;

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr != NULL)
    EnterCriticalSection (LI_MEMORY_CriticalSectionPntr);
#endif

  OldSize = LE_MEMORY_PoolGetBlockSize (
    LI_MEMORY_PoolForDataItemsOnly, OldBlockPntr);

  if (OldSize != (UNS32) -1)
  {
    TotalDataPoolMemoryUsed -= OldSize;

    ReturnValue = LE_MEMORY_PoolReallocBlock (
    LI_MEMORY_PoolForDataItemsOnly, OldBlockPntr, NewSize);
  }
  else // No valid previous block, make a totally new one.
    ReturnValue = LE_MEMORY_PoolAllocBlock (
    LI_MEMORY_PoolForDataItemsOnly, NewSize);

  if (ReturnValue != NULL)
    TotalDataPoolMemoryUsed +=
    LE_MEMORY_PoolGetBlockSize (LI_MEMORY_PoolForDataItemsOnly, ReturnValue);

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr != NULL)
    LeaveCriticalSection (LI_MEMORY_CriticalSectionPntr);
#endif

  return ReturnValue;
}

BOOL LI_MEMORY_DeallocDataBlock (void *BlockPntr)
{
  UNS32 OldSize;
  BOOL  ReturnCode = FALSE;

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr != NULL)
    EnterCriticalSection (LI_MEMORY_CriticalSectionPntr);
#endif

  OldSize = LE_MEMORY_PoolGetBlockSize (
    LI_MEMORY_PoolForDataItemsOnly, BlockPntr);
  if (OldSize != (UNS32) -1) // Error signal is -1 size.
  {
    TotalDataPoolMemoryUsed -= OldSize;
    ReturnCode = LE_MEMORY_PoolDeallocBlock (
      LI_MEMORY_PoolForDataItemsOnly, BlockPntr);
  }

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr != NULL)
    LeaveCriticalSection (LI_MEMORY_CriticalSectionPntr);
#endif

  return ReturnCode;
}

UNS32 LI_MEMORY_GetDataBlockSize (void *BlockPntr)
{
  return
    LE_MEMORY_PoolGetBlockSize (LI_MEMORY_PoolForDataItemsOnly, BlockPntr);
}

UNS32 LI_MEMORY_GetDataTotalUsed (void)
{
  return TotalDataPoolMemoryUsed;
}



/*****************************************************************************
 * Convenience functions for allocating stuff from the standard library pool.
 */

void * LI_MEMORY_AllocLibraryBlock (UNS32 Size)
{
  return LE_MEMORY_PoolAllocBlock (LI_MEMORY_PoolForLibraryOnly, Size);
}

void * LI_MEMORY_ReallocLibraryBlock (void *OldBlockPntr, UNS32 NewSize)
{
  return
    LE_MEMORY_PoolReallocBlock (LI_MEMORY_PoolForLibraryOnly,
    OldBlockPntr, NewSize);
}

BOOL LI_MEMORY_DeallocLibraryBlock (void *BlockPntr)
{
  return LE_MEMORY_PoolDeallocBlock (LI_MEMORY_PoolForLibraryOnly, BlockPntr);
}

UNS32 LI_MEMORY_GetLibraryBlockSize (void *BlockPntr)
{
  return LE_MEMORY_PoolGetBlockSize (LI_MEMORY_PoolForLibraryOnly, BlockPntr);
}



/*****************************************************************************
 * Initialise the memory system.  This mostly involves setting up the
 * default memory pools.
 */

void LI_MEMORY_InitSystem (void)
{
  int i;

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr == NULL)
  {
    InitializeCriticalSection (&LI_MEMORY_CriticalSection);
    LI_MEMORY_CriticalSectionPntr = &LI_MEMORY_CriticalSection;
  }
#endif

  // Clear all the pool data structures.

  for (i = 0; i < CE_ARTLIB_MemoryMaxNumberOfPools; i++)
  {
#if CE_ARTLIB_EnableDebugMode
    if (LI_MEMORY_OpenPools [i] != NULL)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_MEMORY_InitSystem: "
      "Memory pool $%08X at index %d in LI_MEMORY_OpenPools is not NULL.\r\n",
      (int) LI_MEMORY_OpenPools [i], i);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    }
#endif
    LI_MEMORY_OpenPools [i] = NULL;
  }

  // Set up the standard user, library and data pools.

  LE_MEMORY_PoolForUserDynamicAllocation =
    LE_MEMORY_PoolInit (CE_ARTLIB_MemoryUserPoolInitialSize,
    0 /* No max size so that we can do allocations >512K */);

  LI_MEMORY_PoolForDataItemsOnly =
    LE_MEMORY_PoolInit (CE_ARTLIB_MemoryDataPoolInitialSize,
    0 /* No max size so that we can do allocations >512K */);

  LI_MEMORY_PoolForLibraryOnly =
    LE_MEMORY_PoolInit (CE_ARTLIB_MemoryLibraryPoolInitialSize,
    0 /* No max size so that we can do allocations >512K */);

  if (LI_MEMORY_PoolForDataItemsOnly == NULL ||
  LE_MEMORY_PoolForUserDynamicAllocation == NULL ||
  LI_MEMORY_PoolForLibraryOnly == NULL)
    LE_ERROR_ErrorMsg ("LI_MEMORY_InitSystem: "
    "Unable to initialise the usual memory pools.\r\n");

  TotalDataPoolMemoryUsed = 0;
}



/*****************************************************************************
 * Deallocate all memory pools and in debug mode, print out warnings about
 * memory blocks left allocated by mistake.
 */

void LI_MEMORY_RemoveSystem (void)
{
  int i;

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr != NULL)
    EnterCriticalSection (LI_MEMORY_CriticalSectionPntr);
#endif

  // List undeallocated items and free pools.

#if CE_ARTLIB_EnableDebugMode
  LE_ERROR_DebugMsg ("LI_MEMORY_RemoveSystem: Freeing memory pools.\r\n",
    LE_ERROR_DebugMsgToFile);

  if (TotalDataPoolMemoryUsed != 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_MEMORY_RemoveSystem: "
      "You left %u bytes allocated in the data pool.\r\n",
      (int) TotalDataPoolMemoryUsed);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

  for (i = 0; i < CE_ARTLIB_MemoryMaxNumberOfPools; i++)
  {
    if (LI_MEMORY_OpenPools [i] != NULL)
    {
      LE_MEMORY_PoolDestroy (LI_MEMORY_OpenPools [i]);
    }
  }

#if CE_ARTLIB_EnableMultitasking
  if (LI_MEMORY_CriticalSectionPntr != NULL)
  {
    LI_MEMORY_CriticalSectionPntr = NULL;
    DeleteCriticalSection (&LI_MEMORY_CriticalSection);
  }
#endif
}

#endif // if CE_ARTLIB_EnableSystemMemory
