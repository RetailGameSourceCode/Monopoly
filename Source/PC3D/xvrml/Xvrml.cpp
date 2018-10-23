// //////////////////////////////////////////////////
// Copyright (C) 1997, Artech,        
// All rights reserved.          
//                 
// This document contains intellectual property  
// of Artech.  Any copying or reproduction    
// without prior written permission is prohibited. 
//                 
// //////////////////////////////////////////////////
// MODULE: xvrml.cpp              
// Contents: loads VRML meshes,materials,etc and converts them to our internal mesh1 format
//
// 	Deals with references in VRML to other geometry and fixes up cases
//	with multiple texture coordinates for a single vertex 
//	(Direct 3D forces a vertex to have a unique texture coordinate.
// ///////////////// END OF HEADER //////////////////
#include "pc3d.h"
#include "d3dutils.h"
#include "vrml.h"
#include "xvrml.h"

#define CONVERT_LHS_MATRIX 0
BOOL convToPSX=0;

// .......... When extracting hierarchy info from VRML we support Group,Transform & Billboard
// .......... Nodes. We also search for geometry inside other grouping nodes like Collision and Anchor. 
// .......... A 'Group' node is treated like a 'Transform' node with an identity matrix.
// .......... Any grouped node corresponds to one level of the hierarchy. The only supported
// .......... Billboard axis of rotation is (0,1,0).
// .......... For Geometry, the only primitive currently extracted is the 'IndexedFaceSet'.


int namedNodesCount = 0;
int routesCount = 0;

nodeBinding namedNodes[MAX_NAMED_NODES];
vrmlNode *shapeNodes[MAX_NAMED_NODES];
route routes[MAX_ROUTES];
				      
inline BOOL isType(vrmlNode *n, const char *name)
	{return !stricmp(n->getType()->getName(), name);}


void addBinding(vrmlNode *n, mesh1 *m)
	{
	int i;
	for (i = 0 ; i < namedNodesCount; i++)
	     if (namedNodes[i].n == n)
		namedNodes[i].m = m;
	}

void addBinding(vrmlNode *n, frame *f)
	{
	int i;
	for (i = 0 ; i < namedNodesCount; i++)
	     if (namedNodes[i].n == n)
		namedNodes[i].f = f;
	}

void recordRoute(route *r)
	{
	assert (routesCount < MAX_ROUTES);
	routes[ routesCount++ ] = *r;
	}


void *findDEFNodes(vrmlNode *n, void *info)
	{
	if (n->getName())
		{
		int i = -1;
		while (++i < namedNodesCount)
			{
			// if it's already there, dont add
			if (!stricmp(n->getName(),namedNodes[i].n->getName()))
				break;
			};

		// it's not there, add it
		if (i == namedNodesCount)
			{
			assert (namedNodesCount < MAX_NAMED_NODES); // too many named nodes
			namedNodes[ namedNodesCount++ ].n = n;
			}
		}
	return findDEFNodes;
	}

void *findShapeNodes(vrmlNode *n, void *info)
	{
	if (n->getName())
		{
		int i = -1;
		while (++i < namedNodesCount)
			{
			// if it's already there, dont add
			if (!stricmp(n->getName(),namedNodes[i].n->getName()))
				break;
			};

		// it's not there, add it
		if (i == namedNodesCount)
			{
			assert (namedNodesCount < MAX_NAMED_NODES); // too many named nodes
			namedNodes[ namedNodesCount++ ].n = n;
			}
		}
	return findShapeNodes;
	}

inline D3DCOLORVALUE d3dcolor(SFColor c, float alpha=1.0)
	{
	D3DCOLORVALUE d;
	d.r = c.r;
	d.g = c.g;
	d.b = c.b;
	d.a = alpha;
	return d;
	};

inline D3DCOLORVALUE d3dcolor(SFFloat f)
	{
	D3DCOLORVALUE d;
	d.r = f;
	d.g = f;
	d.b = f;
	d.a = 1.0;
	return d;
	};

