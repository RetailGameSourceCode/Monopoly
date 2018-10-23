#ifndef __L_MATRIX_H__
#define __L_MATRIX_H__

/*****************************************************************************
 *
 *   FILE:        L_Matrix.h
 *   DESCRIPTION: Some matrix manipulation code for matrices that represent
 *                2D and 3D transformation operations (the matrices are 3D
 *                and 4D since they also handle translations by using an
 *                extra dimension and homogeneous coordinates).
 *
 *   (C) Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Matrix.h 8     4/20/99 12:10p Andrewx $
 ****************************************************************************/

/************************************************************/
/* DEFINES                                                  */
/*----------------------------------------------------------*/

/************************************************************/
/* MACROS                                                   */
/*----------------------------------------------------------*/

/************************************************************/
/* STRUCTURES, ENUMS, ETC.                                  */
/*----------------------------------------------------------*/


/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

extern void LE_MATRIX_2D_SetZero (TYPE_Matrix2DPointer MatrixPntr);
extern void LE_MATRIX_3D_SetZero (TYPE_Matrix3DPointer MatrixPntr);
extern void LE_MATRIX_2D_SetIdentity (TYPE_Matrix2DPointer MatrixPntr);
extern void LE_MATRIX_3D_SetIdentity (TYPE_Matrix3DPointer MatrixPntr);
extern void LE_MATRIX_2D_SetTranslate (TYPE_Point2DPointer PointPntr, TYPE_Matrix2DPointer MatrixPntr);
extern void LE_MATRIX_3D_SetTranslate (TYPE_Point3DPointer PointPntr, TYPE_Matrix3DPointer MatrixPntr);
extern void LE_MATRIX_2D_SetRotate (TYPE_Angle2D Angle, TYPE_Matrix2DPointer MatrixPntr);
extern void LE_MATRIX_3D_SetRotateX (TYPE_Angle3D Angle, TYPE_Matrix3DPointer MatrixPntr);
extern void LE_MATRIX_3D_SetRotateY (TYPE_Angle3D Angle, TYPE_Matrix3DPointer MatrixPntr);
extern void LE_MATRIX_3D_SetRotateZ (TYPE_Angle3D Angle, TYPE_Matrix3DPointer MatrixPntr);
extern void LE_MATRIX_2D_SetScale (TYPE_Scale2D ScaleFactor, TYPE_Matrix2DPointer MatrixPntr);
extern void LE_MATRIX_2D_SetScaleXY (TYPE_Scale2D ScaleFactorX, TYPE_Scale2D ScaleFactorY, TYPE_Matrix2DPointer MatrixPntr);
extern void LE_MATRIX_3D_SetScale (TYPE_Scale3D ScaleFactor, TYPE_Matrix3DPointer MatrixPntr);
extern void LE_MATRIX_3D_SetScaleXYZ (TYPE_Scale3D ScaleFactorX, TYPE_Scale3D ScaleFactorY, TYPE_Scale3D ScaleFactorZ, TYPE_Matrix3DPointer MatrixPntr);
extern void LE_MATRIX_2D_SetScaleRotateTranslate (TYPE_Scale2D ScaleFactorX, TYPE_Scale2D ScaleFactorY, TYPE_Angle2D Rotate, TYPE_Point2DPointer TranslatePntr, TYPE_Matrix2DPointer MatrixPntr);
extern void LE_MATRIX_3D_SetScaleRotateTranslate (TYPE_Scale3D ScaleFactorX, TYPE_Scale3D ScaleFactorY, TYPE_Scale3D ScaleFactorZ, TYPE_Angle3D Yaw, TYPE_Angle3D Pitch, TYPE_Angle3D Roll, TYPE_Point3DPointer TranslatePntr, TYPE_Matrix3DPointer MatrixPntr);
extern void LE_MATRIX_2D_SetOriginScaleRotateTranslate (TYPE_Point2DPointer OriginPntr, TYPE_Scale2D ScaleFactorX, TYPE_Scale2D ScaleFactorY, TYPE_Angle2D Rotate, TYPE_Point2DPointer TranslatePntr, TYPE_Matrix2DPointer MatrixPntr);
extern void LE_MATRIX_3D_SetOriginScaleRotateTranslate (TYPE_Point3DPointer OriginPntr, TYPE_Scale3D ScaleFactorX, TYPE_Scale3D ScaleFactorY, TYPE_Scale3D ScaleFactorZ, TYPE_Angle3D Yaw, TYPE_Angle3D Pitch, TYPE_Angle3D Roll, TYPE_Point3DPointer TranslatePntr, TYPE_Matrix3DPointer MatrixPntr);
extern void LE_MATRIX_2D_VectorMultiply (TYPE_Point2DPointer InPointPntr, TYPE_Matrix2DPointer MatrixPntr, TYPE_Point2DPointer OutPointPntr);
extern void LE_MATRIX_3D_VectorMultiply (TYPE_Point3DPointer InPointPntr, TYPE_Matrix3DPointer MatrixPntr, TYPE_Point3DPointer OutPointPntr);
extern void LE_MATRIX_2D_MatrixMultiply (TYPE_Matrix2DPointer MatrixAPntr, TYPE_Matrix2DPointer MatrixBPntr, TYPE_Matrix2DPointer ResultPntr);
extern void LE_MATRIX_3D_MatrixMultiply (TYPE_Matrix3DPointer MatrixAPntr, TYPE_Matrix3DPointer MatrixBPntr, TYPE_Matrix3DPointer ResultPntr);
extern BOOL LE_MATRIX_2D_MatrixInverse (TYPE_Matrix2DPointer MatrixPntr, TYPE_Matrix2DPointer InversePntr);
extern BOOL LE_MATRIX_3D_MatrixInverse (TYPE_Matrix3DPointer MatrixPntr, TYPE_Matrix3DPointer InversePntr);
extern BOOL LE_MATRIX_3D_RotateAroundVector ( TYPE_Point3DPointer VectorPntr, TYPE_Angle3D angle, TYPE_Matrix3DPointer ResultPntr );
extern BOOL LE_MATRIX_3D_NormalizeVector( TYPE_Point3DPointer inVectorPntr, TYPE_Point3DPointer outVectorPntr );
extern BOOL LE_MATRIX_3D_VectorCrossProduct( TYPE_Point3DPointer inVectorA, TYPE_Point3DPointer inVectorB, TYPE_Point3DPointer outVector );
extern TYPE_Coord3D LE_MATRIX_3D_VectorDotProduct( TYPE_Point3DPointer inVectorA, TYPE_Point3DPointer inVectorB );
extern TYPE_Coord3D LE_MATRIX_3D_VectorLength( TYPE_Point3DPointer inVector );
extern TYPE_Angle3D LE_MATRIX_3D_GetAngleBetweenVectors( TYPE_Point3DPointer inVectorA, TYPE_Point3DPointer inVectorB );
extern BOOL LE_MATRIX_3D_GetYPRST( TYPE_Matrix3DPointer MatrixPntr, TYPE_Angle3DPointer yaw, TYPE_Angle3DPointer pitch, TYPE_Angle3DPointer roll, TYPE_Coord3DPointer scalex, TYPE_Coord3DPointer scaley, TYPE_Coord3DPointer scalez, TYPE_Point3DPointer translate );


extern void LI_MATRIX_InitSystem (void);
extern void LI_MATRIX_RemoveSystem (void);

#endif /* __L_MATRIX_H__ */
