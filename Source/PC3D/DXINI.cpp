// ===========================================================================
// Module:      DXINI.cpp
//
// Description: Contains functions for the DirectX INI file handling stuff
//
// Created:     April 26, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#include "PC3DHdr.h"
#include "DXINI.h"
#include "PC3D.h"

// ===========================================================================
// Globals ===================================================================
// ===========================================================================

char g_szDXINIPath[MAX_PATH];

DXINI_SelectionDialogInfo* g_pDlgInfo = NULL;


#define GUID_DATA1              TEXT("GUID_DATA1")
#define GUID_DATA2              TEXT("GUID_DATA2")
#define GUID_DATA3              TEXT("GUID_DATA3")

static TCHAR* g_aszGUIDDATA4[] =
{
  TEXT("GUID_DATA4_01"),
  TEXT("GUID_DATA4_02"),
  TEXT("GUID_DATA4_03"),
  TEXT("GUID_DATA4_04"),
  TEXT("GUID_DATA4_05"),
  TEXT("GUID_DATA4_06"),
  TEXT("GUID_DATA4_07"),
  TEXT("GUID_DATA4_08")
};

// Driver selection dialog definitions =======================================

static const WORD DLG_DRVSELECTION[] =
{
  LOWORD(DWORD(DS_MODALFRAME | DS_CENTER | WS_POPUPWINDOW | WS_CAPTION)),
  HIWORD(DWORD(DS_MODALFRAME | DS_CENTER | WS_POPUPWINDOW | WS_CAPTION)),
  0, 0,             // Extended style
#ifdef _DEBUG
  4,                // NumberOfItems
#else
  3,                // NumberOfItems
#endif //_DEBUG
  10, 10, 120, 68,  // x, y, cx, cy
  0, 0,             // Menu, Class
  'C', 'h', 'o', 'o', 's', 'e', ' ', 'D', 'r', 'i', 'v', 'e', 'r', 0, // Caption
  0                 // DWORD alignment
};

static const WORD OKBUTTON[] =
{
  LOWORD(DWORD(BS_DEFPUSHBUTTON | WS_VISIBLE | WS_CHILD)),  // Style
  HIWORD(DWORD(BS_DEFPUSHBUTTON | WS_VISIBLE | WS_CHILD)),  // Style
  0, 0,             // Extended Style
  40, 52, 40, 12,   // x, y, cx, cy
  IDOK,             // ID
  0xFFFF, 0x0080,   // Class ID (Button)
  'O', 'K', 0,      // Button text
  0, 0              // DWORD alignment
};

static const WORD DRIVERCOMBOBOX[] =
{
  LOWORD(DWORD(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE | WS_CHILD)),
  HIWORD(DWORD(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE | WS_CHILD)),
  0, 0,             // Extended Style
  5, 5, 110, 35,    // x, y, cx, cy
  IDC_DRIVERLIST,   // ID
  0xFFFF, 0x0085,   // Class ID
  0, 0, 0           // DWORD alignment
};

static const WORD DEVICECOMBOBOX[] =
{
  LOWORD(DWORD(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE | WS_CHILD)),
  HIWORD(DWORD(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE | WS_CHILD)),
  0, 0,             // Extended Style
  5, 23, 110, 35,   // x, y, cx, cy
  IDC_3DDEVICELIST, // ID
  0xFFFF, 0x0085,   // Class ID
  0, 0, 0           // DWORD alignment
};

static const WORD WINDOWMODECHECKBOX[] =
{
  LOWORD(DWORD(BS_AUTOCHECKBOX | WS_VISIBLE | WS_CHILD)), // Style
  HIWORD(DWORD(BS_AUTOCHECKBOX | WS_VISIBLE | WS_CHILD)), // Style
  0, 0,             // Extended Style
  25, 38, 70, 12,   // x, y, cx, cy
  IDC_WINDOWEDMODE, // ID
  0xFFFF, 0x0080,   // Class ID
  'W', 'i', 'n', 'd', 'o', 'w', 'e', 'd', ' ', 'M', 'o', 'd', 'e', 0, // Text
  0,                // DWORD alignment
};


// ===========================================================================
// Functions =================================================================
// ===========================================================================


// ===========================================================================
// Function:    DXINI_GetFilePath
//
// Description: Gets the full path to the DirectX INI file
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DXINI_GetFilePath()
{
  // Get the full path name of the EXE file
  GetModuleFileName(NULL, g_szDXINIPath, MAX_PATH);

  // Now strip off the file name from the path
  char* szTmp = strrchr(g_szDXINIPath, '\\');
  szTmp[1] = '\0';

  // Stick on the INI file name
  strcat(g_szDXINIPath, DXINIFILE);
}


