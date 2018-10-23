/*****************************************************************************
 * FILE:        L_Chunk.cpp
 *
 * DESCRIPTION: Chunky file format support routines.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Chunk.cpp 114   99-10-19 4:19p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Chunk.cpp $
 * 
 * 114   99-10-19 4:19p Agmsmith
 * Adding the delete key tweeker feature.
 *
 * 113   99/09/10 12:44p Agmsmith
 * Return a flag when getting interpolated data telling you if you are on
 * a keyframe.
 *
 * 112   8/09/99 5:34p Agmsmith
 * Added time to for-all-chunks function, needed for dope sheet select
 * next sequence feature.
 *
 * 111   8/03/99 1:48p Agmsmith
 * Allow getting initial position attributes for any time.
 *
 * 110   7/28/99 2:46p Agmsmith
 * Moved possible data chunk pointers from chunky module to sequencer
 * header.
 *
 * 109   7/26/99 12:25p Agmsmith
 * When modifying sequence data, check for attempts to modify
 * inappropriate data (like field of view on non-camera sequences) and
 * ignore them.
 *
 * 108   7/23/99 1:37p Agmsmith
 * Fixed bug with timeframe for getting interpolated values.
 *
 * 107   7/23/99 12:23p Agmsmith
 * Don't add tweekers before the start of the sequence since that causes
 * mangled chunks.
 *
 * 106   7/20/99 1:42p Agmsmith
 * Added dimensionality subchunk to CreateSequence grouping chunk.
 *
 * 105   7/16/99 4:36p Agmsmith
 * More fiddling.
 *
 * 104   7/16/99 4:31p Agmsmith
 * Change duration now has depth level.
 *
 * 103   7/16/99 2:28p Agmsmith
 * Fixed bug in resizing sequence durations, non-groups also need to be
 * processed.
 *
 * 102   7/15/99 3:22p Agmsmith
 * Add LE_CHUNK_FindOneSelected series of functions.
 *
 * 101   7/07/99 5:31p Agmsmith
 * Have the resize sequence routine tell you if it changed the duration of
 * group chunks to reflect their child durations.
 *
 * 100   6/09/99 7:03p Agmsmith
 * Don't ask for DataID contents for spooled sounds, which have their data
 * in an external file.
 *
 * 99    6/04/99 6:14p Agmsmith
 * When creating data chunks, check if the parent is meaningful (so no
 * volume or pitch data for non-sound parents).
 *
 * 98    6/04/99 1:00p Agmsmith
 * Adding field of view private chunk and getting cameras to work.
 *
 * 97    5/26/99 3:47p Agmsmith
 * Updated for new camera chunk fields.
 *
 * 96    5/26/99 3:02p Agmsmith
 * Don't dump dope sheet chunks when in symbolic mode.
 *
 * 95    5/18/99 1:34p Agmsmith
 *
 * 94    5/09/99 4:32p Agmsmith
 * Create an interpolated data value when adding new key values.
 *
 * 93    5/09/99 12:58p Agmsmith
 * Moved mesh proportion out of the tweeker chunk and into the mesh choice
 * chunk.
 *
 * 92    5/07/99 2:21p Agmsmith
 * Fixed bug where sorting chunks had zero to do.
 *
 * 91    5/03/99 1:33p Agmsmith
 *
 * 90    5/02/99 4:03p Agmsmith
 * Added a function for changing the duration of sequences.
 *
 * 89    4/26/99 12:21p Agmsmith
 * Fixed a bug with Y scaling.
 *
 * 88    4/23/99 5:00p Agmsmith
 * Now with interpolation when getting value of data.
 *
 * 87    4/23/99 10:58a Agmsmith
 * Working on getting interpolated data.
 *
 * 86    4/20/99 4:30p Agmsmith
 * Origin display under construction.
 *
 * 85    4/19/99 7:36p Agmsmith
 * Tweeker editing now works mostly properly.
 *
 * 84    4/19/99 12:43p Agmsmith
 * Private chunk numbering order changed.
 *
 * 83    4/18/99 4:57p Agmsmith
 *
 * 82    4/18/99 4:20p Agmsmith
 * Changed offset/scale/rotate to origin/scale/rotate/offset chunk.
 *
 * 81    4/17/99 5:18p Agmsmith
 * Tweeker splitting and movement partially working now.  Needs promote
 * and demote functions to help with transfering data between adjacent
 * tweekers.
 *
 * 80    4/17/99 3:31p Agmsmith
 * Under construction.
 *
 * 79    4/16/99 4:48p Agmsmith
 * Adjusting tweekers under construction.
 *
 * 78    4/15/99 12:03p Agmsmith
 *
 * 77    4/14/99 7:11p Agmsmith
 * Don't resize zero size DataIDs when writing chunks.
 *
 * 76    4/12/99 1:44p Agmsmith
 *
 * 75    4/08/99 11:34a Agmsmith
 * Oops.
 *
 * 74    4/07/99 6:38p Agmsmith
 * Yet more chunk modifying routines.
 *
 * 73    4/05/99 6:05p Agmsmith
 * Can also modify tweeker proportion.
 *
 * 72    4/04/99 5:06p Agmsmith
 * Modify other kinds of data subchunks in sequences.
 *
 * 71    4/04/99 4:01p Agmsmith
 * Fully working modification of position chunks for 2D.
 *
 * 70    4/04/99 3:14p Agmsmith
 * Array of sizes for private chunks too.  Partially working modification
 * of position chunks.
 *
 * 69    4/03/99 6:22p Agmsmith
 * LI_CHUNK_ModifyData under construction.
 *
 * 68    3/30/99 3:33p Agmsmith
 * Fix bug in serial number allocation under Win95.
 *
 * 67    3/28/99 1:02p Agmsmith
 * Count lines inside text blocks for loadchunky.
 *
 * 66    3/28/99 12:48p Agmsmith
 * Fixed some bugs with printing numbers in dump chunky.
 *
 * 65    3/28/99 12:22p Agmsmith
 * Added dimensionality chunk printing.
 *
 * 64    3/28/99 11:39a Agmsmith
 * Line numbers in load chunky error messages, added N: and W:, sound
 * settings are now percentages in optional subchunks.
 *
 * 63    3/25/99 4:04p Agmsmith
 * Bugs!
 *
 * 62    3/25/99 3:48p Agmsmith
 * New sound pitch, volume, panning chunks.
 *
 * 61    3/25/99 12:40p Agmsmith
 * Added the Cancel type of interpolation.
 *
 * 60    3/25/99 10:23a Agmsmith
 * Print parent end time for convenience.
 *
 * 59    3/24/99 9:09a Agmsmith
 *
 * 58    3/23/99 1:32p Agmsmith
 * Added new private chunks, different offsets and a mesh choice.
 *
 * 57    3/22/99 4:07p Agmsmith
 *
 * 56    3/21/99 3:29p Agmsmith
 * Changes to allow all sequences to have child sequences.
 *
 * 55    3/19/99 10:30a Agmsmith
 * Finished 3D Mesh chunk, and added Tweeker chunk.
 *
 * 54    3/15/99 4:41p Agmsmith
 * More functions for finding subchunks for the dope sheet.
 *
 * 53    3/13/99 5:29p Agmsmith
 * Need a recursive flag when counting selected sequences.
 *
 * 52    3/10/99 7:15p Agmsmith
 * Added LE_CHUNK_SortChildSequencesByTime.
 *
 * 51    3/09/99 7:03p Agmsmith
 * Count number of selected sequences.
 *
 * 50    3/07/99 3:31p Agmsmith
 * Oops.
 *
 * 49    3/07/99 3:27p Agmsmith
 * Don't include path when generating symbolic names for chunk dumping.
 *
 * 48    3/07/99 2:11p Agmsmith
 * Cosmetic changes to printing.
 *
 * 47    3/07/99 1:58p Agmsmith
 * Removed initialClock from sequence headers.
 *
 * 46    3/07/99 1:15p Agmsmith
 * Also update time multiples in LE_CHUNK_SizeSequence, and maybe work
 * better with indirect groups.
 *
 * 45    3/06/99 3:15p Agmsmith
 * Added LE_CHUNK_DeleteSelected function.
 *
 * 44    3/05/99 12:46p Agmsmith
 * Added LE_CHUNK_ToggleByRange.
 *
 * 43    3/05/99 12:08p Agmsmith
 * Oops.
 *
 * 42    3/05/99 10:45a Agmsmith
 * Added LE_CHUNK_SelectByRange.
 *
 * 41    3/04/99 4:18p Agmsmith
 * Added LE_CHUNK_ForAllSubSequences function.
 *
 * 40    2/25/99 11:40a Agmsmith
 * Need 16 bits for finding priority range of a sequence.
 *
 * 39    2/14/99 2:27p Agmsmith
 * Watch out for endTime of zero, it means infinite, when finding sequence
 * times.
 *
 * 38    2/14/99 2:18p Agmsmith
 * Add recursive updating of group chunk sizes to LE_CHUNK_SizeSequence.
 *
 * 37    2/13/99 12:20p Agmsmith
 *
 * 36    2/13/99 12:19p Agmsmith
 * Added LE_CHUNK_FindSerialPointer.
 *
 * 35    2/12/99 7:04p Agmsmith
 * More serial number stuff.
 *
 * 34    2/10/99 6:13p Agmsmith
 * Now inserts subchunks in a parent chunk identified by a serial number.
 *
 * 33    2/10/99 4:07p Agmsmith
 * Deleting subchunks by serial number.
 *
 * 32    2/09/99 2:51p Agmsmith
 * Added LE_CHUNK_FindSerialOffset.
 *
 * 31    2/08/99 6:55p Agmsmith
 * Blank line between important subchunks.
 *
 * 30    2/08/99 6:34p Agmsmith
 * Blank line between sequence items to make dump more readable.
 *
 * 29    2/08/99 5:44p Agmsmith
 * Added serial number subchunk.
 *
 * 28    2/05/99 4:43p Agmsmith
 * Off by one error.
 *
 * 27    2/05/99 3:11p Agmsmith
 *
 * 26    2/05/99 2:55p Agmsmith
 * Added function for finding the time and priority ranges of a sequence.
 *
 * 25    2/02/99 1:46p Agmsmith
 * Added CreateSequence function.
 *
 * 24    2/01/99 5:04p Agmsmith
 * Better dump formatting, no reserved field taking up space.
 *
 * 23    2/01/99 4:49p Agmsmith
 * More chunky options.
 *
 * 22    2/01/99 4:32p Agmsmith
 * Display ending action better.
 *
 * 21    2/01/99 3:48p Agmsmith
 * Type check warning avoided.
 *
 * 20    2/01/99 2:08p Agmsmith
 * New video attributes added.
 *
 * 19    1/31/99 3:24p Agmsmith
 * Video chunk addition.
 *
 * 18    1/22/99 11:21a Agmsmith
 * Now can add file name subchunks to remember where data was loaded from
 * when loading using file names.
 *
 * 17    1/21/99 1:52p Agmsmith
 * Now prompt the user for data items with no file name specified.
 *
 * 16    1/20/99 5:37p Agmsmith
 * Load chunky files using embedded file names to load subsiduary data.
 *
 * 15    1/17/99 3:22p Agmsmith
 * Use MAX_PATH rather than _MAX_PATH, they're the same.
 *
 * 14    1/13/99 8:22p Agmsmith
 * Fixed bug with dumping 3D chunks.
 *
 * 13    1/13/99 6:49p Agmsmith
 * Added a subroutine for loading chunks from an ASCII file.
 *
 * 12    1/13/99 2:10p Agmsmith
 * Added a few more private chunk dumps and other improvements.
 * DumpChunky now essentially finished.
 *
 * 11    1/12/99 8:31p Agmsmith
 * Now can print chunks with symbolic names substituted for DataIDs, but
 * haven't finished the private sequencer chunk definitions.
 *
 * 10    1/12/99 5:36p Agmsmith
 * Added more user names, dump chunk now has framework for printing out
 * fixed part structures.
 *
 * 9     1/12/99 2:12p Agmsmith
 * Converted to a class for storing semi-global data while dumping chunks.
 *
 * 8     1/08/99 4:36p Agmsmith
 * DumpChunks under construction, compiles and prints an outline but no
 * chunk contents yet.
 *
 * 7     1/08/99 12:11p Agmsmith
 *
 * 6     1/08/99 11:49a Agmsmith
 * Added functions for getting info about descended into levels (ChunkID,
 * size, offset).  Also rearranged error checking code to do less in
 * release mode.
 *
 * 5     1/07/99 6:32p Agmsmith
 * Added GrowChunk for inserting and deleting bytes inside nested chunks,
 * and even deleting whole chunks.
 *
 * 4     11/06/98 2:23p Agmsmith
 * Added the ability to write to DataIDs.
 ****************************************************************************/


/****************************************************************************/
/* INCLUDE FILES                                                            */
/*--------------------------------------------------------------------------*/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemChunk


/****************************************************************************/
/* PRIVATE DATA TYPES                                                       */
/*--------------------------------------------------------------------------*/

// The mapper maps a file or data item into a buffer.  All IO operations go
// through a separate function so that it can be replaced by other data
// sources easily.  Of course, this may have been better done as a class.

typedef enum MAPPER_ACTION_ENUM
{
  MAPPER_NO_OPERATION = 0,
    // A null command code.  Means don't do anything.  Used mostly
    // for detecting invalid command blocks.

  MAPPER_OPEN,
    // binds a data source to a chunk stream, using either
    // filename pointer or dataID passed in *dest and creates
    // whatever internal buffers it needs for data.  The size of
    // the file or item is returned in "bytes" for readable things.

  MAPPER_MAP,
    // return a pointer in 'dest' to 'bytes' of data at 'offset'
    // in the data source.  Changes the 'bytes' field to
    // reflect the actual number of bytes read, (e.g. in
    // case of end of file, which can be less than requested).
    // The data should remain valid until the next call to
    // any mapper action pertaining to the given stream.
    // Advances the current stream position past the mapped data.

  MAPPER_READ,
    // copies at most 'bytes' of data from the current
    // source position to a user specified buffer, and
    // advances the current stream position. Changes the 'bytes'
    // field to reflect the number of bytes actually read. If the
    // user specified buffer 'dest' is NULL, the bytes are simply discarded
    // (i.e. it is effectively a seek operation, advancing the pointer)

  MAPPER_WRITE,
    // copies at most 'bytes' of data from the user's buffer
    // to the 'offset' specified position in the file.  Advances the
    // current stream position. Changes the 'bytes' field to reflect
    // the number of bytes actually written.

  MAPPER_CLOSE,
    // closes the data source bound to the stream.
    // (mapper can retain temp buffers for future use, or destroy them)

} MAPPER_ACTION;



typedef enum MapperKindsEnum
{
  MAPPERS_FOR_READING = 0, // Marker for first read type mapper.

  MAPPER_FOR_FILE_READING = MAPPERS_FOR_READING,

#if CE_ARTLIB_EnableSystemData
  MAPPER_FOR_DATA_ITEM_READING,
#endif

#if CE_ARTLIB_EnableChunkWriting
  MAPPERS_FOR_WRITING,  // Marker for first write type mapper.
  MAPPER_FOR_FILE_WRITING = MAPPERS_FOR_WRITING,

  #if CE_ARTLIB_EnableSystemData
  MAPPER_FOR_DATA_ITEM_WRITING,
  #endif
#endif

  MAPPER_KIND_MAX
} MapperKind;



// The command block that gets passed to the mapper function, and
// updated with returned results.

typedef struct MapperActionStruct
{
  MAPPER_ACTION action;
  LE_CHUNK_StreamID streamID;
  void *dest;
  UNS32 offset;
  UNS32 bytes;
} MapperActionRecord, *MapperActionPointer;


typedef BOOL (*MemoryMapperFunctionPointer) (MapperActionPointer Action);
  // Callback to use for a given data source;
  // This makes the data source transparent to the chunk stream parsing
  // mechanism and also allows stuff like an asynchronous streaming
  // source to be plugged in later on. The PSX implementation should
  // be especially aware of memory usage for temporary buffers.
  // Should returns FALSE on error.


// Here's the core part of this module - the chunky data handling stream.

typedef struct LI_CHUNK_StreamStruct
{
  UNS32 currentOffset;
    // Current absolute position in the file that the higher level
    // chunk routines are at.  The actual file could be elsewhere,
    // and you don't need to know where that is because every read
    // operation specifies the offset explicitly.
  UNS32 chunkDataOffset[CE_ARTLIB_MaxChunkNestingLevels];
    // Where the chunk data starts in the file for each nest level.
    // Note that the start of the chunk header may be more obvious
    // but the top level "chunk" is the whole file which doesn't
    // have a header, so it is better to just specify where the data
    // starts.
  UNS32 chunkEnd [CE_ARTLIB_MaxChunkNestingLevels];
    // File position just past the end of the chunk data.  Typically
    // you know child chunks end when you reach the end of the parent
    // chunk which encloses the children.
  LE_CHUNK_ID chunkType [CE_ARTLIB_MaxChunkNestingLevels];
    // What kind of chunk this is.  Duplicates the chunk header value.
  MapperKind mapperKind;
    // Implies the mapper function to call.  Implies read/write use too.
  UNS8 chunkLevel;
    // Current nesting level.  Zero is for whole file, which is a kind of chunk itself.
  UNS8 inUse;
    // TRUE for this stream being used, FALSE for available streams.

  union MapperDataUnion
  {
    struct ReadFileStruct
    {
      FILE *inFile;
      UNS32 inFilePosition;
      BYTE *inBuffer;
      UNS32 inBufferSize;
    } readFile;

#if CE_ARTLIB_EnableSystemData
    struct ReadDataIDStruct
    {
      LE_DATA_DataId dataID;
      BYTE *bytePntr;
    } readDataID;
#endif

#if CE_ARTLIB_EnableChunkWriting
    struct WriteFileStruct
    {
      FILE *outFile;
      UNS32 outFilePosition;
      UNS32 outFileSize;
    } writeFile;

#if CE_ARTLIB_EnableSystemData
    struct WriteDataIDStruct
    {
      LE_DATA_DataId dataID;
      UNS32 allocatedSize;
      UNS32 usedSize;
      BYTE *bytePntr;
    } writeDataID;
#endif // CE_ARTLIB_EnableSystemData
#endif // CE_ARTLIB_EnableChunkWriting

  } mapperData;
} LI_CHUNK_StreamRecord, *LI_CHUNK_StreamPointer;



/****************************************************************************/
/* GLOBAL VARIABLES                                                         */
/*--------------------------------------------------------------------------*/

// This array contains the size of the fixed size data in each of the
// standard types of chunk.  Everything after the fixed size data is
// subchunks.  A value of 0xFFFF means that there are no subchunks,
// everything inside is raw data.  This is useful if you want to parse
// a chunky file without knowing too much about the subchunks.  Note
// that user defined in-context aren't in this array, since the
// internal structure varies depending on the context.  The size doesn't
// include the size of the chunk header, just the fixed data portion.

const UNS16 LE_CHUNK_ChunkFixedDataSizes [LE_CHUNK_ID_NULL_IN_CONTEXT + 1] =
{
  0xFFFF, // LE_CHUNK_ID_NULL_STANDARD == 0
#if CE_ARTLIB_EnableSystemSequencer
  sizeof (LE_SEQNCR_SequenceGroupingChunkRecord),   // LE_CHUNK_ID_SEQ_GROUPING
  sizeof (LE_SEQNCR_SequenceIndirectChunkRecord),   // LE_CHUNK_ID_SEQ_INDIRECT
  sizeof (LE_SEQNCR_Sequence2DBitmapChunkRecord),   // LE_CHUNK_ID_SEQ_2DBITMAP
  sizeof (LE_SEQNCR_Sequence3DModelChunkRecord),    // LE_CHUNK_ID_SEQ_3DMODEL
  sizeof (LE_SEQNCR_SequenceSoundChunkRecord),      // LE_CHUNK_ID_SEQ_SOUND
  sizeof (LE_SEQNCR_SequenceVideoChunkRecord),      // LE_CHUNK_ID_SEQ_VIDEO
  sizeof (LE_SEQNCR_SequenceCameraChunkRecord),     // LE_CHUNK_ID_SEQ_CAMERA
  sizeof (LE_SEQNCR_SequencePreloaderChunkRecord),  // LE_CHUNK_ID_SEQ_PRELOADER
  sizeof (LE_SEQNCR_Sequence3DMeshChunkRecord),     // LE_CHUNK_ID_SEQ_3DMESH
  sizeof (LE_SEQNCR_SequenceTweekerChunkRecord),    // LE_CHUNK_ID_SEQ_TWEEKER
#else
  0xFFFF,  // LE_CHUNK_ID_SEQ_GROUPING
  0xFFFF,  // LE_CHUNK_ID_SEQ_INDIRECT
  0xFFFF,  // LE_CHUNK_ID_SEQ_2DBITMAP
  0xFFFF,  // LE_CHUNK_ID_SEQ_3DMODEL
  0xFFFF,  // LE_CHUNK_ID_SEQ_SOUND
  0xFFFF,  // LE_CHUNK_ID_SEQ_VIDEO
  0xFFFF,  // LE_CHUNK_ID_SEQ_CAMERA
  0xFFFF,  // LE_CHUNK_ID_SEQ_PRELOADER
  0xFFFF,  // LE_CHUNK_ID_SEQ_3DMESH
  0xFFFF,  // LE_CHUNK_ID_SEQ_TWEEKER
#endif /* CE_ARTLIB_EnableSystemSequencer */
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,

  /* 20 to 29 */
  0xFFFF, // LE_CHUNK_ID_FILE_NAME_1
  0xFFFF, // LE_CHUNK_ID_FILE_NAME_2
  0xFFFF, // LE_CHUNK_ID_FILE_NAME_3
  0xFFFF, // LE_CHUNK_ID_FILE_NAME_4
  0xFFFF, // LE_CHUNK_ID_FILE_NAME_5
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,

  /* 30 to 39 */
  0xFFFF, // LE_CHUNK_ID_USER_NAME_1
  0xFFFF, // LE_CHUNK_ID_USER_NAME_2
  0xFFFF, // LE_CHUNK_ID_USER_NAME_3
  0xFFFF, // LE_CHUNK_ID_USER_NAME_4
  0xFFFF, // LE_CHUNK_ID_USER_NAME_5
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,

  /* 40 to 49 */
  sizeof (LE_CHUNK_DopeDataRecord), // LE_CHUNK_ID_DOPE_DATA
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,

  /* 50 to 59 */
  0xFFFF, // LE_CHUNK_ID_ANNOTATION
  0xFFFF, // LE_CHUNK_ID_AUTHOR
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF,
  0xFFFF, // LE_CHUNK_ID_COPYRIGHT

  /* The rest */
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, //  60 to 69
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, //  70 to 79
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, //  80 to 89
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, //  90 to 99
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, // 100 to 109
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, // 110 to 119
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,                 // 120 to 127

  /* 128 */
  0xFFFF // LE_CHUNK_ID_NULL_IN_CONTEXT
};


const char LI_CHUNK_UnknownName[] = "LE_CHUNK_ID_Unknown";

const char * const LE_CHUNK_NamesOfPublicChunkTypes [LE_CHUNK_ID_NULL_IN_CONTEXT + 1] =
{
  "LE_CHUNK_ID_NULL_STANDARD",
  "LE_CHUNK_ID_SEQ_GROUPING",
  "LE_CHUNK_ID_SEQ_INDIRECT",
  "LE_CHUNK_ID_SEQ_2DBITMAP",
  "LE_CHUNK_ID_SEQ_3DMODEL",
  "LE_CHUNK_ID_SEQ_SOUND",
  "LE_CHUNK_ID_SEQ_VIDEO",
  "LE_CHUNK_ID_SEQ_CAMERA",
  "LE_CHUNK_ID_SEQ_PRELOADER",
  "LE_CHUNK_ID_SEQ_3DMESH",
  "LE_CHUNK_ID_SEQ_TWEEKER",
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,

  /* 20 to 29 */
  "LE_CHUNK_ID_FILE_NAME_1",
  "LE_CHUNK_ID_FILE_NAME_2",
  "LE_CHUNK_ID_FILE_NAME_3",
  "LE_CHUNK_ID_FILE_NAME_4",
  "LE_CHUNK_ID_FILE_NAME_5",
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,

  /* 30 to 39 */
  "LE_CHUNK_ID_USER_NAME_1",
  "LE_CHUNK_ID_USER_NAME_2",
  "LE_CHUNK_ID_USER_NAME_3",
  "LE_CHUNK_ID_USER_NAME_4",
  "LE_CHUNK_ID_USER_NAME_5",
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,

  /* 40 to 49 */
  "LE_CHUNK_ID_DOPE_DATA",
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,

  /* 50 to 59 */
  "LE_CHUNK_ID_ANNOTATION",
  "LE_CHUNK_ID_AUTHOR",
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName,
  "LE_CHUNK_ID_COPYRIGHT",

  /* 60 to 127 */
  LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName,
  LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName, LI_CHUNK_UnknownName,

  /* 128 */
  "LE_CHUNK_ID_NULL_IN_CONTEXT"
};



#if CE_ARTLIB_EnableSystemSequencer
const UNS16 LE_CHUNK_SequencerPrivateChunkFixedDataSizes
[LE_SEQNCR_CHUNK_ID_MAX_PRIVATE_CHUNKS - LE_CHUNK_ID_NULL_IN_CONTEXT] =
{
  0xFFFF,                                                   // LE_CHUNK_ID_NULL_IN_CONTEXT == 128
  sizeof (LE_SEQNCR_DimensionalityChunkRecord),             // LE_SEQNCR_CHUNK_ID_DIMENSIONALITY
  sizeof (LE_SEQNCR_2DOffsetChunkRecord),                   // LE_SEQNCR_CHUNK_ID_2D_OFFSET
  sizeof (LE_SEQNCR_2DXformChunkRecord),                    // LE_SEQNCR_CHUNK_ID_2D_XFORM
  sizeof (LE_SEQNCR_2DOriginScaleRotateOffsetChunkRecord),  // LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET
  sizeof (LE_SEQNCR_3DOffsetChunkRecord),                   // LE_SEQNCR_CHUNK_ID_3D_OFFSET
  sizeof (LE_SEQNCR_3DXformChunkRecord),                    // LE_SEQNCR_CHUNK_ID_3D_XFORM
  sizeof (LE_SEQNCR_3DOriginScaleRotateOffsetChunkRecord),  // LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET
  sizeof (LE_SEQNCR_2DBoundingBoxChunkRecord),              // LE_SEQNCR_CHUNK_ID_2D_BOUNDING_BOX
  sizeof (LE_SEQNCR_3DBoundingBoxChunkRecord),              // LE_SEQNCR_CHUNK_ID_3D_BOUNDING_BOX
  sizeof (LE_SEQNCR_3DBoundingSphereChunkRecord),           // LE_SEQNCR_CHUNK_ID_3D_BOUNDING_SPHERE
  sizeof (LE_SEQNCR_3DMeshChoiceChunkRecord),               // LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE
  sizeof (LE_SEQNCR_LabelChunkRecord),                      // LE_SEQNCR_CHUNK_ID_HIT_BOX_LABEL
  sizeof (LE_SEQNCR_SoundPitchChunkRecord),                 // LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH
  sizeof (LE_SEQNCR_SoundVolumeChunkRecord),                // LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME
  sizeof (LE_SEQNCR_SoundPanningChunkRecord),               // LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING
  sizeof (LE_SEQNCR_FieldOfViewChunkRecord)                 // LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW
};
#endif /* CE_ARTLIB_EnableSystemSequencer */


/************************************************************/
/* STATIC VARIABLES                                         */
/*----------------------------------------------------------*/

static MemoryMapperFunctionPointer MapperFunctionsArray [MAPPER_KIND_MAX];

static LI_CHUNK_StreamRecord StreamArray [CE_ARTLIB_MaxOpenChunkStreams];

static CRITICAL_SECTION   StreamAllocCriticalSectionStorage;
static LPCRITICAL_SECTION StreamAllocCS;
  // This critical section prevents one thread from allocating a stream
  // while another one is also busy allocating one.  Otherwise two threads
  // may get the same stream.  Deallocating and other operations aren't
  // a problem, since each stream is used by only one thread at a time.
  // The pointer is NULL when the critical section hasn't been initialised.

#if CE_ARTLIB_EnableChunkFileLoading
static LE_CHUNK_SerialNumber LI_CHUNK_LastSerialNumber;
  // Used for allocating new serial numbers for the LE_CHUNK_ID_DOPE_DATA
  // chunk, as items are loaded into memory.  Avoid zero since that means
  // an uninitialised serial number.
#endif



/*****************************************************************************
 *****************************************************************************
 **  F I L E   R E A D I N G   M A P P E R   R O U T I N E S                **
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * Open the file reading stream.  Returns TRUE if successful.  File name is
 * in "dest" on input, returns file size in "bytes".
 */

static BOOL FileReaderOpen (MapperActionPointer ActionPntr)
{
  char                   *FileName;
  long                    FilePosition;
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + ActionPntr->streamID;

#if CE_ARTLIB_EnableDebugMode
  if (MyStreamPntr->mapperData.readFile.inFile != NULL)
  {
    LE_ERROR_ErrorMsg (__FILE__ " FileReaderOpen: "
    "Trying to open an already open file.\r\n");
    return FALSE; // There is already an open file.
  }
#endif

#if CE_ARTLIB_EnableDebugMode
  if (MyStreamPntr->mapperData.readFile.inBuffer != NULL)
  {
    LE_ERROR_ErrorMsg (__FILE__ " FileReaderOpen: "
    "Buffer is already allocated, should be NULL.\r\n");
    return FALSE; // Something is wrong with our buffer pointer.
  }
#endif

  // Now we know that the file and memory pointers are NULL, so we can
  // safely use ErrorExit without weird things happening.

#if CE_ARTLIB_EnableDebugMode
  if (ActionPntr->dest == NULL)
  {
    LE_ERROR_ErrorMsg (__FILE__ " FileReaderOpen: "
    "Need to specify a file name pointer.\r\n");
    goto ErrorExit; // Need a file name!
  }
#endif

  FileName = (char *) ActionPntr->dest;

#if CE_ARTLIB_EnableDebugMode
  if (FileName[0] == 0)
  {
    LE_ERROR_DebugMsg (__FILE__ " FileReaderOpen: "
    "Need a non-empty file name.\r\n", LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit; // Need some characters in the file name!
  }
#endif

  MyStreamPntr->mapperData.readFile.inFile = fopen (FileName, "rb");

  if (MyStreamPntr->mapperData.readFile.inFile == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " FileReaderOpen: "
    "Unable to open \"%s\" for reading.\r\n", FileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // Failed to open the file.
  }

  // Allocate a temporary buffer for memory mapping (the user gets a pointer
  // to somewhere in this buffer to save from copying into a user buffer).

  MyStreamPntr->mapperData.readFile.inBuffer =
    (LPBYTE) LI_MEMORY_AllocLibraryBlock (CE_ARTLIB_ChunkReadFileBufferSize);

  if (MyStreamPntr->mapperData.readFile.inBuffer == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " FileReaderOpen: "
    "Unable to allocate %d bytes for buffers for \"%s\".\r\n",
    (int) CE_ARTLIB_ChunkReadFileBufferSize, FileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // No memory for buffers.
  }

  // Find the size of the file.

  if (fseek (MyStreamPntr->mapperData.readFile.inFile, 0, SEEK_END))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " FileReaderOpen: "
    "Unable to seek to end of \"%s\".\r\n", FileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // Seek failed.
  }

  FilePosition = ftell (MyStreamPntr->mapperData.readFile.inFile);
  if (FilePosition < 0)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " FileReaderOpen: "
    "Unable to find size of \"%s\".\r\n", FileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // Ftell failed.
  }

  ActionPntr->bytes = FilePosition;  // Size of the file.

  if (fseek (MyStreamPntr->mapperData.readFile.inFile, 0, SEEK_SET))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " FileReaderOpen: "
    "Unable to seek to beginning of \"%s\".\r\n", FileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // Second seek failed.
  }

  MyStreamPntr->mapperData.readFile.inFilePosition = 0;

  // Success!
  return TRUE;


ErrorExit: // Clean up allocations and return FALSE.
  ActionPntr->bytes = 0; // Size of the file, we don't know what it is.

  if (MyStreamPntr->mapperData.readFile.inFile != NULL)
  {
    fclose (MyStreamPntr->mapperData.readFile.inFile);
    MyStreamPntr->mapperData.readFile.inFile = NULL;
  }

  if (MyStreamPntr->mapperData.readFile.inBuffer != NULL)
  {
    LI_MEMORY_DeallocLibraryBlock (MyStreamPntr->mapperData.readFile.inBuffer);
    MyStreamPntr->mapperData.readFile.inBuffer = NULL;
  }

  return FALSE;
}



/*****************************************************************************
 * Load some data into a provided buffer.  This means reading the file at the
 * given position (in "offset") and putting "bytes" bytes into the buffer.
 * Updates "bytes" with the amount actually read.  If Buffer is NULL it just
 * returns the size that would have been read.  Returns TRUE if successful.
 */

static BOOL FileReaderFillBuffer (MapperActionPointer ActionPntr,
void *Buffer, UNS32 BufferSize)
{
  long                    AmountActuallyRead;
  long                    AmountToRead;
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + ActionPntr->streamID;
  AmountToRead = ActionPntr->bytes;
  ActionPntr->bytes = 0;  // In case of an error exit.

#if CE_ARTLIB_EnableDebugMode
  if (MyStreamPntr->mapperData.readFile.inFile == NULL)
  {
    LE_ERROR_ErrorMsg (__FILE__ " FileReaderFillBuffer: "
    "File isn't open!\r\n");
    return FALSE; // Need an open file.
  }
#endif

  // Validate the offset.

  if (ActionPntr->offset > MyStreamPntr->chunkEnd[0] /* File size */)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " FileReaderFillBuffer: "
    "Requested offset is past end of file: %d/%d in stream %d.\r\n",
    (int) ActionPntr->offset, (int) MyStreamPntr->chunkEnd[0],
    (int) ActionPntr->streamID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE; // Bad offset requested.
  }

  // How much do we need to read?  Check for end of file and buffer size limits.

  if (AmountToRead > (long) BufferSize)
    AmountToRead = BufferSize;
  if (ActionPntr->offset + AmountToRead > MyStreamPntr->chunkEnd[0] /* File size */)
    AmountToRead = MyStreamPntr->chunkEnd[0] - ActionPntr->offset;
  if (AmountToRead <= 0)
    return TRUE; // Nothing to do.

  // Are we just simulating a read?

  if (Buffer == NULL)
  {
    ActionPntr->bytes = AmountToRead;
    return TRUE;
  }

  // Seek to the desired spot if needed.

  if (MyStreamPntr->mapperData.readFile.inFilePosition != ActionPntr->offset)
  {
    if (fseek (MyStreamPntr->mapperData.readFile.inFile,
    ActionPntr->offset, SEEK_SET))  // fseek returns non-zero on failure.
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " FileReaderFillBuffer: "
      "Unable to seek to position %d/%d in stream %d.\r\n",
      (int) ActionPntr->offset, (int) MyStreamPntr->chunkEnd[0],
      (int) ActionPntr->streamID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      MyStreamPntr->mapperData.readFile.inFilePosition = (UNS32) -1; // Mark current position as unknown.
      return FALSE;  // Failed to seek in file.
    }
    MyStreamPntr->mapperData.readFile.inFilePosition = ActionPntr->offset;
  }

  // Fill the provided buffer (may or may not be our internal one).

  AmountActuallyRead = fread (Buffer, 1, AmountToRead,
    MyStreamPntr->mapperData.readFile.inFile);

#if CE_ARTLIB_EnableDebugMode
  if (AmountActuallyRead != AmountToRead)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " FileReaderFillBuffer: "
    "IO Error?  Only read %d bytes of %d in stream %d.\r\n",
    (int) AmountActuallyRead, (int) AmountToRead, (int) ActionPntr->streamID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

  ActionPntr->bytes = AmountActuallyRead;
  MyStreamPntr->mapperData.readFile.inFilePosition += AmountActuallyRead;

  return TRUE;
}



/*****************************************************************************
 * Map some data.  This means reading the file at the given position
 * (in "offset") and putting "bytes" bytes into our buffer and letting
 * the user access our buffered data via "dest".
 */

static BOOL FileReaderMap (MapperActionPointer ActionPntr)
{
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + ActionPntr->streamID;
  ActionPntr->dest = MyStreamPntr->mapperData.readFile.inBuffer;

#if CE_ARTLIB_EnableDebugMode
  if (MyStreamPntr->mapperData.readFile.inBuffer == NULL)
    LE_ERROR_ErrorMsg (__FILE__ " FileReaderMap: "
    "Buffer isn't allocated!\r\n");
#endif

  return FileReaderFillBuffer (ActionPntr,
    MyStreamPntr->mapperData.readFile.inBuffer,
    CE_ARTLIB_ChunkReadFileBufferSize);
}



/*****************************************************************************
 * Read some data.  This means reading the file at the given position
 * (in "offset") and putting "bytes" bytes into the user's buffer.
 * The buffer can be NULL, in which case it simulates reading and returns
 * the number of bytes that would have been read.
 * Returns the actual amount read in "bytes".
 */

static BOOL FileReaderRead (MapperActionPointer ActionPntr)
{
  return FileReaderFillBuffer (ActionPntr, ActionPntr->dest, ActionPntr->bytes);
}



/*****************************************************************************
 * Close the file reading stream's file and delete the buffer.
 */

static BOOL FileReaderClose (MapperActionPointer ActionPntr)
{
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + ActionPntr->streamID;

  if (MyStreamPntr->mapperData.readFile.inFile != NULL)
  {
    fclose (MyStreamPntr->mapperData.readFile.inFile);
    MyStreamPntr->mapperData.readFile.inFile = NULL;
  }

  if (MyStreamPntr->mapperData.readFile.inBuffer != NULL)
  {
    LI_MEMORY_DeallocLibraryBlock (MyStreamPntr->mapperData.readFile.inBuffer);
    MyStreamPntr->mapperData.readFile.inBuffer = NULL;
  }

  return TRUE;
}



/*****************************************************************************
 * The entry point for the readable file mapper.  Execute the command.
 */

BOOL LI_CHUNK_FileReadMapper (MapperActionPointer Action)
{
#if CE_ARTLIB_EnableDebugMode
  if (Action == NULL || Action->streamID < 0 ||
  Action->streamID >= CE_ARTLIB_MaxOpenChunkStreams)
  {
    LE_ERROR_ErrorMsg ("LI_CHUNK_FileReadMapper: "
    "Invalid parameters.\r\n");
    return FALSE; // Being called with garbage.
  }
#endif

  switch (Action->action)
  {
  case MAPPER_OPEN:
    return FileReaderOpen (Action);

  case MAPPER_MAP:
    return FileReaderMap (Action);

  case MAPPER_READ:
    return FileReaderRead (Action);

  case MAPPER_CLOSE:
    return FileReaderClose (Action);
  }

#if CE_ARTLIB_EnableDebugMode
  LE_ERROR_DebugMsg ("LI_CHUNK_FileReadMapper: Unknown mapper action code.\r\n",
    LE_ERROR_DebugMsgToFileAndScreen);
#endif

  return FALSE;
}



#if CE_ARTLIB_EnableChunkWriting
/*****************************************************************************
 *****************************************************************************
 **  F I L E   W R I T I N G   M A P P E R   R O U T I N E S                **
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * Open the file writing stream.  Returns TRUE if successful.  File name is
 * in "dest" on input.
 */

static BOOL FileWriterOpen (MapperActionPointer ActionPntr)
{
  char                   *FileName;
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + ActionPntr->streamID;

#if CE_ARTLIB_EnableDebugMode
  if (MyStreamPntr->mapperData.writeFile.outFile != NULL)
  {
    LE_ERROR_ErrorMsg (__FILE__ " FileWriterOpen: "
    "Trying to open an already open file.\r\n");
    return FALSE; // There is already an open file.
  }
#endif

  // Now we know that the file pointer is NULL, so we can
  // safely use ErrorExit without weird things happening.

#if CE_ARTLIB_EnableDebugMode
  if (ActionPntr->dest == NULL)
  {
    LE_ERROR_ErrorMsg (__FILE__ " FileWriterOpen: "
    "Need to specify a file name pointer.\r\n");
    goto ErrorExit; // Need a file name!
  }
#endif

  FileName = (char *) ActionPntr->dest;

#if CE_ARTLIB_EnableDebugMode
  if (FileName[0] == 0)
  {
    LE_ERROR_DebugMsg (__FILE__ " FileWriterOpen: "
    "Need a non-empty file name.\r\n", LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit; // Need some characters in the file name!
  }
#endif

  MyStreamPntr->mapperData.writeFile.outFile = fopen (FileName, "wb");

  if (MyStreamPntr->mapperData.writeFile.outFile == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " FileWriterOpen: "
    "Unable to open \"%s\" for writing.\r\n", FileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // Failed to open the file.
  }

  MyStreamPntr->mapperData.writeFile.outFilePosition = 0;
  MyStreamPntr->mapperData.writeFile.outFileSize = 0;

  // Success!
  return TRUE;


ErrorExit: // Clean up allocations and return FALSE.
  if (MyStreamPntr->mapperData.writeFile.outFile != NULL)
  {
    fclose (MyStreamPntr->mapperData.writeFile.outFile);
    MyStreamPntr->mapperData.writeFile.outFile = NULL;
  }

  return FALSE;
}



/*****************************************************************************
 * Write some data from the given buffer at the given file offset.
 */

static BOOL FileWriterWrite (MapperActionPointer ActionPntr)
{
  long                    AmountActuallyWritten;
  long                    AmountToWrite;
  LI_CHUNK_StreamPointer  MyStreamPntr;
  long                    NumberOfZeroes;
  BYTE                    ZeroesArray [256];
  long                    ZeroesToWrite;

  MyStreamPntr = StreamArray + ActionPntr->streamID;
  AmountToWrite = ActionPntr->bytes;
  ActionPntr->bytes = 0;  // In case of an error exit.

#if CE_ARTLIB_EnableDebugMode
  if (MyStreamPntr->mapperData.writeFile.outFile == NULL)
  {
    LE_ERROR_ErrorMsg (__FILE__ " FileWriterWrite: "
    "File isn't open!\r\n");
    return FALSE; // Need an open file.
  }
#endif

#if CE_ARTLIB_EnableDebugMode
  if (ActionPntr->dest == NULL)
  {
    LE_ERROR_ErrorMsg (__FILE__ " FileWriterWrite: "
    "Buffer pointer is NULL!\r\n");
    return FALSE; // Need a buffer to write from.
  }
#endif

  // Seek to the desired spot if needed.

  if (MyStreamPntr->mapperData.writeFile.outFilePosition != ActionPntr->offset)
  {
    // If the seek spot is after the end of the file, then go to the current
    // end of file and write zeroes until the seek desired spot is reached.

    if (ActionPntr->offset > MyStreamPntr->mapperData.writeFile.outFileSize)
    {
      // Advance to the end of the file as we know it.

      if (fseek (MyStreamPntr->mapperData.writeFile.outFile,
      MyStreamPntr->mapperData.writeFile.outFileSize, SEEK_SET))
      {
  #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " FileWriterWrite: "
        "Unable to seek to position %d (end?) in stream %d.\r\n",
        (int) MyStreamPntr->mapperData.writeFile.outFileSize,
        (int) ActionPntr->streamID);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
        MyStreamPntr->mapperData.writeFile.outFilePosition = (UNS32) -1; // Mark current position as unknown.
        return FALSE;  // Failed to seek in file.
      }

      // Set up a buffer full of zeroes for writing.

      NumberOfZeroes =
        ActionPntr->offset - MyStreamPntr->mapperData.writeFile.outFileSize;
      ZeroesToWrite = min (sizeof (ZeroesArray), NumberOfZeroes);
      memset (ZeroesArray, 0, ZeroesToWrite);
      while (NumberOfZeroes > 0)
      {
        // Write small bunches of zeroes until we have enough.

        AmountActuallyWritten = fwrite (ZeroesArray, 1, ZeroesToWrite,
          MyStreamPntr->mapperData.writeFile.outFile);
#if CE_ARTLIB_EnableDebugMode
        if (AmountActuallyWritten != ZeroesToWrite)
        {
          sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " FileWriterWrite: "
          "IO Error?  Only wrote %d zeroes of %d in stream %d.\r\n",
          (int) AmountActuallyWritten, (int) ZeroesToWrite, (int) ActionPntr->streamID);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
          MyStreamPntr->mapperData.writeFile.outFilePosition = (UNS32) -1; // Mark current position as unknown.
          return FALSE;
        }
#endif
        NumberOfZeroes -= ZeroesToWrite;
        ZeroesToWrite = min (sizeof (ZeroesArray), NumberOfZeroes);
      }
    }
    else // Seek to a position within the current file size.
    {
      if (fseek (MyStreamPntr->mapperData.writeFile.outFile,
      ActionPntr->offset, SEEK_SET))  // fseek returns non-zero on failure.
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " FileWriterWrite: "
          "Unable to seek to position %d/%d in stream %d.\r\n",
          (int) ActionPntr->offset,
          (int) MyStreamPntr->mapperData.writeFile.outFileSize,
          (int) ActionPntr->streamID);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        MyStreamPntr->mapperData.writeFile.outFilePosition = (UNS32) -1; // Mark current position as unknown.
        return FALSE;  // Failed to seek in file.
      }
    }
    // Have successfully moved the current file position to the desired offset.
    MyStreamPntr->mapperData.writeFile.outFilePosition = ActionPntr->offset;
  }

  // Write the requested data.

  AmountActuallyWritten = fwrite (ActionPntr->dest,
  1, AmountToWrite, MyStreamPntr->mapperData.writeFile.outFile);

  ActionPntr->bytes = AmountActuallyWritten;
  MyStreamPntr->mapperData.writeFile.outFilePosition += AmountActuallyWritten;

  // Did the overall size of the file grow?

  if (MyStreamPntr->mapperData.writeFile.outFileSize < MyStreamPntr->mapperData.writeFile.outFilePosition)
    MyStreamPntr->mapperData.writeFile.outFileSize = MyStreamPntr->mapperData.writeFile.outFilePosition;

  // Check for write errors.

  if (AmountActuallyWritten != AmountToWrite)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " FileWriterWrite: "
    "IO Error?  Only wrote %d bytes of %d in stream %d.\r\n",
    (int) AmountActuallyWritten, (int) AmountToWrite, (int) ActionPntr->streamID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  return TRUE;
}



/*****************************************************************************
 * Close the file writing stream's file.
 */

static BOOL FileWriterClose (MapperActionPointer ActionPntr)
{
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + ActionPntr->streamID;

  if (MyStreamPntr->mapperData.writeFile.outFile != NULL)
  {
    fclose (MyStreamPntr->mapperData.writeFile.outFile);
    MyStreamPntr->mapperData.writeFile.outFile = NULL;
  }

  return TRUE;
}



/*****************************************************************************
 * The entry point for the writeable file mapper.  Execute the command.
 */

BOOL LI_CHUNK_FileWriterMapper (MapperActionPointer Action)
{
#if CE_ARTLIB_EnableDebugMode
  if (Action == NULL || Action->streamID < 0 ||
  Action->streamID >= CE_ARTLIB_MaxOpenChunkStreams)
  {
    LE_ERROR_ErrorMsg ("LI_CHUNK_FileWriterMapper: "
    "Invalid parameters.\r\n");
    return FALSE; // Being called with garbage.
  }
#endif

  switch (Action->action)
  {
  case MAPPER_OPEN:
    return FileWriterOpen (Action);

  case MAPPER_WRITE:
    return FileWriterWrite (Action);

  case MAPPER_CLOSE:
    return FileWriterClose (Action);
  }

#if CE_ARTLIB_EnableDebugMode
  LE_ERROR_DebugMsg ("LI_CHUNK_FileWriterMapper: Unknown mapper action code.\r\n",
    LE_ERROR_DebugMsgToFileAndScreen);
#endif

  return FALSE;
}
#endif // CE_ARTLIB_EnableChunkWriting



#if CE_ARTLIB_EnableSystemData
/*****************************************************************************
 *****************************************************************************
 **  D A T A   I D   R E A D I N G   M A P P E R   R O U T I N E S          **
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * Open the data ID reading stream.  Returns TRUE if successful.  The
 * Data ID is stuffed into "dest" on input, returns data item size in "bytes".
 */

static BOOL DataIDReaderOpen (MapperActionPointer ActionPntr)
{
  LE_DATA_DataId          DataID;
  long                    DataSize;
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + ActionPntr->streamID;

  MyStreamPntr->mapperData.readDataID.dataID = DataID =
    (LE_DATA_DataId) ActionPntr->dest;

  DataSize = LE_DATA_GetLoadedSize (DataID);

  if (DataSize <= 0)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " DataIDReaderOpen: "
    "Unable to find DataID $%08X for stream %d.\r\n",
    (int) DataID, (int) ActionPntr->streamID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    MyStreamPntr->mapperData.readDataID.dataID = LE_DATA_EmptyItem;
    ActionPntr->bytes = 0;
    return FALSE; // Invalid DataID.
  }

  ActionPntr->bytes = DataSize; // Size of the item.

  // Keep a reference on it, so it doesn't get unloaded while
  // we are processing the chunk.

  LE_DATA_AddRef (DataID);

  // Cache a pointer to the start of the data.

  MyStreamPntr->mapperData.readDataID.bytePntr =
    (LPBYTE) LE_DATA_Use (DataID);

  if (MyStreamPntr->mapperData.readDataID.bytePntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " DataIDReaderOpen: "
    "Can't get pointer to data for DataID $%08X in stream %d.\r\n",
    (int) MyStreamPntr->mapperData.readDataID.dataID, (int) ActionPntr->streamID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    LE_DATA_RemoveRef (DataID);
    MyStreamPntr->mapperData.readDataID.dataID = LE_DATA_EmptyItem;
    return FALSE; // Unable to access DataID.
  }
  // Success!
  return TRUE;
}



/*****************************************************************************
 * Load some data into a provided buffer.  This means reading the item at the
 * given position (in "offset") and putting "bytes" bytes into the buffer.
 * Updates "bytes" with the amount actually read.  If Buffer is NULL it just
 * returns the size that would have been read and sets "dest" to point at the
 * data.  Returns TRUE if successful.
 */

static BOOL DataIDReaderFillBuffer (MapperActionPointer ActionPntr,
void *Buffer, UNS32 BufferSize)
{
  long                    AmountToRead;
  BYTE                   *DataPntr;
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + ActionPntr->streamID;
  AmountToRead = ActionPntr->bytes;
  ActionPntr->bytes = 0;  // In case of an error exit.
  ActionPntr->dest = NULL;  // Ditto.

  // Validate the offset.

  if (ActionPntr->offset > MyStreamPntr->chunkEnd[0] /* File size */)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " DataIDReaderFillBuffer: "
    "Requested offset is past end of data item: %d/%d in stream %d.\r\n",
    (int) ActionPntr->offset, (int) MyStreamPntr->chunkEnd[0],
    (int) ActionPntr->streamID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE; // Bad offset requested.
  }

  // How much do we need to read?  Check for end of file and buffer size limits.

  if (AmountToRead > (long) BufferSize)
    AmountToRead = BufferSize;
  if (ActionPntr->offset + AmountToRead > MyStreamPntr->chunkEnd[0] /* File size */)
    AmountToRead = MyStreamPntr->chunkEnd[0] - ActionPntr->offset;
  if (AmountToRead <= 0)
    return TRUE; // Nothing to do.

  // Get a pointer to the data in the data system's memory.

  DataPntr = MyStreamPntr->mapperData.readDataID.bytePntr;

  // Fill the buffer (if provided) or just point into the data item.

  ActionPntr->bytes = AmountToRead;

  if (Buffer == NULL)
    ActionPntr->dest = DataPntr + ActionPntr->offset; // Just point at data.
  else // Copy data into a user provided buffer.
    memcpy (Buffer, DataPntr + ActionPntr->offset, AmountToRead);

  return TRUE;
}



/*****************************************************************************
 * Map some data.  This just means pointing at the data at the given offset
 * ("dest" set) and setting "bytes" to the number of available bytes there.
 */

static BOOL DataIDReaderMap (MapperActionPointer ActionPntr)
{
  return DataIDReaderFillBuffer (ActionPntr, NULL, INT32_MAX);
}



/*****************************************************************************
 * Read some data.  This means copying the bytes at the given position
 * (in "offset") and putting "bytes" bytes into the user's buffer.
 * The buffer can be NULL, in which case it simulates reading and returns
 * the number of bytes that would have been read.
 * Returns the actual amount read in "bytes".
 */

static BOOL DataIDReaderRead (MapperActionPointer ActionPntr)
{
  return DataIDReaderFillBuffer (ActionPntr, ActionPntr->dest, ActionPntr->bytes);
}



/*****************************************************************************
 * Close the DataID reading stream's stuff.
 */

static BOOL DataIDReaderClose (MapperActionPointer ActionPntr)
{
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + ActionPntr->streamID;

  LE_DATA_RemoveRef (MyStreamPntr->mapperData.readDataID.dataID);

  MyStreamPntr->mapperData.readDataID.dataID = LE_DATA_EmptyItem;

  // Note - don't unload it here!  Otherwise, whenever a sequence is parsed,
  // the item gets repeatedly loaded and unloaded from the data file.  Better
  // to let the sequencer or other caller explicitly unload it.

  return TRUE;
}



/*****************************************************************************
 * The entry point for the readable DataID mapper.  Execute the command.
 */

BOOL LI_CHUNK_DataIDReadMapper (MapperActionPointer Action)
{
#if CE_ARTLIB_EnableDebugMode
  if (Action == NULL || Action->streamID < 0 ||
  Action->streamID >= CE_ARTLIB_MaxOpenChunkStreams)
  {
    LE_ERROR_ErrorMsg ("LI_CHUNK_DataIDReadMapper: "
    "Invalid parameters.\r\n");
    return FALSE; // Being called with garbage.
  }
#endif

  switch (Action->action)
  {
  case MAPPER_OPEN:
    return DataIDReaderOpen (Action);

  case MAPPER_MAP:
    return DataIDReaderMap (Action);

  case MAPPER_READ:
    return DataIDReaderRead (Action);

  case MAPPER_CLOSE:
    return DataIDReaderClose (Action);
  }

#if CE_ARTLIB_EnableDebugMode
  LE_ERROR_DebugMsg ("LI_CHUNK_DataIDReadMapper: Unknown mapper action code.\r\n",
    LE_ERROR_DebugMsgToFileAndScreen);
#endif

  return FALSE;
}
#endif /* CE_ARTLIB_EnableSystemData */



#if CE_ARTLIB_EnableChunkWriting && CE_ARTLIB_EnableSystemData
/*****************************************************************************
 *****************************************************************************
 **  D A T A   I D   W R I T I N G   M A P P E R   R O U T I N E S          **
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * Open the DataID writing stream.  Returns TRUE if successful.  The
 * Data ID is stuffed into "dest" on input.
 */

static BOOL DataIDWriterOpen (MapperActionPointer ActionPntr)
{
  LE_DATA_DataId          DataID;
  long                    DataSize;
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + ActionPntr->streamID;

  MyStreamPntr->mapperData.writeDataID.dataID = DataID =
    (LE_DATA_DataId) ActionPntr->dest;

  // Remember size for our use.
  DataSize = LE_DATA_GetLoadedSize (DataID);
  MyStreamPntr->mapperData.writeDataID.allocatedSize = DataSize;
  MyStreamPntr->mapperData.writeDataID.usedSize = 0;

  // Size of the item for caller.
  ActionPntr->bytes = DataSize;

  // Keep a reference on it, so it doesn't get unloaded while
  // we are processing the chunk.

  LE_DATA_AddRef (DataID);

  // Store a pointer to the start of the data.  Should stay valid until
  // we unref it, or until it is resized (growth errors set it to NULL).
  // Also mark it as dirty so that the system knows it contains new data
  // (and will save it when the item is unloaded).

  MyStreamPntr->mapperData.writeDataID.bytePntr =
    (LPBYTE) LE_DATA_UseDirty (DataID);

  if (MyStreamPntr->mapperData.writeDataID.bytePntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " DataIDWriterOpen: "
    "Can't get pointer to data for DataID $%08X in stream %d.\r\n",
    (int) MyStreamPntr->mapperData.readDataID.dataID, (int) ActionPntr->streamID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    LE_DATA_RemoveRef (DataID);
    MyStreamPntr->mapperData.writeDataID.dataID = LE_DATA_EmptyItem;
    return FALSE; // Unable to access DataID.
  }

  // Success!
  return TRUE;
}



/*****************************************************************************
 * Write some data from the given buffer at the given offset.
 */

static BOOL DataIDWriterWrite (MapperActionPointer ActionPntr)
{
#if CE_ARTLIB_EnableDebugMode
  char                    Description [2048];
#endif
  UNS32                   AmountToWrite;
  LI_CHUNK_StreamPointer  MyStreamPntr;
  UNS32                   PastEndOffset;

  MyStreamPntr = StreamArray + ActionPntr->streamID;
  AmountToWrite = ActionPntr->bytes;
  PastEndOffset = ActionPntr->offset + AmountToWrite;
  ActionPntr->bytes = 0;  // In case of an error exit.

  if (MyStreamPntr->mapperData.writeDataID.bytePntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " DataIDWriterWrite: "
      "Data isn't available due to previous errors in growing %s.\r\n",
      LE_DATA_DescString (MyStreamPntr->mapperData.writeDataID.dataID,
      Description, sizeof (Description)));
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE; // Need an open DataID.  Can go NULL if errors happen.
  }

#if CE_ARTLIB_EnableDebugMode
  if (ActionPntr->dest == NULL)
  {
    LE_ERROR_ErrorMsg (__FILE__ " DataIDWriterWrite: "
    "Buffer pointer is NULL!\r\n");
    return FALSE; // Need a buffer to write from.
  }
#endif

  // Grow the DataID if more space is needed.

  if (MyStreamPntr->mapperData.writeDataID.allocatedSize < PastEndOffset)
  {
    if (!LE_DATA_Resize (MyStreamPntr->mapperData.writeDataID.dataID,
    2 * PastEndOffset))
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " DataIDWriterWrite: "
      "While writing to stream %u, was unable to increase allocated size to %u bytes for %s.\r\n",
      (unsigned int) ActionPntr->streamID,
      (unsigned int) (2 * PastEndOffset),
      LE_DATA_DescString (MyStreamPntr->mapperData.writeDataID.dataID,
      Description, sizeof (Description)));
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      MyStreamPntr->mapperData.writeDataID.bytePntr = NULL;
      return FALSE; // Out of memory.
    }

    MyStreamPntr->mapperData.writeDataID.bytePntr = (LPBYTE)
      LE_DATA_UseDirty (MyStreamPntr->mapperData.writeDataID.dataID);

    if (MyStreamPntr->mapperData.writeDataID.bytePntr == NULL)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " DataIDWriterWrite: "
      "Unable to get data pointer for stream %u, which uses %s.\r\n",
      (unsigned int) ActionPntr->streamID,
      LE_DATA_DescString (MyStreamPntr->mapperData.writeDataID.dataID,
      Description, sizeof (Description)));
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return FALSE; // Unable to access memory.
    }

    MyStreamPntr->mapperData.writeDataID.allocatedSize =
      LE_DATA_GetCurrentSize (MyStreamPntr->mapperData.writeDataID.dataID);

    if (MyStreamPntr->mapperData.writeDataID.allocatedSize < PastEndOffset)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " DataIDWriterWrite: "
      "Still not enough space in stream %u, for %u bytes, in %s.\r\n",
      (unsigned int) ActionPntr->streamID,
      (unsigned int) PastEndOffset,
      LE_DATA_DescString (MyStreamPntr->mapperData.writeDataID.dataID,
      Description, sizeof (Description)));
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return FALSE; // Still not enough space, give up.
    }
  }

  // Write the requested data.

  memcpy (MyStreamPntr->mapperData.writeDataID.bytePntr + ActionPntr->offset,
    ActionPntr->dest, AmountToWrite);

  // Update the used size.

  if (MyStreamPntr->mapperData.writeDataID.usedSize < PastEndOffset)
    MyStreamPntr->mapperData.writeDataID.usedSize = PastEndOffset;

  return TRUE;
}



/*****************************************************************************
 * Close the DataID writing stream's data item.
 */

static BOOL DataIDWriterClose (MapperActionPointer ActionPntr)
{
#if CE_ARTLIB_EnableDebugMode
  char                    Description [2048];
#endif
  LI_CHUNK_StreamPointer  MyStreamPntr;
  BOOL                    ReturnCode = TRUE;

  MyStreamPntr = StreamArray + ActionPntr->streamID;

  if (MyStreamPntr->mapperData.writeDataID.dataID != LE_DATA_EmptyItem)
  {
    // Truncate the DataID to only use the space actually needed.

    if (MyStreamPntr->mapperData.writeDataID.usedSize > 0 &&
    MyStreamPntr->mapperData.writeDataID.usedSize <
    MyStreamPntr->mapperData.writeDataID.allocatedSize)
    {
      if (!LE_DATA_Resize (MyStreamPntr->mapperData.writeDataID.dataID,
      MyStreamPntr->mapperData.writeDataID.usedSize))
      {
  #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " DataIDWriterClose: "
        "While closing stream %u, was unable to shrink DataID from %u bytes to %u, for %s.\r\n",
        (unsigned int) ActionPntr->streamID,
        (unsigned int) MyStreamPntr->mapperData.writeDataID.allocatedSize,
        (unsigned int) MyStreamPntr->mapperData.writeDataID.usedSize,
        LE_DATA_DescString (MyStreamPntr->mapperData.writeDataID.dataID,
        Description, sizeof (Description)));
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
        ReturnCode = FALSE;
      }
    }
#if CE_ARTLIB_EnableDebugMode
    else if (MyStreamPntr->mapperData.writeDataID.usedSize >
    MyStreamPntr->mapperData.writeDataID.allocatedSize)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, __FILE__ " DataIDWriterClose: "
      "Bug!  While closing stream %u, allocated size is %u, which is less than the used size of %u for %s.\r\n",
      (unsigned int) ActionPntr->streamID,
      (unsigned int) MyStreamPntr->mapperData.writeDataID.allocatedSize,
      (unsigned int) MyStreamPntr->mapperData.writeDataID.usedSize,
      LE_DATA_DescString (MyStreamPntr->mapperData.writeDataID.dataID,
      Description, sizeof (Description)));
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);

      ReturnCode = FALSE;
    }
#endif

    // We have finished using it, decrement the reference count.

    LE_DATA_RemoveRef (MyStreamPntr->mapperData.writeDataID.dataID);
  }

  MyStreamPntr->mapperData.writeDataID.dataID = LE_DATA_EmptyItem;
  MyStreamPntr->mapperData.writeDataID.bytePntr = NULL;

  return ReturnCode;
}



/*****************************************************************************
 * The entry point for the writeable DataID mapper.  Execute the command.
 */

BOOL LI_CHUNK_DataIDWriterMapper (MapperActionPointer Action)
{
#if CE_ARTLIB_EnableDebugMode
  if (Action == NULL || Action->streamID < 0 ||
  Action->streamID >= CE_ARTLIB_MaxOpenChunkStreams)
  {
    LE_ERROR_ErrorMsg ("LI_CHUNK_DataIDWriterMapper: "
    "Invalid parameters.\r\n");
    return FALSE; // Being called with garbage.
  }
#endif

  switch (Action->action)
  {
  case MAPPER_OPEN:
    return DataIDWriterOpen (Action);

  case MAPPER_WRITE:
    return DataIDWriterWrite (Action);

  case MAPPER_CLOSE:
    return DataIDWriterClose (Action);
  }

#if CE_ARTLIB_EnableDebugMode
  LE_ERROR_DebugMsg ("LI_CHUNK_DataIDWriterMapper: Unknown mapper action code.\r\n",
    LE_ERROR_DebugMsgToFileAndScreen);
#endif

  return FALSE;
}
#endif // CE_ARTLIB_EnableChunkWriting && CE_ARTLIB_EnableSystemData



/*****************************************************************************
 *****************************************************************************
 **  G E N E R I C   S T R E A M   R O U T I N E S                          **
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * Open up a new stream.  Returns a negative number if something goes wrong.
 */

static LE_CHUNK_StreamID OpenStream (MapperKind KindOfMapper, void *OpenNameOrID)
{
  MapperActionRecord      MapperCommand;
  LE_CHUNK_StreamID       StreamIndex;
  LI_CHUNK_StreamPointer  StreamPntr;
  BOOL                    SuccessFlag;

  // First get exclusive access to the stream array, we don't want other
  // threads to allocate a thread while we are searching through it for
  // a free one.  Threads that are using the streams are OK, and it is
  // even OK to close them while we are searching for an unused one.

  if (StreamAllocCS == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_ErrorMsg (__FILE__ " OpenStream: Called before critical section initialised!\r\n");
#endif
    return -1;
  }

  EnterCriticalSection (StreamAllocCS);

  // Look for an unused stream entry in the array.

  for (StreamIndex = 0; StreamIndex < CE_ARTLIB_MaxOpenChunkStreams; StreamIndex++)
  {
    if (!StreamArray[StreamIndex].inUse )
      break; // Found a free stream!
  }

  if (StreamIndex >= CE_ARTLIB_MaxOpenChunkStreams)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg (__FILE__ " OpenStream: Ran out of free streams",
    LE_ERROR_DebugMsgToFileAndScreen);
#endif
    LeaveCriticalSection (StreamAllocCS);
    return -2; // Ran out of streams.
  }

  // Initialise the new stream.

  StreamPntr = StreamArray + StreamIndex;

  memset (StreamPntr, 0, sizeof (LI_CHUNK_StreamRecord));
  StreamPntr->inUse = TRUE;  // Claim this stream for our use.
  LeaveCriticalSection (StreamAllocCS); // Safe now that stream has been claimed.
  StreamPntr->mapperKind = KindOfMapper;

  // Try opening the underlying file or whatever thing it is.

  memset (&MapperCommand, 0, sizeof (MapperCommand));
  MapperCommand.action = MAPPER_OPEN;
  MapperCommand.streamID = StreamIndex;
  MapperCommand.dest = OpenNameOrID;

  SuccessFlag = (MapperFunctionsArray [KindOfMapper]) (&MapperCommand);

  if (!SuccessFlag)
  {
    StreamPntr->inUse = FALSE;
    return -3; // Failed to open the file or data item.
  }

  // The stream starts out in the front of the top level chunk, which
  // is an imaginary chunk which contains the whole file contents.

  StreamPntr->chunkEnd[0] = MapperCommand.bytes;

  return StreamIndex;
}



/*****************************************************************************
 * Close an existing stream's file.  Returns TRUE if no errors, FALSE if
 * something went wrong (but the stream gets closed anyways).
 */

static BOOL CloseStream (LE_CHUNK_StreamID StreamID)
{
  MapperActionRecord      MapperCommand;
  LI_CHUNK_StreamPointer  MyStreamPntr;
  BOOL                    SuccessFlag;

  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams)
    goto ErrorExit;  // Invalid stream ID.

  MyStreamPntr = StreamArray + StreamID;

  if (!MyStreamPntr->inUse)
    return FALSE;  // This stream isn't open.

  if (MyStreamPntr->mapperKind >= MAPPER_KIND_MAX)
    goto ErrorExit;  // This stream has been corrupted.

  // Send the close file command.

  memset (&MapperCommand, 0, sizeof (MapperCommand));
  MapperCommand.action = MAPPER_CLOSE;
  MapperCommand.streamID = StreamID;

  SuccessFlag = (MapperFunctionsArray [MyStreamPntr->mapperKind]) (&MapperCommand);
  MyStreamPntr->inUse = FALSE;
  return SuccessFlag;

ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  LE_ERROR_ErrorMsg (__FILE__ " CloseStream: Bad stream ID or corrupted stream info.\r\n");
#endif
  return FALSE;
}



#if CE_ARTLIB_EnableChunkWriting
/*****************************************************************************
 *****************************************************************************
 **  K E E P I N G   T R A C K   O F   W R I T I N G   C H U N K S          **
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * Start writing a new chunk, which will be a child of the current chunk
 * (descends a level of nesting).  The start of the chunk will be remembered,
 * as being at the current position in the file.  When you finish the chunk,
 * the size field will be written at the start position.  Returns TRUE if
 * successful.
 */

BOOL LE_CHUNK_StartChunk (LE_CHUNK_StreamID StreamID, LE_CHUNK_ID ChunkID)
{
  UNS8                    Level;
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + StreamID;

#if CE_ARTLIB_EnableDebugMode
  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams ||
  (ChunkID & 0x7F) == 0 || !MyStreamPntr->inUse)
  {
    LE_ERROR_ErrorMsg ("LI_CHUNK_StartChunk: "
    "Invalid parameters.\r\n");
    return FALSE; // Being called with garbage.
  }
#endif

  Level = (UNS8) (MyStreamPntr->chunkLevel + 1);
  if (Level >= CE_ARTLIB_MaxChunkNestingLevels)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_StartChunk: "
    "Nesting level %d is past the maximum for stream %d.\r\n",
    (int) Level, (int) StreamID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE; // Maximum nesting level reached.
  }

  MyStreamPntr->currentOffset += sizeof (LE_CHUNK_HeaderRecord);
  MyStreamPntr->chunkDataOffset[Level] = MyStreamPntr->currentOffset;
  MyStreamPntr->chunkEnd[Level] = 0; // Not currently used. maybe will be if we ever allow seeks during write.
  MyStreamPntr->chunkLevel = Level;
  MyStreamPntr->chunkType[Level] = ChunkID;

  return TRUE;
}



/*****************************************************************************
 * Write a bunch of bytes into the current chunk at the current level.
 * Returns TRUE if successful.
 */

BOOL LE_CHUNK_WriteChunkData (LE_CHUNK_StreamID StreamID,
void *Buffer, UNS32 WriteAmount)
{
  MapperActionRecord      MapperCommand;
  LI_CHUNK_StreamPointer  MyStreamPntr;
  BOOL                    SuccessFlag;

  MyStreamPntr = StreamArray + StreamID;

#if CE_ARTLIB_EnableDebugMode
  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams ||
  !MyStreamPntr->inUse)
  {
    LE_ERROR_ErrorMsg ("LI_CHUNK_WriteChunkData: "
    "Invalid parameters.\r\n");
    return FALSE; // Being called with garbage.
  }
#endif

  memset (&MapperCommand, 0, sizeof (MapperCommand));
  MapperCommand.action = MAPPER_WRITE;
  MapperCommand.streamID = StreamID;
  MapperCommand.offset = MyStreamPntr->currentOffset;
  MapperCommand.bytes = WriteAmount;
  MapperCommand.dest = Buffer;

  SuccessFlag = (MapperFunctionsArray [MyStreamPntr->mapperKind]) (&MapperCommand);

  if (!SuccessFlag)
    return FALSE;

  // Advance our offset so that following data comes after.  This also
  // indirectly specifies the chunk size for when it is finished.

  MyStreamPntr->currentOffset += WriteAmount;

  return TRUE;
}



/*****************************************************************************
 * Write the given Artech string to the chunk.  Includes the NUL at the end.
 * Returns TRUE if successful.
 */

BOOL LE_CHUNK_WriteChunkACHARString (LE_CHUNK_StreamID StreamID, ACHAR *StringPntr)
{
  ACHAR Letter;
  int   StringByteLength;

  if (StringPntr == NULL)
  {
    Letter = 0; // NUL for end of an empty string.
    StringPntr = &Letter;
  }

  StringByteLength = AStrByteSize (StringPntr);

  return LE_CHUNK_WriteChunkData (StreamID, StringPntr, StringByteLength);
}



/*****************************************************************************
 * Close off a chunk.  Seeks back to the header and fills in the chunk
 * size (assuming that the current position marks the end of the chunk).
 * Then goes up one nesting level and puts the file position back to the
 * end of the chunk we just finished, which should be in the middle of
 * the parent chunk, where more children can be added.
 */

BOOL LE_CHUNK_FinishChunk (LE_CHUNK_StreamID StreamID)
{
  UNS8                    Level;
  MapperActionRecord      MapperCommand;
  LI_CHUNK_StreamPointer  MyStreamPntr;
  LE_CHUNK_HeaderRecord   NewChunkHeader;
  BOOL                    ReturnCode = FALSE;

  MyStreamPntr = StreamArray + StreamID;

#if CE_ARTLIB_EnableDebugMode
  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams ||
  !MyStreamPntr->inUse)
  {
    LE_ERROR_ErrorMsg ("LI_CHUNK_FinishChunk: "
    "Invalid parameters.\r\n");
    return FALSE; // Being called with garbage.
  }
#endif

  Level = MyStreamPntr->chunkLevel;
#if CE_ARTLIB_EnableDebugMode
  if (Level <= 0 /* Level 0 is the whole file - no chunk header */)
  {
    LE_ERROR_ErrorMsg ("LI_CHUNK_FinishChunk: "
    "Finish called more often than Start.\r\n");
    return FALSE; // Too many FinishChunks.
  }
#endif

#if CE_ARTLIB_EnableDebugMode
  if (MyStreamPntr->currentOffset < MyStreamPntr->chunkDataOffset[Level])
  {
    LE_ERROR_ErrorMsg ("LI_CHUNK_FinishChunk: "
    "End of chunk is before the beginning!\r\n");
    goto ErrorExit; // Chunk end position or current position is wrong.
  }
#endif

  // Create a chunk header now that we know the size.

  NewChunkHeader.chunkSize =
    MyStreamPntr->currentOffset -
    MyStreamPntr->chunkDataOffset[Level] +
    sizeof (LE_CHUNK_HeaderRecord);  // Size includes header size.

  NewChunkHeader.chunkID = MyStreamPntr->chunkType[Level];

  // Write out the chunk header at the appropriate position.

  memset (&MapperCommand, 0, sizeof (MapperCommand));
  MapperCommand.action = MAPPER_WRITE;
  MapperCommand.streamID = StreamID;
  MapperCommand.offset =
    MyStreamPntr->chunkDataOffset[Level] - sizeof (LE_CHUNK_HeaderRecord);
  MapperCommand.bytes = sizeof (NewChunkHeader);
  MapperCommand.dest = &NewChunkHeader;

  ReturnCode = (MapperFunctionsArray [MyStreamPntr->mapperKind]) (&MapperCommand);

#if CE_ARTLIB_EnableDebugMode
ErrorExit: // Only used in debug mode.
#endif

  MyStreamPntr->chunkLevel--;  // Close off the level, even if writing failed.
  return ReturnCode;
}
#endif /* CE_ARTLIB_EnableChunkWriting */



/*****************************************************************************
 *****************************************************************************
 **  K E E P I N G   T R A C K   O F   R E A D I N G   C H U N K S          **
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * Go down into a chunk.  If FindChunkID is LE_CHUNK_ID_NULL_STANDARD then it
 * will go down the next available chunk.  Otherwise, it will skip over chunks
 * (ignoring child chunks) until it finds a chunk with the given chunk ID.
 * If it succeeds, it returns the chunk ID, if it fails (ran out of chunks or
 * I/O error) then it returns LE_CHUNK_ID_NULL_STANDARD.  Also, if it succeeds
 * and ReturnedChunkDataSize isn't NULL then it will write the size of the
 * data in the chunk (doesn't include 4 byte chunk header) into the pointed
 * to UNS32 variable.
 */

LE_CHUNK_ID LE_CHUNK_Descend (LE_CHUNK_StreamID StreamID,
LE_CHUNK_ID FindChunkID, UNS32 *ReturnedChunkDataSize)
{
  UNS32                   EndLimit;
  UNS8                    Level;
  MapperActionRecord      MapperCommand;
  LE_CHUNK_HeaderPointer  ChunkHeaderPntr = NULL;
  LI_CHUNK_StreamPointer  MyStreamPntr;
  BOOL                    ReturnCode;

  MyStreamPntr = StreamArray + StreamID;

#if CE_ARTLIB_EnableDebugMode
  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams ||
  !MyStreamPntr->inUse)
  {
    LE_ERROR_ErrorMsg ("LE_CHUNK_Descend: "
      "Invalid parameters.\r\n");
    return LE_CHUNK_ID_NULL_STANDARD; // Being called with garbage.
  }
#endif

  Level = MyStreamPntr->chunkLevel;
  if (Level >= CE_ARTLIB_MaxChunkNestingLevels - 1)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_Descend: "
    "Can't descend into a chunk because the nesting level of %d is "
    "at the maximum for stream %d.\r\n",
    (int) Level, (int) StreamID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return LE_CHUNK_ID_NULL_STANDARD; // Too deep.
  }

  EndLimit = MyStreamPntr->chunkEnd[Level];

  // Search for the desired chunk, advance the current position until
  // it is found.  Aborts if it hits the end of the parent chunk or
  // if something goes wrong.  Only exits the loop if successful.

  while (TRUE)
  {
    if (MyStreamPntr->currentOffset + sizeof (LE_CHUNK_HeaderRecord) > EndLimit)
      return LE_CHUNK_ID_NULL_STANDARD; // Didn't find the chunk.

    // Read the chunk header at the current position.

    memset (&MapperCommand, 0, sizeof (MapperCommand));
    MapperCommand.action = MAPPER_MAP;
    MapperCommand.streamID = StreamID;
    MapperCommand.offset = MyStreamPntr->currentOffset;
    MapperCommand.bytes = sizeof (LE_CHUNK_HeaderRecord);

    ReturnCode = (MapperFunctionsArray [MyStreamPntr->mapperKind]) (&MapperCommand);
    ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) MapperCommand.dest;

    if (!ReturnCode || MapperCommand.bytes != sizeof (LE_CHUNK_HeaderRecord))
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_Descend: "
      "IO error while reading data for chunk at offset %d in stream %d.\r\n",
      (int) MyStreamPntr->currentOffset, (int) StreamID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return LE_CHUNK_ID_NULL_STANDARD; // I/O error happened.
    }

    if (ChunkHeaderPntr->chunkSize < sizeof (LE_CHUNK_HeaderRecord) ||
    ChunkHeaderPntr->chunkSize + MyStreamPntr->currentOffset > EndLimit)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_Descend: "
      "Corrupt data (size field is out of range) while reading data for "
      "chunk at offset %d in stream %d.\r\n",
      (int) MyStreamPntr->currentOffset, (int) StreamID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return LE_CHUNK_ID_NULL_STANDARD; // Corrupt data happened.
    }

    // Is this the chunk we are looking for?

    if (FindChunkID == LE_CHUNK_ID_NULL_STANDARD ||
    FindChunkID == ChunkHeaderPntr->chunkID)
      break; // Yes!

    // No, so advance to the next chunk.

    MyStreamPntr->currentOffset += ChunkHeaderPntr->chunkSize;
  }

  // OK, found the chunk we want, and its header has been read in.
  // Check a few things and return the info to the user and start a new level.

  if ((MyStreamPntr->chunkEnd[Level+1] =
  MyStreamPntr->currentOffset + ChunkHeaderPntr->chunkSize) > EndLimit)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_Descend: "
    "Bad data - chunk at offset %d runs past end of parent chunk in stream %d.\r\n",
    (int) MyStreamPntr->currentOffset, (int) StreamID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return LE_CHUNK_ID_NULL_STANDARD; // Chunk runs past end of parent.
  }

  if ((ChunkHeaderPntr->chunkID & 0x7F) == 0)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_Descend: "
    "Bad data - chunk has invalid ID of %d at offset %d in stream %d.\r\n",
    (int) ChunkHeaderPntr->chunkID, (int) MyStreamPntr->currentOffset, (int) StreamID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return LE_CHUNK_ID_NULL_STANDARD; // Chunk has NULL ID.
  }

  MyStreamPntr->currentOffset += sizeof (LE_CHUNK_HeaderRecord); // Advance to data.
  Level++; // No more error exits possible at this point, so OK to increase level.
  MyStreamPntr->chunkLevel = Level;
  MyStreamPntr->chunkDataOffset[Level] = MyStreamPntr->currentOffset;
  MyStreamPntr->chunkType [Level] = (LE_CHUNK_ID) ChunkHeaderPntr->chunkID;

  if (ReturnedChunkDataSize != NULL)
    *ReturnedChunkDataSize =
    ChunkHeaderPntr->chunkSize - sizeof (LE_CHUNK_HeaderRecord);

  return (LE_CHUNK_ID) ChunkHeaderPntr->chunkID;
}



/*****************************************************************************
 * Ascend out of a chunk.  This leaves the file positioned just after the end
 * of the chunk you ascended out of.  Returns TRUE if successful.
 */

BOOL LE_CHUNK_Ascend (LE_CHUNK_StreamID StreamID)
{
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + StreamID;

#if CE_ARTLIB_EnableDebugMode
  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams ||
  !MyStreamPntr->inUse)
  {
    LE_ERROR_ErrorMsg ("LE_CHUNK_Ascend: "
      "Invalid parameters.\r\n");
    return FALSE; // Being called with garbage.
  }
#endif

#if CE_ARTLIB_EnableDebugMode
  if (MyStreamPntr->chunkLevel <= 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_Ascend: "
    "Can't ascend any more, already at the top level in stream %d.  "
    "Maybe you really want to close this stream?\r\n",
    (int) StreamID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE; // Already at top level.
  }
#endif

  MyStreamPntr->currentOffset = MyStreamPntr->chunkEnd[MyStreamPntr->chunkLevel];
  MyStreamPntr->chunkLevel--;

  return TRUE;
}



/*****************************************************************************
 * Returns the current position of the stream, zero on failure.
 */

UNS32 LE_CHUNK_GetCurrentFilePosition (LE_CHUNK_StreamID StreamID)
{
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + StreamID;

#if CE_ARTLIB_EnableDebugMode
  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams ||
  !MyStreamPntr->inUse)
  {
    LE_ERROR_ErrorMsg ("LE_CHUNK_GetCurrentFilePosition: "
      "Invalid parameters.\r\n");
    return 0; // Being called with garbage.
  }
#endif

  return MyStreamPntr->currentOffset;
}



/*****************************************************************************
 * Sets the current position of the stream, zero on failure.  It restricts you
 * to positions inside the current chunk's data area.  Normally you won't need
 * this.  But it could be useful when scanning a bunch of chunks to redo the
 * scan (save the starting point, search for your chunk with Descend, ascend
 * out of it if you found it, set the file position back to the starting point
 * and do another scan).
 * Returns FALSE if it fails (I/O error or out of bounds).
 */

BOOL LE_CHUNK_SetCurrentFilePosition (LE_CHUNK_StreamID StreamID, UNS32 Position)
{
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + StreamID;

#if CE_ARTLIB_EnableDebugMode
  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams ||
  !MyStreamPntr->inUse)
  {
    LE_ERROR_ErrorMsg ("LE_CHUNK_SetCurrentFilePosition: "
      "Invalid parameters.\r\n");
    return FALSE; // Being called with garbage.
  }
#endif

  // See if the desired offset is within the current chunk.

  if (Position < MyStreamPntr->chunkDataOffset[MyStreamPntr->chunkLevel] ||
  Position > MyStreamPntr->chunkEnd[MyStreamPntr->chunkLevel])
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_SetCurrentFilePosition: "
    "Position of %d is outside the current chunk in stream %d.  "
    "It should be between %d and %d inclusive.\r\n",
    (int) Position, (int) StreamID,
    (int) MyStreamPntr->chunkDataOffset[MyStreamPntr->chunkLevel],
    (int) MyStreamPntr->chunkEnd[MyStreamPntr->chunkLevel]);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  MyStreamPntr->currentOffset = Position;
  return TRUE;
}



/*****************************************************************************
 * Returns the current level of nesting.  A freshly opened file starts at
 * zero.  The level goes up 1 every time you descend into a chunk, and goes
 * down 1 when you ascend.  Returns 0 on error.
 */

UNS8 LE_CHUNK_GetCurrentLevel (LE_CHUNK_StreamID StreamID)
{
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + StreamID;

#if CE_ARTLIB_EnableDebugMode
  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams ||
  !MyStreamPntr->inUse)
  {
    LE_ERROR_ErrorMsg ("LE_CHUNK_GetCurrentLevel: "
      "Invalid parameters.\r\n");
    return 0; // Being called with garbage.
  }
#endif

  return MyStreamPntr->chunkLevel;
}



/*****************************************************************************
 * Returns the Chunk ID for the given currently open level.  Level zero is
 * the whole file and doesn't have a chunk ID, other levels are the currently
 * descended into chunk.
 */

LE_CHUNK_ID LE_CHUNK_GetIDForLevel (LE_CHUNK_StreamID StreamID, UNS8 Level)
{
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + StreamID;

#if CE_ARTLIB_EnableDebugMode
  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams ||
  !MyStreamPntr->inUse || Level > MyStreamPntr->chunkLevel)
  {
    LE_ERROR_ErrorMsg ("LE_CHUNK_GetIDForLevel: Invalid parameters.\r\n");
    return LE_CHUNK_ID_NULL_STANDARD;
  }
#endif

  return MyStreamPntr->chunkType [Level];
}



/*****************************************************************************
 * Returns the size of the chunk currently descended into at the given level,
 * size doesn't include the header size, just the size of the data.
 */

UNS32 LE_CHUNK_GetDataSizeForLevel (LE_CHUNK_StreamID StreamID, UNS8 Level)
{
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + StreamID;

#if CE_ARTLIB_EnableDebugMode
  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams ||
  !MyStreamPntr->inUse || Level > MyStreamPntr->chunkLevel)
  {
    LE_ERROR_ErrorMsg ("LE_CHUNK_GetDataSizeForLevel: Invalid parameters.\r\n");
    return 0;
  }
#endif

  return MyStreamPntr->chunkEnd[Level] - MyStreamPntr->chunkDataOffset[Level];
}



/*****************************************************************************
 * Returns the file position of the start of the data in the chunk at
 * the given level.
 */

UNS32 LE_CHUNK_GetDataStartOffsetForLevel (LE_CHUNK_StreamID StreamID, UNS8 Level)
{
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + StreamID;

#if CE_ARTLIB_EnableDebugMode
  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams ||
  !MyStreamPntr->inUse || Level > MyStreamPntr->chunkLevel)
  {
    LE_ERROR_ErrorMsg ("LE_CHUNK_GetDataStartOffsetForLevel: Invalid parameters.\r\n");
    return 0;
  }
#endif

  return MyStreamPntr->chunkDataOffset[Level];
}



/*****************************************************************************
 * Read some chunk data into an internal buffer and return a pointer to the
 * buffer.  If the file is a memory based file (a data item), then the pointer
 * points directly into the memory and no copying is done.
 * If AmountActuallyRead is not NULL then the pointed to UNS32 gets filled
 * with the number of bytes read (can be less than expected if internal
 * buffers are small or if the chunk has less remaining data than the
 * requested amount or a read error happens).  Returns a NULL pointer if
 * really bad things happen.
 */

void *LE_CHUNK_MapChunkData (LE_CHUNK_StreamID StreamID,
UNS32 ReadAmount, UNS32 *AmountActuallyReadPntr)
{
  MapperActionRecord      MapperCommand;
  LI_CHUNK_StreamPointer  MyStreamPntr;
  void                   *ReturnPointer = NULL;
  UNS32                   ReturnSize = 0;

  MyStreamPntr = StreamArray + StreamID;

#if CE_ARTLIB_EnableDebugMode
  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams ||
  !MyStreamPntr->inUse)
  {
    LE_ERROR_ErrorMsg ("LE_CHUNK_MapChunkData: "
      "Invalid parameters.\r\n");
    goto ErrorExit;
  }
#endif

  // Cut down the amount to read to make it fit within
  // the current chunk data area.

  if (ReadAmount > MyStreamPntr->chunkEnd[MyStreamPntr->chunkLevel] -
  MyStreamPntr->currentOffset)
  {
    ReadAmount = MyStreamPntr->chunkEnd[MyStreamPntr->chunkLevel] -
    MyStreamPntr->currentOffset;
  }

  // Read the requested data, or at least point to it.

  memset (&MapperCommand, 0, sizeof (MapperCommand));
  MapperCommand.action = MAPPER_MAP;
  MapperCommand.streamID = StreamID;
  MapperCommand.offset = MyStreamPntr->currentOffset;
  MapperCommand.bytes = ReadAmount;

  if (!(MapperFunctionsArray [MyStreamPntr->mapperKind]) (&MapperCommand))
    goto ErrorExit; // I/O error or something bad.

  ReturnPointer = MapperCommand.dest;  // Pointer to internal buffer.
  MyStreamPntr->currentOffset += MapperCommand.bytes; // Advance past amount read.
  ReturnSize = MapperCommand.bytes;  // Amount that was actually read.

ErrorExit: // And normal exit goes through here too.
  if (AmountActuallyReadPntr != NULL)
    *AmountActuallyReadPntr = ReturnSize;
  return ReturnPointer;
}




/*****************************************************************************
 * Read some chunk data into a user provided buffer and return the number of
 * bytes read.  The number of bytes read can be less than expected if the
 * chunk has less remaining data than the requested amount or a read error
 * happens.  You can also pass in a NULL buffer pointer, in which case it
 * simulates reading the data.  Returns zero on really bad errors.
 */

UNS32 LE_CHUNK_ReadChunkData (LE_CHUNK_StreamID StreamID, void *Buffer, UNS32 ReadAmount)
{
  MapperActionRecord      MapperCommand;
  LI_CHUNK_StreamPointer  MyStreamPntr;
  UNS32                   ReturnSize = 0;

  MyStreamPntr = StreamArray + StreamID;

#if CE_ARTLIB_EnableDebugMode
  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams ||
  !MyStreamPntr->inUse)
  {
    LE_ERROR_ErrorMsg ("LE_CHUNK_ReadChunkData: "
      "Invalid parameters.\r\n");
    goto ErrorExit;
  }
#endif

  // Cut down the amount to read to make it fit within
  // the current chunk data area.

  if (ReadAmount > MyStreamPntr->chunkEnd[MyStreamPntr->chunkLevel] -
  MyStreamPntr->currentOffset)
  {
    ReadAmount = MyStreamPntr->chunkEnd[MyStreamPntr->chunkLevel] -
    MyStreamPntr->currentOffset;
  }

  // Read the requested data.

  memset (&MapperCommand, 0, sizeof (MapperCommand));
  MapperCommand.action = MAPPER_READ;
  MapperCommand.streamID = StreamID;
  MapperCommand.offset = MyStreamPntr->currentOffset;
  MapperCommand.bytes = ReadAmount;
  MapperCommand.dest = Buffer;

  if (!(MapperFunctionsArray [MyStreamPntr->mapperKind]) (&MapperCommand))
    goto ErrorExit; // I/O error or something bad.

  MyStreamPntr->currentOffset += MapperCommand.bytes; // Advance past amount read.
  ReturnSize = MapperCommand.bytes;  // Amount that was actually read.

ErrorExit: // Normal exit goes through here too.
  return ReturnSize;
}



/*****************************************************************************
 *****************************************************************************
 **  H I G H E R   L E V E L   S T R E A M   R O U T I N E S                **
 *****************************************************************************
 *****************************************************************************/

#if CE_ARTLIB_EnableSystemData
 /*****************************************************************************
 * Open up a new stream for reading from a Data ID.  Returns a negative stream
 * number if it fails.
 */

LE_CHUNK_StreamID LE_CHUNK_ReadFromDataID (LE_DATA_DataId DataID)
{
  return OpenStream (MAPPER_FOR_DATA_ITEM_READING, (void *) DataID);
}
#endif /* CE_ARTLIB_EnableSystemData */



/*****************************************************************************
 * Open up a new stream for reading from a file.  Returns a negative stream
 * number if it fails.
 */

LE_CHUNK_StreamID LE_CHUNK_ReadFromFile (char *FileName)
{
  return OpenStream (MAPPER_FOR_FILE_READING, FileName);
}



#if CE_ARTLIB_EnableChunkWriting
/*****************************************************************************
 * Open up a new stream for writing to a file.  Returns a negative stream
 * number if it fails.
 */

LE_CHUNK_StreamID LE_CHUNK_WriteToFile (char *FileName)
{
  return OpenStream (MAPPER_FOR_FILE_WRITING, FileName);
}
#endif /* CE_ARTLIB_EnableChunkWriting */



#if CE_ARTLIB_EnableChunkWriting && CE_ARTLIB_EnableSystemData
/*****************************************************************************
 * Open up a new stream for writing to a DataID.  Returns a negative stream
 * number if it fails.
 */

LE_CHUNK_StreamID LE_CHUNK_WriteToDataID (LE_DATA_DataId DataID)
{
  return OpenStream (MAPPER_FOR_DATA_ITEM_WRITING, (void *) DataID);
}
#endif // CE_ARTLIB_EnableChunkWriting && CE_ARTLIB_EnableSystemData



/*****************************************************************************
 * Close a read stream.  Basically not much to do beyond closing the
 * underlying file and deallocating associated buffers.
 */

BOOL LE_CHUNK_CloseReadStream (LE_CHUNK_StreamID StreamID)
{
  return CloseStream (StreamID);
}



#if CE_ARTLIB_EnableChunkWriting
/*****************************************************************************
 * Close a write stream.  This does an automatic ascend out of any nested
 * chunks (closing them off) and then closes the file.
 */

BOOL LE_CHUNK_CloseWriteStream (LE_CHUNK_StreamID StreamID)
{
  LI_CHUNK_StreamPointer  MyStreamPntr;

  MyStreamPntr = StreamArray + StreamID;

#if CE_ARTLIB_EnableDebugMode
  if (StreamID < 0 || StreamID >= CE_ARTLIB_MaxOpenChunkStreams ||
  !MyStreamPntr->inUse)
  {
    LE_ERROR_ErrorMsg ("LE_CHUNK_CloseWriteStream: "
    "Invalid parameters.\r\n");
    return FALSE; // Being called with garbage.
  }
#endif

  /* Close off pending nested chunks. */

  while (MyStreamPntr->chunkLevel > 0)
    if (!LE_CHUNK_FinishChunk (StreamID))
      break; // Stop on error.

  return CloseStream (StreamID);
}
#endif /* CE_ARTLIB_EnableChunkWriting */



/*****************************************************************************
 *****************************************************************************
 **  U T I L I T Y   C H U N K   R O U T I N E S                            **
 *****************************************************************************
 *****************************************************************************/

#if CE_ARTLIB_EnableChunkGrow && CE_ARTLIB_EnableChunkWriting && CE_ARTLIB_EnableSystemData
/*****************************************************************************
 * Change the size of a chunky DataID.  Inserts bytes or removes bytes and
 * updates the sizes of the chunks which enclose the changed one to match.
 *
 * DataID identifies the data to be changed, the underlying memory will be
 * resized to hold the new total size.  TargetChunkOffset identifies the
 * chunk to be resized by giving the position of its header within the
 * DataID (offset to get to the first byte of the header, for example
 * zero for the very first chunk in the DataID).  AmountToGrow is the number
 * of bytes to insert, use a negative value to delete bytes.  InsertOffset
 * specifes the absolute offset within the DataID where the new bytes will
 * be inserted (zeroes appear at InsertOffset and beyond) or the end of the
 * region where old ones deleted (bytes are moved down from InsertOffset
 * to fill the hole).  If it is outside the given chunk then the new bytes
 * will be added at the end of the chunk / removed from the end.  Returns
 * TRUE if successful.  FALSE if it fails (may mangle your data).
 */

BOOL LE_CHUNK_GrowChunk (LE_DATA_DataId DataID, UNS32 TargetChunkOffset,
INT32 AmountToGrow, UNS32 InsertOffset)
{
  INT32                   AmountToMove;
  BYTE                   *BasePntr;
  INT32                   BytesRemainingAfterDelete;
  LE_CHUNK_ID             ChunkID;
  LE_CHUNK_HeaderPointer  ChunkPntr;
  UNS32                   ChunkSize;
  UNS32                   CurrentOffset;
  UNS32                   FixedInsertOffset;
  UNS16                   FixedSize;
  BOOL                    ReturnCode = FALSE;
  UNS32                   TargetChunkSize;
  UNS32                   WholeSize;

  if (AmountToGrow == 0)
    return TRUE; // Nothing to do.

  WholeSize = LE_DATA_GetLoadedSize (DataID);
  if (WholeSize == 0)
    goto ErrorExit; // Error while loading the data.

  if (TargetChunkOffset + sizeof (LE_CHUNK_HeaderRecord) > WholeSize)
    goto ErrorExit; // User's chunk is outside the file.

  BasePntr = (BYTE *) LE_DATA_UseDirty (DataID);
  if (BasePntr == NULL)
    goto ErrorExit; // Unable to load data again.

  ChunkPntr = (LE_CHUNK_HeaderPointer) (BasePntr + TargetChunkOffset);
  TargetChunkSize = ChunkPntr->chunkSize;

  if (TargetChunkSize + TargetChunkOffset > WholeSize)
    goto ErrorExit; // User's chunk is partially outside the file or bad data.

  // If the insert position is outside the given chunk,
  // move it to the end of the given chunk.  This also works
  // for the default operation of adding/deleting at the end
  // of the chunk if a position of zero is given.

  if (InsertOffset > TargetChunkSize + TargetChunkOffset ||
  InsertOffset < TargetChunkOffset + sizeof (LE_CHUNK_HeaderRecord))
    FixedInsertOffset = TargetChunkSize + TargetChunkOffset;
  else // Position is in the chunk data portion.
    FixedInsertOffset = InsertOffset;

  // If deleting bytes, make sure that the user doesn't delete too much.

  if (AmountToGrow < 0)
  {
    // First figure out how much is left before the InsertOffset,
    // ignore the stuff after it for now.

    BytesRemainingAfterDelete =
      FixedInsertOffset - TargetChunkOffset + AmountToGrow;

    if (BytesRemainingAfterDelete < 0)
      goto ErrorExit; // Trying to delete too much.

    if (BytesRemainingAfterDelete >= 1 &&
    BytesRemainingAfterDelete < sizeof (LE_CHUNK_HeaderRecord))
      goto ErrorExit; // Can't partially delete the header info.

    if (BytesRemainingAfterDelete == 0 &&
    (UNS32) (-AmountToGrow) != TargetChunkSize)
      goto ErrorExit; // Deleting the header without also deleting whole chunk.

    // Note that BytesRemainingAfterDelete is zero for the special
    // case where the chunk is entirely deleted.  This means that
    // the size of that particular chunk header shouldn't be fixed up.
  }
  else // Inserting, no need to worry about deleting the chunk header.
    BytesRemainingAfterDelete = 1; // Always fix up header in insert mode.

  // Try to resize the data item to the new size, if growing.

  if (AmountToGrow > 0)
  {
    if (!LE_DATA_Resize (DataID, WholeSize + AmountToGrow))
      goto ErrorExit; // Unable to enlarge the memory space.
    BasePntr = (BYTE *) LE_DATA_Use (DataID); // Pointer can change.
    if (BasePntr == NULL) goto ErrorExit; // Something bad.
  }

  // Shuffle memory around to make a gap or remove bytes.
  // The same code even works for shrinking because the negative
  // size math actually works out correctly.

  AmountToMove = WholeSize - FixedInsertOffset;
  if (AmountToMove > 0)
  {
    memmove (
      BasePntr + (FixedInsertOffset + AmountToGrow) /* Destination */,
      BasePntr + FixedInsertOffset /* Source */,
      AmountToMove);

    if (AmountToGrow > 0)
      memset (BasePntr + FixedInsertOffset, 0, AmountToGrow); // Zero the gap.
  }

  // Update the sizes of all chunks which contain the chunk which changed
  // size.  Traverse along the stream past chunks which are before the target
  // chunk, and down into chunks which contain the target chunk.  Don't care
  // about chunks after the target chunk or which are inside it.

  CurrentOffset = 0;
  while (CurrentOffset <= TargetChunkOffset)
  {
    ChunkPntr = (LE_CHUNK_HeaderPointer) (BasePntr + CurrentOffset);
    ChunkSize = ChunkPntr->chunkSize;

    if (TargetChunkOffset < (CurrentOffset + ChunkSize) &&
    TargetChunkOffset >= CurrentOffset)
    {
      // This chunk contains the target chunk or is the target.

      // Fix up its size.  But don't fix up the size of a chunk
      // which has been completely deleted since the header
      // is actually the header of the next chunk in the stream.

      if (CurrentOffset != TargetChunkOffset ||
      BytesRemainingAfterDelete != 0)
        ChunkPntr->chunkSize = ChunkSize + AmountToGrow;

      if (CurrentOffset == TargetChunkOffset)
        break; // This is the target chunk (or was), size fixups all done.

      // Look at chunks within this chunk.  Advance past the fixed
      // size part of the current chunk and resume looking there.

      ChunkID = ChunkPntr->chunkID;

      if (ChunkID == LE_CHUNK_ID_NULL_STANDARD ||
      ChunkID >= LE_CHUNK_ID_NULL_IN_CONTEXT)
        goto ErrorExit; // This chunk doesn't contain others, where is target?

      FixedSize = LE_CHUNK_ChunkFixedDataSizes [ChunkID];
      if (FixedSize == 0xFFFF)
        goto ErrorExit; // This chunk doesn't contain others, where is target?

      // Advance to start of child chunks.
      CurrentOffset += FixedSize + sizeof (LE_CHUNK_HeaderRecord);
    }
    else // Not inside this chunk, go to next chunk.
      CurrentOffset += ChunkSize;
  }

  if (CurrentOffset != TargetChunkOffset)
    goto ErrorExit; // User specified target isn't start of a chunk.

  // Resize the data item if shrinking.

  if (AmountToGrow < 0)
  {
    if (!LE_DATA_Resize (DataID, WholeSize + AmountToGrow))
      goto ErrorExit; // Unable to shrink the memory space.
  }

  ReturnCode = TRUE;
  goto NormalExit;

ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  {
    char TempString [2048];
    sprintf (TempString, "LE_CHUNK_GrowChunk: "
      "Failed.  Was trying to add %d bytes at offset %u (changed to %u) in "
      "chunk starting at offset %u with chunk size %u in ", (int) AmountToGrow,
      (unsigned int) InsertOffset, (unsigned int) FixedInsertOffset,
      (unsigned int) TargetChunkOffset, (unsigned int) TargetChunkSize);
    LE_DATA_DescribeDataID (DataID, TempString + strlen (TempString),
      sizeof (TempString) - strlen (TempString));
    strcat (TempString, "\r\n");
    LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

NormalExit:
  return ReturnCode;
}
#endif // CE_ARTLIB_EnableChunkGrow && CE_ARTLIB_EnableChunkWriting && CE_ARTLIB_EnableSystemData



#if CE_ARTLIB_EnableChunkManipulation
/*****************************************************************************
 * Creates a new in-memory chunky item containing an empty animation
 * grouping sequence with a serial number.  The caller can specify
 * the dimensionality, or use -1 to use the default dimensionality
 * of zero (no dimensionality subchunk).  Returns LE_DATA_EmptyItem
 * if it fails.
 */

LE_DATA_DataId LE_CHUNK_CreateSequence (int Dimensionality /* = -1 */)
{
#pragma pack(push,1)
  struct EmptySequenceStruct
  {
    LE_CHUNK_HeaderRecord                 header;
    LE_SEQNCR_SequenceGroupingChunkRecord contents;
    // Note that private chunks come before public and sequence ones.
    // Dimensionality is a private chunk and dope is a public non-sequence one.
    LE_CHUNK_HeaderRecord                 dimensionalityHeader;
    LE_SEQNCR_DimensionalityChunkRecord   dimensionalityData;
    LE_CHUNK_HeaderRecord                 dopeHeader;
    LE_CHUNK_DopeDataRecord               dopeData;
  } EmptySequence, *EmptySeqPntr;
#pragma pack(pop)

  LE_DATA_DataId DataID;

  // Make an empty grouping sequence DataID in memory,
  // with a serial number subchunk to identify it.

  memset (&EmptySequence, 0, sizeof (EmptySequence));
  EmptySequence.header.chunkSize = sizeof (EmptySequence);
  EmptySequence.header.chunkID = LE_CHUNK_ID_SEQ_GROUPING;
  EmptySequence.contents.commonHeader.timeMultiple = 4;
  EmptySequence.contents.commonHeader.endingAction = LE_SEQNCR_EndingActionStop;
  EmptySequence.contents.commonHeader.endTime = 300;

  // Set up the dope data subchunk.

  EmptySequence.dopeHeader.chunkSize =
    sizeof (LE_CHUNK_DopeDataRecord) + sizeof (LE_CHUNK_HeaderRecord);
  EmptySequence.dopeHeader.chunkID = LE_CHUNK_ID_DOPE_DATA;
  EmptySequence.dopeData.serialNumber = LE_CHUNK_GetNextSerialNumber ();

  // Set up the dimensionality subchunk, or remove it if not needed.

  if (Dimensionality >= 0)
  {
    EmptySequence.dimensionalityHeader.chunkSize =
      sizeof (LE_SEQNCR_DimensionalityChunkRecord) + sizeof (LE_CHUNK_HeaderRecord);
    EmptySequence.dimensionalityHeader.chunkID = LE_SEQNCR_CHUNK_ID_DIMENSIONALITY;
    EmptySequence.dimensionalityData.dimensionality = Dimensionality;
  }
  else // Remove the dimensionality chunk.
  {
    memmove (&EmptySequence.dimensionalityHeader, &EmptySequence.dopeHeader,
      EmptySequence.dopeHeader.chunkSize);
    EmptySequence.header.chunkSize -=
      sizeof (LE_SEQNCR_DimensionalityChunkRecord) + sizeof (LE_CHUNK_HeaderRecord);
  }

  DataID = LE_DATA_AllocMemoryDataID (EmptySequence.header.chunkSize, LE_DATA_DataChunky);
  if (DataID == LE_DATA_EmptyItem)
    goto ErrorExit;

  EmptySeqPntr = (struct EmptySequenceStruct *) LE_DATA_Use (DataID);
  if (EmptySeqPntr == NULL)
    goto ErrorExit;

  memcpy (EmptySeqPntr, &EmptySequence, EmptySequence.header.chunkSize);
  return DataID;

ErrorExit:
  if (DataID != LE_DATA_EmptyItem)
    LE_DATA_FreeRuntimeDataID (DataID);
  return LE_DATA_EmptyItem;
}



/*****************************************************************************
 * Find the range of times and priorities covered by the child chunks of a
 * sequence.  DataID and SequenceChunkOffset identify the parent sequence,
 * with SequenceChunkOffset pointing to the start of the chunk header (not
 * the sequence header in the chunk, the actual start of the whole thing).
 * If RecursiveUpdate is TRUE then it will update the duration of grouping
 * type sequences with the duration of their children, and update the time
 * multiples too with the lowest time multiple.  If FALSE then it
 * just assumes that the groups have the correct time and it doesn't do
 * any recursion or modifications.  The returned values are from the start
 * of the range to just past the end of the range, so equal values mean an
 * empty range.  Returns FALSE if it fails or nothing was changed, returns
 * TRUE if values were changed (meaning that displays of the sequence or
 * subsequences need to be resized).
 */

BOOL LE_CHUNK_SizeSequence (LE_DATA_DataId DataID, UNS32 SequenceChunkOffset,
BOOL RecursiveUpdate, UNS16 *LowPriorityPntr, UNS16 *HighPriorityPntr,
INT32 *LowTimePntr, INT32 *HighTimePntr)
{
#if CE_ARTLIB_EnableDebugMode
  char                                    DescString [2048];
  INT32                                   LastStartTime = INT32_MIN;
#endif

  BYTE                                   *BasePntr;
  int                                     ChildCount = 0;
  LE_CHUNK_ID                             ChunkID;
  LE_CHUNK_HeaderPointer                  ChunkPntr;
  UNS32                                   CurrentOffset;
  INT32                                   DesiredEndTime;
  UNS32                                   EndOffset;
  UNS16                                   HighPriority = 0;
  INT32                                   HighTime = 0;
  LE_SEQNCR_SequenceIndirectChunkPointer  IndirectedSeqHeaderPntr;
  int                                     LowMultiple = 4;
  UNS16                                   LowPriority = 0;
  INT32                                   LowTime = 0;
  BOOL                                    ReturnCode = FALSE;
  LE_SEQNCR_SequenceIndirectChunkPointer  SequenceHeaderPntr;
  LE_DATA_DataId                          SubDataID;
  UNS32                                   SubOffset;
  int                                     TempTime;

  BasePntr = (BYTE *) LE_DATA_Use (DataID);
  if (BasePntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_SizeSequence: "
    "Unable to read sequence data for DataID 0x%08X.\r\n",
    (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  EndOffset = LE_DATA_GetCurrentSize (DataID);

  if (SequenceChunkOffset + sizeof (LE_CHUNK_HeaderRecord) > EndOffset)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_SizeSequence: "
    "Sequence offset of %u is past end of data at %u.\r\n",
    (unsigned int) SequenceChunkOffset, (unsigned int) EndOffset);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  ChunkPntr = (LE_CHUNK_HeaderPointer) (BasePntr + SequenceChunkOffset);
  CurrentOffset = ChunkPntr->chunkSize + SequenceChunkOffset; // End of chunk.
  if (CurrentOffset > EndOffset)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_SizeSequence: "
    "Sequence chunk runs past end of data item, probably corrupt data.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }
  EndOffset = CurrentOffset;
  CurrentOffset = SequenceChunkOffset;

  ChunkID = ChunkPntr->chunkID;

  if (ChunkID < LE_CHUNK_ID_SEQ_GROUPING || ChunkID >= LE_CHUNK_ID_SEQ_MAX)
    return FALSE; // Non-sequence chunks need not apply.

  CurrentOffset += sizeof (LE_CHUNK_HeaderRecord) +
    LE_CHUNK_ChunkFixedDataSizes[ChunkID];

  // Now examine all the children inside the given chunk, finding the
  // largest and smallest values of time and layers and time multiples.

  while (CurrentOffset < EndOffset)
  {
    ChunkPntr = (LE_CHUNK_HeaderPointer) (BasePntr + CurrentOffset);
    ChunkID = ChunkPntr->chunkID;

    if (ChunkID >= LE_CHUNK_ID_SEQ_GROUPING && ChunkID < LE_CHUNK_ID_SEQ_MAX)
    {
      SequenceHeaderPntr = (LE_SEQNCR_SequenceIndirectChunkPointer)
        (BasePntr + CurrentOffset + sizeof (LE_CHUNK_HeaderRecord));

      // Recursively update the range based on the children's ranges.
      // Since it is done in place (no chunk size changes), we can
      // continue using our offsets.

      if (RecursiveUpdate)
      {
        if (ChunkID == LE_CHUNK_ID_SEQ_INDIRECT)
        {
          SubDataID = SequenceHeaderPntr->subsequenceChunkListDataID;
          if (!SequenceHeaderPntr->commonHeader.absoluteDataIDs)
            SubDataID = LE_DATA_IdWithFileFromParent (SubDataID, DataID);
          SubOffset = 0;
        }
        else // An in-line grouping sequence.
        {
          SubDataID = DataID;
          SubOffset = CurrentOffset;
        }

        if (LE_CHUNK_SizeSequence (SubDataID, SubOffset, TRUE,
        NULL, NULL, NULL, NULL))
          ReturnCode = TRUE; // Something was changed in children.

        // Restore our pointer in case examining subsequences made
        // the system dump it from memory.  Not likely, but...

        BasePntr = (BYTE *) LE_DATA_Use (DataID);
        if (BasePntr == NULL) return FALSE;

        SequenceHeaderPntr = (LE_SEQNCR_SequenceIndirectChunkPointer)
          (BasePntr + CurrentOffset + sizeof (LE_CHUNK_HeaderRecord));

        // Copy the indirect sequence's settings from the sequence
        // it was refering to, since they don't automatically get
        // updated like they do with plain grouping sequences.

        if (ChunkID == LE_CHUNK_ID_SEQ_INDIRECT)
        {
          IndirectedSeqHeaderPntr = (LE_SEQNCR_SequenceIndirectChunkPointer)
            ((((LPBYTE) LE_DATA_Use (SubDataID)) + SubOffset +
            sizeof (LE_CHUNK_HeaderRecord)));

          DesiredEndTime =
            IndirectedSeqHeaderPntr->commonHeader.parentStartTime +
            IndirectedSeqHeaderPntr->commonHeader.endTime;

          if (SequenceHeaderPntr->commonHeader.endTime !=
          DesiredEndTime ||
          SequenceHeaderPntr->commonHeader.timeMultiple !=
          IndirectedSeqHeaderPntr->commonHeader.timeMultiple)
          {
            LE_DATA_Dirty (DataID);

            SequenceHeaderPntr->commonHeader.endTime =
              DesiredEndTime;

            SequenceHeaderPntr->commonHeader.timeMultiple =
              IndirectedSeqHeaderPntr->commonHeader.timeMultiple;

            ReturnCode = TRUE; // We changed something.
          }
        }
      }

      // Extra check for bad start times, need to be in increasing order.

#if CE_ARTLIB_EnableDebugMode
      if (SequenceHeaderPntr->commonHeader.parentStartTime < LastStartTime)
      {
        sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_SizeSequence: "
        "Parent start time of child at offset %u is %d, "
        "which is less than the previous child's start time of %d.  "
        "It needs to be in increasing order.  Please fix %s.\r\n",
        (unsigned int) CurrentOffset,
        (int) SequenceHeaderPntr->commonHeader.parentStartTime,
        (int) LastStartTime,
        LE_DATA_DescString (DataID, DescString, sizeof (DescString)));
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      }
      LastStartTime = SequenceHeaderPntr->commonHeader.parentStartTime;
#endif

      // If ending time is zero, it means it is infinitely long in
      // duration.  So, don't consider it if there is no ending.

      if (SequenceHeaderPntr->commonHeader.endTime > 0)
      {
        if (ChildCount == 0)
        {
          LowTime = SequenceHeaderPntr->commonHeader.parentStartTime;
          HighTime = SequenceHeaderPntr->commonHeader.parentStartTime +
            SequenceHeaderPntr->commonHeader.endTime;
          LowPriority = SequenceHeaderPntr->commonHeader.priority;
          HighPriority = LowPriority + 1;
          LowMultiple = SequenceHeaderPntr->commonHeader.timeMultiple;
        }
        else // Not the first time.
        {
          if (SequenceHeaderPntr->commonHeader.parentStartTime < LowTime)
            LowTime = SequenceHeaderPntr->commonHeader.parentStartTime;

          TempTime = SequenceHeaderPntr->commonHeader.parentStartTime +
            SequenceHeaderPntr->commonHeader.endTime;
          if (TempTime > HighTime)
            HighTime = TempTime;

          if (SequenceHeaderPntr->commonHeader.priority < LowPriority)
            LowPriority = SequenceHeaderPntr->commonHeader.priority;

          if (SequenceHeaderPntr->commonHeader.priority >= HighPriority)
            HighPriority = SequenceHeaderPntr->commonHeader.priority + 1;

          if ((int) SequenceHeaderPntr->commonHeader.timeMultiple <
          LowMultiple)
            LowMultiple = SequenceHeaderPntr->commonHeader.timeMultiple;
        }
        ChildCount++;
      }
    }

    CurrentOffset += ChunkPntr->chunkSize;
  }

  // Get the original sequence's header again, so we can update it
  // or just look at it.

  BasePntr = (BYTE *) LE_DATA_Use (DataID);
  ChunkPntr = (LE_CHUNK_HeaderPointer) (BasePntr + SequenceChunkOffset);
  ChunkID = ChunkPntr->chunkID;
  SequenceHeaderPntr = (LE_SEQNCR_SequenceIndirectChunkPointer)
    (BasePntr + SequenceChunkOffset + sizeof (LE_CHUNK_HeaderRecord));

  // If no children at all, just use the settings currently
  // in the sequence itself to pretend there were children.
  // This happens for bitmap and other simple sequences.

  if (ChildCount == 0)
  {
    LowTime = 0;
    HighTime = SequenceHeaderPntr->commonHeader.endTime;
    LowMultiple = SequenceHeaderPntr->commonHeader.timeMultiple;
  }

  // Don't allow time multiples less than 1.

  if (LowMultiple <= 0)
    LowMultiple = 1;

  // Success!  Update the sequence time values if different from
  // the range of times covered by the children.  This applies to
  // non-grouping items too, since they can have tweekers and other
  // things inside them.

  if (RecursiveUpdate && ChildCount != 0)
  {
    if (SequenceHeaderPntr->commonHeader.endTime != HighTime ||
    (int) SequenceHeaderPntr->commonHeader.timeMultiple != LowMultiple)
    {
      BasePntr = (BYTE *) LE_DATA_UseDirty (DataID);
      if (BasePntr == NULL) return FALSE;

      SequenceHeaderPntr = (LE_SEQNCR_SequenceIndirectChunkPointer)
      (BasePntr + SequenceChunkOffset + sizeof (LE_CHUNK_HeaderRecord));

      SequenceHeaderPntr->commonHeader.endTime = HighTime;
      SequenceHeaderPntr->commonHeader.timeMultiple = LowMultiple;

      ReturnCode = TRUE; // We changed something.
    }
  }

  // Return the values to the caller.

  if (LowPriorityPntr != NULL)
    *LowPriorityPntr = LowPriority;

  if (HighPriorityPntr != NULL)
    *HighPriorityPntr = HighPriority;

  if (LowTimePntr != NULL)
    *LowTimePntr = LowTime;

  if (HighTimePntr != NULL)
    *HighTimePntr = HighTime;

  return ReturnCode;
}



/*****************************************************************************
 * Looks in the given DataID for a chunk with the given serial number.
 * Returns the offset of the start of the chunk (the very start, including
 * the 4 byte chunk header, you can seek to it and descend right there) with
 * that serial number if successful.  That's the chunk containing the
 * serial number subchunk, not the serial number subchunk itself.
 * Returns -1 on failure.
 */

int LE_CHUNK_FindSerialOffset (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SerialNumber)
{
  UNS32                     AmountActuallyRead;
  LE_CHUNK_ID               ChunkID;
  LE_CHUNK_DopeDataPointer  DopeDataPntr;
  UNS16                     FixedSize;
  UNS8                      Level;
  LE_CHUNK_StreamID         MyStream = -1;
  UNS32                     ReturnedChunkDataSize;
  int                       ReturnValue = -1;
  UNS32                     SubchunkStartOffset;

  MyStream = LE_CHUNK_ReadFromDataID (DataID);
  if (MyStream < 0) goto ErrorExit;

  while (TRUE)
  {
    if ((ChunkID = LE_CHUNK_Descend (MyStream, LE_CHUNK_ID_NULL_STANDARD,
    &ReturnedChunkDataSize)) == LE_CHUNK_ID_NULL_STANDARD)
    {
      if (LE_CHUNK_GetCurrentLevel (MyStream) == 0)
        goto NormalExit; // Hit the end of file (no parent).

      // Hit the end of the parent chunk, go up a level and continue search.

      if (!LE_CHUNK_Ascend (MyStream)) goto ErrorExit;
      continue;
    }

    if (ChunkID >= LE_CHUNK_ID_NULL_IN_CONTEXT ||
    (FixedSize = LE_CHUNK_ChunkFixedDataSizes [ChunkID]) == 0xFFFF)
    {
      // This is a private chunk or one without a fixed size portion
      // and thus one without child chunks (including serial numbers),
      // so skip it.

      if (!LE_CHUNK_Ascend (MyStream)) goto ErrorExit;
      continue;
    }

    // Skip over the fixed part data, to where the child chunks start.

    Level = LE_CHUNK_GetCurrentLevel (MyStream);
    SubchunkStartOffset =
      LE_CHUNK_GetDataStartOffsetForLevel (MyStream, Level);
    SubchunkStartOffset += FixedSize;
    if (!LE_CHUNK_SetCurrentFilePosition (MyStream, SubchunkStartOffset))
      goto ErrorExit;

    // Search for a serial number subchunk.

    if (LE_CHUNK_Descend (MyStream, LE_CHUNK_ID_DOPE_DATA,
    &ReturnedChunkDataSize) != LE_CHUNK_ID_NULL_STANDARD)
    {
      DopeDataPntr = (LE_CHUNK_DopeDataPointer)
        LE_CHUNK_MapChunkData (MyStream, sizeof (LE_CHUNK_DopeDataRecord),
        &AmountActuallyRead);

      if (AmountActuallyRead < sizeof (LE_CHUNK_DopeDataRecord))
        goto ErrorExit; // IO error usually or smaller chunk than expected.

      if (DopeDataPntr->serialNumber == SerialNumber)
      {
        // Found the serial number!

        ReturnValue =
          SubchunkStartOffset - FixedSize - sizeof (LE_CHUNK_HeaderRecord);
        goto NormalExit;
      }

      // Wrong serial number, skip it.
      if (!LE_CHUNK_Ascend (MyStream)) goto ErrorExit;
    }

    // Now examine child chunks.

    if (!LE_CHUNK_SetCurrentFilePosition (MyStream, SubchunkStartOffset))
      goto ErrorExit;
  }

ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_FindSerialOffset: "
    "Something went wrong while looking for serial number %u in "
    "DataID %08X.\r\n", (unsigned int) SerialNumber, (unsigned int) DataID);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

NormalExit:
  if (MyStream >= 0)
    LE_CHUNK_CloseReadStream (MyStream);
  return ReturnValue;
}



/*****************************************************************************
 * Returns a pointer to the start of the fixed data portion of the
 * chunk with the given serial number in the given DataID.  Returns
 * NULL on failure.
 */

void * LE_CHUNK_FindSerialPointer (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SerialNumber)
{
  BYTE *BasePntr;
  int   Offset;

  Offset = LE_CHUNK_FindSerialOffset (DataID, SerialNumber);
  if (Offset < 0)
    return NULL;

  BasePntr = (BYTE *) LE_DATA_Use (DataID);
  if (BasePntr == NULL) return NULL;

  return BasePntr + Offset + sizeof (LE_CHUNK_HeaderRecord);
}



/*****************************************************************************
 * Look for a subchunk of type ChunkID inside the sequence chunk with the
 * given serial number.  Returns the offset of the start of the chunk (the
 * very start, including the 4 byte chunk header, you can seek to it and
 * descend right there).  Returns -1 on failure.
 */

int LE_CHUNK_FindSerialSubchunkOffset (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SerialNumber, LE_CHUNK_ID ChunkID)
{
  UNS16             FixedSize;
  LE_CHUNK_StreamID MyStream = -1;
  int               Offset;
  LE_CHUNK_ID       ParentID;
  int               ReturnValue = -1;

  Offset = LE_CHUNK_FindSerialOffset (DataID, SerialNumber);
  if (Offset < 0) goto ErrorExit; // Can't find the serial number.

  MyStream = LE_CHUNK_ReadFromDataID (DataID);
  if (MyStream < 0) goto ErrorExit; // Unlikely.

  // Advance to the specified chunk.

  if (!LE_CHUNK_SetCurrentFilePosition (MyStream, Offset))
    goto ErrorExit; // Shouldn't happen.

  ParentID = LE_CHUNK_Descend (MyStream, LE_CHUNK_ID_NULL_STANDARD, NULL);
  if (ParentID == LE_CHUNK_ID_NULL_STANDARD ||
  ParentID >= LE_CHUNK_ID_NULL_IN_CONTEXT)
    goto ErrorExit; // Read failed or this is a private chunk.

  // Skip past the fixed part.

  FixedSize = LE_CHUNK_ChunkFixedDataSizes[ParentID];
  if (FixedSize == 0xFFFF)
    goto ErrorExit; // A variable length one, no children, bad.

  if (LE_CHUNK_MapChunkData (MyStream, FixedSize, NULL) == NULL)
    goto ErrorExit; // Read error.

  // Search for the requested subchunk within the parent chunk.

  if (LE_CHUNK_Descend (MyStream, ChunkID, NULL) != ChunkID)
    goto NormalExit; // Not found, no warning message, just return -1.

  Offset = LE_CHUNK_GetCurrentFilePosition (MyStream);
  if (Offset <= 0) goto ErrorExit; // Something bad happened.

  // Success!

  ReturnValue = Offset - sizeof (LE_CHUNK_HeaderRecord);
  goto NormalExit;

ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_FindSerialSubchunkOffset: "
    "Something went wrong while looking for subchunk type %d in sequence "
    "with serial number %u in DataID %08X.\r\n",
    (unsigned int) ChunkID,
    (unsigned int) SerialNumber,
    (unsigned int) DataID);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

NormalExit:
  if (MyStream >= 0)
    LE_CHUNK_CloseReadStream (MyStream);
  return ReturnValue;
}



/*****************************************************************************
 * Returns a pointer to the start of the fixed data portion of the
 * given type of subchunk inside the sequence chunk with the given
 * serial number in the given DataID.  Returns NULL on failure.
 */

void * LE_CHUNK_FindSerialSubchunkPointer (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SerialNumber, LE_CHUNK_ID ChunkID)
{
  BYTE *BasePntr;
  int   Offset;

  Offset = LE_CHUNK_FindSerialSubchunkOffset (DataID, SerialNumber, ChunkID);
  if (Offset < 0)
    return NULL; // Not found.

  BasePntr = (BYTE *) LE_DATA_Use (DataID);
  if (BasePntr == NULL) return NULL;

  return BasePntr + Offset + sizeof (LE_CHUNK_HeaderRecord);
}



/*****************************************************************************
 * Finds the serial number of the given chunk in the given DataID.  Returns
 * zero if it fails.  Use zero for the StartOffset if you want the serial
 * number of the top level sequence.
 */

LE_CHUNK_SerialNumber LE_CHUNK_FindSerialNumber (LE_DATA_DataId DataID,
UNS32 StartOffset)
{
  BYTE                     *BytePntr;
  LE_CHUNK_ID               ChunkID;
  LE_CHUNK_HeaderPointer    ChunkPntr;
  UNS32                     CurrentOffset;
  LE_CHUNK_DopeDataPointer  DopeDataPntr;
  UNS32                     EndOffset;

  BytePntr = (BYTE *) LE_DATA_Use (DataID);
  if (BytePntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_FindFirstSerial: "
    "Unable to read sequence data.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return 0;
  }

  EndOffset = LE_DATA_GetCurrentSize (DataID);

  if (StartOffset >= EndOffset)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_FindFirstSerial: "
    "Start offset of %u is past end of data item, probably corrupt data.\r\n",
    (unsigned int) StartOffset);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return 0;
  }

  ChunkPntr = (LE_CHUNK_HeaderPointer) (BytePntr + StartOffset);

  if (ChunkPntr->chunkSize + StartOffset > EndOffset)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_FindFirstSerial: "
    "Sequence chunk runs past end of data item, probably corrupt data.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return 0;
  }

  EndOffset = ChunkPntr->chunkSize + StartOffset;

  ChunkID = ChunkPntr->chunkID;
  if (ChunkID >= LE_CHUNK_ID_NULL_IN_CONTEXT)
    return 0; // This is a private chunk, no standard children.

  if (LE_CHUNK_ChunkFixedDataSizes[ChunkID] == 0xFFFF)
    return 0; // This chunk doesn't have children, so no serial subchunk.

  // Point at the first child chunk.

  CurrentOffset = StartOffset + LE_CHUNK_ChunkFixedDataSizes[ChunkID] +
    sizeof (LE_CHUNK_HeaderRecord);

  // Search for a serial number child chunk.

  while (CurrentOffset < EndOffset)
  {
    ChunkPntr = (LE_CHUNK_HeaderPointer) (BytePntr + CurrentOffset);
    if (ChunkPntr->chunkID == LE_CHUNK_ID_DOPE_DATA)
    {
      DopeDataPntr = (LE_CHUNK_DopeDataPointer)
        (BytePntr + CurrentOffset + sizeof (LE_CHUNK_HeaderRecord));

      return DopeDataPntr->serialNumber; // Found it!
    }

    CurrentOffset += ChunkPntr->chunkSize;
  }

  return 0; // Didn't find it.
}



/*****************************************************************************
 * Delete the chunk which contains the given serial number.  Returns TRUE
 * if successful, FALSE if nothing deleted.
 */

BOOL LE_CHUNK_DeleteBySerial (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SerialNumber)
{
  LE_CHUNK_HeaderPointer  ChunkHeaderPntr;
  int                     ChunkOffset;

  ChunkOffset = LE_CHUNK_FindSerialOffset (DataID, SerialNumber);
  if (ChunkOffset < 0)
    return FALSE; // Couldn't find serial number.

  ChunkHeaderPntr = (LE_CHUNK_HeaderPointer)
    (((BYTE *) LE_DATA_Use (DataID)) + ChunkOffset);

  return LE_CHUNK_GrowChunk (DataID, ChunkOffset,
    - (int) ChunkHeaderPntr->chunkSize /* Negative to delete bytes in the chunk */,
    ChunkOffset + ChunkHeaderPntr->chunkSize /* Start deleting at end */);
}



/*****************************************************************************
 * Some internal data structures for communicating the serial numbers of the
 * sequences about to be deleted between the callback function and the main
 * delete loop.  The trouble is that you can't delete inside the for all
 * chunks iteration since that changes the structure being iterated over
 * and breaks the iteration.
 */

#define MAX_DELETE_SELECTED_SERIAL_NUMBERS 100
typedef struct DeleteSelectedStruct
{
  int numberFound;
  LE_CHUNK_SerialNumber serialNumbers [MAX_DELETE_SELECTED_SERIAL_NUMBERS];
} DeleteSelectedRecord, *DeleteSelectedPntr;

BOOL LI_CHUNK_DeleteSelectedCallback (LE_CHUNK_StreamID StreamID,
LE_CHUNK_ID ChunkID, LE_SEQNCR_SequenceChunkHeaderPointer SeqHeaderPntr,
LE_CHUNK_DopeDataPointer DopeDataPntr, void *UserValue, INT32 AbsoluteStartTime)
{
  DeleteSelectedPntr  DeleteInfo;

  DeleteInfo = (DeleteSelectedPntr) UserValue;

  DeleteInfo->serialNumbers [DeleteInfo->numberFound++] =
    DopeDataPntr->serialNumber;

  if (DeleteInfo->numberFound >= MAX_DELETE_SELECTED_SERIAL_NUMBERS)
    return FALSE; // Halt the iteration, have run out of storage.

  return TRUE;
}



/*****************************************************************************
 * Go through the children of the given sequence (serial number zero for
 * the whole DataID top level) and delete the ones which are selected.
 * Not recursive, so selected things inside groups inside the given
 * subsequence are not deleted, unless their group is selected.
 * Returns the number of deleted items, -1 if an error happened.
 */

int LE_CHUNK_DeleteSelected (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SubSequenceSerialNumber)
{
  DeleteSelectedRecord  DeleteInfo;
  int                   i;
  int                   TotalDeleted = 0;

  do
  {
    DeleteInfo.numberFound = 0;

    if (!LE_CHUNK_ForAllSubSequences (DataID, SubSequenceSerialNumber,
    FALSE /* Recursive */, LE_CHUNK_DopeDataFlagSelected /* FlagsToMatch */,
    FALSE /* RequireAllFlags */, LI_CHUNK_DeleteSelectedCallback,
    &DeleteInfo /* UserValue */, 0 /* BaseStartTime */) &&
    DeleteInfo.numberFound < MAX_DELETE_SELECTED_SERIAL_NUMBERS)
      return -1; // Something went wrong, not an ordinary abort.

    TotalDeleted += DeleteInfo.numberFound;

    for (i = 0; i < DeleteInfo.numberFound; i++)
    {
      if (!LE_CHUNK_DeleteBySerial (DataID, DeleteInfo.serialNumbers [i]))
        return -1; // Error while deleting.
    }
  } while (DeleteInfo.numberFound >= MAX_DELETE_SELECTED_SERIAL_NUMBERS);

  return TotalDeleted;
}



/*****************************************************************************
 * Insert a subchunk in the chunk with the given serial number.  Returns
 * TRUE if successful.  If the subchunk is a sequence type of chunk, it also
 * inserts it in the appropriate place according to starting time.  If it
 * is a private chunk, it gets inserted as the first child.  If it is some
 * non-sequence public chunk then it gets inserted at the end.
 */

BOOL LE_CHUNK_InsertChildUnderParentSerial (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SerialNumber, LE_CHUNK_HeaderPointer NewSubchunkPntr)
{
  BYTE                   *BasePntr;
  LE_CHUNK_HeaderPointer  ChunkHeaderPntr;
  LE_CHUNK_ID             ChunkID;
  int                     ChunkOffset;
  UNS32                   EndOffset;
  UNS16                   FixedSize;
  UNS32                   InsertOffset;
  UNS8                    NewPriority;
  INT32                   NewStartTime;
  LE_CHUNK_HeaderPointer  ScanPntr;
  LE_SEQNCR_SequenceChunkHeaderPointer TempSeqPntr;
  INT32                   TestTime;

  if (NewSubchunkPntr == NULL)
    return FALSE;

  ChunkOffset = LE_CHUNK_FindSerialOffset (DataID, SerialNumber);
  if (ChunkOffset < 0)
    return FALSE; // Couldn't find serial number.

  BasePntr = (BYTE *) LE_DATA_Use (DataID);
  ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + ChunkOffset);
  ChunkID = ChunkHeaderPntr->chunkID;

  if (ChunkID >= LE_CHUNK_ID_NULL_IN_CONTEXT)
    return FALSE; // A private chunk, can't have children.

  FixedSize = LE_CHUNK_ChunkFixedDataSizes [ChunkID];
  if (FixedSize == (UNS16) 0xFFFF)
    return FALSE; // A public chunk without children.

  // Figure out where to add the new subchunk.

  if (NewSubchunkPntr->chunkID >= LE_CHUNK_ID_NULL_IN_CONTEXT)
  {
    // Add private subchunks at the front.

    InsertOffset = ChunkOffset + sizeof (LE_CHUNK_HeaderRecord) + FixedSize;
  }
  else if (NewSubchunkPntr->chunkID >= LE_CHUNK_ID_SEQ_GROUPING &&
  NewSubchunkPntr->chunkID < LE_CHUNK_ID_SEQ_MAX)
  {
    // A sequence subchunk - insert based on starting time, or at end
    // if no other existing sequence subchunks (all private chunks have
    // to come before the sequence chunks otherwise the sequencer will
    // get confused, and the sequence chunks have to be in increasing
    // start time order).

    InsertOffset = ChunkOffset + sizeof (LE_CHUNK_HeaderRecord) + FixedSize;
    EndOffset = ChunkOffset + ChunkHeaderPntr->chunkSize;

    TempSeqPntr = (LE_SEQNCR_SequenceChunkHeaderPointer)
      (((BYTE *) NewSubchunkPntr) + sizeof (LE_CHUNK_HeaderRecord));

    NewStartTime = TempSeqPntr->parentStartTime;
    NewPriority = TempSeqPntr->priority;

    while (InsertOffset < EndOffset)
    {
      ScanPntr = (LE_CHUNK_HeaderPointer) (BasePntr + InsertOffset);

      if (ScanPntr->chunkID >= LE_CHUNK_ID_SEQ_GROUPING &&
      ScanPntr->chunkID < LE_CHUNK_ID_SEQ_MAX)
      {
        TempSeqPntr = (LE_SEQNCR_SequenceChunkHeaderPointer)
          (BasePntr + InsertOffset + sizeof (LE_CHUNK_HeaderRecord));

        TestTime = TempSeqPntr->parentStartTime;

        if (NewStartTime < TestTime ||
        (NewStartTime == TestTime && NewPriority < TempSeqPntr->priority))
        {
          // Found another subsequence which is after our start time,
          // so we will insert just before it, at InsertOffset.

          break;
        }
      }

      // Advance to the next subchunk.
      InsertOffset += ScanPntr->chunkSize;
    }
    if (InsertOffset > EndOffset)
      return FALSE; // Data is corrupt, should have ended at end.
  }
  else // Some other kind of public chunk, put at the end.
    InsertOffset = ChunkOffset + ChunkHeaderPntr->chunkSize;

  if (!LE_CHUNK_GrowChunk (DataID, ChunkOffset,
  NewSubchunkPntr->chunkSize, InsertOffset))
    return FALSE;

  // The chunk may have moved around during growth, refetch the pointer,
  // and copy the new subchunk into its place.

  BasePntr = (BYTE *) LE_DATA_Use (DataID);
  memcpy (BasePntr + InsertOffset, NewSubchunkPntr, NewSubchunkPntr->chunkSize);

  return TRUE;
}



/*****************************************************************************
 * LE_CHUNK_SortChildSequencesByTime goes through the given chunk and
 * rearrange all its children in increasing time order.  This is somewhat
 * inefficiently done, but then speed isn't too critical, unless we start
 * doing really big sequences, then someone should use QuickSort or
 * something better than this slow insertion method.
 *
 * This one works by copying the out of order child sequences into a temporary
 * DataID, then goes through them and deletes and reinserts each one.
 */

typedef struct SortStuffStruct
{
  LE_CHUNK_StreamID     streamID;
  INT32                 lastParentStartTime;
  UNS16                 lastPriority;
  LE_DATA_DataId        dataID;
  LE_CHUNK_SerialNumber serialNumber;
  int                   outOfOrderCount;
} SortStuffRecord, *SortStuffPointer;

BOOL LI_CHUNK_SortCopyToTemp (LE_CHUNK_StreamID StreamID,
LE_CHUNK_ID ChunkID, LE_SEQNCR_SequenceChunkHeaderPointer SeqHeaderPntr,
LE_CHUNK_DopeDataPointer DopeDataPntr, void *UserValue, INT32 AbsoluteStartTime)
{
  LE_CHUNK_HeaderPointer  ChunkPntr;
  SortStuffPointer        SortStuff;

  SortStuff = (SortStuffPointer) UserValue;

  if (SeqHeaderPntr->parentStartTime < SortStuff->lastParentStartTime ||
  (SeqHeaderPntr->parentStartTime == SortStuff->lastParentStartTime &&
  SeqHeaderPntr->priority < SortStuff->lastPriority))
  {
    // This one is out of order.  Append to temporary storage.

    ChunkPntr = (LE_CHUNK_HeaderPointer)
      (((BYTE *) SeqHeaderPntr) - sizeof (LE_CHUNK_HeaderRecord));
    if (!LE_CHUNK_WriteChunkData (SortStuff->streamID,
    ChunkPntr, ChunkPntr->chunkSize))
      return FALSE;
    SortStuff->outOfOrderCount++;
  }
  else // One which is in order.
  {
    SortStuff->lastParentStartTime = SeqHeaderPntr->parentStartTime;
    SortStuff->lastPriority = SeqHeaderPntr->priority;
  }

  return TRUE;
}

BOOL LI_CHUNK_SortReInsert (LE_CHUNK_StreamID StreamID,
LE_CHUNK_ID ChunkID, LE_SEQNCR_SequenceChunkHeaderPointer SeqHeaderPntr,
LE_CHUNK_DopeDataPointer DopeDataPntr, void *UserValue, INT32 AbsoluteStartTime)
{
  LE_CHUNK_HeaderPointer  ChunkPntr;
  SortStuffPointer        SortStuff;

  if (DopeDataPntr != NULL)
  {
    SortStuff = (SortStuffPointer) UserValue;

    ChunkPntr = (LE_CHUNK_HeaderPointer)
      (((BYTE *) SeqHeaderPntr) - sizeof (LE_CHUNK_HeaderRecord));

    if (!LE_CHUNK_DeleteBySerial (SortStuff->dataID,
    DopeDataPntr->serialNumber))
      return FALSE;

    if (!LE_CHUNK_InsertChildUnderParentSerial (SortStuff->dataID,
    SortStuff->serialNumber, ChunkPntr))
      return FALSE;
  }

  return TRUE;
}

BOOL LE_CHUNK_SortChildSequencesByTime (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SerialNumber)
{
  LE_CHUNK_StreamID                     MyStreamID = -1;
  BOOL                                  ReturnCode = FALSE;
  SortStuffRecord                       SortStuff;
  LE_DATA_DataId                        TempDataID = LE_DATA_EmptyItem;

  // Make a temporary in-memory data item to hold a copy of
  // the out of order child sequences.

  TempDataID = LE_DATA_AllocMemoryDataID (1000, LE_DATA_DataChunky);
  if (TempDataID == LE_DATA_EmptyItem) goto ErrorExit;

  MyStreamID = LE_CHUNK_WriteToDataID (TempDataID);
  if (MyStreamID < 0) goto ErrorExit;

  // Iterate through all child sequences and add all ones which are out
  // of order to the temporary collection.

  SortStuff.streamID = MyStreamID;
  SortStuff.lastParentStartTime = INT32_MIN;
  SortStuff.lastPriority = 0;
  SortStuff.dataID = DataID;
  SortStuff.serialNumber = SerialNumber;
  SortStuff.outOfOrderCount = 0;

  if (!LE_CHUNK_ForAllSubSequences (DataID, SerialNumber, FALSE,
  0 /* FlagsToMatch */, FALSE /* RequireAllFlags */,
  LI_CHUNK_SortCopyToTemp, &SortStuff, 0 /* BaseStartTime */))
    goto ErrorExit;

  LE_CHUNK_CloseWriteStream (MyStreamID);
  MyStreamID = -1;

  // For each one added, delete it from the original sequence and re-insert it.

  if (SortStuff.outOfOrderCount > 0 &&
  !LE_CHUNK_ForAllSubSequences (TempDataID, 0 /* No serial - top level */,
  FALSE /* Recursive */, 0 /* FlagsToMatch */, FALSE /* RequireAllFlags */,
  LI_CHUNK_SortReInsert, &SortStuff, 0 /* BaseStartTime */))
    goto ErrorExit;

  // Success!
  ReturnCode = TRUE;
  goto NormalExit;

ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_SortChildSequencesByTime: "
  "Some sort of error happened.\r\n");
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

NormalExit:
  if (MyStreamID >= 0)
    LE_CHUNK_CloseWriteStream (MyStreamID);
  if (TempDataID != LE_DATA_EmptyItem)
    LE_DATA_FreeRuntimeDataID (TempDataID);

  return ReturnCode;
}



/*****************************************************************************
 * This class contains the current state for LE_CHUNK_ForAllSubSequences.
 * Things like semi-global variables used in a particular traversal,
 * which makes it easy to access them while still keeping it multitasking
 * safe without passing zillions of arguments to each recursive call.
 */

typedef class LI_CHUNK_ForAllClass
{
private:
  LE_DATA_DataId                          m_DataID;
  UNS32                                   m_FlagsToMatch;
  BOOL                                    m_Recursive;
  BOOL                                    m_RequireAllFlags;
  LE_CHUNK_StreamID                       m_StreamID;
  LE_CHUNK_ForAllCallbackFunctionPointer  m_UserFunctionPntr;
  void                                   *m_UserValue;

  BOOL ScanChildChunks (INT32 AbsoluteStartTime);

public:
  LI_CHUNK_ForAllClass ();
  ~LI_CHUNK_ForAllClass ();

  BOOL TraverseDataID (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SubSequenceSerialNumber, BOOL Recursive,
    UNS32 FlagsToMatch, BOOL RequireAllFlags,
    LE_CHUNK_ForAllCallbackFunctionPointer UserFunctionPntr, void *UserValue,
    INT32 BaseStartTime);

} *LI_CHUNK_ForAllPointer;



/*****************************************************************************
 * Just initialise the class to an empty state.
 */

LI_CHUNK_ForAllClass::LI_CHUNK_ForAllClass ()
{
  m_StreamID = -1;
}



/*****************************************************************************
 * Deallocate things allocated.
 */

LI_CHUNK_ForAllClass::~LI_CHUNK_ForAllClass ()
{
  if (m_StreamID >= 0)
  {
    LE_CHUNK_CloseReadStream (m_StreamID);
    m_StreamID = -1;
  }
}



/*****************************************************************************
 * Open a chunk stream on a DataID and start traversing it.
 */

BOOL LI_CHUNK_ForAllClass::TraverseDataID (LE_DATA_DataId DataID,
  LE_CHUNK_SerialNumber SubSequenceSerialNumber, BOOL Recursive,
  UNS32 FlagsToMatch, BOOL RequireAllFlags,
  LE_CHUNK_ForAllCallbackFunctionPointer UserFunctionPntr, void *UserValue,
  INT32 BaseStartTime)
{
  UNS32       AmountActuallyRead;
  LE_CHUNK_ID ChunkID;
  int         Offset;

  m_DataID = DataID;
  m_StreamID = LE_CHUNK_ReadFromDataID (DataID);
  if (m_StreamID < 0) return FALSE; // Failed to open.

  if (SubSequenceSerialNumber != 0)
  {
    Offset = LE_CHUNK_FindSerialOffset (DataID, SubSequenceSerialNumber);
    if (Offset < 0) return FALSE; // Serial number not found.

    if (!LE_CHUNK_SetCurrentFilePosition (m_StreamID, Offset))
      return FALSE;

    ChunkID = LE_CHUNK_Descend (m_StreamID, LE_CHUNK_ID_NULL_STANDARD, NULL);
    if (ChunkID < LE_CHUNK_ID_SEQ_GROUPING || ChunkID >= LE_CHUNK_ID_SEQ_MAX)
      return FALSE; // We need a grouping chunk for children to exist.

    // Skip past fixed part header of the group.

    LE_CHUNK_MapChunkData (m_StreamID, LE_CHUNK_ChunkFixedDataSizes[ChunkID],
      &AmountActuallyRead);
    if (AmountActuallyRead != LE_CHUNK_ChunkFixedDataSizes[ChunkID])
      return FALSE; // Didn't read enough data.
  }

  // The stream is now positioned at the start of the first child chunk,
  // and will continue on at the current level until it hits the end of
  // the parent chunk (or end of data if doing the whole DataID).

  m_FlagsToMatch = FlagsToMatch;
  m_Recursive = Recursive;
  m_RequireAllFlags = RequireAllFlags;
  m_UserFunctionPntr = UserFunctionPntr;
  m_UserValue = UserValue;

  return ScanChildChunks (BaseStartTime);
}



/*****************************************************************************
 * Scan all the child chunks in the stream, looking for sequences which
 * match the desired flags, and then call the user function on them.
 * AbsoluteStartTime is the starting time in global clock units of the
 * enclosing sequence.  Returns TRUE if all were examined, FALSE on errors
 * or an abort by the user function.
 */

BOOL LI_CHUNK_ForAllClass::ScanChildChunks (INT32 AbsoluteStartTime)
{
  UNS32                                 AmountActuallyRead;
  UNS32                                 AmountToRead;
  UNS32                                 ChildrenStartOffset;
  INT32                                 ChildStartTime;
  LE_CHUNK_ID                           ChunkID;
  LE_CHUNK_DopeDataPointer              DopeDataPntr;
  UNS32                                 Flags;
  LE_DATA_DataId                        IndirectDataID;
  LE_SEQNCR_SequenceChunkHeaderPointer  SeqHeaderPntr;

  while (TRUE)
  {
    ChunkID = LE_CHUNK_Descend (m_StreamID, LE_CHUNK_ID_NULL_STANDARD, NULL);
    if (ChunkID == LE_CHUNK_ID_NULL_STANDARD)
      break; // Reached the end of the data.

    if (ChunkID >= LE_CHUNK_ID_SEQ_GROUPING && ChunkID < LE_CHUNK_ID_SEQ_MAX)
    {
      // Read the variable length sequence header.

      AmountToRead = LE_CHUNK_ChunkFixedDataSizes [ChunkID];
      SeqHeaderPntr = (LE_SEQNCR_SequenceChunkHeaderPointer)
        LE_CHUNK_MapChunkData (m_StreamID, AmountToRead, &AmountActuallyRead);
      if (SeqHeaderPntr == NULL || AmountActuallyRead != AmountToRead)
        return FALSE; // IO error or corrupt data.

      ChildrenStartOffset = LE_CHUNK_GetCurrentFilePosition (m_StreamID);

      // Figure out the absolute time when this child sequence starts.

      ChildStartTime = AbsoluteStartTime + SeqHeaderPntr->parentStartTime;

      // Look for the dope data serial number and flags subchunk.

      if (LE_CHUNK_Descend (m_StreamID, LE_CHUNK_ID_DOPE_DATA, NULL) !=
      LE_CHUNK_ID_NULL_STANDARD)
      {
        AmountToRead = LE_CHUNK_ChunkFixedDataSizes [LE_CHUNK_ID_DOPE_DATA];
        DopeDataPntr = (LE_CHUNK_DopeDataPointer)
          LE_CHUNK_MapChunkData (m_StreamID, AmountToRead, &AmountActuallyRead);
        if (DopeDataPntr == NULL || AmountActuallyRead != AmountToRead)
          return FALSE; // IO error or corrupt data.
        Flags = DopeDataPntr->flags;
        if (!LE_CHUNK_Ascend (m_StreamID))
          return FALSE; // Some stream error.
      }
      else // No serial number chunk, no flags.
      {
        DopeDataPntr = NULL;
        Flags = 0;
      }

      // See if the flags match.

      if (m_FlagsToMatch == 0 ||
      (m_RequireAllFlags && (Flags & m_FlagsToMatch) == m_FlagsToMatch) ||
      (!m_RequireAllFlags && (Flags & m_FlagsToMatch)))
      {
        // Set the stream back to the children.

        if (!LE_CHUNK_SetCurrentFilePosition (m_StreamID, ChildrenStartOffset))
          return FALSE;

        // Call the user function.

        if (!m_UserFunctionPntr (m_StreamID, ChunkID,
        SeqHeaderPntr, DopeDataPntr, m_UserValue, ChildStartTime))
          return FALSE; // Aborted by user.
      }

      // If recursion is required, examine the child chunks
      // in grouping sequences.

      if (m_Recursive)
      {
        if (ChunkID == LE_CHUNK_ID_SEQ_INDIRECT)
        {
          IndirectDataID = ((LE_SEQNCR_SequenceIndirectChunkPointer)
            SeqHeaderPntr)->subsequenceChunkListDataID;
          if (!SeqHeaderPntr->absoluteDataIDs)
            IndirectDataID =
            LE_DATA_IdWithFileFromParent (IndirectDataID, m_DataID);

          if (!LE_CHUNK_ForAllSubSequences (IndirectDataID,
          0 /* Whole DataID */, m_Recursive, m_FlagsToMatch,
          m_RequireAllFlags, m_UserFunctionPntr, m_UserValue,
          ChildStartTime))
            return FALSE;
        }
        else // Other sequence chunks have children inside the same DataID.
        {
          if (!LE_CHUNK_SetCurrentFilePosition (m_StreamID, ChildrenStartOffset))
            return FALSE;

          if (!ScanChildChunks (ChildStartTime))
            return FALSE;
        }
      }
    }

    if (!LE_CHUNK_Ascend (m_StreamID))
      return FALSE; // Some stream error.
  }

  return TRUE;
}



/*****************************************************************************
 * This function iterates through all the subsequences in the given DataID,
 * and possibly even other DataIDs if it includes indirect sequences.
 * Each sequence encountered has its flags tested, if they match then
 * the user function gets called.  If the user function returns TRUE then
 * it continues on iterating to the next subsequence.  Simple, eh?
 *
 * And now for some details:
 *
 * The iteration examines the children of the sequence identified
 * by SubSequenceSerialNumber inside the given DataID (use zero for the
 * whole DataID).  Thus it needs to be a grouping sequence type.
 *
 * If Recursive is TRUE then subsequences (things inside grouping and
 * indirect sequences (in another DataID)) will be examined too (children
 * done after parent done).  If it is FALSE then indirect and child
 * sequences won't be explored.
 *
 * The match is done by looking at the dope sheet data chunk (see
 * LE_CHUNK_DopeDataRecord) for the flags.  If RequireAllFlags is TRUE
 * then all of the bits in FlagsToMatch must be 1's in the flags field
 * of the dope data chunk before the user function will be called.
 * If RequireAllFlags is FALSE then only one or more of the bits from
 * FlagsToMatch must be 1 in the dope data.
 *
 * Once a match is found, the user's callback function is called.  It is
 * passed a stream argument, with the stream positioned at the start of
 * the chunk children (a descend into the chunk has already been done
 * and the fixed header has been read (size depending on the actual
 * sequence type), and is available in SeqHeaderPntr).  DopeDataPntr is
 * either NULL or if the LE_CHUNK_ID_DOPE_DATA subchunk exists, it
 * points to the serial number and flags.  ChunkID is the kind of
 * sequence chunk involved.  If the user function returns FALSE then the
 * traversal is halted and LE_CHUNK_ForAllSubSequences also returns FALSE.
 * If it returns TRUE then the traversal continues (it does an Ascend
 * immediately after the user function returns, to get out of the chunk).
 *
 * LE_CHUNK_ForAllSubSequences returns TRUE if it finished the full traversal,
 * FALSE if an error occurred or if the user function stopped the traversal.
 */

BOOL LE_CHUNK_ForAllSubSequences (LE_DATA_DataId DataID,
  LE_CHUNK_SerialNumber SubSequenceSerialNumber, BOOL Recursive,
  UNS32 FlagsToMatch, BOOL RequireAllFlags,
  LE_CHUNK_ForAllCallbackFunctionPointer UserFunctionPntr, void *UserValue,
  INT32 BaseStartTime)
{
  LI_CHUNK_ForAllClass  Iterator;

  return Iterator.TraverseDataID (DataID, SubSequenceSerialNumber, Recursive,
    FlagsToMatch, RequireAllFlags, UserFunctionPntr, UserValue,
    BaseStartTime);
}



/*****************************************************************************
 * LE_CHUNK_CountSelected counts the number of selected sequences which are
 * children of the given sequence.  Returns -1 on errors.
 */

BOOL LI_CHUNK_CountSelectedCallback (LE_CHUNK_StreamID StreamID,
LE_CHUNK_ID ChunkID, LE_SEQNCR_SequenceChunkHeaderPointer SeqHeaderPntr,
LE_CHUNK_DopeDataPointer DopeDataPntr, void *UserValue, INT32 AbsoluteStartTime)
{
  int *CountPntr;

  CountPntr = (int *) UserValue;
  *CountPntr += 1;

  return TRUE;
}

int LE_CHUNK_CountSelected (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SubSequenceSerialNumber, BOOL Recursive)
{
  int Counter;

  Counter = 0;

  if (!LE_CHUNK_ForAllSubSequences (DataID, SubSequenceSerialNumber,
  Recursive, LE_CHUNK_DopeDataFlagSelected /* FlagsToMatch */,
  FALSE /* RequireAllFlags */, LI_CHUNK_CountSelectedCallback,
  &Counter /* UserValue */, 0 /* BaseStartTime */))
    return -1;

  return Counter;
}



/*****************************************************************************
 * LE_CHUNK_FindOneSelectedSerial returns the serial number of the first
 * selected sequence it can find, returns 0 if none or something is wrong.
 */

BOOL LI_CHUNK_FindOneSelectedSerialCallback (LE_CHUNK_StreamID StreamID,
LE_CHUNK_ID ChunkID, LE_SEQNCR_SequenceChunkHeaderPointer SeqHeaderPntr,
LE_CHUNK_DopeDataPointer DopeDataPntr, void *UserValue, INT32 AbsoluteStartTime)
{
  LE_CHUNK_SerialNumber *SerialPntr;

  SerialPntr = (LE_CHUNK_SerialNumber *) UserValue;
  *SerialPntr = DopeDataPntr->serialNumber;

  return FALSE; // Stop iterating.
}


LE_CHUNK_SerialNumber LE_CHUNK_FindOneSelectedSerial (
LE_DATA_DataId DataID, LE_CHUNK_SerialNumber SubSequenceSerialNumber,
BOOL Recursive)
{
  LE_CHUNK_SerialNumber Serial;

  Serial = 0;

  LE_CHUNK_ForAllSubSequences (DataID, SubSequenceSerialNumber,
  Recursive, LE_CHUNK_DopeDataFlagSelected /* FlagsToMatch */,
  FALSE /* RequireAllFlags */, LI_CHUNK_FindOneSelectedSerialCallback,
  &Serial /* UserValue */, 0 /* BaseStartTime */);

  return Serial;
}



/*****************************************************************************
 * LE_CHUNK_FindOneSelectedOffset returns the offset of the start of some
 * selected sequence chunk.  Returns -1 on failure.
 */

BOOL LI_CHUNK_FindOneSelectedOffsetCallback (LE_CHUNK_StreamID StreamID,
LE_CHUNK_ID ChunkID, LE_SEQNCR_SequenceChunkHeaderPointer SeqHeaderPntr,
LE_CHUNK_DopeDataPointer DopeDataPntr, void *UserValue, INT32 AbsoluteStartTime)
{
  UNS8  CurrentLevel;
  int  *OffsetPntr;
  int   TempOffset;

  OffsetPntr = (int *) UserValue;
  CurrentLevel = LE_CHUNK_GetCurrentLevel (StreamID);
  TempOffset = LE_CHUNK_GetDataStartOffsetForLevel (StreamID, CurrentLevel);
  if (TempOffset > 0)
    TempOffset -= sizeof (LE_CHUNK_HeaderRecord);
  *OffsetPntr = TempOffset;

  return FALSE; // Stop iterating.
}

int LE_CHUNK_FindOneSelectedOffset (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SubSequenceSerialNumber, BOOL Recursive)
{
  int Offset;

  Offset = -1;

  LE_CHUNK_ForAllSubSequences (DataID, SubSequenceSerialNumber,
  Recursive, LE_CHUNK_DopeDataFlagSelected /* FlagsToMatch */,
  FALSE /* RequireAllFlags */, LI_CHUNK_FindOneSelectedOffsetCallback,
  &Offset /* UserValue */, 0 /* BaseStartTime */);

  return Offset;
}



/*****************************************************************************
 * LE_CHUNK_ClearAllSelections turns off the selection flag in all sequences
 * inside DataID, recursively including subsequences and indirect ones.
 * Requires the existence of dope data subchunks in the sequences.
 * Returns FALSE if something goes wrong.  Does not mark the DataID as dirty.
 */

BOOL LI_CHUNK_ClearSelectionCallback (LE_CHUNK_StreamID StreamID,
LE_CHUNK_ID ChunkID, LE_SEQNCR_SequenceChunkHeaderPointer SeqHeaderPntr,
LE_CHUNK_DopeDataPointer DopeDataPntr, void *UserValue, INT32 AbsoluteStartTime)
{
  if (DopeDataPntr != NULL)
    DopeDataPntr->flags &= ~LE_CHUNK_DopeDataFlagSelected;

  return TRUE;
}

BOOL LE_CHUNK_ClearAllSelections (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SubSequenceSerialNumber)
{
  return LE_CHUNK_ForAllSubSequences (DataID, SubSequenceSerialNumber,
    TRUE /* Recursive */, 0 /* FlagsToMatch */, FALSE /* RequireAllFlags */,
    LI_CHUNK_ClearSelectionCallback, NULL /* UserValue */, 0 /* BaseStartTime */);
}



/*****************************************************************************
 * LE_CHUNK_SelectByRange goes through the given sequence and marks all
 * child sequences as selected if they are at least partially within the
 * given priority and time range (priority from PriorityAndTimeRange.left
 * up to but not including .right, time from .top up to but not including
 * .bottom).  It is not recursive, otherwise things could get a bit confusing.
 * The selections are added to any existing selections, so you have to
 * call LE_CHUNK_ClearAllSelections before using this function if you
 * don't want to keep any old selections around.
 */

BOOL LI_CHUNK_RangeSelectCallback (LE_CHUNK_StreamID StreamID,
LE_CHUNK_ID ChunkID, LE_SEQNCR_SequenceChunkHeaderPointer SeqHeaderPntr,
LE_CHUNK_DopeDataPointer DopeDataPntr, void *UserValue, INT32 AbsoluteStartTime)
{
  TYPE_RectPointer  RectPntr;

  if (DopeDataPntr != NULL)
  {
    RectPntr = (TYPE_RectPointer) UserValue;

    if (SeqHeaderPntr->parentStartTime < RectPntr->bottom &&
    SeqHeaderPntr->parentStartTime +
    SeqHeaderPntr->endTime > RectPntr->top &&
    (int) SeqHeaderPntr->priority >= RectPntr->left &&
    (int) SeqHeaderPntr->priority < RectPntr->right)
    {
      DopeDataPntr->flags |= LE_CHUNK_DopeDataFlagSelected;
    }
  }
  return TRUE;
}

BOOL LE_CHUNK_SelectByRange (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SubSequenceSerialNumber,
TYPE_RectPointer PriorityAndTimeRange)
{
  return LE_CHUNK_ForAllSubSequences (DataID, SubSequenceSerialNumber,
    FALSE /* Recursive */, 0 /* FlagsToMatch */, FALSE /* RequireAllFlags */,
    LI_CHUNK_RangeSelectCallback, PriorityAndTimeRange /* UserValue */,
    0 /* BaseStartTime */);
}



/*****************************************************************************
 * LE_CHUNK_ToggleByRange is like LE_CHUNK_SelectByRange except that it
 * toggles (inverts) the selection flag for sequences in the range.
 */

BOOL LI_CHUNK_RangeToggleCallback (LE_CHUNK_StreamID StreamID,
LE_CHUNK_ID ChunkID, LE_SEQNCR_SequenceChunkHeaderPointer SeqHeaderPntr,
LE_CHUNK_DopeDataPointer DopeDataPntr, void *UserValue, INT32 AbsoluteStartTime)
{
  TYPE_RectPointer  RectPntr;

  if (DopeDataPntr != NULL)
  {
    RectPntr = (TYPE_RectPointer) UserValue;

    if (SeqHeaderPntr->parentStartTime < RectPntr->bottom &&
    SeqHeaderPntr->parentStartTime +
    SeqHeaderPntr->endTime > RectPntr->top &&
    (int) SeqHeaderPntr->priority >= RectPntr->left &&
    (int) SeqHeaderPntr->priority < RectPntr->right)
    {
      if (DopeDataPntr->flags & LE_CHUNK_DopeDataFlagSelected)
        DopeDataPntr->flags &= ~LE_CHUNK_DopeDataFlagSelected;
      else
        DopeDataPntr->flags |= LE_CHUNK_DopeDataFlagSelected;
    }
  }
  return TRUE;
}

BOOL LE_CHUNK_ToggleByRange (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SubSequenceSerialNumber,
TYPE_RectPointer PriorityAndTimeRange)
{
  return LE_CHUNK_ForAllSubSequences (DataID, SubSequenceSerialNumber,
    FALSE /* Recursive */, 0 /* FlagsToMatch */, FALSE /* RequireAllFlags */,
    LI_CHUNK_RangeToggleCallback, PriorityAndTimeRange /* UserValue */,
    0 /* BaseStartTime */);
}



/*****************************************************************************
 * LE_CHUNK_CopySelectedFlagToNeedsProcessingFlag recursively goes through
 * all the subchunks in the given DataID and sets the NeedsProcessing flag
 * to the same state as the selected flag.  Returns TRUE if there were
 * some selected items in the DataID, FALSE on errors or no selected items.
 */

BOOL LI_CHUNK_CopySelectedToProcessingCallback (LE_CHUNK_StreamID StreamID,
LE_CHUNK_ID ChunkID, LE_SEQNCR_SequenceChunkHeaderPointer SeqHeaderPntr,
LE_CHUNK_DopeDataPointer DopeDataPntr, void *UserValue, INT32 AbsoluteStartTime)
{
  if (DopeDataPntr != NULL)
  {
    if (DopeDataPntr->flags & LE_CHUNK_DopeDataFlagSelected)
    {
      DopeDataPntr->flags |= LE_CHUNK_DopeDataFlagNeedsProcessing;
      *((BOOL *) UserValue) = TRUE;
    }
    else
      DopeDataPntr->flags &= ~LE_CHUNK_DopeDataFlagNeedsProcessing;
  }
  return TRUE;
}

BOOL LE_CHUNK_CopySelectedFlagToNeedsProcessingFlag (LE_DATA_DataId DataID)
{
  BOOL  ReturnCode = FALSE;

  if (!LE_CHUNK_ForAllSubSequences (DataID, 0 /* SubSequenceSerialNumber */,
  TRUE /* Recursive */, 0 /* FlagsToMatch */, FALSE /* RequireAllFlags */,
  LI_CHUNK_CopySelectedToProcessingCallback, &ReturnCode /* UserValue */,
  0 /* BaseStartTime */))
    ReturnCode = FALSE; // Something went wrong.

  return ReturnCode;
}



/*****************************************************************************
 * Figure out the dimensionality of the given sequence chunk.  First it
 * checks the chunk type.  If that isn't copnclusive, it looks for subchunks
 * which hint at the dimensionality.  If that isn't good enough it then
 * recursively looks at the parent chunk to see what dimensionality is
 * inherited.  TargetChunkOffset points to the chunk header of the sequence
 * chunk being examined.
 * Returns -1 on error or dimensionality not relevant, otherwise returns
 * the dimensionality (0, 2 or 3).
 */

int LE_CHUNK_FindDimensionality (LE_DATA_DataId DataID, UNS32 TargetChunkOffset)
{
  BYTE                                 *BasePntr;
  LE_CHUNK_ID                           ChunkID;
  LE_CHUNK_HeaderPointer                ChunkPntr;
  UNS32                                 CurrentOffset;
  LE_SEQNCR_DimensionalityChunkPointer  DimensionalityPntr;
  INT32                                 ParentOffset;
  int                                   ReturnDimensionality = -1;
  UNS32                                 TargetChunkPastEndOffset;
  UNS32                                 WholeSize;

  BasePntr = (BYTE *) LE_DATA_Use (DataID);
  if (BasePntr == NULL) goto ErrorExit; // Unable to load data.

  WholeSize = LE_DATA_GetCurrentSize (DataID);
  if (WholeSize == 0) goto ErrorExit; // Weirdness.

  if (TargetChunkOffset + sizeof (LE_CHUNK_HeaderRecord) > WholeSize)
    goto ErrorExit; // User's chunk is outside the file.

  ChunkPntr = (LE_CHUNK_HeaderPointer) (BasePntr + TargetChunkOffset);
  TargetChunkPastEndOffset = TargetChunkOffset + ChunkPntr->chunkSize;

  if (TargetChunkPastEndOffset > WholeSize)
    goto ErrorExit; // User's chunk is partially outside the file or bad data.

  ChunkID = ChunkPntr->chunkID;

  if (ChunkID < LE_CHUNK_ID_SEQ_GROUPING || ChunkID >= LE_CHUNK_ID_SEQ_MAX)
    goto NormalExit; // Not a sequence chunk, so no dimensionality.

  // See if we can determine the dimensionality from the sequence type.

  switch (ChunkID)
  {
  case LE_CHUNK_ID_SEQ_TWEEKER:
  case LE_CHUNK_ID_SEQ_PRELOADER:
    ReturnDimensionality = 0; // These things have no coordinates of their own.
    goto NormalExit;

  case LE_CHUNK_ID_SEQ_VIDEO:
  case LE_CHUNK_ID_SEQ_2DBITMAP:
    ReturnDimensionality = 2;
    goto NormalExit;

  case LE_CHUNK_ID_SEQ_3DMODEL:
  case LE_CHUNK_ID_SEQ_3DMESH:
    ReturnDimensionality = 3;
    goto NormalExit;
  }

  // Scan through all child chunks to see if there are any
  // which imply the dimensionality.

  CurrentOffset = TargetChunkOffset + sizeof (LE_CHUNK_HeaderRecord) +
    LE_CHUNK_ChunkFixedDataSizes [ChunkID];

  while (CurrentOffset < TargetChunkPastEndOffset)
  {
    ChunkPntr = (LE_CHUNK_HeaderPointer) (BasePntr + CurrentOffset);

    switch (ChunkPntr->chunkID)
    {
    case LE_SEQNCR_CHUNK_ID_DIMENSIONALITY:
      DimensionalityPntr = (LE_SEQNCR_DimensionalityChunkPointer)
        (BasePntr + CurrentOffset + sizeof (LE_CHUNK_HeaderRecord));
      ReturnDimensionality = DimensionalityPntr->dimensionality;
      goto NormalExit;

    case LE_SEQNCR_CHUNK_ID_2D_XFORM:
    case LE_SEQNCR_CHUNK_ID_2D_OFFSET:
    case LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET:
    case LE_SEQNCR_CHUNK_ID_2D_BOUNDING_BOX:
      ReturnDimensionality = 2;
      goto NormalExit;

    case LE_SEQNCR_CHUNK_ID_3D_XFORM:
    case LE_SEQNCR_CHUNK_ID_3D_OFFSET:
    case LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET:
    case LE_SEQNCR_CHUNK_ID_3D_BOUNDING_BOX:
    case LE_SEQNCR_CHUNK_ID_3D_BOUNDING_SPHERE:
    case LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE:
      ReturnDimensionality = 3;
      goto NormalExit;
    }

    CurrentOffset += ChunkPntr->chunkSize;
  }

  // Inherit the dimensionality of the parent, if there is no parent then it
  // will default to zero (usually for top level grouping sequences).

  ParentOffset = LE_CHUNK_GetParentOffset (DataID, TargetChunkOffset);

  if (ParentOffset < 0)
    ReturnDimensionality = 0;
  else
    ReturnDimensionality = LE_CHUNK_FindDimensionality (DataID, ParentOffset);

  goto NormalExit;


ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  {
    char TempString [2048];
    sprintf (TempString, "LE_CHUNK_FindDimensionality: "
      "Failed.  Was trying to find dimensionality of chunk at offset %u in ",
      (unsigned int) TargetChunkOffset);
    LE_DATA_DescribeDataID (DataID, TempString + strlen (TempString),
      sizeof (TempString) - strlen (TempString));
    strcat (TempString, "\r\n");
    LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

NormalExit:
  return ReturnDimensionality;
}



/*****************************************************************************
 * Find the parent chunk of the given chunk.  Returns the offset of the
 * chunk header or -1 if it can't be found.
 */

INT32 LE_CHUNK_GetParentOffset (LE_DATA_DataId DataID, UNS32 TargetChunkOffset)
{
  BYTE                   *BasePntr;
  LE_CHUNK_ID             ChunkID;
  LE_CHUNK_HeaderPointer  ChunkPntr;
  UNS32                   ChunkSize;
  UNS32                   CurrentOffset;
  UNS16                   FixedSize;
  INT32                   PreviousParentOffset = -1;
  INT32                   ReturnOffset = -1;
  UNS32                   TargetChunkSize;
  UNS32                   WholeSize;

  BasePntr = (BYTE *) LE_DATA_Use (DataID);
  if (BasePntr == NULL) goto ErrorExit;

  WholeSize = LE_DATA_GetCurrentSize (DataID);
  if (WholeSize == 0) goto ErrorExit;

  if (TargetChunkOffset + sizeof (LE_CHUNK_HeaderRecord) > WholeSize)
    goto ErrorExit; // User's chunk is outside the file.

  ChunkPntr = (LE_CHUNK_HeaderPointer) (BasePntr + TargetChunkOffset);
  TargetChunkSize = ChunkPntr->chunkSize;

  if (TargetChunkSize + TargetChunkOffset > WholeSize)
    goto ErrorExit; // User's chunk is partially outside the file or bad data.

  // Traverse along the stream past chunks which are before the target
  // chunk, and down into chunks which contain the target chunk.  Don't care
  // about chunks after the target chunk or which are inside it.

  CurrentOffset = 0;
  while (CurrentOffset <= TargetChunkOffset)
  {
    ChunkPntr = (LE_CHUNK_HeaderPointer) (BasePntr + CurrentOffset);
    ChunkSize = ChunkPntr->chunkSize;

    if (TargetChunkOffset < (CurrentOffset + ChunkSize) &&
    TargetChunkOffset >= CurrentOffset)
    {
      // This chunk contains the target chunk or is the target.

      if (CurrentOffset == TargetChunkOffset)
      {
        // This is the target, return the previous parent.

        ReturnOffset = PreviousParentOffset;
        break;
      }

      PreviousParentOffset = CurrentOffset;

      // Look at chunks within this chunk.  Advance past the fixed
      // size part of the current chunk and resume looking there.

      ChunkID = ChunkPntr->chunkID;

      if (ChunkID == LE_CHUNK_ID_NULL_STANDARD ||
      ChunkID >= LE_CHUNK_ID_NULL_IN_CONTEXT)
        goto ErrorExit; // This chunk doesn't contain others, where is target?

      FixedSize = LE_CHUNK_ChunkFixedDataSizes [ChunkID];
      if (FixedSize == 0xFFFF)
        goto ErrorExit; // This chunk doesn't contain others, where is target?

      // Advance to start of child chunks.
      CurrentOffset += FixedSize + sizeof (LE_CHUNK_HeaderRecord);
    }
    else // Not inside this chunk, go to next chunk.
      CurrentOffset += ChunkSize;
  }
  goto NormalExit;

ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  {
    char TempString [2048];
    sprintf (TempString, "LE_CHUNK_GetParentOffset: "
      "Failed.  Unable to find parent of chunk at offset %u in ",
      (unsigned int) TargetChunkOffset);
    LE_DATA_DescribeDataID (DataID, TempString + strlen (TempString),
      sizeof (TempString) - strlen (TempString));
    strcat (TempString, "\r\n");
    LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

NormalExit:
  return ReturnOffset;
}



/*****************************************************************************
 * Converts a time relative to the top level sequence in the DataID into
 * the time for the given subsequence.  Takes into account looping, infinite
 * ending times and other things along the time hierarchy.  Returns -1 if
 * something went wrong, can also return negative times if the absolute
 * time is before the starting time of the sequence or after the ending time.
 */

INT32 LE_CHUNK_ConvertAbsoluteTimeToSequenceTime (LE_DATA_DataId DataID,
UNS32 SequenceChunkOffset, INT32 AbsoluteTime)
{
  LPBYTE                                BasePntr;
  LE_CHUNK_HeaderPointer                ChunkHeaderPntr;
  INT32                                 ParentOffset;
  INT32                                 RelativeTime;
  LE_SEQNCR_SequenceChunkHeaderPointer  SeqPntr;

  ParentOffset = LE_CHUNK_GetParentOffset (DataID, SequenceChunkOffset);

  BasePntr = (LPBYTE) LE_DATA_Use (DataID);
  if (BasePntr == NULL) return -1;

  ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + SequenceChunkOffset);

  SeqPntr = (LE_SEQNCR_SequenceChunkHeaderPointer) (ChunkHeaderPntr + 1);

  if (ParentOffset < 0) // If top level sequence.
    RelativeTime = AbsoluteTime;
  else // Has a parent sequence, recursively get inherited time.
    RelativeTime = LE_CHUNK_ConvertAbsoluteTimeToSequenceTime (DataID,
    ParentOffset, AbsoluteTime);

  RelativeTime -= SeqPntr->parentStartTime;

  if (RelativeTime <= 0)
    return RelativeTime; // Before or at start of the sequence.

  if (SeqPntr->endTime == 0)
    return RelativeTime; // Infinite ending time.

  if (RelativeTime < SeqPntr->endTime)
    return RelativeTime; // Hasn't hit end yet.

  if (SeqPntr->endingAction == LE_SEQNCR_EndingActionStop)
    return -1; // The sequence stops and disappears from the screen.

  if (SeqPntr->endingAction == LE_SEQNCR_EndingActionStayAtEnd)
    return SeqPntr->endTime; // Clock stays frozen at the ending time.

  // Otherwise looping.

  return RelativeTime % SeqPntr->endTime;
}



/*****************************************************************************
 * Recursively finds the absolute time when the given sequence starts,
 * essentially adding up the parent start time of all sequences up to
 * the top level in the DataID.  Returns -1 on failure.
 */

INT32 LE_CHUNK_GetAbsoluteParentStartTime (LE_DATA_DataId DataID,
UNS32 SequenceChunkOffset)
{
  LPBYTE                                BasePntr;
  LE_CHUNK_HeaderPointer                ChunkHeaderPntr;
  INT32                                 DeltaTime;
  INT32                                 ParentOffset;
  INT32                                 ParentTime;
  LE_SEQNCR_SequenceChunkHeaderPointer  SeqPntr;

  BasePntr = (LPBYTE) LE_DATA_Use (DataID);
  if (BasePntr == NULL)
    return -1;

  ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + SequenceChunkOffset);

  SeqPntr = (LE_SEQNCR_SequenceChunkHeaderPointer) (ChunkHeaderPntr + 1);

  DeltaTime = SeqPntr->parentStartTime;

  ParentOffset = LE_CHUNK_GetParentOffset (DataID, SequenceChunkOffset);

  if (ParentOffset < 0) // If top level sequence.
    return DeltaTime;

  ParentTime = LE_CHUNK_GetAbsoluteParentStartTime (DataID, ParentOffset);

  if (ParentTime < 0)
    return -1;

  DeltaTime += ParentTime;

  return DeltaTime;
}



/*****************************************************************************
 * Returns TRUE if the given data type is modifiable by the ModificationType.
 */

BOOL LE_CHUNK_IsDataChunkModifableType (LE_CHUNK_ID ChunkID,
LE_CHUNK_ModifiableTypesOfSequenceData ModificationType)
{
  BOOL  FoundData = FALSE;

  switch (ChunkID)
  {
  case LE_SEQNCR_CHUNK_ID_2D_XFORM:
  case LE_SEQNCR_CHUNK_ID_2D_OFFSET:
  case LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET:
  case LE_SEQNCR_CHUNK_ID_3D_XFORM:
  case LE_SEQNCR_CHUNK_ID_3D_OFFSET:
  case LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET:
    if (ModificationType == LE_CHUNK_MODIFIABLE_TRANSLATION ||
    ModificationType == LE_CHUNK_MODIFIABLE_ROTATION ||
    ModificationType == LE_CHUNK_MODIFIABLE_ORIGIN ||
    ModificationType == LE_CHUNK_MODIFIABLE_SCALING)
      FoundData = TRUE;
    break;

  case LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE:
    if (ModificationType == LE_CHUNK_MODIFIABLE_MESH_INDEX)
      FoundData = TRUE;
    break;

  case LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH:
    if (ModificationType == LE_CHUNK_MODIFIABLE_PITCH_HZ)
      FoundData = TRUE;
    break;

  case LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME:
    if (ModificationType == LE_CHUNK_MODIFIABLE_VOLUME)
      FoundData = TRUE;
    break;

  case LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING:
    if (ModificationType == LE_CHUNK_MODIFIABLE_PANNING)
      FoundData = TRUE;
    break;

  case LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW:
    if (ModificationType == LE_CHUNK_MODIFIABLE_FOV)
      FoundData = TRUE;
    break;
  }

  return FoundData;
}



/*****************************************************************************
 * Look through the children of the given sequence (can be a tweeker or a
 * regular sequence) to find a data chunk which corresponds to the modifiable
 * type of thing.  If GetSecondOne is TRUE then it looks for the second data
 * chunk of the desired kind (useful for tweekers which sometimes have a pair
 * of chunks).  Returns a pointer to the chunk, or NULL if none found.
 */

LE_CHUNK_HeaderPointer LE_CHUNK_GetTweekerOrSequenceDataChunk (
LPBYTE BasePntr, UNS32 SequenceChunkOffset, BOOL GetSecondOne,
LE_CHUNK_ModifiableTypesOfSequenceData ThingToGet)
{
  LE_CHUNK_HeaderPointer                ChunkHeaderPntr;
  LE_CHUNK_ID                           ChunkID;
  UNS32                                 CurrentOffset;
  BOOL                                  FoundData;
  UNS32                                 PastEndOffset;

  ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + SequenceChunkOffset);
  ChunkID = ChunkHeaderPntr->chunkID;

#if CE_ARTLIB_EnableDebugMode
  if (ChunkID < LE_CHUNK_ID_SEQ_GROUPING || ChunkID >= LE_CHUNK_ID_SEQ_MAX)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_GetTweekerOrSequenceDataChunk: "
      "Not a sequence chunk.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return NULL;
  }
#endif

  PastEndOffset = SequenceChunkOffset + ChunkHeaderPntr->chunkSize;

  // Get offset of first child chunk.
  CurrentOffset = SequenceChunkOffset +
    LE_CHUNK_ChunkFixedDataSizes [ChunkID] +
    sizeof (LE_CHUNK_HeaderRecord);

  while (CurrentOffset < PastEndOffset)
  {
    ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + CurrentOffset);
    ChunkID = ChunkHeaderPntr->chunkID;
    FoundData = LE_CHUNK_IsDataChunkModifableType (ChunkID, ThingToGet);

    if (FoundData)
    {
      if (GetSecondOne)
        GetSecondOne = FALSE;
      else
        return ChunkHeaderPntr;
    }

    CurrentOffset += ChunkHeaderPntr->chunkSize;
  }

  return NULL;
}



/*****************************************************************************
 * Utility to make a new data chunk inspired by the modification type.  Writes
 * the new chunk into the buffer you provide in ChunkHeaderPntr, writes at
 * most sizeof (LI_SEQNCR_MaximalDataChunkRecord) bytes (actual amount is in
 * the new chunk's size field).  If needed, will look at the given parent
 * chunk to determine the dimensionality needed and if the subtype is valid.
 * Returns FALSE and writes an error message (chunk type of
 * LE_CHUNK_ID_ANNOTATION) into your chunk if something goes wrong (includes
 * trying to modify a property which isn't applicable to the parent - like
 * changing field of view when the parent isn't a camera sequence).
 */

static BOOL LI_CHUNK_CreateNewDataChunk (LE_CHUNK_HeaderPointer ChunkHeaderPntr,
LE_CHUNK_ModifiableTypesOfSequenceData ThingToModify,
LE_DATA_DataId DataIDOfParent, UNS32 OffsetOfParent)
{
  LPBYTE                              BasePntr;
  LE_SEQNCR_PossibleDataChunkPointers DataPntr;
  int                                 Dimensionality;
  LE_CHUNK_ID                         ParentChunkID;
  char                                TempString [40];

  DataPntr.voidPntr = (ChunkHeaderPntr + 1);

  ChunkHeaderPntr->chunkID = LE_CHUNK_ID_ANNOTATION;
  sprintf (TempString, "Error %d", ThingToModify);
  ChunkHeaderPntr->chunkSize = sizeof (LE_CHUNK_HeaderRecord) +
    strlen (TempString) + 1;
  strcpy ((char *) DataPntr.voidPntr, TempString);

  BasePntr = (BYTE *) LE_DATA_Use (DataIDOfParent);
  if (BasePntr == NULL) return FALSE; // Unable to load data.

  ParentChunkID =
    ((LE_CHUNK_HeaderPointer) (BasePntr + OffsetOfParent))->chunkID;

  // Write a data chunk if possible, if not applicable, just falls through.

  switch (ThingToModify)
  {
  case LE_CHUNK_MODIFIABLE_ORIGIN:
  case LE_CHUNK_MODIFIABLE_TRANSLATION:
  case LE_CHUNK_MODIFIABLE_ROTATION:
  case LE_CHUNK_MODIFIABLE_SCALING:
    Dimensionality =
      LE_CHUNK_FindDimensionality (DataIDOfParent, OffsetOfParent);
    if (Dimensionality == 2)
    {
      ChunkHeaderPntr->chunkID = LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET;
      ChunkHeaderPntr->chunkSize =
        sizeof (LE_SEQNCR_2DOriginScaleRotateOffsetChunkRecord) +
        sizeof (LE_CHUNK_HeaderRecord);
      memset (DataPntr.voidPntr, 0,
        sizeof (LE_SEQNCR_2DOriginScaleRotateOffsetChunkRecord));
      DataPntr.osrt2D->scaleX = 1.0F;
      DataPntr.osrt2D->scaleY = 1.0F;
    }
    else if (Dimensionality == 3)
    {
      ChunkHeaderPntr->chunkID = LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET;
      ChunkHeaderPntr->chunkSize =
        sizeof (LE_SEQNCR_3DOriginScaleRotateOffsetChunkRecord) +
        sizeof (LE_CHUNK_HeaderRecord);
      memset (DataPntr.voidPntr, 0,
        sizeof (LE_SEQNCR_3DOriginScaleRotateOffsetChunkRecord));
      DataPntr.osrt3D->scaleX = 1.0F;
      DataPntr.osrt3D->scaleY = 1.0F;
      DataPntr.osrt3D->scaleZ = 1.0F;
    }
    break;

  case LE_CHUNK_MODIFIABLE_MESH_INDEX:
    if (ParentChunkID == LE_CHUNK_ID_SEQ_3DMESH)
    {
      ChunkHeaderPntr->chunkID = LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE;
      ChunkHeaderPntr->chunkSize =
        sizeof (LE_SEQNCR_3DMeshChoiceChunkRecord) +
        sizeof (LE_CHUNK_HeaderRecord);
      DataPntr.meshChoice->meshIndexA = 0;
      DataPntr.meshChoice->meshIndexB = 0;
      DataPntr.meshChoice->meshProportion = 0.0F;
    }
    break;

  case LE_CHUNK_MODIFIABLE_PITCH_HZ:
    if (ParentChunkID == LE_CHUNK_ID_SEQ_SOUND ||
    ParentChunkID == LE_CHUNK_ID_SEQ_VIDEO)
    {
      ChunkHeaderPntr->chunkID = LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH;
      ChunkHeaderPntr->chunkSize =
        sizeof (LE_SEQNCR_SoundPitchChunkRecord) +
        sizeof (LE_CHUNK_HeaderRecord);
      DataPntr.soundPitch->soundPitch = 0; // Zero means use default pitch.
    }
    break;

  case LE_CHUNK_MODIFIABLE_PANNING:
    if (ParentChunkID == LE_CHUNK_ID_SEQ_SOUND ||
    ParentChunkID == LE_CHUNK_ID_SEQ_VIDEO)
    {
      Dimensionality =
        LE_CHUNK_FindDimensionality (DataIDOfParent, OffsetOfParent);

      if (Dimensionality == 0)
      {
        ChunkHeaderPntr->chunkID = LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING;
        ChunkHeaderPntr->chunkSize =
          sizeof (LE_SEQNCR_SoundPanningChunkRecord) +
          sizeof (LE_CHUNK_HeaderRecord);
        DataPntr.panning->soundPanning = 0; // Center.
      }
    }
    break;

  case LE_CHUNK_MODIFIABLE_VOLUME:
    if (ParentChunkID == LE_CHUNK_ID_SEQ_SOUND ||
    ParentChunkID == LE_CHUNK_ID_SEQ_VIDEO)
    {
      ChunkHeaderPntr->chunkID = LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME;
      ChunkHeaderPntr->chunkSize =
        sizeof (LE_SEQNCR_SoundVolumeChunkRecord) +
        sizeof (LE_CHUNK_HeaderRecord);
      DataPntr.volume->soundVolume = 100; // Full volume.
    }
    break;

  case LE_CHUNK_MODIFIABLE_FOV:
    if (ParentChunkID == LE_CHUNK_ID_SEQ_CAMERA)
    {
      ChunkHeaderPntr->chunkID = LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW;
      ChunkHeaderPntr->chunkSize =
        sizeof (LE_SEQNCR_FieldOfViewChunkRecord) +
        sizeof (LE_CHUNK_HeaderRecord);
      DataPntr.fov->fieldOfView = 1.0F; // Normal 2D field of view and reasonable 3D.
    }
    break;
  }

  return (ChunkHeaderPntr->chunkID != LE_CHUNK_ID_ANNOTATION);
}



/*****************************************************************************
 * Some helper functions for doing interpolation.  They return number A if
 * the proportion is 0.0 and blend towards number B as the proportion goes
 * up to 1.0.  Numbers beyond that range do extrapolation.
 */

inline int IntegerInterpolation (float Proportion, int IntA, int IntB)
{
  return (int) ((1.0F - Proportion) * (float) IntA + Proportion * (float) IntB);
}

inline float FloatInterpolation (float Proportion, float FloatA, float FloatB)
{
  return ((1.0F - Proportion) * FloatA + Proportion * FloatB);
}



/*****************************************************************************
 * Examines the given sequence chunk and tries to find out a particular
 * data value at a given time (time relative to the sequence, not absolute
 * time).  If UseTweekers is FALSE then it just returns the starting value.
 * If UseTweekers is TRUE then it looks at the tweeker subchunks to find the
 * value, and does interpolation if needed.  Returns the new data value in
 * the buffer you provide in OutputBuffer, in chunky format.  If FromKeyFrame
 * is not NULL, then it sets the pointed to flag to TRUE if the data was
 * from a key frame and FALSE if it was interpolated.  Returns FALSE
 * if it can't find the value or something is wrong.
 */

BOOL LE_CHUNK_GetInterpolatedData (LE_DATA_DataId DataID,
UNS32 SequenceChunkOffset, INT32 TimeToGetAt, BOOL UseTweekers,
LE_CHUNK_ModifiableTypesOfSequenceData ThingToGet,
LE_CHUNK_HeaderPointer OutputBuffer, BOOL *FromKeyFrame)
{
  LPBYTE                                BasePntr;
  LE_CHUNK_HeaderPointer                ChunkHeaderPntr;
  LE_CHUNK_ID                           CurrentChunkID;
  UNS32                                 CurrentOffset;
  LE_SEQNCR_InterpolationTypes          InterpolationType;
  LI_SEQNCR_MaximalDataChunkRecord      NewData;
  LE_SEQNCR_PossibleDataChunkPointers   OldDataA;
  LE_SEQNCR_PossibleDataChunkPointers   OldDataB;
  LE_CHUNK_HeaderPointer                OtherChunkHeaderPntr;
  LE_SEQNCR_PossibleDataChunkPointers   OutputData;
  UNS32                                 PastEndOffset;
  float                                 Proportion = 0.0F;
  BOOL                                  PositionTypeModification;
  BOOL                                  ReturnCode = FALSE;
  LE_DATA_DataId                        TempDataID = LE_DATA_EmptyItem;
  LPBYTE                                TempDataPntr;
  LE_SEQNCR_SequenceTweekerChunkPointer TweekerPntr;

  BasePntr = (LPBYTE) LE_DATA_Use (DataID);
  if (BasePntr == NULL) goto ErrorExit;

  PositionTypeModification = (
    ThingToGet == LE_CHUNK_MODIFIABLE_ORIGIN ||
    ThingToGet == LE_CHUNK_MODIFIABLE_TRANSLATION ||
    ThingToGet == LE_CHUNK_MODIFIABLE_ROTATION ||
    ThingToGet == LE_CHUNK_MODIFIABLE_SCALING);

  if (FromKeyFrame != NULL)
    *FromKeyFrame = FALSE; // Not on a key frame is the default.

  ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + SequenceChunkOffset);
  CurrentChunkID = ChunkHeaderPntr->chunkID;

  if (CurrentChunkID < LE_CHUNK_ID_SEQ_GROUPING ||
  CurrentChunkID >= LE_CHUNK_ID_SEQ_MAX)
    goto ErrorExit; // Not a useful sequence, no time or subsequences.

  if (UseTweekers)
  {
    // Find the tweeker containing the desired time, if any.

    if (TimeToGetAt < 0)
      goto ErrorExit; // Nothing exists before the beginning.

    PastEndOffset = SequenceChunkOffset + ChunkHeaderPntr->chunkSize;

    CurrentOffset = SequenceChunkOffset + sizeof (LE_CHUNK_HeaderRecord) +
      LE_CHUNK_ChunkFixedDataSizes [CurrentChunkID];

    while (CurrentOffset < PastEndOffset)
    {
      ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + CurrentOffset);
      if (ChunkHeaderPntr->chunkID == LE_CHUNK_ID_SEQ_TWEEKER)
      {
        TweekerPntr =
          (LE_SEQNCR_SequenceTweekerChunkPointer) (ChunkHeaderPntr + 1);

        if (TweekerPntr->commonHeader.parentStartTime <= TimeToGetAt &&
        (TweekerPntr->commonHeader.endTime == 0 /* Infinite end time */ ||
        TweekerPntr->commonHeader.parentStartTime +
        TweekerPntr->commonHeader.endTime > TimeToGetAt))
        {
          // Have a tweeker which contains the desired time.  Is it the
          // right kind of tweeker?

          OtherChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (
            BasePntr, CurrentOffset, FALSE /* GetSecondOne */, ThingToGet);

          if ((OtherChunkHeaderPntr != NULL) || (PositionTypeModification &&
          TweekerPntr->interpolationType == LE_SEQNCR_INTERPOLATION_IDENTITY))
          {
            // Have a tweeker which has the right kind of modification type.

            InterpolationType =
              (LE_SEQNCR_InterpolationTypes) TweekerPntr->interpolationType;

            if (InterpolationType >= LE_SEQNCR_INTERPOLATION_LINEAR_PAIR &&
            TweekerPntr->commonHeader.endTime > 0 /* Not infinite end time */)
              Proportion =
              (TimeToGetAt - TweekerPntr->commonHeader.parentStartTime) /
              (float) TweekerPntr->commonHeader.endTime;

            if (TweekerPntr->commonHeader.parentStartTime == TimeToGetAt &&
            FromKeyFrame != NULL)
              *FromKeyFrame = TRUE; // We are at the key frame start time.

            break;
          }
        }
      }
      CurrentOffset += ChunkHeaderPntr->chunkSize;
    }

    if (CurrentOffset >= PastEndOffset)
      ChunkHeaderPntr = NULL;
  }
  else // Not using tweekers, just the data in the sequence itself.
  {
    // Note that time doesn't matter, can get initial position even
    // before the start of the sequence (makes for easier user editing).

    OtherChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (
      BasePntr, SequenceChunkOffset, FALSE /* GetSecondOne */, ThingToGet);

    if (OtherChunkHeaderPntr == NULL)
      ChunkHeaderPntr = NULL; // No relevent data in the sequence.
    else
      ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + SequenceChunkOffset);

    InterpolationType = LE_SEQNCR_INTERPOLATION_CONSTANT;
  }

  // Make a copy of the chunk with the data subchunks.  Then we can
  // promote the data and do other things to it.  If there isn't any
  // sequence (or something with no data chunks for it), then make
  // a fake sequence with the default initial data value.

  if (ChunkHeaderPntr == NULL ||
  InterpolationType == LE_SEQNCR_INTERPOLATION_IDENTITY)
  {
    if (!LI_CHUNK_CreateNewDataChunk (&NewData.header, ThingToGet,
    DataID, SequenceChunkOffset))
      goto ErrorExit; // Property isn't applicable to this sequence.

    if ((TempDataID = LE_CHUNK_CreateSequence ()) == LE_DATA_EmptyItem)
      goto ErrorExit;

    if (!LE_CHUNK_InsertChildUnderParentSerial (TempDataID,
    LE_CHUNK_FindSerialNumber (TempDataID, 0), &NewData.header))
      goto ErrorExit;

    InterpolationType = LE_SEQNCR_INTERPOLATION_CONSTANT;
  }
  else // Copy the data from the existing sequence or tweeker chunk.
  {
    TempDataID = LE_DATA_AllocMemoryDataID (ChunkHeaderPntr->chunkSize,
      LE_DATA_DataChunky);
    if (TempDataID == LE_DATA_EmptyItem) goto ErrorExit;

    TempDataPntr = (LPBYTE) LE_DATA_Use (TempDataID);
    if (TempDataPntr == NULL) goto ErrorExit;

    memcpy (TempDataPntr, ChunkHeaderPntr, ChunkHeaderPntr->chunkSize);
  }

  if (!LE_CHUNK_PromoteOrDemoteDataChunks (TempDataID,
  0 /* OffsetToParentChunk */, ThingToGet, TRUE /* Promote */))
    goto ErrorExit;

  BasePntr = (LPBYTE) LE_DATA_Use (TempDataID);
  if (BasePntr == NULL) goto ErrorExit;

  if (InterpolationType < LE_SEQNCR_INTERPOLATION_LINEAR_PAIR)
  {
    // Constant tweekers and sequences just return the value they tweek.

    ChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr,
      0, FALSE /* GetSecondOne */, ThingToGet);
    if (ChunkHeaderPntr == NULL) goto ErrorExit;

    memcpy (OutputBuffer, ChunkHeaderPntr, ChunkHeaderPntr->chunkSize);
    ReturnCode = TRUE;
    goto NormalExit;
  }

  // Interpolate between two different values, fortunately they have been
  // already promoted to the standard data type so it's simplified a bit.

  ChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr,
    0, FALSE /* GetSecondOne */, ThingToGet);
  if (ChunkHeaderPntr == NULL) goto ErrorExit;
  OldDataA.voidPntr = (ChunkHeaderPntr + 1);

  memcpy (OutputBuffer, ChunkHeaderPntr, ChunkHeaderPntr->chunkSize);
  OutputData.voidPntr = (OutputBuffer + 1);

  OtherChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr,
    0, TRUE /* GetSecondOne */, ThingToGet);
  if (OtherChunkHeaderPntr == NULL) goto ErrorExit;
  OldDataB.voidPntr = (OtherChunkHeaderPntr + 1);

  if (ChunkHeaderPntr->chunkID != OtherChunkHeaderPntr->chunkID)
    goto ErrorExit; // Data types should be matching.

  switch (ChunkHeaderPntr->chunkID)
  {
  case LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET:
    OutputData.osrt2D->offset.x = IntegerInterpolation (Proportion, OldDataA.osrt2D->offset.x, OldDataB.osrt2D->offset.x);
    OutputData.osrt2D->offset.y = IntegerInterpolation (Proportion, OldDataA.osrt2D->offset.y, OldDataB.osrt2D->offset.y);
    OutputData.osrt2D->origin.x = IntegerInterpolation (Proportion, OldDataA.osrt2D->origin.x, OldDataB.osrt2D->origin.x);
    OutputData.osrt2D->origin.y = IntegerInterpolation (Proportion, OldDataA.osrt2D->origin.y, OldDataB.osrt2D->origin.y);
    OutputData.osrt2D->scaleX = FloatInterpolation (Proportion, OldDataA.osrt2D->scaleX, OldDataB.osrt2D->scaleX);
    OutputData.osrt2D->scaleY = FloatInterpolation (Proportion, OldDataA.osrt2D->scaleY, OldDataB.osrt2D->scaleY);
    OutputData.osrt2D->rotate = FloatInterpolation (Proportion, OldDataA.osrt2D->rotate, OldDataB.osrt2D->rotate);
    break;

  case LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET:
    OutputData.osrt3D->offset.x = FloatInterpolation (Proportion, OldDataA.osrt3D->offset.x, OldDataB.osrt3D->offset.x);
    OutputData.osrt3D->offset.y = FloatInterpolation (Proportion, OldDataA.osrt3D->offset.y, OldDataB.osrt3D->offset.y);
    OutputData.osrt3D->offset.z = FloatInterpolation (Proportion, OldDataA.osrt3D->offset.z, OldDataB.osrt3D->offset.z);
    OutputData.osrt3D->origin.x = FloatInterpolation (Proportion, OldDataA.osrt3D->origin.x, OldDataB.osrt3D->origin.x);
    OutputData.osrt3D->origin.y = FloatInterpolation (Proportion, OldDataA.osrt3D->origin.y, OldDataB.osrt3D->origin.y);
    OutputData.osrt3D->origin.z = FloatInterpolation (Proportion, OldDataA.osrt3D->origin.z, OldDataB.osrt3D->origin.z);
    OutputData.osrt3D->scaleX = FloatInterpolation (Proportion, OldDataA.osrt3D->scaleX, OldDataB.osrt3D->scaleX);
    OutputData.osrt3D->scaleY = FloatInterpolation (Proportion, OldDataA.osrt3D->scaleY, OldDataB.osrt3D->scaleY);
    OutputData.osrt3D->scaleZ = FloatInterpolation (Proportion, OldDataA.osrt3D->scaleZ, OldDataB.osrt3D->scaleZ);
    OutputData.osrt3D->yaw = FloatInterpolation (Proportion, OldDataA.osrt3D->yaw, OldDataB.osrt3D->yaw);
    OutputData.osrt3D->pitch = FloatInterpolation (Proportion, OldDataA.osrt3D->pitch, OldDataB.osrt3D->pitch);
    OutputData.osrt3D->roll = FloatInterpolation (Proportion, OldDataA.osrt3D->roll, OldDataB.osrt3D->roll);
    break;

  case LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE:
    OutputData.meshChoice->meshIndexA = OldDataA.meshChoice->meshIndexA;
    OutputData.meshChoice->meshIndexB = OldDataA.meshChoice->meshIndexB;
    OutputData.meshChoice->meshProportion = FloatInterpolation (Proportion, OldDataA.meshChoice->meshProportion, OldDataB.meshChoice->meshProportion);
#if CE_ARTLIB_EnableDebugMode
    if (OldDataA.meshChoice->meshIndexA != OldDataB.meshChoice->meshIndexA ||
    OldDataA.meshChoice->meshIndexB != OldDataB.meshChoice->meshIndexB)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_GetInterpolatedData: "
        "Warning - mesh indices don't match.  First mesh choice is  %d, %d, %f"
        "while second one is %d, %d, %f.  At time %d in sequence at offset %d "
        "in DataID 0x%08X.\r\n",
        (int) OldDataA.meshChoice->meshIndexA,
        (int) OldDataA.meshChoice->meshIndexB,
        (double) OldDataA.meshChoice->meshProportion,
        (int) OldDataB.meshChoice->meshIndexA,
        (int) OldDataB.meshChoice->meshIndexB,
        (double) OldDataB.meshChoice->meshProportion,
        (int) TimeToGetAt,
        (int) SequenceChunkOffset,
        (unsigned int) DataID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    }
#endif
    break;

  case LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH:
    OutputData.soundPitch->soundPitch = IntegerInterpolation (Proportion,
      OldDataA.soundPitch->soundPitch, OldDataB.soundPitch->soundPitch);
    break;

  case LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME:
    OutputData.volume->soundVolume = IntegerInterpolation (Proportion,
      OldDataA.volume->soundVolume, OldDataB.volume->soundVolume);
    break;

  case LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING:
    OutputData.panning->soundPanning = IntegerInterpolation (Proportion,
      OldDataA.panning->soundPanning, OldDataB.panning->soundPanning);
    break;

  case LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW:
    OutputData.fov->fieldOfView = FloatInterpolation (Proportion,
      OldDataA.fov->fieldOfView, OldDataB.fov->fieldOfView);
    break;
  }

  ReturnCode = TRUE;
  goto NormalExit;

ErrorExit:
NormalExit:
  if (TempDataID != LE_DATA_EmptyItem)
    LE_DATA_FreeRuntimeDataID (TempDataID);

  return ReturnCode; // bleeble
}



/*****************************************************************************
 * Go through all the data chunks in the given parent sequence and find the
 * ones matching ThingToPromote and promote or demote them.  Promoting
 * means converting them to the most general format - 2D or 3D offset chunks
 * become 2D or 3D origin/scale/rotate/offset chunks.  Demotion goes in the
 * opposite direction, if possible.  If the Promote flag is TRUE then it does
 * promotion, otherwise it does demotion.  Returns TRUE if no errors happened.
 */

BOOL LE_CHUNK_PromoteOrDemoteDataChunks (LE_DATA_DataId DataID,
UNS32 OffsetToParentChunk,
LE_CHUNK_ModifiableTypesOfSequenceData ThingToPromote,
BOOL Promote)
{
#if CE_ARTLIB_EnableDebugMode
  char *ConversionError = "LE_CHUNK_PromoteOrDemoteDataChunks: Conversion not implemented.\r\n";
  #define ConversionError {LE_ERROR_DebugMsg (ConversionError, LE_ERROR_DebugMsgToFileAndScreen); return FALSE;}
#else
#define ConversionError {return FALSE;}
#endif

  const int                           MAX_DATA_CHUNKS = 10;

  LPBYTE                              BasePntr;
  LE_CHUNK_HeaderPointer              ChunkHeaderPntr;
  LI_SEQNCR_MaximalDataChunkRecord    NewDataChunk;
  LE_CHUNK_ID                         CurrentChunkID;
  UNS32                               CurrentOffset;
  UNS32                               DataChunkOffsets [MAX_DATA_CHUNKS];
  int                                 i;
  LE_SEQNCR_PossibleDataChunkPointers NewDataPntr;
  int                                 NumberOfDataChunks;
  LE_SEQNCR_PossibleDataChunkPointers OldDataPntr;
  UNS32                               PastEndOffset;
  LE_SEQNCR_PrivateChunkIDs           SimplestChunkID;
  LE_SEQNCR_PrivateChunkIDs           TargetChunkID = (LE_SEQNCR_PrivateChunkIDs) 0;

  BasePntr = (LPBYTE) LE_DATA_Use (DataID);
  if (BasePntr == NULL) return FALSE;

  ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + OffsetToParentChunk);

  CurrentChunkID = ChunkHeaderPntr->chunkID;

  if (CurrentChunkID < LE_CHUNK_ID_SEQ_GROUPING ||
  CurrentChunkID >= LE_CHUNK_ID_SEQ_MAX)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_PromoteOrDemoteDataChunks: "
      "Chunk type %d isn't a sequence kind of chunk, which we need to hold data, "
      "at offset %d in DataID 0x%08X.\r\n",
      (int) CurrentChunkID,
      (int) OffsetToParentChunk,
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  // Count the number of data chunks and note their positions.  We need to
  // process them in reverse order due to data size changes messing up
  // the offsets.  Also look at them to find the lowest common denominator
  // chunk type when doing demotions.

  PastEndOffset = OffsetToParentChunk + ChunkHeaderPntr->chunkSize;
  CurrentOffset = OffsetToParentChunk + sizeof (LE_CHUNK_HeaderRecord) +
    LE_CHUNK_ChunkFixedDataSizes [CurrentChunkID];
  NumberOfDataChunks = 0;

  // Look for the appropriate data chunks, if they are already there.

  while (CurrentOffset < PastEndOffset && NumberOfDataChunks < MAX_DATA_CHUNKS)
  {
    ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + CurrentOffset);
    CurrentChunkID = ChunkHeaderPntr->chunkID;

    if (LE_CHUNK_IsDataChunkModifableType (CurrentChunkID, ThingToPromote))
    {
      DataChunkOffsets [NumberOfDataChunks++] = CurrentOffset;

      if (Promote)
      {
        TargetChunkID = (LE_SEQNCR_PrivateChunkIDs) CurrentChunkID;
      }
      else // Demoting.
      {
        // Find the most complex type of chunk needed to represent the data
        // when doing demotions.  All the data chunks must be the same kind,
        // for tweekers with multiple data.

        OldDataPntr.voidPntr = (ChunkHeaderPntr + 1);

        switch (CurrentChunkID)
        {
        case LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET:
          if (OldDataPntr.osrt2D->rotate == 0.0F &&
          OldDataPntr.osrt2D->origin.x == 0 &&
          OldDataPntr.osrt2D->origin.y == 0 &&
          OldDataPntr.osrt2D->scaleX == 1.0F &&
          OldDataPntr.osrt2D->scaleY == 1.0F)
            SimplestChunkID = LE_SEQNCR_CHUNK_ID_2D_OFFSET;
          else
            SimplestChunkID = LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET;
          break;

        case LE_SEQNCR_CHUNK_ID_2D_XFORM:
          SimplestChunkID = LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET;
          break;

        case LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET:
          if (OldDataPntr.osrt3D->yaw == 0.0F &&
          OldDataPntr.osrt3D->pitch == 0.0F &&
          OldDataPntr.osrt3D->roll == 0.0F &&
          OldDataPntr.osrt3D->origin.x == 0.0F &&
          OldDataPntr.osrt3D->origin.y == 0.0F &&
          OldDataPntr.osrt3D->origin.z == 0.0F &&
          OldDataPntr.osrt3D->scaleX == 1.0F &&
          OldDataPntr.osrt3D->scaleY == 1.0F &&
          OldDataPntr.osrt3D->scaleZ == 1.0F)
            SimplestChunkID = LE_SEQNCR_CHUNK_ID_3D_OFFSET;
          else
            SimplestChunkID = LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET;
          break;

        case LE_SEQNCR_CHUNK_ID_3D_XFORM:
          SimplestChunkID = LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET;
          break;

        default:
          SimplestChunkID = (LE_SEQNCR_PrivateChunkIDs) CurrentChunkID;
          break;
        }

        if (SimplestChunkID > TargetChunkID)
          TargetChunkID = SimplestChunkID; // Not so simple now.
      }
    }
    CurrentOffset += ChunkHeaderPntr->chunkSize;
  }

  if (NumberOfDataChunks == 0)
    return TRUE; // All done, nothing to do.

#if CE_ARTLIB_EnableDebugMode
  if (NumberOfDataChunks >= MAX_DATA_CHUNKS)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_PromoteOrDemoteDataChunks: "
      "Sequence probably has too many data chunks to handle.  "
      "At offset %d in DataID 0x%08X.\r\n",
      (int) OffsetToParentChunk,
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

  // Need the target chunk type if promoting.  Already have it for demoting.

  if (Promote)
  {
    switch (TargetChunkID)
    {
    case LE_SEQNCR_CHUNK_ID_2D_OFFSET:
    case LE_SEQNCR_CHUNK_ID_2D_XFORM:
      TargetChunkID = LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET;
      break;

    case LE_SEQNCR_CHUNK_ID_3D_OFFSET:
    case LE_SEQNCR_CHUNK_ID_3D_XFORM:
      TargetChunkID = LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET;
      break;
    }
  }

  // Now go through all the data chunks in reverse order and promote or
  // demote them.

  for (i = NumberOfDataChunks - 1; i >= 0; i--)
  {
    ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + DataChunkOffsets [i]);
    CurrentChunkID = ChunkHeaderPntr->chunkID;

#if CE_ARTLIB_EnableDebugMode
    if (ChunkHeaderPntr->chunkSize > sizeof (NewDataChunk))
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_PromoteOrDemoteDataChunks: "
        "Data chunk of type %d is larger than we can handle, shouldn't happen!  "
        "At offset %d in DataID 0x%08X.\r\n",
        (int) ChunkHeaderPntr->chunkID,
        (int) DataChunkOffsets [i],
        (unsigned int) DataID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      return FALSE;
    }
#endif

    if (CurrentChunkID != TargetChunkID)
    {
      // Need to convert the chunk to a different kind.
      // First make the converted chunk locally.

      memset (&NewDataChunk, 0, sizeof (NewDataChunk));
      NewDataChunk.header.chunkID = TargetChunkID;
      NewDataChunk.header.chunkSize =
        LE_CHUNK_SequencerPrivateChunkFixedDataSizes
        [TargetChunkID - LE_CHUNK_ID_NULL_IN_CONTEXT] +
        sizeof (LE_CHUNK_HeaderRecord);
      NewDataPntr.voidPntr = &NewDataChunk.contents;
      OldDataPntr.voidPntr = (ChunkHeaderPntr + 1);

      switch (CurrentChunkID)
      {
      case LE_SEQNCR_CHUNK_ID_2D_OFFSET:
        if (TargetChunkID == LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET)
        {
          NewDataPntr.osrt2D->offset = OldDataPntr.offset2D->offset;
          NewDataPntr.osrt2D->scaleX = 1.0F;
          NewDataPntr.osrt2D->scaleY = 1.0F;
        }
        else
          ConversionError;
        break;

      case LE_SEQNCR_CHUNK_ID_2D_XFORM:
        ConversionError;
        break;

      case LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET:
        if (TargetChunkID == LE_SEQNCR_CHUNK_ID_2D_OFFSET)
          NewDataPntr.offset2D->offset = OldDataPntr.osrt2D->offset;
        else
          ConversionError;
        break;

      case LE_SEQNCR_CHUNK_ID_3D_OFFSET:
        if (TargetChunkID == LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET)
        {
          NewDataPntr.osrt3D->offset = OldDataPntr.offset3D->offset;
          NewDataPntr.osrt3D->scaleX = 1.0F;
          NewDataPntr.osrt3D->scaleY = 1.0F;
          NewDataPntr.osrt3D->scaleZ = 1.0F;
        }
        else
          ConversionError;
        break;

      case LE_SEQNCR_CHUNK_ID_3D_XFORM:
        if (TargetChunkID == LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET)
        {
          LE_MATRIX_3D_GetYPRST (&OldDataPntr.xForm3D->matrix,
            &NewDataPntr.osrt3D->yaw,
            &NewDataPntr.osrt3D->pitch,
            &NewDataPntr.osrt3D->roll,
            &NewDataPntr.osrt3D->scaleX,
            &NewDataPntr.osrt3D->scaleY,
            &NewDataPntr.osrt3D->scaleZ,
            &NewDataPntr.osrt3D->offset);
        }
        else
          ConversionError;
        break;

      case LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET:
        if (TargetChunkID == LE_SEQNCR_CHUNK_ID_3D_OFFSET)
          NewDataPntr.offset3D->offset = OldDataPntr.osrt3D->offset;
        else
          ConversionError;
        break;

      default:
        ConversionError;
        break;
      }

      // Now copy the converted data into the DataID,
      // shrinking or growing it as needed.

      if (!LE_CHUNK_GrowChunk (DataID, DataChunkOffsets [i],
      (int) NewDataChunk.header.chunkSize - (int) ChunkHeaderPntr->chunkSize,
      DataChunkOffsets [i] + ChunkHeaderPntr->chunkSize))
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_PromoteOrDemoteDataChunks: "
          "Error while changing size of a data chunk "
          "at offset %d in DataID 0x%08X.\r\n",
          (int) DataChunkOffsets [i],
          (unsigned int) DataID);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        return FALSE;
      }

      BasePntr = (LPBYTE) LE_DATA_Use (DataID);
      if (BasePntr == NULL) return FALSE;

      memcpy (BasePntr + DataChunkOffsets [i], &NewDataChunk,
        NewDataChunk.header.chunkSize);
    }
  }
  return TRUE;
}



/*****************************************************************************
 * Look for the appropriate subchunk of data and modify it with the new
 * value.  Assumes that the data subchunk being changed has been promoted
 * to the standard full fledged kind of data.  If the data subchunk is
 * missing, will create a new one.  Doesn't delete extra ones.  The given
 * sequence (at OffsetToParentChunkHeader) is either the real sequence
 * (for changing initial position/properties) or the tweeker sequence (for
 * changing interpolated position/properties).  Returns TRUE if it modified
 * the sequence.  Returns FALSE for everything else, including when you try
 * to change something not appropriate to the given sequence's type (or
 * the parent of OffsetToParentChunkHeader if it is a tweeker).
 */

static BOOL LI_CHUNK_ModifyData (LE_DATA_DataId DataID,
UNS32 OffsetToParentChunkHeader,
BOOL RelativeNotAbsolute,
BOOL UseSecondDataChunk,
LE_CHUNK_ModifiableTypesOfSequenceData ThingToModify,
float X, float Y, float Z)
{
  LPBYTE                                BasePntr;
  LE_CHUNK_HeaderPointer                ChunkHeaderPntr;
  UNS32                                 CurrentOffset;
  LE_SEQNCR_PossibleDataChunkPointers   DataPntr;
  LI_SEQNCR_MaximalDataChunkRecord      NewDataChunk;
  LE_CHUNK_ID                           ParentChunkID;
  int                                   TempInt;
  LE_SEQNCR_SequenceTweekerChunkPointer TweekerPntr;

  BasePntr = (LPBYTE) LE_DATA_Use (DataID);
  if (BasePntr == NULL) return FALSE;

  // Look for the data chunk we want to change, create it if necessary.

  while (TRUE)
  {
    ChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (
      BasePntr, OffsetToParentChunkHeader, UseSecondDataChunk,
      ThingToModify);

    // If we didn't find the desired type of data chunk, append an
    // empty data chunk of the right kind and try again (loop in
    // case we have to append two of them).

    if (ChunkHeaderPntr != NULL)
      break; // Found it!

    ChunkHeaderPntr = (LE_CHUNK_HeaderPointer)
      (BasePntr + OffsetToParentChunkHeader);

    ParentChunkID = ChunkHeaderPntr->chunkID;

    // But first see if the parent chunk is allowed to have the kind
    // of data chunk we want to add.

    if (ParentChunkID == LE_CHUNK_ID_SEQ_TWEEKER)
    {
      TweekerPntr =
        (LE_SEQNCR_SequenceTweekerChunkPointer) (ChunkHeaderPntr + 1);

      if (TweekerPntr->interpolationType == LE_SEQNCR_INTERPOLATION_IDENTITY)
      {
        // Identity tweekers set the position only, and it is always the
        // same value, so we can ignore our inputs and claim success if
        // we were given a position.

        return
          (ThingToModify == LE_CHUNK_MODIFIABLE_ORIGIN ||
          ThingToModify == LE_CHUNK_MODIFIABLE_TRANSLATION ||
          ThingToModify == LE_CHUNK_MODIFIABLE_ROTATION ||
          ThingToModify == LE_CHUNK_MODIFIABLE_SCALING);
      }

      if (UseSecondDataChunk &&
      TweekerPntr->interpolationType < LE_SEQNCR_INTERPOLATION_LINEAR_PAIR)
        return FALSE; // No second data item in these kinds of tweeker.

      // The rest of the validation for this tweeker uses the parent
      // to determine what is allowable.

      CurrentOffset =
        LE_CHUNK_GetParentOffset (DataID, OffsetToParentChunkHeader);

      if ((int) CurrentOffset < 0)
        return FALSE; // Something went wrong.

      ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + CurrentOffset);
      ParentChunkID = ChunkHeaderPntr->chunkID;
    }
    else // A regular chunk, acceptance depends on it directly.
      CurrentOffset = OffsetToParentChunkHeader;

    // Add a new data chunk for the kind of modification we are doing.
    // Uses the real (non-tweeker) parent chunk to determine dimensionality
    // and general validity too.  The new chunk's contents are zero or
    // otherwise zeroish values.

    if (!LI_CHUNK_CreateNewDataChunk (&NewDataChunk.header,
    ThingToModify, DataID, CurrentOffset))
      return FALSE; // Desired thing to modify isn't a property of parent.

    ChunkHeaderPntr = (LE_CHUNK_HeaderPointer)
      (BasePntr + OffsetToParentChunkHeader);

    TempInt = OffsetToParentChunkHeader + ChunkHeaderPntr->chunkSize;

    if (!LE_CHUNK_GrowChunk (DataID, OffsetToParentChunkHeader,
    NewDataChunk.header.chunkSize, TempInt))
      return FALSE;

    BasePntr = (LPBYTE) LE_DATA_UseDirty (DataID);
    if (BasePntr == NULL) return FALSE;

    memcpy (BasePntr + TempInt, &NewDataChunk, NewDataChunk.header.chunkSize);

    // Loop back and hunt for the data chunk.
  }

  // Now apply the user's modifications to the existing data.

  DataPntr.voidPntr = (ChunkHeaderPntr + 1);

  switch (ThingToModify)
  {
  case LE_CHUNK_MODIFIABLE_ORIGIN:
    if (ChunkHeaderPntr->chunkID == LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET)
    {
      if (RelativeNotAbsolute)
      {
        DataPntr.osrt2D->origin.x += (TYPE_Coord2D) X;
        DataPntr.osrt2D->origin.y += (TYPE_Coord2D) Y;
      }
      else
      {
        DataPntr.osrt2D->origin.x = (TYPE_Coord2D) X;
        DataPntr.osrt2D->origin.y = (TYPE_Coord2D) Y;
      }
    }
    else if (ChunkHeaderPntr->chunkID == LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET)
    {
      if (RelativeNotAbsolute)
      {
        DataPntr.osrt3D->origin.x += X;
        DataPntr.osrt3D->origin.y += Y;
        DataPntr.osrt3D->origin.z += Z;
      }
      else
      {
        DataPntr.osrt3D->origin.x = X;
        DataPntr.osrt3D->origin.y = Y;
        DataPntr.osrt3D->origin.z = Z;
      }
    }
    break;


  case LE_CHUNK_MODIFIABLE_TRANSLATION:
    if (ChunkHeaderPntr->chunkID == LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET)
    {
      if (RelativeNotAbsolute)
      {
        DataPntr.osrt2D->offset.x += (TYPE_Coord2D) X;
        DataPntr.osrt2D->offset.y += (TYPE_Coord2D) Y;
      }
      else
      {
        DataPntr.osrt2D->offset.x = (TYPE_Coord2D) X;
        DataPntr.osrt2D->offset.y = (TYPE_Coord2D) Y;
      }
    }
    else if (ChunkHeaderPntr->chunkID == LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET)
    {
      if (RelativeNotAbsolute)
      {
        DataPntr.osrt3D->offset.x += X;
        DataPntr.osrt3D->offset.y += Y;
        DataPntr.osrt3D->offset.z += Z;
      }
      else
      {
        DataPntr.osrt3D->offset.x = X;
        DataPntr.osrt3D->offset.y = Y;
        DataPntr.osrt3D->offset.z = Z;
      }
    }
    break;


  case LE_CHUNK_MODIFIABLE_ROTATION:
    if (ChunkHeaderPntr->chunkID == LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET)
    {
      if (RelativeNotAbsolute)
        DataPntr.osrt2D->rotate += X;
      else
        DataPntr.osrt2D->rotate = X;
    }
    else if (ChunkHeaderPntr->chunkID == LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET)
    {
      // Assign to rotation axis by convenient mouse moves:
      // Left-right to yaw, up-down to pitch and in-out to roll.

      if (RelativeNotAbsolute)
      {
        DataPntr.osrt3D->yaw += X;
        DataPntr.osrt3D->pitch += Y;
        DataPntr.osrt3D->roll += Z;
      }
      else
      {
        DataPntr.osrt3D->yaw = X;
        DataPntr.osrt3D->pitch = Y;
        DataPntr.osrt3D->roll = Z;
      }
    }
    break;


  case LE_CHUNK_MODIFIABLE_SCALING:
    if (ChunkHeaderPntr->chunkID == LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET)
    {
      if (RelativeNotAbsolute)
      {
        DataPntr.osrt2D->scaleX *= X;
        DataPntr.osrt2D->scaleY *= Y;
      }
      else
      {
        DataPntr.osrt2D->scaleX = X;
        DataPntr.osrt2D->scaleY = Y;
      }
    }
    else if (ChunkHeaderPntr->chunkID == LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET)
    {
      if (RelativeNotAbsolute)
      {
        DataPntr.osrt3D->scaleX *= X;
        DataPntr.osrt3D->scaleY *= Y;
        DataPntr.osrt3D->scaleZ *= Z;
      }
      else
      {
        DataPntr.osrt3D->scaleX = X;
        DataPntr.osrt3D->scaleY = Y;
        DataPntr.osrt3D->scaleZ = Z;
      }
    }
    break;


  case LE_CHUNK_MODIFIABLE_MESH_INDEX:
    if (ChunkHeaderPntr->chunkID == LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE)
    {
      if (RelativeNotAbsolute)
      {
        DataPntr.meshChoice->meshIndexA += (int) X;
        DataPntr.meshChoice->meshIndexB += (int) Y;
        DataPntr.meshChoice->meshProportion += Z;
      }
      else
      {
        DataPntr.meshChoice->meshIndexA = (int) X;
        DataPntr.meshChoice->meshIndexB = (int) Y;
        DataPntr.meshChoice->meshProportion = Z;
      }

      if (DataPntr.meshChoice->meshIndexA < 0)
        DataPntr.meshChoice->meshIndexA = 0;
      if (DataPntr.meshChoice->meshIndexB < 0)
        DataPntr.meshChoice->meshIndexB = 0;
      if (DataPntr.meshChoice->meshProportion < 0.0F)
        DataPntr.meshChoice->meshProportion = 0.0F;
      if (DataPntr.meshChoice->meshProportion > 1.0F)
        DataPntr.meshChoice->meshProportion = 1.0F;
    }
    break;


  case LE_CHUNK_MODIFIABLE_PITCH_HZ:
    if (ChunkHeaderPntr->chunkID == LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH)
    {
      if (RelativeNotAbsolute)
        TempInt = DataPntr.soundPitch->soundPitch + (int) X;
      else
        TempInt = (int) X;

      if (TempInt < 0)
        TempInt = 0;
      else if (TempInt > 0xFFFFL)
        TempInt = 0xFFFFL;
      DataPntr.soundPitch->soundPitch = TempInt;
    }
    break;


  case LE_CHUNK_MODIFIABLE_PANNING:
    if (ChunkHeaderPntr->chunkID == LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING)
    {
      if (RelativeNotAbsolute)
        TempInt = DataPntr.panning->soundPanning + (int) X;
      else
        TempInt = (int) X;

      if (TempInt < -100)
        TempInt = -100;
      else if (TempInt > 100)
        TempInt = 100;
      DataPntr.panning->soundPanning = TempInt;
    }
    break;


  case LE_CHUNK_MODIFIABLE_VOLUME:
    if (ChunkHeaderPntr->chunkID == LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME)
    {
      if (RelativeNotAbsolute)
        TempInt = DataPntr.volume->soundVolume + (int) X;
      else
        TempInt = (int) X;

      if (TempInt < 0)
        TempInt = 0;
      else if (TempInt > 100)
        TempInt = 100;
      DataPntr.volume->soundVolume = TempInt;
    }
    break;


  case LE_CHUNK_MODIFIABLE_FOV:
    if (ChunkHeaderPntr->chunkID == LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW)
    {
      if (RelativeNotAbsolute)
        DataPntr.fov->fieldOfView += X;
      else
        DataPntr.fov->fieldOfView = X;

      if (DataPntr.fov->fieldOfView < 0.001F)
        DataPntr.fov->fieldOfView = 0.001F; // Keep it over a fraction of a degree.
    }
    break;
  }

  LE_DATA_Dirty (DataID);
  return TRUE;
}



/*****************************************************************************
 * Utility subroutine to find the tweekers before, at and after a given
 * time and inside a given sequence.  Really should be a nested subroutine
 * but C doesn't support that kind of thing.
 */

static void LI_CHUNK_FindTweekerBeforeDuringAndAfter (
LPBYTE BasePntr,
UNS32 ParentSequenceChunkOffset,
INT32 TimeToModify,
LE_CHUNK_ModifiableTypesOfSequenceData ThingToModify,
UNS32 *OffsetToContainingTweeker,
UNS32 *OffsetToNextTweeker,
UNS32 *OffsetToPreviousTweeker)
{
  typedef enum LI_CHUNK_BCAEnum {BEFORE, CONTAINS, AFTER} LI_CHUNK_BCA;

  LE_CHUNK_HeaderPointer                ChunkHeaderPntr;
  UNS32                                 CurrentOffset;
  UNS32                                 PastEndOffset;
  BOOL                                  PositionTypeModification;
  LI_CHUNK_BCA                          Timing;
  LE_SEQNCR_SequenceTweekerChunkPointer TweekerSeqPntr;

  *OffsetToContainingTweeker = 0;
  *OffsetToNextTweeker = 0;
  *OffsetToPreviousTweeker = 0;

  PositionTypeModification = (
    ThingToModify == LE_CHUNK_MODIFIABLE_ORIGIN ||
    ThingToModify == LE_CHUNK_MODIFIABLE_TRANSLATION ||
    ThingToModify == LE_CHUNK_MODIFIABLE_ROTATION ||
    ThingToModify == LE_CHUNK_MODIFIABLE_SCALING);

  ChunkHeaderPntr = (LE_CHUNK_HeaderPointer)
    (BasePntr + ParentSequenceChunkOffset);

  PastEndOffset = ParentSequenceChunkOffset + ChunkHeaderPntr->chunkSize;

  // Get offset of first child chunk (tweekers are children of the animation).

  CurrentOffset = ParentSequenceChunkOffset +
    LE_CHUNK_ChunkFixedDataSizes [ChunkHeaderPntr->chunkID] +
    sizeof (LE_CHUNK_HeaderRecord);

  while (CurrentOffset < PastEndOffset)
  {
    ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + CurrentOffset);

    if (ChunkHeaderPntr->chunkID == LE_CHUNK_ID_SEQ_TWEEKER)
    {
      // See if this tweeker is the kind we are looking for.

      TweekerSeqPntr = (LE_SEQNCR_SequenceTweekerChunkPointer)
        (ChunkHeaderPntr + 1);

      if ((PositionTypeModification &&
      TweekerSeqPntr->interpolationType == LE_SEQNCR_INTERPOLATION_IDENTITY) ||
      LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr, CurrentOffset,
      FALSE /* GetSecondOne */, ThingToModify) != NULL)
      {
        // Yup, this is a significant tweeker.

        // Figure out if this tweeker is before, after or
        // contains the target time.

        if (TweekerSeqPntr->commonHeader.endTime <= 0)
        {
          // Infinite end time.

          if (TweekerSeqPntr->commonHeader.parentStartTime > TimeToModify)
            Timing = AFTER;
          else
            Timing = CONTAINS;
        }
        else // Has a finite ending time.
        {
          if (TweekerSeqPntr->commonHeader.parentStartTime > TimeToModify)
            Timing = AFTER;
          else if (TweekerSeqPntr->commonHeader.parentStartTime +
          TweekerSeqPntr->commonHeader.endTime <= TimeToModify)
            Timing = BEFORE;
          else
            Timing = CONTAINS;
        }

        if (Timing == BEFORE)
          *OffsetToPreviousTweeker = CurrentOffset;
        else if  (Timing == CONTAINS)
          *OffsetToContainingTweeker = CurrentOffset;
        else
        {
          *OffsetToNextTweeker = CurrentOffset;
          break; // Stop searching for tweekers.
        }
      }
    }

    // Advance to the next subchunk (tweeker) inside the parent sequence.

    CurrentOffset +=
      ((LE_CHUNK_HeaderPointer) (BasePntr + CurrentOffset))->chunkSize;
  }
}



/*****************************************************************************
 * Given a sequence, modify its position, scaling, rotation, panning, volume
 * or other data types.  The modification can be done to the initial values
 * (set InterpolationType to LE_SEQNCR_INTERPOLATION_MAX) or to the
 * tweeker subchunks (other interpolation types).  The dimensionality of
 * the sequence controls which of the X, Y, Z values are used (only X, Y for
 * 2D values, XYZ for 3D, just X for non-position things like volume, except
 * mesh choices which use XYZ (index A, index B, proportion).
 *
 * If modification is being done to interpolated values of the sequence
 * (modifying the tweeker subchunks), then change will be done at TimeToModify
 * (ignored for initial value setting).  This will split an existing tweeker
 * into two separate ones if the time isn't the starting time of the tweeker
 * (so you always get a tweeker with a starting time of TimeToModify).  The
 * tweeker at the TimeToModify gets the new value and InterpolationType.  If
 * the previous tweeker is a Linear Pair tweeker then its ending value is
 * updated to match the tweeker at TimeToModify (so it smoothly interpolates
 * into it).  The previous tweeker uses whatever interpolation type it had
 * before.
 *
 * If RelativeNotAbsolute is TRUE then the given value will be combined
 * with the existing value (multiplying for scale factors, adding for
 * everything else).  If FALSE then the given value will be used for the
 * setting directly.
 *
 * To save space, the smallest possible value subchunk will be used.  This
 * means that positions with no scaling or rotation will show up as a
 * simple OFFSET chunk of the appropriate dimensionality.  You can't specify
 * matrix chunks, but if a tweeker has one and you are doing a relative
 * addition, then it will try to covert it to rotations and translations.
 *
 * Returns TRUE if it modified the sequence, FALSE if something went wrong
 * (such as trying to modify an attribute which doesn't exist for the
 * type of sequence - like field of view for non-camera sequences).
 */

BOOL LE_CHUNK_ModifySequenceData (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SequenceSerialNumber,
LE_SEQNCR_InterpolationTypes InterpolationType,
INT32 TimeToModify,
BOOL RelativeNotAbsolute,
LE_CHUNK_ModifiableTypesOfSequenceData ThingToModify,
float X, float Y, float Z)
{
#pragma pack(push,1)
  struct MyTweekerStruct
  {
    LE_CHUNK_HeaderRecord                 header;
    LE_SEQNCR_SequenceTweekerChunkRecord  sequence;
    LE_CHUNK_HeaderRecord                 dopeDataHeader;
    LE_CHUNK_DopeDataRecord               dopeDataContents;
    BYTE                                  subchunks [2 * sizeof (LI_SEQNCR_MaximalDataChunkRecord)];
  }                                     NewTweekerChunk;
#pragma pack(pop)

  LPBYTE                                BasePntr;
  LE_CHUNK_HeaderPointer                ChunkHeaderPntr;
  LE_CHUNK_HeaderPointer                OtherChunkHeaderPntr;
  int                                   MainOffset;
  LI_SEQNCR_MaximalDataChunkRecord      NewFirstData;
  int                                   NewOffset;
  LPBYTE                                NewPntr;
  LI_SEQNCR_MaximalDataChunkRecord      NewSecondData;
  UNS32                                 OffsetToContainingTweeker;
  UNS32                                 OffsetToNextTweeker;
  UNS32                                 OffsetToPreviousTweeker;
  INT32                                 ParentEndTime;
  BOOL                                  ReturnCode = FALSE;
  LE_SEQNCR_SequenceTweekerChunkPointer TweekerSeqPntr;

#if CE_ARTLIB_EnableDebugMode
  if (ThingToModify <= LE_CHUNK_MODIFIABLE_NOTHING ||
  ThingToModify >= LE_CHUNK_MODIFIABLE_MAX)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_ModifySequenceData: "
      "Type of modification isn't valid.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE; // Bad arguments.
  }
#endif

  MainOffset = LE_CHUNK_FindSerialOffset (DataID, SequenceSerialNumber);
  if (MainOffset < 0)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_ModifySequenceData: "
      "Unable to find serial number %d in DataID 0x%08X.\r\n",
      (int) SequenceSerialNumber,
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  // Get the simple situation out of the way first - setting the initial value.

  if (InterpolationType >= LE_SEQNCR_INTERPOLATION_MAX)
  {
    return
      LI_CHUNK_ModifyData (DataID, MainOffset, RelativeNotAbsolute,
      FALSE /* UseSecondDataChunk */, ThingToModify, X, Y, Z);
  }

  // Identity tweekers can only be used for position type changes since
  // they set the position matrix to the identity matrix.

  if (InterpolationType == LE_SEQNCR_INTERPOLATION_IDENTITY)
  {
    if (ThingToModify != LE_CHUNK_MODIFIABLE_TRANSLATION &&
    ThingToModify != LE_CHUNK_MODIFIABLE_ORIGIN &&
    ThingToModify != LE_CHUNK_MODIFIABLE_ROTATION &&
    ThingToModify != LE_CHUNK_MODIFIABLE_SCALING)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_ModifySequenceData: "
        "Can only use the LE_SEQNCR_INTERPOLATION_IDENTITY interpolation "
        "type on position related things.\r\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return FALSE;
    }
  }

  // If the time being modified is before the start of the sequence, just
  // ignore it since we can't change things there and this causes bugs.
  // We do allow changes past the ending time of the sequence since they
  // are mostly harmless (just space wasting) and may be useful for
  // sequences with infinite end time.  Anyway, when the dope user adds a
  // new item to the sequence, it will get resized to include the tweekers
  // which were formerly past its ending time.

  if (TimeToModify < 0)
    return TRUE;

  // Find three tweekers for the given kind of data (ignore other ones)
  // and note the locations of the previous one, the one containing the
  // given time, and the next one after that (assuming that the tweekers
  // are in increasing time order - normally the rule for sequences).

  BasePntr = (LPBYTE) LE_DATA_Use (DataID);

  LI_CHUNK_FindTweekerBeforeDuringAndAfter (BasePntr, MainOffset,
    TimeToModify, ThingToModify, &OffsetToContainingTweeker,
    &OffsetToNextTweeker, &OffsetToPreviousTweeker);

  // If there aren't any tweekers containing the given time, create a new
  // one lasting from the end of the previous tweeker to the start of the
  // next tweeker (or start / end of sequence as appropriate).  It inherits
  // the initial and final values / proportions from the prior / next tweekers
  // in the sequence.  The priority gets set to the type of data being
  // modified, so that you don't get overlapping sequences in the dope sheet
  // if you happen to view it (which would also give strange results when
  // the time multiplying routines get ahold of it since they fix things
  // which overlap).

  if (OffsetToContainingTweeker == 0)
  {
    memset (&NewFirstData, 0, sizeof (NewFirstData));
    memset (&NewSecondData, 0, sizeof (NewSecondData));

    if (InterpolationType >= LE_SEQNCR_INTERPOLATION_CONSTANT)
    {
      // Need to have at least one tweeker data chunk.  Try to get the
      // value from the previous tweeker, if any, or the following
      // if none.

      ChunkHeaderPntr = NULL;

      if (ChunkHeaderPntr == NULL && OffsetToPreviousTweeker != 0)
        ChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr,
        OffsetToPreviousTweeker, TRUE /* GetSecondOne */, ThingToModify);

      if (ChunkHeaderPntr == NULL && OffsetToPreviousTweeker != 0)
        ChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr,
        OffsetToPreviousTweeker, FALSE /* GetSecondOne */, ThingToModify);

      if (ChunkHeaderPntr == NULL && OffsetToNextTweeker != 0)
        ChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr,
        OffsetToNextTweeker, FALSE /* GetSecondOne */, ThingToModify);

      if (ChunkHeaderPntr == NULL)
      {
        if (!LI_CHUNK_CreateNewDataChunk (&NewFirstData.header, ThingToModify,
        DataID, MainOffset))
          goto ErrorExit; // ThingToModify isn't an applicable property to set.
      }
      else
        memcpy (&NewFirstData, ChunkHeaderPntr, ChunkHeaderPntr->chunkSize);
    }

    if (InterpolationType >= LE_SEQNCR_INTERPOLATION_LINEAR_PAIR)
    {
      // Need to have a second tweeker data chunk too, get the initial
      // value from the following tweeker or from the previous if none.

      ChunkHeaderPntr = NULL;

      if (ChunkHeaderPntr == NULL && OffsetToNextTweeker != 0)
        ChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr,
        OffsetToNextTweeker, FALSE /* GetSecondOne */, ThingToModify);

      if (ChunkHeaderPntr == NULL && OffsetToPreviousTweeker != 0)
        ChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr,
        OffsetToPreviousTweeker, TRUE /* GetSecondOne */, ThingToModify);

      if (ChunkHeaderPntr == NULL && OffsetToPreviousTweeker != 0)
        ChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr,
        OffsetToPreviousTweeker, FALSE /* GetSecondOne */, ThingToModify);

      if (ChunkHeaderPntr == NULL)
      {
        if (!LI_CHUNK_CreateNewDataChunk (&NewSecondData.header, ThingToModify,
        DataID, MainOffset))
          goto ErrorExit; // ThingToModify isn't an applicable property to set.
      }
      else
        memcpy (&NewSecondData, ChunkHeaderPntr, ChunkHeaderPntr->chunkSize);
    }

    // Set up the new tweeker sequence.

    memset (&NewTweekerChunk, 0, sizeof (NewTweekerChunk));

    NewTweekerChunk.header.chunkID = LE_CHUNK_ID_SEQ_TWEEKER;

    // Set the starting time.  Either the beginning of time or the ending
    // of the previous tweeker (which we know isn't infinite otherwise
    // we wouldn't be building this containing tweeker).

    if (OffsetToPreviousTweeker != 0)
    {
      TweekerSeqPntr = (LE_SEQNCR_SequenceTweekerChunkPointer)
        (BasePntr + OffsetToPreviousTweeker + sizeof (LE_CHUNK_HeaderRecord));

      NewTweekerChunk.sequence.commonHeader.parentStartTime =
        TweekerSeqPntr->commonHeader.parentStartTime +
        TweekerSeqPntr->commonHeader.endTime;
    }
    else
      NewTweekerChunk.sequence.commonHeader.parentStartTime = 0;

    // Ending time, either infinite or stop at the next tweeker.

    if (OffsetToNextTweeker != 0)
    {
      TweekerSeqPntr = (LE_SEQNCR_SequenceTweekerChunkPointer)
        (BasePntr + OffsetToNextTweeker + sizeof (LE_CHUNK_HeaderRecord));

      NewTweekerChunk.sequence.commonHeader.endTime =
        TweekerSeqPntr->commonHeader.parentStartTime -
        NewTweekerChunk.sequence.commonHeader.parentStartTime;
    }
    else
      NewTweekerChunk.sequence.commonHeader.endTime = 0; // Infinite.

    // Use category of thing as the priority, to avoid overlap with
    // multiple kinds of tweeker occuring at the same time (like
    // having a volume tweeker and a panning tweeker both changing
    // the same sound - need to have different priorities).

    NewTweekerChunk.sequence.commonHeader.priority = ThingToModify;

    // Properties should be for fast updates since tweekers interpolate
    // nicely and we want that that smoothness from full frame rates.
    // Anyway, the parent object's time multiple will override this
    // so it won't waste CPU time on slow animations if we set the
    // tweeker to update frequently.

    NewTweekerChunk.sequence.commonHeader.timeMultiple = 1;
    NewTweekerChunk.sequence.commonHeader.dropFrames = 1;
    NewTweekerChunk.sequence.commonHeader.endingAction = LE_SEQNCR_EndingActionStop;

    NewTweekerChunk.sequence.interpolationType = InterpolationType;

    // Add the dope data child chunk since it is a sequence.

    NewTweekerChunk.dopeDataHeader.chunkID = LE_CHUNK_ID_DOPE_DATA;
    NewTweekerChunk.dopeDataHeader.chunkSize =
      sizeof (LE_CHUNK_HeaderRecord) + sizeof (LE_CHUNK_DopeDataRecord);
    NewTweekerChunk.dopeDataContents.serialNumber =
      LE_CHUNK_GetNextSerialNumber();

    // Add the child chunks - containing tweeking data, 0, 1 or 2 of them.

    NewPntr = NewTweekerChunk.subchunks;

    if (NewFirstData.header.chunkSize > 0)
    {
      memcpy (NewPntr, &NewFirstData, NewFirstData.header.chunkSize);
      NewPntr += NewFirstData.header.chunkSize;
    }

    if (NewSecondData.header.chunkSize > 0)
    {
      memcpy (NewPntr, &NewSecondData, NewSecondData.header.chunkSize);
      NewPntr += NewSecondData.header.chunkSize;
    }

    NewTweekerChunk.header.chunkSize = NewPntr - (LPBYTE) &NewTweekerChunk;

    // Put the new tweeker into the parent sequence and start over.

    if (!LE_CHUNK_InsertChildUnderParentSerial (DataID, SequenceSerialNumber,
    &NewTweekerChunk.header))
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_ModifySequenceData: "
        "Failed to insert new tweeker chunk.\r\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    // Adjust the new tweeker so that its data chunks match each other and
    // are in minimal size format.

    NewOffset = LE_CHUNK_FindSerialOffset (DataID,
      NewTweekerChunk.dopeDataContents.serialNumber);
    if (NewOffset < 0) goto ErrorExit;

    if (!LE_CHUNK_PromoteOrDemoteDataChunks (DataID, NewOffset,
    ThingToModify, TRUE /* Promote */))
      goto ErrorExit;

    if (!LE_CHUNK_PromoteOrDemoteDataChunks (DataID, NewOffset,
    ThingToModify, FALSE /* Promote */))
      goto ErrorExit;

    BasePntr = (LPBYTE) LE_DATA_Use (DataID);

    LI_CHUNK_FindTweekerBeforeDuringAndAfter (BasePntr, MainOffset,
      TimeToModify, ThingToModify, &OffsetToContainingTweeker,
      &OffsetToNextTweeker, &OffsetToPreviousTweeker);
  }

#if CE_ARTLIB_EnableDebugMode
  if (OffsetToContainingTweeker == 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_ModifySequenceData: "
      "There should be a containing tweeker by now.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit; // Should be a containing tweeker by now.
  }
#endif

  // If the containing tweeker doesn't have the time as the starting time,
  // split it into two tweekers at the time point.  Also do a bit of
  // interpolation so that the split point's value is proportional
  // to the time of the split.

  ChunkHeaderPntr = (LE_CHUNK_HeaderPointer)
    (BasePntr + OffsetToContainingTweeker);

  TweekerSeqPntr = (LE_SEQNCR_SequenceTweekerChunkPointer)
    (ChunkHeaderPntr + 1);

  if (TweekerSeqPntr->commonHeader.parentStartTime < TimeToModify)
  {
    // Yes, have to split the containing tweeker.  First make new data
    // chunks for the SECOND half tweeker.  The first data chunk is the
    // data value at the proportional time and the second one (if needed)
    // is the same as the original's ending data value.

    memset (&NewFirstData, 0, sizeof (NewFirstData));
    memset (&NewSecondData, 0, sizeof (NewSecondData));

    if (InterpolationType >= LE_SEQNCR_INTERPOLATION_CONSTANT)
    {
      // Need to have at least one tweeker data chunk.  This one
      // uses the proportional data value from the split tweeker.

      if (!LE_CHUNK_GetInterpolatedData (DataID, MainOffset,
      TimeToModify, TRUE /* UseTweekers */, ThingToModify,
      &NewFirstData.header, NULL))
      {
        // Didn't get it, just try copying the data chunk from the
        // tweeker being split.  If that doesn't work, make it up.

        ChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr,
          OffsetToContainingTweeker, FALSE /* GetSecondOne */, ThingToModify);

        if (ChunkHeaderPntr == NULL)
        {
          if (!LI_CHUNK_CreateNewDataChunk (&NewFirstData.header, ThingToModify,
          DataID, MainOffset))
          {
#if CE_ARTLIB_EnableDebugMode
            sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_ModifySequenceData: "
              "LI_CHUNK_CreateNewDataChunk mysteriously failed when making "
              "first data chunk for new split tweeker.\r\n");
            LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
            goto ErrorExit;
          }
        }
        else
          memcpy (&NewFirstData, ChunkHeaderPntr, ChunkHeaderPntr->chunkSize);
      }
    }

    if (InterpolationType >= LE_SEQNCR_INTERPOLATION_LINEAR_PAIR)
    {
      // Need to have a second tweeker data chunk too.  Copy it from
      // the second data chunk in the tweeker being split.

      ChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr,
        OffsetToContainingTweeker, TRUE /* GetSecondOne */, ThingToModify);

      if (ChunkHeaderPntr == NULL)
        ChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr,
        OffsetToContainingTweeker, FALSE /* GetSecondOne */, ThingToModify);

      if (ChunkHeaderPntr == NULL)
      {
        if (!LI_CHUNK_CreateNewDataChunk (&NewSecondData.header, ThingToModify,
        DataID, MainOffset))
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_ModifySequenceData: "
            "LI_CHUNK_CreateNewDataChunk mysteriously failed when making "
            "second data chunk for new split tweeker.\r\n");
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
          goto ErrorExit;
        }
      }
      else
        memcpy (&NewSecondData, ChunkHeaderPntr, ChunkHeaderPntr->chunkSize);
    }

    // Cut down the duration of the tweeker being split so that it
    // can become the tweeker for the first half of the time.  Note
    // that the second data subchunk in this first tweeker will be
    // adjusted later on (not now) to match the first data subchunk
    // in the second tweeker.

    ParentEndTime = TweekerSeqPntr->commonHeader.endTime;
    if (ParentEndTime != 0) // If not infinite time.
      ParentEndTime += TweekerSeqPntr->commonHeader.parentStartTime;
    TweekerSeqPntr->commonHeader.endTime =
      TimeToModify - TweekerSeqPntr->commonHeader.parentStartTime;

    // Make a new tweeker sequence for the second half of the time.

    memset (&NewTweekerChunk, 0, sizeof (NewTweekerChunk));

    NewTweekerChunk.header.chunkID = LE_CHUNK_ID_SEQ_TWEEKER;
    NewTweekerChunk.sequence.commonHeader.parentStartTime = TimeToModify;
    if (ParentEndTime != 0)
      NewTweekerChunk.sequence.commonHeader.endTime =
      ParentEndTime - TimeToModify;

    NewTweekerChunk.sequence.commonHeader.priority = ThingToModify;
    NewTweekerChunk.sequence.commonHeader.timeMultiple = 1;
    NewTweekerChunk.sequence.commonHeader.dropFrames = 1;
    NewTweekerChunk.sequence.commonHeader.endingAction = LE_SEQNCR_EndingActionStop;

    NewTweekerChunk.sequence.interpolationType = InterpolationType;

    // Add the dope data child chunk.

    NewTweekerChunk.dopeDataHeader.chunkID = LE_CHUNK_ID_DOPE_DATA;
    NewTweekerChunk.dopeDataHeader.chunkSize =
      sizeof (LE_CHUNK_HeaderRecord) + sizeof (LE_CHUNK_DopeDataRecord);
    NewTweekerChunk.dopeDataContents.serialNumber =
      LE_CHUNK_GetNextSerialNumber();

    // Add the data subchunks, first one is the interpolated data value
    // and the second is the same as the split tweeker's ending value.
    // The data has already been prepared, since it needed to use the
    // unsplit data.

    NewPntr = NewTweekerChunk.subchunks;

    if (NewFirstData.header.chunkSize > 0)
    {
      memcpy (NewPntr, &NewFirstData, NewFirstData.header.chunkSize);
      NewPntr += NewFirstData.header.chunkSize;
    }

    if (NewSecondData.header.chunkSize > 0)
    {
      memcpy (NewPntr, &NewSecondData, NewSecondData.header.chunkSize);
      NewPntr += NewSecondData.header.chunkSize;
    }

    NewTweekerChunk.header.chunkSize = NewPntr - (LPBYTE) &NewTweekerChunk;

    // Put the new split tweeker into the sequence and start over.

    if (!LE_CHUNK_InsertChildUnderParentSerial (DataID, SequenceSerialNumber,
    &NewTweekerChunk.header))
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_ModifySequenceData: "
        "Failed to insert new tweeker chunk.\r\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    // Adjust the new tweeker so that its data chunks match
    // each other and are in minimal size format.

    NewOffset = LE_CHUNK_FindSerialOffset (DataID,
      NewTweekerChunk.dopeDataContents.serialNumber);
    if (NewOffset < 0) goto ErrorExit;

    if (!LE_CHUNK_PromoteOrDemoteDataChunks (DataID, NewOffset,
    ThingToModify, TRUE /* Promote */))
      goto ErrorExit;

    if (!LE_CHUNK_PromoteOrDemoteDataChunks (DataID, NewOffset,
    ThingToModify, FALSE /* Promote */))
      goto ErrorExit;

    BasePntr = (LPBYTE) LE_DATA_Use (DataID);

    LI_CHUNK_FindTweekerBeforeDuringAndAfter (BasePntr, MainOffset,
      TimeToModify, ThingToModify, &OffsetToContainingTweeker,
      &OffsetToNextTweeker, &OffsetToPreviousTweeker);
  }

#if CE_ARTLIB_EnableDebugMode
  if (OffsetToContainingTweeker == 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_ModifySequenceData: "
      "There should be a containing tweeker after the split.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit; // Should be a containing tweeker by now.
  }

  ChunkHeaderPntr = (LE_CHUNK_HeaderPointer)
    (BasePntr + OffsetToContainingTweeker);

  TweekerSeqPntr = (LE_SEQNCR_SequenceTweekerChunkPointer)
    (ChunkHeaderPntr + 1);

  if (TweekerSeqPntr->commonHeader.parentStartTime != TimeToModify)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_ModifySequenceData: "
      "Containing tweeker after the split should start at the start time.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit; // Should start at TimeToModify.
  }
#endif

  // Adjust the containing tweeker's first value to
  // reflect the user's changes to the value.

  if (!LE_CHUNK_PromoteOrDemoteDataChunks (DataID, OffsetToContainingTweeker,
  ThingToModify, TRUE /* Promote */))
    goto ErrorExit;

  if (!LI_CHUNK_ModifyData (DataID, OffsetToContainingTweeker, RelativeNotAbsolute,
  FALSE /* UseSecondDataChunk */, ThingToModify, X, Y, Z))
    goto ErrorExit;

  if (!LE_CHUNK_PromoteOrDemoteDataChunks (DataID, OffsetToContainingTweeker,
  ThingToModify, FALSE /* Promote */))
    goto ErrorExit;

  // Copy the current tweeker's first value to the previous tweeker's
  // final value (if present, if meaningful).

  BasePntr = (LPBYTE) LE_DATA_Use (DataID);

  LI_CHUNK_FindTweekerBeforeDuringAndAfter (BasePntr, MainOffset,
    TimeToModify, ThingToModify, &OffsetToContainingTweeker,
    &OffsetToNextTweeker, &OffsetToPreviousTweeker);

  if (OffsetToContainingTweeker != 0 && OffsetToPreviousTweeker != 0)
  {
    // Look at the previous tweeker.

    ChunkHeaderPntr = (LE_CHUNK_HeaderPointer)
      (BasePntr + OffsetToPreviousTweeker);

    TweekerSeqPntr = (LE_SEQNCR_SequenceTweekerChunkPointer)
      (ChunkHeaderPntr + 1);

    // Only bother setting it if the previous tweeker has a pair of values.

    if (TweekerSeqPntr->interpolationType >= LE_SEQNCR_INTERPOLATION_LINEAR_PAIR)
    {
      // Need to promote both tweekers to full data size so that we can copy
      // the data chunks from one to the other.

      if (!LE_CHUNK_PromoteOrDemoteDataChunks (DataID, OffsetToContainingTweeker,
      ThingToModify, TRUE /* Promote */))
        goto ErrorExit;

      if (!LE_CHUNK_PromoteOrDemoteDataChunks (DataID, OffsetToPreviousTweeker,
      ThingToModify, TRUE /* Promote */))
        goto ErrorExit;

      BasePntr = (LPBYTE) LE_DATA_Use (DataID);

      LI_CHUNK_FindTweekerBeforeDuringAndAfter (BasePntr, MainOffset,
        TimeToModify, ThingToModify, &OffsetToContainingTweeker,
        &OffsetToNextTweeker, &OffsetToPreviousTweeker);

      // Get the data from the updated tweeker's first value.

      OtherChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr,
        OffsetToContainingTweeker, FALSE /* GetSecondOne */, ThingToModify);

      // Will copy it to the previous tweeker's final value.

      ChunkHeaderPntr = LE_CHUNK_GetTweekerOrSequenceDataChunk (BasePntr,
        OffsetToPreviousTweeker, TRUE /* GetSecondOne */, ThingToModify);

      // But only copy if the data chunk is the same kind (should be).

      if (ChunkHeaderPntr != NULL && OtherChunkHeaderPntr != NULL &&
      ChunkHeaderPntr->chunkID == OtherChunkHeaderPntr->chunkID &&
      ChunkHeaderPntr->chunkSize == OtherChunkHeaderPntr->chunkSize)
      {
        memcpy (ChunkHeaderPntr, OtherChunkHeaderPntr,
          OtherChunkHeaderPntr->chunkSize);
      }

      // Demote things back to normal for both tweekers.

      if (!LE_CHUNK_PromoteOrDemoteDataChunks (DataID, OffsetToContainingTweeker,
      ThingToModify, FALSE /* Promote */))
        goto ErrorExit;

      if (!LE_CHUNK_PromoteOrDemoteDataChunks (DataID, OffsetToPreviousTweeker,
      ThingToModify, FALSE /* Promote */))
        goto ErrorExit;
    }
  }

ErrorExit:
  return ReturnCode;
}



/*****************************************************************************
 * Given a sequence, delete the interpolation tweeker which modifies
 * the given attribute at the given time.  The tweeker containing the
 * TimeToModify time will be deleted and the previous one will be updated
 * to have its ending value set to the starting value of the tweeker after
 * the deleted one and its duration increased to cover the extra time
 * that the deleted tweeker occupied.
 *
 * To save space, the smallest possible value subchunk will be used.  This
 * means that positions with no scaling or rotation will show up as a
 * simple OFFSET chunk of the appropriate dimensionality.
 *
 * Returns TRUE if it modified the sequence, FALSE if something went wrong
 * (such as trying to modify an attribute which doesn't exist for the
 * type of sequence - like field of view for non-camera sequences).
 */

BOOL LE_CHUNK_DeleteTweeker (LE_DATA_DataId DataID,
LE_CHUNK_SerialNumber SequenceSerialNumber,
INT32 TimeToModify,
LE_CHUNK_ModifiableTypesOfSequenceData ThingToModify)
{
  return FALSE; // bleeble
}



/*****************************************************************************
 * Adjusts the given chunk's duration to be longer or shorter by the given
 * scale factor.  Can recursively adjusts the duration of child chunks,
 * and makes the child chunks start at proportionaly scaled times too.  Won't
 * make a chunk duration less than 1 (except for chunks which already have
 * zero duration - infinite end time).  If MultiplyNotAdd is TRUE then the
 * duration is multiplied by the TimeFactor, otherwise TimeFactor is added
 * to the duration (can be negative).  If RecursiveDepth is 0 it just changes
 * the given children.  If RecursiveDepth is 1 then it changes the immediate
 * children of the sequence but doesn't change their children (useful for
 * changing the children inside a grouping sequence).  Larger depths affect
 * more children.    Doesn't handle indirect grouping sequences.  Doesn't
 * cause the DataID to grow, so pointers to it stay valid after calling this.
 * Returns TRUE if successful.
 */

BOOL LE_CHUNK_ChangeDuration (LE_DATA_DataId DataID, UNS32 Offset,
float TimeFactor, BOOL MultiplyNotAdd, int RecursiveDepth)
{
  LPBYTE                                BasePntr;
  LE_CHUNK_HeaderPointer                ChunkHeaderPntr;
  LE_CHUNK_ID                           ChunkID;
  INT32                                 CurrentHighEndTimes [256]; // One for each priority level.
  UNS32                                 CurrentOffset;
  INT32                                 CurrentStartTime;
  INT32                                 OverallHighEndTime;
  UNS32                                 PastEndOffset;
  int                                   Priority;
  BOOL                                  ReturnCode = FALSE;
  LE_SEQNCR_SequenceChunkHeaderPointer  SeqHeaderPntr;

  if (MultiplyNotAdd && TimeFactor < 0.0F)
    goto ErrorExit; // Should be zero or positive when multiplying.

  BasePntr = (LPBYTE) LE_DATA_UseDirty (DataID);
  if (BasePntr == NULL) goto ErrorExit;

  ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + Offset);
  ChunkID = ChunkHeaderPntr->chunkID;

  if (ChunkID < LE_CHUNK_ID_SEQ_GROUPING || ChunkID >= LE_CHUNK_ID_SEQ_MAX)
    goto ErrorExit; // Corrupt data or bad arguments.

  // Recursively update the child chunks.  Also make sure that the child chunks
  // don't overlap, push overlapping ones ahead in time until they don't.

  RecursiveDepth--;

  PastEndOffset = Offset + ChunkHeaderPntr->chunkSize;
  CurrentOffset = Offset + sizeof (LE_CHUNK_HeaderRecord) +
    LE_CHUNK_ChunkFixedDataSizes [ChunkID];

  memset (&CurrentHighEndTimes, 0, sizeof (CurrentHighEndTimes));
  OverallHighEndTime = 0;
  CurrentStartTime = 0;

  while (CurrentOffset < PastEndOffset)
  {
    ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + CurrentOffset);
    ChunkID = ChunkHeaderPntr->chunkID;

    if (ChunkID >= LE_CHUNK_ID_SEQ_GROUPING && ChunkID < LE_CHUNK_ID_SEQ_MAX)
    {
      SeqHeaderPntr =
        (LE_SEQNCR_SequenceChunkHeaderPointer) (ChunkHeaderPntr + 1);

      if (RecursiveDepth >= 0)
      {
        if (!LE_CHUNK_ChangeDuration (DataID, CurrentOffset,
        TimeFactor, MultiplyNotAdd, RecursiveDepth))
          goto ErrorExit;
      }

      Priority = SeqHeaderPntr->priority;

      // Fix up the starting time so it is monotonically increasing within
      // the children and so that it doesn't overlap with the previous
      // child in the same priority column.

      if (SeqHeaderPntr->parentStartTime < CurrentStartTime)
        SeqHeaderPntr->parentStartTime = CurrentStartTime;

      if (SeqHeaderPntr->parentStartTime < CurrentHighEndTimes [Priority])
        SeqHeaderPntr->parentStartTime = CurrentHighEndTimes [Priority];

      CurrentStartTime = SeqHeaderPntr->parentStartTime;

      if (SeqHeaderPntr->endTime <= 0) // If infinite ending time.
        CurrentHighEndTimes [Priority] =
        CurrentStartTime + 1; // Avoid exact overlap problem.
      else
        CurrentHighEndTimes [Priority] =
        SeqHeaderPntr->parentStartTime + SeqHeaderPntr->endTime;

      if (OverallHighEndTime < CurrentHighEndTimes [Priority])
        OverallHighEndTime = CurrentHighEndTimes [Priority];
    }
    CurrentOffset += ChunkHeaderPntr->chunkSize;
  }

  // Now adjust this sequence's duration and starting time.

  ChunkHeaderPntr = (LE_CHUNK_HeaderPointer) (BasePntr + Offset);
  SeqHeaderPntr = (LE_SEQNCR_SequenceChunkHeaderPointer) (ChunkHeaderPntr + 1);

  // Adjust starting time.  Doesn't make sense to adjust it
  // when doing addition rather than multiplying.

  if (MultiplyNotAdd)
    SeqHeaderPntr->parentStartTime =
    (INT32) (SeqHeaderPntr->parentStartTime * TimeFactor);

  if (SeqHeaderPntr->parentStartTime < 0)
    SeqHeaderPntr->parentStartTime = 0;

  // Adjust duration, if not infinite.

  if (SeqHeaderPntr->endTime > 0)
  {
    if (MultiplyNotAdd)
      SeqHeaderPntr->endTime = (INT32) (SeqHeaderPntr->endTime * TimeFactor);
    else
      SeqHeaderPntr->endTime = (INT32) (SeqHeaderPntr->endTime + TimeFactor);

    if (SeqHeaderPntr->endTime < OverallHighEndTime)
      SeqHeaderPntr->endTime = OverallHighEndTime;
    if (SeqHeaderPntr->endTime <= 0)
      SeqHeaderPntr->endTime = 1;
  }

  ReturnCode = TRUE;
  goto NormalExit;

ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_ChangeDuration: "
    "Something went wrong.\r\n");
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

NormalExit:
  return ReturnCode;
}
#endif // CE_ARTLIB_EnableChunkManipulation



#if CE_ARTLIB_EnableChunkDumpLoad
/*****************************************************************************
 * This class is used for containing the current dump state, rather than
 * having it as global variables (not multitasking safe) or as passed
 * arguments (too many things to pass) or as a passed record pointer
 * (that's what this class does better).
 */

typedef class LI_CHUNK_DumpClass
{
private:
  FILE               *Outfile;
  LE_CHUNK_StreamID   StreamID;
  BOOL                PrintSymbolsForDataIDs;
  BYTE                FixedBuffer [80];
  UNS32               FixedIndex;   // Next byte in FixedBuffer to be printed.
  UNS32               FixedSize;    // Number of bytes in FixedBuffer or 0xFFFF.
  UNS8                FixedIndent;  // Indent level for printing fixed stuff.
  UNS8                FixedLevel;   // Used only while printing fixed part.
  LE_CHUNK_ID         FixedID;      // ID of the chunk with fixed part being printed.
  UNS8                NumberOfDataIDsUsed; // Number printed in the last fixed part.
  char               *FileNames [5]; // Points to the following string storage.
  char                FileName1 [MAX_PATH]; // Name to use for first DataID.
  char                FileName2 [MAX_PATH]; // Name to use for second DataID.
  char                FileName3 [MAX_PATH]; // ...
  char                FileName4 [MAX_PATH];
  char                FileName5 [MAX_PATH];

  void        PrintIndentSpace (int Level);
  const char *GetChunkNameString (void);
  UNS16       GetChunkFixedPartSize (void);
  void        PrintStartOfChunk (void);
  void        PrintEndOfChunk (void);
  BOOL        PrintChunkFixedDataPart (void);
  BOOL        PrintFixedPartAsString (void);
  BOOL        PrintFixedPartAsBinary (void);
  BOOL        PrintFixedPartAsRecord (void);
  BOOL        PrintNextByte (void);
  BOOL        PrintCommentAndIndent (char *CommentString);
  BOOL        PrintSkippedBytes (void *Address);
  BOOL        PrintRemainder (void);
  BOOL        PrintINT32 (INT32 *Address);
  BOOL        PrintHex32 (UNS32 *Address);
  BOOL        PrintINT16 (INT16 *Address);
  BOOL        PrintUNS16 (UNS16 *Address);
  BOOL        PrintBYTE (BYTE *Address);
  BOOL        PrintINT8 (INT8 *Address);
  BOOL        PrintUNS8 (UNS8 *Address);
  BOOL        PrintFloat (float *Address);
  BOOL        PrintDataID (LE_DATA_DataId *Address);
  BOOL        LoadUpDataIDFileNames (void);
  BOOL        PrintSequenceCommonHeader (void);

public:
  BOOL        DumpChunks (UNS8 DataIDsUsedInParentFixedPart);

  LI_CHUNK_DumpClass (LE_CHUNK_StreamID UserStreamID, FILE *UserFile,
    BOOL UserPrintSymbolsForDataIDs);

} *LI_CHUNK_DumpPointer;



/*****************************************************************************
 * The constructor, just initialise variables and return.
 */

LI_CHUNK_DumpClass::LI_CHUNK_DumpClass (LE_CHUNK_StreamID UserStreamID,
FILE *UserFile, BOOL UserPrintSymbolsForDataIDs)
{
  Outfile = UserFile;
  StreamID = UserStreamID;
  PrintSymbolsForDataIDs = UserPrintSymbolsForDataIDs;
  FixedIndex = 0;
  FixedSize = 0;

  // Set storage to initially empty strings.
  FileName1[0] = 0;
  FileName2[0] = 0;
  FileName3[0] = 0;
  FileName4[0] = 0;
  FileName5[0] = 0;

  // Set up the array of pointers to strings.
  FileNames[0] = FileName1;
  FileNames[1] = FileName2;
  FileNames[2] = FileName3;
  FileNames[3] = FileName4;
  FileNames[4] = FileName5;
}



/*****************************************************************************
 * Print X units of white space indentation for indenting levels of
 * the dumped chunks listing.
 */

void LI_CHUNK_DumpClass::PrintIndentSpace (int Level)
{
  const int IndentStep = 2;
  char      IndentString [256];

  Level -= 1; // The first printing level is #1, so don't indent for it.

  if (Level <= 0)
    return;

  if (Level * IndentStep > sizeof (IndentString))
    Level = sizeof (IndentString) / IndentStep;

  memset (IndentString, ' ', Level * IndentStep);
  fwrite (IndentString, IndentStep, Level, Outfile);
}



/*****************************************************************************
 * Returns the name of the current descended into chunk.  If it is a
 * private chunk, it looks at the parent chunk type to figure it out.
 */

const char * LI_CHUNK_DumpClass::GetChunkNameString (void)
{
  LE_CHUNK_ID ChunkID;
  LE_CHUNK_ID ParentChunkID;
  UNS8        Level;

  Level = LE_CHUNK_GetCurrentLevel (StreamID);
  ChunkID = LE_CHUNK_GetIDForLevel (StreamID, Level);

  if (ChunkID <= LE_CHUNK_ID_NULL_IN_CONTEXT)
    return LE_CHUNK_NamesOfPublicChunkTypes[ChunkID];

  if (Level <= 1)
    return "PrivateIDWithNoParent";

  ParentChunkID = LE_CHUNK_GetIDForLevel (StreamID, Level - 1);
  if (ParentChunkID >= LE_CHUNK_ID_NULL_IN_CONTEXT)
    return "PrivateIDWithPrivateParent";

  if (ParentChunkID >= LE_CHUNK_ID_SEQ_GROUPING &&
  ParentChunkID < LE_CHUNK_ID_SEQ_MAX)
  {
    // This is a private sequence chunk.

    switch (ChunkID)
    {
      case LE_SEQNCR_CHUNK_ID_DIMENSIONALITY: return "LE_SEQNCR_CHUNK_ID_DIMENSIONALITY";
      case LE_SEQNCR_CHUNK_ID_2D_XFORM: return "LE_SEQNCR_CHUNK_ID_2D_XFORM";
      case LE_SEQNCR_CHUNK_ID_2D_OFFSET: return "LE_SEQNCR_CHUNK_ID_2D_OFFSET";
      case LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET: return "LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET";
      case LE_SEQNCR_CHUNK_ID_3D_XFORM: return "LE_SEQNCR_CHUNK_ID_3D_XFORM";
      case LE_SEQNCR_CHUNK_ID_3D_OFFSET: return "LE_SEQNCR_CHUNK_ID_3D_OFFSET";
      case LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET: return "LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET";
      case LE_SEQNCR_CHUNK_ID_2D_BOUNDING_BOX: return "LE_SEQNCR_CHUNK_ID_2D_BOUNDING_BOX";
      case LE_SEQNCR_CHUNK_ID_3D_BOUNDING_BOX: return "LE_SEQNCR_CHUNK_ID_3D_BOUNDING_BOX";
      case LE_SEQNCR_CHUNK_ID_3D_BOUNDING_SPHERE: return "LE_SEQNCR_CHUNK_ID_3D_BOUNDING_SPHERE";
      case LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE: return "LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE";
      case LE_SEQNCR_CHUNK_ID_HIT_BOX_LABEL: return "LE_SEQNCR_CHUNK_ID_HIT_BOX_LABEL";
      case LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH: return "LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH";
      case LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME: return "LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME";
      case LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING: return "LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING";
      case LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW: return "LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW";
    }

    return "LE_SEQNCR_CHUNK_ID_Unknown";
  }

  return "PrivateID";
}



/*****************************************************************************
 * Returns the size of the fixed part of the current descended into chunk.
 * If it is a private chunk, it looks at the parent chunk type to figure it
 * out.  Returns 0xFFFF if the whole chunk is raw data (no subchunks).
 */

UNS16 LI_CHUNK_DumpClass::GetChunkFixedPartSize (void)
{
  LE_CHUNK_ID ChunkID;
  LE_CHUNK_ID ParentChunkID;
  UNS8        Level;
  UNS16       SuggestedSize;

  Level = LE_CHUNK_GetCurrentLevel (StreamID);
  ChunkID = LE_CHUNK_GetIDForLevel (StreamID, Level);

  if (ChunkID <= LE_CHUNK_ID_NULL_IN_CONTEXT)
    SuggestedSize = LE_CHUNK_ChunkFixedDataSizes[ChunkID];
  else if (Level <= 1)
    SuggestedSize = 0xFFFF; // PrivateIDWithNoParent
  else
  {
    ParentChunkID = LE_CHUNK_GetIDForLevel (StreamID, Level - 1);
    if (ParentChunkID >= LE_CHUNK_ID_NULL_IN_CONTEXT)
      SuggestedSize = 0xFFFF; // PrivateIDWithPrivateParent
    else
    {
      if (ParentChunkID >= LE_CHUNK_ID_SEQ_GROUPING &&
      ParentChunkID < LE_CHUNK_ID_SEQ_MAX)
      {
        // This is a private SEQUENCE chunk.

        if (ChunkID > LE_CHUNK_ID_NULL_IN_CONTEXT &&
        ChunkID < LE_SEQNCR_CHUNK_ID_MAX_PRIVATE_CHUNKS)
          SuggestedSize = LE_CHUNK_SequencerPrivateChunkFixedDataSizes
          [ChunkID - LE_CHUNK_ID_NULL_IN_CONTEXT];
        else
          SuggestedSize = 0xFFFF;
      }
      else // Some other class of privates we don't know about.
        SuggestedSize = 0xFFFF;
    }
  }

  return SuggestedSize;
}



/*****************************************************************************
 * Prints the chunk header - essentially the name of the chunk, which depends
 * sometimes on the parent chunk.  A blank line is inserted before common
 * subsequence chunks to make it more readable.
 */

void LI_CHUNK_DumpClass::PrintStartOfChunk (void)
{
  LE_CHUNK_ID ChunkID;
  UNS8        Level;
  BOOL        SequenceChunk;

  Level = LE_CHUNK_GetCurrentLevel (StreamID);
  ChunkID = LE_CHUNK_GetIDForLevel (StreamID, Level);
  SequenceChunk = (ChunkID >= LE_CHUNK_ID_SEQ_GROUPING &&
    ChunkID < LE_CHUNK_ID_SEQ_MAX);

  // A blank line between certain common subchunks for readability.

  if (SequenceChunk)
    fprintf (Outfile, "\n");

  PrintIndentSpace (Level);

  if (SequenceChunk)
    fprintf (Outfile, "STARTCHUNK %02X ; %s, offset %u, size %u.\n",
    (int) ChunkID, GetChunkNameString (),
    (unsigned int) LE_CHUNK_GetDataStartOffsetForLevel (StreamID, Level) - sizeof (LE_CHUNK_HeaderRecord),
    (unsigned int) LE_CHUNK_GetDataSizeForLevel (StreamID, Level) + sizeof (LE_CHUNK_HeaderRecord));
  else
    fprintf (Outfile, "STARTCHUNK %02X ; %s.\n",
    (int) ChunkID, GetChunkNameString ());
}



/*****************************************************************************
 * Prints the end of chunk marker.
 */

void LI_CHUNK_DumpClass::PrintEndOfChunk (void)
{
  UNS8  Level;

  Level = LE_CHUNK_GetCurrentLevel (StreamID);
  PrintIndentSpace (Level);

/*
  fprintf (Outfile, "ENDCHUNK ; %s, offset %u.\n",
    GetChunkNameString (),
    (unsigned int) LE_CHUNK_GetDataStartOffsetForLevel (StreamID, Level));
*/
  fprintf (Outfile, "ENDCHUNK\n");
}



/*****************************************************************************
 * Dump out the chunk's data as a string.
 */

BOOL LI_CHUNK_DumpClass::PrintFixedPartAsString (void)
{
  UNS32       AmountActuallyRead;
  UNS32       AmountToRead;
  UNS32       DataSize;
  UNS32       InitialPosition;
  UNS32       OriginalSize;
  char        TempString[256];

  DataSize = LE_CHUNK_GetDataSizeForLevel (StreamID, FixedLevel);
  OriginalSize = DataSize;
  InitialPosition = LE_CHUNK_GetCurrentFilePosition (StreamID);

  if (DataSize >= 1)
    DataSize -= 1; // Don't print the NUL at the end of string.

  PrintIndentSpace (FixedIndent);
  fprintf (Outfile, "{");

  while (DataSize > 0)
  {
    AmountToRead = DataSize;
    if (AmountToRead > sizeof (TempString))
      AmountToRead = sizeof (TempString);
    AmountActuallyRead = LE_CHUNK_ReadChunkData (StreamID,
      TempString, AmountToRead);

    if (AmountActuallyRead > 0)
    {
      if (fwrite (TempString, AmountActuallyRead, 1, Outfile) != 1)
        return FALSE; // Error while writing.
    }

    if (AmountActuallyRead != AmountToRead)
    {
      if (fwrite (TempString, AmountActuallyRead, 1, Outfile) != 1) return FALSE;

      fprintf (Outfile, "} ; ... Stuff missing here.\n; ERROR!  LI_CHUNK_DumpClass::PrintFixedPartAsString: "
        "Unable to read %d bytes, only read %d, for data block starting at offset %u.  "
        "Perhaps this is an old smaller chunk format?\n",
        (int) AmountToRead, (int) AmountActuallyRead, (unsigned int) InitialPosition);

      return FALSE; // Error while reading.
    }

    DataSize -= AmountToRead;
  }

  fprintf (Outfile, "}\n");
  return TRUE;
}



/*****************************************************************************
 * Dump out the chunk's data as a variable length binary array.
 */

BOOL LI_CHUNK_DumpClass::PrintFixedPartAsBinary (void)
{
  UNS32       AmountActuallyRead;
  UNS32       AmountToRead;
  UNS32       DataSize;
  int         i;
  UNS32       InitialPosition;
  UNS32       OriginalSize;
  BYTE        TempBytes [16];

  if (FixedSize != 0xFFFF)
    DataSize = FixedSize; // Fixed size is known.
  else // No fixed part, use whole chunk data.
    DataSize = LE_CHUNK_GetDataSizeForLevel (StreamID, FixedLevel);

  OriginalSize = DataSize;
  InitialPosition = LE_CHUNK_GetCurrentFilePosition (StreamID);

  while (DataSize > 0)
  {
    AmountToRead = DataSize;
    if (AmountToRead > sizeof (TempBytes))
      AmountToRead = sizeof (TempBytes);
    AmountActuallyRead = LE_CHUNK_ReadChunkData (StreamID,
      TempBytes, AmountToRead);

    PrintIndentSpace (FixedIndent);

    if (AmountActuallyRead == 16)
    {
      if (fprintf (Outfile, "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
      (int) TempBytes[ 0],
      (int) TempBytes[ 1],
      (int) TempBytes[ 2],
      (int) TempBytes[ 3],
      (int) TempBytes[ 4],
      (int) TempBytes[ 5],
      (int) TempBytes[ 6],
      (int) TempBytes[ 7],
      (int) TempBytes[ 8],
      (int) TempBytes[ 9],
      (int) TempBytes[10],
      (int) TempBytes[11],
      (int) TempBytes[12],
      (int) TempBytes[13],
      (int) TempBytes[14],
      (int) TempBytes[15]) <= 0)
        return FALSE; // Error while writing.
    }
    else
    {
      for (i = 0; i < (int) AmountActuallyRead; i++)
      {
        if (fprintf (Outfile, "%02X ", (int) TempBytes [i]) <= 0)
          return FALSE; // Error while writing.
      }
      fprintf (Outfile, "\n");
    }

    if (AmountActuallyRead != AmountToRead)
    {
      fprintf (Outfile, "; ERROR!  LI_CHUNK_DumpClass::PrintFixedPartAsBinary: "
        "Unable to read fixed size part of %d bytes, only read %d, for data block starting at offset %u.  "
        "Perhaps this is an old smaller chunk format?\n",
        (int) OriginalSize, (int) AmountActuallyRead, (unsigned int) InitialPosition);

      return FALSE; // Error while reading.
    }

    DataSize -= AmountToRead;
  }

  return TRUE;
}



/*****************************************************************************
 * Prints out the next byte value from the FixedBuffer.
 */

BOOL LI_CHUNK_DumpClass::PrintNextByte (void)
{
  if (FixedIndex >= FixedSize)
    return FALSE; // Ran out of data.

  if (fprintf (Outfile, "%02X ", (int) FixedBuffer [FixedIndex++]) <= 0)
    return FALSE;

  return TRUE;
}



/*****************************************************************************
 * Prints out a comment and indents for the next line of text.
 */

BOOL LI_CHUNK_DumpClass::PrintCommentAndIndent (char *CommentString)
{
  if (fprintf (Outfile, "; %s.\n", CommentString) <= 0)
    return FALSE;

  PrintIndentSpace (FixedIndent);
  return TRUE;
}



/*****************************************************************************
 * Prints the bytes up to a given address in the FixedBuffer.
 */

BOOL LI_CHUNK_DumpClass::PrintSkippedBytes (void *Address)
{
  BYTE *BufferPntr;
  BYTE *UserPntr;

  UserPntr = (BYTE *) Address;
  BufferPntr = FixedBuffer + FixedIndex;

  if (UserPntr < BufferPntr)
    return FALSE; // The user wants to go backwards.

  while (BufferPntr < UserPntr)
  {
    if (FixedIndex >= FixedSize)
      return FALSE; // Went past end of FixedBuffer.

    fprintf (Outfile, "%02X ", (int) *BufferPntr);
    BufferPntr++;
    FixedIndex++;
  }

  return TRUE;
}



/*****************************************************************************
 * Prints the remaining bytes in the FixedBuffer.
 */

BOOL LI_CHUNK_DumpClass::PrintRemainder ()
{
  BOOL  Remaindered = FALSE;

  while (FixedIndex < FixedSize)
  {
    if (!Remaindered)
    {
      PrintIndentSpace (FixedIndent);
      Remaindered = TRUE;
    }

    if (fprintf (Outfile, "%02X ", (int) FixedBuffer[FixedIndex]) <= 0)
      return FALSE;

    FixedIndex++;
  }

  if (Remaindered)
    fprintf (Outfile, "; Remainder of data.  Did you update the print code when you enlarged your chunk?\n");

  return TRUE;
}



/*****************************************************************************
 * Prints the given address in FixedBuffer as a signed 32 bit integer,
 * in decimal format.
 */

BOOL LI_CHUNK_DumpClass::PrintINT32 (INT32 *Address)
{
  if (!PrintSkippedBytes (Address))
    return FALSE;

  if (FixedIndex + sizeof (INT32) > FixedSize)
    return FALSE; // Doesn't fit in the buffer.

  fprintf (Outfile, "L:%d ",
    (unsigned int) * (INT32 *) (FixedBuffer + FixedIndex));

  FixedIndex += sizeof (INT32); // Add number of bytes we think we printed.
  return TRUE;
}



/*****************************************************************************
 * Prints the given address in FixedBuffer as an unsigned 32 bit number
 * in hexadecimal format.
 */

BOOL LI_CHUNK_DumpClass::PrintHex32 (UNS32 *Address)
{
  if (!PrintSkippedBytes (Address))
    return FALSE;

  if (FixedIndex + sizeof (UNS32) > FixedSize)
    return FALSE; // Doesn't fit in the buffer.

  fprintf (Outfile, "L:0x%X ",
    (unsigned int) * (UNS32 *) (FixedBuffer + FixedIndex));

  FixedIndex += sizeof (UNS32); // Add number of bytes we think we printed.
  return TRUE;
}



/*****************************************************************************
 * Prints the given address in FixedBuffer as a INT16.
 */

BOOL LI_CHUNK_DumpClass::PrintINT16 (INT16 *Address)
{
  if (!PrintSkippedBytes (Address))
    return FALSE;

  if (FixedIndex + sizeof (INT16) > FixedSize)
    return FALSE; // Doesn't fit in the buffer.

  fprintf (Outfile, "W:%d ",
    (int) * (INT16 *) (FixedBuffer + FixedIndex));

  FixedIndex += sizeof (INT16); // Add number of bytes we think we printed.
  return TRUE;
}



/*****************************************************************************
 * Prints the given address in FixedBuffer as a UNS16.
 */

BOOL LI_CHUNK_DumpClass::PrintUNS16 (UNS16 *Address)
{
  if (!PrintSkippedBytes (Address))
    return FALSE;

  if (FixedIndex + sizeof (UNS16) > FixedSize)
    return FALSE; // Doesn't fit in the buffer.

  fprintf (Outfile, "W:%u ",
    (unsigned int) * (UNS16 *) (FixedBuffer + FixedIndex));

  FixedIndex += sizeof (UNS16); // Add number of bytes we think we printed.
  return TRUE;
}



/*****************************************************************************
 * Prints the given address in FixedBuffer as a BYTE.
 */

BOOL LI_CHUNK_DumpClass::PrintBYTE (BYTE *Address)
{
  if (!PrintSkippedBytes (Address))
    return FALSE;

  if (FixedIndex + sizeof (BYTE) > FixedSize)
    return FALSE; // Doesn't fit in the buffer.

  fprintf (Outfile, "%02X ",
    (int) FixedBuffer [FixedIndex]);

  FixedIndex += 1; // Add number of bytes we think we printed.
  return TRUE;
}



/*****************************************************************************
 * Prints the given address in FixedBuffer as a INT8.
 */

BOOL LI_CHUNK_DumpClass::PrintINT8 (INT8 *Address)
{
  if (!PrintSkippedBytes (Address))
    return FALSE;

  if (FixedIndex + sizeof (INT8) > FixedSize)
    return FALSE; // Doesn't fit in the buffer.

  fprintf (Outfile, "N:%d ",
    (int) (INT8) FixedBuffer [FixedIndex]);

  FixedIndex += 1; // Add number of bytes we think we printed.
  return TRUE;
}



/*****************************************************************************
 * Prints the given address in FixedBuffer as a UNS8.
 */

BOOL LI_CHUNK_DumpClass::PrintUNS8 (UNS8 *Address)
{
  if (!PrintSkippedBytes (Address))
    return FALSE;

  if (FixedIndex + sizeof (UNS8) > FixedSize)
    return FALSE; // Doesn't fit in the buffer.

  fprintf (Outfile, "N:%u ",
    (unsigned int) (UNS8) FixedBuffer [FixedIndex]);

  FixedIndex += 1; // Add number of bytes we think we printed.
  return TRUE;
}



/*****************************************************************************
 * Prints the given address in FixedBuffer as a float, dumped as hex bytes
 * so that you get exactly the same value, otherwise conversion to decimal
 * and back will change it slightly (use the R:123.456 chunk format for that).
 */

BOOL LI_CHUNK_DumpClass::PrintFloat (float *Address)
{
  BYTE  *BytePntr;

  if (!PrintSkippedBytes (Address))
    return FALSE;

  if (FixedIndex + sizeof (float) > FixedSize)
    return FALSE; // Doesn't fit in the buffer.

  BytePntr = FixedBuffer + FixedIndex;

  fprintf (Outfile, "%02X %02X %02X %02X ",
    (int) BytePntr[0],
    (int) BytePntr[1],
    (int) BytePntr[2],
    (int) BytePntr[3]);

  // Add on the number of bytes we printed, should be equal to sizeof(float),
  // and if it isn't then the PrintSkippedBytes function will fix it up!

  FixedIndex += 4; // Add number of bytes we think we printed.
  return TRUE;
}



/*****************************************************************************
 * Prints the given address as a DataID.  If PrintSymbolsForDataIDs is on
 * then we print the symbolic name.  If it is off, we just print the number.
 * The names come from the file name strings, in the order encountered in
 * the structure we are printing.
 */

BOOL LI_CHUNK_DumpClass::PrintDataID (LE_DATA_DataId *Address)
{
  if (!PrintSkippedBytes (Address))
    return FALSE;

  if (FixedIndex + sizeof (LE_DATA_DataId) > FixedSize)
    return FALSE; // Doesn't fit in the buffer.

  if (NumberOfDataIDsUsed >= 5 || !PrintSymbolsForDataIDs ||
  FileNames[NumberOfDataIDsUsed][0] == 0)
  {
    if (fprintf (Outfile, "L:0x%08X ",
    (unsigned int) * (LE_DATA_DataId *) (FixedBuffer + FixedIndex)) <= 0)
      return FALSE;
  }
  else // Print the symbolic name instead of the number.
  {
    if (fprintf (Outfile, "L:%s ", FileNames[NumberOfDataIDsUsed]) <= 0)
      return FALSE;
  }

  NumberOfDataIDsUsed++;
  FixedIndex += sizeof (LE_DATA_DataId);
  return TRUE;
}



/*****************************************************************************
 * Skims through the subchunks of the current chunk and collects all the
 * file names mentioned there, assigning them to the corresponding slots
 * in our local storage.  Later they get regurgitated as symbolic DataIDs.
 */

BOOL LI_CHUNK_DumpClass::LoadUpDataIDFileNames (void)
{
  UNS32       AmountActuallyRead;
  UNS32       AmountToRead;
  LE_CHUNK_ID ChunkID;
  int         NameIndex;
  char       *NameStartPntr;
  UNS32       ReturnedChunkDataSize;
  char       *PeriodPntr;
  char       *SymbolicName;
  char        TempString[MAX_PATH];

  if (!PrintSymbolsForDataIDs)
    return TRUE; // Not using symbolic names in this dump.

  for (NameIndex = 0; NameIndex < 5; NameIndex++)
    FileNames [NameIndex] [0] = 0; // Make it an empty string.

  while (TRUE)
  {
    // See if there is a next chunk, descend into it if present.

    if ((ChunkID = LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_NULL_STANDARD,
    &ReturnedChunkDataSize)) == LE_CHUNK_ID_NULL_STANDARD)
      break; // Reached end of data (parent chunk or file), or IO error.

    if (ChunkID >= LE_CHUNK_ID_FILE_NAME_1 && ChunkID <= LE_CHUNK_ID_FILE_NAME_5)
    {
      NameIndex = ChunkID - LE_CHUNK_ID_FILE_NAME_1;

      // Found a file name, read it in.

      AmountToRead = ReturnedChunkDataSize;
      if (AmountToRead > MAX_PATH - 1)
        AmountToRead = MAX_PATH - 1 /* Space for extra NUL */;
      AmountActuallyRead = LE_CHUNK_ReadChunkData (StreamID,
        TempString, AmountToRead);
      if (AmountActuallyRead != AmountToRead)
        return FALSE; // Error while reading.

      // Convert file name to a symbolic name.  Put the extension at
      // the front in uppercase and the name behind, so TCB\MyFile.bmp
      // becomes BMP_myfile.

      SymbolicName = FileNames [NameIndex];
      SymbolicName[0] = 0;
      _strlwr (TempString);

      PeriodPntr = strrchr (TempString, '.');
      if (PeriodPntr != NULL)
      {
        strcat (SymbolicName, PeriodPntr + 1); // Extension becomes type.
        _strupr (SymbolicName);
        *PeriodPntr = 0; // Remove file extension from TempString.
        strcat (SymbolicName, "_");
      }

      // Append the name part of the file name, but not the directory paths.

      NameStartPntr = strrchr (TempString, '\\');
      if (NameStartPntr == NULL)
        NameStartPntr = strrchr (TempString, ':');
      if (NameStartPntr == NULL)
        NameStartPntr = TempString;
      else
        NameStartPntr++; // Skip over the \ or : character.
      strcat (SymbolicName, NameStartPntr);
    }

    // Now ascend out of the chunk.
    if (!LE_CHUNK_Ascend (StreamID))
      return FALSE;
  }

  return TRUE;
}



/*****************************************************************************
 * Prints the common part of all public sequence chunks.
 */

BOOL LI_CHUNK_DumpClass::PrintSequenceCommonHeader (void)
{
  LE_SEQNCR_SequenceChunkHeaderPointer SeqPntr;

  SeqPntr = (LE_SEQNCR_SequenceChunkHeaderPointer) (FixedBuffer + FixedIndex);

  PrintIndentSpace (FixedIndent);
  if (!PrintNextByte ()) return FALSE;
  if (!PrintNextByte ()) return FALSE;
  if (!PrintNextByte ()) return FALSE;
  if (fprintf (Outfile, "; parentStartTime %d (%.2fs).\n",
  (int) SeqPntr->parentStartTime,
  SeqPntr->parentStartTime / (double) CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ) <= 0)
    return FALSE;

  PrintIndentSpace (FixedIndent);
  if (!PrintNextByte ()) return FALSE;
  if (fprintf (Outfile, "; priority %d.\n", SeqPntr->priority) <= 0) return FALSE;

  PrintIndentSpace (FixedIndent);
  if (!PrintNextByte ()) return FALSE;
  if (!PrintNextByte ()) return FALSE;
  if (!PrintNextByte ()) return FALSE;
  if (fprintf (Outfile, (SeqPntr->endTime == 0)
  ? "; endTime %d (infinite).\n"
  : "; endTime %d (%.2fs, parent end %d (%.2fs)).\n",
  (int) SeqPntr->endTime,
  SeqPntr->endTime / (double) CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ,
  (int) (SeqPntr->parentStartTime + SeqPntr->endTime),
  (SeqPntr->parentStartTime + SeqPntr->endTime) / (double) CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ
  ) <= 0)
    return FALSE;

  PrintIndentSpace (FixedIndent);
  if (!PrintNextByte ()) return FALSE;
  if (fprintf (Outfile, "; timeMultiple %d, dropFrames %d, lastUse %d.\n",
    SeqPntr->timeMultiple, SeqPntr->dropFrames, SeqPntr->lastUse) <= 0) return FALSE;

  PrintIndentSpace (FixedIndent);
  if (!PrintNextByte ()) return FALSE;
  if (!PrintNextByte ()) return FALSE;
  if (!PrintNextByte ()) return FALSE;
  if (!PrintNextByte ()) return FALSE;
  if (fprintf (Outfile, "; endingAction %d (%s), scrollingWorld %d, absoluteDataIDs %d, reserved %d.\n",
  SeqPntr->endingAction,
  (SeqPntr->endingAction == LE_SEQNCR_EndingActionStop) ? "Stop"
  : ((SeqPntr->endingAction == LE_SEQNCR_EndingActionStayAtEnd) ? "Stay"
  : ((SeqPntr->endingAction == LE_SEQNCR_EndingActionLoopToBeginning) ? "Loop" : ("?"))),
  SeqPntr->scrollingWorld, SeqPntr->absoluteDataIDs, SeqPntr->reserved)
  <= 0)
    return FALSE;

  return TRUE;
}



/*****************************************************************************
 * Dump out the chunk's data as a known record type.
 */

BOOL LI_CHUNK_DumpClass::PrintFixedPartAsRecord (void)
{
  UNS32       AmountActuallyRead;
  UNS32       InitialPosition;
  LE_CHUNK_ID ParentChunkID;

  if (FixedSize == 0)
    return TRUE; // Just in case we have really small chunks.

  if (FixedSize > sizeof (FixedBuffer))
    return PrintFixedPartAsBinary (); // Too big for us to handle.

  InitialPosition = LE_CHUNK_GetCurrentFilePosition (StreamID);

  AmountActuallyRead = LE_CHUNK_ReadChunkData (StreamID, FixedBuffer, FixedSize);
  if (AmountActuallyRead != FixedSize)
  {
    fprintf (Outfile, "; ERROR!  LI_CHUNK_DumpClass::PrintFixedPartAsRecord: "
      "Unable to read fixed size part of %d bytes, only read %d, at offset %u.  "
      "Perhaps this is an old smaller chunk format?\n",
      (int) FixedSize, (int) AmountActuallyRead, (unsigned int) InitialPosition);
    return FALSE; // Error while reading.
  }

  // Every time we print a DataID, we use one more file name from the
  // array of file names.  Here's where we fill the array.  Note that
  // it assumes that the stream is positioned at the start of the
  // subchunks so that it can read them.

  if (!LoadUpDataIDFileNames ()) return FALSE;
  NumberOfDataIDsUsed = 0; // Reset back to zero for playback.

  // Temporarily move the position backwards in case we have to handle
  // the chunk as pure binary, so that the binary routines then reread
  // it from the start.  We'll advance the position after a successful
  // normal processing.

  LE_CHUNK_SetCurrentFilePosition (StreamID, InitialPosition);

  // This index increments through the buffer, making sure that we don't
  // miss any bytes in the record (all printing routines print any skipped
  // bytes before printing the requested bytes).

  FixedIndex = 0;

  // Now figure out what kind of chunk we are printing out and print it.

  if (FixedID <= LE_CHUNK_ID_NULL_IN_CONTEXT)
  {
    // A standard, public, type of chunk.

    switch (FixedID)
    {
    case LE_CHUNK_ID_SEQ_GROUPING:
      if (!PrintSequenceCommonHeader ()) return FALSE;
      goto NormalExit;

    case LE_CHUNK_ID_SEQ_INDIRECT:
      if (!PrintSequenceCommonHeader ()) return FALSE;
      PrintIndentSpace (FixedIndent);
      if (!PrintDataID (&((LE_SEQNCR_SequenceIndirectChunkPointer)
        FixedBuffer)->subsequenceChunkListDataID)) return FALSE;
      fprintf (Outfile, "; subsequenceChunkListDataID.\n");
      goto NormalExit;

    case LE_CHUNK_ID_SEQ_2DBITMAP:
      if (!PrintSequenceCommonHeader ()) return FALSE;
      PrintIndentSpace (FixedIndent);
      if (!PrintDataID (&((LE_SEQNCR_Sequence2DBitmapChunkPointer)
        FixedBuffer)->bitmapDataID)) return FALSE;
      fprintf (Outfile, "; bitmapDataID.\n");
      goto NormalExit;

    case LE_CHUNK_ID_SEQ_3DMODEL:
      if (!PrintSequenceCommonHeader ()) return FALSE;
      PrintIndentSpace (FixedIndent);
      if (!PrintDataID (&((LE_SEQNCR_Sequence3DModelChunkPointer)
        FixedBuffer)->modelDataID)) return FALSE;
      PrintCommentAndIndent ("modelDataID");
      if (!PrintDataID (&((LE_SEQNCR_Sequence3DModelChunkPointer)
        FixedBuffer)->textureMapDataID)) return FALSE;
      PrintCommentAndIndent ("textureMapDataID");
      if (!PrintDataID (&((LE_SEQNCR_Sequence3DModelChunkPointer)
        FixedBuffer)->jointPositionsDataID)) return FALSE;
      fprintf (Outfile, "; jointPositionsDataID.\n");
      goto NormalExit;

    case LE_CHUNK_ID_SEQ_SOUND:
      if (!PrintSequenceCommonHeader ()) return FALSE;
      PrintIndentSpace (FixedIndent);
      if (!PrintDataID (&((LE_SEQNCR_SequenceSoundChunkPointer)
        FixedBuffer)->soundDataID)) return FALSE;
      fprintf (Outfile, "; soundDataID.\n");
      goto NormalExit;

    case LE_CHUNK_ID_SEQ_VIDEO:
      if (!PrintSequenceCommonHeader ()) return FALSE;
      PrintIndentSpace (FixedIndent);
      if (!PrintBYTE (&((LE_SEQNCR_SequenceVideoChunkPointer)
        FixedBuffer)->drawSolid)) return FALSE;
      PrintCommentAndIndent ("drawSolid");
      if (!PrintBYTE (&((LE_SEQNCR_SequenceVideoChunkPointer)
        FixedBuffer)->flipVertically)) return FALSE;
      PrintCommentAndIndent ("flipVertically");
      if (!PrintUNS8 (&((LE_SEQNCR_SequenceVideoChunkPointer)
        FixedBuffer)->alphaLevel)) return FALSE;
      PrintCommentAndIndent ("alphaLevel");
      if (!PrintBYTE (&((LE_SEQNCR_SequenceVideoChunkPointer)
        FixedBuffer)->enableVideo)) return FALSE;
      PrintCommentAndIndent ("enableVideo");
      if (!PrintBYTE (&((LE_SEQNCR_SequenceVideoChunkPointer)
        FixedBuffer)->enableAudio)) return FALSE;
      PrintCommentAndIndent ("enableAudio");
      if (!PrintBYTE (&((LE_SEQNCR_SequenceVideoChunkPointer)
        FixedBuffer)->drawDirectlyToScreen)) return FALSE;
      PrintCommentAndIndent ("drawDirectlyToScreen");
      if (!PrintBYTE (&((LE_SEQNCR_SequenceVideoChunkPointer)
        FixedBuffer)->doubleAlternateLines)) return FALSE;
      PrintCommentAndIndent ("doubleAlternateLines");
      if (!PrintINT8 (&((LE_SEQNCR_SequenceVideoChunkPointer)
        FixedBuffer)->saturation)) return FALSE;
      PrintCommentAndIndent ("saturation");
      if (!PrintINT8 (&((LE_SEQNCR_SequenceVideoChunkPointer)
        FixedBuffer)->brightness)) return FALSE;
      PrintCommentAndIndent ("brightness");
      if (!PrintINT8 (&((LE_SEQNCR_SequenceVideoChunkPointer)
        FixedBuffer)->contrast)) return FALSE;
      fprintf (Outfile, "; contrast.\n");
      goto NormalExit;

    case LE_CHUNK_ID_SEQ_CAMERA:
      if (!PrintSequenceCommonHeader ()) return FALSE;
      PrintIndentSpace (FixedIndent);
      if (!PrintFloat (&((LE_SEQNCR_SequenceCameraChunkPointer)
        FixedBuffer)->nearClipPlaneDistance)) return FALSE;
      fprintf (Outfile, "; nearClipPlaneDistance %f.\n",
        (double) ((LE_SEQNCR_SequenceCameraChunkPointer)
        FixedBuffer)->nearClipPlaneDistance);
      PrintIndentSpace (FixedIndent);
      if (!PrintFloat (&((LE_SEQNCR_SequenceCameraChunkPointer)
        FixedBuffer)->farClipPlaneDistance)) return FALSE;
      fprintf (Outfile, "; farClipPlaneDistance %f.\n",
        (double) ((LE_SEQNCR_SequenceCameraChunkPointer)
        FixedBuffer)->farClipPlaneDistance);
      PrintIndentSpace (FixedIndent);
      if (!PrintBYTE (&((LE_SEQNCR_SequenceCameraChunkPointer)
        FixedBuffer)->cameraLabel)) return FALSE;
      fprintf (Outfile, "; cameraLabel %d.\n",
        (int) ((LE_SEQNCR_SequenceCameraChunkPointer)
        FixedBuffer)->cameraLabel);
      goto NormalExit;

    case LE_CHUNK_ID_SEQ_PRELOADER:
      if (!PrintSequenceCommonHeader ()) return FALSE;
      PrintIndentSpace (FixedIndent);
      if (!PrintDataID (&((LE_SEQNCR_SequencePreloaderChunkPointer)
        FixedBuffer)->preloadDataID)) return FALSE;
      fprintf (Outfile, "; preloadDataID.\n");
      goto NormalExit;

    case LE_CHUNK_ID_SEQ_3DMESH:
      if (!PrintSequenceCommonHeader ()) return FALSE;
      PrintIndentSpace (FixedIndent);
      if (!PrintDataID (&((LE_SEQNCR_Sequence3DMeshChunkPointer)
        FixedBuffer)->modelDataID)) return FALSE;
      fprintf (Outfile, "; modelDataID.\n");
      goto NormalExit;

    case LE_CHUNK_ID_SEQ_TWEEKER:
      if (!PrintSequenceCommonHeader ()) return FALSE;
      PrintIndentSpace (FixedIndent);
      if (!PrintBYTE (&((LE_SEQNCR_SequenceTweekerChunkPointer)
        FixedBuffer)->interpolationType)) return FALSE;
      fprintf (Outfile, "; interpolationType %d (%s).\n",
        (int) ((LE_SEQNCR_SequenceTweekerChunkPointer)
        FixedBuffer)->interpolationType,
        (((LE_SEQNCR_SequenceTweekerChunkPointer) FixedBuffer)->
        interpolationType == LE_SEQNCR_INTERPOLATION_CONSTANT) ?
        "Constant" : ((((LE_SEQNCR_SequenceTweekerChunkPointer) FixedBuffer)->
        interpolationType == LE_SEQNCR_INTERPOLATION_LINEAR_PAIR) ?
        "Linear Pair" : ((((LE_SEQNCR_SequenceTweekerChunkPointer) FixedBuffer)->
        interpolationType == LE_SEQNCR_INTERPOLATION_IDENTITY) ?
        "Identity" : "?")));
      goto NormalExit;

    case LE_CHUNK_ID_DOPE_DATA:
      PrintIndentSpace (FixedIndent);
      if (!PrintHex32 (&((LE_CHUNK_DopeDataPointer)
        FixedBuffer)->serialNumber)) return FALSE;
      fprintf (Outfile, "; serialNumber %d.\n",
        (int) ((LE_CHUNK_DopeDataPointer) FixedBuffer)->serialNumber);
      PrintIndentSpace (FixedIndent);
      if (!PrintHex32 (&((LE_CHUNK_DopeDataPointer)
        FixedBuffer)->flags)) return FALSE;
      fprintf (Outfile, "; flags %d.\n",
        (int) ((LE_CHUNK_DopeDataPointer) FixedBuffer)->flags);
      goto NormalExit;
    }

    // Unknown type of public chunk.  Dump it as binary.
    return PrintFixedPartAsBinary ();
  }

  if (FixedLevel <= 1)
    return PrintFixedPartAsBinary (); // PrivateIDWithNoParent, can't tell what it is.

  // A private chunk with a parent we can use to decipher what it means.

  ParentChunkID = LE_CHUNK_GetIDForLevel (StreamID, FixedLevel - 1);
  if (ParentChunkID >= LE_CHUNK_ID_NULL_IN_CONTEXT)
    return PrintFixedPartAsBinary (); // PrivateIDWithPrivateParent, undecodable.

  if (ParentChunkID >= LE_CHUNK_ID_SEQ_GROUPING &&
  ParentChunkID < LE_CHUNK_ID_SEQ_MAX)
  {
    // This is a private sequence chunk.

    switch (FixedID)
    {
      case LE_SEQNCR_CHUNK_ID_DIMENSIONALITY:
      {
        LE_SEQNCR_DimensionalityChunkPointer ChunkPntr =
          (LE_SEQNCR_DimensionalityChunkPointer) FixedBuffer;

        PrintIndentSpace (FixedIndent);
        if (!PrintBYTE (&ChunkPntr->dimensionality)) return FALSE;
        fprintf (Outfile, "; dimensionality %d.\n",
          (int) ChunkPntr->dimensionality);
        goto NormalExit;
      }

      case LE_SEQNCR_CHUNK_ID_2D_XFORM:
      {
        LE_SEQNCR_2DXformChunkPointer ChunkPntr =
          (LE_SEQNCR_2DXformChunkPointer) FixedBuffer;

        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->matrix._11)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._12)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._13)) return FALSE;
        fprintf (Outfile, "; %12f %12f %12f\n",
          (double) ChunkPntr->matrix._11,
          (double) ChunkPntr->matrix._12,
          (double) ChunkPntr->matrix._13);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->matrix._21)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._22)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._23)) return FALSE;
        fprintf (Outfile, "; %12f %12f %12f\n",
          (double) ChunkPntr->matrix._21,
          (double) ChunkPntr->matrix._22,
          (double) ChunkPntr->matrix._23);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->matrix._31)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._32)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._33)) return FALSE;
        fprintf (Outfile, "; %12f %12f %12f\n",
          (double) ChunkPntr->matrix._31,
          (double) ChunkPntr->matrix._32,
          (double) ChunkPntr->matrix._33);
        goto NormalExit;
      }

      case LE_SEQNCR_CHUNK_ID_2D_OFFSET:
      {
        LE_SEQNCR_2DOffsetChunkPointer ChunkPntr =
          (LE_SEQNCR_2DOffsetChunkPointer) FixedBuffer;

        PrintIndentSpace (FixedIndent);
        if (!PrintINT32 (&ChunkPntr->offset.x)) return FALSE;
        if (!PrintINT32 (&ChunkPntr->offset.y)) return FALSE;
        fprintf (Outfile, "; offset (%d, %d).\n",
          (int) ChunkPntr->offset.x,
          (int) ChunkPntr->offset.y);
        goto NormalExit;
      }

      case LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET:
      {
        LE_SEQNCR_2DOriginScaleRotateOffsetChunkPointer ChunkPntr =
          (LE_SEQNCR_2DOriginScaleRotateOffsetChunkPointer) FixedBuffer;

        PrintIndentSpace (FixedIndent);
        if (!PrintINT32 (&ChunkPntr->offset.x)) return FALSE;
        if (!PrintINT32 (&ChunkPntr->offset.y)) return FALSE;
        fprintf (Outfile, "; offset (%d, %d).\n",
          (int) ChunkPntr->offset.x,
          (int) ChunkPntr->offset.y);
        PrintIndentSpace (FixedIndent);
        if (!PrintINT32 (&ChunkPntr->origin.x)) return FALSE;
        if (!PrintINT32 (&ChunkPntr->origin.y)) return FALSE;
        fprintf (Outfile, "; origin (%d, %d).\n",
          (int) ChunkPntr->origin.x,
          (int) ChunkPntr->origin.y);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->scaleX)) return FALSE;
        fprintf (Outfile, "; scaleX %f.\n",
          (double) ChunkPntr->scaleX);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->scaleY)) return FALSE;
        fprintf (Outfile, "; scaleY %f.\n",
          (double) ChunkPntr->scaleY);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->rotate)) return FALSE;
        fprintf (Outfile, "; rotate %f (%f degrees).\n",
          (double) ChunkPntr->rotate,
          ((double) ChunkPntr->rotate) * 360.0 / TYPE_FULL_CIRCLE_2D_ANGLE);
        goto NormalExit;
      }

      case LE_SEQNCR_CHUNK_ID_3D_XFORM:
      {
        LE_SEQNCR_3DXformChunkPointer ChunkPntr =
          (LE_SEQNCR_3DXformChunkPointer) FixedBuffer;

        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->matrix._11)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._12)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._13)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._14)) return FALSE;
        fprintf (Outfile, "; %12f %12f %12f %12f\n",
          (double) ChunkPntr->matrix._11,
          (double) ChunkPntr->matrix._12,
          (double) ChunkPntr->matrix._13,
          (double) ChunkPntr->matrix._14);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->matrix._21)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._22)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._23)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._24)) return FALSE;
        fprintf (Outfile, "; %12f %12f %12f %12f\n",
          (double) ChunkPntr->matrix._21,
          (double) ChunkPntr->matrix._22,
          (double) ChunkPntr->matrix._23,
          (double) ChunkPntr->matrix._24);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->matrix._31)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._32)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._33)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._34)) return FALSE;
        fprintf (Outfile, "; %12f %12f %12f %12f\n",
          (double) ChunkPntr->matrix._31,
          (double) ChunkPntr->matrix._32,
          (double) ChunkPntr->matrix._33,
          (double) ChunkPntr->matrix._34);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->matrix._41)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._42)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._43)) return FALSE;
        if (!PrintFloat (&ChunkPntr->matrix._44)) return FALSE;
        fprintf (Outfile, "; %12f %12f %12f %12f\n",
          (double) ChunkPntr->matrix._41,
          (double) ChunkPntr->matrix._42,
          (double) ChunkPntr->matrix._43,
          (double) ChunkPntr->matrix._44);
        goto NormalExit;
      }

      case LE_SEQNCR_CHUNK_ID_3D_OFFSET:
      {
        LE_SEQNCR_3DOffsetChunkPointer ChunkPntr =
          (LE_SEQNCR_3DOffsetChunkPointer) FixedBuffer;

        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->offset.x)) return FALSE;
        if (!PrintFloat (&ChunkPntr->offset.y)) return FALSE;
        if (!PrintFloat (&ChunkPntr->offset.z)) return FALSE;
        fprintf (Outfile, "; offset (%f, %f, %f).\n",
          (double) ChunkPntr->offset.x,
          (double) ChunkPntr->offset.y,
          (double) ChunkPntr->offset.z);
        goto NormalExit;
      }

      case LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET:
      {
        LE_SEQNCR_3DOriginScaleRotateOffsetChunkPointer ChunkPntr =
          (LE_SEQNCR_3DOriginScaleRotateOffsetChunkPointer) FixedBuffer;

        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->offset.x)) return FALSE;
        if (!PrintFloat (&ChunkPntr->offset.y)) return FALSE;
        if (!PrintFloat (&ChunkPntr->offset.z)) return FALSE;
        fprintf (Outfile, "; offset (%f, %f, %f).\n",
          (double) ChunkPntr->offset.x,
          (double) ChunkPntr->offset.y,
          (double) ChunkPntr->offset.z);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->origin.x)) return FALSE;
        if (!PrintFloat (&ChunkPntr->origin.y)) return FALSE;
        if (!PrintFloat (&ChunkPntr->origin.z)) return FALSE;
        fprintf (Outfile, "; origin (%f, %f, %f).\n",
          (double) ChunkPntr->origin.x,
          (double) ChunkPntr->origin.y,
          (double) ChunkPntr->origin.z);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->roll)) return FALSE;
        fprintf (Outfile, "; roll %f (%f degrees).\n",
          (double) ChunkPntr->roll,
          ((double) ChunkPntr->roll) * 360.0 / TYPE_FULL_CIRCLE_3D_ANGLE);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->pitch)) return FALSE;
        fprintf (Outfile, "; pitch %f (%f degrees).\n",
          (double) ChunkPntr->pitch,
          ((double) ChunkPntr->pitch) * 360.0 / TYPE_FULL_CIRCLE_3D_ANGLE);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->yaw)) return FALSE;
        fprintf (Outfile, "; yaw %f (%f degrees).\n",
          (double) ChunkPntr->yaw,
          ((double) ChunkPntr->yaw) * 360.0 / TYPE_FULL_CIRCLE_3D_ANGLE);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->scaleX)) return FALSE;
        fprintf (Outfile, "; scaleX %f.\n", (double) ChunkPntr->scaleX);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->scaleY)) return FALSE;
        fprintf (Outfile, "; scaleY %f.\n", (double) ChunkPntr->scaleY);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->scaleZ)) return FALSE;
        fprintf (Outfile, "; scaleZ %f.\n", (double) ChunkPntr->scaleZ);
        goto NormalExit;
      }

      case LE_SEQNCR_CHUNK_ID_2D_BOUNDING_BOX:
      {
        LE_SEQNCR_2DBoundingBoxChunkPointer ChunkPntr =
          (LE_SEQNCR_2DBoundingBoxChunkPointer) FixedBuffer;

        PrintIndentSpace (FixedIndent);
        if (!PrintINT32 (&ChunkPntr->boundingRect.left)) return FALSE;
        if (!PrintINT32 (&ChunkPntr->boundingRect.top)) return FALSE;
        fprintf (Outfile, "; boundingRect left:%d top:%d.\n",
          (int) ChunkPntr->boundingRect.left,
          (int) ChunkPntr->boundingRect.top);
        PrintIndentSpace (FixedIndent);
        if (!PrintINT32 (&ChunkPntr->boundingRect.right)) return FALSE;
        if (!PrintINT32 (&ChunkPntr->boundingRect.bottom)) return FALSE;
        fprintf (Outfile, "; boundingRect right:%d bottom:%d.\n",
          (int) ChunkPntr->boundingRect.right,
          (int) ChunkPntr->boundingRect.bottom);
        goto NormalExit;
      }

      case LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE:
      {
        LE_SEQNCR_3DMeshChoiceChunkPointer ChunkPntr =
          (LE_SEQNCR_3DMeshChoiceChunkPointer) FixedBuffer;

        PrintIndentSpace (FixedIndent);
        if (!PrintINT16 (&ChunkPntr->meshIndexA)) return FALSE;
        fprintf (Outfile, "; meshIndexA %d.\n",
          (int) ChunkPntr->meshIndexA);
        PrintIndentSpace (FixedIndent);
        if (!PrintINT16 (&ChunkPntr->meshIndexB)) return FALSE;
        fprintf (Outfile, "; meshIndexB %d.\n",
          (int) ChunkPntr->meshIndexB);
        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->meshProportion)) return FALSE;
        fprintf (Outfile, "; meshProportion %f.\n",
          (double) ChunkPntr->meshProportion);
        goto NormalExit;
      }

      case LE_SEQNCR_CHUNK_ID_HIT_BOX_LABEL:
      {
        LE_SEQNCR_LabelChunkPointer ChunkPntr =
          (LE_SEQNCR_LabelChunkPointer) FixedBuffer;

        PrintIndentSpace (FixedIndent);
        if (!PrintBYTE (&ChunkPntr->labelNumber)) return FALSE;
        fprintf (Outfile, "; labelNumber %d.\n",
          (int) ChunkPntr->labelNumber);
        goto NormalExit;
      }

      case LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH:
      {
        LE_SEQNCR_SoundPitchChunkPointer ChunkPntr =
          (LE_SEQNCR_SoundPitchChunkPointer) FixedBuffer;

        PrintIndentSpace (FixedIndent);
        if (!PrintUNS16 (&ChunkPntr->soundPitch)) return FALSE;
        fprintf (Outfile, "; soundPitch %d.\n",
          (int) ChunkPntr->soundPitch);
        goto NormalExit;
      }

      case LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME:
      {
        LE_SEQNCR_SoundVolumeChunkPointer ChunkPntr =
          (LE_SEQNCR_SoundVolumeChunkPointer) FixedBuffer;

        PrintIndentSpace (FixedIndent);
        if (!PrintBYTE (&ChunkPntr->soundVolume)) return FALSE;
        fprintf (Outfile, "; soundVolume %d.\n",
          (int) ChunkPntr->soundVolume);
        goto NormalExit;
      }

      case LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING:
      {
        LE_SEQNCR_SoundPanningChunkPointer ChunkPntr =
          (LE_SEQNCR_SoundPanningChunkPointer) FixedBuffer;

        PrintIndentSpace (FixedIndent);
        if (!PrintINT8 (&ChunkPntr->soundPanning)) return FALSE;
        fprintf (Outfile, "; soundPanning %d.\n",
          (int) ChunkPntr->soundPanning);
        goto NormalExit;
      }

      case LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW:
      {
        LE_SEQNCR_FieldOfViewChunkPointer ChunkPntr =
          (LE_SEQNCR_FieldOfViewChunkPointer) FixedBuffer;

        PrintIndentSpace (FixedIndent);
        if (!PrintFloat (&ChunkPntr->fieldOfView)) return FALSE;
        fprintf (Outfile, "; fieldOfView %f (%f degrees).\n",
          (double) ChunkPntr->fieldOfView,
          ((double) ChunkPntr->fieldOfView) * 360.0 / TYPE_FULL_CIRCLE_3D_ANGLE);
        goto NormalExit;
      }
    }
    return PrintFixedPartAsBinary (); // Unknown private sequence chunk.
  }

  // Some other class of privates we don't know about yet.
  return PrintFixedPartAsBinary ();


NormalExit:
  if (!PrintRemainder ()) return FALSE;
  LE_CHUNK_SetCurrentFilePosition (StreamID, InitialPosition + FixedSize);
  return TRUE;
}



/*****************************************************************************
 * Prints out the fixed part, which can be a string, binary data or
 * one of several records it understands.  Implicitly returns
 * NumberOfDataIDsUsed with the number actually used in printing
 * the fixed part, any extra should be dumped.
 */

BOOL LI_CHUNK_DumpClass::PrintChunkFixedDataPart (void)
{
  BOOL  ReturnCode;

  FixedLevel = LE_CHUNK_GetCurrentLevel (StreamID);
  FixedID = LE_CHUNK_GetIDForLevel (StreamID, FixedLevel);

  FixedSize = GetChunkFixedPartSize ();
  FixedIndent = FixedLevel + 1;
  NumberOfDataIDsUsed = 0;

  // Print out start header of fixed data part.

  PrintIndentSpace (FixedIndent);
  fprintf (Outfile, "STARTFIXEDDATA\n");

  if ((FixedID >= LE_CHUNK_ID_FILE_NAME_1 && FixedID <= LE_CHUNK_ID_FILE_NAME_5) ||
  (FixedID >= LE_CHUNK_ID_USER_NAME_1 && FixedID <= LE_CHUNK_ID_USER_NAME_5) ||
  (FixedID >= LE_CHUNK_ID_ANNOTATION && FixedID <= LE_CHUNK_ID_COPYRIGHT))
    ReturnCode = PrintFixedPartAsString ();
  else if (FixedSize == 0xFFFF)
    ReturnCode = PrintFixedPartAsBinary ();
  else
    ReturnCode = PrintFixedPartAsRecord ();

  PrintIndentSpace (FixedIndent);
  fprintf (Outfile, "ENDFIXEDDATA\n");

  return ReturnCode;
}



/*****************************************************************************
 * Recursively prints the contents of the chunks.
 */

BOOL LI_CHUNK_DumpClass::DumpChunks (UNS8 DataIDsUsedInParentFixedPart)
{
  LE_CHUNK_ID ChunkID;
  BOOL        DumpIt;
  UNS32       ReturnedChunkDataSize;

  while (TRUE)
  {
    // See if there is a next chunk, descend into it if present.

    if ((ChunkID = LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_NULL_STANDARD,
    &ReturnedChunkDataSize)) == LE_CHUNK_ID_NULL_STANDARD)
      break; // Reached end of data (parent chunk or file), or IO error.

    // Ignore chunks containing file names if the names were used by the
    // parent chunk's symbolic DataIDs.  Ones which aren't names or
    // are unused names get printed.  Of course, if we are not doing
    // symbol substitution then they all get printed.  Also ignore the
    // dope sheet private optional chunks.

    if (!PrintSymbolsForDataIDs)
      DumpIt = TRUE;
    else if (ChunkID < LE_CHUNK_ID_FILE_NAME_1)
      DumpIt = TRUE;
    else if (ChunkID <= LE_CHUNK_ID_FILE_NAME_5)
    {
      // Dump file name chunks which are not used, just in case.  Used for
      // things like external spooled audio files and movies.
      DumpIt = ((ChunkID - LE_CHUNK_ID_FILE_NAME_1) >= DataIDsUsedInParentFixedPart);
    }
    else if (ChunkID < LE_CHUNK_ID_DOPE_DATA)
      DumpIt = TRUE;
    else if (ChunkID < LE_CHUNK_ID_ANNOTATION)
      DumpIt = FALSE; // Throw away optional dope sheet chunks.
    else // All other chunks we keep, including private ones.
      DumpIt = TRUE;

    if (DumpIt)
    {
      // Print out chunk header.
      PrintStartOfChunk ();

      // Read in and print out the chunk fixed data part.
      // This also updates NumberOfDataIDsUsed.
      if (!PrintChunkFixedDataPart ())
        return FALSE; // Something went wrong.

      // Print out the nested subchunks, if any.
      if (!DumpChunks (NumberOfDataIDsUsed))
        return FALSE; // An error has occured.

      // End of this chunk.
      PrintEndOfChunk ();
    }

    // Now ascend out of the chunk.
    if (!LE_CHUNK_Ascend (StreamID))
      return FALSE;
  }

  // Finished with no errors.
  return TRUE;
}



/*****************************************************************************
 * Converts a binary chunky file into Human readable form (a text file).
 * It goes through all chunks and their subchunks, one by one, until the
 * end of the source stream is reached.  Presumably the chunk stream and
 * the file are already open (so that you can preprint stuff or start
 * in the middle of a chunky structure).  If PrintSymbolsForDataIDs is
 * TRUE then DataID values will be printed as symbols using the file
 * name subchunks to get the symbol and the file name subchunks won't
 * be printed, as well the LE_CHUNK_ID_DOPE_* series of chunks.  If FALSE
 * then DataIDs will be printed as numbers and optional chunks will be
 * left in; you get exactly what the chunk contains.
 */

BOOL LE_CHUNK_DumpChunks (LE_CHUNK_StreamID StreamID, FILE *Outfile,
BOOL PrintSymbolsForDataIDs)
{
  LI_CHUNK_DumpClass Dumper (StreamID, Outfile, PrintSymbolsForDataIDs);

  return Dumper.DumpChunks (0);
}



/*****************************************************************************
 * This class is conveniently used for holding the current chunk load state.
 */

typedef class LI_CHUNK_LoadClass
{
private:
  FILE               *Infile;
  int                 LineNumber;
  LE_CHUNK_StreamID   StreamID;
  char                TempString [MAX_PATH];
  BOOL                LoadKeywordIntoTempString (char InitialLetter);

public:
  BOOL        LoadChunks (void);

  LI_CHUNK_LoadClass (LE_CHUNK_StreamID UserStreamID, FILE *UserFile);

} *LI_CHUNK_LoadPointer;



/*****************************************************************************
 * Constructor just sets up internal state variables.
 */

LI_CHUNK_LoadClass::LI_CHUNK_LoadClass (LE_CHUNK_StreamID UserStreamID, FILE *UserFile)
{
  Infile = UserFile;
  StreamID = UserStreamID;
  LineNumber = 1;
}



/*****************************************************************************
 * Internal utility to load the rest of the keyword into TempString,
 * stops when it hits a non-letter or non-number, or if TempString
 * gets full.  Converts it to lower case too.  Returns FALSE if end
 * of file encountered, TRUE otherwise.
 */

BOOL LI_CHUNK_LoadClass::LoadKeywordIntoTempString (char InitialLetter)
{
  int i;
  int Letter;

  TempString[0] = tolower (InitialLetter);
  i = 1;
  while (i < sizeof (TempString) - 1)
  {
    Letter = fgetc (Infile);
    if (Letter == EOF)
    {
#if CE_ARTLIB_EnableDebugMode
      TempString[i] = 0;
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadKeywordIntoTempString: "
      "End of file encountered while trying to read a keyword.  Have \"%s\" so far.\r\n",
      TempString);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return FALSE;
    }

    if (!isalnum (Letter))
      break;

    TempString[i++] = tolower (Letter);
  }

  TempString[i] = 0;
  if (i < sizeof (TempString) - 1)
    ungetc (Letter, Infile); // Put back the letter we didn't like.

  return TRUE;
}



/*****************************************************************************
 * Chunk loader goes through the file.  Every time it encounters a
 * STARTCHUNK nn keyword, it creats a new nested chunk of type nn (nn is
 * a byte sized number).  ENDCHUNK closes off a nested chunk.  If it finds
 * STARTFIXEDDATA it then starts reading in binary data, hex bytes by default
 * and {String} for strings (plus NUL added at end) and L:123 for long numbers
 * and R:123.456 for floats.  Binary data stops when it finds an ENDFIXEDDATA.
 * Then it's back to searching for one of STARTCHUNK, ENDCHUNK, STARTFIXEDDATA.
 * Comments start with ";" and end at the end of the line.
 * Returns TRUE if successful, FALSE on error.
 */

BOOL LI_CHUNK_LoadClass::LoadChunks (void)
{
  char *EndPntr;
  int   Letter;
  float MyFloat;
  int   MyInt;
  BYTE  MyByte;
  int   NextLetter;

  while (!feof (Infile))
  {
    // Search for STARTCHUNK, ENDCHUNK, STARTFIXEDDATA.

    Letter = fgetc (Infile);
    if (Letter == EOF)
      break; // OK to get end of file between keywords, exit.

    if (Letter == '\n')
      LineNumber++; // Count lines for error messages.

    if (isspace (Letter))
      continue; // Skip over white space.

    if (Letter == ';') // Start of a comment.
    {
      // Skip over the comment.

      while (TRUE)
      {
        Letter = fgetc (Infile);
        if (Letter == EOF)
          break; // End of comment at end of file.
        if (Letter == '\n')
        {
          LineNumber++;
          break; // End of comment at new line.
        }
      }
      continue; // Comment finished, resume the search.
    }

    // Read in the full keyword, stop at non-alphanumeric or string full,
    // convert to lower case too.

    if (!LoadKeywordIntoTempString (Letter)) return FALSE;

    if (strcmp (TempString, "startchunk") == 0)
    {
      if (fscanf (Infile, "%x", &MyInt) != 1)
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
        "Unable to read chunk type number after STARTCHUNK on line %d.\r\n",
        LineNumber);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        return FALSE;
      }
      if (!LE_CHUNK_StartChunk (StreamID, MyInt))
        return FALSE; // Write error.

      continue; // Done, back to keyword searching.
    }
    else if (strcmp (TempString, "endchunk") == 0)
    {
      if (!LE_CHUNK_FinishChunk (StreamID))
        return FALSE; // Write error.
      continue; // Done, back to keyword searching.
    }
    else if (strcmp (TempString, "startfixeddata") != 0) // Unknown keyword.
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
      "Unknown keyword \"%s\" encountered on line %d while looking "
      "for STARTCHUNK, ENDCHUNK or STARTFIXEDDATA.\r\n",
      TempString, LineNumber);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return FALSE;
    }

    // If we get here it must be because of a STARTFIXEDDATA keyword.
    // Now start reading in binary data.  It can be a 2 digit hex number,
    // an "L:" followed by an integer which will become a DWORD,
    // an "N:" followed by an integer which becomes a BYTE,
    // an "W:" followed by an integer which becomes a WORD (16 bits),
    // an "R:" followed by a floating point number which becomes a float,
    // or a "{" followed by a string, which ends with "}" which gets stored
    // as a string plus a NUL at the end, or it can be ENDFIXEDDATA.

    while (TRUE)
    {
      Letter = fgetc (Infile);
      if (Letter == EOF)
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
        "End of file in the middle of a STARTFIXEDDATA section, where is the ENDFIXEDDATA?\r\n");
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        return FALSE;
      }

      if (Letter == '\n')
        LineNumber++; // Count lines for error messages.

      if (isspace (Letter))
        continue; // Skip over white space.

      if (Letter == ';') // Start of a comment.
      {
        // Skip over the comment.

        while (TRUE)
        {
          Letter = fgetc (Infile);
          if (Letter == EOF)
            break; // End of comment at end of file.
          if (Letter == '\n')
          {
            LineNumber++; // Count lines for error messages.
            break; // End of comment at new line.
          }
        }
        continue; // Comment finished, resume the search.
      }

      if (tolower (Letter) == 'e')
      {
        NextLetter = fgetc (Infile); // Peek at the next letter.
        if (NextLetter == EOF)
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
          "End of file in the middle of a STARTFIXEDDATA section, where is the ENDFIXEDDATA?\r\n");
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
          return FALSE;
        }
        ungetc (NextLetter, Infile);

        if (tolower (NextLetter) == 'n')
        {
          // Ah ha, might it be ENDFIXEDDATA?  Definitely
          // not a 2 digit hex number starting with E.  Read
          // in the whole keyword.

          if (!LoadKeywordIntoTempString (Letter)) return FALSE;

          if (strcmp (TempString, "endfixeddata") == 0)
            break; // Yes!  Hit the end of the binary data.  Back to global keyword search.

#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
          "Unknown keyword \"%s\" found on line %d while looking for "
          "ENDFIXEDDATA or start of raw data.\r\n", TempString, LineNumber);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
          return FALSE;
        }
      }

      if (isxdigit (Letter))
      {
        // A digit which is the start of a hex number.

        if (!LoadKeywordIntoTempString (Letter)) return FALSE;

        MyInt = strtol (TempString, &EndPntr, 16 /* Hexadecimal */);
        if (EndPntr != TempString + strlen (TempString))
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
          "Attempt to convert \"%s\" on line %d into a hexadecimal number has "
          "failed.\r\n", TempString, LineNumber);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
          return FALSE;
        }

        MyByte = MyInt;
        if (!LE_CHUNK_WriteChunkData (StreamID, &MyByte, 1 /* Write one byte */))
          return FALSE; // Error while writing.

        continue; // Continue with reading data section.
      }

      if (Letter == '{')
      {
        // Start of a variable length string.

        while (TRUE)
        {
          Letter = fgetc (Infile);
          if (Letter == EOF)
          {
#if CE_ARTLIB_EnableDebugMode
            sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
            "End of file while trying to read a {} enclosed string.\r\n");
            LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
            return FALSE;
          }

          if (Letter == '}')
          {
            // Write the NUL byte at the end of the string.

            MyByte = 0;
            if (!LE_CHUNK_WriteChunkData (StreamID, &MyByte, 1))
              return FALSE;
            break; // Go back to data hunting.
          }

          if (Letter == '\n')
            LineNumber++;  // Still have to count lines inside text blocks.

          // Just an ordinary letter, write it.

          MyByte = Letter;
          if (!LE_CHUNK_WriteChunkData (StreamID, &MyByte, 1))
            return FALSE;
        }
        continue; // Back to hunting for data.
      }

      if (tolower (Letter) == 'l')
      {
        // A long integer value.

        Letter = fgetc (Infile);
        if (Letter != ':')
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
          "Got a \"%c\" rather than a \":\" on line %d when looking for \"L:\".\r\n",
          Letter, LineNumber);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
          return FALSE;
        }

        // Read the number after the L:, allow hex and decimal values.

        if (fscanf (Infile, "%i", &MyInt) != 1)
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
          "Unable to read your number after an \"L:\" data thingy "
          "on line %d.\r\n", LineNumber);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
          return FALSE;
        }
        if (!LE_CHUNK_WriteChunkData (StreamID, &MyInt, sizeof (MyInt)))
          return FALSE;

        continue; // Continue reading other data.
      }

      if (tolower (Letter) == 'n')
      {
        // A small byte sized number.

        Letter = fgetc (Infile);
        if (Letter != ':')
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
          "Got a \"%c\" rather than a \":\" on line %d "
          "when looking for \"N:\".\r\n", Letter, LineNumber);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
          return FALSE;
        }

        // Read the number after the N:, allow hex and decimal values.

        if (fscanf (Infile, "%i", &MyInt) != 1)
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
          "Unable to read your number after an \"N:\" data thingy "
          "on line %d.\r\n", LineNumber);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
          return FALSE;
        }
        // Fortunately this is running on little-endian processors, so
        // can reuse the address of the 4 byte number to get the low byte.
        if (!LE_CHUNK_WriteChunkData (StreamID, &MyInt, sizeof (BYTE)))
          return FALSE;

        continue; // Continue reading other data.
      }

      if (tolower (Letter) == 'w')
      {
        // A small word sized number.

        Letter = fgetc (Infile);
        if (Letter != ':')
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
          "Got a \"%c\" rather than a \":\" on line %d "
          "when looking for \"W:\".\r\n", Letter, LineNumber);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
          return FALSE;
        }

        // Read the number after the W:, allow hex and decimal values.

        if (fscanf (Infile, "%i", &MyInt) != 1)
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
          "Unable to read your number after an \"W:\" data thingy "
          "on line %d.\r\n", LineNumber);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
          return FALSE;
        }
        // Fortunately this is running on little-endian processors, so
        // can reuse the address of the 4 byte number to get the low word.
        if (!LE_CHUNK_WriteChunkData (StreamID, &MyInt, sizeof (UNS16)))
          return FALSE;

        continue; // Continue reading other data.
      }

      if (tolower (Letter) == 'r')
      {
        // A real number value (float - 32 bits).

        Letter = fgetc (Infile);
        if (Letter != ':')
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
          "Got a \"%c\" rather than a \":\" on line %d when looking "
          "for \"R:\".\r\n", Letter, LineNumber);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
          return FALSE;
        }

        // Read the number after the R:.

        if (fscanf (Infile, "%f", &MyFloat) != 1)
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
          "Unable to read your number on line %d after an \"R:\" data "
          "thingy.\r\n", LineNumber);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
          return FALSE;
        }
        if (!LE_CHUNK_WriteChunkData (StreamID, &MyFloat, sizeof (MyFloat)))
          return FALSE;

        continue; // Continue reading other data.
      }

#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_LoadClass::LoadChunks: "
        "Read a \"%c\" character on line %d while looking for fixed size data.  "
        "It isn't a hexadecimal number, L:123, B:42, or R:123.456 style number or "
        "the ENDFIXEDDATA keyword.\r\n", Letter, LineNumber);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return FALSE;
    }

    // Continue with looking for STARTCHUNK, ENDCHUNK, STARTFIXEDDATA.
  }
  return TRUE;
}



/*****************************************************************************
 * Attempt to load the chunky format text file into a binary chunk
 * stream.  Returns TRUE if successful.
 */

BOOL LE_CHUNK_LoadChunks (FILE *Infile, LE_CHUNK_StreamID StreamID)
{
  LI_CHUNK_LoadClass  MyLoader (StreamID, Infile);

  return MyLoader.LoadChunks ();
}
#endif // CE_ARTLIB_EnableChunkDumpLoad



#if CE_ARTLIB_EnableChunkFileLoading
/*****************************************************************************
 * Process the current chunk, copying from the read stream to the write
 * stream, adding file name subchunks for all referenced DataIDs in it,
 * and adding or removing serial number subchunks as requested.
 * Returns FALSE on error or end of data.
 */

static BOOL LI_CHUNK_AddFileNamesToOneChunk (LE_CHUNK_StreamID ReadStreamID,
LE_CHUNK_StreamID WriteStreamID, BOOL AddSerialNumberChunk)
{
  union FixedPartUnion
  {
    LE_SEQNCR_SequenceIndirectChunkRecord   seqIndirect;
    LE_SEQNCR_Sequence2DBitmapChunkRecord   seq2DBitmap;
    LE_SEQNCR_Sequence3DModelChunkRecord    seq3DModel;
    LE_SEQNCR_SequenceSoundChunkRecord      seqSound;
    LE_SEQNCR_SequencePreloaderChunkRecord  seqPreloader;
    LE_SEQNCR_Sequence3DMeshChunkRecord     seq3DMesh;
  } *FixedPartPntr;

  UNS32                   AmountActuallyRead;
  LE_CHUNK_ID             ChunkID;
  const char             *DataName;
  LE_CHUNK_DopeDataRecord DopeData;
  char                    FileNameArray[5][MAX_PATH];
  UNS16                   FixedSize;
  int                     i;
  BOOL                    ReturnCode = FALSE;
  UNS32                   ReturnedChunkDataSize;
  LE_CHUNK_ID             SubchunkID;
  UNS32                   SubchunkStartOffset;

  // See if there is a next chunk, descend into it if present.

  if ((ChunkID = LE_CHUNK_Descend (ReadStreamID, LE_CHUNK_ID_NULL_STANDARD,
  &ReturnedChunkDataSize)) == LE_CHUNK_ID_NULL_STANDARD)
    return FALSE; // Nothing to do.

  if ((ChunkID >= LE_CHUNK_ID_FILE_NAME_1 && ChunkID <= LE_CHUNK_ID_FILE_NAME_5) ||
  ChunkID == LE_CHUNK_ID_DOPE_DATA)
  {
    // One of those dreaded file name chunks, or maybe a serial number.
    // Don't copy it to the output, the processing of the parent chunk
    // will be creating a new one.

    if (!LE_CHUNK_Ascend (ReadStreamID)) goto ErrorExit;
    return TRUE;
  }

  // Start a matching chunk in the write stream.

  if (!LE_CHUNK_StartChunk (WriteStreamID, ChunkID)) goto ErrorExit;

  // Get a pointer to the data inside the chunk.  Since it is from a
  // DataID, this should be cheap and give us any amount of data,
  // not just the size of the map buffer.

  FixedPartPntr = (union FixedPartUnion *)
    LE_CHUNK_MapChunkData (ReadStreamID,  ReturnedChunkDataSize, &AmountActuallyRead);

  if (AmountActuallyRead != ReturnedChunkDataSize)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_AddFileNamesToOneChunk: "
      "Only read %d bytes when trying to read %d.\r\n",
      (int) AmountActuallyRead, (int) ReturnedChunkDataSize);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Check for chunks which definitely don't need DataID substitution.

  if (ChunkID >= LE_CHUNK_ID_NULL_IN_CONTEXT ||
  (FixedSize = LE_CHUNK_ChunkFixedDataSizes [ChunkID]) == 0xFFFF)
  {
    // A variable sized or private chunk, just copy the whole thing,
    // no DataIDs that we can know about inside.

    if (!LE_CHUNK_WriteChunkData (WriteStreamID, FixedPartPntr,
    ReturnedChunkDataSize))
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_AddFileNamesToOneChunk: "
        "Error while writing %d bytes.\r\n", (int) ReturnedChunkDataSize);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    if (!LE_CHUNK_FinishChunk (WriteStreamID)) goto ErrorExit;
    if (!LE_CHUNK_Ascend (ReadStreamID)) goto ErrorExit;
    return TRUE;
  }

  // This chunk needs DataID substitution.  First write out the
  // fixed part header, which is unchanged (we leave the DataIDs
  // in there for curiousity, even though they get overwritten
  // when loaded the next time).

  if (!LE_CHUNK_WriteChunkData (WriteStreamID, FixedPartPntr, FixedSize))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_AddFileNamesToOneChunk: "
      "Error while writing %d bytes fixed part.\r\n", (int) FixedSize);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Rewind to just after the fixed part, where the child chunks start.

  SubchunkStartOffset = LE_CHUNK_GetDataStartOffsetForLevel (
    ReadStreamID, LE_CHUNK_GetCurrentLevel (ReadStreamID));
  SubchunkStartOffset += FixedSize;
  LE_CHUNK_SetCurrentFilePosition (ReadStreamID, SubchunkStartOffset);

  // Read in the file name and serial number chunks already present.  The
  // names will get overwritten by actual names, if we have them, and then
  // will get written out again, after all other chunks (the sequencer relies
  // on all private chunks (bounding boxes, transformation,
  // dimensionality) coming before public animation (bitmap and sound)
  // chunks).  The same thing is done for the serial number - we keep the
  // contents and then write it out later on.

  for (i = 0; i < 5; i++)
    FileNameArray[i][0] = 0; // Empty string.
  memset (&DopeData, 0, sizeof (DopeData));

  while (TRUE)
  {
    SubchunkID = LE_CHUNK_Descend (ReadStreamID, LE_CHUNK_ID_NULL_STANDARD,
      &ReturnedChunkDataSize);

    if (SubchunkID == LE_CHUNK_ID_NULL_STANDARD)
      break; // Reached the end, or IO error.

    if (SubchunkID >= LE_CHUNK_ID_FILE_NAME_1 &&
    SubchunkID <= LE_CHUNK_ID_FILE_NAME_5)
    {
      if (ReturnedChunkDataSize > MAX_PATH)
        ReturnedChunkDataSize = MAX_PATH;

      AmountActuallyRead = LE_CHUNK_ReadChunkData (ReadStreamID,
        FileNameArray[SubchunkID - LE_CHUNK_ID_FILE_NAME_1],
        ReturnedChunkDataSize);

      if (AmountActuallyRead != ReturnedChunkDataSize)
        goto ErrorExit; // Read error.
    }
    else if (SubchunkID == LE_CHUNK_ID_DOPE_DATA)
    {
      if (ReturnedChunkDataSize > sizeof (DopeData))
        ReturnedChunkDataSize = sizeof (DopeData);

      AmountActuallyRead = LE_CHUNK_ReadChunkData (ReadStreamID,
        &DopeData, ReturnedChunkDataSize);

      if (AmountActuallyRead != ReturnedChunkDataSize)
        goto ErrorExit; // Read error.
    }

    if (!LE_CHUNK_Ascend (ReadStreamID)) goto ErrorExit;
  }

  // Substitute the names from the DataIDs used in the
  // header part of the chunk.

  switch (ChunkID)
  {
  case LE_CHUNK_ID_SEQ_INDIRECT:
    DataName = LI_DATA_GetDataIDName (
      FixedPartPntr->seqIndirect.subsequenceChunkListDataID);
    if (DataName != NULL)
      strcpy (FileNameArray[0], DataName);
    break;

  case LE_CHUNK_ID_SEQ_2DBITMAP:
    DataName = LI_DATA_GetDataIDName (
      FixedPartPntr->seq2DBitmap.bitmapDataID);
    if (DataName != NULL)
      strcpy (FileNameArray[0], DataName);
    break;

  case LE_CHUNK_ID_SEQ_3DMODEL:
    DataName = LI_DATA_GetDataIDName (
      FixedPartPntr->seq3DModel.modelDataID);
    if (DataName != NULL)
      strcpy (FileNameArray[0], DataName);
    DataName = LI_DATA_GetDataIDName (
      FixedPartPntr->seq3DModel.textureMapDataID);
    if (DataName != NULL)
      strcpy (FileNameArray[1], DataName);
    DataName = LI_DATA_GetDataIDName (
      FixedPartPntr->seq3DModel.jointPositionsDataID);
    if (DataName != NULL)
      strcpy (FileNameArray[2], DataName);
    break;

  case LE_CHUNK_ID_SEQ_SOUND:
    DataName = LI_DATA_GetDataIDName (
      FixedPartPntr->seqSound.soundDataID);
    if (DataName != NULL)
      strcpy (FileNameArray[0], DataName);
    break;

  case LE_CHUNK_ID_SEQ_PRELOADER:
    DataName = LI_DATA_GetDataIDName (
      FixedPartPntr->seqPreloader.preloadDataID);
    if (DataName != NULL)
      strcpy (FileNameArray[0], DataName);
    break;

  case LE_CHUNK_ID_SEQ_3DMESH:
    DataName = LI_DATA_GetDataIDName (
      FixedPartPntr->seq3DMesh.modelDataID);
    if (DataName != NULL)
      strcpy (FileNameArray[0], DataName);
    break;
  }

  // Now recursively process the child chunks, before we add the new
  // file name and serial number chunks at the end.

  LE_CHUNK_SetCurrentFilePosition (ReadStreamID, SubchunkStartOffset);

  while (LI_CHUNK_AddFileNamesToOneChunk (ReadStreamID,
  WriteStreamID, AddSerialNumberChunk))
    ; // Empty loop body.

  // Write out the serial number chunk, if requested.

  if (AddSerialNumberChunk)
  {
    DopeData.serialNumber = LE_CHUNK_GetNextSerialNumber ();

    if (!LE_CHUNK_StartChunk (WriteStreamID, LE_CHUNK_ID_DOPE_DATA)) goto ErrorExit;

    if (!LE_CHUNK_WriteChunkData (WriteStreamID, &DopeData, sizeof (DopeData)))
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_AddFileNamesToOneChunk: "
        "Error while writing serial number chunk.\r\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    if (!LE_CHUNK_FinishChunk (WriteStreamID)) goto ErrorExit;
  }

  // Finally, write out the file names we have collected, as child chunks.

  for (i = 0; i < 5; i++)
  {
    if (FileNameArray[i][0] != 0)
    {
      if (!LE_CHUNK_StartChunk (WriteStreamID, LE_CHUNK_ID_FILE_NAME_1 + i)) goto ErrorExit;

      if (!LE_CHUNK_WriteChunkData (WriteStreamID,
      FileNameArray[i], strlen (FileNameArray[i]) + 1))
      {
  #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_AddFileNamesToOneChunk: "
          "Error while writing file name %d (%s).\r\n", i, FileNameArray[i]);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
        goto ErrorExit;
      }

      if (!LE_CHUNK_FinishChunk (WriteStreamID)) goto ErrorExit;
    }
  }

  // End the whole chunk now that we have finished the file names.

  if (!LE_CHUNK_FinishChunk (WriteStreamID)) goto ErrorExit;
  if (!LE_CHUNK_Ascend (ReadStreamID)) goto ErrorExit;

  return TRUE; // Successful!

ErrorExit:
  return FALSE;
}



/*****************************************************************************
 * Go through the given chunky DataID and add file name subchunks for all
 * referenced DataIDs in it.  If there is already a name chunk, it gets
 * replaced with an updated name if there is one, or if the DataID is
 * invalid then the old name gets preserved.  Name chunks beyond the
 * range of used names are passed through unchanged.  If AddSerialNumberChunks
 * is TRUE then serial number chunks will be added for all sequence chunks,
 * existing serial number chunks will be assigned a new number (but the flags
 * will be preserved).  If AddSerialNumberChunks is FALSE then all serial
 * number chunks will be removed.
 */

BOOL LE_CHUNK_AddFileNameChunks (LE_DATA_DataId MainDataID,
BOOL AddSerialNumberChunks)
{
#if CE_ARTLIB_EnableDebugMode
  char  DescString [2048];
#endif

  LE_DATA_DataId    CopyID = LE_DATA_EmptyItem;
  BYTE             *DestPntr;
  UNS32             NewSize;
  LE_CHUNK_StreamID ReadStreamID = -1;
  BOOL              ReturnCode = FALSE;
  BYTE             *SourcePntr;
  LE_CHUNK_StreamID WriteStreamID = -1;

  if (LE_DATA_GetLoadedDataType (MainDataID) != LE_DATA_DataChunky)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_AddFileNameChunks: "
      "Need a chunky data item, instead given %s.\r\n",
      LE_DATA_DescString (MainDataID, DescString, sizeof (DescString)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  ReadStreamID = LE_CHUNK_ReadFromDataID (MainDataID);
  if (ReadStreamID < 0)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_AddFileNameChunks: "
      "Unable to create read stream on %s.\r\n",
      LE_DATA_DescString (MainDataID, DescString, sizeof (DescString)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  CopyID = LE_DATA_AllocMemoryDataID (LE_DATA_GetLoadedSize (MainDataID),
    LE_DATA_DataChunky);

  if (CopyID == LE_DATA_EmptyItem)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_AddFileNameChunks: "
      "Unable to create temporary copy data item for %s.\r\n",
      LE_DATA_DescString (MainDataID, DescString, sizeof (DescString)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  WriteStreamID = LE_CHUNK_WriteToDataID (CopyID);
  if (WriteStreamID < 0)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_AddFileNameChunks: "
      "Unable to create write stream on %s.\r\n",
      LE_DATA_DescString (CopyID, DescString, sizeof (DescString)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Now have the read and write streams open, copy chunks from one to
  // the other, except for file name chunks which get examined and
  // maybe added or removed.

  while (LI_CHUNK_AddFileNamesToOneChunk (ReadStreamID, WriteStreamID,
  AddSerialNumberChunks))
    ; // Empty loop body.

  // Close streams to flush things out.

  LE_CHUNK_CloseReadStream (ReadStreamID);
  ReadStreamID = -1;
  LE_CHUNK_CloseWriteStream (WriteStreamID);
  WriteStreamID = -1;

  // Copy the new DataID over the old one, resize the old one first.

  NewSize = LE_DATA_GetCurrentSize (CopyID);

  if (!LE_DATA_Resize (MainDataID, NewSize))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_AddFileNameChunks: "
      "Unable to resize original %s.\r\n",
      LE_DATA_DescString (CopyID, DescString, sizeof (DescString)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  DestPntr = (LPBYTE) LE_DATA_UseDirty (MainDataID);
  SourcePntr = (LPBYTE) LE_DATA_Use (CopyID);
  if (SourcePntr == NULL || DestPntr == NULL)
    goto ErrorExit;

  memcpy (DestPntr, SourcePntr, NewSize);
  ReturnCode = TRUE; // Successful enough.

ErrorExit:
  if (ReadStreamID >= 0)
    LE_CHUNK_CloseReadStream (ReadStreamID);

  if (WriteStreamID >= 0)
    LE_CHUNK_CloseWriteStream (WriteStreamID);

  if (CopyID != LE_DATA_EmptyItem)
    LE_DATA_FreeRuntimeDataID (CopyID);

  return ReturnCode;
}



/*****************************************************************************
 * Returns the next serial number in a multitasking safe way.
 */

LE_CHUNK_SerialNumber LE_CHUNK_GetNextSerialNumber (void)
{
  LE_CHUNK_SerialNumber         NewSerialNumber;

  // Doesn't work under Win95:
  // NewSerialNumber = InterlockedIncrement (&LI_CHUNK_LastSerialNumber);

  // So use this one instead.

#if CE_ARTLIB_EnableMultitasking
  if (StreamAllocCS != NULL)
  {
    EnterCriticalSection (StreamAllocCS);
    NewSerialNumber = ++LI_CHUNK_LastSerialNumber;
    LeaveCriticalSection (StreamAllocCS);
  }
  else
#endif
    NewSerialNumber = ++LI_CHUNK_LastSerialNumber;

  return NewSerialNumber;
}



/*****************************************************************************
 * Go through the given chunky data item and load all the things it
 * refers to, creating DataIDs as necessary.
 */

static BOOL LI_CHUNK_ParseAndLoadDataIDs (LE_DATA_DataId MainDataID,
const char *UserNameForMainDataID, BOOL AddSerialNumberChunks,
LE_DIRINI_PathListPointer *SearchPathPntrPntr)
{
  union FixedPartUnion
  {
    LE_SEQNCR_SequenceIndirectChunkRecord   seqIndirect;
    LE_SEQNCR_Sequence2DBitmapChunkRecord   seq2DBitmap;
    LE_SEQNCR_Sequence3DModelChunkRecord    seq3DModel;
    LE_SEQNCR_SequenceSoundChunkRecord      seqSound;
    LE_SEQNCR_SequencePreloaderChunkRecord  seqPreloader;
    LE_SEQNCR_Sequence3DMeshChunkRecord     seq3DMesh;
  } *FixedPartPntr;

  UNS32             AmountActuallyRead;
  int               ChunkCounter = 0;
  LE_CHUNK_ID       ChunkID;
  char              FileNameArray[5][MAX_PATH];
  UNS16             FixedSize;
  int               i;
  LE_CHUNK_StreamID ReadStreamID;
  BOOL              ReturnCode = FALSE;
  UNS32             ReturnedChunkDataSize;
  LE_CHUNK_ID       SubchunkID;
  UNS32             SubchunkStartOffset;

  ReadStreamID = LE_CHUNK_ReadFromDataID (MainDataID);
  if (ReadStreamID < 0)
    goto ErrorExit;

  while (TRUE)
  {
    // See if there is a next chunk, descend into it if present.
    // This could be a child chunk or the following sibling chunk.

    if ((ChunkID = LE_CHUNK_Descend (ReadStreamID, LE_CHUNK_ID_NULL_STANDARD,
    &ReturnedChunkDataSize)) == LE_CHUNK_ID_NULL_STANDARD)
    {
      // Reached end of data (parent chunk or file), or IO error.

      if (LE_CHUNK_GetCurrentLevel (ReadStreamID) <= 0)
        break; // Hit the end of the file.

      // Ascend - back out and look at the next sibling.

      if (!LE_CHUNK_Ascend (ReadStreamID))
        goto ErrorExit;

      continue;
    }

    // Check for chunks which don't need DataID substitution.

    if (ChunkID >= LE_CHUNK_ID_NULL_IN_CONTEXT)
    {
      if (!LE_CHUNK_Ascend (ReadStreamID)) goto ErrorExit;
      continue; // This private chunk doesn't contain any DataIDs or children.
    }

    FixedSize = LE_CHUNK_ChunkFixedDataSizes [ChunkID];
    if (FixedSize == 0xFFFF)
    {
      if (!LE_CHUNK_Ascend (ReadStreamID)) goto ErrorExit;
      continue; // Variable sized chunks doesn't contain any DataIDs or children.
    }

    // Read the fixed size header.  Well, actually get a pointer to it
    // in memory, since this is a DataID we can actually write to
    // the memory involved.

    FixedPartPntr = (union FixedPartUnion *)
      LE_CHUNK_MapChunkData (ReadStreamID,  FixedSize, &AmountActuallyRead);

    if (AmountActuallyRead != FixedSize)
      goto ErrorExit; // Read error.

    ChunkCounter++; // Count for user interface reasons.
    SubchunkStartOffset = LE_CHUNK_GetCurrentFilePosition (ReadStreamID);

    // Read the filename subchunks.

    for (i = 0; i < 5; i++)
      FileNameArray[i][0] = 0; // Empty string.

    while (TRUE)
    {
      SubchunkID = LE_CHUNK_Descend (ReadStreamID, LE_CHUNK_ID_NULL_STANDARD,
        &ReturnedChunkDataSize);

      if (SubchunkID == LE_CHUNK_ID_NULL_STANDARD)
        break; // Reached the end, or IO error.

      if (SubchunkID >= LE_CHUNK_ID_FILE_NAME_1 &&
      SubchunkID <= LE_CHUNK_ID_FILE_NAME_5)
      {
        if (ReturnedChunkDataSize > MAX_PATH)
          ReturnedChunkDataSize = MAX_PATH;

        AmountActuallyRead = LE_CHUNK_ReadChunkData (ReadStreamID,
          FileNameArray[SubchunkID - LE_CHUNK_ID_FILE_NAME_1],
          ReturnedChunkDataSize);

        if (AmountActuallyRead != ReturnedChunkDataSize)
          goto ErrorExit; // Read error.
      }
      if (!LE_CHUNK_Ascend (ReadStreamID)) goto ErrorExit;
    }

    // Substitute the names for the DataID portion of the header.

    switch (ChunkID)
    {
    case LE_CHUNK_ID_SEQ_INDIRECT:
      if (FileNameArray[0][0] == 0)
        sprintf (FileNameArray[0], "Indirect Sequence %d from %s sequence.",
        ChunkCounter, UserNameForMainDataID);
      FixedPartPntr->seqIndirect.subsequenceChunkListDataID =
        LE_CHUNK_LoadUsingEmbeddedFileNames (FileNameArray[0],
        AddSerialNumberChunks, SearchPathPntrPntr);
      if (FixedPartPntr->seqIndirect.subsequenceChunkListDataID ==
      LE_DATA_EmptyItem)
        goto ErrorExit;
      break;

    case LE_CHUNK_ID_SEQ_2DBITMAP:
      if (FileNameArray[0][0] == 0)
        sprintf (FileNameArray[0], "2D Bitmap %d from %s sequence.",
        ChunkCounter, UserNameForMainDataID);
      FixedPartPntr->seq2DBitmap.bitmapDataID =
        LE_CHUNK_LoadUsingEmbeddedFileNames (FileNameArray[0],
        AddSerialNumberChunks, SearchPathPntrPntr);
      if (FixedPartPntr->seq2DBitmap.bitmapDataID == LE_DATA_EmptyItem)
        goto ErrorExit;
      break;

    case LE_CHUNK_ID_SEQ_3DMODEL:
      if (FileNameArray[0][0] == 0)
        sprintf (FileNameArray[0], "3D Model %d from %s sequence.",
        ChunkCounter, UserNameForMainDataID);
      FixedPartPntr->seq3DModel.modelDataID =
        LE_CHUNK_LoadUsingEmbeddedFileNames (FileNameArray[0],
        AddSerialNumberChunks, SearchPathPntrPntr);
      if (FixedPartPntr->seq3DModel.modelDataID == LE_DATA_EmptyItem)
        goto ErrorExit;
      // Texture map and joint angles are optional, so no error check
      // or prompting the user if the file name isn't specified.
      FixedPartPntr->seq3DModel.textureMapDataID =
        LE_CHUNK_LoadUsingEmbeddedFileNames (FileNameArray[1],
        AddSerialNumberChunks, SearchPathPntrPntr);
      FixedPartPntr->seq3DModel.jointPositionsDataID =
        LE_CHUNK_LoadUsingEmbeddedFileNames (FileNameArray[2],
        AddSerialNumberChunks, SearchPathPntrPntr);
      break;

    case LE_CHUNK_ID_SEQ_SOUND:
      if (FixedPartPntr->seqSound.soundDataID == LE_DATA_EmptyItem &&
      FileNameArray[4][0] != 0)
      {
        // This is a spooling sound with an external file name specified
        // in the LE_CHUNK_ID_FILE_NAME_5 subchunk and no internal data.
      }
      else // Sound is internal, in the data file / dataID.
      {
        if (FileNameArray[0][0] == 0)
          sprintf (FileNameArray[0], "Sound %d from %s sequence.",
          ChunkCounter, UserNameForMainDataID);
        FixedPartPntr->seqSound.soundDataID =
          LE_CHUNK_LoadUsingEmbeddedFileNames (FileNameArray[0],
          AddSerialNumberChunks, SearchPathPntrPntr);
        if (FixedPartPntr->seqSound.soundDataID == LE_DATA_EmptyItem)
          goto ErrorExit;
      }
      break;

    case LE_CHUNK_ID_SEQ_PRELOADER:
      if (FileNameArray[0][0] == 0)
        sprintf (FileNameArray[0], "Preloader Item %d from %s sequence.",
        ChunkCounter, UserNameForMainDataID);
      FixedPartPntr->seqPreloader.preloadDataID =
        LE_CHUNK_LoadUsingEmbeddedFileNames (FileNameArray[0],
        AddSerialNumberChunks, SearchPathPntrPntr);
      if (FixedPartPntr->seqPreloader.preloadDataID == LE_DATA_EmptyItem)
        goto ErrorExit;
      break;

    case LE_CHUNK_ID_SEQ_3DMESH:
      if (FileNameArray[0][0] == 0)
        sprintf (FileNameArray[0], "3D Mesh %d from %s sequence.",
        ChunkCounter, UserNameForMainDataID);
      FixedPartPntr->seq3DMesh.modelDataID =
        LE_CHUNK_LoadUsingEmbeddedFileNames (FileNameArray[0],
        AddSerialNumberChunks, SearchPathPntrPntr);
      if (FixedPartPntr->seq3DMesh.modelDataID == LE_DATA_EmptyItem)
        goto ErrorExit;
      break;
    }

    // Continue - processing the child chunks of this one.

    LE_CHUNK_SetCurrentFilePosition (ReadStreamID, SubchunkStartOffset);
  }

  ReturnCode = TRUE; // Successful.

ErrorExit:
  if (ReadStreamID >= 0)
    LE_CHUNK_CloseReadStream (ReadStreamID);
  return ReturnCode;
}



/*****************************************************************************
 * Load the given file and all the data referenced by it.  If it isn't a
 * chunky object then it just gets loaded (if not already loaded) and the
 * newly created DataID is returned.  If it is a chunky object then the
 * embedded file names in the chunky object are used to recursively load
 * the data used by the chunky object (and the chunky object's DataID
 * references get updated to point to the newly loaded data).  The file
 * name subchunks specify the file names to be loaded, and are combined
 * with the user search path (NULL if you don't have one) and the global
 * search path.  The newly created file is added to the Name/DataID hash
 * table so that items with the same name don't get loaded twice.  The user
 * search path list has paths encountered during the loading added to it,
 * giving you a kind of log of where the data was found.
 */

LE_DATA_DataId LE_CHUNK_LoadUsingEmbeddedFileNames (
const char *FileName, BOOL AddSerialNumberChunks,
LE_DIRINI_PathListPointer *SearchPathPntrPntr)
{
  char              AbsoluteName [MAX_PATH];
  char             *ExtensionPntr;
  BOOL              GotFileName;
  FILE             *InputFile = NULL;
  LE_DATA_DataId    MainDataID = LE_DATA_EmptyItem;
  OPENFILENAME      OpenInfo;
  char             *StringPntr;
  char              TempString [MAX_PATH];
  LE_CHUNK_StreamID WriteStreamID = -1;

  if (FileName == NULL || FileName[0] == 0)
    return LE_DATA_EmptyItem;

  // Expand the name into an absolute path to the file first,
  // so that when we look for the same name later, we don't get
  // confused by changing current directories.  This also checks
  // that the file exists.  And if it doesn't exist, prompts
  // the user to find it.

  if (!LE_DIRINI_ExpandFileName (FileName, AbsoluteName,
  (SearchPathPntrPntr == NULL) ? NULL : *SearchPathPntrPntr))
  {
    memset (&OpenInfo, 0, sizeof (OpenInfo));
    OpenInfo.lStructSize = sizeof (OpenInfo);
    OpenInfo.hwndOwner = LE_MAIN_HwndMainWindow;
    OpenInfo.lpstrFilter = "All Kinds\0*.*\0";
    OpenInfo.lpstrFile = TempString;
    OpenInfo.nMaxFile = sizeof (TempString);
    TempString [0] = 0;
    sprintf (AbsoluteName, "Find \"%s\"", FileName);
    OpenInfo.lpstrTitle = AbsoluteName;
    OpenInfo.Flags = OFN_FILEMUSTEXIST | OFN_LONGNAMES |
      OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;

    ShowCursor (TRUE);
    GotFileName = GetOpenFileName (&OpenInfo);
    ShowWindow (LE_MAIN_HwndMainWindow, SW_RESTORE);
    ShowCursor (FALSE);
    if (GotFileName)
    {
      if (!LE_DIRINI_ExpandFileName (TempString, AbsoluteName,
      (SearchPathPntrPntr == NULL) ? NULL : *SearchPathPntrPntr))
        GotFileName = FALSE;
    }
    if (!GotFileName)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_LoadUsingEmbeddedFileNames: "
      "Unable to find file name \"%s\".\r\n", FileName);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }
  }

  // Convenience function - convert .CKY text format chunky files into
  // binary format and save the result as a .CNK file (essentially doing
  // a LoadCnk for the user).  Internally chunky files are always referenced
  // with a .CNK file extension in the file names.

  StringPntr = strrchr (AbsoluteName, '.');
  if (StringPntr != NULL && stricmp (StringPntr + 1, "cky") == 0)
  {
    strcpy (TempString, AbsoluteName); // Save the original .CKY name.
    strcpy (StringPntr + 1, "cnk"); // AbsoluteName is now .CNK binary name.

    MainDataID = LE_DATA_LookUpFileNameInIndex (AbsoluteName);
    if (MainDataID != LE_DATA_EmptyItem)
      goto NormalExit; // It already exists, return it.

    // Need to do the conversion.  Open .CKY file for input.

    InputFile = fopen (TempString, "rt");
    if (InputFile == NULL)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_LoadUsingEmbeddedFileNames: "
      "Unable to open file \"%s\" for reading.\r\n", TempString);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    // Create a new DataID using the .CNK variation of the file name.

    MainDataID = LE_DATA_AllocExternalFileDataID (AbsoluteName,
      LE_DATA_DataChunky, 4000);

    if (MainDataID == LE_DATA_EmptyItem)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_LoadUsingEmbeddedFileNames: "
        "Unable to create new DataID for file \"%s\", as part of "
        "converting \"%s\" to binary format.\r\n", AbsoluteName, TempString);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    WriteStreamID = LE_CHUNK_WriteToDataID (MainDataID);
    if (WriteStreamID < 0)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_LoadUsingEmbeddedFileNames: "
        "Unable to create write stream on new DataID for file \"%s\", as part of "
        "converting \"%s\" to binary format.\r\n", AbsoluteName, TempString);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    // Do the conversion.

    if (!LE_CHUNK_LoadChunks (InputFile, WriteStreamID))
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_LoadUsingEmbeddedFileNames: "
        "Conversion went badly when converting \"%s\" to binary format.\r\n",
        TempString);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    // Conversion done, close files so that the new DataID is useable.

    fclose (InputFile);
    InputFile = NULL;
    LE_CHUNK_CloseWriteStream (WriteStreamID);
    WriteStreamID = -1;
  }
  else // Don't need to convert, just open existing file as a binary DataID.
  {
    // Is this item already loaded?

    MainDataID = LE_DATA_LookUpFileNameInIndex (AbsoluteName);
    if (MainDataID != LE_DATA_EmptyItem)
      goto NormalExit; // It already exists, return it.

    MainDataID = LE_DATA_AllocExternalFileDataID (AbsoluteName);
    if (MainDataID == LE_DATA_EmptyItem)
    {
  #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_LoadUsingEmbeddedFileNames: "
      "Unable to create a new DataID from file \"%s\".\r\n", AbsoluteName);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
      goto ErrorExit;
    }
  }

  // See if it is possible to load the data.

  if (!LE_DATA_Load (MainDataID))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_LoadUsingEmbeddedFileNames: "
    "Load of data for new DataID failed, was working on \"%s\".\r\n",
    AbsoluteName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Add this DataID/name to the hash table before recursively expanding
  // subitems, so if it is self referential, it won't get stuck in a loop.

  if (!LE_DATA_AddNamedDataIDToIndex (MainDataID))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_LoadUsingEmbeddedFileNames: "
    "Unable to add DataID to hash table for file \"%s\".\r\n", AbsoluteName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Also add its path to the user's search path list, if present.

  if (SearchPathPntrPntr != NULL)
  {
    strcpy (TempString, AbsoluteName);
    StringPntr = strrchr (TempString, '\\');
    if (StringPntr != NULL)
    {
      *StringPntr = 0; // Remove file name and slash from the string.

      // See if the parent directory is the same as the file's
      // extension, We want to change "C:\Temp\WAV\Mysound.wav"
      // into "C:\Temp".  Also have a few standard directories
      // which are presumed to hold standard file types,
      // like the TCB directory holding TABs and UAPs too.

      StringPntr = strrchr (TempString, '\\'); // Find parent directory start.
      if (StringPntr != NULL)
      {
        ExtensionPntr = strrchr (AbsoluteName, '.'); // Find extension of name.
        if (ExtensionPntr != NULL)
        {
          // If you add more mappings from directories to file extension here,
          // please also add them to LE_DIRINI_ExpandFileName.

          if (stricmp (StringPntr + 1, ExtensionPntr + 1) == 0 ||
          (stricmp (StringPntr + 1, "seq") == 0 && stricmp (ExtensionPntr + 1, "cnk") == 0) ||
          (stricmp (StringPntr + 1, "seq") == 0 && stricmp (ExtensionPntr + 1, "cky") == 0) ||
          (stricmp (StringPntr + 1, "tcb") == 0 && stricmp (ExtensionPntr + 1, "tab") == 0) ||
          (stricmp (StringPntr + 1, "tcb") == 0 && stricmp (ExtensionPntr + 1, "uap") == 0))
          {
            *StringPntr = 0; // Remove parent directory from the string.
          }
        }
      }

      if (!LE_DIRINI_AddPath (TempString, SearchPathPntrPntr))
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_LoadUsingEmbeddedFileNames: "
        "Unable to add path \"%s\" for file \"%s\".\r\n",
        TempString, AbsoluteName);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        goto ErrorExit;
      }
    }
  }

  // If it isn't a chunky item then we are done.

  if (LE_DATA_GetLoadedDataType (MainDataID) != LE_DATA_DataChunky)
    goto NormalExit;

  // Make a readable name for the user prompt which happens when it
  // can't find data refered to by a subchunk.  The full path name
  // is just too long.

  StringPntr = strrchr (AbsoluteName, '\\');
  if (StringPntr == NULL)
    StringPntr = AbsoluteName;
  else
    StringPntr++; // Skip the slash.

  // Now load all data referenced by subchunks, and set the DataIDs to
  // point to the loaded data.

  if (!LI_CHUNK_ParseAndLoadDataIDs (MainDataID, StringPntr,
  AddSerialNumberChunks, SearchPathPntrPntr))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_LoadUsingEmbeddedFileNames: "
    "Unable to parse DataID fields for chunky file \"%s\".\r\n", AbsoluteName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Also add in file name chunks for all the items, so that the user's
  // selections of file names get remembered.

  if (!LE_CHUNK_AddFileNameChunks (MainDataID, AddSerialNumberChunks))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_CHUNK_LoadUsingEmbeddedFileNames: "
    "Unable to add file name subchunks to DataID holding chunky file \"%s\".\r\n",
    AbsoluteName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  goto NormalExit; // Success!


ErrorExit:
  if (MainDataID != LE_DATA_EmptyItem)
  {
    LE_DATA_FreeRuntimeDataID (MainDataID);
    MainDataID = LE_DATA_EmptyItem;
  }
NormalExit:
  if (InputFile != NULL)
    fclose (InputFile);

  if (WriteStreamID >= 0)
    LE_CHUNK_CloseWriteStream (WriteStreamID);

  return MainDataID;
}
#endif // CE_ARTLIB_EnableChunkFileLoading



/*****************************************************************************
 *****************************************************************************
 **  I N I T   A N D   S H U T D O W N   R O U T I N E S                    **
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * Initialise the chunky support routines.  Halts on failure.
 */

void LI_CHUNK_InitSystem (void)
{
  MapperFunctionsArray [MAPPER_FOR_FILE_READING] = LI_CHUNK_FileReadMapper;

#if CE_ARTLIB_EnableSystemData
  MapperFunctionsArray [MAPPER_FOR_DATA_ITEM_READING] = LI_CHUNK_DataIDReadMapper;
#endif

#if CE_ARTLIB_EnableChunkWriting
  MapperFunctionsArray [MAPPER_FOR_FILE_WRITING] = LI_CHUNK_FileWriterMapper;
#endif

#if CE_ARTLIB_EnableChunkWriting && CE_ARTLIB_EnableSystemData
  MapperFunctionsArray [MAPPER_FOR_DATA_ITEM_WRITING] = LI_CHUNK_DataIDWriterMapper;
#endif

  StreamAllocCS = &StreamAllocCriticalSectionStorage;
  InitializeCriticalSection (StreamAllocCS);
}



/*****************************************************************************
 * Deallocate stuff used by the chunky support routines.
 */

void LI_CHUNK_RemoveSystem (void)
{
  LE_CHUNK_StreamID StreamIndex;

#if CE_ARTLIB_EnableDebugMode
  {
    int NumberOfOpenStreams;

    NumberOfOpenStreams = 0;
    for (StreamIndex = 0; StreamIndex < CE_ARTLIB_MaxOpenChunkStreams; StreamIndex++)
      if (StreamArray[StreamIndex].inUse)
        NumberOfOpenStreams++;

    if (NumberOfOpenStreams > 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_CHUNK_RemoveSystem: "
      "There were %d steams left open at exit time!\r\n", NumberOfOpenStreams);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    }
  }
#endif

  // Just close the open files and deallocate buffers.  Don't bother writing
  // out pending chunks etc.  Assume that OpenStream isn't being called while
  // we are cleaning up.

  for (StreamIndex = 0; StreamIndex < CE_ARTLIB_MaxOpenChunkStreams; StreamIndex++)
  {
    if (StreamArray[StreamIndex].inUse)
    {
      CloseStream (StreamIndex);
    }
  }

  if (StreamAllocCS != NULL)
  {
    DeleteCriticalSection (StreamAllocCS);
    StreamAllocCS = NULL;
  }
}

#endif /* CE_ARTLIB_EnableSystemChunk */
