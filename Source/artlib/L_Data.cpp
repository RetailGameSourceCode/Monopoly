/*****************************************************************************
 * FILE:        L_Data.cpp
 * DESCRIPTION: Data items and code for loading and unloading them.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Data.cpp 62    7/28/99 6:33p Agmsmith $
 *
 * Organisation of this file - generally things are declared and implemented
 * from big (top level) to small.  So you will see data groups, then data
 * sources and finally data items.
 *
 * The Basics - Each DataID contains a data group number in the high word and
 * an index number in the low word.  Each data group is an array of data
 * items, the low index specifies which one to use.  The item contains
 * information on how to load the data for that item, and when loaded it has
 * a pointer to the data in memory - which is what your program uses.
 *
 * Data Sources - The item's info includes a data source, which is usually
 * a data file, but it can be user memory (for runtime data items), an
 * external file, or even an Internet URL for really external files.  The
 * sources are implemented as classes inheriting from an abstract class which
 * has the basics for raw loading and unloading.  Many items will usually
 * share the same source (data file) but items in a group can have different
 * sources, which is useful for patching a data group in a later game
 * version (use original file on CD for most items, but patch file data
 * source for selected items).  Some data sources also support writing back
 * their items to external files when they are closed - useful for the dope
 * sheet.
 *
 * Data Items - These contain the basic info about a particular piece of
 * data.  Things like what type it is (a bitmap, sound, etc), size, extra
 * info on how to get it from the source (file offset or external file name),
 * and some data management things.
 *
 * Data Discarding - The management things in a data item include a least
 * recently used link list pointer pair and a reference counter.  When the
 * data system is low on memory, it will look for an item to discard.  It
 * starts with the least recently used item - the one at the tail of the
 * least recently used linked list, and unloads it.  If there still isn't
 * enough space, it keeps on unloading items.  Items which have a reference
 * count greater than zero are in use and won't be unloaded (and aren't in
 * the list).  Items that are unloaded (memoryPntr is NULL) are also not in
 * the list.  The rest of the library code can use the reference count to
 * mark items which are currently being played.  Also, whenever an item gets
 * accessed, it gets moved to the top of the least recently used list (maybe
 * it should be called a most recently used list?).  You can turn off this
 * extra overhead by setting CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded to zero.
 *
 * Critical sections are available in each data source and there is a global
 * one for the Least Recently Used list and another for data groups.  The LRU
 * one must be used quickly since all the other data operations use it (don't
 * want to stop the user when he's getting an item which is already loaded
 * while another thread is busy loading something from disk).  The data source
 * ones ensure that the data source only loads one item at a time.  The group
 * one guards the creation and destruction of data items (useful for runtime
 * DataID creation).  You can turn off the critical section overhead by
 * setting CE_ARTLIB_EnableMultitasking to zero.
 *
 * Error handling philosophy - checks everything in debug mode, assumes that
 * game logic is correct in release mode and doesn't check much other than
 * I/O errors and out of memory errors (this assumes that your data files
 * with items that reference other items by DataID are also correct).  Most
 * errors will cause a failure error code to be returned where possible,
 * rather than halting the program (and also show a message in debug mode).
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Data.cpp $
 * 
 * 62    7/28/99 6:33p Agmsmith
 * Fixed memory allocation size bug with new mesh allocation.
 *
 * 61    7/28/99 10:59a Kens
 * fixed bug where fully transparent TAB was being treated like a regular
 * BMP file: KS
 *
 * 61    7/28/99 10:45 KenS
 * made TAB2UAP check for 0 bitmap offsets in BITMAPFILEHEADER before
 * classifying input file as regular BMP
 *
 * 60    7/27/99 15:26 Davew
 * Changed things to use the new Frame/Mesh classes by default.  The old
 * frame/mesh classes can still be used by defining
 * CE_ARTLIB_3DUsingOldFrame to be 1 in C_ArtLib.h
 *
 * 59    7/19/99 11:00a Agmsmith
 * Found memory allocation bug which causes tickle errors.
 *
 * 58    7/06/99 12:41p Agmsmith
 * Need to check for changed size earlier in the loading process, before
 * memory gets allocated.
 *
 * 57    7/06/99 11:52a Agmsmith
 * Check for changes in file size when loading data from an external file,
 * so that you can edit bitmaps outside the dope sheet and have the
 * changes reflected.
 *
 * 56    7/05/99 11:45a Agmsmith
 * Oops, item name only available in dope sheet version, disable display
 * in debug message if the name table doesn't exist.
 *
 * 55    7/02/99 5:26p Agmsmith
 * Fixed up some error message handling.
 *
 * 54    6/25/99 11:55 Davew
 * Fixed the PC3D dependencies
 *
 * 53    6/22/99 12:49 Davew
 * Dependency changes from pc3d
 *
 * 52    6/09/99 6:35p Agmsmith
 * Find the data type for external files by looking at the extensions
 * array.
 *
 * 51    6/02/99 10:24a Agmsmith
 * Default to discrete alpha mode, for backwards compatability.
 *
 * 50    5/08/99 5:18p Agmsmith
 * Use a search path when opening data files.
 *
 * 49    4/25/99 11:31a Agmsmith
 * Added function for unloading all data.
 *
 * 48    4/23/99 11:21a Agmsmith
 * Removed LI_DATA_PostLoadProcessGenericBitmap since post load processing
 * is no longer done for runtime created items (were problems with
 * processing acting on garbage memory).
 *
 * 47    4/22/99 15:12 Davew
 * Removed an old include
 *
 * 46    4/22/99 2:31p Agmsmith
 * Don't do post-load processing for memory data items since garbage was
 * "loaded".
 *
 * 45    4/16/99 11:14a Agmsmith
 * Better debug messages.
 *
 * 44    4/14/99 5:39p Agmsmith
 * Add memory usage statistics.
 *
 * 43    4/12/99 3:41p Agmsmith
 * Use GetLoadedDataType instead of current, unless you know what you are
 * doing.
 *
 * 42    4/11/99 3:47p Agmsmith
 * Now postload converts BMPs into native screen depth bitmaps, rather
 * than having the sequencer do it.  So remove all BMP things everywhere.
 *
 * 41    3/19/99 11:06a Agmsmith
 *
 * 40    3/19/99 11:02a Agmsmith
 * Cleanup for MeshX done.
 *
 * 39    3/18/99 10:57a Andrewx
 * Modified for HMD display
 *
 * 38    2/03/99 7:30p Agmsmith
 * Need to load before you can become dirty.
 *
 * 37    1/30/99 1:15p Agmsmith
 * Use screen pixel format for Magenta in TAB loader.
 *
 * 36    1/30/99 1:00p Agmsmith
 * Added discrete alpha level option in TAB2UAP.
 *
 * 35    1/30/99 12:32p Agmsmith
 * Changes for being used by the TAB2UAP utility.
 *
 * 34    1/29/99 4:51p Agmsmith
 * Fix bug in TAB to UAP conversion.
 *
 * 33    1/27/99 7:35p Agmsmith
 * Optionally prompt the user when saving files overwrites the file.
 *
 * 32    1/27/99 3:21p Agmsmith
 * When unloading an external file, update the initial size and type with
 * the values describing what was written.
 *
 * 31    1/26/99 12:37p Agmsmith
 * Add some warning messages and only enable the DopeTab converter if the
 * user wants it.
 *
 * 30    1/26/99 11:46a Agmsmith
 * Get TAB colour conversion working and flip image.
 *
 * 29    1/25/99 9:17p Agmsmith
 * Loading TABs directly still under construction - colour conversion not
 * working and upside down.
 *
 * 28    1/22/99 3:29p Agmsmith
 * Prompt for permission when writing an external dataID.
 *
 * 27    1/22/99 11:21a Agmsmith
 * Allow EmptyID when requesting names of items.
 *
 * 26    1/21/99 1:52p Agmsmith
 * Log names of URLs and external files accessed.
 *
 * 25    1/18/99 4:04p Agmsmith
 * Cosmetic.
 *
 * 24    1/17/99 5:26p Agmsmith
 * Cosmetic changes.
 *
 * 23    1/17/99 3:22p Agmsmith
 * Use MAX_PATH rather than _MAX_PATH, they're the same.
 *
 * 22    1/16/99 3:20p Agmsmith
 * Added an optional hash table for converting from file names to DataIDs.
 *
 * 21    1/07/99 6:31p Agmsmith
 * Fixed bug in resizing and in external file creation.
 *
 * 20    12/26/98 4:46p Agmsmith
 *
 * 19    12/24/98 9:56a Agmsmith
 * If you specify a data type for an external file, use that rather than
 * guessing from the file name extension.
 *
 * 18    12/23/98 4:25p Agmsmith
 * Add post-load processing for generic bitmap pictures and textures.
 *
 * 17    12/20/98 7:26p Agmsmith
 * Added user function for loading raw into a buffer.
 *
 * 16    12/18/98 5:37p Agmsmith
 * Add L_BLT in front of global variables, dwRedShift is just too
 * anonymous.
 *
 * 15    12/17/98 10:23a Agmsmith
 * Generic bitmap memory now zeroed by post-loading.
 *
 * 14    12/16/98 5:26p Agmsmith
 * Added PreUnload processing for Generic Bitmaps.
 *
 * 13    12/10/98 12:26p Agmsmith
 * Added generic bitmap types, changed UAP to use TAB prefix for
 * compatibility with the old dope sheet.
 *
 * 12    12/09/98 5:15p Agmsmith
 * Cleaned up data types list.  Also added HTTP data source so that you
 * can now have DataIDs running from web server data.
 *
 * 11    11/30/98 7:36p Agmsmith
 * Changes so that DMake99 can use the same files as the library.
 *
 * 10    11/30/98 5:56p Agmsmith
 * Use ZLib file compression.
 *
 * 9     11/06/98 3:21p Agmsmith
 * Add memory tickling to avoid swap file bug.
 *
 * 8     11/04/98 4:47p Agmsmith
 * Now can resize data items.
 *
 * 7     11/04/98 1:13p Agmsmith
 * More meaningful data type names for TABs vs UAPs, also better parameter
 * order in memory DataID allocation.
 *
 * 6     11/02/98 5:26p Agmsmith
 * Added external file data source and related things (dirty flag).
 *
 * 5     10/30/98 12:37p Agmsmith
 * Disable data source code for unused data sources.
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

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemData


#if CE_ARTLIB_EnableSystemGrafix3D

#if CE_ARTLIB_3DUsingOldFrame
#define USE_OLD_FRAME
#endif CE_ARTLIB_3DUsingOldFrame

#include "../pc3d/PC3DHdr.h"
#include "../pc3d/camera.h"
#include "../pc3d/scene.h"

#if CE_ARTLIB_3DUsingOldFrame

#define GsOT int
#include "../pc3d/hmdstuff.h"
#include "../pc3d/sys\types.h"
#include "../pc3d/libgte.h"
#include "../pc3d/libgpu.h"
#include "../pc3d/libgs.h"
#include "../pc3d/libhmd.h"
#include "../pc3d/hmdload.h"

#else // CE_ARTLIB_3DUsingOldFrame

#include "../pc3d/NewMesh.h"

#endif // CE_ARTLIB_3DUsingOldFrame

#endif // CE_ARTLIB_EnableSystemGrafix3D


#include "../zlib/zimplode.h"



/****************************************************************************/
/* F O R W A R D   R E F E R E N C E S   O F   A L L   K I N D S            */
/*--------------------------------------------------------------------------*/

typedef struct LI_DATA_ItemStruct LI_DATA_ItemRecord, *LI_DATA_ItemPointer;
  // The actual item structure is furthur down in this file, this is a
  // forward reference so that data sources can refer to items and
  // the other way around too.


#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
BOOL LI_DATA_UnloadForFreeSpace (UNS32 DesiredFreeSpace);
  // Forward reference to a library internal function.
#endif



/****************************************************************************/
/* P R I V A T E   T Y P E S   A N D   M A C R O S                          */
/*--------------------------------------------------------------------------*/

// This is for holding info about a single data group.  Basically a group
// is just an array of items.  Normally all the items in a group will
// come from the same source (a data file), but that isn't necessary.
// The extra info keeps track of the item array used by the group, since
// it is allocated at runtime to hold enough items for the data file
// or other data source.

typedef struct LI_DATA_GroupStruct
{
  LI_DATA_ItemPointer dataItemArray;
    // Points to the first element in the array.  NULL if the array hasn't
    // been allocated yet.  It will be allocated from the library pool.

  LI_DATA_ItemPointer firstFreeItem;
    // Points to the start of a singly linked list of free items.  The nextLRU
    // field is used to link them together (obviously a free item can't be in
    // the real Least Recently Used List).  NULL if there are no more free
    // items.

  UNS32 numberOfDataItems;
    // Number of items in the array, used for range checking on DataIDs,
    // and for iterating through all items in the group when closing the
    // group.  Should be at most 64K since we have a 16 bit item index.
} LI_DATA_GroupRecord, *LI_DATA_GroupPointer;



typedef class LI_DATA_GenericDataSourceClass
{
public:
  LI_DATA_GenericDataSourceClass ();
    // Initialise the abstract class (clear count, set up critical section).

  ~LI_DATA_GenericDataSourceClass ();
    // Destructor for the abstract class (deallocate critical section).

  UNS32 numberOfItems;
    // Number of data items using this data source.  When it falls to zero,
    // this data source should close itself.

  char *sourceName;
    // A readable string for this data source.  Used in fatal error messages
    // in release mode as well as during debugging.

#if CE_ARTLIB_EnableMultitasking
  CRITICAL_SECTION   dataSourceCriticalSection;
  LPCRITICAL_SECTION L_TYPE_volatile dataSourceCriticalSectionPntr;
  // The critical section guarding the data source so that two separate
  // threads loading data don't try to read files at the same time, which
  // could cause crashes, or at least bad performance.  Also guards the
  // number of items and other critical variables.
  // dataSourceCriticalSectionPntr points at dataSourceCriticalSection if it has been
  // initialised (part of the constructor), NULL otherwise.
#endif // CE_ARTLIB_EnableMultitasking

  // Each source has its own variation of the initialise function for items,
  // which always locks the data source's critical section while changing the
  // number of items count.  Typically it will look like this:
  // BOOL connectItem (LI_DATA_ItemPointer ItemPntr, source specific arguments...)

  virtual BOOL disconnectItem (LI_DATA_ItemPointer ItemPntr) = 0;
    // This disconnects the data source for that item, unloading will
    // already have been done before this.  Essentially it just sets
    // the item's dataSource pointer to NULL and decrements the
    // numberOfItems, and if it is zero then the source may self destruct
    // too (close the datafile if there is one etc).  Some sources
    // don't self destruct (like the runtime memory one), typically
    // they are allocated as global variables rather than using new
    // and delete.  Always locks the data source while changing the
    // count.

  virtual UNS32 loadDataForItem (LI_DATA_ItemPointer ItemPntr,
  void *MemoryPntr, UNS32 MemorySize, UNS32 StartOffset) = 0;
    // Fill the buffer with data for the item.  Returns the number of bytes
    // loaded, or zero if it fails.  Will start loading at the byte offset
    // specified by StartOffset, so that you can skip over headers and
    // load pure pixels directly into a bitmap.  Note that it doesn't set
    // the item's memoryPntr in the item - so that you can get a fresh copy
    // of the item's data into some other buffer if you want to.  Someone
    // else will do the data type specific post-load processing, possibly
    // reallocating memory if the processing grows or shrinks the data.
    // Presumably the item is already connected to this data source.
    // May lock the data source's critical section if the source doesn't
    // multitask well.

  virtual BOOL unloadDataForItem (LI_DATA_ItemPointer ItemPntr,
  void *MemoryPntr, UNS32 MemorySize) = 0;
    // Unload the data.  For datafiles this is nothing, but for external
    // files it could mean writing the data back out to the file.  Will
    // write MemorySize bytes from the MemoryPntr buffer.  Presumably the
    // data will already have had the pre-unload processing done for the
    // data type before this is called.  Presumably the item is already
    // connected to this data source.  May lock the data source's critical
    // section if the source doesn't multitask well.

  virtual void describeSourceForItem (LI_DATA_ItemPointer ItemPntr,
  char *OutputString);
    // Writes a readable description of the data source for the given item
    // to the string.  Should take less than 1024 characters.  It will
    // usually be appended to a string like "Data from " when printed.
    // Presumably the item is already connected to this data source.
    // The default just uses the data source name from sourceName.
    // May lock the data source's critical section if the source
    // doesn't multitask well.

#if CE_ARTLIB_DataNameHashTable
  virtual const char *getItemNameFromItem (LI_DATA_ItemPointer ItemPntr);
    // Returns the name of this item, for items with names (like external
    // file or URL based data items).  Returns NULL for items with no
    // unique name.  Ideally should be the full path name for the item
    // so that items with the same name in different directories are
    // distinguishable.
#endif // CE_ARTLIB_DataNameHashTable

} *LI_DATA_GenericDataSourceClassPointer;



#if CE_ARTLIB_DataMemorySource
// The data source for in-memory runtime files, not much of a source since
// it doesn't do anything.  Usually there is only one global instance of
// this data source, staticly allocated.

typedef class LI_DATA_MemorySourceClass : public LI_DATA_GenericDataSourceClass
{
public:
  LI_DATA_MemorySourceClass ();

  BOOL connectItem (LI_DATA_ItemPointer ItemPntr, UNS32 ByteSize,
    LE_DATA_DataType DataType);
  // Sets up a new data item to be a runtime memory data item.

  // Real implementations for most virtual functions....

  BOOL disconnectItem (LI_DATA_ItemPointer ItemPntr);
  UNS32 loadDataForItem (LI_DATA_ItemPointer ItemPntr,
    void *MemoryPntr, UNS32 MemorySize, UNS32 StartOffset);
  BOOL unloadDataForItem (LI_DATA_ItemPointer ItemPntr,
    void *MemoryPntr, UNS32 MemorySize);
} *LI_DATA_MemorySourceClassPointer;
#endif // CE_ARTLIB_DataMemorySource



#if CE_ARTLIB_DataDataFileSource
// The data source for loading data items from an ordinary data file.  One of
// these is allocated for every different data file using "new", the destructor
// closes the file etc.  The data file name is stored in sourceName, and
// allocated from the library pool.

typedef class LI_DATA_DataFileSourceClass : public LI_DATA_GenericDataSourceClass
{
public:
  FILE *dataFileHandle; // The file itself, or NULL if closed.
  UNS32 numberOfItemsInIndex; // Number of data items in the file.
  UNS32 crc32Value;  // Cyclic redundancy check - for checking file integrity.

  LI_DATA_DataFileSourceClass (char *DataFileName);
  ~LI_DATA_DataFileSourceClass ();

  BOOL connectItem (LI_DATA_ItemPointer ItemPntr, UNS32 IndexIntoDataFile);

  BOOL connectAllItemsInDataFileToNewGroup (
    LE_DATA_DataGroupIndex GroupIndex, UNS16 NumberOfExtraItems);

  // Real implementations for most virtual functions....

  BOOL disconnectItem (LI_DATA_ItemPointer ItemPntr);
  UNS32 loadDataForItem (LI_DATA_ItemPointer ItemPntr,
    void *MemoryPntr, UNS32 MemorySize, UNS32 StartOffset);
  BOOL unloadDataForItem (LI_DATA_ItemPointer ItemPntr,
    void *MemoryPntr, UNS32 MemorySize);
  void describeSourceForItem (LI_DATA_ItemPointer ItemPntr,
    char *OutputString);
} *LI_DATA_DataFileSourceClassPointer;
#endif // CE_ARTLIB_DataDataFileSource



#if CE_ARTLIB_DataExternalFileSource
// The data source for loading data items from external files.  Usually there
// is only one global instance of this data source, statically allocated.  The
// file name is stored in the item, and the file is opened and closed during
// loading or unloading, it doesn't stay open between uses.  The name is
// allocated on the library pool and freed when the item is disconnected.

typedef class LI_DATA_ExternalFileSourceClass : public LI_DATA_GenericDataSourceClass
{
public:
  LI_DATA_ExternalFileSourceClass ();
  BOOL connectItem (LI_DATA_ItemPointer ItemPntr, char *ExternalFileName,
    LE_DATA_DataType NewFileType, UNS32 NewFileSize);
  BOOL updateInitialSize (LI_DATA_ItemPointer ItemPntr);

  // Real implementations for most virtual functions....

  BOOL disconnectItem (LI_DATA_ItemPointer ItemPntr);
  UNS32 loadDataForItem (LI_DATA_ItemPointer ItemPntr,
    void *MemoryPntr, UNS32 MemorySize, UNS32 StartOffset);
  BOOL unloadDataForItem (LI_DATA_ItemPointer ItemPntr,
    void *MemoryPntr, UNS32 MemorySize);
  void describeSourceForItem (LI_DATA_ItemPointer ItemPntr,
    char *OutputString);
  const char *getItemNameFromItem (LI_DATA_ItemPointer ItemPntr);
} *LI_DATA_ExternalFileSourceClassPointer;

#endif // CE_ARTLIB_DataExternalFileSource



#if CE_ARTLIB_DataURLSource
// The data source for loading data items from the Internet (read only).
// Usually there is only one global instance of this data source, statically
// allocated.  The URL (uniform resource locator) name is stored in the item,
// and the socket is opened and closed during loading or unloading, it doesn't
// stay open between uses.  The name is allocated on the library pool and
// freed when the item is disconnected.

typedef class LI_DATA_URLSourceClass : public LI_DATA_GenericDataSourceClass
{
public:
  HINSTANCE winsockLibraryInstance;
    // Identifies the Winsock DLL that we are using, NULL if the system
    // closed.  Shared by all threads reading from this data source,
    // so use the data source critical section when accessing it.

  // Pointers to Winsock functions.  Used instead of link time DLLs so that
  // the DLL can be optional (like when the user hasn't installed networking).
  // The pointers to the functions are sucked out of the DLL when this class
  // is initialised.

  typedef int (PASCAL FAR *type_closesocket) (SOCKET s);
  typedef int (PASCAL FAR *type_connect) (SOCKET s, const struct sockaddr FAR *name, int namelen);
  typedef struct hostent FAR * (PASCAL FAR *type_gethostbyname) (const char FAR * name);
  typedef u_short (PASCAL FAR *type_htons) (u_short hostshort);
  typedef unsigned long (PASCAL FAR *type_inet_addr) (const char FAR * cp);

  typedef int (PASCAL FAR *type_recv) (SOCKET s, char FAR * buf, int len, int flags);
  typedef int (PASCAL FAR *type_send) (SOCKET s, const char FAR * buf, int len, int flags);
  typedef int (PASCAL FAR *type_setsockopt) (SOCKET s, int level, int optname, const char FAR * optval, int optlen);
  typedef SOCKET (PASCAL FAR *type_socket) (int af, int type, int protocol);
  typedef int (PASCAL FAR *type_WSACleanup) (void);
  typedef int (PASCAL FAR *type_WSAGetLastError) (void);
  typedef int (PASCAL FAR *type_WSAStartup) (WORD wVersionRequired, LPWSADATA lpWSAData);

  type_closesocket pfclosesocket;
  type_connect pfconnect;
  type_gethostbyname pfgethostbyname;
  type_htons pfhtons;
  type_inet_addr pfinet_addr;
  type_recv pfrecv;
  type_send pfsend;
  type_setsockopt pfsetsockopt;
  type_socket pfsocket;
  type_WSACleanup pfWSACleanup;
  type_WSAGetLastError pfWSAGetLastError;
  type_WSAStartup pfWSAStartup;

  typedef enum URLServicesEnum
  {
    UNKNOWN_SERVICE = 0,
    HTTP_SERVICE,
    MAX_SERVICES
  } URLServices;

  LI_DATA_URLSourceClass ();
  ~LI_DATA_URLSourceClass ();

  BOOL connectItem (LI_DATA_ItemPointer ItemPntr, char *URLName,
    LE_DATA_DataType PreKnownType, UNS32 PreKnownSize);

  // Real implementations for most virtual functions....

  BOOL disconnectItem (LI_DATA_ItemPointer ItemPntr);
  UNS32 loadDataForItem (LI_DATA_ItemPointer ItemPntr,
    void *MemoryPntr, UNS32 MemorySize, UNS32 StartOffset);
  BOOL unloadDataForItem (LI_DATA_ItemPointer ItemPntr,
    void *MemoryPntr, UNS32 MemorySize);
  void describeSourceForItem (LI_DATA_ItemPointer ItemPntr,
    char *OutputString);
  const char *getItemNameFromItem (LI_DATA_ItemPointer ItemPntr);

  // Some support functions.

  URLServices parseURL (const char *URL, char *ComputerNameBuffer,
    char *ResourceNameBuffer, int& PortNumber);
  SOCKET openSocket (char *ComputerName, int PortNumber);
} *LI_DATA_URLSourceClassPointer;

#endif // CE_ARTLIB_DataURLSource



// This structure is used for keeping track of every data item currently
// available to the user's program.  This includes all data in and out
// of memory (every item in a data file gets one of these even when it
// isn't loaded).  A data group is simply an array of these structures.

struct LI_DATA_ItemStruct
{
  void * L_TYPE_volatile memoryPntr;
    // If this item is loaded into memory, this points to the memory,
    // otherwise (when unloaded) it is NULL.  First item in the structure
    // to speed up machine code access, since it is the most frequently
    // used field.  It will always be from the data memory pool.  Note that
    // the multithreaded version needs this to be a volatile field since
    // other threads check the value to see if the item is loaded or
    // needs loading.

  UNS32 initialSize;
    // The size of the item while it is in an unloaded state.  The actual
    // size (currentSize) can be different due to post-load processing.
    // For data files, this is also the uncompressed size.  The initial
    // size gets updated if the data is written during unloading.

  UNS32 currentSize;
    // How big the item is in bytes when loaded into memory.  Note that some
    // items have a size which is different from their on disk size due to
    // preprocessing during loading (like converting from 24 bit graphics
    // to the current screen depth).  Also, for writeable items, the size
    // of the in-memory item can grow larger as it is added to.

  LI_DATA_ItemPointer nextLRU;
#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  LI_DATA_ItemPointer previousLRU;
#endif
    // When loaded, and with a zero reference count, this item is kept in a
    // global least recently used (LRU) list of items.  Freshly accessed
    // items get moved to the head of the list while stale items get removed
    // from the tail and unloaded, when memory space is needed.  If the item
    // is unloaded, it isn't in the LRU list.  Totally unused data items
    // (not assigned to a data source) are in a singly linked free list in
    // their data group, reusing the nextLRU pointer for linking them
    // together.
    // Bleeble: Have separate LRU lists for different kinds of memory,
    // for example DirectDraw surfaces would be on an LRU list of
    // items with DirectDraw surfaces, and when we got low in video
    // memory then that LRU would give us the surface to discard.  Same
    // thing for DirectSound sounds?  There should then also be a field
    // here which specifies which LRU list the item should use.
    // Nope, if DirectDraw surfaces are being used, they would still have
    // a header record in the data system memory pool, so you should have
    // a second LRU for surfaces only and put it in the graphics sytem,
    // so bitmaps would be in two LRUs simultaneously (next/prev for the
    // graphics LRU would be in the bitmap header, not the data item info).

  LI_DATA_GenericDataSourceClassPointer dataSource;
    // This object will be used for getting data when needed and for
    // unloading this item too.  NULL if the item doesn't have a data
    // source (the item can't be used for anything in that case).

  union SourceDataUnion
  {
    struct DataFileStruct
    {
      UNS32 compressedSize;
        // Size of the item when it is on disk in a compressed data file.

      UNS32 offset;
        // Position in the compressed data file where the data starts.
    } dataFile;

    struct ExternalFileStruct
    {
      char *fileName;
        // For items which are stored in an individual external file, this
        // contains the file name.  Storage allocated for the name from
        // the library pool.
    } externalFile;

    struct URLStruct
    {
      char *name;
        // For items which are stored on the Internet, this contains
        // the URL string.  Storage allocated for the name from the
        // library pool.  Usually of the form
        // "http://www.artech.ca/images/logo.bmp".
    } URL;

    void *genericPntr;
      // Other data sources can store their stuff here.

  } sourceData;
    // Extra data for use by the data source to find the data for this item.

  LE_DATA_ReferenceCount L_TYPE_volatile referenceCount;
    // The reference count for this item.  User code bumps this up when it
    // wants to make sure that the item stays in memory (isn't unloaded)
    // and decrements it when it has finished using it.  If several things
    // are using it, then the count will be more than 1.  When the count
    // is zero it is safe to unload the item (nobody using it and thus
    // holding dangerous pointers to the data).  A 16 bit value is needed,
    // since preloading sequences adds a count for each time a bitmap is
    // present in the sequence, which can be quite a few times for common
    // bitmaps like the shadow under a character.  Needs to be volatile
    // so that tests of non-zero counts (to see if it is in the LRU)
    // work when multitasking.

  LE_DATA_DataType initialDataType;
    // The type of the data while it is in an unloaded state.  Like the
    // size, the actual type can be different after it is loaded.  For
    // example, a TAB (8 bit image) could be converted to a GBM (generic
    // bitmap), so initialDataType would be TAB and dataType would be GBM.
    // We keep it around since the item might need to be reloaded, which
    // means redoing the post-load processing using the initial type.

  LE_DATA_DataType dataType;
    // What kind of data is in this item.  Controls the preprocessing and
    // postprocessing for the item when it is loaded or unloaded.  Also
    // lets the user know what they are dealing with.  Initially, the data
    // source sets the type to the kind it is loading.  The post-load
    // processing can then change it to some other kind if it wants to
    // (usually when converting specific bitmap types into generic bitmaps).
    // I considered storing data attributes (bitmap width, height, depth,
    // sound duration) in the item directly to avoid having to load the data,
    // but you usually use the item anyways, so it isn't worthwhile wasting
    // memory space on extra attributes.

  UNS8 dirtyData;
    // A boolean which gets set to TRUE if this data item has been written
    // to since loading.  This means that the unloading function will have
    // to write out the changed data.

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
  UNS32 timesLoaded;            // Count 1 for each time it is loaded from disk.
  UNS32 timesAccessed;          // Count 1 for each time it is used.
  UNS32 timesUnloadedBySystem;  // Count 1 for unloaded to free up memory.
  UNS32 timesUnloadedByUser;    // Count 1 for unloaded by the user.
  TYPE_Tick timeOfLastLoad;     // Global clock when last load finished.
  UNS32 durationInMemory;       // Number of ticks (60hz) that it has been in memory.
  UNS32 serialNumberOfFirstLoad; // Sequence number for doing data file defragmentation.
#endif // CE_ARTLIB_EnableDebugMode

public:
  BOOL loadItem (void);
    // Allocates memory for the item from the data pool and fills it
    // with data from the item's data source.  Then it does the post-
    // load processing.  Returns TRUE if successful, FALSE if it fails.

  BOOL unloadItem (void);
    // Does pre-unload processing.  Then unloadItem gets this item's data
    // source to unload the data (flush dirty data to disk if it is a
    // writeable data item).  Then it deallocates the memory.

  BOOL resize (UNS32 NewSize);
    // Resizes the data used by the item to the new size, which can be bigger
    // or smaller.  If the item is loaded, the allocated memory is also
    // reallocated to the new size.  If the item is in an unloaded state,
    // this just changes the current size field.  That doesn't do much
    // since the current size gets changed to the actual amount of data
    // in the data source when it is loaded.  Does not change the dirty flag.

