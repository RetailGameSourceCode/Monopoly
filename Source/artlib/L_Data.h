#ifndef __L_DATA_H__
#define __L_DATA_H__

/*****************************************************************************
 * FILE:        L_Data.h
 *
 * DESCRIPTION: Data items and code for loading and unloading them.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Data.h 19    5/08/99 5:18p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Data.h $
 * 
 * 19    5/08/99 5:18p Agmsmith
 * Use a search path when opening data files.
 * 
 * 18    4/25/99 11:31a Agmsmith
 * Added function for unloading all data.
 *
 * 17    3/19/99 10:44a Agmsmith
 *
 * 16    3/18/99 10:57a Andrewx
 * Modified for HMD display
 *
 * 15    1/16/99 3:20p Agmsmith
 * Added an optional hash table for converting from file names to DataIDs.
 *
 * 14    12/24/98 9:56a Agmsmith
 * If you specify a data type for an external file, use that rather than
 * guessing from the file name extension.
 *
 * 13    12/20/98 7:26p Agmsmith
 * Added user function for loading raw into a buffer.
 *
 * 12    12/16/98 5:26p Agmsmith
 * Comments updated.
 *
 * 11    12/10/98 12:26p Agmsmith
 * Added generic bitmap types, changed UAP to use TAB prefix for
 * compatibility with the old dope sheet.
 *
 * 10    12/09/98 5:12p Agmsmith
 * Cleaned up data types list.  Also added HTTP data source so that you
 * can now have DataIDs running from web server data.
 *
 * 9     12/02/98 4:09p Agmsmith
 * Needed some more brackets in the macros to avoid operator precedence
 * problems.
 *
 * 8     11/30/98 7:36p Agmsmith
 * Changes so that DMake99 can use the same files as the library.
 *
 * 7     11/04/98 4:47p Agmsmith
 * Now can resize data items.
 *
 * 6     11/04/98 1:10p Agmsmith
 * More meaningful data type names for TABs vs UAPs, also better parameter
 * order in memory DataID allocation.
 *
 * 5     11/02/98 5:26p Agmsmith
 * Added external file data source and related things (dirty flag).
 *
 * 4     10/29/98 6:36p Agmsmith
 * New improved memory and data system added.  Memory and data systems
 * have been separated.  Now have memory pools, and optional corruption
 * check.  The data system has data groups rather than files and
 * individual items can come from any data source. Also has a least
 * recently used data unloading system (rather than programmer set
 * priorities).  And it can all be turned off for AndrewX!
 *
 * 1     9/28/98 11:25a Agmsmith
 * Under construction.
 ****************************************************************************/



/*****************************************************************************
 * A 32 bit value identifying a data group item (bottom 16 bits) and which
 * data group it comes from (top 16 bits).
 */

typedef UNS32 LE_DATA_DataId;

typedef UNS16 LE_DATA_DataGroupIndex;
  // The high 16 bits are the group index, which is never zero since a
  // a DataId of zero is the NULL of DataIds.

#if CE_ARTLIB_DataMemorySource
  #define LE_DATA_MemoryGroupIndex ((LE_DATA_DataGroupIndex) CE_ARTLIB_DataMaxGroups)
#endif
  // Special group number for the runtime in-memory programmer data
  // items and other runtime items.  This group is automatically set
  // up by the data system, if the data memory source exists.

typedef UNS16 LE_DATA_DataItemIndex;
  // The low 16 bits are the index of the particular item in the group,
  // zero is allowed.

#define LE_DATA_IdFromTag(file, tag)   ((LE_DATA_DataId) (((file)<<16)|(tag)))
  // Macro used to combine a 16 bit data id tag and a 16 bit datafile
  // descriptor (now group) into a 32 bit LE_DATA_DataId value.

#define LE_DATA_TagFromId(id)          ((LE_DATA_DataItemIndex) (id))
  // Macro used to remove the 16 bit data id tag from
  // the low word of a 32 bit LE_DATA_DataId value.