// ===========================================================================
// Function:    DXINI_FileExists
//
// Description: Determines if the DirectX INI file exits or not
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the file exists and FALSE if it does not
// ===========================================================================
BOOL DXINI_FileExists()
{
  // Make sure we have a valid path
  if (!DXINI_PathIsValid())
    DXINI_GetFilePath();

  // Try opening the file
  HANDLE hFile = CreateFile(g_szDXINIPath, 0, FILE_SHARE_READ, NULL,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  // Check to see if we obtained a valid handle
  if (hFile)
  {
    // We found it
    CloseHandle(hFile);
    return(TRUE);
  }

  // The file does not exist
  return(FALSE);
}


// ===========================================================================
// Function:    DXINI_ReadGUID
//
// Description: Reads a GUID from the DirectX INI file
//
// Params:      szSection:  The section from which the GUID will be read
//              szBaseKey:  The base name of the key that is used to identify
//                          the GUID
//              guid:       Value where the read GUID will be put
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the GUID is successfully read from the INI
//              file and FALSE if not.
// ===========================================================================
BOOL DXINI_ReadGUID(LPCTSTR szSection, LPCTSTR szBaseKey, GUID& guid)
{
  TCHAR szKey[MAX_PATH];
  DWORD dwValue = 0;

  // Read the first segement of the GUID
  lstrcpy(szKey, szBaseKey); _tcscat(szKey, GUID_DATA1);
  if (!DXINI_ReadDWORD(szSection, szKey, dwValue)) return(FALSE);
  guid.Data1 = dwValue;

  // Second segment...
  strcpy(szKey, szBaseKey); _tcscat(szKey, GUID_DATA2);
  if (!DXINI_ReadDWORD(szSection, szKey, dwValue)) return(FALSE);
  guid.Data2 = (WORD)dwValue;

  // Third segment...
  lstrcpy(szKey, szBaseKey); _tcscat(szKey, GUID_DATA3);
  if (!DXINI_ReadDWORD(szSection, szKey, dwValue)) return(FALSE);
  guid.Data3 = (WORD)dwValue;

  // Fourth segment
  for (int i = 0; i < 8; i++)
  {
    lstrcpy(szKey, szBaseKey); _tcscat(szKey, g_aszGUIDDATA4[i]);
    if (!DXINI_ReadDWORD(szSection, szKey, dwValue)) return(FALSE);
    guid.Data4[i] = (BYTE)dwValue;
  }

  GUID guidTmp;
  ZeroMemory(&guidTmp, sizeof(GUID));

  // Make sure we don't have an empty GUID
  return(0 != memcmp(&guidTmp, &guid, sizeof(GUID)));
}


// ===========================================================================
// Function:    DXINI_WriteGUID
//
// Description: Writes a GUID to the DirectX INI file
//
// Params:      szSection:  The section where we want to write the GUID
//              szBaseKey:  The base name of the key that will be used to
//                          identify the GUID
//              guid:       The GUID to write
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the GUID is successfully written to the INI
//              file and FALSE if not.
// ===========================================================================
BOOL DXINI_WriteGUID(LPCTSTR szSection, LPCTSTR szBaseKey, const GUID& guid)
{
  TCHAR szKey[MAX_PATH];

  // Write the first segement of the GUID
  lstrcpy(szKey, szBaseKey); _tcscat(szKey, GUID_DATA1);
  if (!DXINI_WriteDWORD(szSection, szKey, guid.Data1))
    return(FALSE);
  
  // Second segment...
  lstrcpy(szKey, szBaseKey); _tcscat(szKey, GUID_DATA2);
  if (!DXINI_WriteDWORD(szSection, szKey, (DWORD)(guid.Data2)))
    return(FALSE);

  // Third segment...
  lstrcpy(szKey, szBaseKey); _tcscat(szKey, GUID_DATA3);
  if (!DXINI_WriteDWORD(szSection, szKey, (DWORD)(guid.Data3)))
    return(FALSE);

  // Fourth segment
  for (int i = 0; i < 8; i++)
  {
    lstrcpy(szKey, szBaseKey); _tcscat(szKey, g_aszGUIDDATA4[i]);
    if (!DXINI_WriteDWORD(szSection, szKey, (DWORD)(guid.Data4[i])))
      return(FALSE);
  }
  
  return(TRUE);
}


// ===========================================================================
// Function:    DXINI_ReadRes
//
// Description: Reads a resolution value from the DirectX ini file
//
// Params:      szSection:  (IN)  The section from which to read
//              szKey:      (IN)  The key to read
//              dwWidth:    (OUT) The width that was read
//              dwHeight:   (OUT) The height that was read                              
//              dwBitDepth: (OUT) The bit depth that was read
//
// Returns:     BOOL
//
// Comments:    Returns TRUE is the resolution was successfully read or FALSE
//              if it was not.
// ===========================================================================
BOOL DXINI_ReadRes(LPCTSTR szSection, LPCTSTR szKey, DWORD& dwWidth,
                   DWORD& dwHeight, DWORD& dwBitDepth)
{
  TCHAR szValue[MAX_PATH];
  TCHAR* szToken = NULL;

  // Read the data from the INI file
  if (!DXINI_ReadString(szSection, szKey, szValue, MAX_PATH)) return(FALSE);

  // Resolution will be in this format: 'width'X'height'X'bitdepth'

  // Width
  if (!(szToken = _tcstok(szValue, TEXT("X")))) return(FALSE);
  dwWidth = _tcstoul(szToken, NULL, 10);
  // Height
  if (!(szToken = _tcstok(NULL, TEXT("X")))) return(FALSE);
  dwHeight = _tcstoul(szToken, NULL, 10);
  // Bit depth
  if (!(szToken = _tcstok(NULL, TEXT("X")))) return(FALSE);
  dwBitDepth = _tcstoul(szToken, NULL, 10);

  return(TRUE);
}


// ===========================================================================
// Function:    DXINI_WriteRes
//
// Description: Writes a resolution value to the DirectX ini file
//
// Params:      szSection:  The section to write to
//              szKey:      The key to write
//              dwWidth:    The resolution's width
//              dwHeight:   The resolution's height
//              dwBitDepth: The resolution's bit depth
//
// Returns:     BOOL
//
// Comments:    Returns TRUE is the resolution was successfully written or 
//              FALSE if it was not.
// ===========================================================================
BOOL DXINI_WriteRes(LPCTSTR szSection, LPCTSTR szKey, DWORD dwWidth,
                    DWORD dwHeight, DWORD dwBitDepth)
{
  TCHAR szValue[MAX_PATH];
  _stprintf(szValue, "%dX%dX%d", dwWidth, dwHeight, dwBitDepth);
  return(DXINI_WriteString(szSection, szKey, szValue));
}


// ===========================================================================
// Function:    DXINI_GetDriverList
//
// Description: Returns a list of the available drivers from the INI file.
//
// Params:      aDrivers:   An array of DXINI_DriverInfo structures that will
//                          be filled with the driver information
//              nSize:      The maximum number of drivers that can be inserted
//                          into the DriverInfo array
//
// Returns:     int
//
// Comments:    Returns the number of drivers that were added to the lists if
//              everything was successful.  Returns -1 if there was an error.
// ===========================================================================
int DXINI_GetDriverList(DXINI_DriverInfo* aDrivers, int nMaxDrivers)
{
  // Make sure we have enough room in the arrays for the information
  ASSERT(0 < nMaxDrivers); if (0 >= nMaxDrivers) return(-1);

  // Check to make sure the INI file exists
  if (!DXINI_FileExists()) return(-1);

  DWORD dwValue = 0xFFFFFFFF;

  // Get the number of available DirectDraw drivers
  if (!DXINI_ReadDWORD(DXINI_DDRAWDRIVERSSECTION, DXINI_NUM_DDRAWDRIVERS, 
                       dwValue))
  {
    return(-1);
  }

  int   nNum3DDevices = 0, n3DDeviceCount = 0;
  int   nNumDrivers   = min(nMaxDrivers, int(dwValue));
  TCHAR szSection[MAX_PATH];
  TCHAR szKey[MAX_PATH];

  // Now get the names and descriptions of the devices
  for (int i = 0; i < nNumDrivers; i++)
  {
    // Format the section name
    _stprintf(szSection, TEXT("%s_%02d"), DXINI_DDRAWDRIVER, i);

    // Read the driver name
    if (!DXINI_ReadString(szSection, DXINI_DRIVERNAME, aDrivers[i].driver.szName, 
                          MAX_DDDEVICEID_STRING))
    {
      break;
    }

    // Read the Driver Description
    if (!DXINI_ReadString(szSection, DXINI_DRIVERDESC, aDrivers[i].driver.szDesc, 
                          MAX_DDDEVICEID_STRING))
    {
      break;
    }

    // Get the number of 3D devices for this driver
    if (!DXINI_ReadDWORD(szSection, DXINI_NUM_3DDEVICES, dwValue))
      break;

    nNum3DDevices = min(MAX_3DDEVICES, (int)dwValue);

    // Read in the 3D device information
    for (aDrivers[i].nNum3DDevices = 0; 
         aDrivers[i].nNum3DDevices < nNum3DDevices; 
         aDrivers[i].nNum3DDevices++)
    {
      // Format the key name
      _stprintf(szKey, "%s_%02d_Name", DXINI_3DDEVICE, n3DDeviceCount);

      // Read the 3D device name
      if (!DXINI_ReadString(szSection, szKey,
                            aDrivers[i].a3DDevices[aDrivers[i].nNum3DDevices].szName,
                            MAX_DDDEVICEID_STRING))
      {
        break;
      }

      // Read the 3D device description
      sprintf(szKey, "%s_%02d_Description", DXINI_3DDEVICE, n3DDeviceCount);
      if (!DXINI_ReadString(szSection, szKey, 
                            aDrivers[i].a3DDevices[aDrivers[i].nNum3DDevices].szDesc, 
                            MAX_DDDEVICEID_STRING))
      {
        break;
      }

      // Increase the device count
      n3DDeviceCount++;
    }
  }

  return(i);
}


// ===========================================================================
// Function:    DXINI_InitDrvSelDlgTemplate
//
// Description: Initializes the given pointer to be a dialog template.
//
// Params:      pTemplate:  Pointer to a WORD buffer that will be filled with
//                          data describing the dialog template for the driver
//                          selection dialog
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if successful, FALSE otherwise.
// ===========================================================================
BOOL DXINI_InitDrvSelDlgTemplate(WORD* pTemplate)
{
  WORD* p = pTemplate;
  ASSERT(NULL != p); if (!p) return(FALSE);

  // Initialize the dialog
  CopyMemory(p, DLG_DRVSELECTION, sizeof(DLG_DRVSELECTION));
  p += sizeof(DLG_DRVSELECTION) / 2;

  // Add the Ok button
  CopyMemory(p, OKBUTTON, sizeof(OKBUTTON));
  p += sizeof(OKBUTTON) / 2;

  // Add combo box for driver selection
  CopyMemory(p, DRIVERCOMBOBOX, sizeof(DRIVERCOMBOBOX));
  p += sizeof(DRIVERCOMBOBOX) / 2;

  // Add combo box for 3D device selection
  CopyMemory(p, DEVICECOMBOBOX, sizeof(DEVICECOMBOBOX));
  p += sizeof(DEVICECOMBOBOX) / 2;

#ifdef _DEBUG
  // Add check box for the windowed mode
  CopyMemory(p, WINDOWMODECHECKBOX, sizeof(WINDOWMODECHECKBOX));
  p += sizeof(WINDOWMODECHECKBOX) / 2;
#endif // _DEBUG
  
  return(TRUE);
}


// ===========================================================================
// Function:    DXINI_InitDrvSelDlg
//
// Description: Initializes the contents of the driver selection dialog
//
// Params:      hDlg: Handle to the dialog
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the initialization was successful, or FALSE if
//              something went wrong.
// ===========================================================================
BOOL DXINI_InitDrvSelDlg(HWND hDlg)
{
  ASSERT(NULL != hDlg); if (!hDlg) return(FALSE);

  // Get the driver list combo box
  HWND hList = GetDlgItem(hDlg, IDC_DRIVERLIST); 
  
  for (int i = 0; i < g_pDlgInfo->nNumDrivers; i++)
  {
    SendMessage(hList, CB_ADDSTRING, 0, 
                (LPARAM)(LPCTSTR)(g_pDlgInfo->aDrivers[i].driver.szDesc));
  }

  // Select the first item in the list
  SendMessage(hList, CB_SETCURSEL, 0, 0);

  // We now need to fill the 3D device list.  Calling the driver changed
  // handling function will do this for us.
  return(DXINI_DrvChangedDrvSelDlg(hDlg));
}


// ===========================================================================
// Function:    DXINI_CloseDrvSelDlg
//
// Description: Closes the driver selection dialog
//
// Params:      hDlg: Handle to the driver selection dialog
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if everthing was successful, or FALSE if not.
// ===========================================================================
BOOL DXINI_CloseDrvSelDlg(HWND hDlg)
{
  ASSERT(NULL != g_pDlgInfo); if (!g_pDlgInfo) return(FALSE);
  ASSERT(NULL != hDlg);       if (!hDlg)       return(FALSE);

  // Get the selected driver
  g_pDlgInfo->nSelectedDriver = SendMessage(GetDlgItem(hDlg, IDC_DRIVERLIST),
                                            CB_GETCURSEL, 0, 0);

  // Get the selected 3D device
  int n3DIdx = SendMessage(GetDlgItem(hDlg, IDC_3DDEVICELIST),
                           CB_GETCURSEL, 0, 0);

  // Adjust the numbering of the 3D device
  g_pDlgInfo->nSelected3DDevice = -1;

  for (int i = 0; i < g_pDlgInfo->nSelectedDriver; i++)
    g_pDlgInfo->nSelected3DDevice += g_pDlgInfo->aDrivers[i].nNum3DDevices;

  g_pDlgInfo->nSelected3DDevice += n3DIdx + 1;

  // Get the windowed mode status
  g_pDlgInfo->bWindowedMode = SendMessage(GetDlgItem(hDlg, IDC_WINDOWEDMODE),
                                          BM_GETSTATE, 0, 0);

  return(EndDialog(hDlg, TRUE));
}


// ===========================================================================
// Function:    DXINI_DrvChangedDrvSelDlg
//
// Description: Called when the selected driver is changed in the driver
//              selection dialog
//
// Params:      hDlg:   Handle to the dialog
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if successful, FALSE otherwise.
// ===========================================================================
BOOL DXINI_DrvChangedDrvSelDlg(HWND hDlg)
{
  ASSERT(NULL != g_pDlgInfo); if (!g_pDlgInfo) return(FALSE);
  ASSERT(NULL != hDlg);       if (!hDlg)       return(FALSE);

  // Select the first item in the list
  int nIdx = SendMessage(GetDlgItem(hDlg, IDC_DRIVERLIST), CB_GETCURSEL, 0, 0);

  // We now need to fill the 3D device list
  HWND hList = GetDlgItem(hDlg, IDC_3DDEVICELIST);

  // We need to make sure that we empty the current list
  SendMessage(hList, CB_RESETCONTENT, 0, 0);

  // Now add the new 3D devices for the selected driver
  for (int i = 0; i < g_pDlgInfo->aDrivers[nIdx].nNum3DDevices; i++)
  {
    SendMessage(hList, CB_ADDSTRING, 0, 
                (LPARAM)(LPCTSTR)(g_pDlgInfo->aDrivers[nIdx].a3DDevices[i].szName));
  }

  // Select the first item in the list
  SendMessage(hList, CB_SETCURSEL, 0, 0);

  return(TRUE);
}


// ===========================================================================
// Function:    DrvSelDlgProc
//
// Description: Dialog procedure for handling the messages sent to the driver
//              selection dialog.
//
// Params:      hWnd:   The window that is receiving the message
//              uiMsg:  ID of the message
//              wParam: 32 bit unsigned message parameter
//              lParam: 32 bit signed message parameter
//
// Returns:     LRESULT
//
// Comments:    
// ===========================================================================
LRESULT APIENTRY DrvSelDlgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, 
                               LPARAM lParam)
{
  switch (uiMsg)
  {
    case WM_INITDIALOG:
      return(DXINI_InitDrvSelDlg(hWnd));

    case WM_COMMAND:
      if (IDOK == LOWORD(wParam))
        return(DXINI_CloseDrvSelDlg(hWnd));
      else if (CBN_SELCHANGE == HIWORD(wParam) && 
               IDC_DRIVERLIST == (int)LOWORD(wParam))
      {
        return(DXINI_DrvChangedDrvSelDlg(hWnd));
      }
      break;

    case WM_SYSCOMMAND:
      if (SC_CLOSE == wParam)
        return(DXINI_CloseDrvSelDlg(hWnd));
      break;
  }

  return(0);
}


