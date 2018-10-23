// ===========================================================================
// Module:      Matrix.inl
//
// Description: Inline functions of the Matrix class which represents a 4x4 
//              matrix.  Derived from the D3DMATRIX structure.
//
// Created:     May 21, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
//
// NOTES:       It should be pointed out that RotateX(), RotateY(), RotateZ()
//              and Translate() have all been optimized with the assumption
//              that the last column in the 4x4 matrix will always be 0,0,0,1.
// ===========================================================================

#ifndef INC_MATRIXINL
#define INC_MATRIXINL


// ===========================================================================
// INLINE: Matrix member functions ===========================================
// ===========================================================================

static Matrix g_mtrxTmp;
static float  g_fSin = 0.0f;
static float  g_fCos = 0.0f;

// ===========================================================================
INL void Matrix::ProjectionMatrix(float fNearPlane, float fFarPlane,
                                  float fFov, float fAspect /*= 1.0f*/)
{
  assert(WITHINTOLERANCE(fNearPlane - fFarPlane));
  assert(WITHINTOLERANCE(fFarPlane));
  Zero();
//  FLOAT w = fAspect * float(cos(fFov / 2) / sin(fFov / 2));
//  FLOAT h = (FLOAT)( cos(fFov/2)/sin(fFov/2) );
//  FLOAT Q = fFarPlane / ( fFarPlane - fNearPlane );
  g_fCos = float(cos(fFov * 0.5f) / sin(fFov * 0.5f));  // g_fCos used as temp value
  _11 = fAspect * g_fCos;                               // w
  _22 = g_fCos;                                         // h
  _33 = fFarPlane / (fFarPlane - fNearPlane);           // Q
  _34 = 1.0f;
  _43 = -_33 * fNearPlane;                              // -Q * fNearPlane
}

// ===========================================================================
INL void Matrix::RotateX(float fRads)
{
  g_fSin = (float)sin(fRads);
  g_fCos = (float)cos(fRads);
  g_mtrxTmp._21 = g_fCos * _21 + g_fSin * _31;
  g_mtrxTmp._22 = g_fCos * _22 + g_fSin * _32;
  g_mtrxTmp._23 = g_fCos * _23 + g_fSin * _33;
  _31 = g_fCos * _31 - g_fSin * _21;
  _32 = g_fCos * _32 - g_fSin * _22;
  _33 = g_fCos * _33 - g_fSin * _23;
  _21 = g_mtrxTmp._21;
  _22 = g_mtrxTmp._22;
  _23 = g_mtrxTmp._23;
}

// ===========================================================================
INL void Matrix::RotateY(float fRads)
{
  g_fSin = (float)sin(fRads);
  g_fCos = (float)cos(fRads);
  g_mtrxTmp._11 = g_fCos * _11 - g_fSin * _31;
  g_mtrxTmp._12 = g_fCos * _12 - g_fSin * _32;
  g_mtrxTmp._13 = g_fCos * _13 - g_fSin * _33;
  _31 = g_fCos * _31 + g_fSin * _11;
  _32 = g_fCos * _32 + g_fSin * _12;
  _33 = g_fCos * _33 + g_fSin * _13;
  _11 = g_mtrxTmp._11;
  _12 = g_mtrxTmp._12;
  _13 = g_mtrxTmp._13;
}

// ===========================================================================
INL void Matrix::RotateZ(float fRads)
{
  g_fSin = (float)sin(fRads);
  g_fCos = (float)cos(fRads);
  g_mtrxTmp._11 = g_fCos * _11 + g_fSin * _21;
  g_mtrxTmp._12 = g_fCos * _12 + g_fSin * _22;
  g_mtrxTmp._13 = g_fCos * _13 + g_fSin * _23;
  _21 = g_fCos * _21 - g_fSin * _11;
  _22 = g_fCos * _22 - g_fSin * _12;
  _23 = g_fCos * _23 - g_fSin * _13;
  _11 = g_mtrxTmp._11;
  _12 = g_mtrxTmp._12;
  _13 = g_mtrxTmp._13;
}

// ===========================================================================
INL void Matrix::Translate(float dx, float dy, float dz)
{
  _41 += dx;  _42 += dy;  _43 += dz;
}

// ===========================================================================
INL void Matrix::Translate(const D3DVECTOR& v)
{
  Translate(v.x, v.y, v.z);
}

