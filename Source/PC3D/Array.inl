// ===========================================================================
// Module:      Array.inl
//
// Description: INL functions implementation for the Array class
//
// Created:     July 16, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

// ===========================================================================
// Array class ===============================================================
// ===========================================================================

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL Array<TYPE, ARG_TYPE>::Array() : m_pData(NULL), m_nSize(0), 
                                     m_nMaxSize(0), m_nGrowBy(0)
{                                                                           }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL Array<TYPE, ARG_TYPE>::~Array()
{ 
  if (m_pData)
  {
    ::MEMUTL_DestroyElements<TYPE>(m_pData, m_nSize);
    delete[] (BYTE*)m_pData;
  }
}


// ===========================================================================
// INLINE: Array member functions ===========================================
// ===========================================================================

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL int Array<TYPE, ARG_TYPE>::GetSize() const
{ return(m_nSize);                                                          }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL int Array<TYPE, ARG_TYPE>::GetDataSize() const
{ return(sizeof(TYPE));                                                     }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL int Array<TYPE, ARG_TYPE>::GetAllocSize() const
{ return(m_nMaxSize);                                                       }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL int Array<TYPE, ARG_TYPE>::GetUpperBound() const
{ return(m_nSize - 1);                                                      }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL void Array<TYPE, ARG_TYPE>::RemoveAll()
{ SetSize(0, -1);                                                           }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL BOOL Array<TYPE, ARG_TYPE>::Verify()
{ return(MEMUTL_IsValidAddress(m_pData, m_nSize * sizeof(TYPE)));           }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL TYPE Array<TYPE, ARG_TYPE>::GetAt(int nIndex) const
{ ASSERT(nIndex >= 0 && nIndex < m_nSize); return(m_pData[nIndex]);         }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL void Array<TYPE, ARG_TYPE>::SetAt(int nIndex, ARG_TYPE newElement)
{ ASSERT(nIndex >= 0 && nIndex < m_nSize); m_pData[nIndex] = newElement;    }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL void Array<TYPE, ARG_TYPE>::SetAtGrow(int nIndex, ARG_TYPE newElement)
{
  ASSERT(0 <= nIndex); 
  if (nIndex >= m_nSize) SetSize(nIndex + 1, -1);
  m_pData[nIndex] = newElement;
}

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL void Array<TYPE, ARG_TYPE>::Copy(const Array& array)
{
  ASSERT(this != &array); SetSize(array.m_nSize);
  MEMUTL_CopyElements<TYPE>(m_pData, array.m_pData, array.m_nSize);
}

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL TYPE& Array<TYPE, ARG_TYPE>::ElementAt(int nIndex)
{ ASSERT(nIndex >= 0 && nIndex < m_nSize); return(m_pData[nIndex]);         }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL const TYPE* Array<TYPE, ARG_TYPE>::GetData() const
{ return((const TYPE*)m_pData);                                             }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL TYPE* Array<TYPE, ARG_TYPE>::GetData()
{ return((TYPE*)m_pData);                                                   }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL void Array<TYPE, ARG_TYPE>::SetData(TYPE* pData, int nCount)
{ RemoveAll(); m_pData = pData; m_nSize = m_nMaxSize = nCount;              }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL int Array<TYPE, ARG_TYPE>::Add(ARG_TYPE newElement)
{ int nIndex = m_nSize; SetAtGrow(nIndex, newElement); return(nIndex);      }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL TYPE Array<TYPE, ARG_TYPE>::operator[](int nIndex) const
{ return(GetAt(nIndex));                                                    }

// ===========================================================================
template<class TYPE, class ARG_TYPE>
INL TYPE& Array<TYPE, ARG_TYPE>::operator[](int nIndex)
{ return(ElementAt(nIndex));                                                }


// ===========================================================================
// PUBLIC: Array member functions ===========================================
// ===========================================================================


