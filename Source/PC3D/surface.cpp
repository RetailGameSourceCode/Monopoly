// ===========================================================================
// Module:      Surface.cpp
//
// Description: Surface class implementation.  Encapsulates a DirectDraw surface
//              as well as a Direct3D texture.
//
// Created:     June 16, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#include "PC3DHdr.h"
#include "Surface.h"

#include "DDrawDrv.h"


// ===========================================================================
// Local prototypes ==========================================================
// ===========================================================================

extern "C" Surface* CreateNewSurface(HBITMAP hBmp, BOOL bIsTexture = FALSE,
                                     BOOL bUseVideoMem = FALSE);

// ===========================================================================
// Surface class =============================================================
// ===========================================================================

// Initialize our static member variable
DDrawDriver* Surface::m_pDDDriver = NULL;


// ===========================================================================
// Function:    Constuctor
//
// Description: Constructs a new surface from the given bitmap tag
//
// Params:      pBitmap:      The bitmap tag to create the surface from
//              bIsTexture:   Are we creating a texture?
//              bUseVideoMem: Are we supposed to use video memory? (Ignored if
//                            creating a texture)
//
// Returns:     void
//
// Comments:    
// ===========================================================================
Surface::Surface(Tag* pBitmap, BOOL bIsTexture /*= FALSE*/,
                 BOOL bUseVideoMem /*= FALSE*/)
        : m_pDDSurface(NULL), m_pD3DTexture(NULL), m_bIsPrimary(FALSE),
          m_bIsTexture(FALSE), m_bHasPalette(FALSE), m_bInVideoMem(FALSE)
{
  m_Size.cx = m_Size.cy = 0;

  // We require the bitmap tag
  if (!pBitmap) return;

  // Get the full path of the bitmap
  char szBitmapName[MAX_PATH] = "";
  pBitmap->getFullName(szBitmapName);

  // Load the bitmap
  HBITMAP hBmp = LoadBMP(szBitmapName);
  ASSERT(NULL != hBmp); if (!hBmp) return;

  // Create the new surface
  VERIFY(Create(hBmp, bIsTexture, bUseVideoMem));

  // Clean up the bitmap
  ::DeleteObject(hBmp);
}


// ===========================================================================
// Function:    Constuctor
//
// Description: Constructs a new surface from the given bitmap file
//
// Params:      szBmpFile:    The name of the bitmap file
//              bIsTexture:   Are we creating a texture?
//              bUseVideoMem: Are we supposed to use video memory? (Ignored if
//                            creating a texture)
//
// Returns:     void
//
// Comments:    
// ===========================================================================
Surface::Surface(LPCSTR szBmpFile, BOOL bIsTexture /*= FALSE*/,
                 BOOL bUseVideoMem /*= FALSE*/)
        : m_pDDSurface(NULL), m_pD3DTexture(NULL), m_bIsPrimary(FALSE),
          m_bIsTexture(FALSE), m_bHasPalette(FALSE), m_bInVideoMem(FALSE)
{
  m_Size.cx = m_Size.cy = 0;

  // If no valid bitmap file name was provided, we can't very well create a
  //  surface from it now can we
  if (!szBmpFile) return;

  // Load the bitmap
  HBITMAP hBmp = LoadBMP(szBmpFile);
  ASSERT(NULL != hBmp); if (!hBmp) return;

  // Create the new surface
  VERIFY(Create(hBmp, bIsTexture, bUseVideoMem));

  // Clean up the bitmap
  ::DeleteObject(hBmp);
}


// ===========================================================================
// PRIVATE: Surface member functions =========================================
// ===========================================================================


// ===========================================================================
// PROTECTED: Surface member functions =======================================
// ===========================================================================


// ===========================================================================
// Function:    DetermineIfPrimary
//
// Description: Determins if this surface is the primary surface.
//
// Params:      void
//
// Returns:     void
//
// Comments:    Sets the internal m_bIsPrimary member variable.  See Surface.h
//              for an explanation of how the flags work.
// ===========================================================================
void Surface::DetermineIfPrimary()
{
  // Clear the flag
  m_bIsPrimary = FALSE;

  // Get the capabilities of the surface
  DDSURFCAPS surfCaps;
  VERIFY(GetCaps(surfCaps));

  // Determine if this is a primary surface
  if (DDSCAPS_PRIMARYSURFACE & surfCaps.dwCaps)
    m_bIsPrimary = TRUE;

  // Make sure our flag has its initialized bit set
  m_bIsPrimary |= 0x02;
}


// ===========================================================================
// Function:    DetermineIfTexture
//
// Description: Determins if this surface is a texture
//
// Params:      void
//
// Returns:     void
//
// Comments:    Sets the internal m_bIsTexture member variable.  See Surface.h
//              for an explanation of how the flags work.
// ===========================================================================
void Surface::DetermineIfTexture()
{
  // Clear the flag
  m_bIsTexture = FALSE;

  // Get the capabilities of the surface
  DDSURFCAPS surfCaps;
  VERIFY(GetCaps(surfCaps));

  // Determine if this is a texture
  if (DDSCAPS_TEXTURE & surfCaps.dwCaps)
    m_bIsTexture = TRUE;

  // Make sure our flag has its initialized bit set
  m_bIsTexture |= 0x02;
}


// ===========================================================================
// Function:    DetermineIfMipMapped
//
// Description: Determines if this surface is a mipmapped texture
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Surface::DetermineIfMipMapped()
{
  // Clear the flag
  m_bIsMipMap = FALSE;

  // If this surface isn't a texture, don't even bother doing the test.  It
  //  cannot possibly be a mipmapped texture
  if (IsTexture())
  {
    // Get the capabilities of the surface
    DDSURFCAPS surfCaps;
    VERIFY(GetCaps(surfCaps));

    // Determine if this is a mipmapped texture
    if (DDSCAPS_MIPMAP & surfCaps.dwCaps)
      m_bIsMipMap = TRUE;
  }

  // Make sure our flag has its initialized bit set
  m_bIsMipMap |= 0x02;
}


// ===========================================================================
// Function:    DetermineIfPalettized
//
// Description: Determins if this surface has(is suppposed to have) a palette
//
// Params:      void
//
// Returns:     void
//
// Comments:    Sets the internal m_bHasPalette member variable. See Surface.h
//              for an explanation of how the flags work.
// ===========================================================================
void Surface::DetermineIfPalettized()
{
  // Clear the flag
  m_bHasPalette = FALSE;

  // Get the pixel format of the surface
  DDPIXELFORMAT ddpf;
  VERIFY(GetPixelFormat(ddpf));

  // Determine if this surface is palettized
  if ((DDPF_PALETTEINDEXED1 | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED4 |
      DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8) & ddpf.dwFlags)
  {
    m_bHasPalette = TRUE;
  }

  // Make sure our flag has its initialized bit set
  m_bHasPalette |= 0x02;
}


// ===========================================================================
// Function:    DetermineIfInVideoMem
//
// Description: Determins if this surface is located in video memory.  If the
//              surface is a texture, this is not a valid question because
//              the texture manager will determine where the texture resides.
//
// Params:      void
//
// Returns:     void
//
// Comments:    Sets the internal m_bInVideoMem member variable. See Surface.h
//              for an explanation of how the flags work.
// ===========================================================================
void Surface::DetermineIfInVideoMem()
{
  // Clear the flag
  m_bInVideoMem = FALSE;

  // We can't ask a texture if it resides in video memory
  if (IsTexture()) return;

  // Get the capabilities of the surface
  DDSURFCAPS surfCaps;
  VERIFY(GetCaps(surfCaps));

  // Determine if this is a texture
  if (DDSCAPS_VIDEOMEMORY & surfCaps.dwCaps)
    m_bInVideoMem = TRUE;

  // Make sure our flag has its initialized bit set
  m_bInVideoMem |= 0x02;
}


// ===========================================================================
// Function:    DetermineSize
//
// Description: Determine's the size (width and height) of the given surface
//
// Params:      pSurface: The surface we want the size of
//              size:     Variable where the size will be stored
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Surface::DetermineSize(PDDSURFACE pSurface, SIZE& size)
{
  ASSERT(NULL != pSurface);
  DDSURFDESC ddsd;
  ZeroMemory(&ddsd, sizeof(DDSURFDESC));
  ddsd.dwSize = sizeof(DDSURFDESC);
  HRESULT hResult = pSurface->GetSurfaceDesc(&ddsd);
  QUIT_ON_ERROR(hResult);
  size.cx = long(ddsd.dwWidth);
  size.cy = long(ddsd.dwHeight);
}


