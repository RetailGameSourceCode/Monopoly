// ===========================================================================
// Module:      DDrawDrv.h
//
// Description: DirectDraw Driver class definition.  Instances of this class
//              represent different DirectDraw devices (usually different
//              video cards) available on the machine we are running on.
//
// Created:     April 14, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================

#ifndef INC_DDRAWDRV
#define INC_DDRAWDRV


// ===========================================================================
// Typdefs and defines =======================================================
// ===========================================================================

// Realistically, there are usually always 2 3D devices available:
//  a software RGB/MMX driver and a hardware driver.
#define MAX_3DDEVICES               6

// We can never be quite sure how many display modes will be enumerated,
//  however these days there are around 50 or so usually.
#define MAX_DISPLAYMODES            120

// Enumerated type for the different bit depths available
typedef enum
{
  _0BIT   = 0x0000,   // Used for unsupported and unknown values
  _8BIT   = 0x0001,
  _16BIT  = 0x0002,
  _24BIT  = 0x0004,
  _32BIT  = 0x0008
} BIT_DEPTH;

typedef Array<DDSURFDESC, DDSURFDESC>   SurfDescArray;

// ===========================================================================
// Global function prototypes ================================================
// ===========================================================================

HRESULT WINAPI EnumDisplayModesCallback(PDDSURFDESC pSurfDesc, LPVOID lpContext);

HRESULT CALLBACK EnumD3DDevicesCallback(GUID FAR* lpGuid, LPSTR szDeviceDesc,
                                        LPSTR szDeviceName,
                                        LPD3DDEVICEDESC lpD3DHWDeviceDesc,
                                        LPD3DDEVICEDESC lpD3DHELDeviceDesc,
                                        LPVOID lpContext);


// ===========================================================================
// Forward declarations ======================================================
// ===========================================================================

class D3DDevice;


// ===========================================================================
// DDrawDriver class =========================================================
// ===========================================================================
class DDrawDriver
{
  // Friend functions ========================================================
  friend HRESULT WINAPI EnumDisplayModesCallback(PDDSURFDESC pSurfDesc,
                                                 LPVOID lpContext);

  friend HRESULT CALLBACK EnumD3DDevicesCallback(GUID FAR* lpGuid,
                                                 LPSTR szDeviceDesc,
                                                 LPSTR szDeviceName,                  
                                                 LPD3DDEVICEDESC lpD3DHWDeviceDesc,
                                                 LPD3DDEVICEDESC lpD3DHELDeviceDesc,
                                                 LPVOID lpContext);


  // Member variables ========================================================
  private:
    LPGUID            m_pGUID;        // The GUID representing this driver
                                      //  (Will be NULL if using the primary
                                      //  DirectDraw driver)

    PDDRAW            m_pDDraw;       // Pointer to an IDirectDraw4 interface
    PD3D              m_pD3D;         // Pointer to an IDirect3D3 interface

    DWORD             m_dwMemory;     // The amount of memory available on
                                      //  this video device

    HWND              m_hWnd;         // Window handle to attach to the
                                      // DirectDraw device

    // Name and description of the DDraw device
    char              m_szName[MAX_DDDEVICEID_STRING]; 
    char              m_szDescription[MAX_DDDEVICEID_STRING];

    DDCAPS            m_HardwareCaps;       // Hardware capabilities
    DDCAPS            m_SoftwareCaps;       // Software capabilities

    DDSURFDESC*       m_apDisplayModes[MAX_DISPLAYMODES]; // Video modes
    D3DDevice*        m_ap3DDevices[MAX_3DDEVICES];       // 3D Devices

    int               m_nNumDisplayModes;   // Number of display modes
    int               m_nNum3DDevices;      // Number of 3D devices

    LPGUID            m_pSearchGUID;        // GUID used for searches


  // Construction / Destruction ==============================================
  public:
    DDrawDriver()             { Clear();                                    }

    DDrawDriver(HWND hWnd, LPSTR szDriverName, LPSTR szDriverDesc,
                LPGUID pGUID = NULL)
    { Clear(); VERIFY(Initialize(hWnd, szDriverName, szDriverDesc, pGUID)); }

    ~DDrawDriver()            { Destroy();                                  }


  // Private member functions ================================================
  private:
    void Destroy();
    void DestroyEnumerations();

    void Clear()            { ZeroMemory(this, sizeof(DDrawDriver));        }


  // Protected member functions ==============================================
  protected:
    BOOL    CreateDDraw(LPGUID pGUID = NULL);

