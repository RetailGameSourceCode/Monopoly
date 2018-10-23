#ifndef TAG_H
#define TAG_H

// .......... a type used to reference data objects

// syntax:     Tag("model","f3d") OR
//             Tag("modelpath\\standard\\model","f3d") OR
//             Tag("modelpath\\standard\\model.f3d") OR
//             Tag()

#ifdef WIN32
	const MAX_PATHS=8;
	#define MAX_PATH_LEN 63
#else
// .......... potentially save some memory on a console if needed
	const MAX_PATHS=8;
	#define MAX_PATH_LEN 63
#endif

#define MAX_TAG_CHARS 15
#if ((MAX_TAG_CHARS+1) % 8) != 0
	#error "size of the tag should be a multiple of 8 bytes, to try and avoid strange Pentium cache alignment issues"
#endif
#if ((MAX_PATH_LEN+1) % 8) != 0
	#error "size of the paths should be a nice number, for memory efficiency
#endif

class Tag {

       char 	name[MAX_TAG_CHARS+1];
       char 	ext[4];
	int 	path;

	static int pathCount;
	static char paths[MAX_PATHS][MAX_PATH_LEN+1];

	void init(const char *dataname, const char *dataext);

public:
		Tag(const Tag& t);
		Tag(const char *name = 0);
		Tag(const char *dataname, const char *dataext);
       	operator char *() const {return (char *) name;}
		char *getExtension() const {return (char *) ext;}
		void getFullName(char *str) const;

		//void operator =(Tag &t) const;
		int operator ==(Tag &t) const;

friend class datafile;
};/* end class Tag */


#endif
