// ===========================================================================
// Module:      NewMesh.cpp
//
// Description: Implementation of the Mesh class.  This class represents 3D
//              objects.  Can also contain interpolation data for smooth
//              animation.
//
// Created:     July 8, 1999
//
// Author:      Dave Wilson
//
// Copywrite:   Artech, 1999
// ===========================================================================


#include "PC3DHdr.h"
#include "NewMesh.h"
#include "l_Material.h"
#include "GlobalData.h"
#include "D3DDevice.h"

#ifndef USE_OLD_FRAME

#define GsOT int
#include "sys\types.h"
#include "libgte.h"
#include "libgpu.h"
#include "libgs.h"
#include "libhmd.h"

#define BYTE3(x)            (BYTE)(((x) & 0xFF000000) >> 24)
#define BYTE2(x)            (BYTE)(((x) & 0x00FF0000) >> 16)
#define BYTE1(x)            (BYTE)(((x) & 0x0000FF00) >> 8)
#define BYTE0(x)            (BYTE)(x)

#define FIXEDTOFLOAT(fixed) ((float)(fixed) / 4096.0f)


typedef Array<TextureID*, TextureID*>   TexturePtrArray;
static TexturePtrArray                  g_apTmpTextures;
static BOOL                             g_bPurgeTextures = TRUE;


// ===========================================================================
// Mesh class ================================================================
// ===========================================================================


// Constructor ===============================================================
Mesh::Mesh()
{
  // Clear the stride settings
  ZeroMemory(&m_StrideData, sizeof(D3DSTRIDEDATA));

  // No matter how many times we change the stride pointers, the size of the
  //  stride will not change
  m_StrideData.position.dwStride          = sizeof(Vertex);
  m_StrideData.normal.dwStride            = sizeof(Vertex);
  m_StrideData.diffuse.dwStride           = sizeof(Vertex);
  m_StrideData.specular.dwStride          = sizeof(Vertex);
  m_StrideData.textureCoords[0].dwStride  = sizeof(Vertex);

  // Create ourselves a vertex array
  m_pVertices = new VertexArray;
  ASSERT(NULL != m_pVertices);
  m_apVertexSets.Add(m_pVertices);

  m_pDiffBlocks     = NULL;
  m_nNumDiffBlocks  = m_nNumVtxDiffBlocks = m_nNumNormDiffBlocks = 0;
  m_dwSrcBlend      = m_dwDestBlend = 0;
  m_byZBias         = 0;
}


// Destructor ================================================================
Mesh::~Mesh()
{
  // Destroy all of the mesh groups
  for (int i = 0; i < m_apGroups.GetSize(); i++)
    delete m_apGroups.GetAt(i);

  // Destroy all of the vertex sets
  for (i = 0; i < m_apVertexSets.GetSize(); i++)
    delete m_apVertexSets.GetAt(i);
}


// ===========================================================================
// PRIVATE: Mesh member functions ============================================
// ===========================================================================


// ===========================================================================
// PROTECTED: Mesh member functions ==========================================
// ===========================================================================


// ===========================================================================
// Function:    ProcessHMDHeader
//
// Description: Processes the header information for an HMD file.
//
// Params:      pData:  Pointer to the data block having the header info.
//
// Returns:     HMDHeader*
//
// Comments:    Returns a pointer to the processed HMDHeader structure if
//              successful, or NULL if something went wrong.
// ===========================================================================
HMDHeader* Mesh::ProcessHMDHeader(DWORD* pData)
{
  // We need to scan through the data and map the pointers to their proper
  //  location

  HMDHeader* pHeader = (HMDHeader*)pData;
  ASSERT(NULL != pHeader); if (!pHeader) return(NULL);

  DWORD   dwNumBlocks       = 0;
  DWORD   dwNumPointers     = 0;
  DWORD*  pdwCurrentPointer = NULL;

  // Check if we've already mapped this data
  if (0 != pHeader->dwMapFlag) return(NULL);

  pHeader->dwMapFlag = 1;
  
  // Now, map the pointers

  // Do the prim header section
  pHeader->pPrimitiveHdrSection = 
    (HMDPrimitiveHdr*)(pData + (DWORD)(pHeader->pPrimitiveHdrSection));

  pdwCurrentPointer = (DWORD*)pHeader->pPrimitiveHdrSection;

  //The number of primitive headers
  dwNumBlocks = *pdwCurrentPointer;

  //Advance the pointer.
  ++pdwCurrentPointer;

  for (DWORD i = 0; i < dwNumBlocks; i++)
  {
    // The number of words in this block that must be converted to pointers
    dwNumPointers = *pdwCurrentPointer;
    pdwCurrentPointer++;
    for (DWORD j = 0; j < dwNumPointers; j++)
    {
      // Check to see if we've been mapped (if the high bit is set, we've not
      //  been mapped)
      if (*pdwCurrentPointer & 0x80000000)
        *pdwCurrentPointer = (DWORD)(pData + (*pdwCurrentPointer & 0x7FFFFFFF));
      pdwCurrentPointer++;
    }
  }

  return(pHeader);
}


// ===========================================================================
// Function:    ProcessHMDPrimitive
//
// Description: Processes the data for a primitive contained in an HMD file.
//
// Params:      pPrimitive: Pointer to the primitive
//              pHeader:    Pointer to the HMDHeader
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Mesh::ProcessHMDPrimitive(HMDPrimitive* pPrimitive, HMDHeader* pHeader)
{
  DWORD   dwIncrement = 0;
  DWORD*  pdwData     = NULL;

  // Are we at the last primitive in the chain?
  if ((DWORD)(pPrimitive->pNextPrim) != 0xFFFFFFFF)
  {
    // We aren't at the last primitive yet, so recurse to the end so we will
    //  end up loading the last primitive first.
    pPrimitive->pNextPrim = (HMDPrimitive*)((DWORD*)pHeader + 
                            (DWORD)pPrimitive->pNextPrim);
    ProcessHMDPrimitive(pPrimitive->pNextPrim, pHeader);
  }

  // Set the header point
  pPrimitive->pHeader = (HMDPrimitiveHdr*)((DWORD*)pHeader + 
                        (DWORD)pPrimitive->pHeader);

  if (0x80000000 & pPrimitive->dwTypeCount)
  {
    pPrimitive->dwTypeCount &= 0x7FFFFFFF;

    //We haven't mapped this one yet
    //Forward the pointer so it points at the first type
    pdwData = (DWORD*)(pPrimitive + 1);

    // Now process the data blocks for the primitive
    for (DWORD i = 0; i < pPrimitive->dwTypeCount; i++)
    {
      ProcessHMDPrimitiveData(pdwData, (DWORD*)(pPrimitive->pHeader));
      pdwData += ((HMDDataHdr*)pdwData)->primCountSize.wSize + 1;
    }
  }
}


