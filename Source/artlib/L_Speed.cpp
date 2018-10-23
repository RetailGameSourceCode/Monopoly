/*****************************************************************************
 *
 *   FILE:             SPEED.C - Measure the computer's speed.
 *
 *   (C) Copyright 97  Artech Digital Entertainments.
 *                     All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Speed.cpp 5     11/08/99 11:44a Russellk $
 *
 * $Log: /Artlib_99/ArtLib/L_Speed.cpp $
 * 
 * 5     11/08/99 11:44a Russellk
 * Added max virtual memory size  detection - used to see if the machine
 * is running low on space (check it instead of crashing)
 * 
 * 4     4/11/99 3:50p Agmsmith
 * Now postload converts BMPs into native screen depth bitmaps, rather
 * than having the sequencer do it.  So remove all BMP things everywhere.
 *
 * 3     10/29/98 6:36p Agmsmith
 * New improved memory and data system added.  Memory and data systems
 * have been separated.  Now have memory pools, and optional corruption
 * check.  The data system has data groups rather than files and
 * individual items can come from any data source. Also has a least
 * recently used data unloading system (rather than programmer set
 * priorities).  And it can all be turned off for AndrewX!
 *
 * 2     9/17/98 12:16p Agmsmith
 * Updated to work with ArtLib99.
 *
 * 4     8/14/98 4:01p Agmsmith
 * Only show messages in debug mode.
 *
 * 1     7/27/98 2:11p Lzou
 * for the CPU speed testing module
 *
 * 10    10/04/97 4:52p Disc
 * Added Global defines for memoryloading priorities.
 *
 * 1     9/12/97 5:20p Agmsmith
 * CPU and system speed measurement.
 ************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/
#include "l_inc.h"

#if CE_ARTLIB_EnableSystemSpeed
/************************************************************/

/************************************************************/
/* STRUCTURES AND TYPEDEFS                                  */
/************************************************************/
/************************************************************/

/************************************************************/
/* GLOBALS                                                  */
/************************************************************/

BOOL LE_SPEED_MMX;
  /* TRUE if MMX instructions are available. */

BOOL LE_SPEED_16KDataCache;
  /* TRUE if this system has a 16K or larger data cache.  It is likely then
  that this is a Pentium II or Pentium Pro or future computer. */

DWORD LE_SPEED_IterationsPerMs;
  /* A crude CPU speed measurement in iterations per millisecond.  Note
  that the values will vary widely between release and debug versions. */

DWORD LE_SPEED_CDROM;
  /* Number of bytes per second when reading data files, 1X is 150000,
  4X (the minimum requirement) is 600000 and so on. */

DWORD LE_SPEED_RealRAM;
  /* Amount of real memory this system has.  You may want to avoid
  preloading too much if it is only 16 megabytes. */

DWORD LE_SPEED_MaxVirtualMemory;
  /* The maximum size the virtual memory can grow too.  You may
  want to warn the user if it is too low, wierd crashes result. */


enum LE_SPEED_OSEnum LE_SPEED_OSInformation = SPEED_OS_NONE;
  /* Which operating system is this running under. */


/************************************************************/


/*********************************************************
*
*  void LE_SPEED_OSVersionAndProcessorType(void)
*
*  DESCRIPTION:
*    Prints out the OS version and Processor type.
*
*/

#define LE_SPEED_SHOWMESSAGE (1 && CE_ARTLIB_EnableDebugMode)

