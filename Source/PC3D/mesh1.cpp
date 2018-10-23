// //////////////////////////////////////////////////
// Copyright (C) 1997, Artech,        
// All rights reserved.          
//                 
// This document contains intellectual property  
// of Artech.  Any copying or reproduction    
// without prior written permission is prohibited. 
//                 
// //////////////////////////////////////////////////
// MODULE: node.cpp              
// Contents: handles the scene's nodes (ususally visible elements)
//                  
// ///////////////// END OF HEADER //////////////////
#include "PC3DHdr.h"
#include "l_mesh1.h"
#include "tomesh.h"


CvectList<mesh*> mesh1::allMeshes(16,16);


mesh1::mesh1(Tag& t)
  : coordIndex(0,16),
  normalIndex(0,16),
  texCoordIndex(0,16),
  colorIndex(0,16),
  vectors(0),
  colors(0),
  texCoords(0),
  normals(0),
  vertices(0,16),
  indexedTris(0,16),
  stripData(0),
  stripIndices(0),
  solid(TRUE),
  ccw(TRUE),
  colorPerVertex(TRUE),
  normalPerVertex(TRUE),
  textureName(),
  textureSurf(0),
  editing(FALSE),
  bboxDirty(TRUE)
{
  strcpy(name,t); allMeshes.add(this);


  // setup default material info
  // leave the default material black
  memset(&Mat, 0, sizeof(MATERIALDATA));

  Mat.dwRampSize = 16;
  Mat.diffuse.a = 1.0f;
}


mesh1::~mesh1()
{
// .......... NOTE: if vertices,etc. are shared by something else, 
// ..........   the vertex data will remain valid even when 
// ..........   member 'vectors' goes out of scope

  deleteSourceGeometry();
  
  if (stripData)
  {
    delete stripData;
    delete stripIndices;
    stripData = 0;
  }
  allMeshes.detach(this);
}


void mesh1::deleteSourceGeometry()
{
  if (vectors)
    delete vectors;
  if (normals)
    delete normals;
  if (colors)
    delete colors;
  if (texCoords)
    delete texCoords;

  vectors=0;
  normals=0;
  colors=0;
  texCoords=0;

  coordIndex.detachAll();
  colorIndex.detachAll();
  normalIndex.detachAll();
  texCoordIndex.detachAll();
}


void mesh1::beginEdit()
{
  assert (!editing); // can't start if you already are editing
  editing = TRUE;
}


void mesh1::endEdit(BOOL commit)
{
  assert (editing); // can't end if you didn't start

  char msg[128];
  if (!checkIntegrity(&msg[0]))
    MSG(msg, "mesh1 integrity check failed");

  initRenderableGeometry();

  if (commit)
    deleteSourceGeometry();

  editing = FALSE;
}


void mesh1::setFaceIndex(int *indices,int num)
{
  coordIndex.detachAll();
  coordIndex.add(indices, num);
}

void mesh1::setColorIndex(int *indices,int num)
{
  colorIndex.detachAll();
  colorIndex.add(indices, num);
}

void mesh1::setNormalIndex(int *indices,int num)
{
  normalIndex.detachAll();
  normalIndex.add(indices, num);
}

void mesh1::setTextureCoordIndex(int *indices,int num)
{
  texCoordIndex.detachAll();
  texCoordIndex.add(indices, num);
}


void mesh1::setVertices(vector *v, int num)
{
  if (vectors)
    delete vectors;

  vectors = new shared<vectorList>( new vectorList(num,16) );
  ((vectorList&)*vectors).add( v, num );
}

void mesh1::setNormals(normal *n, int num)
{
  if (normals)
    delete normals;

  normals = new shared<normalList>( new normalList(num,16) );
  ((normalList&)*normals).add( n, num );
}

void mesh1::setColors(rgbcolor *c, int num)
{
  if (colors)
    delete colors;

  colors = new shared<colorList>( new colorList(num,16) );
  ((colorList&)*colors).add( c, num );
}

