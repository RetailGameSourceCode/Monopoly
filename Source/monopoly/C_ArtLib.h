#ifndef __C_ARTLIB_H__
#define __C_ARTLIB_H__

/*****************************************************************************
 *
 * FILE:        C_ArtLib.h
 *
 * DESCRIPTION: The user changeable library configuration file.  The various
 *              defines here turn on and off parts of the library and set
 *              compile time limits for array sizes etc.
 *
 * Only defines are allowed in this file (because it is included before any
 * other types are defined).
 *
 * Note that this may be broken up into C_Grafix.h, C_Sound.h and other system
 * modules if we ever decide that it is more convenient to do it that way.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Projects - 1999/Mono 2/C_ArtLib.h 52    29/09/99 5:13p Agmsmith $
 *****************************************************************************
 * $Log: /Projects - 1999/Mono 2/C_ArtLib.h $
 * 
 * 52    29/09/99 5:13p Agmsmith
 * Now have voice chat.  Note that it doesn't do anything unless you also
 * turn on the enable flag in the game .INI file.
 * 
 * 51    28/09/99 6:38p Agmsmith
 * 
 * 50    9/24/99 10:52a Russellk
 * More 3D models allowed
 * 
 * 49    9/20/99 2:03p Russellk
 * Gotta buffer music
 * 
 * 48    20/09/99 12:04p Agmsmith
 * Added Bink video player.
 *
 * 47    9/20/99 11:14a Russellk
 *
 * 46    9/16/99 2:27p Russellk
 *
 * 45    9/14/99 12:11p Russellk
 * Speed test dropped - not used.
 *
 * 44    9/13/99 4:09p Russellk
 * Reduced stop warnings as our sound system does stops on sounds which
 * may have already stopped.
 *
 * 43    9/08/99 11:05a Russellk
 * Monpoly cd now required
 *
 * 42    8/27/99 12:24p Russellk
 *
 * 41    8/20/99 10:58a Agmsmith
 * Added error message controls for SetPitch and SetCamera and
 * SetViewport.
 *
 * 40    8/20/99 10:25a Russellk
 *
 * 39    8/17/99 4:56p Russellk
 * Now requires a CD, soon will require one named monopoly.
 *
 * 38    8/13/99 10:41a Russellk
 * Global sound level lowered
 *
 * 37    8/11/99 12:12 Davew
 * Turned off the use of the old frame stuff for PC3D.
 *
 * 36    8/04/99 2:31p Russellk
 *
 * 35    8/03/99 3:05p Agmsmith
 * Added default sound level to compensate for poorly recorded sound
 * samples.
 *
 * 34    7/27/99 16:20 Davew
 * Disabled the new Frame/Mesh stuff
 *
 * 33    7/27/99 15:29 Davew
 * Added the CE_ARTLIB_3DUsingOldFrame #define
 *
 * 32    7/21/99 11:26a Russellk
 *
 * 31    7/20/99 11:05a Agmsmith
 *
 * 30    7/20/99 9:47a Agmsmith
 * Updated with new and delete override define.
 *
 * 29    7/19/99 2:38p Agmsmith
 * Updated to match current library version.
 *
 * 28    6/21/99 11:54a Timp
 * Turned off multitasking
 *
 * 27    6/11/99 11:31a Russellk
 *
 * 26    6/03/99 5:02p Timp
 * Added MONOPOLY2000 define for L_Main.cpp to detect.  L_Main has to do
 * editbox handling for the M2K chat module.
 *
 * 25    5/31/99 3:35p Russellk
 *
 * 24    5/30/99 4:16p Russellk
 * gaphics debug lines added
 *
 * 23    5/27/99 3:29p Davide
 * Added 200 extra runtime memory items for the custom board editor
 *
 * 22    5/12/99 9:53a Russellk
 *
 * 21    4/29/99 3:20p Russellk
 *
 * 20    27/04/99 19:32 Timp
 * Turn on Unicode
 *
 * 19    4/27/99 3:05p Russellk
 *
 * 18    4/27/99 2:56p Russellk
 *
 * 17    4/15/99 1:36p Kens
 * changed CE_ARTLIB_EnableSystemGrafix3D to TRUE
 *
 * 16    4/13/99 1:55p Russellk
 * Modifed to use setbackdrop & other demo defines
 *
 * 15    4/12/99 2:51p Russellk
 *
 * 14    4/09/99 17:38 Davew
 * Changed some video buffering settings
 *
 * 13    4/09/99 10:42 Davew
 * Enabled the system video
 *
 * 12    4/09/99 9:51 Davew
 * Added the new video defs
 *
 * 11    4/05/99 4:13p Russellk
 * Extra shade level support
 *
 * 10    3/30/99 1:45p Russellk
 *
 * 9     3/25/99 9:34a Russellk
 *
 * 8     3/24/99 9:41a Russellk
 *
 * 7     3/04/99 8:53a Russellk
 * IBar complete with Programmer graphics - 5 more (easy) bars to program
 * when finished graphics are ready.
 *
 * 6     2/19/99 4:14p Russellk
 * IBAR current player support for starter functions.  Colors go with
 * player, property bar running, mouseovers for everything.
 *
 * 5     2/17/99 8:45a Russellk
 *
 * 4     2/11/99 2:08p Russellk
 * New IBAR look & features begun.
 *
 * 3     2/03/99 2:52p Russellk
 * Library update
 *
 * 34    2/02/99 12:15p Agmsmith
 * More chunky options.
 *
 * 33    2/01/99 4:49p Agmsmith
 * More chunky options.
 *
 * 32    2/01/99 2:54p Agmsmith
 * Adding sequencer commands for setting video attributes.
 *
 * 31    1/27/99 7:35p Agmsmith
 * Optionally prompt the user when saving files overwrites the file.
 *
 * 30    1/26/99 12:38p Agmsmith
 * Only enable the DopeTab converter if the user wants it.
 *
 * 29    1/19/99 12:05p Agmsmith
 *
 * 28    1/19/99 11:58a Agmsmith
 *
 * 27    1/19/99 11:52a Agmsmith
 * Remove unnecessary features.
 *
 * 26    1/18/99 2:00p Agmsmith
 * Documentation improvements.
 *
 * 25    1/17/99 2:38p Agmsmith
 * Adding DirIni module: directory paths and .INI files.
 *
 * 24    1/16/99 4:42p Agmsmith
 * Allow optional use of old blitter routines.
 *
 * 20    1/07/99 6:32p Agmsmith
 * More changes for Dope Sheet - GrowChunk related.
 *
 * 18    12/09/98 5:28p Agmsmith
 * Added URL data source.
 *
 * 17    11/02/98 5:21p Agmsmith
 * Added external file data source.
 *
 * 16    10/29/98 6:37p Agmsmith
 * New improved memory and data system added.  Memory and data systems
 * have been separated.  Now have memory pools, and optional corruption
 * check.  The data system has data groups rather than files and
 * individual items can come from any data source. Also has a least
 * recently used data unloading system (rather than programmer set
 * priorities).  And it can all be turned off for AndrewX!
 *
 * 1     9/15/98 10:05a Agmsmith
 * Library control defines now in a separate file from the main program
 * exports, so that they can be included early without worrying about
 * defining data types.
 ****************************************************************************/