  BOOL addRef (void);
    // Increments the reference count, and if it was zero before, it
    // also removes the item from the least recently used list.  Note
    // that it doesn't load the item.  Returns TRUE if successful,
    // FALSE if something weird happened.

  BOOL removeRef (void);
    // Decrements the reference count.  If it is zero, the item also
    // gets added to the head of the LRU list.  Returns TRUE on success.

  BOOL disconnectFromDataSource (void);
    // Disconnect this item from its data source.

  void describeItemSource (char *OutputString);
    // Writes a text description of the data source for this item to
    // the given string.  Keeps it under 1024 characters.

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
  void printUsageStatistics (LE_DATA_DataId HintID = LE_DATA_EmptyItem);
    // Prints a description of the data item's usage history to the error
    // log.  If HintID is specified, it gets printed, but it is optional.
#endif

#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  BOOL addItemToLRU (void);
  BOOL removeItemFromLRU (void);
    // A couple of functions for adding and removing this item from the
    // global least recently used list.  Add adds it to the head, remove
    // removes it from anywhere in the list.  The pointer to the item is
    // implied in the C++ indirect call: ItemPntr->addItemToLRU().

  BOOL moveItemToHeadOfLRU (void);
    // A convenience function, it just removes the item and adds it back
    // to the head of the list, if the reference count is zero.  It is
    // is multitasking safe while separate removes and adds aren't
    // (two threads could remove it simultaneously and add it back twice
    // in a row).

#endif // CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded

#if CE_ARTLIB_DataNameHashTable
  const char *getItemName (void);
    // Returns the file name or URL or other name of this data item, used
    // in the name hash table.  Returns NULL for items with no name.
    // Goes through the data source to actually get the name out of the
    // item record.
#endif // CE_ARTLIB_DataNameHashTable

}; // Note: LI_DATA_ItemRecord, *LI_DATA_ItemPointer declared in L_DATA.h



typedef void * (*LI_DATA_PrePostLoadFunction) (LI_DATA_ItemPointer ItemPntr,
void *RawDataPntr);
  // Pointer to a function for doing data postloading or preunloading.
  // Returns a pointer to the processed data (either the same as RawDataPntr
  // or a freshly allocated pointer from the DATA pool) if successful.
  // The size of the data is in the item's currentSize field (which you
  // should change if you change the size of the item).  Please leave the
  // item's memoryPntr field set to NULL, the caller will change it when
  // the data item is fully loaded to avoid multitasking problems.
  // Returns NULL if it failed and the item should be left in an unloaded
  // state.  The caller will take care of deallocating the raw data pointer
  // if you return a different pointer value.



// Used for pkware compression / decompression callback.

typedef struct LI_DATA_DeompressionParametersStruct
{
  FILE *inputFile;                  // File we are reading from.
  BYTE *outputPntr;                 // Current output buffer write position.
  UNS32 compressedDataRemaining;    // Number of bytes we have left to read.
  UNS32 skipDecompressedRemaining;  // Skip this many dest bytes before writing decompressed data.
  UNS32 decompressedDataRemaining;  // Number of bytes we still need to write.
  BOOL  timeToStop;                 // True to force the decompressor to abort.
} LI_DATA_DecompressionParametersRecord, *LI_DATA_DecompressionParametersPointer;



// Hash table entries which are used for converting file names into
// DataIDs.  These are dynamically allocated on the Library memory pool,
// though it would be better to have a static array of them and dish
// them out of that.  But it's only used by utilities so it doesn't
// matter that much.  Anyway, there is a linked list of these for all
// names which have the same hash value.

#if CE_ARTLIB_DataNameHashTable
typedef struct LI_DATA_HashEntryStruct LI_DATA_HashEntryRecord, *LI_DATA_HashEntryPointer;

struct LI_DATA_HashEntryStruct
{
  LI_DATA_HashEntryPointer  next;
  LE_DATA_DataId            dataID;
};
#endif // CE_ARTLIB_DataNameHashTable




/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

char * LE_DATA_DataTypeNames [LE_DATA_DataTypeMax] =
{
  "LE_DATA_DataUnknown",
  "LE_DATA_DataBMP",
  "LE_DATA_DataDopeTAB",
  "LE_DATA_DataUAP",
  "LE_DATA_DataNative",
  "LE_DATA_DataGBMTexture",
  "LE_DATA_DataGBMPicture",
  "LE_DATA_DataGenericBitmap",
  "LE_DATA_DataWave",
  "LE_DATA_DataString",
  "LE_DATA_DataUserCreated1",
  "LE_DATA_DataIndexTable",
  "LE_DATA_DataChunky",
  "LE_DATA_DataTextureArray",
  "LE_DATA_Data3DModel",
  "LE_DATA_Data3DPose",
  "LE_DATA_DataHMD",
  "LE_DATA_DataMESHX"
};



char * LE_DATA_DataTypePrefixes [LE_DATA_DataTypeMax] =
{
  "UKN",
  "BMP",
  "DOPETAB",
  "TAB",  // Really should be UAP but the old dope sheet writes TAB_...
  "NTV",
  "GTXR",
  "GPIC",
  "GBM",
  "WAV",
  "STR",
  "UC1",
  "IDX",
  "CNK",
  "TRY",
  "C3D",
  "POS",
  "HMD",
  "MX"
};



UNS32 LE_DATA_DataPoolMemoryLimit = CE_ARTLIB_MemoryDataPoolMaxSize;
// The data system tries to keep the data pool's memory usage to under this
// number of bytes.  When it needs to allocate space, it unloads items
// until the new allocation will leave the total allocated data pool memory
// at this limit.  User code is allowed to change this, possibly to the
// actual system memory size minus some fixed overhead (if the system
// swaps too much, increase the overhead amount).


/****************************************************************************/
/* P R I V A T E   V A R I A B L E S                                        */
/*--------------------------------------------------------------------------*/

static LI_DATA_GroupRecord LI_DATA_GroupArray [CE_ARTLIB_DataMaxGroups+1];
  // Remember that group zero is never used since DataID zero is being
  // used for no item, just groups 1 to CE_ARTLIB_DataMaxGroups are valid,
  // also the very last group is used for runtime memory item allocation.

#if CE_ARTLIB_EnableMultitasking
static CRITICAL_SECTION LI_DATA_DataGroupCriticalSection;
static LPCRITICAL_SECTION L_TYPE_volatile LI_DATA_DataGroupCriticalSectionPntr;
  // The critical section guarding the data groups - so that two threads
  // can create new data items independently.  Needed in particular for the
  // runtime memory based data ID creation.
#endif



#if CE_ARTLIB_DataMemorySource
static LI_DATA_MemorySourceClassPointer LI_DATA_GlobalRuntimeMemoryDataSource;
  // This single instance of the runtime memory class is used as the data
  // source for all runtime memory data items.  NULL when not initialised.
  // A dynamic allocation is done rather than having a static object so that
  // initialisation gets done after other things are ready (like the error
  // reporting system).
#endif // CE_ARTLIB_DataMemorySource



#if CE_ARTLIB_DataExternalFileSource
static LI_DATA_ExternalFileSourceClassPointer LI_DATA_GlobalRuntimeExternalFileDataSource;
  // This single instance of the external file data source class is used for
  // all external file data items.  NULL when not initialised.
#endif // CE_ARTLIB_DataExternalFileSource



#if CE_ARTLIB_DataURLSource
static LI_DATA_URLSourceClassPointer LI_DATA_GlobalRuntimeURLDataSource;
  // This single instance of the URL data source class is used for
  // all URL data items.
#endif // CE_ARTLIB_DataURLSource



#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
static LI_DATA_ItemPointer LI_DATA_LeastRecentlyUsedHead;
static LI_DATA_ItemPointer LI_DATA_LeastRecentlyUsedTail;
#if CE_ARTLIB_EnableDebugMode
static UNS32 NumberOfItemsInLeastRecentlyUsedList;
#endif // CE_ARTLIB_EnableDebugMode
#endif //CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  // These keep track of the list of least recently used items, items which
  // are available for unloading if we need memory space.  New items are
  // added to the head, old items are removed from the tail.  When an item
  // is accessed, it is removed from the middle and added to the head
  // (which is why it has to be a doubly linked list).  Note that only loaded
  // items with a zero reference count are in the list, unloaded ones and
  // ones with a non-zero reference count are not in any list at all.  When
  // a reference count falls to zero, the item gets added to the head of the
  // LRU list.  Same for loading an item.  Unloading or adding a reference
  // removes the item from the list.

#if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
static CRITICAL_SECTION LI_DATA_LRUCriticalSection;
static LPCRITICAL_SECTION L_TYPE_volatile LI_DATA_LRUCriticalSectionPntr;
  // The critical section guarding the Least Recently Used list,
  // so that two separate threads loading data don't try to update
  // the list at the same time.  LRUListCriticalSectionPntr points
  // at LRUListCriticalSection if it has been initialised, NULL otherwise.
#endif



static LI_DATA_PrePostLoadFunction LE_DATA_PostLoadFunctions [LE_DATA_DataTypeMax];
static LI_DATA_PrePostLoadFunction LE_DATA_PreUnloadFunctions [LE_DATA_DataTypeMax];
  // Functions used for massaging the data after it has been loaded (for
  // example bitmaps get converted to the current screen depth) and for
  // processing it before unloading (deallocating extra stuff which was
  // allocated by the postloader).


static const char StringContentLength [] = "content-length:";
static const char StringContentType [] = "content-type:";



// The hash table used for converting file names into DataIDs.

#if CE_ARTLIB_DataNameHashTable
#define LI_DATA_HASH_TABLE_SIZE  256

static LI_DATA_HashEntryPointer LI_DATA_HashTable [LI_DATA_HASH_TABLE_SIZE];
#endif // CE_ARTLIB_DataNameHashTable


// Keeps track of the maximum amount of memory used for statistical purposes.

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
static UNS32 LI_DATA_DataPoolMaxUsed;
#endif



/****************************************************************************/
/* I M P L E M E N T A T I O N   O F   LI_DATA_DataGroup                    */
/*--------------------------------------------------------------------------*/

/*****************************************************************************
 * Get the next available data item in a data group.  Returns NULL on failure.
 * If you are just getting items without freeing any, they are in increasing
 * index order.
 */

static LI_DATA_ItemPointer LI_DATA_AllocateItemFromDataGroup (
LI_DATA_GroupPointer GroupPntr)
{
  LI_DATA_ItemPointer NewItemPntr;

  if (GroupPntr == NULL)
    return NULL;

  #if CE_ARTLIB_EnableMultitasking
  if (LI_DATA_DataGroupCriticalSectionPntr != NULL)
    EnterCriticalSection (LI_DATA_DataGroupCriticalSectionPntr);
  #endif

  if (GroupPntr->firstFreeItem == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_AllocateItemFromDataGroup: "
      "No more free data items in data group 0x%08X (#%u).\r\n",
      (unsigned int) GroupPntr, (unsigned int) (GroupPntr - LI_DATA_GroupArray));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    NewItemPntr = NULL; // Out of memory.
  }
  else
  {
    NewItemPntr = GroupPntr->firstFreeItem;
    GroupPntr->firstFreeItem = NewItemPntr->nextLRU;

    // Initialise the item to be empty - no data source,
    // NULL LRU pointers, etc.

    memset (NewItemPntr, 0, sizeof (LI_DATA_ItemRecord));
  }

  #if CE_ARTLIB_EnableMultitasking
  if (LI_DATA_DataGroupCriticalSectionPntr != NULL)
    LeaveCriticalSection (LI_DATA_DataGroupCriticalSectionPntr);
  #endif

  return NewItemPntr;
}



/*****************************************************************************
 * Put the given item back onto the list of free items in the data group.
 */

static BOOL LI_DATA_FreeItemToDataGroup (LI_DATA_ItemPointer FreedItemPntr,
LI_DATA_GroupPointer GroupPntr)
{
  if (GroupPntr == NULL || FreedItemPntr == NULL)
    return FALSE;

#if CE_ARTLIB_EnableDebugMode
  if (FreedItemPntr->dataSource != NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_FreeItemToDataGroup: "
      "Data source isn't NULL!  Somebody didn't disconnect the item.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif

  // Just to make it obvious in the debugger which items are free.

#if CE_ARTLIB_EnableDebugMode
  memset (FreedItemPntr, 255, sizeof (LI_DATA_ItemRecord));
#endif

  // Add the free item to the group's free list.

  #if CE_ARTLIB_EnableMultitasking
  if (LI_DATA_DataGroupCriticalSectionPntr != NULL)
    EnterCriticalSection (LI_DATA_DataGroupCriticalSectionPntr);
  #endif

  FreedItemPntr->nextLRU = GroupPntr->firstFreeItem;
  GroupPntr->firstFreeItem = FreedItemPntr;

  // Identify this item as a disconnected item.

  FreedItemPntr->dataSource = NULL;

  #if CE_ARTLIB_EnableMultitasking
  if (LI_DATA_DataGroupCriticalSectionPntr != NULL)
    LeaveCriticalSection (LI_DATA_DataGroupCriticalSectionPntr);
  #endif

  return TRUE;
}



/*****************************************************************************
 * Allocate and set up free list of items in a data group.
 * Returns TRUE if successful.
 */

BOOL LE_DATA_InitialiseDataGroup (LE_DATA_DataGroupIndex GroupIndex,
int DesiredNumberOfItems)
{
  LI_DATA_ItemPointer   ArrayStart;
  UNS32                 ByteSize;
  LI_DATA_GroupPointer  GroupPntr;
  int                   i;
  LI_DATA_ItemPointer   ItemPntr;

#if CE_ARTLIB_EnableDebugMode
  if (GroupIndex == 0 ||
  GroupIndex > CE_ARTLIB_DataMaxGroups ||
  DesiredNumberOfItems < 1 ||
  DesiredNumberOfItems > 0x10000 /* Max 64K per group since 16 bit index */)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_InitialiseDataGroup: "
      "Bad group index (%u) or number of items (%u).\r\n",
      (unsigned int) GroupIndex, (unsigned int) DesiredNumberOfItems);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE; // Bad arguments.
  }
#endif

  GroupPntr = LI_DATA_GroupArray + GroupIndex;

#if CE_ARTLIB_EnableDebugMode
  if (GroupPntr->dataItemArray != NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_InitialiseDataGroup: "
      "Group %u already seems to have memory allocated to its item array "
      "and supposedly already has %u items.\r\n",
      (unsigned int) GroupIndex, (unsigned int) GroupPntr->numberOfDataItems);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE; // Group is already allocated.
  }
#endif

  // Allocate space for the array of data items.

  ByteSize = DesiredNumberOfItems * sizeof (LI_DATA_ItemRecord);
  ArrayStart = (LI_DATA_ItemPointer) LI_MEMORY_AllocLibraryBlock (ByteSize);
  if (ArrayStart == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_InitialiseDataGroup: "
      "Unable to allocate array for %u data items (%u bytes).\r\n",
      (unsigned int) DesiredNumberOfItems, (unsigned int) ByteSize);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE; // Out of memory.
  }

  // Initialise the data items to be all ones so that you can tell they
  // are on the free list (no data source assigned etc).  If you try to
  // use them you will quickly get an error.

#if CE_ARTLIB_EnableDebugMode
  memset (ArrayStart, 255, ByteSize);
#endif

  // Set up the free list of data items, so that the items get allocated in
  // increasing array index order (useful for batch loading a data file).

  for (i = 0, ItemPntr = ArrayStart;
  i < DesiredNumberOfItems;
  i++, ItemPntr++)
  {
    ItemPntr->dataSource = NULL;
    ItemPntr->nextLRU = ItemPntr + 1;
  }
  ArrayStart [DesiredNumberOfItems-1].nextLRU = NULL; // Last node ends list.

  // Hook up the free list.

  GroupPntr->firstFreeItem = GroupPntr->dataItemArray = ArrayStart;
  GroupPntr->numberOfDataItems = DesiredNumberOfItems;

  return TRUE;
}



/*****************************************************************************
 * Terminate a data group.  Frees its memory and otherwise closes it down,
 * does a last ditch unloading of data items.
 */

BOOL LE_DATA_RemoveDataGroup (LE_DATA_DataGroupIndex GroupIndex)
{
  LE_DATA_DataItemIndex ItemIndex;
  LI_DATA_ItemPointer   ItemPntr;
  LI_DATA_GroupPointer  GroupPntr;

#if CE_ARTLIB_EnableDebugMode
  if (GroupIndex == 0 || GroupIndex > CE_ARTLIB_DataMaxGroups)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_RemoveDataGroup: "
      "Bad group index (%u).\r\n",
      (unsigned int) GroupIndex);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE; // Bad arguments.
  }
#endif

  GroupPntr = LI_DATA_GroupArray + GroupIndex;

  if (GroupPntr->dataItemArray != NULL)
  {
    // Unload all data items first, even if they have reference counts etc.
    // That way data sources with items outstanding will get closed when
    // their item count falls to zero.

    for (ItemIndex = 0, ItemPntr = GroupPntr->dataItemArray;
    ItemIndex < GroupPntr->numberOfDataItems;
    ItemIndex++, ItemPntr++)
    {
      if (ItemPntr->dataSource != NULL)
      {
        if (ItemPntr->memoryPntr != NULL)
        {
          if (ItemPntr->referenceCount != 0)
          {
#if CE_ARTLIB_EnableDebugMode
            char DescriptionBuffer [2048];

            sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_RemoveDataGroup: "
              "Encountered an item still in use, reference count is %u, "
              "will unload it anyways.  %s\r\n",
              (unsigned int) ItemPntr->referenceCount,
              LE_DATA_DescString (LE_DATA_IdFromTag (GroupIndex, ItemIndex),
              DescriptionBuffer, sizeof (DescriptionBuffer)));
            LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
            ItemPntr->referenceCount = 1;
            ItemPntr->removeRef ();
          }
          ItemPntr->unloadItem ();
        }
#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
        ItemPntr->printUsageStatistics (LE_DATA_IdFromTag (GroupIndex, ItemIndex));
#endif
        ItemPntr->disconnectFromDataSource ();
        LI_DATA_FreeItemToDataGroup (ItemPntr, GroupPntr);
      }
    }

    LI_MEMORY_DeallocLibraryBlock (GroupPntr->dataItemArray);
    GroupPntr->firstFreeItem = GroupPntr->dataItemArray = NULL;
    GroupPntr->numberOfDataItems = 0;
  }

  return TRUE;
}



/****************************************************************************/
/* I M P L E M E N T A T I O N   O F   LI_DATA_GenericDataSource            *
/*--------------------------------------------------------------------------*/

/*****************************************************************************
 * The initialisation function for the stuff that all data sources have in
 * common - initialise the critical section and set the item count to zero.
 */

LI_DATA_GenericDataSourceClass::LI_DATA_GenericDataSourceClass ()
{
#if CE_ARTLIB_EnableMultitasking
  InitializeCriticalSection (&dataSourceCriticalSection);
  dataSourceCriticalSectionPntr = &dataSourceCriticalSection;
#endif // CE_ARTLIB_EnableMultitasking

  numberOfItems = 0;
  sourceName = "Generic Data Source (you should never see this)";
}



/*****************************************************************************
 * The common destruction processing for all data sources, just deallocate
 * the critical section.
 */

LI_DATA_GenericDataSourceClass::~LI_DATA_GenericDataSourceClass ()
{
#if CE_ARTLIB_EnableMultitasking
  if (dataSourceCriticalSectionPntr != NULL)
  {
    // Set the critical section access pointer to NULL first so that
    // when the section is deleted, all the waiting threads wake up and
    // see that the critical section is gone, and return immediately.  Note
    // that they wake up instantly at the delete, and this thread may not
    // run the statement after the delete for quite some time.

    EnterCriticalSection (dataSourceCriticalSectionPntr);
    dataSourceCriticalSectionPntr = NULL;
    DeleteCriticalSection (&dataSourceCriticalSection);
  }
#endif // CE_ARTLIB_EnableMultitasking

#if CE_ARTLIB_EnableDebugMode
  if (numberOfItems != 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "~LI_DATA_GenericDataSourceClass: "
      "There are still %u data items using this data source!  Should be zero.\r\n",
      (unsigned int) numberOfItems);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif // CE_ARTLIB_EnableDebugMode
}



/*****************************************************************************
 * Default virtual function for writing a readable description of the data
 * source for the given item to the string.  Should take less than 1024
 * characters.  It will usually be appended to a string like "Data from "
 * when printed.  The default just copies the source name to the string.
 */

void LI_DATA_GenericDataSourceClass::describeSourceForItem (
LI_DATA_ItemPointer ItemPntr, char *OutputString)
{
  strcpy (OutputString, sourceName);
}



#if CE_ARTLIB_DataNameHashTable
/*****************************************************************************
 * Default virtual function for getting the name of a data item.  By
 * default there is no name since only some data sources have named items.
 */

const char *LI_DATA_GenericDataSourceClass::getItemNameFromItem (
LI_DATA_ItemPointer ItemPntr)
{
  return NULL;
}
#endif // CE_ARTLIB_DataNameHashTable



#if CE_ARTLIB_DataMemorySource
/****************************************************************************/
/* I M P L E M E N T A T I O N   O F   LI_DATA_MemorySource                 */
/*--------------------------------------------------------------------------*/

/*****************************************************************************
 * Initialise the memory source.  The generic constructor does most of the
 * work before this gets called.
 */

LI_DATA_MemorySourceClass::LI_DATA_MemorySourceClass ()
{
  sourceName = "Memory Source";
}



/*****************************************************************************
 * Initialise a data item for use as a memory item.  It isn't actually
 * allocated until it is loaded.  Returns TRUE if successful.
 */

BOOL LI_DATA_MemorySourceClass::connectItem (LI_DATA_ItemPointer ItemPntr,
UNS32 ByteSize, LE_DATA_DataType DataType)
{
#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_MemorySourceClass::connectItem: "
      "NULL item pointer!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (ItemPntr->dataSource != NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_MemorySourceClass::connectItem: "
      "The item already has a data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif // CE_ARTLIB_EnableDebugMode

  #if CE_ARTLIB_EnableMultitasking
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (dataSourceCriticalSectionPntr);
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

  numberOfItems++;

  ItemPntr->memoryPntr = NULL;
  ItemPntr->currentSize = ItemPntr->initialSize = ByteSize;
  ItemPntr->dataType = ItemPntr->initialDataType = DataType;
  ItemPntr->dataSource = this;

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSourceCriticalSectionPntr);
  #endif

  return TRUE;
}



/*****************************************************************************
 * Disconnect the given item.  Since this is usually a static source, it
 * won't self destruct if there are no items.
 */

BOOL LI_DATA_MemorySourceClass::disconnectItem (LI_DATA_ItemPointer ItemPntr)
{
#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_MemorySourceClass::disconnectItem: "
      "NULL item pointer!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (ItemPntr->dataSource != this)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_MemorySourceClass::disconnectItem: "
      "The item has something else as its data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (ItemPntr->memoryPntr != NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_MemorySourceClass::disconnectItem: "
      "The item has a non-NULL memory pointer, someone didn't unload it!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (numberOfItems == 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_MemorySourceClass::disconnectItem: "
      "The item count is already zero!  What's going on?\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif // CE_ARTLIB_EnableDebugMode

  #if CE_ARTLIB_EnableMultitasking
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (dataSourceCriticalSectionPntr);
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

  ItemPntr->dataSource = NULL;
  numberOfItems--;

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSourceCriticalSectionPntr);
  #endif

  return TRUE;
}



/*****************************************************************************
 * There is no data for memory items, just zeroes.  The system generally
 * tries to avoid unloading/reloading memory items for that reason.  Returns
 * the number of bytes loaded.  Returns zero on failure.
 */

UNS32 LI_DATA_MemorySourceClass::loadDataForItem (LI_DATA_ItemPointer ItemPntr,
void *MemoryPntr, UNS32 MemorySize, UNS32 StartOffset)
{
  UNS32 AmountToLoad;

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_MemorySourceClass::loadDataForItem: "
      "Called with NULL item pointer!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (MemoryPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_MemorySourceClass::loadDataForItem: "
      "Called with NULL MemoryPntr!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (ItemPntr->dataSource != this)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_MemorySourceClass::loadDataForItem: "
      "The item has something else as its data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }
#endif // CE_ARTLIB_EnableDebugMode

  if (StartOffset >= ItemPntr->initialSize)
    return 0; // No data available at that offset.

  AmountToLoad = ItemPntr->initialSize - StartOffset;
  if (AmountToLoad > MemorySize)
    AmountToLoad = MemorySize;

  // Would fill memory with data here, but this is a memory
  // source so we do nothing.

  return AmountToLoad;
}



/*****************************************************************************
 * These memory items are read-only, don't need to do anything to unload
 * them.
 */

BOOL LI_DATA_MemorySourceClass::unloadDataForItem (LI_DATA_ItemPointer ItemPntr,
void *MemoryPntr, UNS32 MemorySize)
{
#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_MemorySourceClass::unloadDataForItem: "
      "Called with NULL item\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (ItemPntr->dataSource != this)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_MemorySourceClass::unloadDataForItem: "
      "The item has something else as its data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif // CE_ARTLIB_EnableDebugMode

  // Would unload the data here - writing back to a real file etc,
  // but this is a memory source so we do nothing.

  return TRUE;
}
#endif // CE_ARTLIB_DataMemorySource



#if CE_ARTLIB_DataDataFileSource
/****************************************************************************/
/* I M P L E M E N T A T I O N   O F   LI_DATA_DataFileSourceClass          */
/*--------------------------------------------------------------------------*/

/*****************************************************************************
 * Initialise the data file source.  If it fails, the dataFileHandle field is
 * left at NULL (you should then destroy this object - not sure if you can
 * do a "delete" inside the initialiser, or even if it was allocated with
 * "new").  A successful initialisation leaves the data file open, but
 * doesn't read in the index or set up the data items.
 */

LI_DATA_DataFileSourceClass::LI_DATA_DataFileSourceClass (char *DataFileName)
{
  char                          CurrentDirectoryName [MAX_PATH];
  int                           FileNameLength;
  LI_DATA_DatafileHeaderRecord  HeaderRecord;

  dataFileHandle = NULL;
  sourceName = NULL;

  if (DataFileName == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass: "
      "File name string pointer is NULL!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Allocate space for a copy of the file name using the library pool.

  FileNameLength = strlen (DataFileName);

  if (FileNameLength == 0)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass: "
      "File name is an empty string!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  sourceName = (char *) LI_MEMORY_AllocLibraryBlock (FileNameLength + 1);
  if (sourceName == NULL) goto ErrorExit;

  strcpy (sourceName, DataFileName);

  dataFileHandle = fopen (sourceName, "rb");

  if (dataFileHandle == NULL)
  {
    _getcwd (CurrentDirectoryName, sizeof (CurrentDirectoryName));
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass: "
      "Unable to open data file \"%s\" with current directory \"%s\".\r\n",
      sourceName, CurrentDirectoryName);
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#else // A fatal error in release mode.
    LE_ERROR_ErrorMsg (LE_ERROR_DebugMessageBuffer);
#endif
    goto ErrorExit;
  }

  // First load the data file header info.

  if (fread (&HeaderRecord, sizeof (HeaderRecord), 1, dataFileHandle) != 1)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass: "
      "Unable to read full header info from data file \"%s\".  Maybe it is not a data file?\r\n",
      sourceName);
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#else // A fatal error in release mode.
    LE_ERROR_ErrorMsg (LE_ERROR_DebugMessageBuffer);
#endif
    goto ErrorExit;
  }

  // Check for the Artech string.

  if (strncmp (DATAFILE_ARTECH_STRING, HeaderRecord.ArtechString,
  sizeof (DATAFILE_ARTECH_STRING)) != 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass: "
      "Invalid header in data file \"%s\".  Maybe it is not a data file?\r\n",
      sourceName);
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#else // A fatal error in release mode.
    LE_ERROR_ErrorMsg (LE_ERROR_DebugMessageBuffer);
#endif
    goto ErrorExit;
  }

  // Check data file version number.

  if (HeaderRecord.VersionMajor != LI_DATA_DataFileVersion_Major ||
  HeaderRecord.VersionMinor != LI_DATA_DataFileVersion_Minor)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass: "
      "Data file \"%s\" is version %u.%u, we need %u.%u.\r\n",
      sourceName,
      (unsigned int) HeaderRecord.VersionMajor,
      (unsigned int) HeaderRecord.VersionMinor,
      (unsigned int) LI_DATA_DataFileVersion_Major,
      (unsigned int) LI_DATA_DataFileVersion_Minor);
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#else // A fatal error in release mode.
    LE_ERROR_ErrorMsg (LE_ERROR_DebugMessageBuffer);
#endif
    goto ErrorExit;
  }

#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass: "
    "Data file \"%s\" is version %u.%u, patch version %u.%u.  "
    "It has %u data items and a CRC of 0x%08X.\r\n",
    sourceName,
    (unsigned int) HeaderRecord.VersionMajor,
    (unsigned int) HeaderRecord.VersionMinor,
    (unsigned int) HeaderRecord.PatchMajor,
    (unsigned int) HeaderRecord.PatchMinor,
    (unsigned int) HeaderRecord.IndexSize,
    (unsigned int) HeaderRecord.CRC32);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif

  // Retrieve important information from the header.

  numberOfItemsInIndex = HeaderRecord.IndexSize;
  crc32Value = HeaderRecord.CRC32;

  return; // Successful!

ErrorExit:
  if (dataFileHandle != NULL)
  {
    fclose (dataFileHandle);
    dataFileHandle = NULL;
  }

  if (sourceName != NULL)
  {
    LI_MEMORY_DeallocLibraryBlock (sourceName);
    sourceName = NULL;
  }
}



/*****************************************************************************
 * Deallocate this data source.  Mostly just closing the file and
 * deallocating memory.
 */

LI_DATA_DataFileSourceClass::~LI_DATA_DataFileSourceClass ()
{
  if (dataFileHandle != NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::~LI_DATA_DataFileSourceClass: "
      "Now closing data file \"%s\".\r\n",
      sourceName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif

    fclose (dataFileHandle);
    dataFileHandle = NULL;
  }

  if (sourceName != NULL)
  {
    LI_MEMORY_DeallocLibraryBlock (sourceName);
    sourceName = NULL;
  }
}



/*****************************************************************************
 * Set up a data item to use a particular piece of data from the data file.
 * Returns TRUE if successful.  Inefficient - it reads the index entry
 * from the file; this more useful for patching an existing data file
 * than bulk loading all the data items in a whole data file.
 */

