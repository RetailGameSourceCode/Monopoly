// ===========================================================================
// Module:      DDrawDrv.cpp
//
// Description: DirectDraw Driver class implementation. Instances of this class
//              represent different DirectDraw devices (usually different
//              video cards) available on the machine we are running on.
//
// Created:     April 14, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================

#include "PC3DHdr.h"

#include "DDrawDrv.h"
#include "D3DDevice.h"
#include "DXINI.h"
#include "Surface.h"

// ===========================================================================
// DDrawDriver class =========================================================
// ===========================================================================


// ===========================================================================
// PRIVATE: DDrawDriver member functions =====================================
// ===========================================================================


// ===========================================================================
// Function:    Destroy
//
// Description: Destroys the DDrawDriver object.  Called by the destructor.
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DDrawDriver::Destroy()
{
  // Destroy the enumeration resources
  DestroyEnumerations();

  // Destroy the GUID
  DESTROYPOINTER(m_pGUID);

  // Release the Direct3D and DirectDraw interfaces
  RELEASEPOINTER(m_pD3D);
  RELEASEPOINTER(m_pDDraw);

  // Make sure DirectDraw was successfully realeased
//  ASSERT(0 == LASTREFCOUNT);

  Clear();
}


// ===========================================================================
// Function:    DestroyEnumerations
//
// Description: Destroy any allocated resources as a result of performing
//              the enumerations
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DDrawDriver::DestroyEnumerations()
{
  // Destroy the list of display modes
  for (int i = 0; i < m_nNumDisplayModes; i++)
  { 
    DESTROYPOINTER(m_apDisplayModes[i]);
  }
  m_nNumDisplayModes = 0;

  // Destroy the list of 3D Devices
  ReleaseD3DDevices();
}


// ===========================================================================
// PROTECTED: DDrawDriver member functions ===================================
// ===========================================================================


// ===========================================================================
// Function:    CreateDDraw
//
// Description: Creates the DirectDraw and Direct3D interfaces representing
//              this DDrawDriver object.
//
// Params:      pGUID:  The GUID to use to create the DirectDraw interface.
//                      Will be NULL if we are creating the DirectDraw
//                      interfaces for the primary display device.
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the interfaces were successfully create,
//              FALSE otherwise.
// ===========================================================================
BOOL DDrawDriver::CreateDDraw(LPGUID pGUID /*= NULL*/)
{
  HRESULT hResult = NULL;
  PDDRAW1 pDDraw1 = NULL;

  // Verify that we haven't already created the interfaces
  ASSERT(NULL == m_pDDraw); if (m_pDDraw) return(FALSE);
  ASSERT(NULL == m_pD3D);   if (m_pD3D)   return(FALSE);
  
  // Create the DirectDraw interface
  hResult = DirectDrawCreate(pGUID, &pDDraw1, NULL);
  RETURN_ON_ERROR(hResult, FALSE);

  // Now query for the most recent version of the interface
  hResult = pDDraw1->QueryInterface(CURRENT_DDRAW_GUID, (LPVOID*)&m_pDDraw);
  GOTO_ON_ERROR(hResult, CREATION_FAILURE);

  // Okay we have the most recent version of IDirectDraw, now query for
  //  IDirect3D
  hResult = m_pDDraw->QueryInterface(CURRENT_D3D_GUID, (LPVOID*)&m_pD3D);
  GOTO_ON_ERROR(hResult, CREATION_FAILURE);

  // Everything worked.  Make sure we release version 1 of IDirectDraw
  RELEASEPOINTER(pDDraw1);

  return(TRUE);

CREATION_FAILURE:
  RELEASEPOINTER(pDDraw1);
  RELEASEPOINTER(m_pD3D);
  RELEASEPOINTER(m_pDDraw);

  return(FALSE);
}


// ===========================================================================
// Function:    AddDisplayMode
//
// Description: Adds a new display mode to the list of modes supported by the
//              driver.  This function is called by the EnumDisplayModesCallback
//              function.
//
// Params:      ddSurfDesc: Surface description of the display mode
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the mode was added and FALSE if not.
// ===========================================================================
BOOL DDrawDriver::AddDisplayMode(const DDSURFDESC& ddSurfDesc)
{
  // Make sure we have enough room to add the new mode
  if (MAX_DISPLAYMODES <= m_nNumDisplayModes) return(FALSE);

  // Add the new mode
  m_apDisplayModes[m_nNumDisplayModes]          = new DDSURFDESC;
  CopyMemory(m_apDisplayModes[m_nNumDisplayModes], &ddSurfDesc, 
             sizeof(DDSURFDESC));

  // Increment the number of display modes we have
  m_nNumDisplayModes++;

  return(TRUE);
}


