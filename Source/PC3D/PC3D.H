// //////////////////////////////////////////////////
// Copyright (C) 1997, Artech,        
// All rights reserved.          
//                 
// This document contains intellectual property  
// of Artech.  Any copying or reproduction    
// without prior written permission is prohibited. 
//                 
// //////////////////////////////////////////////////
// MODULE: pc3d.h              
// Contents: API level 3d object type definitions
// ///////////////// END OF HEADER //////////////////
#ifndef PC3D_H
#define PC3D_H

#include "DDrawWnd.h"
#include "DDrawDrv.h"
#include "D3DDevice.h"

// .......... option to test for software speed increase when palettes are identical
#define FORCE_MAIN_SURFACE_PALETTE


// .......... PC 3D API code philosophy
/*
The code is intented to provide a high level interface similar
to something like Direct3D Retained mode or OpenGL's OpenInventor. hopefully,
this will shield content developers from system dependencies like: "what's
a D3DRENDERSTATE_BLENDENABLE execute buffer flag ?" (ugh!). It will also make
a port to OpenGL or any other 3D system (hopefully) easier.

  We avoid using Direct3D retained mode because of this, as well as because
we can't customize its behaviour internally when we want to; very important in creating
a custom effect or controlling how the system is managing low level 3D resources.
  
  At the same time, the system does not try to hide some data types like matrices
and vectors. Since Direct3D is the main API for the PC platform, we do not want to
compromise speed doing things like copying an array of vectors from one format to another,
or copying matrix elements before setting up a corresponding matrix in the driver. 
For example, if D3D's vector transformation API is used to support hardware, vertices
would have to be copied over to the D3DVERTEX format, eating up precious memory bandwidth.
Thus, the library retains the Direct3D equivalents for such data types.
*/


// types used for elemental 3D values 
typedef float     coord;
typedef float     angle;
typedef D3DVERTEX vertex;
typedef D3DVECTOR vector;
typedef vector    normal;
typedef D3DCOLOR  rgbcolor;


// ===========================================================================
// External C function prototypes ============================================
// ===========================================================================
extern "C" {

BOOL        DDRAW_Init(HWND hWnd, UINT width, UINT height, UINT bpp,
                       BOOL bUseZBuffer, BOOL bMultitasking = FALSE,
                       WindowedMode nWindowMode = WINDOWED_USE_DEFAULT);

BOOL        DDRAW_GetVidMemStats(DWORD *total, DWORD *free, DWORD *totalTex,
                                 DWORD *freeTex, PDDSURFCAPS pExtraCaps = NULL);

BOOL        DDRAW_Cleanup();
void        DDRAW_ClearMainSurfaces();
void        DDRAW_GetRenderWindowRect(RECT& rect);
PDDRAW      DDRAW_GetDirectDraw();
PDDSURFACE  DDRAW_GetRenderSurface();
PDDSURFACE  DDRAW_GetPrimarySurface();
PD3DDEVICE  DDRAW_GetD3DDevice();
BOOL        DDRAW_SetMode(BOOL bWindowed, UINT nWidth, UINT nHeight,
                          UINT nBPP = 0, BOOL bUseZBuffer = TRUE);

BOOL        DDRAW_SetINIMode(UINT nWidth, UINT nHeight, UINT nBPP);

LRESULT CALLBACK DDRAW_WindowProc(HWND hWindow, UINT uiMessage, WPARAM wParam,
                                  LPARAM lParam);

} // extern "C"


// ===========================================================================
// Structures ================================================================
// ===========================================================================

struct Box
{
  float minx, miny, minz;
  float maxx, maxy, maxz;

  Box()  { minx = miny = minz = maxx = maxy = maxz = 0.0f; }
  Box(const D3DVECTOR& pt1, const D3DVECTOR& pt2)  { SetBox(pt1, pt2);      }