// ===========================================================================
// Function:    SetSize
//
// Description: Sets the size of the array
//
// Params:      nNewSize: The new size of the array
//              nGrowBy:  How much we want the array to grow by when more
//                        elements need to be allocated
//
// Returns:     void
//
// Comments:    
// ===========================================================================
template<class TYPE, class ARG_TYPE>
void Array<TYPE, ARG_TYPE>::SetSize(int nNewSize, int nGrowBy)
{
  ASSERT(0 <= nNewSize);

  if (nGrowBy != -1)
    m_nGrowBy = nGrowBy;  // set new size

  if (0 == nNewSize)
  {
    // shrink to nothing
    if (m_pData)
    {
      MEMUTL_DestroyElements<TYPE>(m_pData, m_nSize);
      delete[] (BYTE*)m_pData;
      m_pData = NULL;
    }

    m_nSize = m_nMaxSize = 0;
  }
  else if (!m_pData)
  {
    // create one with exact size
    ASSERT(nNewSize <= ARRAY_MAX/sizeof(TYPE));    // no overflow
    m_pData = (TYPE*)new BYTE[nNewSize * sizeof(TYPE)];
    MEMUTL_ConstructElements<TYPE>(m_pData, nNewSize);
    m_nSize = m_nMaxSize = nNewSize;
  }
  else if (nNewSize <= m_nMaxSize)
  {
    // it fits
    if (nNewSize > m_nSize)
    {
      // initialize the new elements
      MEMUTL_ConstructElements<TYPE>(&m_pData[m_nSize], nNewSize - m_nSize);
    }
    else if (m_nSize > nNewSize)
    {
      // destroy the old elements
      MEMUTL_DestroyElements<TYPE>(&m_pData[nNewSize], m_nSize - nNewSize);
    }
    m_nSize = nNewSize;
  }
  else
  {
    // otherwise, grow array
    int nGrowBy = m_nGrowBy;
    if (nGrowBy == 0)
    {
      // heuristically determine growth when nGrowBy == 0
      //  (this avoids heap fragmentation in many situations)
      nGrowBy = m_nSize / 8;
      nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
    }
    int nNewMax;
    if (nNewSize < m_nMaxSize + nGrowBy)
      nNewMax = m_nMaxSize + nGrowBy;  // granularity
    else
      nNewMax = nNewSize;  // no slush

    ASSERT(nNewMax >= m_nMaxSize);  // no wrap around
    ASSERT(nNewSize <= ARRAY_MAX/sizeof(TYPE));    // no overflow
    TYPE* pNewData = (TYPE*) new BYTE[nNewMax * sizeof(TYPE)];

    // copy new data from old
    memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));

    // construct remaining elements
    ASSERT(nNewSize > m_nSize);
    MEMUTL_ConstructElements<TYPE>(&pNewData[m_nSize], nNewSize-m_nSize);

    // get rid of old stuff (note: no destructors called)
    delete[] (BYTE*)m_pData;
    m_pData     = pNewData;
    m_nSize     = nNewSize;
    m_nMaxSize  = nNewMax;
  }
}


// ===========================================================================
// Function:    Append
//
// Description: Appends another array to this array
//
// Params:      array:  The array we want to append
//
// Returns:     int
//
// Comments:    Returns the index of the first appended element
// ===========================================================================
template<class TYPE, class ARG_TYPE>
int Array<TYPE, ARG_TYPE>::Append(const Array& array)
{
  ASSERT(this != &array);   // cannot append to itself

  int nOldSize = m_nSize;
  SetSize(m_nSize + array.m_nSize);
  MEMUTL_CopyElements<TYPE>(m_pData + nOldSize, array.m_pData, array.m_nSize);
  return(nOldSize);
}


