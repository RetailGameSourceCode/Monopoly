// ===========================================================================
// Module:      Scene.cpp
//
// Description: The basic function of a 'Scene' is to abstract the rendering
//              and 3D scene management process into a structure that
//              applications can either use, extend, or avoid entirely 
//              depending on their requirements. This is particularly 
//              necessary to support different rendering methods which may
//              rely on varying scene management techniques or calls to invoke
//              and manage hardware features. The goal is to allow an app to
//              decide how it wants to make use of the system's resources,
//              since no particular scheme works well for all cases. (see
//              testscene*.cpp for a sample usages)
//
// Copywrite:   Artech, 1999
// ===========================================================================
#include "PC3DHdr.h"
#include "Scene.h"

#include "l_Light.h"
#include "l_Material.h"

#ifdef USE_OLD_FRAME
#include "l_Frame.h"
#include "l_Mesh1.h"
#else
#include "NewFrame.h"
#include "NewMesh.h"
#endif


// ===========================================================================
// Typedefs and defines ======================================================
// ===========================================================================
#define INTERVAL 100

static struct
{
  HFONT   hFont;
  SIZE    szFrameRate;
  SIZE    szInfo;
} statglobs;


// ===========================================================================
// Global variables ==========================================================
// ===========================================================================

char    StatTxt[100];
int     StatTxtLen;
int     count         = 0;
int     last_polygons = 0;
int     this_frames   = 0;
time_t  last_time;
float   fps;
long    tps;


// ===========================================================================
// Local function prototypes =================================================
// ===========================================================================

void  ReleaseFont();
BOOL  InitFont();
void  ResetFrameRate();


// ===========================================================================
// Scene class ===============================================================
// ===========================================================================


// Constructor ===============================================================
Scene::Scene(Tag *desc) : m_pRView(NULL), m_pOldDevice(NULL), m_pRoot(NULL), 
                          m_pCamera(NULL), m_pDefaultMaterial(NULL),
                          m_bStatsOn(FALSE), m_fNearZ(NEAR_Z), m_fFarZ(FAR_Z), 
                          m_fViewingAngle(VIEWING_ANGLE)
{
  ASSERT(pc3D::IsInitialized());
  InitMeshes();
  InitTextures();
  InitMaterials();
  InitLights();
}


// Destructor ================================================================
Scene::~Scene()
{
  ReleaseFont();
  DestroyLights();
  DestroyMaterials();
  DestroyTextures();
  DestroyMeshes();
}


// ===========================================================================
// PRIVATE: Scene member functions ===========================================
// ===========================================================================


// ===========================================================================
// PROTECTED: Scene member functions =========================================
// ===========================================================================


// ===========================================================================
// Function:    InitMaterials
//
// Description: Initializes the materials for the scene
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Scene::InitMaterials()
{
  m_pDefaultMaterial = new material();
  m_pDefaultMaterial->SetDiffuse(CLRVALUE(1.0f, 1.0f, 1.0f));
  m_pDefaultMaterial->SetAmbient(CLRVALUE(1.0f, 1.0f, 1.0f));
}


// ===========================================================================
// Function:    DestroyLights
//
// Description: Destroys all of the lights in the scene
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Scene::DestroyLights()
{
  Light* pNext = NULL;
  while (m_Lights.count())
  {
    pNext = m_Lights[m_Lights.count()-1];
    m_Lights.detach(m_Lights.count()-1, 0);
    delete pNext;
  }
}


// ===========================================================================
// Function:    DestroyMaterials
//
// Description: Destroys all of the materials in the scene.
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Scene::DestroyMaterials()
{
  material* pNext = NULL;
  while (m_Materials.count())
  {
    pNext = m_Materials[m_Materials.count()-1];
    m_Materials.detach(m_Materials.count()-1, 0);
    delete pNext;
  }
  DESTROYPOINTER(m_pDefaultMaterial);
}


