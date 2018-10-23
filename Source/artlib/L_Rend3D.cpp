/*****************************************************************************
 *
 * FILE:        L_Rend3D.c
 *
 * DESCRIPTION: Draws the models defined by an animation sequence tree onto
 *              the screen.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Rend3D.cpp 97    99-10-16 4:30p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Rend3D.cpp $
 * 
 * 97    99-10-16 4:30p Agmsmith
 * Now computes the screen bounding boxes correctly, using clipping when
 * the box is half behind the camera and half in front.
 *
 * 96    99-10-16 3:07p Agmsmith
 * Now projection and view and viewport matrices are part of the renderer
 * class, not hidden elsewhere, so we can quickly use them for bounding
 * box calculations.
 *
 * 95    99-10-15 4:41p Agmsmith
 * Remove unnecessary vertex buffers.
 *
 * 94    99-10-15 4:39p Agmsmith
 * Trying to get screen bounding rect for a mesh, with clipping.  Add
 * vertex buffers.  Then decide to avoid Direct3D and do our own math
 * (easier to precompute matrices).  Still haven't finished clipping for
 * points behind the camera, but got math working.
 *
 * 93    9/21/99 16:40 Davew
 * Needed to take advantage of the new Force parameter when setting the
 * background of the viewport.
 *
 * 92    21/09/99 3:45p Agmsmith
 * Fix missing update of bounding boxes for new items when camera isn't
 * moving.
 *
 * 91    21/09/99 2:29p Agmsmith
 * Nope, quick hack makes it crash.
 *
 * 89    9/20/99 12:01 Davew
 * Fixed a memory leak
 *
 * 88    9/20/99 11:36 Davew
 * Changed the new "ClearZBuffer" function to a Clear() function.  After
 * all, it was clearing the render target as well.
 *
 * 87    18/09/99 12:02p Agmsmith
 * Oops.
 *
 * 86    18/09/99 11:20a Agmsmith
 * Removed dead code, now uses sequence tree to control drawing and only
 * draws 3D models which intersect the invalid area (used to draw them
 * all).
 *
 * 85    17/09/99 3:59p Agmsmith
 * Under construction, now just clears only parts of the Z buffer it needs
 * cleared.
 *
 * 84    9/09/99 12:47 Davew
 * Added two functions to set the background of the viewport
 *
 * 83    99/09/02 12:50p Agmsmith
 * Get a 3D bounding box for a 3D animation.
 *
 * 82    8/26/99 9:59 Davew
 * Mesh rendering calls now take a pointer to a 3D device.
 *
 * 81    8/20/99 11:28a Agmsmith
 * Fixing SetViewport and SetCamera commands.
 *
 * 80    8/18/99 8:15p Agmsmith
 * Added SetCamera sequencer command.
 *
 * 79    8/17/99 11:02 Davew
 * Added a new function to set the viewing angle of the scene.
 *
 * 78    8/17/99 10:58 Davew
 * Cleaned things up quite a bit.  Added a new function to set the near
 * and far clipping planes.
 *
 * 77    8/16/99 5:24p Agmsmith
 * Adding viewport sequencer command.
 *
 * 76    8/09/99 15:37 Davew
 * Removed some stuff that wasn't being used.  Made some minor changes.
 *
 * 75    7/30/99 12:13p Agmsmith
 * It's a forwards vector, not a position to look at.
 *
 * 74    7/27/99 15:26 Davew
 * Changed things to use the new Frame/Mesh classes by default.  The old
 * frame/mesh classes can still be used by defining
 * CE_ARTLIB_3DUsingOldFrame to be 1 in C_ArtLib.h
 *
 * 73    7/19/99 16:20 Davew
 * Fixed a problem when locking surfaces and printing error messages.
 * Can't be done during a WinMutex lock.
 *
 * 72    7/19/99 9:53 Davew
 * Propagated changes made to the material class
 *
 * 71    7/06/99 14:09 Davew
 * Minor changes propagated from Pc3d
 *
 * 70    6/30/99 9:48 Davew
 * Fixed a memory leak with the scene. Added some code to count the number
 * of frames rendered.
 *
 * 69    6/28/99 12:43p Russellk
 * Added functions to return m_RendCamera location & orientation
 *
 * 68    6/25/99 11:55 Davew
 * Fixed the PC3D dependencies
 *
 * 67    6/22/99 16:38 Davew
 * Removed a number of calls to SetRenderState in favour of a more global
 * approach.  See RenderSettings in pc3D and
 * D3DDevice::SetInitialRenderStates
 *
 * 66    6/22/99 12:50 Davew
 * Dependency changes from pc3d
 *
 * 65    6/21/99 1:59p Agmsmith
 * Invalidate the whole scene when lighting changes, not just when camera
 * moves.
 *
 * 64    6/18/99 15:01 Davew
 * The surface class changed so I propagated the changes here.
 *
 * 63    6/15/99 15:06 Davew
 * Propigated changes made to the Scene class.
 *
 * 62    6/14/99 9:20p Agmsmith
 * Remove memory leak with allocating regions.
 *
 * 61    6/14/99 8:40p Agmsmith
 * Now the 3D camera works!
 *
 * 60    6/14/99 7:35p Agmsmith
 * Overhauled invalid area updating and use bounding boxes more
 * frequently.  All for making cameras visible in the dope sheet.
 *
 * 59    6/14/99 12:24p Agmsmith
 * 3D camera from a sequence is under construction.
 *
 * 58    6/09/99 2:02p Agmsmith
 * Accept only 3D meshes.
 *
 * 57    6/08/99 9:02a Agmsmith
 * Check for open render slot when setting camera.
 *
 * 56    6/07/99 1:56p Andrewx
 * Re-set the 'draw bounding boxes' if statement to a conditional ;)
 *
 * 55    6/07/99 1:51p Andrewx
 * Fixed the bounding box overlapping the non-render area.
 *
 * 54    5/31/99 11:48 Davew
 * Added in a sanity check in LE_REND3D_SetViewportRect()
 *
 * 53    5/31/99 11:32 Davew
 * Added new LE_REND3D_SetViewportRect function
 *
 * 52    5/26/99 17:13 Davew
 * Enabled dithering.  Fixed some logic.
 *
 * 51    5/26/99 15:26 Davew
 * Fixed a bug in the light stuff where it wasn't setting the light in the
 * array properly
 *
 * 50    5/26/99 2:07p Agmsmith
 *
 * 49    5/26/99 1:16p Andrewx
 * Modification for lighting work, and new camera move functions for
 * Mr.Smith.
 *
 * 48    5/25/99 10:01 Davew
 * Fixed the ambient light stuff
 *
 * 47    5/21/99 15:21 Davew
 * Converted to the new Matrix class in PC3D
 *
 * 46    5/20/99 4:00p Andrewx
 * Modifications for light support, and removal of code for VRML.
 *
 * 45    5/17/99 1:37p Andrewx
 * Modified for proper camera handling.
 *
 * 44    5/14/99 9:22 Davew
 * Updated the view stuff after having changed the view class
 * implementation.  Also added in a change for Alex.
 *
 * 43    5/06/99 10:30 Davew
 * Fixed all of the freakin' warnings
 *
 * 42    5/06/99 9:09 Davew
 * Changed the isInitialized() call in PC3D to IsInitialized()
 *
 * 41    5/04/99 2:12p Agmsmith
 * Slight cleanup.
 *
 * 40    5/04/99 12:24p Agmsmith
 * Close open render slots when exiting.
 *
 * 39    5/03/99 5:34p Agmsmith
 * Minor changes during bug hunt.
 *
 * 38    4/29/99 3:19p Andrewx
 * Fixed bounding boxes for DX6 using vertex buffers.
 *
 * 37    4/29/99 12:38p Andrewx
 * Added a flag for the 3d render slots which allow the hardware to clear
 * the viewport reight before rendering.
 *
 * 36    4/27/99 1:37p Agmsmith
 * Number of render slots is now user defined.
 *
 * 35    4/27/99 12:50p Andrewx
 * Added LE_CAMERA_MoveSmooth, it's a hack for now, but works well.
 *
 * 34    4/25/99 2:18p Agmsmith
 * That selection box thingy...
 *
 * 33    4/23/99 13:06 Davew
 * Updated to use the new DX types
 *
 * 32    4/22/99 16:49 Davew
 * Removed a unwanted and unneeded dependency
 *
 * 31    4/22/99 15:01 Davew
 * Updated to DX v6
 *
 * 30    4/16/99 11:46a Andrewx
 * Half fixed bounding boxes. *BLEAH*
 *
 * 29    4/15/99 3:19p Andrewx
 * Oops, left bounding boxes all turned on.
 *
 * 28    4/15/99 12:38p Andrewx
 * Modified to accomodate dave's changes in header files.
 *
 * 27    4/14/99 4:08p Andrewx
 * Added camera move function, bounding box returns, and camera plane
 * dragging.
 *
 * 26    4/12/99 2:24p Andrewx
 * Added support for new render loop (render3) which should boost
 * performance on hardware cards. Also added requested changes by Alex.
 *
 * 25    4/08/99 10:52a Andrewx
 * Modified so that the references for meshes contain the proper
 * bounding/interpolation values. When multiple copies of a single
 * sequence are started.
 *
 * 24    4/06/99 5:42p Andrewx
 * Added selection boxes and click areas to 3d renderer.
 *
 * 23    4/06/99 12:05p Andrewx
 * Modified for accurate bounding boxes.
 *
 * 22    3/30/99 4:26p Andrewx
 * Modified for interpolation values to work.
 *
 * 21    3/26/99 12:34p Andrewx
 * Fixed oddball bug, now 3D works off of the world matrix (so, we can
 * actually do 3d shit in the dope, and model a fully working robot or
 * some such)
 *
 * 20    3/25/99 6:22p Andrewx
 * Camera and object movement seperated out.
 *
 * 19    3/24/99 10:29a Andrewx
 * Fixed the camera movement for MESHX meshes.
 *
 * 18    3/23/99 3:07p Agmsmith
 * Use contentsDataID for model access, since sequence's DataID could be
 * pointing at sequence data, not a model.
 *
 * 17    3/23/99 11:03a Andrewx
 * Changed subDataID to contentsDataID as per l_seqncr.h definition of
 * LE_SEQNCR_RuntimeInfoPointer
 *
 * 16    3/23/99 10:57a Andrewx
 * Adding rect support.
 *
 * 15    3/18/99 10:57a Andrewx
 * Modified for HMD display
 *
 * 14    1/31/99 3:26p Agmsmith
 * Rearranged video player code so that feeding was done during
 * renderering, not in the sequencer, so that it can draw directly on the
 * screen.
 *
 * 13    1/05/99 5:00p Agmsmith
 * Renderer / sequencer interface converted from a bunch of arrays of
 * pointers to functions into C++ virtual function calls and an abstract
 * base class.  Now it should be easier to add new functionality.
 *
 * 12    12/30/98 2:58p Agmsmith
 * Don't flip also try to fix up exit cleanup, but fail.
 *
 * 11    12/03/98 2:20p Agmsmith
 * Update matrices after children added so that their dirty flags get set
 * and then they move around as the parent moves around.
 *
 * 10    12/03/98 12:20p Agmsmith
 * Now have textures working in software mode.
 *
 * 9     12/02/98 5:57p Agmsmith
 * Got first 3D rendering, but all in white, but it does show motion.
 *
 * 8     11/30/98 6:14p Agmsmith
 * Use new surface constructor for TABs from the data file.
 *
 * 7     11/27/98 3:15p Agmsmith
 * Compiles but doesn't work yet.
 *
 * 6     11/26/98 5:06p Agmsmith
 * Use current version of PC3D library (name changes back to PC3D names),
 * but still under construction.
 *
 * 5     11/10/98 12:35p Agmsmith
 * Better single tasking support.
 *
 * 4     9/28/98 10:45a Adamg
 * Added semi-propper texture handling
 *
 * 3     9/23/98 2:40p Adamg
 * compiles with Graphix3D
 *
 * 2     9/22/98 4:23p Agmsmith
 * Updated to work with ArtLib99.
 *
 ****************************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemGrafix3D

#define GsOT int

#include "l_can.h"
#include "l_cantyp.h"

#if CE_ARTLIB_3DUsingOldFrame
#define USE_OLD_FRAME
#endif CE_ARTLIB_3DUsingOldFrame

#include "../pc3d/PC3DHdr.h"

#include "../pc3d/camera.h"
#include "../pc3d/l_Light.h"
#include "../pc3d/l_Material.h"
#include "../pc3d/scene.h"
#include "../pc3d/l_view.h"

#if CE_ARTLIB_3DUsingOldFrame

#include "../pc3d/l_mesh1.h"
#include "../pc3d/hmdstuff.h"
#include "../pc3d/sys\types.h"
#include "../pc3d/libgte.h"
#include "../pc3d/libgpu.h"
#include "../pc3d/libgs.h"
#include "../pc3d/libhmd.h"
#include "../pc3d/hmdload.h"

#else // CE_ARTLIB_3DUsingOldFrame

#include "../pc3d/NewMesh.h"

#endif // CE_ARTLIB_3DUsingOldFrame


#ifdef RECORD_3DSTATS
  DWORD g_dwFramesRendered = 0;
#endif


/************************************************************/
/* Stuff missing from Rend3DCamera                          */
/*----------------------------------------------------------*/

