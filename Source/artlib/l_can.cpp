#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#include "l_can.h"


struct CANobj {
	
	BOOL inUse;
	CAN_MemoryMapper *dataSource;
	BOOL openForRead;
	int readOffset;
	int chunkLevel;
	BOOL currentChunkValid;
	int chunkDataOffset[32];
	int numChunksAtLevel[32];
	struct chunkInfo ci[32];
	
#ifdef CAN_CREATE_API
	
	FILE *can;
	char *writePointer;
	char *writeBuffer;
	int	buffersize;
	char	canName[256];
	int	chunkStack[32];
	short headersizeStack[32];
	
#endif
};

#define MAXCANS 1
int numCans;

struct CANobj cans[MAXCANS];


// .......... chunky animation format (.CAN) load\save API


// .......... LOADING API

// Initializes an object for reading a .CAN file from the given memory
// mapper.  Returns the 'canID' or -1 on failure.
// Use dataMapper == CAN_FileMemoryMapper and specify filename to parse a
// CAN from disk.  Use dataMapper == CAN_DataIDMemoryMapper and specifiy
// dataID to parse a CAN from the datafile.
int openCAN(char *filename, int dataID, CAN_MemoryMapper *dataMapper)
{
	int count=-1;
	static BOOL firstTime = TRUE;
	struct MapperAction m;
	
	if (firstTime)
	{
		memset( cans, 0, sizeof(struct CANobj) * MAXCANS);
		firstTime = FALSE;
	}
	
	// .......... find an available slot
	while (++count < MAXCANS)
	{
		if (!cans[count].inUse)
			break;
	};
	
	// .......... must find a slot
	assert (count != MAXCANS);

	// .......... reinitialize the slot
	memset( &cans[count], 0, sizeof(struct CANobj) );
		
	cans[count].inUse = TRUE;
	cans[count].openForRead = TRUE;
	cans[count].readOffset = 0;
	cans[count].chunkLevel = 0;
	cans[count].dataSource = dataMapper;
	
	// .......... call to open the data source
	m.action = CAN_MAPPER_OPEN;
	m.canID = count;
	m.dataID = dataID;
	m.filename = filename;
	if (! (*cans[count].dataSource)(&m) )
	{
		cans[count].inUse = FALSE;
		return -1;
	}
	
	return count;
}


// .......... destroys the CAN object, releasing internal buffers, etc.
BOOL closeCAN(int canID)
{
	int count = -1;
	struct MapperAction m;
	
	// .......... must be a valid slot
	assert (canID < MAXCANS && cans[canID].inUse && cans[canID].openForRead);
	
	// .......... call to close the data source
	m.action = CAN_MAPPER_CLOSE;
	m.canID = canID;
	if (! (*cans[canID].dataSource)(&m) )
		return FALSE;
	
	if (cans[ canID ].chunkLevel != 0)
		return FALSE; // some chunks are not properly nested
	
	cans[canID].inUse = FALSE;
	cans[canID].openForRead = FALSE;
	
	return TRUE;
}


// .......... moves the current location to the given chunk name,
// .......... or to the given chunk ID if name is NULL
BOOL setPosition(int canID, char *chunkName, short chunkID)
{
	assert(0); // not yet implemented...need to consider implications on current 'nextChunk' mechanism
	
	assert (canID < MAXCANS && cans[canID].inUse && cans[canID].openForRead);
	return TRUE;
}

BOOL nextChunkOfType(int canID, short chunkType)
{
  BOOL  Successful;
	struct chunkInfo c;
	while (1)
	{
		if ( !nextChunk( canID ) )
			return FALSE; // no more sibling chunks
		
		// stop if the current chunk is the right type, otherwise continue
		Successful = getChunkHeader( canID, &c );
		assert (Successful);
		if ( GET_CHUNK_TYPE( c.chunkCode ) == chunkType)
			break;
	};
	return TRUE;
}

