/******************************************************************
 * HMD loader
 ******************************************************************/

/******************************************************************
 * INCLUDES
 ******************************************************************/

#include "PC3DHdr.h"
#include "l_material.h"
#include "Tag.h"

/*****************************************************************************
 * Standard C library include files.
 */

BOOL          g_HMD_UseDefaultShadingMode = FALSE;

#ifdef USE_OLD_FRAME

#define HMD_MemoryMalloc  LI_MEMORY_AllocDataBlock
#define HMD_MemoryFree    LI_MEMORY_DeallocDataBlock

#if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_PlatformWin95
  #include <process.h>  // Needed for BeginThread.
#endif


#include "../artlib/l_type.h"
#include "../artlib/l_memory.h"

#include "pc3d.h"
#include "scene.h"
#include "vectlist.h"
#include "camera.h"

#define GsOT int
#include "hmdstuff.h"
#include "sys\types.h"
#include "libgte.h"
#include "libgpu.h"
#include "libgs.h"
#include "libhmd.h"
#include "hmdload.h"


/******************************************************************
 * STRUCTURES
 ******************************************************************/


/******************************************************************
 * GLOBALS
 ******************************************************************/

//16 megs of available default space.
HMD_TimTracker*           HMD_Tracker[ HMD_MAX_TEXTURE_BANKS ][ 32 ];
HMD_MaterialTracker       HMD_Materials[ HMD_MAX_MATERIALS ];
HMD_RenderBuffer*         HMD_RenderBuffers[ HMD_MAX_MATERIALS ];
HMD_MaterialTracker*      HMD_MaterialsFreeHead     = NULL;
HMD_MaterialTracker*      HMD_MaterialsUsedHead     = NULL;
HMD_RenderBufferTracker*  HMD_RenderBufferTrackers  = NULL;

BITMAPINFO*   myBitmapInfo                = NULL;
material*     HMD_Material                = NULL;
int           HMD_CurrentTimBank          = 0;
BOOL          g_bHMD_TrackerInitialized   = FALSE;

#define NEGX 
#define NEGY -
#define NEGZ 
#define NNEGX 
#define NNEGY -
#define NNEGZ 

/********************************************************************
 * Local Functions
 ********************************************************************/

int AddMaterial( NewGroup *aGroup );
HMD_MaterialTracker *FindMaterial( NewGroup *aGroup );
HMD_MaterialTracker *AddGlobalMaterial( NewGroup *aGroup );

#define HighWord(x)         ( ( x & 0xFFFF0000 ) >> 16 )
#define LowWord(x)          ( x & 0x0000FFFF ) 
#define Byte1(x)            ( ( x & 0xFF000000 ) >> 24 )
#define Byte2(x)            ( ( x & 0x00FF0000 ) >> 16 )
#define Byte3(x)            ( ( x & 0x0000FF00 ) >> 8 )
#define Byte4(x)            ( x & 0x000000FF )
#define FD3DVALUE(a)        ( (D3DVALUE)a / (D3DVALUE)4096.0 )


void meshx::SetVertexAndSiblings(ULONG vertex, ULONG normal,
                                 NewVertex* CurrentVertex)
{
  ULONG start;
  ULONG number;
  ULONG DiffIndex;

  SVECTOR*    Diffs     = NULL;
  DiffBlock*  DBlock    = m_pDiffBlocks;
  NewVertex*  SubVertex = CurrentVertex->sibling;

  for (int i = 0; i < m_nSubVertices; ++i)
  {
    //Copy the vertex info.
    SubVertex->vertex = CurrentVertex->vertex;

    if (DBlock->HMD_VertexDiffBlock)
    {
      start     = DBlock->HMD_VertexDiffBlock[ 0 ];
      number    = HighWord( DBlock->HMD_VertexDiffBlock[ 1 ] );
      Diffs     = (SVECTOR *)( DBlock->HMD_VertexDiffBlock + 2 );
      DiffIndex = vertex - start;

      if (DiffIndex < number)
      {
        SubVertex->vertex.x += NEGX(D3DVALUE)Diffs[ DiffIndex ].vx;
        SubVertex->vertex.y += NEGY(D3DVALUE)Diffs[ DiffIndex ].vy;
        SubVertex->vertex.z += NEGZ(D3DVALUE)Diffs[ DiffIndex ].vz;
      }
    }

    if (DBlock->HMD_NormalDiffBlock)
    {
      start     = DBlock->HMD_NormalDiffBlock[ 0 ];
      number    = HighWord( DBlock->HMD_NormalDiffBlock[ 1 ] );
      Diffs     = (SVECTOR *)( DBlock->HMD_NormalDiffBlock + 2 );
      DiffIndex = normal - start;

      if( DiffIndex < number )
      {
        SubVertex->vertex.nx += NEGX(D3DVALUE)Diffs[ DiffIndex ].vx;
        SubVertex->vertex.ny += NEGY(D3DVALUE)Diffs[ DiffIndex ].vy;
        SubVertex->vertex.nz += NEGZ(D3DVALUE)Diffs[ DiffIndex ].vz;
      }
    }
    DBlock    = DBlock->next;
    SubVertex = SubVertex->sibling;
  }
}

