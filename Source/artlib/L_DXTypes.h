// ===========================================================================
// Module:      DXTypes.h
//
// Description: Contains DirectX type definitions.
//
// Created:     April 14, 1999
//
// Author:      Dave Wilson
//
// Copywrite:		Artech, 1999
// ===========================================================================


#ifndef INC_DXTYPES
#define INC_DXTYPES

// ===========================================================================
// DirectDraw types ==========================================================
// ===========================================================================

#define DDRAW1_GUID                     IID_IDirectDraw
#define DDRAW2_GUID                     IID_IDirectDraw2
#define DDRAW4_GUID                     IID_IDirectDraw4
#define CURRENT_DDRAW_GUID              IID_IDirectDraw4

typedef struct IDirectDraw              DDRAW1;
typedef struct IDirectDraw2             DDRAW2;
typedef struct IDirectDraw4             DDRAW4;
typedef struct IDirectDraw4             DDRAW;    //  Most current version

typedef LPDIRECTDRAW                    PDDRAW1;
typedef LPDIRECTDRAW2                   PDDRAW2;
typedef LPDIRECTDRAW4                   PDDRAW4;
typedef LPDIRECTDRAW4                   PDDRAW;   // Most current version

// Surfaces ==================================================================
typedef struct IDirectDrawSurface       DDSURFACE1;
typedef struct IDirectDrawSurface2      DDSURFACE2;
typedef struct IDirectDrawSurface3      DDSURFACE3;
typedef struct IDirectDrawSurface4      DDSURFACE4;
typedef struct IDirectDrawSurface4      DDSURFACE;  // Most current version

typedef LPDIRECTDRAWSURFACE             PDDSURFACE1;
typedef LPDIRECTDRAWSURFACE2            PDDSURFACE2;
typedef LPDIRECTDRAWSURFACE3            PDDSURFACE3;
typedef LPDIRECTDRAWSURFACE4            PDDSURFACE4;
typedef LPDIRECTDRAWSURFACE4            PDDSURFACE; // Mose current version

typedef DDSURFACEDESC                   DDSURFDESC1;
typedef DDSURFACEDESC2                  DDSURFDESC2;
typedef DDSURFACEDESC2                  DDSURFDESC; // Most current version

typedef LPDDSURFACEDESC                 PDDSURFDESC1;
typedef LPDDSURFACEDESC2                PDDSURFDESC2;
typedef LPDDSURFACEDESC2                PDDSURFDESC;  // Most current version

typedef struct _DDSCAPS                 DDSURFCAPS1;
typedef struct _DDSCAPS2                DDSURFCAPS2;
typedef struct _DDSCAPS2                DDSURFCAPS; // Most current version

typedef LPDDSCAPS                       PDDSURFCAPS1;
typedef LPDDSCAPS2                      PDDSURFCAPS2;
typedef LPDDSCAPS2                      PDDSURFCAPS;  // Most current version

// Palettes ==================================================================
typedef struct IDirectDrawPalette       DDPALETTE1;
typedef struct IDirectDrawPalette       DDPALETTE;

typedef LPDIRECTDRAWPALETTE             PDDPALETTE1;
typedef LPDIRECTDRAWPALETTE             PDDPALETTE; // Most current version

// Clippers ==================================================================
typedef struct IDirectDrawClipper       DDCLIPPER1;
typedef struct IDirectDrawClipper       DDCLIPPER;

typedef LPDIRECTDRAWCLIPPER             PDDCLIPPER1;
typedef LPDIRECTDRAWCLIPPER             PDDCLIPPER; // Most current version


// ===========================================================================
// Direct3D types ============================================================
// ===========================================================================

#define D3D1_GUID                       IID_IDirect3D
#define D3D2_GUID                       IID_IDirect3D2
#define D3D3_GUID                       IID_IDirect3D3
#define CURRENT_D3D_GUID                IID_IDirect3D3

typedef struct IDirect3D                D3D1;
typedef struct IDirect3D2               D3D2;
typedef struct IDirect3D3               D3D3;
typedef struct IDirect3D3               D3D;    // Most current version

typedef LPDIRECT3D                      PD3D1;
typedef LPDIRECT3D2                     PD3D2;
typedef LPDIRECT3D3                     PD3D3;
typedef LPDIRECT3D3                     PD3D;   // Most current version

// Devices ===================================================================
typedef struct IDirect3DDevice          D3DDEVICE1;
typedef struct IDirect3DDevice2         D3DDEVICE2;
typedef struct IDirect3DDevice3         D3DDEVICE3;
typedef struct IDirect3DDevice3         D3DDEVICE;  // Most current version

