// Added a method to return the index corresponding to a given 
//field name...also made it cas insensitive for convenience  -cc -Sept 23,1997

/**************************************************
 * VRML 2.0 Parser
 * original code was from 1996 Silicon Graphics, Inc.
 **************************************************
 */

//
// The VrmlNodeType class is responsible for storing information about node
// or prototype types.
//

#include <fstream.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "stldefs.h"
#include "VrmlNodeType.h"
#include "Vrml.h"

//
// Static list of node types.
//
CvectList<VrmlNodeType*> VrmlNodeType::typeList;

VrmlNodeType::VrmlNodeType(const char *nm)
{
	assert(nm != NULL);
	name = strdup(nm);
}

VrmlNodeType::~VrmlNodeType()
{
	free(name);

    // Free strings duplicated when fields/eventIns/eventOuts added:

	int i, num;
	num = eventIns.count();
	for (i=0; i < num; i++)
		{
		NameTypeRec* r = eventIns[i];
	    	free(r->name);
       	 delete r;
	    }

	num = eventOuts.count();
	for (i=0; i < num; i++)
		{
		NameTypeRec* r = eventOuts[i];
	    	free(r->name);
       	 delete r;
	    }

	num = fields.count();
	for (i=0; i < num; i++)
		{
		NameTypeRec* r = fields[i];
	    	free(r->name);

// .......... 	delete any prototype values allocated (note there can be no nodes allocated by the prototype fields)
		vrmlNode::destroyField(r->def, r->type);
       	 delete r;
	    }

}

void
VrmlNodeType::addToNameSpace(VrmlNodeType *_type)
{
	if (find(_type->getName()) != NULL) {
    	cerr << "PROTO " << _type->getName() << " already defined\n";
        return;
    }
    typeList.push(_type);
}

void VrmlNodeType::deleteAllFromNameSpace()
{
while (typeList.count())
	{
	VrmlNodeType *toDelete =  typeList[typeList.count()-1];
	typeList.detach(toDelete);
	delete toDelete;
	}
}

//
// One list is used to store all the node types.  Nested namespaces are
// separated by NULL elements.
// This isn't terribly efficient, but it is nice and simple.
//
void
VrmlNodeType::pushNameSpace()
{
	typeList.push((VrmlNodeType *) NULL);
}

void
VrmlNodeType::popNameSpace()
{
	// Remove everything up to and including the next NULL marker:
	int num = typeList.count(),i;
	for (i = num-1; i >= 0; i--)
	{
	VrmlNodeType *nodeType = typeList.pop();

        if (nodeType == NULL) {
            break;
        }
        else {
            // NOTE:  Instead of just deleting the VrmlNodeTypes, you will
            // probably want to reference count or garbage collect them, since
            // any nodes created as part of the PROTO implementation will
            // probably point back to their VrmlNodeType structure.
            delete nodeType;
        }
    }
}

const VrmlNodeType *
VrmlNodeType::find(const char *_name)
{
	// Look through the type stack:

	int num = typeList.count(),i;
	for (i = 0; i < num; i++)
	{
    	const VrmlNodeType *nt = typeList[i];
    	if (nt != NULL && strcmp(nt->getName(),_name) == 0) {
            return nt;
        }
    }
    return NULL;
}

void
VrmlNodeType::addEventIn(const char *name, int type)
{
	add(eventIns, name, type);
};
void
VrmlNodeType::addEventOut(const char *name, int type)
{
	add(eventOuts, name, type);
};
void
VrmlNodeType::addField(const char *name, int type)
{
	add(fields, name, type);
};
void
VrmlNodeType::addExposedField(const char *name, int type)
{
	char tmp[1000];
	add(fields, name, type);
    sprintf(tmp, "set_%s", name);
    add(eventIns, tmp, type);
    sprintf(tmp, "%s_changed", name);
    add(eventOuts, tmp, type);
};

extern NameTypeRec *currentProtoFieldRec;

void
VrmlNodeType::add(CvectList<NameTypeRec*> &recs, const char *name, int type)
{

// .......... flag whether it's a field as opposed to an event
	BOOL isField = FALSE;
	if (&recs == &fields)
		isField = TRUE;

	NameTypeRec *r = new NameTypeRec;
    r->name = strdup(name);
    r->type = type;

// .......... zero out the field...
// .......... note that this has the effect of making the node pointers SFNode and MFNode NULL by default
    memset(&r->def,0,sizeof(vrmlField));

	if (isField)
	{
// .......... just added a field name and type, now signal the lexer to add its default values
	currentProtoFieldRec = r;
	}

	recs.push(r);
}

int
VrmlNodeType::hasEventIn(const char *name) const
{
	return has(eventIns, name);
}
int
VrmlNodeType::hasEventOut(const char *name) const
{
	return has(eventOuts, name);
}
int
VrmlNodeType::hasField(const char *name) const
{
	return has(fields, name);
}
int
VrmlNodeType::hasExposedField(const char *name) const
{
	// Must have field "name", eventIn "set_name", and eventOut
    // "name_changed", all with same type:
	char tmp[1000];
	int type;
	if ( (type = has(fields, name)) == 0) return 0;

    sprintf(tmp, "set_%s\n", name);
    if (type != has(eventIns, name)) return 0;

    sprintf(tmp, "%s_changed", name);
    if (type != has(eventOuts, name)) return 0;

    return type;
}
int
VrmlNodeType::has(const CvectList<NameTypeRec*> &recs, const char *name) const
{
	int i, num = recs.count();
	for (i=0; i < num; i++)
		{
		NameTypeRec* pi = recs[i];
	    	if (strcmp((pi)->name, name) == 0)
       	 	return (pi)->type;
		}

    return 0;
}


int
VrmlNodeType::getFieldIndex(const char *fieldname, int *type) const
{

	int index = 0;
	int i, num = fields.count();
	for (i=0; i < num; i++, index++)
		{
		NameTypeRec* pi = fields[i];		

    		if (stricmp((pi)->name, fieldname) == 0)
			{
			if (type)
				*type = (pi)->type;
			return index;
			}
		}
    return -1;
}

vrmlField *VrmlNodeType::getFieldDefault(int index) const
{

	int j=0;
	int i, num = fields.count();
	for (i=0; i < num; i++, j++)
		{
		NameTypeRec* pi = fields[i];		

	    	if (j == index)
			return &((pi)->def);
		}

	assert(0); // invalid field index
    return 0;
}

int
VrmlNodeType::getFieldType(int index) const
{
	int j=0;
	int i, num = fields.count();
	for (i=0; i < num; i++, j++)
		{
		NameTypeRec* pi = fields[i];		

    		if (j == index)
			return (pi)->type;
		}

	assert(0); // invalid field index
    return -1;
}


const char *VrmlNodeType::getFieldName(int index) const
{
	int j=0;
	int i, num = fields.count();
	for (i=0; i < num; i++, j++)
		{
		NameTypeRec* pi = fields[i];		
    		if (j == index)
			return (pi)->name;
		}

	assert(0); // invalid field index
    return 0;
}

int VrmlNodeType::getNumFields() const 
{
	return fields.count();
}

