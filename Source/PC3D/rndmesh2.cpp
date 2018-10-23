#include "PC3DHdr.h"

#include "l_mesh2.h"

BOOL render(mesh2 *m, view* in)
{
  D3DDevice* p3DDevice = pc3D::Get3DDevice();
  ASSERT(NULL != p3DDevice);

  // ..........   call to D3D's DrawPrimitive
  return(p3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
                                         m->getVertexType(), m->verts,
                                         m->getNumVertices(), m->indices,
                                         m->getNumTris() * 3, 0));
}

