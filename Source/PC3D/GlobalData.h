// ===========================================================================
// Module:      GlobalData.h
//
// Description: 
//
// Created:     July 8, 1999
//
// Author:      
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef INC_GLOBALDATA
#define INC_GLOBALDATA


#define MAX_TEXTURES      1024
#define MAX_MATERIALS     64

class Surface;
class material;


extern int                g_nTextureBank;
#define G_TEXTUREBANK     g_nTextureBank


struct TextureID
{
  Surface*  pTexture;
  WORD      wTPageID;
  WORD      wXPos;
  WORD      wYPos;
  WORD      wWidth;
  WORD      wHeight;
  int       nBank;
  int       nRefCount;

  TextureID()
  { ZeroMemory(this, sizeof(TextureID)); nBank = G_TEXTUREBANK; nRefCount = 1; }

  ~TextureID() { DESTROYSURFACE(pTexture); }

  BOOL operator==(const TextureID& textureID)
  { 
    return(textureID.wTPageID == wTPageID && textureID.nBank   == nBank &&
           textureID.wXPos    == wXPos    && textureID.wYPos   == wYPos/* &&
           textureID.wWidth   == wWidth   && textureID.wHeight == wHeight*/);
  }

  TextureID& operator=(const TextureID& textureID)
  { CopyMemory(this, &textureID, sizeof(TextureID)); return(*this);         }

  int AddRef()                { return(++nRefCount);                        }

  int Release()
  {
    if (0 < nRefCount) nRefCount--;
    if (!nRefCount)
    { delete this; return(0); }
    return(nRefCount);
  }

};

extern Array<material*, material*>    g_apMaterials;

#define G_MATERIALS                   g_apMaterials



// ===========================================================================
// Prototypes ================================================================
// ===========================================================================


extern "C"
{

void  SetCurrentTIMBank(WORD wBank);
void  InitDefaultMaterial();
void  DestroyMaterials();

TextureID*  LoadBMPTexture(WORD wX, WORD wY, LPCSTR szBmpFile);

} // extern "C"


#endif // INC_GLOBALDATA
// ===========================================================================