// ===========================================================================
// Function:    DestroyTextures
//
// Description: Destroys all of the textures in the scene.
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Scene::DestroyTextures()
{
  Surface* pNext = NULL;
  while (m_Textures.count())
  {
    pNext = m_Textures[m_Textures.count()-1];
    m_Textures.detach(m_Textures.count()-1, 0);
    DESTROYSURFACE(pNext);
  }
}


// ===========================================================================
// Function:    DestroyMeshes
//
// Description: Destroys all of the meshes in the scene.
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Scene::DestroyMeshes()
{
  MeshType* pNext = NULL;
  while (m_Meshes.count())
  {
    pNext = m_Meshes[m_Meshes.count()-1];
    m_Meshes.detach( m_Meshes.count()-1, 0);
    delete pNext;
  }
}


// ===========================================================================
// Function:    AttachMaterials
//
// Description: Attaches all of the materials in the scene to the viewport
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Scene::AttachMaterials()
{
  int i = -1;
  while (++i < m_Materials.count())
    m_Materials[i]->Attach(m_pRView);
  m_pDefaultMaterial->Attach(m_pRView);
}


// ===========================================================================
// Function:    PreRender
//
// Description: Called just before rendering the scene.  Used to update any
//              lights in the scene and process the textures.
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE unless overridden in a subclass.
// ===========================================================================
BOOL Scene::PreRender()
{
  SetupLightsinView();
  ProcessTextures();
  return TRUE;
}


// ===========================================================================
// Function:    PostRender
//
// Description: Called just after rendering the scene.
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE unless overridden in a subclass.
// ===========================================================================
BOOL Scene::PostRender()
{
  if (m_bStatsOn)
    return DrawStats();

  return TRUE;
}


// ===========================================================================
// Function:    Restore
//
// Description: Attempts to restore the primary surfaces
//
// Params:      void
//
// Returns:     HRESULT
//
// Comments:    
// ===========================================================================
HRESULT Scene::Restore()
{
  // remember to restore the source surfaces and the target render surface here
  return(D3D_OK);
}


// ===========================================================================
// Function:    RenderMesh
//
// Description: Renders a mesh
//
// Params:      pMesh:    Pointer to the mesh we want to render
//
// Returns:     HRESULT
//
// Comments:    Returns the Direct3D result of attempting to render the mesh
// ===========================================================================
HRESULT Scene::RenderMesh(MeshType* pMesh)
{
  HRESULT hResult = NULL;
  ASSERT(NULL != pMesh);

  // only render the basic "mesh1" for now
//  if (pMesh->getType() != 1)
//    return 0;

  m_pDefaultMaterial->SetAsCurrent();

//  hResult = (::render((mesh1*)pMesh, m_pRView )) ? D3D_OK : !D3D_OK;
  return hResult;
}