// What game am I?  L_Main.cpp needs to know for Monopoly 2000 chat stuff.
#define MONOPOLY2000 "Yes, this is the Monopoly Game."

/*****************************************************************************
 * For the following flags set the define to 1 if you want the mode enabled
 * or set the define to 0 if you want the mode disabled.
 */

#define CE_ARTLIB_EnableDebugMode                 1
// - Enable this flag for debug mode (enables sanity checking).

#define CE_ARTLIB_PlatformWin95                   1   /* Win95 & NT5 */
#define CE_ARTLIB_PlatformPSX                     0   /* Sony Playstation */
#define CE_ARTLIB_PlatformDreamcast               0   /* Sega Dreamcast - WinCE plus DirectX */
#define CE_ARTLIB_PlatformWinCE                   0   /* Plain WinCE without DirectX */
// Enables the library for various destination platforms.
// Currently only Win95 is supported and therefore should be the only one set,
// but hopefully some of the modules can be reused for both.  But only if
// people stop trying to reinvent the wheel!  Argh.

#define CE_ARTLIB_UseEnumDataTypes                1
// Some compilers can't handle variables that are declared as being enum
// types, instead use an integer to hold them (but the enum will still
// be used to define the constants).  Other compilers allow enums and
// even show the symbolic names for values in the debugger.

