/*****************************************************************************
 *
 *   FILE:        L_Matrix.c
 *   DESCRIPTION: Some matrix manipulation code for matrices that represent
 *                2D and 3D transformation operations (the matrices are 3D
 *                and 4D since they also handle translations by using an
 *                extra dimension and homogeneous coordinates).
 *
 *   (C) Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Matrix.cpp 7     4/20/99 12:10p Andrewx $
 ****************************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemMatrix


/************************************************************/
/* PRIVATE DATA TYPE DECLARATIONS                           */
/*----------------------------------------------------------*/

/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

/************************************************************/
/* STATIC VARIABLES                                         */
/*----------------------------------------------------------*/


/*****************************************************************************
 * Set a matrix to all zero.
 */

void LE_MATRIX_2D_SetZero (TYPE_Matrix2DPointer MatrixPntr)
{
  MatrixPntr->_11 = MatrixPntr->_12 = MatrixPntr->_13 =
  MatrixPntr->_21 = MatrixPntr->_22 = MatrixPntr->_23 =
  MatrixPntr->_31 = MatrixPntr->_32 = MatrixPntr->_33 = 0.0F;
}



/*****************************************************************************
 * Set a matrix to all zero.
 */

void LE_MATRIX_3D_SetZero (TYPE_Matrix3DPointer MatrixPntr)
{
  MatrixPntr->_11 = MatrixPntr->_12 = MatrixPntr->_13 = MatrixPntr->_14 =
  MatrixPntr->_21 = MatrixPntr->_22 = MatrixPntr->_23 = MatrixPntr->_24 =
  MatrixPntr->_31 = MatrixPntr->_32 = MatrixPntr->_33 = MatrixPntr->_34 =
  MatrixPntr->_41 = MatrixPntr->_42 = MatrixPntr->_43 = MatrixPntr->_44 = 0.0F;
}



/*****************************************************************************
 * Set a matrix to the identity matrix.
 */

void LE_MATRIX_2D_SetIdentity (TYPE_Matrix2DPointer MatrixPntr)
{
  MatrixPntr->_12 = MatrixPntr->_13 =
  MatrixPntr->_21 = MatrixPntr->_23 =
  MatrixPntr->_31 = MatrixPntr->_32 = 0.0F;

  MatrixPntr->_11 = MatrixPntr->_22 = MatrixPntr->_33 = 1.0F;
}



/*****************************************************************************
 * Set a matrix to the identity matrix.
 */

void LE_MATRIX_3D_SetIdentity (TYPE_Matrix3DPointer MatrixPntr)
{
  MatrixPntr->_12 = MatrixPntr->_13 = MatrixPntr->_14 =
  MatrixPntr->_21 = MatrixPntr->_23 = MatrixPntr->_24 =
  MatrixPntr->_31 = MatrixPntr->_32 = MatrixPntr->_34 =
  MatrixPntr->_41 = MatrixPntr->_42 = MatrixPntr->_43 = 0.0F;

  MatrixPntr->_11 = MatrixPntr->_22 = MatrixPntr->_33 = MatrixPntr->_44 = 1.0F;
}



/*****************************************************************************
 * Set the matrix to one that does a translation (moves the distance and
 * direction given in the point).
 */

void LE_MATRIX_2D_SetTranslate (TYPE_Point2DPointer PointPntr,
TYPE_Matrix2DPointer MatrixPntr)
{
  MatrixPntr->_31 = (TYPE_2DMatrixValue) PointPntr->x;
  MatrixPntr->_32 = (TYPE_2DMatrixValue) PointPntr->y;

  MatrixPntr->_12 = MatrixPntr->_13 =
  MatrixPntr->_21 = MatrixPntr->_23 = 0.0F;

  MatrixPntr->_11 = MatrixPntr->_22 = MatrixPntr->_33 = 1.0F;
}



/*****************************************************************************
 * Set the matrix to one that does a translation (moves the distance and
 * direction given in the point).
 */

void LE_MATRIX_3D_SetTranslate (TYPE_Point3DPointer PointPntr,
TYPE_Matrix3DPointer MatrixPntr)
{
  MatrixPntr->_41 = PointPntr->x;
  MatrixPntr->_42 = PointPntr->y;
  MatrixPntr->_43 = PointPntr->z;

  MatrixPntr->_12 = MatrixPntr->_13 = MatrixPntr->_14 =
  MatrixPntr->_21 = MatrixPntr->_23 = MatrixPntr->_24 =
  MatrixPntr->_31 = MatrixPntr->_32 = MatrixPntr->_34 = 0.0F;

  MatrixPntr->_11 = MatrixPntr->_22 = MatrixPntr->_33 = MatrixPntr->_44 = 1.0F;
}



/*****************************************************************************
 * Set the matrix to the one for a 2D rotation.  Positive goes clockwise
 * when looking at the screen from in front of the screen.  The coordinate
 * system for 2D has X increasing to the right and Y increasing downwards
 * (upside down from the 3D world, but then the 3D rotation around Z would be
 * from looking from behind the screen, so it ends up the same matrix).
 */

void LE_MATRIX_2D_SetRotate (TYPE_Angle2D Angle,
TYPE_Matrix2DPointer MatrixPntr)
{
  MatrixPntr->_21 = -(MatrixPntr->_12 = (TYPE_2DMatrixValue) sin (Angle));
  MatrixPntr->_11 =   MatrixPntr->_22 = (TYPE_2DMatrixValue) cos (Angle);

  MatrixPntr->_13 =
  MatrixPntr->_23 =
  MatrixPntr->_31 = MatrixPntr->_32 = 0.0F;

  MatrixPntr->_33 = 1.0F;
}



/*****************************************************************************
 * Set the matrix to the one for a 3D rotation around the X axis.  Positive
 * goes clockwise when looking from +X to the origin.  Left handed coordinate
 * system assumed (+Z goes into the screen).
 */

void LE_MATRIX_3D_SetRotateX (TYPE_Angle3D Angle, TYPE_Matrix3DPointer MatrixPntr)
{
  MatrixPntr->_22 =   MatrixPntr->_33 = (TYPE_3DMatrixValue) cos (Angle);
  MatrixPntr->_32 = -(MatrixPntr->_23 = (TYPE_3DMatrixValue) sin (Angle));

  MatrixPntr->_12 = MatrixPntr->_13 = MatrixPntr->_14 =
  MatrixPntr->_21 = MatrixPntr->_24 =
  MatrixPntr->_31 = MatrixPntr->_34 =
  MatrixPntr->_41 = MatrixPntr->_42 = MatrixPntr->_43 = 0.0F;

  MatrixPntr->_11 = MatrixPntr->_44 = 1.0F;
}



