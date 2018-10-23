// ===========================================================================
// Module:      Surface.h
//
// Description: Surface class definition.  Encapsulates a DirectDraw surface
//              as well as a Direct3D texture.
//
// Created:     June 16, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_SURFACE
#define INC_SURFACE


// Forward declarations
class Tag;
class DDrawDriver;

// ===========================================================================
// MACROS ====================================================================
// ===========================================================================

#define FLAGINITIALIZED(f)    ((BYTE)(f) & 0x02)
#define FLAGISSET(f)          ((BYTE)(f) & 0x01)

#define RELEASESURFACE(surf)  (surf).ReleaseSurface(#surf, __FILE__, __LINE__)
#define DESTROYSURFACE(psurf)                                               \
  if (psurf)                                                                \
  {                                                                         \
    (psurf)->ReleaseSurface(#psurf, __FILE__, __LINE__);                    \
    delete (psurf);                                                         \
    (psurf) = NULL;                                                         \
  }

// ===========================================================================
// Global functions ==========================================================
// ===========================================================================
extern "C" {
  HBITMAP LoadBMP(LPCSTR szBitmapName);
  DWORD GetBitShift(DWORD dwBitMask);
  DWORD GetBitDepth(DWORD dwBitMask);
}


// ===========================================================================
// Surface class =============================================================
// ===========================================================================
class Surface
{
  // Member variables ========================================================
  private:
    // Static member holding a pointer to the DirectDraw driver.  We need this
    //  to create things like surfaces and palettes.
    static DDrawDriver* m_pDDDriver;

    PDDSURFACE      m_pDDSurface;       // IDirectDrawSurface pointer
    PD3DTEXTURE     m_pD3DTexture;      // IDirect3DTexture pointer
    
    SIZE            m_Size;             // Size of the surface in pixels

    // The following member variables are flags indicating various properties
    //  of the surface.  Each flag is two bits in size.  If the high bit is
    //  not set, the flag has not been initialized yet.  The lower bit
    //  indicates the actual property.  So for example if m_bIsTexture == 11
    //  then this is a texture surface.  If m_bIsTexture == 01 or 
    //  m_bIsTexture == 00, the fact of whether this surface is a texture or
    //  not has not yet been determined.  See the FLAGINITIALIZED() and
    //  FLAGISSET() macros at the top for more info.

    BYTE            m_bIsPrimary  : 2;  // Is the surface primary?
    BYTE            m_bIsTexture  : 2;  // Is the surface a texture?
    BYTE            m_bIsMipMap   : 2;  // Is the surface a mipmap texture?
    BYTE            m_bHasPalette : 2;  // Does the surface have a palette
    BYTE            m_bInVideoMem : 2;  // Is the surface in video memory
                                        // (Not valid if surface is a texture)


  // Construction / Destruction ==============================================
  public:
    Surface();
    Surface(Tag* pBitmap, BOOL bIsTexture = FALSE, BOOL bUseVideoMem = FALSE);
    Surface(LPCSTR szBmpFile, BOOL bIsTexture = FALSE, BOOL bUseVideoMem = FALSE);

    INL Surface(PDDSURFACE pSurface);
    INL Surface(Surface& surface, BOOL bIsTexture = FALSE, 
                BOOL bUseVideoMem = FALSE);

    INL Surface(UINT nWidth, UINT nHeight, BOOL bUseVideoMem = FALSE);
    INL Surface(UINT nWidth, UINT nHeight, BYTE byClrDepth,
                UINT nAlphaBits = 0, PALETTEENTRY* pPalEntries = NULL);

    INL Surface(HBITMAP hBitmap, BOOL bIsTexture = TRUE,
                BOOL bUseVideoMem = FALSE);

    INL ~Surface();


  // Private member functions ================================================
  private:


  // Protected member functions ==============================================
  protected:
    void  DetermineIfPrimary();
    void  DetermineIfTexture();
    void  DetermineIfMipMapped();
    void  DetermineIfPalettized();
    void  DetermineIfInVideoMem();

    void  DetermineSize(PDDSURFACE pSurface, SIZE& size);
    void  ObtainD3DTexture();

    BOOL  IsSurfaceCompatible(Surface& surface);

    PDDPALETTE  PaletteFromBitmap(HBITMAP hBitmap);

    BOOL  Save8BitBmpFile(HANDLE hFile, const RECT& rctBitmap);
    BOOL  Save24BitBmpFile(HANDLE hFile, const RECT& rctBitmap);


  // Public member functions =================================================
  public:
    static void SetDDrawDriver(DDrawDriver* pDDDriver) 
    { ASSERT(NULL != pDDDriver); Surface::m_pDDDriver = pDDDriver;          }

    static BOOL FindTextureFormat(BYTE byBPP, UINT nAlphaBits,
                                  DDPIXELFORMAT& ddpf);

    INL PDDSURFACE  Detach();
    INL PDDSURFACE  GetDDSurface() const;
    INL PD3DTEXTURE GetD3DTexture();
    INL DWORD       GetBitDepth();
    INL void        GetRect(RECT& rct) const;
    INL void        GetSize(SIZE& size) const;
    INL void        GetSize(DWORD& dwWidth, DWORD& dwHeight) const;

    INL BOOL        IsPrimary();
    INL BOOL        IsTexture();
    INL BOOL        IsPalettized();
    INL BOOL        IsInVideoMem();
    INL BOOL        IsMipMap();
    INL BOOL        IsInitialized();

    INL BOOL        AddAttachedSurface(Surface& pSurface);
    INL BOOL        DeleteAttachedSurface(DWORD dwFlags, Surface& surface);
    INL BOOL        GetAttachedSurface(DDSURFCAPS& ddsCaps, Surface& surface);
    INL BOOL        SolidRect(RECT* pRect, DWORD dwClr = 0);
    INL BOOL        ClearSurface(DWORD dwClr = 0);

    INL void        ReleaseSurface(LPSTR szVarname = NULL, LPSTR szFile = NULL,
                                   DWORD dwLine = 0);

    BOOL  Create(DDSURFDESC& surfDesc);
    BOOL  Create(PDDSURFACE pDDSurface);
    BOOL  Create(Surface& surface, BOOL bIsTexture = FALSE,
                 BOOL bUseVideoMem = FALSE);

    BOOL  Create(UINT nWidth, UINT nHeight, BOOL bUseVideoMem = FALSE);
    BOOL  Create(UINT nWidth, UINT nHeight, BYTE nClrDepth,
                 UINT nAlphabits = 0, PALETTEENTRY* pPalEntries = NULL);
    
    BOOL  Create(HBITMAP hBitmap, BOOL bIsTexture = FALSE,
                 BOOL bUseVideoMem = FALSE);

    BOOL  CreateTexture(UINT nWidth, UINT nHeight, 
                        const DDPIXELFORMAT& ddPixelFormat);

    BOOL  CreateMipMap(UINT nMaxWidth, UINT nMaxHeight, BYTE byLevels,
                       const DDPIXELFORMAT& ddPixelFormat);

    BOOL  LoadMipMaps(HBITMAP* aBitmaps, int nNumBitmaps);
    BOOL  LoadMipMaps(HBITMAP hCompositeBmp, int nNumLevels);

    BOOL  CopySurface(Surface& ddsSource);
    BOOL  CopyBitmap(HBITMAP hBitmap);
 
    BOOL  BltFast(DWORD dwX, DWORD dwY, Surface& sourceSurf,
                  RECT& rctSrc, DWORD dwTrans);

    BOOL  GDIBlit(DWORD dwX, DWORD dwY, Surface& sourceSurf, 
                  const RECT& rctSrc);

    BOOL  RestoreSurface(Surface& surface);
    BOOL  SetAsActiveTexture(DWORD dwStage = 0);

    BOOL  SaveToBmpFile(LPCSTR szFilename, LPRECT pRect = NULL);


  // IDirectDrawSurface wrapper functions ====================================
  public:
    INL BOOL  AddAttachedSurface(PDDSURFACE pSurface);

        BOOL  Blt(RECT& rctDest, PDDSURFACE pDDSSrc, RECT& rctSrc,
                  DWORD dwFlags, DDBLTFX& ddBltFx);

    INL BOOL  BltBatch(DDBLTBATCH& ddBltBatch, DWORD dwCount,
                       DWORD dwFlags);

        BOOL  BltFast(DWORD dwX, DWORD dwY, PDDSURFACE pDDSSrc, RECT& rctSrc,
                      DWORD dwTrans);
    
    INL BOOL  DeleteAttachedSurface(DWORD dwFlags, PDDSURFACE pDDSAttached);
    INL BOOL  EnumAttachedSurfaces(LPVOID lpContext, 
                                   LPDDENUMSURFACESCALLBACK2 lpEnumSurfacesCallback);

    INL BOOL  Flip(DWORD dwFlags = 0, PDDSURFACE pDDSTargetOverride = NULL);

    INL PDDSURFACE  GetAttachedSurface(DDSURFCAPS& ddsCaps);
    INL HRESULT     GetBltStatus(DWORD dwFlags);
    INL BOOL        GetCaps(DDSURFCAPS& ddsCaps);
    INL PDDCLIPPER  GetClipper();
    INL BOOL        GetColorKey(DWORD dwFlags, DDCOLORKEY& ddColorKey);
    INL HDC         GetDC();
    INL HRESULT     GetFlipStatus(DWORD dwFlags);
    INL PDDPALETTE  GetPalette();

    INL BOOL        GetPixelFormat(DDPIXELFORMAT& ddPixelFormat);
    INL BOOL        GetSurfaceDesc(DDSURFDESC& surfDesc);

    INL BOOL  IsLost();

    INL BOOL  Lock(DDSURFDESC& surfDesc, DWORD dwFlags = 0,
                   LPRECT prctDest = NULL, HANDLE hEvent = NULL);

    INL BOOL  PageLock(DWORD dwFlags = 0);
    INL BOOL  PageUnlock(DWORD dwFlags = 0);
    INL BOOL  Unlock(LPRECT prct = NULL);

    INL BOOL  Restore();

    INL BOOL  ReleaseDC(HDC hSurfaceDC);
    INL BOOL  SetClipper(PDDCLIPPER pClipper);
    INL BOOL  SetColorKey(DWORD dwFlags, DDCOLORKEY& ddColorKey);
    INL BOOL  SetPalette(PDDPALETTE pPalette);
    INL BOOL  SetSurfaceDesc(DDSURFDESC& ddsd, DWORD dwFlags = 0);

};

// Include the inline functions
#include "Surface.inl"

#endif // INC_SURFACE
// ===========================================================================
