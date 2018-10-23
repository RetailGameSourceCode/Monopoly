#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pc3d.h"
#include "vrml.h"
#include "xvrml.h"
#include "can.h"
#include "cantyp.h"
#include "vrml2can.h"
#include "d3dutils.h"
#include "iostream.h"

struct xformBinding {
	frame *f;
	short ID;
	BOOL hasRoute;
	xformBinding() : f(0), ID(0), hasRoute(FALSE) {}
};

struct frameRoute {
	route r;
	frame *f; // the frame that the routed event affects

	frameRoute(route& rte) : r(rte), f(0) {}
	frameRoute() : r(), f(0) {}
};

// Tables used to hold info about the model and routes
char         textureNames[MAX_TEXTURES_IN_LIST][MAX_TEXTURE_NAME_LEN+1];
int          textureIndex;
MatInfo      materials[MAX_MATERIALS_IN_LIST];
int		       materialIndex;
animFrame3D_CHUNK anims[MAX_ANIM_FRAMES];
int          numAnimFrames;
frameTiming_DATA overallTiming;

// Tables used while building
const        MAX_TRANSFORM_BINDINGS = 1024;
xformBinding IDtoFrame[MAX_TRANSFORM_BINDINGS];
int          numXformBindings;
frameRoute   framePRS[MAX_ROUTES];
int          frameRoutesCount;

// IDs used while building
short xformID;
short geometryID;



// .......... .CAN FILE CONVERSION STUFF ................................

// .......... MODEL CONVERSION STUFF...

#define FLOAT2FIX(n)	(FIXEDPSX)((n)*4096.0)

inline SFIXED vertex2PSX(float f, float scalemult)
	{
	// +/- 100 corresponds to the the full short int (16 bit range)
	if (f > 100.0 || f < -100.0)
		{
		MSG("vertex component out of bounds","conversion error");
		exit(-1);
		}
	float retf = f * 327.67f;
	SFIXED ret = (SFIXED) retf; // truncate the decimal points
	return ret;
	}

inline 	FIXEDPSX trans2PSX(float f, float scalemult)
	{
	float limit = (float) ((32767.0 * 32767.0) / 327.67f);
	if (f > limit || f < -limit)
		{
		MSG("matrix translation component out of bounds","conversion error");
		exit(-1);
		}
	float retf = f * 327.67f;
	FIXEDPSX ret = ( FIXEDPSX ) retf; // truncate the decimal points
	return ret;
	}

inline SFIXED rot2PSX(float f, float scalemult)
	{
	if (f > (7.0) || f < (-7.0) ) // actually the max range is 7.9 or something (1,3,12)
		{
		MSG("matrix rotation / scale component out of bounds","conversion error");
		exit(-1);
		}
 	float retf = f * 4096.0f;
	FIXEDPSX ret = ( FIXEDPSX ) retf; // truncate the decimal points
	return (SFIXED) ret;

/* 	float retf = f * 327.67f;
	FIXEDPSX ret = ( FIXEDPSX ) retf; // truncate the decimal points

	int neg = 0;
	if (f < 0.0)
		{
		f = -f;
		neg= 1;
		}

	float modf = (float) fmod( f, 1.0 );
	float scaledmodf = modf * 4096.0f;
	SFIXED modportion = (SFIXED) scaledmodf; // the 12 bit decimal portion
	assert(modportion >= 0);

	float scaledf = (f-modf) * 4096.0f;
	SFIXED intportion = (SFIXED) scaledf;
	assert(intportion >= 0);
	SFIXED result = intportion + modportion;

	assert (result == (intportion | modportion)); // should be the same thing

	if (neg && (result != 0))
		result |= 0x8000; // the negative bit flag
	
	return result;
  */
	}

inline SFIXED color2PSX(float f, float scalemult)
	{
	if (f > (1.0) || f < (-1.0) ) 
		{
		MSG("color component out of bounds","conversion error");
		exit(-1);
		}
 	return rot2PSX(f, scalemult);
	}

inline SFIXED uv2PSX(float f, float scalemult)
	{
	if (f > (1.0) || f < (-1.0) ) 
		{
		MSG("texture coordinate component out of bounds","conversion error");
		exit(-1);
		}
    //	float f2 =(f - floor(f));
	return rot2PSX(f, scalemult);
	}


inline SFIXED float2PSX(float f, float scalemult)
	{
	assert(0);
	return (SFIXED) FLOAT2FIX( (f / scalemult) );
	}

float fixedPtScaleFactorPSX = 1.0; // the global scale that applies to the chunks currently being written
BOOL convToPSX =FALSE;
enum platformTypesPSX { CONV_MATRIX, CONV_VERTEX, CONV_BOX };

void VRML2CAN_setConvertToPSXFlag(BOOL flag) {
	convToPSX = flag;
}

// max 1 megabyte data size on PSX
const MAX_DATA_SIZE_PSX = 1024*1024; 
char tempForPSX[MAX_DATA_SIZE_PSX];

// converts PC float values to the PSX "normalized" equivalent
int convertVertex(void *data, void *tempForPSX, int len)
	{
	int i=-1;
	char TheString[250];

	int num = len / sizeof (vertex_DATA); 
	assert(num * sizeof(vertex_DATA_PSX) < MAX_DATA_SIZE_PSX);

	vertex_DATA *pv = (vertex_DATA *) data;
	vertex_DATA_PSX *pvPSX = (vertex_DATA_PSX *) tempForPSX;
//	sprintf(TheString,"Num = %d\n",num);
  //	OutputDebugString(TheString);
	while (++i < num)
		{
		pvPSX[i].x =  vertex2PSX( pv[i].x, fixedPtScaleFactorPSX);
		pvPSX[i].y =  vertex2PSX( pv[i].y, fixedPtScaleFactorPSX);//in the pxs, -y axis is up.
		pvPSX[i].z =  vertex2PSX( pv[i].z, fixedPtScaleFactorPSX);
		pvPSX[i].nx =  color2PSX( pv[i].nx, 1); // this is vertex color
		pvPSX[i].ny =  color2PSX( pv[i].ny, 1);
		pvPSX[i].nz =  color2PSX( pv[i].nz, 1);
		pvPSX[i].u =  uv2PSX( pv[i].u, fixedPtScaleFactorPSX);
		pvPSX[i].v = uv2PSX( pv[i].v, fixedPtScaleFactorPSX);

//		sprintf(TheString,"Vert [%4d]  In: %9.5f %9.5f %9.5f    Out: %6d %6d %6d\n",i , pv[i].x, pv[i].y, pv[i].z, 
//																pvPSX[i].x, pvPSX[i].y, pvPSX[i].z);
		OutputDebugString(TheString);
	}
	return num * sizeof(vertex_DATA_PSX);
	}

