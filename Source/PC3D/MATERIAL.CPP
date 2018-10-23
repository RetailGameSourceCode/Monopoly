// //////////////////////////////////////////////////
// Copyright (C) 1997, Artech,        
// All rights reserved.          
//                 
// This document contains intellectual property  
// of Artech.  Any copying or reproduction    
// without prior written permission is prohibited. 
//                 
// //////////////////////////////////////////////////
// MODULE: material.cpp 
//                  
// ///////////////// END OF HEADER //////////////////
#include "PC3DHdr.h"
#include "l_Material.h"


// Constructor ===============================================================
material::material()
{
  ZeroMemory(&m_MaterialData, sizeof(MATERIALDATA));
  m_MaterialData.dwSize = sizeof(MATERIALDATA);

  // Leave the default material black
  m_MaterialData.dwRampSize = 1;
  m_MaterialData.diffuse.a  = 1.0f;

  m_pMaterial = pc3D::GetDDrawDriver()->CreateMaterial();
  ASSERT(NULL != m_pMaterial);
  m_pMaterial->SetMaterial(&m_MaterialData);

  m_pMaterial->GetHandle(DDRAW_GetD3DDevice(), &m_hMaterial);
  m_pCurrentView  = NULL;
  m_pTexture      = NULL;
  m_bChanged      = TRUE;

  Refresh();
} // end of material::material


// ===========================================================================
// Function:    setAsCurrent
//
// Description: Sets this material and its texture as the active ones for
//              rendering
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void material::SetAsCurrent()
{
//  ASSERT(NULL != m_pCurrentView); // material must have been attached to a view
  HRESULT hResult = NULL;

  Refresh();
  
  D3DDevice* p3DDevice = pc3D::Get3DDevice();
  ASSERT(NULL != p3DDevice);
  
  if (!p3DDevice->SetLightState(D3DLIGHTSTATE_MATERIAL, m_hMaterial)) return;

#ifdef USE_OLD_FRAME  
  if (m_pTexture)
  {
    VERIFY(m_pTexture->SetAsActiveTexture());
  }
  else
  {
    VERIFY(p3DDevice->SetTexture(0, NULL));
  }
#endif // USE_OLD_FRAME  
}
  

// ===========================================================================
// End of file ===============================================================
// ===========================================================================
