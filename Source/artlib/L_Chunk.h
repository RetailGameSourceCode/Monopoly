#ifndef __L_CHUNK_H__
#define __L_CHUNK_H__

/*****************************************************************************
 * FILE:        L_Chunk.h
 *
 * DESCRIPTION: Chunky file format utilities and structures.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Chunk.h 49    99-10-19 4:18p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Chunk.h $
 * 
 * 49    99-10-19 4:18p Agmsmith
 * Adding the delete key tweeker feature.
 *
 * 48    99/09/10 12:44p Agmsmith
 * Return a flag when getting interpolated data telling you if you are on
 * a keyframe.
 *
 * 47    8/09/99 5:35p Agmsmith
 * Added time to for-all-chunks function, needed for dope sheet select
 * next sequence feature.
 *
 * 46    7/20/99 1:43p Agmsmith
 * Added dimensionality subchunk to CreateSequence grouping chunk.
 *
 * 45    7/16/99 4:31p Agmsmith
 * Change duration now has depth level.
 *
 * 44    7/15/99 3:22p Agmsmith
 * Add LE_CHUNK_FindOneSelected series of functions.
 *
 * 43    6/04/99 1:00p Agmsmith
 * Adding field of view private chunk and getting cameras to work.
 *
 * 42    5/02/99 4:03p Agmsmith
 * Added a function for changing the duration of sequences.
 *
 * 41    4/23/99 10:58a Agmsmith
 * Working on getting interpolated data.
 *
 * 40    4/20/99 4:29p Agmsmith
 * Origin display under construction.
 *
 * 39    4/19/99 7:35p Agmsmith
 *
 * 38    4/19/99 12:40p Agmsmith
 *
 * 37    4/18/99 4:21p Agmsmith
 * Changed offset/scale/rotate to origin/scale/rotate/offset chunk.
 *
 * 36    4/17/99 3:31p Agmsmith
 *
 * 35    4/07/99 6:38p Agmsmith
 * Yet more chunk modifying routines.
 *
 * 34    4/05/99 5:44p Agmsmith
 * Added tweeker proportion to modifiable things.
 *
 * 33    4/04/99 3:13p Agmsmith
 * Array of sizes for private chunks too.
 *
 * 32    3/30/99 4:53p Agmsmith
 * Forgot modifiable includes mesh index.
 *
 * 31    3/30/99 3:33p Agmsmith
 * Fix bug in serial number allocation under Win95.
 *
 * 30    3/19/99 10:30a Agmsmith
 * Finished 3D Mesh chunk, and added Tweeker chunk.
 *
 * 29    3/18/99 10:57a Andrewx
 * Modified for HMD display
 *
 * 28    3/15/99 4:41p Agmsmith
 * More functions for finding subchunks for the dope sheet.
 *
 * 27    3/13/99 5:29p Agmsmith
 * Need a recursive flag when counting selected sequences.
 *
 * 26    3/10/99 7:15p Agmsmith
 * Added LE_CHUNK_SortChildSequencesByTime.
 *
 * 25    3/10/99 2:07p Agmsmith
 * More dope data flags.
 *
 * 24    3/09/99 7:03p Agmsmith
 * Count number of selected sequences.
 *
 * 23    3/06/99 3:15p Agmsmith
 * Added LE_CHUNK_DeleteSelected function.
 *
 * 22    3/05/99 12:46p Agmsmith
 * Added LE_CHUNK_ToggleByRange.
 *
 * 21    3/04/99 4:19p Agmsmith
 * Added LE_CHUNK_ForAllSubSequences function.
 *
 * 20    2/25/99 11:40a Agmsmith
 * Need 16 bits for finding priority range of a sequence.
 *
 * 19    2/14/99 2:18p Agmsmith
 * Add recursive updating of group chunk sizes to LE_CHUNK_SizeSequence.
 *
 * 18    2/13/99 12:19p Agmsmith
 * Added LE_CHUNK_FindSerialPointer.
 *
 * 17    2/12/99 7:04p Agmsmith
 * More serial number stuff.
 *
 * 16    2/10/99 4:06p Agmsmith
 * Deleting and inserting subchunks by serial number.
 *
 * 15    2/09/99 2:51p Agmsmith
 * Added LE_CHUNK_FindSerialOffset.
 *
 * 14    2/08/99 5:44p Agmsmith
 * Added serial number subchunk.
 *
 * 13    2/05/99 2:56p Agmsmith
 * Added function for finding the time and priority ranges of a sequence.
 *
 * 12    2/02/99 1:46p Agmsmith
 * Added CreateSequence function.
 *
 * 11    2/01/99 4:49p Agmsmith
 * More chunky options.
 *
 * 10    1/22/99 11:21a Agmsmith
 * Now can add file name subchunks to remember where data was loaded from
 * when loading using file names.
 *
 * 9     1/20/99 5:38p Agmsmith
 * Load chunky files using embedded file names to load subsiduary data.
 *
 * 8     1/13/99 6:49p Agmsmith
 * Added a subroutine for loading chunks from an ASCII file.
 *
 * 7     1/12/99 5:35p Agmsmith
 * Added more user names.
 *
 * 6     1/08/99 4:36p Agmsmith
 * DumpChunks under construction, compiles and prints an outline but no
 * chunk contents yet.
 *
 * 5     1/08/99 11:49a Agmsmith
 * Added functions for getting info about descended into levels (ChunkID,
 * size, offset).  Also rearranged error checking code to do less in
 * release mode.
 *
 * 4     1/07/99 6:32p Agmsmith
 * Added GrowChunk for inserting and deleting bytes inside nested chunks,
 * and even deleting whole chunks.
 *
 * 3     11/06/98 2:24p Agmsmith
 * Added the ability to write to DataIDs.
 ****************************************************************************/