int meshx::HMD_MapType(ULONG* type, ULONG* header)
{
  //Okay, here is where we check the various types.
  static CLRVALUE cVector(1.0f, 1.0f, 1.0f, 1.0f);

  ULONG       i,j;
  ULONG       size              = ( *( type + 1 ) & 0x0000FFFF ) + 1;
  ULONG       number            = *( type + 1 ) >> 16;
  ULONG*      Polys             = (ULONG *)*( header + 1 );
  SVECTOR*    Vertices          = (SVECTOR *)*( header + 2 );
  SVECTOR*    Normals           = (SVECTOR *)*( header + 3 );
  int         foo               = *type & 0xFF7FFFFF;
  ULONG*      CurrentPoly;
  ULONG*      CurrentBlockBase;
  ULONG*      CurrentBlock;
  DiffBlock*  CurrentDiffBlock;
  NewGroup*   CurrentGroup;
  ULONG       NumberOfDiffBlocks;
  int         dwordSize;
  D3DVALUE    x[ 3 ], y[ 3 ], z[ 3 ], u[ 3 ], v[ 3 ], nx[ 3 ], ny[ 3 ], nz[ 3 ];
  int         faceVerts[ 3 ];
  HMD_TimTracker* tracker;
  int         tpage;
  int         ou,ov;
  NewFace*    CurrentFace;
  NewVertex*  CurrentVertex;

  ULONG       vertexID[ 3 ];
  ULONG       normalID[ 3 ];

  int         myX, myY;

  if( !(number & 0x00008000) )
    return size;
  else
    number &= 0x00007FFF;

  switch( foo )
  {
    case  GsUF3:      // 0x00000008 flat triangle 
      dwordSize = 3;
      CurrentPoly = Polys + *( type + 2 );
      for( i = 0; i < number; ++i )
      {
        cVector.r = (D3DVALUE)Byte4( CurrentPoly[ 0 ] ) / (D3DVALUE)255;
        cVector.g = (D3DVALUE)Byte3( CurrentPoly[ 0 ] ) / (D3DVALUE)255;
        cVector.b = (D3DVALUE)Byte2( CurrentPoly[ 0 ] ) / (D3DVALUE)255;

        CurrentGroup = GroupThatMatches( NULL, FALSE, &cVector );

        if( CurrentGroup != NULL )
        {
          ++CurrentGroup->number;
        }
        else
        {
          CurrentGroup            = AddGroup();
          CurrentGroup->number    = 1;
          CurrentGroup->gouraud   = FALSE;
          CurrentGroup->cVector.r = (D3DVALUE)Byte4( CurrentPoly[ 0 ] ) / (D3DVALUE)255;
          CurrentGroup->cVector.g = (D3DVALUE)Byte3( CurrentPoly[ 0 ] ) / (D3DVALUE)255;
          CurrentGroup->cVector.b = (D3DVALUE)Byte2( CurrentPoly[ 0 ] ) / (D3DVALUE)255;
          CurrentGroup->cVector.a = 1.0f;
          CurrentGroup->texture   = NULL;
          AddMaterial( CurrentGroup );
        }

        CurrentFace = AddFace( CurrentGroup );

        vertexID[ 0 ] = HighWord( CurrentPoly[ 1 ] );
        vertexID[ 1 ] = LowWord( CurrentPoly[ 2 ] );
        vertexID[ 2 ] = HighWord( CurrentPoly[ 2 ] );

        normalID[ 0 ] = LowWord( CurrentPoly[ 1 ] );
        normalID[ 1 ] = LowWord( CurrentPoly[ 1 ] );
        normalID[ 2 ] = LowWord( CurrentPoly[ 1 ] );

        x[ 0 ]  = NEGX(D3DVALUE)Vertices[ vertexID[ 0 ] ].vx;
        y[ 0 ]  = NEGY(D3DVALUE)Vertices[ vertexID[ 0 ] ].vy;
        z[ 0 ]  = NEGZ(D3DVALUE)Vertices[ vertexID[ 0 ] ].vz;
        nx[ 0 ] = NNEGX( FD3DVALUE( Normals[ normalID[ 0 ] ].vx ) );
        ny[ 0 ] = NNEGY( FD3DVALUE( Normals[ normalID[ 0 ] ].vy ) );
        nz[ 0 ] = NNEGZ( FD3DVALUE( Normals[ normalID[ 0 ] ].vz ) );

        x[ 1 ]  = NEGX(D3DVALUE)Vertices[ vertexID[ 1 ] ].vx;
        y[ 1 ]  = NEGY(D3DVALUE)Vertices[ vertexID[ 1 ] ].vy;
        z[ 1 ]  = NEGZ(D3DVALUE)Vertices[ vertexID[ 1 ] ].vz;
        nx[ 1 ] = nx[ 0 ];
        ny[ 1 ] = ny[ 0 ];
        nz[ 1 ] = nz[ 0 ];

        x[ 2 ]  = NEGX(D3DVALUE)Vertices[ vertexID[ 2 ] ].vx;
        y[ 2 ]  = NEGY(D3DVALUE)Vertices[ vertexID[ 2 ] ].vy;
        z[ 2 ]  = NEGZ(D3DVALUE)Vertices[ vertexID[ 2 ] ].vz;
        nx[ 2 ] = nx[ 0 ];
        ny[ 2 ] = ny[ 0 ];
        nz[ 2 ] = nz[ 0 ];

        for( j = 0; j < 3; ++j )
        {
          faceVerts[ j ] = IndexOfVertex( x[ j ], y[ j ], z[ j ],
                                         nx[ j ], ny[ j ], nz[ j ] );

          if( faceVerts[ j ] < 0 )
          {
            faceVerts[ j ]  = m_nVertices;
            CurrentVertex   = AddVertex();
            HMD_SetVertexXYZ( CurrentVertex, x[ j ], y[ j ], z[ j ] );
            HMD_SetVertexNXYZ( CurrentVertex, nx[ j ], ny[ j ], nz[ j ] );
            SetVertexAndSiblings( vertexID[ j ], normalID[ j ], CurrentVertex );
          }
        }

        CurrentFace->v1 = faceVerts[ 0 ];
        CurrentFace->v2 = faceVerts[ 1 ];
        CurrentFace->v3 = faceVerts[ 2 ];

        //Do the pointer arithmetic
        CurrentPoly = CurrentPoly + dwordSize;
      }
      break;

    case  GsUFT3:     // 0x00000009 texture flat triangle 
      dwordSize = 5;
      CurrentPoly = Polys + *( type + 2 );
      for( i = 0; i < number; ++i )
      {
        ou = Byte4( CurrentPoly[ 0 ] );
        ov = Byte3( CurrentPoly[ 0 ] );
        tpage = HighWord( CurrentPoly[ 1 ] );
        tracker = CompatibleTracker( ou, ov, tpage );
        if( tracker == NULL )
        {
          TRACE("Texture tracker not found, rejecting %d triangles\n", 
                number - i);
          return size;
        }
        CurrentGroup = LastGroup();

        cVector.r = (D3DVALUE)1.0;
        cVector.g = (D3DVALUE)1.0;
        cVector.b = (D3DVALUE)1.0;

        CurrentGroup = GroupThatMatches( tracker->mySurface, FALSE, &cVector );

        if( CurrentGroup != NULL )
          ++CurrentGroup->number;
        else
        {
          CurrentGroup            = AddGroup();
          CurrentGroup->number    = 1;
          CurrentGroup->gouraud   = FALSE;
          CurrentGroup->texture   = tracker->mySurface;
          CurrentGroup->cVector.r = (D3DVALUE)1.0;
          CurrentGroup->cVector.g = (D3DVALUE)1.0;
          CurrentGroup->cVector.b = (D3DVALUE)1.0;
          CurrentGroup->cVector.a = 1.0f;
          AddMaterial( CurrentGroup );
        }

        // Make sure the coords are masked properly for 16bit pixels
        // Left shifting by one is the same as multiplying by 2.
        // (Need to multiply by 2 for 8 bit textures)
        myX = (tracker->x & 63) << 1; 
        myY = tracker->y & 255;

        CurrentFace = AddFace( CurrentGroup );

        vertexID[ 0 ] = HighWord( CurrentPoly[ 3 ] );
        vertexID[ 1 ] = LowWord( CurrentPoly[ 4 ] );
        vertexID[ 2 ] = HighWord( CurrentPoly[ 4 ] );

        normalID[ 0 ] = LowWord( CurrentPoly[ 3 ] );
        normalID[ 1 ] = LowWord( CurrentPoly[ 3 ] );
        normalID[ 2 ] = LowWord( CurrentPoly[ 3 ] );

        x[ 0 ]  = NEGX(D3DVALUE)Vertices[ vertexID[ 0 ] ].vx;
        y[ 0 ]  = NEGY(D3DVALUE)Vertices[ vertexID[ 0 ] ].vy;
        z[ 0 ]  = NEGZ(D3DVALUE)Vertices[ vertexID[ 0 ] ].vz;
        nx[ 0 ] = NNEGX( FD3DVALUE( Normals[ normalID[ 0 ] ].vx ) );
        ny[ 0 ] = NNEGY( FD3DVALUE( Normals[ normalID[ 0 ] ].vy ) );
        nz[ 0 ] = NNEGZ( FD3DVALUE( Normals[ normalID[ 0 ] ].vz ) );
        u[ 0 ]  = (D3DVALUE)( Byte4( CurrentPoly[ 0 ] ) - myX ) / (D3DVALUE)( tracker->w );
        v[ 0 ]  = (D3DVALUE)( Byte3( CurrentPoly[ 0 ] ) - myY ) / (D3DVALUE)( tracker->h );

        x[ 1 ]  = NEGX(D3DVALUE)Vertices[ vertexID[ 1 ] ].vx;
        y[ 1 ]  = NEGY(D3DVALUE)Vertices[ vertexID[ 1 ] ].vy;
        z[ 1 ]  = NEGZ(D3DVALUE)Vertices[ vertexID[ 1 ] ].vz;
        nx[ 1 ] = nx[ 0 ];
        ny[ 1 ] = ny[ 0 ];
        nz[ 1 ] = nz[ 0 ];
        u[ 1 ]  = (D3DVALUE)( Byte4( CurrentPoly[ 1 ] ) - myX ) / (D3DVALUE)( tracker->w );
        v[ 1 ]  = (D3DVALUE)( Byte3( CurrentPoly[ 1 ] ) - myY ) / (D3DVALUE)( tracker->h );

        x[ 2 ]  = NEGX(D3DVALUE)Vertices[ vertexID[ 2 ] ].vx;
        y[ 2 ]  = NEGY(D3DVALUE)Vertices[ vertexID[ 2 ] ].vy;
        z[ 2 ]  = NEGZ(D3DVALUE)Vertices[ vertexID[ 2 ] ].vz;
        nx[ 2 ] = nx[ 0 ];
        ny[ 2 ] = ny[ 0 ];
        nz[ 2 ] = nz[ 0 ];
        u[ 2 ]  = (D3DVALUE)( Byte4( CurrentPoly[ 2 ] ) - myX ) / (D3DVALUE)( tracker->w );
        v[ 2 ]  = (D3DVALUE)( Byte3( CurrentPoly[ 2 ] ) - myY ) / (D3DVALUE)( tracker->h );

        for( j = 0; j < 3; ++j )
        {
          faceVerts[ j ] = IndexOfVertex( x[ j ], y[ j ], z[ j ], nx[ j ], 
                                         ny[ j ], nz[ j ], u[ j ], v[ j ] );

          if( faceVerts[ j ] < 0 )
          {
            faceVerts[ j ] = m_nVertices;
            CurrentVertex = AddVertex();
          }
          else
            CurrentVertex = VertexAt( faceVerts[ j ] );

          HMD_SetVertexXYZ( CurrentVertex, x[ j ], y[ j ], z[ j ] );
          HMD_SetVertexNXYZ( CurrentVertex, nx[ j ], ny[ j ], nz[ j ] );
          HMD_SetVertexUV( CurrentVertex, u[ j ], v[ j ] );
          SetVertexAndSiblings( vertexID[ j ], normalID[ j ], CurrentVertex );
        }

        CurrentFace->v1 = faceVerts[ 0 ];
        CurrentFace->v2 = faceVerts[ 1 ];
        CurrentFace->v3 = faceVerts[ 2 ];

        //Do the pointer arithmetic
        CurrentPoly = CurrentPoly + dwordSize;
      }
      break;

    case  GsUG3:      // 0x0000000c gour triangle 
      dwordSize = 4;
      CurrentPoly = Polys + *( type + 2 );
      for( i = 0; i < number; ++i )
      {
        cVector.r = (D3DVALUE)Byte4( CurrentPoly[ 0 ] ) / (D3DVALUE)255;
        cVector.g = (D3DVALUE)Byte3( CurrentPoly[ 0 ] ) / (D3DVALUE)255;
        cVector.b = (D3DVALUE)Byte2( CurrentPoly[ 0 ] ) / (D3DVALUE)255;

        CurrentGroup = GroupThatMatches( NULL, TRUE, &cVector );

        if( CurrentGroup != NULL )
          ++CurrentGroup->number;
        else
        {
          CurrentGroup            = AddGroup();
          CurrentGroup->number    = 1;
          CurrentGroup->gouraud   = TRUE;
          CurrentGroup->cVector.r = (D3DVALUE)Byte4( CurrentPoly[ 0 ] ) / (D3DVALUE)255;
          CurrentGroup->cVector.g = (D3DVALUE)Byte3( CurrentPoly[ 0 ] ) / (D3DVALUE)255;
          CurrentGroup->cVector.b = (D3DVALUE)Byte2( CurrentPoly[ 0 ] ) / (D3DVALUE)255;
          CurrentGroup->cVector.a = 1.0f;
          CurrentGroup->texture   = NULL;
          AddMaterial( CurrentGroup );
        }

        CurrentFace = AddFace( CurrentGroup );

        vertexID[ 0 ] = HighWord( CurrentPoly[ 1 ] );
        vertexID[ 1 ] = HighWord( CurrentPoly[ 2 ] );
        vertexID[ 2 ] = HighWord( CurrentPoly[ 3 ] );

        normalID[ 0 ] = LowWord( CurrentPoly[ 1 ] );
        normalID[ 1 ] = LowWord( CurrentPoly[ 2 ] );
        normalID[ 2 ] = LowWord( CurrentPoly[ 3 ] );

        x[ 0 ]  = NEGX(D3DVALUE)Vertices[ vertexID[ 0 ] ].vx;
        y[ 0 ]  = NEGY(D3DVALUE)Vertices[ vertexID[ 0 ] ].vy;
        z[ 0 ]  = NEGZ(D3DVALUE)Vertices[ vertexID[ 0 ] ].vz;
        nx[ 0 ] = NNEGX( FD3DVALUE( Normals[ normalID[ 0 ] ].vx ) );
        ny[ 0 ] = NNEGY( FD3DVALUE( Normals[ normalID[ 0 ] ].vy ) );
        nz[ 0 ] = NNEGZ( FD3DVALUE( Normals[ normalID[ 0 ] ].vz ) );

        x[ 1 ]  = NEGX(D3DVALUE)Vertices[ vertexID[ 1 ] ].vx;
        y[ 1 ]  = NEGY(D3DVALUE)Vertices[ vertexID[ 1 ] ].vy;
        z[ 1 ]  = NEGZ(D3DVALUE)Vertices[ vertexID[ 1 ] ].vz;
        nx[ 1 ] = NNEGX( FD3DVALUE( Normals[ normalID[ 1 ] ].vx ) );
        ny[ 1 ] = NNEGY( FD3DVALUE( Normals[ normalID[ 1 ] ].vy ) );
        nz[ 1 ] = NNEGZ( FD3DVALUE( Normals[ normalID[ 1 ] ].vz ) );

        x[ 2 ]  = NEGX(D3DVALUE)Vertices[ vertexID[ 2 ] ].vx;
        y[ 2 ]  = NEGY(D3DVALUE)Vertices[ vertexID[ 2 ] ].vy;
        z[ 2 ]  = NEGZ(D3DVALUE)Vertices[ vertexID[ 2 ] ].vz;
        nx[ 2 ] = NNEGX( FD3DVALUE( Normals[ normalID[ 2 ] ].vx ) );
        ny[ 2 ] = NNEGY( FD3DVALUE( Normals[ normalID[ 2 ] ].vy ) );
        nz[ 2 ] = NNEGZ( FD3DVALUE( Normals[ normalID[ 2 ] ].vz ) );

        for( j = 0; j < 3; ++j )
        {
          faceVerts[ j ] = IndexOfVertex( x[ j ], y[ j ], z[ j ],
                                         nx[ j ], ny[ j ], nz[ j ] );

          if( faceVerts[ j ] < 0 )
          {
            faceVerts[ j ] = m_nVertices;
            CurrentVertex = AddVertex();
            HMD_SetVertexXYZ( CurrentVertex, x[ j ], y[ j ], z[ j ] );
            HMD_SetVertexNXYZ( CurrentVertex, nx[ j ], ny[ j ], nz[ j ] );
            SetVertexAndSiblings( vertexID[ j ], normalID[ j ], CurrentVertex );
          }
        }

        CurrentFace->v1 = faceVerts[ 0 ];
        CurrentFace->v2 = faceVerts[ 1 ];
        CurrentFace->v3 = faceVerts[ 2 ];

        //Do the pointer arithmetic
        CurrentPoly = CurrentPoly + dwordSize;
      }
      break;

    case  GsUGT3:     // 0x0000000d texture gour triangle 
      dwordSize = 6;
      CurrentPoly = Polys + *( type + 2 );
      for( i = 0; i < number; ++i )
      {
        ou      = Byte4( CurrentPoly[ 0 ] );
        ov      = Byte3( CurrentPoly[ 0 ] );
        tpage   = HighWord( CurrentPoly[ 1 ] );
        tracker = CompatibleTracker( ou, ov, tpage );

        if( tracker == NULL )
        {
          TRACE("Texture tracker not found, rejecting %d triangles\n", 
                number - i);
          return size;
        }

        cVector.r = (D3DVALUE)1.0;
        cVector.g = (D3DVALUE)1.0;
        cVector.b = (D3DVALUE)1.0;

        CurrentGroup = GroupThatMatches( tracker->mySurface, TRUE, &cVector );

        if( CurrentGroup != NULL )
          ++CurrentGroup->number;
        else
        {
          CurrentGroup            = AddGroup();
          CurrentGroup->number    = 1;
          CurrentGroup->gouraud   = TRUE;
          CurrentGroup->texture   = tracker->mySurface;
          CurrentGroup->cVector.r = (D3DVALUE)1.0;
          CurrentGroup->cVector.g = (D3DVALUE)1.0;
          CurrentGroup->cVector.b = (D3DVALUE)1.0;
          CurrentGroup->cVector.a = 1.0f;
          AddMaterial( CurrentGroup );
        }

        // Make sure the coords are masked properly for 16bit pixels
        // Left shifting by one is the same as multiplying by 2.
        // (Need to multiply by 2 for 8 bit textures)
        myX = (tracker->x & 63) << 1; 
        myY = tracker->y & 255;

        CurrentFace = AddFace( CurrentGroup );

        vertexID[ 0 ] = HighWord( CurrentPoly[ 3 ] );
        vertexID[ 1 ] = HighWord( CurrentPoly[ 4 ] );
        vertexID[ 2 ] = HighWord( CurrentPoly[ 5 ] );

        normalID[ 0 ] = LowWord( CurrentPoly[ 3 ] );
        normalID[ 1 ] = LowWord( CurrentPoly[ 4 ] );
        normalID[ 2 ] = LowWord( CurrentPoly[ 5 ] );

        x[ 0 ]  = NEGX(D3DVALUE)Vertices[ vertexID[ 0 ] ].vx;
        y[ 0 ]  = NEGY(D3DVALUE)Vertices[ vertexID[ 0 ] ].vy;
        z[ 0 ]  = NEGZ(D3DVALUE)Vertices[ vertexID[ 0 ] ].vz;
        nx[ 0 ] = NNEGX( FD3DVALUE( Normals[ normalID[ 0 ] ].vx ) );
        ny[ 0 ] = NNEGY( FD3DVALUE( Normals[ normalID[ 0 ] ].vy ) );
        nz[ 0 ] = NNEGZ( FD3DVALUE( Normals[ normalID[ 0 ] ].vz ) );
        u[ 0 ]  = (D3DVALUE)( Byte4( CurrentPoly[ 0 ] ) - myX ) / (D3DVALUE)( tracker->w );
        v[ 0 ]  = (D3DVALUE)( Byte3( CurrentPoly[ 0 ] ) - myY ) / (D3DVALUE)( tracker->h );

        x[ 1 ]  = NEGX(D3DVALUE)Vertices[ vertexID[ 1 ] ].vx;
        y[ 1 ]  = NEGY(D3DVALUE)Vertices[ vertexID[ 1 ] ].vy;
        z[ 1 ]  = NEGZ(D3DVALUE)Vertices[ vertexID[ 1 ] ].vz;
        nx[ 1 ] = NNEGX( FD3DVALUE( Normals[ normalID[ 1 ] ].vx ) );
        ny[ 1 ] = NNEGY( FD3DVALUE( Normals[ normalID[ 1 ] ].vy ) );
        nz[ 1 ] = NNEGZ( FD3DVALUE( Normals[ normalID[ 1 ] ].vz ) );
        u[ 1 ]  = (D3DVALUE)( Byte4( CurrentPoly[ 1 ] ) - myX ) / (D3DVALUE)( tracker->w );
        v[ 1 ]  = (D3DVALUE)( Byte3( CurrentPoly[ 1 ] ) - myY ) / (D3DVALUE)( tracker->h );

        x[ 2 ]  = NEGX(D3DVALUE)Vertices[ vertexID[ 2 ] ].vx;
        y[ 2 ]  = NEGY(D3DVALUE)Vertices[ vertexID[ 2 ] ].vy;
        z[ 2 ]  = NEGZ(D3DVALUE)Vertices[ vertexID[ 2 ] ].vz;
        nx[ 2 ] = NNEGX( FD3DVALUE( Normals[ normalID[ 2 ] ].vx ) );
        ny[ 2 ] = NNEGY( FD3DVALUE( Normals[ normalID[ 2 ] ].vy ) );
        nz[ 2 ] = NNEGZ( FD3DVALUE( Normals[ normalID[ 2 ] ].vz ) );
        u[ 2 ]  = (D3DVALUE)( Byte4( CurrentPoly[ 2 ] ) - myX ) / (D3DVALUE)( tracker->w );
        v[ 2 ]  = (D3DVALUE)( Byte3( CurrentPoly[ 2 ] ) - myY ) / (D3DVALUE)( tracker->h );

        for( j = 0; j < 3; ++j )
        {
          faceVerts[ j ] = IndexOfVertex( x[ j ], y[ j ], z[ j ], nx[ j ],
                                          ny[ j ], nz[ j ], u[ j ], v[ j ] );

          if( faceVerts[ j ] < 0 )
          {
            faceVerts[ j ] = m_nVertices;
            CurrentVertex = AddVertex();
          }
          else
            CurrentVertex = VertexAt( faceVerts[ j ] );

          HMD_SetVertexXYZ( CurrentVertex, x[ j ], y[ j ], z[ j ] );
          HMD_SetVertexNXYZ( CurrentVertex, nx[ j ], ny[ j ], nz[ j ] );
          HMD_SetVertexUV( CurrentVertex, u[ j ], v[ j ] );
          SetVertexAndSiblings( vertexID[ j ], normalID[ j ], CurrentVertex );
        }

        CurrentFace->v1 = faceVerts[ 0 ];
        CurrentFace->v2 = faceVerts[ 1 ];
        CurrentFace->v3 = faceVerts[ 2 ];

        //Do the pointer arithmetic
        CurrentPoly = CurrentPoly + dwordSize;
      }
      break;

//    case  GsUF4:      // 0x00000010 flat quad 
//    case  GsUFT4:     // 0x00000011 texture flat quad 
//    case  GsUG4:      // 0x00000014 gour quad 
//    case  GsUGT4:     // 0x00000015 texture gour quad 
//      break;
//
//    case  GsUFF3:     // 0x00020008 fog flat triangle 
//    case  GsUFFT3:    // 0x00020009 fog texture flat triangle 
//    case  GsUFG3:     // 0x0002000c fog gour triangle 
//    case  GsUFGT3:    // 0x0002000d fog texture gour triangle 
//    case  GsUFF4:     // 0x00020010 fog flat quad 
//    case  GsUFFT4:    // 0x00020011 fog texture flat quad 
//    case  GsUFG4:     // 0x00020014 fog gour quad 
//    case  GsUFGT4:    // 0x00020015 fog texture gour quad 
//      break;
//
//    case  GsUCF3:     // 0x0000000a colored flat triangle 
//    case  GsUCFT3:    // 0x0000000b colored texture flat triangle 
//    case  GsUCG3:     // 0x0000000e colored gour triangle 
//    case  GsUCGT3:    // 0x0000000f colored texture gour triangle 
//    case  GsUCF4:     // 0x00000012 colored flat quad 
//    case  GsUCFT4:    // 0x00000013 colored texture flat quad 
//    case  GsUCG4:     // 0x00000016 colored gour quad 
//    case  GsUCGT4:    // 0x00000017 colored texture gour quad 
//      break;
//
//    case  GsUNF3:     // 0x00040048 nonLight flat triangle 
//    case  GsUNFT3:    // 0x00040049 nonLight texture flat triangle 
//    case  GsUNG3:     // 0x0004004c nonLight gouraud triangle 
//    case  GsUNGT3:    // 0x0004004d nonLight texture gouraud triangle 
//    case  GsUNF4:     // 0x00040050 nonLight flat quad 
//    case  GsUNFT4:    // 0x00040051 nonLight texture flat quad 
//    case  GsUNG4:     // 0x00040054 nonLight gouraud quad 
//    case  GsUNGT4:    // 0x00040055 nonLight texture gouraud quad 
//      break;
//    
//    case  GsUDF3:     // 0x00010008 div flat triangle 
//    case  GsUDFT3:    // 0x00010009 div texture flat triangle 
//    case  GsUDG3:     // 0x0001000c div gour triangle 
//    case  GsUDGT3:    // 0x0001000d div texture gour triangle 
//    case  GsUDF4:     // 0x00010010 div flat quad 
//    case  GsUDFT4:    // 0x00010011 div texture flat quad 
//    case  GsUDG4:     // 0x00010014 div gour quad 
//    case  GsUDGT4:    // 0x00010015 div texture gour quad 
//      break;
//
//    case  GsUDFF3:    // 0x00030008 div fog flat triangle 
//    case  GsUDFFT3:   // 0x00030009 div fog texture flat triangle 
//    case  GsUDFG3:    // 0x0003000c div fog gour triangle 
//    case  GsUDFGT3:   // 0x0003000d div fog texture gour triangle 
//    case  GsUDFF4:    // 0x00030010 div fog flat quad 
//    case  GsUDFFT4:   // 0x00030011 div fog texture flat quad 
//    case  GsUDFG4:    // 0x00030014 div fog gour quad 
//    case  GsUDFGT4:   // 0x00030015 div fog texture gour quad 
//      break;
//
//    case  GsUDNF3:    // 0x00050048 div nonLight flat triangle 
//    case  GsUDNFT3:   // 0x00050049 div nonLight texture flat triangle 
//    case  GsUDNG3:    // 0x0005004c div nonLight gouraud triangle 
//    case  GsUDNGT3:   // 0x0005004d div nonLight tex gouraud triangle 
//    case  GsUDNF4:    // 0x00050050 div nonLight flat quad 
//    case  GsUDNFT4:   // 0x00050051 div nonLight texture flat quad 
//    case  GsUDNG4:    // 0x00050054 div nonLight gouraud quad 
//    case  GsUDNGT4:   // 0x00050055 div nonLight tex gouraud quad 
//      break;
//
//    case  GsUSCAL:    // 0x01000000 shared calculate vertex and normal 
//    case  GsUSG3:     // 0x0100000c shared gour triangle 
//    case  GsUSGT3:    // 0x0100000d shared texture gour triangle 
//    case  GsUSG4:     // 0x01000014 shared gour quad 
//    case  GsUSGT4:    // 0x01000015 shared texture gour quad 
//      break;
//
//    case  GsUSTGT3:   // 0x0100020d shared tile texture gour triangle 
//    case  GsUSTGT4:   // 0x01000215 shared tile texture gour quad 
//      break;
//
//    case  GsUSFG3:    // 0x0102000c shared fog gour triangle 
//    case  GsUSFGT3:   // 0x0102000d shared fog texture gour triangle 
//    case  GsUSFG4:    // 0x01020014 shared fog gour quad 
//    case  GsUSFGT4:   // 0x01020015 shared fog texture gour quad 
//      break;
//    
//    case  GsUSNF3:    // 0x01040048 shared nonLight flat tri 
//    case  GsUSNFT3:   // 0x01040049 shared nonLight texture flat tri 
//    case  GsUSNG3:    // 0x0104004c shared nonLight gour tri 
//    case  GsUSNGT3:   // 0x0104004d shared nonLight texture gour tri 
//    case  GsUSNF4:    // 0x01040050 shared nonLight flat quad 
//    case  GsUSNFT4:   // 0x01040051 shared nonLight texture flat quad 
//    case  GsUSNG4:    // 0x01040054 shared nonLight gour quad 
//    case  GsUSNGT4:   // 0x01040055 shared nonLight texture gour quad 
//      break;
//
//    case  GsUMF3:     // 0x00000018 mesh flat tri 
//    case  GsUMFT3:    // 0x00000019 mesh texture flat tri 
//    case  GsUMG3:     // 0x0000001c mesh gour triangle 
//    case  GsUMGT3:    // 0x0000001d mesh texture gour triangle 
//    case  GsUMNF3:    // 0x00040058 mesh nonLight flat tri 
//    case  GsUMNFT3:   // 0x00040059 mesh nonLight tex flat tri 
//    case  GsUMNG3:    // 0x0004005c mesh nonLight gour triangle 
//    case  GsUMNGT3:   // 0x0004005d mesh nonLight tex gour tri 
//      break;
//
//    case  GsUTFT3:    // 0x00000209 tile texture flat triangle
//    case  GsUTGT3:    // 0x0000020d tile texture gour triangle 
//    case  GsUTFT4:    // 0x00000211 tile texture flat quad 
//    case  GsUTGT4:    // 0x00000215 tile texture gour quad 
//      break;
//
//    case  GsUPNF3:    // 0x00040148 preset nonLight flat triangle 
//    case  GsUPNFT3:   // 0x00040149 preset nonLight tex flat triangle 
//    case  GsUPNG3:    // 0x0004014c preset nonLight gouraud triangle 
//    case  GsUPNGT3:   // 0x0004014d preset nonLight tex gour triangle 
//    case  GsUPNF4:    // 0x00040150 preset nonLight flat quad 
//    case  GsUPNFT4:   // 0x00040151 preset nonLight tex flat quad 
//    case  GsUPNG4:    // 0x00040154 preset nonLight gouraud quad 
//    case  GsUPNGT4:   // 0x00040155 preset nonLight tex gour quad 
//      break;
//
//    case  GsUSTPF3:   // 0x00200008 semi-trans flat triangle 
//    case  GsUSTPFT3:  // 0x00200009 semi-trans texture flat triangle 
//    case  GsUSTPG3:   // 0x0020000c semi-trans gour triangle 
//    case  GsUSTPGT3:  // 0x0020000d semi-trans texture gour triangle 
//    case  GsUSTPF4:   // 0x00200010 semi-trans flat quad 
//    case  GsUSTPFT4:  // 0x00200011 semi-trans texture flat quad 
//    case  GsUSTPG4:   // 0x00200014 semi-trans gour quad 
//    case  GsUSTPGT4:  // 0x00200015 semi-trans texture gour quad 
//    case  GsUSTPSG3:  // 0x0120000c semi-trans shared gour tri 
//    case  GsUSTPSGT3: // 0x0120000d semi-trans shared tex gour tri 
//    case  GsUSTPSG4:  // 0x01200014 semi-trans shared gour quad 
//    case  GsUSTPSGT4: // 0x01200015 semi-trans shared tex gour quad 
//      break;
//
//    case  GsUSTPNF3:    // 0x00240048 semi-trans nonLight flat tri 
//    case  GsUSTPNFT3:   // 0x00240049 semi-trans nonLight tex flat tri 
//    case  GsUSTPNG3:    // 0x0024004c semi-trans nonLight gour tri 
//    case  GsUSTPNGT3:   // 0x0024004d semi-trans nonLight tex gour tri 
//    case  GsUSTPNF4:    // 0x00240050 semi-trans nonLight flat quad 
//    case  GsUSTPNFT4:   // 0x00240051 semi-trans nonLight tex flat quad 
//    case  GsUSTPNG4:    // 0x00240054 semi-trans nonLight gour quad 
//    case  GsUSTPNGT4:   // 0x00240055 semi-trans nonLight tex gour quad 
//      break;
//
//    case  GsUSTPSNF3:   // 0x01240048 stp shared nonLight flat tri 
//    case  GsUSTPSNFT3:  // 0x01240049 stp shared nonLight tex flat tri 
//    case  GsUSTPSNG3:   // 0x0124004c stp shared nonLight gour tri 
//    case  GsUSTPSNGT3:  // 0x0124004d stp shared nonLight tex gour tri 
//    case  GsUSTPSNF4:   // 0x01240050 stp shared nonLight flat quad 
//    case  GsUSTPSNFT4:  // 0x01240051 stp shared nonLight tex flat quad 
//    case  GsUSTPSNG4:   // 0x01240054 stp shared nonLight gour quad 
//    case  GsUSTPSNGT4:  // 0x01240055 stp shared nonLight tex gour quad 
//      break;
//
//    case  GsUADF3:    // 0x00080008 active-div flat triangle 
//    case  GsUADFT3:   // 0x00080009 active-div texture flat triangle 
//    case  GsUADG3:    // 0x0008000c active-div gour triangle 
//    case  GsUADGT3:   // 0x0008000d active-div texture gour triangle 
//    case  GsUADF4:    // 0x00080010 active-div flat quad 
//    case  GsUADFT4:   // 0x00080011 active-div texture flat quad 
//    case  GsUADG4:    // 0x00080014 active-div gour quad 
//    case  GsUADGT4:   // 0x00080015 active-div texture gour quad 
//      break;
//
//    case  GsUADFF3:   // 0x000a0008 active-div fog flat tri 
//    case  GsUADFFT3:  // 0x000a0009 active-div fog texture flat tri 
//    case  GsUADFG3:   // 0x000a000c active-div fog gour tri 
//    case  GsUADFGT3:  // 0x000a000d active-div fog texture gour tri 
//    case  GsUADFF4:   // 0x000a0010 active-div fog flat quad 
//    case  GsUADFFT4:  // 0x000a0011 active-div fog texture flat quad 
//    case  GsUADFG4:   // 0x000a0014 active-div fog gour quad 
//    case  GsUADFGT4:  // 0x000a0015 active-div fog texture gour quad 
//      break;
//
//    case  GsUADNF3:   // 0x000c0048 active-div nonLight flat tri 
//    case  GsUADNFT3:  // 0x000c0049 active-div nonLight tex flat tri 
//    case  GsUADNG3:   // 0x000c004c active-div nonLight gour tri 
//    case  GsUADNGT3:  // 0x000c004d active-div nonLight tex gour tri 
//    case  GsUADNF4:   // 0x000c0050 active-div nonLight flat quad 
//    case  GsUADNFT4:  // 0x000c0051 active-div nonLight tex flat quad 
//    case  GsUADNG4:   // 0x000c0054 active-div nonLight gour quad 
//    case  GsUADNGT4:  // 0x000c0055 active-div nonLight tex gour quad 
//      break;
//
//    case  GsUBF3:     // 0x00100008 back-f flat tri 
//    case  GsUBFT3:    // 0x00100009 back-f tex flat tri 
//    case  GsUBG3:     // 0x0010000c back-f gour tri 
//    case  GsUBGT3:    // 0x0010000d back-f tex gour tri 
//    case  GsUBF4:     // 0x00100010 back-f flat quad 
//    case  GsUBFT4:    // 0x00100011 back-f tex flat quad 
//    case  GsUBG4:     // 0x00100014 back-f gour quad 
//    case  GsUBGT4:    // 0x00100015 back-f tex gour quad 
//      break;
//
//    case  GsUBCF3:    // 0x0010000a back-f colored flat tri 
//    case  GsUBCFT3:   // 0x0010000b back-f colored tex flat tri 
//    case  GsUBCG3:    // 0x0010000e back-f colored gour tri 
//    case  GsUBCGT3:   // 0x0010000f back-f colored tex gour tri 
//    case  GsUBCF4:    // 0x00100012 back-f colored flat quad 
//    case  GsUBCFT4:   // 0x00100013 back-f colored tex flat quad 
//    case  GsUBCG4:    // 0x00100016 back-f colored gour quad 
//    case  GsUBCGT4:   // 0x00100017 back-f colored tex gour quad 
//      break;
//
//    case  GsUBSTPF3:  // 0x00300008 back-f semi-trans flat tri 
//    case  GsUBSTPFT3: // 0x00300009 back-f semi-trans tex flat tri 
//    case  GsUBSTPG3:  // 0x0030000c back-f semi-trans gour tri 
//    case  GsUBSTPGT3: // 0x0030000d back-f semi-trans tex gour tri 
//    case  GsUBSTPF4:  // 0x00300010 back-f semi-trans flat quad 
//    case  GsUBSTPFT4: // 0x00300011 back-f semi-trans tex flat quad 
//    case  GsUBSTPG4:  // 0x00300014 back-f semi-trans gour quad 
//    case  GsUBSTPGT4: // 0x00300015 back-f semi-trans tex gour quad 
//      break;
//
//    case  GsUBNF3:    // 0x00140048 back-f noLgt flat tri 
//    case  GsUBNFT3:   // 0x00140049 back-f noLgt tex flat tri 
//    case  GsUBNG3:    // 0x0014004c back-f noLgt gouraud tri 
//    case  GsUBNGT3:   // 0x0014004d back-f noLgt tex gouraud tri 
//    case  GsUBNF4:    // 0x00140050 back-f noLgt flat quad 
//    case  GsUBNFT4:   // 0x00140051 back-f noLgt tex flat quad 
//    case  GsUBNG4:    // 0x00140054 back-f noLgt gouraud quad 
//    case  GsUBNGT4:   // 0x00140055 back-f noLgt tex gouraud quad 
//      break;
//
//    case  GsUBSTPNF3:   // 0x00340048 back-f stp noLgt flat tri 
//    case  GsUBSTPNFT3:  // 0x00340049 back-f stp noLgt tex flat tri 
//    case  GsUBSTPNG3:   // 0x0034004c back-f stp noLgt gour tri 
//    case  GsUBSTPNGT3:  // 0x0034004d back-f stp noLgt tex gour tri 
//    case  GsUBSTPNF4:   // 0x00340050 back-f stp noLgt flat quad 
//    case  GsUBSTPNFT4:  // 0x00340051 back-f stp noLgt tex flat quad 
//    case  GsUBSTPNG4:   // 0x00340054 back-f stp noLgt gour quad 
//    case  GsUBSTPNGT4:  // 0x00340055 back-f stp noLgt tex gour quad 
//      break;
//
//    case  GsUBSNF3:   // 0x01140048 back-f shrd noLgt flat tri 
//    case  GsUBSNFT3:  // 0x01140049 back-f shrd noLgt tex flat tri 
//    case  GsUBSNG3:   // 0x0114004c back-f shrd noLgt gour tri 
//    case  GsUBSNGT3:  // 0x0114004d back-f shrd noLgt tex gour tri 
//    case  GsUBSNF4:   // 0x01140050 back-f shrd noLgt flat quad 
//    case  GsUBSNFT4:  // 0x01140051 back-f shrd noLgt tex flat quad 
//    case  GsUBSNG4:   // 0x01140054 back-f shrd noLgt gour quad 
//    case  GsUBSNGT4:  // 0x01140055 back-f shrd noLgt tex gour quad 
//      break;
//
//    case  GsUBSTPSNF3:  // 0x01340048 back-f stp shrd noLgt flat tri 
//    case  GsUBSTPSNFT3: // 0x01340049 back-f stp shrd noLgt tex flat tri 
//    case  GsUBSTPSNG3:  // 0x0134004c back-f stp shrd noLgt gour tri 
//    case  GsUBSTPSNGT3: // 0x0134004d back-f stp shrd noLgt tex gour tri 
//    case  GsUBSTPSNF4:  // 0x01340050 back-f stp shrd noLgt flat quad 
//    case  GsUBSTPSNFT4: // 0x01340051 back-f stp shrd noLgt tex flat quad
//    case  GsUBSTPSNG4:  // 0x01340054 back-f stp shrd noLgt gour quad 
//    case  GsUBSTPSNGT4: // 0x01340055 back-f stp shrd noLgt tex gour quad
//      break;
//
//    case  GsUIMG0:    // 0x02000000 image data with no-clut 
//      break;

    case  GsUIMG1:    // 0x02000001 image data with clut 
      HMD_LoadTIM( type, header );
      break;
  
//    case  GsUGNDF:    // 0x05000000 ground flat 
//    case  GsUGNDFT:   // 0x05000001 ground flat texture 
//      break;
//
//    case  GsUSCAL2:   // 0x06000100 envmap shared calculate 
//    case  GsUE1G3:    // 0x0600100c envmap 1D gour tri 
//    case  GsUE1G4:    // 0x06001014 envmap 1D gour quad 
//    case  GsUE1SG3:   // 0x0600110c envmap 1D shared gour tri 
//    case  GsUE1SG4:   // 0x06001114 envmap 1D shared gour quad 
//    case  GsUE2LG3:   // 0x0600200c envmap 2D reflect gour tri 
//    case  GsUE2LG4:   // 0x06002014 envmap 2D reflect gour quad 
//    case  GsUE2RG3:   // 0x0600300c envmap 2D refract gour tri 
//    case  GsUE2RG4:   // 0x06003014 envmap 2D refract gour quad 
//    case  GsUE2RLG3:  // 0x0600400c envmap 2D both gour tri 
//    case  GsUE2RLG4:  // 0x06004014 envmap 2D both gour quad 
//    case  GsUE2OLG3:  // 0x0600500c envmap 2D org+reflect gour tri 
//    case  GsUE2OLG4:  // 0x06005014 envmap 2D org+reflect gour quad 
//      break;

    case  GsVtxMIMe:  // 0x04010020 Vertex-MIMe 
      CurrentBlockBase = (ULONG *)*( header + 4 );
    
      for( i = 0; i < number; ++i )
      {
        CurrentBlock = CurrentBlockBase + *( type + 2 + i );
        NumberOfDiffBlocks = HighWord( CurrentBlock[ 0 ] );

        for( j = 0; j < NumberOfDiffBlocks; ++j )
        {
          CurrentDiffBlock = AddVertexDiffBlock();
          CurrentDiffBlock->HMD_VertexDiffBlock = CurrentBlockBase + CurrentBlock[ 2 + j ];
        }
      }
      break;

    case  GsNrmMIMe:    // 0x04010021 Normal-MIMe 
      CurrentBlockBase = (ULONG *)*(header + 4 );

      for( i = 0; i < number; ++i )
      {
        CurrentBlock = CurrentBlockBase + *( type + 2 + i );
        NumberOfDiffBlocks = LowWord( CurrentBlock[ 0 ] );

        for( j = 0; j < NumberOfDiffBlocks; ++j )
        {
          CurrentDiffBlock = AddNormalDiffBlock();
          CurrentDiffBlock->HMD_NormalDiffBlock = CurrentBlockBase + CurrentBlock[ 2 + j ];
        }
      }
      break;

//    case  GsRstVtxMIMe:     // 0x04010028 Reset-Vertex-MIMe 
//    case  GsRstNrmMIMe:     // 0x04010029 Reset-Normal-MIMe 
//    case  GsJntAxesMIMe:    // 0x04010010 Joint-Axes-MIMe 
//    case  GsRstJntAxesMIMe: // 0x04010018 Reset-Joint-Axes-MIMe 
//    case  GsJntRPYMIMe:     // 0x04010011 Joint-RPY-MIMe 
//    case  GsRstJntRPYMIMe:  // 0x04010019 Reset-Joint-RPY-MIMe 
//      break;

    default:
      //Oops, return an error code.
      TRACE("Rejecting %d data items of type %p\n", number, foo);
      break;
  }

  return( size );
}


