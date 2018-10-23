// ===========================================================================
// Module:      l_Light.h
//
// Description: Light class definition.  Lights are used in viewports to
//              calculate shading values at vertices.
//
// Created:     May 10, 1999 (module header insertion)
//
// Author:      Unknown
//
// Copywrite:   Artech, 1999
// ===========================================================================

#ifndef LIGHT_H
#define LIGHT_H

#include "l_view.h"

// ===========================================================================
// Light class ===============================================================
// ===========================================================================
class Light
{
  // Friend functions ========================================================
  friend void view::addLight(Light *l);
  friend void view::removeLight(Light *l);
  friend void view::removeLights();


  // Member variables ========================================================
  protected:
    LIGHTDATA     m_LightData;    // structure defining the light
    PLIGHT        m_pD3DLight;    // object pointer for the light

    view*         m_pInView;      // The viewport to which this light is 
                                  //  attached

    BOOL          m_bChanged :1;  // Indicates the properties of the light
                                  //  have changed
    

  // Construction / Destruction ==============================================
  public:
    Light();
    ~Light();


  // Private member functions ================================================
  private:

  // Protected member functions ==============================================
  protected:
    // view class privately manages ref counts to lights
    void    AddViewRef(view* v)     { ASSERT(!m_pInView);  m_pInView = v;   }
    void    ReleaseViewRef(view* v) { ASSERT(m_pInView == v); m_pInView = NULL; }
    view*   GetView() const         { return(m_pInView);                    }


  // Public member functions =================================================
  public:
    operator PLIGHT() const         { return(m_pD3DLight);                  }
    
    INL HRESULT       Set(void);

    INL D3DLIGHTTYPE  GetType()         const;
    INL CLRVALUE      GetColor()        const;
    INL vector        GetPosition()     const;
    INL vector        GetDirection()    const;
    INL vector        GetAttenuation()  const;
    INL float         GetRange()        const;
    INL float         GetFalloff()      const;
    INL float         GetPhi()          const;
    INL float         GetTheta()        const;
    INL float         GetUmbra()        const;
    INL float         GetPenumbra()     const;
    INL DWORD         GetFlags()        const;

    INL void          SetType(D3DLIGHTTYPE eLightType);
    INL void          SetColor(const CLRVALUE& color);
    INL void          SetPosition(const vector& position);
    INL void          SetDirection(const vector& direction);
    INL void          SetAttenuation(const vector& attenuation);
    INL void          SetRange(float fRange);
    INL void          SetFalloff(float fFalloff);
    INL void          SetPhi(float fRads);
    INL void          SetTheta(float fRads);
    INL void          SetUmbra(float fUmbra);
    INL void          SetPenumbra(float fPenumbra);
    INL void          SetFlags(DWORD flags);
};



// ===========================================================================
// PointLight class ==========================================================
// ===========================================================================
class PointLight : public Light
{
  public:
    PointLight(const CLRVALUE& color, const vector& position);
};


// ===========================================================================
// SpotLight class ===========================================================
// ===========================================================================
class SpotLight : public Light
{
  public:
    SpotLight(const CLRVALUE& color, const vector& position,
              const vector& direction, const float umbra_angle, 
              const float penumbra_angle);
};


// ===========================================================================
// DirectionalLight class ====================================================
// ===========================================================================
class DirectionalLight : public Light
{
  public:
    DirectionalLight(const CLRVALUE& color, const vector& direction);
};


// ===========================================================================
// ParallelPointLight class ==================================================
// ===========================================================================
class ParallelPointLight : public Light
{
  public:
    ParallelPointLight(const CLRVALUE& color, const vector& position);
};


// ===========================================================================
// INLINE: Light member functions ============================================
// ===========================================================================

// ===========================================================================
INL HRESULT Light::Set(void)
{ 
  if (m_bChanged)
  { 
    m_bChanged = FALSE;
    return(m_pD3DLight->SetLight((PLIGHTDATA1)&m_LightData));
  }
  return D3D_OK;
}

// ===========================================================================
D3DLIGHTTYPE Light::GetType() const
{
  return(m_LightData.dltType);
}

// ===========================================================================
INL CLRVALUE Light::GetColor() const
{
  return(CLRVALUE(m_LightData.dcvColor));
}

// ===========================================================================
INL vector Light::GetPosition() const
{ 
  return(m_LightData.dvPosition);
}

// ===========================================================================
INL vector Light::GetDirection() const
{ 
  return(m_LightData.dvDirection);
}

// ===========================================================================
INL vector Light::GetAttenuation() const
{ 
  vector  ret;
  ret[0] = m_LightData.dvAttenuation0;
  ret[1] = m_LightData.dvAttenuation1;
  ret[2] = m_LightData.dvAttenuation2;
  return(ret);
}

// ===========================================================================
INL float Light::GetRange() const
{
  return(m_LightData.dvRange);
}

// ===========================================================================
INL float Light::GetFalloff() const
{
  return(m_LightData.dvFalloff);
}

// ===========================================================================
INL float Light::GetPhi() const
{
  return(m_LightData.dvPhi);
}

// ===========================================================================
INL float Light::GetTheta() const
{
  return(m_LightData.dvTheta);
}

// ===========================================================================
INL float Light::GetUmbra() const
{
  return(GetTheta());
}

// ===========================================================================
INL float Light::GetPenumbra() const
{
  return(GetPhi());
}

// ===========================================================================
INL DWORD Light::GetFlags() const
{
  return(m_LightData.dwFlags);
}

// ===========================================================================
INL void Light::SetType(D3DLIGHTTYPE eLightType)
{
  m_LightData.dltType = eLightType; m_bChanged = TRUE;
}

// ===========================================================================
INL void Light::SetColor(const CLRVALUE& color)
{
  CopyMemory(&(m_LightData.dcvColor), &color, sizeof(CLRVALUE));
  m_bChanged = TRUE;
}

// ===========================================================================
INL void Light::SetPosition(const vector& position)
{ 
  m_LightData.dvPosition = position; m_bChanged = TRUE;
}

// ===========================================================================
INL void Light::SetDirection(const vector& direction)
{ 
  m_LightData.dvDirection = direction; m_bChanged = TRUE;
}

// ===========================================================================
INL void Light::SetAttenuation(const vector& attenuation)
{ 
  m_LightData.dvAttenuation0 = attenuation[0];
  m_LightData.dvAttenuation1 = attenuation[1];
  m_LightData.dvAttenuation2 = attenuation[2];
  m_bChanged = TRUE;
}

// ===========================================================================
INL void Light::SetRange(float fRange)
{
  m_LightData.dvRange = fRange; m_bChanged = TRUE;
}

// ===========================================================================
INL void Light::SetFalloff(float fFalloff)
{
  m_LightData.dvFalloff = fFalloff; m_bChanged = TRUE;
}

// ===========================================================================
INL void Light::SetPhi(float fRads)
{
  m_LightData.dvPhi = fRads; m_bChanged = TRUE;
}

// ===========================================================================
INL void Light::SetTheta(float fRads)
{
  m_LightData.dvTheta = fRads; m_bChanged = TRUE;
}

// ===========================================================================
INL void Light::SetUmbra(float fUmbra)
{
  SetTheta(fUmbra);
}

// ===========================================================================
INL void Light::SetPenumbra(float fPenumbra)
{
  SetPhi(fPenumbra);
}

// ===========================================================================
INL void Light::SetFlags(DWORD dwFlags)
{
  m_LightData.dwFlags = dwFlags; m_bChanged = TRUE;
}


#endif LIGHT_H
// ===========================================================================
