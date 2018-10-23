// ===========================================================================
// Module:      DDDrvMgr.cpp
//
// Description: DirectDraw driver manager class implementation.  This class
//              handles all aspects of DirectDraw and Direct3D setup and
//              enumeration.
//
// Created:     April 14, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================

#include "PC3DHdr.h"

#include "DDDrvMgr.h"
#include "DDrawDrv.h"
#include "D3DDevice.h"

#include "DXINI.h"

extern DXINI_SelectionDialogInfo* g_pDlgInfo;


// ===========================================================================
// DDDrvMgr class ============================================================
// ===========================================================================

// Constructor ===============================================================
DDDrvMgr::DDDrvMgr(HWND hWnd /*= NULL*/)
{
  Clear();
  m_hWnd                  = hWnd;
  m_nSelectedDriverIdx    = -1;
  m_nSelected3DDeviceIdx  = -1;
  m_bWindowedMode         = FALSE;
}


// ===========================================================================
// PRIVATE: DDDrvMgr member functions ========================================
// ===========================================================================


// ===========================================================================
// Function:    Destroy
//
// Description: Destroys any resources allocated by the driver manager
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DDDrvMgr::Destroy()
{
  // Destroy the list of DirectDraw drivers/devices
  DestroyDrivers();

  Clear();
}


// ===========================================================================
// PROTECTED: DDDrvMgr member functions ======================================
// ===========================================================================


// ===========================================================================
// Function:    AddDriver
//
// Description: Adds a new DirectDraw device to the list of drivers available
//              from this driver manager.
//
// Params:      pDDrawDriver: The new DDraw driver to add
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the driver was successfully added and FALSE
//              otherwise.
// ===========================================================================
BOOL DDDrvMgr::AddDriver(DDrawDriver* pDDrawDriver)
{
  // Validate our new device
  ASSERT(NULL != pDDrawDriver); if (!pDDrawDriver) return(FALSE);

  // If we don't have enough room to add the new device, we can't do anything
  if (MAX_DDRAWDRIVERS <= m_nNumDDrawDrivers) return(FALSE);

  // Add the new device
  m_apDDDrivers[m_nNumDDrawDrivers++] = pDDrawDriver;

  return(TRUE);
}


// ===========================================================================
// PUBLIC: DDDrvMgr member functions =========================================
// ===========================================================================


// ===========================================================================
// Function:    PerformEnumeration
//
// Description: Performs the DirectDraw driver enumeration
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the enumeration was successful, and FALSE 
//              otherwise.
// ===========================================================================
BOOL DDDrvMgr::PerformEnumeration()
{
  ASSERT(NULL != m_hWnd); if (!m_hWnd) return(FALSE);

  // Enumerate all of the drivers
  HRESULT hResult = DirectDrawEnumerateEx(DDrawEnumCallbackEx, (LPVOID)this, 
                                          DDENUM_ATTACHEDSECONDARYDEVICES | 
                                          DDENUM_NONDISPLAYDEVICES);
  RETURN_ON_ERROR(hResult, FALSE);
  return(TRUE);  
}