/************************************************************/
/* DEFINES                                                  */
/*----------------------------------------------------------*/

/************************************************************/
/* MACROS                                                   */
/*----------------------------------------------------------*/

/************************************************************/
/* STRUCTURES, ENUMS, ETC.                                  */
/*----------------------------------------------------------*/


// The various different kinds of chunks and the chunk ID value used.

enum LE_CHUNK_ID_Enum
{
  LE_CHUNK_ID_NULL_STANDARD = 0,
    // This is the null chunk identifier.  No actual chunks use it,
    // but it can be returned from functions to signal an error.

  LE_CHUNK_ID_SEQ_GROUPING = 1,
  LE_CHUNK_ID_SEQ_INDIRECT,
  LE_CHUNK_ID_SEQ_2DBITMAP,
  LE_CHUNK_ID_SEQ_3DMODEL,
  LE_CHUNK_ID_SEQ_SOUND,
  LE_CHUNK_ID_SEQ_VIDEO,
  LE_CHUNK_ID_SEQ_CAMERA,
  LE_CHUNK_ID_SEQ_PRELOADER,
  LE_CHUNK_ID_SEQ_3DMESH,
  LE_CHUNK_ID_SEQ_TWEEKER,
  LE_CHUNK_ID_SEQ_MAX,
    // A variety of chunk types used for sequences.

  LE_CHUNK_ID_FILE_NAME_1 = 20,
  LE_CHUNK_ID_FILE_NAME_2,
  LE_CHUNK_ID_FILE_NAME_3,
  LE_CHUNK_ID_FILE_NAME_4,
  LE_CHUNK_ID_FILE_NAME_5,
    // Standard chunk to use for file names.  The contents are an ASCII string
    // of characters that gives a path name.  Or they could be Unicode too,
    // you can tell by examining the last two bytes - if both are zero then
    // it is Unicode, if only the last is zero then it is ASCII.  You can
    // have five different file names, useful for things that have several
    // different parts.

  LE_CHUNK_ID_USER_NAME_1 = 30,
  LE_CHUNK_ID_USER_NAME_2,
  LE_CHUNK_ID_USER_NAME_3,
  LE_CHUNK_ID_USER_NAME_4,
  LE_CHUNK_ID_USER_NAME_5,
    // Standard chunk to use for a user defined name.  This is a label
    // for a thing which is Human readable but isn't a file name or
    // other computer understandable thing.  Useful for things like
    // column names in the dope sheet or understandable names for a
    // sequence when file names are an encoded mess.

  LE_CHUNK_ID_DOPE_DATA = 40,
    // Inserted in chunky sequences by the dope sheet to carry extra
    // info about the sequence, like the serial number and selection
    // states.  Can be stripped out when just playing the sequence.

