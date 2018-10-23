// //////////////////////////////////////////////////
// Copyright (C) 1997, Artech,        
// All rights reserved.          
//                 
// This document contains intellectual property  
// of Artech.  Any copying or reproduction    
// without prior written permission is prohibited. 
//                 
// //////////////////////////////////////////////////
// MODULE: funky3d.cpp 
// Contents: main component for system initialization             
//                  
// ///////////////// END OF HEADER //////////////////
#include "PC3DHdr.h"
#include "pc3D.h"
#include "l_Material.h"
#include "GlobalData.h"

extern void GetDXVersion(DWORD& dwDXVersion, DWORD& dwDXPlatform);


// ===========================================================================
// External C functions ======================================================
// ===========================================================================
extern "C" {

// NOTE: Do NOT inline these functions !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// ===========================================================================
BOOL DDRAW_Init(HWND hWnd, UINT width, UINT height, UINT bpp,
                BOOL bUseZBuffer, BOOL bMultitasking /*= FALSE*/,
                WindowedMode nWindowMode /*= WINDOWED_USE_DEFAULT*/)
{
  return(pc3D::Init(hWnd, width, height, bpp, bUseZBuffer,
                    bMultitasking, nWindowMode));
}

// ===========================================================================
BOOL DDRAW_GetVidMemStats(DWORD *total, DWORD *free, DWORD *totalTex,
                          DWORD *freeTex, PDDSURFCAPS pExtraCaps /*= NULL*/)
{
  return pc3D::GetVidMemStats(*total, *free, *totalTex, *freeTex, pExtraCaps);
}

// ===========================================================================
BOOL DDRAW_Cleanup(void)
{
  return pc3D::Cleanup();
}

// ===========================================================================
void DDRAW_ClearMainSurfaces()
{
  ASSERT(NULL != pc3D::m_pMainWnd); if (!pc3D::m_pMainWnd) return;
  (pc3D::m_pMainWnd)->ClearMainSurfaces();
}

// ===========================================================================
void DDRAW_GetRenderWindowRect(RECT& rect)
{
  ASSERT(NULL != pc3D::m_pMainWnd);
  pc3D::m_pMainWnd->GetWindowRect(rect);
}

// ===========================================================================
PDDRAW DDRAW_GetDirectDraw(void)
{
  DDrawDriver* pDDDriver = pc3D::GetDDrawDriver();
  ASSERT(NULL != pDDDriver);
  return(pDDDriver->GetDDraw());
}

// ===========================================================================
PDDSURFACE DDRAW_GetRenderSurface()
{
  ASSERT(NULL != pc3D::m_pMainWnd);
  Surface* pRender = pc3D::m_pMainWnd->GetRenderSurface();
  ASSERT(NULL != pRender);
  return(pRender->GetDDSurface());
}

// ===========================================================================
PDDSURFACE DDRAW_GetPrimarySurface()
{
  ASSERT(NULL != pc3D::m_pMainWnd);
  Surface* pPrimary = pc3D::m_pMainWnd->GetPrimarySurface();
  ASSERT(NULL != pPrimary);
  return(pPrimary->GetDDSurface());
}

// ===========================================================================
PD3DDEVICE DDRAW_GetD3DDevice()
{
  ASSERT(NULL != pc3D::m_pMainWnd);
  D3DDevice* p3DDevice = pc3D::Get3DDevice();
  ASSERT(NULL != p3DDevice);
  return(p3DDevice->GetD3DDevice());
}

// ===========================================================================
BOOL DDRAW_SetMode(BOOL bWindowed, UINT nWidth, UINT nHeight, 
                   UINT nBPP /*= 0*/, BOOL bUseZBuffer /*= TRUE*/)
{
  ASSERT(NULL != pc3D::m_pMainWnd);
  return(pc3D::m_pMainWnd->SetDisplayMode(bWindowed, nWidth, nHeight, nBPP,
                                          bUseZBuffer));
}

// ===========================================================================
BOOL DDRAW_SetINIMode(UINT nWidth, UINT nHeight, UINT nBPP)
{
  ASSERT(NULL != pc3D::m_pMainWnd);
  return(pc3D::m_pMainWnd->SetINIDisplayMode(nWidth, nHeight, nBPP));
}

// ===========================================================================
LRESULT CALLBACK DDRAW_WindowProc(HWND hWindow, UINT uiMessage,
                                  WPARAM wParam, LPARAM lParam)
{
  return pc3D::WindowProc(hWindow, uiMessage, wParam, lParam);
}

} // End extern "C" ==========================================================