#define CE_ARTLIB_UnicodeAlphabet                 1
// Controls which character set is used for ACHAR and related functions.
// If 1 then Unicode and 16 bit characters are used (useful for doing
// international games), 0 for ordinary ASCII.

#define CE_ARTLIB_EnableMultitasking              0
// Enable this flag to turn on Multitasking (game thread separate from the
// animation thread, so that you get smoother animations).  Disable it for
// games that have a main loop that runs once per frame (input, compute,
// draw, repeat).

#define CE_ARTLIB_DO_NOT_OVERRIDE_NEW_AND_DELETE  (0 || !CE_ARTLIB_EnableSystemGrafix3D)
// Set this to non-zero to disable the overridden new and delete operators
// in PC3D, which can cause problems when compiling with MFC which also
// overrides the global new and delete operators.  Automatically turns it
// off when not using the PC3D stuff.



/*****************************************************************************
 * Enable flags for the various systems (and sub-systems) contained in the
 * library.  If you wish to remove a system set the flag to zero and recompile
 * library from scratch.  An unused system removed from memory with provide
 * smaller executable code.  Most systems are fairly independent but some are
 * not. For example you cannot remove the Memory system without removing the
 * Data system (it will be obvious at link time which one you are missing).
 *
 * Please add new systems in alphabetical order to this list.
 */

#define CE_ARTLIB_EnableSystemBink                0 /* The Bink video player, $4000 per game license fee, use Bink or Video, not both.  Also add BinkW32.lib to your link settings and BinkW32.dll to your install program. */
#define CE_ARTLIB_EnableSystemBinkStandAlone      1 /* Enables the stand-alone Bink player LE_SEQNCR_TakeOverAndPlayBinkVideo */
#define CE_ARTLIB_EnableSystemButton              0
#define CE_ARTLIB_EnableSystemChunk               1
#define CE_ARTLIB_EnableSystemData                1
#define CE_ARTLIB_EnableSystemDirIni              1
#define CE_ARTLIB_EnableSystemFonts               1
#define CE_ARTLIB_EnableSystemGrafix2D            1
#define CE_ARTLIB_EnableSystemGrafix3D            1
#define CE_ARTLIB_EnableSystemGrafix              (CE_ARTLIB_EnableSystemGrafix2D || CE_ARTLIB_EnableSystemGrafix3D)
#define CE_ARTLIB_EnableSystemJoypad              0
#define CE_ARTLIB_EnableSystemKeybrd              1
#define CE_ARTLIB_EnableSystemMain                1
#define CE_ARTLIB_EnableSystemMatrix              1
#define CE_ARTLIB_EnableSystemMemory              1
#define CE_ARTLIB_EnableSystemMidi                0
#define CE_ARTLIB_EnableSystemMouse               1
#define CE_ARTLIB_EnableSystemMMX                 1
#define CE_ARTLIB_EnableSystemPrint               0
#define CE_ARTLIB_EnableSystemQueue               1
#define CE_ARTLIB_EnableSystemRend0D              1
#define CE_ARTLIB_EnableSystemRend2D              1
#define CE_ARTLIB_EnableSystemSequencer           1
#define CE_ARTLIB_EnableSystemSound               1
#define CE_ARTLIB_EnableSystemSpeed               0
#define CE_ARTLIB_EnableSystemSprite              0
#define CE_ARTLIB_EnableSystemTimers              1
#define CE_ARTLIB_EnableSystemUIMsg               1
#define CE_ARTLIB_EnableSystemVideo               1



