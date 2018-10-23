#ifndef XVRML_H
#define XVRML_H
//
// utility module to load VRML into a renderable scene graph
// and perform file conversions to .can format, etc.

#include "vrml.h"


// .......... really cheap implementation of references to named nodes 
// .......... and field -> field routings. Basically, these arrays
// .......... are filled in while 'extractScene' is processing the VRML file
const MAX_NAMED_NODES = 1024;
const MAX_ROUTES = MAX_NAMED_NODES * 2;

struct nodeBinding {

	vrmlNode *n;
	frame *f;
	mesh *m;

	nodeBinding() : n(0),f(0),m(0) {}
};

extern int namedNodesCount;
extern nodeBinding namedNodes[MAX_NAMED_NODES];
extern vrmlNode *shapeNodes[MAX_NAMED_NODES];

extern int routesCount;
extern route routes[MAX_ROUTES];


// .......... constructs a renderable scene from a VRML file.
// .......... if 'vrmlGraph' is non-NULL, the VRML scene graph will be
// .......... returned, otherwise it will be deleted when the renderable
// .......... scene is created. If not deleted, it is the caller's
// .......... responsibility to do so.
frame *VRMLextractScene(char *filename, char *filter, vrmlNode **vrmlGraph=0);

frame *VRMLextractScene(vrmlNode *root, char *filter);



#endif