// ===========================================================================
// Function:    DXINI_ChooseDriver
//
// Description: Shows a dialog so the user may select a DirectDraw driver and
//              3D device.
//
// Params:      nDriver:    The driver to select
//              n3DDevice:  The 3D device to select
//              bWindowed:  Are we using windowed mode
//
// Returns:     BOOL
//
// Comments:    
// ===========================================================================
BOOL DXINI_ChooseDriver(int nDriver /*= -1*/, int n3DDevice /*= -1*/,
                        BOOL bWindowed /*= FALSE*/)
{
  // Setup our dialog structure
  g_pDlgInfo = new DXINI_SelectionDialogInfo;
  ZeroMemory(g_pDlgInfo, sizeof(DXINI_SelectionDialogInfo));

  g_pDlgInfo->nSelectedDriver   = nDriver;
  g_pDlgInfo->nSelected3DDevice = n3DDevice;
  g_pDlgInfo->bWindowedMode     = bWindowed;

  // If no driver or 3D device was specified, open the dialog and ask the user
  //  what they want to use
  if (-1 == nDriver && -1 == n3DDevice)
  {
    // Get the driver list
    g_pDlgInfo->nNumDrivers = DXINI_GetDriverList(g_pDlgInfo->aDrivers,
                                                  MAX_DDDRIVERS);
    if (0 >= g_pDlgInfo->nNumDrivers) return(FALSE);

    // Setup the dialog
    WORD* pDlgTemplate = (PWORD)LocalAlloc(LPTR, 1000);
    DXINI_InitDrvSelDlgTemplate(pDlgTemplate);

    // Make sure the mouse is shown
    ShowCursor(TRUE);

    // Show the dialog
    DialogBoxIndirect(GetModuleHandle(NULL), (LPDLGTEMPLATE)pDlgTemplate, 
                      pc3D::GetHWnd(), (DLGPROC)DrvSelDlgProc);

    // Hide the cursor
    ShowCursor(FALSE);

    // Free up the memory we allocated for the dialog template
    LocalFree(LocalHandle(pDlgTemplate));
  }

  ASSERT(0 <= g_pDlgInfo->nSelectedDriver);
  ASSERT(0 <= g_pDlgInfo->nSelected3DDevice);

  // Now set the choices
  if (!DXINI_SetSelectedDriver((DWORD)(g_pDlgInfo->nSelectedDriver)))
    return(FALSE);
  if (!DXINI_SetSelected3DDevice((DWORD)(g_pDlgInfo->nSelected3DDevice)))
    return(FALSE);
  if (!DXINI_SetWindowedMode(g_pDlgInfo->bWindowedMode))
    return(FALSE);

  // Release our allocated resources
  DESTROYPOINTER(g_pDlgInfo);

  return(TRUE);
}


// ===========================================================================
// Function:    DXINI_CheckForSelectionChoice
//
// Description: Checks the command line for the "-choosesettings" option.
//
// Params:      void
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the "-choosesettings" option is present in the
//              command line and FALSE if it is not.
// ===========================================================================
BOOL DXINI_CheckForChooseSettings()
{
  return(NULL != _tcsstr(GetCommandLine(), TEXT("-choosesettings")));
}