/*****************************************************************************
 * Parameters and other compile time settings for the various systems, again
 * in alphabetical order by system.
 */

// Button defines.
#define CE_ARTLIB_ButtonsUseUIMSGInput            1
// 1 to use messages from the L_UIMSG module for setting mouse position, 0 for
// using ordinary Windows messages.  If ordinary Windows messages are used then
// the button code is called from a different thread than the game and can
// push buttons while the game isn't expecting them.  If you use UIMSG mode then
// your game code has to call the LI_BUTTON_ProcessInputMessage.
#define CE_ARTLIB_NumberOfButtonLists             1
// Defines the number of button lists to allocate. Each button list can have
// any number of buttons. Buttons are given priority based on the button list
// they are contained in and the order of the buttons within each button list.
// Button lists are specified by a number between 0 and BUTTON_NUMBER_OF_BUTTON_LISTS - 1,
// where button list N has a higher priority than button list N+1. This means all
// buttons in button list N have a higher priority than all buttons in button list
// N+1. Further, in any button list N, buttons are ordered from 0 to the number of
// buttons in the list minus 1 where similarly, button X in the list will have a higher
// priority than button X+1 in the list. It is important to keep this in mind when
// designing button lists because unless a button is transparent, processing stops
// after one button is triggered.
// You can define any number of button lists you want. However the more
// button lists, and thus more buttons are defined, the more memory
// is required and also more processing time.


// Chunky file stuff.
#define CE_ARTLIB_EnableChunkWriting              0
// If you don't need to write chunks to files or DataIDs then turn this off.
#define CE_ARTLIB_EnableChunkGrow                 0
// Enables the routine for inserting and deleting inside a chunky DataID.
#define CE_ARTLIB_EnableChunkManipulation         0
// Enables the routines used by the dope sheet for manipulating chunky
// sequences (things like creating a new sequence, adding frames, ...).
#define CE_ARTLIB_EnableChunkDumpLoad             0
// Turns on routines for dumping chunks to a text file and loading them
// from a text file.
#define CE_ARTLIB_EnableChunkFileLoading          0
// Turns on code used for loading chunky files by file name, and processing
// the file name subchunks while loading.
#define CE_ARTLIB_MaxOpenChunkStreams             4
// Kind of like maximum number of open files, except for chunk readers.
#define CE_ARTLIB_MaxChunkNestingLevels           8
// A multiple of 4 is good.
#define CE_ARTLIB_ChunkReadFileBufferSize         (1 * 1024)
// Should be large enough to hold the largest possible lowest level subchunk.


// Data System Defines
#define CE_ARTLIB_DataMaxGroups                   16
// Number of data groups (kind of like the old data files) that can exist,
// user groups are numbered from 1 to this value minus 1.  There is also a
// special data group for in-memory items just after the user groups.  The
// high word in a DataID thus can be from 1 to CE_ARTLIB_DataMaxGroups
// inclusive.
#define CE_ARTLIB_DataMaxRuntimeMemoryItems       300
// Maximum number of runtime created data items simultaneously allocated,
// for things like temporary bitmaps identified by a DataID.  In other
// words, this is the size of the memory (and other sources) data group,
// that's the group with index number equal to CE_ARTLIB_DataMaxGroups.
#define CE_ARTLIB_DataMemorySource                1
#define CE_ARTLIB_DataDataFileSource              1
#define CE_ARTLIB_DataExternalFileSource          1
#define CE_ARTLIB_DataURLSource                   0
// These enable and disable the various different kinds of data source,
// mostly to save space by not compiling the code needed for ones you
// don't use.
#define CE_ARTLIB_DataEnableDataWriting           0
// Enables the code which can write out changed data items.  This also
// enables code which converts internal formats into external file
// formats (such as a bitmap to PNG converter).  This is useful for
// tools like the dope sheet, but not needed for most games (unless
// you want to conveniently do a screen dump to a file).
#define CE_ARTLIB_DataAutoUnloadWhenSpaceNeeded   1
// If TRUE then the data system will automatically unload data items until it
// has enough free memory space in the data pool, when it is loading some
// other data item.  The least recently used item(s) will be the one(s) to be
// unloaded.  If FALSE (zero) then you just run out of memory when loading
// when there isn't any free memory.  Items with reference counts aren't
// unloaded in any case.  Also, if FALSE, there will be less CPU overhead when
// accessing data items (moving item to top of doubly linked list) and less
// memory used (4 bytes per item for previous pointer, next is still used).
#define CE_ARTLIB_DataNameHashTable               0
// Enable this to turn on the hash table which lets you find DataIDs based
// on the file name.  Useful for utilities like the dope sheet which don't
// have a data file and DataIDs, just file names linking data together.
#define CE_ARTLIB_DataDopeTABSupport              0
// Enable this to allow direct reading of TAB files (8 bit with alpha
// transparency) and internal conversion to UAP (NEWBITMAPHEADER) format.
// Also reads 8 bit BMP images and converts them to UAPs too.
#define CE_ARTLIB_DataPromptWhenSavingFiles       1
// Enable this to make the external file data source ask for permission
// when it saves to a file.
#define CE_ARTLIB_DataCollectUsageStatistics      0
// Set to 1 to collect the usage statistics and dump them to the debug log
// when data files are closed etc.  Only works in debug mode.  0 for none.