void meshx::HMD_MapUnit( ULONG *unit, ULONG *origin )
{
  ULONG *current;
  ULONG *primHeader;
  int increment;
  int nTypes;
  int i;

  //Set the current pointer and increment
  current = unit;

  if( *current != 0xFFFFFFFF )
  {
    *current = (ULONG)( origin + *current );
    //Recurse down
    HMD_MapUnit( (ULONG *)*current, origin );
  }

  ++current;

  //Set the header point.
  *current = (ULONG)( origin + *current );
  primHeader = (ULONG *)*current;

  //Grab the number of types
  ++current;
  nTypes = *current;

  if( nTypes & 0x80000000 )
  {
    nTypes &= 0x7FFFFFFF;
    //We haven't mapped this one yet
    //Forward the pointer so it points at the first type
    ++current;

    for( i = 0; i < nTypes; ++i )
    {
      increment = HMD_MapType( current, primHeader );
      current += increment;
    }
  }
}

void meshx::HMD_Map( HMD_Header *data )
{
  ULONG i;
  ULONG j;
  ULONG nBlocks;
  ULONG nPointers;
  ULONG *origin;
  ULONG *current;
  //We've already mapped this data.
  if( data->map_flag != 0 )
    return;

  data->map_flag = 1;
  
  //Cast the silly thing so we get the origin in word width
  //This will make sense in a bit.
  origin = (ULONG *)data;

  //Now, map the pointers.
  //Do the prim header section.
  data->primitive_header_section_point = origin + 
                      (long)(data->primitive_header_section_point);

  current = data->primitive_header_section_point;

  //The number of primitive headers
  nBlocks = *current;

  //Advance the pointer.
  ++current;

  for( i = 0; i < nBlocks; ++i )
  {
    //The number of words in this block that must be converted to
    //pointers.
    nPointers = *current;
    ++current;
    for( j = 0; j < nPointers; ++j )
    {
      //Check to see if we've been mapped (if the high bit is set, we've not
      //been mapped
      if( *current & 0x80000000 )
        *current = (ULONG)( origin + ( *current & 0x7FFFFFFF ) );
      ++current;
    }
  }

  //The number of Primitive sets.
  nBlocks = data->num_blocks;

  //Set the current pointer to the end of the main data block
  current = (ULONG *)( data + 1 );

  for( i = 0; i < nBlocks; ++i )
  {
    if( *current != NULL )
    {
      *current = (ULONG)( origin + *current );
      HMD_MapUnit( (ULONG *)*current, origin );
    }
    ++current;
  }
}