// ===========================================================================
// Function:    GenerateDXINIFile
//
// Description: Generates the DirectX INI file from the enumerated DDrawDriver
//              info
//
// Params:      dwRequiredMemory: Tells the driver manager how much memory is
//                                required for 3D hardware support.
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the INI file is created successfully or FALSE
//              if it fails.
// ===========================================================================
BOOL DDDrvMgr::GenerateDXINIFile(DWORD dwRequiredMemory)
{
  // Do the enumeration so we can dump the info to the INI file
  if (!PerformEnumeration()) return(FALSE);

  int   nDriver       = -1;
  int   nD3DDevice    = -1;
  int   n3DDeviceIdx  =  0; 
  BOOL  bWindowed     = FALSE;

  // Do we want to show the setting dialog
  // Do we want to show the setting dialog
  if (!DXINI_CheckForChooseSettings())
  {
    // Choose some defaults
    nDriver    = 0;
    nD3DDevice = 0;
    bWindowed  = TRUE;

    // If there is more than one driver we will assume that the second one
    //  is a separate 3D accelerator and use it.  Otherwise, we will use the
    //  default.
    if (2 <= m_nNumDDrawDrivers)
      nDriver = 1;

    if (1 < m_apDDDrivers[nDriver]->GetNumD3DDevices())
    { 
      // There is more than one 3D device available.  Usually the second one is
      //  a hardware device.  Choose that if it is available.
      if (m_apDDDrivers[nDriver]->GetD3DDevice(1)->IsHardware())
      {
        // We need to make sure we aren't selecting a device that doesn't exist
        if (1 == nDriver)
        {
          if (1 < m_apDDDrivers[nDriver]->GetNumD3DDevices())
            nD3DDevice = 1 + m_apDDDrivers[0]->GetNumD3DDevices();
        }
        else
        {
          if (1 < m_apDDDrivers[nDriver]->GetNumD3DDevices())
            nD3DDevice = 1;
        }

        // We need the index of the selected 3D device
        n3DDeviceIdx = nDriver ? 
                       nD3DDevice - m_apDDDrivers[0]->GetNumD3DDevices() :
                       nD3DDevice;

        // If the device we have selected is using hardare, we need to make
        //  sure that the driver has enough memory to handle it
        if (m_apDDDrivers[nDriver]->GetD3DDevice(n3DDeviceIdx)->IsHardware())
        {
          // If we don't have enough memory for hardware, go back to software
          if (dwRequiredMemory >= m_apDDDrivers[nDriver]->GetDriverMemory())
            nD3DDevice = nDriver ? m_apDDDrivers[0]->GetNumD3DDevices() : 0;
        }
      }
    }
    bWindowed = FALSE;
  }

  // If we haven't initialized the INI file path, do so now
  if (!DXINI_PathIsValid())
    DXINI_GetFilePath();

  // If the file already exists, we want to get rid of it and start over
  DXINI_Clear();

  // The DDraw driver enumeration must have been performed before calling
  //  this function
  ASSERT(0 < m_nNumDDrawDrivers);  if (0 >= m_nNumDDrawDrivers) return(FALSE);

  // First we write the number of DirectDraw drivers available
  if (!DXINI_WriteDWORD(DXINI_DDRAWDRIVERSSECTION, DXINI_NUM_DDRAWDRIVERS,
                       (DWORD)m_nNumDDrawDrivers))
  {
    // Okay we failed to write to the INI file.  Likely we are running from
    //  a CD so we won't be able to generate the INI file.  Let's just
    //  choose the defaults that we selected up above.
    m_nSelectedDriverIdx    = nDriver;
    m_bWindowedMode         = bWindowed;

    ASSERT(-1 != n3DDeviceIdx);
    m_nSelected3DDeviceIdx  = n3DDeviceIdx; 

    return(TRUE);
  }

  TCHAR szKey[MAX_PATH];
  int n3DDeviceCount = 0;

  // Write the driver names and details
  for (int i = 0; i < m_nNumDDrawDrivers; i++)
  {
    // Driver name
    _stprintf(szKey, TEXT("%s_%02d"), DXINI_DDRAWDRIVER, i);
    if (!DXINI_WriteString(DXINI_DDRAWDRIVERSSECTION, szKey, 
                          m_apDDDrivers[i]->GetName()))
    {
      return(FALSE);
    }

    // Now write the info for the driver
    if (!m_apDDDrivers[i]->WriteINIInfo(szKey, n3DDeviceCount))
      return(FALSE);
  }

  // Release any resources we may have alloctated during enumeration
  DestroyDrivers();

  // Now write the settings section by prompting the user for their choices
  if (!DXINI_ChooseDriver(nDriver, nD3DDevice, bWindowed))
    return(FALSE);

  // Write the default resolution 640 x 480 x 16 bits per pixel
  if (!DXINI_SetSelectedResolution(640, 480, 16))
  {
    return(FALSE);
  }

  // Write whether we are going to use the resolution setting
  if (!DXINI_SetUsingResolution(FALSE))
    return(FALSE);

  return(TRUE);
}


// ===========================================================================
// Function:    DestroyDrivers
//
// Description: Releases all of the allocated resources for the enumerated
//              list of DirectDraw Drivers
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DDDrvMgr::DestroyDrivers()
{
  for (int i = 0; i < m_nNumDDrawDrivers; i++)
  { 
    DESTROYPOINTER(m_apDDDrivers[i]);
  }
  m_nNumDDrawDrivers = 0;
}