/*****************************************************************************
 * Set the matrix to the one for a 3D rotation around the Y axis.  Positive
 * goes clockwise when looking from +Y to the origin.  Left handed coordinate
 * system assumed (+Z goes into the screen).
 */

void LE_MATRIX_3D_SetRotateY (TYPE_Angle3D Angle, TYPE_Matrix3DPointer MatrixPntr)
{
  MatrixPntr->_11 =   MatrixPntr->_33 = (TYPE_3DMatrixValue) cos (Angle);
  MatrixPntr->_13 = -(MatrixPntr->_31 = (TYPE_3DMatrixValue) sin (Angle));

  MatrixPntr->_12 = MatrixPntr->_14 =
  MatrixPntr->_21 = MatrixPntr->_23 = MatrixPntr->_24 =
  MatrixPntr->_32 = MatrixPntr->_34 =
  MatrixPntr->_41 = MatrixPntr->_42 = MatrixPntr->_43 = 0.0F;

  MatrixPntr->_22 = MatrixPntr->_44 = 1.0F;
}



/*****************************************************************************
 * Set the matrix to the one for a 3D rotation around the Z axis.  Positive
 * goes clockwise when looking from +Z to the origin.  Left handed coordinate
 * system assumed (+Z goes into the screen).
 */

void LE_MATRIX_3D_SetRotateZ (TYPE_Angle3D Angle, TYPE_Matrix3DPointer MatrixPntr)
{
  MatrixPntr->_11 =   MatrixPntr->_22 = (TYPE_3DMatrixValue) cos (Angle);
  MatrixPntr->_21 = -(MatrixPntr->_12 = (TYPE_3DMatrixValue) sin (Angle));

  MatrixPntr->_13 = MatrixPntr->_14 =
  MatrixPntr->_23 = MatrixPntr->_24 =
  MatrixPntr->_31 = MatrixPntr->_32 = MatrixPntr->_34 =
  MatrixPntr->_41 = MatrixPntr->_42 = MatrixPntr->_43 = 0.0F;

  MatrixPntr->_33 = MatrixPntr->_44 = 1.0F;
}



/*****************************************************************************
 * Set the matrix to one that shrinks or enlarges things.  1.0 means keep
 * it the same, fractions < 1.0 shrink, > 1.0 enlarges.  Try to avoid using
 * negative values since that turns things inside out.
 */

void LE_MATRIX_2D_SetScale (TYPE_Scale2D ScaleFactor,
TYPE_Matrix2DPointer MatrixPntr)
{
  MatrixPntr->_11 = MatrixPntr->_22 = (TYPE_2DMatrixValue) ScaleFactor;

  MatrixPntr->_12 = MatrixPntr->_13 =
  MatrixPntr->_21 = MatrixPntr->_23 =
  MatrixPntr->_31 = MatrixPntr->_32 = 0.0F;

  MatrixPntr->_33 = 1.0F;
}



/*****************************************************************************
 * Set the matrix to one that shrinks or enlarges things by different amounts
 * in different dimensions.  Useful for squash and stretch animation effects.
 */

void LE_MATRIX_2D_SetScaleXY (TYPE_Scale2D ScaleFactorX,
TYPE_Scale2D ScaleFactorY, TYPE_Matrix2DPointer MatrixPntr)
{
  MatrixPntr->_11 = (TYPE_2DMatrixValue) ScaleFactorX;
  MatrixPntr->_22 = (TYPE_2DMatrixValue) ScaleFactorY;

  MatrixPntr->_12 = MatrixPntr->_13 =
  MatrixPntr->_21 = MatrixPntr->_23 =
  MatrixPntr->_31 = MatrixPntr->_32 = 0.0F;

  MatrixPntr->_33 = 1.0F;
}



/*****************************************************************************
 * Set the matrix to one that shrinks or enlarges things.  1.0 means keep
 * it the same, fractions < 1.0 shrink, > 1.0 enlarges.  Try to avoid using
 * negative values since that turns things inside out.
 */

void LE_MATRIX_3D_SetScale (TYPE_Scale3D ScaleFactor,
TYPE_Matrix3DPointer MatrixPntr)
{
  MatrixPntr->_11 = MatrixPntr->_22 = MatrixPntr->_33 = (TYPE_3DMatrixValue) ScaleFactor;

  MatrixPntr->_12 = MatrixPntr->_13 = MatrixPntr->_14 =
  MatrixPntr->_21 = MatrixPntr->_23 = MatrixPntr->_24 =
  MatrixPntr->_31 = MatrixPntr->_32 = MatrixPntr->_34 =
  MatrixPntr->_41 = MatrixPntr->_42 = MatrixPntr->_43 = 0.0F;

  MatrixPntr->_44 = 1.0F;
}



/*****************************************************************************
 * Set the matrix to one that shrinks or enlarges things by different amounts
 * in different dimensions.  Useful for squash and stretch animation effects.
 */

void LE_MATRIX_3D_SetScaleXYZ (TYPE_Scale3D ScaleFactorX,
TYPE_Scale3D ScaleFactorY, TYPE_Scale3D ScaleFactorZ,
TYPE_Matrix3DPointer MatrixPntr)
{
  MatrixPntr->_11 = (TYPE_3DMatrixValue) ScaleFactorX;
  MatrixPntr->_22 = (TYPE_3DMatrixValue) ScaleFactorY;
  MatrixPntr->_33 = (TYPE_3DMatrixValue) ScaleFactorZ;

  MatrixPntr->_12 = MatrixPntr->_13 = MatrixPntr->_14 =
  MatrixPntr->_21 = MatrixPntr->_23 = MatrixPntr->_24 =
  MatrixPntr->_31 = MatrixPntr->_32 = MatrixPntr->_34 =
  MatrixPntr->_41 = MatrixPntr->_42 = MatrixPntr->_43 = 0.0F;

  MatrixPntr->_44 = 1.0F;
}



/*****************************************************************************
 * Set the matrix to one that first scales the item then rotates it
 * and finally moves it around.
 */