// ===========================================================================
// Function:    ProcessHMDPrimitiveData
//
// Description: Processes the data for a primitive.
//
// Params:      pData:    Pointer to the primitive data
//              pHeader:  Pointer to the header of the primitive
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Mesh::ProcessHMDPrimitiveData(DWORD* pData, DWORD* pHeader)
{
  HMDDataHdr* pDataHdr  = (HMDDataHdr*)pData;

  // Check to make sure we haven't loaded this data set already
  if (!pDataHdr->primCountSize.bScanFlag) return;

  // Determine what kind of data we need to load
  switch (pDataHdr->primType.category)
  {
    case HMDDT_POLYGON:
      switch (pDataHdr->primType.polygonShape)
      {
        case HMDPT_TRIANGLE:
          ProcessHMDTriangle(pData, (HMDPrimitiveHdr*)pHeader);
          break;

        default:
          TRACE("Attempted to load an unsupported polytype\n");
          break;
      }
      break;

    // Load an image
    case HMDDT_IMAGE: 
      if (pDataHdr->primType.bTextureMapped)
        ProcessHMDImage(pData, pHeader);
      break;

    case HMDDT_MIMe:
      ProcessHMDMIMe(pData, pHeader);
      break;
  }

  // Make sure we don't load this data set again
  pDataHdr->primCountSize.bScanFlag = 0;
}


// ===========================================================================
// Function:    ProcessHMDTriangle
//
// Description: Processes the triangle data of a primitive.
//
// Params:      pData:    Pointer to the data blocks of the primitve
//              pPrimHdr: Pointer to the header of the primitive
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Mesh::ProcessHMDTriangle(DWORD* pData, HMDPrimitiveHdr* pPrimHdr)
{
  HMDDataHdr* pDataHdr      = (HMDDataHdr*)pData;
  DWORD*      pPointer      = pPrimHdr->pSection + *( pData + 2 );
  DWORD       dwNumClrs     = 1;
  
  HMDTriangle triangle;
  ZeroMemory(&triangle, sizeof(HMDTriangle));

  // If we have already loaded this triangle, we don't want to do so again
  if (!pDataHdr->primCountSize.bScanFlag)
    return;

  // Mask out the initialize bit
  pDataHdr->primType.bInitialize = 0;

  for (DWORD i = 0; i < pDataHdr->primCountSize.wPolyCount; i++)
  {
    // If this triangle has tiling texture info, we'll skip it for now
    if (pDataHdr->primType.bTiledTextures)
      pPointer++;

    // Figure out how many colour values we need to read
    if (!(pDataHdr->primType.bTextureMapped && 
        !pDataHdr->primType.bUseSeparateColours))
    {
      dwNumClrs = 1;
      if (pDataHdr->primType.bUseSeparateColours) dwNumClrs = 3;

      // Get the colour values
      CopyMemory(triangle.aColours, pPointer, sizeof(HMDColour) * dwNumClrs);
      pPointer += dwNumClrs;

      // If we only read one colour, make sure the other colour values of the
      //  triangle are set accordingly
      if (1 == dwNumClrs)
        triangle.aColours[2] = triangle.aColours[1] = triangle.aColours[0];
    }
    else
    {
      // There is no colour information, default to white
      triangle.aColours[0] = triangle.aColours[1] = 
      triangle.aColours[2] = 0x00FFFFFF;
    }

    // Does the triangle have texture information
    if (pDataHdr->primType.bTextureMapped)
    {
      // Read the texture blook (three DWORDs)
      triangle.aTexturePts[0] = LOWORD(*pPointer);
      pPointer++;
      triangle.wTextureID     = HIWORD(*pPointer);
      triangle.aTexturePts[1] = LOWORD(*pPointer);
      pPointer++;
      triangle.aTexturePts[2] = LOWORD(*pPointer);
      pPointer++;
    }
    else
      triangle.wTextureID     = 0xFFFF;

    // Is this triangle gouraud shaded?
    if (pDataHdr->primType.bGouraud)
    {
      // Read three vertices with three different normals
      triangle.awVertexIndices[0] = HIWORD(*pPointer);
      triangle.awNormalIndices[0] = LOWORD(*pPointer);
      pPointer++;
      triangle.awVertexIndices[1] = HIWORD(*pPointer);
      triangle.awNormalIndices[1] = LOWORD(*pPointer);
      pPointer++;
      triangle.awVertexIndices[2] = HIWORD(*pPointer);
      triangle.awNormalIndices[2] = LOWORD(*pPointer);
    }
    else
    {
      // Read three vertices with one normal
      triangle.awVertexIndices[0] = HIWORD(*pPointer);
      triangle.awNormalIndices[0] = LOWORD(*pPointer);
      triangle.awNormalIndices[1] = triangle.awNormalIndices[0];
      triangle.awNormalIndices[2] = triangle.awNormalIndices[0];
      pPointer++;
      triangle.awVertexIndices[1] = LOWORD(*pPointer);
      triangle.awVertexIndices[2] = HIWORD(*pPointer);
    }

    // Now take the triangle info and make a face out of it
    AddTriangle(triangle, pPrimHdr);

    // Move to the next triangle
    pPointer++;
  }
}