// ===========================================================================
// Function:    RenderFrame
//
// Description: Renders all of the meshes in a frame and all its child frames.
//
// Params:      pFrame:                 Pointer to the frame to render
//              pmtrxParent:            Pointer to parent matrix
//              bBillboardInFrameTree:  Indicates if the frame is a billboard
//                                      in the frame tree.
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the frame was render successfully or FALSE if
//              something went wrong.
// ===========================================================================
BOOL Scene::RenderFrame(FrameType* pFrame, Matrix* pmtrxParent /*= NULL*/,
                        BOOL bBillboardInFrameTree /*= FALSE*/)
{
  int       i, n, nNumMeshes;
  HRESULT   hResult;
  Matrix    forChildren;
  BOOL      bbInFrameTree = bBillboardInFrameTree;
  MeshType*     pMesh = NULL;

  ASSERT(NULL != m_pRView); // must be an active view
  ASSERT(NULL != pFrame); // must be a valid frame

  // set the world matrix to that of the mesh's frame

  // either adjust the frame's matrix because the frame's content is a billboard
  if (pFrame->IsBillboard() && m_pCamera)
  {
    if (!pFrame->CalcBillboardMatrix(m_pCamera, &forChildren))
      forChildren = *pFrame->GetWorldMatrix(); //  set to world matrix only if billboard is invalid
    bbInFrameTree = TRUE;
  }
  else  // or process the frame's matrix normally 
  {
    // optimization: if there's no billboard in the hierarchy up to this point,
    // just use the frame's world matrix, which could potentially be pre-calculated
    // by the frame class, (avoiding the matrix mult)
    if (bbInFrameTree)  
    {
      forChildren = *pFrame->GetMatrix();
      // calc the effective world matrix by multiplying local with previous world matrix
      if (pmtrxParent) 
        forChildren *= *pmtrxParent;
    }
    else
      forChildren = *pFrame->GetWorldMatrix();
  }

  // Get the 3D device
  D3DDevice* p3DDevice = pc3D::Get3DDevice();
  ASSERT(NULL != p3DDevice);

  // set the device world matrix state
  if (!p3DDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &forChildren))
    return(FALSE);

  nNumMeshes = pFrame->GetNumMeshes();
  n = -1;
  while (++n < nNumMeshes)
  {
    pMesh = pFrame->GetMesh(n);
    if (RejectMesh(pMesh))
      continue;

    hResult = RenderMesh(pMesh);
    if (hResult != D3D_OK)
      goto CLEANUP;
  }

  // render the children last, after computing the parent matrix
  n = pFrame->GetNumChildren();
  for (i = 0; i < n; i++)
  {
    if (!RenderFrame(pFrame->GetChild(i), &forChildren, bbInFrameTree))
      return FALSE;
  }

CLEANUP:
  return hResult == D3D_OK;
}


// ===========================================================================
// Function:    SetViewMatrix
//
// Description: Sets the camera frame.  Camera frame doesn't support offset
//              and scale orientation axis (i.e. offset must be (0,0,0), scale
//              orientation must be (0,0,1), scale angle must be 0)
//
// Params:      pCamera:  The camera to use to set our camera frame
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the camera frame was set successfully or FALSE
//              if it was not.
// ===========================================================================
BOOL Scene::SetViewMatrix(FrameType* pCamera)
{
  ASSERT(NULL != m_pRView); // must have an active view
  return m_pRView->setViewMatrix(pCamera);
}


// ===========================================================================
// Function:    SetProjectionMatrix
//
// Description: Sets the project matrix for the scene
//
// Params:      pMtrx:  Pointer to the projection matrix that will be used to
//                      set the projection matrix
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the matrix was set successfully, or FALSE if
//              it was not.
// ===========================================================================
BOOL Scene::SetProjectionMatrix(Matrix* pMtrx)
{
  ASSERT(NULL != pMtrx);

  // Get the 3D device
  D3DDevice* p3DDevice = pc3D::Get3DDevice();
  ASSERT(NULL != p3DDevice);

  // Set the world projection matrix of the 3D device
  return(p3DDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, pMtrx));
}


// ===========================================================================
// Function:    SetupWorldMatrixForFrame
//
// Description: Sets the device's world matrix according to the given frame. 
//              This is convenient for customized rendering procedures where
//              you want to avoid the usual depth first render of every mesh
//              in the hierarchy.  If 'bAllowForBillboards' is FALSE, the
//              matrix calc may occur faster because it can ignore the
//              possibility that the frame is a descendent of a frame that is
//              a billboard.
//
// Params:      pFrame:               The frame to use to setup the world
//                                    matrix
//              bAllowForBillboards:  Indicates if we are to take into account
//                                    the allowance of billboards
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the world matrix was setup successfully or
//              FALSE if it was not.
// ===========================================================================
BOOL Scene::SetupWorldMatrixForFrame(FrameType* pFrame,
                                     BOOL bAllowForBillboards /*= TRUE*/)
{
  Matrix forWorld;

  ASSERT(NULL != m_pRView);  // must be an active view
  ASSERT(NULL != pFrame);    // must be a valid frame

  // either adjust the frame's matrix because the frame's content is a billboard
  if (bAllowForBillboards && pFrame->IsBillboard() && m_pCamera)
  {
    if (!pFrame->CalcBillboardMatrix(m_pCamera, &forWorld))
      forWorld = *pFrame->GetWorldMatrix(); //  set to world matrix only if billboard is invalid
  }
  else
    // or process the frame's matrix normally 
    forWorld = *pFrame->GetWorldMatrix();

  // Get the 3D device
  D3DDevice* p3DDevice = pc3D::Get3DDevice();
  ASSERT(NULL != p3DDevice);

  // set the device world matrix state
  return(p3DDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &forWorld));
}