// ===========================================================================
// Function:    ObtainD3DTexture
//
// Description: Querys our surface for it's texture interface.  Only works if
//              the surface is actually a texture.
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Surface::ObtainD3DTexture()
{
  // If this surface isn't a texture, we won't be able to get the interafce
  if (!IsTexture()) return;

  // We better have not already obtained the texture
  ASSERT(NULL == m_pD3DTexture);
  ASSERT(NULL != m_pDDSurface);

  // Query for the texture interface
  HRESULT hResult = m_pDDSurface->QueryInterface(CURRENT_D3DTEXTURE_GUID, 
                                                 (LPVOID*)&m_pD3DTexture);
  REPORT_ON_ERROR(hResult);
}


// ===========================================================================
// Function:    IsSurfaceCompatible
//
// Description: Determines if the given surface is compatible with this
//              surface.  Basically it compares the pixel formats to see if
//              data can be copied from one to the other without a great deal
//              of grief.
//
// Params:      surface: The surface to test for compatibility
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the surfaces are compatible and FALSE if they
//              are not.
// ===========================================================================
BOOL Surface::IsSurfaceCompatible(Surface& surface)
{
  DDPIXELFORMAT ddpf1;
  DDPIXELFORMAT ddpf2;

  // Get the pixel format of our surface
  if (!GetPixelFormat(ddpf1)) return(FALSE);

  // Get the pixel format of the test surface
  if (!surface.GetPixelFormat(ddpf2)) return(FALSE);

  // Do the comparison and return the result
  return(0 == memcmp(&ddpf1, &ddpf2, sizeof(DDPIXELFORMAT)));
}


// ===========================================================================
// Function:    PaletteFromBitmap
//
// Description: Creates a DirectDraw palette object from a bitmap
//
// Params:      hBitmap:  Handle to the bitmap containing the palette we want
//                        to use to create our DirectDraw palette
//
// Returns:     PDDPALETTE
//
// Comments:    Returns the DirectDraw palette if successful, or NULL if
//              something went wrong.
// ===========================================================================
PDDPALETTE Surface::PaletteFromBitmap(HBITMAP hBitmap)
{
  PALETTEENTRY    aEntries[256];
  PDDPALETTE      pPalette  = NULL;

  // Create ourselves a memory DC
  HDC hBitmapDC   = ::CreateCompatibleDC(NULL);
  ASSERT(NULL != hBitmapDC);

  // Select our bitmap into the memory DC
  HBITMAP hOldBmp = (HBITMAP)::SelectObject(hBitmapDC, hBitmap);

  // Get the colour table from the bitmap
  int nNumColours = ::GetDIBColorTable(hBitmapDC, 0, 256, (RGBQUAD*)aEntries);

  if (0 != nNumColours)
  {
    BYTE byTmp = 0;

    // Convert BGR to RGB (stupid IBM!)
    for (int i = 0; i < nNumColours; i++)
    {
      byTmp               = aEntries[i].peRed;
      aEntries[i].peRed   = aEntries[i].peBlue;
      aEntries[i].peBlue  = byTmp;
    }

    // Make sure our driver has been set
    ASSERT(NULL != Surface::m_pDDDriver);

    // create a DirectDraw palette for the texture.
    pPalette = Surface::m_pDDDriver->CreatePalette(DDPCAPS_8BIT | 
                                                   DDPCAPS_ALLOW256,
                                                   (PALETTEENTRY*)aEntries);
  }

  // Restore the old bitmap selection
  ::SelectObject(hBitmapDC, hOldBmp);

  // Destroy our memory DC
  VERIFY(::DeleteDC(hBitmapDC));

  return(pPalette);
}


// ===========================================================================
// Function:    Save8BitBmpFile
//
// Description: Saves this surface to the given file handle as an 8 bit
//              palettized .bmp file
//
// Params:      hFile:  The file to write the data to
//              rctBitmap:  Rectangle describing the arean we want to save
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the contents of the surface were successfully
//              written to the file, or FALSE if something went wrong.
//              NOTE: It is assumed that the BITMAPFILEHEADER has already been
//                    written to the file before calling this function.
// ===========================================================================
BOOL Surface::Save8BitBmpFile(HANDLE hFile, const RECT& rctBitmap)
{
  DWORD               dwWritten = 0;
  DWORD               dwPixel   = 0;
  DDSURFDESC          ddsd;
  BITMAPINFO          bmpInfo;
  RGBQUAD             palEntries[256];

  // Get the surface description
  if (!GetSurfaceDesc(ddsd)) return(FALSE);

  // Save the info we care about
  DWORD dwWidth     = rctBitmap.right - rctBitmap.left;
  DWORD dwHeight    = rctBitmap.bottom - rctBitmap.top;
  long  lPitch      = ddsd.lPitch;

  // Make sure this is an 8 bit format
  ASSERT(8 == ddsd.ddpfPixelFormat.dwRGBBitCount);

  // Get the palette info
  PDDPALETTE pPalette = GetPalette();

  if (!pPalette)
  {
    // We weren't able to get the palette from the surface, try getting it
    //  from the primary surface
    Surface* pPrimary = pc3D::GetPrimarySurface();
    ASSERT(NULL != pPrimary);

    // If the primary surface isn't palettized, we won't be able to write 
    //  the bitmap
    if (8 != pPrimary->GetBitDepth()) return(FALSE);

    // Get the palette of the primary surface
    pPalette = pPrimary->GetPalette();

    // If we weren't able to get the palette, we won't be able to write 
    //  the bitmap
    if (!pPalette) return(FALSE);
  }

  // Get the palette entries
  HRESULT hResult = pPalette->GetEntries(0, 0, 256, (PALETTEENTRY*)palEntries);
  RETURN_ON_ERROR(hResult, FALSE);

  // Prepare the bitmap info header
  bmpInfo.bmiHeader.biSize           = sizeof(BITMAPINFOHEADER);
  bmpInfo.bmiHeader.biWidth          = dwWidth;
  bmpInfo.bmiHeader.biHeight         = dwHeight;
  bmpInfo.bmiHeader.biPlanes         = 1;
  bmpInfo.bmiHeader.biBitCount       = 8;
  bmpInfo.bmiHeader.biCompression    = BI_RGB;
  bmpInfo.bmiHeader.biSizeImage      = dwWidth * dwHeight;
  bmpInfo.bmiHeader.biXPelsPerMeter  = 10000;
  bmpInfo.bmiHeader.biYPelsPerMeter  = 10000;
  bmpInfo.bmiHeader.biClrUsed        = 0;
  bmpInfo.bmiHeader.biClrImportant   = 0;

  // Set the bitmap info colour table

  // The palette data is kinda retarded.  It's stored in BGR format so we need
  //  to reverse the order of our palette entries
  for (int i = 0; i < 256; i++)
  {
    // The rgbReserved item of the RGBQUAD structure is supposed to be zero
    //  so we are going to use it as temporary storage and then set it back
    //  to zero when we're done
    palEntries[i].rgbReserved = palEntries[i].rgbBlue;
    palEntries[i].rgbBlue     = palEntries[i].rgbRed;
    palEntries[i].rgbRed      = palEntries[i].rgbReserved;
    palEntries[i].rgbReserved = 0;
  }

  // Write the bmp info header to the file
  if (!::WriteFile(hFile, &bmpInfo, sizeof(BITMAPINFOHEADER), &dwWritten, NULL))
    return(FALSE);

  // Write the palette info to the file
  if (!::WriteFile(hFile, &palEntries, sizeof(RGBQUAD) * 256, &dwWritten, NULL))
    return(FALSE);

  // Create ourselves a buffer to write the info to
  DWORD dwSize = dwWidth * dwHeight;

  // If we are saving a bitmap with an odd with, we need to add an extra
  //  pixel to every scan line.  Make sure the buffer is large enough.
  if (dwWidth % 2) 
    dwSize += dwHeight;

  BYTE* pbyBuffer = new BYTE[dwSize];

  // Lock our surface
  if (!Lock(ddsd, DDLOCK_WAIT | DDLOCK_NOSYSLOCK | DDLOCK_READONLY))
  {
    delete [] pbyBuffer;
    return(FALSE);
  }

  // Get the pointer to the surface data
  BYTE* pbyData = (BYTE*)ddsd.lpSurface;

  int nIdx = 0;

  // Write the info to the file, from bottom to the top
  for (int nRow = rctBitmap.bottom - 1; nRow >= rctBitmap.top; nRow--)
  {
    for (int nCol = rctBitmap.left; nCol < rctBitmap.right; nCol++)
    {
      // Get the next pixel from the surface and add it to the buffer
      pbyBuffer[nIdx++] = *(pbyData + nCol + (nRow * lPitch));
    }

    // If we have written an odd number of pixels to the scanline, we need to
    //  pad it with an extra pixel.
    if (dwWidth % 2)
      pbyBuffer[nIdx++] = 0;
  }

  // Now unlock the surface and we're done
  VERIFY(Unlock());

  // The buffer should be full, so just write it out
  if (!::WriteFile(hFile, pbyBuffer, nIdx, &dwWritten, NULL))
  {
    delete [] pbyBuffer;
    return(FALSE);
  }

  delete [] pbyBuffer;

  return(TRUE);
}