#define LE_DATA_FileFromId(id)         ((LE_DATA_DataGroupIndex) ((id)>>16))
  // Macro used to remove the 16 bit datafile descriptor (now group) from
  // a 32 bit LE_DATA_DataId value.

#define LE_DATA_IdWithFileFromParent(id, parent) (((id) & 0x0000FFFF) | ((parent) & 0xFFFF0000))
  // Macro used to combine the tag of a child item with the data file
  // of the parent.  Typically used for sequences that refer to bitmaps
  // in the same datafile by only specifying the bitmap's tag.

#define LE_DATA_EmptyItem              (0L)
  // Signifies an empty data item.  There is never a data group number zero.
  // Used to be -1, but it is more efficient to use 0 since the machine
  // instructions can detect zero quite efficiently.

#define LED_IFT(file, tag)   LE_DATA_IdFromTag(file, tag)
#define LED_TFI(id)          LE_DATA_TagFromId(id)
#define LED_FFI(id)          LE_DATA_FileFromId(id)
#define LED_EI               LE_DATA_EmptyItem
  // Duplicates of the above macros with much shorter names.



/*****************************************************************************
 * The various different data types.
 */

typedef UNS16 LE_DATA_ReferenceCount;
  // A 16 bit value is needed, since preloading sequences adds a count for
  // each time a bitmap is present in the sequence, which can be quite a few
  // times for common bitmaps like the shadow under a character.


// These codes are used to define what type of data an item is. This
// can be used as a check to make sure a function receiving data is
// receiving a correct data type. This will also make the use of
// functions to preprocess particular data types possible.

enum LE_DATA_DataTypeEnum
{
    LE_DATA_DataUnknown = 0,  // Unknown data item.  The NULL of data types.
    LE_DATA_DataBMP,          // An image in BMP format, usually needs to be in 24 bits, usually for backgrounds.
    LE_DATA_DataDopeTAB,      // An 8 bit Transparent Alpha Channel bitmap in original hacked BMP format, for dope sheet use.
    LE_DATA_DataUAP,          // A TAB converted to binary format (see NEWBITMAPHEADER & Tab2Uap utility), used by games.
    LE_DATA_DataNative,       // Current screen depth runtime created bitmap (uses NEWBITMAPHEADER without palette).
    LE_DATA_DataGBMTexture,   // A BMP or UAP to be used as a texture, usually 8 bits deep, converted to Generic Bitmap on loading.
    LE_DATA_DataGBMPicture,   // A BMP to be used as a plain picture (non-texture) native depth generic bitmap.  Usually 24 bits deep.
    LE_DATA_DataGenericBitmap,// All generic bitmaps get converted to this DirectDraw surface wrapper type after loading, texture mode becomes an internal flag, depth too.
    LE_DATA_DataWave,         // Digital sound data in Microsoft RIFF/WAVE format.
    LE_DATA_DataString,       // String data, Unicode or ASCII (see CE_ARTLIB_UnicodeAlphabet).
    LE_DATA_DataUserCreated1, // User created data type, treated as raw binary data by the library.
    LE_DATA_DataIndexTable,   // Index table, lists other data items.
    LE_DATA_DataChunky,       // Chunky file format, 4 byte chunk header style.
    LE_DATA_DataTextureArray, // Array of data IDs for textures.
    LE_DATA_Data3DModel,      // Hierarchy of 3D meshes.
    LE_DATA_Data3DPose,       // Table of joint matrices for hierarchical model.
    LE_DATA_DataHMD,          // HMD 3D model + textures information, gets converted to LE_DATA_DataMESHX upon loading.
    LE_DATA_DataMESHX,        // MESHX model information (post processed HMD).
    LE_DATA_DataTypeMax       // Last one in the enum, used for array dimensioning.
};

#if CE_ARTLIB_UseEnumDataTypes
  typedef enum LE_DATA_DataTypeEnum LE_DATA_DataType;
#else
  typedef UNS8 LE_DATA_DataType;
#endif



// Current version of the data file that this program understands.