struct Rend3DProjection {
  TYPE_Coord3D nearPlane;
  TYPE_Coord3D farPlane;
  TYPE_Angle2D fovAngle;
};



/************************************************************/
/* Scene which only this file uses.                         */
/*----------------------------------------------------------*/

class LI_REND3D_Scene : public Scene
{
public:
  LI_REND3D_Scene();
  virtual ~LI_REND3D_Scene();
  virtual BOOL SetRenderView (view *v);

private:
  BOOL turnOnZBuffer;

public:
  void render3 (HRGN InvalidArea, LE_SEQNCR_RuntimeInfoPointer RootPntr,
    LE_REND_RenderSlot SlotNumber, TYPE_RectPointer CameraRenderAreaPntr,
    BOOL ClearToBlackFirst);

  void setCamera(FrameType *cam);
  FrameType* getCamera( void );
  view* getView( void ) { return m_pRView; };
  void setCCamera(CCamera*cam);
  void updateProjectionMatrix (Matrix *ReturnedMatrix);

  void setZbufferON(BOOL on = TRUE);
};/* end class LI_REND3D_Scene */



/************************************************************/
/* STRUCTURES                                               */
/*----------------------------------------------------------*/

// This structure is associated with each active 3D render slot.

class LI_REND3D_SlotDataClass : public LI_REND_CommonScreenWriterRenderSlotClass
{
  // Member variables ========================================================
  public:
    CCamera*        m_pCamera;      // A pc3d camera, now being used only as a
                                    //  trivial camera at the origin.

    view*           m_pRend3DView;  // A pc3d view.  It's connected to the
                                    //  working surface.

    Rend3DCamera    m_RendCamera;   // Positioning of the eyepoint.

    Matrix          m_mtrxView;
      // Converts from world coordinates to camera coordinates.  In the
      // camera system, the camera is at (0,0,0) looking towards +Z and
      // the world is relative to that.

    Matrix          m_mtrxProjection;
      // Applies perspective to the camera coordinate system (things furthur
      // from the camera appear smaller - divides by Z in effect) to give one
      // where things at the near clip plane are at Z=0.0, things at the far
      // clip plane are at Z=1.0, things on the far left have X=-1.0, far
      // right is X=1.0, top is Y=1.0 and lowest visible Y is Y=-1.0.  Well
      // not quite true; the aspect ratio hasn't been applied, so this is
      // really only true for a square screen, wide non-square ones don't draw
      // the full range of Y coordinates, tall skinny ones probably draw more
      // than the -1 to +1 range of Y (not sure).

    Matrix          m_mtrxViewport;
      // This one converts from projected camera coordinates to actual screen
      // pixel coordinates.  Derived from the rectangle of screen real-estate
      // we are drawing into.

    TYPE_Matrix3D   m_mtrxCameraToScreen;
      // A derived matrix which converts from camera coordinates to pixels.

    BOOL            m_bRendCameraDirty;
      // Whether camera settings have changed or not.  If they do change, then
      // both m_mtrxView and m_mtrxProjection need recomputation and all
      // objects need new screen bounding boxes.

    BOOL            m_bWholeSceneDirty;
      // Set to TRUE when something changes the whole scene, things like a
      // lighting change.  Forces it to redraw all, but doesn't need to
      // recompute camera matrices or bounding boxes.

    DWORD           m_dwAmbientLight;   // Ambient light

    Light*          m_apLights[CE_ARTLIB_NumLights];  // Array of light objects

    LI_REND3D_Scene*  m_pRend3DScene;   // Scene object for the render slot

    BOOL            m_bInterpolatingCameraPos;
    int             m_nInterpolationSteps;
    int             m_nNumOfStepsTaken;
    Rend3DCamera    m_OriginalPos;
    Rend3DCamera    m_DestPos;
    BOOL            m_bClearRegion;


  // Construction / Destruction ==============================================
  public:
    // See also LE_REND3D_InstallInRenderSlot
    LI_REND3D_SlotDataClass() : m_dwAmbientLight(0xFFFFFFFF),
                                LI_REND_CommonScreenWriterRenderSlotClass() {}

  // Public member functions =================================================
  public:
    virtual BOOL SequenceStartUp(LE_SEQNCR_RuntimeInfoPointer SequencePntr);
    virtual void SequenceShutDown(LE_SEQNCR_RuntimeInfoPointer SequencePntr);
    virtual BOOL SequenceMoved(LE_SEQNCR_RuntimeInfoPointer SequencePntr);

    virtual BOOL CanHandleSequenceType(LE_SEQNCR_SequenceType SequenceType,
                                       int Dimensionality);

    virtual void UpdateInvalidAreas(LE_SEQNCR_RuntimeInfoPointer RootPntr);
    virtual void DrawInvalidAreas(LE_SEQNCR_RuntimeInfoPointer RootPntr);
    virtual void AddInvalidArea(HRGN InvalidAreaRegion, PDDSURFACE ScreenSurface);

    virtual HRGN GetSolidArea(LE_SEQNCR_RuntimeInfoPointer RootPntr,
                              PDDSURFACE ScreenSurface);

    virtual BOOL SequenceTouchesScreenRectangle(LE_SEQNCR_RuntimeInfoPointer SequencePntr,
                                                TYPE_RectPointer TouchingRectanglePntr);

    virtual BOOL GetScreenBoundingRectangle(LE_SEQNCR_RuntimeInfoPointer SequencePntr,
                                            TYPE_RectPointer ScreenRectPntr);

    virtual BOOL DragOnCameraPlane(LE_SEQNCR_RuntimeInfoPointer SequencePntr,
                                   TYPE_Point2DPointer DragFromVirtualScreenPointPntr,
                                   TYPE_Point2DPointer DragToVirtualScreenPointPntr,
                                   TYPE_Point3DPointer ResultingDeltaVector);

    virtual UNS8 GetCameraLabelNumber(void);
    virtual BOOL SetViewport (const TYPE_Rect *ViewportRectPntr); // Please don't change prototype unless you edit L_REND.H base class too.
    virtual BOOL SetCamera (UNS8 CameraNumber, TYPE_Point3DPointer Position, TYPE_Point3DPointer Forwards, TYPE_Point3DPointer Up, TYPE_Angle3D FovAngle, TYPE_Coord3D NearZ, TYPE_Coord3D FarZ);
    virtual BOOL Uninstall(void);
    virtual PDDSURFACE GetWorkingSurface(void);


    // Internal camera setting, just sets it and returns, no animated cameras etc.
    BOOL  InternalSetCamera (TYPE_Point3DPointer Position, TYPE_Point3DPointer Forwards, TYPE_Point3DPointer Up, TYPE_Angle3D FovAngle, TYPE_Coord3D NearZ, TYPE_Coord3D FarZ);

    // For interpolating cameras.
    BOOL  UpdateSmoothCamera(void); // Return TRUE if we changed the camera.
    BOOL  UpdateSequencedCamera(void); // For animated cameras.

    // Dirty area updating.
    void InvalidateAllSequences (LE_SEQNCR_RuntimeInfoPointer RootPntr,
      BOOL JustOnesNeedingRedraw);

    void ClipAndAddRectangleToInvalidArea (LPRECT RectPntr);

    BOOL LI_REND3D_GetMeshScreenRect (MeshType *myMesh,
      RECT *ScreenRectPntr,
      TYPE_Matrix3DPointer ModelToCameraMatrix,
      TYPE_Matrix3DPointer CameraToScreenMatrix);
};

typedef LI_REND3D_SlotDataClass* LI_REND3D_SlotDataPointer;

typedef struct Private3DDataStruct
{
  TYPE_Rect boundingBox;  // Screen coordinates it covers, zero width for offscreen.
  FrameType   *myFrame;
#if CE_ARTLIB_3DUsingOldFrame
  meshx       *reference;
#else // CE_ARTLIB_3DUsingOldFrame
  Mesh*       reference;
#endif // CE_ARTLIB_3DUsingOldFrame
} Private3DData;



/************************************************************/
/* STATIC VARIABLES                                         */
/*----------------------------------------------------------*/

static LI_REND3D_SlotDataClass SlotDataArray [CE_ARTLIB_RendMaxRenderSlots];
static HRGN GlobalTempRegion;



/************************************************************/
/* Big blob for LI_REND3D_Scene                             */
/*----------------------------------------------------------*/

LI_REND3D_Scene::LI_REND3D_Scene ()
: Scene (NULL), turnOnZBuffer(TRUE)
{
  m_Materials.add (m_pDefaultMaterial);
  m_pRoot = new FrameType ();
}



LI_REND3D_Scene::~LI_REND3D_Scene()
{
  // To avoid deleting it twice...
  m_Materials.detach(m_pDefaultMaterial);

  DESTROYPOINTER(m_pRoot);

#ifdef RECORD_3DSTATS
  TRACE("\n\t\tNumber of frames rendered: %d\n\n", g_dwFramesRendered);
#endif
}



BOOL LI_REND3D_Scene::SetRenderView(view *v)
{
  assert(NULL != v);

  m_pRView = v;
  AttachMaterials();

  // Note that projection matrix gets set with camera matrix, not here,
  // since this is now a one-time init thing.

  return TRUE;
}


void LI_REND3D_Scene::setCamera(FrameType *cam)
{
  m_pCamera = cam;
}



FrameType *LI_REND3D_Scene::getCamera()
{
  return m_pCamera;
}



void LI_REND3D_Scene::setCCamera( CCamera * cam )
{
  m_pCamera = cam->getCamera();
}



void LI_REND3D_Scene::updateProjectionMatrix (Matrix *ReturnedMatrix)
{
  ReturnedMatrix->ProjectionMatrix (m_fNearZ, m_fFarZ, m_fViewingAngle);
  SetProjectionMatrix (ReturnedMatrix);
}



void LI_REND3D_Scene::setZbufferON (BOOL on)
{
  turnOnZBuffer = on;
}



/************************************************************/
/* SEQUENCER CALLBACK FUNCTION IMPLEMENTATIONS              */
/*----------------------------------------------------------*/