  LE_CHUNK_ID_ANNOTATION = 50,
    // This contains a string which is an embedded comment in the chunky
    // file, annotating it.  Usually only used when debugging to insert
    // readable markers or comments.  Also useful for comments about the
    // data, such as "Made by Dope99, the Artech dope sheet." or anything
    // else which doesn't fit elsewhere.  The file should still be useable
    // if the annotation is stripped out.

  LE_CHUNK_ID_AUTHOR,
    // Identifies the author of the data / music / video / etc.  If there
    // are several authors, insert several author chunks.

  // Insert more generic string type chunks in here.

  LE_CHUNK_ID_COPYRIGHT = 59,
    // The copyright notice string.  Last of the annotation style strings too.

  LE_CHUNK_ID_NULL_IN_CONTEXT = 128,
    // The null chunk ID for use in private chunks where the context
    // of the chunk (the parent chunk) defines what the values mean.
    // All chunk IDs larger than this (129 to 255) are for private use.

  LE_CHUNK_ID_MAX = 256
    // All valid chunk IDs are less than this value.
};
typedef UNS8 LE_CHUNK_ID;



/* This pragma makes fields adjacent in memory in the following structure. */
#pragma pack(push,1)
typedef struct LE_CHUNK_HeaderStruct
{
  unsigned int chunkSize : 24;
  unsigned int chunkID : 8;
} LE_CHUNK_HeaderRecord, *LE_CHUNK_HeaderPointer;
  // In a file the 4 byte chunk header is followed by (chunkSize - 4) bytes
  // of data.  The header size is included in the size.  The format of the
  // data varies depending on the chunkID, but typically it will include
  // some fixed length data followed by nested child chunks, with the whole
  // thing adding up to chunkSize bytes.  The child chunks end when you
  // reach the end of the parent chunk.
#pragma pack(pop)


typedef INT8 LE_CHUNK_StreamID;
  // Identifies a particular chunk stream.  Negative signals an error.
  // Each stream is associated with an open file or memory block.
  // Also, it keeps track of the position and a certain number of
  // levels of backtracking (parent chunks of current chunk) so that
  // it can descend into child chunks and ascend back to the parent.



typedef UNS32 LE_CHUNK_SerialNumber, *LE_CHUNK_SerialNumberPointer;
  // Used for identifying sequences in the dope sheet.  Not used
  // in games or their data files.  A value of zero means no
  // serial number.



#define LE_CHUNK_DopeDataFlagSelected         0x0001
  // This flag bit is turned on in sequences which have been selected
  // by the user.  They usually get drawn in highlighted mode.

#define LE_CHUNK_DopeDataFlagOverlapping      0x0002
  // This flag is used by the insert sequences function in the dope sheet
  // to mark ones which are overlapping the area where the new sequence
  // will go, and thus need to be moved.

#define LE_CHUNK_DopeDataFlagNeedsProcessing  0x0004
  // This flag is used by the various functions which modify a bunch of
  // sequences to keep track of which sequences still need to be done.
  // Needed because modifying the sequence can change the size and
  // position of the subchunks and thus require restarting the iteration
  // at the beginning.

typedef struct LE_CHUNK_DopeDataStruct
{
  LE_CHUNK_SerialNumber serialNumber;
  UNS32 flags;
} LE_CHUNK_DopeDataRecord, *LE_CHUNK_DopeDataPointer;
  // LE_CHUNK_ID_DOPE_DATA chunks contain this structure.  They are
  // used by the dope sheet to mark subchunks.  The serial number is unique
  // among all chunks loaded into memory, so that you can find a particular
  // subchunk no matter how it has been shifted around by deleting and
  // adding other subchunks.  The flags are used for identifying selected
  // items in the sequence, and other purposes.  The sequencer ignores
  // these chunks, so you can remove them in final data if you don't need
  // them (the dope sheet does remove ones for unselected items before
  // saving them).



/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

extern const UNS16 LE_CHUNK_ChunkFixedDataSizes [LE_CHUNK_ID_NULL_IN_CONTEXT + 1];
  // This array contains the size of the fixed size data in each of the
  // standard types of chunk.  Everything after the fixed size data is
  // subchunks.  A value of 0xFFFF means that there are no subchunks,
  // everything inside is raw data.  This is useful if you want to parse
  // a chunky file without knowing too much about the subchunks.  Note
  // that user defined in-context aren't in this array, since the
  // internal structure varies depending on the context.  The size doesn't
  // include the size of the chunk header, just the fixed data portion.