int g_nTCount = 0;

int HMD_LoadTIM( ULONG *type, ULONG *header )
{
  int             i, j;
  int             number;
  USHORT          myRGB;
  ULONG           tpage, clut;
  ULONG           x, y;
  ULONG           slot;
  ULONG*          clutTop;
  ULONG*          imageTop;
  ULONG*          myType;
  int             width, height;
  char*           bits;
  HDC             myDC;
  HBITMAP         myBitmap;
  HMD_TimTracker* newTracker;
  Surface*        asurface;

  if (!g_bHMD_TrackerInitialized)
  {
    ZeroMemory(HMD_Tracker, sizeof(HMD_TimTracker*) * 16 * 32);
    g_bHMD_TrackerInitialized = TRUE;
  }

  if( myBitmapInfo == NULL )
  {
    myBitmapInfo = (BITMAPINFO *)HMD_MemoryMalloc( sizeof( BITMAPINFO ) + ( 256 * sizeof( RGBQUAD ) ) );
  }

  number = *( type + 1 ) >> 16;
  number &= 0x00007FFF;

  myType = type;
  for( j = 0; j < number; ++j )
  {
    imageTop = ( (ULONG *)*( header + 1 ) ) + myType[ 4 ];
    clutTop = ( (ULONG *)*( header + 2 ) ) + myType[ 7 ];

    width   = LOWORD(myType[3]) * 2; //in 16 bit pixels...2 8 bit pixels per.
    height  = (int)HIWORD(myType[3]);

    x       = LOWORD(myType[2]);
    y       = HIWORD(myType[2]);
    tpage   = getTPage( 1, 0, x, y );
    slot    = tpage & 0x00000001F; //00->31
    clut    = getClut(LOWORD(myType[5]), HIWORD(myType[5]));

    //Have we already loaded a map at this place?
    if( CompatibleTracker2( x, y, width, height, tpage ) != NULL )
    {
      myType += 6; //Advance type 6 dwords.
      continue;
    }

    myBitmapInfo->bmiHeader.biSize          = sizeof( BITMAPINFOHEADER );
    myBitmapInfo->bmiHeader.biWidth         = width;
    myBitmapInfo->bmiHeader.biHeight        = -height;
    myBitmapInfo->bmiHeader.biPlanes        = 1;
    myBitmapInfo->bmiHeader.biBitCount      = 8;
    myBitmapInfo->bmiHeader.biCompression   = 0;
    myBitmapInfo->bmiHeader.biSizeImage     = 0;
    myBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
    myBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
    myBitmapInfo->bmiHeader.biClrUsed       = 0;
    myBitmapInfo->bmiHeader.biClrImportant  = 0;

    for( i = 0; i < 256; ++i )
    {
      if( i % 2 )
        myRGB = (USHORT)( ( clutTop[ i / 2 ] & 0xFFFF0000 ) >> 16 );
      else
        myRGB = (USHORT)( clutTop[ i / 2 ] & 0x0000FFFF );

      myBitmapInfo->bmiColors[ i ].rgbRed = (char)( (float)( myRGB & 0x001F ) / 31.0f * 255.0f );
      myBitmapInfo->bmiColors[ i ].rgbGreen = (char)( (float)( ( myRGB & 0x03E0 ) >> 5 ) / 31.0f * 255.0f );
      myBitmapInfo->bmiColors[ i ].rgbBlue = (char)( (float)( ( myRGB & 0x7C00 ) >> 10 ) / 31.0f * 255.0f );
      myBitmapInfo->bmiColors[ i ].rgbReserved = (char)0;
    }

    myDC = ::CreateCompatibleDC( NULL );

    myBitmap = ::CreateDIBSection(myDC, myBitmapInfo, DIB_RGB_COLORS, 
                                (void **)&bits, NULL, NULL);

    ::GdiFlush();

    CopyMemory(bits, imageTop, width * height);

    // Create ourselves a new texture
    asurface = new Surface(myBitmap, TRUE);
TRACE("Loading Texture %02d at x: %d, y: %d, w: %d, h: %d, tpage: %d, timbank: %d\n",
      g_nTCount++, x, y, width, height, tpage, HMD_CurrentTimBank);
//char szFile[MAX_PATH];
//sprintf(szFile, ".\\Textures\\Texture%02d.bmp", g_nTCount++);
//asurface->SaveToBmpFile(szFile);

    //pmode is 8bit.

    // Release our GDI resources
    ::DeleteObject(myBitmap);
    ::DeleteDC(myDC);
    myBitmap  = NULL;
    myDC      = NULL;
    
    newTracker = (HMD_TimTracker *)HMD_MemoryMalloc( sizeof( HMD_TimTracker ) );
    if( newTracker == NULL )
      return TRUE;

    newTracker->clut      = clut;
    newTracker->tpage     = tpage;
    newTracker->mySurface = asurface;
    newTracker->x         = x;
    newTracker->y         = y;
    newTracker->w         = width;
    newTracker->h         = height;
    newTracker->next      = NULL;

    if( HMD_Tracker[ HMD_CurrentTimBank ][ slot ] == NULL )
      HMD_Tracker[ HMD_CurrentTimBank ][ slot ] = newTracker;
    else
    {
      newTracker->next = HMD_Tracker[ HMD_CurrentTimBank ][ slot ];
      HMD_Tracker[ HMD_CurrentTimBank ][ slot ] = newTracker;
    }

    myType += 6; //Advance type 6 dwords.
  }

  HMD_MemoryFree( myBitmapInfo );
  myBitmapInfo = NULL;
  return FALSE;
}

