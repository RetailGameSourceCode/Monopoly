// //////////////////////////////////////////////////
// Copyright (C) 1997, Artech,        
// All rights reserved.          
//                 
// This document contains intellectual property  
// of Artech.  Any copying or reproduction    
// without prior written permission is prohibited. 
//                 
// //////////////////////////////////////////////////
// MODULE: frame.cpp              
// Contents: handles the Position/Rotation/Scale matrix hierarchy             
//                  
// ///////////////// END OF HEADER //////////////////
#include "PC3DHdr.h"
#include "l_frame.h"
#include "l_mesh.h"


frame::frame(frame *initParent /*= NULL*/, mesh* n /*= NULL*/)
      : parent(initParent), dirty(TRUE), worldDirty(TRUE), isIdentity(TRUE),
        children(0,4), contents(0), key(0), override(FALSE)
{
  if (n)
  {
    contents = new meshRec();
    contents->n = n;
    contents->next = NULL;
  }

  billboard = FALSE;

  local.Identity();
  opx = opy = opz = 0.0f;      
  ofx = ofy = ofz = 0.0f;      
  odx = ody = oux = ouz = 0.0f;
  odz = ouy = 1.0f;
  ax  = ay  = 0.0f;
  az  = 1.0f;
  ang = 0.0f;

  osx = osy = osz = 1.0f;
  scx = scy = 0.0f;
  scz = 1.0f;
  scang = 0.0f;
  SetupMatrix();

  if (parent)
    parent->AddChild( this );
}


// ===========================================================================
// Function:    Initialize
//
// Description: Initializes the frame
//
// Params:      initParent: Pointer to parent frame
//              pMesh:      Pointer to mesh
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void frame::Initialize(frame *initParent /*= NULL*/, mesh* pMesh /*= NULL*/)
{
  Destroy();

  parent      = initParent;
  dirty       = TRUE;
  worldDirty  = TRUE;
  isIdentity  = TRUE;
  contents    = NULL;
  key         = 0;
  override    = FALSE;

  children.resize(0);
  children.setDelta(4);

  if (pMesh)
  {
    contents        = new meshRec();
    contents->n     = pMesh;
    contents->next  = NULL;
  }

  billboard = FALSE;

  local.Identity();
  opx = opy = opz = 0.0f;      
  ofx = ofy = ofz = 0.0f;      
  odx = ody = oux = ouz = 0.0f;
  odz = ouy = 1.0f;
  ax  = ay  = 0.0f;
  az  = 1.0f;
  ang = 0.0f;

  osx = osy = osz = 1.0f;
  scx = scy = 0.0f;
  scz = 1.0f;
  scang = 0.0f;
  SetupMatrix();

  // Add this frame to the parent
  if (parent)
    parent->AddChild(this);
}


// ===========================================================================
// Function:    Destroy
//
// Description: Destroys the contents of the frame
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void frame::Destroy()
{
  // Remove this frame from its parent
  if (parent)
    parent->DetachChild(this);

  // Detach the child frames
  while (children.count())
  {
    frame *child = children [ children.count()-1 ];
    children.detach( children.count()-1, 0);
    delete child;
  }

  // Remove our contents
  DESTROYPOINTER(contents);
}


mesh* frame::GetMesh(int index)
{
  meshRec *nr = contents;
  int i = -1;
  while (++i < index && nr->next)
    nr = nr->next;

  ASSERT(nr); // bad index
  return nr->n;
}


int frame::GetNumMeshes()
{
  meshRec *nr = contents;
  int i = 0;
  while (nr)
  {
    i++;
    nr = nr->next;
  }
  return i;
}

void frame::AddMesh(mesh *n)
{
  ASSERT(n);

  meshRec *nr = new meshRec();
  nr->n     = n;
  nr->next  = NULL;
  if (contents)
    nr->next = contents;
  contents = nr;
}