void mesh1::setTextureCoordinates(texCoord *t, int num)
{
  if (texCoords)
    delete texCoords;
  texCoords = new shared<texCoordList>( new texCoordList(num,16) );
  ((texCoordList&)*texCoords).add( t, num );
}


BOOL mesh1::checkIntegrity(char *msg)
{
#ifndef _DEBUG
    return TRUE;
#else

  //not yet implemented
  return TRUE;

#endif
}
  

void mesh1::initRenderableGeometry()
{
// .......... get rid of any old data
  vertices.detachAll();
  indexedTris.detachAll();

// .......... check for degenerate cases
  if (!vectors || ((vectorList&)*vectors).count() == 0)
    return;

// .......... init the new data 


// .......... create the face array from the coordinate index info
  int j=0, numfaces = 0, IndexArraySize = coordIndex.count();

// .......... set up a reasonable sized array so we don't do a lot of reallocating
// .......... (use the expected number of triangles (i.e. verts / 4... div. by 4 not 3 because of the '-1' end of face marker)
  int resizeAmt = IndexArraySize >> 2;
  if (!resizeAmt)
    resizeAmt = 4;

// .......... this should handle the eventuality of quads if necessary to reallocate
  indexedTris.setDelta(resizeAmt);
  indexedTris.resize(resizeAmt);

// .......... setup the vertex array similarly
  int vertnum = ((vectorList&)*vectors).count();
  vertices.setDelta(16);

  BOOL normalsExist = FALSE;
  if (normals && ((normalList&)*normals).count() != 0)
       normalsExist = TRUE; 

  BOOL texCoordsExist = FALSE;
  if (texCoords && ((texCoordList&)*texCoords).count() != 0)
       texCoordsExist = TRUE; 

  BOOL colorsExist = FALSE;
  if (colors && ((colorList&)*colors).count() != 0)
       colorsExist = TRUE;

// .......... bounding box vars
  coord minx,maxx,miny,maxy,minz,maxz;

// .......... loop through the faces, constructing renderable vertices & face indices
  while (j < IndexArraySize)
    {
    int k = 0,val,checkQuad=0;
    face f;
    while ( ( val=coordIndex[j] ) != -1 )
      {
      // before constructing the face, re-index the vertices
      // in the face so that unused vertices are discarded.
      // This can happen if sharing source vertices between meshes   

      // construct next vertex...
      vertex v;

      // NOTE: we ignore color per vertex since we'd 
      // have to do our own lighting and render tris one by one
      // (it's a Direct 3D thing)
  
      v.x = ((vectorList&)*vectors)[val].x;
      v.y = ((vectorList&)*vectors)[val].y;
      v.z = ((vectorList&)*vectors)[val].z;

      // set normal according to the VRML conventions
      normal n;
      if (normalsExist)
        {
        if (normalPerVertex)
          {
          if (normalIndex.count() == 0)
            n = ((normalList&)*normals)[ val ];
          else
            n = ((normalList&)*normals)[ normalIndex[ j ] ];
          }
        else
          {
          if (normalIndex.count() == 0)
            n = ((normalList&)*normals)[ numfaces ];
          else
            n = ((normalList&)*normals)[ normalIndex[ numfaces ] ];
          }
        }
      else
         // we'll generate normals later
        {n.x = n.y = n.z = 0;}


      // set color per face/vertex according to the VRML conventions...
      if (colorsExist)
        {
        D3DCOLOR c;
        if (colorPerVertex)
          {
          if (colorIndex.count() == 0)
            c = ((colorList&)*colors)[ val ];
          else
            c = ((colorList&)*colors)[ colorIndex[ j ] ];
          }
        else
          {
          if (colorIndex.count() == 0)
            c = ((colorList&)*colors)[ numfaces ];
          else
            c = ((colorList&)*colors)[ colorIndex[ numfaces ] ];
          }
        }


      v.nx = n.x; v.ny = n.y; v.nz = n.z;
  
      texCoord t;
      // add texture coordinates according to the VRML conventions
      if (texCoordsExist)
        {
        if (texCoordIndex.count() == 0)
          t = ((texCoordList&)*texCoords)[ val ];
        else
          t = ((texCoordList&)*texCoords)[ texCoordIndex[ j ] ];
        }
      else
        {
        t.x = t.y = 0.5; // we should really use the VRML default texture mappings, but we probably won't need them
        }
      v.tu = t.x; v.tv = t.y;

      // now see if that vertex has already been added...
      // we check the new vertex including it's texture coordinates & normal to see 
      // if they are identical. Only then can we reuse the vertex...
      // All this because D3D wants the vertex, normal, & texcoords to be in the same structure

      int newVertexIndex = vertices.find( v );
      
      // not found, so add
      if (newVertexIndex == MAX_VECTLIST_INDEX)
        { 
        newVertexIndex = vertices.count();
        vertices.add( v );

        // bounding box is initially just a point
        if (newVertexIndex == 0)
          {
          minx = maxx = v.x;
          miny = maxy = v.y;
          minz = maxz = v.z;
          }
        else
          {
          // update the bounding box
          if (v.x < minx) minx = v.x;
          if (v.x > maxx) maxx = v.x;
          if (v.y < miny) miny = v.y;
          if (v.y > maxy) maxy = v.y;
          if (v.z < minz) minz = v.z;
          if (v.z > maxz) maxz = v.z;
          }
        }

      // and add the referenced vertex to the face
      f.verts[k++] = newVertexIndex;

      // is a triangle or quad completed ?
      if (k == 3)
        {
        if (checkQuad > 1)
          {
          MSG("more than 4 vertices specified in a face", "mesh1 error");
          return;
          }

        if (!ccw) // verts are specified ccw or cw (don't reverse if ccw)
          {
          // reverse before adding face
          int temp = f.verts[1];
          f.verts[1] = f.verts[2];
          f.verts[2] = temp;
          }

        indexedTris.add( f );
        numfaces++;

        k = 2; // where the index of the new vertex for a quad will be

        if (ccw) // verts are specified ccw, reverse 1 & 2 
          {
          // reverse after adding face, so if next tri is part of a quad the order is correct 
          int temp = f.verts[1];
          f.verts[1] = f.verts[2];
          f.verts[2] = temp;
          }

        checkQuad++;
        }
      j++;
      };
    j++;
    };

  if (!normalsExist)
    generateNormals();

// ..........   init the bounding box
  bbox.minx = minx; 
  bbox.maxx = maxx; 
  bbox.miny = miny; 
  bbox.maxy = maxy; 
  bbox.minz = minz; 
  bbox.maxz = maxz; 
  bboxDirty = FALSE;  

#ifdef _DEBUG
// .......... check the ratio of the number of destination vertices to source vertices
    int sourceVerts = ((vectorList&)*vectors).count(); 
    int destVerts = vertices.count();
  float vertexExpansion = ((float) destVerts) / sourceVerts;

// .......... set up an alarm condition to check that a mesh1 doesn't get ridiculously over expanded 
  if (vertexExpansion >= 1.5)
      DPF(DEBUG_ERROR, "%s: expanded vertices more than 1.5x source...dest / source: %d / %d", (char *) name, destVerts, sourceVerts);
#endif
}




