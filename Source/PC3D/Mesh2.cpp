#include "PC3DHdr.h"
#include "l_mesh2.h"


void mesh2::calcVertexSizeAndOffsets(int vertexType)
{
  int size = 0;

  // some types are mutually exclusive
  assert (! ((vType & D3DFVF_XYZRHW) && (vType & D3DFVF_NORMAL)) );

  // by default, they don't exist
  wOffset = normalOffset = diffuseOffset = specularOffset = uvOffset = -1;

  if (vType & D3DFVF_XYZ)
    size += sizeof(float) * 3;

  if (vType & D3DFVF_XYZRHW)
    {
    wOffset = size;
    size += sizeof(float);
    }

  if (vType & D3DFVF_NORMAL)
    {
    normalOffset = size;
    size += sizeof(float) * 3;
    }

  if (vType & D3DFVF_DIFFUSE)
    {
    diffuseOffset = size;
    size += sizeof(DWORD);
    }

  if (vType & D3DFVF_SPECULAR)
    {
    specularOffset = size;
    size += sizeof(DWORD);
    }

  int texCount = (vType & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
  if (texCount)
    {
    uvOffset = size;
    size += texCount * sizeof(float) * 2;
    }

  // must have a non-zero vertex size
  assert (size);

  vSize = size;
}


mesh2::mesh2(int vertexType, int numVerts, int maxTris, void *vertsInit)
  {
// no more than these 3 types are supported if not DX6

#ifndef D3DFVF_RESERVED0
  assert (vertexType == D3DFVF_VERTEX ||
      vertexType == D3DFVF_LVERTEX ||
      vertexType == D3DFVF_TLVERTEX );
#endif

  // must have at least 3 vertices
  assert (numVerts >= 3 && maxTris >= 0);

  vType = vertexType;
  calcVertexSizeAndOffsets(vertexType);

  // allocate the vertex and face arrays
  if (maxTris == 0)
    maxTris = numVerts * 2;

  tAlloc = maxTris;
  tCount = 0;
  vCount = numVerts;

  verts = new char[vSize * vCount];
  indices = new WORD[tAlloc * 3];
  }


mesh2::~mesh2()
{
delete [] verts;
delete [] indices;
}



void mesh2::generateNormals()
{
  vector ab,ac,abxac;
  WORD fa,fb,fc;

  // do nothing if no normals exist
  if ( normalOffset == -1 )
    return;

  // there's no unique way to generate normals with no other info,
  // so just init all the vertices using the cross product of the face edges
  // some normals will be calc'd more than once...not efficient, but it's just for convenience
  for (int i = 0; i < tCount; i++)
  {
    getTri(i, &fa,&fb,&fc);

    vertex& a = (*this)[fa];
    vertex& b = (*this)[fb];
    vertex& c = (*this)[fc];
    vector va(a.x,a.y,a.z);
    vector vb(b.x,b.y,b.z);
    vector vc(c.x,c.y,c.z);
    ab = vb - va; 
    ac = vc - va; 
    abxac = CrossProduct(ab, ac);

    if (abxac.x || abxac.y || abxac.z)
      abxac = Normalize(abxac);

    // all three vertices of the tri share the same normal
    getNormal(fa) = abxac;
    getNormal(fb) = abxac;
    getNormal(fc) = abxac;
  }
}


void mesh2::getBox(Box *bounds)
{
  assert( bounds );

  Box& bbox = *bounds;
  bbox.minx = bbox.miny = bbox.minz = FLT_MAX;
  bbox.maxx = bbox.maxy = bbox.maxz = -FLT_MAX;

  float x,y,z;

  for (int i=0; i < vCount; i++)
  {
    x = (*this)[i].x;
    y = (*this)[i].y;
    z = (*this)[i].z;

    // update the bounding box
    if (x < bbox.minx) bbox.minx = x;
    if (y < bbox.miny) bbox.miny = y;
    if (z < bbox.minz) bbox.minz = z;

    if (x > bbox.maxx) bbox.maxx = x;
    if (y > bbox.maxy) bbox.maxy = y;
    if (z > bbox.maxz) bbox.maxz = z;
  }
}


void mesh2::optimize()
{
// currently does nothing, but can rearrange stuff into a vertex buffer
// or the strided format once DX6 is supported
}




// .......... sample mesh generators 

mesh2 *newBoid(float size)
{
  mesh2& boid = * new mesh2(D3DFVF_VERTEX,16);

// generate the boid data
  // top
  boid[ 0] = D3DVERTEX(D3DVECTOR(0.0f, 0.0f, 10.0f), Normalize(D3DVECTOR(0.2f, 1.0f, 0.0f)), 0.0f, 0.5f);
  boid[ 1] = D3DVERTEX(D3DVECTOR(10.0f, 0.0f, -10.0f), Normalize(D3DVECTOR(0.1f, 1.0f, 0.0f)), 0.5f, 1.0f);
  boid[ 2] = D3DVERTEX(D3DVECTOR(3.0f, 3.0f, -7.0f), Normalize(D3DVECTOR(0.0f, 1.0f, 0.0f)), 0.425f, 0.575f);
  boid[ 3] = D3DVERTEX(D3DVECTOR(-3.0f, 3.0f, -7.0f), Normalize(D3DVECTOR(-0.1f, 1.0f, 0.0f)), 0.425f, 0.425f);
  boid[ 4] = D3DVERTEX(D3DVECTOR(-10.0f, 0.0f, -10.0f), Normalize(D3DVECTOR(-0.2f, 1.0f, 0.0f)), 0.5f, 0.0f);

  //bottom
  boid[ 5] = D3DVERTEX(D3DVECTOR(0.0f, 0.0f, 10.0f), Normalize(D3DVECTOR(0.2f, -1.0f, 0.0f)), 1.0f, 0.5f);
  boid[ 6] = D3DVERTEX(D3DVECTOR(10.0f, 0.0f, -10.0f), Normalize(D3DVECTOR(0.1f, -1.0f, 0.0f)), 0.5f, 1.0f);
  boid[ 7] = D3DVERTEX(D3DVECTOR(3.0f, -3.0f, -7.0f), Normalize(D3DVECTOR(0.0f, -1.0f, 0.0f)), 0.575f, 0.575f);
  boid[ 8] = D3DVERTEX(D3DVECTOR(-3.0f, -3.0f, -7.0f), Normalize(D3DVECTOR(-0.1f, -1.0f, 0.0f)), 0.575f, 0.425f);
  boid[ 9] = D3DVERTEX(D3DVECTOR(-10.0f, 0.0f, -10.0f), Normalize(D3DVECTOR(-0.2f, -1.0f, 0.0f)), 0.5f, 0.0f);

  // rear
  boid[10] = D3DVERTEX(D3DVECTOR(10.0f, 0.0f, -10.0f), Normalize(D3DVECTOR(-0.4f, 0.0f, -1.0f)), 0.5f, 1.0f);
  boid[11] = D3DVERTEX(D3DVECTOR(3.0f, 3.0f, -7.0f), Normalize(D3DVECTOR(-0.2f, 0.0f, -1.0f)), 0.425f, 0.575f);
  boid[12] = D3DVERTEX(D3DVECTOR(-3.0f, 3.0f, -7.0f), Normalize(D3DVECTOR(0.2f, 0.0f, -1.0f)), 0.425f, 0.425f);
  boid[13] = D3DVERTEX(D3DVECTOR(-10.0f, 0.0f, -10.0f), Normalize(D3DVECTOR(0.4f, 0.0f, -1.0f)), 0.5f, 0.0f);
  boid[14] = D3DVERTEX(D3DVECTOR(-3.0f, -3.0f, -7.0f), Normalize(D3DVECTOR(0.2f, 0.0f, -1.0f)), 0.575f, 0.425f);
  boid[15] = D3DVERTEX(D3DVECTOR(3.0f, -3.0f, -7.0f), Normalize(D3DVECTOR(-0.2f, 0.0f, -1.0f)), 0.575f, 0.575f);

  // top
  boid.addTri(0,1,2);
  boid.addTri(0,2,3);
  boid.addTri(0,3,4);

  // bottom
  boid.addTri(5,7,6);
  boid.addTri(5,8,7);
  boid.addTri(5,9,8);

  // rear
  boid.addTri(10,15,11);
  boid.addTri(11,15,12);
  boid.addTri(12,15,14);
  boid.addTri(12,14,13);

  // adjust the verts so the size is roughly the given size,
  // given that the original size is 10 units in both the + and - axes
  int i;
  for(i=0; i < 15; i++)
  {
    boid[i].x *= (float) (size / 20.0);
    boid[i].y *= (float)(size / 20.0);
    boid[i].z *= (float) (size / 20.0);
  }

  return &boid;
}
