// ===========================================================================
// Module:      Surface.inl
//
// Description: Surface class inline functions.
//
// Created:     June 16, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_SURFACEINL
#define INC_SURFACEINL

static HRESULT g_hSurfResult = NULL;


// ===========================================================================
// INLINE: Surface member functions ==========================================
// ===========================================================================

// Default Constructor =======================================================
INL Surface::Surface() : m_pDDSurface(NULL), m_pD3DTexture(NULL),
                         m_bIsPrimary(FALSE), m_bIsTexture(FALSE), 
                         m_bHasPalette(FALSE), m_bInVideoMem(FALSE)
{
  m_Size.cx = m_Size.cy = 0;
}

// ===========================================================================
INL Surface::Surface(PDDSURFACE pDDSurface)
            : m_pDDSurface(NULL), m_pD3DTexture(NULL), m_bIsPrimary(FALSE),
              m_bIsTexture(FALSE), m_bHasPalette(FALSE), m_bInVideoMem(FALSE)
{ VERIFY(Create(pDDSurface));                                               }

// ===========================================================================
INL Surface::Surface(Surface& surface, BOOL bIsTexture /*= FALSE*/,
                     BOOL bUseVideoMem /*= FALSE*/)
            : m_pDDSurface(NULL), m_pD3DTexture(NULL), m_bIsPrimary(FALSE),
              m_bIsTexture(FALSE), m_bHasPalette(FALSE), m_bInVideoMem(FALSE)
{
  VERIFY(Create(surface, bIsTexture, bUseVideoMem));
}

// ===========================================================================
INL Surface::Surface(UINT nWidth, UINT nHeight, BOOL bUseVideoMem /*= FALSE*/)
            : m_pDDSurface(NULL), m_pD3DTexture(NULL), m_bIsPrimary(FALSE),
              m_bIsTexture(FALSE), m_bHasPalette(FALSE), m_bInVideoMem(FALSE)
{
  VERIFY(Create(nWidth, nHeight, bUseVideoMem));
}

// ===========================================================================
INL Surface::Surface(UINT nWidth, UINT nHeight, BYTE byClrDepth, 
                     UINT nAlphaBits /*= 0*/,
                     PALETTEENTRY* pPalEntries /*= NULL*/)
            : m_pDDSurface(NULL), m_pD3DTexture(NULL), m_bIsPrimary(FALSE),
              m_bIsTexture(FALSE), m_bHasPalette(FALSE), m_bInVideoMem(FALSE)
{ VERIFY(Create(nWidth, nHeight, byClrDepth, nAlphaBits, pPalEntries));     }

// ===========================================================================
INL Surface::Surface(HBITMAP hBitmap, BOOL bIsTexture /*= TRUE*/,
                     BOOL bUseVideoMem /*= FALSE*/)
            : m_pDDSurface(NULL), m_pD3DTexture(NULL), m_bIsPrimary(FALSE),
              m_bIsTexture(FALSE), m_bHasPalette(FALSE), m_bInVideoMem(FALSE)
{ VERIFY(Create(hBitmap, bIsTexture, bUseVideoMem));                        }

// ===========================================================================
INL Surface::~Surface() 
{
  if (m_pDDSurface)
  {
    TRACE("\tSurface begin destroyed from ~Surface destructor\r\n");
    TRACE("\tSurface should have been released previously\r\n");
    ReleaseSurface("~Surface(): ", __FILE__, __LINE__);
  }
}

// ===========================================================================
INL PDDSURFACE Surface::Detach()
{
  RELEASEPOINTER(m_pD3DTexture);
  PDDSURFACE pSurface = m_pDDSurface;
  m_pDDSurface        = NULL;
  m_Size.cx           = m_Size.cy = 0;
  return(pSurface);
}

// ===========================================================================
INL PDDSURFACE Surface::GetDDSurface() const
{ return(m_pDDSurface);                                                     }

// ===========================================================================
INL PD3DTEXTURE Surface::GetD3DTexture()
{ 
  if (!m_pD3DTexture) ObtainD3DTexture();
  return(m_pD3DTexture);
}

// ===========================================================================
INL DWORD Surface::GetBitDepth()
{
  DDPIXELFORMAT ddpf;
  GetPixelFormat(ddpf);
  return(ddpf.dwRGBBitCount);
}

