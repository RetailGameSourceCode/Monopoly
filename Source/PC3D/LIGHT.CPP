// //////////////////////////////////////////////////
// Copyright (C) 1997, Artech,        
// All rights reserved.          
//                 
// This document contains intellectual property  
// of Artech.  Any copying or reproduction    
// without prior written permission is prohibited. 
//                 
// //////////////////////////////////////////////////
// MODULE: Light.cpp 
//                  
// ///////////////// END OF HEADER //////////////////
#include "PC3DHdr.h"
#include "l_Light.h"



// ===========================================================================
// Light class ===============================================================
// ===========================================================================

// Constructor ===============================================================
Light::Light() : m_pInView(NULL)
{
  // Get the DDraw driver we are using
  DDrawDriver* pDDrawDriver = pc3D::GetDDrawDriver();
  ASSERT(NULL != pDDrawDriver);

  // Initialize our light data structure
  ZeroMemory(&m_LightData, sizeof(LIGHTDATA));
  m_LightData.dwSize = sizeof(LIGHTDATA);
  
  // Create the DirectX light interface
  m_pD3DLight = pDDrawDriver->CreateLight();
  ASSERT(NULL != m_pD3DLight);

  m_bChanged = TRUE;

  // default to no attenuation with distance
  m_LightData.dvAttenuation0 = 1.0f;
  m_LightData.dvAttenuation1 = 0.0f;
  m_LightData.dvAttenuation2 = 0.0f;

  // default to no max distance
  m_LightData.dvRange         = D3DLIGHT_RANGE_MAX;

  // default to linear ramp from inner to outer cone
  m_LightData.dvFalloff       = 1.0f;
  
  // default to on
  m_LightData.dwFlags         = D3DLIGHT_ACTIVE;
}


// Destructor ================================================================
Light::~Light()
{
  // report that this light is still being used in a view
  ASSERT(NULL == m_pInView); 
  RELEASEPOINTER(m_pD3DLight);
}

  
// ===========================================================================
// PointLight class ==========================================================
// ===========================================================================


// Constructor ===============================================================
PointLight::PointLight(const CLRVALUE& color, const vector& position) : Light()
{
  m_LightData.dltType = D3DLIGHT_POINT;
  CopyMemory(&(m_LightData.dcvColor), &color, sizeof(CLRVALUE));
  CopyMemory(&(m_LightData.dvPosition), &position, sizeof(vector));
  m_bChanged = TRUE;
}
  

// ===========================================================================
// SpotLight class ===========================================================
// ===========================================================================


// Constructor ===============================================================
SpotLight::SpotLight(const CLRVALUE& color, const vector& position, 
                     const vector& direction, float umbra_angle,
                     float penumbra_angle) : Light()
{
  m_LightData.dltType = D3DLIGHT_SPOT;
  CopyMemory(&(m_LightData.dcvColor), &color, sizeof(CLRVALUE));
  CopyMemory(&(m_LightData.dvPosition), &position, sizeof(vector));
  CopyMemory(&(m_LightData.dvDirection), &direction, sizeof(vector));
  m_LightData.dvTheta = umbra_angle;
  m_LightData.dvPhi   = penumbra_angle;

  m_bChanged = TRUE;
}
  

// ===========================================================================
// DirectionalLight class ====================================================
// ===========================================================================


// Constructor ===============================================================
DirectionalLight::DirectionalLight(const CLRVALUE& color, 
                                   const vector& direction) : Light()
{
  m_LightData.dltType = D3DLIGHT_DIRECTIONAL;
  CopyMemory(&(m_LightData.dcvColor), &color, sizeof(CLRVALUE));
  CopyMemory(&(m_LightData.dvDirection), &direction, sizeof(vector));
  m_bChanged = TRUE;
}


// ===========================================================================
// ParallelPointLight class ==================================================
// ===========================================================================


// Constructor ===============================================================
ParallelPointLight::ParallelPointLight(const CLRVALUE& color, 
                                       const vector& position) : Light()
{
  m_LightData.dltType = D3DLIGHT_PARALLELPOINT;
  CopyMemory(&(m_LightData.dcvColor), &color, sizeof(CLRVALUE));
  CopyMemory(&(m_LightData.dvPosition), &position, sizeof(vector));
  m_bChanged = TRUE;
}


// ===========================================================================
// End Of File ===============================================================
// ===========================================================================
