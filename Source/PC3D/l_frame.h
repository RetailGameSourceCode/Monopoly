#ifndef FRAME_H
#define FRAME_H

class mesh;
class frame;

// callback for mesh enumerations. should return TRUE if enumeration is to continue.
typedef BOOL meshEnumCallback(mesh *m, frame *local, void *data);


const BILLBOARD_ALIGN = 0x02;


// .......... A frame is the fundamental node in a scene graph. A scene graph is a tree
// .......... of 'frame' nodes, where each frame  has a set of children and a transformation
// .......... matrix compatible with VRML 2.0.
// .......... NOTE: frames own their child frames, however they do NOT own their meshes. It
// .......... is the user's responsibility to delete any meshes that were created.
class frame
{
  struct meshRec 
  {
    mesh*           n;
    meshRec*        next;
    ~meshRec()      { DESTROYPOINTER(next);  }
  };
  
  meshRec*  contents;
  BOOL      override;
  Matrix    local;
  Matrix    world;
  frame*    parent;
  int       dirty;
  int       worldDirty;
  int       isIdentity;
  int       key;
  BOOL      billboard;
  
  CvectList<frame*> children;
  
  coord   opx, opy, opz;                // current position of object
  coord   osx, osy, osz;                // current scale of object
  coord   odx, ody, odz, oux, ouy, ouz; // current orientation of object
  coord   scx, scy, scz, scang;         // current axis & rotation for scaling
  coord   ax, ay, az, ang;              // current orientation of object in
                                        // terms of axis, angle
  coord   ofx, ofy, ofz;                // current local offset of object (for
                                        // scaling & orientation)
  
  void    SetDirty();
  void    SetupMatrix();
  void    SetWorldDirty();
  void    SetupWorldMatrix();

  public: 
    frame(frame* initParent = NULL, mesh* n = NULL);
    ~frame()  { Destroy(); }
    
    void Initialize(frame *initParent = NULL, mesh* pMesh = NULL);
    void Destroy();

    // ..........Overrides the position, orientation & scale values for the matrix
    // ..........To use original values, call 'overrideMatrix' with NULL 
    // .......... Note that even if you override the matrix
    // ..........values, any calls to get the position, orientation, or scale 
    // ..........will return the original values. It is only the computed matrix that 
    // ..........is altered.
    void    OverrideMatrix(D3DMATRIX* m) 
    { override = (m)? TRUE : FALSE; if (m) local = *m; SetDirty();          }

    // .......... returns pointers to the internal matrices...use with care.
    Matrix*  GetMatrix() {if (dirty) SetupMatrix(); return &local;}
    int      MatrixIsIdentity() {if (dirty) SetupMatrix(); return isIdentity;}
    Matrix*  GetWorldMatrix() {if (worldDirty) SetupWorldMatrix(); return &world;}
    
    void    SetPosition(coord x, coord y, coord z);
    void    SetPosition(vector *pos) {SetPosition(pos->x,pos->y,pos->z);}
    void    GetPosition(vector *pos);
    
    // .......... a local offset to the point considered the 'center' of the frame for scaling and orientation
    void    SetOffset(coord x, coord y, coord z);
    void    SetOffset(vector *off) {SetOffset(off->x,off->y,off->z);}
    void    GetOffset(vector *off);
    
    void    SetDirection( vector* dir ) {SetOrientation( dir->x, dir->y, dir->z, oux, ouy, ouz );}
    void    GetDirection( vector* dir); 

    void    SetUp( vector* up ){SetOrientation( odx, ody, odz, up->x, up->y, up->z );}
    void    GetUp( vector* up); 

    // .......... one way to set the frame's orientation,
    void    SetOrientation( coord dx, coord dy, coord dz,
                  coord ux, coord uy, coord uz );
    void    SetOrientation( vector* dir, vector *up ){SetOrientation( dir->x, dir->y, dir->z, up->x, up->y, up->z );}
    void    GetOrientation( vector* dir, vector *up );  
    
    // .......... an alternate way to set the frame's orientation,
    // .......... i.e. by specifying a counterclockwise rotation about an arbitrary axis
    void    SetOrientation( vector* axis, coord angle );
    
    // .......... scale is centred about the 'Offset' and oriented along 'scaleOrientation'
    void    SetScale(coord sx, coord sy, coord sz);
    void    SetScale(vector *scale) {SetScale(scale->x,scale->y,scale->z);}
    void    GetScale(vector *scale);
    
    void    SetScaleOrientation(coord sx, coord sy, coord sz, coord ang);
    void    SetScaleOrientation(vector *axis, coord ang) {SetScaleOrientation(axis->x,axis->y,axis->z, ang);}
    void    GetScaleOrientation(vector *axis, coord *ang);
    
    // see VRML Billboard node description on billboards
    void    SetBillboardMode(BOOL on = TRUE, BOOL align = FALSE) {billboard = on; if (on && align) billboard |= BILLBOARD_ALIGN;}
    BOOL    IsBillboard() {return billboard & TRUE;}
    BOOL    IsScreenAligned() {return billboard & BILLBOARD_ALIGN;}

    // calculates a billboard-aware world matrix for the frame relative to the given camera
    // returns FALSE if no valid billboard can be calculated, e.g. if the camera is 
    // right on top of the billboard y-axis or something degenerate like that. 
    // Note that this calculation takes into account any parent frames which might be billboards
    // even if this frame is NOT a billboard, so the correct world matrix is returned,
    // If this frame is known to be unaffected by a billboard, 'getWorldMatrix' should be used instead,
    // as it is potentially faster.
    BOOL    CalcBillboardMatrix(frame* camera, Matrix* bbMatrix);

    // .......... application specific key, ID, or whatever you like  
    int     GetKey() {return key;}
    void    SetKey(int newKey) { key = newKey; }

    mesh *  GetMesh(int index);
    int     GetNumMeshes();
    void    AddMesh(mesh *n);
    void    DetachMesh(mesh *n);

    frame * GetParent() {return parent;}
    void    AddChild(frame *child) {assert(child); children.add( child ); child->parent = this;}
    frame * GetChild(int index) {return children[index];}
    int     GetNumChildren() {return children.count();}
    void    DetachChild(frame *child) {children.detach( child ); child->parent = 0;}
    
    // .......... enumerates all the meshes in the frame, traversing any child frames before itself
    BOOL    EnumMeshes(meshEnumCallback *callback, void *data);

    // .......... Calc the frame's meshes' bounding box relative to the given frame.
    // .......... Any child frames of this frame are included in the calculation.
    // .......... If 'inFrame' is NULL, the bounding box will be in world coordinates
    BOOL    GetExtent3D(frame *inFrame, Box *extent3D);

    // .......... Transforms 'numPts' vertices from the given coordinate system 'from'
    // .......... to a target coordinate system 'to'. If 'to' is NULL, the input points
    // .......... are transformed to the root of the 'from' frame tree (i.e. world coordinates).
    // .......... If 'from' is NULL, the input points are assumed world coordinates. 'in & out
    // .......... may be the same buffer. 
    // .......... The in & out buffers are lists of vertices with floating point x,y,& z values
    // .......... in sequence. 'stride' is the value in bytes between each vertex in the
    // .......... list. This is useful for transforming different vertex structure types. For example,
    // ...........struct MyVertex {float x,y,z; float u,v;} would have a 'stride' sizeof (MyVertex),
    // ...........and in & out arrays would both be lists of 'MyVertex' structures.       
    // ........... (NOT YET IMPLEMENTED)
    static void Transform(frame *from, frame *to, int numPts, int stride, void *in, void *out);

};/* end class frame */


#endif