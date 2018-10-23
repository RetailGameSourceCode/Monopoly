// ===========================================================================
// Module:      GlobalData.cpp
//
// Description: 
//
// Created:     July 8, 1999
//
// Author:      
//
// Copywrite:   Artech, 1999
// ===========================================================================

#include "PC3DHdr.h"
#include "l_Material.h"
#include "GlobalData.h"

#define GetTPage(tp, abr, x, y)                                             \
  ((((tp)&0x3)<<7)|(((abr)&0x3)<<5)|(((y)&0x100)>>4)|(((x)&0x3ff)>>6)|      \
  (((y)&0x200)<<2))

// ===========================================================================
// Global variables ==========================================================
// ===========================================================================

Array<material*, material*>   g_apMaterials;

int                           g_nTextureBank = 0;


// ===========================================================================
// Global functions ==========================================================
// ===========================================================================
extern "C" {

void SetCurrentTIMBank(WORD nBank)
{
  G_TEXTUREBANK = nBank;
}


// ===========================================================================
// Function:    InitDefaultMaterial
//
// Description: Initializes the default material
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void InitDefaultMaterial()
{
  material* pDefaultMaterial = new material;
  ASSERT(NULL != pDefaultMaterial);
  pDefaultMaterial->SetDiffuse(CLRVALUE(1.0f, 1.0f, 1.0f));
  pDefaultMaterial->SetAmbient(CLRVALUE(1.0f, 1.0f, 1.0f));
  ASSERT(0 == G_MATERIALS.GetSize());
  G_MATERIALS.Add(pDefaultMaterial);
}


// ===========================================================================
// Function:    LoadBMPTexture
//
// Description: Loads a BMP file and creates a new TextureID structure from it
//
// Params:      dwX:        X position of the new texture
//              dwY:        Y position of the new texture
//              szBmpFile:  Filename of the BMP to load as the new texture
//
// Returns:     TextureID*
//
// Comments:    Returns a pointer to the loaded TextureID if successfull, or
//              NULL if not.
// ===========================================================================
TextureID* LoadBMPTexture(WORD wX, WORD wY, LPCSTR szBmpFile)
{
  SIZE size;
  TextureID* pTextureID = new TextureID;
  ASSERT(NULL != pTextureID); if (!pTextureID) return(NULL);
  
  // Create the new surface and setup the other TextureID parameters
  pTextureID->wXPos     = wX;
  pTextureID->wYPos     = wY;
  pTextureID->pTexture  = new Surface(szBmpFile, TRUE);

  // Check to make sure that the texture was loaded properly
  if (!pTextureID->pTexture || !pTextureID->pTexture->IsInitialized())
  {
    DESTROYSURFACE(pTextureID->pTexture);
    DESTROYPOINTER(pTextureID);
    return(NULL);
  }

  // Get the size of the new texture
  pTextureID->pTexture->GetSize(size);

  pTextureID->wWidth    = (WORD)size.cx;
  pTextureID->wHeight   = (WORD)size.cy;

  // Get the texture page index
  pTextureID->wTPageID = GetTPage(1, 0, pTextureID->wXPos, pTextureID->wYPos);

  ASSERT(pTextureID->nBank == G_TEXTUREBANK);

  return(pTextureID);
}


// ===========================================================================
// Function:    DestroyMaterials
//
// Description: Destroys all of the materials in the global material array
//
// Params:      void
//
// Returns:     void
//
// Comments:    
// ===========================================================================
void DestroyMaterials()
{
  material* pMaterial = NULL;

  for (int i = G_MATERIALS.GetUpperBound(); i >= 0; i--)
  {
    pMaterial = G_MATERIALS.GetAt(i);
    DESTROYPOINTER(pMaterial);
    G_MATERIALS.RemoveAt(i);
  }
}

} // extern "C"


// ===========================================================================
// End Of File
// ===========================================================================
