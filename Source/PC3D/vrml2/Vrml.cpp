#include <fstream.h>
#include <stdio.h>
#include <time.h>
#include <memory.h>

#include "debug.h"
#include "stldefs.h"
#include "tokens.h"
#include "vectlist.h"
#include "VrmlNodeType.h"
#include "vrml.h"

CvectList<vrmlNode *> vrmlNode::allNodes(16,32);

BOOL operator ==(vrmlField &v1, vrmlField &v2)
{
	return !memcmp(&v1,&v2,sizeof( vrmlField ));
}


BOOL vrmlNode::equal(vrmlField &v1, vrmlField &v2, int fieldType)
	{
	switch(fieldType) {

	case SFBOOL:
		return v1.Bool == v2.Bool; 
	case SFCOLOR: 
		return v1.Color == v2.Color;
	case SFFLOAT:
		return v1.Float == v2.Float;
	case SFIMAGE: 
		assert(0); // this inline image type not yet supported
		break;
	case SFINT32: 
		return v1.Int32 == v2.Int32;
	case SFROTATION: 
		return v1.Rotation == v2.Rotation;
	case SFSTRING: 
		if (v1.sfString == v2.sfString )
			return TRUE;
		if (v1.sfString && v2.sfString )
			return !strcmp(v1.sfString, v2.sfString);
		return FALSE;
	case SFTIME: 
		return v1.Time == v2.Time;
	case SFVEC2F: 
		return v1.Vec2f == v2.Vec2f;
	case SFVEC3F: 
		return v1.Vec3f == v2.Vec3f;
	case SFNODE:
		return v1.sfNode == v2.sfNode;

// .......... 	we don't support equality tests on arrays of things,...we could,
// .......... 		but it is only used right now for default values and there
// .......... 		are only a few nodes with MFxxx default values in the VRML spec.
	case MFCOLOR: 
		return FALSE;
	case MFFLOAT: 
		return FALSE;
	case MFINT32: 
		return FALSE;
	case MFROTATION: 
		return FALSE;
	case MFSTRING:
		return FALSE;
	case MFVEC2F: 
		return FALSE;
	case MFVEC3F: 
		return FALSE;
	
        case MFNODE:
		return v1.sfNode == v2.sfNode;
	default:
		assert(0); // not a valid field type 
		break;
	};
return FALSE;
}


int vrmlNode::getNumNodes() {
return allNodes.count();
}

vrmlNode *vrmlNode::findNode(char *name)
	{
	int i;
	vrmlNode *found = 0;
	for (i=0; i < allNodes.count(); i++)
		if (allNodes[i]->getName() != 0 && !strcmp(allNodes[i]->getName(), name))
			{
			found = allNodes[i];
			break;
			}
	return found;
	}

vrmlNode::vrmlNode(const VrmlNodeType *ntype)
	:type (ntype),
	refcount(1),
	parent(0),
	name(0),
	fields(ntype->getNumFields(),1) 
	{
	unsigned i;
	for (i=0; i < fields.limit(); i++)
		{
		// zero out the field
		vrmlField f; memset(&f,0,sizeof(vrmlField));

		// we must explicitly add the field to the list so the 
		// list object knows how many fields it has, i.e. the count
		// of fields will be zero even though enough physical spaces
		// has been allocated. Silly, I should have just used a plain old array.
		fields.add( f );
		}

// .......... 	initialize the default field values here, according to PROTO nodes
	initDefaultValues();
	}

void vrmlNode::initDefaultValues()
	{
	unsigned i;
	for (i=0; i < fields.limit(); i++)
		{
		deleteField( i );
		vrmlField& f = fields[i];
		f = *(type->getFieldDefault( i ));
		}
	}

vrmlNode::~vrmlNode()
	{
	int i = getNumFields();
	while ( i-- )
		deleteField( i );
	if (name)
		delete name;
	}


void vrmlNode::deleteField(int index)
	{
	int fieldType = type->getFieldType(index);

	vrmlField& f = fields[index];
	destroyField( f, fieldType );
	}


void vrmlNode::setField(const char *fieldname, int fieldType, char *str)
{

#ifdef DEBUG 
	int fType;
	type->getFieldIndex(fieldname, &fType);
	assert (fType == fieldType); // the field type must be the same as what we're setting it to
#endif

	void *fv = field( fieldname );
	vrmlNode::initField( fv, fieldType, str);
}




void tab(::ostream& o, int tabs)
	{
	int i;
//	for (i=0;i<tabs;i++)
//		o << "\t";
	for (i=0;i<tabs*3;i++)
		o << " ";
	}


void vrmlNode::write(::ostream& o, int tabLevel, BOOL writeDefaults)
	{

// .......... 	print node name and curly brackets
	tab(o, tabLevel);

	if (getName())
		o << "DEF " << getName() << " ";
	o << type->getName() << " {\n";

// .......... 	visit the node's fields
	int flds = getNumFields(),i;
	for (i=0; i < flds; i++)
		{
		printField(o, i, tabLevel+1, writeDefaults);
		}

	tab(o, tabLevel);
	o << "}\n";
	}



