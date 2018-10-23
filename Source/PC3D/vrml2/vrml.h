#ifndef VRML_H
#define VRML_H

#include <fstream.h>
#include "stldefs.h"
#include "tokens.h"
#include "VrmlNodeType.h"
#include "vectlist.h"
#include "field.h"

class VrmlNodeType;


// .......... VRML node graph implementation notes
// -----------------------------------------------

// .......... 		'vrmlNode' is the basic VRML 2.0 node class. Vrml nodes exist in a graph (almost a tree) structure,
// .......... with nodes referencing their children and sometimes other nodes in the scene.
// .......... This class represents a loaded VRML scene with which you can do whatever you want.
// .......... it is primarily intended for utilities or things that don't require an efficient
// .......... internal representation. The implementation is intended to be general and to allow 
// .......... easier porting to updates of the VRML standard. This is the main reason
// .......... that specific nodes are not coded as C/C++ structures. A general node / field architecture
// .......... is used so that new node types can be created using the PROTO declarations. -cc Sept 23,1997

// .......... 		As much care as possible was taken to preserve the underlying SGI reference parser
// .......... implementation so that future updates might be smoothly incorporated.
// .......... It was necessary to modify the SGI files since all it does is syntax checking. 
// .......... Since I didn't want to get into the lex/yacc stuff, I didn't change the parser
// .......... source...just the output C/++ file. It's simple to diff the lexer.c files to see
// .......... where to change the lexer.l source if desired.

// .......... There is an unresolved issue as to why the parser won't accept MFString fields in braces such as ["Hello", "World"]
// .......... This error also happens on the unaltered SGI parser, I've emailed them...In the
// .......... meantime I've hacked a fix right into the parse "state" tables.  

// .......... currently unsupported nodes (will not result in valid node / field information) 
// .......... 		PixelImage, Script

// .......... 		Routes are currently supported via a callback, allowing a user to 
// .......... implement the signal connections if needed

// .......... default values for arrays (MFxxx) are currently not initialized in nodes...all such fields are zero'ed out
// .......... MFTime arrays are currently unsupported...easy to do, but no such fields are in VRML 2.0 spec

// .......... The navigationInfo node does not get parsed correctly, and should be avoided..will cause
// .......... the loader to crash when deleting the MFString field in that node.
// .......... There's a strange bug with parsing MFStrings...Frankly, rewriting or debugging the SGI code would be a pain, 
// .......... since it parses based on cryptic state tables and values I have no clue about. In the past, I've been 
// .......... able to tweak a value here and there, but it's not a long term solution. I've decided I hate VRML :)



class vrmlNode;
const MAX_FIELD_LEN = 127;
struct route {
	vrmlNode *from, *to;
	char nodeFrom[MAX_FIELD_LEN];
	char nodeTo[MAX_FIELD_LEN];
	char fieldFrom[MAX_FIELD_LEN+1];
	char fieldTo[MAX_FIELD_LEN+1];
	route() : from(0), to(0) {}
};

typedef void routeCallback(route *r);

class vrmlNode {

	const VrmlNodeType *	type;
	char 			*name;
	int 			refcount;
	vrmlNode 		*parent;
	CvectList<vrmlField> 	fields;

				vrmlNode(const VrmlNodeType *ntype);  // private so can't be called directly
				~vrmlNode(); 
	void 			deleteField(int index);
	void 			printField(::ostream& o, int index, int indent,BOOL writeDefaults=FALSE);

	static CvectList<vrmlNode *>	allNodes;

public:

	static vrmlNode *create(const char *nodeTypeName, vrmlNode *parentNode, const char *parentFieldname, vrmlNode *useNode = 0);
	static void 	release(vrmlNode *n);

	const VrmlNodeType	*getType() const {return type;}
	void 			addRef() {refcount++;}
	int 			getRefCount() {return refcount;}
	vrmlNode *		getParent() const {return parent;}

	void 			setName(const char *n);
	char * 			getName() const {return name;}

	void 			initDefaultValues();
	void 			setField(const char *fieldName, void *data);
	void 			setField(const char *fieldname, int fieldType, char *str);
	void 			*field(const char *fieldName);
	int			getNumFields() const {return fields.limit();}
	void			write(::ostream& o, int indent, BOOL writeDefaults=FALSE);

// .......... a VRMLNODE_CALLBACK returns the address of another callback 
// .......... to be used for subsequent traversal down that branch of the tree.
// .......... returns NULL to end traversal.
typedef void * (VRMLNODE_CALLBACK) (vrmlNode *n, void *info);

// .......... depth-first, in-order traversal
	void 			traverse(VRMLNODE_CALLBACK *nodeFunc, void *info);


	static int		getNumNodes();
	static vrmlNode		*findNode(char *name);

// .......... scene handling functions
	// 	NOTE: the route given to the 'routeCallback' does not currently 
	//	have its node from & to pointers initialized; only the node & field names. 
	// 	This may be implemented later on. -cc
	static vrmlNode 	*loadScene(char *vrmlfile, routeCallback *routes = 0);
	static void 		writeScene(char *vrmlfile, vrmlNode *root,BOOL writeDefaults=FALSE);
	static void 		destroyLoader();

// .......... initializes a field with string data (not meant to be used generally, 
// .......... just used externally when initializing default values in PROTO nodes
	static void 		initField(void *fv, int fieldType, char *str);
	static void 		*getFieldPtr(vrmlField& f, int fieldType);
	static BOOL 		equal(vrmlField &v1, vrmlField &v2, int fieldType);

// .......... deallocates any data and releases any nodes associated whith the given field
	static void 		destroyField(vrmlField& f, int fieldtype);

friend void *testnode(vrmlNode *n, void *info);
};


void *testnode(vrmlNode *n, void *info);

#endif
