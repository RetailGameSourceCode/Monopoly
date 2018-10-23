// //////////////////////////////////////////////////
// Copyright (C) 1997, Artech,        
// All rights reserved.          
//                 
// This document contains intellectual property  
// of Artech.  Any copying or reproduction    
// without prior written permission is prohibited. 
//                 
// //////////////////////////////////////////////////
// MODULE: view.cpp 
//                  
// ///////////////// END OF HEADER //////////////////
#include "PC3DHdr.h"
#include "l_view.h"

#include "Camera.h"
#include "l_Light.h"
#include "l_Material.h"

#ifdef USE_OLD_FRAME
#include "l_Mesh.h"
#else
#include "NewMesh.h"
#endif


// ===========================================================================
// view class ================================================================
// ===========================================================================


// Constructor ===============================================================
view::view(Surface* pHostSurface, iRect* viewArea, FrameType* world) 
          : m_pAttachedSurface(pHostSurface)
{
  ASSERT(NULL != m_pAttachedSurface);
  m_mtrxCamera.Identity();

  m_pBackground = NULL;
  m_fHScale = m_fVScale = 0.0f;

  if (viewArea)
    m_rctArea = *viewArea;

  DDrawDriver*  pDDDriver = pc3D::GetDDrawDriver();
  D3DDevice*    pDevice   = pc3D::Get3DDevice();

  ASSERT(NULL != pDDDriver);
  ASSERT(NULL != pDevice);

  // Create Viewport
  m_pViewport = pDDDriver->CreateViewport();
  ASSERT(NULL != m_pViewport);

  // Attach viewport to D3D device
  if (!pDevice->AddViewport(m_pViewport))
  {
    RELEASEPOINTER(m_pViewport);
    return;
  }

  // Set up Initial Viewport parameters
  if (!init())
  {
    pDevice->RemoveViewport(m_pViewport);
    RELEASEPOINTER(m_pViewport);
  }
}


// Destructor ================================================================
view::~view()
{
  removeLights();

  D3DDevice* pDevice = pc3D::Get3DDevice();
  ASSERT(NULL != pDevice);

  if (!pDevice->RemoveViewport(m_pViewport))
    DPF(DEBUG_ERROR, "error deleting viewport");

  RELEASEPOINTER(m_pViewport);

  DESTROYSURFACE(m_pBackground);
}

/*
**-----------------------------------------------------------------------------
**  Name:       D3DWindow::UpdateViewport
**  Purpose:    Keeps viewport updated with current window size
**  Notes:    
**
**  1. The viewport construction here assumes that you are rendering 
**    Triangles using the D3DVERTEX and D3DIM is doing Transform,
**    lighting, and rasterization for you.
**
**  2. If you are rendering triangles using D3DTLVERTEX and doing your
**     own transform and lighting then you need to setup the viewport
**     differently.   As follows:
**
**      // Replace the following values below:
**    dvClipX     = 0.0f;
**    dvClipY     = 0.0f;
**    dvClipWidth   = dwSurfW;
**    dvClipHeight  = dvSurfH;
**
**  3. This perserves the aspect ratio.  If you don't need or want to
**     perserve the aspect ratio then set inv_aspect = 1.0 below and
**     work this constant through the rest of the viewport setup.
**
**-----------------------------------------------------------------------------
*/

BOOL view::init()
{
  HRESULT         hResult;
  VIEWPORTDATA    viewportData;

  D3DDevice* pDevice = pc3D::Get3DDevice();
  ASSERT(NULL != pDevice);

  float inv_aspect = 1.0f;
  
  if (m_rctArea.xdim)
    inv_aspect = (float)(m_rctArea.ydim) / (float)(m_rctArea.xdim);

  // Update Viewport
  ZeroMemory(&viewportData, sizeof(VIEWPORTDATA));
  viewportData.dwSize       = sizeof(VIEWPORTDATA);
  viewportData.dwX          = m_rctArea.x;
  viewportData.dwY          = m_rctArea.y;
  viewportData.dwWidth      = m_rctArea.xdim;
  viewportData.dwHeight     = m_rctArea.ydim;
  viewportData.dvClipX      = -1.0f;
  viewportData.dvClipY      = inv_aspect;
  viewportData.dvClipWidth  = 2.0f;
  viewportData.dvClipHeight = 2.0f * inv_aspect;
  viewportData.dvMinZ       = 0.0f;
  viewportData.dvMaxZ       = 1.0f;

  // Update Viewport
  hResult = m_pViewport->SetViewport2(&viewportData);
  RETURN_ON_ERROR(hResult, FALSE);

  // Update D3D device to use this viewport
  return(pDevice->SetCurrentViewport(m_pViewport));
}