// ===========================================================================
// pc3D class ================================================================
// ===========================================================================

DDrawWnd*             pc3D::m_pMainWnd    = NULL;
TextureFilterOptions  pc3D::m_TxtrFilters = { D3DTFN_POINT, D3DTFG_POINT, D3DTFP_NONE };


// ===========================================================================
// Function:    init
//
// Description: Initializes the static pc3D class.  Sets up the DirectDraw and
//              Direct3D components.
//
// Params:      hWnd:         Window handle of the application's main window
//              nWidth:       Requested width of the primary surface
//              nHeight:      Requested height of the primary surface
//              nBpp:         Requested colour depth of the primary surface
//              bUseZBuffer:  Are we going to use a Z Buffer?
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if everything was successfully initialized, FALSE
//              otherwise.
// ===========================================================================
BOOL pc3D::Init(HWND hWnd, UINT nWidth, UINT nHeight, UINT nBpp,
                BOOL bUseZBuffer /*= TRUE*/, BOOL bMultitasking /*= FALSE*/,
                WindowedMode nWindowedMode /*= WINDOWED_USE_DEFAULT*/)
{
  HRESULT hResult       = NULL;
  DWORD   dwDxVersion   = 0;
  DWORD   dwDxPlatform  = 0;

  // Check for DX5.0 or greater
  GetDXVersion(dwDxVersion, dwDxPlatform);
  if (dwDxVersion < 0x600)
  {
    MessageBox(NULL, TEXT("This App requires DirectX 6.0 or greater\r\nin order to run."),
               TEXT("Error"), MB_OK);
    return FALSE;
  }

  //  Gotcha:   You must Show the window before you change to Fullscreen
  //            Otherwise the following can happen.
  //            1.  SetCooperativeLevel(DDSCL_EXCLUSIVE) works
  //            2.  SetDisplayMode(w, h, bpp) loses the exclusive mode state
  //                because the window is not fully initialized. (DDraw gets
  //                confused by the responses it gets from the window)
  //            3.  CreateSurface(PRIMARY | FLIP) fails with
  //                DDERR_NOEXCLUSIVEMODE.  As the exclusive mode state has
  //                been lost.

  //  ADDENDUM: In actuality you can get away with changing to fullscreen
  //            mode before the window is shown.  What will happen is after
  //            you SetDisplayMode you will loose your Exclusive access.
  //            You can test this with the new 
  //            IDirectDraw4::TestCooperativeLevel() call (introduced with
  //            DirectX 6.0).  If you have lost your Exclusive cooperative
  //            level, just call SetCoorperativeLevel() again.
  //

  // Make sure to show the Window and Paint it's contents
  ShowWindow(hWnd, SW_SHOWDEFAULT);
  UpdateWindow(hWnd);

  m_pMainWnd = new DDrawWnd();
  if (m_pMainWnd->Create(hWnd, nWidth, nHeight, nBpp, bUseZBuffer,
                         bMultitasking, nWindowedMode))
  {
    // Initialize the default material
    ::InitDefaultMaterial();
    return(TRUE);
  }

  // If we make it here, something went wrong.
  DESTROYPOINTER(m_pMainWnd);
  return(FALSE);
}


void pc3D::GetDisplaySize(DWORD& dwX, DWORD& dwY)
{
  ASSERT(NULL != m_pMainWnd);
  m_pMainWnd->GetRenderSurface()->GetSize(dwX, dwY);
}


DWORD pc3D::GetRenderSurfaceBitDepth()
{
  ASSERT(NULL != m_pMainWnd);
  Surface* pPrimary = m_pMainWnd->GetPrimarySurface();
  ASSERT(NULL != pPrimary);
  return(pPrimary->GetBitDepth());
}

BOOL pc3D::Cleanup()
{
  ::DestroyMaterials();

  DESTROYPOINTER(m_pMainWnd);
  return TRUE;
}


/*
**-----------------------------------------------------------------------------
**  Name:       D3DWindowProc
**  Purpose:    handles windows messages for D3DWindow's
**-----------------------------------------------------------------------------
*/

LRESULT CALLBACK pc3D::WindowProc(HWND hWindow, UINT uiMsg, WPARAM wParam,
                                  LPARAM lParam)
{
  if (pc3D::m_pMainWnd)
    return(pc3D::m_pMainWnd->SendMessage(uiMsg, wParam, lParam));

  // Do Default window behavior
  return DefWindowProc(hWindow, uiMsg, wParam, lParam);
}


// ===========================================================================
// End of File ===============================================================
// ===========================================================================
