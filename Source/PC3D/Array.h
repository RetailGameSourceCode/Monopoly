// ===========================================================================
// Module:      Array.h
//
// Description: Class definition for the Array class
//
// Created:     July 16, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_ARRAY
#define INC_ARRAY

// ===========================================================================
// Array class ===============================================================
// ===========================================================================

template<class TYPE, class ARG_TYPE> class Array
{
  // Member variables ========================================================
  protected:
    TYPE* m_pData;      // the actual array of data
    int   m_nSize;      // # of elements (upperBound + 1)
    int   m_nMaxSize;   // max allocated
    int   m_nGrowBy;    // grow amount


  // Construction / Destruction ==============================================
  public:
    INL Array();
    INL virtual ~Array();


  // Private member functions ================================================
  private:

  // Protected member functions ==============================================
  protected:


  // Public member functions =================================================
  public:
    // Attributes ============================================================
    INL int   GetSize() const;
    INL int   GetDataSize() const;
    INL int   GetAllocSize() const;
    INL int   GetUpperBound() const;
        void  SetSize(int nNewSize, int nGrowBy = -1);

    // Operations ============================================================
        void  FreeExtra();
    INL void  RemoveAll();
    INL BOOL  Verify();

    // Accessing elements ====================================================
    INL TYPE  GetAt(int nIndex) const;
    INL void  SetAt(int nIndex, ARG_TYPE newElement);
    INL TYPE& ElementAt(int nIndex);

    // Direct Access to the element data (may return NULL)
    INL const TYPE* GetData() const;
    INL TYPE* GetData();

    // This can be really dangerous
    INL void  SetData(TYPE* pData, int nCount);

    // Potentially growing the array
    INL void  SetAtGrow(int nIndex, ARG_TYPE newElement);
    INL int   Add(ARG_TYPE newElement);
    INL void  Copy(const Array& src);
        int   Append(const Array& src);

    // Overloaded operator helpers
    INL TYPE  operator[](int nIndex) const;
    INL TYPE& operator[](int nIndex);

    // Operations that move elements around
        void  InsertAt(int nIndex, ARG_TYPE newElement, int nCount = 1);
        void  RemoveAt(int nIndex, int nCount = 1);
        void  InsertAt(int nStartIndex, Array* pNewArray);
};

#ifndef UINT_MAX
#define ARRAY_MAX       0x7FFFFFFF
#else
#define ARRAY_MAX        UINT_MAX
#endif

#include "Array.inl"

// Define some common array types
typedef Array<int, int>        IntArray;
typedef Array<UINT, UINT>      UIntArray;
typedef Array<BYTE, BYTE>      ByteArray;
typedef Array<WORD, WORD>      WordArray;
typedef Array<DWORD, DWORD>    DWordArray;

#endif // INC_ARRAY