// .......... moves the current location to the next sibling chunk
// .......... return FALSE if there are no more sibling chunks
BOOL nextChunk(int canID)
{
	int i=0,dataReadSoFarInChunk,remainingDataInChunk;
	char *name;
	struct MapperAction m;
	struct chunkInfo *c;
	assert (canID < MAXCANS && cans[canID].inUse && cans[canID].openForRead);
	
	// .......... if the number of chunks read so far is equal to the
	// .......... number of children of the parent chunk, can't read any
	// .......... more chunks at this level
	
	if (cans[canID].chunkLevel > 0 )
	{
		if (	cans[canID].numChunksAtLevel[cans[canID].chunkLevel] >=
			cans[canID].ci[cans[canID].chunkLevel - 1].numChildren )
		{
			cans[canID].currentChunkValid = FALSE;
			return FALSE;
		}
	}
	// .......... the top level has only one chunk, so if we've read any bytes at
	// .......... all, we've already advanced to the one and only top level chunk
	else if (cans[canID].readOffset != 0)
	{
		cans[canID].currentChunkValid = FALSE;
		return FALSE; 
	}

	if (cans[canID].readOffset != 0  && cans[canID].numChunksAtLevel[cans[canID].chunkLevel] != 0)
	{
		// .......... we're not at the beginning of the chunk,
		// .......... so advance past the unread bytes in the chunk data
		
		// .......... see how many bytes to skip to the next chunk
		dataReadSoFarInChunk = cans[canID].readOffset - cans[canID].chunkDataOffset[cans[canID].chunkLevel];
		assert (dataReadSoFarInChunk >= 0); // must be positive
		
		remainingDataInChunk = \
			cans[ canID ].ci[cans[canID].chunkLevel].chunkLength - dataReadSoFarInChunk;
		
		if (remainingDataInChunk)
		{
			m.action = CAN_MAPPER_READ;
			m.canID = canID;
			m.offset = cans[canID].readOffset;
			m.bytes = remainingDataInChunk; 	// read to the start of next chunk
			m.dest = 0;								// not interested in the data
			
			// read past the current chunk
			(*cans[canID].dataSource)(&m);
			cans[canID].readOffset += remainingDataInChunk;
		}
		
	}
	
	c = &cans[canID].ci[cans[canID].chunkLevel];
	
	// .......... request a data map corresponding to the minimum possible
	// .......... size of a chunk header, i.e. with the name field NULL.
	// .......... If the name field is not NULL, map another 4 bytes at a time
	// .......... until the name is read.
	m.action = CAN_MAPPER_MAP;
	m.canID = canID;
	m.offset = cans[canID].readOffset;
	m.bytes = sizeof(int) * 4 + sizeof(char) * 4; // 4 fields plus 4 bytes of the name field
	
	// .......... update the file position
	cans[canID].readOffset += m.bytes;
	
	// .......... and map the data for the header
	(*cans[canID].dataSource)(&m);
	
	c->chunkCode = *((int*) m.dest);
	c->numChildren = *(((int*) m.dest)+1);
	c->chunkDataLength = *(((int*) m.dest)+2);
	c->chunkLength = *(((int*) m.dest)+3);
	name = (char *) (((int*) m.dest) + 4);
	
	if ( (*(int*)name) == 0 )
		c->chunkName[0] = '\0'; // no name
	else
	{
		memcpy(c->chunkName,name,4);
		
		while (1)
		{
			i += 4; // the number of bytes read for the name
			assert( i <= MAX_CHUNK_NAME_LEN+1 ); // # bytes can't be more than allowed
			
			// if any char is zero, we've reached the end of the string
			if (  name[0] == 0 ||
				name[1] == 0 ||
				name[2] == 0 ||
				name[3] == 0 )
				break;
			
			m.action = CAN_MAPPER_MAP;
			m.canID = canID;
			m.offset = cans[canID].readOffset;
			m.bytes = 4; // 4 more bytes of the name
			
			// map the data
			(*cans[canID].dataSource)(&m);
			cans[canID].readOffset += 4;
			
			memcpy(c->chunkName+i,m.dest,4);
			name = (char *) m.dest;
		}
		
	}
	
	// .......... detect error with name read
	assert (strlen(c->chunkName) <= MAX_CHUNK_NAME_LEN);
	
	// .......... set the current chunk data offset (used for data read verification)
	cans[canID].chunkDataOffset [cans[canID].chunkLevel] = cans[canID].readOffset;
	
	cans[canID].numChunksAtLevel[cans[canID].chunkLevel]++;
	cans[canID].currentChunkValid = TRUE;
	return TRUE;
}