void LE_SPEED_OSVersionAndProcessorType ( void )
{
  OSVERSIONINFO   MyOSInfo;
  char            OsPlatformString[MAX_PATH];
  char            OsVersionString[50];

  SYSTEM_INFO     MySystemInfo;
  char            MyProcessorString[MAX_PATH];

  // get OS information

  memset(&MyOSInfo, 0, sizeof(MyOSInfo));
  MyOSInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&MyOSInfo);

  // get the OS platform

  if( MyOSInfo.dwPlatformId == VER_PLATFORM_WIN32s )  // Win32 on Windows 3.1
  {
    strcpy(OsPlatformString, "Win32s on Windows 3.1");
    LE_SPEED_OSInformation = SPEED_OS_WIN32S;
  }
  else if( MyOSInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS &&
    MyOSInfo.dwMinorVersion == 0 )    // Win32 on Windows 95
  {
    strcpy(OsPlatformString, "Win32 on Windows 95");
    LE_SPEED_OSInformation = SPEED_OS_WIN95;
  }
  else if( MyOSInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS &&
    MyOSInfo.dwMinorVersion == 10 )   // Win32 on Windows 98
  {
    strcpy(OsPlatformString, "Win32 on Windows 98");
    LE_SPEED_OSInformation = SPEED_OS_WIN98;
  }
  else if( MyOSInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)  // Win32 on Windows NT
  {
    strcpy(OsPlatformString, "Win32 on Windows NT");

    // check for Windows NT major and minor version numbers
    if( MyOSInfo.dwMajorVersion == 3 && MyOSInfo.dwMinorVersion == 51 ) // NT 3.51
      LE_SPEED_OSInformation = SPEED_OS_WINNT_351;
    else if( MyOSInfo.dwMajorVersion == 4 && MyOSInfo.dwMinorVersion == 0 ) // NT 4.0
      LE_SPEED_OSInformation = SPEED_OS_WINNT_40;
    else if( MyOSInfo.dwMajorVersion == 5 && MyOSInfo.dwMinorVersion == 0 ) // NT 5.0
      LE_SPEED_OSInformation = SPEED_OS_WINNT_50;
  }
  else
  {
    strcpy(OsPlatformString, "an unknown platform");
    LE_SPEED_OSInformation = SPEED_OS_NONE;
  }

  // get the OS extra information
  if( strlen(MyOSInfo.szCSDVersion) != 0 )
  {
    strcat(OsPlatformString, "." );
    strcat(OsPlatformString, MyOSInfo.szCSDVersion );
  }

  // get OS's major/minor version numbers
  sprintf(OsVersionString, "Version Number: %d.%d\n",
    MyOSInfo.dwMajorVersion, MyOSInfo.dwMinorVersion);

  strcat(OsPlatformString, "\n");
  strcat(OsPlatformString, OsVersionString);

  // get system information

  memset(&MySystemInfo, 0, sizeof(SYSTEM_INFO));

  GetSystemInfo(&MySystemInfo);

  // processor type or architecture
  if( MyOSInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) // Windows 95
  {
    switch (MySystemInfo.dwProcessorType)
    {
    case PROCESSOR_INTEL_386:
      strcpy(MyProcessorString, "Processor: Intel 386");
      break;

    case PROCESSOR_INTEL_486:
      strcpy(MyProcessorString, "Processor: Intel 486");
      break;

    case PROCESSOR_INTEL_PENTIUM:
      strcpy(MyProcessorString, "Processor: Intel Pentium");
      break;

    default:
      strcpy(MyProcessorString, "Processor: Unknown");
    }
  }
  else if( MyOSInfo.dwPlatformId == VER_PLATFORM_WIN32_NT ) // Windows NT
  {
    if( MySystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL )
    {
      switch (MySystemInfo.wProcessorLevel)
      {
      case 3:  // Intel 80386
        strcpy(MyProcessorString, "Processor: Intel 80386");
        break;

      case 4: // Intel 80486
        strcpy(MyProcessorString, "Processor: Intel 80486");
        break;

      case 5: // Pentium
        strcpy(MyProcessorString, "Processor: Intel Pentium");
        break;

      default:
        strcpy(MyProcessorString, "Processor: Unknown");
      }
    }
    else if( MySystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_MIPS )
    {
      switch (MySystemInfo.wProcessorLevel)
      {
      case 4:  // MIPS 4000
        strcpy(MyProcessorString, "Processor: MIPS 4000");
        break;

      default:
        strcpy(MyProcessorString, "Processor: Unknown");
      }
    }
    else if( MySystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA )
    {
      switch(MySystemInfo.wProcessorLevel)
      {
      case 21064:  // Alpha 21064
        strcpy(MyProcessorString, "Processor: Alpha 21064");
        break;

      case 21066:  // Alpha 21066
        strcpy(MyProcessorString, "Processor: Alpha 21066");
        break;

      case 21164:  // Alpha 21164
        strcpy(MyProcessorString, "Processor: Alpha 21164");
        break;

      default:
        strcpy(MyProcessorString, "Processor: Unknown");
      }
    }
    else if( MySystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_PPC )
    {
      switch(MySystemInfo.wProcessorLevel)
      {
      case 1:  // PPC 601
        strcpy(MyProcessorString, "Processor: PPC 601");
        break;

      case 3:  // PPC 603
        strcpy(MyProcessorString, "Processor: PPC 603");
        break;

      case 4:  // PPC 604
        strcpy(MyProcessorString, "Processor: PPC 604");
        break;

      case 6:  // PPC 603+
        strcpy(MyProcessorString, "Processor: PPC 603+");
        break;

      case 9:  // PPC 604+
        strcpy(MyProcessorString, "Processor: PPC 604+");
        break;

      case 20:  // PPC 620
        strcpy(MyProcessorString, "Processor: PPC 620");
        break;

      default:
        strcpy(MyProcessorString, "Processor: Unknown");
      }
    }
  }

  // prints out the OS info and processor type