// ===========================================================================
// Function:    Add3DDevice
//
// Description: Adds a new 3D device to the list of devices available from
//              this driver.
//
// Params:      pNew3DDevice: Pointer to a new 3D Device object
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the device was added and FALSE if not
// ===========================================================================
BOOL DDrawDriver::Add3DDevice(D3DDevice* pNew3DDevice)
{
  // Validate our new device
  ASSERT(NULL != pNew3DDevice); if (!pNew3DDevice) return(FALSE);

  // If we don't have enough room to add the new device, we can't do anything
  if (MAX_3DDEVICES <= m_nNum3DDevices) return(FALSE);

  // Add the new device
  m_ap3DDevices[m_nNum3DDevices++] = pNew3DDevice;

  return(TRUE);
}


// ===========================================================================
// Function:    DetermineDriverMemory
//
// Description: Queries the driver for the amount of memory it has
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the memory is successfully determined, or
//              FALSE it is not.
// ===========================================================================
BOOL DDrawDriver::DetermineDriverMemory()
{
  ASSERT(NULL != m_pDDraw);

  DWORD dwTotal   = 0;
  DWORD dwFree    = 0;
  DWORD dwAGPMem  = 0;
  DWORD dwTexMem  = 0;

  DDSURFDESC ddsd;
  DDSURFCAPS ddSurfCaps;
  ZeroMemory(&ddSurfCaps, sizeof(DDSURFCAPS));
  ddSurfCaps.dwCaps = DDSCAPS_VIDEOMEMORY;

  // Get the amount of memory available
  if (!GetAvailableVidMem(ddSurfCaps, dwTotal, dwFree)) return(FALSE);

  // If the driver has separate memory for texture, we need to remove that
  //  from this calcuation

  // Get the amount of AGP memory
  ddSurfCaps.dwCaps = DDSCAPS_NONLOCALVIDMEM;
  if (!GetAvailableVidMem(ddSurfCaps, dwAGPMem, dwFree)) return(FALSE);

  // Get the amount of texture memory
  ddSurfCaps.dwCaps = DDSCAPS_TEXTURE;
  if (!GetAvailableVidMem(ddSurfCaps, dwTexMem, dwFree)) return(FALSE);

  // If there is no AGP memory and the total amount of memory is different
  //  than the total amount of texture memory, we know that the card must
  //  have separate texture memory.
  if (0 < dwAGPMem)
  {
    // Okay, we have some AGP memory.  Since AGP memory is used for textures
    //  we need to remove it from the texture memory calculation.
    ASSERT(dwTexMem >= dwAGPMem); // We better have more texture than AGP
    dwTexMem -= dwAGPMem;
  }

  // Remove the texture memory from out caculated total
  if (dwTexMem != dwTotal)
  {
    ASSERT(dwTotal > dwTexMem); // Better have more vid mem than tex mem
    dwTotal -= dwTexMem;
  }

  // If this is the primary display device, we need to add the amount of
  //  memory taken by the current display mode.
  if (!m_pGUID)
  {
    // Now we need to get the display mode
    if (!GetDisplayMode(ddsd)) return(FALSE);

    // NOTE:  The amount of memory reported as being the "total" amount 
    //        available from the GetAvailableVidMem() call is only what is
    //        available to DirectX at this time.  By setting the cooperative
    //        mode to fullscreen-exclusive, DirectX will also gain the
    //        memory currently being held by the windows desktop.  We need to
    //        add this to our calculated amount.
    dwTotal += ddsd.dwWidth * ddsd.dwHeight * 
               (ddsd.ddpfPixelFormat.dwRGBBitCount / 8);
  }

  // Now we have a value that is pretty darn close to the real amount of
  // video ram available to us.  Round off to the nearest megabyte.
  m_dwMemory = 1048576 * (DWORD)ceil((float)dwTotal / 1048576.0f);

  return(TRUE);
}


// ===========================================================================
// PUBLIC: DDrawDriver member functions ======================================
// ===========================================================================


