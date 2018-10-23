// ===========================================================================
// Module:      Vertex.h
//
// Description: 
//
// Created:     July 5, 1999
//
// Author:      
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_VERTEX
#define INC_VERTEX

#define VTXCOLOUR   ( D3DFVF_DIFFUSE | D3DFVF_SPECULAR )
#define VTXTEXTURE  ( D3DFVF_TEX1 )

// No texture, no colour
#define VTXFORMAT   ( D3DFVF_XYZ | D3DFVF_NORMAL )

// Untextured, coloured
#define VTX_CLR  ( VTXFORMAT | VTXCOLOUR )

// Textured, uncoloured
#define VTX_TXTR  ( VTXFORMAT | VTXTEXTURE )

// Textured, coloured
#define VTX_CLRTXTR  ( VTXFORMAT | VTXCOLOUR | VTXTEXTURE )

// ===========================================================================
// Vertex structure ==========================================================
// ===========================================================================
struct Vertex
{
  // Member variables ========================================================
  D3DVECTOR   vPos;       // Position of the vertex
  D3DVECTOR   vNormal;    // Normal of the vertex
  DWORD       dwDiffuse;  // Diffuse colour component of the vertex
  DWORD       dwSpecular; // Specular colour component of the vertex
  float       fU, fV;     // U, V texture coordinates of the vertex


  // Constructors ============================================================
  INL Vertex();

  INL Vertex(const D3DVECTOR& pos, const D3DVECTOR& norm, float u, float v,
             DWORD dwDiff = 0xFFFFFFFF, DWORD dwSpec = 0x00000000);
  
  INL Vertex(const D3DVECTOR& pos, DWORD dwDiff, DWORD dwSpec, float u, float v,
             const D3DVECTOR& norm = D3DVECTOR(0, 0, 0));
  
  INL Vertex(float x, float y, float z, float nx, float ny, float nz, float u,
             float v, DWORD dwDiff = 0xFFFFFFFF, DWORD dwSpec = 0x00000000);
  
  INL Vertex(float x, float y, float z, DWORD dwDiff, DWORD dwSpec, float u,
             float v, float nx = 0.0f, float ny = 0.0f, float nz = 0.0f);

  INL BOOL operator==(const Vertex& vtx) const;

  INL BOOL PartialEquals(const Vertex& vtx) const;
  INL void Reset();

};

// ===========================================================================
// INLINE: Vertex functions ==================================================
// ===========================================================================

// ===========================================================================
INL Vertex::Vertex() :  vPos(0, 0, 0), vNormal(0, 0, 0), dwDiffuse(0xFFFFFFFF),
                        dwSpecular(0), fU(0), fV(0)
{                                                                           }

// ===========================================================================
INL Vertex::Vertex(const D3DVECTOR& pos, const D3DVECTOR& norm, float u, float v,
                   DWORD dwDiff /*= 0xFFFFFFFF*/, DWORD dwSpec /*= 0x00000000*/)
          :  vPos(pos), vNormal(norm), dwDiffuse(dwDiff), dwSpecular(dwSpec),
             fU(u), fV(v)
{                                                                           }

// ===========================================================================
INL Vertex::Vertex(const D3DVECTOR& pos, DWORD dwDiff, DWORD dwSpec,
                   float u, float v,
                   const D3DVECTOR& norm /*= D3DVECTOR(0, 0, 0)*/)
          :  vPos(pos), vNormal(norm), dwDiffuse(dwDiff), dwSpecular(dwSpec),
             fU(u), fV(v)
{                                                                           }

// ===========================================================================
INL Vertex::Vertex(float x, float y, float z, float nx, float ny, float nz,
                   float u, float v, DWORD dwDiff /*= 0xFFFFFFFF*/,
                   DWORD dwSpec /*= 0x00000000*/)
          :  vPos(x, y, z), vNormal(nx, ny, nz), dwDiffuse(dwDiff),
             dwSpecular(dwSpec), fU(u), fV(v)
{                                                                           }
  
// ===========================================================================
INL Vertex::Vertex(float x, float y, float z, DWORD dwDiff, DWORD dwSpec,
                   float u, float v, float nx /*= 0.0f*/, float ny /*= 0.0f*/,
                   float nz /*= 0.0f*/)
          :  vPos(x, y, z), vNormal(nx, ny, nz), dwDiffuse(dwDiff),
             dwSpecular(dwSpec), fU(u), fV(v)
{                                                                           }

// ===========================================================================
INL BOOL Vertex::operator==(const Vertex& vtx) const
{ return(0 == memcmp(&vtx, this, sizeof(Vertex)));                          }

// ===========================================================================
INL BOOL Vertex::PartialEquals(const Vertex& vtx) const
{ return(0 == memcmp(&vtx, this, sizeof(Vertex) - (2 * sizeof(float))));    }

// ===========================================================================
INL void Vertex::Reset()
{ ZeroMemory(this, sizeof(Vertex)); dwDiffuse = 0xFFFFFFFF;                 }

#endif // INC_VERTEX
// ===========================================================================