BOOL LI_DATA_DataFileSourceClass::connectItem (LI_DATA_ItemPointer ItemPntr,
UNS32 IndexIntoDataFile)
{
  LI_DATA_DatafileIndexRecord IndexRecord;
  BOOL                        ReturnCode = FALSE;

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::connectItem: "
      "NULL item pointer!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (ItemPntr->dataSource != NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::connectItem: "
      "The item already has a data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (dataFileHandle == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::connectItem: "
      "The data file is closed!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (IndexIntoDataFile >= numberOfItemsInIndex)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::connectItem: "
      "Index %u is larger than the number of items in the data file (%u).\r\n",
      (unsigned int) IndexIntoDataFile,
      (unsigned int) numberOfItemsInIndex);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif // CE_ARTLIB_EnableDebugMode

  #if CE_ARTLIB_EnableMultitasking
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (dataSourceCriticalSectionPntr);
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

  // Read the data file info entry for the given index.

  if (0 != fseek (dataFileHandle, sizeof (LI_DATA_DatafileHeaderRecord) +
  IndexIntoDataFile * sizeof (LI_DATA_DatafileIndexRecord), SEEK_SET))
    goto ErrorExit;

  if (fread (&IndexRecord, sizeof (IndexRecord), 1, dataFileHandle) != 1)
    goto ErrorExit;

  // Copy the index info into our new data item.

  ItemPntr->currentSize = ItemPntr->initialSize = IndexRecord.UCompSize;
  ItemPntr->dataType = ItemPntr->initialDataType = IndexRecord.DataType;
  ItemPntr->sourceData.dataFile.compressedSize = IndexRecord.CompSize;
  ItemPntr->sourceData.dataFile.offset = IndexRecord.Offset;
  ItemPntr->memoryPntr = NULL;
  ItemPntr->dataSource = this;

  numberOfItems++;

  ReturnCode = TRUE;
  goto NormalExit;

ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::connectItem: "
    "Error while reading the data file (%s) index (%u) header info.\r\n",
    sourceName, (unsigned int) IndexIntoDataFile);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif // CE_ARTLIB_EnableDebugMode

NormalExit:
  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSourceCriticalSectionPntr);
  #endif

  return ReturnCode;
}



/*****************************************************************************
 * Connects all the items from the data file to a newly created data group.
 * Returns TRUE if successful.  The group should be empty before you start.
 * An additional NumberOfExtraItems will be available in the data group
 * for future expansion (runtime allocation, patching in from other files,
 * and that sort of thing).
 */

BOOL LI_DATA_DataFileSourceClass::connectAllItemsInDataFileToNewGroup (
LE_DATA_DataGroupIndex GroupIndex, UNS16 NumberOfExtraItems)
{
  BOOL                          GroupInitialised = FALSE;
  LI_DATA_GroupPointer          GroupPntr;
  int                           i;
  LI_DATA_DatafileIndexPointer  IndexArray = NULL;
  UNS32                         IndexByteSize;
  LI_DATA_DatafileIndexPointer  IndexPntr;
  LI_DATA_ItemPointer           ItemPntr;
  BOOL                          ReturnCode = FALSE;

#if CE_ARTLIB_EnableDebugMode
  if (dataFileHandle == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::connectAllItemsInDataFileToNewGroup: "
      "The data file is closed!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif // CE_ARTLIB_EnableDebugMode

  // Critical sections needed for both the data source (so that
  // some other thread doesn't try reading the data file while
  // we are using it), and for the data groups (so that our
  // new group doesn't get modified while we are working on it).

  #if CE_ARTLIB_EnableMultitasking
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (dataSourceCriticalSectionPntr);
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

  #if CE_ARTLIB_EnableMultitasking
  if (LI_DATA_DataGroupCriticalSectionPntr != NULL)
    EnterCriticalSection (LI_DATA_DataGroupCriticalSectionPntr);
  #endif

  // First set up our new data group.  Big enough to hold the whole
  // data file plus a few extra items.

  if (!LE_DATA_InitialiseDataGroup (GroupIndex,
  numberOfItemsInIndex + NumberOfExtraItems))
    goto ErrorExit;

  GroupPntr = LI_DATA_GroupArray + GroupIndex;
  GroupInitialised = TRUE;

  // Temporarily allocate space to hold the index from the data file.

  IndexByteSize = numberOfItemsInIndex * sizeof (LI_DATA_DatafileIndexRecord);

  IndexArray = (LI_DATA_DatafileIndexPointer)
    LI_MEMORY_AllocLibraryBlock (IndexByteSize);

  if (IndexArray == NULL)
    goto ErrorExit;

  // Read in the whole index.

  if (0 != fseek (dataFileHandle, sizeof (LI_DATA_DatafileHeaderRecord), SEEK_SET))
    goto ErrorExit;

  if (fread (IndexArray, IndexByteSize, 1, dataFileHandle) != 1)
    goto ErrorExit;

  // Set up the data items from the index information.  Rather than
  // calling LI_DATA_AllocateItemFromDataGroup, we do it all in one
  // big batch here.

  // Initialise our data items to the initial state (NULL pointers
  // and zero counts etc).

  memset (GroupPntr->dataItemArray, 0,
    sizeof (LI_DATA_ItemRecord) * numberOfItemsInIndex);

  // Reset the free list of the group to exclude our new items.

  if (NumberOfExtraItems > 0)
    GroupPntr->firstFreeItem = GroupPntr->dataItemArray + numberOfItemsInIndex;
  else // No extra items.
    GroupPntr->firstFreeItem = NULL;

  // Copy the index info to our new data items.

  for (i = 0, ItemPntr = GroupPntr->dataItemArray, IndexPntr = IndexArray;
  i < (int) numberOfItemsInIndex;
  i++, ItemPntr++, IndexPntr++)
  {
    ItemPntr->currentSize = ItemPntr->initialSize = IndexPntr->UCompSize;
    ItemPntr->dataType = ItemPntr->initialDataType = IndexPntr->DataType;
    ItemPntr->sourceData.dataFile.compressedSize = IndexPntr->CompSize;
    ItemPntr->sourceData.dataFile.offset = IndexPntr->Offset;
    ItemPntr->dataSource = this;
  }

  numberOfItems += numberOfItemsInIndex;

  ReturnCode = TRUE;
  goto NormalExit;

ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::connectAllItemsInDataFileToNewGroup: "
    "Error while trying to bulk init %u (%u + %u extra) data items "
    "from data file \"%s\".\r\n",
    (unsigned int) (numberOfItemsInIndex + NumberOfExtraItems),
    (unsigned int) numberOfItemsInIndex,
    (unsigned int) NumberOfExtraItems,
    sourceName);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif // CE_ARTLIB_EnableDebugMode

  if (GroupInitialised)
    LE_DATA_RemoveDataGroup (GroupIndex);

NormalExit:
  if (IndexArray != NULL)
    LI_MEMORY_DeallocLibraryBlock (IndexArray);

  #if CE_ARTLIB_EnableMultitasking
  if (LI_DATA_DataGroupCriticalSectionPntr != NULL)
    LeaveCriticalSection (LI_DATA_DataGroupCriticalSectionPntr);
  #endif

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSourceCriticalSectionPntr);
  #endif

  return ReturnCode;
}



/*****************************************************************************
 * Disconnect the given item.  Also self destruct (close file) if there are
 * no more data items using this data file.
 */

BOOL LI_DATA_DataFileSourceClass::disconnectItem (LI_DATA_ItemPointer ItemPntr)
{
#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::disconnectItem: "
      "NULL item pointer!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (ItemPntr->dataSource != this)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::disconnectItem: "
      "The item has something else as its data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (ItemPntr->memoryPntr != NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::disconnectItem: "
      "The item has a non-NULL memory pointer, someone didn't unload it!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (numberOfItems == 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::disconnectItem: "
      "The item count is already zero!  What's going on?\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif // CE_ARTLIB_EnableDebugMode

  #if CE_ARTLIB_EnableMultitasking
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (dataSourceCriticalSectionPntr);
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

  // The data file is read-only (at least for now), so just discard the data
  // in the user's buffer, even if it is different from that in the data file.

  ItemPntr->dataSource = NULL;
  numberOfItems--;

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSourceCriticalSectionPntr);
  #endif

  if (numberOfItems == 0)
    delete this;

  return TRUE;
}



/*****************************************************************************
 * Decompression routine callback for reading more compressed data.  Returns
 * the number of bytes read, zero will make the decompressor stop.  Don't
 * know why it passes the size as *Size, but you don't have to update it.
 */

extern "C" unsigned int LI_DATA_ReadCompressedData (
  char *Buffer, unsigned int *Size, void *Param)
{
  UNS32                                   AmountActuallyRead;
  UNS32                                   AmountToRead;
  LI_DATA_DecompressionParametersPointer  ParamPntr;

  ParamPntr = (LI_DATA_DecompressionParametersPointer) Param;

  if (ParamPntr->timeToStop)
    return 0; // The writing part requested an abort.

  AmountToRead = *Size;
  if (ParamPntr->compressedDataRemaining < AmountToRead)
    AmountToRead = ParamPntr->compressedDataRemaining;

  if (AmountToRead > 0)
  {
    AmountActuallyRead = fread (Buffer, 1 /* Read in byte size units */,
      AmountToRead, ParamPntr->inputFile);

#if CE_ARTLIB_EnableDebugMode
    if (AmountActuallyRead > AmountToRead)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ReadCompressedData: "
        "Amount actually read (%u) is bigger than requested amount (%u)!\r\n",
        (unsigned int) AmountActuallyRead,
        (unsigned int) AmountToRead);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    }
#endif

    ParamPntr->compressedDataRemaining -= AmountActuallyRead;
  }
  else // Don't need to read anything, at end of data item.
    AmountActuallyRead = 0;

  return AmountActuallyRead;
}



/*****************************************************************************
 * Decompression routine callback for writing the decompressed data.  Just
 * writes it into the user's buffer, and advances the buffer pointer.
 * Well, not quite, it can skip some initial number of bytes if requested.
 * Also it can abort the decompression if the user's buffer is full
 * (happens when only reading part of a data item).
 */

extern "C" void LI_DATA_WriteUncompressedData (
  char *Buff, unsigned int *Size, void *Param)
{
  UNS32                                   AmountToWrite;
  LI_DATA_DecompressionParametersPointer  ParamPntr;

  ParamPntr = (LI_DATA_DecompressionParametersPointer) Param;

  if (ParamPntr->timeToStop)
    return; // Still busy aborting, do nothing.

  AmountToWrite = *Size;

  // See if we should skip over some of the decompressed data,
  // this feature is used for reading in a portion of a data item.

  if (ParamPntr->skipDecompressedRemaining > 0)
  {
    if (AmountToWrite <= ParamPntr->skipDecompressedRemaining)
    {
      // Skipping over whole buffer, just update the numbers.

      ParamPntr->skipDecompressedRemaining -= AmountToWrite;
      return;
    }

    // Skipping over only part of the buffer.

    AmountToWrite -= ParamPntr->skipDecompressedRemaining;
    Buff += ParamPntr->skipDecompressedRemaining;
    ParamPntr->skipDecompressedRemaining = 0;
  }


  // If we are already full, tell it to stop.

  if (ParamPntr->decompressedDataRemaining <= 0)
  {
    ParamPntr->timeToStop = TRUE;
    return;
  }

  // If whole amount will fit in our output buffer, just copy it.

  if (AmountToWrite <= ParamPntr->decompressedDataRemaining)
  {
    memcpy (ParamPntr->outputPntr, Buff, AmountToWrite);
    ParamPntr->decompressedDataRemaining -= AmountToWrite;
    ParamPntr->outputPntr += AmountToWrite;
    return;
  }

  // Have more data than we need.  Copy part and tell it to stop.

  memcpy (ParamPntr->outputPntr, Buff, ParamPntr->decompressedDataRemaining);
  ParamPntr->outputPntr += ParamPntr->decompressedDataRemaining;
  ParamPntr->decompressedDataRemaining = 0;
  ParamPntr->timeToStop = TRUE;
}



/*****************************************************************************
 * Load the data for the given item.  This means decompressing the block of
 * data starting at a particular offset in the data file.  Returns
 * the number of bytes loaded.  Returns zero on failure.
 */

UNS32 LI_DATA_DataFileSourceClass::loadDataForItem (LI_DATA_ItemPointer ItemPntr,
void *MemoryPntr, UNS32 MemorySize, UNS32 StartOffset)
{
  UNS32                                 AmountToLoad;
  LI_DATA_DecompressionParametersRecord CallbackParameters;
  int                                   ErrorCode;
  char                                  WorkBuffer [EXP_BUFFER_SIZE];

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::loadDataForItem: "
      "Called with NULL item pointer!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (MemoryPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::loadDataForItem: "
      "Called with NULL MemoryPntr!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (ItemPntr->dataSource != this)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::loadDataForItem: "
      "The item has something else as its data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (dataFileHandle == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::loadDataForItem: "
      "The data file is closed!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }
#endif // CE_ARTLIB_EnableDebugMode

  // Calculate the amount of data we need to read.

  if (StartOffset >= ItemPntr->initialSize)
    return 0; // No data available at that offset.

  #if CE_ARTLIB_EnableMultitasking
  if (dataSourceCriticalSectionPntr == NULL) return 0; // System is closed.
  EnterCriticalSection (dataSourceCriticalSectionPntr);
  if (dataSourceCriticalSectionPntr == NULL) return 0; // Closed while we waited.
  #endif

  AmountToLoad = ItemPntr->initialSize - StartOffset;
  if (AmountToLoad > MemorySize)
    AmountToLoad = MemorySize;

  // Set up the callback record, so that the callback functions can keep
  // track of the decompression progress.

  CallbackParameters.inputFile = dataFileHandle;
  CallbackParameters.outputPntr = (LPBYTE) MemoryPntr;
  CallbackParameters.compressedDataRemaining =
    ItemPntr->sourceData.dataFile.compressedSize;
  CallbackParameters.skipDecompressedRemaining = StartOffset;
  CallbackParameters.decompressedDataRemaining = AmountToLoad;
  CallbackParameters.timeToStop = FALSE;

  // Seek to the start of the decompressed data.  Note that we have to
  // always start at the beginning of the compressed data even when the
  // StartOffset is non-zero since we can't tell where the end of the
  // skipped over part is in the compressed data (and the decompressor
  // also needs to see all the prior data anyways to work properly).
  // Instead, we decompress the whole thing (or less if the user's
  // buffer is too small), and just don't save the initial StartOffset
  // number of bytes.

  if (0 != fseek (dataFileHandle,
  ItemPntr->sourceData.dataFile.offset, SEEK_SET))
    goto ErrorExit;

  ErrorCode = explode (LI_DATA_ReadCompressedData,
    LI_DATA_WriteUncompressedData, WorkBuffer, &CallbackParameters);

  if (ErrorCode == CMP_INVALID_DICTSIZE ||
  ErrorCode == CMP_INVALID_MODE ||
  ErrorCode == CMP_BAD_DATA)
    goto ErrorExit; // Corrupt data detected.

  if (CallbackParameters.decompressedDataRemaining != 0)
    goto ErrorExit; // Stopped in the middle of reading, IO error?

  goto NormalExit; // Successful!


ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::loadDataForItem: "
    "Error while trying to load data item at "
    "offset %u in data file \"%s\".\r\n",
    (unsigned int) ItemPntr->sourceData.dataFile.offset,
    sourceName);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif // CE_ARTLIB_EnableDebugMode

  AmountToLoad = 0; // Signal that an error happened.

NormalExit:
  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSourceCriticalSectionPntr);
  #endif

  return AmountToLoad;
}



/*****************************************************************************
 * These data file items are read-only, don't need to do anything to unload
 * them.
 */

BOOL LI_DATA_DataFileSourceClass::unloadDataForItem (LI_DATA_ItemPointer ItemPntr,
void *MemoryPntr, UNS32 MemorySize)
{
#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::unloadDataForItem: "
      "Called with NULL item\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (ItemPntr->dataSource != this)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_DataFileSourceClass::unloadDataForItem: "
      "The item has something else as its data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif // CE_ARTLIB_EnableDebugMode

  return TRUE;
}



/*****************************************************************************
 * Writing a readable description of the data source for the given item to
 * the string.  Should take less than 1024 characters.  It will usually be
 * appended to a string like "Data from " when printed.
 */

void LI_DATA_DataFileSourceClass::describeSourceForItem (
LI_DATA_ItemPointer ItemPntr, char *OutputString)
{
  sprintf (OutputString, "data file \"%s\"", sourceName);
}
#endif // CE_ARTLIB_DataDataFileSource



#if CE_ARTLIB_DataExternalFileSource
/****************************************************************************/
/* I M P L E M E N T A T I O N   O F   LI_DATA_ExternalFileSourceClass      */
/*--------------------------------------------------------------------------*/

/*****************************************************************************
 * Class constructor.  The parent class sets up the critical section etc.
 * All this specific constructor needs to do is to set the debug string
 * name.  Note that there is only one static actual instance of this class,
 * which handles all external file items.
 */

LI_DATA_ExternalFileSourceClass::LI_DATA_ExternalFileSourceClass ()
{
  sourceName = "External File Source";
}



/*****************************************************************************
 * Set up a data item to use a particular external file.  If you specify
 * zero for the NewFileSize then it assumes you want an old file, and it
 * gets the size from the existing file's size and uses your specified
 * data type.  If you specified LE_DATA_DataUnknown then it guesses the
 * data type from the file extension.  If you specify a non-zero NewFileSize
 * then it will create a new file with the given size and it will use the
 * given NewFileType rather than guessing.  Returns TRUE if successful.
 */

BOOL LI_DATA_ExternalFileSourceClass::connectItem (
LI_DATA_ItemPointer ItemPntr, char *ExternalFileName,
LE_DATA_DataType NewFileType, UNS32 NewFileSize)
{
  UNS32             AmountActuallyWritten;
  UNS32             AmountToWrite;
  LE_DATA_DataType  ExtensionType;
  char             *FileExtension;
  FILE             *FileHandle = NULL;
  UNS32             FillLength;
  char             *FillString = "Artech Digital Entertainments 1998 External File Data Source.\r\n$Header: /Artlib_99/ArtLib/L_Data.cpp 62    7/28/99 6:33p Agmsmith $\r\nCompiled on " __DATE__ " at " __TIME__ ".\r\n";
  HANDLE            FindHandle = INVALID_HANDLE_VALUE;
  WIN32_FIND_DATA   FindInfo;
  BOOL              ReturnCode = FALSE;

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::connectItem: "
      "NULL item pointer!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (ExternalFileName == NULL || ExternalFileName[0] == 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::connectItem: "
      "No file name specified!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (ItemPntr->dataSource != NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::connectItem: "
      "The item already has a data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (NewFileType < 0 || NewFileType >= LE_DATA_DataTypeMax)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::connectItem: "
      "NewFileType of %d isn't a valid data type number!\r\n", (int) NewFileType);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif // CE_ARTLIB_EnableDebugMode

  #if CE_ARTLIB_EnableMultitasking
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (dataSourceCriticalSectionPntr);
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

  if (NewFileSize == 0) // Using an existing file?
  {
    if (NewFileType == LE_DATA_DataUnknown)
    {
      // Try and figure out what kind of data is in this file.

      ItemPntr->initialDataType = LE_DATA_DataUserCreated1; // Default.

      FileExtension = strrchr (ExternalFileName, '.');
      if (FileExtension != NULL)
      {
        FileExtension++; // Skip over the period.

        // First check for non-standard extension exceptions,
        // there is some fiddling to make UAP's into TABs due
        // to the old dope sheet and data system way of working.

        if (stricmp (FileExtension, "UAP") == 0)
          ItemPntr->initialDataType = LE_DATA_DataUAP;
        else if (stricmp (FileExtension, "TAB") == 0)
          ItemPntr->initialDataType = LE_DATA_DataDopeTAB;
        else if (stricmp (FileExtension, "TXT") == 0)
          ItemPntr->initialDataType = LE_DATA_DataString;
        else
        {
          // Look through the standard extensions list.

          for (ExtensionType = (LE_DATA_DataType) 1 /* Zero is the NULL kind */;
          ExtensionType < LE_DATA_DataTypeMax;
          ExtensionType = (LE_DATA_DataType) (ExtensionType + 1))
          {
            if (stricmp (LE_DATA_DataTypePrefixes [ExtensionType], FileExtension) == 0)
            {
              ItemPntr->initialDataType = ExtensionType;
              break;
            }
          }
        }
      }
    }
    else // User explicitly specified the data type for the file.
      ItemPntr->initialDataType = NewFileType; // Let the user specify it.

    // Try to find the file size, this also tests if the file exists.

    FindHandle = FindFirstFile (ExternalFileName, &FindInfo);

    if (FindHandle == INVALID_HANDLE_VALUE)
    {
  #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::connectItem: "
        "FindFirst failed, couldn't find the file named \"%s\"?\r\n", ExternalFileName);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
      goto ErrorExit;
    }

    if (FindInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
  #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::connectItem: "
        "The \"%s\" file is really a directory!  We need real files.\r\n",
        ExternalFileName);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
      goto ErrorExit; // This is a directory.
    }

    ItemPntr->initialSize = FindInfo.nFileSizeLow; // Lower 32 bits of file size.
  }
  else // Creating a new file.
  {
    ItemPntr->initialSize = NewFileSize;
    if (NewFileType == LE_DATA_DataUnknown)
      ItemPntr->initialDataType = LE_DATA_DataUserCreated1;
    else
      ItemPntr->initialDataType = NewFileType;

    // See if there is an existing file with the same name.

#if CE_ARTLIB_EnableDebugMode
    FileHandle = fopen (ExternalFileName, "rb");

    if (FileHandle != NULL)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::connectItem: "
        "Overwriting existing file \"%s\" with a new file.\r\n",
        ExternalFileName);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        CE_ARTLIB_DataPromptWhenSavingFiles ?
        LE_ERROR_DebugMsgToFileAndScreen : LE_ERROR_DebugMsgToFile);
      fclose (FileHandle);
    }
#endif

    // Try to create an actual file.

    FileHandle = fopen (ExternalFileName, "wb");

    if (FileHandle == NULL)
    {
  #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::connectItem: "
        "Unable to create new file \"%s\".\r\n",
        ExternalFileName);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
      goto ErrorExit;
    }

    // Try writing to the file, fill the requested amount with garbage data.

    FillLength = strlen (FillString);
    AmountToWrite = NewFileSize;

    while (AmountToWrite >= FillLength)
    {
      AmountActuallyWritten =
        fwrite (FillString, 1, FillLength, FileHandle);
      AmountToWrite -= AmountActuallyWritten;
      if (AmountActuallyWritten != FillLength)
      {
    #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::connectItem: "
          "Wrote only %u bytes when trying to write %u bytes to new file \"%s\".\r\n",
          (unsigned int) AmountActuallyWritten,
          (unsigned int) FillLength,
          ExternalFileName);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
        goto ErrorExit;
      }
    }

    if (AmountToWrite > 0)
    {
      AmountActuallyWritten =
        fwrite (FillString, 1, AmountToWrite, FileHandle);
      if (AmountActuallyWritten != AmountToWrite)
      {
    #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::connectItem: "
          "Wrote only %u bytes when trying to write %u bytes to new file \"%s\".\r\n",
          (unsigned int) AmountActuallyWritten,
          (unsigned int) AmountToWrite,
          ExternalFileName);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
        goto ErrorExit;
      }
      AmountToWrite -= AmountActuallyWritten;
    }

    // Close the file and check to see if the close worked.

    if (fclose (FileHandle) != 0)
    {
      FileHandle = NULL;
    #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::connectItem: "
          "Unable to close new file \"%s\" after writing data to it, is your hard drive full?\r\n",
          (unsigned int) AmountActuallyWritten,
          (unsigned int) AmountToWrite,
          ExternalFileName);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
        goto ErrorExit;
    }
    FileHandle = NULL;
  }

  // Make a copy of the file name and store it in the data item.

  ItemPntr->sourceData.externalFile.fileName = (char *)
    LI_MEMORY_AllocLibraryBlock (strlen (ExternalFileName) + 1);

  if (ItemPntr->sourceData.externalFile.fileName == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::connectItem: "
      "Unable to allocate memory for the \"%s\" name.\r\n", ExternalFileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // Out of memory.
  }

  strcpy (ItemPntr->sourceData.externalFile.fileName, ExternalFileName);

  // Set up the rest of the item, in an unloaded state.

  ItemPntr->currentSize = ItemPntr->initialSize;
  ItemPntr->dataType = ItemPntr->initialDataType;
  ItemPntr->memoryPntr = NULL;
  ItemPntr->dataSource = this;

  // OK, successfully set up the item.

  numberOfItems++;
  ReturnCode = TRUE;
  goto NormalExit;

ErrorExit:
NormalExit:
  if (FindHandle != INVALID_HANDLE_VALUE)
    FindClose (FindHandle);

  if (FileHandle != NULL)
    fclose (FileHandle);

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSourceCriticalSectionPntr);
  #endif

  return ReturnCode;
}



/*****************************************************************************
 * Disconnect the given item.  Basically just forget about it.
 */

BOOL LI_DATA_ExternalFileSourceClass::disconnectItem (LI_DATA_ItemPointer ItemPntr)
{
#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr->dataSource != this)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::disconnectItem: "
      "The item has something else as its data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (ItemPntr->memoryPntr != NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::disconnectItem: "
      "The item has a non-NULL memory pointer, someone didn't unload it!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (numberOfItems == 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::disconnectItem: "
      "The item count is already zero!  What's going on?\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif // CE_ARTLIB_EnableDebugMode

  #if CE_ARTLIB_EnableMultitasking
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (dataSourceCriticalSectionPntr);
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

  // Deallocate the space we allocated for the file name in the item.

  if (ItemPntr->sourceData.externalFile.fileName != NULL)
  {
    LI_MEMORY_DeallocLibraryBlock (ItemPntr->sourceData.externalFile.fileName);
    ItemPntr->sourceData.externalFile.fileName = NULL;
  }

  ItemPntr->dataSource = NULL;

  numberOfItems--;

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSourceCriticalSectionPntr);
  #endif

  return TRUE;
}



/*****************************************************************************
 * Reset the file size since someone may have changed the file (the dope
 * sheet purposely unloads all files then reloads them when it regains
 * control after running the opaquer).  Assume file type is unchanged.
 */

BOOL LI_DATA_ExternalFileSourceClass::updateInitialSize (LI_DATA_ItemPointer ItemPntr)
{
  HANDLE            FindHandle = INVALID_HANDLE_VALUE;
  WIN32_FIND_DATA   FindInfo;
  BOOL              ReturnCode = FALSE;

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::updateInitialSize: "
      "Called with NULL item pointer!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (ItemPntr->dataSource != this)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::updateInitialSize: "
      "The item has something else as its data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif // CE_ARTLIB_EnableDebugMode

  // Find the current size of the file.  It should exist at this point.

  FindHandle =
    FindFirstFile (ItemPntr->sourceData.externalFile.fileName, &FindInfo);

  if (FindHandle == INVALID_HANDLE_VALUE)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::updateInitialSize: "
      "FindFirst failed, couldn't find the file named \"%s\"?\r\n",
      ItemPntr->sourceData.externalFile.fileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  if (FindInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::updateInitialSize: "
      "The \"%s\" file is really a directory!  We need real files.\r\n",
      ItemPntr->sourceData.externalFile.fileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // This is a directory.
  }

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr->initialSize != FindInfo.nFileSizeLow)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::updateInitialSize: "
      "The \"%s\" file size has changed from %d to %d, now reloading it.  "
      "Hope that's what you wanted!\r\n",
      ItemPntr->sourceData.externalFile.fileName,
      (int) ItemPntr->initialSize,
      (int) FindInfo.nFileSizeLow);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

  ItemPntr->initialSize = FindInfo.nFileSizeLow; // Lower 32 bits of file size.
  ReturnCode = TRUE;

ErrorExit:
  if (FindHandle != INVALID_HANDLE_VALUE)
    FindClose (FindHandle);

  return ReturnCode;
}



/*****************************************************************************
 * Load the data for the given item.  This means opening the file, reading in
 * the requested data (including seeking past undesired data) and then closing
 * the file (doesn't leave the file open).  We also reset the file size at
 * this time since someone may have changed the file (the dope sheet purposely
 * unloads all files then reloads them when it regains control after running
 * the opaquer - assume file type is unchanged).  Returns the number of bytes
 * loaded.  Returns zero on failure.
 */

UNS32 LI_DATA_ExternalFileSourceClass::loadDataForItem (LI_DATA_ItemPointer ItemPntr,
void *MemoryPntr, UNS32 MemorySize, UNS32 StartOffset)
{
  UNS32 AmountActuallyRead;
  UNS32 AmountToLoad;
  FILE *FileHandle = NULL;

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::loadDataForItem: "
      "Called with NULL item pointer!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (MemoryPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::loadDataForItem: "
      "Called with NULL MemoryPntr!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (ItemPntr->dataSource != this)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::loadDataForItem: "
      "The item has something else as its data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }
#endif // CE_ARTLIB_EnableDebugMode

  #if CE_ARTLIB_EnableMultitasking
  if (dataSourceCriticalSectionPntr == NULL) return 0; // System is closed.
  EnterCriticalSection (dataSourceCriticalSectionPntr);
  if (dataSourceCriticalSectionPntr == NULL) return 0; // Closed while we waited.
  #endif

  // Calculate the amount of data we need to read.

  if (StartOffset >= ItemPntr->initialSize)
    goto ErrorExit; // No data available at that offset.

  AmountToLoad = ItemPntr->initialSize - StartOffset;
  if (AmountToLoad > MemorySize)
    AmountToLoad = MemorySize;

  if (AmountToLoad <= 0)
    goto ErrorExit; // Nothing to do.

  // Open the file for reading.

  FileHandle = fopen (ItemPntr->sourceData.externalFile.fileName, "rb");

  if (FileHandle == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::loadDataForItem: "
      "Unable to open file named \"%s\".\r\n",
      ItemPntr->sourceData.externalFile.fileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Seek to the start of the desired data.  But only if
  // the StartOffset isn't zero.

  if (StartOffset != 0 && 0 != fseek (FileHandle, StartOffset, SEEK_SET))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::loadDataForItem: "
      "Unable to seek to position %u in file \"%s\".\r\n",
      (unsigned int) StartOffset, ItemPntr->sourceData.externalFile.fileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Read the data.

  AmountActuallyRead = fread (MemoryPntr, 1, AmountToLoad, FileHandle);

  if (AmountActuallyRead == 0)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::loadDataForItem: "
      "Failed to read any data from file \"%s\".\r\n",
      ItemPntr->sourceData.externalFile.fileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // A warning message if we read less than requested (not an error).

#if CE_ARTLIB_EnableDebugMode
  if (AmountActuallyRead != AmountToLoad)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::loadDataForItem: "
      "Only read %u bytes when requested to read %u bytes from file \"%s\".  "
      "It's not an error, but is that what you expected?\r\n",
      (unsigned int) AmountActuallyRead,
      (unsigned int) AmountToLoad,
      ItemPntr->sourceData.externalFile.fileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

  goto NormalExit; // Successful!


ErrorExit:
  AmountActuallyRead = 0; // Signal that an error happened.

NormalExit:
  if (FileHandle != NULL)
    fclose (FileHandle);

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSourceCriticalSectionPntr);
  #endif

  return AmountActuallyRead;
}



/*****************************************************************************
 * When unloading the data, write it back to the file if it has changed,
 * also prompt the user if there is an existing file.  If it is written,
 * update the initial size and data type to reflect what was written.
 * Returns TRUE if successful.
 */

