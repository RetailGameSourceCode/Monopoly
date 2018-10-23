// ===========================================================================
// Module:      MemManager.cpp
//
// Description: 
//
// Created:     July 16, 1999
//
// Author:      
//
// Copywrite:   Artech, 1999
// ===========================================================================

#include "PC3DHdr.h"

MemManager  g_MemoryManager;

#ifdef _DEBUG


// ===========================================================================
// Function:    Alloc
//
// Description: Allocates a block of memory from the memory manager.
//
// Params:      nSize:  The size (in Bytes) of the requested block of memory
//              szFile: The file where the allocation is taking place
//              dwLine: The line number of the file where the allocation is
//                      taking place
//
// Returns:     void*
//
// Comments:    Returns a pointer to the newly allocated block of memory if
//              successful, or NULL if the memory could not be allocated.
//              This is a DEBUG version function and will be replaced when
//              compiling a RELEASE verison.
// ===========================================================================
void* MemManager::Alloc(UINT nSize, LPCSTR szFile, DWORD dwLine)
{
  ASSERT(NULL != m_hHeap);
  void* pMemBlock = ::HeapAlloc(m_hHeap, 0, (DWORD)nSize);
  if (pMemBlock)
  {
    // Create ourselves a new memory block to track the memory
    MemBlock* pMemory = (MemBlock*)::HeapAlloc(m_hMemTrackerHeap,
                                               HEAP_ZERO_MEMORY,
                                               sizeof(MemBlock));
    ASSERT(NULL != pMemory);
    pMemory->pBlock     = pMemBlock;
    pMemory->dwSize     = ::HeapSize(m_hHeap, 0, pMemBlock);
    pMemory->szFile     = (LPSTR)szFile;
    pMemory->dwLine     = dwLine;
    pMemory->pNextBlock = m_pHeadBlock;
    m_pHeadBlock        = pMemory;
    m_dwMemAllocated    += pMemory->dwSize;
    m_dwNumBlocks++;
  }
  return(pMemBlock);
}


// ===========================================================================
// Function:    DeAlloc
//
// Description: Deallocates a block of memory previously allocated from the
//              memory manager
//
// Params:      pMemBlock:  Pointer to the block of memory we want to destroy
//
// Returns:     void
//
// Comments:    This is a DEBUG version function and will be replaced when
//              compiling a RELEASE verison.
// ===========================================================================
void MemManager::DeAlloc(void* pMemBlock)
{
  ASSERT(NULL != m_hHeap);

  // We can't delete an empty pointer
  if (!pMemBlock) return;

  MemBlock* pPrev     = NULL;
  MemBlock* pCurrent  = m_pHeadBlock;

  // Find the block of memory in our tracking list
  while (pCurrent)
  {
    if (pCurrent->pBlock == pMemBlock) break;
    pPrev     = pCurrent;
    pCurrent  = pCurrent->pNextBlock;
  }

  // If we don't have a valid memory block, we can't destroy it
  ASSERT(NULL != pCurrent); if (!pCurrent) return;

  // Remove the memory block from the list
  if (pPrev)
    pPrev->pNextBlock = pCurrent->pNextBlock;
  else
    m_pHeadBlock = pCurrent->pNextBlock;

  // Free our memory block
  if (::HeapFree(m_hHeap, 0, pMemBlock))
  {
    m_dwMemAllocated -= pCurrent->dwSize;
    m_dwNumBlocks--;
    VERIFY(::HeapFree(m_hMemTrackerHeap, 0, pCurrent));
  }
}


// ===========================================================================
// Function:    DumpMemoryLeaks
//
// Description: Dumps the contents of the memory block list maintained by the
//              memory manager.  This is called by the destructor to display
//              any memory blocks that were not properly deallocated.
//
// Params:      void
//
// Returns:     void
//
// Comments:    This is a DEBUG version function and will be replaced when
//              compiling a RELEASE verison.
// ===========================================================================
void MemManager::DumpMemoryLeaks()
{
  TRACE("\n\n****************** Dumping Memory Leaks ******************\n\n");

  MemBlock* pCurrent = m_pHeadBlock;
  while (pCurrent)
  {
    TRACE("%s(%d): Allocation of %d bytes starting at %p\n", pCurrent->szFile,
          pCurrent->dwLine, pCurrent->dwSize, pCurrent->pBlock);
    pCurrent = pCurrent->pNextBlock;
  }

  TRACE("Total memory still allocated: %d bytes\n", m_dwMemAllocated);
  TRACE("Total number of blocks still allocated: %d\n", m_dwNumBlocks);
  TRACE("\n********************** End of dump ***********************\n\n");
}

#endif // _DEBUG

// ===========================================================================
// End Of File ===============================================================
// ===========================================================================