// .......... moves the current location to the first child chunk 
// .......... of the current chunk. returns FALSE if there are no 
// .......... child chunks.
BOOL descend(int canID)
{
	int dataReadSoFarInChunk,remainingDataInChunk;
	struct MapperAction m;
	
	assert (canID < MAXCANS && cans[canID].inUse && cans[canID].openForRead);
	if (!cans[canID].currentChunkValid)
		return FALSE;
	
	// there must be children to descend
	assert (cans[ canID ].ci[cans[canID].chunkLevel].numChildren);
	
	// advance past any unread chunk data
	dataReadSoFarInChunk = cans[canID].readOffset - cans[canID].chunkDataOffset[cans[canID].chunkLevel];
	remainingDataInChunk = \
		cans[ canID ].ci[cans[canID].chunkLevel].chunkDataLength - dataReadSoFarInChunk;
	
	assert (remainingDataInChunk >= 0); // detect an over-read of chunk data
	
	if (remainingDataInChunk)
	{
		m.action = CAN_MAPPER_READ;
		m.canID = canID;
		m.offset = cans[canID].readOffset;
		m.bytes = remainingDataInChunk; 	// read to the start of next chunk
		m.dest = 0;								// not interested in the data
		
		// read past the current chunk
		(*cans[canID].dataSource)(&m);
		cans[canID].readOffset += remainingDataInChunk;
	}
	
	cans[ canID ].chunkLevel++;
	
	cans[canID].numChunksAtLevel[cans[canID].chunkLevel]=0;
	cans[canID].currentChunkValid = FALSE;
	
	return TRUE;
}


// .......... moves the current location to the parent chunk 
// .......... of the current chunk. returns FALSE if there is no 
// .......... parent chunk.
BOOL ascend(int canID)
{
	assert (canID < MAXCANS && cans[canID].inUse && cans[canID].openForRead);
	
	cans[ canID ].chunkLevel--;
	assert(cans[ canID ].chunkLevel >= 0); // detect an unmatched 'ascend' call
	
	
	return TRUE;
}


// .......... gets the header info for the current chunk
BOOL getChunkHeader(int canID, struct chunkInfo *c)
{
	assert (canID < MAXCANS && cans[canID].inUse && cans[canID].openForRead);
	
	// .......... make sure this 'chunkInfo' is valid
	assert (cans[ canID ].currentChunkValid);
	
	// .......... and copy the info
	*c = cans[ canID ].ci[cans[canID].chunkLevel];
	return TRUE;
}