void LE_MATRIX_2D_SetScaleRotateTranslate (TYPE_Scale2D ScaleFactorX,
TYPE_Scale2D ScaleFactorY, TYPE_Angle2D Rotate,
TYPE_Point2DPointer TranslatePntr, TYPE_Matrix2DPointer MatrixPntr)
{
  TYPE_Matrix2D Temp2DMatrixA;
  TYPE_Matrix2D Temp2DMatrixB;
  TYPE_Matrix2D Temp2DMatrixC;

  LE_MATRIX_2D_SetScaleXY (ScaleFactorX, ScaleFactorY, &Temp2DMatrixA);

  LE_MATRIX_2D_SetRotate (Rotate, &Temp2DMatrixB);
  LE_MATRIX_2D_MatrixMultiply (&Temp2DMatrixA, &Temp2DMatrixB, &Temp2DMatrixC);

  LE_MATRIX_2D_SetTranslate (TranslatePntr, &Temp2DMatrixA);
  LE_MATRIX_2D_MatrixMultiply (&Temp2DMatrixC, &Temp2DMatrixA, MatrixPntr);
}



/*****************************************************************************
 * Set the matrix to one that first scales the item then rotates it
 * and finally moves it around.
 *
 * Scale - Stretches the thing in various directions, 1.0 for no stretch.
 * Roll  - Sides of a boat going up and down.
 * Pitch - Bow and stern of the boat going up and down.
 * Yaw   - Compass heading that the boat is pointing towards.
 * Translate - Move the whole mess over by the given position delta.
 *
 * In 3D the character is facing forwards along the positive Z axis
 * (yaw zero by definition), so you see the character's back when he
 * is in its default orientation.
 */

void LE_MATRIX_3D_SetScaleRotateTranslate (TYPE_Scale3D ScaleFactorX,
TYPE_Scale3D ScaleFactorY, TYPE_Scale3D ScaleFactorZ,
TYPE_Angle3D Yaw, TYPE_Angle3D Pitch, TYPE_Angle3D Roll,
TYPE_Point3DPointer TranslatePntr, TYPE_Matrix3DPointer MatrixPntr)
{
  TYPE_Matrix3D Temp3DMatrixA;
  TYPE_Matrix3D Temp3DMatrixB;
  TYPE_Matrix3D Temp3DMatrixC;
  
  LE_MATRIX_3D_SetScaleXYZ (ScaleFactorX, ScaleFactorY, ScaleFactorZ,
    &Temp3DMatrixA);

  LE_MATRIX_3D_SetRotateZ (Roll, &Temp3DMatrixB);
  LE_MATRIX_3D_MatrixMultiply (&Temp3DMatrixA, &Temp3DMatrixB, &Temp3DMatrixC);

  LE_MATRIX_3D_SetRotateX (Pitch, &Temp3DMatrixA);
  LE_MATRIX_3D_MatrixMultiply (&Temp3DMatrixC, &Temp3DMatrixA, &Temp3DMatrixB);

  LE_MATRIX_3D_SetRotateY (Yaw, &Temp3DMatrixA);
  LE_MATRIX_3D_MatrixMultiply (&Temp3DMatrixB, &Temp3DMatrixA, &Temp3DMatrixC);

  LE_MATRIX_3D_SetTranslate (TranslatePntr, &Temp3DMatrixA);
  LE_MATRIX_3D_MatrixMultiply (&Temp3DMatrixC, &Temp3DMatrixA, MatrixPntr);
}



/*****************************************************************************
 * Multiply a vector (a point in other words) by a matrix.  The result is
 * a transformed point.  Homogeneous coordinates are hidden from the caller.
 * The multiplication is equivalent to one manually done with the vector
 * horizontal on the left of the matrix.  The input and output point can
 * be the same piece of memory.
 */

void LE_MATRIX_2D_VectorMultiply (TYPE_Point2DPointer InPointPntr,
TYPE_Matrix2DPointer MatrixPntr, TYPE_Point2DPointer OutPointPntr)
{
  TYPE_2DMatrixValue  NewW;
  TYPE_2DMatrixValue  NewX;
  TYPE_2DMatrixValue  NewY;

  TYPE_2DMatrixValue  OldX;
  TYPE_2DMatrixValue  OldY;

  OldX = (TYPE_2DMatrixValue) InPointPntr->x;
  OldY = (TYPE_2DMatrixValue) InPointPntr->y;

  NewX = MatrixPntr->_11 * OldX + MatrixPntr->_21 * OldY + MatrixPntr->_31;
  NewY = MatrixPntr->_12 * OldX + MatrixPntr->_22 * OldY + MatrixPntr->_32;
  NewW = MatrixPntr->_13 * OldX + MatrixPntr->_23 * OldY + MatrixPntr->_33;

  if (NewW == 0.0F)
    OutPointPntr->x = OutPointPntr->y = (TYPE_Coord2D) 0;
  else // Note: this uses the rounding mode set in LI_MATRIX_InitSystem.
  {
    OutPointPntr->x = (TYPE_Coord2D) (NewX / NewW);
    OutPointPntr->y = (TYPE_Coord2D) (NewY / NewW);
  }
}



/*****************************************************************************
 * Multiply a vector (a point in other words) by a matrix.  The result is
 * a transformed point.  Homogeneous coordinates are hidden from the caller.
 * The multiplication is equivalent to one manually done with the vector
 * horizontal on the left of the matrix.  The input and output point can
 * be the same piece of memory.
 */