// ===========================================================================
// Function:    Initialize
//
// Description: Initializes the DDrawDriver by creating its DirectDraw and 
//              Direct3D interfaces and enumerating its various components.
//
// Params:      hWnd:         Window handle
//              szDriverName: Name of the DirectDraw driver
//              szDriverDesc: Text description of the DirectDraw driver
//              pGUID:        The GUID to use to create the DirectDraw
//                            interface.  Will be NULL if we are creating the
//                            DirectDraw interface for the primary display
//                            device.
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if everything was successfully initialized,
//              FALSE otherwise.
// ===========================================================================
BOOL DDrawDriver::Initialize(HWND hWnd, LPSTR szDriverName, LPSTR szDriverDesc,
                             LPGUID pGUID /*= NULL*/)
{
  HRESULT hResult = NULL;

  // We require a valid window handle
  ASSERT(NULL != hWnd); if (!hWnd || !::IsWindow(hWnd)) return(FALSE);
  m_hWnd = hWnd;

  // First we need to create the DirectDraw and Direct3D interfaces
  if (!CreateDDraw(pGUID)) return(FALSE);

  // Make sure we remember the GUID
  if (pGUID)
  {
    m_pGUID = new GUID;
    ASSERT(NULL != m_pGUID);
    CopyMemory(m_pGUID, pGUID, sizeof(GUID));
  }

  ASSERT(NULL != m_pDDraw);
  ASSERT(NULL != m_pD3D);

  // Copy the driver info
  if (!szDriverName || !szDriverDesc)
  { 
    // The driver name and description were not provided so get it from the
    //  DirectDraw driver itself
    DDDEVICEIDENTIFIER drvInfo;
    GetDeviceID(drvInfo);
    strcpy(m_szName, drvInfo.szDriver);
    strcpy(m_szDescription, drvInfo.szDescription);
  }
  else
  {
    strcpy(m_szName, szDriverName);
    strcpy(m_szDescription, szDriverDesc);
  }

  // Get the driver capabilities
  ZeroMemory(&m_HardwareCaps, sizeof(DDCAPS));
  ZeroMemory(&m_SoftwareCaps, sizeof(DDCAPS));
  m_HardwareCaps.dwSize = sizeof(DDCAPS);
  m_SoftwareCaps.dwSize = sizeof(DDCAPS);
  hResult = m_pDDraw->GetCaps(&m_HardwareCaps, &m_SoftwareCaps);
  GOTO_ON_ERROR(hResult, INIT_FAILURE);

  // Figure out how much memory is availble for display on the card
  DetermineDriverMemory();

  return(TRUE);

INIT_FAILURE:

  // The initialization failed.  Make sure we clean up after ourselves.
  Destroy();
  return(FALSE);
}


