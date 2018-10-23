// ===========================================================================
// Module:      DDrawWnd.cpp
//
// Description: DirectDraw window class implementation.  This class represents
//              a "window" which is the primary and secondary surfaces where
//              all of the rendering will take place.
//
// Created:     April 19, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================


#include "PC3DHdr.h"

#include "DDrawWnd.h"
#include "D3DDevice.h"
#include "DDrawDrv.h"
#include "DDDrvMgr.h"

#include "DXINI.h"


// ===========================================================================
// DDrawWnd class ============================================================
// ===========================================================================


// ===========================================================================
// PRIVATE: DDrawWnd member functions ========================================
// ===========================================================================


// ===========================================================================
// Function:    Destroy
//
// Description: Destroys resources allocated by the DDrawWnd
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DDrawWnd::Destroy()
{
  // Make sure the 3D device doesn't have any references to our surfaces
  m_pD3DDevice->SetTexture(0, NULL);
  VERIFY(m_pDDDriver->EvictManagedTextures());

  DESTROYPOINTER(m_pD3DDevice);
  
  RELEASESURFACE(m_ZBuffer);
  RELEASESURFACE(m_Render);
  RELEASESURFACE(m_Primary);

  DESTROYPOINTER(m_pDDDriver);
}


// ===========================================================================
// PROTECTED: DDrawWnd member functions ======================================
// ===========================================================================


// ===========================================================================
// Function:    FindDDDriver
//
// Description: Attempts to find the DirectDraw driver having the GUID passed
//              in.
//
// Params:      ddDrvMgr: The DirectDraw driver manager that we are to use to
//                        help us find an appropriate driver
//              dwRequiredMem: The amount of required video memory
//
// Returns:     DDrawDriver*
//
// Comments:    Returns a pointer to the DirectDraw driver if one is found
//              with the required GUID or NULL if it could not be found.
// ===========================================================================
DDrawDriver* DDrawWnd::FindDDDriver(DDDrvMgr& ddDrvMgr, DWORD dwRequiredMem)
{
  m_bINIFileGenerated = FALSE;

  // If the DirectX INI file has not already been created, we need to create
  //  it now
  if (!DXINI_FileExists())
  {
    // Otherwise, the INI file can't be found so we better create it
    m_bINIFileGenerated = ddDrvMgr.GenerateDXINIFile(dwRequiredMem);
    if (!m_bINIFileGenerated)
    {
      // We failed to generate the INI file.  If the user specified that they
      //  want to choose their settings, go ahead and ask them.
      if (DXINI_CheckForChooseSettings())
      {
        VERIFY(ddDrvMgr.SelectDriverDialog());
      }
      else
        return(NULL);
    }
  }

  // Get the driver from the INI file
  DDrawDriver* pDDDriver = ddDrvMgr.GetDDDriverFromINI();

  // If we managed to obtain a proper driver, we're done
  if (!pDDDriver)
  {
    // If we already generated the INI file and we were unable to get the
    //  driver, we're in big trouble
    if (m_bINIFileGenerated) return(NULL);

    m_bINIFileGenerated = ddDrvMgr.GenerateDXINIFile(dwRequiredMem);
    if (m_bINIFileGenerated)
    {
      // Get the driver from the INI file
      pDDDriver = ddDrvMgr.GetDDDriverFromINI();
    }
    else
    {
      // We failed to generate the INI file.  If the user specified that they
      //  want to choose their settings, go ahead and ask them.
      if (DXINI_CheckForChooseSettings())
      {
        VERIFY(ddDrvMgr.SelectDriverDialog());
        pDDDriver = ddDrvMgr.GetDDDriverFromINI();
      }
      else
        return(NULL);
    }
  }

  // If we still don't have a DirectDraw driver at this point, try getting
  //  the first one from the driver manager.
  if (!pDDDriver && 0 != ddDrvMgr.GetNumDDDrivers())
    pDDDriver = ddDrvMgr.RemoveDDDriver(0);

  return(pDDDriver);
}


