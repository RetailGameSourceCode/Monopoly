/*****************************************************************************
 * FILE:        L_DirIni.cpp
 * DESCRIPTION: Directory paths and the INI preferences file for storing
 *              program settings.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_DirIni.cpp 12    28/09/99 12:55p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_DirIni.cpp $
 * 
 * 12    28/09/99 12:55p Agmsmith
 * More international text for the please insert CDROM message.
 *
 * 11    24/09/99 12:34p Agmsmith
 * Use operating system language to determine error message for no CD
 * present.
 *
 * 10    18/09/99 1:45p Agmsmith
 * Typo.
 *
 * 9     16/09/99 11:20a Agmsmith
 * Internationalised insert CD message.
 *
 * 8     5/31/99 1:28p Agmsmith
 *
 * 7     5/08/99 5:18p Agmsmith
 * Add executable directory to the global path.
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
 * 2     1/17/99 5:27p Agmsmith
 * Under construction.
 *
 * 1     1/17/99 2:39p Agmsmith
 * Adding DirIni module: directory paths and .INI files.
 ****************************************************************************/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemDirIni

/****************************************************************************/
/* P R I V A T E   T Y P E S   A N D   M A C R O S                          */
/*--------------------------------------------------------------------------*/



/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

char LE_DIRINI_CDROM_PathName [MAX_PATH];
  // The path name to the CD-ROM drive root directory, includes a trailing
  // slash.  Usually something like "D:\".  Can be an empty string if there
  // is no CD-ROM (only happens in debug mode).  Set by the init code.

char LE_DIRINI_INI_PathName [MAX_PATH];
  // The full path name of the .INI file currently being used.  Set by the
  // initialisation code.

LE_DIRINI_PathListPointer LE_DIRINI_GlobalPathList;
  // This contains the global path list, which all search requests will use
  // if the file can't be found in the user specified path list.  It is
  // also automatically loaded and saved from the game's .INI file.



/****************************************************************************/
/* P R I V A T E   V A R I A B L E S                                        */
/*--------------------------------------------------------------------------*/

static const char StringGlobaPathNames [] = "Global Pathnames";



/****************************************************************************/
/* I M P L E M E N T A T I O N                                              */
/*--------------------------------------------------------------------------*/

/*****************************************************************************
 * Initialises the system.  Reads standard settings (search path list) from
 * the given .INI file in the same directory as the program executable.  If
 * none, it creates one there and uses default values.  A search is done for
 * the game's CD-ROM volume and the resulting path (usually just a drive
 * letter) is put into LE_DIRINI_CDROM_PathName.  In debug mode, if it can't
 * find the CD, then it asks the user to select a file in the root directory
 * of the pseudo-CDROM.  If it can't find it in release mode, the game will
 * simply exit with a warning message.  For games with no CD-ROM, pass in
 * NULL for the CDROMVolumeName.  You can also specify an empty string
 * for CDROMVolumeName to use whatever CD-ROM is in the drive.
 */

