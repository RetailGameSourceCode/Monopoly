// ===========================================================================
// Module:      RenderSettings.inl
//
// Description: Contains the inline functions and macros for changing the
//              render settings for Direct3D.
//
// Created:     June 22, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_RENDERSETTINGSINL
#define INC_RENDERSETTINGSINL

// ===========================================================================
// INLINE: Render settings functions =========================================
// ===========================================================================

// ===========================================================================
INL BOOL pc3D::SetShadeMode(BYTE byMode)
{
  return(Get3DDevice()->SetRenderState(D3DRENDERSTATE_SHADEMODE, (DWORD)byMode));
}

// ===========================================================================
INL BOOL pc3D::SetFillMode(BYTE byMode)
{
  return(Get3DDevice()->SetRenderState(D3DRENDERSTATE_FILLMODE, (DWORD)byMode));
}

// ===========================================================================
INL BOOL pc3D::EnableDithering(BOOL bEnable)
{
  return(Get3DDevice()->SetRenderState(D3DRENDERSTATE_DITHERENABLE, (DWORD)bEnable));
}

// ===========================================================================
INL BOOL pc3D::EnableSpecularHighLights(BOOL bEnable)
{
  return(Get3DDevice()->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, (DWORD)bEnable));
}

// ===========================================================================
INL BOOL pc3D::EnablePerspectiveCorrection(BOOL bEnable)
{
  return(Get3DDevice()->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)bEnable));
}

// ===========================================================================
INL BOOL pc3D::EnableAlphaBlending(BOOL bEnable)
{
  return(Get3DDevice()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)bEnable));
}

// ===========================================================================
INL BOOL pc3D::EnableAntiAliasing(BOOL bEnable)
{
  D3DDevice* p3DDevice = Get3DDevice();
  D3DDEVICEDESC* d3dDeviceCaps = p3DDevice->GetCaps();

  if (!bEnable)
    return(p3DDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_NONE));
    
  if (d3dDeviceCaps->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT)
  {
    return(p3DDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS,
                                     D3DANTIALIAS_SORTINDEPENDENT));
  }
  return(FALSE);
}

// ===========================================================================
INL BOOL pc3D::EnableFog(BOOL bEnable)
{
  return(Get3DDevice()->SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD)bEnable));
}

// ===========================================================================
INL BOOL pc3D::EnableColourKeying(BOOL bEnable)
{
  return(Get3DDevice()->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, (DWORD)bEnable));
}

// ===========================================================================
INL BOOL pc3D::EnableBilinearFiltering(DWORD dwStage, BOOL bEnable)
{
  D3DDevice* p3DDevice = Get3DDevice(); ASSERT(NULL != p3DDevice);
  if (bEnable)
  {
    m_TxtrFilters.byMinFilter = D3DTFN_LINEAR;
    m_TxtrFilters.byMagFilter = D3DTFG_LINEAR;
  }
  else
  {
    m_TxtrFilters.byMinFilter = D3DTFN_POINT;
    m_TxtrFilters.byMagFilter = D3DTFG_POINT;
  }

  return(p3DDevice->SetTextureStageState(dwStage, D3DTSS_MAGFILTER,
                                         m_TxtrFilters.byMagFilter) &&
         p3DDevice->SetTextureStageState(dwStage, D3DTSS_MINFILTER,
                                         m_TxtrFilters.byMinFilter));
}

// ===========================================================================
INL BOOL pc3D::EnableMipMapFiltering(DWORD dwStage, BOOL bEnable)
{
  if (bEnable)
    m_TxtrFilters.byMipMapFilter = D3DTFP_POINT;
  else
    m_TxtrFilters.byMipMapFilter = D3DTFP_NONE;

  return(Get3DDevice()->SetTextureStageState(dwStage, D3DTSS_MIPFILTER,
                                             m_TxtrFilters.byMipMapFilter));
}

// ===========================================================================
INL BOOL pc3D::EnableTrilinearMipMapFiltering(DWORD dwStage, BOOL bEnable)
{
  if (bEnable)
    m_TxtrFilters.byMipMapFilter = D3DTFP_LINEAR;
  else
    m_TxtrFilters.byMipMapFilter = D3DTFP_NONE;

  return(Get3DDevice()->SetTextureStageState(dwStage, D3DTSS_MIPFILTER,
                                             m_TxtrFilters.byMipMapFilter));
}


// ===========================================================================
// MACROS ====================================================================
// ===========================================================================

#define SET_SHADEMODE(mode)             pc3D::SetShadeMode(mode)
#define SET_FILLMODE(mode)              pc3D::SetFillMode(mode)

#define ENABLE_DITHERING(b)             pc3D::EnableDithering(b)
#define ENABLE_SPECULARHIGHLIGHTS(b)    pc3D::EnableSpecularHighLights(b)
#define ENABLE_PERSPECTIVECORRECTION(b) pc3D::EnablePerspectiveCorrection(b)
#define ENABLE_ANTIALIASING(b)          pc3D::EnableAntiAliasing(b)

#define ENABLE_BILINEARFILTERING(s, b)  pc3D::EnableBilinearFiltering(s, b)
#define ENABLE_MIPMAPFILTERING(s, b)    pc3D::EnableMipMapFiltering(s, b)
#define ENABLE_TRILINEARMIPMAPFILTERING(s, b) pc3D::EnableTrilinearMipMapFiltering(s, b)

#define ENABLE_ALPHABLENDING(b)         pc3D::EnableAlphaBlending(b)
#define ENABLE_FOG(b)                   pc3D::EnableFog(b)
#define ENABLE_COLOURKEYING(b)          pc3D::EnableColourKeying(b)

extern BOOL g_HMD_UseDefaultShadingMode;
#define FORCE_HMD_USEDEFAULTSHADING(b)  g_HMD_UseDefaultShadingMode = (b)


#endif // INC_RENDERSETTINGSINL
// ===========================================================================
