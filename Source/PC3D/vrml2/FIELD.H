#ifndef FIELD_H
#define FIELD_H

#include "vectlist.h"
class vrmlNode;

struct type_sfVec2f {
float x,y;
};
struct type_sfVec3f {
float x,y,z;
};
struct type_rotation {
float x,y,z,ang;
};
struct type_sfColor {
float r,g,b;
};

// .......... to satisfy the list template's comparison operator requirement
inline BOOL operator ==(type_rotation &r1, type_rotation &r2) {return r1.x == r2.x && r1.y == r2.y && r1.z == r2.z && r1.ang == r2.ang;}
inline BOOL operator ==(type_sfVec2f &v1, type_sfVec2f &v2) {return v1.x == v2.x && v1.y == v2.y;}
inline BOOL operator ==(type_sfVec3f &v1, type_sfVec3f &v2) {return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;}
inline BOOL operator ==(type_sfColor &c1, type_sfColor &c2) {return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;}


typedef char *	 	SFString;
typedef char * 	SFImage; 
typedef int 		SFInt32;
typedef type_sfVec2f 	SFVec2f;
typedef type_sfVec3f 	SFVec3f;
typedef float 		SFFloat;
typedef float 		SFTime; 
typedef BOOL 		SFBool;
typedef type_sfColor	SFColor; 
typedef type_rotation 	SFRotation;
typedef vrmlNode 	SFNode;

#define listType CvectList

typedef listType< SFString > 	MFString;
typedef listType< SFInt32 > 	MFInt32;
typedef listType< SFVec2f > 	MFVec2f;
typedef listType< SFVec3f > 	MFVec3f;
typedef listType< SFFloat > 	MFFloat;
typedef listType< SFTime > 	MFTime; 
typedef listType< SFBool > 	MFBool;
typedef listType< SFColor > 	MFColor; 
typedef listType< SFRotation >	MFRotation;
typedef listType< SFNode *> 	MFNode; 



union vrmlField {

// .......... single valued fields
	SFString 	sfString; 
	SFInt32 	Int32;
	SFVec2f		Vec2f;
	SFVec3f		Vec3f;
	SFFloat		Float;
	SFTime		Time; 
	SFBool		Bool;
	SFColor		Color; 
	SFRotation	Rotation;
	SFImage		Image;

// .......... NOTE: the field structure does not own the SFNode...
// .......... the node is part of the scene graph just like other nodes. 
// .......... It is referenced here to allow getting the SFNode field
// .......... without searching through a node's children.
	SFNode		*sfNode; 
	MFNode		*mfNode; 

// .......... multiple valued fields
	MFString	*mfString;
	MFInt32		*mfInt32;
	MFVec2f		*mfVec2f;
	MFVec3f		*mfVec3f;
	MFFloat		*mfFloat;
	MFTime 		*mfTime; 
	MFBool		*mfBool;
	MFColor	 	*mfColor; 
	MFRotation	*mfRotation;

};


// .......... to satisfy the list template's comparison operator requirement
BOOL operator ==(vrmlField &v1, vrmlField &v2);

#endif

