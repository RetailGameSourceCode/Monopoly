// ===========================================================================
// Module:      D3DDevice.inl
//
// Description: Direct3D device class inline functions
//
// Created:     April 30, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================

static HRESULT g_hResult = NULL;

// ===========================================================================
// INLINE: D3DDevice member functions ========================================
// ===========================================================================

// ===========================================================================
INL BOOL D3DDevice::GUIDMatches(GUID& guid) const
{ return(0 == memcmp(&guid, &m_GUID, sizeof(GUID)));                        }

// ===========================================================================
INL void D3DDevice::ReleaseD3DDevice()
{ RELEASEPOINTER(m_pD3DDevice);                                             }

// ===========================================================================
INL DDPIXELFORMAT* D3DDevice::GetZBufferFormat(int nIdx) const
{
  if (0 <= nIdx && m_apZBufFormats.GetSize() > nIdx) 
    return(m_apZBufFormats.GetAt(nIdx));
  return(NULL);
}

// ===========================================================================
INL DDPIXELFORMAT* D3DDevice::GetTextureFormat(int nIdx) const
{
  if (0 <= nIdx && m_apTextrFormats.GetSize() > nIdx) 
    return(m_apTextrFormats.GetAt(nIdx));
  return(NULL);
}

// ===========================================================================
INL DWORD FlagToBitDepth(DWORD dwFlag)
{
  switch (dwFlag)
  {
    case DDBD_1:    return(1);
    case DDBD_2:    return(2);
    case DDBD_4:    return(4);
    case DDBD_8:    return(8);
    case DDBD_16:   return(16);
    case DDBD_24:   return(24);
    case DDBD_32:   return(32);
  }
  return(0);
}

