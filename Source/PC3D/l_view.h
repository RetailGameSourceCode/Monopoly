#ifndef VIEW_H
#define VIEW_H

class Light;
class material;
class Surface;


// A View represents a 3D viewport on any surface. It is "attached" to a
//  surface and can be sized, moved, etc. on the surface. Advanced stuff
//  such as setting the clip region, z-buffer alignment, etc. can be done
//  here in the future -cc
class view
{
  // Member variables ========================================================
  protected:
    iRect       m_rctArea;          // Rectangle describing the view's area
    PVIEWPORT   m_pViewport;        // Direct3D viewport interface
    Surface*    m_pAttachedSurface; // Surface attached to the viewport
    Matrix      m_mtrxCamera;       // Camera matrix

    Surface*    m_pBackground;      // Background surface
    float       m_fHScale;          // Horizontal scaling factor for the background
    float       m_fVScale;          // Vertical scaling factor for the background

    CvectList<Light*> lightsInView;
    

  // Construction / Destruction ==============================================
  public:
    view(Surface* pHostSurface, iRect* viewArea = NULL, FrameType *world = NULL);
    ~view();


  // Private member functions ================================================
  private:


  // Protected member functions ==============================================
  protected:
    BOOL    init();
    BOOL    findLight(Light* l);


  // Public member functions =================================================
  public:
    operator  PVIEWPORT() const { return(m_pViewport); }

    INL void  SetAsCurrent();
        void  SetBackground( material *m );
        BOOL  SetBackgroundSurface(Surface* pBackground, BOOL bForce = FALSE);

        BOOL  SetViewportRect(const RECT& rct);

    BOOL      Clear();
    BOOL      Clear(DWORD dwClr, D3DRECT* pRect = NULL, float fZValue = 1.0f);
    BOOL      Clear(HRGN hRegion, float fZValue = 1.0f, BOOL bClearZ = TRUE,
                    BOOL bClearTarget = FALSE, DWORD dwClr = 0xFF000000);
    BOOL      ClearZBuffer(D3DRECT* pRect = NULL, float fZValue = 1.0f);
    
    void      addLight(Light *l);
    void      removeLight(Light *l);
    void      removeLights();

    const iRect&  getViewArea() const   { return(m_rctArea); }
    Surface*      getSurface() const    { return(m_pAttachedSurface); }


    // sets the viewing matrix of the underlying device to 
    // to transform world coordinates to the given camera frame
    BOOL  setViewMatrix(FrameType *camera);
	  BOOL	setViewMatrix(D3DMATRIX *viewMatrix);

    // returns the last view matrix that was set as a result of the camera frame
    Matrix* getViewMatrix() { return(&m_mtrxCamera); }
                   
    // calc the frame 'object's extent in the viewport using the given camera.
    // NOTE: will alter the internal Direct3D device matrices. returns FALSE
    // if there is no intersection between the viewport and the extent. The 
    // extent is returned in 'inViewport'. 
    // NOTE: avoid calling during a scene render, because these result in
    // driver calls that may interrupt the hardware renderer.
    BOOL    getExtent2D(FrameType* object, FrameType* camera, iRect* inViewport);

    // Similar to 'getExtent2D', but calculates the extent of the given 
    //  'objectBox' using 'object' as its local coordinate system. No children
    //  of the frame or meshes in the frame are involved in the computation. 
    BOOL    getExtent2DBox(FrameType* object, Box* objectBox, FrameType* camera,
                           iRect* inViewport);

};

// ===========================================================================
// INLINE: view member functions =============================================
// ===========================================================================

// ===========================================================================
INL void view::SetAsCurrent()
{
  VERIFY(pc3D::Get3DDevice()->SetCurrentViewport(m_pViewport));
}


#endif // VIEW_H
// ===========================================================================
