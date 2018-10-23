#ifndef MESH2_H
#define MESH2_H

// A mesh class ( 3D point-based geometry and texture ).
// It is meant for special purpose, real-time mesh generation.
// It has a smaller memory overhead to store the geometry, resulting
// in imposed limitations on its flexibility. There are no
// duplicated vertices (due to different texture coordinates or 
// normals), except those that are explicitly added. This (hopefully) ensures vertices 
// are transformed only once during rendering. It cannot be turned into 
// strips prior to rendering. It calculates but does not retain bounding box info. 
// The vertices are accessible for manipulation directly, and they are
// guaranteed to be stored in the same order in which they were added.

class view;

class mesh2
{
	
	int					vType,vSize;
	int					vCount,tCount;
	int					tAlloc;

	int 				wOffset;
	int 				normalOffset;
	int 				diffuseOffset;
	int 				specularOffset;
	int 				uvOffset;
	
	void *				verts;
	WORD *				indices;

	// calculates the size in bytes of a given vertex type,
	// and offsets to the various components within it
	void calcVertexSizeAndOffsets(int vertexType);


public:
	
	// constructs a mesh of a maximum of 'numVerts' and 'maxTris', if 'maxTris' is
	// not specified, we use the convention that a maximum of 2 x numVerts 
	// faces will be used. Avoids having to specify in advance how many
	// faces the mesh will have...the onus is on the user to make sure it's no more than
	// twice the alloted # of verts. 'vertsInit' is optional. If specified, it must 
	// be an array of 'numVerts' vertices of the given 'vertexType'. If not specified.
	// the vertices are allocated but uninitialized. 
			mesh2(int vertexType, int numVerts, int maxTris=0, void *vertsInit = 0);
		   ~mesh2();

	// returns the # bytes between successive vertices
	int		getVertexSize() {return vSize;}

	// returns the type of the vertex
	int		getVertexType() {return vType;}

	// returns the #tris or #vertices added 
	int	    getNumTris() {return tCount;}
	int	    getNumVertices() {return vCount;}

	// retrieves a reference to vertex 'i'. If user must access other fields of the specific vertex type, 
	// it must be re-cast to an appropriate flexible vertex structure. The 'vertex' type is equivalent
	//  to the flexVertex1 type
    vertex&	operator [](int i) { assert(i < vCount); return *((vertex*) (void *) (((char*)verts)+(i * vSize))); }

	// retrieve a reference to the 'ith' normal component, if any, as a vector, 
	vector&		getNormal(int i)
							{
							assert(normalOffset != -1); // normals must be there
							return *((vector*) (void *) ( ((char*)verts) + (i * vSize) + normalOffset ) );
							}

	// adds a triangle (indexed) at the end of the triangle array. 
	// Does not check to ensure indices correspond to valid vertices
    void addTri(int a, int b, int c)
			{assert(tCount < tAlloc); // can't add more faces than were allocated
			WORD *ptr = indices + ((tCount << 1)+tCount);
			*ptr++ = a;
			*ptr++ = b;
			*ptr = c;
			tCount++;}

	// returns the indices of the ith triangle
    void    getTri(int i, WORD *a, WORD *b, WORD *c)
			{
			assert(i < tCount);
			WORD *ptr = indices + ((i << 1)+i);
			*a = *ptr++;
			*b = *ptr++;
			*c = *ptr;
			}

	// optimize the mesh for drawing. No changes can be made to the
	// mesh after an optimize call; mesh must remain static.
	void optimize();

	// automatically generate normals using a naive method, 
	// does nothing if normals are not part of the vertex type
	void    generateNormals();
	
    // get the bounding box
	virtual void getBox(Box *bounds);

  friend BOOL render(mesh2* m, view* in);
};/* end class mesh2 */


// basic renderer for a mesh in a given view
BOOL render(mesh2* m, view* in);


// sample object generators
mesh2 *newBox(int xPts,int yPts, int zPts, float xDim, float yDim, int zDim);
mesh2 *newBoid(float size);
mesh2 *newSphere(float radius);

#endif