// ===========================================================================
INL BOOL D3DDevice::AddViewport(PVIEWPORT pViewport)
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  g_hResult = m_pD3DDevice->AddViewport(pViewport);
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::BeginScene()
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  g_hResult = m_pD3DDevice->BeginScene();
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::RemoveViewport(PVIEWPORT pViewport)
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  g_hResult = m_pD3DDevice->DeleteViewport(pViewport);
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::DrawIndexedPrimitive(D3DPRIMITIVETYPE primType, 
                                         DWORD dwVertexType, LPVOID lpvVertices,
                                         DWORD dwVertexCount, LPWORD lpwIndices,
                                         DWORD dwIndexCount, DWORD dwFlags)
{
  ASSERT(NULL != m_pD3DDevice);
  g_hResult = m_pD3DDevice->DrawIndexedPrimitive(primType, dwVertexType, 
                                                 lpvVertices, dwVertexCount,
                                                 lpwIndices, dwIndexCount, 
                                                 dwFlags);
  RETURN_ON_ERROR(g_hResult, FALSE);
#ifdef RECORD_3DSTATS
  if (0 == g_llStartTime) ::QueryPerformanceCounter((LARGE_INTEGER*)&g_llStartTime);
  ADDTRIANGLES(primType, dwIndexCount);
#endif
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::DrawIndexedPrimitiveStrided(D3DPRIMITIVETYPE primType,  
                                                DWORD dwVertexType, 
                                                PD3DSTRIDEDATA pVertexArray, 
                                                DWORD dwVertexCount, 
                                                LPWORD lpwIndices,
                                                DWORD dwIndexCount,
                                                DWORD dwFlags)
{
  ASSERT(NULL != m_pD3DDevice);
  g_hResult = m_pD3DDevice->DrawIndexedPrimitiveStrided(primType, dwVertexType, 
                                                        pVertexArray, 
                                                        dwVertexCount, 
                                                        lpwIndices, 
                                                        dwIndexCount, dwFlags);
  RETURN_ON_ERROR(g_hResult, FALSE);
#ifdef RECORD_3DSTATS
  if (0 == g_llStartTime) ::QueryPerformanceCounter((LARGE_INTEGER*)&g_llStartTime);
  ADDTRIANGLES(primType, dwIndexCount);
#endif
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE primType,
                                           PD3DVTXBUFFER pVertexBuffer,
                                           LPWORD lpwIndices,
                                           DWORD dwIndexCount,
                                           DWORD dwFlags)
{
  ASSERT(NULL != m_pD3DDevice);
  g_hResult = m_pD3DDevice->DrawIndexedPrimitiveVB(primType, pVertexBuffer, 
                                                   lpwIndices, dwIndexCount,
                                                   dwFlags);
  RETURN_ON_ERROR(g_hResult, FALSE);
#ifdef RECORD_3DSTATS
  if (0 == g_llStartTime) ::QueryPerformanceCounter((LARGE_INTEGER*)&g_llStartTime);
  ADDTRIANGLES(primType, dwIndexCount);
#endif
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::DrawPrimitive(D3DPRIMITIVETYPE primType, DWORD dwVertexType,
                                  LPVOID lpvVertices, DWORD dwVertexCount,
                                  DWORD dwFlags)
{
  ASSERT(NULL != m_pD3DDevice);
  g_hResult = m_pD3DDevice->DrawPrimitive(primType, dwVertexType, lpvVertices,
                                          dwVertexCount, dwFlags);
  RETURN_ON_ERROR(g_hResult, FALSE);
#ifdef RECORD_3DSTATS
  if (0 == g_llStartTime) ::QueryPerformanceCounter((LARGE_INTEGER*)&g_llStartTime);
  ADDTRIANGLES(primType, dwVertexCount);
#endif
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::DrawPrimitiveStrided(D3DPRIMITIVETYPE primType,
                                         DWORD dwVertexType,
                                         PD3DSTRIDEDATA lpVertexArray,  
                                         DWORD  dwVertexCount, DWORD dwFlags)
{
  ASSERT(NULL != m_pD3DDevice);
  g_hResult = m_pD3DDevice->DrawPrimitiveStrided(primType, dwVertexType, 
                                                 lpVertexArray, dwVertexCount,
                                                 dwFlags);
  RETURN_ON_ERROR(g_hResult, FALSE);
#ifdef RECORD_3DSTATS
  if (0 == g_llStartTime) ::QueryPerformanceCounter((LARGE_INTEGER*)&g_llStartTime);
  ADDTRIANGLES(primType, dwVertexCount);
#endif
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::DrawPrimitiveVB(D3DPRIMITIVETYPE primType,
                                    PD3DVTXBUFFER pVertexBuffer,
                                    DWORD dwStartVertex, DWORD dwNumVertices,
                                    DWORD dwFlags)
{
  ASSERT(NULL != m_pD3DDevice);
  g_hResult = m_pD3DDevice->DrawPrimitiveVB(primType, pVertexBuffer,
                                            dwStartVertex, dwNumVertices,
                                            dwFlags);
  RETURN_ON_ERROR(g_hResult, FALSE);
#ifdef RECORD_3DSTATS
  if (0 == g_llStartTime) ::QueryPerformanceCounter((LARGE_INTEGER*)&g_llStartTime);
  ADDTRIANGLES(primType, dwNumVertices);
#endif // RECORD_3DSTATS
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::EndScene()
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  g_hResult = m_pD3DDevice->EndScene();
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::GetClipStatus(D3DCLIPSTATUS& d3dClipStatus)
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  g_hResult = m_pD3DDevice->GetClipStatus(&d3dClipStatus);
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL PVIEWPORT D3DDevice::GetCurrentViewport()
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  PVIEWPORT pViewport = NULL;
  g_hResult = m_pD3DDevice->GetCurrentViewport(&pViewport);
  RETURN_ON_ERROR(g_hResult, NULL);
  return(pViewport);
}

// ===========================================================================
INL PVIEWPORT D3DDevice::NextViewport(PVIEWPORT pViewport,  DWORD dwFlags)
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  PVIEWPORT pNextViewport = NULL;
  g_hResult = m_pD3DDevice->NextViewport(pViewport, &pNextViewport, dwFlags);
  RETURN_ON_ERROR(g_hResult, NULL);
  return(pNextViewport);
}

// ===========================================================================
INL BOOL D3DDevice::SetCurrentViewport(PVIEWPORT pViewport)
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  g_hResult = m_pD3DDevice->SetCurrentViewport(pViewport);
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL PDDSURFACE D3DDevice::GetRenderTarget()
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  PDDSURFACE pSurface = NULL;
  g_hResult = m_pD3DDevice->GetRenderTarget(&pSurface);
  RETURN_ON_ERROR(g_hResult, NULL);
  return(pSurface);
}

// ===========================================================================
INL PD3DTEXTURE D3DDevice::GetTexture(DWORD dwStage)
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  PD3DTEXTURE pTexture = NULL;
  g_hResult = m_pD3DDevice->GetTexture(dwStage, &pTexture);
  RETURN_ON_ERROR(g_hResult, NULL);
  return(pTexture);
}

// ===========================================================================
INL DWORD D3DDevice::GetLightState(D3DLIGHTSTATETYPE eLightStateType)
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  DWORD dwValue = 0;
  g_hResult = m_pD3DDevice->GetLightState(eLightStateType, &dwValue);
  RETURN_ON_ERROR(g_hResult, 0);
  return(dwValue);
}