// ===========================================================================
// Function:    Save24BitBmpFile
//
// Description: Saves this surface to the given file handle as a 24 bit .bmp
//              file
//
// Params:      hFile:      The file to write the data to
//              rctBitmap:  Rectangle describing the arean we want to save
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the contents of the surface were successfully
//              written to the file, or FALSE if something went wrong.
//              NOTE: It is assumed that the BITMAPFILEHEADER has already been
//                    written to the file before calling this function.
// ===========================================================================
BOOL Surface::Save24BitBmpFile(HANDLE hFile, const RECT& rctBitmap)
{
  DWORD               dwWritten = 0;
  DWORD               dwPixel   = 0;
  DDSURFDESC          ddsd;
  BITMAPINFOHEADER    bmpInfoHdr;

  // Get the surface description
  if (!GetSurfaceDesc(ddsd)) return(FALSE);

  // Save the info we care about
  DWORD dwWidth     = rctBitmap.right - rctBitmap.left;
  DWORD dwHeight    = rctBitmap.bottom - rctBitmap.top;
  DWORD dwBPPMode   = ddsd.ddpfPixelFormat.dwRGBBitCount / 8;
  DWORD dwRedMask   = ddsd.ddpfPixelFormat.dwRBitMask;
  DWORD dwGreenMask = ddsd.ddpfPixelFormat.dwGBitMask;
  DWORD dwBlueMask  = ddsd.ddpfPixelFormat.dwBBitMask;
  long  lPitch      = ddsd.lPitch;

  // We must have at least 16 bits per pixel
  ASSERT(1 < dwBPPMode);

  // Prepare the bitmap info header
  bmpInfoHdr.biSize           = sizeof(BITMAPINFOHEADER);
  bmpInfoHdr.biWidth          = dwWidth;
  bmpInfoHdr.biHeight         = dwHeight;
  bmpInfoHdr.biPlanes         = 1;
  bmpInfoHdr.biBitCount       = 24;
  bmpInfoHdr.biCompression    = BI_RGB;
  bmpInfoHdr.biSizeImage      = dwWidth * dwHeight * 3;
  bmpInfoHdr.biXPelsPerMeter  = 10000;
  bmpInfoHdr.biYPelsPerMeter  = 10000;
  bmpInfoHdr.biClrUsed        = 0;
  bmpInfoHdr.biClrImportant   = 0;

  // Write the info header to the file
  if (!::WriteFile(hFile, &bmpInfoHdr, sizeof(BITMAPINFOHEADER), &dwWritten, NULL))
    return(FALSE);

  // Create ourselves a buffer to write the info to
  //  width * height * 24-bits / pixel
  DWORD dwSize = dwWidth * dwHeight * 3;

  // If we are saving a bitmap with an odd with, we need to add an extra
  //  pixel to every scan line.  Make sure the buffer is large enough.
  if (dwWidth % 2) 
    dwSize += 3 * dwHeight;

  BYTE* pbyBuffer = new BYTE[dwSize];

  // Get the bit shift of each of the colour element's bit mask
  DWORD dwRBitShift = ::GetBitShift(dwRedMask);
  DWORD dwGBitShift = ::GetBitShift(dwGreenMask);
  DWORD dwBBitShift = ::GetBitShift(dwBlueMask);

  // Get the multiplication factors, these are important for 16 bit mode
  DWORD dwRFactor = (16 == ddsd.ddpfPixelFormat.dwRGBBitCount) ? 8 : 1;
  DWORD dwGFactor = (16 == ddsd.ddpfPixelFormat.dwRGBBitCount) ? 8 : 1;
  DWORD dwBFactor = (16 == ddsd.ddpfPixelFormat.dwRGBBitCount) ? 8 : 1;

  // Adjust the factors for funky 16 bit modes (like 565 or 655)
  if (6 == ::GetBitDepth(dwRedMask))    dwRFactor = 4;
  if (6 == ::GetBitDepth(dwGreenMask))  dwGFactor = 4;
  if (6 == ::GetBitDepth(dwBlueMask))   dwBFactor = 4;

  // Lock our surface
  if (!Lock(ddsd, DDLOCK_WAIT | DDLOCK_NOSYSLOCK | DDLOCK_READONLY))
  {
    delete [] pbyBuffer;
    return(FALSE);
  }

  // Get the pointer to the surface data
  BYTE* pbyData = (BYTE*)ddsd.lpSurface;

  int nIdx = 0;

  // Write the info to the file, from bottom to the top
  for (int nRow = rctBitmap.bottom - 1; nRow >= rctBitmap.top; nRow--)
  {
    for (int nCol = rctBitmap.left; nCol < rctBitmap.right; nCol++)
    {
      // Get the next pixel based on our bits/pixel mode
      switch (dwBPPMode)
      {
        case 4:   // 32 bit mode
          dwPixel = *((DWORD*)(pbyData + (nCol * dwBPPMode) + (nRow * lPitch)));
          break;
        case 3:   // 24 bit mode
          CopyMemory(((BYTE*)(&dwPixel)) + 1, 
                     pbyData + (nCol * dwBPPMode) + (nRow * lPitch), 3);
          break;
        case 2:   // 16 bit mode
          dwPixel = *((WORD*)(pbyData + (nCol * dwBPPMode) + (nRow * lPitch)));
          break;
        default:
          ASSERT(FALSE);  // We should never get here
          break;
      }

      // Write the blue, green and red components to the buffer
      pbyBuffer[nIdx++] = (BYTE)(dwBFactor * ((dwPixel & dwBlueMask) >> dwBBitShift));
      pbyBuffer[nIdx++] = (BYTE)(dwGFactor * ((dwPixel & dwGreenMask) >> dwGBitShift));
      pbyBuffer[nIdx++] = (BYTE)(dwRFactor * ((dwPixel & dwRedMask) >> dwRBitShift));
    }

    // If we have written an odd number of pixels to the scanline, we need to
    //  pad it with an extra pixel.
    if (dwWidth % 2)
    {
      pbyBuffer[nIdx++] = 0;
      pbyBuffer[nIdx++] = 0;
      pbyBuffer[nIdx++] = 0;
    }
  }

  // Now unlock the surface and we're done
  VERIFY(Unlock());

  // The buffer should be full, so just write it out
  if (!::WriteFile(hFile, pbyBuffer, nIdx, &dwWritten, NULL))
  {
    delete [] pbyBuffer;
    return(FALSE);
  }

  delete [] pbyBuffer;

  return(TRUE);
}


// ===========================================================================
// PUBLIC: Surface member functions ==========================================
// ===========================================================================


// ===========================================================================
// Function:    Create
//
// Description: Initializes the m_pDDSurface member function by creating a new
//              DirectDraw surface from the given surface description.
//
// Params:      surfDesc: Surface description structure
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the surface was created successfully or FALSE
//              if it was not.
// ===========================================================================
BOOL Surface::Create(DDSURFDESC& surfDesc)
{
  // Make sure we don't have resources hanging around
  ReleaseSurface("Create(DDSURFDESC& surfDesc): ", __FILE__, __LINE__);

  // Make sure our driver has been set
  ASSERT(NULL != Surface::m_pDDDriver);

  // Create the surface
  m_pDDSurface = Surface::m_pDDDriver->CreateSurface(surfDesc);
  ASSERT(NULL != m_pDDSurface); if (!m_pDDSurface) return(FALSE);

  // Get the size of the surface
  DetermineSize(m_pDDSurface, m_Size);

  return(TRUE);
}