BOOL LI_REND3D_SlotDataClass::SequenceStartUp(
                              LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  Private3DData *PrivateDataPntr;

  if (SequencePntr->dimensionality != 3)
    return TRUE; // Nothing to do for 0D groups etc.

  PrivateDataPntr = (Private3DData *)
    LI_MEMORY_AllocLibraryBlock (sizeof (Private3DData));
  if (PrivateDataPntr == NULL) return FALSE;

  SequencePntr->renderSlotData [mySlotNumber] = PrivateDataPntr;
  memset (PrivateDataPntr, 0, sizeof (Private3DData));

  switch (SequencePntr->sequenceType)
  {
    case LE_CHUNK_ID_SEQ_3DMESH:
    {
      FrameType *myFrame;

#if CE_ARTLIB_3DUsingOldFrame
      meshx* myMesh = *(meshx**)LE_DATA_UseRef(SequencePntr->contentsDataID);
#else // CE_ARTLIB_3DUsingOldFrame
      Mesh* myMesh = *(Mesh**)LE_DATA_UseRef(SequencePntr->contentsDataID);
#endif // CE_ARTLIB_3DUsingOldFrame

      if( myMesh == NULL )
      {
#if CE_ARTLIB_EnableDebugMode
        LE_ERROR_DebugMsg("LI_REND3D_SequenceStart: DATAID not loaded.\r\n",
                          LE_ERROR_DebugMsgToFileAndScreen);
#endif
        LI_MEMORY_DeallocLibraryBlock (PrivateDataPntr);
        SequencePntr->renderSlotData [mySlotNumber] = NULL;
        return FALSE;
      }

#if CE_ARTLIB_3DUsingOldFrame
      PrivateDataPntr->reference = myMesh->copy( HMD_COPYREFERENCE );
#else // CE_ARTLIB_3DUsingOldFrame
      PrivateDataPntr->reference = myMesh;
#endif // CE_ARTLIB_3DUsingOldFrame

      myFrame = new FrameType();
      myFrame->AddMesh(PrivateDataPntr->reference);
      PrivateDataPntr->myFrame = myFrame;

#if CE_ARTLIB_3DUsingOldFrame
      myMesh->HMD_interpolate (
        SequencePntr->dimensionSpecificData.d3->meshIndexA,
        SequencePntr->dimensionSpecificData.d3->meshIndexB,
        SequencePntr->dimensionSpecificData.d3->meshProportion);
#else // CE_ARTLIB_3DUsingOldFrame
      myMesh->Interpolate(SequencePntr->dimensionSpecificData.d3->meshIndexA,
                          SequencePntr->dimensionSpecificData.d3->meshIndexB,
                          SequencePntr->dimensionSpecificData.d3->meshProportion);
#endif // CE_ARTLIB_3DUsingOldFrame

      // Note that all startups get a SequenceMoved to initialise position,
      // so don't bother to override the matrix here.

      break;
    }

  default:
    break; // Leave private data all set to NULL, only bounding box will be used.
  }
  return TRUE;
}



void LI_REND3D_SlotDataClass::SequenceShutDown (
LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  Private3DData *privateData;

  privateData = (Private3DData *) SequencePntr->renderSlotData [mySlotNumber];
  if (privateData == NULL) return; // Nothing to do for 0D groups etc.

  switch (SequencePntr->sequenceType)
  {
  case LE_CHUNK_ID_SEQ_3DMESH:
    DESTROYPOINTER(privateData->myFrame);

#if CE_ARTLIB_3DUsingOldFrame
    DESTROYPOINTER(privateData->reference);
#else
    privateData->reference = NULL;
#endif

    LE_DATA_RemoveRef (SequencePntr->contentsDataID);
    break;
  }

  // Add the old screen area to the invalid area so that the stuff
  // behind the disappeared thing gets redrawn.

  ClipAndAddRectangleToInvalidArea (&privateData->boundingBox);

  LI_MEMORY_DeallocLibraryBlock (privateData);
  SequencePntr->renderSlotData [mySlotNumber] = NULL;
}



/*****************************************************************************
 * Move the sequence, return TRUE if it is visible on the screen.
 */

BOOL LI_REND3D_SlotDataClass::SequenceMoved (
LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  FrameType          *myFrame;

#if CE_ARTLIB_3DUsingOldFrame
  meshx           *myMesh;
#else // CE_ARTLIB_3DUsingOldFrame
  Mesh*           myMesh;
#endif // CE_ARTLIB_3DUsingOldFrame

  Private3DData  *PrivateDataPntr;

  PrivateDataPntr = (Private3DData *)
    SequencePntr->renderSlotData [mySlotNumber];
  if (PrivateDataPntr == NULL) return FALSE; // Ignore 0D groups etc.

  // Invalidate the old position, if any.

  ClipAndAddRectangleToInvalidArea (&PrivateDataPntr->boundingBox);

  // Move the sequence, update mesh vertices for interpolation too.

  switch (SequencePntr->sequenceType)
  {
  case LE_CHUNK_ID_SEQ_3DMESH:
    myFrame = PrivateDataPntr->myFrame;
    myMesh = PrivateDataPntr->reference;
    if( myMesh == NULL ) return FALSE;
#if CE_ARTLIB_3DUsingOldFrame
    myMesh->HMD_interpolate(SequencePntr->dimensionSpecificData.d3->meshIndexA,
                             SequencePntr->dimensionSpecificData.d3->meshIndexB,
                             SequencePntr->dimensionSpecificData.d3->meshProportion );
#else // CE_ARTLIB_3DUsingOldFrame
    myMesh->Interpolate(SequencePntr->dimensionSpecificData.d3->meshIndexA,
                        SequencePntr->dimensionSpecificData.d3->meshIndexB,
                        SequencePntr->dimensionSpecificData.d3->meshProportion );
#endif // CE_ARTLIB_3DUsingOldFrame
    myFrame->OverrideMatrix( &SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D );
    break;
  }

  // No need to add the new invalid area to the list, since it will be done
  // later when all things needing redraw have their areas invalidated (moved
  // items are also automatically marked as needing a redraw and thus get
  // invalidated by UpdateInvalidAreas).

  return TRUE; // Bleeble: really should figure out if on screen for scrolling worlds usage.
}



BOOL LI_REND3D_SlotDataClass::CanHandleSequenceType (
LE_SEQNCR_SequenceType SequenceType, int Dimensionality)
{
  switch (SequenceType)
  {
  case LE_CHUNK_ID_SEQ_3DMESH:
  case LE_CHUNK_ID_SEQ_CAMERA:
    return (Dimensionality == 3);

  case LE_CHUNK_ID_SEQ_GROUPING:
  case LE_CHUNK_ID_SEQ_INDIRECT:
    return (Dimensionality == 0 || Dimensionality == 3);
  }
  return FALSE;
}



/*****************************************************************************
 * Modifies your rectangle to fit on the screen (or sets it to the empty
 * rectangle if it doesn't) and adds it to the invalid area of the slot.
 */

void LI_REND3D_SlotDataClass::ClipAndAddRectangleToInvalidArea (LPRECT RectPntr)
{
  RECT  IntersectedRect;

  if (IntersectRect (&IntersectedRect, RectPntr, &cameraRenderArea))
  {
    // Save the chopped down rectangle.
    *RectPntr = IntersectedRect;

    if (invalidArea == NULL)
    {
      invalidArea = CreateRectRgn (
        IntersectedRect.left, IntersectedRect.top,
        IntersectedRect.right, IntersectedRect.bottom);
    }
    else
    {
      SetRectRgn (GlobalTempRegion,
        IntersectedRect.left, IntersectedRect.top,
        IntersectedRect.right, IntersectedRect.bottom);

      CombineRgn (invalidArea, invalidArea, GlobalTempRegion, RGN_OR);
    }

  }
  else // Offscreen rectangle, make it empty.
    memset (RectPntr, 0, sizeof (RECT));
}



/*****************************************************************************
 * Updates the bounding box of changed sequences and adds the bounding box
 * of ones needing redraw (or all) of them to the invalid area of the slot.
 */

void LI_REND3D_SlotDataClass::InvalidateAllSequences (
LE_SEQNCR_RuntimeInfoPointer RootPntr, BOOL JustOnesNeedingRedraw)
{
  TYPE_Rect                     EnclosingRect;
  TYPE_Matrix3D                 ModelToCameraMatrix;
  Private3DData                *PrivateDataPntr;
  LE_REND_RenderSlotSet         RenderMask;
  LE_REND_RenderSlot            RenderSlot;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;
  TYPE_Point3D                  TempPoint;
  HRGN                          TempRegion;

  // Initialise variables.

  RenderSlot = mySlotNumber;
  RenderMask = mySlotSetMask;
  SequencePntr = RootPntr;

  TempRegion = CreateRectRgn (0, 0, 0, 0);
  if (TempRegion == NULL) return;

  // Traverse the sequences tree.

  while (TRUE)
  {
    // Process the current node.  This is optimised for handling the usual
    // case of lots of nodes where only a few need redrawing.  So efficiently
    // traverse the tree and simplify the initial tests.

    if ((SequencePntr->renderSlotSet & RenderMask) &&
    (!JustOnesNeedingRedraw || SequencePntr->needsRedraw))
    {
      if ((PrivateDataPntr = (Private3DData *)
      SequencePntr->renderSlotData[RenderSlot]) != NULL)
      {
        // The default bounding box is none, zero width and height.
        memset (&PrivateDataPntr->boundingBox, 0, sizeof (PrivateDataPntr->boundingBox));

        if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_3DMESH)
        {
          LE_MATRIX_3D_MatrixMultiply (&SequencePntr->dimensionSpecificData.d3->
            sequenceToWorldTransformation.matrix3D,
            &m_mtrxView, &ModelToCameraMatrix);

          // Add the mesh's on-screen area to the collected area.

          if (LI_REND3D_GetMeshScreenRect (PrivateDataPntr->reference,
          &EnclosingRect, &ModelToCameraMatrix, &m_mtrxCameraToScreen))
          {
            // Trim off the stuff which isn't visible in the current display,
            // that includes stuff off the screen and off the camera's
            // rendering area (which could be off the screen too if we
            // are running in windowed mode).

            PrivateDataPntr->boundingBox = EnclosingRect;
            ClipAndAddRectangleToInvalidArea (&PrivateDataPntr->boundingBox);
          }
        }
        else if (SequencePntr->drawSelectionBox)
        {
          // A normally invisible grouping object or camera. Invent a
          // bounding box centered on the location (origin) of the object.

          memset (&TempPoint, 0, sizeof (TempPoint));

          // Convert to camera relative coordinates.

          LE_MATRIX_3D_VectorMultiply (
            &TempPoint,
            &SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D,
            &TempPoint);

          LE_MATRIX_3D_VectorMultiply (
            &TempPoint,
            &m_mtrxView,
            &TempPoint);

          // See if it is in front of the camera and maybe visible.

          if (TempPoint.z >= m_pRend3DScene->m_fNearZ)
          {
            // Get the position in screen coordinates.

            LE_MATRIX_3D_VectorMultiply (
              &TempPoint,
              &m_mtrxCameraToScreen,
              &TempPoint);

            EnclosingRect.left = (long) TempPoint.x - 5;
            EnclosingRect.right = EnclosingRect.left + 10;
            EnclosingRect.top = (long) TempPoint.y - 5;
            EnclosingRect.bottom = EnclosingRect.top + 10;

            PrivateDataPntr->boundingBox = EnclosingRect;
            ClipAndAddRectangleToInvalidArea (&PrivateDataPntr->boundingBox);
          }
        }
      }

      // Advance to the next sequence, which includes children.

      if (SequencePntr->child != NULL)
        SequencePntr = SequencePntr->child;
      else // Move along to the next sibling.
      {
        while (SequencePntr != NULL && SequencePntr->sibling == NULL)
          SequencePntr = SequencePntr->parent;
        if (SequencePntr == NULL)
          break; // All done.
        SequencePntr = SequencePntr->sibling;
      }
    }
    else
    {
      // Advance to the next sequence, excluding children since we know
      // the current sequence isn't interesting to us therefore its
      // children are also irrelevant.

      while (SequencePntr != NULL && SequencePntr->sibling == NULL)
        SequencePntr = SequencePntr->parent;
      if (SequencePntr == NULL)
        break; // All done.
      SequencePntr = SequencePntr->sibling;
    }
  }

  DeleteObject( TempRegion );
}



