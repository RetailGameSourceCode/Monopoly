// //////////////////////////////////////////////////
// MODULE: tag.cpp              
// Contents: string constant management
// ///////////////// END OF HEADER //////////////////
#include "PC3DHdr.h"

#include "tag.h"


/*
void Tag::operator =(Tag &t) const
	{
	strcpy(name,t.name);
	strcpy(ext,t.ext);
	path = t.path;
	}
*/

int Tag::operator ==(Tag &t) const
	{
	int equal = !stricmp(name,t.name);
	if (equal)
		equal = !stricmp(ext,t.ext);
	if (equal)
		equal = (path == t.path);
	return equal;
	}


void splitPathAndName(const char *fullpath,char *filename,char *pathstr)
	{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];

	assert(fullpath);
	_splitpath(fullpath,drive,dir,filename,0);

	if (pathstr)
		{
		strcpy(pathstr,drive);
		strcat(pathstr,dir);
		}
	}

int Tag::pathCount=0;


char Tag::paths[MAX_PATHS][MAX_PATH_LEN+1];

void Tag::getFullName(char *str) const
	{
	str[0] = '\0';
	if (path != -1)
		strcpy(str,paths[path]);
	strcat(str,name);
	if (ext[0])
		{
		strcat(str,".");
		strcat(str,ext);
		}
	}


Tag::Tag(const char *dataname)
{
	if ( !dataname )
	{
		init( 0, 0 );
		return; 
	}

	// must fit in the temp array
	assert ( strlen( dataname ) < MAX_PATH_LEN+1+_MAX_PATH+MAX_TAG_CHARS+1+_MAX_FNAME );

	// copy to temp array
	char temp[ MAX_PATH_LEN+1+_MAX_PATH+MAX_TAG_CHARS+1+_MAX_FNAME ];
	strcpy( temp, dataname );

	//	find the last occurence of the '.' extension character to get a possible extension in the name
	char *ext = strrchr( temp, '.');
	if (ext)
	{
		*ext = '\0';
		init( temp, ext+1 );
	}
	else
		init( temp, "" );
}

Tag::Tag(const char *dataname, const char *dataext)
	{
	init( dataname, dataext );
	}

void Tag::init(const char *dataname, const char *dataext)
	{
	name[0]='\0';
	ext[0]='\0';
	path = -1; // indicates no path (or current directory)
	
	if (dataname)
		{
		// extract the path first
		char pathstr[MAX_PATH_LEN+1+_MAX_PATH];
		char filename[MAX_TAG_CHARS+1+_MAX_FNAME]; // silly splitpath func causes stack overflow even if the required filename size is correct
		splitPathAndName(dataname,filename,pathstr);

		assert (filename); // must find a valid file name after the path

		if (pathstr[0]) // if there's a non null path
		{
			int i = -1;
			while (++i < pathCount)
				{
				// .......... does path string exists
				if (!stricmp(paths[i], pathstr))
					break;
				};
			path = i;

			// otherwise add it
			if (i == pathCount)
				{
				assert (i < MAX_PATHS); // can't add more than MAX_PATHS
				strcpy(paths[pathCount++], pathstr);
				}
		 }

			// finally copy the actual data name 
			strncpy(name,filename,MAX_TAG_CHARS);
			name[MAX_TAG_CHARS] = '\0';
		}

	if (dataext)
		{
		strncpy(ext,dataext,3);
		ext[3] = '\0';
		}
	}


Tag::Tag(const Tag& t)
	{
       strcpy(name, t.name);
       strcpy(ext, t.ext);
	path = t.path;
	}