// ===========================================================================
// Function:    DXINI_WriteD3DDeviceInfo
//
// Description: Writes all of the capabilites of a D3D device to the INI file.
//
// Params:      d3dDeviceInfo:  Structure containing the D3D device info
//              szDeviceName:   Name of the D3D device
//              szdeviceDesc:   Text description of the D3D device
//              szDevice:       Name of the device we are going to use as the
//                              name of the INI file
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if everything is written successfully and FALSE
//              if an error occur.
// ===========================================================================
BOOL DXINI_WriteD3DDeviceInfo(const D3DDEVICEDESC& d3dDeviceInfo,
                              LPSTR szDeviceName, LPSTR szDeviceDesc,
                              LPCTSTR szDevice)
{
  TCHAR szBuffer[MAX_PATH];
  LPCTSTR szFile  = NULL;
  LPCTSTR szGroup = szDevice;
  TCHAR szValue[20];

  // Get the path of the module name
  ::GetModuleFileName(NULL, szBuffer, MAX_PATH);

  // Now strip off the file name from the path
  TCHAR* szTmp = _tcsrchr(szBuffer, TEXT('\\'));
  szTmp[1] = '\0';

  // Stick on the INI file name
  _tcscat(szBuffer, szDevice);
  _tcscat(szBuffer, TEXT(".ini"));
  szFile = szBuffer;

  // Setup our local macros

#define WRITESTRING(key, str)                                               \
  if (!WritePrivateProfileString(szGroup, key, str, szFile))                \
    return(FALSE)

#define WRITEDWORD(key, value)                                              \
  _ultoa((value), szValue, 10); WRITESTRING(key, szValue)

#define WRITEFLOAT(key, value)                                              \
  _stprintf(szValue, "%.5f", value); WRITESTRING(key, szValue)

#define WRITEFLAG(key, testvalue, flag)                                     \
  WRITEDWORD(key, (((testvalue) & (flag)) ? TRUE : FALSE))

  // Now we have our file, start writing the caps

  WRITESTRING(TEXT("Name"), szDeviceName);
  WRITESTRING(TEXT("Description"), szDeviceDesc);

  // Colour model
  switch (d3dDeviceInfo.dcmColorModel)
  {
    case D3DCOLOR_MONO: WRITESTRING(DXINI_3DCAP_COLOURMODEL, TEXT("Mono"));    break;
    case D3DCOLOR_RGB:  WRITESTRING(DXINI_3DCAP_COLOURMODEL, TEXT("RGB"));     break;
    default:            WRITESTRING(DXINI_3DCAP_COLOURMODEL, TEXT("(empty)")); break;
  }

  WRITEDWORD(DXINI_3DCAP_CLIPPING, d3dDeviceInfo.bClipping);

  // Supported rendering depths
  szValue[0] = 0;
  if (d3dDeviceInfo.dwDeviceRenderBitDepth & DDBD_8)  _tcscat(szValue, TEXT("8"));
  if (d3dDeviceInfo.dwDeviceRenderBitDepth & DDBD_16) _tcscat(szValue, TEXT(", 16"));
  if (d3dDeviceInfo.dwDeviceRenderBitDepth & DDBD_24) _tcscat(szValue, TEXT(", 24"));
  if (d3dDeviceInfo.dwDeviceRenderBitDepth & DDBD_32) _tcscat(szValue, TEXT(", 32"));
  if (',' == szValue[0])
  {
    int nLen = lstrlen(szValue) - 2;
    TCHAR szTmp[MAX_PATH];
    lstrcpy(szTmp, &szValue[2]);
    lstrcpy(szValue, szTmp);
  }
  WRITESTRING(DXINI_3DCAP_SUPPORTEDRENDERDEPTHS, szValue);

  // Supported rendering Z buffer formats
  szValue[0] = 0;
  if (d3dDeviceInfo.dwDeviceZBufferBitDepth & DDBD_8)  _tcscat(szValue, TEXT("8"));
  if (d3dDeviceInfo.dwDeviceZBufferBitDepth & DDBD_16) _tcscat(szValue, TEXT(", 16"));
  if (d3dDeviceInfo.dwDeviceZBufferBitDepth & DDBD_24) _tcscat(szValue, TEXT(", 24"));
  if (d3dDeviceInfo.dwDeviceZBufferBitDepth & DDBD_32) _tcscat(szValue, TEXT(", 32"));
  if (',' == szValue[0])
  {
    int nLen = strlen(szValue) - 2;
    TCHAR szTmp[MAX_PATH];
    lstrcpy(szTmp, &szValue[2]);
    lstrcpy(szValue, szTmp);
  }
  WRITESTRING(DXINI_3DCAP_SUPPORTEDZBUFFERFORMATS, szValue);

  WRITEDWORD(DXINI_3DCAP_MAXEXECUTEBUFFERSIZE, d3dDeviceInfo.dwMaxBufferSize);
  WRITEDWORD(DXINI_3DCAP_MAXVERTEXCOUNT, d3dDeviceInfo.dwMaxVertexCount);
  WRITEDWORD(DXINI_3DCAP_MINTEXTUREWIDTH, d3dDeviceInfo.dwMinTextureWidth);
  WRITEDWORD(DXINI_3DCAP_MAXTEXTUREWIDTH, d3dDeviceInfo.dwMaxTextureWidth);
  WRITEDWORD(DXINI_3DCAP_MINTEXTUREHEIGHT, d3dDeviceInfo.dwMinTextureHeight);
  WRITEDWORD(DXINI_3DCAP_MAXTEXTUREHEIGHT, d3dDeviceInfo.dwMaxTextureHeight);
  WRITEDWORD(DXINI_3DCAP_MINSTIPPLEWIDTH, d3dDeviceInfo.dwMinStippleWidth);
  WRITEDWORD(DXINI_3DCAP_MAXSTIPPLEWIDTH, d3dDeviceInfo.dwMaxStippleWidth);
  WRITEDWORD(DXINI_3DCAP_MINSTIPPLEHEIGHT, d3dDeviceInfo.dwMinStippleHeight);
  WRITEDWORD(DXINI_3DCAP_MAXSTIPPLEHEIGHT, d3dDeviceInfo.dwMaxStippleHeight);
  WRITEDWORD(DXINI_3DCAP_MAXTEXTUREREPEAT, d3dDeviceInfo.dwMaxTextureRepeat);
  WRITEDWORD(DXINI_3DCAP_MAXTEXTUREASPECTRATIO, d3dDeviceInfo.dwMaxTextureAspectRatio);
  WRITEDWORD(DXINI_3DCAP_MAXANISOTROPY, d3dDeviceInfo.dwMaxAnisotropy);
  WRITEFLOAT(DXINI_3DCAP_GUARDBANDLEFT, d3dDeviceInfo.dvGuardBandLeft);
  WRITEFLOAT(DXINI_3DCAP_GUARDBANDRIGHT, d3dDeviceInfo.dvGuardBandRight);
  WRITEFLOAT(DXINI_3DCAP_GUARDBANDTOP, d3dDeviceInfo.dvGuardBandTop);
  WRITEFLOAT(DXINI_3DCAP_GUARDBANDBOTTOM, d3dDeviceInfo.dvGuardBandBottom);
  WRITEFLOAT(DXINI_3DCAP_EXTENTSADJUST, d3dDeviceInfo.dvExtentsAdjust);
  WRITEDWORD(DXINI_3DCAP_MAXTEXTURESTAGES, (DWORD)d3dDeviceInfo.wMaxTextureBlendStages);
  WRITEDWORD(DXINI_3DCAP_MAXSIMULTANEOUSTEXTURES, (DWORD)d3dDeviceInfo.wMaxSimultaneousTextures);

  // dwDevCaps
  szGroup = TEXT("DeviceCaps");
#define FLAGS         d3dDeviceInfo.dwDevCaps
  WRITEFLAG(DXINI_3DCAP_CANRENDERAFTERFLIP, FLAGS, D3DDEVCAPS_CANRENDERAFTERFLIP);
  WRITEFLAG(DXINI_3DCAP_DRAWPRIMTLVERTEX, FLAGS, D3DDEVCAPS_DRAWPRIMTLVERTEX);
  WRITEFLAG(DXINI_3DCAP_EXECUTESYSTEMMEMORY, FLAGS, D3DDEVCAPS_EXECUTESYSTEMMEMORY);
  WRITEFLAG(DXINI_3DCAP_EXECUTEVIDEOMEMORY, FLAGS, D3DDEVCAPS_EXECUTEVIDEOMEMORY);
  WRITEFLAG(DXINI_3DCAP_FLOATTLVERTEX, FLAGS, D3DDEVCAPS_FLOATTLVERTEX);
  WRITEFLAG(DXINI_3DCAP_SEPARATETEXTUREMEMORIES, FLAGS, D3DDEVCAPS_SEPARATETEXTUREMEMORIES);
  WRITEFLAG(DXINI_3DCAP_SORTDECREASINGZ, FLAGS, D3DDEVCAPS_SORTDECREASINGZ);
  WRITEFLAG(DXINI_3DCAP_SORTEXACT, FLAGS, D3DDEVCAPS_SORTEXACT);
  WRITEFLAG(DXINI_3DCAP_SORTINCREASINGZ, FLAGS, D3DDEVCAPS_SORTINCREASINGZ);
//  WRITEFLAG(DXINI_3DCAP_TEXREPEATNOTSCALEDBYSIZE, FLAGS, D3DDEVCAPS_TEXREPEATNOTSCALEDBYSIZE);
  WRITEFLAG(DXINI_3DCAP_TEXTURENONLOCALVIDMEM, FLAGS, D3DDEVCAPS_TEXTURENONLOCALVIDMEM);
  WRITEFLAG(DXINI_3DCAP_TEXTURESYSTEMMEMORY, FLAGS, D3DDEVCAPS_TEXTURESYSTEMMEMORY);
  WRITEFLAG(DXINI_3DCAP_TEXTUREVIDEOMEMORY, FLAGS, D3DDEVCAPS_TEXTUREVIDEOMEMORY);
  WRITEFLAG(DXINI_3DCAP_TLVERTEXSYSTEMMEMORY, FLAGS, D3DDEVCAPS_TLVERTEXSYSTEMMEMORY);
  WRITEFLAG(DXINI_3DCAP_TLVERTEXVIDEOMEMORY, FLAGS, D3DDEVCAPS_TLVERTEXVIDEOMEMORY);
#undef FLAGS

  // dtcTransformCaps
  szGroup = TEXT("TransformCaps");
#define FLAGS         d3dDeviceInfo.dtcTransformCaps.dwCaps
  WRITEFLAG(DXINI_3DCAP_TRANSFORM_CLIP, FLAGS, D3DTRANSFORMCAPS_CLIP);
#undef FLAGS

  // dlcLightingCaps
  szGroup = TEXT("LightingCaps");
#define FLAGS         d3dDeviceInfo.dlcLightingCaps.dwCaps
  WRITEFLAG(DXINI_3DCAP_LIGHT_DIRECTIONAL, FLAGS, D3DLIGHTCAPS_DIRECTIONAL);
//  WRITEFLAG(DXINI_3DCAP_LIGHT_GLSPOT, FLAGS, D3DLIGHTCAPS_GLSPOT);
//  WRITEFLAG(DXINI_3DCAP_LIGHT_PARALLELPOINT, FLAGS, D3DLIGHTCAPS_PARALLELPOINT);
  WRITEFLAG(DXINI_3DCAP_LIGHT_POINT, FLAGS, D3DLIGHTCAPS_POINT);
  WRITEFLAG(DXINI_3DCAP_LIGHT_SPOT, FLAGS, D3DLIGHTCAPS_SPOT);
#undef FLAGS

  switch (d3dDeviceInfo.dlcLightingCaps.dwLightingModel)
  {
    case D3DLIGHTINGMODEL_MONO: WRITESTRING(DXINI_3DCAP_LIGHT_MODEL, TEXT("Mono"));    break;
    case D3DLIGHTINGMODEL_RGB:  WRITESTRING(DXINI_3DCAP_LIGHT_MODEL, TEXT("RGB"));     break;
    default:                    WRITESTRING(DXINI_3DCAP_LIGHT_MODEL, TEXT("(empty)")); break;
  }
  
  WRITEDWORD(DXINI_3DCAP_LIGHT_MAXLIGHTS, d3dDeviceInfo.dlcLightingCaps.dwNumLights);

  // dpcLineCaps
  szGroup = TEXT("LineCaps");
#define FLAGS         d3dDeviceInfo.dpcLineCaps.dwMiscCaps
  WRITEFLAG(DXINI_3DCAP_LINE_MISC_CONFORMANT, FLAGS, D3DPMISCCAPS_CONFORMANT);
  WRITEFLAG(DXINI_3DCAP_LINE_MISC_CULLCCW, FLAGS, D3DPMISCCAPS_CULLCCW);
  WRITEFLAG(DXINI_3DCAP_LINE_MISC_CULLCW, FLAGS, D3DPMISCCAPS_CULLCW);
  WRITEFLAG(DXINI_3DCAP_LINE_MISC_CULLNONE, FLAGS, D3DPMISCCAPS_CULLNONE);
  WRITEFLAG(DXINI_3DCAP_LINE_MISC_LINEPATTERNREP, FLAGS, D3DPMISCCAPS_LINEPATTERNREP);
  WRITEFLAG(DXINI_3DCAP_LINE_MISC_MASKPLANES, FLAGS, D3DPMISCCAPS_MASKPLANES);
  WRITEFLAG(DXINI_3DCAP_LINE_MISC_MASKZ, FLAGS, D3DPMISCCAPS_MASKZ);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcLineCaps.dwRasterCaps
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_ANISOTROPY, FLAGS, D3DPRASTERCAPS_ANISOTROPY);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_ANTIALIASEDGES, FLAGS, D3DPRASTERCAPS_ANTIALIASEDGES);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_ANTIALIASSORTDEPENDENT, FLAGS, D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_ANTIALIASSORTINDEPENDENT, FLAGS, D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_DITHER, FLAGS, D3DPRASTERCAPS_DITHER);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_FOGRANGE, FLAGS, D3DPRASTERCAPS_FOGRANGE);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_FOGTABLE, FLAGS, D3DPRASTERCAPS_FOGTABLE);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_FOGVERTEX, FLAGS, D3DPRASTERCAPS_FOGVERTEX);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_MIPMAPLODBIAS, FLAGS, D3DPRASTERCAPS_MIPMAPLODBIAS);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_PAT, FLAGS, D3DPRASTERCAPS_PAT);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_ROP2, FLAGS, D3DPRASTERCAPS_ROP2);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_STIPPLE, FLAGS, D3DPRASTERCAPS_STIPPLE);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_SUBPIXEL, FLAGS, D3DPRASTERCAPS_SUBPIXEL);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_SUBPIXELX, FLAGS, D3DPRASTERCAPS_SUBPIXELX);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_TRANSLUCENTSORTINDEPENDENT, FLAGS, D3DPRASTERCAPS_TRANSLUCENTSORTINDEPENDENT);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_WBUFFER, FLAGS, D3DPRASTERCAPS_WBUFFER);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_WFOG, FLAGS, D3DPRASTERCAPS_WFOG);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_XOR, FLAGS, D3DPRASTERCAPS_XOR);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_ZBIAS, FLAGS, D3DPRASTERCAPS_ZBIAS);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_ZBUFFERLESSHSR, FLAGS, D3DPRASTERCAPS_ZBUFFERLESSHSR);
  WRITEFLAG(DXINI_3DCAP_LINE_RAST_ZTEST, FLAGS, D3DPRASTERCAPS_ZTEST);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcLineCaps.dwZCmpCaps
  WRITEFLAG(DXINI_3DCAP_LINE_ZCMP_ALWAYS, FLAGS, D3DPCMPCAPS_ALWAYS);
  WRITEFLAG(DXINI_3DCAP_LINE_ZCMP_EQUAL, FLAGS, D3DPCMPCAPS_EQUAL);
  WRITEFLAG(DXINI_3DCAP_LINE_ZCMP_GREATER, FLAGS, D3DPCMPCAPS_GREATER);
  WRITEFLAG(DXINI_3DCAP_LINE_ZCMP_GREATEREQUAL, FLAGS, D3DPCMPCAPS_GREATEREQUAL);
  WRITEFLAG(DXINI_3DCAP_LINE_ZCMP_LESS, FLAGS, D3DPCMPCAPS_LESS);
  WRITEFLAG(DXINI_3DCAP_LINE_ZCMP_LESSEQUAL, FLAGS, D3DPCMPCAPS_LESSEQUAL);
  WRITEFLAG(DXINI_3DCAP_LINE_ZCMP_NEVER, FLAGS, D3DPCMPCAPS_NEVER);
  WRITEFLAG(DXINI_3DCAP_LINE_ZCMP_NOTEQUAL, FLAGS, D3DPCMPCAPS_NOTEQUAL);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcLineCaps.dwSrcBlendCaps
  WRITEFLAG(DXINI_3DCAP_LINE_SBLND_BOTHINVSRCALPHA, FLAGS, D3DPBLENDCAPS_BOTHINVSRCALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_SBLND_BOTHSRCALPHA, FLAGS, D3DPBLENDCAPS_BOTHSRCALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_SBLND_DESTALPHA, FLAGS, D3DPBLENDCAPS_DESTALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_SBLND_DESTCOLOR, FLAGS, D3DPBLENDCAPS_DESTCOLOR);
  WRITEFLAG(DXINI_3DCAP_LINE_SBLND_INVDESTALPHA, FLAGS, D3DPBLENDCAPS_INVDESTALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_SBLND_INVDESTCOLOR, FLAGS, D3DPBLENDCAPS_INVDESTCOLOR);
  WRITEFLAG(DXINI_3DCAP_LINE_SBLND_INVSRCALPHA, FLAGS, D3DPBLENDCAPS_INVSRCALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_SBLND_INVSRCCOLOR, FLAGS, D3DPBLENDCAPS_INVSRCCOLOR);
  WRITEFLAG(DXINI_3DCAP_LINE_SBLND_ONE, FLAGS, D3DPBLENDCAPS_ONE);
  WRITEFLAG(DXINI_3DCAP_LINE_SBLND_SRCALPHA, FLAGS, D3DPBLENDCAPS_SRCALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_SBLND_SRCALPHASAT, FLAGS, D3DPBLENDCAPS_SRCALPHASAT);
  WRITEFLAG(DXINI_3DCAP_LINE_SBLND_SRCCOLOR, FLAGS, D3DPBLENDCAPS_SRCCOLOR);
  WRITEFLAG(DXINI_3DCAP_LINE_SBLND_ZERO, FLAGS, D3DPBLENDCAPS_ZERO);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcLineCaps.dwDestBlendCaps
  WRITEFLAG(DXINI_3DCAP_LINE_DBLND_BOTHINVSRCALPHA, FLAGS, D3DPBLENDCAPS_BOTHINVSRCALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_DBLND_BOTHSRCALPHA, FLAGS, D3DPBLENDCAPS_BOTHSRCALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_DBLND_DESTALPHA, FLAGS, D3DPBLENDCAPS_DESTALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_DBLND_DESTCOLOR, FLAGS, D3DPBLENDCAPS_DESTCOLOR);
  WRITEFLAG(DXINI_3DCAP_LINE_DBLND_INVDESTALPHA, FLAGS, D3DPBLENDCAPS_INVDESTALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_DBLND_INVDESTCOLOR, FLAGS, D3DPBLENDCAPS_INVDESTCOLOR);
  WRITEFLAG(DXINI_3DCAP_LINE_DBLND_INVSRCALPHA, FLAGS, D3DPBLENDCAPS_INVSRCALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_DBLND_INVSRCCOLOR, FLAGS, D3DPBLENDCAPS_INVSRCCOLOR);
  WRITEFLAG(DXINI_3DCAP_LINE_DBLND_ONE, FLAGS, D3DPBLENDCAPS_ONE);
  WRITEFLAG(DXINI_3DCAP_LINE_DBLND_SRCALPHA, FLAGS, D3DPBLENDCAPS_SRCALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_DBLND_SRCALPHASAT, FLAGS, D3DPBLENDCAPS_SRCALPHASAT);
  WRITEFLAG(DXINI_3DCAP_LINE_DBLND_SRCCOLOR, FLAGS, D3DPBLENDCAPS_SRCCOLOR);
  WRITEFLAG(DXINI_3DCAP_LINE_DBLND_ZERO, FLAGS, D3DPBLENDCAPS_ZERO);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcLineCaps.dwAlphaCmpCaps
  WRITEFLAG(DXINI_3DCAP_LINE_ACMP_ALWAYS, FLAGS, D3DPCMPCAPS_ALWAYS);
  WRITEFLAG(DXINI_3DCAP_LINE_ACMP_EQUAL, FLAGS, D3DPCMPCAPS_EQUAL);
  WRITEFLAG(DXINI_3DCAP_LINE_ACMP_GREATER, FLAGS, D3DPCMPCAPS_GREATER);
  WRITEFLAG(DXINI_3DCAP_LINE_ACMP_GREATEREQUAL, FLAGS, D3DPCMPCAPS_GREATEREQUAL);
  WRITEFLAG(DXINI_3DCAP_LINE_ACMP_LESS, FLAGS, D3DPCMPCAPS_LESS);
  WRITEFLAG(DXINI_3DCAP_LINE_ACMP_LESSEQUAL, FLAGS, D3DPCMPCAPS_LESSEQUAL);
  WRITEFLAG(DXINI_3DCAP_LINE_ACMP_NEVER, FLAGS, D3DPCMPCAPS_NEVER);
  WRITEFLAG(DXINI_3DCAP_LINE_ACMP_NOTEQUAL, FLAGS, D3DPCMPCAPS_NOTEQUAL);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcLineCaps.dwShadeCaps
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_ALPHAFLATBLEND, FLAGS, D3DPSHADECAPS_ALPHAFLATBLEND);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_ALPHAFLATSTIPPLED, FLAGS, D3DPSHADECAPS_ALPHAFLATSTIPPLED);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_ALPHAGOURAUDBLEND, FLAGS, D3DPSHADECAPS_ALPHAGOURAUDBLEND);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_ALPHAGOURAUDSTIPPLED, FLAGS, D3DPSHADECAPS_ALPHAGOURAUDSTIPPLED);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_ALPHAPHONGBLEND, FLAGS, D3DPSHADECAPS_ALPHAPHONGBLEND);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_ALPHAPHONGSTIPPLED, FLAGS, D3DPSHADECAPS_ALPHAPHONGSTIPPLED);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_COLORFLATMONO, FLAGS, D3DPSHADECAPS_COLORFLATMONO);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_COLORFLATRGB, FLAGS, D3DPSHADECAPS_COLORFLATRGB);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_COLORGOURAUDMONO, FLAGS, D3DPSHADECAPS_COLORGOURAUDMONO);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_COLORGOURAUDRGB, FLAGS, D3DPSHADECAPS_COLORGOURAUDRGB);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_COLORPHONGMONO, FLAGS, D3DPSHADECAPS_COLORPHONGMONO);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_COLORPHONGRGB, FLAGS, D3DPSHADECAPS_COLORPHONGRGB);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_FOGFLAT, FLAGS, D3DPSHADECAPS_FOGFLAT);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_FOGGOURAUD, FLAGS, D3DPSHADECAPS_FOGGOURAUD);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_FOGPHONG, FLAGS, D3DPSHADECAPS_FOGPHONG);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_SPECULARFLATMONO, FLAGS, D3DPSHADECAPS_SPECULARFLATMONO);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_SPECULARFLATRGB, FLAGS, D3DPSHADECAPS_SPECULARFLATRGB);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_SPECULARGOURAUDMONO, FLAGS, D3DPSHADECAPS_SPECULARGOURAUDMONO);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_SPECULARGOURAUDRGB, FLAGS, D3DPSHADECAPS_SPECULARGOURAUDRGB);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_SPECULARPHONGMONO, FLAGS, D3DPSHADECAPS_SPECULARPHONGMONO);
  WRITEFLAG(DXINI_3DCAP_LINE_SHADE_SPECULARPHONGRGB, FLAGS, D3DPSHADECAPS_SPECULARPHONGRGB);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcLineCaps.dwTextureCaps
  WRITEFLAG(DXINI_3DCAP_LINE_TXTR_ALPHA, FLAGS, D3DPTEXTURECAPS_ALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_TXTR_ALPHAPALETTE, FLAGS, D3DPTEXTURECAPS_ALPHAPALETTE);
  WRITEFLAG(DXINI_3DCAP_LINE_TXTR_BORDER, FLAGS, D3DPTEXTURECAPS_BORDER);
  WRITEFLAG(DXINI_3DCAP_LINE_TXTR_NONPOW2CONDITIONAL, FLAGS, D3DPTEXTURECAPS_NONPOW2CONDITIONAL);
  WRITEFLAG(DXINI_3DCAP_LINE_TXTR_PERSPECTIVE, FLAGS, D3DPTEXTURECAPS_PERSPECTIVE);
  WRITEFLAG(DXINI_3DCAP_LINE_TXTR_POW2, FLAGS, D3DPTEXTURECAPS_POW2);
  WRITEFLAG(DXINI_3DCAP_LINE_TXTR_SQUAREONLY, FLAGS, D3DPTEXTURECAPS_SQUAREONLY);
  WRITEFLAG(DXINI_3DCAP_LINE_TXTR_TEXREPEATNOTSCALEDBYSIZE, FLAGS, D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE);
  WRITEFLAG(DXINI_3DCAP_LINE_TXTR_TRANSPARENCY, FLAGS, D3DPTEXTURECAPS_TRANSPARENCY);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcLineCaps.dwTextureFilterCaps
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_LINEAR, FLAGS, D3DPTFILTERCAPS_LINEAR);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_LINEARMIPLINEAR, FLAGS, D3DPTFILTERCAPS_LINEARMIPLINEAR);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_LINEARMIPNEAREST, FLAGS, D3DPTFILTERCAPS_LINEARMIPNEAREST);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_MIPLINEAR, FLAGS, D3DPTFILTERCAPS_MIPLINEAR);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_MIPNEAREST, FLAGS, D3DPTFILTERCAPS_MIPNEAREST);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_NEAREST, FLAGS, D3DPTFILTERCAPS_NEAREST);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_MAGFAFLATCUBIC, FLAGS, D3DPTFILTERCAPS_MAGFAFLATCUBIC);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_MAGFANISOTROPIC, FLAGS, D3DPTFILTERCAPS_MAGFANISOTROPIC);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_MAGFGAUSSIANCUBIC, FLAGS, D3DPTFILTERCAPS_MAGFGAUSSIANCUBIC);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_MAGFLINEAR, FLAGS, D3DPTFILTERCAPS_MAGFLINEAR);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_MAGFPOINT, FLAGS, D3DPTFILTERCAPS_MAGFPOINT);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_MINFANISOTROPIC, FLAGS, D3DPTFILTERCAPS_MINFANISOTROPIC);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_MINFLINEAR, FLAGS, D3DPTFILTERCAPS_MINFLINEAR);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_MINFPOINT, FLAGS, D3DPTFILTERCAPS_MINFPOINT);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_MIPFLINEAR, FLAGS, D3DPTFILTERCAPS_MIPFLINEAR);
  WRITEFLAG(DXINI_3DCAP_LINE_TFILT_MIPFPOINT, FLAGS, D3DPTFILTERCAPS_MIPFPOINT);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcLineCaps.dwTextureBlendCaps
  WRITEFLAG(DXINI_3DCAP_LINE_TBLND_ADD, FLAGS, D3DPTBLENDCAPS_ADD);
  WRITEFLAG(DXINI_3DCAP_LINE_TBLND_COPY, FLAGS, D3DPTBLENDCAPS_COPY);
  WRITEFLAG(DXINI_3DCAP_LINE_TBLND_DECAL, FLAGS, D3DPTBLENDCAPS_DECAL);
  WRITEFLAG(DXINI_3DCAP_LINE_TBLND_DECALALPHA, FLAGS, D3DPTBLENDCAPS_DECALALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_TBLND_DECALMASK, FLAGS, D3DPTBLENDCAPS_DECALMASK);
  WRITEFLAG(DXINI_3DCAP_LINE_TBLND_MODULATE, FLAGS, D3DPTBLENDCAPS_MODULATE);
  WRITEFLAG(DXINI_3DCAP_LINE_TBLND_MODULATEALPHA, FLAGS, D3DPTBLENDCAPS_MODULATEALPHA);
  WRITEFLAG(DXINI_3DCAP_LINE_TBLND_MODULATEMASK, FLAGS, D3DPTBLENDCAPS_MODULATEMASK);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcLineCaps.dwTextureAddressCaps 
  WRITEFLAG(DXINI_3DCAP_LINE_TADDR_BORDER, FLAGS, D3DPTADDRESSCAPS_BORDER);
  WRITEFLAG(DXINI_3DCAP_LINE_TADDR_CLAMP, FLAGS, D3DPTADDRESSCAPS_CLAMP);
  WRITEFLAG(DXINI_3DCAP_LINE_TADDR_INDEPENDENTUV, FLAGS, D3DPTADDRESSCAPS_INDEPENDENTUV);
  WRITEFLAG(DXINI_3DCAP_LINE_TADDR_MIRROR, FLAGS, D3DPTADDRESSCAPS_MIRROR);
  WRITEFLAG(DXINI_3DCAP_LINE_TADDR_WRAP, FLAGS, D3DPTADDRESSCAPS_WRAP);