BOOL LI_DATA_ExternalFileSourceClass::unloadDataForItem (LI_DATA_ItemPointer ItemPntr,
void *MemoryPntr, UNS32 MemorySize)
{
#if CE_ARTLIB_DataPromptWhenSavingFiles
  int   ErrorCode;
  char  TitleString [200];
#endif

  UNS32 AmountActuallyWritten;
  FILE *FileHandle = NULL;
  BOOL  ReturnCode = FALSE;

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::unloadDataForItem: "
      "Called with NULL item pointer!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (MemoryPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::unloadDataForItem: "
      "Called with NULL MemoryPntr!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (ItemPntr->dataSource != this)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::unloadDataForItem: "
      "The item has something else as its data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (ItemPntr->sourceData.externalFile.fileName == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::unloadDataForItem: "
      "The item doesn't have a file name!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }
#endif // CE_ARTLIB_EnableDebugMode

  #if CE_ARTLIB_EnableMultitasking
  if (dataSourceCriticalSectionPntr == NULL) return 0; // System is closed.
  EnterCriticalSection (dataSourceCriticalSectionPntr);
  if (dataSourceCriticalSectionPntr == NULL) return 0; // Closed while we waited.
  #endif

  if (ItemPntr->dirtyData)
  {
#if CE_ARTLIB_DataPromptWhenSavingFiles
    // Ask the user if they want to save their changes.  First stop the
    // animation engine from drawing on the screen while our box is up,
    // this is also safe when this is the animation engine thread.

#if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_EnableSystemSequencer
    if (LI_SEQNCR_CommonRuntimeTreeRoot != NULL)
      EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
#endif

    ShowCursor (TRUE);

    sprintf (LE_ERROR_DebugMessageBuffer,
      "Do you want to save changes (%u bytes, type %s) to file %s?  "
      "Picking NO will discard them and leave the file unchanged.",
      (unsigned int) MemorySize,
      LE_DATA_DataTypeNames[ItemPntr->dataType],
      ItemPntr->sourceData.externalFile.fileName);

#if CE_ARTLIB_EnableSystemMain
    sprintf (TitleString, "%s - Save File?", LE_MAIN_ApplicationNameString);
#else
    sprintf (TitleString, "%s - Save File?", CE_ARTLIB_ApplicationName);
#endif

    ErrorCode = MessageBox (NULL,
      LE_ERROR_DebugMessageBuffer, TitleString,
      MB_YESNO | MB_ICONQUESTION | MB_TASKMODAL);

    ShowCursor (FALSE);

    // Let the animation engine continue.

#if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_EnableSystemSequencer
    if (LI_SEQNCR_CommonRuntimeTreeRoot != NULL)
      LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
#endif

    if (ErrorCode == IDYES)
#else // Not prompting the user, always save.
    if (TRUE)
#endif // CE_ARTLIB_DataPromptWhenSavingFiles
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::unloadDataForItem: "
        "Writing %u bytes of type %s to file %s\r\n",
        (unsigned int) MemorySize,
        LE_DATA_DataTypeNames[ItemPntr->dataType],
        ItemPntr->sourceData.externalFile.fileName);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif

      // Open the file for writing.

      FileHandle = fopen (ItemPntr->sourceData.externalFile.fileName, "wb");

      if (FileHandle == NULL)
      {
    #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::unloadDataForItem: "
          "Unable to open file named \"%s\" for writing.\r\n",
          ItemPntr->sourceData.externalFile.fileName);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
        goto ErrorExit;
      }

      // Write the data.

      AmountActuallyWritten = fwrite (MemoryPntr, 1, MemorySize, FileHandle);

      // Check for write errors.

      if (AmountActuallyWritten != MemorySize)
      {
    #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::unloadDataForItem: "
          "Only wrote %u bytes when requested to write %u bytes to file \"%s\".\r\n",
          (unsigned int) AmountActuallyWritten,
          (unsigned int) MemorySize,
          ItemPntr->sourceData.externalFile.fileName);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
        goto ErrorExit;
      }

      // Close the file and check for errors, in case the disk is full and it
      // finds out when it flushes buffers while closing.

      if (fclose (FileHandle) != 0)
      {
        FileHandle = NULL; // Don't try closing it again.
    #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::unloadDataForItem: "
          "Unable to close the \"%s\" file, is the disk full?\r\n",
          ItemPntr->sourceData.externalFile.fileName);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
        goto ErrorExit;
      }
      FileHandle = NULL; // Was closed.

      // Update initial settings to reflect the new file.
      ItemPntr->initialSize = MemorySize;
      ItemPntr->initialDataType = ItemPntr->dataType;
    }
    else // Discarding data.
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::unloadDataForItem: "
        "Discarding %u bytes of type %s for file %s\r\n",
        (unsigned int) MemorySize,
        LE_DATA_DataTypeNames[ItemPntr->dataType],
        ItemPntr->sourceData.externalFile.fileName);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif
    }

    ItemPntr->dirtyData = FALSE;
  }

  ReturnCode = TRUE;
  goto NormalExit; // Successful!

ErrorExit:
NormalExit:
  if (FileHandle != NULL)
    fclose (FileHandle);

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSourceCriticalSectionPntr);
  #endif

  return ReturnCode;
}



/*****************************************************************************
 * Writes a readable description of the data source for the given item to
 * the string.  Should take less than 1024 characters.  It will usually be
 * appended to a string like "Data from " when printed.
 */

void LI_DATA_ExternalFileSourceClass::describeSourceForItem (
LI_DATA_ItemPointer ItemPntr, char *OutputString)
{
  if (ItemPntr == NULL || ItemPntr->dataSource != this ||
  ItemPntr->sourceData.externalFile.fileName == NULL)
    sprintf (OutputString, "external file without a good name");
  else
    sprintf (OutputString, "external file \"%s\"",
    ItemPntr->sourceData.externalFile.fileName);
}



#if CE_ARTLIB_DataNameHashTable
/*****************************************************************************
 * Returns the name of this item, in this case it is just the file name.
 */

const char *LI_DATA_ExternalFileSourceClass::getItemNameFromItem (
LI_DATA_ItemPointer ItemPntr)
{
  if (ItemPntr == NULL || ItemPntr->dataSource != this ||
  ItemPntr->sourceData.externalFile.fileName == NULL)
    return NULL;
  else
    return ItemPntr->sourceData.externalFile.fileName;
}
#endif // CE_ARTLIB_DataNameHashTable
#endif // CE_ARTLIB_DataExternalFileSource



#if CE_ARTLIB_DataURLSource
/****************************************************************************/
/* I M P L E M E N T A T I O N   O F   LI_DATA_URLSourceClass               */
/*--------------------------------------------------------------------------*/

/*****************************************************************************
 * Class constructor.  The parent class sets up the critical section etc.
 * This specific constructor needs to set the debug string and initialise
 * the WinSock library, if it is available.  Note that there is only one
 * static actual instance of this class, which handles all URL items.
 */

LI_DATA_URLSourceClass::LI_DATA_URLSourceClass ()
{
  int     ErrorCode;
  WSADATA WSAData;

  // Set the string used for debugging this source.

  sourceName = "URL Source";

  // Load the Winsock DLL, searches the standard paths.  If it is loaded,
  // fill in our pointers to functions in Winsock.

  winsockLibraryInstance = LoadLibrary ("WSOCK32");
  if (winsockLibraryInstance == NULL)
  {
    winsockLibraryInstance = LoadLibrary ("WINSOCK");
    if (winsockLibraryInstance == NULL)
      ErrorCode = GetLastError ();
  }

  /* As a last ditch attempt, could try GetModuleHandle.  But we don't. */

  // If WinSock is not available at all on this system, don't report an error,
  // just silently operate, and reject all later data operations.

  if (winsockLibraryInstance == NULL)
    return;

  pfclosesocket = (type_closesocket) GetProcAddress (winsockLibraryInstance, "closesocket");
  pfconnect = (type_connect) GetProcAddress (winsockLibraryInstance, "connect");
  pfgethostbyname = (type_gethostbyname) GetProcAddress (winsockLibraryInstance, "gethostbyname");
  pfhtons = (type_htons) GetProcAddress (winsockLibraryInstance, "htons");
  pfinet_addr = (type_inet_addr) GetProcAddress (winsockLibraryInstance, "inet_addr");
  pfrecv = (type_recv) GetProcAddress (winsockLibraryInstance, "recv");
  pfsend = (type_send) GetProcAddress (winsockLibraryInstance, "send");
  pfsetsockopt = (type_setsockopt) GetProcAddress (winsockLibraryInstance, "setsockopt");
  pfsocket = (type_socket) GetProcAddress (winsockLibraryInstance, "socket");
  pfWSACleanup = (type_WSACleanup) GetProcAddress (winsockLibraryInstance, "WSACleanup");
  pfWSAGetLastError = (type_WSAGetLastError) GetProcAddress (winsockLibraryInstance, "WSAGetLastError");
  pfWSAStartup = (type_WSAStartup) GetProcAddress (winsockLibraryInstance, "WSAStartup");

  if (pfWSAStartup == NULL) /* Safety check, wrong DLL loaded? */
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass: "
      "Unable to find the WSAStartup function in the WinSock DLL, "
      "maybe it isn't the right DLL?\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    ErrorCode = 1;
  }
  else
  {
    // Try to initialize the Winsock system.

    memset (&WSAData, 0, sizeof (WSAData));
    ErrorCode = pfWSAStartup (0x101 /* We need version 1.1 */, &WSAData);

    if (ErrorCode != 0)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass: "
        "Error %d when starting up Winsock.\r\n", ErrorCode);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    }
    else
    {
      /* Did we get version 1.1?  Reject if we didn't. */
      if (LOBYTE (WSAData.wVersion) != 1 || HIBYTE (WSAData.wVersion) != 1)
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf( LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass: "
          "The current Winsock library doesn't support the version 1.1 interface.\r\n");
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        ErrorCode = 2;
        pfWSACleanup ();
      }
    }
  }

  if (ErrorCode == 0)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf( LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass: "
      "Has successfully initialised the WinSock system.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif
  }
  else
  {
    FreeLibrary (winsockLibraryInstance);
    winsockLibraryInstance = NULL;
  }
}



/*****************************************************************************
 * Shut down the WinSock system.  Note that the usual parent class destructor
 * is also called automatically after this function finishes.
 */

LI_DATA_URLSourceClass::~LI_DATA_URLSourceClass ()
{
  int ErrorCode;

  if (winsockLibraryInstance != NULL)
  {
    ErrorCode = pfWSACleanup ();

#if CE_ARTLIB_EnableDebugMode
    if (ErrorCode != 0)
    {
      sprintf( LE_ERROR_DebugMessageBuffer, "~LI_DATA_URLSourceClass: "
        "Error %d while trying to close the WinSock system.  "
        "Were there pending operations?\r\n", ErrorCode);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    }
#endif

    FreeLibrary (winsockLibraryInstance);
    winsockLibraryInstance = NULL;
  }
}



/*****************************************************************************
 * Parses the URL string into a service type (http, ftp, etc), a computer
 * name, a port number (uses default for the service if none) and a resource
 * name.  So, "http://www.Artech.ca:85/index.html" will give you service
 * type HTTP, port 85, resource "/index.html".  Degenerate strings like
 * "http://www.Artech.ca" also work, with the default port being assumed
 * and the default path name of "/" being assumed.  Writes the result into
 * the given strings.  Returns the service type or UNKNOWN_SERVICE if it
 * fails.  Assumes your buffers are big enough.
 */

LI_DATA_URLSourceClass::URLServices LI_DATA_URLSourceClass::parseURL (
const char *URL, char *ComputerNameBuffer, char *ResourceNameBuffer,
int& PortNumber)
{
  const char *ComputerName;
  char        Letter;
  const char *NameEnd;
  int         NameLength;

  if (strnicmp (URL, "http://", 7) != 0)
    return UNKNOWN_SERVICE; // We only know HTTP.

  // Look for the computer name and port number.

  ComputerName = URL + 7;
  NameEnd = ComputerName;

  PortNumber = 80; // Default port for HTTP.

  while (TRUE)
  {
    Letter = *NameEnd;

    if (Letter == '/' || Letter == 0)
      break;

    if (Letter == ':')
    {
      // The part after the ":" is the port number.

      PortNumber = atoi (NameEnd + 1);
      break;
    }

    NameEnd++;
  }

  NameLength = NameEnd - ComputerName;
  if (NameLength <= 0)
    return UNKNOWN_SERVICE; // No name specified.

  strncpy (ComputerNameBuffer, ComputerName, NameLength);
  ComputerNameBuffer[NameLength] = 0; // End of string.

  // Look for the resource name after the "/", this skips over
  // the port number.

  while (TRUE)
  {
    Letter = *NameEnd;
    if (Letter == '/' || Letter == 0)
      break;
    NameEnd++;
  }

  if (*NameEnd == 0)
    strcpy (ResourceNameBuffer, "/"); // No name specified.
  else
    strcpy (ResourceNameBuffer, NameEnd);

  return HTTP_SERVICE;
}



/*****************************************************************************
 * Open a socket connection to the given computer.  Returns INVALID_SOCKET
 * it fails, otherwise returns the socket handle.  The computer name can
 * be a name ("www.artech.ca" or an IP numeric address "172.16.3.2").
 */

SOCKET LI_DATA_URLSourceClass::openSocket (char *ComputerName, int PortNumber)
{
  int         ErrorCode;
  UNS32       FourByteAddress;
  PHOSTENT    HostEntryPntr;
  SOCKET      SocketHandle;
  SOCKADDR_IN SocketInternetAddress;
  int         WinSockBoolean;

  if (winsockLibraryInstance == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::openSocket: "
      "WinSock isn't initialised!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    SocketHandle = INVALID_SOCKET;
    goto ErrorExit;
  }

  // Try to create a socket.

  SocketHandle = pfsocket (AF_INET /* Use Internet address family */,
    SOCK_STREAM /* Want stream TCP based kind (has error correction) */,
    0 /* Use protocol that matches address family */);

  if (SocketHandle == INVALID_SOCKET)
  {
#if CE_ARTLIB_EnableDebugMode
    ErrorCode = pfWSAGetLastError ();
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::openSocket: "
      "Creation of socket failed, code %d.", ErrorCode);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Set the socket option to turn on routing, we want to allow
  // connections to systems outside of our LAN.

  WinSockBoolean = FALSE;

  pfsetsockopt (SocketHandle, SOL_SOCKET, SO_DONTROUTE,
    (char *) &WinSockBoolean, sizeof (WinSockBoolean));

  // Find the address of the entity we want to connect to.  Try looking
  // it up in the host name database first.  If not found, try interpreting
  // it as a 1.2.3.4 format address.

  HostEntryPntr = pfgethostbyname (ComputerName);

  if (HostEntryPntr == NULL)
    ErrorCode = pfWSAGetLastError ();
  else
  {
    memcpy (&SocketInternetAddress.sin_addr, HostEntryPntr->h_addr,
      HostEntryPntr->h_length);
    ErrorCode = 0;
  }

  SocketInternetAddress.sin_family = AF_INET;
  SocketInternetAddress.sin_port = pfhtons (PortNumber);

  if (ErrorCode != 0)
  {
    // Not in hosts file, probably a raw address.

    FourByteAddress = pfinet_addr (ComputerName);
    if (FourByteAddress != INADDR_NONE) // Converted ok?
    {
      memset (&SocketInternetAddress, 0, sizeof (SocketInternetAddress));
      SocketInternetAddress.sin_family = AF_INET;
      SocketInternetAddress.sin_port = pfhtons (PortNumber);
      SocketInternetAddress.sin_addr.s_addr = FourByteAddress;
      ErrorCode = 0;
    }
  }

  if (ErrorCode != 0)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::openSocket: "
      "\"%s\" doesn't appear to be a valid Internet address, code %d.",
      ComputerName, ErrorCode);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Now that we have an address, try making the connection.  Maybe we
  // should put up a dialog box here to tell the user to wait?  The
  // connect also binds a default local address to the socket (so we
  // don't have to do a separate bind operation).

  if (pfconnect (SocketHandle, (PSOCKADDR) &SocketInternetAddress,
  sizeof (SocketInternetAddress)) != 0)
  {
#if CE_ARTLIB_EnableDebugMode
    ErrorCode = pfWSAGetLastError ();
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::openSocket: "
      "Unable to connect to \"%s\", code %d.", ComputerName, ErrorCode);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Success!
  goto NormalExit;

ErrorExit:
  if (SocketHandle != INVALID_SOCKET)
  {
    pfclosesocket (SocketHandle);
    SocketHandle = INVALID_SOCKET;
  }

NormalExit:
  return SocketHandle;
}



/*****************************************************************************
 * Set up a data item to use a particular URL with predefined size and type
 * (don't bother obtaining info about the web page, just use what's given).
 * Returns TRUE if successful.
 */

BOOL LI_DATA_URLSourceClass::connectItem (
LI_DATA_ItemPointer ItemPntr, char *URLName,
LE_DATA_DataType PreKnownType, UNS32 PreKnownSize)
{
  int         AmountReceived;
  int         AmountSent;
  char        ComputerNameBuffer [128];
  char        HeaderBuffer [2048];
  SOCKET      MySocket = INVALID_SOCKET;
  int         PortNumber;
  char        ResourceNameBuffer [1024];
  BOOL        ReturnCode = FALSE;
  URLServices ServiceType;
  char       *StringPntr;
  char        TempLine [1300]; // Can include a resource path plus a bit more.

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::connectItem: "
      "NULL item pointer!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (URLName == NULL || URLName[0] == 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::connectItem: "
      "No URL name specified!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (ItemPntr->dataSource != NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::connectItem: "
      "The item already has a data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif // CE_ARTLIB_EnableDebugMode

  // Parse the URL and make sure it is legal.

  ServiceType = parseURL (URLName, ComputerNameBuffer, ResourceNameBuffer,
    PortNumber);

  if (ServiceType != HTTP_SERVICE)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (TempLine, "LI_DATA_URLSourceClass::connectItem: "
      "Unable to parse the URL for \"%s\", perhaps it is not HTTP://?\r\n", URLName);
    LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  // Only allow one internet access at a time.  From this point
  // onwards all errors should go through ErrorExit, not return.

  #if CE_ARTLIB_EnableMultitasking
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (dataSourceCriticalSectionPntr);
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

  // If we don't know what type of data it is,
  // contact the web server to find out.

  if (PreKnownType == LE_DATA_DataUnknown)
  {
    // Open a socket connection to the given computer.

    MySocket = openSocket (ComputerNameBuffer, PortNumber);
    if (MySocket == INVALID_SOCKET)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (TempLine, "LI_DATA_URLSourceClass::connectItem: "
        "Unable to open socket to computer \"%s\" on port %d for URL \"%s\".\r\n",
        ComputerNameBuffer, PortNumber, URLName);
      LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    // Request the header info from the web server.  This will tell us the
    // type and size of the item.

    sprintf (TempLine, "HEAD %s HTTP/1.0\n\n", ResourceNameBuffer);
    AmountSent = pfsend (MySocket, TempLine, strlen (TempLine), 0 /* flags */);

    if (AmountSent == SOCKET_ERROR)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (TempLine, "LI_DATA_URLSourceClass::connectItem: "
        "Send failed when requesting header info for URL \"%s\".\r\n", URLName);
      LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    // Process the header, store any useful data.

    AmountReceived = pfrecv (MySocket,
      HeaderBuffer, sizeof (HeaderBuffer) - 1, 0 /* flags */);

    if (AmountReceived == SOCKET_ERROR || AmountReceived <= 0 ||
    AmountReceived >= sizeof (HeaderBuffer))
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (TempLine, "LI_DATA_URLSourceClass::connectItem: "
        "Recv failed when getting header info for URL \"%s\".\r\n", URLName);
      LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    // Make the received data into a string, all lower case.

    HeaderBuffer [AmountReceived] = 0;
    _strlwr (HeaderBuffer);

    // Find the size of the data, look for the "content-length:" field.

    StringPntr = strstr (HeaderBuffer, StringContentLength);
    if (StringPntr == NULL)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (TempLine, "LI_DATA_URLSourceClass::connectItem: "
        "\"%s\" field isn't present in header info from URL \"%s\".\r\n",
        StringContentLength, URLName);
      LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    ItemPntr->initialSize = atol (StringPntr + strlen (StringContentLength));

    if (ItemPntr->initialSize <= 0)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (TempLine, "LI_DATA_URLSourceClass::connectItem: "
        "\"%s\" field isn't valid, can't determine size for URL \"%s\".\r\n",
        StringContentLength, URLName);
      LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    // Now find out what kind of data we got, based on "content-type:".

    StringPntr = strstr (HeaderBuffer, StringContentType);
    if (StringPntr == NULL)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (TempLine, "LI_DATA_URLSourceClass::connectItem: "
        "\"%s\" field isn't present in header info from URL \"%s\".\r\n",
        StringContentType, URLName);
      LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    // Skip past header and leading spaces.

    StringPntr += strlen (StringContentType);
    while (*StringPntr != 0 && *StringPntr == ' ')
      StringPntr++;

    // Analyse the type of data.

    if (strncmp (StringPntr, "text", 4) == 0)
      ItemPntr->initialDataType = LE_DATA_DataString;
    else if (strncmp (StringPntr, "image/bmp", 9) == 0)
      ItemPntr->initialDataType = LE_DATA_DataBMP;
    else if (strncmp (StringPntr, "audio/x-wav", 11) == 0)
      ItemPntr->initialDataType = LE_DATA_DataWave;
    else if (strncmp (StringPntr, "image/x-artech-tab", 18) == 0)
      ItemPntr->initialDataType = LE_DATA_DataDopeTAB;
    else if (strncmp (StringPntr, "image/x-artech-uap", 18) == 0)
      ItemPntr->initialDataType = LE_DATA_DataUAP;
    else // Don't know what it is, treat it as binary.  Bleeble: look at file name extension.
      ItemPntr->initialDataType = LE_DATA_DataUserCreated1;
  }
  else // Know the data type and size in advance, don't contact web server.
  {
    ItemPntr->initialDataType = PreKnownType;
    ItemPntr->initialSize = PreKnownSize;
  }

  // Looks good.  Make a copy of the URL name and store it in the data item.

  ItemPntr->sourceData.URL.name = (char *)
    LI_MEMORY_AllocLibraryBlock (strlen (URLName) + 1);

  if (ItemPntr->sourceData.URL.name == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (TempLine, "LI_DATA_URLSourceClass::connectItem: "
      "Unable to allocate memory for the \"%s\" name.\r\n", URLName);
    LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // Out of memory.
  }

  strcpy (ItemPntr->sourceData.URL.name, URLName);

  // Set up the rest of the item, in an unloaded state.

  ItemPntr->currentSize = ItemPntr->initialSize;
  ItemPntr->dataType = ItemPntr->initialDataType;
  ItemPntr->memoryPntr = NULL;
  ItemPntr->dataSource = this;

  // OK, successfully set up the item.

  numberOfItems++;
  ReturnCode = TRUE;
  goto NormalExit;

ErrorExit:
NormalExit:
  if (MySocket != INVALID_SOCKET)
    pfclosesocket (MySocket);

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSourceCriticalSectionPntr);
  #endif

  return ReturnCode;
}



/*****************************************************************************
 * Disconnect the given item.  Basically just forget about it.
 */

BOOL LI_DATA_URLSourceClass::disconnectItem (LI_DATA_ItemPointer ItemPntr)
{
#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr->dataSource != this)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::disconnectItem: "
      "The item has something else as its data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (ItemPntr->memoryPntr != NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::disconnectItem: "
      "The item has a non-NULL memory pointer, someone didn't unload it!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (numberOfItems == 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::disconnectItem: "
      "The item count is already zero!  What's going on?\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif // CE_ARTLIB_EnableDebugMode

  #if CE_ARTLIB_EnableMultitasking
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (dataSourceCriticalSectionPntr);
  if (dataSourceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

  // Deallocate the space we allocated for the URL name in the item.

  if (ItemPntr->sourceData.URL.name != NULL)
  {
    LI_MEMORY_DeallocLibraryBlock (ItemPntr->sourceData.URL.name);
    ItemPntr->sourceData.URL.name = NULL;
  }

  ItemPntr->dataSource = NULL;

  numberOfItems--;

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSourceCriticalSectionPntr);
  #endif

  return TRUE;
}



/*****************************************************************************
 * Load the data for the given item.  This means opening the URL, reading in
 * the requested data (including seeking past undesired data) and then closing
 * the URL (doesn't leave the socket open).  Returns the number of bytes
 * loaded (can be less than requested).  Returns zero on failure.
 */

