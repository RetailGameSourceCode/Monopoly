// ===========================================================================
// Module:      DXINI.h
//
// Description: Contains defines and function prototypes for the DirectX INI
//              file handling stuff
//
// Created:     April 26, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_DXINI
#define INC_DXINI

#include "DDrawDrv.h"

// ===========================================================================
// Typdefs and defines =======================================================
// ===========================================================================

struct DXINI_Name_Desc_Pair
{
  char  szName[MAX_DDDEVICEID_STRING];
  char  szDesc[MAX_DDDEVICEID_STRING];
};

struct DXINI_DriverInfo
{
  DXINI_Name_Desc_Pair    driver;
  DXINI_Name_Desc_Pair    a3DDevices[MAX_3DDEVICES];
  int                     nNum3DDevices;
};

#define MAX_DDDRIVERS     7

struct DXINI_SelectionDialogInfo
{
  DXINI_DriverInfo    aDrivers[MAX_DDDRIVERS];
  int                 nNumDrivers;
  int                 nSelectedDriver;
  int                 nSelected3DDevice;
  BOOL                bWindowedMode;
};


// Driver selection dialog definition ========================================
#define IDD_DRV_SELECTION             201
#define IDC_DRIVERLIST                2000
#define IDC_3DDEVICELIST              2001
#define IDC_WINDOWEDMODE              2002


// DirectX INI file key names ================================================

#define DXINIFILE                     "DirectX.ini"

#define DXINI_DDRAWDRIVERSSECTION     TEXT("DirectDrawDrivers")
#define DXINI_DDRAWDRIVER             TEXT("DirectDrawDriver")
#define DXINI_NUM_DDRAWDRIVERS        TEXT("NumberOfDirectDrawDrivers")
#define DXINI_DRIVERNAME              TEXT("DriverName")
#define DXINI_DRIVERDESC              TEXT("DriverDescription")
#define DXINI_DRIVERGUIDBASE          TEXT("Driver")

#define DXINI_SETTINGSSECTION         TEXT("Settings")
#define DXINI_SELECTED_DDRAWDRIVER    TEXT("SelectedDirectDrawDriver")
#define DXINI_SELECTED_3DDEVICE       TEXT("Selected3DDevice")
#define DXINI_SELECTED_RESOLUTION     TEXT("SelectedResolution")
#define DXINI_USE_SELECTED_RESOLUTION TEXT("UseSelectedResolution")
#define DXINI_WINDOWEDMODE            TEXT("WindowedMode")

#define DXINI_NUM_3DDEVICES           TEXT("NumberOf3DDevices")
#define DXINI_3DDEVICE                TEXT("3DDevice")

#define DXINI_NUM_RESOLUTIONS         TEXT("NumberOfScreenResolutions")
#define DXINI_RESOLUTION              TEXT("ScreenResolution")


// 3D device capabilites =====================================================
#define DXINI_3DCAP_COLOURMODEL               TEXT("ColourModel")
#define DXINI_3DCAP_CLIPPING                  TEXT("Clipping")
#define DXINI_3DCAP_SUPPORTEDRENDERDEPTHS     TEXT("SupportedRenderDepths")
#define DXINI_3DCAP_SUPPORTEDZBUFFERFORMATS   TEXT("SupportedZBufferFormats")
#define DXINI_3DCAP_MAXEXECUTEBUFFERSIZE      TEXT("MaxExecuteBufferSize")
#define DXINI_3DCAP_MAXVERTEXCOUNT            TEXT("MaxVertexCount")
#define DXINI_3DCAP_MINTEXTUREWIDTH           TEXT("MinTextureWidth")
#define DXINI_3DCAP_MAXTEXTUREWIDTH           TEXT("MaxTextureWidth")
#define DXINI_3DCAP_MINTEXTUREHEIGHT          TEXT("MinTextureHeight")
#define DXINI_3DCAP_MAXTEXTUREHEIGHT          TEXT("MaxTextureHeight")
#define DXINI_3DCAP_MINSTIPPLEWIDTH           TEXT("MinStippleWidth")
#define DXINI_3DCAP_MAXSTIPPLEWIDTH           TEXT("MaxStippleWidth")
#define DXINI_3DCAP_MINSTIPPLEHEIGHT          TEXT("MinStippleHeight")
#define DXINI_3DCAP_MAXSTIPPLEHEIGHT          TEXT("MaxStippleHeight")
#define DXINI_3DCAP_MAXTEXTUREREPEAT          TEXT("MaxTextureRepeat")
#define DXINI_3DCAP_MAXTEXTUREASPECTRATIO     TEXT("MaxTextureAspectRatio")
#define DXINI_3DCAP_MAXANISOTROPY             TEXT("MaxAnisotropy")
#define DXINI_3DCAP_GUARDBANDLEFT             TEXT("GuardBandLeft")
#define DXINI_3DCAP_GUARDBANDRIGHT            TEXT("GuardBandRight")
#define DXINI_3DCAP_GUARDBANDTOP              TEXT("GuardBandTop")
#define DXINI_3DCAP_GUARDBANDBOTTOM           TEXT("GuardBandBottom")
#define DXINI_3DCAP_EXTENTSADJUST             TEXT("ExtentsAdjust")
#define DXINI_3DCAP_MAXTEXTURESTAGES          TEXT("MaxTextureStages")
#define DXINI_3DCAP_MAXSIMULTANEOUSTEXTURES   TEXT("MaxSimultaneousTextures")

// dwDevCaps
#define DXINI_3DCAP_CANRENDERAFTERFLIP        TEXT("CanRenderAfterFlip")
#define DXINI_3DCAP_DRAWPRIMTLVERTEX          TEXT("DrawPrimTLVertex")
#define DXINI_3DCAP_EXECUTESYSTEMMEMORY       TEXT("ExecuteSystemMemory")
#define DXINI_3DCAP_EXECUTEVIDEOMEMORY        TEXT("ExecuteVideoMemory")
#define DXINI_3DCAP_FLOATTLVERTEX             TEXT("FloatVertex")
#define DXINI_3DCAP_SEPARATETEXTUREMEMORIES   TEXT("SeparateTextureMemories")
#define DXINI_3DCAP_SORTDECREASINGZ           TEXT("SortDecreasingZ")
#define DXINI_3DCAP_SORTEXACT                 TEXT("SortExact")
#define DXINI_3DCAP_SORTINCREASINGZ           TEXT("SortIncreasingZ")
#define DXINI_3DCAP_TEXREPEATNOTSCALEDBYSIZE  TEXT("TextureRepeatNotScaledBySize")
#define DXINI_3DCAP_TEXTURENONLOCALVIDMEM     TEXT("TextureNonLocalVidMem")
#define DXINI_3DCAP_TEXTURESYSTEMMEMORY       TEXT("TextureSystemMemory")
#define DXINI_3DCAP_TEXTUREVIDEOMEMORY        TEXT("TextureVideoMemory")
#define DXINI_3DCAP_TLVERTEXSYSTEMMEMORY      TEXT("TLVertexSystemMemory")
#define DXINI_3DCAP_TLVERTEXVIDEOMEMORY       TEXT("TLVertexVideoMemory")