void LE_MATRIX_3D_VectorMultiply (TYPE_Point3DPointer InPointPntr,
TYPE_Matrix3DPointer MatrixPntr, TYPE_Point3DPointer OutPointPntr)
{
  TYPE_3DMatrixValue  NewW;
  TYPE_3DMatrixValue  NewX;
  TYPE_3DMatrixValue  NewY;
  TYPE_3DMatrixValue  NewZ;

  TYPE_3DMatrixValue  OldX;
  TYPE_3DMatrixValue  OldY;
  TYPE_3DMatrixValue  OldZ;

  OldX = (TYPE_3DMatrixValue) InPointPntr->x;
  OldY = (TYPE_3DMatrixValue) InPointPntr->y;
  OldZ = (TYPE_3DMatrixValue) InPointPntr->z;

  NewX = MatrixPntr->_11 * OldX + MatrixPntr->_21 * OldY + MatrixPntr->_31 * OldZ + MatrixPntr->_41;
  NewY = MatrixPntr->_12 * OldX + MatrixPntr->_22 * OldY + MatrixPntr->_32 * OldZ + MatrixPntr->_42;
  NewZ = MatrixPntr->_13 * OldX + MatrixPntr->_23 * OldY + MatrixPntr->_33 * OldZ + MatrixPntr->_43;
  NewW = MatrixPntr->_14 * OldX + MatrixPntr->_24 * OldY + MatrixPntr->_34 * OldZ + MatrixPntr->_44;

  if (NewW == 0.0F)
    OutPointPntr->x = OutPointPntr->y = OutPointPntr->z = (TYPE_Coord3D) 0;
  else
  {
    OutPointPntr->x = (TYPE_Coord3D) (NewX / NewW);
    OutPointPntr->y = (TYPE_Coord3D) (NewY / NewW);
    OutPointPntr->z = (TYPE_Coord3D) (NewZ / NewW);
  }
}



/*****************************************************************************
 * Multiply matrix A by matrix B and put the result in the result matrix.
 * This is equivalent to doing the transformation represented by A followed
 * by the transformation represented by B.  Note that the result matrix
 * should not be A or B otherwise it won't work.
 */

void LE_MATRIX_2D_MatrixMultiply (TYPE_Matrix2DPointer MatrixAPntr,
TYPE_Matrix2DPointer MatrixBPntr, TYPE_Matrix2DPointer ResultPntr)
{
  typedef TYPE_2DMatrixValue (*MatrixAsArrayPointer)[3];

  MatrixAsArrayPointer A, B, R;

  int Column;
  int Row;

  A = (MatrixAsArrayPointer) MatrixAPntr;
  B = (MatrixAsArrayPointer) MatrixBPntr;
  R = (MatrixAsArrayPointer) ResultPntr;

  for (Row = 0; Row < 3; Row++)
  {
    for (Column = 0; Column < 3; Column++)
    {
      R[Row][Column] =
        A[Row][0] * B[0][Column] +
        A[Row][1] * B[1][Column] +
        A[Row][2] * B[2][Column];
    }
  }
}



/*****************************************************************************
 * Multiply matrix A by matrix B and put the result in the result matrix.
 * This is equivalent to doing the transformation represented by A followed
 * by the transformation represented by B.  Note that the result matrix
 * should not be A or B otherwise it won't work.
 */

void LE_MATRIX_3D_MatrixMultiply (TYPE_Matrix3DPointer MatrixAPntr,
TYPE_Matrix3DPointer MatrixBPntr, TYPE_Matrix3DPointer ResultPntr)
{
  typedef TYPE_3DMatrixValue (*MatrixAsArrayPointer)[4];

  MatrixAsArrayPointer A, B, R;

  int Column;
  int Row;

  A = (MatrixAsArrayPointer) MatrixAPntr;
  B = (MatrixAsArrayPointer) MatrixBPntr;
  R = (MatrixAsArrayPointer) ResultPntr;

  for (Row = 0; Row < 4; Row++)
  {
    for (Column = 0; Column < 4; Column++)
    {
      R[Row][Column] =
        A[Row][0] * B[0][Column] +
        A[Row][1] * B[1][Column] +
        A[Row][2] * B[2][Column] +
        A[Row][3] * B[3][Column];
    }
  }
}



/*****************************************************************************
 * Find the inverse of a matrix.  This is useful for undoing transformations.
 * For example, given a location in the world, find out where this is inside
 * a transformed object.  This is done for mapping mouse clicks back into
 * a location in a model.  The output matrix can be the same as the input.
 * Returns TRUE if successful, FALSE if no inverse exists or something else
 * is wrong.
 *
 * The algorithm works by reducing a copy of the matrix to the
 * identity matrix while doing the same manipulations on an identity
 * matrix to create the inverse.
 */

BOOL LE_MATRIX_2D_MatrixInverse (TYPE_Matrix2DPointer MatrixPntr,
TYPE_Matrix2DPointer InversePntr)
{
  typedef TYPE_2DMatrixValue (*MatrixAsArrayPointer)[3];

  int                   EliminationRow;
  MatrixAsArrayPointer  I;
  MatrixAsArrayPointer  M;
  TYPE_Matrix2D         MatrixCopy;
  TYPE_2DMatrixValue    MaxValue;
  int                   PivotIndex;
  TYPE_2DMatrixValue    ScaleFactor;
  int                   SwapRow;
  int                   TempIndex;
  TYPE_2DMatrixValue    TempValue;
  int                   TestRow;
  TYPE_2DMatrixValue    TestValue;

  MatrixCopy = *MatrixPntr;
  M = (MatrixAsArrayPointer) &MatrixCopy;
  LE_MATRIX_2D_SetIdentity (InversePntr);
  I = (MatrixAsArrayPointer) InversePntr;

  // Perform maximal column pivoting, with the element on the diagonal
  // at matrix position (pivotIndex, pivotIndex).

  for (PivotIndex = 0; PivotIndex < 3; PivotIndex++)
  {
    // First find out which unprocessed row has the largest value in the
    // pivot column and make it the next row to be pivoted.  Only look
    // at rows which haven't been done yet (from pivot row to the bottom).

    MaxValue = (TYPE_2DMatrixValue) fabs (M[PivotIndex][PivotIndex]);
    SwapRow = PivotIndex;
    for (TestRow = PivotIndex + 1; TestRow < 3; TestRow++)
    {
      TestValue = (TYPE_2DMatrixValue) fabs (M[TestRow][PivotIndex]);
      if (TestValue > MaxValue)
      {
        MaxValue = TestValue;
        SwapRow = TestRow;
      }
    }
    if (SwapRow != PivotIndex)
    {
      // Exchange the row we found with the pivot row.  Remember
      // to do the same operation to the inverse matrix.

      for (TempIndex = 0; TempIndex < 3; TempIndex++)
      {
        TempValue = M[SwapRow][TempIndex];
        M[SwapRow][TempIndex] = M[PivotIndex][TempIndex];
        M[PivotIndex][TempIndex] = TempValue;

        TempValue = I[SwapRow][TempIndex];
        I[SwapRow][TempIndex] = I[PivotIndex][TempIndex];
        I[PivotIndex][TempIndex] = TempValue;
      }
    }

    // Scale the pivot row so that it has a 1.0 in the pivot column.
    // Should already be all zeroes to the left of the pivot column,
    // but it isn't worth optimising since we still have to scale
    // the inverse.

    MaxValue = M[PivotIndex][PivotIndex];
    if (MaxValue == 0.0F)
      return FALSE; // Can't divide by zero.  Matrix doesn't have inverse.
    ScaleFactor = 1.0F / MaxValue;

    for (TempIndex = 0; TempIndex < 3; TempIndex++)
    {
      M[PivotIndex][TempIndex] *= ScaleFactor;
      I[PivotIndex][TempIndex] *= ScaleFactor;
    }
    M[PivotIndex][PivotIndex] = 1.0F; // Avoid roundoff error.

    // Subtract the pivot row from all the rows below and above it so that
    // they all have zeroes in the pivot column.

    for (EliminationRow = 0; EliminationRow < 3; EliminationRow++)
    {
      if (EliminationRow == PivotIndex)
        continue; // Don't touch pivot row, it stays as 1.0 in the pivot column.
      ScaleFactor = -M[EliminationRow][PivotIndex];
      if (ScaleFactor == 0.0F)
        continue; // This row already is zero in the pivot column, nothing to do.
      for (TempIndex = 0; TempIndex < 3; TempIndex++)
      {
        M[EliminationRow][TempIndex] += M[PivotIndex][TempIndex] * ScaleFactor;
        I[EliminationRow][TempIndex] += I[PivotIndex][TempIndex] * ScaleFactor;
      }
      M[EliminationRow][PivotIndex] = 0.0F; // Avoid roundoff error.
    }
  }
  return TRUE;
}