UNS32 LI_DATA_URLSourceClass::loadDataForItem (LI_DATA_ItemPointer ItemPntr,
void *MemoryPntr, UNS32 MemorySize, UNS32 StartOffset)
{
  UNS32       AmountActuallyRead = 0;
  int         AmountReceived;
  int         AmountSent;
  UNS32       AmountToLoad;
  int         AmountToReceive;
  char        ComputerNameBuffer [128];
  char        HeaderBuffer [2048];
  char       *MemoryBytePntr = (char *) MemoryPntr;
  SOCKET      MySocket = INVALID_SOCKET;
  char       *PastDataEndPntr;
  int         PortNumber;
  char        ResourceNameBuffer [1024];
  URLServices ServiceType;
  char       *StringPntr;
  char        TempLine [1300]; // Can include a resource path plus a bit more.

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::loadDataForItem: "
      "Called with NULL item pointer!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (MemoryPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::loadDataForItem: "
      "Called with NULL MemoryPntr!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (ItemPntr->dataSource != this)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::loadDataForItem: "
      "The item has something else as its data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }
#endif // CE_ARTLIB_EnableDebugMode

  // Calculate the amount of data we need to read.

  if (StartOffset >= ItemPntr->initialSize)
    return 0; // No data available at that offset.

  AmountToLoad = ItemPntr->initialSize - StartOffset;
  if (AmountToLoad > MemorySize)
    AmountToLoad = MemorySize;

  if (AmountToLoad <= 0)
    return 0;

  // Parse the URL (should be legal by now).

  ServiceType = parseURL (ItemPntr->sourceData.URL.name,
    ComputerNameBuffer, ResourceNameBuffer, PortNumber);

  if (ServiceType != HTTP_SERVICE)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (TempLine, "LI_DATA_URLSourceClass::loadDataForItem: "
      "Unable to parse the URL for \"%s\", perhaps it is not HTTP://?\r\n",
      ItemPntr->sourceData.URL.name);
    LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return 0;
  }

  #if CE_ARTLIB_EnableMultitasking
  if (dataSourceCriticalSectionPntr == NULL) return 0; // System is closed.
  EnterCriticalSection (dataSourceCriticalSectionPntr);
  if (dataSourceCriticalSectionPntr == NULL) return 0; // Closed while we waited.
  #endif

  // Read data from the URL.  Currently we only support the
  // Hypertext protocol (HTTP).  First open the socket.

  MySocket = openSocket (ComputerNameBuffer, PortNumber);
  if (MySocket == INVALID_SOCKET)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (TempLine, "LI_DATA_URLSourceClass::loadDataForItem: "
      "Unable to open socket to computer \"%s\" on port %d for URL \"%s\".\r\n",
      ComputerNameBuffer, PortNumber, ItemPntr->sourceData.URL.name);
    LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Request the whole web page from the web server.

  sprintf (TempLine, "GET %s HTTP/1.0\n\n", ResourceNameBuffer);
  AmountSent = pfsend (MySocket, TempLine, strlen (TempLine), 0 /* flags */);

  if (AmountSent == SOCKET_ERROR)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (TempLine, "LI_DATA_URLSourceClass::loadDataForItem: "
      "Send failed when requesting web page for URL \"%s\".\r\n",
      ItemPntr->sourceData.URL.name);
    LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Get the header and some of the following data (assume that the
  // entire header is less than the size of our buffer).

  AmountReceived = pfrecv (MySocket,
    HeaderBuffer, sizeof (HeaderBuffer), 0 /* flags */);

  if (AmountReceived == SOCKET_ERROR || AmountReceived <= 0 ||
  AmountReceived > sizeof (HeaderBuffer))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (TempLine, "LI_DATA_URLSourceClass::loadDataForItem: "
      "Recv failed when getting first batch of data for URL \"%s\".\r\n",
      ItemPntr->sourceData.URL.name);
    LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Skip over the header, which ends after a blank line (look for
  // two line feeds in a row).  Ignore the header info, assume it
  // has the same type and length as when we queried it last.

  StringPntr = HeaderBuffer;
  PastDataEndPntr = HeaderBuffer + AmountReceived;

  while (StringPntr < PastDataEndPntr)
  {
    if (StringPntr[0] == '\n')
    {
      if (StringPntr[1] == '\n')
      {
        StringPntr += 2; // Point past second linefeed for UNIX web servers.
        break; // Two line feeds in a row.
      }

      if (StringPntr[1] == '\r' && StringPntr[2] == '\n')
      {
        StringPntr += 3; // Point past LF/CR/LF for MS-DOS web servers.
        break; // Two CR/LF pairs in a row (don't bother to check for first CR).
      }
    }
    StringPntr++;
  }

  if (StringPntr >= PastDataEndPntr)
    StringPntr = HeaderBuffer; // Couldn't find end of header, use everything.

  AmountReceived -= (StringPntr - HeaderBuffer); // Don't count header in size.

  // Copy the remaining data after the header to the user's buffer.
  // But first skip past the user's seek offset.

  if (StartOffset >= (unsigned int) AmountReceived)
    StartOffset -= AmountReceived; // Skip over all data from first read.
  else // Some of the data immediately after the header is useful.
  {
    StringPntr += StartOffset;
    AmountReceived -= StartOffset;
    StartOffset = 0;

    if (AmountToLoad <= (unsigned int) AmountReceived)
    {
      // Initial data after the header contains all of requested data.

      AmountActuallyRead += AmountToLoad;
      memcpy (MemoryBytePntr, StringPntr, AmountToLoad);
      MemoryBytePntr += AmountToLoad;
      AmountToLoad = 0;
    }
    else // All of initial data is consumed, and need to read more.
    {
      AmountActuallyRead += AmountReceived;
      memcpy (MemoryBytePntr, StringPntr, AmountReceived);
      MemoryBytePntr += AmountReceived;
      AmountToLoad -= AmountReceived;
    }
  }

  // Now do pure data reading, have handled the header/data mixed buffer.
  // First skip over data being seeked past.

  while (StartOffset > 0)
  {
    AmountToReceive = StartOffset;
    if (AmountToReceive > sizeof (HeaderBuffer))
      AmountToReceive = sizeof (HeaderBuffer);

    AmountReceived = pfrecv (MySocket,
      HeaderBuffer, AmountToReceive, 0 /* flags */);

    if (AmountReceived == SOCKET_ERROR || AmountReceived <= 0 ||
    AmountReceived > AmountToReceive)
    {
  #if CE_ARTLIB_EnableDebugMode
      sprintf (TempLine, "LI_DATA_URLSourceClass::loadDataForItem: "
        "Recv failed when skipping over data for URL \"%s\".\r\n",
        ItemPntr->sourceData.URL.name);
      LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
      goto ErrorExit;
    }

    StartOffset -= AmountReceived;
  }

  // Now directly fill the user's buffer with the rest of the data.

  while (AmountToLoad > 0)
  {
    AmountReceived = pfrecv (MySocket, MemoryBytePntr, AmountToLoad, 0 /* flags */);

    // Note that a zero size read is ok here, means premature end of file.

    if (AmountReceived == SOCKET_ERROR || AmountReceived < 0 ||
    ((unsigned int) AmountReceived) > AmountToLoad)
    {
  #if CE_ARTLIB_EnableDebugMode
      sprintf (TempLine, "LI_DATA_URLSourceClass::loadDataForItem: "
        "Recv failed when bulk loading data for URL \"%s\".\r\n",
        ItemPntr->sourceData.URL.name);
      LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
      goto ErrorExit;
    }

    if (AmountReceived == 0)
      break; // Hit end of file.

    MemoryBytePntr += AmountReceived;
    AmountActuallyRead += AmountReceived;
    AmountToLoad -= AmountReceived;
  }

  // A warning message if we read less than requested (not an error).

#if CE_ARTLIB_EnableDebugMode
  if (AmountActuallyRead != MemorySize)
  {
    sprintf (TempLine, "LI_DATA_URLSourceClass::loadDataForItem: "
      "Only read %u bytes when requested to read %u bytes from URL \"%s\".  "
      "It's not an error, but is that what you expected?\r\n",
      (unsigned int) AmountActuallyRead,
      (unsigned int) MemorySize,
      ItemPntr->sourceData.URL.name);
    LE_ERROR_DebugMsg (TempLine, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

  goto NormalExit; // Successful!


ErrorExit:
  AmountActuallyRead = 0; // Signal that an error happened.

NormalExit:
  if (MySocket != INVALID_SOCKET)
    pfclosesocket (MySocket);

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSourceCriticalSectionPntr);
  #endif

  return AmountActuallyRead;
}



/*****************************************************************************
 * When unloading the data, just discard it.  We don't support writing.
 * Returns TRUE if successful.
 */

BOOL LI_DATA_URLSourceClass::unloadDataForItem (LI_DATA_ItemPointer ItemPntr,
void *MemoryPntr, UNS32 MemorySize)
{
#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::unloadDataForItem: "
      "Called with NULL item pointer!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (MemoryPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::unloadDataForItem: "
      "Called with NULL MemoryPntr!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (ItemPntr->dataSource != this)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::unloadDataForItem: "
      "The item has something else as its data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }

  if (ItemPntr->sourceData.URL.name == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_URLSourceClass::unloadDataForItem: "
      "The item doesn't have a URL name!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }
#endif // CE_ARTLIB_EnableDebugMode

  // We don't actually do any writing...

  ItemPntr->dirtyData = FALSE;
  return TRUE;
}



/*****************************************************************************
 * Writes a readable description of the data source for the given item to
 * the string.  Should take less than 1024 characters.  It will usually be
 * appended to a string like "Data from " when printed.
 */

void LI_DATA_URLSourceClass::describeSourceForItem (
LI_DATA_ItemPointer ItemPntr, char *OutputString)
{
  if (ItemPntr == NULL || ItemPntr->dataSource != this ||
  ItemPntr->sourceData.URL.name == NULL)
    sprintf (OutputString, "URL without a good name");
  else
    sprintf (OutputString, "URL \"%s\"",
    ItemPntr->sourceData.URL.name);
}



#if CE_ARTLIB_DataNameHashTable
/*****************************************************************************
 * Returns the name of this item, in this case it is just the URL.
 */

const char *LI_DATA_URLSourceClass::getItemNameFromItem (
LI_DATA_ItemPointer ItemPntr)
{
  if (ItemPntr == NULL || ItemPntr->dataSource != this ||
  ItemPntr->sourceData.URL.name == NULL)
    return NULL;
  else
    return ItemPntr->sourceData.URL.name;
}
#endif // CE_ARTLIB_DataNameHashTable
#endif // CE_ARTLIB_DataURLSource



/****************************************************************************/
/* I M P L E M E N T A T I O N   O F   LI_DATA_Item                         */
/*--------------------------------------------------------------------------*/

#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
/*****************************************************************************
 * Add the given item to the least recently used global list, at the head.
 * Returns TRUE if successful, FALSE if debug mode and the item has non-null
 * pointers which mean it is already in the list (or corrupted).
 *
 * The head of the list points to the first item, then its next pointer points
 * to the next one and so on until the tail is reached.  Previous pointers
 * go in the other direction.
 */

BOOL LI_DATA_ItemRecord::addItemToLRU (void)
{
  LI_DATA_ItemPointer OldHeadItemPntr;
  BOOL                ReturnCode = FALSE;

  #if CE_ARTLIB_EnableMultitasking
  if (LI_DATA_LRUCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (LI_DATA_LRUCriticalSectionPntr);
  if (LI_DATA_LRUCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

#if CE_ARTLIB_EnableDebugMode
  if (nextLRU != NULL || previousLRU != NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::addItemToLRU: "
      "This data item has non-NULL list pointers already!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit;
  }

  if (referenceCount != 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::addItemToLRU: "
      "This data item has a reference count of %u, "
      "it must be zero for items in the list!\r\n",
      (unsigned int) referenceCount);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit;
  }

  if (memoryPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::addItemToLRU: "
      "This data item has a NULL memoryPntr, "
      "ones in the list must have data!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit;
  }

  if (dataSource == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::addItemToLRU: "
      "Item with a NULL data source - probably not a valid data item!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit;
  }
#endif

  nextLRU = OldHeadItemPntr = LI_DATA_LeastRecentlyUsedHead;
  LI_DATA_LeastRecentlyUsedHead = this;

  if (OldHeadItemPntr == NULL)
  {
    // List was empty, this new item is also the new tail.
    #if CE_ARTLIB_EnableDebugMode
    if (LI_DATA_LeastRecentlyUsedTail != NULL)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::addItemToLRU: "
        "Tail of list isn't NULL while the head is NULL?  Something is wrong!\r\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      goto ErrorExit;
    }
    #endif
    LI_DATA_LeastRecentlyUsedTail = this;
  }
  else // There was an item at the head of the list.
  {
    #if CE_ARTLIB_EnableDebugMode
    if (OldHeadItemPntr->previousLRU != NULL)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::addItemToLRU: "
        "Old item at the head of the list is corrupt, has a non-NULL previous pointer.\r\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      goto ErrorExit;
    }
    #endif
    OldHeadItemPntr->previousLRU = this;
  }

  // previousLRU = NULL; /* It was already NULL on entry. */

  #if CE_ARTLIB_EnableDebugMode
  NumberOfItemsInLeastRecentlyUsedList++;
  #endif

  ReturnCode = TRUE; // Successfully added the item!

#if CE_ARTLIB_EnableDebugMode
ErrorExit: // Error exit is only used in debug mode.
#endif

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (LI_DATA_LRUCriticalSectionPntr);
  #endif

  return ReturnCode;
}
#endif // CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded



#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
/*****************************************************************************
 * Remove the given item from the least recently used global list, taking
 * it off from wherever it is in the list.  Returns TRUE if successful,
 * FALSE if something is wrong.
 *
 * The head of the list points to the first item, then its next pointer points
 * to the next one and so on until the tail is reached.  Previous pointers
 * go in the other direction.
 */

BOOL LI_DATA_ItemRecord::removeItemFromLRU (void)
{
  BOOL ReturnCode = FALSE;

  #if CE_ARTLIB_EnableMultitasking
  if (LI_DATA_LRUCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (LI_DATA_LRUCriticalSectionPntr);
  if (LI_DATA_LRUCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

#if CE_ARTLIB_EnableDebugMode
  if (dataSource == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::removeItemFromLRU: "
      "Item with a NULL data source - probably not a valid data item, "
      "so it probably is not in the LRU list!  Please fix!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit;
  }

  if (memoryPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::removeItemFromLRU: "
      "This data item has a NULL memoryPntr, "
      "ones in the list must have data!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit;
  }

  if (referenceCount != 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::removeItemFromLRU: "
      "Item with a non-zero (%u) reference count, "
      "it should not be in the LRU list!\r\n",
      (unsigned int) referenceCount);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit;
  }
#endif

  if (nextLRU == NULL) // If this is the last one in the list.
  {
    #if CE_ARTLIB_EnableDebugMode
    if (LI_DATA_LeastRecentlyUsedTail != this)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::removeItemFromLRU: "
        "Item with no next pointer should be tail of the list, but it isn't.  Something is wrong!\r\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      goto ErrorExit;
    }
    #endif
    LI_DATA_LeastRecentlyUsedTail = previousLRU;
  }
  else // Somewhere in the middle.
    nextLRU->previousLRU = previousLRU;

  if (previousLRU == NULL) // If this is the first item in the list.
  {
    #if CE_ARTLIB_EnableDebugMode
    if (LI_DATA_LeastRecentlyUsedHead != this)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::removeItemFromLRU: "
        "Item with no previous pointer should be head of the list, but it isn't.  Something is wrong!\r\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      goto ErrorExit;
    }
    #endif
    LI_DATA_LeastRecentlyUsedHead = nextLRU;
  }
  else // Somewhere in the middle.
    previousLRU->nextLRU = nextLRU;

  nextLRU = NULL;
  previousLRU = NULL;

#if CE_ARTLIB_EnableDebugMode
  if (NumberOfItemsInLeastRecentlyUsedList == 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::removeItemFromLRU: "
      "The count says there are no items, yet we are removing one.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
  else
    NumberOfItemsInLeastRecentlyUsedList--;  // Note that this is only in debug mode.

  if (NumberOfItemsInLeastRecentlyUsedList == 0 &&
  (LI_DATA_LeastRecentlyUsedTail != NULL || LI_DATA_LeastRecentlyUsedHead != NULL))
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::removeItemFromLRU: "
      "The count says there are no items, yet the list pointers aren't NULL.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif // CE_ARTLIB_EnableDebugMode

  ReturnCode = TRUE; // Successfully removed the item!

#if CE_ARTLIB_EnableDebugMode
ErrorExit: // Error exit is only used in debug mode.
#endif

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (LI_DATA_LRUCriticalSectionPntr);
  #endif

  return ReturnCode;
}
#endif // CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded



#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
/*****************************************************************************
 * Remove the given item from the middle of the least recently used global
 * list and add it back to the head, but only if the reference count is
 * zero (meaning it is in the LRU).  This is usually done whenever the
 * item is accessed.  Returns TRUE if successful, FALSE if something is wrong.
 */

BOOL LI_DATA_ItemRecord::moveItemToHeadOfLRU (void)
{
  BOOL  ReturnCode = FALSE;

#if CE_ARTLIB_EnableMultitasking
  if (referenceCount != 0 || memoryPntr == NULL)
    return TRUE; // Quick exit test to avoid critical section overhead.

  if (LI_DATA_LRUCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (LI_DATA_LRUCriticalSectionPntr);
  if (LI_DATA_LRUCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
#endif

  // Is the item still in the least recently used list?

  if (referenceCount == 0 && memoryPntr != NULL)
  {
    // Still in the LRU list (may have been moved out during the critical
    // section wait so we need to retest the reference count and it has
    // to be a volatile variable).

    if (this != LI_DATA_LeastRecentlyUsedHead)
    {
      // Not already at the head of the list.

      if (!removeItemFromLRU ())
        goto ErrorExit;

      if (!addItemToLRU ())
        goto ErrorExit;
    }
  }

  ReturnCode = TRUE;
  goto NormalExit;

ErrorExit:
  // No need to print error messages since sub-functions have done it.

NormalExit:
  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (LI_DATA_LRUCriticalSectionPntr);
  #endif

  return ReturnCode;
}
#endif // CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded



/*****************************************************************************
 * Increments the reference count, and if it was zero before, it
 * also removes the item from the least recently used list.  Note
 * that it doesn't load the item.  Returns TRUE if successful.
 */

BOOL LI_DATA_ItemRecord::addRef (void)
{
  BOOL  ReturnCode = FALSE;

  #if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  if (LI_DATA_LRUCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (LI_DATA_LRUCriticalSectionPntr);
  if (LI_DATA_LRUCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

#if CE_ARTLIB_EnableDebugMode
  if (dataSource == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::addRef: "
      "This item doesn't have a data source, something is wrong!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit;
  }
#endif // CE_ARTLIB_EnableDebugMode

  if (memoryPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::addRef: "
      "Someone beat you to this item, unloading it before the AddRef "
      "succeeded: memoryPntr is NULL.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif // CE_ARTLIB_EnableDebugMode
    goto ErrorExit;
  }

  // If this is the first addRef, remove the item from the least recently
  // used list.  No need to have it there since items with a non-zero
  // reference count can't be unloaded.

#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  if (referenceCount == 0)
  {
    if (!removeItemFromLRU ())
      goto ErrorExit;
  }
#endif

  referenceCount++;

  // Did the counter overflow and wrap around to zero?

#if CE_ARTLIB_EnableDebugMode
  if (referenceCount == 0)
  {
    // Rather than pegging the count at the max and issuing this
    // message repeatedly, skip over zero.

    referenceCount++;

    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::addRef: "
      "Reference counter has overflowed (wrapped around to zero).  "
      "Fix it or your game will sporadically not work in release mode!  "
      "Or debug mode!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit;
  }
#endif // CE_ARTLIB_EnableDebugMode

  ReturnCode = TRUE; // Success!

ErrorExit:
  #if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  LeaveCriticalSection (LI_DATA_LRUCriticalSectionPntr);
  #endif

  return ReturnCode;
}



/*****************************************************************************
 * Decrements the reference count, and if it becomes zero, it
 * also adds the item from the least recently used list.
 * Returns TRUE if successful.
 */

BOOL LI_DATA_ItemRecord::removeRef (void)
{
  BOOL  ReturnCode = FALSE;

  #if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  if (LI_DATA_LRUCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (LI_DATA_LRUCriticalSectionPntr);
  if (LI_DATA_LRUCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

#if CE_ARTLIB_EnableDebugMode
  if (dataSource == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::removeRef: "
      "This item doesn't have a data source, something is wrong!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit;
  }
#endif // CE_ARTLIB_EnableDebugMode

  // Is the reference count already zero?  Shouldn't be removing references.

  if (referenceCount == 0)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::removeRef: "
      "Reference counter is already zero!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  if (memoryPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::removeRef: "
      "This item isn't loaded, memoryPntr is NULL.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  referenceCount--;

  // If this is the last removeRef, add the item to the least recently
  // used list, since it now can be unloaded

#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  if (referenceCount == 0)
  {
    if (!(addItemToLRU ()))
      goto ErrorExit;
  }
#endif

  ReturnCode = TRUE; // Success!

ErrorExit:
  #if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  LeaveCriticalSection (LI_DATA_LRUCriticalSectionPntr);
  #endif

  return ReturnCode;
}



/*****************************************************************************
 * Disconnect this item from its data source.  Returns TRUE if successful.
 * The item should be unloaded before this is called.
 */

BOOL LI_DATA_ItemRecord::disconnectFromDataSource (void)
{
#if CE_ARTLIB_DataNameHashTable
  const char *ItemNamePntr;
#endif // CE_ARTLIB_DataNameHashTable
  BOOL  ReturnCode = FALSE;

#if CE_ARTLIB_EnableDebugMode
  if (dataSource == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::disconnectFromDataSource: "
      "This item doesn't have a data source, something is wrong!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit;
  }

  if (memoryPntr != NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::disconnectFromDataSource: "
      "The item has a non-NULL memory pointer, someone didn't unload it!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit;
  }
#endif // CE_ARTLIB_EnableDebugMode

#if CE_ARTLIB_DataNameHashTable
  ItemNamePntr = getItemName ();
  if (ItemNamePntr != NULL)
    LE_DATA_RemoveNameFromIndex (ItemNamePntr);
#endif // CE_ARTLIB_DataNameHashTable

  if (!dataSource->disconnectItem (this))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::disconnectFromDataSource: "
      "Unable to disconnect the item.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif // CE_ARTLIB_EnableDebugMode
    goto ErrorExit;
  }

  ReturnCode = TRUE; // Successful.

ErrorExit:
  return ReturnCode;
}



/*****************************************************************************
 * Writes a text description of the data source for this item to
 * the given string.  Keeps it under 1024 characters.  Exists even
 * in the non-debug version so that fatal errors can print out a
 * description of the data item.  Will usually appear after a
 * string saying "Data from ".
 */

void LI_DATA_ItemRecord::describeItemSource (char *OutputString)
{
  if (dataSource == NULL)
    strcpy (OutputString, "NULL data source");
  else
    dataSource->describeSourceForItem (this, OutputString);
}



#if CE_ARTLIB_DataNameHashTable
/*****************************************************************************
 * Returns the file name or URL or other name of this data item, used
 * in the name hash table.  Returns NULL for items with no name.
 * Goes through the data source to actually get the name out of the
 * item record.
 */

const char * LI_DATA_ItemRecord::getItemName (void)
{
  if (dataSource == NULL)
    return NULL;
  else
    return dataSource->getItemNameFromItem (this);
}
#endif // CE_ARTLIB_DataNameHashTable



/*****************************************************************************
 * Allocates some memory for the data, then uses the item's data source
 * to get the data for the item, then does the post-load processing and
 * adds the item to the least recently used list.
 * Returns TRUE if successful, FALSE if it fails.
 */

BOOL LI_DATA_ItemRecord::loadItem (void)
{
  const int                   MaxRetryCount = (CE_ARTLIB_EnableMultitasking ? 3 : 1);
  void                       *ProcessedMemoryPntr;
  LI_DATA_PrePostLoadFunction ProcessingFunction;
  int                         RetryCount;
  BOOL                        ReturnCode = FALSE;
  static UNS32                SerialNumberForLoads = 1;
  void                       *TempMemoryPntr = NULL;

#if CE_ARTLIB_EnableDebugMode
  if (dataSource == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::loadItem: "
      "No data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (initialSize == 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::loadItem: "
      "Initial size of data item is zero.  That won't work!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif

  // External files can change size while unloaded (some other program
  // like the opaquer changes it), refresh the size.

#if CE_ARTLIB_DataExternalFileSource
  if (dataSource == LI_DATA_GlobalRuntimeExternalFileDataSource)
  {
    if (!LI_DATA_GlobalRuntimeExternalFileDataSource->updateInitialSize (this))
      return FALSE; // Something went wrong, file deleted?
  }
#endif

  // Attempt to unload data items until there is the given amount
  // of free space.  May fail, in which case our memory allocation
  // attempt may fail too.  Do the unloading outside the critical
  // section to avoid the deadlock where some other thread loading
  // some other item tries to unload an item from this data source
  // but is unable to since we have locked this data source's critical
  // section, and then we try to unload things from the other data
  // source and wait for it while it waits for us.  This will mean
  // sometimes not unloading enough or sometimes doing too much
  // unloading, but that's why there is a loop here.

  for (RetryCount = 0; RetryCount < MaxRetryCount; RetryCount++)
  {
#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
    LI_DATA_UnloadForFreeSpace (initialSize);
#endif

    // OK, now turn on the critical section so that we can safely check
    // the data item in case some other thread is trying to load the
    // very same item right now.

    #if CE_ARTLIB_EnableMultitasking
    if (dataSource->dataSourceCriticalSectionPntr == NULL) return FALSE; // System is closed.
    EnterCriticalSection (dataSource->dataSourceCriticalSectionPntr);
    if (dataSource->dataSourceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
    #endif

    if (memoryPntr != NULL)
    {
      // Someone else loaded the item while we were waiting at the critical
      // section entry, or even before this function got called.  In the
      // non-multitasking version this can't happen.

  #if CE_ARTLIB_EnableMultitasking
      ReturnCode = TRUE; // The data is loaded, not by us, but it is loaded.
      #if CE_ARTLIB_EnableDebugMode
      strcpy (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::loadItem: "
        "Hey!  Some other thread loaded a data item while we were trying "
        "to load it.  Not a problem, just thought you'd like to know!  From ");
      describeItemSource (LE_ERROR_DebugMessageBuffer + strlen (LE_ERROR_DebugMessageBuffer));
      #if CE_ARTLIB_DataNameHashTable
      if (getItemName () != NULL)
      {
        strcat (LE_ERROR_DebugMessageBuffer, ", ");
        strcat (LE_ERROR_DebugMessageBuffer, getItemName ());
      }
      #endif // CE_ARTLIB_DataNameHashTable
      strcat (LE_ERROR_DebugMessageBuffer, ".\r\n");
      // Note - don't display on screen since we are in a critcal section
      // which means the sequencer will lock up while we wait for it.
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
      #endif // CE_ARTLIB_EnableDebugMode
      goto NormalExit;
  #else
      #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::loadItem: "
        "Item at address 0x%08X has a memoryPntr of 0x%08X, "
        "should be NULL before loading!\r\n",
        (unsigned int) this, (unsigned int) memoryPntr);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      #endif // CE_ARTLIB_EnableDebugMode
      goto ErrorExit;
  #endif // CE_ARTLIB_EnableMultitasking
    }

    TempMemoryPntr = LI_MEMORY_AllocDataBlock (initialSize);

    if (TempMemoryPntr != NULL)
      break; // Success!  Allocated our memory.  Stay in the critical section.

    #if CE_ARTLIB_EnableMultitasking
    LeaveCriticalSection (dataSource->dataSourceCriticalSectionPntr);
    #endif
  }

  if (TempMemoryPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::loadItem: "
      "Unable to allocate %u bytes for the item's data after %u attempts.  "
      "Data system is currently using %u bytes from the data pool and the "
      "maximum limit is %u\r\n",
      (unsigned int) initialSize,
      (unsigned int) MaxRetryCount,
      (unsigned int) LI_MEMORY_GetDataTotalUsed (),
      (unsigned int) LE_DATA_DataPoolMemoryLimit);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE; // Not in critical section, can just return here.
  }

  // OK, still in the data source's critical section here so that nobody else
  // can interfere with setting up this new data item.  Now set the data type.

  dataType = initialDataType;

  // Load the data.

  currentSize = dataSource->loadDataForItem (this, TempMemoryPntr,
    initialSize, 0 /* Start offset */);

  if (currentSize == 0)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::loadItem: "
      "Unable to load the data item's %u bytes of data, I/O error?\r\n",
      (unsigned int) initialSize);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // Load failed.
  }

#if CE_ARTLIB_EnableDebugMode
  if (currentSize != initialSize)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::loadItem: "
      "Item claims to have %u bytes, but we only loaded %u bytes.  Oh well...\r\n",
      (unsigned int) initialSize, (unsigned int) currentSize);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

  // Do the post-load processing.  We want the type which corresponds to the data
  // we just loaded, since the item's type may have been changed in the past.
  // The processing may change the size of the item, modifying currentSize,
  // and the dataType too.

  if (dataType >= 0 && dataType < LE_DATA_DataTypeMax)
  {
    ProcessingFunction = LE_DATA_PostLoadFunctions [dataType];
    if (ProcessingFunction != NULL)
    {
      ProcessedMemoryPntr = ProcessingFunction (this, TempMemoryPntr);
      if (ProcessedMemoryPntr == NULL)
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::loadItem: "
          "Post-load processing has failed.\r\n");
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        goto ErrorExit; // Post-load processing failed.
      }

      // If the post-load processing allocated its own memory, get rid of
      // the raw input data buffer and use the processed data.  Presumably
      // the processing function has updated the item's currentSize field
      // too, and maybe changed the dataType.  And used the data pool for
      // its allocation.

      if (ProcessedMemoryPntr != TempMemoryPntr)
      {
#if CE_ARTLIB_PlatformWin95
        LE_MEMORY_TickleFlush ();
#endif
        LI_MEMORY_DeallocDataBlock (TempMemoryPntr);
        TempMemoryPntr = ProcessedMemoryPntr;
      }
    }
  }

  // A little sanity check for buggy processing code.

#if CE_ARTLIB_EnableDebugMode
  if (memoryPntr != NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::loadItem: "
      "Someone has set the item's memoryPntr to 0x%08X, it should still "
      "be NULL, maybe the post-load processing is the guilty party?\r\n",
      (unsigned int) memoryPntr);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit;
  }
#endif // CE_ARTLIB_EnableDebugMode

  // Set up the various other fields in the newly loaded item.

  referenceCount = 0;
  dirtyData = FALSE;

#if CE_ARTLIB_EnableDebugMode  && CE_ARTLIB_DataCollectUsageStatistics
  timesLoaded++;
  #if CE_ARTLIB_EnableSystemTimers
  timeOfLastLoad = LE_TIME_TickCount;
  #endif
  if (serialNumberOfFirstLoad == 0)
    serialNumberOfFirstLoad = SerialNumberForLoads++;
#endif // CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics

  // Fix a Windows 95 (in later releases) quirk where it swaps out
  // the data right after loading it in.

#if CE_ARTLIB_PlatformWin95
  LE_MEMORY_TickleRange (TempMemoryPntr, currentSize);
#endif

  // Now enable the item, add it to the least recently used list and
  // set its memoryPntr to non-NULL all in one atomic operation (so
  // that some other thread doesn't find the item and try to use it
  // before it is in the LRU list - since using it moves it to the
  // top of the list).

#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded && CE_ARTLIB_EnableMultitasking
  if (LI_DATA_LRUCriticalSectionPntr == NULL) goto ErrorExit; // System is closed.
  EnterCriticalSection (LI_DATA_LRUCriticalSectionPntr);
  if (LI_DATA_LRUCriticalSectionPntr == NULL) goto ErrorExit; // Closed while we waited.
#endif

  memoryPntr = TempMemoryPntr; // Enable the data item now!

#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  addItemToLRU ();  // Now it is in the least recently used list.
#endif

#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded && CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (LI_DATA_LRUCriticalSectionPntr);
#endif

  ReturnCode = TRUE; // Success!
  goto NormalExit;

ErrorExit: // Undo partial allocations.

  if (TempMemoryPntr != NULL)
  {
#if CE_ARTLIB_PlatformWin95
    LE_MEMORY_TickleFlush ();
#endif
    LI_MEMORY_DeallocDataBlock (TempMemoryPntr);
  }

NormalExit:
  #if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
  if (LI_DATA_DataPoolMaxUsed < LI_MEMORY_GetDataTotalUsed ())
    LI_DATA_DataPoolMaxUsed = LI_MEMORY_GetDataTotalUsed ();
  #endif

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSource->dataSourceCriticalSectionPntr);
  #endif

  return ReturnCode;
}



/*****************************************************************************
 * Does pre-unload processing.  Then unloadItem gets this item's data
 * source to unload the data (flush dirty data to disk if it is a
 * writeable data item).  Then it deallocates the memory.
 */

BOOL LI_DATA_ItemRecord::unloadItem (void)
{
  BOOL                        DoUnload;
  void                       *CurrentMemoryPntr = NULL;
  void                       *ProcessedMemoryPntr;
  LI_DATA_PrePostLoadFunction ProcessingFunction;
  BOOL                        ReturnCode = FALSE;

#if CE_ARTLIB_EnableDebugMode
  if (dataSource == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::unloadItem: "
      "No data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif

  // Enter the data source's critical section here, so that we don't
  // accidentally unload the same item twice in a row (the second unload
  // will see the NULL memory pointer and give up).

  #if CE_ARTLIB_EnableMultitasking
  if (dataSource->dataSourceCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (dataSource->dataSourceCriticalSectionPntr);
  if (dataSource->dataSourceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

  // Temporarily enter the LRU critical section while we check the reference
  // count and remove the item from the LRU list and set its memoryPntr to
  // NULL so that other threads see it as being in an unloaded state and thus
  // will wait for the data source critical section before trying to load or
  // unload it a second time.

  #if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  if (LI_DATA_LRUCriticalSectionPntr == NULL) goto ErrorExit; // System is closed.
  EnterCriticalSection (LI_DATA_LRUCriticalSectionPntr);
  if (LI_DATA_LRUCriticalSectionPntr == NULL) goto ErrorExit; // Closed while we waited.
  #endif

  DoUnload =
    (referenceCount == 0 && (CurrentMemoryPntr = memoryPntr) != NULL);

  if (DoUnload)
  {
    // Still in memory (someone else didn't unload it on us) and
    // it is still unloadable.  We are responsible for unloading it.
    // So, remove it from the least recently used list and prevent
    // other threads from using it (NULLify the pointer).

    #if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
    removeItemFromLRU ();
    #endif

    #if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
    timesUnloadedBySystem++;
    #endif

    memoryPntr = NULL;
  }

#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded && CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (LI_DATA_LRUCriticalSectionPntr);
#endif

  if (!DoUnload)
  {
    // Someone else unloaded the item while we were waiting at the critical
    // section entry, or did an AddRef, even before this function got called.
    // In the non-multitasking version this can't happen.

    ReturnCode = (memoryPntr == NULL);

#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::unloadItem: "
      "Data item can't be unloaded because it recently had a NULL "
      "memoryPntr (now 0x%08X), or non-zero reference count (now %u).\r\n",
      (unsigned int) memoryPntr, (unsigned int) referenceCount);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif // CE_ARTLIB_EnableDebugMode

    CurrentMemoryPntr = NULL; // So we don't deallocate it at the exit.
    goto NormalExit;
  }

  // Let the tickler know that we are deallocating memory, so it shouldn't
  // tickle things which may no longer be there.

#if CE_ARTLIB_PlatformWin95
  LE_MEMORY_TickleFlush ();
#endif

  // Do the pre-unload processing, based on the current data type and size.
  // Returns a pointer to the processed data (allocated in the data pool),
  // or NULL if something went wrong.  If the returned pointer is different
  // from the current memory pointer, the current memory will be deallocated.

  if (dataType >= 0 && dataType < LE_DATA_DataTypeMax)
  {
    ProcessingFunction = LE_DATA_PreUnloadFunctions [dataType];
    if (ProcessingFunction != NULL)
    {
      ProcessedMemoryPntr = ProcessingFunction (this, CurrentMemoryPntr);
      if (ProcessedMemoryPntr == NULL)
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::unloadItem: "
          "Pre-unload processing has failed.\r\n");
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        goto ErrorExit;
      }

      // If the post-load processing allocated its own memory, get rid of
      // the raw input data buffer and use the processed data.  Presumably
      // the processing function has updated the item's currentSize field
      // too, and maybe changed the dataType (used for converting from
      // internal file formats to public file formats for images etc).
      // And used the data pool for its new allocation.

      if (ProcessedMemoryPntr != NULL &&
      ProcessedMemoryPntr != CurrentMemoryPntr)
      {
        LE_MEMORY_TickleFlush ();
        LI_MEMORY_DeallocDataBlock (CurrentMemoryPntr);
        CurrentMemoryPntr = ProcessedMemoryPntr;
      }
    }
  }

  // Get the data source to unload the data (flush to disk).

  if (!dataSource->unloadDataForItem (this,
  CurrentMemoryPntr, currentSize))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::unloadItem: "
      "Data source was unable to unload the data, data lost.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Statistics updating.

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_EnableSystemTimers && CE_ARTLIB_DataCollectUsageStatistics
  durationInMemory += LE_TIME_TickCount - timeOfLastLoad;
#endif

  ReturnCode = TRUE; // Success!
  goto NormalExit;

ErrorExit:
NormalExit:
  if (CurrentMemoryPntr != NULL)
    LI_MEMORY_DeallocDataBlock (CurrentMemoryPntr);

  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSource->dataSourceCriticalSectionPntr);
  #endif

  return ReturnCode;
}



/*****************************************************************************
 * Resizes the data used by the item to the new size, which can be bigger
 * or smaller.  If the item is loaded, the allocated memory is also
 * reallocated to the new size.  If the item is in an unloaded state,
 * this just changes the current size field.  That doesn't do much
 * since the current size gets changed to the actual amount of data
 * in the data source when it is loaded.  Does not change the dirty flag.
 */

