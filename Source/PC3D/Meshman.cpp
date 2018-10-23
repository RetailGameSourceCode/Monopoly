#include "PC3DHdr.h"

#include "pc3d.h"
#include "meshman.h"
#include "vectlist.h"



meshMan::meshMan(frame *root, BOOL addChildFrames)
: sortedMeshRecs(16,32)
{
init();
addMeshes(root, addChildFrames);
}

meshMan::meshMan(mesh *meshList, int num, frame *meshParent)
: sortedMeshRecs(16,32)
{
init();
for (int i = 0; i < num; i++)
    addMesh( &meshList[i], meshParent);
}

void meshMan::init()
{
  frameRecs = 0;
  distHint2 = 0.0f;

  setView(0);
  setFrustum(VIEWING_ANGLE, NEAR_Z, FAR_Z);

    // .......... set up a record for the NULL frame, i.e. world coordinates
  frameRecs = new frameManRec(); // all fields zero'ed by default
  frameRecs->viewXform.Identity();
}

meshMan::~meshMan()
{

  // .......... step through the linked lists and destroy everyhing
  frameManRec *nextFrameRec = frameRecs, *frameToDelete;
  while (nextFrameRec)
  {

    meshManRec *nextMeshRec = nextFrameRec->meshList, *toDelete;
    while (nextMeshRec)
    {
      toDelete = nextMeshRec;
      nextMeshRec = nextMeshRec->nextRec; 
      delete toDelete;
    }

    frameToDelete = nextFrameRec;
    nextFrameRec = nextFrameRec->nextRec; 
    delete frameToDelete;
  }
}


BOOL meshMan::addMesh(mesh *toAdd, frame *toAddParent)
{
  frameManRec *nextFrameRec = frameRecs, *prevFrameRec = 0;

  while (nextFrameRec && (nextFrameRec->coordSys != toAddParent))
  {
    prevFrameRec = nextFrameRec;
    nextFrameRec = nextFrameRec->nextRec;
  }

  // .......... frame not found, add a record for it
  if (!nextFrameRec)
  {
    nextFrameRec = new frameManRec();
    nextFrameRec->coordSys = toAddParent;

    prevFrameRec->nextRec = nextFrameRec; // link it up
  }

  // .......... add the mesh to the list of meshes associated with the frame
  meshManRec *nextMeshRec = nextFrameRec->meshList, *prevMeshRec = 0;

  while (nextMeshRec && (nextMeshRec->m != toAdd))
  {
    prevMeshRec = nextMeshRec;
    nextMeshRec = nextMeshRec->nextRec;
  }

  // .......... error: mesh already exists for this parent
  if( nextMeshRec ) 
    return FALSE;

  // .......... create a new record for the mesh & link it up
  nextMeshRec = new meshManRec();
  if ( prevMeshRec )
    prevMeshRec->nextRec = nextMeshRec;
  else
    nextFrameRec->meshList = nextMeshRec;

  nextMeshRec->m = toAdd;
  nextMeshRec->parentRec = nextFrameRec;

  return TRUE;
}

BOOL meshMan::addMeshes(frame *toAddParent, BOOL addChildFrames)
{
  ASSERT(toAddParent); // must be a valid frame

  int numChildren = toAddParent->getNumChildren();

  // .......... recursive call to add child frames' meshes
  while (addChildFrames && numChildren--)
    addMeshes( toAddParent->getChild( numChildren ), TRUE );
  
  int count = toAddParent->getNumMeshes();

  // .......... add each mesh from the current frame
  BOOL addedOK = TRUE;
  while (count--)
    if (!addMesh( toAddParent->getMesh( count ), toAddParent ))
      addedOK = FALSE;

  return addedOK;
}