// ===========================================================================
// Function:    AddTriangle
//
// Description: Takes an HMD triangle and converts it to mesh components
//
// Params:      triangle:   The triangle we want to convert
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Mesh::AddTriangle(const HMDTriangle& triangle, HMDPrimitiveHdr* pPrimHdr)
{
  int       nTextureIdx   = -1;
  POINT     ptTopLeft     = { 0, 0 };
  TextureID*  pTextureID  = NULL;

  // Find our texture, if this is a texture mapped triangle
  if (0xFFFF != triangle.wTextureID)
  {
    pTextureID = FindTexture(triangle.aTexturePts[0].u,
                             triangle.aTexturePts[0].v, triangle.wTextureID);

    if (!pTextureID)
    {
//      TRACE("Unable to find texture\n");
      return;
    }

    // Get the top left point of the texture
    // Make sure the coords are masked properly for 16bit pixels.
    // Left shifting by one is the same as multiplying by 2.
    // (Need to multiply by 2 for 8 bit textures)
    ptTopLeft.x = (pTextureID->wXPos & 0x3F) << 1;
    ptTopLeft.y = pTextureID->wYPos & 0xFF;
  }

  // Find the a mesh group to add these vertices to
  MeshGroup* pGroup = FindMeshGroup(pTextureID, triangle.aColours[0]);

  // If we didn't find a group to insert this triangle into, create a new one
  if (!pGroup)
    pGroup = CreateNewMeshGroup(pTextureID, triangle.aColours[0]);

  ASSERT(NULL != pGroup);

  WORD      wVertexIdx  = 0;
  WORD      wNormalIdx  = 0;
  int       nVtxIdx     = -1;
  SVECTOR*  pVertices   = (SVECTOR*)*(((DWORD*)pPrimHdr) + 2);
  SVECTOR*  pNormals    = (SVECTOR*)*(((DWORD*)pPrimHdr) + 3);
  Vertex    newVertex;

  // Convert and add the three vertices
  for (int i = 0; i < 3; i++)
  {
    wVertexIdx = triangle.awVertexIndices[i];
    wNormalIdx = triangle.awNormalIndices[i];

    // Vertices
    newVertex.vPos.x =  float(pVertices[wVertexIdx].vx);
    newVertex.vPos.y = -float(pVertices[wVertexIdx].vy);
    newVertex.vPos.z =  float(pVertices[wVertexIdx].vz);

    // Normals
    newVertex.vNormal.x =  FIXEDTOFLOAT(pNormals[wNormalIdx].vx);
    newVertex.vNormal.y = -FIXEDTOFLOAT(pNormals[wNormalIdx].vy);
    newVertex.vNormal.z =  FIXEDTOFLOAT(pNormals[wNormalIdx].vz);

    // Texture points
    if (pTextureID)
    {
      newVertex.fU = float(triangle.aTexturePts[i].u - ptTopLeft.x) / 
                     float(pTextureID->wWidth);
      newVertex.fV = float(triangle.aTexturePts[i].v - ptTopLeft.y) / 
                     float(pTextureID->wHeight);
    }
    else
      newVertex.fU = newVertex.fV = -1.0f;

    // If the vertex already exits, we don't want to add it twice
    if (-1 != (nVtxIdx = FindVertex(newVertex)))
      pGroup->AddVertex(nVtxIdx);
    else
    {
      // Add the vertex to our list of vertices and add the vertex's index to
      //  the group we found earlier
      nVtxIdx = m_pVertices->Add(newVertex);
      pGroup->AddVertex(nVtxIdx);

      SetSubVertices(nVtxIdx, wVertexIdx, wNormalIdx);
    }
  }
}

int g_nTexCount = 0;