// ===========================================================================
// Function:    SetViewportRect
//
// Description: Set the rectangle of the viewport, but leave aspect ratio alone.
//
// Params:      rct:  The rectangle to use to set the viewport
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the viewport rect is successfully set, or
//              FALSE if it is not.
// ===========================================================================
BOOL view::SetViewportRect(const RECT& rct)
{
  VIEWPORTDATA viewportData;
  ZeroMemory(&viewportData, sizeof(VIEWPORTDATA));
  viewportData.dwSize       = sizeof(VIEWPORTDATA);

  ASSERT(NULL != m_pViewport);
  HRESULT hResult = m_pViewport->GetViewport2(&viewportData);
  RETURN_ON_ERROR(hResult, FALSE);

  viewportData.dwX          = rct.left;
  viewportData.dwY          = rct.top;
  viewportData.dwWidth      = abs(rct.right - rct.left);
  viewportData.dwHeight     = abs(rct.bottom - rct.top);
  hResult = m_pViewport->SetViewport2(&viewportData);
  RETURN_ON_ERROR(hResult, FALSE);

  // Make sure we update our internal rect to reflect the new
  //  viewport rectangle
  m_rctArea.x = rct.left;
  m_rctArea.y = rct.top;
  m_rctArea.xdim = viewportData.dwWidth;
  m_rctArea.ydim = viewportData.dwHeight;

  // If we have a background bitmap, we need to adjust the scale factors
  if (m_pBackground)
  {
    // Get the size of the background
    SIZE size;
    m_pBackground->GetSize(size);

    // Set our scaling factors accordingly
    m_fHScale = (WITHINTOLERANCE(m_rctArea.xdim)) ?
                float(size.cx) / float(m_rctArea.xdim) : 0.0f;
    m_fVScale = (WITHINTOLERANCE(m_rctArea.ydim)) ?
                float(size.cy) / float(m_rctArea.ydim) : 0.0f;
  }

  return(TRUE);
}


// ===========================================================================
// Function:    Clear
//
// Description: Clears the viewport.  Uses the "Clear()" function of 
//              IDirect3DViewport which will use whatever material is set as
//              the background to clear the viewport.
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the viewport is successfully cleared and FALSE
//              otherwise.
// ===========================================================================
BOOL view::Clear()
{
  // Do we have a surface to use as the background???
  if (m_pBackground)
  {
    RECT rct = { m_rctArea.x, m_rctArea.y, m_rctArea.xdim, m_rctArea.ydim };

    // Get the rendering surface
    Surface* pRenderSurface = pc3D::GetRenderSurface();
    ASSERT(NULL != pRenderSurface);

    RECT rctSrc = { 0, 0, 0 ,0 };
    SIZE* pSize = (SIZE*)(&rctSrc.right);
    m_pBackground->GetSize(*pSize);

    DDBLTFX ddBltFx;
    ZeroMemory(&ddBltFx, sizeof(DDBLTFX));
    ddBltFx.dwSize = sizeof(DDBLTFX);

    // Copy the background to the render surface
    if (!pRenderSurface->Blt(rct, m_pBackground->GetDDSurface(), rctSrc,
                             0, ddBltFx))
    {
      // The blit failed, so most likely it is not supported.
      //  Turn off the background.
      DESTROYSURFACE(m_pBackground);

      // Instead do a clear to black
      m_pViewport->Clear2(1ul, (D3DRECT*)(&rct), D3DCLEAR_TARGET, 0, 1.0f, 0);
    }

    return(ClearZBuffer());
  }
  else
  {
    D3DRECT rct = { m_rctArea.x, m_rctArea.y, m_rctArea.xdim, m_rctArea.ydim };

    // Otherwise, do a regular clear
    HRESULT hResult = m_pViewport->Clear(1ul, &rct, D3DCLEAR_TARGET | 
                                         D3DCLEAR_ZBUFFER);
    RETURN_ON_ERROR(hResult, FALSE);
  }

  return(TRUE);
}