// dtcTransformCaps
#define DXINI_3DCAP_TRANSFORM_CLIP            TEXT("TRANSFORM_Clip")

// dlcLightingCaps
#define DXINI_3DCAP_LIGHT_DIRECTIONAL         TEXT("LIGHT_Directional")
#define DXINI_3DCAP_LIGHT_GLSPOT              TEXT("LIGHT_GLSpot")
#define DXINI_3DCAP_LIGHT_PARALLELPOINT       TEXT("LIGHT_ParallelPoint")
#define DXINI_3DCAP_LIGHT_POINT               TEXT("LIGHT_Point")
#define DXINI_3DCAP_LIGHT_SPOT                TEXT("LIGHT_Spot")
#define DXINI_3DCAP_LIGHT_MODEL               TEXT("LIGHT_Model")
#define DXINI_3DCAP_LIGHT_MAXLIGHTS           TEXT("LIGHT_MaxLights")

// dpcLineCaps
#define DXINI_3DCAP_LINE_MISC_CONFORMANT      TEXT("LINE_MISC_Conformant")
#define DXINI_3DCAP_LINE_MISC_CULLCCW         TEXT("LINE_MISC_CullCCW")
#define DXINI_3DCAP_LINE_MISC_CULLCW          TEXT("LINE_MISC_CullCW")
#define DXINI_3DCAP_LINE_MISC_CULLNONE        TEXT("LINE_MISC_CullNone")
#define DXINI_3DCAP_LINE_MISC_LINEPATTERNREP  TEXT("LINE_MISC_LinePatternRep")
#define DXINI_3DCAP_LINE_MISC_MASKPLANES      TEXT("LINE_MISC_MaskPlanes")
#define DXINI_3DCAP_LINE_MISC_MASKZ           TEXT("LINE_MISC_MaskZ")

#define DXINI_3DCAP_LINE_RAST_ANISOTROPY      TEXT("LINE_RAST_Anisotropy")
#define DXINI_3DCAP_LINE_RAST_ANTIALIASEDGES  TEXT("LINE_RAST_AntiAliasEdges")
#define DXINI_3DCAP_LINE_RAST_ANTIALIASSORTDEPENDENT    TEXT("LINE_RAST_AntiAliasSortDependent")
#define DXINI_3DCAP_LINE_RAST_ANTIALIASSORTINDEPENDENT  TEXT("LINE_RAST_AntiAliasSortIndependent")
#define DXINI_3DCAP_LINE_RAST_DITHER          TEXT("LINE_RAST_Dither")
#define DXINI_3DCAP_LINE_RAST_FOGRANGE        TEXT("LINE_RAST_FogRange")
#define DXINI_3DCAP_LINE_RAST_FOGTABLE        TEXT("LINE_RAST_FogTable")
#define DXINI_3DCAP_LINE_RAST_FOGVERTEX       TEXT("LINE_RAST_FogVertex")
#define DXINI_3DCAP_LINE_RAST_MIPMAPLODBIAS   TEXT("LINE_RAST_MipMapLODBias")
#define DXINI_3DCAP_LINE_RAST_PAT             TEXT("LINE_RAST_Pat")
#define DXINI_3DCAP_LINE_RAST_ROP2            TEXT("LINE_RAST_ROP2")
#define DXINI_3DCAP_LINE_RAST_STIPPLE         TEXT("LINE_RAST_Stipple")
#define DXINI_3DCAP_LINE_RAST_SUBPIXEL        TEXT("LINE_RAST_SubPixel")
#define DXINI_3DCAP_LINE_RAST_SUBPIXELX       TEXT("LINE_RAST_SubPixelX")
#define DXINI_3DCAP_LINE_RAST_TRANSLUCENTSORTINDEPENDENT  TEXT("LINE_RAST_TranslucentSortIndpendent")
#define DXINI_3DCAP_LINE_RAST_WBUFFER         TEXT("LINE_RAST_WBuffer")
#define DXINI_3DCAP_LINE_RAST_WFOG            TEXT("LINE_RAST_WFog")
#define DXINI_3DCAP_LINE_RAST_XOR             TEXT("LINE_RAST_XOR")
#define DXINI_3DCAP_LINE_RAST_ZBIAS           TEXT("LINE_RAST_ZBias")
#define DXINI_3DCAP_LINE_RAST_ZBUFFERLESSHSR  TEXT("LINE_RAST_ZBufferlessHSR")
#define DXINI_3DCAP_LINE_RAST_ZTEST           TEXT("LINE_RAST_ZTest")

#define DXINI_3DCAP_LINE_ZCMP_ALWAYS          TEXT("LINE_ZCMP_Always")
#define DXINI_3DCAP_LINE_ZCMP_EQUAL           TEXT("LINE_ZCMP_Equal")
#define DXINI_3DCAP_LINE_ZCMP_GREATER         TEXT("LINE_ZCMP_Greater")
#define DXINI_3DCAP_LINE_ZCMP_GREATEREQUAL    TEXT("LINE_ZCMP_GreaterEqual")
#define DXINI_3DCAP_LINE_ZCMP_LESS            TEXT("LINE_ZCMP_Less")
#define DXINI_3DCAP_LINE_ZCMP_LESSEQUAL       TEXT("LINE_ZCMP_LessEqual")
#define DXINI_3DCAP_LINE_ZCMP_NEVER           TEXT("LINE_ZCMP_Never")
#define DXINI_3DCAP_LINE_ZCMP_NOTEQUAL        TEXT("LINE_ZCMP_NotEqual")

#define DXINI_3DCAP_LINE_SBLND_BOTHINVSRCALPHA  TEXT("LINE_SBLND_BothInvSrcAlpha")
#define DXINI_3DCAP_LINE_SBLND_BOTHSRCALPHA   TEXT("LINE_SBLND_BothSrcAlpha")
#define DXINI_3DCAP_LINE_SBLND_DESTALPHA      TEXT("LINE_SBLND_DestAlpha")
#define DXINI_3DCAP_LINE_SBLND_DESTCOLOR      TEXT("LINE_SBLND_DestColour")
#define DXINI_3DCAP_LINE_SBLND_INVDESTALPHA   TEXT("LINE_SBLND_InvDestAlpha")
#define DXINI_3DCAP_LINE_SBLND_INVDESTCOLOR   TEXT("LINE_SBLND_InvDestColour")
#define DXINI_3DCAP_LINE_SBLND_INVSRCALPHA    TEXT("LINE_SBLND_InvSrcAlpha")
#define DXINI_3DCAP_LINE_SBLND_INVSRCCOLOR    TEXT("LINE_SBLND_InvSrcColour")
#define DXINI_3DCAP_LINE_SBLND_ONE            TEXT("LINE_SBLND_One")
#define DXINI_3DCAP_LINE_SBLND_SRCALPHA       TEXT("LINE_SBLND_SrcAlpha")
#define DXINI_3DCAP_LINE_SBLND_SRCALPHASAT    TEXT("LINE_SBLND_SrcAlphaSat")
#define DXINI_3DCAP_LINE_SBLND_SRCCOLOR       TEXT("LINE_SBLND_SrcColour")
#define DXINI_3DCAP_LINE_SBLND_ZERO           TEXT("LINE_SBLND_Zero")