int convertBox(void *data, void *tempForPSX, int len)
	{
	int i = -1;
	int num = len / sizeof (box_DATA); 
	assert(num * sizeof(box_DATA_PSX) < MAX_DATA_SIZE_PSX);

	box_DATA *pb = (box_DATA *) data;
	box_DATA_PSX *pbPSX = (box_DATA_PSX *) tempForPSX;
/*
	while (++i < num)
		{
		pbPSX[i].minx =  vertex2PSX( pb[i].minx, fixedPtScaleFactorPSX );
		pbPSX[i].miny =  vertex2PSX( pb[i].miny, fixedPtScaleFactorPSX );
		pbPSX[i].minz =  vertex2PSX( pb[i].minz, fixedPtScaleFactorPSX );
		pbPSX[i].maxx =  vertex2PSX( pb[i].maxx, fixedPtScaleFactorPSX );
		pbPSX[i].maxy =  vertex2PSX( pb[i].maxy, fixedPtScaleFactorPSX );
		pbPSX[i].maxz =  vertex2PSX( pb[i].maxz, fixedPtScaleFactorPSX );
		}
		*/ //comented out cause not used and was making the converter stop.....vertex out of the -1,1 range
	return num * sizeof(box_DATA_PSX);
	}

int convertMatrix(void *data, void *tempForPSX, int len)
	{
	char TheString[250];
	int i = -1;
	int num = len / sizeof (matrix_DATA); 
	assert(num * sizeof(matrix_DATA_PSX) < MAX_DATA_SIZE_PSX);

	matrix_DATA *pm = (matrix_DATA *) data;
	matrix_DATA_PSX *pmPSX = (matrix_DATA_PSX *) tempForPSX;
	while (++i < num)
		{
		// converts PC float matrices to the PSX "normalized" equivalent
		int c,r;
		//D3DMATRIX scalemat = ScaleMatrix((1.0/fixedPtScaleFactorPSX));
		D3DMATRIX newmat;
		D3DMATRIX *matdata;

		matdata = (D3DMATRIX *)&pm[i];
	    // newmat = MatrixMult(scalemat,*matdata);
	    newmat = *matdata;

		for(r=0;r<3;r++)
		{
			for(c=0;c<3;c++)
			{
			    pmPSX[i].m[c][r] = rot2PSX( newmat.m[c][r] , fixedPtScaleFactorPSX);
				//pmPSX[i].m[c][r] = (SFIXED)FLOAT2FIX( (pm[i].m[c][r]) );
				//pmPSX[i].m[c][r] = (SFIXED)FLOAT2FIX( (pm[i].m[c][r] / fixedPtScaleFactorPSX) );
				sprintf(TheString, "%f ", (pm[i].m[c][r]) );   //before convert
				//sprintf(TheString, "%d ", pmPSX[i].m[c][r] );//after convert psx format
				OutputDebugString(TheString);
			}
			OutputDebugString("\n");
		}

		for(c=0;c<3;c++)
		{
		    pmPSX[i].t[c] = trans2PSX( newmat.m[3][c], fixedPtScaleFactorPSX );
			//pmPSX[i].t[c] = FLOAT2FIX( (pm[i].m[3][c]) );
			//pmPSX[i].t[c] = FLOAT2FIX( (pm[i].m[3][c] / fixedPtScaleFactorPSX) );
			sprintf(TheString, "%f ", pm[i].m[3][c] );		//before convert
			//sprintf(TheString, "%d ", pmPSX[i].t[c] );	//after convert	in psx format
			OutputDebugString(TheString);
		}
		OutputDebugString("\n");
		OutputDebugString("\n");
		}

	return num * sizeof(matrix_DATA_PSX);
	}


BOOL setChunkDataPlatformAware(int canID, void *data, int len, platformTypesPSX datatype)
{
if  (!convToPSX) // just use the normal PC chunks
	return setChunkData( canID, data, len);
	
int lenForPSX;
switch (datatype) {
		case CONV_VERTEX:
			lenForPSX = convertVertex(data, tempForPSX, len);
			break;
		case CONV_BOX:
			lenForPSX = convertBox(data, tempForPSX, len);
			break;
		case CONV_MATRIX:
			lenForPSX = convertMatrix(data, tempForPSX, len);
			break;
		default:
			MSG("no PSX conversion defined for that type", "error");
			return FALSE;
			};

return setChunkData( canID, tempForPSX, lenForPSX );
}