// .......... 	Returns a pointer to the current chunk's data...
// .......... this is provided so the data source can provide the
// .......... chunk data without forcing an extra copy. The data is
// .......... guaranteed valid ONLY until the next call to any
// .......... .CAN -related function in this header. Note that if
// .......... the data is too large, e.g. a full screen bitmap on the PSX,
// .......... the call may fail (depending on the size of the data source's
// .......... mapping buffer). In these cases it is usually appropriate to
// .......... read the data directly into the area of memory where it will
// .......... be used (especially considering the PSX's limited memory).
void *mapChunkData(int canID, int dataLength)
{
	int bytesToMap;
	struct MapperAction m;
	assert (canID < MAXCANS && cans[canID].inUse && cans[canID].openForRead);
	
	m.action = CAN_MAPPER_MAP;
	m.canID = canID;
	m.offset = cans[canID].readOffset;
	m.bytes = bytesToMap = dataLength;
	
	// .......... make sure the chunk data is not being over-read
	assert ( m.bytes <= cans[canID].ci[cans[canID].chunkLevel].chunkDataLength - \
		(cans[canID].readOffset - cans[canID].chunkDataOffset[cans[canID].chunkLevel]) );
	
	// .......... and map the data for the chunk
	(*cans[canID].dataSource)(&m);
	
	assert (bytesToMap == m.bytes); // make sure the num bytes is correct
	
	// .......... update the file position
	cans[canID].readOffset += m.bytes;
	
	return m.dest;
}


// .......... 	Reads the current chunk's data up to a maximum of 'maxBytes'
// .......... into the buffer provided by the client. This is an alternate
// .......... way to access chunk data when you want a static copy of it
// .......... or the data being read is too large (e.g a large bitmap)
// .......... to be mapped into the data source's internal cache. In some cases,
// .......... this may be more efficient because the data source has the option
// .......... to read data directly into the client's buffer without using
// .......... its own temporary buffer as an intermediate step.
BOOL readChunkData(int canID, void *dest, int dataLength)
{
	int bytesToRead;
	struct MapperAction m;
	assert (canID < MAXCANS && cans[canID].inUse && cans[canID].openForRead);
	
	m.action = CAN_MAPPER_READ;
	m.canID = canID;
	m.offset = cans[canID].readOffset;
	m.bytes = bytesToRead = dataLength;
	m.dest = dest;
	
	// .......... make sure the chunk data is not being over-read
	assert ( m.bytes <= cans[canID].ci[cans[canID].chunkLevel].chunkDataLength - \
		(cans[canID].readOffset - cans[canID].chunkDataOffset[cans[canID].chunkLevel]) );
	
	// .......... and read the data for the chunk
	(*cans[canID].dataSource)(&m);
	
	assert(bytesToRead == m.bytes); // make sure the num bytes is correct
	
	// .......... update the file position
	cans[canID].readOffset += m.bytes;
	
	return TRUE;
}





#ifdef CAN_CREATE_API

#include <stdio.h>
#include <stdlib.h>

#include "cantyp.h" // basic .can chunk types


// .......... 	CREATE / WRITE API
#define DEF_BUFFER_SIZE (1024)

// .......... PRIVATE FUNCTION
void increaseBufferSize (int canID, int newSize)
{
	int offset;
	char *temp;
	if (cans[ canID ].buffersize >= newSize)
		return;
	temp = malloc(newSize);
	assert (temp);
	
	offset = cans[ canID ].writePointer - cans[ canID ].writeBuffer;
	
	if (cans[ canID ].writeBuffer)
	{
		memcpy( temp, cans[ canID ].writeBuffer, cans[ canID ].buffersize);
		free( cans[ canID ].writeBuffer );
	}
	
	cans[ canID ].buffersize = newSize;
	cans[ canID ].writeBuffer = temp;
	cans[ canID ].writePointer = cans[ canID ].writeBuffer + offset;
}

