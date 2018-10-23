#ifndef __L_REND2D_H__
#define __L_REND2D_H__

/*****************************************************************************
 *
 * FILE:        L_REND2D.h
 * DESCRIPTION: Rendering 2D bitmaps to the screen, under sequencer command,
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Rend2D.h 6     4/22/99 15:01 Davew $
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
/* GLOBAL VARIABLES - see L_REND2D.c for details            */
/*----------------------------------------------------------*/

/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

extern void LI_REND2D_InitSystem (void);
extern void LI_REND2D_RemoveSystem (void);

extern BOOL LE_REND2D_InstallInRenderSlot (
  LE_REND_RenderSlot RenderSlot,
  TYPE_Point2D CameraCenterWorldPosition,
  TYPE_Angle2D CameraScreenRotation,
  TYPE_Scale2D CameraScaleFactor,
  TYPE_Rect CameraRenderArea,
  TYPE_Coord2D CameraOffScreenSoundQuietDistance,
  UNS8 LabelOfCameraToUse,
  PDDSURFACE WorkingSurface,
  PDDSURFACE OutputSurface,
  LE_REND_RenderSlotSet OverlappingRenderSlotSet);

extern BOOL LE_REND2D_MoveCamera (LE_REND_RenderSlot RenderSlot,
  TYPE_Coord2D X, TYPE_Coord2D Y, TYPE_Scale2D Scale);

extern BOOL LE_REND2D_ScreenToWorldCoordinates (LE_REND_RenderSlot RenderSlot,
  TYPE_Point2DPointer ScreenPointPntr, TYPE_Point2DPointer WorldPointPntr);

#endif // __L_REND2D_H__