/*****************************************************************************
 * Find the inverse of a matrix.  This is useful for undoing transformations.
 * For example, given a location in the world, find out where this is inside
 * a transformed object.  This is done for mapping mouse clicks back into
 * a location in a model.  The output matrix can be the same as the input.
 * Returns TRUE if successful, FALSE if no inverse exists or something else
 * is wrong.
 *
 * The algorithm works by reducing a copy of the matrix to the
 * identity matrix while doing the same manipulations on an identity
 * matrix to create the inverse.
 */

BOOL LE_MATRIX_3D_MatrixInverse (TYPE_Matrix3DPointer MatrixPntr,
TYPE_Matrix3DPointer InversePntr)
{
  typedef TYPE_3DMatrixValue (*MatrixAsArrayPointer)[4];

  int                   EliminationRow;
  MatrixAsArrayPointer  I;
  MatrixAsArrayPointer  M;
  TYPE_Matrix3D         MatrixCopy;
  TYPE_3DMatrixValue    MaxValue;
  int                   PivotIndex;
  TYPE_3DMatrixValue    ScaleFactor;
  int                   SwapRow;
  int                   TempIndex;
  TYPE_3DMatrixValue    TempValue;
  int                   TestRow;
  TYPE_3DMatrixValue    TestValue;

  MatrixCopy = *MatrixPntr;
  M = (MatrixAsArrayPointer) &MatrixCopy;
  LE_MATRIX_3D_SetIdentity (InversePntr);
  I = (MatrixAsArrayPointer) InversePntr;

  // Perform maximal column pivoting, with the element on the diagonal
  // at matrix position (pivotIndex, pivotIndex).

  for (PivotIndex = 0; PivotIndex < 4; PivotIndex++)
  {
    // First find out which unprocessed row has the largest value in the
    // pivot column and make it the next row to be pivoted.  Only look
    // at rows which haven't been done yet (from pivot row to the bottom).

    MaxValue = (TYPE_3DMatrixValue) fabs (M[PivotIndex][PivotIndex]);
    SwapRow = PivotIndex;
    for (TestRow = PivotIndex + 1; TestRow < 4; TestRow++)
    {
      TestValue = (TYPE_3DMatrixValue) fabs (M[TestRow][PivotIndex]);
      if (TestValue > MaxValue)
      {
        MaxValue = TestValue;
        SwapRow = TestRow;
      }
    }
    if (SwapRow != PivotIndex)
    {
      // Exchange the row we found with the pivot row.  Remember
      // to do the same operation to the inverse matrix.

      for (TempIndex = 0; TempIndex < 4; TempIndex++)
      {
        TempValue = M[SwapRow][TempIndex];
        M[SwapRow][TempIndex] = M[PivotIndex][TempIndex];
        M[PivotIndex][TempIndex] = TempValue;

        TempValue = I[SwapRow][TempIndex];
        I[SwapRow][TempIndex] = I[PivotIndex][TempIndex];
        I[PivotIndex][TempIndex] = TempValue;
      }
    }

    // Scale the pivot row so that it has a 1.0 in the pivot column.
    // Should already be all zeroes to the left of the pivot column,
    // but it isn't worth optimising since we still have to scale
    // the inverse.

    MaxValue = M[PivotIndex][PivotIndex];
    if (MaxValue == 0.0F)
      return FALSE; // Can't divide by zero.  Matrix doesn't have inverse.
    ScaleFactor = 1.0F / MaxValue;

    for (TempIndex = 0; TempIndex < 4; TempIndex++)
    {
      M[PivotIndex][TempIndex] *= ScaleFactor;
      I[PivotIndex][TempIndex] *= ScaleFactor;
    }
    M[PivotIndex][PivotIndex] = 1.0F; // Avoid roundoff error.

    // Subtract the pivot row from all the rows below and above it so that
    // they all have zeroes in the pivot column.

    for (EliminationRow = 0; EliminationRow < 4; EliminationRow++)
    {
      if (EliminationRow == PivotIndex)
        continue; // Don't touch pivot row, it stays as 1.0 in the pivot column.
      ScaleFactor = -M[EliminationRow][PivotIndex];
      if (ScaleFactor == 0.0F)
        continue; // This row already is zero in the pivot column, nothing to do.
      for (TempIndex = 0; TempIndex < 4; TempIndex++)
      {
        M[EliminationRow][TempIndex] += M[PivotIndex][TempIndex] * ScaleFactor;
        I[EliminationRow][TempIndex] += I[PivotIndex][TempIndex] * ScaleFactor;
      }
      M[EliminationRow][PivotIndex] = 0.0F; // Avoid roundoff error.
    }
  }
  return TRUE;
}