BOOL LI_DATA_ItemRecord::resize (UNS32 NewSize)
{
  BOOL  ReturnCode = FALSE;

#if CE_ARTLIB_EnableDebugMode
  if (NewSize <= 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::resize: "
      "The new size (%u) needs to be larger than zero!\r\n",
      (unsigned int) NewSize);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  if (dataSource == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::resize: "
      "No data source!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif

  // Enter the data source's critical section here, so that other threads
  // trying to unload or load the item don't interfere with the resizing.

  #if CE_ARTLIB_EnableMultitasking
  if (dataSource->dataSourceCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (dataSource->dataSourceCriticalSectionPntr);
  if (dataSource->dataSourceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
  #endif

  // If the item isn't loaded, just set the current size to
  // the new size value.  Of course, as soon as it gets reloaded
  // the size gets set back to the amount of data available.

  if (memoryPntr == NULL)
  {
    currentSize = NewSize;
    ReturnCode = TRUE;
    goto NormalExit;
  }

  // Reallocate the memory to the new size.  If it fails, unload the item.

  if ((memoryPntr = LI_MEMORY_ReallocDataBlock (memoryPntr, NewSize)) == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ItemRecord::resize: "
      "Failed to resize the memory to be %u bytes, was originally %u.  "
      "Data item contents lost!  "
      "Reference count of %u will be forcefully zeroed.  "
      "This is now an ex-data-item, in an unloaded state.\r\n",
      (unsigned int) NewSize,
      (unsigned int) currentSize,
      (unsigned int) referenceCount);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

    // Make this item an unloaded item now.  Which means it is out of
    // the least recently used list and the reference count is wiped out.
    // Note that no unload processing (or data source saving) is done since
    // the data was lost.

    #if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
    if (LI_DATA_LRUCriticalSectionPntr == NULL) goto ErrorExit; // System is closed.
    EnterCriticalSection (LI_DATA_LRUCriticalSectionPntr);
    if (LI_DATA_LRUCriticalSectionPntr == NULL) goto ErrorExit; // Closed while we waited.
    #endif

    #if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
    if (referenceCount == 0)
      removeItemFromLRU (); // Was in the least recently used list.
    #endif

    referenceCount = 0;

    #if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
    timesUnloadedBySystem++;
    #endif

    #if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded && CE_ARTLIB_EnableMultitasking
    LeaveCriticalSection (LI_DATA_LRUCriticalSectionPntr);
    #endif

    goto ErrorExit;
  }

  currentSize = NewSize;
  ReturnCode = TRUE;
  goto NormalExit;

ErrorExit:
NormalExit:
  #if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (dataSource->dataSourceCriticalSectionPntr);
  #endif

  return ReturnCode;
}



#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
/*****************************************************************************
 * Prints a description of the data item's usage history to the error
 * log.  If HintID is specified, it gets printed, but it is optional.
 */

void LI_DATA_ItemRecord::printUsageStatistics (LE_DATA_DataId HintID)
{
  char  NameBuffer [2048];
  char  TempString [3000];

  LE_DATA_DescribeDataID (HintID, NameBuffer, sizeof (NameBuffer));

  sprintf (TempString, "Removing DataID 0x%08X (%08X) loads:%-4d unloads:%-4d "
    "user unloads:%-4d accesses:%-6d time:%-7.2f order:%-4d name:%s\r\n",
    (int) HintID,
    (int) this,
    (int) timesLoaded,
    (int) timesUnloadedBySystem,
    (int) timesUnloadedByUser,
    (int) timesAccessed,
    (double) durationInMemory / (double) CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ,
    (int) serialNumberOfFirstLoad,
    NameBuffer);
  LE_ERROR_DebugMsg (TempString, FALSE);
}
#endif // CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics



/****************************************************************************/
/* G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N              */
/*--------------------------------------------------------------------------*/

#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
/*****************************************************************************
 * Tries to unload data items (least recently used ones) until there
 * is the given amount of space available in the data memory pool.
 * Note that you shouldn't call this function while in a critical section
 * since it needs to unload data which could be from any data source,
 * which could be busy doing something (like loading) and thus get stuck
 * waiting for us to finish while we wait for it to finish.  Returns
 * TRUE if successful at freeing enough space, though that doesn't
 * guarantee that your memory allocation will succeed (some other
 * thread may beat you to it).
 */

BOOL LI_DATA_UnloadForFreeSpace (UNS32 DesiredFreeSpace)
{
  INT32               AmountFree;
  LI_DATA_ItemPointer ItemPntr;
  int                 NumberOfTimesFreeSpaceDidNotImprove = 0;
  INT32               PreviousFree = 0;

  while (TRUE)
  {
    // Find out the free space, use signed math since it can be
    // negative if we over-allocated memory.

    AmountFree = LE_DATA_DataPoolMemoryLimit;
    AmountFree -= LI_MEMORY_GetDataTotalUsed ();

    if (AmountFree >= (INT32) DesiredFreeSpace)
      return TRUE; // Have enough free space now!

    // See if the free space has gone up since the last time,
    // if it fails to go up several times in a row then we
    // give up (LRU list is probably corrupted or something).

    if (AmountFree <= PreviousFree)
    {
      NumberOfTimesFreeSpaceDidNotImprove++;
      if (NumberOfTimesFreeSpaceDidNotImprove > 10)
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_UnloadForFreeSpace: "
          "After %d attempts to unload items (least recently used ones "
          "first), the data pool free space didn't improve, remaining "
          "stuck at %d bytes.  Perhaps the least recently used list is "
          "corrupt?\r\n",
          (int) NumberOfTimesFreeSpaceDidNotImprove, (int) AmountFree);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        return FALSE; // Something is wrong, give up.
      }
    }
    else
      NumberOfTimesFreeSpaceDidNotImprove = 0;

    PreviousFree = AmountFree;

    ItemPntr = LI_DATA_LeastRecentlyUsedTail;

    if (ItemPntr == NULL)
    {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_UnloadForFreeSpace: "
          "No more items available for unloading, can't fill request to "
          "free up space for %u bytes, only have %d available.  Data pool "
          "currently has %u bytes allocated to data (things with non-zero "
          "reference counts).\r\n",
          (unsigned int) DesiredFreeSpace,
          (int) AmountFree,
          (unsigned int) LI_MEMORY_GetDataTotalUsed ());
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return FALSE; // No more items available for unloading.
    }

    // Attempt to unload it, don't care if it fails since we only
    // need to look at the free space count.

    ItemPntr->unloadItem ();
  }
}
#endif // CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded



/*****************************************************************************
 * A private function for finding the item from a DataID, also set to be
 * inline for increased performance in release mode (it is also quite small
 * in Release mode).  Returns the item if it exists, returns NULL if it
 * can't find it (except in Release mode when it returns garbage or crashes).
 */

#if CE_ARTLIB_EnableDebugMode
  static // Debug mode, has a large local variable so it shouldn't really be inline.
#else
  inline // Release mode, can inline this function.
#endif
LI_DATA_ItemPointer LI_DATA_GetItemPntrFromID (LE_DATA_DataId DataID)
{
#if CE_ARTLIB_EnableDebugMode
  char                    Description [2048];
#endif
  LE_DATA_DataGroupIndex  GroupIndex;
  LI_DATA_GroupPointer    GroupPntr;
  LE_DATA_DataItemIndex   ItemIndex;
  LI_DATA_ItemPointer     ItemPntr;

  ItemIndex = LE_DATA_TagFromId (DataID);
  GroupIndex = LE_DATA_FileFromId (DataID);

  // Lots of error checking, but only in debug mode, nothing at all
  // (and thus very speedy but dangerous access) in release mode.

#if CE_ARTLIB_EnableDebugMode
  if (DataID == LE_DATA_EmptyItem)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_GetItemPntrFromID: "
      "Trying to load %s!\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return NULL;
  }

  if (GroupIndex == 0 || GroupIndex > CE_ARTLIB_DataMaxGroups)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_GetItemPntrFromID: "
      "Out of range group index of %u for %s!\r\n",
      (unsigned int) GroupIndex,
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return NULL;
  }
#endif

  GroupPntr = LI_DATA_GroupArray + GroupIndex;

#if CE_ARTLIB_EnableDebugMode
  if (GroupPntr->dataItemArray == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_GetItemPntrFromID: "
      "Data group hasn't been initialised for %s!\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return NULL;
  }

  if (ItemIndex >= GroupPntr->numberOfDataItems)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_GetItemPntrFromID: "
      "Item index is larger than the number of items in the data group for %s!\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return NULL;
  }
#endif

  ItemPntr = GroupPntr->dataItemArray + ItemIndex;

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr->dataSource == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_GetItemPntrFromID: "
      "Data source is NULL, not an available item (yet) for %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return NULL;
  }
#endif

  return ItemPntr;
}



#if CE_ARTLIB_DataDataFileSource
/*****************************************************************************
 * Opens and initialises a traditional datafile for use by code, creating
 * and filling the given data group with the items in the data file
 * (makes the items point to the appropriate places in the data file
 * but doesn't actually load any data).  If you want to have extra spare
 * items after the ones from the file, you can specify the optional
 * ExtraItems argument and use the LE_DATA_AllocMemoryDataID and other
 * functions to set up the spare items.  This is useful for using an
 * old data file and adding on some more items, which can later be
 * connected to a new data file (useful for patching CD-ROM data files).
 * It will search the user paths then the global path list in an attempt
 * to find the file.  Remember you can use CD: for the CD-ROM drive.
 * Returns TRUE if successful.  Use LE_DATA_RemoveDataGroup to close it.
 */

BOOL LE_DATA_InitDatafile (char *DataFileName,
LE_DATA_DataGroupIndex DataGroupIndex,
UNS16 ExtraItems /* = 0 */
#if CE_ARTLIB_EnableSystemDirIni
, LE_DIRINI_PathListPointer UserSearchPaths /* = NULL */
#endif
)
{
  LI_DATA_DataFileSourceClassPointer  DataSourcePntr;
  char                                FullDataFileName [MAX_PATH];

  // First find the data file, searching the user and global search paths,
  // if the name provided doesn't lead to a file directly.

#if CE_ARTLIB_EnableSystemDirIni
  if (!LE_DIRINI_ExpandFileName (DataFileName, FullDataFileName,
  UserSearchPaths))
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_InitDatafile: "
      "Unable to find a data file with name \"%s\" in the usual places.\r\n",
      DataFileName);
    LE_ERROR_Msg (LE_ERROR_DebugMessageBuffer, LE_ERROR_OUTPUT_USER, FALSE /* Fatal */);
    return FALSE;
  }
#else // No path searching facility available.
  strcpy (FullDataFileName, DataFileName);
#endif

  DataSourcePntr = new LI_DATA_DataFileSourceClass (FullDataFileName);

  if (DataSourcePntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_InitDatafile: "
      "Unable to allocate a data source object for file \"%s\", "
      "out of memory.\r\n", FullDataFileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  if (DataSourcePntr->dataFileHandle == NULL)
  {
    delete DataSourcePntr;
    return FALSE; // Unable to open the file.
  }

  if (!DataSourcePntr->connectAllItemsInDataFileToNewGroup (DataGroupIndex, ExtraItems))
  {
    delete DataSourcePntr;
    return FALSE;
  }

  return TRUE;
}
#endif // CE_ARTLIB_DataDataFileSource



/*****************************************************************************
 * Loads the given data item if necessary.  Returns TRUE if it is now in
 * memory (either previously there or freshly loaded) or FALSE if it
 * failed to load.  Use LE_DATA_GetPointer to get a pointer to it.
 * Or if you want to do both at once, use LE_DATA_Use.  Note that this
 * assumes that you aren't removing or adding data items (opening or
 * closing data files) while you are trying to load them.
 */

BOOL LE_DATA_Load (LE_DATA_DataId DataID)
{
#if CE_ARTLIB_EnableDebugMode
  char Description [2048];
#endif
  LI_DATA_ItemPointer ItemPntr;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_Load: "
      "Unable to find item record for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif

  if (ItemPntr->memoryPntr != NULL)
  {
#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
    if (ItemPntr->referenceCount == 0)
      ItemPntr->moveItemToHeadOfLRU ();
#endif
#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
    ItemPntr->timesAccessed++;
#endif

    return TRUE; // This item was recently seen to be loaded.
  }

  if (ItemPntr->loadItem ())
    return TRUE; // It was successfully loaded.

  // Print out a descriptive error message about the DataID which failed
  // to load (the item loader doesn't show the DataID in its message).

#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_Load: "
    "Unable to load data for %s.\r\n",
    LE_DATA_DescString (DataID, Description, sizeof (Description)));
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

  return FALSE;
}



/*****************************************************************************
 * Loads the raw data corresponding to the given data item into the
 * buffer, no post-load processing is done and the data item itself
 * is untouched (remaining unloaded or loaded).  Just loads the data.
 * Returns the number of bytes loaded, zero for errors.
 */

UNS32 LE_DATA_LoadRawIntoBuffer (LE_DATA_DataId DataID,
void *MemoryPntr, UNS32 MemorySize, UNS32 StartOffset)
{
  LI_DATA_GenericDataSourceClassPointer DataSourcePntr;
  LI_DATA_ItemPointer                   ItemPntr;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_LoadRawIntoBuffer: "
      "Unable to find item record for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }
#endif

  DataSourcePntr = ItemPntr->dataSource;

#if CE_ARTLIB_EnableDebugMode
  if (DataSourcePntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_LoadRawIntoBuffer: "
      "NULL data source for DataID 0x%08X (it is probably uninitialised).\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }
#endif

  return DataSourcePntr->loadDataForItem (ItemPntr,
    MemoryPntr, MemorySize, StartOffset);
}



/*****************************************************************************
 * Returns a pointer to the data for the given DataID, or NULL if the data
 * isn't loaded (does not do loading).  You can also use this as a way of
 * telling if an item is loaded or not.
 */

void *LE_DATA_GetPointer (LE_DATA_DataId DataID)
{
  LI_DATA_ItemPointer ItemPntr;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_GetPointer: "
      "Unable to find item for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return NULL;
  }
#endif

  return ItemPntr->memoryPntr;
}



/*****************************************************************************
 * Returns the current data type of the given DataID.  This is a fast
 * call (doesn't load the item) but can give you different answers
 * depending on if the item is loaded or not.  Before an item is loaded,
 * it is the raw initial data type (the kind in the data file).  After
 * being loaded, it is the type set by the post-load processing, which
 * can be different (for example LE_DATA_DataTAB could be upgraded into
 * LE_DATA_DataGenericBitmap).  If it gets unloaded, the type doesn't
 * get reset, and stays the way it was.  Reloading should hopefully
 * set it to the same post-processed value as it was after the very
 * first loading.  Returns LE_DATA_DataUnknown if an error happens.
 */

LE_DATA_DataType LE_DATA_GetCurrentDataType (LE_DATA_DataId DataID)
{
  LI_DATA_ItemPointer ItemPntr;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_GetCurrentDataType: "
      "Unable to find item for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return LE_DATA_DataUnknown;
  }
#endif

  return ItemPntr->dataType;
}



/*****************************************************************************
 * Returns the data type of the given DataID as it is before pre-load
 * processing.  This is the type specified in the data file.
 */

LE_DATA_DataType LE_DATA_GetInitialDataType (LE_DATA_DataId DataID)
{
  LI_DATA_ItemPointer ItemPntr;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_GetInitialDataType: "
      "Unable to find item for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return LE_DATA_DataUnknown;
  }
#endif

  return ItemPntr->initialDataType;
}



/*****************************************************************************
 * Makes sure the item is loaded and then returns the data type.  That
 * way you always get the type corresponding to the in-memory data,
 * after post-load processing.
 */

LE_DATA_DataType LE_DATA_GetLoadedDataType (LE_DATA_DataId DataID)
{
  if (!LE_DATA_Load (DataID))
    return LE_DATA_DataUnknown;

  return LE_DATA_GetCurrentDataType (DataID);
}



/*****************************************************************************
 * Returns the current size in bytes of the given DataID.  Initially it
 * returns the unloaded size of the item (the size in the data file).
 * Once the item has been loaded, it then returns the post-load processed
 * size.  If the item is unloaded, it still returns the post-load processed
 * size.  If the item is reloaded, the size is updated, but it will usually
 * be the same post-load processed size.  Returns zero if an error happens.
 */

UNS32 LE_DATA_GetCurrentSize (LE_DATA_DataId DataID)
{
  LI_DATA_ItemPointer ItemPntr;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_GetCurrentSize: "
      "Unable to find item for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }
#endif

  return ItemPntr->currentSize;
}



/*****************************************************************************
 * Returns the initial size in bytes of the given DataID.  This corresponds
 * to the size of the raw data in the data file, before any post-load
 * processing gets done.
 */

UNS32 LE_DATA_GetInitialSize (LE_DATA_DataId DataID)
{
  LI_DATA_ItemPointer ItemPntr;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_GetInitialSize: "
      "Unable to find item for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }
#endif

  return ItemPntr->initialSize;
}



/*****************************************************************************
 * Loads the item if needed and returns the size (includes changes made by
 * the post-load processing to the size).
 */

UNS32 LE_DATA_GetLoadedSize (LE_DATA_DataId DataID)
{
  if (!LE_DATA_Load (DataID))
    return 0;

  return LE_DATA_GetCurrentSize (DataID);
}



/*****************************************************************************
 * Resizes the data used by the item to the new size, which can be bigger
 * or smaller.  If the item is loaded, the allocated memory is also
 * reallocated to the new size.  If the item is in an unloaded state,
 * this doesn't do much since the size gets changed to the actual amount
 * of data in the data source when it is loaded.  Does not change the
 * dirty flag.
 */

BOOL LE_DATA_Resize (LE_DATA_DataId DataID, UNS32 NewSize)
{
#if CE_ARTLIB_EnableDebugMode
  char Description [2048];
#endif
  LI_DATA_ItemPointer ItemPntr;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_Resize: "
      "Unable to find item record for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif

  if (!ItemPntr->resize (NewSize))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_Resize: "
      "Unable to resize data for %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE; // Probably out of memory.
  }

  return TRUE;
}



/*****************************************************************************
 * Loads the given data item if necessary, or uses the in-memory copy, and
 * returns a pointer to its data.  Returns NULL if it fails.  The pointer
 * stays valid until the data gets unloaded (which could happen soon unless
 * you add a reference to the data item while you are using it, or you have
 * turned off automatic unloading).
 */

void *LE_DATA_Use (LE_DATA_DataId DataID)
{
#if CE_ARTLIB_EnableDebugMode
  char Description [2048];
#endif
  LI_DATA_ItemPointer ItemPntr;
  void               *TempMemPntr; // A non-volatile version for faster code.

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_Use: "
      "Unable to find item record for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return NULL;
  }
#endif

  if ((TempMemPntr = ItemPntr->memoryPntr) != NULL)
  {
#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
    if (ItemPntr->referenceCount == 0)
      ItemPntr->moveItemToHeadOfLRU ();
#endif
#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
    ItemPntr->timesAccessed++;
#endif
    return TempMemPntr;
  }

  if (!ItemPntr->loadItem ())
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_Use: "
      "Unable to load data for %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL; // Failed to load.
  }

  return ItemPntr->memoryPntr;
}



/*****************************************************************************
 * Like LE_DATA_Use except that it also marks the item as dirty - meaning
 * that it will get written to disk when it is unloaded.
 */

void *LE_DATA_UseDirty (LE_DATA_DataId DataID)
{
#if CE_ARTLIB_EnableDebugMode
  char Description [2048];
#endif
  LI_DATA_ItemPointer ItemPntr;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_UseDirty: "
      "Unable to find item record for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return NULL;
  }
#endif

  if (ItemPntr->memoryPntr != NULL)
  {
#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
    if (ItemPntr->referenceCount == 0)
      ItemPntr->moveItemToHeadOfLRU ();
#endif
#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
    ItemPntr->timesAccessed++;
#endif
  }
  else if (!ItemPntr->loadItem ())
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_UseDirty: "
      "Unable to load data for %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL; // Failed to load.
  }

  ItemPntr->dirtyData = TRUE;

  return ItemPntr->memoryPntr;
}



/*****************************************************************************
 * Like LE_DATA_Use except it automatically adds 1 to the item's reference
 * count before returning the pointer to you.
 */

void *LE_DATA_UseRef (LE_DATA_DataId DataID)
{
#if CE_ARTLIB_EnableDebugMode
  char Description [2048];
#endif
  LI_DATA_ItemPointer ItemPntr;
  void               *TempMemPntr; // A non-volatile version for faster code.

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_UseRef: "
      "Unable to find item record for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return NULL;
  }
#endif

  if ((TempMemPntr = ItemPntr->memoryPntr) != NULL)
  {
    ItemPntr->addRef ();
    // No need to update the least recently used list - item no longer in it.
    return TempMemPntr;
  }

  // Have to load the item.

  if (!ItemPntr->loadItem ())
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_UseRef: "
      "Unable to load data for %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL; // Failed to load.
  }

  ItemPntr->addRef ();
  return ItemPntr->memoryPntr;
}



/*****************************************************************************
 * Mark the given DataID as having dirty bits.  That means it will get
 * written back out to disk when it is unloaded (if the data source
 * supports that - external files are currently the only ones which
 * do that).  The Dirty flag is TRUE to mark it as dirty, FALSE to
 * mark it as clean (no save when unloading if it is clean).
 */

BOOL LE_DATA_Dirty (LE_DATA_DataId DataID, BOOL Dirty)
{
  LI_DATA_ItemPointer ItemPntr;

  // Need to load it before it can be marked dirty, since
  // loading clears the dirty flag.

  if (Dirty && !LE_DATA_Load (DataID))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_Dirty: "
      "Unable to load DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_Dirty: "
      "Unable to find item record for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif

  ItemPntr->dirtyData = Dirty;

  return TRUE;
}



/*****************************************************************************
 * Removes the given item from memory, if possible.  Call this when you
 * know you won't be using the data for a while.  Returns FALSE if it
 * fails, usually because you have a non-zero reference counter on the data.
 * Note that the system can automatically unload data when it needs to,
 * if the reference count is zero.
 */

BOOL LE_DATA_Unload (LE_DATA_DataId DataID)
{
#if CE_ARTLIB_EnableDebugMode
  char Description [2048];
#endif
  LI_DATA_ItemPointer ItemPntr;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_Unload: "
      "Unable to find item record for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
  ItemPntr->timesUnloadedByUser++;
#endif

  if (ItemPntr->memoryPntr == NULL)
    return TRUE; // Already unloaded.

  if (!ItemPntr->unloadItem ())
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_Unload: "
      "Unable to unload data for %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE; // Failed to unload.
  }

  return TRUE;
}



/*****************************************************************************
 * Like LE_DATA_Unload except that it removes a reference before trying
 * to unload.  Returns TRUE if successful (item is unloaded).
 */

BOOL LE_DATA_UnloadRef (LE_DATA_DataId DataID)
{
#if CE_ARTLIB_EnableDebugMode
  char Description [2048];
#endif
  LI_DATA_ItemPointer ItemPntr;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_UnloadRef: "
      "Unable to find item record for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
  ItemPntr->timesUnloadedByUser++;
#endif

  if (ItemPntr->memoryPntr == NULL)
    return TRUE; // Already unloaded.

  if (!ItemPntr->removeRef ())
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_UnloadRef: "
      "Unable to remove the reference for %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
  }

  if (!ItemPntr->unloadItem ())
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_UnloadRef: "
      "Unable to unload data for %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE; // Failed to unload.
  }

  return TRUE;
}



/*****************************************************************************
 * Tries to unload all things which are loaded and have a zero reference
 * count.  Useful for switching video modes, where each bitmap gets
 * converted to the current screen depth as part of loading, so you need
 * to unload them all during switching.  Returns TRUE if everything which
 * should have unloaded did unload.
 */

BOOL LE_DATA_UnloadEverything (BOOL UnloadByUser /* = TRUE */)
{
  LE_DATA_DataId          DataID;
  LE_DATA_DataGroupIndex  GroupIndex;
  LI_DATA_GroupPointer    GroupPntr;
  LE_DATA_DataItemIndex   ItemIndex;
  LI_DATA_ItemPointer     ItemPntr;
  int                     NumberFailed = 0;

#if CE_ARTLIB_EnableMultitasking
  if (LI_DATA_DataGroupCriticalSectionPntr != NULL)
    EnterCriticalSection (LI_DATA_DataGroupCriticalSectionPntr);
#endif

#if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  if (LI_DATA_LRUCriticalSectionPntr != NULL)
    EnterCriticalSection (LI_DATA_LRUCriticalSectionPntr);
#endif

  for (GroupIndex = 1; GroupIndex <= CE_ARTLIB_DataMaxGroups; GroupIndex++)
  {
    GroupPntr = LI_DATA_GroupArray + GroupIndex;

    if (GroupPntr->dataItemArray != NULL)
    {
      for (ItemIndex = 0, ItemPntr = GroupPntr->dataItemArray;
      ItemIndex < GroupPntr->numberOfDataItems;
      ItemIndex++, ItemPntr++)
      {
        DataID = LE_DATA_IdFromTag (GroupIndex, ItemIndex);

        if (ItemPntr->dataSource != NULL && ItemPntr->memoryPntr != NULL)
        {
          if (ItemPntr->referenceCount == 0)
          {
#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
            if (UnloadByUser)
              ItemPntr->timesUnloadedByUser++;
#endif
            ItemPntr->unloadItem ();
          }
          else if (ItemPntr->dataType == LE_DATA_DataUAP ||
          ItemPntr->dataType == LE_DATA_DataNative ||
          ItemPntr->dataType == LE_DATA_DataGenericBitmap ||
          ItemPntr->dataType == LE_DATA_DataMESHX)
          {
#if CE_ARTLIB_EnableDebugMode
            strcpy (LE_ERROR_DebugMessageBuffer, "LE_DATA_UnloadEverything: "
              "Unable to unload an item which is screen depth dependent, for ");
            LE_DATA_DescribeDataID (DataID,
              LE_ERROR_DebugMessageBuffer + strlen (LE_ERROR_DebugMessageBuffer),
              sizeof (LE_ERROR_DebugMessageBuffer) - strlen (LE_ERROR_DebugMessageBuffer));
            strcat (LE_ERROR_DebugMessageBuffer, ".\r\n");
            LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif
            NumberFailed++;
          }
        }
      }
    }
  }

#if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  if (LI_DATA_LRUCriticalSectionPntr != NULL)
    LeaveCriticalSection (LI_DATA_LRUCriticalSectionPntr);
#endif

#if CE_ARTLIB_EnableMultitasking
  if (LI_DATA_DataGroupCriticalSectionPntr != NULL)
    LeaveCriticalSection (LI_DATA_DataGroupCriticalSectionPntr);
#endif

  // Now that we are outside the critical sections, we can try to display
  // an error message (while inside, the sequencer can't access bitmaps and
  // thus will lock up if we try to display a message box).

#if CE_ARTLIB_EnableDebugMode
  if (NumberFailed != 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_UnloadEverything: "
      "Unable to unload %d data items which are screen depth dependent, "
      "see the error log for details.  UnloadByUser: %d.\r\n",
      (int) NumberFailed, (int) UnloadByUser);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
      UnloadByUser ? LE_ERROR_DebugMsgToFile : LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

  return (NumberFailed == 0); // TRUE if no failures.
}



/*****************************************************************************
 * Increments the reference count for the given item, which means loading it
 * into memory if it isn't already there.  Returns the new count, or zero
 * if it failed (unable to load or find the item).
 */

LE_DATA_ReferenceCount LE_DATA_AddRef (LE_DATA_DataId DataID)
{
#if CE_ARTLIB_EnableDebugMode
  char Description [2048];
#endif
  LI_DATA_ItemPointer ItemPntr;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AddRef: "
      "Unable to find item record for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }
#endif

  // Needs to have data loaded for a reference to be meaningful.

  if (ItemPntr->memoryPntr == NULL)
  {
    if (!ItemPntr->loadItem ())
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AddRef: "
        "Unable to load data for %s.\r\n",
        LE_DATA_DescString (DataID, Description, sizeof (Description)));
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return 0;
    }
  }

  if (!ItemPntr->addRef ())
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AddRef: "
      "Unable to increment reference count for %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return 0;
  }

  return ItemPntr->referenceCount;
}



/*****************************************************************************
 * Decrements the reference counter by 1, usually meaning that you have
 * finished using DataID.  Returns the new count.  If the count is zero,
 * then the system is allowed to unload the item if it needs the space.
 */

LE_DATA_ReferenceCount LE_DATA_RemoveRef (LE_DATA_DataId DataID)
{
#if CE_ARTLIB_EnableDebugMode
  char Description [2048];
#endif
  LI_DATA_ItemPointer ItemPntr;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_RemoveRef: "
      "Unable to find item record for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }
#endif

  // Needs to have data loaded for a reference to be meaningful.

  if (ItemPntr->memoryPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_RemoveRef: "
      "Data is already unloaded, can't remove a reference for %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return 0;
  }

  if (!ItemPntr->removeRef ())
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_RemoveRef: "
      "Unable to decrement reference count for %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return 0;
  }

  return ItemPntr->referenceCount;
}



/*****************************************************************************
 * Returns the current reference count for the DataID, or zero if it
 * fails (but the count could legitimately be zero too).
 */

LE_DATA_ReferenceCount LE_DATA_GetRef (LE_DATA_DataId DataID)
{
  LI_DATA_ItemPointer ItemPntr;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_GetRef: "
      "Unable to find item record for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return 0;
  }
#endif

  if (ItemPntr->memoryPntr == NULL)
    return 0; // Not loaded.

  return ItemPntr->referenceCount;
}



#if CE_ARTLIB_DataMemorySource
/*****************************************************************************
 * Creates a runtime data item, using the data memory pool for allocation,
 * and adds the DataID to the given data group (defaults to the runtime
 * items group).  Automatically adds a reference to it so that it doesn't
 * get unloaded (runtime memory items get deallocated when they are
 * unloaded, and refilled with garbage when they are loaded - so the
 * freshly created one will be full of garbage).  When you are finished
 * with it, use LE_DATA_FreeRuntimeDataID to deallocate it.
 */

LE_DATA_DataId LE_DATA_AllocMemoryDataID (UNS32 NumberOfBytes,
LE_DATA_DataType DataType, LE_DATA_DataGroupIndex GroupIndex)
{
  LI_DATA_GroupPointer  GroupPntr;
  LE_DATA_DataItemIndex ItemIndex;
  LI_DATA_ItemPointer   ItemPntr;
  LE_DATA_DataId        NewID = LE_DATA_EmptyItem;

#if CE_ARTLIB_EnableDebugMode
  if (GroupIndex == 0 || GroupIndex > CE_ARTLIB_DataMaxGroups ||
  NumberOfBytes == 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AllocMemoryDataID: "
      "Bad group index (%u) or number of bytes (%u).\r\n",
      (unsigned int) GroupIndex, (unsigned int) NumberOfBytes);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return LE_DATA_EmptyItem; // Bad arguments.
  }
#endif

  // First try to create a data item for the user, using the given
  // data group.  This doesn't allocate memory, until it is loaded.

  GroupPntr = LI_DATA_GroupArray + GroupIndex;
  ItemPntr = LI_DATA_AllocateItemFromDataGroup (GroupPntr);

  if (ItemPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AllocMemoryDataID: "
      "Unable to allocate a DataID, data group %u is "
      "full (max %u items) or isn't initialised.\r\n",
      (unsigned int) GroupIndex,
      (unsigned int) GroupPntr->numberOfDataItems);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Set up the size of the item.  The type is LE_DATA_DataUserCreated1
  // for the initial type since we don't want any post-load processing
  // to happen on garbage memory data.

  if (LI_DATA_GlobalRuntimeMemoryDataSource == NULL ||
  !LI_DATA_GlobalRuntimeMemoryDataSource->connectItem (ItemPntr,
  NumberOfBytes, LE_DATA_DataUserCreated1))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AllocMemoryDataID: "
      "Unable to connect the new item to the memory data source.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Allocate the actual memory used by the item.

  if (!ItemPntr->loadItem ())
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AllocMemoryDataID: "
      "Unable to load the new memory item, usually means you are out of memory.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Bump the reference count so that it doesn't get auto-unloaded.
  // This should always work, unless the LRU list is corrupt.

  ItemPntr->addRef ();

  // Change the in-memory data type to the one the user wants.

  ItemPntr->dataType = DataType;

  // Generate the DataID for accessing our new item.

  ItemIndex = ItemPntr - GroupPntr->dataItemArray;
  NewID = LE_DATA_IdFromTag (GroupIndex, ItemIndex);

  goto NormalExit; // Successful.


