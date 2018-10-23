#ifndef MESH1_H
#define MESH1_H

#include "l_mesh.h"

struct texCoord { float x, y; texCoord(float ix,float iy) : x(ix), y(iy) {} texCoord(){}};
struct face   {WORD verts[3];};

inline int operator ==(vertex&a, vertex&b) {return !memcmp(&a,&b,sizeof(vertex));}
inline int operator ==(texCoord&a, texCoord&b) {return !memcmp(&a,&b,sizeof(texCoord));}
inline int operator ==(face&a, face&b) {return !memcmp(&a,&b,sizeof(face));}

typedef CvectList<vertex>   vertexList;
typedef CvectList<face>     faceList;
typedef CvectList<vector>   vectorList;
typedef CvectList<rgbcolor> colorList;
typedef CvectList<texCoord> texCoordList;
typedef CvectList<normal>   normalList;
typedef CvectList<int>      indexList;

// This class is meant to support generalized mesh rendering, with an interface
// that corresponds directly to the VRML 2.0 IndexedFaceSet specification. 
// Take a look at the VRMl docs for info on how this interface works.
//   In addition to being directly renderable in realtime, This class
// is designed to be independent of any 3D system. It can be called 
// inside a utility to process or generate mesh data, etc. 
class mesh1 : public mesh {
  
private:
  
  // .......... callbacks & variables related to the SGI/Microsoft triangle stripping module
  static mesh1 *  stripping;
  static int  nextStripAt;
  static CvectList<int> *reindex;
  
  static int out_amhashvert( long v );
  static int out_amvertsame( long v1, long v2 );
  static void out_ambegin( int nverts, int ntris );
  static void out_amend( void );
  static void out_amvertdata( long fptr );
  static void out_ambgntmesh( void );
  static void out_amendtmesh( void );
  static void out_amswaptmesh( void );
  static void out_amvert( long index );
  // .......... end of callbacks & variables related to the SGI/Microsoft triangle stripping module
  
  static CvectList<mesh*> allMeshes;
  
protected:
public: // make this public for anyone to access if they know what they're doing :)
  
  indexList     coordIndex;
  indexList     normalIndex;
  indexList     texCoordIndex;
  indexList     colorIndex;
  
  shared<vectorList>    *vectors;
  shared<colorList>     *colors;
  shared<texCoordList>  *texCoords;
  shared<normalList>    *normals;
  
  vertexList      vertices;
  faceList      indexedTris;
  
  shared<vertexList>    *stripData;
  CvectList<WORD>     *stripIndices;
  
  
  Tag         name;

  MATERIALDATA  Mat;
  Tag           textureName;
  Surface *     textureSurf;
  
  BOOL        editing;
  BOOL        colorPerVertex;
  BOOL        normalPerVertex;  
  BOOL        solid, ccw;
  BOOL        bboxDirty;
  Box         bbox;
  
  void    initStrips();
  void    deleteStrips();
  void    calcBBoxPts(vertex *v, int num);
  void    calcBBox();
  
  void    initRenderableGeometry();
  void    initSourceGeometry();
  void    deleteSourceGeometry();
  
  
public:
  enum    {MESH};
  virtual int   getType() const;
  virtual void  getBox(Box *bounds) {if (bboxDirty) {calcBBox();} *bounds = bbox;};
  
  mesh1(Tag& t);
  virtual        ~mesh1();
  const Tag&      getName() const {return name;}
  
  // modify the mesh within a begin/endEdit pair. This protocol 
  // allows modification in terms of VRML-based indices & arrays
  // before organizing the data into more compact and ready-to-render
  // device-dependent format. if 'commit' is true, the VRML-based 
  // information stored in the mesh is deleted when done editing. If it
  // is not deleted, it will still be available for editing. It is recommended
  // that you set commit = TRUE to save memory if you don't need to edit the mesh later.
  
  void    beginEdit();
  void    endEdit(BOOL commit=TRUE);
  
