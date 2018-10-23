#ifndef CANTYPE_H
#define CANTYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef REND3D_PSX

// map macros to PC structures

// data                             
#define		matrix_DATA_MACRO					 	  matrix_DATA              
#define 	vector_DATA_MACRO					 	  vector_DATA              
#define 	vertex_DATA_MACRO					 	  vertex_DATA              
#define 	box_DATA_MACRO			    		 	  box_DATA                 
#define 	camera_DATA_MACRO					 	  camera_DATA              
#define 	vertexList_DATA_MACRO				 	  vertexList_DATA          
#define 	stripIndexList_DATA_MACRO   		 	  stripIndexList_DATA      
						                		 	                           
//chunks                            		
#define 	model3D_CHUNK_MACRO		    		 	  model3D_CHUNK            
#define 	texture_CHUNK_MACRO 				 	  texture_CHUNK            
#define 	textureList_CHUNK_MACRO 			 	  textureList_CHUNK        
#define 	material_CHUNK_MACRO 				 	  material_CHUNK           
#define 	materialList_CHUNK_MACRO 			 	  materialList_CHUNK       
#define 	light_CHUNK_MACRO 		    		 	  light_CHUNK              
#define 	geometry_CHUNK_MACRO 				 	  geometry_CHUNK           
#define 	transformList_CHUNK_MACRO   		 	  transformList_CHUNK      
#define 	geometryTree_CHUNK_MACRO 			 	  geometryTree_CHUNK       
#define 	animFrame3D_CHUNK_MACRO 			 	  animFrame3D_CHUNK        
#define 	anim3D_CHUNK_MACRO 		    		 	  anim3D_CHUNK             
		 	                           
#else
// map macros to PSX structures

// data 
#define		matrix_DATA_MACRO					 	  matrix_DATA_PSX          
#define 	vector_DATA_MACRO					 	  vector_DATA_PSX          
#define 	vertex_DATA_MACRO					 	  vertex_DATA_PSX          
#define 	box_DATA_MACRO			    		 	  box_DATA_PSX             
#define 	camera_DATA_MACRO					 	  camera_DATA_PSX          
#define 	vertexList_DATA_MACRO				 	  vertexList_DATA_PSX      
#define 	stripIndexList_DATA_MACRO   		 	  stripIndexList_DATA_PSX  

//chunks
#define 	model3D_CHUNK_MACRO	 			 	   		model3D_CHUNK_PSX        
#define 	texture_CHUNK_MACRO 					   	texture_CHUNK_PSX        
#define 	textureList_CHUNK_MACRO 			 	   	textureList_CHUNK_PSX    
#define 	material_CHUNK_MACRO 				 	   	material_CHUNK_PSX       
#define 	materialList_CHUNK_MACRO 			 	   	materialList_CHUNK_PSX   
#define 	light_CHUNK_MACRO 		    		 	   	light_CHUNK_PSX          
#define 	geometry_CHUNK_MACRO 				  	   	geometry_CHUNK_PSX       
#define 	transformList_CHUNK_MACRO   			   	transformList_CHUNK_PSX  
#define 	geometryTree_CHUNK_MACRO 				   	geometryTree_CHUNK_PSX   
#define 	animFrame3D_CHUNK_MACRO						animFrame3D_CHUNK_PSX
#define 	anim3D_CHUNK_MACRO 		    			   	anim3D_CHUNK_PSX
			            			   	  
#endif


#define CAN_TYPEDEF(x) CAN_##x
enum chunkType {
		CAN, // basic header chunk for each file

		CAN_TYPEDEF(model3D_CHUNK),
		CAN_TYPEDEF(texture_CHUNK), 
		CAN_TYPEDEF(textureList_CHUNK), 
		CAN_TYPEDEF(material_CHUNK), 
		CAN_TYPEDEF(materialList_CHUNK), 
		CAN_TYPEDEF(light_CHUNK), 
		CAN_TYPEDEF(geometry_CHUNK), 
		CAN_TYPEDEF(transformList_CHUNK), 
		CAN_TYPEDEF(geometryTree_CHUNK), 
		CAN_TYPEDEF(animFrame3D_CHUNK), 
		CAN_TYPEDEF(anim3D_CHUNK), 

		ADDITIONAL_CHUNK_TYPES
};


// .......... ANIM DATA CHUNK TYPES for PC3D

// .......... 	NOTE: 'type_CHUNK' indicates an actual chunk, whereas  
// .......... 	'type_DATA' refers to data within a chunk

struct vector_DATA {
	float x,y,z;
};

struct box_DATA {
	float minx,miny,minz;
	float maxx,maxy,maxz;
};

struct vertex_DATA {
	float x,y,z;
	float nx,ny,nz;
	float u,v;
};

struct matrix_DATA {
	float m[4][4];
};

struct camera_DATA {
	struct matrix_DATA prs;
	float fieldOfView;
};

struct vertexList_DATA {
	int numVerts;
	struct vertex_DATA *verts;
};

struct stripIndexList_DATA {
	int numIndices;//should be read, numStrip    :)

	//each cell contains the starting vertex index of the strip,                   
	//the following cell value-1 contains the ending vertex end index of the strip.
	short *indices;	
					
};

struct texture_CHUNK {
	int width, height, bpp;
	int compressionType;
	void *data;
};

struct textureList_CHUNK {
	int numTextures;
	struct texture_CHUNK *textures;
};

struct material_CHUNK {
	float alpha;
	float shininess;
	float diffuse[3];
	float ambient[3];
	float emmisive[3];
	float specular[3];
};

struct materialList_CHUNK {
	int numMaterials;
	struct material_CHUNK *materials;
};

struct light_CHUNK {
	int type;
	float ambientIntensity;
	float intensity;
	struct vector_DATA direction;
	float color[3];           
};