int HMD_LoadTIM( void *timPointer )
{
  USHORT          myRGB;
  ULONG           tpage, clut;
  ULONG           x, y;
  ULONG           slot;
  ULONG           pmode;
  ULONG*          clutTop;
  ULONG*          imageTop;
  ULONG*          tempBuff;
  int             nextRead;
  int             width, height;
  HBITMAP         myBitmap;
  HDC             myDC;
  char*           bits;
  HMD_TimTracker* newTracker;
  Surface*        asurface;

  if (!g_bHMD_TrackerInitialized)
  {
    ZeroMemory(HMD_Tracker, sizeof(HMD_TimTracker*) * 16 * 32);
    g_bHMD_TrackerInitialized = TRUE;
  }

  if( myBitmapInfo == NULL )
  {
    myBitmapInfo = (BITMAPINFO *)malloc( sizeof( BITMAPINFO ) + ( 256 * sizeof( RGBQUAD ) ) );
  }

  tempBuff = (ULONG *)timPointer;

  pmode = tempBuff[ 1 ] & 0x00000007;

  if( pmode != 1 )
  {
    free( tempBuff );
    return TRUE;
  }

  if( tempBuff[ 1 ] & 0x00000008 )
  {
    //We have a clut
  }
  else
  {
    //We're not 8 bit
    free( tempBuff );
    return TRUE;
  }

  clutTop = &tempBuff[ 5 ];

  nextRead = 128 + 5; //256 entries, 16 bits each
  
  nextRead += 1; //second byte in the structure
  y       = ( tempBuff[ nextRead ] & 0xFFFF0000 ) >> 16;
  x       = ( tempBuff[ nextRead ] & 0x0000FFFF );
  clut    = getClut(tempBuff[5] & 0x0000FFFF, (tempBuff[5] & 0xFFFF0000) >> 16);

  width   = ( tempBuff[ nextRead + 1 ] & 0x0000FFFF ) * 2;
  height  = ( tempBuff[ nextRead + 1 ] & 0xFFFF0000 ) >> 16;
  tpage   = getTPage( 1, 0, x, y );

  //Have we already loaded a map at this place?
  if( CompatibleTracker2( x, y, width, height, tpage ) != NULL )
  {
    return TRUE;
  }

  imageTop = &tempBuff[ nextRead + 2 ];

  myBitmapInfo->bmiHeader.biSize          = sizeof( BITMAPINFOHEADER );
  myBitmapInfo->bmiHeader.biWidth         = width;
  myBitmapInfo->bmiHeader.biHeight        = -height;
  myBitmapInfo->bmiHeader.biPlanes        = 1;
  myBitmapInfo->bmiHeader.biBitCount      = 8;
  myBitmapInfo->bmiHeader.biCompression   = 0;
  myBitmapInfo->bmiHeader.biSizeImage     = 0;
  myBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
  myBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
  myBitmapInfo->bmiHeader.biClrUsed       = 0;
  myBitmapInfo->bmiHeader.biClrImportant  = 0;

  for (int i = 0; i < 256; ++i )
  {
    if( i % 2 )
      myRGB = (USHORT)( ( clutTop[ i / 2 ] & 0xFFFF0000 ) >> 16 );
    else
      myRGB = (USHORT)( clutTop[ i / 2 ] & 0x0000FFFF );

    myBitmapInfo->bmiColors[ i ].rgbRed = (char)( (float)( myRGB & 0x001F ) / 31.0f * 255.0f );
    myBitmapInfo->bmiColors[ i ].rgbGreen = (char)( (float)( ( myRGB & 0x03E0 ) >> 5 ) / 31.0f * 255.0f );
    myBitmapInfo->bmiColors[ i ].rgbBlue = (char)( (float)( ( myRGB & 0x7C00 ) >> 10 ) / 31.0f * 255.0f );
    myBitmapInfo->bmiColors[ i ].rgbReserved = (char)0;
  }

  myDC = ::CreateCompatibleDC( NULL );

  myBitmap = ::CreateDIBSection(myDC, myBitmapInfo, DIB_RGB_COLORS,
                                (void **)&bits, NULL, NULL);
  ASSERT(NULL != myBitmap);

  ::GdiFlush();

  CopyMemory(bits, imageTop, width * height);

  // Create a system memory surface
  asurface = new Surface(myBitmap, TRUE);

  // Release our GDI resources
  ::DeleteObject(myBitmap);
  ::DeleteDC(myDC);
  myBitmap  = NULL;
  myDC      = NULL;

  tpage = getTPage( 1, 0, x, y );

  slot = tpage & 0x00000001F; //00->31
  
  newTracker = (HMD_TimTracker *)malloc( sizeof( HMD_TimTracker ) );
  if( newTracker == NULL )
  {
    return TRUE;
  }

  newTracker->clut      = clut;
  newTracker->tpage     = tpage;
  newTracker->mySurface = asurface;
  newTracker->x         = x;
  newTracker->y         = y;
  newTracker->w         = width;
  newTracker->h         = height;
  newTracker->next      = NULL;

  if( HMD_Tracker[ HMD_CurrentTimBank ][ slot ] == NULL )
  {
    HMD_Tracker[ HMD_CurrentTimBank ][ slot ] = newTracker;
  }
  else
  {
    newTracker->next = HMD_Tracker[ HMD_CurrentTimBank ][ slot ];
    HMD_Tracker[ HMD_CurrentTimBank ][ slot ] = newTracker;
  }

  return FALSE;
}


/******************************************************************
 * Returns a timTracker if it exists, null otherwise.
 ******************************************************************/
HMD_TimTracker *CompatibleTracker( int x, int y, ULONG tpage )
{
  int myX, myY;
  HMD_TimTracker* pTracker = HMD_Tracker[HMD_CurrentTimBank][int(tpage & 0x1F)];

  while (pTracker)
  {
    // Make sure the coords are masked properly for 16bit pixels
    // Left shifting by one is the same as multiplying by 2.
    // (Need to multipy by two for 8 bit textures)
    myX = (pTracker->x & 63) << 1; 
    myY = pTracker->y & 255;

    if( tpage == pTracker->tpage )
    {
      if (myX <= x && x <= myX + pTracker->w &&
          myY <= y && y <= myY + pTracker->h)
      {
        return pTracker;
      }
    }
    pTracker = pTracker->next;
  }

  return(NULL);
}


HMD_TimTracker *CompatibleTracker2( int x, int y, int w, int h, ULONG tpage )
{
  HMD_TimTracker* pTracker = HMD_Tracker[HMD_CurrentTimBank][int(tpage & 0x1F)];

  while (pTracker)
  {
    if (tpage == pTracker->tpage)
    {
      if (x == pTracker->x && y == pTracker->y && 
          h == pTracker->h && w == pTracker->w)
      {
        return pTracker;
      }
    }
    pTracker = pTracker->next;
  }

  return(NULL);
}


meshx::meshx( void )
{
  m_nType                 = 4;
  m_nSize                 = 0;
  m_nFaces                = 0;
  m_nGroups               = 0;
  m_nVertices             = 0;
  m_nSubVertices          = 0;
  m_nSubVerticesVtx       = 0;
  m_nSubVerticesNrm       = 0;
  m_nCopy                 = 0;
  m_nLinear               = 0;
  m_pGroups               = NULL;
  m_pVertices             = NULL;
  m_pFaces                = NULL;
  m_pNewVertices          = NULL;
  m_ppSubVertices         = NULL;
  m_pDiffBlocks           = NULL;
  m_pInterpolationBuffer  = NULL;
}


meshx::meshx( HMD_Header *header )
{
  m_nType                 = 4;
  m_nSize                 = 0;
  m_nFaces                = 0;
  m_nGroups               = 0;
  m_nVertices             = 0;
  m_nSubVertices          = 0;
  m_nSubVerticesVtx       = 0;
  m_nSubVerticesNrm       = 0;
  m_nCopy                 = 0;
  m_nLinear               = 0;
  m_pGroups               = NULL;
  m_pVertices             = NULL;
  m_pFaces                = NULL;
  m_pNewVertices          = NULL;
  m_ppSubVertices         = NULL;
  m_pInterpolationBuffer  = NULL;
  m_pDiffBlocks           = NULL;
  HMD_Map( header );
  Linearize();
}

meshx::~meshx( void )
{
  NewGroup*  myGroup;
  NewGroup*  oldGroup;
  NewVertex* myVertex;
  NewVertex* oldVertex;
  NewVertex* mySibling;
  NewVertex* oldSibling;
  NewFace*   myFace;
  NewFace*   oldFace;
  DiffBlock* myDBlock;
  DiffBlock* oldDBlock;

  //All objects will have an interpolation buffer of their own,
  //So if there is one, it can be destroyed without thinking twice.
  if( m_pInterpolationBuffer )
    HMD_MemoryFree( m_pInterpolationBuffer );

  m_pInterpolationBuffer = NULL;

  //We have a local copy of the vertices.
  if( ( m_nCopy & HMD_COPYVERTICES ) || !m_nCopy )
  {
    if( m_nSubVertices > 0 )
    {
      if( m_ppSubVertices )
      {
        for (int i = 0; i <= m_nSubVertices; ++i)
        {
          if( m_ppSubVertices[ i ] )
            HMD_MemoryFree( m_ppSubVertices[ i ] );
        }
        HMD_MemoryFree( m_ppSubVertices );
        m_ppSubVertices = NULL;
      }
    }
    else
    {
      if( m_pVertices )
        HMD_MemoryFree( m_pVertices );
    }
  }
  m_pVertices    = NULL;
  m_ppSubVertices = NULL;

  if( ( m_nCopy & HMD_COPYGROUPS ) || !m_nCopy )
  {
    if( !m_nCopy )
    {
      //We're the original, therefore our groups are not contiguous.
      //Well, they might be, but they weren't allocated in one huge block
      myGroup = m_pGroups;
      while( myGroup )
      {
        //Just in case we're destroying a work in progress.
        if( myGroup->pFaces )
        {
          myFace = myGroup->pFaces;
          while( myFace )
          {
            oldFace = myFace;
            myFace  = myFace->next;
            HMD_MemoryFree( oldFace );
          }
        }
        oldGroup  = myGroup;
        myGroup   = myGroup->pNext;
        HMD_MemoryFree( oldGroup );
      }
    }
    else
    {
      //We're a copy, our groups are contiguous and the
      //faces have already been taken care of.
      if( m_pGroups )
        HMD_MemoryFree( m_pGroups );
    }
  }

  m_pGroups = NULL;

  if( ( m_nCopy & HMD_COPYFACES ) || !m_nCopy )
  {
    //We have our own local copy of the faces.
    if( m_pFaces )
      HMD_MemoryFree( m_pFaces );
  }

  if( ( m_nCopy & HMD_COPYBOXES ) || !m_nCopy )
  {
    if( m_pTweenBoxes )
      HMD_MemoryFree( m_pTweenBoxes );
    m_pTweenBoxes = NULL;
  }

  m_pFaces = NULL;

  //Here we take care of the rest of everything, in case we
  //Weren't finished building the object when we deleted it.
  myVertex = m_pNewVertices;
  while( myVertex )
  {
    mySibling = myVertex->sibling;
    while( mySibling )
    {
      oldSibling  = mySibling;
      mySibling   = mySibling->next;
      HMD_MemoryFree( oldSibling );
    }
    oldVertex = myVertex;
    myVertex  = myVertex->next;
    HMD_MemoryFree( oldVertex );
  }

  myDBlock = m_pDiffBlocks;
  while( myDBlock )
  {
    oldDBlock = myDBlock;
    myDBlock  = myDBlock->next;
    HMD_MemoryFree( oldDBlock );
  }
}