// ===========================================================================
// Function:    FindD3DDevice
//
// Description: Finds the requested D3D device from the DirectDraw driver
//
// Params:      ddDrvMgr:   The DirectDraw driver manager that we are to use
//                          to help us find an appropriate device
//              dwRequiredMem: The amount of required video memory
//
// Returns:     D3DDevice*
//
// Comments:    Returns a pointer to a D3DDevice if successful, or NULL if an
//              appropriate device could not be found.
// ===========================================================================
D3DDevice* DDrawWnd::FindD3DDevice(DDDrvMgr& ddDrvMgr, DWORD dwRequiredMem)
{
  // We require that the DirectDraw driver haven been initialized
  ASSERT(NULL != m_pDDDriver); if (!m_pDDDriver) return(NULL);

  // Get the D3D device from the INI file
  D3DDevice* pD3DDevice = ddDrvMgr.GetD3DDeviceFromINI(*m_pDDDriver);

  if (!pD3DDevice)
  {
    // If we were unsuccessful in retrieving the selected D3D device from the
    //  INI file and we are sure we have recently generated it, then something
    //  is wrong in a big way.
    if (m_bINIFileGenerated)
      return(NULL);
    else
    {
      // We know that we were unsuccessful in acquiring the D3D device from
      //  the INI file.  So try regenerating the INI file.
      
      // Generate the INI file
      m_bINIFileGenerated = ddDrvMgr.GenerateDXINIFile(dwRequiredMem);
      if (m_bINIFileGenerated)
        pD3DDevice = ddDrvMgr.GetD3DDeviceFromINI(*m_pDDDriver);
    }
  }

  // By the time we reach this point we have tried many things.  If we still
  //  don't have a D3D device, just take the first one from the DDrawDriver.
  if (!pD3DDevice && 0 != m_pDDDriver->GetNumD3DDevices())
    pD3DDevice = m_pDDDriver->RemoveD3DDevice(0);

  return(pD3DDevice);
}


// ===========================================================================
// Function:    CreateSurfaces
//
// Description: Creates the primary and back buffer surfaces
//
// Params:      dwWidth:  Width of the rendering surface
//              dwHeight: Height of the rendering surface
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the surfaces are successfully created and
//              FALSE otherwise.
// ===========================================================================
BOOL DDrawWnd::CreateSurfaces(DWORD dwWidth, DWORD dwHeight)
{
  HRESULT     hResult = NULL;
  DDSURFDESC  ddSurfDesc;
  ZeroMemory(&ddSurfDesc, sizeof(DDSURFDESC));
  ddSurfDesc.dwSize   = sizeof(DDSURFDESC);
  ddSurfDesc.dwFlags  = DDSD_CAPS;

  // If we are running in windowed mode, or we don't have enough memory
  //  to create everything in hardware, create the back surface in
  //  system memory
  if (IsWindowed() || !m_pD3DDevice->IsHardware())
  {
    ddSurfDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;

    // Create the primary surface
    if (!m_Primary.Create(ddSurfDesc)) return(FALSE);

    // Get the surface description of the primary surface
    ddSurfDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    VERIFY(m_Primary.GetPixelFormat(ddSurfDesc.ddpfPixelFormat));

    ddSurfDesc.dwWidth  = dwWidth;
    ddSurfDesc.dwHeight = dwHeight;
    ddSurfDesc.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN;
    ddSurfDesc.dwFlags  = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | 
                          DDSD_PIXELFORMAT;

    // Make sure were using the correct memory
    if (m_pD3DDevice->IsHardware()) 
      ddSurfDesc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
    else
      ddSurfDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

    // Now create the back buffer
    if (!m_Render.Create(ddSurfDesc)) return(FALSE);

    // Make sure we are set for windowed mode.  This is needed for the
    //  case where we are using software.
    m_bWindowed = TRUE;
  }
  else
  {
    ddSurfDesc.dwFlags      |= DDSD_BACKBUFFERCOUNT;
    ddSurfDesc.dwBackBufferCount  = 1;     

    // Create a D3D compatible surface
    ddSurfDesc.ddsCaps.dwCaps     = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | 
                                    DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;
    // Create the surfaces
    if (!m_Primary.Create(ddSurfDesc)) return(FALSE);

    // Get the back buffer from the flipping chain
    DDSURFCAPS ddSCaps = { DDSCAPS_BACKBUFFER, 0, 0, 0 };
    if (!m_Primary.GetAttachedSurface(ddSCaps, m_Render)) return(FALSE);

    // GetAttachedSurface automatically dereferences the surface before
    //  returning it, so we need to add a reference here
    m_Render.GetDDSurface()->AddRef();
  }

  // If the primary surface is palettized, we had better add a palette to it
  if (m_Primary.IsPalettized())
  {
    // Create ourselves a blank palette
    PALETTEENTRY aPalEntries[256];
    PDDPALETTE pPalette = m_pDDDriver->CreatePalette(DDPCAPS_8BIT | 
                                                     DDPCAPS_ALLOW256,
                                                     aPalEntries);
    ASSERT(NULL != pPalette);

    // Add the palette to the primary and rendering surfaces
    m_Primary.SetPalette(pPalette);
    m_Render.SetPalette(pPalette);

    // Dereference the palette now so we won't have to later
    RELEASEPOINTER(pPalette);
  }

  return(TRUE);
}