struct geometry_CHUNK {
	int ID;		// internal code to reference geometry
	int materialIndex;
	int textureIndex;	
	struct vertexList_DATA v;
	struct stripIndexList_DATA i;
};

struct transformList_CHUNK {
	int numTransforms;
	int *nodeIDs;
	struct matrix_DATA *transforms;
};

struct geometryTree_CHUNK {

	int ID;		// internal code to reference xform nodes/frames
	int billboard;
	struct box_DATA bounds;
	struct matrix_DATA xform;
	int numLights;
	int numGeometry;
	int numTrees;

	struct light_CHUNK *lights;
	struct geometry_CHUNK *geometry;
	struct geometryTree_CHUNK *children;
};

struct model3D_CHUNK {
	struct box_DATA bounding; // 3D bounding box for this model (world coordinates)
	struct materialList_CHUNK materials;
	struct textureList_CHUNK textures;
	struct geometryTree_CHUNK tree;
	};


struct frameTiming_DATA {
	int StartTime:24;
	int Duration:24;
	int SubPriority:8;
	int LastUse:1;
	int EndingAction:2;
	int TimeMultiple:4;
	int DropFrames:1;
};

struct animFrame3D_CHUNK {

	struct frameTiming_DATA timing;
	struct matrix_DATA xform; // global position / rotate / scale 

	struct model3D_CHUNK model;
	struct transformList_CHUNK xforms;
	struct textureList_CHUNK textures;
	struct materialList_CHUNK materials;
};


// a grouping header for a sorted list of 3D frames
// (set up in a fashion similar to the 1998 sequence
// format to ease porting to the '98 sequencer)
struct anim3D_CHUNK {

	struct frameTiming_DATA timing;
	
	// subsequent anim frames, sorted by start time
	struct animFrame3D_CHUNK *frameAnims;
};


// .......... ANIM DATA CHUNK TYPES for PSX

typedef short	SFIXED;
typedef long	FIXEDPSX;

struct matrix_DATA_PSX {
	SFIXED	m[3][3];
	FIXEDPSX	t[3];
	};

struct vector_DATA_PSX {
	SFIXED x,y,z;
};

struct vertex_DATA_PSX {
	SFIXED x,y,z;
	SFIXED nx,ny,nz;
	SFIXED u,v;
};

struct box_DATA_PSX {
	SFIXED minx,miny,minz;
	SFIXED maxx,maxy,maxz;
};

struct camera_DATA_PSX {
	struct matrix_DATA_PSX prs;
	float fieldOfView;
};

struct vertexList_DATA_PSX {
	int numVerts;
	struct vertex_DATA_PSX *verts;
};

struct stripIndexList_DATA_PSX {
	int numIndices;//should be read, numStrip    :)

	//each cell contains the starting vertex index of the strip,                   
	//the following cell value-1 contains the ending vertex end index of the strip.
	short *indices;	
					
};

struct texture_CHUNK_PSX {
	int width, height, bpp;
	int compressionType;
	void *data;
};

struct textureList_CHUNK_PSX {
	int numTextures;
	struct texture_CHUNK_PSX *textures;
};

struct material_CHUNK_PSX {
	float alpha;
	float shininess;
	float diffuse[3];
	float ambient[3];
	float emmisive[3];
	float specular[3];
};

struct materialList_CHUNK_PSX {
	int numMaterials;
	struct material_CHUNK_PSX *materials;
};

struct light_CHUNK_PSX {
	int type;
	float ambientIntensity;
	float intensity;
	struct vector_DATA_PSX direction;
	float color[3];           
};

struct geometry_CHUNK_PSX {
	int ID;		// internal code to reference geometry
	int materialIndex;
	int textureIndex;	
	struct vertexList_DATA_PSX v;
	struct stripIndexList_DATA_PSX i;
};

struct transformList_CHUNK_PSX {
	int numTransforms;
	int *nodeIDs;
	struct matrix_DATA_PSX *transforms;
};

struct geometryTree_CHUNK_PSX {

	int ID;		// internal code to reference xform nodes/frames
	int billboard;
	struct box_DATA_PSX bounds;
	struct matrix_DATA_PSX xform;
	int numLights;
	int numGeometry;
	int numTrees;

	struct light_CHUNK_PSX *lights;
	struct geometry_CHUNK_PSX *geometry;
	struct geometryTree_CHUNK_PSX *children;
};

struct model3D_CHUNK_PSX {
	struct box_DATA_PSX bounding; // 3D bounding box for this model (world coordinates)
	struct materialList_CHUNK_PSX materials;
	struct textureList_CHUNK_PSX textures;
	struct geometryTree_CHUNK_PSX tree;
	};


struct frameTiming_DATA_PSX {
	int StartTime:24;
	int Duration:24;
	int SubPriority:8;
	int LastUse:1;
	int EndingAction:2;
	int TimeMultiple:4;
	int DropFrames:1;
};

struct animFrame3D_CHUNK_PSX {

	struct frameTiming_DATA_PSX timing;
	struct matrix_DATA_PSX xform; // global position / rotate / scale 

	struct model3D_CHUNK_PSX model;
	struct transformList_CHUNK_PSX xforms;
	struct textureList_CHUNK_PSX textures;
	struct materialList_CHUNK_PSX materials;
};


// a grouping header for a sorted list of 3D frames
// (set up in a fashion similar to the 1998 sequence
// format to ease porting to the '98 sequencer)
struct anim3D_CHUNK_PSX {

	struct frameTiming_DATA_PSX timing;
	
	// subsequent anim frames, sorted by start time
	struct animFrame3D_CHUNK_PSX *frameAnims;
};

#ifdef __cplusplus
	}
#endif

#endif