#define  DATAFILE_ARTECH_STRING "Artech"
#define  LI_DATA_DataFileVersion_Major          2
#define  LI_DATA_DataFileVersion_Minor          3
#define  LI_DATA_DataFileVersion_PatchMajor     1
#define  LI_DATA_DataFileVersion_PatchMinor     0

// Artech DMake99 datafile header structure.
// Immediately following the header in the datafile will be the datafile
// index which will be IndexSize * sizeof(LI_DATA_DatafileIndexRecord)
// in size.

typedef struct LI_DATA_DatafileHeaderStruct
{
  char ArtechString[8];
  // Hard code string for datafile identification, NUL padded.

  short VersionMajor;
  short VersionMinor;
  // Version number of datafile creation utility.

  short PatchMajor;
  short PatchMinor;
  // Patch version number this datafile is currently using.

  short Reserved1;
  short Reserved2;
  // Two more values added to datafile header for future use.

  long IndexSize;
  // Number of entries in datafile index.

  unsigned long CRC32;
  // CRC for the whole data file, from after this point to the end,
  // including the index stuff.
} LI_DATA_DatafileHeaderRecord, *LI_DATA_DatafileHeaderPointer;


// Datafile index structure, whole thing is 16 bytes long.
typedef struct LI_DATA_DatafileIndexStruct
{
  long Offset;
  /* Offset into datafile calculated from start of datafile. */

  long UCompSize;
  /* Uncompressed size of data. */

  union
  {
    LE_DATA_DataType DataType;
    UNS32 Force32BitsForThisField;
  };
  /* Used mainly to express data type. See datafile data type enums
  above.  32 bits though only first 8 are used and the rest are zero. */

  long CompSize;
  /* Compressed size of data. */
} LI_DATA_DatafileIndexRecord, *LI_DATA_DatafileIndexPointer;



// This record is used in a table that translates from user values into
// data IDs.  Useful for things like associating an international string
// DataID with a string number.  Then switching the table lets you use
// the same code in a different language.
// Used by the LE_DATA_GetIndexedItemId function, the index table is in
// packed form in the data file (no structure pad bytes).

/* This pragma makes fields adjacent in memory in the following structure. */
#pragma pack(push,1)
typedef struct LE_DATA_DataIndexStruct
{
  UNS32 indexValue;
  UNS16 dataID;
} LE_DATA_DataIndexEntry;
#pragma pack(pop)



/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

extern char * LE_DATA_DataTypeNames [LE_DATA_DataTypeMax];
  // Names of the data types, like "LE_DATA_DataString".

extern char * LE_DATA_DataTypePrefixes [LE_DATA_DataTypeMax];
  // Three letter prefixes for #defines of data types, like "STR".

extern UNS32 LE_DATA_DataPoolMemoryLimit;
  // The data system tries to keep the data pool's memory usage to under
  // this number of bytes.  Initialised to CE_ARTLIB_MemoryDataPoolMaxSize.
  // You can change it if you want to, possibly to the real memory size
  // of the computer minus some overhead.



/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

extern void LI_DATA_InitSystem (void);
extern void LI_DATA_RemoveSystem (void);
// The usual suspects.  Initialises the runtime items data group, but
// leaves the others empty.  Removal unloads and closes everything.

extern BOOL LE_DATA_InitDatafile (char *DataFileName,
  LE_DATA_DataGroupIndex DataGroupIndex, UNS16 ExtraItems = 0
#if CE_ARTLIB_EnableSystemDirIni
  , LE_DIRINI_PathListPointer UserSearchPaths = NULL
#endif
  );
// Opens and initialises a traditional datafile for use by code, creating
// and filling the given data group with the items in the data file
// (makes the items point to the appropriate places in the data file
// but doesn't actually load any data).  If you want to have extra spare
// items after the ones from the file, you can specify the optional
// ExtraItems argument and use the LE_DATA_AllocMemoryDataID and other
// functions to set up the spare items.  This is useful for using an
// old data file and adding on some more items, which can later be
// connected to a new data file (useful for patching CD-ROM data files).
// It will search the user paths then the global path list in an attempt
// to find the file.  Remember you can use CD: for the CD-ROM drive.
// Returns TRUE if successful.  Use LE_DATA_RemoveDataGroup to close it.