/*****************************************************************************
 * Initialise the matrix manipulation routines.  Not much to do here, unless
 * we are using special matrix math hardware or something similarly oddball.
 */

void LI_MATRIX_InitSystem (void)
{
  // Switch the floating point mode to use rounding to nearest integer
  // when doing float to integer conversions.  Needed for 2D vector by
  // matrix multiplication.

  _controlfp (_RC_NEAR, _MCW_RC);
}

/********************************************************************
 * Sets the matrix so that it would produce a rotation around the
 * given vector. (thank god for algebra source books) AG
 ********************************************************************/
BOOL LE_MATRIX_3D_RotateAroundVector (TYPE_Point3DPointer VectorPntr,
									  TYPE_Angle3D angle,
									  TYPE_Matrix3DPointer ResultPntr )
{
	TYPE_Point3D n;

	TYPE_Matrix3D Rotx, Rotxinv,Roty,Rotyinv, Rotz;
	TYPE_Matrix3D temp, temp2;

	float a, b, c, d;
	float c_d,b_d; //These last two for x rotation figuration.

	LE_MATRIX_3D_SetIdentity( ResultPntr );

	//If the vector is along an axis...
	if ( VectorPntr->x == 0  && VectorPntr->y == 0 )
	{
		LE_MATRIX_3D_SetRotateZ( angle, ResultPntr );
		return TRUE;
	}
	if ( VectorPntr->x == 0  && VectorPntr->z == 0 )
	{
		LE_MATRIX_3D_SetRotateY( angle, ResultPntr );
		return TRUE;
	}
	if ( VectorPntr->y == 0  && VectorPntr->z == 0 )
	{
		LE_MATRIX_3D_SetRotateX( angle, ResultPntr );
		return TRUE;
	}

	//Set all of our matrices to be identity.
	LE_MATRIX_3D_SetIdentity( &Rotx );
	Rotz = Rotyinv = Roty = Rotxinv = Rotx;

	LE_MATRIX_3D_NormalizeVector( &n, VectorPntr );
	
	//Okay, set up for eq'n.
	a = n.x;
	b = n.y;
	c = n.z;
	d = (float) sqrt( b * b + c * c );

	//if d is zero, there isn't any x rotation to be applied.
	if( d != 0.0f )
	{
		c_d = c/d; 
		b_d = b/d;

		Rotx._22 = c_d;	
		Rotx._23 = b_d;	
		Rotx._32 = -b_d;
		Rotx._33 = c_d;	
		LE_MATRIX_3D_MatrixInverse( &Rotx, &Rotxinv );
	}

	Roty._11 = d;	
	Roty._13 = a;	
	Roty._31 = -a;	
	Roty._33 = d;	
	LE_MATRIX_3D_MatrixInverse( &Roty, &Rotyinv );

	LE_MATRIX_3D_SetRotateZ( angle, &Rotz );

	//if d is zero, there isn't any x rotation to be applied.
	if( d != 0.0f )
		LE_MATRIX_3D_MatrixMultiply( &Rotyinv, &Rotxinv, &temp );
	else
		temp = Rotyinv;

	LE_MATRIX_3D_MatrixMultiply( &Rotz, &temp, &temp2 );
	LE_MATRIX_3D_MatrixMultiply( &Roty, &temp2, &temp );

	//if d is zero, there isn't any x rotation to be applied.
	if( d != 0.0f )
	{
		LE_MATRIX_3D_MatrixMultiply( &Rotx, &temp, ResultPntr );
	}
	else
	{
		*ResultPntr = temp;
	}

	return TRUE;
}

/********************************************************************
 * Normalizes the inVector, stores to the out vector, in and out
 * may be the same.
 ********************************************************************/
extern BOOL LE_MATRIX_3D_NormalizeVector( TYPE_Point3DPointer inVectorPntr,
										  TYPE_Point3DPointer outVectorPntr )
{
	TYPE_Coord3D vx, vy, vz, inv_mod;
    vx = inVectorPntr->x;
    vy = inVectorPntr->y;
    vz = inVectorPntr->z;
    if ( ( vx == 0 ) && ( vy == 0 ) && ( vz == 0 ) )
        return FALSE;
    inv_mod = (TYPE_Coord3D)( 1.0 / sqrt( vx * vx + vy * vy + vz * vz ) );
    outVectorPntr->x = vx * inv_mod;
    outVectorPntr->y = vy * inv_mod;
    outVectorPntr->z = vz * inv_mod;
    return TRUE;
}

/********************************************************************
 * Cross product of vectors A and B....A, B and out must be different
 ********************************************************************/
BOOL LE_MATRIX_3D_VectorCrossProduct( TYPE_Point3DPointer inVectorA,
											 TYPE_Point3DPointer inVectorB,
											 TYPE_Point3DPointer outVector )
{
	outVector->x = inVectorA->y * inVectorB->z - inVectorA->z * inVectorB->y;
	outVector->y = inVectorA->z * inVectorB->x - inVectorA->x * inVectorB->z;
	outVector->z = inVectorA->x * inVectorB->y - inVectorA->y * inVectorB->x;
	return TRUE;
}

/********************************************************************
 * Returns the dot product of two vectors.
 ********************************************************************/
TYPE_Coord3D LE_MATRIX_3D_VectorDotProduct( TYPE_Point3DPointer inVectorA,
												   TYPE_Point3DPointer inVectorB )
{
	return( ( inVectorA->x * inVectorB->x ) + ( inVectorA->y * inVectorB->y ) + ( inVectorA->z * inVectorB->z ) );
}


/********************************************************************
 * Returns the length of a vector.
 ********************************************************************/
TYPE_Coord3D LE_MATRIX_3D_VectorLength( TYPE_Point3DPointer inVector )
{
	return (TYPE_Coord3D)sqrt( inVector->x * inVector->x +
							   inVector->y * inVector->y +
							   inVector->z * inVector->z );
}