void initShapeAppearance(mesh1 *shape, vrmlNode *appearance)
	{
	vrmlNode *material = *(SFNode **) appearance->field("material");
	vrmlNode *texture = *(SFNode **) appearance->field("texture");
	vrmlNode *textureTransform = *(SFNode **) appearance->field("textureTransform");

	if (texture)
		{
		assert (isType(texture,"ImageTexture")); // only support this kind of texture

		// don't know why the url is a string array
		MFString *url = *(MFString **) texture->field("url");

		shape->setDefaultTextureName( Tag((*url)[0],"png") );
		// NOT DONE: need to strip the texture name & ext from the url path into
		}

	SFFloat ambientIntensity = *(SFFloat*) material->field("ambientIntensity");
	SFColor diffuseColor     = *(SFColor*) material->field("diffuseColor");
	SFColor emissiveColor    = *(SFColor*) material->field("emissiveColor");
	SFFloat shininess        = *(SFFloat*) material->field("shininess");
	SFColor specularColor    = *(SFColor*) material->field("specularColor");   
	SFFloat transparency     = *(SFFloat*) material->field("transparency");    

	MatInfo i;
	memset(&i,0,sizeof(MatInfo));
	float alpha = 1-transparency;
	
	i.diffuse = d3dcolor( diffuseColor, alpha );       
	i.ambient = d3dcolor(ambientIntensity);       
	i.specular = d3dcolor(specularColor);       
	i.emissive = d3dcolor(emissiveColor);      
	i.power = shininess;         
	i.hTexture=0;  	 
	i.dwRampSize=16;
	
	shape->setDefaultMaterialInfo( &i );
	}
	 
inline vector xform(D3DMATRIX& m, vector& p)
	{
	vector x;
	x.x = m._11 * p.x + m._21 * p.y + m._31 * p.z + m._41;
	x.y = m._12 * p.x + m._22 * p.y + m._32 * p.z + m._42;
	x.z = m._13 * p.x + m._23 * p.y + m._33 * p.z + m._43;
	return x;
	}

