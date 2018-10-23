// ===========================================================================
// Module:      D3DDevice.h
//
// Description: Direct3D device class definition.  Represents a Direct3D
//              device and its capabilities.
//
// Created:     April 14, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================

#ifndef INC_D3DDEVICE
#define INC_D3DDEVICE


// ===========================================================================
// Typdefs and defines =======================================================
// ===========================================================================

#ifdef _DEBUG
  extern  LONGLONG g_llNumTriangles;
  extern  LONGLONG g_llStartTime;

  #define ADDTRIANGLES(type, vtxCount)                                      \
    switch (type)                                                           \
    {                                                                       \
      case D3DPT_TRIANGLELIST:  g_llNumTriangles += vtxCount / 3; break;    \
      case D3DPT_TRIANGLESTRIP:                                             \
      case D3DPT_TRIANGLEFAN:   g_llNumTriangles += vtxCount - 2; break;    \
    }

#endif // _DEBUG

typedef Array<DDPIXELFORMAT*, DDPIXELFORMAT*>   PixelFormatPtrArray;

extern const int g_kn3DDeviceSize;

// ===========================================================================
// Global function prototypes ================================================
// ===========================================================================

HRESULT CALLBACK EnumZBufferFormatsCallback(LPDDPIXELFORMAT lpDDPixFmt,  
                                            LPVOID lpContext);

HRESULT CALLBACK EnumTextureFormatsCallback(LPDDPIXELFORMAT lpDDPixFmt,  
                                            LPVOID lpContext);

int CompareTextureFormats(const void* pArg1, const void* pArg2);


// ===========================================================================
// Forward declarations ======================================================
// ===========================================================================

class DDrawDriver;
class Surface;


// ===========================================================================
// D3DDevice class ===========================================================
// ===========================================================================
class D3DDevice
{
  // Friend functions ========================================================
  friend HRESULT CALLBACK EnumZBufferFormatsCallback(LPDDPIXELFORMAT lpDDPixFmt,  
                                                     LPVOID lpContext);

  friend HRESULT CALLBACK EnumTextureFormatsCallback(LPDDPIXELFORMAT lpDDPixFmt,  
                                                     LPVOID lpContext);


  // Member variables ========================================================
  private:
    DDrawDriver*      m_pParent;      // The parent DDraw driver
    PD3DDEVICE        m_pD3DDevice;   // Pointer to a D3D device
    GUID              m_GUID;         // The GUID of this 3D device

    // Name and description of the 3D device
    char              m_szName[MAX_DDDEVICEID_STRING];
    char              m_szDescription[MAX_DDDEVICEID_STRING];

    Surface*          m_pRenderTarget;// Rendering target of the device

    D3DDEVICEDESC     m_HardwareDesc; // Hardware description of the 3D device
    D3DDEVICEDESC     m_SoftwareDesc; // Software description of the 3D device

    BOOL              m_bModeChanged; // Indicates we had to change the
                                      //  display mode to create the device

    PixelFormatPtrArray  m_apZBufFormats;  // Z Buffer formats
    PixelFormatPtrArray  m_apTextrFormats; // Texture formats


  // Construction / Destruction ==============================================
  public:
    D3DDevice()             { Clear();                                      }
    D3DDevice(DDrawDriver* pParent, GUID& guid, LPSTR szDeviceName,
              LPSTR szDeviceDesc, D3DDEVICEDESC* pHardwareDesc,
              D3DDEVICEDESC* pSoftwareDesc)
    {
      Clear();
      VERIFY(Initialize(pParent, guid, szDeviceName, szDeviceDesc,
                        pHardwareDesc, pSoftwareDesc));
    }

    ~D3DDevice();


  // Private member functions ================================================
  private:
    void    Destroy();
    void    Clear();


  // Protected member functions ==============================================
  protected:
    BOOL    CreateD3DDevice(GUID& guid);
    BOOL    CreateRenderTarget();

    BOOL    AddZBufFormat(DDPIXELFORMAT* pZBufFormat);
    BOOL    AddTextrFormat(DDPIXELFORMAT* pTextrFormat);


  // Public member functions =================================================
  public:
    BOOL    Initialize(DDrawDriver* pParent, GUID& guid, LPSTR szDeviceName,
                       LPSTR szDeviceDesc, D3DDEVICEDESC* pHardwareDesc,
                       D3DDEVICEDESC* pSoftwareDesc);

    BOOL    InitD3D(Surface* pRenderTarget);

    void    SetInitialRenderingStates();

    BOOL    ColourDepthSupported(DWORD dwColourDepth);
    BOOL    WriteDeviceCapsToINI(LPCTSTR szSection);

    BOOL    IsHardware() const { return(0 != m_HardwareDesc.dcmColorModel); }

