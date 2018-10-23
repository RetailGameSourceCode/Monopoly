#ifndef CAN_H
#define CAN_H

#ifdef __cplusplus
extern "C" {
#endif
#ifndef TRUE
#define TRUE 1
#define FALSE 0
	typedef int BOOL;
#endif
	
	
// .......... macros for dealing with chunk codes

	
#define MAKE_CHUNK_CODE(ChunkType, ChunkID) ((((int)ChunkType) << 16) | (ChunkID & 0x0000FFFF))
#define GET_CHUNK_ID(ChunkCode) (ChunkCode & 0xFFFF)
#define GET_CHUNK_TYPE(ChunkCode) (ChunkCode >> 16)
	
#define MAX_CHUNK_NAME_LEN 63 // a practical limit for length of a chunk name
#if ((MAX_CHUNK_NAME_LEN+1)%4) 
	#error "must be a multiple of 4 bytes"
#endif

// NOTE on child chunks...
// The implementation of nested chunks here is less flexible
// than in other systems to save space and the hassle of managing,
// reading, writing, & searching through chunks for the info you want.
// By definition, all child chunks occur after the chunk data.
// Since it is the chunk reader's responsibility to search through
// the chunks an discard anything it doesn't want to deal with, this
// definition will help the search, although it doesn't give you 
// the flexibility to define hierarchical data anywhere in the chunk.
// 	The main considerations here are the real-time loading speed
// and the space the chunks take, as well as the and amount of code
// required to parse the file.


// NOTE on chunk IDs and names...To save memory and reduce complexity of
// implementation, the chunk ID can be used as a 16 bit data ID for datafile
// access. if there is data inlined in the chunk, the data ID does not necessarily
// mean anything, but if there is no data, either the name or data ID
// is used to resolve the reference...Theoretically, there shouldn't be a need
// to have both a name and a data ID present, but there can be. The name is
// useful for individual file access. Of course, any individual chunk can
// always implement its own referencing conventions within itself.

	struct chunkInfo {
		
		// code for the type of chunk...
		// a code is a combination of chunk type and chunk ID
		int chunkCode; 		

		// zero indicates leaf data only, 
		//non-zero indicates the # of child chunks
		int numChildren;	

		// length of the chunk data in bytes, 
		//excluding children (0 indicates no data)
		int chunkDataLength; 

		// length of the chunk data in bytes, 
		//including children (0 indicates no data & children)
		int chunkLength; 	

		// always zero-padded to a 4-byte boundary, null indicates no name	
		char chunkName[MAX_CHUNK_NAME_LEN+1];
					
		};
	
	
	// .......... represents the byte layout of a chunk (for documentation only, not used in code)
	// struct chunk {
	//	int chunkCode 		// code for the type of chunk
	//	int numChildren		// zero indicates leaf data, non-zero indicates a child chunk
	//	int chunkDataLength; // length of the chunk data in bytes, excluding children (0 indicates no data)
	//	int chunkLength; 	// length of the chunk data in bytes, including children (0 indicates no data & children)
	//	char chunkName[] 	// always zero-padded to a 4-byte boundary, null indicates no name
	// 	byte chunkData[]	// the chunk data
	// 	{chunk *subChunks;} 	// the byte data includes all child chunks before the chunk's actual data
	// };
	
	
	
	// .......... types of actions to be performed by the mapper (see struct 'MapperAction' below)
	// .......... ( see canf.c for a sample implementation of a mapper for a disk file )
	enum CAN_MAPPER_ACTION {
		CAN_MAPPER_OPEN, 	// binds a canID to a data source, using either
			// 'filename' or dataID, and creates whatever 
			// internal buffers it needs for data
			
			CAN_MAPPER_MAP,  	// return a pointer to 'bytes' of data at 'offset'
			// in the data source. Must generate a run time error
			// if the requested data size is too big for its temp buffer(s)
			// Changes the 'bytes' field to reflect the actual
			// number of bytes read, (e.g. in case of end of file)
			// The data should remain valid until the next call to
			// any mapper action pertaining to the given canID
			// Advances the current stream position past the mapped data.
			
			CAN_MAPPER_READ, 	// copies at most 'bytes' of data from the current
			// source position to a user specified buffer, and
			// advances the current stream position. Changes the 'bytes' 
			// field to reflect the number of bytes actually read. If the 
			// user specified buffer 'dest' is NULL, the bytes are simply discarded
			// (i.e. it is effectively a seek operation, advancing the pointer)
			
			CAN_MAPPER_CLOSE,	// closes the data source bound to the canID
			// (mapper can retain temp buffers for future use, or destroy them)
			
	};
	
	
	struct MapperAction {
		enum CAN_MAPPER_ACTION action;
		int canID;
		int dataID;
		char *filename;
		void *dest;
		int offset;
		int bytes;
	};
	
	// .......... 	Callback to use for a given data source; 
	// .......... This makes the data source transparent to the CAN parsing
	// .......... mechanism and also allows stuff like an asynchronous streaming 
	// .......... source to be plugged in later on. The PSX implementation should 
	// .......... be especially aware of memory usage for temporary buffers.
	// .......... Should returns FALSE on error.
	typedef BOOL CAN_MemoryMapper(struct MapperAction *m);

  // These are the two available callbacks.
  BOOL CAN_FileMemoryMapper(struct MapperAction *m);
  BOOL CAN_DataIDMemoryMapper(struct MapperAction *m);
	
	
	// .......... chunky animation format (.CAN) load\save API
	
	// .......... LOADING API
	
  // Initializes an object for reading a .CAN file from the given memory
  // mapper.  Returns the 'canID' or -1 on failure.
  // Use dataMapper == CAN_FileMemoryMapper and specify filename to parse a
  // CAN from disk.  Use dataMapper == CAN_DataIDMemoryMapper and specifiy
  // dataID to parse a CAN from the datafile.
	int openCAN(char *filename, int dataID, CAN_MemoryMapper *dataMapper);
	
	// .......... destroys the CAN object, releasing internal buffers, etc.
	BOOL closeCAN(int canID);
	
	// .......... moves the current location to the given chunk name,
	// .......... or to the given chunk ID if name is NULL. 
	// .......... NOTE: depending on the mapper implementation,
	// .......... this may cause a lot of seeking. 
	// .......... (NOT YET IMPLEMENTED)
	BOOL setPosition(int canID, char *chunkName, short chunkID);
	
	// .......... moves the current location to the next sibling chunk
	// .......... return FALSE if there are no more sibling chunks
	BOOL nextChunk(int canID);

	// .......... similar to 'nextChunk', but skips chunks 
	// .......... that are not of type 'chunkType'
	BOOL nextChunkOfType(int canID, short chunkType);
	
	// .......... moves the current location to the first child chunk 
	// .......... of the current chunk. returns FALSE if there are no 
	// .......... child chunks. You must read or map any chunk data 
	// .......... before a 'descend', as the stream pointer is advanced
	// .......... past the chunk data to the child chunks.
	BOOL descend(int canID);
	
	// .......... moves the current location to the parent chunk 
	// .......... of the current chunk. returns FALSE if there is no 
	// .......... parent chunk. You cannot read or map chunk data after
	// .......... an 'ascend'...it must be followed by 'nextChunk'
	BOOL ascend(int canID);
	
	// .......... gets the header info for the current chunk. 
	BOOL getChunkHeader(int canID, struct chunkInfo *c);
	
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

	// .......... 	By default, you should pass the full chunk length obtained
	// .......... by 'getChunkHeader'. You may read the chunk data in smaller blocks
	// .......... if it is convenient. Successive calls to 'mapChunkHeader' will
	// .......... map the next 'dataLength' bytes. An error is generated if you
	// .......... read past the chunk data.

  //-------------------------------------
  // jeffp note:  Things are slightly different if you use CAN_DataIDMemoryMapper
  // instead.  With it, you can call mapChunkData all you want and the pointers
  // will remain valid right up until you close the file.  (Since the CAN structure
  // is in the datafile, the whole thing gets read into a block of memory when you
  // use it.)
  //-------------------------------------
  void *mapChunkData(int canID, int dataLength);
	
	// .......... 	Reads the current chunk's data up to a maximum of 'maxBytes'
	// .......... into the buffer provided by the client. This is an alternate
	// .......... way to access chunk data when you want a static copy of it
	// .......... or the data being read is too large (e.g a large bitmap)
	// .......... to be mapped into the data source's internal cache. In some cases,
	// .......... this may be more efficient because the data source has the option
	// .......... to read data directly into the client's buffer without using
	// .......... its own temporary buffer as an intermediate step.

	// .......... 	By default, you should pass the full chunk length obtained
	// .......... by 'getChunkHeader'. You may read the chunk data in smaller blocks
	// .......... if it is convenient. Successive calls to 'readChunkHeader' will
	// .......... read the next 'dataLength' bytes. An error is generated if you
	// .......... read past the chunk data.
	BOOL readChunkData(int canID, void *dest, int dataLength);	
	
#ifdef CAN_CREATE_API

	
	// .......... 	CREATE / WRITE API
	
	// .......... initializes an object for creating a .CAN file
	// .......... returns the CAN object id. The can object maintains
	// .......... an internal buffer to hold the .CAN data until it is written.	
	int createCAN(char *filename);
	
	// .......... Destroys the CAN object,and closes the file. Returns
	// .......... FALSE on an error. A common error is a bad file format,
	// .......... for example incomplete chunk caused by mismatched
	// .......... ascend/descend pairs.
	BOOL writeCAN(int canID);
	
	// .......... Creates a chunk at the current position. The chunk size, 
	// .......... children, and other data are initialized automatically
	// .......... when the chunk is closed. You must 'createChunk' before
	// .......... setting any chunk data.
	BOOL createChunk(int canID, short chunkType, short chunkID, char *chunkName);
	
	// .......... Sets the chunk data. You must set any chunk data before you
	// .......... descend (via 'descendChunk'). This guarantees that all child chunks
	// .......... occur after the parent chunk's data. You can make multiple
	// .......... sucesssive calls to 'setChunkData' if it is convenient.
	// .......... If you do not call 'setChunkData' for a given chunk, the
	// .......... data size is assumed to be zero (plus the sizes of any children)
	BOOL setChunkData(int canID, void *data, int len);
	
	// .......... moves the current location down a level to create child chunks
	// .......... Must be called after setting any chunk data 
	BOOL descendChunk(int canID);
	
	// .......... moves the current location up a level after creating child chunks
	// .......... 'ascendChunk' will resolve the chunk size, etc. including child chunks
	// .......... Must be followed by an 'ascendChunk', a 'createChunk' or 'writeCAN', 
	// .......... i.e. you cannot set chunk data at the new level after ascending
	// .......... (all chunk data comes before the child chunks).
	BOOL ascendChunk(int canID);
	
	
/* .......... sample usage...
	
// .......... create the can object
int canID = createCAN("BillAnim");

// bounding box for the hierarchical object
createChunk( canID, GEOMETRY_BOX, -1, "Bill" );
setChunkData(canID, box, sizeof(box));

	descendChunk( canID );
	createChunk( canID, GEOMETRY_HIERARCHY, -1, "BillBounds" );

		descendChunk(canID);

		// data for the frame transform
		createChunk(canID, FRAME_TRANSFORM, -1, 0 );
		setChunkData(canID, matrix, sizeof(vertex) * 4);

		createChunk(canID, GEOMETRY, -1, "pelvis" );

		// data for the GEOMETRY chunk
		setChunkData(canID, vertexlist, 40);
		setChunkData(canID, normallist, 40);
		setChunkData(canID, facelist, 65);
		setChunkData(canID, texturecoordinatelist, 65 * 3);

		// give the texture a chunk ID
		createChunk(canID, TEXTURE, 14, "skin" ); 
		setChunkData(canID, SkinTextureData16bpp, 256*256*2);

		// child frames
		createChunk( canID, GEOMETRY_HIERARCHY, -1, "Bill_Torso" );
			decendChunk(canID);
				.
				. // child geometry, etc.
				.
			ascendChunk(canID);
		ascendChunk(canID);
	ascendChunk(canID);
  
// .......... save & destroy the can object
writeCAN( canID );
			  
*/
	
	
#endif // CAN_CREATE_API
	
	
#ifdef __cplusplus
	}
#endif
	
#endif