void frame::DetachMesh(mesh *n)
{
  meshRec *nr = contents, *lr = NULL;
  while (nr)
  {
    if (nr->n == n)
    {
      if (nr == contents)
        contents = nr->next;
      else
        lr->next = nr->next;

      delete nr;
      return;
    }
    lr = nr;
    nr = nr->next;
  }

  ASSERT(0); // must find the mesh
}


void frame::SetDirty()
{
  worldDirty = dirty = TRUE;

  int i=0,n=children.count();
  for (i = 0; i < n; i++)
    children[i]->SetWorldDirty();
}


void frame::SetWorldDirty()
{
  worldDirty = TRUE;

  int n = children.count();
  for (int i = 0; i < n; i++)
    children[i]->SetWorldDirty();
}


void frame::GetOrientation(vector* dir, vector* up) 
{
  ASSERT(NULL != dir);
  ASSERT(NULL != up);
  dir->x  = odx;  dir->y  = ody;  dir->z  = odz;
  up->x   = oux;  up->y   = ouy;  up->z   = ouz;
}


void  frame::GetUp(vector* up) 
{
  ASSERT(NULL != up);
  up->x = oux;    up->y = ouy;    up->z = ouz;
}

void  frame::GetDirection(vector* dir) 
{
  ASSERT(NULL != dir);
  dir->x = odx;   dir->y = ody;   dir->z = odz;
}

void  frame::GetOffset(vector* off) 
{
  ASSERT(NULL != off);
  off->x = ofx;   off->y = ofy;   off->z = ofz;
}

void  frame::GetPosition(vector* pos) 
{
  ASSERT(NULL != pos);
  pos->x = opx;   pos->y = opy;   pos->z = opz;
}


void frame::GetScale(vector* scale)
{
  ASSERT(NULL != scale);
  scale->x = osx;   scale->y = osy;   scale->z = osz;
}


void frame::SetScale(coord sx, coord sy, coord sz)
{
  osx = sx; osy = sy; osz = sz;
  SetDirty();
}


void frame::SetPosition(coord x, coord y, coord z)
{
  opx = x; opy = y; opz = z;
  SetDirty();
}

void frame::SetOffset(coord x, coord y, coord z)
{
  ofx = x; ofy = y; ofz = z;
  SetDirty();
}

void frame::SetOrientation(coord dx, coord dy, coord dz, coord ux, coord uy,
                           coord uz)
{
  odx = dx; ody = dy; odz =dz;
  oux = ux; ouy = uy; ouz = uz;
  SetDirty();
}

void frame::SetOrientation(vector* axis, coord angle)
{
  ax = axis->x;
  ay = axis->y;
  az = axis->z;
  ang = angle;

  // we need to get the corresponding up and direction vectors
  Matrix rotaxis = Matrix::RotateAxisMatrix(*axis, angle);
  vector up(0, 1, 0);
  vector dir(0, 0, 1);
  rotaxis.TransformVector(up);
  rotaxis.TransformVector(dir);

  SetOrientation(&dir, &up);
}


void frame::SetScaleOrientation(coord sx, coord sy, coord sz, coord ang)
{
  scx = sx; scy = sy; scz = sz; scang = ang;
  SetDirty();
}


void frame::GetScaleOrientation(vector* axis, coord* ang)
{
  ASSERT(NULL != axis);
  ASSERT(NULL != ang);
  axis->x = scx;   axis->y = scy;   axis->z = scz;
  *ang    = scang;
}


