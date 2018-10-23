// ===========================================================================
// Module:      RenderSettings.h
//
// Description: Contains the RenderSettings structure definition used to set
//              default rendering behaviour for Direct3D.
//
// Created:     June 22, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_RENDERSETTINGS
#define INC_RENDERSETTINGS

// ===========================================================================
// RenderSettings structure ==================================================
// ===========================================================================
struct RenderSettings
{
  BYTE  byDefaultShadeMode      : 4;  // Default shading mode
  BYTE  byDefaultFillMode       : 4;  // Default fill mode for solid geometry
  BYTE  bDithering              : 1;  // Are we supposed to use dithering?
  BYTE  bSpecularHighLights     : 1;  // Should we use specular highlights?
  BYTE  bPerspectiveCorrection  : 1;  // Should we use perspecitve correction?
  BYTE  bBilinearFiltering      : 1;  // Should we use bilinear filtering?
};


extern RenderSettings     g_3DRenderSettings;
#define RS                g_3DRenderSettings


// ===========================================================================
// RenderSettings manipulation macros ========================================
// ===========================================================================

#define SET_SHADEMODE(mode)             RS.byDefaultShadeMode = (mode)
#define SET_FILLMODE(mode)              RS.byDefaultFillMode = (mode)

#define ENABLE_DITHERING(b)             RS.bDithering = (b)
#define ENABLE_SPECULARHIGHLIGHTS(b)    RS.bSpecularHighLights = (b)
#define ENABLE_PERSPECTIVECORRECTION(b) RS.bPerspectiveCorrection = (b)
#define ENABLE_BILINEARFILTERING(b)     RS.bBilinearFiltering = (b)


#endif // INC_RENDERSETTINGS
// ===========================================================================