  // returns the 8 points of the box (array must be at least 8 vertices in length)
  void getPts(vector *pts);
  void SetBox(const D3DVECTOR& pt1, const D3DVECTOR& pt2)
  { 
    minx = pt1.x; miny = pt1.y; minz = pt1.z;
    maxx = pt2.x; maxy = pt2.y; maxz = pt2.z; 
  }

  Box& operator =(const Box& box) 
  { CopyMemory(this, &box, sizeof(Box)); return(*this);                     }

};

// ===========================================================================
INL void Box::getPts(vector *pts) 
{
  pts[0].x = minx; pts[0].y = miny; pts[0].z = minz;
  pts[1].x = minx; pts[1].y = miny; pts[1].z = maxz;
  pts[2].x = minx; pts[2].y = maxy; pts[2].z = minz;
  pts[3].x = minx; pts[3].y = maxy; pts[3].z = maxz;
  pts[4].x = maxx; pts[4].y = miny; pts[4].z = minz;
  pts[5].x = maxx; pts[5].y = miny; pts[5].z = maxz;
  pts[6].x = maxx; pts[6].y = maxy; pts[6].z = minz;
  pts[7].x = maxx; pts[7].y = maxy; pts[7].z = maxz; 
}

struct TextureFilterOptions
{
  BYTE  byMinFilter;
  BYTE  byMagFilter;
  BYTE  byMipMapFilter;
};

// ===========================================================================


// basic "system" object. here can be only one, and its job is to initialize the
// system, manage pertinent interactions with the Windows messaging architecture,
// and destroy resources created upon initialization.
class pc3D
{
    friend BOOL DDRAW_setMode(BOOL bWindowed, UINT nWidth, UINT nHeight,
                              UINT nBPP, BOOL bUseZBuffer);

    pc3D() {} // prevent object from being created, everything is static
  
  
  public:
    static DDrawWnd*              m_pMainWnd;
    static TextureFilterOptions   m_TxtrFilters;
  
    // Windows message handler
    static LRESULT CALLBACK WindowProc(HWND hWindow, UINT uiMessage,
                                       WPARAM wParam, LPARAM lParam);
  
    static BOOL   Init(HWND hWnd, UINT nWidth, UINT nHeight, UINT nBpp,
                       BOOL bUseZBuffer = TRUE, BOOL bMultitasking = FALSE,
                       WindowedMode nWindowedMode = WINDOWED_USE_DEFAULT);

    // Provides the total & free amount of video and video texture memory.
    // Note that some cards do not distinguish between texture and regular
    // video RAM, and some do. So, allocating surfaces from on type may
    // affect the other. NULL fields are not initialized. 
    // Does a DirectDraw2::GetAvailableVidMem call internally, and
    // if extraCaps is not NULL, will OR the bits in 'extraCaps' with 
    // the default ones before querying the driver, so you can 
    // add any additional flags if desired. 
    INL static BOOL GetVidMemStats(DWORD& dwTotal, DWORD& dwFree,
                                   DWORD& dwTotalTex, DWORD& dwFreeTex,
                                   PDDSURFCAPS pExtraCaps = NULL);

    static BOOL   Cleanup();
  
    static DWORD GetRenderSurfaceBitDepth();

    INL static Surface* GetPrimarySurface();
    INL static Surface* GetRenderSurface();

    static void   GetDisplaySize(DWORD& dwX, DWORD& dwY);

    INL static BOOL   FlipDisplay();

    INL static BOOL   Using3DHardware();
  
    INL static BOOL   IsWindowed();
    INL static BOOL   IsInitialized();
    INL static BOOL   IsActive();
    INL static BOOL   IsPaused();

    INL static void   Pause(BOOL bPause);    

    INL static DDrawDriver* GetDDrawDriver();
    INL static D3DDevice*   Get3DDevice();
    INL static HWND         GetHWnd();
  
    // 3D Rendering states ===================================================
    INL static BOOL SetShadeMode(BYTE byMode);
    INL static BOOL SetFillMode(BYTE byMode);