// ===========================================================================
// Function:    WriteINIInfo
//
// Description: Writes the appropriate info to the DirectX INI file
//
// Params:      szSection:      The name of the section where we want to write
//                              the DirectDraw driver info
//              n3DDeviceCount: The current number of 3D devices having been
//                              added to the INI file
//
// Returns:     BOOL
//
// Comments:    Returs TRUE if the information was successfully written and
//              FALSE if not.
// ===========================================================================
BOOL DDrawDriver::WriteINIInfo(LPCSTR szSection, int& n3DDeviceCount)
{
  // We need to have enumerated all of the 3D devices for this to work
  ASSERT(0 < m_nNum3DDevices); if (0 >= m_nNum3DDevices) return(FALSE);

  // The only object that should be requesting a DDrawDriver to write its INI
  //  info is the DriverManager.  As such we will assume that the INI file
  //  path has already been initialized

  // First we need to write the Driver's name, description and GUID
  if (!DXINI_WriteString(szSection, DXINI_DRIVERNAME, m_szName))
    return(FALSE);
  if (!DXINI_WriteString(szSection, DXINI_DRIVERDESC, m_szDescription))
    return(FALSE);

  // We will only write out the GUID if it is not the default (NULL)
  if (m_pGUID)
  {
    if (!DXINI_WriteGUID(szSection, DXINI_DRIVERGUIDBASE, *m_pGUID))
      return(FALSE);
  }

  // Write the number of display modes
  if (!DXINI_WriteDWORD(szSection, DXINI_NUM_RESOLUTIONS, m_nNumDisplayModes))
  {
    return(FALSE);
  }

  // Write the number of 3D devices
  if (!DXINI_WriteDWORD(szSection, DXINI_NUM_3DDEVICES, m_nNum3DDevices))
  {
    return(FALSE);
  }

  TCHAR szKey[MAX_PATH];

  // Write the info for each enumerated D3D device
  for (int i = 0; i < m_nNum3DDevices; i++)
  {
    _stprintf(szKey, TEXT("%s_%02d_Name"), DXINI_3DDEVICE, n3DDeviceCount);

    // Write the name of the 3D device
    if (!DXINI_WriteString(szSection, szKey, m_ap3DDevices[i]->GetName()))
      return(FALSE);

    _stprintf(szKey, TEXT("%s_%02d_Description"), DXINI_3DDEVICE, n3DDeviceCount);

    // Write the description of the 3D device
    if (!DXINI_WriteString(szSection, szKey, 
                           m_ap3DDevices[i]->GetDescription()))
    {
      return(FALSE);
    }

    _stprintf(szKey, TEXT("%s_%02d_"), DXINI_3DDEVICE, n3DDeviceCount);

    // Now write the 3D device GUID
    if (!DXINI_WriteGUID(szSection, szKey, *(m_ap3DDevices[i]->GetGUID())))
      return(FALSE);

    // We only want to write the 3D capabilities if we are in DEBUG mode
#ifdef _DEBUG
    _stprintf(szKey, TEXT("%s_%02d"), DXINI_3DDEVICE, n3DDeviceCount);
    // We don't care if this fails
    m_ap3DDevices[i]->WriteDeviceCapsToINI(szKey);
#endif // _DEBUG

    n3DDeviceCount++;
  }

  // Write the info for each display mode supported by the driver
  for (i = 0; i < m_nNumDisplayModes; i++)
  {
    _stprintf(szKey, TEXT("%s_%03d"), DXINI_RESOLUTION, i);
    if (!DXINI_WriteRes(szSection, szKey, m_apDisplayModes[i]->dwWidth,
                        m_apDisplayModes[i]->dwHeight, 
                        m_apDisplayModes[i]->ddpfPixelFormat.dwRGBBitCount))
    {
      return(FALSE);
    }
  }

  return(TRUE);
}


// ===========================================================================
// Function:    EnumerateDisplayModes
//
// Description: Enumerates all of the display modes available to this
//              DirectDraw driver
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the enumeration was successful and FALSE if it
//              was not.
// ===========================================================================
BOOL DDrawDriver::EnumerateDisplayModes()
{
  // Enumerate the display modes
  HRESULT hResult = m_pDDraw->EnumDisplayModes(0, NULL, (LPVOID)this,
                                               EnumDisplayModesCallback);
  RETURN_ON_ERROR(hResult, FALSE);
  return(TRUE);
}


// ===========================================================================
// Function:    Enumerate3DDevices
//
// Description: Enumerates all of the D3D Devices available to this
//              DirectDraw driver
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the enumeration was successful and FALSE if it
//              was not.
// ===========================================================================
BOOL DDrawDriver::Enumerate3DDevices()
{
  // Enumerate the 3D Devices
  HRESULT hResult = m_pD3D->EnumDevices(EnumD3DDevicesCallback, (LPVOID)this);
  RETURN_ON_ERROR(hResult, FALSE);
  return(TRUE);
}


// ===========================================================================
// Function:    EnumerateFor3DDevice
//
// Description: Enuerates the D3D devices available until it finds the one
//              having the given GUID
//
// Params:      pGUID:  The GUID of the D3DDevice we are looking for
//
// Returns:     D3DDevice*
//
// Comments:    Returns a newly create D3DDevice object if one was found
//              having the GUID we were looking for.  Returns NULL if an error
//              occured or the device could not be found.
// ===========================================================================
D3DDevice* DDrawDriver::EnumerateFor3DDevice(LPGUID pGUID)
{
  // Verify our GUID
  ASSERT(NULL != pGUID); if (!pGUID) return(NULL);

  m_pSearchGUID = pGUID;

  // Enumerate the 3D devices
  Enumerate3DDevices();
  m_pSearchGUID = NULL;

  // Make sure that we found the device we were looking for
  if (1 != m_nNum3DDevices) return(NULL);

  // Remove the device
  D3DDevice* pD3DDevice = RemoveD3DDevice(0);
  m_nNum3DDevices = 0;

  // Return the device we found
  return(pD3DDevice);
}