// Grafix system defines
#define CE_ARTLIB_InitialScreenWidth              800
#define CE_ARTLIB_InitialScreenHeight             600
#define CE_ARTLIB_InitialScreenDepth              16 // It will try to get this depth.  But will take any of the below.
#define CE_ARTLIB_InitialAcceptableDepths         (LE_GRAFIX_16_DEEP_BIT | LE_GRAFIX_24_DEEP_BIT | LE_GRAFIX_32_DEEP_BIT)
#define CE_ARTLIB_GrafixShowBoundingBoxes         0 // Set to 1 to flash the bounding boxes on the screen when updating.
#define CE_ARTLIB_GrafixShowBoundingBoxesDelay    10 // Time delay to show the boxes before they get erased.
// - Requested screen size for this application.


// Bit block copy routine defines.
#define CE_ARTLIB_BlitBoundsCheck                 0
// Set to 1 to turn on bounding box checks while bliting - warns you if you are
// copying outside the bitmap.  Leave zero or don't use debug mode to turn it off.
#define CE_ARTLIB_BlitUseGreenColourKey           1
#define CE_ARTLIB_BlitUseBlueColourKey            0
// Which colour to use for the ColorKeyPureGreenBitBlt series
// of functions.  Note that the alpha blit still uses the old
// pure green only code (there's too much stuff to fix).
// Pick only one of these, or pick none if you want black to
// be the transparent colour.  Also see LE_BLT_KeyRedLimit etc.
#define CE_ARTLIB_BlitUseOldRoutines              0
// Turn this on to use the older blitter routines, they are slightly
// faster (use MMX) for the 8 bit to N bit copies but have math problems
// (can't draw 1 pixel tall, cuts off half a pixel on the edges of
// source images, can't scale true colour images).  The new routines
// also have problems, mostly with clipping and a confusing API (needs
// to be rethought).
#define CE_ARTLIB_BlitUse255AlphaLevels           1
// To enable full 255 levels of alpha channel effects instead of normal
// 16 alpha levels. The new 255 alpha levels ranges from 0 (transparent),
// 1*255/255, 2*255/255, ..., 255*255/255. This works for both bitmap
// unity and stretch copies of 16to16, 24to24, and 32to32 bit colour depths.