NewGroup *meshx::AddGroup( void )
{
  NewGroup *myGroup;
  NewGroup *aGroup;

  if( m_nLinear ) return NULL;

  ++m_nGroups;

  myGroup = (NewGroup *)HMD_MemoryMalloc( sizeof( NewGroup ) );

  if( myGroup == NULL )
  {
    return NULL;
  }

  myGroup->texture    = NULL;
  myGroup->gouraud    = 0;
  myGroup->number     = 0;
  myGroup->pNext       = NULL;
  myGroup->pFaces     = NULL;
  myGroup->myMaterial = NULL;

  if( m_pGroups == NULL )
  {
    m_pGroups = myGroup;
  }
  else
  {
    aGroup = m_pGroups;
    while( aGroup->pNext != NULL )
    {
      aGroup = aGroup->pNext;
    }
    aGroup->pNext = myGroup;
  }

  return( myGroup );
}


DiffBlock *meshx::AddVertexDiffBlock( void )
{
  DiffBlock *myDiffBlock;

  if( m_nLinear ) return NULL;

  if( m_nSubVerticesVtx < m_nSubVerticesNrm )
  {
    //We'll use a previously allocated diff block
    myDiffBlock = m_pDiffBlocks;
    for (int i = 0; i < m_nSubVerticesVtx; ++i)
      myDiffBlock = myDiffBlock->next;
    ++m_nSubVerticesVtx;
  }
  else
  {
    //We need a new diff block
    myDiffBlock = AddDiffBlock();
    if( myDiffBlock )
      ++m_nSubVerticesVtx;
  }

  return( myDiffBlock );
}

DiffBlock *meshx::AddNormalDiffBlock( void )
{
  DiffBlock *myDiffBlock;

  if( m_nLinear ) return NULL;

  if( m_nSubVerticesNrm < m_nSubVerticesVtx )
  {
    //We'll use a previously allocated diff block
    myDiffBlock = m_pDiffBlocks;
    for (int i = 0; i < m_nSubVerticesNrm; ++i)
      myDiffBlock = myDiffBlock->next;
    ++m_nSubVerticesNrm;
  }
  else
  {
    //We need a new diff block
    myDiffBlock = AddDiffBlock();
    if( myDiffBlock )
      ++m_nSubVerticesNrm;
  }

  return( myDiffBlock );
}


DiffBlock *meshx::AddDiffBlock( void )
{
  DiffBlock *myDiffBlock;
  DiffBlock *aDiffBlock;

  if( m_nLinear ) return NULL;

  myDiffBlock = (DiffBlock *)HMD_MemoryMalloc( sizeof( DiffBlock ) );

  if( myDiffBlock == NULL )
  {
    return NULL;
  }

  myDiffBlock->next = NULL;
  myDiffBlock->HMD_VertexDiffBlock = NULL;
  myDiffBlock->HMD_NormalDiffBlock = NULL;

  if( m_pDiffBlocks == NULL )
  {
    m_pDiffBlocks = myDiffBlock;
  }
  else
  {
    aDiffBlock = m_pDiffBlocks;
    while( aDiffBlock->next != NULL )
      aDiffBlock = aDiffBlock->next;
    aDiffBlock->next = myDiffBlock;
  }

  ++m_nSubVertices;

  return( myDiffBlock );
}


NewVertex *meshx::AddVertex( void )
{
  NewVertex *myVertex;
  NewVertex *aVertex;

  if( m_nLinear ) return NULL;

  ++m_nVertices;

  myVertex = (NewVertex*)HMD_MemoryMalloc(sizeof(NewVertex));

  if (!myVertex)
    return NULL;

  myVertex->normal  = 0;
  myVertex->uv      = 0;
  myVertex->next    = NULL;
  myVertex->sibling = NULL;

  if( m_pNewVertices == NULL )
  {
    m_pNewVertices = myVertex;
  }
  else
  {
    aVertex = m_pNewVertices;
    while( aVertex->next != NULL )
      aVertex = aVertex->next;
    aVertex->next = myVertex;
  }

  aVertex = myVertex;

  //Add the subsequent sub-vertices (poses)
  for (int i = 0; i < m_nSubVertices ; ++i)
  {
    aVertex->sibling = (NewVertex *)HMD_MemoryMalloc( sizeof( NewVertex ) );
    if( aVertex->sibling == NULL )
      return NULL;
    
    aVertex->sibling->sibling = NULL;
    aVertex->sibling->next    = NULL;
    aVertex->sibling->uv      = 0;
    aVertex->sibling->normal  = 0;

    aVertex = aVertex->sibling;
  }

  return( myVertex );
}


NewFace *meshx::AddFace( NewGroup *to )
{
  NewFace *myFace;
  NewFace *aFace;

  if( m_nLinear ) return NULL;

  ++m_nFaces;

  myFace = (NewFace *)HMD_MemoryMalloc( sizeof( NewFace ) );
  if( myFace == NULL )
    return NULL;

  myFace->next = NULL;

  if( to->pFaces == NULL )
  {
    to->pFaces = myFace;
  }
  else
  {
    aFace = to->pFaces;
    while( aFace->next != NULL )
      aFace = aFace->next;
    aFace->next = myFace;
  }

  return( myFace );
}

/******************************************************************
 * Returns an index of a compatible vertex. If none, returns -1;
 ******************************************************************/

int meshx::IndexOfVertex(D3DVALUE x,  D3DVALUE y,  D3DVALUE z,
                         D3DVALUE nx, D3DVALUE ny, D3DVALUE nz,
                         D3DVALUE u,  D3DVALUE v)
{
  NewVertex *myVertex;
  int     i;
  //Good enough for now.....look up normals later.
  if( m_pNewVertices == NULL )
  {
    return( -1 );
  }

  myVertex = m_pNewVertices;

  i = 0;

  while( myVertex != NULL )
  {
    if( x == myVertex->vertex.dvX &&
        y == myVertex->vertex.dvY &&
        z == myVertex->vertex.dvZ )
    {
      if( HMD_CaresAboutN( myVertex ) )
      {
        if( nx == myVertex->vertex.dvNX &&
            ny == myVertex->vertex.dvNY &&
            nz == myVertex->vertex.dvNZ )
        {
          if( HMD_CaresAboutUV( myVertex ) )
          {
            if( u == myVertex->vertex.tu &&
                v == myVertex->vertex.tv )
              return( i );
          }
          else
          {
            return( i );
          }
        }
      }
      else
      {
        if( HMD_CaresAboutUV( myVertex ) )
        {
          if( u == myVertex->vertex.tu &&
              v == myVertex->vertex.tv )
            return( i );
        }
        else
        {
          return( i );
        }
      }
    }
    ++i;
    myVertex = myVertex->next;
  }

  return( -1 );
}

int meshx::IndexOfVertex( D3DVALUE x, D3DVALUE y, D3DVALUE z,
                          D3DVALUE nx, D3DVALUE ny, D3DVALUE nz )
{
  NewVertex *myVertex;
  int     i;

  //Good enough for now.....look up normals later.
  if( m_pNewVertices == NULL )
  {
    return( -1 );
  }

  myVertex = m_pNewVertices;

  i = 0;

  while( myVertex != NULL )
  {
    if( x == myVertex->vertex.dvX &&
        y == myVertex->vertex.dvY &&
        z == myVertex->vertex.dvZ )
    {
      if( HMD_CaresAboutN( myVertex ) )
      {
        if( nx == myVertex->vertex.dvNX &&
            ny == myVertex->vertex.dvNY &&
            nz == myVertex->vertex.dvNZ )
        {
          return( i );
        }
      }
      else
      {
        return( i );
      }
    }
    ++i;
    myVertex = myVertex->next;
  }

  return( -1 );
}


int meshx::IndexOfVertex(D3DVALUE x, D3DVALUE y, D3DVALUE z, D3DVALUE u, 
                         D3DVALUE v)
{
  NewVertex *myVertex;
  int     i;
  //Good enough for now.....look up normals later.
  if( m_pNewVertices == NULL )
  {
    return( -1 );
  }

  myVertex = m_pNewVertices;

  i = 0;

  while( myVertex != NULL )
  {
    if( x == myVertex->vertex.dvX &&
        y == myVertex->vertex.dvY &&
        z == myVertex->vertex.dvZ )
    {
      if( HMD_CaresAboutUV( myVertex ) )
      {
        if( u == myVertex->vertex.tu &&
            v == myVertex->vertex.tv )
        {
          return( i );
        }
        else
        {
          return( i );
        }
      }
    }
    myVertex = myVertex->next;
    ++i;
  }

  return( -1 );
}


int meshx::IndexOfVertex( D3DVALUE x, D3DVALUE y, D3DVALUE z )
{
  NewVertex *myVertex;
  int     i;
  //Good enough for now.....look up normals later.
  if( m_pNewVertices == NULL )
  {
    return( -1 );
  }

  myVertex = m_pNewVertices;

  i = 0;

  while( myVertex != NULL )
  {
    if (x == myVertex->vertex.dvX && y == myVertex->vertex.dvY &&
        z == myVertex->vertex.dvZ)
    {
      return( i );
    }
    myVertex = myVertex->next;
    ++i;
  }
  return( -1 );
}



NewGroup *meshx::LastGroup( void )
{
  NewGroup *myGroup;

  if( m_pGroups == NULL )
    return NULL;
  
  myGroup = m_pGroups;

  while( myGroup->pNext != NULL )
    myGroup = myGroup->pNext;

  return myGroup;
}


int meshx::getType(void) const
{
  return 5;
}


void meshx::Linearize( void )
{
  int         i, j;
  void*       aBuff;
  int         counter = 0;
  NewGroup*   aGroup;
  NewVertex*  aVertex;
  NewVertex*  sibling;
  NewVertex*  oldVertex;
  NewFace*    aFace;
  NewFace*    oldFace;
  DiffBlock*  myDBlock;
  DiffBlock*  oldDBlock;

  if( m_nLinear ) return;

  if( m_nSubVertices > 0 )
  {
    aBuff = HMD_MemoryMalloc( m_nVertices * sizeof( D3DVERTEX ) );
    if( aBuff == NULL ) return;
    m_pInterpolationBuffer = (D3DVERTEX *)aBuff;
  }

  aBuff = HMD_MemoryMalloc( m_nVertices * sizeof( D3DVERTEX ) );

  if( aBuff == NULL ) return;

  m_pVertices = (D3DVERTEX *)aBuff;

  aVertex = m_pNewVertices;

  //If we have more than one set of vertices.
  if( m_nSubVertices )
  {
    m_ppSubVertices = (D3DVERTEX **)HMD_MemoryMalloc( ( m_nSubVertices + 1 ) * sizeof( D3DVERTEX ** ) );
    if( m_ppSubVertices == NULL )
      return;

    m_ppSubVertices[ 0 ] = m_pVertices;
    for( i = 0; i < m_nSubVertices; ++i )
    {
      m_ppSubVertices[ i + 1 ] = (D3DVERTEX *)HMD_MemoryMalloc( m_nVertices * sizeof( D3DVERTEX ) );
      if( m_ppSubVertices[ i + 1 ] == NULL )
        return;
    }
    m_pTweenBoxes = (Box *)HMD_MemoryMalloc( ( m_nSubVertices + 1 ) * sizeof( Box ) );
    if( m_pTweenBoxes == NULL )
      return;
  }
  else
  {
    m_pTweenBoxes = NULL;
  }

  for( i = 0; i < m_nVertices; ++i )
  {
    //This works because the D3DVERTEX member of aVertex is the first.
    memcpy( (void *)( m_pVertices + i ), (void *)aVertex, sizeof( D3DVERTEX ) );

    sibling = aVertex->sibling;
    for( j = 0; j < m_nSubVertices; ++j )
    {
      memcpy( (void *)( m_ppSubVertices[ j + 1 ] + i), (void *)sibling, sizeof( D3DVERTEX ) );
      sibling = sibling->sibling;
    }

    if( i == 0 )
    {
      m_BBox.minx = ( (D3DVERTEX *)aVertex )->x;
      m_BBox.maxx = ( (D3DVERTEX *)aVertex )->x;
      m_BBox.miny = ( (D3DVERTEX *)aVertex )->y;
      m_BBox.maxy = ( (D3DVERTEX *)aVertex )->y;
      m_BBox.minz = ( (D3DVERTEX *)aVertex )->z;
      m_BBox.maxz = ( (D3DVERTEX *)aVertex )->z;
      if( m_nSubVertices )
      {
        for( j = 0; j <= m_nSubVertices; ++j )
        {
          m_pTweenBoxes[ j ].minx = ( m_ppSubVertices[ j ] + i )->x;
          m_pTweenBoxes[ j ].maxx = ( m_ppSubVertices[ j ] + i )->x;
          m_pTweenBoxes[ j ].miny = ( m_ppSubVertices[ j ] + i )->y;
          m_pTweenBoxes[ j ].maxy = ( m_ppSubVertices[ j ] + i )->y;
          m_pTweenBoxes[ j ].minz = ( m_ppSubVertices[ j ] + i )->z;
          m_pTweenBoxes[ j ].maxz = ( m_ppSubVertices[ j ] + i )->z;
        }
      }
    }
    else
    {
      if( ( (D3DVERTEX *)aVertex )->x < m_BBox.minx )
        m_BBox.minx = ( (D3DVERTEX *)aVertex )->x;
      if( ( (D3DVERTEX *)aVertex )->x > m_BBox.maxx )
        m_BBox.maxx = ( (D3DVERTEX *)aVertex )->x;
      if( ( (D3DVERTEX *)aVertex )->y < m_BBox.miny )
        m_BBox.miny = ( (D3DVERTEX *)aVertex )->y;
      if( ( (D3DVERTEX *)aVertex )->y > m_BBox.maxy )
        m_BBox.maxy = ( (D3DVERTEX *)aVertex )->y;
      if( ( (D3DVERTEX *)aVertex )->z < m_BBox.minz )
        m_BBox.minz = ( (D3DVERTEX *)aVertex )->z;
      if( ( (D3DVERTEX *)aVertex )->z > m_BBox.maxz )
        m_BBox.maxz = ( (D3DVERTEX *)aVertex )->z;
      if( m_nSubVertices )
      {
        for( j = 0; j <= m_nSubVertices; ++j )
        {
          if( ( m_ppSubVertices[ j ] + i )->x < m_pTweenBoxes[ j ].minx )
            m_pTweenBoxes[ j ].minx = ( m_ppSubVertices[ j ] + i )->x;
          if( ( m_ppSubVertices[ j ] + i )->x > m_pTweenBoxes[ j ].maxx )
            m_pTweenBoxes[ j ].maxx = ( m_ppSubVertices[ j ] + i )->x;
          if( ( m_ppSubVertices[ j ] + i )->y < m_pTweenBoxes[ j ].miny )
            m_pTweenBoxes[ j ].miny = ( m_ppSubVertices[ j ] + i )->y;
          if( ( m_ppSubVertices[ j ] + i )->y > m_pTweenBoxes[ j ].maxy )
            m_pTweenBoxes[ j ].maxy = ( m_ppSubVertices[ j ] + i )->y;
          if( ( m_ppSubVertices[ j ] + i )->z < m_pTweenBoxes[ j ].minz )
            m_pTweenBoxes[ j ].minz = ( m_ppSubVertices[ j ] + i )->z;
          if( ( m_ppSubVertices[ j ] + i )->z > m_pTweenBoxes[ j ].maxz )
            m_pTweenBoxes[ j ].maxz = ( m_ppSubVertices[ j ] + i )->z;
        }
      }
    }
    aVertex = aVertex->next;
  }

  //3 verts per face...
  aBuff = HMD_MemoryMalloc( m_nFaces * sizeof( WORD ) * 3 );

  if( aBuff == NULL )
  {
    HMD_MemoryFree( m_pVertices );
    for( i = 0; i < m_nSubVertices; ++i )
    {
      HMD_MemoryFree( m_ppSubVertices[ i + 1 ] );
    }
    return;
  }

  m_pFaces = (WORD *)aBuff;

  aGroup = m_pGroups;

  i = 0;

  counter = 0;

  while( aGroup != NULL )
  {
    aFace = aGroup->pFaces;
    aGroup->offset = counter;
    counter += aGroup->number;

    while( aFace != NULL )
    {
      m_pFaces[ ( i * 3 ) ]      = (WORD)aFace->v1;
      m_pFaces[ ( i * 3 ) + 1 ]  = (WORD)aFace->v2;
      m_pFaces[ ( i * 3 ) + 2 ]  = (WORD)aFace->v3;
      aFace = aFace->next;
      ++i;
    }

    aGroup = aGroup->pNext;
  }
  //Cleanup pointers.
  aVertex = m_pNewVertices;
  for( i = 0; i < m_nVertices; ++i )
  {
    sibling = aVertex->sibling;   
    while( sibling )
    {
      oldVertex = sibling;
      sibling = sibling->next;
      HMD_MemoryFree( oldVertex );
    }
    oldVertex = aVertex;
    aVertex = aVertex->next;
    HMD_MemoryFree( oldVertex );
  }
  m_pNewVertices = NULL;

  aGroup = m_pGroups;

  while( aGroup != NULL )
  {
    aFace = aGroup->pFaces;
    while( aFace != NULL )
    {
      oldFace = aFace;
      aFace   = aFace->next;
      HMD_MemoryFree( oldFace );
    }

    aGroup->pFaces = NULL;
    aGroup        = aGroup->pNext;
  }

  myDBlock = m_pDiffBlocks;
  while( myDBlock )
  {
    oldDBlock = myDBlock;
    myDBlock  = myDBlock->next;
    HMD_MemoryFree( oldDBlock );
  }

  m_pDiffBlocks = NULL;

  m_nLinear = 1;
}