extern const char * const LE_CHUNK_NamesOfPublicChunkTypes [LE_CHUNK_ID_NULL_IN_CONTEXT + 1];
  // Readable names for all the chunk types.

#if CE_ARTLIB_EnableSystemSequencer
extern const UNS16 LE_CHUNK_SequencerPrivateChunkFixedDataSizes [];
  // A similar array for the sizes of the sequencer private chunks.
  // Subtract LE_CHUNK_ID_NULL_IN_CONTEXT from the ChunkID to get
  // the index into this array (or AND with 0x7F).
#endif



/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

extern void LI_CHUNK_InitSystem (void);
extern void LI_CHUNK_RemoveSystem (void);

// Functions for reading chunks from a file or from a data item.
// Only use these with read streams.

#if CE_ARTLIB_EnableSystemData
extern LE_CHUNK_StreamID  LE_CHUNK_ReadFromDataID (LE_DATA_DataId DataID);
#endif // CE_ARTLIB_EnableSystemData
extern LE_CHUNK_StreamID  LE_CHUNK_ReadFromFile (char *FileName);
extern BOOL               LE_CHUNK_CloseReadStream (LE_CHUNK_StreamID StreamID);

extern LE_CHUNK_ID        LE_CHUNK_Descend (LE_CHUNK_StreamID StreamID, LE_CHUNK_ID FindChunkID, UNS32 *ReturnedChunkDataSize);
extern BOOL               LE_CHUNK_Ascend (LE_CHUNK_StreamID StreamID);
extern UNS32              LE_CHUNK_GetCurrentFilePosition (LE_CHUNK_StreamID StreamID);
extern BOOL               LE_CHUNK_SetCurrentFilePosition (LE_CHUNK_StreamID StreamID, UNS32 Position);
extern UNS8               LE_CHUNK_GetCurrentLevel (LE_CHUNK_StreamID StreamID);
extern LE_CHUNK_ID        LE_CHUNK_GetIDForLevel (LE_CHUNK_StreamID StreamID, UNS8 Level);
extern UNS32              LE_CHUNK_GetDataSizeForLevel (LE_CHUNK_StreamID StreamID, UNS8 Level);
extern UNS32              LE_CHUNK_GetDataStartOffsetForLevel (LE_CHUNK_StreamID StreamID, UNS8 Level);
extern void *             LE_CHUNK_MapChunkData (LE_CHUNK_StreamID StreamID, UNS32 ReadAmount, UNS32 *AmountActuallyReadPntr);
extern UNS32              LE_CHUNK_ReadChunkData (LE_CHUNK_StreamID StreamID, void *Buffer, UNS32 ReadAmount);

// Functions for writing chunks to a file.  Note that you can't
// mix reading and writing to the same stream.  Only use these
// functions with write streams.

#if CE_ARTLIB_EnableChunkWriting
  extern LE_CHUNK_StreamID  LE_CHUNK_WriteToFile (char *FileName);
#if CE_ARTLIB_EnableSystemData
  extern LE_CHUNK_StreamID  LE_CHUNK_WriteToDataID (LE_DATA_DataId DataID);
#endif // CE_ARTLIB_EnableSystemData
  extern BOOL               LE_CHUNK_CloseWriteStream (LE_CHUNK_StreamID StreamID);

  extern BOOL               LE_CHUNK_StartChunk (LE_CHUNK_StreamID StreamID, LE_CHUNK_ID ChunkID);
  extern BOOL               LE_CHUNK_WriteChunkData (LE_CHUNK_StreamID StreamID, void *Buffer, UNS32 WriteAmount);
  extern BOOL               LE_CHUNK_WriteChunkACHARString (LE_CHUNK_StreamID StreamID, ACHAR *StringPntr);
  extern BOOL               LE_CHUNK_FinishChunk (LE_CHUNK_StreamID StreamID);
#endif


// Other chunk operations.