// ===========================================================================
// Function:    Create
//
// Description: Initializes this surface using the passed in DirectDraw
//              surface
//
// Params:      pDDSurface: Pointer to the DirectDraw surface object we are
//                          using to initialize this surface
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if successful, or FALSE if not.
// ===========================================================================
BOOL Surface::Create(PDDSURFACE pDDSurface)
{
  // Make sure we don't have resources hanging around
  ReleaseSurface("Create(PDDSURFACE pDDSurface): ", __FILE__, __LINE__);

  // We can't wrap NULL surfaces
  ASSERT(NULL != pDDSurface); if (!pDDSurface) return(FALSE);

  // Set our surface pointer
  m_pDDSurface = pDDSurface;

  // Get the surface size
  DetermineSize(m_pDDSurface, m_Size);

  return(TRUE);
}


// ===========================================================================
// Function:    Create
//
// Description: Creates a new surface which is an exact copy of the one passed
//              in.
//
// Params:      surface:      The surface we want to copy
//              bIsTexture:   Indicates we are creating a texture
//              bUseVidMem:   Indicates if the new surface is to be created in
//                            video memory
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the surface was created successfully, or FALSE
//              if it was not.
// ===========================================================================
BOOL Surface::Create(Surface& surface, BOOL bIsTexture /*= FALSE*/,
                     BOOL bUseVideoMem /*= FALSE*/)
{
  DDSURFDESC ddsdSource;
  DDSURFDESC ddsd;

  // Get the surface description of the source surface
  if (!surface.GetSurfaceDesc(ddsdSource)) return(FALSE);

  // Clear out surface description
  ZeroMemory(&ddsd, sizeof(DDSURFDESC));
  ddsd.dwSize = sizeof(DDSURFDESC);

  // Set up the surface description for our new surface
  ddsd.dwWidth          = ddsdSource.dwWidth;
  ddsd.dwHeight         = ddsdSource.dwHeight;
  ddsd.ddpfPixelFormat  = ddsdSource.ddpfPixelFormat;
  ddsd.dwFlags          = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | 
                          DDSD_PIXELFORMAT;

  // If the surface we are copying is a texture, turn on the texture manager
  if (bIsTexture)
  {
    ddsd.ddsCaps.dwCaps   = DDSCAPS_TEXTURE;
    ddsd.ddsCaps.dwCaps2  = DDSCAPS2_TEXTUREMANAGE;
  }
  else
  {
    // Otherwise use the video memory setting
    if (bUseVideoMem)
    {
      ddsd.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;
      // We need to use the same pixel format as the main rendering surface
      Surface* pRender = pc3D::GetRenderSurface();
      ASSERT(NULL != pRender);
      VERIFY(pRender->GetPixelFormat(ddsd.ddpfPixelFormat));
    }
    else
      ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;
  }

  // Create the new surface
  if (!Create(ddsd)) return(FALSE);

  // If the original surface had a palette we need to copy it to our new
  //  surface
  if (surface.IsPalettized())
  {
    // Get the palette and its entries
    PALETTEENTRY aPalEntries[256];
    PDDPALETTE pDDPalette = surface.GetPalette();
    ASSERT(NULL != pDDPalette);
    pDDPalette->GetEntries(0, 0, 256, aPalEntries);

    // Create a new palette from the one we obtained
    pDDPalette = Surface::m_pDDDriver->CreatePalette(DDPCAPS_8BIT |
                                                     DDPCAPS_ALLOW256,
                                                     aPalEntries);
    // Set the palette of our new surface
    VERIFY(SetPalette(pDDPalette));

    // Do a release on our palette so it will automatically get released
    //  when the surface is released.
    RELEASEPOINTER(pDDPalette);
  }

  // Now copy the contents of the source
  return(CopySurface(surface));
}


// ===========================================================================
// Function:    Create
//
// Description: Creates a new surface of the given dimensions using the
//              pixel format of the primary surface.
//
// Params:      nWidth:       The width of the new surface
//              nHeight:      The height of the new surface
//              bUseVideoMem: Are we supposed to use video memory for this
//                            surface?
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the surface is created successfully and FALSE
//              if it is not.
// ===========================================================================
BOOL Surface::Create(UINT nWidth, UINT nHeight, BOOL bUseVideoMem /*= FALSE*/)
{
  DDSURFDESC ddsd;
  ZeroMemory(&ddsd, sizeof(DDSURFDESC));
  ddsd.dwSize = sizeof(DDSURFDESC);

  // Set the flags and capabilities
  ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
  ddsd.ddsCaps.dwCaps = bUseVideoMem ? DDSCAPS_VIDEOMEMORY :
                                       DDSCAPS_SYSTEMMEMORY;
  ddsd.dwWidth        = nWidth;
  ddsd.dwHeight       = nHeight;

  // Get the primary surface's pixel format
  Surface* pPrimary = pc3D::GetPrimarySurface();
  ASSERT(NULL != pPrimary);
  VERIFY(pPrimary->GetPixelFormat(ddsd.ddpfPixelFormat));

  // Create the surface
  return(Create(ddsd));
}


// ===========================================================================
// Function:    Create
//
// Description: Creates an application defined surface of the given dimensions
//              and using the given depth and 8 bit palette.  Used by external
//              texture suppliers.
//
// Params:      nWidth:       The width of the new surface
//              nHeight:      The height of the new surface
//              nClrDepth:    The desired bit depth of the new surface
//              nAlphaBits:   The desired number of bits in the alpha channel
//              pPalEntries:  Palette entries for 8 bit surfaces
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the surface is created successfully and FALSE
//              if it is not.
// ===========================================================================
BOOL Surface::Create(UINT nWidth, UINT nHeight, BYTE nClrDepth,
                     UINT nAlphabits /*= 0*/,
                     PALETTEENTRY* pPalEntries /*= NULL*/)
{
  DDSURFDESC ddsd;
  ZeroMemory(&ddsd, sizeof(ddsd));
  ddsd.dwSize = sizeof(ddsd);

  // Although we aren't explicitly creating a texture here, we want to create
  //  a surface that will be compatible with the 3D rendering device
  if (!FindTextureFormat(nClrDepth, nAlphabits, ddsd.ddpfPixelFormat))
    return(FALSE);

  // Set our flags for surface creation
  ddsd.dwFlags  = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
  ddsd.dwWidth  = nWidth;
  ddsd.dwHeight = nHeight;
  ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;
  
  // Create the surface
  if (!Create(ddsd))
    return(FALSE);

  // Does this surface need a palette, and was one given to us?
  if (pPalEntries && IsPalettized())
  {
    // Make sure our driver has been set
    ASSERT(NULL != Surface::m_pDDDriver);

    // Create the palette
    PDDPALETTE pPalette = Surface::m_pDDDriver->CreatePalette(DDPCAPS_8BIT |
                                                              DDPCAPS_ALLOW256,
                                                              pPalEntries);
    ASSERT(NULL != pPalette);

    // Set the palette of our surface
    VERIFY(SetPalette(pPalette));

    // Okay we don't need to keep this reference around.  The DirectDraw
    //  surface will do that for us.
    RELEASEPOINTER(pPalette);
  }

  return(TRUE);
}