mesh1 *initShape(vrmlNode *n)
	{
	assert (isType(n,"Shape"));

	vrmlNode *geometry = *(SFNode **) n->field("geometry");

	// don't load anything but Indexed Face Sets
	if (!geometry || !isType(geometry,"IndexedFaceSet"))
		return 0;

#if 0
	// warn that we don't support pre-colored face/vertex mode, and we'll ignore color
	//	(we'd have to use lit vertices for that)
	vrmlNode *color = *(SFNode **) geometry->field("color");
	if (color)
		{
		MSG(n->getName(), "no support for colored geometry mode...ignoring color");
		}
	// don't support normal per face mode 
	//	(we'd have to draw triangle by triangle (instead of strips) for that)
	SFBool normalPerVertex = *(SFBool *) geometry->field("normalPerVertex");
	if (!normalPerVertex)
		{
		MSG("no support for normals per face mode, ignoring object", "shape error");
		return 0;
		}
#endif

	// all faces in the shape must be convex (and they must be triangles or quads, check that later...)
	SFBool convex = *(SFBool *) geometry->field("convex");
	if (!convex)
		{
		MSG(n->getName(), "no support for non-convex faces, ignoring object");
		return 0;
		}

// .......... general mesh1 hint flags
	SFBool solid = *(SFBool *) geometry->field("solid");
	SFBool ccw = *(SFBool *) geometry->field("ccw");
	SFBool normalPerVertex = *(SFBool *) geometry->field("normalPerVertex");
	SFBool colorPerVertex = *(SFBool *) geometry->field("colorPerVertex");

// .......... actual geometry (per vertex) data
	vrmlNode *coords = *(SFNode **) geometry->field("coord");
	if (!coords)
		{
		MSG(n->getName(), "shape has no points");
		return 0;
		}
	MFVec3f *points = *(MFVec3f **) coords->field("point");
	if (!points)
		{
		MSG(n->getName(), "shape has no points");
		return 0;
		}

// .......... get normal list
	vrmlNode *normaln = *(SFNode **) geometry->field("normal");
	MFVec3f *normals = 0;
	if(normaln)
		normals = *(MFVec3f **) normaln->field("vector");

// .......... get texture coordinate list
	vrmlNode *texcoord = *(SFNode **) geometry->field("texCoord");
	MFVec2f *texpoints = 0;
	if(texcoord)
		texpoints = *(MFVec2f **) texcoord->field("point");

// .......... get color list
	vrmlNode *color = *(SFNode **) geometry->field("color");
	MFColor *colors = 0;
	if(color)
		colors = *(MFColor **) color->field("color");

	MFInt32 *coordIndex = *(MFInt32 **)geometry->field("coordIndex"); 
	if (!coordIndex)
		{
		MSG(n->getName(), "shape has no indices");
		return 0;
		}

	MFInt32 *normalIndex = *(MFInt32 **)geometry->field("normalIndex");
	MFInt32 *texCoordIndex = *(MFInt32 **)geometry->field("texCoordIndex");
	MFInt32 *colorIndex = *(MFInt32 **)geometry->field("colorIndex");

// .......... copy the points, normals, colors, & texcoords arrays to the format required by the mesh1

// .......... points list
	#if CONVERT_LHS_MATRIX
		D3DMATRIX toLHS = IdentityMatrix();
		D3DVECTOR toLHSUP(0,0,-1);
		D3DVECTOR toLHSDIR(0,1,0);
		SetRotationMatrix(&toLHS, &toLHSDIR, &toLHSUP);
	#endif
		int i,pn=0; pn = points->count();
		vector *pList = new vector[ pn ];
		for (i=0; i < pn; i++)
			{
			#if CONVERT_LHS_MATRIX

			// do it just like in the race game ?
			pList[i].x = (*points)[i].x;
			pList[i].y = (*points)[i].y;
			pList[i].z = (*points)[i].z;

			pList[i] = xform(toLHS, pList[i]);
			pList[i].z = -pList[i].z;

			#else
			// note the point's reversal for RHS->LHS coordinates
			if (convToPSX)
				pList[i].x = (*points)[i].x; // PSX hack
			else
				pList[i].x = (*points)[i].x;
			pList[i].y = (*points)[i].y;
			pList[i].z = -(*points)[i].z;
			#endif
			}

// .......... normal list
		int nn=0;
		normal *nList = 0;
		if (normals)
			{
			nn = normals->count();
			nList = new normal[ nn ];
			for (i=0; i < nn; i++)
				{
				// note the normal reversal for RHS->LHS coordinates
				nList[i].x = (*normals)[i].x;
				nList[i].y = (*normals)[i].y;
				nList[i].z = -(*normals)[i].z;
				}
			}

// ..........  before copying the uv coordinates, get the repeat flags from the appearance node
// ..........  that indicate how the texture coordinates are to be interpreted. We alter the 
// ..........  texture coordinates here (a) if the texture coordinates are clamped to [0..1], or (b)
// ..........  if the mode is non-clamped to flip the u-value to d3d's interpretation (top-down).
// ..........	NOTE: this is kind of a "cheat" in that we lose the original texture coordinate
// ..........  values. To support VRML animation of this stuff we'll have to modify this conversion -cc
	vrmlNode *appearanceForUV = *(SFNode **) n->field("appearance");
	vrmlNode *texture = *(SFNode **) appearanceForUV->field("texture");

	BOOL repeatS = TRUE, repeatT = TRUE;
	if (texture)
		{
		// don't know why the url is a string array
		SFBool rS = *(SFBool *) texture->field("repeatS");
		SFBool rT = *(SFBool *) texture->field("repeatT");
		repeatS = rS;
		repeatT = rT;
		}

// ..........  texture coordinate list
		int tn=0;
		texCoord *tList = 0;
		if (texpoints)
			{
			tn = texpoints->count();
			tList = new texCoord[ tn ];
			for (i=0; i < tn; i++)
				{
				tList[i].x = (*texpoints)[i].x;
				tList[i].y = (*texpoints)[i].y;
				if (repeatT)
						// note the v - coordinate reversal...d3d interprets v from the top 
						// of the texture down, while VRML interprets from the bottom up
						// this only applies to texture wrap mode, in clamped mode the 
						// texture coordinates are clipped to the [0..1] range
						tList[i].y = -tList[i].y;
				else
					{ // clamp mode
					 if (tList[i].y < 0)
						tList[i].y = 0;
					 else if (tList[i].y > 1)
						tList[i].y = 1;
					}

				if (repeatS)
					{} // do nothing, VRML and D3D interpret '' the same (left to right)
				else
					{ // clamp mode
					 if (tList[i].x < 0)
						tList[i].x = 0;
					 else if (tList[i].x > 1)
						tList[i].x = 1;
					}

				}
			}

// ..........  color list
		int cn=0;
		rgbcolor *cList = 0;
		if (colors)
			{
			cn = colors->count();
			cList = new rgbcolor[ cn ];
			for (i=0; i < cn; i++)
				{
				float r = (*colors)[i].r;
				float g = (*colors)[i].g;
				float b = (*colors)[i].b;
				cList[i] = D3DRGB(r,g,b);
				}
			}


// .......... add the vertices and faces to a newly created mesh1
	mesh1 *shape = new mesh1( Tag(geometry->getName()) );
	shape->beginEdit();

// .......... we can use the integer index arrays "as is" to initialize the mesh1
	assert (sizeof(int) == sizeof(SFInt32)); //just making sure

	if (coordIndex)
		shape->setFaceIndex(&(*coordIndex)[0], coordIndex->count());
	if (colorIndex)
		shape->setColorIndex(&(*colorIndex)[0],	colorIndex->count());
	if (normalIndex)
		shape->setNormalIndex(&(*normalIndex)[0], normalIndex->count());
	if (texCoordIndex)
		shape->setTextureCoordIndex(&(*texCoordIndex)[0],texCoordIndex->count());

// .......... set the mesh1's vertex,normal,color,&texture coordinate data 
	shape->setVertices(pList, pn);
	shape->setNormals(nList, nn);
	shape->setColors(cList, cn);
	shape->setTextureCoordinates(tList, tn);

// .......... setup the other flags
	shape->setCCWFlag( !ccw ); // NOTE: we reverse the interpretation of the vertex order because of RHS->LHS conversion
	shape->setSolidFlag( solid );
	shape->setColorPerVertex(colorPerVertex);
	shape->setNormalPerVertex(normalPerVertex);

	shape->endEdit();

// .......... get rid of the temp init arrays
	if (pList) delete pList;
	if (nList) delete nList;
	if (cList) delete cList;
	if (tList) delete tList;

// .......... setup the material properties of the shape
	vrmlNode *appearance = *(SFNode **) n->field("appearance");
	initShapeAppearance(shape, appearance);

	return shape;
	}