#undef FLAGS

  WRITEDWORD(DXINI_3DCAP_LINE_STIPPLEWIDTH, d3dDeviceInfo.dpcLineCaps.dwStippleWidth);
  WRITEDWORD(DXINI_3DCAP_LINE_STIPPLEHEIGHT, d3dDeviceInfo.dpcLineCaps.dwStippleHeight);

  // dpcTriCaps 
  szGroup = TEXT("TriangleCaps");
#define FLAGS         d3dDeviceInfo.dpcTriCaps.dwMiscCaps
  WRITEFLAG(DXINI_3DCAP_TRI_MISC_CONFORMANT, FLAGS, D3DPMISCCAPS_CONFORMANT);
  WRITEFLAG(DXINI_3DCAP_TRI_MISC_CULLCCW, FLAGS, D3DPMISCCAPS_CULLCCW);
  WRITEFLAG(DXINI_3DCAP_TRI_MISC_CULLCW, FLAGS, D3DPMISCCAPS_CULLCW);
  WRITEFLAG(DXINI_3DCAP_TRI_MISC_CULLNONE, FLAGS, D3DPMISCCAPS_CULLNONE);
  WRITEFLAG(DXINI_3DCAP_TRI_MISC_LINEPATTERNREP, FLAGS, D3DPMISCCAPS_LINEPATTERNREP);
  WRITEFLAG(DXINI_3DCAP_TRI_MISC_MASKPLANES, FLAGS, D3DPMISCCAPS_MASKPLANES);
  WRITEFLAG(DXINI_3DCAP_TRI_MISC_MASKZ, FLAGS, D3DPMISCCAPS_MASKZ);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcTriCaps.dwRasterCaps
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_ANISOTROPY, FLAGS, D3DPRASTERCAPS_ANISOTROPY);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_ANTIALIASEDGES, FLAGS, D3DPRASTERCAPS_ANTIALIASEDGES);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_ANTIALIASSORTDEPENDENT, FLAGS, D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_ANTIALIASSORTINDEPENDENT, FLAGS, D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_DITHER, FLAGS, D3DPRASTERCAPS_DITHER);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_FOGRANGE, FLAGS, D3DPRASTERCAPS_FOGRANGE);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_FOGTABLE, FLAGS, D3DPRASTERCAPS_FOGTABLE);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_FOGVERTEX, FLAGS, D3DPRASTERCAPS_FOGVERTEX);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_MIPMAPLODBIAS, FLAGS, D3DPRASTERCAPS_MIPMAPLODBIAS);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_PAT, FLAGS, D3DPRASTERCAPS_PAT);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_ROP2, FLAGS, D3DPRASTERCAPS_ROP2);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_STIPPLE, FLAGS, D3DPRASTERCAPS_STIPPLE);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_SUBPIXEL, FLAGS, D3DPRASTERCAPS_SUBPIXEL);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_SUBPIXELX, FLAGS, D3DPRASTERCAPS_SUBPIXELX);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_TRANSLUCENTSORTINDEPENDENT, FLAGS, D3DPRASTERCAPS_TRANSLUCENTSORTINDEPENDENT);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_WBUFFER, FLAGS, D3DPRASTERCAPS_WBUFFER);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_WFOG, FLAGS, D3DPRASTERCAPS_WFOG);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_XOR, FLAGS, D3DPRASTERCAPS_XOR);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_ZBIAS, FLAGS, D3DPRASTERCAPS_ZBIAS);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_ZBUFFERLESSHSR, FLAGS, D3DPRASTERCAPS_ZBUFFERLESSHSR);
  WRITEFLAG(DXINI_3DCAP_TRI_RAST_ZTEST, FLAGS, D3DPRASTERCAPS_ZTEST);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcTriCaps.dwZCmpCaps
  WRITEFLAG(DXINI_3DCAP_TRI_ZCMP_ALWAYS, FLAGS, D3DPCMPCAPS_ALWAYS);
  WRITEFLAG(DXINI_3DCAP_TRI_ZCMP_EQUAL, FLAGS, D3DPCMPCAPS_EQUAL);
  WRITEFLAG(DXINI_3DCAP_TRI_ZCMP_GREATER, FLAGS, D3DPCMPCAPS_GREATER);
  WRITEFLAG(DXINI_3DCAP_TRI_ZCMP_GREATEREQUAL, FLAGS, D3DPCMPCAPS_GREATEREQUAL);
  WRITEFLAG(DXINI_3DCAP_TRI_ZCMP_LESS, FLAGS, D3DPCMPCAPS_LESS);
  WRITEFLAG(DXINI_3DCAP_TRI_ZCMP_LESSEQUAL, FLAGS, D3DPCMPCAPS_LESSEQUAL);
  WRITEFLAG(DXINI_3DCAP_TRI_ZCMP_NEVER, FLAGS, D3DPCMPCAPS_NEVER);
  WRITEFLAG(DXINI_3DCAP_TRI_ZCMP_NOTEQUAL, FLAGS, D3DPCMPCAPS_NOTEQUAL);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcTriCaps.dwSrcBlendCaps
  WRITEFLAG(DXINI_3DCAP_TRI_SBLND_BOTHINVSRCALPHA, FLAGS, D3DPBLENDCAPS_BOTHINVSRCALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_SBLND_BOTHSRCALPHA, FLAGS, D3DPBLENDCAPS_BOTHSRCALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_SBLND_DESTALPHA, FLAGS, D3DPBLENDCAPS_DESTALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_SBLND_DESTCOLOR, FLAGS, D3DPBLENDCAPS_DESTCOLOR);
  WRITEFLAG(DXINI_3DCAP_TRI_SBLND_INVDESTALPHA, FLAGS, D3DPBLENDCAPS_INVDESTALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_SBLND_INVDESTCOLOR, FLAGS, D3DPBLENDCAPS_INVDESTCOLOR);
  WRITEFLAG(DXINI_3DCAP_TRI_SBLND_INVSRCALPHA, FLAGS, D3DPBLENDCAPS_INVSRCALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_SBLND_INVSRCCOLOR, FLAGS, D3DPBLENDCAPS_INVSRCCOLOR);
  WRITEFLAG(DXINI_3DCAP_TRI_SBLND_ONE, FLAGS, D3DPBLENDCAPS_ONE);
  WRITEFLAG(DXINI_3DCAP_TRI_SBLND_SRCALPHA, FLAGS, D3DPBLENDCAPS_SRCALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_SBLND_SRCALPHASAT, FLAGS, D3DPBLENDCAPS_SRCALPHASAT);
  WRITEFLAG(DXINI_3DCAP_TRI_SBLND_SRCCOLOR, FLAGS, D3DPBLENDCAPS_SRCCOLOR);
  WRITEFLAG(DXINI_3DCAP_TRI_SBLND_ZERO, FLAGS, D3DPBLENDCAPS_ZERO);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcTriCaps.dwDestBlendCaps
  WRITEFLAG(DXINI_3DCAP_TRI_DBLND_BOTHINVSRCALPHA, FLAGS, D3DPBLENDCAPS_BOTHINVSRCALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_DBLND_BOTHSRCALPHA, FLAGS, D3DPBLENDCAPS_BOTHSRCALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_DBLND_DESTALPHA, FLAGS, D3DPBLENDCAPS_DESTALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_DBLND_DESTCOLOR, FLAGS, D3DPBLENDCAPS_DESTCOLOR);
  WRITEFLAG(DXINI_3DCAP_TRI_DBLND_INVDESTALPHA, FLAGS, D3DPBLENDCAPS_INVDESTALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_DBLND_INVDESTCOLOR, FLAGS, D3DPBLENDCAPS_INVDESTCOLOR);
  WRITEFLAG(DXINI_3DCAP_TRI_DBLND_INVSRCALPHA, FLAGS, D3DPBLENDCAPS_INVSRCALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_DBLND_INVSRCCOLOR, FLAGS, D3DPBLENDCAPS_INVSRCCOLOR);
  WRITEFLAG(DXINI_3DCAP_TRI_DBLND_ONE, FLAGS, D3DPBLENDCAPS_ONE);
  WRITEFLAG(DXINI_3DCAP_TRI_DBLND_SRCALPHA, FLAGS, D3DPBLENDCAPS_SRCALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_DBLND_SRCALPHASAT, FLAGS, D3DPBLENDCAPS_SRCALPHASAT);
  WRITEFLAG(DXINI_3DCAP_TRI_DBLND_SRCCOLOR, FLAGS, D3DPBLENDCAPS_SRCCOLOR);
  WRITEFLAG(DXINI_3DCAP_TRI_DBLND_ZERO, FLAGS, D3DPBLENDCAPS_ZERO);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcTriCaps.dwAlphaCmpCaps
  WRITEFLAG(DXINI_3DCAP_TRI_ACMP_ALWAYS, FLAGS, D3DPCMPCAPS_ALWAYS);
  WRITEFLAG(DXINI_3DCAP_TRI_ACMP_EQUAL, FLAGS, D3DPCMPCAPS_EQUAL);
  WRITEFLAG(DXINI_3DCAP_TRI_ACMP_GREATER, FLAGS, D3DPCMPCAPS_GREATER);
  WRITEFLAG(DXINI_3DCAP_TRI_ACMP_GREATEREQUAL, FLAGS, D3DPCMPCAPS_GREATEREQUAL);
  WRITEFLAG(DXINI_3DCAP_TRI_ACMP_LESS, FLAGS, D3DPCMPCAPS_LESS);
  WRITEFLAG(DXINI_3DCAP_TRI_ACMP_LESSEQUAL, FLAGS, D3DPCMPCAPS_LESSEQUAL);
  WRITEFLAG(DXINI_3DCAP_TRI_ACMP_NEVER, FLAGS, D3DPCMPCAPS_NEVER);
  WRITEFLAG(DXINI_3DCAP_TRI_ACMP_NOTEQUAL, FLAGS, D3DPCMPCAPS_NOTEQUAL);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcTriCaps.dwShadeCaps
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_ALPHAFLATBLEND, FLAGS, D3DPSHADECAPS_ALPHAFLATBLEND);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_ALPHAFLATSTIPPLED, FLAGS, D3DPSHADECAPS_ALPHAFLATSTIPPLED);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_ALPHAGOURAUDBLEND, FLAGS, D3DPSHADECAPS_ALPHAGOURAUDBLEND);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_ALPHAGOURAUDSTIPPLED, FLAGS, D3DPSHADECAPS_ALPHAGOURAUDSTIPPLED);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_ALPHAPHONGBLEND, FLAGS, D3DPSHADECAPS_ALPHAPHONGBLEND);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_ALPHAPHONGSTIPPLED, FLAGS, D3DPSHADECAPS_ALPHAPHONGSTIPPLED);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_COLORFLATMONO, FLAGS, D3DPSHADECAPS_COLORFLATMONO);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_COLORFLATRGB, FLAGS, D3DPSHADECAPS_COLORFLATRGB);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_COLORGOURAUDMONO, FLAGS, D3DPSHADECAPS_COLORGOURAUDMONO);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_COLORGOURAUDRGB, FLAGS, D3DPSHADECAPS_COLORGOURAUDRGB);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_COLORPHONGMONO, FLAGS, D3DPSHADECAPS_COLORPHONGMONO);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_COLORPHONGRGB, FLAGS, D3DPSHADECAPS_COLORPHONGRGB);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_FOGFLAT, FLAGS, D3DPSHADECAPS_FOGFLAT);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_FOGGOURAUD, FLAGS, D3DPSHADECAPS_FOGGOURAUD);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_FOGPHONG, FLAGS, D3DPSHADECAPS_FOGPHONG);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_SPECULARFLATMONO, FLAGS, D3DPSHADECAPS_SPECULARFLATMONO);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_SPECULARFLATRGB, FLAGS, D3DPSHADECAPS_SPECULARFLATRGB);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_SPECULARGOURAUDMONO, FLAGS, D3DPSHADECAPS_SPECULARGOURAUDMONO);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_SPECULARGOURAUDRGB, FLAGS, D3DPSHADECAPS_SPECULARGOURAUDRGB);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_SPECULARPHONGMONO, FLAGS, D3DPSHADECAPS_SPECULARPHONGMONO);
  WRITEFLAG(DXINI_3DCAP_TRI_SHADE_SPECULARPHONGRGB, FLAGS, D3DPSHADECAPS_SPECULARPHONGRGB);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcTriCaps.dwTextureCaps
  WRITEFLAG(DXINI_3DCAP_TRI_TXTR_ALPHA, FLAGS, D3DPTEXTURECAPS_ALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_TXTR_ALPHAPALETTE, FLAGS, D3DPTEXTURECAPS_ALPHAPALETTE);
  WRITEFLAG(DXINI_3DCAP_TRI_TXTR_BORDER, FLAGS, D3DPTEXTURECAPS_BORDER);
  WRITEFLAG(DXINI_3DCAP_TRI_TXTR_NONPOW2CONDITIONAL, FLAGS, D3DPTEXTURECAPS_NONPOW2CONDITIONAL);
  WRITEFLAG(DXINI_3DCAP_TRI_TXTR_PERSPECTIVE, FLAGS, D3DPTEXTURECAPS_PERSPECTIVE);
  WRITEFLAG(DXINI_3DCAP_TRI_TXTR_POW2, FLAGS, D3DPTEXTURECAPS_POW2);
  WRITEFLAG(DXINI_3DCAP_TRI_TXTR_SQUAREONLY, FLAGS, D3DPTEXTURECAPS_SQUAREONLY);
  WRITEFLAG(DXINI_3DCAP_TRI_TXTR_TEXREPEATNOTSCALEDBYSIZE, FLAGS, D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE);
  WRITEFLAG(DXINI_3DCAP_TRI_TXTR_TRANSPARENCY, FLAGS, D3DPTEXTURECAPS_TRANSPARENCY);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcTriCaps.dwTextureFilterCaps
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_LINEAR, FLAGS, D3DPTFILTERCAPS_LINEAR);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_LINEARMIPLINEAR, FLAGS, D3DPTFILTERCAPS_LINEARMIPLINEAR);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_LINEARMIPNEAREST, FLAGS, D3DPTFILTERCAPS_LINEARMIPNEAREST);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_MIPLINEAR, FLAGS, D3DPTFILTERCAPS_MIPLINEAR);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_MIPNEAREST, FLAGS, D3DPTFILTERCAPS_MIPNEAREST);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_NEAREST, FLAGS, D3DPTFILTERCAPS_NEAREST);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_MAGFAFLATCUBIC, FLAGS, D3DPTFILTERCAPS_MAGFAFLATCUBIC);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_MAGFANISOTROPIC, FLAGS, D3DPTFILTERCAPS_MAGFANISOTROPIC);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_MAGFGAUSSIANCUBIC, FLAGS, D3DPTFILTERCAPS_MAGFGAUSSIANCUBIC);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_MAGFLINEAR, FLAGS, D3DPTFILTERCAPS_MAGFLINEAR);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_MAGFPOINT, FLAGS, D3DPTFILTERCAPS_MAGFPOINT);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_MINFANISOTROPIC, FLAGS, D3DPTFILTERCAPS_MINFANISOTROPIC);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_MINFLINEAR, FLAGS, D3DPTFILTERCAPS_MINFLINEAR);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_MINFPOINT, FLAGS, D3DPTFILTERCAPS_MINFPOINT);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_MIPFLINEAR, FLAGS, D3DPTFILTERCAPS_MIPFLINEAR);
  WRITEFLAG(DXINI_3DCAP_TRI_TFILT_MIPFPOINT, FLAGS, D3DPTFILTERCAPS_MIPFPOINT);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcTriCaps.dwTextureBlendCaps
  WRITEFLAG(DXINI_3DCAP_TRI_TBLND_ADD, FLAGS, D3DPTBLENDCAPS_ADD);
  WRITEFLAG(DXINI_3DCAP_TRI_TBLND_COPY, FLAGS, D3DPTBLENDCAPS_COPY);
  WRITEFLAG(DXINI_3DCAP_TRI_TBLND_DECAL, FLAGS, D3DPTBLENDCAPS_DECAL);
  WRITEFLAG(DXINI_3DCAP_TRI_TBLND_DECALALPHA, FLAGS, D3DPTBLENDCAPS_DECALALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_TBLND_DECALMASK, FLAGS, D3DPTBLENDCAPS_DECALMASK);
  WRITEFLAG(DXINI_3DCAP_TRI_TBLND_MODULATE, FLAGS, D3DPTBLENDCAPS_MODULATE);
  WRITEFLAG(DXINI_3DCAP_TRI_TBLND_MODULATEALPHA, FLAGS, D3DPTBLENDCAPS_MODULATEALPHA);
  WRITEFLAG(DXINI_3DCAP_TRI_TBLND_MODULATEMASK, FLAGS, D3DPTBLENDCAPS_MODULATEMASK);
