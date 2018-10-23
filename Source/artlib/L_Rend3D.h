#ifndef __REND3D_H__
#define __REND3D_H__

/*****************************************************************************
 *
 * FILE:        L_REND3D.h
 * DESCRIPTION: This module handles 3D positioning and drawing requests
 *              from the sequencer.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Rend3D.h 20    9/21/99 16:40 Davew $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Rend3D.h $
 * 
 * 20    9/21/99 16:40 Davew
 * Needed to take advantage of the new Force parameter when setting the
 * background of the viewport.
 * 
 * 19    9/09/99 12:47 Davew
 * Added two functions to set the background of the viewport
 * 
 * 18    99/09/02 12:51p Agmsmith
 * Get a 3D bounding box for a 3D animation.
 *
 * 17    8/17/99 11:02 Davew
 * Added a new function to set the viewing angle of the scene.
 *
 * 16    8/17/99 10:58 Davew
 * Cleaned things up quite a bit.  Added a new function to set the near
 * and far clipping planes.
 *
 * 15    7/30/99 12:13p Agmsmith
 * It's a forwards vector, not a position to look at.
 *
 * 14    6/28/99 12:43p Russellk
 * Added functions to return rendCamera location & orientation
 *
 * 13    5/31/99 11:32 Davew
 * Added new LE_REND3D_SetViewportRect function
 *
 * 12    5/26/99 1:16p Andrewx
 * Modification for lighting work, and new camera move functions for
 * Mr.Smith.
 *
 * 11    5/25/99 10:01 Davew
 * Fixed the ambient light stuff
 *
 * 10    5/20/99 4:00p Andrewx
 * Modifications for light support, and removal of code for VRML.
 *
 * 9     4/29/99 12:38p Andrewx
 * Added a flag for the 3d render slots which allow the hardware to clear
 * the viewport reight before rendering.
 *
 * 8     4/27/99 12:50p Andrewx
 * Added LE_CAMERA_MoveSmooth, it's a hack for now, but works well.
 *
 * 7     4/22/99 15:01 Davew
 * Updated to DX v6
 *
 * 6     3/23/99 10:57a Andrewx
 * Adding rect support.
 *
 * 5     1/05/99 5:00p Agmsmith
 * Renderer / sequencer interface converted from a bunch of arrays of
 * pointers to functions into C++ virtual function calls and an abstract
 * base class.  Now it should be easier to add new functionality.
 ****************************************************************************/

/************************************************************/
/* STRUCTURES                                               */
/*----------------------------------------------------------*/

typedef struct _Rend3DCamera {
  TYPE_Point3D location;
  TYPE_Point3D forwardVector;
  TYPE_Point3D upVector;
  TYPE_Angle2D rollAngle;
} Rend3DCamera;

class Surface;

/************************************************************/
/* GAMECODE FUNCTIONS                                       */
/*----------------------------------------------------------*/


// Set up a 3D render slot.
BOOL LE_REND3D_InstallInRenderSlot(LE_REND_RenderSlot RenderSlot,
                                   TYPE_Rect CameraRenderArea,
                                   PDDSURFACE WorkingSurface,
                                   PDDSURFACE OutputSurface,
                                   LE_REND_RenderSlotSet OverlappingRenderSlotSet);


// Get the scenes camera location & orientation
TYPE_Point3D LE_REND3D_GetLocation( LE_REND_RenderSlot RenderSlot );
TYPE_Point3D LE_REND3D_GetForwardVector( LE_REND_RenderSlot RenderSlot );
TYPE_Point3D LE_REND3D_GetUpwardVector( LE_REND_RenderSlot RenderSlot );

// Manipulate the scene's camera.
Rend3DCamera *LE_REND3D_BeginCameraMove(LE_REND_RenderSlot RenderSlot);
// Call this when finished making changes to the scene's camera.
BOOL LE_REND3D_EndCameraMove(LE_REND_RenderSlot RenderSlot);
BOOL LE_REND3D_MoveCamera( LE_REND_RenderSlot RenderSlot,
               TYPE_Point3DPointer position,
               TYPE_Point3DPointer forwards,
               TYPE_Point3DPointer up,
               TYPE_Angle3D fovAngle,
               TYPE_Coord3D nearZ,
               TYPE_Coord3D farZ );