#define DXINI_3DCAP_LINE_DBLND_BOTHINVSRCALPHA  TEXT("LINE_DBLND_BothInvSrcAlpha")
#define DXINI_3DCAP_LINE_DBLND_BOTHSRCALPHA   TEXT("LINE_DBLND_BothSrcAlpha")
#define DXINI_3DCAP_LINE_DBLND_DESTALPHA      TEXT("LINE_DBLND_DestAlpha")
#define DXINI_3DCAP_LINE_DBLND_DESTCOLOR      TEXT("LINE_DBLND_DestColour")
#define DXINI_3DCAP_LINE_DBLND_INVDESTALPHA   TEXT("LINE_DBLND_InvDestAlpha")
#define DXINI_3DCAP_LINE_DBLND_INVDESTCOLOR   TEXT("LINE_DBLND_InvDestColour")
#define DXINI_3DCAP_LINE_DBLND_INVSRCALPHA    TEXT("LINE_DBLND_InvSrcAlpha")
#define DXINI_3DCAP_LINE_DBLND_INVSRCCOLOR    TEXT("LINE_DBLND_InvSrcColour")
#define DXINI_3DCAP_LINE_DBLND_ONE            TEXT("LINE_DBLND_One")
#define DXINI_3DCAP_LINE_DBLND_SRCALPHA       TEXT("LINE_DBLND_SrcAlpha")
#define DXINI_3DCAP_LINE_DBLND_SRCALPHASAT    TEXT("LINE_DBLND_SrcAlphaSat")
#define DXINI_3DCAP_LINE_DBLND_SRCCOLOR       TEXT("LINE_DBLND_SrcColour")
#define DXINI_3DCAP_LINE_DBLND_ZERO           TEXT("LINE_DBLND_Zero")

#define DXINI_3DCAP_LINE_ACMP_ALWAYS          TEXT("LINE_ACMP_Always")
#define DXINI_3DCAP_LINE_ACMP_EQUAL           TEXT("LINE_ACMP_Equal")
#define DXINI_3DCAP_LINE_ACMP_GREATER         TEXT("LINE_ACMP_Greater")
#define DXINI_3DCAP_LINE_ACMP_GREATEREQUAL    TEXT("LINE_ACMP_GreaterEqual")
#define DXINI_3DCAP_LINE_ACMP_LESS            TEXT("LINE_ACMP_Less")
#define DXINI_3DCAP_LINE_ACMP_LESSEQUAL       TEXT("LINE_ACMP_LessEqual")
#define DXINI_3DCAP_LINE_ACMP_NEVER           TEXT("LINE_ACMP_Never")
#define DXINI_3DCAP_LINE_ACMP_NOTEQUAL        TEXT("LINE_ACMP_NotEqual")

#define DXINI_3DCAP_LINE_SHADE_ALPHAFLATBLEND       TEXT("LINE_SHADE_AlphaFlatBlend")
#define DXINI_3DCAP_LINE_SHADE_ALPHAFLATSTIPPLED    TEXT("LINE_SHADE_AlphaFlatStippled")
#define DXINI_3DCAP_LINE_SHADE_ALPHAGOURAUDBLEND    TEXT("LINE_SHADE_AlphaGouraudBlend")
#define DXINI_3DCAP_LINE_SHADE_ALPHAGOURAUDSTIPPLED TEXT("LINE_SHADE_AlphaGouraudSippled")
#define DXINI_3DCAP_LINE_SHADE_ALPHAPHONGBLEND      TEXT("LINE_SHADE_AlphaPhongBlend")
#define DXINI_3DCAP_LINE_SHADE_ALPHAPHONGSTIPPLED   TEXT("LINE_SHADE_AlphaPhongStippled")
#define DXINI_3DCAP_LINE_SHADE_COLORFLATMONO        TEXT("LINE_SHADE_ColourFlatMono")
#define DXINI_3DCAP_LINE_SHADE_COLORFLATRGB         TEXT("LINE_SHADE_ColourFlatRGB")
#define DXINI_3DCAP_LINE_SHADE_COLORGOURAUDMONO     TEXT("LINE_SHADE_ColourGouraudBlend")
#define DXINI_3DCAP_LINE_SHADE_COLORGOURAUDRGB      TEXT("LINE_SHADE_ColourGouraudRGB")
#define DXINI_3DCAP_LINE_SHADE_COLORPHONGMONO       TEXT("LINE_SHADE_ColourPhongBlend")
#define DXINI_3DCAP_LINE_SHADE_COLORPHONGRGB        TEXT("LINE_SHADE_ColourPhongRGB")
#define DXINI_3DCAP_LINE_SHADE_FOGFLAT              TEXT("LINE_SHADE_FogFlat")
#define DXINI_3DCAP_LINE_SHADE_FOGGOURAUD           TEXT("LINE_SHADE_FogGouraud")
#define DXINI_3DCAP_LINE_SHADE_FOGPHONG             TEXT("LINE_SHADE_FogPhong")
#define DXINI_3DCAP_LINE_SHADE_SPECULARFLATMONO     TEXT("LINE_SHADE_SpecularFlatMono")
#define DXINI_3DCAP_LINE_SHADE_SPECULARFLATRGB      TEXT("LINE_SHADE_SpecularFlatRGB")
#define DXINI_3DCAP_LINE_SHADE_SPECULARGOURAUDMONO  TEXT("LINE_SHADE_SpecularGouraudMono")
#define DXINI_3DCAP_LINE_SHADE_SPECULARGOURAUDRGB   TEXT("LINE_SHADE_SpecularGouraudRGB")
#define DXINI_3DCAP_LINE_SHADE_SPECULARPHONGMONO    TEXT("LINE_SHADE_SpecularPhongMono")
#define DXINI_3DCAP_LINE_SHADE_SPECULARPHONGRGB     TEXT("LINE_SHADE_SpecularPhongRGB")

#define DXINI_3DCAP_LINE_TXTR_ALPHA                 TEXT("LINE_TXTR_Alpha")
#define DXINI_3DCAP_LINE_TXTR_ALPHAPALETTE          TEXT("LINE_TXTR_AlphaPalette")
#define DXINI_3DCAP_LINE_TXTR_BORDER                TEXT("LINE_TXTR_Border")
#define DXINI_3DCAP_LINE_TXTR_NONPOW2CONDITIONAL    TEXT("LINE_TXTR_NonPow2Conditional")
#define DXINI_3DCAP_LINE_TXTR_PERSPECTIVE           TEXT("LINE_TXTR_Perspective")
#define DXINI_3DCAP_LINE_TXTR_POW2                  TEXT("LINE_TXTR_Pow2")
#define DXINI_3DCAP_LINE_TXTR_SQUAREONLY            TEXT("LINE_TXTR_SquareOnly")
#define DXINI_3DCAP_LINE_TXTR_TEXREPEATNOTSCALEDBYSIZE  TEXT("LINE_TXTR_TexRepeatNotScaledBySize")
#define DXINI_3DCAP_LINE_TXTR_TRANSPARENCY          TEXT("LINE_TXTR_Transparency")

