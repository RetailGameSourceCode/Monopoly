// ===========================================================================
// Module:      D3DDevice.cpp
//
// Description: Direct3D device class implementation.  Represents a Direct3D
//              device and its capabilities.
//
// Created:     April 14, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================


#include "PC3DHdr.h"

#include "D3DDevice.h"
#include "DDrawDrv.h"
#include "DXINI.h"
#include "Surface.h"


// ===========================================================================
// Globals ===================================================================
// ===========================================================================

// Compute the size of the main data elements of a D3DDevice
const int g_kn3DDeviceSize = sizeof(DDrawDriver*)                       + 
                             sizeof(PD3DDEVICE)                         + 
                             sizeof(GUID)                               + 
                             (sizeof(char) * MAX_DDDEVICEID_STRING * 2) +
                             sizeof(Surface*)                           + 
                             (sizeof(D3DDEVICEDESC) * 2)                +
                             sizeof(BOOL);

#ifdef RECORD_3DSTATS

LONGLONG g_llNumTriangles = 0;
LONGLONG g_llStartTime    = 0;

#endif // RECORD_3DSTATS


// ===========================================================================
// D3DDevice class ===========================================================
// ===========================================================================


// Destructor ================================================================
D3DDevice::~D3DDevice()
{
#ifdef RECORD_3DSTATS
  LONGLONG llEndTime    = 0;
  LONGLONG llFrequency  = 0;
  VERIFY(::QueryPerformanceCounter((LARGE_INTEGER*)&llEndTime));
  VERIFY(::QueryPerformanceFrequency((LARGE_INTEGER*)&llFrequency));
  
  double dElapsedTime = double(llEndTime - g_llStartTime) / double(llFrequency);

  TRACE("\r\n\t\tTotal time for rendering: %9.2f seconds\r\n", dElapsedTime);
  TRACE("\t\tTotal number of triangles rendered: %I64d\r\n", g_llNumTriangles);
  TRACE("\t\tAverage number of triangles per second: %9.2f\r\n\r\n", 
        double(g_llNumTriangles) / dElapsedTime);
#endif // RECORD_3DSTATS

  Destroy();
}


// ===========================================================================
// PRIVATE: D3DDevice member functions =======================================
// ===========================================================================


// ===========================================================================
// Function:    Clear
//
// Description: Resets all of the member variables of the D3DDevice
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void D3DDevice::Clear()
{ 
  // Clear the main data set
  ZeroMemory(this, g_kn3DDeviceSize);

  DDPIXELFORMAT* pFormat = NULL;

  // Remove the Z Buffer formats
  for (int i = m_apZBufFormats.GetUpperBound(); i >= 0; i--)
  {
    pFormat = m_apZBufFormats.GetAt(i);
    DESTROYPOINTER(pFormat);
    m_apZBufFormats.RemoveAt(i);
  }

  // Remove the texture formats
  for (i = m_apTextrFormats.GetUpperBound(); i >= 0; i--)
  {
    pFormat = m_apTextrFormats.GetAt(i);
    DESTROYPOINTER(pFormat);
    m_apTextrFormats.RemoveAt(i);
  }
}


// ===========================================================================
// Function:    Destroy
//
// Description: Releases all memory allocated by a D3DDevice object and clears
//              all member variables
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void D3DDevice::Destroy()
{
  // Release the D3D device and its render target
  ReleaseD3DDevice();

  // Clear all member variables
  Clear();
}


// ===========================================================================
// PROTECTED: D3DDevice member functions =====================================
// ===========================================================================