void vrmlNode::printField(::ostream& o, int index, int indent, BOOL writeDefaults)
{
	int i;
	int fieldType = type->getFieldType(index);

	if (! writeDefaults )
		{
// .......... 	we censure printing of some fields to save output space 
// .......... 		(if they are their default values)

		vrmlField *def = type->getFieldDefault( index );
		if (equal(*def, fields[index], fieldType))
			return;
		}

	const char *fieldName = type->getFieldName(index);
	void *fv = field( fieldName );

// .......... indent, then print the field name
	tab(o, indent);
	o << fieldName << " ";

// .......... temp variables for initializing fields
int sfi;
SFRotation *pr;
SFString *ps;
SFVec2f *v2;
SFVec3f *v3;

SFVec3f v3f;
SFVec2f v2f;
MFVec3f **pmfv3; 
MFVec3f *mfv3;
MFVec2f **pmfv2;
MFVec2f *mfv2;

MFString **pmfs;
MFString *mfs;
SFString s;

SFRotation sfr;
MFRotation **pmfr;
MFRotation *mfr;

MFInt32 **pmfi;
MFInt32 *mfi;  

MFFloat **pmff;
MFFloat *mff;
SFFloat f;

MFColor **pmfc;
MFColor *mfc;
SFColor c;

SFNode *sfn;
MFNode *mfn;

// .......... end temp vars

      switch(fieldType) {
        case SFBOOL: if ((*(SFBool*)fv))
			o << "TRUE\n";
		else
			o << "FALSE\n";
		 break;
        case SFCOLOR:
		c = *( SFColor *) fv;
		o << c.r << " " << c.g << " " << c.b << "\n";
		break;
        case SFFLOAT: o << *(SFFloat *)fv << "\n";
		break;
        case SFIMAGE: assert(0);// can't handle this inline image type yet
		 break;
        case SFINT32:
		 o << *(SFInt32 *)fv << " ";
		 break;
        case SFROTATION:
		pr = (SFRotation *) fv;
		o << pr->x << " " << pr->y << " " << pr->z << " " << pr->ang << "\n";
		 break;
        case SFSTRING:
		ps = (SFString *) fv;
		if (*ps == 0)
			o << "NULL\n";
		else
			o << "\"" << *ps << "\"" << "\n";
		 break;
        case SFVEC2F:
		v2 = (SFVec2f *) fv;
		o << v2->x << " " << v2->y << "\n";
		break;
        case SFVEC3F:
		v3 = (SFVec3f *) fv;
		o << v3->x << " " << v3->y << " " << v3->z << "\n";
		break;
	case SFNODE:
		// recursively print the node
		sfn = *(SFNode **) fv;
		if (!sfn)
			o << "NULL\n";
		else
			{
			if (sfn->getParent() != this)
				{
				// we're referencing a node, not inlining it
				char *nodeName = sfn->getName();
				assert (nodeName); // if we reference a node, it must be named
				o << "USE " << nodeName << "\n";
				}
			else
				{
				o << "\n";
				sfn->write(o, indent+1, writeDefaults);
				}
			}
		break;


		
// .......... multiple (arrayed) values which we print out on separate lines if necesary
        case MFCOLOR:
		pmfc = (MFColor **) fv;
		mfc = *pmfc;			
		if (mfc == 0)
			o << "[]\n";
		else
		{
		o << "[ ";
		for (i = 0; i < mfc->count(); i++)
			{
			o << "\n";
			tab(o, indent+1);
			c = (*mfc)[i];
			o << c.r << " " << c.g << " " << c.b << ",";
			}
		o << " ]\n";
		}
		break;
        case MFFLOAT: 
		pmff = (MFFloat **) fv;
		mff = *pmff;
		if (mff == 0) 
			o << "[]\n";
		else
		{
		o << "[ ";
		for (i = 0; i < mff->count(); i++)
			{
			if (!(i % 4))
				{
				o << "\n";
				tab(o, indent+1);
				}
			f = (*mff)[i];
			o << f << ", ";
			}
		o << "]\n";
		}
		break;
        case MFINT32: 
		pmfi = (MFInt32 **) fv;
		mfi = *pmfi;
		if (mfi == 0) // not allocated yet
			{
			o << "[]\n";
			}
		else
		{
		o << "[ ";
		for (i = 0; i < mfi->count(); i++)
			{
			if (!(i % 8))
				{
				o << "\n";
				tab(o, indent+1);
				}
			sfi = (*mfi)[i];
			o << sfi << ", ";
			}
		o << "]\n";
		}
		break;
        case MFROTATION:
		pmfr = (MFRotation **) fv;
		mfr = *pmfr;
		if (mfr == 0) // not allocated yet
			{
			o << "[]\n";
			}
		else
		{
		o << "[ ";
		for (i = 0; i < mfr->count(); i++)
			{
			o << "\n";
			tab(o, indent+1);
			sfr = (*mfr)[i];
			o << sfr.x << " " << sfr.y << " " << sfr.z << " " << sfr.ang << ",";
			}
		o << " ]\n";
		}
		break;
        case MFSTRING:
		pmfs = (MFString **) fv;
		mfs = *pmfs;
		if (mfs == 0) // not allocated yet
			{
			o << "[]\n";
			}
		else
		{
		o << "[ ";
		for (i = 0; i < mfs->count(); i++)
			{
			o << "\n";
			tab(o, indent+1);
			s = (*mfs)[i];
			o << "\"" << s << "\",";
			}
		o << " ]\n";
		}
		break;
        case MFVEC2F: 
		pmfv2 = (MFVec2f **) fv;
		mfv2 = *pmfv2;
		if (mfv2 == 0) // not allocated yet
			{
			o << "[]\n";
			}
		else
		{
		o << "[ ";
		for (i = 0; i < mfv2->count(); i++)
			{
			o << "\n";
			tab(o, indent+1);
			v2f = (*mfv2)[i];
			o << v2f.x << " " << v2f.y << ",";
			}
		o << " ]\n";
		}
		break;
        case MFVEC3F:
		pmfv3 = (MFVec3f **) fv;
		mfv3 = *pmfv3;
		if (mfv3 == 0) // not allocated yet
			{
			o << "[]\n";
			}
		else
		{
		o << "[ ";
		for (i = 0; i < mfv3->count(); i++)
			{
			o << "\n";
			tab(o, indent+1);
			v3f = (*mfv3)[i];
			o << v3f.x << " " << v3f.y << " " << v3f.z << ",";
			}
		o << " ]\n";
		}
		break;

        case MFNODE:
		// recursively print the nodes
		mfn = *(MFNode **) fv;
		if (!mfn)
			o << "[]\n";
		else
		{	  
		o << "[ \n";
		for (i = 0; i < mfn->count(); i++)
			{
			SFNode *sfn = (*mfn)[i];
			if (sfn->getParent() != this)
				{
 				// we're referencing a node, not inlining it
				char *nodeName = sfn->getName();
				assert (nodeName); // if we reference a node, it must be named
				tab(o, indent+1);
				o << "USE " << nodeName;
				}
			else
				{
				sfn->write(o, indent+1, writeDefaults);
				tab(o, indent+1);
				}
			o << "," << " # end node: " << i << " tab level: " << indent << "\n";
			}
		tab(o, indent);
		o << " ]\n";
		}
		break;

        default: assert (0); // bad type
	};
}