// .......... initialize a transform matrix
void initXformMatrix( vrmlNode *n, frame *f )
	{
	
	SFVec3f c = 		*(SFVec3f*) n->field("center");
	SFVec3f t = 		*(SFVec3f*) n->field("translation");     
	SFVec3f s = 		*(SFVec3f*) n->field("scale");           
	SFRotation r = 		*(SFRotation*) n->field("rotation");        
	SFRotation sc = 	*(SFRotation*) n->field("scaleOrientation");
	vector rv(r.x,r.y,r.z);

  	if (convToPSX)
    {
        // PSX hack...don;t know what it does, but works magically with PSX loader ?
        f->setPosition( t.x, t.y, t.z );
		f->setOrientation( &rv, r.ang ); 
        f->setOffset( c.x, c.y, c.z );
	    f->setScale( s.x, s.y, s.z );
		f->setScaleOrientation( sc.x, sc.y, sc.z, sc.ang );
    }
	else // D3D RHS->LHS conversion
    {
        // note the reverse interpretation of the z-coordinate for LHS->RHS
        f->setPosition( t.x, t.y, -t.z );

    	// REVISE: note the reverse interpretation of the rotation angle for LHS->RHS
		rv.z = -rv.z;
		f->setOrientation( &rv, r.ang );

    	// note the reverse interpretation of the rotation offset for LHS->RHS
    	f->setOffset( c.x, c.y, -c.z );

    	// NO reverse interpretation applies to scaling for LHS->RHS
    	f->setScale( s.x, s.y, s.z );

	    // note the reverse interpretation of the scale rotation z-coordinate for LHS->RHS
		f->setScaleOrientation( sc.x, sc.y, -sc.z, sc.ang );
    }
}


