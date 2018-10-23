// ===========================================================================
// Module:      MeshGroup.h
//
// Description: 
//
// Created:     July 8, 1999
//
// Author:      
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_MESHGROUP
#define INC_MESHGROUP

class Mesh;
struct TextureID;

// ===========================================================================
// MeshGroup class ===========================================================
// ===========================================================================
class MeshGroup
{
  // Member variables ========================================================
  private:
    DWORD       m_dwVtxFormat;
    TextureID*  m_pTextureID;
    int         m_nMaterialIdx;
    WordArray   m_awVertices;


  // Construction / Destruction ==============================================
  public:
    MeshGroup() : m_dwVtxFormat(VTX_TXTR), m_nMaterialIdx(0),
                  m_pTextureID(NULL) {                                      }

    ~MeshGroup();


  // Private member functions ================================================
  private:


  // Protected member functions ==============================================
  protected:


  // Public member functions =================================================
  public:
    DWORD GetVertexFormat() const         { return(m_dwVtxFormat);          }
    int   GetMaterialIdx()  const         { return(m_nMaterialIdx);         }
    int   GetNumVertices()  const         { return(m_awVertices.GetSize()); }
    TextureID* GetTexture() const         { return(m_pTextureID);           }

    void  SetVertexFormat(DWORD dwFormat) { m_dwVtxFormat = dwFormat;       }
    void  SetMaterialIdx(int nIdx)        { m_nMaterialIdx = nIdx;          }
    void  AddVertex(WORD wVtxIdx)         { m_awVertices.Add(wVtxIdx);      }

    void  SetTexture(TextureID* pTxtrID);


    void Render(D3DDevice* p3DDevice, Mesh* pParentMesh, DWORD dwFlags = 0);

};


#endif // INC_MESHGROUP
// ===========================================================================