extern BOOL LE_DATA_InitialiseDataGroup (LE_DATA_DataGroupIndex GroupIndex,
  int DesiredNumberOfItems);
// Creates a new data group with the given number of items.  The items
// are all in an initial unconnected state (no data associated with them).
// Returns TRUE if successful.

extern BOOL LE_DATA_RemoveDataGroup (LE_DATA_DataGroupIndex GroupIndex);
// Remove a whole data group (datafile) from use.  Any data items in
// memory that belong to a datafile or other external data source are
// unloaded (possibly writing changed data to disk) and the group is
// deallocated too.

extern BOOL LE_DATA_Load (LE_DATA_DataId DataID);
// Loads the given data item if necessary.  Returns TRUE if it is now in
// memory (either previously there or freshly loaded) or FALSE if it
// failed to load.  Use LE_DATA_GetPointer to get a pointer to it.
// Or if you want to do both at once, use LE_DATA_Use.

extern UNS32 LE_DATA_LoadRawIntoBuffer (LE_DATA_DataId DataID,
  void *MemoryPntr, UNS32 MemorySize, UNS32 StartOffset);
// Loads the raw data corresponding to the given data item into the
// buffer, no post-load processing is done and the data item itself
// is untouched (remaining unloaded or loaded).  Just loads the data.
// Returns the number of bytes loaded, zero for errors.

extern void *LE_DATA_GetPointer (LE_DATA_DataId DataID);
// Returns a pointer to the data for the given DataID, or NULL if the data
// isn't loaded (does not do loading).  You can also use this as a way of
// telling if an item is loaded or not.

extern LE_DATA_DataType LE_DATA_GetCurrentDataType (LE_DATA_DataId DataID);
// Returns the current data type of the given DataID.  This is a fast
// call (doesn't load the item) but can give you different answers
// depending on if the item is loaded or not.  Before an item is loaded,
// it is the raw initial data type (the kind in the data file).  After
// being loaded, it is the type set by the post-load processing, which
// can be different (for example LE_DATA_DataTAB could be upgraded into
// LE_DATA_DataGenericBitmap).  If it gets unloaded, the type doesn't
// get reset, and stays the way it was.  Reloading should hopefully
// set it to the same post-processed value as it was after the very
// first loading.  Returns LE_DATA_DataUnknown if an error happens.

extern LE_DATA_DataType LE_DATA_GetInitialDataType (LE_DATA_DataId DataID);
// Returns the data type of the given DataID as it is before pre-load
// processing.  This is the type specified in the data file.

extern LE_DATA_DataType LE_DATA_GetLoadedDataType (LE_DATA_DataId DataID);
// Makes sure the item is loaded and then returns the data type.  That
// way you always get the type corresponding to the in-memory data,
// after post-load processing.

extern UNS32 LE_DATA_GetCurrentSize (LE_DATA_DataId DataID);
// Returns the current size in bytes of the given DataID.  Initially it
// returns the unloaded size of the item (the size in the data file).
// Once the item has been loaded, it then returns the post-load processed
// size.  If the item is unloaded, it still returns the post-load processed
// size.  If the item is reloaded, the size is updated, but it will usually
// be the same post-load processed size.  Returns zero if an error happens.

extern UNS32 LE_DATA_GetInitialSize (LE_DATA_DataId DataID);
// Returns the initial size in bytes of the given DataID.  This corresponds
// to the size of the raw data in the data file, before any post-load
// processing gets done.

extern UNS32 LE_DATA_GetLoadedSize (LE_DATA_DataId DataID);
// Loads the item if needed and returns the size (includes changes made by
// the post-load processing to the size).

extern BOOL LE_DATA_Resize (LE_DATA_DataId DataID, UNS32 NewSize);
// Resizes the data used by the item to the new size, which can be bigger
// or smaller.  If the item is loaded, the allocated memory is also
// reallocated to the new size.  If the item is in an unloaded state,
// this doesn't do much since the size gets changed to the actual amount
// of data in the data source when it is loaded.  Does not change the
// dirty flag.

