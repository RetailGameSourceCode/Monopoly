#ifndef __L_DIRINI_H__
#define __L_DIRINI_H__

/*****************************************************************************
 * FILE:        L_DirIni.h
 *
 * DESCRIPTION: Directory paths and the INI preferences file for storing
 *              program settings.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_DirIni.h 6     1/20/99 5:38p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_DirIni.h $
 * 
 * 6     1/20/99 5:38p Agmsmith
 * Minor improvements.
 * 
 * 5     1/19/99 11:04a Agmsmith
 * Now accepts a pathname when searching for files, and returns an
 * absolute path name for ones it finds.
 * 
 * 4     1/18/99 4:05p Agmsmith
 * Now uses the search paths to find a file name.  Working!
 * 
 * 3     1/18/99 2:01p Agmsmith
 * Added support for multiple search path lists.
 *
 * 2     1/17/99 5:26p Agmsmith
 * Cosmetic changes.
 *
 * 1     1/17/99 2:39p Agmsmith
 * Adding DirIni module: directory paths and .INI files.
 ****************************************************************************/


/****************************************************************************/
/* D A T A   T Y P E S                                                      */
/*--------------------------------------------------------------------------*/

#pragma warning(disable:4200) /* Disable warning about zero size arrays being an extension. */
typedef struct LE_DIRINI_PathListStruct
{
  UNS32 sizeOfPaths;
    // Number of bytes in paths[], including NUL characters.

  char  paths [0];
    // All the known paths are stuck together into this string of strings.
    // Each path name is followed by a NUL (zero) byte and then followed
    // by the next path name.  The whole structure is resized (using
    // LI_MEMORY_ReallocLibraryBlock) as needed.  It gets completely
    // deallocated (sizeOfPaths too) when there are no path strings,
    // in other words a NULL LE_DIRINI_PathListPointer means zero paths.

} LE_DIRINI_PathListRecord, *LE_DIRINI_PathListPointer;
#pragma warning(default:4200)



/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

extern char LE_DIRINI_CDROM_PathName [MAX_PATH];
  // The path name to the CD-ROM drive root directory, includes a trailing
  // slash.  Usually something like "D:\".  Can be an empty string if there
  // is no CD-ROM (only happens in debug mode or in utility programs that
  // don't want a CD-ROM).  Also can be a long string in debug mode, pointing
  // to another directory where the CD-ROM image is.  Set by the init code.

extern char LE_DIRINI_INI_PathName [MAX_PATH];
  // The full path name of the .INI file currently being used.  Set by the
  // initialisation code.

extern LE_DIRINI_PathListPointer LE_DIRINI_GlobalPathList;
  // This contains the global path list, which all search requests will use
  // if the file can't be found in the user specified path list.  It is
  // also automatically loaded and saved from the game's .INI file.



/****************************************************************************/
/* F U N C T I O N   P R O T O T Y P E S                                    */
/*--------------------------------------------------------------------------*/

extern void LI_DIRINI_InitSystem (const char *INIFileName, const char *CDROMVolumeName);
// Initialises the system.  Reads standard settings (search path list) from
// the given .INI file in the same directory as the program executable.  If
// none, it creates one there and uses default values.  A search is done for
// the game's CD-ROM volume and the resulting path (usually just a drive
// letter) is put into LE_DIRINI_CDROM_PathName.  In debug mode, if it can't
// find the CD, then it asks the user to select a file in the root directory
// of the pseudo-CDROM.  If it can't find it in release mode, the game will
// simply exit with a warning message.  For games with no CD-ROM, pass in
// NULL for the CDROMVolumeName.  You can also specify an empty string
// for CDROMVolumeName to use whatever CD-ROM is in the drive.

extern void LI_DIRINI_RemoveSystem (void);
// Dumps out the list of paths to the .INI file and deallocates memory.


extern BOOL LE_DIRINI_AddPath (const char *NewPathName,
  LE_DIRINI_PathListPointer *PathListPntrPntr);
// Add the given path to the end of the given search list.  Returns TRUE
// if successful.  You can use the magic drive letter CD, as in
// "CD:\\MyData\\" to specify the drive with the CD-ROM.  It will
// add a trailing slash to the name if you don't specify one.
// If you pass in a pointer to NULL then it will allocate the path
// list for you and overwrite the NULL with the new path list pointer.

extern BOOL LE_DIRINI_ListPath (int PathIndex, char *PathNameBuffer,
  LE_DIRINI_PathListPointer PathListPntr);
// Read back the given path from the list of path names.  You need to
// allocate at least MAX_PATH (260) bytes for your buffer.  Returns
// TRUE if successful, FALSE if the name doesn't exist or you specified
// bad parameters.  The index starts at number zero.

extern BOOL LE_DIRINI_RemovePath (int PathIndex,
  LE_DIRINI_PathListPointer *PathListPntrPntr);
// Removes the given path from the list of paths.  Returns TRUE if
// successful.

extern void LE_DIRINI_DeallocatePathList (
  LE_DIRINI_PathListPointer *PathListPntrPntr);
// Deletes all the paths from the given path list and deallocates memory,
// leaving it a path list with zero paths (you can still add paths safely).

extern BOOL LE_DIRINI_CombinePathAndName (const char *InputPath,
  const char *InputName, char *OutputName);
// Combines a file name and a directory path to give you a full
// path name for the file.  Also replaces CD: with the actual CD-ROM
// drive letter and makes sure the result has the right number of
// slashes at the glue point.  Expects the output to be at least MAX_PATH
// characters long.  The output can be the same buffer as any input.
// Returns FALSE if it fails (no CD-ROM drive when you specify CD:, etc).
// Output buffer can be the same as the input buffer.

extern BOOL LE_DIRINI_TestIfFileExists (const char *FilePathName);
// Check and see if the given file exists.  Returns TRUE if present.

extern BOOL LE_DIRINI_ExpandFileName (const char *InputName, char *OutputName,
  LE_DIRINI_PathListPointer PathListPntr);
// Given the input file name, it will try to find a file with that name.
// The resulting absolute path name will be copied to your OutputName buffer
// (should be MAX_PATH bytes long).  Returns TRUE if successful.  First it
// tries the file name as-is, in case you specified a full name.  Then it
// extracts just the NAME.EXT portion and looks in the various search paths.
// For each path it tries TestPath/UserPathName.Ext, then TestPath/Name.Ext
// then TestPath/Ext/Name.Ext.  So, when looking for /Temp/XYZ.BMP, it will
// try Path/Temp/XYZ.BMP, Path/XYZ.BMP and Path/BMP/XYZ.BMP.  Of course, if
// there is no extension, then it won't try looking in the second directory
// which the extension specifies.  Also if your name contains a colon
// (C:/Temp/XYZ.BMP for example) then it won't try looking at your path
// (only looks for Path/XYZ.BMP and Path/BMP/XYZ.BMP, not
// Path/C:/Temp/XYZ.BMP).  The search paths start off with the current
// directory, then the search path list you provide in PathListPntr,
// which can be NULL to skip it, then it tries the global path list in
// the LE_DIRINI_GlobalPathList variable.

#endif // __L_DIRINI_H__