// ===========================================================================
INL void Surface::GetRect(RECT& rct) const
{ rct.left = rct.top = 0; rct.right = m_Size.cx; rct.bottom = m_Size.cy;    }

// ===========================================================================
INL void Surface::GetSize(SIZE& size) const
{ size = m_Size;                                                            }

// ===========================================================================
INL void Surface::GetSize(DWORD& dwWidth, DWORD& dwHeight) const
{ dwWidth = m_Size.cx; dwHeight = m_Size.cy;                                }

// ===========================================================================
BOOL Surface::IsPrimary()
{
  if (!FLAGINITIALIZED(m_bIsPrimary)) DetermineIfPrimary();  
  return(FLAGISSET(m_bIsPrimary));
}

// ===========================================================================
BOOL Surface::IsTexture()
{
  if (!FLAGINITIALIZED(m_bIsTexture)) DetermineIfTexture();  
  return(FLAGISSET(m_bIsTexture));
}

// ===========================================================================
INL BOOL Surface::IsMipMap()
{
  if (!FLAGINITIALIZED(m_bIsMipMap)) DetermineIfMipMapped();  
  return(FLAGISSET(m_bIsMipMap));
}

// ===========================================================================
INL BOOL Surface::IsPalettized()
{
  if (!FLAGINITIALIZED(m_bHasPalette)) DetermineIfPalettized();  
  return(FLAGISSET(m_bHasPalette));
}

// ===========================================================================
INL BOOL Surface::IsInVideoMem()
{
  if (!IsTexture() && !FLAGINITIALIZED(m_bInVideoMem)) DetermineIfInVideoMem();  
  return(FLAGISSET(m_bInVideoMem));
}

// ===========================================================================
INL BOOL Surface::IsInitialized()
{ return(NULL != m_pDDSurface);                                             }

// ===========================================================================
INL BOOL Surface::AddAttachedSurface(Surface& surface)
{ return(AddAttachedSurface(surface.m_pDDSurface));                         }

// ===========================================================================
INL BOOL Surface::DeleteAttachedSurface(DWORD dwFlags, Surface& surface)
{ return(DeleteAttachedSurface(dwFlags, surface.m_pDDSurface));             }

// ===========================================================================
INL BOOL Surface::GetAttachedSurface(DDSURFCAPS& ddsCaps, Surface& surface)
{
  PDDSURFACE pDDSurface = GetAttachedSurface(ddsCaps);
  if (!pDDSurface) return(FALSE);
  return(surface.Create(pDDSurface));
}

// ===========================================================================
INL BOOL Surface::SolidRect(RECT* pRect, DWORD dwClr /*= 0*/)
{ 
  ASSERT(NULL != m_pDDSurface);
  DDBLTFX ddBltFx;
  ZeroMemory(&ddBltFx, sizeof(DDBLTFX));
  ddBltFx.dwSize      = sizeof(DDBLTFX);
  ddBltFx.dwFillColor = dwClr;
  return(!FAILED(m_pDDSurface->Blt(pRect, NULL, NULL, DDBLT_COLORFILL | 
                                   DDBLT_WAIT, &ddBltFx)));
}

// ===========================================================================
INL BOOL Surface::ClearSurface(DWORD dwClr /*= 0*/)
{ return(SolidRect(NULL, dwClr));                                           }

// ===========================================================================
INL void Surface::ReleaseSurface(LPSTR szVarname /*= NULL*/,
                                 LPSTR szFile /*= NULL*/, DWORD dwLine /*= 0*/)
{
#ifdef _DEBUG

  if (m_pD3DTexture)
  {
    LASTREFCOUNT = m_pD3DTexture->Release();
    TRACE("%s(%d) : Release of surface texture %s(%08X), ref count: %d\r\n",
          szFile, dwLine, szVarname, m_pD3DTexture, LASTREFCOUNT);
    m_pD3DTexture = NULL;
  }
  if (m_pDDSurface)
  {
    LASTREFCOUNT = m_pDDSurface->Release();
    TRACE("%s(%d) : Release of surface %s(%08X), ref count: %d\r\n",
          szFile, dwLine, szVarname, m_pDDSurface, LASTREFCOUNT);
    m_pDDSurface = NULL;
  }

#else // _DEBUG

  RELEASEPOINTER(m_pD3DTexture);
  RELEASEPOINTER(m_pDDSurface);

#endif // _DEBUG

  if (!m_pDDSurface)
  {
    m_Size.cx = m_Size.cy = 0;
    m_bIsPrimary = m_bIsTexture = m_bIsMipMap = m_bHasPalette = m_bInVideoMem = 0;
  }
}