// .......... error reporing macro
#define ERR(x)	if ((x) == FALSE)\
			{\
			fprintf(stdout,"error on call: %s\n",#x);\
			return FALSE;\
			}

// .......... transforms a box's endpoints by the given matrix and
// .......... updates another 'bounds' box to enclose the 
// .......... resulting transformed points of the box 'b'
void updateBox( struct box_DATA& bounds, struct box_DATA& b, matrix& m)
{
	// 8 points for the box corners
	vector boxPt[8];
	boxPt[0] = vector(b.minx,b.miny,b.minz);
	boxPt[1] = vector(b.minx,b.miny,b.maxz);
	boxPt[2] = vector(b.minx,b.maxy,b.minz);
	boxPt[3] = vector(b.minx,b.maxy,b.maxz);
	boxPt[4] = vector(b.maxx,b.miny,b.minz);
	boxPt[5] = vector(b.maxx,b.miny,b.maxz);
	boxPt[6] = vector(b.maxx,b.maxy,b.minz);
	boxPt[7] = vector(b.maxx,b.maxy,b.maxz);
	for ( int j = 0; j < 8; j++)
		{
		boxPt[ j ] = TransformVector(boxPt[ j ], m);

		// .......... update the bounding box
		if (boxPt[j].x < bounds.minx) bounds.minx = boxPt[j].x;
		if (boxPt[j].y < bounds.miny) bounds.miny = boxPt[j].y;
		if (boxPt[j].z < bounds.minz) bounds.minz = boxPt[j].z;
		if (boxPt[j].x > bounds.maxx) bounds.maxx = boxPt[j].x;
		if (boxPt[j].y > bounds.maxy) bounds.maxy = boxPt[j].y;
		if (boxPt[j].z > bounds.maxz) bounds.maxz = boxPt[j].z;
		}
}


// .......... adds a material to the list if not already there
// .......... and returns an index to it
int addMaterial(MatInfo *m)
	{
	int i; 
	for (i = 0; i < materialIndex; i++ )
		if (!memcmp( &materials[i], m, sizeof(MatInfo) ) )
			return i;
	assert (materialIndex <	MAX_MATERIALS_IN_LIST); // too many materials !

	materials[ materialIndex ] = *m;
	return materialIndex++; 
	}


// .......... adds a texture to the list if not already there
// .......... and returns an index to it
int addTexture(char *name)
	{
	int i;
	assert (strlen(name) <= MAX_TEXTURE_NAME_LEN); // name too long

	for (i = 0; i < textureIndex; i++ )
		if ( !stricmp(&textureNames[i][0], name ) )
			return i;

	assert (textureIndex <	MAX_TEXTURES_IN_LIST); // too many textures !

	strcpy( &textureNames[textureIndex][0], name);
	return textureIndex++; 
	}


void addTransformBinding( short xformID, frame *f )
{
int i; 
for (i = 0; i < numXformBindings; i++)
	assert (IDtoFrame[i].f != f); // can't already exist

assert (numXformBindings <	MAX_TRANSFORM_BINDINGS); // too many bindings !
IDtoFrame[numXformBindings].f = f;
IDtoFrame[numXformBindings].ID = xformID;
numXformBindings++;
}


void writeGeometryTreeChunk(int canID, struct geometryTree_CHUNK *tree)
	{
	int i;
	createChunk( canID, CAN_TYPEDEF(geometryTree_CHUNK), -1, 0 );
	setChunkData( canID, &tree->ID, sizeof(int) );
	setChunkData( canID, &tree->billboard, sizeof(int) );
	setChunkDataPlatformAware( canID, &tree->bounds, sizeof(box_DATA), CONV_BOX );
	setChunkDataPlatformAware( canID, &tree->xform, sizeof(matrix_DATA), CONV_MATRIX );
	setChunkData( canID, &tree->numLights, sizeof(int) );
	setChunkData( canID, &tree->numGeometry, sizeof(int) );
	setChunkData( canID, &tree->numTrees, sizeof(int) );

	// skip light sub-chunks for now, there are none
	
	// .......... write geometry chunks
		descendChunk( canID );
		for (i=0; i < tree->numGeometry; i++)
			{
			createChunk( canID, CAN_TYPEDEF(geometry_CHUNK), -1, 0 );
			setChunkData( canID, &tree->geometry[i].ID, sizeof(int) );
			setChunkData( canID, &tree->geometry[i].materialIndex, sizeof(int) );
			setChunkData( canID, &tree->geometry[i].textureIndex, sizeof(int) );

			// write the vertexList_DATA structure
			int numVerts = tree->geometry[i].v.numVerts;
			vertex_DATA *verts = tree->geometry[i].v.verts;
			setChunkData( canID, &numVerts, sizeof(int) );
			setChunkDataPlatformAware( canID, verts, sizeof(vertex_DATA) * numVerts, CONV_VERTEX);

			// write the stripIndexList_DATA structure
			int numIndices = tree->geometry[i].i.numIndices;
			short *indices = tree->geometry[i].i.indices;
			setChunkData( canID, &numIndices, sizeof(int) );
			setChunkData( canID, indices, sizeof(short) * numIndices);
			}

	// .......... write child tree chunks, recursively

		for (i=0; i < tree->numTrees; i++)
			writeGeometryTreeChunk( canID, &tree->children[i] );
		ascendChunk( canID );
	}

void VRML2CAN_writeModelChunks(int canID, struct model3D_CHUNK *model, char *name)
	{
	int i;
	createChunk( canID, CAN_TYPEDEF(model3D_CHUNK), -1, name );
	setChunkDataPlatformAware( canID, &model->bounding, sizeof(box_DATA), CONV_BOX );
		descendChunk( canID );

		createChunk( canID, CAN_TYPEDEF(materialList_CHUNK), -1, 0 );
		setChunkData( canID, &model->materials.numMaterials, sizeof(int) );
			descendChunk( canID );
			for (i = 0; i < model->materials.numMaterials; i++)
				{
				createChunk( canID, CAN_TYPEDEF(material_CHUNK), -1, 0 );
				setChunkData( canID, &model->materials.materials[i], sizeof(material_CHUNK) );
				}
			ascendChunk( canID );

		createChunk( canID, CAN_TYPEDEF(textureList_CHUNK), -1, 0 );
		setChunkData( canID, &model->textures.numTextures, sizeof(int) );
			descendChunk( canID );
			for (i = 0; i < model->textures.numTextures; i++)
				{
				// NOTE: currently we use textures indirectly via names,
				// so just get the names straight from the list...
				// i.e. no chunk data is set for the texture
				createChunk( canID, CAN_TYPEDEF(texture_CHUNK), -1, &textureNames[i][0] );
				}
			ascendChunk( canID );
	
		writeGeometryTreeChunk( canID, &model->tree );
		ascendChunk( canID );
	}

void VRML2CAN_writeTransformListChunk(int canID, transformList_CHUNK *xforms)
{
	int numXforms = xforms->numTransforms;
	createChunk( canID, CAN_TYPEDEF(transformList_CHUNK), -1, 0 );
	setChunkData( canID, &numXforms, sizeof(int) );
	setChunkData( canID, xforms->nodeIDs, sizeof(int) * numXforms);
	setChunkDataPlatformAware( canID, xforms->transforms, sizeof(struct matrix_DATA) * numXforms, CONV_MATRIX);
}


// HACKS FOR GEOMETRY LISTING OUTPUT -cc

#define GEOLIST 
int treeLevel;
ofstream* gout;

void initGeoListingHack(char *fname)
	{
	char listname[256];
	assert(fname);
	strcpy( listname, fname);
	treeLevel = -1;
	char *ext = strstr(listname,".");
	if (ext)
		*ext = '\0';
	strcat(listname,".geo");
	gout = new ofstream(listname);
	}

void closeGeoListingHack()
	{
	delete gout;
	}


BOOL initGeometryTree( frame *f, struct geometryTree_CHUNK *tree, struct model3D_CHUNK *model )
{
#ifdef GEOLIST
	treeLevel++;
	*gout << "tree level: " << treeLevel << '\n' << '\n';
#endif

memset( tree, sizeof(struct geometryTree_CHUNK), 0);

// .......... init the children first, so the bounding box 
// .......... of the parent can be calculated
int i,nChild = f->getNumChildren();
tree->numTrees = nChild;
tree->children = new struct geometryTree_CHUNK[ nChild ];
for (i=0; i < nChild; i++)
	{

#ifdef GEOLIST
	*gout << "child: " << i << '\n';
#endif

	frame *nextChild = f->getChild( i );
	int rc = initGeometryTree( nextChild, &tree->children[ i ], model );
	assert ( rc );

#ifdef GEOLIST
	*gout << "END child: " << i << '\n' << '\n';
#endif
	}


// .......... init the matrix & billboard flag
tree->billboard = f->isBillboard();
matrix *local = f->getMatrix();
for (int r = 0; r < 4; r++)
for (int c = 0; c < 4; c++)
	tree->xform.m[r][c] = local->m[r][c];

// .......... init the meshes at this level
int nMeshes = f->getNumMeshes();
tree->numGeometry = nMeshes;
tree->geometry = new struct geometry_CHUNK[ nMeshes ];
Box bounds, next;
bounds.minx = bounds.miny = bounds.minz = FLT_MAX;
bounds.maxx = bounds.maxy = bounds.maxz = -FLT_MAX;

for ( i=0; i < nMeshes; i++ )
	{
	mesh *m0 = f->getMesh( i );
	assert (m0->getType() == 1); // must be a mesh1
	mesh1 *m = (mesh1 *) m0;

#ifdef GEOLIST
	const Tag& name = m->getName();
	*gout << "mesh#: " << i << " name: " << &name[0] << '\n';

#endif

	// calc the union of all bounding boxes at this level
	m->getBox(&next);


// .......... update the bounding box
	if (next.minx < bounds.minx) bounds.minx = next.minx;
	if (next.miny < bounds.miny) bounds.miny = next.miny;
	if (next.minz < bounds.minz) bounds.minz = next.minz;
	if (next.maxx > bounds.maxx) bounds.maxx = next.maxx;
	if (next.maxy > bounds.maxy) bounds.maxy = next.maxy;
	if (next.maxz > bounds.maxz) bounds.maxz = next.maxz;

// .......... convert the mesh1 to the geometry_CHUNK format
	WORD *indices;
	vertex *verts;
	int numIndices,numVerts;
	int rc = m->calcStrips(&indices, &numIndices, &verts, &numVerts);
	assert ( rc );

#ifdef GEOLIST
	*gout << "indices:\n";
	for (int ind=0; ind < numIndices; ind++)
		{
		*gout << indices[ ind ] << ", ";
		}
	*gout << "\n\n";
#endif

// .......... init 'vertexList_DATA' component
	tree->geometry[ i ].v.numVerts = numVerts;
	tree->geometry[ i ].v.verts = new vertex_DATA[ numVerts ];
	for (int j = 0; j < numVerts; j++)
		{
		tree->geometry[ i ].v.verts[j].x = verts[ j ].x;
		tree->geometry[ i ].v.verts[j].y = verts[ j ].y;
		tree->geometry[ i ].v.verts[j].z = verts[ j ].z;
		tree->geometry[ i ].v.verts[j].nx = verts[ j ].nx;
		tree->geometry[ i ].v.verts[j].ny = verts[ j ].ny;
		tree->geometry[ i ].v.verts[j].nz = verts[ j ].nz;
		tree->geometry[ i ].v.verts[j].u = verts[ j ].tu;
		tree->geometry[ i ].v.verts[j].v = verts[ j ].tv;

#ifdef GEOLIST

	// note: to output list for PC, don't use the "2PSX" conversion stuff 
 
	if  (convToPSX) 
	{
		SFIXED pX =  vertex2PSX( tree->geometry[ i ].v.verts[j].x, fixedPtScaleFactorPSX );
		SFIXED pY =  vertex2PSX( tree->geometry[ i ].v.verts[j].y, fixedPtScaleFactorPSX );
		SFIXED pZ =  vertex2PSX( tree->geometry[ i ].v.verts[j].z, fixedPtScaleFactorPSX );

		SFIXED pU = uv2PSX(tree->geometry[ i ].v.verts[j].u, fixedPtScaleFactorPSX );
		SFIXED pV = uv2PSX(tree->geometry[ i ].v.verts[j].v, fixedPtScaleFactorPSX );
	 
		*gout << "vert: " << j << ": " << pX << ", " << pY << ", " << pZ << ",  u,v:" << pU << "," << pV << '\n';
	}
#endif

		}

#ifdef GEOLIST
	*gout << "\n" << "strip data: " << '\n';
#endif

// .......... init 'stripIndexList_DATA' component
	tree->geometry[ i ].i.numIndices = numIndices;
	tree->geometry[ i ].i.indices = new short[ numIndices ];
	for (j = 0; j < numIndices; j++)
		tree->geometry[ i ].i.indices[ j ] = indices[ j ];


// .......... get material & texture info, and add them to their respective
// .......... lists to be stored in the containing 'model3D_CHUNK'
	MatInfo *mat;
	mat = m->getDefaultMaterialInfo();
	char *texture = m->getDefaultTextureName();

// .......... set the indices into the texture & material list
	tree->geometry[ i ].materialIndex = -1;
	tree->geometry[ i ].textureIndex = -1;
	tree->geometry[ i ].materialIndex = addMaterial( mat );
	if (texture && texture[0])
		tree->geometry[ i ].textureIndex = addTexture( texture );

// .......... how to reference geometry for animated meshes
	tree->geometry[ i ].ID = geometryID++;
	}

// .......... calc the overall bounding box from the union of the boxes
// .......... at this level and the child bounding boxes

// .......... convert from one box format to another (silly)
box_DATA bbounds;
bbounds.minx = bounds.minx; bbounds.maxx = bounds.maxx;
bbounds.miny = bounds.miny; bbounds.maxy = bounds.maxy;
bbounds.minz = bounds.minz; bbounds.maxz = bounds.maxz;

// .......... transform each child bounding box to this level
for (i=0; i < nChild; i++)
	{
	matrix *m = f->getChild( i )->getMatrix();
	struct box_DATA b = tree->children [ i ].bounds;

// .......... transforms a box's endpoints by the given matrix and
// .......... updates another 'bounds' box to enclose the 
// .......... resulting transformed points of the box 'b'
	updateBox( bbounds, b, *m );
	}

// .......... init this node's box 
tree->bounds = bbounds;

// .......... how to reference node for animations
addTransformBinding( xformID, f ); 
tree->ID = xformID++;

// .......... NOTE: we ignore the lights field for now, as xvrml.cpp 
// .......... does not currently extract lights anyway -cc
tree->numLights = 0;

#ifdef GEOLIST
	*gout << "END tree level: " << treeLevel << '\n' << '\n';
	treeLevel--;
#endif

return TRUE;
}

model3D_CHUNK *VRML2CAN_createModelChunks( frame *f )
{
model3D_CHUNK *model = new model3D_CHUNK;
memset(model, sizeof( struct model3D_CHUNK ), 0);

// .......... reset the texture & material list counter
textureIndex = 0;
materialIndex = 0;

// .......... reset the geometry & xform ID assignment counter
xformID = 0;
geometryID = 0;

// .......... reset the frame <-> ID binding counter
numXformBindings = 0;

// .......... search the geometry hierarchy and initialize
// .......... geometry, xform matrices, bounds, & billboard flags
// .......... (also compiles a list of materials & textures)
int rc = initGeometryTree( f, &model->tree, model );
assert ( rc );

// .......... init the bounding box of the entire model
matrix *world = f->getMatrix();
model->bounding.minx = model->bounding.miny = model->bounding.minz = FLT_MAX;
model->bounding.maxx = model->bounding.maxy = model->bounding.maxz = -FLT_MAX;
updateBox( model->bounding, model->tree.bounds, *world );

// .......... set up the 'materialList_CHUNK' and 'textureList_CHUNK' fields
// .......... from the lists compiled in 'initGeometryTree'
model->materials.numMaterials = materialIndex;
model->materials.materials = new material_CHUNK[ materialIndex ];

for (int i = 0; i < materialIndex; i++)
	{
	struct material_CHUNK& mat = model->materials.materials[ i ];
	mat.alpha = materials[ i ].diffuse.a;
	mat.shininess;
	mat.diffuse[0] = materials[ i ].diffuse.r;
	mat.diffuse[1] = materials[ i ].diffuse.g;
	mat.diffuse[2] = materials[ i ].diffuse.b;

	mat.ambient[0] = materials[ i ].ambient.r;
	mat.ambient[1] = materials[ i ].ambient.g;
	mat.ambient[2] = materials[ i ].ambient.b;

	mat.emmisive[0] = materials[ i ].emissive.r;
	mat.emmisive[1] = materials[ i ].emissive.g;
	mat.emmisive[2] = materials[ i ].emissive.b;

	mat.specular[0] = materials[ i ].specular.r;
	mat.specular[1] = materials[ i ].specular.g;
	mat.specular[2] = materials[ i ].specular.b;

	mat.shininess = materials[ i ].power;
	}

// .......... no texture data, just an array of names to be used in the corresponding
// .......... chunk names. This will later be replaced by datafile IDs, and the 
// .......... textures will be compressed as needed and placed into a datafile
model->textures.numTextures = textureIndex;
model->textures.textures = 0;

return model;
}


// .......... searches the node bindings to find the frame 
// .......... that is connected to the route's output event
// .......... by comparing the route's output node name to
// .......... the node bindings node's name 

frame *findFrameFromRoute( route *r )
{
int i;
for (i = 0; i < namedNodesCount; i++)
	{
	char *bindingNodeName = namedNodes[i].n->getName();
	char *routeNodeName = r->to->getName();
	if ( !stricmp(bindingNodeName, routeNodeName) )
		return (namedNodes[i].f);
	}
return 0;
}

// .......... filters the entire list of routes into those 
// .......... whose output is a frame transform
void compileFrameRouteList ()
{
int i;
frameRoutesCount=0;
for (i=0;i<routesCount;i++)
	if ( !stricmp(routes[i].to->getType()->getName(),"Transform") )
		framePRS[frameRoutesCount++] = routes[i];

for (i=0;i < frameRoutesCount; i++)
	{
	framePRS[i].f =  findFrameFromRoute( &framePRS[i].r );
	assert ( framePRS[i].f ); // must find the frame
	}
}


// .......... searches the entire list of routes for one
// .......... whose input is a time sensor...finds them all
// .......... and verifies that they are all the same for our animation extraction process
float getTimeSensorCycleTime()
{
int i;
float retVal = -1;
float checkValSame = -1;
for (i=0;i<routesCount;i++)
	if ( !stricmp(routes[i].from->getType()->getName(),"TimeSensor") )
	{
		if (retVal == -1)
		{
			retVal = *(SFTime *) routes[i].from->field("cycleInterval");
			checkValSame = retVal;
		}
		else
			retVal = *(SFTime *) routes[i].from->field("cycleInterval");

		assert (retVal == checkValSame); // don't support different cycle intervals right now
	}

assert (retVal != -1);// found no time sensors !
return retVal;
}



// .......... evaluates an intepolator at the given time, returning 
// .......... FALSE if the given time value is outside the interpolated range
BOOL evaluate(vrmlNode *interp, SFRotation& rot, float Time)
{
BOOL retval = TRUE;
MFFloat *keys = *(MFFloat **) interp->field("key");
MFRotation *rotations = *(MFRotation **) interp->field("keyValue");

// .......... for now, we simply use the time 
// .......... as an integer index into the key value array
int num = rotations->count(), numKeys = keys->count();
assert (num == numKeys); // must be the same as a VRML rule

// find the key interval that contains the time
int iLower=-1;
float upperVal,lowerVal;
while (++iLower < numKeys-1)
{
lowerVal = (*keys)[ iLower ];
upperVal = (*keys)[ iLower+1 ];
if (Time == lowerVal)
{
	rot = (*rotations)[iLower];
	return TRUE;
}
if ( lowerVal < Time && Time < upperVal )
{
// calc the linear interpolation betwen the two keys
   float interpFactorL = (Time-lowerVal) / (upperVal - lowerVal);
   SFRotation rL = (*rotations)[iLower];
   SFRotation rU = (*rotations)[iLower+1];
    
   rot.x = rL.x +  interpFactorL * (rU.x - rL.x);
   rot.y = rL.y +  interpFactorL * (rU.y - rL.y);
   rot.z = rL.z +  interpFactorL * (rU.z - rL.z);
   rot.ang = rL.ang + interpFactorL * (rU.ang - rL.ang);
	return TRUE;
}

};
// time is outside range, so return value is last key value
rot = (*rotations)[numKeys-1];
return FALSE;
}



BOOL evaluate(vrmlNode *interp, SFVec3f& v3f, float Time)
{
BOOL retval = TRUE;
MFFloat *keys = *(MFFloat **) interp->field("key");
MFVec3f *vec3fs = *(MFVec3f **) interp->field("keyValue");

// .......... for now, we simply use the time 
// .......... as an integer index into the key value array
int num = vec3fs->count(), numKeys = keys->count();
assert (num == numKeys); // must be the same as a VRML rule

// find the key interval that contains the time
int iLower=-1;
float upperVal,lowerVal;
while (++iLower < numKeys-1)
{
lowerVal = (*keys)[ iLower ];
upperVal = (*keys)[ iLower+1 ];
if (Time == lowerVal)
{
	v3f = (*vec3fs)[iLower];
	return TRUE;
}
if ( lowerVal < Time && Time < upperVal )
{
// calc the linear interpolation betwen the two keys
   float interpFactorL = (Time-lowerVal) / (upperVal - lowerVal);
   SFVec3f rL = (*vec3fs)[iLower];
   SFVec3f rU = (*vec3fs)[iLower+1];
    
   v3f.x = rL.x + interpFactorL * (rU.x - rL.x);
   v3f.y = rL.y + interpFactorL * (rU.y - rL.y);
   v3f.z = rL.z + interpFactorL * (rU.z - rL.z);
	return TRUE;
}

};
// time is outside range, so return value is last key value
v3f = (*vec3fs)[numKeys-1];
return FALSE;
}

// .......... ANIMATION CONVERSION STUFF...

// .......... Sets the frame's field value associated with the route at time 't'... 
// .......... we currently don't do any interpolation; each key value is assumed 
// .......... to correspond to one animation "tick". 
BOOL setValue(frameRoute *fr, float Time, BOOL& lastTime)
{
SFRotation rot;
SFVec3f v3f;

// .......... get the route & corresponding transform frame
route *r = &fr->r;
frame *f = fr->f;
assert (f); // frame not there;

// calculate the current value of the interpolator 
if ( !stricmp(r->from->getType()->getName(),"OrientationInterpolator") )
lastTime =	!evaluate(r->from, rot, Time);
else if ( !stricmp(r->from->getType()->getName(),"PositionInterpolator") )
lastTime =	!evaluate(r->from, v3f, Time);
else
	assert(0); // unsupported interpolator

// .......... get old matrix for comparison
matrix mOld = *(f->getMatrix());

// .......... set the frame PRS data accordingly
// .......... NOTE: have to watch out for coordinate system here, there may be a bug
if ( !stricmp(r->fieldTo,"set_translation") )
{
	if (convToPSX)
		f->setPosition(v3f.x,v3f.y, v3f.z);
	else
		f->setPosition(v3f.x,v3f.y, -v3f.z); // note the reverse interpretation of the translation for RHS->LHS

}
else if ( !stricmp(r->fieldTo,"set_scale") )
{
	f->setScale(v3f.x,v3f.y, v3f.z);
}
else if ( !stricmp(r->fieldTo,"set_center") )
{
	if (convToPSX)
		f->setOffset(v3f.x,v3f.y, v3f.z);
	else
		f->setOffset(v3f.x,v3f.y, -v3f.z); // note the reverse interpretation of the translation for RHS->LHS
}
else if ( !stricmp(r->fieldTo,"set_rotation") )
{
	vector v(rot.x, rot.y, rot.z);
	if (convToPSX)
		f->setOrientation( &v, rot.ang ); // PSX hack
	else
	{
		v.z = -v.z;
		f->setOrientation( &v, rot.ang ); // note the reverse interpretation of the rotation angle for RHS->LHS
		//f->setOrientation( &v, -rot.ang ); // note the reverse interpretation of the rotation angle for RHS->LHS
	}
}	 

else if ( !stricmp(r->fieldTo,"set_scaleOrientation") )
{
	vector v(rot.x, rot.y, rot.z);
	if (convToPSX)
		f->setScaleOrientation( &v, rot.ang );// PSX hack
	else
	{
		v.z = -v.z;
		f->setScaleOrientation( &v, rot.ang );// note the reverse interpretation of the rotation angle for LHS->RHS
		//f->setScaleOrientation( &v, -rot.ang );// note the reverse interpretation of the rotation angle for LHS->RHS
	}
}
else
	assert(0); // not a valid transform 'eventIn' field name

// .......... get new matrix for comparison
matrix *mnew = f->getMatrix();
return ( memcmp(&mOld, mnew, sizeof(matrix)) )? TRUE : FALSE; // TRUE means matrix has changed
}

void updateFrameInAnimChunk( frame *f, struct animFrame3D_CHUNK *current )
{
int i=-1;
int found = -1;
while (++i < current->xforms.numTransforms)
	{
	int ID = current->xforms.nodeIDs[i];

	int j=-1;
	while(++j < numXformBindings)
			if ( ID == IDtoFrame[ j ].ID )
				break;

	assert (j != numXformBindings); // must have found it

	if (IDtoFrame[ j ].f == f)
		break;
	}
	assert (i != current->xforms.numTransforms); // must have found it

matrix *m = f->getMatrix();
matrix_DATA& md = current->xforms.transforms[i];
for (int r = 0; r < 4; r++)
for (int c = 0; c < 4; c++)
	md.m[r][c] = m->m[r][c];
}


void addFrames( struct animFrame3D_CHUNK *current )
{
assert (numAnimFrames < MAX_ANIM_FRAMES); // too many frames
memcpy( &anims[numAnimFrames], current, sizeof(struct animFrame3D_CHUNK) );

struct animFrame3D_CHUNK &nc = anims[numAnimFrames++];

// .......... need to allocate new array of xforms to record 'current' ones
nc.xforms.nodeIDs = new int[ nc.xforms.numTransforms ];
nc.xforms.transforms = new struct matrix_DATA[ nc.xforms.numTransforms ];

// .......... copy the arrays over
int i=-1;
while (++i < nc.xforms.numTransforms)
	{
	nc.xforms.nodeIDs[ i ] = current->xforms.nodeIDs[ i ];
	nc.xforms.transforms[ i ] = current->xforms.transforms[ i ];
	}

// .......... update overall timing
overallTiming.Duration += nc.timing.Duration;
} 	

BOOL writeFrameList( int canID )
{
return TRUE;
}


void createXformNodeIDArray(animFrame3D_CHUNK *current) {
	int numTransforms = 0;
	for (int i = 0; i < numXformBindings; i++) {
		// find a frame route that has same target as this xform binding
		for (int j = 0; j < frameRoutesCount; j++) {
			if (framePRS[j].f == IDtoFrame[i].f) {
				IDtoFrame[i].hasRoute = TRUE;
				numTransforms++;
				break;
			}
		}
	}

	// allocate array
	current->xforms.numTransforms = numTransforms;
	current->xforms.nodeIDs = new int[ numTransforms ];
	current->xforms.transforms = new struct matrix_DATA[ numTransforms ];

	// init the xform node IDs and matrices
	int IDIndex = 0;
	for (i = 0; i < numTransforms; i++) {
		// get the next index with a route
		while ( !IDtoFrame[IDIndex].hasRoute )
			IDIndex++;
		// stick it in the node ID array
		current->xforms.nodeIDs[i] = IDtoFrame[IDIndex].ID;
		matrix_DATA& md = current->xforms.transforms[i];
		matrix *m = IDtoFrame[IDIndex].f->getMatrix();
		for (int r = 0; r < 4; r++)
			for (int c = 0; c < 4; c++)
				md.m[r][c] = m->m[r][c];
		IDIndex++;
	}
}

BOOL VRML2CAN_createAnimationFrameChunks() {
	// Global overallTiming holds the overall timing info for the anim
	memset(&overallTiming, 0, sizeof(struct frameTiming_DATA));
	overallTiming.TimeMultiple = 4;
	overallTiming.LastUse = 1;

	// every time unit has a current 'animFrame3D_CHUNK' to add to
	struct animFrame3D_CHUNK current;
	// initialize the first frame
	current.timing.StartTime = 0;
	current.timing.Duration = 1;
	current.timing.SubPriority = 0;
	current.timing.LastUse = 0;
	current.timing.EndingAction = 0;
	current.timing.TimeMultiple = 1;
	current.timing.DropFrames = 0;
	// initialize its general position matrix
	matrix identity = IdentityMatrix();
	for (int r = 0; r < 4; r++)		// "different" matrix formats
		for (int c = 0; c < 4; c++)
			current.xform.m[r][c] = identity.m[r][c];

	// model3D_CHUNK ...nothing to init, model is referenced
	// textureList_CHUNK ...ignoring texture animations, there are none
	// materialList_CHUNK ...ignoring texture animations, there are none

	// init the transformList_CHUNK...
	// Uses the global list of routes made from xvrml, filters the
	// ones which send to a transform node, and stores them in the
	// framePRS global var.
	compileFrameRouteList ();
	if (frameRoutesCount == 0)
		return FALSE;		// nothing to animate

	// Figure out the duration of the overall animation
	float animCycleTime = getTimeSensorCycleTime();
	int Time = 0; // simulated animation time, in abstract ticks, zero-based
	int maxTime = (int) (animCycleTime * 30.0);
				// #frames in animation = total time * frames per second.
				// by convention, if the anim lasts for 1/30th of a second,
				// that means two frames, not one frame. If it lasts for less
				// than 1/30th, that means one frame only.									

	// calc the # of transforms -- uses global xform bindings and frameroutes
	// Note:  This means that every frame of animation is going to use
	// the exact same table of node ID's, only with different matrices
	// at each frame.  (As opposed to varying table size per animframe.)
	createXformNodeIDArray(&current);


	//---------------------
	// THE SIMULATION
	//---------------------
	// Initialize the global number of frames ... for addFrames()
	numAnimFrames = 0;
	// Currently using the fixed Time increment thing
	for (Time = 0; Time < maxTime; Time++) {
		BOOL lastTime;	// flag used by setValue (currently for nothing)
		float simTime;

		// Calculate current time
		if (animCycleTime * 30)
			simTime = ((float)Time) / (animCycleTime * 30); // entire anim cycle time / frames per second
		else
			simTime = 0;
		current.timing.StartTime = Time;

		// Update all the dependent values for the current Time
		for (int i = 0; i < frameRoutesCount; i++)
			setValue(framePRS + i, simTime, lastTime);

		// Update the matrix for each geometry frame, in the current animation frame
		for (i = 0; i < frameRoutesCount; i++)
			updateFrameInAnimChunk( framePRS[i].f, &current );

		if (Time == maxTime)
			current.timing.LastUse = 1;
		addFrames(&current);
	}	// end Time loop
	return TRUE;
}


//============================================================================
//  vrml2Can
//============================================================================

// .......... converts a VRML file with scene and / or animation information
// .......... into a binary .CAN file optimized for loading & playing
BOOL vrml2Can(char *filename, char *animFileName, char *outputDir, int options)
{
VRML2CAN_setConvertToPSXFlag(options == GENERATE_PSX);

vrmlNode *vrmlRoot;
frame *root = VRMLextractScene(filename, 0, &vrmlRoot);
if (!root)
	return FALSE;

// .......... We now have a scene graph, a corresponding static frame->mesh1 scene,
// .......... and possibly some animation interpolators. We need to combine the PRS
// .......... interpolators into a single matrix ( done via animating the frame on
// .......... each simulated "tick" ). 


//--------------------------------------------
//  Output the model
//--------------------------------------------

#ifdef GEOLIST 
initGeoListingHack(filename);
#endif

// Create the model chunks, ready for output
model3D_CHUNK *model = VRML2CAN_createModelChunks(root);

#ifdef GEOLIST 
closeGeoListingHack();
#endif

// Output it to a file........
// .......... create the can object (.C3D)
char modelName[128];
char canName[128]="",*ext;

if (outputDir && outputDir[0]) // there's a non=null path
{
		strcpy(canName, outputDir);
		if (canName[strlen(canName)-1] != '\\')
				strcat(canName, "\\"); // make sure the path has a '\'
}

strcat(canName, filename);
ext = strchr(canName, '.'); // make sure the extension is .C3D
if (ext)
	{
	*ext = '\0';
	strcpy(modelName, canName);
	strcpy(ext,".C3D");
	}
else
	{
	strcpy(modelName, canName);
	strcat(canName,".C3D");
	}

////// SIMPLE CAN TEST CODE ////////////////
// int canID2 = createCAN(canName);
// 
// struct test1_CHUNK test1; test1.testInt1 = 255+(256*256*255);
// createChunk( canID2, CAN_TYPEDEF(test1_CHUNK), -1, "test" );
// setChunkData( canID2, &test1, sizeof(struct test1_CHUNK) );
// 
// writeCAN( canID2 );
///////END TEST CODE ///////////////

int canID = createCAN(canName);
if (canID < 0)
	{
	fprintf(stdout,"error on .C3D create\n");
	return FALSE;
	}

// .......... save the geometry tree in the .CAN file
VRML2CAN_writeModelChunks( canID, model, modelName);

// .......... save & destroy the can object
ERR ( writeCAN( canID ) );


//--------------------------------------------
//  Now output the animation frames
//--------------------------------------------

// Initialize all the data into the global array anims[]
// number of elements is global numAnimFrames
// also initializes global overallTiming
if (!VRML2CAN_createAnimationFrameChunks()) {
	fprintf(stdout, "Can't create any animation frames");
	return FALSE;
}

ext = strchr(canName, '.'); // make sure the extension is .CAN
strcpy(ext,".CAN");
canID = createCAN( canName );
if (canID < 0)
	{
	fprintf(stdout,"error on .CAN create\n");
	return FALSE;
	}

// .......... write the animheader chunk
createChunk( canID, CAN_TYPEDEF(anim3D_CHUNK), -1, animFileName );
setChunkData( canID, &overallTiming, sizeof(struct frameTiming_DATA) );

// .......... and write the frame chunks
	descendChunk( canID );
	for (int i = 0; i < numAnimFrames; i++)
		{
		createChunk( canID, CAN_TYPEDEF(animFrame3D_CHUNK), -1, 0 );
		setChunkData( canID, &anims[ i ].timing, sizeof(struct frameTiming_DATA) );
		setChunkDataPlatformAware( canID, &anims[ i ].xform, sizeof(struct matrix_DATA), CONV_MATRIX );

			descendChunk( canID );

		// just use the model name (by reference, no chunk data)
			createChunk( canID, CAN_TYPEDEF(model3D_CHUNK), -1, modelName );

		// write the PRS xforms
			VRML2CAN_writeTransformListChunk(canID, &anims[i].xforms);
			ascendChunk( canID );

		// no texture or material chunks to write, so next frame
		}
	ascendChunk( canID );

// .......... save & destroy the can object
ERR ( writeCAN( canID ) );

return TRUE;
}