// ===========================================================================
// Function:    RejectMesh
//
// Description: Used to reject a mesh based on its bounding box.  Uses the
//              current device matrices to test for visibility on screen.  In
//              the D3D world, view & projection matrices must be set up
//              before calling this function.  NOTE: for small meshes, this
//              function can actually degrade performance because there is a
//              significant overhead in the D3D 'TransformVertices' call. 
//
// Params:      toReject:   Pointer to the mesh to test for rejection
//
// Returns:     BOOL
//
// Comments:    Recommended use is for large meshes only (what is large ?...
//              you may want to experiment with mesh sizes).  Returns FALSE if
//              any part of the mesh's bounding box is visible.  Otherwise,
//              Returns TRUE.
// ===========================================================================
BOOL Scene::RejectMesh(MeshType* pTestMesh)
{
  // transform the bounding box of the mesh using D3D's 'TranformVertices'
  // this is the cheap and easy way, but its just a generic trivial reject...
  // specific scene implementations can do their own trivial reject if necessary
  Box b;
#ifdef USE_OLD_FRAME
  pTestMesh->getBox(b);
#else
  pTestMesh->GetBoundingBox(b);
#endif // USE_OLD_FRAME
  vector ptsV[8];
  b.getPts(ptsV);

  D3DTLVERTEX outpts[8];
  D3DHVERTEX outptsH[8];

  D3DTRANSFORMDATA boxData;
  memset(&boxData, 0, sizeof (D3DTRANSFORMDATA));
  boxData.dwSize              = sizeof (D3DTRANSFORMDATA) ; 
  boxData.lpIn                = &ptsV[0]; 
  boxData.dwInSize            = sizeof(vector); 
  boxData.lpOut               = &outpts[0]; 
  boxData.dwOutSize           = sizeof(D3DTLVERTEX); 
  boxData.lpHOut              = &outptsH[0]; 
  boxData.dwClip              = 0; 
  boxData.dwClipIntersection  = 0; 
  boxData.dwClipUnion         = 0; 

  DWORD offScreen = 0;
  ASSERT( m_pRView ); // must be an active view
  PVIEWPORT viewport = (PVIEWPORT)*m_pRView;

  HRESULT hResult = viewport->TransformVertices(8, &boxData, D3DTRANSFORM_CLIPPED, &offScreen);
  if (hResult != D3D_OK)
    REPORTERR( hResult );
  return (offScreen != 0);
}