// ===========================================================================
// Function:    CreateZBuffer
//
// Description: Creates the Z Buffer for the 3D device
//
// Params:      dwWidth:  The width of the ZBuffer
//              dwHeight: The height of the ZBuffer
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the ZBuffer was successfully create and FALSE
//              otherwise.
// ===========================================================================
BOOL DDrawWnd::CreateZBuffer()
{
  // We need a valid D3DDevice object
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice)  return(FALSE);
  ASSERT(NULL != m_pDDDriver);  if (!m_pDDDriver)   return(FALSE);

  // We require that the 3D device we are planning to use has enumerated its
  //  Z Buffer types
  int nSize = m_pD3DDevice->GetNumZBufferFormats();
  ASSERT(0 < nSize); if (0 >= nSize) return(FALSE);

  HRESULT hResult = NULL;

  // We need to get the size of the render target
  DDSURFDESC ddRenderDesc;
  if (!m_Render.GetSurfaceDesc(ddRenderDesc)) return(FALSE);

  DDPIXELFORMAT*  pZBufFormat = NULL;

  DDSURFDESC ddZBuffDesc;
  ZeroMemory(&ddZBuffDesc, sizeof(DDSURFDESC));

  ddZBuffDesc.dwSize    = sizeof(DDSURFDESC);
  ddZBuffDesc.dwWidth   = ddRenderDesc.dwWidth;
  ddZBuffDesc.dwHeight  = ddRenderDesc.dwHeight;
  ddZBuffDesc.dwFlags   = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT |
                          DDSD_CAPS;
  ddZBuffDesc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;

  // We need to create the Z Buffer in the appropriate section of memory
  //  depending on whether we are using hardware acceleration or not.
  if (m_pD3DDevice->IsHardware()) 
    ddZBuffDesc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
  else
    ddZBuffDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

  int i = 0;

  // Now all we need is a Z Buffer pixel format from the device
  while (!pZBufFormat && i < nSize)
  {
    pZBufFormat = m_pD3DDevice->GetZBufferFormat(i);

    // We require a 16 bit Z Buffer (or greater)
    if (16 > pZBufFormat->dwZBufferBitDepth)
    {
      pZBufFormat = NULL;
      i++;
    }
  }

  // If we were unable to acquire a Z Buffer format that we can use, we
  //  cannot create the Z Buffer
  if (!pZBufFormat) return(FALSE);

  CopyMemory(&(ddZBuffDesc.ddpfPixelFormat), pZBufFormat,
             sizeof(DDPIXELFORMAT));

  // Now create the Z Buffer
  if (!m_ZBuffer.Create(ddZBuffDesc)) return(FALSE);

  // Now attach the Z Buffer to the render target
  if (!m_Render.AddAttachedSurface(m_ZBuffer)) return(FALSE);

  return(TRUE);
}


// ===========================================================================
// Function:    ValidateRenderTarget
//
// Description: Ensures that the render target being used is compatible with
//              the 3D device we are going to be using
//
// Params:      renderTarget:  A render target surface
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the current 3D device supports the render
//              target and FALSE if it does not.
// ===========================================================================
BOOL DDrawWnd::ValidateRenderTarget(Surface& renderTarget)
{
  // Get the pixel format of the rendering target
  DDPIXELFORMAT ddPixelFormat;
  if (!renderTarget.GetPixelFormat(ddPixelFormat)) return(FALSE);

  BOOL bSupported = FALSE;

  // The rendering target is compatible if the 3D device supports its colour
  //  depth
  bSupported = m_pD3DDevice->ColourDepthSupported(ddPixelFormat.dwRGBBitCount);

  // Inform the user if the validation failed
  if (!bSupported)
  {
    char szError[MAX_PATH];
    sprintf(szError, "Your rendering surface's colour depth (%d bits per "
            "pixel) is not supported by the chosen 3D device\r\nTry "
            "fullscreen mode", ddPixelFormat.dwRGBBitCount);
    MSGBOX(szError, "FATAL ERROR!");
  }

  return(bSupported);
}


// ===========================================================================
// Function:    FlipToGDI
//
// Description: Flips to the surface representing the GDI surface
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
BOOL DDrawWnd::FlipToGDI()
{
  if (IsWindowed()) return(TRUE);
  if (m_pDDDriver)
    return(m_pDDDriver->FlipToGDISurface());
  return(FALSE);
}


// ===========================================================================
// PUBLIC: DDrawWnd member functions =========================================
// ===========================================================================