#define DXINI_3DCAP_LINE_TFILT_LINEAR               TEXT("LINE_TFILT_Linear")
#define DXINI_3DCAP_LINE_TFILT_LINEARMIPLINEAR      TEXT("LINE_TFILT_LinearMipLinear")
#define DXINI_3DCAP_LINE_TFILT_LINEARMIPNEAREST     TEXT("LINE_TFILT_LinearMipNearest")
#define DXINI_3DCAP_LINE_TFILT_MIPLINEAR            TEXT("LINE_TFILT_MipLinear")
#define DXINI_3DCAP_LINE_TFILT_MIPNEAREST           TEXT("LINE_TFILT_MipNearest")
#define DXINI_3DCAP_LINE_TFILT_NEAREST              TEXT("LINE_TFILT_Nearest")
#define DXINI_3DCAP_LINE_TFILT_MAGFAFLATCUBIC       TEXT("LINE_TFILT_MagFAFlatCubic")
#define DXINI_3DCAP_LINE_TFILT_MAGFANISOTROPIC      TEXT("LINE_TFILT_MagFAnisotropic")
#define DXINI_3DCAP_LINE_TFILT_MAGFGAUSSIANCUBIC    TEXT("LINE_TFILT_MagFGaussianCubic")
#define DXINI_3DCAP_LINE_TFILT_MAGFLINEAR           TEXT("LINE_TFILT_MagFLinear")
#define DXINI_3DCAP_LINE_TFILT_MAGFPOINT            TEXT("LINE_TFILT_MagFPoint")
#define DXINI_3DCAP_LINE_TFILT_MINFANISOTROPIC      TEXT("LINE_TFILT_MinFAnisotropic")
#define DXINI_3DCAP_LINE_TFILT_MINFLINEAR           TEXT("LINE_TFILT_MinFLinear")
#define DXINI_3DCAP_LINE_TFILT_MINFPOINT            TEXT("LINE_TFILT_MinFPoint")
#define DXINI_3DCAP_LINE_TFILT_MIPFLINEAR           TEXT("LINE_TFILT_MipFLinear")
#define DXINI_3DCAP_LINE_TFILT_MIPFPOINT            TEXT("LINE_TFILT_MipFPoint")

#define DXINI_3DCAP_LINE_TBLND_ADD                  TEXT("LINE_TBLND_Add")
#define DXINI_3DCAP_LINE_TBLND_COPY                 TEXT("LINE_TBLND_Copy")
#define DXINI_3DCAP_LINE_TBLND_DECAL                TEXT("LINE_TBLND_Decal")
#define DXINI_3DCAP_LINE_TBLND_DECALALPHA           TEXT("LINE_TBLND_DecalAlpha")
#define DXINI_3DCAP_LINE_TBLND_DECALMASK            TEXT("LINE_TBLND_DecalMask")
#define DXINI_3DCAP_LINE_TBLND_MODULATE             TEXT("LINE_TBLND_Modulate")
#define DXINI_3DCAP_LINE_TBLND_MODULATEALPHA        TEXT("LINE_TBLND_ModulateAlpha")
#define DXINI_3DCAP_LINE_TBLND_MODULATEMASK         TEXT("LINE_TBLND_ModulateMask")

#define DXINI_3DCAP_LINE_TADDR_BORDER               TEXT("LINE_TADDR_Border")
#define DXINI_3DCAP_LINE_TADDR_CLAMP                TEXT("LINE_TADDR_Clamp")
#define DXINI_3DCAP_LINE_TADDR_INDEPENDENTUV        TEXT("LINE_TADDR_IndependentUV")
#define DXINI_3DCAP_LINE_TADDR_MIRROR               TEXT("LINE_TADDR_Mirror")
#define DXINI_3DCAP_LINE_TADDR_WRAP                 TEXT("LINE_TADDR_Wrap")

#define DXINI_3DCAP_LINE_STIPPLEWIDTH               TEXT("LINE_StippleWidth")
#define DXINI_3DCAP_LINE_STIPPLEHEIGHT              TEXT("LINE_StippleHeight")

// dpcTriCaps 
#define DXINI_3DCAP_TRI_MISC_CONFORMANT      TEXT("TRI_MISC_Conformant")
#define DXINI_3DCAP_TRI_MISC_CULLCCW         TEXT("TRI_MISC_CullCCW")
#define DXINI_3DCAP_TRI_MISC_CULLCW          TEXT("TRI_MISC_CullCW")
#define DXINI_3DCAP_TRI_MISC_CULLNONE        TEXT("TRI_MISC_CullNone")
#define DXINI_3DCAP_TRI_MISC_LINEPATTERNREP  TEXT("TRI_MISC_LinePatternRep")
#define DXINI_3DCAP_TRI_MISC_MASKPLANES      TEXT("TRI_MISC_MaskPlanes")
#define DXINI_3DCAP_TRI_MISC_MASKZ           TEXT("TRI_MISC_MaskZ")

#define DXINI_3DCAP_TRI_RAST_ANISOTROPY      TEXT("TRI_RAST_Anisotropy")
#define DXINI_3DCAP_TRI_RAST_ANTIALIASEDGES  TEXT("TRI_RAST_AntiAliasEdges")
#define DXINI_3DCAP_TRI_RAST_ANTIALIASSORTDEPENDENT    TEXT("TRI_RAST_AntiAliasSortDependent")
#define DXINI_3DCAP_TRI_RAST_ANTIALIASSORTINDEPENDENT  TEXT("TRI_RAST_AntiAliasSortIndependent")
#define DXINI_3DCAP_TRI_RAST_DITHER          TEXT("TRI_RAST_Dither")
#define DXINI_3DCAP_TRI_RAST_FOGRANGE        TEXT("TRI_RAST_FogRange")
#define DXINI_3DCAP_TRI_RAST_FOGTABLE        TEXT("TRI_RAST_FogTable")
#define DXINI_3DCAP_TRI_RAST_FOGVERTEX       TEXT("TRI_RAST_FogVertex")
#define DXINI_3DCAP_TRI_RAST_MIPMAPLODBIAS   TEXT("TRI_RAST_MipMapLODBias")
#define DXINI_3DCAP_TRI_RAST_PAT             TEXT("TRI_RAST_Pat")
#define DXINI_3DCAP_TRI_RAST_ROP2            TEXT("TRI_RAST_ROP2")
#define DXINI_3DCAP_TRI_RAST_STIPPLE         TEXT("TRI_RAST_Stipple")
#define DXINI_3DCAP_TRI_RAST_SUBPIXEL        TEXT("TRI_RAST_SubPixel")
#define DXINI_3DCAP_TRI_RAST_SUBPIXELX       TEXT("TRI_RAST_SubPixelX")
#define DXINI_3DCAP_TRI_RAST_TRANSLUCENTSORTINDEPENDENT  TEXT("TRI_RAST_TranslucentSortIndpendent")
#define DXINI_3DCAP_TRI_RAST_WBUFFER         TEXT("TRI_RAST_WBuffer")
#define DXINI_3DCAP_TRI_RAST_WFOG            TEXT("TRI_RAST_WFog")
#define DXINI_3DCAP_TRI_RAST_XOR             TEXT("TRI_RAST_XOR")
#define DXINI_3DCAP_TRI_RAST_ZBIAS           TEXT("TRI_RAST_ZBias")
#define DXINI_3DCAP_TRI_RAST_ZBUFFERLESSHSR  TEXT("TRI_RAST_ZBufferlessHSR")
#define DXINI_3DCAP_TRI_RAST_ZTEST           TEXT("TRI_RAST_ZTest")

