#ifndef D3DUTILS_H
#define D3DUTILS_H
/*
**-----------------------------------------------------------------------------
** Name:    D3DUtils.h
** Purpose: Various D3D utility functions
** Notes:
**
**-----------------------------------------------------------------------------
*/

#define SQR(x)                    ((x) * (x))


// ===========================================================================
// Global Function Prototypes ================================================
// ===========================================================================

D3DMATRIX ViewMatrix(const D3DVECTOR& from, const D3DVECTOR& at, 
                     const D3DVECTOR& up, float fRoll = 0.0f);

INL D3DMATRIX MatrixMult(const D3DMATRIX& a, const D3DMATRIX& b)
{
  D3DMATRIX ret;
  ret._11 = a._11 * b._11 + a._12 * b._21 + a._13 * b._31 + a._14 * b._41;
  ret._12 = a._11 * b._12 + a._12 * b._22 + a._13 * b._32 + a._14 * b._42;
  ret._13 = a._11 * b._13 + a._12 * b._23 + a._13 * b._33 + a._14 * b._43;
  ret._14 = a._11 * b._14 + a._12 * b._24 + a._13 * b._34 + a._14 * b._44;
  ret._21 = a._21 * b._11 + a._22 * b._21 + a._23 * b._31 + a._24 * b._41;
  ret._22 = a._21 * b._12 + a._22 * b._22 + a._23 * b._32 + a._24 * b._42;
  ret._23 = a._21 * b._13 + a._22 * b._23 + a._23 * b._33 + a._24 * b._43;
  ret._24 = a._21 * b._14 + a._22 * b._24 + a._23 * b._34 + a._24 * b._44;
  ret._31 = a._31 * b._11 + a._32 * b._21 + a._33 * b._31 + a._34 * b._41;
  ret._32 = a._31 * b._12 + a._32 * b._22 + a._33 * b._32 + a._34 * b._42;
  ret._33 = a._31 * b._13 + a._32 * b._23 + a._33 * b._33 + a._34 * b._43;
  ret._34 = a._31 * b._14 + a._32 * b._24 + a._33 * b._34 + a._34 * b._44;
  ret._41 = a._41 * b._11 + a._42 * b._21 + a._43 * b._31 + a._44 * b._41;
  ret._42 = a._41 * b._12 + a._42 * b._22 + a._43 * b._32 + a._44 * b._42;
  ret._43 = a._41 * b._13 + a._42 * b._23 + a._43 * b._33 + a._44 * b._43;
  ret._44 = a._41 * b._14 + a._42 * b._24 + a._43 * b._34 + a._44 * b._44;
  return(ret);
}

// Rotate matrix about arbitrary axis (positive rotations are
//  counter-clockwise)
//D3DMATRIX RotateAxisMatrix(D3DVECTOR& vDir, float fRads);

// Set the rotation part of a D3DMATRIX such that the vector lpD is the new
// z-axis and lpU is the new y-axis.
LPD3DMATRIX SetRotationMatrix(LPD3DMATRIX lpM, LPD3DVECTOR lpD,
                              LPD3DVECTOR lpU);


/*
 * Calculates a point along a B-Spline curve defined by four points. p
 * n output, contain the point. t                                Position
 * along the curve between p2 and p3.  This position is a float between 0
 * and 1. p1, p2, p3, p4    Points defining spline curve. p, at parameter
 * t along the spline curve
 */
void spline(LPD3DVECTOR p, float t, LPD3DVECTOR p1, LPD3DVECTOR p2,
            LPD3DVECTOR p3, LPD3DVECTOR p4);



#endif // D3DUTILS_H
// ===========================================================================