#if CE_ARTLIB_EnableChunkGrow
  extern BOOL LE_CHUNK_GrowChunk (LE_DATA_DataId DataID, UNS32 TargetChunkOffset,
    INT32 AmountToGrow, UNS32 InsertOffset = 0);
#endif // CE_ARTLIB_EnableChunkGrow



#if CE_ARTLIB_EnableChunkManipulation

  typedef enum LE_CHUNK_ModifiableTypesOfSequenceDataEnum
  {
    LE_CHUNK_MODIFIABLE_NOTHING = 0,
    LE_CHUNK_MODIFIABLE_ORIGIN,
    LE_CHUNK_MODIFIABLE_SCALING,
    LE_CHUNK_MODIFIABLE_ROTATION,
    LE_CHUNK_MODIFIABLE_TRANSLATION,
    LE_CHUNK_MODIFIABLE_MESH_INDEX,
    LE_CHUNK_MODIFIABLE_PITCH_HZ,
    LE_CHUNK_MODIFIABLE_PANNING,
    LE_CHUNK_MODIFIABLE_VOLUME,
    LE_CHUNK_MODIFIABLE_FOV,
    LE_CHUNK_MODIFIABLE_MAX
  } LE_CHUNK_ModifiableTypesOfSequenceData;

  extern LE_DATA_DataId LE_CHUNK_CreateSequence (int Dimensionality = -1);

  extern BOOL LE_CHUNK_SizeSequence (
    LE_DATA_DataId DataID, UNS32 SequenceChunkOffset, BOOL RecursiveUpdate,
    UNS16 *LowPriorityPntr, UNS16 *HighPriorityPntr,
    INT32 *LowTimePntr, INT32 *HighTimePntr);

  extern int LE_CHUNK_FindSerialOffset (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SerialNumber);

  extern void * LE_CHUNK_FindSerialPointer (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SerialNumber);

  extern int LE_CHUNK_FindSerialSubchunkOffset (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SerialNumber, LE_CHUNK_ID ChunkID);

  extern void * LE_CHUNK_FindSerialSubchunkPointer (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SerialNumber, LE_CHUNK_ID ChunkID);

  extern LE_CHUNK_SerialNumber LE_CHUNK_FindSerialNumber (
    LE_DATA_DataId DataID, UNS32 StartOffset);

  extern BOOL LE_CHUNK_DeleteBySerial (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SerialNumber);

  extern int LE_CHUNK_DeleteSelected (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SubSequenceSerialNumber);

  extern BOOL LE_CHUNK_InsertChildUnderParentSerial (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SerialNumber, LE_CHUNK_HeaderPointer NewSubchunkPntr);

  extern BOOL LE_CHUNK_SortChildSequencesByTime (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SerialNumber);