// ===========================================================================
INL void Matrix::Scale(float sx, float sy, float sz)
{
  _11 *= sx;    _12 *= sy;    _13 *= sz;
  _21 *= sx;    _22 *= sy;    _23 *= sz;
  _31 *= sx;    _32 *= sy;    _33 *= sz;
  _41 *= sx;    _42 *= sy;    _43 *= sz;
}

// ===========================================================================
INL void Matrix::Scale(const D3DVECTOR& v)
{
  Scale(v.x, v.y, v.z);
}

// ===========================================================================
INL void Matrix::Scale(float fSize)
{
  Scale(fSize, fSize, fSize);
}

// ===========================================================================
INL void Matrix::Transpose()
{
  g_mtrxTmp = *this;
  // Upper Triangle       // Lower Triangle
  _12 = _21;              _21 = g_mtrxTmp._12;
  _13 = _31;              _31 = g_mtrxTmp._13;
  _14 = _41;              _41 = g_mtrxTmp._14;
  _23 = _32;              _32 = g_mtrxTmp._23;
  _24 = _42;              _42 = g_mtrxTmp._24;
  _34 = _43;              _43 = g_mtrxTmp._34;
}

// ===========================================================================
INL void Matrix::TransformVector(D3DVECTOR& v) const
{
  g_mtrxTmp._14 = v.x * _14 + v.y * _24 + v.z * _34 + _44;
  assert(WITHINTOLERANCE(g_mtrxTmp._14));
  g_mtrxTmp._11 = (v.x * _11 + v.y * _21 + v.z * _31 + _41) / g_mtrxTmp._14;
  g_mtrxTmp._12 = (v.x * _12 + v.y * _22 + v.z * _32 + _42) / g_mtrxTmp._14;
  g_mtrxTmp._13 = (v.x * _13 + v.y * _23 + v.z * _33 + _43) / g_mtrxTmp._14;
  CopyMemory(&v, &(g_mtrxTmp._11), sizeof(D3DVECTOR));
}

// ===========================================================================
INL D3DVECTOR Matrix::TransVector(const D3DVECTOR& v) const
{
  D3DVECTOR vResult(v);
  TransformVector(vResult);
  return(vResult);
}

// ===========================================================================
INL D3DMATRIX Matrix::Inverse() const
{
  Matrix mtrx(*this);
  mtrx.Invert();
  return(mtrx);
}

// ===========================================================================
INL void Matrix::Square()
{
  *this *= *this;
}

// ===========================================================================
INL D3DMATRIX Matrix::Squared() const
{
  return((*this) * (*this));
}

// ===========================================================================
// INLINE: Matrix operators ==================================================
// ===========================================================================


// ===========================================================================
INL BOOL Matrix::operator ==(const D3DMATRIX& mtrx) const
{
  return(0 == memcmp(this, &mtrx, sizeof(D3DMATRIX)));
}

// ===========================================================================
INL BOOL Matrix::operator !=(const D3DMATRIX& mtrx) const
{
  return(0 != memcmp(this, &mtrx, sizeof(D3DMATRIX)));
}