void vrmlNode::setName(const char *n)
	{
	if ( !name || (name && strlen(name) < strlen(n)) )
			{
			if (name)
				delete name;
			name = new char[strlen(n)+1];
			}
	strcpy(name, n);
	}

vrmlNode *vrmlNode::create(const char *nodeTypeName, vrmlNode *parentNode, const char *parentFieldname, vrmlNode *useNode)
	{

	vrmlNode *n = useNode;
	if (!useNode)
		{
// .......... get node type object (i.e. the stuff that's loaded as part of the PROTO header)
		const VrmlNodeType *ntype = VrmlNodeType::find(nodeTypeName);
		assert (ntype);

		n = new vrmlNode(ntype);
		allNodes.add( n );
		n->parent = parentNode;
		}

// .......... 	let the parent know
	if (parentNode)
		{
		int fType;
		int parentFieldIndex = parentNode->type->getFieldIndex(parentFieldname, &fType);
		if  (fType == MFNODE)
			{
			MFNode *mfn = parentNode->fields[ parentFieldIndex ].mfNode;

// .......... We set the initial size of the children array to one as many
// .......... nodes have only one child node and we don't want to use up unnecessary memory.
// .......... Note that we only allocate an array if parent actually has children.
			if (!mfn)
				{
				mfn = new MFNode(1,4);
				parentNode->fields[ parentFieldIndex ].mfNode = mfn;
				}

// .......... add the child
			mfn->add( n );
			}

		else if (fType == SFNODE)
			{
			SFNode **sfn = & parentNode->fields[ parentFieldIndex ].sfNode;
			assert (! *sfn); // node field can't already be allocated 
			*sfn = n;
			}
		else 
			assert (0); // must be an SF or MF NODE

// .......... if we're referencing an existing node, it needs another ref count		
		if (useNode)
			useNode->addRef();
		}

	return n;
	}

void vrmlNode::release(vrmlNode *n)
	{
	n->refcount--;
	if (!n->refcount)
		{
		allNodes.detach( n );
		delete n;
		}
	}


void vrmlNode::destroyField(vrmlField& f, int fieldtype)
	{
	int i;

	switch (fieldtype)
	{
	case SFBOOL:
		break;
	case SFCOLOR: 
		break;
	case SFFLOAT: 
		break;
	case SFIMAGE: 
		//assert(0); // this inline image type not yet supported
		break;
	case SFINT32: 
		break;
	case SFROTATION: 
		break;
	case SFSTRING: 
		if (f.sfString)
			delete f.sfString;
		break;
	case SFTIME: 
		break;
	case SFVEC2F: 
		break;
	case SFVEC3F: 
		break;
	case SFNODE:
		if (f.sfNode)
			release (f.sfNode);
		break;
	case MFCOLOR: 
		if (f.mfColor)
			delete f.mfColor;
		break;
	case MFFLOAT: 
		if (f.mfFloat)
			delete f.mfFloat;
		break;
	case MFINT32: 
		if (f.mfInt32)
			delete f.mfInt32;
		break;
	case MFROTATION: 
		if (f.mfRotation)
			delete f.mfRotation;
		break;
	case MFSTRING:
		// Thre's a strange bug with parsing MFStrings...Frankly, rewriting or debugging the SGI code would be a pain, 
		// since it parses based on cryptic state tables and values I have no clue about. In the past, I've been 
		// able to tweak a value here and there, but it's not a long term solution. I've decided I hate VRML :)
		 if (f.mfString)
			{
			for (i=0; i < f.mfString->count(); i++)
				{
				SFString s = (*(f.mfString))[i];
				if (s)
					delete s;
				}
			delete f.mfString;
			}
		break;
	case MFVEC2F: 
		if (f.mfVec2f)
			delete f.mfVec2f;
		break;
	case MFVEC3F: 
		if (f.mfVec3f)
			delete f.mfVec3f;
		break;
	
        case MFNODE:
		if (f.mfNode)
			{
// .......... release the references to child nodes (we reference count because
// .......... of things like the DEF/USE pair which allows other nodes to refer
// .......... to a named node). NOTE: Circular references are forbidden and unsupported.
			for (i=0; i < f.mfNode->count(); i++)
				{
				SFNode *sfn = (*f.mfNode)[i];
				release (sfn);
				}
			delete f.mfNode;
			}

		break;
	default:
		assert(0); // not a valid field type 
		break;
	};

// .......... zero out the field...
// .......... note that this has the effect of making the node pointers SFNode and MFNode NULL by default
    memset(&f,0,sizeof(vrmlField));
}