#define DXINI_3DCAP_TRI_ZCMP_ALWAYS          TEXT("TRI_ZCMP_Always")
#define DXINI_3DCAP_TRI_ZCMP_EQUAL           TEXT("TRI_ZCMP_Equal")
#define DXINI_3DCAP_TRI_ZCMP_GREATER         TEXT("TRI_ZCMP_Greater")
#define DXINI_3DCAP_TRI_ZCMP_GREATEREQUAL    TEXT("TRI_ZCMP_GreaterEqual")
#define DXINI_3DCAP_TRI_ZCMP_LESS            TEXT("TRI_ZCMP_Less")
#define DXINI_3DCAP_TRI_ZCMP_LESSEQUAL       TEXT("TRI_ZCMP_LessEqual")
#define DXINI_3DCAP_TRI_ZCMP_NEVER           TEXT("TRI_ZCMP_Never")
#define DXINI_3DCAP_TRI_ZCMP_NOTEQUAL        TEXT("TRI_ZCMP_NotEqual")

#define DXINI_3DCAP_TRI_SBLND_BOTHINVSRCALPHA  TEXT("TRI_SBLND_BothInvSrcAlpha")
#define DXINI_3DCAP_TRI_SBLND_BOTHSRCALPHA   TEXT("TRI_SBLND_BothSrcAlpha")
#define DXINI_3DCAP_TRI_SBLND_DESTALPHA      TEXT("TRI_SBLND_DestAlpha")
#define DXINI_3DCAP_TRI_SBLND_DESTCOLOR      TEXT("TRI_SBLND_DestColour")
#define DXINI_3DCAP_TRI_SBLND_INVDESTALPHA   TEXT("TRI_SBLND_InvDestAlpha")
#define DXINI_3DCAP_TRI_SBLND_INVDESTCOLOR   TEXT("TRI_SBLND_InvDestColour")
#define DXINI_3DCAP_TRI_SBLND_INVSRCALPHA    TEXT("TRI_SBLND_InvSrcAlpha")
#define DXINI_3DCAP_TRI_SBLND_INVSRCCOLOR    TEXT("TRI_SBLND_InvSrcColour")
#define DXINI_3DCAP_TRI_SBLND_ONE            TEXT("TRI_SBLND_One")
#define DXINI_3DCAP_TRI_SBLND_SRCALPHA       TEXT("TRI_SBLND_SrcAlpha")
#define DXINI_3DCAP_TRI_SBLND_SRCALPHASAT    TEXT("TRI_SBLND_SrcAlphaSat")
#define DXINI_3DCAP_TRI_SBLND_SRCCOLOR       TEXT("TRI_SBLND_SrcColour")
#define DXINI_3DCAP_TRI_SBLND_ZERO           TEXT("TRI_SBLND_Zero")

#define DXINI_3DCAP_TRI_DBLND_BOTHINVSRCALPHA  TEXT("TRI_DBLND_BothInvSrcAlpha")
#define DXINI_3DCAP_TRI_DBLND_BOTHSRCALPHA   TEXT("TRI_DBLND_BothSrcAlpha")
#define DXINI_3DCAP_TRI_DBLND_DESTALPHA      TEXT("TRI_DBLND_DestAlpha")
#define DXINI_3DCAP_TRI_DBLND_DESTCOLOR      TEXT("TRI_DBLND_DestColour")
#define DXINI_3DCAP_TRI_DBLND_INVDESTALPHA   TEXT("TRI_DBLND_InvDestAlpha")
#define DXINI_3DCAP_TRI_DBLND_INVDESTCOLOR   TEXT("TRI_DBLND_InvDestColour")
#define DXINI_3DCAP_TRI_DBLND_INVSRCALPHA    TEXT("TRI_DBLND_InvSrcAlpha")
#define DXINI_3DCAP_TRI_DBLND_INVSRCCOLOR    TEXT("TRI_DBLND_InvSrcColour")
#define DXINI_3DCAP_TRI_DBLND_ONE            TEXT("TRI_DBLND_One")
#define DXINI_3DCAP_TRI_DBLND_SRCALPHA       TEXT("TRI_DBLND_SrcAlpha")
#define DXINI_3DCAP_TRI_DBLND_SRCALPHASAT    TEXT("TRI_DBLND_SrcAlphaSat")
#define DXINI_3DCAP_TRI_DBLND_SRCCOLOR       TEXT("TRI_DBLND_SrcColour")
#define DXINI_3DCAP_TRI_DBLND_ZERO           TEXT("TRI_DBLND_Zero")

#define DXINI_3DCAP_TRI_ACMP_ALWAYS          TEXT("TRI_ACMP_Always")
#define DXINI_3DCAP_TRI_ACMP_EQUAL           TEXT("TRI_ACMP_Equal")
#define DXINI_3DCAP_TRI_ACMP_GREATER         TEXT("TRI_ACMP_Greater")
#define DXINI_3DCAP_TRI_ACMP_GREATEREQUAL    TEXT("TRI_ACMP_GreaterEqual")
#define DXINI_3DCAP_TRI_ACMP_LESS            TEXT("TRI_ACMP_Less")
#define DXINI_3DCAP_TRI_ACMP_LESSEQUAL       TEXT("TRI_ACMP_LessEqual")
#define DXINI_3DCAP_TRI_ACMP_NEVER           TEXT("TRI_ACMP_Never")
#define DXINI_3DCAP_TRI_ACMP_NOTEQUAL        TEXT("TRI_ACMP_NotEqual")