#undef FLAGS

#define FLAGS         d3dDeviceInfo.dpcTriCaps.dwTextureAddressCaps 
  WRITEFLAG(DXINI_3DCAP_TRI_TADDR_BORDER, FLAGS, D3DPTADDRESSCAPS_BORDER);
  WRITEFLAG(DXINI_3DCAP_TRI_TADDR_CLAMP, FLAGS, D3DPTADDRESSCAPS_CLAMP);
  WRITEFLAG(DXINI_3DCAP_TRI_TADDR_INDEPENDENTUV, FLAGS, D3DPTADDRESSCAPS_INDEPENDENTUV);
  WRITEFLAG(DXINI_3DCAP_TRI_TADDR_MIRROR, FLAGS, D3DPTADDRESSCAPS_MIRROR);
  WRITEFLAG(DXINI_3DCAP_TRI_TADDR_WRAP, FLAGS, D3DPTADDRESSCAPS_WRAP);
#undef FLAGS

  WRITEDWORD(DXINI_3DCAP_TRI_STIPPLEWIDTH, d3dDeviceInfo.dpcTriCaps.dwStippleWidth);
  WRITEDWORD(DXINI_3DCAP_TRI_STIPPLEHEIGHT, d3dDeviceInfo.dpcTriCaps.dwStippleHeight);

  // dwStencilCaps
  szGroup = TEXT("StencilCaps");