extern void *LE_DATA_Use (LE_DATA_DataId DataID);
// Loads the given data item if necessary, or uses the in-memory copy, and
// returns a pointer to its data.  Returns NULL if it fails.  The pointer
// stays valid until the data gets unloaded (which could happen soon unless
// you add a reference to the data item while you are using it, or you have
// turned off automatic unloading).

extern void *LE_DATA_UseDirty (LE_DATA_DataId DataID);
// Like LE_DATA_Use except that it also marks the item as dirty - meaning
// that it will get written to disk when it is unloaded.

extern void *LE_DATA_UseRef (LE_DATA_DataId DataID);
// Like LE_DATA_Use except it automatically adds 1 to the item's reference
// count before returning the pointer to you.

extern BOOL LE_DATA_Dirty (LE_DATA_DataId DataID, BOOL Dirty = TRUE);
// Mark the given DataID as having dirty bits.  That means it will get
// written back out to disk when it is unloaded (if the data source
// supports that - external files are currently the only ones which
// do that).  The Dirty flag is TRUE to mark it as dirty, FALSE to
// mark it as clean (no save when unloading if it is clean).

extern BOOL LE_DATA_Unload (LE_DATA_DataId DataID);
// Removes the given item from memory, if possible.  Call this when you
// know you won't be using the data for a while.  Returns FALSE if it
// fails, usually because you have a non-zero reference counter on the data.
// Note that the system can automatically unload data when it needs to,
// if the reference count is zero.

extern BOOL LE_DATA_UnloadRef (LE_DATA_DataId DataID);
// Like LE_DATA_Unload except that it decrements the reference count
// before trying to unload it.

extern BOOL LE_DATA_UnloadEverything (BOOL UnloadByUser = TRUE);
// Tries to unload all things which are loaded and have a zero reference
// count.  Useful for switching video modes, where each bitmap gets
// converted to the current screen depth as part of loading, so you need
// to unload them all during switching.  Returns TRUE if everything which
// should have unloaded did unload.

extern LE_DATA_ReferenceCount LE_DATA_AddRef (LE_DATA_DataId DataID);
// Increments the reference count for the given item, which means loading it
// into memory if it isn't already there.  Returns the new count, or zero
// if it failed (unable to load or find the item).

extern LE_DATA_ReferenceCount LE_DATA_RemoveRef (LE_DATA_DataId DataID);
// Decrements the reference counter by 1, usually meaning that you have
// finished using DataID.  Returns the new count.  If the count is zero,
// then the system is allowed to unload the item if it needs the space.

extern LE_DATA_ReferenceCount LE_DATA_GetRef (LE_DATA_DataId DataID);
// Returns the current reference count for the DataID, or zero if it
// fails (but the count could legitimately be zero too).

#if CE_ARTLIB_DataMemorySource
extern LE_DATA_DataId LE_DATA_AllocMemoryDataID (
  UNS32 NumberOfBytes, LE_DATA_DataType DataType = LE_DATA_DataUserCreated1,
  LE_DATA_DataGroupIndex GroupIndex = LE_DATA_MemoryGroupIndex);
// Creates a runtime data item, using the data memory pool for allocation,
// and adds the DataID to the given data group (defaults to the runtime
// items group).  Automatically adds a reference to it so that it doesn't
// get unloaded (runtime memory items get deallocated when they are
// unloaded, and refilled with garbage when they are loaded - so the
// freshly created one will be full of garbage).  When you are finished
// with it, use LE_DATA_FreeRuntimeDataID to deallocate it.
#endif

#if CE_ARTLIB_DataExternalFileSource
extern LE_DATA_DataId LE_DATA_AllocExternalFileDataID (
char                   *ExternalFileName,
LE_DATA_DataType        NewFileType = LE_DATA_DataUnknown,
UNS32                   NewFileSize = 0,
LE_DATA_DataGroupIndex  GroupIndex = LE_DATA_MemoryGroupIndex);
// Creates a runtime data item, using the external file data source,
// with the given file name.  If NewFileSize is zero then it will
// use an existing file, otherwise it will create a new file with
// the given size and type.  If NewFileType is LE_DATA_DataUnknown
// then the file name will be used to guess the type of the existing
// file.  Does not add a reference to it.  When you are finished with
// it, use LE_DATA_FreeMemoryDataID to deallocate it.
#endif