    BOOL    AddDisplayMode(const DDSURFDESC& ddSurfDesc);
    BOOL    Add3DDevice(D3DDevice* pNew3DDevice);

    BOOL    DetermineDriverMemory();


  // Public member functions =================================================
  public:
    BOOL    Initialize(HWND hWnd, LPSTR szDriverName, LPSTR szDriverDesc,
                       LPGUID pGUID = NULL);

    BOOL    WriteINIInfo(LPCSTR szSection, int& n3DDeviceCount);

    BOOL    EnumerateDisplayModes();
    BOOL    Enumerate3DDevices();

    D3DDevice*  EnumerateFor3DDevice(LPGUID pGUID);

    void    ReleaseD3DDevices();

    BOOL    FindPixelFormat(DWORD dwColourDepth, DDPIXELFORMAT& ddPixelFormat);
    BOOL    SupportsDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP);

    PDDRAW  GetDDraw() const  { ASSERT(NULL != m_pDDraw); return(m_pDDraw); }
    PD3D    GetD3D()   const  { ASSERT(NULL != m_pD3D);   return(m_pD3D);   }

    LPGUID  GetGUID()               const { return(m_pGUID);                }
    LPSTR   GetName()               const { return((LPSTR)m_szName);        }
    LPSTR   GetDescription()        const { return((LPSTR)m_szDescription); }
    const DDCAPS* GetHardwareCaps() const { return(&m_HardwareCaps);        }
    const DDCAPS* GetSoftwareCaps() const { return(&m_SoftwareCaps);        }
    int     GetNumDisplayModes()    const { return(m_nNumDisplayModes);     }
    int     GetNumD3DDevices()      const { return(m_nNum3DDevices);        }

    BOOL    SearchingFor3DDevice()  const { return(NULL != m_pSearchGUID);  }
    LPGUID  SearchGUID()            const { return(m_pSearchGUID);          }
    DWORD   GetDriverMemory()       const { return(m_dwMemory);             }

    void    GetDisplayModeList(SurfDescArray& aDisplayModes,
                               DWORD dwMinWidth = 0, DWORD dwMaxWidth = 0,
                               DWORD dwMinHeight = 0xFFFFFFFF,
                               DWORD dwMaxHeight = 0xFFFFFFFF,
                               DWORD dwSupportedBitDepths = 0x0000000F);

    INL BOOL          GUIDMatches(LPGUID pGUID) const;

    INL PDDSURFDESC   GetDisplayMode(int nIdx) const;
    INL D3DDevice*    GetD3DDevice(int nIdx) const;
    INL D3DDevice*    RemoveD3DDevice(int nIdx);

    INL BOOL          HasHardwareBlitter() const;

    // DirectDraw creation routines
    PDDSURFACE  CreateSurface(DDSURFDESC& ddSurfDesc);
    PDDSURFACE  CreateSurface(UINT nWidth, UINT nHeight, DWORD dwCaps1,
                              DWORD dwCaps2 = 0, DWORD dwFlags = 0);

    PDDPALETTE  CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpPalEntries);
    PDDCLIPPER  CreateClipper(DWORD dwFlags);

    // Direct3D creation routines
    PD3DDEVICE    CreateD3DDevice(GUID& guid, PDDSURFACE pRenderTarget);
    PLIGHT        CreateLight();
    PMATERIAL     CreateMaterial();
    PVIEWPORT     CreateViewport();
    PD3DVTXBUFFER CreateVertexBuffer(PD3DVTXBUFFERDESC lpVBDesc, DWORD dwFlags);


  // Inline DirectDraw wrapper functions =====================================
  public:
    INL BOOL    GetDeviceID(DDDEVICEIDENTIFIER& ddDeviceInfo);
    INL BOOL    GetDisplayMode(DDSURFDESC& ddSurfDesc);
        BOOL    SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP);

    INL BOOL    GetAvailableVidMem(DDSURFCAPS& SurfCaps, DWORD& dwTotal, 
                                   DWORD& dwFree);
    INL BOOL    FlipToGDISurface();
    INL BOOL    SetCooperativeLevel(DWORD dwFlags);
    INL BOOL    RestoreDisplayMode();
    INL BOOL    RestoreAllSurfaces();

    INL PDDSURFACE GetSurfaceFromDC(HDC hdc);


  // Inline Direct3D wrapper functions =======================================
  public:
    INL BOOL    EvictManagedTextures();

};

#include "DDrawDrv.inl"

#endif // INC_DDRAWDRV
// ===========================================================================