BOOL meshMan::removeMesh(mesh *toRemove, frame *toRemoveParent)
{
  frameManRec *nextFrameRec = frameRecs, *prevFrameRec = 0;

  while (nextFrameRec && (nextFrameRec->coordSys != toRemoveParent))
  {
    prevFrameRec = nextFrameRec;
    nextFrameRec = nextFrameRec->nextRec;
  }

  if (!nextFrameRec)
  // .......... frame not found, can't remove anything
    return FALSE;

  // .......... find the mesh to be deleted from the list of meshes associated with the frame
  meshManRec *nextMeshRec = nextFrameRec->meshList, *prevMeshRec = 0;

  while (nextMeshRec && (nextMeshRec->m != toRemove))
  {
    prevMeshRec = nextMeshRec;
    nextMeshRec = nextMeshRec->nextRec;
  }

  // error: can't find mesh
  if ( !nextMeshRec )
    return FALSE;

  // .......... delete the record for the mesh and connect the previous to the next
  if ( prevMeshRec )
    prevMeshRec->nextRec = nextMeshRec->nextRec;
  else
    nextFrameRec->meshList = nextMeshRec->nextRec;

  delete nextMeshRec;

  // .......... get rid of frame if no more meshes are associated with it,
  // .......... except if it is the NULL (world coordinate) frame
  if ( (!nextFrameRec->meshList) && prevFrameRec )
  {
    prevFrameRec->nextRec = nextFrameRec->nextRec;
    delete nextFrameRec;
  }

  return TRUE;
}


BOOL meshMan::removeFrame(frame *toRemove)
{
  frameManRec *nextFrameRec = frameRecs, *prevFrameRec = 0;

  while (nextFrameRec && (nextFrameRec->coordSys != toRemove))
  {
    prevFrameRec = nextFrameRec;
    nextFrameRec = nextFrameRec->nextRec;
  }

  if (!nextFrameRec)
  // .......... frame not found, can't remove anything
    return FALSE;

  // .......... dump all the meshes for this frame
  meshManRec *nextMeshRec = nextFrameRec->meshList, *toDelete;
  while (nextMeshRec)
  {
    toDelete = nextMeshRec;
    nextMeshRec = nextMeshRec->nextRec; 
    delete toDelete;
  }

  // .......... get rid of the frame link
  if ( prevFrameRec )
  {
    prevFrameRec->nextRec = nextFrameRec->nextRec;
    delete nextFrameRec;
  }
  else
  // .......... don't delete the null frame, just set the head mesh ptr to zero
    frameRecs->meshList = 0;

  return TRUE;
}

void meshMan::setView( Matrix *viewXform )
{
  if (viewXform)
    viewingXform = *viewXform;
  else
    viewingXform.Identity();// world coordinates
}
  

void meshMan::setFrustum(angle viewAngle, float nearPlane, float farPlane)
{
  ASSERT(viewAngle < PIFLOAT); // view angle must be less than 180 degrees
  ASSERT(nearPlane >= 0);
  ASSERT(farPlane > nearPlane);

  viewingAngle = viewAngle;
  nearClipPlane = nearPlane;
  farClipPlane = farPlane;
  viewingHalfAngleTan = (float) tan( viewingAngle / 2 );
}

void meshMan::setDistanceRejectHint(coord dist)
{
  distHint2 = dist * dist;
  ASSERT( dist >= 0.0); // -ve values illegal
}

const float ROOT2 = 1.41421356f;

BOOL meshMan::isVisible( visibleSphereInfo *vsi )
{
  coord radroot2 = vsi->radius * ROOT2;

  // .......... test X-Z plane, left edge of frustum
  coord frustumEdge = -vsi->originView.z * viewingHalfAngleTan;
  coord sphereExtent = vsi->originView.x + radroot2;
  if (sphereExtent < frustumEdge)
    return FALSE;

  // .......... test X-Z plane, right edge of frustum
  frustumEdge = -frustumEdge;
  sphereExtent = vsi->originView.x - radroot2;
  if (sphereExtent > frustumEdge)
    return FALSE;

  // .......... test Y-Z plane, top edge of frustum
  sphereExtent = vsi->originView.y - radroot2;
  if (sphereExtent > frustumEdge)
    return FALSE;

  // .......... test Y-Z plane, bottom edge of frustum
  frustumEdge = -frustumEdge;
  sphereExtent = vsi->originView.y + radroot2;
  if (sphereExtent < frustumEdge)
    return FALSE;

  // .......... OK, the circle may intersect the frustum
  return TRUE;
}