// .......... hash value for performance on large meshes...we don't care since our meshes are realtime-small
int  mesh1::out_amhashvert( long v )
{
  return (v & 255L); // expect a simple ordering of vertices 1..n
}

int  mesh1::out_amvertsame( long v1, long v2 )
{
// ..........   just return simple equality test...we could use this to distinguish between
// ..........   input vertices with identical position but different additional attributes,
// ..........   or vertices with different indices that are actually the same vertex, but we hope
// ..........   that stuff doesn't happen often in practice.
  return v1 == v2;
}

mesh1 *mesh1::stripping=0;
int mesh1::nextStripAt = 0;
CvectList<int> *mesh1::reindex=0;


void mesh1::out_ambegin( int nverts, int ntris )
{
  assert(ntris);
  assert(stripping);
  assert(!stripping->stripData);
  assert(!stripping->stripIndices);
  assert(!reindex);

// .......... allocate a reasonable size array for the strips

  int delta = (nverts/4);
  if (!delta) delta = 2;

  // it's at least nverts, probably more
  stripping->stripData = new shared<vertexList>(new vertexList(nverts,delta));
  stripping->stripIndices = new CvectList<WORD>(4,4);

// .......... pre-size the temporary re-indexing array so we don't do a lot of dynamic reallocating
  reindex = new CvectList<int>(nverts);
  nextStripAt = 0;

// .......... expect 'out_amvertdata' call next to init reordered vertices
}