// ===========================================================================
// Function:    CreateD3DDevice
//
// Description: Creates the Direct3D device represented by this D3DDevice
//              object
//
// Params:      guid:   The GUID of the D3D device to create
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the device was successfully create, FALSE
//              otherwise.
// ===========================================================================
BOOL D3DDevice::CreateD3DDevice(GUID& guid)
{
  BOOL bRet = FALSE;

  // Save the GUID
  CopyMemory(&m_GUID, &guid, sizeof(GUID));

  // To create the D3D device we need a rendering target first.  We can create
  //  one by using our parent, so make sure we have a valid parent.
  ASSERT(NULL != m_pParent); if (!m_pParent) return(FALSE);

  // We also need to make sure that we haven't already created a rendering
  //  target for the 3D device
  ASSERT(NULL == m_pRenderTarget); if (m_pRenderTarget) return(FALSE);

  // Create our temporary rendering target
  if (!CreateRenderTarget()) goto ABORT_CREATE;

  // Now we can create the 3D device
  m_pD3DDevice = m_pParent->CreateD3DDevice(m_GUID, 
                                            m_pRenderTarget->GetDDSurface());
  if (!m_pD3DDevice) goto ABORT_CREATE;

  bRet = TRUE;

ABORT_CREATE:
  // If we had to change the video mode to create the device, we should
  //  resort the old mode
  if (m_bModeChanged)
  {
    VERIFY(m_pParent->RestoreDisplayMode());
    VERIFY(m_pParent->SetCooperativeLevel(DDSCL_NORMAL));
  }

  // Clean up if something went wrong
  if (!bRet) DESTROYSURFACE(m_pRenderTarget);

  return(bRet);
}


// ===========================================================================
// Function:    CreateRenderTarget
//
// Description: Creates a rendering surface compatible with the current D3D
//              device.
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the surface was successfully create and FALSE
//              otherwise
// ===========================================================================
BOOL D3DDevice::CreateRenderTarget()
{
  ASSERT(NULL != m_pParent); if (!m_pParent) return(FALSE);

  DDSURFDESC ddDisplayMode;
  m_bModeChanged = FALSE;

  // First thing to do is to check if our current display mode is compatible
  //  with the 3D device
  m_pParent->GetDisplayMode(ddDisplayMode);

  // We need to make sure that we have set the cooperative level before we
  //  can create a surface
  if (!m_pParent->SetCooperativeLevel(DDSCL_NORMAL))
    return(FALSE);

  // If the current colour depth is not supported by the rendering device
  //  we need to change to one that is before we can create a rendering target
  if (!ColourDepthSupported(ddDisplayMode.ddpfPixelFormat.dwRGBBitCount))
  {
    for (DWORD dwBitDepth = 16; dwBitDepth <= 32; dwBitDepth += 8)
    {
      if (ColourDepthSupported(dwBitDepth))
      {
        // Set the cooperative mode
        if (!m_pParent->SetCooperativeLevel(DDSCL_EXCLUSIVE |
                                            DDSCL_FULLSCREEN))
        {
          return(FALSE);
        }

        // Set the display mode
        if (!m_pParent->SetDisplayMode(640, 480, dwBitDepth))
          return(FALSE);

        // Okay we just change the display mode so remember that so we can
        //  change it back after we have created the device
        m_bModeChanged = TRUE;

        break;
      }
    }
  }

  // By the time we reach here we are ensured that a supported display mode
  //  has been set
  DDSURFDESC ddsd;
  ZeroMemory(&ddsd, sizeof(DDSURFDESC));
  ddsd.dwSize         = sizeof(DDSURFDESC);
  ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
  ddsd.ddsCaps.dwCaps = DDSCAPS_3DDEVICE;
  ddsd.dwWidth = ddsd.dwHeight = 50;

  // If we are creating a render target for a hardware device, the surface
  //  better be created in video memory
  if (IsHardware())
    ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;

  // Create our rendering surface
  m_pRenderTarget = new Surface();
  ASSERT(NULL != m_pRenderTarget);

  return(m_pRenderTarget->Create(ddsd));
}


// ===========================================================================
// Function:    AddZBufFormat
//
// Description: Adds a new Z buffer format to the list of formats supported by
//              this 3D Device.
//
// Params:      pZBufFormat:  Pointer to a DDPIXELFORMAT structure describing
//                            the Z Buffer format
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the Z Buffer format was successfully added,
//              FALSE otherwise.
// ===========================================================================
BOOL D3DDevice::AddZBufFormat(DDPIXELFORMAT* pZBufFormat)
{
  // Validate our new format
  ASSERT(NULL != pZBufFormat); if (!pZBufFormat) return(FALSE);

  // Add the new format
  DDPIXELFORMAT* pNewFormat = new DDPIXELFORMAT;
  ASSERT(NULL != pNewFormat);
  CopyMemory(pNewFormat, pZBufFormat, sizeof(DDPIXELFORMAT));
  m_apZBufFormats.Add(pNewFormat);

  return(TRUE);
}


