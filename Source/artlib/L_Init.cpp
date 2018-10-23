/*****************************************************************************
 *
 * FILE:        L_Init.cpp
 *
 * DESCRIPTION: Central location for generic initialization
 *              and cleanup functions.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Init.cpp 14    99/08/30 11:20a Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Init.cpp $
 * 
 * 14    99/08/30 11:20a Agmsmith
 * Added COM init call.
 *
 * 13    7/05/99 11:59a Mikeh
 * Added the define CE_ARTLIB_InitShutdownMessageBoxDisplay
 * (found in c_artlib.h) which enables/disables the display
 * of the shutdown message dialog box (debug mode only).
 *
 * 12    5/08/99 12:27p Agmsmith
 * Can now explicitly ask for full screen or windowed, or ask for the
 * default based on the .INI file.
 *
 * 11    5/03/99 3:13p Agmsmith
 * Init MMX before speed module.
 *
 * 10    1/17/99 2:38p Agmsmith
 * Adding DirIni module: directory paths and .INI files.
 *
 * 9     12/30/98 1:03p Agmsmith
 * Don't run in windowed mode.
 *
 * 8     12/30/98 11:11a Agmsmith
 * Now does source and destination rectangle setup better, and makes the
 * working surface the requested size even though the real screen may be
 * different.
 *
 * 7     12/18/98 5:41p Agmsmith
 * Need a window when initialising graphics now.
 *
 * 6     9/23/98 12:25p Agmsmith
 * Extra warning if compiled in debug mode.
 *
 * 5     9/17/98 4:19p Agmsmith
 *
 * 4     9/17/98 12:54p Agmsmith
 * Moved data loader preload function for bitmaps into graphics module.
 *
 * 3     9/16/98 4:59p Agmsmith
 * Now compiles under C++.
 ****************************************************************************/

#include "l_inc.h"

static BOOL LI_INIT_NeedToUninitCOM;



/*****************************************************************************
 * Central system initializion procedure.  Uses the status of the various
 * CE_ARTLIB_EnableSystem?? flags to initialize the various sub systems.
 */

void LE_INIT_SystemInit (void)
{
  HRESULT ErrorCode;

  // Initialise the COM (Component Object Model) library,
  // needed by DirectX code in graphics and sound and network modules.

  ErrorCode = CoInitialize (NULL);
  LI_INIT_NeedToUninitCOM = SUCCEEDED (ErrorCode);

#if CE_ARTLIB_EnableDebugMode
  LI_ERROR_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemMMX
  LI_MMX_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemMemory
  LI_MEMORY_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemUIMsg
  LE_UIMSG_InitializeSystem ();
#endif

#if CE_ARTLIB_EnableSystemTimers
  LI_TIMERS_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemChunk
  LI_CHUNK_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemMatrix
  LI_MATRIX_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemData
  LI_DATA_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemDirIni
  LI_DIRINI_InitSystem (CE_ARTLIB_DirIniINIFileName, CE_ARTLIB_DirIniCDROMVolumeName);
#endif

#if CE_ARTLIB_EnableSystemState
  LI_STATE_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemSpeed
  LE_SPEED_CalculateSpeed (LE_DATA_EmptyItem);
#endif

#if CE_ARTLIB_EnableSystemGrafix
  LE_GRAFIX_InitSystem (CE_ARTLIB_InitialScreenWidth, CE_ARTLIB_InitialScreenHeight,
    TRUE /* AcceptLargerSizes */, FALSE /* AcceptSmallerSizes */,
    (LE_GRAFIX_DepthInBytesValue) (CE_ARTLIB_InitialScreenDepth / 8) /* Desired depth in bytes */,
    CE_ARTLIB_InitialAcceptableDepths /* Set of allowed depths */,
    TRUE /* HaltOnError */,
    WINDOWED_USE_DEFAULT /* Read windowed/full screen from .INI file */);
#endif

#if CE_ARTLIB_EnableSystemSound
  LI_SOUND_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemAnim
  LI_ANIM_InitSystem ();
#endif

  // Renderer comes before sequencer since it is used by the sequencer
  // and shutting it down first would cause problems.

#if CE_ARTLIB_EnableSystemRend0D
  LI_REND0D_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemRend2D
  LI_REND2D_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemSequencer
  LI_SEQNCR_InitSystem ();
#endif

  // CE_ARTLIB_EnableSystemGrafix3D must be set so LI_REND3D_InitSystem() is initialised
  // to be able to use 3d sequencer

#if CE_ARTLIB_EnableSystemGrafix3D
  LI_REND3D_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystem3DSequencer
  LI_ANIM3D_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemButton
  LI_BUTTON_InitializeSystem ();
#endif

#if CE_ARTLIB_EnableSystemKeybrd
  LI_KEYBRD_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemMouse
  LI_MOUSE_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemJoypad
  LI_JOYPAD_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemFonts
  LI_FONTS_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemCcont
  LI_CCONT_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemMidi
  LI_MIDI_InitSystem ();
#endif

#if CE_ARTLIB_EnableSystemPrint
  LI_PRINT_InitSystem ();
#endif

#if CE_ARTLIB_EnableDebugMode
  wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
  wsprintf(LE_ERROR_DebugMessageBuffer,"LE_INIT_SystemInit() finished.\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
  wsprintf(LE_ERROR_DebugMessageBuffer,"Debug version.\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
  wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n\r\n");
  LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
#endif
}



/*****************************************************************************
 * Central system cleanup and removal procedure.  Uses the status of the
 * various CE_ARTLIB_EnableSystem?? flags to cleanup and remove the various
 * sub systems.
 */

void LE_INIT_SystemRemoval (void)
{
#if CE_ARTLIB_EnableSystemPrint
  LI_PRINT_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemMidi
  LI_MIDI_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemButton
  LI_BUTTON_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemCcont
  LI_CCONT_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemFonts
  LI_FONTS_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemJoypad
  LI_JOYPAD_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemMouse
  LI_MOUSE_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemKeybrd
  LI_KEYBRD_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemSequencer
  LI_SEQNCR_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemRend2D
  LI_REND2D_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemRend0D
  LI_REND0D_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemAnim
  LI_ANIM_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystem3DSequencer
  LI_ANIM3D_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemGrafix3D
  LI_REND3D_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemGrafix
  LI_GRAFIX_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemSound
  LI_SOUND_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemState
  LI_STATE_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemDirIni
  LI_DIRINI_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemData
  LI_DATA_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemMatrix
  LI_MATRIX_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemChunk
  LI_CHUNK_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemTimers
  LI_TIMERS_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemUIMsg
  LE_UIMSG_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableSystemMemory
  LI_MEMORY_RemoveSystem ();
#endif

#if CE_ARTLIB_EnableDebugMode
  LI_ERROR_RemoveSystem ();
#endif

  if (LI_INIT_NeedToUninitCOM)
  {
    CoUninitialize ();
    LI_INIT_NeedToUninitCOM = FALSE;
  }

#if CE_ARTLIB_EnableDebugMode || defined (_DEBUG)
#if CE_ARTLIB_InitShutdownMessageBoxDisplay
  MessageBox (0,
    "This is a debug version, not a final release!\r\n"
    "That means game performance won't be as good as in the release version.\r\n\r\n"
    "Artech Library has finished cleaning up.\r\n"
    "Game compiled at " __TIME__ " on " __DATE__ ".",
    LE_MAIN_ApplicationNameString,
    MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
#endif
#endif
}