#if CE_ARTLIB_DataURLSource
extern LE_DATA_DataId LE_DATA_AllocURLDataID (
char                   *URLName,
LE_DATA_DataType        KnownType = LE_DATA_DataUnknown,
UNS32                   KnownSize = 0,
LE_DATA_DataGroupIndex  GroupIndex = LE_DATA_MemoryGroupIndex);
// Creates a runtime data item, using the URL (uniform resource
// locator) data source, with the given URL identifying a web
// page or image to retrieve ("http://www.Artech.ca:80/index.html"
// is an example, though the :80 can be left out).  Does not add a
// reference to it.  When you call this function, it contacts
// the web site to find the size and data type if you didn't
// specify them (which can take time).  When you later use the
// item, it contacts the site again to do the loading.  When you
// are finished with it, use LE_DATA_FreeMemoryDataID to deallocate
// it.
#endif

extern BOOL LE_DATA_FreeRuntimeDataID (LE_DATA_DataId DataID);
// Decrements the reference count for memory items only, unloads the data,
// and deallocates the runtime data item.  Returns TRUE if successful
// (can fail if you have added other reference counts to it).  Really
// only useful for runtime created DataIDs, if you use it on a datafile
// based DataID then it will wipe it out (that particular ID won't exist
// any more and can't be loaded).

extern UNS32 LE_DATA_DescribeDataID (LE_DATA_DataId DataID,
  char *Buffer, UNS32 BufferSize);
// Usually used for debug messages, this function creates a readable string
// describing a DataID, including things like the name of the data file it
// came from and what type of data it is.  Returns the number of bytes written
// to your buffer, including the NUL at the end of the string.  If it is
// equal to BufferSize then your buffer is probably too small.

extern char *LE_DATA_DescString (LE_DATA_DataId DataID,
  char *Buffer, UNS32 BufferSize);
// A convenience function for printing DataIDs in printf statements,
// since it returns the string pointer after filling it with the
// description, otherwise it is the same as LE_DATA_DescribeDataID.

extern LE_DATA_DataId LE_DATA_GetIndexedItemId (LE_DATA_DataId indexFile, unsigned long index);
// - Return data ID for an item inside a user created index table.  Returns
//   LE_DATA_EmptyItem if the item isn't in the table.
// - The parameter indexFile contains the identifier for the index table
//   as well as the data file identifier. ( using LED_IFT macro ).
// - The index parameter is the index entry into the index table.
//   It is crossed referenced and the data ID is returned.
// - It is assumed that the index table and associated items are located
//   in the same data file.

#if CE_ARTLIB_DataNameHashTable
extern const char *LI_DATA_GetDataIDName (LE_DATA_DataId DataID);
// Returns the file name or URL of a DataID, or NULL if it doesn't
// have a name.

extern LE_DATA_DataId LE_DATA_LookUpFileNameInIndex (const char *FileName);
// Looks in the hash table of known file names and tries to find the DataID
// which matches it.  Returns LE_DATA_EmptyItem if it doesn't find it.
// This is useful for utilities which have data which refers to other
// files by name (like sequences refering to bitmaps used).

extern BOOL LE_DATA_AddNamedDataIDToIndex (LE_DATA_DataId DataID);
// Adds the given DataID with a file name (external file or URL based)
// to the hash table using its name as the key to index it.  Returns FALSE
// if out of memory or otherwise failed (duplicate name, DataID unnamed, etc).

extern BOOL LE_DATA_RemoveNameFromIndex (const char *FileName);
// Takes the given entry out of the hash table.  Returns TRUE if successful.
// This is also automatically called by LE_DATA_FreeRuntimeDataID or when
// the data item otherwise gets deallocated (not merely unloaded).
#endif // CE_ARTLIB_DataNameHashTable

#endif // __L_DATA_H__