// ===========================================================================
// Function:    Clear
//
// Description: Clears the viewport.  Uses the "Clear2()" function of 
//              IDirect3DViewport which will clear the viewport to the given
//              colour.  This version will ignore any material set as the
//              background.
//
// Params:      dwClr:    The colour we want to clear the viewport to
//              fZValue:  The value we want to set the ZBuffer to
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the viewport is successfully cleared and FALSE
//              otherwise.
// ===========================================================================
BOOL view::Clear(DWORD dwClr, D3DRECT* pRect /*= NULL*/, 
                 float fZValue /*= 1.0f*/)
{
  HRESULT hResult = NULL;

  // If we are given a rectangle to clear, use it.  Otherwise clear everything.
  if (pRect)
  {
    hResult = m_pViewport->Clear2(1ul, pRect, D3DCLEAR_TARGET |
                                  D3DCLEAR_ZBUFFER, dwClr, fZValue, 0);
  }
  else
  {
    D3DRECT rct = { m_rctArea.x, m_rctArea.y, m_rctArea.xdim, m_rctArea.ydim };
    hResult = m_pViewport->Clear2(1ul, &rct, D3DCLEAR_TARGET |
                                  D3DCLEAR_ZBUFFER, dwClr, fZValue, 0);
  }
  RETURN_ON_ERROR(hResult, FALSE);

  return(TRUE);
}


// ===========================================================================
// Function:    ClearZBuffer for rectangles.
//
// Description: Clears the ZBUffer of the viewport
//
// Params:      fZValue:  The value we want to set the ZBuffer to
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the viewport is successfully cleared and FALSE
//              otherwise.
// ===========================================================================
BOOL view::ClearZBuffer(D3DRECT* pRect /*= NULL*/, float fZValue /*= 1.0f*/)
{
  HRESULT hResult = NULL;

  // If we are given a rectangle to clear, use it.  Otherwise clear everything.
  if (pRect)
    hResult = m_pViewport->Clear2(1ul, pRect, D3DCLEAR_ZBUFFER, 0, fZValue, 0);
  else
  {
    D3DRECT rct = { m_rctArea.x, m_rctArea.y, m_rctArea.xdim, m_rctArea.ydim };
    hResult = m_pViewport->Clear2(1ul, &rct, D3DCLEAR_ZBUFFER, 0, fZValue, 0);
  }

  RETURN_ON_ERROR(hResult, FALSE);
  
  return(TRUE);
}