void LI_DIRINI_InitSystem (const char *INIFileName, const char *CDROMVolumeName)
{
  DWORD ErrorCode;
  char  ExecutableDirectory [MAX_PATH];
  char  KeyString [80];
  int   PathNumber;
  char *StringPntr;
  char  TempString[1024];
  char  VolumeName[MAX_PATH];
  DWORD VolumeSerial;
  DWORD VolumeMaxFileNameLength;
  DWORD VolumeFileSystemFlags;
  char  VolumeFileSystemName[MAX_PATH];

  LE_DIRINI_GlobalPathList = NULL;
  LE_DIRINI_CDROM_PathName[0] = 0;
  LE_DIRINI_INI_PathName[0] = 0;

  if (GetModuleFileName (NULL, LE_DIRINI_INI_PathName,
  sizeof (LE_DIRINI_INI_PathName)) == 0)
    LE_ERROR_ErrorMsg ("LI_DIRINI_InitSystem: "
    "Unable to get path name of this program's executable file!\r\n");

  // Remove the program name and just leave the path name, ending with
  // the slash character.

  StringPntr = strrchr (LE_DIRINI_INI_PathName, '\\');
  StringPntr ++;
  *StringPntr = '\0';
  strcpy (ExecutableDirectory, LE_DIRINI_INI_PathName);

  // Stick on the user's .INI file name.

  strcat (LE_DIRINI_INI_PathName, INIFileName);

  // Now read the saved path names from the .INI file.

  PathNumber = 0;
  while (TRUE)
  {
    sprintf (KeyString, "Path%d", PathNumber);
    if (0 == GetPrivateProfileString (StringGlobaPathNames, KeyString,
    "" /* Default string of size zero */,
    TempString, sizeof (TempString), LE_DIRINI_INI_PathName))
      break; // Ran out of strings.

    if (!LE_DIRINI_AddPath (TempString, &LE_DIRINI_GlobalPathList))
      LE_ERROR_ErrorMsg ("LI_DIRINI_InitSystem: "
      "Problems while loading path strings from the .INI file.\r\n");

    PathNumber++;
  }

  // Add the executable path to the global path list.

  LE_DIRINI_AddPath (ExecutableDirectory, &LE_DIRINI_GlobalPathList);

  // Hunt for the CD-ROM, if any.  First get a list of all drives.

  if (CDROMVolumeName != NULL) // This game wants a CD-ROM.
  {
    ErrorCode = GetLogicalDriveStrings (sizeof (TempString), TempString);
    if (ErrorCode > 0 && ErrorCode < sizeof (TempString))
    {
      StringPntr = TempString;

      while (*StringPntr != 0)
      {
        if (GetDriveType (StringPntr) == DRIVE_CDROM)
        {
          // Check that the disk label is the desired one,
          // this actually reads the CD.

          if (GetVolumeInformation (StringPntr,
          VolumeName, sizeof (VolumeName),
          &VolumeSerial,
          &VolumeMaxFileNameLength,
          &VolumeFileSystemFlags,
          VolumeFileSystemName, sizeof (VolumeFileSystemName)))
          {
            if (CDROMVolumeName[0] == 0 /* Any CD is good */ ||
            stricmp (VolumeName, CDROMVolumeName) == 0)
            {
              // Found our CD-ROM, save the drive path string.
              strcpy (LE_DIRINI_CDROM_PathName, StringPntr);
              break;
            }
          }
        }

        // On to the next drive root name.
        StringPntr += strlen (StringPntr) + 1;
      }
    }
  }

  // If we didn't find the CD-ROM, ask the user for it in debug mode,
  // exit the program in release mode.  If no CD-ROM is needed, don't
  // worry about it.

  if (CDROMVolumeName != NULL && LE_DIRINI_CDROM_PathName[0] == 0)
  {
#if CE_ARTLIB_EnableDebugMode
    BOOL            GotFileName;
    int             FileNameLength;
    OPENFILENAME    OpenInfo;
    char            WindowTitle [MAX_PATH];

    memset (&OpenInfo, 0, sizeof (OpenInfo));
    OpenInfo.lStructSize = sizeof (OpenInfo);
    OpenInfo.hwndOwner = LE_MAIN_HwndMainWindow;
    OpenInfo.lpstrFilter = "Pick any file in your directory!\0*.*\0";
    OpenInfo.lpstrFile = TempString;
    OpenInfo.nMaxFile = sizeof (TempString);
    TempString [0] = 0;
    sprintf (WindowTitle, "Find \"%s\" fake CD-ROM root directory", CDROMVolumeName);
    OpenInfo.lpstrTitle = WindowTitle;
    OpenInfo.Flags = OFN_LONGNAMES | OFN_NOCHANGEDIR |
      OFN_NOTESTFILECREATE | OFN_PATHMUSTEXIST | OFN_NOVALIDATE;
    ShowCursor (TRUE);
    GotFileName = GetSaveFileName (&OpenInfo);
    ShowWindow (LE_MAIN_HwndMainWindow, SW_RESTORE);
    ShowCursor (FALSE);
    if (GotFileName)
    {
      // Remove file name and just get the directory the user specified.

      FileNameLength = strlen (TempString);
      if (TempString[FileNameLength] != '\\')
      {
        StringPntr = strrchr (TempString, '\\');
        if (StringPntr != NULL)
        {
          StringPntr[1] = 0; // Chop off after the \.
          strcpy (LE_DIRINI_CDROM_PathName, TempString);
        }
        // else If bad name, pretend no CD.
      }
      else // String already ends with \.
        strcpy (LE_DIRINI_CDROM_PathName, TempString);
    }
    // Else no CD-ROM specified, but that's OK in debug mode.
#else
    // No CD-ROM in release mode, put up an international
    // warning message and exit the program.  Defaults to
    // English if it doesn't know the language.

    char     *ErrorMessage = "Please insert CD-ROM.";
    LANGID    UserLanguage;

    UserLanguage = GetUserDefaultLangID ();

    if ((UserLanguage & 0x3FF) == LANG_FRENCH)
      ErrorMessage = "Veuillez insérer le CD-ROM.";

    if ((UserLanguage & 0x3FF) == LANG_GERMAN)
      ErrorMessage = "Bitte legen Sie die CD-ROM ein.";

    if ((UserLanguage & 0x3FF) == LANG_SPANISH)
      ErrorMessage = "Por favor, introduzca el CD-ROM.";

    if ((UserLanguage & 0x3FF) == LANG_DUTCH)
      ErrorMessage = "Plaats a.u.b. de CD-ROM.";

    if ((UserLanguage & 0x3FF) == LANG_SWEDISH)
      ErrorMessage = "Sätt in cd:n";

    if ((UserLanguage & 0x3FF) == LANG_FINNISH)
      ErrorMessage = "Aseta CD-ROM CD-ROM-asemaan.";

    if ((UserLanguage & 0x3FF) == LANG_DANISH)
      ErrorMessage = "Indsæt cd-rom.";

    if ((UserLanguage & 0x3FF) == LANG_NORWEGIAN)
      ErrorMessage = "Vennligst sett in CD-ROM.";

    MessageBox (LE_MAIN_HwndMainWindow,
      ErrorMessage, LE_MAIN_ApplicationNameString, MB_OK);
    exit (10);
#endif
  }
}