// Main program defines.
#define CE_ARTLIB_GraficsThreadPriority           THREAD_PRIORITY_BELOW_NORMAL
#define CE_ARTLIB_GameThreadPriority              THREAD_PRIORITY_LOWEST
// Note that the window message processing thread is at the normal thread
// priority.  All other threads must be at lower priorities otherwise the
// gradual windows slowdown bug will happen (as the windows kernel queue
// grows larger and larger).
#define CE_ARTLIB_ApplicationName                 "Monopoly"
// String given as your window class name and used for the window title too,
// and copied to the LE_MAIN_ApplicationNameString variable (which you should
// use instead, to avoid wasting memory).
#define CE_ARTLIB_DirIniINIFileName               "Monopoly.ini"
// Name you want to use for your .INI user preferences file.  This file will
// be in the same directory as the program executable.  Used for the global
// search path list as well as your stuff.
#define CE_ARTLIB_DirIniCDROMVolumeName           "Monopoly"
// This is the disk label on the CD-ROM for the game.  The program will look
// for a CD-ROM drive with a disk in it with this label (in case the user has
// several drives, and for copy protection).  You set the label when you make
// the CD-ROM.  Then, if found, the magic path name of "CD:" will be replaced
// with the actual drive letter of the CD-ROM.  Use "" if your game will work
// with any CD-ROM and NULL if you don't want to check for a CD-ROM.
#define WM_ENDGAME      (WM_USER + 0x1100)
// Add user defined Windows messages here.


// Memory System Defines.
#define CE_ARTLIB_MemoryUserPoolInitialSize       ( 1*1024*1024)
#define CE_ARTLIB_MemoryDataPoolInitialSize       (18*1024*1024)
#define CE_ARTLIB_MemoryDataPoolMaxSize           (30*1024*1024)
#define CE_ARTLIB_MemoryLibraryPoolInitialSize    ( 2*1024*1024)
// The initial sizes of the standard memory pools.  The User one is used for
// runtime allocated memory (but not runtime data items).  The Data pool is
// used by the data system only (included runtime created data items), it can
// deallocate items when it needs space and reallocate and reload them later
// when it needs the item.  The user pool on the other hand has to stick
// around because it can't be reloaded, which is why it is separate.
#define CE_ARTLIB_MemoryMaxNumberOfPools          3
// Number of memory pools that the system can keep track of, should be at
// least 3 for the standard user, library and data pools.
#define CE_ARTLIB_MemoryFillWithGarbage           (1 && CE_ARTLIB_EnableDebugMode)
#define CE_ARTLIB_MemoryBoundsCheckMargin         20
// Fills memory with $AA when freshly allocated, fills it with
// $BB when it is deallocated.  Useful for debugging.  Also,
// CE_ARTLIB_MemoryBoundsCheckMargin bytes before and after the
// allocated block are filled with a serial number that gets
// checked during deallocation to see if you are overwriting
// memory.


// Keyboard system defines.
#define CE_ARTLIB_KeyboardPressedAndLatchedArray  1
// Set to nonzero if you want to use the array of latched keys feature.
// Normally you should use the UIMSG keyboard messages, since using
// the arrays can lead to lost keys (like when the user hits the same
// key twice while you are busy doing something else, or hits multiple
// keys and you then can't tell which one was first).

// Mouse system defines
#define CE_ARTLIB_MousePositionMessages           1
// Turn on if you want UIMSG_MOUSE_MOVED messages, which can slow you down
// since there are a lot of them (about 60 per second).  If you use the
// button module and you want roll-over buttons (ones that semi-press when
// you wave the mouse over them), then you need to turn this on.
// Note that even with this off, you still get mouse click messages.
#define CE_ARTLIB_MousePressedAndLatchedArray     1
// Use this if you wish to monitor mouse activity without using messages,
// though it isn't as reliable as the messages since you can miss clicks
// and get the order of clicking wrong.