// ===========================================================================
// Function:    AddTextrFormat
//
// Description: Adds a new texture format to the list of formats supported by
//              this 3D Device.
//
// Params:      pTextrFormat: Pointer to a DDPIXELFORMAT structure describing
//                            the texture format
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the texture format was successfully added,
//              FALSE otherwise.
// ===========================================================================
BOOL D3DDevice::AddTextrFormat(DDPIXELFORMAT* pTextrFormat)
{
  // Validate our new format
  ASSERT(NULL != pTextrFormat); if (!pTextrFormat) return(FALSE);

  // Add the new format
  DDPIXELFORMAT* pNewFormat = new DDPIXELFORMAT;
  ASSERT(NULL != pNewFormat);
  CopyMemory(pNewFormat, pTextrFormat, sizeof(DDPIXELFORMAT));
  m_apTextrFormats.Add(pNewFormat);

  return(TRUE);
}


// ===========================================================================
// PUBLIC: D3DDevice member functions ========================================
// ===========================================================================


// ===========================================================================
// Function:    Initialize
//
// Description: Initializes a Direct3D device
//
// Params:      pParent:        The parent DDraw driver of the device
//              guid:           GUID for the device
//              szDeviceName:   Name of the device
//              szDeviceDesc:   Text description of the device
//              pHardwareDesc:  Hardware description of the device
//              pSoftwareDesc:  Software description of the device
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if everything was successfully initialized,
//              FALSE otherwise.
// ===========================================================================
BOOL D3DDevice::Initialize(DDrawDriver* pParent,  GUID& guid,
                           LPSTR szDeviceName, LPSTR szDeviceDesc,
                           D3DDEVICEDESC* pHardwareDesc, 
                           D3DDEVICEDESC* pSoftwareDesc)
{
  HRESULT hResult = NULL;

  // We require a valid parent
  ASSERT(NULL != pParent); if (!pParent) return(FALSE);
  m_pParent = pParent;

  // We require the hardware and software description structures
  if (pHardwareDesc)
    CopyMemory(&m_HardwareDesc, pHardwareDesc, sizeof(D3DDEVICEDESC));
  else
    return(FALSE);

  if (pSoftwareDesc)
    CopyMemory(&m_SoftwareDesc, pSoftwareDesc, sizeof(D3DDEVICEDESC));
  else
    return(FALSE);

  // Create our D3D device
  if (!CreateD3DDevice(guid)) return(FALSE);

  // Save the info
  if (szDeviceName) strcpy(m_szName, szDeviceName);
  if (szDeviceDesc) strcpy(m_szDescription, szDeviceDesc);
  
  // Enumerate the Z Buffer formats
  hResult = m_pParent->GetD3D()->EnumZBufferFormats(m_GUID,
                                                    EnumZBufferFormatsCallback,
                                                    (LPVOID)this);
  GOTO_ON_ERROR(hResult, INIT_FAILURE);

  // Enumerate the texture formats
  hResult = m_pD3DDevice->EnumTextureFormats(EnumTextureFormatsCallback,
                                             (LPVOID)this);
  GOTO_ON_ERROR(hResult, INIT_FAILURE);

  // Sort the texture formats
  ::qsort((LPVOID)m_apTextrFormats.GetData(), m_apTextrFormats.GetSize(),
          sizeof(DDPIXELFORMAT*), CompareTextureFormats);

  // Make sure we destroy our temporary render target
  DESTROYSURFACE(m_pRenderTarget);

  // Everything was successful
  return(TRUE);

INIT_FAILURE:

  // Something went wrong
  Destroy();
  return(FALSE);
}