// ===========================================================================
// Function:    Create
//
// Description: Initializes this surface using a bitmap
//
// Params:      hBitmap:      Handle to the bitmap to use to create the surface
//              bIsTexture:   Are we creating a texture?
//              bUseVideoMem: Are we supposed to use video memory? (Ignored if
//                            we are creating a texture)
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the surface is created successfully and FALSE
//              if it is not.
// ===========================================================================
BOOL Surface::Create(HBITMAP hBitmap, BOOL bIsTexture /*= FALSE*/,
                     BOOL bUseVideoMem /*= FALSE*/)
{
  // Get the size of the bitmap
  BITMAP bmp;
  ::GetObject(hBitmap, sizeof(BITMAP), &bmp);

  // Are we creating a texture?
  if (bIsTexture)
  {
    DDPIXELFORMAT ddpf;

    // We need to find an appropriate pixel format for the texure
    if (!FindTextureFormat(BYTE(bmp.bmBitsPixel), 0, ddpf))
      return(FALSE);

    // Create the texture
    if (!CreateTexture(bmp.bmWidth, bmp.bmHeight, ddpf))
      return(FALSE);
  }
  else
  {
    DDSURFDESC ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    // Although we aren't explicitly creating a texture here, we want to create
    //  a surface that will be compatible with the 3D rendering device
    if (!FindTextureFormat(BYTE(bmp.bmBitsPixel), 0, ddsd.ddpfPixelFormat))
      return(FALSE);

    // Set our flags for surface creation
    ddsd.dwFlags  = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.dwWidth  = bmp.bmWidth;
    ddsd.dwHeight = bmp.bmHeight;

    if (bUseVideoMem)
      ddsd.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;
    else
      ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;

    // We're not creating a texture so just create a regular surface
    if (!Create(ddsd))
      return(FALSE);
  }

  // Sanity check
  ASSERT(NULL != m_pDDSurface);

  // Do we need to create a palette for our surface?
  if (IsPalettized())
  {
    // Get the palette from the bitmap
    PDDPALETTE pPalette = PaletteFromBitmap(hBitmap);
    ASSERT(NULL != pPalette);

    // Set the surface's palette
    VERIFY(SetPalette(pPalette));

    // Okay we don't need to keep this reference around.  The DirectDraw
    //  surface will do that for us.
    RELEASEPOINTER(pPalette);
  }

  // Finally copy the bitmap to the surface
  return(CopyBitmap(hBitmap));
}


// ===========================================================================
// Function:    CreateTexture
//
// Description: Creates a new texture surface
//
// Params:      nWidth:         The width of the texture
//              nHeight:        The height of the texture
//              ddPixelFormat:  The pixel format of the new texture
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the texture was created successfully, or FALSE
//              if it was not.
// ===========================================================================
BOOL Surface::CreateTexture(UINT nWidth, UINT nHeight, 
                            const DDPIXELFORMAT& ddPixelFormat)
{
  // Make sure everything is clean
  ReleaseSurface("CreateTexture(): ", __FILE__, __LINE__);

  DDSURFDESC ddsd;
  ZeroMemory(&ddsd, sizeof(DDSURFDESC));
  ddsd.dwSize = sizeof(DDSURFDESC);

  // Set the flags and capabilites
  ddsd.dwFlags          = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | 
                          DDSD_PIXELFORMAT;
  ddsd.ddsCaps.dwCaps   = DDSCAPS_TEXTURE;
  ddsd.ddsCaps.dwCaps2  = DDSCAPS2_TEXTUREMANAGE;
  ddsd.dwWidth          = nWidth;
  ddsd.dwHeight         = nHeight;

  // Copy the pixel format
  CopyMemory(&ddsd.ddpfPixelFormat, &ddPixelFormat, sizeof(DDPIXELFORMAT));

  // Create the texure
  return(Create(ddsd));
}


// ===========================================================================
// Function:    CreateMipMap
//
// Description: Creates a mip mapped texture
//
// Params:      nMaxWidth:      Maximum width of the mip map
//              nMaxHeight:     Maximum height of the mip map
//              byLevels:       Number of mip map levels to create
//              ddPixelFormat:  Pixel format of the texture's surface
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the mipmap is successfully created, or FALSE
//              if it is not.
// ===========================================================================
BOOL Surface::CreateMipMap(UINT nMaxWidth, UINT nMaxHeight, BYTE byLevels,
                           const DDPIXELFORMAT& ddPixelFormat)
{
  DDSURFDESC ddsd;
  ZeroMemory(&ddsd, sizeof(DDSURFDESC));

  // Setup our surface description
  ddsd.dwSize           = sizeof(DDSURFDESC); 
  ddsd.dwFlags          = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | 
                          DDSD_MIPMAPCOUNT | DDSD_PIXELFORMAT;
  ddsd.ddsCaps.dwCaps   = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
  ddsd.ddsCaps.dwCaps2  = DDSCAPS2_TEXTUREMANAGE;
  ddsd.dwMipMapCount    = (DWORD)byLevels;
  ddsd.dwWidth          = (DWORD)nMaxWidth;
  ddsd.dwHeight         = (DWORD)nMaxHeight;

  // Copy the pixel format
  CopyMemory(&ddsd.ddpfPixelFormat, &ddPixelFormat, sizeof(DDPIXELFORMAT));

  return(Create(ddsd));
}


// ===========================================================================
// Function:    LoadMipMaps
//
// Description: Initializes all of the surfaces in a mip map texture using the
//              passed in bitmaps
//
// Params:      ahBitmaps:    An array of bitmap handles to load onto the mip
//                            map surfaces
//              nNumBitmaps:  The number of bitmaps in the array
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the mip maps were successfully loaded, or
//              FALSE if they were not
// ===========================================================================
BOOL Surface::LoadMipMaps(HBITMAP* ahBitmaps, int nNumBitmaps)
{
  DDSURFDESC ddsd;
  DDSURFCAPS surfCaps;

  // Get the surface description of the this surface
  if (!GetSurfaceDesc(ddsd)) return(FALSE);

  // Make sure this surface is a mipmap texture before we start loading
  //  bitmaps onto it
  if (!(ddsd.ddsCaps.dwCaps & DDSCAPS_MIPMAP)) return(FALSE);

  // We need a valid mipmap count to continue
  if (!(ddsd.dwFlags & DDSD_MIPMAPCOUNT)) return(FALSE);

  // If we have more bitmaps than mipmap levels, we will only use the number
  //  bitmaps that we can handle
  int nLevels = min(nNumBitmaps, (int)ddsd.dwMipMapCount);

  // We need to add a reference to the DirectDraw surface before we wrap it
  //  in another Surface class
  m_pDDSurface->AddRef();

  Surface surfLevel(m_pDDSurface);
  ZeroMemory(&surfCaps, sizeof(DDSURFCAPS));
  surfCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

  // Load the bitmaps
  for (int i = 0; i < nLevels; i++)
  {
    // Process this level
    if (!surfLevel.CopyBitmap(ahBitmaps[i])) break;

    // Get the next level
    if (!surfLevel.GetAttachedSurface(surfCaps, surfLevel)) break;
  }

  // If we haven't already released the last level, do so now
  if (surfLevel.IsInitialized())
    ReleaseSurface("Surface::LoadMipMaps(): ", __FILE__, __LINE__);

  // If we weren't able to load all of the bitmaps, something went wrong
  if (i != nLevels - 1) return(FALSE);

  return(TRUE);
}


// ===========================================================================
// Function:    LoadMipMaps
//
// Description: Initializes all of the surfaces in a mip map texture using the
//              passed in bitmap.  The passed in bitmap is assumed to be a
//              composite comprised of all of the levels of the mip map.
//
// Params:      hCompositeBmp:  A composite bitmap containing all of the
//                              levels of the mip map
//              nNumLevels:     The number of levels of mip map contained in
//                              the bitmap
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the mip maps were successfully loaded, or
//              FALSE if they were not
// ===========================================================================
BOOL Surface::LoadMipMaps(HBITMAP hCompositeBmp, int nNumLevels)
{
  DDSURFDESC ddsd;
  DDSURFCAPS surfCaps;

  // Get the surface description of the this surface
  if (!GetSurfaceDesc(ddsd)) return(FALSE);

  // Make sure this surface is a mipmap texture before we start loading
  //  bitmaps onto it
  if (!(ddsd.ddsCaps.dwCaps & DDSCAPS_MIPMAP)) return(FALSE);

  // We need a valid mipmap count to continue
  if (!(ddsd.dwFlags & DDSD_MIPMAPCOUNT)) return(FALSE);

  // If we have more bitmaps than mipmap levels, we will only use the number
  //  bitmaps that we can handle
  int nLevels = min(nNumLevels, (int)ddsd.dwMipMapCount);

  // We need to add a reference to the DirectDraw surface before we wrap it
  //  in another Surface class
  m_pDDSurface->AddRef();

  Surface surfLevel(m_pDDSurface);
  ZeroMemory(&surfCaps, sizeof(DDSURFCAPS));
  surfCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

  // Create a memory DC for the bitmap
  HDC hBitmapDC = ::CreateCompatibleDC(NULL);
  ASSERT(NULL != hBitmapDC);

  // Select the bitmap into the memory DC
  HBITMAP hOldBmp = (HBITMAP)::SelectObject(hBitmapDC, hCompositeBmp);

  int   nXPos       = 0;
  int   nYPos       = 0;
  BOOL  bSuccess    = FALSE;
  HDC   hSurfaceDC  = NULL;

  // Load the bitmaps
  for (int i = 0; i < nLevels; i++)
  {
    // Get the DC for the surface
    hSurfaceDC = surfLevel.GetDC();
    ASSERT(NULL != hSurfaceDC);

    // Compute the x, y position where we will be copying from
    nXPos = 0 == i ? 0 : m_Size.cx;
    nYPos = 0 == i ? 0 : surfLevel.m_Size.cy;

    // Blit the next portion to this level
    bSuccess = ::StretchBlt(hSurfaceDC, 0, 0, surfLevel.m_Size.cx,
                            surfLevel.m_Size.cy, hBitmapDC, nXPos, nYPos,
                            surfLevel.m_Size.cx, surfLevel.m_Size.cy, SRCCOPY);

    // Release the surface DC
    VERIFY(surfLevel.ReleaseDC(hSurfaceDC));
  
    if (!bSuccess) break;

    // Get the next level
    if (!surfLevel.GetAttachedSurface(surfCaps, surfLevel)) break;
  }

  // Select the old bitmap
  ::SelectObject(hBitmapDC, hOldBmp);

  // Release the memory DC
  ::DeleteDC(hBitmapDC);

  // If we haven't already released the last level, do so now
  if (surfLevel.IsInitialized())
  {
    RELEASESURFACE(surfLevel);
  }

  // If we weren't able to load all of the bitmaps, something went wrong
  if (i != nLevels - 1) return(FALSE);

  return(TRUE);
}


