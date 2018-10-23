// ===========================================================================
// Module:      DDrawDrv.inl
//
// Description: DirectDraw Driver class inline functions.
//
// Created:     May 31, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================


// ===========================================================================
// INLINE: DDrawDriver member functions ======================================
// ===========================================================================

static HRESULT hReturned = NULL;

// ===========================================================================
INL PDDSURFDESC DDrawDriver::GetDisplayMode(int nIdx) const
{ 
  ASSERT(0 <= nIdx && m_nNumDisplayModes > nIdx);
  return(m_apDisplayModes[nIdx]);
}

// ===========================================================================
INL D3DDevice* DDrawDriver::GetD3DDevice(int nIdx) const
{
  ASSERT(0 <= nIdx && m_nNum3DDevices > nIdx);
  return(m_ap3DDevices[nIdx]);
}

// ===========================================================================
INL D3DDevice* DDrawDriver::RemoveD3DDevice(int nIdx)
{
  D3DDevice* pD3DDevice = NULL;
  if (0 <= nIdx && m_nNum3DDevices > nIdx)
  { pD3DDevice = m_ap3DDevices[nIdx]; m_ap3DDevices[nIdx] = NULL; }
  return(pD3DDevice);
}

// ===========================================================================
INL BOOL DDrawDriver::GUIDMatches(LPGUID pGUID) const
{
  if (pGUID && m_pGUID) return(0 == memcmp(pGUID, m_pGUID, sizeof(GUID)));
  return(TRUE); // Both GUIDs are NULL (both designate the default driver)
}

// ===========================================================================
INL BOOL DDrawDriver::GetDeviceID(DDDEVICEIDENTIFIER& ddDeviceID)
{
  ASSERT(NULL != m_pDDraw); if (!m_pDDraw) return(FALSE);
  ZeroMemory(&ddDeviceID, sizeof(DDDEVICEIDENTIFIER));
  hReturned = m_pDDraw->GetDeviceIdentifier(&ddDeviceID, 0);
  RETURN_ON_ERROR(hReturned, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL DDrawDriver::GetDisplayMode(DDSURFDESC& ddSurfDesc)
{
  ASSERT(NULL != m_pDDraw); if (!m_pDDraw) return(FALSE);
  ZeroMemory(&ddSurfDesc, sizeof(DDSURFDESC));
  ddSurfDesc.dwSize = sizeof(DDSURFDESC);
  hReturned = m_pDDraw->GetDisplayMode(&ddSurfDesc);
  RETURN_ON_ERROR(hReturned, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL DDrawDriver::HasHardwareBlitter() const
{
  return((BOOL)(m_HardwareCaps.dwCaps & DDCAPS_BLT));
}

// ===========================================================================
INL BOOL DDrawDriver::GetAvailableVidMem(DDSURFCAPS& surfCaps, DWORD& dwTotal,
                                         DWORD& dwFree)
{
  ASSERT(NULL != m_pDDraw); if (!m_pDDraw) return(FALSE);
  hReturned = m_pDDraw->GetAvailableVidMem(&surfCaps, &dwTotal, &dwFree);
  RETURN_ON_ERROR(hReturned, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL DDrawDriver::FlipToGDISurface()
{
  ASSERT(NULL != m_pDDraw); if (!m_pDDraw) return(FALSE);
  hReturned = m_pDDraw->FlipToGDISurface();
  RETURN_ON_ERROR(hReturned, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL DDrawDriver::SetCooperativeLevel(DWORD dwFlags)
{
  ASSERT(NULL != m_pDDraw); if (!m_pDDraw) return(FALSE);
  hReturned = m_pDDraw->SetCooperativeLevel(m_hWnd, dwFlags);
  RETURN_ON_ERROR(hReturned, FALSE);
  return(TRUE);
}

// ===========================================================================
INL PDDSURFACE DDrawDriver::GetSurfaceFromDC(HDC hdc)
{
  ASSERT(NULL != m_pDDraw); if (!m_pDDraw) return(FALSE);
  PDDSURFACE pSurface = NULL;
  hReturned = m_pDDraw->GetSurfaceFromDC(hdc, &pSurface);
  RETURN_ON_ERROR(hReturned, FALSE);
  return(pSurface);
}

// ===========================================================================
INL BOOL DDrawDriver::RestoreDisplayMode()
{
  ASSERT(NULL != m_pDDraw); if (!m_pDDraw) return(FALSE);
  hReturned = m_pDDraw->RestoreDisplayMode();
  RETURN_ON_ERROR(hReturned, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL DDrawDriver::RestoreAllSurfaces()
{
  ASSERT(NULL != m_pDDraw); if (!m_pDDraw) return(FALSE);
  hReturned = m_pDDraw->RestoreAllSurfaces();
  RETURN_ON_ERROR(hReturned, FALSE);
  return(TRUE);
}

// ===========================================================================
// INLINE: Direct3D wrapper functions ========================================
// ===========================================================================

// ===========================================================================
INL BOOL DDrawDriver::EvictManagedTextures()
{
  ASSERT(NULL != m_pD3D); if (!m_pD3D) return(FALSE);
  hReturned = m_pD3D->EvictManagedTextures();
  RETURN_ON_ERROR(hReturned, FALSE);
  return(TRUE);
}

// ===========================================================================
// End Of File ===============================================================
// ===========================================================================