/*****************************************************************************
 * Dumps out the list of paths to the .INI file and deallocates memory.
 */

void LI_DIRINI_RemoveSystem (void)
{
  char  KeyString [80];
  UNS32 Offset;
  int   PathNumber;
  char *StringPntr;

  // Write out the list of search paths.

  if (LE_DIRINI_GlobalPathList != NULL && LE_DIRINI_INI_PathName[0] != 0)
  {
    PathNumber = 0;
    StringPntr = LE_DIRINI_GlobalPathList->paths;
    Offset = 0;

    while (Offset < LE_DIRINI_GlobalPathList->sizeOfPaths)
    {
      sprintf (KeyString, "Path%d", PathNumber);
      if (0 == WritePrivateProfileString (StringGlobaPathNames, KeyString,
      StringPntr, LE_DIRINI_INI_PathName))
        break; // Error while writing.

      Offset += strlen (StringPntr) + 1;
      StringPntr = LE_DIRINI_GlobalPathList->paths + Offset;
      PathNumber++;
    }

    // In case of shrinking numbers of paths, wipe out the stored path
    // just after the last one, this will prevent it from reading in
    // ones following (init code stops at a gap).

    sprintf (KeyString, "Path%d", PathNumber);
    WritePrivateProfileString (StringGlobaPathNames, KeyString, NULL,
      LE_DIRINI_INI_PathName);
  }

  // Deallocate storage for the search paths.

  LE_DIRINI_DeallocatePathList (&LE_DIRINI_GlobalPathList);
}



/*****************************************************************************
 * Add the given path to the given search list.  Returns TRUE if successful.
 * You can use the magic drive letter CD, as in "CD:\\MyData\\" to
 * specify the drive with the CD-ROM.  It will add a trailing slash
 * to the name if you don't specify one.
 * If you pass in a pointer to NULL then it will allocate the path
 * list for you and overwrite the NULL with the new path list pointer.
 */