// ===========================================================================
// Function:    CopySurface
//
// Description: Copies the contents of a surface to this one
//
// Params:      ddsSource: The surface we want to copy from
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the copy was successful or FALSE if it was not
// ===========================================================================
BOOL Surface::CopySurface(Surface& surface)
{
  // Make sure the surfaces have compatible pixel formats
  if (!IsSurfaceCompatible(surface))
  {
    TRACE("Surface::CopySurface() : surface pixel formats are not compatible!\r\n");
    return(FALSE);
  }

  // Get the rectangle of the source surface
  RECT rctSrc;
  surface.GetRect(rctSrc);

  // If the surface we are copying from is bigger than our surface, we need to
  //  adjust the size of the source rectangle
  if (surface.m_Size.cx > m_Size.cx || surface.m_Size.cy > m_Size.cy)
  {
    rctSrc.right = m_Size.cx;
    rctSrc.bottom = m_Size.cy;
  }

  // Copy the surfaces
  return(BltFast(0, 0, surface, rctSrc, DDBLTFAST_WAIT));
}


// ===========================================================================
// Function:    CopyBitmap
//
// Description: Copies a bitmap to this surface
//
// Params:      hBitmap:  Handle to the bitmap to copy to this surface
//
// Returns:     BOOL
//
// Comments:    Return TRUE if the bitmap was successfully copied or FALSE
//              if it was not copied.
// ===========================================================================
BOOL Surface::CopyBitmap(HBITMAP hBitmap)
{
  BITMAP  bmp;
  BOOL    bSuccess = FALSE;

  // Make sure we have a valid bitmap
  ASSERT(NULL != hBitmap); if (!hBitmap) return(FALSE);

  // Get the size of the bitmap
  ::GetObject(hBitmap, sizeof(BITMAP), &bmp);

  // Adjust our blitting size if our bitmap is too big
  if (bmp.bmWidth > m_Size.cx)  bmp.bmWidth = m_Size.cx;
  if (bmp.bmHeight > m_Size.cy) bmp.bmHeight = m_Size.cy;

  // Create a memory DC for the bitmap
  HDC hBitmapDC = ::CreateCompatibleDC(NULL);
  ASSERT(NULL != hBitmapDC);

  // Select the bitmap into the memory DC
  HBITMAP hOldBmp = (HBITMAP)::SelectObject(hBitmapDC, hBitmap);

  // Get the DC for the suface
  HDC hSurfaceDC = GetDC();

  if (hSurfaceDC)
  {
    // Blit the bitmap to the surface
    bSuccess = ::BitBlt(hSurfaceDC, 0, 0, bmp.bmWidth, bmp.bmHeight,
                        hBitmapDC, 0, 0, SRCCOPY);
    VERIFY(ReleaseDC(hSurfaceDC));
  }

  // Select the old bitmap
  ::SelectObject(hBitmapDC, hOldBmp);

  // Release the memory DC
  ::DeleteDC(hBitmapDC);

  return(bSuccess);
}


// ===========================================================================
// Function:    FindTextureFormat
//
// Description: Finds a texture format compatible with the given bit depth and
//              alpha information
//
// Params:      byBPP:      The colour depth of the format we are looking for
//              nAlphaBits: The number of Alpha Bits we are looking for
//              ddpf:       A DDPIXELFORMAT structure that will be filled with
//                          the pixel format for the texture
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if a texture format is found having the required
//              components, or FALSE if a format cannot be found.  This is a
//              static funtion.
// ===========================================================================
BOOL Surface::FindTextureFormat(BYTE byBPP, UINT nAlphaBits, 
                                DDPIXELFORMAT& ddpf)
{
  DDPIXELFORMAT*  pPixelFormat            = NULL;
  BOOL            bMeetsAlphaRequirements = FALSE;

  // Clear the pixel format
  ZeroMemory(&ddpf, sizeof(DDPIXELFORMAT));

  // Get the D3DDevice from PC3D
  D3DDevice* pDevice = pc3D::Get3DDevice();
  ASSERT(NULL != pDevice);

  // Iterate through the D3DDevice's texture formats, to find a suitable one
  for (int i = 0; i < pDevice->GetNumTextureFormats(); i++)
  {
    // Get the next texture format
    pPixelFormat            = pDevice->GetTextureFormat(i);
    bMeetsAlphaRequirements = FALSE;

    // We don't support 1,4 or 32 bit textures
    if (8 > pPixelFormat->dwRGBBitCount || 16 < pPixelFormat->dwRGBBitCount)
      continue;

    // Make sure we are using an RGB format
    if (!(pPixelFormat->dwFlags & DDPF_RGB))
      continue;

    // Make sure that, if we are choosing an 8 bit format, that it is palettized
    if (8 >= pPixelFormat->dwRGBBitCount && 
        !(pPixelFormat->dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4)))
    {
      continue;
    }

    // Do we have alpha in the format, and do we wan't it ?
    if (pPixelFormat->dwFlags & DDPF_ALPHAPIXELS)
    {
      // If there's alpha, there must be an alpha mask
      ASSERT(pPixelFormat->dwRGBAlphaBitMask);

      // If the alpha depth we are looking for is the same as the format
      //  we are currently looking at, we'll take it
      if (nAlphaBits == ::GetBitDepth(pPixelFormat->dwRGBAlphaBitMask))
        bMeetsAlphaRequirements = TRUE;
    }
    else
    {
      // If there is no alpha channel, it is safe to choose this format
      if (0 == nAlphaBits)
        bMeetsAlphaRequirements = TRUE;
    }

    // If it doesn't meet the alpha requirements, we don't want it
    if (!bMeetsAlphaRequirements)
      continue;

    // Choose the format that is closest to the bit depth we are looking for
    if (byBPP == pPixelFormat->dwRGBBitCount)
      CopyMemory(&ddpf, pPixelFormat, sizeof(DDPIXELFORMAT));
    else if (byBPP > pPixelFormat->dwRGBBitCount)
    {
      if (byBPP - pPixelFormat->dwRGBBitCount < byBPP - ddpf.dwRGBBitCount)
        CopyMemory(&ddpf, pPixelFormat, sizeof(DDPIXELFORMAT));
    }
    else
    {
      if (UINT(pPixelFormat->dwRGBBitCount - byBPP) <
          UINT(ddpf.dwRGBBitCount - byBPP))
      {
        CopyMemory(&ddpf, pPixelFormat, sizeof(DDPIXELFORMAT));
      }
    }
  }

  // Check to make sure we actually selected a texture format
  if (ddpf.dwRGBBitCount == 0)
  {
    TRACE("\r\n\tFailed to find suitable texture format for %d bits per ",
          "pixel and %d bits per alpha channel.\r\n", byBPP, nAlphaBits);
    return(FALSE);
  }

  TRACE("FindTextureFormat: %2d %5s %3s %3s A:0x%08X R:0x%08X G:0x%08X B:0x%08X\n", 
        ddpf.dwRGBBitCount, 
        (ddpf.dwFlags & (DDPF_ALPHA|DDPF_ALPHAPIXELS)) ? "ALPHA" : "", 
        (ddpf.dwFlags & (DDPF_RGB)) ? "RGB" : "", 
        (ddpf.dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4)) ? "PAL" : "", 
        ddpf.dwRGBAlphaBitMask, ddpf.dwRBitMask, ddpf.dwGBitMask, ddpf.dwBBitMask);

  return(TRUE);
}