// ===========================================================================
// Function:    ReleaseD3DDevices
//
// Description: Releases all of the D3D devices which were enumerated
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DDrawDriver::ReleaseD3DDevices()
{
  for (int i = 0; i < m_nNum3DDevices; i++)
  {
    DESTROYPOINTER(m_ap3DDevices[i]);
  }
  m_nNum3DDevices = 0;
}


// ===========================================================================
// Function:    FindPixelFormat
//
// Description: Searches through the list of enumerated display modes and
//              looks for mode having the requested colour depth.
//
// Params:      dwColourDepth:  The colour depth we are looking for
//              ddPixelFormat:  The DDPIXELFORMAT structure that will be
//                              filled with the found pixel format
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if a mode having the colour depth we are looking
//              for has been found.
// ===========================================================================
BOOL DDrawDriver::FindPixelFormat(DWORD dwColourDepth, 
                                  DDPIXELFORMAT& ddPixelFormat)
{
  if (0 >= m_nNumDisplayModes) return(FALSE);

  for (int i = 0; i < m_nNumDisplayModes; i++)
  {
    if (dwColourDepth == m_apDisplayModes[i]->ddpfPixelFormat.dwRGBBitCount && 
        640 == m_apDisplayModes[i]->dwWidth && 
        480 == m_apDisplayModes[i]->dwHeight)
    {
      CopyMemory(&ddPixelFormat, &(m_apDisplayModes[i]->ddpfPixelFormat),
                 sizeof(DDPIXELFORMAT));
      return(TRUE);
    }
  }

  return(FALSE);
}


// ===========================================================================
// Function:    SupportsDisplayMode
//
// Description: Determines if the given display mode is sported by this driver
//
// Params:      dwWidth:  Display mode width
//              dwHeight: Display mode height
//              dwBPP:    Display mode colour depth
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the mode is supported and FALSE if it is not
// ===========================================================================
BOOL DDrawDriver::SupportsDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP)
{
  if (0 >= m_nNumDisplayModes) return(FALSE);

  for (int i = 0; i < m_nNumDisplayModes; i++)
  {
    if (dwBPP == m_apDisplayModes[i]->ddpfPixelFormat.dwRGBBitCount && 
        dwWidth == m_apDisplayModes[i]->dwWidth                     && 
        dwHeight == m_apDisplayModes[i]->dwHeight)
    {
      return(TRUE);
    }
  }

  return(FALSE);
}


// ===========================================================================
// Function:    GetDisplayModeList
//
// Description: Gets the list of display modes available to the driver.
//              Filtering of the modes is accomplished using the function
//              parameters.
//
// Params:      aDisplayModes:        An array that will contain the list of
//                                    display modes. (Return value)
//              dwMinWidth:           The list of display modes may not contain
//                                    any mode having a width less than this.
//              dwMaxWidth:           The list of display modes may not contain
//                                    any mode having a width more than this.
//              dwMinHeight:          The list of display modes may not contain
//                                    any mode having a height less than this.
//              dwMaxHeight:          The list of display modes may not contain
//                                    any mode having a hight more than this.
//              dwSupportedBitDepths: The list of display modes may only 
//                                    contain modes having a bit depth that is
//                                    specified here.
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DDrawDriver::GetDisplayModeList(SurfDescArray& aDisplayModes,
                                     DWORD dwMinWidth /*= 0*/,
                                     DWORD dwMaxWidth /*= 0*/,
                                     DWORD dwMinHeight /*= 0xFFFFFFFF*/,
                                     DWORD dwMaxHeight /*= 0xFFFFFFFF*/,
                                     DWORD dwSupportedBitDepths /*= 0x0000000F*/)
{
  // Remove any previous items in the array
  aDisplayModes.RemoveAll();

  // If there aren't any available display modes, we can't get a list of them
  if (0 >= m_nNumDisplayModes) return;

  BIT_DEPTH eBitDepth = _0BIT;

  // Iterate through the display modes
  for (int i = 0; i < m_nNumDisplayModes; i++)
  {
    // Does this mode meet our criteria???
    if (m_apDisplayModes[i]->dwWidth >= dwMinWidth    &&
        m_apDisplayModes[i]->dwWidth <= dwMaxWidth    &&
        m_apDisplayModes[i]->dwHeight >= dwMinHeight  &&
        m_apDisplayModes[i]->dwHeight <= dwMaxHeight)
    {
      // The size is okay.  Check the bit depth.
      switch (m_apDisplayModes[i]->ddpfPixelFormat.dwRGBBitCount)
      {
        case 8:   eBitDepth = _8BIT;   break;
        case 16:  eBitDepth = _16BIT;  break;
        case 24:  eBitDepth = _24BIT;  break;
        case 32:  eBitDepth = _32BIT;  break;
      }

      // Is the bit depth allowed???
      if (dwSupportedBitDepths & eBitDepth)
      {
        // The mode is allowed.  Add it to our list
        aDisplayModes.Add(*m_apDisplayModes[i]);
      }
    }
  }
}