void vrmlNode::initField(void *fv, int fieldType, char *str)
{

// .......... temp variables for initializing fields
SFRotation *pr;
SFString *ps;
SFVec2f *v2;
SFVec3f *v3;
int len;

SFVec3f v3f;
SFVec2f v2f;
MFVec3f **pmfv3; 
MFVec3f *mfv3;
MFVec2f **pmfv2;
MFVec2f *mfv2;

MFString **pmfs;
MFString *mfs;
SFString s;

SFRotation sfr;
MFRotation **pmfr;
MFRotation *mfr;

MFInt32 **pmfi;
MFInt32 *mfi;  
int i;

MFFloat **pmff;
MFFloat *mff;
SFFloat f;

MFColor **pmfc;
MFColor *mfc;
SFColor c;
SFColor *sfc;
// .......... end temp vars

      switch(fieldType) {
        case SFBOOL: sscanf(str,"%d",(SFBool *)fv);
		 break;
        case SFCOLOR:
		sfc = (SFColor *) fv;
		sscanf(str,"%f %f %f",&sfc->r,&sfc->g,&sfc->b);
		break;
        case SFFLOAT: sscanf(str,"%f",(SFFloat *)fv);
		break;
        case SFIMAGE: assert(0);// can't handle this inline image type yet
		 break;
        case SFINT32: sscanf(str,"%d",(SFInt32 *)fv);
		 break;
        case SFROTATION:
		pr = (SFRotation *) fv;
		sscanf(str,"%f %f %f %f",&pr->x,&pr->y,&pr->z,&pr->ang);
		 break;
        case SFSTRING:
		len = strlen(str);
		ps = (SFString *) fv;
		assert (*ps == 0); // string can't be allocated yet
		*ps = new char[len+1];
		strcpy (*ps, str);
		 break;
        case SFTIME: sscanf(str,"%f",(SFTime *)fv);
		break;
        case SFVEC2F:
		v2 = (SFVec2f *) fv;
		sscanf(str,"%f %f",&v2->x,&v2->y);
		break;
        case SFVEC3F:
		v3 = (SFVec3f *) fv;
		sscanf(str,"%f %f %f",&v3->x,&v3->y,&v3->z);
		break;

// .......... multiple (arrayed) values which we create and add to
        case MFCOLOR:
		pmfc = (MFColor **) fv;
		mfc = *pmfc;			
		if (mfc == 0) // not allocated yet
			{
			mfc = new MFColor(8,16);
			*pmfc = mfc;
			}
		sscanf(str,"%f %f %f",&c.r,&c.g,&c.b);
		mfc->add( c );
		break;
        case MFFLOAT: 
		pmff = (MFFloat **) fv;
		mff = *pmff;
		if (mff == 0) // not allocated yet
			{
			mff = new MFFloat(8,16);
			*pmff = mff;
			}
		sscanf(str,"%f",&f);
		mff->add( f );
		break;
        case MFINT32: 
		pmfi = (MFInt32 **) fv;
		mfi = *pmfi;
		if (mfi == 0) // not allocated yet
			{
			mfi = new MFInt32(8,16);
			*pmfi = mfi;
			}
		sscanf(str,"%d", &i);
		mfi->add ( i );
		break;
        case MFROTATION:
		pmfr = (MFRotation **) fv;
		mfr = *pmfr;
		if (mfr == 0) // not allocated yet
			{
			mfr = new MFRotation(8,16);
			*pmfr = mfr;
			}
		sscanf(str,"%f %f %f %f",&sfr.x,&sfr.y,&sfr.z,&sfr.ang);
		mfr->add( sfr );
		break;
        case MFSTRING:
		pmfs = (MFString **) fv;
		mfs = *pmfs;
		if (mfs == 0) // not allocated yet
			{
			mfs = new MFString(4,16);
			*pmfs = mfs;
			}
		len = strlen(str);
		s = new char[len+1];
		strcpy(s, str);
		mfs->add( s );
		break;
        case MFVEC2F: 
		pmfv2 = (MFVec2f **) fv;
		mfv2 = *pmfv2;
		if (mfv2 == 0) // not allocated yet
			{
			mfv2 = new MFVec2f(8,16);
			*pmfv2 = mfv2;
			}
		sscanf(str,"%f %f",&v2f.x,&v2f.y);
		mfv2->add( v2f );
		break;
        case MFVEC3F:
		pmfv3 = (MFVec3f **) fv;
		mfv3 = *pmfv3;
		if (mfv3 == 0) // not allocated yet
			{
			mfv3 = new MFVec3f(8,16);
			*pmfv3 = mfv3;
			}
		sscanf(str,"%f %f %f",&v3f.x,&v3f.y,&v3f.z);
		mfv3->add( v3f );
		break;

        case SFNODE:
        case MFNODE:
		assert (0); // cannot set nodes in this manner
		break;
        
        default: assert (0); // bad type
	};
}



void *vrmlNode::field(const char *fieldName)
	{
	int fieldType;
	int index = type->getFieldIndex(fieldName, &fieldType);

	assert (index != -1 && index < (int) fields.limit()); // not a valid field

	vrmlField& f = fields[index];

	return getFieldPtr(f,fieldType);
	}

void *vrmlNode::getFieldPtr(vrmlField& f, int fieldType)
	{
	switch (fieldType)
	{
	case SFBOOL:
		return (void *)	&f.Bool;
		break;
	case SFCOLOR: 
		return (void *) &f.Color;
		break;
	case SFFLOAT: 
		return (void *) &f.Float;
		break;
	case SFIMAGE: 
		assert(0); // this inline image type not yet supported
		break;
	case SFINT32: 
		return (void *) &f.Int32;
		break;
	case SFROTATION: 
		return (void *) &f.Rotation;
		break;
	case SFSTRING: 
		return (void *) &f.sfString;
		break;
	case SFTIME: 
		return (void *) &f.Time;
		break;
	case SFVEC2F: 
		return (void *) &f.Vec2f;
		break;
	case SFVEC3F: 
		return (void *) &f.Vec3f;
		break;
	case SFNODE: 
		return (void *) &f.sfNode;
		break;
	case MFCOLOR: 
		return (void *) &f.mfColor;
		break;
	case MFFLOAT: 
		return (void *) &f.mfFloat;
		break;
	case MFINT32: 
		return (void *) &f.mfInt32;
		break;
	case MFNODE: 
		return (void *) &f.mfNode;
		break;
	case MFROTATION: 
		return (void *) &f.mfRotation;
		break;
	case MFSTRING: 
		return (void *) &f.mfString;
		break;
	case MFVEC2F: 
		return (void *) &f.mfVec2f;
		break;
	case MFVEC3F: 
		return (void *) &f.mfVec3f;
		break;
	
	default:
		assert(0); // not a valid field type 
		break;
	};
	
	return 0;
	}


