// ===========================================================================
// Module:      MeshGroup.cpp
//
// Description: 
//
// Created:     July 8, 1999
//
// Author:      
//
// Copywrite:   Artech, 1999
// ===========================================================================

#include "PC3DHdr.h"
#include "MeshGroup.h"

#include "GlobalData.h"
#include "l_Material.h"
#include "NewMesh.h"

// ===========================================================================
// MeshGroup class ===========================================================
// ===========================================================================


// Destructor ================================================================
MeshGroup::~MeshGroup()
{
  RELEASEPOINTER(m_pTextureID);
}

// ===========================================================================
// PUBLIC: MeshGroup member functions ========================================
// ===========================================================================


// ===========================================================================
// Function:    SetTexture
//
// Description: Sets the texture for this mesh group
//
// Params:      pTxtrID:  Pointer to the textureID we are using to set the
//                        texture for this mesh group
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void MeshGroup::SetTexture(TextureID* pTxtrID)
{
  RELEASEPOINTER(m_pTextureID);
  m_pTextureID = pTxtrID;
}


// ===========================================================================
// Function:    Render
//
// Description: Renders this mesh group
//
// Params:      p3DDevice:    The device to use to render the mesh group
//              pParentMesh:  Pointer to the parent mesh
//              dwFlags:      Extra render settings
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void MeshGroup::Render(D3DDevice* p3DDevice, Mesh* pParentMesh,
                       DWORD dwFlags /*= 0*/)
{
  // Set the current texture
  if (m_pTextureID)
  {
    ASSERT(NULL != m_pTextureID->pTexture);
    m_pTextureID->pTexture->SetAsActiveTexture();
  }
  else
    p3DDevice->SetTexture(0, NULL);

  // Set the current material
  if (-1 != m_nMaterialIdx /*&& !pParentMesh->UsingAlphaBlending()*/)
    G_MATERIALS.GetAt(m_nMaterialIdx)->SetAsCurrent();

  // Render the group
  p3DDevice->DrawIndexedPrimitiveStrided(D3DPT_TRIANGLELIST, m_dwVtxFormat,
                                         &(pParentMesh->m_StrideData),
                                         pParentMesh->m_pVertices->GetSize(), 
                                         m_awVertices.GetData(),
                                         m_awVertices.GetSize(), dwFlags);
}