// ===========================================================================
// Function:    CreateSurface
//
// Description: Creates a surface using the description passed in
//
// Params:      ddSurfDesc:   The description of the surface to be created
//
// Returns:     PDDSURFACE
//
// Comments:    Returns a pointer to the newly created surface or NULL if
//              something went wrong.
// ===========================================================================
PDDSURFACE DDrawDriver::CreateSurface(DDSURFDESC& ddSurfDesc)
{
  // To create a surface we need a valid DirectDraw interface
  ASSERT(NULL != m_pDDraw); if (!m_pDDraw) return(NULL);

  PDDSURFACE pSurface = NULL;

  // Create the surface
  HRESULT hResult = m_pDDraw->CreateSurface(&ddSurfDesc, &pSurface, NULL);
  RETURN_ON_ERROR(hResult, NULL);

  return(pSurface);
}


// ===========================================================================
// Function:    CreateSurface
//
// Description: Creates a surface using the size parameters passed in
//
// Params:      nWidth:   The width of the surface in pixels
//              nHeight:  The height of the surface in pixels
//              dwCaps1:  First set of capabilities
//              dwCaps2:  Second set of capabilities
//              dwFlags:  Addition surface flags to use
//
// Returns:     PDDSURFACE
//
// Comments:    Returns a pointer to the newly created surface or NULL if
//              something went wrong.
// ===========================================================================
PDDSURFACE DDrawDriver::CreateSurface(UINT nWidth, UINT nHeight, DWORD dwCaps1,
                                      DWORD dwCaps2/*= 0*/, DWORD dwFlags/*= 0*/)
{
  DDSURFDESC ddSurfDesc;
  ZeroMemory(&ddSurfDesc, sizeof(DDSURFDESC));

  // Setup the description for the new surface
  ddSurfDesc.dwSize   = sizeof(DDSURFDESC);
  ddSurfDesc.dwWidth  = nWidth;
  ddSurfDesc.dwHeight = nHeight;
  ddSurfDesc.dwFlags  = DDSD_WIDTH | DDSD_HEIGHT | dwFlags;

  if (0 != dwCaps1 || 0 != dwCaps2)
  {
    ddSurfDesc.dwFlags         |= DDSD_CAPS;
    ddSurfDesc.ddsCaps.dwCaps   = dwCaps1;
    ddSurfDesc.ddsCaps.dwCaps2  = dwCaps2;
  }

  // Create the surface
  return(CreateSurface(ddSurfDesc));
}


// ===========================================================================
// Function:    CreatePalette
//
// Description: Creates a DirectDraw palette
//
// Params:      dwFlags:      Creation flags for the palette
//              lpPalEntries: The colour entries of the palette
//
// Returns:     PDDPALETTE
//
// Comments:    Returns the newly created palette if successful, and NULL if
//              something went wrong.
// ===========================================================================
PDDPALETTE DDrawDriver::CreatePalette(DWORD dwFlags,
                                      LPPALETTEENTRY lpPalEntries)
{
  // To create a surface we need a valid DirectDraw interface
  ASSERT(NULL != m_pDDraw); if (!m_pDDraw) return(NULL);

  PDDPALETTE pPalette = NULL;

  // Create the surface
  HRESULT hResult = m_pDDraw->CreatePalette(dwFlags, lpPalEntries, &pPalette,
                                            NULL);
  RETURN_ON_ERROR(hResult, NULL);
  return(pPalette);
}


// ===========================================================================
// Function:    CreateClipper
//
// Description: Creates a DirectDraw Clipper
//
// Params:      dwFlags:  Clipper creation flags
//
// Returns:     PDDCLIPPER
//
// Comments:    Returns the newly created clipper, or NULL if something went
//              wrong.
// ===========================================================================
PDDCLIPPER DDrawDriver::CreateClipper(DWORD dwFlags)
{
  // To create a surface we need a valid DirectDraw interface
  ASSERT(NULL != m_pDDraw); if (!m_pDDraw) return(NULL);

  PDDCLIPPER pClipper = NULL;

  // Create the surface
  HRESULT hResult = m_pDDraw->CreateClipper(dwFlags, &pClipper, NULL);
  RETURN_ON_ERROR(hResult, NULL);

  return(pClipper);
}


