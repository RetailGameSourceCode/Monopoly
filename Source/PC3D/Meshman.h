#ifndef MESHMAN_H
#define MESHMAN_H

#include <memory.h>
#include "pc3d.h"


struct visibleSphereInfo {
  vector origin;  // origin of bounding sphere in local coordniates
  coord radius;   // radius of sphere
  vector originView;  // origin of bounding sphere in view coordniates
  coord originDistSquared;  // distance of origin from view, squared
  Matrix frame2view; // how to convert mesh coordinates to view coordinates
  void *data;   // user-specific data passed into the enumeration
};

struct frameManRec;
 
struct meshManRec {

  mesh *m;
  visibleSphereInfo vs;
  Box oldBounds;  // previously calculated box, for optimization if box doesn't change
  frameManRec *parentRec;
  meshManRec *nextRec;

  meshManRec() {  memset( this, 0, sizeof( meshManRec ) ); }
};


struct frameManRec
{
  Matrix        viewXform;
  frame*        coordSys;
  meshManRec*   meshList;
  frameManRec*  nextRec;

  frameManRec() { memset( this, 0, sizeof( frameManRec ) ); }
};



// .......... Manages a set of meshes with respect to a given viewpoint and frustum...
// .......... Note that this does not necessarily have to be the camera viewpoint; it can
// .......... be any frame of reference. For example, this might be useful for doing 
// .......... specific lighting effects on a given object, where the light frame is the viewpoint
// .......... and a set of objects are lit based on whether they intersect the light's "frustum".
// ..........   It is meant as a computational tool for trivial rejection and whatever
// .......... operations you can think of that depend on spatial relationships to a given 
// .......... coordinate system.
class meshMan
{

  // linked list of mesh<->frame relationships and associated data for management
  frameManRec *frameRecs;

  // sorted list of meshes with respect to the view
  CvectList<meshManRec *> sortedMeshRecs;
  float distHint2;

  angle viewingAngle;
  float nearClipPlane, farClipPlane;
  Matrix viewingXform;

  // frustum slope relative to z-axis
  float viewingHalfAngleTan;

  void init();
  BOOL isVisible( visibleSphereInfo *vsi );
  BOOL doVisibilityCalc( meshManRec *mr );

public:

// .......... construct the manager, adding all meshes from the given parent and
// .......... optionally including meshes of child frames. For a discussion of adding
// .......... meshes, see 'addMesh' below
        meshMan(frame *root, BOOL addChildFrames = FALSE);

// .......... construct the manager from a list of meshes, associationg them with
// .......... the given parent. A NULL parent indicates the meshes are in world coordinates
        meshMan(mesh *meshList = 0, int num = 0, frame *meshParent = 0);
        ~meshMan();

// .......... Adds the given mesh from the managed set of meshes.
// .......... If 'toAddParent' is NULL, the mesh is assumed to be in world
// .......... coordinates. The parent frame is needed to distinguish between
// .......... frames that may share the same mesh, and to indicate what coordinate
// .......... system the mesh is in so it can be transformed to view coordinates.
// .......... The return value is FALSE if the mesh cannot be added, which would 
// .......... happen if it is associated with the same parent more than once. 
// .......... 
// .......... Do not confuse this relationship between frames & meshes with
// .......... the similar relationship formed when meshes are added to frames directly.
// .......... Frequently, the parent frame specified here is the same as the parent
// .......... that actually contains the mesh, but it is not necessarily so. This seemingly
// .......... duplicated relationship is provided for two reasons:
// .......... 
// ..........    (1) You can use the mesh manager to relate arbitrary meshes
// .......... created for whatever reason...they do not have to be inside the 
// .......... visible scene graph; they may be used for some logical computation.
// .......... For example, a simpler mesh substituted for a more complex visual one 
// .......... could be used for collision detection in certain cases. Or you could 
// .......... rearrange / recreate the mesh <-> frame association to suit some 
// .......... unimagined function. 
// .......... 
// ..........   (2) Meshes can have more than one parent frame, allowing them
// .......... to be shared among parents. To allow the flexibility of adding only certain
// .......... meshes (i.e. not ALL meshes from a given frame) you need to explicitly 
// .......... specify which parent you're talking about.
// .......... 
// .......... NOTE: Removing a mesh from a frame has no effect in the mesh manager. 
// .......... If you manipulate the the scene graph via the frame interface, remember 
// .......... to reflect those relationships in the mesh manager if it is appropriate. 
// .......... The manager is meant as a computational tool, and does not keep track of
// .......... or care about the visual scene graph. The manager does not own either the
// .......... meshes or the associated frames.
  BOOL addMesh(mesh *toAdd, frame *toAddParent=0);

// .......... adds all meshes from the given parent, optionally including meshes of child frames
// .......... returns FALSE if any of the meshes couldn't be added.
  BOOL addMeshes(frame *toAddParent, BOOL addChildFrames = FALSE);

// .......... Removes the given mesh from the managed set of meshes.
// .......... The mesh will be removed only if it belongs to the given 
// .......... parent ('toRemoveParent'). This does not extend to the
// .......... parent's child frames, i.e. the mesh is searched for only
// .......... in the given frame. This mechanism allows a mesh to be used
// .......... in more than one frame.
  BOOL removeMesh(mesh *toRemove, frame *toRemoveParent = 0);

// .......... removes all meshes belonging to the given frame (including child frames)
// .......... return FALSE if frame was not found
  BOOL removeFrame(frame *toRemove);

// .......... setup the viewing coordinate system parameters...the manager copies the
// .......... matrix, i.e. does not use it directly. If the view matrix changes, you must 
// .......... 'setView' again here. A view transform matrix (NULL implies world coordinate system)
  void setView( Matrix *viewXform = 0 );  

// .......... setup the viewing frustum parameters...
  void setFrustum( angle viewAngle = VIEWING_ANGLE,// frustum viewing angle (0 indicates 360 degrees)
        float nearPlane = NEAR_Z,   // frustum near clip plane (minimum: 0.0)
        float farPlane = FAR_Z );     // frustum far clip plane (must be greater than near clip) 

// .......... if this distance "hint" is set, any mesh whose coordinate system 
// .......... origin is farther than 'dist' in the view coordinate system will
// .......... be rejected out of hand. A quick transformation involving only the
// .......... translation component will eliminate them from the traversal. 
// .......... Note that this does not take into account any rotation or scaling
// .......... of the meshes by the coordinate systems, so the user must decide
// .......... if it is appropriate to make such rejections based on position alone
// .......... (relying on knowledge of the set of objects and their intended function)
// .......... A value of 0.0 tells the manager to ignore the distance hint.
  void setDistanceRejectHint(coord dist = 0.0);

// .......... enumerates all meshes that are partially or fully visible from the current view.
// .......... Uses the mesh's bounding box to calculate a bounding sphere, transforms
// .......... the sphere center and radius to view coordinates for the visibility
// .......... test. The meshes are enumerated in the order given by the distance 
// .......... from the view coordinate system's origin. It can be specified as either
// .......... farthest first or the other way around (nearest first). 
// ..........   The void * passed to the the 'meshEnumCallback' function is a 'visibleSphereInfo *',
// .......... containing info about the sphere's location in the view 
// .......... coordinate system. It has a "void *data" member which will contain whatever
// .......... data is passed to the 'enumMeshes' call by the user. if the user callback
// .......... returns FALSE, enumeration will stop and the return value wil be FALSE as well.
  BOOL enumMeshes_withVisibleSpheres( meshEnumCallback *callback, BOOL farthestFirst, void *data);

};



#endif