ErrorExit:
  // Undo partial allocations.
  if (ItemPntr != NULL)
  {
    if (ItemPntr->dataSource != NULL)
      ItemPntr->disconnectFromDataSource ();
    LI_DATA_FreeItemToDataGroup (ItemPntr, GroupPntr);
  }

NormalExit:
  return NewID;
}
#endif // CE_ARTLIB_DataMemorySource



#if CE_ARTLIB_DataExternalFileSource
/*****************************************************************************
 * Creates a runtime data item, using the external file data source,
 * with the given file name.  If NewFileSize is zero then it will
 * use an existing file, otherwise it will create a new file with
 * the given size and type.  If NewFileType is LE_DATA_DataUnknown
 * then the file name will be used to guess the type of the existing
 * file.  Does not add a reference to it.  When you are finished with
 * it, use LE_DATA_FreeMemoryDataID to deallocate it.
 */

LE_DATA_DataId LE_DATA_AllocExternalFileDataID (char *ExternalFileName,
LE_DATA_DataType NewFileType, UNS32 NewFileSize,
LE_DATA_DataGroupIndex GroupIndex)
{
  LI_DATA_GroupPointer  GroupPntr;
  LE_DATA_DataItemIndex ItemIndex;
  LI_DATA_ItemPointer   ItemPntr;
  LE_DATA_DataId        NewID = LE_DATA_EmptyItem;

#if CE_ARTLIB_EnableDebugMode
  if (GroupIndex == 0 || GroupIndex > CE_ARTLIB_DataMaxGroups)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AllocExternalFileDataID: "
      "Bad group index (%u).\r\n", (unsigned int) GroupIndex);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return LE_DATA_EmptyItem; // Bad arguments.
  }
#endif

  // First try to create a data item for the user, using the given
  // data group.  This doesn't allocate memory, until it is loaded.

  GroupPntr = LI_DATA_GroupArray + GroupIndex;
  ItemPntr = LI_DATA_AllocateItemFromDataGroup (GroupPntr);

  if (ItemPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AllocExternalFileDataID: "
      "Unable to allocate a DataID, data group %u is "
      "full (max %u items) or isn't initialised.\r\n",
      (unsigned int) GroupIndex,
      (unsigned int) GroupPntr->numberOfDataItems);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Set up the size and type of the item, possibly creating a new file.

  if (LI_DATA_GlobalRuntimeExternalFileDataSource == NULL ||
  !LI_DATA_GlobalRuntimeExternalFileDataSource->connectItem (ItemPntr,
  ExternalFileName, NewFileType, NewFileSize))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AllocExternalFileDataID: "
      "Unable to connect the new item to the external file "
      "data source named \"%s\", with size %u and type %u (%s).\r\n",
      ExternalFileName, (unsigned int) NewFileSize,
      (unsigned int) NewFileType, LE_DATA_DataTypeNames [NewFileType]);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Generate the DataID for accessing our new item.

  ItemIndex = ItemPntr - GroupPntr->dataItemArray;
  NewID = LE_DATA_IdFromTag (GroupIndex, ItemIndex);

#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_ExternalFileSourceClass::connectItem: "
    "Set up %s external file \"%s\" with size %u and type %s for DataID 0x%08X.\r\n",
    (NewFileSize == 0) ? "existing" : "new",
    ExternalFileName, ItemPntr->initialSize,
    LE_DATA_DataTypeNames [ItemPntr->initialDataType],
    NewID);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif

  goto NormalExit; // Successful.


ErrorExit:
  // Undo partial allocations.
  if (ItemPntr != NULL)
  {
    if (ItemPntr->dataSource != NULL)
      ItemPntr->disconnectFromDataSource ();
    LI_DATA_FreeItemToDataGroup (ItemPntr, GroupPntr);
  }

NormalExit:
  return NewID;
}
#endif // CE_ARTLIB_DataExternalFileSource



#if CE_ARTLIB_DataURLSource
/*****************************************************************************
 * Creates a runtime data item, using the URL (uniform resource
 * locator) data source, with the given URL identifying a web
 * page or image to retrieve ("http://www.Artech.ca:80/index.html"
 * is an example, though the :80 can be left out).  Does not add a
 * reference to it.  When you call this function, it contacts
 * the web site to find the size and data type if you didn't
 * specify them (which can take time).  When you later use the
 * item, it contacts the site again to do the loading.  When you
 * are finished with it, use LE_DATA_FreeMemoryDataID to deallocate
 * it.
 */

LE_DATA_DataId LE_DATA_AllocURLDataID (char *URLName,
LE_DATA_DataType KnownType, UNS32 KnownSize,
LE_DATA_DataGroupIndex GroupIndex)
{
  LI_DATA_GroupPointer  GroupPntr;
  LE_DATA_DataItemIndex ItemIndex;
  LI_DATA_ItemPointer   ItemPntr;
  LE_DATA_DataId        NewID = LE_DATA_EmptyItem;

#if CE_ARTLIB_EnableDebugMode
  if (GroupIndex == 0 || GroupIndex > CE_ARTLIB_DataMaxGroups)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AllocURLDataID: "
      "Bad group index (%u).\r\n", (unsigned int) GroupIndex);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return LE_DATA_EmptyItem; // Bad arguments.
  }
#endif

  // First try to create a data item for the user, using the given
  // data group.  This doesn't allocate memory, until it is loaded.

  GroupPntr = LI_DATA_GroupArray + GroupIndex;
  ItemPntr = LI_DATA_AllocateItemFromDataGroup (GroupPntr);

  if (ItemPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AllocURLDataID: "
      "Unable to allocate a DataID, data group %u is "
      "full (max %u items) or isn't initialised.\r\n",
      (unsigned int) GroupIndex,
      (unsigned int) GroupPntr->numberOfDataItems);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Set up the size and type of the item, possibly connecting to the web site
  // if they aren't known.

  if (LI_DATA_GlobalRuntimeURLDataSource == NULL ||
  !LI_DATA_GlobalRuntimeURLDataSource->connectItem (ItemPntr,
  URLName, KnownType, KnownSize))
  {
#if CE_ARTLIB_EnableDebugMode
    char  LongErrorMessage [1300];
    sprintf (LongErrorMessage, "LE_DATA_AllocURLDataID: "
      "URL stuff not initialised, or more likely just unable to "
      "connect the new item for URL \"%s\", with preset size %u "
      "and type %u (%s).\r\n",
      URLName, (unsigned int) KnownSize,
      (unsigned int) KnownType, LE_DATA_DataTypeNames [KnownType]);
    LE_ERROR_DebugMsg (LongErrorMessage, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Generate the DataID for accessing our new item.

  ItemIndex = ItemPntr - GroupPntr->dataItemArray;
  NewID = LE_DATA_IdFromTag (GroupIndex, ItemIndex);

#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AllocURLDataID::connectItem: "
    "Set up URL based data item \"%s\" with size %u and type %s for DataID 0x%08X.\r\n",
    URLName, ItemPntr->initialSize,
    LE_DATA_DataTypeNames [ItemPntr->initialDataType],
    NewID);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif

  goto NormalExit; // Successful.


ErrorExit:
  // Undo partial allocations.
  if (ItemPntr != NULL)
  {
    if (ItemPntr->dataSource != NULL)
      ItemPntr->disconnectFromDataSource ();
    LI_DATA_FreeItemToDataGroup (ItemPntr, GroupPntr);
  }

NormalExit:
  return NewID;
}
#endif // CE_ARTLIB_DataURLSource



/*****************************************************************************
 * Decrements the reference count, unloads the data, and deallocates the
 * runtime data item.  Returns TRUE if successful (can fail if you have
 * added other reference counts to it).  Really only useful for runtime
 * created DataIDs, if you use it on a datafile based DataID then it will
 * wipe it out (that particular ID won't exist any more and can't be
 * loaded).
 */

BOOL LE_DATA_FreeRuntimeDataID (LE_DATA_DataId DataID)
{
#if CE_ARTLIB_EnableDebugMode
  char Description [2048];
#endif
  LI_DATA_GroupPointer  GroupPntr;
  LI_DATA_ItemPointer   ItemPntr;
  BOOL                  ReturnCode = FALSE;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_FreeRuntimeDataID: "
      "Unable to find item record for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }

  #if CE_ARTLIB_DataMemorySource
  if (ItemPntr->dataSource == LI_DATA_GlobalRuntimeMemoryDataSource &&
  LI_DATA_GlobalRuntimeMemoryDataSource != NULL &&
  ItemPntr->referenceCount != 1)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_FreeRuntimeDataID: "
      "Reference count isn't 1 for a memory sourced item, "
      "will remove a reference anyways (but not free it) from %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
  #endif // CE_ARTLIB_DataMemorySource
#endif

  // Decrement the reference count, but only for memory sourced items.

#if CE_ARTLIB_DataMemorySource
  if (ItemPntr->dataSource == LI_DATA_GlobalRuntimeMemoryDataSource &&
  LI_DATA_GlobalRuntimeMemoryDataSource != NULL)
    ItemPntr->removeRef (); // Don't mind if it fails.
#endif

  // Unload the data if it is still there.

  if (ItemPntr->memoryPntr != NULL && !ItemPntr->unloadItem ())
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_FreeRuntimeDataID: "
      "Unable to unload the data, so disconnect can't be done for %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
  ItemPntr->printUsageStatistics (DataID);
#endif

  if (!ItemPntr->disconnectFromDataSource ())
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_FreeRuntimeDataID: "
      "Unable to disconnect data source for %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  GroupPntr = LI_DATA_GroupArray + LE_DATA_FileFromId (DataID);
  if (!LI_DATA_FreeItemToDataGroup (ItemPntr, GroupPntr))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_FreeRuntimeDataID: "
      "Unable to free the DataID from the data group for %s.\r\n",
      LE_DATA_DescString (DataID, Description, sizeof (Description)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  ReturnCode = TRUE; // Successful.

ErrorExit:
  return ReturnCode;
}



/*****************************************************************************
 * Usually used for debug messages, this function creates a readable string
 * describing a DataID, including things like the name of the data file it
 * came from and what type of data it is.  Returns the number of bytes written
 * to your buffer, including the NUL at the end of the string.  If it is
 * equal to BufferSize then your buffer is probably too small.
 */

UNS32 LE_DATA_DescribeDataID (LE_DATA_DataId DataID, char *Buffer,
  UNS32 BufferSize)
{
  UNS32                   CopyLength;
  LE_DATA_DataGroupIndex  GroupIndex;
  LI_DATA_GroupPointer    GroupPntr;
  LE_DATA_DataItemIndex   ItemIndex;
  LI_DATA_ItemPointer     ItemPntr;
  char                   *NextString;
  char                    TempString [2048];

  if (Buffer == NULL || BufferSize == 0)
    return 0;

  // Smallest result is "...", since we append "..." if there is more
  // data than space for it.

  if (BufferSize < 4)
  {
    Buffer[0] = 0; // Return an empty string.
    return 1;
  }

  // General format is one long line containing something like:
  // DataID 0x12345678 (Group 4460, Index 22136, Type LE_DATA_DataChunky,
  // From data file "MainData.dat" index 123, Loaded, Current size 4567,
  // Original file name "Rabbit.seq")

  sprintf (TempString, "DataID 0x%08X (", (unsigned int) DataID);
  NextString = TempString + strlen (TempString);

  if (DataID == LE_DATA_EmptyItem)
  {
    strcpy (NextString, "LE_DATA_EmptyItem");
    NextString += strlen (NextString);
  }
  else
  {
    GroupIndex = LE_DATA_FileFromId (DataID);
    ItemIndex = LE_DATA_TagFromId (DataID);

    sprintf (NextString, "Group %u", (unsigned int) GroupIndex);
    NextString += strlen (NextString);

    if (GroupIndex == 0 || GroupIndex > CE_ARTLIB_DataMaxGroups)
    {
      strcpy (NextString, " (Invalid index)");
      NextString += strlen (NextString);
    }
    else
    {
      GroupPntr = LI_DATA_GroupArray + GroupIndex;

      if (GroupPntr->dataItemArray == NULL)
      {
        strcpy (NextString, " (Uninitialised)");
        NextString += strlen (NextString);
      }
      else
      {
#if CE_ARTLIB_DataMemorySource
        if (GroupIndex == LE_DATA_MemoryGroupIndex)
        {
          strcpy (NextString, " (Runtime Created Items)");
          NextString += strlen (NextString);
        }
#endif

        sprintf (NextString, ", Index %u", (unsigned int) ItemIndex);
        NextString += strlen (NextString);

#if CE_ARTLIB_EnableDebugMode
        if (ItemIndex >= GroupPntr->numberOfDataItems)
        {
          sprintf (NextString, " (Too big, group holds only %u items)",
            (unsigned int) GroupPntr->numberOfDataItems);
          NextString += strlen (NextString);
        }
        else
#endif
        {
          ItemPntr = GroupPntr->dataItemArray + ItemIndex;

          if (ItemPntr->memoryPntr == NULL)
            strcpy (NextString, ", Unloaded");
          else
            sprintf (NextString, ", Loaded (at 0x%08X, size %u)",
            (unsigned int) ItemPntr->memoryPntr,
            (unsigned int) ItemPntr->currentSize);
          NextString += strlen (NextString);

          sprintf (NextString, ", Type %u", (unsigned int) ItemPntr->dataType);
          NextString += strlen (NextString);

          if (ItemPntr->dataType < 0 || ItemPntr->dataType >= LE_DATA_DataTypeMax)
            strcpy (NextString, " (Unknown)");
          else
            sprintf (NextString, " (%s)", LE_DATA_DataTypeNames [ItemPntr->dataType]);
          NextString += strlen (NextString);

          sprintf (NextString, ", InitType %u", (unsigned int) ItemPntr->initialDataType);
          NextString += strlen (NextString);

          if (ItemPntr->initialDataType < 0 || ItemPntr->initialDataType >= LE_DATA_DataTypeMax)
            strcpy (NextString, " (Unknown)");
          else
            sprintf (NextString, " (%s)", LE_DATA_DataTypeNames [ItemPntr->initialDataType]);
          NextString += strlen (NextString);

          sprintf (NextString, ", RefCnt %u, Data from ",
            (unsigned int) ItemPntr->referenceCount);
          NextString += strlen (NextString);

          ItemPntr->describeItemSource (NextString);
          NextString += strlen (NextString);
        }
      }
    }
  }

  strcpy (NextString, ")");
  NextString += strlen (NextString);

  CopyLength = (NextString - TempString) + 1;
  if (CopyLength > BufferSize)
  {
    memcpy (Buffer, TempString, BufferSize);
    strcpy (Buffer + (BufferSize - 4), "...");
    CopyLength = BufferSize;
  }
  else
    strcpy (Buffer, TempString);

  return CopyLength;
}



/*****************************************************************************
 * A convenience function for printing DataIDs in printf statements,
 * since it returns the string pointer after filling it with the
 * description, otherwise it is the same as LE_DATA_DescribeDataID.
 */

char *LE_DATA_DescString (LE_DATA_DataId DataID, char *Buffer, UNS32 BufferSize)
{
  LE_DATA_DescribeDataID (DataID, Buffer, BufferSize);
  return Buffer;
}



#if CE_ARTLIB_DataNameHashTable
/*****************************************************************************
 * Returns the file name or URL of a DataID, or NULL if it doesn't
 * have a name.
 */

const char *LI_DATA_GetDataIDName (LE_DATA_DataId DataID)
{
  LI_DATA_ItemPointer ItemPntr;

  if (DataID == LE_DATA_EmptyItem)
    return NULL;

  ItemPntr = LI_DATA_GetItemPntrFromID (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (ItemPntr == NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_GetDataIDName: "
      "Unable to find item record for DataID 0x%08X.\r\n",
      (unsigned int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return NULL;
  }
#endif

  return ItemPntr->getItemName ();
}



/*****************************************************************************
 * Computes a hash value for the given string.  Since most strings will be
 * file names like C:\Blah\Blah\XXX12345.BMP, look at the last 8 (or less)
 * letters in the name part of the file name and treat it as a base 10
 * number.  The rest of the characters just get mushed together (added)
 * and multiplied by a prime number (37) so that ones with similar file
 * names map to non-overlapping ranges (assuming animations typically have
 * less than 37 frames).
 */

static UNS32 LI_DATA_ComputeHashValue (const char *FileName)
{
  UNS32       Mush;
  UNS32       Number;
  const UNS8 *NumberStart;
  const UNS8 *Period;
  const UNS8 *StringStart;

  StringStart = (const UNS8 *) FileName; // We want to work with UNSIGNED values.

  // Find the trailing period character, if none, pretend that there
  // is one just past the end of the string.

  Period = (const UNS8 *) strrchr (FileName, '.');
  if (Period == NULL)
    Period = StringStart + strlen (FileName); // Point at end of string.

  NumberStart = Period - 8; // Max 8 digits in the numericish part.

  if (NumberStart < StringStart)
    NumberStart = StringStart;

  Mush = 0;
  Number = 0;

  while (StringStart < NumberStart)
    Mush += tolower (*StringStart++);

  while (NumberStart < Period)
    Number = Number * 10 + tolower (*NumberStart++);

  while (*Period != 0)
    Mush += tolower (*Period++);

  return Mush * 37 + Number;
}



/*****************************************************************************
 * Searches the given hash chain for the given name, returns the hash
 * entry with it, or returns NULL.
 */

static LI_DATA_HashEntryPointer LI_DATA_LookUpFileNameInHashChain (
const char *FileName, LI_DATA_HashEntryPointer HashEntryPntr)
{
  const char  *TestName;

  while (HashEntryPntr != NULL)
  {
    TestName = LI_DATA_GetDataIDName (HashEntryPntr->dataID);
    if (TestName == NULL)
    {
#if CE_ARTLIB_EnableDebugMode
      char  TempString [2048];
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_LookUpFileNameInHashChain: "
        "While looking for \"%s\" in the hash table, hit a DataID with no "
        "name, how did it get there?  The problem is with %s.\r\n",
        FileName,
        LE_DATA_DescString (HashEntryPntr->dataID, TempString, sizeof (TempString)));
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif // CE_ARTLIB_EnableDebugMode
    }
    else
    {
      if (stricmp (FileName, TestName) == 0)
        break; // Found it!
    }

    HashEntryPntr = HashEntryPntr->next;
  }

  return HashEntryPntr;
}



/*****************************************************************************
 * Looks in the hash table of known file names and tries to find the DataID
 * which matches it.  Returns LE_DATA_EmptyItem if it doesn't find it.
 * This is useful for utilities which have data which refers to other
 * files by name (like sequences refering to bitmaps used).
 */

LE_DATA_DataId LE_DATA_LookUpFileNameInIndex (const char *FileName)
{
  LI_DATA_HashEntryPointer  HashEntryPntr;
  UNS32                     HashValue;
  int                       Index;

  if (FileName == NULL)
    return LE_DATA_EmptyItem;

  HashValue = LI_DATA_ComputeHashValue (FileName);
  Index = HashValue % LI_DATA_HASH_TABLE_SIZE;

  HashEntryPntr = LI_DATA_LookUpFileNameInHashChain (FileName,
    LI_DATA_HashTable [Index]);

  if (HashEntryPntr == NULL)
    return LE_DATA_EmptyItem;

  return HashEntryPntr->dataID;
}



/*****************************************************************************
 * Adds the given DataID with a file name (external file or URL based)
 * to the hash table using its name as the key to index it.  Returns FALSE
 * if out of memory or otherwise failed (duplicate name, DataID unnamed, etc).
 */

BOOL LE_DATA_AddNamedDataIDToIndex (LE_DATA_DataId DataID)
{
  LI_DATA_HashEntryPointer  HashEntryPntr;
  UNS32                     HashValue;
  int                       Index;
  const char               *FileName;

  FileName = LI_DATA_GetDataIDName (DataID);
  if (FileName == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    char  TempString [2048];
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AddNamedDataIDToIndex: "
      "The DataID doesn't have a name, so can't add %s.\r\n",
      LE_DATA_DescString (DataID, TempString, sizeof (TempString)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif // CE_ARTLIB_EnableDebugMode
    return FALSE;
  }

  HashValue = LI_DATA_ComputeHashValue (FileName);
  Index = HashValue % LI_DATA_HASH_TABLE_SIZE;

  HashEntryPntr = LI_DATA_LookUpFileNameInHashChain (
    FileName, LI_DATA_HashTable [Index]);

  if (HashEntryPntr != NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    char  TempString [2048];
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AddNamedDataIDToIndex: "
      "There is already something with the same name (%s) in the hash table, "
      "so not adding %s.\r\n", FileName,
      LE_DATA_DescString (DataID, TempString, sizeof (TempString)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif // CE_ARTLIB_EnableDebugMode
    return FALSE;
  }

  HashEntryPntr = (LI_DATA_HashEntryPointer)
    LI_MEMORY_AllocLibraryBlock (sizeof (LI_DATA_HashEntryRecord));

  if (HashEntryPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    char  TempString [2048];
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_AddNamedDataIDToIndex: "
      "Unable to allocate memory for a hash node while adding %s.\r\n",
      LE_DATA_DescString (DataID, TempString, sizeof (TempString)));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif // CE_ARTLIB_EnableDebugMode
    return FALSE;
  }

  HashEntryPntr->dataID = DataID;
  HashEntryPntr->next = LI_DATA_HashTable [Index];
  LI_DATA_HashTable [Index] = HashEntryPntr;

  return TRUE;
}



/*****************************************************************************
 * Takes the given entry out of the hash table.  Returns TRUE if successful.
 * This is also automatically called by LE_DATA_FreeRuntimeDataID or when
 * the data item otherwise gets deallocated (not merely unloaded).
 */

BOOL LE_DATA_RemoveNameFromIndex (const char *FileName)
{
  LI_DATA_HashEntryPointer  HashEntryPntr;
  UNS32                     HashValue;
  int                       Index;
  LI_DATA_HashEntryPointer  PreviousPntr;
  const char               *TestName;

  if (FileName == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_RemoveNameFromIndex: "
      "NULL name specified!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif // CE_ARTLIB_EnableDebugMode
    return FALSE;
  }

  HashValue = LI_DATA_ComputeHashValue (FileName);
  Index = HashValue % LI_DATA_HASH_TABLE_SIZE;
  HashEntryPntr = LI_DATA_HashTable [Index];
  PreviousPntr = NULL;

  while (HashEntryPntr != NULL)
  {
    TestName = LI_DATA_GetDataIDName (HashEntryPntr->dataID);
    if (TestName == NULL)
    {
#if CE_ARTLIB_EnableDebugMode
      char  TempString [2048];
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_RemoveNameFromIndex: "
        "While looking for \"%s\" in the hash table, hit a DataID with no "
        "name, how did it get there?  The problem is with %s.\r\n",
        FileName,
        LE_DATA_DescString (HashEntryPntr->dataID, TempString, sizeof (TempString)));
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif // CE_ARTLIB_EnableDebugMode
    }
    else
    {
      if (stricmp (FileName, TestName) == 0)
        break; // Found it!
    }

    PreviousPntr = HashEntryPntr;
    HashEntryPntr = HashEntryPntr->next;
  }

  if (HashEntryPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DATA_RemoveNameFromIndex: "
      "Unable to find \"%s\" in the hash table.\r\n", FileName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif // CE_ARTLIB_EnableDebugMode
    return FALSE;
  }

  if (PreviousPntr == NULL)
    LI_DATA_HashTable [Index] = HashEntryPntr->next;
  else
    PreviousPntr->next = HashEntryPntr->next;

  LI_MEMORY_DeallocLibraryBlock (HashEntryPntr);
  return TRUE;
}
#endif // CE_ARTLIB_DataNameHashTable



#if CE_ARTLIB_EnableSystemGrafix
/*****************************************************************************
 * Postload processing for TABs (actually UAPs): Processes the color table of
 * an 8 bit bitmap by converting the color table to native pixel format.
 * Returns a pointer to the converted data (since we convert it in place
 * it is the same as the raw data), returns NULL on failure.
 */

void *LI_DATA_PostLoadProcessUAPBitmap (LI_DATA_ItemPointer ItemPntr, void *RawDataPntr)
{
  LPNEWBITMAPHEADER BitmapHeaderPntr;

  BitmapHeaderPntr = (LPNEWBITMAPHEADER) RawDataPntr;

  BitmapHeaderPntr->dwFlags |= BITMAP_NOTRECOLORED; // Turn on this flag when loaded.

  if (LE_GRAFIX_ScreenBitsPerPixel == 16)
    LI_BLT_ConvertRGBColorTableTo16BitColor (
    (LPLONG) &BitmapHeaderPntr->dwColorTable, BitmapHeaderPntr->nColors);
  else if (LE_GRAFIX_ScreenBitsPerPixel == 32)
    LI_BLT_ConvertRGBColorTableTo32BitColor (
    (LPLONG) &BitmapHeaderPntr->dwColorTable, BitmapHeaderPntr->nColors);
  // Else screen is in 24 bit mode and the palette is already 24 bits.

  return RawDataPntr; // Successful!
}
#endif /* CE_ARTLIB_EnableSystemGrafix */



#if CE_ARTLIB_EnableSystemGrafix
/*****************************************************************************
 * Postload processing for 24 bit BMP images: converts it into a native
 * bitmap (same bits per pixel as the screen) and then switches to it,
 * changing the datatype to native bitmap in the process.  Returns a
 * pointer to the newly allocated buffer with the converted data,
 * returns NULL on failure.
 */

void *LI_DATA_PostLoadProcessBMPBitmap (LI_DATA_ItemPointer ItemPntr, void *RawDataPntr)
{
  LPBYTE              lpBackgroundBitData;
  LPBITMAPFILEHEADER  lpBitmapFileHeader;
  LPBITMAPINFO        lpBitmapInfo;
  LPBITMAPINFOHEADER  lpBitmapInfoHeader;
  LPNEWBITMAPHEADER   lpBMH = NULL;
  LPBYTE              lpConvertedBackgroundBitmapBits;
  long                malloc_size = 0;
  int                 nNewBytesPerLine;
  int                 nXWidthBitmapInPixels;
  int                 nYHeightBitmapInPixels;

  lpBitmapFileHeader = (LPBITMAPFILEHEADER) RawDataPntr;

  // Check for a valid BMP file header.

  if (lpBitmapFileHeader == NULL || lpBitmapFileHeader->bfType != 0x4D42)
    return NULL;

  // get pointer to various info structures
  lpBitmapInfo = (LPBITMAPINFO)((LPBYTE)lpBitmapFileHeader + sizeof(BITMAPFILEHEADER));
  lpBitmapInfoHeader = &lpBitmapInfo->bmiHeader;

  // get pointer to raw pixel data in the BMP.
  lpBackgroundBitData = (LPBYTE)lpBitmapFileHeader + lpBitmapFileHeader->bfOffBits;

  // Validate bitmap bits per pixel and screen depth.
  if (LE_GRAFIX_ScreenBitsPerPixel <= 8 || lpBitmapInfoHeader->biBitCount != 24)
    return NULL;

  // get bitmap dimensions
  nXWidthBitmapInPixels = lpBitmapInfoHeader->biWidth;
  nYHeightBitmapInPixels = lpBitmapInfoHeader->biHeight;

  // calculate native bitmap bits size, include padding in each scan line
  // so that each line comes out to a multiple of 4 bytes.
  if (LE_GRAFIX_ScreenBitsPerPixel == 16)
    nNewBytesPerLine = ((nXWidthBitmapInPixels * 2) + 3) & 0xfffffffc;
  else if (LE_GRAFIX_ScreenBitsPerPixel == 24)
    nNewBytesPerLine = ((nXWidthBitmapInPixels * 3) + 3) & 0xfffffffc;
  else if (LE_GRAFIX_ScreenBitsPerPixel == 32)
    nNewBytesPerLine = nXWidthBitmapInPixels * 4;
  else // Unsupported destination depth.
    return NULL;

  malloc_size = nNewBytesPerLine * nYHeightBitmapInPixels;
  malloc_size += sizeof(NEWBITMAPHEADER);

  lpBMH = (LPNEWBITMAPHEADER) LI_MEMORY_AllocDataBlock (malloc_size);
  if (lpBMH == NULL)
    return NULL; // Out of memory.

  lpBMH->nXBitmapWidth    = nXWidthBitmapInPixels;
  lpBMH->nYBitmapHeight   = nYHeightBitmapInPixels;
  lpBMH->nXOriginOffset   = 0;
  lpBMH->nYOriginOffset   = 0;
  lpBMH->dwFlags          = BITMAP_NOTRANSPARENCY;
  lpBMH->nColors          = 0;
  lpBMH->nAlpha           = ALPHA_OPAQUE100_0;

  lpConvertedBackgroundBitmapBits = (LPBYTE) &lpBMH->dwColorTable;

  // convert 24 bit BMP to native depth.

  LE_BLT_CopySolidUnityRaw (lpBackgroundBitData /* SourceBitsPntr */,
    ((nXWidthBitmapInPixels * 3) + 3) & 0xfffffffc /* SourceBytesPerLine */,
    24 /* SourceBitsPerPixel */,
    TRUE /* SourceHasBMPColours */,
    nXWidthBitmapInPixels /* SourceWidth */,
    nYHeightBitmapInPixels /* SourceHeight */,
    NULL /* SourcePalettePntr */,
    4 /* SourcePaletteEntryByteSize */,
    lpConvertedBackgroundBitmapBits /* DestinationBitsPntr */,
    nNewBytesPerLine /* DestinationBytesPerLine */,
    LE_GRAFIX_ScreenBitsPerPixel /* DestinationBitsPerPixel */,
    nXWidthBitmapInPixels /* DestinationWidth */,
    nYHeightBitmapInPixels /* DestinationHeight */,
    NULL /* DestinationPalettePntr */,
    4 /* DestinationPaletteEntryByteSize */,
    0 /* CopyToDestX */,
    0 /* CopyToDestY */,
    nXWidthBitmapInPixels /* CopyToWidth */,
    nYHeightBitmapInPixels /* CopyToHeight */,
    TRUE /* FlipVertically */);

  ItemPntr->currentSize = malloc_size;
  ItemPntr->dataType = LE_DATA_DataNative;
  return lpBMH;
}
#endif /* CE_ARTLIB_EnableSystemGrafix */



#if (CE_ARTLIB_EnableSystemGrafix || CE_ARTLIB_DataTAB2UAPUtility) && CE_ARTLIB_DataDopeTABSupport
/*****************************************************************************
 * Postload processing for DOPETABs (actually TABs): Create a UAP style
 * bitmap from the TAB.  Takes the 8 bit palette, premultiplies the colours
 * by the alpha values (so that the blitting routines don't have to do
 * it) then sorts it by alpha colour (so the alphas come first, before
 * the solids, and so we have the nAlpha count) then remaps the 8 bit
 * pixels to account for the sort, while copying them into the new UAP.
 * Also does the UAP conversion of the color table to native pixel format.
 * Returns a pointer to the newly allocated space holding the UAP data,
 * returns NULL on failure.  When compiling for the TAB2UAP utility,
 * the palette doesn't get mapped to the current screen depth.
 */

void *LI_DATA_PostLoadProcessDopeTAB (LI_DATA_ItemPointer ItemPntr, void *RawDataPntr)
{
#if CE_ARTLIB_EnableSystemGrafix
  int                             ByteBlueShift;
  int                             ByteGreenShift;
  int                             ByteRedShift;
  UNS32                           LocalBlueMask;
  UNS32                           LocalGreenMask;
  UNS32                           LocalRedMask;
#endif

  BITMAPFILEHEADER               *BitmapFileHeaderPntr;
  BITMAPINFOHEADER               *BitmapInfoHeaderPntr;
  UNS32                           BytesPerScanLine;
  int                             ColoursTransfered;
  BYTE                           *DataPntr;
  int                             i;
  int                             MappedIndex;
  int                             NewNumberOfColours;
  int                             NewNumberOfAlphas;
  LE_BLT_AlphaPaletteEntryPointer NewPalettePntr;
  BYTE                           *NewPixelPntr;
  BYTE                           *NewScanLinePntr;
  UNS32                           NewSize;
  LPNEWBITMAPHEADER               NewUAPPntr;
  int                             NumberTransparent;
  RGBQUAD                        *OldPalettePntr;
  BYTE                           *OldPixelPntr;
  BYTE                           *OldScanLinePntr;
  BYTE                            Pixel;
  UNS32                           PixelCount [256];
  BYTE                            RemapArray [256];
  UNS32                           TempAlpha;
  UNS32                           TempColour;
  DWORD                           TempDWORD;
  int                             x;
  int                             y;

  DataPntr = (BYTE *) RawDataPntr;
  BitmapFileHeaderPntr = (BITMAPFILEHEADER *) DataPntr;
  DataPntr += sizeof (BITMAPFILEHEADER);

  if (BitmapFileHeaderPntr->bfType != 'MB')
  {
#if CE_ARTLIB_EnableDebugMode || CE_ARTLIB_DataTAB2UAPUtility
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_PostLoadProcessDopeTAB: "
      "Not a BMP file, file header doesn't have \"BM\" at front.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL; // Not a BMP format file.
  }

  BitmapInfoHeaderPntr = (BITMAPINFOHEADER *) DataPntr;
  DataPntr += sizeof (BITMAPINFOHEADER);
  OldPalettePntr = (RGBQUAD *) DataPntr;

  if (BitmapInfoHeaderPntr->biWidth <= 0 ||
  BitmapInfoHeaderPntr->biHeight <= 0 ||
  BitmapInfoHeaderPntr->biPlanes != 1 ||
  BitmapInfoHeaderPntr->biSize < sizeof (BITMAPINFOHEADER) ||
  BitmapInfoHeaderPntr->biBitCount != 8 ||
  BitmapInfoHeaderPntr->biCompression != BI_RGB)
  {
#if CE_ARTLIB_EnableDebugMode || CE_ARTLIB_DataTAB2UAPUtility
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_PostLoadProcessDopeTAB: "
      "We can't handle this kind of BMP file, need 8 bit uncompressed, "
      "bottom up scan line order.  Got one with depth %d, width %d, height %d, "
      "compression 0x%X, planes %d (need 1), header size %d (need at least 40).\r\n",
      (int) BitmapInfoHeaderPntr->biBitCount,
      (int) BitmapInfoHeaderPntr->biWidth,
      (int) BitmapInfoHeaderPntr->biHeight,
      (int) BitmapInfoHeaderPntr->biCompression,
      (int) BitmapInfoHeaderPntr->biPlanes,
      (int) BitmapInfoHeaderPntr->biSize);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL; // Not a kind of BMP we can handle (need 8 bit, uncompressed).
  }

  // Fix up the number of colours in the palette.

  if (BitmapInfoHeaderPntr->biClrUsed == 0 ||
  BitmapInfoHeaderPntr->biClrUsed > 256)
    BitmapInfoHeaderPntr->biClrUsed = 256; // All colours in palette are used.

  // Optionally convert the palette to discrete alpha levels (17 levels rather
  // than 256), for the TAB2UAP utility only.

#if CE_ARTLIB_DataTAB2UAPUtility
  if (!TAB2UAP_FullAlpha)
  {
    for (i = 0; i < (int) BitmapInfoHeaderPntr->biClrUsed; i++)
    {
      TempAlpha = OldPalettePntr[i].rgbReserved;
      TempAlpha = (TempAlpha) / 15; // Makes it into 17 equal portions.
      if (TempAlpha > 16)
        TempAlpha = 16;
      // Now have a number from 0 to 16.  Convert to magic levels.
      TempAlpha = TempAlpha * 255 / 16;
      OldPalettePntr[i].rgbReserved = (BYTE) TempAlpha;
    }
  }
#endif

  // Sort the palette: move all the transparent colours to the
  // front, and count them.  Colour zero is always fully transparent, just
  // that some bitmaps have no transparent pixels (a flag gets set for that
  // so that faster bitmap copying routines can be used).

  for (i = 0; i < 256; i++)
  {
    RemapArray [i] = i;
    PixelCount [i] = 0;
  }

  ColoursTransfered = 0;
  NumberTransparent = 0;

  // Map all fully transparent colours to colour zero.  The colour
  // component doesn't matter since it is transparent.

  for (i = 0; i < (int) BitmapInfoHeaderPntr->biClrUsed; i++)
  {
    if (OldPalettePntr[i].rgbReserved == 0)
    {
      RemapArray [i] = 0;
      ColoursTransfered = 1;
      NumberTransparent++;
    }
  }

  // If everything is very transparent, and bitmap origin offsets are 0,
  // then we have opened a regular BMP file, which has default zeroes in
  // the alpha values.  Retroactively make everything solid in that case.

  if ((NumberTransparent >= (int) BitmapInfoHeaderPntr->biClrUsed) &&
     (BitmapFileHeaderPntr->bfReserved1 == 0) && (BitmapFileHeaderPntr->bfReserved2 == 0))
  {
    for (i = 0; i < (int) BitmapInfoHeaderPntr->biClrUsed; i++)
      OldPalettePntr[i].rgbReserved = 255;
    ColoursTransfered = 0;
  }

  // Map all the colours with semi-transparent alpha to just after colour zero.

  for (i = 0; i < (int) BitmapInfoHeaderPntr->biClrUsed; i++)
  {
    if (OldPalettePntr[i].rgbReserved != 0 &&
    OldPalettePntr[i].rgbReserved != 255)
    {
      if (ColoursTransfered == 0)
      {
        // Need to have colour zero fully transparent if semi-transparency
        // is in use (the blitter code assumes that).  But it isn't in the
        // palette, so add it manually.

        ColoursTransfered = 1; // Skip colour zero.
      }

      if (ColoursTransfered >= 256)
      {
        RemapArray [i] = 255;
#if CE_ARTLIB_EnableDebugMode || CE_ARTLIB_DataTAB2UAPUtility
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_PostLoadProcessDopeTAB: "
          "Semi-transparent colour index %d got misconverted.\r\n", (int) i);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      }
      else
        RemapArray [i] = ColoursTransfered;

      if (++ColoursTransfered > 256)
        ColoursTransfered = 256;
    }
  }

  // Record number of transparentish colours.

  NewNumberOfAlphas = ColoursTransfered;

  // Map the rest of the colours, the solid colours.

  for (i = 0; i < (int) BitmapInfoHeaderPntr->biClrUsed; i++)
  {
    if (OldPalettePntr[i].rgbReserved == 255)
    {
      if (ColoursTransfered >= 256)
      {
        RemapArray [i] = 255;
#if CE_ARTLIB_EnableDebugMode || CE_ARTLIB_DataTAB2UAPUtility
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_PostLoadProcessDopeTAB: "
          "Solid colour index %d got misconverted.\r\n", (int) i);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      }
      else
        RemapArray [i] = ColoursTransfered;
      if (++ColoursTransfered > 256)
        ColoursTransfered = 256;
    }
  }

  // Record number of colours in the new palette, may be less if there
  // were duplicate transparent colours, may be 1 more of the original
  // was semi-transparent but lacked a fully transparent colour.

  NewNumberOfColours = ColoursTransfered;

  // Account for rounding up to next multiple of 4 bytes per scan line,
  // when allocating memory for this new bitmap.

  BytesPerScanLine = ((BitmapInfoHeaderPntr->biWidth + 3) & 0xFFFFFFFC);

  NewSize = sizeof (NEWBITMAPHEADER) +
    NewNumberOfColours * sizeof (LE_BLT_AlphaPaletteEntryRecord) +
    BytesPerScanLine * BitmapInfoHeaderPntr->biHeight;

  // Allocate our UAP format bitmap.

  NewUAPPntr = (LPNEWBITMAPHEADER) LI_MEMORY_AllocDataBlock (NewSize);

  if (NewUAPPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode || CE_ARTLIB_DataTAB2UAPUtility
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_PostLoadProcessDopeTAB: "
      "Out of memory.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL; // Out of memory.
  }

  NewUAPPntr->nXBitmapWidth = (UNS16) BitmapInfoHeaderPntr->biWidth;
  NewUAPPntr->nYBitmapHeight = (UNS16) BitmapInfoHeaderPntr->biHeight;
  NewUAPPntr->nXOriginOffset = BitmapFileHeaderPntr->bfReserved1;
  NewUAPPntr->nYOriginOffset = BitmapFileHeaderPntr->bfReserved2;
  NewUAPPntr->dwFlags = BITMAP_NOTRECOLORED;
  NewUAPPntr->nColors = NewNumberOfColours;
  NewUAPPntr->nAlpha = NewNumberOfAlphas;

  // Point to the palette.

  NewPalettePntr = (LE_BLT_AlphaPaletteEntryPointer) NewUAPPntr->dwColorTable;

  // Copy the colours to the new palette, doing the order remapping
  // and also converting colours to native screen pixel values, and
  // also premultiplying by the alpha as a fraction (the blitter
  // premultiples the screen pixel by (1 - alpha) and then adds the
  // two together.  When doing the TAB2UAP utility, only the
  // premultiplication is done, there isn't any screen depth conversion.