typedef LPDIRECT3DDEVICE                PD3DDEVICE1;
typedef LPDIRECT3DDEVICE2               PD3DDEVICE2;
typedef LPDIRECT3DDEVICE3               PD3DDEVICE3;
typedef LPDIRECT3DDEVICE3               PD3DDEVICE; // Most current version

// Textures ==================================================================
#define D3DTEXTURE1_GUID                IID_IDirect3DTexture
#define D3DTEXTURE2_GUID                IID_IDirect3DTexture2
#define CURRENT_D3DTEXTURE_GUID         IID_IDirect3DTexture2

typedef struct IDirect3DTexture         D3DTEXTURE1;
typedef struct IDirect3DTexture2        D3DTEXTURE2;
typedef struct IDirect3DTexture2        D3DTEXTURE; // Most current version

typedef LPDIRECT3DTEXTURE               PD3DTEXTURE1;
typedef LPDIRECT3DTEXTURE2              PD3DTEXTURE2;
typedef LPDIRECT3DTEXTURE2              PD3DTEXTURE;  // Most current version

typedef D3DTEXTUREHANDLE                HTEXTURE;

// Viewports =================================================================
typedef struct IDirect3DViewport        VIEWPORT1;
typedef struct IDirect3DViewport2       VIEWPORT2;
typedef struct IDirect3DViewport3       VIEWPORT3;
typedef struct IDirect3DViewport3       VIEWPORT;  // Most current version

typedef LPDIRECT3DVIEWPORT              PVIEWPORT1;
typedef LPDIRECT3DVIEWPORT2             PVIEWPORT2;
typedef LPDIRECT3DVIEWPORT3             PVIEWPORT3;
typedef LPDIRECT3DVIEWPORT3             PVIEWPORT;  // Most current version

typedef struct _D3DVIEWPORT             VIEWPORTDATA1;
typedef struct _D3DVIEWPORT2            VIEWPORTDATA2;
typedef struct _D3DVIEWPORT2            VIEWPORTDATA; // Most current version

typedef LPD3DVIEWPORT                   PVIEWPORTDATA1;
typedef LPD3DVIEWPORT2                  PVIEWPORTDATA2;
typedef LPD3DVIEWPORT2                  PVIEWPORTDATA; // Most current version

// Materials =================================================================
typedef struct IDirect3DMaterial        MATERIAL1;
typedef struct IDirect3DMaterial2       MATERIAL2;
typedef struct IDirect3DMaterial3       MATERIAL3;
typedef struct IDirect3DMaterial3       MATERIAL;     // Most current version

typedef LPDIRECT3DMATERIAL              PMATERIAL1;
typedef LPDIRECT3DMATERIAL2             PMATERIAL2;
typedef LPDIRECT3DMATERIAL3             PMATERIAL3;
typedef LPDIRECT3DMATERIAL3             PMATERIAL;  // Most current version

typedef struct _D3DMATERIAL             MATERIALDATA1;
typedef struct _D3DMATERIAL             MATERIALDATA;

typedef LPD3DMATERIAL                   PMATERIALDATA1;
typedef LPD3DMATERIAL                   PMATERIALDATA;

typedef D3DMATERIALHANDLE               HMATERIAL;

// Lights ====================================================================
typedef struct IDirect3DLight           LIGHT1;
typedef struct IDirect3DLight           LIGHT;

typedef LPDIRECT3DLIGHT                 PLIGHT1;
typedef LPDIRECT3DLIGHT                 PLIGHT;

typedef struct _D3DLIGHT                LIGHTDATA1;
typedef struct _D3DLIGHT2               LIGHTDATA2;
typedef struct _D3DLIGHT2               LIGHTDATA;  // Most current version

typedef LPD3DLIGHT                      PLIGHTDATA1;
typedef LPD3DLIGHT2                     PLIGHTDATA2;
typedef LPD3DLIGHT2                     PLIGHTDATA; // Most current version

// Vertex buffers ============================================================
typedef struct IDirect3DVertexBuffer    D3DVTXBUFFER1;
typedef struct IDirect3DVertexBuffer    D3DVTXBUFFER;

typedef LPDIRECT3DVERTEXBUFFER          PD3DVTXBUFFER1;
typedef LPDIRECT3DVERTEXBUFFER          PD3DVTXBUFFER;

typedef struct _D3DVERTEXBUFFERDESC     D3DVTXBUFFERDESC1;
typedef struct _D3DVERTEXBUFFERDESC     D3DVTXBUFFERDESC;

typedef LPD3DVERTEXBUFFERDESC           PD3DVTXBUFFERDESC1;
typedef LPD3DVERTEXBUFFERDESC           PD3DVTXBUFFERDESC;

// General stuff =============================================================
typedef D3DDRAWPRIMITIVESTRIDEDDATA     D3DSTRIDEDATA1;
typedef D3DDRAWPRIMITIVESTRIDEDDATA     D3DSTRIDEDATA;