void LI_REND3D_SlotDataClass::UpdateInvalidAreas (
LE_SEQNCR_RuntimeInfoPointer RootPntr)
{
  LI_REND_AbstractRenderSlotPointer OtherSlotPntr;
  LE_REND_RenderSlotSet             RenderMask;
  LE_REND_RenderSlot                RenderSlot;
  LE_SEQNCR_RuntimeInfoPointer      SequencePntr;

  RenderMask = mySlotSetMask; // Local variable for faster access.
  SequencePntr = RootPntr;

  // Update the camera position using assorted techniques, stops
  // at the one which returns TRUE.

  if (!UpdateSmoothCamera ())
    UpdateSequencedCamera ();

  // Make sure the view matrix is up-to-date.

  if (m_bRendCameraDirty)
  {
    // Recalculate the view matrix (direction & location) and
    // projection (zoom factor) matrices, which both depend on
    // the camera settings.

    m_mtrxView.ViewMatrix(m_RendCamera.location,
                          m_RendCamera.location + m_RendCamera.forwardVector,
                          m_RendCamera.upVector, m_RendCamera.rollAngle);

    m_pRend3DView->setViewMatrix(&m_mtrxView);

    m_pRend3DScene->updateProjectionMatrix (&m_mtrxProjection);

    // Calculate a few derived matrices, used to save time elsewhere.

    LE_MATRIX_3D_MatrixMultiply (&m_mtrxProjection, &m_mtrxViewport,
      &m_mtrxCameraToScreen);
  }

  // Invalidate all parts of the screen that have moving 3D models on them,
  // or the whole screen if something global (camera, lights) has changed.

  if (m_bRendCameraDirty || m_bWholeSceneDirty)
  {
    // Mark our whole drawing area as invalid, since everything has changed.
    ClipAndAddRectangleToInvalidArea(&cameraRenderArea);

    // Also update all bounding boxes if the camera has moved, which is
    // part of the InvalidateAllSequences operation (it should be efficient
    // since adding a bounding rectangle to a whole screen rectangle is
    // trival).  Still need to update the bounding boxes for new items,
    // even if the camera doesn't move, so always call the function.

    InvalidateAllSequences (RootPntr,
      !m_bRendCameraDirty /* JustOnesNeedingRedraw */);

    m_bRendCameraDirty = FALSE;
    m_bWholeSceneDirty = FALSE;
  }
  else // Nothing has changed, just invalidate moving things.
    InvalidateAllSequences (RootPntr, TRUE /* JustOnesNeedingRedraw */);

  // Tell other render slots about our changes.

  if (invalidArea != NULL)
  {
    for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
    {
      if (overlappingRenderSlotSet & (1 << RenderSlot))
      {
        OtherSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot];
        if (OtherSlotPntr != NULL)
          OtherSlotPntr->AddInvalidArea (invalidArea, outputSurface);
      }
    }
  }
}



void LI_REND3D_SlotDataClass::DrawInvalidAreas (
LE_SEQNCR_RuntimeInfoPointer RootPntr)
{
  int                           BoxSide;
  HRESULT                       ErrorCode;
  int                           i;
  BOOL                          InOurSlot;
  Private3DData                *PrivateDataPntr;
  LE_REND_RenderSlotSet         RenderMask;
  TYPE_Rect                     SelectionBoxRectangle;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;
  DDSURFDESC                    SurfaceDescriptor;

  if (invalidArea == NULL)
    return; // Nothing to do.

  RenderMask = mySlotSetMask;
  SequencePntr = RootPntr;

#if CE_ARTLIB_EnableMultitasking
  if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL) return; // System is closed.
  EnterCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
  if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL) return; // Closed while we waited.
#endif

  D3DDevice* pDevice = pc3D::Get3DDevice();
  ASSERT(NULL != pDevice);
  pDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, m_dwAmbientLight);

  for( i = 0; i < CE_ARTLIB_NumLights; ++i )
  {
    if( m_apLights[ i ] )
    {
      m_apLights[ i ]->Set();
    }
  }

  // Draw the 3D stuff.  Only draw things which intersect
  // the invalid area, skip drawing the rest of them since they
  // don't get copied to the real screen from the work screen.

  m_pRend3DScene->render3 (invalidArea, RootPntr, mySlotNumber,
    &cameraRenderArea, m_bClearRegion);

  // Now draw the bounding box markers over the images.  Done after
  // everything else drawn so that they don't get obscured by things
  // over them in the draw order.

  memset (&SurfaceDescriptor, 0, sizeof (SurfaceDescriptor));
  SurfaceDescriptor.dwSize = sizeof (SurfaceDescriptor);

  ErrorCode = workingSurface->Lock (
    NULL /* Ask for whole surface */, &SurfaceDescriptor,
    DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_NOSYSLOCK, NULL);

  if (ErrorCode != DD_OK)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND3D_SlotDataClass::DrawInvalidAreas: "
      "Unable to lock working area.\r\n", LE_ERROR_DebugMsgToFile);
#endif
    goto ErrorExit;
  }

  workingBitsPntr = SurfaceDescriptor.lpSurface;
  workingDepth = (UNS16) SurfaceDescriptor.ddpfPixelFormat.dwRGBBitCount;
  workingPixelsToNextScanLine =
    (UNS16) (SurfaceDescriptor.lPitch / (workingDepth / 8));
  workingBytesToNextScanLine = (WORD)SurfaceDescriptor.lPitch;

  while (TRUE)
  {
    // Process the current node.
    InOurSlot = ((SequencePntr->renderSlotSet & RenderMask) != 0);

    if (InOurSlot &&
    SequencePntr->drawSelectionBox &&
    (PrivateDataPntr = (Private3DData *) SequencePntr->renderSlotData[mySlotNumber]) != NULL &&
    PrivateDataPntr->boundingBox.left < PrivateDataPntr->boundingBox.right)
    {
      // Draw the bounding box.

      for (BoxSide = 3; BoxSide >= 0; BoxSide--)
      {
        SelectionBoxRectangle = PrivateDataPntr->boundingBox;
        if (BoxSide == 0) // Top
          SelectionBoxRectangle.bottom = SelectionBoxRectangle.top + 1;
        else if (BoxSide == 1) // Right
          SelectionBoxRectangle.left = SelectionBoxRectangle.right - 1;
        else if (BoxSide == 2) // Bottom
          SelectionBoxRectangle.top = SelectionBoxRectangle.bottom - 1;
        else // Left
          SelectionBoxRectangle.right = SelectionBoxRectangle.left + 1;

        switch (workingDepth)
        {
        case 16:
          LI_BLT_Fill16Raw ((DWORD) -1 /* White */, TRUE /* Native colour */,
            (BYTE *) workingBitsPntr,
            workingPixelsToNextScanLine,
            LE_GRAFIX_VirtualScreenHeightInPixels,
            &SelectionBoxRectangle);
          break;

        case 24:
          LI_BLT_Fill24Raw ((DWORD) -1 /* White */, TRUE /* Native colour */,
            (BYTE *) workingBitsPntr,
            workingPixelsToNextScanLine,
            LE_GRAFIX_VirtualScreenHeightInPixels,
            &SelectionBoxRectangle);
          break;

        case 32:
          LI_BLT_Fill32Raw ((DWORD) -1 /* White */, TRUE /* Native colour */,
            (BYTE *) workingBitsPntr,
            workingPixelsToNextScanLine,
            LE_GRAFIX_VirtualScreenHeightInPixels,
            &SelectionBoxRectangle);
          break;
        }
      }
    }

    // Traverse down the tree, then do siblings, then back up to
    // the next parent's sibling.

    if (SequencePntr->child != NULL && InOurSlot)
      SequencePntr = SequencePntr->child;
    else // Move along to the next sibling.
    {
      while (SequencePntr != NULL && SequencePntr->sibling == NULL)
        SequencePntr = SequencePntr->parent;
      if (SequencePntr == NULL)
        break; // All done.
      SequencePntr = SequencePntr->sibling;
    }
  }


ErrorExit:

  if (workingBitsPntr != NULL)
  {
    ErrorCode = workingSurface->Unlock(NULL/*workingBitsPntr*/);
    workingBitsPntr = NULL;
  }

  // Get ready for the next sequencer update pass.

  if (invalidArea != NULL)
  {
    DeleteObject (invalidArea);
    invalidArea = NULL;
  }

#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
#endif
  return;
}



void LI_REND3D_SlotDataClass::AddInvalidArea (
HRGN InvalidAreaRegion, PDDSURFACE ScreenSurface)
{
  RECT  AddedBoundingRect;
  RECT  IntersectedRect;

  if (ScreenSurface != NULL && outputSurface != ScreenSurface)
    return; // Some other screen surface, ignore this bogus update.

  if (InvalidAreaRegion == NULL)
    return; // Nothing to do.

  if (GetRgnBox (InvalidAreaRegion, &AddedBoundingRect) == 0)
    return; // Bad region.

  if (!IntersectRect (&IntersectedRect, &AddedBoundingRect, &cameraRenderArea))
    return; // Doesn't intersect our screen area.

  // Generate a temporary region which is the part of the given
  // region which overlaps our screen area.

  SetRectRgn (GlobalTempRegion,
    cameraRenderArea.left, cameraRenderArea.top,
    cameraRenderArea.right, cameraRenderArea.bottom);

  CombineRgn (GlobalTempRegion, InvalidAreaRegion, GlobalTempRegion, RGN_AND);

  // Add it to our invalid area.

  if (invalidArea == NULL)
  {
    // Swap regions, leave the GlobalTempRegion allocated with a dummy.
    invalidArea = GlobalTempRegion;
    GlobalTempRegion = CreateRectRgn (0, 0, 0, 0);
  }
  else // Have existing region to combine it with.
  {
    CombineRgn (invalidArea, invalidArea, GlobalTempRegion, RGN_OR);
  }
}



HRGN LI_REND3D_SlotDataClass::GetSolidArea (
LE_SEQNCR_RuntimeInfoPointer RootPntr, PDDSURFACE ScreenSurface)
{
  TYPE_Rect                     EnclosingRect;
  BOOL                          InOurSlot;
  Private3DData                *PrivateDataPntr;
  LE_REND_RenderSlotSet         RenderMask;
  HRGN                          ReturnedRegion;
  HRGN                          TempRegion;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;

  ReturnedRegion = CreateRectRgn (0, 0, 0, 0);

  if ((ScreenSurface != NULL && outputSurface != ScreenSurface) ||
  ReturnedRegion == NULL)
    return ReturnedRegion; // Some other screen surface, not relevant to us.

  TempRegion = CreateRectRgn (0, 0, 0, 0);
  if (TempRegion == NULL)
    return ReturnedRegion;

  RenderMask = mySlotSetMask;
  SequencePntr = RootPntr;

  while (TRUE)
  {
    // Process the current node.

    InOurSlot = ((SequencePntr->renderSlotSet & RenderMask) != 0);

    if (InOurSlot && (PrivateDataPntr = (Private3DData *)
    SequencePntr->renderSlotData[mySlotNumber]) != NULL)
    {
      if (IntersectRect (&EnclosingRect, &PrivateDataPntr->boundingBox,
      &cameraRenderArea))
      {
        SetRectRgn (TempRegion,
          EnclosingRect.left, EnclosingRect.top,
          EnclosingRect.right, EnclosingRect.bottom);

        CombineRgn (ReturnedRegion, ReturnedRegion, TempRegion, RGN_OR);
      }
    }

    // Traverse down the tree, then do siblings, then back up to
    // the next parent's sibling.

    if (SequencePntr->child != NULL && InOurSlot)
      SequencePntr = SequencePntr->child;
    else // Move along to the next sibling.
    {
      while (SequencePntr != NULL && SequencePntr->sibling == NULL)
        SequencePntr = SequencePntr->parent;
      if (SequencePntr == NULL)
        break; // All done.
      SequencePntr = SequencePntr->sibling;
    }
  }

  DeleteObject (TempRegion);
  return ReturnedRegion;
}



/*****************************************************************************
 * Gets the camera number.
 */

UNS8 LI_REND3D_SlotDataClass::GetCameraLabelNumber (void)
{
  return labelOfCameraToUse;
}



/*****************************************************************************
 * Return the working surface, if there is one.  NULL on failure.
 */