// ===========================================================================
// Function:    InitD3D
//
// Description: Initializes the Direct3D components.  It is assumed that the
//              GUID, parent DirectDraw driver, device descriptions and 
//              enumerated lists have already been initialized previously.
//
// Params:      renderTarget:  The render target to use
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the D3D device was successfully initialized,
//              and FALSE if something went wrong.
// ===========================================================================
BOOL D3DDevice::InitD3D(Surface* pRenderTarget)
{
  // Verify our requirements
  ASSERT(NULL != m_pParent); if (!m_pParent) return(FALSE);
  ASSERT(NULL != pRenderTarget); if (!pRenderTarget) return(FALSE);

  ASSERT(0 < m_apZBufFormats.GetSize());
  if (0 >= m_apZBufFormats.GetSize()) return(FALSE);
  
  ASSERT(0 < m_apTextrFormats.GetSize()); 
  if (0 >= m_apTextrFormats.GetSize()) return(FALSE);

  // We also need to make sure that we haven't already created a rendering
  //  target for the 3D device
  ASSERT(NULL == m_pRenderTarget); if (m_pRenderTarget) return(FALSE);
  m_pRenderTarget = pRenderTarget;

  m_pD3DDevice = m_pParent->CreateD3DDevice(m_GUID, 
                                            m_pRenderTarget->GetDDSurface());
  if (!m_pD3DDevice)
  {
    // Something went wrong, release our resources
    m_pRenderTarget = NULL;
    return(FALSE);
  }

  return(TRUE);
}


// ===========================================================================
// Function:    SetInitialRenderingStates
//
// Description: Setup the initial rendering states of the D3D device.
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void D3DDevice::SetInitialRenderingStates()
{
  // Make sure our device is valid
  ASSERT(NULL != m_pD3DDevice); if (!m_pD3DDevice) return;

  SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
  SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
  SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE);

  // We only want to enable ZBuffering if we have created a ZBuffer
  ASSERT(NULL != pc3D::m_pMainWnd);
  Surface* pZBuffer = pc3D::m_pMainWnd->GetZBuffer();
  if (pZBuffer->IsInitialized())
  {
    SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
    SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
  }
}


// ===========================================================================
// Function:    ColourDepthSupported
//
// Description: Determines if the given colour depth is supported by the D3D
//              device.
//
// Params:      dwColourDepth:    The colour depth we want to check
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the colour depth is supported and FALSE if it
//              is not.
// ===========================================================================
BOOL D3DDevice::ColourDepthSupported(DWORD dwColourDepth)
{
  D3DDEVICEDESC* pDeviceDesc = NULL;

  if (IsHardware()) pDeviceDesc = &m_HardwareDesc;
  else              pDeviceDesc = &m_SoftwareDesc;

  switch (dwColourDepth)
  {
    case 8:   return((BOOL)pDeviceDesc->dwDeviceRenderBitDepth & DDBD_8);
    case 16:  return((BOOL)pDeviceDesc->dwDeviceRenderBitDepth & DDBD_16);
    case 24:  return((BOOL)pDeviceDesc->dwDeviceRenderBitDepth & DDBD_24);
    case 32:  return((BOOL)pDeviceDesc->dwDeviceRenderBitDepth & DDBD_32);
    default:  return(FALSE);
  }
}


// ===========================================================================
// Function:    TextureFormatSupported
//
// Description: Searches through the list of enumerated textures and
//              determines if the given texture format is supported.
//
// Params:      ddpf: Pixel format to look for
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the format is supported, FALSE if it is not.
// ===========================================================================
BOOL D3DDevice::TextureFormatSupported(const DDPIXELFORMAT& ddpf)
{
  for (int i = 0; i < m_apTextrFormats.GetSize(); i++)
  {
    if (0 == memcmp(&ddpf, m_apTextrFormats.GetAt(i), sizeof(DDPIXELFORMAT)))
      return(TRUE);
  }

  return(FALSE);
}


// ===========================================================================
// Function:    WriteDeviceCapsToINI
//
// Description: Writes all of the device's capabilities to the DirectX INI
//              file
//
// Params:      szSection:  The section where we want the capabilites written
//                          to
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if everything was successfully written and FALSE
//              otherwise
// ===========================================================================
BOOL D3DDevice::WriteDeviceCapsToINI(LPCTSTR szSection)
{
  if (IsHardware())
  {
    return(DXINI_WriteD3DDeviceInfo(m_HardwareDesc, m_szName, m_szDescription,
                                    szSection));
  }
  else
  {
    return(DXINI_WriteD3DDeviceInfo(m_SoftwareDesc, m_szName, m_szDescription,
                                    szSection));
  }
}