// init the matrix which is equivalent to rotating a frame
// to its orientation and translating it to its position in
// world coordinates.
void frame::SetupMatrix()
{
  vector D(odx, ody, odz);
  vector N(oux, ouy, ouz);
  vector DSCALE(scx, scy, scz);
  Matrix mtrxTmp(Matrix::IDENTITY);

  // the matrix is not overriden , so calculate it
  if (!override)
  {
    // 1) translate offset to center
    mtrxTmp.Translate(-ofx, -ofy, -ofz);

    if (1.0f != osx || 1.0f != osy || 1.0f != osz)
    {
      Matrix SR;

      // 2) rotate scale axis to root / parent axis
      if (0.0f != scang)
      {
        SR = Matrix::RotateAxisMatrix(DSCALE, scang);
        mtrxTmp *= SR.Inverse();
      }

      // 3) apply the non uniform scale in root coordinates
      mtrxTmp.Scale(DSCALE);

      // 4) rotate back to scale axis
      if (0.0f != scang)
        mtrxTmp *= SR;  
    }

    // 5) rotate frame to its orientation
    Matrix R;
    SetRotationMatrix(&R, &D, &N);
    mtrxTmp *= R; 

    // 6) translate center back to offset
    mtrxTmp.Translate(ofx, ofy, ofz);

    // 7) translate frame to its position relative to root
    mtrxTmp.Translate(opx, opy, opz);
    local = mtrxTmp;
  } // end if not overriden

  dirty = FALSE;
  isIdentity = local.IsIdentity();
}


// inits the matrix representing the entire world transform
void frame::SetupWorldMatrix()
{
  if (dirty)
    SetupMatrix();

  if (parent)
  {
    // optimization here when all parents up to the root are identity matrices
    frame *testparent = parent;
    while ( testparent != NULL)
    {
      if (!testparent->MatrixIsIdentity())
      {
        world = local * (*(parent->GetWorldMatrix()));
        worldDirty = FALSE;
        return;
      }
      testparent = testparent->parent;
    }
    // all parents are identity, so just copy local to world
    world = local; 
  }
  else
    // no parent, we're at the top of the hierarchy
    world = local;

  worldDirty = FALSE;
}


BOOL frame::EnumMeshes(meshEnumCallback *callback, void *data)
{
  int i = -1, nChild = GetNumChildren();
  BOOL continueEnum = TRUE;

  // do the children first
  while (++i < nChild && continueEnum)
    continueEnum = GetChild(i)->EnumMeshes( callback, data);

  if (!continueEnum)
    return FALSE;

  int m = -1, nMeshes = GetNumMeshes();
  while (++m < nMeshes && continueEnum)
    continueEnum = (*callback)(GetMesh(m), this, data);

  return continueEnum;
}

// used in 'updateExtent'
struct extentQuery 
{
  Box     bounds;
  frame*  target;
  Matrix  targetInv;
};


BOOL updateExtent(mesh* m, frame* local, void* data)
{
  // update the extent with the meshes of this frame
  Box bounds;
  extentQuery& exQ = *((extentQuery *) data);
  Matrix xform(*(local->GetWorldMatrix()));

  // if target coordinate system is not world, multiply by target's inverse matrix
  if ( exQ.target )
    xform *= exQ.targetInv;

  m->getBox(bounds);
  vector b[8];
  bounds.getPts(b);

  // transform the points to target coordinate system
  xform.TransformVectors(b, 8);

  // ......... update the bounding box
  for (int i = 0; i < 8; i++)
  {
    if (b[i].x < exQ.bounds.minx) exQ.bounds.minx = b[i].x;
    if (b[i].y < exQ.bounds.miny) exQ.bounds.miny = b[i].y;
    if (b[i].z < exQ.bounds.minz) exQ.bounds.minz = b[i].z;

    if (b[i].x > exQ.bounds.maxx) exQ.bounds.maxx = b[i].x;
    if (b[i].y > exQ.bounds.maxy) exQ.bounds.maxy = b[i].y;
    if (b[i].z > exQ.bounds.maxz) exQ.bounds.maxz = b[i].z;
  }

  return TRUE;
}