TYPE_Angle3D LE_MATRIX_3D_GetAngleBetweenVectors( TYPE_Point3DPointer inVectorA,
												  TYPE_Point3DPointer inVectorB )
{
	TYPE_Point3D nVectorA;
	TYPE_Point3D nVectorB;
	TYPE_Coord3D dotProduct;
	TYPE_Angle3D angle;
	int			 sign;

	LE_MATRIX_3D_NormalizeVector( inVectorA, &nVectorA );
	LE_MATRIX_3D_NormalizeVector( inVectorB, &nVectorB );

	dotProduct = LE_MATRIX_3D_VectorDotProduct( &nVectorA, &nVectorB );

	if( dotProduct == 0.0 )
	{
		return TYPE_QUARTER_CIRCLE_3D_ANGLE;
	}

	if( dotProduct >= 0.0 )
	{
		sign = 1;
		dotProduct = -dotProduct;
	}
	else
	{
		sign = 0;
	}

	if( dotProduct > 1.0f ) dotProduct = 1.0f;
	if( dotProduct < -1.0f ) dotProduct = -1.0f;
	angle = (TYPE_Angle3D)acos( (double)dotProduct );

	if( sign )
	{
		angle = TYPE_HALF_CIRCLE_3D_ANGLE - angle;
	}

	return angle;
}

/********************************************************************
 * Returns the Yaw, Pitch, Roll, Scale (along all 3 axis) and
 * Final translation of a matrix. If any of the parameters is
 * null, it won't return it (it probably has to figure it out
 * tho)
 ********************************************************************/
BOOL LE_MATRIX_3D_GetYPRST( TYPE_Matrix3DPointer MatrixPntr,
							TYPE_Angle3DPointer Yaw,
							TYPE_Angle3DPointer Pitch,
							TYPE_Angle3DPointer Roll,
							TYPE_Coord3DPointer ScaleX,
							TYPE_Coord3DPointer ScaleY,
							TYPE_Coord3DPointer ScaleZ,
							TYPE_Point3DPointer Translate )
{
	TYPE_Matrix3D matrix;
	TYPE_Point3D unitX, unitY, unitZ;
	TYPE_Point3D resultX, resultY, resultZ;
	TYPE_Point3D newY;
	TYPE_Point3D temp;

	TYPE_Angle3D yaw, pitch, roll;
	TYPE_Coord3D sx, sy, sz;
	TYPE_Point3D t;

	unitX.x = 1.0f;
	unitX.y = 0.0f;
	unitX.z = 0.0f;
	unitY.x = 0.0f;
	unitY.y = 1.0f;
	unitY.z = 0.0f;
	unitZ.x = 0.0f;
	unitZ.y = 0.0f;
	unitZ.z = 1.0f;

	//The translate can be yanked directly out of the matrix
	t.x = MatrixPntr->_41;
	t.y = MatrixPntr->_42;
	t.z = MatrixPntr->_43;

	LE_MATRIX_3D_VectorMultiply( &unitX, MatrixPntr, &resultX );
	LE_MATRIX_3D_VectorMultiply( &unitY, MatrixPntr, &resultY );
	LE_MATRIX_3D_VectorMultiply( &unitZ, MatrixPntr, &resultZ );

	//Un-translate the resulting vertices..
	resultX.x -= t.x;
	resultX.y -= t.y;
	resultX.z -= t.z;
	resultY.x -= t.x;
	resultY.y -= t.y;
	resultY.z -= t.z;
	resultZ.x -= t.x;
	resultZ.y -= t.y;
	resultZ.z -= t.z;

	//Okay, here we can figure out the scale by getting the length 
	//since we started from unit vectors.
	sx = LE_MATRIX_3D_VectorLength( &resultX );
	sy = LE_MATRIX_3D_VectorLength( &resultY );
	sz = LE_MATRIX_3D_VectorLength( &resultZ );

	//Normalize the vectors.
//	LE_MATRIX_3D_NormalizeVector( &resultX, &resultX );
//	LE_MATRIX_3D_NormalizeVector( &resultY, &resultY );
//	LE_MATRIX_3D_NormalizeVector( &resultZ, &resultZ );

	//Here comes the interesting parts....
	//Yaw, grab the Z result, and figure out how far around the Y axis we
	//had to spin it.
	if( resultZ.z == 0.0f )
	{
		if( resultZ.x > 0.0f )
			yaw = TYPE_QUARTER_CIRCLE_3D_ANGLE;
		else
			yaw = -TYPE_QUARTER_CIRCLE_3D_ANGLE;
	}
	else
	{
		yaw = (TYPE_Angle3D)atan( resultZ.x / resultZ.z );
		if( resultZ.z < 0.0f )
			yaw = TYPE_HALF_CIRCLE_3D_ANGLE + yaw;
		if( resultZ.z > 0.0f && resultZ.x < 0.0f )
			yaw = TYPE_FULL_CIRCLE_3D_ANGLE + yaw;
	}

	LE_MATRIX_3D_SetRotateY( -yaw, &matrix );

	LE_MATRIX_3D_VectorMultiply( &resultZ, &matrix, &resultZ );

	//Pitch, grab the Z vector and figure out how far around the X axis we
	//had to spin it.
	if( resultZ.z == 0.0f )
	{
		if( resultZ.y > 0.0f )
			pitch = TYPE_QUARTER_CIRCLE_3D_ANGLE;
		else
			pitch = -TYPE_QUARTER_CIRCLE_3D_ANGLE;
	}
	else
	{
		pitch = -(TYPE_Angle3D)atan( resultZ.y / resultZ.z );
		if( resultZ.z < 0.0f )
			pitch = TYPE_HALF_CIRCLE_3D_ANGLE + pitch;
		if( resultZ.z > 0.0f && resultZ.y > 0.0f )
			pitch = TYPE_FULL_CIRCLE_3D_ANGLE + pitch;
	}

	temp.x = 0.0f;
	temp.y = 0.0f;
	temp.z = 0.0f;

	LE_MATRIX_3D_SetScaleRotateTranslate( 1.0f, 1.0f, 1.0f, yaw, pitch, 0.0f, &temp, &matrix );

	LE_MATRIX_3D_VectorMultiply( &unitY, &matrix, &newY );
	LE_MATRIX_3D_NormalizeVector( &resultY, &temp );

	//Roll, grab the Y vector and figure out how far around the Z axis we
	//had to spin it.
	roll = LE_MATRIX_3D_GetAngleBetweenVectors( &temp, &newY );

	LE_MATRIX_3D_VectorCrossProduct( &resultY, &newY, &temp );

	if( LE_MATRIX_3D_VectorDotProduct( &temp, &resultZ ) > 0.0f )
	{
		roll = TYPE_FULL_CIRCLE_3D_ANGLE - roll;
	}

	//Okay, we have our results, now for the coup de grace...returning them.
	if( Yaw ) *Yaw = yaw;
	if( Pitch ) *Pitch = pitch;
	if( Roll ) *Roll = roll;
	if( ScaleX ) *ScaleX = sx;
	if( ScaleY ) *ScaleY = sy;
	if( ScaleZ ) *ScaleZ = sz;
	if( Translate ) *Translate = t;

	return TRUE;
}