void vrmlNode::traverse(VRMLNODE_CALLBACK *nodeFunc, void *info)
	{

// .......... 	visit this node
	VRMLNODE_CALLBACK *callback = (VRMLNODE_CALLBACK *)(*nodeFunc)(this, info);
	
// .......... stop traversal if indicated
	if (!callback)
		return;

// .......... traverse the MFNode types recursively
	unsigned i;
	for (i=0; i < fields.limit(); i++)
			{
			int ftype = type->getFieldType( i );
			if (ftype == SFNODE)
				{
				SFNode *sfn = fields[i].sfNode;
				if (sfn)
					sfn->traverse(callback, info );
				continue;
				}

			if (ftype != MFNODE)
				continue;

			int j;
			MFNode *mfn = fields[i].mfNode;
			if (!mfn)
				continue;
			for (j=0; j < mfn->count(); j++)
				{
				vrmlNode *nextchild = (*mfn)[j];
				nextchild->traverse(callback, info );
				}
			}
	}





// .......... references to VRML2 parser code & variable interface
extern vrmlNode *parserRoot;

extern int yyparse();
extern void yyResetLineNumber();
extern int yydebug;
extern int yy_flex_debug;
extern FILE *yyin;

BOOL fileInMemory = FALSE;
int memfilepos = 0;
BOOL protosLoaded = FALSE;
BOOL loadingProtos = FALSE;
NameTypeRec *currentProtoFieldRec;
route callbackRoute;
routeCallback *router;

