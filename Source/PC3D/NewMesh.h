// ===========================================================================
// Module:      Mesh.h
//
// Description: Definition of the Mesh class.
//
// Created:     July 8, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_MESH
#define INC_MESH

#include "MeshGroup.h"

typedef Array<Vertex, Vertex>             VertexArray;
typedef Array<VertexArray*, VertexArray*> VertexSet;
typedef Array<MeshGroup*, MeshGroup*>     MeshGroupPtrArray;
typedef Array<Box, Box>                   BoxArray;


// ===========================================================================
// Mesh class ================================================================
// ===========================================================================
class Mesh
{
  friend void MeshGroup::Render(D3DDevice* p3DDevice, Mesh* pParentMesh,
                                DWORD dwFlags /*= 0*/);

  // Member variables ========================================================
  private:
    VertexSet           m_apVertexSets;  // Array of vertex arrays
    VertexArray         m_aInterpolatedVertices;
    VertexArray*        m_pVertices;
    MeshGroupPtrArray   m_apGroups;
    D3DSTRIDEDATA       m_StrideData;
    Box                 m_BoundingBox;
    BoxArray            m_aBBoxes;
    
    HMDDiffBlock*       m_pDiffBlocks;
    int                 m_nNumDiffBlocks;
    int                 m_nNumVtxDiffBlocks;
    int                 m_nNumNormDiffBlocks;

    BYTE                m_byZBias;
    DWORD               m_dwSrcBlend;
    DWORD               m_dwDestBlend;


  // Construction / Destruction ==============================================
  public:
    Mesh();
    ~Mesh();


  // Private member functions ================================================
  private:


  // Protected member functions ==============================================
  protected:
    HMDHeader* ProcessHMDHeader(DWORD* pData);
    void  ProcessHMDPrimitive(HMDPrimitive* pPrimitive, HMDHeader* pHeader);
    void  ProcessHMDPrimitiveData(DWORD* pData, DWORD* pHeader);
    void  ProcessHMDTriangle(DWORD* pData, HMDPrimitiveHdr* pPrimHdr);
    BOOL  ProcessHMDImage(DWORD* pData, DWORD* pHeader);
    void  ProcessHMDMIMe(DWORD* pData, DWORD* pHeader);
    void  ProcessHMDDiffBlock(DWORD* pData, DWORD* pHeader, BOOL bNormalBlock);

    void  AddTriangle(const HMDTriangle& triangle, HMDPrimitiveHdr* pPrimHdr);

    HMDDiffBlock* NewDiffBlock(BOOL bNormalBlock);

    TextureID*  FindTexture(BYTE byXPos, BYTE byYPos, WORD wTPageID);
    BOOL        TextureHasBeenLoaded(TextureID* pTextureID);

    MeshGroup*  FindMeshGroup(TextureID* pTextureID, const HMDColour& clr);
    MeshGroup*  CreateNewMeshGroup(TextureID* pTextureID, const HMDColour& clr);

    int   FindVertex(const Vertex& vtx);
    void  SetSubVertices(int nVertex, WORD wHMDVtxIdx, WORD wHMDNormIdx);

    void  ComputeBoundingBox();
    void  BindStrideData(void* pData);


  // Public member functions =================================================
  public:
    void  GetBoundingBox(Box& BBox) const { BBox = m_BoundingBox;           }
    void  SetBoundingBox(const Box& BBox) { m_BoundingBox = BBox;           }
    int   GetNumVertices() const          { return(m_pVertices->GetSize()); }
    int   GetNumSubVertices() const     { return(m_apVertexSets.GetSize()); }
    void  SelectVertices(int nIdx) { m_pVertices = m_apVertexSets.GetAt(nIdx); }
    void  SetZBias(BYTE byZBias)          { m_byZBias = byZBias;            }
    BYTE  GetZBias() const                { return(m_byZBias);              }

    int   GetNumMeshGroups()              { return(m_apGroups.GetSize());   }
    MeshGroup* GetMeshGroup(int nIdx)     { return(m_apGroups.GetAt(nIdx)); }

    BOOL  UsingAlphaBlending() const      
    { return(0 != m_dwSrcBlend && 0!= m_dwDestBlend);                       }

    void  SetBlendingFactors(DWORD dwSrc, DWORD dwDest)
    { m_dwSrcBlend = dwSrc; m_dwDestBlend = dwDest;                         }

    VertexArray* GetVertices()            { return(m_pVertices);            }
    VertexSet*   GetVertexSets()          { return(&m_apVertexSets);        }

    BOOL  LoadFromHMD(DWORD* pData);
    BOOL  SubstituteTexture(TextureID* pTextureID);
  
    void  Interpolate(int nPose1, int nPose2, float fAmount);

    void  Render(D3DDevice* p3DDevice);

};

extern "C" {

void Mesh_EnableTexturePurge(BOOL bEnable);
void Mesh_PurgeTextures();

}

#endif // INC_MESH
// ===========================================================================