// ===========================================================================
INL D3DMATRIX Matrix::operator =(const D3DMATRIX& mtrx)
{ 
  CopyMemory(this, &mtrx, sizeof(D3DMATRIX)); return(*this);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator =(float fValue)
{
  _11 = _12 = _13 = _14 = _21 = _22 = _23 = _24 = fValue;
  _31 = _32 = _33 = _34 = _41 = _42 = _43 = _44 = fValue;
  return(*this);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator *(const D3DMATRIX& mtrx) const
{
  g_mtrxTmp._11 = _11*mtrx._11 + _12*mtrx._21 + _13*mtrx._31 + _14*mtrx._41;
  g_mtrxTmp._12 = _11*mtrx._12 + _12*mtrx._22 + _13*mtrx._32 + _14*mtrx._42;
  g_mtrxTmp._13 = _11*mtrx._13 + _12*mtrx._23 + _13*mtrx._33 + _14*mtrx._43;
  g_mtrxTmp._14 = _11*mtrx._14 + _12*mtrx._24 + _13*mtrx._34 + _14*mtrx._44;
  g_mtrxTmp._21 = _21*mtrx._11 + _22*mtrx._21 + _23*mtrx._31 + _24*mtrx._41;
  g_mtrxTmp._22 = _21*mtrx._12 + _22*mtrx._22 + _23*mtrx._32 + _24*mtrx._42;
  g_mtrxTmp._23 = _21*mtrx._13 + _22*mtrx._23 + _23*mtrx._33 + _24*mtrx._43;
  g_mtrxTmp._24 = _21*mtrx._14 + _22*mtrx._24 + _23*mtrx._34 + _24*mtrx._44;
  g_mtrxTmp._31 = _31*mtrx._11 + _32*mtrx._21 + _33*mtrx._31 + _34*mtrx._41;
  g_mtrxTmp._32 = _31*mtrx._12 + _32*mtrx._22 + _33*mtrx._32 + _34*mtrx._42;
  g_mtrxTmp._33 = _31*mtrx._13 + _32*mtrx._23 + _33*mtrx._33 + _34*mtrx._43;
  g_mtrxTmp._34 = _31*mtrx._14 + _32*mtrx._24 + _33*mtrx._34 + _34*mtrx._44;
  g_mtrxTmp._41 = _41*mtrx._11 + _42*mtrx._21 + _43*mtrx._31 + _44*mtrx._41;
  g_mtrxTmp._42 = _41*mtrx._12 + _42*mtrx._22 + _43*mtrx._32 + _44*mtrx._42;
  g_mtrxTmp._43 = _41*mtrx._13 + _42*mtrx._23 + _43*mtrx._33 + _44*mtrx._43;
  g_mtrxTmp._44 = _41*mtrx._14 + _42*mtrx._24 + _43*mtrx._34 + _44*mtrx._44;
  return(g_mtrxTmp);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator *(float fScalar) const
{
  g_mtrxTmp._11 = _11 * fScalar;  g_mtrxTmp._21 = _21 * fScalar;
  g_mtrxTmp._12 = _12 * fScalar;  g_mtrxTmp._22 = _22 * fScalar;
  g_mtrxTmp._13 = _13 * fScalar;  g_mtrxTmp._23 = _23 * fScalar;
  g_mtrxTmp._14 = _14 * fScalar;  g_mtrxTmp._24 = _24 * fScalar;

  g_mtrxTmp._31 = _31 * fScalar;  g_mtrxTmp._41 = _41 * fScalar;
  g_mtrxTmp._32 = _32 * fScalar;  g_mtrxTmp._42 = _42 * fScalar;
  g_mtrxTmp._33 = _33 * fScalar;  g_mtrxTmp._43 = _43 * fScalar;
  g_mtrxTmp._34 = _34 * fScalar;  g_mtrxTmp._44 = _44 * fScalar;
  return(g_mtrxTmp);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator *=(const D3DMATRIX& mtrx)
{ 
  *this = (*this) * mtrx; return(*this);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator *=(float fScalar)
{
  *this = (*this) * fScalar; return(*this);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator +(const D3DMATRIX& mtrx) const
{
  g_mtrxTmp._11 = _11 + mtrx._11;   g_mtrxTmp._21 = _21 + mtrx._21;
  g_mtrxTmp._12 = _12 + mtrx._12;   g_mtrxTmp._22 = _22 + mtrx._22;
  g_mtrxTmp._13 = _13 + mtrx._13;   g_mtrxTmp._23 = _23 + mtrx._23;
  g_mtrxTmp._14 = _14 + mtrx._14;   g_mtrxTmp._24 = _24 + mtrx._24;

  g_mtrxTmp._31 = _31 + mtrx._31;   g_mtrxTmp._41 = _41 + mtrx._41;
  g_mtrxTmp._32 = _32 + mtrx._32;   g_mtrxTmp._42 = _42 + mtrx._42;
  g_mtrxTmp._33 = _33 + mtrx._33;   g_mtrxTmp._43 = _43 + mtrx._43;
  g_mtrxTmp._34 = _34 + mtrx._34;   g_mtrxTmp._44 = _44 + mtrx._44;
  return(g_mtrxTmp);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator +(float fValue) const
{
  g_mtrxTmp._11 = _11 + fValue;   g_mtrxTmp._21 = _21 + fValue;
  g_mtrxTmp._12 = _12 + fValue;   g_mtrxTmp._22 = _22 + fValue;
  g_mtrxTmp._13 = _13 + fValue;   g_mtrxTmp._23 = _23 + fValue;
  g_mtrxTmp._14 = _14 + fValue;   g_mtrxTmp._24 = _24 + fValue;

  g_mtrxTmp._31 = _31 + fValue;   g_mtrxTmp._41 = _41 + fValue;
  g_mtrxTmp._32 = _32 + fValue;   g_mtrxTmp._42 = _42 + fValue;
  g_mtrxTmp._33 = _33 + fValue;   g_mtrxTmp._43 = _43 + fValue;
  g_mtrxTmp._34 = _34 + fValue;   g_mtrxTmp._44 = _44 + fValue;
  return(g_mtrxTmp);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator +=(const D3DMATRIX& mtrx)
{
  *this = (*this) + mtrx; return(*this);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator +=(float fValue)
{
  *this = (*this) + fValue; return(*this);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator -(const D3DMATRIX& mtrx) const
{
  g_mtrxTmp._11 = _11 - mtrx._11;   g_mtrxTmp._21 = _21 - mtrx._21;
  g_mtrxTmp._12 = _12 - mtrx._12;   g_mtrxTmp._22 = _22 - mtrx._22;
  g_mtrxTmp._13 = _13 - mtrx._13;   g_mtrxTmp._23 = _23 - mtrx._23;
  g_mtrxTmp._14 = _14 - mtrx._14;   g_mtrxTmp._24 = _24 - mtrx._24;

  g_mtrxTmp._31 = _31 - mtrx._31;   g_mtrxTmp._41 = _41 - mtrx._41;
  g_mtrxTmp._32 = _32 - mtrx._32;   g_mtrxTmp._42 = _42 - mtrx._42;
  g_mtrxTmp._33 = _33 - mtrx._33;   g_mtrxTmp._43 = _43 - mtrx._43;
  g_mtrxTmp._34 = _34 - mtrx._34;   g_mtrxTmp._44 = _44 - mtrx._44;
  return(g_mtrxTmp);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator -(float fValue) const
{
  g_mtrxTmp._11 = _11 - fValue;   g_mtrxTmp._21 = _21 - fValue;
  g_mtrxTmp._12 = _12 - fValue;   g_mtrxTmp._22 = _22 - fValue;
  g_mtrxTmp._13 = _13 - fValue;   g_mtrxTmp._23 = _23 - fValue;
  g_mtrxTmp._14 = _14 - fValue;   g_mtrxTmp._24 = _24 - fValue;

  g_mtrxTmp._31 = _31 - fValue;   g_mtrxTmp._41 = _41 - fValue;
  g_mtrxTmp._32 = _32 - fValue;   g_mtrxTmp._42 = _42 - fValue;
  g_mtrxTmp._33 = _33 - fValue;   g_mtrxTmp._43 = _43 - fValue;
  g_mtrxTmp._34 = _34 - fValue;   g_mtrxTmp._44 = _44 - fValue;
  return(g_mtrxTmp);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator -=(const D3DMATRIX& mtrx)
{
  *this = (*this) - mtrx; return(*this);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator -=(float fValue)
{
  *this = (*this) - fValue; return(*this);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator +() const
{
  g_mtrxTmp._11 = +_11;   g_mtrxTmp._21 = +_21;
  g_mtrxTmp._12 = +_12;   g_mtrxTmp._22 = +_22;
  g_mtrxTmp._13 = +_13;   g_mtrxTmp._23 = +_23;
  g_mtrxTmp._14 = +_14;   g_mtrxTmp._24 = +_24;

  g_mtrxTmp._31 = +_31;   g_mtrxTmp._41 = +_41;
  g_mtrxTmp._32 = +_32;   g_mtrxTmp._42 = +_42;
  g_mtrxTmp._33 = +_33;   g_mtrxTmp._43 = +_43;
  g_mtrxTmp._34 = +_34;   g_mtrxTmp._44 = +_44;
  return(g_mtrxTmp);
}

// ===========================================================================
INL D3DMATRIX Matrix::operator -() const
{
  g_mtrxTmp._11 = -_11;   g_mtrxTmp._21 = -_21;
  g_mtrxTmp._12 = -_12;   g_mtrxTmp._22 = -_22;
  g_mtrxTmp._13 = -_13;   g_mtrxTmp._23 = -_23;
  g_mtrxTmp._14 = -_14;   g_mtrxTmp._24 = -_24;

  g_mtrxTmp._31 = -_31;   g_mtrxTmp._41 = -_41;
  g_mtrxTmp._32 = -_32;   g_mtrxTmp._42 = -_42;
  g_mtrxTmp._33 = -_33;   g_mtrxTmp._43 = -_43;
  g_mtrxTmp._34 = -_34;   g_mtrxTmp._44 = -_44;
  return(g_mtrxTmp);
}


#endif  // INC_MATRIXINL
// ===========================================================================