// ===========================================================================
// Function:    CreateD3DDevice
//
// Description: Creates a D3D Device
//
// Params:      guid:           GUID of the D3D device to create
//              pRenderTarget:  The rendering target the device is to use
//
// Returns:     PD3DDEVICE
//
// Comments:    Returns a pointer to the newly created D3D Device or NULL if
//              something went wrong.
// ===========================================================================
PD3DDEVICE DDrawDriver::CreateD3DDevice(GUID& guid, PDDSURFACE pRenderTarget)
{
  // To create a surface we need a valid Direct3D interface
  ASSERT(NULL != m_pD3D); if (!m_pD3D) return(NULL);

  PD3DDEVICE p3DDevice = NULL;

  // Create the surface
  HRESULT hResult = m_pD3D->CreateDevice(guid, pRenderTarget, &p3DDevice, NULL);
  RETURN_ON_ERROR(hResult, NULL);

  return(p3DDevice);
}


// ===========================================================================
// Function:    CreateLight
//
// Description: Creates a light for a Direct3D device
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns the newly created light if successful, NULL otherwise
// ===========================================================================
PLIGHT DDrawDriver::CreateLight()
{
  ASSERT(NULL != m_pD3D); if (!m_pD3D) return(NULL);
  PLIGHT pLight = NULL;
  HRESULT hReturned = m_pD3D->CreateLight(&pLight, NULL);
  RETURN_ON_ERROR(hReturned, NULL);
  return(pLight);
}


// ===========================================================================
// Function:    CreateMaterial
//
// Description: Create a material for a Direct3D device
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns the newly created material if successful, NULL
//              otherwise.
// ===========================================================================
PMATERIAL DDrawDriver::CreateMaterial()
{
  ASSERT(NULL != m_pD3D); if (!m_pD3D) return(NULL);
  PMATERIAL pMaterial = NULL;
  HRESULT hReturned = m_pD3D->CreateMaterial(&pMaterial, NULL);
  RETURN_ON_ERROR(hReturned, NULL);
  return(pMaterial);
}


// ===========================================================================
// Function:    CreateViewport
//
// Description: Create a viewport for a Direct3D device
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns the newly created viewport if successful, NULL
//              otherwise.
// ===========================================================================
PVIEWPORT DDrawDriver::CreateViewport()
{
  ASSERT(NULL != m_pD3D); if (!m_pD3D) return(NULL);
  PVIEWPORT pViewport = NULL;
  HRESULT hReturned = m_pD3D->CreateViewport(&pViewport, NULL);
  RETURN_ON_ERROR(hReturned, NULL);
  return(pViewport);
}


// ===========================================================================
// Function:    CreateVertexBuffer
//
// Description: Creates a new Direct3D vertex buffer
//
// Params:      lpVBDesc: Pointer to a vertex buffer description structure
//              dwFlags:  Creation flags of the vertex buffer
//
// Returns:     PD3DVTXBUFFER
//
// Comments:    Returns the newly created vertex buffer if successful, or NULL
//              if something went wrong.
// ===========================================================================
PD3DVTXBUFFER DDrawDriver::CreateVertexBuffer(PD3DVTXBUFFERDESC lpVBDesc,
                                              DWORD dwFlags)
{
  ASSERT(NULL != m_pD3D); if (!m_pD3D) return(NULL);
  PD3DVTXBUFFER pVertexBuffer = NULL;
  HRESULT hReturned = m_pD3D->CreateVertexBuffer(lpVBDesc, &pVertexBuffer, 
                                                 dwFlags, NULL);
  RETURN_ON_ERROR(hReturned, NULL);
  return(pVertexBuffer);
}