BOOL LE_REND3D_MoveCameraSmooth( LE_REND_RenderSlot RenderSlot,
               TYPE_Point3DPointer position,
               TYPE_Point3DPointer at,
               TYPE_Point3DPointer up,
               TYPE_Angle3D fovAngle,
               int steps );


// Lighting functions ========================================================

BOOL LE_REND3D_SetAmbient(LE_REND_RenderSlot RenderSlot,
                          TYPE_Point3DPointer color);
BOOL LE_REND3D_SetAmbient(LE_REND_RenderSlot RenderSlot,
                          DWORD dwColour);

BOOL LE_REND3D_MakeLight(LE_REND_RenderSlot RenderSlot, int lightNum);

BOOL LE_REND3D_DeleteLight(LE_REND_RenderSlot RenderSlot, int lightNum);

BOOL LE_REND3D_SetLightType(LE_REND_RenderSlot RenderSlot, int lightNum,
                            D3DLIGHTTYPE eType);

BOOL LE_REND3D_SetLightColor(LE_REND_RenderSlot RenderSlot, int lightNum,
                             const CLRVALUE& color);

BOOL LE_REND3D_SetLightPosition(LE_REND_RenderSlot RenderSlot, int lightNum,
                                TYPE_Point3DPointer position);

BOOL LE_REND3D_SetLightDirection(LE_REND_RenderSlot RenderSlot, int lightNum,
                                 TYPE_Point3DPointer direction );

BOOL LE_REND3D_SetLightAttenuation(LE_REND_RenderSlot RenderSlot, int lightNum,
                                   TYPE_Point3DPointer attenuation );

BOOL LE_REND3D_SetLightRange(LE_REND_RenderSlot RenderSlot, int lightNum,
                             TYPE_Coord3D range);

BOOL LE_REND3D_SetLightFalloff(LE_REND_RenderSlot RenderSlot, int lightNum,
                               TYPE_Coord3D falloff);

BOOL LE_REND3D_SetLightPhi(LE_REND_RenderSlot RenderSlot, int lightNum,
                           TYPE_Coord3D phi);

BOOL LE_REND3D_SetLightTheta(LE_REND_RenderSlot RenderSlot, int lightNum,
                             TYPE_Coord3D theta);

BOOL LE_REND3D_SetLightUmbra(LE_REND_RenderSlot RenderSlot, int lightNum,
                             TYPE_Coord3D umbra);

BOOL LE_REND3D_SetLightPenumbra(LE_REND_RenderSlot RenderSlot, int lightNum,
                                TYPE_Coord3D umbra);

BOOL LE_REND3D_SetLightFlags(LE_REND_RenderSlot RenderSlot, int lightNum,
                             DWORD flags);

// Other miscellaneous stuff =================================================

BOOL LE_REND3D_SetViewportRect(LE_REND_RenderSlot RenderSlot, const RECT& rct);

void LE_REND3D_SetViewingAngle(LE_REND_RenderSlot RenderSlot, float fRads);
void LE_REND3D_SetClippingPlanes(LE_REND_RenderSlot RenderSlot, float fNear,
                                 float fFar);

//Set clear to true if we want the system to clear to the background color
//before rendering
void LE_REND3D_ClearBeforeRender(LE_REND_RenderSlot RenderSlot, BOOL clear);

BOOL LE_REND3D_LoadBackgroundBitmap(LE_REND_RenderSlot RenderSlot,
                                    LPCSTR szFile);

BOOL LE_REND3D_SetBackgroundSurface(LE_REND_RenderSlot RenderSlot,
                                    Surface* pBackground,
                                    BOOL bForce = FALSE);


/************************************************************/
/* INTERNAL LIBRARY FUNCTIONS                               */
/*----------------------------------------------------------*/

BOOL LI_REND3D_InitSystem (void);

BOOL LI_REND3D_RemoveSystem (void);

BOOL LI_REND3D_GetBoundingBoxFor3DMesh (
  LE_SEQNCR_RuntimeInfoPointer SeqPntr, TYPE_Point3DPointer PointArray);


#endif  //__REND3D_H__