// ===========================================================================
// Function:    Create
//
// Description: Initializes an instance of a DDrawWnd class
//
// Params:      hWnd:         Window handle needed for DirectX initialization
//              dwWidth:      Width of the window
//              dwHeight:     Height of the window
//              dwBPP:        Colour depth (bits per pixel) of the window
//              bUseZBuffer:  Indicates if we are to use a Z Buffer
//              bMultitasking:Indicates we are using multitasking
//              nWindowMode:  Specifies full screen or windowed mode.
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if everything was initialized properly and FALSE
//              otherwise.
// ===========================================================================
BOOL DDrawWnd::Create(HWND hWnd, DWORD  dwWidth, DWORD dwHeight, DWORD dwBPP,
                      BOOL bUseZBuffer /*= TRUE*/, 
                      BOOL bMultitasking /*= FALSE*/,
                      WindowedMode nWindowMode /*= WINDOWED_USE_DEFAULT*/)
{
  DWORD dwValue   = 0xFFFFFFFF;
  ULONG nWidth    = dwWidth;
  ULONG nHeight   = dwHeight;
  ULONG nClrDepth = dwBPP;
  BOOL  bWindowed = TRUE;

  // Calculate how much memory the surfaces are going to use
  DWORD dwMem = (dwWidth * dwHeight * (dwBPP / 8) * 2);

  // If we are using a ZBuffer, we need to add that as well
  if (bUseZBuffer)
    dwMem += dwWidth * dwHeight * 2; // 16 bit zbuffer

  // We require a valid window handle to create the DDraw window
  ASSERT(NULL != hWnd); if (!hWnd) return(FALSE);
  m_hWindow = hWnd;

  // Create ourselves a driver manager
  DDDrvMgr driverManager(m_hWindow);

  // Check if we are to ask the user which driver they wish to use
  if (DXINI_FileExists() && DXINI_CheckForChooseSettings())
    DXINI_ChooseDriver();

  // First attempt to create the DirectDraw driver
  m_pDDDriver = FindDDDriver(driverManager, dwMem);
  ASSERT(NULL != m_pDDDriver); if (!m_pDDDriver) goto CREATION_FAILED;

  // Set the driver for the surfaces
  Surface::SetDDrawDriver(m_pDDDriver);

  // If we have set the application's resolution from the INI file, use
  //  that one
  if (DXINI_FileExists() && DXINI_UsingResolution())
  {
    if (!DXINI_GetSelectedResolution(nWidth, nHeight, nClrDepth))
    {
      // We failed to read the setting from the INI file, use the default
      nWidth = dwWidth;   nHeight = dwHeight;   nClrDepth = dwBPP;
    }
  }

  // Figure out if we are supposed to run in windowed mode or not.
  // If the caller doesn't explicity specify it, use the .INI file setting.
  if (nWindowMode == WINDOWED_USE_DEFAULT)
  {
    if (!DXINI_ReadDWORD(DXINI_SETTINGSSECTION, DXINI_WINDOWEDMODE, dwValue))
    {
      // We failed to read the value from the INI file.  Assume fullscreen
      bWindowed = FALSE;
    }
    else
      bWindowed = (BOOL)dwValue;
  }
  else if (nWindowMode == WINDOWED_USE_WINDOW)
    bWindowed = TRUE;
  else
    bWindowed = FALSE;

  // Set our multithreaded flag
  m_bMultithreaded = bMultitasking;

  // Enumerate all of the display modes of the selected driver
  m_pDDDriver->EnumerateDisplayModes();

  // Find an appropriate D3D device
  m_pD3DDevice = FindD3DDevice(driverManager, dwMem);

  // Verify our D3D device
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) goto CREATION_FAILED;

  // Get the windowed mode setting if we used the driver manager's selection
  //  dialog to choose our settings.
  if (driverManager.UsingSelectionFromDialog())
    bWindowed = driverManager.UsingWindowedMode();

  driverManager.DestroyDrivers();

  // When the display mode is set, we create the primary and rendering
  //  surfaces, along with the ZBuffer.  This will handle all of the stuff
  //  we are looking to accomplish at this point.
  m_bInitialized = SetDisplayMode(bWindowed, nWidth, nHeight, nClrDepth,
                                  bUseZBuffer);

  if (!m_bInitialized)
  {
    // We failed to set the display mode so check if we were attempting to
    //  use the INI settings.
    if (nWidth != dwWidth || nHeight != dwHeight || nClrDepth != dwBPP)
    {
      char szMsg[MAX_PATH];
      sprintf(szMsg, "Failed to set the selected display mode: %d x %d x %d.\r\n"
              "Attempting to use the default mode: %d x %d x %d\r\n",
              nWidth, nHeight, nClrDepth, dwWidth, dwHeight, dwBPP);
      MSGBOX(szMsg, "Warning");

      // We were using some setting from the INI file.  Try the default.
      m_bInitialized = SetDisplayMode(bWindowed, dwWidth, dwHeight, dwBPP,
                                      bUseZBuffer);
      if (m_bInitialized)
      {
        // We successfully set the display mode using the default settings.
        //  Set the INI file back to the default.
        VERIFY(DXINI_SetUsingResolution(FALSE));
      }
    }
  }

  // If setting the display mode failed, we are in big trouble
  ASSERT(m_bInitialized); if (!m_bInitialized) goto CREATION_FAILED;

  // Set the initial rendering states of the 3D device
  m_pD3DDevice->SetInitialRenderingStates();

  return(TRUE);