// ===========================================================================
// Function:    RejectMesh
//
// Description: Reject meshes without D3D's transform vertices (potentially
//              faster)
//
// Params:      pTestMesh:  Pointer to the mesh we are to test
//              mtrxWorld:  World matrix to use for the test
//              mtrxView:   Viewing matrix to use for the test
//              fViewAng:   Viewing angle (in radians) to use for the test
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the mesh is rejected (i.e. it does not lie
//              within the viewing frustum).  Returns FALSE otherwise.
//              NOTE: THIS FUNCTION ONLY PSEUDO-IMPLEMENTED!  Doesn;t work 
//              yet, something wrong with the xforms or something
// ===========================================================================
BOOL Scene::RejectMesh(MeshType* pTestMesh, const Matrix& mtrxWorld,
                       const Matrix& mtrxView, float fViewAng)
{
  // THIS FUNCTION ONLY PSEUDO-IMPLEMENTED !
  ASSERT(FALSE); return(FALSE);

  // matrix to convert to camera coordinates
  Matrix toCam = mtrxView * mtrxWorld;
    
  // transform the bounding box of the mesh to camera coordinates
  Box b;

#ifdef USE_OLD_FRAME
  pTestMesh->getBox(b);
#else
  pTestMesh->GetBoundingBox(b);
#endif // USE_OLD_FRAME

  vector pts[8], bCamPts[8];
  b.getPts(pts);

  // transform the points to the camera system
  toCam.TransVectors(pts, bCamPts, 8);

  // the points are now relative to a camera 
  // pointing at the positive z axis, where up vector is y axis
  // should we xform all 8 pts ? computational geometry to the rescue ?

  for (int i = 0; i < 8; i++)
  {
    if (bCamPts[i].z < 0.0 || bCamPts[i].z > 500.0)
    return TRUE;
  }

  return FALSE;
}


BOOL Scene::DrawInfo(void)
{
  char buf[40];

  DWORD dx,dy;
  m_pRView->getSurface()->GetSize(dx, dy);

  wsprintf(buf, "%dx%d (%s)", dx, dy, (pc3D::Using3DHardware()) ? 
           "Hardware" : "Software");

  return(OutputText(buf, 2, 16));
}


BOOL Scene::DrawFrameRate(float fps)
{
  char buf[60];

  if (fps > 0.0f)
  {
    wsprintf(buf, "%d.%02d fps", (int)(fps * 100) / 100,
             (int)(fps * 100) % 100);
  }
  else
    buf[0] = 0;

  return(OutputText(buf, 2, 0));
}


// ===========================================================================
// Function:    CalculateAndDrawFrameRate
//
// Description: Calculates and draws the frame rate on the sceen
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if successfull, FALSE otherwise.
// ===========================================================================
BOOL Scene::CalculateAndDrawFrameRate()
{
  // Calculate the frame rate and get other stats.
  count++;
  this_frames++;
  if (count == INTERVAL)
  {
    double    t;
    int       p = 0, f = 0;
    time_t    this_time;

    count     = 0;
    this_time = clock();
    t         = (this_time - last_time) / (double)CLOCKS_PER_SEC;
    last_time = this_time;

    f = this_frames;
    this_frames = 0;

    fps = (float)(f / t);
  }

  if (!DrawFrameRate(fps))
    return FALSE;            

  return TRUE;
}


// ===========================================================================
// Function:    OutputText
//
// Description: Renders a text string to the screen at a specific location.
//
// Params:      szText:   The text we want to render
//              x, y:     The horizontal and vertical position where we want
//                        the text to start.
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the text was successfully rendered, or FALSE
//              if it was not.
// ===========================================================================
BOOL Scene::OutputText(LPSTR szText, int x, int y)
{
  int len = strlen(szText);

  HDC hdc = m_pRView->getSurface()->GetDC();
  if (!hdc) return(FALSE);
  
  SelectObject(hdc, statglobs.hFont);
  SetTextColor(hdc, RGB(255,255,255));
  SetBkColor(hdc, RGB(0,0,0));
  SetBkMode(hdc, OPAQUE);
  GetTextExtentPoint32(hdc, szText, len, &statglobs.szFrameRate);
  
  ExtTextOut(hdc, x, y, ETO_OPAQUE, NULL, szText, len, NULL);
  VERIFY(m_pRView->getSurface()->ReleaseDC(hdc));

  return TRUE;
}


// ===========================================================================
// PUBLIC: Scene member functions ============================================
// ===========================================================================


// ===========================================================================
// Function:    Draw
//
// Description: Performs the actions to render the scene to the screen.
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the scene was drawn successfully, FALSE if not
// ===========================================================================
BOOL Scene::Draw()
{
  HRESULT hResult = DD_OK;

  PreRender();

  // Paint until we truly succeed or error out
  while (TRUE)
  {
    // render scene
    hResult = Render();

    if (SUCCEEDED(hResult))
    {
      // Success, exit
      break;
    }

    // Check if busy or drawing
    if ((DDERR_SURFACEBUSY == hResult) || (DDERR_WASSTILLDRAWING == hResult))
    {
      // Try again
      continue;
    }

    // Check for lost surfaces
    while (DDERR_SURFACELOST == hResult)
    {
      // Restore surfaces
      hResult = Restore();
    }

    // Check for real error
    RETURN_ON_ERROR(hResult, FALSE);
  }

  PostRender();

  // Success
  return TRUE;
}


// ===========================================================================
// Function:    SetStats
//
// Description: Turns the statistics on or off
//
// Params:      bOn: Boolean indicating whether to turn the stats on or off
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Scene::SetStats(BOOL bOn)
{
  if (!m_bStatsOn && bOn)
  {
    InitFont();
    ResetFrameRate();
  }
  m_bStatsOn = bOn;
}


// ===========================================================================
// Function:    SetRenderView
//
// Description: Sets the viewport for the scene.
//
// Params:      v:  Pointer to the viewport to set for the scene
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the viewport is successfully set, or FALSE if
//              it is not.
// ===========================================================================
BOOL Scene::SetRenderView(view* v)
{
  ASSERT(NULL != v);

  if (v == m_pRView)
    return TRUE;

  v->removeLights(); // detach any old lights (from other scenes, etc.)

  // We have a different view, but maybe the same device (surface).
  // If it is actually the same device, we don't need to setup 
  // the materials, etc. again.
  
  D3DDevice* pNewDevice = pc3D::Get3DDevice();

  if (!m_pRView || m_pOldDevice != pNewDevice)
  {
    m_pRView = v;
    AttachMaterials();
  }
  else
    m_pRView = v;

  SetupProjectionValues();
  m_pOldDevice = pNewDevice;
  return TRUE;
}


void Scene::AddTexture(Surface* pTexture)
{
  ASSERT(NULL != pTexture);

  // Must be a system memory surface
  ASSERT(pTexture->IsTexture()); 
  m_Textures.add(pTexture);
}


void Scene::RemoveTexture(Surface* pTexture)
{
  int index = m_Textures.find(pTexture);
  if (NOT_FOUND == index) return;
  m_Textures.detach(index, 0);
}



// ===========================================================================
// Local functions ===========================================================
// ===========================================================================


BOOL InitFont()
{
  HDC hdc;
  char dummyinfo[]  = "000x000x00 (MONO) 0000";
  char dummyfps[]   = "000.00 fps 00000000.00 tps 0000.00 mppps";
 
  // Create the font.
  if (statglobs.hFont != NULL) 
  {
    DeleteObject(statglobs.hFont);
    statglobs.hFont = NULL;
  }

  statglobs.hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                               ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                               VARIABLE_PITCH, "Arial");

  hdc = GetDC(NULL);
  SelectObject(hdc, statglobs.hFont);
  GetTextExtentPoint(hdc, dummyfps, strlen(dummyfps), &statglobs.szFrameRate);
  GetTextExtentPoint(hdc, dummyinfo, strlen(dummyinfo), &statglobs.szInfo);
  ReleaseDC(NULL, hdc);

  return(TRUE);
}


void ReleaseFont()
{
  if (NULL != statglobs.hFont) 
  {
    DeleteObject(statglobs.hFont);
    statglobs.hFont = NULL;
  }
}


void ResetFrameRate()
{
  last_time     = clock();
  count         = 0;
  last_polygons = 0;
  this_frames   = 0;
  fps           = 0.0f; 
  tps           = 0;
  StatTxt[0]    = 0;
  StatTxtLen    = 0;
}

// ===========================================================================
// End Of File ===============================================================
// ===========================================================================
