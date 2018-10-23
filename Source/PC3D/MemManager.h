// ===========================================================================
// Module:      MemManager.h
//
// Description: 
//
// Created:     July 16, 1999
//
// Author:      
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_MEMMANAGER
#define INC_MEMMANAGER

#define DEFAULT_HEAPSIZE         262144 // 256 * 1024 = 256 KB

#ifdef _DEBUG
#define DEBUG_HEAPTRACKERSIZE    32768  //  32 * 1024 =  32 KB
#endif // _DEBUG


// ===========================================================================
// MemManager class ==========================================================
// ===========================================================================
class MemManager
{
  // Member variables ========================================================
  private:
    // Handle to the heap this memory manager uses for allocating and
    //  deallocating memory
    HANDLE        m_hHeap;    

#ifdef _DEBUG
    struct MemBlock           // Memory block structure
    {
      void*       pBlock;     // Pointer to actual memory
      DWORD       dwSize;     // Size of allocated memory
      char*       szFile;     // File where the memory was allocated
      DWORD       dwLine;     // The line number where the memory was allocated
      MemBlock*   pNextBlock; // Pointer to the next memory block
    };

    HANDLE        m_hMemTrackerHeap;// Heap used to keep track of memory usage
    DWORD         m_dwMemAllocated; // Stores how much memory has been allocated
    DWORD         m_dwNumBlocks;    // Stores how many blocks have been allocated
    MemBlock*     m_pHeadBlock;     // Linked list of all the allocated items
#endif // _DEBUG


  // Construction / Destruction ==============================================
  public:
    INL MemManager(DWORD dwInitialSize = DEFAULT_HEAPSIZE);
    INL ~MemManager();


  // Private member functions ================================================
  private:
#ifdef _DEBUG
        void  DumpMemoryLeaks();
#endif // _DEBUG


  // Public member functions =================================================
  public:
    INL void  Compact();

#ifndef _DEBUG
    INL void* Alloc(UINT nSize);
    INL void  DeAlloc(void* pMemBlock);
#else // _DEBUG
        void* Alloc(UINT nSize, LPCSTR szFile, DWORD dwLine);
        void  DeAlloc(void* pMemBlock);
#endif // _DEBUG
};

extern MemManager   g_MemoryManager;

// ===========================================================================
// INLINE: MemManager member functions =======================================
// ===========================================================================


// ===========================================================================
INL MemManager::MemManager(DWORD dwInitialSize /*= DEFAULT_HEAPSIZE*/)
{
  m_hHeap = ::HeapCreate(0, dwInitialSize, 0);
  ASSERT(NULL != m_hHeap);

#ifdef _DEBUG
  m_dwMemAllocated  = 0;
  m_dwNumBlocks     = 0;
  m_pHeadBlock      = NULL;
  m_hMemTrackerHeap = ::HeapCreate(0, DEBUG_HEAPTRACKERSIZE, 0);
  ASSERT(NULL != m_hMemTrackerHeap);
#endif // _DEBUG
}

// ===========================================================================
INL MemManager::~MemManager()
{
#ifdef _DEBUG
  if (0 != m_dwNumBlocks) DumpMemoryLeaks();
  ASSERT(NULL != m_hMemTrackerHeap);
  VERIFY(::HeapDestroy(m_hMemTrackerHeap));
#endif // _DEBUG

  ASSERT(NULL != m_hHeap);
  VERIFY(::HeapDestroy(m_hHeap));
}

// ===========================================================================
INL void MemManager::Compact()
{ ASSERT(NULL != m_hHeap); ::HeapCompact(m_hHeap, 0);                       }


#ifndef _DEBUG

// ===========================================================================
INL void* MemManager::Alloc(UINT nSize)
{ return(::HeapAlloc(m_hHeap, 0, (DWORD)nSize));                            }

// ===========================================================================
INL void MemManager::DeAlloc(void* pMemBlock)
{ if (!pMemBlock) return; ::HeapFree(m_hHeap, 0, pMemBlock);                }


#if !CE_ARTLIB_DO_NOT_OVERRIDE_NEW_AND_DELETE

// ===========================================================================
INL void* __cdecl operator new(UINT nSize)
{ return(g_MemoryManager.Alloc(nSize));                                     }

// ===========================================================================
INL void __cdecl operator delete(void* pMemBlock)
{ g_MemoryManager.DeAlloc(pMemBlock);                                       }

#endif // !CE_ARTLIB_DO_NOT_OVERRIDE_NEW_AND_DELETE

#else // _DEBUG

#if !CE_ARTLIB_DO_NOT_OVERRIDE_NEW_AND_DELETE

// ===========================================================================
INL void* __cdecl operator new(UINT nSize, LPCSTR szFile, DWORD dwLine)
{ return(g_MemoryManager.Alloc(nSize, szFile, dwLine));                     }

// ===========================================================================
INL void __cdecl operator delete(void* pMemBlock)
{ g_MemoryManager.DeAlloc(pMemBlock);                                       }


#define DEBUGNEW    new(__FILE__, __LINE__)
#define new         DEBUGNEW  

#endif // !CE_ARTLIB_DO_NOT_OVERRIDE_NEW_AND_DELETE

#endif // _DEBUG

#define COMPACTMEMORY() g_MemoryManager.Compact()


#endif // INC_MEMMANAGER
// ===========================================================================