#define USE_HMD_MATERIAL 0

int render( meshx *m, view *in )
{
  int     current;
  D3DDevice* p3DDevice = pc3D::Get3DDevice();
  ASSERT(NULL != p3DDevice);

  NewGroup* aGroup;

  aGroup = m->m_pGroups;
  current = 0;

#if USE_HMD_MATERIAL
  if( HMD_Material == NULL )
  {
    HMD_Material = new material();
    HMD_Material->attach( in );
  }
#endif

  while( aGroup != NULL )
  {
    // Only set the shading mode if we aren't using the default shading mode
    if (!g_HMD_UseDefaultShadingMode)
    {
      if( aGroup->gouraud )
      {
        if (!p3DDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD))
          return(DDERR_GENERIC);
      }
      else
      {
        if (!p3DDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT))
          return(DDERR_GENERIC);
      }
    }
#if USE_HMD_MATERIAL
    HMD_Material->setAmbient( aGroup->cVector );
    HMD_Material->setDiffuse( aGroup->cVector );
    HMD_Material->setSpecular( aGroup->cVector );
    HMD_Material->setTexture( aGroup->texture );
    HMD_Material->setAsCurrent();
#else
    //Make sure it hooks in.
    aGroup->myMaterial->Attach( in );
    aGroup->myMaterial->SetAsCurrent();
#endif

    if (!p3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                         m->m_pVertices, m->m_nVertices,
                                         ( m->m_pFaces + 3 * current ),
                                         aGroup->number * 3, 0))
    {
      return(DDERR_GENERIC);
    }

    current += aGroup->number;
    aGroup = aGroup->pNext;
  }

  return 0;
}


meshx *meshx::copy( void )
{
  //If we're not linear, we can't copy.
  return copy( HMD_COPYALL );
}


meshx *meshx::copy( int flags )
{
  int i;
  meshx *myMesh;
  NewGroup *myGroup;

  //If we're not linear, return.
  if( !m_nLinear )
    return NULL;

  myMesh = new meshx();

  myMesh->m_nType         = m_nType;
  myMesh->m_nSize         = m_nSize;
  myMesh->m_nFaces        = m_nFaces;
  myMesh->m_nGroups       = m_nGroups;
  myMesh->m_BBox          = m_BBox;
  myMesh->m_nVertices     = m_nVertices;
  myMesh->m_nSubVertices  = m_nSubVertices;

  //Linear is set to zero, so we're not initialized
  //Check for initialized on return.
  myMesh->m_nLinear = 0;

  myMesh->m_nCopy = flags;

  if( m_nSubVertices > 0 )
  {
    myMesh->m_pInterpolationBuffer = (D3DVERTEX*)HMD_MemoryMalloc(m_nVertices * sizeof(D3DVERTEX));
    if( myMesh->m_pInterpolationBuffer == NULL )
      return NULL;
  }

  if( flags & HMD_COPYGROUPS )
  {
    if( m_nGroups > 0 )
    {
      myMesh->m_pGroups = (NewGroup*)HMD_MemoryMalloc(m_nGroups * sizeof(NewGroup));
      if( myMesh->m_pGroups == NULL )
        return NULL;
    }
    else
    {
      myMesh->m_pGroups = NULL;
    }

    if( m_nCopy & HMD_COPYGROUPS )
    {
      if( m_nGroups > 0 )
        memcpy( myMesh->m_pGroups, m_pGroups, m_nGroups * sizeof( NewGroup ) );
    }
    else
    {
      myGroup = m_pGroups;
      for( i = 0; i < m_nGroups; ++i )
      {
        memcpy( ( ( myMesh->m_pGroups ) + i ), myGroup, sizeof( NewGroup ) );
        myGroup = myGroup->pNext;
      }
    }

    //Set the pointers for the groups.
    for( i = 0; i < m_nGroups; ++i );
    {
      myMesh->m_pGroups[ i ].pNext = &( myMesh->m_pGroups[ i + 1 ] );
    }
    myMesh->m_pGroups[ m_nGroups - 1 ].pNext = NULL;

  }
  else
  {
    myMesh->m_pGroups = m_pGroups;
  }

  if( flags & HMD_COPYVERTICES )
  {
    if( m_nSubVertices > 0 )
    {
      //We have multiple sets of vertices here.
      myMesh->m_ppSubVertices = (D3DVERTEX **)HMD_MemoryMalloc( m_nSubVertices * sizeof( D3DVERTEX ** ) );
      if( myMesh->m_ppSubVertices == NULL )
        return NULL;
      
      for( i = 0; i <= m_nSubVertices; ++i )
      {
        myMesh->m_ppSubVertices[ i ] = (D3DVERTEX *)HMD_MemoryMalloc( m_nSubVertices * sizeof( D3DVERTEX * ) );
        if( myMesh->m_ppSubVertices[ i ] == NULL )
          return NULL;

        if( m_nVertices > 0 )
        {
          memcpy( myMesh->m_ppSubVertices[ i ], m_ppSubVertices[ i ], m_nVertices * sizeof( D3DVERTEX ) );
        }
      }
      myMesh->m_pVertices = m_ppSubVertices[ 0 ];
    }
    else
    {
      myMesh->m_ppSubVertices = NULL;
      if( m_nVertices > 0 )
      {
        myMesh->m_pVertices = (D3DVERTEX *)HMD_MemoryMalloc( m_nVertices * sizeof( D3DVERTEX ) );
        if( myMesh->m_pVertices == NULL )
        {
          if( myMesh->m_pGroups != NULL && ( flags & HMD_COPYGROUPS ) ) 
            HMD_MemoryFree( myMesh->m_pGroups );
          return NULL;
        }
      }
      else
      {
        myMesh->m_pVertices = NULL;
      }

      if( m_nVertices > 0 )
      {
        memcpy( myMesh->m_pVertices, m_pVertices, m_nVertices * sizeof( D3DVERTEX ) );
      }
    }
  }
  else
  {
    myMesh->m_pVertices = m_pVertices;
    myMesh->m_ppSubVertices = m_ppSubVertices;
  }

  if( flags & HMD_COPYFACES )
  {
    if( m_nFaces > 0 )
    {
      myMesh->m_pFaces = (WORD *)HMD_MemoryMalloc( m_nFaces * sizeof( WORD ) * 3 );
      if( m_pFaces == NULL )
      {
        if( myMesh->m_pGroups != NULL && ( flags & HMD_COPYGROUPS ) )
          HMD_MemoryFree( myMesh->m_pGroups );
        if( myMesh->m_pVertices != NULL && ( flags & HMD_COPYVERTICES ) )
          HMD_MemoryFree( myMesh->m_pVertices );
        return NULL;
      }
    }
    else
    {
      myMesh->m_pFaces = NULL;
    }

    if( m_nFaces > 0 )
    {
      memcpy( myMesh->m_pFaces, m_pFaces, m_nFaces * 3 * sizeof( WORD ) );
    }
  }
  else
  {
    myMesh->m_pFaces = m_pFaces;
  }

  if( flags & HMD_COPYBOXES )
  {
    if( m_nSubVertices )
    {
      myMesh->m_pTweenBoxes = (Box *)HMD_MemoryMalloc( ( m_nSubVertices + 1 ) * sizeof( Box ) );
      if( myMesh->m_pTweenBoxes == NULL )
        return NULL;
      memcpy( myMesh->m_pTweenBoxes, m_pTweenBoxes, ( m_nSubVertices + 1 ) * sizeof( Box ) );
    }
  }
  else
  {
    myMesh->m_pTweenBoxes = m_pTweenBoxes;
  }

  myMesh->m_nLinear = 1;

  return myMesh;
}


int meshx::initialized( void )
{
  return( m_nLinear );
}


NewVertex *meshx::VertexAt( int position )
{
  int i;
  NewVertex *myVertex;

  if( m_nLinear ) return NULL;

  if( position >= m_nVertices ) return NULL;

  myVertex = m_pNewVertices;
  for( i = 0; i < position; ++i )
    myVertex = myVertex->next;

  return myVertex;

}


NewGroup *meshx::GroupAt( int index )
{
  NewGroup *myGroup;

  if( index >= m_nGroups )
    return NULL;

  myGroup = m_pGroups;

  for(int i = 0; i < index; ++i )
    myGroup = myGroup->pNext;

  return myGroup;
}


NewGroup *meshx::GroupThatMatches(Surface *texture, int gouraud, 
                                  CLRVALUE* cVector)
{
  NewGroup *myGroup;

  if( m_nLinear ) return NULL;

  myGroup = m_pGroups;

  while( myGroup != NULL )
  {
    if ( ( texture == myGroup->texture ) &&
         ( gouraud == myGroup->gouraud ) &&
         ( cVector->r == myGroup->cVector.r ) &&
         ( cVector->g == myGroup->cVector.g ) &&
         ( cVector->b == myGroup->cVector.b ) )
      return myGroup;

    myGroup = myGroup->pNext;
  }

  return NULL;
}