// Sequencer (new animation engine) defines.
#define CE_ARTLIB_SeqncrMax2DSequences            400 /* No maximum */
#define CE_ARTLIB_SeqncrMax3DSequences            150 /* No maximum */
#define CE_ARTLIB_SeqncrMaxActiveSequences        (CE_ARTLIB_SeqncrMax2DSequences + CE_ARTLIB_SeqncrMax3DSequences)
#define CE_ARTLIB_SeqncrMaxChainedThings          50  /* No maximum */
#define CE_ARTLIB_SeqncrMaxCollectedCommands      500  /* No maximum */
#define CE_ARTLIB_SeqncrMaxLabels                 10  /* Max 255 */
#define CE_ARTLIB_SeqncrMaxWatches                12  /* Max 255 */
#define CE_ARTLIB_SeqncrMaxRecolourRanges         0   /* Keep this small for speed, zero to turn off this feature completely. */
#define CE_ARTLIB_SeqncrWaitForAllFinishDelay     500 /* Zero for off, else #ticks, see LE_SEQNCR_WaitForAllSequencesFinished for more. */
#define CE_ARTLIB_SeqncrUseVerticalBlank          0   /* Makes renderers wait for vertical blank before copying to screen. */
#define CE_ARTLIB_SeqncrShowFrameRatePeriod       60  /* Displays frame rate every N ticks (60 for one update per second).  The bar graph on the left side of the screen shows 100% when at 60 frames per second.  Use zero to turn it off. */
#define CE_ARTLIB_SeqncrDefaultSoundLevelPercent  32  /* The default volume level to use when starting sounds.  Varies between 0 for quiet and 100 for full volume. */
#define CE_ARTLIB_SeqncrUseSerialNumbers          0   /* Uses serial numbers to identify sequences rather than label numbers, in certain functions, mostly for dope sheet use. */
#define CE_ARTLIB_SeqncrChainStopsAllSameIDAndPri 0   /* On or Off */
#define CE_ARTLIB_SeqncrStayAtEndLowUpdateRate    1   /* 1 to make sequences which stay at their end update only once every 4 seconds, rather than their normal rate.  Useful for cutting down on CPU time if you have a lot of single frame sequences. */
#define CE_ARTLIB_SeqncrImmediateCommandExecution 0   /* 1 to make it execute your commands as they are submitted, useful for debugging without the command queue creating confusion.  Works best in single tasking mode. */
#define CE_ARTLIB_SeqncrIgnoreStopErrors          1   /* 0=none, 1=debug file, 2=file & display */
#define CE_ARTLIB_SeqncrIgnoreMoveErrors          2
#define CE_ARTLIB_SeqncrIgnorePauseErrors         2
#define CE_ARTLIB_SeqncrIgnoreSetClockErrors      2
#define CE_ARTLIB_SeqncrIgnoreSetEndingActErrors  2
#define CE_ARTLIB_SeqncrIgnoreChainIntrErrors     2
#define CE_ARTLIB_SeqncrIgnoreForceRedrawErrors   2
#define CE_ARTLIB_SeqncrIgnoreSetVolumeErrors     2
#define CE_ARTLIB_SeqncrIgnoreSetPanErrors        2
#define CE_ARTLIB_SeqncrIgnoreSetPitchErrors      2
#define CE_ARTLIB_SeqncrIgnoreSetSaturationErrors 2
#define CE_ARTLIB_SeqncrIgnoreSetBrightnessErrors 2
#define CE_ARTLIB_SeqncrIgnoreSetContrastErrors   2
#define CE_ARTLIB_SeqncrIgnoreSetUpVidAltErrors   2
#define CE_ARTLIB_SeqncrIgnoreChooseVidAltErrors  2
#define CE_ARTLIB_SeqncrIgnoreForgetVidAltErrors  2
#define CE_ARTLIB_SeqncrIgnoreSetViewportErrors   2
#define CE_ARTLIB_SeqncrIgnoreSetCameraErrors     2

// General render defines.
#define CE_ARTLIB_RendMaxRenderSlots              5 /* Maximum of 8 due to byte size RenderSlotSet, keep as small as possible to save space. */

// Rend3D system defines
#define CE_ARTLIB_Max3DModels                32
#define CE_ARTLIB_Max3DTextures              32
#define CE_ARTLIB_Max3DTextureArrays         32
// Just array size limits for the hack which maps PC3D pointers into DataIDs.
#define CE_ARTLIB_NumLights                   4
// Indicates we are using the old frame/mesh classes in PC3D
#define CE_ARTLIB_3DUsingOldFrame             0