BOOL frame::GetExtent3D(frame* inFrame, Box* extent3D)
{
  extentQuery exQ;
  exQ.bounds.minx = exQ.bounds.miny = exQ.bounds.minz = FLT_MAX;
  exQ.bounds.maxx = exQ.bounds.maxy = exQ.bounds.maxz = -FLT_MAX;
  exQ.target = inFrame; // indicate transform to target frame's coordinates, or NULL for world

  if (inFrame)
  {
    exQ.targetInv = *(inFrame->GetWorldMatrix());
    exQ.targetInv.Invert();
  }

  // visit all the meshes in the object tree, transform their bounding boxes to camera coordinates
  EnumMeshes( updateExtent, &exQ );
  *extent3D = exQ.bounds;
  return TRUE;
}


BOOL frame::CalcBillboardMatrix(frame* camera, Matrix* bbMatrix )
{
  // make sure local matrix is valid
  if (dirty)
    SetupMatrix();

  // if this node is a billboard, we don't search further up the parent tree
  // just calc the billboard-aware matrix
  if ( billboard )
  { 
    // 1) get the camera's position in world coordinates
    // 2) get the xform from the camera world pos to the billboard coordinate system to get the vector camera->billboard
    // 3) project that vector to the xz plane and get the vector's xz angle
    // 4) rotate the billboard frame counterclockwise to turn the z-axis to that angle 
    // 5) apply that rotation before applying the billboard's world matrix
    //    NOTE: we use the frame's world matrix instead of just composing one from 
    //    easily calculated vectors so that matrices containing scale work properly

    // STEP 1)
    Matrix mCam = *(camera->GetWorldMatrix()); // use camera's world matrix for position info (as it could be part of a hierarchy)

    // STEP 2)
    Matrix bill2world = *(GetWorldMatrix());
    Matrix world2bill(bill2world.Inverse());
    Matrix billboardCC;       

    // get the camera's world position
    vector cam; 
    cam.x = mCam(3,0); cam.y = mCam(3,1); cam.z = mCam(3,2);

    float angXZ = 0.0f;

    if ( !IsScreenAligned() ) // not screen aligned, normal billboard mode
    {
      world2bill.TransformVector(cam); 

      // if camera is right on top of billboard frame, don't even draw
      if (cam.x == 0 && cam.z == 0)
        return FALSE;

      // STEP 3)
      angXZ = (float)atan2(cam.z, cam.x);

      // STEP 4)           
//      billboardCC = UTILS_RotateYMatrix( +(angXZ-(PIFLOAT/2)) ); // z-axis points to pi/2, rotate it to new XZ angle
    }
    else // screen aligned, use the camera direction (not position) to find the rotation angle
    {
      // get the camera direction vector in world coordinates
      vector camDir(0, 0, 1);
      mCam.TransformVector(camDir);
      world2bill.TransformVector(camDir); // now in billboard frame space
      world2bill.TransformVector(cam);     // same for the camera position
      camDir = camDir - cam; // convert to a direction vector in the origin of billboard frame space

      // STEP 3)
      angXZ = (float)atan2(camDir.z, camDir.x);
    }

    // z-axis points to pi/2, rotate it to new XZ angle
    billboardCC.RotateY( -(angXZ-(PIFLOAT/2)) ); 

    // STEP 5)
    *bbMatrix = billboardCC;
    return TRUE;
  }

  // if this node is NOT a billboard search further up the parent tree for the most immediate billboard 
  else 
  {
    // optimization here when all parents up to the root are identity matrices and non-billboard frames
    frame *testparent = parent;
    while ( testparent != 0 )
    {
      if ( !testparent->MatrixIsIdentity() || testparent->billboard )
      {
        Matrix forParent;
        if (! parent->CalcBillboardMatrix( camera, &forParent ) )
          return FALSE;

        *bbMatrix = local * forParent;
        return TRUE;
      }
      
      testparent=testparent->parent;
    }

    // all parents are identity and non-billboard, so billboard matrix is same as local
    *bbMatrix = local; 
    return TRUE;
  }

   return FALSE; // should never get here
}