// ===========================================================================
// Function:    SetDisplayMode
//
// Description: Sets the display mode the DDraw driver is to use
//
// Params:      dwWidth:  Width of the display mode
//              dwHeight: Height of the display mode
//              dwBPP:    Colour depth of the display mode
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the display mode is properly set and FALSE
//              otherwise
// ===========================================================================
BOOL DDrawDriver::SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP)
{
  // Make sure we have a valid DirectDraw driver
  ASSERT(NULL != m_pDDraw); if (!m_pDDraw) return(FALSE);

  DWORD dwFlags = 0;

  // Special check for mode 320 x 200 x 8
  if ((320 == dwWidth) && (200 == dwHeight) && (8 == dwBPP))
  {
    // Make sure we use Mode 13 instead of Mode X
    dwFlags = DDSDM_STANDARDVGAMODE;
  } 

  // Set the display mode
  hReturned = m_pDDraw->SetDisplayMode(dwWidth, dwHeight, dwBPP, 0, dwFlags);
  RETURN_ON_ERROR(hReturned, FALSE);
  return(TRUE);
}


// ===========================================================================
// GLOBAL FUNCTIONS ==========================================================
// ===========================================================================


// ===========================================================================
// Function:    EnumDisplayModesCallback
//
// Description: Callback function for enumerating display modes
//
// Params:      pSurfDesc:  Surface description of the display mode
//              lpContext:  User defined data.  Will be used to pass in a
//                          pointer to a DDrawDriver object.
//
// Returns:     HRESULT
//
// Comments:    Returns DDENUMRET_OK to continue the enumeration and
//              DDENUMRET_CANCEL to abort.
// ===========================================================================
HRESULT WINAPI EnumDisplayModesCallback(PDDSURFDESC pSurfDesc, LPVOID lpContext)
{
  // Verify that we have valid user data
  ASSERT(NULL != lpContext); if (!lpContext) return(DDENUMRET_CANCEL);

  DDrawDriver* pDDrawDriver = (DDrawDriver*)lpContext;

  // Add the new display mode
  if (!pDDrawDriver->AddDisplayMode(*pSurfDesc))
  {
    // We were unable to add the new mode, something is wrong
    return(DDENUMRET_CANCEL);
  }

  return(DDENUMRET_OK);
}


// ===========================================================================
// Function:    EnumD3DDevicesCallback
//
// Description: Callback function for enumerating 3D devices
//
// Params:      lpGuid:             GUID of the enumerated device
//              szDeviceDesc:       Text description of the device
//              szDeviceName:       Text name of the device
//              lpD3DHWDeviceDesc:  Hardware description of the device
//              lpD3DHELDeviceDesc: Software description of the device
//              lpContext:          User supplied data.  Will be a pointer to
//                                  a DDrawDriver object.
//
// Returns:     HRESULT
//
// Comments:    Returns D3DENUMRET_OK to continue the enumeration and
//              D3DENUMRET_CANCEL to abort.
// ===========================================================================
HRESULT CALLBACK EnumD3DDevicesCallback(GUID FAR* lpGuid, LPSTR szDeviceDesc,
                                        LPSTR szDeviceName,
                                        LPD3DDEVICEDESC lpD3DHWDeviceDesc,
                                        LPD3DDEVICEDESC lpD3DHELDeviceDesc,
                                        LPVOID lpContext)
{
  // Verify that we have valid user data
  ASSERT(NULL != lpContext); if (!lpContext) return(DDENUMRET_CANCEL);

  DDrawDriver* pDDrawDriver = (DDrawDriver*)lpContext;
  ASSERT(NULL != lpGuid);

  if (pDDrawDriver->SearchingFor3DDevice())
  {
    if (0 == memcmp(lpGuid, pDDrawDriver->SearchGUID(), sizeof(GUID)))
    {
      Surface::SetDDrawDriver(pDDrawDriver);
      pDDrawDriver->Add3DDevice(new D3DDevice(pDDrawDriver, *lpGuid,
                                              szDeviceName, szDeviceDesc,
                                              lpD3DHWDeviceDesc,
                                              lpD3DHELDeviceDesc));
      // We are finished one way or another
      return(D3DENUMRET_CANCEL);
    }
  }
  else
  {
    Surface::SetDDrawDriver(pDDrawDriver);
    // Add the new device
    if (!pDDrawDriver->Add3DDevice(new D3DDevice(pDDrawDriver, *lpGuid,
                                                 szDeviceName, szDeviceDesc,
                                                 lpD3DHWDeviceDesc,
                                                 lpD3DHELDeviceDesc)))
    {
      // We were unable to add the device, something is wrong
      return(D3DENUMRET_CANCEL);
    }
  }
  return(D3DENUMRET_OK);
}