// ===========================================================================
// Function:    GetDDDriverFromINI
//
// Description: Attempts to obtain information from the DirectX INI file to
//              properly create a DDrawDriver
//
// Params:      void
//
// Returns:     DDrawDriver*
//
// Comments:    Returns a pointer to the newly create DirectDraw driver or
//              NULL if something went wrong.
// ===========================================================================
DDrawDriver* DDDrvMgr::GetDDDriverFromINI()
{
  // If a driver was already selected, we won't use the INI
  if (-1 != m_nSelectedDriverIdx)
  {
    return(new DDrawDriver(m_hWnd, 
                           m_apDDDrivers[m_nSelectedDriverIdx]->GetName(),
                           m_apDDDrivers[m_nSelectedDriverIdx]->GetDescription(),
                           m_apDDDrivers[m_nSelectedDriverIdx]->GetGUID()));
  }

  // If the INI file does not exist, we can't get information from it
  if (!DXINI_FileExists()) return(NULL);

  DWORD dwDriver = 0xFFFFFFFF;
  TCHAR szSection[MAX_PATH];
  TCHAR szDriverName[MAX_PATH];
  TCHAR szDriverDesc[MAX_PATH];

  // First get the name of the selected DirectDraw driver
  if (!DXINI_ReadDWORD(DXINI_SETTINGSSECTION, DXINI_SELECTED_DDRAWDRIVER,
                       dwDriver))
  {
    return(NULL);
  }

  _stprintf(szSection, TEXT("%s_%02d"), DXINI_DDRAWDRIVER, dwDriver);

  // Get the name and description of the driver from its section
  if (!DXINI_ReadString(szSection, DXINI_DRIVERNAME, szDriverName, MAX_PATH))
    return(NULL);
  if (!DXINI_ReadString(szSection, DXINI_DRIVERDESC, szDriverDesc, MAX_PATH))
    return(NULL);

  GUID guid;
  ZeroMemory(&guid, sizeof(GUID));

  // Now attempt to read the GUID of the selected driver
  if (DXINI_ReadGUID(szSection, DXINI_DRIVERGUIDBASE, guid))
  {
    // We were successful, so create our Driver using the retreived GUID
    return(new DDrawDriver(m_hWnd, szDriverName, szDriverDesc, &guid));
  }

  // Okay, we weren't able to read the GUID so that means that there
  // wasn't one there to read, or it has become corrupt.  We will use
  // the default primary display device in this case.
  return(new DDrawDriver(m_hWnd, szDriverName, szDriverDesc, NULL));
}


// ===========================================================================
// Function:    GetD3DDeviceFromINI
//
// Description: Attempts to obtain information from the DirectX INI file to
//              properly create a D3DDevice
//
// Params:      ddDriver: A DirectDraw driver used to create the 3D device
//
// Returns:     D3DDevice*
//
// Comments:    Returns a pointer to the newly create Direct3D device or
//              NULL if something went wrong.
// ===========================================================================
D3DDevice* DDDrvMgr::GetD3DDeviceFromINI(DDrawDriver& ddDriver)
{
  // If a 3D Device was already selected, we won't use the INI
  if (-1 != m_nSelected3DDeviceIdx)
  {
    ASSERT(-1 != m_nSelectedDriverIdx);
    return(ddDriver.EnumerateFor3DDevice(m_apDDDrivers[m_nSelectedDriverIdx]->
           GetD3DDevice(m_nSelected3DDeviceIdx)->GetGUID()));
  }

  // If the INI file does not exist, we can't get information from it
  if (!DXINI_FileExists()) return(NULL);

  DWORD dwDriver    = 0xFFFFFFFF;
  DWORD dw3DDevice  = 0xFFFFFFFF;

  // First get the number of the selected DirectDraw driver
  if (!DXINI_ReadDWORD(DXINI_SETTINGSSECTION, DXINI_SELECTED_DDRAWDRIVER,
                       dwDriver))
  {
    return(NULL);
  }

  // Format the section name
  TCHAR szSection[MAX_PATH];
  _stprintf(szSection, TEXT("%s_%02d"), DXINI_DDRAWDRIVER, dwDriver);

  // Get the number of the selected 3D device
  if (!DXINI_ReadDWORD(DXINI_SETTINGSSECTION, DXINI_SELECTED_3DDEVICE,
                       dw3DDevice))
  {
    return(NULL);
  }

  // Format the 3D device key name
  TCHAR szKey[MAX_PATH];
  _stprintf(szKey, TEXT("%s_%02d_"), DXINI_3DDEVICE, dw3DDevice);

  GUID guid;
  ZeroMemory(&guid, sizeof(GUID));

  // Now try to read the GUID for the 3d device
  if (!DXINI_ReadGUID(szSection, szKey, guid))
    return(NULL);

  // We have our GUID, create the device
  return(ddDriver.EnumerateFor3DDevice(&guid));
}


// ===========================================================================
// Function:    FindDefaultDDDriver
//
// Description: Finds the default DirectDraw driver
//
// Params:      void
//
// Returns:     int
//
// Comments:    Returns the index to the default driver
// ===========================================================================
int DDDrvMgr::FindDefaultDDDriver() const
{
  // Interate through the available drivers until we find the default one
  for (int i = 0; i < m_nNumDDrawDrivers; i++)
  {
    if (m_apDDDrivers[i] && m_apDDDrivers[i]->GUIDMatches(NULL))
      return(i);
  }

  // We were unable to find the default driver
  return(-1);
}