// .......... buffer to hold the PROTO nodes contents so they don't have to be loaded from an external file
// (not working yet)
char PROTOS[] = "";
#if 0
char PROTOS[] = "\
\
#VRML V2.0 utf8\
#\
# **************************************************\
# * VRML 2.0 Parser\
# * Copyright (C) 1996 Silicon Graphics, Inc.\
# *\
# * Author(s)    : Gavin Bell\
# *                Daniel Woods (first port)\
# **************************************************\
#\
# Definitions for all of the nodes built-in to the spec.\
# Taken almost directly from the VRML 2.0 final spec:\
\
PROTO Anchor [\
  eventIn      MFNode   addChildren\
  eventIn      MFNode   removeChildren\
  exposedField MFNode   children        []\
  exposedField SFString description     \"\" \
  exposedField MFString parameter       []\
  exposedField MFString url             []\
  field        SFVec3f  bboxCenter      0 0 0\
  field        SFVec3f  bboxSize        -1 -1 -1\
] { }\
\
PROTO Appearance [\
  exposedField SFNode material          NULL\
  exposedField SFNode texture           NULL\
  exposedField SFNode textureTransform  NULL\
] { }\
\
PROTO AudioClip [\
  exposedField   SFString description  \"\"\
  exposedField   SFBool   loop         FALSE\
  exposedField   SFFloat  pitch        1.0\
  exposedField   SFTime   startTime    0\
  exposedField   SFTime   stopTime     0\
  exposedField   MFString url          []\
  eventOut       SFTime   duration_changed\
  eventOut       SFBool   isActive\
] { }\
\
PROTO Background [\
  eventIn      SFBool   set_bind\
  exposedField MFFloat  groundAngle  []\
  exposedField MFColor  groundColor  []\
  exposedField MFString backUrl      []\
  exposedField MFString bottomUrl    []\
  exposedField MFString frontUrl     []\
  exposedField MFString leftUrl      []\
  exposedField MFString rightUrl     []\
  exposedField MFString topUrl       []\
  exposedField MFFloat  skyAngle     []\
  exposedField MFColor  skyColor     [ 0 0 0  ]\
  eventOut     SFBool   isBound\
] { }\
\
PROTO Billboard [\
  eventIn      MFNode   addChildren\
  eventIn      MFNode   removeChildren\
  exposedField SFVec3f  axisOfRotation  0 1 0\
  exposedField MFNode   children        []\
  field        SFVec3f  bboxCenter      0 0 0\
  field        SFVec3f  bboxSize        -1 -1 -1\
] { }\
\
PROTO Box [\
  field    SFVec3f size  2 2 2 \
] { }\
\
PROTO Collision [ \
  eventIn      MFNode   addChildren\
  eventIn      MFNode   removeChildren\
  exposedField MFNode   children        []\
  exposedField SFBool   collide         TRUE\
  field        SFVec3f  bboxCenter      0 0 0\
  field        SFVec3f  bboxSize        -1 -1 -1\
  field        SFNode   proxy           NULL\
  eventOut     SFTime   collideTime\
] { }\
\
PROTO Color [\
  exposedField MFColor color     []\
] { }\
\
PROTO ColorInterpolator [\
  eventIn      SFFloat set_fraction\
  exposedField MFFloat key       []\
  exposedField MFColor keyValue  []\
  eventOut     SFColor value_changed\
] { }\
\
PROTO Cone [\
  field     SFFloat   bottomRadius 1\
  field     SFFloat   height       2\
  field     SFBool    side         TRUE\
  field     SFBool    bottom       TRUE\
] { }\
\
PROTO Coordinate [\
  exposedField MFVec3f point  []\
] { }\
\
PROTO CoordinateInterpolator [\
  eventIn      SFFloat set_fraction\
  exposedField MFFloat key       []\
  exposedField MFVec3f keyValue  []\
  eventOut     MFVec3f value_changed\
] { }\
\
PROTO Cylinder [\
  field    SFBool    bottom  TRUE\
  field    SFFloat   height  2\
  field    SFFloat   radius  1\
  field    SFBool    side    TRUE\
  field    SFBool    top     TRUE\
] { }\
\
PROTO CylinderSensor [\
  exposedField SFBool     autoOffset TRUE\
  exposedField SFFloat    diskAngle  0.262\
  exposedField SFBool     enabled    TRUE\
  exposedField SFFloat    maxAngle   -1\
  exposedField SFFloat    minAngle   0\
  exposedField SFFloat    offset     0\
  eventOut     SFBool     isActive\
  eventOut     SFRotation rotation_changed\
  eventOut     SFVec3f    trackPoint_changed\
] { }\
\
PROTO DirectionalLight [\
  exposedField SFFloat ambientIntensity  0 \
  exposedField SFColor color             1 1 1\
  exposedField SFVec3f direction         0 0 -1\
  exposedField SFFloat intensity         1 \
  exposedField SFBool  on                TRUE \
] { }\
\
PROTO ElevationGrid [\
  eventIn      MFFloat  set_height\
  exposedField SFNode   color             NULL\
  exposedField SFNode   normal            NULL\
  exposedField SFNode   texCoord          NULL\
  field        SFBool   ccw               TRUE\
  field        SFBool   colorPerVertex    TRUE\
  field        SFFloat  creaseAngle       0\
  field        MFFloat  height            []\
  field        SFBool   normalPerVertex   TRUE\
  field        SFBool   solid             TRUE\
  field        SFInt32  xDimension        0\
  field        SFFloat  xSpacing          0.0\
  field        SFInt32  zDimension        0\
  field        SFFloat  zSpacing          0.0\
\
] { }\
\
PROTO Extrusion [\
  eventIn MFVec2f    set_crossSection\
  eventIn MFRotation set_orientation\
  eventIn MFVec2f    set_scale\
  eventIn MFVec3f    set_spine\
  field   SFBool     beginCap         TRUE\
  field   SFBool     ccw              TRUE\
  field   SFBool     convex           TRUE\
  field   SFFloat    creaseAngle      0\
  field   MFVec2f    crossSection     [ 1 1, 1 -1, -1 -1, -1 1, 1 1 ]\
  field   SFBool     endCap           TRUE\
  field   MFRotation orientation      0 0 1 0\
  field   MFVec2f    scale            1 1\
  field   SFBool     solid            TRUE\
  field   MFVec3f    spine            [ 0 0 0, 0 1 0 ]\
] { }\
\
PROTO Fog [\
  exposedField SFColor  color            1 1 1\
  exposedField SFString fogType          \"LINEAR\"\
  exposedField SFFloat  visibilityRange  0\
  eventIn      SFBool   set_bind\
  eventOut     SFBool   isBound\
] { }\
\
PROTO FontStyle [\
  field SFString family     \"SERIF\"\
  field SFBool   horizontal  TRUE\
  field MFString justify     \"BEGIN\"\
  field SFString language    \"\"\
  field SFBool   leftToRight TRUE\
  field SFFloat  size       1.0\
  field SFFloat  spacing     1.0\
  field SFString style       \"PLAIN\"\
  field SFBool   topToBottom TRUE\
] { }\
\
PROTO Group [\
  eventIn      MFNode  addChildren\
  eventIn      MFNode  removeChildren\
  exposedField MFNode  children   []\
  field        SFVec3f bboxCenter 0 0 0\
  field        SFVec3f bboxSize   -1 -1 -1\
] { }\
\
PROTO ImageTexture [\
  exposedField MFString url     []\
  field        SFBool   repeatS TRUE\
  field        SFBool   repeatT TRUE\
] { }\
\
PROTO IndexedFaceSet [ \
  eventIn       MFInt32 set_colorIndex\
  eventIn       MFInt32 set_coordIndex\
  eventIn       MFInt32 set_normalIndex\
  eventIn       MFInt32 set_texCoordIndex\
  exposedField  SFNode  color             NULL\
  exposedField  SFNode  coord             NULL\
  exposedField  SFNode  normal            NULL\
  exposedField  SFNode  texCoord          NULL\
  field         SFBool  ccw               TRUE\
  field         MFInt32 colorIndex        []\
  field         SFBool  colorPerVertex    TRUE\
  field         SFBool  convex            TRUE\
  field         MFInt32 coordIndex        []\
  field         SFFloat creaseAngle       0\
  field         MFInt32 normalIndex       []\
  field         SFBool  normalPerVertex   TRUE\
  field         SFBool  solid             TRUE\
  field         MFInt32 texCoordIndex     []\
] { }\
\
PROTO IndexedLineSet [\
  eventIn       MFInt32 set_colorIndex\
  eventIn       MFInt32 set_coordIndex\
  exposedField  SFNode  color             NULL\
  exposedField  SFNode  coord             NULL\
  field         MFInt32 colorIndex        []\
  field         SFBool  colorPerVertex    TRUE\
  field         MFInt32 coordIndex        []\
] { }\
\
PROTO Inline [\
  exposedField MFString url        []\
  field        SFVec3f  bboxCenter 0 0 0\
  field        SFVec3f  bboxSize   -1 -1 -1\
] { }\
\
PROTO LOD [\
  exposedField MFNode  level    [] \
  field        SFVec3f center   0 0 0\
  field        MFFloat range    [] \
] { }\
\
PROTO Material [\
  exposedField SFFloat ambientIntensity  0.2\
  exposedField SFColor diffuseColor      0.8 0.8 0.8\
  exposedField SFColor emissiveColor     0 0 0\
  exposedField SFFloat shininess         0.2\
  exposedField SFColor specularColor     0 0 0\
  exposedField SFFloat transparency      0\
] { }\
\
PROTO MovieTexture [\
  exposedField SFBool   loop       FALSE\
  exposedField SFFloat  speed      1\
  exposedField SFTime   startTime  0\
  exposedField SFTime   stopTime   0\
  exposedField MFString url       []\
  field        SFBool   repeatS    TRUE\
  field        SFBool   repeatT    TRUE\
  eventOut     SFFloat  duration_changed\
  eventOut     SFBool   isActive\
] { }\
\
PROTO NavigationInfo [\
  eventIn      SFBool   set_bind\
  exposedField MFFloat  avatarSize       [ 0.25, 1.6, 0.75 ]\
  exposedField SFBool   headlight        TRUE\
  exposedField SFFloat  speed            1.0 \
  exposedField MFString type             \"WALK\" \
  exposedField SFFloat  visibilityLimit  0.0 \
  eventOut     SFBool   isBound\
] { }\
\
PROTO Normal [\
  exposedField MFVec3f vector []\
] { }\
\
PROTO NormalInterpolator [\
  eventIn      SFFloat set_fraction\
  exposedField MFFloat key       []\
  exposedField MFVec3f keyValue  []\
  eventOut     MFVec3f value_changed\
] { }\
\
PROTO OrientationInterpolator [\
  eventIn      SFFloat    set_fraction\
  exposedField MFFloat    key       []\
  exposedField MFRotation keyValue  []\
  eventOut     SFRotation value_changed\
] { }\
\
PROTO PixelTexture [\
  exposedField SFImage  image      0 0 0\
  field        SFBool   repeatS    TRUE\
  field        SFBool   repeatT    TRUE\
] { }\
\
PROTO PlaneSensor [\
  exposedField SFBool  autoOffset  TRUE\
  exposedField SFBool  enabled     TRUE\
  exposedField SFVec2f maxPosition -1 -1\
  exposedField SFVec2f minPosition 0 0\
  exposedField SFVec3f offset      0 0 0\
  eventOut     SFBool  isActive\
  eventOut     SFVec3f trackPoint_changed\
  eventOut     SFVec3f translation_changed\
] { }\
\
PROTO PointLight [\
  exposedField SFFloat ambientIntensity  0 \
  exposedField SFVec3f attenuation       1 0 0\
  exposedField SFColor color             1 1 1 \
  exposedField SFFloat intensity         1\
  exposedField SFVec3f location          0 0 0\
  exposedField SFBool  on                TRUE \
  exposedField SFFloat radius            100\
] { }\
\
PROTO PointSet [\
  exposedField  SFNode  color      NULL\
  exposedField  SFNode  coord      NULL\
] { }\
\
PROTO PositionInterpolator [\
  eventIn      SFFloat set_fraction\
  exposedField MFFloat key       []\
  exposedField MFVec3f keyValue  []\
  eventOut     SFVec3f value_changed\
] { }\
\
PROTO ProximitySensor [\
  exposedField SFVec3f    center      0 0 0\
  exposedField SFVec3f    size        0 0 0\
  exposedField SFBool     enabled     TRUE\
  eventOut     SFBool     isActive\
  eventOut     SFVec3f    position_changed\
  eventOut     SFRotation orientation_changed\
  eventOut     SFTime     enterTime\
  eventOut     SFTime     exitTime\
] { }\
\
PROTO ScalarInterpolator [\
  eventIn      SFFloat set_fraction\
  exposedField MFFloat key       []\
  exposedField MFFloat keyValue  []\
  eventOut     SFFloat value_changed\
] { }\
\
PROTO Script [\
  exposedField MFString url           [ ] \
  field        SFBool   directOutput  FALSE\
  field        SFBool   mustEvaluate  FALSE\
] { }\
\
PROTO Shape [\
  field SFNode appearance NULL\
  field SFNode geometry   NULL\
] { }\
\
PROTO Sound [\
  exposedField SFVec3f  direction     0 0 1\
  exposedField SFFloat  intensity     1\
  exposedField SFVec3f  location      0 0 0\
  exposedField SFFloat  maxBack       10\
  exposedField SFFloat  maxFront      10\
  exposedField SFFloat  minBack       1\
  exposedField SFFloat  minFront      1\
  exposedField SFFloat  priority      0\
  exposedField SFNode   source        NULL\
  field        SFBool   spatialize    TRUE\
] { }\
\
PROTO Sphere [\
  field SFFloat radius  1\
] { }\
\
PROTO SphereSensor [\
  exposedField SFBool     autoOffset TRUE\
  exposedField SFBool     enabled    TRUE\
  exposedField SFRotation offset     0 1 0 0\
  eventOut     SFBool     isActive\
  eventOut     SFRotation rotation_changed\
  eventOut     SFVec3f    trackPoint_changed\
] { }\
\
PROTO SpotLight [\
  exposedField SFFloat ambientIntensity  0 \
  exposedField SFVec3f attenuation       1 0 0\
  exposedField SFFloat beamWidth         1.570796\
  exposedField SFColor color             1 1 1 \
  exposedField SFFloat cutOffAngle       0.785398 \
  exposedField SFVec3f direction         0 0 -1\
  exposedField SFFloat intensity         1  \
  exposedField SFVec3f location          0 0 0  \
  exposedField SFBool  on                TRUE\
  exposedField SFFloat radius            100\
] { }\
\
PROTO Switch [\
  exposedField    MFNode  choice      []\
  exposedField    SFInt32 whichChoice -1\
] { }\
\
PROTO Text [\
  exposedField  MFString string    []\
  field         SFNode   fontStyle NULL\
  field         MFFloat  length    []\
  field         SFFloat  maxExtent 0.0\
] { }\
\
PROTO TextureCoordinate [\
  exposedField MFVec2f point []\
] { }\
\
PROTO TextureTransform [\
  exposedField SFVec2f center      0 0\
  exposedField SFFloat rotation    0\
  exposedField SFVec2f scale       1 1\
  exposedField SFVec2f translation 0 0\
] { }\
\
PROTO TimeSensor [\
  exposedField SFTime   cycleInterval 1\
  exposedField SFBool   enabled       TRUE\
  exposedField SFBool   loop          FALSE\
  exposedField SFTime   startTime     0\
  exposedField SFTime   stopTime      0\
  eventOut     SFTime   cycleTime\
  eventOut     SFFloat  fraction_changed\
  eventOut     SFBool   isActive\
  eventOut     SFTime   time\
] { }\
\
PROTO TouchSensor [\
  exposedField SFBool  enabled TRUE\
  eventOut     SFVec3f hitNormal_changed\
  eventOut     SFVec3f hitPoint_changed\
  eventOut     SFVec2f hitTexCoord_changed\
  eventOut     SFBool  isActive\
  eventOut     SFBool  isOver\
  eventOut     SFTime  touchTime\
] { }\
\
PROTO Transform [\
  eventIn      MFNode      addChildren\
  eventIn      MFNode      removeChildren\
  exposedField SFVec3f     center           0 0 0\
  exposedField MFNode      children         []\
  exposedField SFRotation  rotation         0 0 1  0\
  exposedField SFVec3f     scale            1 1 1\
  exposedField SFRotation  scaleOrientation 0 0 1  0\
  exposedField SFVec3f     translation      0 0 0\
  field        SFVec3f     bboxCenter       0 0 0\
  field        SFVec3f     bboxSize         -1 -1 -1\
] { }\
\
PROTO Viewpoint [\
  eventIn      SFBool     set_bind\
  exposedField SFFloat    fieldOfView    0.785398\
  exposedField SFBool     jump           TRUE\
  exposedField SFRotation orientation    0 0 1  0\
  exposedField SFVec3f    position       0 0 10\
  field        SFString   description    \"\"\
  eventOut     SFTime     bindTime\
  eventOut     SFBool     isBound\
] { }\
\
PROTO VisibilitySensor [\
  exposedField SFVec3f center   0 0 0\
  exposedField SFBool  enabled  TRUE\
  exposedField SFVec3f size     0 0 0\
  eventOut     SFTime  enterTime\
  eventOut     SFTime  exitTime\
  eventOut     SFBool  isActive\
] { }\
\
PROTO WorldInfo [\
  field MFString info  []\
  field SFString title \"\"\
] { }\
\
";
#endif