PDDSURFACE LI_REND3D_SlotDataClass::GetWorkingSurface (void)
{
  return workingSurface;
}



/*****************************************************************************
 * Change the viewport, and thus the screen area used.
 */

BOOL LI_REND3D_SlotDataClass::SetViewport (const TYPE_Rect *ViewportRectPntr)
{
  float AspectRatio;

  m_bRendCameraDirty = TRUE; // Recalculate camera settings (bounding boxes) & redraw too.

  cameraRenderArea = *ViewportRectPntr;

  AspectRatio =
    (cameraRenderArea.right - cameraRenderArea.left) /
    (float) (cameraRenderArea.bottom - cameraRenderArea.top);

  LE_MATRIX_3D_SetIdentity (&m_mtrxViewport);
  m_mtrxViewport._11 = (float) ((cameraRenderArea.right - cameraRenderArea.left) / 2);
  m_mtrxViewport._22 = AspectRatio * (float) ((cameraRenderArea.top - cameraRenderArea.bottom) / 2); // Note negative to flip.
  m_mtrxViewport._41 = (float) (cameraRenderArea.left + (cameraRenderArea.right - cameraRenderArea.left) / 2);
  m_mtrxViewport._42 = (float) (cameraRenderArea.top + (cameraRenderArea.bottom - cameraRenderArea.top) / 2);

  ASSERT(NULL != m_pRend3DView); if (!m_pRend3DView) return(FALSE);

  m_pRend3DView->SetViewportRect(cameraRenderArea);

  return(TRUE);
}



/*****************************************************************************
 * Change the camera settings, user style.  Called by sequencer.
 */

BOOL LI_REND3D_SlotDataClass::SetCamera (UNS8 CameraNumber,
TYPE_Point3DPointer Position, TYPE_Point3DPointer Forwards,
TYPE_Point3DPointer Up, TYPE_Angle3D FovAngle, TYPE_Coord3D NearZ,
TYPE_Coord3D FarZ)
{
  labelOfCameraToUse = CameraNumber;

  // Only use the provided coordinates if there isn't a camera object,
  // otherwise the user would have to specify reasonable values when
  // just switching objects.

  if (CameraNumber == 0)
    return InternalSetCamera (Position, Forwards, Up, FovAngle, NearZ, FarZ);

  return TRUE;
}



/*****************************************************************************
 * Change the camera settings, internal use.
 */

BOOL LI_REND3D_SlotDataClass::InternalSetCamera (
TYPE_Point3DPointer Position, TYPE_Point3DPointer Forwards,
TYPE_Point3DPointer Up, TYPE_Angle3D FovAngle, TYPE_Coord3D NearZ,
TYPE_Coord3D FarZ)
{
  m_bRendCameraDirty              = TRUE;
  m_RendCamera.location           = *Position;
  m_RendCamera.forwardVector      = *Forwards;
  m_RendCamera.upVector           = *Up;
  m_RendCamera.rollAngle          = 0.0f; // Obsolete, we use up vector.
  m_pRend3DScene->m_fViewingAngle = FovAngle;
  m_pRend3DScene->m_fNearZ        = NearZ;
  m_pRend3DScene->m_fFarZ         = FarZ;

  return TRUE;
}



/*****************************************************************************
 * Deinstall the render in the given slot.
 */

BOOL LI_REND3D_SlotDataClass::Uninstall (void)
{
  BOOL  ReturnCode;

  // Disconnect from the sequencer first.  This stops sequences too,
  // which will call our own sequence shutdown callback repeatedly.

  ReturnCode = LI_REND_CommonScreenWriterRenderSlotClass::Uninstall ();

  // Remove references to the drawing surfaces and other things.

  RELEASEPOINTER(workingSurface);
  RELEASEPOINTER(outputSurface);

  if (invalidArea != NULL)
  {
    DeleteObject (invalidArea);
    invalidArea = NULL;
  }

  // Delete everything we created during installation.
  if (m_pCamera != NULL)
  {
    if (m_pCamera->getCamera() != NULL)
    {
      delete m_pCamera->getCamera();
      m_pCamera->setCamera (NULL);
    }
    delete m_pCamera;
    m_pCamera = NULL;
  }

  // Destroy any lights that may have been left behind
  for (int i = 0; i < CE_ARTLIB_NumLights; i++)
  {
    // Remove the light from the viewport
    if (m_apLights[i])
      m_pRend3DView->removeLight(m_apLights[i]);
    DESTROYPOINTER(m_apLights[i]);
  }

  DESTROYPOINTER(m_pRend3DView);
  DESTROYPOINTER(m_pRend3DScene);

  return ReturnCode;
}



/*****************************************************************************
 * Draw all the 3D models in this slot.  First clears the Z buffer, but only
 * the parts overlapping the invalid area (the rest doesn't matter since
 * things drawn there don't get copied to the real screen).  That way slow
 * systems don't waste time clearing a huge screen bitmap.  Then draw the
 * 3D models, but only ones whose bounding boxes intersect the invalid area
 * and which are in the given render slot set.
 */

void LI_REND3D_Scene::render3 (HRGN InvalidArea,
LE_SEQNCR_RuntimeInfoPointer RootPntr, LE_REND_RenderSlot SlotNumber,
TYPE_RectPointer CameraRenderAreaPntr, BOOL ClearToBlackFirst)
{
  TYPE_Rect                     EnclosingRect;
  HRESULT                       hResult;
  BOOL                          InOurSlot;
  Private3DData                *PrivateDataPntr;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;
  LE_REND_RenderSlotSet         SlotMask;

  // .......... get the device
  D3DDevice* p3DDevice = pc3D::Get3DDevice();
  ASSERT(NULL != p3DDevice);

  // Clear the Z buffer, if needed.  Also clear the screen to black
  // if the user wants to do that.

  if (turnOnZBuffer || ClearToBlackFirst)
  {
    if (!m_pRView->Clear(InvalidArea, 1.0F, turnOnZBuffer, ClearToBlackFirst))
      return;
  }

  // Begin Scene
  // possible errors include lost surfaces
  if (!p3DDevice->BeginScene())
    return;

  #if CE_ARTLIB_3DUsingOldFrame
    HMD_ClearCollected();
  #endif

  // Iterate through all active 3D objects and draw the ones whose
  // bounding boxes intersect the invalid area.  Would be nice
  // to only redraw the parts in the invalid area, but we can't easily
  // do that so each thing gets fully redrawn (artists should break
  // large things up into smaller parts).

  SequencePntr = RootPntr;
  SlotMask = (1 << SlotNumber);

  while (TRUE)
  {
    // Process the current node.

    InOurSlot = ((SequencePntr->renderSlotSet & SlotMask) != 0);

    if (InOurSlot &&
    SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_3DMESH &&
    (PrivateDataPntr = (Private3DData *) SequencePntr->
    renderSlotData[SlotNumber]) != NULL)
    {
      if (IntersectRect (&EnclosingRect, &PrivateDataPntr->boundingBox,
      CameraRenderAreaPntr) && RectInRegion (InvalidArea, &EnclosingRect))
      {
        // Draw this object since it intersects the invalid area.
        // Really would only like to draw the portion inside EnclosingRect
        // but have to make do with drawing the whole thing and only
        // copying the portion in the invalid area to the screen later on.

        #if CE_ARTLIB_3DUsingOldFrame
          HMD_CollectForRendering (PrivateDataPntr->myFrame);
        #else
          p3DDevice->SetTransform (D3DTRANSFORMSTATE_WORLD, PrivateDataPntr->myFrame->GetWorldMatrix());
          PrivateDataPntr->myFrame->GetMesh (PrivateDataPntr->myFrame->GetNumMeshes ())->Render (p3DDevice);
        #endif // CE_ARTLIB_3DUsingOldFrame
      }
    }

    // Traverse down the tree, then do siblings, then back up to
    // the next parent's sibling.

    if (SequencePntr->child != NULL && InOurSlot)
      SequencePntr = SequencePntr->child;
    else // Move along to the next sibling.
    {
      while (SequencePntr != NULL && SequencePntr->sibling == NULL)
        SequencePntr = SequencePntr->parent;
      if (SequencePntr == NULL)
        break; // All done.
      SequencePntr = SequencePntr->sibling;
    }
  }

  #if CE_ARTLIB_3DUsingOldFrame
    HMD_RenderCollected(m_pRView);
  #endif // CE_ARTLIB_3DUsingOldFrame

  #ifdef RECORD_3DSTATS
    g_dwFramesRendered++;
  #endif

  // End Scene
  // Note:  This is complicated by the need to restore lost surfaces
  if (!p3DDevice->EndScene())
  {
    hResult = Restore();
    REPORT_ON_ERROR(hResult);
  }
}



BOOL LI_REND3D_SlotDataClass::SequenceTouchesScreenRectangle (
  LE_SEQNCR_RuntimeInfoPointer SequencePntr,
  TYPE_RectPointer TouchingRectanglePntr)
{
  TYPE_Rect                   *BoundingRectangle;
  Private3DData               *PrivateDataPntr;

  PrivateDataPntr =
    (Private3DData *) SequencePntr->renderSlotData[mySlotNumber];
  if (PrivateDataPntr == NULL)
    return FALSE;

  BoundingRectangle = &PrivateDataPntr->boundingBox;

  if (TouchingRectanglePntr->left >= TouchingRectanglePntr->right)
  {
    // Point test - is the point inside the bounding rectangle?

    return (
      TouchingRectanglePntr->left >= BoundingRectangle->left &&
      TouchingRectanglePntr->right <= BoundingRectangle->right &&
      TouchingRectanglePntr->top >= BoundingRectangle->top &&
      TouchingRectanglePntr->bottom <= BoundingRectangle->bottom);
  }

  // True rectangle test, is the sequence's bounding rectangle fully
  // inside the touching rectangle?

  return (
    TouchingRectanglePntr->left <= BoundingRectangle->left &&
    TouchingRectanglePntr->right >= BoundingRectangle->right &&
    TouchingRectanglePntr->top <= BoundingRectangle->top &&
    TouchingRectanglePntr->bottom >= BoundingRectangle->bottom);
}

BOOL LI_REND3D_SlotDataClass::GetScreenBoundingRectangle (
  LE_SEQNCR_RuntimeInfoPointer SequencePntr,
  TYPE_RectPointer ScreenRectPntr)
{
  Private3DData               *PrivateDataPntr;

  PrivateDataPntr =
    (Private3DData *) SequencePntr->renderSlotData[mySlotNumber];
  if (PrivateDataPntr == NULL)
    return FALSE;

  //Copy the bounding rect to the output.
  *ScreenRectPntr = PrivateDataPntr->boundingBox;

  return TRUE;
}

BOOL LI_REND3D_SlotDataClass::DragOnCameraPlane (
  LE_SEQNCR_RuntimeInfoPointer SequencePntr,
  TYPE_Point2DPointer DragFromVirtualScreenPointPntr,
  TYPE_Point2DPointer DragToVirtualScreenPointPntr,
  TYPE_Point3DPointer ResultingDeltaVector)
{
    TYPE_Point3D    rightAngleVector;
    TYPE_Point3D    upAngleVector;

    TYPE_Point2D    screenDifference;

    LE_MATRIX_3D_NormalizeVector( &m_RendCamera.upVector, &upAngleVector );
    LE_MATRIX_3D_VectorCrossProduct( &m_RendCamera.forwardVector, &m_RendCamera.upVector, &rightAngleVector );

    LE_MATRIX_3D_NormalizeVector( &rightAngleVector, &rightAngleVector );

    screenDifference.x = DragToVirtualScreenPointPntr->x - DragFromVirtualScreenPointPntr->x;
    screenDifference.y = DragToVirtualScreenPointPntr->y - DragFromVirtualScreenPointPntr->y;

    ResultingDeltaVector->x = ( screenDifference.x * rightAngleVector.x ) +
                              ( screenDifference.y * upAngleVector.x );
    ResultingDeltaVector->y = ( screenDifference.x * rightAngleVector.y ) +
                              ( screenDifference.y * upAngleVector.y );
    ResultingDeltaVector->z = ( screenDifference.x * rightAngleVector.z ) +
                              ( screenDifference.y * upAngleVector.z );


    return TRUE;
}


