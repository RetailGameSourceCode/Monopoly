#ifndef VRML2CAN_H
#define VRML2CAN_H

//========================================================================
//
//  The main service provided by this module is vrml2can().  However,
//  other modules can use some of its functionality to import, stripify
//  and index the joints and poses of an animated 3D model, and convert the
//  resulting information into a format ready for storage on disk.
//
//========================================================================

#include "pc3d.h"
#include "xvrml.h"
#include "cantyp.h"

// max array sizes
const MAX_MATERIALS_IN_LIST = 128;
const MAX_TEXTURES_IN_LIST = 128;
const MAX_TEXTURE_NAME_LEN = 63;
const	MAX_ANIM_FRAMES = 1024;

//--------------------------------------------------------
// Tables used to hold info about the model and routes
//--------------------------------------------------------

// textureNames, materials, are filled in by createModelChunks():
extern char         textureNames[MAX_TEXTURES_IN_LIST][MAX_TEXTURE_NAME_LEN+1];
extern int          textureIndex;
extern MatInfo      materials[MAX_MATERIALS_IN_LIST];
extern int		      materialIndex;
// These are filled in by createAnimationFrameChunks():
extern animFrame3D_CHUNK anims[MAX_ANIM_FRAMES];
extern int          numAnimFrames;
extern frameTiming_DATA overallTiming;

// .......... The geometry is extracted from the VRML tree and stored in another
// .......... tree format in a binary file after undergoing processing into triangle
// .......... strips. The format of the data in the .CAN file chunks is in cantyp.h
 
enum {
	GENERATE_PC=0x0,
	GENERATE_PSX=0x1
	};

//-------------------------------------------------------------------
// Data preparing functions. These fill in the global tables listed
// above, which can then be written to a file.
//-------------------------------------------------------------------

model3D_CHUNK *VRML2CAN_createModelChunks(frame *f);
	// ...allocates a new model3D_CHUNK structure, describing the
	// given model in that format (also stripifies the meshes).
	// Takes a PC3D frame typically obtained by VRMLExtractScene().
	// Also initializes global textureNames and materials.

BOOL VRML2CAN_createAnimationFrameChunks();
	// ...fills in anims and overallTiming.
	// You must call VRMLExtractScene() and createModelChunks() first.

void VRML2CAN_setConvertToPSXFlag(BOOL flag);
	// Sets operating mode for the following two functions:

void VRML2CAN_writeModelChunks(int canID, struct model3D_CHUNK *model, char *name);
	// Spews the chunk-format model into an open canID.

void VRML2CAN_writeTransformListChunk(int canID, transformList_CHUNK *xforms);
	// Spews the chunk-format transformlist into an open canID.

// .......... converts a VRML file with scene and / or animation information
// .......... into a binary .CAN file optimized for loading & playing
BOOL vrml2Can(char *filename, char *animFileName, char *outputDir=0, int options=GENERATE_PC);

#endif