// ===========================================================================
// Function:    ProcessHMDImage
//
// Description: Loads a TIM image from an HMD file.
//
// Params:      pData:    Pointer to the data containing the image data
//              pHeader:  Pointer to the header of the image data
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the image is successfully loaded, or FALSE if
//              it is not.
// ===========================================================================
BOOL Mesh::ProcessHMDImage(DWORD* pData, DWORD* pHeader)
{
  ASSERT(NULL != pData); if (!pData) return(FALSE);
  ASSERT(NULL != pHeader); if (!pHeader) return(FALSE);

  HMDDataHdr*   pDataHdr      = (HMDDataHdr*)pData;
  DWORD*        pdwImageData  = NULL;
  DWORD*        pdwClutData   = NULL;
  DWORD*        pPointer      = pData;
  BITMAPINFO*   pBmpInfo      = NULL;
  HBITMAP       hBitmap       = NULL;
  HDC           hMemDC        = NULL;
  BYTE*         pBits         = NULL;
  int           nIdx          = 0;
  TextureID*    pTextureID    = NULL;
  BYTE          abyBytes[1068];
  CLUTEntry     entry0;
  CLUTEntry     entry1;
  int           knBmpInfoSize = sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD);

  // Small macro to convert a 5 bit colour value to an 8 bit colour value
  // 8.225806f = conversion factor for converting 16 bit colours (5, 5, 5)
  //  to 24 bit colours (8, 8, 8).  Is equal to 255 / 31.
  #define _5BIT_TO_8BIT(value) (BYTE)((float)(value) * 8.225806f)

  pBmpInfo = (BITMAPINFO*)abyBytes;
  ZeroMemory(pBmpInfo, knBmpInfoSize);

  pBmpInfo->bmiHeader.biSize      = sizeof(BITMAPINFOHEADER);
  pBmpInfo->bmiHeader.biPlanes    = 1;
  pBmpInfo->bmiHeader.biBitCount  = 8;

  // Process each image
  for (DWORD j = 0; j < pDataHdr->primCountSize.wPolyCount; j++, pPointer += 6)
  {
    ASSERT(NULL != pPointer);

    pTextureID = new TextureID;
    ASSERT(NULL != pTextureID);

    // Get the pointers to the image data
    pdwImageData      = ((DWORD*)pHeader[1]) + pPointer[4];
    pdwClutData       = ((DWORD*)pHeader[2]) + pPointer[7];

    // Get the position and size of the texture
    pTextureID->wXPos   = LOWORD(pPointer[2]);
    pTextureID->wYPos   = HIWORD(pPointer[2]);
    pTextureID->wWidth  = LOWORD(pPointer[3]) * 2; //in 16 bit pixels...2 8 bit pixels per.
    pTextureID->wHeight = HIWORD(pPointer[3]);

    // Get the texture page index
    pTextureID->wTPageID = getTPage(1, 0, pTextureID->wXPos, pTextureID->wYPos);
    ASSERT(pTextureID->nBank == G_TEXTUREBANK);

    // Have we already loaded this texture?
    if (TextureHasBeenLoaded(pTextureID))
    {
      // We have already loaded this texture, so delete the texture ID and
      //  advance to the next one
      RELEASEPOINTER(pTextureID);
      continue;
    }

    pBmpInfo->bmiHeader.biWidth   = pTextureID->wWidth;
    pBmpInfo->bmiHeader.biHeight  = -((int)pTextureID->wHeight);

    // The texture mapped flag of the primitive type indicates whether the
    //  image has a clut or not.  If it does, we are assuming that it is an
    //  8 bit clut.
    if (pDataHdr->primType.bTextureMapped)
    {
      // Load the clut (colour look-up table / palette)
      for (int i = 0; i < 128; i++)
      {
        // Get the next pair of clut entries
        entry0 = LOWORD(pdwClutData[i]);
        entry1 = HIWORD(pdwClutData[i]);

        // Each of the red, green and blue elements are 5 bits which means that
        //  each element has a range of 0 - 31.  So if we divide each element 
        //  by 31 this will give us the relative percentage for each element.
        //  We can then use this percentage and multiply it by 255 to achieve
        //  a full 24 bit (8, 8, 8) RGB value.
        pBmpInfo->bmiColors[i*2].rgbRed       = _5BIT_TO_8BIT(entry0.red);
        pBmpInfo->bmiColors[i*2].rgbGreen     = _5BIT_TO_8BIT(entry0.green);
        pBmpInfo->bmiColors[i*2].rgbBlue      = _5BIT_TO_8BIT(entry0.blue);
        pBmpInfo->bmiColors[i*2].rgbReserved  = 0;
        pBmpInfo->bmiColors[i*2+1].rgbRed     = _5BIT_TO_8BIT(entry1.red);
        pBmpInfo->bmiColors[i*2+1].rgbGreen   = _5BIT_TO_8BIT(entry1.green);
        pBmpInfo->bmiColors[i*2+1].rgbBlue    = _5BIT_TO_8BIT(entry1.blue);
        pBmpInfo->bmiColors[i*2+1].rgbReserved = 0;
      }
    }

    // Create a memory DC and a DIB section in the DC
    hMemDC  = ::CreateCompatibleDC(NULL);
    hBitmap = ::CreateDIBSection(hMemDC, pBmpInfo, DIB_RGB_COLORS, 
                                 (LPVOID*)&pBits, NULL, NULL);

    ::GdiFlush();

    // Copy the image bits to the DIB
    ::CopyMemory(pBits, (BYTE*)pdwImageData,
                 pTextureID->wWidth * pTextureID->wHeight);

    // Create ourselves a new texture
    pTextureID->pTexture = new Surface(hBitmap, TRUE);
    g_apTmpTextures.Add(pTextureID);

TRACE("Adding new texture number %d: (%d, %d, %d, %d, %d, %d)\n", 
      ++g_nTexCount, pTextureID->wXPos, pTextureID->wYPos,
      pTextureID->wWidth, pTextureID->wHeight, pTextureID->wTPageID,
      pTextureID->nBank);

//char szFile[MAX_PATH];
//sprintf(szFile, ".\\Textures\\Texture%03d.bmp", g_nTexCount);
//pTextureID->pTexture->SaveToBmpFile(szFile);

    // Remove our GDI resources
    ::DeleteObject(hBitmap);
    ::DeleteDC(hMemDC);

  }

  return(TRUE);

#undef _5BIT_TO_8BIT
}


// ===========================================================================
// Function:    ProcessHMDMIMe
//
// Description: 
//
// Params:      pData:
//              pHeader:
//
// Returns:     
//
// Comments:    
// ===========================================================================
void Mesh::ProcessHMDMIMe(DWORD* pData, DWORD* pHeader)
{
  HMDMIMeDataHdr* pMIMeHdr = (HMDMIMeDataHdr*)pData;
  ASSERT(NULL != pMIMeHdr);

  // Make sure this is the correct category, and that this is a MIMe primitive
  ASSERT(4 == pMIMeHdr->MIMeType.category);
  ASSERT(pMIMeHdr->MIMeType.bMIMePrimitive);

  // We don't currently support resetting of MIMe types
  if (pMIMeHdr->MIMeType.bReset) return;

  // We currently only support vertex/normal MIMe types
  if (pMIMeHdr->MIMeType.bVNMIMe)
    ProcessHMDDiffBlock(pData, pHeader, (BOOL)pMIMeHdr->MIMeType.code1);
}


// ===========================================================================
// Function:    ProcessHMDDiffBlock
//
// Description: 
//
// Params:      pData:
//              pHeader:
//              bNormalBlock:
//
// Returns:     
//
// Comments:    
// ===========================================================================
void Mesh::ProcessHMDDiffBlock(DWORD* pData, DWORD* pHeader, BOOL bNormalBlock)
{
  HMDMIMeDataHdr* pMIMeHdr        = (HMDMIMeDataHdr*)pData;
  DWORD*          pBlockBase      = (DWORD*)pHeader[4];
  DWORD*          pStart          = pData + 2;
  DWORD*          pCurrentBlock   = NULL;
  WORD            wNumDiffBlocks  = 0;
  HMDDiffBlock*   pNewBlock       = NULL;

  for (int i = 0; i < pMIMeHdr->MIMeCountSize.wPolyCount; i++)
  {
    pCurrentBlock = pBlockBase + pStart[i];
    wNumDiffBlocks = bNormalBlock ? LOWORD(pCurrentBlock[0]) : 
                                    HIWORD(pCurrentBlock[0]);

    for (int j = 0; j < wNumDiffBlocks; j++)
    {
      pNewBlock = NewDiffBlock(bNormalBlock);
      if (bNormalBlock)
        pNewBlock->pNormalDiffBlock = pBlockBase + pCurrentBlock[2 + j];
      else
        pNewBlock->pVertexDiffBlock = pBlockBase + pCurrentBlock[2 + j];
    }
  }

}