// ===========================================================================
// Function:    Clear for regions
//
// Description: Clears the specified portion of the ZBUffer of the viewport
//
// Params:      hRegion:  A Windows region (list of rectangles) to be cleared
//              fZValue:  The value we want to set the ZBuffer to
//              bClearZ:  TRUE to clear the Z buffer, FALSE to leave it alone
//              bClearTarget: TRUE if you also want the same parts of the
//                            screen to be cleared to the given colour
//              dwClr:    The colour we want to clear the background to
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the viewport is successfully cleared and FALSE
//              otherwise.
// ===========================================================================
BOOL view::Clear(HRGN hRegion, float fZValue /*= 1.0f*/, 
                 BOOL bClearZ /*= TRUE*/, BOOL bClearTarget /*= FALSE*/,
                 DWORD dwClr /*= 0xFF000000*/)
{
  const int knBufferSize = 10000;

  struct MyRegionStruct {
    RGNDATA header;
    BYTE    abyBuffer[knBufferSize - 1];
  };

  HRESULT        hResult  = NULL;
  LPD3DRECT      pRect    = NULL;
  MyRegionStruct regionData;

  if (!hRegion || (!bClearZ && !bClearTarget))
    return(TRUE); // Nothing to do.

  // Get the user provided region as rectangles
  hResult = ::GetRegionData(hRegion, knBufferSize, &regionData.header);
  if (hResult == 0 || regionData.header.rdh.iType != RDH_RECTANGLES)
    return FALSE; // Can't convert region to rectangles.

  pRect = (LPD3DRECT)(regionData.header.Buffer + 0);

  // If we are clearing the background and we have a background bitmap to
  //  clear to, we need to blit the background to the viewport rather than
  //  using solid colour.
  if (bClearTarget && m_pBackground)
  {
    RECT* prctDest  = (RECT*)pRect;
    RECT  rctSrc    = { 0, 0, 0 ,0 };

    // Get the rendering surface
    Surface* pRenderSurface = pc3D::GetRenderSurface();
    ASSERT(NULL != pRenderSurface);

    DDBLTFX ddBltFx;
    ZeroMemory(&ddBltFx, sizeof(DDBLTFX));
    ddBltFx.dwSize = sizeof(DDBLTFX);

    // Copy all of the areas of the background bitmap to the render surface
    for (UINT i = 0; i < regionData.header.rdh.nCount; i++, prctDest++)
    {
      // Scale the source rectangle to the size of the background
      rctSrc.left   = int((prctDest->left - m_rctArea.x)   * m_fHScale);
      rctSrc.top    = int((prctDest->top - m_rctArea.y)    * m_fVScale);
      rctSrc.right  = int((prctDest->right - m_rctArea.x)  * m_fHScale);
      rctSrc.bottom = int((prctDest->bottom - m_rctArea.y) * m_fVScale);

      // If either the width or height of the source rectangle is 0
      //  skip this rectangle
      if (0 == rctSrc.bottom  - rctSrc.top ||
          0 == rctSrc.right   - rctSrc.left)
      {
        continue;
      }

      // Copy the background to the render surface
      if (!pRenderSurface->Blt(*prctDest, m_pBackground->GetDDSurface(),
                               rctSrc, 0, ddBltFx))
      {
        // The blit failed, so most likely it is not supported.
        //  Turn off the background.
        DESTROYSURFACE(m_pBackground);
        break; // At least clear the Z buffer if things go wrong.
      }
    }

    // Clear the rectangles
    if (bClearZ)
    {
      hResult = m_pViewport->Clear2(regionData.header.rdh.nCount, pRect,
                                    D3DCLEAR_ZBUFFER, dwClr, fZValue, 0);
    }
  }
  else
  {
    // Clear the rectangles
    hResult = m_pViewport->Clear2(regionData.header.rdh.nCount, pRect,
                                  (bClearZ ? D3DCLEAR_ZBUFFER : 0) |
                                  (bClearTarget ? D3DCLEAR_TARGET : 0),
                                  dwClr, fZValue, 0);
  }
  RETURN_ON_ERROR(hResult, FALSE);

  return(TRUE);
}



// ===========================================================================
// Function:    SetBackground
//
// Description: Sets the background material of the viewport
//
// Params:      pMaterial:  A pointer to the material to set as the background
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void view::SetBackground(material* pMaterial)
{
  HRESULT hResult = m_pViewport->SetBackground((HMATERIAL)*pMaterial);
  REPORT_ON_ERROR(hResult);
}


// ===========================================================================
// Function:    SetBackgroundSurface
//
// Description: Sets the background surface for the viewport.  This is the
//              surface used to clear the viewport.
//
// Params:      pBackground:  Pointer to the surface to set as the background
//              bForce:       Forces the viewport to use the surface as the
//                            background regardless of the existence a
//                            hardware stretch blitter.
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the background is successfully set, or FALSE
//              if it cannot set the background.  If the hardware does not
//              support stretch blitting this call will fail.
// ===========================================================================
BOOL view::SetBackgroundSurface(Surface* pBackground, BOOL bForce /*= FALSE*/)
{
  // Destroy any previous background we may be using
  DESTROYSURFACE(m_pBackground);;

  // Are we turning off the background???
  if (!pBackground)
    return(TRUE);

  // Get the hardware capabilities of our DirectDraw driver
  DDrawDriver* pDDDriver = pc3D::GetDDrawDriver();
  ASSERT(NULL != pDDDriver);
  const DDCAPS* pddHardware = pDDDriver->GetHardwareCaps();

  // If we aren't forcing the background, do the stretch blit test
  if (!bForce)
  {
    // Check for hardware support of stretch blitting
    if (!(pddHardware->dwSVBCaps & DDCAPS_BLT && // Can we blit from system to video?
          pddHardware->dwCaps & DDCAPS_BLTSTRETCH)) // Can we do a stretch
    {
      // The hardware does not support stretch blitting
      TRACE("WARNING: The hardware does not support blit stretching.\n");
      TRACE("Basic driver caps: 0X%X\n", pddHardware->dwCaps);

      // We are responsible for the destruction of the surface so do
      //  that here
      DESTROYSURFACE(pBackground);

      return(FALSE);
    }
  }
  else
    TRACE("Forcing the background\n");

  // We're okay, set the background
  m_pBackground = pBackground;

  // Get the size of the background
  SIZE size;
  m_pBackground->GetSize(size);

  // Set our scaling factors accordingly
  m_fHScale = float(size.cx) / float(m_rctArea.xdim);
  m_fVScale = float(size.cy) / float(m_rctArea.ydim);

  return(TRUE);
}