void mesh1::out_amend( void )
{

// ..........   delete the temporary vertex array created when re-indexing the vertices
  delete reindex;
  reindex = 0;
}


void mesh1::out_amvertdata( long fptr )
{
// ..........   just store the index to the original vertex
// .......... strips are output in terms of the reindexed vertices
  reindex->add((int)fptr);
}

void mesh1::out_ambgntmesh( void )
{
  stripping->stripIndices->add( nextStripAt );
}

void mesh1::out_amendtmesh( void )
{
  vertexList& sL = (vertexList&) *(stripping->stripData);
  nextStripAt = sL.count();
}

void mesh1::out_amswaptmesh( void )
{
// ..........   swap not supported; should never happen with 
// .......... new, modified triangle stripping code. Note that 
// .......... the outmesh function that calls this back does do at 
// .......... least one swap...AFAIK, this doesn't affect the mesh1
// .......... I don't know why it happens but the mesh1 seems to be output OK.
// .......... (Test mesh1 was the MAX2 teapot)
}

void mesh1::out_amvert( long index )
{
// ..........   add a new vertex to the array of strips
  vertexList& sL = (vertexList&) *(stripping->stripData);

// ..........   get the vertex from the original list 
  vertexList& original = (vertexList&) stripping->vertices;
  int orgIndex = (*reindex)[ index ];
  vertex& v = original [ orgIndex ];
  sL.add( v );
}


void mesh1::calcBBoxPts(vertex *v, int num)
{
  bbox.minx = bbox.miny = bbox.minz = FLT_MAX;
  bbox.maxx = bbox.maxy = bbox.maxz = -FLT_MAX;

  for (int i=0; i < num; i++)
  {
    // .......... update the bounding box
    if (v[i].x < bbox.minx) bbox.minx = v[i].x;
    if (v[i].y < bbox.miny) bbox.miny = v[i].y;
    if (v[i].z < bbox.minz) bbox.minz = v[i].z;

    if (v[i].x > bbox.maxx) bbox.maxx = v[i].x;
    if (v[i].y > bbox.maxy) bbox.maxy = v[i].y;
    if (v[i].z > bbox.maxz) bbox.maxz = v[i].z;
  }
  bboxDirty = FALSE;
}


void mesh1::calcBBox()
{
// ........... use strips if they're there, as they can be initialized
// ...........  or set and thus override the original triangle data
  if (stripData)
  {
    vertexList& sL = (vertexList&) *stripData;
    calcBBoxPts(&sL[0], sL.count());
  }
  else
    calcBBoxPts(&vertices[0], vertices.count());
}


BOOL mesh1::calcStrips(WORD **indices, int *numIndices, vertex **verts, int *numVerts)
{
  initStrips();
  vertexList& sL = (vertexList&) *stripData;

  if (verts)
    *verts = &sL[0];
  if (numVerts)
      *numVerts = sL.count();

  if( indices )
    *indices = &(*stripIndices)[0];
  if( numIndices )
    *numIndices = stripIndices->count();

  return TRUE;
}

BOOL mesh1::setStrips(WORD *indices, int numIndices, vertex *verts, int numVerts)
{
  deleteStrips();
  stripData = new shared<vertexList>(new vertexList(numVerts,16));
  stripIndices = new CvectList<WORD>(numIndices,4);
  ((vertexList&)*stripData).add( verts, numVerts );
  stripIndices->add( indices, numIndices );
  bboxDirty = TRUE;
  return TRUE;
}


void mesh1::deleteStrips()
{
  // .......... get rid of any old strips
  if (stripData)
  {
    delete stripData;
    delete stripIndices;
    stripData = 0;
    stripIndices = 0;
  }
}