CREATION_FAILED:
  DESTROYPOINTER(m_pDDDriver);
  DESTROYPOINTER(m_pD3DDevice);

  return(FALSE);
}


// ===========================================================================
// Function:    GetAvailableVidMem
//
// Description: Obtains the available memory statistics
//
// Params:      dwTotal:    (OUT) Will be filled with the total amount of
//                                video memory available
//              dwFree:     (OUT) Will be filled with the total amount of
//                                free video memory available
//              dwTotalTex: (OUT) Will be filled with the total amount of
//                                texture memory available
//              dwFreeTex:  (OUT) Will be filled with the total amount of
//                                free texture memory available
//              pExtraCaps: (IN)  Contains extra capabilities to check for
//
// Returns:     BOOL
//
// Comments:    Returns if the available memories are obtained and FALSE
//              otherwise.
// ===========================================================================
BOOL DDrawWnd::GetAvailableVidMem(DWORD& dwTotal, DWORD& dwFree,
                                  DWORD& dwTotalTex, DWORD& dwFreeTex,
                                  const PDDSURFCAPS pExtraCaps /*= NULL*/)
{
  ASSERT(NULL != m_pDDDriver); if (!m_pDDDriver) return(FALSE);

  DDSURFCAPS ddSurfCaps;
  ZeroMemory(&ddSurfCaps, sizeof(DDSURFCAPS));
  ddSurfCaps.dwCaps = DDSCAPS_VIDEOMEMORY;

  if (pExtraCaps)
  {
    ddSurfCaps.dwCaps   |= pExtraCaps->dwCaps;
    ddSurfCaps.dwCaps2  |= pExtraCaps->dwCaps2;
    ddSurfCaps.dwCaps3  |= pExtraCaps->dwCaps2;
    ddSurfCaps.dwCaps4  |= pExtraCaps->dwCaps2;
  }

  // Get the total video memory available
  if (!m_pDDDriver->GetAvailableVidMem(ddSurfCaps, dwTotal, dwFree))
    return(FALSE);

  // Now get the texture memory stats
  ddSurfCaps.dwCaps = DDSCAPS_TEXTURE;
  if (pExtraCaps)
    ddSurfCaps.dwCaps |= pExtraCaps->dwCaps;

  if (!m_pDDDriver->GetAvailableVidMem(ddSurfCaps, dwTotalTex, dwFreeTex))
    return(FALSE);

  return TRUE;
} 


