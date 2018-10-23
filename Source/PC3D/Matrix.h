// ===========================================================================
// Module:      Matrix.h
//
// Description: Definition of the Matrix class which represents a 4x4 matrix.
//              Derived from the D3DMATRIX structure.
//
// Created:     May 17, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_MATRIX
#define INC_MATRIX

#ifndef INL
#define INL                       inline
#endif // INL

#ifndef TOLERANCE
#define TOLERANCE                 0.000001f
#endif // TOLERANCE

#ifndef WITHINTOLERANCE
#define WITHINTOLERANCE(value)    (TOLERANCE < fabs(value))
#endif // WITHINTOLERANCE


extern const D3DMATRIX  ZEROMATRIX;
extern const D3DMATRIX  IDENTITYMATRIX;


// ===========================================================================
// Matrix class ==============================================================
// ===========================================================================
class Matrix : public D3DMATRIX
{
  // Construction / Destruction ==============================================
  public:
    typedef enum { ZERO, IDENTITY } MATRIXTYPE;

    Matrix(MATRIXTYPE eType = ZERO) { if (IDENTITY) Identity(); else Zero(); }
    Matrix(const D3DMATRIX& mtrx)   { *this = mtrx;                          }


  // Private member functions ================================================
  private:


  // Protected member functions ==============================================
  protected:


  // Public member functions =================================================
  public:
    void Zero()     { ZeroMemory(this, sizeof(D3DMATRIX));                  }
    void Identity() { CopyMemory(this, &IDENTITYMATRIX, sizeof(D3DMATRIX)); }

    INL BOOL IsZero()     const   { return(*this == ZEROMATRIX);            }
    INL BOOL IsIdentity() const   { return(*this == IDENTITYMATRIX);        }

    INL void ProjectionMatrix(float fNearPlane, float fFarPlane, float fFov,
                              float fAspect = 1.0f);
        void ViewMatrix(const D3DVECTOR& vFrom, const D3DVECTOR& vAt, 
                        const D3DVECTOR& vWorldUp, float fRoll = 0.0f);

    static D3DMATRIX RotateAxisMatrix(const D3DVECTOR& vAxis, float fRads);

    INL void RotateX(float fRads);
    INL void RotateY(float fRads);
    INL void RotateZ(float fRads);
        void RotateAxis(const D3DVECTOR& vAxis, float fRads);

    INL void Translate(float dx, float dy, float dz);
    INL void Translate(const D3DVECTOR& v);

    INL void Scale(float sx, float sy, float sz);
    INL void Scale(const D3DVECTOR& v);
    INL void Scale(float fSize);

    INL void Transpose();

        BOOL      Invert();
    INL D3DMATRIX Inverse() const;

    INL void      TransformVector(D3DVECTOR& v) const;
    INL D3DVECTOR TransVector(const D3DVECTOR& v) const;

        void TransformVectors(D3DVECTOR* aVectors, int nNum) const;
        void TransVectors(const D3DVECTOR* aInVectors,
                          D3DVECTOR* aOutVectors, int nNum) const;

    INL void      Square();
    INL D3DMATRIX Squared() const;

    // Operators =============================================================
    INL BOOL      operator ==(const D3DMATRIX& mtrx)  const;
    INL BOOL      operator !=(const D3DMATRIX& mtrx)  const;
    INL D3DMATRIX operator  =(const D3DMATRIX& mtrx);
    INL D3DMATRIX operator  =(float fValue);

    INL D3DMATRIX operator  *(const D3DMATRIX& mtrx)  const;
    INL D3DMATRIX operator  *(float fScalar)          const;
    INL D3DMATRIX operator *=(const D3DMATRIX& mtrx);
    INL D3DMATRIX operator *=(float fScalar);

    INL D3DMATRIX operator  +(const D3DMATRIX& mtrx)  const;
    INL D3DMATRIX operator  +(float fValue)           const;
    INL D3DMATRIX operator +=(const D3DMATRIX& mtrx);
    INL D3DMATRIX operator +=(float fValue);

    INL D3DMATRIX operator  -(const D3DMATRIX& mtrx)  const;
    INL D3DMATRIX operator  -(float fValue)           const;
    INL D3DMATRIX operator -=(const D3DMATRIX& mtrx);
    INL D3DMATRIX operator -=(float fValue);

    INL D3DMATRIX operator +() const;
    INL D3DMATRIX operator -() const;
};

#include "Matrix.inl"

#endif // INC_MATRIX
// ===========================================================================