extern int freadMem(void *buf,size_t size, size_t count)
	{
	int len = size*count;
	memcpy(buf, &PROTOS[memfilepos], len);
	memfilepos += len;
	return len;
	}


vrmlNode *vrmlNode::loadScene(char *vrmlfile,routeCallback *routes)
{
    if (!protosLoaded)
	   {
     	   currentProtoFieldRec=0;
	   loadingProtos = TRUE;
	   fileInMemory = FALSE;
	   memfilepos = 0;

	     //
	    // Read in PROTO declarations for all of the standard nodes:
	    //
	    yydebug = 0;
	    yy_flex_debug = 0;

	    FILE *standardNodes=0;
	    if (!fileInMemory)
		{
         // look for the standard nodes file in the exectuable's directory
    	char drive[_MAX_DRIVE];
	    char dir[_MAX_DIR];
	    char fullpath[_MAX_PATH];
	    char nodepath[_MAX_PATH];
        GetModuleFileName( NULL, fullpath, _MAX_PATH);
       	_splitpath(fullpath,drive,dir,0,0);

		strcpy(nodepath,drive);
		strcat(nodepath,dir);
		strcat(nodepath,"standardNodes.wrl");

		// reading Protos from a file
		standardNodes = fopen(nodepath, "r");
		if (standardNodes == NULL) {
		    	MSG("standardNodes.wrl", "can't open standard node description file");
		        return 0;
	    		}
		yyin = standardNodes;
		}
	    else
		yyin = NULL;

	    yyparse();
	    yyin = NULL;
	    yyResetLineNumber();
	    if (standardNodes)
		fclose(standardNodes);

	    fileInMemory = FALSE;
	    protosLoaded = TRUE;
     	    loadingProtos = FALSE;
     	   currentProtoFieldRec=0;
	}

    int flags = 0;
    yydebug = flags & 1;
    yy_flex_debug = flags & 2;

    char filename[100];
//    cerr << "Enter test file name: ";
//    cin >> filename;

	strcpy(filename,vrmlfile);
    yyin = fopen(filename, "r");

    if (yyin == NULL) {
    	MSG(filename, "can't open file");
        return 0;
    }

    // For this little test application, pushing and popping the node
    // namespace isn't really necessary.  But each VRML .wrl file is
    // a separate namespace for PROTOs (except for things predefined
    // in the spec), and pushing/popping the namespace when reading each
    // file is a good habit to get into:
    VrmlNodeType::pushNameSpace();

// .......... create the root node (necessary because the VRML file does not necessarily have a root node itself)
// .......... use "Anchor" so that the grouping does not introduce another transform
    parserRoot = vrmlNode::create("Anchor", 0, 0, 0);
    yyResetLineNumber();

// .......... setup the callback variables for the lexer's ROUTE case
    router = routes;
	
// .......... and GO!
	yyparse();
    VrmlNodeType::popNameSpace();
	fclose(yyin);
    return parserRoot;
}