// ===========================================================================
// Function:    SetDisplayMode
//
// Description: Sets the display mode to the given resolution and colour depth
//
// Params:      bWindowed:    Are we using windowed mode?
//              nWidth:       Width of the new display mode
//              nHeight:      Height of the new display mode
//              nClrDepth:    Colour depth of the new display mode
//              bUseZBuffer:  Are we using a ZBuffer?
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the mode was successfully changed and FALSE if
//              it was not.
// ===========================================================================
BOOL DDrawWnd::SetDisplayMode(BOOL bWindowed, UINT nWidth, UINT nHeight,
                              UINT nClrDepth /*= 0*/, BOOL bUseZBuffer /*= TRUE*/)
{
  char  szError[MAX_PATH];
  DWORD dwValue             = 0;
  ULONG ulReferences        = 0;

  ASSERT(NULL != m_pDDDriver);  if (!m_pDDDriver) return(FALSE);
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return(FALSE);

  // First thing we need to do is free up everything =========================

  // Dump all of our textures to system memory
  m_pDDDriver->EvictManagedTextures();

  // Release the 3D device
  m_pD3DDevice->ReleaseD3DDevice();

  // Release the rendering surface 
  RELEASESURFACE(m_Render);

  // Release the ZBuffer
  RELEASESURFACE(m_ZBuffer);

  // Release the primary surface
  RELEASESURFACE(m_Primary);

  // Now put everything back together ========================================

#ifdef _DEBUG
  // Check the video memory to make sure that all of it has now been released
  DWORD dwTotal = 0, dwFree = 0;
  DDSURFCAPS ddSurfCaps;
  ZeroMemory(&ddSurfCaps, sizeof(DDSURFCAPS));
  ddSurfCaps.dwCaps = DDSCAPS_VIDEOMEMORY;
  VERIFY(m_pDDDriver->GetAvailableVidMem(ddSurfCaps, dwTotal, dwFree));
  if (dwTotal != dwFree)
  {
    sprintf(szError, "DDrawWnd::SetDisplayMode():  After releasing all items "
            "from video memory there is still %d bytes allocated.\r\n",
            dwTotal - dwFree);
    MSGBOX(szError, "WARNING!");
  }
#endif // _DEBUG

  m_bWindowed = bWindowed;

  // If the colour depth was not specified, use what we are currently using
  if (0 == nClrDepth)
  {
    DDSURFDESC ddsd;
    if (!m_pDDDriver->GetDisplayMode(ddsd)) goto SET_DISPLAY_FAILED;
    nClrDepth = ddsd.ddpfPixelFormat.dwRGBBitCount;
  }

  // Before we go setting the cooperative level and the display mode
  //  we should make sure that the display mode that has been selected
  //  is supported by the driver
  if (!m_pDDDriver->SupportsDisplayMode(nWidth, nHeight, nClrDepth))
  {
    sprintf(szError, "The selected display driver does not support the "
            "requested video mode: %d x %d x %d\n", nWidth, nHeight, nClrDepth);
    MSGBOX(szError, "FATAL ERROR!");
    return(FALSE);
  }

  // We need to set the window size here because when we set the cooperative
  //  level, it will generate a WM_SIZE message.

  if (IsWindowed())
  {
    GetClientRect (m_hWindow, &m_rctWindow);
    ClientToScreen (m_hWindow, (LPPOINT) &m_rctWindow.left);
    ClientToScreen (m_hWindow, (LPPOINT) &m_rctWindow.right);
  }
  else
  {
    m_rctWindow.left    = 0;
    m_rctWindow.top     = 0;
    m_rctWindow.right   = nWidth;
    m_rctWindow.bottom  = nHeight;
  }

  // Prepare the flags for setting the cooperative mode
  dwValue = DDSCL_ALLOWREBOOT;

  if (m_bMultithreaded)
    dwValue |= DDSCL_MULTITHREADED; // DDraw needs to know

  // Now attempt to set the display mode we want.
  if (IsWindowed())
  {
    // Set the cooperative level to share screen with other apps
    if (!m_pDDDriver->SetCooperativeLevel(dwValue | DDSCL_NORMAL))
      goto SET_DISPLAY_FAILED;
  }
  else
  {
    // Set the cooperative level to have exclusive rights to the screen
    if (!m_pDDDriver->SetCooperativeLevel(dwValue | DDSCL_EXCLUSIVE | 
                                          DDSCL_FULLSCREEN))
    {
      goto SET_DISPLAY_FAILED;
    }

    if (!m_pDDDriver->SetDisplayMode(nWidth, nHeight, nClrDepth))
    {
      // We were unable to acquire the display mode we want
      // We nee to do a search for something close to what we are looking for
      sprintf(szError, "Failed to set the display mode to %d x %d x %d.",
              nWidth, nHeight, nClrDepth);
      MSGBOX(szError, "FATAL ERROR!");
      goto SET_DISPLAY_FAILED;
    }
  }

  // Create the primary and rendering surfaces
  if (!CreateSurfaces(nWidth, nHeight))
  {
    sprintf(szError, "Failed to create the primary and rendering surfaces. "
            "Possibly out of memory.");
    MSGBOX(szError, "FATAL ERROR!");
    goto SET_DISPLAY_FAILED;
  }

  // Make sure that the rendering target surface is compatible with the 3D
  //  device
  if (!ValidateRenderTarget(m_Render))
    goto SET_DISPLAY_FAILED;

  // Now create ourselves a Z Buffer if one is requested
  if (bUseZBuffer)
  {
    if (!CreateZBuffer())
    {
      sprintf(szError, "Failed to create the Z Buffer. Possibly out of memory.");
      MSGBOX(szError, "FATAL ERROR!");
      goto SET_DISPLAY_FAILED;
    }
  }

  // Now create the Direct3D device
  if (!m_pD3DDevice->InitD3D(&m_Render)) goto SET_DISPLAY_FAILED;

  // Clear the main surfaces
  ClearMainSurfaces();

  // Restore all of the surfaces
  VERIFY(m_pDDDriver->RestoreAllSurfaces());

  return(TRUE);

SET_DISPLAY_FAILED:
  RELEASESURFACE(m_Render);
  RELEASESURFACE(m_ZBuffer);
  RELEASESURFACE(m_Primary);

  return(FALSE);
}