#define DXINI_3DCAP_TRI_SHADE_ALPHAFLATBLEND       TEXT("TRI_SHADE_AlphaFlatBlend")
#define DXINI_3DCAP_TRI_SHADE_ALPHAFLATSTIPPLED    TEXT("TRI_SHADE_AlphaFlatStippled")
#define DXINI_3DCAP_TRI_SHADE_ALPHAGOURAUDBLEND    TEXT("TRI_SHADE_AlphaGouraudBlend")
#define DXINI_3DCAP_TRI_SHADE_ALPHAGOURAUDSTIPPLED TEXT("TRI_SHADE_AlphaGouraudSippled")
#define DXINI_3DCAP_TRI_SHADE_ALPHAPHONGBLEND      TEXT("TRI_SHADE_AlphaPhongBlend")
#define DXINI_3DCAP_TRI_SHADE_ALPHAPHONGSTIPPLED   TEXT("TRI_SHADE_AlphaPhongStippled")
#define DXINI_3DCAP_TRI_SHADE_COLORFLATMONO        TEXT("TRI_SHADE_ColourFlatMono")
#define DXINI_3DCAP_TRI_SHADE_COLORFLATRGB         TEXT("TRI_SHADE_ColourFlatRGB")
#define DXINI_3DCAP_TRI_SHADE_COLORGOURAUDMONO     TEXT("TRI_SHADE_ColourGouraudBlend")
#define DXINI_3DCAP_TRI_SHADE_COLORGOURAUDRGB      TEXT("TRI_SHADE_ColourGouraudRGB")
#define DXINI_3DCAP_TRI_SHADE_COLORPHONGMONO       TEXT("TRI_SHADE_ColourPhongBlend")
#define DXINI_3DCAP_TRI_SHADE_COLORPHONGRGB        TEXT("TRI_SHADE_ColourPhongRGB")
#define DXINI_3DCAP_TRI_SHADE_FOGFLAT              TEXT("TRI_SHADE_FogFlat")
#define DXINI_3DCAP_TRI_SHADE_FOGGOURAUD           TEXT("TRI_SHADE_FogGouraud")
#define DXINI_3DCAP_TRI_SHADE_FOGPHONG             TEXT("TRI_SHADE_FogPhong")
#define DXINI_3DCAP_TRI_SHADE_SPECULARFLATMONO     TEXT("TRI_SHADE_SpecularFlatMono")
#define DXINI_3DCAP_TRI_SHADE_SPECULARFLATRGB      TEXT("TRI_SHADE_SpecularFlatRGB")
#define DXINI_3DCAP_TRI_SHADE_SPECULARGOURAUDMONO  TEXT("TRI_SHADE_SpecularGouraudMono")
#define DXINI_3DCAP_TRI_SHADE_SPECULARGOURAUDRGB   TEXT("TRI_SHADE_SpecularGouraudRGB")
#define DXINI_3DCAP_TRI_SHADE_SPECULARPHONGMONO    TEXT("TRI_SHADE_SpecularPhongMono")
#define DXINI_3DCAP_TRI_SHADE_SPECULARPHONGRGB     TEXT("TRI_SHADE_SpecularPhongRGB")

#define DXINI_3DCAP_TRI_TXTR_ALPHA                 TEXT("TRI_TXTR_Alpha")
#define DXINI_3DCAP_TRI_TXTR_ALPHAPALETTE          TEXT("TRI_TXTR_AlphaPalette")
#define DXINI_3DCAP_TRI_TXTR_BORDER                TEXT("TRI_TXTR_Border")
#define DXINI_3DCAP_TRI_TXTR_NONPOW2CONDITIONAL    TEXT("TRI_TXTR_NonPow2Conditional")
#define DXINI_3DCAP_TRI_TXTR_PERSPECTIVE           TEXT("TRI_TXTR_Perspective")
#define DXINI_3DCAP_TRI_TXTR_POW2                  TEXT("TRI_TXTR_Pow2")
#define DXINI_3DCAP_TRI_TXTR_SQUAREONLY            TEXT("TRI_TXTR_SquareOnly")
#define DXINI_3DCAP_TRI_TXTR_TEXREPEATNOTSCALEDBYSIZE  TEXT("TRI_TXTR_TexRepeatNotScaledBySize")
#define DXINI_3DCAP_TRI_TXTR_TRANSPARENCY          TEXT("TRI_TXTR_Transparency")

#define DXINI_3DCAP_TRI_TFILT_LINEAR               TEXT("TRI_TFILT_Linear")
#define DXINI_3DCAP_TRI_TFILT_LINEARMIPLINEAR      TEXT("TRI_TFILT_LinearMipLinear")
#define DXINI_3DCAP_TRI_TFILT_LINEARMIPNEAREST     TEXT("TRI_TFILT_LinearMipNearest")
#define DXINI_3DCAP_TRI_TFILT_MIPLINEAR            TEXT("TRI_TFILT_MipLinear")
#define DXINI_3DCAP_TRI_TFILT_MIPNEAREST           TEXT("TRI_TFILT_MipNearest")
#define DXINI_3DCAP_TRI_TFILT_NEAREST              TEXT("TRI_TFILT_Nearest")
#define DXINI_3DCAP_TRI_TFILT_MAGFAFLATCUBIC       TEXT("TRI_TFILT_MagFAFlatCubic")
#define DXINI_3DCAP_TRI_TFILT_MAGFANISOTROPIC      TEXT("TRI_TFILT_MagFAnisotropic")
#define DXINI_3DCAP_TRI_TFILT_MAGFGAUSSIANCUBIC    TEXT("TRI_TFILT_MagFGaussianCubic")
#define DXINI_3DCAP_TRI_TFILT_MAGFLINEAR           TEXT("TRI_TFILT_MagFLinear")
#define DXINI_3DCAP_TRI_TFILT_MAGFPOINT            TEXT("TRI_TFILT_MagFPoint")
#define DXINI_3DCAP_TRI_TFILT_MINFANISOTROPIC      TEXT("TRI_TFILT_MinFAnisotropic")
#define DXINI_3DCAP_TRI_TFILT_MINFLINEAR           TEXT("TRI_TFILT_MinFLinear")
#define DXINI_3DCAP_TRI_TFILT_MINFPOINT            TEXT("TRI_TFILT_MinFPoint")
#define DXINI_3DCAP_TRI_TFILT_MIPFLINEAR           TEXT("TRI_TFILT_MipFLinear")
#define DXINI_3DCAP_TRI_TFILT_MIPFPOINT            TEXT("TRI_TFILT_MipFPoint")

#define DXINI_3DCAP_TRI_TBLND_ADD                  TEXT("TRI_TBLND_Add")
#define DXINI_3DCAP_TRI_TBLND_COPY                 TEXT("TRI_TBLND_Copy")
#define DXINI_3DCAP_TRI_TBLND_DECAL                TEXT("TRI_TBLND_Decal")
#define DXINI_3DCAP_TRI_TBLND_DECALALPHA           TEXT("TRI_TBLND_DecalAlpha")
#define DXINI_3DCAP_TRI_TBLND_DECALMASK            TEXT("TRI_TBLND_DecalMask")
#define DXINI_3DCAP_TRI_TBLND_MODULATE             TEXT("TRI_TBLND_Modulate")
#define DXINI_3DCAP_TRI_TBLND_MODULATEALPHA        TEXT("TRI_TBLND_ModulateAlpha")
#define DXINI_3DCAP_TRI_TBLND_MODULATEMASK         TEXT("TRI_TBLND_ModulateMask")

