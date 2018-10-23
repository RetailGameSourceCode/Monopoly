/**************************************************
 * VRML 2.0 Parser
 * original code was from 1996 Silicon Graphics, Inc.
 **************************************************
 */
#ifndef VRMLNODETYPE_H
#define VRMLNODETYPE_H
//
// The VrmlNodeType class is responsible for storing information about node
// or prototype types.
//

#include "field.h" //used for initializing default field values

    typedef struct {
    	char *name;
        int type;
	vrmlField def;
    } NameTypeRec;

class VrmlNodeType {
  public:
  	// Constructor.  Takes name of new type (e.g. "Transform" or "Box")
    // Copies the string given as name.
    VrmlNodeType(const char *nm);

    // Destructor exists mainly to deallocate storage for name
    ~VrmlNodeType();

    // Namespace management functions.  PROTO definitions add node types
    // to the namespace.  PROTO implementations are a separate node
    // namespace, and require that any nested PROTOs NOT be available
    // outside the PROTO implementation.
    // addToNameSpace will print an error to stderr if the given type
    // is already defined.
    static void addToNameSpace(VrmlNodeType *);
    static void pushNameSpace();
    static void popNameSpace();
    static void deleteAllFromNameSpace();

    // Find a node type, given its name.  Returns NULL if type is not defined.
    static const VrmlNodeType *find(const char *nm);

    // Routines for adding/getting eventIns/Outs/fields
    void addEventIn(const char *name, int type);
    void addEventOut(const char *name, int type);
    void addField(const char *name, int type);
    void addExposedField(const char *name, int type);

    int hasEventIn(const char *name) const;
    int hasEventOut(const char *name) const;
    int hasField(const char *name) const;
    int hasExposedField(const char *name) const;
    
    const char *getName() const { return name; }

	vrmlField *getFieldDefault(int index) const;
	int getFieldIndex(const char *fieldname, int *type) const;
	int getFieldType(int index) const;
	const char *getFieldName(int index) const;
	int getNumFields() const;

        
  private:
    void add(CvectList<NameTypeRec*> &,const char *,int);
    int has(const CvectList<NameTypeRec*> &,const char *) const;

    char *name;

    // Node types are stored in this data structure:
    static CvectList<VrmlNodeType*> typeList;

    CvectList<NameTypeRec*> eventIns;
    CvectList<NameTypeRec*> eventOuts;
    CvectList<NameTypeRec*> fields;
};

// This is used to keep track of which field in which type of node is being
// parsed.  Field are nested (nodes are contained inside MFNode/SFNode fields)
// so a stack of these is needed:
class vrmlNode;
typedef struct {
	const VrmlNodeType *nodeType;
	const char *fieldName;
	vrmlNode *node;
} FieldRec;


#endif