typedef LPD3DDRAWPRIMITIVESTRIDEDDATA   PD3DSTRIDEDATA1;
typedef LPD3DDRAWPRIMITIVESTRIDEDDATA   PD3DSTRIDEDATA;


// ===========================================================================
// Class ColourValue (D3DCOLORVALUE class wrapper) ===========================
// ===========================================================================
typedef class ColourValue : public D3DCOLORVALUE
{
  public:
    ColourValue()   { ZeroMemory(this, sizeof(D3DCOLORVALUE));              }

    ColourValue(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f)
    { r = fRed; g = fGreen; b = fBlue; a = fAlpha;                          }
    
    ColourValue(const D3DCOLORVALUE& clrValue)
    { CopyMemory(this, &clrValue, sizeof(D3DCOLORVALUE));                   }

    D3DCOLORVALUE operator =(const D3DCOLORVALUE& clrValue)
    { 
      CopyMemory(this, &clrValue, sizeof(D3DCOLORVALUE));
      return(*this);
    }

    BOOL operator ==(const D3DCOLORVALUE& clrValue) const
    { return(0 == memcmp(this, &clrValue, sizeof(D3DCOLORVALUE)));          }

} CLRVALUE;

// Matrix class ==============================================================
#if !CE_ARTLIB_EnableSystemGrafix2D || CE_ARTLIB_EnableSystemGrafix3D
// Not included if only compiling for ArtLib 2D.  Included if compiling
// for ArtLib 3D or non-Artlib things like PC3D.
#include "..\PC3D\Matrix.h"
#endif


// ===========================================================================
// DirectSound types =========================================================
// ===========================================================================

typedef struct IDirectSound             DSOUND1;
typedef struct IDirectSound             DSOUND;
typedef LPDIRECTSOUND                   PDSOUND1;
typedef LPDIRECTSOUND                   PDSOUND;

typedef struct IDirectSoundBuffer       DSBUFFER1;
typedef struct IDirectSoundBuffer       DSBUFFER;
typedef LPDIRECTSOUNDBUFFER             PDSBUFFER1;
typedef LPDIRECTSOUNDBUFFER             PDSBUFFER;

typedef struct IDirectSound3DListener   DSLISTENER1;
typedef struct IDirectSound3DListener   DSLISTENER;
typedef LPDIRECTSOUND3DLISTENER         PDS3DLISTENER1;
typedef LPDIRECTSOUND3DLISTENER         PDS3DLISTENER;

typedef struct _DS3DLISTENER            DS3DLISTENERDATA1;
typedef struct _DS3DLISTENER            DS3DLISTENERDATA;
typedef LPDS3DLISTENER                  PDS3DLISTENERDATA1;
typedef LPDS3DLISTENER                  PDS3DLISTENERDATA;

typedef struct IDirectSound3DBuffer     DS3DBUF1;
typedef struct IDirectSound3DBuffer     DS3DBUF;
typedef LPDIRECTSOUND3DBUFFER           PDS3DBUFFER1;
typedef LPDIRECTSOUND3DBUFFER           PDS3DBUFFER;

typedef struct _DS3DBUFFER              DS3DBUFFERDATA1;
typedef struct _DS3DBUFFER              DS3DBUFFERDATA;
typedef LPDS3DBUFFER                    PDS3DBUFFERDATA1;
typedef LPDS3DBUFFER                    PDS3DBUFFERDATA;

typedef struct IDirectSoundCapture      DSCAPTURE1;
typedef struct IDirectSoundCapture      DSCAPTURE;
typedef LPDIRECTSOUNDCAPTURE            PDSCAPTURE1;
typedef LPDIRECTSOUNDCAPTURE            PDSCAPTURE;

typedef struct IDirectSoundCaptureBuffer  DSCAPTUREBUFFER1;
typedef struct IDirectSoundCaptureBuffer  DSCAPTUREBUFFER;
typedef LPDIRECTSOUNDCAPTUREBUFFER        PDSCAPTUREBUFFER1;
typedef LPDIRECTSOUNDCAPTUREBUFFER        PDSCAPTUREBUFFER;

typedef struct IDirectSoundNotify       DSNOTIFY1;
typedef struct IDirectSoundNotify       DSNOTIFY;
typedef LPDIRECTSOUNDNOTIFY             PDSNOTIFY1;
typedef LPDIRECTSOUNDNOTIFY             PDSNOTIFY;


// Things common to ArtLib and PC3D ==========================================
typedef enum WindowedModeEnum {
  WINDOWED_USE_DEFAULT = 0,
  WINDOWED_USE_WINDOW,
  WINDOWED_USE_FULL_SCREEN
} WindowedMode;


#endif  // INC_DXTYPES
// ===========================================================================