#define DXINI_3DCAP_TRI_TADDR_BORDER               TEXT("TRI_TADDR_Border")
#define DXINI_3DCAP_TRI_TADDR_CLAMP                TEXT("TRI_TADDR_Clamp")
#define DXINI_3DCAP_TRI_TADDR_INDEPENDENTUV        TEXT("TRI_TADDR_IndependentUV")
#define DXINI_3DCAP_TRI_TADDR_MIRROR               TEXT("TRI_TADDR_Mirror")
#define DXINI_3DCAP_TRI_TADDR_WRAP                 TEXT("TRI_TADDR_Wrap")

#define DXINI_3DCAP_TRI_STIPPLEWIDTH               TEXT("TRI_StippleWidth")
#define DXINI_3DCAP_TRI_STIPPLEHEIGHT              TEXT("TRI_StippleHeight")

// dwStencilCaps
#define DXINI_3DCAP_STENCILOP_DECR            TEXT("STENCILOP_Decr")
#define DXINI_3DCAP_STENCILOP_DECRSAT         TEXT("STENCILOP_DecrSat")
#define DXINI_3DCAP_STENCILOP_INCR            TEXT("STENCILOP_Incr")
#define DXINI_3DCAP_STENCILOP_INCRSAT         TEXT("STENCILOP_IncrSat")
#define DXINI_3DCAP_STENCILOP_INVERT          TEXT("STENCILOP_Invert")
#define DXINI_3DCAP_STENCILOP_KEEP            TEXT("STENCILOP_Keep")
#define DXINI_3DCAP_STENCILOP_REPLACE         TEXT("STENCILOP_Replace")
#define DXINI_3DCAP_STENCILOP_ZERO            TEXT("STENCILOP_Zero")

// dwFVFCaps 
#define DXINI_3DCAP_FVF_DONOTSTRIPELEMENTS    TEXT("FVF_DoNotStripElements")
#define DXINI_3DCAP_FVF_TEXCOORDCOUNTMASK     TEXT("FVF_TexCoordCountMask")

// dwTextureOpCaps 
#define DXINI_3DCAP_TEXTUREOP_ADD             TEXT("TEXTUREOP_Add")
#define DXINI_3DCAP_TEXTUREOP_ADDSIGNED       TEXT("TEXTUREOP_AddSigned")
#define DXINI_3DCAP_TEXTUREOP_ADDSIGNED2X     TEXT("TEXTUREOP_AddSigned2X")
#define DXINI_3DCAP_TEXTUREOP_ADDSMOOTH       TEXT("TEXTUREOP_AddSmooth")
#define DXINI_3DCAP_TEXTUREOP_BLENDCURRENTALPHA   TEXT("TEXTUREOP_BlendCurrentAlpha")
#define DXINI_3DCAP_TEXTUREOP_BLENDDIFFUSEALPHA   TEXT("TEXTUREOP_BlendDiffuseAlpha")
#define DXINI_3DCAP_TEXTUREOP_BLENDFACTORALPHA    TEXT("TEXTUREOP_BlendFactorAlpha")
#define DXINI_3DCAP_TEXTUREOP_BLENDTEXTUREALPHA   TEXT("TEXTUREOP_BlendTextureAlpha")
#define DXINI_3DCAP_TEXTUREOP_BLENDTEXTUREALPHAPM TEXT("TEXTUREOP_BlendTextureAlphaPM")
#define DXINI_3DCAP_TEXTUREOP_BUMPENVMAP      TEXT("TEXTUREOP_BumpEnvMap")
#define DXINI_3DCAP_TEXTUREOP_BUMPENVMAPLUMINANCE TEXT("TEXTUREOP_BumpEnvMapLuminance")
#define DXINI_3DCAP_TEXTUREOP_DISABLE         TEXT("TEXTUREOP_Disable")
#define DXINI_3DCAP_TEXTUREOP_DOTPRODUCT3     TEXT("TEXTUREOP_DotProduct3")
#define DXINI_3DCAP_TEXTUREOP_MODULATE        TEXT("TEXTUREOP_Modulate")
#define DXINI_3DCAP_TEXTUREOP_MODULATE2X      TEXT("TEXTUREOP_Modulate2X")
#define DXINI_3DCAP_TEXTUREOP_MODULATE4X      TEXT("TEXTUREOP_Modulate4X")
#define DXINI_3DCAP_TEXTUREOP_MODULATEALPHA_ADDCOLOR    TEXT("TEXTUREOP_ModulateAlpha_AddColour")
#define DXINI_3DCAP_TEXTUREOP_MODULATECOLOR_ADDALPHA    TEXT("TEXTUREOP_ModulateColour_AddAlpha")
#define DXINI_3DCAP_TEXTUREOP_MODULATEINVALPHA_ADDCOLOR TEXT("TEXTUREOP_ModulateInvAlpha_AddColour")
#define DXINI_3DCAP_TEXTUREOP_MODULATEINVCOLOR_ADDALPHA TEXT("TEXTUREOP_ModulateInvColour_AddAlpha")
#define DXINI_3DCAP_TEXTUREOP_PREMODULATE     TEXT("TEXTUREOP_Premodulate")
#define DXINI_3DCAP_TEXTUREOP_SELECTARG1      TEXT("TEXTUREOP_SelectArg1")
#define DXINI_3DCAP_TEXTUREOP_SELECTARG2      TEXT("TEXTUREOP_SelectArg2")
#define DXINI_3DCAP_TEXTUREOP_SUBTRACT        TEXT("TEXTUREOP_Subtract")


extern char g_szDXINIPath[];

// ===========================================================================
// Prototypes ================================================================
// ===========================================================================

INL void  DXINI_Clear();
INL BOOL  DXINI_PathIsValid();

INL BOOL  DXINI_ReadDWORD(LPCTSTR szSection, LPCTSTR szKey, DWORD& dwValue);
INL BOOL  DXINI_WriteDWORD(LPCTSTR szSection, LPCTSTR szKey, DWORD dwValue);

INL BOOL  DXINI_ReadString(LPCTSTR szSection, LPCTSTR szKey, LPSTR szValue,
                           int nSize);
INL BOOL  DXINI_WriteString(LPCTSTR szSection, LPCTSTR szKey, LPSTR szValue);

INL BOOL  DXINI_GetSelectedDriver(DWORD& dwDriver);
INL BOOL  DXINI_GetNumDrivers(DWORD& dwNumDrivers);

INL BOOL  DXINI_GetSelected3DDevice(DWORD& dw3DDevice);
INL BOOL  DXINI_GetNum3DDevices(DWORD dwDriver, DWORD& dwNum3DDevices);

INL BOOL  DXINI_SetSelectedDriver(DWORD dwDriver);
INL BOOL  DXINI_SetSelected3DDevice(DWORD dw3DDevice);
INL BOOL  DXINI_SetSelectedResolution(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP);
INL BOOL  DXINI_SetUsingResolution(BOOL bUsingRes);
INL BOOL  DXINI_SetWindowedMode(BOOL bWindowed);