// ===========================================================================
// Function:    NewDiffBlock
//
// Description: Creates a new diff block
//
// Params:      bNormalBlock: Indicates if we are requesting a new normal
//                            diff block (TRUE) or a new vertex diff 
//                            block (FALSE)
//
// Returns:     HMDDiffBlock*
//
// Comments:    Returns a pointer to the new diff block.
// ===========================================================================
HMDDiffBlock* Mesh::NewDiffBlock(BOOL bNormalBlock)
{
  HMDDiffBlock* pTmpBlock = m_pDiffBlocks;
  int* pnNumBlocks = bNormalBlock ? &m_nNumNormDiffBlocks : 
                                    &m_nNumVtxDiffBlocks;
                                    
  // If we have already allocated space for the other type of diff block, then
  // we don't need to allocate a new one.
  if (*pnNumBlocks < m_nNumDiffBlocks)
  {
    for (int i = 0; i < *pnNumBlocks; i++)
      pTmpBlock = pTmpBlock->pNext;
  }
  else
  {
    // We didn't have enough blocks, so make a new one
    pTmpBlock = new HMDDiffBlock;
    ASSERT(NULL != pTmpBlock);

    // Add it to the head of the list
    if (m_pDiffBlocks)
      pTmpBlock->pNext = m_pDiffBlocks;

    m_pDiffBlocks = pTmpBlock;

    // Increase the number of diff blocks
    m_nNumDiffBlocks++;
  }

  // Increase our number of blocks
  (*pnNumBlocks)++;

  return(pTmpBlock);
}


// ===========================================================================
// Function:    SetSubVertices
//
// Description: 
//
// Params:      nVertex:
//              wHMDVtxIdx:
//              wHMDNormIdx:
//
// Returns:     
//
// Comments:    
// ===========================================================================
void Mesh::SetSubVertices(int nVertex, WORD wHMDVtxIdx, WORD wHMDNormIdx)
{
  DWORD         dwDiffIdx   = 0;
  SVECTOR*      pDiffs      = NULL;
  HMDDiffBlock* pDiffBlock  = m_pDiffBlocks;
  Vertex        subVertex;

  if (m_nNumDiffBlocks + 1 < m_apVertexSets.GetSize())
  {
    // We must add more vertex arrays to the vertex set so we have enough to
    //  hold all of the sub vertices
    for (int i = 0; i < m_nNumDiffBlocks - m_apVertexSets.GetSize() + 1; i++)
      m_apVertexSets.Add(new VertexArray);
  }

  for (int i = 0; i < m_nNumDiffBlocks; ++i)
  {
    // Reset our subvertex
    subVertex.Reset();

    if (pDiffBlock->pVertexDiffBlock)
    {
      // Get the vertex diff info
      pDiffs    = (SVECTOR*)(pDiffBlock->pVertexDiffBlock + 2);
      dwDiffIdx = wHMDVtxIdx - pDiffBlock->pVertexDiffBlock[0];;

      if (dwDiffIdx < HIWORD(pDiffBlock->pVertexDiffBlock[1]))
      {
        subVertex.vPos.x =  float(pDiffs[dwDiffIdx].vx);
        subVertex.vPos.y = -float(pDiffs[dwDiffIdx].vy);
        subVertex.vPos.z =  float(pDiffs[dwDiffIdx].vz);
      }
    }

    if (pDiffBlock->pNormalDiffBlock)
    {
      // Get the normal diff info
      pDiffs    = (SVECTOR*)(pDiffBlock->pNormalDiffBlock + 2);
      dwDiffIdx = wHMDNormIdx - pDiffBlock->pNormalDiffBlock[0];

      if (dwDiffIdx < HIWORD(pDiffBlock->pNormalDiffBlock[1]))
      {
        subVertex.vNormal.x =  float(pDiffs[dwDiffIdx].vx);
        subVertex.vNormal.y = -float(pDiffs[dwDiffIdx].vy);
        subVertex.vNormal.z =  float(pDiffs[dwDiffIdx].vz);
      }
    }

    // Add the sub vertex
    m_apVertexSets.GetAt(i)->SetAtGrow(nVertex, subVertex);

    // Get the next diff block
    pDiffBlock = pDiffBlock->pNext;
  }
}


// ===========================================================================
// Function:    FindTexture
//
// Description: Searches the array of textures for one fitting the location
//              info provided.
//
// Params:      byXPos:   The X position of a location in texture memory
//              byYPos:   The Y position of a location in texture memory
//              wTPageID: The texture page ID.
//
// Returns:     TextureID*
//
// Comments:    Returns a pointer to the TextureID if the texture is found, or
//              NULL if no texture is found at that location.  All of the
//              parameters are based on the PSX-based texture memory setup.
// ===========================================================================
TextureID* Mesh::FindTexture(BYTE byXPos, BYTE byYPos, WORD wTPageID)
{
  BYTE byTmpX = 0;
  BYTE byTmpY = 0;

  TextureID* pTextureID = NULL;

  // Search though our global list of textures for one having the appropriate
  //  properties
  for (int i = g_apTmpTextures.GetUpperBound(); i >= 0; i--)
  {
    pTextureID = g_apTmpTextures.GetAt(i);

    if (wTPageID == pTextureID->wTPageID && G_TEXTUREBANK == pTextureID->nBank)
    {
      byTmpX = (pTextureID->wXPos & 0x3F) << 1;
      byTmpY = pTextureID->wYPos & 0xFF;

      if (byXPos >= byTmpX && byXPos <= byTmpX + pTextureID->wWidth &&
          byYPos >= byTmpY && byYPos <= byTmpY + pTextureID->wHeight)
      {
        return(pTextureID);
      }
    }
  }

  return(NULL);
}


// ===========================================================================
// Function:    TextureHasBeenLoaded
//
// Description: Checks if the given texture has already been loaded
//
// Params:      pTextureID: The texture ID we are checking
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the texture already exists, FALSE otherwise.
// ===========================================================================
BOOL Mesh::TextureHasBeenLoaded(TextureID* pTextureID)
{
  for (int i = 0; i < g_apTmpTextures.GetSize(); i++)
  {
    if (g_apTmpTextures.GetAt(i)->operator==(*pTextureID))
      return(TRUE);
  }
  return(FALSE);
}