void meshx::HMD_interpolate( int pose0, int pose1, D3DVALUE amount )
{
  int i;
  D3DVERTEX *subVertex0;
  D3DVERTEX *subVertex1;

  //No sub-vertices to interpolate between.
  if( m_nSubVertices == 0 )
    return;

  //Out of range checks
  if( pose0 < 0 || pose0 > m_nSubVertices )
    return;
  if( pose1 < 0 || pose1 > m_nSubVertices )
    return;

  //Shortcuts, if we're at either end of the spectrum, just
  //Set the pointer to the predefined vertices.
  if( pose0 == pose1 )
  {
    m_pVertices = m_ppSubVertices[ pose0 ];
    memcpy( &m_BBox, &m_pTweenBoxes[ pose0 ], sizeof( Box ) );
    return;
  }
  if( amount == 0.0f )
  {
    m_pVertices = m_ppSubVertices[ pose0 ];
    memcpy( &m_BBox, &m_pTweenBoxes[ pose0 ], sizeof( Box ) );
    return;
  }
  if( amount == 1.0f )
  {
    m_pVertices = m_ppSubVertices[ pose1 ];
    memcpy( &m_BBox, &m_pTweenBoxes[ pose1 ], sizeof( Box ) );
    return;
  }

  m_pVertices = m_pInterpolationBuffer;

  subVertex0 = m_ppSubVertices[ pose0 ];
  subVertex1 = m_ppSubVertices[ pose1 ];

  for( i = 0; i < m_nVertices; ++i )
  {
    m_pInterpolationBuffer[ i ].x = subVertex0[ i ].x + ( subVertex1[ i ].x - subVertex0[ i ].x ) * amount;
    m_pInterpolationBuffer[ i ].y = subVertex0[ i ].y + ( subVertex1[ i ].y - subVertex0[ i ].y ) * amount;
    m_pInterpolationBuffer[ i ].z = subVertex0[ i ].z + ( subVertex1[ i ].z - subVertex0[ i ].z ) * amount;
    m_pInterpolationBuffer[ i ].nx = subVertex0[ i ].nx + ( subVertex1[ i ].nx - subVertex0[ i ].nx ) * amount;
    m_pInterpolationBuffer[ i ].ny = subVertex0[ i ].ny + ( subVertex1[ i ].ny - subVertex0[ i ].ny ) * amount;
    m_pInterpolationBuffer[ i ].nz = subVertex0[ i ].nz + ( subVertex1[ i ].nz - subVertex0[ i ].nz ) * amount;
    m_pInterpolationBuffer[ i ].tu = subVertex0[ i ].tu;
    m_pInterpolationBuffer[ i ].tv = subVertex0[ i ].tv;
  }
  m_BBox.minx = m_pTweenBoxes[ pose0 ].minx + ( m_pTweenBoxes[ pose1 ].minx - m_pTweenBoxes[ pose0 ].minx ) * amount;
  m_BBox.maxx = m_pTweenBoxes[ pose0 ].maxx + ( m_pTweenBoxes[ pose1 ].maxx - m_pTweenBoxes[ pose0 ].maxx ) * amount;
  m_BBox.miny = m_pTweenBoxes[ pose0 ].miny + ( m_pTweenBoxes[ pose1 ].miny - m_pTweenBoxes[ pose0 ].miny ) * amount;
  m_BBox.maxy = m_pTweenBoxes[ pose0 ].maxy + ( m_pTweenBoxes[ pose1 ].maxy - m_pTweenBoxes[ pose0 ].maxy ) * amount;
  m_BBox.minz = m_pTweenBoxes[ pose0 ].minz + ( m_pTweenBoxes[ pose1 ].minz - m_pTweenBoxes[ pose0 ].minz ) * amount;
  m_BBox.maxz = m_pTweenBoxes[ pose0 ].maxz + ( m_pTweenBoxes[ pose1 ].maxz - m_pTweenBoxes[ pose0 ].maxz ) * amount;
}


void HMD_RestoreCurrentBank( void )
{
//  HMD_TimTracker *myTimTracker;
//  for(int i = 0; i < 32; ++i)
//  {
//    myTimTracker = HMD_Tracker[ HMD_CurrentTimBank ][ i ];
//    while( myTimTracker )
//    {
//      if( myTimTracker->myHwSurface )
//      {
//        myTimTracker->myHwSurface->RestoreSurface(*myTimTracker->mySurface);
//      }
//      myTimTracker = myTimTracker->next;
//    }
//  }
}

void HMD_RestoreAll( void )
{
//  int currentBank;
//  int i;
//
//  currentBank = HMD_CurrentTimBank;
//
//  for( i = 0; i < HMD_MAX_TEXTURE_BANKS; ++i )
//  {
//    HMD_CurrentTimBank = i;
//    HMD_RestoreCurrentBank();
//  }
//
//  HMD_CurrentTimBank = currentBank;
}

void HMD_ReleaseCurrentBank( void )
{
  HMD_TimTracker *myTimTracker;
  HMD_TimTracker *oldTimTracker;

  for (int i = 0; i < 32; ++i)
  {
    myTimTracker = HMD_Tracker[ HMD_CurrentTimBank ][ i ];
    HMD_Tracker[ HMD_CurrentTimBank ][ i ] = NULL;
    while( myTimTracker )
    {
      DESTROYSURFACE(myTimTracker->mySurface);

      oldTimTracker = myTimTracker;
      myTimTracker = myTimTracker->next;
      HMD_MemoryFree( oldTimTracker );
    }
  }
}

void HMD_ReleaseAll( void )
{
  int currentBank;
  int i;

  currentBank = HMD_CurrentTimBank;

  for( i = 0; i < HMD_MAX_TEXTURE_BANKS; ++i )
  {
    HMD_CurrentTimBank = i;
    HMD_ReleaseCurrentBank();
  }

  HMD_CurrentTimBank = currentBank;
	HMD_ClearMaterials();
}

void HMD_SetCurrentTimBank( int bank )
{
  HMD_CurrentTimBank = bank;
}

void HMD_Foo( int posex, int posey, D3DVALUE amount, void *mesh )
{
  meshx *myMesh;
  myMesh = (meshx *)mesh;

  myMesh->HMD_interpolate( posex, posey, amount );
}

/********************************************************************
 * Shuts down the HMD system...releases everything. So everything will
 * have to be re-loaded.
 ********************************************************************/
void HMD_Shutdown( void )
{
//	HMD_ClearCollected();
//	HMD_ReleaseRenderBuffers();
//	HMD_ReleaseAll();
	HMD_ClearMaterials();
	HMD_CurrentTimBank = 0;
}


int AddMaterial( NewGroup *aGroup )
{
  HMD_MaterialTracker *myTracker;

  myTracker = FindMaterial( aGroup );

  if( myTracker == NULL )
  {
    myTracker = AddGlobalMaterial( aGroup );
  }

  if( myTracker == NULL )
    return 1;
  
  aGroup->myMaterial = myTracker->myMaterial;
  aGroup->materialIndex = myTracker->index;

  return 0;
}

HMD_MaterialTracker *FindMaterial( NewGroup *aGroup )
{
  HMD_MaterialTracker *myMaterialTracker;

  myMaterialTracker = HMD_MaterialsUsedHead;

  while( myMaterialTracker )
  {
    if( myMaterialTracker->cVector == aGroup->cVector )
    {
      if( myMaterialTracker->matTexture == aGroup->texture )
        return myMaterialTracker;
    }
    myMaterialTracker = myMaterialTracker->next;
  }
  return NULL;
}


HMD_MaterialTracker *AddGlobalMaterial( NewGroup *aGroup )
{
  int i;
  material *myMat;

  HMD_MaterialTracker *returnValue;

  if( HMD_MaterialsFreeHead == NULL )
  {
    if( HMD_MaterialsUsedHead == NULL )
    {
      //initialize it;
      HMD_MaterialsFreeHead = HMD_Materials;
      for( i = 0; i < HMD_MAX_MATERIALS; ++i )
      {
        HMD_Materials[ i ].next = &HMD_Materials[ i + 1 ];
        HMD_Materials[ i ].index = i;
      }
      HMD_Materials[ HMD_MAX_MATERIALS - 1 ].next = NULL;
    }
    else
    {
      //No more left, return NULL
      return NULL;
    }
  }

  returnValue = HMD_MaterialsFreeHead;
  HMD_MaterialsFreeHead = HMD_MaterialsFreeHead->next;

  returnValue->next = HMD_MaterialsUsedHead;
  HMD_MaterialsUsedHead = returnValue;

  myMat = new material();

  myMat->SetAmbient( aGroup->cVector );
  myMat->SetDiffuse( aGroup->cVector );
  myMat->SetSpecular( aGroup->cVector );

  myMat->SetTexture( aGroup->texture );
  returnValue->matTexture = aGroup->texture;

  aGroup->myMaterial      = myMat;
  returnValue->myMaterial = myMat;
  returnValue->cVector    = aGroup->cVector;

  return returnValue;
}

void HMD_ClearMaterials( void )
{
	HMD_MaterialTracker *myMaterial;

	myMaterial = HMD_MaterialsUsedHead;

	while( myMaterial )
	{
		DESTROYPOINTER(myMaterial->myMaterial);
    myMaterial = myMaterial->next;
	}

	HMD_MaterialsFreeHead = NULL;
	HMD_MaterialsUsedHead = NULL;
}

void HMD_CollectForRendering( frame *aFrame )
{
  int i;
  int n;
  NewGroup *aGroup;
  meshx *aMesh;
  mesh *myMesh;
  HMD_RenderBuffer *myBuffer;

  n = aFrame->GetNumMeshes();

  for( i = 0; i < n; ++i )
  {
    myMesh = aFrame->GetMesh( i );
    if( myMesh->getType() == 5 )
    {
      aMesh = (meshx *)aFrame->GetMesh( n );
      aGroup = aMesh->m_pGroups;
      while( aGroup )
      {
        myBuffer          = HMD_NewRenderBuffer();
        myBuffer->frame   = aFrame;
        myBuffer->mesh    = aMesh;
        myBuffer->group   = aGroup;
        myBuffer->next    = HMD_RenderBuffers[ aGroup->materialIndex ];
        HMD_RenderBuffers[ aGroup->materialIndex ] = myBuffer;
        aGroup = aGroup->pNext;
      }
    }
  }

  n = aFrame->GetNumChildren();
  for (i=0; i < n; i++)
  {
    HMD_CollectForRendering( aFrame->GetChild(i) ); 
  }
}

void HMD_RenderCollected( view *aView )
{
  D3DDevice* p3DDevice = pc3D::Get3DDevice();
  ASSERT(NULL != p3DDevice);

  HMD_RenderBuffer  *myBuffer;

  for(int i = 0; i < HMD_MAX_MATERIALS; ++i)
  {
    myBuffer = HMD_RenderBuffers[ i ];

    if( myBuffer == NULL ) continue;

    HMD_Materials[ i ].myMaterial->Attach( aView );
    HMD_Materials[ i ].myMaterial->SetAsCurrent();

    while( myBuffer )
    {
      // Only set the shading mode if we aren't using the default shading mode
      if (!g_HMD_UseDefaultShadingMode)
      {
        if( myBuffer->group->gouraud )
          p3DDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
        else
          p3DDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);
      }

      p3DDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,
                              myBuffer->frame->GetWorldMatrix());

      p3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                      myBuffer->mesh->m_pVertices,     
                                      myBuffer->mesh->m_nVertices,
                                      (myBuffer->mesh->m_pFaces + 
                                      (3 * myBuffer->group->offset)),
                                      myBuffer->group->number * 3, 0);

      myBuffer = myBuffer->next;
    }
  }
}

void HMD_ClearCollected( void )
{
  HMD_RenderBufferTracker *myTracker;
  ZeroMemory( &HMD_RenderBuffers, sizeof( HMD_RenderBuffer * ) * HMD_MAX_MATERIALS );

  myTracker = HMD_RenderBufferTrackers;

  while( myTracker )
  {
    myTracker->indexUsed = 0;
    myTracker = myTracker->next;
  }
}

HMD_RenderBuffer *HMD_NewRenderBuffer( void )
{
  HMD_RenderBuffer *myBuffer;
  HMD_RenderBufferTracker *myTracker;

  if( HMD_RenderBufferTrackers == NULL )
  {
    HMD_RenderBufferTrackers = (HMD_RenderBufferTracker *)HMD_MemoryMalloc( sizeof( HMD_RenderBufferTracker ) );
    HMD_RenderBufferTrackers->indexUsed = 0;
    HMD_RenderBufferTrackers->next = NULL;
  }

  myTracker = HMD_RenderBufferTrackers;

  while( myTracker->next && myTracker->indexUsed >= HMD_BUFFERS_IN_TRACKER )
  {
    myTracker = myTracker->next;
  }

  myBuffer = &( myTracker->buffers[ myTracker->indexUsed ] );

  myTracker->indexUsed++;

  if( !myTracker->next && myTracker->indexUsed >= HMD_BUFFERS_IN_TRACKER )
  {
    myTracker->next  = (HMD_RenderBufferTracker *)HMD_MemoryMalloc( sizeof( HMD_RenderBufferTracker ) );
    myTracker->next->indexUsed = 0;
    myTracker->next->next = NULL; 
  }

  return( myBuffer );
}

void HMD_ReleaseRenderBuffers( void )
{
  HMD_RenderBufferTracker *myTracker, *oldTracker;

  myTracker = HMD_RenderBufferTrackers;

  while( myTracker )
  {
    oldTracker = myTracker;
    myTracker = myTracker->next;
    HMD_MemoryFree( oldTracker );
  }

  HMD_RenderBufferTrackers = NULL;
}

void HMD_LoadBMPForTIM(ULONG x, ULONG y, ULONG w, ULONG h, LPCSTR szBmpFile)
{
  ULONG           tpage       = getTPage( 1, 0, x, y );
  ULONG           slot        = tpage & 0x00000001F; //00->31
  HMD_TimTracker* pNewTracker = NULL;

  if (!g_bHMD_TrackerInitialized)
  {
    ZeroMemory(HMD_Tracker, sizeof(HMD_TimTracker*) * 16 * 32);
    g_bHMD_TrackerInitialized = TRUE;
  }

  //Have we already loaded a map at this place?
  if (pNewTracker = CompatibleTracker2(x, y, w, h, tpage))
  {
    pNewTracker->clut = 0;
    DESTROYSURFACE(pNewTracker->mySurface);
    pNewTracker->mySurface = new Surface(szBmpFile, TRUE);
  }
  else
  {
    pNewTracker = (HMD_TimTracker *)HMD_MemoryMalloc( sizeof( HMD_TimTracker ) );
    if (!pNewTracker) return;

    pNewTracker->clut       = 0;
    pNewTracker->tpage      = tpage;
    pNewTracker->mySurface  = new Surface(szBmpFile, TRUE);
    pNewTracker->x          = x;
    pNewTracker->y          = y;
    pNewTracker->w          = w;
    pNewTracker->h          = h;
    pNewTracker->next       = NULL;

    if (!HMD_Tracker[HMD_CurrentTimBank][slot])
      HMD_Tracker[HMD_CurrentTimBank][slot] = pNewTracker;
    else
    {
      pNewTracker->next = HMD_Tracker[HMD_CurrentTimBank][slot];
      HMD_Tracker[HMD_CurrentTimBank][slot] = pNewTracker;
    }
  }

  ASSERT(NULL != pNewTracker);
}

#endif // USE_OLD_FRAME