    INL static BOOL EnableDithering(BOOL bEnable);
    INL static BOOL EnableSpecularHighLights(BOOL bEnable);
    INL static BOOL EnablePerspectiveCorrection(BOOL bEnable);
    INL static BOOL EnableBilinearFiltering(DWORD dwStage, BOOL bEnable);
    INL static BOOL EnableAlphaBlending(BOOL bEnable);
    INL static BOOL EnableAntiAliasing(BOOL bEnable);
    INL static BOOL EnableFog(BOOL bEnable);
    INL static BOOL EnableColourKeying(BOOL bEnable);
    INL static BOOL EnableMipMapFiltering(DWORD dwStage, BOOL bEnable);
    INL static BOOL EnableTrilinearMipMapFiltering(DWORD dwStage, BOOL bEnable);

};


// ===========================================================================
// INLINE: pc3D member functions =============================================
// ===========================================================================

// ===========================================================================
INL Surface* pc3D::GetPrimarySurface()
{
  ASSERT(NULL != pc3D::m_pMainWnd);
  return(pc3D::m_pMainWnd->GetPrimarySurface());
}

// ===========================================================================
INL Surface* pc3D::GetRenderSurface()
{
  ASSERT(NULL != pc3D::m_pMainWnd);
  return(pc3D::m_pMainWnd->GetRenderSurface());
}

// ===========================================================================
INL DDrawDriver* pc3D::GetDDrawDriver()
{
  ASSERT(NULL != pc3D::m_pMainWnd);
  return(pc3D::m_pMainWnd->GetDDrawDriver());
}

// ===========================================================================
INL D3DDevice* pc3D::Get3DDevice()
{
  ASSERT(NULL != pc3D::m_pMainWnd);
  return(pc3D::m_pMainWnd->Get3DDevice());
}

// ===========================================================================
INL BOOL pc3D::IsWindowed()
{
  if (m_pMainWnd) return(m_pMainWnd->IsWindowed());
  return(TRUE);
}

// ===========================================================================
INL BOOL pc3D::IsInitialized()
{
  ASSERT(NULL != pc3D::m_pMainWnd); if (!pc3D::m_pMainWnd) return(FALSE);
  return(pc3D::m_pMainWnd->IsInitialized());
}

// ===========================================================================
INL BOOL pc3D::IsActive()
{
  ASSERT(NULL != pc3D::m_pMainWnd); if (!pc3D::m_pMainWnd) return(FALSE);
  return(pc3D::m_pMainWnd->IsActive());
}

// ===========================================================================
INL BOOL pc3D::IsPaused()
{
  ASSERT(NULL != pc3D::m_pMainWnd);
  return(pc3D::m_pMainWnd->IsPaused());
}

// ===========================================================================
INL void pc3D::Pause(BOOL bPause)
{
  ASSERT(NULL != pc3D::m_pMainWnd);
  pc3D::m_pMainWnd->Pause(bPause);
}

// ===========================================================================
INL BOOL pc3D::Using3DHardware()
{
  ASSERT(NULL != m_pMainWnd);
  return(m_pMainWnd->Get3DDevice()->IsHardware());
}

// ===========================================================================
INL BOOL pc3D::FlipDisplay()
{
  ASSERT(NULL != m_pMainWnd);
  return(m_pMainWnd->FlipDisplay());
}

// ===========================================================================
INL BOOL pc3D::GetVidMemStats(DWORD& dwTotal, DWORD& dwFree, DWORD& dwTotalTex,
                              DWORD& dwFreeTex, PDDSURFCAPS pExtraCaps /*= NULL*/)
{
  ASSERT(NULL != m_pMainWnd);
  return(m_pMainWnd->GetAvailableVidMem(dwTotal, dwFree, dwTotalTex, dwFreeTex,
                                        pExtraCaps));
}

// ===========================================================================
INL HWND pc3D::GetHWnd()
{
  ASSERT(NULL != m_pMainWnd);
  return(pc3D::m_pMainWnd->GetMainWindow());
}

#include "RenderSettings.inl"
  
#endif /* end of file */
// ===========================================================================