// .......... PRIVATE FUNCTION
// .......... reads chunk info from a buffer,
// .......... returns a pointer to the next chunk,
// .......... and sets a pointer to the chunk data if desired
char *readChunkInfo( char *chunkHeader, struct chunkInfo *c, char **chunkData )
{
#ifdef PSX
	assert(0); this code needs to be altered for PSX
#endif
		
		c->chunkCode = *(int *) chunkHeader;
	chunkHeader += sizeof(int);
	c->numChildren = *(int *) chunkHeader;
	chunkHeader += sizeof(int);
	c->chunkDataLength = *(int *) chunkHeader;
	chunkHeader += sizeof(int);
	c->chunkLength = *(int *) chunkHeader;
	chunkHeader += sizeof(int);
	
	// .......... 	make sure the chunk name length is OK, and copy it
	assert(strlen(chunkHeader) <= MAX_CHUNK_NAME_LEN);
	strcpy(c->chunkName, chunkHeader);
	
	if ( *((int *) chunkHeader) != 0 ) 
	{
		if (strlen(c->chunkName) % 4)
			chunkHeader += ((strlen(chunkHeader) + 3) / 4) * 4;
		else
			chunkHeader += strlen(c->chunkName) + 4;
	}
	else
		chunkHeader += 4;
	
	if (chunkData)
		*chunkData = chunkHeader;
	
	return chunkHeader + c->chunkLength;
}


// .......... initializes an object for creating a .CAN file
// .......... returns the CAN object id. The can object maintains
// .......... an internal buffer to hold the .CAN data until it is written.	
int createCAN(char *filename)
{
	int count = -1;
	char *extension;
	static BOOL firstTime = TRUE;
	
	if (firstTime)
	{
		memset( cans, 0, sizeof(struct CANobj) * MAXCANS);
		firstTime = FALSE;
	}
	
	// .......... find an available slot
	while (++count < MAXCANS)
	{
		if (!cans[count].inUse)
			break;
	};
	
	// .......... must find a slot
	assert (count != MAXCANS);
	
	// .......... make sure data is reset
	memset(&cans[count], 0, sizeof(struct CANobj));
	
	cans[count].inUse = TRUE;
	cans[count].can = fopen(filename, "wb");
	if (!cans[count].can)
		return -1;
	
	increaseBufferSize( count, DEF_BUFFER_SIZE );
	
	// .......... to name the .can,  get rid of the filename extension
	strcpy(cans[count].canName, filename);
	if ( (extension = strstr(cans[count].canName,".")) )
		*extension = '\0';
	
	// .......... create a default chunk to identify the .can file
	createChunk(count, CAN, 0, cans[count].canName);
	descendChunk(count);
	return count;
}




// .......... Destroys the CAN object,and closes the file. Returns
// .......... FALSE on an error. A common error is a bad file format,
// .......... for example incomplete chunk caused by mismatched
// .......... ascend/descend pairs.
BOOL writeCAN(int canID)
{
	int count,written;
	
	assert (canID < MAXCANS && cans[canID].inUse);
	
	ascendChunk(canID);
	
	if (cans[ canID ].chunkLevel != 0)
		return FALSE; // some chunks are not properly nested
	
	cans[canID].inUse = FALSE;
	cans[canID].openForRead = FALSE;
	
	count = cans[ canID ].writePointer - cans[ canID ].writeBuffer;
	written = fwrite( cans[ canID ].writeBuffer, 1, count, cans[ canID ].can );
	
	if (cans[ canID ].writeBuffer)
		free( cans[ canID ].writeBuffer );
	
	if (fclose(cans[ canID ].can))
		return FALSE;
	
	return (written == count) ? TRUE : FALSE;
}