void mesh1::initStrips()
{
  deleteStrips();

// .......... use the Microsoft/SGI derived package to return triangle strips
// .......... with vertices given as indices into the original array. While reading off
// .......... the strip data, we reconstruct new vertex arrays so that we don't cause
// .......... the entire orginal data set to be copied in the 3D driver when drawing
// .......... each Direct 3D tri-strip primitive.

  stripping = this;

  Meshobj *m = newMeshobj(
    out_ambegin,
    out_amend,
    out_amhashvert,
    out_amvertsame,
    out_amvertdata,
    out_ambgntmesh,
    out_amendtmesh,
    out_amswaptmesh,
    out_amvert );

  in_ambegin( m );

// .......... setup the individual triangles
  int i;
  for (i=0; i < indexedTris.count(); i++)
  {
    in_amnewtri( m );
    in_amvert( m, indexedTris[i].verts[0] );
    in_amvert( m, indexedTris[i].verts[1] );
    in_amvert( m, indexedTris[i].verts[2] );
  }

// .......... this should call back the funcs with strip info...
// .......... the strips are constructed within the callbacks
  in_amend( m );
  
  freeMeshobj( m );
  stripping = 0;
}


vertexList& mesh1::getStripVertices()
{
  assert (stripData); // must have calculate strips
  return (vertexList&) *stripData;
}

CvectList<WORD>& mesh1::getStripIndices()   
{
  assert (stripIndices); // must have calculate strips
  return  *stripIndices;
}

int mesh1::getType() const {return 1;} // by convention, just return the mesh class name #

void mesh1::generateNormals()
{
  vertexList &vl = (vertexList&) vertices;
  unsigned i,num = indexedTris.count();
  vector ab,ac,abxac;

  // thre's no unique way to generate normals with no other info,
  // so just init all the vertices using the cross product of the face edges
  // some normals will be calc'd more than once...not efficient, but it's just for convenience
  for (i = 0; i < num; i++)
  {
    face& f1 = indexedTris[i];  
    vertex& a = vl[f1.verts[0]];  
    vertex& b = vl[f1.verts[1]];  
    vertex& c = vl[f1.verts[2]];  
    vector va(a.x, a.y, a.z);
    vector vb(b.x, b.y, b.z);
    vector vc(c.x, c.y, c.z);
    ab = vb - va; 
    ac = vc - va; 
    abxac = CrossProduct(ab, ac);

    if (abxac.x || abxac.y || abxac.z)
      abxac = Normalize(abxac);

    // all three vertices of the tri share the same normal
    a.nx = b.nx = c.nx = abxac.x; 
    a.ny = b.ny = c.ny = abxac.y; 
    a.nz = b.nz = c.nz = abxac.z; 
  }
}


// ------------------------------------------------------
// .......... utility / enhanced functionality...
// ------------------------------------------------------