BOOL meshMan::doVisibilityCalc( meshManRec *mr )
{
  // .......... use the distance hint to see if the mesh can be rejected quickly
  if ( distHint2 )
  {
    coord dx = mr->parentRec->viewXform(3, 0);
    coord dy = mr->parentRec->viewXform(3, 1);
    coord dz = mr->parentRec->viewXform(3, 2);

    coord distFrame2 = dx * dx + dy * dy + dz * dz;
    if ( distFrame2 > distHint2 )
      return FALSE;
  }

  Box bounds;
  mr->m->getBox( &bounds );

  // .......... if bounding boxes are different, recompute the sphere info
  if ( memcmp(&bounds, &mr->oldBounds, sizeof (Box)) )
  {
    // init the bounding sphere
    mr->vs.origin.x = (bounds.maxx - bounds.minx) / 2;
    mr->vs.origin.y = (bounds.maxy - bounds.miny) / 2;
    mr->vs.origin.z = (bounds.maxz - bounds.minz) / 2;

    coord bx = bounds.maxx - mr->vs.origin.x;
    coord by = bounds.maxy - mr->vs.origin.y;
    coord bz = bounds.maxz - mr->vs.origin.z;

    mr->vs.radius = (float) sqrt( bx * bx + by * by * bz * bz );

    mr->oldBounds = bounds; // update bounds for next time visibility is tested
  }

  // .......... xform the sphere's origin to view system
  mr->vs.frame2view = mr->parentRec->viewXform;
  mr->vs.originView = mr->vs.frame2view.TransVector(mr->vs.origin);

  // .......... test for visibility based on origin and radius
  if ( !isVisible( &mr->vs ) )
    return FALSE;

  // .......... this is the distance key to sort the visible meshes by
  mr->vs.originDistSquared = 
    mr->vs.originView.x * mr->vs.originView.x +
    mr->vs.originView.y * mr->vs.originView.y +
    mr->vs.originView.z * mr->vs.originView.z;

  return TRUE;
}


// .......... comparison func for qsort
int __cdecl distanceCompare( const void *a, const void *b )
{
  meshManRec *ma = (meshManRec *) a, *mb = (meshManRec *) b;
  if ( ma->vs.originDistSquared < mb->vs.originDistSquared )
    return -1;
  else if ( ma->vs.originDistSquared == mb->vs.originDistSquared )
    return 0;
  else
    return 1;
}


BOOL meshMan::enumMeshes_withVisibleSpheres( meshEnumCallback *callback, BOOL farthestFirst, void *data)
{
  sortedMeshRecs.detachAll(); // remove all from list (very fast).

  // .......... step through the linked lists and test each mesh wrt the view
  frameManRec *nextFrameRec = frameRecs;

  while (nextFrameRec)
  {
    meshManRec *nextMeshRec = nextFrameRec->meshList;

    // calc the frame to view coordinate transform
    if ( nextFrameRec->coordSys )
    {
      // NOTE: this matrix mult won't take billboards into account.
      // to support it in the future, the viewpoint frame must be used
      // in a 'calcBillboardMatrix' call.
      nextFrameRec->viewXform =
        (*(nextFrameRec->coordSys->getWorldMatrix())) * viewingXform;
    }
    else
      // frame is in world coordinates, no multiply necessary
      nextFrameRec->viewXform = viewingXform;

    while ( nextMeshRec )
    {
      // if the mesh is visible, add it to the list
      if ( doVisibilityCalc( nextMeshRec ) )
          sortedMeshRecs.add( nextMeshRec );

      nextMeshRec = nextMeshRec->nextRec;
    }

    nextFrameRec = nextFrameRec->nextRec; 
  }


  // .......... now output the meshes ordered by distance 
  // .......... from the view coordinate system origin
  int numMeshes = sortedMeshRecs.count();

  // .......... sort the elements
  qsort( &sortedMeshRecs[0], numMeshes, sizeof (meshManRec *), &distanceCompare );

  // .......... order is specified as farthest to nearest or the reverse
  int ordering = (farthestFirst)? -1 : 1;
  int i = (farthestFirst)? numMeshes-1 : 0;

  BOOL cont = TRUE;

  // .......... call the user's function for every mesh withing the view frustum
  for (; numMeshes-- > 0; i += ordering)
  {
    meshManRec& mmr = *(sortedMeshRecs[i]);

    // init with the user data before the callback
    mmr.vs.data = data; 

    // and call the user's function
    cont = (*callback)(mmr.m, mmr.parentRec->coordSys, &mmr.vs);
    if ( !cont )
      break;
  }

  return cont;
}