#if CE_ARTLIB_EnableSystemGrafix
  ByteBlueShift = (LE_BLT_nBlueShift - 8) & (sizeof (unsigned long) * 8 - 1);
  ByteGreenShift = (LE_BLT_nGreenShift - 8) & (sizeof (unsigned long) * 8 - 1);
  ByteRedShift = (LE_BLT_nRedShift - 8) & (sizeof (unsigned long) * 8 - 1);

  LocalBlueMask = LE_BLT_dwBlueMask;  // Local copies for speed.
  LocalGreenMask = LE_BLT_dwGreenMask;
  LocalRedMask = LE_BLT_dwRedMask;
#endif

  for (i = 0; i < (int) BitmapInfoHeaderPntr->biClrUsed; i++)
  {
    TempAlpha = OldPalettePntr[i].rgbReserved;

    TempColour = OldPalettePntr[i].rgbBlue;
    TempColour = TempColour * TempAlpha / 255;
#if CE_ARTLIB_EnableSystemGrafix
    TempDWORD = (_lrotl (TempColour, ByteBlueShift) & LocalBlueMask);
#else
    TempDWORD = ((BYTE) TempColour);
#endif

    TempColour = OldPalettePntr[i].rgbGreen;
    TempColour = TempColour * TempAlpha / 255;
#if CE_ARTLIB_EnableSystemGrafix
    TempDWORD |= (_lrotl (TempColour, ByteGreenShift) & LocalGreenMask);
#else
    TempDWORD |= (((BYTE) TempColour) << 8);
#endif

    TempColour = OldPalettePntr[i].rgbRed;
    TempColour = TempColour * TempAlpha / 255;
#if CE_ARTLIB_EnableSystemGrafix
    TempDWORD |= (_lrotl (TempColour, ByteRedShift) & LocalRedMask);
#else
    TempDWORD |= (((BYTE) TempColour) << 16);
#endif

    MappedIndex = RemapArray [i];
    NewPalettePntr[MappedIndex].dwAlpha = TempAlpha;
    NewPalettePntr[MappedIndex].lowDWORD.dwColour = TempDWORD;
  }

  // Find the pixel pointers and remap all pixels to match the new palette,
  // also flip the image upside down since BMPs are bottom up while we
  // want to use top down.

  // Point to last scan line in the source bitmap.

  OldScanLinePntr = (BYTE *) RawDataPntr;
  OldScanLinePntr += BitmapFileHeaderPntr->bfOffBits;
  OldScanLinePntr += BytesPerScanLine * (BitmapInfoHeaderPntr->biHeight - 1);

  // First scan line in the destination bitmap.

  NewScanLinePntr = (BYTE *) (NewPalettePntr + ColoursTransfered);

  for (y = BitmapInfoHeaderPntr->biHeight; y > 0; y--)
  {
    OldPixelPntr = OldScanLinePntr;
    NewPixelPntr = NewScanLinePntr;

    for (x = BitmapInfoHeaderPntr->biWidth; x > 0; x--)
    {
      Pixel = *OldPixelPntr++;
      Pixel = RemapArray[Pixel];
      PixelCount[Pixel]++;
      *NewPixelPntr++ = Pixel;
    }

    // Our source pointer is going backwards in memory to flip the
    // image upside down.

    OldScanLinePntr -= BytesPerScanLine;
    NewScanLinePntr += BytesPerScanLine;
  }

  // Hack the transparent colour to be my favorite magenta (a colour
  // which doesn't occur in nature, unlike green or black or blue),
  // since it is fully transparent it never gets drawn.  Also,
  // need to set it since it may not have been in the original
  // palette (transparent gets added if there are any semi-
  // transparent colours).

  if (NewNumberOfAlphas > 0)
  {
#if CE_ARTLIB_EnableSystemGrafix
    NewPalettePntr[0].lowDWORD.dwColour = LocalBlueMask | LocalRedMask;
#else
    NewPalettePntr[0].lowDWORD.colours.blue = 255;
    NewPalettePntr[0].lowDWORD.colours.red = 255;
    NewPalettePntr[0].lowDWORD.colours.green = 0;
    NewPalettePntr[0].lowDWORD.colours.reserved = 0;
#endif
    NewPalettePntr[0].dwAlpha = 0;
  }

  // See if any of the alpha colours are actually used.

  for (i = NewUAPPntr->nAlpha - 1; i >= 0; i--)
  {
    if (PixelCount[i] != 0 &&
    NewPalettePntr[i].dwAlpha < 255 && NewPalettePntr[i].dwAlpha > 0)
    {
      NewUAPPntr->dwFlags |= BITMAP_ALPHACHANNEL;
      break;
    }
  }

  // See if colour zero, transparent, is not used.  If no alpha
  // colours either then this is a solid bitmap.

  if ((NewUAPPntr->dwFlags & BITMAP_ALPHACHANNEL) == 0 &&
  (PixelCount[0] == 0 || NewPalettePntr[0].dwAlpha != 0))
    NewUAPPntr->dwFlags |= BITMAP_NOTRANSPARENCY;

  // Now change the current data type and size to match the new data.

  ItemPntr->currentSize = NewSize;
  ItemPntr->dataType = LE_DATA_DataUAP;

  return NewUAPPntr; // Successful!
}
#endif // (CE_ARTLIB_EnableSystemGrafix || CE_ARTLIB_DataTAB2UAPUtility) && CE_ARTLIB_DataDopeTABSupport



#if CE_ARTLIB_EnableSystemGrafix
/*****************************************************************************
 * PostLoad processing for LE_DATA_DataGBMPicture and LE_DATA_DataGBMTexture.
 * Converts the image data into an LE_DATA_DataGenericBitmap.  Figures out
 * the format of the data by examining it (BMP or UAP).
 */

void *LI_DATA_PostLoadProcessGBMPictureOrTexture (LI_DATA_ItemPointer ItemPntr,
void *RawDataPntr)
{
  LE_GRAFIX_GenericBitmapPointer GBMPntr;

  GBMPntr = (LE_GRAFIX_GenericBitmapPointer) LI_MEMORY_AllocDataBlock (
    sizeof (LE_GRAFIX_GenericBitmapRecord));
  if (GBMPntr == NULL)
    return NULL; // Out of memory.

  if (!LI_GRAFIX_CreateGBMFromImageBuffer ((LPBYTE) RawDataPntr, ItemPntr->currentSize,
  (ItemPntr->dataType == LE_DATA_DataGBMTexture) /* TRUE to make a texture */,
  GBMPntr))
  {
    LI_MEMORY_DeallocDataBlock (GBMPntr);
    return NULL; // Failed to make the surface and GBM.
  }

  // Promote the data into a generic bitmap.

  ItemPntr->dataType = LE_DATA_DataGenericBitmap;
  ItemPntr->currentSize = sizeof (LE_GRAFIX_GenericBitmapRecord);
  return GBMPntr;
}
#endif /* CE_ARTLIB_EnableSystemGrafix */



#if CE_ARTLIB_EnableSystemGrafix
/*****************************************************************************
 * Preunload processing for GenericBitmaps - deallocate the DirectDraw
 * surface involved, returns NULL on failure.
 */

void *LI_DATA_PreUnloadProcessGenericBitmap (LI_DATA_ItemPointer ItemPntr, void *RawDataPntr)
{
  LI_GRAFIX_DestroyGBM ((LE_GRAFIX_GenericBitmapPointer) RawDataPntr);
  return RawDataPntr;
}
#endif /* CE_ARTLIB_EnableSystemGrafix */



#if CE_ARTLIB_EnableSystemGrafix3D
/*****************************************************************************
 * PostLoad processing for HMD 3D models + textures.  Convert them into
 * MeshX data (essentially a wrapper around some PC3D data structures).
 * So, the data system only stores a pointer to the PC3D stuff, and thus
 * only allocates and deallocates 4 bytes as far as it is concerned.
 */

void *LI_DATA_PostLoadProcessHMD( LI_DATA_ItemPointer ItemPntr, void *RawDataPntr )
{
#if CE_ARTLIB_3DUsingOldFrame
  meshx **myMesh;

  myMesh = (meshx **) LI_MEMORY_AllocDataBlock (sizeof (meshx *));
  if (myMesh == NULL)
    return NULL;

  *myMesh = new meshx( (HMD_Header *)RawDataPntr );
  if (*myMesh == NULL)
  {
    LI_MEMORY_DeallocDataBlock (myMesh);
    return NULL;
  }

  if( !(*myMesh)->initialized() )
  {
    delete *myMesh;
    LI_MEMORY_DeallocDataBlock (myMesh);
    return NULL;
  }

  ItemPntr->dataType = LE_DATA_DataMESHX;
  ItemPntr->currentSize = sizeof( meshx *);

  return (void *)myMesh;

#else // CE_ARTLIB_3DUsingOldFrame

  Mesh** ppMesh = NULL;

  ppMesh = (Mesh**)LI_MEMORY_AllocDataBlock(sizeof(Mesh*));
  if (!ppMesh) return(NULL);

  *ppMesh = new Mesh;
  ASSERT(NULL != *ppMesh);
  if (!*ppMesh)
  {
    LI_MEMORY_DeallocDataBlock(ppMesh);
    return(NULL);
  }

  if (!(*ppMesh)->LoadFromHMD((DWORD*)RawDataPntr))
  {
    delete *ppMesh;
    LI_MEMORY_DeallocDataBlock(ppMesh);
    return(NULL);
  }

  ItemPntr->dataType    = LE_DATA_DataMESHX;
  ItemPntr->currentSize = sizeof(Mesh *);
  // Yes, size of a pointer.  That's what the data system uses for meshes,
  // just 4 bytes, the rest is in PC3D and allocated/deallocated by PC3D.

  return((void*)ppMesh);

#endif // CE_ARTLIB_3DUsingOldFrame
}
#endif // CE_ARTLIB_EnableSystemGrafix3D



#if CE_ARTLIB_EnableSystemGrafix3D
/*****************************************************************************
 * Preunload processing for MeshX - deallocate the PC3D internals.
 */

void *LI_DATA_PreUnloadMESHX( LI_DATA_ItemPointer ItemPntr, void *RawDataPntr )
{
#if CE_ARTLIB_3DUsingOldFrame
  delete *(meshx **)RawDataPntr;
#else // CE_ARTLIB_3DUsingOldFrame
  delete *(Mesh**)RawDataPntr;
#endif // CE_ARTLIB_3DUsingOldFrame
  return RawDataPntr;
}
#endif // CE_ARTLIB_EnableSystemGrafix3D



/*****************************************************************************
 * Initialise the data system's structures.
 */

void LI_DATA_InitSystem (void)
{
  int i;

  // Initialize all the post-load and pre-unload processing functions.

  for (i = 0; i < LE_DATA_DataTypeMax; i++)
  {
    LE_DATA_PostLoadFunctions [i] = NULL;
    LE_DATA_PreUnloadFunctions [i] = NULL;
  }

#if (CE_ARTLIB_EnableSystemGrafix || CE_ARTLIB_DataTAB2UAPUtility) && CE_ARTLIB_DataDopeTABSupport
  LE_DATA_PostLoadFunctions [LE_DATA_DataDopeTAB] =
    LI_DATA_PostLoadProcessDopeTAB;
#endif


#if CE_ARTLIB_EnableSystemGrafix
  LE_DATA_PostLoadFunctions [LE_DATA_DataUAP] =
    LI_DATA_PostLoadProcessUAPBitmap;

  LE_DATA_PostLoadFunctions [LE_DATA_DataBMP] =
    LI_DATA_PostLoadProcessBMPBitmap;

  LE_DATA_PostLoadFunctions [LE_DATA_DataGBMPicture] =
    LI_DATA_PostLoadProcessGBMPictureOrTexture;

  LE_DATA_PostLoadFunctions [LE_DATA_DataGBMTexture] =
    LI_DATA_PostLoadProcessGBMPictureOrTexture;

  LE_DATA_PreUnloadFunctions [LE_DATA_DataGenericBitmap] =
    LI_DATA_PreUnloadProcessGenericBitmap;
#endif

#if CE_ARTLIB_EnableSystemGrafix3D
  LE_DATA_PostLoadFunctions[ LE_DATA_DataHMD ] =
    LI_DATA_PostLoadProcessHMD;
  LE_DATA_PreUnloadFunctions[ LE_DATA_DataMESHX ] =
    LI_DATA_PreUnloadMESHX;
#endif

  // Initialise statistics.

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
  LI_DATA_DataPoolMaxUsed = 0;
#endif

  // Set up the least recently used list and its critical section.

#if CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  LI_DATA_LeastRecentlyUsedHead = NULL;
  LI_DATA_LeastRecentlyUsedTail = NULL;
#if CE_ARTLIB_EnableDebugMode
  NumberOfItemsInLeastRecentlyUsedList = 0;
#endif // CE_ARTLIB_EnableDebugMode
#endif // CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded

#if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  InitializeCriticalSection (&LI_DATA_LRUCriticalSection);
  LI_DATA_LRUCriticalSectionPntr = &LI_DATA_LRUCriticalSection;
#endif

  // Create the single instance data sources.  Done here rather than having
  // them as static items so that initialisation gets done after other things
  // are ready (like the error reporting system).

#if CE_ARTLIB_DataMemorySource
  LI_DATA_GlobalRuntimeMemoryDataSource = new LI_DATA_MemorySourceClass;
  if (LI_DATA_GlobalRuntimeMemoryDataSource == NULL)
    LE_ERROR_ErrorMsg ("LI_DATA_InitSystem: "
    "Unable to create the memory data source (ran out of memory?).\r\n");
#endif

#if CE_ARTLIB_DataExternalFileSource
  LI_DATA_GlobalRuntimeExternalFileDataSource = new LI_DATA_ExternalFileSourceClass;
  if (LI_DATA_GlobalRuntimeExternalFileDataSource == NULL)
    LE_ERROR_ErrorMsg ("LI_DATA_InitSystem: "
    "Unable to create the external file data source (ran out of memory?).\r\n");
#endif

#if CE_ARTLIB_DataURLSource
  LI_DATA_GlobalRuntimeURLDataSource = new LI_DATA_URLSourceClass;
  if (LI_DATA_GlobalRuntimeURLDataSource == NULL)
    LE_ERROR_ErrorMsg ("LI_DATA_InitSystem: "
    "Unable to create the URL data source (ran out of memory?).\r\n");
#endif

  // Create the data groups critical section.

#if CE_ARTLIB_EnableMultitasking
  InitializeCriticalSection (&LI_DATA_DataGroupCriticalSection);
  LI_DATA_DataGroupCriticalSectionPntr = &LI_DATA_DataGroupCriticalSection;
#endif

  // Set up the data groups as initially empty.

  memset (LI_DATA_GroupArray, 0, sizeof (LI_DATA_GroupArray));

  // Create the default data group for runtime allocated programmer items.

#if CE_ARTLIB_DataMemorySource
  LE_DATA_InitialiseDataGroup (LE_DATA_MemoryGroupIndex,
    CE_ARTLIB_DataMaxRuntimeMemoryItems);
#endif

  // Clear the hash table to an empty state.

#if CE_ARTLIB_DataNameHashTable
  for (i = 0; i < LI_DATA_HASH_TABLE_SIZE; i++)
    LI_DATA_HashTable [i] = NULL;
#endif // CE_ARTLIB_DataNameHashTable
}



/*****************************************************************************
 * Deinitialise the data system.
 */

void LI_DATA_RemoveSystem (void)
{
#if CE_ARTLIB_DataNameHashTable
  int                       HashUndeletedCount;
  LI_DATA_HashEntryPointer  HashEntry;
  int                       HashIndex;
  LI_DATA_HashEntryPointer  HashNextEntry;
#endif // CE_ARTLIB_DataNameHashTable
#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
  char                      StatisticsString [256];
#endif
  LE_DATA_DataGroupIndex    GroupIndex;


  // Grab a few critical sections so that we can kill things without
  // interference.

#if CE_ARTLIB_EnableMultitasking
  if (LI_DATA_DataGroupCriticalSectionPntr != NULL)
    EnterCriticalSection (LI_DATA_DataGroupCriticalSectionPntr);
#endif

#if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  if (LI_DATA_LRUCriticalSectionPntr != NULL)
    EnterCriticalSection (LI_DATA_LRUCriticalSectionPntr);
#endif

  // Wipe out the data groups and associated data items and sources.

  for (GroupIndex = 1; GroupIndex <= CE_ARTLIB_DataMaxGroups; GroupIndex++)
    LE_DATA_RemoveDataGroup (GroupIndex);

  // Deallocate the hash table stuff, which should be empty anyways.

#if CE_ARTLIB_DataNameHashTable
  HashUndeletedCount = 0;
  for (HashIndex = 0; HashIndex < LI_DATA_HASH_TABLE_SIZE; HashIndex++)
  {
    HashEntry = LI_DATA_HashTable [HashIndex];
    while (HashEntry != NULL)
    {
      HashNextEntry = HashEntry->next;
      HashUndeletedCount++;
      LI_MEMORY_DeallocLibraryBlock (HashEntry);
      HashEntry = HashNextEntry;
    }
  }
#if CE_ARTLIB_EnableDebugMode
  if (HashUndeletedCount > 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DATA_RemoveSystem: "
      "There were %d entries left in the hash table after all data items had "
      "been freed.  There shouldn't have been any!\r\n", HashUndeletedCount);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif // CE_ARTLIB_EnableDebugMode
#endif // CE_ARTLIB_DataNameHashTable

  // Deallocate the Data Group critical section.

#if CE_ARTLIB_EnableMultitasking
  if (LI_DATA_DataGroupCriticalSectionPntr != NULL)
  {
    LI_DATA_DataGroupCriticalSectionPntr = NULL;
    DeleteCriticalSection (&LI_DATA_DataGroupCriticalSection);
  }
#endif

  // Deallocate the single instance data sources.

#if CE_ARTLIB_DataMemorySource
  if (LI_DATA_GlobalRuntimeMemoryDataSource != NULL)
  {
    delete LI_DATA_GlobalRuntimeMemoryDataSource;
    LI_DATA_GlobalRuntimeMemoryDataSource = NULL;
  }
#endif

#if CE_ARTLIB_DataExternalFileSource
  if (LI_DATA_GlobalRuntimeExternalFileDataSource != NULL)
  {
    delete LI_DATA_GlobalRuntimeExternalFileDataSource;
    LI_DATA_GlobalRuntimeExternalFileDataSource = NULL;
  }
#endif

#if CE_ARTLIB_DataURLSource
  if (LI_DATA_GlobalRuntimeURLDataSource != NULL)
  {
    delete LI_DATA_GlobalRuntimeURLDataSource;
    LI_DATA_GlobalRuntimeURLDataSource = NULL;
  }
#endif

  // Deallocate the critical section for the least recently used list.

#if CE_ARTLIB_EnableMultitasking && CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded
  if (LI_DATA_LRUCriticalSectionPntr != NULL)
  {
    // Set the global critical section access pointer to NULL first so that
    // when the section is deleted, all the waiting threads wake up and
    // see that the critical section is gone, and return immediately.  Note
    // that they wake up instantly at the delete, and this thread may not
    // run the statement after the delete for quite some time.

    LI_DATA_LRUCriticalSectionPntr = NULL;
    DeleteCriticalSection (&LI_DATA_LRUCriticalSection);
  }
#endif // CE_ARTLIB_EnableMultitasking && CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded

  // Print final statistics.
#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_DataCollectUsageStatistics
  sprintf (StatisticsString, "LI_DATA_RemoveSystem: "
    "Data system peak memory usage was %d bytes from the data pool.\r\n",
    (int) LI_DATA_DataPoolMaxUsed);
  LE_ERROR_DebugMsg (StatisticsString, FALSE);
#endif
}



/************************************************************
*   LE_DATA_GetIndexedItemId                                *
************************************************************/
int compareEntries( const void *key, const void *datum )
{
    unsigned long k = (unsigned long)key;
    LE_DATA_DataIndexEntry *d = (LE_DATA_DataIndexEntry *)datum;

    if ( k < d->indexValue )
        return( -1 );
    else if ( k > d->indexValue )
        return( 1 );
    else
        return( 0 );
}

LE_DATA_DataId LE_DATA_GetIndexedItemId(LE_DATA_DataId indexFile, unsigned long index)
{
    LE_DATA_DataIndexEntry *table, *tableEntry;
    unsigned long tableSize;

    // Verify that the item given is in fact an index table.
    if ( LE_DATA_GetLoadedDataType( indexFile ) != LE_DATA_DataIndexTable )
    {
#if CE_ARTLIB_EnableDebugMode
        sprintf(LE_ERROR_DebugMessageBuffer,"LE_DATA_GetIndexedItemId(): "
          "Not an index table 0x%08X\r\n", (unsigned int) indexFile );
        LE_ERROR_ErrorMsg(LE_ERROR_DebugMessageBuffer);
#endif
        return( LED_EI );
    }

    // Load the index table into memory.
    table = (LE_DATA_DataIndexEntry *) LE_DATA_Use (indexFile);
    tableSize = LE_DATA_GetCurrentSize ( indexFile ) / sizeof( LE_DATA_DataIndexEntry );

    // Binary search the index table for the given index.
    tableEntry = (LE_DATA_DataIndexEntry *)bsearch( (void *)index, (void *)table, tableSize, sizeof(LE_DATA_DataIndexEntry), compareEntries );

    // Verify that the index exists in the table.  It is OK to not find it,
    // just return a null pointer kind of data item.
    if ( tableEntry == NULL )
    {
        return( LED_EI );
    }

    // Everythings OK. Return the data id.
    return( LED_IFT( LED_FFI( indexFile ), tableEntry->dataID ) );
}


#endif  // if CE_ARTLIB_EnableSystemData