// ===========================================================================
// Function:    FindMeshGroup
//
// Description: Finds a mesh group having the given texture index and colour
//
// Params:      nTextureIdx:  Texture index we are looking for
//              clr:          Material colour we are looking for
//
// Returns:     MeshGroup*
//
// Comments:    Returns a pointer to a MeshGroup if one is found, or NULL if
//              no matching group exists.
// ===========================================================================
MeshGroup* Mesh::FindMeshGroup(TextureID* pTextureId, const HMDColour& clr)
{
  int nSize         = m_apGroups.GetSize();
  MeshGroup* pGroup = NULL;

  // Create ourselves a colour value
  CLRVALUE clrValue(float(clr.byRed)    / 255.0f,
                    float(clr.byGreen)  / 255.0f,
                    float(clr.byBlue)   / 255.0f, 1.0f);

  // Search through all of the groups
  for (int i = 0; i < nSize; i++)
  {
    pGroup = m_apGroups.GetAt(i);
    if (pGroup->GetTexture() == pTextureId && 
        clrValue == G_MATERIALS.GetAt(pGroup->GetMaterialIdx())->GetDiffuse())
    {
      // We found one that matches, return it
      return(pGroup);
    }
  }

  return(NULL);
}


// ===========================================================================
// Function:    CreateNewMeshGroup
//
// Description: Creates a new mesh group
//
// Params:      nTextureIdx:  The index of the texture to use for this group
//              clr:          An HMDColour structure used to create a new
//                            material if necessary
//
// Returns:     MeshGroup*
//
// Comments:    Returns a pointer to the new MeshGroup
// ===========================================================================
MeshGroup* Mesh::CreateNewMeshGroup(TextureID* pTextureID, const HMDColour& clr)
{
  // Initialize a colour value with the HMDColour that was passed in
  CLRVALUE clrValue(float(clr.byRed)    / 255.0f,
                    float(clr.byGreen)  / 255.0f,
                    float(clr.byBlue)   / 255.0f);

  // Create the new group.  If texture purging is turned off, we are assuming
  //  that the mesh group is sharing it's texture.
  MeshGroup* pNewGroup = new MeshGroup;
  ASSERT(NULL != pNewGroup);

  // If there is no texture, we need to create a new material using the
  //  loaded colour
  if (!pTextureID)
  {
    // Create the new material
    material* pNewMaterial = new material;
    ASSERT(NULL != pNewMaterial);

    // Set the properties of the new material
    pNewMaterial->SetDiffuse(clrValue);
    pNewMaterial->SetAmbient(clrValue);

    // Add the material to our global materials array and set the material
    //  index of our new mesh group
    pNewGroup->SetMaterialIdx(G_MATERIALS.Add(pNewMaterial));
  }

  // Increase the reference cound of our texture ID
  if (pTextureID) pTextureID->AddRef();

  // Set the texture of our new mesh group
  pNewGroup->SetTexture(pTextureID);

  // Add the new group to our array of groups
  m_apGroups.Add(pNewGroup);

  // Return the new group
  return(pNewGroup);
}


// ===========================================================================
// Function:    FindVertex
//
// Description: Finds a vertex in the array of vertices contained by the mesh
//
// Params:      vtx:  The vertex we are looking for
//
// Returns:     int
//
// Comments:    Returns the index to the vertex if it is found or -1 if it is
//              not.
// ===========================================================================
int Mesh::FindVertex(const Vertex& vtx)
{
  // If the texture coords are negative, we don't care whether they match or
  //  not, so we can ignore them in our comparison
  if (-1.0f == vtx.fU && -1.0f == vtx.fV)
  {
    for (int i = 0; i < m_pVertices->GetSize(); i++)
      if (m_pVertices->GetAt(i).PartialEquals(vtx)) return(i);
  }
  else
  {
    for (int i = 0; i < m_pVertices->GetSize(); i++)
      if (m_pVertices->GetAt(i) == vtx) return(i);
  }
  return(-1);
}


// ===========================================================================
// Function:    ComputeBoundingBox
//
// Description: Goes through the list of vertices and finds the bounding box
//              of the mesh
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Mesh::ComputeBoundingBox()
{
  ASSERT(0 < m_pVertices->GetSize()); if (0 >= m_pVertices->GetSize()) return;

  // Initialize the box to the first vertex
  m_BoundingBox.SetBox(m_pVertices->GetAt(0).vPos, m_pVertices->GetAt(0).vPos);

  // If this mesh has interpolation data, we need to initialize our bounding
  //  boxes array
  if (1 < m_apVertexSets.GetSize())
  {
    for (int i = 0; i < m_apVertexSets.GetSize(); i++)
    {
      m_aBBoxes.Add(Box(m_apVertexSets.GetAt(i)->GetAt(0).vPos,
                        m_apVertexSets.GetAt(i)->GetAt(0).vPos));
    }
  }

  // Now interate through all of the vertices in the mesh and find the min
  //  and max values so we can set the bounding box
  for (int i = 0; i < m_pVertices->GetSize(); ++i)
  {
    if (m_pVertices->GetAt(i).vPos.x < m_BoundingBox.minx)
      m_BoundingBox.minx = m_pVertices->GetAt(i).vPos.x;
    else if (m_pVertices->GetAt(i).vPos.x > m_BoundingBox.maxx)
      m_BoundingBox.maxx = m_pVertices->GetAt(i).vPos.x;
    
    if (m_pVertices->GetAt(i).vPos.y < m_BoundingBox.miny)
      m_BoundingBox.miny = m_pVertices->GetAt(i).vPos.y;
    else if (m_pVertices->GetAt(i).vPos.y > m_BoundingBox.maxy)
      m_BoundingBox.maxy = m_pVertices->GetAt(i).vPos.y;

    if (m_pVertices->GetAt(i).vPos.z < m_BoundingBox.minz)
      m_BoundingBox.minz = m_pVertices->GetAt(i).vPos.z;
    else if (m_pVertices->GetAt(i).vPos.z > m_BoundingBox.maxz)
      m_BoundingBox.maxz = m_pVertices->GetAt(i).vPos.z;

    if (1 < m_aBBoxes.GetSize())
      m_aBBoxes.SetAt(0, m_BoundingBox);

    for (int j = 1; j < m_aBBoxes.GetSize(); i++)
    {
      if (m_apVertexSets.GetAt(j)->GetAt(i).vPos.x < m_BoundingBox.minx)
        m_BoundingBox.minx = m_apVertexSets.GetAt(j)->GetAt(i).vPos.x;
      else if (m_apVertexSets.GetAt(j)->GetAt(i).vPos.x > m_BoundingBox.maxx)
        m_BoundingBox.maxx = m_apVertexSets.GetAt(j)->GetAt(i).vPos.x;
    
      if (m_apVertexSets.GetAt(j)->GetAt(i).vPos.y < m_BoundingBox.miny)
        m_BoundingBox.miny = m_apVertexSets.GetAt(j)->GetAt(i).vPos.y;
      else if (m_apVertexSets.GetAt(j)->GetAt(i).vPos.y > m_BoundingBox.maxy)
        m_BoundingBox.maxy = m_apVertexSets.GetAt(j)->GetAt(i).vPos.y;

      if (m_apVertexSets.GetAt(j)->GetAt(i).vPos.z < m_BoundingBox.minz)
        m_BoundingBox.minz = m_apVertexSets.GetAt(j)->GetAt(i).vPos.z;
      else if (m_apVertexSets.GetAt(j)->GetAt(i).vPos.z > m_BoundingBox.maxz)
        m_BoundingBox.maxz = m_apVertexSets.GetAt(j)->GetAt(i).vPos.z;
    }
  }
}