BOOL view::findLight(Light* pLight)
{
  int i = -1, num = lightsInView.count();
  while (++i < num)
  {
    if (lightsInView[i] == pLight)
      return TRUE;
  }
  return FALSE;
}


void view::addLight(Light* pLight)
{
  HRESULT hResult;
  if (findLight(pLight))
    return;   // already in this view

  view* oldView = pLight->GetView();
  if (oldView)
    oldView->removeLight(pLight); //can't be in more than one view at a time

  hResult = m_pViewport->AddLight((PLIGHT)*pLight);
  QUIT_ON_ERROR(hResult);

  lightsInView.add(pLight);
  pLight->AddViewRef(this);
  pLight->Set();
}


void view::removeLight(Light* pLight)
{
  HRESULT hResult;

  if (!findLight(pLight))
    return;

  hResult = m_pViewport->DeleteLight((PLIGHT)*pLight);
  QUIT_ON_ERROR(hResult);

  lightsInView.detach(pLight);
  pLight->ReleaseViewRef(this);
}


void view::removeLights()
{
  HRESULT hResult;
  int i = lightsInView.count();
  while (i--)
  {
    hResult = m_pViewport->DeleteLight((PLIGHT)(*lightsInView[i]));
    QUIT_ON_ERROR(hResult);

    // detach at the end of the list first so the indices are still valid
    // (it's also faster... see the Cvectlist implementation)
    lightsInView.detach(lightsInView[i]);
    lightsInView[i]->ReleaseViewRef(this);
    lightsInView[i] = NULL;
  }
}


BOOL view::setViewMatrix(FrameType* camera)
{
  D3DDevice* pDevice = pc3D::Get3DDevice();
  ASSERT(NULL != pDevice);

  // setup the device's view matrix from the camera 
  vector  vPos;
  vector  vUp;
  vector  vDir;
  vector  vScale;
  Matrix  tmp;

  ASSERT(NULL != camera); // must have set a valid camera

  camera->GetPosition(&vPos);
  camera->GetOrientation(&vDir, &vUp);
  camera->GetScale(&vScale);

  // init the matrix which is equivalent to having a "frame" at a
  // specified position & orientation. This matrix can be used to 
  // convert other vertices to this frame's coordinate system.

  m_mtrxCamera.ViewMatrix(vPos, vPos+vDir, vUp);

  //  finally, postmultiply by the axis scale factors if necessary
  // acts as a camera 'zoom' (magnifies world)
  if (vScale.x != 1.0 || vScale.y != 1.0 || vScale.z != 1.0)
    m_mtrxCamera.Scale(vScale);

  // .......... finally set the device matrix to the newly computed one
  return(pDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &m_mtrxCamera));
}


BOOL view::setViewMatrix( D3DMATRIX *matrix )
{
  D3DDevice* pDevice = pc3D::Get3DDevice();
  ASSERT(NULL != pDevice);

  // .......... finally set the device matrix to the newly computed one
  return(pDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, matrix));
}


struct extent2DQuery
{
  view*   v;
  iRect   extent;
  BOOL    extentValid;
  FrameType*  camera;
  Box*    meshProxy;
};