extern void deleteVrmlBuffer();
void vrmlNode::destroyLoader()
{
if (protosLoaded)
{
		VrmlNodeType::deleteAllFromNameSpace();
}
deleteVrmlBuffer();
}


void vrmlNode::writeScene(char *vrmlfile, vrmlNode *root, BOOL writeDefaults)
{
	char stime[32], sdate[32];

	::ofstream *vrmlF = new ::ofstream(vrmlfile,::ios::out);

	*vrmlF << "#VRML V2.0 utf8\n";
	*vrmlF << "#Copyright Artech Studios, 1997\n";
	*vrmlF << "#	Creation date / time: \t" << _strdate(sdate) << "...." << _strtime(stime) << "\n\n";
	
	root->write( *vrmlF, 0, writeDefaults );

	*vrmlF << "\n#end of file.\n";
	delete vrmlF;
}


void *testnode(vrmlNode *n, void *info)
	{
	int i;
	for (i=0; i < n->getNumFields(); i++)
		{
		// just to inspect in a debugger
		vrmlField f = n->fields[i];
		if (!stricmp(n->type->getName(), "Coordinate"))
			{
			MFVec3f& v3f = **(MFVec3f **) n->field("point");
			int j;
			for (j=0; j < v3f.count(); j++)
				{
				SFVec3f v3 = v3f[j];
				}
			}
		if (!stricmp(n->type->getName(), "Text"))
			{
			MFString& s = **(MFString **) n->field("string");
			SFNode& sfn = **(SFNode **) n->field("fontStyle");
			MFFloat& mf = **(MFFloat **) n->field("length");
			SFFloat& f = *(SFFloat *) n->field("maxExtent");

			}
		}
	return testnode;
	}