void constructFrame(vrmlNode *n, frame *parent, char *filter)
	{
	// look for a shape node 
	// and add to the frame's nodes

	if (isType(n,"Shape"))
		{
		mesh1 *shape = initShape( n );
		if (shape)
			parent->addMesh ( shape );
		
		// record the relationship between the VRML node and the actual object
		addBinding(n,shape);
		}

// .......... check for a hierarchy node or other grouping node
	if (isType(n,"Group") ||
	  isType(n,"Transform") ||
	  isType(n,"Billboard") ||
	  isType(n,"Collision") ||
	  isType(n,"Anchor") )
		{
		if (	isType(n,"Group") ||
		    	isType(n,"Transform") ||
		    	isType(n,"Billboard") )
			{
			// init the transform data according to the grouping type
			// Note that types 'Group' and 'Billboard' get the default 
			// identity transform
			parent = new frame(parent,0);

			if (isType(n,"Billboard"))
				// NOTE: we don't currently use the axis of rotation field, just assume the default (0,1,0) 
				parent->setBillboardMode(TRUE);

			if (isType(n,"Transform"))
				// initialize the transform matrix
				initXformMatrix( n, parent );

			// record the relationship between the VRML node and the actual object
			addBinding(n,parent);
			}

		int i;
		MFNode *children = *(MFNode **) n->field("children");

		if (children && children->count())
			{
			for (i = 0; i < children->count(); i++)
				{
				// construct recursively
				vrmlNode *next = (*children)[i];
				constructFrame( next, parent,filter );
				}
			}

		} /* end if isType */
	}


frame *VRMLextractScene(vrmlNode *root, char *filter)
	{
// .......... create a new frame for the root of the scene 
// .......... (the VRML scene graph may have no single root of its own.)

	frame *scene = new frame();
	constructFrame( root, scene, filter );

// .......... if it turns out that there was a single root in the VRML scene
// .......... itself, i.e. no meshes at the root and all transforms under
// .......... one top-level parent (a common case), we can get rid of the 
// .......... one we created (which was an identity transform).
	if ( scene->getNumChildren() == 1 && scene->getNumMeshes() == 0 )
		{
		frame *newRoot = scene->getChild(0);
		scene->detachChild( newRoot );
		delete scene;
		scene = newRoot;		
		}
	 		 
	return scene;
	}


frame *VRMLextractScene(char *filename, char *filter, vrmlNode **vrmlGraph)
	{
	routesCount = 0;
	vrmlNode *root = vrmlNode::loadScene(filename, recordRoute);
	if (!root) {
		MSG(filename, "error loading VRML file");
		return 0;
		}

// .......... 	find all the named and referenced nodes (i.e. the ones we export and the ones referenced by other nodes)
	namedNodesCount = 0;	
	root->traverse ( findDEFNodes, 0 );


// .......... 	fill in the node pointers for the 'routes' table
// .......... 	this should really be done via the vrml route callback mechanism
// .......... 	but it's currently not implemented (node fields not filled in by callback)
	int inode,iroute;
	for (iroute = 0; iroute < routesCount; iroute++)
		{
		route& r = routes[ iroute ];

		// find the node with the given name, if it exists
		for (inode = 0; inode < namedNodesCount; inode++ )
			{
		   	vrmlNode &n = *namedNodes[ inode ].n;
			if ( !stricmp(r.nodeFrom,n.getName()) )
				r.from = &n;
			if ( !stricmp(r.nodeTo,n.getName()) )
				r.to = &n;
			}
		}

	// .......... 	create the 3D scene
	frame *f = VRMLextractScene(root, filter);

	if (vrmlGraph)
		*vrmlGraph = root;
	else
		vrmlNode::release(root);
	return f;
	}

// .......... generates a mesh1 from a VRML file.
// .......... The file must contain a geometry node with the name given.
mesh1 *VRMLextract(char *filename, Tag &name)
	{
	return 0;	
	}