// ===========================================================================
INL DWORD D3DDevice::GetRenderState(D3DRENDERSTATETYPE eRenderStateType)
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  DWORD dwValue = 0;
  g_hResult = m_pD3DDevice->GetRenderState(eRenderStateType, &dwValue);
  RETURN_ON_ERROR(g_hResult, 0);
  return(dwValue);
}

// ===========================================================================
INL DWORD D3DDevice::GetTextureStageState(DWORD dwStage, 
                                          D3DTEXTURESTAGESTATETYPE eState)
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  DWORD dwValue = 0;
  g_hResult = m_pD3DDevice->GetTextureStageState(dwStage, eState, &dwValue);
  RETURN_ON_ERROR(g_hResult, 0);
  return(dwValue);
}

// ===========================================================================
INL BOOL D3DDevice::GetTransform(D3DTRANSFORMSTATETYPE eTransformStateType, 
                                 D3DMATRIX& d3dMatrix)
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  g_hResult = m_pD3DDevice->GetTransform(eTransformStateType, &d3dMatrix);
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(FALSE);
}

// ===========================================================================
INL BOOL D3DDevice::MultiplyTransform(D3DTRANSFORMSTATETYPE eTransformStateType,
                                      LPD3DMATRIX pD3DMatrix)
{
  ASSERT(NULL != m_pD3DDevice);
  g_hResult = m_pD3DDevice->MultiplyTransform(eTransformStateType, pD3DMatrix);
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(FALSE);
}

// ===========================================================================
INL BOOL D3DDevice::SetClipStatus(LPD3DCLIPSTATUS pD3DClipStatus)
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  g_hResult = m_pD3DDevice->SetClipStatus(pD3DClipStatus);
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::SetLightState(D3DLIGHTSTATETYPE eLightStateType, 
                                  DWORD dwLightState)
{
  ASSERT(NULL != m_pD3DDevice);
  g_hResult = m_pD3DDevice->SetLightState(eLightStateType, dwLightState);
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::SetRenderState(D3DRENDERSTATETYPE eRenderStateType, 
                                   DWORD dwRenderState)
{
  ASSERT(NULL != m_pD3DDevice);
  g_hResult = m_pD3DDevice->SetRenderState(eRenderStateType, dwRenderState);
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::SetRenderTarget(PDDSURFACE pRenderTarget)
{
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);
  g_hResult = m_pD3DDevice->SetRenderTarget(pRenderTarget, 0);
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::SetTexture(DWORD dwStage, PD3DTEXTURE pTexture)
{
  ASSERT(NULL != m_pD3DDevice);
  g_hResult = m_pD3DDevice->SetTexture(dwStage, pTexture);
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::SetTextureStageState(DWORD dwStage,
                                         D3DTEXTURESTAGESTATETYPE eState,
                                         DWORD dwValue)
{
  ASSERT(NULL != m_pD3DDevice);
  g_hResult = m_pD3DDevice->SetTextureStageState(dwStage, eState, dwValue);
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL D3DDevice::SetTransform(D3DTRANSFORMSTATETYPE eTransformStateType,  
                                 LPD3DMATRIX pD3DMatrix)
{
  ASSERT(NULL != m_pD3DDevice);
  g_hResult = m_pD3DDevice->SetTransform(eTransformStateType, pD3DMatrix);
  RETURN_ON_ERROR(g_hResult, FALSE);
  return(TRUE);
}


// ===========================================================================
// End Of File ===============================================================
// ===========================================================================