// ===========================================================================
// Function:    BindStrideData
//
// Description: Sets up the StrideData structure for this mesh.  This is
//              required to render the mesh.
//
// Params:      pData:  Pointer to the beginning of a Vertex array
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Mesh::BindStrideData(void* pData)
{
  ASSERT(NULL != pData);
  m_StrideData.position.lpvData         = pData;
  m_StrideData.normal.lpvData           = (BYTE*)(m_StrideData.position.lpvData) + sizeof(D3DVECTOR);
  m_StrideData.diffuse.lpvData          = (BYTE*)(m_StrideData.normal.lpvData) + sizeof(D3DVECTOR);
  m_StrideData.specular.lpvData         = (BYTE*)(m_StrideData.diffuse.lpvData) + sizeof(DWORD);
  m_StrideData.textureCoords[0].lpvData = (BYTE*)(m_StrideData.specular.lpvData) + sizeof(DWORD);
}


// ===========================================================================
// PUBLIC: Mesh member functions =============================================
// ===========================================================================


// ===========================================================================
// Function:    LoadFromHMD
//
// Description: Loads a mesh from a raw HMD data pointer that was previously
//              loaded into memory
//
// Params:      pData:
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if the mesh was successfully loaded, or FALSE if
//              it was not
// ===========================================================================
BOOL Mesh::LoadFromHMD(DWORD* pData)
{
  // Process the header
  HMDHeader* pHeader = ProcessHMDHeader(pData);
  ASSERT(NULL != pHeader); if (!pHeader) return(FALSE);

  // Now process the primitives

  //Set the current pointer to the end of the main data block
  HMDPrimitive* pPrimitive = (HMDPrimitive*)(pHeader + 1);

  for (DWORD i = 0; i < pHeader->dwNumBlocks; i++)
  {
    if (pPrimitive->pNextPrim)
    {
      pPrimitive->pNextPrim = (HMDPrimitive*)(pData + (DWORD)pPrimitive->pNextPrim);
      ProcessHMDPrimitive(pPrimitive->pNextPrim, pHeader);
    }
    pPrimitive = (HMDPrimitive*)(((DWORD*)pPrimitive) + 1);
  }

  // Destroy any temporary data
  HMDDiffBlock* pDiffBlock = m_pDiffBlocks;
  HMDDiffBlock* pNextBlock = NULL;
  while (pDiffBlock)
  {
    pNextBlock = pDiffBlock->pNext;
    delete pDiffBlock;
    pDiffBlock = pNextBlock;
  }
  m_pDiffBlocks     = NULL;
  m_nNumDiffBlocks  = m_nNumVtxDiffBlocks = m_nNumNormDiffBlocks = 0;

  // Remove all of the temporary textures, if purging has been enabled
  if (g_bPurgeTextures)
    Mesh_PurgeTextures();

  // If there weren't any vertices loaded, something went wrong
  if (0 == m_pVertices->GetSize()) return(FALSE);

  // Now compute the bounding box for this mesh
  ComputeBoundingBox();

  // Bind the stride data so we can render the mesh
  BindStrideData(m_pVertices->GetData());

  return(TRUE);
}


// ===========================================================================
// Function:    SubstituteTexture
//
// Description: Searches through the mesh groups for a texture having the
//              same properites as the given texture and replaces the mesh 
//              group's texture with the given one.
//
// Params:      pTextureID: The texture we want to replace.
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if a mesh group is found having a texture with
//              the same properties as the given texture and the mesh group's
//              texture is replace with the given texture.  Otherwise, it
//              returns FALSE.
// ===========================================================================
BOOL Mesh::SubstituteTexture(TextureID* pTextureID)
{
  MeshGroup* pMeshGroup = NULL;

  // Iterate through all of the groups
  for (int i = 0; i < m_apGroups.GetSize(); i++)
  {
    // Get the next meshgroup
    pMeshGroup = m_apGroups.GetAt(i);
    ASSERT(NULL != pMeshGroup);

    if (pMeshGroup->GetTexture())
    {
      // If the mesh group's texture matches the given texture, replace it.
      if (pMeshGroup->GetTexture()->operator==(*pTextureID))
      {
        pMeshGroup->SetTexture(pTextureID);
        return(TRUE);
      }
    }
  }

  // We couldn't find the texture
  return(FALSE);
}


