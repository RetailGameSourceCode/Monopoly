// //////////////////////////////////////////////////
// Copyright (C) 1997, Artech,        
// All rights reserved.          
//                 
// This document contains intellectual property  
// of Artech.  Any copying or reproduction    
// without prior written permission is prohibited. 
//                 
// //////////////////////////////////////////////////
// MODULE: rndmesh1.cpp              
// Contents: handles the mesh's device-dependent render calls
//                  
// ///////////////// END OF HEADER //////////////////
#include "PC3DHdr.h"
#include "l_mesh1.h"
#include "l_view.h"

BOOL render(mesh1 *m, view* in)
{
  D3DDevice* p3DDevice = pc3D::Get3DDevice();
  ASSERT(NULL != p3DDevice);

  if (!m->stripData)
  {
    face *f = &m->indexedTris[0];
    vertex *v = &((vertexList&)m->vertices)[0];

    // check to see if the illegal cast below won't work
    assert (sizeof(face) == 3 * sizeof(WORD));

    if (!p3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                    v, ((vertexList&)m->vertices).count(),
                                    &(f->verts[0]),
                                    m->indexedTris.count() * 3, 0))
    {
      return(FALSE);
    }

  }

  else
  {
    // ..........  render in strips if they are initialized
    int totalStrips = m->stripIndices->count();
    int totalVerts = ((vertexList&)*(m->stripData)).count();

    for (int i = 0; i < totalStrips; i++)
    {
      int stripStartAt = (*(m->stripIndices))[ i ];
      int numVertsInStrip = (i == totalStrips-1) ?
      totalVerts - stripStartAt :
      (*(m->stripIndices))[ i + 1 ] - stripStartAt;
          
      vertex *v = &((vertexList&)*(m->stripData))[ stripStartAt ];

      if (!p3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, v,
                                    numVertsInStrip, 0))
      {
        return(FALSE);
      }
    }
  }

  return(TRUE);
}