// ===========================================================================
// Function:    SetINIDisplayMode
//
// Description: Changes the display mode settings in the DirectX INI file
//
// Params:      nWidth:     The width of the new display mode
//              nHeight:    The height of the new display mode
//              nClrDepth:  The colour depth of the new display mode
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if successful, FALSE otherwise.
// ===========================================================================
BOOL DDrawWnd::SetINIDisplayMode(UINT nWidth, UINT nHeight, UINT nClrDepth)
{
  ASSERT(NULL != m_pDDDriver);  if (!m_pDDDriver) return(FALSE);

  char szMsg[MAX_PATH];

  // Check if the driver supports the display mode
  if (!m_pDDDriver->SupportsDisplayMode(nWidth, nHeight, nClrDepth))
  {
#ifdef _DEBUG
    sprintf(szMsg, "The selected DirectDraw driver does not support the "
            "desired display mode: %d x %d x %d\r\n", nWidth, nHeight, nClrDepth);
    MSGBOX(szMsg, "FATAL ERROR!");
#endif
    return(FALSE);
  }

  // Set the selected resolution in the INI file
  if (!DXINI_SetSelectedResolution(nWidth, nHeight, nClrDepth))
    return(FALSE);

  // Make sure that we are acutally going to use the new resolution
  if (!DXINI_SetUsingResolution(TRUE))
    return(FALSE);

#ifdef _DEBUG
  // Inform the user that they will need to restart before this change will
  //  take effect.
  sprintf(szMsg, "Thoust must needs restart thy application before this "
          "setting willst take effect\r\n");
  MSGBOX(szMsg, "Warning");
#endif

  return(TRUE);
}



// ===========================================================================
// Function:    ClearMainSurfaces
//
// Description: Clears the primary and rendering surfaces.
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DDrawWnd::ClearMainSurfaces()
{
  LPRECT  pRect   = NULL;

  // Clear the off-screen render surface
  VERIFY(m_Render.ClearSurface());

  if (IsWindowed())
  {
    // If we are running windowed mode we only want to clear
    // the area of the screen we are using.
    pRect = &m_rctWindow;
  }

  // Clear the primary surface
  VERIFY(m_Primary.SolidRect(pRect, 0));
}


// ===========================================================================
// Function:    RestoreMainSurfaces
//
// Description: Restores the primary, rendering and ZBuffer surfaces, if they
//              are lost.
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the restoration was successful, or FALSE if
//              it was not.
// ===========================================================================
BOOL DDrawWnd::RestoreMainSurfaces()
{
  if (!IsInitialized()) return(TRUE);

  // Restore the primary surface
  if (m_Primary.IsLost())
  {
    if (!m_Primary.Restore()) return(FALSE);
  }

  // Restore the rendering/back surface
  if (m_Render.IsLost())
  {
    if (!m_Render.Restore()) return(FALSE);
  }

  // Restore the ZBuffer
  if (m_ZBuffer.IsLost())
  {
    if (!m_ZBuffer.Restore()) return(FALSE);
  }

  return(TRUE);
}


// ===========================================================================
// Function:    FlipDisplay
//
// Description: Flips the primary and rendering buffers
//
// Params:      bWaitUntilReady:  Indicates if we are to specify as a flag to
//                                wait until the display is ready for the flip
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the flip is successfull or FALSE if it is not.
// ===========================================================================
BOOL DDrawWnd::FlipDisplay(BOOL bWaitUntilReady /*= TRUE*/)
{
  if (IsPaused()) return(TRUE);

  if (IsWindowed())
  {
    DWORD dwFlags = bWaitUntilReady ? DDBLTFAST_WAIT : 0;
    
    // We are in windowed mode so we need to copy the rendering buffer to the
    //  primary buffer
    return(m_Primary.BltFast(0, 0, m_Render, m_rctWindow, dwFlags));
  }
  else
  {
    // We are using fullscreen mode so we can just ask it to flip
    DWORD dwFlags = bWaitUntilReady ? DDFLIP_WAIT : 0;
    return(m_Primary.Flip(dwFlags));
  }
}


// ===========================================================================
// Function:    Pause
//
// Description: Pauses the application and flips to GDI
//
// Params:      bPause:   Are we pausing or resuming?
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DDrawWnd::Pause(BOOL bPause)
{
  if (!IsInitialized()) return;

  // Turning pausing on/off?
  if (bPause)
  {
    TRACE("\t...Pausing the application\n");
    if (!m_bPaused)
    {
      m_bPaused = TRUE;
      ::ShowCursor(TRUE);

      // Call the callback
      if (m_pPauseCallback)
        (*m_pPauseCallback)(m_bPaused, m_pCallbackData);

      if (m_pDDDriver)
      {
        // Flip to GDI surface
        VERIFY(m_pDDDriver->EvictManagedTextures());
        VERIFY(FlipToGDI());
      }

#ifdef _DEBUG
      // If we are in full-screen mode make sure we minimize the window
      if (!IsWindowed())
#endif
        // In the case of a release build, we want to always minimize the app
        ::ShowWindow(m_hWindow, SW_MINIMIZE);
    }
  }
  else
  {
    TRACE("\t...Unpausing the application\n");
    if (m_bPaused)
    {
      m_bPaused = FALSE;

      // Call the callback
      if (m_pPauseCallback)
        (*m_pPauseCallback)(m_bPaused, m_pCallbackData);

      ::ShowCursor(FALSE);
      ::ShowWindow(m_hWindow, SW_RESTORE);
    }
  }
}


