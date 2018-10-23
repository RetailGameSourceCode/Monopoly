#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include "l_can.h"

// .......... a memory mapper implementation for a disk data source

struct filemap {
	BOOL inUse;
	int canID;
	int filePos;
	void *buffer;
	FILE *open;
};

// make this smaller if a 64K buffer is too big for the PSX
#define BUFFER_SIZE (1024 * 64) 
#define MAXMAPS 4
struct filemap maps[MAXMAPS];


BOOL fmmOpen(struct MapperAction *m);
BOOL fmmClose(struct MapperAction *m);
BOOL fmmRead(struct MapperAction *m);
BOOL fmmMap(struct MapperAction *m); 


BOOL CAN_FileMemoryMapper(struct MapperAction *m)
{
static BOOL firstTime = TRUE;
if (firstTime)
	{
	memset( maps, sizeof(struct filemap) * MAXMAPS, 0);
	firstTime = FALSE;
	}

switch (m->action)
	{
	case CAN_MAPPER_OPEN:
		return fmmOpen( m );
	case CAN_MAPPER_MAP:
		return fmmMap( m );
	case CAN_MAPPER_READ:
		return fmmRead( m );
	case CAN_MAPPER_CLOSE:
		return fmmClose( m );
	default:
		assert(0); // bad action
	};

return FALSE;
}




BOOL fmmOpen(struct MapperAction *m)
{
int count=-1;

// .......... find an available slot
while (++count < MAXMAPS)
	{
	if (!maps[count].inUse)
		break;
	};

// .......... must find a slot
assert (count != MAXMAPS);

// .......... open the file
assert (m->filename); // must be a disk file
maps[count].open = fopen(m->filename,"rb");
if (!maps[count].open )
	return FALSE;

maps[count].inUse = TRUE;
maps[count].canID = m->canID;

// .......... allocate a temp buffer for mapping
maps[count].buffer = malloc(BUFFER_SIZE);

assert (maps[count].buffer); // must have enough memory
return TRUE;
}


BOOL fmmClose(struct MapperAction *m)
{
int count=-1;

// .......... find the slot
while (++count < MAXMAPS)
	if (maps[count].canID == m->canID)
		break;

assert(count != MAXMAPS || !maps[count].inUse); // not found or not in use

assert(!fclose( maps[count].open ) );

maps[count].inUse = FALSE;

assert(maps[count].buffer);
free(maps[count].buffer);
return TRUE;
}

// .......... read a number of bytes from the file into the destination provided
BOOL fmmRead(struct MapperAction *m)
{
int count=-1,bytes;

// .......... find the slot
while (++count < MAXMAPS)
	if (maps[count].canID == m->canID)
		break;

assert(count != MAXMAPS || !maps[count].inUse); // not found or not in use

// .......... just read straight into the mem area, avoiding the temp buffer
if (m->dest)
	bytes = fread(m->dest, 1, m->bytes, maps[count].open);
else
	{
	// dest is NULL, so we'll just seek past the data
	assert(!fseek(maps[count].open,m->bytes,SEEK_CUR));
	bytes = m->bytes; // fake the # bytes read
	}

maps[count].filePos = ftell( maps[count].open );

if (!bytes || maps[count].filePos == -1)
	return FALSE;

m->bytes = bytes;
return TRUE;
}

// .......... map the requested window into the temp buffer
BOOL fmmMap(struct MapperAction *m)
{
int count=-1,bytes;

// .......... find the slot
while (++count < MAXMAPS)
	if (maps[count].canID == m->canID)
		break;

assert(count != MAXMAPS || !maps[count].inUse); // not found or not in use

// .......... don't seek if the current position is the one we want
if (maps[count].filePos != m->offset)
{
	if ( fseek(maps[count].open,m->offset,SEEK_SET) )
		return FALSE;
}
assert (m->bytes <= BUFFER_SIZE); // too big for the buffer

// .......... simply read to the top of the buffer. A more sophisticated
// .......... mapper might try to fit the new data into the buffer to
// .......... retain previous data (in case of a read request to previously
// .......... read data).
bytes = fread( maps[count].buffer, 1, m->bytes, maps[count].open );
maps[count].filePos = ftell( maps[count].open );
assert (maps[count].filePos != -1); // error occured on file position query

if (!bytes)	return FALSE;

m->bytes = bytes;
m->dest = maps[count].buffer;
return TRUE;
}