  // these mesh editing functions must be called within a beginEdit / endEdit pair
  // NOTE: no function calls other than those designated should be called within
  // the edit pair. Although some may work, they are not guaranteed to.
  
  // set the indices defining the mesh's faces... 
  // i.e. vertices, normals, texture coordinates & color
  // (see the VRML 2 IndexedFaceSet node spec. for info on how these work... 
  // currently only triangle & quad data are supported -cc)
  void    setFaceIndex(int *indices,int num);
  void    setColorIndex(int *indices,int num);
  void    setNormalIndex(int *indices,int num);
  void    setTextureCoordIndex(int *indices,int num);
  
  // set the mesh's vertex,normal,color,&texture coordinate data 
  void    setVertices(vector *v, int num);
  void    setNormals(normal *n, int num);
  void    setColors(rgbcolor *c, int num);
  void    setTextureCoordinates(texCoord *t, int num);
  
  // similar to above, but shares the data of another mesh...
  // these calls will do nothing if the shared mesh's data has been committed
  void    setVertices(mesh *shareWith);
  void    setNormals(mesh *shareWith);
  void    setColors(mesh *shareWith);
  void    setTextureCoordinates(mesh *shareWith);
  
  void    setColorPerVertex(BOOL b) {colorPerVertex = b;}
  BOOL    usesColorPerVertex() {return colorPerVertex;}
  
  void    setNormalPerVertex(BOOL b) {normalPerVertex = b;}
  BOOL    usesNormalPerVertex() {return normalPerVertex;}
  
  void    setSolidFlag( BOOL isSolid ) {solid = isSolid;}
  BOOL    isSolid() const {return solid;}
  
  void    setCCWFlag( BOOL isCCW ) {ccw = isCCW;}
  BOOL    isCCW() const {return ccw;}
  
  // check for mesh specification errors. if an error occurs, a string 'msg' will be initialized if 'msg' is non-null 
  // not yet implemented
  BOOL    checkIntegrity(char *msg);
  
  // automatically generate normals using a naive method
  void    generateNormals();
  
  // end of beginEdit / endEdit functions
  
  // converts the edited mesh data to triangular strips for faster
  // processing down the rasterizing pipeline. The triangle strips can
  // optionally be set via 'setStrips'. If strips are set or calculated
  // they can later be used in a rendering routine.
  BOOL    calcStrips(WORD **indices, int *numIndices, vertex **verts, int *numVerts);
  BOOL    setStrips(WORD *indices, int numIndices, vertex *verts, int numVerts);
    vertexList&     getStripVertices();
  CvectList<WORD>&  getStripIndices();    

  int   getNumFaces() {assert(!editing); return indexedTris.count();}
  int   getNumVertices() {assert(!editing); return ((vertexList)vertices).count();}
  
  void    setDefaultMaterialInfo(PMATERIALDATA i) { Mat = *i;}
  PMATERIALDATA getDefaultMaterialInfo() { return &Mat;}

  // .......... optional texture to use for the mesh...
  // .......... used in an application-specific context
  void    setDefaultTexture( Surface* pTexture ) { textureSurf = pTexture; }
  Surface* getDefaultTexture() { return textureSurf; }

  // ..........   optional texture name to use for the mesh...
  // .......... used in an application-specific context
  void    setDefaultTextureName( const Tag& t) { textureName = t;}
  const Tag & getDefaultTextureName() const {return textureName;}
  
  // .......... utility / enhanced functionality...
  
  // adds a sphere (dynamically generated) to the mesh
  void addSphere(coord radius=1.0, vector offset=vector(0,0,0), int meshSize=8);
  
  // save to a file (not yet implemented)
  void saveRenderableGeometry(Tag& name);
  
  friend BOOL render(mesh1* m, view* in);

};/* end class mesh */

// .......... basic renderer for a mesh in a given view
BOOL render(mesh1 *m, view* in);

#endif