#if LE_SPEED_SHOWMESSAGE
  strcpy(LE_ERROR_DebugMessageBuffer, OsPlatformString);
  strcat(LE_ERROR_DebugMessageBuffer, "\n");
  strcat(LE_ERROR_DebugMessageBuffer, MyProcessorString);
  MessageBox( NULL, LE_ERROR_DebugMessageBuffer, "OS info and Processor type", MB_SYSTEMMODAL | MB_OK);
#endif
}


/*****************************************************************************
 * Call this function to do the speed tests and update the global variables.
 * Note that it requires that some other modules (like FLOC and the data file
 * system) to be already working.
 *
 * nDataID should be a data id of a relatively large image, say 800x600x24,
 * to work out how long it takes to load up a big image.
 */

void LE_SPEED_CalculateSpeed ( LE_DATA_DataId nDataID )
{
  #define         SIXTEEN_K (1024 * 16)
  volatile DWORD  BigArray [SIXTEEN_K / 4];
  DWORD           BytesLoaded;
  DWORD           ElapsedTime;
//  DWORD           ElapsedLarge;
  DWORD           ElapsedSmall;
  long            i;
  long            index;
  long            Iterations;
  long            ModuloLimit;
  MEMORYSTATUS    MyMemoryStatus;
  BOOL            bLoadingFlag;

  /* Do we have MMX available? */

#if CE_ARTLIB_EnableSystemMMX
  LE_SPEED_MMX = LE_MMX_Is_Available ();
#else
  LE_SPEED_MMX = FALSE;
#endif

  /* How much real memory is there? */

  memset (&MyMemoryStatus, 0, sizeof (MyMemoryStatus));
  MyMemoryStatus.dwLength = sizeof (MyMemoryStatus);
  GlobalMemoryStatus (&MyMemoryStatus);
  LE_SPEED_RealRAM = MyMemoryStatus.dwTotalPhys;

  /* Measure the data cache size.  First find out how many iterations over
     a 1K data area it takes to delay for 9 ticks (usually 18 ticks per
     second), which we call 500 milliseconds.  A PentiumII 200Mhz uses
     33 million. */

  Iterations = 1;
  ModuloLimit = 1024 / sizeof (DWORD);
  while (TRUE)
  {
    ElapsedTime = GetTickCount ();
    for (i = 0, index = 0; i < Iterations; i++)
    {
      BigArray [index]; /* Reference the byte from the array. */
      if (++index >= ModuloLimit)
        index = 0;
    }
    ElapsedTime = GetTickCount () - ElapsedTime;
    if (ElapsedTime >= 500)
      break; /* Ok, can stop now that we take longer than 100ms. */
    else
      Iterations *= 2; /* Double the number of things we count. */
  }

  /* Ok, now measure the amount of time for Iterations over 6K (the older
     processors (Pentium) have an 8K cache, so we test up to 6K so that it
     will definitely fit in the cache).  But first wait until the next
     tick happens. */

  ElapsedTime = GetTickCount ();
  while (ElapsedTime == GetTickCount ())
    ; /* Do nothing until start of next tick. */

  ModuloLimit = (1024 * 6) / sizeof (DWORD);
  ElapsedTime = GetTickCount ();
  for (i = 0, index = 0; i < Iterations; i++)
  {
    BigArray [index]; /* Reference the byte from the array. */
    if (++index >= ModuloLimit)
      index = 0;
  }
  ElapsedSmall = GetTickCount () - ElapsedTime;

  /* Calculate the speed measurement for the small cache test setting - so
     that it is representative of all computers running at full speed. */

  LE_SPEED_IterationsPerMs = Iterations / ElapsedSmall;

#if 0

  /* Measure the cache time for 14K of stuff.  Should be much slower on
     processors with only 8K of cache. */

  ElapsedTime = GetTickCount ();
  while (ElapsedTime == GetTickCount ())
    ; /* Do nothing until start of next tick. */

  ModuloLimit = (1024 * 14) / sizeof (DWORD);
  ElapsedTime = GetTickCount ();
  for (i = 0, index = 0; i < Iterations; i++)
  {
    BigArray [index]; /* Reference the byte from the array. */
    if (++index >= ModuloLimit)
      index = 0;
  }
  ElapsedLarge = GetTickCount () - ElapsedTime;

  /* If the 6K time is smaller than the 14K time then the system
     probably has a small cache.  On a Pentium II or Pro the times
     are almost exactly the same (small is a bit bigger due to
     extra modularity counter activity). */

  LE_SPEED_16KDataCache = ((ElapsedSmall * 65 / 64) >= ElapsedLarge);

#endif


  /* See how long it takes to load up a big image, 800x600x24. */

  bLoadingFlag = FALSE;

  if( nDataID != LE_DATA_EmptyItem )
  {
    ElapsedTime = GetTickCount ();
    LE_DATA_Use (nDataID);
    ElapsedTime = GetTickCount () - ElapsedTime;
    BytesLoaded = LE_DATA_GetCurrentSize (nDataID);
    LE_DATA_Unload (nDataID);

    if( ElapsedTime != 0 )
    {
      LE_SPEED_CDROM = BytesLoaded * 1000UL / ElapsedTime;
      bLoadingFlag = TRUE;
    }
  }

  // empty data item or too small image
  if( bLoadingFlag == FALSE )
    LE_SPEED_CDROM = 0;


  /* How much swap space is left */
  {
    MEMORYSTATUS myMemory;
    myMemory.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus( &myMemory );
    LE_SPEED_MaxVirtualMemory = (DWORD)(myMemory.dwTotalPageFile / 1024 / 1024 );
  }



  /* Display the results. */

#if LE_SPEED_SHOWMESSAGE
  if( bLoadingFlag == TRUE )
    sprintf(LE_ERROR_DebugMessageBuffer, "LE_SPEED_IterationsPerMs = %d\n"
      "LE_SPEED_CDROM = %d\n"
      "LE_SPEED_MaxVirtualMemory = %d MB.", LE_SPEED_IterationsPerMs, LE_SPEED_CDROM, LE_SPEED_MaxVirtualMemory);
  else if( nDataID == LE_DATA_EmptyItem )
    sprintf(LE_ERROR_DebugMessageBuffer, "LE_SPEED_IterationsPerMs = %d\n"
      "No loading speed, as nDataID == LE_DATA_EmptyItem\n"
      "LE_SPEED_MaxVirtualMemory = %d MB.", LE_SPEED_IterationsPerMs, LE_SPEED_MaxVirtualMemory);
  else if( nDataID != LE_DATA_EmptyItem && bLoadingFlag == FALSE )
    sprintf(LE_ERROR_DebugMessageBuffer, "LE_SPEED_IterationsPerMs = %d\n"
      "No loading speed, as the image loaded in is too small\n"
      "LE_SPEED_MaxVirtualMemory = %d MB.", LE_SPEED_IterationsPerMs, LE_SPEED_MaxVirtualMemory);

  #if CE_ARTLIB_EnableMultitasking
    strcat (LE_ERROR_DebugMessageBuffer,
      "\n\nWarning: speeds will be lower if you call the speed measurement function "
      "from a lower priority game thread.");
  #endif

  MessageBox (NULL, LE_ERROR_DebugMessageBuffer, "CPU Speed Test", MB_SYSTEMMODAL | MB_OK);
#endif
}

#endif // CE_ARTLIB_EnableSystemSpeed