/*****************************************************************************
 * Set the matrix to one that first adjusts the origin and then scales
 * and rotates it around the origin and finally moves it around in the world.
 */

void LE_MATRIX_2D_SetOriginScaleRotateTranslate (TYPE_Point2DPointer OriginPntr,
TYPE_Scale2D ScaleFactorX, TYPE_Scale2D ScaleFactorY, TYPE_Angle2D Rotate,
TYPE_Point2DPointer TranslatePntr, TYPE_Matrix2DPointer MatrixPntr)
{
  TYPE_Matrix2D Temp2DMatrixA;
  TYPE_Matrix2D Temp2DMatrixB;
  TYPE_Point2D  Temp2DPointA;

  Temp2DPointA.x = -OriginPntr->x;
  Temp2DPointA.y = -OriginPntr->y;
  LE_MATRIX_2D_SetTranslate (&Temp2DPointA, MatrixPntr);

  if (ScaleFactorX != 1.0F || ScaleFactorY != 1.0F)
  {
    LE_MATRIX_2D_SetScaleXY (ScaleFactorX, ScaleFactorY, &Temp2DMatrixA);
    LE_MATRIX_2D_MatrixMultiply (MatrixPntr, &Temp2DMatrixA, &Temp2DMatrixB);
    *MatrixPntr = Temp2DMatrixB;
  }

  if (Rotate != 0.0F)
  {
    LE_MATRIX_2D_SetRotate (Rotate, &Temp2DMatrixA);
    LE_MATRIX_2D_MatrixMultiply (MatrixPntr, &Temp2DMatrixA, &Temp2DMatrixB);
    *MatrixPntr = Temp2DMatrixB;
  }

  if (TranslatePntr->x != 0 || TranslatePntr->y != 0)
  {
    LE_MATRIX_2D_SetTranslate (TranslatePntr, &Temp2DMatrixA);
    LE_MATRIX_2D_MatrixMultiply (MatrixPntr, &Temp2DMatrixA, &Temp2DMatrixB);
    *MatrixPntr = Temp2DMatrixB;
  }
}



/*****************************************************************************
 * Set the matrix to one that first adjusts the origin, then scales the item
 * then rotates it and finally moves it around.
 *
 * Origin - Where scaling and rotation are centered on.
 * Scale - Stretches the thing in various directions, 1.0 for no stretch.
 * Roll  - Sides of a boat going up and down.
 * Pitch - Bow and stern of the boat going up and down.
 * Yaw   - Compass heading that the boat is pointing towards.
 * Translate - Move the whole mess over by the given position delta.
 *
 * In 3D the character is facing forwards along the positive Z axis
 * (yaw zero by definition), so you see the character's back when he
 * is in its default orientation.
 */

void LE_MATRIX_3D_SetOriginScaleRotateTranslate (TYPE_Point3DPointer OriginPntr,
TYPE_Scale3D ScaleFactorX, TYPE_Scale3D ScaleFactorY, TYPE_Scale3D ScaleFactorZ,
TYPE_Angle3D Yaw, TYPE_Angle3D Pitch, TYPE_Angle3D Roll,
TYPE_Point3DPointer TranslatePntr, TYPE_Matrix3DPointer MatrixPntr)
{
  TYPE_Matrix3D Temp3DMatrixA;
  TYPE_Matrix3D Temp3DMatrixB;
  TYPE_Point3D  Temp3DPointA;

  Temp3DPointA.x = -OriginPntr->x;
  Temp3DPointA.y = -OriginPntr->y;
  Temp3DPointA.z = -OriginPntr->z;
  LE_MATRIX_3D_SetTranslate (&Temp3DPointA, MatrixPntr);

  if (ScaleFactorX != 1.0F || ScaleFactorY != 1.0F || ScaleFactorZ != 1.0F)
  {
    LE_MATRIX_3D_SetScaleXYZ (ScaleFactorX, ScaleFactorY, ScaleFactorZ, &Temp3DMatrixA);
    LE_MATRIX_3D_MatrixMultiply (MatrixPntr, &Temp3DMatrixA, &Temp3DMatrixB);
    *MatrixPntr = Temp3DMatrixB;
  }

  if (Roll != 0.0F)
  {
    LE_MATRIX_3D_SetRotateZ (Roll, &Temp3DMatrixA);
    LE_MATRIX_3D_MatrixMultiply (MatrixPntr, &Temp3DMatrixA, &Temp3DMatrixB);
    *MatrixPntr = Temp3DMatrixB;
  }

  if (Pitch != 0.0F)
  {
    LE_MATRIX_3D_SetRotateX (Pitch, &Temp3DMatrixA);
    LE_MATRIX_3D_MatrixMultiply (MatrixPntr, &Temp3DMatrixA, &Temp3DMatrixB);
    *MatrixPntr = Temp3DMatrixB;
  }

  if (Yaw != 0.0F)
  {
    LE_MATRIX_3D_SetRotateY (Yaw, &Temp3DMatrixA);
    LE_MATRIX_3D_MatrixMultiply (MatrixPntr, &Temp3DMatrixA, &Temp3DMatrixB);
    *MatrixPntr = Temp3DMatrixB;
  }

  if (TranslatePntr->x != 0.0F || TranslatePntr->y != 0.0F || TranslatePntr->z != 0.0F)
  {
    LE_MATRIX_3D_SetTranslate (TranslatePntr, &Temp3DMatrixA);
    LE_MATRIX_3D_MatrixMultiply (MatrixPntr, &Temp3DMatrixA, &Temp3DMatrixB);
    *MatrixPntr = Temp3DMatrixB;
  }
}


/*****************************************************************************
 * Deallocate stuff used by the matrix manipulation routines.
 */
void LI_MATRIX_RemoveSystem (void)
{
}

#endif /* CE_ARTLIB_EnableSystemMatrix */