#define FLAGS         d3dDeviceInfo.dwStencilCaps
  WRITEFLAG(DXINI_3DCAP_STENCILOP_DECR, FLAGS, D3DSTENCILCAPS_DECR);
  WRITEFLAG(DXINI_3DCAP_STENCILOP_DECRSAT, FLAGS, D3DSTENCILCAPS_DECRSAT);
  WRITEFLAG(DXINI_3DCAP_STENCILOP_INCR, FLAGS, D3DSTENCILCAPS_INCR);
  WRITEFLAG(DXINI_3DCAP_STENCILOP_INCRSAT, FLAGS, D3DSTENCILCAPS_INCRSAT);
  WRITEFLAG(DXINI_3DCAP_STENCILOP_INVERT, FLAGS, D3DSTENCILCAPS_INVERT);
  WRITEFLAG(DXINI_3DCAP_STENCILOP_KEEP, FLAGS, D3DSTENCILCAPS_KEEP);
  WRITEFLAG(DXINI_3DCAP_STENCILOP_REPLACE, FLAGS, D3DSTENCILCAPS_REPLACE);
  WRITEFLAG(DXINI_3DCAP_STENCILOP_ZERO, FLAGS, D3DSTENCILCAPS_ZERO);
#undef FLAGS

  // dwFVFCaps 
  szGroup = TEXT("FlexibleVertextFormatCaps");