INL BOOL  DXINI_GetSelectedResolution(DWORD& dwWidth, DWORD& dwHeight, DWORD& dwBPP);
INL BOOL  DXINI_UsingResolution();

void  DXINI_GetFilePath();
BOOL  DXINI_FileExists();

BOOL  DXINI_ReadGUID(LPCTSTR szSection, LPCTSTR szBaseKey, GUID& guid);
BOOL  DXINI_WriteGUID(LPCTSTR szSection, LPCTSTR szBaseKey, const GUID& guid);

BOOL  DXINI_ReadRes(LPCTSTR szSection, LPCTSTR szKey, DWORD& dwWidth,
                    DWORD& dwHeight, DWORD& dwBitDepth);
BOOL  DXINI_WriteRes(LPCTSTR szSection, LPCTSTR szKey, DWORD dwWidth,
                     DWORD dwHeight, DWORD dwBitDepth);

int   DXINI_GetDriverList(DXINI_DriverInfo* aDrivers, int nMaxDrivers);

BOOL  DXINI_ChooseDriver(int nDriver = -1, int n3DDevice = -1,
                         BOOL bWindowed = FALSE);

BOOL  DXINI_CheckForChooseSettings();

BOOL  DXINI_WriteD3DDeviceInfo(const D3DDEVICEDESC& d3dDeviceInfo,
                               LPSTR szDeviceName, LPSTR szDeviceDesc,
                               LPCTSTR szDevice);

// ===========================================================================

BOOL                DXINI_InitDrvSelDlgTemplate(WORD* pTemplate);
BOOL                DXINI_InitDrvSelDlg(HWND hDlg);
BOOL                DXINI_CloseDrvSelDlg(HWND hDlg);
BOOL                DXINI_DrvChangedDrvSelDlg(HWND hDlg);
LRESULT APIENTRY    DrvSelDlgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, 
                                  LPARAM lParam);

// ===========================================================================
// INLINE functions ==========================================================
// ===========================================================================

// ===========================================================================
INL void DXINI_Clear()
{
  if (DXINI_PathIsValid() && DXINI_FileExists())
    ::DeleteFile(g_szDXINIPath);
}

// ===========================================================================
INL BOOL DXINI_PathIsValid()
{
  return(0 != strlen(g_szDXINIPath));
}

// ===========================================================================
INL BOOL DXINI_ReadDWORD(LPCTSTR szSection, LPCTSTR szKey, DWORD& dwValue)
{
  TCHAR szValue[20];
  BOOL bRet = (0 != GetPrivateProfileString(szSection, szKey, "4294967295",
                                            szValue, 20, g_szDXINIPath));
  if (bRet) dwValue = _tcstoul(szValue, NULL, 10);
  return(bRet && 0xFFFFFFFF != dwValue);
}

// ===========================================================================
INL BOOL DXINI_WriteDWORD(LPCTSTR szSection, LPCTSTR szKey, DWORD dwValue)
{
  char szValue[20];
  _ultoa(dwValue, szValue, 10);
  return(WritePrivateProfileString(szSection, szKey, szValue, g_szDXINIPath));
}

// ===========================================================================
INL BOOL DXINI_ReadString(LPCTSTR szSection, LPCTSTR szKey, LPSTR szValue,
                          int nSize)
{
  BOOL bRet = (0 != GetPrivateProfileString(szSection, szKey, "", szValue,
                                            nSize, g_szDXINIPath));
  return(bRet && 0 != strlen(szValue));
}

// ===========================================================================
INL BOOL DXINI_WriteString(LPCTSTR szSection, LPCTSTR szKey, LPSTR szValue)
{
  return(WritePrivateProfileString(szSection, szKey, szValue, g_szDXINIPath));
}

// ===========================================================================
INL BOOL DXINI_GetNumDrivers(DWORD& dwNumDrivers)
{
  return(DXINI_ReadDWORD(DXINI_DDRAWDRIVERSSECTION, DXINI_NUM_DDRAWDRIVERS,
                         dwNumDrivers));
}

// ===========================================================================
INL BOOL DXINI_GetSelectedDriver(DWORD& dwDriver)
{
  return(DXINI_ReadDWORD(DXINI_SETTINGSSECTION, DXINI_SELECTED_DDRAWDRIVER,
                         dwDriver));
}

// ===========================================================================
INL BOOL DXINI_SetSelectedDriver(DWORD dwDriver)
{
  return(DXINI_WriteDWORD(DXINI_SETTINGSSECTION, DXINI_SELECTED_DDRAWDRIVER,
                          dwDriver));
}

// ===========================================================================
INL BOOL DXINI_GetSelected3DDevice(DWORD& dw3DDevice)
{
  return(DXINI_ReadDWORD(DXINI_SETTINGSSECTION, DXINI_SELECTED_3DDEVICE,
                         dw3DDevice));
}

// ===========================================================================
INL BOOL DXINI_GetNum3DDevices(DWORD dwDriver, DWORD& dwNum3DDevices)
{
  // Format the driver name
  TCHAR szSection[MAX_PATH];
  _stprintf(szSection, "%s_%02d", DXINI_DDRAWDRIVER, dwDriver);
  return(DXINI_ReadDWORD(szSection, DXINI_NUM_3DDEVICES,
                         dwNum3DDevices));
}

// ===========================================================================
INL BOOL DXINI_SetSelected3DDevice(DWORD dw3DDevice)
{
  return(DXINI_WriteDWORD(DXINI_SETTINGSSECTION, DXINI_SELECTED_3DDEVICE,
                          dw3DDevice));
}

// ===========================================================================
INL BOOL DXINI_SetSelectedResolution(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP)
{
  return(DXINI_WriteRes(DXINI_SETTINGSSECTION, DXINI_SELECTED_RESOLUTION,
                        dwWidth, dwHeight, dwBPP));
}

// ===========================================================================
INL BOOL DXINI_SetUsingResolution(BOOL bUsingRes)
{
  return(DXINI_WriteDWORD(DXINI_SETTINGSSECTION, DXINI_USE_SELECTED_RESOLUTION,
                          (DWORD)bUsingRes));
}

// ===========================================================================
INL BOOL DXINI_SetWindowedMode(BOOL bWindowed)
{
  return(DXINI_WriteDWORD(DXINI_SETTINGSSECTION, DXINI_WINDOWEDMODE,
                          (DWORD)bWindowed));
}

// ===========================================================================
INL BOOL DXINI_GetSelectedResolution(DWORD& dwWidth, DWORD& dwHeight, DWORD& dwBPP)
{
  return(DXINI_ReadRes(DXINI_SETTINGSSECTION, DXINI_SELECTED_RESOLUTION,
                       dwWidth, dwHeight, dwBPP));
}

// ===========================================================================
INL BOOL DXINI_UsingResolution()
{
  DWORD dwValue = 0;
  DXINI_ReadDWORD(DXINI_SETTINGSSECTION, DXINI_USE_SELECTED_RESOLUTION, dwValue);
  return((BOOL)dwValue);
}

#endif // INC_DXINI
// ===========================================================================
