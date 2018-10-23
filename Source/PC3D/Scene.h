// ===========================================================================
// Module:      Scene.h
//
// Description: The basic function of a 'Scene' is to abstract the rendering
//              and 3D scene management process into a structure that
//              applications can either use, extend, or avoid entirely 
//              depending on their requirements. This is particularly 
//              necessary to support different rendering methods which may
//              rely on varying scene management techniques or calls to invoke
//              and manage hardware features. The goal is to allow an app to
//              decide how it wants to make use of the system's resources,
//              since no particular scheme works well for all cases. (see
//              testscene*.cpp for a sample usages)
//
// Copywrite:   Artech, 1999
// ===========================================================================
#ifndef SCENE_H
#define SCENE_H

class D3DDevice;
class view;
class material;
class Light;
class Surface;
class Tag;

// defaults for view projection
#define NEAR_Z 1.0
#define FAR_Z 5000
#define VIEWING_ANGLE (((float)PIDOUBLE)/4)

// ===========================================================================
// Scene class ===============================================================
// ===========================================================================
class Scene
{
  // Protected member variables ==============================================
  protected:
    view*                 m_pRView;
    FrameType*            m_pRoot;
    FrameType*            m_pCamera;
    material*             m_pDefaultMaterial;
    BOOL                  m_bStatsOn;

    // keep track of old device for efficency when changing views
    D3DDevice*            m_pOldDevice;

    // Scene object init, attach, & destroy functionws
    CvectList<MeshType*>  m_Meshes;
    CvectList<Light*>     m_Lights;
    CvectList<material*>  m_Materials;
    CvectList<Surface*>   m_Textures;


  // Public member variables =================================================
  public:
    float                 m_fNearZ;         // Near clipping plane
    float                 m_fFarZ;          // Far clipping plane
    float                 m_fViewingAngle;  // Viewing angle


  // Construction / Destruction ==============================================
  protected:
    // Make the default constructor proctect so users can't call it.  They
    //  must use the Scene(Tag* desc) constructor to create a Scene.
    Scene()               {                                                 }

  public:
    Scene(Tag *desc); 
    virtual ~Scene();


  // Private member functions ================================================
  private:


  // Protected member functions ==============================================
  protected:
    virtual void InitMeshes()             {                                 }
    virtual void InitTextures()           {                                 }
    virtual void InitLights()             {                                 }
    virtual void InitMaterials();

    virtual void DestroyLights();
    virtual void DestroyMaterials();
    virtual void DestroyTextures();
    virtual void DestroyMeshes();

    virtual void SetupLightsinView()      {                                 }
    virtual void RemoveLightsfromView()   {                                 }
    virtual void ProcessTextures()        {                                 }
    virtual void AttachMaterials();

    virtual BOOL PreRender();
    virtual HRESULT Render()              { return(D3D_OK);                 }
    virtual BOOL PostRender();
    virtual HRESULT Restore();

    virtual BOOL SetDefaultRenderStates() { return(TRUE);                   }

    virtual HRESULT RenderMesh(MeshType* pMesh);
    virtual BOOL RenderFrame(FrameType* pFrame, Matrix* pmtrxParent = NULL,
                             BOOL bBillboardInFrameTree = FALSE);

    BOOL SetViewMatrix(FrameType* pCamera);
    BOOL SetProjectionMatrix(Matrix* pMtrx);
    BOOL SetupWorldMatrixForFrame(FrameType* pFrame,
                                  BOOL bAllowForBillboards = TRUE);

    BOOL RejectMesh(MeshType* pTestMesh);
    BOOL RejectMesh(MeshType* pTestMesh, const Matrix& mtrxWorld,
                    const Matrix& mtrxView, float fViewAng);

    BOOL DrawStats()  { return(CalculateAndDrawFrameRate() & DrawInfo());   }
    BOOL DrawInfo();
    BOOL DrawFrameRate(float fps);
    BOOL CalculateAndDrawFrameRate();
    BOOL OutputText(LPSTR szText, int x, int y);


  // Public member functions =================================================
  public:
    BOOL Draw();

        void SetStats(BOOL on);
    INL void SetNearClipPlane(float fNearPlane = NEAR_Z);
    INL void SetFarClipPlane(float fFarPlane = FAR_Z);
    INL void SetClippingPlanes(float fNear = NEAR_Z, float fFar = FAR_Z);
    INL void SetViewingAngle(float fViewAngle = VIEWING_ANGLE);

    INL BOOL SetupProjectionValues();

    virtual BOOL SetRenderView(view *v);
    virtual void AddTexture(Surface *toAdd);
    virtual void RemoveTexture(Surface *toRemove);
};

// ===========================================================================
// INLINE: Scene member functions ============================================
// ===========================================================================


// ===========================================================================
INL void Scene::SetNearClipPlane(float fNearPlane /*= NEAR_Z*/)
{ ASSERT(fNearPlane > 0); m_fNearZ = fNearPlane; SetupProjectionValues();   }

// ===========================================================================
INL void Scene::SetFarClipPlane(float fFarPlane /*= FAR_Z*/)
{ ASSERT(fFarPlane > 0); m_fFarZ = fFarPlane; SetupProjectionValues();      }

// ===========================================================================
INL void Scene::SetClippingPlanes(float fNear /*= NEAR_Z*/,
                                  float fFar /*= FAR_Z*/)
{
  ASSERT(fNear > 0 && fFar > 0);
  m_fNearZ  = fNear;
  m_fFarZ   = fFar;
  SetupProjectionValues();
}

// ===========================================================================
INL void Scene::SetViewingAngle(float fViewAngle /*= VIEWING_ANGLE*/)
{ 
  ASSERT(fViewAngle > 0); 
  m_fViewingAngle = fViewAngle;
  SetupProjectionValues();
}

// ===========================================================================
INL BOOL Scene::SetupProjectionValues()
{
  Matrix proj;
  proj.ProjectionMatrix(m_fNearZ, m_fFarZ, m_fViewingAngle, 1.0f);
  return(Scene::SetProjectionMatrix(&proj));
}

#endif // SCENE_H
// ===========================================================================