/* See L_Seqncr.h for the actual declarations, since these can't be declared
   here because the sequencer defines are needed.

typedef BOOL (*LE_CHUNK_ForAllCallbackFunctionPointer)
  (LE_CHUNK_StreamID StreamID,
  LE_CHUNK_ID ChunkID,
  LE_SEQNCR_SequenceChunkHeaderPointer SeqHeaderPntr,
  LE_CHUNK_DopeDataPointer DopeDataPntr,
  void *UserValue, INT32 AbsoluteStartTime);

extern BOOL LE_CHUNK_ForAllSubSequences (LE_DATA_DataId DataID,
  LE_CHUNK_SerialNumber SubSequenceSerialNumber, BOOL Recursive,
  UNS32 FlagsToMatch, BOOL RequireAllFlags,
  LE_CHUNK_ForAllCallbackFunctionPointer UserFunctionPntr, void *UserValue,
  INT32 BaseStartTime);

extern BOOL LE_CHUNK_ModifySequenceData (LE_DATA_DataId DataID,
  LE_CHUNK_SerialNumber SequenceSerialNumber,
  LE_SEQNCR_InterpolationTypes InterpolationType,
  INT32 TimeToModify,
  BOOL RelativeNotAbsolute,
  LE_CHUNK_ModifiableTypesOfSequenceData ThingToModify,
  float X, float Y, float Z);

*/

  extern BOOL LE_CHUNK_IsDataChunkModifableType (LE_CHUNK_ID ChunkID,
    LE_CHUNK_ModifiableTypesOfSequenceData ModificationType);

  extern LE_CHUNK_HeaderPointer LE_CHUNK_GetTweekerOrSequenceDataChunk (
    LPBYTE BasePntr, UNS32 SequenceChunkOffset,
    LE_CHUNK_ModifiableTypesOfSequenceData ThingToGet);

  extern BOOL LE_CHUNK_GetInterpolatedData (LE_DATA_DataId DataID,
    UNS32 SequenceChunkOffset, INT32 TimeToGetAt, BOOL UseTweekers,
    LE_CHUNK_ModifiableTypesOfSequenceData ThingToGet,
    LE_CHUNK_HeaderPointer OutputBuffer, BOOL *FromKeyFrame);

  extern BOOL LE_CHUNK_PromoteOrDemoteDataChunks (LE_DATA_DataId DataID,
    UNS32 OffsetToParentChunk,
    LE_CHUNK_ModifiableTypesOfSequenceData ThingToPromote,
    BOOL Promote);

  extern BOOL LE_CHUNK_DeleteTweeker (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SequenceSerialNumber,
    INT32 TimeToModify,
    LE_CHUNK_ModifiableTypesOfSequenceData ThingToModify);

  extern int LE_CHUNK_CountSelected (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SubSequenceSerialNumber, BOOL Recursive);

  extern LE_CHUNK_SerialNumber LE_CHUNK_FindOneSelectedSerial (
    LE_DATA_DataId DataID, LE_CHUNK_SerialNumber SubSequenceSerialNumber,
    BOOL Recursive);

  extern int LE_CHUNK_FindOneSelectedOffset (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SubSequenceSerialNumber, BOOL Recursive);

  extern BOOL LE_CHUNK_ClearAllSelections (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SubSequenceSerialNumber);

  extern BOOL LE_CHUNK_SelectByRange (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SubSequenceSerialNumber,
    TYPE_RectPointer PriorityAndTimeRange);

  extern BOOL LE_CHUNK_ToggleByRange (LE_DATA_DataId DataID,
    LE_CHUNK_SerialNumber SubSequenceSerialNumber,
    TYPE_RectPointer PriorityAndTimeRange);

  extern BOOL LE_CHUNK_CopySelectedFlagToNeedsProcessingFlag (
    LE_DATA_DataId DataID);

  extern int LE_CHUNK_FindDimensionality (LE_DATA_DataId DataID,
    UNS32 TargetChunkOffset);

  extern INT32 LE_CHUNK_GetParentOffset (LE_DATA_DataId DataID,
    UNS32 TargetChunkOffset);

  extern INT32 LE_CHUNK_ConvertAbsoluteTimeToSequenceTime (
    LE_DATA_DataId DataID, UNS32 SequenceChunkOffset, INT32 AbsoluteTime);

  extern INT32 LE_CHUNK_GetAbsoluteParentStartTime (LE_DATA_DataId DataID,
    UNS32 SequenceChunkOffset);

  extern BOOL LE_CHUNK_ChangeDuration (LE_DATA_DataId DataID, UNS32 Offset,
    float TimeFactor, BOOL MultiplyNotAdd, int RecursiveDepth);

#endif // CE_ARTLIB_EnableChunkManipulation



#if CE_ARTLIB_EnableChunkDumpLoad

  extern BOOL LE_CHUNK_DumpChunks (LE_CHUNK_StreamID StreamID, FILE *Outfile,
    BOOL PrintSymbolsForDataIDs);

  extern BOOL LE_CHUNK_LoadChunks (FILE *Infile, LE_CHUNK_StreamID StreamID);

#endif // CE_ARTLIB_EnableChunkDumpLoad



#if CE_ARTLIB_EnableChunkFileLoading

  extern LE_DATA_DataId LE_CHUNK_LoadUsingEmbeddedFileNames (
    const char *FileName,
    BOOL AddSerialNumberChunks,
    LE_DIRINI_PathListPointer *SearchPathPntrPntr);

  extern BOOL LE_CHUNK_AddFileNameChunks (
    LE_DATA_DataId MainDataID,
    BOOL AddSerialNumberChunks);

  extern LE_CHUNK_SerialNumber LE_CHUNK_GetNextSerialNumber (void);

#endif // CE_ARTLIB_EnableChunkFileLoading

#endif // __L_CHUNK_H__