BOOL updateExtent2D(MeshType* m, FrameType* local, void* data)
{
  HRESULT hResult;
  extent2DQuery& exQ = *((extent2DQuery *) data);

  // update the extent with the bounding boxes of meshes of this frame.
  // if there is a mesh "proxy" box, use that instead of the mesh's bounding box
  Box bounds;
  if (exQ.meshProxy)
    bounds = *(exQ.meshProxy);
  else
  {
#ifdef USE_OLD_FRAME
    m->getBox(bounds);
#else
    m->GetBoundingBox(bounds);
#endif // USE_OLD_FRAME
  }

  vector boxPts[8];
  bounds.getPts( boxPts );
  
  FrameType* f = local;

  D3DTRANSFORMDATA  td;
  D3DTLVERTEX       boxTL[8];
  D3DHVERTEX        boxH[8];
  DWORD offscreenClipped = FALSE;
  DWORD offscreenUnclipped = FALSE;

  PVIEWPORT pv = (PVIEWPORT) *(exQ.v);
  D3DDevice* pDevice = pc3D::Get3DDevice();
  ASSERT(NULL != pDevice);

  // set the world matrix to that of the mesh's frame...

  // either process the frame's matrix normally 
  // or adjust it because the frame's content is a billboard
  ASSERT(NULL != exQ.camera); // must provide a camera if there are billboards
  Matrix billboardWorldXform;
  if (f->CalcBillboardMatrix(exQ.camera, &billboardWorldXform))
  {
    VERIFY(pDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &billboardWorldXform));
  }
  else
    return TRUE; // no valid billboard matrix, don't update extent

  // and transform the mesh bounding box
  memset(boxTL, 0, sizeof(D3DTLVERTEX) * 8);
  td.dwSize     = sizeof(D3DTRANSFORMDATA);
  td.lpIn       = boxPts; 
  td.dwInSize   = sizeof(vector); 
  td.lpOut      = boxTL; 
  td.dwOutSize  = sizeof( D3DTLVERTEX ); 
  td.lpHOut     = boxH; 
  td.dwClip     = 0; 
  td.dwClipIntersection = 0; 
  td.dwClipUnion        = 0; 
  memset(&td.drExtent, 0, sizeof(D3DRECT)); 

  // transform clipped, to see if it is outside the frustum
  // (the box can tranform to on-screen coordinates but still be beyond the
  //  clipping planes)
  ATTEMPT(pv->TransformVertices(8, &td, D3DTRANSFORM_CLIPPED,
                                &offscreenClipped));

  // transform unclipped, to get the actual mathematical extents (ignoring
  //  clipping planes)
  ATTEMPT(pv->TransformVertices(8, &td, D3DTRANSFORM_UNCLIPPED,
                                &offscreenUnclipped));

  // update the extent info as a result of the query
  if (!offscreenUnclipped && !offscreenClipped)
  {
    iRect extent;
    if (!exQ.extentValid)
    {
      exQ.extent.x = (int) boxTL[0].sx; exQ.extent.y = (int) boxTL[0].sy; 
      exQ.extent.xdim = exQ.extent.ydim = 1;
      exQ.extentValid = TRUE;
    }

    // we have a valid extent rectangle, so add the new points to this extent
    iPoint newPt;
    for (int i=0; i < 8; i++)
    {
      newPt.x = (int) boxTL[i].sx;
      newPt.y = (int) boxTL[i].sy;
      exQ.extent.capture( newPt );
    }
  }
    
CLEANUP:
  return TRUE;
}



BOOL view::getExtent2D(FrameType* object, FrameType* camera, iRect* inViewport)
{
  extent2DQuery exQ;
  exQ.extent.setRect(0,0,0,0);
  exQ.v           = this;
  exQ.extentValid = FALSE;
  exQ.camera      = camera;
  exQ.meshProxy   = 0; // don't use a box, use the meshes' own bounding boxes

  // set the device viewing & projection matrices to correspond to the camera frame
  setViewMatrix( camera );

  // visit all the meshes in the object tree, transform their bounding boxes
  // to view coordinates and update the overall extent
  object->EnumMeshes( updateExtent2D, &exQ );

  // calc the extent's intersection with the viewport
  const iRect &viewArea = getViewArea();
  iRect result;
  viewArea.intersect(exQ.extent, result);

  *inViewport = result;
  return(exQ.extentValid ? TRUE : FALSE);
}



BOOL view::getExtent2DBox(FrameType* object, Box* objectBox, FrameType* camera,
                          iRect* inViewport)
{
  extent2DQuery exQ;
  exQ.extent.setRect(0,0,0,0);
  exQ.v           = this;
  exQ.extentValid = FALSE;
  exQ.camera      = camera;

  // use a box defined in the 'object' coordinate system instead of the mesh's
  //  own bounding box
  exQ.meshProxy = objectBox; 

  // set the device viewing & projection matrices to correspond to the camera
  //  frame
  setViewMatrix( camera );

  // calculate the 2D extent... don't visit all meshes, just provide the box
  //  in lieu of a mesh
  updateExtent2D(0, object, &exQ);

  // calc the extent's intersection with the viewport
  const iRect &viewArea = getViewArea();
  iRect result;
  viewArea.intersect(exQ.extent, result);

  *inViewport = result;
  return ( exQ.extentValid )? TRUE : FALSE;
}