// ===========================================================================
// Function:    SelectDriverDialog
//
// Description: Display a driver selection dialog so the user can select which
//              DirectDraw driver and 3D Device they want to use.
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if successfull, FALSE otherwise
// ===========================================================================
BOOL DDDrvMgr::SelectDriverDialog()
{
  // Setup our dialog structure
  g_pDlgInfo = new DXINI_SelectionDialogInfo;
  ZeroMemory(g_pDlgInfo, sizeof(DXINI_SelectionDialogInfo));

  g_pDlgInfo->nSelectedDriver   = -1;
  g_pDlgInfo->nSelected3DDevice = -1;
  g_pDlgInfo->bWindowedMode     = FALSE;

  g_pDlgInfo->nNumDrivers = m_nNumDDrawDrivers;
  if (0 >= g_pDlgInfo->nNumDrivers) return(FALSE);
  
  D3DDevice* p3DDevice = NULL;

  // Get the driver list
  for (int i = 0; i < m_nNumDDrawDrivers; i++)
  {
    strcpy(g_pDlgInfo->aDrivers[i].driver.szName, m_apDDDrivers[i]->GetName());
    strcpy(g_pDlgInfo->aDrivers[i].driver.szDesc, m_apDDDrivers[i]->GetDescription());
    g_pDlgInfo->aDrivers[i].nNum3DDevices = m_apDDDrivers[i]->GetNumD3DDevices();

    // Copy all of the 3D device info
    for (int j = 0; j < m_apDDDrivers[i]->GetNumD3DDevices(); j++)
    {
      p3DDevice = m_apDDDrivers[i]->GetD3DDevice(j);
      strcpy(g_pDlgInfo->aDrivers[i].a3DDevices[j].szName, p3DDevice->GetName());
      strcpy(g_pDlgInfo->aDrivers[i].a3DDevices[j].szDesc, p3DDevice->GetDescription());
    }
  }

  // Setup the dialog
  WORD* pDlgTemplate = (PWORD)::LocalAlloc(LPTR, 1000);
  DXINI_InitDrvSelDlgTemplate(pDlgTemplate);

  // Make sure the mouse is shown
  ::ShowCursor(TRUE);

  // Show the dialog
  ::DialogBoxIndirect(::GetModuleHandle(NULL), (LPDLGTEMPLATE)pDlgTemplate, 
                      pc3D::GetHWnd(), (DLGPROC)DrvSelDlgProc);

  // Hide the cursor
  ::ShowCursor(FALSE);

  // Free up the memory we allocated for the dialog template
  ::LocalFree(LocalHandle(pDlgTemplate));

  ASSERT(0 <= g_pDlgInfo->nSelectedDriver);
  ASSERT(0 <= g_pDlgInfo->nSelected3DDevice);

  // Now set the choices
  m_nSelectedDriverIdx    = g_pDlgInfo->nSelectedDriver;
  m_nSelected3DDeviceIdx  = g_pDlgInfo->nSelected3DDevice;
  m_bWindowedMode         = g_pDlgInfo->bWindowedMode;

  return(TRUE);
}


// ===========================================================================
// GLOBAL functions ==========================================================
// ===========================================================================


// ===========================================================================
// Function:    DDrawEnumCallbackEx
//
// Description: Callback function for enumerating DirectDraw devices
//
// Params:      lpGUID:       GUID of the DDraw device.  Will be NULL for the
//                            primary device.
//              szDriverDesc: Text description of the DDraw device
//              szDriverName: Name of the DDraw device
//              lpContext:    User defined data.  Will be a pointer to a
//                            DDDrvMgr object.
//              hMonitor:     Handle to the monitor associated with the
//                            enumerated DirectDraw object. This parameter
//                            will be NULL when the enumerated DirectDraw
//                            object is for the primary device, a non-display
//                            device (such as a 3-D accelerator with no 2-D
//                            capabilities), and devices not attached to the
//                            desktop. 
//
// Returns:     BOOL
//
// Comments:    Return TRUE to continue the enumeration and FALSE to stop.
// ===========================================================================
BOOL WINAPI DDrawEnumCallbackEx(GUID FAR* lpGUID, LPSTR szDriverDesc,
                                LPSTR szDriverName, LPVOID lpContext,
                                HMONITOR hMonitor)
{
  // Verify that we have valid user data
  ASSERT(NULL != lpContext); if (!lpContext) return(FALSE);

  DDDrvMgr* pDDDrvMgr = (DDDrvMgr*)lpContext;

  // Create the new driver
  DDrawDriver* pDDDriver = new DDrawDriver(pDDDrvMgr->GetHWnd(), szDriverName,
                                           szDriverDesc, lpGUID);
  ASSERT(NULL != pDDDriver);

  // Enumerate the driver's display modes
  if (!pDDDriver->EnumerateDisplayModes())
    return(FALSE);

  // We need to make sure that the 3D devices have also been enumerated
  if (!pDDDriver->Enumerate3DDevices())
    return(FALSE);

  // Add the new device
  if (!pDDDrvMgr->AddDriver(pDDDriver))
  {
    // We were unable to add the device, something is wrong
    return(FALSE);
  }

  return(TRUE);
}