#define INTERP(X,Y,V) ( ( ( (Y) - (X) ) * V ) + (X) )

BOOL LI_REND3D_SlotDataClass::UpdateSmoothCamera( void )
{
    float percentIndex;

    if (!m_bInterpolatingCameraPos)
      return FALSE; // Nope, we didn't update it.

    m_nNumOfStepsTaken++;

    if (m_nNumOfStepsTaken >= m_nInterpolationSteps)
    {
        m_RendCamera              = m_DestPos;
        m_bInterpolatingCameraPos = FALSE;
        m_bRendCameraDirty        = TRUE;
        return TRUE; // Yes, we changed it.
    }

    percentIndex = (float)m_nNumOfStepsTaken / (float)m_nInterpolationSteps;

    m_RendCamera.location.x = INTERP( m_OriginalPos.location.x, m_DestPos.location.x, percentIndex);
    m_RendCamera.location.y = INTERP( m_OriginalPos.location.y, m_DestPos.location.y, percentIndex);
    m_RendCamera.location.z = INTERP( m_OriginalPos.location.z, m_DestPos.location.z, percentIndex);
    m_RendCamera.forwardVector.x = INTERP( m_OriginalPos.forwardVector.x, m_DestPos.forwardVector.x, percentIndex);
    m_RendCamera.forwardVector.y = INTERP( m_OriginalPos.forwardVector.y, m_DestPos.forwardVector.y, percentIndex);
    m_RendCamera.forwardVector.z = INTERP( m_OriginalPos.forwardVector.z, m_DestPos.forwardVector.z, percentIndex);
    m_RendCamera.upVector.x = INTERP( m_OriginalPos.upVector.x, m_DestPos.upVector.x, percentIndex);
    m_RendCamera.upVector.y = INTERP( m_OriginalPos.upVector.y, m_DestPos.upVector.y, percentIndex);
    m_RendCamera.upVector.z = INTERP( m_OriginalPos.upVector.z, m_DestPos.upVector.z, percentIndex);
    LE_MATRIX_3D_NormalizeVector( &m_RendCamera.forwardVector, &m_RendCamera.forwardVector );
    LE_MATRIX_3D_NormalizeVector( &m_RendCamera.upVector, &m_RendCamera.upVector );
    m_bRendCameraDirty = TRUE;
    return TRUE; // We changed it.
}



/*****************************************************************************
 * Update the camera using the labeled camera sequence object.  If no camera
 * then return FALSE and don't change it.  The labeled sequence's position and
 * orientation set the camera's position and orientation, the field of view
 * angle (for zooming in and out) is taken from auxiliary camera data if it
 * is a camera object, or just defaults to 45 degrees for other objects.
 */

BOOL LI_REND3D_SlotDataClass::UpdateSequencedCamera (void)
{
  LE_SEQNCR_Runtime3DCameraInfoPointer  CameraDataPntr;
  LE_SEQNCR_RuntimeInfoPointer          CameraSeqPntr;
  float                                 FarClip;
  float                                 FieldOfView;
  TYPE_Point3D                          InputPoint;
  float                                 NearClip;
  TYPE_Point3D                          OutputForwards;
  TYPE_Point3D                          OutputLocation;
  TYPE_Point3D                          OutputUp;
  static TYPE_Scale3D                   PreviousFOV;
  static TYPE_Matrix3D                  PreviousMatrix;
  LE_SEQNCR_Runtime3DInfoPointer        Seq3DInfo;

  // See if there is a labeled camera.  For that matter, any 3D object
  // can specify position and rotation, but only cameras have FOV.

  if (labelOfCameraToUse > 0 &&
  labelOfCameraToUse < CE_ARTLIB_SeqncrMaxLabels &&
  (CameraSeqPntr = LE_SEQNCR_LabelArray[labelOfCameraToUse]) != NULL &&
  CameraSeqPntr->dimensionality == 3 &&
  (Seq3DInfo = CameraSeqPntr->dimensionSpecificData.d3) != NULL)
  {
    // And now for the scale factor, which only gets updated from
    // the sequence if it is a real camera.

    if (CameraSeqPntr->sequenceType == LE_CHUNK_ID_SEQ_CAMERA)
    {
      FieldOfView = CameraSeqPntr->aux.cameraFieldOfView;
      CameraDataPntr = CameraSeqPntr->streamSpecificData.cameraData;
    }
    else
    {
      FieldOfView =  0.7853981633974F; // 45 degrees in radians.
      CameraDataPntr = NULL;
    }

    // And the clipping plane distances.

    if (CameraDataPntr == NULL)
    {
      NearClip = 1.0F;
      FarClip = 5000.0F;
    }
    else
    {
      NearClip = CameraDataPntr->nearClipPlaneDistance;
      FarClip = CameraDataPntr->farClipPlaneDistance;
    }

    if (PreviousFOV != FieldOfView ||
    memcmp (&PreviousMatrix, &Seq3DInfo->sequenceToWorldTransformation.matrix3D, sizeof (PreviousMatrix)) != 0)
    {
      // Camera settings have changed.

      PreviousFOV = FieldOfView;
      PreviousMatrix = Seq3DInfo->sequenceToWorldTransformation.matrix3D;

      // The camera starts out at the origin orientated to look down the Z
      // axis, with Y axis being upwards.  The sequence to world matrix
      // modifies the forwards and up vectors to give us the world
      // coordinates.  We want to ignore scaling, so the resulting
      // vectors get normalised before use.

      // Set the location.

      memset (&InputPoint, 0, sizeof (InputPoint));
      LE_MATRIX_3D_VectorMultiply (&InputPoint,
        &Seq3DInfo->sequenceToWorldTransformation.matrix3D,
        &OutputLocation);

      // Set the forwards vector.

      InputPoint.z = 1.0F;
      LE_MATRIX_3D_VectorMultiply (&InputPoint,
        &Seq3DInfo->sequenceToWorldTransformation.matrix3D,
        &OutputForwards);
      OutputForwards -= OutputLocation;
      LE_MATRIX_3D_NormalizeVector (&OutputForwards, &OutputForwards);

      // Set the up vector.

      InputPoint.y = 1.0F;
      InputPoint.z = 0.0F;
      LE_MATRIX_3D_VectorMultiply (&InputPoint,
        &Seq3DInfo->sequenceToWorldTransformation.matrix3D,
        &OutputUp);
      OutputUp -= OutputLocation;
      LE_MATRIX_3D_NormalizeVector (&OutputUp, &OutputUp);

      // Set the camera parameters.

      InternalSetCamera (
        &OutputLocation,
        &OutputForwards,
        &OutputUp,
        FieldOfView,
        NearClip,
        FarClip);

      return TRUE; // We changed it.
    }
  }

  return FALSE;
}


// ===========================================================================
// Global functions ==========================================================
// ===========================================================================


// Fills in an HRGN to describe the portion of the screen that is occupied by
// the given model, viewed through this render slot.
//
void REND3D_SetRegionToModelScreenExtent(LI_REND3D_SlotDataPointer SlotPntr,
                                         FrameType *rootFrame, HRGN destRegion)
{
  iRect pc3dRect;

  SlotPntr->m_pRend3DView->getExtent2D(rootFrame,
                                       SlotPntr->m_pCamera->getCamera(),
                                       &pc3dRect);
  SetRectRgn(destRegion,
             pc3dRect.x,                        // left
             pc3dRect.y,                        // top
             pc3dRect.x + pc3dRect.getXdim(),   // right
             pc3dRect.y + pc3dRect.getYdim());  // bottom
}


/****************************************************************************
 **  GAMECODE CALLS THESE FUNCTIONS                                        **
 ****************************************************************************/

TYPE_Point3D LE_REND3D_GetLocation(LE_REND_RenderSlot RenderSlot) {
  if (RenderSlot >= CE_ARTLIB_RendMaxRenderSlots)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LE_REND3D_GetLocation: Slot out of range.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return D3DVECTOR( 0, 0, 0 );
  }
  return SlotDataArray[RenderSlot].m_RendCamera.location ;
}

TYPE_Point3D LE_REND3D_GetForwardVector(LE_REND_RenderSlot RenderSlot) {
  if (RenderSlot >= CE_ARTLIB_RendMaxRenderSlots)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LE_REND3D_GetLocation: Slot out of range.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return D3DVECTOR( 0, 0, 0 );
  }
  return SlotDataArray[RenderSlot].m_RendCamera.forwardVector;
}

TYPE_Point3D LE_REND3D_GetUpwardVector(LE_REND_RenderSlot RenderSlot) {
  if (RenderSlot >= CE_ARTLIB_RendMaxRenderSlots)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LE_REND3D_GetLocation: Slot out of range.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return D3DVECTOR( 0, 0, 0 );
  }
  return SlotDataArray[RenderSlot].m_RendCamera.upVector;
}


Rend3DCamera *LE_REND3D_BeginCameraMove(LE_REND_RenderSlot RenderSlot) {
  if (RenderSlot >= CE_ARTLIB_RendMaxRenderSlots)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND3D_InstallInRenderSlot: Slot out of range.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL;
  }

  return &SlotDataArray[RenderSlot].m_RendCamera;
}

BOOL LE_REND3D_EndCameraMove(LE_REND_RenderSlot RenderSlot) {
  if (RenderSlot >= CE_ARTLIB_RendMaxRenderSlots)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND3D_EndCameraMove: Slot out of range.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  SlotDataArray[RenderSlot].m_bRendCameraDirty = TRUE;
  return TRUE;
}

/********************************************************************
 * Clears the D3DViewport to black before rendering if true.  You may
 * just want to have a 2D render slot behind the 3D one to accomplish
 * the same thing, and you can put a bitmap or animation in that 2D
 * slot too.  But this one is slightly faster, if you like black.
 ********************************************************************/
void LE_REND3D_ClearBeforeRender( LE_REND_RenderSlot RenderSlot, BOOL clear )
{
  if( RenderSlot >= CE_ARTLIB_RendMaxRenderSlots )
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND3D_ClearBeforeRender: Slot out of range.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
  }
  SlotDataArray[ RenderSlot ].m_bClearRegion = clear;
}



/*****************************************************************************
 *****************************************************************************
 ** I N I T I A L I S A T I O N   &   S H U T D O W N   F U N C T I O N S   **
 *****************************************************************************
 ****************************************************************************/

/*****************************************************************************
 * Install the 3D rendering module into a particular render slot.  Returns
 * TRUE if successful.
 */