// ===========================================================================
// IDirectDrawSurface4 wrappers ==============================================
// ===========================================================================


// ===========================================================================
INL BOOL Surface::AddAttachedSurface(PDDSURFACE pSurface)
{ 
  ASSERT(NULL != m_pDDSurface);
  g_hSurfResult = m_pDDSurface->AddAttachedSurface(pSurface);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL Surface::BltBatch(DDBLTBATCH& ddBltBatch, DWORD dwCount, DWORD dwFlags)
{ return(FALSE); /*Hasn't been implemented in DirectDraw yet */             }

// ===========================================================================
INL BOOL Surface::DeleteAttachedSurface(DWORD dwFlags, PDDSURFACE pDDSAttached)
{
  ASSERT(NULL != m_pDDSurface); 
  g_hSurfResult = m_pDDSurface->DeleteAttachedSurface(dwFlags, pDDSAttached);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL Surface::EnumAttachedSurfaces(LPVOID lpContext, 
                                       LPDDENUMSURFACESCALLBACK2 lpEnumSurfacesCallback)
{
  ASSERT(NULL != m_pDDSurface); 
  g_hSurfResult = m_pDDSurface->EnumAttachedSurfaces(lpContext,
                                                     lpEnumSurfacesCallback);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL Surface::Flip(DWORD dwFlags /*= 0*/,
                       PDDSURFACE pDDSTargetOverride /*= NULL*/)
{
  ASSERT(NULL != m_pDDSurface); 
  g_hSurfResult = m_pDDSurface->Flip(pDDSTargetOverride, dwFlags);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL PDDSURFACE Surface::GetAttachedSurface(DDSURFCAPS& ddsCaps)
{
  ASSERT(NULL != m_pDDSurface);
  PDDSURFACE pSurface = NULL;
  g_hSurfResult = m_pDDSurface->GetAttachedSurface(&ddsCaps, &pSurface);
  RETURN_ON_ERROR(g_hSurfResult, NULL);
  if (pSurface) pSurface->Release(); // Dereference it so we won't have to later
  return(pSurface);
}

// ===========================================================================
INL HRESULT Surface::GetBltStatus(DWORD dwFlags)
{
  ASSERT(NULL != m_pDDSurface); 
  return(m_pDDSurface->GetBltStatus(dwFlags));
}

// ===========================================================================
INL BOOL Surface::GetCaps(DDSURFCAPS& ddsCaps)
{
  ASSERT(NULL != m_pDDSurface);
  ZeroMemory(&ddsCaps, sizeof(DDSURFCAPS));
  g_hSurfResult = m_pDDSurface->GetCaps(&ddsCaps);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL PDDCLIPPER Surface::GetClipper()
{
  ASSERT(NULL != m_pDDSurface); 
  PDDCLIPPER pClipper = NULL;
  g_hSurfResult = m_pDDSurface->GetClipper(&pClipper);
  RETURN_ON_ERROR(g_hSurfResult, NULL);
  if (pClipper) pClipper->Release(); // Dereference it so we won't have to later
  return(pClipper);
}

// ===========================================================================
INL BOOL Surface::GetColorKey(DWORD dwFlags, DDCOLORKEY& ddColorKey)
{
  ASSERT(NULL != m_pDDSurface); 
  g_hSurfResult = m_pDDSurface->GetColorKey(dwFlags, &ddColorKey);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL HDC Surface::GetDC()
{
  ASSERT(NULL != m_pDDSurface);
  HDC hSurfaceDC = NULL;
  g_hSurfResult = m_pDDSurface->GetDC(&hSurfaceDC);
  RETURN_ON_ERROR(g_hSurfResult, NULL);
  return(hSurfaceDC);
}

// ===========================================================================
INL HRESULT Surface::GetFlipStatus(DWORD dwFlags)
{
  ASSERT(NULL != m_pDDSurface); 
  return(m_pDDSurface->GetFlipStatus(dwFlags));
}

// ===========================================================================
INL PDDPALETTE Surface::GetPalette()
{
  ASSERT(NULL != m_pDDSurface); 
  PDDPALETTE pPalette = NULL;
  g_hSurfResult = m_pDDSurface->GetPalette(&pPalette);
  RETURN_ON_ERROR(g_hSurfResult, NULL);
  if (pPalette) pPalette->Release(); // Dereference it now so we won't have to later
  return(pPalette);
}

// ===========================================================================
INL BOOL Surface::GetPixelFormat(DDPIXELFORMAT& ddPixelFormat)
{
  ASSERT(NULL != m_pDDSurface); 
  ZeroMemory(&ddPixelFormat, sizeof(DDPIXELFORMAT));
  ddPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
  g_hSurfResult = m_pDDSurface->GetPixelFormat(&ddPixelFormat);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL Surface::GetSurfaceDesc(DDSURFDESC& surfDesc)
{
  ASSERT(NULL != m_pDDSurface); 
  ZeroMemory(&surfDesc, sizeof(DDSURFDESC));
  surfDesc.dwSize = sizeof(DDSURFDESC);
  g_hSurfResult = m_pDDSurface->GetSurfaceDesc(&surfDesc);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL Surface::IsLost()
{
  ASSERT(NULL != m_pDDSurface); 
  g_hSurfResult = m_pDDSurface->IsLost();
  return(DDERR_SURFACELOST == g_hSurfResult);
}

// ===========================================================================
INL BOOL Surface::Lock(DDSURFDESC& surfDesc, DWORD dwFlags /*= 0*/,
                       LPRECT prctDest /*= NULL*/, HANDLE hEvent /*= NULL*/)
{
  ASSERT(NULL != m_pDDSurface); 
  ZeroMemory(&surfDesc, sizeof(DDSURFDESC));
  surfDesc.dwSize = sizeof(DDSURFDESC);
  g_hSurfResult = m_pDDSurface->Lock(prctDest, &surfDesc, dwFlags, hEvent);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL Surface::PageLock(DWORD dwFlags /*= 0*/)
{
  ASSERT(NULL != m_pDDSurface); 
  g_hSurfResult = m_pDDSurface->PageLock(dwFlags);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL Surface::PageUnlock(DWORD dwFlags /*= 0*/)
{
  ASSERT(NULL != m_pDDSurface); 
  g_hSurfResult = m_pDDSurface->PageUnlock(dwFlags);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL Surface::Unlock(LPRECT prct /*= NULL*/)
{
  ASSERT(NULL != m_pDDSurface); 
  g_hSurfResult = m_pDDSurface->Unlock(prct);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL Surface::Restore()
{
  ASSERT(NULL != m_pDDSurface); 
  g_hSurfResult = m_pDDSurface->Restore();
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL Surface::ReleaseDC(HDC hSurfaceDC)
{
  ASSERT(NULL != m_pDDSurface); 
  g_hSurfResult = m_pDDSurface->ReleaseDC(hSurfaceDC);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL Surface::SetClipper(PDDCLIPPER pClipper)
{
  ASSERT(NULL != m_pDDSurface); 
  g_hSurfResult = m_pDDSurface->SetClipper(pClipper);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL Surface::SetColorKey(DWORD dwFlags, DDCOLORKEY& ddColorKey)
{
  ASSERT(NULL != m_pDDSurface); 
  g_hSurfResult = m_pDDSurface->SetColorKey(dwFlags, &ddColorKey);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL Surface::SetPalette(PDDPALETTE pPalette)
{
  ASSERT(NULL != m_pDDSurface); 
  g_hSurfResult = m_pDDSurface->SetPalette(pPalette);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}

// ===========================================================================
INL BOOL Surface::SetSurfaceDesc(DDSURFDESC& ddsd, DWORD dwFlags /*= 0*/)
{
  ASSERT(NULL != m_pDDSurface); 
  g_hSurfResult = m_pDDSurface->SetSurfaceDesc(&ddsd, dwFlags);
  RETURN_ON_ERROR(g_hSurfResult, FALSE);
  return(TRUE);
}


#endif // INC_SURFACEINL
// ===========================================================================