BOOL LE_DIRINI_AddPath (const char *NewPathName,
LE_DIRINI_PathListPointer *PathListPntrPntr)
{
  UNS32 NewCollectionSize;
  int   NewPathSize;
  UNS32 Offset;
  char  PathWithSlash [MAX_PATH];
  UNS32 PreviousSizeOfPaths;
  char *StringPntr;

  if (NewPathName == NULL || PathListPntrPntr == NULL)
    return FALSE;

  NewPathSize = strlen (NewPathName);
  if (NewPathSize == 0)
    return FALSE;

  // Make a copy of the path, and make sure that there is a trailing
  // slash in the copy.

  strcpy (PathWithSlash, NewPathName);
  if (NewPathName[NewPathSize - 1] != '\\')
  {
    strcpy (PathWithSlash + NewPathSize, "\\");
    NewPathSize++;
  }

  if (*PathListPntrPntr == NULL)
    PreviousSizeOfPaths = 0;
  else
    PreviousSizeOfPaths = (*PathListPntrPntr)->sizeOfPaths;

  // See if the path is already in the list of paths.

  Offset = 0;
  while (Offset < PreviousSizeOfPaths)
  {
    StringPntr = (*PathListPntrPntr)->paths + Offset;
    if (stricmp (StringPntr, PathWithSlash) == 0)
      return TRUE; // Path is already in the list.
    Offset += strlen (StringPntr) + 1;
  }

  NewCollectionSize = PreviousSizeOfPaths + NewPathSize + 1 /* For NUL */;

  if (PreviousSizeOfPaths == 0)
    *PathListPntrPntr = (LE_DIRINI_PathListPointer)
    LI_MEMORY_AllocLibraryBlock (
    NewCollectionSize + sizeof (LE_DIRINI_PathListRecord));
  else
    *PathListPntrPntr = (LE_DIRINI_PathListPointer)
    LI_MEMORY_ReallocLibraryBlock (*PathListPntrPntr,
    NewCollectionSize + sizeof (LE_DIRINI_PathListRecord));

  if (*PathListPntrPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DIRINI_AddPath: "
      "Ran out of memory while adding path \"%s\", all previous "
      "paths have been lost too!\r\n",
      NewPathName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  strcpy ((*PathListPntrPntr)->paths + PreviousSizeOfPaths, PathWithSlash);
  (*PathListPntrPntr)->sizeOfPaths = NewCollectionSize;

  return TRUE;
}



/*****************************************************************************
 * Read back the given path from the list of path names.  You need to
 * allocate at least MAX_PATH (260) bytes for your buffer.  Returns
 * TRUE if successful, FALSE if the name doesn't exist or you specified
 * bad parameters.  The index starts at number zero.
 */

BOOL LE_DIRINI_ListPath (int PathIndex, char *PathNameBuffer,
LE_DIRINI_PathListPointer PathListPntr)
{
  UNS32 Offset;
  int   PathNumber;
  char *StringPntr;

  if (PathIndex < 0 || PathNameBuffer == NULL)
    return FALSE;

  if (PathListPntr == NULL)
    return FALSE; // No paths in the list.

  PathNumber = 0;
  StringPntr = PathListPntr->paths;
  Offset = 0;

  while (Offset < PathListPntr->sizeOfPaths && PathNumber < PathIndex)
  {
    Offset += strlen (StringPntr) + 1;
    StringPntr = PathListPntr->paths + Offset;
    PathNumber++;
  }

  if (Offset >= PathListPntr->sizeOfPaths)
    return FALSE;

  strcpy (PathNameBuffer, StringPntr);
  return TRUE;
}



/*****************************************************************************
 * Removes the given path from the list of paths.  Returns TRUE if
 * successful.
 */

BOOL LE_DIRINI_RemovePath (int PathIndex,
LE_DIRINI_PathListPointer *PathListPntrPntr)
{
  UNS32 DeleteLength;
  UNS32 Offset;
  int   PathNumber;
  char *StringPntr;

  if (PathIndex < 0 || PathListPntrPntr == NULL)
    return FALSE;

  if (*PathListPntrPntr == NULL)
    return FALSE; // No paths available to delete.

  // Look for the start of the path string to be deleted.

  PathNumber = 0;
  StringPntr = (*PathListPntrPntr)->paths;
  Offset = 0;

  while (Offset < (*PathListPntrPntr)->sizeOfPaths && PathNumber < PathIndex)
  {
    Offset += strlen (StringPntr) + 1;
    StringPntr = (*PathListPntrPntr)->paths + Offset;
    PathNumber++;
  }

  if (Offset >= (*PathListPntrPntr)->sizeOfPaths)
    return FALSE;

  DeleteLength = strlen (StringPntr) + 1;

  // If this is the last path, delete the whole record.

  if ((*PathListPntrPntr)->sizeOfPaths <= DeleteLength)
  {
    LI_MEMORY_DeallocLibraryBlock (*PathListPntrPntr);
    *PathListPntrPntr = NULL;
    return TRUE;
  }

  // Move over following strings to fill the hole left by the
  // deleted string.

  memmove (StringPntr, StringPntr + DeleteLength,
    (*PathListPntrPntr)->sizeOfPaths - Offset - DeleteLength);

  (*PathListPntrPntr)->sizeOfPaths -= DeleteLength;

  *PathListPntrPntr = (LE_DIRINI_PathListPointer)
    LI_MEMORY_ReallocLibraryBlock (
    *PathListPntrPntr,
    (*PathListPntrPntr)->sizeOfPaths + sizeof (LE_DIRINI_PathListRecord));

  if (*PathListPntrPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_DIRINI_RemovePath: "
      "Ran out of memory while deleting paths, all previous "
      "paths have been lost too!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  return TRUE;
}



/*****************************************************************************
 * Deletes all the paths from the given path list and deallocates memory,
 * leaving it a path list with zero paths (you can still add paths safely).
 */

void LE_DIRINI_DeallocatePathList (LE_DIRINI_PathListPointer *PathListPntrPntr)
{
  if (PathListPntrPntr != NULL && *PathListPntrPntr != NULL)
  {
    LI_MEMORY_DeallocLibraryBlock (*PathListPntrPntr);
    *PathListPntrPntr = NULL;
  }
}



/*****************************************************************************
 * Combines a file name and a directory path to give you a full
 * path name for the file.  Also replaces CD: with the actual CD-ROM
 * drive letter and makes sure the result has the right number of
 * slashes at the glue point.  Expects the output to be at least MAX_PATH
 * characters long.  The output can be the same buffer as any input.
 * Returns FALSE if it fails (no CD-ROM drive when you specify CD:, etc).
 * Output buffer can be the same as the input buffer.
 */

BOOL LE_DIRINI_CombinePathAndName (const char *InputPath,
const char *InputName, char *OutputName)
{
  int   i;
  char  TempOutput [MAX_PATH];

  if (InputName == NULL || InputPath == NULL || OutputName == NULL)
    return FALSE;

  // Get the path name part, expand CD: into the CD-ROM drive name,
  // if there is no CD then the CDROM name is an empty string.

  if (strnicmp (InputPath, "CD:", 3) == 0)
  {
    if (LE_DIRINI_CDROM_PathName[0] == 0)
      return FALSE; // No CD-ROM drive present.

    // Prepend the CD-ROM name, which ends with a slash.  Don't
    // copy the user's slash if there is one, to avoid having
    // two slashes.

    strcpy (TempOutput, LE_DIRINI_CDROM_PathName);

    if (InputPath[3] == '\\')
      strcat (TempOutput, InputPath + 4);
    else
      strcat (TempOutput, InputPath + 3);
  }
  else
    strcpy (TempOutput, InputPath);

  // Stick the path name and file name together, with a slash between them.
  // First make sure that the path name doesn't have a slash at the end.
  // This is so you can specify an empty string for the path and a
  // full name for the file name portion, and still have it work.

  i = strlen (TempOutput);
  if (i > 0 && TempOutput[i-1] == '\\')
    TempOutput[i-1] = 0;

  // Now make sure that the file name starts with a slash (add one if
  // needed), and copy the rest of it to the output.

  if (InputName[0] != '\\')
    strcat (TempOutput, "\\");
  strcat (TempOutput, InputName);

  // Now copy the result to the user's buffer, since we have finished with
  // the input strings, which may be using the same buffer.

  strcpy (OutputName, TempOutput);
  return TRUE;
}



/*****************************************************************************
 * Check and see if the given file exists.  Returns TRUE if present.
 */

BOOL LE_DIRINI_TestIfFileExists (const char *FilePathName)
{
  HANDLE          FindHandle = INVALID_HANDLE_VALUE;
  WIN32_FIND_DATA FindInfo;
  BOOL            ReturnCode = FALSE;

  FindHandle = FindFirstFile (FilePathName, &FindInfo);

  if (FindHandle == INVALID_HANDLE_VALUE)
    goto ErrorExit;

  if (FindInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    goto ErrorExit; // This is a directory.

  ReturnCode = TRUE;

ErrorExit:
  if (FindHandle != INVALID_HANDLE_VALUE)
    FindClose (FindHandle);

  return ReturnCode;
}



/*****************************************************************************
 * Given the input file name, it will try to find a file with that name.
 * The resulting absolute path name will be copied to your OutputName buffer
 * (should be MAX_PATH bytes long).  Returns TRUE if successful.  First it
 * tries the file name as-is, in case you specified a full name.  Then it
 * extracts just the NAME.EXT portion and looks in the various search paths.
 * For each path it tries TestPath/UserPathName.Ext, then TestPath/Name.Ext
 * then TestPath/Ext/Name.Ext.  So, when looking for /Temp/XYZ.BMP, it will
 * try Path/Temp/XYZ.BMP, Path/XYZ.BMP and Path/BMP/XYZ.BMP.  Of course, if
 * there is no extension, then it won't try looking in the second directory
 * which the extension specifies.  Also if your name contains a colon
 * (C:/Temp/XYZ.BMP for example) then it won't try looking at your path
 * (only looks for Path/XYZ.BMP and Path/BMP/XYZ.BMP, not
 * Path/C:/Temp/XYZ.BMP).  The search paths start off with the current
 * directory, then the search path list you provide in PathListPntr,
 * which can be NULL to skip it, then it tries the global path list in
 * the LE_DIRINI_GlobalPathList variable.
 */

BOOL LE_DIRINI_ExpandFileName (const char *InputName, char *OutputName,
LE_DIRINI_PathListPointer PathListPntr)
{
  DWORD ErrorCode;
  char  Extension [MAX_PATH];
  int   PathIndex;
  char  PureName [MAX_PATH];
  char *StringPntr;
  char  TempOutput [MAX_PATH];
  char  TestPath [MAX_PATH];
  BOOL  TryOriginalName;

  if (InputName == NULL || OutputName == NULL)
    return FALSE;

  // Check for the wrong kind of slashes in the name, we only look for
  // backslashes at the moment.

#if CE_ARTLIB_EnableDebugMode
  if (strchr (InputName, '/') != NULL)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_DIRINI_ExpandFileName: "
      "Your file name of \"%s\" has forward slashes (/), we need "
      "backwards slashes (\\) to work correctly.  Please fix!\r\n",
      InputName);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

  // If the user's name has a drive letter in it (a colon in other words),
  // don't try combining the original path with search paths, only try
  // with the file name portion.

  TryOriginalName = (strchr (InputName, ':') == NULL);

  // Check the user's original full path and name first.

  strcpy (TempOutput, InputName);
  if (LE_DIRINI_TestIfFileExists (TempOutput))
    goto NormalExit;

  // Find the file name and extension, look for a period in the last part
  // of the InputName (in case the user has directories with periods).

  StringPntr = strrchr (InputName, '\\');
  if (StringPntr == NULL)
  {
    StringPntr = strrchr (InputName, ':');
    if (StringPntr == NULL)
      StringPntr = (char *) InputName;
    else
      StringPntr++; // Point past the colon.
  }
  else
    StringPntr++; // Point past the slash.

  strcpy (PureName, StringPntr); // Save the NAME.EXT portion without path.

  StringPntr = strrchr (PureName, '.');
  if (StringPntr == NULL)
    Extension[0] = 0; // No extension.
  else // Everything after the last period is the extension.
  {
    strcpy (Extension, StringPntr + 1);
    strupr (Extension); // Make it uppercase.

    // Some file extensions actually go into different directories,
    // remap the extension to a directory.  If you add more here,
    // also add them to LE_CHUNK_LoadUsingEmbeddedFileNames.

    if (strcmp (Extension, "CNK") == 0) strcpy (Extension, "SEQ");
    if (strcmp (Extension, "CKY") == 0) strcpy (Extension, "SEQ");
    if (strcmp (Extension, "TAB") == 0) strcpy (Extension, "TCB");
    if (strcmp (Extension, "UAP") == 0) strcpy (Extension, "TCB");
  }

  // Try just the NAME.EXT in the current directory.

  strcpy (TempOutput, PureName);
  if (LE_DIRINI_TestIfFileExists (TempOutput))
      goto NormalExit;

  // If the extension exists, try looking in a directory with the
  // same name as the extension, off the current directory,
  // in other words look for "EXT\NAME.EXT".

  if (Extension[0] != 0 &&
  LE_DIRINI_CombinePathAndName (Extension /* as directory */,
  PureName, TempOutput))
  {
    if (LE_DIRINI_TestIfFileExists (TempOutput))
      goto NormalExit;
  }

  // Now try the user provided directory search paths.  For each path,
  // try sticking on the user's path name, then try it with just the
  // user's file name, then try it with the extension as a directory.

  for (PathIndex = 0; TRUE; PathIndex++)
  {
    if (!LE_DIRINI_ListPath (PathIndex, TestPath, PathListPntr))
      break; // Ran out of items in the path list.

    // Look for our path plus the user's path and name.

    if (TryOriginalName &&
    LE_DIRINI_CombinePathAndName (TestPath, InputName, TempOutput))
      if (LE_DIRINI_TestIfFileExists (TempOutput))
        goto NormalExit;

    // Check for just the name with our path.

    if (LE_DIRINI_CombinePathAndName (TestPath, PureName, TempOutput))
      if (LE_DIRINI_TestIfFileExists (TempOutput))
        goto NormalExit;

    // Try our path plus the extension plus the user's name.

    if (Extension[0] != 0)
    {
      strcat (TestPath, Extension);
      if (LE_DIRINI_CombinePathAndName (TestPath, PureName, TempOutput))
        if (LE_DIRINI_TestIfFileExists (TempOutput))
          goto NormalExit;
    }
  }

  // That didn't work, try the global search paths.

  for (PathIndex = 0; TRUE; PathIndex++)
  {
    if (!LE_DIRINI_ListPath (PathIndex, TestPath, LE_DIRINI_GlobalPathList))
      break; // Ran out of items in the path list.

    // Look for our path plus the user's path and name.

    if (TryOriginalName &&
    LE_DIRINI_CombinePathAndName (TestPath, InputName, TempOutput))
      if (LE_DIRINI_TestIfFileExists (TempOutput))
        goto NormalExit;

    // Check for just the name with our path.

    if (LE_DIRINI_CombinePathAndName (TestPath, PureName, TempOutput))
      if (LE_DIRINI_TestIfFileExists (TempOutput))
        goto NormalExit;

    // Try our path plus the extension plus the user's name.

    if (Extension[0] != 0)
    {
      strcat (TestPath, Extension);
      if (LE_DIRINI_CombinePathAndName (TestPath, PureName, TempOutput))
        if (LE_DIRINI_TestIfFileExists (TempOutput))
          goto NormalExit;
    }
  }

  // Didn't find the file.
  return FALSE;

NormalExit:
  // Convert the path name into absolute path form.
  ErrorCode = GetFullPathName (TempOutput, MAX_PATH, OutputName, &StringPntr);
  if (ErrorCode == 0 || ErrorCode > MAX_PATH)
    strcpy (OutputName, TempOutput); // Failed, so just copy our partial path.
  return TRUE;
}

#endif // CE_ARTLIB_EnableSystemDirIni
