#ifndef _HMDLOAD_H_
#define _HMDLOAD_H_

#include "l_mesh.h"

/******************************************************************
 * HMD loader
 ******************************************************************/

struct HMD_MaterialTracker
{
  ULONG                   index;
  material*               myMaterial;
  Surface*                matTexture;
  CLRVALUE                cVector;
  HMD_MaterialTracker*    next;
};

struct HMD_TimTracker
{
  ULONG             tpage;
  ULONG             clut;
  int               x,y,w,h;
  Surface*          mySurface;
  HMD_TimTracker*   next;
};

struct NewVertex
{
  D3DVERTEX     vertex;
  int           normal;
  int           uv;
  NewVertex*    next;
  NewVertex*    sibling;
};

struct NewFace
{
  int         v1, v2, v3;
  NewFace*   next;
};

struct NewGroup
{
  Surface*    texture;
  material*   myMaterial;
  ULONG       materialIndex;
  UINT        DataIDSW;
  UINT        DataIDHW;
  int         gouraud;
  int         offset;
  int         number;
  CLRVALUE    cVector;
  NewGroup*   pNext;
  NewFace*    pFaces;
};

struct DiffBlock
{
  ULONG*        HMD_VertexDiffBlock;
  ULONG*        HMD_NormalDiffBlock;
  DiffBlock*    next;
};

#define   HMD_COPYVERTICES  0x00000001
#define   HMD_COPYGROUPS    0x00000002
#define   HMD_COPYFACES     0x00000004
#define   HMD_COPYBOXES     0x00000008
#define   HMD_COPYREFERENCE 0x00000010
#define   HMD_COPYALL     HMD_COPYVERTICES | HMD_COPYGROUPS | HMD_COPYFACES | HMD_COPYBOXES
/********************************************************************
 * Okay, here's the explanation we've all been waiting for.
 *
 * MESHX is a non-hierarchical mesh specification (ie: no sub-meshes
 * or otherwise seperate parts) meant to be used as interpolation
 * fodder for the engine.
 * 
 * If the source HMD has MIMe information, the subVertices and
 * interpolation buffers will be initialized.
 *
 * Otherwise, the Groups pointers divides all of the faces of the
 * object into their respective D3D materials: (ie: groups of
 * gouraud shaded red polygons or flat texture with texture X)
 *
 * During normal operation. If the model is not being interpolated,
 * the vertices pointer will be pointing at one of the subVertices
 * arrays, otherwise it will point at the interpolation buffer.
 *
 * If the object has a single mesh in it. (ie: no MIMe data) the
 * interpolationBuffer will be NULL, as will the subVertices pointer.
 *
 ********************************************************************/
class meshx : public mesh
{
  private:
    int         m_nCopy;
    int         m_nLinear;

  public:
    int         m_nType;
    int         m_nSize;
    int         m_nVertices;
    int         m_nFaces;
    int         m_nGroups;
    int         m_nSubVertices;
    int         m_nSubVerticesVtx;
    int         m_nSubVerticesNrm;
    NewGroup*   m_pGroups;
    D3DVERTEX*  m_pVertices;
    D3DVERTEX** m_ppSubVertices;
    D3DVERTEX*  m_pInterpolationBuffer;
    WORD*       m_pFaces;
    NewVertex*  m_pNewVertices;
    DiffBlock*  m_pDiffBlocks;
    Box         m_BBox;
    Box*        m_pTweenBoxes;

    meshx( void );
    meshx( HMD_Header *header );
    ~meshx( void );

    virtual int   getType() const;
    virtual void  getBox(Box& bounds) const { bounds = m_BBox; };

    meshx   *copy( void );
    meshx   *copy( int flags );


    int     initialized( void );

    NewGroup  *GroupAt( int index );

