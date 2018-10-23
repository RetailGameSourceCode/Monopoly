#ifndef __L_INC_H__
#define __L_INC_H__

/*****************************************************************************
 *
 * FILE:        L_Inc.h
 * DESCRIPTION: Central include file for library code, OS APIs and C_MAIN.h.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Inc.h 15    5/08/99 5:19p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Inc.h $
 * 
 * 15    5/08/99 5:19p Agmsmith
 * Changes for search paths in data file opening.
 * 
 * 14    4/22/99 15:00 Davew
 * Added in the new L_DXTypes.h include for DirectX type definitions
 * 
 * 13    4/09/99 2:25p Agmsmith
 * Need multithreading for video even if game isn't multithreaded.
 * 
 * 12    3/19/99 11:14a Agmsmith
 * 
 * 11    1/31/99 3:24p Agmsmith
 * Video needs to be before render and sequencer.
 * 
 * 10    1/17/99 2:38p Agmsmith
 * Adding DirIni module: directory paths and .INI files.
 *
 * 9     1/16/99 4:39p Agmsmith
 * Changing blitter routines to also use old routines if desired.
 *
 * 8     11/26/98 5:05p Agmsmith
 * Moved DD init stuff from L_DDInit.h to PC3D.h.
 *
 * 7     10/06/98 1:58p Agmsmith
 * Added sprites.
 *
 * 6     9/22/98 4:23p Agmsmith
 * Now includes L_REND.h
 *
 * 5     9/17/98 12:54p Agmsmith
 * Moved data loader preload function for bitmaps into graphics module.
 *
 * 4     9/16/98 11:05a Agmsmith
 * Need to include main entry point header!
 *
 * 3     9/15/98 10:44a Agmsmith
 * Minor changes.
 *
 * 2     9/14/98 4:49p Agmsmith
 * Removed unused includes, added comments explaining cryptic ones.
 ****************************************************************************/



/*****************************************************************************
 * The user's configuration file, which has flags to turn on and off various
 * things.  In future this may be broken up into several smaller files to
 * avoid recompiling everything when something changes.  Note that the
 * makefile has the project directory in the include path so that the
 * C_ArtLib.h specific to the game can be picked up even if compiling
 * library code in "../ArtLib".
 */

#include <c_artlib.h>



/*****************************************************************************
 * Windows Specific Include Files
 */

#ifdef _WIN32
  #define STRICT 1
  #include <windows.h>    /* Microsoft Windows standard stuff */
  #include <windowsx.h>   /* Yet more Windows stuff */
  #include <commctrl.h>   /* Windows common controls */
  #include <direct.h>     /* Directory handling */
  #include <mmsystem.h>   /* Multimedia Windows stuff - like MIDI */
  #include <vfw.h>        /* Video for Windows */

  #define D3D_OVERLOADS   /* For those C++ operators on vectors */
  #include <dsound.h>     /* DirectSound audio stuff */
  #include <dplay.h>      /* DirectPlay networking stuff */
  #include <d3d.h>        /* Direct3D for 3D graphics and 2D DirectDraw too */
#endif



/*****************************************************************************
 * Standard C library include files.
 */

#include <assert.h>
#include <math.h>
#include <memory.h>   // Needed for memcpy and relatives.
#include <process.h>  // Needed for BeginThread.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*****************************************************************************
 * Artech library include files, only ones enabled by the game programmer are
 * actually included (see C_ArtLib.h).  Include order is significant, so can't
 * be just alphabetical.
 */

#include "l_type.h"
#include "L_DXTypes.h"

#if CE_ARTLIB_EnableSystemPrint
#include "l_print.h"
#endif

#if CE_ARTLIB_EnableSystemMatrix
#include "l_matrix.h"
#endif

#if CE_ARTLIB_EnableSystemDirIni
#include "l_dirini.h"
#endif

#if CE_ARTLIB_EnableSystemData
#include "l_data.h"
#endif

#if CE_ARTLIB_EnableSystemMemory
#include "l_memory.h"
#endif

#if CE_ARTLIB_EnableSystemSpeed
#include "l_speed.h"
#endif

#if CE_ARTLIB_EnableSystemChunk
#include "l_chunk.h"
#endif

#if CE_ARTLIB_EnableSystemQueue
#include "l_queue.h"
#endif

#if CE_ARTLIB_EnableSystemUIMsg
#include "l_uimsg.h"
#endif

#if CE_ARTLIB_EnableSystemKeybrd
#include "l_keybrd.h"
#endif

#if CE_ARTLIB_EnableSystemMMX
#include "l_mmx.h"
#endif

#if CE_ARTLIB_EnableSystemMouse
#include "l_mouse.h"
#endif

#if CE_ARTLIB_EnableSystemMidi
#include "l_midi.h"
#endif

#if CE_ARTLIB_EnableSystemJoypad
#include "l_joypad.h"
#endif

#if CE_ARTLIB_EnableSystemSound
#include "l_sound.h"
#endif

#if CE_ARTLIB_EnableSystemGrafix3D || CE_ARTLIB_EnableSystemRend0D || CE_ARTLIB_EnableSystemRend2D
#include "l_rend.h"
#endif

#if CE_ARTLIB_EnableSystemVideo
#include "l_video.h"
#endif

#if CE_ARTLIB_EnableSystemSequencer
#include "l_seqncr.h"
#endif

#if CE_ARTLIB_EnableSystemRend0D
#include "l_rend0d.h"
#endif

#if CE_ARTLIB_EnableSystemRend2D
#include "l_rend2d.h"
#endif

#if CE_ARTLIB_EnableSystemGrafix3D
  #include "l_rend3d.h"
#endif

#if CE_ARTLIB_EnableSystemFonts
#include "l_fonts.h"
#endif

#if CE_ARTLIB_EnableSystemTimers
#include "l_timers.h"
#endif

#if CE_ARTLIB_EnableSystemGrafix
#include "l_grafix.h"
#include "l_blt.h"
#endif

#if CE_ARTLIB_EnableSystemGrafix2D && CE_ARTLIB_BlitUseOldRoutines
  #include "l_blt16.h"
  #include "l_blt24.h"
  #include "l_blt32.h"
#endif

#if CE_ARTLIB_EnableSystemSprite
#include "l_sprite.h"
#endif

#if CE_ARTLIB_EnableSystemButton
#include "l_button.h"
#endif

#include "l_error.h"

#include "l_init.h"

#include "l_main.h"

#endif // __L_INC_H__