// Sound system defines.  Note that current values are in LE_SOUND_CurrentHardwareHz etc.
#define CE_ARTLIB_SoundHardwareHz                 22050
// Desired speed of the output mixed sound.
#define CE_ARTLIB_SoundHardwareBits               16
// Desired bits per sample of the output mixed sound.
#define CE_ARTLIB_SoundHardwareChannels           1
// 2 for stereo, 1 for mono output.
#define CE_ARTLIB_SoundEnable3DSound              0
// Non-zero to enable the use of 3D sounds.
#define CE_ARTLIB_SoundMaxNonSpooledSound         (90 * CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ)
// Spooling seems to kill performance - only a couple background sounds get this long.
// Sounds this long or shorter get fully decompressed and loaded into memory
// (their own DirectSound buffer) and not spooled.  They are also eligible for
// caching.
#define CE_ARTLIB_SoundSpoolBufferDuration        (7 * CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ)
// Spooled sounds use a DirectSound buffer this long, should be as long as the
// longest delay between feedings.  On a P-90 running the Pony game over the
// network this can be 7 seconds.
#define CE_ARTLIB_SoundMaxCachedDirectSounds      7
// Number of non-spooled data file based sounds that can have their
// DirectSound buffers cached for quick replay.  Use zero to turn off this
// caching feature and associated code.
#define CE_ARTLIB_SoundEnableVoiceChat            1
// Non-zero to enable the voice chat feature.  Of course, you also
// have to check for data and send it across the network for it to work.
#define CE_ARTLIB_SoundMaxChatsReceivable         10
// Maximum number of voice chats you expect to be listening to
// simultaneously, usually equal to the maximum number of players
// plus a few spectators.  A sound buffer will be allocated for
// each one up to this maximum.  Don't make it too large since it
// does a linear search to map player IDs to buffers.


// Timer system defines
#define CE_ARTLIB_MaxTimersArray                  4
// Just how many timers do you want to use today?
// Extra ones will slow down your game a bit.
#define CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ        60
// Set this to the standard update rate for ticks.  This is 60 for 60hz
// that comes from North American TV and power system update rates.
// In some European countries it is 50.


// User Interface Message system defines.
#define CE_ARTLIB_UIMsgQueueSize                  100  /* No maximum */
// Size of the event queue in messages.


// Video system defines
#define CE_ARTLIB_VideoMultiTaskingEnabled        1
// 1 to enable multi-tasking mode for avi data buffering
// 0 to switch to normal single-threaded video player
#define CE_ARTLIB_VideoFrameJumpModeEnabled       0
// 0 -- switch on normal video playback mode
// 1 -- switch on video frame jump playback mode
#define CE_ARTLIB_VideoAVIBufferNumber            8
// Number of preallocated avi data buffers for avi data preloading.
// Normally, 3 avi data buffers are enough. More avi data buffers may
// be helpful when sometimes playing back an enormous big video clip
// or on a slow computer.
#define CE_ARTLIB_VideoMultipleSecondsData        1
// This instructs the video player to roughly each time buffer a
// user specified (MULTIPLE_SECONDS_DATA) seconds of avi data. Thus,
// the video player buffers the following amount of data (in seconds)
// CE_ARTLIB_VideoAVIBufferNumber * CE_ARTLIB_VideoMultipleSecondsData.
#define CE_ARTLIB_VideoViewDataBufferState        0
// This is only for debugging/testing purpose, showing how many avi data
// buffers are used, how much amount of avi data are preloaded, and how they
// are fed. This is useful when playing back video clips on a slow machine.
// Four colours are used to show the video player's buffering states.
//    yellowish green  -- current (playing) avi data buffer
//    black            -- empty avi data buffer
//    blue             -- freshly loaded avi data (both audio and video data)
//    red              -- buffer with valid video data (audio data are fed)
//

// Init system Defines
#define CE_ARTLIB_InitShutdownMessageBoxDisplay   1
// When set to 0 the game shutdown MessageBox is not displayed.
// Available in debug mode only.

#endif // __C_ARTLIB_H__