    int     IndexOfVertex( D3DVALUE x, D3DVALUE y, D3DVALUE z,
                  D3DVALUE nx, D3DVALUE ny, D3DVALUE nz,
                  D3DVALUE u, D3DVALUE v );
    int     IndexOfVertex( D3DVALUE x, D3DVALUE y, D3DVALUE z,
                  D3DVALUE nx, D3DVALUE ny, D3DVALUE nz );
    int     IndexOfVertex( D3DVALUE x, D3DVALUE y, D3DVALUE z, D3DVALUE u, D3DVALUE v );
    int     IndexOfVertex( D3DVALUE x, D3DVALUE y, D3DVALUE z );
  
    NewGroup  *AddGroup( void );
    NewVertex *AddVertex( void );
    NewVertex *VertexAt( int position );
    NewFace   *AddFace( NewGroup *to );
    NewGroup  *LastGroup( void );
    DiffBlock *AddVertexDiffBlock( void );
    DiffBlock *AddNormalDiffBlock( void );
    DiffBlock *AddDiffBlock( void );
  
    void  Linearize( void );
    int   HMD_MapType( ULONG *type, ULONG *header );
    void  HMD_MapUnit( ULONG *unit, ULONG *origin );
    void  HMD_Map( HMD_Header *data );
    NewGroup* GroupThatMatches(Surface* texture, int gouraud, 
                               CLRVALUE* cVector);

    void  SetVertexAndSiblings(ULONG vertex, ULONG normal,
                               NewVertex *CurrentVertex);

    void HMD_interpolate( int pose0, int pose1, D3DVALUE amount );

};

struct HMD_RenderBuffer
{
  HMD_RenderBuffer*  next;
  NewGroup*           group;
  meshx*              mesh;
  frame*              frame;
};

#define HMD_BUFFERS_IN_TRACKER 64

struct HMD_RenderBufferTracker
{
  HMD_RenderBufferTracker*  next;
  int                       indexUsed;
  HMD_RenderBuffer          buffers[ HMD_BUFFERS_IN_TRACKER ];
};


int render( meshx *m, view *in );
int HMD_LoadTIM( ULONG *type, ULONG *header );
int HMD_LoadTIM( void *timPointer );

#ifdef __cplusplus
extern "C" {
#endif

HMD_TimTracker *CompatibleTracker( int x, int y, ULONG tpage );
HMD_TimTracker *CompatibleTracker2( int x, int y, int w, int h, ULONG tpage );

void HMD_RestoreCurrentBank( void );
void HMD_RestoreAll( void );
void HMD_ReleaseCurrentBank( void );
void HMD_ReleaseAll( void );

void HMD_CollectForRendering( frame *aFrame );
void HMD_RenderCollected( view *aView );
void HMD_ClearCollected( void );

void HMD_ClearMaterials( void );

void HMD_ReleaseRenderBuffers( void );

/********************************************************************
 * For shutting the system down when re-initializing for when the
 * 3d system wants to shut down. NB: Doing this when you have more
 * than one 3d slot open would be bad. All 3D slots have to be either
 * on or off. Up or down at the same time. Capice?
 ********************************************************************/
void HMD_Shutdown( void );

HMD_RenderBuffer *HMD_NewRenderBuffer( void );

#define HMD_MAX_TEXTURE_BANKS 16
#define HMD_MAX_MATERIALS   255

void HMD_SetCurrentTimBank( int bank );
void HMD_LoadBMPForTIM(ULONG x, ULONG y, ULONG w, ULONG h, LPCSTR szBmpFile);
#define HMD_SetVertexXYZ(v,a,b,c) { v->vertex.x = a; v->vertex.y = b; v->vertex.z = c; }
#define HMD_SetVertexNXYZ(v,a,b,c) { v->vertex.nx = a; v->vertex.ny = b; v->vertex.nz = c; v->normal = 1; }
#define HMD_SetVertexUV(vt,u,v) { vt->vertex.tu = u; vt->vertex.tv = v; vt->uv = 1; }
#define HMD_CaresAboutUV(v) ( v->uv )
#define HMD_CaresAboutN(v) ( v->normal )

#ifdef __cplusplus
}
#endif


#endif 