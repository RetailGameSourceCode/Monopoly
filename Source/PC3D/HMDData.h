// ===========================================================================
// Module:      HMDData.h
//
// Description: 
//
// Created:     July 2, 1999
//
// Author:      
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_HMDDATA
#define INC_HMDDATA

enum HMDPOLYTYPE
{
  HMDPT_TRIANGLE    = 1,
  HMDPTPT_QUAD      = 2,
  HMDPTPT_STRIPMESH = 3
};

enum HMDDATATYPE
{
  HMDDT_POLYGON       = 0,
  HMDDT_SHAREDPOLYGON = 1,
  HMDDT_IMAGE         = 2,
  HMDDT_ANIMATION     = 3,
  HMDDT_MIMe          = 4,
  HMDDT_GROUND        = 5
};

struct HMDPrimType
{
  union
  {
    struct
    {
      BYTE bTextureMapped      : 1;  // 32 bits
      BYTE bUseSeparateColours : 1;
      BYTE bGouraud            : 1;
      BYTE polygonShape        : 3;
      BYTE bHasNoNormal        : 1;
      BYTE bMipMapped          : 1;
      BYTE bUsePresetPacket    : 1;  // 24 bits
      BYTE bTiledTextures      : 1;
      BYTE bMIMe               : 1;
      BYTE reserved0           : 5;

      BYTE bPerformSubdivision : 1;  // 16 bits
      BYTE bEnableFog          : 1;
      BYTE bDisbleLighting     : 1;
      BYTE bAutoDivision       : 1;
      BYTE bDoubleSided        : 1;
      BYTE bForceTransparency  : 1;
      BYTE bEnableClipping     : 1;
      BYTE bInitialize         : 1;

      BYTE category            : 4;  // 8 bits
      BYTE developerID         : 4;
    };
    DWORD   dwType;
  };

  BOOL operator ==(DWORD dwValue) { return(dwType == dwValue); }
};


struct HMDMIMeType
{
  union
  {
    struct
    {
      BYTE code1              : 3;
      BYTE bReset             : 1;
//      BYTE code0              : 3;
      BYTE bJointMIMe         : 1;
      BYTE bVNMIMe            : 1;
      BYTE reserved0          : 1;

      BYTE reserved1          : 1;  // 8  bits
      BYTE reserved2          : 8;  // 16 bits

      BYTE bMIMePrimitive     : 1;  
      BYTE reserved3          : 7;  // 24 bits

      BYTE category           : 4;  
      BYTE developerID        : 4;  // 32 buts
    };

    DWORD dwDiffType;
  };

  BOOL  operator ==(DWORD dwValue)  { return(dwDiffType == dwValue);        }
  DWORD operator =(DWORD dwValue)   { return(dwDiffType = dwValue);         }
};


struct HMDColour
{
  union
  {
    struct
    {
      BYTE  byRed;
      BYTE  byGreen;
      BYTE  byBlue;
      BYTE  byReserved0;
    };
    DWORD dwClrValue;
  };

  DWORD operator=(DWORD dwValue) { dwClrValue = dwValue; return(dwClrValue); }
};

struct HMDTexturePt
{
  union
  {
    struct
    {
      BYTE u;
      BYTE v;
    };
    WORD wPoints;
  };

  WORD operator=(WORD wValue) { wPoints = wValue; return(wPoints); }

};

struct HMDPrimCountSize
{
  union
  {
    struct
    {
      WORD  wSize       : 16;
      WORD  wPolyCount  : 15;
      WORD  bScanFlag   : 1;
    };
    DWORD   dwCountSize;
  };
};

struct HMDPrimitiveHdr
{
  DWORD   dwHdrSize;
  DWORD*  pSection;  // Array of section pointers, one after another.
};

struct HMDPrimitive
{
  HMDPrimitive*     pNextPrim;    // Next primitive
  HMDPrimitiveHdr*  pHeader;      // Header for this primitive
  DWORD             dwTypeCount;  // Number of data sections
};

struct HMDDataHdr
{
  HMDPrimType       primType;
  HMDPrimCountSize  primCountSize;
};

struct HMDMIMeDataHdr
{
  HMDMIMeType       MIMeType;
  HMDPrimCountSize  MIMeCountSize;
};

struct HMDDiffBlock
{
  DWORD*        pVertexDiffBlock;
  DWORD*        pNormalDiffBlock;
  HMDDiffBlock* pNext;

  HMDDiffBlock() { ZeroMemory(this, sizeof(HMDDiffBlock)); }
};

struct HMDTriangle
{
  HMDColour     aColours[3];
  WORD          wTextureID;
  WORD          awVertexIndices[3];
  WORD          awNormalIndices[3];
  HMDTexturePt  aTexturePts[3];
};

struct HMDHeader
{
  DWORD             dwVersionID;
  DWORD             dwMapFlag;
  HMDPrimitiveHdr*  pPrimitiveHdrSection; // pointer to primitive header section
  DWORD             dwNumBlocks; // Number of sections/blocks.  
};

struct TIMHeader
{
  union
  {
    struct 
    {
      BYTE id         : 4;
      BYTE version    : 4;
      WORD wReserved0 : 16;
    };
    DWORD dwID;
  };

  union
  {
    struct
    {
      BYTE  pixelMode   : 3;
      BYTE  clutFlag    : 1;
      DWORD dwReserved  : 28;
    };
    DWORD dwFlag;
  };
};


struct CLUTEntry
{
  union
  {
    struct
    {
      WORD red            : 5;
      WORD green          : 5;
      WORD blue           : 5;
      WORD bTransparency  : 1;  // 16 Bits
    };
    WORD wClutEntry;
  };

  WORD operator=(WORD wValue) { wClutEntry = wValue; return(wClutEntry); }
};


#endif // INC_HMDDATA
// ===========================================================================