// ===========================================================================
// Function:    RestoreSurface
//
// Description: Resores the surface memory and refreshes the contents using
//              the passed in surface.
//
// Params:      surface:  The surface to use to restore the contents of this
//                        surface
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the resoration was successful, or FALSE if it
//              was not.
// ===========================================================================
BOOL Surface::RestoreSurface(Surface& surface)
{
  // First check if the surface is lost
  BOOL bLost = FALSE;

  // If our surface is lost we will need to restore it
  if (bLost = IsLost())
    bLost = Restore();

  // If we were unable to restore the surface, then we won't be able to
  //  refresh its contents either
  if (!bLost) return(FALSE);

  // We should also check if the surface we are copying from is lost as well
  if (surface.IsLost())
    return(FALSE);

  // Everything seems to be okay, so copy the surface
  return(CopySurface(surface));
}


// ===========================================================================
// Function:    Blt
//
// Description: Wrapper around the IDirectDrawSurface::Blt() call.  Does some
//              extra checking to ensure that the blit succeeds but it will
//              NOT resort to using GDI if the blit fails because there is no
//              blitting hardware.  This is a wrapper function, not an
//              all-purpose function.
//
// Params:      rctDest:  The area on the destination where we are to blit to
//              pDDSSrc:  The surface we are blitting from
//              rctSrc:   The area on the source that we are to blit
//              dwFlags:  Blitting flags
//              ddBltFx:  A DDBLTFX (Blitting effects) structure used to
//                        describe the blitting effects we want to use
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the blit was successful, FALSE if not.
// ===========================================================================
BOOL Surface::Blt(RECT& rctDest, PDDSURFACE pDDSSrc, RECT& rctSrc,
                  DWORD dwFlags, DDBLTFX& ddBltFx)
{
  ASSERT(NULL != m_pDDSurface);

  HRESULT hResult = NULL;

  // Make sure that the blit will wait until finished.  This was added because
  //  in all previous implementations of blitting functions using DirectX
  //  they always had a loop and checked continuously to see if the surface
  //  was busy after failing the blit.  Well, if you're going to wait until
  //  the blit is successful, you'd might as well let DirectX do the waiting.
  dwFlags |= DDBLT_WAIT;

  // Do the blit
  hResult = m_pDDSurface->Blt(&rctDest, pDDSSrc, &rctSrc, dwFlags, &ddBltFx);
  
  // Were we successful?
  if (!FAILED(hResult)) return(TRUE);

  // We weren't successful, check a few things

  // Is one of the surfaces lost?
  if (DDERR_SURFACELOST == hResult)
  {
    // Destination surface
    if (DDERR_SURFACELOST == m_pDDSurface->IsLost())
    {
      if (FAILED(m_pDDSurface->Restore()))
        return(FALSE);  // We were unable to restore the surface
    }
    // Source surface
    if (DDERR_SURFACELOST == pDDSSrc->IsLost())
    {
      if (FAILED(pDDSSrc->Restore()))
        return(FALSE);  // We were unable to restore the surface
    }
    
    // Try again after restoring.  If we fail then something is wrong.
    if (FAILED(m_pDDSurface->Blt(&rctDest, pDDSSrc, &rctSrc, dwFlags, &ddBltFx)))
      return(FALSE);
  }
  else
  {
    // Some other error occured
    REPORTERR(hResult);
  }
  
  return(FALSE);
}


// ===========================================================================
// Function:    BltFast
//
// Description: Wrapper around the IDirectDrawSurface::BltFast() call.  Does
//              some extra checking to ensure that the blit succeeds but it
//              will NOT resort to using GDI if the blit fails because there
//              is not blitting hardware.  This is a wrapper function, not an
//              all-purpose function.
//
// Params:      dwX, dwY:   The x,y position of where the blit is to start
//              pDDSSrc:    The surface we are blitting from
//              rctSrc:     The area on the source that we are to blit
//              dwTrans:    Blitting flags
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the blit was successful, FALSE if not.
// ===========================================================================
BOOL Surface::BltFast(DWORD dwX, DWORD dwY, PDDSURFACE pDDSSrc, RECT& rctSrc,
                      DWORD dwTrans)
{
  ASSERT(NULL != m_pDDSurface);

  HRESULT hResult = NULL;

  // Make sure that the blit will wait until finished.  This was added because
  //  in all previous implementations of blitting functions using DirectX
  //  they always had a loop and checked continuously to see if the surface
  //  was busy after failing the blit.  Well, if you're going to wait until
  //  the blit is successful, you'd might as well let DirectX do the waiting.
  dwTrans |= DDBLTFAST_WAIT;

  // Do the blit
  hResult = m_pDDSurface->BltFast(dwX, dwY, pDDSSrc, &rctSrc, dwTrans);
  
  // Were we successful?
  if (!FAILED(hResult)) return(TRUE);

  // We weren't successful, check a few things

  // Is one of the surfaces lost?
  if (DDERR_SURFACELOST == hResult)
  {
    // Destination surface
    if (DDERR_SURFACELOST == m_pDDSurface->IsLost())
    {
      if (FAILED(m_pDDSurface->Restore()))
        return(FALSE);  // We were unable to restore the surface
    }
    // Source surface
    if (DDERR_SURFACELOST == pDDSSrc->IsLost())
    {
      if (FAILED(pDDSSrc->Restore()))
        return(FALSE);  // We were unable to restore the surface
    }
    
    // Try again after restoring.  If we fail then something is wrong.
    if (FAILED(m_pDDSurface->BltFast(dwX, dwY, pDDSSrc, &rctSrc, dwTrans)))
      return(FALSE);
  }
  else
  {
    // Some other error occured
    REPORTERR(hResult);
  }
  
  return(FALSE);
}


// ===========================================================================
// Function:    GDIBlit
//
// Description: Blits from one surface to another use the GDI BitBlit function
//
// Params:      dwX, dwY:   The x,y position on the desitination surface where
//                          the blit will start
//              sourceSurf: The source surface we are blitting from
//              rctSrc:     The rectangle of the source we are blitting
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the blit was successful, or FALSE if it wasn't
// ===========================================================================
BOOL Surface::GDIBlit(DWORD dwX, DWORD dwY, Surface& sourceSurf,
                      const RECT& rctSrc)
{
  TRACE("WARNING!!!\r\n\t\tUsing GDI to blit between surfaces!\r\n\r\n");

  BOOL bSuccess = FALSE;

  // Get the DC for the surfaces
  HDC hDestDC   = GetDC();
  HDC hSourceDC = sourceSurf.GetDC();

  if (!hSourceDC || !hDestDC) return(FALSE);

  // Blit the surfaces
  bSuccess = ::BitBlt(hDestDC, dwX, dwY, rctSrc.right - rctSrc.left,
                      rctSrc.bottom - rctSrc.top, hSourceDC, 
                      rctSrc.left, rctSrc.top, SRCCOPY);

  // Release the Device Contexts
  VERIFY(ReleaseDC(hDestDC));
  VERIFY(sourceSurf.ReleaseDC(hSourceDC));

  return(bSuccess);
}


// ===========================================================================
// Function:    BltFast
//
// Description: Blits from another surface to this one
//
// Params:      dwX, dwY:   The x,y position on the desitination surface where
//                          the blit will start
//              sourceSurf: The source surface we are blitting from
//              rctSrc:     The rectangle of the source we are blitting
//              dwTrans:    Blitting flags
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the blit was successful, or FALSE if it wasn't
// ===========================================================================
BOOL Surface::BltFast(DWORD dwX, DWORD dwY, Surface& sourceSurf, 
                      RECT& rctSrc, DWORD dwTrans)
{
  // Check the driver for blitting hardware before we ask it to do the blit
  if (pc3D::GetDDrawDriver()->HasHardwareBlitter())
    return(BltFast(dwX, dwY, sourceSurf.m_pDDSurface, rctSrc, dwTrans));

  // The hardware is not cabapable of blitting, use GDI
  return(GDIBlit(dwX, dwY, sourceSurf, rctSrc));
}