    PD3DDEVICE GetD3DDevice() const     { return(m_pD3DDevice);             }
    LPSTR   GetName()         const     { return((LPSTR)m_szName);          }
    LPSTR   GetDescription()  const     { return((LPSTR)m_szDescription);   }
    LPGUID  GetGUID()                   { return(&m_GUID);                  }

    D3DDEVICEDESC* GetCaps()
    { return(IsHardware() ? &m_HardwareDesc : &m_SoftwareDesc);             }

    INL BOOL  GUIDMatches(GUID& guid) const;
    INL void  ReleaseD3DDevice();

    int   GetNumZBufferFormats() const  { return(m_apZBufFormats.GetSize());}
    int   GetNumTextureFormats() const  { return(m_apTextrFormats.GetSize());}

    INL DDPIXELFORMAT*  GetZBufferFormat(int nIdx) const;
    INL DDPIXELFORMAT*  GetTextureFormat(int nIdx) const;

    BOOL    TextureFormatSupported(const DDPIXELFORMAT& ddpf);


  // IDirect3DDevice function wrappers =======================================
  public:
    INL BOOL    AddViewport(PVIEWPORT pViewport);
    INL BOOL    BeginScene();
    INL BOOL    RemoveViewport(PVIEWPORT pViewport);

    INL BOOL    DrawIndexedPrimitive(D3DPRIMITIVETYPE primType, 
                                     DWORD dwVertexType, LPVOID lpvVertices,
                                     DWORD dwVertexCount, LPWORD lpwIndices,
                                     DWORD dwIndexCount, DWORD dwFlags);

    INL BOOL    DrawIndexedPrimitiveStrided(D3DPRIMITIVETYPE primType,  
                                            DWORD dwVertexType, 
                                            PD3DSTRIDEDATA pVertexArray, 
                                            DWORD dwVertexCount, 
                                            LPWORD lpwIndices,
                                            DWORD dwIndexCount, DWORD dwFlags);

    INL BOOL    DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE primType,
                                       PD3DVTXBUFFER pVertexBuffer,
                                       LPWORD lpwIndices, DWORD dwIndexCount,
                                       DWORD dwFlags);

    INL BOOL    DrawPrimitive(D3DPRIMITIVETYPE primType, DWORD dwVertexType,
                              LPVOID lpvVertices, DWORD dwVertexCount,
                              DWORD dwFlags);

    INL BOOL    DrawPrimitiveStrided(D3DPRIMITIVETYPE primType,
                                     DWORD dwVertexType,
                                     PD3DSTRIDEDATA pVertexArray,  
                                     DWORD  dwVertexCount, DWORD dwFlags);

    INL BOOL    DrawPrimitiveVB(D3DPRIMITIVETYPE primType,
                                PD3DVTXBUFFER pVertexBuffer,
                                DWORD dwStartVertex, DWORD dwNumVertices,
                                DWORD dwFlags);

    INL BOOL    EndScene();
    INL BOOL    GetClipStatus(D3DCLIPSTATUS& d3dClipStatus); 

    INL PVIEWPORT   GetCurrentViewport();
    INL PVIEWPORT   NextViewport(PVIEWPORT pViewport,  DWORD dwFlags);
    INL BOOL        SetCurrentViewport(PVIEWPORT pViewport);
    INL PDDSURFACE  GetRenderTarget();
    INL PD3DTEXTURE GetTexture(DWORD dwStage);

    INL DWORD   GetLightState(D3DLIGHTSTATETYPE eLightStateType);
    INL DWORD   GetRenderState(D3DRENDERSTATETYPE eRenderStateType);

    INL DWORD   GetTextureStageState(DWORD dwStage, 
                                     D3DTEXTURESTAGESTATETYPE eState);

    INL BOOL    GetTransform(D3DTRANSFORMSTATETYPE eTransformStateType, 
                             D3DMATRIX& d3dMatrix);

    INL BOOL    MultiplyTransform(D3DTRANSFORMSTATETYPE eTransformStateType,
                                  LPD3DMATRIX pD3DMatrix);

    INL BOOL    SetClipStatus(LPD3DCLIPSTATUS pD3DClipStatus);

    INL BOOL    SetLightState(D3DLIGHTSTATETYPE eLightStateType, 
                              DWORD dwLightState);
    INL BOOL    SetRenderState(D3DRENDERSTATETYPE eRenderStateType, 
                               DWORD dwRenderState);
    INL BOOL    SetRenderTarget(PDDSURFACE pRenderTarget);
    INL BOOL    SetTexture(DWORD dwStage, PD3DTEXTURE pTexture);
    INL BOOL    SetTextureStageState(DWORD dwStage,
                                     D3DTEXTURESTAGESTATETYPE eState,
                                     DWORD dwValue);
    INL BOOL    SetTransform(D3DTRANSFORMSTATETYPE eTransformStateType,  
                             LPD3DMATRIX pD3DMatrix);
};

// Include our inline functions
#include "D3DDevice.inl"

#endif // INC_D3DDEVICE
// ===========================================================================
