#ifndef __L_SPEED_H__
#define __L_SPEED_H__

/*****************************************************************************
 *
 *   FILE:             SPEED.H
 *
 *   (C) Copyright 97 Artech Digital Entertainments.
 *                    All rights reserved.
 *
 *   Measures the speed of the computer.
 *
 * $Header: /Artlib_99/ArtLib/L_Speed.h 3     11/08/99 11:44a Russellk $
 *
 * $Log: /Artlib_99/ArtLib/L_Speed.h $
 * 
 * 3     11/08/99 11:44a Russellk
 * Added max virtual memory size  detection - used to see if the machine
 * is running low on space (check it instead of crashing)
 * 
 * 2     9/17/98 12:16p Agmsmith
 * Updated to work with ArtLib99.
 * 
 * 1     9/14/98 12:35p Agmsmith
 * Initial version copied over from ArtLib98 and renamed from .C to .CPP.
 * 
 * 3     8/13/98 4:57p Agmsmith
 * Avoid name conflict in headers.
 * 
 * 1     7/27/98 2:11p Lzou
 * for the CPU speed testing module
 * 
 * 7     10/08/97 3:25p Disc
 * 
 * 1     9/12/97 5:20p Agmsmith
 * CPU and system speed measurement.
 ****************************************************************************/

// Exported global variables.

extern BOOL LE_SPEED_MMX;
  /* TRUE if MMX instructions are available. */

extern BOOL LE_SPEED_16KDataCache;
  /* TRUE if this system has a 16K or larger data cache.  It is likely then
  that this is a Pentium II or Pentium Pro or future computer. */

extern DWORD LE_SPEED_IterationsPerMs;
  /* A crude CPU speed measurement in iterations per second.  Note that the
  values will vary widely between release and debug versions. */

#if _DEBUG
  #define SPEED_KLAMATH200_ITMS     20650
  #define SPEED_PENTIUMPRO200_ITMS  17730
  #define SPEED_INBETWEEN_ITMS      10000
  #define SPEED_PENTIUM90_ITMS       5200
#else /* Release version - optimized code. */
  #define SPEED_KLAMATH200_ITMS     45450
  #define SPEED_PENTIUMPRO200_ITMS  39100
  #define SPEED_INBETWEEN_ITMS      30000
  #define SPEED_PENTIUM90_ITMS      20400
#endif
  /* Some average speed values for comparing LE_SPEED_IterationsPerMs with. */


extern DWORD LE_SPEED_CDROM;
  /* Number of bytes per second when reading data files, 1X is 150000,
  4X (the minimum requirement) is 600000 and so on. */

extern DWORD LE_SPEED_RealRAM;
  /* Amount of real memory this system has.  You may want to avoid
  preloading too much if it is only 16 megabytes. */


extern DWORD LE_SPEED_MaxVirtualMemory;
  /* The maximum size the virtual memory can grow too.  You may
  want to warn the user if it is too low, wierd crashes result. */

enum LE_SPEED_OSEnum
{
  SPEED_OS_NONE           = -1,    // unknown operating system
  SPEED_OS_WIN32S         = 0,     // Win32 on Windows 3.1
  SPEED_OS_WIN95          = 1,     // Win32 on Windows 95
  SPEED_OS_WIN98          = 2,     // Win32 on Windows 98
  SPEED_OS_WINNT_351      = 3,     // Win32 on Windows NT 3.51
  SPEED_OS_WINNT_40       = 4,     // Win32 on Windows NT 4.0
  SPEED_OS_WINNT_50       = 5,     // Win32 on Windows NT 5.0
  SPEED_OS_MAX,
};
extern enum LE_SPEED_OSEnum LE_SPEED_OSInformation;


// User callable functions.

extern void LE_SPEED_OSVersionAndProcessorType ( void );
// Prints out the OS version and Processor type in a message box

extern void LE_SPEED_CalculateSpeed (LE_DATA_DataId  nDataID);
  /* Call this function to do the speed tests and update the global variables.
  Note that it requires that some other modules (like FLOC and the data file
  system) to be already working. */

#endif // __L_SPEED_H__