// .......... Creates a chunk at the current position. The chunk size, 
// .......... children, and other data are initialized automatically
// .......... when the chunk is closed.
BOOL createChunk(int canID, short chunkType, short chunkID, char *chunkName)
{
	int chunkCode, sizeName, headersize=0;
	char *parentChunkHeader;
	int *chunkNumChildren;
	int remaining;

	assert (canID < MAXCANS && cans[canID].inUse && !cans[canID].openForRead);
	
	// .......... we impose a maximum allowable chunk name length for practicality 
	if (chunkName)
		assert (strlen(chunkName) <= MAX_CHUNK_NAME_LEN);
	
	// .......... calc the size of the chunk header
	headersize += sizeof(int); // chunk code
	headersize += sizeof(int); // num children
	headersize += sizeof(int); // chunk data length
	headersize += sizeof(int); // chunk length
	
	// chunk name...pad to a 4 byte boundary
	sizeName = 4;
	if (chunkName)
	{
		if (strlen(chunkName) % 4)
			sizeName = ((strlen(chunkName) + 3) / 4) * 4;
		else
			sizeName = strlen(chunkName) + 4;
	}
	
	headersize += sizeName;
	
	// .......... if there's not enough space in the buffer, resize it
	// .......... if there's not enough space in the buffer, resize it
	remaining = cans[canID].buffersize - ((int) (cans[canID].writePointer - cans[canID].writeBuffer));
	if (headersize > remaining)
		{
		int newSize = cans[canID].buffersize * 2; // ideally resize to double
		if (newSize < cans[canID].buffersize + headersize - remaining)
			newSize = cans[canID].buffersize + headersize - remaining;
		increaseBufferSize(canID, newSize);
		}

	// .......... store the relative location & header size of the chunk on our stack
	cans[ canID ].headersizeStack[ cans[ canID ].chunkLevel ] = headersize;
	cans[ canID ].chunkStack[ cans[ canID ].chunkLevel ] =
		cans[ canID ].writePointer - cans[ canID ].writeBuffer;
	
	// .......... write the chunk type / ID
	chunkCode = MAKE_CHUNK_CODE( chunkType, chunkID );
	memcpy( cans[ canID ].writePointer, &chunkCode, sizeof(int) );
	cans[ canID ].writePointer += sizeof(int);
	
	// .......... write the num children as zero (initially)
	memset( cans[ canID ].writePointer, 0, sizeof(int));
	cans[ canID ].writePointer += sizeof(int);
	
	// .......... write the chunk data length as zero (initially)
	memset( cans[ canID ].writePointer, 0, sizeof(int) );
	cans[ canID ].writePointer += sizeof(int);
	
	// .......... write the chunk length as zero (initially)
	memset( cans[ canID ].writePointer, 0, sizeof(int) );
	cans[ canID ].writePointer += sizeof(int);
	
	// .......... write the chunk name
	memset( cans[ canID ].writePointer, 0, sizeName ); // to ensure padding
	if (chunkName)
		strcpy( (char *) cans[ canID ].writePointer, chunkName );
	
	cans[ canID ].writePointer += sizeName;
	
	
	// .......... if there is a parent chunk, increment its child count 
	if( cans[ canID ].chunkLevel != 0)
	{
		// .......... advance to the chunk parent count
		parentChunkHeader = cans[ canID ].writeBuffer +
			cans[ canID ].chunkStack[ cans[ canID ].chunkLevel - 1 ];
		
		parentChunkHeader += sizeof(int); // bypass chunk code
		chunkNumChildren = (int *) parentChunkHeader;
		(*chunkNumChildren)++;
	}
	
	return TRUE;
}


// .......... Sets the chunk data. You must set chunk data before
// .......... descending (via 'descendChunk') This guarantees that all child chunks
// .......... occur after the parent chunk's data. You can make multiple
// .......... sucessive calls to 'setChunkData' if it is convenient.
// .......... If you do not call 'setChunkData' for a given chunk, the
// .......... data size is assumed to be zero (not including any children)
BOOL setChunkData(int canID, void *data, int len)
{
	char *chunkHeader;
	int *chunkLen,*chunkDataLen;
	int remaining;

	assert (canID < MAXCANS && cans[ canID ].inUse && !cans[ canID ].openForRead);
	
	// .......... must have created a chunk header before setting chunk data
	assert (cans[ canID ].headersizeStack[ cans[ canID ].chunkLevel ] != 0);
	
	// .......... if there's not enough space in the buffer, resize it
	remaining = cans[canID].buffersize - (int) (cans[canID].writePointer - cans[canID].writeBuffer);
	if (len > remaining)
		{
		int newSize = cans[canID].buffersize * 2; // ideally resize to double
		if (newSize < cans[canID].buffersize + len - remaining)
			newSize = cans[canID].buffersize + len - remaining;
		increaseBufferSize(canID, newSize);
		}

	// .......... copy the data
	memcpy( cans[ canID ].writePointer, data, len );
	cans[ canID ].writePointer += len;
	
	// .......... and adjust the chunk header's info
	chunkHeader = cans[ canID ].writeBuffer +
		cans[ canID ].chunkStack[ cans[ canID ].chunkLevel ];
	
	// .......... point to the chunk length
	chunkHeader += sizeof(int); // bypass chunk code
	
	// .......... there can be no children if we're still setting chunk data
	assert ( *((int*) chunkHeader) == 0 ); 
	
	chunkHeader += sizeof(int); // bypass num children
	chunkDataLen = (int *) chunkHeader;
	*chunkDataLen += len; // update data length
	
	chunkHeader += sizeof(int); // bypass chunk Length
	chunkLen = (int *) chunkHeader;
	*chunkLen += len; // update entire chunk length
	
	return TRUE;
}


