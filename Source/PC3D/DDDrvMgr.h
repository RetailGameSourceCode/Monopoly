// ===========================================================================
// Module:      DDDrvMgr.h
//
// Description: DirectDraw driver manager class definition.  This class
//              handles all aspects of DirectDraw and Direct3D setup and
//              enumeration.
//
// Created:     April 14, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================

#ifndef INC_DDDRVMGR
#define INC_DDDRVMGR

#include "DXINI.h"

// ===========================================================================
// Typdefs and defines =======================================================
// ===========================================================================

// Realistically a user can't have more than about four possible 
//  DirectDraw devices.  This is due to the fact that there are only so
//  many PCI slots in each machine and it's not likely that the user will
//  have every slot filled with a different video card.
#define MAX_DDRAWDRIVERS            12

class DDrawDriver;
class D3DDevice;

// ===========================================================================
// Global function prototypes ================================================
// ===========================================================================

BOOL WINAPI DDrawEnumCallbackEx(GUID FAR *lpGUID, LPSTR szDriverDesc,
                                LPSTR szDriverName, LPVOID lpContext,
                                HMONITOR hMonitor);


// ===========================================================================
// DDDrvMgr class ============================================================
// ===========================================================================
class DDDrvMgr
{
  // Friend functions ========================================================
  friend BOOL WINAPI DDrawEnumCallbackEx(GUID FAR *lpGUID, LPSTR szDriverDesc,
                                         LPSTR szDriverName, LPVOID lpContext,
                                         HMONITOR hMonitor);


  // Member variables ========================================================
  private:
    DDrawDriver*    m_apDDDrivers[MAX_DDRAWDRIVERS];// List if DDraw drivers
    int             m_nNumDDrawDrivers;             // Number of DDraw drivers

    HWND            m_hWnd;                         // Window handle

    int             m_nSelectedDriverIdx;   // Used when INI cannot be generated
    int             m_nSelected3DDeviceIdx; // Used when INI cannot be generated
    BOOL            m_bWindowedMode;


  // Construction / Destruction ==============================================
  public:
    DDDrvMgr(HWND hWnd = NULL);
    virtual ~DDDrvMgr()     { Destroy();                                    }


  // Private member functions ================================================
  private:
    void    Destroy();
    void    Clear()         { ZeroMemory(this, sizeof(DDDrvMgr));           }


  // Protected member functions ==============================================
  protected:
    BOOL    AddDriver(DDrawDriver* pDDrawDriver);


  // Public member functions =================================================
  public:
    BOOL  PerformEnumeration();
    BOOL  GenerateDXINIFile(DWORD dwRequiredMemory);
    void  DestroyDrivers();

    DDrawDriver*  GetDDDriverFromINI();
    D3DDevice*    GetD3DDeviceFromINI(DDrawDriver& ddDriver);

    INL DDrawDriver*  GetDDDriver(int nIdx) const;
    INL DDrawDriver*  RemoveDDDriver(int nIdx);

    int   FindDefaultDDDriver() const;

    int   GetNumDDDrivers() const       { return(m_nNumDDrawDrivers);       }
    HWND  GetHWnd() const               { return(m_hWnd);                   }

    BOOL  HasBeenEnumerated() const     { return(0 != m_nNumDDrawDrivers);  }

    BOOL  SelectDriverDialog();
    BOOL  UsingWindowedMode() const     { return(m_bWindowedMode);          }
    BOOL  UsingSelectionFromDialog() const { return(-1 != m_nSelectedDriverIdx); }
};


// ===========================================================================
// INLINE: DDDrvMgr member functions =========================================
// ===========================================================================


// ===========================================================================
INL DDrawDriver* DDDrvMgr::GetDDDriver(int nIdx) const
{
  if (0 <= nIdx && m_nNumDDrawDrivers > nIdx) return(m_apDDDrivers[nIdx]);
  return(NULL);
}

// ===========================================================================
INL DDrawDriver* DDDrvMgr::RemoveDDDriver(int nIdx)
{
  DDrawDriver* pDDDriver = NULL;
  if (0 <= nIdx && m_nNumDDrawDrivers > nIdx)
  { pDDDriver = m_apDDDrivers[nIdx]; m_apDDDrivers[nIdx] = NULL; }
  return(pDDDriver);
}


#endif // INC_DDDRVMGR
// ===========================================================================
