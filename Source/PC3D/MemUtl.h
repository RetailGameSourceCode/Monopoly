// ===========================================================================
// Module:      MemUtl.h
//
// Description: Function prototypes for the memory utility functions
//
// Created:     July 16, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_MEMUTL
#define INC_MEMUTL

// ===========================================================================
// Macros ====================================================================
// ===========================================================================


#ifdef _DEBUG

static ULONG g_ulLastRefCount = 0;

#define LASTREFCOUNT          g_ulLastRefCount
#define RELEASEPOINTER(p)                                                 \
  if (p)                                                                  \
  {                                                                       \
    LASTREFCOUNT = (p)->Release();                                        \
    TRACE("%s(%d) : Release of pointer %s(%08X), ref count: %d\r\n",      \
          __FILE__, __LINE__, #p, p, LASTREFCOUNT);                       \
    (p) = NULL;                                                           \
  }                                                                       \
  else LASTREFCOUNT = 0

#else // _DEBUG

  #define RELEASEPOINTER(p)       if (p) ((p)->Release(), (p) = NULL)
#endif // _DEBUG

#define DESTROYPOINTER(p)         if (p) (delete (p), (p) = NULL)
#define DESTROYARRAY(p)           if (p) (delete [] (p), (p) = NULL)


// ===========================================================================
// PROTOTYPES: MemUtl functions ==============================================
// ===========================================================================


INL BOOL MEMUTL_IsValidAddress(const void* lp, UINT nBytes,
                               BOOL bReadWrite = TRUE);

template<class TYPE> 
INL void MEMUTL_ConstructElements(TYPE* pElements, int nCount);

template<class TYPE>
INL void MEMUTL_DestroyElements(TYPE* pElements, int nCount);

template<class TYPE>
INL void MEMUTL_CopyElements(TYPE* pDest, const TYPE* pSrc, int nCount);


// ===========================================================================
// INLINE: MemUtl functions ==================================================
// ===========================================================================


// ===========================================================================
INL void* __cdecl operator new(UINT nSize, void* p)
{ return(p);                                                                }


// ===========================================================================
INL BOOL MEMUTL_IsValidAddress(const void* lp, UINT nBytes,
                               BOOL bReadWrite /*= TRUE*/)
{
  return(NULL != lp && !::IsBadReadPtr(lp, nBytes) &&
         (!bReadWrite || !::IsBadWritePtr((LPVOID)lp, nBytes)));
}

// ===========================================================================
template<class TYPE> 
INL void MEMUTL_ConstructElements(TYPE* pElements, int nCount)
{
  ASSERT(nCount == 0 || MEMUTL_IsValidAddress(pElements, nCount * sizeof(TYPE)));
  ZeroMemory((void*)pElements, nCount * sizeof(TYPE));
  for (; nCount--; pElements++) ::new((void*)pElements) TYPE;
}

// ===========================================================================
template<class TYPE>
INL void MEMUTL_DestroyElements(TYPE* pElements, int nCount)
{
  ASSERT(nCount == 0 || MEMUTL_IsValidAddress(pElements, nCount * sizeof(TYPE)));
  for (; nCount--; pElements++) pElements->~TYPE();
}

template<class TYPE>
INL void MEMUTL_CopyElements(TYPE* pDest, const TYPE* pSrc, int nCount)
{
  ASSERT(nCount == 0 || MEMUTL_IsValidAddress(pDest, nCount * sizeof(TYPE)));
  ASSERT(nCount == 0 || MEMUTL_IsValidAddress(pSrc, nCount * sizeof(TYPE)));
  while (nCount--) *pDest++ = *pSrc++;
}


#endif // INC_MEMUTL
// ===========================================================================