// .......... moves the current location down a level to create child chunks
BOOL descendChunk(int canID)
{
	assert (canID < MAXCANS && cans[ canID ].inUse && !cans[ canID ].openForRead);
	
	cans[ canID ].chunkLevel++;
	cans[ canID ].headersizeStack[ cans[ canID ].chunkLevel ] = 0; // no header created
	
	return TRUE;
}


// .......... moves the current location up a level after creating child chunks
BOOL ascendChunk(int canID)
{
	char *chunkHeader,*chunkHeaderLast;
	char *chunkData;
	int *chunkDataLen;
	int *chunkLen;
	int childCount=0,headerLen,*chunkNumChildren;
	struct chunkInfo c;
	
	assert (canID < MAXCANS && cans[ canID ].inUse && !cans[ canID ].openForRead);
	
	cans[ canID ].chunkLevel--;
	assert(cans[ canID ].chunkLevel >= 0); // detect an unmatched 'ascend' call
	
	// .......... resolve the chunk size, based on its child chunks
	
	chunkHeader = cans[ canID ].writeBuffer +
		cans[ canID ].chunkStack[ cans[ canID ].chunkLevel ];
	
	// .......... find the chunk data
	chunkData = chunkHeader +
		cans[ canID ].headersizeStack[ cans[ canID ].chunkLevel ];
	
	// .......... get the number of children
	chunkHeader += sizeof(int); // bypass chunk code
	chunkNumChildren = (int *) chunkHeader;
	chunkHeader += sizeof(int); // bypass num children
	chunkDataLen = (int *) chunkHeader;
	chunkHeader += sizeof(int); // bypass chunk data length
	chunkLen = (int *) chunkHeader;
	
	// .......... there can be no child chunks at this point
	assert ( *chunkLen == *chunkDataLen );
	
	
	// .......... point to child chunks
	chunkHeader = chunkData;
	chunkHeader += *chunkDataLen;
	
	// .......... add the sizes of the child chunks to get the size of this chunk
	// .......... NOTE: the size of a chunk includes all the data for the child chunks,
	// .......... including their headers, but doesn't include this chunk's header.
	while (childCount++ < *chunkNumChildren)
	{
		chunkHeaderLast = chunkHeader;
		
		// detect an abnormal condition (chunk header outside allocated buffer)
		// probably caused by bad data / overwrites, etc.
		assert(	chunkHeader >= cans[ canID ].writeBuffer &&
			chunkHeader < cans[ canID ].writeBuffer + cans[ canID ].buffersize );
		
		// can call this safely because the child chunk is already resolved
		chunkHeader = readChunkInfo( chunkHeader, &c, &chunkData );
		headerLen = chunkData - chunkHeaderLast;
		
		// add the child length & child header size
		*chunkLen += c.chunkLength;
		*chunkLen += headerLen;
	}
	
	return TRUE;
}


#endif