void mesh1::addSphere(coord radius, vector offset, int meshSize)
{
  int i, j;
  float   dj = PIFLOAT/(meshSize+1.0f);
  float di = PIFLOAT/meshSize;

// .......... temp vertex / face holding array
  int SPHERE_VERTICES = (2+meshSize*meshSize*2);
  int SPHERE_INDICES = ((meshSize*4 + meshSize*4*(meshSize-1))*3);

  WORD *sphere_indices = new WORD[SPHERE_INDICES];
  vector *spherevect = new vector[SPHERE_VERTICES];
  vector *spherenorm = new normal[SPHERE_VERTICES];
  texCoord *sphereuv = new texCoord[SPHERE_VERTICES];
  int *sphere_faces = new int[(SPHERE_INDICES << 2)/3];

  


  // generate the sphere data

  // vertices 0 and 1 are the north and south poles
  spherevect[0] = vector(offset.x,  radius+offset.y, offset.z);
  spherenorm[0] = Normalize(vector(0.0f, 1.0f, 0.0f));
  sphereuv[0] = texCoord(0.0f, 0.0f);

  spherevect[1] = vector(offset.x,  -radius+offset.y, offset.z);
  spherenorm[1] = Normalize(vector(0.0f, -1.0f, 0.0f));
  sphereuv[1] = texCoord(1.0f, 1.0f);

  for (j=0; j<meshSize; j++) {
    for (i=0; i<meshSize*2; i++) {
      vector  p,n;
      float   u, v;

      n.y = (float) cos((j+1) * dj);
      n.x = (float) sin(i * di) * (float) sin((j+1) * dj);
      n.z = (float) cos(i * di) * (float) sin((j+1) * dj);

      p.y = (float) ( radius * n.y ); p.y += offset.y;
      p.x = (float) ( radius * n.x ); p.x += offset.x;
      p.z = (float) ( radius * n.z ); p.z += offset.z;

      u = (float)i/meshSize;
      if (u>1.0f) 
        u -= 1.0f;
      u = 1.0f - u; // flip so texture is not mirrored
      v = (float)j/meshSize;

      int vertIndex = 2+i+j*meshSize*2;
      spherevect[vertIndex] = p;
      spherenorm[vertIndex] = n;
      sphereuv[vertIndex] = texCoord(u,v);
    }
  }

  // now generate the traingle indices
  // strip around north pole first
  for (i=0; i<meshSize*2; i++) {
    sphere_indices[3*i] = 0;
    sphere_indices[3*i+1] = i+2;
    sphere_indices[3*i+2] = i+3;
    if (i==meshSize*2-1)
      sphere_indices[3*i+2] = 2;
  }

  // now all the middle strips
  int v;    // vertex offset
  int ind;  // indices offset
  for (j=0; j<meshSize-1; j++) {
    v = 2+j*meshSize*2;
    ind = 3*meshSize*2 + j*6*meshSize*2;
    for (i=0; i<meshSize*2; i++) {
      sphere_indices[6*i+ind] = v+i;
      sphere_indices[6*i+2+ind] = v+i+1;
      sphere_indices[6*i+1+ind] = v+i+meshSize*2;

      sphere_indices[6*i+ind+3] = v+i+meshSize*2;
      sphere_indices[6*i+2+ind+3] = v+i+1;
      sphere_indices[6*i+1+ind+3] = v+i+meshSize*2+1;
      if (i==meshSize*2-1) {
        sphere_indices[6*i+2+ind] = v+i+1-2*meshSize;
        sphere_indices[6*i+2+ind+3] = v+i+1-2*meshSize;
        sphere_indices[6*i+1+ind+3] = v+i+meshSize*2+1-2*meshSize;
      }
    }
  }

  // finally strip around south pole
  v = SPHERE_VERTICES-meshSize*2;
  ind = SPHERE_INDICES-3*meshSize*2;
  for (i=0; i<meshSize*2; i++) {
    sphere_indices[3*i+ind] = 1;
    sphere_indices[3*i+1+ind] = v+i+1;
    sphere_indices[3*i+2+ind] = v+i;
    if (i==meshSize*2-1)
      sphere_indices[3*i+1+ind] = v;
  }

// .......... setup the mesh1...didn't need to use a separate temp array, 
// .......... but it was convenient to just use the sphere generate function as it was found
        for (i=0; i < SPHERE_INDICES/3; i++)
    {
    sphere_faces[i*4] = sphere_indices[i*3];
    sphere_faces[i*4+1] = sphere_indices[i*3+1];
    sphere_faces[i*4+2] = sphere_indices[i*3+2];
    sphere_faces[i*4+3] = -1; // end of face marker
    }

  beginEdit();

  // set the mesh1's face data 
  setFaceIndex(sphere_faces,(SPHERE_INDICES << 2)/3);

  // set the mesh1's vertex,normal,color,&texture coordinate data 
  setVertices(spherevect, SPHERE_VERTICES);
  setNormals(spherenorm, SPHERE_VERTICES);
  setTextureCoordinates(sphereuv, SPHERE_VERTICES);

  endEdit();

  
// .......... get rid of the temp stuff
  delete spherevect;
  delete spherenorm;
  delete sphereuv;
  delete sphere_faces;
  delete sphere_indices;
}



//void mesh1::load(Tag& name)
//
//void mesh1::saveRenderableGeometry(Tag& name)
//{
//
//}
