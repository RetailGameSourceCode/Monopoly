// ===========================================================================
// Module:      Matrix.cpp
//
// Description: Implementation of the Matrix class which represents a 4x4
//              matrix.  Derived fromt the D3DMATRIX structure.
//
// Created:     May 17, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#include "PC3DHdr.h"


// ===========================================================================
// Globals ===================================================================
// ===========================================================================

const D3DMATRIX  ZEROMATRIX(0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
const D3DMATRIX  IDENTITYMATRIX(1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f, 
                                0.0f, 0.0f, 1.0f, 0.0f, 
                                0.0f, 0.0f, 0.0f, 1.0f);


// ===========================================================================
// Matrix class ==============================================================
// ===========================================================================


// ===========================================================================
// PRIVATE: Matrix member functions ==========================================
// ===========================================================================


// ===========================================================================
// PROTECTED: Matrix member functions ========================================
// ===========================================================================


// ===========================================================================
// PUBLIC: Matrix member functions ===========================================
// ===========================================================================


// ===========================================================================
// Function:    ViewMatrix
//
// Description: Initializes the matrix as a view (camera) matrix
//
// Params:      vFrom:    Position where the camera is looking from
//              vAt:      Point where the camera is looking at
//              vWorldUp: Vector specifying which direction is up
//              fRoll:    A roll factor in radians
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Matrix::ViewMatrix(const D3DVECTOR& vFrom, const D3DVECTOR& vAt, 
                        const D3DVECTOR& vWorldUp, float fRoll /*= 0.0f*/)
{
  Identity();

  // Get the z basis vector, which points straight ahead. This is the
  // difference from the eyepoint to the lookat point.
  D3DVECTOR vView = vAt - vFrom;

  FLOAT fLength = Magnitude(vView);
  if (!WITHINTOLERANCE(fLength))
    return;

  // Normalize the z basis vector
  vView /= fLength;

  // Get the dot product, and calculate the projection of the z basis
  // vector onto the up vector. The projection is the y basis vector.
  D3DVECTOR vUp = vWorldUp - (DotProduct(vWorldUp, vView) * vView);

#ifdef _DEBUG
  // If this vector has near-zero length because the input specified a
  // bogus up vector, let's try a default up vector
  if (!WITHINTOLERANCE(Magnitude(vUp)))
  {
    vUp = D3DVECTOR(0.0f, 1.0f, 0.0f) - vView.y * vView;

    // If we still have near-zero length, resort to a different axis.
    if (!WITHINTOLERANCE(Magnitude(vUp)))
    {
      vUp = D3DVECTOR(0.0f, 0.0f, 1.0f) - vView.z * vView;

      if (!WITHINTOLERANCE(Magnitude(vUp)))
        return;
    }
  }
#endif // _DEBUG

  // Normalize the y basis vector
  vUp = Normalize(vUp);

  // The x basis vector is found simply with the cross product of the y
  // and z basis vectors
  D3DVECTOR vRight = CrossProduct(vUp, vView);
  
  // Start building the matrix. The first three rows contains the basis
  // vectors used to rotate the view to point at the lookat point
  _11 = vRight.x;   _12 = vUp.x;    _13 = vView.x;
  _21 = vRight.y;   _22 = vUp.y;    _23 = vView.y;
  _31 = vRight.z;   _32 = vUp.z;    _33 = vView.z;

  // Do the translation values (rotations are still about the eyepoint)
  _41 = -DotProduct(vFrom, vRight);
  _42 = -DotProduct(vFrom, vUp);
  _43 = -DotProduct(vFrom, vView);

  // Set roll
  if (WITHINTOLERANCE(fRoll))
    RotateZ(fRoll);
}


// ===========================================================================
// Function:    RotateAxisMatrix
//
// Description: Creates a rotation matrix for rotating about an arbitrary
//              axis.
//
// Params:      vAxis:  The axis to rotate around
//              fRads:  The number of degrees to rotate (in radians)
//
// Returns:     D3DMATRIX
//
// Comments:    Returns a matrix representing the calculated rotation.  This
//              is a static function.
// ===========================================================================
D3DMATRIX Matrix::RotateAxisMatrix(const D3DVECTOR& vAxis, float fRads)
{
  D3DMATRIX mtrx;
  float     fCos  = (float)cos(fRads);
  float     fSin  = (float)sin(fRads);
  D3DVECTOR v     = Normalize(vAxis);
  float f1_Cos    = 1.0f - fCos;

  mtrx._11 = (v.x * v.x) * f1_Cos + fCos;
  mtrx._12 = (v.x * v.y) * f1_Cos - (v.z * fSin);
  mtrx._13 = (v.x * v.z) * f1_Cos + (v.y * fSin);

  mtrx._21 = (v.y * v.x) * f1_Cos + (v.z * fSin);
  mtrx._22 = (v.y * v.y) * f1_Cos + fCos;
  mtrx._23 = (v.y * v.z) * f1_Cos - (v.x * fSin);

  mtrx._31 = (v.z * v.x) * f1_Cos - (v.y * fSin);
  mtrx._32 = (v.z * v.y) * f1_Cos + (v.x * fSin);
  mtrx._33 = (v.z * v.z) * f1_Cos + fCos;

  mtrx._14 = mtrx._24 = mtrx._34 = 0.0f;
  mtrx._41 = mtrx._42 = mtrx._43 = 0.0f;
  mtrx._44 = 1.0f;

  return(mtrx);
}


// ===========================================================================
// Function:    RotateAxis
//
// Description: Applies a rotation to the matrix around the given arbitrary
//              axis.
//
// Params:      vAxis:  The axis to rotate around
//              fRads:  The number of degrees to rotate (in radians)
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Matrix::RotateAxis(const D3DVECTOR& vAxis, float fRads)
{
  D3DMATRIX mtrx;
  float     fCos = (float)cos(fRads);
  float     fSin = (float)sin(fRads);
  D3DVECTOR v    = Normalize(vAxis);

  // Temporary local macros
  #define   XY      mtrx._14
  #define   XZ      mtrx._24
  #define   YZ      mtrx._34
  #define   F1_COS  mtrx._44
  #define   XSIN    mtrx._41
  #define   YSIN    mtrx._42
  #define   ZSIN    mtrx._43

  // Precompute these values so we can save some on the number of multiplies
  XY      = v.x * v.y;
  XZ      = v.x * v.z;
  YZ      = v.y * v.z;
  F1_COS  = 1.0f - fCos;
  XSIN    = v.x * fSin;
  YSIN    = v.y * fSin;
  ZSIN    = v.z * fSin;

  // Compute the components of the rotation matrix
  mtrx._11 = SQR(v.x) * F1_COS + fCos;
  mtrx._12 = XY       * F1_COS - ZSIN;
  mtrx._13 = XZ       * F1_COS + YSIN;

  mtrx._21 = XY       * F1_COS + ZSIN;
  mtrx._22 = SQR(v.y) * F1_COS + fCos;
  mtrx._23 = YZ       * F1_COS - XSIN;

  mtrx._31 = XZ       * F1_COS - YSIN;
  mtrx._32 = YZ       * F1_COS + XSIN;
  mtrx._33 = SQR(v.z) * F1_COS + fCos;

  // Clean up our local macros
  #undef    XY
  #undef    XZ
  #undef    YZ
  #undef    F1_COS
  #undef    XSIN
  #undef    YSIN
  #undef    ZSIN

  // Now apply the multiplication
  //  INLINE DIRECT COMPUTATION)              FULL MATRIX MULTIPLY
  //  ==================================      ================================
  //  9 * 4 = 36 multiplications              64 multiplications
  //  6 * 4 = 24 additions                    48 additions
  //  20 assignements                         16 assignments
  //                                          1 memory copy (sizeof(D3DMATRIX)
  //
  // Conclusion:  Use the direct computation, even though it's a bit more
  //              work in terms of coding.

  // First row
  v.x = _11 * mtrx._11 + _12 * mtrx._21 + _13 * mtrx._31;
  v.y = _11 * mtrx._12 + _12 * mtrx._22 + _13 * mtrx._32;
  _13 = _11 * mtrx._13 + _12 * mtrx._23 + _13 * mtrx._33;
  _11 = v.x; _12 = v.y;

  // Second row
  v.x = _21 * mtrx._11 + _22 * mtrx._21 + _23 * mtrx._31;
  v.y = _21 * mtrx._12 + _22 * mtrx._22 + _23 * mtrx._32;
  _23 = _21 * mtrx._13 + _22 * mtrx._23 + _23 * mtrx._33;
  _21 = v.x; _22 = v.y;

  // Third row
  v.x = _31 * mtrx._11 + _32 * mtrx._21 + _33 * mtrx._31;
  v.y = _31 * mtrx._12 + _32 * mtrx._22 + _33 * mtrx._32;
  _33 = _31 * mtrx._13 + _32 * mtrx._23 + _33 * mtrx._33;
  _31 = v.x; _32 = v.y;

  // Forth row
  v.x = _41 * mtrx._11 + _42 * mtrx._21 + _43 * mtrx._31;
  v.y = _41 * mtrx._12 + _42 * mtrx._22 + _43 * mtrx._32;
  _43 = _41 * mtrx._13 + _42 * mtrx._23 + _43 * mtrx._33;
  _41 = v.x; _42 = v.y;
}


// ===========================================================================
// Function:    Invert
//
// Description: Computes the inverse of the matrix, if it exists.
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the inverse of the matrix was successfully
//              computed or FALSE if the inverse does not exist.
// ===========================================================================
BOOL Matrix::Invert()
{
  Matrix  mtrxTmp(*this);
  Matrix  mtrxInverse(Matrix::IDENTITY);
  
  float   fScaleFactor  = 0.0f;
  float   fTestValue    = 0.0f;
  float   fMax          = 0.0f;
  float   fTemp         = 0.0f;

  int     nPivotIdx     = 0;
  int     nSwapRow      = 0;
  int     nRow          = 0;
  int     i             = 0;

  // Perform maximal column pivoting, with the element on the diagonal
  // at matrix position (pivotIndex, pivotIndex).
  for (nPivotIdx = 0; nPivotIdx < 4; nPivotIdx++)
  {
    // First find out which unprocessed row has the largest value in the
    // pivot column and make it the next row to be pivoted.  Only look
    // at rows which haven't been done yet (from pivot row to the bottom).
    fMax      = (float)fabs(mtrxTmp(nPivotIdx, nPivotIdx));
    nSwapRow  = nPivotIdx;

    for (nRow = nPivotIdx + 1; nRow < 4; nRow++)
    {
      fTestValue = (float)fabs(mtrxTmp(nRow, nPivotIdx));
      if (fTestValue > fMax)
      {
        fMax      = fTestValue;
        nSwapRow  = nRow;
      }
    }

    if (nSwapRow != nPivotIdx)
    {
      // Exchange the row we found with the pivot row.  Remember
      // to do the same operation to the inverse matrix.

      for (i = 0; i < 4; i++)
      {
        fTemp                     = mtrxTmp(nSwapRow, i);
        mtrxTmp(nSwapRow, i)      = mtrxTmp(nPivotIdx, i);
        mtrxTmp(nPivotIdx, i)     = fTemp;

        fTemp                     = mtrxInverse(nSwapRow, i);
        mtrxInverse(nSwapRow, i)  = mtrxInverse(nPivotIdx, i);
        mtrxInverse(nPivotIdx, i) = fTemp;
      }
    }

    // Scale the pivot row so that it has a 1.0 in the pivot column.
    // Should already be all zeroes to the left of the pivot column,
    // but it isn't worth optimising since we still have to scale
    // the inverse.
    fMax = mtrxTmp(nPivotIdx, nPivotIdx);
    
    // Can't divide by zero.  Matrix doesn't have inverse
    if (0.0f == fMax)
      return(FALSE);

    fScaleFactor = 1.0f / fMax;

    for (i = 0; i < 4; i++)
    {
      mtrxTmp(nPivotIdx, i)     *= fScaleFactor;
      mtrxInverse(nPivotIdx, i) *= fScaleFactor;
    }

    mtrxTmp(nPivotIdx, nPivotIdx) = 1.0f; // Avoid roundoff error

    // Subtract the pivot row from all the rows below and above it so that
    // they all have zeroes in the pivot column.
    for (nRow = 0; nRow < 4; nRow++)
    {
      // Don't touch pivot row, it stays as 1.0 in the pivot column
      if (nRow == nPivotIdx) continue; 

      fScaleFactor = -mtrxTmp(nRow, nPivotIdx);
      
      // This row already is zero in the pivot column, nothing to do.
      if (fScaleFactor == 0.0F) continue; 

      for (i = 0; i < 4; i++)
      {
        mtrxTmp(nRow, i)      += mtrxTmp(nPivotIdx, i)      * fScaleFactor;
        mtrxInverse(nRow, i)  += mtrxInverse(nPivotIdx, i)  * fScaleFactor;
      }
      
      // Avoid roundoff error
      mtrxTmp(nRow, nPivotIdx) = 0.0f;
    }
  }

  // Make this matrix the inverse
  *this = mtrxInverse;

  return(TRUE);
}


// ===========================================================================
// Function:    TransformVectors
//
// Description: Transforms a number of vectors in place.
//
// Params:      aVectors:   Array of vectors to transform
//              nNum:       The number of vectors to transform
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Matrix::TransformVectors(D3DVECTOR* aVectors, int nNum) const
{
  ASSERT(nNum > 0);
  ASSERT(NULL != aVectors);
  for (int i = 0; i < nNum; i++)
    TransformVector(aVectors[i]);
}


// ===========================================================================
// Function:    TransVectors
//
// Description: Transforms a number of vectors using this matrix.
//
// Params:      aInVectors:   Array of input vectors
//              aOutVectors:  Array of output vectors (where the results will
//                            be stored)
//              nNum:         The number of vectors to transform
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Matrix::TransVectors(const D3DVECTOR* aInVectors,
                          D3DVECTOR* aOutVectors, int nNum) const
{
  ASSERT(nNum > 0);
  ASSERT(NULL != aInVectors);
  ASSERT(NULL != aOutVectors);
  for (int i = 0; i < nNum; i++)
    aOutVectors[i] = TransVector(aInVectors[i]);
}

// ===========================================================================
// End Of File ===============================================================
// ===========================================================================