#define FLAGS         d3dDeviceInfo.dwFVFCaps
  WRITEFLAG(DXINI_3DCAP_FVF_DONOTSTRIPELEMENTS, FLAGS, D3DFVFCAPS_DONOTSTRIPELEMENTS);
  WRITEFLAG(DXINI_3DCAP_FVF_TEXCOORDCOUNTMASK, FLAGS, D3DFVFCAPS_TEXCOORDCOUNTMASK);
#undef FLAGS

  // dwTextureOpCaps 
  szGroup = TEXT("TextureOpCaps");
#define FLAGS         d3dDeviceInfo.dwTextureOpCaps
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_ADD, FLAGS, D3DTEXOPCAPS_ADD);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_ADDSIGNED, FLAGS, D3DTEXOPCAPS_ADDSIGNED);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_ADDSIGNED2X, FLAGS, D3DTEXOPCAPS_ADDSIGNED2X);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_ADDSMOOTH, FLAGS, D3DTEXOPCAPS_ADDSMOOTH);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_BLENDCURRENTALPHA, FLAGS, D3DTEXOPCAPS_BLENDCURRENTALPHA);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_BLENDDIFFUSEALPHA, FLAGS, D3DTEXOPCAPS_BLENDDIFFUSEALPHA);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_BLENDFACTORALPHA, FLAGS, D3DTEXOPCAPS_BLENDFACTORALPHA);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_BLENDTEXTUREALPHA, FLAGS, D3DTEXOPCAPS_BLENDTEXTUREALPHA);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_BLENDTEXTUREALPHAPM, FLAGS, D3DTEXOPCAPS_BLENDTEXTUREALPHAPM);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_BUMPENVMAP, FLAGS, D3DTEXOPCAPS_BUMPENVMAP);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_BUMPENVMAPLUMINANCE, FLAGS, D3DTEXOPCAPS_BUMPENVMAPLUMINANCE);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_DISABLE, FLAGS, D3DTEXOPCAPS_DISABLE);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_DOTPRODUCT3, FLAGS, D3DTEXOPCAPS_DOTPRODUCT3);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_MODULATE, FLAGS, D3DTEXOPCAPS_MODULATE);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_MODULATE2X, FLAGS, D3DTEXOPCAPS_MODULATE2X);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_MODULATE4X, FLAGS, D3DTEXOPCAPS_MODULATE4X);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_MODULATEALPHA_ADDCOLOR, FLAGS, D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_MODULATECOLOR_ADDALPHA, FLAGS, D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_MODULATEINVALPHA_ADDCOLOR, FLAGS, D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_MODULATEINVCOLOR_ADDALPHA, FLAGS, D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_PREMODULATE, FLAGS, D3DTEXOPCAPS_PREMODULATE);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_SELECTARG1, FLAGS, D3DTEXOPCAPS_SELECTARG1);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_SELECTARG2, FLAGS, D3DTEXOPCAPS_SELECTARG2);
  WRITEFLAG(DXINI_3DCAP_TEXTUREOP_SUBTRACT, FLAGS, D3DTEXOPCAPS_SUBTRACT);
#undef FLAGS

#undef WRITESTRING
#undef WRITEDWORD
#undef WRITEFLOAT
#undef WRITEFLAG

  return(TRUE);
}


// ===========================================================================
// End of file ===============================================================
// ===========================================================================
