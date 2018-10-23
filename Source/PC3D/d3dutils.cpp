/*
**-----------------------------------------------------------------------------
** Name:    D3DUtils.cpp
** Purpose: Various D3D utility functions
** Notes:
**-----------------------------------------------------------------------------
*/

#include "PC3DHdr.h"


// ===========================================================================
// Global Functions ==========================================================
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
D3DMATRIX ViewMatrix(const D3DVECTOR& vFrom, const D3DVECTOR& vAt, 
                     const D3DVECTOR& vWorldUp, float fRoll /*= 0.0f*/)
{
  Matrix mtrxView(Matrix::IDENTITY);

  D3DVECTOR vViewDir  = Normalize(vAt - vFrom);
  D3DVECTOR vRight    = Normalize(CrossProduct(vWorldUp, vViewDir));
  D3DVECTOR vUp       = Normalize(CrossProduct(vViewDir, vRight));

  mtrxView._11 = vRight.x;
  mtrxView._21 = vRight.y;
  mtrxView._31 = vRight.z;
  mtrxView._12 = vUp.x;
  mtrxView._22 = vUp.y;
  mtrxView._32 = vUp.z;
  mtrxView._13 = vViewDir.x;
  mtrxView._23 = vViewDir.y;
  mtrxView._33 = vViewDir.z;

  mtrxView._41 = -DotProduct(vFrom, vRight);
  mtrxView._42 = -DotProduct(vFrom, vUp);
  mtrxView._43 = -DotProduct(vFrom, vViewDir);

  // Set roll
  if (WITHINTOLERANCE(fRoll))
    mtrxView.RotateZ(fRoll);

  return mtrxView;
}


// ===========================================================================
// Name:      RotateAxisMatrix 
// Purpose:   Rotate matrix about arbitrary axis (positive rotations are 
//            defined as counter-clockwise)
// ===========================================================================
//D3DMATRIX RotateAxisMatrix(D3DVECTOR& vDir, float fRads)
//{
//  // optimize for orthogonal axes
//  Matrix mtrx(Matrix::IDENTITY);
//
//  if (v.x == 0 && v.y == 0)
//    return(mtrx.RotateZ(v.z < 0 ? rads : -rads), mtrx);
//  if (v.x == 0 && v.z == 0)
//    return(mtrx.RotateY(v.y < 0 ? rads : -rads), mtrx);
//  if (v.y == 0 && v.z == 0)
//    return(mtrx.RotateX(v.x < 0 ? rads : -rads), mtrx);
//
//  D3DVECTOR n = Normalize(v);
//  
//  Matrix Rx(Matrix::IDENTITY);
//  Matrix Rxinv(Matrix::IDENTITY); 
//  Matrix Ry(Matrix::IDENTITY); 
//  Matrix Ryinv(Matrix::IDENTITY);
//
//  float d     = (float)sqrt(SQR(n.y) + SQR(n.z));
//  float cd    = n.z * d;
//  float bd    = n.y * d;
//  float a2pb2 = SQR(n.x) + SQR(n.y);
//  float a2pd2 = SQR(n.x) + SQR(d);
//
//  float c_d, b_d;
//
//  // only rotate around x if necessary
//  if (d) 
//  {
//    c_d = n.z / d; 
//    b_d = n.y / d;
//
//    Rx(1,1) = c_d;  
//    Rx(1,2) = b_d;  
//    Rx(2,1) = -b_d; 
//    Rx(2,2) = c_d;  
//    Rxinv = Rx.Inverse();
//  }
//
//  Ry(0,0) = d;  
//  Ry(0,2) = n.x;  
//  Ry(2,0) = -n.x; 
//  Ry(2,2) = d;  
//  Ryinv = Ry.Inverse();
//
//  Matrix temp;
//
//  if (d)
//    temp = Ryinv * Rxinv;
//  else
//    temp = Ryinv; // don't apply the x-rotation if not needed
//
//  temp.RotateZ(rads);
//  temp = Ry * temp;
//
//  // don't apply the x-rotation if not needed
//  if (d)
//    temp = Rx * temp; 
//
//  return temp;
//}


// Set the rotation part of a D3DMATRIX such that the vector lpD is the new
// z-axis and lpU is the new y-axis.
LPD3DMATRIX SetRotationMatrix(LPD3DMATRIX lpM, LPD3DVECTOR lpD, LPD3DVECTOR lpU)
{
  float t;
  D3DVECTOR d, u, r;

  // Normalise the direction vector.
  d = Normalize(*lpD);

  u.x = lpU->x;
  u.y = lpU->y;
  u.z = lpU->z;

  // Project u into the plane defined by d and normalise.
  t = u.x * d.x + u.y * d.y + u.z * d.z;
  u.x -= d.x * t;
  u.y -= d.y * t;
  u.z -= d.z * t;
  if (u.x == 0 && u.y == 0 && u.z == 0) 
  {
    // UP vector is in same dir as direction, so call recursively with a
    //  meaningful UP vector
    D3DVECTOR temp = d; 
    if (temp.x || temp.z)
      temp.y += 10;
    else
      temp.z += 10;
    return SetRotationMatrix( lpM, lpD, &temp);   
  }
  
  u = Normalize(u);

  // Calculate the vector pointing along the D3DMATRIX x axis (in a right
  // handed floatinate system) using cross product.
  r = CrossProduct(u, d);

  lpM->_11 = r.x;
  lpM->_12 = r.y, lpM->_13 = r.z;
  lpM->_21 = u.x;
  lpM->_22 = u.y, lpM->_23 = u.z;
  lpM->_31 = d.x;
  lpM->_32 = d.y;
  lpM->_33 = d.z;

  lpM->_14 = lpM->_24 = lpM->_34 = lpM->_41 = lpM->_42 = lpM->_43 = 0.0;
  lpM->_44 = 1.0;
  return lpM;
}


// ===========================================================================
// Function:    spline
//
// Description: Calculates a point along a B-Spline curve defined by four
//              points.
//
// Params:      p:              Output value used to store the computed point
//              t:              Position along the curve between p2 and p3.
//                              This position is a float between 0 and 1.
//              p1, p2, p3, p4: Points defining spline curve
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void spline(LPD3DVECTOR p, float t, LPD3DVECTOR p1, LPD3DVECTOR p2,
            LPD3DVECTOR p3, LPD3DVECTOR p4)
{
  double t2, t3;
  float m1, m2, m3, m4;

  t2 = (double)(t * t);
  t3 = t2 * (double)t;

  m1 = (float)((-1.0 * t3) + ( 2.0 * t2) + (-1.0 * (double)t));
  m2 = (float)(( 3.0 * t3) + (-5.0 * t2) + ( 0.0 * (double)t) + 2.0);
  m3 = (float)((-3.0 * t3) + ( 4.0 * t2) + ( 1.0 * (double)t));
  m4 = (float)(( 1.0 * t3) + (-1.0 * t2) + ( 0.0 * (double)t));

  m1 /= 2.0f;
  m2 /= 2.0f;
  m3 /= 2.0f;
  m4 /= 2.0f;

  p->x = p1->x * m1 + p2->x * m2 + p3->x * m3 + p4->x * m4;
  p->y = p1->y * m1 + p2->y * m2 + p3->y * m3 + p4->y * m4;
  p->z = p1->z * m1 + p2->z * m2 + p3->z * m3 + p4->z * m4;
}


// ===========================================================================
// End Of File ===============================================================
// ===========================================================================