// ===========================================================================
// Function:    Interpolate
//
// Description: Takes two poses and interpolates the vertices between them.
//
// Params:      nPose1:   Index to the first set of vertices
//              nPose2:   Index to the second set of vertices
//              fAmount:  The amount to interpolate
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Mesh::Interpolate(int nPose1, int nPose2, float fAmount)
{
  // Set this now in case we quit early
  m_pVertices = m_apVertexSets.GetAt(0);

  // If we only have one set of vertices, we can't interpolate between
  //  sub-vertices.
  if (1 == m_apVertexSets.GetSize())
    return;

  // Out of range checks
  if (0 > nPose1 || nPose1 > m_apVertexSets.GetUpperBound())
    return;
  if (0 < nPose2 || nPose2 > m_apVertexSets.GetUpperBound())
    return;

  // Shortcuts, if we're at either end of the spectrum, just
  // Set the pointer to the predefined vertices.
  if (nPose1 == nPose2)
  {
    m_pVertices = m_apVertexSets[nPose1];
    CopyMemory(&m_BoundingBox, &m_aBBoxes[nPose1], sizeof(Box));
    return;
  }
  if (0.0f == fAmount)
  {
    m_pVertices = m_apVertexSets[nPose1];
    CopyMemory(&m_BoundingBox, &m_aBBoxes[nPose1], sizeof(Box));
    return;
  }
  if (1.0f == fAmount)
  {
    m_pVertices = m_apVertexSets[nPose2];
    CopyMemory(&m_BoundingBox, &m_aBBoxes[nPose2], sizeof(Box));
    return;
  }

  m_pVertices = &m_aInterpolatedVertices;
  Vertex* aVertices = m_pVertices->GetData();

  VertexArray* paSubVertices1 = m_apVertexSets.GetAt(nPose1);
  VertexArray* paSubVertices2 = m_apVertexSets.GetAt(nPose2);

  for (int i = 0; i < m_aInterpolatedVertices.GetSize(); i++)
  {
    // We only interpolate using position and normal
    aVertices[i].vPos =     paSubVertices1->GetAt(i).vPos + 
                          ((paSubVertices2->GetAt(i).vPos -
                            paSubVertices1->GetAt(i).vPos) * fAmount);
    aVertices[i].vNormal =  paSubVertices1->GetAt(i).vNormal + 
                          ((paSubVertices2->GetAt(i).vNormal - 
                            paSubVertices1->GetAt(i).vNormal) * fAmount);

//    m_pVertices[i].x = paSubVertices1[i].x + (paSubVertices1[i].x - paSubVertices2[i].x) * fAmount;
//    m_pVertices[i].y = paSubVertices1[i].y + (paSubVertices1[i].y - paSubVertices2[i].y) * fAmount;
//    m_pVertices[i].z = paSubVertices1[i].z + (paSubVertices1[i].z - paSubVertices2[i].z) * fAmount;
//    m_pVertices[i].nx = paSubVertices1[i].nx + (paSubVertices1[i].nx - paSubVertices2[i].nx) * fAmount;
//    m_pVertices[i].ny = paSubVertices1[i].ny + (paSubVertices1[i].ny - paSubVertices2[i].ny) * fAmount;
//    m_pVertices[i].nz = paSubVertices1[i].nz + (paSubVertices1[i].nz - paSubVertices2[i].nz) * fAmount;
  }
#define INTERP_BOXELEMENT(part)   m_aBBoxes.GetAt(nPose1).part +            \
                                ((m_aBBoxes.GetAt(nPose2).part -            \
                                  m_aBBoxes.GetAt(nPose1).part) * fAmount)

  m_BoundingBox.minx = INTERP_BOXELEMENT(minx);
  m_BoundingBox.maxx = INTERP_BOXELEMENT(maxx);
  m_BoundingBox.miny = INTERP_BOXELEMENT(miny);
  m_BoundingBox.maxy = INTERP_BOXELEMENT(maxy);
  m_BoundingBox.minz = INTERP_BOXELEMENT(minz);
  m_BoundingBox.maxz = INTERP_BOXELEMENT(maxz);

#undef INTERP_BOXELEMENT

//  m_BoundingBox.minx = m_aBBoxes.GetAt(nPose1).minx + 
//                     ((m_aBBoxes.GetAt(nPose2).minx - 
//                       m_aBBoxes.GetAt(nPose1).minx) * fAmount);
//  m_BoundingBox.maxx = m_aBBoxes.GetAt(nPose1).maxx + 
//                     ((m_aBBoxes.GetAt(nPose2).maxx - 
//                       m_aBBoxes.GetAt(nPose1).maxx) * fAmount);
//  m_BoundingBox.miny = m_aBBoxes.GetAt(nPose1).miny + 
//                     ((m_aBBoxes.GetAt(nPose2).miny - 
//                       m_aBBoxes.GetAt(nPose1).miny) * fAmount);
//  m_BoundingBox.maxy = m_aBBoxes.GetAt(nPose1).maxy + 
//                     ((m_aBBoxes.GetAt(nPose2).maxy -
//                       m_aBBoxes.GetAt(nPose1).maxy) * fAmount);
//  m_BoundingBox.minz = m_aBBoxes.GetAt(nPose1).minz + 
//                     ((m_aBBoxes.GetAt(nPose2).minz - 
//                       m_aBBoxes.GetAt(nPose1).minz) * fAmount);
//  m_BoundingBox.maxz = m_aBBoxes.GetAt(nPose1).maxz + 
//                     ((m_aBBoxes.GetAt(nPose2).maxz - 
//                       m_aBBoxes.GetAt(nPose1).maxz) * fAmount);
}


// ===========================================================================
// Function:    Render
//
// Description: Renders all of the groups in this mesh
//
// Params:      p3DDevice:  3D device to use to render the mesh
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void Mesh::Render(D3DDevice* p3DDevice)
{
  ASSERT(NULL != p3DDevice);
  VERIFY(p3DDevice->SetRenderState(D3DRENDERSTATE_ZBIAS, (DWORD)m_byZBias));

  if (UsingAlphaBlending())
  {
    // If we are using alpha blending, turn it on
    VERIFY(p3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE));
    VERIFY(p3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, m_dwSrcBlend));
    VERIFY(p3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, m_dwDestBlend));
    G_MATERIALS.GetAt(1)->SetAsCurrent();
  }

  // Render all of the groups in the mesh
  int nSize = m_apGroups.GetSize();
  for (int i = 0; i < nSize; i++)
    m_apGroups.GetAt(i)->Render(p3DDevice, this);

  // Turn off the alpha blending if we were using it to render this mesh
  if (UsingAlphaBlending())
  {
    VERIFY(p3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE));
    G_MATERIALS.GetAt(0)->SetAsCurrent();
  }
}


// ===========================================================================
// Global C functions ========================================================
// ===========================================================================

extern "C" {


// ===========================================================================
void Mesh_EnableTexturePurge(BOOL bEnable)
{ g_bPurgeTextures = bEnable;                                               }

// ===========================================================================
void Mesh_PurgeTextures()
{
  for (int i = 0; i < g_apTmpTextures.GetSize(); i++)
    g_apTmpTextures.GetAt(i)->Release();
  g_apTmpTextures.RemoveAll();
}

} // extern "C"

#endif // USE_OLD_FRAME

// ===========================================================================
// End of File
// ===========================================================================