BOOL LE_REND3D_InstallInRenderSlot(LE_REND_RenderSlot RenderSlot,
                                   TYPE_Rect CameraRenderArea,
                                   PDDSURFACE WorkingSurface,
                                   PDDSURFACE OutputSurface,
                                   LE_REND_RenderSlotSet OverlappingRenderSlotSet)
{
  DDrawDriver              *pDDDriver;
  BOOL                      ReturnCode = FALSE;
  LI_REND3D_SlotDataPointer SlotPntr;

  // Check function arguments:
  if (RenderSlot >= CE_ARTLIB_RendMaxRenderSlots)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND3D_InstallInRenderSlot: Slot out of range.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  if (LI_SEQNCR_RenderSlotArray [RenderSlot] != NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND3D_InstallInRenderSlot: Slot is already in use.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  if (WorkingSurface == NULL || OutputSurface == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND3D_InstallInRenderSlot: Bad parameters.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  // Need a 3D device, assume there is only one global one, but there
  // really should be an argument like the working surface one to
  // specify it.

  pDDDriver = pc3D::GetDDrawDriver();
  if (pDDDriver == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND3D_InstallInRenderSlot: Global 3D device doesn't exist!\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  //=================================================================================
  // Initialize this slot:
  //=================================================================================

  SlotPntr = SlotDataArray + RenderSlot;

  // Initialize variables needed to handle rendering
  SlotPntr->m_pRend3DScene    = new LI_REND3D_Scene;
  SlotPntr->cameraRenderArea  = CameraRenderArea;
  SlotPntr->m_bClearRegion    = FALSE;

  SlotPntr->workingSurface    = WorkingSurface;
  WorkingSurface->AddRef();

  SlotPntr->outputSurface     = OutputSurface;
  OutputSurface->AddRef ();

  SlotPntr->labelOfCameraToUse = 0;
  SlotPntr->overlappingRenderSlotSet = (OverlappingRenderSlotSet & ~(1 << RenderSlot));
  SlotPntr->invalidArea       = NULL;

  for( int t = 0; t < CE_ARTLIB_NumLights; t++)
  {
    SlotPntr->m_apLights[t] = NULL;
  }

  // Configure the viewing surface and matrices.
  SlotPntr->m_pRend3DView = new view(pc3D::GetRenderSurface(),
      &iRect(CameraRenderArea.left, CameraRenderArea.top,
            CameraRenderArea.right - CameraRenderArea.left,
            CameraRenderArea.bottom - CameraRenderArea.top));

  // Configure the pc3d camera
  SlotPntr->m_pCamera = new CCamera();
  SlotPntr->m_pCamera->setCamera(new FrameType);

  // Configure the rend3d camera
  SlotPntr->m_RendCamera.location         = D3DVECTOR(0, 0, -320.f);
  SlotPntr->m_RendCamera.forwardVector    = D3DVECTOR(0, 0, 1);
  SlotPntr->m_RendCamera.upVector         = D3DVECTOR(0, 1, 0);
  SlotPntr->m_RendCamera.rollAngle        = 0;
  SlotPntr->m_bInterpolatingCameraPos     = FALSE;
  SlotPntr->m_bRendCameraDirty            = TRUE;

  // Set up our internal viewport matrix.  Rarely changes.
  SlotPntr->SetViewport (&CameraRenderArea);

  // The camera's matrices (m_mtrxView and m_mtrxProjection) will be
  // recalculated at the next update, since the camera is marked as dirty.

  // Some more one time init stuff, since we always render from
  // the same view, just changing the camera settings.
  SlotPntr->m_pRend3DScene->SetRenderView(SlotPntr->m_pRend3DView);
  SlotPntr->m_pRend3DScene->setCCamera(SlotPntr->m_pCamera);

  // Enable the sequencer callbacks for our slot.

  ReturnCode = SlotPntr->InstallInSlot (RenderSlot);

  if (!ReturnCode)
    SlotPntr->Uninstall (); // Undo allocations / AddRefs.

  return ReturnCode;
}



/*****************************************************************************
 * Init the system.
 */

BOOL LI_REND3D_InitSystem (void)
{
  // l_grafix.c is responsible for initializing pc3d via ddinit.h
  assert(pc3D::IsInitialized());

  GlobalTempRegion = CreateRectRgn(0, 0, 0, 0);

  return TRUE;
}



/*****************************************************************************
 * Remove the system.
 */

BOOL LI_REND3D_RemoveSystem (void)
{
  LE_REND_RenderSlot  RenderSlot;

  for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
    SlotDataArray[RenderSlot].Uninstall();

  if (GlobalTempRegion != NULL)
  {
    DeleteObject (GlobalTempRegion);
    GlobalTempRegion = NULL;
  }

#if CE_ARTLIB_3DUsingOldFrame
  HMD_ReleaseRenderBuffers();
  HMD_ReleaseAll();
#endif // CE_ARTLIB_3DUsingOldFrame

  return TRUE;
}



/*****************************************************************************
 * Find the screen bounding box for the given 3D mesh using the given
 * model to camera coordinate system matrix and the camera to screen
 * coordinates projection matrix.  Returns TRUE if the bounding box
 * exists and is on screen.
 */

BOOL LI_REND3D_SlotDataClass::LI_REND3D_GetMeshScreenRect (
MeshType *myMesh, RECT *ScreenRectPntr,
TYPE_Matrix3DPointer ModelToCameraMatrix,
TYPE_Matrix3DPointer CameraToScreenMatrix)
{
  Box           BoundingBox;
  TYPE_Point3D  CornerPoints[8];
  int           Dimension;
  int           i, j;
  TYPE_Point3D  InputCameraVertices[16];
  TYPE_Point3D  IntersectionPoint;
  int           nVisiblePoints;
  TYPE_Point3D  OutputScreenVertices[16];
  float         Proportion;
  float         NearZ;
  TYPE_Coord2D  X, Y;

  if (m_pRend3DScene == NULL || myMesh == NULL)
    return FALSE;

  NearZ = m_pRend3DScene->m_fNearZ;

#if CE_ARTLIB_3DUsingOldFrame
  myMesh->getBox (BoundingBox);
#else // New frame style.
  myMesh->GetBoundingBox (BoundingBox);
#endif // CE_ARTLIB_3DUsingOldFrame

  // Get the points in hypercube order, Z is bit 0, Y is bit 1, X is bit 2.

  BoundingBox.getPts (CornerPoints);

  // Convert to camera coordinate space, with the camera at (0,0,0) looking
  // into the +Z direction.

  for (i = 0; i < 8; i++)
    LE_MATRIX_3D_VectorMultiply (CornerPoints + i, ModelToCameraMatrix, CornerPoints + i);

  // Add all corners which are visible to the list of visible points.

  nVisiblePoints = 0;
  for (i = 0; i < 8; i++)
  {
    if (CornerPoints[i].z >= NearZ)
      InputCameraVertices [nVisiblePoints++] = CornerPoints[i];
  }

  if (nVisiblePoints <= 0)
    return FALSE; // No visible points at all.

  if (nVisiblePoints < 8)
  {
    // Some corner points are invisible and some are visible,
    // add the intersection of the edge of the cube between the
    // visible corner and the invisible corner to our list of
    // visible points.

    for (i = 0; i < 8; i++)
    {
      for (Dimension = 0; Dimension < 3; Dimension++)
      {
        // Find the number of a corner joined to this corner
        // by an edge of the cube.  Just flip one of the dimension
        // bits to get the index.

        j = i ^ (1 << Dimension);

        // Always use the pair where i < j, otherwise we would look
        // at the same edge again when i > j.  In other words, process
        // the edge between 3 and 4, but not the reverse one between
        // 4 and 3.

        if (j < i)
          continue;

        // See if one corner is visible while the other isn't.

        if ((CornerPoints[i].z >= NearZ && CornerPoints[j].z < NearZ) ||
        (CornerPoints[i].z < NearZ && CornerPoints[j].z >= NearZ))
        {
          // Find the intersection point.  The proportion is 0.0 when
          // the intersection is at point i, and 1.0 when it is at
          // point j and inbetween if the intersection is inbetween.

          Proportion = CornerPoints[j].z - CornerPoints[i].z;
          Proportion = (NearZ - CornerPoints[i].z) / Proportion;

          IntersectionPoint.x = CornerPoints[i].x +
            (CornerPoints[j].x - CornerPoints[i].x) * Proportion;
          IntersectionPoint.y = CornerPoints[i].y +
            (CornerPoints[j].y - CornerPoints[i].y) * Proportion;
          IntersectionPoint.z = NearZ;

#if CE_ARTLIB_EnableDebugMode
          if (nVisiblePoints >= 16)
          {
            LE_ERROR_DebugMsg ("LI_REND3D_GetMeshScreenRect: Bug in algorithm.\r\n",
            LE_ERROR_DebugMsgToFileAndScreen);
            return FALSE;
          }
#endif
          InputCameraVertices [nVisiblePoints++] = IntersectionPoint;
        }
      }
    }
  }

  // Convert from camera to screen coordinates.

  for (i = nVisiblePoints - 1; i >= 0; i--)
  {
    LE_MATRIX_3D_VectorMultiply (InputCameraVertices + i,
      CameraToScreenMatrix, OutputScreenVertices + i);
  }

  // Find the bounding box around the screen points.

  X = TYPE_Coord2D (OutputScreenVertices[0].x);
  Y = TYPE_Coord2D (OutputScreenVertices[0].y);

  ScreenRectPntr->top     = Y;
  ScreenRectPntr->bottom  = Y;
  ScreenRectPntr->left    = X;
  ScreenRectPntr->right   = X;

  for (i = nVisiblePoints - 1; i > 0 /* Note zero already done */; i--)
  {
    X = TYPE_Coord2D (OutputScreenVertices[i].x);
    Y = TYPE_Coord2D (OutputScreenVertices[i].y);

    if (ScreenRectPntr->top > Y)
      ScreenRectPntr->top = Y;
    if (ScreenRectPntr->bottom < Y)
      ScreenRectPntr->bottom = Y;
    if (ScreenRectPntr->left > X)
      ScreenRectPntr->left = X;
    if (ScreenRectPntr->right < X)
      ScreenRectPntr->right = X;
  }

  ScreenRectPntr->top--;
  ScreenRectPntr->left--;
  ScreenRectPntr->bottom++;
  ScreenRectPntr->right++;

  return TRUE;
}



/*****************************************************************************
 * Internal use function for finding the 3D bounding box of a 3D mesh
 * sequence object.  Uses the first 3D render slot the object is in
 * in order to determine the mesh shape (can be an interpolated mesh).
 * Returns 8 points in object coordinates.  Returns FALSE on failure.
 */

BOOL LI_REND3D_GetBoundingBoxFor3DMesh (
LE_SEQNCR_RuntimeInfoPointer SeqPntr, TYPE_Point3DPointer PointArray)
{
  Box                       BoundingBoxData;
  Private3DData            *PrivateDataPntr;
  LE_REND_RenderSlot        RenderSlotNumber;
  LI_REND3D_SlotDataPointer SlotPntr;

  if (SeqPntr == NULL || PointArray == NULL ||
  SeqPntr->sequenceType != LE_CHUNK_ID_SEQ_3DMESH)
    return FALSE;

  // Find a 3D render slot being used by this sequence.

  for (RenderSlotNumber = 0; RenderSlotNumber < CE_ARTLIB_RendMaxRenderSlots;
  RenderSlotNumber++)
  {
    if ((SeqPntr->renderSlotSet & (1 << RenderSlotNumber)) &&
    SlotDataArray + RenderSlotNumber == LI_SEQNCR_RenderSlotArray [RenderSlotNumber])
      break; // Found an active 3D slot which this sequence is in.
  }
  if (RenderSlotNumber >= CE_ARTLIB_RendMaxRenderSlots)
    return FALSE; // Not in a 3D render slot.

  SlotPntr = SlotDataArray + RenderSlotNumber;

  // Get the 3D mesh and its 3D bounding box.

  PrivateDataPntr = (Private3DData *) SeqPntr->renderSlotData[RenderSlotNumber];
  if (PrivateDataPntr == NULL || PrivateDataPntr->reference == NULL)
    return FALSE;

#if CE_ARTLIB_3DUsingOldFrame
  PrivateDataPntr->reference->getBox (BoundingBoxData);
#else
  PrivateDataPntr->reference->GetBoundingBox (BoundingBoxData);
#endif // CE_ARTLIB_3DUsingOldFrame

  BoundingBoxData.getPts (PointArray);

  return TRUE;
}



/*****************************************************************************
 * Moves the camera right NOW.  You may wish to use the sequencer command
 * for changing camera settings if you want to synchronise it with adding
 * and deleting objects.  This also disables the animated camera sequencer
 * object (sets camera label to zero).
 */

BOOL LE_REND3D_MoveCamera (LE_REND_RenderSlot RenderSlot,
  TYPE_Point3DPointer position,
  TYPE_Point3DPointer forwards,
  TYPE_Point3DPointer up,
  TYPE_Angle3D fovAngle,
  TYPE_Coord3D nearZ,
  TYPE_Coord3D farZ )
{
  LI_REND3D_SlotDataPointer SlotPntr;

  if (RenderSlot >= CE_ARTLIB_RendMaxRenderSlots)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND3D_MoveCamera: Slot out of range.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  SlotPntr = (LI_REND3D_SlotDataPointer) LI_SEQNCR_RenderSlotArray [RenderSlot];
  if (SlotPntr != SlotDataArray + RenderSlot)
    return FALSE; // Not our render slot, some other renderer, or NULL.

  // OK, set the camera!

  return SlotPntr->SetCamera (0 /* CameraNumber */,
    position, forwards, up, fovAngle, nearZ, farZ);
}



BOOL LE_REND3D_MoveCameraSmooth( LE_REND_RenderSlot RenderSlot,
                                 TYPE_Point3DPointer position,
                                 TYPE_Point3DPointer at,
                                 TYPE_Point3DPointer up,
                                 TYPE_Angle3D fovAngle,
                                 int steps )
{
//  LI_REND3D_SlotDataPointer SlotPntr;

  if (RenderSlot >= CE_ARTLIB_RendMaxRenderSlots)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND3D_MoveCameraSmooth: Slot out of range.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }
  SlotDataArray[RenderSlot].m_bInterpolatingCameraPos = TRUE;
  SlotDataArray[RenderSlot].m_OriginalPos             = SlotDataArray[RenderSlot].m_RendCamera;
  SlotDataArray[RenderSlot].m_DestPos.location        = *position;
  SlotDataArray[RenderSlot].m_DestPos.forwardVector   = *at;
  SlotDataArray[RenderSlot].m_DestPos.upVector        = *up;
  SlotDataArray[RenderSlot].m_DestPos.rollAngle       = 0.0f;
  SlotDataArray[RenderSlot].m_nInterpolationSteps     = steps;
  SlotDataArray[RenderSlot].m_nNumOfStepsTaken        = 0;

  return TRUE;
}


BOOL LE_REND3D_SetAmbient(LE_REND_RenderSlot RenderSlot,
                          TYPE_Point3DPointer color)
{

  SlotDataArray[RenderSlot].m_dwAmbientLight = D3DRGB(color->x, color->y, color->z);
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;
  return TRUE;
}


BOOL LE_REND3D_SetAmbient(LE_REND_RenderSlot RenderSlot, DWORD dwColour)
{
  SlotDataArray[RenderSlot].m_dwAmbientLight = dwColour;
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;
  return TRUE;
}


BOOL LE_REND3D_MakeLight(LE_REND_RenderSlot RenderSlot, int lightNum)
{
  Light* myLight = NULL;
  CLRVALUE cVal( 1.0f, 1.0f, 1.0f, 0.0f );
  TYPE_Point3D aPt;

  aPt.x = 0.0f;
  aPt.y = 0.0f;
  aPt.z = 1.0f;

  if( lightNum > CE_ARTLIB_NumLights ) return FALSE;

  myLight = SlotDataArray[RenderSlot].m_apLights[ lightNum ];

  if (NULL != myLight) return FALSE;

  myLight = new Light;
  ASSERT(NULL != myLight); if (!myLight) return(FALSE);

  myLight->SetFlags( 0 ); //Turn light off.
  myLight->SetType( D3DLIGHT_DIRECTIONAL ); //Set a base light type. ( I don't want to
                        //fuck with the different sub-classes of
                        //light. You want a point light, you set
                        //it up.

  myLight->SetColor( cVal );
  myLight->SetDirection( aPt );
  myLight->Set();

  SlotDataArray[RenderSlot].m_pRend3DView->addLight( myLight );
  SlotDataArray[RenderSlot].m_apLights[ lightNum ] = myLight;
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;

  return TRUE;
}


BOOL LE_REND3D_DeleteLight(LE_REND_RenderSlot RenderSlot, int lightNum)
{
  Light *myLight;

  if( lightNum > CE_ARTLIB_NumLights ) return FALSE;

  myLight = SlotDataArray[RenderSlot].m_apLights[ lightNum ];

  if( myLight == NULL ) return FALSE;

  SlotDataArray[RenderSlot].m_pRend3DView->removeLight( myLight );

  SlotDataArray[RenderSlot].m_apLights[ lightNum ] = NULL;
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;

  DESTROYPOINTER(myLight);

  return TRUE;
}


BOOL LE_REND3D_SetLightType(LE_REND_RenderSlot RenderSlot, int lightNum,
                            D3DLIGHTTYPE eType)
{
  if (lightNum > CE_ARTLIB_NumLights) return(FALSE);
  if (!SlotDataArray[RenderSlot].m_apLights[lightNum]) return(FALSE);

  SlotDataArray[RenderSlot].m_apLights[lightNum]->SetType(eType);
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;

  return TRUE;
}


BOOL LE_REND3D_SetLightColor(LE_REND_RenderSlot RenderSlot, int lightNum,
                             const CLRVALUE& color)
{
  if (lightNum > CE_ARTLIB_NumLights) return(FALSE);
  if (!SlotDataArray[RenderSlot].m_apLights[lightNum]) return(FALSE);

  SlotDataArray[RenderSlot].m_apLights[lightNum]->SetColor(color);
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;

  return TRUE;
}

BOOL LE_REND3D_SetLightPosition(LE_REND_RenderSlot RenderSlot, int lightNum,
                                TYPE_Point3DPointer position)
{
  if (lightNum > CE_ARTLIB_NumLights) return(FALSE);
  if (!SlotDataArray[RenderSlot].m_apLights[lightNum]) return(FALSE);

  SlotDataArray[RenderSlot].m_apLights[lightNum]->SetPosition(*position);
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;

  return TRUE;
}

BOOL LE_REND3D_SetLightDirection(LE_REND_RenderSlot RenderSlot, int lightNum,
                                 TYPE_Point3DPointer direction)
{
  if (lightNum > CE_ARTLIB_NumLights) return(FALSE);
  if (!SlotDataArray[RenderSlot].m_apLights[lightNum]) return(FALSE);

  SlotDataArray[RenderSlot].m_apLights[lightNum]->SetDirection(*direction);
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;

  return TRUE;
}

BOOL LE_REND3D_SetLightAttenuation(LE_REND_RenderSlot RenderSlot, int lightNum,
                                   TYPE_Point3DPointer attenuation)
{
  if (lightNum > CE_ARTLIB_NumLights) return(FALSE);
  if (!SlotDataArray[RenderSlot].m_apLights[lightNum]) return(FALSE);

  SlotDataArray[RenderSlot].m_apLights[lightNum]->SetAttenuation(*attenuation);
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;

  return TRUE;
}

BOOL LE_REND3D_SetLightRange(LE_REND_RenderSlot RenderSlot, int lightNum,
                             TYPE_Coord3D range )
{
  if (lightNum > CE_ARTLIB_NumLights) return(FALSE);
  if (!SlotDataArray[RenderSlot].m_apLights[lightNum]) return(FALSE);

  SlotDataArray[RenderSlot].m_apLights[lightNum]->SetRange(range);
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;

  return TRUE;
}

BOOL LE_REND3D_SetLightFalloff(LE_REND_RenderSlot RenderSlot, int lightNum,
                               TYPE_Coord3D falloff)
{
  if (lightNum > CE_ARTLIB_NumLights) return(FALSE);
  if (!SlotDataArray[RenderSlot].m_apLights[lightNum]) return(FALSE);

  SlotDataArray[RenderSlot].m_apLights[lightNum]->SetFalloff(falloff);
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;

  return TRUE;
}

BOOL LE_REND3D_SetLightPhi(LE_REND_RenderSlot RenderSlot, int lightNum,
                           TYPE_Coord3D phi)
{
  if (lightNum > CE_ARTLIB_NumLights) return(FALSE);
  if (!SlotDataArray[RenderSlot].m_apLights[lightNum]) return(FALSE);

  SlotDataArray[RenderSlot].m_apLights[lightNum]->SetPhi(phi);
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;

  return TRUE;
}

BOOL LE_REND3D_SetLightTheta(LE_REND_RenderSlot RenderSlot, int lightNum,
                             TYPE_Coord3D theta)
{
  if (lightNum > CE_ARTLIB_NumLights) return(FALSE);
  if (!SlotDataArray[RenderSlot].m_apLights[lightNum]) return(FALSE);

  SlotDataArray[RenderSlot].m_apLights[lightNum]->SetTheta(theta);
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;

  return TRUE;
}

BOOL LE_REND3D_SetLightUmbra(LE_REND_RenderSlot RenderSlot, int lightNum,
                             TYPE_Coord3D umbra)
{
  if (lightNum > CE_ARTLIB_NumLights) return(FALSE);
  if (!SlotDataArray[RenderSlot].m_apLights[lightNum]) return(FALSE);

  SlotDataArray[RenderSlot].m_apLights[lightNum]->SetUmbra(umbra);
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;

  return TRUE;
}

BOOL LE_REND3D_SetLightPenumbra(LE_REND_RenderSlot RenderSlot, int lightNum,
                                TYPE_Coord3D umbra)
{
  if (lightNum > CE_ARTLIB_NumLights) return(FALSE);
  if (!SlotDataArray[RenderSlot].m_apLights[lightNum]) return(FALSE);

  SlotDataArray[RenderSlot].m_apLights[lightNum]->SetPenumbra(umbra);
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;

  return TRUE;
}

BOOL LE_REND3D_SetLightFlags(LE_REND_RenderSlot RenderSlot, int lightNum,
                             DWORD dwFlags)
{
  if (lightNum > CE_ARTLIB_NumLights) return(FALSE);
  if (!SlotDataArray[RenderSlot].m_apLights[lightNum]) return(FALSE);

  SlotDataArray[RenderSlot].m_apLights[lightNum]->SetFlags(dwFlags);
  SlotDataArray[RenderSlot].m_bWholeSceneDirty = TRUE;

  return TRUE;
}


BOOL LE_REND3D_SetViewportRect(LE_REND_RenderSlot RenderSlot, const RECT& rct)
{
  return SlotDataArray[RenderSlot].SetViewport(&rct);
}


void LE_REND3D_SetClippingPlanes(LE_REND_RenderSlot RenderSlot, float fNear,
                                 float fFar)
{
  SlotDataArray[RenderSlot].m_pRend3DScene->SetClippingPlanes(fNear, fFar);
}

void LE_REND3D_SetViewingAngle(LE_REND_RenderSlot RenderSlot, float fRads)
{
  SlotDataArray[RenderSlot].m_pRend3DScene->SetViewingAngle(fRads);
}


// ===========================================================================
// Function:    LE_REND3D_LoadBackgroundBitmap
//
// Description: Loads a bitmap from a file and sets it as the background for
//              the render slot
//
// Params:      RenderSlot:   The render slot to set the background of
//              szFile:       Filename of the bitmap to set as the background
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the background is successfully set, or FALSE
//              if not.
// ===========================================================================
BOOL LE_REND3D_LoadBackgroundBitmap(LE_REND_RenderSlot RenderSlot,
                                    LPCSTR szFile)
{
  // Create a new surface with the given bitmap
  Surface* pSurface = new Surface(szFile);
  ASSERT(NULL != pSurface);

  // Check to make sure we successfully created the surface
  if (!pSurface->IsInitialized())
  {
    DESTROYSURFACE(pSurface);
    return(FALSE);
  }

  // Now set the new surface as our background
  return(LE_REND3D_SetBackgroundSurface(RenderSlot, pSurface));
}


// ===========================================================================
// Function:    LE_REND3D_SetBackgroundSurface
//
// Description: Takes the given surface and uses it to set the background
//              surface of the render slot's viewport.
//
// Params:      RenderSlot:   The render slot to set the background of
//              pBackground:  Pointer to the surface to set as the background
//              bForce:       Whether we want to force the background or not
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the background is successfully set, or FALSE
//              if it is not.
// ===========================================================================
BOOL LE_REND3D_SetBackgroundSurface(LE_REND_RenderSlot RenderSlot,
                                    Surface* pBackground,
                                    BOOL bForce /*= FALSE*/)
{
  return(SlotDataArray[RenderSlot].m_pRend3DView->SetBackgroundSurface(pBackground, bForce));
}

#endif // CE_ARTLIB_EnableSystemGrafix3D