// ===========================================================================
// MESSAGES: DDrawWnd window message functions ===============================
// ===========================================================================


// ===========================================================================
// Function:    SendMessage
//
// Description: Processes window messages sent to the DDrawWnd
//
// Params:      uiMsg:  The message to process
//              wParam: First message parameter
//              lParam: Second message parameter
//
// Returns:     LRESULT
//
// Comments:    
// ===========================================================================
LRESULT DDrawWnd::SendMessage(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  if (m_hWindow)
  {
    switch (uiMsg)
    {
      case WM_ACTIVATEAPP:
		    if ((BOOL)wParam)
        {
          TRACE("\r\n\t...Activating the application\r\n\r\n");
          Pause(FALSE);
        }
		    else
        {
          Pause(TRUE);
          TRACE("\r\n\t...Deactivating the application\r\n\r\n");
        }
        break;

      case WM_ENTERMENULOOP:      OnEnterMenuLoop(wParam, lParam);      break;
      case WM_EXITMENULOOP:       OnExitMenuLoop(wParam, lParam);       break;
      case WM_GETMINMAXINFO:      OnGetMinMaxInfo(wParam, lParam);      break;

      // Override this message so it doesn't paint the frame of our window
      case WM_NCPAINT:            return(1L);

      case WM_SYSCOMMAND:         OnSysCommand(wParam, lParam);         break;

      case WM_WINDOWPOSCHANGING:  OnWindowPosChanging(wParam, lParam);  break;

    }
  }

  // Do the default processing
  return(DefWindowProc(m_hWindow, uiMsg, wParam, lParam));
}


// ===========================================================================
// Function:    OnGetMinMaxInfo
//
// Description: Message handler for the WM_GETMINMAXINFO message
//
// Params:      wParam: Generic window message parameter
//              lParam: Generic window message parameter
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DDrawWnd::OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam)
{
  LPMINMAXINFO lpmmi = (LPMINMAXINFO)lParam;

  if (!IsWndMinimized())
  {
    // Get Width and height of current surface
    DWORD dwWidth  = abs(m_rctWindow.right - m_rctWindow.left);
    DWORD dwHeight = abs(m_rctWindow.bottom - m_rctWindow.top);

    // Prevent changes in size
    lpmmi->ptMaxTrackSize.x = dwWidth;
    lpmmi->ptMaxTrackSize.y = dwHeight;
    lpmmi->ptMinTrackSize.x = dwWidth;
    lpmmi->ptMinTrackSize.y = dwHeight;
  }
}


// ===========================================================================
// Function:    OnSysCommand
//
// Description: Called when a system command message is received.
//
// Params:      wParam: Generic window message parameter
//              lParam: Generic window message parameter
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DDrawWnd::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
  switch (wParam)
  {
    case SC_CLOSE:
      ::PostQuitMessage(0);
      break;
  }
}


// ===========================================================================
// Function:    OnWindowPosChanging
//
// Description: Message handler for the WM_WINDOWPOSCHANGING window message
//
// Params:      wParam: Generic window message parameter
//              lParam: Generic window message parameter
//
// Returns:     LRESULT
//
// Comments:    
// ===========================================================================
void DDrawWnd::OnWindowPosChanging(WPARAM wParam, LPARAM lParam)
{
  // We don't support moving Full-screen windows
  LPWINDOWPOS lpwp = (LPWINDOWPOS)lParam;

  if (IsInitialized() && lpwp && !IsWndMinimized())
  {
    RECT  rWin;
    DWORD dwWinW, dwWinH;
    DWORD dwSurfW, dwSurfH;

    // Get the size of the window
    ::GetWindowRect(m_hWindow, &rWin);
    dwWinW = abs(rWin.right - rWin.left);
    dwWinH = abs(rWin.bottom - rWin.top);

    // Get the size of our rendering surface
    dwSurfW = abs(m_rctWindow.right - m_rctWindow.left);
    dwSurfH = abs(m_rctWindow.bottom - m_rctWindow.top);

    // Make sure the surface is some reasonable size
    if (dwSurfW < 320)  dwSurfW = 320;
    if (dwSurfH < 200)  dwSurfH = 200;

    // Is our window the same size as our surface ?!?
    if ((dwSurfW != dwWinW) || (dwSurfH != dwWinH))
    {
      // No, then resize it
      lpwp->x   = m_rctWindow.left;
      lpwp->y   = m_rctWindow.top;
      lpwp->cx  = dwSurfW;
      lpwp->cy  = dwSurfH;

      // Force resize
      lpwp->flags &= ~(SWP_NOMOVE | SWP_NOSIZE);
    }
    else
    {
      // Prevent resizing and moving
      lpwp->flags |= (SWP_NOMOVE | SWP_NOSIZE);
    }
  }
}


// ===========================================================================
// End of File ===============================================================
// ===========================================================================