// ===========================================================================
// Function:    SetAsActiveTexture
//
// Description: Sets this surface as the active texture to be used by the
//              Direct3D rendering device.
//
// Params:      dwStage:  Indicates which texture level to set this texture to
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the texture was successfully set as the active
//              texture on the specified stage, or FALSE if something went 
//              wrong.
// ===========================================================================
BOOL Surface::SetAsActiveTexture(DWORD dwStage /*= 0*/)
{
  // It kinda helps if this surface is a texture
  ASSERT(IsTexture());

  // Get the 3D device
  D3DDevice* p3DDevice = pc3D::Get3DDevice();
  ASSERT(NULL != p3DDevice);

  // Make sure the correct texture filtering mode is active
  VERIFY(p3DDevice->SetTextureStageState(dwStage, D3DTSS_MIPFILTER, 
                                         IsMipMap() ? 
                                         pc3D::m_TxtrFilters.byMipMapFilter :
                                         D3DTFP_NONE));

  // Set the texture
  return(p3DDevice->SetTexture(dwStage, GetD3DTexture()));
}


// ===========================================================================
// Function:    SaveToBmpFile
//
// Description: Saves the contents of the surface to a .BMP file
//
// Params:      szFilename: The filename of the .BMP file we are going to
//                          save the surface to
//              pRect:      Optional rectangle specifying the area to save to
//                          the file
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the surface was successfully saved, or FALSE
//              if it was not.
// ===========================================================================
BOOL Surface::SaveToBmpFile(LPCSTR szFilename, LPRECT pRect /*= NULL*/)
{
  DWORD             dwWritten = 0;
  BITMAPFILEHEADER  bmpFileHdr;
  DDSURFDESC        ddsd;
  RECT              rctBitmap = { 0, 0, 0, 0 };

  // First thing we need to do is create ourselves a new file
  HANDLE hFile = ::CreateFile(szFilename, GENERIC_WRITE, FILE_SHARE_WRITE,
                              NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  // If we couldn't create the new file, we won't be able to save the bitmap
  if (INVALID_HANDLE_VALUE == hFile)
    return(FALSE);

  // Get the surface description
  if (!GetSurfaceDesc(ddsd))
    return(FALSE);

  // Initialize our bitmap rectangle
  if (pRect)
  {
    rctBitmap.left    = pRect->left;
    rctBitmap.top     = pRect->top;
    rctBitmap.right   = ((int)ddsd.dwWidth < abs(pRect->right - pRect->left)) ?
                        ((int)ddsd.dwWidth - pRect->left) : pRect->right;
    rctBitmap.bottom  = ((int)ddsd.dwHeight < abs(pRect->bottom - pRect->top)) ?
                        ((int)ddsd.dwHeight - pRect->top) : pRect->bottom;
  }
  else
  {
    rctBitmap.right   = ddsd.dwWidth;
    rctBitmap.bottom  = ddsd.dwHeight;
  }

  ASSERT(rctBitmap.right > rctBitmap.left);
  ASSERT(rctBitmap.bottom > rctBitmap.top);

  // Prepare the file header
  bmpFileHdr.bfType = 0x4D42;        // 0x42 = "B", 0x4D = "M"

  // Calculate the size in BYTES
  bmpFileHdr.bfSize = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) +
                      (rctBitmap.right - rctBitmap.left) * 
                      (rctBitmap.bottom - rctBitmap.top) * 
                      (ddsd.ddpfPixelFormat.dwRGBBitCount / 8); 

  bmpFileHdr.bfReserved1 = 0;
  bmpFileHdr.bfReserved2 = 0;
  bmpFileHdr.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

  // If we're in 8 bit mode, add the size info for the palette
  if (8 == ddsd.ddpfPixelFormat.dwRGBBitCount)
  {
    bmpFileHdr.bfSize     += sizeof(RGBQUAD) * 256;
    bmpFileHdr.bfOffBits  += sizeof(RGBQUAD) * 256;
  }

  // Write the header to the file
  if (!::WriteFile(hFile, &bmpFileHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL))
    return(FALSE);

  BOOL bSuccess = FALSE;

  // Now, based upon bit depth, write the surface info to the new file in the 
  //  .bmp format
  if (16 <= ddsd.ddpfPixelFormat.dwRGBBitCount)
    bSuccess = Save24BitBmpFile(hFile, rctBitmap);
  else if (8 == ddsd.ddpfPixelFormat.dwRGBBitCount)
    bSuccess = Save8BitBmpFile(hFile, rctBitmap);

  // Make sure we close the file
  VERIFY(::CloseHandle(hFile));

  // If we weren't successful, delete the partial file we created
  if (!bSuccess)
    VERIFY(::DeleteFile(szFilename));

  // Finish by closing the file
  return(bSuccess);
}


// ===========================================================================
// Global functions ==========================================================
// ===========================================================================

extern "C" {


// ===========================================================================
// Function:    LoadBMP
//
// Description: Loads a bitmap from a file or resource.
//
// Params:      szBitmapName: The name of the bitmap to load.  This is either
//                            a resource string (usually made with
//                            MAKEINTRESOURCE()) or the name of a file.
//
// Returns:     HBITMAP
//
// Comments:    Returns a handle to the loaded bitmap if it was successfully
//              loaded or NULL if it was not.
// ===========================================================================
HBITMAP LoadBMP(LPCSTR szBitmapName)
{
  HBITMAP hBitmap = NULL;

  // Try loading the bitmap as a file
  hBitmap = (HBITMAP)LoadImage(NULL, szBitmapName, IMAGE_BITMAP, 0, 0,
                               LR_LOADFROMFILE | LR_CREATEDIBSECTION);

  // If that didn't work Attempt to load the bitmap from the application's
  //  resources
  if (!hBitmap)
  {
    hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmapName,
                                 IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
  }

  return(hBitmap);
}


// ===========================================================================
// Function:    GetBitShift
//
// Description: Determines how many bits we need to shift a given bitmask by
//              so that it no longer has any zeroes in the lower bits.
//
// Params:      dwBitMask:  The bit mask to find the shift of
//
// Returns:     DWORD
//
// Comments:    Returns the number of bits need to shift the bit mask.
// ===========================================================================
DWORD GetBitShift(DWORD dwBitMask)
{
  DWORD dwValue = dwBitMask;
  if (!dwValue) return 0;

  // Determine how many bits we need to shift the mask by so it doesn't have
  //  any zeroes in the lower bits
  for (DWORD dwShift = 0; !(dwValue & 1);  dwShift++, dwValue >>= 1);

  return(dwShift);
}


// ===========================================================================
// Function:    GetBitDepth
//
// Description: Determine the bit depth of a given bit mask
//
// Params:      dwBitMask:  The bit mask to determine the depth of
//
// Returns:     DWORD
//
// Comments:    Returns the bit depth of the bit mask
// ===========================================================================
DWORD GetBitDepth(DWORD dwBitMask)
{
  // Shift the bitmask so it starts at bit 0
  DWORD dwValue = dwBitMask >> ::GetBitShift(dwBitMask);
  DWORD dwMask  = 1;
  DWORD dwCount = 0;

  // Count how many bits are in the bit mask
  while (dwMask & dwValue)
  { 
    dwMask <<= 1;
    dwCount++;
  }

  return(dwCount);
}


// ===========================================================================
// Function:    CreateNewSurface
//
// Description: Standard C style call to create a surface from a bitmap
//
// Params:      hBitmap:      Handle to the bitmap to use to create the surface
//              bIsTexture:   Are we creating a texture?
//              bUseVideoMem: Are we supposed to use video memory? (Ignored if
//                            we are creating a texture)
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the surface is created successfully and FALSE
//              if it is not.
// ===========================================================================
Surface* CreateNewSurface(HBITMAP hBmp, BOOL bIsTexture /*= FALSE*/,
                          BOOL bUseVideoMem /*= FALSE*/)
{
  return(new Surface(hBmp, bIsTexture, bUseVideoMem));
}

} // exterm "C"

// ===========================================================================
// End Of File ===============================================================
// ===========================================================================
