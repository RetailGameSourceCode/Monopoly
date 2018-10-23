#ifndef MATERIAL_H
#define MATERIAL_H

#include "l_view.h"

//  materials are used with meshes when rendering

// ===========================================================================
// material class ============================================================
// ===========================================================================
class material 
{
  // Member variables ========================================================
  private:
    MATERIALDATA    m_MaterialData;
    HMATERIAL       m_hMaterial;
    PMATERIAL       m_pMaterial;
    view*           m_pCurrentView;
    Surface*        m_pTexture;
    BOOL            m_bChanged;

  // Construction / Destruction ==============================================
  public:
    material();

    INL ~material();


  // Private member functions ================================================
  private:


  // Protected member functions ==============================================
  protected:


  // Public member functions =================================================
  public:
    operator HMATERIAL() const    { return(m_hMaterial);                    }
    operator PMATERIAL() const    { return(m_pMaterial);                    }

    INL BOOL      Attach(view* pView);
    INL void      Refresh();
    INL void      SetAsBackground();
        void      SetAsCurrent();

    
    INL CLRVALUE  GetDiffuse()  const;
    INL CLRVALUE  GetAmbient()  const;
    INL CLRVALUE  GetEmissive() const;
    INL CLRVALUE  GetSpecular() const;
    INL D3DVALUE  GetPower()    const;
    INL DWORD     GetRampSize() const;

    INL void      SetDiffuse(const CLRVALUE& clr);
    INL void      SetAmbient(const CLRVALUE& clr);
    INL void      SetEmissive(const CLRVALUE& clr);
    INL void      SetSpecular(const CLRVALUE& clr);
    INL void      SetPower(D3DVALUE power);
    INL void      SetRampSize(DWORD ramp);
  
    //  Notes on using textures with materials
    //  -several materials may use the same texture
    //  -set the current texture to NULL to remove texture
    //  -never delete the surface before removing it from the material.
    //  -the texture must be attached to the same view as the material.
    INL void      SetTexture(Surface* pTex = NULL);
    INL Surface*  GetTexture(void);
  
}; // End class material


// ===========================================================================
// INLINE: material member functions =========================================
// ===========================================================================

// ===========================================================================
INL material::~material()
{ RELEASEPOINTER(m_pMaterial);                                              }

// ===========================================================================
INL BOOL material::Attach(view* pView)
{
  if (m_pCurrentView == pView) return(TRUE);
  m_pCurrentView = pView;

  m_bChanged = TRUE;
  Refresh();
  
  D3DDevice* p3DDevice = pc3D::Get3DDevice();
  ASSERT(NULL != p3DDevice);

  HRESULT hResult = m_pMaterial->GetHandle(p3DDevice->GetD3DDevice(),
                                           &m_hMaterial);
  RETURN_ON_ERROR(hResult, FALSE);
  return TRUE;
}

// ===========================================================================
INL void material::Refresh()
{
  if (m_bChanged) 
  {
    m_bChanged      = FALSE;
    HRESULT hResult = m_pMaterial->SetMaterial(&m_MaterialData);
    REPORT_ON_ERROR(hResult);
  } 
}

// ===========================================================================
INL void material::SetAsBackground()
{
  ASSERT(NULL != m_pCurrentView); // material must have been attached to a view
  m_MaterialData.dwRampSize = 1;
  m_bChanged                = TRUE;
  Refresh();
  m_pCurrentView->SetBackground(this);
}

// ===========================================================================
INL void material::SetDiffuse(const CLRVALUE& clr)
{ m_MaterialData.diffuse = clr; m_bChanged = TRUE;                          }

// ===========================================================================
INL CLRVALUE material::GetDiffuse() const
{ return(m_MaterialData.diffuse);                                           }

// ===========================================================================
INL void material::SetAmbient(const CLRVALUE& clr)
{ m_MaterialData.ambient = clr; m_bChanged = TRUE;                          }

// ===========================================================================
INL CLRVALUE material::GetAmbient() const
{ return(m_MaterialData.ambient);                                           }

// ===========================================================================
INL void material::SetEmissive(const CLRVALUE& clr)
{ m_MaterialData.emissive = clr; m_bChanged = TRUE;                         }

// ===========================================================================
INL CLRVALUE material::GetEmissive() const
{ return(m_MaterialData.emissive);                                          }

// ===========================================================================
INL void material::SetSpecular(const CLRVALUE& clr)
{ m_MaterialData.specular = clr; m_bChanged = TRUE;                         }

// ===========================================================================
INL CLRVALUE material::GetSpecular() const
{ return(m_MaterialData.specular);                                          }

// ===========================================================================
INL void material::SetPower(D3DVALUE power)
{ m_MaterialData.power = power; m_bChanged = TRUE;                          }

// ===========================================================================
INL D3DVALUE material::GetPower() const
{ return(m_MaterialData.power);                                             }

// ===========================================================================
INL void material::SetRampSize(DWORD dwRamp)
{ m_MaterialData.dwRampSize = dwRamp; m_bChanged = TRUE;                    }

// ===========================================================================
INL DWORD material::GetRampSize() const
{ return(m_MaterialData.dwRampSize);                                        }

// ===========================================================================
INL void material::SetTexture(Surface* pTex /*= NULL*/)
{ m_pTexture = pTex; m_bChanged = TRUE;                                     }

// ===========================================================================
INL Surface* material::GetTexture()
{ Refresh(); return(m_pTexture);                                            }

#endif // MATERIAL_H
// ===========================================================================
