// ===========================================================================
// Module:      DDrawWnd.h
//
// Description: DirectDraw window class definition.  This class represents a
//              "window" which is the primary and secondary surfaces where all
//              of the rendering will take place.
//
// Created:     April 19, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_DDRAWWND
#define INC_DDRAWWND

// Forward declarations ======================================================
class DDrawDriver;
class D3DDevice;
class DDDrvMgr;

// Typedefs ==================================================================
typedef void (*APP_PAUSE_CALLBACK)(BOOL bPaused, LPVOID pData);


// ===========================================================================
// DDrawWnd class ============================================================
// ===========================================================================
class DDrawWnd
{
  // Member variables ========================================================
  private:
    DDrawDriver*      m_pDDDriver;    // The DirectDraw driver we are using
    D3DDevice*        m_pD3DDevice;   // Direct3D device

    Surface           m_Primary;      // Primary surface
    Surface           m_Render;       // Rendering surface
    Surface           m_ZBuffer;      // ZBuffer surface

    HWND              m_hWindow;      // Attached window handle
    RECT              m_rctWindow;    // Rectangle bounding the window

    LPVOID            m_pCallbackData;    // Data passed to the pause callback
    APP_PAUSE_CALLBACK m_pPauseCallback;  // Callback function for when the
                                          // app is paused or unpaused

    // Flags
    BYTE              m_bWindowed         :1; // Are we in windowed mode?
    BYTE              m_bPaused           :1; // Are we paused?
    BYTE              m_bMultithreaded    :1; // Are we running a multithreaded app?
    BYTE              m_bINIFileGenerated :1; // Has the INI file been generated?
    BYTE              m_bInitialized      :1; // Is the window initialized?
    BYTE              m_bDoDefault        :1; // Are we to perform default
                                                //  window message actions?


  // Construction / Destruction ==============================================
  public:
    DDrawWnd()                  { Clear();                                  }
     ~DDrawWnd()                { Destroy();                                }


  // Private member functions ================================================
  private:
    void    Destroy();
    void    Clear()             { ZeroMemory(this, sizeof(DDrawWnd));       }


  // Protected member functions ==============================================
  protected:
    BOOL    CreateSurfaces(DWORD dwWidth, DWORD dwHeight);
    BOOL    CreateZBuffer();
    BOOL    ValidateRenderTarget(Surface& renderTarget);

    DDrawDriver*  FindDDDriver(DDDrvMgr& ddDrvMgr, DWORD dwRequiredMem);
    D3DDevice*    FindD3DDevice(DDDrvMgr& ddDrvMgr, DWORD dwRequiredMem);

    BOOL    FlipToGDI();


  // Public member functions =================================================
  public:
    BOOL    Create(HWND hWnd, DWORD  dwWidth, DWORD dwHeight, DWORD dwBPP,
                   BOOL bUseZBuffer = TRUE, BOOL bMultitasking = FALSE,
                   WindowedMode nWindowMode = WINDOWED_USE_DEFAULT);

    BOOL    GetAvailableVidMem(DWORD& dwTotal, DWORD& dwFree,
                               DWORD& dwTotalTex, DWORD& dwFreeTex,
                               const PDDSURFCAPS pExtraCaps = NULL);

    BOOL    SetDisplayMode(BOOL bWindowed, UINT nWidth, UINT nHeight,
                           UINT nClrDepth = 0, BOOL bUseZBuffer = TRUE);

    BOOL    SetINIDisplayMode(UINT nWidth, UINT nHeight, UINT nClrDepth);

    void    SetPauseCallback(APP_PAUSE_CALLBACK pCallback, LPVOID pData)
    { m_pPauseCallback = pCallback; m_pCallbackData = pData;                }

    void    ClearMainSurfaces();
    BOOL    RestoreMainSurfaces();
    void    Pause(BOOL bPause);

    DDrawDriver* GetDDrawDriver()   const   { return(m_pDDDriver);          }
    D3DDevice*   Get3DDevice()      const   { return(m_pD3DDevice);         }

    Surface*    GetPrimarySurface()         { return(&m_Primary);           }
    Surface*    GetRenderSurface()          { return(&m_Render);            }
    Surface*    GetZBuffer()                { return(&m_ZBuffer);           }

    void        GetWindowRect(RECT& rct)    { rct = m_rctWindow;            }
    HWND        GetMainWindow()     const   { return(m_hWindow);            }

    BOOL        IsWindowed()        const   { return(m_bWindowed);          }
    BOOL        IsInitialized()     const   { return(m_bInitialized);       }
    BOOL        IsPaused()          const   { return(m_bPaused);            }

    BOOL        FlipDisplay(BOOL bWaitUntilReady = TRUE);

    INL BOOL    IsWndMinimized()    const;
    INL BOOL    IsWndMaximized()    const;
    INL BOOL    IsActive()          const;


  // Window message handlers =================================================
  public:
    LRESULT SendMessage(UINT uiMsg, WPARAM wParam, LPARAM lParam);

    INL void OnEnterMenuLoop(WPARAM wParam, LPARAM lParam);
    INL void OnExitMenuLoop(WPARAM wParam, LPARAM lParam);

        void OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam);
        void OnSysCommand(WPARAM wParam, LPARAM lParam);
        void OnWindowPosChanging(WPARAM wParam, LPARAM lParam);

};


// ===========================================================================
// INLINE: DDrawWnd member functions =========================================
// ===========================================================================

// ===========================================================================
INL BOOL DDrawWnd::IsWndMinimized() const
{
  return(::IsIconic(m_hWindow));
}

// ===========================================================================
INL BOOL DDrawWnd::IsWndMaximized() const
{
  return(::IsZoomed(m_hWindow));
}

// ===========================================================================
INL BOOL DDrawWnd::IsActive() const
{
  return(!m_bPaused);
//  return(::GetActiveWindow() == m_hWindow);
}

// ===========================================================================
INL void DDrawWnd::OnEnterMenuLoop(WPARAM wParam, LPARAM lParam)
{
  if (m_hWindow) Pause(TRUE);
}

// ===========================================================================
INL void DDrawWnd::OnExitMenuLoop(WPARAM wParam, LPARAM lParam)
{
  if (m_hWindow)
  {
    Pause(FALSE);
    ::SetCursor(NULL);
  }
}

#endif // INC_DDRAWWND
// ===========================================================================