// ===========================================================================
// Function:    FreeExtra
//
// Description: Frees all extra memory allocated by the array
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
template<class TYPE, class ARG_TYPE>
void Array<TYPE, ARG_TYPE>::FreeExtra()
{
  if (m_nSize != m_nMaxSize)
  {
    // shrink to desired size
    ASSERT(nNewSize <= ARRAY_MAX/sizeof(TYPE));    // no overflow
    TYPE* pNewData = NULL;
    if (0 != m_nSize)
    {
      // Create new buffer
      pNewData = (TYPE*)new BYTE[m_nSize * sizeof(TYPE)];
      // copy new data from old
      memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));
    }

    // get rid of old stuff (note: no destructors called)
    delete[] (BYTE*)m_pData;
    m_pData     = pNewData;
    m_nMaxSize  = m_nSize;
  }
}


// ===========================================================================
// Function:    InsertAt
//
// Description: Inserts one or more new elements in the array
//
// Params:      nIndex:     The index where to insert the new element(s)
//              newElement: The new element we want to insert
//              nCound:     How many new elements to insert
//
// Returns:     void
//
// Comments:    This function will NOT grow the array to ensure that all of
//              the new elements being inserted will fit.
// ===========================================================================
template<class TYPE, class ARG_TYPE>
void Array<TYPE, ARG_TYPE>::InsertAt(int nIndex, ARG_TYPE newElement,
                                      int nCount /*=1*/)
{
  ASSERT(0 <= nIndex);    // will expand to meet need
  ASSERT(0 < nCount);     // zero or negative size not allowed

  if (nIndex >= m_nSize)
  {
    // adding after the end of the array
    SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
  }
  else
  {
    // inserting in the middle of the array
    int nOldSize = m_nSize;
    SetSize(m_nSize + nCount, -1);  // grow it to new size
    
    // destroy initial data before copying over it
    MEMUTL_DestroyElements<TYPE>(&m_pData[nOldSize], nCount);

    // shift old data up to fill gap
    ::memmove(&m_pData[nIndex+nCount], &m_pData[nIndex], 
              (nOldSize-nIndex) * sizeof(TYPE));

    // re-init slots we copied from
    MEMUTL_ConstructElements<TYPE>(&m_pData[nIndex], nCount);
  }

  // insert new value in the gap
  ASSERT(nIndex + nCount <= m_nSize);
  while (nCount--)
    m_pData[nIndex++] = newElement;
}


// ===========================================================================
// Function:    RemoveAt
//
// Description: Removes one or more elements from the array
//
// Params:      nIndex: Where to remove the element from
//              nCount: How many elements to remove
//
// Returns:     void
//
// Comments:    
// ===========================================================================
template<class TYPE, class ARG_TYPE>
void Array<TYPE, ARG_TYPE>::RemoveAt(int nIndex, int nCount /*= 1*/)
{
  ASSERT(0 <= nIndex);
  ASSERT(0 <= nCount);
  ASSERT(nIndex + nCount <= m_nSize);

  // just remove a range
  int nMoveCount = m_nSize - (nIndex + nCount);
  MEMUTL_DestroyElements<TYPE>(&m_pData[nIndex], nCount);

  if (nMoveCount)
  {
    memcpy(&m_pData[nIndex], &m_pData[nIndex + nCount], 
           nMoveCount * sizeof(TYPE));
  }

  m_nSize -= nCount;
}


// ===========================================================================
// Function:    InsertAt
//
// Description: Inserts the contents of another array into this array
//
// Params:      nStartIndex:  Where to insert the array
//              pNewArray:    The array of new elements to insert
//
// Returns:     void
//
// Comments:    
// ===========================================================================
template<class TYPE, class ARG_TYPE>
void Array<TYPE, ARG_TYPE>::InsertAt(int nStartIndex, Array* pNewArray)
{
  ASSERT(NULL != pNewArray);
  ASSERT(0 <= nStartIndex);

  if (0 < pNewArray->m_nSize)
  {
    InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
    for (int i = 0; i < pNewArray->GetSize(); i++)
      SetAt(nStartIndex + i, pNewArray->GetAt(i));
  }
}