// ===========================================================================
// Global Functions ==========================================================
// ===========================================================================


// ===========================================================================
// Function:    EnumZBufferFormatsCallback
//
// Description: Callback function for enumerating Z Buffer formats
//
// Params:      lpDDPixFmt:   Pixel format describing the Z Buffer format
//              lpContext:    User defined data.  Will be a pointer to a
//                            D3DDevice object.
//
// Returns:     HRESULT
//
// Comments:    Returns D3DENUMRET_OK to continue the enumeration and
//              D3DENUMRET_CANCEL to abort.
// ===========================================================================
HRESULT CALLBACK EnumZBufferFormatsCallback(LPDDPIXELFORMAT lpDDPixFmt,  
                                            LPVOID lpContext)
{
  // Verify that we have valid user data
  ASSERT(NULL != lpContext); if (!lpContext) return(DDENUMRET_CANCEL);

  D3DDevice* pD3DDevice = (D3DDevice*)lpContext;

  // Add the new Z Buffer format
  if (!pD3DDevice->AddZBufFormat(lpDDPixFmt))
  {
    // We were unable to add the Z Buffer format, something is wrong
    return(D3DENUMRET_CANCEL);
  }

  return(D3DENUMRET_OK);
}


// ===========================================================================
// Function:    EnumTextureFormatsCallback
//
// Description: Callback function for enumerating texture formats
//
// Params:      lpDDPixFmt:   Pixel format describing the texture format
//              lpContext:    User defined data.  Will be a pointer to a
//                            D3DDevice object.
//
// Returns:     HRESULT
//
// Comments:    Returns D3DENUMRET_OK to continue the enumeration and
//              D3DENUMRET_CANCEL to abort.
// ===========================================================================
HRESULT CALLBACK EnumTextureFormatsCallback(LPDDPIXELFORMAT lpDDPixFmt,  
                                            LPVOID lpContext)
{
  // Verify that we have valid user data
  ASSERT(NULL != lpContext); if (!lpContext) return(DDENUMRET_CANCEL);

  D3DDevice* pD3DDevice = (D3DDevice*)lpContext;

  // Add the new texture format
  if (!pD3DDevice->AddTextrFormat(lpDDPixFmt))
  {
    // We were unable to add the texture format, something is wrong
    return(D3DENUMRET_CANCEL);
  }

  return(D3DENUMRET_OK);
}


// ===========================================================================
// Function:    CompareTextureFormats
//
// Description: Compares two texture formats.  Used for sorting.
//
// Params:      pArg1:  Pointer to the first texture format
//              pArg2:  Pointer to the second texture format
//
// Returns:     int
//
// Comments:    Returns -1 if pArg1 < pArg2, 1 if pArg1 > pArg2 or 0 if they
//              are equal.
// ===========================================================================
int CompareTextureFormats(const void* pArg1, const void* pArg2)
{
  DDPIXELFORMAT** pFmt1 = (DDPIXELFORMAT**)pArg1;
  DDPIXELFORMAT** pFmt2 = (DDPIXELFORMAT**)pArg2;

  // Check the bit count
  if ((*pFmt1)->dwRGBBitCount < (*pFmt2)->dwRGBBitCount) return(-1);
  if ((*pFmt1)->dwRGBBitCount > (*pFmt2)->dwRGBBitCount) return( 1);

  // Check the alpha mask
  if ((*pFmt1)->dwRGBAlphaBitMask < (*pFmt2)->dwRGBAlphaBitMask) return(-1);
  if ((*pFmt1)->dwRGBAlphaBitMask > (*pFmt2)->dwRGBAlphaBitMask) return( 1);

  // Check the red mask
  if ((*pFmt1)->dwRBitMask < (*pFmt2)->dwRBitMask) return(-1);
  if ((*pFmt1)->dwRBitMask > (*pFmt2)->dwRBitMask) return( 1);

  // The formats are equal
  return(0);
}
